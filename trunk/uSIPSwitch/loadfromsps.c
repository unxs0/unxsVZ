/*
FILE
	loadfromsps.c
	$Id$
PURPOSE
	Load data from unxsSPS MySQL database into
	localhost memcached for uSIPSwitch use.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2012.
	GPLv2 License applies. See LICENSE file.
NOTES
*/

#include "sipproxy.h"

MYSQL gMysql;
char gcQuery[8192]={""};
char gcProgram[100]={""};
unsigned guLogLevel=4;
unsigned guSilent=0;

void TextConnectDb(void);//mysqlconnect.c
void AddPBXs(char const *cCluster);
void AddDIDs(char const *cCluster);
void AddGWs(char const *cCluster);
void logfileLine(const char *cFunction,const char *cLogline);
int iSetupAndTestMemcached(void);

static FILE *gLfp=NULL;
memcached_st *gsMemc;

int main(int iArgc, char *cArgv[])
{
	sprintf(gcProgram,"%.99s",cArgv[0]);

	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		logfileLine("main","fopen logfile failed");
		exit(1);
	}

	iSetupAndTestMemcached();
	TextConnectDb();

	if(iArgc>=3)
	{
		if(!strncmp(cArgv[1],"AddPBXs",7))
		{
			AddPBXs(cArgv[2]);
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"AddDIDs",7))
		{
			AddDIDs(cArgv[2]);
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"AddGWs",6))
		{
			AddGWs(cArgv[2]);
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"AddAll",6))
		{
			AddPBXs(cArgv[2]);
			AddDIDs(cArgv[2]);
			AddGWs(cArgv[2]);
			goto CommonExit;
		}
	}

	if(iArgc==4 && !strncmp(cArgv[3],"silent",6))
		guSilent=1;

	printf("Usage: %s AddPBXs, AddDIDs, AddGWs, AddRules AddAll <cCluster> [silent]\n",gcProgram);

CommonExit:
	mysql_close(&gMysql);
	fclose(gLfp);
	return(0);

}//main()


void AddGWs(char const *cCluster)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;

	sprintf(gcQuery,"SELECT tGateway.cAddress,tGateway.uPort"
			" FROM tGateway,tCluster"
			" WHERE tGateway.uCluster=tCluster.uCluster"
			" AND tCluster.cLabel='%s'",cCluster);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf(gcQuery);
		logfileLine("AddGWs",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		char cKey[100];
		char cValue[100];
		unsigned rc;

		sprintf(cKey,"%.90s-gw",field[0]);
		sprintf(cValue,"cDestinationIP=%.15s;uDestinationPort=%.5s;",field[0],field[1]);
		rc=memcached_set(gsMemc,cKey,strlen(cKey),cValue,strlen(cValue),(time_t)0,(uint32_t)0);
		if(rc!=MEMCACHED_SUCCESS)
		{
			if(guLogLevel>0)
			{
				logfileLine("AddGWs",cKey);
				logfileLine("AddGWs",cValue);
			}
		}
		else
		{
			uCount++;
		}
		if(!guSilent)
			printf("%s %s\n",cKey,cValue);
	}
	mysql_free_result(res);

	if(guLogLevel>0)
	{
		sprintf(gcQuery,"Added %u keys",uCount);
		logfileLine("AddGWs",gcQuery);
	}

}//void AddGWs()


void AddDIDs(char const *cCluster)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;

	sprintf(gcQuery,"SELECT tDID.cDID,tPBX.cAddress,tPBX.uPort"
			" FROM tPBX,tDID,tCluster"
			" WHERE tDID.uPBX=tPBX.uPBX"
			" AND tDID.uCluster=tCluster.uCluster"
			" AND tCluster.cLabel='%s'",cCluster);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf(gcQuery);
		logfileLine("AddDIDs",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		char cKey[100];
		char cValue[100];
		unsigned rc;

		sprintf(cKey,"%.90s-did",field[0]);
		sprintf(cValue,"cDestinationIP=%.15s;uDestinationPort=%.5s;",field[1],field[2]);
		rc=memcached_set(gsMemc,cKey,strlen(cKey),cValue,strlen(cValue),(time_t)0,(uint32_t)0);
		if(rc!=MEMCACHED_SUCCESS)
		{
			if(guLogLevel>0)
			{
				logfileLine("AddDIDs",cKey);
				logfileLine("AddDIDs",cValue);
			}
		}
		else
		{
			uCount++;
		}
		if(!guSilent)
			printf("%s %s\n",cKey,cValue);
	}
	mysql_free_result(res);

	if(guLogLevel>0)
	{
		sprintf(gcQuery,"Added %u keys",uCount);
		logfileLine("AddDIDs",gcQuery);
	}

}//void AddDIDs()


void AddPBXs(char const *cCluster)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;

	sprintf(gcQuery,"SELECT tPBX.cAddress,tPBX.uPort"
			" FROM tPBX,tCluster"
			" WHERE tPBX.uCluster=tCluster.uCluster"
			" AND tCluster.cLabel='%s'",cCluster);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf(gcQuery);
		logfileLine("AddPBXs",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		char cKey[100];
		char cValue[100];
		unsigned rc;

		sprintf(cKey,"%.90s-pbx",field[0]);
		sprintf(cValue,"cDestinationIP=%.15s;uDestinationPort=%.5s;",field[0],field[1]);
		rc=memcached_set(gsMemc,cKey,strlen(cKey),cValue,strlen(cValue),(time_t)0,(uint32_t)0);
		if(rc!=MEMCACHED_SUCCESS)
		{
			if(guLogLevel>0)
			{
				logfileLine("AddPBXs",cKey);
				logfileLine("AddPBXs",cValue);
			}
		}
		else
		{
			uCount++;
		}
		if(!guSilent)
			printf("%s %s\n",cKey,cValue);
	}
	mysql_free_result(res);

	if(guLogLevel>0)
	{
		sprintf(gcQuery,"Added %u keys",uCount);
		logfileLine("AddPBXs",gcQuery);
	}

}//void AddPBXs()


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

		fprintf(gLfp,"%s unxsLoadFromSPS[%u]::%s %s.\n",cTime,pidThis,cFunction,cLogline);
		fflush(gLfp);
	}
	else
	{
		fprintf(stderr,"%s: unxsLoadFromSPS::%s.\n",cFunction,cLogline);
	}

}//void logfileLine()


int iSetupAndTestMemcached(void)
{
	memcached_server_st *servers = NULL;
	memcached_return rc;
	char *key= "unxsLoadFromSPS";
	char *value= "$Id$";

	memcached_server_st *memcached_servers_parse(const char *server_strings);
	gsMemc=memcached_create(NULL);

	servers=memcached_server_list_append(servers,"localhost",11211,&rc);
	rc=memcached_server_push(gsMemc, servers);
	if(rc!=MEMCACHED_SUCCESS)
	{
		sprintf(gcQuery,"couldn't add server: %s",memcached_strerror(gsMemc, rc));
		logfileLine("iSetupAndTestMemcached",gcQuery);
		return(-1);
	}

	rc=memcached_set(gsMemc,key,strlen(key),value,strlen(value),(time_t)0,(uint32_t)0);
	if(rc!=MEMCACHED_SUCCESS)
	{
		sprintf(gcQuery,"couldn't store test key: %s",memcached_strerror(gsMemc, rc));
		logfileLine("iSetupAndTestMemcached",gcQuery);
		return(-1);
	}

	char cValue[100]={""};
	size_t size=100;
	uint32_t flags=0;
	sprintf(cValue,"%.99s",memcached_get(gsMemc,key,strlen(key),&size,&flags,&rc));
	if(rc!=MEMCACHED_SUCCESS)
	{
		sprintf(gcQuery,"couldn't retrieve test key: %s",memcached_strerror(gsMemc, rc));
		logfileLine("iSetupAndTestMemcached",gcQuery);
		return(-1);
	}

	if(strncmp(cValue,value,size))
	{
		sprintf(gcQuery,"keys differ: (%s) (%s)",cValue,value);
		logfileLine("iSetupAndTestMemcached",gcQuery);
		return(-1);
	}
	logfileLine("iSetupAndTestMemcached","memcached running");

	return(0);

}//int iSetupAndTestMemcached(void)

