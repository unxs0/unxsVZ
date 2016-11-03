/*
FILE
	sms.c
	svn ID removed
PURPOSE
	SMS gateway preprocessor interface
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2010.
	GPLv2 License applies. See LICENSE file.
NOTES
	See README in this dir.
*/

#include "mysqlrad.h"
#include <sys/sysinfo.h>
#include <ctype.h>
#include <curl/curl.h>

MYSQL gMysql;
FILE *gLfp=NULL;
char gcQuery[8192]={""};
unsigned guLoginClient=1;//Root user
char cHostname[100]={""};
char gcProgram[32]={""};
unsigned guStatus=0;//not a valid status
unsigned guDebug=1;

//extern same dir
void TextConnectDb(void);

//this file protos TOC
void Version(void);
void Run(void);
void Set(char *cPhone,const char *cuDigestThreshold,const char *cuReceivePeriod,const char *cuSendPeriod,const char *cuPeriodCount);
void QueueMessage(const char *cPhone,char *cMessage);
void logfileLine(const char *cFunction,const char *cLogline);
int main(int iArgc, char *cArgv[]);
void Initialize(const char *cPasswd);
void ErrorExit(void);
void NormalExit(void);
void mySQLRootConnect(const char *cPasswd);
unsigned SendMessage(const char *cPhone,char *cMessage);

#define macro_MySQLQueryBasic \
	mysql_query(&gMysql,gcQuery);\
	if(mysql_errno(&gMysql))\
	{\
		printf("%s\n",mysql_error(&gMysql));\
		ErrorExit();\
	}


void Run(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	logfileLine("Run","Entry");

	TextConnectDb();

	if(guDebug)
	{

		logfileLine("Run","Dump tPhone");
		sprintf(gcQuery,"SELECT uPhone,cNumber,uDigestThreshold,uReceivePeriod,uSendPeriod,uPeriodCount FROM tPhone");
		macro_MySQLQueryBasic;
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			printf("%s %s %s %s %s %s\n",field[0],field[1],field[2],field[3],field[4],field[5]);
		}
		mysql_free_result(res);

		logfileLine("Run","Dump tQueue");
		sprintf(gcQuery,"SELECT uQueue,cMessage,uPhone,FROM_UNIXTIME(uDateCreated),FROM_UNIXTIME(uDateMod) FROM tQueue");
		macro_MySQLQueryBasic;
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			printf("uQueue:%s cMessage:%s uPhone:%s uDateCreated:%s uDateMod:%s\n",
				field[0],field[1],field[2],field[3],field[4]);
		}
		mysql_free_result(res);
	}


	//Send messages that are ready, then delete them from the queue and reset their uPeriodCount
	sprintf(gcQuery,"SELECT NOW(),tPhone.cNumber,tQueue.cMessage,FROM_UNIXTIME(tQueue.uDateMod),"
			"tPhone.uSendPeriod,tPhone.uPeriodCount,tPhone.uDigestThreshold,tPhone.uPhone,tQueue.uQueue"
			" FROM tQueue,tPhone WHERE tQueue.uPhone=tPhone.uPhone"
			" AND tQueue.uDateMod+tPhone.uSendPeriod<UNIX_TIMESTAMP(NOW())"
			" AND tPhone.uPeriodCount>tPhone.uDigestThreshold");
	macro_MySQLQueryBasic;
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		
		//if(guDebug)
			printf("%s %s \"%s\" uDateMod:%s uSendPeriod:%s uPeriodCount:%s uDigestThreshold:%s\n",
				field[0],field[1],field[2],field[3],field[4],field[5],field[6]);
	}
	mysql_free_result(res);

	mysql_close(&gMysql);
	logfileLine("Run","Exit");

}//void Run(void)


void Version(void)
{
	printf("svn ID removed
}//void Version(void)


void Set(char *cPhone,const char *cuDigestThreshold,const char *cuReceivePeriod,const char *cuSendPeriod,const char *cuPeriodCount)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPhone=0;
	register int x;

	//TODO
	//Minor dumb cleanup of message for now.
	for(x=0;cPhone[x];x++)
	{
		if(!isdigit(cPhone[x]))
		{
			cPhone[x]=0;
			break;
		}
	}


	if(guDebug)
		logfileLine("Set","Entry");

	TextConnectDb();

	sprintf(gcQuery,"SELECT uPhone FROM tPhone WHERE cNumber='%s'",cPhone);
	macro_MySQLQueryBasic;
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uPhone);
	mysql_free_result(res);

	if(!uPhone)
	{
		sprintf(gcQuery,"INSERT tPhone SET cNumber='%s',uDigestThreshold=%s,uReceivePeriod=%s,uSendPeriod=%s,uPeriodCount=%s",
				cPhone,cuDigestThreshold,cuReceivePeriod,cuSendPeriod,cuPeriodCount);
		macro_MySQLQueryBasic;
		if(guDebug)
			logfileLine("Set","Insert");
	}
	else
	{
		sprintf(gcQuery,"UPDATE tPhone SET uDigestThreshold=%s,uReceivePeriod=%s,uSendPeriod=%s,uPeriodCount=%s WHERE cNumber='%s'",
				cuDigestThreshold,cuReceivePeriod,cuSendPeriod,cuPeriodCount,cPhone);
		macro_MySQLQueryBasic;
		if(guDebug)
			logfileLine("Set","Update");
	}

	mysql_close(&gMysql);

	if(guDebug)
		logfileLine("Set","Exit");

}//void Set()


void QueueMessage(const char *cPhone,char *cMessage)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPhone=0;
	unsigned uDigestThreshold=0;
	unsigned uReceivePeriod=0;
	unsigned uSendPeriod=0;
	unsigned uPeriodCount=0;

	TextConnectDb();
	if(guDebug)
		logfileLine("QueueMessage","Entry");

	//Check
	sprintf(gcQuery,"SELECT uPhone,uDigestThreshold,uReceivePeriod,uSendPeriod,uPeriodCount FROM tPhone WHERE cNumber='%s'",cPhone);
	macro_MySQLQueryBasic;
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uPhone);
		sscanf(field[1],"%u",&uDigestThreshold);
		sscanf(field[2],"%u",&uReceivePeriod);
		sscanf(field[3],"%u",&uSendPeriod);
		sscanf(field[4],"%u",&uPeriodCount);
	}
	mysql_free_result(res);

	//If phone has been configured queue and increase uReceivePeriod window counter uPeriodCount
	if(uPhone && uDigestThreshold && uReceivePeriod && uSendPeriod)
	{
		unsigned uQueue=0;

		if(guDebug)
		{
			sprintf(gcQuery,"%s configured",cPhone);
			logfileLine("QueueMessage",gcQuery);
		}

		sprintf(gcQuery,"SELECT uQueue FROM tQueue WHERE uPhone=%u",uPhone);
		macro_MySQLQueryBasic;
	        res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uQueue);

		if(!uQueue)
		{
			sprintf(gcQuery,"INSERT tQueue SET uPhone=%u,cMessage='%.140s',uDateCreated=UNIX_TIMESTAMP(NOW(),uDateMod=UNIX_TIMESTAMP(NOW())"
				,uPhone,cMessage);
			macro_MySQLQueryBasic;
			if(guDebug)
				logfileLine("QueueMessage","Insert new");
		}
		else
		{

			if(uPeriodCount+1>uDigestThreshold)
			{
				sprintf(gcQuery,"UPDATE tQueue SET cMessage='d%u %s',uDateMod=UNIX_TIMESTAMP(NOW()) WHERE uQueue=%u"
					,uPeriodCount+1,cMessage,uQueue);
				macro_MySQLQueryBasic;
				if(guDebug)
					logfileLine("QueueMessage","Update");
			}
			else
			{
				sprintf(gcQuery,"INSERT tQueue SET uPhone=%u,cMessage='%.140s',uDateCreated=UNIX_TIMESTAMP(NOW(),uDateMod=UNIX_TIMESTAMP(NOW())"
					,uPhone,cMessage);
				macro_MySQLQueryBasic;
				if(guDebug)
					logfileLine("QueueMessage","Insert uDigestThreshold");
			}
		}

		sprintf(gcQuery,"UPDATE tPhone SET uPeriodCount=uPeriodCount+1 WHERE uPhone=%u",uPhone);
		macro_MySQLQueryBasic;
		if(guDebug)
			logfileLine("QueueMessage","Update uPeriodCount++");
	}
	else
	{ 
		//Incorrectly configured phone we log this condition.
		sprintf(gcQuery,"%s not configured",cPhone);
		logfileLine("QueueMessage",gcQuery);
	}

	//

	if(guDebug)
		logfileLine("QueueMessage","Exit");

}//void QueueMessage()


void logfileLine(const char *cFunction,const char *cLogline)
{
	if(gLfp!=NULL)
	{
		time_t luClock;
		char cTime[32];
		pid_t pidThis;
		const struct tm *tmTime;

		pidThis=getpid();

		time(&luClock);
		tmTime=localtime(&luClock);
		strftime(cTime,31,"%b %d %T",tmTime);

		fprintf(gLfp,"%s %s[%u]: %s.\n",cTime,cFunction,pidThis,cLogline);
		if(guDebug)
			fprintf(stdout,"%s %s[%u]: %s.\n",cTime,cFunction,pidThis,cLogline);
		fflush(gLfp);
	}

}//void logfileLine()


int main(int iArgc, char *cArgv[])
{
	struct sysinfo structSysinfo;

	if((gLfp=fopen("/var/log/unxsSMS.log","a"))==NULL)
	{
		fprintf(stderr,"Could not open /var/log/unxsSMS.log error exit\n");
		exit(1);
	}

	sprintf(gcProgram,"%.31s",cArgv[0]);

	if(sysinfo(&structSysinfo))
	{
		logfileLine("main","sysinfo() failed");
		ErrorExit();
	}
#define LINUX_SYSINFO_LOADS_SCALE 65536
#define JOBQUEUE_MAXLOAD 20 //This is equivalent to uptime 20.00 last 1 min avg load
	if(structSysinfo.loads[0]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
	{
		logfileLine("main","structSysinfo.loads[0] larger than JOBQUEUE_MAXLOAD");
		ErrorExit();
	}

	switch(iArgc)
	{
		case 1:
			printf("Usage: %s <cPhone> <cMessage> | initialize <mysql password> | run | debug | version\n"
				"\t\t\tset <cPhone> <uDigestThreshold> <uReceivePeriod> <uSendPeriod> <uPeriodCount>\n",cArgv[0]);
		break;

		case 2:
			if(!strncmp(cArgv[1],"run",3))
			{
				guDebug=0;
				Run();
			}
			else if(!strncmp(cArgv[1],"debug",5))
			{
				guDebug=1;
				Run();
			}
			else if(!strncmp(cArgv[1],"version",7))
			{
				guDebug=1;
				Version();
			}
			else
			{
				sprintf(gcQuery,"unknown command cArgv[1]=%.32s",cArgv[1]);
				logfileLine("main",gcQuery);
				ErrorExit();
			}
		break;

		case 3:
			if(!strncmp(cArgv[1],"initialize",10))
			{
				Initialize(cArgv[2]);
			}
			else if(isdigit(cArgv[1][0]) && strlen(cArgv[1])<33 && strlen(cArgv[2])<145)
			{
				QueueMessage(cArgv[1],cArgv[2]);
			}
			else
			{
				sprintf(gcQuery,"unknown command cArgv[1]=%.32s cArgv[2]=%.140s",cArgv[1],cArgv[2]);
				logfileLine("main",gcQuery);
				ErrorExit();
			}
		break;

		case 7:
			if(!strncmp(cArgv[1],"set",10))
			{
				Set(cArgv[2],cArgv[3],cArgv[4],cArgv[5],cArgv[6]);
			}
			else
			{
				sprintf(gcQuery,"unknown command cArgv[1]=%.32s cArgv[2]=%.32s cArgv[3]=%.32s cArgv[4]=%.32s cArgv[5]=%.32s cArgv[6]=%.32s",
					cArgv[1],cArgv[2],cArgv[3],cArgv[4],cArgv[5],cArgv[6]);
				logfileLine("main",gcQuery);
				ErrorExit();
			}
		break;

		default:
			sprintf(gcQuery,"unknown command iArgc=%d",iArgc);
			logfileLine("main",gcQuery);
			ErrorExit();
	}

	NormalExit();
	return(0);

}//main()


void Initialize(const char *cPasswd)
{
	if(guDebug)
		printf("Initialize()\n");

	if(getuid()!=0)
	{
		logfileLine("Initialize","Must be root to run Initialize");
		ErrorExit();
	}

	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"DROP DATABASE IF EXISTS unxssms");
	macro_MySQLQueryBasic;

	sprintf(gcQuery,"CREATE DATABASE unxssms");
	macro_MySQLQueryBasic;

	sprintf(gcQuery,"USE unxssms");
	macro_MySQLQueryBasic;

	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tPhone ("
			" uPhone INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			" cNumber VARCHAR(32) NOT NULL DEFAULT '', INDEX (cNumber),"
			" uDigestThreshold INT UNSIGNED NOT NULL DEFAULT 0,"
			" uReceivePeriod INT UNSIGNED NOT NULL DEFAULT 0,"
			" uSendPeriod INT UNSIGNED NOT NULL DEFAULT 0,"
			" uPeriodCount INT UNSIGNED NOT NULL DEFAULT 0"
			" )");
	macro_MySQLQueryBasic;

	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tQueue ("
			" uQueue INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			" cMessage VARCHAR(140) NOT NULL DEFAULT '',"
			" uPhone INT UNSIGNED NOT NULL DEFAULT 0,"
			" uDateCreated INT UNSIGNED NOT NULL DEFAULT 0,"
			" uDateMod INT UNSIGNED NOT NULL DEFAULT 0"
			" )");
	macro_MySQLQueryBasic;

	sprintf(gcQuery,"GRANT ALL ON unxssms.* TO unxssms@localhost IDENTIFIED BY 'wsxedc'");
	macro_MySQLQueryBasic;

	if(guDebug)
		printf("Initialize() done\n");

}//void Initialize()


void ErrorExit(void)
{
	fclose(gLfp);
	exit(1);
}//void ErrorExit(void)


void NormalExit(void)
{
	fclose(gLfp);
	exit(0);
}//void NormalExit(void)


void mySQLRootConnect(const char *cPasswd)
{
        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,NULL,"root",cPasswd,"mysql",0,NULL,0))
        {
		logfileLine("mySQLRootConnect","MySQL server not available");
		ErrorExit();
        }

}//void mySQLRootConnect(void)


//Here we will use libcurl to https the message to our SMS gateway provider.
unsigned SendMessage(const char *cPhone,char *cMessage)
{
	CURL *curl;
	CURLcode res;
	char cURL[256];
	register int x;

	//TODO
	//Minor dumb cleanup of message for now.
	for(x=0;cMessage[x];x++)
		if(cMessage[x]==' ' || cMessage[x]=='&') cMessage[x]='+';

	sprintf(cURL,"http://unixservice.com?cPhone=%.32s&cMessage=%.140s",cPhone,cMessage);

	curl = curl_easy_init();
	if(curl)
	{
		curl_easy_setopt(curl,CURLOPT_URL,cURL);
		res=curl_easy_perform(curl);
 
		// always cleanup
		curl_easy_cleanup(curl);

		logfileLine("SendMessage ok",cPhone);
		return(0);
	}

	logfileLine("SendMessage curl error",cPhone);
	return(1);

}//unsigned SendMessage(const char *cPhone,char *cMessage)
