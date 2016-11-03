/*
FILE
	svn ID removed
PURPOSE
	Command line interface, and main page.

AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza
	GPL License applies. Included in this distribution in file LICENSE.
NOTES
	Cleaned out customer specific import gcFunctions in CVS if needed later.
	Upgraded to use tConfiguration MySQLRootPwd.
	6/2004 ggw.
 
*/

#include "local.h"
#include "mysqlisp.h"

#define macro_mySQLQueryErrorText	mysql_query(&gMysql,gcQuery);\
					if(mysql_errno(&gMysql))\
					{\
						printf("%s\n",mysql_error(&gMysql));\
						printf("</td></tr>\n");\
						CloseFieldSet();\
						return;\
					}\
					mysqlRes=mysql_store_result(&gMysql);

	
static char cTableList[64][32]={"tAccountType", "tAuthorize", "tClient", "tClientConfig", "tConfig", "tConfiguration", "tGlossary", "tInstance", "tInvoice", "tInvoiceItems", "tInvoiceMonth", "tInvoiceStatus", "tJob", "tJobStatus", "tLog", "tLogMonth", "tLogType", "tMonth", "tMotd", "tPaid", "tParameter", "tParamType", "tPayment", "tPeriod", "tProduct", "tProductType", "tService", "tServiceGlue", "tServer", "tShipping", "tStatus", "tTemplate", "tTemplateType", "tTemplateSet",""};

static char cInitTableList[64][32]={"tAccountType", "tAuthorize", "tClient", "tClientConfig", "tConfig", "tConfiguration", "tGlossary", "tInstance", "tInvoiceStatus", "tJobStatus", "tLogType", "tMotd", "tParameter", "tParamType", "tPeriod", "tProduct", "tProductType", "tService", "tServiceGlue", "tServer", "tShipping", "tStatus", "tTemplate", "tTemplateType", "tTemplateSet",""};

void ExtMainShell(int argc, char *argv[]);
void Initialize(char *cPasswd);
void Backup(char *cPasswd);
void Restore(char *cTableName,char *cPasswd);
void RestoreAll(char *cPasswd);
void mySQLRootConnect(char *cPasswd);
void ImportTemplateFile(char *cTemplate, char *cFile, char *cTemplateSet);
void ExtracttLog(char *cMonth, char *cYear, char *cPasswd, char *cTablePath);
time_t cDateToUnixTime(char *cDate);
void CreatetLogTable(char *cTableName);
void NextMonthYear(char *cMonth,char *cYear,char *cNextMonth,char *cNextYear);
void ExtDashboardSelect(char *cTable,char *cVarList,char *cCondition, char *cOrderBy, unsigned uMaxResults);

//Invoicing functions
void GenerateInvoices(void);
unsigned uClientHasInvoice(char *cuClient);
unsigned uHasInvoiceItem(unsigned uInvoice,char *cuProduct);
unsigned uGetClientOwner(unsigned uClient);
void EmailAllInvoices(void);//tinvoicefunc.h

void CalledByAlias(int iArgc,char *cArgv[]);
void TextConnectDb(void);
void UpgradeSchema(char *cPasswd);
void ArchiveInvoice(char *cMonth, char *cYear, char *cTablePath,char *cPasswd);
char *cShortenText(char *cText,unsigned uNumWords);

//external
void CreatetInvoiceMonthTable(char *cTableName);//tinvoicemonthfunc.h
void CreatetInvoiceItemsMonthTable(char *cTableName);
void CreatetPaidMonthTable(char *cTableName);
time_t cDateToUnixTime(char *cDate);//tinstancefunc.h
void GetConfiguration(char const *cName, char *cValue, unsigned uHtml);

void EncodeMD5tClientConfig(void); //One time use only!
void InserttClientConfig(void); //One time use only!
void mysqlMailuClientUpdate(void); //One time use only!
void mysqlApache2uClientUpdate(void); //One time use only!
void mysqlRadius2uClientUpdate(void); //One time use only!
void iDNSuClientUpdate(void); //One time use only!


int iExtMainCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"MainTools"))
	{
		if(!strcmp(gcCommand,"Dashboard"))
			unxsISP("DashBoard");
	}

	return(0);
}

void ExtMainContent(void)
{
	printf("<p><input type=hidden name=gcFunction value=MainTools>");

	OpenFieldSet("System Information",100);

	OpenRow("Hostname","black");
	printf("<td>%s</td></tr>\n",gcHostname);

	OpenRow("Build Information","black");
	printf("<td>%s</td></tr>\n",gcBuildInfo);

	OpenRow("RAD Status","black");
	printf("<td>%s %s</td></tr>\n",gcRADStatus,REV);

	OpenRow("Application Summary","black");
	printf("<td>Manage ISP customers, resellers and their resources. Centralize resource and product usage.</td></tr>\n");

	if(guPermLevel>9)
	{
		register unsigned int i;
		OpenRow("Table List","black");
		printf("<td>\n");
		for(i=0;cTableList[i][0];i++)
			printf("<a href=unxsISP.cgi?gcFunction=%.32s>%.32s</a><br>\n",
				cTableList[i],cTableList[i]);
		printf("</td></tr>\n");
        	OpenRow("Admin Functions","black");
		printf("<td><input type=hidden name=gcFunction value=MainTools>\n");
		printf(" <input class=largeButton type=submit name=gcCommand value=Dashboard></td></tr>\n");
	}
	CloseFieldSet();

	
}//void ExtMainContent(void)


void ExtMainShell(int argc, char *argv[])
{
	uid_t uUID= -1;

	uUID=getuid();

	//You can eliminate this if in a very secure environment
	if(uUID)
	{
		printf("Only root can run this program\n");
		exit(1);
	}

	if(argc==3 && !strcmp(argv[1],"Initialize"))
                Initialize(argv[2]);
        else if(argc==3 && !strcmp(argv[1],"Backup"))
                Backup(argv[2]);
        else if(argc==4 && !strcmp(argv[1],"Restore"))
                Restore(argv[2],argv[3]);
        else if(argc==3 && !strcmp(argv[1],"RestoreAll"))
                RestoreAll(argv[2]);
        else if(argc==3 && !strcmp(argv[1],"UpgradeSchema"))
                UpgradeSchema(argv[2]);
        else if(argc==6 && !strcmp(argv[1],"ArchiveInvoice"))
                ArchiveInvoice(argv[2],argv[3],argv[4],argv[5]);
        else if(argc==5 && !strcmp(argv[1],"ImportTemplateFile"))
                ImportTemplateFile(argv[2],argv[3],argv[4]);
	else if(argc==2 && !strcmp(argv[1],"GenerateInvoices"))
		GenerateInvoices();
	else if(argc==2 && !strcmp(argv[1],"EmailAllInvoices"))
		EmailAllInvoices();
	else if(argc==2 && !strcmp(argv[1],"EncodeMD5tClientConfig"))
		EncodeMD5tClientConfig();
	else if(argc==2 && !strcmp(argv[1],"InserttClientConfig"))
		InserttClientConfig();
	else if(argc==2 && !strcmp(argv[1],"mysqlMailuClientUpdate"))
		mysqlMailuClientUpdate();
	else if(argc==2 && !strcmp(argv[1],"mysqlApache2uClientUpdate"))
		mysqlApache2uClientUpdate();
	else if(argc==2 && !strcmp(argv[1],"mysqlRadius2uClientUpdate"))
		mysqlRadius2uClientUpdate();
	else if(argc==2 && !strcmp(argv[1],"iDNSuClientUpdate"))
		iDNSuClientUpdate();
	else
	{
		printf("\n%s Menu %s\n",argv[0],RELEASE);

		printf("\nDatabase Ops\n");
		printf("\nDatabase Ops\n");
		printf("\tInitialize <mysql root passwd>\n");
		printf("\tBackup|RestoreAll <mysql root passwd>\n");
		printf("\tRestore <Restore table name> <mysql root passwd>\n");

		printf("\nOther Admin Ops\n");
                printf("\tProcessJobQueue cServer\n");
                printf("\tProcessJobQueueDebug cServer\n");
		printf("\tUpgradeSchema <mysql root passwd>\n");
                printf("\tArchiveInvoice <Mon> <Year> <path to mysql table> <mysql root passwd>\n");
                printf("\tGenerateInvoices\n");
		printf("\tEmailAllInvoices\n");

		printf("\n");

	}
	mysql_close(&gMysql);
        exit(0);

}//void ExtMainShell(int argc, char *argv[])


void DashBoard(const char *cOptionalMsg)
{

	//To handle error messages etc.
	if(cOptionalMsg[0] && strcmp(cOptionalMsg,"DashBoard"))
	{
		printf("%s\n",cOptionalMsg);
		return;
	}

        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	time_t luClock;
	char cConfigBuffer[256]={""};
	char cWhere[65]={""};

	OpenFieldSet("Dashboard",100);

	GetConfiguration("DashGraph0",cConfigBuffer,0);
	if(cConfigBuffer[0])
	{
		char cURL[256]={""};

		OpenRow("DashGraphs","black");
		printf("</td></tr>\n");

		GetConfiguration("DashGraph0URL",cURL,0);
		if(cConfigBuffer[0] && cURL[0])
			printf("<tr><td></td><td colspan=3><a href=%s><img src=%s border=0></a>\n",
					cURL,cConfigBuffer);
		else if(cConfigBuffer[0])
			printf("<tr><td></td><td colspan=3><img src=%s>\n",cConfigBuffer);

		cURL[0]=0;
		cConfigBuffer[0]=0;
		GetConfiguration("DashGraph1",cConfigBuffer,0);
		GetConfiguration("DashGraph1URL",cURL,0);
		if(cConfigBuffer[0] && cURL[0])
			printf("<a href=%s><img src=%s border=0></a>\n",cURL,cConfigBuffer);
		else if(cConfigBuffer[0])
			printf("<img src=%s>\n",cConfigBuffer);

		cURL[0]=0;
		cConfigBuffer[0]=0;
		GetConfiguration("DashGraph2",cConfigBuffer,0);
		GetConfiguration("DashGraph2URL",cURL,0);
		if(cConfigBuffer[0] && cURL[0])
			printf("<a href=%s><img src=%s border=0></a>\n",cURL,cConfigBuffer);
		else if(cConfigBuffer[0])
			printf("<img src=%s>\n",cConfigBuffer);


		printf("</td>");
	}



	//Will add when we have something for this section.
	//OpenRow("Cluster Health","black");

	//
	OpenRow("MotD (Last 5)","black");
	ExtDashboardSelect("tMotd",
				"FROM_UNIXTIME(GREATEST(tMotd.uCreatedDate,tMotd.uModDate)),cMessage",
				"1=1",
				"GREATEST(tMotd.uCreatedDate,tMotd.uModDate) DESC",5);
	macro_mySQLQueryErrorText;
	printf("</td></tr>\n");
	while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		printf("<td></td><td>%s</td><td>%s</td></tr>\n",
				mysqlField[0],mysqlField[1]);
	}
	mysql_free_result(mysqlRes);


if(guPermLevel>11)
{

	//	
	OpenRow("Cluster Errors (Last 10)","black");
	ExtDashboardSelect("tLog",
				"tLog.cMessage,GREATEST(tLog.uCreatedDate,tLog.uModDate),cServer,tLog.cLabel,"
				"uPermLevel,uTablePK",
				"uLogType=5",
				"GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC",10);
	macro_mySQLQueryErrorText;
	printf("</td></tr>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[1],"%lu",&luClock);
		printf("<td></td><td>%s</td><td colspan=2><a href=unxsISP.cgi?gcFunction=tZone&uZone=%s>%s"
			"</a> %s (%s times)</td><td>%s</td></tr>\n",
				ctime(&luClock),mysqlField[5],mysqlField[3],mysqlField[0],mysqlField[4],mysqlField[2]);
	}
	mysql_free_result(mysqlRes);

	//
	OpenRow("System Messages (Last 10)","black");
	ExtDashboardSelect("tLog",
				"tLog.cMessage,GREATEST(tLog.uCreatedDate,tLog.uModDate),cServer,uLog",
				"uLogType=4",
				"GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC",10);
	macro_mySQLQueryErrorText;
	printf("</td></tr>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[1],"%lu",&luClock);
		printf("<td></td><td>%s</td><td colspan=2>%s...<a href=unxsISP.cgi?gcFunction=tLog&uLog=%s>"
				"(More)</a></td><td>%s</td></tr>\n",
					ctime(&luClock),cShortenText(mysqlField[0],10),mysqlField[3],mysqlField[2]);
	}
	mysql_free_result(mysqlRes);

	//1-3 backend org admin interfaces
	OpenRow("General Usage (Last 10)","black");
	ExtDashboardSelect("tLog,tLogType",
				"tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),"
				"tLog.cLogin,tLog.cTableName,tLog.cHost,tLogType.cLabel",
				"tLog.uLogType=tLogType.uLogType AND tLog.uLogType<=3",
				"GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC",10);
	macro_mySQLQueryErrorText;
	printf("</td></tr>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[1],"%lu",&luClock);
		printf("<td></td><td>%s</td><td>%s %s</td><td>%s %s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[3],mysqlField[2],mysqlField[5],mysqlField[4]);
	}
	mysql_free_result(mysqlRes);

}

	//
	OpenRow("Login Activity (Last 10)","black");
	ExtDashboardSelect("tLog,tLogType","tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),tLog.cServer,"
				"tLog.cHost,tLogType.cLabel"
				,"tLog.uLogType=tLogType.uLogType AND tLog.uLogType>=6",
				"GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC",10);
	macro_mySQLQueryErrorText;
	printf("</td></tr>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[1],"%lu",&luClock);
		printf("<td></td><td>%s</td><td>%s %s</td><td>%s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[4],mysqlField[2],mysqlField[3]);
	}
	mysql_free_result(mysqlRes);

	//
	OpenRow("Jobs Pending (Last 10)","black");
	sprintf(cWhere,"tJob.uJobClient=tClient.uClient AND tJob.uJobStatus!=%u",unxsISP_Deployed);

	if(guPermLevel>11) //Let's avoid the 'table/alias is not unique' error ;)
		ExtDashboardSelect("tJob,tClient",
					"tJob.cJobName,GREATEST(tJob.uCreatedDate,tJob.uModDate),tClient.cLabel,cServer",
					cWhere,
					"GREATEST(tJob.uCreatedDate,tJob.uModDate) DESC",10);
	else
		ExtDashboardSelect("tJob",
					"tJob.cJobName,GREATEST(tJob.uCreatedDate,tJob.uModDate),tClient.cLabel,cServer",
					cWhere,
					"GREATEST(tJob.uCreatedDate,tJob.uModDate) DESC",20);
	macro_mySQLQueryErrorText;
	printf("</td></tr>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[1],"%lu",&luClock);
		printf("<td></td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[2],mysqlField[3]);
	}
	mysql_free_result(mysqlRes);

	CloseFieldSet();

}//void DashBoard(const char *cOptionalMsg)


char *cShortenText(char *cText,unsigned uNumWords)
{
	//Return the first n word from cText
	//will use the spaces for word counting.
	unsigned uCount=0;
	register int i=0;
	static char cResult[100];
	
	for(i=0;i<strlen(cText);i++)
	{
		cResult[i]=cText[i];
		if(cText[i]==' ')
			uCount++;
		if(uCount>=uNumWords) break;
	}

	cResult[i]='\0';
	return(cResult);
	
}//char *cShortenText(char *cText)


void RestoreAll(char *cPasswd)
{
	char cISMROOT[256]={""};
	register int i=0;

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		fprintf(stderr,"You must set ISMROOT env var first. Ex. (bash) export ISMROOT=/home/ism-3.0\n");
		mysql_close(&gMysql);
		exit(1);
	}

	printf("Restoring unxsISP data from .txt file in %s/unxsISP/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(1);
	}

	for(i=0;cTableList[i][0];i++)
	{
sprintf(gcQuery,"load data infile '%s/unxsISP/data/%s.txt' replace into table %s",cISMROOT,cTableList[i],cTableList[i]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			mysql_close(&gMysql);
			exit(1);
		}
		printf("%s\n",cTableList[i]);
	}

	printf("\nDone\n");

}//void RestoreAll()


void Restore(char *cTableName,char *cPasswd)
{
	char cISMROOT[256]={""};

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		fprintf(stderr,"You must set ISMROOT env var first. Ex. (bash) export ISMROOT=/home/ism-3.0\n");
		mysql_close(&gMysql);
		exit(1);
	}

	printf("Restoring unxsISP data from .txt file in %s/unxsISP/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(1);
	}

	sprintf(gcQuery,"load data infile '%s/unxsISP/data/%s.txt' replace into table %s",cISMROOT,cTableName,cTableName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(1);
	}

	printf("%s\n\nDone\n",cTableName);

}//void Restore(char *cTableName)


void Backup(char *cPasswd)
{
	char cISMROOT[256]={""};
	register int i=0;

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		fprintf(stderr,"You must set ISMROOT env var first. Ex. (bash) export ISMROOT=/home/ism-3.0\n");
		mysql_close(&gMysql);
		exit(1);
	}

	printf("Backing up unxsISP data to .txt files in %s/unxsISP/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(1);
	}

	for(i=0;cTableList[i][0];i++)
	{
		char cFileName[300];

		sprintf(cFileName,"%s/unxsISP/data/%s.txt"
				,cISMROOT,cTableList[i]);
		unlink(cFileName);

sprintf(gcQuery,"SELECT * INTO OUTFILE '%s' FROM %s",cFileName,cTableList[i]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			mysql_close(&gMysql);
			exit(1);
		}
		printf("%s\n",cTableList[i]);
	}


	printf("\nDone.\n");

}//void Backup()


void Initialize(char *cPasswd)
{
	char cISMROOT[256]={""};
	register int i=0;

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		fprintf(stderr,"You must set ISMROOT env var first. Ex. bash)"
			" export ISMROOT=/home/ism-3.0\n");
		mysql_close(&gMysql);
		exit(1);
	}

	printf("Creating db and setting permissions, installing data from %s/unxsISP...\n\n",
					cISMROOT);

	//connect as root to master db
        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,NULL,"root",cPasswd,"mysql",DBPORT,DBSOCKET,0))
        {
		if(mysql_errno(&gMysql))
			fprintf(stderr,"%s\n",mysql_error(&gMysql));

                fprintf(stderr,"Database server unavailable\n");
		mysql_close(&gMysql);
                exit(1);
        }


	//Create database
	sprintf(gcQuery,"CREATE DATABASE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(1);
	}

	//Grant localaccess privileges.
	sprintf(gcQuery,"GRANT ALL ON %s.* to %s@localhost identified by '%s'",
							DBNAME,DBLOGIN,DBPASSWD);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(1);
	}
	
	//Change to mysqlX db. Then initialize some tables with needed data
	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(1);
	}
	
	
	//Create tables and install default data
        CreatetAccountType();
	CreatetAuthorize();
	CreatetClient();
        CreatetClientConfig();
	CreatetConfig();
	CreatetConfiguration();
        CreatetGlossary();
        CreatetInstance();
	CreatetInvoice();
	CreatetInvoiceItems();
        CreatetInvoiceMonth();
	CreatetInvoiceStatus();
        CreatetJob();
        CreatetJobStatus();
        CreatetLog();
        CreatetLogMonth();
        CreatetLogType();
        CreatetMonth();
        CreatetMotd();
	CreatetPaid();
	CreatetParameter();
	CreatetParamType();
	CreatetPayment();
	CreatetPeriod();
        CreatetProduct();
        CreatetProductType();
        CreatetService();
	CreatetServiceGlue();
        CreatetServer();
	CreatetShipping();
	CreatetStatus();
	CreatetTemplate();
	CreatetTemplateType();
	CreatetTemplateSet();

        for(i=0;cInitTableList[i][0];i++)
        {
                sprintf(gcQuery,"load data local infile '%s/unxsISP/data/%s.txt' replace into table %s",cISMROOT,cInitTableList[i],cInitTableList[i]);
                mysql_query(&gMysql,gcQuery);
                if(mysql_errno(&gMysql))
                {
                        printf("%s\n",mysql_error(&gMysql));
			mysql_close(&gMysql);
                        exit(1);
                }
        }

        printf("Done\n");

}//void Initialize(char *cPasswd)


void mySQLRootConnect(char *cPasswd)
{
        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,NULL,"root",cPasswd,"mysql",DBPORT,DBSOCKET,0))
        {
		if(mysql_errno(&gMysql))
			fprintf(stderr,"%s\n",mysql_error(&gMysql));

                fprintf(stderr,"Database server unavailable\n");
                exit(1);
        }
}//void mySQLRootConnect(void)


void UpgradeSchema(char *cPasswd)
{
	printf("Upgrading/updating older version database schema...\n\n");

	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("\nUpgradeSchema() if needed\n");
	sprintf(gcQuery,"CREATE TABLE `tTicket` ("
			"`uTicket` int(10) unsigned NOT NULL auto_increment,"
			" `uOwner` int(10) unsigned NOT NULL default '0',"
			"  `uCreatedBy` int(10) unsigned NOT NULL default '0',"
			"  `uCreatedDate` int(10) unsigned NOT NULL default '0',"
			"  `uModBy` int(10) unsigned NOT NULL default '0',"
			"  `uModDate` int(10) unsigned NOT NULL default '0',"
			"  `uTicketStatus` int(10) unsigned NOT NULL default '0',"
			"  `uTicketOwner` int(10) unsigned NOT NULL default '0',"
			"  `uScheduleDate` int(10) unsigned NOT NULL default '0',"
			"  `cText` text NOT NULL,"
			"  `cKeywords` varchar(128) NOT NULL default '',"
			"  `cSubject` varchar(255) NOT NULL default '',"
			" PRIMARY KEY  (`uTicket`),"
			"  KEY `uOwner` (`uOwner`)"
			")");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		fprintf(stderr,"%s\n",mysql_error(&gMysql));

	sprintf(gcQuery,"CREATE TABLE `tTicketComment` ("
			"  `uTicketComment` int(10) unsigned NOT NULL auto_increment,"
			"  `uTicket` int(10) unsigned NOT NULL default '0',"
			"  `cComment` text NOT NULL,"
			"  `uOwner` int(10) unsigned NOT NULL default '0',"
			"  `uCreatedBy` int(10) unsigned NOT NULL default '0',"
			"  `uCreatedDate` int(10) unsigned NOT NULL default '0',"
			"  `uModBy` int(10) unsigned NOT NULL default '0',"
			"  `uModDate` int(10) unsigned NOT NULL default '0',"
			"  PRIMARY KEY  (`uTicketComment`),"
			"  KEY `uOwner` (`uOwner`)"
			")");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		fprintf(stderr,"%s\n",mysql_error(&gMysql));

}//void UpgradeSchema()



//Archive in read-only compressed myISAM format all tInvoice and related data
//Keep all relations intact.
//Be overly cautious. This is business accounting data!
void ArchiveInvoice(char *cMonth, char *cYear, char *cTablePath,char *cPasswd)
{
	char cInvoiceTableName[33]={""};
	char cItemTableName[33]={""};
	char cPaidTableName[33]={""};
	char *cNextMonth;
	char cNextYear[8]={""};
	char cThisYear[8]={""};
	char cThisMonth[4]={""};
	time_t uStart=0;
	time_t uEnd=0;
	unsigned uRows=0;
	time_t clock;
	struct tm *structTime;
	struct stat info;
	unsigned uMainVersion=0,uMinorVersion=0,uSubVersion=0;

	time(&clock);
	structTime=localtime(&clock);
	strftime(cThisYear,8,"%Y",structTime);
	strftime(cThisMonth,4,"%b",structTime);

	printf("%s %s\n",cThisMonth,cThisYear);

	if(stat("/usr/bin/myisampack",&info) )
	{
		fprintf(stderr,"/usr/bin/myisampack is not installed!\n");
		exit(1);
	}

	if(stat("/usr/bin/myisamchk",&info))
	{
		fprintf(stderr,"/usr/bin//usrmyisamchk is not installed!\n");
		exit(1);
	}

	mySQLRootConnect(cPasswd);
	mysql_query(&gMysql,"USE unxsisp");
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	sscanf(mysql_get_server_info(&gMysql),"%u.%u.%u",&uMainVersion,&uMinorVersion,&uSubVersion);

	if(uMainVersion<4 || (uMainVersion==4 && uSubVersion<4 && uMinorVersion<1) )
	{
		printf("ArchiveInvoice() Needs at least mySQL 4.0.4 server");
		exit(0);
	}


	sprintf(cInvoiceTableName,"tInvoice%.3s%.7s",cMonth,cYear);
	sprintf(cItemTableName,"tInvoiceItems%.3s%.7s",cMonth,cYear);
	sprintf(cPaidTableName,"tPaid%.3s%.7s",cMonth,cYear);

	sprintf(gcQuery,"1-%s-%s",cMonth,cYear);
	uStart=cDateToUnixTime(gcQuery);
	printf("Start: %s",ctime(&uStart));

	if(uStart== -1 || !uStart)
	{
		fprintf(stderr,"Garbled month year input. Hint month is 3 char US name.\n");
		exit(1);
	}

	NextMonthYear(cMonth,cYear,cNextMonth,cNextYear);

	sprintf(gcQuery,"1-%s-%s",cNextMonth,cNextYear);
	uEnd=cDateToUnixTime(gcQuery);
	printf("End:   %s",ctime(&uEnd));
	if(uEnd== -1 || !uEnd)
	{
		fprintf(stderr,"Garbled month year input\n");
		exit(1);
	}

	//printf("%s %s to %s %s",cMonth,cYear,cNextMonth,cNextYear);
	
	CreatetInvoiceMonthTable(cInvoiceTableName);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s. on %s.\n",mysql_error(&gMysql),cInvoiceTableName);
		exit(1);
	}

	CreatetInvoiceItemsMonthTable(cItemTableName);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s. on %s.\n",mysql_error(&gMysql),cItemTableName);
		exit(1);
	}

	CreatetPaidMonthTable(cPaidTableName);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s. on %s.\n",mysql_error(&gMysql),cPaidTableName);
		exit(1);
	}


	//Use mysqlisp.h constants sooner or later :) for th schema from tinvoice.c

	printf("Inserting paid or voided invoices...\n");
	sprintf(gcQuery,"INSERT %s (cAuthCode,cComments,uInvoice,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate,uInvoiceStatus,mTotal,mSH,mTax,uShipping,cPnRef,uAvsAddr,uAvsZip,uResultCode,cFirstName,cLastName,cEmail,cAddr1,cAddr2,cCity,cState,cZip,cCountry,cCardType,cCardNumber,uExpMonth,uExpYear,cCardName,cShipName,cShipAddr1,cShipAddr2,cShipCity,cShipState,cShipZip,cShipCountry,cTelephone,cFax,uPayment,uClient,uQueueStatus,mPaid) SELECT cAuthCode,cComments,uInvoice,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate,uInvoiceStatus,mTotal,mSH,mTax,uShipping,cPnRef,uAvsAddr,uAvsZip,uResultCode,cFirstName,cLastName,cEmail,cAddr1,cAddr2,cCity,cState,cZip,cCountry,cCardType,cCardNumber,uExpMonth,uExpYear,cCardName,cShipName,cShipAddr1,cShipAddr2,cShipCity,cShipState,cShipZip,cShipCountry,cTelephone,cFax,uPayment,uClient,uQueueStatus,mPaid FROM tInvoice WHERE uCreatedDate>=%lu AND uCreatedDate<%lu AND uInvoiceStatus>100",cInvoiceTableName,uStart,uEnd);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	//If 0 records inserted delete from tMonth and exit now
	uRows=mysql_affected_rows(&gMysql);

	printf("Number of rows found and inserted: %u\n",uRows);

	if(uRows)
	{

		guLoginClient=1;//Implicitly Root
		sprintf(gcQuery,"REPLACE tMonth SET cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",cInvoiceTableName,guLoginClient,guLoginClient,clock);
        	mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}
	}
	else
	{
		sprintf(gcQuery,"DELETE FROM tMonth WHERE cLabel='%s'",cInvoiceTableName);
        	mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}

		sprintf(gcQuery,"DROP TABLE %s",cInvoiceTableName);
        	mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}


		printf("Exiting early no data to be archived, Check!\n");
		exit(0);
	}

	sprintf(gcQuery,"/usr/bin/myisampack %s/%s",cTablePath,cInvoiceTableName);
	system(gcQuery);

	sprintf(gcQuery,"/usr/bin/myisamchk -rq %s/%s",cTablePath,cInvoiceTableName);
	system(gcQuery);

	printf("Flushing compressed RO table...\n");
	sprintf(gcQuery,"FLUSH TABLE %s",cInvoiceTableName);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Copied invoice data and archived into: %s\n",cInvoiceTableName);
	printf("Use unxsISP tInvoiceMonth to use it.\n");


	//Need to do the same with tInvoiceItems but make sure we only archive
	//items for invoices above!
	printf("Inserting related data from tInvoiceItems...\n");
	sprintf(gcQuery,"INSERT %s (uInvoiceItems,uInvoice,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate,uClient,uProduct,uQuantity,mTax,mTotal,mSH,mPrice,cLabel,uInstance) SELECT tInvoiceItems.uInvoiceItems,tInvoiceItems.uInvoice,tInvoiceItems.uOwner,tInvoiceItems.uCreatedBy,tInvoiceItems.uCreatedDate,tInvoiceItems.uModBy,tInvoiceItems.uModDate,tInvoiceItems.uClient,tInvoiceItems.uProduct,tInvoiceItems.uQuantity,tInvoiceItems.mTax,tInvoiceItems.mTotal,tInvoiceItems.mSH,tInvoiceItems.mPrice,tInvoiceItems.cLabel,tInvoiceItems.uInstance FROM tInvoiceItems,tInvoice WHERE tInvoiceItems.uInvoice=tInvoice.uInvoice AND tInvoice.uCreatedDate>=%lu AND tInvoice.uCreatedDate<%lu AND uInvoiceStatus>100",cItemTableName,uStart,uEnd);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	uRows=mysql_affected_rows(&gMysql);
	if(!uRows)
	{
		printf("Exiting early no data to be archived, Check!\n");
		exit(0);
	}

	printf("Item number of rows found and inserted: %u\n",uRows);

	sprintf(gcQuery,"/usr/bin/myisampack %s/%s",cTablePath,cItemTableName);
	system(gcQuery);

	sprintf(gcQuery,"/usr/bin/myisamchk -rq %s/%s",cTablePath,cItemTableName);
	system(gcQuery);

	printf("Flushing compressed RO table...\n");
	sprintf(gcQuery,"FLUSH TABLE %s",cItemTableName);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Copied invoice item data and archived into: %s\n",cItemTableName);


	//Need to do the same with tPaid but make sure we only archive
	//items for invoices above!
	printf("Getting data from tPaid...\n");
	sprintf(gcQuery,"INSERT %s (uPaid,uPayment,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate,mAmount,uInvoice,uClient,cComment) SELECT tPaid.uPaid,tPaid.uPayment,tPaid.uOwner,tPaid.uCreatedBy,tPaid.uCreatedDate,tPaid.uModBy,tPaid.uModDate,tPaid.mAmount,tPaid.uInvoice,tPaid.uClient,tPaid.cComment FROM tPaid,tInvoice WHERE tPaid.uInvoice=tInvoice.uInvoice AND tInvoice.uCreatedDate>=%lu AND tInvoice.uCreatedDate<%lu AND uInvoiceStatus>100",cPaidTableName,uStart,uEnd);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	uRows=mysql_affected_rows(&gMysql);
	if(!uRows)
	{
		printf("Exiting early no data to be archived, Check!\n");
		exit(0);
	}

	printf("Item number of rows found and inserted: %u\n",uRows);

	sprintf(gcQuery,"/usr/bin/myisampack %s/%s",cTablePath,cPaidTableName);
	system(gcQuery);

	sprintf(gcQuery,"/usr/bin/myisamchk -rq %s/%s",cTablePath,cPaidTableName);
	system(gcQuery);

	printf("Flushing compressed RO table...\n");
	sprintf(gcQuery,"FLUSH TABLE %s",cPaidTableName);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Copied paid data and archived into: %s\n",cPaidTableName);


	printf("Removing archived data from tPaid, tInvoice and tInvoiceItems...\n");

	sprintf(gcQuery,"DELETE tPaid FROM tPaid,tInvoice WHERE tPaid.uInvoice=tInvoice.uInvoice AND tInvoice.uCreatedDate>=%lu AND tInvoice.uCreatedDate<%lu AND uInvoiceStatus>100",uStart,uEnd);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	sprintf(gcQuery,"DELETE tInvoiceItems FROM tInvoiceItems,tInvoice WHERE tInvoiceItems.uInvoice=tInvoice.uInvoice AND tInvoice.uCreatedDate>=%lu AND tInvoice.uCreatedDate<%lu AND uInvoiceStatus>100",uStart,uEnd);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	sprintf(gcQuery,"DELETE FROM tInvoice WHERE uCreatedDate>=%lu AND uCreatedDate<%lu AND uInvoiceStatus>100",uStart,uEnd);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Everything worked fine. But check before you delete backups...\n");

	exit(0);

}//void ArchiveInvoice(char *cMonth, char *cYear, char *cTablePath)


void ImportTemplateFile(char *cTemplate, char *cFile, char *cTemplateSet)
{
	FILE *fp;
	unsigned uTemplate=0;
	unsigned uTemplateSet=0;
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	char cBuffer[2048]={""};

	printf("\nImportTemplateFile(): Start\n");

	TextConnectDb();

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	if((fp=fopen(cFile,"r"))==NULL)
	{
		printf("Could not open %s\n",cFile);
		exit(1);
	}

	//uTemplateSet
	sprintf(gcQuery,"SELECT uTemplateSet FROM tTemplateSet WHERE cLabel='%s'",cTemplateSet);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        mysqlRes=mysql_store_result(&gMysql);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sscanf(mysqlField[0],"%u",&uTemplateSet);
	mysql_free_result(mysqlRes);

	if(!uTemplateSet)
	{
		printf("Could not find tTemplateSet.clabel=%s\n",cTemplateSet);
		exit(1);
	}

	//uTemplate
	sprintf(gcQuery,"SELECT uTemplate FROM tTemplate WHERE cLabel='%s' AND uTemplateSet=%u",cTemplate,uTemplateSet);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        mysqlRes=mysql_store_result(&gMysql);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sscanf(mysqlField[0],"%u",&uTemplate);
	mysql_free_result(mysqlRes);


	if(uTemplate)
	{
		printf("Updating tTemplate for %s\n",cTemplate);
		sprintf(cBuffer,"UPDATE tTemplate SET uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()),"
				"cTemplate='' WHERE uTemplate=%u",uTemplate);
		mysql_query(&gMysql,cBuffer);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n%.254s\n",mysql_error(&gMysql),cBuffer);
			exit(1);
		}

	}
	else
	{
		printf("Inserting new tTemplate for %s\n",cTemplate);
		sprintf(cBuffer,"INSERT INTO tTemplate SET uOwner=1,uCreatedBy=1,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW()),cLabel='%s',uTemplateSet=%u",
				cTemplate,uTemplateSet);
		mysql_query(&gMysql,cBuffer);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n%.254s\n",mysql_error(&gMysql),cBuffer);
			exit(1);
		}

		uTemplate=mysql_insert_id(&gMysql);

	}

	while(fgets(gcQuery,1024,fp)!=NULL)
	{
		sprintf(cBuffer,"UPDATE tTemplate SET cTemplate=CONCAT(cTemplate,'%s') WHERE uTemplate=%u",TextAreaSave(gcQuery),uTemplate);
		mysql_query(&gMysql,cBuffer);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n%.254s\n",mysql_error(&gMysql),cBuffer);
			exit(1);
		}
			
	}
	fclose(fp);

	printf("\nDone\n");

}//void ImportTemplateFile()


void CalledByAlias(int iArgc,char *cArgv[])
{
	if(strstr(cArgv[0],"unxsISPRSS.xml"))
	{
		MYSQL_RES *res;
		MYSQL_ROW field;
		char cRSSDate[200];
		char cLinkStart[200]={""};
           	time_t tTime,luClock;
           	struct tm *tmTime;
		char *cHTTP="http";

		time(&luClock);
           	tTime=time(NULL);
           	tmTime=gmtime(&tTime);
           	strftime(cRSSDate,sizeof(cRSSDate),"%a, %d %b %Y %T GMT",tmTime);

		if(getenv("HTTP_HOST")!=NULL)
			sprintf(gcHost,"%.99s",getenv("HTTP_HOST"));
		if(getenv("HTTPS")!=NULL)
			cHTTP="https";

		//This is the standard place. With much parsing nonsense we can
		//	do better by using env vars
		sprintf(cLinkStart,"%s://%s/cgi-bin/unxsISP.cgi",cHTTP,gcHost);

		printf("Content-type: text/xml\n\n");

		//Open xml
		printf("<?xml version='1.0' encoding='UTF-8'?>\n");
		printf("<rss version='2.0'>\n");
		printf("<channel>\n");
		printf("<title>unxsISP RSS tJob Errors</title>\n");
		printf("<link>http://openisp.net/unxsISP</link>\n");
		printf("<description>unxsISP tJob Errors</description>\n");
		printf("<lastBuildDate>%.199s</lastBuildDate>\n",cRSSDate);
		printf("<generator>unxsISP RSS Generator</generator>\n");
		printf("<docs>http://blogs.law.harvard.edu/tech/rss</docs>\n");
		printf("<ttl>120</ttl>\n");

		//Loop for each tJob error
		//Connect to local mySQL
		TextConnectDb();
		sprintf(gcQuery,"SELECT uJob,cServer,cJobName,uUser,cJobData FROM tJob WHERE uJobStatus=4");//4 is tJobStatus Done Error(s)
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			printf("\n<item>\n");
			printf("<title>unxsISP.tJob.uJob=%s</title>\n",field[0]);
			printf("<link>%s?gcFunction=tJob&amp;uJob=%s</link>\n",cLinkStart,
							field[0]);
			printf("<description>cJobName=%s Server=%s uUser=%s\ncJobData=(%s)</description>\n",field[2],field[1],field[3],field[4]);
			printf("<guid isPermaLink='false'>%s-%lu</guid>\n",field[0],luClock);
			printf("<pubDate>%.199s</pubDate>\n",cRSSDate);
			printf("</item>\n");

		}
       		mysql_free_result(res);

		//Close xml
		printf("\n</channel>\n");
		printf("</rss>\n");
		mysql_close(&gMysql);
		exit(0);
	}
	else
	{
		printf("Content-type: text/plain\n\n");
		printf("Called as unsupported alias %s\n",cArgv[0]);
		exit(0);
	}

}//void CalledByAlias(int iArgc,char *cArgv[])


void ExtracttLog(char *cMonth, char *cYear, char *cPasswd, char *cTablePath)
{
	char cTableName[33]={""};
	char cNextMonth[4]={""};
	char cNextYear[8]={""};
	char cThisYear[8]={""};
	char cThisMonth[4]={""};
	time_t uStart=0;
	time_t uEnd=0;
	unsigned uRows=0;
	time_t clock;
	struct tm *structTime;
	struct stat info;

	time(&clock);
	structTime=localtime(&clock);
	strftime(cThisYear,8,"%Y",structTime);
	strftime(cThisMonth,4,"%b",structTime);

	printf("ExtracttLog() Start\n");

	printf("cThisMonth:%s cThisYear:%s\n",cThisMonth,cThisYear);

	if(!strcmp(cThisMonth,cMonth) && !strcmp(cThisYear,cYear))
	{
		fprintf(stderr,"Can't extract this months data!\n");
		exit(1);
	}

	if(stat("/usr/bin/myisampack",&info) )
	{
		fprintf(stderr,"/usr/bin/myisampack is not installed!\n");
		exit(1);
	}

	if(stat("/usr/bin/myisamchk",&info))
	{
		fprintf(stderr,"/usr/bin/myisamchk is not installed!\n");
		exit(1);
	}


	mySQLRootConnect(cPasswd);
	mysql_query(&gMysql,"USE unxsisp");
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	sprintf(gcQuery,"INSERT INTO tLog SET cMessage='ExtracttLog() Start...',cServer='%s',uLogType=4,uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",gcHostname);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		fprintf(stderr,"%s\n",mysql_error(&gMysql));

	sprintf(cTableName,"t%.3s%.7s",cMonth,cYear);

	sprintf(gcQuery,"1-%s-%s",cMonth,cYear);
	uStart=cDateToUnixTime(gcQuery);
	printf("Start: %s",ctime(&uStart));

	if(uStart== -1 || !uStart)
	{
		fprintf(stderr,"Garbled month year input (uStart)\n");
		exit(1);
	}

	NextMonthYear(cMonth,cYear,cNextMonth,cNextYear);
	//Debug only
	//printf("%s %s to %s %s\n",cMonth,cYear,cNextMonth,cNextYear);
	//exit(0);

	sprintf(gcQuery,"1-%s-%s",cNextMonth,cNextYear);
	uEnd=cDateToUnixTime(gcQuery);
	printf("End:   %s",ctime(&uEnd));
	if(uEnd== -1 || !uEnd)
	{
		fprintf(stderr,"Garbled month year input (uEnd)\n");
		exit(1);
	}

	
	CreatetLogTable(cTableName);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Clearing data from %s...\n",cTableName);
	sprintf(gcQuery,"DELETE FROM %s",cTableName);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Getting data from tLog...\n");
	sprintf(gcQuery,"INSERT %s (uLog,cLabel,uLogType,cHash,uPermLevel,uLoginClient,cLogin,cHost,uTablePK,cTableName,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate) SELECT uLog,cLabel,uLogType,cHash,uPermLevel,uLoginClient,cLogin,cHost,uTablePK,cTableName,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate FROM tLog WHERE uCreatedDate>=%lu AND uCreatedDate<%lu",cTableName,uStart,uEnd);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	//If 0 records inserted delete from tMonth and exit now
	uRows=mysql_affected_rows(&gMysql);

	printf("Number of rows found and inserted: %u\n",uRows);

	if(uRows)
	{

		sprintf(gcQuery,"REPLACE tMonth SET cLabel='%s',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",cTableName);
        	mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}
	}
	else
	{
		sprintf(gcQuery,"DELETE FROM tMonth WHERE cLabel='%s'",cTableName);
        	mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}

		sprintf(gcQuery,"DROP TABLE %s",cTableName);
        	mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}


		printf("Exiting early no data to be archived\n");
		exit(0);
	}

	sprintf(gcQuery,"/usr/bin/myisampack %s/%s",cTablePath,cTableName);
	if(system(gcQuery))
	{
		fprintf(stderr,"Failed: %s\n",gcQuery);
		exit(1);
	}

	sprintf(gcQuery,"/usr/bin/myisamchk -rq %s/%s",cTablePath,cTableName);
	if(system(gcQuery))
	{
		fprintf(stderr,"Failed: %s\n",gcQuery);
		exit(1);
	}

	printf("Flushing compressed RO table...\n");
	sprintf(gcQuery,"FLUSH TABLE %s",cTableName);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Removing records from tLog...\n");
	sprintf(gcQuery,"DELETE QUICK FROM tLog WHERE uCreatedDate>=%lu AND uCreatedDate<%lu AND uLogType!=5",uStart,uEnd);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}


	printf("Extracted and Archived. Table flushed: %s\n",cTableName);
	printf("ExtracttLog() End\n");
	sprintf(gcQuery,"INSERT INTO tLog SET cMessage='ExtracttLog() End',cServer='%s',uLogType=4,uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",gcHostname);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
	exit(0);

}//void ExtracttLog(char *cMonth, char *cYear, char *cPasswd, char *cTablePath)


void CreatetLogTable(char *cTableName)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS %s ( uTablePK VARCHAR(32) NOT NULL DEFAULT '', cHost VARCHAR(32) NOT NULL DEFAULT '', uLoginClient INT UNSIGNED NOT NULL DEFAULT 0, cLogin VARCHAR(32) NOT NULL DEFAULT '', uPermLevel INT UNSIGNED NOT NULL DEFAULT 0, cTableName VARCHAR(32) NOT NULL DEFAULT '', uLog INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(64) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cHash VARCHAR(32) NOT NULL DEFAULT '', uLogType INT UNSIGNED NOT NULL DEFAULT 0,index (uLogType) )",cTableName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

}//void CreatetLogTable(char *cTableName)


void NextMonthYear(char *cMonth,char *cYear,char *cNextMonth,char *cNextYear)
{
	unsigned uYear=0;

	//Preset for all but Dec cases
	sprintf(cNextYear,"%.7s",cYear);
	sscanf(cNextYear,"%u",&uYear);

	if(!strcmp(cMonth,"Jan"))
	{
		strcpy(cNextMonth,"Feb");
	}
	else if(!strcmp(cMonth,"Feb"))
	{
		strcpy(cNextMonth,"Mar");
	}
	else if(!strcmp(cMonth,"Mar"))
	{
		strcpy(cNextMonth,"Apr");
	}
	else if(!strcmp(cMonth,"Apr"))
	{
		strcpy(cNextMonth,"May");
	}
	else if(!strcmp(cMonth,"May"))
	{
		strcpy(cNextMonth,"Jun");
	}
	else if(!strcmp(cMonth,"Jun"))
	{
		strcpy(cNextMonth,"Jul");
	}
	else if(!strcmp(cMonth,"Jul"))
	{
		strcpy(cNextMonth,"Aug");
	}
	else if(!strcmp(cMonth,"Aug"))
	{
		strcpy(cNextMonth,"Sep");
	}
	else if(!strcmp(cMonth,"Sep"))
	{
		strcpy(cNextMonth,"Oct");
	}
	else if(!strcmp(cMonth,"Oct"))
	{
		strcpy(cNextMonth,"Nov");
	}
	else if(!strcmp(cMonth,"Nov"))
	{
		strcpy(cNextMonth,"Dec");
	}
	else if(!strcmp(cMonth,"Dec"))
	{
		strcpy(cNextMonth,"Jan");
		uYear++;
		sprintf(cNextYear,"%u",uYear);
	}

}//NextMonthYear()


void TextError(const char *cError, unsigned uContinue)
{
	char cQuery[1024];

	printf("\nPlease report this unxsISP fatal error ASAP:\n%s\n",cError);

	//Attempt to report error in tLog
        sprintf(cQuery,"INSERT INTO tLog SET cLabel='TextError',uLogType=4,uPermLevel=%u,uLoginClient=%u,cLogin='%s',cHost='%s',cMessage=\"%s\",cServer='%s',uOwner=1,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",guPermLevel,guLoginClient,gcUser,gcHost,cError,gcHostname,guLoginClient);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		printf("Another error occurred while attempting to log: %s\n",
				mysql_error(&gMysql));
	if(!uContinue) exit(0);

}//void TextError(const char *cError, unsigned uContinue)


unsigned uGetClientOwner(unsigned uClient)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uRet=0;

	sprintf(gcQuery,"SELECT uOwner FROM "TCLIENT"  WHERE uClient=%u",uClient);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uRet);
	mysql_free_result(res);

	return(uRet);

}//unsigned uGetClientOwner(unsigned uClient)


unsigned uGetDateLastInvoiced(unsigned uInvoice,unsigned uProduct);

unsigned uGetDateLastInvoiced(unsigned uInvoice,unsigned uProduct)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uRet=0;


	sprintf(gcQuery,"SELECT GREATEST(uCreatedDate,uModDate) FROM tInvoiceItems WHERE uInvoice=%u AND uProduct=%u",uInvoice,uProduct);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uRet);
	mysql_free_result(res);

	return(uRet);

}//unsigned uGetDateLastInvoiced(unsigned uInvoice)


void GenerateInvoices(void)
{
	//This function will run everyday. Products are billed monthly.
	//We need to query tInvoice and check if the customer was already invoiced.
	//The invoices are generated every 1st day of the month and stored in tInvoice.
	//The admin user reviews the invoices with the ispAdmin interface and sends them
	//via a command in the 'Invoice' tab left panel.
	//

	unsigned uInvoice=0;
	unsigned uOwner=0;
	unsigned uClient=0;

	MYSQL_RES *res;
	MYSQL_ROW field;

	MYSQL_RES *res2;
	MYSQL_ROW field2;

	MYSQL_RES *res3;
	MYSQL_ROW field3;
	
	MYSQL_RES *res4;
	MYSQL_ROW field4;
	
	MYSQL_RES *res5;
	MYSQL_ROW field5;

	//tInstace.uStatus must be:
	//1.Waiting for Initial Deploment
	//2.Deployed and Modified
	//3.Deployed

	TextConnectDb();
	
	//Dev only:
	/*sprintf(gcQuery,"truncate tInvoice");
	mysql_query(&gMysql,gcQuery);
	sprintf(gcQuery,"truncate tInvoiceItems");
	mysql_query(&gMysql,gcQuery);
	*/
	//1. Get client id(s) from tInstace: outer loop
	sprintf(gcQuery,"SELECT DISTINCT uClient FROM tInstance WHERE tInstance.uStatus=%u OR tInstance.uStatus=%u OR tInstance.uStatus=%u",
			unxsISP_WaitingInitial
			,unxsISP_DeployedMod
			,unxsISP_Deployed
	       );
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
	
	res3=mysql_store_result(&gMysql);

	while((field3=mysql_fetch_row(res3)))
	{		
		sscanf(field3[0],"%u",&uClient);
		uOwner=uGetClientOwner(uClient);

		//1a. Get client information
		sprintf(gcQuery,"SELECT tClient.uClient,tClient.cFirstName,tClient.cLastName,tClient.cEmail,tClient.cAddr1,tClient.cAddr2,tClient.cCity,"
				"tClient.cState,tClient.cZip,tClient.cCountry,tClient.cShipName,tClient.cShipAddr1,tClient.cShipAddr2,tClient.cShipCity,"
				"tClient.cShipState,tClient.cShipZip,tClient.cShipCountry,tClient.cTelephone,tClient.cFax,tClient.uOwner FROM tClient "
				"WHERE tClient.uClient='%s'",field3[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			TextError(mysql_error(&gMysql),0);

		res=mysql_store_result(&gMysql);

/* Last query field indexes:
tClient.uClient 0
tClient.cFirstName 1
tClient.cLastName 2
tClient.cEmail 3
tClient.cAddr1 4
tClient.cAddr2 5
tClient.cCity 6
tClient.cState 7
tClient.cZip 8
tClient.cCountry 9
tClient.cShipName 10
tClient.cShipAddr1 11
tClient.cShipAddr2 12
tClient.cShipCity 13
tClient.cShipState 14
tClient.cShipZip 15
tClient.cShipCountry 16
tClient.cTelephone 17
tClient.cFax 18
tClient.uOwner 19
*/
		while((field=mysql_fetch_row(res)))
		{
			uInvoice=uClientHasInvoice(field[0]);
			if(uInvoice)
			{
				//If the client already has an invoice, update tInvoice data with data from tClient
				//and add not invoiced product(s) to tInvoiceItems
				//2a. Update tInvoice 
				sprintf(gcQuery,"UPDATE tInvoice SET cFirstName='%s',cLastName='%s',cEmail='%s',cAddr1='%s',cAddr2='%s',cCity='%s',"
						"cState='%s',cZip='%s',cCountry='%s',cShipName='%s',cShipAddr1='%s',cShipAddr2='%s',cShipCity='%s',"
						"cShipState='%s',cShipZip='%s',cShipCountry='%s',cTelephone='%s',cFax='%s',uModBy=1,"
						"uModDate=UNIX_TIMESTAMP(NOW()) WHERE tInvoice.uClient=%s "
						"AND tInvoice.uInvoice=%u",
						TextAreaSave(field[1])
						,TextAreaSave(field[2])
						,TextAreaSave(field[3])
						,TextAreaSave(field[4])
						,TextAreaSave(field[5])
						,TextAreaSave(field[6])
						,TextAreaSave(field[7])
						,TextAreaSave(field[8])
						,TextAreaSave(field[9])
						,TextAreaSave(field[10])
						,TextAreaSave(field[11])
						,TextAreaSave(field[12])
						,TextAreaSave(field[13])
						,TextAreaSave(field[14])
						,TextAreaSave(field[15])
						,TextAreaSave(field[16])
						,TextAreaSave(field[17])
						,TextAreaSave(field[18])
						,field[0]
						,uInvoice
						);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					TextError(mysql_error(&gMysql),0);
			
				sprintf(gcQuery,"SELECT uProduct FROM tInstance WHERE uClient=%s AND (uStatus=%u OR uStatus=%u)",
						field[0]
						,unxsISP_DeployedMod
						,unxsISP_Deployed
						);

				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					TextError(mysql_error(&gMysql),0);
				res4=mysql_store_result(&gMysql);

				while((field4=mysql_fetch_row(res4)))
				{
					sprintf(gcQuery,"SELECT tProduct.uProduct,tProduct.mPrice,tProduct.uPeriod FROM tProduct WHERE "
							"tProduct.uProduct='%s'",field4[0]);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						TextError(mysql_error(&gMysql),0);

					res2=mysql_store_result(&gMysql);

					//Invoice items loop for existing invoice
					while((field2=mysql_fetch_row(res2)))
					{
						//3a. Update tInvoiceItems records of products that may have changed their price,
						if(uHasInvoiceItem(uInvoice,field4[0]))
						{
							/*
							unsigned uProduct=0;
							unsigned uPeriod=0;
							unsigned uDateLastInvoiced=0;
							time_t luClock;
							*/
							sprintf(gcQuery,"UPDATE tInvoiceItems SET mPrice='%s',mTotal=mPrice*uQuantity,uModBy=1,"
									"uModDate=UNIX_TIMESTAMP(NOW()) WHERE uProduct='%s' AND uInvoice='%u'",
									field2[0]
									,field2[1]
									,uInvoice
						       		);
							//3b. If we have to rebill (period) do it here
						}
						else
							//4a. If no record to update, insert a new tInvoiceItems record
							sprintf(gcQuery,"INSERT INTO tInvoiceItems SET uInvoice='%u',uClient='%s',uProduct='%s',uQuantity=1,"
									"mPrice='%s',uOwner=%s,mTotal=mPrice*uQuantity,uCreatedBy=1,"
									"uCreatedDate=UNIX_TIMESTAMP(NOW())",
									uInvoice
									,field[0]
									,field2[0]
									,field2[1]
									,field[19]
									);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							TextError(mysql_error(&gMysql),0);
				}//while((field4=mysql_fetch_row(res4)))

			}//end invoice items loop for existing invoice
		
		}//End customer has invoice
		else
		{
			//Add a tInvoice record with data from tClient and create one tInvoiceItems record
			//per each deployed product instance (tInstance record)

			//2b. Insert tInvoice 
			sprintf(gcQuery,"INSERT INTO tInvoice SET cFirstName='%s',cLastName='%s',cEmail='%s',cAddr1='%s',cAddr2='%s',cCity='%s',"
					"cState='%s',cZip='%s',cCountry='%s',cShipName='%s',cShipAddr1='%s',cShipAddr2='%s',cShipCity='%s',"
					"cShipState='%s',cShipZip='%s',cShipCountry='%s',cTelephone='%s',cFax='%s',uOwner=%u,uCreatedBy=1,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW()),uClient='%s',uInvoiceStatus=1",
					TextAreaSave(field[1])
					,TextAreaSave(field[2])
					,TextAreaSave(field[3])
					,TextAreaSave(field[4])
					,TextAreaSave(field[5])
					,TextAreaSave(field[6])
					,TextAreaSave(field[7])
					,TextAreaSave(field[8])
					,TextAreaSave(field[9])
					,TextAreaSave(field[10])
					,TextAreaSave(field[11])
					,TextAreaSave(field[12])
					,TextAreaSave(field[13])
					,TextAreaSave(field[14])
					,TextAreaSave(field[15])
					,TextAreaSave(field[16])
					,TextAreaSave(field[17])
					,TextAreaSave(field[18])
					,uOwner
					,field[0]
					);
			//printf("%s\n",gcQuery);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				TextError(mysql_error(&gMysql),0);
			
			uInvoice=mysql_insert_id(&gMysql);
		
			sprintf(gcQuery,"SELECT uProduct FROM tInstance WHERE uClient=%s AND (uStatus=%u OR uStatus=%u)",
					field[0]
					,unxsISP_DeployedMod
					,unxsISP_Deployed
					);
			//printf("%s\n",gcQuery);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				TextError(mysql_error(&gMysql),0);

			res4=mysql_store_result(&gMysql);
			while((field4=mysql_fetch_row(res4)))
			{
				sprintf(gcQuery,"SELECT tProduct.uProduct,tProduct.mPrice FROM tProduct WHERE tProduct.uProduct='%s'",field4[0]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					TextError(mysql_error(&gMysql),0);

				res2=mysql_store_result(&gMysql);

				//Invoice items loop for new  invoice
				while((field2=mysql_fetch_row(res2)))
				{
					//3a. Insert a new tInvoiceItems record
					sprintf(gcQuery,"SELECT uInvoiceItems FROM tInvoiceItems WHERE uInvoice=%u AND uProduct=%s",
							uInvoice
							,field2[0]
							);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						TextError(mysql_error(&gMysql),0);

					res5=mysql_store_result(&gMysql);
					if((field5=mysql_fetch_row(res5)))
						sprintf(gcQuery,"UPDATE tInvoiceItems SET uQuantity=uQuantity+1,mTotal=mPrice*uQuantity,uModBy=1,"
								"uModDate=UNIX_TIMESTAMP(NOW()) WHERE uInvoiceItems=%s",field5[0]);
					else
						sprintf(gcQuery,"INSERT INTO tInvoiceItems SET uInvoice='%u',uClient='%s',uProduct='%s',"
								"uQuantity=1,mPrice='%s',mTotal=mPrice*uQuantity,uOwner=%s,uCreatedBy=1,"
								"uCreatedDate=UNIX_TIMESTAMP(NOW())",
								uInvoice
								,field[0]
								,field2[0]
								,field2[1]
								,field[19]
								);
					mysql_free_result(res5);

					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						TextError(mysql_error(&gMysql),0);

				}//end invoice items loop for existing invoice
			}//while((field4=mysql_fetch_row(res4)))
			uInvoice=0;
		}//End new invoice
	}//End main customer data loop
	}//End outer tInstance loop
	
	//Final query, common for all invoices, update tInvoice.mTotal in a single step ;)
	sprintf(gcQuery,"UPDATE tInvoice SET mTotal=(SELECT SUM(mTotal) FROM tInvoiceItems WHERE"
			" tInvoiceItems.uInvoice=tInvoice.uInvoice)");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);

}//void GenerateInvoices(void)


unsigned uClientHasInvoice(char *cuClient)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uInvoice=0;
	//Updated query as per ticket #101, added AND MONTH(NOW())=MONTH(FROM_UNIXTIME(uCreatedDate))
	sprintf(gcQuery,"SELECT uInvoice FROM tInvoice WHERE uClient=%s AND MONTH(NOW())=MONTH(FROM_UNIXTIME(uCreatedDate))",cuClient);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);

	res=mysql_store_result(&gMysql);
	
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uInvoice);
	
	return(uInvoice);

}//unsigned uClientHasInvoice(unsigned uClient)


unsigned uHasInvoiceItem(unsigned uInvoice,char *cuProduct)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uInvoiceItems FROM tInvoiceItems WHERE uInvoice='%u' AND uProduct='%s'",uInvoice,cuProduct);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);

	res=mysql_store_result(&gMysql);

	return(unsigned)mysql_num_rows(res);

}//unsigned uHasInvoiceItem(unsigned uInvoice,char *cuProduct)


void ExtDashboardSelect(char *cTable,char *cVarList,char *cCondition, char *cOrderBy, unsigned uMaxResults)
{
	char *cp;
	char cLocalTable[100]={""};

	sprintf(cLocalTable,"%.99s",cTable);

	if((cp=strchr(cLocalTable,','))) *cp=0;
	
	if(guPermLevel>11 || !strcmp(cTable,"tMotd"))//Root level can read access all
		sprintf(gcQuery,"SELECT %1$s FROM %2$s WHERE %3$s ORDER BY %4$s",
					cVarList,cTable,cCondition,cOrderBy);
	else 
		sprintf(gcQuery,"SELECT %1$s FROM %3$s," TCLIENT
				" WHERE %6$s.uOwner=" TCLIENT ".uClient"
				" AND (" TCLIENT ".uClient=%2$u OR " TCLIENT ".uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))"
				" AND %4$s ORDER BY %5$s",
					cVarList,guCompany,
					cTable,cCondition,cOrderBy,
					cLocalTable);

	if(uMaxResults)
	{
		char cLimit[33]={""};
		sprintf(cLimit," LIMIT %u",uMaxResults);
		strcat(gcQuery,cLimit);
	}

}//void ExtDashboardSelect(char *cTable,char *cVarList,char *cCondition, char *cOrderBy, unsigned uMaxResults)


void EncodeMD5tClientConfig(void)
{
	//This function is written for one time use only.
	//tInstance (uInstance=X) , login data -> leads to tUser... from tUser.cPasswd becomes 
	//the value of the tClientConfig record for uInstance=X and uParameter=2 

	MYSQL mysqlMail2Db;
	
	MYSQL_RES *res;
	MYSQL_ROW field;

	MYSQL_RES *res2;
	MYSQL_ROW field2;

	TextConnectDb();

        mysql_init(&mysqlMail2Db);
	if(!mysql_real_connect(&mysqlMail2Db,NULL,"mysqlmail2","wsxedc","mysqlmail2",0,0,0))
	{
                fprintf(stderr,"mysqlMail2 db unavailable.\n");
		mysql_close(&mysqlMail2Db);
		exit(1);
	}

	//Outer loop, tClientConfig records for uParameter=2 will lead us to tInstance
	//look below and you'll see the trick ;)

	//uGroup is tInstance.uInstance
	sprintf(gcQuery,"SELECT uGroup,uConfig FROM tClientConfig WHERE uParameter=2");

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s",mysql_error(&gMysql));
		exit(1);
	}
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		//1. Get login. Login is uParameter=1
		sprintf(gcQuery,"SELECT cValue FROM tClientConfig WHERE uParameter=1 AND uGroup=%s",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s",mysql_error(&gMysql));
			exit(1);
		}
		res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			//2. We got login, cool, get MD5 password from mysqlMail2.tUser ;)
			sprintf(gcQuery,"SELECT cPasswd FROM tUser WHERE cLogin='%s'",field2[0]);
			mysql_query(&mysqlMail2Db,gcQuery);
			if(mysql_errno(&mysqlMail2Db))
			{
				fprintf(stderr,"%s",mysql_error(&mysqlMail2Db));
				exit(1);
			}
			mysql_free_result(res2);

			res2=mysql_store_result(&mysqlMail2Db);
			if((field2=mysql_fetch_row(res2)))
			{
				//3. We have a winner
				sprintf(gcQuery,"UPDATE tClientConfig SET cValue='%s' WHERE uConfig=%s",field2[0],field[1]);
				//printf("%s\n",gcQuery);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					fprintf(stderr,"%s",mysql_error(&gMysql));
					exit(1);
				}
				
			}
			mysql_free_result(res2);
		}

	}//Outer tClientConfig loop

}//void EncodeMD5tClientConfig(void)


void InserttClientConfig(void)
{
	//This function will insert the missing tClientConfig records
	//for the mailbox products.
	//Will add:
	//1. mysqlMail2.cServerGroup (Will get from mysqlMail2.tUser)
	//2. mysqlMail2.cUserType=Unix mail account
	//3. mysqlMail2.Domain (Will get from mysqlMail2.tUser)

	MYSQL mysqlMail2Db;
	
	MYSQL_RES *res;
	MYSQL_ROW field;

	MYSQL_RES *res2;
	MYSQL_ROW field2;

	TextConnectDb();

        mysql_init(&mysqlMail2Db);
	if(!mysql_real_connect(&mysqlMail2Db,NULL,"mysqlmail2","wsxedc","mysqlmail2",0,0,0))
	{
                fprintf(stderr,"mysqlMail2 db unavailable.\n");
		mysql_close(&mysqlMail2Db);
		exit(1);
	}

	//Outer loop, the user login
	sprintf(gcQuery,"SELECT cValue,uGroup,uService,uOwner FROM tClientConfig WHERE uParameter=1");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s",mysql_error(&gMysql));
		exit(1);
	}
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		//1. Get domain and servergroup from mysqlMail2.tUser
		sprintf(gcQuery,"SELECT (SELECT tDomain.cDomain FROM tDomain WHERE tDomain.uDomain=tUser.uDomain),"
				"(SELECT tServerGroup.cLabel FROM tServerGroup WHERE tServerGroup.uServerGroup=tUser.uServerGroup) "
				"FROM tUser WHERE cLogin='%s'",field[0]);
		mysql_query(&mysqlMail2Db,gcQuery);
		if(mysql_errno(&mysqlMail2Db))
		{
			fprintf(stderr,"%s",mysql_error(&mysqlMail2Db));
			exit(1);
		}
		res2=mysql_store_result(&mysqlMail2Db);
		if((field2=mysql_fetch_row(res2)))
		{
			//mysqlMail2.cServerGroup
			sprintf(gcQuery,"INSERT INTO tClientConfig SET uGroup=%s,uService=%s,uOwner=%s,uParameter=51,cValue='%s',uCreatedBy=1,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW())",
					field[1]
					,field[2]
					,field[3]
					,field2[1]
					);
			//printf("%s\n",gcQuery);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stderr,"%s",mysql_error(&gMysql));
				exit(1);
			}
			//mysqlMail2.Domain
			sprintf(gcQuery,"INSERT INTO tClientConfig SET uGroup=%s,uService=%s,uOwner=%s,uParameter=3,cValue='%s',uCreatedBy=1,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW())",
					field[1]
					,field[2]
					,field[3]
					,field2[0]
					);
			//printf("%s\n",gcQuery);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stderr,"%s",mysql_error(&gMysql));
				exit(1);
			}
			//mysqlMail2.cUserType
			sprintf(gcQuery,"INSERT INTO tClientConfig SET uGroup=%s,uService=%s,uOwner=%s,uParameter=61,cValue='Unix mail account',uCreatedBy=1,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW())",
					field[1]
					,field[2]
					,field[3]
					);
			//printf("%s\n",gcQuery);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stderr,"%s",mysql_error(&gMysql));
				exit(1);
			}
		}//User found at mysqlMail2.tUser
		else
			printf("Warning: user %s doesn't exist at mysqlMail2 db\n",field[0]);

	}//Outer loop

}//void InserttClientConfig(void)


void mysqlMailuClientUpdate(void)
{
	//This function will set tUser.uClient value using tInstance.uOwner value

	MYSQL mysqlMail2Db;
	
	MYSQL_RES *res;
	MYSQL_ROW field;

	TextConnectDb();

        mysql_init(&mysqlMail2Db);
	if(!mysql_real_connect(&mysqlMail2Db,NULL,"mysqlmail2","wsxedc","mysqlmail2",0,0,0))
	{
                fprintf(stderr,"mysqlMail2 db unavailable.\n");
		mysql_close(&mysqlMail2Db);
		exit(1);
	}

	//Outer loop, the user login
	sprintf(gcQuery,"SELECT cValue,uOwner FROM tClientConfig WHERE uParameter=1");
	mysql_query(&gMysql,gcQuery);
	
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s",mysql_error(&gMysql));
		exit(1);
	}
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tUser SET uClient=%s WHERE cLogin='%s'",field[1],field[0]);
		//printf("%s\n",gcQuery);
		mysql_query(&mysqlMail2Db,gcQuery);
		if(mysql_errno(&mysqlMail2Db))
		{
			fprintf(stderr,"%s",mysql_error(&mysqlMail2Db));
			exit(1);
		}
	}//outer loop

}//void mysqlMailuClientUpdate(void)


void mysqlApache2uClientUpdate(void)
{
	//This function will set tUser.uClient value using tInstance.uOwner value

	MYSQL mysqlApache2Db;
	
	MYSQL_RES *res;
	MYSQL_ROW field;

	MYSQL_RES *res2;
	MYSQL_ROW field2;

	TextConnectDb();

        mysql_init(&mysqlApache2Db);
	if(!mysql_real_connect(&mysqlApache2Db,NULL,"mysqlapache2","wsxedc","mysqlapache2",0,0,0))
	{
                fprintf(stderr,"mysqlApache2 db unavailable.\n");
		mysql_close(&mysqlApache2Db);
		exit(1);
	}

	//Outer loop, the user login
	sprintf(gcQuery,"SELECT cValue,uOwner FROM tClientConfig WHERE uParameter=20");
	mysql_query(&gMysql,gcQuery);
	
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s",mysql_error(&gMysql));
		exit(1);
	}
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tSite SET uClient=%s WHERE cDomain='%s'",field[1],field[0]);
		printf("%s\n",gcQuery);
		mysql_query(&mysqlApache2Db,gcQuery);
		if(mysql_errno(&mysqlApache2Db))
		{
			fprintf(stderr,"%s",mysql_error(&mysqlApache2Db));
			exit(1);
		}
		sprintf(gcQuery,"SELECT uSite FROM tSite WHERE cDomain='%s'",field[0]);
		mysql_query(&mysqlApache2Db,gcQuery);
		if(mysql_errno(&mysqlApache2Db))
		{
			fprintf(stderr,"%s",mysql_error(&mysqlApache2Db));
			exit(1);
		}
		res2=mysql_store_result(&mysqlApache2Db);
		if((field2=mysql_fetch_row(res2)))
		{
			sprintf(gcQuery,"UPDATE tSiteUser SET uClient=%s WHERE uSite=%s",field[1],field2[0]);
			mysql_query(&mysqlApache2Db,gcQuery);
			if(mysql_errno(&mysqlApache2Db))
			{
				fprintf(stderr,"%s",mysql_error(&mysqlApache2Db));
				exit(1);
			}
		}
		else
			printf("Warning: %s site not found\n",field[0]);
		mysql_free_result(res2);
	}//outer loop


}//void mysqlApache2uClientUpdate(void)


void mysqlRadius2uClientUpdate(void)
{
	//This function will set tUser.uClient value using tInstance.uOwner value

	MYSQL mysqlRadius2Db;
	
	MYSQL_RES *res;
	MYSQL_ROW field;

	TextConnectDb();

        mysql_init(&mysqlRadius2Db);
	if(!mysql_real_connect(&mysqlRadius2Db,NULL,"mysqlradius2","wsxedc","mysqlradius2",0,0,0))
	{
                fprintf(stderr,"mysqlRadius2 db unavailable.\n");
		mysql_close(&mysqlRadius2Db);
		exit(1);
	}

	//Outer loop, the user login
	sprintf(gcQuery,"SELECT cValue,uOwner FROM tClientConfig WHERE uParameter=5");
	mysql_query(&gMysql,gcQuery);
	
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s",mysql_error(&gMysql));
		exit(1);
	}
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tUser SET uClient=%s WHERE cLogin='%s'",field[1],field[0]);
		printf("%s\n",gcQuery);
		mysql_query(&mysqlRadius2Db,gcQuery);
		if(mysql_errno(&mysqlRadius2Db))
		{
			fprintf(stderr,"%s",mysql_error(&mysqlRadius2Db));
			exit(1);
		}
	
	}//outer loop


}//void mysqlRadius2uClientUpdate(void)


void iDNSuClientUpdate(void)
{
	//This function will set tUser.uClient value using tInstance.uOwner value

	MYSQL iDNSDb;
	
	MYSQL_RES *res;
	MYSQL_ROW field;

	TextConnectDb();

        mysql_init(&iDNSDb);
	if(!mysql_real_connect(&iDNSDb,NULL,"idns","wsxedc","idns",0,0,0))
	{
                fprintf(stderr,"iDNS db unavailable.\n");
		mysql_close(&iDNSDb);
		exit(1);
	}

	//Outer loop, the user login
	sprintf(gcQuery,"SELECT cValue,uOwner FROM tClientConfig WHERE uParameter=16");
	mysql_query(&gMysql,gcQuery);
	
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s",mysql_error(&gMysql));
		exit(1);
	}
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tZone SET uClient=%s WHERE cZone='%s'",field[1],field[0]);
		printf("%s\n",gcQuery);
		mysql_query(&iDNSDb,gcQuery);
		if(mysql_errno(&iDNSDb))
		{
			fprintf(stderr,"%s",mysql_error(&iDNSDb));
			exit(1);
		}
	
	}//outer loop


}//void iDNSuClientUpdate(void)


