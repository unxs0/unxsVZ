/*
FILE
	jobqueue.c
	$Id$
PURPOSE
	Command line processing of jobs in the tJob queue.
AUTHOR/LEGAL
	(C) 2008-2010 Gary Wallis for Unxiservice, LLC.
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
void ChangeHostnameContainer(unsigned uJob,unsigned uContainer);
void StopContainer(unsigned uJob,unsigned uContainer);
void StartContainer(unsigned uJob,unsigned uContainer);
void MigrateContainer(unsigned uJob,unsigned uContainer,char *cJobData);
void GetGroupProp(const unsigned uGroup,const char *cName,char *cValue);
void GetContainerProp(const unsigned uContainer,const char *cName,char *cValue);
void UpdateContainerUBC(unsigned uJob,unsigned uContainer,const char *cJobData);
void SetContainerUBC(unsigned uJob,unsigned uContainer,const char *cJobData);
void TemplateContainer(unsigned uJob,unsigned uContainer,const char *cJobData);
void ActionScripts(unsigned uJob,unsigned uContainer);
void CloneContainer(unsigned uJob,unsigned uContainer,char *cJobData);
void AppFunctions(FILE *fp,char *cFunction);
void LocalImportTemplate(unsigned uJob,unsigned uDatacenter,const char *cJobData);
void LocalImportConfig(unsigned uJob,unsigned uDatacenter,const char *cJobData);
void FailoverTo(unsigned uJob,unsigned uContainer,const char *cJobData);
void FailoverFrom(unsigned uJob,unsigned uContainer,const char *cJobData);
void GetIPFromtIP(const unsigned uIPv4,char *cIP);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);
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
unsigned SetContainerProperty(const unsigned uContainer,const char *cPropertyName,const  char *cPropertyValue);
unsigned FailToJobDone(unsigned uJob);
unsigned ProcessCloneSyncJob(unsigned uNode,unsigned uContainer,unsigned uRemoteContainer);

int CreateActionScripts(unsigned uContainer, unsigned uOverwrite);

//extern protos
void TextConnectDb(void); //main.c
void SetContainerStatus(unsigned uContainer,unsigned uStatus);
void SetContainerNode(unsigned uContainer,unsigned uNode);
void GetConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer,
		unsigned uHtml);

//file scoped vars.
static unsigned gfuNode=0;
static unsigned gfuDatacenter=0;
static unsigned guDebug=0;
static char cHostname[100]={""};//file scope
static FILE *gLfp=NULL;//log file


//Using the local server hostname get max 100 jobs for this node from the tJob queue.
//Then dispatch jobs via ProcessJob() this function in turn calls specific functions for
//each known cJobName.
void ProcessJobQueue(unsigned uDebug)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uDatacenter=0;
	unsigned uNode=0;
	unsigned uContainer=0;
	unsigned uRemoteContainer=0;
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
#define LINUX_SYSINFO_LOADS_SCALE 65536
#define JOBQUEUE_MAXLOAD 20 //This is equivalent to uptime 20.00 last 1 min avg load
	if(structSysinfo.loads[0]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
	{
		logfileLine("ProcessJobQueue","structSysinfo.loads[0] larger than JOBQUEUE_MAXLOAD");
		exit(1);
	}
	//debug only
	//printf("structSysinfo.loads[0]/65536.0=%2.2f\n",(float)structSysinfo.loads[0]/(float)LINUX_SYSINFO_LOADS_SCALE);
	//exit(0);

	TextConnectDb();//Uses login data from local.h
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

	//Special recurring jobs based on special containers
	//Main loop normal jobs
	//1-. Maintain clone containers
	//We need to rsync from running container to clone container
	//where the source container is running on this node
	//and the target node is a remote node.
	//1 uACTIVE
	//31 uSTOPPED
	sprintf(gcQuery,"SELECT uSource,uContainer FROM tContainer WHERE uSource>0 AND"
			" uDatacenter=%u AND (uStatus=1 OR uStatus=31)",uDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessJobQueue",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		//uSource==uContainer has to be on this node. We defer that determination
		//to ProcessCloneSyncJob()
		sscanf(field[0],"%u",&uContainer);
		sscanf(field[1],"%u",&uRemoteContainer);
		if((uError=ProcessCloneSyncJob(uNode,uContainer,uRemoteContainer)))
			LogError("ProcessCloneSyncJob()",uError);
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
	sprintf(gcQuery,"SELECT uJob,uContainer,cJobName,cJobData FROM tJob WHERE uJobStatus=1"
				" AND uDatacenter=%u AND uNode=%u"
				" AND uJobDate<=UNIX_TIMESTAMP(NOW()) ORDER BY uJob LIMIT 100",
						uDatacenter,uNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessJobQueue",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
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

	if(guDebug) logfileLine("ProcessJobQueue","End");
	fclose(gLfp);
	mysql_close(&gMysql);
	exit(0);

}//void ProcessJobQueue()



void ProcessJob(unsigned uJob,unsigned uDatacenter,unsigned uNode,
		unsigned uContainer,char *cJobName,char *cJobData)
{
	//static unsigned uCount=0;
	gfuNode=uNode;
	gfuDatacenter=uDatacenter;

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
	if(!strcmp(cJobName,"FailoverTo"))
	{
		FailoverTo(uJob,uContainer,cJobData);
	}
	else if(!strcmp(cJobName,"FailoverFrom"))
	{
		FailoverFrom(uJob,uContainer,cJobData);
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
	else if(!strcmp(cJobName,"ChangeHostnameContainer"))
	{
		ChangeHostnameContainer(uJob,uContainer);
	}
	else if(!strcmp(cJobName,"ChangeIPContainer"))
	{
		ChangeIPContainer(uJob,uContainer,cJobData);
	}
	else if(!strcmp(cJobName,"StopContainer"))
	{
		StopContainer(uJob,uContainer);
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
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --nameserver \"%.99s\" --searchdomain %.32s --save",
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
		}
		else
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
		FILE *pp;

		sprintf(gcQuery,"/usr/sbin/vzctl exec %u -",uContainer);
		if((pp=popen(gcQuery,"w"))!=NULL)
		{
			fprintf(pp,"passwd\n");
			fprintf(pp,"%s\n",cPasswd);
			fprintf(pp,"%s\n",cPasswd);
			pclose(pp);
			logfileLine("NewContainer","Container passwd changed");
		}
	}

	//Everything went ok;
	SetContainerStatus(uContainer,1);//Active
	tJobDoneUpdate(uJob);

//In this case the goto MIGHT be justified
CommonExit:
	mysql_free_result(res);


}//void NewContainer(...)


void DestroyContainer(unsigned uJob,unsigned uContainer)
{

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u",uContainer);
	if(system(gcQuery))
	{
		logfileLine("DestroyContainer",gcQuery);
		tJobErrorUpdate(uJob,"vzctl stop failed");
		return;
	}

	//2-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose destroy %u",uContainer);
	if(system(gcQuery))
	{
		logfileLine("DestroyContainer",gcQuery);
		tJobErrorUpdate(uJob,"vzctl destroy failed");

		//Attempt roll back
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
		if(system(gcQuery))
		{
			logfileLine("DestroyContainer",gcQuery);
			tJobErrorUpdate(uJob,"vzctl start rollback failed");
		}
		return;
	}

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
		sprintf(gcQuery,"UPDATE tIP SET uAvailable=1 WHERE cLabel='%.31s'",cIPOld);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ChangeIPContainer",mysql_error(&gMysql));
			tJobErrorUpdate(uJob,"Release old IP from tIP failed");
			goto CommonExit;
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


void ChangeHostnameContainer(unsigned uJob,unsigned uContainer)
{
	char cHostname[100]={""};
	char cName[100]={""};
        MYSQL_RES *res;
        MYSQL_ROW field;

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
		SetContainerStatus(uContainer,1);//Active
		tJobDoneUpdate(uJob);
	}
	else
	{
		logfileLine("ChangeHostnameContainer",gcQuery);
		tJobErrorUpdate(uJob,"No line from query");
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


	//Everything ok
	SetContainerStatus(uContainer,uACTIVE);
	tJobDoneUpdate(uJob);

}//void StartContainer(...)


void MigrateContainer(unsigned uJob,unsigned uContainer,char *cJobData)
{
	char cTargetNodeIPv4[256]={""};
	unsigned uTargetNode=0;

	sscanf(cJobData,"uTargetNode=%u;",&uTargetNode);
	if(!uTargetNode)
	{
		logfileLine("MigrateContainer","Could not determine uTargetNode");
		tJobErrorUpdate(uJob,"uTargetNode==0");
		return;
	}

	GetNodeProp(uTargetNode,"cIPv4",cTargetNodeIPv4);
	if(!cTargetNodeIPv4[0])
	{
		logfileLine("MigrateContainer","Could not determine tProperty.cTargetNodeIPv4");
		tJobErrorUpdate(uJob,"tProperty.cTargetNodeIPv4");
		return;
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
	if(!cSCPOptions[0] || uNotValidSystemCallArg(cSCPOptions))
		sprintf(cSCPOptions,"-P 22 -c arcfour");

	if(cSSHOptions[0])
		sprintf(gcQuery,"export PATH=/usr/sbin:/usr/bin:/bin:/usr/local/bin:/usr/local/sbin;"
				"/usr/sbin/vzmigrate --ssh=\"%s\" --keep-dst --online -v %s %u",
					cSSHOptions,cTargetNodeIPv4,uContainer);
	else
		sprintf(gcQuery,"export PATH=/usr/sbin:/usr/bin:/bin;"
				"/usr/sbin/vzmigrate --keep-dst --online -v %s %u",
					cTargetNodeIPv4,uContainer);
	if(system(gcQuery))
	{
		//We may not want this optional behavior may violate QoS for given migration
		logfileLine("MigrateContainer","Trying offline migration (check kernel compat)");
		tJobErrorUpdate(uJob,"Live failed trying offline");

		if(cSSHOptions[0])
			sprintf(gcQuery,"export PATH=/usr/sbin:/usr/bin:/bin:/usr/local/bin:/usr/local/sbin;"
				"/usr/sbin/vzmigrate --ssh=\"%s\" -v %s %u",
					cSSHOptions,cTargetNodeIPv4,uContainer);
		else
			sprintf(gcQuery,"export PATH=/usr/sbin:/usr/bin:/bin:/usr/local/bin:/usr/local/sbin;"
				"/usr/sbin/vzmigrate -v %s %u",
					cTargetNodeIPv4,uContainer);
		if(system(gcQuery))
		{
			logfileLine("MigrateContainer","Giving up!");
			tJobErrorUpdate(uJob,"vzmigrate on/off-line failed");
			return;
		}
	}

	//If container rrd files exists in our standard fixed dir cp to new node
	//Note that cleanup maybe required in the future
	sprintf(gcQuery,"/var/lib/rrd/%u.rrd",uContainer);
	if(!access(gcQuery,R_OK))
	{
		sprintf(gcQuery,"/usr/bin/scp %s /var/lib/rrd/%u.rrd %s:/var/lib/rrd/",
			cSCPOptions,uContainer,cTargetNodeIPv4);
		if(system(gcQuery))
			logfileLine("MigrateContainer",gcQuery);
	}

	//Everything ok
	SetContainerStatus(uContainer,1);//Active
	SetContainerNode(uContainer,uTargetNode);//Migrated!
	tJobDoneUpdate(uJob);

}//void MigrateContainer(...)




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
	GetContainerProp(uContainer,cUBC,cBuf);
	sscanf(cBuf,"%f",&luBar);

	sprintf(cUBC,"%.32s.luLimit",cResource);
	GetContainerProp(uContainer,cUBC,cBuf);
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
	char cOSTemplateBase[68]={"centos-5-x86_64"};
	char *cp;
	register int i;
	struct stat statInfo;
	unsigned uOwner=1;
	unsigned uCreatedBy=1;

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

	sprintf(gcQuery,"SELECT tOSTemplate.cLabel FROM tContainer,tOSTemplate"
			" WHERE tContainer.uOSTemplate=tOSTemplate.uOSTemplate AND"
			" tContainer.uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("TemplateContainer",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tOSTemplate.cLabel");
		goto CommonExit;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cOSTemplateBase,"%.67s",field[0]);
	mysql_free_result(res);
	if(!cOSTemplateBase[0])
	{
		logfileLine("TemplateContainer","Could not determine cOSTemplateBase");
		tJobErrorUpdate(uJob,"cOSTemplateBase[0]==0");
		goto CommonExit;
	}

	//Remove ending dash part if any
	for(i=strlen(cOSTemplateBase)-1;i>0;i--)
	{
		if(cOSTemplateBase[i]=='-')
		{
			cOSTemplateBase[i]=0;	
			break;
		}
	}
	//debug only
	//printf("cConfigLabel=%s; cOSTemplateBase=%s;\n",cConfigLabel,cOSTemplateBase);
	//exit(0);

	if(uNotValidSystemCallArg(cOSTemplateBase) || uNotValidSystemCallArg(cConfigLabel))
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

	//2-.	
	if(!cSnapshotDir[0])
		sprintf(gcQuery,"mv /vz/dump/vzdump-%u.tgz /vz/template/cache/%.67s-%.32s.tar.gz",
				uContainer,cOSTemplateBase,cConfigLabel);
	else
		sprintf(gcQuery,"mv %s/vzdump-%u.tgz /vz/template/cache/%.67s-%.32s.tar.gz",
				cSnapshotDir,uContainer,cOSTemplateBase,cConfigLabel);
	if(system(gcQuery))
	{
		logfileLine("TemplateContainer",gcQuery);
		tJobErrorUpdate(uJob,"mv tgz to cache tar.gz failed");
		goto CommonExit;
	}

	//2b-. md5sum generation
	if(!stat("/usr/bin/md5sum",&statInfo))
	{
		sprintf(gcQuery,"/usr/bin/md5sum /vz/template/cache/%.67s-%.32s.tar.gz >"
				" /vz/template/cache/%.67s-%.32s.tar.gz.md5sum",
			cOSTemplateBase,cConfigLabel,cOSTemplateBase,cConfigLabel);
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
		sprintf(gcQuery,"/usr/sbin/allnodescp.sh /vz/template/cache/%.67s-%.32s.tar.gz",
			cOSTemplateBase,cConfigLabel);
		if(system(gcQuery))
		{
			logfileLine("TemplateContainer",gcQuery);
			tJobErrorUpdate(uJob,"allnodescp.sh .tar.gz failed");
			goto CommonExit;
		}
		sprintf(gcQuery,"/usr/sbin/allnodescp.sh /vz/template/cache/%.67s-%.32s.tar.gz.md5sum",
			cOSTemplateBase,cConfigLabel);
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
		sprintf(gcQuery,"/usr/sbin/allnodecmd.sh \"/usr/bin/md5sum -c /vz/template/cache/%.67s-%.32s.tar.gz.md5sum\"",
			cOSTemplateBase,cConfigLabel);
		if(system(gcQuery))
		{
			logfileLine("TemplateContainer",gcQuery);
			tJobErrorUpdate(uJob,"allnodecmd.sh md5sum -c .tar.gz.md5sum failed");
			goto CommonExit;
		}
	}

	//4-. Make /etc/vz/conf tConfig file and scp to all nodes. ve-proxpop.conf-sample
	sprintf(gcQuery,"/bin/cp /etc/vz/conf/%u.conf /etc/vz/conf/ve-%.32s.conf-sample",
		uContainer,cConfigLabel);
	if(system(gcQuery))
	{
		logfileLine("TemplateContainer",gcQuery);
		tJobErrorUpdate(uJob,"cp conf container failed");
		goto CommonExit;
	}
	if(!stat("/usr/sbin/allnodescp.sh",&statInfo))
	{
		sprintf(gcQuery,"/usr/sbin/allnodescp.sh /etc/vz/conf/ve-%.32s.conf-sample",cConfigLabel);
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

	sprintf(gcQuery,"SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='%.67s-%.32s'"
							,cOSTemplateBase,cConfigLabel);
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
		sprintf(gcQuery,"INSERT tOSTemplate SET cLabel='%.67s-%.32s',uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",cOSTemplateBase,cConfigLabel,
									uOwner,uCreatedBy);
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
	sprintf(gcQuery,"SELECT uConfig FROM tConfig WHERE cLabel='%.32s'",cConfigLabel);
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
		sprintf(gcQuery,"INSERT tConfig SET cLabel='%.32s',uOwner=%u,uCreatedBy=%u,"
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
	SetContainerStatus(uContainer,1);//Active
	tJobDoneUpdate(uJob);

//This goto MAYBE ok
CommonExit:
	//6-. remove lock file
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
	char cSourceContainerIP[32]={""};
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

	sprintf(gcQuery,"SELECT tIP.cLabel FROM tIP,tContainer WHERE tIP.uIP=tContainer.uIPv4"
			" AND tContainer.uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("CloneContainer",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tOSTemplate.cLabel");
		goto CommonExit;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cSourceContainerIP,"%.31s",field[0]);
	mysql_free_result(res);
	if(!cSourceContainerIP[0])
	{
		logfileLine("CloneContainer","Could not determine cSourceContainerIP");
		tJobErrorUpdate(uJob,"No cSourceContainerIP");
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
	if(!cSSHOptions[0] || uNotValidSystemCallArg(cSSHOptions))
		sprintf(cSSHOptions,"-p 22 -c arcfour");

	char cSCPOptions[256]={""};
	GetConfiguration("cSCPOptions",cSCPOptions,gfuDatacenter,gfuNode,0,0);//First try node specific
	if(!cSCPOptions[0])
	{
		GetConfiguration("cSCPOptions",cSCPOptions,gfuDatacenter,0,0,0);//Second try datacenter wide
		if(!cSCPOptions[0])
			GetConfiguration("cSCPOptions",cSCPOptions,0,0,0,0);//Last try global
	}
	//Default for less conditions below
	if(!cSCPOptions[0] || uNotValidSystemCallArg(cSCPOptions))
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
	if(!cSnapshotDir[0])
		sprintf(gcQuery,"/usr/sbin/vzdump --compress --suspend %u",uContainer);
	else
		sprintf(gcQuery,"/usr/sbin/vzdump --compress --dumpdir %s --snapshot %u",
									cSnapshotDir,uContainer);
	if(system(gcQuery))
	{
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 1");
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
	if(!cSnapshotDir[0])
		sprintf(gcQuery,"/usr/bin/scp %s /vz/dump/vzdump-%u.tgz %s:/vz/dump/vzdump-%u.tgz",
				cSCPOptions,uContainer,cTargetNodeIPv4,uContainer);
	else
		sprintf(gcQuery,"/usr/bin/scp %s %s/vzdump-%u.tgz %s:/vz/dump/vzdump-%u.tgz",
				cSCPOptions,cSnapshotDir,uContainer,cTargetNodeIPv4,uContainer);
	if(system(gcQuery))
	{
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 2");
		goto CommonExit;
	}

	//2a-. Need md5sum file moved to target. TODO
	if(!cSnapshotDir[0])
		sprintf(gcQuery,"/usr/bin/scp %s /vz/dump/vzdump-%u.tgz.md5sum %s:/vz/dump/vzdump-%u.tgz.md5sum",
				cSCPOptions,uContainer,cTargetNodeIPv4,uContainer);
	else
		sprintf(gcQuery,"/usr/bin/scp %s %s/vzdump-%u.tgz.md5sum %s:/vz/dump/vzdump-%u.tgz.md5sum",
				cSCPOptions,cSnapshotDir,uContainer,cTargetNodeIPv4,uContainer);
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
		logfileLine("CloneContainer",gcQuery);
		tJobErrorUpdate(uJob,"error 3");
		goto CommonExit;
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
	if(uNotValidSystemCallArg(cSourceContainerIP))
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

//This goto MIGHT be ok.
CommonExit:
	//9c-. remove lock file
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
	if(!cSCPOptions[0] || uNotValidSystemCallArg(cSCPOptions))
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
	if(!cSCPOptions[0] || uNotValidSystemCallArg(cSCPOptions))
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
	char cVEIDLabel[32];
	sprintf(cVEIDLabel,"%.28s.fo",cLabel);
	if(SetContainerHostname(uSourceContainer,cHostname,cVEIDLabel) || 
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
		tJobWaitingUpdate(uJob);
		return;
	}


	//debug only
	//printf("FailoverFrom() cJobData: uIPv4=%u cLabel=%s cHostname=%s uSource=%u\n",
	//			uIPv4,cLabel,cHostname,uSource);

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
			tJobErrorUpdate(uJob,"vzctl stop");

			//rollback
			sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cOrigIP);
			system(gcQuery);
			//level 1 done
			return;
		}
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

	if(uJob==0) return(0);

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


void GetNodeProp(const unsigned uNode,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uNode==0) return;

	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=2 AND cName='%s'",
				uNode,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(gLfp!=NULL)
		{
			logfileLine("GetNodeProp",mysql_error(&gMysql));
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

}//void GetNodeProp()


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


unsigned ProcessCloneSyncJob(unsigned uNode,unsigned uContainer,unsigned uRemoteContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uPeriod=0;

	if(guDebug)
	{
		sprintf(gcQuery,"Start uContainer=%u,uRemoteContainer=%u",uContainer,uRemoteContainer);
		logfileLine("ProcessCloneSyncJob",gcQuery);
	}

	//Make sure source uContainer is on this node if not return no error.
	sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uContainer=%u AND uNode=%u",
					uContainer,uNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessCloneSyncJob",mysql_error(&gMysql));
		return(1);
	}
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)<1)
	{
		mysql_free_result(res);
		if(guDebug)
			logfileLine("ProcessCloneSyncJob","source container not on this node");
		return(0);
	}
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=3 AND cName='cuSyncPeriod'",
					uRemoteContainer);
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

		sprintf(gcQuery,"SELECT tNode.cLabel FROM tContainer,tNode WHERE"
				" tContainer.uNode=tNode.uNode AND"
				" tNode.uStatus=1 AND"//Active NODE
				" tContainer.uContainer=%u AND"
				" tContainer.uModDate+%u<=UNIX_TIMESTAMP(NOW())",uRemoteContainer,uPeriod);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessCloneSyncJob",mysql_error(&gMysql));
			return(3);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			if(uNotValidSystemCallArg(field[0]))
			{
				mysql_free_result(res);
				logfileLine("ProcessCloneSyncJob","security alert");
				return(4);
			}
			//Before running a recurring job we must update the cloned containers 
			// uModDate. This is done to not allow a script to run concurrently,
			// scripts should also have a lockfile mechanism to avoid this.
			sprintf(gcQuery,"UPDATE tContainer SET uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1"
						" WHERE uContainer=%u",uRemoteContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				mysql_free_result(res);
				logfileLine("ProcessCloneSyncJob",mysql_error(&gMysql));
				return(5);
			}
			sprintf(gcQuery,"/usr/sbin/clonesync.sh %u %u %s",uContainer,uRemoteContainer,field[0]);
			if(guDebug)
				logfileLine("ProcessCloneSyncJob",gcQuery);

			if(system(gcQuery))
			{
				mysql_free_result(res);
				logfileLine("ProcessCloneSyncJob",gcQuery);
				return(6);
			}
			else
			{
				//After running a recurring job we also update the cloned containers 
				// uModDate. This is for marking last time it was done.
				sprintf(gcQuery,"UPDATE tContainer SET uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1"
							" WHERE uContainer=%u",uRemoteContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					mysql_free_result(res);
					logfileLine("ProcessCloneSyncJob",mysql_error(&gMysql));
					return(7);
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
		printf("Job: uMonth=%u uDayOfMonth=%u uDayOfWeek=%u uHour=%u uMin=%u\n",
				uMonth,uDayOfMonth,uDayOfWeek,uHour,uMin);
	time(&luClock);
	localtime_r(&luClock,&structTm);

	//Note structTm.tm_mon+1. We adjust for normal 1-12
	if(guDebug)
		printf("Now: uMonth=%d uDayOfMonth=%d uDayOfWeek=%d uHour=%d uMin=%d\n",
			structTm.tm_mon+1,structTm.tm_mday,structTm.tm_wday,structTm.tm_hour,structTm.tm_min);

	//If not any month and now month is less than job month do not run.
	if(uMonth && uMonth>structTm.tm_mon+1)
	{
		if(guDebug)
			printf("uMonth not reached\n");
		goto Common_WaitingExit;
	}
	//If not any day of month and now day of month is less than job day of month do not run.
	if(uDayOfMonth && uDayOfMonth>structTm.tm_mday)
	{
		if(guDebug)
			printf("uDayOfMonth not reached\n");
		goto Common_WaitingExit;
	}
	//If not any day of week and now day of week is less than job day of week do not run.
	if(uDayOfWeek && uDayOfWeek>structTm.tm_wday)
	{
		if(guDebug)
			printf("uDayOfWeek not reached\n");
		goto Common_WaitingExit;
	}
	//If not any hour and now hour is less than job hour do not run.
	if(uHour && uHour>structTm.tm_hour)
	{
		if(guDebug)
			printf("uHour not reached\n");
		goto Common_WaitingExit;
	}
	//If now min is less than job min do not run.
	if(uMin>structTm.tm_min)
	{
		if(guDebug)
			printf("uMin not reached\n");
		goto Common_WaitingExit;
	}

	//Passed all constraints run job
	//Get command via cRecurringJob

        sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=0 AND uType=%u AND cName='%.99s'",
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

	if(guDebug)
		printf("Run job (%s)\n",cCommand);

	if(uNotValidSystemCallArg(cCommand))
	{
		logfileLine("RecurringJob","cCommand security alert");
		tJobErrorUpdate(uJob,"cCommand sec alert!");
		return;
	}
	//Only run if command is chmod 500 for extra security reasons.
	if(stat(cCommand,&statInfo))
	{
		tJobErrorUpdate(uJob,"stat(cCommand)");
		logfileLine("RecurringJob","stat failed for cCommand");
		return;
	}
	if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
	{
		tJobErrorUpdate(uJob,"cCommand is not chmod 500");
		logfileLine("RecurringJob","cCommand is not chmod 500");
		return;
	}
	//printf("good perms (%x)\n",statInfo.st_mode&(S_IWOTH|S_IWGRP|S_IWUSR|S_IXOTH|S_IROTH|S_IXGRP|S_IRGRP));
	//goto Common_WaitingExit;
	if(system(cCommand))
	{
		tJobErrorUpdate(uJob,"system(cCommand)");
		logfileLine("RecurringJob",cCommand);
		return;//Here we might want to continue anyway TODO
	}

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
	if(uMonth)
        	sprintf(gcQuery,"UPDATE tJob SET"
		" uJobDate=UNIX_TIMESTAMP(DATE_ADD(CURDATE(),INTERVAL 1 YEAR))+%u+%u-((DAYOFYEAR(CURDATE())+(%u*30))*86400)"
		" WHERE uJob=%u",uMin*60,uHour*3600,uMonth,uJob);
	else if(uDayOfMonth)
        	sprintf(gcQuery,"UPDATE tJob SET"
		" uJobDate=UNIX_TIMESTAMP(DATE_ADD(CURDATE(),INTERVAL 1 MONTH))+%u+%u-((DAY(CURDATE())+%u)*86400)"
		" WHERE uJob=%u",uMin*60,uHour*3600,uDayOfMonth,uJob);
	else if(uDayOfWeek)
        	sprintf(gcQuery,"UPDATE tJob SET"
		" uJobDate=UNIX_TIMESTAMP(DATE_ADD(CURDATE(),INTERVAL 1 WEEK))+%u+%u-((DAYOFWEEK(CURDATE())+%u)*86400)"
		" WHERE uJob=%u",uMin*60,uHour*3600,uDayOfWeek,uJob);
	else if(1)
        	sprintf(gcQuery,"UPDATE tJob SET"
		" uJobDate=UNIX_TIMESTAMP(CURDATE())+%u+%u"
		" WHERE uJob=%u",uMin*60,uHour*3600,uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("RecurringJob",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"DATE_ADD(CURDATE(),INTERVAL");
		return;
	}
	//Done
	SetJobStatus(uJob,uWAITING);
	return;

Common_WaitingExit:
	if(guDebug)
		printf("Skipping job not time yet\n");
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
