/*
FILE
	mcs.c
	svn ID removed
PURPOSE
	Collection of diverse VZ operating parameters and other system vars.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2011.
	GPLv2 License applies. See LICENSE file.
NOTES
*/

#include "local.h"
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
void SendAlertEmail(char *cMsg);

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
		else if(!strncmp(cArgv[1],"TestEmail",9))
		{
			SendAlertEmail("Test message");
			goto CommonExit;
		}
	}

	printf("Usage: %s Process|DeleteMCSData|RoundRobinMCSData|TestEmail\n",gcProgram);

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

	sprintf(gcQuery,"SELECT uProperty,uKey FROM tProperty WHERE uType=3 AND cName='cOrg_MCS_%.32s'",cNamePreFix);
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
		sprintf(gcQuery,"DELETE FROM tProperty WHERE" 
					" cName=CONCAT('cOrg_MCS_',WEEKDAY(FROM_UNIXTIME(UNIX_TIMESTAMP(NOW())-86400)),'%.32s')"
					" AND uKey=%u AND uType=3",cNamePreFix,uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("RoundRobinMCSData",mysql_error(&gMysql),uContainer);
			mysql_close(&gMysql);
			exit(2);
		}
		sprintf(gcQuery,"UPDATE tProperty SET"
					" cName=CONCAT('cOrg_MCS_',WEEKDAY(FROM_UNIXTIME(UNIX_TIMESTAMP(NOW())-86400)),'%.32s')"
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

}//void RoundRobinMCSDataElement(char *cNamePreFix)


void RoundRobinMCSData(void)
{
	RoundRobinMCSDataElement("MOS");
	RoundRobinMCSDataElement("avgMOS");
	RoundRobinMCSDataElement("JitterUp");
	RoundRobinMCSDataElement("JitterDown");
	RoundRobinMCSDataElement("LossUp");
	RoundRobinMCSDataElement("LossDown");
	exit(0);

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
	char cNameArray[16][32]={"JitterUp","JitterDown","MOS","LossUp","LossDown",""};
	char cFuncArray[16][32]={"GREATEST","GREATEST","LEAST","GREATEST","GREATEST",""};

	//Uses login data from local.h
	TextConnectDb();
	TextConnectOpenMCSDb();
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT tIP.cLabel,tContainer.uContainer,tContainer.uOwner,tContainer.cHostname"
			" FROM tContainer,tGroupGlue,tGroup,tIP"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tIP.uIP=tContainer.uIPv4"
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

		//MAX Section
		sprintf(gcQuery,"SELECT MAX(dUpJitter),MAX(dDownJitter),MIN(dMOS),MAX(dUpLoss),MAX(dDownLoss)"
					" FROM tVoipTest WHERE cIP='%s' AND uStatus=0",field[0]);
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
			unsigned i;
			float dMOS=0.0;

			//debug only
			//printf("MAX %s\n",field[0]);

			//Using groups always will get at least one row
			if(field2[0]==NULL)
				goto ExitSection;

			//Send alert email when dMOS<3.8
			sscanf(field2[2],"%f",&dMOS);
			if(dMOS<3.8)
			{
				sprintf(gcQuery,"Warning MOS for %s/%s is %f\n\n"
						"Upstream jitter=%s, downstream jitter=%s\n"
						"Upstream loss=%s, downstream loss=%s\n",
							field[3],field[0],dMOS,
							field2[0],field2[1],
							field2[3],field2[4]);
				SendAlertEmail(gcQuery);
			}

			//Each array element
			for(i=0;i<5;i++)
			{
				//debug only
				//printf("%s %s\n",cNameArray[i],cFuncArray[i]);
	
				sprintf(gcQuery,"SELECT uProperty FROM tProperty"
						" WHERE cName='cOrg_MCS_%s' AND uType=3 AND uKey=%u",
							cNameArray[i],
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
					sprintf(gcQuery,"UPDATE tProperty SET cValue=%s(cValue,'%s')"
							",uModBy=1"
							",uModDate=UNIX_TIMESTAMP(NOW())"
							" WHERE uProperty=%s",
								cFuncArray[i],
								field2[i],field3[0]);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						logfileLine("Process",mysql_error(&gMysql),uContainer);
						mysql_close(&gMysql);
						mysql_close(&gMysqlExt);
						exit(2);
					}
					//debug only
					//printf("UPDATE %s %s\n",field3[0],field2[i]);
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_MCS_%s',cValue='%s'"
						",uType=3,uKey=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
							cNameArray[i],field2[i],uContainer,uOwner);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						logfileLine("Process",mysql_error(&gMysql),uContainer);
						mysql_close(&gMysql);
						mysql_close(&gMysqlExt);
						exit(2);
					}
					//debug only
					//printf("INSERT %s\n",field2[i]);
				}
				mysql_free_result(res3);
			}//for each array element

			//We wharehouse data here now, so we mark processed.
			//Note the we may clobber some data points if they come in at exactly the wrong second.
			sprintf(gcQuery,"UPDATE tVoipTest SET uStatus=1 WHERE cIP='%s' AND uStatus=0",field[0]);
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				logfileLine("Process",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
		}//if
ExitSection:
		mysql_free_result(res2);
		//End MAX section

		//daily STDDEV section
		sprintf(gcQuery,"SELECT FORMAT(STDDEV(dMOS),2),FORMAT(AVG(dMOS),2),MAX(dMOS) FROM tVoipTest WHERE"
				" uTime>DATE_FORMAT(NOW(),'%%Y%%m%%d000000') AND cIP='%s'",field[0]);
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
			//debug only
			//printf("STDDEV %s\n",field[0]);

			//Using groups always will get at least one row
			if(field2[0]==NULL)
				goto ExitSection1;

			//STDDEV
			sprintf(gcQuery,"SELECT uProperty FROM tProperty"
					" WHERE cName='cOrg_MCS_stddevMOS' AND uType=3 AND uKey=%u",
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
				sprintf(gcQuery,"UPDATE tProperty SET cValue='%s'"
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
				sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_MCS_stddevMOS',cValue='%s'"
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

			//AVG
			sprintf(gcQuery,"SELECT uProperty FROM tProperty"
					" WHERE cName='cOrg_MCS_avgMOS' AND uType=3 AND uKey=%u",
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
				sprintf(gcQuery,"UPDATE tProperty SET cValue='%s'"
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
				//debug only
				//printf("UPDATE %s %s\n",field3[0],field2[1]);
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_MCS_avgMOS',cValue='%s'"
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
				//debug only
				//printf("INSERT %s\n",field2[1]);
			}
			mysql_free_result(res3);

			//MAX
			sprintf(gcQuery,"SELECT uProperty FROM tProperty"
					" WHERE cName='cOrg_MCS_maxMOS' AND uType=3 AND uKey=%u",
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
				sprintf(gcQuery,"UPDATE tProperty SET cValue='%s'"
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
				//debug only
				//printf("UPDATE %s %s\n",field3[0],field2[2]);
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_MCS_maxMOS',cValue='%s'"
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
				//debug only
				//printf("INSERT %s\n",field2[2]);
			}
			mysql_free_result(res3);

		}//if
ExitSection1:
		mysql_free_result(res2);
		//End STDDEV section

	}//While each container

	mysql_free_result(res);

	mysql_close(&gMysql);
	mysql_close(&gMysqlExt);

	exit(0);

}//void Process(void)


void TextConnectOpenMCSDb(void)
{


        mysql_init(&gMysqlExt);
        if (!mysql_real_connect(&gMysqlExt,NULL,"mcs",MCSPWD,"mcs",0,NULL,0))
	{
		logfileLine("TextConnectOpenMCSDb","mysql_real_connect()",0);
		exit(3);
	}

}//TextConnectOpenMCSDb()


void SendAlertEmail(char *cMsg)
{
	FILE *pp;
	pid_t pidChild;

	pidChild=fork();
	if(pidChild!=0)
		return;

	pp=popen("/usr/lib/sendmail -t","w");
	if(pp==NULL)
	{
		logfileLine("SendAlertEmail","popen() /usr/lib/sendmail",0);
		return;
	}
			
	//should be defined in local.h
	fprintf(pp,"To: %s\n",cMAILTO);
	if(cBCC!=NULL)
	{
		char cBcc[512]={""};
		sprintf(cBcc,"%.511s",cBCC);
		if(cBcc[0])
			fprintf(pp,"Bcc: %s\n",cBcc);
	}
	fprintf(pp,"From: %s\n",cFROM);
	fprintf(pp,"Subject: %s\n",cSUBJECT);

	fprintf(pp,"%s\n",cMsg);

	fprintf(pp,".\n");

	pclose(pp);

	logfileLine("SendAlertEmail","email attempt ok",0);

}//void SendAlertEmail(char *cMsg)
