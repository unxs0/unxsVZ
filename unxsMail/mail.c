/*
FILE
	mail.c
	svn ID removed
PURPOSE
	Mostly command line functions and common functions that are used in more than
	one tXfunc.h file.
AUTHOR/LEGAL
	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice.
*/


#include "mysqlrad.h"
#include <ctype.h>
#include <shadow.h>
#include <errno.h>
#include <grp.h>
#include "mail.h"
#include <openisp/template.h>

int NewUserJob(unsigned uJob,unsigned uUser);
int ModUserJob(unsigned uJob, unsigned uUser);
int DelUserJob(unsigned uJob, unsigned uUser);
int UserPerms(char *cLogin, char *cUserDir);
int AllRelatedJobsDone(unsigned uJob);
int NewServerConfigJob(unsigned uJob);
int ModServerConfigJob(unsigned uJob);
int DelServerConfigJob(unsigned uJob);
int NewVUTConfigJob(unsigned uJob);
int NewAccessConfigJob(unsigned uJob);
int MakeAccessFile(unsigned uServer);
int NewLocalConfigJob(unsigned uJob);
int MakeLocalFile(unsigned uServer);
int NewRelayConfigJob(unsigned uJob);
int MakeRelayFile(unsigned uServer);
int NewAliasFile(unsigned uServer);
int MakeAliasConfigJob(unsigned uJob);
int MakeWhiteListConfigJob(unsigned uJob);
int MakeBlackListConfigJob(unsigned uJob);
void UpdateSystemPasswd(void);
void PrepHomeDirs(unsigned uServer);
void ProcessJobQueue(char *cServer);
void UpdateJobStatus(unsigned uJob,unsigned uJobStatus);
void UpdateUserStatus(unsigned uUser,unsigned uStatus);

unsigned uGetServerGroup(unsigned uServer);
unsigned uFileExists(char *cPath,unsigned uSymLink);
unsigned PrepSingleUserHomeDir(unsigned uUID,char const *cLogin);
unsigned MakeUserPrefs(unsigned uUser);
int UpdateVacationStatus(unsigned uUser);
int UpdateUserConfig(unsigned uUser,unsigned uJob,unsigned uNew);

//These should be common to all our RAD3 backends
int ChangeSystemPasswd(char *cLogin, char *cPasswd);
void GetConfiguration(const char *cName, char *cValue, unsigned uServer, unsigned uHtml);
unsigned GetuServer(const char *cLabel);
void CrontabConnectDb(void);
void TextError(const char *cError, unsigned uContinue);

//File global
static unsigned uServer=0;

//Externl mysqlISP2 job queue processing
typedef struct
{
	char cServerGroup[33];
	char cDomain[100];
	char cLogin[65];
	char cPasswd[65];
	char ctVUTEntry[256];
	char cUserType[33];
	unsigned uCreateVUTEntry;
	
	unsigned uISPClient;
	char cClientName[33];

}  structExtJobParameters;

void ProcessExtJobQueue(char *cServer);
void InitializeParams(structExtJobParameters *structExtParam);
void ParseExtParams(structExtJobParameters *structExtParam, char *cJobData);
int InformExtJob(char *cRemoteMsg,char *cServer,unsigned uJob,unsigned uJobStatus);
int SubmitISPJob(char *cJobName,char *cJobData,char *cServer,unsigned uJobDate);
unsigned uRowId(char *cTableName,char *cIndex,char *cValue);
unsigned uGetClientOwner(unsigned uClient);
//mysqlISP constants
//tJob.uJobStatus
#define mysqlISP_RemotelyQueued 7
#define mysqlISP_Waiting 10
#define mysqlISP_Running 31
#define mysqlISP_Error 21
//tInstance.uStatus:
#define mysqlISP_Deployed 4
#define mysqlISP_Canceled 5
#define mysqlISP_OnHold 6
void to64(register char *s, register long v, register int n);

//tuserfunc.h
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServerGroup,char *cJobData,
		unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy);
MYSQL mysqlext;

#define POSTFIX_SERVER_GROUP 1

void ExtConnectDb(unsigned uHtml)
{
	char cDbIp[256]={""};
	char *cEffectiveDbIp=NULL;
	char cDbName[256]={"unxsisp"};
	char cDbPwd[256]={"wsxedc"};
	char cDbLogin[256]={"unxsisp"};

	GetConfiguration("cExtJobQueueDbIp",cDbIp,0,1);
	GetConfiguration("cExtJobQueueDbName",cDbName,0,1);
	GetConfiguration("cExtJobQueueDbPwd",cDbPwd,0,1);
	GetConfiguration("cExtJobQueueDbLogin",cDbLogin,0,1);

	//Debug only	
	//printf("%s %s %s %s\n",cDbIp,cDbName,cDbPwd,cDbLogin);

	if(cDbIp[0]) cEffectiveDbIp=cDbIp;

        mysql_init(&mysqlext);
        if (!mysql_real_connect(&mysqlext,cEffectiveDbIp,
				cDbLogin,cDbPwd,cDbName,0,NULL,0))
        {
		sprintf(gcQuery,"ExtConnectDb failed for %s.%s",cDbIp,cDbName);
		if(uHtml)
                	htmlPlainTextError(gcQuery);
		else
			fprintf(stderr,"%s\n",gcQuery);
		exit(1);
        }

}//void ExtConnectDb(unsigned uHtml)


void ParseExtParams(structExtJobParameters *structExtParam, char *cJobData)
{
	char *cp,*cp2;
	
	if((cp=strstr(cJobData,"ServerGroup=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+12,'\n'))) *cp2=0;
		sprintf(structExtParam->cServerGroup,"%.33s",cp+12);
		if(cp2) *cp2='\n';
	}

	if((cp=strstr(cJobData,"UserType=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+9,'\n'))) *cp2=0;
		sprintf(structExtParam->cUserType,"%.33s",cp+9);
		if(cp2) *cp2='\n';
	}

	if((cp=strstr(cJobData,"Domain=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+7,'\n'))) *cp2=0;
		sprintf(structExtParam->cDomain,"%.99s",cp+7);
		if(cp2) *cp2='\n';
	}
	
	if((cp=strstr(cJobData,"Login=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+6,'\n'))) *cp2=0;
		sprintf(structExtParam->cLogin,"%.64s",cp+6);
		if(cp2) *cp2='\n';
	}
	
	if((cp=strstr(cJobData,"Passwd=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+8,'\n'))) *cp2=0;
		sprintf(structExtParam->cPasswd,"%.64s",cp+7);
		if(cp2) *cp2='\n';
	}
		
	if((cp=strstr(cJobData,"tVUTEntry=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+10,'\n'))) *cp2=0;
		sprintf(structExtParam->ctVUTEntry,"%.255s",cp+10);
		if(cp2) *cp2='\n';
		if(structExtParam->ctVUTEntry[0])
			structExtParam->uCreateVUTEntry=1;
	}


	if((cp=strstr(cJobData,"ISPClient=")))
	{
		sscanf(cp+10,"%u",&structExtParam->uISPClient);
	}
	if((cp=strstr(cJobData,"ClientName=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+11,'\n'))) *cp2=0;
		sprintf(structExtParam->cClientName,"%.32s",cp+11);
		if(cp2) *cp2='\n';
	}
	
}//void ParseExtParams(structExtJobParameters *structExtParam, char *cJobData)


void InitializeParams(structExtJobParameters *structExtParam)
{
	structExtParam->cServerGroup[0]=0;
	structExtParam->cDomain[0]=0;
	structExtParam->cLogin[0]=0;
	structExtParam->cPasswd[0]=0;
	structExtParam->ctVUTEntry[0]=0;
	structExtParam->cUserType[0]=0;
	structExtParam->uISPClient=0;
	structExtParam->cClientName[0]=0;
	
}//void InitializeParams(structExtJobParameters *structExtParam)


unsigned uRowId(char *cTableName,char *cIndex,char *cValue)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uRow=0;
	
	sprintf(gcQuery,"SELECT _rowid FROM %s WHERE %s='%s'",cTableName,cIndex,cValue);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
	
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uRow);
	mysql_free_result(res);

	return(uRow);
	
}//unsigned uRowId(char *cTableName,char *cIndex,char *cValue)


unsigned uGetClientOwner(unsigned uClient)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uRet=0;

	sprintf(gcQuery,"SELECT uOwner FROM "TCLIENT"  WHERE uClient=%u",uClient);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uRet);
	mysql_free_result(res);

	return(uRet);

}//unsigned uGetClientOwner(unsigned uClient)


void ProcessExtJobQueue(char *cServer)
{
	//
	//Very important note:
	//The present ProcessExtJobQueue() code will work as is for mysqlISP2 shared tClient/tAuthorize (recommended) setups
	
	MYSQL_RES *res;
	MYSQL_ROW field;

	MYSQL_RES *res2;
	MYSQL_ROW field2;

	time_t clock;

	structExtJobParameters structExtParam;

	unsigned uJob=0;
	unsigned uDomain=0;
	unsigned uUser=0;
	unsigned uJobClient=0;
	unsigned uOwner=0;

	char cJobData[2048]={""};

	CrontabConnectDb();
	ExtConnectDb(0);

	time(&clock);
	sprintf(gcQuery,"SELECT cJobName,cJobData,uJob,uJobClient FROM tJob WHERE (cServer='Any' OR cServer='%s') AND uJobStatus=%u "
			"AND uJobDate<=%lu AND cJobName LIKE 'unxsMail.%%'",cServer,mysqlISP_Waiting,clock);

	//Debug only	
	//printf("%s\n",gcQuery);
	//exit(0);

	mysql_query(&mysqlext,gcQuery);
        if(mysql_errno(&mysqlext))
	{
		fprintf(stderr,"%s\n",mysql_error(&mysqlext));
		return;
	}

	res=mysql_store_result(&mysqlext);
        while((field=mysql_fetch_row(res)))
	{
		InitializeParams(&structExtParam);
				
		sscanf(field[2],"%u",&uJob);
		sscanf(field[3],"%u",&uJobClient);
		if(!(uOwner=uGetClientOwner(uJobClient))) uOwner=1; //Safe default. Should inform? How?

		if(!strcmp("unxsMail.MailBox.New",field[0]))
		{
			printf("unxsMail.MailBox.New\n");

			ParseExtParams(&structExtParam,field[1]);
		
			InformExtJob("Running job",cServer,uJob,mysqlISP_Running);

			//Create the tDomain record if needed
			uDomain=uRowId("tDomain","cDomain",structExtParam.cDomain);
			if(!uDomain)
			{
				sprintf(gcQuery,"INSERT INTO tDomain SET cDomain='%s',uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
						structExtParam.cDomain,uOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					TextError(mysql_error(&gMysql),1);
					InformExtJob("tDomain INSERT failed",cServer,uJob,mysqlISP_Error);
					continue;
				}
				uDomain=mysql_insert_id(&gMysql);
			}

			//Create the tUser record
			sprintf(gcQuery,"INSERT INTO tUser SET cLogin='%s',uDomain='%u',cPasswd='%s',uUserType='%u',uServerGroup='%u',uStatus='%u',"
					"uClient=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
					structExtParam.cLogin
					,uRowId("tDomain","cDomain",structExtParam.cDomain)
					,structExtParam.cPasswd
					,uRowId("tUserType","cLabel",structExtParam.cUserType)
					,uRowId("tServerGroup","cLabel",structExtParam.cServerGroup)
					,STATUS_PENDING
					,uJobClient
					,uOwner
					);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				TextError(mysql_error(&gMysql),1);
				InformExtJob("tUser INSERT failed",cServer,uJob,mysqlISP_Error);
				continue;
			}
			uUser=mysql_insert_id(&gMysql);
			//Create the tVUT record if needed
			
			if(structExtParam.uCreateVUTEntry)
			{
				unsigned uVUT=0;
				unsigned uVUTEntries=0;
				char cVirtualEmail[128]={""};
				char cTargetEmail[128]={""};
				
				printf("structExtParam.uCreateVUTEntry=1\n");
		
				sprintf(cVirtualEmail,strtok(structExtParam.ctVUTEntry,";"));
				sprintf(cTargetEmail,strtok(NULL,";"));

				if(!cVirtualEmail[0] || !cTargetEmail[0])
				{
					InformExtJob("Invalid cJobData for VUT entry creation",cServer,uJob,mysqlISP_Error);
					continue;
				}
				
				//
				//Before creating a new tVUT record we must test if we don't have it already!
				sprintf(gcQuery,"SELECT uVUT FROM tVUT WHERE cDomain='%s' AND uServerGroup='%u'",
						structExtParam.cDomain
						,uRowId("tServerGroup","cLabel",structExtParam.cServerGroup)
					);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					TextError(mysql_error(&gMysql),1);
					InformExtJob("tVUT SELECT failed",cServer,uJob,mysqlISP_Error);
					continue;
				}

				res=mysql_store_result(&gMysql);

				if((field=mysql_fetch_row(res)))
					sscanf(field[0],"%u",&uVUT);

				if(!uVUT)
				{
			sprintf(gcQuery,"INSERT INTO tVUT SET cDomain='%s',uServerGroup='%u',uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
					structExtParam.cDomain
					,uRowId("tServerGroup","cLabel",structExtParam.cServerGroup)
					,uOwner
					);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						TextError(mysql_error(&gMysql),1);
						InformExtJob("tVUT INSERT failed",cServer,uJob,mysqlISP_Error);
						continue;
					}
				
					uVUT=mysql_insert_id(&gMysql);
				}
				
				//Can't be a tVUTEntries record for the same cTargetEmail
				if(!uRowId("tVUTEntries","cTargetEmail",cTargetEmail))
				{
					sprintf(gcQuery,"INSERT INTO tVUTEntries SET uVUT='%u',cVirtualEmail='%s',cTargetEmail='%s',uOwner=%u,"
							"uClient=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
							uVUT
							,cVirtualEmail
							,cTargetEmail
							,uOwner
							,uJobClient
							);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						TextError(mysql_error(&gMysql),1);
						InformExtJob("tVUTEntries INSERT failed",cServer,uJob,mysqlISP_Error);
						continue;
					}
				
					uVUTEntries=mysql_insert_id(&gMysql);
				
					sprintf(cJobData,"uVUTEntries=%u;\ncVirtualEmail=%s;\ncTargetEmail=%s;\ncDomain=%s;\nmysqlISP2.tJob.uJob=%u\n",
							uVUTEntries
							,cVirtualEmail
							,cTargetEmail
							,structExtParam.cDomain
							,uJob
							);
						SubmitJob("ExtNewVUT",structExtParam.cDomain,cVirtualEmail,structExtParam.cServerGroup,cJobData,0,0,uOwner,1);
				}
			}//if(structExtParam.uCreateVUTEntry)
			

			printf("Done Ok\n");
			//If no errors submit jobs
			sprintf(cJobData,"mysqlISP2.tJob.uJob=%u\n",uJob);
			SubmitJob("ExtNewUser",structExtParam.cDomain,structExtParam.cLogin,structExtParam.cServerGroup,cJobData,uDomain,uUser,uOwner,1);
			
			InformExtJob("Jobs queued at unxsMail",cServer,uJob,mysqlISP_RemotelyQueued);	
			
		}//if(!strcmp("unxsMail.MailBox.New",field[0]))
		
		else if(!strcmp("unxsMail.MailBox.Mod",field[0]))
		{
			ParseExtParams(&structExtParam,field[1]);
			InformExtJob("Running job",cServer,uJob,mysqlISP_Running);

			uUser=uRowId("tUser","cLogin",structExtParam.cLogin);
			
			if(!uUser)
			{
				InformExtJob("No such user",cServer,uJob,mysqlISP_Error);
				continue;
			}
			
			//Only password supported for modification
			
			sprintf(gcQuery,"UPDATE tUser SET cPasswd='%s',uStatus='%u',uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1 WHERE cLogin='%s'",
					structExtParam.cPasswd
					,STATUS_PENDING_MOD
					,structExtParam.cLogin
					);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				TextError(mysql_error(&gMysql),1);
				InformExtJob("tUser UPDATE failed",cServer,uJob,mysqlISP_Error);
				continue;
			}
			
			//If we are modifying an existent user, the previous VUT entry has to be updated
			//So, only updating tVUTEntries if required.
			sprintf(gcQuery,"SELECT uVUTEntries FROM tVUTEntries WHERE cTargetEmail='%s'",structExtParam.cLogin);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				TextError(mysql_error(&gMysql),1);
				InformExtJob("tVUTEntries SELECT failed",cServer,uJob,mysqlISP_Error);
				continue;
			}

			res2=mysql_store_result(&gMysql);
			
			if((field2=mysql_fetch_row(res2)))
			{
				char cVirtualEmail[128]={""};
				char cTargetEmail[128]={""};

				unsigned uVUTEntries=0;

				sscanf(field2[0],"%u",&uVUTEntries);
				sprintf(cVirtualEmail,strtok(structExtParam.ctVUTEntry,";"));
				sprintf(cTargetEmail,strtok(NULL,";"));

				if(!cVirtualEmail[0] || !cTargetEmail[0])
				{
					//Fail if missing any VUT entry parameter
					InformExtJob("Invalid cJobData for VUT entry modification",cServer,uJob,mysqlISP_Error);
					continue;
				}
				
				sprintf(gcQuery,"UPDATE tVUTEntries SET cVirtualEmail='%s',uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1 WHERE cTargetEmail='%s'",
						cVirtualEmail
						,cTargetEmail
						);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					TextError(mysql_error(&gMysql),1);
					InformExtJob("tVUTEntries UPDATE failed",cServer,uJob,mysqlISP_Error);
					continue;
				}
				
				sprintf(cJobData,"uVUTEntries=%u;\ncVirtualEmail=%s;\ncTargetEmail=%s;\ncDomain=%s;\n",
						uVUTEntries
						,cVirtualEmail
						,cTargetEmail
						,structExtParam.cDomain
						);
				SubmitJob("ExtModVUT",structExtParam.cDomain,cVirtualEmail,structExtParam.cServerGroup,cJobData,0,0,uOwner,1);
				
			}//if((field2=mysql_fetch_row(res2)))

			sprintf(cJobData,"mysqlISP2.tJob.uJob=%u\n",uJob);
			SubmitJob("ExtModUser",structExtParam.cDomain,structExtParam.cLogin,structExtParam.cServerGroup,cJobData,uDomain,uUser,uOwner,1);
			
			InformExtJob("Jobs queued at unxsMail",cServer,uJob,mysqlISP_RemotelyQueued);
			
		}//else if(!strcmp("unxsMail.MailBox.Mod",field[0]))

		else if(!strcmp("unxsMail.MailBox.Hold",field[0]))
		{
			//To hold an email account we will save the user password to a db table
			//tHoldUser, upon hold removal we will update the tUser record with that stored
			//password

			ParseExtParams(&structExtParam,field[1]);
			InformExtJob("Running job",cServer,uJob,mysqlISP_Running);

			//Debug only
			//printf("unxsMail.MailBox.Hold\n");
			
			uUser=uRowId("tUser","cLogin",structExtParam.cLogin);
			
			if(!uUser)
			{
				InformExtJob("No such user",cServer,uJob,mysqlISP_Error);
				continue;
			}
			
			sprintf(gcQuery,"SELECT cPasswd FROM tUser WHERE cLogin='%s'",structExtParam.cLogin);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				TextError(mysql_error(&gMysql),1);
				InformExtJob("tUser SELECT failed",cServer,uJob,mysqlISP_Error);
				continue;
			}
			
			res2=mysql_store_result(&gMysql);

			if((field2=mysql_fetch_row(res2)))
			{
				char cPasswd[17]={""};
				
				//Debug only
				//printf("Inserting tHoldUser record\n");
				sprintf(gcQuery,"INSERT INTO tHoldUser SET cLogin='%s',cPasswd='%s',uOwner=%u,uCreatedBy=1,"
						"uCreatedDate=UNIX_TIMESTAMP(NOW())",
						structExtParam.cLogin
						,field2[0]
						,uOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					TextError(mysql_error(&gMysql),1);
					printf("Query was: %s\n",gcQuery);
					InformExtJob("tHoldUser INSERT failed",cServer,uJob,mysqlISP_Error);
					continue;
				}
				//Debug only
				//printf("Updating tUser record\n");
				//Set random password for user
				(void)srand((int)time((time_t *)NULL));
				to64(&cPasswd[0],rand(),16);
				cPasswd[17]=0;
				sprintf(gcQuery,"UPDATE tUser SET cPasswd='%s',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE cLogin='%s'",
						cPasswd
						,structExtParam.cLogin
					);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					TextError(mysql_error(&gMysql),1);
					InformExtJob("tUser UPDATE failed",cServer,uJob,mysqlISP_Error);
					continue;
				}
				
				sprintf(cJobData,"mysqlISP2.tJob.uJob=%u\n",uJob);
				SubmitJob("ExtModUser",structExtParam.cDomain,structExtParam.cLogin,structExtParam.cServerGroup,cJobData,uDomain,uUser,uOwner,1);
				InformExtJob("Jobs queued at unxsMail",cServer,uJob,mysqlISP_RemotelyQueued);
				//Debug only
				//printf("End OK\n");
			}
		
		}//else if(!strcmp("unxsMail.MailBox.Hold",field[0]))

		else if(!strcmp("unxsMail.MailBox.RemoveHold",field[0]))
		{
			//Update tUser with the tHoldUser information and remove tHoldUser record
			
			ParseExtParams(&structExtParam,field[1]);
			InformExtJob("Running job",cServer,uJob,mysqlISP_Running);

			uUser=uRowId("tHoldUser","cLogin",structExtParam.cLogin);
			if(!uUser)
			{
				InformExtJob("No such hold user",cServer,uJob,mysqlISP_Error);
				continue;
			}

			sprintf(gcQuery,"SELECT cPasswd FROM tHoldUser WHERE cLogin='%s'",structExtParam.cLogin);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				TextError(mysql_error(&gMysql),1);
				InformExtJob("tHoldUser SELECT failed",cServer,uJob,mysqlISP_Error);
				continue;
			}

			res2=mysql_store_result(&gMysql);

			if((field2=mysql_fetch_row(res2)))
			{
				sprintf(gcQuery,"UPDATE tUser SET cPasswd='%s',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE cLogin='%s'",
						field[0]
						,structExtParam.cLogin
					);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					TextError(mysql_error(&gMysql),1);
					InformExtJob("tUser UPDATE failed",cServer,uJob,mysqlISP_Error);
					continue;
				}

				sprintf(gcQuery,"DELETE FROM tHoldUser WHERE cLogin='%s'",structExtParam.cLogin);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					TextError(mysql_error(&gMysql),1);
					InformExtJob("tHoldUser DELETE failed",cServer,uJob,mysqlISP_Error);
					continue;
				}

				sprintf(cJobData,"mysqlISP2.tJob.uJob=%u\n",uJob);
				SubmitJob("ExtModUser",structExtParam.cDomain,structExtParam.cLogin,structExtParam.cServerGroup,cJobData,uDomain,uUser,uOwner,1);
				
				InformExtJob("Jobs queued at unxsMail",cServer,uJob,mysqlISP_RemotelyQueued);
			}
	
		}//else if(!strcmp("unxsMail.MailBox.RemoveHold",field[0]))

		else if(!strcmp("unxsMail.MailBox.Cancel",field[0]))
		{
			
			//Remove tUser and tVUT entries associated with the user
			char cVirtualEmail[128]={""};
			char cTargetEmail[128]={""};
			unsigned uVUTEntries=0;

			ParseExtParams(&structExtParam,field[1]);
			InformExtJob("Running job",cServer,uJob,mysqlISP_Running);

			uUser=uRowId("tUser","cLogin",structExtParam.cLogin);

			if(!uUser)
			{
				InformExtJob("No such user",cServer,uJob,mysqlISP_Error);
				continue;
			}

			sprintf(gcQuery,"SELECT uVUTEntries,cVirtualEmail FROM tVUTEntries WHERE cTargetEmail='%s'",structExtParam.cLogin);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				TextError(mysql_error(&gMysql),1);
				InformExtJob("tVUTEntries SELECT failed",cServer,uJob,mysqlISP_Error);
				continue;
			}

			res2=mysql_store_result(&gMysql);

			if((field2=mysql_fetch_row(res2)))
			{
				sscanf(field2[0],"%u",&uVUTEntries);
				sprintf(cVirtualEmail,"%.127s",field2[1]);
				
				sprintf(gcQuery,"DELETE FROM tVUTEntries WHERE cTargetEmail='%s'",structExtParam.cLogin);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					TextError(mysql_error(&gMysql),1);
					InformExtJob("tVUTEntries DELETE failed",cServer,uJob,mysqlISP_Error);
					continue;
				}
			
				sprintf(cJobData,"uVUTEntries=%u;\ncVirtualEmail=%s;\ncTargetEmail=%s;\ncDomain=%s;\nmysqlISP2.tJob.uJob=%u\n\n",
						uVUTEntries
						,cVirtualEmail
						,cTargetEmail
						,structExtParam.cDomain
						,uJob
					);
				SubmitJob("ExtModVUT",structExtParam.cDomain,cVirtualEmail,structExtParam.cServerGroup,cJobData,0,0,uOwner,1);
				sprintf(cJobData,"mysqlISP2.tJob.uJob=%u\n",uJob);
				SubmitJob("ExtDelUser",structExtParam.cDomain,structExtParam.cLogin,structExtParam.cServerGroup,"",uDomain,uUser,uOwner,1);
				InformExtJob("Jobs queued at unxsMail",cServer,uJob,mysqlISP_RemotelyQueued);
					
			}
			sprintf(cJobData,"mysqlISP2.tJob.uJob=%u\n",uJob);
			SubmitJob("ExtDelUser",structExtParam.cDomain,structExtParam.cLogin,structExtParam.cServerGroup,cJobData,uDomain,uUser,uOwner,1);
			InformExtJob("Jobs queued at unxsMail",cServer,uJob,mysqlISP_RemotelyQueued);

		}//else if(!strcmp("unxsMail.MailBox.Cancel",field[0]))

		
		else if(1)
		{
			fprintf(stderr,"Unknown job:%s\n",field[0]);
		}

	}//Main job loop
	
}//void ProcessExtJobQueue(const char *cServer)


int InformExtJob(char *cRemoteMsg,char *cServer,unsigned uJob,unsigned uJobStatus)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uJobGroup=0;
	unsigned uInstance=0;
	time_t clock;

	time(&clock);

	sprintf(gcQuery,"UPDATE tJob SET cServer='%s',cRemoteMsg='%.32s',uModBy=1,uModDate=%lu,uJobStatus=%u WHERE uJob=%u",cServer,cRemoteMsg,clock,uJobStatus,uJob);

	mysql_query(&mysqlext,gcQuery);
	if(mysql_errno(&mysqlext))
	{
		fprintf(stderr,"InformExtJob():%s\n%s\n",mysql_error(&mysqlext),gcQuery);
		SubmitISPJob("unxsMail.InformExtJob.Failed",
				(char *)mysql_error(&mysqlext),cServer,clock);
		return(1);
	}

	//Do not continue unless closing ext job	
	if(uJobStatus!=mysqlISP_Deployed)
		return(0);
	
	//See if all services of client product instance (now grouped via unique 
	//uJobGroup by mysqlISP SubmitJob routines) are now deployed. If they are 
	//then change mysqlISP.tInstance.uStatus to mysqlISP_Deployed, 
	//				mysqlISP_Canceled or mysqlISP_OnHold
	//
	sprintf(gcQuery,"SELECT uJobGroup,uInstance FROM tJob WHERE uJob=%u",uJob);
	mysql_query(&mysqlext,gcQuery);
	if(mysql_errno(&mysqlext))
	{
		fprintf(stderr,"%s\n",mysql_error(&mysqlext));
		return(1);
	}
	res=mysql_store_result(&mysqlext);
        if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uJobGroup);
		sscanf(field[1],"%u",&uInstance);
	}
	mysql_free_result(res);

	if(!uJobGroup || !uInstance)
	{
		fprintf(stderr,"Unexpected missing uJobGroup/uInstance for uJob=%u\n"
						,uJob);
		return(0);
	}

	sprintf(gcQuery,"SELECT uJobStatus=%u,(max(uJobStatus)=min(uJobStatus)),cJobName FROM tJob WHERE uJobGroup=%u GROUP BY uJobGroup",mysqlISP_Deployed,uJobGroup);

	mysql_query(&mysqlext,gcQuery);
	if(mysql_errno(&mysqlext))
	{
		fprintf(stderr,"%s\n",mysql_error(&mysqlext));
	}
	else
	{
		res=mysql_store_result(&mysqlext);
        	if((field=mysql_fetch_row(res)))
		{
			unsigned uInstanceStatus=mysqlISP_Deployed;

			if(field[0][0]=='1' && field[1][0]=='1')
			{
				if(strstr(field[2],".Cancel"))
					uInstanceStatus=mysqlISP_Canceled;
				else if(strstr(field[2],".Hold"))
					uInstanceStatus=mysqlISP_OnHold;

				sprintf(gcQuery,"UPDATE tInstance SET uStatus=%u,uModBy=1,uModDate=%lu WHERE uInstance=%u",uInstanceStatus,clock,uInstance);
				mysql_query(&mysqlext,gcQuery);
				if(mysql_errno(&mysqlext))
					fprintf(stderr,"%s\n",mysql_error(&mysqlext));
			}
		}
		mysql_free_result(res);
	}

	return(0);

}//int InformExtJob()


int SubmitISPJob(char *cJobName,char *cJobData,char *cServer,unsigned uJobDate)
{
	time_t clock;

	time(&clock);
	
	sprintf(gcQuery,"INSERT INTO tJob SET  cServer='%s', cJobName='%s', cJobData='%.1024s', uJobDate=%u, uOwner=1, uCreatedBy=1, uCreatedDate=%lu, uJobStatus=%u, cLabel='mysqlApache2.SubmitISPJob'",
			cServer
			,cJobName
			,TextAreaSave(cJobData)
			,uJobDate
			,clock
			,mysqlISP_Waiting);
	mysql_query(&mysqlext,gcQuery);
	if(mysql_errno(&mysqlext))
	{
                fprintf(stderr,"%s\n",mysql_error(&mysqlext));
		return(1);
	}

	return(0);
}//int SubmitISPJob(char *cJobName,char *cJobData,char *cServer,unsigned uJobDate)


void ProcessJobQueue(char *cServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	time_t luClock=0;
	unsigned uUser=0;
	unsigned uJob=0;
	unsigned uExtJob=0;
	
	time(&luClock);

	if(getuid())
	{
		printf("Only root can run this command\n");
                exit(1);
	}

	CrontabConnectDb();

	if(!(uServer=GetuServer(cServer)))
	{
		printf("Must specify valid cServer. %s is unknown\n",cServer);
                exit(1);
	}
		
        sprintf(gcQuery,"SELECT uJob,cJobName,uJobTarget,uJobTargetUser,cJobData,uJobDate,uJobStatus,cLabel FROM tJob WHERE cServer='%s' AND uJobDate<%lu AND uJobStatus=1 ORDER BY uJobTargetUser LIMIT 20",cServer,luClock);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        
	while((field=mysql_fetch_row(res)))
	{
		//Debug only
		printf("cJobName=%s; cLabel=%s;\n",field[1],field[7]);
		//continue;

		sscanf(field[0],"%u",&uJob);
		UpdateJobStatus(uJob,JOBSTATUS_RUNNING);
		
		if(!strncmp(field[1],"Ext",3))
		{
			//Read mysqlISP2.tJob.uJob from cJobData
			char *cp,*cp2;
			char cJobData[4096]={""};

			sprintf(cJobData,"%.4095s",field[4]);
			if((cp=strstr(cJobData,"mysqlISP2.tJob.uJob=")))
			{
				cp2=NULL;
				if((cp2=strchr(cp+20,'\n'))) *cp2=0;
				sprintf(gcQuery,"%s",cp+20);
				sscanf(gcQuery,"%u",&uExtJob);
				if(cp2) *cp2='\n';
			}
		}
		//tUser and related section
		sscanf(field[3],"%u",&uUser);

		if(!strcmp(field[1],"NewUser") || !strcmp(field[1],"ExtNewUser"))
		{
			if(NewUserJob(uJob,uUser))
			{
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
				if(!strcmp(field[1],"ExtNewUser"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtNewUser() failed",cServer,uExtJob,mysqlISP_Error);
				}
			}			
			else
			{
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
				if(!strcmp(field[1],"ExtNewUser"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtNewUser() OK",cServer,uExtJob,mysqlISP_Deployed);
				}
				UpdateUserStatus(uUser,STATUS_ACTIVE);
			}
		}
		else if(!strcmp(field[1],"ModUser") || !strcmp(field[1],"ExtModUser"))
		{
			if(ModUserJob(uJob,uUser))
			{
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
				if(!strcmp(field[1],"ExtModUser"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtModUser() failed",cServer,uExtJob,mysqlISP_Error);
				}
			}
			else
			{
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
				if(!strcmp(field[1],"ExtModUser"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtModUser() OK",cServer,uExtJob,mysqlISP_Deployed);
				}
				UpdateUserStatus(uUser,STATUS_ACTIVE);
			}
		}
		else if(!strcmp(field[1],"DelUser") || !strcmp(field[1],"ExtDelUser"))
		{
			if(DelUserJob(uJob,uUser))
			{
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
				if(!strcmp(field[1],"ExtDelUser"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtDelUser() failed",cServer,uExtJob,mysqlISP_Error);
				}
			}
			else
			{
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
				if(!strcmp(field[1],"ExtDelUser"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtDelUser() OK",cServer,uExtJob,mysqlISP_Deployed);
				}
			}
		}

		//tServerConfig section
		else if(!strcmp(field[1],"NewServerConfig"))
		{
			if(NewServerConfigJob(uJob))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}
		else if(!strcmp(field[1],"ModServerConfig"))
		{
			if(ModServerConfigJob(uJob))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}
		else if(!strcmp(field[1],"DelServerConfig"))
		{
			if(DelServerConfigJob(uJob))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}

		//Specific well known mail config files
		else if(	!strcmp(field[1],"NewVUT") ||
				!strcmp(field[1],"ModVUT") ||
				!strcmp(field[1],"DelVUT") ||
				!strcmp(field[1],"ExtNewVUT") ||
				!strcmp(field[1],"ExtModVUT") ||
				!strcmp(field[1],"ExtDelVUT") )
		{
			if(NewVUTConfigJob(uJob))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}
		else if(	!strcmp(field[1],"NewAccess") ||
				!strcmp(field[1],"ModAccess") ||
				!strcmp(field[1],"DelAccess") )
		{
			if(NewAccessConfigJob(uJob))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}
		else if(	!strcmp(field[1],"NewLocal") ||
				!strcmp(field[1],"ModLocal") ||
				!strcmp(field[1],"DelLocal") )
		{
			if(NewLocalConfigJob(uJob))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}
		else if(	!strcmp(field[1],"NewRelay") ||
				!strcmp(field[1],"ModRelay") ||
				!strcmp(field[1],"DelRelay") )
		{
			if(NewRelayConfigJob(uJob))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}
		else if(	!strcmp(field[1],"NewAlias") ||
				!strcmp(field[1],"ModAlias") ||
				!strcmp(field[1],"DelAlias") ||
				!strcmp(field[1],"ExtNewAlias") ||
				!strcmp(field[1],"ExtModAlias") ||
				!strcmp(field[1],"ExtDelAlias") )
		{
			if(MakeAliasConfigJob(uJob))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}	
		else if(	!strcmp(field[1],"NewWhiteList") ||
				!strcmp(field[1],"ModWhiteList") ||
				!strcmp(field[1],"DelWhiteList") ||
				!strcmp(field[1],"ExtNewWhiteList") ||
				!strcmp(field[1],"ExtModWhiteList") ||
				!strcmp(field[1],"ExtDelWhiteList") )
		{
			if(MakeWhiteListConfigJob(uJob))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}
		else if(	!strcmp(field[1],"NewBlackList") ||
				!strcmp(field[1],"ModBlackList") ||
				!strcmp(field[1],"DelBlackList") ||
				!strcmp(field[1],"ExtNewBlackList") ||
				!strcmp(field[1],"ExtModBlackList") ||
				!strcmp(field[1],"ExtDelBlackList") )
		{
			if(MakeBlackListConfigJob(uJob))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}
		else if(        !strcmp(field[1],"NewUserPrefs") ||
				!strcmp(field[1],"ModUserPrefs") ||
				!strcmp(field[1],"DelUserPrefs") )
		{
			unsigned uTargetUser=0;
			sscanf(field[3],"%u",&uTargetUser);
			if(MakeUserPrefs(uTargetUser))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}
		else if(        !strcmp(field[1],"NewUserVacation") ||
				!strcmp(field[1],"ModUserVacation") ||
				!strcmp(field[1],"DelUserVacation") )
		{
			unsigned uTargetUser=0;
			sscanf(field[3],"%u",&uTargetUser);
			if(UpdateVacationStatus(uTargetUser))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}
		else if(!strcmp(field[1],"NewUserConfig"))
		{
			unsigned uTargetUser=0;
			sscanf(field[3],"%u",&uTargetUser);
			if(UpdateUserConfig(uTargetUser,uJob,1))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}
		else if(!strcmp(field[1],"ModUserConfig"))
		{
			unsigned uTargetUser=0;
			sscanf(field[3],"%u",&uTargetUser);
			if(UpdateUserConfig(uTargetUser,uJob,0))
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
			else
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
		}
		//No job handler -yet?
		else if(1)
		{
			printf("No such job handler: %s\n",field[1]);
			UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
		}
		
	}//End of while for each job queue entry select

	exit(0);

}//void ProcessJobQueue(const char *cServer)


int NewUserJob(unsigned uJob,unsigned uUser)
{
	//debug only
	printf("NewUserJob(%u,%u)\n",uJob,uUser);
	//return(0);

        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uGID=0;
	unsigned uUID=0;
	struct passwd *structPasswd;
	struct group *structGroup;
	unsigned uRetVal;
	char cUserRoot[256]={"/home"};
	char cUserDir[512]={""};
	char cLogin[36]={""};
	
        sprintf(gcQuery,"SELECT tUser.cLogin,tUser.cPasswd,tDomain.cDomain,tUser.uDomain FROM tUser,tDomain WHERE tUser.uDomain=tDomain.uDomain AND tUser.uUser=%u",uUser);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		if(!(structGroup=getgrgid(MAIL_GID_BASE)))
		{
			sprintf(gcQuery,"NewUserJob() No uGID:%u for uUser:%u\n",
					MAIL_GID_BASE,uUser);
			TextError(gcQuery,1);
			mysql_free_result(res);
			return(1);
		}
			
		uUID=uUser+MAIL_UID_BASE;
		if((structPasswd=getpwuid(uUID)))
		{
			sprintf(gcQuery,"NewUserJob() User with uUID:%u already exists\n",uUID);
			TextError(gcQuery,1);
			mysql_free_result(res);
			return(1);
		}

		//implicit limit of 9999 domains per cluster
		//this is due to the linux login name length limit
		//and our balance in favor of relat. long login names
		
		//User names with xxx..1 only for postfix servers
		if(uGetServerGroup(uServer)==POSTFIX_SERVER_GROUP)		
			sprintf(cLogin,"%.25s..%.4s",field[0],field[3]);
		else
			sprintf(cLogin,"%.25s",field[0]);
		GetConfiguration("cUserRoot",cUserRoot,0,0);
		sprintf(cUserDir,"%s/%s",cUserRoot,cLogin);
		uGID=MAIL_GID_BASE;
		sprintf(gcQuery,"/usr/sbin/useradd -m -u %u -d %s -g %u -c \"%s %s(%s)\" -s %s %s",
			uUID,cUserDir,uGID,field[0],field[2],field[3],"/bin/bash",cLogin);
		if(system(gcQuery))
		{
			TextError(gcQuery,1);
			mysql_free_result(res);
			return(1);
		}
		if((uRetVal=ChangeSystemPasswd(cLogin,field[1])))
		{
			sprintf(gcQuery,"NewUserJob() ChangeSystemPasswd() Error:%u\n",uRetVal);
			TextError(gcQuery,1);
			mysql_free_result(res);
			return(1);
		}
		if(UserPerms(cLogin,cUserDir))
		{
			TextError("UserPerms() Returned error(s)",1);
			mysql_free_result(res);
			return(1);
		}
	}
	mysql_free_result(res);

	if(PrepSingleUserHomeDir(uUID,cLogin))
		return(1);

	//If all cServerGroup member server jobs have uJobStatus done then do this
	if(AllRelatedJobsDone(uJob))
		UpdateUserStatus(uUser,STATUS_ACTIVE);
	return(0);

}//int NewUserJob(unsigned uJob,unsigned uUser)


int UserPerms(char *cLogin, char *cUserDir)
{
	//struct stat statInfo;
	//char cQuery[512];
	int uRetVal=0;

	return(uRetVal);

}//int UserPerms(char *cLogin, char *cUserDir)


int ModUserJob(unsigned uJob, unsigned uUser)
{
	//debug only
	printf("ModUserJob(%u,%u)\n",uJob,uUser);
	//return(0);

        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uUID=0;
	struct passwd *structPasswd;
	unsigned uRetVal=0;
	char cUserRoot[256]={"/home"};
	char cUserDir[512]={""};
	char cLogin[36]={""};
	unsigned uServerGroup=0;
	
        sprintf(gcQuery,"SELECT tUser.cLogin,tUser.cPasswd,tDomain.cDomain,tUser.uDomain,tUser.uServerGroup FROM tUser,tDomain WHERE tUser.uDomain=tDomain.uDomain AND tUser.uUser=%u",uUser);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		uUID=uUser+MAIL_UID_BASE;
		if(!(structPasswd=getpwuid(uUID)))
		{
			sprintf(gcQuery,"ModUserJob() User with uUID:%u does not exist\n",uUID);
			TextError(gcQuery,1);
			return(1);
		}

		//Account format cLogin..uDomain Example: john.doe..345
		//Therefore cLogins must not have more than one consecutive . (period)
		sscanf(field[4],"%u",&uServerGroup);
		
		if(uServerGroup==POSTFIX_SERVER_GROUP)
			sprintf(cLogin,"%.25s..%.4s",field[0],field[3]);
		else
			sprintf(cLogin,"%.25s",field[0]);
		GetConfiguration("cUserRoot",cUserRoot,0,0);
		sprintf(cUserDir,"%s/%s",cUserRoot,cLogin);
		sprintf(gcQuery,"/usr/sbin/usermod -u %u -g %u -d %s -c \"%s %s %s\" -s %s %s",uUID,MAIL_GID_BASE,cUserDir,field[0],field[2],field[4],"/bin/bash",cLogin);
		if(system(gcQuery))
		{
	//		TextError(gcQuery,1);
			//debug only
			printf("cmd was:%s\n",gcQuery);
			return(1);
		}
		if((uRetVal=ChangeSystemPasswd(cLogin,field[1])))
		{
			sprintf(gcQuery,"ModUserJob() ChangeSystemPasswd() Error:%u\n",uRetVal);
			TextError(gcQuery,1);
			return(1);
		}
		if(UserPerms(cLogin,cUserDir))
		{
			TextError("UserPerms() Returned error(s)",1);
			return(1);
		}
	}
	mysql_free_result(res);
	if(AllRelatedJobsDone(uJob))
		UpdateUserStatus(uUser,STATUS_ACTIVE);
	return(0);

}//int ModUserJob(unsigned uJob, unsigned uUser)


int DelUserJob(unsigned uJob, unsigned uUser)
{
	//debug only
	printf("DelUserJob(%u,%u)\n",uJob,uUser);
	//return(0);

        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uUID=0;
	unsigned uServerGroup=0;
	unsigned uDomain=0;
	struct passwd *structPasswd;
	char cLogin[36]={""};

        sprintf(gcQuery,"SELECT cLogin,uServerGroup,uDomain FROM tUser WHERE uUser=%u",uUser);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
                sscanf(field[1],"%u",&uServerGroup);
                sscanf(field[2],"%u",&uDomain);

		if(!uServer || !uDomain)
		{
			sprintf(gcQuery,"DelUserJob() User (%u/%u/%u) uServerGroup and/or uDomain error\n",uUser,uServer,uDomain);
			TextError(gcQuery,1);
			mysql_free_result(res);
			return(1);
		}

		uUID=uUser+MAIL_UID_BASE;
		if(!(structPasswd=getpwuid(uUID)))
		{
			sprintf(gcQuery,"DelUserJob() User with uUID:%u does not exist\n",uUID);
			TextError(gcQuery,1);
			mysql_free_result(res);
			return(1);
		}
		
		if(uServerGroup==POSTFIX_SERVER_GROUP)
			sprintf(cLogin,"%.25s..%.4s",field[0],field[2]);
		else
			sprintf(cLogin,"%.25s",field[0]);

		sprintf(gcQuery,"/usr/sbin/userdel %s",cLogin);
		if(system(gcQuery))
		{
			TextError(gcQuery,1);
			mysql_free_result(res);
			return(1);
		}

		sprintf(gcQuery,"/bin/rm -Rf /home/%s",cLogin);
		if(system(gcQuery))
		{
			TextError(gcQuery,1);
			mysql_free_result(res);
			return(1);
		}
	}
	mysql_free_result(res);

        sprintf(gcQuery,"DELETE FROM tUser WHERE uUser=%u",uUser);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
	
	return(0);

}//int DelUserJob(unsigned uJob, unsigned uUser)


void UpdateJobStatus(unsigned uJob,unsigned uJobStatus)
{
	char cQuery[256];

        sprintf(cQuery,"UPDATE tJob SET uJobStatus=%u WHERE uJob=%u",
							uJobStatus,uJob);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);

}//void UpdateJobStatus(unsigned uJob,unsigned uJobStatus)


void UpdateUserStatus(unsigned uUser,unsigned uStatus)
{
	char cQuery[256];

        sprintf(cQuery,"UPDATE tUser SET uStatus=%u WHERE uUser=%u",
							uStatus,uUser);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);

}//void UpdateUserStatus(unsigned uUser,unsigned uStatus)



////Common RAD3 command line support functions
//This is a dangerous function that should be replaced with kernel style code
int ChangeSystemPasswd(char *cLogin, char *cPasswd)
{
        FILE *fp,*ofp;
        char cSearch[256]={""};
	struct passwd *userinfo;
	register unsigned uFound=0;

	if((userinfo=getpwnam(cLogin))==NULL)
		return(16);

        if(userinfo->pw_uid<100)
		return(17);

        umask(~(S_IRUSR | S_IWUSR));

        if(lckpwdf()) return(8);

        if((fp=fopen("/etc/shadow","r"))==NULL)
                return(1);

        if((ofp=fopen("/etc/shadow.apache","w"))==NULL)
                return(2);

        sprintf(cSearch,"%s:",cLogin);

        while(fgets(gcQuery,1024,fp)!=NULL)
        {
		
                if(!strncmp(gcQuery,cSearch,strlen(cSearch)))
                {
			uFound++;
                        fprintf(ofp,"%.64s:%.36s:10556::::::\n",cLogin,cPasswd);
                }
                else
                {
                        fprintf(ofp,"%s",gcQuery);
                }
        }

        fclose(ofp);
        fclose(fp);

	if((fp=fopen("/etc/shadow.apache","r"))==NULL)
                return(3);
        if((ofp=fopen("/etc/shadow","w"))==NULL)
                return(4);
        while(fgets(gcQuery,1024,fp)!=NULL)
                        fprintf(ofp,"%s",gcQuery);

        fclose(ofp);
        fclose(fp);

        ulckpwdf();

        if(uFound)
		return(0);
	else
		return(5);

}//int ChangeSystemPasswd(char *cLogin, char *cPasswd)


void UpdateSystemPasswd(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cChoice=0;

	printf("Are you sure you want to synchronize db password with /etc/shadow? This operation has NO UNDO! (Y/N): ");
	scanf("%c",&cChoice);

	cChoice=tolower(cChoice);

	if(cChoice=='n') exit(0);

	CrontabConnectDb();

	sprintf(gcQuery,"SELECT cLogin,cPasswd FROM tUser ORDER BY cLogin");
	mysql_query(&gMysql,gcQuery);

	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		ChangeSystemPasswd(field[0],field[1]);
	}

}//void UpdateSystemPasswd(void)


void GetConfiguration(const char *cName, char *cValue, unsigned uServer, unsigned uHtml)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        char cQuery[512];

        sprintf(cQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='%s' AND uServer=%u",
			cName,uServer);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
        	        htmlPlainTextError(mysql_error(&gMysql));
		else
        	        TextError(mysql_error(&gMysql),0);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
        	sprintf(cValue,"%.255s",field[0]);
        mysql_free_result(res);

}//void GetConfiguration(const char *cName, char *cValue)


unsigned GetuServer(const char *cLabel)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        unsigned uRetVal=0;

        sprintf(gcQuery,"SELECT uServer FROM tServer WHERE cLabel='%s'",cLabel);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
                sscanf(field[0],"%u",&uRetVal);
        mysql_free_result(res);

        return(uRetVal);

}//unsigned GetuServer(const char *cLabel)


void CrontabConnectDb(void)
{
	mysql_init(&gMysql);
	if (!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,0,NULL,0))
	{
		if (!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,0,NULL,0))
		{
			printf("Database server unavailable\n");
       			printf("%s\n",mysql_error(&gMysql)); 
			exit(1);
		}
	}
			
}//end of CrontabConnectDb()


int AllRelatedJobsDone(unsigned uJob)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        unsigned uRetVal=0;
        unsigned uCount=0;
	char cJobName[100]={""};
	unsigned uJobStatus=0;
	unsigned uJobTarget=0;
	unsigned uJobTargetUser=0;
	time_t luJobDate=0;

        sprintf(gcQuery,"SELECT cJobName,uJobTarget,uJobTargetUser,uJobDate FROM tJob WHERE uJob=%u",uJob);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sprintf(cJobName,"%.99s",field[0]);
                sscanf(field[1],"%u",&uJobTarget);
                sscanf(field[2],"%u",&uJobTargetUser);
                sscanf(field[3],"%lu",&luJobDate);
	}
        mysql_free_result(res);

	//The 2 sec time window may cause problems on heavily loaded mysql servers...fix asap
	//Use more complex tServerGroup based outer loop?
        sprintf(gcQuery,"SELECT uJobStatus FROM tJob WHERE cJobName='%s' AND uJobTarget=%u AND uJobTargetUser=%u AND (uJobDate>%lu-1 AND uJobDate<%lu+1)",cJobName,uJobTarget,uJobTargetUser,luJobDate,luJobDate);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{
		uCount++;
		uJobStatus=0;
		sscanf(field[0],"%u",&uJobStatus);
		if(uJobStatus==JOBSTATUS_DONE)
			uRetVal++;
	}
        mysql_free_result(res);

	if(uCount==uRetVal)
        	return(1);
	else
        	return(0);

}//int AllRelatedJobsDone(unsigned uJob)


int NewServerConfigJob(unsigned uJob)
{
	struct passwd *structUserInfo;
	struct group *structGroupInfo;
	unsigned uServerConfig=0;
	unsigned uConfigSpec=0;
	char *cp;

        MYSQL_RES *res;
        MYSQL_ROW field;

	char cPath[512]={""};
	char cOwner[100]={""};
	char cGroup[100]={""};
	char cPerms[32]={""};
	char cNewExec[100]={""};
	unsigned uMode=0;

	FILE *fp;

	printf("NewServerConfigJob(%u)\n",uJob);

        sprintf(gcQuery,"SELECT cJobData FROM tJob WHERE uJob=%u",uJob);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		if((cp=strstr(field[0],"uServerConfig=")))
			sscanf(cp+14,"%u;",&uServerConfig);
		if((cp=strstr(field[0],"uConfigSpec=")))
			sscanf(cp+12,"%u;",&uConfigSpec);
	}
	mysql_free_result(res);

	if(!uServerConfig || !uConfigSpec)
	{
		TextError("NewServerConfigJob() Unexpected cJobData format",1);
		return(1);
	}


        sprintf(gcQuery,"SELECT cPath,cOwner,cGroup,cPerms,cNewExec FROM tConfigSpec WHERE uConfigSpec=%u",uConfigSpec);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sprintf(cPath,"%.511s",field[0]);
		sprintf(cOwner,"%.99s",field[1]);
		sprintf(cGroup,"%.99s",field[2]);
		sprintf(cPerms,"%.31s",field[3]);
		sprintf(cNewExec,"%.99s",field[4]);
	}
	else
	{
		mysql_free_result(res);
		TextError("NewServerConfigJob() no tConfigSpec data",1);
		return(1);
	}
	mysql_free_result(res);

	if(!cPath[0] || !cOwner[0] || !cGroup[0] || !cPerms[0])
	{
		TextError("NewServerConfigJob() empty tConfigSpec data",1);
		return(1);
	}

	if((fp=fopen(cPath,"w"))==NULL)
	{
		TextError("NewServerConfigJob() could not open cPath for write",1);
		printf("cPath=%s\n",cPath);
		return(1);
	}

	//Write file and set ownership and perms
        sprintf(gcQuery,"SELECT cConfig FROM tServerConfig WHERE uServerConfig=%u",uServerConfig);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"%s",field[0]);
		fclose(fp);
	}
	else
	{
		fclose(fp);
		mysql_free_result(res);
		TextError("NewServerConfigJob() no Config data",1);
		return(1);
	}
	mysql_free_result(res);


	if((structUserInfo=getpwnam(cOwner))==NULL)
	{
		TextError("NewServerConfigJob() no such cOwner",1);
		printf("cOwner=%s\n",cOwner);
		return(1);
	}

	if((structGroupInfo=getgrnam(cGroup))==NULL)
	{
		TextError("NewServerConfigJob() no such cGroup",1);
		printf("cGroup=%s\n",cGroup);
		return(1);
	}

	if(chown(cPath,structUserInfo->pw_uid,structGroupInfo->gr_gid))
	{
		TextError("NewServerConfigJob() chown() error",1);
		return(1);
	}

	sscanf(cPerms,"%o",&uMode);
	if(chmod(cPath,uMode))
	{
		TextError("NewServerConfigJob() chmod() error",1);
		return(1);
	}

	//Attack vector
	//All tConfigSpec Exec field data should be audited continously to not
	//contain malicious commands. These fields should be then set readonly.
	//Changing effective UID may help.
	if(cNewExec[0])
	{
		if(system(cNewExec))
		{
			TextError("NewServerConfigJob() system(cNewExec) error",1);
			return(1);
		}
	}

	return(0);

}//int NewServerConfigJob(unsigned uJob)


int DelServerConfigJob(unsigned uJob)
{
	unsigned uServerConfig=0;
	unsigned uConfigSpec=0;
	char *cp;

        MYSQL_RES *res;
        MYSQL_ROW field;

	char cPath[512]={""};
	char cDelExec[100]={""};

	printf("DelServerConfigJob(%u)\n",uJob);

        sprintf(gcQuery,"SELECT cJobData FROM tJob WHERE uJob=%u",uJob);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		if((cp=strstr(field[0],"uServerConfig=")))
			sscanf(cp+14,"%u;",&uServerConfig);
		if((cp=strstr(field[0],"uConfigSpec=")))
			sscanf(cp+12,"%u;",&uConfigSpec);
	}
	mysql_free_result(res);

	if(!uServerConfig || !uConfigSpec)
	{
		TextError("DelServerConfigJob() Unexpected cJobData format",1);
		return(1);
	}


        sprintf(gcQuery,"SELECT cPath,cDelExec FROM tConfigSpec WHERE uConfigSpec=%u",
			uConfigSpec);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sprintf(cPath,"%.511s",field[0]);
		sprintf(cDelExec,"%.99s",field[1]);
	}
	else
	{
		mysql_free_result(res);
		TextError("DelServerConfigJob() no tConfigSpec data",1);
		return(1);
	}
	mysql_free_result(res);

	if(!cPath[0])
	{
		TextError("DelServerConfigJob() empty tConfigSpec.cPath",1);
		return(1);
	}

	if(unlink(cPath))
	{
		TextError("DelServerConfigJob() could not remove cPath",1);
		printf("cPath=%s\n",cPath);
		return(1);
	}

	//Attack vector
	//All tConfigSpec Exec field data should be audited continously to not
	//contain malicious commands. These fields should be then set readonly.
	//Changing effective UID may help.
	if(cDelExec[0])
	{
		if(system(cDelExec))
		{
			TextError("DelServerConfigJob() system(cDelExec) error",1);
			return(1);
		}
	}

	return(0);

}//int DelServerConfigJob(unsigned uJob)


int ModServerConfigJob(unsigned uJob)
{
	struct passwd *structUserInfo;
	struct group *structGroupInfo;
	unsigned uServerConfig=0;
	unsigned uConfigSpec=0;
	char *cp;

        MYSQL_RES *res;
        MYSQL_ROW field;

	char cPath[512]={""};
	char cOwner[100]={""};
	char cGroup[100]={""};
	char cPerms[32]={""};
	char cModExec[100]={""};
	unsigned uMode=0;

	FILE *fp;

	printf("ModServerConfigJob(%u)\n",uJob);

        sprintf(gcQuery,"SELECT cJobData FROM tJob WHERE uJob=%u",uJob);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		if((cp=strstr(field[0],"uServerConfig=")))
			sscanf(cp+14,"%u;",&uServerConfig);
		if((cp=strstr(field[0],"uConfigSpec=")))
			sscanf(cp+12,"%u;",&uConfigSpec);
	}
	mysql_free_result(res);

	if(!uServerConfig || !uConfigSpec)
	{
		TextError("ModServerConfigJob() Unexpected cJobData format",1);
		return(1);
	}


        sprintf(gcQuery,"SELECT cPath,cOwner,cGroup,cPerms,cModExec FROM tConfigSpec WHERE uConfigSpec=%u",uConfigSpec);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sprintf(cPath,"%.511s",field[0]);
		sprintf(cOwner,"%.99s",field[1]);
		sprintf(cGroup,"%.99s",field[2]);
		sprintf(cPerms,"%.31s",field[3]);
		sprintf(cModExec,"%.99s",field[4]);
	}
	else
	{
		mysql_free_result(res);
		TextError("ModServerConfigJob() no tConfigSpec data",1);
		return(1);
	}
	mysql_free_result(res);

	if(!cPath[0] || !cOwner[0] || !cGroup[0] || !cPerms[0])
	{
		TextError("ModServerConfigJob() empty tConfigSpec data",1);
		return(1);
	}

	if((fp=fopen(cPath,"w"))==NULL)
	{
		TextError("ModServerConfigJob() could not open cPath for write",1);
		printf("cPath=%s\n",cPath);
		return(1);
	}

	//Write file and set ownership and perms
        sprintf(gcQuery,"SELECT cConfig FROM tServerConfig WHERE uServerConfig=%u",uServerConfig);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"%s",field[0]);
		fclose(fp);
	}
	else
	{
		fclose(fp);
		mysql_free_result(res);
		TextError("ModServerConfigJob() no Config data",1);
		return(1);
	}
	mysql_free_result(res);


	if((structUserInfo=getpwnam(cOwner))==NULL)
	{
		TextError("ModServerConfigJob() no such cOwner",1);
		printf("cOwner=%s\n",cOwner);
		return(1);
	}

	if((structGroupInfo=getgrnam(cGroup))==NULL)
	{
		TextError("ModServerConfigJob() no such cGroup",1);
		printf("cGroup=%s\n",cGroup);
		return(1);
	}

	if(chown(cPath,structUserInfo->pw_uid,structGroupInfo->gr_gid))
	{
		TextError("ModServerConfigJob() chown() error",1);
		return(1);
	}

	sscanf(cPerms,"%o",&uMode);
	if(chmod(cPath,uMode))
	{
		TextError("ModServerConfigJob() chmod() error",1);
		return(1);
	}

	//Attack vector
	//All tConfigSpec Exec field data should be audited continously to not
	//contain malicious commands. These fields should be then set readonly.
	//Changing effective UID may help.
	if(cModExec[0])
	{
		if(system(cModExec))
		{
			TextError("ModServerConfigJob() system(cModExec) error",1);
			return(1);
		}
	}

	return(0);

}//int ModServerConfigJob(unsigned uJob)


int NewVUTConfigJob(unsigned uJob)
{
	struct passwd *structUserInfo;
	struct group *structGroupInfo;

        MYSQL_RES *res;
        MYSQL_ROW field;

	char cPath[512]={""};
	char cOwner[100]={""};
	char cGroup[100]={""};
	char cPerms[32]={""};
	char cNewExec[100]={""};
	unsigned uMode=0;

	FILE *fp;

	printf("NewVUTConfigJob(%u)\n",uJob);

        sprintf(gcQuery,"SELECT cPath,cOwner,cGroup,cPerms,cNewExec FROM tConfigSpec WHERE cLabel='NewVUTConfigJob'");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sprintf(cPath,"%.511s",field[0]);
		sprintf(cOwner,"%.99s",field[1]);
		sprintf(cGroup,"%.99s",field[2]);
		sprintf(cPerms,"%.31s",field[3]);
		sprintf(cNewExec,"%.99s",field[4]);
	}
	else
	{
		mysql_free_result(res);
		TextError("NewVUTConfigJob() no tConfigSpec data",1);
		return(1);
	}
	mysql_free_result(res);

	if(!cPath[0] || !cOwner[0] || !cGroup[0] || !cPerms[0])
	{
		TextError("NewVUTConfigJob() empty tConfigSpec data",1);
		return(1);
	}

	if((fp=fopen(cPath,"w"))==NULL)
	{
		TextError("NewVUTConfigJob() could not open cPath for write",1);
		printf("cPath=%s\n",cPath);
		return(1);
	}
	fprintf(fp,"#unxsMail::NewVUTConfigJob() svn ID removed

	//Write file and set ownership and perms
        sprintf(gcQuery,"SELECT cVirtualEmail,cDomain,cTargetEmail FROM tVUT,tVUTEntries,tServerGroupGlue WHERE tVUT.uVUT=tVUTEntries.uVUT AND tVUT.uServerGroup=tServerGroupGlue.uServerGroup AND tServerGroupGlue.uServer=%u ORDER BY cDomain,cVirtualEmail",uServer);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
        	while((field=mysql_fetch_row(res)))
		{
			//Dependency on correct data entry in tvutentriesfunc.h
			if(strcmp(field[0],"DEFAULT"))
				fprintf(fp,"%s@%s %s\n",field[0],field[1],field[2]);
			else
				fprintf(fp,"@%s %s\n",field[1],field[2]);
		}
	}
	else
	{
		mysql_free_result(res);
		TextError("NewVUTConfigJob() no row data",1);
		//Continue with empty VUT table
	}
	fclose(fp);
	mysql_free_result(res);


	if((structUserInfo=getpwnam(cOwner))==NULL)
	{
		TextError("NewVUTConfigJob() no such cOwner",1);
		printf("cOwner=%s\n",cOwner);
		return(1);
	}

	if((structGroupInfo=getgrnam(cGroup))==NULL)
	{
		TextError("NewVUTConfigJob() no such cGroup",1);
		printf("cGroup=%s\n",cGroup);
		return(1);
	}

	if(chown(cPath,structUserInfo->pw_uid,structGroupInfo->gr_gid))
	{
		TextError("NewVUTConfigJob() chown() error",1);
		return(1);
	}

	sscanf(cPerms,"%o",&uMode);
	if(chmod(cPath,uMode))
	{
		TextError("NewVUTConfigJob() chmod() error",1);
		return(1);
	}

	//Attack vector
	//All tConfigSpec Exec field data should be audited continously to not
	//contain malicious commands. These fields should be then set readonly.
	//Changing effective UID may help.
	if(cNewExec[0])
	{
		if(system(cNewExec))
		{
			TextError("NewVUTConfigJob() system(cNewExec) error",1);
			return(1);
		}
	}

	return(0);

}//int NewVUTConfigJob(unsigned uJob)


int NewAccessConfigJob(unsigned uJob)
{
	//Redhat 7.3 default with our other defaults
	char cMakeAccess[256]={"/usr/bin/makemap hash /etc/mail/access.db < /etc/mail/access"};
	unsigned uErrNum;

	GetConfiguration("cMakeAccess",cMakeAccess,0,0);
	printf("MakeAccessJob() %s\n",cMakeAccess);
	//Make file
	if(MakeAccessFile(uServer))
		return(1);
	
	if((uErrNum=system(cMakeAccess)))
		return(1);
	
	return(0);

}//int NewAccessConfigJob(unsigned uJob);


int MakeAccessFile(unsigned uServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	FILE *fp;
	char cFile[256]={"/etc/mail/access"};
	unsigned uServerGroup=0;

	uServerGroup=uGetServerGroup(uServer);

	GetConfiguration("cAccessFile",cFile,0,0);
	
	if(!(fp=fopen(cFile,"w")))
        {
		char cMsg[256];
		sprintf(cMsg,"Can't open %.230s for write",cFile);
                printf("%s\n",cMsg);
               	return(1);
        }

	fprintf(fp,"#OpenISP unxsMail.MakeAccessFile() svn ID removed

	
	sprintf(gcQuery,"SELECT DISTINCT cDomainIP,cRelayAttr FROM tAccess WHERE uServerGroup=%u",uServerGroup);
		
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
		printf("%s\n",mysql_error(&gMysql));
		return(1);
	}

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
                fprintf(fp,"%s %s\n",field[0],field[1]);
        mysql_free_result(res);
	fclose(fp);

	return(0);

}//int MakeAccessFile()


int NewLocalConfigJob(unsigned uJob)
{
	//Redhat 7.3 default
	char cMakeLocal[256]={"/etc/rc.d/init.d/sendmail restart"};
	unsigned uErrNum;

	GetConfiguration("cMakeLocal",cMakeLocal,0,0);
	printf("MakeLocalJob() %s\n",cMakeLocal);
	//Make file
	if(MakeLocalFile(uServer))
		return(1);
	
	if((uErrNum=system(cMakeLocal)))
		return(1);
	
	return(0);

}//int NewLocalConfigJob(unsigned uJob);


int NewRelayConfigJob(unsigned uJob)
{
	char cMakeRelay[256]={"/etc/postfix/reload.sh"};
	unsigned uErrNum;

	GetConfiguration("cMakeRelay",cMakeRelay,0,0);
	printf("MakeRelayJob() %s\n",cMakeRelay);
	//Make file
	if(MakeRelayFile(uServer))
		return(1);
	
	if((uErrNum=system(cMakeRelay)))
		return(1);
	
	return(0);

}//int NewRelayConfigJob(unsigned uJob)


int MakeRelayFile(unsigned uServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	FILE *fp1,*fp2,*fp3;
	char cRelayDomainsFile[256]={"/etc/postfix/relay_domains"};
	char cRelayRecipientsFile[256]={"/etc/postfix/relay_recipients"};
	char cTransportFile[256]={"/etc/postfix/transport"};
	unsigned uServerGroup=0;
	
printf("NewRelayConfigJob()\n");
	GetConfiguration("cRelayDomainsFile",cRelayDomainsFile,0,0);
	GetConfiguration("cRelayRecipientsFile",cRelayRecipientsFile,0,0);
	GetConfiguration("cTransportFile",cTransportFile,0,0);
	uServerGroup=uGetServerGroup(uServer);

	if(!(fp1=fopen(cRelayDomainsFile,"w")))
        {
		char cMsg[256];
		sprintf(cMsg,"Can't open %.230s for write",cRelayDomainsFile);
                printf("%s\n",cMsg);
               	return(1);
        }
	if(!(fp2=fopen(cRelayRecipientsFile,"w")))
        {
		char cMsg[256];
		sprintf(cMsg,"Can't open %.230s for write",cRelayRecipientsFile);
                printf("%s\n",cMsg);
		fclose(fp1);
               	return(2);
        }
	if(!(fp3=fopen(cTransportFile,"w")))
        {
		char cMsg[256];
		sprintf(cMsg,"Can't open %.230s for write",cTransportFile);
                printf("%s\n",cMsg);
		fclose(fp1);
		fclose(fp2);
               	return(3);
        }


	//relay_domains file
	sprintf(gcQuery,"SELECT cDomain FROM tRelay WHERE uServerGroup=%u",uServerGroup);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s\n",mysql_error(&gMysql));
               	return(4);
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
                fprintf(fp1,"%s\t\t1\n",field[0]);
        mysql_free_result(res);
	fclose(fp1);

	//relay_recipients file
	sprintf(gcQuery,"SELECT cDomain FROM tRelay WHERE uServerGroup=%u",uServerGroup);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s\n",mysql_error(&gMysql));
               	return(5);
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
                fprintf(fp2,"@%s\t\t1\n",field[0]);
        mysql_free_result(res);
	fclose(fp2);

	//transport file
	sprintf(gcQuery,"SELECT cDomain,cTransport FROM tRelay WHERE uServerGroup=%u",uServerGroup);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s\n",mysql_error(&gMysql));
               	return(6);
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
                fprintf(fp3,"%s\t\tsmtp:%s\n",field[0],field[1]);
        mysql_free_result(res);
	fclose(fp3);

	return(0);

}//int MakeRelayFile(unsigned uServer)


int MakeLocalFile(unsigned uServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	FILE *fp;
	char cFile[256]={"/etc/mail/sendmail.cw"};
	unsigned uServerGroup=0;

	uServerGroup=uGetServerGroup(uServer);

	GetConfiguration("cLocalFile",cFile,0,0);
	
	if(!(fp=fopen(cFile,"w")))
        {
		char cMsg[256];
		sprintf(cMsg,"Can't open %.230s for write",cFile);
                printf("%s\n",cMsg);
               	return(1);
        }

	fprintf(fp,"#OpenISP unxsMail.MakeLocalFile() svn ID removed

	sprintf(gcQuery,"SELECT cDomain FROM tLocal WHERE uServerGroup=%u",uServerGroup);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s\n",mysql_error(&gMysql));
               	return(1);
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
                fprintf(fp,"%s\n",field[0]);
        mysql_free_result(res);
	fclose(fp);

	return(0);

}//int MakeLocalFile()


unsigned uGetServerGroup(unsigned uServer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uServerGroup=0;
	
	sprintf(gcQuery,"SELECT uServerGroup FROM tServerGroupGlue WHERE uServer='%u'",uServer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		return(0);
	}

	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uServerGroup);

	return(uServerGroup);

}//unsigned uGetServerGroup(unsigned uServer)

//Removes both uSource=2 modified qualcomm qpopper and our mysqlRadacct Cistron 1.6.6 radius
//This last is uSource=3
//NukeSpam is 4,5,6. Only 4 used for now 4=signature 1: misses.
//tDUL uSource=7
//So we use the eExpireDate diff from 0 as the rule for flexibility.
void CleanLocalAccess(unsigned uServer)
{
	time_t luClock;
	unsigned uServerGroup=0;
	char cServerGroup[100]={""};

	time(&luClock);
	
	uServerGroup=uServer;
	
	ConnectDb();

	sprintf(gcQuery,"DELETE FROM tAccess WHERE uServerGroup='%u' AND uExpireDate!=0 AND uExpireDate<=%lu",
			uServerGroup,
			(unsigned long)luClock);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
			return;
        }
	if(mysql_affected_rows(&gMysql))
	{
		sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
		SubmitJob("DelAccess","","",cServerGroup,"CleanLocalAccess()",0,0,1,1);
	}

}//void CleanLocalAccess(unsigned uServer)


void FixUID(unsigned uServer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uUID=0;
	unsigned uServerGroup=0;

	uServerGroup=uGetServerGroup(uServer);

	sprintf(gcQuery,"SELECT uUser,cLogin FROM tUser WHERE uServerGroup=%u ORDER BY cLogin",uServerGroup);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uUID);
		uUID+=MAIL_UID_BASE;

		sprintf(gcQuery,"/usr/sbin/usermod -u %u %s",uUID,field[1]);
		if(system(gcQuery))
		{
			printf("Updating %s UID failed!\n",field[1]);
			continue;
		}
	}

}//void FixUID(void)


void FixMboxPerms(unsigned uServer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uUID=0;
	unsigned uServerGroup=0;

	uServerGroup=uGetServerGroup(uServer);

	sprintf(gcQuery,"SELECT uUser,cLogin FROM tUser WHERE uServerGroup=%u ORDER BY cLogin",uServerGroup);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uUID);
		uUID+=MAIL_UID_BASE;

		sprintf(gcQuery,"/bin/chown %u /var/mail/%s",uUID,field[1]);
		if(system(gcQuery))
		{
			printf("Updating /var/mail/%s ownership failed!\n",field[1]);
			continue;
		}
	}

}//void FixMboxPerms(void)


int MakeAllUsers(unsigned uServer)
{
	//debug only
	printf("MakeAllUsers(%u)\n",uServer);
	//return(0);

        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uGID=0;
	unsigned uUID=0;
	struct passwd *structPasswd;
	struct group *structGroup;
	unsigned uRetVal;
	char cUserRoot[256]={"/home"};
	char cUserDir[512]={""};
	char cLogin[36]={""};
	unsigned uServerGroup=0;
	unsigned uUser=0;

	uServerGroup=uGetServerGroup(uServer);

        sprintf(gcQuery,"SELECT tUser.cLogin,tUser.cPasswd,tDomain.cDomain,tUser.uDomain,tUser.uUser FROM tUser,tDomain WHERE tUser.uDomain=tDomain.uDomain"
			" AND tUser.uServerGroup=%u",uServerGroup);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{
		sscanf(field[4],"%u",&uUser);
		printf("login=%s\n",field[0]);
		if(!(structGroup=getgrgid(MAIL_GID_BASE)))
		{
			sprintf(gcQuery,"NewUserJob() No uGID:%u for uUser:%u\n",
					MAIL_GID_BASE,uUser);
	//		TextError(gcQuery,1);
			continue;
		}
			
		uUID=uUser+MAIL_UID_BASE;
		if((structPasswd=getpwuid(uUID)))
		{
			sprintf(gcQuery,"NewUserJob() User with uUID:%u already exists\n",uUID);
	//		TextError(gcQuery,1);
			continue;
		}

		//implicit limit of 9999 domains per cluster
		//this is due to the linux login name length limit
		//and our balance in favor of relat. long login names
		
		//User names with xxx..1 only for postfix servers
		if(uGetServerGroup(uServer)==POSTFIX_SERVER_GROUP)		
			sprintf(cLogin,"%.25s..%.4s",field[0],field[3]);
		else
			sprintf(cLogin,"%.25s",field[0]);
		GetConfiguration("cUserRoot",cUserRoot,0,0);
		sprintf(cUserDir,"%s/%s",cUserRoot,cLogin);
		uGID=MAIL_GID_BASE;
		sprintf(gcQuery,"/usr/sbin/useradd -m -u %u -d %s -g %u -c \"%s %s(%s)\" -s %s %s",
			uUID,cUserDir,uGID,field[0],field[2],field[3],"/bin/bash",cLogin);
		if(system(gcQuery))
		{
//			TextError(gcQuery,1);
			continue;
		}
		if((uRetVal=ChangeSystemPasswd(cLogin,field[1])))
		{
			sprintf(gcQuery,"NewUserJob() ChangeSystemPasswd() Error:%u\n",uRetVal);
//			TextError(gcQuery,1);
			continue;
		}
		if(UserPerms(cLogin,cUserDir))
		{
			TextError("UserPerms() Returned error(s)",1);
			continue;
		}
	}
	mysql_free_result(res);

	//If all cServerGroup member server jobs have uJobStatus done then do this
	return(0);

}//int NewUserJob(unsigned uJob,unsigned uUser)


void PrepHomeDirs(unsigned uServer)
{
	//
	//This function will create the required directory files for supporting the Spam folder
	//at Squirrelmail via symlink to /var/mail/{$cLogin}.spam mailbox.
	//

	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uServerGroup=0;
	unsigned uUID=0;
	unsigned uUser=0;
	unsigned uCount=0;

	uServerGroup=uGetServerGroup(uServer);

	printf("PrepHomeDirs() start\n");

	sprintf(gcQuery,"SELECT uUser,cLogin FROM tUser WHERE uServerGroup=%u AND uStatus=4 ORDER BY cLogin",uServerGroup);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
	
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uUser);
		uUID=uUser+MAIL_UID_BASE;
		PrepSingleUserHomeDir(uUID,field[1]);
		uCount++;
	}
	mysql_free_result(res);

	printf("PrepHomeDirs() end. %u accounts processed.\n",uCount);
	exit(0);

}//void PrepHomeDirs(unsigned uServer)


unsigned PrepSingleUserHomeDir(unsigned uUID,char const *cLogin)
{	
	FILE *fp;
	char cPath[256];
	char cMailPath[256];
	struct stat statFileInfo;

	sprintf(cPath,"/home/%.32s/Spam",cLogin);
	if(!lstat(cPath,&statFileInfo))
	{
		//debug only
		printf("%s already exists!\n",cPath);
		return(0);
	}

	//debug only
	//printf("%s %u\n",cLogin,uUID);
	//return;

	sprintf(cPath,"/home/%.32s/.mailboxlist",cLogin);
	if(!stat(cPath,&statFileInfo))
	{
		if((fp=fopen(cPath,"a"))!=NULL)
		{
			fprintf(fp,"Spam\n");
			fclose(fp);
		}
		else
		{
			TextError(cPath,1);
			return(1);
		}
	}
	else
	{
		if((fp=fopen(cPath,"w"))!=NULL)
		{
			fprintf(fp,"Sent\nTrash\nDrafts\nSpam\n");
			fclose(fp);
		}
		else
		{
			TextError(cPath,1);
			return(1);
		}
	}
		
	if((chown(cPath,uUID,MAIL_GID_BASE)))
	{
		TextError(cMailPath,0);
		return(1);
	}

	sprintf(cPath,"/home/%.32s/Spam",cLogin);
	sprintf(cMailPath,"/var/mail/%.32s.spam",cLogin);
	if(stat(cPath,&statFileInfo))
	{
		if(symlink(cMailPath,cPath))
		{
			TextError(cMailPath,0);
			return(1);
		}
	}

	if(stat(cMailPath,&statFileInfo))
	{
		if((fp=fopen(cMailPath,"w"))!=NULL)
		{
			char *cDefaultData="From MAILER-DAEMON Thu Jan 01 00:00:00 GMT 2009\n"
			"Date: 01 Jan 2009 0:00:00 UTC\n"
			"From: Mail System Internal Data <MAILER-DAEMON@localhost>\n"
			"Subject: DON'T DELETE THIS MESSAGE -- FOLDER INTERNAL DATA\n"
			"Message-ID: <1236796779@localhost>\n"
			"X-IMAP: 1236697492 0000000010\n"
			"Status: RO\n\n"
			"This text is part of the internal format of your mail folder, and is not\n"
			"a real message.  It is created automatically by the mail system software.\n"
			"If deleted, important folder data will be lost, and it will be re-created\n"
			"with the data reset to initial values.\n";
			fprintf(fp,cDefaultData);	
			fclose(fp);
		}
		else
		{
			TextError(cMailPath,0);
			return(1);
		}
	}

	if((chown(cMailPath,uUID,MAIL_GID_BASE)))
	{
		TextError(cMailPath,0);
		return(1);
	}

	return(0);

}//unsigned PrepSingleUserHomeDir(unsigned uUID,char const *cLogin)


int MakeAliasConfigJob(unsigned uJob)
{
	char cNewAliases[256]={"/usr/bin/newaliases"};
	unsigned uErrNum;

	GetConfiguration("cNewAliases",cNewAliases,0,0);
	printf("MakeAliasConfigJob() %s\n",cNewAliases);

	if(NewAliasFile(uServer))
		return(1);

	if((uErrNum=system(cNewAliases)))
		return(1);
	
	return(0);
}//int MakeAliasConfigJob(unsigned uJob)


int NewAliasFile(unsigned uServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	FILE *fp;
	char cFile[256]={"/etc/mail/aliases"};
	unsigned uServerGroup=0 ;
	
	uServerGroup=uGetServerGroup(uServer);
	
	printf("uServer=%u uServerGroup=%u\n",uServer,uServerGroup);

	GetConfiguration("cAliasFile",cFile,uServerGroup,0);
	
	if(!(fp=fopen(cFile,"w")))
        {
		char cMsg[256];
		sprintf(cMsg,"Can't open %.230s for write",cFile);
		TextError(cMsg,1);
		return(1);
        }

	fprintf(fp,"#OpenISP unxsMail.MakeAliasFile() svn ID removed

	sprintf(gcQuery,"SELECT cUser,cTargetEmail FROM tAlias WHERE uServerGroup=%u",uServerGroup);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		TextError(mysql_error(&gMysql),1);
               	return(1);
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
                fprintf(fp,"%s: %s\n",field[0],field[1]);
        mysql_free_result(res);
	fclose(fp);

	return(0);

}//int NewAliasFile(unsigned uServer)


unsigned MakeWhiteListFile(unsigned uServer)
{
	FILE *fp;
	unsigned uServerGroup=0;
	char cFile[100]={"/etc/mail/spamassassin/whitelist.cf"};
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	uServerGroup=uGetServerGroup(uServer);

	GetConfiguration("cWhitelistFile",cFile,uServerGroup,0);

	if(!(fp=fopen(cFile,"w")))
	{
		char cMsg[256];
		sprintf(cMsg,"Can't open %.230s for write",cFile);
		TextError(cMsg,1);
		return(1);
	}

	fprintf(fp,"#OpenISP unxsMail.MakeWhiteListFile() svn ID removed

	sprintf(gcQuery,"SELECT cLabel FROM tWhiteList WHERE uServerGroup=%u AND uUser=0",uServerGroup);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		TextError(mysql_error(&gMysql),1);
		return(1);
	}

	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"whitelist_from %s\n",field[0]);
	mysql_free_result(res);
	fclose(fp);

	return(0);
	
}//unsigned MakeWhiteListFile(unsigned uServerGroup)


unsigned MakeBlackListFile(unsigned uServer)
{
	FILE *fp;
	unsigned uServerGroup=0;
	char cFile[100]={"/etc/mail/spamassassin/blacklist.cf"};
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	uServerGroup=uGetServerGroup(uServer);

	GetConfiguration("cWhitelistFile",cFile,uServerGroup,0);

	if(!(fp=fopen(cFile,"w")))
	{
		char cMsg[256];
		sprintf(cMsg,"Can't open %.230s for write",cFile);
		TextError(cMsg,1);
		return(1);
	}

	fprintf(fp,"#OpenISP unxsMail.MakeBlackListFile() svn ID removed

	sprintf(gcQuery,"SELECT cLabel FROM tBlackList WHERE uServerGroup=%u AND uUser=0",uServerGroup);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		TextError(mysql_error(&gMysql),1);
		return(1);
	}

	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"blacklist_from %s\n",field[0]);
	mysql_free_result(res);
	fclose(fp);

	return(0);
	
}//unsigned MakeBlackListFile(unsigned uServerGroup)


unsigned MakeUserPrefs(unsigned uUser)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLogin FROM tUser WHERE uUser=%u",uUser);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		TextError(mysql_error(&gMysql),1);
		return(1);
	}

	res=mysql_store_result(&gMysql);
	
	if((field=mysql_fetch_row(res)))
	{
		FILE *fp;
		char cFile[256]={""};
		
		//Make sure that we have all the directories in place bewfore fopen()
		sprintf(gcQuery,"/home/%s/.spamassassin",field[0]);
		mkdir(gcQuery,0x664);

		sprintf(cFile,"/home/%s/.spamassassin/user_prefs",field[0]);

		if((fp=fopen(cFile,"w"))==NULL)
		{
			TextError("fopen() failed",1);
			return(1);
		}
		
		mysql_free_result(res);
		
		sprintf(gcQuery,"SELECT tWhiteList.cLabel FROM tWhiteList WHERE "
				"tWhiteList.uUser=%u",uUser);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		
		while((field=mysql_fetch_row(res)))
			fprintf(fp,"whitelist_from %s\n",field[0]);
		mysql_free_result(res);
		
		sprintf(gcQuery,"SELECT tBlackList.cLabel FROM tBlackList WHERE "
				"tBlackList.uUser=%u",uUser);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		
		while((field=mysql_fetch_row(res)))
			fprintf(fp,"blacklist_from %s\n",field[0]);
		mysql_free_result(res);
	
		fclose(fp);	

		return(0);
	}		
	
	fprintf(stderr,"MakeUserPrefs(): no such user!");
	return(1);
	
}//unsigned MakeUserPrefs(unsigned uUser)


int MakeWhiteListConfigJob(unsigned uJob)
{
	if(MakeWhiteListFile(uServer))
		return(1);
	
	if(system("/etc/init.d/spamassassin restart"))
		return(1);
		
	return(0);
	
}//unsigned MakeWhiteListConfigJob(unsigned uJob)


int MakeBlackListConfigJob(unsigned uJob)
{
	if(MakeBlackListFile(uServer))
		return(1);
	
	if(system("/etc/init.d/spamassassin restart"))
		return(1);

	return(0);
	
}//unsigned MakeBlackListConfigJob(unsigned uJob)


int UpdateVacationStatus(unsigned uUser)
{
	//
	//This function will either create or delete the required files
	//for setting up mail autoreply
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT (SELECT tUser.cLogin FROM tUser WHERE tUser.uUser=tVacation.uUser),"
			"SELECT (SELECT tUser.uDomain FROM tUser WHERE tUser.uUser=tVacation.uUser),"
			"uHTML,cText FROM tVacation WHERE uUser=%u",uUser);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		TextError(mysql_error(&gMysql),1);
		return(1);
	}

	res=mysql_store_result(&gMysql);
	
	if((field=mysql_fetch_row(res)))
	{
		//Add w/tVacation data
		FILE *fp;

		if((fp=fopen("/home/%s/.vacation.msg","w"))==NULL)
		{
			TextError("fopen() failed",1);
			return(1);
		}
		fprintf(fp,"From: %s@%s\n",field[0],ForeignKey("tDomain","cDomain",strtoul(field[1],NULL,10)));
		fprintf(fp,"Subject: Re: $SUBJECT\n");
		if(!strcmp(field[2],"1"))
			fprintf(fp,"Content-Type: text/html; charset=windows-1252\n");
		else
			fprintf(fp,"Content-Type: text/plain; charset=windows-1252\n");
		fprintf(fp,"Content-Transfer-Encoding: quoted-printable\n");
		fprintf(fp,"%s\n",field[3]);

		fclose(fp);
		if((fp=fopen("/home/%s/.forward","w"))==NULL)
		{
			TextError("fopen() failed",1);
			return(1);
		}
		fprintf(fp,"\\%1$s, \"|/usr/bin/vacation %1$s",field[0]);
		fclose(fp);
	}
	else
	{
		//Remove, no tVacation data
		unlink("/home/%s/.vacation.msg");
		unlink("/home/%s/.forward");

	}
	return(0);
	
}//int UpdateVacationStatus(unsigned uUser)


int UpdateUserConfig(unsigned uUser,unsigned uJob,unsigned uNew)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cJobData[100]={""};
	unsigned uUserConfig=0;

	sprintf(gcQuery,"SELECT cJobData FROM tJob WHERE uJob=%u",uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		TextError(mysql_error(&gMysql),1);
		return(1);
	}
	res=mysql_store_result(&gMysql);
	
	if((field=mysql_fetch_row(res)))
	{
		char *cp,*cp2;
		sprintf(cJobData,"%.99s",field[0]);
		if((cp=strstr(cJobData,"uUserConfig=")))
		{
			cp2=NULL;
			if((cp2=strchr(cp+12,';'))) *cp2=0;
			sscanf(cp+12,"%u",&uUserConfig);
			if(cp2) *cp2=';';
		}
	}

	//printf("uUserConfig=%u\n",uUserConfig);
	sprintf(gcQuery,"SELECT cConfig,cPath,tConfigSpec.cLabel,cNewExec,cModExec "
			"FROM tUserConfig,tConfigSpec WHERE tUserConfig.uConfigSpec=tConfigSpec.uConfigSpec "
			"AND tUserConfig.uUserConfig=%u",
			uUserConfig);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		TextError(mysql_error(&gMysql),1);
		return(1);
	}
	res=mysql_store_result(&gMysql);
	
	if((field=mysql_fetch_row(res)))
	{
		char cPath[100]={""};
		char cDir[256]={""};
		char cLogin[100]={""};
		char cPostDeployCmd[100]={""};
		FILE *fp;
		char *cp;

		sprintf(cLogin,"%.99s",ForeignKey("tUser","cLogin",uUser));
		sprintf(cPath,field[1],cLogin);
		
		//Make sure we have the full path available
		cp=strtok(cPath,"/");
		sprintf(cDir,"/%s",cp);
		mkdir(cDir,0x444);
		while((cp=strtok(NULL,"/"))!=NULL)
		{
			strcat(cDir,"/");
			strcat(cDir,cp);
			mkdir(cDir,0x444);
		}
		
		sprintf(cDir,field[1],cLogin);
		sprintf(cPath,"%s/%s",cDir,field[2]);
		if((fp=fopen(cPath,"w"))==NULL)
		{
			sprintf(gcQuery,"fopen() error. Could not open %s",cPath);
			TextError(gcQuery,1);
			return(1);
		}

		fprintf(fp,"%s\n",field[0]);

		fclose(fp);

		if(uNew)
			sprintf(cPostDeployCmd,"%.99s",field[3]);
		else
			sprintf(cPostDeployCmd,"%.99s",field[4]);
		if(cPostDeployCmd[0])
		{
			if((system(cPostDeployCmd)))
			{
				sprintf(gcQuery,"UpdateUserConfig() Warning: Execution of %s failed\n",cPostDeployCmd);
				TextError(gcQuery,1);
			}
		}
	}

	return(0);

}//int UpdateUserConfig(unsigned uUser)

