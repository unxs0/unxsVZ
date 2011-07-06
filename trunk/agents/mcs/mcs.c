/*
FILE
	mcs.c
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
void DeleteMCSData(void);
void RoundRobinMCSDataElement(char *cNamePreFix);
void RoundRobinMCSData(void);
void Process(void);
void TextConnectOpenMCSDb(void);

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

		fprintf(gLfp,"%s unxsMCS::%s[%u]: %s. uContainer=%u\n",cTime,cFunction,pidThis,cLogline,uContainer);
		fflush(gLfp);
	}
	else
	{
		fprintf(stderr,"%s: unxsMCS::%s. uContainer=%u\n",cFunction,cLogline,uContainer);
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
#define JOBQUEUE_MAXLOAD 10 //This is equivalent to uptime 20.00 last 1 min avg load
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
		if(!strncmp(cArgv[1],"Process",9))
		{
			Process();
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"DeleteMCSData",13))
		{
			DeleteMCSData();
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"RoundRobinMCSData",16))
		{
			RoundRobinMCSData();
			goto CommonExit;
		}
	}

	printf("Usage: %s Process|DeleteMCSData|RoundRobinMCSData\n",gcProgram);

CommonExit:
	fclose(gLfp);
	return(0);

}//main()


void DeleteMCSData(void)
{
	TextConnectDb();

	sprintf(gcQuery,"DELETE FROM tProperty WHERE cName LIKE 'cOrg_MCS_%%' AND uType=3");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("DeleteMCSData",mysql_error(&gMysql),0);
		mysql_close(&gMysql);
		exit(2);
	}

	mysql_close(&gMysql);
	exit(0);

}//void DeleteMCSData(void)


void RoundRobinMCSDataElement(char *cNamePreFix)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContainer=0;

	TextConnectDb();

	sprintf(gcQuery,"SELECT uProperty,uKey FROM tProperty WHERE uType=3 AND cName='%.32s'",cNamePreFix);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("RoundRobinMCSData",mysql_error(&gMysql),0);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uContainer);

		//Move to previous day
		sprintf(gcQuery,"DELETE FROM tProperty WHERE cName=concat('%.32s_',DAYNAME(FROM_UNIXTIME(UNIX_TIMESTAMP(NOW())-86400)))"
					" AND uKey=%u AND uType=3",cNamePreFix,uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("RoundRobinMCSData",mysql_error(&gMysql),uContainer);
			mysql_close(&gMysql);
			exit(2);
		}
		sprintf(gcQuery,"UPDATE tProperty SET cName=concat('%.32s_',DAYNAME(FROM_UNIXTIME(UNIX_TIMESTAMP(NOW())-86400)))"
					" WHERE uProperty=%s",cNamePreFix,field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("RoundRobinMCSData",mysql_error(&gMysql),uContainer);
			mysql_close(&gMysql);
			exit(2);
		}
	}
	mysql_close(&gMysql);
	exit(0);

}//void RoundRobinMCSDataElement(char *cNamePreFix)


void RoundRobinMCSData(void)
{
	RoundRobinMCSDataElement("cOrg_MCS_MOS");
	RoundRobinMCSDataElement("cOrg_MCS_JitterUp");
	RoundRobinMCSDataElement("cOrg_MCS_JitterDown");
	RoundRobinMCSDataElement("cOrg_MCS_LossUp");
	RoundRobinMCSDataElement("cOrg_MCS_LossDown");

}//void RoundRobinMCSData(void)


void Process(void)
{
        MYSQL_RES *res3;
        MYSQL_ROW field3;
        MYSQL_RES *res2;
        MYSQL_ROW field2;
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContainer=0;
	unsigned uOwner=0;

	//Uses login data from local.h
	TextConnectDb();
	TextConnectOpenMCSDb();
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT tContainer.cHostname,tContainer.uContainer,tContainer.uOwner FROM tContainer,tGroupGlue,tGroup"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel='Appliance PBXs'"
			" AND tContainer.uSource=0");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("Process",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	//debug only
	//if((field=mysql_fetch_row(res)))
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uContainer);
		sscanf(field[2],"%u",&uOwner);

		sprintf(gcQuery,"SELECT MAX(dUpJitter),MAX(dDownJitter),MIN(dMOS),MAX(dUpLoss),MAX(dDownLoss)"
					" FROM tVoipTest WHERE cSID LIKE '%%/%s' AND uStatus=0",field[0]);
		mysql_query(&gMysqlExt,gcQuery);
		if(mysql_errno(&gMysqlExt))
		{
			logfileLine("Process",mysql_error(&gMysqlExt),uContainer);
			mysql_close(&gMysql);
			mysql_close(&gMysqlExt);
			exit(2);
		}
		res2=mysql_store_result(&gMysqlExt);
		if((field2=mysql_fetch_row(res2)))
		{
			//Using groups always will get at least one row
			if(field2[0]==NULL)
				continue;

			//debug only
			//printf("%s\n",field[0]);

			//cOrg_MCS_JitterUp
			sprintf(gcQuery,"SELECT uProperty FROM tProperty"
						" WHERE cName='cOrg_MCS_JitterUp' AND uType=3 AND uKey=%u",
							uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("Process",mysql_error(&gMysql),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			res3=mysql_store_result(&gMysql);
			if((field3=mysql_fetch_row(res3)))
			{
				sprintf(gcQuery,"UPDATE tProperty SET cValue=GREATEST(cValue,'%s')"
						",uModBy=1"
						",uModDate=UNIX_TIMESTAMP(NOW())"
						" WHERE uProperty=%s",
							field2[0],field3[0]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("Process",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
				//debug only
				//printf("UPDATE %s %s\n",field3[0],field2[0]);
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_MCS_JitterUp',cValue='%s'"
					",uType=3,uKey=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
						field2[0],uContainer,uOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("Process",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
				//debug only
				//printf("INSERT %s\n",field2[0]);
			}
			mysql_free_result(res3);
			//cOrg_MCS_JitterUp end

			//cOrg_MCS_JitterDown
			sprintf(gcQuery,"SELECT uProperty FROM tProperty"
						" WHERE cName='cOrg_MCS_JitterDown' AND uType=3 AND uKey=%u",
							uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("Process",mysql_error(&gMysql),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			res3=mysql_store_result(&gMysql);
			if((field3=mysql_fetch_row(res3)))
			{
				sprintf(gcQuery,"UPDATE tProperty SET cValue=GREATEST(cValue,'%s')"
						",uModBy=1"
						",uModDate=UNIX_TIMESTAMP(NOW())"
						" WHERE uProperty=%s",
							field2[1],field3[0]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("Process",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_MCS_JitterDown',cValue='%s'"
					",uType=3,uKey=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
						field2[1],uContainer,uOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("Process",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			mysql_free_result(res3);
			//cOrg_MCS_JitterDown end

			//cOrg_MCS_MOS
			sprintf(gcQuery,"SELECT uProperty FROM tProperty"
						" WHERE cName='cOrg_MCS_MOS' AND uType=3 AND uKey=%u",
							uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("Process",mysql_error(&gMysql),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			res3=mysql_store_result(&gMysql);
			if((field3=mysql_fetch_row(res3)))
			{
				sprintf(gcQuery,"UPDATE tProperty SET cValue=LEAST(cValue,'%s')"
						",uModBy=1"
						",uModDate=UNIX_TIMESTAMP(NOW())"
						" WHERE uProperty=%s",
							field2[2],field3[0]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("Process",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_MCS_MOS',cValue='%s'"
					",uType=3,uKey=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
						field2[2],uContainer,uOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("Process",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			mysql_free_result(res3);
			//cOrg_MCS_MOS end

			//cOrg_MCS_LossUp
			sprintf(gcQuery,"SELECT uProperty FROM tProperty"
						" WHERE cName='cOrg_MCS_LossUp' AND uType=3 AND uKey=%u",
							uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("Process",mysql_error(&gMysql),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			res3=mysql_store_result(&gMysql);
			if((field3=mysql_fetch_row(res3)))
			{
				sprintf(gcQuery,"UPDATE tProperty SET cValue=GREATEST(cValue,'%s')"
						",uModBy=1"
						",uModDate=UNIX_TIMESTAMP(NOW())"
						" WHERE uProperty=%s",
							field2[3],field3[0]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("Process",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_MCS_LossUp',cValue='%s'"
					",uType=3,uKey=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
						field2[3],uContainer,uOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("Process",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			mysql_free_result(res3);
			//cOrg_MCS_LossUp end

			//cOrg_MCS_LossDown
			sprintf(gcQuery,"SELECT uProperty FROM tProperty"
						" WHERE cName='cOrg_MCS_LossDown' AND uType=3 AND uKey=%u",
							uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("Process",mysql_error(&gMysql),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
			res3=mysql_store_result(&gMysql);
			if((field3=mysql_fetch_row(res3)))
			{
				sprintf(gcQuery,"UPDATE tProperty SET cValue=GREATEST(cValue,'%s')"
						",uModBy=1"
						",uModDate=UNIX_TIMESTAMP(NOW())"
						" WHERE uProperty=%s",
							field2[4],field3[0]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("Process",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_MCS_LossDown',cValue='%s'"
					",uType=3,uKey=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
						field2[4],uContainer,uOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("Process",mysql_error(&gMysql),uContainer);
					mysql_close(&gMysql);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			mysql_free_result(res3);

			//We wharehouse data here now, so we mark processed.
			//Note the we may clobber some data points if they come in at exactly the wrong second.
			sprintf(gcQuery,"UPDATE tVoipTest SET uStatus=1 WHERE cSID LIKE '%%/%s'",field[0]);
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				logfileLine("Process",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
		}
		mysql_free_result(res2);
		
	}
	mysql_free_result(res);

	mysql_close(&gMysql);
	mysql_close(&gMysqlExt);

	exit(0);

}//void Process(void)


void TextConnectOpenMCSDb(void)
{

#include "local.h"

        mysql_init(&gMysqlExt);
        if (!mysql_real_connect(&gMysqlExt,NULL,"mcs",MCSPWD,"mcs",0,NULL,0))
	{
		logfileLine("TextConnectOpenMCSDb","mysql_real_connect()",0);
		exit(3);
	}

}//TextConnectOpenMCSDb()

