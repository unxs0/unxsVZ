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

static unsigned uTotalLines=0;
static unsigned uProcessed=0;
static unsigned uIgnored=0;

char *ParseTextAreaLines(char *cTextArea);//bulkop.c
void RIPEImport(void);

void CommitTransaction(void);

void htmlIPAuthReport(void);

int floorLog2(unsigned int n);


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
			if(!cMassList[0])
			{
				gcMessage="<blink>Error: </blink>You must enter data at the import panel";
				htmlIPAuth();
			}
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


void htmlIPAuthDetail(void)
{
	htmlHeader("DNS System","Header");
	htmlIPAuthPage("DNS System","IPAuthDetails.Body");
	htmlFooter("Footer");

}//void htmlIPAuthDetail(void)


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


unsigned CSVFileData(unsigned uClient,char *cName);
unsigned uDefaultClient=0;

void funcIPAuthReport(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cCompanyName[100]={""};
	unsigned uCompanyId=0;

	sprintf(gcQuery,"SELECT cBlock,uClient,cBlockAction,cOwnerAction FROM tTransaction "
			"WHERE (cBlockAction!='None' OR cOwnerAction!='None') ORDER BY uTransaction");
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);
	
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uCompanyId);
		if(uCompanyId!=uDefaultClient)
			CSVFileData(uCompanyId,cCompanyName);
		else
			sprintf(cCompanyName,"%s",ForeignKey("tClient","cLabel",uDefaultClient));

		fprintf(fp,"<tr><td align=center>%s</td><td align=center>%s</td>"
			"<td align=center>%s</td><td align=center>%s</td><td align=center>%s</td></tr>\n",
			field[0]
			,field[1]
			,cCompanyName
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
//#define uDefaultClient 2

void CreateTransactionTable();
unsigned uGetBlockStatus(char *cBlock,char *cCompany);
unsigned uGetOwnerStatus(char *cCompany);
unsigned uClientCSVCheck(unsigned uClient);
void AddToRejectsTable(char *cLine);


void CreateTransactionTable()
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tTransaction ( uTransaction INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, "
			"cBlock VARCHAR(100) NOT NULL DEFAULT '',unique (cBlock,uOwner), uOwner INT UNSIGNED NOT NULL DEFAULT 0,"
			"index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, "
			"uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, "
			"cBlockAction VARCHAR(100) NOT NULL DEFAULT '', cOwnerAction VARCHAR(100) NOT NULL DEFAULT '',"
			"uClient INT UNSIGNED NOT NULL DEFAULT 0, cCompany VARCHAR(255) NOT NULL DEFAULT '' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tIgnoredTransaction (uIgnoredTransaction INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, "
			"cLine VARCHAR(255) NOT NULL DEFAULT '')");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);

}//void CreateTransactionTable()


void RIPEImport(void)
{
	char cLine[512]={"ERROR"};
	unsigned uLineNumber=0;
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
	char cuDefaultClient[16]={""};
	char cCompany[100]={""};

	CreateTransactionTable();
	
	GetConfiguration("uDefaultClient",cuDefaultClient,1);
	sscanf(cuDefaultClient,"%u",&uDefaultClient);
	if(!uDefaultClient)
		htmlPlainTextError("Create a tConfiguration entry for uDefaultClient, it must contain the tClient.uClient value for your company");

	sprintf(gcQuery,"TRUNCATE tTransaction");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);
	sprintf(gcQuery,"TRUNCATE tIgnoredTransaction");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);
	
	while(1)
	{

		uLineNumber++;
		sprintf(cLine,"%.255s",ParseTextAreaLines(cMassList));
		//ParseTextAreaLines() required break;
		if(cLine[0]==0) break;
		//Comments ignore
		if(cLine[0]=='#') continue;
		if(cLine[0]==';') continue;
		uClient=0;
		uTotalLines++;
		if(strstr(cLine,"PKXG-CUST"))
		{
			uProcessed++;
			//80.253.98.0 - 80.253.98.255 256 20060404 PKXG-CUST-1234-01
			sscanf(cLine,"%s - %s %u %u PKXG-CUST-%u-%u",
				cIPBlockStart
				,cIPBlockEnd
				,&uSize
				,&uDate
				,&uClient
				,&uOther);
			if(!uClient)
			{
				uIgnored++;
				strcat(cLine," *** COMPANY NOT FOUND ***");
				AddToRejectsTable(cLine);
				continue;
			}
		}
		else if(strstr(cLine,"PKXG-MRP"))
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
		else if(strstr(cLine,"PKXG-INFRA"))
		{
			uProcessed++;
			char cUnused[100]={""};
			sscanf(cLine,"%s - %s %u %u PKXG-INFRA-%s",
				cIPBlockStart
				,cIPBlockEnd
				,&uSize
				,&uDate
				,cUnused);
			uClient=uDefaultClient;
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
			AddToRejectsTable(cLine);
			continue;
		}

		//Common processing
		if(!uClientCSVCheck(uClient))
		{
			//Add record to rejects table
			strcat(cLine," *** COMPANY NOT FOUND ***");
			AddToRejectsTable(cLine);
			uIgnored++;
			continue;
		}
		//uCidr=(unsigned)(32-log2(uSize));
		uCidr=(unsigned)(32-floorLog2(uSize));

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
		
		if(uClient!=uDefaultClient)
			CSVFileData(uClient,cCompany);
		else
			sprintf(cCompany,"%.99s",ForeignKey("tClient","cLabel",uClient));

		uBlockStatus=uGetBlockStatus(cIPBlock,cCompany);
		uOwnerStatus=uGetOwnerStatus(cCompany);

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

		//if(!strcmp(cOwnerAction,"None")&&!strcmp(cBlockAction,"None")) continue; //No record if nothing to do
		
		sprintf(gcQuery,"INSERT INTO tTransaction SET cBlock='%s',cBlockAction='%s',cOwnerAction='%s',"
				"uClient=%u,cCompany='%s',uCreatedBy=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				cIPBlock
				,cBlockAction
				,cOwnerAction
				,uClient
				,cCompany
				,guLoginClient
				,guOrg
				);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(gcQuery);
	}
	//Lock tTransaction for writing?

}//void RIPEImport(void)


void AddToRejectsTable(char *cLine)
{
	sprintf(gcQuery,"INSERT INTO tIgnoredTransaction SET cLine='%s'",cLine);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);

}//void AddToRejectsTable(char *cLine)


unsigned uGetBlockStatus(char *cBlock,char *cCompany)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uOwner=0;

	sprintf(gcQuery,"SELECT uBlock,uOwner FROM tBlock WHERE cLabel='%s'",cBlock);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);
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

		if(!strcmp(ForeignKey("tClient","cLabel",uOwner),cCompany))
			return(NA_BLOCK);
		else
			return(MOD_BLOCK);
	}

	return(0);

}//unsigned uGetBlockStatus(char *cBlock)


unsigned uGetOwnerStatus(char *cCompany)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cLabel='%s'",cCompany);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);
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


unsigned uClientCSVCheck(unsigned uClient)
{
	return(CSVFileData(uClient,NULL));

}

unsigned CSVFileData(unsigned uClient,char *cName)
{
	FILE *fp;
	char cCompanyCSVLocation[100]={"/usr/local/idns/csv/companycode.csv"};
	unsigned uFileClient=0;
	char cLabel[100]={""};

	if(uClient==uDefaultClient) return(1); //exception ;)

	//Open CSV file at fixed location
	fp=fopen(cCompanyCSVLocation,"r");
	if(fp==NULL)
		htmlPlainTextError("Could not open CSV file for companies");
	
	//Search for uClient at CSV file
	while(fgets(gcQuery,2048,fp)!=NULL)
	{
		sscanf(gcQuery,"%u,%s",&uFileClient,cLabel);
		if(cName!=NULL) sprintf(cName,"%.65s",cLabel);
		if(uClient==uFileClient)
		{
			fclose(fp);
			return(1);
		}
	}
	fclose(fp);
	return(0);

}//unsigned uClientCSVCheck(unsigned uClient)

//
//End data processing functions
//

//
//Begin data commit functions
//

void funcReportActions(FILE *fp)
{
	MYSQL_RES *res;
	unsigned uWillDeleteCompanies=0;
	unsigned uWillDeleteBlocks=0;
	unsigned uWillCreateBlocks=0;
	unsigned uWillModBlocks=0;
	unsigned uWillCreateCompanies=0;
	unsigned uImportCompanies=0;

	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cLabel NOT IN "
		"(SELECT DISTINCT cCompany FROM tTransaction) AND "
		"uClient!=1 AND uClient!=%u AND cCode='Organization'",uDefaultClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);

	res=mysql_store_result(&gMysql);
	uWillDeleteCompanies=mysql_num_rows(res);
	
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT uBlock FROM tBlock WHERE uOwner IN (SELECT uClient FROM tClient WHERE cLabel NOT IN "
		"(SELECT DISTINCT cCompany FROM tTransaction) AND "
		"uClient!=1 AND uClient!=%u AND cCode='Organization')",uDefaultClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);

	res=mysql_store_result(&gMysql);
	uWillDeleteBlocks=mysql_num_rows(res);
	
	mysql_free_result(res);
	
	sprintf(gcQuery,"SELECT uTransaction FROM tTransaction WHERE cBlockAction='New'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);

	res=mysql_store_result(&gMysql);
	uWillCreateBlocks=mysql_num_rows(res);
	
	mysql_free_result(res);
	
	sprintf(gcQuery,"SELECT uTransaction FROM tTransaction WHERE cBlockAction='Modify'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);

	res=mysql_store_result(&gMysql);
	uWillModBlocks=mysql_num_rows(res);
	
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT uTransaction FROM tTransaction WHERE cOwnerAction='New'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);

	res=mysql_store_result(&gMysql);
	uWillCreateCompanies=mysql_num_rows(res);
	
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT DISTINCT cCompany FROM tTransaction");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);

	res=mysql_store_result(&gMysql);
	uImportCompanies=mysql_num_rows(res);
	
	mysql_free_result(res);

	fprintf(fp,"After import, %u companies and their contacts will be removed from the database.<br>\n",uWillDeleteCompanies);
	fprintf(fp,"These companies own %u blocks that will also be removed.<br>\n",uWillDeleteBlocks);
	fprintf(fp,"Also, %u companies and a default contact will be added to the database. %u blocks will be created "
			"and %u blocks ownership will be updated.<br>\n",
			uWillCreateCompanies
			,uWillCreateBlocks
			,uWillModBlocks
			);
	fprintf(fp,"%u companies were found in the import data.<br>\n",uImportCompanies);
	//fprintf(fp,"From a total of %u lines, %u lines were correctly processed and %u ignored.<br>\n",uTotalLines,uProcessed,uIgnored);

}//void funcReportActions(FILE *fp)


void funcRemovedCompanies(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uClient,cLabel FROM tClient WHERE cLabel NOT IN "
		"(SELECT DISTINCT cCompany FROM tTransaction) AND "
		"uClient!=1 AND uClient!=%u AND cCode='Organization' ORDER BY cLabel",uDefaultClient);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);

	res=mysql_store_result(&gMysql);
	if(!mysql_num_rows(res))
	{
		fprintf(fp,"<tr><td colspan=2>None</td>\n");
		mysql_free_result(res);
		return;
	}

	while((field=mysql_fetch_row(res)))
		fprintf(fp,"<tr><td>%s</td><td>%s</td>\n",field[0],field[1]);
	
	mysql_free_result(res);

}//void funcRemovedCompanies(FILE *fp)


void funcRemovedBlocks(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tBlock.cLabel,tClient.cLabel " 
		"FROM tBlock,tClient WHERE tBlock.uOwner IN (SELECT uClient FROM tClient WHERE cLabel NOT IN "
		"(SELECT DISTINCT cCompany FROM tTransaction) AND "
		"uClient!=1 AND uClient!=%u AND cCode='Organization') AND tClient.uClient=tBlock.uOwner ORDER BY tClient.cLabel",uDefaultClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);

	res=mysql_store_result(&gMysql);
	if(!mysql_num_rows(res))
	{
		fprintf(fp,"<tr><td colspan=2>None</td>\n");
		mysql_free_result(res);
		return;
	}
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"<tr><td>%s</td><td>%s</td>\n",field[0],field[1]);

	mysql_free_result(res);

}//void funcRemovedBlocks(FILE *fp)


void funcIgnoredLines(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char *cColor="";

	sprintf(gcQuery,"SELECT cLine FROM tIgnoredTransaction");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(gcQuery);

	res=mysql_store_result(&gMysql);
	if(!mysql_num_rows(res))
	{
		fprintf(fp,"None\n");
		mysql_free_result(res);
		return;
	}
	while((field=mysql_fetch_row(res)))
	{
		if(strstr(field[0],"COMPANY NOT FOUND")) 
			cColor="color=red";
		else
			cColor="";

		fprintf(fp,"<font %s>%s</font><br>\n",cColor,field[0]);
	}
	
	mysql_free_result(res);

}//void funcIgnoredLines(FILE *fp)


void CommitTransaction(void)
{
	//Submit meta job entry for commiting transaction
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cFQDN,tServer.cLabel,tNSType.cLabel FROM tNS,tNSSet,tServer,tNSType "
			"WHERE (tNS.uNSType=1 OR tNS.uNSType=2) AND tNS.uServer=tServer.uServer "
			"AND tNS.uNSType=tNSType.uNSType "
			"AND tNS.uNSSet=tNSSet.uNSSet AND tNSSet.uNSSet=1 LIMIT 1");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"INSERT INTO tJob SET cJob='IPAuthCommit',uNSSet=1,cTargetServer='%s %s',"
				"uTime=UNIX_TIMESTAMP(NOW()),uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				field[0]
				,field[2]
				,guOrg
				,guLoginClient
				);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

	}
	else
		htmlPlainTextError("Couldn't find master or hidden master for uNSSet=1");

}//void CommitTransaction(void)

int floorLog2(unsigned int n)
{
	int pos = 0;
	if (n >= 1<<16) { n >>= 16; pos += 16; }
	if (n >= 1<< 8) { n >>=  8; pos +=  8; }
	if (n >= 1<< 4) { n >>=  4; pos +=  4; }
	if (n >= 1<< 2) { n >>=  2; pos +=  2; }
	if (n >= 1<< 1) {           pos +=  1; }
	return ((n == 0) ? (-1) : pos);
}

