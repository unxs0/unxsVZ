/*
FILE
	mainfunc.h
PURPOSE
	Included in main.c. For command line interface and html main link.
AUTHOR
	(C) 2001-2017 Gary Wallis for Unixservice, LLC.
 
*/

#include "local.h"
char *strptime(const char *s, const char *format, struct tm *tm);

static char cTableList[64][32]={
		"tAuthorize",
		"tClient",
		"tConfiguration",
		"tField",
		"tFieldType",
		"tGlossary",
		"tIndexType",
		"tJob",
		"tJobStatus",
		"tLog",
		"tLogMonth",
		"tLogType",
		"tMonth",
		"tProject",
		"tProjectStatus",
		"tStatus",
		"tServer",
		"tTable",
		"tTemplate",
		"tTemplateSet",
		"tTemplateType",
		""};

void ExtMainShell(int argc, char *argv[]);
void mySQLRootConnect(char *cPasswd);
void ImportTemplateFile(char *cTemplate, char *cFile, char *cTemplateType, char *cTemplateSet);
void ExportTemplateFiles(char *cDir, char *cTemplateType);
void ExtracttLog(char *cMonth, char *cYear, char *cPasswd, char *cTablePath);
time_t cDateToUnixTime(char *cDate);
void CreatetLogTable(char *cTableName);
void NextMonthYear(char *cMonth,char *cYear,char *cNextMonth,char *cNextYear);

void CalledByAlias(int iArgc,char *cArgv[]);
void TextConnectDb(void);
void DashBoard(const char *cOptionalMsg);
void EncryptPasswdMD5(char *pw);
void UpdateSchema(void);

//ProjectFunctionProtos()


int iExtMainCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"MainTools"))
	{
		if(!strcmp(gcCommand,"Dashboard"))
		{
			unxsRAD("DashBoard");
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
	sprintf(gcQuery,"SELECT cMessage,GREATEST(uCreatedDate,uModDate),cServer"
			" FROM tLog WHERE uLogType=4 ORDER BY GREATEST(uCreatedDate,uModDate) DESC LIMIT 20");
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
	sprintf(gcQuery,"SELECT tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),tLog.cLogin,tLog.cTableName,tLog.cHost,tLogType.cLabel"
			" FROM tLog,tLogType WHERE tLog.uLogType=tLogType.uLogType AND tLog.uLogType!=4"
			" ORDER BY GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC LIMIT 20");
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
	sprintf(gcQuery,"SELECT cLabel,GREATEST(uCreatedDate,uModDate),cServer,cHost"
			" FROM tLog WHERE uLogType=6 ORDER BY GREATEST(uCreatedDate,uModDate) DESC LIMIT 20");
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

	OpenRow("Pending or Stuck Jobs (Last 20)","black");
	sprintf(gcQuery,"SELECT tJob.cLabel,GREATEST(tJob.uCreatedDate,tJob.uModDate),tServer.cLabel,tJobStatus.cLabel"
			" FROM tJob,tJobStatus,tServer WHERE tJob.uJobStatus=tJobStatus.uJobStatus AND"
			" tJob.uJobStatus!=3 AND tJob.uServer=tServer.uServer"
			" ORDER BY GREATEST(tJob.uCreatedDate,tJob.uModDate) DESC LIMIT 20");
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

	OpenRow("main.c Build Information","black");
	if(!strstr(sgcBuildInfo,"-dirty"))
		printf("<td><a target=github class=darkLink href=https://github.com/unxs0/unxsVZ/commit/%1$s>%1$s</a></td></tr>\n",sgcBuildInfo);
	else
		printf("<td>Local git mods: %1$s</td></tr>\n",sgcBuildInfo);

	OpenRow("Application Summary","black");
	printf("<td>This tool is used to create C MySQL CGI applications from template code sets."
		" <br>First you setup the schema and it's UI characteristics then you can build the source code app directory."
		" <br>You can program the application logic in the per table .c files while the schema is still under development.</td></tr>\n");

	if(guPermLevel>9)
	{
		register unsigned int i;
		OpenRow("Table List","black");
		printf("<td>\n");
		for(i=0;cTableList[i][0];i++)
			printf("<a class=darkLink href=unxsRAD.cgi?gcFunction=%.32s>%.32s</a><br>\n",
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

        if(argc==2 && !strcmp(argv[1],"UpdateSchema"))
                UpdateSchema();
	else if(argc==6 && !strcmp(argv[1],"ImportTemplateFile"))
                ImportTemplateFile(argv[2],argv[3],argv[4],argv[5]);
	else if(argc==4 && !strcmp(argv[1],"ExportTemplateFiles"))
                ExportTemplateFiles(argv[2],argv[3]);
	else if(argc==6 && !strcmp(argv[1],"ExtracttLog"))
               	ExtracttLog(argv[2],argv[3],argv[4],argv[5]);
        else
	{
		printf("\n%s %s Menu\n\nDatabase Ops:\n",argv[0],RELEASE);
		printf("\tUpdateSchema (!First backup your db! You have been warned)\n");
		printf("\tImportTemplateFile <tTemplate.cLabel> <filespec> <tTemplateSet.cLabel> <tTemplateType.cLabel>\n");
		printf("\tExportTemplateFiles <Base dir> <tTemplateSet.cLabel>\n");
		printf("\tExtracttLog <Mon> <Year> <mysql root passwd> <path to mysql table>\n");
		printf("\n");
	}
        exit(0);


}//void ExtMainShell(int argc, char *argv[])


void mySQLRootConnect(char *cPasswd)
{
        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,NULL,"root",cPasswd,"mysql",0,NULL,0))
        {
                printf("Database root server unavailable cPasswd=%s\n",cPasswd);
                exit(1);
        }
}//void mySQLRootConnect(void)


void ExportTemplateFiles(char *cDir, char *cTemplateSet)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

	printf("\nExportTemplateFiles(): Start %s/%s\n",cDir,cTemplateSet);

	TextConnectDb();

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}


	sprintf(gcQuery,"SELECT tTemplate.cLabel,tTemplate.cTemplate,tTemplateType.cLabel"
			" FROM tTemplate,tTemplateType,tTemplateSet"
			" WHERE tTemplate.uTemplateType=tTemplateType.uTemplateType"
			" AND tTemplate.uTemplateSet=tTemplateSet.uTemplateSet"
			" AND tTemplateSet.cLabel='%s'",cTemplateSet);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        mysqlRes=mysql_store_result(&gMysql);
	char cPath[256]={""};
	sprintf(cPath,"%.99s/%.32s",cDir,cTemplateSet);
	mkdir(cPath,0755);
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		FILE *fp;
		char cFile[378]={""};
		sprintf(cPath,"%.99s/%.32s/%.32s",cDir,cTemplateSet,mysqlField[2]);
		mkdir(cPath,0755);
		sprintf(cFile,"%s/%.99s",cPath,mysqlField[0]);
		printf("%s\n",cFile);
		if((fp=fopen(cFile,"w"))!=NULL)
		{
			fprintf(fp,"%s",mysqlField[1]);
			fclose(fp);
		}
		else
		{
			printf("fopen() error: %s\n",cFile);
		}
	}
	mysql_free_result(mysqlRes);


	printf("\nDone\n");

}//void ExportTemplateFiles()


void ImportTemplateFile(char *cTemplate, char *cFile, char *cTemplateSet, char *cTemplateType)
{
	FILE *fp;
	unsigned uTemplate=0;
	unsigned uTemplateSet=0;
	unsigned uTemplateType=0;
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
		printf("Could not find tTemplateSet.clabel=%s creating!\n",cTemplateSet);
		sprintf(gcQuery,"INSERT INTO tTemplateSet SET cLabel='%s',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				cTemplateSet);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		uTemplateSet=mysql_insert_id(&gMysql);
	}

	//uTemplateType
	sprintf(gcQuery,"SELECT uTemplateType FROM tTemplateType WHERE cLabel='%s'",cTemplateType);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        mysqlRes=mysql_store_result(&gMysql);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sscanf(mysqlField[0],"%u",&uTemplateType);
	mysql_free_result(mysqlRes);

	if(!uTemplateType)
	{
		printf("Could not find tTemplateType.clabel=%s creating!\n",cTemplateType);
		sprintf(gcQuery,"INSERT INTO tTemplateType SET cLabel='%s',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				cTemplateType);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		uTemplateType=mysql_insert_id(&gMysql);
	}

	//uTemplate
	sprintf(gcQuery,"SELECT uTemplate FROM tTemplate WHERE cLabel='%s' AND uTemplateType=%u AND uTemplateSet=%u",
			cTemplate,uTemplateType,uTemplateSet);
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
		sprintf(cBuffer,"UPDATE tTemplate SET uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()),cTemplate='',uTemplateSet=%u"
				",uTemplateType=%u WHERE uTemplate=%u",uTemplateSet,uTemplateType,uTemplate);
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
		sprintf(cBuffer,"INSERT INTO tTemplate SET uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
				",cLabel='%s',uTemplateSet=%u,uTemplateType=%u",cTemplate,uTemplateSet,uTemplateType);
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
	if(strstr(cArgv[0],"unxsRADRSS.xml"))
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
		sprintf(cLinkStart,"%s://%s/cgi-bin/unxsRAD.cgi",cHTTP,gcHost);

		printf("Content-type: text/xml\n\n");

		//Open xml
		printf("<?xml version='1.0' encoding='UTF-8'?>\n");
		printf("<rss version='2.0'>\n");
		printf("<channel>\n");
		printf("<title>unxsRAD RSS tJob Errors</title>\n");
		printf("<link>http://openisp.net/unxsRAD</link>\n");
		printf("<description>unxsRAD tJob Errors</description>\n");
		printf("<lastBuildDate>%.199s</lastBuildDate>\n",cRSSDate);
		printf("<generator>unxsRAD RSS Generator</generator>\n");
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
			printf("<title>unxsRAD.tJob.uJob=%s</title>\n",field[0]);
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
	//
	//First will try to connect to the MySQL server at DBIP0.
	//If it fails it will try to connect to the MySQL server at DBIP1, if that fails too
	//an error message will be displayed and the software will exit
	//
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
	sprintf(gcQuery,"INSERT INTO tLog"
			" SET cMessage='ExtracttLog() Start...',cServer='%s',uLogType=4,uOwner=1,"
			"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",gcHostname);
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
	sprintf(gcQuery,"INSERT %s (uLog,cLabel,uLogType,cHash,uPermLevel,uLoginClient,cLogin,cHost,uTablePK,cTableName,"
			"uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate) SELECT uLog,cLabel,uLogType,cHash,uPermLevel,"
			"uLoginClient,cLogin,cHost,uTablePK,cTableName,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate"
			" FROM tLog WHERE uCreatedDate>=%lu AND uCreatedDate<%lu",cTableName,uStart,uEnd);
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
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS %s ("
			" uLog INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			" uTablePK VARCHAR(32) NOT NULL DEFAULT '',"
			" cHost VARCHAR(32) NOT NULL DEFAULT '',"
			" uLoginClient INT UNSIGNED NOT NULL DEFAULT 0,"
			" cLogin VARCHAR(32) NOT NULL DEFAULT '',"
			" uPermLevel INT UNSIGNED NOT NULL DEFAULT 0,"
			" cTableName VARCHAR(32) NOT NULL DEFAULT '',"
			" cLabel VARCHAR(64) NOT NULL DEFAULT '',"
			" uOwner INT UNSIGNED NOT NULL DEFAULT 0,INDEX (uOwner),"
			" uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			" cHash VARCHAR(32) NOT NULL DEFAULT '',"
			" uLogType INT UNSIGNED NOT NULL DEFAULT 0,INDEX (uLogType) )",cTableName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
}//CreatetLogTable()


void UpdateSchema(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	printf("UpdateSchema(): Start\n");

	TextConnectDb();

	//tTable
	//
	unsigned uTableSubDir=0;
	unsigned uTableClass=0;
	unsigned uTableTemplateType=0;
	sprintf(gcQuery,"SHOW COLUMNS IN tTable");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[0],"cSubDir"))
			uTableSubDir=1;
		else if(!strcmp(field[0],"uClass"))
			uTableClass=1;
		else if(!strcmp(field[0],"uTemplateType"))
			uTableTemplateType=1;
	}
       	mysql_free_result(res);
	if(!uTableSubDir)
	{
		sprintf(gcQuery,"ALTER TABLE tTable ADD cSubDir VARCHAR(100) NOT NULL DEFAULT ''");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added cSubDir to tTable\n");
	}
	if(!uTableClass)
	{
		sprintf(gcQuery,"ALTER TABLE tTable ADD uClass INT UNSIGNED NOT NULL DEFAULT 0");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added uClass to tTable\n");
		//Try to fix but warn
		printf("Trying to fix existing default tables. Check!\n");
		sprintf(gcQuery,"UPDATE tTable SET uClass=%u WHERE uTableOrder>=1000",uDEFAULTCLASS);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Ran: %s. Check results!\n",gcQuery);
	}
	if(!uTableTemplateType)
	{
		sprintf(gcQuery,"ALTER TABLE tTable ADD uTemplateType INT UNSIGNED NOT NULL DEFAULT 0");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added uTemplateType to tTable\n");
	}
	//
	//tTable
	
	//tField
	//
	unsigned uFieldClass=0;
	sprintf(gcQuery,"SHOW COLUMNS IN tField");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[0],"uClass"))
			uFieldClass=1;
	}
       	mysql_free_result(res);
	if(!uFieldClass)
	{
		sprintf(gcQuery,"ALTER TABLE tField ADD uClass INT UNSIGNED NOT NULL DEFAULT 0");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added uClass to tField\n");

		//Try to fix but warn
		printf("Trying to fix existing default fields. Check!\n");
		sprintf(gcQuery,"UPDATE tField SET uClass=%u WHERE uOrder>=1000",uDEFAULTCLASS);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Ran: %s. Check results!\n",gcQuery);
		sprintf(gcQuery,"UPDATE tField SET uClass=%u WHERE uOrder<=2 AND (cTitle='Primary key' OR cTitle='Short label')",uDEFAULTCLASS);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Ran: %s. Check results!\n",gcQuery);
	}
	//
	//tField
	
	printf("UpdateSchema(): End\n");
	
}//void UpdateSchema(void)


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
	printf("\nPlease report this unxsRAD fatal error ASAP:\n%s\n",cError);

	//Attempt to report error in tLog
        sprintf(gcQuery,"INSERT INTO tLog SET cLabel='TextError',uLogType=4,uPermLevel=%u,uLoginClient=%u,cLogin='%s',"
			"cHost='%s',cMessage=\"%s\",cServer='%s',uOwner=1,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				guPermLevel,guLoginClient,gcUser,gcHost,cError,gcHostname,guLoginClient);
        mysql_query(&gMysql,gcQuery);
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

