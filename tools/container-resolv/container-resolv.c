/*
FILE
	container-resolv.c
	$Id$
PURPOSE
	update tContainer.uIPNumActive and Backup from DNS
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2014.
	GPLv2 License applies. See LICENSE file.
NOTES
*/

#include <openisp/dns.h>
#include "container-resolv.h"

MYSQL gMysql;
char gcQuery[8192]={""};
char gcProgram[100]={""};
unsigned guLogLevel=4;
unsigned guSilent=0;

//TOC
void TextConnectDb(void);//mysqlconnect.c
void DNSUpdate(unsigned uPBX);
void logfileLine(const char *cFunction,const char *cLogline);

static FILE *gLfp=NULL;

int main(int iArgc, char *cArgv[])
{
	sprintf(gcProgram,"%.99s",cArgv[0]);

	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		logfileLine("main","fopen logfile failed");
		exit(1);
	}

	TextConnectDb();

	if(iArgc==4 && !strncmp(cArgv[3],"silent",6))
		guSilent=1;

	if(iArgc>=2)
	{
		unsigned uContainer=0;
		if(cArgv[2])
			sscanf(cArgv[2],"%u",&uContainer);
		if(!strncmp(cArgv[1],"DNSUpdate",9))
		{
			DNSUpdate(uContainer);
			goto CommonExit;
		}
	}

	printf("Usage: %s DNSUpdate [<uContainer>] [silent]\n",gcProgram);

CommonExit:
	mysql_close(&gMysql);
	fclose(gLfp);
	return(0);

}//main()

void UpdatetContainer(unsigned uPBX,char *cIP,int port,int priority,int weight);
void UpdatetContainer(unsigned uPBX,char *cIP,int port,int priority,int weight)
{
	if(priority<11)
		sprintf(gcQuery,"UPDATE tContainer SET uIPNumActive=INET_ATON('%s'),uResolvDate=UNIX_TIMESTAMP(NOW())"
			" WHERE tContainer.uContainer=%u",cIP,uPBX);
	else
		sprintf(gcQuery,"UPDATE tContainer SET uIPNumBackup=INET_ATON('%s'),uResolvDate=UNIX_TIMESTAMP(NOW())"
			" WHERE tContainer.uContainer=%u",cIP,uPBX);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf(gcQuery);
		logfileLine("UpdatetContainer",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
	sprintf(gcQuery,"cIP=%s port=%u priority=%u weight=%u",cIP,port,priority,weight);
	if(guLogLevel>3)
	{
		printf("%s\n",gcQuery);
		logfileLine("UpdatetContainer",gcQuery);
	}
}//UpdatetContainer()

//Can be used to resolv a single PBX if uPBX!=0
void DNSUpdate(unsigned uPBX)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;
	unsigned uRRCount=0;

	if(!guSilent) printf("DNSUpdate(%u) start\n",uPBX);

	sprintf(gcQuery,"SELECT tContainer.cHostname,tContainer.uContainer"
			" FROM tContainer,tGroup,tGroupGlue"
			" WHERE tGroup.cLabel='Production PBXs'"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tContainer.uContainer=tGroupGlue.uContainer"
			" AND IF(%u,tContainer.uContainer=%u,1)"
				,uPBX,uPBX);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf(gcQuery);
		logfileLine("DNSUpdate",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		unsigned uA=0,uB=0,uC=0,uD=0;
		if(!guSilent)
			printf("%s\n",field[0]);

		//This has to be done first
		sscanf(field[1],"%u",&uPBX);

		//Handle special case where cDomain is an IPv4 number
		if(sscanf(field[0],"%u.%u.%u.%u",&uA,&uB,&uC,&uD)==4)
		{
			//Not sure if this is an exact test see RFC
			if(uA>0 && uA<256 && uB<256 && uC<256 && uD<256)
			{
				//5060,0,0 pattern it is not an A record
				//UpdatetAddressForPBX(uPBX,field[0],5060,0,0);
				UpdatetContainer(uPBX,field[0],5060,0,0);
				sprintf(gcQuery,"%s added as IP to %u\n",field[0],uPBX);
				if(!guSilent)
					printf(gcQuery);
				if(guLogLevel>0)
					logfileLine("DNSUpdate",gcQuery);
				continue;
			}
			//Matches pattern but out of range values
			logfileLine("DNSUpdate",field[0]);
		}
		uCount++;

		//DNS
		dns_host_t sDnsHost;
		dns_host_t sDnsHostSave;
		dns_host_t sDnsHost2;
		dns_host_t sDnsHostSave2;
		dns_srv_t sDnsSrv;
		char cHostname[128]={""};

		//SRV records
		unsigned uSRVCount=0;
		sprintf(cHostname,"_sip._udp.%.99s",field[0]);
		sDnsHost=dns_resolve(cHostname,3);
		if(guLogLevel>3)
			logfileLine("DNSUpdate dns_resolv",cHostname);
		sDnsHostSave=sDnsHost;
		if(sDnsHost!=NULL)
		{
			if(!guSilent)
				printf("SRV records\n");
			sDnsSrv=(dns_srv_t)sDnsHost->rr;

			//Nested get A record
			sprintf(cHostname,"%.99s",sDnsSrv->name);
			sDnsHost2=dns_resolve(cHostname,1);
			if(guLogLevel>3)
				logfileLine("DNSUpdate dns_resolv",cHostname);
			sDnsHostSave2=sDnsHost2;
			if(sDnsHost2!=NULL)
			{
				UpdatetContainer(uPBX,(char *)sDnsHost2->rr,sDnsSrv->port,sDnsSrv->priority,sDnsSrv->weight);
				uSRVCount++;
				if(guLogLevel>3)
					logfileLine("DNSUpdate A",(char *)sDnsHost2->rr);
				uRRCount++;
				while(sDnsHost2->next!=NULL)
				{
					sDnsHost2=sDnsHost2->next;
					UpdatetContainer(uPBX,(char *)sDnsHost2->rr,sDnsSrv->port,sDnsSrv->priority,sDnsSrv->weight);
					uSRVCount++;
					if(guLogLevel>3)
						logfileLine("DNSUpdate A",(char *)sDnsHost2->rr);
					uRRCount++;
				}
			}
			//
			if(guLogLevel>3)
				logfileLine("DNSUpdate SRV",sDnsSrv->name);
			uRRCount++;
			while(sDnsHost->next!=NULL)
			{
				sDnsHost=sDnsHost->next;
				sDnsSrv=(dns_srv_t)sDnsHost->rr;
				//Nested get A record
				sprintf(cHostname,"%.99s",sDnsSrv->name);
				sDnsHost2=dns_resolve(cHostname,1);
				sDnsHostSave2=sDnsHost2;
				if(sDnsHost2!=NULL)
				{
					UpdatetContainer(uPBX,(char *)sDnsHost2->rr,sDnsSrv->port,sDnsSrv->priority,sDnsSrv->weight);
					uSRVCount++;
					if(guLogLevel>3)
						logfileLine("DNSUpdate A",(char *)sDnsHost2->rr);
					uRRCount++;
					while(sDnsHost2->next!=NULL)
					{
						sDnsHost2=sDnsHost2->next;
						UpdatetContainer(uPBX,(char *)sDnsHost2->rr,sDnsSrv->port,sDnsSrv->priority,sDnsSrv->weight);
						uSRVCount++;
						if(guLogLevel>3)
							logfileLine("DNSUpdate A",(char *)sDnsHost2->rr);
						uRRCount++;
					}
			}
			//
				if(guLogLevel>3)
					logfileLine("DNSUpdate SRV",sDnsSrv->name);
				uRRCount++;
			}
		}
		else
		{
			if(guLogLevel>4)
				logfileLine("DNSUpdate No SRV",cHostname);
		}
		dns_free(sDnsHostSave);

		//Only if not two SRV records
		if(uSRVCount<2)
		{
			//A records
			sprintf(cHostname,"%.99s",field[0]);
			sDnsHost=dns_resolve(cHostname,1);
			sDnsHostSave=sDnsHost;
			if(sDnsHost!=NULL)
			{
				//UpdatetAddressForPBX(uPBX,(char *)sDnsHost->rr,5060,1000,100);
				UpdatetContainer(uPBX,(char *)sDnsHost2->rr,5060,0,0);
				if(guLogLevel>3)
					logfileLine("DNSUpdate A not SRV",(char *)sDnsHost->rr);
				uRRCount++;
				while(sDnsHost->next!=NULL)
				{
					sDnsHost=sDnsHost->next;
					//UpdatetAddressForPBX(uPBX,(char *)sDnsHost->rr,5060,1000,100);
					UpdatetContainer(uPBX,(char *)sDnsHost2->rr,5060,0,0);
					if(guLogLevel>3)
						logfileLine("DNSUpdate A not SRV",(char *)sDnsHost->rr);
					uRRCount++;
				}
			}
			else
			{
				if(guLogLevel>0)
					logfileLine("DNSUpdate No A not SRV",cHostname);
				if(!guSilent)
					printf("No A records not SRV\n");
			}
			dns_free(sDnsHostSave);
		}
	}
	mysql_free_result(res);

	if(guLogLevel>0)
	{
		sprintf(gcQuery,"Processed %u cHostnames updated %u tAddress records",uCount,uRRCount);
		logfileLine("DNSUpdate",gcQuery);
	}

	if(!guSilent) printf("DNSUpdate() end\n");
}//void DNSUpdate()


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

		fprintf(gLfp,"%s unxsContainerResolver[%u]::%s %s.\n",cTime,pidThis,cFunction,cLogline);
		fflush(gLfp);
	}
	else
	{
		fprintf(stderr,"%s: unxsContainerResolver::%s.\n",cFunction,cLogline);
	}

}//void logfileLine()
