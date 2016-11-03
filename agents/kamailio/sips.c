/*
FILE
	sips.c
	svn ID removed
PURPOSE
	Kamailio agent for PBX provisioning and data scraping.
AUTHOR
	(C) 2011-2015 Gary Wallis for Unxiservice, LLC.
	GPLv2 License applies. See LICENSE file.
NOTES

*/

#include "../../mysqlrad.h"
#include <sys/sysinfo.h>

MYSQL gMysql;
MYSQL gMysqlExt;
char gcQuery[8192]={""};
unsigned guLoginClient=1;//Root user
char cHostname[100]={""};
char gcProgram[100]={""};
unsigned guNodeOwner=1;
unsigned guContainerOwner=1;
unsigned guStatus=0;//not a valid status

//dir protos
void SIPTextConnectDb(void);

//local protos
void ProcessDR(void);
void unxsVZJobs(char const *cServer);
void TextConnectKamailioDb(void);
void UpdateJob(unsigned uStatus,unsigned uContainer,unsigned uJob,char *cMessage);
void ParseDIDJobData(char *cJobData,char *cDID,char *cHostname,char *cCustomerName,char *cServer, unsigned *uCustomerLimit);
void Report(void);
void ProvisionDR(void);
unsigned StripNonNumericChars(char *cInput);
unsigned StripNonIPv4Chars(char *cInput);

static FILE *gSIPLfp=NULL;
void SIPlogfileLine(const char *cFunction,const char *cLogline,const unsigned uContainer)
{
	if(gSIPLfp!=NULL)
	{
		time_t luClock;
		char cTime[32];
		pid_t pidThis;
		const struct tm *tmTime;

		pidThis=getpid();

		time(&luClock);
		tmTime=localtime(&luClock);
		strftime(cTime,31,"%b %d %T",tmTime);

		if(uContainer)
			fprintf(gSIPLfp,"%s unxsSIPS::%s[%u]: %s. uContainer=%u\n",cTime,cFunction,pidThis,cLogline,uContainer);
		else
			fprintf(gSIPLfp,"%s unxsSIPS::%s[%u]: %s.\n",cTime,cFunction,pidThis,cLogline);
		fflush(gSIPLfp);
	}
	else
	{
		fprintf(stderr,"%s: unxsSIPS::%s. uContainer=%u\n",cFunction,cLogline,uContainer);
	}

}//void SIPlogfileLine()


int main(int iArgc, char *cArgv[])
{
	struct sysinfo structSysinfo;

	sprintf(gcProgram,"%.99s",cArgv[0]);

	if((gSIPLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		SIPlogfileLine("main","fopen logfile failed",0);
		exit(1);
	}

	if(sysinfo(&structSysinfo))
	{
		SIPlogfileLine("main","sysinfo() failed",0);
		exit(1);
	}
#define LINUX_SYSINFO_LOADS_SCALE 65536
#define JOBQUEUE_MAXLOAD 20 //This is equivalent to uptime 20.00 last 1 min avg load
	if(structSysinfo.loads[0]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
	{
		SIPlogfileLine("main","structSysinfo.loads[0] larger than JOBQUEUE_MAXLOAD",0);
		exit(1);
	}
	//Check to see if this program is still running. If it is exit.
	//This may mean losing data gathering data points. But it
	//will avoid runaway du and other unexpected high load
	//situations. See #120.

	if(iArgc==2)
	{
		if(!strncmp(cArgv[1],"ProcessDR",9))
		{
			ProcessDR();
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"ProvisionDR",11))
		{
			ProvisionDR();
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"Report",9))
		{
			//Report();
			goto CommonExit;
		}
	}
	else if(iArgc==3)
	{
		if(!strncmp(cArgv[1],"unxsVZJobs",10))
		{
			//unxsVZJobs(cArgv[2]);
			goto CommonExit;
		}
	}

	printf("Usage: %s ProcessDR|ProvisionDR\n",gcProgram);

CommonExit:
	fclose(gSIPLfp);
	return(0);

}//main()


void unxsVZJobs(char const *cServer)
{

}//void unxsVZJobs()


void ProcessDR(void)
{
        MYSQL_RES *res2;
        MYSQL_ROW field2;
        MYSQL_RES *res3;
        MYSQL_ROW field3;
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContainer=0;
	unsigned uSelects=0;
	unsigned uInserts=0;
	unsigned uOwner=0;
	unsigned uGwid=0;//OpenSIPS schema
	char cAttrs[256];//OpenSIPS schema
	char cPrefix[65];//OpenSIPS schema

	//Uses login data from local.h
	SIPTextConnectDb();
	TextConnectKamailioDb();
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT tContainer.cHostname,tContainer.uContainer,tContainer.uOwner,tIP.cLabel FROM tContainer,tGroupGlue,tGroup,tIP"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tIP.uIP=tContainer.uIPv4"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel LIKE '%%PBX%%'"
			//" AND tContainer.uSource=0");
			" AND tContainer.uSource=0 LIMIT 4");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		SIPlogfileLine("ProcessDR",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	//debug only
	//if((field=mysql_fetch_row(res)))
	while((field=mysql_fetch_row(res)))
	{
		uGwid=0;
		cAttrs[0]=0;
		cPrefix[0]=0;
		sscanf(field[1],"%u",&uContainer);
		sscanf(field[2],"%u",&uOwner);

		//debug only
		printf("%s %u %s\n",field[0],uContainer,field[3]);
		uSelects++;

		//Wish the OPenSIPS guys would use modern SQL var naming conventions
		//like we do ;)
		sprintf(gcQuery,"SELECT id,scan_prefix,description FROM carrierroute WHERE rewrite_host='%s'",field[0]);
		mysql_query(&gMysqlExt,gcQuery);
		if(mysql_errno(&gMysqlExt))
		{
			SIPlogfileLine("ProcessDR",mysql_error(&gMysqlExt),uContainer);
			mysql_close(&gMysql);
			mysql_close(&gMysqlExt);
			exit(2);
		}
		res2=mysql_store_result(&gMysqlExt);
		if(mysql_num_rows(res2)>0)
		{
			//Clean out for given container only once at top but only if this server has data
			sprintf(gcQuery,"DELETE FROM tProperty WHERE (cName='cOrg_KamailioDID' OR"
					" cName='cOrg_KamailioDesc' OR cName='cOrg_KamailioLimit')"
					" AND uType=3 AND uKey=%u",uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				SIPlogfileLine("ProcessDR",mysql_error(&gMysql),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
		}
		unsigned uOnlyOnce=1;
		while((field2=mysql_fetch_row(res2)))
		{
			sscanf(field2[0],"%u",&uGwid);
			//debug only
			printf("id=%s scan_prefix=%s description=%s\n",field2[0],field2[1],field2[2]);


			sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_KamailioDID',cValue='%s'"
					",uType=3,uKey=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
						field2[1],uContainer,uOwner);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				SIPlogfileLine("ProcessDR",mysql_error(&gMysql),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			uInserts++;

			sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_KamailioDesc',cValue='%s %s'"
					",uType=3,uKey=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
						field2[1],field2[2],uContainer,uOwner);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				SIPlogfileLine("ProcessDR",mysql_error(&gMysql),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			uInserts++;

			//lookup value via IP number only once
			if(uOnlyOnce)
			{
				sprintf(gcQuery,"SELECT key_value FROM call_limit WHERE key_name='%s'",field[3]);
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					SIPlogfileLine("ProcessDR",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
				res3=mysql_store_result(&gMysqlExt);
				if((field3=mysql_fetch_row(res3)))
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_KamailioLimit',cValue='%s'"
						",uType=3,uKey=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
							field3[0],uContainer,uOwner);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						SIPlogfileLine("ProcessDR",mysql_error(&gMysql),uContainer);
						mysql_close(&gMysql);
						mysql_close(&gMysqlExt);
						exit(2);
					}
					uInserts++;
				}
				else
				{
					SIPlogfileLine("ProcessDR","No call_limit data",uContainer);
					SIPlogfileLine("ProcessDR",field[3],uContainer);
				}
				mysql_free_result(res3);
				uOnlyOnce=0;
			}//uOnlyOnce
		}//while container in  carrierroute
		mysql_free_result(res2);
	}//while container
	mysql_free_result(res);

	mysql_close(&gMysql);
	mysql_close(&gMysqlExt);

	printf("uSelects=%u uInserts=%u\n",uSelects,uInserts);
	exit(0);

}//void ProcessDR(void)


void TextConnectKamailioDb(void)
{

#include "local.h"

        mysql_init(&gMysqlExt);
        if (!mysql_real_connect(&gMysqlExt,NULL,KAMAILIO_USER,KAMAILIO_PWD,"kamailio",0,NULL,0))
	{
		SIPlogfileLine("TextConnectKamailioDb","mysql_real_connect()",0);
		exit(3);
	}

}//TextConnectKamailioDb()


void UpdateJob(unsigned uStatus,unsigned uContainer,unsigned uJob,char *cMessage)
{
	char cQuery[512]={""};

	//Update tJob running
	sprintf(cQuery,"UPDATE tJob SET"
			" uJobStatus=%u,"
			" cRemoteMsg='%.99s',"
			" uModBy=1,"
			" uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJob=%u",uStatus,cMessage,uJob);
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
	{
		//debug only
		fprintf(stderr,"Fatal error: %s\n",cQuery);
		SIPlogfileLine("UpdateJob",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		mysql_close(&gMysqlExt);
		exit(2);
	}

}//void UpdateJob()


void ParseDIDJobData(char *cJobData,char *cDID,char *cHostname,char *cCustomerName,char *cServer, unsigned *uCustomerLimit)
{
	char *cp;
	char *cp2;

	if((cp=strstr(cJobData,"cDID=")))
	{
		if((cp2=strchr(cp+5,';')))
		{
			*cp2=0;
			sprintf(cDID,"%.32s",cp+5);
			*cp2=';';
		}
	}
	if((cp=strstr(cJobData,"cHostname=")))
	{
		if((cp2=strchr(cp+10,';')))
		{
			*cp2=0;
			sprintf(cHostname,"%.63s",cp+10);
			*cp2=';';
		}
	}
	if((cp=strstr(cJobData,"cServer=")))
	{
		if((cp2=strchr(cp+8,';')))
		{
			*cp2=0;
			sprintf(cServer,"%.31s",cp+8);
			*cp2=';';
		}
	}
	if((cp=strstr(cJobData,"cCustomerName=")))
	{
		if((cp2=strchr(cp+14,';')))
		{
			*cp2=0;
			sprintf(cCustomerName,"%.32s",cp+14);
			*cp2=';';
		}
	}
	if((cp=strstr(cJobData,"uCustomerLimit=")))
	{
		sscanf(cp+15,"%u;",uCustomerLimit);
	}
}//void ParseDIDJobData()


void Report(void)
{

}//void Report(void)


//Provision from OpenSIPS data in backend. Merge only.
void ProvisionDR(void)
{
        MYSQL_RES *res2;
        MYSQL_ROW field2;
        MYSQL_RES *res3;
        //MYSQL_ROW field3;
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContainer=0;
	unsigned uInserts=0;
	char cLog[128];

	SIPlogfileLine("ProvisionDR","start",0);

	//Uses login data from local.h
	SIPTextConnectDb();
	TextConnectKamailioDb();
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT tContainer.cHostname,tContainer.uContainer,tIP.cLabel FROM tContainer,tGroupGlue,tGroup,tIP"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tIP.uIP=tContainer.uIPv4"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel LIKE '%%PBX%%'"
			" AND tContainer.uSource=0");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		SIPlogfileLine("ProvisionDR",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	//debug only
	//if((field=mysql_fetch_row(res)))
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uContainer);

		//debug only
		//printf("%s %u %s\n",field[0],uContainer,field[2]);

		//call_limit table
		//key_type | value_type
		//Provision concurrent i/o call limit per IP
		if(StripNonIPv4Chars(field[2]))
		{
			sprintf(cLog,"StripNonIPv4Chars(0) (%s)",field[2]);
			SIPlogfileLine("ProvisionDR",cLog,uContainer);
		}
		if(field[2][0])
		{
			sprintf(gcQuery,"SELECT id,key_value FROM call_limit"
					" WHERE key_name='%s' AND key_type=0 AND value_type=1",
						field[2]);
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				SIPlogfileLine("ProvisionDR",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			res3=mysql_store_result(&gMysqlExt);
			if(mysql_num_rows(res3)==0)
			{
				if(field[2][0])
				{
					sprintf(gcQuery,"INSERT INTO call_limit"
								" SET key_name='%s',key_type=0,value_type=1,key_value='50'",field[2]);
					mysql_query(&gMysqlExt,gcQuery);
					if(mysql_errno(&gMysqlExt))
					{
						SIPlogfileLine("ProvisionDR",mysql_error(&gMysqlExt),uContainer);
						mysql_close(&gMysql);
						mysql_close(&gMysqlExt);
						exit(2);
					}
					uInserts++;
					sprintf(cLog,"call_limit %.32s added",field[2]);
					SIPlogfileLine("ProvisionDR",cLog,uContainer);
				}
			}
			else
			{
				//next we will update if key_value is different from what we have in the backend.
				//debug only
				//printf("\tcall_limit %s already there\n",field[2]);
				;//nop
			}

			//address table
			//Provision IPs that are allowed to use this SIP proxy
			sprintf(gcQuery,"SELECT id FROM address"
						" WHERE ip_addr='%s' AND grp=1 AND mask=32 AND port=0",
							field[2]);
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				SIPlogfileLine("ProvisionDR",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			res3=mysql_store_result(&gMysqlExt);
			if(mysql_num_rows(res3)==0)
			{
				if(field[2][0])
				{
					sprintf(gcQuery,"INSERT INTO address"
								" SET ip_addr='%s',grp=1,mask=32,port=0,tag='unxsSIPS %.54s'",field[2],field[0]);
					mysql_query(&gMysqlExt,gcQuery);
					if(mysql_errno(&gMysqlExt))
					{
						SIPlogfileLine("ProvisionDR",mysql_error(&gMysqlExt),uContainer);
						mysql_close(&gMysql);
						mysql_close(&gMysqlExt);
						exit(2);
					}
					uInserts++;
					//debug only
					sprintf(cLog,"%.32s added",field[2]);
					SIPlogfileLine("ProvisionDR",cLog,uContainer);
				}
			}
			else
			{
				//debug only
				//printf("\t%s already there\n",field[2]);
				;//nop
			}

		}//non empty IP field[2][0]

		//Next in main loop
		//Provision DIDs.
		sprintf(gcQuery,"SELECT TRIM(cValue) FROM tProperty WHERE cName='cOrg_OpenSIPS_DID' AND uType=3 AND uKey=%u",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			SIPlogfileLine("ProvisionDR",mysql_error(&gMysql),uContainer);
			mysql_close(&gMysql);
			mysql_close(&gMysqlExt);
			exit(2);
		}
		res2=mysql_store_result(&gMysql);
		while((field2=mysql_fetch_row(res2)))
		{
			char cDID[32]={""};
			sprintf(cDID,"%.31s",field2[0]);
			if(StripNonNumericChars(field2[0]))
			{
				sprintf(cLog,"StripNonNumericChars(1) (%.32s/%.32s)",cDID,field2[0]);
				SIPlogfileLine("ProvisionDR",cLog,uContainer);
			}

			if(field2[0][0])
			{
				sprintf(gcQuery,"SELECT id,scan_prefix,description FROM carrierroute"
					" WHERE scan_prefix='%s'",
						field2[0]);
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					SIPlogfileLine("ProvisionDR",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
				res3=mysql_store_result(&gMysqlExt);
				if(mysql_num_rows(res3)==0)
				{
					sprintf(gcQuery,"INSERT INTO carrierroute"
							" SET scan_prefix='%s',rewrite_host='%s',description='%s',carrier=1,domain=1,prob=1",
									field2[0],field[0],"cOrg_OpenSIPS_DID");
					mysql_query(&gMysqlExt,gcQuery);
					if(mysql_errno(&gMysqlExt))
					{
						SIPlogfileLine("ProvisionDR",mysql_error(&gMysqlExt),uContainer);
						mysql_close(&gMysql);
						mysql_close(&gMysqlExt);
						exit(2);
					}
					uInserts++;
					sprintf(cLog,"%.32s added",field2[0]);
					SIPlogfileLine("ProvisionDR",cLog,uContainer);
				}
				else
				{
					//debug only
					//printf("\t%s already there\n",field2[0]);
					;//nop
				}
			}//if non empty DID
		}//while DID for given container
	}//while container
	mysql_free_result(res);

	mysql_close(&gMysql);
	mysql_close(&gMysqlExt);
	sprintf(cLog,"uInserts=%u",uInserts);
	SIPlogfileLine("ProvisionDR",cLog,uContainer);

	SIPlogfileLine("ProvisionDR","end",0);
	exit(0);

}//void ProvisionDR(void)

#include <ctype.h>
unsigned StripNonNumericChars(char *cInput)
{
	register unsigned i;
	unsigned uStrip=0;
	for(i=0;cInput[i];i++)
	{
		if(!isdigit(cInput[i]))
		{
			uStrip++;
			break;
		}
	}
	cInput[i]=0;
	return(uStrip);
}//void StripNonNumericChars(char *cInput)


unsigned StripNonIPv4Chars(char *cInput)
{
	register unsigned i;
	unsigned uStrip=0;
	for(i=0;cInput[i];i++)
	{
		if(!isdigit(cInput[i]) && cInput[i]!='.')
		{
			uStrip++;
			break;
		}
	}
	cInput[i]=0;
	return(uStrip);
}//void StripNonIPv4Chars(char *cInput)
