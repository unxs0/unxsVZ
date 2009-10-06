/*
FILE
	$Id$
PURPOSE
	Included in main.c. For command line interface and html main link.

AUTHOR/LEGAL
	(C) 2001-2009 Gary Wallis for Unixservice, LLC.
	This software distributed under the GPLv2 license.
 
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

#include "local.h"
char *strptime(const char *s, const char *format, struct tm *tm);

static char cTableList[64][32]={ "tAuthorize", "tClient", "tConfig", "tConfiguration", "tContainer",
		"tDatacenter", "tGlossary", "tGroup", "tGroupGlue", "tGroupType", "tIP", "tJob",
		"tJobStatus", "tLog", "tLogMonth", "tLogType", "tMonth", "tNameserver", "tNode",
		"tOSTemplate", "tProperty", "tSearchdomain", "tStatus", "tTemplate", "tTemplateSet",
			"tTemplateType", "tType", ""};

char cInitTableList[64][32]={ "tAuthorize", "tClient", "tConfig", "tGlossary", "tGroupType",
		"tJobStatus", "tLogType", "tOSTemplate", "tStatus", "tTemplate", "tTemplateSet",
			"tTemplateType", "tType", ""};

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
void GetConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer,
		unsigned uHtml);
void UpdateSchema(void);

//jobqueue.c
void ProcessJobQueue(void);


int iExtMainCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"MainTools"))
	{
		if(!strcmp(gcCommand,"Dashboard"))
		{
			unxsVZ("DashBoard");
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

	long unsigned luTotalFailcnt=1;
	long unsigned luFailcnt=0;
	long unsigned luTotalUsage=1,luTotalSoftLimit=1;
	double fRatio;
	char *cColor={""};

	//To handle error messages etc.
	if(cOptionalMsg[0] && strcmp(cOptionalMsg,"DashBoard"))
	{
		printf("%s\n",cOptionalMsg);
		return;
	}

	OpenFieldSet("Dashboard",100);

	GetConfiguration("DashGraph0",cConfigBuffer,0,0,0,0);//any server, txt error msg
	if(cConfigBuffer[0])
	{
		char cURL[256]={""};

		OpenRow("DashGraphs","black");
		printf("</td></tr>\n");

		GetConfiguration("DashGraph0URL",cURL,0,0,0,0);
		if(cConfigBuffer[0] && cURL[0])
			printf("<tr><td></td><td colspan=3><a href=%s><img src=%s border=0></a>\n",
					cURL,cConfigBuffer);
		else if(cConfigBuffer[0])
			printf("<tr><td></td><td colspan=3><img src=%s>\n",cConfigBuffer);

		cURL[0]=0;
		cConfigBuffer[0]=0;
		GetConfiguration("DashGraph1",cConfigBuffer,0,0,0,0);
		GetConfiguration("DashGraph1URL",cURL,0,0,0,0);
		if(cConfigBuffer[0] && cURL[0])
			printf("<a href=%s><img src=%s border=0></a>\n",cURL,cConfigBuffer);
		else if(cConfigBuffer[0])
			printf("<img src=%s>\n",cConfigBuffer);

		cURL[0]=0;
		cConfigBuffer[0]=0;
		GetConfiguration("DashGraph2",cConfigBuffer,0,0,0,0);
		GetConfiguration("DashGraph2URL",cURL,0,0,0,0);
		if(cConfigBuffer[0] && cURL[0])
			printf("<a href=%s><img src=%s border=0></a>\n",cURL,cConfigBuffer);
		else if(cConfigBuffer[0])
			printf("<img src=%s>\n",cConfigBuffer);


		printf("</td>");
	}



	//
	OpenRow("Global Cluster Health","black");
	sprintf(gcQuery,"SELECT COUNT(uProperty) FROM tProperty WHERE cName LIKE '%%.luFailDelta'");
	macro_mySQLQueryErrorText
	printf("</td></tr>\n");
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[0],"%lu",&luTotalFailcnt);
	}
	mysql_free_result(mysqlRes);

	sprintf(gcQuery,"SELECT COUNT(uProperty) FROM tProperty WHERE cName LIKE '%%.luFailDelta' AND cValue!='0'");
	macro_mySQLQueryErrorText
	printf("</td></tr>\n");
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[0],"%lu",&luFailcnt);
	}
	mysql_free_result(mysqlRes);
	if(luTotalFailcnt==0) luTotalFailcnt=1;
	fRatio= ((float) luFailcnt / (float) luTotalFailcnt) * 100.00;
	if(fRatio==0.0)
		cColor="green";
	else if(fRatio<1.5)
		cColor="teal";
	else if(fRatio<2.0)
		cColor="yellow";
	else if(fRatio<5.0)
		cColor="fuchsia";
	else if(fRatio>=5.0)
		cColor="red";
	printf("<tr><td></td><td>Current luFailDelta Ratio %2.2f%%</td><td>%lu/%lu</td><td bgcolor=%s colspan=2></td></tr>\n",
		fRatio,luFailcnt,luTotalFailcnt,cColor);
	//


	//
	luTotalFailcnt=1;
	sprintf(gcQuery,"SELECT COUNT(uProperty) FROM tProperty WHERE cName LIKE '%%.luFailcnt'");
	macro_mySQLQueryErrorText
	printf("</td></tr>\n");
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[0],"%lu",&luTotalFailcnt);
	}
	mysql_free_result(mysqlRes);

	//sprintf(gcQuery,"SELECT COUNT(uProperty) FROM tProperty WHERE cName LIKE '%%.luFailcnt' AND cValue!='0'");
	sprintf(gcQuery,"select count(uLog) from tLog where cMessage like '%%>%% %%:%%'"
				" and uCreatedDate>(unix_timestamp(now())-(86400*7))");
	macro_mySQLQueryErrorText
	printf("</td></tr>\n");
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[0],"%lu",&luFailcnt);
	}	
	if(luTotalFailcnt==0) luTotalFailcnt=1;
	mysql_free_result(mysqlRes);
	if(luTotalFailcnt==0) luTotalFailcnt=1;
	fRatio= ((float) luFailcnt / (float) luTotalFailcnt) * 100.00;
	if(fRatio<1.0)
		cColor="green";
	else if(fRatio<2.0)
		cColor="teal";
	else if(fRatio<5.0)
		cColor="yellow";
	else if(fRatio<10.0)
		cColor="fuchsia";
	else if(fRatio>=10.0)
		cColor="red";
	printf("<tr><td></td><td>Last 7 Day <i>failcnt</i> Ratio %2.2f%%</td><td>%lu/%lu</td><td bgcolor=%s colspan=2></td></tr>\n",
		fRatio,luFailcnt,luTotalFailcnt,cColor);
	//


	sprintf(gcQuery,"SELECT SUM(CONVERT(cValue,UNSIGNED)) FROM tProperty WHERE cName='1k-blocks.luUsage'");
	macro_mySQLQueryErrorText
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		if(mysqlField[0]!=NULL)
			sscanf(mysqlField[0],"%lu",&luTotalUsage);
	}
	mysql_free_result(mysqlRes);

	sprintf(gcQuery,"SELECT SUM(CONVERT(cValue,UNSIGNED)) FROM tProperty WHERE cName='1k-blocks.luSoftlimit'");
	macro_mySQLQueryErrorText
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		if(mysqlField[0]!=NULL)
			sscanf(mysqlField[0],"%lu",&luTotalSoftLimit);
	}
	mysql_free_result(mysqlRes);
	if(luTotalSoftLimit==0) luTotalSoftLimit=1;
	fRatio= ((float) luTotalUsage/ (float) luTotalSoftLimit) * 100.00 ;
	if(fRatio<20.00)
		cColor="green";
	else if(fRatio<30.00)
		cColor="teal";
	else if(fRatio<40.00)
		cColor="yellow";
	else if(fRatio<50.00)
		cColor="fuchsia";
	else if(fRatio>=50.00)
		cColor="red";
	printf("<tr><td></td><td>All Container Usage Ratio %2.2f%%</td>"
		"<td>%luG/%luG</td><td bgcolor=%s colspan=2></td></tr>\n",
		fRatio,luTotalUsage/1048576,luTotalSoftLimit/1048576,cColor);
	//
	//


	if(guPermLevel>11 && guLoginClient==1)
	{
		OpenRow("System Messages (Last 10)","black");
		sprintf(gcQuery,"SELECT cMessage,GREATEST(uCreatedDate,uModDate),cServer FROM tLog"
				" WHERE uLogType=4 ORDER BY GREATEST(uCreatedDate,uModDate) DESC LIMIT 10");
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
		sprintf(gcQuery,"SELECT tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),tLog.cLogin,"
				"tLog.cTableName,tLog.cHost,tLogType.cLabel FROM tLog,tLogType WHERE "
				"tLog.uLogType=tLogType.uLogType AND tLog.uLogType!=4 AND tLog.uLogType!=6 "
				"ORDER BY GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC LIMIT 10");
		macro_mySQLQueryErrorText
		printf("</td></tr>\n");
	        while((mysqlField=mysql_fetch_row(mysqlRes)))
		{
			sscanf(mysqlField[1],"%lu",&luClock);
			printf("<tr><td></td><td>%s</td><td>%s %s</td><td>%s %s</td><td>%s</td></tr>\n",
				ctime(&luClock),mysqlField[0],mysqlField[3],mysqlField[5],mysqlField[2],mysqlField[4]);
		}
		mysql_free_result(mysqlRes);

		OpenRow("Login Activity (Last 10)","black");
		sprintf(gcQuery,"SELECT cLabel,GREATEST(uCreatedDate,uModDate),cServer,cHost FROM"
				" tLog WHERE uLogType=6 ORDER BY GREATEST(uCreatedDate,uModDate)"
				" DESC LIMIT 10");
		macro_mySQLQueryErrorText
		printf("</td></tr>\n");
	        while((mysqlField=mysql_fetch_row(mysqlRes)))
		{
			sscanf(mysqlField[1],"%lu",&luClock);
			printf("<td></td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
				ctime(&luClock),mysqlField[0],mysqlField[2],mysqlField[3]);
		}
		mysql_free_result(mysqlRes);
	}
	else
	{
		OpenRow("System Messages (Last 10)","black");
		sprintf(gcQuery,"SELECT cMessage,GREATEST(uCreatedDate,uModDate),cServer FROM tLog"
				" WHERE uLogType=4 AND uOwner=%u ORDER BY GREATEST(uCreatedDate,uModDate)"
				" DESC LIMIT 10",guCompany);
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
		sprintf(gcQuery,"SELECT tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),tLog.cLogin,"
				"tLog.cTableName,tLog.cHost,tLogType.cLabel FROM tLog,tLogType WHERE "
				"tLog.uLogType=tLogType.uLogType AND tLog.uLogType!=4 AND tLog.uLogType!=6 AND"
				" tLog.uOwner=%u ORDER BY GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC LIMIT 10",
						guCompany);
		macro_mySQLQueryErrorText
		printf("</td></tr>\n");
	        while((mysqlField=mysql_fetch_row(mysqlRes)))
		{
			sscanf(mysqlField[1],"%lu",&luClock);
			printf("<tr><td></td><td>%s</td><td>%s %s</td><td>%s %s</td><td>%s</td></tr>\n",
				ctime(&luClock),mysqlField[0],mysqlField[3],mysqlField[5],mysqlField[2],mysqlField[4]);
		}
		mysql_free_result(mysqlRes);

		OpenRow("Login Activity (Last 10)","black");
		sprintf(gcQuery,"SELECT cLabel,GREATEST(uCreatedDate,uModDate),cServer,cHost FROM"
				" tLog WHERE uLogType=6 AND uOwner=%u ORDER BY GREATEST(uCreatedDate,uModDate)"
				" DESC LIMIT 10",guCompany);
		macro_mySQLQueryErrorText
		printf("</td></tr>\n");
	        while((mysqlField=mysql_fetch_row(mysqlRes)))
		{
			sscanf(mysqlField[1],"%lu",&luClock);
			printf("<td></td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
				ctime(&luClock),mysqlField[0],mysqlField[2],mysqlField[3]);
		}
		mysql_free_result(mysqlRes);
	}//end of if(guPermLevel>11 ...)

	OpenRow("Pending or Stuck Jobs (Last 10)","black");
	if(guPermLevel>11 && guLoginClient==1)
		sprintf(gcQuery,"SELECT tJob.cLabel,GREATEST(tJob.uCreatedDate,tJob.uModDate),tNode.cLabel,tJobStatus.cLabel"
			" FROM tJob,tJobStatus,tNode WHERE tJob.uNode=tNode.uNode AND"
			" tJob.uJobStatus=tJobStatus.uJobStatus AND tJob.uJobStatus!=3"
			" AND tJob.uJobStatus!=7 AND tJob.uJobStatus!=6"
			" ORDER BY GREATEST(tJob.uCreatedDate,tJob.uModDate) DESC LIMIT 10");
	else
		sprintf(gcQuery,"SELECT tJob.cLabel,GREATEST(tJob.uCreatedDate,tJob.uModDate),tNode.cLabel,tJobStatus.cLabel"
			" FROM tJob,tJobStatus,tNode WHERE tJob.uNode=tNode.uNode AND"
			" tJob.uJobStatus=tJobStatus.uJobStatus AND tJob.uJobStatus!=3"
			" AND tJob.uJobStatus!=7 AND tJob.uJobStatus!=6 AND tJob.uOwner=%u"
			" ORDER BY GREATEST(tJob.uCreatedDate,tJob.uModDate) DESC LIMIT 10",guCompany);
	macro_mySQLQueryErrorText
	printf("</td></tr>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[1],"%lu",&luClock);
		printf("<tr><td></td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
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
	printf("<td>Manages OpenVZ containers across datacenters and hardware nodes. Flexible and"
		" extensible configuration management. Cloning, migration, duplication and backup"
		" operations supported, across both nodes and datacenters.</td></tr>\n");
	if(guPermLevel>9)
	{
		register unsigned int i;
		OpenRow("Table List","black");
		printf("<td>\n");
		for(i=0;cTableList[i][0];i++)
			printf("<a href=unxsVZ.cgi?gcFunction=%.32s>%.32s</a><br>\n",
				cTableList[i],cTableList[i]);
		printf("</td></tr>\n");
        	OpenRow("Admin Functions","black");
		printf("<td><input type=hidden name=gcFunction value=MainTools>\n");
		printf(" <input class=largeButton type=submit name=gcCommand value=Dashboard></td></tr>\n");
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

        if(argc==2 && !strcmp(argv[1],"ProcessJobQueue"))
                ProcessJobQueue();
        else if(argc==2 && !strcmp(argv[1],"UpdateSchema"))
                UpdateSchema();
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
        else
	{
		printf("\n%s %s Menu\n\nDatabase Ops:\n",argv[0],RELEASE);
		printf("\tInitialize|Backup|RestoreAll <mysql root passwd>\n");
		printf("\tRestore <mysql root passwd> <Restore table name>\n");
		printf("\nCrontab Ops:\n");
		printf("\tProcessJobQueue\n");
		//printf("\tProcessExtJobQueue <cServer>\n");
		printf("\nSpecial Admin Ops:\n");
		printf("\tUpdateSchema\n");
		printf("\tImportTemplateFile <mysql root passwd> <tTemplate.cLabel>"
				" <filespec> <tTemplateSet.cLabel>\n");
		printf("\tExtracttLog <Mon> <Year> <mysql root passwd> <path to mysql table>\n");
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
		printf("You must set ISMROOT env var first. Ex. export ISMROOT=/home/joe\n");
		exit(1);
	}

	printf("Restoring unxsVZ data from .txt file in %s/unxsVZ/data...\n\n",cISMROOT);

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
		sprintf(gcQuery,"LOAD DATA LOCAL INFILE '%s/unxsVZ/data/%s.txt' REPLACE INTO TABLE %s",
				cISMROOT,cTableList[i],cTableList[i]);
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
		printf("You must set ISMROOT env var first. Ex. export ISMROOT=/home/joe\n");
		exit(1);
	}

	printf("Restoring unxsVZ data from .txt file in %s/unxsVZ/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	sprintf(gcQuery,"LOAD DATA LOCAL INFILE '%s/unxsVZ/data/%s.txt' REPLACE INTO TABLE %s",
			cISMROOT,cTableName,cTableName);
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
		printf("You must set ISMROOT env var first. Ex. export ISMROOT=/home/joe\n");
		exit(1);
	}

	printf("Backing up unxsVZ data to .txt files in %s/unxsVZ/data...\n\n",cISMROOT);

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

		sprintf(cFileName,"%s/unxsVZ/data/%s.txt"
				,cISMROOT,cTableList[i]);
		unlink(cFileName);

		sprintf(gcQuery,"SELECT * INTO OUTFILE '%s' FROM %s",
							cFileName,cTableList[i]);
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
		printf("You must set ISMROOT env var first. Ex. export ISMROOT=/home/joe\n");
		exit(1);
	}

	printf("Creating db and setting permissions, installing data from %sunxsVZ...\n\n",
			cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	//Create database
	sprintf(gcQuery,"CREATE DATABASE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	//Grant localaccess privileges.
	sprintf(gcQuery,"GRANT ALL ON %s.* to %s@localhost IDENTIFIED BY '%s'",
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
	

	//Create tables and install default data
	//Standard RAD3 required tables
	CreatetAuthorize();
	CreatetClient();
	CreatetConfig();
        CreatetConfiguration();
	CreatetContainer();
	CreatetDatacenter();
        CreatetGlossary();
	CreatetGroup();
	CreatetGroupGlue();
	CreatetGroupType();
	CreatetIP();
        CreatetJob();
        CreatetJobStatus();
        CreatetLog();
        CreatetLogMonth();
        CreatetLogType();
        CreatetMonth();
	CreatetNameserver();
	CreatetNode();
	CreatetOSTemplate();
	CreatetProperty();
	CreatetSearchdomain();
        CreatetStatus();
        CreatetTemplate();
        CreatetTemplateSet();
        CreatetTemplateType();
	CreatetType();

	//Unique uContainer values starting at 101
	sprintf(gcQuery,"INSERT INTO tContainer SET uContainer=100");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	sprintf(gcQuery,"DELETE FROM tContainer");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

        for(i=0;cInitTableList[i][0];i++)
        {
                sprintf(gcQuery,"LOAD DATA LOCAL INFILE '%s/unxsVZ/data/%s.txt' REPLACE INTO TABLE %s",
			cISMROOT,cInitTableList[i],cInitTableList[i]);
                mysql_query(&gMysql,gcQuery);
                if(mysql_errno(&gMysql))
                {
                        printf("%s.\nAttempting to drop database for re-initialize...\n",mysql_error(&gMysql));

			sprintf(gcQuery,"DROP DATABASE %s",DBNAME);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}

                        exit(1);
                }
        }

        printf("Done\n");

}//void Initialize(void)


void mySQLRootConnect(char *cPasswd)
{
        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,NULL,"root",cPasswd,"mysql",0,NULL,0))
        {
                printf("Database server unavailable\n");
                exit(1);
        }
}//void mySQLRootConnect(void)


void UpdateSchema(void)
{
	printf("\nUpdateSchema(): Start\n");

	TextConnectDb();

	sprintf(gcQuery,"ALTER TABLE tContainer ADD uVeth INT UNSIGNED DEFAULT 0");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	sprintf(gcQuery,"INSERT INTO tStatus SET uStatus=81,cLabel='Awaiting Clone',"
				"uCreatedBy=1,uOwner=1,uCreatedDate=UNIX_TIMESTAMP(NOW())");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	sprintf(gcQuery,"UPDATE tConfiguration SET cLabel='cSSHOptions' WHERE cLabel='cSSLOptions'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	sprintf(gcQuery,"ALTER TABLE tOSTemplate MODIFY cLabel VARCHAR(100) NOT NULL DEFAULT ''");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	sprintf(gcQuery,"ALTER TABLE tContainer ADD uSource INT UNSIGNED DEFAULT 0");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	sprintf(gcQuery,"ALTER TABLE tContainer ADD INDEX (uSource)");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	printf("\nUpdateSchema(): End\n");

}//void UpdateSchema(void)


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
	if(strstr(cArgv[0],"unxsVZRSS.xml"))
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
		sprintf(cLinkStart,"%s://%s/cgi-bin/unxsVZ.cgi",cHTTP,gcHost);

		printf("Content-type: text/xml\n\n");

		//Open xml
		printf("<?xml version='1.0' encoding='UTF-8'?>\n");
		printf("<rss version='2.0'>\n");
		printf("<channel>\n");
		printf("<title>unxsVZ RSS tJob Errors</title>\n");
		printf("<link>http://openisp.net/unxsVZ</link>\n");
		printf("<description>unxsVZ tJob Errors</description>\n");
		printf("<lastBuildDate>%.199s</lastBuildDate>\n",cRSSDate);
		printf("<generator>unxsVZ RSS Generator</generator>\n");
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
			printf("<title>unxsVZ.tJob.uJob=%s</title>\n",field[0]);
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
        if (!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
        {
        	if (!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
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
	long uStart=0;
	long uEnd=0;
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
	sprintf(gcQuery,"INSERT INTO tLog SET cMessage='ExtracttLog() Start...',"
			"cServer='%s',uLogType=4,uOwner=1,uCreatedBy=1,"
			"uCreatedDate=UNIX_TIMESTAMP(NOW())",gcHostname);
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
	sprintf(gcQuery,"INSERT %s (uLog,cLabel,uLogType,cHash,uPermLevel,uLoginClient,cLogin,"
			"cHost,uTablePK,cTableName,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate)"
			" SELECT uLog,cLabel,uLogType,cHash,uPermLevel,uLoginClient,cLogin,cHost,"
			"uTablePK,cTableName,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate"
			" FROM tLog WHERE uCreatedDate>=%lu AND uCreatedDate<%lu",
				cTableName,uStart,uEnd);
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

		sprintf(gcQuery,"REPLACE tMonth SET cLabel='%s',uOwner=1,uCreatedBy=1,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW())",cTableName);
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
	sprintf(gcQuery,"DELETE QUICK FROM tLog WHERE uCreatedDate>=%lu AND uCreatedDate<%lu AND uLogType!=5",
				uStart,uEnd);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}


	printf("Extracted and Archived. Table flushed: %s\n",cTableName);
	printf("ExtracttLog() End\n");
	sprintf(gcQuery,"INSERT INTO tLog SET cMessage='ExtracttLog() End',cServer='%s',uLogType=4,"
			"uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",gcHostname);
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
	printf("\nPlease report this unxsVZ fatal error ASAP:\n%s\n",cError);

	//Attempt to report error in tLog
        sprintf(gcQuery,"INSERT INTO tLog SET cLabel='TextError',uLogType=4,uPermLevel=%u,uLoginClient=%u,"
			"cLogin='%s',cHost='%s',cMessage=\"%s\",cServer='%s',uOwner=1,uCreatedBy=%u,"
			"uCreatedDate=UNIX_TIMESTAMP(NOW())",
			guPermLevel,guLoginClient,gcUser,gcHost,cError,gcHostname,guLoginClient);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		printf("Another error occurred while attempting to log: %s\n",
				mysql_error(&gMysql));
	if(!uContinue) exit(0);

}//void TextError(const char *cError, unsigned uContinue)


void GetConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer,
		unsigned uHtml)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        char cQuery[1024];
	char cExtra[100]={""};

        sprintf(cQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='%s'",
			cName);
	if(uDatacenter)
	{
		sprintf(cExtra," AND uDatacenter=%u",uDatacenter);
		strcat(cQuery,cExtra);
	}
	if(uNode)
	{
		sprintf(cExtra," AND uNode=%u",uNode);
		strcat(cQuery,cExtra);
	}
	if(uContainer)
	{
		sprintf(cExtra," AND uContainer=%u",uContainer);
		strcat(cQuery,cExtra);
	}
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
        	        htmlPlainTextError(mysql_error(&gMysql));
		else
        	        TextError(mysql_error(&gMysql),0);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
        	sprintf(cValue,"%.255s",field[0]);
        mysql_free_result(res);

}//void GetConfiguration(...)

