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
		
		sprintf(gcQuery,"DELETE FROM tTransaction WHERE cBlock='%s'",cIPBlock);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

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

void CleanUPBlock(char *cIPBlock);
void CleanUPCompanies(void);
char *cGetRandomPassword(void);


void ProcessTransaction(char *cIPBlock,unsigned uClient,char *cAction)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	register unsigned f;
	unsigned a,b,c,d,e;
	unsigned uNumIPs=0;
	unsigned uZone=0;

	char cZone[100]={""};
	char cSerial[100]={""};
	char cUpdateHost[100]={"packetexchange.net"}; //This will come from tConfiguration, later

	sscanf(cIPBlock,"%u.%u.%u.%u/%u",&a,&b,&c,&d,&e);
	uNumIPs=uGetNumIPs(cIPBlock);
	sprintf(cZone,"%u.%u.%u.in-addr.arpa",c,b,a);

	if(strcmp(cAction,"New"))
	{
		//
		//Create tBlock entry owned by uClient
		sprintf(gcQuery,"INSERT INTO tBlock SET cLabel='%s',uOwner=%u,"
				"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				cIPBlock
				,uClient
				,guLoginClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		//Check for .arpa zone if it doesn't exist, create it
		//owned by DEFAULT_CLIENT
		sprintf(gcQuery,"SELECT uZone FROM tZone WHERE cZone='%s' AND uView=2",cZone);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if(!mysql_num_rows(res))
		{
CreateZone:		
			sprintf(gcQuery,"INSERT INTO tZone SET cZone='%s',uNSSet=1,cHostmaster='%s',"
				"uSerial='%s',uExpire=604800,uRefresh=28800,uTTL=86400,"
				"uRetry=7200,uZoneTTL=86400,uMailServers=0,uView=2,uOwner=%u,"
				"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				cZone
				,HOSTMASTER
				,cSerial
				,DEFAULT_CLIENT
				,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			uZone=mysql_insert_id(&gMysql);
		}
		else
		{
			field=mysql_fetch_row(res);
			sscanf(field[0],"%u",&uZone);
		}
		mysql_free_result(res);

		//Submit new job

		//Create block default RRs uOwner=uClient
		//
		for(f=d;f<uNumIPs;f++)
		{
			sprintf(gcQuery,"INSERT INTO tResource SET "
					"cName=%u,uRRType=7,cParam1='%u-%u-%u-%u.%s',uZone=%u,"
					"uCreatedBy=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
					,
					f
					,f
					,c
					,b
					,a
					,cUpdateHost
					,uZone
					,guLoginClient
					,uClient
					);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		}
		//Update zone serial
		//Submit mod job
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

		sprintf(gcQuery,"SELECT uZone FROM tZone WHERE cZone='%s' AND uView=2",cZone);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uZone);
		}
		else
			goto CreateZone;

		//Update zone RRs
		//to be owned by uClient
		//
		for(f=d;f<uNumIPs;f++)
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
		//Submit Mod job for zone
	}

}//void ProcessTransaction(char *cIPBlock,unsigned uClient,char *cAction)


void ProcessCompanyTransaction(unsigned uClient,char *cAction)
{
	FILE *fp;
	char cCompanyCSVLocation[100]={"/usr/local/idns/csv/companycode.csv"};
	unsigned uFileClient=0;
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
		if(uClient==uFileClient)
		{
			unsigned uContact=0;

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
		}

	}

	//Create tClient record
	//Create default contact with same cLabel
	//Password should be 8 characters random text

}//void ProcessCompanyTransaction(unsigned uClient)


char *cGetRandomPassword(void)
{
	static char cPasswd[10]={""};

	unsigned uLength=8;
	unsigned i,f;

	srand((unsigned int)time(0));
	for(i=0;i<uLength;i++)
	{
		f=rand()%93+33;
		cPasswd[i]=f;
	}
	cPasswd[9]=0;
	return(cPasswd);

}//char *cGetRandomPassword(void)


void CleanUPCompanies(void)
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
		"uClient!=1 AND uClient!=%u",DEFAULT_CLIENT);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		//Delete forward zones and their RRs
		//The query below ensures that the reverse
		//zones RRs are not touched, those will be handled
		//by the CleanUPBlock() function call below
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
			CleanUPBlock(field2[0]);

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
	}
	
}//void CleanUPCompanies(void)


void CleanUPBlock(char *cIPBlock)
{
	unsigned uNumIPs=0;
	unsigned a,b,c,d,e,f;
	char cZone[100]={""};
	char cUpdateHost[100]={"packetexchange.net"}; //This will come from tConfiguration, later

	//This initial version is for /24 and smaller blocks only!!
	
	uNumIPs=uGetNumIPs(cIPBlock);

	sscanf(cIPBlock,"%u.%u.%u.%u/%u",&a,&b,&c,&d,&e);
	sprintf(cZone,"%u.%u.%u.in-adrr.arpa",c,b,a);

	//Update RRs
	for(f=d;f<uNumIPs;f++)
	{
		sprintf(gcQuery,"UPDATE tResource SET cParam1='%u-%u-%u-%u.%s' WHERE cName='%u' "
				"AND uZone IN (SELECT uZone FROM tZone WHERE cZone='%s')",
				c
				,b
				,a
				,f
				,cUpdateHost
				,f
				,cZone
				);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	//Update zone serial
	//Submit Mod job for zone
	
	sprintf(gcQuery,"DELETE FROM tBlock WHERE cLabel='%s'",cIPBlock);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void CleanUPBlock(char *cIPBlock)


