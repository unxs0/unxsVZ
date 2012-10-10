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

//TOC
void TextConnectDb(void);//mysqlconnect.c
void AddPBXs(char const *cCluster);
void AddDIDs(char const *cCluster);
void AddGWs(char const *cCluster);
void logfileLine(const char *cFunction,const char *cLogline);
int iSetupAndTestMemcached(void);
void AddRules(char const *cCluster);

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

	if(iArgc==4 && !strncmp(cArgv[3],"silent",6))
		guSilent=1;

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
		else if(!strncmp(cArgv[1],"AddRules",8))
		{
			AddRules(cArgv[2]);
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"AddAll",6))
		{
			AddPBXs(cArgv[2]);
			AddDIDs(cArgv[2]);
			AddGWs(cArgv[2]);
			AddRules(cArgv[2]);
			goto CommonExit;
		}
	}

	printf("Usage: %s AddPBXs, AddDIDs, AddGWs, AddRules, AddAll <cCluster> [silent]\n",gcProgram);

CommonExit:
	mysql_close(&gMysql);
	fclose(gLfp);
	return(0);

}//main()


//Inbound gateways
//(the outbound gateways are added in the AddRules -rule)
void AddGWs(char const *cCluster)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;

	sprintf(gcQuery,"SELECT tAddress.cIP,tAddress.uPort,tAddress.uPriority,tAddress.uWeight"
			" FROM tGateway,tCluster,tAddress"
			" WHERE tGateway.uCluster=tCluster.uCluster"
			" AND tAddress.uGateway=tGateway.uGateway"
			" AND tGateway.uGatewayType=1"//DID Inbound
			" AND tCluster.cLabel='%s'"
			" GROUP BY tGateway.uGateway"
			" ORDER BY tAddress.uPriority,tAddress.uWeight"
				,cCluster);
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
		sprintf(cValue,"cDestinationIP=%.15s;uDestinationPort=%.5s;uType=1;uPriority=%s;uWeight=%s;",
				field[0],field[1],field[2],field[3]);
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

	sprintf(gcQuery,"SELECT tDID.cDID,tAddress.cIP,tAddress.uPort,tAddress.uPriority,tAddress.uWeight"
			" FROM tAddress,tPBX,tDID,tCluster"
			" WHERE tDID.uPBX=tPBX.uPBX"
			" AND tPBX.uPBX=tAddress.uPBX"
			" AND tDID.uCluster=tCluster.uCluster"
			" AND tCluster.cLabel='%s'"
			" GROUP BY tDID.uDID"
			" ORDER BY tAddress.uPriority,tAddress.uWeight"
				,cCluster);
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
		sprintf(cValue,"cDestinationIP=%.15s;uDestinationPort=%.5s;uPriority=%s;uWeight=%s;",
				field[1],field[2],field[3],field[4]);
		rc=memcached_set(gsMemc,cKey,strlen(cKey),cValue,strlen(cValue),(time_t)0,(uint32_t)0);
		if(rc!=MEMCACHED_SUCCESS)
		{
			if(!guSilent)
				printf("Error: %s %s\n",cKey,cValue);
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

	sprintf(gcQuery,"SELECT tAddress.cIP,tAddress.uPort,tAddress.uPriority,tAddress.uWeight"
			" FROM tPBX,tCluster,tAddress"
			" WHERE tPBX.uCluster=tCluster.uCluster"
			" AND tAddress.uPBX=tPBX.uPBX"
			" AND tCluster.cLabel='%s'"
			" GROUP BY tPBX.uPBX"
			" ORDER BY tAddress.uPriority,tAddress.uWeight"
				,cCluster);
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

		//PBXs are gateways too. This makes the server run faster.
		sprintf(cKey,"%.90s-gw",field[0]);
		sprintf(cValue,"cDestinationIP=%.15s;uDestinationPort=%.5s;uType=2;uPriority=%s;uWeight=%s;",
				field[0],field[1],field[2],field[3]);
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


void AddRules(char const *cCluster)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_ROW field2;
	unsigned uRuleCount=0;

	if(!guSilent) printf("AddRules() start\n");
	sprintf(gcQuery,"SELECT tRule.uRule,tRule.cLabel,if(tRule.cPrefix='','Any',tRule.cPrefix),tRule.uPriority"
			" FROM tRule,tGroupGlue,tTimeInterval"
			" WHERE tTimeInterval.uTimeInterval=tGroupGlue.uKey"
			" AND tGroupGlue.uGroup=tRule.uRule"
			" AND tGroupGlue.uGroupType=1"
			" AND IF(tTimeInterval.cStartDate='',1,DATE(NOW())>=tTimeInterval.cStartDate)"
			" AND IF(tTimeInterval.cEndDate='',1,DATE(NOW())<=tTimeInterval.cEndDate)"
			" AND IF(tTimeInterval.cStartTime='',1,TIME(NOW())>=tTimeInterval.cStartTime)"
			" AND IF(tTimeInterval.cEndTime='',1,TIME(NOW())<=tTimeInterval.cEndTime)"
			" AND INSTR(tTimeInterval.cDaysOfWeek,DAYOFWEEK(NOW()))>0"
			" ORDER BY tRule.uPriority");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf(gcQuery);
		logfileLine("AddRules",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res2=mysql_store_result(&gMysql);
	while((field2=mysql_fetch_row(res2)))
	{
		unsigned uCount=0;
		char cData[1024]={""};
		sprintf(cData,"uRule=%.16s;cLabel=%.32s;cPrefix=%.32s;uPriority=%.5s;\n",field2[0],field2[1],field2[2],field2[3]);

		sprintf(gcQuery,"SELECT DISTINCT tAddress.cIP,tAddress.uPort,tAddress.uPriority,tAddress.uWeight"
			" FROM tRule,tGroupGlue,tGateway,tAddress,tCluster"
			" WHERE tGateway.uCluster=tCluster.uCluster"
			" AND tGateway.uGateway=tGroupGlue.uKey"
			" AND tGroupGlue.uGroup=tRule.uRule"
			" AND tRule.uRule=%s"
			" AND tAddress.uGateway=tGateway.uGateway"
			" AND tGroupGlue.uGroupType=2"
			" AND tGateway.uGatewayType=2"//PSTN Outbound
			" AND tCluster.cLabel='%s'"
			" ORDER BY tAddress.uPriority,tAddress.uWeight"
				,field2[0],cCluster);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf(gcQuery);
			logfileLine("AddRules",mysql_error(&gMysql));
			mysql_close(&gMysql);
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			if(uCount>8) break;
			char cValue[128]={""};
			sprintf(cValue,"cDestinationIP=%.15s;uDestinationPort=%.5s;uPriority=%.5s,uWeight=%.5s;\n",
						field[0],field[1],field[2],field[3]);
			if((strlen(cData)+strlen(cValue))<sizeof(cData))
				strcat(cData,cValue);
			uCount++;
		}//while ips
		mysql_free_result(res);

		//We created the data string now commit to memcached
		char cKey[100];
		unsigned rc;
		sprintf(cKey,"%u-rule",uRuleCount++);
		rc=memcached_set(gsMemc,cKey,strlen(cKey),cData,strlen(cData),(time_t)0,(uint32_t)0);
		if(rc!=MEMCACHED_SUCCESS)
		{
			if(guLogLevel>0)
			{
				logfileLine("AddRules error",cKey);
				logfileLine("AddRules error",cData);
			}
		}
		if(!guSilent)
			printf("%s %s\n",cKey,cData);

		if(guLogLevel>2)
		{
			sprintf(gcQuery,"Added 1 key with %u outbound servers",uCount);
			logfileLine("AddRules",gcQuery);
		}
	}//while rules
	if(!guSilent) printf("AddRules() end\n");

}//void AddRules()
