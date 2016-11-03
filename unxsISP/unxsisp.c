/*
FILE
	mysqlisp.c
	svn ID removed
PURPOSE
	Jobqueue and other special gcCommand line processing.
LEGAL
	(C) 2004-2009 Gary Wallis and Hugo Urquiza. GPL License Applies.
WORK IN PROGRESS
	Using print invoice gcFunction to develop basic templates and procedures.
	Then once that is working ok. Will do the mail invoice gcFunction.

*/

#include "mysqlrad.h"
#include "mysqlisp.h"


//Contents
void ProcessJobQueue(unsigned const uDebug, char const *cServer);
void UpdateJobStatus(unsigned uJob,unsigned uJobStatus,char *cRemoteMsg);
void ApplicationFunctions(FILE *fp,char const *cVarName);

void TextConnectDb(void);
void AnalyzeTables(void);


void UpdateJobStatus(unsigned uJob,unsigned uJobStatus,char *cRemoteMsg)
{
	time_t luClock;

	time(&luClock);
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=%u,uModBy=1,uModDate=%lu,cRemoteMsg='%.32s' WHERE uJob=%u",uJobStatus,luClock,TextAreaSave(cRemoteMsg),uJob);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                fprintf(stderr,"%s\n",mysql_error(&gMysql));

}//void UpdateJobStatus(unsigned uJob,unsigned uJobStatus,char *cRemoteMsg)


void TextConnectDb(void)
{
        mysql_init(&gMysql);
	if(!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
	{
		if (!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
		{
	                fprintf(stderr,"Database server unavailable.\n");
			mysql_close(&gMysql);
			exit(1);
		}
	}

}//end of TextConnectDb()


void AnalyzeTables(void)
{
	TextConnectDb();

	mysql_query(&gMysql,"ANALYZE TABLE tInstance");
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	mysql_query(&gMysql,"ANALYZE TABLE tProduct");
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	mysql_query(&gMysql,"ANALYZE TABLE tClient");
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	mysql_query(&gMysql,"ANALYZE TABLE tPeriod");
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	mysql_close(&gMysql);
	exit(0);

}//void AnalyzeTables(void)


//libtemplate.a required
void AppFunctions(FILE *fp,char *cFunction)
{
	//Empty function
}//void AppFunctions(char *cFunction)

