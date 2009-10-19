/*
FILE 
	bulkop.c
	$Id: bulkop.c 660 2008-12-18 18:48:48Z hus $
AUTHOR
	(C) 2006-2009 Gary Wallis  and Hugo Urquiza for Unixservice
PURPOSE
	iDNS Administration (ASP) Interface
	program file.
*/
#include "interface.h"
#include <openisp/ucidr.h>

static char *cMassList={""};
static char cImportMsg[32762]={""}; //A 32k buffer will be enough, if not, truncate the data.
static unsigned uFormat=0;

char *ParseTextAreaLines(char *cTextArea);//bulkop.c
void RIPEImport(void);

void CommitTransaction(void);

void htmlIPAuthReport(void);

void ProcessIPAuthVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cMassList"))
			cMassList=entries[i].val;
		else if(!strcmp(entries[i].name,"uFormat"))
			sscanf(entries[i].val,"%u",&uFormat);
	}

}//void ProcessIPAuthVars(pentry entries[], int x)


void IPAuthGetHook(entry gentries[],int x)
{
/*	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"cZone"))
			sprintf(gcZone,"%.99s",gentries[i].val);
		else if(!strcmp(gentries[i].name,"uView"))
			sprintf(cuView,"%.15s",gentries[i].val);
	}
*/	
	htmlIPAuth();

}//void IPAuthGetHook(entry gentries[],int x)


void IPAuthCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"IPAuth"))
	{
		ProcessIPAuthVars(entries,x);
		if(!strcmp(gcFunction,"IP Auth Import"))
		{
			switch(uFormat)
			{
				case 1:
					RIPEImport();
					htmlIPAuthReport();
					break;
				default:
					gcMessage="<blink>Error:</blink> I don't know how to handle that format";
			}
		}
		else if(!strcmp(gcFunction,"Commit IP Auth Import"))
		{
			CommitTransaction();
			htmlIPAuthReport();
		}
		htmlIPAuth();
	}

}//void IPAuthCommands(pentry entries[], int x)


void htmlIPAuth(void)
{
	htmlHeader("DNS System","Header");
	htmlIPAuthPage("DNS System","IPAuth.Body");
	htmlFooter("Footer");

}//void htmlIPAuth(void)


void htmlIPAuthReport(void)
{
	htmlHeader("DNS System","Header");
	htmlIPAuthPage("DNS System","IPAuthReport.Body");
	htmlFooter("Footer");

}//void htmlIPAuthReport(void)


void htmlIPAuthPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect(cTemplateName);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			char cuResource[16]={""};

			sprintf(cuResource,"%u",uResource);

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="idnsAdmin.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcUser;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcModStep";
			template.cpValue[7]=gcModStep;

			template.cpName[8]="cZone";
			template.cpValue[8]=gcZone;

			template.cpName[9]="gcMessage";
			template.cpValue[9]=gcMessage;

			template.cpName[10]="cuView";
			template.cpValue[10]=cuView;

			template.cpName[11]="cImportMsg";
			template.cpValue[11]=cImportMsg;

			template.cpName[12]="cCustomer";
			template.cpValue[12]=gcCustomer;

			template.cpName[13]="uResource";
			template.cpValue[13]=cuResource;

			template.cpName[14]="";

			printf("\n<!-- Start htmlIPAuthPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlIPAuthPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlIPAuthPage()


void funcIPAuthReport(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cBlock,uClient,cBlockAction,cOwnerAction FROM tTransaction ORDER BY uTransaction");
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<tr><td align=center>%s</td><td align=center>%s</td><td align=center>%s</td><td align=center>%s</td></tr>\n",
			field[0]
			,field[1]
			,field[2]
			,field[3]
			);
	}

}//void funcIPAuthReport(FILE *fp)


//Import functions code begins here
//Will extend in the future for ARIN and LACNIC
//
#define NEW_BLOCK 1
#define MOD_BLOCK 2
#define NA_BLOCK 3
#define DEFAULT_CLIENT 2

void CreateTransactionTable();
unsigned uGetBlockStatus(char *cBlock,unsigned uClient);
unsigned uGetOwnerStatus(unsigned uClient);


void CreateTransactionTable()
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tTransaction ( uTransaction INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, "
			"cBlock VARCHAR(100) NOT NULL DEFAULT '',unique (cBlock,uOwner), uOwner INT UNSIGNED NOT NULL DEFAULT 0,"
			"index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, "
			"uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, "
			"cBlockAction VARCHAR(100) NOT NULL DEFAULT '', cOwnerAction VARCHAR(100) NOT NULL DEFAULT '',"
			"uClient INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void CreateTransactionTable()


void RIPEImport(void)
{
	char cLine[512]={"ERROR"};
	unsigned uLineNumber=0;
	unsigned uProcessed=0;
	unsigned uIgnored=0;
	char cIPBlock[100]={""};
	char cIPBlockStart[64]={""};
	char cIPBlockEnd[64]={""};
	unsigned uCidr=0;
	unsigned uSize=0;
	unsigned uClient=0;
	unsigned uOther=0;
	unsigned uDate=0;
	unsigned uBlockStatus=0;
	unsigned uOwnerStatus=0;
	char *cBlockAction="";
	char *cOwnerAction="";

	CreateTransactionTable();

	sprintf(gcQuery,"TRUNCATE tTransaction");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	while(1)
	{

		uLineNumber++;
		sprintf(cLine,"%.255s",ParseTextAreaLines(cMassList));
		//ParseTextAreaLines() required break;
		if(cLine[0]==0) break;
		//Comments ignore
		if(cLine[0]=='#') continue;
		if(cLine[0]==';') continue;
		if(strstr(cLine,"CUST"))
		{
			//80.253.98.0 - 80.253.98.255 256 20060404 PKXG-CUST-1234-01
			uProcessed++;
			sscanf(cLine,"%s - %s %u %u PKXG-CUST-%u-%u",
				cIPBlockStart
				,cIPBlockEnd
				,&uSize
				,&uDate
				,&uClient
				,&uOther);
		}
		else if(strstr(cLine,"MRP"))
		{
			uProcessed++;
			//89.167.255.0 - 89.167.255.255 256 20090805 PKXG-MRP-1234-01
			sscanf(cLine,"%s - %s %u %u PKXG-MRP-%u-%u",
				cIPBlockStart
				,cIPBlockEnd
				,&uSize
				,&uDate
				,&uClient
				,&uOther);

		}
		else if(strstr(cLine,"INFRA"))
		{
			char cUnused[100]={""};
			uProcessed++;
			sscanf(cLine,"%s - %s %u %u PKXG-INFRA-%s",
				cIPBlockStart
				,cIPBlockEnd
				,&uSize
				,&uDate
				,cUnused);
			uClient=DEFAULT_CLIENT;
		}
		else if(strstr(cLine,"-DELE"))
		{
			//Managed outside iDNS, skip
			uIgnored++;
			continue;
		}
		else if(1)
		{
			//Invalid, skip
			uIgnored++;
			continue;
		}

		//Common processing
		uCidr=(unsigned)(32-log2(uSize));

		/*printf("cIPBlockStart='%s' cIPBlockEnd='%s' uSize=%u uDate=%u uClient=%u uOther=%u uCidr=%u\n",
			cIPBlockStart
			,cIPBlockEnd
			,uSize
			,uDate
			,uClient
			,uOther
			,uCidr);
		*/
		sprintf(cIPBlock,"%s/%u",cIPBlockStart,uCidr);
		uBlockStatus=uGetBlockStatus(cIPBlock,uClient);
		uOwnerStatus=uGetOwnerStatus(uClient);

		switch(uBlockStatus)
		{
			case NEW_BLOCK:
				cBlockAction="New";
			break;

			case MOD_BLOCK:
				cBlockAction="Modify";
			break;

			case NA_BLOCK:
				cBlockAction="None";
			break;
		}
		switch(uOwnerStatus)
		{
			case NEW_BLOCK:
				cOwnerAction="New";
			break;

			case MOD_BLOCK:
				cOwnerAction="Modify";
			break;

			case NA_BLOCK:
				cOwnerAction="None";
			break;
		}
		//printf("IP Block Label=%s uClient=%u cBlockAction=%s cOwnerAction=%s\n"
		//	,cIPBlock,uClient,cBlockAction,cOwnerAction);

		if(!strcmp(cOwnerAction,"None")&&!strcmp(cBlockAction,"None")) continue; //No record if nothing to do
		
		sprintf(gcQuery,"INSERT INTO tTransaction SET cBlock='%s',cBlockAction='%s',cOwnerAction='%s',"
				"uClient=%u,uCreatedBy=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				cIPBlock
				,cBlockAction
				,cOwnerAction
				,uClient
				,guLoginClient
				,guOrg
				);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

	}
	//Lock tTransaction for writing?
	
}//void RIPEImport(void)


unsigned uGetBlockStatus(char *cBlock,unsigned uClient)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uOwner=0;

	sprintf(gcQuery,"SELECT uBlock,uOwner FROM tBlock WHERE cLabel='%s'",cBlock);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if(!mysql_num_rows(res))
	{
		mysql_free_result(res);
		return(NEW_BLOCK);
	}
	else
	{
		field=mysql_fetch_row(res);
		sscanf(field[1],"%u",&uOwner);

		mysql_free_result(res);

		if(uOwner==uClient)
			return(NA_BLOCK);
		else
			return(MOD_BLOCK);
	}

	return(0);

}//unsigned uGetBlockStatus(char *cBlock)


unsigned uGetOwnerStatus(unsigned uClient)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE uClient='%u'",uClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if(!mysql_num_rows(res))
	{
		mysql_free_result(res);
		return(NEW_BLOCK);
	}
	else
	{
		mysql_free_result(res);
		return(NA_BLOCK);
	}

	return(0);

}//unsigned uGetOwnerStatus(unsigned uClient)

//
//End data processing functions
//

//
//Begin data commit functions
//

void CleanUpBlock(char *cIPBlock);
void CleanUpCompanies(void);
void EncryptPasswd(char *cPasswd);
char *cGetRandomPassword(void);
void ProcessTransaction(char *cIPBlock,unsigned uClient,char *cAction);
void ProcessCompanyTransaction(unsigned uClient,char *cAction);
void UpdateSerialNum(char *cZone,char *cuView);
unsigned uCreateZone(char *cZone,unsigned uOwner);
void CreateDefaultRR(unsigned uName,char *cParam1,unsigned uZone,unsigned uOwner);
void ResetRR(char *cZone,unsigned uName,char *cParam1,unsigned uOwner);
MYSQL_RES *ZoneQuery(char *cZone);

//Action counters
unsigned uBlockAdd=0;
unsigned uBlockMod=0;
unsigned uBlockDel=0;
unsigned uCompanyAdd=0;
unsigned uCompanyDel=0;

void CommitTransaction(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uClient=0;
	char cMsg[100]={""};

	sprintf(gcQuery,"SELECT DISTINCT uClient FROM tTransaction WHERE cOwnerAction='New' ORDER BY uTransaction");
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{		
		sscanf(field[0],"%u",&uClient);
		ProcessCompanyTransaction(uClient,"New");
		uCompanyAdd++;
	}

	mysql_free_result(res);	

	sprintf(gcQuery,"SELECT cBlock,uClient,cBlockAction FROM tTransaction ORDER BY uTransaction");
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uClient);
		ProcessTransaction(field[0],uClient,field[2]);
	}
	
	CleanUpCompanies();

	sprintf(cImportMsg,"Added %u block(s)\n",uBlockAdd);

	sprintf(cMsg,"Modified %u block(s)\n",uBlockMod);
	strcat(cImportMsg,cMsg);

	sprintf(cMsg,"Deleted %u block(s)\n",uBlockDel);
	strcat(cImportMsg,cMsg);
	
	sprintf(cMsg,"Added %u companies\n",uCompanyAdd);
	strcat(cImportMsg,cMsg);
	
	sprintf(cMsg,"Deleted %u companies and their contacts\n",uCompanyDel);
	strcat(cImportMsg,cMsg);

}//void CommitTransaction(void)


unsigned uCreateZone(char *cZone,unsigned uOwner)
{
	//This function creates a new zone
	//sets it serial number
	//and submits a new job
	//
	unsigned uZone=0;
	time_t luClock;

	time(&luClock);

	sprintf(gcQuery,"INSERT INTO tZone SET cZone='%s',uNSSet=1,cHostmaster='%s',"
			"uSerial=0,uExpire=604800,uRefresh=28800,uTTL=86400,"
			"uRetry=7200,uZoneTTL=86400,uMailServers=0,uView=2,uOwner=%u,"
			"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			cZone
			,HOSTMASTER
			,uOwner
			,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uZone=mysql_insert_id(&gMysql);
	UpdateSerialNum(cZone,"2");
	//Submit new job
	if(AdminSubmitJob("New",1,cZone,0,luClock))
			htmlPlainTextError(mysql_error(&gMysql));
	
	return(uZone);

}//unsigned uCreateZone(char *cZone,unsigned uOwner)


void CreateDefaultRR(unsigned uName,char *cParam1,unsigned uZone,unsigned uOwner)
{
	sprintf(gcQuery,"INSERT INTO tResource SET "
			"cName=%u,uRRType=7,cParam1='%s',uZone=%u,"
			"uCreatedBy=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uName
			,cParam1
			,uZone
			,guLoginClient
			,uOwner
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void CreateDefaultRR(unsigned uName,char *cParam1,unsigned uZone,unsigned uOwner)


MYSQL_RES *ZoneQuery(char *cZone)
{
	static MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uZone FROM tZone WHERE cZone='%s' AND uView=2",cZone);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	return(res);

}//MYSQL_RES *ZoneQuery(void)


void ProcessTransaction(char *cIPBlock,unsigned uClient,char *cAction)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	register unsigned f;
	unsigned a,b,c,d,e;
	unsigned uNumNets=0;
	unsigned uNumIPs=0;
	unsigned uZone=0;

	char cZone[100]={""};
	char cParam1[200]={""};
	char cUpdateHost[100]={"packetexchange.net"}; //This will come from tConfiguration, later
	
	time_t luClock;
	
	sscanf(cIPBlock,"%u.%u.%u.%u/%u",&a,&b,&c,&d,&e);
	uNumIPs=uGetNumIPs(cIPBlock);
	uNumNets=uGetNumNets(cIPBlock);
	
	time(&luClock);
	//printf("uNumIPs=%u\n",uNumIPs);
	//printf("uNumNets=%u\n",uNumNets);

	if(!strcmp(cAction,"New"))
	{
		//
		//Create tBlock entry owned by uClient
		sprintf(gcQuery,"INSERT INTO tBlock SET cLabel='%s',uOwner=%u,"
				"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				cIPBlock
				,uClient
				,guLoginClient);
		printf("%s\n",gcQuery);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		uBlockAdd++;

		if(uNumNets==1)
		{
			//24 and smaller blocks
			sprintf(cZone,"%u.%u.%u.in-addr.arpa",c,b,a);
			//Check for .arpa zone if it doesn't exist, create it
			//owned by DEFAULT_CLIENT
			res=ZoneQuery(cZone);
			if(!mysql_num_rows(res))
CreateZone:			
				uZone=uCreateZone(cZone,DEFAULT_CLIENT);
			else
			{
				field=mysql_fetch_row(res);
				sscanf(field[0],"%u",&uZone);
			}
			mysql_free_result(res);
			
			//Create block default RRs uOwner=uClient
			//
			if(d==0)d++;
			for(f=d;f<(uNumIPs+1);f++)
			{
				sprintf(cParam1,"%u-%u-%u-%u.%s",f,c,b,a,cUpdateHost);
				CreateDefaultRR(f,cParam1,uZone,uClient);
			}
			//Update zone serial
			UpdateSerialNum(cZone,"2");
			//Submit mod job
			//Default uNSSet=1 ONLY
			if(AdminSubmitJob("Mod",1,cZone,0,luClock+300))
					htmlPlainTextError(mysql_error(&gMysql));
		}//if(uNumNets==1)
		else
		{
			register int x;
			//Larger than /24 blocks
CreateZoneLargeBlock:			
			for(x=c;x<(c+uNumNets);x++)
			{
				//
				sprintf(cZone,"%u.%u.%u.in-addr.arpa",x,b,a);
				//printf("cZone=%s\n",cZone);

				res=ZoneQuery(cZone);
				if(!mysql_num_rows(res))
				{
					uZone=uCreateZone(cZone,DEFAULT_CLIENT);
				}
				else
				{
					field=mysql_fetch_row(res);
					sscanf(field[0],"%u",&uZone);
				}
				mysql_free_result(res);
			
				//Create block default RRs uOwner=uClient
				//
				if(d==0)d++;
				for(f=d;f<255;f++)
				{
					sprintf(cParam1,"%u-%u-%u-%u.%s",f,c,b,a,cUpdateHost);
					CreateDefaultRR(f,cParam1,uZone,uClient);
				}
				//Update zone serial
				UpdateSerialNum(cZone,"2");
				//Submit mod job
				//Default uNSSet=1 ONLY
				if(AdminSubmitJob("Mod",1,cZone,0,luClock+300))
					htmlPlainTextError(mysql_error(&gMysql));
			}//for(f=c;f<((c+uNumNets));f++)
		}
	}
	else if(strcmp(cAction,"Modify"))
	{
		//
		//Update tBlock uOwner
		sprintf(gcQuery,"UPDATE tBlock SET uOwner=%u,uModBy=%u,"
				"uModDate=UNIX_TIMESTAMP(NOW()) WHERE cLabel='%s'",
				uClient
				,guLoginClient
				,cIPBlock);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		
		uBlockMod++;

		if(uNumNets==1)
		{
			//24 and smaller blocks
			sprintf(cZone,"%u.%u.%u.in-addr.arpa",c,b,a);
			//Check for .arpa zone if it doesn't exist, create it
			//owned by DEFAULT_CLIENT
			res=ZoneQuery(cZone);
			if(!mysql_num_rows(res))
				goto CreateZone;
			else
			{
				field=mysql_fetch_row(res);
				sscanf(field[0],"%u",&uZone);
			}

			//Update zone RRs
			//to be owned by uClient
			//
			if(d==0)d++;
			for(f=d;f<(uNumIPs+1);f++)
			{
				//Update to default cParam1 or just uOwner update?
				sprintf(gcQuery,"UPDATE tResource SET cParam1='%u-%u-%u-%u.%s',uOwner=%u WHERE cName='%u' "
						"AND uZone=%u",
						c
						,b
						,a
						,f
						,cUpdateHost
						,uClient
						,f
						,uZone
						);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
			}
			//Update zone serial
			UpdateSerialNum(cZone,"2");
			//Submit Mod job for zone
			if(AdminSubmitJob("Mod",1,cZone,0,luClock+300))
				htmlPlainTextError(mysql_error(&gMysql));
		}//if(uNumNets==1)
		else
		{
			register int x;
			//Larger than /24 blocks
			
			for(x=c;x<(c+uNumNets);x++)
			{
				//
				sprintf(cZone,"%u.%u.%u.in-addr.arpa",x,b,a);
				//printf("cZone=%s\n",cZone);

				res=ZoneQuery(cZone);
				if(!mysql_num_rows(res))
				{
					goto CreateZoneLargeBlock;
				}
				else
				{
					field=mysql_fetch_row(res);
					sscanf(field[0],"%u",&uZone);
				}
				mysql_free_result(res);
				for(f=d;f<254;f++)
				{
					sprintf(cParam1,"%u-%u-%u-%u.%s",f,x,b,a,cUpdateHost);
					ResetRR(cZone,f,cParam1,uClient);
				}
				//Update zone serial
				UpdateSerialNum(cZone,"2");
				//Submit Mod job for zone
				if(AdminSubmitJob("Mod",1,cZone,0,luClock+300))
					htmlPlainTextError(mysql_error(&gMysql));
			}
		}
	}

}//void ProcessTransaction(char *cIPBlock,unsigned uClient,char *cAction)


void ProcessCompanyTransaction(unsigned uClient,char *cAction)
{
	FILE *fp;
	char cCompanyCSVLocation[100]={"/usr/local/idns/csv/companycode.csv"};
	unsigned uFileClient=0;
	unsigned uMatch=0;
	char cLabel[100]={""};

	if(!strcmp(cAction,"None")) return;

	//Default 'New'
	
	//Open CSV file at fixed location
	fp=fopen(cCompanyCSVLocation,"r");
	if(fp==NULL)
		htmlPlainTextError("Could not open CSV file for companies");
	
	//Search for uClient at CSV file
	while(fgets(gcQuery,2048,fp)!=NULL)
	{
		sscanf(gcQuery,"%u,%s",&uFileClient,cLabel);
		if((uClient==uFileClient)&&!uMatch)
		{
			uMatch=1;
			unsigned uContact=0;
			char cPasswd[100]={""};
			char cSavePasswd[16]={""};
			//Create tClient record
			sprintf(gcQuery,"INSERT INTO tClient SET uClient=%u,cLabel='%s',"
					"cCode='Organization',uOwner=1,uCreatedBy=%u,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uClient
					,cLabel
					,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			//Create default contact with same cLabel
			sprintf(gcQuery,"INSERT INTO tClient SET uOwner=%u,cLabel='%s',"
					"cCode='Contact',uCreatedBy=%u,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uClient
					,cLabel
					,guLoginClient
					);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));

			uContact=mysql_insert_id(&gMysql);
			//Password should be 8 characters random text
			sprintf(cPasswd,"%s",cGetRandomPassword());
			sprintf(cSavePasswd,"%s",cPasswd);
			EncryptPasswd(cPasswd);
			sprintf(gcQuery,"INSERT INTO tAuthorize SET cLabel='%s',uCertClient=%u,"
					"uOwner=%u,cPasswd='%s',cClrPasswd='%s',cIpMask='0.0.0.0',"
					"uPerm=6,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					cLabel
					,uContact
					,uClient
					,cPasswd
					,cSavePasswd
					,guLoginClient
					);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		}
	}
	
	//Do something based on uMatch?

}//void ProcessCompanyTransaction(unsigned uClient)


char *cGetRandomPassword(void)
{
	static char cPasswd[10]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;

	mysql_query(&gMysql,"DROP function if exists generate_alpha");
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	mysql_query(&gMysql,"CREATE FUNCTION generate_alpha () RETURNS CHAR(1) "
			"RETURN ELT(FLOOR(1 + (RAND() * (50-1))), 'a','b','c','d'"
			",'e','f','g','h','i','j','k','l','m  ','n','o','p','q','r'"
			",'s','t','u','v','w','x','y',  'z','A','B','C','D','E','F',"
			"'G','H','I','J','K','L','M  ','N','O','P','Q','R','S','T','U'"
			",'V','W','X','Y',  'Z' )");
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	mysql_query(&gMysql,"SELECT CONCAT(generate_alpha (),generate_alpha (),generate_alpha (),"
			"generate_alpha (),generate_alpha (),generate_alpha (),generate_alpha (),"
			"generate_alpha ())");
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	field=mysql_fetch_row(res); //We will always have a row if the above queries didn't fail
	
	sprintf(cPasswd,"%s",field[0]);

	return(cPasswd);

}//char *cGetRandomPassword(void)


void CleanUpCompanies(void)
{
	//If a company doesn't exist in the tTransaction table:
	//* Company removed
	//* Associated contacts removed
	//* Associated tBlocks (IP Blocks) removed
	//* Associated Forward zones only removed (Reverse zones always left)
	//* Associated Resource Records removed. For reverse zones only ; the IP Address(es)
	// removed need to be replaced with standard reverse record(s) having the following
	// example format (hopefully the .packetexchange.net will not be hardcoded and just a
	//variable etc). Obviously this will differ for different IP Blocks!:
	//4            PTR 4-71-245-83.packetexchange.net.
	//5            PTR 5-71-245-83.packetexchange.net.
	//6            PTR 6-71-245-83.packetexchange.net.
	//7            PTR 7-71-245-83.packetexchange.net.
	MYSQL_RES *res;
	MYSQL_ROW field;

	MYSQL_RES *res2;
	MYSQL_ROW field2;

	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE uClient NOT IN "
		"(SELECT DISTINCT uClient FROM tTransaction) AND "
		"uClient!=1 AND uClient!=%u AND cCode='Organization'",DEFAULT_CLIENT);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		//Delete forward zones and their RRs
		//The query below ensures that the reverse
		//zones RRs are not touched, those will be handled
		//by the CleanUpBlock() function call below
		sprintf(gcQuery,"DELETE FROM tResource WHERE uZone IN "
				"(SELECT uZone FROM tZone WHERE uOwner=%s)",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		sprintf(gcQuery,"DELETE FROM tZone WHERE uOwner=%s",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		
		//Delete blocks
		sprintf(gcQuery,"SELECT cLabel FROM tBlock WHERE uOwner=%s",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res2=mysql_store_result(&gMysql);
		while((field2=mysql_fetch_row(res2)))
		{
			uBlockDel++;
			CleanUpBlock(field2[0]);
		}
		//Delete contacts
		sprintf(gcQuery,"DELETE FROM tAuthorize WHERE uCertClient IN "
				"(SELECT uClient FROM tClient WHERE uOwner=%s)",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		sprintf(gcQuery,"DELETE FROM tClient WHERE uOwner=%s",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		sprintf(gcQuery,"DELETE FROM tClient WHERE uClient=%s",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		uCompanyDel++;
	}
	
}//void CleanUpCompanies(void)


void CleanUpBlock(char *cIPBlock)
{
	unsigned uNumIPs=0;
	unsigned uNumNets=0;

	unsigned a,b,c,d,e,f;
	char cZone[100]={""};
	char cParam1[200]={""};
	char cUpdateHost[100]={"packetexchange.net"}; //This will come from tConfiguration, later
	time_t luClock;

	time(&luClock);
	
	uNumIPs=uGetNumIPs(cIPBlock);
	uNumNets=uGetNumNets(cIPBlock);

	sscanf(cIPBlock,"%u.%u.%u.%u/%u",&a,&b,&c,&d,&e);

	sprintf(gcQuery,"DELETE FROM tBlock WHERE cLabel='%s'",cIPBlock);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	if(uNumNets==1)
	{
		//Update RRs
		if(d==0)d++;
		sprintf(cZone,"%u.%u.%u.in-adrr.arpa",c,b,a);

		for(f=d;f<(uNumIPs+1);f++)
		{
			sprintf(cParam1,"%u-%u-%u-%u.%s",f,c,b,a,cUpdateHost);
			ResetRR(cZone,f,cParam1,DEFAULT_CLIENT);
		}
		//Update zone serial
		UpdateSerialNum(cZone,"2");
		//Submit Mod job for zone
		if(AdminSubmitJob("Mod",1,cZone,0,luClock+300))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	else
	{
		register int x;
		//Larger than /24 blocks
			
		for(x=c;x<(c+uNumNets);x++)
		{
			//
			sprintf(cZone,"%u.%u.%u.in-addr.arpa",x,b,a);
			//printf("cZone=%s\n",cZone);
			for(f=d;f<254;f++)
			{
				sprintf(cParam1,"%u-%u-%u-%u.%s",f,x,b,a,cUpdateHost);
				ResetRR(cZone,f,cParam1,DEFAULT_CLIENT);
			}
			//Update zone serial
			UpdateSerialNum(cZone,"2");
			//Submit Mod job for zone
			if(AdminSubmitJob("Mod",1,cZone,0,luClock+300))
				htmlPlainTextError(mysql_error(&gMysql));
		}
		

	}

}//void CleanUpBlock(char *cIPBlock)


void ResetRR(char *cZone,unsigned uName,char *cParam1,unsigned uOwner)
{
	sprintf(gcQuery,"UPDATE tResource SET cParam1='%s',uOwner=%u WHERE cName='%u' "
			"AND uZone IN (SELECT uZone FROM tZone WHERE cZone='%s')",
			cParam1
			,uOwner
			,uName
			,cZone
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void ResetRR(char *cZone,char *cParam1)
