/*
FILE
	svn ID removed
PURPOSE
	Included in main.c. For command line interface and html main link.

AUTHOR
	Template and mysqlRAD2 author: 
	(C) 2001-2009 Gary Wallis and hugo Urquiza.
 
*/

#include "local.h"
#include "mail.h"

char *strptime(const char *s, const char *format, struct tm *tm);

static char cTableList[64][32]={"tAccess","tAlias","tAuthorize","tBlackList","tClient","tConfigSpec","tConfiguration","tDomain","tGlossary","tHoldUser","tJob","tJobStatus","tLocal","tLog","tLogMonth","tLogType","tMonth","tRelay","tRequest","tServer","tServerConfig","tServerGroup","tServerGroupGlue","tStatus","tTemplate","tTemplateSet","tTemplateType","tUser","tUserConfig","tUserType","tVUT","tVUTEntries","tVacation","tWhiteList",""};


char cInitTableList[64][32]={"tAuthorize","tClient","tConfigSpec","tConfiguration","tDomain","tJobStatus","tLogType","tServer","tServerGroup","tServerGroupGlue","tStatus","tTemplate","tTemplateSet","tTemplateType","tUserType","tGlossary",""};

void ExtMainShell(int argc, char *argv[]);
void Initialize(char *cPasswd);
void Backup(char *cPasswd);
void Restore(char *cPasswd, char *cTableName);
void RestoreAll(char *cPasswd);
void mySQLRootConnect(char *cPasswd);
void ImportTemplateFile(char *cTemplate, char *cFile, char *cTemplateSet);
void ExtracttLog(char *cMonth, char *cYear, char *cPasswd, char *cTablePath);
time_t cDateToUnixTime(char *cDate);
void CreatetLogTable(char *cTableName);
void NextMonthYear(char *cMonth,char *cYear,char *cNextMonth,char *cNextYear);

void CalledByAlias(int iArgc,char *cArgv[]);
void TextConnectDb(void);
void DashBoard(const char *cOptionalMsg);
void EncryptPasswdMD5(char *pw);
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServerGroup,char *cJobData,
		unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy);

void ActiveUser(char *cLogin);
void ExtDashboardSelect(char *cTable,char *cVarList,char *cCondition, char *cOrderBy, unsigned uMaxResults);
//mail.c
void ProcessJobQueue(const char *cServer);
void CrontabConnectDb(void);
void ProcessExtJobQueue(const char *cServer);
void CleanLocalAccess(unsigned uServer);
unsigned GetuServer(const char *cLabel);
void UpdateSystemPasswd(void);
void FixUID(unsigned uServer);
int MakeAllUsers(unsigned uServer);
void FixMboxPerms(unsigned uServer);
void PrepHomeDirs(unsigned uServer);

int iExtMainCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"MainTools"))
	{
		if(!strcmp(gcCommand,"Dashboard"))
		{
			unxsMail("DashBoard");
		}
	}
	return(0);
}


void DashBoard(const char *cOptionalMsg)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	time_t luClock;
	char cConfigBuffer[256]={""};
	char cURL[256]={""};

	//To handle error messages etc.
	if(cOptionalMsg[0] && strcmp(cOptionalMsg,"DashBoard"))
	{
		printf("%s\n",cOptionalMsg);
		return;
	}

	OpenFieldSet("Dashboard",100);
	
	GetConfiguration("DashGraph0",cConfigBuffer,0,0);//any server, txt error msg
	if(cConfigBuffer[0])
	{
		OpenRow("DashGraphs","black");
		printf("</td></tr>\n");
		GetConfiguration("DashGraph0URL",cURL,0,0);
		if(cConfigBuffer[0] && cURL[0])
			printf("<tr><td></td><td colspan=3><a href=%s><img src=%s border=0></a>\n",
					cURL,cConfigBuffer);
		else if(cConfigBuffer[0])
			printf("<tr><td></td><td colspan=3><img src=%s>\n",cConfigBuffer);
		GetConfiguration("DashGraph1",cConfigBuffer,0,0);
		GetConfiguration("DashGraph1URL",cURL,0,0);
		if(cConfigBuffer[0] && cURL[0])
			printf("<a href=%s><img src=%s border=0></a>\n",cURL,cConfigBuffer);
		else if(cConfigBuffer[0])
			printf("<img src=%s>\n",cConfigBuffer);
		printf("</td>");
	}

	OpenRow("System Messages (Last 20)","black");
	ExtDashboardSelect("tLog","cMessage,GREATEST(tLog.uCreatedDate,tLog.uModDate),cServer","uLogType=4",
				"GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC",20);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        mysqlRes=mysql_store_result(&gMysql);
	printf("</td></tr>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[1],"%lu",&luClock);
		printf("<td></td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[2]);
	}
	mysql_free_result(mysqlRes);


	OpenRow("tLog (Last 20)","black");
	ExtDashboardSelect("tLog,tLogType","tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),tLog.cLogin,tLog.cTableName,tLog.cHost,tLogType.cLabel",
				"tLog.uLogType=tLogType.uLogType AND tLog.uLogType!=4 AND tLog.uLogType!=6",
				" GREATEST(tLog.uCreatedDate,tLog.uModDate)",20);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        mysqlRes=mysql_store_result(&gMysql);
	printf("</td></tr>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[1],"%lu",&luClock);
		printf("<td></td><td>%s</td><td>%s %s</td><td>%s %s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[3],mysqlField[5],mysqlField[2],mysqlField[4]);
	}
	mysql_free_result(mysqlRes);

	//login/logout activity
	OpenRow("Login Activity (Last 20)","black");
	ExtDashboardSelect("tLog,tLogType","tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),tLog.cServer,tLog.cHost,tLogType.cLabel",
				"tLog.uLogType=tLogType.uLogType AND tLog.uLogType>=6","GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC",20);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        mysqlRes=mysql_store_result(&gMysql);
	printf("</td></tr>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[1],"%lu",&luClock);
		printf("<td></td><td>%s</td><td>%s %s</td><td>%s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[4],mysqlField[2],mysqlField[3]);
	}
	mysql_free_result(mysqlRes);
	
	OpenRow("Pending or Stuck Jobs (Last 20)","black");
	ExtDashboardSelect("tJob,tJobStatus","tJob.cLabel,GREATEST(tJob.uCreatedDate,tJob.uModDate),tJob.cServer,tJobStatus.cLabel",
				"tJob.uJobStatus=tJobStatus.uJobStatus AND tJob.uJobStatus!=3","GREATEST(tJob.uCreatedDate,tJob.uModDate) DESC",20);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        mysqlRes=mysql_store_result(&gMysql);
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
	printf("<td>New RAD3 based sendmail and postfix controller as well as configuration generator for all mail services across multiple servers and multiple mail domains. For ESMTP, pop3, imap, webmail, clamAV, Spamassassin, procmail, etc. Replaces older tech mysqlSendmail and mysqlPostfix.</td></tr>\n");

	if(guPermLevel>9)
	{
		register unsigned int i;
		OpenRow("Table List","black");
		printf("<td>\n");
		for(i=0;cTableList[i][0];i++)
			printf("<a href=unxsMail.cgi?gcFunction=%.32s>%.32s</a><br>\n",
				cTableList[i],cTableList[i]);
		printf("</td></tr>\n");
	
        	//OpenRow("Admin Functions","black");
		//printf("<td><input type=hidden name=gcFunction value=MainTools>\n");
		//printf(" <input class=largeButton type=submit name=gcCommand value=Dashboard></td></tr>\n");
	}

	CloseFieldSet();

}//void ExtMainContent(void)


//If called from command line
void ExtMainShell(int argc, char *argv[])
{
	if(getuid())
	{
		printf("Only root can run this command!\n");
		exit(0);
	}

	if(argc==3 && !strcmp(argv[1],"ProcessJobQueue"))
                ProcessJobQueue(argv[2]);
	if(argc==3 && !strcmp(argv[1],"ProcessExtJobQueue"))
		ProcessExtJobQueue(argv[2]);
	else if(argc==3 && !strcmp(argv[1],"ActiveUser"))
                ActiveUser(argv[2]);
	else if(argc==5 && !strcmp(argv[1],"ImportTemplateFile"))
                ImportTemplateFile(argv[2],argv[3],argv[4]);
	else if(argc==3 && !strcmp(argv[1],"Initialize"))
                Initialize(argv[2]);
        else if(argc==3 && !strcmp(argv[1],"Backup"))
                Backup(argv[2]);
        else if(argc==4 && !strcmp(argv[1],"Restore"))
                Restore(argv[2],argv[3]);
        else if(argc==3 && !strcmp(argv[1],"RestoreAll"))
                RestoreAll(argv[2]);
	else if(argc==6 && !strcmp(argv[1],"ExtracttLog"))
               	ExtracttLog(argv[2],argv[3],argv[4],argv[5]);
	else if(argc==2 && !strcmp(argv[1],"CleanLocalAccess"))
	{
		unsigned uServer=0;
		TextConnectDb();
		uServer=GetuServer(argv[2]);

		if(!uServer)
		{
			printf("%s not found in tServer. Aborting\n",argv[2]);
			exit(1);
		}
		CleanLocalAccess(uServer);
		exit(0);
	}
	else if(argc==2 && !strcmp(argv[1],"UpdateSystemPasswd"))
	{
		UpdateSystemPasswd();
		exit(0);
	}
	else if(argc==3 && !strcmp(argv[1],"FixUID"))
	{
		unsigned uServer=0;
		TextConnectDb();
		uServer=GetuServer(argv[2]);

		FixUID(uServer);
		exit(0);
	}
	else if(argc==3 && !strcmp(argv[1],"FixMboxPerms"))
	{
		unsigned uServer=0;
		TextConnectDb();
		uServer=GetuServer(argv[2]);

		FixMboxPerms(uServer);
		exit(0);
	}
	else if(argc==3 && !strcmp(argv[1],"MakeAllUsers"))
	{
		unsigned uServer=0;
		TextConnectDb();
		uServer=GetuServer(argv[2]);

		MakeAllUsers(uServer);
	}
	else if(argc==3 && !strcmp(argv[1],"PrepHomeDirs"))
	{
		unsigned uServer=0;
		TextConnectDb();
		uServer=GetuServer(argv[2]);

		PrepHomeDirs(uServer);
	}
        else
	{
		printf("\n%s %s Menu\n\nDatabase Ops:\n",argv[0],RELEASE);
		printf("\tInitialize|Backup|RestoreAll <mysql root passwd>\n");
		printf("\tRestore <mysql root passwd> <Restore table name>\n");
		printf("\tUpdateSystemPasswd\n");
		printf("\tFixUID <cServer>\n");
		printf("\tMakeAllUsers <cServer>\n");
		printf("\tFixMboxPerms <cServer>\n");
		printf("\tPrepHomeDirs <cServer>\n");

		printf("\nCrontab Ops:\n");
		printf("\tProcessJobQueue <cServer>\n");
		printf("\tProcessExtJobQueue <cServer>\n");
		printf("\tCleanLocalAccess <cServer>\n");
		printf("\nSpecial Admin Ops:\n");
		printf("\tImportTemplateFile <mysql root passwd> <tTemplate.cLabel> <filespec> <tTemplateSet.cLabel>\n");
		printf("\tExtracttLog <Mon> <Year> <mysql root passwd> <path to mysql table>\n");
		printf("\tActiveUser <tUser.cLogin..uDomain>\n");
		printf("\n");
	}
        exit(0);


}//void ExtMainShell(int argc, char *argv[])


//ProjectFunctionStubs()


void RestoreAll(char *cPasswd)
{
	char cISMROOT[256]={""};
	register int i;

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		printf("You must set ISMROOT env var first. Ex. (bash) export ISMROOT=/home/ism-3.0\n");
		exit(1);
	}

	printf("Restoring unxsMail data from .txt file in %s/unxsMail/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	for(i=0;cTableList[i][0];i++)
	{
sprintf(gcQuery,"LOAD DATA LOCAL INFILE '%s/unxsMail/data/%s.txt' REPLACE INTO TABLE %s",cISMROOT,cTableList[i],cTableList[i]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		printf("%s\n",cTableList[i]);
	}

	printf("\nDone\n");

}//void RestoreAll(char *cPasswd)


void Restore(char *cPasswd, char *cTableName)
{
	char cISMROOT[256]={""};

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		printf("You must set ISMROOT env var first. Ex. (bash) export ISMROOT=/home/ism-3.0\n");
		exit(1);
	}

	printf("Restoring unxsMail data from .txt file in %s/unxsMail/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	sprintf(gcQuery,"LOAD DATA LOCAL INFILE '%s/unxsMail/data/%s.txt' REPLACE INTO TABLE %s",cISMROOT,cTableName,cTableName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("%s\n\nDone\n",cTableName);

}//void Restore(char *cPasswd, char *cTableName)


void Backup(char *cPasswd)
{
	register int i;
	char cISMROOT[256]={""};

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		printf("You must set ISMROOT env var first. Ex. (bash) export ISMROOT=/home/ism-3.0\n");
		exit(1);
	}

	printf("Backing up unxsMail data to .txt files in %s/unxsMail/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	for(i=0;cTableList[i][0];i++)
	{
		char cFileName[300];

		sprintf(cFileName,"%s/unxsMail/data/%s.txt"
				,cISMROOT,cTableList[i]);
		unlink(cFileName);

sprintf(gcQuery,"SELECT * INTO OUTFILE '%s' FROM %s",cFileName,cTableList[i]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		printf("%s\n",cTableList[i]);
	}


	printf("\nDone.\n");

}//void Backup(char *cPasswd)


void Initialize(char *cPasswd)
{
	char cISMROOT[256]={""};
	register int i;

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		printf("You must set ISMROOT env var first. Ex. (bash) export ISMROOT=/home/ism-3.0\n");
		exit(1);
	}

	printf("Creating db and setting permissions, installing data from %s/unxsMail...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	//Create database
	sprintf(gcQuery,"CREATE DATABASE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("0-%s\n",mysql_error(&gMysql));
		exit(1);
	}

	//Grant access privileges.
	sprintf(gcQuery,"GRANT ALL ON %s.* to %s@localhost IDENTIFIED BY '%s'",
							DBNAME,DBLOGIN,DBPASSWD);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("1-%s\n",mysql_error(&gMysql));
		exit(1);
	}
	
	//Change to mysqlbind db. Then initialize some tables with needed data
	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("2-%s\n",mysql_error(&gMysql));
		exit(1);
	}
	
	
	//Create tables and install default data
	CreatetAccess();
	CreatetAlias();
	CreatetAuthorize();
	CreatetBlackList();
	CreatetClient();
	CreatetDomain();
	CreatetHoldUser();
	CreatetLocal();
	CreatetRelay();
	CreatetServerConfig();
	CreatetUser();
	CreatetUserConfig();
	CreatetVUT();
	CreatetVUTEntries();
	CreatetWhiteList();
        CreatetConfigSpec();
        CreatetConfiguration();
        CreatetGlossary();
        CreatetJob();
        CreatetJobStatus();
        CreatetLog();
        CreatetLogMonth();
        CreatetLogType();
        CreatetMonth();
        CreatetRequest();
        CreatetServer();
        CreatetServerGroup();
        CreatetServerGroupGlue();
        CreatetStatus();
        CreatetTemplate();
        CreatetTemplateSet();
        CreatetTemplateType();
        CreatetUserType();
	
        for(i=0;cInitTableList[i][0];i++)
        {
                sprintf(gcQuery,"LOAD DATA LOCAL INFILE '%s/unxsMail/data/%s.txt' REPLACE INTO TABLE %s",
				cISMROOT,cInitTableList[i],cInitTableList[i]);
                mysql_query(&gMysql,gcQuery);
                if(mysql_errno(&gMysql))
                {
                        printf("%s\n",mysql_error(&gMysql));
                        exit(1);
                }
        }

        printf("Done\n");

}//void Initialize(void)


void mySQLRootConnect(char *cPasswd)
{
        mysql_init(&gMysql);
	if (!mysql_real_connect(&gMysql,DBIP0,"root",cPasswd,"mysql",0,NULL,0)) 
        {
		if (!mysql_real_connect(&gMysql,DBIP1,"root",cPasswd,"mysql",0,NULL,0)) 
		{
	                printf("Database server unavailable\n");
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}
        }
}//void mySQLRootConnect(void)


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
	sprintf(gcQuery,"SELECT uTemplate FROM tTemplate WHERE cLabel='%s'",cTemplate);
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
		sprintf(cBuffer,"UPDATE tTemplate SET uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()),cTemplate='',uTemplateSet=%u WHERE uTemplate=%u",uTemplateSet,uTemplate);
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
		sprintf(cBuffer,"INSERT INTO tTemplate SET uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW()),cLabel='%s',uTemplateSet=%u",cTemplate,uTemplateSet);
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
	if(strstr(cArgv[0],"unxsMailRSS.xml"))
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
		sprintf(cLinkStart,"%s://%s/cgi-bin/unxsMail.cgi",cHTTP,gcHost);

		printf("Content-type: text/xml\n\n");

		//Open xml
		printf("<?xml version='1.0' encoding='UTF-8'?>\n");
		printf("<rss version='2.0'>\n");
		printf("<channel>\n");
		printf("<title>unxsMail RSS tJob Errors</title>\n");
		printf("<link>http://openisp.net/unxsMail</link>\n");
		printf("<description>unxsMail tJob Errors</description>\n");
		printf("<lastBuildDate>%.199s</lastBuildDate>\n",cRSSDate);
		printf("<generator>unxsMail RSS Generator</generator>\n");
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
			printf("<title>unxsMail.tJob.uJob=%s</title>\n",field[0]);
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


void TextConnectDb(void)
{
        mysql_init(&gMysql);
	if(!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
	{
		if (!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
		{
	                fprintf(stderr,"Database server unavailable.\n");
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}
	}

}//end of TextConnectDb()


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
	mysql_query(&gMysql,"USE idns");
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


time_t cDateToUnixTime(char *cDate)
{
        struct  tm locTime;
        time_t  res;

        bzero(&locTime, sizeof(struct tm));
	if(strchr(cDate,'-'))
        	strptime(cDate,"%d-%b-%Y", &locTime);
	else if(strchr(cDate,'/'))
        	strptime(cDate,"%d/%b/%Y", &locTime);
	else if(strchr(cDate,' '))
        	strptime(cDate,"%d %b %Y", &locTime);
        locTime.tm_sec = 0;
        locTime.tm_min = 0;
        locTime.tm_hour = 0;
        res = mktime(&locTime);
        return(res);
}//time_t cDateToUnixTime(char *cDate)


//Another schema dependent item
void CreatetLogTable(char *cTableName)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS %s ( uTablePK VARCHAR(32) NOT NULL DEFAULT '', cHost VARCHAR(32) NOT NULL DEFAULT '', uLoginClient INT UNSIGNED NOT NULL DEFAULT 0, cLogin VARCHAR(32) NOT NULL DEFAULT '', uPermLevel INT UNSIGNED NOT NULL DEFAULT 0, cTableName VARCHAR(32) NOT NULL DEFAULT '', uLog INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(64) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cHash VARCHAR(32) NOT NULL DEFAULT '', uLogType INT UNSIGNED NOT NULL DEFAULT 0,index (uLogType) )",cTableName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
}//CreatetLogTable()


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

	printf("\nPlease report this unxsMail fatal error ASAP:\n%s\n",cError);

	//Attempt to report error in tLog
        sprintf(cQuery,"INSERT INTO tLog SET cLabel='TextError',uLogType=4,uPermLevel=%u,uLoginClient=%u,cLogin='%s',cHost='%s',cMessage=\"%s\",cServer='%s',uOwner=1,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",guPermLevel,guLoginClient,gcUser,gcHost,cError,gcHostname,guLoginClient);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		printf("Another error occurred while attempting to log: %s\n",
				mysql_error(&gMysql));
	if(!uContinue) exit(0);

}//void TextError(const char *cError, unsigned uContinue)

//Passwd stuff
static unsigned char itoa64[] =         /* 0 ... 63 => ascii - 64 */
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void to64(s, v, n)
  register char *s;
  register long v;
  register int n;
{
    while (--n >= 0) {
        *s++ = itoa64[v&0x3f];
        v >>= 6;
    }
}//void to64(s, v, n)


void EncryptPasswdMD5(char *pw)
{
	char cSalt[] = "$1$01234567$";
        char *cpw;

    	(void)srand((int)time((time_t *)NULL));
    	to64(&cSalt[3],rand(),8);
	
	cpw = crypt(pw,cSalt);
	strcpy(pw,cpw);

}//void EncryptPasswdMD5(char *pw)
//End passwd stuff ;)


void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServerGroup,char *cJobData,
		unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy)
{
	char cLabel[100]={""};
	char cServer[100]={""};

        MYSQL_RES *mysqlRes;
        MYSQL_RES *mysqlRes2;
	MYSQL_ROW mysqlField2;

	sprintf(cLabel,"%.32s %.32s %.32s",cJobName,cDomain,cLogin);
	
	//Outside loop for each cServer of cServerGroup
	sprintf(gcQuery,"SELECT tServer.cLabel FROM tServer,tServerGroup,tServerGroupGlue WHERE "
			"tServerGroup.cLabel='%s' AND tServer.uServer=tServerGroupGlue.uServer AND "
			"tServerGroupGlue.uServerGroup=tServerGroup.uServerGroup",cServerGroup);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(gcQuery);
	mysqlRes2=mysql_store_result(&gMysql);
        while((mysqlField2=mysql_fetch_row(mysqlRes2)))
	{
		sprintf(cServer,"%.99s",mysqlField2[0]);


		//If same pending job exists do not add another
		sprintf(gcQuery,"SELECT uJob FROM tJob WHERE cLabel='%.99s' AND cServer='%.64s' AND "
				"cJobName='%.32s' AND uJobTarget=%u AND uJobTargetUser=%u AND cJobData='%.254s' "
				"AND uJobStatus=1 AND uOwner=%u AND uCreatedBy=%u",
				cLabel,cServer,cJobName,uJobTarget,uJobTargetUser,cJobData,uOwner,uCreatedBy);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql)) htmlPlainTextError(gcQuery);
        
		mysqlRes=mysql_store_result(&gMysql);
		if(mysql_num_rows(mysqlRes)==0)
		{
			sprintf(gcQuery,"INSERT INTO tJob SET cLabel='%.99s',cServer='%.64s',cJobName='%.32s',"
					"uJobTarget=%u,uJobTargetUser=%u,cJobData='%.254s',uJobDate=UNIX_TIMESTAMP(NOW()),"
					"uJobStatus=1,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					cLabel,cServer,cJobName,uJobTarget,uJobTargetUser,cJobData,uOwner,uCreatedBy);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql)) htmlPlainTextError(gcQuery);
		}
		else
		{
			MYSQL_ROW mysqlField;

	        	if((mysqlField=mysql_fetch_row(mysqlRes)))
			{
				sprintf(gcQuery,"UPDATE tJob SET uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJob=%s",
						uCreatedBy,mysqlField[0]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql)) htmlPlainTextError(gcQuery);
			}
		}
		mysql_free_result(mysqlRes);
	}
	mysql_free_result(mysqlRes2);

}//void SubmitJob()


void ActiveUser(char *cInput)
{
	MYSQL_RES *res;

	char cLogin[100]={""};
	unsigned  uDomain=0;
	unsigned uActiveUser=0;
	char *cp;



	TextConnectDb();

	sprintf(cLogin,"%.99s",cInput);
	if((cp=strstr(cLogin,"..")))
	{
		*cp=0;
		sscanf(cp+2,"%u",&uDomain);
	}

	//debug only
	//printf("cLogin=%s\nuDomain=%u\n",cLogin,uDomain);
	//exit(0);
	//
	if(!uDomain)
		goto ExitPoint1;

	sprintf(gcQuery,"SELECT uUser FROM tUser WHERE (uStatus=%u OR uStatus=%u) AND cLogin='%s' AND uDomain=%u",STATUS_PENDING_MOD,STATUS_ACTIVE,cLogin,uDomain);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(2);
	}
       	res=mysql_store_result(&gMysql);
       	if(mysql_num_rows(res)==1)
		uActiveUser++;
       	mysql_free_result(res);

	if(uActiveUser)
	{
		if( getpwnam(cInput)==NULL )
		{
			printf("NOTACTIVEUSER %s (getpwnam())\n",cInput);
			exit(1);
		}

		printf("%s\n",cInput);
		exit(0);
	}
	else
	{
ExitPoint1:
		printf("NOTACTIVEUSER %s\n",cInput);
		exit(1);
	}

}//void ActiveUser(char *cInput)


void ExtDashboardSelect(char *cTable,char *cVarList,char *cCondition, char *cOrderBy, unsigned uMaxResults)
{
	char *cp;
	char cLocalTable[100]={""};

	sprintf(cLocalTable,"%.99s",cTable);

	if((cp=strchr(cLocalTable,','))) *cp=0;
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
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
