/*
FILE
	svn ID removed
	(sslauthlogin template set member)
PURPOSE

AUTHOR
	(Template and mysqlRAD2 author: (C) 2002-2009 Gary Wallis and Hugo Urquiza.)
 
*/

#define macro_mySQLQueryErrorText	mysql_query(&gMysql,gcQuery);\
					if(mysql_errno(&gMysql))\
					{\
						printf("%s\n",mysql_error(&gMysql));\
						printf("</td></tr>\n");\
						CloseFieldSet();\
						return;\
					}\
					mysqlRes=mysql_store_result(&gMysql);


static char cTableList[64][32]={"tAuthorize","tClient","tConfiguration","tGlossary","tJob","tJobStatus","tLog","tLogMonth","tLogType","tMonth","tNAS","tNASGroup","tProfile","tProfileName","tServer","tServerGroup","tTemplate","tTemplateSet","tTemplateType","tUser",""};

static char cInitTableList[32][32]={"tClient","tAuthorize","tServer","tNAS","tProfile","tProfileName","tConfiguration","tNASGroup","tUser","tServerGroup","tLogType","tTemplateType",""};

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

void ImportUsers(char *cPasswd);
void ProcessJobQueue(unsigned uTestMode,char *cServer);
void AutoHoldRelease(char *cServer);//radius.c
void UpdateSchema(char *cPasswd);
void ExtDashboardSelect(char *cTable,char *cVarList,char *cCondition, char *cOrderBy, unsigned uMaxResults);

int MakeClientsFile(unsigned uHtml, unsigned uServer);
int MakeNASListFile(unsigned uHtml, unsigned uServer);
unsigned GetuServer(char *cLabel);
int MakeUsersFile(unsigned uHtml, unsigned uServer, char *cLogin);
int ReloadRadius(unsigned uHtml, unsigned uServer,unsigned uStopRequired);
int MakeClientsFile(unsigned uHtml, unsigned uServer);
int MakeNASPasswdFile(unsigned uHtml, unsigned uServer);
void ProcessExtJobQueue(char *cServer);
void ExtConnectDb(unsigned uHtml);
int InformExtJob(char *cRemoteMsg,char *cServer,unsigned uJob,unsigned uJobStatus);
char *strptime(const char *s, const char *format, struct tm *tm);
void to64(char *s, long v, int n);
void GetConfiguration(const char *cName, char *cValue, unsigned uHtml);//radius.c

int CreateClientConf(unsigned uHtml, unsigned uServer);

static int i=0;

int iExtMainCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"MainTools"))
	{
		if(!strcmp(gcCommand,"Dashboard"))
		{
			unxsRadius("DashBoard");
		}
	}

	return(0);
}

void DashBoard(const char *cOptionalMsg)
{

        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	char cConfigBuffer[256]={""};
	time_t luClock;

	//To handle error messages etc.
	if(cOptionalMsg[0] && strcmp(cOptionalMsg,"DashBoard"))
	{
		printf("%s\n",cOptionalMsg);
		return;
	}

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



	//
	OpenRow("Cluster Health","black");


	if(guPermLevel>11)
	{
		OpenRow("System Messages (Last 10)","black");
		sprintf(gcQuery,"SELECT cMessage,GREATEST(uCreatedDate,uModDate),cServer FROM"
				" tLog WHERE uLogType=4 ORDER BY GREATEST(uCreatedDate,uModDate) DESC LIMIT 10");
		macro_mySQLQueryErrorText
		printf("</td></tr>\n");
		while((mysqlField=mysql_fetch_row(mysqlRes)))
		{
			sscanf(mysqlField[1],"%lu",&luClock);
			printf("<tr><td></td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
				ctime(&luClock),mysqlField[0],mysqlField[2]);
		}
		mysql_free_result(mysqlRes);


		OpenRow("tLog (Last 10)","black");
		sprintf(gcQuery,"SELECT tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),"
				"tLog.cLogin,tLog.cTableName,tLog.cHost,tLogType.cLabel FROM"
				" tLog,tLogType WHERE tLog.uLogType=tLogType.uLogType AND"
				" tLog.uLogType!=4 AND tLog.uLogType!=6"
				" ORDER BY GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC LIMIT 10");
		macro_mySQLQueryErrorText
		printf("</td></tr>\n");
		while((mysqlField=mysql_fetch_row(mysqlRes)))
		{
			sscanf(mysqlField[1],"%lu",&luClock);
			printf("<tr><td></td><td>%s</td><td>%s %s</td><td>%s %s</td><td>%s</td></tr>\n",
				ctime(&luClock),mysqlField[0],mysqlField[3],mysqlField[5],mysqlField[2],mysqlField[4]);
		}
		mysql_free_result(mysqlRes);
	}//end of if(guPermLevel>11)

	OpenRow("Login Activity (Last 10)","black");
	if(guPermLevel>11)
		sprintf(gcQuery,"SELECT cLabel,GREATEST(uCreatedDate,uModDate),cServer,cHost"
			" FROM tLog WHERE uLogType=6 ORDER BY GREATEST(uCreatedDate,uModDate) DESC LIMIT 10");
	else
		sprintf(gcQuery,"SELECT cLabel,GREATEST(uCreatedDate,uModDate),cServer,cHost"
			" FROM tLog WHERE uLogType=6 AND uOwner=%u"
			" ORDER BY GREATEST(uCreatedDate,uModDate) DESC LIMIT 10",guCompany);
	macro_mySQLQueryErrorText
	printf("</td></tr>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[1],"%lu",&luClock);
		printf("<td></td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[2],mysqlField[3]);
	}
	mysql_free_result(mysqlRes);

	OpenRow("Pending or Stuck Jobs (Last 10)","black");
	if(guPermLevel>11)
		sprintf(gcQuery,"SELECT tJob.cJobName,GREATEST(tJob.uCreatedDate,tJob.uJobDate),tJob.cServer,"
			"tJobStatus.cLabel,tJob.cLabel FROM tJob,tJobStatus WHERE"
			" tJob.uJobStatus=tJobStatus.uJobStatus AND tJob.uJobStatus!=3"
			" AND tJob.uJobStatus!=7 AND tJob.uJobStatus!=6"
			" ORDER BY GREATEST(tJob.uCreatedDate,tJob.uModDate) DESC LIMIT 10");
	else
		sprintf(gcQuery,"SELECT tJob.cJobName,GREATEST(tJob.uCreatedDate,tJob.uJobDate),tJob.cServer,"
			"tJobStatus.cLabel,tJob.cLabel FROM tJob,tJobStatus WHERE"
			" tJob.uJobStatus=tJobStatus.uJobStatus AND tJob.uJobStatus!=3"
			" AND tJob.uJobStatus!=7 AND tJob.uJobStatus!=6 AND tJob.uOwner=%u"
			" ORDER BY GREATEST(tJob.uCreatedDate,tJob.uModDate) DESC LIMIT 10",guCompany);
	macro_mySQLQueryErrorText
	printf("</td></tr>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[1],"%lu",&luClock);
		printf("<tr><td></td><td>%s</td><td>%s %s</td><td>%s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[4],mysqlField[2],mysqlField[3]);
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
	printf("<td>Mission critical RADIUS server central administration system. With company-contact-role model and support for disjoint sets of RADIUS server configuration and users. Can be controlled via master ISP unxsISP/ispAdmin.</td></tr>\n");

	if(guPermLevel>9)
	{
		register unsigned int i;
		OpenRow("Table List","black");
		printf("<td>\n");
		for(i=0;cTableList[i][0];i++)
			printf("<a href=unxsRadius.cgi?gcFunction=%.32s>%.32s</a><br>\n",
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

        if(argc==3 && !strcmp(argv[1],"ProcessJobQueue"))
	{
		ConnectDb();
                ProcessJobQueue(0,argv[2]);
		exit(0);
	}
	else if(argc==3 && !strcmp(argv[1],"ProcessExtJobQueue"))
	{
		ConnectDb();
                ProcessExtJobQueue(argv[2]);
		exit(0);
	}
        else if(argc==3 && !strcmp(argv[1],"AutoHoldRelease"))
	{
		ConnectDb();
                AutoHoldRelease(argv[2]);
	}
        else if(argc==3 && !strcmp(argv[1],"Start"))
	{
		ConnectDb();
		ReloadRadius(0,GetuServer(argv[2]),0);
		exit(0);
	}
	else if(argc==3 && !strcmp(argv[1],"TestJobQueue"))
	{
		ConnectDb();
                ProcessJobQueue(1,argv[2]);
		exit(0);
	}
	else if(argc==3 && !strcmp(argv[1],"Initialize"))
                Initialize(argv[2]);
        else if(argc==3 && !strcmp(argv[1],"Backup"))
                Backup(argv[2]);
        else if(argc==4 && !strcmp(argv[1],"Restore"))
                Restore(argv[2],argv[3]);
        else if(argc==3 && !strcmp(argv[1],"ImportUsers"))
                ImportUsers(argv[2]);
        else if(argc==3 && !strcmp(argv[1],"RestoreAll"))
                RestoreAll(argv[2]);
        else if(argc==3 && !strcmp(argv[1],"UpdateSchema"))
                UpdateSchema(argv[2]);
        else if(argc==3 && !strcmp(argv[1],"MakeUsers"))
	{
		ConnectDb();
                MakeUsersFile(0,GetuServer(argv[2]),"");
		exit(0);
	}
        else if(argc==3 && !strcmp(argv[1],"MakeClients"))
	{
		ConnectDb();
                MakeClientsFile(0,GetuServer(argv[2]));
		exit(0);
	}
        else if(argc==3 && !strcmp(argv[1],"MakeNASList"))
	{
		ConnectDb();
		CreateClientConf(0,GetuServer(argv[2]));
		exit(0);
	}
        else
	{
		printf("\n%s Menu %s\n",argv[0],RELEASE);

                printf("\nDatabase admin gcCommands\n");
                printf("\tInitialize|Backup|RestoreAll <mysql root passwd>\n");
                printf("\tRestore <mysql root passwd> <Restore table name>\n");
                printf("\tImportUsers <mysql root passwd>\n");
                printf("\tUpdateSchema <mysql root passwd>\n");

                printf("\nRadius file gcCommands\n");
                printf("\tMakeUsers|MakeClients|MakeNASList <servername>\n");

                printf("\nJob queue gcCommands\n");
                printf("\tProcessJobQueue|TestJobQueue <servername>\n");
                printf("\tProcessExtJobQueue <servername>\n");
                printf("\tAutoHoldRelease <servername>\n");

                printf("\nRadius daemon operation gcCommands\n");
                printf("\tStart <servername>\n\n");
	}
        exit(0);

}//void ExtMainShell(int argc, char *argv[])


void RestoreAll(char *cPasswd)
{
	char cISMROOT[256]={""};

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		printf("You must set ISMROOT env var first. Ex. (csh) setenv ISMROOT /home/ism-3.0\n");
	exit(1);
	}

	printf("Restoring mysqlRadius data from .txt file in %s/{{cProject}}/data...\n\n",cISMROOT);

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
sprintf(gcQuery,"load data infile '%s/unxsRadius/data/%s.txt' replace into table %s",cISMROOT,cTableList[i],cTableList[i]);
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


void ImportUsers(char *cPasswd)
{
	char cISMROOT[256]={""};
	time_t clock;
        MYSQL_RES *res;
        MYSQL_ROW field;

	//Help useful if ISMROOT is unset
	printf("Importing mysqlRadius tUser data\n");
	printf("\tFrom $ISMROOT/unxsRadius/data/userimport.txt\n");
	printf("\tWhere this is a , delimited, optionally \"field\" enclosed flat file.\n");
	printf("\tFormat login,clearpasswd,profilenumber,userid.\n");
	printf("And:\n");
	printf("\tFrom $ISMROOT/unxsRadius/data/profileimport.txt\n");
	printf("\tWhere this is a , delimited, optionally \"field\" enclosed flat file.\n");
	printf("\tFormat profilename,profilenumber.\n\n");
	
	time(&clock);

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}
	if(!cISMROOT[0])
	{
		printf("You must set ISMROOT env var first. Ex. (bash) export ISMROOT=/home/joe\n");
		exit(1);
	}


	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("User import\n");
	
sprintf(gcQuery,"LOAD DATA LOCAL INFILE '%s/unxsRadius/data/userimport.txt' REPLACE INTO TABLE tUser FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '\"' (cLogin,cEnterPasswd,uProfileName,uUser)",cISMROOT);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	
sprintf(gcQuery,"UPDATE tUser SET uClearText=1,uSimulUse=1,uOwner=9999,uCreatedBy=1,uCreatedDate=%lu WHERE uCreatedBy=0",clock);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	


	//Remove all assignments
        sprintf(gcQuery,"SELECT uUser FROM tUser WHERE uOwner=9999");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {

		sprintf(gcQuery,"DELETE FROM tServerGroup WHERE uUser=%s",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
        }

	//Assign localhost to all
        sprintf(gcQuery,"SELECT uUser FROM tUser WHERE uOwner=9999");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {

		sprintf(gcQuery,"INSERT INTO tServerGroup SET uUser=%s,uServer=1"
							,field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
        }
        mysql_free_result(res);
	
	/////////
	printf("Profile import\n");
	
sprintf(gcQuery,"LOAD DATA LOCAL INFILE '%s/unxsRadius/data/profileimport.txt' REPLACE INTO TABLE tProfileName FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '\"' (cLabel,uProfileName)",cISMROOT);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	
sprintf(gcQuery,"UPDATE tProfileName SET uOwner=9999,uCreatedBy=1,uCreatedDate=%lu WHERE uCreatedBy=0",clock);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	
	printf("Done\n");

}//void ImportUsers(char *cPasswd)


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
		printf("You must set ISMROOT env var first. Ex. (csh) setenv ISMROOT /home/ism-3.0\n");
	exit(1);
	}

	printf("Restoring mysqlRadius data from .txt file in %s/{{cProject}}/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	sprintf(gcQuery,"load data infile '%s/unxsRadius/data/%s.txt' replace into table %s",cISMROOT,cTableName,cTableName);
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
	char cISMROOT[256]={""};

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		printf("You must set ISMROOT env var first. Ex. (csh) setenv ISMROOT /home/ism-3.0\n");
	exit(1);
	}

	printf("Backing up mysqlRadius data to .txt files in \
%s/unxsRadius/data...\n\n",cISMROOT);

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

		sprintf(cFileName,"%s/unxsRadius/data/%s.txt"
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
	time_t clock;

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		printf("You must set ISMROOT env var first. Ex. (bash)# export ISMROOT=/var/src\n");
		exit(1);
	}

	printf("Creating db and setting permissions, installing data from %s/unxsRadius...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	//Create database as defined in local.h
	sprintf(gcQuery,"CREATE DATABASE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	//Grant localaccess privileges.
	sprintf(gcQuery,"GRANT ALL ON %s.* to %s@localhost identified by '%s'",
						DBNAME,DBLOGIN,DBPASSWD);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	
	//Change to mysqlbind db. Then initialize some tables with needed data
	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	
	
	time(&clock);
	

	//Create tables and install default data
	CreatetAuthorize();
	CreatetClient();
	CreatetConfiguration();
	CreatetGlossary();
	CreatetJob();
	CreatetJobStatus();
	CreatetLog();
	CreatetLogMonth();
	CreatetLogType();
	CreatetMonth();
	CreatetNAS();
	CreatetNASGroup();
	CreatetProfile();
	CreatetProfileName();
	CreatetServer();
	CreatetServerGroup();
	CreatetTemplate();
	CreatetTemplateSet();
	CreatetTemplateType();
	CreatetUser();

        for(i=0;cInitTableList[i][0];i++)
        {
                sprintf(gcQuery,"load data local infile '%s/unxsRadius/data/%s.txt' replace into table %s",cISMROOT,cInitTableList[i],cInitTableList[i]);
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
        if (!mysql_real_connect(&gMysql,DBIP0,"root",cPasswd,"mysql",DBPORT0,DBSOCKET,0))
        {
		if (!mysql_real_connect(&gMysql,DBIP1,"root",cPasswd,"mysql",DBPORT0,DBSOCKET,0))
		{
	                if(mysql_errno(&gMysql))
        	        {
                	        fprintf(stderr,"Database server unavailable: %s\n",mysql_error(&gMysql));
                        	exit(1);
	                }
		}
        }
}//void mySQLRootConnect(void)


void UpdateSchema(char *cPasswd)
{
	printf("Upgrading/updating older version database schema...\n\n");

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	mysql_query(&gMysql,"use unxsradius");
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	//Upgrade tRadacct
	printf("Doing tUser...\n\n");
sprintf(gcQuery,"ALTER TABLE tUser MODIFY cPasswd varchar(64) NOT NULL DEFAULT ''");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("\nDone\n");
	exit(0);

}//void UpdateSchema(char *cPasswd)


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
	if(strstr(cArgv[0],"unxsRadiusRSS.xml"))
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
		sprintf(cLinkStart,"%s://%s/cgi-bin/unxsRadius.cgi",cHTTP,gcHost);

		printf("Content-type: text/xml\n\n");

		//Open xml
		printf("<?xml version='1.0' encoding='UTF-8'?>\n");
		printf("<rss version='2.0'>\n");
		printf("<channel>\n");
		printf("<title>unxsRadius RSS tJob Errors</title>\n");
		printf("<link>http://openisp.net/unxsRadius</link>\n");
		printf("<description>unxsRadius tJob Errors</description>\n");
		printf("<lastBuildDate>%.199s</lastBuildDate>\n",cRSSDate);
		printf("<generator>unxsRadius RSS Generator</generator>\n");
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
			printf("<title>unxsRadius.tJob.uJob=%s</title>\n",field[0]);
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
	if(!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,DBPORT0,DBSOCKET,0))
	{
		if (!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,DBPORT1,DBSOCKET,0))
		{
	                fprintf(stderr,"Database server unavailable.\n");
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

	printf("\nPlease report this unxsRadius fatal error ASAP:\n%s\n",cError);

	//Attempt to report error in tLog
        sprintf(cQuery,"INSERT INTO tLog SET cLabel='TextError',uLogType=4,uPermLevel=%u,uLoginClient=%u,cLogin='%s',cHost='%s',cMessage=\"%s\",cServer='%s',uOwner=1,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",guPermLevel,guLoginClient,gcUser,gcHost,cError,gcHostname,guLoginClient);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		printf("Another error occurred while attempting to log: %s\n",
				mysql_error(&gMysql));
	if(!uContinue) exit(0);

}//void TextError(const char *cError, unsigned uContinue)


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


