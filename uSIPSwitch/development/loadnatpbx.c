/*
FILE
	loadnatpbx.c
	svn ID removed
PURPOSE
	Load unxsVZ NatPBX Hostnames, IPs and Ports into
	localhost memcached.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2012.
	GPLv2 License applies. See LICENSE file.
NOTES
*/

#include "sipproxy.h"

MYSQL gMysql;
char gcQuery[8192]={""};
char gcProgram[100]={""};

void TextConnectDb(void);//mysqlconnect.c
void AddNatPBXs(char const *cServer);
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

	if(iArgc==3)
	{
		if(!strncmp(cArgv[1],"AddNatPBXs",9))
		{
			AddNatPBXs(cArgv[2]);
			goto CommonExit;
		}
	}

	printf("Usage: %s AddNatPBXs <cServer>\n",gcProgram);

CommonExit:
	fclose(gLfp);
	return(0);

}//main()


void AddNatPBXs(char const *cServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	TextConnectDb();

	sprintf(gcQuery,"SELECT tContainer.cHostname,tIP.cLabel,tProperty.cValue"
			" FROM tContainer,tGroup,tGroupGlue,tIP,tNode,tProperty"
			" WHERE tGroup.cLabel='NatPBX'"
			" AND tGroupGlue.uGroup=tGroup.uGroup"
			" AND tGroupGlue.uContainer=tContainer.uContainer"
			" AND tContainer.uContainer=tProperty.uKey"
			" AND tProperty.cName='cOrg_SIPPort'"
			" AND tProperty.uType=3"//tType.cLabel='VZ Container'
			" AND tContainer.uNode=tNode.uNode"
			" AND tNode.cLabel='%s'"
			" AND tContainer.uIPv4=tIP.uIP",cServer);
		printf(gcQuery);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		//printf(gcQuery);
		logfileLine("AddNatPBXs",mysql_error(&gMysql));
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
		sprintf(cValue,"cDestinationIP=%.15s;uDestinationPort=%.5s;",field[1],field[2]);
		rc=memcached_set(gsMemc,cKey,strlen(cKey),cValue,strlen(cValue),(time_t)0,(uint32_t)0);
		if(rc!=MEMCACHED_SUCCESS)
			logfileLine("AddNatPBXs",cKey);
		printf("%s %s\n",cKey,cValue);
	}
	mysql_free_result(res);

	mysql_close(&gMysql);
	exit(0);

}//void AddNatPBXs()


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

		fprintf(gLfp,"%s unxsLoadNatPBX[%u]::%s %s.\n",cTime,pidThis,cFunction,cLogline);
		fflush(gLfp);
	}
	else
	{
		fprintf(stderr,"%s: unxsLoadNatPBX::%s.\n",cFunction,cLogline);
	}

}//void logfileLine()


int iSetupAndTestMemcached(void)
{
	memcached_server_st *servers = NULL;
	memcached_return rc;
	char *key= "unxsLoadNatPBX";
	char *value= "svn ID removed

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

