/*
FILE
	jobqueue.c
	$Id$
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
void ProcessJob(unsigned uJob,unsigned uDatacenter,unsigned uNode,
		unsigned uContainer,char *cJobName,char *cJobData);
void tJobErrorUpdate(unsigned uJob, const char *cErrorMsg);
void tJobDoneUpdate(unsigned uJob);
void tJobWaitingUpdate(unsigned uJob);
void NewContainer(unsigned uJob,unsigned uContainer);
void DestroyContainer(unsigned uJob,unsigned uContainer);
void ChangeIPContainer(unsigned uJob,unsigned uContainer,char *cJobData);
void SwapIPContainer(unsigned uJob,unsigned uContainer,char *cJobData);
void ChangeHostnameContainer(unsigned uJob,unsigned uContainer,char *cJobData);
void ExecuteCommands(unsigned uJob,unsigned uContainer,char *cJobData);
void StopContainer(unsigned uJob,unsigned uContainer);
void StartContainer(unsigned uJob,unsigned uContainer);
void MigrateContainer(unsigned uJob,unsigned uContainer,char *cJobData);
void DNSMoveContainer(unsigned uJob,unsigned uContainer,char *cJobData,unsigned uDatacenter,unsigned uNode);
void GetGroupProp(const unsigned uGroup,const char *cName,char *cValue);
void GetContainerProp(const unsigned uContainer,const char *cName,char *cValue);
void GetContainerPropUBC(const unsigned uContainer,const char *cName,char *cValue);
void UpdateContainerUBC(unsigned uJob,unsigned uContainer,const char *cJobData);
void SetContainerUBC(unsigned uJob,unsigned uContainer,const char *cJobData);
void TemplateContainer(unsigned uJob,unsigned uContainer,const char *cJobData);
void ActionScripts(unsigned uJob,unsigned uContainer);
void AllowAccess(unsigned uJob,const char *cJobData,unsigned uDatacenter,unsigned uNode);
void AllowAllAccess(unsigned uJob,const char *cJobData,unsigned uDatacenter,unsigned uNode);
void BlockAccess(unsigned uJob,const char *cJobData,unsigned uDatacenter,unsigned uNode);
void UndoBlockAccess(unsigned uJob,const char *cJobData,unsigned uDatacenter,unsigned uNode);
void RemoveDropFromIPTables(unsigned uJob,const char *cJobData,unsigned uDatacenter,unsigned uNode);
void RemoveAcceptFromIPTables(unsigned uJob,const char *cJobData,unsigned uDatacenter,unsigned uNode);
void DenyAccess(unsigned uJob,const char *cJobData);
void UpdateIPFWStatus(const char *cIPv4,unsigned uFWStatus);
void CloneContainer(unsigned uJob,unsigned uContainer,char *cJobData);
void CloneRemoteContainer(unsigned uJob,unsigned uContainer,char *cJobData,unsigned uNewVeid);
void AppFunctions(FILE *fp,char *cFunction);
void LocalImportTemplate(unsigned uJob,unsigned uDatacenter,const char *cJobData);
void LocalImportConfig(unsigned uJob,unsigned uDatacenter,const char *cJobData);
void FailoverTo(unsigned uJob,unsigned uContainer,const char *cJobData);
void FailoverFrom(unsigned uJob,unsigned uContainer,const char *cJobData);
void GetIPFromtIP(const unsigned uIPv4,char *cIP);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);
void GetDatacenterProp(const unsigned uDatacenter,const char *cName,char *cValue);
void logfileLine(const char *cFunction,const char *cLogline);
void LogError(char *cErrorMsg,unsigned uKey);
void RecurringJob(unsigned uJob,unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData);
void SetJobStatus(unsigned uJob,unsigned uJobStatus);

unsigned uNotValidSystemCallArg(char *cSSHOptions);
unsigned GetContainerStatus(const unsigned uContainer, unsigned *uStatus);
unsigned GetContainerMainIP(const unsigned uContainer,char *cIP);
unsigned GetContainerSource(const unsigned uContainer, unsigned *uSource);
unsigned SetContainerIP(const unsigned uContainer,char *cIP);
unsigned SetContainerSource(const unsigned uContainer,const unsigned uSource);
unsigned SetContainerHostname(const unsigned uContainer,
			const char *cHostname,const char *cLabel);
unsigned GetContainerNames(const unsigned uContainer,char *cHostname,char *cLabel);
unsigned GetContainerNodeStatus(const unsigned uContainer, unsigned *uStatus);
unsigned SetContainerPropertyUBC(const unsigned uContainer,const char *cPropertyName,const  char *cPropertyValue);
unsigned SetContainerProperty(const unsigned uContainer,const char *cPropertyName,const  char *cPropertyValue);
unsigned FailToJobDone(unsigned uJob);
//Clone maintenance clone UPDATE functions
unsigned ProcessCloneSyncJob(unsigned uNode,unsigned uContainer,unsigned uCloneContainer);
//This is experimental designed for remote sync, but is useless as is. On the other hand the remote
//clone function makes some sense. Based on why move the whole OS template, when we have it on the remote node.
unsigned ProcessOSDeltaSyncJob(unsigned uNode,unsigned uContainer,unsigned uCloneContainer);

int CreateActionScripts(unsigned uContainer, unsigned uOverwrite);
void NodeCommandJob(unsigned uJob,unsigned uContainer,char *cJobData,unsigned uNode,unsigned uDatacenter);
void RestartContainer(unsigned uJob,unsigned uContainer);
void GetGroupBasedPropertyValue(unsigned uContainer,char const *cName,char *cValue);
void ActivateNATContainer(unsigned uJob,unsigned uContainer,unsigned uNode);
void ActivateNATNode(unsigned uJob,unsigned uContainer,unsigned uNode);

//extern protos
unsigned TextConnectDb(void); //mysqlconnect.c
void SetContainerStatus(unsigned uContainer,unsigned uStatus);
void SetContainerNode(unsigned uContainer,unsigned uNode);
void SetContainerDatacenter(unsigned uContainer,unsigned uDatacenter);
unsigned uNodeCommandJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,
			unsigned uOwner, unsigned uLoginClient, unsigned uConfiguration, char *cArgs);
unsigned uCheckMaxContainers(unsigned uNode);
unsigned uCheckMaxCloneContainers(unsigned uNode);
unsigned uContainerStatus(unsigned uContainer);

//file scoped vars.
static unsigned gfuNode=0;
static unsigned gfuDatacenter=0;
static unsigned guDebug=0;
static char cHostname[100]={""};//file scope
#define LINUX_SYSINFO_LOADS_SCALE 65536
//Load and which we do nothing at all.
#define JOBQUEUE_MASTER_MAXLOAD 80
//Load at which we do not run clone jobs
#define JOBQUEUE_CLONE_MAXLOAD 5
//Low priority job load limit
#define JOBQUEUE_NORMAL_MAXLOAD 10
//New load control system allows for assigning specific load limits on
//	a job per job basis. E.g. firewall block job should run no matter what
//	since external DoS may raise load but we need to block the bad actor IP.
static unsigned guSystemLoad=0;//sysinfo() load


//Using the local server hostname get max 32 jobs for this node from the tJob queue.
//Then dispatch jobs via ProcessJob() this function in turn calls specific functions for
//each known cJobName.
void ProcessJobQueue(unsigned uDebug)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uDatacenter=0;
	unsigned uNode=0;
	unsigned uContainer=0;
	unsigned uCloneContainer=0;
	unsigned uJob=0;
	unsigned uError=0;
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
	guSystemLoad=structSysinfo.loads[1]/LINUX_SYSINFO_LOADS_SCALE;
	if(guSystemLoad>JOBQUEUE_MASTER_MAXLOAD)
	{
		sprintf(gcQuery,"Load %u larger than master load limit %u. Exiting now.",guSystemLoad,JOBQUEUE_MASTER_MAXLOAD);
		logfileLine("ProcessJobQueue",gcQuery);
		exit(1);
	}
	//debug only
	//sprintf(gcQuery,"Load %u, load limit %u.",guSystemLoad,JOBQUEUE_MASTER_MAXLOAD);
	//logfileLine("debug",gcQuery);

	char cRandom[8]={""};
	int fd=open("/dev/urandom",O_RDONLY);
	if(fd>0)
	{
		if(!read(fd,cRandom,sizeof(cRandom))!=sizeof(cRandom))
			(void)srand((unsigned int)cRandom[0]);
	}

	if(!cRandom[0])
	{
		logfileLine("ProcessJobQueue","/dev/urandom error");
		(void)srand((int)time((time_t *)NULL));
	}

	unsigned uDelay=0;
    	uDelay=rand() % 60;
	//sprintf(gcQuery,"random delay of %us added",uDelay);
	//logfileLine("ProcessJobQueue",gcQuery);
	sleep(uDelay);

	//Uses login data from local.h
	if(TextConnectDb())
		exit(1);

	guLoginClient=1;//Root user

	//Get node and datacenter via hostname
	sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
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
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
	}
	mysql_free_result(res);

	//FQDN vs short name of 2nd NIC mess
	if(!uNode)
	{
		char *cp;

		if((cp=strchr(cHostname,'.')))
			*cp=0;
		sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
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
			sscanf(field[0],"%u",&uNode);
			sscanf(field[1],"%u",&uDatacenter);
		}
		mysql_free_result(res);
	}

	if(!uNode)
	{
		logfileLine("ProcessJobQueue","could not determine uNode: aborted");
		mysql_close(&gMysql);
		exit(1);
	}

	//Some file scoped globals we need to cleanout someday
	gfuNode=uNode;
	gfuDatacenter=uDatacenter;

	//Special recurring jobs based on special containers
	//Main loop normal jobs
	//1-. Maintain clone containers
	//We need to rsync from running container to clone container
	//where the source container is running on this node
	//and the target node is a remote node.
	//1 uACTIVE
	//31 uSTOPPED
	//logfileLine("ProcessCloneSyncJob","Start");
	//Select clone containers of active or stopped source containers
	//running on this node
	sprintf(gcQuery,"SELECT t1.uContainer,t2.uContainer,t1.uStatus FROM tContainer AS t1,tContainer AS t2"
			" WHERE (t2.uStatus=1 OR t2.uStatus=31)"
			" AND t1.uSource=t2.uContainer AND t2.uNode=%u",uNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("CloneSync",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		unsigned uCloneStatus=0;

		if(sysinfo(&structSysinfo))
		{
			logfileLine("CloneSync","sysinfo() failed");
			mysql_free_result(res);
			fclose(gLfp);
			mysql_close(&gMysql);
			exit(0);
		}
		guSystemLoad=structSysinfo.loads[1]/LINUX_SYSINFO_LOADS_SCALE;
		if(guSystemLoad>JOBQUEUE_CLONE_MAXLOAD)
		{
			sprintf(gcQuery,"Load %u larger than clone load limit %u. Exiting now.",guSystemLoad,JOBQUEUE_CLONE_MAXLOAD);
			logfileLine("CloneSync",gcQuery);
			mysql_free_result(res);
			fclose(gLfp);
			mysql_close(&gMysql);
			exit(0);
		}
		sscanf(field[0],"%u",&uCloneContainer);
		sscanf(field[1],"%u",&uContainer);
		sscanf(field[2],"%u",&uCloneStatus);
		if(guDebug)
		{
			sprintf(gcQuery,"uCloneStatus=%u",uCloneStatus);
			logfileLine("CloneSync",gcQuery);
		}

		switch(uCloneStatus)
		{
			//We only move the delta files to the
			//remote node for backup (cold spare) and possible remote failover BC/DR.
			//Provided sample script osdeltasync.sh handles the details.
			case uINITSETUP:
				if((uError=ProcessOSDeltaSyncJob(uNode,uContainer,uCloneContainer)))
				{
					logfileLine("ProcessOSDeltaCloneSyncJob uError",field[1]);
					//error return(6) when clone script fails.
					if(uError!=6)
						LogError("ProcessOSDeltaCloneSyncJob()",uError);
				}
			break;

			//Here provided sample script clonesync.sh does an rsync
			//and keeps the clone container as warm spare.
			case uSTOPPED:
			case uACTIVE:
				if((uError=ProcessCloneSyncJob(uNode,uContainer,uCloneContainer)))
				{
					logfileLine("ProcessCloneSyncJob uError",field[1]);
					//error return(6) when clone script fails.
					if(uError!=6)
						LogError("ProcessCloneSyncJob()",uError);
				}
		}
	}
	mysql_free_result(res);

	if(guDebug)
	{
		sprintf(gcQuery,"Start %s(uNode=%u,uDatacenter=%u)",cHostname,uNode,uDatacenter);
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
				" AND (uDatacenter=%u OR uDatacenter=0) AND (uNode=%u OR uNode=0)" //uDatacenter,uNode=0 all type jobs
				" AND uJobDate<=UNIX_TIMESTAMP(NOW()) ORDER BY uJob LIMIT 128",
						uDatacenter,uNode);
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
		guSystemLoad=structSysinfo.loads[1]/LINUX_SYSINFO_LOADS_SCALE;
		if(guSystemLoad>JOBQUEUE_MASTER_MAXLOAD)
		{
			sprintf(gcQuery,"Load %u larger than master load limit %u. Exiting now.",guSystemLoad,JOBQUEUE_MASTER_MAXLOAD);
			logfileLine("ProcessJobQueue",gcQuery);
			mysql_free_result(res);
			if(rmdir("/var/run/unxsvz.lock"))
				logfileLine("ProcessJobQueue-mainloop","/var/run/unxsvz.lock rmdir error");
			fclose(gLfp);
			mysql_close(&gMysql);
			exit(0);
		}
		sscanf(field[0],"%u",&uJob);
		sscanf(field[1],"%u",&uContainer);
		//Job dispatcher based on cJobName
		//These log entries combined with the output of system calls will provide framing
		sprintf(gcQuery,"Start %s",field[2]);
		logfileLine("ProcessJobQueue",gcQuery);
		ProcessJob(uJob,uDatacenter,uNode,uContainer,field[2],field[3]);
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



void ProcessJob(unsigned uJob,unsigned uDatacenter,unsigned uNode,
		unsigned uContainer,char *cJobName,char *cJobData)
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


	//Only run special high priority jobs if load is high
	if(guSystemLoad>JOBQUEUE_NORMAL_MAXLOAD)
	{
		if(!strcmp(cJobName,"AllowAllAccess") && uNode)
		{
			AllowAllAccess(uJob,cJobData,uDatacenter,uNode);
		}
		else if(!strcmp(cJobName,"AllowAccess") && uNode)
		{
			AllowAccess(uJob,cJobData,uDatacenter,uNode);
		}
		else if(!strcmp(cJobName,"UndoBlockAccess") && uNode)
		{
			UndoBlockAccess(uJob,cJobData,uDatacenter,uNode);
		}
		else if(!strcmp(cJobName,"RemoveDropFromIPTables") && uNode)
		{
			RemoveDropFromIPTables(uJob,cJobData,uDatacenter,uNode);
		}
		else if(!strcmp(cJobName,"RemoveAcceptFromIPTables") && uNode)
		{
			RemoveAcceptFromIPTables(uJob,cJobData,uDatacenter,uNode);
		}
		else if(!strcmp(cJobName,"BlockAccess") && uNode)
		{
			BlockAccess(uJob,cJobData,uDatacenter,uNode);
		}
		else if(!strcmp(cJobName,"DenyAccess") && uNode)
		{
			DenyAccess(uJob,cJobData);
		}
		else if(1)
		{
			logfileLine("ProcessJob() highload",cJobName);
			tJobErrorUpdate(uJob,cJobName);
		}
	}
	else
	{
		//normal load jobs
		if(!strcmp(cJobName,"FailoverTo"))
		{
			FailoverTo(uJob,uContainer,cJobData);
		}
		else if(!strcmp(cJobName,"FailoverFrom"))
		{
			FailoverFrom(uJob,uContainer,cJobData);
		}
		else if(!strcmp(cJobName,"DNSMoveContainer"))
		{
			DNSMoveContainer(uJob,uContainer,cJobData,uDatacenter,uNode);
		}
		else if(!strcmp(cJobName,"MigrateContainer"))
		{
			MigrateContainer(uJob,uContainer,cJobData);
		}
		else if(!strcmp(cJobName,"CloneContainer"))
		{
			CloneContainer(uJob,uContainer,cJobData);
		}
		else if(!strcmp(cJobName,"NewContainer"))
		{
			NewContainer(uJob,uContainer);
		}
		else if(!strcmp(cJobName,"ActionScripts"))
		{
			//OpenVZ action scripts
			ActionScripts(uJob,uContainer);
		}
		else if(!strcmp(cJobName,"StartContainer"))
		{
			StartContainer(uJob,uContainer);
		}
		else if(!strcmp(cJobName,"ActivateNATContainer"))
		{
			ActivateNATContainer(uJob,uContainer,uNode);
		}
		else if(!strcmp(cJobName,"ActivateNATNode"))
		{
			ActivateNATNode(uJob,uContainer,uNode);
		}
		else if(!strcmp(cJobName,"ChangeHostnameContainer"))
		{
			ChangeHostnameContainer(uJob,uContainer,cJobData);
		}
		else if(!strcmp(cJobName,"ExecuteCommands"))
		{
			ExecuteCommands(uJob,uContainer,cJobData);
		}
		else if(!strcmp(cJobName,"ChangeIPContainer"))
		{
			ChangeIPContainer(uJob,uContainer,cJobData);
		}
		else if(!strcmp(cJobName,"SwapIPContainer"))
		{
			SwapIPContainer(uJob,uContainer,cJobData);
		}
		else if(!strcmp(cJobName,"StopContainer"))
		{
			StopContainer(uJob,uContainer);
		}
		else if(!strcmp(cJobName,"RestartContainer"))
		{
			RestartContainer(uJob,uContainer);
		}
		else if(!strcmp(cJobName,"DestroyContainer"))
		{
			DestroyContainer(uJob,uContainer);
		}
		else if(!strcmp(cJobName,"UpdateContainerUBCJob"))
		{
			UpdateContainerUBC(uJob,uContainer,cJobData);
		}
		else if(!strcmp(cJobName,"SetUBCJob"))
		{
			SetContainerUBC(uJob,uContainer,cJobData);
		}
		else if(!strcmp(cJobName,"TemplateContainer"))
		{
			TemplateContainer(uJob,uContainer,cJobData);
		}
		else if(!strcmp(cJobName,"LocalImportTemplateJob"))
		{
			LocalImportTemplate(uJob,uDatacenter,cJobData);
		}
		else if(!strcmp(cJobName,"LocalImportConfigJob"))
		{
			LocalImportConfig(uJob,uDatacenter,cJobData);
		}
		else if(!strcmp(cJobName,"RecurringJob"))
		{
			RecurringJob(uJob,uDatacenter,uNode,uContainer,cJobData);
		}
		else if(!strcmp(cJobName,"NodeCommandJob"))
		{
			NodeCommandJob(uJob,uContainer,cJobData,uNode,uDatacenter);
		}
		//also in high load section
		else if(!strcmp(cJobName,"AllowAllAccess") && uNode)
		{
			AllowAllAccess(uJob,cJobData,uDatacenter,uNode);
		}
		else if(!strcmp(cJobName,"AllowAccess") && uNode)
		{
			AllowAccess(uJob,cJobData,uDatacenter,uNode);
		}
		else if(!strcmp(cJobName,"UndoBlockAccess") && uNode)
		{
			UndoBlockAccess(uJob,cJobData,uDatacenter,uNode);
		}
		else if(!strcmp(cJobName,"RemoveDropFromIPTables") && uNode)
		{
			RemoveDropFromIPTables(uJob,cJobData,uDatacenter,uNode);
		}
		else if(!strcmp(cJobName,"RemoveAcceptFromIPTables") && uNode)
		{
			RemoveAcceptFromIPTables(uJob,cJobData,uDatacenter,uNode);
		}
		else if(!strcmp(cJobName,"BlockAccess") && uNode)
		{
			BlockAccess(uJob,cJobData,uDatacenter,uNode);
		}
		else if(!strcmp(cJobName,"DenyAccess") && uNode)
		{
			DenyAccess(uJob,cJobData);
		}
		//high load section
		else if(1)
		{
			logfileLine("ProcessJob()",cJobName);
			tJobErrorUpdate(uJob,cJobName);
		}
	}//normal load jobs

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
		" cLabel='unxsVZ.cgi ProcessJobQueue Error',"
		"uLogType=4,uLoginClient=1,"
		"cLogin='unxsVZ.cgi',cMessage=\"%s uJob=%u\","
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


//Specific job handlers


void NewContainer(unsigned uJob,unsigned uContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uVeth=0;
	unsigned uDeployStopped=0;
	char cDeployOptions[256]={""};

	//Must wait for clone or template operations to finish.
	if(access("/var/run/vzdump.lock",R_OK)==0)
	{
		logfileLine("NewContainer","/var/run/vzdump.lock exists");
		tJobWaitingUpdate(uJob);
		return;
	}

	sprintf(gcQuery,"SELECT tContainer.cLabel,tContainer.cHostname,tIP.cLabel"
			",tOSTemplate.cLabel,tNameserver.cLabel,tSearchdomain.cLabel,tConfig.cLabel,tContainer.uVeth"
			" FROM tContainer,tOSTemplate,tNameserver,tSearchdomain,tConfig,tIP WHERE uContainer=%u"
			" AND tContainer.uOSTemplate=tOSTemplate.uOSTemplate"
			" AND tContainer.uNameserver=tNameserver.uNameserver"
			" AND tContainer.uConfig=tConfig.uConfig"
			" AND tContainer.uIPv4=tIP.uIP"
			" AND tContainer.uSearchdomain=tSearchdomain.uSearchdomain",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("NewContainer",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{


		sscanf(field[7],"%u",&uVeth);

		//0-. Create vz conf action script files if applicable. 1 is for overwriting existing files
		//OpenVZ action scripts
		if(CreateActionScripts(uContainer,1))
		{
			logfileLine("NewContainer","CreateActionScripts(x,1) failed");
			tJobErrorUpdate(uJob,"CreateActionScripts(x,1) failed");
			goto CommonExit;
		}

		//vzctl [flags] create veid --ostemplate name] [--config name] [--private path] 
		//[--root path] [--ipadd addr] [--hostname name]
		//1-.
		
		if(	uNotValidSystemCallArg(field[0]) ||
			uNotValidSystemCallArg(field[1]) ||
			uNotValidSystemCallArg(field[2]) ||
			uNotValidSystemCallArg(field[3]) ||
			uNotValidSystemCallArg(field[4]) ||
			uNotValidSystemCallArg(field[5]) ||
			uNotValidSystemCallArg(field[6])	)
		{
			logfileLine("NewContainer","security alert");
			tJobErrorUpdate(uJob,"failed sec alert!");
			goto CommonExit;
		}


		//rename out of the way old conf file
		sprintf(gcQuery,"mv /etc/vz/conf/%1$u.conf /etc/vz/conf/%1$u.conf.NewContainer > /dev/null 2>&1",uContainer);
		system(gcQuery);

		if(uVeth)
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose create %u --ostemplate %s --hostname %s"
				" --name %s --config %s",
				uContainer,field[3],field[1],field[0],field[6]);
		else
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose create %u --ostemplate %s --hostname %s"
				" --ipadd %s --name %s --config %s",
				uContainer,field[3],field[1],field[2],field[0],field[6]);
		if(system(gcQuery))
		{
			logfileLine("NewContainer",gcQuery);
			tJobErrorUpdate(uJob,"vzctl create failed");
		}

		//2-.
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --nameserver \"%.99s\" --searchdomain \"%.32s\" --save",
				uContainer,field[4],field[5]);
		if(system(gcQuery))
		{
			logfileLine("NewContainer",gcQuery);
			tJobErrorUpdate(uJob,"vzctl set failed");
			//Roll back step 1-.
			sprintf(gcQuery,"/usr/sbin/vzctl destroy %u",uContainer);
			if(system(gcQuery))
			{
				logfileLine("NewContainer",gcQuery);
				tJobErrorUpdate(uJob,"rb0: vzctl destroy failed");
			}
			goto CommonExit;
		}

		//3-.
		//TODO hardcoded eth0 problem. Solution is easy get from node properties.
		//problem remains if datacenter nodes have diff eth device!
		GetContainerProp(uContainer,"cDeployOptions",cDeployOptions);
		if(strstr(cDeployOptions,"uDeployStopped=1;"))
			uDeployStopped=1;
		//always deploy stopped clone containers
		if(strstr(field[0],"-clone"))
			uDeployStopped=1;
		if(uVeth)
		{
			char cIPv4[32]={""};

			sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
			if(system(gcQuery))
			{
				logfileLine("NewContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl start1 failed");
				//Roll back step 1-.
				sprintf(gcQuery,"/usr/sbin/vzctl destroy %u",uContainer);
				if(system(gcQuery))
				{
					logfileLine("NewContainer",gcQuery);
					tJobErrorUpdate(uJob,"rb1: vzctl destroy failed");
				}
				goto CommonExit;
			}

			sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --netif_add eth0,,,,vmbr0 --save",uContainer);
			if(system(gcQuery))
			{
				logfileLine("NewContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl --netif_add failed");
				//Roll back step 2-.
				sprintf(gcQuery,"/usr/sbin/vzctl stop %u",uContainer);
				if(system(gcQuery))
				{
					logfileLine("NewContainer",gcQuery);
					tJobErrorUpdate(uJob,"rb2: vzctl stop failed");
				}
				//Roll back step 1-.
				sprintf(gcQuery,"/usr/sbin/vzctl destroy %u",uContainer);
				if(system(gcQuery))
				{
					logfileLine("NewContainer",gcQuery);
					tJobErrorUpdate(uJob,"rb2: vzctl destroy failed");
				}
				goto CommonExit;
			}

			if(GetContainerMainIP(uContainer,cIPv4))
				logfileLine("NewContainer","Empty cIPv4");

			sprintf(gcQuery,"/usr/sbin/vzctl exec %u \"/sbin/ifconfig eth0 0\"",uContainer);
			if(system(gcQuery))
				logfileLine("NewContainer",gcQuery);

			sprintf(gcQuery,"/usr/sbin/vzctl exec %u \"/sbin/ip addr add %s dev eth0\"",
						uContainer,cIPv4);
			if(system(gcQuery))
				logfileLine("NewContainer",gcQuery);

			sprintf(gcQuery,"/usr/sbin/vzctl exec %u \"/sbin/ip route add default dev eth0\"",uContainer);
			if(system(gcQuery))
				logfileLine("NewContainer",gcQuery);

			if(uDeployStopped)
			{
				sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u",uContainer);
				if(system(gcQuery))
					logfileLine("NewContainer",gcQuery);
				sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --onboot=no --save",uContainer);
				if(system(gcQuery))
					logfileLine("NewContainer",gcQuery);
			}
		}
		else
		{
			if(!uDeployStopped)
			{
				sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
				if(system(gcQuery))
				{
					logfileLine("NewContainer",gcQuery);
					tJobErrorUpdate(uJob,"vzctl start failed");
					//Roll back step 1-.
					sprintf(gcQuery,"/usr/sbin/vzctl destroy %u",uContainer);
					if(system(gcQuery))
					{
						logfileLine("NewContainer",gcQuery);
						tJobErrorUpdate(uJob,"rb3: vzctl destroy failed");
					}
					goto CommonExit;
				}
			}
			else
			{
				sprintf(gcQuery,"/usr/sbin/vzctl set %u --onboot no --save",uContainer);
				//warn via logfile only
				if(system(gcQuery))
					logfileLine("NewContainer",gcQuery);
			}
		}
	}
	else
	{
		sprintf(gcQuery,"Select for %u failed",uContainer);
		logfileLine("NewContainer",gcQuery);
		tJobErrorUpdate(uJob,"Select failed");
		goto CommonExit;
	}

	//4-. Optional container password set
	//This option requires MySQL SSL replication and
	//much more security measures in place to avoid a db penetration leading
	//to multiple container breaches.
	char cPasswd[256]={""};
	GetContainerProp(uContainer,"cPasswd",cPasswd);
	if(cPasswd[0] && !uNotValidSystemCallArg(cPasswd) )
	{
		//sprintf(gcQuery,"/usr/sbin/vzctl --userpasswd \"root:%s\" %u",cPasswd,uContainer);
		//This works on older vzctl also
		sprintf(gcQuery,"/usr/sbin/vzctl set %u --userpasswd \"root:%s\"",uContainer,cPasswd);
		if(system(gcQuery))
			logfileLine("NewContainer","Container passwd not changed!");
	}

	//5-. Optional container CentOS linux timezone set
	//Example (cOrg_TimeZone) cTimezone "Europe/Zurich"
	//For /usr/share/zoneinfo/Europe/Zurich
	char cTimezone[256]={""};
	GetContainerProp(uContainer,"cOrg_TimeZone",cTimezone);
	if(cTimezone[0] && !uNotValidSystemCallArg(cTimezone) )
	{
		sprintf(gcQuery,"rm -f /vz/private/%u/etc/localtime",uContainer);
		if(system(gcQuery))
		{
			logfileLine("NewContainer",gcQuery);
		}
		else
		{
			sprintf(gcQuery,"ln -s /usr/share/zoneinfo/%s /vz/private/%u/etc/localtime",
							cTimezone,uContainer);
			if(system(gcQuery))
				logfileLine("NewContainer",gcQuery);
			else
				logfileLine("NewContainer","Container timezone changed");
		}
	}

	//6-.
	//Optional group based script may exist to be executed.
	//
	//Primary group is oldest tGroupGlue entry.
	//UBC safe
	sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
			" AND tProperty.uKey=tGroupGlue.uGroup"
			" AND tGroupGlue.uContainer=%u"
			" AND tProperty.cName='cJob_OnNewScript' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",uPROP_GROUP,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("NewContainer",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char cOnScriptCall[386];
		struct stat statInfo;
		char cCommand[256];
		char *cp;

		sprintf(cCommand,"%.255s",field[0]);

		//Remove trailing junk
		if((cp=strchr(cCommand,'\n')) || (cp=strchr(cCommand,'\r'))) *cp=0;

		if(uNotValidSystemCallArg(cCommand))
		{
			logfileLine("NewContainer","cJob_OnNewScript security alert");
			goto CommonExit2;
		}

		//Only run if command is chmod 500 and owned by root for extra security reasons.
		if(stat(cCommand,&statInfo))
		{
			logfileLine("NewContainer","stat failed for cJob_OnNewScript");
			logfileLine("NewContainer",cCommand);
			goto CommonExit2;
		}
		if(statInfo.st_uid!=0)
		{
			logfileLine("NewContainer","cJob_OnNewScript is not owned by root");
			goto CommonExit2;
		}
		if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
		{
			logfileLine("NewContainer","cJob_OnNewScript is not chmod 500");
			goto CommonExit2;
		}

		char cHostname[100]={""};
		sprintf(gcQuery,"SELECT tContainer.cHostname"
				" FROM tContainer WHERE uContainer=%u",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("",mysql_error(&gMysql));
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(cHostname,"%.99s",field[0]);

			//Please note that any script here must not use uStatus=1 as a condition.
			sprintf(cOnScriptCall,"%.255s %.64s %u",cCommand,cHostname,uContainer);
			if(system(cOnScriptCall))
			{
				logfileLine("NewContainer",cOnScriptCall);
			}
		}
		//end 6-.
	}

//In this case the goto MIGHT be justified
CommonExit2:

	//Everything went ok;
	if(uDeployStopped)
		SetContainerStatus(uContainer,31);//Stopped
	else
		SetContainerStatus(uContainer,1);//Active
	tJobDoneUpdate(uJob);

//In this case the goto MIGHT be justified
CommonExit:
	mysql_free_result(res);

}//void NewContainer(...)


void DestroyContainer(unsigned uJob,unsigned uContainer)
{
	unsigned uStopFailed=0;
	struct stat statInfo;

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u",uContainer);
	if(system(gcQuery))
	{
		logfileLine("DestroyContainer",gcQuery);
		tJobErrorUpdate(uJob,"vzctl stop failed");
		uStopFailed=1;
	}

	//2-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose destroy %u",uContainer);
	if(system(gcQuery))
	{
		logfileLine("DestroyContainer",gcQuery);
		tJobErrorUpdate(uJob,"vzctl destroy failed");
		char cFileSpec[100]="";
		sprintf(cFileSpec,"/vz/private/%u",uContainer);
		if(!uStopFailed || !stat(cFileSpec,&statInfo))
			return;
	}

	//Optional group based script may exist to be executed.
	//
	//Primary group is oldest tGroupGlue entry.
        MYSQL_RES *res;
        MYSQL_ROW field;
	//UBC safe
	sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
			" AND tProperty.uKey=tGroupGlue.uGroup"
			" AND tGroupGlue.uContainer=%u"
			" AND tProperty.cName='cJob_OnDestroyScript' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",uPROP_GROUP,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("DestroyContainer",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char cOnScriptCall[512];
		char cCommand[256];
		char *cp;

		sprintf(cCommand,"%.255s",field[0]);

		//Remove trailing junk
		if((cp=strchr(cCommand,'\n')) || (cp=strchr(cCommand,'\r'))) *cp=0;

		if(uNotValidSystemCallArg(cCommand))
		{
			logfileLine("DestroyContainer","cJob_OnDestroyScript security alert");
			goto CommonExit2;
		}

		//Only run if command is chmod 500 and owned by root for extra security reasons.
		if(stat(cCommand,&statInfo))
		{
			logfileLine("DestroyContainer","stat failed for cJob_OnDestroyScript");
			logfileLine("DestroyContainer",cCommand);
			goto CommonExit2;
		}
		if(statInfo.st_uid!=0)
		{
			logfileLine("DestroyContainer","cJob_OnDestroyScript is not owned by root");
			goto CommonExit2;
		}
		if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
		{
			logfileLine("DestroyContainer","cJob_OnDestroyScript is not chmod 500");
			goto CommonExit2;
		}

		char cHostname[100]={""};
		sprintf(gcQuery,"SELECT tContainer.cHostname"
				" FROM tContainer WHERE uContainer=%u",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("",mysql_error(&gMysql));
			goto CommonExit3;
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(cHostname,"%.99s",field[0]);

			sprintf(cOnScriptCall,"%.255s %.64s %u",cCommand,cHostname,uContainer);
			if(system(cOnScriptCall))
			{
				logfileLine("DestroyContainer",cOnScriptCall);
			}
		}
	}
CommonExit2:
	mysql_free_result(res);
CommonExit3:

	//Everything ok
	SetContainerStatus(uContainer,uINITSETUP);//Initial
	tJobDoneUpdate(uJob);


}//void DestroyContainer(...)


void ChangeIPContainer(unsigned uJob,unsigned uContainer,char *cJobData)
{

	char cIPOld[256]={""};
	char cIPNew[31]={""};
	char *cp;
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uVeth=0;
	unsigned uNoRelease=0;

	//Check 1-. Check to make sure container is on this node, if not 
	//	give job back to queue
	sprintf(gcQuery,"/usr/sbin/vzlist %u > /dev/null 2>&1",uContainer);
	if(system(gcQuery))
	{
		logfileLine("ChangeIPContainer","Job returned to queue no such container");
		tJobWaitingUpdate(uJob);
		return;
	}

	//Check 2-. Wait till any other jobs currently in the job queue for this
	//	container are done.
	sprintf(gcQuery,"SELECT uJob FROM tJob"
			" WHERE uContainer=%u AND (uJobStatus=%u OR uJobStatus=%u) AND uJob!=%u"
			" AND cJobName!='ChangeHostnameContainer'"
			" AND cJobName!='ActivateNATNode'"
			" AND cJobName!='ActivateNATContainer'",
					uContainer,uWAITING,uRUNNING,uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ChangeIPContainer",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		logfileLine("ChangeIPContainer","Job returned to queue other jobs pending");
		tJobWaitingUpdate(uJob);
		mysql_free_result(res);
		return;
	}
	mysql_free_result(res);


	//0-. Get required data
	sprintf(gcQuery,"SELECT tIP.cLabel,tContainer.uVeth"
			" FROM tContainer,tIP WHERE uContainer=%u"
			" AND tContainer.uIPv4=tIP.uIP",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ChangeIPContainer",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uVeth);

		if(strstr(cJobData,"uNoRelease"))
			uNoRelease=1;

		sprintf(cIPNew,"%.31s",field[0]);
		if(!cIPNew[0])	
		{
			logfileLine("ChangeIPContainer cIPNew",gcQuery);
			tJobErrorUpdate(uJob,"Get cIPNew failed");
			goto CommonExit;
		}
		sscanf(cJobData,"cIPOld=%31s;",cIPOld);
		if((cp=strchr(cIPOld,';')))
			*cp=0;
		if(!cIPOld[0])	
		{
			logfileLine("ChangeIPContainer cIPOld from cJobData=",cJobData);
			tJobErrorUpdate(uJob,"Get cIPOld failed");
			goto CommonExit;
		}

		if(uNotValidSystemCallArg(cIPNew)||uNotValidSystemCallArg(cIPOld))
		{
			logfileLine("ChangeIPContainer","security alert");
			tJobErrorUpdate(uJob,"failed sec alert!");
			goto CommonExit;
		}


		if(!uVeth)
		{
			//0-.
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u",uContainer);
			if(system(gcQuery))
			{
				logfileLine("ChangeIPContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl stop failed");
				goto CommonExit;
			}

			//1-.
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cIPNew);
			if(system(gcQuery))
			{
				logfileLine("ChangeIPContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl set ipadd failed");
				goto CommonExit;
			}
	
	
			//2-.
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel %s --save",uContainer,cIPOld);
			if(system(gcQuery))
			{
				logfileLine("ChangeIPContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl set ipdel failed");
				goto CommonExit;
			}
		}
		else
		{
			//VETH based container
			sprintf(gcQuery,"/usr/sbin/vzctl exec %u \"/sbin/ifconfig eth0 0\"",uContainer);
			if(system(gcQuery))
			{
				logfileLine("ChangeIPContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl exec 1");
				goto CommonExit;
			}

			sprintf(gcQuery,"/usr/sbin/vzctl exec %u \"/sbin/ip addr add %s dev eth0\"",
						uContainer,cIPNew);
			if(system(gcQuery))
			{
				logfileLine("ChangeIPContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl exec 2");
				goto CommonExit;
			}

			sprintf(gcQuery,"/usr/sbin/vzctl exec %u \"/sbin/ip route add default dev eth0\"",uContainer);
			if(system(gcQuery))
			{
				logfileLine("ChangeIPContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl exec 3");
				goto CommonExit;
			}

			sprintf(gcQuery,"/sbin/ip route del %.31s dev veth%u.0",cIPOld,uContainer);
			if(system(gcQuery))
			{
				logfileLine("ChangeIPContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl exec 4");
				goto CommonExit;
			}

			sprintf(gcQuery,"/sbin/ip route add %.31s dev veth%u.0",cIPNew,uContainer);
			if(system(gcQuery))
			{
				logfileLine("ChangeIPContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl exec 4");
				goto CommonExit;
			}
		}

		//3-.
		if(!uNoRelease)
		{
			sprintf(gcQuery,"UPDATE tIP SET uAvailable=1 WHERE cLabel='%.31s'",cIPOld);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ChangeIPContainer",mysql_error(&gMysql));
				tJobErrorUpdate(uJob,"Release old IP from tIP failed");
				goto CommonExit;
			}
		}

		if(!uVeth)
		{
			//4-.
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
			if(system(gcQuery))
			{
				logfileLine("ChangeIPContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl start failed");
				goto CommonExit;
			}
		}

		//Everything ok
		sprintf(gcQuery,"/usr/sbin/vzctl set %u --onboot yes --save",uContainer);
		if(system(gcQuery))
			logfileLine("ChangeIPContainer",gcQuery);
		SetContainerStatus(uContainer,1);//Active
		tJobDoneUpdate(uJob);

	}
	else
	{
		logfileLine("ChangeIPContainer",gcQuery);
		tJobErrorUpdate(uJob,"No line from query");
	}

//This goto MIGHT be ok
CommonExit:
	mysql_free_result(res);
	return;

}//void ChangeIPContainer()


void ChangeHostnameContainer(unsigned uJob,unsigned uContainer,char *cJobData)
{
	char cHostname[100]={""};
	char cName[100]={""};
	char cTimezone[256]={""};
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uPrevStatus=0;

	//Check 1-. Check to make sure container is on this node, if not 
	//	give job back to queue
	sprintf(gcQuery,"/usr/sbin/vzlist %u > /dev/null 2>&1",uContainer);
	if(system(gcQuery))
	{
		logfileLine("ChangeHostnameContainer","Job returned to queue no such container");
		tJobWaitingUpdate(uJob);
		return;
	}

	//Check 2-. Wait till any other jobs currently in the job queue for this
	//	container are done.
	sprintf(gcQuery,"SELECT uJob FROM tJob"
			" WHERE uContainer=%u AND (uJobStatus=%u OR uJobStatus=%u) AND uJob!=%u"
			" AND cJobName!='ChangeIPContainer'"
			" AND cJobName!='ActivateNATNode'"
			" AND cJobName!='ActivateNATContainer'",
					uContainer,uWAITING,uRUNNING,uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ChangeHostnameContainer",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		logfileLine("ChangeHostnameContainer","Job returned to queue other jobs pending");
		tJobWaitingUpdate(uJob);
		mysql_free_result(res);
		return;
	}
	mysql_free_result(res);


	//New for external script
	char cPrevHostname[100]={""};
	char *cp;
	sscanf(cJobData,"cPrevHostname=%99s;",cPrevHostname);
	if((cp=strchr(cPrevHostname,';')))
		*cp=0;
	if((cp=strstr(cJobData,"uPrevStatus=")))
		sscanf(cp+(strlen("uPrevStatus=")),"%u;",&uPrevStatus);

	sprintf(gcQuery,"SELECT tContainer.cLabel,tContainer.cHostname"
			" FROM tContainer WHERE uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ChangeHostnameContainer",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cName,"%.99s",field[0]);
		sprintf(cHostname,"%.99s",field[1]);

		if(!cHostname[0] || !cName[0])	
		{
			logfileLine("ChangeHostnameContainer",gcQuery);
			tJobErrorUpdate(uJob,"Get cHostname failed");
			goto CommonExit;
		}

		if(uNotValidSystemCallArg(cHostname)||uNotValidSystemCallArg(cName))
		{
			logfileLine("ChangeHostnameContainer","security alert");
			tJobErrorUpdate(uJob,"failed sec alert!");
			goto CommonExit;
		}

		//1-.
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --hostname %s --name %s --save",
					uContainer,cHostname,cName);
		if(system(gcQuery))
		{
			logfileLine("ChangeHostnameContainer",gcQuery);
			tJobErrorUpdate(uJob,"vzctl set hostname failed");
			goto CommonExit;
		}

		//Everything ok
		//If previous status exists AND is not uSTOPPED or uACTIVE we use it.
		//otherwise we use the real status that can only be uSTOPPED or uACTIVE
		//This allows us to keep transitional states but also allow
		//to correct the system status when the real disk status is different from
		//the syste, status
		unsigned uRealStatus=uContainerStatus(uContainer);
		if( uPrevStatus && (uPrevStatus!=uSTOPPED || uPrevStatus!=uACTIVE) )
			SetContainerStatus(uContainer,uPrevStatus);
		else
			SetContainerStatus(uContainer,uRealStatus);
		tJobDoneUpdate(uJob);
	}
	else
	{
		logfileLine("ChangeHostnameContainer",gcQuery);
		tJobErrorUpdate(uJob,"No line from query");
	}


	//Optional group based script may exist to be executed.
	//
	//Primary group is oldest tGroupGlue entry.
	//UBC safe
	sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
			" AND tProperty.uKey=tGroupGlue.uGroup"
			" AND tGroupGlue.uContainer=%u"
			" AND tProperty.cName='cJob_OnChangeHostnameScript' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",uPROP_GROUP,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ChangeHostnameContainer",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		struct stat statInfo;
		char cOnScriptCall[512];
		char cCommand[256];
		char *cp;

		sprintf(cCommand,"%.255s",field[0]);

		//Remove trailing junk
		if((cp=strchr(cCommand,'\n')) || (cp=strchr(cCommand,'\r'))) *cp=0;

		if(uNotValidSystemCallArg(cCommand))
		{
			logfileLine("ChangeHostnameContainer","cJob_OnChangeHostnameScript security alert");
			goto CommonExit2;
		}

		//Only run if command is chmod 500 and owned by root for extra security reasons.
		if(stat(cCommand,&statInfo))
		{
			logfileLine("ChangeHostnameContainer","stat failed for cJob_OnChangeHostnameScript");
			logfileLine("ChangeHostnameContainer",cCommand);
			goto CommonExit2;
		}
		if(statInfo.st_uid!=0)
		{
			logfileLine("ChangeHostnameContainer","cJob_OnChangeHostnameScript is not owned by root");
			goto CommonExit2;
		}
		if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
		{
			logfileLine("ChangeHostnameContainer","cJob_OnChangeHostnameScript is not chmod 500");
			goto CommonExit2;
		}

		sprintf(cOnScriptCall,"%.255s %.64s %u %.99s",cCommand,cHostname,uContainer,cPrevHostname);
		if(system(cOnScriptCall))
		{
			logfileLine("ChangeHostnameContainer",cOnScriptCall);
		}
	}

CommonExit2:

	//Please note that some container programs may need to also have time based info
	//set or maybe restarted. This should be done by the above script.
	//Since it is beyond the scope of unxsVZ to handle all the diff types of container
	//Good example is a complex service container like a FreePBX/Asterisk virtual server.

	//Optional container CentOS linux timezone set. See New also.
	//Example (cOrg_TimeZone) cTimezone "Europe/Zurich"
	//For /usr/share/zoneinfo/Europe/Zurich
	GetContainerProp(uContainer,"cOrg_TimeZone",cTimezone);
	if(cTimezone[0] && !uNotValidSystemCallArg(cTimezone) )
	{
		sprintf(gcQuery,"rm -f /vz/private/%u/etc/localtime",uContainer);
		if(system(gcQuery))
		{
			logfileLine("ChangeHostnameContainer",gcQuery);
		}
		else
		{
			//The link target is internal to container
			sprintf(gcQuery,"ln -s /usr/share/zoneinfo/%s /vz/private/%u/etc/localtime",
							cTimezone,uContainer);
			if(system(gcQuery))
				logfileLine("ChangeHostnameContainer",gcQuery);
			else
				logfileLine("ChangeHostnameContainer","Container timezone changed");
		}
	}

CommonExit:
	mysql_free_result(res);
	return;

}//void ChangeHostnameContainer()


void StopContainer(unsigned uJob,unsigned uContainer)
{

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u",uContainer);
	if(system(gcQuery))
	{
		logfileLine("StopContainer",gcQuery);
		tJobErrorUpdate(uJob,"vzctl stop failed");
		return;
	}

	//2-.
	sprintf(gcQuery,"/usr/sbin/vzctl set %u --onboot no --save",uContainer);
	if(system(gcQuery))
	{
		logfileLine("StopContainer",gcQuery);
		tJobErrorUpdate(uJob,"vzctl set failed");
		return;
	}

	//Optional group based script may exist to be executed.
	//
	//Primary group is oldest tGroupGlue entry.
	//UBC safe
        MYSQL_RES *res;
        MYSQL_ROW field;
	sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
			" AND tProperty.uKey=tGroupGlue.uGroup"
			" AND tGroupGlue.uContainer=%u"
			" AND tProperty.cName='cJob_OnStopScript' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",uPROP_GROUP,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("StopContainer",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		struct stat statInfo;
		char cOnScriptCall[512];
		char cCommand[256];
		char *cp;

		sprintf(cCommand,"%.255s",field[0]);

		//Remove trailing junk
		if((cp=strchr(cCommand,'\n')) || (cp=strchr(cCommand,'\r'))) *cp=0;

		if(uNotValidSystemCallArg(cCommand))
		{
			logfileLine("StopContainer","cJob_OnStopScript security alert");
			goto CommonExit2;
		}

		//Only run if command is chmod 500 and owned by root for extra security reasons.
		if(stat(cCommand,&statInfo))
		{
			logfileLine("StopContainer","stat failed for cJob_OnStopScript");
			logfileLine("StopContainer",cCommand);
			goto CommonExit2;
		}
		if(statInfo.st_uid!=0)
		{
			logfileLine("StopContainer","cJob_OnStopScript is not owned by root");
			goto CommonExit2;
		}
		if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
		{
			logfileLine("StopContainer","cJob_OnStopScript is not chmod 500");
			goto CommonExit2;
		}

		char cHostname[100]={""};
		sprintf(gcQuery,"SELECT tContainer.cHostname"
				" FROM tContainer WHERE uContainer=%u",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("",mysql_error(&gMysql));
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(cHostname,"%.99s",field[0]);

			sprintf(cOnScriptCall,"%.255s %.64s %u",cCommand,cHostname,uContainer);
			if(system(cOnScriptCall))
			{
				logfileLine("StopContainer",cOnScriptCall);
			}
		}
	}
CommonExit2:
	mysql_free_result(res);

	//Everything ok
	SetContainerStatus(uContainer,uSTOPPED);
	tJobDoneUpdate(uJob);

}//void StopContainer()


void StartContainer(unsigned uJob,unsigned uContainer)
{
	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
	if(system(gcQuery))
	{
		logfileLine("StartContainer",gcQuery);
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose restart %u",uContainer);
		if(system(gcQuery))
		{
			logfileLine("StartContainer",gcQuery);
			tJobErrorUpdate(uJob,"vzctl start/restart failed");
			return;
		}
	}

	//2-.
	sprintf(gcQuery,"/usr/sbin/vzctl set %u --onboot yes --save",uContainer);
	if(system(gcQuery))
	{
		logfileLine("StartContainer",gcQuery);
		tJobErrorUpdate(uJob,"vzctl set failed");
		return;
	}

	//Optional group based script may exist to be executed.
	//
	//Primary group is oldest tGroupGlue entry.
        MYSQL_RES *res;
        MYSQL_ROW field;
	//UBC safe
	sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
			" AND tProperty.uKey=tGroupGlue.uGroup"
			" AND tGroupGlue.uContainer=%u"
			" AND tProperty.cName='cJob_OnStartScript' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",uPROP_GROUP,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("StartContainer",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		struct stat statInfo;
		char cOnScriptCall[512];
		char cCommand[256];
		char *cp;

		sprintf(cCommand,"%.255s",field[0]);

		//Remove trailing junk
		if((cp=strchr(cCommand,'\n')) || (cp=strchr(cCommand,'\r'))) *cp=0;

		if(uNotValidSystemCallArg(cCommand))
		{
			logfileLine("StartContainer","cJob_OnStartScript security alert");
			goto CommonExit2;
		}

		//Only run if command is chmod 500 and owned by root for extra security reasons.
		if(stat(cCommand,&statInfo))
		{
			logfileLine("StartContainer","stat failed for cJob_OnStartScript");
			logfileLine("StartContainer",cCommand);
			goto CommonExit2;
		}
		if(statInfo.st_uid!=0)
		{
			logfileLine("StartContainer","cJob_OnStartScript is not owned by root");
			goto CommonExit2;
		}
		if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
		{
			logfileLine("StartContainer","cJob_OnStartScript is not chmod 500");
			goto CommonExit2;
		}

		char cHostname[100]={""};
		sprintf(gcQuery,"SELECT tContainer.cHostname"
				" FROM tContainer WHERE uContainer=%u",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("",mysql_error(&gMysql));
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(cHostname,"%.99s",field[0]);

			sprintf(cOnScriptCall,"%.255s %.64s %u",cCommand,cHostname,uContainer);
			if(system(cOnScriptCall))
			{
				logfileLine("StartContainer",cOnScriptCall);
			}
		}
	}
CommonExit2:
	mysql_free_result(res);

	//Everything ok
	SetContainerStatus(uContainer,uACTIVE);
	tJobDoneUpdate(uJob);

}//void StartContainer(...)


//Covers Migration Wizard and Remote Migration
//Remote Migration must setup ChangeIP and DNS jobs
void MigrateContainer(unsigned uJob,unsigned uContainer,char *cJobData)
{
	char cTargetNodeIPv4[256]={""};
	char cIPv4[32]={""};
	unsigned uTargetNode=0;
	unsigned uTargetDatacenter=0;
	unsigned uIPv4=0;
	unsigned uPrevStatus=0;

	sscanf(cJobData,"uTargetNode=%u;",&uTargetNode);
	if(!uTargetNode)
	{
		logfileLine("MigrateContainer","Could not determine uTargetNode");
		tJobErrorUpdate(uJob,"uTargetNode==0");
		return;
	}

	if(uCheckMaxContainers(uTargetNode))
	{
		logfileLine("MigrateContainer","Max limit containers reached");
		tJobErrorUpdate(uJob,"uTargetNode==0");
		return;
	}

	if(uCheckMaxCloneContainers(uTargetNode))
	{
		logfileLine("MigrateContainer","Max clone limit containers reached");
		tJobErrorUpdate(uJob,"uTargetNode==0");
		return;
	}

	//New...Now we always get the target datacenter
	sscanf(ForeignKey("tNode","uDatacenter",uTargetNode),"%u",&uTargetDatacenter);
	if(!uTargetDatacenter)
	{
		logfileLine("MigrateContainer","Could not determine uTargetDatacenter");
		tJobErrorUpdate(uJob,"uTargetDatacenter==0");
		return;
	}

	sscanf(cJobData,"uTargetNode=%*u;\nuIPv4=%u;",&uIPv4);
	if(uIPv4)
	{
		logfileLine("MigrateContainer","Migration with new IP");
		sprintf(cIPv4,"%.31s",ForeignKey("tIP","cLabel",uIPv4));
		logfileLine("MigrateContainer",cIPv4);

		//Remote migration uses node name for ssh
		sprintf(cTargetNodeIPv4,"%.255s",ForeignKey("tNode","cLabel",uTargetNode));
	}
	else
	{
		//Local cluster migration use private LAN IP
		GetNodeProp(uTargetNode,"cIPv4",cTargetNodeIPv4);
	}

	if(!cTargetNodeIPv4[0])
	{
		logfileLine("MigrateContainer","Could not determine cTargetNodeIPv4");
		tJobErrorUpdate(uJob,"cTargetNodeIPv4");
		return;
	}

	sscanf(cJobData,"uTargetNode=%*u;\nuIPv4=%*u;\nuPrevStatus=%u;",&uPrevStatus);
	if(uPrevStatus!=uACTIVE && uPrevStatus!=uSTOPPED)
	{
		sprintf(gcQuery,"cJobData uPrevStatus not active or stopped. uPrevStatus=%u",uPrevStatus);
		logfileLine("MigrateContainer",gcQuery);
		tJobErrorUpdate(uJob,"uPrevStatus problem");
		return;
	}

	//Optional bandwidth limit for inter datacenter transfers
	char cVZMigrateBWLimit[32]={""};
	char cSCPBWLimit[64]={""};
	if(uTargetDatacenter && uTargetDatacenter!=gfuDatacenter)
	{
        	MYSQL_RES *res;
        	MYSQL_ROW field;

		//UBC safe
		sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
				" AND tProperty.uKey=tGroupGlue.uGroup"
				" AND tGroupGlue.uContainer=%u"
				" AND tProperty.cName='cJob_MigrateBW' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",
						uPROP_GROUP,uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("MigrateContainer",mysql_error(&gMysql));
			tJobErrorUpdate(uJob,"cJob_MigrateBW error");
			return;
		}
        	res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			unsigned uBWLimit=0;//0 is no limit
			sscanf(field[0],"%u",&uBWLimit);
			sprintf(cSCPBWLimit," -l %u",uBWLimit);
			sprintf(cVZMigrateBWLimit,"--rsync=\" --bwlimit=%u\"",uBWLimit);
			//debug only
			logfileLine("MigrateContainer uBWLimit",field[0]);
		}
		mysql_free_result(res);
	}

	//vzmigrate --online -v <destination_address> <veid>
	//Most specific tConfiguration is used. This allows for some nodes to be set global
	//and others specific. But is slower than the other option with what maybe
	//very large numbers of per node tConfiguration entries.
	char cSSHOptions[256]={""};
	GetConfiguration("cSSHOptions",cSSHOptions,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cSSHOptions[0])
	{
		GetConfiguration("cSSHOptions",cSSHOptions,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cSSHOptions[0])
			GetConfiguration("cSSHOptions",cSSHOptions,0,0,0,0);//Last try global
	}
	if(uNotValidSystemCallArg(cSSHOptions))
	{
		logfileLine("MigrateContainer","security alert");
		cSSHOptions[0]=0;
	}
	char cSCPOptions[256]={""};
	GetConfiguration("cSCPOptions",cSCPOptions,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cSCPOptions[0])
	{
		GetConfiguration("cSCPOptions",cSCPOptions,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cSCPOptions[0])
			GetConfiguration("cSCPOptions",cSCPOptions,0,0,0,0);//Last try global
	}
	//Default for less conditions below
	//Configure normal default via /etc/ssh/ssh_config
	if(uNotValidSystemCallArg(cSCPOptions))
		sprintf(cSCPOptions,"-P 22 -c arcfour");


	if(uPrevStatus==uACTIVE)
	{
		if(cSSHOptions[0])
			sprintf(gcQuery,"export PATH=/usr/sbin:/usr/bin:/bin:/usr/local/bin:/usr/local/sbin;"
				"/usr/sbin/vzmigrate --ssh=\"%s\" %s --keep-dst --online -v %s %u",
					cSSHOptions,cVZMigrateBWLimit,cTargetNodeIPv4,uContainer);
		else
			sprintf(gcQuery,"export PATH=/usr/sbin:/usr/bin:/bin;"
				"/usr/sbin/vzmigrate %s --keep-dst --online -v %s %u",
					cVZMigrateBWLimit,cTargetNodeIPv4,uContainer);
	}
	else
	{
		if(cSSHOptions[0])
			sprintf(gcQuery,"export PATH=/usr/sbin:/usr/bin:/bin:/usr/local/bin:/usr/local/sbin;"
				"/usr/sbin/vzmigrate --ssh=\"%s\" %s --keep-dst -v %s %u",
					cSSHOptions,cVZMigrateBWLimit,cTargetNodeIPv4,uContainer);
		else
			sprintf(gcQuery,"export PATH=/usr/sbin:/usr/bin:/bin;"
				"/usr/sbin/vzmigrate %s --keep-dst -v %s %u",
					cVZMigrateBWLimit,cTargetNodeIPv4,uContainer);
	}

	if(system(gcQuery))
	{
		//We may not want this optional behavior may violate QoS for given migration
		logfileLine("MigrateContainer","Trying offline migration");

		if(cSSHOptions[0])
			sprintf(gcQuery,"export PATH=/usr/sbin:/usr/bin:/bin:/usr/local/bin:/usr/local/sbin;"
				"/usr/sbin/vzmigrate --ssh=\"%s\" %s --keep-dst -v %s %u",
					cSSHOptions,cVZMigrateBWLimit,cTargetNodeIPv4,uContainer);
		else
			sprintf(gcQuery,"export PATH=/usr/sbin:/usr/bin:/bin:/usr/local/bin:/usr/local/sbin;"
				"/usr/sbin/vzmigrate %s --keep-dst -v %s %u",
					cVZMigrateBWLimit,cTargetNodeIPv4,uContainer);
		if(system(gcQuery))
		{
			logfileLine("MigrateContainer","Giving up!");
			logfileLine("MigrateContainer error",gcQuery);
			tJobErrorUpdate(uJob,"vzmigrate on/off-line failed");
			return;
		}
	}

	//If container rrd files exists in our standard fixed dir cp to new node
	//Note that cleanup maybe required in the future
	sprintf(gcQuery,"/var/lib/rrd/%u.rrd",uContainer);
	if(!access(gcQuery,R_OK))
	{
		sprintf(gcQuery,"/usr/bin/scp %s%s /var/lib/rrd/%u.rrd %s:/var/lib/rrd/",
			cSCPOptions,cSCPBWLimit,uContainer,cTargetNodeIPv4);
		if(system(gcQuery))
			logfileLine("MigrateContainer",gcQuery);
	}

	//Everything ok
	SetContainerStatus(uContainer,uPrevStatus);//Previous to awaiting migration
	SetContainerNode(uContainer,uTargetNode);//Migrated!
	SetContainerDatacenter(uContainer,uTargetDatacenter);
	tJobDoneUpdate(uJob);

}//void MigrateContainer(...)




//UBC safe
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


//Do not use for UBC props. Not distributed UBC safe. Use the below function.
void GetContainerProp(const unsigned uContainer,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0) return;

	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=3 AND cName='%s'",
				uContainer,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(gLfp!=NULL)
		{
			logfileLine("GetContainerProp",mysql_error(&gMysql));
			exit(2);
		}
		else
		{
			htmlPlainTextError(mysql_error(&gMysql));
		}
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

}//void GetContainerProp(...)


//UBC safe
//Only for UBC properties
void GetContainerPropUBC(const unsigned uContainer,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0) return;

	unsigned uDatacenter=0;
	sscanf(ForeignKey("tContainer","uDatacenter",uContainer),"%u",&uDatacenter);
	if(!uDatacenter)
	{
		logfileLine("GetContainerPropUBC","!uDatacenter error");
		return;
	}
	unsigned uPrivate=0;
	if(gfuDatacenter && gfuDatacenter==uDatacenter)
		uPrivate=1;
	if(ConnectToOptionalUBCDb(uDatacenter,uPrivate))
	{
		logfileLine("GetContainerPropUBC","ConnectToOptionalUBCDb error");
		return;
	}
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=3 AND cName='%s'",
				uContainer,cName);
	mysql_query(&gMysqlUBC,gcQuery);
	if(mysql_errno(&gMysqlUBC))
	{
		logfileLine("GetContainerPropUBC",mysql_error(&gMysqlUBC));
		return;	
	}
        res=mysql_store_result(&gMysqlUBC);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		if((cp=strchr(field[0],'\n')))
			*cp=0;
		sprintf(cValue,"%.255s",field[0]);
	}
	mysql_free_result(res);
	mysql_close(&gMysqlUBC);

}//void GetContainerPropUBC()


void UpdateContainerUBC(unsigned uJob,unsigned uContainer,const char *cJobData)
{

	float luBar=0,luLimit=0;
	char cBuf[256]={""};
	char cResource[256]={""};
	char cApplyConfigPath[100]={""};
	char cContainerPath[100]={""};
	char cUBC[64]={""};
	char *cp;

	sscanf(cJobData,"cResource=%32s;",cResource);
	if((cp=strchr(cResource,';')))
		*cp=0;
	if(!cResource[0])
	{
		logfileLine("UpdateContainerUBC","Could not determine cResource");
		tJobErrorUpdate(uJob,"cResource[0]==0");
		goto CommonExit;
	}

	sprintf(cUBC,"%.32s.luBarrier",cResource);
	GetContainerPropUBC(uContainer,cUBC,cBuf);
	sscanf(cBuf,"%f",&luBar);

	sprintf(cUBC,"%.32s.luLimit",cResource);
	GetContainerPropUBC(uContainer,cUBC,cBuf);
	sscanf(cBuf,"%f",&luLimit);

	//No PID control yet. 10% increase
	//No resource based rules
	//No node resources based adjustments
	//No expert per resource rules applied
	luBar = luBar + luBar * 0.1;
	luLimit = luLimit + luLimit * 0.1;

	if(uNotValidSystemCallArg(cResource))
	{
		logfileLine("UpdateContainerUBC","security alert");
		tJobErrorUpdate(uJob,"failed sec alert!");
		goto CommonExit;
	}

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl set %u --%s %.0f:%.0f --save",
		uContainer,cResource,luBar,luLimit);
	if(system(gcQuery))
	{
		logfileLine("UpdateContainerUBC",gcQuery);
		tJobErrorUpdate(uJob,"vzctl set failed");
		goto CommonExit;
	}

	//2-. validate and/or repair conf file
	sprintf(gcQuery,"/usr/sbin/vzcfgvalidate -r /etc/vz/conf/%u.conf > "
			" /tmp/UpdateContainerUBC.vzcfgcheck.output 2>&1",uContainer);
	if(system(gcQuery))
	{
		logfileLine("UpdateContainerUBC",gcQuery);
		tJobErrorUpdate(uJob,"vzcfgvalidate failed");
		goto CommonExit;
	}

	//2a-. See if vzcfgvalidate -r changed anything if so report in log and then use
	// changed conf file for updates
	FILE *fp;
	unsigned uChange=0;
	if((fp=fopen("/tmp/UpdateContainerUBC.vzcfgcheck.output","r")))
	{
		while(fgets(cBuf,255,fp)!=NULL)
		{
			if(strncmp(cBuf,"set to ",7))
				continue;
			//May have multiple set to lines, the last one is the one we want
			sscanf(cBuf,"set to %f:%f",&luBar,&luLimit);
			uChange++;
		}
		fclose(fp);
		unlink("/tmp/UpdateContainerUBC.vzcfgcheck.output");
	}

	if(uChange)
	{
		//3-. See 4-.
		sprintf(cContainerPath,"/etc/vz/conf/%u.conf",uContainer);
		sprintf(cApplyConfigPath,"/etc/vz/conf/ve-%u.conf-sample",uContainer);
		if(symlink(cContainerPath,cApplyConfigPath))
		{
			logfileLine("UpdateContainerUBC","symlink failed");
			tJobErrorUpdate(uJob,"symlink failed");
			goto CommonExit;
		}

		//4-. Apply any changes produced by vzcfgvalidate -r
		sprintf(gcQuery,"/usr/sbin/vzctl set %u --applyconfig %u --save",
			uContainer,uContainer);
		if(system(gcQuery))
		{
			logfileLine("UpdateContainerUBC",gcQuery);
			tJobErrorUpdate(uJob,"vzctl set 4 failed");
			goto CommonExit;
		}
		unlink(cApplyConfigPath);
	}

	//5-.
	sprintf(gcQuery,"INSERT INTO tLog SET"
		" cLabel='UpdateContainerUBC()',"
		"uLogType=4,uLoginClient=1,"
		"cLogin='unxsVZ.cgi',cMessage='set %u --%s %.0f:%.0f (c=%u)',"
		"cServer='%s',uOwner=1,uCreatedBy=1,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,cResource,luBar,luLimit,uChange,cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("UpdateContainerUBC",gcQuery);
		tJobErrorUpdate(uJob,"INSERT INTO tLog failed");
		goto CommonExit;
	}

	//Everything ok
	tJobDoneUpdate(uJob);

CommonExit:
	return;

}//void UpdateContainerUBC(...)


void SetContainerUBC(unsigned uJob,unsigned uContainer,const char *cJobData)
{

	long unsigned luBar=0,luLimit=0;
	char cBuf[256];
	char cResource[64]={""};
	char cApplyConfigPath[100]={""};
	char cContainerPath[100]={""};
	char *cp;

	sscanf(cJobData,"--%63s %lu:%lu",cResource,&luBar,&luLimit);
	if((cp=strchr(cResource,' ')))
		*cp=0;
	if(!cResource[0])
	{
		logfileLine("SetContainerUBC","Could not determine cResource");
		tJobErrorUpdate(uJob,"cResource[0]==0");
		return;
	}
	if(!luBar || !luLimit)
	{
		logfileLine("SetContainerUBC","Could not determine luBar||luLimit");
		tJobErrorUpdate(uJob,"!luBar||!luLimit");
		return;
	}

	if(uNotValidSystemCallArg(cResource))
	{
		logfileLine("SetContainerUBC","security alert");
		tJobErrorUpdate(uJob,"failed sec alert!");
		return;
	}

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl set %u --%s %lu:%lu --save",
		uContainer,cResource,luBar,luLimit);
	if(system(gcQuery))
	{
		logfileLine("SetContainerUBC",gcQuery);
		tJobErrorUpdate(uJob,"vzctl set failed");
		return;
	}

	//2-. validate and/or repair conf file
	sprintf(gcQuery,"/usr/sbin/vzcfgvalidate -r /etc/vz/conf/%u.conf > "
			" /tmp/SetContainerUBC.vzcfgcheck.output 2>&1",uContainer);
	if(system(gcQuery))
	{
		logfileLine("SetContainerUBC",gcQuery);
		tJobErrorUpdate(uJob,"vzcfgvalidate failed");
		//No need to roll back above set
		return;
	}

	//2a-. See if vzcfgvalidate -r changed anything if so report in log and then use
	// changed conf file for updates
	FILE *fp;
	unsigned uChange=0;
	if((fp=fopen("/tmp/SetContainerUBC.vzcfgcheck.output","r")))
	{
		while(fgets(cBuf,255,fp)!=NULL)
		{
			if(strncmp(cBuf,"set to ",7))
				continue;
			//May have multiple set to lines, the last one is the one we want
			sscanf(cBuf,"set to %lu:%lu",&luBar,&luLimit);
			uChange++;
		}
		fclose(fp);
		unlink("/tmp/SetContainerUBC.vzcfgcheck.output");
	}

	if(uChange)
	{
		//3-. See 4-.
		sprintf(cContainerPath,"/etc/vz/conf/%u.conf",uContainer);
		sprintf(cApplyConfigPath,"/etc/vz/conf/ve-%u.conf-sample",uContainer);
		if(symlink(cContainerPath,cApplyConfigPath))
		{
			logfileLine("SetContainerUBC",cContainerPath);
			tJobErrorUpdate(uJob,"symlink failed");
			//no rollback needed
			return;
		}

		//4-. Apply any changes produced by vzcfgvalidate -r
		sprintf(gcQuery,"/usr/sbin/vzctl set %u --applyconfig %u --save",
			uContainer,uContainer);
		if(system(gcQuery))
		{
			logfileLine("SetContainerUBC",gcQuery);
			tJobErrorUpdate(uJob,"vzctl set 4 failed");
			//no rollback needed
			return;
		}
		unlink(cApplyConfigPath);
	}

	//5-.
	sprintf(gcQuery,"INSERT INTO tLog SET"
		" cLabel='SetContainerUBC()',"
		"uLogType=4,uLoginClient=1,"
		"cLogin='unxsVZ.cgi',cMessage='set %u --%s %lu:%lu (c=%u)',"
		"cServer='%s',uOwner=1,uCreatedBy=1,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,cResource,luBar,luLimit,uChange,cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("SetContainerUBC",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"INSERT INTO tLog failed");
		return;
	}

	//Everything ok
	tJobDoneUpdate(uJob);

}//void SetContainerUBC(...)


void TemplateContainer(unsigned uJob,unsigned uContainer,const char *cJobData)
{
	//Only run one vzdump job per HN. Wait for lock release.
	//Log this rare condition
	//This lock file is created by the external proxmox script vzdump
	if(access("/var/run/vzdump.lock",R_OK)==0)
	{
		logfileLine("TemplateContainer","/var/run/vzdump.lock exists");
		tJobWaitingUpdate(uJob);
		return;
	}

        MYSQL_RES *res;
        MYSQL_ROW field;

	char cConfigLabel[32]={""};
	char *cp;
	struct stat statInfo;
	unsigned uOwner=1;
	unsigned uCreatedBy=1;
	unsigned uPrevStatus=0;

	//Parse data and basic sanity checks
	sscanf(cJobData,"tConfig.Label=%31s;",cConfigLabel);
	if((cp=strchr(cConfigLabel,';')))
		*cp=0;
	if(!cConfigLabel[0])
	{
		logfileLine("TemplateContainer","Could not determine cConfigLabel");
		tJobErrorUpdate(uJob,"cConfigLabel[0]==0");
		goto CommonExit;
	}
	if((cp=strstr(cJobData,"uPrevStatus=")))
		sscanf(cp+12,"%u",&uPrevStatus);
	if(!uPrevStatus)
	{
		logfileLine("TemplateContainer","Could not determine uPrevStatus assuming active");
		uPrevStatus=1;
	}


	if(uNotValidSystemCallArg(cConfigLabel))
	{
		logfileLine("TemplateContainer","security alert");
		tJobErrorUpdate(uJob,"failed sec alert!");
		goto CommonExit;
	}

	char cSnapshotDir[256]={""};
	GetConfiguration("cSnapshotDir",cSnapshotDir,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cSnapshotDir[0])
	{
		GetConfiguration("cSnapshotDir",cSnapshotDir,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cSnapshotDir[0])
			GetConfiguration("cSnapshotDir",cSnapshotDir,0,0,0,0);//Last try global
	}
	if(uNotValidSystemCallArg(cSnapshotDir))
		cSnapshotDir[0]=0;
	//1-.
	if(!cSnapshotDir[0])
		sprintf(gcQuery,"/usr/sbin/vzdump --compress --suspend %u",uContainer);
	else
		sprintf(gcQuery,"/usr/sbin/vzdump --compress --dumpdir %s --snapshot %u",
									cSnapshotDir,uContainer);
	if(system(gcQuery))
	{
		logfileLine("TemplateContainer",gcQuery);
		tJobErrorUpdate(uJob,"vzdump error1");
		goto CommonExit;
	}

	//New vzdump uses new file format, E.G.: /var/vzdump/vzdump-openvz-10511-2011_02_03-07_37_01.tgz
	//Quick fix (hackorama) just mv it to old format
	//Added support for old vzdump
	if(!cSnapshotDir[0])
		sprintf(gcQuery,"if [ ! -f /var/vzdump/vzdump-%u.tgz ];then"
				" mv `ls -1 /vz/dump/vzdump*-%u-*.tgz | head -n 1` /vz/dump/vzdump-%u.tgz; fi;",
					uContainer,uContainer,uContainer);
	else
		sprintf(gcQuery,"if [ -f /var/vzdump/vzdump-%u.tgz ] && [ \"%s\" != \"/var/vzdump\" ];then"
				" mv /var/vzdump/vzdump-%u.tgz %s/vzdump-%u.tgz;else"
				" if [ -f %s/vzdump*-%u-*.tgz ];then mv `ls -1 %s/vzdump*-%u-*.tgz | head -n 1` %s/vzdump-%u.tgz;fi;fi;",
				uContainer,cSnapshotDir,
				uContainer,cSnapshotDir,uContainer,
				cSnapshotDir,uContainer,cSnapshotDir,uContainer,cSnapshotDir,uContainer);
	if(system(gcQuery))
	{
		logfileLine("TemplateContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 1a1");
		goto CommonExit;
	}


	//2-.	
	if(!cSnapshotDir[0])
		sprintf(gcQuery,"mv /vz/dump/vzdump-%u.tgz /vz/template/cache/%s.tar.gz",uContainer,cConfigLabel);
	else
		sprintf(gcQuery,"mv %s/vzdump-%u.tgz /vz/template/cache/%s.tar.gz",
				cSnapshotDir,uContainer,cConfigLabel);
	if(system(gcQuery))
	{
		logfileLine("TemplateContainer",gcQuery);
		tJobErrorUpdate(uJob,"mv tgz to cache tar.gz failed");
		goto CommonExit;
	}

	//2b-. md5sum generation
	if(!stat("/usr/bin/md5sum",&statInfo))
	{
		sprintf(gcQuery,"/usr/bin/md5sum /vz/template/cache/%s.tar.gz >"
				" /vz/template/cache/%s.tar.gz.md5sum",cConfigLabel,cConfigLabel);
		if(system(gcQuery))
		{
			logfileLine("TemplateContainer",gcQuery);
			tJobErrorUpdate(uJob,"md5sum tgz failed");
			goto CommonExit;
		}
	}


	//3-. scp template to all nodes depends on /usr/sbin/allnodescp.sh installed and configured correctly
	if(!stat("/usr/sbin/allnodescp.sh",&statInfo))
	{
		sprintf(gcQuery,"/usr/sbin/allnodescp.sh /vz/template/cache/%s.tar.gz",cConfigLabel);
		if(system(gcQuery))
		{
			logfileLine("TemplateContainer",gcQuery);
			tJobErrorUpdate(uJob,"allnodescp.sh .tar.gz failed");
			goto CommonExit;
		}
		sprintf(gcQuery,"/usr/sbin/allnodescp.sh /vz/template/cache/%s.tar.gz.md5sum",cConfigLabel);
		if(system(gcQuery))
		{
			logfileLine("TemplateContainer",gcQuery);
			tJobErrorUpdate(uJob,"allnodescp.sh .tar.gz.md5sum failed");
			goto CommonExit;
		}
	}

	//3b-. check transfer via md5sum
	//Relies on new version of "/usr/sbin/allnodecmd.sh" that adds return values.
	if(!stat("/usr/sbin/allnodecmd.sh",&statInfo) && !stat("/usr/bin/md5sum",&statInfo))
	{
		sprintf(gcQuery,"/usr/sbin/allnodecmd.sh"
				" \"/usr/bin/md5sum -c /vz/template/cache/%s.tar.gz.md5sum\"",cConfigLabel);
		if(system(gcQuery))
		{
			logfileLine("TemplateContainer",gcQuery);
			tJobErrorUpdate(uJob,"allnodecmd.sh md5sum -c .tar.gz.md5sum failed");
			goto CommonExit;
		}
	}

	//4-. Make /etc/vz/conf tConfig file and scp to all nodes. ve-proxpop.conf-sample
	sprintf(gcQuery,"/bin/cp /etc/vz/conf/%u.conf /etc/vz/conf/ve-%s.conf-sample",
		uContainer,cConfigLabel);
	if(system(gcQuery))
	{
		logfileLine("TemplateContainer",gcQuery);
		tJobErrorUpdate(uJob,"cp conf container failed");
		goto CommonExit;
	}
	if(!stat("/usr/sbin/allnodescp.sh",&statInfo))
	{
		sprintf(gcQuery,"/usr/sbin/allnodescp.sh /etc/vz/conf/ve-%s.conf-sample",cConfigLabel);
		if(system(gcQuery))
		{
			logfileLine("TemplateContainer",gcQuery);
			tJobErrorUpdate(uJob,"allnodescp.sh .conf failed");
			goto CommonExit;
		}
	}

	//5a-. Add tOSTemplate and tConfig entries if not already there tContainer wizard
	//should help prevent this.
	sprintf(gcQuery,"SELECT uOwner,uCreatedBy FROM tJob WHERE uJob=%u",uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("TemplateContainer",mysql_error(&gMysql));
	}
	else
	{
        	res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uOwner);
			sscanf(field[1],"%u",&uCreatedBy);
		}
		mysql_free_result(res);
	}

	sprintf(gcQuery,"SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='%s'",cConfigLabel);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("TemplateContainer",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tOSTemplate");
		goto CommonExit;
	}
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)<1)
	{
		sprintf(gcQuery,"INSERT tOSTemplate SET cLabel='%s',uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",cConfigLabel,uOwner,uCreatedBy);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("TemplateContainer",mysql_error(&gMysql));
			tJobErrorUpdate(uJob,"INSERT tOSTemplate");
			goto CommonExit;
		}
	}
	mysql_free_result(res);
	//5b-.
	sprintf(gcQuery,"SELECT uConfig FROM tConfig WHERE cLabel='%s'",cConfigLabel);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("TemplateContainer",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tConfig");
		goto CommonExit;
	}
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)<1)
	{
		sprintf(gcQuery,"INSERT tConfig SET cLabel='%s',uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",cConfigLabel,uOwner,uCreatedBy);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("TemplateContainer",mysql_error(&gMysql));
			tJobErrorUpdate(uJob,"INSERT tConfig");
			goto CommonExit;
		}
	}
	mysql_free_result(res);

	//Everything ok
	SetContainerStatus(uContainer,uPrevStatus);
	tJobDoneUpdate(uJob);

//This goto MAYBE ok
CommonExit:
	//6-. remove lock file
	//vzdump script should handle this?
	unlink("/var/run/vzdump.lock");
	return;

}//void TemplateContainer(...)


//OpenVZ action scripts
void ActionScripts(unsigned uJob,unsigned uContainer)
{
	//0 means do not overwrite existing files.
	if(CreateActionScripts(uContainer,0))
	{
		logfileLine("ActionScripts","CreateActionScripts(x,0) failed");
		tJobErrorUpdate(uJob,"CreateActionScripts(x,0) failed");
		return;
	}

	tJobDoneUpdate(uJob);
	return;

}//void ActionScripts(unsigned uJob,unsigned uContainer);


void CloneContainer(unsigned uJob,unsigned uContainer,char *cJobData)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cTargetNodeIPv4[256]={""};
	unsigned uNewVeid=0;
	unsigned uCloneStop=0;
	unsigned uStatus=0;
	unsigned uPrevStatus=0;
	unsigned uTargetNode=0;
	char cNewIP[32]={""};
	char cHostname[100]={""};
	char cName[32]={""};


	//We can't clone a container that is not ready
	if(GetContainerStatus(uContainer,&uStatus))
	{
		logfileLine("CloneContainer","GetContainerStatus() failed");
		//Keep job status running...hopefully someone will notice.
		return;
	}
	if(!(uStatus==uACTIVE || uStatus==uSTOPPED || uStatus==uAWAITCLONE))
	{
		sprintf(gcQuery,"UPDATE tJob SET uJobStatus=1,cRemoteMsg='Waiting for source container',uModBy=1,"
				"uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJob=%u",uJob);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			logfileLine("ProcessJob()",mysql_error(&gMysql));
		logfileLine("CloneContainer","Waiting");
		return;
	}

	//CloneWizard created a new tContainer with "Awaiting Clone" status.
	//CloneWizard created a job (this job) that runs on the source container node.
	//The created job has job data to complete the operations on source and
	//target nodes.
	//vzdump script must be installed on all datacenter nodes.

	//Only run one CloneContainer job per HN. Wait for lock release.
	//Log this rare condition
	//This lock file is created by the external proxmox script vzdump
	if(access("/var/run/vzdump.lock",R_OK)==0)
	{
		logfileLine("CloneContainer","/var/run/vzdump.lock exists");
		tJobWaitingUpdate(uJob);
		return;
	}

	//Set job data based vars
	sscanf(cJobData,"uTargetNode=%u;",&uTargetNode);
	if(!uTargetNode)
	{
		logfileLine("CloneContainer","Could not determine uTargetNode");
		tJobErrorUpdate(uJob,"uTargetNode==0");
		goto CommonExit;
	}
	sscanf(cJobData,"uTargetNode=%*u;\nuNewVeid=%u;",&uNewVeid);
	if(!uNewVeid)
	{
		logfileLine("CloneContainer","Could not determine uNewVeid");
		tJobErrorUpdate(uJob,"uNewVeid==0");
		goto CommonExit;
	}

	//
	//Handle remote datacenter clone jobs differently
	unsigned uTargetDatacenter=0;
	sprintf(gcQuery,"SELECT uDatacenter FROM tContainer WHERE uContainer=%u",uNewVeid);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("CloneContainer",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT uDatacenter");
		goto CommonExit;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uTargetDatacenter);
	mysql_free_result(res);
	if(gfuDatacenter!=uTargetDatacenter && uTargetDatacenter!=0 && gfuDatacenter!=0)
	{
		logfileLine("CloneContainer","CloneRemoteContainer() bypass test on");
		//CloneRemoteContainer(uJob,uContainer,cJobData,uNewVeid);
		//return;
	}

	//
	sscanf(cJobData,"uTargetNode=%*u;\nuNewVeid=%*u;\nuCloneStop=%u;",&uCloneStop);
	sscanf(cJobData,"uTargetNode=%*u;\nuNewVeid=%*u;\nuCloneStop=%*u;\nuPrevStatus=%u;",&uPrevStatus);
	if(!uPrevStatus)
	{
		logfileLine("CloneContainer","Could not determine uPrevStatus");
		tJobErrorUpdate(uJob,"uPrevStatus==0");
		goto CommonExit;
	}

	sprintf(gcQuery,"SELECT tIP.cLabel,tContainer.cHostname,tContainer.cLabel FROM tIP,tContainer"
				" WHERE tIP.uIP=tContainer.uIPv4"
				" AND tContainer.uContainer=%u",uNewVeid);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("CloneContainer",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tIP.cLabel");
		goto CommonExit;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cNewIP,"%.31s",field[0]);
		sprintf(cHostname,"%.99s",field[1]);
		sprintf(cName,"%.31s",field[2]);
	}
	mysql_free_result(res);

	if(!cNewIP[0])
	{
		logfileLine("CloneContainer","Could not determine cNewIP");
		tJobErrorUpdate(uJob,"No cNewIP");
		goto CommonExit;
	}

	if(!cHostname[0])
	{
		logfileLine("CloneContainer","Could not determine cHostname");
		tJobErrorUpdate(uJob,"No cHostname");
		goto CommonExit;
	}

	if(!cName[0])
	{
		logfileLine("CloneContainer","Could not determine cName");
		tJobErrorUpdate(uJob,"No cName");
		goto CommonExit;
	}

	//This needs it's tcontainerfunc.h counterpart on new and a tConfiguration entry
	//to set this IP to either the internal transfer IP or the public IP if servers
	//have only one NIC (yuck ;)
	GetNodeProp(uTargetNode,"cIPv4",cTargetNodeIPv4);
	if(!cTargetNodeIPv4[0])
	{
		logfileLine("CloneContainer","Could not determine cTargetNodeIPv4");
		tJobErrorUpdate(uJob,"cTargetNodeIPv4");
		goto CommonExit;
	}

	//if(uDebug)
	//{
	//	printf("uNewVeid=%u uTargetNode=%u cNewIP=%s cHostname=%s cTargetNodeIPv4=%s"
	//		"cSourceContainerIP=%s uCloneStop=%u\n",
	//			uNewVeid,uTargetNode,cNewIP,cHostname,
	//			cTargetNodeIPv4,cSourceContainerIP,uCloneStop);
	//	return;
	//}

	//1-. vzdump w/suspend on source node or if cSnapshotDir is not empty we try to use LVM2
	//2-. scp dump to target node
	//3-. restore on target node to new veid
	//4-. change ip, name and hostname
	//5-. remove any other /etc/vz/conf/veid.x files
	//6-. conditionally start new veid and modify VEID.conf file to not start on boot.
	//7-. update source container status
	//8-. update target container status
	//9-. remove /vz/dump files

	//Most specific tConfiguration is used. This allows for some nodes to be set global
	//and others specific. But is slower than the other option with what maybe
	//very large numbers of per node tConfiguration entries.
	char cSSHOptions[256]={""};
	GetConfiguration("cSSHOptions",cSSHOptions,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cSSHOptions[0])
	{
		GetConfiguration("cSSHOptions",cSSHOptions,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cSSHOptions[0])
			GetConfiguration("cSSHOptions",cSSHOptions,0,0,0,0);//Last try global
	}
	//Default for less conditions below
	if(uNotValidSystemCallArg(cSSHOptions))
		sprintf(cSSHOptions,"-p 22 -c arcfour");

	char cSCPOptions[256]={""};
	GetConfiguration("cSCPOptions",cSCPOptions,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cSCPOptions[0])
	{
		GetConfiguration("cSCPOptions",cSCPOptions,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cSCPOptions[0])
			GetConfiguration("cSCPOptions",cSCPOptions,0,0,0,0);//Last try global
	}
	//Default for security violation below
	//Configure normal default via /etc/ssh/ssh_config
	if(uNotValidSystemCallArg(cSCPOptions))
		sprintf(cSCPOptions,"-P 22 -c arcfour");

	char cSnapshotDir[256]={""};
	GetConfiguration("cSnapshotDir",cSnapshotDir,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cSnapshotDir[0])
	{
		GetConfiguration("cSnapshotDir",cSnapshotDir,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cSnapshotDir[0])
			GetConfiguration("cSnapshotDir",cSnapshotDir,0,0,0,0);//Last try global
	}
	if(uNotValidSystemCallArg(cSnapshotDir))
		cSnapshotDir[0]=0;
	//1-.
	//if(!cSnapshotDir[0])
		//sprintf(gcQuery,"/usr/sbin/vzdump --compress --suspend %u",uContainer);
		sprintf(gcQuery,"/usr/sbin/vzdump --compress %u",uContainer);
	//else
	//	sprintf(gcQuery,"/usr/sbin/vzdump --compress --dumpdir %s --snapshot %u",
	//									cSnapshotDir,uContainer);
	if(system(gcQuery))
	{
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 1");
		goto CommonExit;
	}

	//debug test only
	cSnapshotDir[0]=0;

	//Support for old and new vzdump
	//New vzdump uses new file format, E.G.: /var/vzdump/vzdump-openvz-10511-2011_02_03-07_37_01.tgz
	//Quick fix (hackorama) just mv it to old format
	if(!cSnapshotDir[0])
		sprintf(gcQuery,"if [ ! -f /var/vzdump/vzdump-%1$u.tgz ] && [ ! -f /vz/dump/vzdump-%1$u.tgz ];then"
				" mv `ls -1 /vz/dump/vzdump*-%1$u*.tgz | head -n 1` /vz/dump/vzdump-%1$u.tgz; fi;",
						uContainer);
	else
		sprintf(gcQuery,"if [ -f /var/vzdump/vzdump-%u.tgz ] && [ \"%s\" != \"/var/vzdump\" ];then"
				" mv /var/vzdump/vzdump-%u.tgz %s/vzdump-%u.tgz;else"
				" if [ -f %s/vzdump*-%u*.tgz ];then mv `ls -1 %s/vzdump*-%u*.tgz | head -n 1` %s/vzdump-%u.tgz;fi;fi;",
				uContainer,cSnapshotDir,
				uContainer,cSnapshotDir,uContainer,
				cSnapshotDir,uContainer,cSnapshotDir,uContainer,cSnapshotDir,uContainer);
	if(system(gcQuery))
	{
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 1a1a");
		goto CommonExit;
	}


	//1a-. Need an md5sum file created. TODO
	if(!cSnapshotDir[0])
		sprintf(gcQuery,"/usr/bin/md5sum /vz/dump/vzdump-%u.tgz > /vz/dump/vzdump-%u.tgz.md5sum",
				uContainer,uContainer);
	else
		//Some trickery to get the right file listed in the md5sum file
		sprintf(gcQuery,"ln -s %s/vzdump-%u.tgz /vz/dump/vzdump-%u.tgz;"
				" /usr/bin/md5sum /vz/dump/vzdump-%u.tgz > %s/vzdump-%u.tgz.md5sum;"
				" rm -f /vz/dump/vzdump-%u.tgz",
				cSnapshotDir,uContainer,uContainer,uContainer,cSnapshotDir,uContainer,uContainer);
	if(system(gcQuery))
	{
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 1a");
		goto CommonExit;
	}


	//2-.
	if(uNotValidSystemCallArg(cTargetNodeIPv4))
	{
		tJobErrorUpdate(uJob,"fail sec alert!");
		goto CommonExit;
	}
	//Optional bandwidth limit for inter datacenter transfers
	char cSCPBWLimit[32]={""};
	if(uTargetDatacenter && uTargetDatacenter!=gfuDatacenter)
	{
        	MYSQL_RES *res;
        	MYSQL_ROW field;

		//UBC safe
		sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
				" AND tProperty.uKey=tGroupGlue.uGroup"
				" AND tGroupGlue.uContainer=%u"
				" AND tProperty.cName='cJob_CloneBW' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",
						uPROP_GROUP,uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("CloneContainer",mysql_error(&gMysql));
			tJobErrorUpdate(uJob,"cJob_CloneBW error");
			return;
		}
        	res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			unsigned uBWLimit=0;//0 is no limit
			sscanf(field[0],"%u",&uBWLimit);
			sprintf(cSCPBWLimit," -l %u",uBWLimit);
			//debug only
			logfileLine("CloneContainer uBWLimit",field[0]);
		}
		mysql_free_result(res);
	}
	if(!cSnapshotDir[0])
		sprintf(gcQuery,"/usr/bin/scp %s%s /vz/dump/vzdump-%u.tgz %s:/vz/dump/vzdump-%u.tgz",
				cSCPOptions,cSCPBWLimit,uContainer,cTargetNodeIPv4,uContainer);
	else
		sprintf(gcQuery,"/usr/bin/scp %s%s %s/vzdump-%u.tgz %s:/vz/dump/vzdump-%u.tgz",
				cSCPOptions,cSCPBWLimit,cSnapshotDir,uContainer,cTargetNodeIPv4,uContainer);
	if(system(gcQuery))
	{
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 2");
		//roll back large file created above
		sprintf(gcQuery,"rm -f /var/vzdump/vzdump-%u.tgz",uContainer);
		if(system(gcQuery))
		{
			logfileLine("CloneContainer",gcQuery);
			tJobErrorUpdate(uJob,"error 2+rb1");
		}
		goto CommonExit;
	}

	//2a-. Need md5sum file moved to target. TODO
	if(!cSnapshotDir[0])
		sprintf(gcQuery,"/usr/bin/scp %s%s /vz/dump/vzdump-%u.tgz.md5sum %s:/vz/dump/vzdump-%u.tgz.md5sum",
				cSCPOptions,cSCPBWLimit,uContainer,cTargetNodeIPv4,uContainer);
	else
		sprintf(gcQuery,"/usr/bin/scp %s%s %s/vzdump-%u.tgz.md5sum %s:/vz/dump/vzdump-%u.tgz.md5sum",
				cSCPOptions,cSCPBWLimit,cSnapshotDir,uContainer,cTargetNodeIPv4,uContainer);
	if(system(gcQuery))
	{
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 2a");
		goto CommonExit;
	}

	//2b-. Need an md5sum check. TODO
	sprintf(gcQuery,"ssh %s %s '/usr/bin/md5sum -c /vz/dump/vzdump-%u.tgz.md5sum'",
				cSSHOptions,cTargetNodeIPv4,uContainer);
	if(system(gcQuery))
	{
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 2b");
		goto CommonExit;
	}

	//3-.
	sprintf(gcQuery,"ssh %s %s '/usr/sbin/vzdump --compress --restore /vz/dump/vzdump-%u.tgz %u'",
				cSSHOptions,cTargetNodeIPv4,uContainer,uNewVeid);
	//debug only
	//logfileLine("CloneContainer",gcQuery);
	if(system(gcQuery))
	{
		//We need to check the error number and if like this error (that maybe be related to cascading cloning
		//operations):
		//INFO: extracting configuration to '/etc/vz/conf/3281.conf'
		//command 'sed -e 's/VE_ROOT=.*/VE_ROOT=\"\/vz\/root\/$VEID\"/' -e 's/VE_PRIVATE=.*/VE_PRIVATE=\"\/vz\/private\/$VEID\"/'  <'/vz/private/3281/etc/vzdump/vps.conf' >'/etc/vz/conf/3281.conf'' failed with exit code 1
		//INFO: sh: /vz/private/3281/etc/vzdump/vps.conf: No such file or directory
		//Apr 01 17:24:19 jobqueue.CloneContainer[16404]: ssh -p 12337 -c blowfish -C 10.0.10.2 '/usr/sbin/vzdump --compress --restore /vz/dump/vzdump-2704.tgz 3281'
		//then we "resubmit" the job for 30 mins in the future. 30 mins tConfiguration configurable.

		//Try new version vzrestore
		sprintf(gcQuery,"ssh %s %s '/usr/sbin/vzrestore /vz/dump/vzdump-%u.tgz %u'",
				cSSHOptions,cTargetNodeIPv4,uContainer,uNewVeid);
		if(system(gcQuery))
		{
			logfileLine("CloneContainer",gcQuery);
			tJobErrorUpdate(uJob,"error 3");
			goto CommonExit;
		}
	}

	//4a-.
	if(uNotValidSystemCallArg(cHostname))
	{
		tJobErrorUpdate(uJob,"fail sec alert!");
		goto CommonExit;
	}
	sprintf(gcQuery,"ssh %s %s 'vzctl set %u --hostname %s --save'",
				cSSHOptions,cTargetNodeIPv4,uNewVeid,cHostname);
	//debug only
	//logfileLine("CloneContainer",gcQuery);
	if(system(gcQuery))
	{
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 4a");
		goto CommonExit;
	}

	//4b-.
	sprintf(gcQuery,"ssh %s %s 'vzctl set %u --name %s --save'",
				cSSHOptions,cTargetNodeIPv4,uNewVeid,cName);
	//debug only
	//logfileLine("CloneContainer",gcQuery);
	if(system(gcQuery))
	{
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 4b");
		goto CommonExit;
	}

	//4c-.
	//Some containers have more than one IP when how? Not via jobqueue.c
	//Only via action scripts other IPs are used.
	//Any way we can use --ipdel all instead of ticket #83
	if(uNotValidSystemCallArg(cTargetNodeIPv4))
	{
		tJobErrorUpdate(uJob,"fail sec alert!");
		goto CommonExit;
	}
	sprintf(gcQuery,"ssh %s %s 'vzctl set %u --ipdel all --save'",
				cSSHOptions,cTargetNodeIPv4,uNewVeid);
	if(system(gcQuery))
	{
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 4c");
		goto CommonExit;
	}
	//4d-.
	sprintf(gcQuery,"ssh %s %s 'vzctl set %u --ipadd %s --save'",
				cSSHOptions,cTargetNodeIPv4,uNewVeid,cNewIP);
	//debug only
	//logfileLine("CloneContainer",gcQuery);
	if(system(gcQuery))
	{
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 4d");
		goto CommonExit;
	}

	//5-. Remove action script files if found
	//Business logic: Target container may have these files, but we need them only if we
	//'Failover' to this cloned VE. Also we defer to that action the setup of the
	//containers tProperty values needed for processing the action script templates:
	//cNetmask, cExtraNodeIP, cPrivateIPs, cService1, cService2, cVEID.mount and cVEID.umount, etc.
	sprintf(gcQuery,"ssh %3$s %1$s 'rm -f /etc/vz/conf/%2$u.umount /etc/vz/conf/%2$u.mount"
					" /etc/vz/conf/%2$u.start /etc/vz/conf/%2$u.stop'",
						cTargetNodeIPv4,uNewVeid,cSSHOptions);
	if(system(gcQuery))
	{
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 5");
		goto CommonExit;
	}

	//6-.
	//This is optional since clones can be in stopped state and still be
	//kept in sync with source container. Also the failover can start (a fast operation)
	//with the extra advantage of being able to keep original IPs. Only needing an arping
	//to move the VIPs around the datacenter.
	if(uCloneStop==0)
	{
		sprintf(gcQuery,"ssh %s %s 'vzctl start %u'",cSSHOptions,cTargetNodeIPv4,uNewVeid);
		if(system(gcQuery))
		{
			logfileLine("CloneContainer",gcQuery);
			tJobErrorUpdate(uJob,"error 6");
			goto CommonExit;
		}
		sprintf(gcQuery,"ssh %s %s 'vzctl set %u --onboot yes --save'",
								cSSHOptions,cTargetNodeIPv4,uNewVeid);
		if(system(gcQuery))
		{
			logfileLine("CloneContainer",gcQuery);
			tJobErrorUpdate(uJob,"error 6b");
			goto CommonExit;
		}
		SetContainerStatus(uNewVeid,uACTIVE);
	}
	else
	{
		sprintf(gcQuery,"ssh %s %s 'vzctl set %u --onboot no --save'",
								cSSHOptions,cTargetNodeIPv4,uNewVeid);
		if(system(gcQuery))
		{
			logfileLine("CloneContainer",gcQuery);
			tJobErrorUpdate(uJob,"error 6c");
			goto CommonExit;
		}
		SetContainerStatus(uNewVeid,uSTOPPED);
	}

	//if(uDebug)
	//	return;

	//7-. 8-. Everything ok
	if(uPrevStatus!=uINITSETUP)
		//Not an auto clone job is only possibility but this maybe a hack. TODO
		SetContainerStatus(uContainer,uPrevStatus);
	tJobDoneUpdate(uJob);

	//9a-. local clean up
	if(!cSnapshotDir[0])
		sprintf(gcQuery,"rm /vz/dump/vzdump-%u.tgz",uContainer);
	else
		sprintf(gcQuery,"rm %s/vzdump-%u.tgz",cSnapshotDir,uContainer);
	if(system(gcQuery))
		logfileLine("CloneContainer",gcQuery);
	
	//9b-. remote clean up
	sprintf(gcQuery,"ssh %s %s 'rm /vz/dump/vzdump-%u.tgz'",cSSHOptions,cTargetNodeIPv4,uContainer);
	if(system(gcQuery))
		logfileLine("CloneContainer",gcQuery);
	sprintf(gcQuery,"ssh %s %s 'rm /vz/dump/vzdump-%u.tgz.md5sum'",cSSHOptions,cTargetNodeIPv4,uContainer);
	if(system(gcQuery))
		logfileLine("CloneContainer",gcQuery);

	//This breaks starting clones turn off for now until we can figure out how to migrate quotas correctly.
	//10-. If quota file exists copy also
	//sprintf(gcQuery,"scp /var/vzquota/quota.%u %s:/var/vzquota/quota.%u",uContainer,cTargetNodeIPv4,uNewVeid);
	//if(system(gcQuery))
	//	logfileLine("CloneContainer",gcQuery);

	//Run optional post clone script
	//10-.
	//Optional group based script may exist to be executed.
	//
	//Primary group is oldest tGroupGlue entry.
	//UBC safe
	sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
			" AND tProperty.uKey=tGroupGlue.uGroup"
			" AND tGroupGlue.uContainer=%u"
			" AND tProperty.cName='cJob_OnCloneScript' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",uPROP_GROUP,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("CloneContainer",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char cOnScriptCall[386];
		char cCommand[256];
		char *cp;

		sprintf(cCommand,"%.255s",field[0]);

		//Remove trailing junk
		if((cp=strchr(cCommand,'\n')) || (cp=strchr(cCommand,'\r'))) *cp=0;

		char cHostname[100]={""};
		sprintf(gcQuery,"SELECT tContainer.cHostname"
				" FROM tContainer WHERE uContainer=%u",uNewVeid);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("CloneContainer","mysql_error()");
			logfileLine("CloneContainer",mysql_error(&gMysql));
			goto CommonExit;
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(cHostname,"%.99s",field[0]);

			//Please note that any script here must not use uStatus=1 as a condition.
			//note no security checks for remote script
			sprintf(cOnScriptCall,"ssh %s %s '%.255s %.64s %u'",cSSHOptions,cTargetNodeIPv4,cCommand,cHostname,uNewVeid);
			if(system(cOnScriptCall))
			{
				logfileLine("CloneContainer",cOnScriptCall);
			}
		}
	}
	//end 10-. optional clone script

//This goto MIGHT be ok.
CommonExit:
	//9c-. remove lock file
	//vzdump script should handle this?
	unlink("/var/run/vzdump.lock");

	return;

}//void CloneContainer(...)


//Required by libtemplate
void AppFunctions(FILE *fp,char *cFunction)
{
}//void AppFunctions(FILE *fp,char *cFunction)


//OpenVZ action scripts: Creates all OpenVZ action scripts will rename when time permits.
int CreateActionScripts(unsigned uContainer, unsigned uOverwrite)
{
	//Create VEID.mount and VEID.umount files if container so specifies via tProperty
	//	and everything needed is available: template and template vars from tProperty.
        MYSQL_RES *res2;
        MYSQL_ROW field2;
	FILE *fp;
	char cTemplateName[256]={""};//required
	char cFile[100]={""};//required
	char cVeID[32]={""};//required
	char cService1[256]={"80"};//default
	char cService2[256]={"443"};//default
	char cExtraNodeIP[256]={""};//required by some templates not all TODO fix this mess
	char cNetmask[256]={"255.255.255.0"};//default
	char cPrivateIPs[256]={"10.0.0.0/24"};//default
	char cIPv4[32]={""};
	struct stat statInfo;

	sprintf(cVeID,"%u",uContainer);	
	if(GetContainerMainIP(uContainer,cIPv4))
	{
		logfileLine("CreateActionScripts","Empty cIPv4");
		return(1);
	}

	GetContainerProp(uContainer,"cExtraNodeIP",cExtraNodeIP);
	if(!cExtraNodeIP[0])
		//Allow backwards compatability
		GetContainerProp(uContainer,"cNodeIP",cExtraNodeIP);


	//First action script set
	GetContainerProp(uContainer,"cVEID.mount",cTemplateName);
	sprintf(cFile,"/etc/vz/conf/%u.mount",uContainer);
	//Do not overwrite existing mount files! Do that before if you really want to.
	//stat returns 0 if file exists
	if(cTemplateName[0] && (stat(cFile,&statInfo) || uOverwrite))
	{
		GetContainerProp(uContainer,"cNetmask",cNetmask);
		GetContainerProp(uContainer,"cPrivateIPs",cPrivateIPs);
		GetContainerProp(uContainer,"cService1",cService1);
		GetContainerProp(uContainer,"cService2",cService2);

		if((fp=fopen(cFile,"w"))==NULL)
		{
			logfileLine("CreateActionScripts-1",cFile);
			return(1);
		}

		TemplateSelect(cTemplateName);
		res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			struct t_template template;

			template.cpName[0]="cExtraNodeIP";
			template.cpValue[0]=cExtraNodeIP;
					
			template.cpName[1]="cNetmask";
			template.cpValue[1]=cNetmask;
					
			template.cpName[2]="cPrivateIPs";
			template.cpValue[2]=cPrivateIPs;
					
			template.cpName[3]="cVeID";
			template.cpValue[3]=cVeID;

			template.cpName[4]="cService1";
			template.cpValue[4]=cService1;
					
			template.cpName[5]="cService2";
			template.cpValue[5]=cService2;
					
			template.cpName[6]="cIPv4";
			template.cpValue[6]=cIPv4;
					
			template.cpName[7]="";
			Template(field2[0],&template,fp);
		}
		mysql_free_result(res2);
		fclose(fp);
		chmod(cFile,S_IRUSR|S_IWUSR|S_IXUSR);
	}

	cTemplateName[0]=0;
	GetContainerProp(uContainer,"cVEID.umount",cTemplateName);
	sprintf(cFile,"/etc/vz/conf/%u.umount",uContainer);
	if(cTemplateName[0] && (stat(cFile,&statInfo) || uOverwrite) )
	{
		if((fp=fopen(cFile,"w"))==NULL)
		{
			logfileLine("CreateActionScripts-2",cFile);
			return(1);
		}

		TemplateSelect(cTemplateName);
		res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			struct t_template template;

			template.cpName[0]="cExtraNodeIP";
			template.cpValue[0]=cExtraNodeIP;
						
			template.cpName[1]="cNetmask";
			template.cpValue[1]=cNetmask;
					
			template.cpName[2]="cPrivateIPs";
			template.cpValue[2]=cPrivateIPs;
					
			template.cpName[3]="cVeID";
			template.cpValue[3]=cVeID;

			template.cpName[4]="cService1";
			template.cpValue[4]=cService1;
					
			template.cpName[5]="cService2";
			template.cpValue[5]=cService2;
					
			template.cpName[6]="cIPv4";
			template.cpValue[6]=cIPv4;
				
			template.cpName[7]="";
			Template(field2[0],&template,fp);
		}
		mysql_free_result(res2);
		fclose(fp);
		chmod(cFile,S_IRUSR|S_IWUSR|S_IXUSR);
	}

	//Second action script set: Always a set of both start and stop? No!
	GetContainerProp(uContainer,"cVEID.start",cTemplateName);
	sprintf(cFile,"/etc/vz/conf/%u.start",uContainer);
	//Do not overwrite existing files! Do that before if you really want to.
	//stat returns 0 if file exists
	if(cTemplateName[0] && (stat(cFile,&statInfo) || uOverwrite))
	{
		if((fp=fopen(cFile,"w"))==NULL)
		{
			logfileLine("CreateActionScripts-3",cFile);
			return(1);
		}

		TemplateSelect(cTemplateName);
		res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			struct t_template template;

			template.cpName[0]="cIPv4";
			template.cpValue[0]=cIPv4;
					
			template.cpName[1]="";
			Template(field2[0],&template,fp);
		}
		mysql_free_result(res2);
		fclose(fp);
		chmod(cFile,S_IRUSR|S_IWUSR|S_IXUSR);
	}

	cTemplateName[0]=0;
	GetContainerProp(uContainer,"cVEID.stop",cTemplateName);
	sprintf(cFile,"/etc/vz/conf/%u.stop",uContainer);
	if(cTemplateName[0] && (stat(cFile,&statInfo) || uOverwrite) )
	{
		if((fp=fopen(cFile,"w"))==NULL)
		{
			logfileLine("CreateActionScripts-4",cFile);
			return(1);
		}

		TemplateSelect(cTemplateName);
		res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			struct t_template template;

			template.cpName[0]="cIPv4";
			template.cpValue[0]=cIPv4;
					
			template.cpName[1]="";
			Template(field2[0],&template,fp);

		}
		mysql_free_result(res2);
		fclose(fp);
		chmod(cFile,S_IRUSR|S_IWUSR|S_IXUSR);
	}

	return(0);

}//int CreateActionScripts()


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


void LocalImportTemplate(unsigned uJob,unsigned uDatacenter,const char *cJobData)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	char cOSTemplateFilePath[55]={"/vz/template/cache/"};
	char cOSTemplateFile[200]={""};
	char cOSTemplateFileMd5sum[200]={""};
	struct stat statInfo;
	unsigned uOSTemplate=0;

	//1-. Parse data and basic sanity checks
	sscanf(cJobData,"uOSTemplate=%u;",&uOSTemplate);
	if(!uOSTemplate)
	{
		logfileLine("LocalImportTemplate","Could not determine uOSTemplate");
		tJobErrorUpdate(uJob,"uOSTemplate=0");
		return;
	}

	sprintf(gcQuery,"SELECT cLabel FROM tOSTemplate"
			" WHERE uOSTemplate=%u",uOSTemplate);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("LocalImportTemplate",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tOSTemplate.cLabel");
		return;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cOSTemplateFile,"%.54s%.100s.tar.gz",cOSTemplateFilePath,field[0]);
	}
	else
	{
		mysql_free_result(res);
		logfileLine("LocalImportTemplate","Could not determine tOSTemplate.cLabel\n");
		tJobErrorUpdate(uJob,"tOSTemplate.cLabel");
		return;
	}
	mysql_free_result(res);
	if(uNotValidSystemCallArg(cOSTemplateFile))
	{
		logfileLine("LocalImportTemplate","security alert");
		tJobErrorUpdate(uJob,"failed sec alert!");
		return;
	}

	//2-. stat file
	if(stat(cOSTemplateFile,&statInfo)!=0)
	{
		logfileLine("LocalImportTemplate",cOSTemplateFile);
		tJobErrorUpdate(uJob,"cOSTemplateFile stat");
		return;
	}

	//3-. stat file.md5sum
	sprintf(cOSTemplateFileMd5sum,"%.154s.md5sum",cOSTemplateFile);
	if(stat(cOSTemplateFileMd5sum,&statInfo)!=0)
	{
		logfileLine("LocalImportTemplate",cOSTemplateFileMd5sum);
		tJobErrorUpdate(uJob,"cOSTemplateFilemd5sum stat");
		return;
	}

	//4-. check md5sum
	sprintf(gcQuery,"/usr/bin/md5sum -c %s > /dev/null 2>&1",cOSTemplateFileMd5sum);
	if(system(gcQuery))
	{
		logfileLine("LocalImportTemplate","md5sum -c failed");
		tJobErrorUpdate(uJob,"md5sum -c failed!");
		return;
	}


	//5-. copy to all same datacenter nodes nicely (hopefully on GB 2nd NIC internal lan.)
	char cSCPOptions[256]={""};
	GetConfiguration("cSCPOptions",cSCPOptions,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cSCPOptions[0])
	{
		GetConfiguration("cSCPOptions",cSCPOptions,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cSCPOptions[0])
			GetConfiguration("cSCPOptions",cSCPOptions,0,0,0,0);//Last try global
	}
	//Default for less conditions below
	//Configure normal default via /etc/ssh/ssh_config
	if(uNotValidSystemCallArg(cSCPOptions))
		sprintf(cSCPOptions,"-P 22 -c arcfour");
	sprintf(gcQuery,"SELECT cLabel FROM tNode WHERE uDatacenter=%u AND uNode!=%u",gfuDatacenter,gfuNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("LocalImportTemplate",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tNode.cLabel");
		return;
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"nice /usr/bin/scp %s %s %s:%s\n",cSCPOptions,cOSTemplateFile,field[0],cOSTemplateFile);
		if(system(gcQuery))
		{
			mysql_free_result(res);
			logfileLine("LocalImportTemplate",gcQuery);
			tJobErrorUpdate(uJob,"scp-1 failed!");
			return;
		}
		sprintf(gcQuery,"nice /usr/bin/scp %s %s %s:%s\n",cSCPOptions,cOSTemplateFileMd5sum,field[0],
						cOSTemplateFileMd5sum);
		if(system(gcQuery))
		{
			mysql_free_result(res);
			logfileLine("LocalImportTemplate",gcQuery);
			tJobErrorUpdate(uJob,"scp-1 failed!");
			return;
		}
	}
	mysql_free_result(res);

	//Everything ok
	tJobDoneUpdate(uJob);

}//void LocalImportTemplate(unsigned uJob,unsigned uContainer,const char *cJobData)


void LocalImportConfig(unsigned uJob,unsigned uDatacenter,const char *cJobData)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	char cConfigFilePath[55]={"/etc/vz/conf/"};
	char cConfigFile[200]={""};
	struct stat statInfo;
	unsigned uConfig=0;

	//1-. Parse data and basic sanity checks
	sscanf(cJobData,"uConfig=%u;",&uConfig);
	if(!uConfig)
	{
		logfileLine("LocalImportConfig","Could not determine uConfig");
		tJobErrorUpdate(uJob,"uConfig=0");
		return;
	}

	sprintf(gcQuery,"SELECT cLabel FROM tConfig"
			" WHERE uConfig=%u",uConfig);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("LocalImportConfig",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tConfig.cLabel");
		return;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cConfigFile,"%.54sve-%.100s.conf-sample",cConfigFilePath,field[0]);
	}
	else
	{
		mysql_free_result(res);
		logfileLine("LocalImportConfig","Could not determine tConfig.cLabel");
		tJobErrorUpdate(uJob,"tConfig.cLabel");
		return;
	}
	mysql_free_result(res);
	if(uNotValidSystemCallArg(cConfigFile))
	{
		logfileLine("LocalImportConfig","security alert");
		tJobErrorUpdate(uJob,"failed sec alert!");
		return;
	}

	//2-. stat file
	if(stat(cConfigFile,&statInfo)!=0)
	{
		logfileLine("LocalImportConfig",cConfigFile);
		tJobErrorUpdate(uJob,"cConfigFile stat");
		return;
	}

	//3-. copy to all same datacenter nodes nicely (hopefully on GB 2nd NIC internal lan.)
	char cSCPOptions[256]={""};
	GetConfiguration("cSCPOptions",cSCPOptions,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cSCPOptions[0])
	{
		GetConfiguration("cSCPOptions",cSCPOptions,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cSCPOptions[0])
			GetConfiguration("cSCPOptions",cSCPOptions,0,0,0,0);//Last try global
	}
	//Default for less conditions below
	//Configure normal default via /etc/ssh/ssh_config
	if(uNotValidSystemCallArg(cSCPOptions))
		sprintf(cSCPOptions,"-P 22 -c arcfour");
	sprintf(gcQuery,"SELECT cLabel FROM tNode WHERE uDatacenter=%u AND uNode!=%u",gfuDatacenter,gfuNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("LocalImportConfig",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tNode.cLabel");
		return;
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"nice /usr/bin/scp %s %s %s:%s\n",cSCPOptions,cConfigFile,field[0],cConfigFile);
		//debug only
		//printf("%s\n",gcQuery);
		if(system(gcQuery))
		{
			mysql_free_result(res);
			logfileLine("LocalImportConfig",gcQuery);
			tJobErrorUpdate(uJob,"scp failed!");
			return;
		}
	}
	mysql_free_result(res);

	//Everything ok
	tJobDoneUpdate(uJob);

}//void LocalImportConfig(unsigned uJob,unsigned uContainer,const char *cJobData)


void FailoverTo(unsigned uJob,unsigned uContainer,const char *cJobData)
{
	//Notes
	//Initial rollback has no error checking, just best effort.

	unsigned uRetVal=0;
	unsigned uStatus=0;//Real vzlist status
	unsigned uSourceContainer=0;
	char cIP[32]={""};
	char cLabel[32]={""};
	char cHostname[64]={""};
	//For rollback
	char cOrigIP[32]={""};
	char cOrigLabel[32]={""};
	char cOrigHostname[64]={""};
	
	//Get this cloned container source
	if(GetContainerSource(uContainer,&uSourceContainer))
	{
		logfileLine("FailoverTo","GetContainerSource()");
		tJobErrorUpdate(uJob,"No uSourceContainer");
		return;
	}
	//Now we quickly unset to avoid failed clon sync jobs
	if(SetContainerSource(uContainer,0))
	{
		logfileLine("FailoverTo","SetContainerSource()");
		tJobErrorUpdate(uJob,"SetContainerSource");
		return;
	}
	//Get data about container
	if(GetContainerNodeStatus(uContainer,&uStatus))
	{
		logfileLine("FailoverTo","GetContainerNodeStatus()");
		tJobErrorUpdate(uJob,"GetContainerNodeStatus");

		//rollback
		SetContainerSource(uContainer,uSourceContainer);
		//level 1 done
		return;
	}

	//1-.
	//If container is stopped must start. Since for unknown reasons
	//the container may already be running (vzctl returns 32 in the version we have at this time) 
	//we do not consider that start error to be fatal for now.
	if(uStatus==uSTOPPED)	
	{
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u ",uContainer);
		if((uRetVal=system(gcQuery)))
		{
			if(uRetVal==32)//32 is vzctl already running
			{
				logfileLine("FailoverTo",gcQuery);
			}
			else
			{
				logfileLine("FailoverTo",gcQuery);
				tJobErrorUpdate(uJob,"start error");

				//rollback
				SetContainerSource(uContainer,uSourceContainer);
				//level 1 done
				return;
			}
		}

		sprintf(gcQuery,"/usr/sbin/vzctl set %u --onboot yes --save",uContainer);
		if(system(gcQuery))
		{
			logfileLine("FailoverTo",gcQuery);
			tJobErrorUpdate(uJob,"set onboot failed");

			//rollback
			SetContainerSource(uContainer,uSourceContainer);
			//level 1 done
			return;
		}
	}
	SetContainerStatus(uContainer,uACTIVE);//rollback item

	//2-.
	//We remove all the previous IPs.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
	if(system(gcQuery))
	{
		logfileLine("FailoverTo",gcQuery);
		tJobErrorUpdate(uJob,"ipdel all");

		//rollback
		SetContainerSource(uContainer,uSourceContainer);
		//level 1 done
		if(uStatus==uSTOPPED)	
		{
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u ",uContainer);
			system(gcQuery);
		}
		SetContainerStatus(uContainer,uAWAITFAIL);
		//level 2 done
		return;
	}

	//3-.
	//We add (from the source container!) (any other IPs?) first the main IP
	GetContainerMainIP(uContainer,cOrigIP);//rollback required
	if((uRetVal=GetContainerMainIP(uSourceContainer,cIP)))
	{
		logfileLine("FailoverTo","GetContainerMainIP()");
		tJobErrorUpdate(uJob,"GetContainerMainIP");

		//rollback
		SetContainerSource(uContainer,uSourceContainer);
		//level 1 done
		if(uStatus==uSTOPPED)	
		{
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u ",uContainer);
			system(gcQuery);
		}
		SetContainerStatus(uContainer,uAWAITFAIL);
		//level 2 done
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
		system(gcQuery);
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
		system(gcQuery);
		//level 3 done
		return;
	}
	if(uNotValidSystemCallArg(cIP))
	{
		logfileLine("FailoverTo","security alert cIP");
		tJobErrorUpdate(uJob,"cIP security");

		//rollback
		SetContainerSource(uContainer,uSourceContainer);
		//level 1 done
		if(uStatus==uSTOPPED)	
		{
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u ",uContainer);
			system(gcQuery);
		}
		SetContainerStatus(uContainer,uAWAITFAIL);
		//level 2 done
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
		system(gcQuery);
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
		system(gcQuery);
		//level 3 done
	}
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cIP);
	if(system(gcQuery))
	{
		logfileLine("FailoverTo",gcQuery);
		tJobErrorUpdate(uJob,"ipadd cIP");

		//rollback
		SetContainerSource(uContainer,uSourceContainer);
		//level 1 done
		if(uStatus==uSTOPPED)	
		{
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u ",uContainer);
			system(gcQuery);
		}
		SetContainerStatus(uContainer,uAWAITFAIL);
		//level 2 done
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
		system(gcQuery);
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
		system(gcQuery);
		//level 3 done
	}
	//3b-.
	//No we can update the db for the new production container
	if(SetContainerIP(uContainer,cIP))
	{
		logfileLine("FailoverTo","SetContainerIP()");
		tJobErrorUpdate(uJob,"SetContainerIP");

		//rollback
		SetContainerSource(uContainer,uSourceContainer);
		//level 1 done
		if(uStatus==uSTOPPED)	
		{
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u ",uContainer);
			system(gcQuery);
			SetContainerStatus(uContainer,uAWAITFAIL);
		}
		SetContainerStatus(uContainer,uAWAITFAIL);
		//level 2 done
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
		system(gcQuery);
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
		system(gcQuery);
		//level 3 done
	}
	//3c-.
	//Change the names to the source ones
	if(GetContainerNames(uSourceContainer,cHostname,cLabel))
	{
		logfileLine("FailoverTo","GetContainerNames()");
		tJobErrorUpdate(uJob,"GetContainerNames");

		//rollback
		SetContainerSource(uContainer,uSourceContainer);
		//level 1 done
		if(uStatus==uSTOPPED)	
		{
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u ",uContainer);
			system(gcQuery);
		}
		SetContainerStatus(uContainer,uAWAITFAIL);
		//level 2 done
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
		system(gcQuery);
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
		system(gcQuery);
		//level 3 done
		SetContainerIP(uContainer,cOrigIP);
		//level 4 done
		return;
	}
	//This is needed until the sister job runs, due to unique index on cLabel,uDatacenter
	//We will also add the .fo cLabel to the "domain part" of cHostname.
	char cDomain[100];
	char cFOHostname[100];
	sscanf(cHostname,"%*[a-zA-Z0-9\\-].%s",cDomain);
	sprintf(cFOHostname,"%.28s.fo.%.67s",cLabel,cDomain);
	char cVEIDLabel[32];
	sprintf(cVEIDLabel,"%.28s.fo",cLabel);
	if(SetContainerHostname(uSourceContainer,cFOHostname,cVEIDLabel) || 
		SetContainerHostname(uContainer,cHostname,cLabel))
	{
		logfileLine("FailoverTo","SetContainerHostname()");
		tJobErrorUpdate(uJob,"SetContainerHostname");

		//rollback
		SetContainerSource(uContainer,uSourceContainer);
		//level 1 done
		if(uStatus==uSTOPPED)	
		{
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u ",uContainer);
			system(gcQuery);
		}
		SetContainerStatus(uContainer,uAWAITFAIL);
		//level 2 done
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
		system(gcQuery);
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
		system(gcQuery);
		//level 3 done
		SetContainerIP(uContainer,cOrigIP);
		//level 4 done
		return;
	}
	else
	{
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --hostname %s --name %s --save",
				uContainer,cHostname,cLabel);
		if(system(gcQuery))
		{
			logfileLine("FailoverTo",gcQuery);
			tJobErrorUpdate(uJob,"vzctl set hostname");

			//rollback
			SetContainerSource(uContainer,uSourceContainer);
			//level 1 done
			if(uStatus==uSTOPPED)	
			{
				sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u ",uContainer);
				system(gcQuery);
			}
			SetContainerStatus(uContainer,uAWAITFAIL);
			//level 2 done
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
			system(gcQuery);
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
			system(gcQuery);
			//level 3 done
			SetContainerIP(uContainer,cOrigIP);
			//level 4 done
			return;
		}
	}

	//4-.
	//When we clone we purposefully remove any action scripts
	//Here we must add them.
	//Trouble is that these scripts must allow for failover.
	//for example if strange firewall rules or IP ranges are used
	//these scripts will not be portable --even in the same datacenter!
	//It follows that all hardware nodes of the same type (VENET xor VETH)
	//must have the same basic firewall and device settings. HN device differences
	//can be added later via tNode properties.
	if(CreateActionScripts(uContainer,1))
	{
		logfileLine("FailoverTo","CreateActionScripts(x,1)");
		tJobErrorUpdate(uJob,"CreateActionScripts(x,1)");

		//rollback
		SetContainerSource(uContainer,uSourceContainer);
		//level 1 done
		if(uStatus==uSTOPPED)	
		{
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u ",uContainer);
			system(gcQuery);
		}
		SetContainerStatus(uContainer,uAWAITFAIL);
		//level 2 done
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
		system(gcQuery);
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
		system(gcQuery);
		//level 3 done
		SetContainerIP(uContainer,cOrigIP);
		//level 4 done
		GetContainerNames(uContainer,cOrigHostname,cOrigLabel);
		SetContainerHostname(uContainer,cOrigHostname,cOrigLabel);
		//level 5 done
		return;
	}

	//Attempt to get neighbors to refresh their arp table
	//	This adds 3 seconds to every job
	//arpsend -c 5 -U -i 65.49.53.159 eth0
	char cArpDevice[256]={""};
	GetConfiguration("cArpDevice",cArpDevice,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cArpDevice[0])
	{
		GetConfiguration("cArpDevice",cArpDevice,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cArpDevice[0])
			GetConfiguration("cArpDevice",cArpDevice,0,0,0,0);//Last try global
		else
			sprintf(cArpDevice,"eth0");//default eth0
	}
	sprintf(gcQuery,"/usr/sbin/arpsend -c 3 -U -i %.15s %.15s",cIP,cArpDevice);
	system(gcQuery);

	//Everything ok
	tJobDoneUpdate(uJob);

}//void FailoverTo()


void FailoverFrom(unsigned uJob,unsigned uContainer,const char *cJobData)
{
	//These are the clone container's
	unsigned uSource=0;
	unsigned uFailToJob=0;
	unsigned uStatus=0;
	unsigned uIPv4=0;
	char cIP[32]={""};
	char cLabel[32]={""};
	char cHostname[64]={""};
	char *cp,*cp2;
	//For rollback
	char cOrigIP[32]={""};
	char cOrigLabel[32]={""};
	char cOrigHostname[64]={""};

	unsigned uDebug=0;

	if(uDebug)
		printf("DEBUG FailoverFrom() uJob=%u uContainer=%u\n",uJob,uContainer);


	//0-. uCloneContainer not used yet.
	if((cp=strstr(cJobData,"uIPv4=")))
	{
		sscanf(cp+6,"%u",&uIPv4);
	}
	if(!uIPv4)
	{
		logfileLine("FailoverFrom","no uIPv4");
		tJobErrorUpdate(uJob,"no uIPv4");
		return;
	}

	if((cp=strstr(cJobData,"cLabel=")))
	{
		if((cp2=strchr(cp+7,';')))
		{
			*cp2=0;
			sprintf(cLabel,"%.31s",cp+7);
			*cp2=';';
		}
		
	}
	if(!cLabel[0])
	{
		logfileLine("FailoverFrom","no cLabel");
		tJobErrorUpdate(uJob,"no cLabel");
		return;
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
	if(!cHostname[0])
	{
		logfileLine("FailoverFrom","no cHostname");
		tJobErrorUpdate(uJob,"no cHostname");
		return;
	}

	if((cp=strstr(cJobData,"uSource=")))
	{
		sscanf(cp+8,"%u",&uSource);
	}
	if(!uSource)
	{
		logfileLine("FailoverFrom","no uSource");
		tJobErrorUpdate(uJob,"no uSource");
		return;
	}

	if((cp=strstr(cJobData,"uStatus=")))
	{
		sscanf(cp+8,"%u",&uStatus);
	}
	if(!uStatus)
	{
		logfileLine("FailoverFrom","no uStatus");
		tJobErrorUpdate(uJob,"no uStatus");
		return;
	}

	if((cp=strstr(cJobData,"uFailToJob=")))
	{
		sscanf(cp+11,"%u",&uFailToJob);
	}
	if(!uFailToJob)
	{
		logfileLine("FailoverFrom","no uFailToJob");
		tJobErrorUpdate(uJob,"no uFailToJob");
		return;
	}

	//Does this job have to run AFTER FailoverTo()? yes!
	//So if it hasn't we contine to wait.
	if(!FailToJobDone(uFailToJob))
	{
		char cuFailToJob[64];
		sprintf(cuFailToJob,"uFailToJob=%u",uFailToJob);
		logfileLine("FailoverFrom",cuFailToJob);
		tJobWaitingUpdate(uJob);
		return;
	}


	if(uDebug)
		printf("DEBUG FailoverFrom() cJobData: uIPv4=%u cLabel=%s cHostname=%s uSource=%u\n",
				uIPv4,cLabel,cHostname,uSource);

	//1-.
	GetContainerMainIP(uContainer,cOrigIP);//rollback required
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
	if(system(gcQuery))
	{
		logfileLine("FailoverFrom",gcQuery);
		tJobErrorUpdate(uJob,"ipdel all");
		return;
	}


	//2-.
	//If the clone was running we should also keep the new clone running also
	//If not we keep in some state that is assumed to be uACTIVE from the way the
	//backend creates these jobs.
	if(uStatus==uSTOPPED)
	{
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u",uContainer);
		if(system(gcQuery))
		{
			logfileLine("FailoverFrom",gcQuery);

			//check again for some reason, system returns non zero but the stop
			//actually takes place
			sprintf(gcQuery,"sleep 10;/usr/sbin/vzlist -a %u | grep stopped",uContainer);
			if(system(gcQuery))
			{
				//rollback
				tJobErrorUpdate(uJob,"vzctl stop fail");
				sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
				system(gcQuery);
				//level 1 done
				return;
			}
			tJobErrorUpdate(uJob,"vzctl stop then ok");
		}

		//If we stop we keep stopped on reboot.
		sprintf(gcQuery,"/usr/sbin/vzctl set %u --onboot no --save",uContainer);
		system(gcQuery);

		SetContainerStatus(uContainer,uSTOPPED);
	}
	else
	{
		SetContainerStatus(uContainer,uACTIVE);
	}

	//Everything ok as far as failover goes, now we do housekeeping.

	//3-.
	//Change the names to the cloned ones
	GetContainerNames(uContainer,cOrigHostname,cOrigLabel);
	if(SetContainerHostname(uContainer,cHostname,cLabel))
	{
		logfileLine("FailoverFrom","SetContainerHostname()");
		tJobErrorUpdate(uJob,"SetContainerHostname");

		//rollback
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
		system(gcQuery);
		//level 1 done
		SetContainerStatus(uContainer,uAWAITFAIL);
		if(uStatus==uSTOPPED)
		{
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
			system(gcQuery);
			sprintf(gcQuery,"/usr/sbin/vzctl set %u --onboot yes --save",uContainer);
			system(gcQuery);
		}
		//level 2 done
		return;
	}
	else
	{
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --hostname %s --name %s --save",
				uContainer,cHostname,cLabel);
		if(system(gcQuery))
		{
			logfileLine("FailoverFrom",gcQuery);
			tJobErrorUpdate(uJob,"vzctl set hostname");

			//rollback
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
			system(gcQuery);
			//level 1 done
			SetContainerStatus(uContainer,uAWAITFAIL);
			if(uStatus==uSTOPPED)
			{
				sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
				system(gcQuery);
				sprintf(gcQuery,"/usr/sbin/vzctl set %u --onboot yes --save",uContainer);
				system(gcQuery);
			}
			//level 2 done
			SetContainerHostname(uContainer,cOrigHostname,cOrigLabel);
			//level 3 done
			return;
		}
	}


	//4-.
	//Change IP over to the one the clone used to have
	GetIPFromtIP(uIPv4,cIP);
	if(SetContainerIP(uContainer,cIP))
	{
		logfileLine("FailoverFrom","SetContainerIP()");
		tJobErrorUpdate(uJob,"SetContainerIP");

		//rollback
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
		system(gcQuery);
		//level 1 done
		SetContainerStatus(uContainer,uAWAITFAIL);
		if(uStatus==uSTOPPED)
		{
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
			system(gcQuery);
			sprintf(gcQuery,"/usr/sbin/vzctl set %u --onboot yes --save",uContainer);
			system(gcQuery);
		}
		//level 2 done
		SetContainerHostname(uContainer,cOrigHostname,cOrigLabel);
		//level 3 done
		return;
	}
	else
	{
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cIP);
		if(system(gcQuery))
		{
			logfileLine("FailoverFrom",gcQuery);
			tJobErrorUpdate(uJob,"ipadd");

			//rollback
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
			system(gcQuery);
			//level 1 done
			SetContainerStatus(uContainer,uAWAITFAIL);
			if(uStatus==uSTOPPED)
			{
				sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
				system(gcQuery);
				sprintf(gcQuery,"/usr/sbin/vzctl set %u --onboot yes --save",uContainer);
				system(gcQuery);
			}
			//level 2 done
			SetContainerHostname(uContainer,cOrigHostname,cOrigLabel);
			//level 3 done
			SetContainerIP(uContainer,cOrigIP);
			//level 4 done
			return;

		}
	}


	//5-. Remove any action script files.
	sprintf(gcQuery,"rm -f /etc/vz/conf/%1$u.umount /etc/vz/conf/%1$u.mount"
				" /etc/vz/conf/%1$u.start /etc/vz/conf/%1$u.stop"
					,uContainer);
	if(system(gcQuery))
	{
		logfileLine("FailoverFrom",gcQuery);
		tJobErrorUpdate(uJob,"rm action scripts");

		//rollback
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
		system(gcQuery);
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
		system(gcQuery);
		//level 1 done
		SetContainerStatus(uContainer,uAWAITFAIL);
		if(uStatus==uSTOPPED)
		{
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
			system(gcQuery);
			sprintf(gcQuery,"/usr/sbin/vzctl set %u --onboot yes --save",uContainer);
			system(gcQuery);
		}
		//level 2 done
		SetContainerHostname(uContainer,cOrigHostname,cOrigLabel);
		//level 3 done
		SetContainerIP(uContainer,cOrigIP);
		//level 4 done
		//level 5 done see expanded level 1 done
		return;
	}

	//This is relatively safe since a cuSyncPeriod non 0 has to exist for this container.
	//The issues identified so far are:
	//A-. What if the new production container does not work and the old source has
	//	important data that could fix this problem quickly?
	if(SetContainerSource(uContainer,uSource))
	{
		logfileLine("FailoverFrom","SetContainerSource()");
		tJobErrorUpdate(uJob,"SetContainerSource");

		//rollback
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
		system(gcQuery);
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
		system(gcQuery);
		//level 1 done
		SetContainerStatus(uContainer,uAWAITFAIL);
		if(uStatus==uSTOPPED)
		{
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
			system(gcQuery);
			sprintf(gcQuery,"/usr/sbin/vzctl set %u --onboot yes --save",uContainer);
			system(gcQuery);
		}
		//level 2 done
		SetContainerHostname(uContainer,cOrigHostname,cOrigLabel);
		//level 3 done
		SetContainerIP(uContainer,cOrigIP);
		//level 4 done
		//level 5 done see expanded level 1
		return;
	}
	//To play it safe for now we remove any cuSyncPeriod property.
	//This means that once the node is operational and we want to resume
	//clone sync operations we need to copy the source container cuSyncPeriod
	//to the clone container for sync operation to resume.
	//UBC safe	
	SetContainerProperty(uContainer,"cuSyncPeriod","0");

	//Everything ok
	tJobDoneUpdate(uJob);

}//void FailoverFrom()


unsigned GetContainerStatus(const unsigned uContainer, unsigned *uStatus)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0) return(1);

	sprintf(gcQuery,"SELECT uStatus FROM tContainer WHERE uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("GetContainerStatus",mysql_error(&gMysql));
		return(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",uStatus);
		if(!uStatus) return(3);
	}
	else
	{
		mysql_free_result(res);
		return(4);
	}
	mysql_free_result(res);
	return(0);

}//unsigned GetContainerStatus()


unsigned GetContainerMainIP(const unsigned uContainer,char *cIP)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0) return(1);

	sprintf(gcQuery,"SELECT tIP.cLabel FROM tContainer,tIP WHERE tIP.uIP=tContainer.uIPv4 AND"
				" tContainer.uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("GetContainerMainIP",mysql_error(&gMysql));
		return(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cIP,"%.31s",field[0]);
		if(!cIP[0]) return(3);
	}
	else
	{
		mysql_free_result(res);
		return(4);
	}
	mysql_free_result(res);
	return(0);

}//void GetContainerMainIP()


unsigned GetContainerSource(const unsigned uContainer, unsigned *uSource)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0) return(1);

	sprintf(gcQuery,"SELECT uSource FROM tContainer WHERE uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("GetContainerSource",mysql_error(&gMysql));
		return(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",uSource);
		if(!uSource) return(3);
	}
	else
	{
		mysql_free_result(res);
		return(4);
	}
	mysql_free_result(res);
	return(0);

}//void GetContainerSource()


unsigned SetContainerIP(const unsigned uContainer,char *cIP)
{
	if(uContainer==0 || cIP[0]==0)
		return(1);

	sprintf(gcQuery,"UPDATE tContainer SET uIPv4=(SELECT uIP FROM tIP WHERE cLabel='%s' LIMIT 1) WHERE"
				" uContainer=%u",cIP,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("SetContainerIP",mysql_error(&gMysql));
		return(2);
	}
	return(0);

}//void SetContainerIP()


unsigned SetContainerSource(const unsigned uContainer,const unsigned uSource)
{
	if(uContainer==0) return(1);

	sprintf(gcQuery,"UPDATE tContainer SET uSource=%u WHERE uContainer=%u",uSource,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("SetContainerSource",mysql_error(&gMysql));
		return(2);
	}
	return(0);

}//void SetContainerSource()


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


unsigned SetContainerHostname(const unsigned uContainer,
			const char *cHostname,const char *cLabel)
{
	if(uContainer==0) return(1);

	sprintf(gcQuery,"UPDATE tContainer SET cHostname='%s',cLabel='%s' WHERE uContainer=%u",
		cHostname,cLabel,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("SetContainerHostname",mysql_error(&gMysql));
		return(2);
	}
	return(0);

}//void SetContainerHostname()


unsigned GetContainerNames(const unsigned uContainer,char *cHostname,char *cLabel)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0) return(1);

	sprintf(gcQuery,"SELECT cLabel,cHostname FROM tContainer WHERE uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("GetContainerNames",mysql_error(&gMysql));
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cLabel,"%.31s",field[0]);
		sprintf(cHostname,"%.63s",field[1]);
		if(!cLabel[0]) return(2);
		if(!cHostname[0]) return(3);
	}
	else
	{
		mysql_free_result(res);
		return(4);
	}
	mysql_free_result(res);

	return(0);

}//unsigned GetContainerNames()


unsigned GetContainerNodeStatus(const unsigned uContainer, unsigned *uStatus)
{
	//Notes
	//This is the real running or stopped status on the node this runs on
	//for the given container.

	//first check
	sprintf(gcQuery,"/usr/sbin/vzlist --no-header %u > /dev/null 2>&1",uContainer);
	if(system(gcQuery))
		return(1);

	//now get status
	sprintf(gcQuery,"/usr/sbin/vzlist --no-header %u | /bin/grep running > /dev/null 2>&1",
										uContainer);
	if(system(gcQuery))
		*uStatus=uSTOPPED;
	else
		*uStatus=uACTIVE;

	return(0);

}//void GetContainerNodeStatus()


//UBC safe
unsigned SetContainerPropertyUBC(const unsigned uContainer,const char *cPropertyName,const  char *cPropertyValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0 || cPropertyName[0]==0 || cPropertyValue[0]==0)
		return(1);

	unsigned uDatacenter=0;
	sscanf(ForeignKey("tContainer","uDatacenter",uContainer),"%u",&uDatacenter);
	if(!uDatacenter)
	{
		logfileLine("SetContainerPropertyUBC","!uDatacenter error");
		return(1);
	}
	if(ConnectToOptionalUBCDb(uDatacenter,1))
	{
		logfileLine("SetContainerPropertyUBC","ConnectToOptionalUBCDb error");
		return(1);
	}

	//UBC safe
	sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uType=3 AND uKey=%u AND cName='%s'",
					uContainer,cPropertyName);
	mysql_query(&gMysqlUBC,gcQuery);
	if(mysql_errno(&gMysqlUBC))
	{
		logfileLine("SetContainerPropertyUBC",mysql_error(&gMysqlUBC));
		return(2);
	}
        res=mysql_store_result(&gMysqlUBC);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tProperty SET cValue='%s' WHERE uProperty=%s",
					cPropertyValue,field[0]);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
		{
			mysql_free_result(res);
			logfileLine("SetContainerPropertyUBC",mysql_error(&gMysqlUBC));
			return(3);
		}
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tProperty SET cName='%s',cValue='%s',uType=3,uKey=%u,"
				"uOwner=(SELECT uOwner FROM tContainer WHERE uContainer=%u),"
				"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					cPropertyName,cPropertyValue,uContainer,uContainer);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
		{
			mysql_free_result(res);
			logfileLine("SetContainerPropertyUBC",mysql_error(&gMysqlUBC));
			return(4);
		}
	}
	mysql_free_result(res);

	return(0);

}//void SetContainerPropertyUBC()


//Not UBC safe use above function
unsigned SetContainerProperty(const unsigned uContainer,const char *cPropertyName,const  char *cPropertyValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0 || cPropertyName[0]==0 || cPropertyValue[0]==0)
		return(1);

	sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uType=3 AND uKey=%u AND cName='%s'",
					uContainer,cPropertyName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("SetContainerProperty",mysql_error(&gMysql));
		return(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tProperty SET cValue='%s' WHERE uProperty=%s",
					cPropertyValue,field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			mysql_free_result(res);
			logfileLine("SetContainerProperty",mysql_error(&gMysql));
			return(3);
		}
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tProperty SET cName='%s',cValue='%s',uType=3,uKey=%u,"
				"uOwner=(SELECT uOwner FROM tContainer WHERE uContainer=%u),"
				"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					cPropertyName,cPropertyValue,uContainer,uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			mysql_free_result(res);
			logfileLine("SetContainerProperty",mysql_error(&gMysql));
			return(4);
		}
	}
	mysql_free_result(res);

	return(0);

}//void SetContainerProperty()


unsigned FailToJobDone(unsigned uJob)
{
        MYSQL_RES *res;

	if(uJob==0)
	{
		logfileLine("FailToJobDone","uJob==0");
		return(0);
	}

	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uJob=%u AND uJobStatus=3",uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("FailToJobDone",mysql_error(&gMysql));
		return(0);
	}
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
		mysql_free_result(res);
		return(1);
	}
	mysql_free_result(res);
	return(0);

}//unsigned FailToJobDone(unsigned uContainer)


//Not UBC safe. Use function below
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uNode==0) return;

	//2 is node
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=2 AND cName='%s'",
				uNode,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("GetNodeProp",mysql_error(&gMysql));
		return;
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

}//void GetNodeProp()


//UBC safe
void GetNodePropUBC(const unsigned uNode,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uNode==0) return;

	unsigned uDatacenter=0;
	sscanf(ForeignKey("tNode","uDatacenter",uNode),"%u",&uDatacenter);
	if(!uDatacenter)
	{
		logfileLine("GetNodePropUBC","!uDatacenter error");
		return;
	}
	if(ConnectToOptionalUBCDb(uDatacenter,1))
	{
		logfileLine("GetNodePropUBC","ConnectToOptionalUBCDb error");
		return;
	}
	//2 is node
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=2 AND cName='%s'",
				uNode,cName);
	mysql_query(&gMysqlUBC,gcQuery);
	if(mysql_errno(&gMysqlUBC))
	{
		logfileLine("GetNodePropUBC",mysql_error(&gMysqlUBC));
		return;
	}
        res=mysql_store_result(&gMysqlUBC);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		if((cp=strchr(field[0],'\n')))
			*cp=0;
		sprintf(cValue,"%.255s",field[0]);
	}
	mysql_free_result(res);

}//void GetNodePropUBC()


//UBC safe
void GetDatacenterProp(const unsigned uDatacenter,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uDatacenter==0) return;

	//1 is datacenter
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=1 AND cName='%s'",
				uDatacenter,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(gLfp!=NULL)
		{
			logfileLine("GetDatacenterProp",mysql_error(&gMysql));
			exit(2);
		}
		else
		{
			htmlPlainTextError(mysql_error(&gMysql));
		}
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

}//void GetDatacenterProp()


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

	if(gLfp==NULL)
		gLfp=stdout;
        fprintf(gLfp,"%s jobqueue.%s[%u]: %s\n",cTime,cFunction,pidThis,cLogline);
	fflush(gLfp);

}//void logfileLine(char *cLogline)


unsigned ProcessCloneSyncJob(unsigned uNode,unsigned uContainer,unsigned uCloneContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uPeriod=0;

	if(guDebug)
	{
		sprintf(gcQuery,"Start uContainer=%u,uCloneContainer=%u",uContainer,uCloneContainer);
		logfileLine("ProcessCloneSyncJob",gcQuery);
	}

	//After failover cuSyncPeriod is 0 for clone (remote) container.
	//This allows for safekeeping of potentially useful data.
	//mainfunc RecoverMode recover cuSyncPeriod from source to clone.
	//UBC safe
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=3 AND cName='cuSyncPeriod'",
					uCloneContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessCloneSyncJob",mysql_error(&gMysql));
		return(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uPeriod);
	mysql_free_result(res);

	if(uPeriod!=0)
	{
		if(guDebug)
		{
			sprintf(gcQuery,"uPeriod=%u",uPeriod);
			logfileLine("ProcessCloneSyncJob",gcQuery);
		}

		sprintf(gcQuery,"SELECT tNode.cLabel,tNode.uDatacenter,tContainer.uStatus FROM tContainer,tNode WHERE"
				" tContainer.uNode=tNode.uNode AND"
				" tNode.uStatus=1 AND"//Active NODE
				" tContainer.uContainer=%u AND"
				" tContainer.uBackupDate+%u<=UNIX_TIMESTAMP(NOW())",uCloneContainer,uPeriod);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessCloneSyncJob",mysql_error(&gMysql));
			return(3);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			unsigned uCloneDatacenter=0;
			unsigned uCloneStatus=0;
        		MYSQL_RES *res2;
			MYSQL_ROW field2;

			sscanf(field[1],"%u",&uCloneDatacenter);
			sscanf(field[2],"%u",&uCloneStatus);

			//start of scheduler block
			//If remote datacenter check for schedule limitations of backup traffic
			if(uCloneDatacenter && uCloneDatacenter!=gfuDatacenter)
			{
				unsigned uCount=0;//should be two items
				unsigned uCloneScheduleStart=0;//0hrs to 24hrs
				unsigned uCloneScheduleWindow=0;//number of hours to add to above

				//Get backup schedule	
				//UBC safe
				sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
					" AND tProperty.uKey=tGroupGlue.uGroup"
					" AND tGroupGlue.uContainer=%u"
					" AND tProperty.cName='cJob_RemoteCloneSchedule' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",
						uPROP_GROUP,uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("ProcessCloneSyncJob",mysql_error(&gMysql));
					return(8);
				}
		        	res2=mysql_store_result(&gMysql);
				if((field2=mysql_fetch_row(res2)))
				{
					uCount=sscanf(field2[0],"From %uHS for %uHS",&uCloneScheduleStart,&uCloneScheduleWindow);
					//debug only
					logfileLine("ProcessCloneSyncJob cJob_RemoteCloneSchedule",field2[0]);

					//We have a schedule limit
					if(uCount==2)
					{
						//Get localtime hours
						time_t luClock;
						struct tm structTm;

						time(&luClock);
						localtime_r(&luClock,&structTm);

						//debug only
						char cMsg[256];
						//sprintf(cMsg,"uCloneScheduleStart=%u uCloneScheduleWindow=%u structTm.tm_hour=%u %u %u",
						//	uCloneScheduleStart,uCloneScheduleWindow,structTm.tm_hour,uContainer,uCloneContainer);
						//logfileLine("ProcessCloneSyncJob",cMsg);

						//Are we in the schedule window?
						if(structTm.tm_hour>=uCloneScheduleStart && structTm.tm_hour < (uCloneScheduleStart+uCloneScheduleWindow))
						{
							logfileLine("ProcessCloneSyncJob","in schedule window continue");
						}
						else
						{
							//here we randomly place next backup time in the schedule
							(void)srand((int)time((time_t *)NULL));
							//We need a random number of secs between uCloneScheduleStart and 
							//based hour seconds uCloneScheduleStart+uCloneScheduleWindow
							long unsigned uNewBackupSecs=(uCloneScheduleStart*3600)+(unsigned)((rand()/(RAND_MAX +1.0))*
								(((uCloneScheduleStart+uCloneScheduleWindow)*3600)-(uCloneScheduleStart*3600)+1));
							//debug only
							//sprintf(cMsg,"Start=%u End=%u uNewBackupSecs=%lu",uCloneScheduleStart*3600,
							//		(uCloneScheduleStart+uCloneScheduleWindow)*3600,uNewBackupSecs);
							//logfileLine("ProcessCloneSyncJob",cMsg);

							//Make it fall during schedule
							uNewBackupSecs+=luClock-(uPeriod+(structTm.tm_hour*3600)+(structTm.tm_min*60)+structTm.tm_sec);
							sprintf(gcQuery,"UPDATE tContainer SET uBackupDate=%lu"
								" WHERE uContainer=%u",uNewBackupSecs,uCloneContainer);
							mysql_query(&gMysql,gcQuery);
							if(mysql_errno(&gMysql))
							{
								mysql_free_result(res);
								logfileLine("ProcessCloneSyncJob",mysql_error(&gMysql));
								return(15);
							}
							mysql_free_result(res2);
							sprintf(cMsg,"schedule adjusted for %u",uCloneContainer);
							logfileLine("ProcessCloneSyncJob",cMsg);
							return(0);
						}
					}
					else
					{
						logfileLine("ProcessCloneSyncJob","cJob_RemoteCloneSchedule format error");
					}
				}
				mysql_free_result(res2);
			}
			//end of scheduler block

			if(uNotValidSystemCallArg(field[0]))
			{
				mysql_free_result(res);
				logfileLine("ProcessCloneSyncJob","security alert");
				return(4);
			}

			//Try to keep options out of scripts
			char cSSHOptions[256]={""};
			GetConfiguration("cSSHOptions",cSSHOptions,gfuDatacenter,gfuNode,0,0);//First try node specific
			if(!cSSHOptions[0])
			{
				GetConfiguration("cSSHOptions",cSSHOptions,gfuDatacenter,0,0,0);//Second try datacenter wide
				if(!cSSHOptions[0])
					GetConfiguration("cSSHOptions",cSSHOptions,0,0,0,0);//Last try global
			}
			//Default for less conditions below
			if(uNotValidSystemCallArg(cSSHOptions))
				sprintf(cSSHOptions,"-p 22 -c arcfour");
			unsigned uSSHPort=22;
			char *cp;
			if((cp=strstr(cSSHOptions,"-p ")))
				sscanf(cp+3,"%u",&uSSHPort);

			//New uBackupDate
			unsigned uLastBackupDate=0;
			sprintf(gcQuery,"SELECT uBackupDate FROM tContainer"
						" WHERE uContainer=%u",uCloneContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				mysql_free_result(res);
				logfileLine("ProcessCloneSyncJob",mysql_error(&gMysql));
				return(5);
			}
			res2=mysql_store_result(&gMysql);
			if((field2=mysql_fetch_row(res2)))
				sscanf(field2[0],"%u",&uLastBackupDate);
			mysql_free_result(res2);
			sprintf(gcQuery,"UPDATE tContainer SET uBackupDate=UNIX_TIMESTAMP(NOW())"
						" WHERE uContainer=%u",uCloneContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				mysql_free_result(res);
				logfileLine("ProcessCloneSyncJob",mysql_error(&gMysql));
				return(5);
			}


			//Here we should run a script based on container primary group
			//
			//Primary group is oldest tGroupGlue entry.
			char *cActivePostfix="";
			char cOnScriptCall[512];
			char cCommand[256]="/usr/sbin/clonesync.sh";//default non active
			if(uCloneStatus==uACTIVE)
			{
				sprintf(cCommand,"/usr/sbin/clonesync-active.sh");//default active
				cActivePostfix="Active";
			}
			//optional if so configured via tContainer primary group (lowest uGroupGlue)
			//UBC safe
			sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
					" AND tProperty.uKey=tGroupGlue.uGroup"
					" AND tGroupGlue.uContainer=%u"
					" AND tProperty.cName='cJob_CloneSyncScript%s' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",
						uPROP_GROUP,uContainer,cActivePostfix);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessCloneSyncJob",mysql_error(&gMysql));
				return(8);
			}
		        res2=mysql_store_result(&gMysql);
			if((field2=mysql_fetch_row(res2)))
			{
				struct stat statInfo;
				char *cp;

				sprintf(cCommand,"%.255s",field2[0]);
				mysql_free_result(res2);
				//Remove trailing junk
				if((cp=strchr(cCommand,'\n')) || (cp=strchr(cCommand,'\r'))) *cp=0;

				if(uNotValidSystemCallArg(cCommand))
				{
					logfileLine("ProcessCloneSyncJob","cJob_CloneSyncScript* security alert");
					return(9);
				}
		
				//Only run if command is chmod 500 and owned by root for extra security reasons.
				if(stat(cCommand,&statInfo))
				{
					logfileLine("ProcessCloneSyncJob","stat failed for cJob_CloneSyncScript*");
					logfileLine("ProcessCloneSyncJob",cCommand);
					return(10);
				}
				if(statInfo.st_uid!=0)
				{
					logfileLine("ProcessCloneSyncJob","cJob_CloneSyncScript* is not owned by root");
					return(11);
				}
				if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
				{
					logfileLine("ProcessCloneSyncJob","cJob_CloneSyncScript* is not chmod 500");
					return(12);
				}
			}
			else
			{
				mysql_free_result(res2);
			}

			//Get optional --bwlimit value for sync script
			//New optional parameter, configured per container group cJob_CloneSyncRemoteBW*
			//	 where * is empty str or "Active"
			unsigned uBWLimit=0;//0 is no limit

			if(uCloneDatacenter && uCloneDatacenter!=gfuDatacenter)
			{
				//UBC safe
				sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
					" AND tProperty.uKey=tGroupGlue.uGroup"
					" AND tGroupGlue.uContainer=%u"
					" AND tProperty.cName='cJob_CloneSyncRemoteBW%s' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",
						uPROP_GROUP,uContainer,cActivePostfix);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("ProcessCloneSyncJob",mysql_error(&gMysql));
					return(8);
				}
		        	res2=mysql_store_result(&gMysql);
				if((field2=mysql_fetch_row(res2)))
				{
					sscanf(field2[0],"%u",&uBWLimit);
					//debug only
					logfileLine("ProcessCloneSyncJob uBWLimit",field2[0]);
				}
				mysql_free_result(res2);
			}

			sprintf(cOnScriptCall,"%.255s %u %u %s %u %u",cCommand,uContainer,uCloneContainer,field[0],uSSHPort,uBWLimit);

			if(system(cOnScriptCall))
			{
				mysql_free_result(res);
				//Mark clone sync script error by not updating backup date.
				sprintf(gcQuery,"UPDATE tContainer SET uBackupDate=%u"
						" WHERE uContainer=%u",uLastBackupDate,uCloneContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					mysql_free_result(res);
					logfileLine("ProcessCloneSyncJob",mysql_error(&gMysql));
					return(7);
				}
				logfileLine("ProcessCloneSyncJob","clone sync script error");
				logfileLine("ProcessCloneSyncJob",cOnScriptCall);
				return(6);
			}
			else
			{
				if(guDebug)
				//if(1)
				{
					logfileLine("ProcessCloneSyncJob","clone sync script ok");
					logfileLine("ProcessCloneSyncJob",cOnScriptCall);
				}
			}
		}
		mysql_free_result(res);
	}
	if(guDebug)
		logfileLine("ProcessCloneSyncJob","End");
	return(0);

}//void ProcessCloneSyncJob()


void LogError(char *cErrorMsg,unsigned uKey)
{
	sprintf(gcQuery,"INSERT INTO tLog SET"
		" cLabel='unxsVZ CLI Error',"
		"uLogType=4,uLoginClient=1,"
		"cLogin='unxsVZ.cgi',cMessage=\"%s uKey=%u\","
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


void RecurringJob(unsigned uJob,unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData)
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

	//UBC safe
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

}//void RecurringJob(uJob,uDatacenter,uNode,uContainer,cJobData)


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


unsigned ProcessOSDeltaSyncJob(unsigned uNode,unsigned uContainer,unsigned uCloneContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uPeriod=0;
	char cSyncMode[32]={""};

	//UBC safe
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=3 AND cName='cSyncMode'",
					uCloneContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessOSDeltaSyncJob",mysql_error(&gMysql));
		return(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cSyncMode,"%.31s",field[0]);
	mysql_free_result(res);

	if(guDebug)
	{
		sprintf(gcQuery,"Start uContainer=%u,uCloneContainer=%u,cSyncMode=%s",uContainer,uCloneContainer,cSyncMode);
		logfileLine("ProcessOSDeltaSyncJob",gcQuery);
	}

	if(cSyncMode[0]==0 || cSyncMode[0]!='C')
		return(0);

	//After failover cuSyncPeriod is 0 for clone (remote) container.
	//This allows for safekeeping of potentially useful data.
	//mainfunc RecoverMode recover cuSyncPeriod from source to clone.
	//UBC safe
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=3 AND cName='cuSyncPeriod'",
					uCloneContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessOSDeltaSyncJob",mysql_error(&gMysql));
		return(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uPeriod);
	mysql_free_result(res);

	if(uPeriod!=0)
	{
		if(guDebug)
		{
			sprintf(gcQuery,"uPeriod=%u",uPeriod);
			logfileLine("ProcessOSDeltaSyncJob",gcQuery);
		}

		sprintf(gcQuery,"SELECT tNode.cLabel FROM tContainer,tNode WHERE"
				" tContainer.uNode=tNode.uNode AND"
				" tNode.uStatus=1 AND"//Active NODE
				" tContainer.uContainer=%u AND"
				" tContainer.uBackupDate+%u<=UNIX_TIMESTAMP(NOW())",uCloneContainer,uPeriod);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessOSDeltaSyncJob",mysql_error(&gMysql));
			return(3);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			if(uNotValidSystemCallArg(field[0]))
			{
				mysql_free_result(res);
				logfileLine("ProcessOSDeltaSyncJob","security alert");
				return(4);
			}

			//Try to keep options out of scripts
			char cSSHOptions[256]={""};
			GetConfiguration("cSSHOptions",cSSHOptions,gfuDatacenter,gfuNode,0,0);//First try node specific
			if(!cSSHOptions[0])
			{
				GetConfiguration("cSSHOptions",cSSHOptions,gfuDatacenter,0,0,0);//Second try datacenter wide
				if(!cSSHOptions[0])
					GetConfiguration("cSSHOptions",cSSHOptions,0,0,0,0);//Last try global
			}
			//Default for less conditions below
			if(uNotValidSystemCallArg(cSSHOptions))
				sprintf(cSSHOptions,"-p 22 -c arcfour");
			unsigned uSSHPort=22;
			char *cp;
			if((cp=strstr(cSSHOptions,"-p ")))
				sscanf(cp+3,"%u",&uSSHPort);

			//New uBackupDate. Undone later if backup fails
			unsigned uLastBackupDate=0;
        		MYSQL_RES *res2;
			MYSQL_ROW field2;
			sprintf(gcQuery,"SELECT uBackupDate FROM tContainer"
						" WHERE uContainer=%u",uCloneContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				mysql_free_result(res);
				logfileLine("ProcessOSDeltaSyncJob",mysql_error(&gMysql));
				return(5);
			}
			res2=mysql_store_result(&gMysql);
			if((field2=mysql_fetch_row(res2)))
				sscanf(field2[0],"%u",&uLastBackupDate);
			mysql_free_result(res2);
			sprintf(gcQuery,"UPDATE tContainer SET uBackupDate=UNIX_TIMESTAMP(NOW())"
						" WHERE uContainer=%u",uCloneContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				mysql_free_result(res);
				logfileLine("ProcessOSDeltaSyncJob",mysql_error(&gMysql));
				return(5);
			}

			sprintf(gcQuery,"/usr/sbin/osdeltasync.sh %u %u %s %u",uContainer,uCloneContainer,field[0],uSSHPort);
			if(guDebug)
				logfileLine("ProcessOSDeltaSyncJob",gcQuery);
			if(system(gcQuery))
			{
				mysql_free_result(res);
				logfileLine("ProcessOSDeltaSyncJob",gcQuery);
				//sync script failed so we reset the backup date
				sprintf(gcQuery,"UPDATE tContainer SET uBackupDate=%u"
						" WHERE uContainer=%u",uLastBackupDate,uCloneContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					mysql_free_result(res);
					logfileLine("ProcessOSDeltaSyncJob",mysql_error(&gMysql));
					return(7);
				}
				return(6);
			}
		}
		mysql_free_result(res);
	}
	if(guDebug)
		logfileLine("ProcessOSDeltaSyncJob","End");
	return(0);

}//void ProcessOSDeltaSyncJob()


void CloneRemoteContainer(unsigned uJob,unsigned uContainer,char *cJobData,unsigned uNewVeid)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cTargetNodeIPv4[256]={""};
	unsigned uCloneStop=0;
	unsigned uPrevStatus=0;
	unsigned uTargetNode=0;
	char cNewIP[32]={""};
	char cHostname[100]={""};
	char cName[32]={""};
	char cOSTemplate[32]={""};
	char cConfig[32]={""};

	logfileLine("CloneRemoteContainer","Start");

	//Set job data based vars
	sscanf(cJobData,"uTargetNode=%u;",&uTargetNode);
	sscanf(cJobData,"uTargetNode=%*u;\nuNewVeid=%*u;\nuCloneStop=%u;",&uCloneStop);
	sscanf(cJobData,"uTargetNode=%*u;\nuNewVeid=%*u;\nuCloneStop=%*u;\nuPrevStatus=%u;",&uPrevStatus);
	if(!uPrevStatus)
	{
		logfileLine("CloneRemoteContainer","Could not determine uPrevStatus");
		tJobErrorUpdate(uJob,"uPrevStatus==0");
		goto CommonExit;
	}

	sprintf(gcQuery,"SELECT tIP.cLabel,tContainer.cHostname,tContainer.cLabel FROM tIP,tContainer"
				" WHERE tIP.uIP=tContainer.uIPv4"
				" AND tContainer.uContainer=%u",uNewVeid);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("CloneRemoteContainer",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tIP.cLabel");
		goto CommonExit;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cNewIP,"%.31s",field[0]);
		sprintf(cHostname,"%.99s",field[1]);
		sprintf(cName,"%.31s",field[2]);
	}
	mysql_free_result(res);
	if(!cNewIP[0])
	{
		logfileLine("CloneRemoteContainer","Could not determine cNewIP");
		tJobErrorUpdate(uJob,"No cNewIP");
		goto CommonExit;
	}
	if(!cHostname[0])
	{
		logfileLine("CloneRemoteContainer","Could not determine cHostname");
		tJobErrorUpdate(uJob,"No cHostname");
		goto CommonExit;
	}
	if(!cName[0])
	{
		logfileLine("CloneRemoteContainer","Could not determine cName");
		tJobErrorUpdate(uJob,"No cName");
		goto CommonExit;
	}

	sprintf(gcQuery,"SELECT tOSTemplate.cLabel FROM tOSTemplate,tContainer WHERE tOSTemplate.uOSTemplate=tContainer.uOSTemplate"
			" AND tContainer.uContainer=%u",uNewVeid);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("CloneRemoteContainer",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tOSTemplate.cLabel");
		goto CommonExit;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cOSTemplate,"%.31s",field[0]);
	mysql_free_result(res);
	if(!cOSTemplate[0])
	{
		logfileLine("CloneRemoteContainer","Could not determine cOSTemplate");
		tJobErrorUpdate(uJob,"No cOSTemplate");
		goto CommonExit;
	}

	sprintf(gcQuery,"SELECT tConfig.cLabel FROM tConfig,tContainer WHERE tConfig.uConfig=tContainer.uConfig"
			" AND tContainer.uContainer=%u",uNewVeid);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("CloneRemoteContainer",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tConfig.cLabel");
		goto CommonExit;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cConfig,"%.31s",field[0]);
	mysql_free_result(res);
	if(!cOSTemplate[0])
	{
		logfileLine("CloneRemoteContainer","Could not determine cConfig");
		tJobErrorUpdate(uJob,"No cConfig");
		goto CommonExit;
	}

	//Remote datacenter node: We use the node cLabel. It follows that DNS/hosts must
	//be setup correctly.
	sprintf(cTargetNodeIPv4,"%.31s",ForeignKey("tNode","cLabel",uTargetNode));
	if(!cTargetNodeIPv4[0])
	{
		logfileLine("CloneRemoteContainer","Could not determine cTargetNodeIPv4");
		tJobErrorUpdate(uJob,"cTargetNodeIPv4");
		goto CommonExit;
	}

	//Most specific tConfiguration is used. This allows for some nodes to be set global
	//and others specific. But is slower than the other option with what maybe
	//very large numbers of per node tConfiguration entries.
	char cSSHOptions[256]={""};
	GetConfiguration("cSSHOptions",cSSHOptions,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cSSHOptions[0])
	{
		GetConfiguration("cSSHOptions",cSSHOptions,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cSSHOptions[0])
			GetConfiguration("cSSHOptions",cSSHOptions,0,0,0,0);//Last try global
	}
	//Default for less conditions below
	if(uNotValidSystemCallArg(cSSHOptions))
		sprintf(cSSHOptions,"-p 22 -c arcfour");

	//debug only
	//printf("uNewVeid=%u uTargetNode=%u cNewIP=%s cHostname=%s cTargetNodeIPv4=%s"
	//		" cOSTemplate=%s uCloneStop=%u cSSHOptions=%s cConfig=%s\n",
	//			uNewVeid,uTargetNode,cNewIP,cHostname,
	//			cTargetNodeIPv4,cOSTemplate,uCloneStop,cSSHOptions,cConfig);

	//0-. we check data that could be comprimised that will be used by root shell commands
	//1-. we create a container on target node from same os template as source container
	//2-. we run osdeltasync on this source node to create a working clone
	//3-. we set ip, name and hostname
	//4-. remove any other /etc/vz/conf/veid.x files
	//5-. conditionally start new veid and modify VEID.conf file to not start on boot.
	//6-. update source container status
	//7-. update target container status

	//0-.
	if(uNotValidSystemCallArg(cTargetNodeIPv4))
	{
		tJobErrorUpdate(uJob,"fail sec alert!");
		goto CommonExit;
	}
	if(uNotValidSystemCallArg(cHostname))
	{
		tJobErrorUpdate(uJob,"fail sec alert!");
		goto CommonExit;
	}
	if(uNotValidSystemCallArg(cName))
	{
		tJobErrorUpdate(uJob,"fail sec alert!");
		goto CommonExit;
	}

	//2 is no container cold initial state only in db
	if(uCloneStop!=COLD_CLONE)
	{

		//1-.
		sprintf(gcQuery,"ssh %s %s '/usr/sbin/vzctl --verbose create %u --ostemplate %s --config %s'",
				cSSHOptions,cTargetNodeIPv4,
				uNewVeid,cOSTemplate,cConfig);
		if(system(gcQuery))
		{
			logfileLine("CloneRemoteContainer",gcQuery);
			tJobErrorUpdate(uJob,"vzctl create failed");
			goto CommonExit;
		}
	}

	//2-.
	unsigned uSSHPort=22;
	char *cp;
	if((cp=strstr(cSSHOptions,"-p ")))
		sscanf(cp+3,"%u",&uSSHPort);
	sprintf(gcQuery,"/usr/sbin/osdeltasync.sh %u %u %s %u",uContainer,uNewVeid,cTargetNodeIPv4,uSSHPort);
	if(system(gcQuery))
	{
		logfileLine("CloneRemoteContainer",gcQuery);
		tJobErrorUpdate(uJob,"osdeltasync.sh");
		//rollback
		if(uCloneStop!=COLD_CLONE)
		{
			sprintf(gcQuery,"ssh %s %s 'vzctl destroy %u'",
				cSSHOptions,cTargetNodeIPv4,uNewVeid);
			system(gcQuery);
		}
		goto CommonExit;
	}

	if(uCloneStop!=COLD_CLONE)
	{
		//3-.
		//cHostname set
		sprintf(gcQuery,"ssh %s %s 'vzctl set %u --hostname %s --save'",
				cSSHOptions,cTargetNodeIPv4,uNewVeid,cHostname);
		if(system(gcQuery))
		{
			logfileLine("CloneRemoteContainer",gcQuery);
			tJobErrorUpdate(uJob,"cHostname");
			goto CommonExit;
		}
		//cName set
		sprintf(gcQuery,"ssh %s %s 'vzctl set %u --name %s --save'",
				cSSHOptions,cTargetNodeIPv4,uNewVeid,cName);
		if(system(gcQuery))
		{
			logfileLine("CloneRemoteContainer",gcQuery);
			tJobErrorUpdate(uJob,"cName");
			goto CommonExit;
		}
		//IP set
		sprintf(gcQuery,"ssh %s %s 'vzctl set %u --ipdel all --save'",
				cSSHOptions,cTargetNodeIPv4,uNewVeid);
		if(system(gcQuery))
		{
			logfileLine("CloneRemoteContainer",gcQuery);
			tJobErrorUpdate(uJob,"error 4c");
			goto CommonExit;
		}
		sprintf(gcQuery,"ssh %s %s 'vzctl set %u --ipadd %s --save'",
				cSSHOptions,cTargetNodeIPv4,uNewVeid,cNewIP);
		if(system(gcQuery))
		{
			logfileLine("CloneRemoteContainer",gcQuery);
			tJobErrorUpdate(uJob,"error 4d");
			goto CommonExit;
		}

		//4-. Remove action script files if found
		//Business logic: Target container may have these files, but we need them only if we
		//'Failover' to this cloned VE. Also we defer to that action the setup of the
		//containers tProperty values needed for processing the action script templates:
		//cNetmask, cExtraNodeIP, cPrivateIPs, cService1, cService2, cVEID.mount and cVEID.umount, etc.
		sprintf(gcQuery,"ssh %3$s %1$s 'rm -f /etc/vz/conf/%2$u.umount /etc/vz/conf/%2$u.mount"
					" /etc/vz/conf/%2$u.start /etc/vz/conf/%2$u.stop'",
						cTargetNodeIPv4,uNewVeid,cSSHOptions);
		if(system(gcQuery))
		{
			logfileLine("CloneRemoteContainer",gcQuery);
			tJobErrorUpdate(uJob,"error 4");
			goto CommonExit;
		}

		//5-.
		//This is optional since clones can be in stopped state and still be
		//kept in sync with source container. Also the failover can start (a fast operation)
		//with the extra advantage of being able to keep original IPs. Only needing an arping
		//to move the VIPs around the datacenter.
		if(uCloneStop==HOT_CLONE)
		{
			sprintf(gcQuery,"ssh %s %s 'vzctl start %u'",cSSHOptions,cTargetNodeIPv4,uNewVeid);
			if(system(gcQuery))
			{
				logfileLine("CloneRemoteContainer",gcQuery);
				tJobErrorUpdate(uJob,"error 5");
				goto CommonExit;
			}
			sprintf(gcQuery,"ssh %s %s 'vzctl set %u --onboot yes --save'",
									cSSHOptions,cTargetNodeIPv4,uNewVeid);
			if(system(gcQuery))
			{
				logfileLine("CloneRemoteContainer",gcQuery);
				tJobErrorUpdate(uJob,"error 5b");
				goto CommonExit;
			}
			SetContainerStatus(uNewVeid,uACTIVE);
		}
		else
		{
			sprintf(gcQuery,"ssh %s %s 'vzctl set %u --onboot no --save'",
									cSSHOptions,cTargetNodeIPv4,uNewVeid);
			if(system(gcQuery))
			{
				logfileLine("CloneRemoteContainer",gcQuery);
				tJobErrorUpdate(uJob,"error 5c");
				goto CommonExit;
			}
			SetContainerStatus(uNewVeid,uSTOPPED);
		}
	}
	else
	{
		//Cold clone
		SetContainerStatus(uNewVeid,uINITSETUP);
	}

	//Some job setup may have already changed it to active?
	if(uPrevStatus!=uINITSETUP)	
		SetContainerStatus(uContainer,uPrevStatus);
	tJobDoneUpdate(uJob);

//This goto MIGHT be ok.
CommonExit:
	return;

}//void CloneRemoteContainer(...)


void SwapIPContainer(unsigned uJob,unsigned uContainer,char *cJobData)
{

	char cIPNew1[31]={""};
	char cIPNew2[31]={""};
	char cIPOld1[31]={""};
	char cIPOld2[31]={""};
	char *cp;
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uVeth=0;
	unsigned uContainer2=0;

	//Check 1-. Check to make sure container is on this node, if not 
	//	give job back to queue
	sprintf(gcQuery,"/usr/sbin/vzlist %u > /dev/null 2>&1",uContainer);
	if(system(gcQuery))
	{
		logfileLine("SwapIPContainer","Job returned to queue");
		tJobWaitingUpdate(uJob);
		return;
	}


	//0-. Get required data
	sprintf(gcQuery,"SELECT tIP.cLabel,tContainer.uVeth"
			" FROM tContainer,tIP WHERE uContainer=%u"
			" AND tContainer.uIPv4=tIP.uIP",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("SwapIPContainer",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
        	MYSQL_RES *res2;
	        MYSQL_ROW field2;

		//db
		sscanf(field[1],"%u",&uVeth);

		sprintf(cIPNew1,"%.31s",field[0]);
		if(!cIPNew1[0])	
		{
			logfileLine("SwapIPContainer cIPNew",gcQuery);
			tJobErrorUpdate(uJob,"Get cIPNew failed");
			goto CommonExit;
		}

		//job data
		sscanf(cJobData,"uContainer2=%u;",&uContainer2);
		if(!uContainer2)	
		{
			logfileLine("SwapIPContainer uContainer2 from cJobData=",cJobData);
			tJobErrorUpdate(uJob,"Get uContainer2 failed");
			goto CommonExit;
		}
		sscanf(cJobData,"uContainer2=%*u;\ncIPOld1=%31s;\n",cIPOld1);
		if((cp=strchr(cIPOld1,';')))
			*cp=0;
		if(!cIPOld1[0])	
		{
			logfileLine("ChangeIPContainer cIPOld1 from cJobData=",cJobData);
			tJobErrorUpdate(uJob,"Get cIPOld1 failed");
			goto CommonExit;
		}
		if((cp=strstr(cJobData,"cIPOld2=")))
		{
			char *cp1;

			sscanf(cp,"cIPOld2=%31s;",cIPOld2);
			if((cp1=strchr(cIPOld2,';')))
				*cp1=0;
		}
		if(!cIPOld2[0])	
		{
			logfileLine("ChangeIPContainer cIPOld2 from cJobData=",cJobData);
			tJobErrorUpdate(uJob,"Get cIPOld2 failed");
			goto CommonExit;
		}

		sprintf(gcQuery,"SELECT tIP.cLabel,tContainer.uVeth"
			" FROM tContainer,tIP WHERE uContainer=%u"
			" AND tContainer.uIPv4=tIP.uIP",uContainer2);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("SwapIPContainer",mysql_error(&gMysql));
			exit(2);
		}
		res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			sprintf(cIPNew2,"%.31s",field2[0]);
			if(!cIPNew2[0])	
			{
				logfileLine("SwapIPContainer cIPNew2",gcQuery);
				tJobErrorUpdate(uJob,"Get cIPNew2 failed");
				goto CommonExit;
			}
		}
		mysql_free_result(res2);

		if(uNotValidSystemCallArg(cIPNew1)||uNotValidSystemCallArg(cIPNew2))
		{
			logfileLine("SwapIPContainer","security alert");
			tJobErrorUpdate(uJob,"failed sec alert!");
			goto CommonExit;
		}


		//debug only
		sprintf(gcQuery,"%u %s %s, %u %s %s",uContainer,cIPNew1,cIPOld1,uContainer2,cIPNew2,cIPOld2);
		logfileLine("SwapIPContainer",gcQuery);
		//tJobErrorUpdate(uJob,"debug");
		//goto CommonExit;

		if(!uVeth)
		{
			//1-.
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel %s --save",uContainer,cIPOld1);
			if(system(gcQuery))
			{
				logfileLine("SwapIPContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl set ipdel failed");
				goto CommonExit;
			}
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel %s --save",uContainer2,cIPOld2);
			if(system(gcQuery))
			{
				logfileLine("SwapIPContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl set ipdel failed");
				goto CommonExit;
			}

			//2-.
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cIPNew1);
			if(system(gcQuery))
			{
				logfileLine("SwapIPContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl set ipadd failed");
				goto CommonExit;
			}
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer2,cIPNew2);
			if(system(gcQuery))
			{
				logfileLine("SwapIPContainer",gcQuery);
				tJobErrorUpdate(uJob,"vzctl set ipadd failed");
				goto CommonExit;
			}
		}
		else
		{
			logfileLine("SwapIPContainer","uVeth containers not supported");
			tJobErrorUpdate(uJob,"uVeth container");
			goto CommonExit;
		}

		//Everything ok
		SetContainerStatus(uContainer,1);//Active
		SetContainerStatus(uContainer2,1);//Active
		tJobDoneUpdate(uJob);

	}
	else
	{
		logfileLine("SwapIPContainer",gcQuery);
		tJobErrorUpdate(uJob,"No line from query");
	}

//This goto MIGHT be ok
CommonExit:
	mysql_free_result(res);
	return;

}//void SwapIPContainer()


//AllowAccess and DenyAccess are used for unxsvzOrg.cgi interface only at this time
//These functions assume something like this at end part of /etc/sysconfig/iptables:
// -A FORWARD -p tcp -m tcp --dport 443 -j REJECT --reject-with icmp-port-unreachable
void AllowAccess(unsigned uJob,const char *cJobData,unsigned uDatacenter,unsigned uNode)
{
	char cIPv4[16]={""};
	char *cp;

	sscanf(cJobData,"cIPv4=%15s;",cIPv4);
	if((cp=strchr(cIPv4,';')))
		*cp=0;
	if(!cIPv4[0])
	{
		logfileLine("AllowAccess","Could not determine cIPv4");
		tJobErrorUpdate(uJob,"cIPv4[0]==0");
		return;
	}

        MYSQL_RES *res;
        MYSQL_ROW field;

	char cTemplate[512]={
				"/sbin/iptables -L -n | grep -w %1$s > /dev/null; if [ $? != 0 ];then"
					" /sbin/iptables -I FORWARD -s %1$s -p tcp -m tcp --dport 443 -j ACCEPT;"
				" fi;"
					};

	FILE *fp;
	char cPrivateKey[256]={""};
	if((fp=fopen("/etc/unxsvz/unxsvz.key","r")))
	{
		if(fgets(cPrivateKey,255,fp)!=NULL)
			cPrivateKey[strlen(cPrivateKey)-1]=0;//cut off /n
		fclose(fp);
	}

	if(cPrivateKey[0])
	{
		sprintf(gcQuery,"SELECT cComment FROM tConfiguration"
			//trick to get most specific datacenter node combo
			" WHERE SHA1(CONCAT(LEFT(cComment,LOCATE('#unxsVZKey=',cComment)),'%1$s'))=SUBSTR(cComment,LOCATE('#unxsVZKey=',cComment)+11)"
			" AND ((uDatacenter=%2$u AND uNode=%3$u) OR (uDatacenter=%2$u AND uNode=0))"
			" AND cLabel='cAllowAccessTemplate' AND cValue='cComment' ORDER BY uNode DESC LIMIT 1",cPrivateKey,uDatacenter,uNode);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			logfileLine("AllowAccess",mysql_error(&gMysql));
		else
		{
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(cTemplate,"%.512s",field[0]);
				logfileLine("AllowAccess","secure template used");
			}
			mysql_free_result(res);
		}
	}

	//debug only
	//char cData[128];
	//sprintf(cData,"uDatacenter=%u uNode=%u",uDatacenter,uNode);
	//logfileLine("AllowAccess",cData);

	sprintf(gcQuery,cTemplate,cIPv4,cIPv4);
	if(system(gcQuery))
	{
		logfileLine("AllowAccess","iptables command failed");
		tJobErrorUpdate(uJob,"iptables command failed");
		return;
	}

	logfileLine("AllowAccess","iptables command ok");
	tJobDoneUpdate(uJob);
	//UpdateIPFWStatus(cIPv4,uFWACCESS);
	return;

}//void AllowAccess()


//AllowAccess and DenyAccess are used for unxsvzOrg.cgi interface only at this time
void DenyAccess(unsigned uJob,const char *cJobData)
{
	char cIPv4[16]={""};
	char *cp;

	sscanf(cJobData,"cIPv4=%15s;",cIPv4);
	if((cp=strchr(cIPv4,';')))
		*cp=0;
	if(!cIPv4[0])
	{
		logfileLine("DenyAccess","Could not determine cIPv4");
		tJobErrorUpdate(uJob,"cIPv4[0]==0");
		return;
	}

	//Test fixed rule for now
	sprintf(gcQuery,
				" /sbin/iptables -L -n | grep %1$.15s > /dev/null; if [ $? == 0 ];then"
					" /sbin/iptables -D FORWARD -s %1$.15s -p tcp -m tcp --dport 443 -j ACCEPT;"
				" fi;"
					,cIPv4);
	if(system(gcQuery))
		logfileLine("DenyAccess","iptables del command failed but ignored");

	logfileLine("DenyAccess","iptables command ok");
	tJobDoneUpdate(uJob);
	//UpdateIPFWStatus(cIPv4,uFWBLOCKED);
	return;

}//void DenyAccess(unsigned uJob,const char *cJobData)


//This is a big security risk implement some kind of MD5 hash shared secret for it
//If MySQL is under enemy control they can run commands as root in any and all containers
void ExecuteCommands(unsigned uJob,unsigned uContainer,char *cJobData)
{
        MYSQL_RES *res;

	//Check 1-. Check to make sure container is on this node, if not 
	//	give job back to queue
	sprintf(gcQuery,"/usr/sbin/vzlist %u > /dev/null 2>&1",uContainer);
	if(system(gcQuery))
	{
		logfileLine("ExecuteCommands","Job returned to queue no such container");
		tJobWaitingUpdate(uJob);
		return;
	}

	//Check 2-. Wait till any OTHER jobs currently in the job queue for this
	//	container are done.
	sprintf(gcQuery,"SELECT uJob FROM tJob"
			" WHERE uContainer=%u AND (uJobStatus=%u OR uJobStatus=%u)"
			" AND uJob!=%u AND cJobName!='ExecuteCommands'",
				uContainer,uWAITING,uRUNNING,uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ExecuteCommands",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
		logfileLine("ExecuteCommands","Job returned to queue other jobs pending");
		tJobWaitingUpdate(uJob);
		mysql_free_result(res);
		return;
	}
	mysql_free_result(res);

	//1-.
	FILE *pfp;
	sprintf(gcQuery,"/usr/sbin/vzctl exec %u -",uContainer);
	if((pfp=popen(gcQuery,"w"))==NULL)
	{
		logfileLine("ExecuteCommands",gcQuery);
		tJobErrorUpdate(uJob,"popen error");
		return;
	}
	if(fprintf(pfp,"%s",cJobData)<0)
	{
		logfileLine("ExecuteCommands",gcQuery);
		tJobErrorUpdate(uJob,"fprintf error");
		return;
	}
	if(pclose(pfp)<0)
	{
		logfileLine("ExecuteCommands",gcQuery);
		tJobErrorUpdate(uJob,"pclose error");
		return;
	}

	//Everything ok
	//This job has no transient status change
	//SetContainerStatus(uContainer,1);//Active
	tJobDoneUpdate(uJob);
	//Clean out what may be a lot of repeated cJobData
	sprintf(gcQuery,"UPDATE tJob SET cJobData='' WHERE uJob=%u",uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		logfileLine("ExecuteCommands",mysql_error(&gMysql));
	return;

}//void ExecuteCommands()


void NodeCommandJob(unsigned uJob,unsigned uContainer,char *cJobData,unsigned uNode,unsigned uDatacenter)
{
	//1-. Get script/command name
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cCommand[256]={""};
	unsigned uConfiguration=0;
	sscanf(cJobData,"uConfiguration=%u;",&uConfiguration);
	if(!uConfiguration)
	{
		logfileLine("NodeCommandJob","Could not determine uConfiguration");
		tJobErrorUpdate(uJob,"uConfiguration==0");
		return;
	}
	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uConfiguration=%u",uConfiguration);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("NodeCommandJob",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cCommand,"%.255s",field[0]);
	}
	else
	{
		logfileLine("NodeCommandJob",cCommand);
		tJobErrorUpdate(uJob,"select cValue failed");
		return;
	}
	mysql_free_result(res);


	//2-. Make sure it is security valid
	struct stat statInfo;

	if(uNotValidSystemCallArg(cCommand))
	{
		logfileLine("NodeCommandJob","uNotValidSystemCallArg security alert");
		tJobErrorUpdate(uJob,"security alert");
		return;
	}

	//Only run if command is chmod 500 and owned by root for extra security reasons.
	if(stat(cCommand,&statInfo))
	{
		logfileLine("NodeCommandJob","stat failed");
		logfileLine("NodeCommandJob",cCommand);
		tJobErrorUpdate(uJob,"stat failed");
		return;
	}
	if(statInfo.st_uid!=0)
	{
		logfileLine("NodeCommandJob","not owned by root");
		tJobErrorUpdate(uJob,"not owned by root");
		return;
	}
	if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
	{
		logfileLine("NodeCommandJob","not chmod 500");
		tJobErrorUpdate(uJob,"not chmod 500");
		return;
	}


	//3-. Make sure any other uJobN=uJob; jobs have run ok. Unless uJob0==0
	unsigned uJob0=0,uJob1=0,uJob2=0,uJob3=0;
	unsigned uCount=0;
	char *cp;
	if((cp=strstr(cJobData,"uJob0=")))
	{
		uCount=sscanf(cp,"uJob0=%u;\nuJob1=%u;\nuJob2=%u;\nuJob3=%u;",&uJob0,&uJob1,&uJob2,&uJob3);
		sprintf(gcQuery,"uJob0=%u; uJob1=%u; uJob2=%u; uJob3=%u;",uJob0,uJob1,uJob2,uJob3);
		logfileLine("NodeCommandJob",gcQuery);
	}
	if(uJob0)
	{
		sprintf(gcQuery,"SELECT SUM(uJobStatus) FROM tJob WHERE uJob=%u OR uJob=%u OR uJob=%u OR uJob=%u",
			uJob0,uJob1,uJob2,uJob3);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("NodeCommandJob",mysql_error(&gMysql));
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			unsigned uSumJobStatus=0;
			sscanf(field[0],"%u",&uSumJobStatus);
			if(uJob3 && uJob2 && uJob1 && uJob0)
				uCount=4;
			else if(uJob2 && uJob1 && uJob0)
				uCount=3;
			else if(uJob1 && uJob0)
				uCount=2;
			else if(uJob0)
				uCount=1;

			if(uSumJobStatus!=(uCount*3))
			{
				logfileLine("NodeCommandJob","waiting for job");
				tJobWaitingUpdate(uJob);
				mysql_free_result(res);
				return;
			}

		}
		else
		{
			logfileLine("NodeCommandJob","no uJobStatus");
			tJobErrorUpdate(uJob,"no uJobStatus");
			mysql_free_result(res);
			return;
		}
		mysql_free_result(res);
	}
	else
	{
		logfileLine("NodeCommandJob","no uJob0 parameter or uJob==0");
		//We allow no dns jobs now
		//tJobErrorUpdate(uJob,"no uJob0 parameters");
		//return;
	}

	//4-. Get basic container parameters and run the command
	char cOnScriptCall[512];

	sprintf(gcQuery,"SELECT tContainer.cHostname,tIP.cLabel"
				" FROM tContainer,tIP WHERE tContainer.uIPv4=tIP.uIP AND tContainer.uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("NodeCommandJob",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
	
		char *cOptions;	
		char *cp;
		unsigned uRetval=0;
		unsigned uDestroyOnSuccess=0;
		unsigned unxsSIPSJobs=0;
		unsigned uReleaseOldIp=0;

		if(strstr(cJobData,"DestroyContainer=yes"))
			uDestroyOnSuccess=1;
	
		if(strstr(cJobData,"unxsSIPSJobs=yes"))
			unxsSIPSJobs=1;

		if((cp=strstr(cJobData,"ReleaseOldIp=")))
			sscanf(cp+strlen("ReleaseOldIp="),"%u;",&uReleaseOldIp);
	
		if((cOptions=strstr(cJobData,"cOptions")))
		{
			if((cp=strchr(cOptions,'\n'))) *cp=0;
		}
		else
		{
			cOptions="";	
		}
		if(cOptions[0])
			sprintf(cOnScriptCall,"%.255s %u %s %s %s",cCommand,uContainer,field[0],field[1],cOptions+strlen("cOptions: "));
		else
			sprintf(cOnScriptCall,"%.255s %u %s %s",cCommand,uContainer,field[0],field[1]);
		logfileLine("NodeCommandJob",cOnScriptCall);
		//system returns retval of cOnScriptCall times 256.
		//cOnScriptCall must return values only between 0 and 255.
		if((uRetval=system(cOnScriptCall)))
		{
			char cMsg[32];
			tJobErrorUpdate(uJob,"cOnScriptCall failed");
			logfileLine("NodeCommandJob","cOnScriptCall failed");
			sprintf(cMsg,"uRetval/256=%u",uRetval/256);
			logfileLine("NodeCommandJob",cMsg);
			uDestroyOnSuccess=0;
		}

	
		if(unxsSIPSJobs)
		{
			//unxsSIPS jobs
			register int i,j=0;
			char cSIPProxyList[256];
			char cServer[32];
			GetConfiguration("cSIPProxyList",cSIPProxyList,0,0,0,0);
			for(i=0;cSIPProxyList[i];i++)
			{
				if(cSIPProxyList[i]!=';')
					continue;
				cSIPProxyList[i]=0;
				sprintf(cServer,"%.31s",cSIPProxyList+j);
				j=i+1;//next beg if app
				cSIPProxyList[i]=';';
				sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsSIPSReload(%u)',cJobName='unxsSIPSReload'"
						",uDatacenter=%u,uNode=%u,uContainer=%u"
						",uJobDate=UNIX_TIMESTAMP(NOW())"
						",uJobStatus=%u"
						",cJobData='"
						"cServer=%s;\n'"
						",uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							uContainer,
							uDatacenter,
							uNode,
							uContainer,
							uREMOTEWAITING,
							cServer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					logfileLine("NodeCommandJob",mysql_error(&gMysql));
				logfileLine("NodeCommandJob",cServer);
			}
		}

		//always stop
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u",uContainer);
		if(system(gcQuery))
		{
			logfileLine("NodeCommandJob",gcQuery);
			tJobErrorUpdate(uJob,"vzctl stop failed");
			return;
		}

		//always release old IP
		if(uReleaseOldIp)
		{
			sprintf(gcQuery,"UPDATE tIP SET uAvailable=1 WHERE uIP=%u",uReleaseOldIp);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				logfileLine("NodeCommandJob",mysql_error(&gMysql));
			logfileLine("NodeCommandJob","uReleaseOldIp update done");
		}


		if(uDestroyOnSuccess)
		{
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose destroy %u",uContainer);
			if(system(gcQuery))
			{
				logfileLine("NodeCommandJob",gcQuery);
				tJobErrorUpdate(uJob,"vzctl destroy failed");
				return;
			}

			//remove -m if everything went fine
			char cLabel[64]={""};
			sprintf(cLabel,"%.63s",ForeignKey("tContainer","cLabel",uContainer));
			if((cp=strstr(cLabel+(strlen(cLabel)-2),"-m")))
				*cp=0;
			sprintf(gcQuery,"UPDATE tContainer SET cLabel='%s' WHERE uContainer=%u",cLabel,uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				logfileLine("NodeCommandJob",mysql_error(&gMysql));
		}
	}
	mysql_free_result(res);

	//Everything ok
	tJobDoneUpdate(uJob);


}//void NodeCommandJob()


void RestartContainer(unsigned uJob,unsigned uContainer)
{

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose restart %u",uContainer);
	if(system(gcQuery))
	{
		logfileLine("RestartContainer",gcQuery);
		tJobErrorUpdate(uJob,"vzctl restart failed");
		return;
	}

	//Optional group based script may exist to be executed.
	//
	//Primary group is oldest tGroupGlue entry.
        MYSQL_RES *res;
        MYSQL_ROW field;
	//UBC safe
	sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
			" AND tProperty.uKey=tGroupGlue.uGroup"
			" AND tGroupGlue.uContainer=%u"
			" AND tProperty.cName='cJob_OnRestartScript' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",uPROP_GROUP,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("RestartContainer",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		struct stat statInfo;
		char cOnScriptCall[512];
		char cCommand[256];
		char *cp;

		sprintf(cCommand,"%.255s",field[0]);

		//Remove trailing junk
		if((cp=strchr(cCommand,'\n')) || (cp=strchr(cCommand,'\r'))) *cp=0;

		if(uNotValidSystemCallArg(cCommand))
		{
			logfileLine("RestartContainer","cJob_OnRestartScript security alert");
			goto CommonExit2;
		}

		//Only run if command is chmod 500 and owned by root for extra security reasons.
		if(stat(cCommand,&statInfo))
		{
			logfileLine("RestartContainer","stat failed for cJob_OnRestartScript");
			logfileLine("RestartContainer",cCommand);
			goto CommonExit2;
		}
		if(statInfo.st_uid!=0)
		{
			logfileLine("RestartContainer","cJob_OnRestartScript is not owned by root");
			goto CommonExit2;
		}
		if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
		{
			logfileLine("RestartContainer","cJob_OnRestartScript is not chmod 500");
			goto CommonExit2;
		}

		char cHostname[100]={""};
		sprintf(gcQuery,"SELECT tContainer.cHostname"
				" FROM tContainer WHERE uContainer=%u",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("",mysql_error(&gMysql));
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(cHostname,"%.99s",field[0]);

			sprintf(cOnScriptCall,"%.255s %.64s %u",cCommand,cHostname,uContainer);
			if(system(cOnScriptCall))
			{
				logfileLine("RestartContainer",cOnScriptCall);
			}
		}
	}
CommonExit2:
	mysql_free_result(res);

	//Everything ok
	SetContainerStatus(uContainer,uACTIVE);
	tJobDoneUpdate(uJob);

}//void RestartContainer()


//Special migration of container that runs optional source node script.
//and that is usually used for DNS based services running in container.
//Code that create job must double check every resource that will be used.
void DNSMoveContainer(unsigned uJob,unsigned uContainer,char *cJobData,unsigned uDatacenter,unsigned uNode)
{
	char cTargetNodeIPv4[256]={""};
	char cIPv4[32]={""};
	unsigned uTargetNode=0;
	unsigned uTargetDatacenter=0;
	unsigned uIPv4=0;
	unsigned uPrevStatus=0;

	//Must wait for clone or template operations to finish.
	if(access("/var/run/vzdump.lock",R_OK)==0)
	{
		logfileLine("DNSMoveContainer","/var/run/vzdump.lock exists");
		tJobWaitingUpdate(uJob);
		return;
	}

	sscanf(cJobData,"uTargetNode=%u;",&uTargetNode);
	if(!uTargetNode)
	{
		logfileLine("DNSMoveContainer","Could not determine uTargetNode");
		tJobErrorUpdate(uJob,"uTargetNode==0");
		return;
	}

	sscanf(ForeignKey("tNode","uDatacenter",uTargetNode),"%u",&uTargetDatacenter);
	if(!uTargetDatacenter)
	{
		logfileLine("DNSMoveContainer","Could not determine uTargetDatacenter");
		tJobErrorUpdate(uJob,"uTargetDatacenter==0");
		return;
	}

	sscanf(cJobData,"uTargetNode=%*u;\nuIPv4=%u;",&uIPv4);
	if(uIPv4)
	{
		sprintf(cIPv4,"%.31s",ForeignKey("tIP","cLabel",uIPv4));
		logfileLine("DNSMoveContainer",cIPv4);

		//Remote migration uses node name for ssh
		sprintf(cTargetNodeIPv4,"%.255s",ForeignKey("tNode","cLabel",uTargetNode));
	}
	else
	{
		logfileLine("DNSMoveContainer","Could not determine uIPv4");
		tJobErrorUpdate(uJob,"uIPv4==0");
		return;
	}

	if(!cTargetNodeIPv4[0])
	{
		logfileLine("DNSMoveContainer","Could not determine cTargetNodeIPv4");
		tJobErrorUpdate(uJob,"cTargetNodeIPv4");
		return;
	}

	sscanf(cJobData,"uTargetNode=%*u;\nuIPv4=%*u;\nuPrevStatus=%u;",&uPrevStatus);
	if(uPrevStatus!=uACTIVE && uPrevStatus!=uSTOPPED)
	{
		sprintf(gcQuery,"cJobData uPrevStatus not active or stopped. uPrevStatus=%u",uPrevStatus);
		logfileLine("DNSMoveContainer",gcQuery);
		tJobErrorUpdate(uJob,"uPrevStatus problem");
		return;
	}

	//SSH and SCP options
	char cSSHOptions[256]={""};
	GetConfiguration("cSSHOptions",cSSHOptions,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cSSHOptions[0])
	{
		GetConfiguration("cSSHOptions",cSSHOptions,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cSSHOptions[0])
			GetConfiguration("cSSHOptions",cSSHOptions,0,0,0,0);//Last try global
	}
	if(uNotValidSystemCallArg(cSSHOptions))
	{
		logfileLine("DNSMoveContainer","security alert");
		cSSHOptions[0]=0;
	}
	char cSCPOptions[256]={""};
	GetConfiguration("cSCPOptions",cSCPOptions,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cSCPOptions[0])
	{
		GetConfiguration("cSCPOptions",cSCPOptions,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cSCPOptions[0])
			GetConfiguration("cSCPOptions",cSCPOptions,0,0,0,0);//Last try global
	}
	//Default for less conditions below
	//Configure normal default via /etc/ssh/ssh_config
	if(uNotValidSystemCallArg(cSCPOptions))
		sprintf(cSCPOptions,"-P 22 -c arcfour");


	//0-. Get configuration data
	char cSnapshotDir[256]={""};
	GetConfiguration("cSnapshotDir",cSnapshotDir,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cSnapshotDir[0])
	{
		GetConfiguration("cSnapshotDir",cSnapshotDir,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cSnapshotDir[0])
			GetConfiguration("cSnapshotDir",cSnapshotDir,0,0,0,0);//Last try global
	}
	if(uNotValidSystemCallArg(cSnapshotDir))
		cSnapshotDir[0]=0;

	//1-. vzdump the container unless it already exists and was created recently
	struct stat statInfo;
	char cSnapshotFile[256]={""};
	if(cSnapshotDir[0])
		sprintf(cSnapshotFile,"%s/vzdump-%u.tgz",cSnapshotDir,uContainer);
	else
		sprintf(cSnapshotFile,"/var/vzdump/vzdump-%u.tgz",uContainer);
	//if(stat(cSnapshotFile,&statInfo))
	//TODO use mod time and figure out
	if(1)
	{

		if(!cSnapshotDir[0])
			sprintf(gcQuery,"/usr/sbin/vzdump --compress --suspend %u",uContainer);
		else
			sprintf(gcQuery,"/usr/sbin/vzdump --compress --dumpdir %s --snapshot %u",
									cSnapshotDir,uContainer);
		if(system(gcQuery))
		{
			logfileLine("DNSMoveContainer",gcQuery);
			tJobErrorUpdate(uJob,"vzdump error1");
			unlink("/var/run/vzdump.lock");
			return;
		}
		unlink("/var/run/vzdump.lock");
		//1b-.
		//New vzdump uses new file format, E.G.: /var/vzdump/vzdump-openvz-10511-2011_02_03-07_37_01.tgz
		//Quick fix (hackorama) just mv it to old format
		//Added support for old vzdump
		if(!cSnapshotDir[0])
			sprintf(gcQuery,"if [ ! -f /var/vzdump/vzdump-%u.tgz ];then"
					" mv `ls -1 /vz/dump/vzdump*-%u-*.tgz | head -n 1` /vz/dump/vzdump-%u.tgz; fi;",
						uContainer,uContainer,uContainer);
		else
			sprintf(gcQuery,"if [ -f /var/vzdump/vzdump-%u.tgz ] && [ \"%s\" != \"/var/vzdump\" ];then"
					" mv /var/vzdump/vzdump-%u.tgz %s/vzdump-%u.tgz;else"
					" if [ -f %s/vzdump*-%u-*.tgz ];then mv `ls -1 %s/vzdump*-%u-*.tgz | head -n 1` %s/vzdump-%u.tgz;fi;fi;",
					uContainer,cSnapshotDir,
					uContainer,cSnapshotDir,uContainer,
					cSnapshotDir,uContainer,cSnapshotDir,uContainer,cSnapshotDir,uContainer);
		if(system(gcQuery))
		{
			logfileLine("DNSMoveContainer",gcQuery);
			tJobErrorUpdate(uJob,"error rename");
			return;
		}

		//1c-. md5sum generation we stash in /vz/template/cache
		if(!stat("/usr/bin/md5sum",&statInfo))
		{
			if(!cSnapshotDir[0])
				sprintf(gcQuery,"/usr/bin/md5sum /var/vzdump/vzdump-%1$u.tgz >"
					" /var/vzdump/vzdump-%1$u.tgz.md5sum",uContainer);
			else
				sprintf(gcQuery,"/usr/bin/md5sum %1$s/vzdump-%2$u.tgz >"
					" %1$s/vzdump-%2$u.tgz.md5sum",cSnapshotDir,uContainer);
			if(system(gcQuery))
			{
				logfileLine("DNSMoveContainer",gcQuery);
				tJobErrorUpdate(uJob,"md5sum failed");
				return;
			}
		}
		else
		{
			logfileLine("DNSMoveContainer",gcQuery);
			tJobErrorUpdate(uJob,"no md5sum");
			return;
		}
	}//If tgz file does not already exist
	else
	{
			char cMsg[128]={""};
			sprintf(cMsg,"%.99s already exists",cSnapshotFile);
			logfileLine("DNSMoveContainer",cMsg);
	}


	//2-. Change container label after vzdump to avoid any confusion
	//	first save for use in DNS section
	char cHostname[128]={""};
	sprintf(cHostname,"%.127s",ForeignKey("tContainer","cHostname",uContainer));
	sprintf(gcQuery,"UPDATE tContainer SET cLabel=CONCAT(cLabel,'-m') WHERE uContainer=%u AND LENGTH(cLabel)<30 AND cLabel NOT LIKE '%%-m'",
				uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("DNSMoveContainer",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"Update cLabel error");
		return;
	}

	//3-. SCP the dump to the new node.
	char cBWLimit[32]={""};
	if(uTargetDatacenter && uTargetDatacenter!=uDatacenter)
	{
        	MYSQL_RES *res;
        	MYSQL_ROW field;
		//UBC safe
		sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
				" AND tProperty.uKey=tGroupGlue.uGroup"
				" AND tGroupGlue.uContainer=%u"
				" AND tProperty.cName='cJob_DNSMoveBW' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",
						uPROP_GROUP,uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("DNSMoveContainer",mysql_error(&gMysql));
			tJobErrorUpdate(uJob,"cJob_DNSMoveBW error");
			return;
		}
        	res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			unsigned uBWLimit=0;//0 is no limit
			sscanf(field[0],"%u",&uBWLimit);
			sprintf(cBWLimit," -l %u",uBWLimit);
			//debug only
			logfileLine("DNSMoveContainer uBWLimit",field[0]);
		}
		mysql_free_result(res);
	}

	logfileLine("DNSMoveContainer","scp start");
	if(cSnapshotDir[0])
	{
		sprintf(gcQuery,"/usr/bin/ssh %s %s 'mkdir -p %s'",
				cSSHOptions,cTargetNodeIPv4,cSnapshotDir);
		if(system(gcQuery))
		{
			logfileLine("DNSMoveContainer",gcQuery);
			tJobErrorUpdate(uJob,"ssh mkdir cSnapshotDir");
			return;
		}
	}
	sprintf(gcQuery,"/usr/bin/scp %s%s %s %s:%s",
			cSCPOptions,cBWLimit,cSnapshotFile,cTargetNodeIPv4,cSnapshotFile);
	if(system(gcQuery))
	{
		logfileLine("DNSMoveContainer",gcQuery);
		tJobErrorUpdate(uJob,"scp vzdump");
		return;
	}
	//scp md5sum 
	sprintf(gcQuery,"/usr/bin/scp %s%s %s.md5sum %s:%s.md5sum",
			cSCPOptions,cBWLimit,cSnapshotFile,cTargetNodeIPv4,cSnapshotFile);
	if(system(gcQuery))
	{
		logfileLine("DNSMoveContainer",gcQuery);
		tJobErrorUpdate(uJob,"scp md5sum");
		return;
	}
	//Check scp via remote md5sum
	sprintf(gcQuery,"ssh %s %s '/usr/bin/md5sum -c %s.md5sum'",
				cSSHOptions,cTargetNodeIPv4,cSnapshotFile);
	if(system(gcQuery))
	{
		logfileLine("DNSMoveContainer",gcQuery);
		tJobErrorUpdate(uJob,"ssh md5sum");
		return;
	}
	logfileLine("DNSMoveContainer","scp end");

	//4-. Restore the container on the new node. Start if uPrevStatus=1;
	logfileLine("DNSMoveContainer","vzrestore start");
	sprintf(gcQuery,"ssh %s %s '/usr/sbin/vzrestore %s %u'",
				cSSHOptions,cTargetNodeIPv4,cSnapshotFile,uContainer);
	if(system(gcQuery))
	{
		logfileLine("DNSMoveContainer",gcQuery);
		tJobErrorUpdate(uJob,"ssh vzrestore");
		return;
	}
	logfileLine("DNSMoveContainer","vzrestore end");
	//Local cleanup /var/vzdump/vzdump-32131.tgz
	sprintf(gcQuery,"rm -f /var/vzdump/vzdump-%u.tgz",uContainer);
	if(system(gcQuery))
	{	
		//non fatal 
		logfileLine("DNSMoveContainer",gcQuery);
	}
	//Remote cleanup /var/vzdump/vzdump-32131.tgz
	sprintf(gcQuery,"ssh %s %s 'rm -f /var/vzdump/vzdump-%u.tgz'",cSSHOptions,cTargetNodeIPv4,uContainer);
	if(system(gcQuery))
	{	
		//non fatal 
		logfileLine("DNSMoveContainer",gcQuery);
	}

	//Change IP
	sprintf(gcQuery,"ssh %s %s 'vzctl set %u --ipdel all --save'",
				cSSHOptions,cTargetNodeIPv4,uContainer);
	if(system(gcQuery))
	{
		logfileLine("DNSMoveContainer",gcQuery);
		tJobErrorUpdate(uJob,"ssh vzctl ipdel all");
		return;
	}
	sprintf(gcQuery,"ssh %s %s 'vzctl set %u --ipadd %s --save'",
				cSSHOptions,cTargetNodeIPv4,uContainer,cIPv4);
	if(system(gcQuery))
	{
		logfileLine("DNSMoveContainer",gcQuery);
		tJobErrorUpdate(uJob,"ssh vzctl ipadd");
		return;
	}
	//Conditionally start
	if(uPrevStatus==uACTIVE)
	{
		sprintf(gcQuery,"ssh %s %s '/usr/sbin/vzctl start %u'",
				cSSHOptions,cTargetNodeIPv4,uContainer);
		if(system(gcQuery))
		{
			logfileLine("DNSMoveContainer",gcQuery);
			tJobErrorUpdate(uJob,"ssh vzctl start");
			return;
		}
	}

	//5-. Change the DNS A record to the new IP of the new node. Unless stopped clone.
	unsigned uSource=0;
	sscanf(ForeignKey("tContainer","uSource",uContainer),"%u",&uSource);
	unsigned uCreateDNSJob=0;
	if(!(uSource && uPrevStatus==uSTOPPED))
	//ramifications: uCreateDNSJob will be zero in node job
	{
		if(!(uCreateDNSJob=CreateDNSJob(0,1,cIPv4,cHostname,uTargetDatacenter,1,uContainer,uNode)))
		{
			logfileLine("DNSMoveContainer","CreateDNSJob() error");
			tJobErrorUpdate(uJob,"create dns job");
			return;
		}
	}

	//6-. Run optional post dns change script on source node.
	//	The script should internally wait until the DNS A record has changed.
	//	The script should also handle any cleanup required. Like in 7-.
	//	Maybe we should create a job here? This will keep things neat and
	//	and allow code reuse
	char cPostDNSNodeScript[256]={""};
	char cPostDNSNodeScriptName[256]={"cPostDNSNodeScript"};
	char cArgs[512];
	unsigned uConfiguration=0;
	//First see if based on container primary group a different cPostDNSNodeScriptName should be used
	GetGroupBasedPropertyValue(uContainer,"cJob_DNSMoveScriptName",cPostDNSNodeScriptName);
	if(strcmp("cPostDNSNodeScript",cPostDNSNodeScriptName))
	{
		logfileLine("DNSMoveContainer","using alternative cPostDNSNodeScriptName");
		logfileLine("DNSMoveContainer",cPostDNSNodeScriptName);
	}
		
	//Try most specific match datacenter and node first
	uConfiguration=GetConfiguration(cPostDNSNodeScriptName,cPostDNSNodeScript,uDatacenter,uNode,0,0);
	//If that fails try datacenter wide configuration
	if(!uConfiguration)
		uConfiguration=GetConfiguration(cPostDNSNodeScriptName,cPostDNSNodeScript,uDatacenter,0,0,0);
	//Please note that this means that the script must be installed for all nodes of
	//any datacenter that has at least one tConfiguration for a specific uDatacenter+uNode
	if(cPostDNSNodeScript[0] && uConfiguration && !uSource)
	{
		unsigned uIP=0;

		sscanf(ForeignKey("tContainer","uIPv4",uContainer),"%u",&uIP);
		sprintf(cArgs,"Configured script:%.127s\nRun after:\nuJob0=%u;\nuJob1=%u;\nuJob2=%u;\n"
				//"cOptions: cRemoteNodeIPv4=%s; cIPv4=%s;\n",
				"DestroyContainer=yes;\n"
				"unxsSIPSJobs=yes;\n"
				"ReleaseOldIp=%u;\n"
				//the next line must be last see NodeCommand
				"cOptions: %s %s\n",
							cPostDNSNodeScript,uJob,uCreateDNSJob,0,
							uIP,
							cTargetNodeIPv4,cIPv4);
		if(!uNodeCommandJob(uDatacenter,uNode,uContainer,1,1,uConfiguration,cArgs))
		{
			logfileLine("DNSMoveContainer","uNodeCommandJob() error");
			tJobErrorUpdate(uJob,"create command job");
			return;
		}
		logfileLine("DNSMoveContainer",cPostDNSNodeScript);
	}
	else
	{
		if(uSource)
			sprintf(cArgs,"Script bypassed for uSource!=0 container");
		else
			sprintf(cArgs,"No cPostDNSNodeScript found for uDatacenter=%u,uNode=%u,uConfiguration=%u,%s",
					uDatacenter,uNode,uConfiguration,cPostDNSNodeScript);
		logfileLine("DNSMoveContainer",cArgs);
	}

	//7-. If everything went well, destroy the local PBX
	//	And clean up our database.

	//!NO! we need to wait for all services to be moved.

	//Other misc work
	//If container rrd files exists in our standard fixed dir cp to new node
	//Note that cleanup maybe required in the future
	sprintf(gcQuery,"/var/lib/rrd/%u.rrd",uContainer);
	if(!access(gcQuery,R_OK))
	{
		sprintf(gcQuery,"/usr/bin/scp %s%s /var/lib/rrd/%u.rrd %s:/var/lib/rrd/",
			cSCPOptions,cBWLimit,uContainer,cTargetNodeIPv4);
		if(system(gcQuery))
			logfileLine("DNSMoveContainer",gcQuery);
	}

	//Everything ok
	SetContainerStatus(uContainer,uPrevStatus);//Previous to awaiting migration
	SetContainerNode(uContainer,uTargetNode);//Migrated!
	SetContainerIP(uContainer,cIPv4);//Has new IP
	SetContainerDatacenter(uContainer,uTargetDatacenter);
	tJobDoneUpdate(uJob);

}//void DNSMoveContainer(unsigned uJob,unsigned uContainer,char *cJobData)



//returns script in cCommand 256 byte buffer on success else empty string or same string as on entry
//UBC safe
void GetGroupBasedPropertyValue(unsigned uContainer,char const *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	//Primary group is oldest tGroupGlue entry.
	sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
			" AND tProperty.uKey=tGroupGlue.uGroup"
			" AND tGroupGlue.uContainer=%u"
			" AND tProperty.cName='%s' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",uPROP_GROUP,uContainer,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("GetGroupBasedPropertyValue",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;

		sprintf(cValue,"%.255s",field[0]);
		//Remove trailing junk
		if((cp=strchr(cValue,'\n')) || (cp=strchr(cValue,'\r'))) *cp=0;
	}

	mysql_free_result(res);

}//void GetGroupBasedPropertyValue(unsigned uContainer,char const *cName,char *cValue)


void ActivateNATContainer(unsigned uJob,unsigned uContainer,unsigned uNode)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	//Roadmap
	//At group function in tcontainerfunc.h
	//	Check to make sure container is of correct type
	//	Check to make sure that required node properties exist
	//		"cPublicNATIP" and "cPrivateNATNetwork"
	//	Update container to private IP based on available rfc1918 IPs for the given node/datacenter
	//		and if exists a tConfiguration cAutoNATIPClass SQL pattern
	//	Make sure container has the required properties setup. See unxsNAT natpbx.c
	//		We will try to get ActivateNATNode() to always run first to avoid this issue.
	//	Create a single per node job to run the iptables setup of the unxsNAT program.
	//		See ActivateNATNode() below. This must run first. This job must wait.
	//	Create this job once per node container.
	//Here:
	//	We verify security for script
	//	Create system command for script
	//	Execute the script
	//The script:
	//	ChangeFreePBX part of unxsNAT program.
	//	Other per container required changes to be determined.

	//Gather NAT data
	char cPublicNATIP[256]={""};
	GetNodeProp(uNode,"cPublicNATIP",cPublicNATIP);
	if(!cPublicNATIP[0])
	{
		logfileLine("ActivateNATContainer","No cPublicNATIP");
		tJobErrorUpdate(uJob,"No cPublicNATIP");
		return;
	}
	logfileLine("ActivateNATContainer",cPublicNATIP);
	
	char cPrivateNATNetwork[256]={""};
	GetNodeProp(uNode,"cPrivateNATNetwork",cPrivateNATNetwork);
	if(!cPrivateNATNetwork[0])
	{
		logfileLine("ActivateNATContainer","No cPrivateNATNetwork");
		tJobErrorUpdate(uJob,"No cPrivateNATNetwork");
		return;
	}
	logfileLine("ActivateNATContainer",cPrivateNATNetwork);

	//Only run after the related node script has run
	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uJobStatus=1"
			" AND uNode=%u"
			" AND cJobName='ActivateNATNode'",uNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ActivateNATContainer",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"mysql error 1");
		return;
	}
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
		logfileLine("ActivateNATContainer","waiting for ActivateNATNode");
		tJobWaitingUpdate(uJob);
		mysql_free_result(res);
		return;
	}
	mysql_free_result(res);

	//Only run once the container is running
	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uJobStatus=1"
			" AND uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ActivateNATContainer",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"mysql error 1");
		return;
	}
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
		logfileLine("ActivateNATContainer","waiting for container");
		tJobWaitingUpdate(uJob);
		mysql_free_result(res);
		return;
	}
	mysql_free_result(res);

	//Execute node script
	//Optional group based script may exist to be executed.
	//
	//Primary group is oldest tGroupGlue entry.
	//UBC safe
	sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
			" AND tProperty.uKey=tGroupGlue.uGroup"
			" AND tGroupGlue.uContainer=%u"
			" AND tProperty.cName='cJob_ActivateNATScript' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",uPROP_GROUP,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ActivateNATContainer",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"mysql error 2");
		return;
	}
        res=mysql_store_result(&gMysql);
	char cCommand[256];
	//default command setup	
	//The args are not used currently by the iptables and change freepbx command of unxsNAT
	//	but we add them anyway for future extensions.
	sprintf(cCommand,"/usr/sbin/ActivateNATContainer.sh");
	if((field=mysql_fetch_row(res)))
		sprintf(cCommand,"%.255s",field[0]);
	mysql_free_result(res);

	//Remove trailing junk
	char *cp;
	if((cp=strchr(cCommand,'\n')) || (cp=strchr(cCommand,'\r'))) *cp=0;

	if(uNotValidSystemCallArg(cCommand))
	{
		logfileLine("ActivateNATContainer","cJob_ActivateNATScript security alert");
		tJobErrorUpdate(uJob,"security violation 1");
		return;
	}

	//Only run if command is chmod 500 and owned by root for extra security reasons.
	struct stat statInfo;
	if(stat(cCommand,&statInfo))
	{
		logfileLine("ActivateNATContainer","stat failed for cJob_ActivateNATScript");
		logfileLine("ActivateNATContainer",cCommand);
		tJobErrorUpdate(uJob,"security violation 2");
		return;
	}
	if(statInfo.st_uid!=0)
	{
		logfileLine("ActivateNATContainer","cJob_ActivateNATScript is not owned by root");
		tJobErrorUpdate(uJob,"security violation 3");
		return;
	}
	if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
	{
		logfileLine("ActivateNATContainer","cJob_ActivateNATScript is not chmod 500");
		tJobErrorUpdate(uJob,"security violation 4");
		return;
	}

	char cOnScriptCall[512];
	sprintf(cOnScriptCall,"%.255s %u %s %s",cCommand,uContainer,cPublicNATIP,cPrivateNATNetwork);
	logfileLine("ActivateNATContainer",cOnScriptCall);
	if(system(cOnScriptCall))
	{
		logfileLine("ActivateNATContainer","cOnScriptCall error");
		tJobErrorUpdate(uJob,"cOnScriptCall error");
		return;
	}
	logfileLine("ActivateNATContainer","cOnScriptCall ok");

	//Everything ok
	SetContainerStatus(uContainer,uACTIVE);
	tJobDoneUpdate(uJob);

}//void ActivateNATContainer()


void ActivateNATNode(unsigned uJob,unsigned uContainer,unsigned uNode)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	//Roadmap
	//At group function in tcontainerfunc.h
	//	Check to make sure container is of correct type
	//	Check to make sure that required node properties exist
	//		"cPublicNATIP" and "cPrivateNATNetwork"
	//	Update container to private IP based on available rfc1918 IPs for the given node/datacenter
	//	Create a per each container job to run the FreePBX asterisk setup of the unxsNAT program.
	//		See ActivateNATContainer() above.
	//	Create this job once per node container.
	//Here:
	//	We verify security for script
	//	Create system command for script
	//	Execute the script
	//The script:
	//	CreateIptables part of unxsNAT program.
	//	Optionally run the reverse proxy setup of the unxsNAT program.
	//	Run other per hardware node items to be determined.

	//Only run after any pending container change IP jobs are finished
	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uJobStatus=1"
			" AND uNode=%u"
			" AND cJobName='ChangeIPContainer'",uNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ActivateNATNode",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"mysql error 11");
		return;
	}
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
		logfileLine("ActivateNATNode","waiting for ChangeIPContainer");
		tJobWaitingUpdate(uJob);
		mysql_free_result(res);
		return;
	}
	mysql_free_result(res);

	//Only run after any pending container change IP jobs are finished
	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uJobStatus=1"
			" AND uNode=%u"
			" AND cJobName='ChangeHostnameContainer'",uNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ActivateNATNode",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"mysql error 11");
		return;
	}
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
		logfileLine("ActivateNATNode","waiting for ChangeHostnameContainer");
		tJobWaitingUpdate(uJob);
		mysql_free_result(res);
		return;
	}
	mysql_free_result(res);

	//Gather NAT data
	char cPublicNATIP[256]={""};
	GetNodeProp(uNode,"cPublicNATIP",cPublicNATIP);
	if(!cPublicNATIP[0])
	{
		logfileLine("ActivateNATNode","No cPublicNATIP");
		tJobErrorUpdate(uJob,"No cPublicNATIP");
		return;
	}
	logfileLine("ActivateNATNode",cPublicNATIP);
	
	char cPrivateNATNetwork[256]={""};
	GetNodeProp(uNode,"cPrivateNATNetwork",cPrivateNATNetwork);
	if(!cPrivateNATNetwork[0])
	{
		logfileLine("ActivateNATNode","No cPrivateNATNetwork");
		tJobErrorUpdate(uJob,"No cPrivateNATNetwork");
		return;
	}
	logfileLine("ActivateNATNode",cPrivateNATNetwork);

	//Execute node script
	//Optional group based script may exist to be executed.
	//
	//Primary group is oldest tGroupGlue entry.
	//UBC safe
	sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tGroupGlue WHERE tProperty.uType=%u"
			" AND tProperty.uKey=tGroupGlue.uGroup"
			" AND tGroupGlue.uContainer=%u"
			" AND tProperty.cName='cJob_ActivateNATNodeScript' ORDER BY tGroupGlue.uGroupGlue LIMIT 1",uPROP_GROUP,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ActivateNATNode",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"mysql error 3");
		return;
	}
        res=mysql_store_result(&gMysql);
	char cCommand[256];
	//default command setup	
	//The args are not used currently by the iptables and change freepbx command of unxsNAT
	//	but we add them anyway for future extensions.
	sprintf(cCommand,"/usr/sbin/ActivateNATNode.sh");
	if((field=mysql_fetch_row(res)))
		sprintf(cCommand,"%.255s",field[0]);
	mysql_free_result(res);

	//Remove trailing junk
	char *cp;
	if((cp=strchr(cCommand,'\n')) || (cp=strchr(cCommand,'\r'))) *cp=0;

	if(uNotValidSystemCallArg(cCommand))
	{
		logfileLine("ActivateNATNode","cJob_ActivateNATNodeScript security alert");
		tJobErrorUpdate(uJob,"security violation 1");
		return;
	}

	//Only run if command is chmod 500 and owned by root for extra security reasons.
	struct stat statInfo;
	if(stat(cCommand,&statInfo))
	{
		logfileLine("ActivateNATNode","stat failed for cJob_ActivateNATNodeScript");
		logfileLine("ActivateNATNode",cCommand);
		tJobErrorUpdate(uJob,"security violation 2");
		return;
	}
	if(statInfo.st_uid!=0)
	{
		logfileLine("ActivateNATNode","cJob_ActivateNATNodeScript is not owned by root");
		tJobErrorUpdate(uJob,"security violation 3");
		return;
	}
	if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
	{
		logfileLine("ActivateNATNode","cJob_ActivateNATNodeScript is not chmod 500");
		tJobErrorUpdate(uJob,"security violation 4");
		return;
	}

	char cOnScriptCall[512];
	//Eventhough supplied cPrivateNATNetwork not used by script at this time
	sprintf(cOnScriptCall,"%.255s %s %s",cCommand,cPublicNATIP,cPrivateNATNetwork);
	logfileLine("ActivateNATNode",cOnScriptCall);
	if(system(cOnScriptCall))
	{
		logfileLine("ActivateNATNode","cOnScriptCall error");
		tJobErrorUpdate(uJob,"cOnScriptCall error");
		return;
	}
	logfileLine("ActivateNATNode","cOnScriptCall ok");

	//Everything ok
	SetContainerStatus(uContainer,uACTIVE);
	tJobDoneUpdate(uJob);

}//void ActivateNATNode()


//get real vz container status
unsigned uContainerStatus(unsigned uContainer)
{
	FILE *pfp;
	char cResponse[16]={""};
	unsigned uStatus=0;//unknown

	sprintf(gcQuery,"/usr/sbin/vzlist --no-header --output status %u",uContainer);
	if((pfp=popen(gcQuery,"r"))==NULL)
	{
		logfileLine("uContainerStatus","open error");
		return(0);
	}
	if(fscanf(pfp,"%15s",cResponse)<0)
	{
		logfileLine("uContainerStatus","read error");
		return(0);
	}
	if(pclose(pfp)<0)
		logfileLine("uContainerStatus","pclose error");

	logfileLine("uContainerStatus",cResponse);
	if(strstr(cResponse,"run"))
		uStatus=1;
	else
		uStatus=31;
	return(uStatus);

}//unsigned uContainerStatus(unsigned uContainer)


void BlockAccess(unsigned uJob,const char *cJobData,unsigned uDatacenter,unsigned uNode)
{
	char cIPv4[32]={""};
	char *cp;

	sscanf(cJobData,"cIPv4=%31s;",cIPv4);
	if((cp=strchr(cIPv4,';')))
		*cp=0;
	if(!cIPv4[0])
	{
		logfileLine("BlockAccess","Could not determine cIPv4");
		tJobErrorUpdate(uJob,"cIPv4[0]==0");
		return;
	}

        MYSQL_RES *res;
        MYSQL_ROW field;

	//remove any ACCEPT then DROP
	char cTemplate[512]={	
				"/sbin/iptables -L -n | grep -w %1$s | grep -w ACCEPT > /dev/null; if [ $? == 0 ];then"
					" /sbin/iptables -D FORWARD -s %1$s -j ACCEPT > /dev/null 2>&1;"
				" fi;"
				"/sbin/iptables -L -n | grep -w %1$s > /dev/null; if [ $? != 0 ];then"
					" /sbin/iptables -I FORWARD -s %1$s -j DROP;"
				" fi;"
					};

	FILE *fp;
	char cPrivateKey[256]={""};
	if((fp=fopen("/etc/unxsvz/unxsvz.key","r")))
	{
		if(fgets(cPrivateKey,255,fp)!=NULL)
			cPrivateKey[strlen(cPrivateKey)-1]=0;//cut off /n
		fclose(fp);
	}

	if(cPrivateKey[0])
	{
		sprintf(gcQuery,"SELECT cComment FROM tConfiguration"
			//trick to get most specific datacenter node combo
			" WHERE SHA1(CONCAT(LEFT(cComment,LOCATE('#unxsVZKey=',cComment)),'%1$s'))=SUBSTR(cComment,LOCATE('#unxsVZKey=',cComment)+11)"
			" AND ((uDatacenter=%2$u AND uNode=%3$u) OR (uDatacenter=%2$u AND uNode=0))"
			" AND cLabel='cBlockAccessTemplate' AND cValue='cComment' ORDER BY uNode DESC LIMIT 1",cPrivateKey,uDatacenter,uNode);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			logfileLine("BlockAccess",mysql_error(&gMysql));
		else
		{
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(cTemplate,"%.512s",field[0]);
				logfileLine("BlockAccess","secure template used");
			}
			mysql_free_result(res);
		}
	}

	//debug only
	//char cData[128];
	//sprintf(cData,"uDatacenter=%u uNode=%u",uDatacenter,uNode);
	//logfileLine("BlockAccess",cData);

	sprintf(gcQuery,cTemplate,cIPv4,cIPv4);
	if(system(gcQuery))
	{
		logfileLine("BlockAccess","iptables command failed");
		tJobErrorUpdate(uJob,"iptables command failed");
		return;
	}

	logfileLine("BlockAccess","iptables command ok");
	tJobDoneUpdate(uJob);

	//
	//Multiple related job control system
	unsigned uMasterJob=0;
	unsigned uAllJobsDone=1;
	sscanf(ForeignKey("tJob","uMasterJob",uJob),"%u",&uMasterJob);
	if(!uMasterJob)
		uMasterJob=uJob;
	sprintf(gcQuery,"SELECT uJob FROM tJob"
			" WHERE (uMasterJob=%u OR uJob=%u)"
			" AND uJobStatus!=%u",
				uMasterJob,uMasterJob,uDONEOK);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		logfileLine("BlockAccess",mysql_error(&gMysql));
	else
	{
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			uAllJobsDone=0;
		mysql_free_result(res);
	}
	if(uAllJobsDone)
		UpdateIPFWStatus(cIPv4,uFWBLOCKED);
	//end
	return;

}//void BlockAccess()


void UndoBlockAccess(unsigned uJob,const char *cJobData,unsigned uDatacenter,unsigned uNode)
{
	char cIPv4[32]={""};
	char *cp;

	sscanf(cJobData,"cIPv4=%31s;",cIPv4);
	if((cp=strchr(cIPv4,';')))
		*cp=0;
	if(!cIPv4[0])
	{
		logfileLine("UndoBlockAccess","Could not determine cIPv4");
		tJobErrorUpdate(uJob,"cIPv4[0]==0");
		return;
	}

        MYSQL_RES *res;
        MYSQL_ROW field;

	//remove DROP but add ACCEPT for accounting
	char cTemplate[512]={
			"/sbin/iptables -L -n | grep -w %1$s | grep -w DROP > /dev/null; if [ $? == 0 ];then"
				" /sbin/iptables -D FORWARD -s %1$s -j DROP;"
			" fi;"
			"/sbin/iptables -L -n | grep -w %1$s > /dev/null; if [ $? != 0 ];then"
				" /sbin/iptables -I FORWARD -s %1$s -j ACCEPT;"
			" fi;"
									};

	FILE *fp;
	char cPrivateKey[256]={""};
	if((fp=fopen("/etc/unxsvz/unxsvz.key","r")))
	{
		if(fgets(cPrivateKey,255,fp)!=NULL)
			cPrivateKey[strlen(cPrivateKey)-1]=0;//cut off /n
		fclose(fp);
	}

	if(cPrivateKey[0])
	{
		sprintf(gcQuery,"SELECT cComment FROM tConfiguration"
			//trick to get most specific datacenter node combo
			" WHERE SHA1(CONCAT(LEFT(cComment,LOCATE('#unxsVZKey=',cComment)),'%1$s'))=SUBSTR(cComment,LOCATE('#unxsVZKey=',cComment)+11)"
			" AND ((uDatacenter=%2$u AND uNode=%3$u) OR (uDatacenter=%2$u AND uNode=0))"
			" AND cLabel='cUndoBlockAccessTemplate' AND cValue='cComment' ORDER BY uNode DESC LIMIT 1",cPrivateKey,uDatacenter,uNode);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			logfileLine("UndoBlockAccess",mysql_error(&gMysql));
		else
		{
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(cTemplate,"%.512s",field[0]);
				logfileLine("UndoBlockAccess","secure template used");
			}
			mysql_free_result(res);
		}
	}

	//debug only
	//char cData[128];
	//sprintf(cData,"uDatacenter=%u uNode=%u",uDatacenter,uNode);
	//logfileLine("UndoBlockAccess",cData);

	sprintf(gcQuery,cTemplate,cIPv4,cIPv4);
	if(system(gcQuery))
	{
		logfileLine("UndoBlockAccess","iptables command failed");
		tJobErrorUpdate(uJob,"iptables command failed");
		return;
	}

	logfileLine("UndoBlockAccess","iptables command ok");
	tJobDoneUpdate(uJob);

	//
	//Multiple related job control system
	unsigned uMasterJob=0;
	unsigned uAllJobsDone=1;
	sscanf(ForeignKey("tJob","uMasterJob",uJob),"%u",&uMasterJob);
	if(!uMasterJob)
		uMasterJob=uJob;
	sprintf(gcQuery,"SELECT uJob FROM tJob"
			" WHERE (uMasterJob=%u OR uJob=%u)"
			" AND uJobStatus!=%u",
				uMasterJob,uMasterJob,uDONEOK);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		logfileLine("UndoBlockAccess",mysql_error(&gMysql));
	else
	{
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			uAllJobsDone=0;
		mysql_free_result(res);
	}
	if(uAllJobsDone)
		UpdateIPFWStatus(cIPv4,uFWACCESS);
	//end
	return;

}//void UndoBlockAccess()


//All port access used by unxsSnort only at this time.
void AllowAllAccess(unsigned uJob,const char *cJobData,unsigned uDatacenter,unsigned uNode)
{
	char cIPv4[32]={""};
	char *cp;

	sscanf(cJobData,"cIPv4=%31s;",cIPv4);
	if((cp=strchr(cIPv4,';')))
		*cp=0;
	if(!cIPv4[0])
	{
		logfileLine("AllowAllAccess","Could not determine cIPv4");
		tJobErrorUpdate(uJob,"cIPv4[0]==0");
		return;
	}

        MYSQL_RES *res;
        MYSQL_ROW field;

	//add ACCEPT but try to remove DROP if it exists
	char cTemplate[512]={
				"/sbin/iptables -L -n | grep -w %1$s | grep -w DROP > /dev/null; if [ $? == 0 ];then"
					" /sbin/iptables -D FORWARD -s %1$s -j DROP;"
				" fi;"
				"/sbin/iptables -L -n | grep -w %1$s > /dev/null; if [ $? != 0 ];then"
					" /sbin/iptables -I FORWARD -s %1$s -j ACCEPT;"
				" fi;"
							};

	FILE *fp;
	char cPrivateKey[256]={""};
	if((fp=fopen("/etc/unxsvz/unxsvz.key","r")))
	{
		if(fgets(cPrivateKey,255,fp)!=NULL)
			cPrivateKey[strlen(cPrivateKey)-1]=0;//cut off /n
		fclose(fp);
	}

	if(cPrivateKey[0])
	{
		sprintf(gcQuery,"SELECT cComment FROM tConfiguration"
			//trick to get most specific datacenter node combo
			" WHERE SHA1(CONCAT(LEFT(cComment,LOCATE('#unxsVZKey=',cComment)),'%1$s'))=SUBSTR(cComment,LOCATE('#unxsVZKey=',cComment)+11)"
			" AND ((uDatacenter=%2$u AND uNode=%3$u) OR (uDatacenter=%2$u AND uNode=0))"
			" AND cLabel='cAllowAllAccessTemplate' AND cValue='cComment' ORDER BY uNode DESC LIMIT 1",cPrivateKey,uDatacenter,uNode);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			logfileLine("AllowAllAccess",mysql_error(&gMysql));
		else
		{
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(cTemplate,"%.512s",field[0]);
				logfileLine("AllowAllAccess","secure template used");
			}
			mysql_free_result(res);
		}
	}

	//debug only
	//char cData[128];
	//sprintf(cData,"uDatacenter=%u uNode=%u",uDatacenter,uNode);
	//logfileLine("AllowAllAccess",cData);

	sprintf(gcQuery,cTemplate,cIPv4,cIPv4);
	if(system(gcQuery))
	{
		logfileLine("AllowAllAccess","iptables command failed");
		tJobErrorUpdate(uJob,"iptables command failed");
		return;
	}

	logfileLine("AllowAllAccess","iptables command ok");
	tJobDoneUpdate(uJob);

	//
	//Multiple related job control system
	unsigned uMasterJob=0;
	unsigned uAllJobsDone=1;
	sscanf(ForeignKey("tJob","uMasterJob",uJob),"%u",&uMasterJob);
	if(!uMasterJob)
		uMasterJob=uJob;
	sprintf(gcQuery,"SELECT uJob FROM tJob"
			" WHERE (uMasterJob=%u OR uJob=%u)"
			" AND uJobStatus!=%u",
				uMasterJob,uMasterJob,uDONEOK);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		logfileLine("AllowAllAccess",mysql_error(&gMysql));
	else
	{
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			uAllJobsDone=0;
		mysql_free_result(res);
	}
	if(uAllJobsDone)
		UpdateIPFWStatus(cIPv4,uFWACCESS);
	//end

	return;

}//void AllowAllAccess()


void UpdateIPFWStatus(const char *cIPv4,unsigned uFWStatus)
{
	sprintf(gcQuery,"UPDATE tIP"
		" SET uModDate=UNIX_TIMESTAMP(NOW()),"
		" uFWStatus=%u"
		" WHERE uIPNum=INET_ATON('%s')",uFWStatus,cIPv4);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		logfileLine("UpdateIPFWStatus",mysql_error(&gMysql));
	else if(mysql_affected_rows(&gMysql)!=1)
		logfileLine("UpdateIPFWStatus","mysql_affected_rows()!=1");

}//void UpdateIPFWStatus(const char *cIPv4,unsigned uFWACCESS)


void RemoveAcceptFromIPTables(unsigned uJob,const char *cJobData,unsigned uDatacenter,unsigned uNode)
{
	char cIPv4[32]={""};
	char *cp;

	sscanf(cJobData,"cIPv4=%31s;",cIPv4);
	if((cp=strchr(cIPv4,';')))
		*cp=0;
	if(!cIPv4[0])
	{
		logfileLine("RemoveAcceptFromIPTables","Could not determine cIPv4");
		tJobErrorUpdate(uJob,"cIPv4[0]==0");
		return;
	}

        MYSQL_RES *res;
        MYSQL_ROW field;

	//remove DROP and/or ACCEPT
	char cTemplate[512]={
			"/sbin/iptables -L -n | grep -w %1$s | grep -w ACCEPT > /dev/null; if [ $? == 0 ];then"
				" /sbin/iptables -D FORWARD -s %1$s -j ACCEPT;"
			" fi;"
									};

	FILE *fp;
	char cPrivateKey[256]={""};
	if((fp=fopen("/etc/unxsvz/unxsvz.key","r")))
	{
		if(fgets(cPrivateKey,255,fp)!=NULL)
			cPrivateKey[strlen(cPrivateKey)-1]=0;//cut off /n
		fclose(fp);
	}

	if(cPrivateKey[0])
	{
		sprintf(gcQuery,"SELECT cComment FROM tConfiguration"
			//trick to get most specific datacenter node combo
			" WHERE SHA1(CONCAT(LEFT(cComment,LOCATE('#unxsVZKey=',cComment)),'%1$s'))=SUBSTR(cComment,LOCATE('#unxsVZKey=',cComment)+11)"
			" AND ((uDatacenter=%2$u AND uNode=%3$u) OR (uDatacenter=%2$u AND uNode=0))"
			" AND cLabel='cRemoveAcceptFromIPTablesTemplate' AND cValue='cComment' ORDER BY uNode DESC LIMIT 1",cPrivateKey,uDatacenter,uNode);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			logfileLine("RemoveAcceptFromIPTables",mysql_error(&gMysql));
		else
		{
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(cTemplate,"%.512s",field[0]);
				logfileLine("RemoveAcceptFromIPTables","secure template used");
			}
			mysql_free_result(res);
		}
	}

	sprintf(gcQuery,cTemplate,cIPv4,cIPv4);
	if(system(gcQuery))
	{
		logfileLine("RemoveAcceptFromIPTables","iptables command failed");
		logfileLine("RemoveAcceptFromIPTables",cIPv4);
		tJobErrorUpdate(uJob,"iptables command failed");
		return;
	}

	logfileLine("RemoveAcceptFromIPTables","iptables command ok");
	tJobDoneUpdate(uJob);

	//
	//Multiple related job control system
	unsigned uMasterJob=0;
	unsigned uAllJobsDone=1;
	sscanf(ForeignKey("tJob","uMasterJob",uJob),"%u",&uMasterJob);
	if(!uMasterJob)
		uMasterJob=uJob;
	sprintf(gcQuery,"SELECT uJob FROM tJob"
			" WHERE (uMasterJob=%u OR uJob=%u)"
			" AND uJobStatus!=%u",
				uMasterJob,uMasterJob,uDONEOK);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		logfileLine("RemoveAcceptFromIPTables",mysql_error(&gMysql));
	else
	{
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			uAllJobsDone=0;
		mysql_free_result(res);
	}
	if(uAllJobsDone)
		UpdateIPFWStatus(cIPv4,uFWREMOVED);
	//end
	return;

}//void RemoveAcceptFromIPTables()


void RemoveDropFromIPTables(unsigned uJob,const char *cJobData,unsigned uDatacenter,unsigned uNode)
{
	char cIPv4[32]={""};
	char *cp;

	sscanf(cJobData,"cIPv4=%31s;",cIPv4);
	if((cp=strchr(cIPv4,';')))
		*cp=0;
	if(!cIPv4[0])
	{
		logfileLine("RemoveDropFromIPTables","Could not determine cIPv4");
		tJobErrorUpdate(uJob,"cIPv4[0]==0");
		return;
	}

        MYSQL_RES *res;
        MYSQL_ROW field;

	//remove DROP and/or ACCEPT
	char cTemplate[512]={
			"/sbin/iptables -L -n | grep -w %1$s | grep -w DROP > /dev/null; if [ $? == 0 ];then"
				" /sbin/iptables -D FORWARD -s %1$s -j DROP;"
			" fi;"
									};

	FILE *fp;
	char cPrivateKey[256]={""};
	if((fp=fopen("/etc/unxsvz/unxsvz.key","r")))
	{
		if(fgets(cPrivateKey,255,fp)!=NULL)
			cPrivateKey[strlen(cPrivateKey)-1]=0;//cut off /n
		fclose(fp);
	}

	if(cPrivateKey[0])
	{
		sprintf(gcQuery,"SELECT cComment FROM tConfiguration"
			//trick to get most specific datacenter node combo
			" WHERE SHA1(CONCAT(LEFT(cComment,LOCATE('#unxsVZKey=',cComment)),'%1$s'))=SUBSTR(cComment,LOCATE('#unxsVZKey=',cComment)+11)"
			" AND ((uDatacenter=%2$u AND uNode=%3$u) OR (uDatacenter=%2$u AND uNode=0))"
			" AND cLabel='cRemoveDropFromIPTablesTemplate' AND cValue='cComment' ORDER BY uNode DESC LIMIT 1",cPrivateKey,uDatacenter,uNode);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			logfileLine("RemoveDropFromIPTables",mysql_error(&gMysql));
		else
		{
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(cTemplate,"%.512s",field[0]);
				logfileLine("RemoveDropFromIPTables","secure template used");
			}
			mysql_free_result(res);
		}
	}

	sprintf(gcQuery,cTemplate,cIPv4,cIPv4);
	if(system(gcQuery))
	{
		logfileLine("RemoveDropFromIPTables","iptables command failed");
		tJobErrorUpdate(uJob,"iptables command failed");
		return;
	}

	logfileLine("RemoveDropFromIPTables","iptables command ok");
	tJobDoneUpdate(uJob);

	//
	//Multiple related job control system
	unsigned uMasterJob=0;
	unsigned uAllJobsDone=1;
	sscanf(ForeignKey("tJob","uMasterJob",uJob),"%u",&uMasterJob);
	if(!uMasterJob)
		uMasterJob=uJob;
	sprintf(gcQuery,"SELECT uJob FROM tJob"
			" WHERE (uMasterJob=%u OR uJob=%u)"
			" AND uJobStatus!=%u",
				uMasterJob,uMasterJob,uDONEOK);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		logfileLine("RemoveDropFromIPTables",mysql_error(&gMysql));
	else
	{
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			uAllJobsDone=0;
		mysql_free_result(res);
	}
	if(uAllJobsDone)
		UpdateIPFWStatus(cIPv4,uFWREMOVED);
	//end
	return;

}//void RemoveDropFromIPTables()
