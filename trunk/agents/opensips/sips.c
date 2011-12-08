/*
FILE
	sips.c
	$Id$
PURPOSE
	Collection of diverse VZ operating parameters and other system vars.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2011.
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
void TextConnectDb(void);

//local protos
void ProcessDR(void);
void unxsVZJobs(void);
void TextConnectOpenSIPSDb(void);
void UpdateJob(unsigned uStatus,unsigned uContainer,unsigned uJob,char *cMessage);
void ParseDIDJobData(char *cJobData,char *cDID,char *cHostname,char *cCustomerName);

static FILE *gLfp=NULL;
void logfileLine(const char *cFunction,const char *cLogline,const unsigned uContainer)
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

		fprintf(gLfp,"%s unxsSIPS::%s[%u]: %s. uContainer=%u\n",cTime,cFunction,pidThis,cLogline,uContainer);
		fflush(gLfp);
	}
	else
	{
		fprintf(stderr,"%s: unxsSIPS::%s. uContainer=%u\n",cFunction,cLogline,uContainer);
	}

}//void logfileLine()


int main(int iArgc, char *cArgv[])
{
	struct sysinfo structSysinfo;

	sprintf(gcProgram,"%.99s",cArgv[0]);

	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		logfileLine("main","fopen logfile failed",0);
		exit(1);
	}

	if(sysinfo(&structSysinfo))
	{
		logfileLine("main","sysinfo() failed",0);
		exit(1);
	}
#define LINUX_SYSINFO_LOADS_SCALE 65536
#define JOBQUEUE_MAXLOAD 20 //This is equivalent to uptime 20.00 last 1 min avg load
	if(structSysinfo.loads[0]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
	{
		logfileLine("main","structSysinfo.loads[0] larger than JOBQUEUE_MAXLOAD",0);
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
		else if(!strncmp(cArgv[1],"unxsVZJobs",10))
		{
			unxsVZJobs();
			goto CommonExit;
		}
	}

	printf("Usage: %s ProcessDR|unxsVZJobs\n",gcProgram);

CommonExit:
	fclose(gLfp);
	return(0);

}//main()


void unxsVZJobs(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_ROW field2;
	unsigned uDRReload=0;
	unsigned uContainer;

	//Uses login data from local.h
	TextConnectDb();
	TextConnectOpenSIPSDb();
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT uJob,uDatacenter,uNode,uContainer,uOwner,cJobName,cJobData FROM tJob"
			" WHERE uJobStatus=10"
			" AND cJobName LIKE 'unxsSIPS%%'"
			" AND uJobDate<=UNIX_TIMESTAMP(NOW()) LIMIT 100");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("unxsVZJobs",mysql_error(&gMysql),0);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		char cDID[16]={""};
		char cHostname[64]={""};
		char cJobName[33]={""};
		char cCustomerName[33]={""};
		char cMessage[128]={""};
		unsigned uJob=0;
		unsigned uDatacenter=0;
		unsigned uNode=0;
		uContainer=0;
		unsigned uOwner=0;
		unsigned uGwid=0;//OpenSIPS schema

		sscanf(field[0],"%u",&uJob);
		sscanf(field[1],"%u",&uDatacenter);
		sscanf(field[2],"%u",&uNode);
		sscanf(field[3],"%u",&uContainer);
		sscanf(field[4],"%u",&uOwner);
		sprintf(cJobName,"%.32s",field[5]);

		if(!strncmp(cJobName,"unxsSIPSNewDID",14))
		{
			//Update tJob running
			UpdateJob(2,uContainer,uJob,"");

			ParseDIDJobData(field[6],cDID,cHostname,cCustomerName);

			//Make sure PBX is registered
			sprintf(gcQuery,"SELECT gwid,attrs FROM dr_gateways WHERE type=1 AND address='%s'",cHostname);
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				//Update tJob error
				UpdateJob(14,uContainer,uJob,gcQuery);
				logfileLine("unxsSIPSNewDID",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			res2=mysql_store_result(&gMysqlExt);
			if((field2=mysql_fetch_row(res2)))
				sscanf(field2[0],"%u",&uGwid);
			mysql_free_result(res2);

			//If unxsVZ.tProperty cOrg_LinesContracted exists we can safely register and get a new uGwid
			if(!uGwid)
			{
        			MYSQL_RES *res;
			        MYSQL_ROW field;
				unsigned uLinesContracted=0;
				char cCustomerName[64]={""};

				sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=3 AND cName='cOrg_LinesContracted'",
					uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					//Update tJob error
					UpdateJob(14,uContainer,uJob,gcQuery);
					logfileLine("unxsSIPSNewDID",mysql_error(&gMysql),0);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			        res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
					sscanf(field[0],"%u",&uLinesContracted);
				mysql_free_result(res);

				sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=3 AND cName='cOrg_CustomerName'",
					uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					//Update tJob error
					UpdateJob(14,uContainer,uJob,gcQuery);
					logfileLine("unxsSIPSNewDID",mysql_error(&gMysql),0);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			        res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
					sprintf(cCustomerName,"%.63s",field[0]);
				mysql_free_result(res);

				if(!cCustomerName[0])
					sprintf(cCustomerName,"%.63s",cHostname);
				
				if(uLinesContracted)
				{
					//gwid    type    address strip   pri_prefix      attrs   probe_mode      description
					sprintf(gcQuery,"INSERT INTO dr_gateways SET"
							" type=1,"
							" address='%s',"
							" attrs='unxsvzOrg|%u',"
							" description='%s'"
									,cHostname,uLinesContracted,cCustomerName);
					mysql_query(&gMysqlExt,gcQuery);
					if(mysql_errno(&gMysqlExt))
					{
						//Update tJob error
						UpdateJob(14,uContainer,uJob,gcQuery);
						logfileLine("unxsSIPSNewDID",mysql_error(&gMysqlExt),uContainer);
						mysql_close(&gMysql);
						mysql_close(&gMysqlExt);
						exit(2);
					}
					logfileLine("unxsSIPSNewDID","dr_gateways record added",uContainer);
					uGwid=mysql_insert_id(&gMysqlExt);
				}
			}

			if(uGwid)
			{
				//Add new DID only if not already in table
				sprintf(gcQuery,"SELECT prefix FROM dr_rules WHERE gwlist='%u' AND prefix='%s'",
								uGwid,cDID);
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					//Update tJob error
					UpdateJob(14,uContainer,uJob,gcQuery);
					logfileLine("unxsSIPSNewDID",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
				res2=mysql_store_result(&gMysqlExt);
				if(mysql_num_rows(res2)==0)
				{
					sprintf(gcQuery,"INSERT INTO dr_rules SET"
							" groupid=1,"//Magic number TODO! 1 is for PBX, 2 is the id for trunks.
							" prefix='%s',"
							" gwlist='%u',"
							" description='%s'",
								cDID,uGwid,cHostname);
					//debug only
					//printf("%s\n",gcQuery);
					mysql_query(&gMysqlExt,gcQuery);
					if(mysql_errno(&gMysqlExt))
					{
						//Update tJob error
						UpdateJob(14,uContainer,uJob,gcQuery);
						logfileLine("unxsSIPSNewDID",mysql_error(&gMysqlExt),uContainer);
						mysql_close(&gMysql);
						mysql_close(&gMysqlExt);
						exit(2);
					}

					sprintf(cMessage,"Added %.11s for %.32s",cDID,cHostname);
					//debug only
					//printf("%s\n",cMessage);

					uDRReload=uJob;

					//Rename pending DID
					sprintf(gcQuery,"UPDATE tProperty SET cName='cOrg_OpenSIPS_DID',uModBy=1,"
							"uModDate=UNIX_TIMESTAMP(NOW()) WHERE cName='cOrg_Pending_DID'"
							" AND cValue='%s'"
							" AND uKey=%u"
							" AND uType=3",cDID,uContainer);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						//Update tJob error
						UpdateJob(14,uContainer,uJob,gcQuery);
						logfileLine("unxsSIPSNewDID",mysql_error(&gMysql),uContainer);
						mysql_close(&gMysql);
						mysql_close(&gMysqlExt);
						exit(2);
					}
				}
				else
				{
					sprintf(cMessage,"%.11s for %.32s in dr_rules",cDID,cHostname);
					//debug only
					//printf("%s\n",cMessage);
				}
			}
			else
			{
				sprintf(cMessage,"%.32s not in dr_gateways",cHostname);
				//debug only
				//printf("%s\n",cMessage);
			}

			//Update tJob OK
			UpdateJob(3,uContainer,uJob,cMessage);
			logfileLine("unxsSIPSNewDID",cMessage,uContainer);

		}//unxsSIPSNewDID
		else if(!strncmp(cJobName,"unxsSIPSRemoveDID",14))
		{
			//Update tJob running
			UpdateJob(2,uContainer,uJob,"");

			ParseDIDJobData(field[6],cDID,cHostname,cCustomerName);

			//Make sure PBX is registered
			sprintf(gcQuery,"SELECT gwid,attrs FROM dr_gateways WHERE type=1 AND address='%s'",cHostname);
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				//Update tJob error
				UpdateJob(14,uContainer,uJob,gcQuery);
				logfileLine("unxsSIPSRemoveDID",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			res2=mysql_store_result(&gMysqlExt);
			if((field2=mysql_fetch_row(res2)))
				sscanf(field2[0],"%u",&uGwid);
			mysql_free_result(res2);

			if(uGwid)
			{
				//Remove DID
				sprintf(gcQuery,"DELETE FROM dr_rules WHERE gwlist='%u' AND prefix='%s'",
								uGwid,cDID);
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					//Update tJob error
					UpdateJob(14,uContainer,uJob,gcQuery);
					logfileLine("unxsSIPSRemoveDID",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}

				sprintf(cMessage,"Removed %.11s for %.32s",cDID,cHostname);
				//debug only
				//printf("%s\n",cMessage);

				uDRReload=uJob;

				//Remove from unxsVZ
				sprintf(gcQuery,"DELETE FROM tProperty WHERE cName='cOrg_Remove_DID'"
							" AND cValue='%s'"
							" AND uKey=%u"
							" AND uType=3",cDID,uContainer);
					mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					//Update tJob error
					UpdateJob(14,uContainer,uJob,gcQuery);
					logfileLine("unxsSIPSRemoveDID",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			else
			{
				sprintf(cMessage,"%.32s not in dr_gateways",cHostname);
				//debug only
				//printf("%s\n",cMessage);
			}

			//Update tJob OK
			UpdateJob(3,uContainer,uJob,cMessage);
			logfileLine("unxsSIPSRemoveDID",cMessage,uContainer);

		}//unxsSIPSRemoveDID
		else if(!strncmp(cJobName,"unxsSIPSModCustomerName",22))
		{
			//Update tJob running
			UpdateJob(2,uContainer,uJob,"");

			ParseDIDJobData(field[6],cDID,cHostname,cCustomerName);

			//Make sure PBX is registered
			sprintf(gcQuery,"SELECT gwid FROM dr_gateways WHERE type=1 AND address='%s'",cHostname);
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				//Update tJob error
				UpdateJob(14,uContainer,uJob,gcQuery);
				logfileLine("unxsSIPSModCustomerName",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			res2=mysql_store_result(&gMysqlExt);
			if((field2=mysql_fetch_row(res2)))
				sscanf(field2[0],"%u",&uGwid);
			mysql_free_result(res2);

			if(uGwid)
			{
				//Update description
				sprintf(gcQuery,"UPDATE dr_gateways SET description='%s' WHERE gwid='%u'",cCustomerName,uGwid);
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					//Update tJob error
					UpdateJob(14,uContainer,uJob,gcQuery);
					logfileLine("unxsSIPSModCustomerName",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}

				//Delete previous
				sprintf(gcQuery,"DELETE FROM tProperty"
						" WHERE cName='cOrg_CustomerName'"
						" AND uKey=%u"
						" AND uType=3",uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					//Update tJob error
					UpdateJob(14,uContainer,uJob,gcQuery);
					logfileLine("unxsSIPSModCustomerName",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}

				//Rename pending name change
				sprintf(gcQuery,"UPDATE tProperty SET cName='cOrg_CustomerName',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
						" WHERE cName='cOrg_CustomerMod'"
						" AND cValue='%s'"
						" AND uKey=%u"
						" AND uType=3",cCustomerName,uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					//Update tJob error
					UpdateJob(14,uContainer,uJob,gcQuery);
					logfileLine("unxsSIPSModCustomerName",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}

				sprintf(cMessage,"Updated %.11s for %.32s",cCustomerName,cHostname);
				//debug only
				//printf("%s\n",cMessage);
			}
			else
			{
				sprintf(cMessage,"%.32s not in dr_gateways",cHostname);
				//debug only
				//printf("%s\n",cMessage);
			}

			//Update tJob OK
			UpdateJob(3,uContainer,uJob,cMessage);
			logfileLine("unxsSIPSModCustomerName",cMessage,uContainer);

		}//unxsSIPSModCustomerName
	}
	mysql_free_result(res);

	if(uDRReload)
	{
		//debug only
		//printf("Reloading DR rules...\n");
		sprintf(gcQuery,"/usr/sbin/opensipsctl fifo dr_reload");	
		if(system(gcQuery))
		{
			logfileLine("unxsVZJobs",gcQuery,uContainer);
			//debug only
			//printf("Failed!\n");
			//At least mark the last one (uDRReload=tJob.uJob) as error to notify operator
			UpdateJob(14,0,uDRReload,gcQuery);
		}
		else
		{
			logfileLine("unxsVZJobs","DR rules reloaded ok",uContainer);
			//debug only
			//printf("Done\n");
		}
	}

	mysql_close(&gMysql);
	mysql_close(&gMysqlExt);
	exit(0);


}//void unxsVZJobs(void)


void ProcessDR(void)
{
        MYSQL_RES *res2;
        MYSQL_ROW field2;
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
	TextConnectDb();
	TextConnectOpenSIPSDb();
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT tContainer.cHostname,tContainer.uContainer,tContainer.uOwner FROM tContainer,tGroupGlue,tGroup"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel LIKE '%%PBX%%'"
			" AND tContainer.uSource=0");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessDR",mysql_error(&gMysql),uContainer);
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
		//printf("%s %u\n",field[0],uContainer);
		uSelects++;

		//Wish the OPenSIPS guys would use modern SQL var naming conventions
		//like we do ;)
		sprintf(gcQuery,"SELECT gwid,attrs,description FROM dr_gateways WHERE type=1 AND address='%s'",field[0]);
		mysql_query(&gMysqlExt,gcQuery);
		if(mysql_errno(&gMysqlExt))
		{
			logfileLine("ProcessDR",mysql_error(&gMysqlExt),uContainer);
			mysql_close(&gMysql);
			mysql_close(&gMysqlExt);
			exit(2);
		}
		res2=mysql_store_result(&gMysqlExt);
		if((field2=mysql_fetch_row(res2)))
		{
			sscanf(field2[0],"%u",&uGwid);
			//sprintf(cAttrs,"%.255s",field2[1]);
			//debug only
			//printf("gwid=%s attrs=%s\n",field2[0],field2[1]);

			//Clean out
			sprintf(gcQuery,"DELETE FROM tProperty WHERE (cName='cOrg_OpenSIPS_DID' OR"
					" cName='cOrg_OpenSIPS_Attrs' OR cName='cOrg_CustomerName')"
					" AND uType=3 AND uKey=%u",uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessDR",mysql_error(&gMysql),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}

			sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_OpenSIPS_Attrs',cValue='%s'"
					",uType=3,uKey=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
						field2[1],uContainer,uOwner);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessDR",mysql_error(&gMysql),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			uInserts++;

			sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_CustomerName',cValue='%s'"
					",uType=3,uKey=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
						field2[2],uContainer,uOwner);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessDR",mysql_error(&gMysql),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			uInserts++;
		}
		mysql_free_result(res2);

		if(uGwid)
		{
			sprintf(gcQuery,"SELECT prefix FROM dr_rules WHERE gwlist='%u'",uGwid);
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				logfileLine("ProcessDR",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			res2=mysql_store_result(&gMysqlExt);
			while((field2=mysql_fetch_row(res2)))
			{
				sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_OpenSIPS_DID',cValue='%s'"
					",uType=3,uKey=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
						field2[0],uContainer,uOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("ProcessDR",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
				//debug only
				//printf("\tprefix=%s\n",field2[0]);
				uInserts++;
	
			}
			mysql_free_result(res2);
		}
		
	}
	mysql_free_result(res);

	mysql_close(&gMysql);
	mysql_close(&gMysqlExt);

	printf("uSelects=%u uInserts=%u\n",uSelects,uInserts);
	exit(0);

}//void ProcessDR(void)


void TextConnectOpenSIPSDb(void)
{

#include "local.h"

        mysql_init(&gMysqlExt);
        if (!mysql_real_connect(&gMysqlExt,NULL,"opensips",OPENSIPSPWD,"opensips",0,NULL,0))
	{
		logfileLine("TextConnectOpenSIPSDb","mysql_real_connect()",0);
		exit(3);
	}

}//TextConnectOpenSIPSDb()


void UpdateJob(unsigned uStatus,unsigned uContainer,unsigned uJob,char *cMessage)
{
	//Update tJob running
	sprintf(gcQuery,"UPDATE tJob SET"
			" uJobStatus=%u,"
			" cRemoteMsg='%.100s',"
			" uModBy=1,"
			" uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJob=%u",uStatus,cMessage,uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("UpdateJob",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		mysql_close(&gMysqlExt);
		exit(2);
	}

}//void UpdateJob()


void ParseDIDJobData(char *cJobData,char *cDID,char *cHostname,char *cCustomerName)
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
	if((cp=strstr(cJobData,"cCustomerName=")))
	{
		if((cp2=strchr(cp+14,';')))
		{
			*cp2=0;
			sprintf(cCustomerName,"%.32s",cp+14);
			*cp2=';';
		}
	}
}//void ParseDIDJobData()