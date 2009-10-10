/*
FILE
	jobqueue.c
	$Id$
PURPOSE
	Command line processing of jobs in the tJob queue.
AUTHOR
	Gary Wallis for Unxiservice (C) 2008-2009. GPLv2 License applies.
NOTES
	We still use KISS code, var naming conventions, and Allman (ANSI) style C 
	indentation to make our software readable and writable by any programmer. 
	At the same time this approach (although with redundant code) has kept these
	programs lean and faster than anything available in any other language.
TODO
	Create more #define based "macros," to help the compiler optimize the
	many simple, fast but redundant code blocks.
	Get rid of any goto statements that do not add too many layers of nested
	logic that makes the code hard to maintain by non-authors.
	Use uNotValidSystemCallArg() before all system() calls where any args
	come from db and are not formatted (sprintf) as numbers.
*/

#include "mysqlrad.h"
#include <openisp/template.h>

#define mysqlrad_Query_TextErr_Exit	mysql_query(&gMysql,gcQuery);\
					if(mysql_errno(&gMysql))\
					{\
						printf("%s\n",mysql_error(&gMysql));\
						exit(2);\
					}

//the following prototype declarations should provide a 
//	table of contents


//local protos
void ProcessJobQueue(void);
void ProcessJob(unsigned uJob,unsigned uDatacenter,unsigned uNode,
		unsigned uContainer,char *cJobName,char *cJobData);
void tJobErrorUpdate(unsigned uJob, char *cErrorMsg);
void tJobDoneUpdate(unsigned uJob);
void tJobWaitingUpdate(unsigned uJob);
void InstallConfigFile(unsigned uJob,unsigned uContainer,char *cJobData);
void ChangeIPContainer(unsigned uJob,unsigned uContainer,char *cJobData);
void ChangeHostnameContainer(unsigned uJob,unsigned uContainer);
void MountFilesContainer(unsigned uJob,unsigned uContainer);
void NewContainer(unsigned uJob,unsigned uContainer);
void DestroyContainer(unsigned uJob,unsigned uContainer);
void StopContainer(unsigned uJob,unsigned uContainer);
void StartContainer(unsigned uJob,unsigned uContainer);
void MigrateContainer(unsigned uJob,unsigned uContainer,char *cJobData);
void CloneContainer(unsigned uJob,unsigned uContainer,char *cJobData);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);
void GetGroupProp(const unsigned uGroup,const char *cName,char *cValue);
void GetContainerProp(const unsigned uContainer,const char *cName,char *cValue);
void UpdateContainerUBC(unsigned uJob,unsigned uContainer,const char *cJobData);
void SetContainerUBC(unsigned uJob,unsigned uContainer,const char *cJobData);
void TemplateContainer(unsigned uJob,unsigned uContainer,const char *cJobData);
void LocalImportTemplate(unsigned uJob,unsigned uDatacenter,const char *cJobData);
void LocalImportConfig(unsigned uJob,unsigned uDatacenter,const char *cJobData);
int CreateMountFiles(unsigned uContainer, unsigned uOverwrite);
unsigned uNotValidSystemCallArg(char *cSSHOptions);
void FailoverTo(unsigned uJob,unsigned uContainer,const char *cJobData);
void FailoverFrom(unsigned uJob,unsigned uContainer,const char *cJobData);
unsigned GetContainerStatus(const unsigned uContainer,unsigned *uStatus);
unsigned GetContainerMainIP(const unsigned uContainer,char *cIP);
unsigned GetContainerSource(const unsigned uContainer, unsigned *uSource);
unsigned SetContainerIP(const unsigned uContainer,char *cIP);

//extern protos
void TextConnectDb(void); //main.c
void SetContainerStatus(unsigned uContainer,unsigned uStatus);
void SetContainerNode(unsigned uContainer,unsigned uNode);
void GetConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer,
		unsigned uHtml);

//file scoped. fix this someday
static unsigned guNode=0;
static unsigned guDatacenter=0;


//Using the local server hostname get max 100 jobs for this node from the tJob queue.
//Then dispatch jobs via ProcessJob() this function in turn calls specific functions for
//each known cJobName.
static char cHostname[100]={""};//file scope

unsigned ProcessCloneSyncJob(unsigned uNode,unsigned uContainer,unsigned uRemoteNode, unsigned uRemoteContainer);
unsigned ProcessCloneSyncJob(unsigned uNode,unsigned uContainer,unsigned uRemoteNode, unsigned uRemoteContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uPeriod=0;

	//debug only
	//printf("ProcessCloneSyncJob() for %s (uNode=%u,uContainer=%u,uRemoteNode=%u,uRemoteContainer=%u)\n",
	//		cHostname,uNode,uContainer,uRemoteNode,uRemoteContainer);

	//Make sure source uContainer is on this node if not return no error.
	sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uContainer=%u AND uNode=%u",
					uContainer,uNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		return(1);
	}
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)<1)
	{
		mysql_free_result(res);
		return(0);
	}
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=3 AND cName='cuSyncPeriod'",
					uRemoteContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		return(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uPeriod);
	mysql_free_result(res);

	if(uPeriod!=0)
	{

		sprintf(gcQuery,"SELECT tNode.cLabel FROM tContainer,tNode WHERE"
				" tContainer.uNode=tNode.uNode AND"
				" tContainer.uContainer=%u AND"
				" tContainer.uModDate+%u<=UNIX_TIMESTAMP(NOW())",uRemoteContainer,uPeriod);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			return(3);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			//debug only
			//printf("ProcessCloneSyncJob() uPeriod=%u\n",uPeriod);
			if(uNotValidSystemCallArg(field[0]))
			{
				mysql_free_result(res);
				printf("ProcessCloneSyncJob() sec alert!\n");
				return(4);
			}
			sprintf(gcQuery,"/usr/sbin/clonesync.sh %u %u %s",uContainer,uRemoteContainer,field[0]);
			//debug only
			//printf("ProcessCloneSyncJob() '%s'\n",gcQuery);
			if(system(gcQuery))
			{
				mysql_free_result(res);
				printf("ProcessCloneSyncJob() '%s' failed!\n",gcQuery);
				return(5);
			}
			else
			{
				//After running a recurring job we must update the cloned containers uModDate!
				sprintf(gcQuery,"UPDATE tContainer SET uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1"
							" WHERE uContainer=%u",uRemoteContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					mysql_free_result(res);
					printf("%s\n",mysql_error(&gMysql));
					return(6);
				}
			}
		}
		mysql_free_result(res);
	}
	return(0);

}//void ProcessCloneSyncJob()


void LogError(char *cErrorMsg,unsigned uKey);
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
		printf("%s\n",mysql_error(&gMysql));
		exit(2);
	}

}//void LogError(char *cErrorMsg)


void ProcessJobQueue(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uDatacenter=0;
	unsigned uNode=0;
	unsigned uRemoteNode=0;
	unsigned uContainer=0;
	unsigned uRemoteContainer=0;
	unsigned uJob=0;
	unsigned uError=0;

	if(gethostname(cHostname,99)!=0)
	{
		printf("gethostname() failed: aborted\n");
		exit(1);
	}

	TextConnectDb();//Uses login data from local.h
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
	}
	mysql_free_result(res);

	if(!uNode)
	{
		char *cp;

		//FQDN vs short name of 2nd NIC mess
		if((cp=strchr(cHostname,'.')))
			*cp=0;
		sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
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
		printf("could not determine uNode: aborted\n");
		exit(1);
	}

	//Special recurring jobs based on special containers
	//Main loop normal jobs
	//1-. Maintain clone containers
	//We need to rsync from running container to clone container
	//where the source container is running on this node
	//and the target node is a remote node.
	sprintf(gcQuery,"SELECT uSource,uContainer,uNode FROM tContainer WHERE uSource>0 AND"
			" uDatacenter=%u AND (uStatus=1 OR uStatus=31)",uDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		//uSource==uContainer has to be on this node. We defer that determination
		//to ProcessCloneSyncJob()
		sscanf(field[0],"%u",&uContainer);
		sscanf(field[1],"%u",&uRemoteContainer);
		sscanf(field[2],"%u",&uRemoteNode);
		if((uError=ProcessCloneSyncJob(uNode,uContainer,uRemoteNode,uRemoteContainer)))
			LogError("ProcessCloneSyncJob()",uError);
	}
	mysql_free_result(res);

	//debug only
	//printf("ProcessJobQueue() for %s (uNode=%u,uDatacenter=%u)\n",
	//		cHostname,uNode,uDatacenter);
	//exit(0);

	//Main loop normal jobs
	sprintf(gcQuery,"SELECT uJob,uContainer,cJobName,cJobData FROM tJob WHERE uJobStatus=1"
				" AND uDatacenter=%u AND uNode=%u"
				" AND uJobDate<=UNIX_TIMESTAMP(NOW()) LIMIT 100",
						uDatacenter,uNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uJob);
		sscanf(field[1],"%u",&uContainer);
		//Job dispatcher based on cJobName
		ProcessJob(uJob,uDatacenter,uNode,uContainer,field[2],field[3]);
	}
	mysql_free_result(res);

	//debug only
	//printf("ProcessJobQueue() End\n");
	exit(0);

}//void ProcessJobQueue(void)



void ProcessJob(unsigned uJob,unsigned uDatacenter,unsigned uNode,
		unsigned uContainer,char *cJobName,char *cJobData)
{
	static unsigned uCount=0;
	guNode=uNode;
	guDatacenter=uDatacenter;

	//Some jobs may take quite some time, we need to make sure we don't run again!
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=2,cRemoteMsg='Running',uModBy=1,"
				"uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJob=%u",uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("(%s) but we continue...\n",mysql_error(&gMysql));


	//if debug
	printf("%3.3u uJob=%u uContainer=%u cJobName=%s; cJobData=%s;\n",
			uCount++,uJob,uContainer,cJobName,cJobData);

	//Is priority order needed in some cases?
	if(!strcmp(cJobName,"FailoverFrom"))
	{
		FailoverFrom(uJob,uContainer,cJobData);
	}
	else if(!strcmp(cJobName,"FailoverTo"))
	{
		FailoverTo(uJob,uContainer,cJobData);
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
	else if(!strcmp(cJobName,"MountFilesContainer"))
	{
		MountFilesContainer(uJob,uContainer);
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
	else if(!strcmp(cJobName,"InstallConfigFile"))
	{
		InstallConfigFile(uJob,uContainer,cJobData);
	}
	else if(1)
	{
		printf("Not-implemented cJobName=%s skipping...\n",cJobName);
		tJobErrorUpdate(uJob,cJobName);
	}

}//ProcessJob(...)


//Shared functions


void tJobErrorUpdate(unsigned uJob, char *cErrorMsg)
{
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=14,cRemoteMsg='%.31s',uModBy=1,"
				"uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJob=%u",
									cErrorMsg,uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
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
		printf("%s\n",mysql_error(&gMysql));
		exit(2);
	}

}//void tJobErrorUpdate(unsigned uJob, char *cErrorMsg)


void tJobDoneUpdate(unsigned uJob)
{
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=3,uModBy=1,cRemoteMsg='tJobDoneUpdate() ok',"
				"uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJob=%u",uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
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
		printf("%s\n",mysql_error(&gMysql));
		exit(2);
	}

}//void tJobWaitingUpdate(unsigned uJob, char *cErrorMsg)


//Specific job handlers


void InstallConfigFile(unsigned uJob,unsigned uContainer,char *cJobData)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uConfiguration=0;
	char cDir[32]={""};
	char *cp;

	if((cp=strstr(cJobData,"uConfiguration=")))
		sscanf(cp+15,"%u",&uConfiguration);

	if(!uConfiguration)
	{
		printf("InstallConfigFile() error: No uConfiguration. Skipping...\n");
		tJobErrorUpdate(uJob,"uConfiguration=0");
	}

	//debug only
	printf("InstallConfigFile() uConfiguration=%u\n",uConfiguration);

	if(uContainer)
	{
		struct stat statInfo;

		if(uContainer>100 && uContainer<99999)
		{
			sprintf(cDir,"/vz/private/%u",uContainer);
		}
		else
		{
			printf("InstallConfigFile() error: Out of range uContainer=%u.\n",uContainer);
			tJobErrorUpdate(uJob,"Outofrange uContainer");
			return;
		}

		if(stat(cDir,&statInfo)!=0)
		{
			printf("InstallConfigFile() error: No %s.\n",cDir);
			tJobErrorUpdate(uJob,cDir);
			return;
		}
	}


	sprintf(gcQuery,"SELECT cLabel,cValue,cComment FROM tConfiguration WHERE uConfiguration=%u",
				uConfiguration);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char *cp2;
		char cSystem[256]={""};
		char cOwnerGroup[100]={""};
		char cPostOp[100]={""};
		unsigned uFilePerms=0;
		unsigned uError=0;
		char cFilename[256]={""};
		FILE *fp;

		//Parse cLabel file;root:root;400;none;
		if((cp=strstr(field[0],"file;")))
		{
			//find end of cOwnerGroup
			if((cp2=strchr(cp+6,';')))
			{
				*cp2=0;
				sprintf(cOwnerGroup,"%.99s",cp+5);
				//find end of uFilePerms
				if((cp=strchr(cp2+1,';')))
					sscanf(cp2+1,"%u",&uFilePerms);
				//find end of cPostOp
				if((cp2=strchr(cp+1,';')))
				{
					*cp2=0;
					sprintf(cPostOp,"%.99s",cp+1);
				}
			}
		}
		//debug only
		printf("InstallConfigFile() cOwnerGroup=%s; uFilePerms=%u cPostOp=%s; cFilename=%s;\n",
			cOwnerGroup,uFilePerms,cPostOp,cFilename);

		//Install file
		if((fp=fopen(cFilename,"w"))==NULL)
		{
			printf("InstallConfigFile() error: Can't install %s.\n",cFilename);
			tJobErrorUpdate(uJob,cFilename);
			return;//Do not continue if we can't even open file
		}
		//we may need to filter out PC cr-lf's
		fprintf(fp,"%s\n",field[2]);
		if(fclose(fp))
		{
			printf("InstallConfigFile() error: fclose() %s.\n",cFilename);
			tJobErrorUpdate(uJob,cFilename);
			return;//Do not continue if we can't close the file.
		}

		sprintf(cSystem,"/bin/chown %s %s",cOwnerGroup,cFilename);
		if(system(cSystem))
		{
			printf("InstallConfigFile() error: %s.\n",cSystem);
			tJobErrorUpdate(uJob,cSystem);
			uError++;
		}

		sprintf(cSystem,"/bin/chmod %u %s",uFilePerms,cFilename);
		if(system(cSystem))
		{
			printf("InstallConfigFile() error: %s.\n",cSystem);
			tJobErrorUpdate(uJob,cSystem);
			uError++;
		}

		//Do not run post file install command if none or if chown and/or chmod failed
		if(strcmp(cPostOp,"none") && !uError)
		{
			//Limits on cPostOP:
			//The post install command must return 0 if everything went ok;
			if(system(cPostOp))
			{
				printf("InstallConfigFile() error: %s.\n",cPostOp);
				tJobErrorUpdate(uJob,cPostOp);
				return;
			}
		}

		if(!uError)
			tJobDoneUpdate(uJob);
	}
	mysql_free_result(res);


}//void InstallConfigFile(...)


void NewContainer(unsigned uJob,unsigned uContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tContainer.cLabel,tContainer.cHostname,tIP.cLabel"
			",tOSTemplate.cLabel,tNameserver.cLabel,tSearchdomain.cLabel,tConfig.cLabel"
			" FROM tContainer,tOSTemplate,tNameserver,tSearchdomain,tConfig,tIP WHERE uContainer=%u"
			" AND tContainer.uOSTemplate=tOSTemplate.uOSTemplate"
			" AND tContainer.uNameserver=tNameserver.uNameserver"
			" AND tContainer.uConfig=tConfig.uConfig"
			" AND tContainer.uIPv4=tIP.uIP"
			" AND tContainer.uSearchdomain=tSearchdomain.uSearchdomain",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{


		//0-. vz conf mount umount files if applicable. 1 is for overwrite existing files
		if(CreateMountFiles(uContainer,1))
		{
			tJobErrorUpdate(uJob,"CreateMountFiles(x,1) failed");
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
			printf("NewContainer() error: uNotValidSystemCallArg()\n");
			tJobErrorUpdate(uJob,"failed sec alert!");
			goto CommonExit;
		}
		
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose create %u --ostemplate %s --hostname %s"
				" --ipadd %s --name %s --config %s",
				uContainer,field[3],field[1],field[2],field[0],field[6]);
		if(system(gcQuery))
		{
			printf("NewContainer() error: %s\n",gcQuery);
			tJobErrorUpdate(uJob,"vzctl create failed");
		}

		//2-.
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --nameserver \"%.99s\" --searchdomain %.32s --save",
				uContainer,field[4],field[5]);
		if(system(gcQuery))
		{
			printf("NewContainer() error: %s\n",gcQuery);
			tJobErrorUpdate(uJob,"vzctl set failed");
			//Roll back step 1-.
			sprintf(gcQuery,"/usr/sbin/vzctl destroy %u",uContainer);
			if(system(gcQuery))
			{
				printf("NewContainer() error: %s\n",gcQuery);
				tJobErrorUpdate(uJob,"rb: vzctl destroy failed");
			}
			goto CommonExit;
		}

		//3-.
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
		if(system(gcQuery))
		{
			printf("NewContainer() error: %s\n",gcQuery);
			tJobErrorUpdate(uJob,"vzctl start failed");
			//Roll back step 1-.
			sprintf(gcQuery,"/usr/sbin/vzctl destroy %u",uContainer);
			if(system(gcQuery))
			{
				printf("NewContainer() error: %s\n",gcQuery);
				tJobErrorUpdate(uJob,"rb: vzctl destroy failed");
			}
			goto CommonExit;
		}
	}
	else
	{
		printf("NewContainer() error: Select for %u failed.\n",uContainer);
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

			printf("Container root passwd changed\n");
		}
	}

	//Everything went ok;
	SetContainerStatus(uContainer,1);//Active
	tJobDoneUpdate(uJob);

CommonExit:
	mysql_free_result(res);


}//void NewContainer(...)


void DestroyContainer(unsigned uJob,unsigned uContainer)
{

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u",uContainer);
	if(system(gcQuery))
	{
		printf("DestroyContainer() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"vzctl stop failed");
		goto CommonExit;
	}

	//2-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose destroy %u",uContainer);
	if(system(gcQuery))
	{
		printf("DestroyContainer() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"vzctl destroy failed");
		goto CommonExit;
	}

	//Everything ok
	SetContainerStatus(uContainer,11);//Initial
	tJobDoneUpdate(uJob);

CommonExit:
	return;

}//void DestroyContainer(...)


void ChangeIPContainer(unsigned uJob,unsigned uContainer,char *cJobData)
{

	char cIPOld[256]={""};
	char cIPNew[31]={""};
	char *cp;
        MYSQL_RES *res;
        MYSQL_ROW field;

	//0-. Get required data
	sprintf(gcQuery,"SELECT tIP.cLabel"
			" FROM tContainer,tIP WHERE uContainer=%u"
			" AND tContainer.uIPv4=tIP.uIP",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cIPNew,"%.31s",field[0]);
	if(!cIPNew[0])	
	{
		printf("ChangeIPContainer() error 1\n");
		tJobErrorUpdate(uJob,"Get cIPNew failed");
		goto CommonExit;
	}
	sscanf(cJobData,"cIPOld=%31s;",cIPOld);
	if((cp=strchr(cIPOld,';')))
		*cp=0;
	if(!cIPOld[0])	
	{
		printf("ChangeIPContainer() error 2\n");
		tJobErrorUpdate(uJob,"Get cIPOld failed");
		goto CommonExit;
	}

	if(uNotValidSystemCallArg(cIPNew)||uNotValidSystemCallArg(cIPOld))
	{
		tJobErrorUpdate(uJob,"failed sec alert!");
		goto CommonExit;
	}

	//0-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u",uContainer);
	if(system(gcQuery))
	{
		printf("ChangeIPContainer() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"vzctl stop failed");
		goto CommonExit;
	}

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cIPNew);
	if(system(gcQuery))
	{
		printf("ChangeIPContainer() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"vzctl set ipadd failed");
		goto CommonExit;
	}


	//2-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel %s --save",uContainer,cIPOld);
	if(system(gcQuery))
	{
		printf("ChangeIPContainer() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"vzctl set ipdel failed");
		goto CommonExit;
	}

	//3-.
	sprintf(gcQuery,"UPDATE tIP SET uAvailable=1 WHERE cLabel='%.31s'",cIPOld);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"Release old IP from tIP failed");
		goto CommonExit;
	}

	//4-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
	if(system(gcQuery))
	{
		printf("ChangeIPContainer() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"vzctl start failed");
		goto CommonExit;
	}


	//Everything ok
	SetContainerStatus(uContainer,1);//Active
	tJobDoneUpdate(uJob);

CommonExit:
	mysql_free_result(res);
	return;

}//void ChangeIPContainer(...)


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
		printf("%s\n",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cName,"%.99s",field[0]);
		sprintf(cHostname,"%.99s",field[1]);
	}

	if(!cHostname[0] || !cName[0])	
	{
		printf("ChangeHostnameContainer() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"Get cHostname failed");
		goto CommonExit;
	}

	if(uNotValidSystemCallArg(cHostname)||uNotValidSystemCallArg(cName))
	{
		tJobErrorUpdate(uJob,"failed sec alert!");
		goto CommonExit;
	}

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --hostname %s --name %s --save",
				uContainer,cHostname,cName);
	if(system(gcQuery))
	{
		printf("ChangeHostnameContainer() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"vzctl set hostname failed");
		goto CommonExit;
	}


	//Everything ok
	SetContainerStatus(uContainer,1);//Active
	tJobDoneUpdate(uJob);

CommonExit:
	mysql_free_result(res);
	return;

}//void ChangeHostnameContainer(...)


void StopContainer(unsigned uJob,unsigned uContainer)
{

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u",uContainer);
	if(system(gcQuery))
	{
		printf("StopContainer() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"vzctl stop failed");
		goto CommonExit;
	}

	//Everything ok
	SetContainerStatus(uContainer,uSTOPPED);
	tJobDoneUpdate(uJob);

CommonExit:
	return;

}//void StopContainer(...)


void StartContainer(unsigned uJob,unsigned uContainer)
{

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u",uContainer);
	if(system(gcQuery))
	{
		sprintf(gcQuery,"/usr/sbin/vzctl --verbose restart %u",uContainer);
		if(system(gcQuery))
		{
			printf("StartContainer() error: %s\n",gcQuery);
			tJobErrorUpdate(uJob,"vzctl start/restart failed");
			goto CommonExit;
		}
	}

	//Everything ok
	SetContainerStatus(uContainer,uACTIVE);
	tJobDoneUpdate(uJob);

CommonExit:
	return;

}//void StartContainer(...)


void MigrateContainer(unsigned uJob,unsigned uContainer,char *cJobData)
{
	char cTargetNodeIPv4[256]={""};
	unsigned uTargetNode=0;

	sscanf(cJobData,"uTargetNode=%u;",&uTargetNode);
	if(!uTargetNode)
	{
		printf("MigrateContainer() error: Could not determine uTargetNode\n");
		tJobErrorUpdate(uJob,"uTargetNode==0");
		goto CommonExit;
	}

	GetNodeProp(uTargetNode,"cIPv4",cTargetNodeIPv4);
	if(!cTargetNodeIPv4[0])
	{
		printf("MigrateContainer() error: Could not determine cTargetNodeIPv4. uTargetNode=%u;\n",uTargetNode);
		tJobErrorUpdate(uJob,"cTargetNodeIPv4");
		goto CommonExit;
	}

	//vzmigrate --online -v <destination_address> <veid>
	//Most specific tConfiguration is used. This allows for some nodes to be set global
	//and others specific. But is slower than the other option with what maybe
	//very large numbers of per node tConfiguration entries.
	char cSSHOptions[256]={""};
	GetConfiguration("cSSHOptions",cSSHOptions,guDatacenter,guNode,0,0);//First try node specific
	if(!cSSHOptions[0])
	{
		GetConfiguration("cSSHOptions",cSSHOptions,guDatacenter,0,0,0);//Second try datacenter wide
		if(!cSSHOptions[0])
			GetConfiguration("cSSHOptions",cSSHOptions,0,0,0,0);//Last try global
	}
	if(uNotValidSystemCallArg(cSSHOptions))
		cSSHOptions[0]=0;
	char cSCPOptions[256]={""};
	GetConfiguration("cSCPOptions",cSCPOptions,guDatacenter,guNode,0,0);//First try node specific
	if(!cSCPOptions[0])
	{
		GetConfiguration("cSCPOptions",cSCPOptions,guDatacenter,0,0,0);//Second try datacenter wide
		if(!cSCPOptions[0])
			GetConfiguration("cSCPOptions",cSCPOptions,0,0,0,0);//Last try global
	}
	//Default for less conditions below
	if(!cSCPOptions[0] || uNotValidSystemCallArg(cSCPOptions))
		sprintf(cSCPOptions,"-P 22");

		

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
		printf("MigrateContainer() error: %s.\nTrying offline migration (check kernel compat)...\n",gcQuery);
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
			printf("MigrateContainer() error: %s.\nGiving up!\n",gcQuery);
			tJobErrorUpdate(uJob,"vzmigrate on/off-line failed");
			goto CommonExit;
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
			printf("MigrateContainer() non-fatal error: %s.\n",gcQuery);
	}

	//Everything ok
	SetContainerStatus(uContainer,1);//Active
	SetContainerNode(uContainer,uTargetNode);//Migrated!
	tJobDoneUpdate(uJob);

CommonExit:
	return;

}//void MigrateContainer(...)


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
		printf("%s\n",mysql_error(&gMysql));
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

}//void GetNodeProp(...)


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
		printf("%s\n",mysql_error(&gMysql));
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
		printf("%s\n",mysql_error(&gMysql));
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
		printf("UpdateContainerUBC() error: Could not determine cResource\n");
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
		tJobErrorUpdate(uJob,"failed sec alert!");
		goto CommonExit;
	}

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl set %u --%s %.0f:%.0f --save",
		uContainer,cResource,luBar,luLimit);
	if(system(gcQuery))
	{
		printf("UpdateContainerUBC() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"vzctl set failed");
		goto CommonExit;
	}

	//2-. validate and/or repair conf file
	sprintf(gcQuery,"/usr/sbin/vzcfgvalidate -r /etc/vz/conf/%u.conf > "
			" /tmp/UpdateContainerUBC.vzcfgcheck.output 2>&1",uContainer);
	if(system(gcQuery))
	{
		printf("UpdateContainerUBC() error: %s\n",gcQuery);
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
	}

	if(uChange)
	{
		//3-. See 4-.
		sprintf(cContainerPath,"/etc/vz/conf/%u.conf",uContainer);
		sprintf(cApplyConfigPath,"/etc/vz/conf/ve-%u.conf-sample",uContainer);
		if(symlink(cContainerPath,cApplyConfigPath))
		{
			printf("UpdateContainerUBC() error: 3-. symlink failed\n");
			tJobErrorUpdate(uJob,"symlink failed");
			goto CommonExit;
		}

		//4-. Apply any changes produced by vzcfgvalidate -r
		sprintf(gcQuery,"/usr/sbin/vzctl set %u --applyconfig %u --save",
			uContainer,uContainer);
		if(system(gcQuery))
		{
			printf("UpdateContainerUBC() error: %s\n",gcQuery);
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
		printf("UpdateContainerUBC() error: %s\n",mysql_error(&gMysql));
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
		printf("SetContainerUBC() error: Could not determine cResource\n");
		tJobErrorUpdate(uJob,"cResource[0]==0");
		goto CommonExit;
	}
	if(!luBar || !luLimit)
	{
		printf("SetContainerUBC() error: Could not determine luBar||luLimit\n");
		tJobErrorUpdate(uJob,"!luBar||!luLimit");
		goto CommonExit;
	}

	if(uNotValidSystemCallArg(cResource))
	{
		tJobErrorUpdate(uJob,"failed sec alert!");
		goto CommonExit;
	}

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl set %u --%s %lu:%lu --save",
		uContainer,cResource,luBar,luLimit);
	if(system(gcQuery))
	{
		printf("SetContainerUBC() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"vzctl set failed");
		goto CommonExit;
	}

	//2-. validate and/or repair conf file
	sprintf(gcQuery,"/usr/sbin/vzcfgvalidate -r /etc/vz/conf/%u.conf > "
			" /tmp/SetContainerUBC.vzcfgcheck.output 2>&1",uContainer);
	if(system(gcQuery))
	{
		printf("SetContainerUBC() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"vzcfgvalidate failed");
		goto CommonExit;
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
	}

	if(uChange)
	{
		//3-. See 4-.
		sprintf(cContainerPath,"/etc/vz/conf/%u.conf",uContainer);
		sprintf(cApplyConfigPath,"/etc/vz/conf/ve-%u.conf-sample",uContainer);
		if(symlink(cContainerPath,cApplyConfigPath))
		{
			printf("SetContainerUBC() error: 3-. symlink failed\n");
			tJobErrorUpdate(uJob,"symlink failed");
			goto CommonExit;
		}

		//4-. Apply any changes produced by vzcfgvalidate -r
		sprintf(gcQuery,"/usr/sbin/vzctl set %u --applyconfig %u --save",
			uContainer,uContainer);
		if(system(gcQuery))
		{
			printf("SetContainerUBC() error: %s\n",gcQuery);
			tJobErrorUpdate(uJob,"vzctl set 4 failed");
			goto CommonExit;
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
		printf("SetContainerUBC() error: %s\n",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"INSERT INTO tLog failed");
		goto CommonExit;
	}

	//Everything ok
	tJobDoneUpdate(uJob);

CommonExit:
	return;

}//void SetContainerUBC(...)


void TemplateContainer(unsigned uJob,unsigned uContainer,const char *cJobData)
{
	//Only run one vzdump job per HN. Wait for lock release.
	//Log this rare condition
	if(access("/var/run/vzdump.lock",R_OK)==0)
	{
		printf("/var/run/vzdump.lock exists\n");
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
		printf("TemplateContainer() error: Could not determine cConfigLabel\n");
		tJobErrorUpdate(uJob,"cConfigLabel[0]==0");
		goto CommonExit;
	}

	sprintf(gcQuery,"SELECT tOSTemplate.cLabel FROM tContainer,tOSTemplate"
			" WHERE tContainer.uOSTemplate=tOSTemplate.uOSTemplate AND"
			" tContainer.uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tOSTemplate.cLabel");
		goto CommonExit;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cOSTemplateBase,"%.67s",field[0]);
	mysql_free_result(res);
	if(!cOSTemplateBase[0])
	{
		printf("TemplateContainer() error: Could not determine cOSTemplateBase\n");
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
		tJobErrorUpdate(uJob,"failed sec alert!");
		goto CommonExit;
	}

	char cSnapshotDir[256]={""};
	GetConfiguration("cSnapshotDir",cSnapshotDir,guDatacenter,guNode,0,0);//First try node specific
	if(!cSnapshotDir[0])
	{
		GetConfiguration("cSnapshotDir",cSnapshotDir,guDatacenter,0,0,0);//Second try datacenter wide
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
		printf("TemplateContainer() error: %s.\n",gcQuery);
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
		printf("TemplateContainer() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"mv tgz to cache tar.gz failed");
		goto CommonExit;
	}

	//3-. scp template to all nodes depends on /usr/sbin/allnodescp.sh installed and configured correctly
	if(!stat("/usr/sbin/allnodescp.sh",&statInfo))
	{
		sprintf(gcQuery,"/usr/sbin/allnodescp.sh /vz/template/cache/%.67s-%.32s.tar.gz",
			cOSTemplateBase,cConfigLabel);
		if(system(gcQuery))
		{
			printf("TemplateContainer() error: %s\n",gcQuery);
			tJobErrorUpdate(uJob,"allnodescp.sh .tar.gz failed");
			goto CommonExit;
		}
	}

	//4-. Make /etc/vz/conf tConfig file and scp to all nodes. ve-proxpop.conf-sample
	sprintf(gcQuery,"/bin/cp /etc/vz/conf/%u.conf /etc/vz/conf/ve-%.32s.conf-sample",
		uContainer,cConfigLabel);
	if(system(gcQuery))
	{
		printf("TemplateContainer() error: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"cp conf container failed");
		goto CommonExit;
	}
	if(!stat("/usr/sbin/allnodescp.sh",&statInfo))
	{
		sprintf(gcQuery,"/usr/sbin/allnodescp.sh /etc/vz/conf/ve-%.32s.conf-sample",cConfigLabel);
		if(system(gcQuery))
		{
			printf("TemplateContainer() error: %s\n",gcQuery);
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
		printf("%s\n",mysql_error(&gMysql));
		printf("No such uJob? Using uOwner=1 and uCreatedBy=1\n");
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
		printf("%s\n",mysql_error(&gMysql));
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
			printf("%s\n",mysql_error(&gMysql));
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
		printf("%s\n",mysql_error(&gMysql));
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
			printf("%s\n",mysql_error(&gMysql));
			tJobErrorUpdate(uJob,"INSERT tConfig");
			goto CommonExit;
		}
	}
	mysql_free_result(res);

	//Everything ok
	SetContainerStatus(uContainer,1);//Active
	tJobDoneUpdate(uJob);

CommonExit:
	//6-. remove lock file
	unlink("/var/run/vzdump.lock");
	return;

}//void TemplateContainer(...)


void MountFilesContainer(unsigned uJob,unsigned uContainer)
{
	//0 means do not overwrite existing files.
	if(CreateMountFiles(uContainer,0))
	{
		tJobErrorUpdate(uJob,"CreateMountFiles(x,0) failed");
		return;
	}

	tJobDoneUpdate(uJob);
	return;

}//void MountFilesContainer(unsigned uJob,unsigned uContainer);


void CloneContainer(unsigned uJob,unsigned uContainer,char *cJobData)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cTargetNodeIPv4[256]={""};
	unsigned uNewVeid=0;
	unsigned uCloneStop=0;
	unsigned uTargetNode=0;
	char cSourceContainerIP[32]={""};
	char cNewIP[32]={""};
	char cHostname[100]={""};
	char cName[32]={""};

	//CloneWizard created a new tContainer with "Awaiting Clone" status.
	//CloneWizard created a job (this job) that runs on the source container node.
	//The created job has job data to complete the operations on source and
	//target nodes.
	//vzdump script must be installed on all datacenter nodes.

	//Only run one CloneContainer job per HN. Wait for lock release.
	//Log this rare condition
	if(access("/var/run/vzdump.lock",R_OK)==0)
	{
		printf("/var/run/vzdump.lock exists\n");
		tJobWaitingUpdate(uJob);
		return;
	}

	//Set job data based vars
	sscanf(cJobData,"uTargetNode=%u;",&uTargetNode);
	if(!uTargetNode)
	{
		printf("CloneContainer() error: Could not determine uTargetNode\n");
		tJobErrorUpdate(uJob,"uTargetNode==0");
		goto CommonExit;
	}
	sscanf(cJobData,"uTargetNode=%*u;\nuNewVeid=%u;",&uNewVeid);
	if(!uNewVeid)
	{
		printf("CloneContainer() error: Could not determine uNewVeid\n");
		tJobErrorUpdate(uJob,"uNewVeid==0");
		goto CommonExit;
	}
	sscanf(cJobData,"uTargetNode=%*u;\nuNewVeid=%*u;\nuCloneStop=%u;",&uCloneStop);

	sprintf(gcQuery,"SELECT tIP.cLabel,tContainer.cHostname,tContainer.cLabel FROM tIP,tContainer"
				" WHERE tIP.uIP=tContainer.uIPv4"
				" AND tContainer.uContainer=%u",uNewVeid);
	mysqlrad_Query_TextErr_Exit;
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
		printf("CloneContainer() error: Could not determine cNewIP\n");
		tJobErrorUpdate(uJob,"No cNewIP");
		goto CommonExit;
	}

	if(!cHostname[0])
	{
		printf("CloneContainer() error: Could not determine cHostname\n");
		tJobErrorUpdate(uJob,"No cHostname");
		goto CommonExit;
	}

	if(!cName[0])
	{
		printf("CloneContainer() error: Could not determine cName\n");
		tJobErrorUpdate(uJob,"No cName");
		goto CommonExit;
	}

	sprintf(gcQuery,"SELECT tIP.cLabel FROM tIP,tContainer WHERE tIP.uIP=tContainer.uIPv4"
			" AND tContainer.uContainer=%u",uContainer);
	mysqlrad_Query_TextErr_Exit;
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cSourceContainerIP,"%.31s",field[0]);
	mysql_free_result(res);

	if(!cSourceContainerIP[0])
	{
		printf("CloneContainer() error: Could not determine cSourceContainerIP\n");
		tJobErrorUpdate(uJob,"No cSourceContainerIP");
		goto CommonExit;
	}

	//This needs it's tcontainerfunc.h counterpart on new and a tConfiguration entry
	//to set this IP to either the internal transfer IP or the public IP if servers
	//have only one NIC (yuck ;)
	GetNodeProp(uTargetNode,"cIPv4",cTargetNodeIPv4);
	if(!cTargetNodeIPv4[0])
	{
		printf("CloneContainer() error: Could not determine cTargetNodeIPv4. uTargetNode=%u;\n",uTargetNode);
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
	//6-. conditionally start new veid
	//7-. update source container status
	//8-. update target container status
	//9-. remove /vz/dump files

	//Most specific tConfiguration is used. This allows for some nodes to be set global
	//and others specific. But is slower than the other option with what maybe
	//very large numbers of per node tConfiguration entries.
	char cSSHOptions[256]={""};
	GetConfiguration("cSSHOptions",cSSHOptions,guDatacenter,guNode,0,0);//First try node specific
	if(!cSSHOptions[0])
	{
		GetConfiguration("cSSHOptions",cSSHOptions,guDatacenter,0,0,0);//Second try datacenter wide
		if(!cSSHOptions[0])
			GetConfiguration("cSSHOptions",cSSHOptions,0,0,0,0);//Last try global
	}
	//Default for less conditions below
	if(!cSSHOptions[0] || uNotValidSystemCallArg(cSSHOptions))
		sprintf(cSSHOptions,"-p 22");

	char cSCPOptions[256]={""};
	GetConfiguration("cSCPOptions",cSCPOptions,guDatacenter,guNode,0,0);//First try node specific
	if(!cSCPOptions[0])
	{
		GetConfiguration("cSCPOptions",cSCPOptions,guDatacenter,0,0,0);//Second try datacenter wide
		if(!cSCPOptions[0])
			GetConfiguration("cSCPOptions",cSCPOptions,0,0,0,0);//Last try global
	}
	//Default for less conditions below
	if(!cSCPOptions[0] || uNotValidSystemCallArg(cSCPOptions))
		sprintf(cSCPOptions,"-P 22");

	char cSnapshotDir[256]={""};
	GetConfiguration("cSnapshotDir",cSnapshotDir,guDatacenter,guNode,0,0);//First try node specific
	if(!cSnapshotDir[0])
	{
		GetConfiguration("cSnapshotDir",cSnapshotDir,guDatacenter,0,0,0);//Second try datacenter wide
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
		printf("CloneContainer() error: %s.\n",gcQuery);
		tJobErrorUpdate(uJob,"error 1");
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
		printf("CloneContainer() error: %s.\n",gcQuery);
		tJobErrorUpdate(uJob,"error 2");
		goto CommonExit;
	}

	//3-.
	sprintf(gcQuery,"ssh %s %s '/usr/sbin/vzdump --compress --restore /vz/dump/vzdump-%u.tgz %u'",
				cSSHOptions,cTargetNodeIPv4,uContainer,uNewVeid);
	if(system(gcQuery))
	{
		printf("CloneContainer() error: %s.\n",gcQuery);
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
	if(system(gcQuery))
	{
		printf("CloneContainer() error: %s.\n",gcQuery);
		tJobErrorUpdate(uJob,"error 4a");
		goto CommonExit;
	}

	//4b-.
	sprintf(gcQuery,"ssh %s %s 'vzctl set %u --name %s --save'",
				cSSHOptions,cTargetNodeIPv4,uNewVeid,cName);
	if(system(gcQuery))
	{
		printf("CloneContainer() error: %s.\n",gcQuery);
		tJobErrorUpdate(uJob,"error 4b");
		goto CommonExit;
	}

	//4c-.
	//Some containers have more than one IP when how? Not via jobqueue.c
	//Only via mount/umount scripts other IPs are used.
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
		printf("CloneContainer() error: %s.\n",gcQuery);
		tJobErrorUpdate(uJob,"error 4c");
		goto CommonExit;
	}
	//4d-.
	sprintf(gcQuery,"ssh %s %s 'vzctl set %u --ipadd %s --save'",
				cSSHOptions,cTargetNodeIPv4,uNewVeid,cNewIP);
	if(system(gcQuery))
	{
		printf("CloneContainer() error: %s.\n",gcQuery);
		tJobErrorUpdate(uJob,"error 4d");
		goto CommonExit;
	}

	//5-. Remove umount and mount files if found
	//Business logic: Target container may have these files, but we need them only if we
	//'Failover' to this cloned VE. Also we defer to that action the setup of the
	//containers tProperty values needed for processing the umount/mount templates:
	//cNetmask, cExtraNodeIP, cPrivateIPs, cService1, cService2, cVEID.mount and cVEID.umount.
	sprintf(gcQuery,"ssh %3$s %1$s 'rm -f /etc/vz/conf/%2$u.umount /etc/vz/conf/%2$u.mount'",
				cTargetNodeIPv4,uNewVeid,cSSHOptions);
	if(system(gcQuery))
	{
		printf("CloneContainer() error: %s.\n",gcQuery);
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
			printf("CloneContainer() error: %s.\n",gcQuery);
			tJobErrorUpdate(uJob,"error 6");
			goto CommonExit;
		}
		SetContainerStatus(uNewVeid,1);//Active
	}
	else
	{
		SetContainerStatus(uNewVeid,31);//Stopped
	}

	//if(uDebug)
	//	return;

	//7-. 8-. Everything ok
	SetContainerStatus(uContainer,1);//Active
	tJobDoneUpdate(uJob);

	//9a-. local
	if(!cSnapshotDir[0])
		sprintf(gcQuery,"rm /vz/dump/vzdump-%u.tgz",uContainer);
	else
		sprintf(gcQuery,"rm %s/vzdump-%u.tgz",cSnapshotDir,uContainer);
	if(system(gcQuery))
		printf("CloneContainer() Warning error: %s.\n",gcQuery);
	
	//9b-. remote
	sprintf(gcQuery,"ssh %s %s 'rm /vz/dump/vzdump-%u.tgz'",cSSHOptions,cTargetNodeIPv4,uContainer);
	if(system(gcQuery))
		printf("CloneContainer() Warning error: %s.\n",gcQuery);


CommonExit:
	//9c-. remove lock file
	unlink("/var/run/vzdump.lock");

	return;

}//void CloneContainer(...)


//Required by libtemplate
void AppFunctions(FILE *fp,char *cFunction)
{
}//void AppFunctions(FILE *fp,char *cFunction)


int CreateMountFiles(unsigned uContainer, unsigned uOverwrite)
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
	struct stat statInfo;



	sprintf(cVeID,"%u",uContainer);	

	GetContainerProp(uContainer,"cVEID.mount",cTemplateName);
	sprintf(cFile,"/etc/vz/conf/%u.mount",uContainer);
	//Do not overwrite existing mount files! Do that before if you really want to.
	//stat returns 0 if file exists
	if(cTemplateName[0] && (stat(cFile,&statInfo) || uOverwrite))
	{
		GetContainerProp(uContainer,"cExtraNodeIP",cExtraNodeIP);
		if(!cExtraNodeIP[0])
		{
			//Allow backwards compatability
			GetContainerProp(uContainer,"cNodeIP",cExtraNodeIP);
			//if(!cExtraNodeIP[0])
			//{
			//	printf("CreateMountFiles() error: required cExtraNodeIP container property not found\n");
			//	goto CommonExit;
			//}
		}

		GetContainerProp(uContainer,"cNetmask",cNetmask);
		GetContainerProp(uContainer,"cPrivateIPs",cPrivateIPs);
		GetContainerProp(uContainer,"cService1",cService1);
		GetContainerProp(uContainer,"cService2",cService2);

		if((fp=fopen(cFile,"w"))==NULL)
		{
			printf("CreateMountFiles() error: %s\n",cFile);
			goto CommonExit;
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
					
			template.cpName[6]="";
			Template(field2[0],&template,fp);
		}
		mysql_free_result(res2);
		fclose(fp);
		chmod(cFile,S_IRUSR|S_IWUSR|S_IXUSR);

		cTemplateName[0]=0;
		GetContainerProp(uContainer,"cVEID.umount",cTemplateName);
		sprintf(cFile,"/etc/vz/conf/%u.umount",uContainer);
		if(cTemplateName[0] && (stat(cFile,&statInfo) || uOverwrite) )
		{
			if((fp=fopen(cFile,"w"))==NULL)
			{
				printf("CreateMountFiles() error: %s\n",cFile);
				goto CommonExit;
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
						
				template.cpName[6]="";
				Template(field2[0],&template,fp);
			}
			mysql_free_result(res2);
			fclose(fp);
			chmod(cFile,S_IRUSR|S_IWUSR|S_IXUSR);
		}
		else
		{
			printf("CreateMountFiles() error: %s missing\n",cTemplateName);
			goto CommonExit;
		}
	}

	return(0);

CommonExit:
	return(1);

}//int CreateMountFiles()


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
				printf("SECURITY ALERT uNotValidSystemCallArg\n");
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
		printf("LocalImportTemplate() error: Could not determine uOSTemplate\n");
		tJobErrorUpdate(uJob,"uOSTemplate=0");
		goto CommonExit;
	}

	sprintf(gcQuery,"SELECT cLabel FROM tOSTemplate"
			" WHERE uOSTemplate=%u",uOSTemplate);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tOSTemplate.cLabel");
		goto CommonExit;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cOSTemplateFile,"%.54s%.100s.tar.gz",cOSTemplateFilePath,field[0]);
	}
	else
	{
		mysql_free_result(res);
		printf("LocalImportTemplate() error: Could not determine tOSTemplate.cLabel\n");
		tJobErrorUpdate(uJob,"tOSTemplate.cLabel");
		goto CommonExit;
	}
	mysql_free_result(res);
	if(uNotValidSystemCallArg(cOSTemplateFile))
	{
		tJobErrorUpdate(uJob,"failed sec alert!");
		goto CommonExit;
	}

	//2-. stat file
	if(stat(cOSTemplateFile,&statInfo)!=0)
	{
		printf("Could not find %s\n",cOSTemplateFile);
		tJobErrorUpdate(uJob,"cOSTemplateFile stat");
		goto CommonExit;
	}

	//3-. stat file.md5sum
	sprintf(cOSTemplateFileMd5sum,"%.154s.md5sum",cOSTemplateFile);
	if(stat(cOSTemplateFileMd5sum,&statInfo)!=0)
	{
		printf("Could not find %s\n",cOSTemplateFileMd5sum);
		tJobErrorUpdate(uJob,"cOSTemplateFilemd5sum stat");
		goto CommonExit;
	}

	//4-. check md5sum
	sprintf(gcQuery,"/usr/bin/md5sum -c %s > /dev/null 2>&1",cOSTemplateFileMd5sum);
	if(system(gcQuery))
	{
		printf("md5sum -c %s failed!\n",cOSTemplateFileMd5sum);
		tJobErrorUpdate(uJob,"md5sum -c failed!");
		goto CommonExit;
	}


	//5-. copy to all same datacenter nodes nicely (hopefully on GB 2nd NIC internal lan.)
	char cSCPOptions[256]={""};
	GetConfiguration("cSCPOptions",cSCPOptions,guDatacenter,guNode,0,0);//First try node specific
	if(!cSCPOptions[0])
	{
		GetConfiguration("cSCPOptions",cSCPOptions,guDatacenter,0,0,0);//Second try datacenter wide
		if(!cSCPOptions[0])
			GetConfiguration("cSCPOptions",cSCPOptions,0,0,0,0);//Last try global
	}
	//Default for less conditions below
	if(!cSCPOptions[0] || uNotValidSystemCallArg(cSCPOptions))
		sprintf(cSCPOptions,"-P 22 -c blowfish");
	sprintf(gcQuery,"SELECT cLabel FROM tNode WHERE uDatacenter=%u AND uNode!=%u",guDatacenter,guNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tNode.cLabel");
		goto CommonExit;
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"nice /usr/bin/scp %s %s %s:%s\n",cSCPOptions,cOSTemplateFile,field[0],cOSTemplateFile);
		if(system(gcQuery))
		{
			mysql_free_result(res);
			printf("%s failed!\n",gcQuery);
			tJobErrorUpdate(uJob,"scp-1 failed!");
			goto CommonExit;
		}
		sprintf(gcQuery,"nice /usr/bin/scp %s %s %s:%s\n",cSCPOptions,cOSTemplateFileMd5sum,field[0],
						cOSTemplateFileMd5sum);
		if(system(gcQuery))
		{
			mysql_free_result(res);
			printf("%s failed!\n",gcQuery);
			tJobErrorUpdate(uJob,"scp-1 failed!");
			goto CommonExit;
		}
	}
	mysql_free_result(res);

	//Everything ok
	tJobDoneUpdate(uJob);

CommonExit:
	return;

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
		printf("LocalImportConfig() error: Could not determine uConfig\n");
		tJobErrorUpdate(uJob,"uConfig=0");
		goto CommonExit;
	}

	sprintf(gcQuery,"SELECT cLabel FROM tConfig"
			" WHERE uConfig=%u",uConfig);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tConfig.cLabel");
		goto CommonExit;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cConfigFile,"%.54sve-%.100s.conf-sample",cConfigFilePath,field[0]);
	}
	else
	{
		mysql_free_result(res);
		printf("LocalImportConfig() error: Could not determine tConfig.cLabel\n");
		tJobErrorUpdate(uJob,"tConfig.cLabel");
		goto CommonExit;
	}
	mysql_free_result(res);
	if(uNotValidSystemCallArg(cConfigFile))
	{
		tJobErrorUpdate(uJob,"failed sec alert!");
		goto CommonExit;
	}

	//2-. stat file
	if(stat(cConfigFile,&statInfo)!=0)
	{
		printf("Could not find %s\n",cConfigFile);
		tJobErrorUpdate(uJob,"cConfigFile stat");
		goto CommonExit;
	}

	//3-. copy to all same datacenter nodes nicely (hopefully on GB 2nd NIC internal lan.)
	char cSCPOptions[256]={""};
	GetConfiguration("cSCPOptions",cSCPOptions,guDatacenter,guNode,0,0);//First try node specific
	if(!cSCPOptions[0])
	{
		GetConfiguration("cSCPOptions",cSCPOptions,guDatacenter,0,0,0);//Second try datacenter wide
		if(!cSCPOptions[0])
			GetConfiguration("cSCPOptions",cSCPOptions,0,0,0,0);//Last try global
	}
	//Default for less conditions below
	if(!cSCPOptions[0] || uNotValidSystemCallArg(cSCPOptions))
		sprintf(cSCPOptions,"-P 22 -c blowfish");
	sprintf(gcQuery,"SELECT cLabel FROM tNode WHERE uDatacenter=%u AND uNode!=%u",guDatacenter,guNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		tJobErrorUpdate(uJob,"SELECT tNode.cLabel");
		goto CommonExit;
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
			printf("%s failed!\n",gcQuery);
			tJobErrorUpdate(uJob,"scp failed!");
			goto CommonExit;
		}
	}
	mysql_free_result(res);

	//Everything ok
	tJobDoneUpdate(uJob);

CommonExit:
	return;

}//void LocalImportConfig(unsigned uJob,unsigned uContainer,const char *cJobData)


void FailoverTo(unsigned uJob,unsigned uContainer,const char *cJobData)
{
	unsigned uStatus=0;
	unsigned uSourceContainer=0;
	char cIP[32]={""};
	
	//Get this cloned container source
	if(GetContainerSource(uContainer,&uSourceContainer))
	{
		printf("FailoverTo() error1: GetContainerSource()\n");
		tJobErrorUpdate(uJob,"No uSourceContainer");
		goto CommonExit;
	}

	//Get data about container
	if(GetContainerStatus(uContainer,&uStatus))
	{
		printf("FailoverTo() error1: GetContainerStatus()\n");
		tJobErrorUpdate(uJob,"No uStatus");
		goto CommonExit;
	}

	//1-.
	//If container is stopped must start. Since for unknown reasons
	//the container may already be running (vzctl returns 32 in the version we have at this time) 
	//we do not consider that start error to be fatal for now.
	if(uStatus==uSTOPPED)	
	{
		unsigned uReturn=0;

		sprintf(gcQuery,"/usr/sbin/vzctl --verbose start %u ",uContainer);
		if((uReturn=system(gcQuery)))
		{
			if(uReturn==32)
			{
				printf("FailoverTo() error3a: %s\n",gcQuery);
			}
			else
			{
				printf("FailoverTo() error3b: %s\n",gcQuery);
				tJobErrorUpdate(uJob,"start error");
				goto CommonExit;
			}
		}
	}

	//2-.
	//We remove all the previous IPs.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
	if(system(gcQuery))
	{
		printf("FailoverTo() error4: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"ipdel all");
		goto CommonExit;
	}

	//3-.
	//We add (from the source container!) (any other IPs?) first the main IP
	if(GetContainerMainIP(uSourceContainer,cIP))
	{
		printf("FailoverTo() error5: %s\n",cIP);
		tJobErrorUpdate(uJob,"GetContainerMainIP");
		goto CommonExit;
	}
	if(uNotValidSystemCallArg(cIP))
	{
		printf("FailoverTo() error6: security error for %s\n",cIP);
		tJobErrorUpdate(uJob,"cIP security");
		goto CommonExit;
	}
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipadd %s --save",uContainer,cIP);
	if(system(gcQuery))
	{
		printf("FailoverTo() error7: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"ipadd cIP");
		goto CommonExit;
	}
	//3b-.
	//No we can update the db for the new production container
	//Non fatal so we can get this failover done.
	if(SetContainerIP(uContainer,cIP))
		printf("FailoverTo() error7b: SetContainerIP(%u,%s)\n",uContainer,cIP);

	//4-.
	//When we clone we purposefully remove any mount/umount scripts
	//Here we must add them.
	//Trouble is that these scripts must allow for failover.
	//for example if strange firewall rules or IP ranges are used
	//these scripts will not be portable --even in the same datacenter!
	if(CreateMountFiles(uContainer,1))
	{
		printf("FailoverTo() error8: CreateMountFiles(x,1)\n");
		tJobErrorUpdate(uJob,"CreateMountFiles(x,1)");
		goto CommonExit;
	}


	//Everything ok
	SetContainerStatus(uContainer,uACTIVE);
	tJobDoneUpdate(uJob);

CommonExit:
	return;
}//void FailoverTo()


void FailoverFrom(unsigned uJob,unsigned uContainer,const char *cJobData)
{
	unsigned uCloneContainer=0;
	char cIP[32]={""};

	//0-.
	sscanf(cJobData,"uCloneContainer=%u;",&uCloneContainer);
	if(!uCloneContainer)
	{
		printf("FailoverFrom() error0: no uCloneContainer\n");
		tJobErrorUpdate(uJob,"no uCloneContainer");
		goto CommonExit;
	}

	//1-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose set %u --ipdel all --save",uContainer);
	if(system(gcQuery))
	{
		printf("FailoverFrom() error1: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"ipdel all");
		goto CommonExit;
	}


	//2-.
	sprintf(gcQuery,"/usr/sbin/vzctl --verbose stop %u",uContainer);
	if(system(gcQuery))
	{
		printf("FailoverFrom() error2: %s\n",gcQuery);
		tJobErrorUpdate(uJob,"vzctl stop");
		goto CommonExit;
	}
	SetContainerStatus(uContainer,uSTOPPED);

	//3-.
	//Change IP over to the one the clone used to have
	if(GetContainerMainIP(uCloneContainer,cIP))
	{
		printf("FailoverFrom() error3: GetContainerMainIP(%u,%s)\n",uCloneContainer,cIP);
	}
	else
	{
		if(SetContainerIP(uContainer,cIP))
			printf("FailoverFrom() error3a: SetContainerIP(%u,%s)\n",uContainer,cIP);
	}

	//4-. Remove any mount/umount files.
	sprintf(gcQuery,"rm -f /etc/vz/conf/%1$u.umount /etc/vz/conf/%1$u.mount'",uContainer);
	if(system(gcQuery))
	{
		printf("FailoverFrom() error4: %s.\n",gcQuery);
		tJobErrorUpdate(uJob,"rm mount files");
		goto CommonExit;
	}

	//Everything ok
	tJobDoneUpdate(uJob);

CommonExit:
	return;
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
		printf("%s\n",mysql_error(&gMysql));
		return(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",uStatus);
	}
	else
	{
		mysql_free_result(res);
		return(3);
	}
	mysql_free_result(res);
	return(0);

}//void GetContainerStatus()


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
		printf("%s\n",mysql_error(&gMysql));
		return(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cIP,"%.31s",field[0]);
	}
	else
	{
		mysql_free_result(res);
		return(3);
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
		printf("%s\n",mysql_error(&gMysql));
		return(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",uSource);
	}
	else
	{
		mysql_free_result(res);
		return(3);
	}
	mysql_free_result(res);
	return(0);

}//void GetContainerSource()


unsigned SetContainerIP(const unsigned uContainer,char *cIP)
{
	if(uContainer==0) return(1);

	sprintf(gcQuery,"UPDATE tContainer SET uIPv4=(SELECT uIP FROM tIP WHERE cLabel='%s' LIMIT 1) WHERE"
				" uContainer=%u",cIP,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		return(2);
	}
	return(0);

}//void SetContainerIP()

