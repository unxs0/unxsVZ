/*
FILE
	jobqueue.c
	svn ID removed
PURPOSE
	Command line processing of jobs in the tJob queue.
AUTHOR/LEGAL
	(C) 2008-2011 Gary Wallis for Unxiservice, LLC.
	GPLv2 license applies. See LICENSE file included.
NOTES
	We still use KISS code, var naming conventions, and Allman (ANSI) style C 
	indentation to make our software readable and writable by any programmer. 
	At the same time this approach (although with some redundant code) has kept these
	programs lean and faster than anything available in any other language.
TODO
	Create more #define based "macros," to help the compiler optimize the
	many simple, fast but redundant code blocks.
	Get rid of any goto statements that do not add too many layers of nested
	logic that makes the code hard to maintain by non-authors.
	Use uNotValidSystemCallArg() before all system() calls where any args
	come from db and are not formatted (sprintf) as numbers.
FILE CONTENTS
	1-. Top redundant protos as the appear in file for a simple TOC.
	2-. More protos: Major external functions used.
	3-. File scoped vars.
	4-. Top level functions.
	5-. Rest of functions.
*/

#include "mysqlrad.h"
#include <openisp/template.h>
#include <sys/sysinfo.h>

//
//The following prototype declarations should provide a 
//	table of contents

//local protos, order=ret type, in file
void ProcessJobQueue(unsigned uDebug);
void ProcessJob(unsigned uJob,unsigned uDatacenter,unsigned uServer,char *cJobName,char *cJobData);
void tJobErrorUpdate(unsigned uJob, const char *cErrorMsg);
void tJobDoneUpdate(unsigned uJob);
void tJobWaitingUpdate(unsigned uJob);
void GetGroupProp(const unsigned uGroup,const char *cName,char *cValue);
void AppFunctions(FILE *fp,char *cFunction);
void LocalImportTemplate(unsigned uJob,unsigned uDatacenter,const char *cJobData);
void LocalImportConfig(unsigned uJob,unsigned uDatacenter,const char *cJobData);
void GetIPFromtIP(const unsigned uIPv4,char *cIP);
void GetDatacenterProp(const unsigned uDatacenter,const char *cName,char *cValue);
void logfileLine(const char *cFunction,const char *cLogline);
void LogError(char *cErrorMsg,unsigned uKey);
void RecurringJob(unsigned uJob,unsigned uDatacenter,unsigned uServer,const char *cJobData);
void SetJobStatus(unsigned uJob,unsigned uJobStatus);

unsigned uNotValidSystemCallArg(char *cSSHOptions);

//extern protos
unsigned TextConnectDb(void); //mysqlconnect.c
void GetConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uServer,
		unsigned uContainer,
		unsigned uHtml);

//file scoped vars.
static unsigned gfuServer=0;
static unsigned gfuDatacenter=0;
static unsigned guDebug=0;
static char cHostname[100]={""};//file scope
static FILE *gLfp=NULL;//log file


//Using the local server hostname get max 32 jobs for this server from the tJob queue.
//Then dispatch jobs via ProcessJob() this function in turn calls specific functions for
//each known cJobName.
void ProcessJobQueue(unsigned uDebug)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uDatacenter=0;
	unsigned uServer=0;
	unsigned uJob=0;
	struct sysinfo structSysinfo;

	if(uDebug) guDebug=1;

	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"Could not open logfile: %s\n",cLOGFILE);
		exit(300);
       	}

	if(gethostname(cHostname,99)!=0)
	{
		logfileLine("ProcessJobQueue","gethostname() failed");
		exit(1);
	}

	if(sysinfo(&structSysinfo))
	{
		logfileLine("ProcessJobQueue","sysinfo() failed");
		exit(1);
	}
#define LINUX_SYSINFO_LOADS_SCALE 65536
#define JOBQUEUE_MAXLOAD 10 //This is equivalent to uptime 10.00 last 5 min avg load
	if(structSysinfo.loads[1]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
	{
		sprintf(gcQuery,"structSysinfo.loads[1]=%lu larger than JOBQUEUE_MAXLOAD=%u",
				structSysinfo.loads[1]/LINUX_SYSINFO_LOADS_SCALE,JOBQUEUE_MAXLOAD);
		logfileLine("ProcessJobQueue",gcQuery);
		exit(1);
	}
	//debug only
	//sprintf(gcQuery,"%2.2f",(float)structSysinfo.loads[1]/(float)LINUX_SYSINFO_LOADS_SCALE);
	//logfileLine("structSysinfo.loads",gcQuery);
	//exit(0);

	//Uses login data from local.h
	if(TextConnectDb())
		exit(1);

	guLoginClient=1;//Root user

	//Get server and datacenter via hostname
	sprintf(gcQuery,"SELECT uServer,uDatacenter FROM tServer WHERE cLabel='%.99s'",cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessJobQueue",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uServer);
		sscanf(field[1],"%u",&uDatacenter);
	}
	mysql_free_result(res);

	//FQDN vs short name of 2nd NIC mess
	if(!uServer)
	{
		char *cp;

		if((cp=strchr(cHostname,'.')))
			*cp=0;
		sprintf(gcQuery,"SELECT uServer,uDatacenter FROM tServer WHERE cLabel='%.99s'",cHostname);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessJobQueue",mysql_error(&gMysql));
			mysql_close(&gMysql);
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uServer);
			sscanf(field[1],"%u",&uDatacenter);
		}
		mysql_free_result(res);
	}

	if(!uServer)
	{
		logfileLine("ProcessJobQueue","could not determine uServer: aborted");
		mysql_close(&gMysql);
		exit(1);
	}

	//Some file scoped globals we need to cleanout someday
	gfuServer=uServer;
	gfuDatacenter=uDatacenter;

	if(guDebug)
	{
		sprintf(gcQuery,"Start %s(uServer=%u,uDatacenter=%u)",cHostname,uServer,uDatacenter);
		logfileLine("ProcessJobQueue",gcQuery);
	}

	//
	//Main loop normal jobs
	//uWAITING==1
	//TODO can the LIMIT partition related jobs that need to run close together?
	//Testing allow only one to run at the same time.
	if(mkdir("/var/run/unxsvz.lock",S_IRWXU))
	{
		logfileLine("ProcessJobQueue","/var/run/unxsvz.lock");
		exit(127);
	}

	sprintf(gcQuery,"SELECT uJob,uContainer,cJobName,cJobData FROM tJob WHERE uJobStatus=1"
				" AND (uDatacenter=%u OR uDatacenter=0) AND (uServer=%u OR uServer=0)" //uDatacenter,uServer=0 all type jobs
				" AND uJobDate<=UNIX_TIMESTAMP(NOW()) ORDER BY uJob LIMIT 128",
						uDatacenter,uServer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessJobQueue",mysql_error(&gMysql));
		mysql_close(&gMysql);
		if(rmdir("/var/run/unxsvz.lock"))
			logfileLine("ProcessJobQueue","/var/run/unxsvz.lock rmdir error");
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(sysinfo(&structSysinfo))
		{
			logfileLine("ProcessJobQueue","sysinfo() failed");
			exit(1);
		}
		if(structSysinfo.loads[1]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
		{
			logfileLine("ProcessJobQueue-mainloop","structSysinfo.loads[1] too high");
			mysql_free_result(res);
			if(rmdir("/var/run/unxsvz.lock"))
				logfileLine("ProcessJobQueue-mainloop","/var/run/unxsvz.lock rmdir error");
			fclose(gLfp);
			mysql_close(&gMysql);
			exit(0);
		}
		sscanf(field[0],"%u",&uJob);
		//Job dispatcher based on cJobName
		//These log entries combined with the output of system calls will provide framing
		sprintf(gcQuery,"Start %s",field[2]);
		logfileLine("ProcessJobQueue",gcQuery);
		ProcessJob(uJob,uDatacenter,uServer,field[2],field[3]);
		logfileLine("ProcessJobQueue","End");
	}
	mysql_free_result(res);

	if(rmdir("/var/run/unxsvz.lock"))
		logfileLine("ProcessJobQueue","/var/run/unxsvz.lock rmdir error");
	if(guDebug) logfileLine("ProcessJobQueue","End");
	fclose(gLfp);
	mysql_close(&gMysql);
	exit(0);

}//void ProcessJobQueue()



void ProcessJob(unsigned uJob,unsigned uDatacenter,unsigned uServer,char *cJobName,char *cJobData)
{
	//Some jobs may take quite some time, we need to make sure we don't run again!
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=2,cRemoteMsg='Running',uModBy=1,"
				"uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJob=%u",uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		logfileLine("ProcessJob()",mysql_error(&gMysql));


	//if debug
	//printf("%3.3u uJob=%u uContainer=%u cJobName=%s; cJobData=%s;\n",
	//		uCount++,uJob,uContainer,cJobName,cJobData);

	//Is priority order needed in some cases?
	if(!strcmp(cJobName,"RecurringJob"))
	{
		RecurringJob(uJob,uDatacenter,uServer,cJobData);
	}
	else if(1)
	{
		logfileLine("ProcessJob()",cJobName);
		tJobErrorUpdate(uJob,cJobName);
	}

}//ProcessJob(...)


//Shared functions


void tJobErrorUpdate(unsigned uJob, const char *cErrorMsg)
{
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=14,cRemoteMsg='%.31s',uModBy=1,"
				"uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJob=%u",
									cErrorMsg,uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("tJobErrorUpdate",mysql_error(&gMysql));
		exit(2);
	}

	sprintf(gcQuery,"INSERT INTO tLog SET"
		" cLabel='unxsSPS.cgi ProcessJobQueue Error',"
		"uLogType=4,uLoginClient=1,"
		"cLogin='unxsSPS.cgi',cMessage=\"%s uJob=%u\","
		"cServer='%s',uOwner=1,uCreatedBy=1,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())",
					cErrorMsg,uJob,cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("tJobErrorUpdate",mysql_error(&gMysql));
		exit(2);
	}

}//void tJobErrorUpdate()


void tJobDoneUpdate(unsigned uJob)
{
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=3,uModBy=1,cRemoteMsg='tJobDoneUpdate() ok',"
				"uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJob=%u",uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("tJobDoneUpdate",mysql_error(&gMysql));
		exit(2);
	}

}//void tJobDoneUpdate(unsigned uJob, char *cErrorMsg)


void tJobWaitingUpdate(unsigned uJob)
{
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=1,uModBy=1,cRemoteMsg='tJobWaitingUpdate()',"
				"uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJob=%u",uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("tJobWaitingUpdate",mysql_error(&gMysql));
		exit(2);
	}

}//void tJobWaitingUpdate(unsigned uJob, char *cErrorMsg)


void GetGroupProp(const unsigned uGroup,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uGroup==0) return;

	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=11 AND cName='%s'",
				uGroup,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("GetGroupProp",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		if((cp=strchr(field[0],'\n')))
			*cp=0;
		sprintf(cValue,"%.255s",field[0]);
	}
	mysql_free_result(res);

}//void GetGroupProp(...)


//Required by libtemplate
void AppFunctions(FILE *fp,char *cFunction)
{
}//void AppFunctions(FILE *fp,char *cFunction)


//Make sure internal sabotage or db compromise does not allow exploit via system call injection
unsigned uNotValidSystemCallArg(char *cSSHOptions)
{
	int register i;

	for(i=0;cSSHOptions[i];i++)
	{
		if(
			cSSHOptions[i]==';' ||
			cSSHOptions[i]=='&' ||
			cSSHOptions[i]=='`' ||
			cSSHOptions[i]=='\'' ||
			cSSHOptions[i]=='"' ||
			cSSHOptions[i]=='|' ||
			cSSHOptions[i]=='*' ||
			cSSHOptions[i]=='?' ||
			cSSHOptions[i]=='~' ||
			cSSHOptions[i]=='<' ||
			cSSHOptions[i]=='>' ||
			cSSHOptions[i]=='^' ||
			cSSHOptions[i]=='(' ||
			cSSHOptions[i]==')' ||
			cSSHOptions[i]=='[' ||
			cSSHOptions[i]==']' ||
			cSSHOptions[i]=='{' ||
			cSSHOptions[i]=='}' ||
			cSSHOptions[i]=='$' ||
			cSSHOptions[i]=='\\'
						)
			{
				return(1);
			}
	}

	return(0);

}//unsigned uNotValidSystemCallArg(char *cSSHOptions)


void GetIPFromtIP(const unsigned uIPv4,char *cIP)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uIPv4==0)
		return;

	sprintf(gcQuery,"SELECT cLabel FROM tIP WHERE uIP=%u",uIPv4);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("GetIPFromtIP",mysql_error(&gMysql));
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cIP,"%.31s",field[0]);
	}
	mysql_free_result(res);

}//void GetIPFromtIP(const unsigned uIPv4,char *cIP)


void logfileLine(const char *cFunction,const char *cLogline)
{
	time_t luClock;
	char cTime[32];
	pid_t pidThis;
	const struct tm *tmTime;

	pidThis=getpid();

	time(&luClock);
	tmTime=localtime(&luClock);
	strftime(cTime,31,"%b %d %T",tmTime);

        fprintf(gLfp,"%s jobqueue.%s[%u]: %s\n",cTime,cFunction,pidThis,cLogline);
	fflush(gLfp);

}//void logfileLine(char *cLogline)


void LogError(char *cErrorMsg,unsigned uKey)
{
	sprintf(gcQuery,"INSERT INTO tLog SET"
		" cLabel='unxsSPS CLI Error',"
		"uLogType=4,uLoginClient=1,"
		"cLogin='unxsSPS.cgi',cMessage=\"%s uKey=%u\","
		"cServer='%s',uOwner=1,uCreatedBy=1,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())",
					cErrorMsg,uKey,cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("LogError",mysql_error(&gMysql));
		exit(2);
	}

}//void LogError(char *cErrorMsg)


void RecurringJob(unsigned uJob,unsigned uDatacenter,unsigned uServer,const char *cJobData)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	char *cp;
	unsigned uMin=-1,uHour=-1,uDayOfWeek=-1,uDayOfMonth=-1,uMonth=-1;
	char cRecurringJob[100]={""};
	char cCommand[100]={""};
	time_t luClock;
	struct tm structTm;
	struct stat statInfo;

	//error policy: only on job format or sql errors mark as error, otherwise try again later
	//	but provide tJob error/info message.

	//devel only
	//unsigned guDebug=1;

	//Parse job data
	if((cp=strstr(cJobData,"uMin=")))
		sscanf(cp+5,"%u",&uMin);
	if((cp=strstr(cJobData,"uHour=")))
		sscanf(cp+6,"%u",&uHour);
	if((cp=strstr(cJobData,"uDayOfWeek=")))
		sscanf(cp+11,"%u",&uDayOfWeek);
	if((cp=strstr(cJobData,"uDayOfMonth=")))
		sscanf(cp+12,"%u",&uDayOfMonth);
	if((cp=strstr(cJobData,"uMonth=")))
		sscanf(cp+7,"%u",&uMonth);
	if(uMin== -1 || uHour== -1 || uDayOfWeek== -1 || uDayOfMonth== -1 || uMonth== -1)
	{
		logfileLine("RecurringJob One or more data=-1 cJobData=",cJobData);
		tJobErrorUpdate(uJob,"recurring job data=-1");
		return;
	}
	if(!uMin && !uHour && !uDayOfWeek && !uDayOfMonth && !uMonth)
	{
		logfileLine("RecurringJob All recurring job data=0 cJobData=",cJobData);
		tJobErrorUpdate(uJob,"recurring job data=0");
		return;
	}

	if((cp=strstr(cJobData,"cRecurringJob=")))
		sprintf(cRecurringJob,"%.99s",cp+14);
	if((cp=strchr(cRecurringJob,';')))
		*cp=0;
	if(!cRecurringJob[0])	
	{
		logfileLine("RecurringJob cRecurringJob empty cJobData=",cJobData);
		tJobErrorUpdate(uJob,"cRecurringJob empty");
		return;
	}

	//Determine if we run the job
	if(guDebug)
	{
		sprintf(gcQuery,"Job: uMonth=%u uDayOfMonth=%u uDayOfWeek=%u uHour=%u uMin=%u",
				uMonth,uDayOfMonth,uDayOfWeek,uHour,uMin);
		logfileLine("RecurringJob info",gcQuery);
	}

	time(&luClock);
	localtime_r(&luClock,&structTm);

	//Note structTm.tm_mon+1. We adjust for normal 1-12
	if(guDebug)
	{
		sprintf(gcQuery,"Now: uMonth=%d uDayOfMonth=%d uDayOfWeek=%d uHour=%d uMin=%d",
			structTm.tm_mon+1,structTm.tm_mday,structTm.tm_wday+1,structTm.tm_hour,structTm.tm_min);
		logfileLine("RecurringJob info",gcQuery);
	}

	//If not any month and now month is less than job month do not run.
	if(uMonth && uMonth>structTm.tm_mon+1)
	{
		if(guDebug)
			logfileLine("RecurringJob info","uMonth not reached");
		goto Common_WaitingExit;
	}
	//If not any day of month and now day of month is less than job day of month do not run.
	if(uDayOfMonth && uDayOfMonth>structTm.tm_mday)
	{
		if(guDebug)
			logfileLine("RecurringJob info","uDayOfMonth not reached");
		goto Common_WaitingExit;
	}
	//If not any day of week and now day of week is less than job day of week do not run.
	if(uDayOfWeek && uDayOfWeek>structTm.tm_wday+1)
	{
		if(guDebug)
			logfileLine("RecurringJob info","uDayOfWeek not reached");
		goto Common_WaitingExit;
	}
	//If not any hour and now hour is less than job hour do not run.
	if(uHour && uHour>structTm.tm_hour)
	{
		if(guDebug)
			logfileLine("RecurringJob info","uHour not reached");
		goto Common_WaitingExit;
	}
	//If now min is less than job min do not run.
	if(uMin>structTm.tm_min)
	{
		if(guDebug)
			logfileLine("RecurringJob info","uMin not reached");
		goto Common_WaitingExit;
	}

	//Passed all constraints run job
	//Get command via cRecurringJob

        sprintf(gcQuery,"SELECT TRIM(cValue) FROM tProperty WHERE uKey=0 AND uType=%u AND cName='%.99s'",
										uPROP_RECJOB,cRecurringJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("RecurringJob",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"tProperty WHERE uKey=0 AND uType");
		return;
	}
        mysqlRes=mysql_store_result(&gMysql);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sprintf(cCommand,"%.99s",mysqlField[0]);
        mysql_free_result(mysqlRes);

	//Remove trailing junk
	if((cp=strchr(cCommand,'\n')) || (cp=strchr(cCommand,'\r')))
		*cp=0;
		
	if(guDebug)
	{
		sprintf(gcQuery,"Proposed job to run: %s",cCommand);
		logfileLine("RecurringJob info",gcQuery);
	}

	if(uNotValidSystemCallArg(cCommand))
	{
		tJobErrorUpdate(uJob,"cCommand sec alert!");
		logfileLine("RecurringJob","cCommand security alert");
		goto Common_WaitingExit;
	}
	//Only run if command is chmod 500 for extra security reasons.
	if(stat(cCommand,&statInfo))
	{
		tJobErrorUpdate(uJob,"stat(cCommand)");
		logfileLine("RecurringJob","stat failed for cCommand");
		goto Common_WaitingExit;
	}
	if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
	{
		tJobErrorUpdate(uJob,"cCommand is not chmod 500");
		logfileLine("RecurringJob","cCommand is not chmod 500");
		goto Common_WaitingExit;
	}
	//printf("good perms (%x)\n",statInfo.st_mode&(S_IWOTH|S_IWGRP|S_IWUSR|S_IXOTH|S_IROTH|S_IXGRP|S_IRGRP));
	//goto Common_WaitingExit;
	if(guDebug)
	{
		sprintf(gcQuery,"Attempting %s",cCommand);
		logfileLine("RecurringJob",gcQuery);
	}
	if(system(cCommand))
	{
		tJobErrorUpdate(uJob,"system(cCommand)");
		logfileLine("RecurringJob",cCommand);
		goto Common_WaitingExit;
	}
	sprintf(gcQuery,"Ran %s",cCommand);
	logfileLine("RecurringJob",gcQuery);
	//Update uJobDate based on cJobData.
	//TODO: Analyze what happens when jobs for some reason do not run for given periods.
	//Case 1: Job set to run every Sunday (day 7) at 3:15 AM. Server was down since last saturday, it is now
	//	 Friday and job has not run since Sunday more than a week ago. So 
	//	tJob.uJobDate<UNIX_TIMESTAMP(NOW)) and RecurringJob() will
	//	be run. Since uDayOfWeek is 5 job will not run. On Sunday it will run correctly.
	//Case 2: Job is set to run every day 1 of every month. Server was down from previous month up to
	//	day 2. RecurringJob() will be called since it is day 2 > day 1 job will run. But next time
	//	it will also run on day 2 instead of day 1 unless we adjust the DATE_ADD() by 1 month minus 1
	//	day (i.e. Current day - target day.) (This at first blush seems to able to be extended to
	//	all the cases below except the every hour case, which does not require it -it seems.)
	//SELECT FROM_UNIXTIME( UNIX_TIMESTAMP(NOW())  - 
	//	(EXTRACT(MINUTE FROM NOW())*60) - 
	//	(EXTRACT(SECOND FROM NOW())) +
	//		 3600 ) AS RoundUpNextHour;
	if(uMonth)
        	sprintf(gcQuery,"UPDATE tJob SET"
		" uJobDate=UNIX_TIMESTAMP(DATE_ADD(CURDATE(),INTERVAL 1 YEAR))+%u+%u-((DAYOFYEAR(CURDATE())-(%u*30))*86400)"
		" WHERE uJob=%u",uMin*60,uHour*3600,uMonth,uJob);
	else if(uDayOfMonth)
        	sprintf(gcQuery,"UPDATE tJob SET"
		" uJobDate=UNIX_TIMESTAMP(DATE_ADD(CURDATE(),INTERVAL 1 MONTH))+%u+%u-((DAY(CURDATE())-%u)*86400)"
		" WHERE uJob=%u",uMin*60,uHour*3600,uDayOfMonth,uJob);
	else if(uDayOfWeek)
        	sprintf(gcQuery,"UPDATE tJob SET"
		" uJobDate=UNIX_TIMESTAMP(DATE_ADD(CURDATE(),INTERVAL 1 WEEK))+%u+%u-((DAYOFWEEK(CURDATE())-%u)*86400)"
		" WHERE uJob=%u",uMin*60,uHour*3600,uDayOfWeek,uJob);
	else if(1)
        	sprintf(gcQuery,"UPDATE tJob SET"
		" uJobDate=UNIX_TIMESTAMP(NOW())-(EXTRACT(MINUTE FROM NOW())*60)-(EXTRACT(SECOND FROM NOW()))+3600+%u+%u"
		" WHERE uJob=%u",uMin*60,uHour*3600,uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("RecurringJob",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"DateAdjustSQL");
		return;
	}
	tJobDoneUpdate(uJob);

Common_WaitingExit:
	SetJobStatus(uJob,uWAITING);
	return;

}//void RecurringJob(uJob,uDatacenter,uServer,uContainer,cJobData)


void SetJobStatus(unsigned uJob,unsigned uJobStatus)
{
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=%u,uModBy=1,"
				"uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJob=%u",uJobStatus,uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("SetJobStatus",mysql_error(&gMysql));
		exit(2);
	}

}//void SetJobStatus(unsigned uJob,unsigned uJobStatus)

