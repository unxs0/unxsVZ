/*
FILE
	svn ID removed
PURPOSE
	CLI and main tab related functions.
AUTHOR
	(Template and mysqlRAD2/RAD3 author: (C) 2001-2009 Gary Wallis and Hugo Urquiza.)
*/


static char cTableList[64][32]={"tAuthorize","tClient","tConfiguration","tGlossary","tLog","tLogMonth","tLogType","tMonth","tMonthLog","tRadacct","tRadacctMonth","tServer",""};

static char cInitTableList[64][32]={"tAuthorize","tClient","tConfiguration","tGlossary","tLogType","tServer",""};

void ExtMainShell(int argc, char *argv[]);
void Initialize(char *cPasswd);
void Backup(char *cPasswd);
void Restore(char *cPasswd, char *cTableName);
void RestoreAll(char *cPasswd);
void UpdateSchema(char *cPasswd);
void mySQLRootConnect(char *cPasswd);
void AcctReport(char *cTableName);
void AcctReportLegacy(char *cTableName);
void AcctReportLegacyProxy(char *cRealm,char *cTableName);
void Extract(char *cMonth, char *cYear, char *cPasswd, char *cTablePath);
void Report(char *cMonth, char *cYear, char *cPasswd);
void ReportLegacy(char *cMonth, char *cYear, char *cPasswd);
void ReportLegacyProxy(char *cMonth, char *cYear, char *cRealm, char *cPasswd);
void CurrentMonthReport(char *cPasswd);
void CurrentMonthReportLegacy(char *cPasswd);
void CurrentMonthReportLegacyProxy(char *cRealm, char *cPasswd);
void NoISMROOTMsg(void);
void TextConnectDb(void);
void ExtDashboardSelect(char *cTable,char *cVarList,char *cCondition, char *cOrderBy, unsigned uMaxResults);

//tradacctmonthfunc.h
void CreatetRadacctMonthTable(char *cTableName);
time_t cDateToUnixTime(char *cDate);

//tradacctfunc.h
void UserReport(char *cLogin, char *cTableName);
void HoldUsers(char *cMaxHours, char *cPasswd, unsigned uWarnOnly, char *cTableName);
void TopUsersReport(char *cTableName);


//..mysqlISP/mysqlisp.h
//Instance status only
#define mysqlISP_NeverDeployed 1
#define mysqlISP_WaitingInitial 2
#define mysqlISP_DeployedMod 3
#define mysqlISP_Canceled 5
#define mysqlISP_OnHold 6
#define mysqlISP_WaitingRedeployment 9 
#define mysqlISP_WaitingCancel 11 
#define mysqlISP_WaitingHold 12 
//Job status Only
#define mysqlISP_RemotelyQueued 7
#define mysqlISP_Waiting 10
//Both
#define mysqlISP_Deployed 4
void SubmitServiceJob(unsigned uService,char *cJobName, char *cServer,unsigned uInstance,time_t uJobDate, unsigned uPrevStatus, unsigned uClient);
void SubmitInstanceJob(char *cJobType,time_t uJobDate, unsigned uInstance, unsigned uProduct, unsigned uClient);
unsigned ReaduPrevStatus(unsigned uInstance);
int ExtConnectDb(char *cDbName, char *cDbIp, char *cDbPwd, char *cDbLogin);
MYSQL mysqlext;
static int i=0;

void GetConfiguration(const char *cName, char *cValue, unsigned uHtml);

void ExtMainShell(int argc, char *argv[])
{
	if(argc==3 && !strcmp(argv[1],"Initialize"))
                Initialize(argv[2]);
        else if(argc==3 && !strcmp(argv[1],"Backup"))
                Backup(argv[2]);
        else if(argc==4 && !strcmp(argv[1],"HoldUsers"))
                HoldUsers(argv[2],argv[3],0,"");
        else if(argc==4 && !strcmp(argv[1],"HoldUsersWarnOnly"))
                HoldUsers(argv[2],argv[3],1,"");
        else if(argc==5 && !strcmp(argv[1],"HoldUsersWarnOnly"))
                HoldUsers(argv[2],argv[3],1,argv[4]);
        else if(argc==4 && !strcmp(argv[1],"Restore"))
                Restore(argv[2],argv[3]);
        else if(argc==3 && !strcmp(argv[1],"RestoreAll"))
                RestoreAll(argv[2]);
        else if(argc==6 && !strcmp(argv[1],"Extract"))
                Extract(argv[2],argv[3],argv[4],argv[5]);
        else if(argc==5 && !strcmp(argv[1],"ReportLegacy"))
                ReportLegacy(argv[2],argv[3],argv[4]);
        else if(argc==6 && !strcmp(argv[1],"ReportLegacyProxy"))
                ReportLegacyProxy(argv[2],argv[3],argv[4],argv[5]);
        else if(argc==5 && !strcmp(argv[1],"Report"))
                Report(argv[2],argv[3],argv[4]);
        else if(argc==3 && !strcmp(argv[1],"CurrentMonthReport"))
                CurrentMonthReport(argv[2]);
        else if(argc==3 && !strcmp(argv[1],"CurrentMonthReportLegacy"))
                CurrentMonthReportLegacy(argv[2]);
        else if(argc==4 && !strcmp(argv[1],"CurrentMonthReportLegacyProxy"))
                CurrentMonthReportLegacyProxy(argv[2],argv[3]);
        else if(argc==3 && !strcmp(argv[1],"UpdateSchema"))
                UpdateSchema(argv[2]);
        else
	{
		printf("\n%s Menu %s\n",argv[0],RELEASE);

                printf("\nDatabase Ops:\n");
		printf("\tInitialize|Backup|RestoreAll <mysql root passwd>\n");
		printf("\tRestore <mysql root passwd> <Restore table name>\n");
                printf("\tExtract <Mon> <Year> <mysql root passwd> <path to mysql table>\n");

                printf("\n\nOther Ops:\n");
                printf("\tReport <Mon> <Year> <mysql root passwd>\n");
                printf("\tReportLegacy <Mon> <Year> <mysql root passwd>\n");
                printf("\tReportLegacyProxy <Mon> <Year> <realm> <mysql root passwd>\n");
                printf("\tCurrentMonthReport <mysql root passwd>\n");
                printf("\tCurrentMonthReportLegacy <mysql root passwd>\n");
                printf("\tCurrentMonthReportLegacyProxy <realm> <mysql root passwd>\n");
                printf("\tHoldUsers <Max hours in tRadacct> <mysql root passwd>\n");
                printf("\tHoldUsersWarnOnly <Max hours in tRadacct> <mysql root passwd> [archive table name]\n");
                printf("\tUpdateSchema <mysql root passwd>\n");

		printf("\nNotes:\n");
                printf("\t<Mon> is 3 letter us en locale name abbreviation for month. Ex. Apr\n");
                printf("\t<path to mysql table> is something like /var/mysql/data/unxsradacct\n");
                printf("\t<realm> has no @ in front.\n");
	}
        exit(0);


}//void ExtMainShell(int argc, char *argv[])


int iExtMainCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"MainTools"))
	{
		if(!strcmp(gcCommand,"Dashboard"))
		{
			unxsRadacct("DashBoard");
		}
	}

	return(0);
}


void DashBoard(const char *cOptionalMsg)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	time_t luClock;

	//To handle error messages etc.
	if(cOptionalMsg[0] && strcmp(cOptionalMsg,"DashBoard"))
	{
		printf("%s\n",cOptionalMsg);
		return;
	}

	OpenFieldSet("Dashboard",100);


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
		printf("<tr><td></td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[2]);
	}
	mysql_free_result(mysqlRes);


	OpenRow("tLog (Last 20)","black");
	ExtDashboardSelect("tLog,tLogType","tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),tLog.cLogin,tLog.cTableName,tLog.cHost,tLogType.cLabel","tLog.uLogType=tLogType.uLogType AND tLog.uLogType<4","GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC",20);
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
		printf("<tr><td></td><td>%s</td><td>%s %s</td><td>%s %s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[3],mysqlField[5],mysqlField[2],mysqlField[4]);
	}
	mysql_free_result(mysqlRes);

	OpenRow("Login Activity (Last 20)","black");
	ExtDashboardSelect("tLog","tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),cServer,cHost","uLogType=6",
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
	printf("<td>%s (using %s %s)</td></tr>\n",gcHostname,TCLIENT,TAUTHORIZE);

	OpenRow("Build Information","black");
	printf("<td>%s</td></tr>\n",gcBuildInfo);

	OpenRow("RAD Status","black");
	printf("<td>%s %s</td></tr>\n",gcRADStatus,REV);

	OpenRow("Application Summary","black");
	printf("<td>unxsRadacct is a RADIUS accounting data collection, reporting and warehousing application. It has a CLI and at least one backend web interface. It also allows for account hold management based on collected usage statistics. unxsRadius is usually used with this application. mysqlISP2 can be a master enterprise application that controls all RADIUS infrastructure servers.</td></tr>\n");

	if(guPermLevel>9)
	{
		register unsigned int i;
		OpenRow("Table List","black");
		printf("<td>\n");
		for(i=0;cTableList[i][0];i++)
			printf("<a href=unxsRadacct.cgi?gcFunction=%.32s>%.32s</a><br>\n",
				cTableList[i],cTableList[i]);
		printf("</td></tr>\n");
        	OpenRow("Admin Functions","black");
		printf("<td><input type=hidden name=gcFunction value=MainTools>\n");
		printf(" <input class=largeButton type=submit name=gcCommand value=Dashboard></td></tr>\n");
	}

	CloseFieldSet();

}//void ExtMainContent(void)


void Extract(char *cMonth, char *cYear, char *cPasswd, char *cTablePath)
{
	char cTableName[33]={""};
	char *cNextMonth;
	char cNextYear[8]={""};
	char cThisYear[8]={""};
	char cThisMonth[4]={""};
	time_t uStart=0;
	time_t uEnd=0;
	unsigned uYear=0;
	unsigned uRows=0;
	time_t clock;
	struct tm *structTime;
	struct stat info;

	time(&clock);
	structTime=localtime(&clock);
	strftime(cThisYear,8,"%Y",structTime);
	strftime(cThisMonth,4,"%b",structTime);

	//printf("%s %s\n",cThisMonth,cThisYear);

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
		fprintf(stderr,"/usr/bin//usrmyisamchk is not installed!\n");
		exit(1);
	}


	/*mySQLRootConnect(cPasswd);
	mysql_query(&gMysql,"use unxsradacct");
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	*/
	ConnectDb();

	sprintf(cTableName,"t%.3s%.7s",cMonth,cYear);

	sprintf(gcQuery,"1-%s-%s",cMonth,cYear);
	uStart=cDateToUnixTime(gcQuery);
	printf("Start: %s",ctime(&uStart));

	if(uStart== -1 || !uStart)
	{
		fprintf(stderr,"Garbled month year input\n");
		exit(1);
	}

	strcpy(cNextYear,cYear);

	if(!strcmp(cMonth,"Jan"))
	{
		cNextMonth="Feb";
	}
	else if(!strcmp(cMonth,"Feb"))
	{
		cNextMonth="Mar";
	}
	else if(!strcmp(cMonth,"Mar"))
	{
		cNextMonth="Apr";
	}
	else if(!strcmp(cMonth,"Apr"))
	{
		cNextMonth="May";
	}
	else if(!strcmp(cMonth,"May"))
	{
		cNextMonth="Jun";
	}
	else if(!strcmp(cMonth,"Jun"))
	{
		cNextMonth="Jul";
	}
	else if(!strcmp(cMonth,"Jul"))
	{
		cNextMonth="Aug";
	}
	else if(!strcmp(cMonth,"Aug"))
	{
		cNextMonth="Sep";
	}
	else if(!strcmp(cMonth,"Sep"))
	{
		cNextMonth="Oct";
	}
	else if(!strcmp(cMonth,"Oct"))
	{
		cNextMonth="Nov";
	}
	else if(!strcmp(cMonth,"Nov"))
	{
		cNextMonth="Dec";
	}
	else if(!strcmp(cMonth,"Dec"))
	{
		cNextMonth="Jan";
		sscanf(cYear,"%u",&uYear);
		uYear++;
		sprintf(cNextYear,"%u",uYear);
	}

	sprintf(gcQuery,"1-%s-%s",cNextMonth,cNextYear);
	uEnd=cDateToUnixTime(gcQuery);
	printf("End:   %s",ctime(&uEnd));
	if(uEnd== -1 || !uEnd)
	{
		fprintf(stderr,"Garbled month year input\n");
		exit(1);
	}

	//printf("%s %s to %s %s",cMonth,cYear,cNextMonth,cNextYear);
	
	CreatetRadacctMonthTable(cTableName);
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

	printf("Getting data from tRadacct...\n");
	sprintf(gcQuery,"INSERT %s (uRadacct,cLogin,cSessionID,cNAS,uConnectTime,uPort,uPortType,uService,uProtocol,cIP,cLine,cCallerID,uInBytes,uOutBytes,uTermCause,uStartTime,uStopTime,cInfo) SELECT uRadacct,cLogin,cSessionID,cNAS,uConnectTime,uPort,uPortType,uService,uProtocol,cIP,cLine,cCallerID,uInBytes,uOutBytes,uTermCause,uStartTime,uStopTime,cInfo FROM tRadacct WHERE ( uStopTime>=%lu AND uStopTime<%lu ) OR ( uStopTime=0 AND uStartTime>=%lu AND uStartTime<%lu) ",cTableName,uStart,uEnd,uStart,uEnd);
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
		sprintf(gcQuery,"REPLACE tMonth SET cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",cTableName,guLoginClient,guLoginClient,clock);
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

	printf("Removing records from tRadacct...\n");
	sprintf(gcQuery,"DELETE QUICK FROM tRadacct WHERE (uStopTime>=%lu AND uStopTime<%lu) OR ( uStopTime=0 AND uStartTime>=%lu AND uStartTime<%lu) ",uStart,uEnd,uStart,uEnd);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}


	printf("Extracted and Archived. Table flushed: %s\n",cTableName);
	exit(0);

}//void Extract(char *cMonth, char *cYear, char *cPasswd, char *cTablePath)


void Report(char *cMonth, char *cYear, char *cPasswd)
{
	char cTableName[12]={""};

	ConnectDb();

	sprintf(cTableName,"t%.3s%.7s",cMonth,cYear);
	AcctReport(cTableName);
	exit(0);

}//void Report(char *cMonth, char *cYear, char *cPasswd)


void ReportLegacyProxy(char *cMonth, char *cYear, char *cRealm, char *cPasswd)
{
	char cTableName[12]={""};

	ConnectDb();

	sprintf(cTableName,"t%.3s%.7s",cMonth,cYear);
	AcctReportLegacyProxy(cRealm,cTableName);
	exit(0);

}//void ReportLegacyProxy(char *cMonth, char *cYear, char *cPasswd, char *cRealm)


void ReportLegacy(char *cMonth, char *cYear, char *cPasswd)
{
	char cTableName[12]={""};

	ConnectDb();

	sprintf(cTableName,"t%.3s%.7s",cMonth,cYear);
	AcctReportLegacy(cTableName);
	exit(0);

}//void ReportLegacy(char *cMonth, char *cYear, char *cPasswd)


void CurrentMonthReport(char *cPasswd)
{
	ConnectDb();
	AcctReport("tRadacct");
	exit(0);

}//void CurrentMonthReport(char *cPasswd)



void CurrentMonthReportLegacyProxy(char *cRealm, char *cPasswd)
{
	ConnectDb();
	AcctReportLegacyProxy(cRealm,"tRadacct");
	exit(0);

}//void CurrentMonthReportLegacyProxy(char *cRealm, char *cPasswd)


void CurrentMonthReportLegacy(char *cPasswd)
{
	ConnectDb();
	AcctReportLegacy("tRadacct");
	exit(0);

}//void CurrentMonthReportLegacy(char *cPasswd)


void RestoreAll(char *cPasswd)
{
	char cISMROOT[256]={""};

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0]) NoISMROOTMsg();

	printf("Restoring unxsRadacct data from .txt file in %s/unxsRadacct/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	mysql_query(&gMysql,"use unxsradacct");
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	for(i=0;cTableList[i][0];i++)
	{
sprintf(gcQuery,"load data local infile '%s/unxsRadacct/data/%s.txt' replace into table %s",cISMROOT,cTableList[i],cTableList[i]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		printf("%s\n",cTableList[i]);
	}

	printf("\nDone\n");
	exit(0);

}//void RestoreAll(char *cPasswd)


void Restore(char *cPasswd, char *cTableName)
{
	char cISMROOT[256]={""};

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0]) NoISMROOTMsg();

	printf("Restoring unxsRadacct data from .txt file in %s/unxsRadacct/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	mysql_query(&gMysql,"use unxsradacct");
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	sprintf(gcQuery,"load data infile '%s/unxsRadacct/data/%s.txt' replace into table %s",cISMROOT,cTableName,cTableName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("%s\n\nDone\n",cTableName);
	exit(0);

}//void Restore(char *cPasswd, char *cTableName)


void Backup(char *cPasswd)
{
	char cISMROOT[256]={""};

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0]) NoISMROOTMsg();

	printf("Backing up unxsRadacct data to .txt files in %s/unxsRadacct/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	mysql_query(&gMysql,"use unxsradacct");
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	for(i=0;cTableList[i][0];i++)
	{
		char cFileName[300];

		sprintf(cFileName,"%s/unxsRadacct/data/%s.txt",cISMROOT,cTableList[i]);
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
	exit(0);

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

	if(!cISMROOT[0]) NoISMROOTMsg();

	printf("Creating db and setting permissions, installing data from %s/unxsRadacct...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	//Create database
	mysql_query(&gMysql,"CREATE DATABASE unxsradacct");
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	//Grant localaccess privileges.
	mysql_query(&gMysql,"GRANT ALL ON unxsradacct.* TO unxsradacct@localhost IDENTIFIED BY 'wsxedc'");
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	
	//Change to mysqlbind db. Then initialize some tables with needed data
	mysql_query(&gMysql,"USE unxsradacct");
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	
	
	time(&clock);
	

	//Create tables and install default data
        //CreatetJob();
	CreatetAuthorize();
	CreatetClient();
        CreatetConfiguration();
        CreatetGlossary();
        CreatetLog();
        CreatetLogMonth();
        CreatetLogType();
        CreatetMonth();
        CreatetMonthLog();
        CreatetRadacct();
        CreatetRadacctMonth();
        CreatetServer();

        for(i=0;cInitTableList[i][0];i++)
        {
                sprintf(gcQuery,"LOAD DATA INFILE '%s/unxsRadacct/data/%s.txt' REPLACE INTO TABLE %s",
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
        if (!mysql_real_connect(&gMysql,DBIP0,"root",cPasswd,"mysql",DBPORT0,DBSOCKET,0))
        {
                printf("Database server unavailable\n");
                exit(1);
        }
}//void mySQLRootConnect(void)


void AcctReport(char *cTableName)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	time_t uStartTime,uStopTime;
	unsigned uFound=0;

        sprintf(gcQuery,"SELECT  COUNT(*), SUM(uConnectTime), SUM(uInBytes), SUM(uOutbytes), MIN(uStartTime), MAX(uStopTime),cLogin,cCallerID FROM %s WHERE uConnectTime>0 GROUP BY cLogin",cTableName);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("\n");
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
		time_t uConnectTime=0;

		sscanf(field[4],"%lu",&uStartTime);
		sscanf(field[5],"%lu",&uStopTime);
		sscanf(field[1],"%lu",&uConnectTime);

		printf("cLogin User:\t\t%s\n",field[6]);
		printf("Number of sessions:\t%s\n",field[0]);
		printf("Total connect time:\t%2.2f\n",(float)(uConnectTime/3600.00));
		printf("Time of first session:\t%s",ctime(&uStartTime));
		printf("Time of last session:\t%s",ctime(&uStopTime));
		printf("Total bytes downloaded:\t%s\n",field[3]);
		printf("Total bytes uploaded:\t%s\n",field[2]);
		printf("Last Caller ID:\t\t%s\n",field[7]);
		printf("\n\n");
		
		uFound++;
        }
        mysql_free_result(res);
	
	if(!uFound)
	{
                fprintf(stderr,"No records found in %s\n",cTableName);
		exit(1);
	}
	else
	{
		printf("For %u users. svn ID removed
		exit(0);
	}


}//void AcctReport(char *cTableName)


void AcctReportLegacy(char *cTableName)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uFound=0;

        sprintf(gcQuery,"SELECT SUM(uConnectTime),cLogin FROM %s WHERE uConnectTime>0 GROUP BY cLogin",cTableName);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
		time_t uConnectTime=0;

		sscanf(field[0],"%lu",&uConnectTime);
		printf("%s, %2.2f\n",field[1],(float)(uConnectTime/3600.00));
		
		uFound++;
        }
        mysql_free_result(res);
	
	if(!uFound)
	{
                fprintf(stderr,"No records found in %s\n",cTableName);
		exit(1);
	}
	else
	{
		printf("For %u users. svn ID removed
		exit(0);
	}

}//void AcctReportLegacy(char *cTableName)


void AcctReportLegacyProxy(char *cRealm,char *cTableName)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uFound=0;

        sprintf(gcQuery,"SELECT SUM(uConnectTime),cLogin FROM %s WHERE uConnectTime>0 AND cLogin LIKE '%%@%s' GROUP BY cLogin",cTableName,cRealm);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
		time_t uConnectTime=0;

		sscanf(field[0],"%lu",&uConnectTime);
		printf("%s, %2.2f\n",field[1],(float)(uConnectTime/3600.00));
		
		uFound++;
        }
        mysql_free_result(res);
	
	if(!uFound)
	{
                fprintf(stderr,"No proxy records found in %s. Realm:%s\n",cTableName,cRealm);
		exit(1);
	}
	else
	{
		printf("For %u realm:%s users. svn ID removed
		exit(0);
	}

}//void AcctReportLegacyProxy(char *cRealm,char *cTableName)


void HoldUsers(char *cMaxHours, char *cPasswd, unsigned uWarnOnly, char *cTableName)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	time_t uConnectTime=0;
	float fConnectTime=0.0;
	float fMaxAllowedConnectTime=0.0;
	time_t clock;
	unsigned uInstance=0,uProduct=0,uClient=0;

	sscanf(cMaxHours,"%f",&fMaxAllowedConnectTime);

	mySQLRootConnect(cPasswd);//for local mysqlRaddacct database

	mysql_query(&gMysql,"use unxsradacct");
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}


	if(!cTableName[0])
		cTableName="tRadacct";

        sprintf(gcQuery,"SELECT SUM(uConnectTime),cLogin FROM %s WHERE uConnectTime>0 GROUP BY cLogin",cTableName);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("\n");
        res=mysql_store_result(&gMysql);
	if(!mysql_num_rows(res))
        {
		fprintf(stderr,"No uConnectTime>0 records found in %s\n",cTableName);
		exit(1);
	}

	time(&clock);
        while((field=mysql_fetch_row(res)))
        {

		sscanf(field[0],"%lu",&uConnectTime);
		fConnectTime=(float)(uConnectTime/3600.00);

		if(fConnectTime>=fMaxAllowedConnectTime)
		{
        		MYSQL_RES *res2;
		        MYSQL_ROW field2;
			unsigned uCount=0;
			char cHoldUsersEmail[256]={""};
			char cFromEmail[256]={"unxsradacct"};
			char cMTA[256]={"mail"};
			char cMTAArgs[256]={""};
			char cExtJobQueueDbIp[256]={""};
			char cExtJobQueueDbName[256]={""};
			char cExtJobQueueDbLogin[256]={""};
			char cExtJobQueueDbPwd[256]={""};

			printf("%s total connect time:\t%2.2f Hr\n",field[1],fConnectTime);

			//Get all required job info from mysqlISP
			//Assume mysqlISP db is on local server also
		
			GetConfiguration("cExtJobQueueDbIp",cExtJobQueueDbIp,0);
			GetConfiguration("cExtJobQueueDbName",cExtJobQueueDbName,0);
			GetConfiguration("cExtJobQueueDbLogin",cExtJobQueueDbLogin,0);
			GetConfiguration("cExtJobQueueDbPwd",cExtJobQueueDbPwd,0);
			if( !cExtJobQueueDbName[0] || !cExtJobQueueDbLogin[0] ||
					!cExtJobQueueDbPwd[0])
			{
				static unsigned uOnlyOnce=0;

				if(!uOnlyOnce)
				{
					fprintf(stderr,"\ncExtJobQueue tConfiguration values not found."
								" Will not process any hold jobs!\n\n");
					uOnlyOnce=1;
				}
				continue;
			}

			//For mysqlisp database somewhere: uses mysqlext
			ExtConnectDb(cExtJobQueueDbName,cExtJobQueueDbIp,
					cExtJobQueueDbPwd,cExtJobQueueDbLogin);

			sprintf(gcQuery,"SELECT tClientConfig.uGroup FROM tClientConfig,tInstance,tParameter WHERE"
					" tClientConfig.uParameter=tParameter.uParameter AND"
					" tClientConfig.uGroup=tInstance.uInstance AND"
					" tInstance.uStatus=%u AND tParameter.cParameter='mysqlRadius.Login' AND"
					" tClientConfig.cValue='%s'", mysqlISP_Deployed,field[1]);

			mysql_query(&mysqlext,gcQuery);
			if(mysql_errno(&mysqlext))
			{
				fprintf(stderr,"%s\n",mysql_error(&mysqlext));
				exit(1);
			}
			res2=mysql_store_result(&mysqlext);
			uCount=mysql_num_rows(res2);
			if(uCount>1)
			{
				fprintf(stderr,"Unexpected number of rows select1: %u\n",uCount);
				exit(1);
			}

			if((field2=mysql_fetch_row(res2)))
			{
				sscanf(field2[0],"%u",&uInstance);

			}
			else
			{
				printf("%s:\nNo such mysqlISP.mysqlRadius user that is deployed\n\n"
						,field[1]);
				continue;
			}
			mysql_free_result(res2);

			sprintf(gcQuery,"SELECT tInstance.uProduct,tClientConfig.uOwner FROM"
					" tClientConfig,tParameter,tInstance WHERE"
					" tClientConfig.uParameter=tParameter.uParameter AND"
					" tInstance.uInstance=tClientConfig.uGroup AND"
					" tParameter.cParameter='mysqlRadius.Login' AND tClientConfig.uGroup=%u",
						uInstance);
			mysql_query(&mysqlext,gcQuery);
			if(mysql_errno(&mysqlext))
			{
				fprintf(stderr,"%s\n",mysql_error(&mysqlext));
				exit(1);
			}
			res2=mysql_store_result(&mysqlext);
			if((field2=mysql_fetch_row(res2)))
			{
				sscanf(field2[0],"%u",&uProduct);
				sscanf(field2[1],"%u",&uClient);
			}
			else
			{
				fprintf(stderr,"Select2 returned null\n");
				exit(1);
			}
			mysql_free_result(res2);


			if(!uWarnOnly)
			{
				//debug
				printf("SubmitInstanceJob(\"Hold\",%lu,%u,%u,%u);\n\n",
					clock,uInstance,uProduct,uClient);
				SubmitInstanceJob("Hold",clock,uInstance,uProduct,uClient);
			}

			GetConfiguration("cHoldUsersEmail",cHoldUsersEmail,0);
			//debug stuff delete later when app is stable
			printf("Attempting to send mail to %s\n",cHoldUsersEmail);
			if(cHoldUsersEmail[0])
			{
				GetConfiguration("cMTA",cMTA,0);

				if(cMTA[0])
				{
					char cMtaString[256];
					FILE *fp;

					GetConfiguration("cMTAArgs",cMTAArgs,0);
					GetConfiguration("cFromEmail",cFromEmail,0);

					sprintf(cMtaString,"%.200s %.55s",cMTA,cMTAArgs);

					if((fp=popen(cMtaString,"w")))	
					{
						fprintf(fp,"To: %s\n",cHoldUsersEmail);
						if(cHoldUsersEmail[0])
							fprintf(fp,"From: %s\n",cHoldUsersEmail);
						if(cFromEmail[0])
							fprintf(fp,"From: %s\n",cFromEmail);

						if(uWarnOnly)
							fprintf(fp,"Subject: HoldUsersWarnOnly(%s)",cMaxHours);
						else
							fprintf(fp,"Subject: HoldUsers(%s)",cMaxHours);

				       			fprintf(fp," %s %s %2.2f\n\n",
								field[1],cTableName,fConnectTime);

						if(!uWarnOnly)
						fprintf(fp,"HoldUsers(%s) job done at :%s",
								cMaxHours,
								ctime(&clock));

						fprintf(fp,"cLogin:%s fConnectTime: %2.2f\n\n",
								field[1],fConnectTime);
						fclose(fp);
						//debug stuff delete later when app is stable
						printf("Mail sent to %s\n\n",cHoldUsersEmail);

					}
					else
					{
						fprintf(stderr,"Email error: %s %s %s %s\n",
							cMtaString,cMTAArgs,
							cFromEmail,cHoldUsersEmail);
					}

				}//If cMTA[0]

			}//If cHoldUsersEmail[0]
		}
	}

	printf("Done OK\n\n");
	exit(0);


}//void HoldUsers(char *cMaxHours, char *cPasswd)


void SubmitInstanceJob(char *cJobType,time_t uJobDate, unsigned uInstance, unsigned uProduct,
				unsigned uClient)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uService=0;
	unsigned uJob=0;
	unsigned uJobGroup=0;
	unsigned uFirst=1;
	unsigned uPrevStatus=0;
	char cJobName[100];
	char query[256];

	uPrevStatus=ReaduPrevStatus(uInstance);

	//Remove all waiting jobs of same product instance
        sprintf(gcQuery,"DELETE FROM tJob WHERE uInstance=%u AND uJobStatus=%u"
						,uInstance,mysqlISP_Waiting);
        mysql_query(&mysqlext,query);
        if(mysql_errno(&mysqlext))
	{
		fprintf(stderr,mysql_error(&mysqlext));
		return;
	}

	//One job for each service that comprise the product
        sprintf(gcQuery,"SELECT tService.uService,tService.cJobName,tService.cServer FROM"
			" tService,tServiceGlue WHERE tServiceGlue.uService=tService.uService AND"
			" tServiceGlue.uServiceGroup=%u",uProduct);
        mysql_query(&mysqlext,query);
        if(mysql_errno(&mysqlext))
	{
		fprintf(stderr,mysql_error(&mysqlext));
		return;
	}

        res=mysql_store_result(&mysqlext);
	if(!mysql_num_rows(res))
	{
		fprintf(stderr,"No service found for uProduct=%u\nQuery follows:\n",uProduct);
		fprintf(stderr,query);
		fprintf(stderr,"\n");
		return;
	}
        while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uService);
		sprintf(cJobName,"%.81s.%.18s",field[1],cJobType);
		SubmitServiceJob(uService,cJobName,field[2],uInstance,uJobDate,uPrevStatus,uClient);

		uJob=mysql_insert_id(&mysqlext);
		if(uFirst)
		{
			uFirst=0;
			uJobGroup=uJob;
		}
		sprintf(gcQuery,"UPDATE tJob SET uJobGroup=%u WHERE uJob=%u",uJobGroup
				,uJob);
        	mysql_query(&mysqlext,query);
        	if(mysql_errno(&mysqlext))
		{
			fprintf(stderr,mysql_error(&mysqlext));
			exit(1);
		}
	}
	mysql_free_result(res);

}//void SubmitInstanceJob()


void SubmitServiceJob(unsigned uService,char *cJobName,char *cServer,
			unsigned uInstance,time_t uJobDate,unsigned uPrevStatus, unsigned uClient)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cJob[2048]={""};
	char cLine[256]={""};
	char cPrevStatus[16]={""};
	time_t clock;
	

        sprintf(gcQuery,"SELECT tParameter.cParameter,tClientConfig.cValue FROM tClientConfig,tParameter WHERE"
			" tClientConfig.uParameter=tParameter.uParameter AND tClientConfig.uGroup=%u AND"
			" tClientConfig.uService=%u ORDER BY tParameter.cParameter",uInstance,uService);
        mysql_query(&mysqlext,gcQuery);
        if(mysql_errno(&mysqlext))
	{
		fprintf(stderr,mysql_error(&mysqlext));
		exit(1);;
	}

        res=mysql_store_result(&mysqlext);
	if(!mysql_num_rows(res))
	{
		fprintf(stderr,"Could not find any parameter");
		exit(1);
	}
        while((field=mysql_fetch_row(res)))
	{
		sprintf(cLine,"%.125s=%.125s\n",field[0],field[1]);
		if(strlen(cJob)+strlen(cLine)<2030)
			strcat(cJob,cLine);
		else
		{
			fprintf(stderr,"cJob length exceeded. SubmitInstanceJob()");
			exit(1);
		}
	}
	mysql_free_result(res);
	
	if(strstr(cJobName,".Hold") || strstr(cJobName,".Cancel"))
	{
		sprintf(cPrevStatus,"\nuPrevStatus=%u",uPrevStatus);
		strcat(cJob,cPrevStatus);
	}
	
	time(&clock);
			
        sprintf(gcQuery,"INSERT INTO tJob SET uInstance=%u,cLabel='%s',cJobData='%s',cJobName='%s',"
			"cServer='%s',uJobClient=%u,uJobDate=%lu,uJobStatus=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
			uInstance,
			"Priority=Normal",
			cJob,
			cJobName,
			cServer,
			uClient,
			uJobDate,
			mysqlISP_Waiting,
			guLoginClient,
			guLoginClient,
			(time_t)clock );

        mysql_query(&mysqlext,gcQuery);
        if(mysql_errno(&mysqlext))
	{
		fprintf(stderr,mysql_error(&mysqlext));
		exit(1);;
	}

}//void SubmitServiceJob()


unsigned ReaduPrevStatus(unsigned uInstance)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char query[256];
	char *cp;
	unsigned uPrevStatus=0;

        sprintf(gcQuery,"SELECT cJobData FROM tJob WHERE uInstance=%u AND uJobStatus=%u"
						,uInstance,mysqlISP_Waiting);
        mysql_query(&mysqlext,query);
        if(mysql_errno(&mysqlext))
	{
		fprintf(stderr,mysql_error(&mysqlext));
		exit(1);;
	}
        res=mysql_store_result(&mysqlext);
        if((field=mysql_fetch_row(res)))
	{
		if((cp=strstr(field[0],"uPrevStatus=")))
			sscanf(cp+12,"%u",&uPrevStatus);
	}
	mysql_free_result(res);

	return(uPrevStatus);

}//void ReaduPrevStatus()

//Uses MYSQL mysqlext
int ExtConnectDb(char *cDbName, char *cDbIp, char *cDbPwd, char *cDbLogin)
{
	if(!cDbIp[0]) cDbIp=NULL;

        mysql_init(&mysqlext);
        if (!mysql_real_connect(&mysqlext,cDbIp,cDbLogin,cDbPwd,cDbName,DBPORT0,DBSOCKET,0))
        {
		fprintf(stderr,"ExtConnectDb failed for %s.%s",cDbIp,cDbName);
		exit(1);
        }
	return(0);

}//end of ExtConnectDb()


void GetConfiguration(const char *cName, char *cValue, unsigned uHtml)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='%s'",cName);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
		{
        	        tConfiguration(mysql_error(&gMysql));
		}
		else
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
                strcpy(cValue,field[0]);
        mysql_free_result(res);

}//void GetConfiguration(char *cName, char *cValue)


void UpdateSchema(char *cPasswd)
{
	printf("Upgrading/updating older version database schema...\n\n");

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	mysql_query(&gMysql,"use unxsradacct");
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	//Upgrade tRadacct
	printf("Doing tRadacct...\n\n");

	//Upgrade tRadacctMonth
	printf("Doing tRadacctMonth...\n\n");

	printf("\nDone\n");
	exit(0);

}//void UpdateSchema(char *cPasswd)


void NoISMROOTMsg(void)
{
	printf("You must set ISMROOT env var first. Ex. (bash) export ISMROOT=/home/joe/\n");
	exit(1);
}//void NoISMROOTMsg(void)


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
	if(strstr(cArgv[0],"unxsRadacctRSS.xml"))
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
		sprintf(cLinkStart,"%s://%s/cgi-bin/unxsRadacct.cgi",cHTTP,gcHost);

		printf("Content-type: text/xml\n\n");

		//Open xml
		printf("<?xml version='1.0' encoding='UTF-8'?>\n");
		printf("<rss version='2.0'>\n");
		printf("<channel>\n");
		printf("<title>unxsRadacct RSS tJob Errors</title>\n");
		printf("<link>http://openisp.net/unxsRadacct</link>\n");
		printf("<description>unxsRadacct tJob Errors</description>\n");
		printf("<lastBuildDate>%.199s</lastBuildDate>\n",cRSSDate);
		printf("<generator>unxsRadacct RSS Generator</generator>\n");
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
			printf("<title>unxsRadacct.tJob.uJob=%s</title>\n",field[0]);
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
		if(!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,DBPORT1,DBSOCKET,0))
		{
                	fprintf(stderr,"Database server unavailable.\n");
			exit(1);
		}
	}

}//end of TextConnectDb()


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
