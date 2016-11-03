/*
FILE
	svn ID removed
	radius.c part of the mysqlradius.cgi application
PURPOSE
	raddb/users and raddb/clients file generation
LEGAL
	(C) 2002-2009, Gary Wallis and Hugo Urquiza. GPL License Applies.
TODO
	mysqlISP interface jobqueue
NOTES
	1-.
	If an external (mysqlISP) job provides for example
	unxsRadius.Server2=tristan, then we will currently add this server to the
	user and create a local job for that "slave" server. This job will be
	not be prefixed with "Ext", therefore it will not notify mysqlISP if it 
	fails for whatever reason. To fix this issue we need to pass to local
	job queue in ProcessExtJobQueue() the uJob=N line also. And prefix
	with "Ext".
	This should all be changed to a tServerGroup style setup and also
	every slave in these cLabel identified groups of servers will be able to mark
	the job as in an error state, For admin to clean up. This is explained
	above, see uJob=.
	2-.
	mysqlISP now provides all mysqlX subsystems the global ISP customer id
	number as part of the cJobData. unxsRadius.needs to use this also. This 
	is how I think we should: uISPClient should be the owner of the tUser entry
	and we should create a tClient but no tAuthorize for now.

	This has already been done in mysqlApache so we should get the code
	from there. Been there, done that.

	Example cJobData:
	...
	uISPClient=7832
	cClientName=Joe Client
	...

*/

//mysqlISP constants
//tJob.uJobStatus

#define macro_mySQLQueryStoreErrorTextExit	mysql_query(&gMysql,gcQuery);\
					if(mysql_errno(&gMysql))\
					{\
						printf("%s\n",mysql_error(&gMysql));\
						exit(1);\
					}\
					mysqlRes=mysql_store_result(&gMysql);

#define macro_mySQLQueryErrorTextExit	mysql_query(&gMysql,gcQuery);\
					if(mysql_errno(&gMysql))\
					{\
						printf("%s\n",mysql_error(&gMysql));\
						exit(1);\
					}\


#define mysqlISP_RemotelyQueued 7
#define mysqlISP_Waiting 10
//tInstance.uStatus:
#define mysqlISP_Deployed 4
#define mysqlISP_Canceled 5
#define mysqlISP_OnHold 6
#define mysqlISP_Invalid 31

typedef struct {
	char cPasswd[65];
	char cLogin[64];
	char cIP[19];
	char cProfile[64];
	char cServer1[33];
	char cServer2[33];
	char cEnterPasswd[16];	
	unsigned uLogins;
	unsigned uClearText;
	unsigned uProfileName;

	//tClient
	unsigned uISPClient; 
	char cClientName[33];//cLabel

	unsigned uParamClientName;

} structExtJobParameters;


#include <unistd.h>
#include "mysqlrad.h"

int MakeUsersFile(unsigned uHtml, unsigned uServer, char *cLogin);
int MakeClientsFile(unsigned uHtml, unsigned uServer);
int MakeNASListFile(unsigned uHtml, unsigned uServer);
int MakeNASPasswdFile(unsigned uHtml, unsigned uServer);
void to64(char *s, long v, int n);
void EncryptPasswd(char *pw);
void ConvertEscapedTabs(char *cLine);
void GetConfiguration(const char *cName, char *cValue, unsigned uHtml);

MYSQL mysqlext;
void ExtConnectDb(unsigned uHtml);

void ProcessJobQueue(unsigned uTestMode,char *cServer);
void ProcessExtJobQueue(char *cServer);
void AutoHoldRelease(char *cServer);
unsigned GetuProfileName(char *cProfile);
int SubmitISPJob(char *cJobName,char *cJobData,const char *cServer,unsigned uJobDate);
int InformExtJob(char *cRemoteMsg,char *cServer,unsigned uJob,unsigned uJobStatus);
int SubmitExtSingleJob(char *cJobName,char *cJobData,char *cServer,unsigned uJobDate,
		unsigned uJob);
int SubmitSingleJob(const char *cJobName,char *cJobData,const char *cServer,unsigned uJobDate);
unsigned uGetClientOwner(unsigned uClient);
unsigned GetuServer(char *cLabel);//tuserfunc.h
void CreateSlaveDelUserJobs(char *cDelUser);
void CleanUpServerGroup(char *cDelUser);
void CreateNewClient(structExtJobParameters *structExtParam);
void InitializeParams(structExtJobParameters *structExtParam);
void DeleteClient(structExtJobParameters *structExtParam);
void TextError(const char *cError, unsigned uContinue);//mainfunc.h

int CreateClientConf(unsigned uHtml,unsigned uServer);
void replace_spaces(char *cBuffer);
void ReleaseFromHold(const unsigned uUser, const unsigned uServer);
void PlaceOnHold(const unsigned uUser, const unsigned uServer);


int CreateClientConf(unsigned uHtml,unsigned uServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	FILE *fp;
	char cFile[256]={""};
	char cRaddbDir[256]={""};
	char cParam[100]={""}; 
	char cServer[33]={""};

	printf("CreateClientConf(%u,%u)\n",uHtml,uServer);
	if(!uServer)
	{
		if(uHtml)
			tNAS("Must specify valid uServer");
		else
                	fprintf(stderr,"Must specify valid uServer\n");
		exit(1);
	}

	sprintf(cServer,"%.32s",ForeignKey("tServer","cLabel",uServer));
	sprintf(cParam,"cRaddbDir-%s",cServer);
	GetConfiguration(cParam,cRaddbDir,uHtml);
	if(!cRaddbDir[0])
		GetConfiguration("cRaddbDir",cRaddbDir,uHtml);
	sprintf(cFile,"%s/clients.conf",cRaddbDir);

	if(!(fp=fopen(cFile,"w")))
        {
		char cMsg[256];
		sprintf(cMsg,"Can't open %.230s for write",cFile);
		if(uHtml)
			tNAS(cMsg);
		else
                	fprintf(stderr,"%s\n",cMsg);
               	return(1);
        }

	sprintf(gcQuery,"SELECT tNAS.cIP,tNAS.cLabel,tNAS.cKey,tNAS.cType,tNAS.cLogin,tNAS.cPasswd FROM tNAS,tNASGroup\
		WHERE tNAS.uNAS=tNASGroup.uNAS AND tNASGroup.uServer=%u\
		ORDER BY tNAS.cLabel",uServer);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
                	fprintf(stderr,"%s\n",mysql_error(&gMysql));
               	exit(1);
        }

        res=mysql_store_result(&gMysql);

	fprintf(fp,"#naslist unxsRadius.svn ID removed
	fprintf(fp,"#uServer:%u\n",uServer);

        while((field=mysql_fetch_row(res)))
	{
		replace_spaces(field[1]);
		fprintf(fp,"client %s {\n",field[0]);
		fprintf(fp,"\tipaddr = %s\n",field[0]);
		fprintf(fp,"\tsecret = %s\n",field[2]);
		fprintf(fp,"\tshortname = %s\n",field[1]);
		if(field[3][0])
			fprintf(fp,"\tnastype = %s\n",field[3]);
		if(field[4][0] && field[5][0])
		{
			fprintf(fp,"\tlogin = %s\n",field[4]);
			fprintf(fp,"\tpassword = %s\n",field[5]);
		}
		fprintf(fp,"}\n");
	}
        mysql_free_result(res);
	fclose(fp);

	return(0);

	
}//int CreateClientConf(unsigned uHtml,unsigned uServer)


void replace_spaces(char *cBuffer)
{
	register int x=0;
	for(x=0;x<strlen(cBuffer);x++)
		if(cBuffer[x]==' ') cBuffer[x]='_';

}//void replace_spaces(char *cBuffer)


int MakeUsersFile(unsigned uHtml, unsigned uServer, char *cLogin)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_ROW field2;
	FILE *fp,*flp;
	char cFile[256]={""};
	char cFileLock[256]={""};
	char cPasswd[65]={""};
	char cOpenMode[2]={"w"};
	char cRaddbDir[256]={""};
	unsigned uStaticIP;
	unsigned uAddMode=0;
	char cParam[100]={""}; 
	char cServer[33]={""};
	struct stat structStat;
	pid_t uProcessId=0;

	printf("MakeUsersFile(%u,%u,%s)\n",uHtml,uServer,cLogin);
	sprintf(cServer,"%.32s",ForeignKey("tServer","cLabel",uServer));

	sprintf(cParam,"cRaddbDir-%s",cServer); 
	GetConfiguration(cParam,cRaddbDir,uHtml); 
	if(!cRaddbDir[0]) 
		GetConfiguration("cRaddbDir",cRaddbDir,uHtml);
	
	sprintf(cFile,"%s/users",cRaddbDir);

	//See if the file is locked by this software
	sprintf(cFileLock,"%s.unxsRadius.lock",cFile);
	if(!stat(cFileLock,&structStat))
	{
		sleep(5);
		if(!stat(cFileLock,&structStat))
		{
                	fprintf(stderr,"MakeUsersFile() wait for file lock release failed!\n");
			exit(1);
		}
	}
	
	if(!uServer)
	{
		if(uHtml)
			tUser("Must specify valid uServer");
		else
                	fprintf(stderr,"Must specify valid uServer\n");
		exit(1);
	}

	if(cLogin[0])
	{
		cOpenMode[0]='a';
		uAddMode=1;
	}
	if(!(fp=fopen(cFile,cOpenMode)))
        {
		char cMsg[256];
		sprintf(cMsg,"Can't open %.230s for write",cFile);
		if(uHtml)
			tUser(cMsg);
		else
                	fprintf(stderr,"%s\n",cMsg);
               	exit(1);
        }

	if(!(flp=fopen(cFileLock,"w")))
        {
		char cMsg[256];
		sprintf(cMsg,"Can't open %.230s for write",cFileLock);
		if(uHtml)
			tUser(cMsg);
		else
                	fprintf(stderr,"%s\n",cMsg);
		exit(1);
        }
	uProcessId=getpid();
	fprintf(flp,"%u\n",uProcessId);
	fclose(flp);

	
	if(!uAddMode)
	{
//Page header
fprintf(fp,"#users unxsRadius.svn ID removed
fprintf(fp,"#For uServer:%u\n",uServer);

		//Outside select for each active uUser
		sprintf(gcQuery,"SELECT tUser.uUser,cLogin,cPasswd,uSimulUse,cIP,uClearText,uProfileName,cEnterPasswd FROM "
				"tUser,tServerGroup WHERE tUser.uUser=tServerGroup.uUser AND tServerGroup.uServer=%u "
				"AND tUser.uOnHold=0 ORDER BY tUser.cLogin",uServer);
	}
	else
		sprintf(gcQuery,"SELECT tUser.uUser,cLogin,cPasswd,uSimulUse,cIP,uClearText,uProfileName,cEnterPasswd FROM "
				"tUser,tServerGroup WHERE tUser.uUser=tServerGroup.uUser AND tServerGroup.uServer=%u "
				"AND tUser.uOnHold=0 AND tUser.cLogin='%s'",uServer,cLogin);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
		unlink(cFileLock);	
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
                	fprintf(stderr,"%s\n",mysql_error(&gMysql));
               	exit(1);
        }


        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
		//Allowing MACs now, and they start with 00 mostly
		//if(field[4][0] && field[4][0]!='0')
		if(field[4][0])
			uStaticIP=1;
		else
			uStaticIP=0;

		//Find most specific profile template 
sprintf(gcQuery,"SELECT tProfile.cProfile,tProfileName.cLabel,tProfile.uUser,tProfile.uServer,tProfile.uProfile,tProfile.uStaticIP,tProfile.uClearText\
		FROM tProfileName,tProfile,tServerGroup\
		WHERE tProfileName.uProfileName=%s\
		AND tProfile.uClearText=%s\
		AND tProfile.uStaticIP=%u\
		AND tServerGroup.uUser=%s\
		AND tServerGroup.uServer=%u\
		AND (tProfile.uUser=%s OR tProfile.uUser=0)\
		AND (tProfile.uServer=%u OR tProfile.uServer=0)\
		AND tProfile.uProfileName=tProfileName.uProfileName\
		ORDER BY tProfile.uServer DESC,tProfile.uUser DESC LIMIT 1",

		field[6],field[5],uStaticIP,field[0],uServer,field[0],uServer);

	        mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			unlink(cFileLock);	
			if(uHtml)
				htmlPlainTextError(mysql_error(&gMysql));
			else
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
        	}

        	res2=mysql_store_result(&gMysql);
		
		while((field2=mysql_fetch_row(res2)))
		{
/*
field
tUser.uUser		0
tUser.cLogin		1
tUser.cPasswd		2
tUser.uSimulUse		3
tUser.cIP		4
tUser.uClearText	5
tUser.uProfileName	6
tUser.cEnterPasswd	7

field2
tProfile.cProfile	0
tProfileName.cLabel	1
tProfile.uUser		2
tProfile.uServer	3
tProfile.uProfile	4
tProfile.uStaticIP	5
tProfile.uClearText 	6
*/
//Profile header
fprintf(fp,"#%s:%s uUser:%s uServer:%s uClearText:%s uStaticIP:%s\n",field2[1],field2[4],field2[2],field2[3],field2[6],field2[5]);

			ConvertEscapedTabs(field2[0]);

			//Clear text or encrypted passwd
			if(field[5][0]=='0')
				sprintf(cPasswd,"%.64s",field[2]);
			else
				sprintf(cPasswd,"%.64s",field[7]);

			//Static IP or not profile template
			if(field2[5][0]=='1')
				fprintf(fp,field2[0],field[1],cPasswd,
					field[3],field[4]);
			else
				fprintf(fp,field2[0],field[1],cPasswd,
						field[3]);

	                fprintf(fp,"\n");
	                fprintf(fp,"#\n");
        	}
        	mysql_free_result(res2);
	}
        mysql_free_result(res);

	fclose(fp);

	unlink(cFileLock);	
	return(0);

}//int MakeUsersFile()


void ConvertEscapedTabs(char *cLine)
{
	register int i,j=0;
	char cBuffer[2048];

	cBuffer[2047]=0;

	for(i=0;cLine[i] && i<2047;i++)
	{
		if(cLine[i]=='\\' && cLine[i+1]=='t')
		{
			cBuffer[j++]='\t';
			i++;
		}
		else
			cBuffer[j++]=cLine[i];
	}
	cBuffer[j]=0;
	
	strcpy(cLine,cBuffer);

}//void ConvertEscapedTabs(char *cLine)

//Passwd stuff
static unsigned char itoa64[] =         /* 0 ... 63 => ascii - 64 */
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void to64(s, v, n)
  register char *s;
  register long v;
  register int n;
{
    while (--n >= 0) {
        *s++ = itoa64[v&0x3f];
        v >>= 6;
    }
}//void to64(s, v, n)


void EncryptPasswd(char *pw)
{
	//Notes:
	//	We should change time based salt 
	//	(could be used for faster dictionary attack)
	//	to /dev/random based system.

        char salt[3];
        char passwd[102]={""};
        char *cpw;
	char cMethod[16] ={""}; 

	GetConfiguration("cCryptMethod",cMethod,0);
	if(!strcmp(cMethod,"MD5"))
	{
		char cSalt[] = "$1$01234567$";
	    	(void)srand((int)time((time_t *)NULL));
    		to64(&cSalt[3],rand(),8);
		cpw = crypt(pw,cSalt);
		// error not verified, str NULL ("") returned	
	}
	else
	{
		// default DES method
	        strcpy(passwd,pw);
    		(void)srand((int)time((time_t *)NULL));
    		to64(&salt[0],rand(),2);
		cpw=crypt(passwd,salt);
	}	
	strcpy(pw,cpw);

}//void EncryptPasswd(char *pw)


int MakeClientsFile(unsigned uHtml, unsigned uServer)
{
	return(CreateClientConf(0,uServer));

}//int MakeClientsFile()


int MakeNASListFile(unsigned uHtml, unsigned uServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	FILE *fp;
	char cFile[256]={""};
	char cRaddbDir[256]={"/usr/local/etc/raddb"};

	GetConfiguration("cRaddbDir",cRaddbDir,uHtml);
	sprintf(cFile,"%s/naslist",cRaddbDir);
	
	if(!uServer)
	{
		if(uHtml)
			tNAS("Must specify valid uServer");
		else
                	fprintf(stderr,"Must specify valid uServer\n");
		exit(1);
	}
	
	if(!(fp=fopen(cFile,"w")))
        {
		char cMsg[256];
		sprintf(cMsg,"Can't open %.230s for write",cFile);
		if(uHtml)
			tNAS(cMsg);
		else
                	fprintf(stderr,"%s\n",cMsg);
               	return(1);
        }

	sprintf(gcQuery,"SELECT tNAS.cIP,tNAS.cLabel,tNAS.cType FROM tNAS,tNASGroup\
		WHERE tNAS.uNAS=tNASGroup.uNAS AND tNASGroup.uServer=%u\
		ORDER BY tNAS.cLabel",uServer);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
                	fprintf(stderr,"%s\n",mysql_error(&gMysql));
               	exit(1);
        }

        res=mysql_store_result(&gMysql);

	fprintf(fp,"#naslist unxsRadius.svn ID removed
	fprintf(fp,"#uServer:%u\n",uServer);

        while((field=mysql_fetch_row(res)))
		fprintf(fp,"%s\t%s\t\t%s\n",field[0],field[1],field[2]);
        mysql_free_result(res);
	fclose(fp);

	MakeNASPasswdFile(uHtml,uServer);

	return(0);

}//int MakeNASListFile()


int MakeNASPasswdFile(unsigned uHtml, unsigned uServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	FILE *fp;
	char cFile[256]={""};
	char cRaddbDir[256]={"/usr/local/etc/raddb"};

	GetConfiguration("cRaddbDir",cRaddbDir,uHtml);
	sprintf(cFile,"%s/naspasswd",cRaddbDir);
	
	if(!uServer)
	{
		if(uHtml)
			tNAS("Must specify valid uServer");
		else
                	fprintf(stderr,"Must specify valid uServer\n");
		exit(1);
	}
	
	if(!(fp=fopen(cFile,"w")))
        {
		char cMsg[256];
		sprintf(cMsg,"Can't open %.230s for write",cFile);
		if(uHtml)
			tNAS(cMsg);
		else
                	fprintf(stderr,"%s\n",cMsg);
               	return(1);
        }

	sprintf(gcQuery,"SELECT tNAS.cIP,tNAS.cLogin,tNAS.cPasswd,tNAS.cLabel FROM tNAS,tNASGroup\
		WHERE tNAS.uNAS=tNASGroup.uNAS AND tNASGroup.uServer=%u\
		ORDER BY tNAS.cLabel",uServer);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
                	fprintf(stderr,"%s\n",mysql_error(&gMysql));
               	exit(1);
        }

        res=mysql_store_result(&gMysql);

	fprintf(fp,"#naspasswd unxsRadius.svn ID removed
	fprintf(fp,"#uServer:%u\n",uServer);

        while((field=mysql_fetch_row(res)))
		fprintf(fp,"#%s\n%s\t%s\t%s\n",field[3],field[0],field[1],field[2]);
        mysql_free_result(res);
	fclose(fp);

	return(0);

}//int MakeNASPasswdFile()

void StartRadiusServer(char *cRadiusdPath,char *cRadiusdArgs,unsigned uHtml);
void StopRadiusServer(char *cRadiusdPID);
int ReloadRadius(unsigned uHtml,unsigned uServer,unsigned uStopRequired);

int ReloadRadius(unsigned uHtml,unsigned uServer,unsigned uStopRequired)
{
	char cMsg[256];
	char cRadiusdArgs[256]={""};
	char cParam[100]={""};
	char cRadiusdPath[256]={""};
	char cRaddbDir[256]={""};
	char cRadiusdPID[256]={""};
	char cServer[33]={""};

	//For all only once
	strcpy(cServer,ForeignKey("tServer","cLabel",uServer));

	sprintf(cParam,"cRadiusdArgs-%s",cServer);
	GetConfiguration(cParam,cRadiusdArgs,uHtml);
	if(!cRadiusdArgs[0])
		GetConfiguration("cRadiusdArgs",cRadiusdArgs,uHtml);

	sprintf(cParam,"cRadiusdPath-%s",cServer);
	GetConfiguration(cParam,cRadiusdPath,uHtml);
	if(!cRadiusdPath[0])
		GetConfiguration("cRadiusdPath",cRadiusdPath,uHtml);

	sprintf(cParam,"cRaddbDir-%s",cServer);
	GetConfiguration(cParam,cRaddbDir,uHtml);
	if(!cRaddbDir[0])
		GetConfiguration("cRaddbDir",cRaddbDir,uHtml);
	sprintf(cParam,"cRadiusdPID-%s",cServer);
	GetConfiguration(cParam,cRadiusdPID,uHtml);
	if(!cRadiusdPID[0])
		GetConfiguration("cRadiusdPID",cRadiusdPID,uHtml);

	//debug only
	//////printf("[debug]Server:%u\ncParam:%s\ncRadiusdArgs:%s\ncRadiusdPath:%s\ncRaddbDir:%s\ncRadiusdPID:%s\n",uServer,cParam,cRadiusdArgs,cRadiusdPath,cRaddbDir,cRadiusdPID);
	if(access(cRadiusdPID,R_OK)==0) 
	{
		//If pid file exists, the radius server may be running
		sprintf(gcQuery,"kill -0 `cat %s` 2> /dev/null",cRadiusdPID);
		////printf("[debug] running: %s\n",gcQuery);
		if(system(gcQuery))
		{
			//We don't have a radius server running, just a dead pidfile
			//Try to start again radius server
			//and remove stale pidfile!
		//	//printf("[debug] Dead pidfile\n");
			unlink(cRadiusdPID);
			StartRadiusServer(cRadiusdPath,cRadiusdArgs,uHtml);
		}
	}
	else
	{
		//No pid file, start radius server
		////printf("[debug] No pid file, start radius server (%s)\n",cRadiusdPID);
		StartRadiusServer(cRadiusdPath,cRadiusdArgs,uHtml);
	}
	
	if(uStopRequired)
	{
		////printf("[debug] Stop required\n");
		StopRadiusServer(cRadiusdPID);
		StartRadiusServer(cRadiusdPath,cRadiusdArgs,uHtml);
	}
	else
	{
		sprintf(gcQuery,"kill -HUP `cat %s` > /dev/null",cRadiusdPID);
		if(system(gcQuery))
		{
			sprintf(cMsg,"Could not reload radius server! Contact support asap.");
			if(uHtml)
				tUser(cMsg);
			else
				fprintf(stderr,"%s\n",cMsg);
			return(1);
		}
	}

	return(0);

}//int ReloadRadius(unsigned uHtml,unsigned uServer)


void StartRadiusServer(char *cRadiusdPath,char *cRadiusdArgs,unsigned uHtml)
{
	char cMsg[256];
	int i=0;
	sprintf(gcQuery,"%s %s",cRadiusdPath,cRadiusdArgs);
	//printf("[debug] Running %s\n",gcQuery);
	i=system(gcQuery);
	
	if(i)
	{
		sprintf(cMsg,"Radius daemon was not running. Could not start!(uRet=%i)",i);
		if(uHtml)
			tUser(cMsg);
		else
		{
			fprintf(stderr,"%s\n",cMsg);
			exit(1);
		}
	}
	else
	{
		sprintf(cMsg,"Radius daemon was not running. Was started");
		if(uHtml)
			tUser(cMsg);
		else
		{
			fprintf(stderr,"%s\n",cMsg);
			exit(0);
		}
	}
}//int StartRadiusServer(char *cRadiusdPath,char *cRadiusdArgs)


void StopRadiusServer(char *cRadiusdPID)
{
	if(access(cRadiusdPID,R_OK)==0)
	{
		sprintf(gcQuery,"kill -TERM `cat %s` > /dev/null",cRadiusdPID);
		//printf("[debug] running: %s\n",gcQuery);
		if(system(gcQuery))
		{
			printf("I couldn't stop radius server using the pidfile.\n");
			if(system("killall -TERM radiusd"))
			{
				printf("Oops neither using killall. Server is stopped.\n");
				exit(0);
			}
		}
	}

}//void StopRadiusServer(void)


void GetConfiguration(const char *cName, char *cValue, unsigned uHtml)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='%s'",cName);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
		{
        	        tConfiguration(mysql_error(&gMysql));
		}
		else
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
                strcpy(cValue,field[0]);
        mysql_free_result(res);

}//void GetConfiguration(char *cName, char *cValue)


void ParseExtParams(structExtJobParameters *structExtParam, char *cJobData)
{
	char *cp,*cp2;

	if((cp=strstr(cJobData,"ClearPasswd=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+12,'\n'))) *cp2=0;
		sprintf(structExtParam->cPasswd,"%.64s",cp+12);
		if(cp2) *cp2='\n';
		//Can have emtpy ClearPasswd for migration from only des3 passwd
		//imports. 
		if(structExtParam->cPasswd[0])
			structExtParam->uClearText=1;
	}

	//ClearText passwd has precedence
	if( !structExtParam->uClearText && (cp=strstr(cJobData,".Passwd=")) )
	{
		cp2=NULL;
		if((cp2=strchr(cp+8,'\n'))) *cp2=0;
		sprintf(structExtParam->cPasswd,"%.64s",cp+8);
		if(cp2) *cp2='\n';
	}

	if((cp=strstr(cJobData,"Login=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+6,'\n'))) *cp2=0;
			sprintf(structExtParam->cLogin,"%.63s",cp+6);
		if(cp2) *cp2='\n';
	}

	if((cp=strstr(cJobData,"IP=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+3,'\n'))) *cp2=0;
			sprintf(structExtParam->cIP,"%.18s",cp+3);
		if(cp2) *cp2='\n';
	}

	if((cp=strstr(cJobData,"Server1=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+8,'\n'))) *cp2=0;
			sprintf(structExtParam->cServer1,"%.32s",cp+8);
		if(cp2) *cp2='\n';
	}

	if((cp=strstr(cJobData,"Server2=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+8,'\n'))) *cp2=0;
			sprintf(structExtParam->cServer2,"%.32s",cp+8);
		if(cp2) *cp2='\n';
	}

	if((cp=strstr(cJobData,"Profile=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+8,'\n'))) *cp2=0;
			sprintf(structExtParam->cProfile,"%.63s",cp+8);
		if(cp2) *cp2='\n';

	/*	if((cp=strchr(structExtParam->cProfile,'(')))
		{
			*cp=0;
			cp--;
			if(*cp==' ') *cp=0;
		}*/
		structExtParam->uProfileName=GetuProfileName(structExtParam->cProfile);
	}

	if((cp=strstr(cJobData,"Logins=")))
		sscanf(cp+7,"%u",&structExtParam->uLogins);

	//Service can have both Passwd type params and have override
	//This not known to be of any use so far...but you never know
	if((cp=strstr(cJobData,"ClearText=")))
	{
		cp+=10;
		if( *cp=='Y' || *cp=='y')
			structExtParam->uClearText=1;
		else
			structExtParam->uClearText=0;
	}

	//tClient
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
		structExtParam->uParamClientName=1;
	}
	

	//tSiteUsers	
		
	//Debug only	
	printf("cLogin=%s cPasswd=%s cProfile=%s(%u)\n"
		"uLogins=%u uClearText=%u\n"
		"cServer1=%s cServer2=%s\ncIP=%s\n"
		"uISPClient=%u\ncClientName=%s\n",
			structExtParam->cLogin,
			structExtParam->cPasswd,
			structExtParam->cProfile,
			structExtParam->uProfileName,
			structExtParam->uLogins,
			structExtParam->uClearText,
			structExtParam->cServer1,
			structExtParam->cServer2,
			structExtParam->cIP,
			structExtParam->uISPClient,
			structExtParam->cClientName);


}//void ParseExtParams(structExtJobParameters *structExtParam)


unsigned uGetClientOwner(unsigned uClient)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uRet=0;

	sprintf(gcQuery,"SELECT uOwner FROM "TCLIENT" WHERE uClient=%u",uClient);
	//printf("%s\n",gcQuery);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uRet);
	mysql_free_result(res);

	return(uRet);

}//unsigned uGetClientOwner(unsigned uClient)


//Needs tConfiguration cExtunxsRadius.p/Db/Pwd set.
void ProcessExtJobQueue(char *cServer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	structExtJobParameters structExtParam;

	unsigned uJob=0;
	unsigned uUser=0;
	unsigned uOwner=0;
	unsigned uJobClient=0;

	time_t clock;
		
	ExtConnectDb(0);

	time(&clock);

	//debug only
	//printf("ProcessExtJobQueue()\n");

	sprintf(gcQuery,"SELECT cJobName,cJobData,uJob,cServer,uJobClient FROM tJob WHERE (cServer='Any' OR cServer='%s') "
			"AND uJobStatus=%u AND uJobDate<=%lu AND cJobName LIKE 'unxsRadius.%%'",cServer,mysqlISP_Waiting,clock);

	//Debug only	
	//printf("%s\n",gcQuery);

	mysql_query(&mysqlext,gcQuery);
        if(mysql_errno(&mysqlext))
		fprintf(stderr,"%s\n",mysql_error(&mysqlext));

	res=mysql_store_result(&mysqlext);
        while((field=mysql_fetch_row(res)))
	{
		//debug
		printf("mysqlISP.tJob.cServer=%s\n",field[3]);
		
		sscanf(field[4],"%u",&uJobClient);
		if(!(uOwner=uGetClientOwner(uJobClient)))
		{
			printf("Warning: uGetClientOwner() didn't return a value. uJobClient=%u\n",uJobClient);
			printf("Warning: uOwner set to 1 (Root)\n");
			uOwner=1; //Safe default.
		}

		InitializeParams(&structExtParam);

		sscanf(field[2],"%u",&uJob);

		if(!strcmp("unxsRadius.Unlim.New",field[0]))
		{	
			MYSQL_RES *mysqlRes;
			MYSQL_ROW mysqlField;
			unsigned uServer=0;

			printf("\n%s(%s):\n",field[0],field[2]);
		
			ParseExtParams(&structExtParam,field[1]);
		
			if(structExtParam.uClearText)
			{
				strcpy(structExtParam.cEnterPasswd,
					structExtParam.cPasswd);
				EncryptPasswd(structExtParam.cPasswd);
			}

			//We expect mysqlISP to pass on good data but...
			//Other error conditions pre-update
			//Each uProfileName entry MUST have tProfile correct entries
			if(!structExtParam.uProfileName)
			{
				InformExtJob("No such profile",cServer,uJob,mysqlISP_Invalid);
				fprintf(stderr,"No such profile\n");
				goto Fast_Exit;
			}
			
			//Since we now support multiple servers for RADIUS accounts
			//we need to do a slightly more clever check before adding a new entry
			//at tUser table below.
			//This is explained as follows:
			//You have a radius account service which is configured to sumit a job
			//for three radius servers: radius0, radius1 and radius2
			//The first of these servers it will be the one that inserts the tUser
			//record entry. For that, we need to verify first if the record exists
			//and was created by another job queue processor. How to do this? If
			//the tUser record exists it means that another server processed the entry before us.
			
			sprintf(gcQuery,"SELECT uUser,uOnHold FROM tUser WHERE cLogin='%s'",structExtParam.cLogin);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("tUser Select failed",cServer,uJob,
					mysqlISP_Waiting);
				fprintf(stderr,"tUser Select failed");
				continue;
			}

			mysqlRes=mysql_store_result(&gMysql);
			if((mysqlField=mysql_fetch_row(mysqlRes)))
			{
				unsigned uOnHold=0;
				sscanf(mysqlField[0],"%u",&uUser);
				sscanf(mysqlField[0],"%u",&uOnHold);

				if(uOnHold)
				{
					//What? The record is on hold? We are deploying something here
					//Update it!
					sprintf(gcQuery,"UPDATE tUser SET uOnHold=0 WHERE uUser=%u",uUser);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						fprintf(stderr,"%s\n",mysql_error(&gMysql));
						InformExtJob("tUser Update failed",cServer,uJob,
							mysqlISP_Invalid);
						fprintf(stderr,"tUser Select failed");
						continue;
					}
				}
			}
			else
			{
				//No tUser record, we have to add it
				sprintf(gcQuery,"INSERT INTO tUser SET  cLogin='%s', cEnterPasswd='%s', uClearText=%u,"
						"cPasswd='%s', uProfileName=%u, uSimulUse=%u, cIP='%s', uClient=%u,uOwner=%u, uCreatedBy=1, uCreatedDate=%lu"
						,structExtParam.cLogin
						,structExtParam.cEnterPasswd
						,structExtParam.uClearText
						,structExtParam.cPasswd
						,structExtParam.uProfileName
						,structExtParam.uLogins
						,structExtParam.cIP
						,structExtParam.uISPClient
						,uOwner
						,clock);

			//Debug only	
	
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
       		 		{
					fprintf(stderr,"%s\n",mysql_error(&gMysql));
					InformExtJob("tUser Insert failed",cServer,uJob,
							mysqlISP_Invalid);
					fprintf(stderr,"tUser Insert failed\n");
					continue;
				}
				uUser=mysql_insert_id(&gMysql);
			}
			mysql_free_result(mysqlRes);

			uServer=GetuServer(cServer);
			sprintf(gcQuery,"SELECT uUser FROM tServerGroup WHERE uUser=%u AND uServer=%u",uUser,uServer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("tServerGroup Select failed",cServer,uJob,
					mysqlISP_Invalid);
				fprintf(stderr,"tServerGroup Select failed");
				continue;
			}

			mysqlRes=mysql_store_result(&gMysql);
			if(!mysql_num_rows(mysqlRes))
			{
				sprintf(gcQuery,"INSERT INTO tServerGroup SET  uUser=%u,uServer=%u",uUser,uServer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
        			{
					fprintf(stderr,"%s\n",mysql_error(&gMysql));
					InformExtJob("tServerGroup Insert failed",
						    cServer,uJob,mysqlISP_Invalid);
					fprintf(stderr,"tServerGroup Insert failed-1\n");
				}
			}
			mysql_free_result(mysqlRes);

			SubmitExtSingleJob("ExtNewUser",
					structExtParam.cLogin,cServer,clock,uJob);
			InformExtJob("unxsRadius.ExtNewUser",cServer,uJob,
						mysqlISP_RemotelyQueued);

			//CreateNewClient(&structExtParam);

			printf("Ok\n");
		}//NewUser

		//Cancel
		else if(!strcmp("unxsRadius.Unlim.Cancel",field[0]))
		{
			MYSQL_RES *res2;
			unsigned uServer=0;

			printf("\n%s(%s):\n",field[0],field[2]);
			ParseExtParams(&structExtParam,field[1]);

			sprintf(gcQuery,"SELECT uUser FROM tUser WHERE cLogin='%s'",structExtParam.cLogin);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("Select query failed",cServer,uJob,
						mysqlISP_Invalid);
				fprintf(stderr,"Select query failed\n");
			}
			res2=mysql_store_result(&gMysql);
			if(mysql_num_rows(res2))
			{
				//The first server to get here deletes the tUser record
				CleanUpServerGroup(structExtParam.cLogin);

				sprintf(gcQuery,"DELETE FROM tUser WHERE cLogin='%s'",structExtParam.cLogin);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					fprintf(stderr,"%s\n",mysql_error(&gMysql));
					InformExtJob("tUser delete failed",cServer,uJob,
							mysqlISP_Invalid);
					fprintf(stderr,"tUser delete failed\n");
				}
			}
			uServer=GetuServer(cServer);

			//Rebuild users file
			MakeUsersFile(0,uServer,"");
			ReloadRadius(0,uServer,0);
			//Update instance and job stuff
			InformExtJob("User removed",cServer,uJob,mysqlISP_Deployed);


		}//Cancel

		//Mod
		//Implicit hold release on each mod: uOnHold=0
		else if(!strcmp("unxsRadius.Unlim.Mod",field[0]))
		{	
			unsigned uServer=0;
			printf("\n%s(%s):\n",field[0],field[2]);

			ParseExtParams(&structExtParam,field[1]);

			if(structExtParam.uClearText)
			{
				strcpy(structExtParam.cEnterPasswd,
					structExtParam.cPasswd);
				EncryptPasswd(structExtParam.cPasswd);
			}

			//Other error conditions pre-update
			//Each uProfileName entry MUST have tProfile correct entries
			if(!structExtParam.uProfileName)
			{
				InformExtJob("No such profile",cServer,uJob,mysqlISP_Invalid);
				fprintf(stderr,"No such profile\n");
				goto Fast_Exit;
			}

			sprintf(gcQuery,"UPDATE tUser SET cPasswd='%s',cEnterPasswd='%s',"
					"uClearText=%u,uProfileName=%u,uSimulUse=%u,cIP='%s',"
					"uModBy=1,uModDate=%lu,uOnHold=0 WHERE cLogin='%s'",
					structExtParam.cPasswd,
					structExtParam.cEnterPasswd,
					structExtParam.uClearText,
					structExtParam.uProfileName,
					structExtParam.uLogins,
					structExtParam.cIP,
					clock,
					structExtParam.cLogin);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("Update failed",cServer,uJob,
						mysqlISP_Invalid);
				fprintf(stderr,"Update failed\n");
			}
			uServer=GetuServer(cServer);

			//Rebuild users file
			MakeUsersFile(0,uServer,"");
			ReloadRadius(0,uServer,0);
			//Update instance and job stuff
			InformExtJob("User modified",cServer,uJob,mysqlISP_Deployed);
		}//Mod

		else if(!strcmp("unxsRadius.Unlim.Hold",field[0]))
		{	
			unsigned uServer=0;
			printf("\n%s(%s):\n",field[0],field[2]);

			ParseExtParams(&structExtParam,field[1]);

			if(structExtParam.uClearText)
			{
				strcpy(structExtParam.cEnterPasswd,
					structExtParam.cPasswd);
				EncryptPasswd(structExtParam.cPasswd);
			}

			sprintf(gcQuery,"UPDATE tUser SET uOnHold=1,uModBy=1,uModDate=%lu WHERE cLogin='%s'",
					clock,
					structExtParam.cLogin);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("Update failed",cServer,uJob,
						mysqlISP_Invalid);
				continue;
			}
			uServer=GetuServer(cServer);

			//Rebuild users file
			MakeUsersFile(0,uServer,"");
			ReloadRadius(0,uServer,0);
			//Update instance and job stuff
			InformExtJob("User set on hold",cServer,uJob,mysqlISP_Deployed);
		}//Hold

		else if(!strcmp("unxsRadius.Unlim.RemoveHold",field[0]))
		{	
			unsigned uServer=0;
			printf("\n%s(%s):\n",field[0],field[2]);

			ParseExtParams(&structExtParam,field[1]);

			if(structExtParam.uClearText)
			{
				strcpy(structExtParam.cEnterPasswd,
					structExtParam.cPasswd);
				EncryptPasswd(structExtParam.cPasswd);
			}

			sprintf(gcQuery,"UPDATE tUser SET uOnHold=0,uModBy=1,uModDate=%lu WHERE cLogin='%s'",
					clock,
					structExtParam.cLogin);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("Update failed",cServer,uJob,
						mysqlISP_Invalid);
				continue;
			}
			uServer=GetuServer(cServer);

			//Rebuild users file
			MakeUsersFile(0,uServer,"");
			ReloadRadius(0,uServer,0);
			//Update instance and job stuff
			InformExtJob("User restored",cServer,uJob,mysqlISP_Deployed);
		
		}//RemoveHold

		else if(1)
		{
			fprintf(stderr,"Unknown job:%s\n",field[0]);
		}
	}

Fast_Exit:
	mysql_free_result(res);

}//void ProcessExtJobQueue(char *cServer)
		

//Uses MYSQL mysqlext
void ExtConnectDb(unsigned uHtml)
{
	char cDbIp[256]={""};
	char *cEffectiveDbIp=NULL;
	char cDbName[256]={"unxsisp"};
	char cDbPwd[256]={"wsxedc"};
	char cDbLogin[256]={"unxsisp"};

	GetConfiguration("cExtJobQueueDbIp",cDbIp,0);
	GetConfiguration("cExtJobQueueDbName",cDbName,0);
	GetConfiguration("cExtJobQueueDbPwd",cDbPwd,0);
	GetConfiguration("cExtJobQueueDbLogin",cDbLogin,0);

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

}//end of ExtConnectDb()


unsigned GetuProfileName(char *cProfile)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uProfileName=0;

	sprintf(gcQuery,"SELECT uProfileName FROM tProfileName WHERE cLabel='%s'"
						,cProfile);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
               	exit(1);
        }
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uProfileName);
	mysql_free_result(res);

	return(uProfileName);

}//unsigned GetuProfileName(char *cProfile)	


int InformExtJob(char *cRemoteMsg,char *cServer,unsigned uJob,unsigned uJobStatus)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uJobGroup=0;
	unsigned uInstance=0;
	time_t clock;

	time(&clock);

	sprintf(gcQuery,"UPDATE tJob SET cServer='%s',cRemoteMsg='%.32s',uModBy=1,uModDate=%lu,uJobStatus=%u WHERE uJob=%u",
			cServer,cRemoteMsg,clock,uJobStatus,uJob);

	mysql_query(&mysqlext,gcQuery);
	if(mysql_errno(&mysqlext))
	{
		fprintf(stderr,"%s\n",mysql_error(&mysqlext));
		SubmitISPJob("unxsRadius.InformExtJob.Failed",
				(char *)mysql_error(&mysqlext),cServer,clock);
		return(1);
	}
	//Special case, invalid instances, only we update tInstance and exit
	////printf("[debug]JobStatus=%u\n",uJobStatus);
	if(uJobStatus==mysqlISP_Invalid)
	{
		sprintf(gcQuery,"SELECT uInstance FROM tJob WHERE uJob=%u",uJob);
		mysql_query(&mysqlext,gcQuery);
		if(mysql_errno(&mysqlext))
		{
			fprintf(stderr,"%s\n",mysql_error(&mysqlext));
			return(1);
		}
		res=mysql_store_result(&mysqlext);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uInstance);
			sprintf(gcQuery,"UPDATE tInstance SET uStatus=%u,uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uInstance=%u",
					mysqlISP_Invalid,uInstance);
			mysql_query(&mysqlext,gcQuery);
			if(mysql_errno(&mysqlext))
				fprintf(stderr,"%s\n",mysql_error(&mysqlext));
		}
		else
			fprintf(stderr,"Warning, could not update tInstance record\n");
		return(0);
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

	sprintf(gcQuery,"SELECT uJobStatus=%u,(max(uJobStatus)=min(uJobStatus)),cJobName "
			"FROM tJob WHERE uJobGroup=%u GROUP BY uJobGroup",mysqlISP_Deployed,uJobGroup);

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


//Uses mysqlext
int SubmitISPJob(char *cJobName,char *cJobData,const char *cServer,unsigned uJobDate)
{
	time_t clock;

	time(&clock);
	
	sprintf(gcQuery,"INSERT INTO tJob SET  cServer='%s', cJobName='%s', cJobData='%.1024s', uJobDate=%u, uOwner=1, uCreatedBy=1, uCreatedDate=%lu, uJobStatus=%u, cLabel='unxsRadius.SubmitISPJob'",
			cServer
			,cJobName
			,TextAreaSave(cJobData)
			,uJobDate
			,clock
			,mysqlISP_Waiting);
	mysql_query(&mysqlext,gcQuery);
	if(mysql_errno(&mysqlext))
	{
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
		return(1);
	}

	return(0);

}//int SubmitISPJob()


int SubmitExtSingleJob(char *cJobName,char *cJobData,char *cServer,unsigned uJobDate,
		unsigned uJob)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE cJobName='%s' AND cJobData='%s' AND cServer='%s'",cJobName,cJobData,cServer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	
	if(mysql_num_rows(res)==0)
	{
		time_t clock;

		time(&clock);
	
		sprintf(gcQuery,"INSERT INTO tJob SET  cServer='%s', cJobName='%s', cJobData='%.1024s\nmysqlISP.tJob.uJob=%u', uJobDate=%u, uOwner=%u, uCreatedBy=%u, uCreatedDate=%lu",
			cServer
			,cJobName
			,TextAreaSave(cJobData)
			,uJob
			,uJobDate
			,1
			,1
			,clock);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

	return(0);

}//int SubmitExtSingleJob()


void ProcessJobQueue(unsigned uTestMode,char *cServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uRestartServer=0;
	unsigned uStopRequired=0;
	unsigned uMakeUsersFile=0;
	unsigned uMakeNASFiles=0;
	unsigned uNewUser=0;
	unsigned uNewNAS=0;
	char cUser[100]={"XxX"};
	char cNewUser[100]={""};
	unsigned uServer=0;
	char *cp;

	unsigned uOnlyOnce=1;

	uServer=GetuServer(cServer);
	if(!uServer)
	{
		fprintf(stderr,"Must specify valid uServer: cServer=%s unknown\n",cServer);
		exit(1);
	}

	if(uTestMode)
		printf("First pass check for redundant jobs. uServer=%u\n",uServer);

	//Remove jobs to be ignored: Add/(n x Mod/)Del=Eliminate all
	sprintf(gcQuery,"SELECT cJobName,cJobData FROM tJob WHERE cServer='%s' ORDER BY cJobData,uJob",cServer);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
               	return;
        }
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{
		if(strcmp(field[1],cUser))
		{
			strcpy(cUser,field[1]);
			uNewUser=0;
			uNewNAS=0;
		}

		if(uTestMode)
			printf("%s %s\n",field[0],field[1]);
		
		//User Jobs
		if(strstr(field[0],"NewUser")) uNewUser=1;
		if(strstr(field[0],"DelUser") && uNewUser)
		{
			if(uTestMode)
				printf("Eliminating all jobs for %s from queue\n",field[1]);
			if(!uTestMode)
			{
				if(!strncmp(field[0],"Ext",3))
				sprintf(gcQuery,"DELETE FROM tJob WHERE cServer='%s' AND cJobData LIKE '%s\n%%'",cServer,field[1]);
				sprintf(gcQuery,"DELETE FROM tJob WHERE cServer='%s' AND cJobData='%s'",cServer,field[1]);
        			mysql_query(&gMysql,gcQuery);
        			if(mysql_errno(&gMysql))
        			{
					fprintf(stderr,"%s\n",mysql_error(&gMysql));
					return;
				}
			}
		}
		
		//NAS Jobs
		if(!strcmp(field[0],"NewNAS")) uNewNAS=1;
		if(!strcmp(field[0],"DelNAS") && uNewNAS)
		{
			if(uTestMode)
				printf("Eliminating all NAS jobs for %s from queue\n",field[1]);
			if(!uTestMode)
			{
				sprintf(gcQuery,"DELETE FROM tJob WHERE cServer='%s' AND cJobData='%s'",cServer,field[1]);
        			mysql_query(&gMysql,gcQuery);
        			if(mysql_errno(&gMysql))
        			{
					fprintf(stderr,"%s\n",mysql_error(&gMysql));
					return;
				}
			}
		}
	}
	
	uNewUser=0;
	if(uTestMode)
		printf("Second pass execute jobs\n");
	//Now run remaining jobs
	sprintf(gcQuery,"SELECT cJobName,cJobData FROM tJob WHERE cServer='%s'",
			cServer);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
               	return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
        	while((field=mysql_fetch_row(res)))
		{
			if(uTestMode)
				printf("%s %s\n",field[0],field[1]);

			//User jobs can be Ext prefixed (from mysqlISP central control)
			if(strstr(field[0],"NewUser"))
			{
				if(!uNewUser)
				{
				//Only if single add then append, else do the whole
				//file. See logic below. Save the first cNewUser for 
				//append here.
				if(!strncmp(field[0],"Ext",3))
				{
					cp=NULL;
					if((cp=strchr(field[1],'\n'))) *cp=0;
					sprintf(cNewUser,"%.99s",field[1]);
					if(cp) *cp='\n';
				}
				else
				{
					sprintf(cNewUser,"%.99s",field[1]);
				}
				}
				uNewUser++;
				uRestartServer=1;
			}
			else if(strstr(field[0],"DelUser"))
			{
				uMakeUsersFile=1;
				uRestartServer=1;
	
				if(!strncmp(field[0],"Ext",3))
				{
					cp=NULL;
					if((cp=strchr(field[1],'\n'))) *cp=0;
					sprintf(cNewUser,"%.99s",field[1]);
					if(cp) *cp='\n';
				}
				else
				{
					sprintf(cNewUser,"%.99s",field[1]);
				}
	
	
				CreateSlaveDelUserJobs(cNewUser);
				CleanUpServerGroup(cNewUser);
	
				sprintf(gcQuery,"DELETE FROM tUser WHERE cLogin='%s'",cNewUser);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					fprintf(stderr,"%s\n",mysql_error(&gMysql));
	
			}
			else if(strstr(field[0],"ModUser"))
			{
				uMakeUsersFile=1;
				uRestartServer=1;
			}
	
			//NAS jobs
			else if(!strcmp(field[0],"NewNAS"))
			{
				uMakeNASFiles=1;
				uRestartServer=1;
				uStopRequired=1;
			}
			else if(!strcmp(field[0],"ModNAS"))
			{
				uMakeNASFiles=1;
				uRestartServer=1;
				uStopRequired=1;
			}
			else if(!strcmp(field[0],"DelNAS"))
			{
				uMakeNASFiles=1;
				uRestartServer=1;
				uStopRequired=1;
			}
			else if(!strcmp(field[0],"AddNASServer"))
			{
				uMakeNASFiles=1;
				uRestartServer=1;
				uStopRequired=1;
			}
			else if(!strcmp(field[0],"DelNASServer"))
			{
				uMakeNASFiles=1;
				uRestartServer=1;
				uStopRequired=1;
			}
			else if(1)
			{
				fprintf(stderr,"Unknown job: %s\n",field[0]);
				continue;
			}

			//Clear job entry
			if(!uTestMode)
			{
				sprintf(gcQuery,"DELETE FROM tJob WHERE cServer='%s' AND cJobName='%s' AND cJobData='%s'",cServer,field[0],field[1]);
	        mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
	
				//mysqlISP Ext jobs

				if(!strncmp(field[0],"Ext",3) && (cp=strstr(field[1],"uJob=")))
				{
					char cMsg[33];
					unsigned uExtJob=0;

					//In a loop...
					if(uOnlyOnce)
					{
						ExtConnectDb(0);
						uOnlyOnce=0;
					}

					sprintf(cMsg,"unxsRadius.%.20s",field[0]);
					sscanf(cp+5,"%u",&uExtJob);
					InformExtJob(cMsg,cServer,uExtJob,mysqlISP_Deployed);
				}
			}//!uTestMode
		}
	
		if(uTestMode)
			printf("Grouped system mods: uMakeUsersFile=%u uNewUser=%u uMakeNASFiles=%u\nuRestartServer=%u cNewUser=%s uServer=%u\n",uMakeUsersFile,uNewUser,uMakeNASFiles,uRestartServer,cNewUser,uServer);

		//Do the whole file unless only single NewUser then append
		if(uMakeUsersFile || uNewUser>1)
		{
			if(MakeUsersFile(0,uServer,""))
			{
				//wait try once more
				sleep(5);
				if(MakeUsersFile(0,uServer,""))
				{
					fprintf(stderr,"MakeUsersFile() failed after waiting for file lock release");
        				mysql_free_result(res);
					return;
				}
			}
		}
		else if(cNewUser[0] && uNewUser==1)
		{
			MakeUsersFile(0,uServer,cNewUser);
		}
		else if(uMakeUsersFile || !uNewUser)
		{
			MakeUsersFile(0,uServer,"");
		}

		if(uMakeNASFiles)
		{
			CreateClientConf(0,uServer);
		}

		if(uRestartServer)
		{
			printf("Calling ReloadRadius(0,%u,%u)\n",uServer,uStopRequired);
			ReloadRadius(0,uServer,uStopRequired);
		}

	}//If jobs
        mysql_free_result(res);

}//void ProcessJobQueue(unsigned uTestMode,char *cServer)


unsigned GetuUser(char *cDelUser)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uUser=0;

	//debug only
	//printf("GetuUser(%s)\n",cDelUser);

	sprintf(gcQuery,"SELECT uUser FROM tUser WHERE cLogin='%s'",cDelUser);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		return(0);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uUser);
        mysql_free_result(res);

	return(uUser);

}//unsigned GetuUser(char *cDelUser)



void CreateSlaveDelUserJobs(char *cDelUser)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	time_t clock;
	unsigned uUser=0;

	uUser=GetuUser(cDelUser);
	//debug only
	//printf("CreateSlaveDelUserJobs(%s/%u)\n",cDelUser,uUser);


	if(!uUser)
	{
		fprintf(stderr,"Could not find uUser: %s\n",gcQuery);
		return;
	}

	sprintf(gcQuery,"SELECT tServer.cLabel FROM tServerGroup,tServer WHERE tServer.uServer=tServerGroup.uServer AND tServerGroup.uUser=%u AND tServerGroup.uServer!=1",uUser);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		return;
	}
	time(&clock);
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
		SubmitSingleJob("DelUser",cDelUser,field[0],clock);
        mysql_free_result(res);
	
}//void CreateSlaveDelUserJobs(char *cDelUser)


void CleanUpServerGroup(char *cDelUser)
{
	unsigned uUser;

	uUser=GetuUser(cDelUser);
	//debug only
	//printf("CleanUpServerGroup(%s/%u)\n",cDelUser,uUser);

	sprintf(gcQuery,"DELETE FROM tServerGroup WHERE uUser=%u",uUser);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		fprintf(stderr,"%s\n",mysql_error(&gMysql));

	//debug only
	//printf("Out CleanUpServerGroup(%s/%u)\n",cDelUser,uUser);


}//CleanUpServerGroup(char *cDelUser)


void CreateNewClient(structExtJobParameters *structExtParam)
{
	time_t luClock;
	MYSQL_RES *res;

	//Create new tClient if needed
	time(&luClock);
	if(structExtParam->uISPClient>1)
	{
		sprintf(gcQuery,"SELECT uClient FROM tClient WHERE uClient=%u",
			structExtParam->uISPClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			return;
		}

		res=mysql_store_result(&gMysql);
		if(!mysql_num_rows(res)) 
		{
			sprintf(gcQuery,"INSERT INTO tClient SET cLabel='%s',cInfo='(uISPClient)',uClient=%u,uOwner=1,uCreatedBy=1,uCreatedDate=%lu",structExtParam->cClientName,structExtParam->uISPClient,luClock);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				mysql_free_result(res);
				return;
			}
		}
		else
		{
			sprintf(gcQuery,"UPDATE tClient SET cLabel='%s',cInfo='(uISPClient)',uModBy=1,uModDate=%lu WHERE uClient=%u",structExtParam->cClientName,luClock,structExtParam->uISPClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				mysql_free_result(res);
				return;
			}
		}
		mysql_free_result(res);
	}

}//void CreateNewClient(structExtJobParameters *structExtParam)

			
void InitializeParams(structExtJobParameters *structExtParam)
{
		structExtParam->cPasswd[0]=0;
		structExtParam->cLogin[0]=0;
		structExtParam->cProfile[0]=0;
		structExtParam->cEnterPasswd[0]=0;
		structExtParam->uLogins=1;
		structExtParam->uClearText=0;
		structExtParam->uProfileName=0;
		structExtParam->cServer1[0]=0;
		structExtParam->cServer2[0]=0;
		structExtParam->cIP[0]=0;
		structExtParam->uISPClient=0;
		structExtParam->cClientName[0]=0;
		structExtParam->uParamClientName=0;

}//void InitializeParams(structExtJobParameters *structExtParam)


//TODO check this all out.		
void DeleteClient(structExtJobParameters *structExtParam)
{
	if(structExtParam->uISPClient<2) return;

	sprintf(gcQuery,"DELETE FROM tClient WHERE uClient=%u AND cFirstName='(uISPClient)'",structExtParam->uISPClient);
	macro_mySQLQueryErrorTextExit;

}//void DeleteClient(structExtJobParameters *structExtParam)


void AutoHoldRelease(char *cServer)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW field;
	unsigned uServer=0;
	time_t luClock;
	struct tm *structTm;
	char cTime[100];
	unsigned uNow=0;

	time(&luClock);
	structTm= localtime(&luClock);
	strftime(cTime,100,"%H%M",structTm);
	sscanf(cTime,"%u",&uNow);

	//debug only
	printf("AutoHoldRelease(%s) Start. Now it's %u hours\n\n",cServer,uNow);

	uServer=GetuServer(cServer);
	if(!uServer)
	{
		fprintf(stderr,"Must specify valid uServer: cServer=%s unknown\n",cServer);
		exit(1);
	}


	sprintf(gcQuery,"SELECT tUser.uUser,tUser.uProfileName,tUser.cLogin,tProfileName.cLabel,"
			"tProfile.uHourStart,tProfile.uHourEnd,tUser.uOnHold,tProfile.uProfile FROM"
			" tUser,tServerGroup,tProfileName,tProfile WHERE"
			" tUser.uUser=tServerGroup.uUser AND"
			" tUser.uProfileName=tProfileName.uProfileName AND"
			" tProfileName.uProfileName=tProfile.uProfileName AND"
			" tProfile.uClearText=tUser.uClearText AND"
			" tServerGroup.uServer=%u AND"
			" tProfile.uHourStart>0 AND"
			" tProfile.uHourEnd>0"
				,uServer);
	macro_mySQLQueryStoreErrorTextExit;
        while((field=mysql_fetch_row(mysqlRes)))
	{
		unsigned uUser=0;
		unsigned uHourStart=0;
		unsigned uHourEnd=0;
		unsigned uSameDay=0;
		unsigned uOnHold=0;

		//debug only
		printf("tUser.uUser=%s tUserProfileName=%s tUser.cLogin=%s tProfileName.cLabel=%s tProfile.uHourStart=%s tProfile.uHourEnd=%s tUser.uOnHold=%s tProfile.uProfile=%s\n",
				field[0],field[1],field[2],field[3],field[4],field[5],field[6],field[7]);

		sscanf(field[0],"%u",&uUser);
		sscanf(field[4],"%u",&uHourStart);
		sscanf(field[5],"%u",&uHourEnd);
		sscanf(field[6],"%u",&uOnHold);

		//Ignore rule and day rules, different day implicit
		if(uHourStart==uHourEnd)
			continue;
		else if(uHourStart<uHourEnd)
			uSameDay=1;

		//TODO For one minute we have the unforseen logic condition is this due to
		//	the lack of <= or >= to include that not included in a rule minute.
		//	Must apply formal math to fix. Will do ASAP.
		//TODO we assume that the uOnHold=0 state corresponds with current production users file.
		//	This may not be a good idea discuss in group meeting.
		if(!uSameDay)
		{
			//Logical complent: Same as considering uHourEnd the start and uHourEnd the end of an exclusion
			//period during the same day. This way we can use the 2400 hour system and keep things simple.
			if( uNow>uHourEnd && uNow<=uHourStart && uOnHold)
				printf("\toff-hours but user already on-hold no action\n");
			else if( uNow>uHourEnd && uNow<=uHourStart && !uOnHold)
				PlaceOnHold(uUser,uServer);
			else if( (uNow<uHourEnd || uNow>=uHourStart) && uOnHold)
				ReleaseFromHold(uUser,uServer);
			else if(!uOnHold)
				printf("\tallowed hours but user already off hold take no action\n");
			else if(1)
				printf("\tunforseen logic condition-1\n");
		}
		else
		{
			if( (uNow<=uHourStart || uNow>uHourEnd ) && !uOnHold)
				PlaceOnHold(uUser,uServer);
			else if( (uNow<=uHourStart || uNow>uHourEnd) && uOnHold)
				printf("\toff-hours but user already on-hold take no action\n");
			else if(uNow>=uHourStart && uNow<uHourEnd && !uOnHold)
				printf("\tallowed hours but user already off hold take no action\n");
			else if(uOnHold)
				ReleaseFromHold(uUser,uServer);
			else if(1)
				printf("\tunforseen logic condition-2\n");
		}
		
	}
	mysql_free_result(mysqlRes);
	

	//debug only
	printf("\nAutoHoldRelease() End\n");
	exit(0);

}//void AutoHoldRelease(char *cServer)


void ReleaseFromHold(const unsigned uUser, const unsigned uServer)
{
	//debug only
	printf("\tallowed hours release from hold\n");

	sprintf(gcQuery,"UPDATE tUser SET uOnHold=0 WHERE uUser=%u",uUser);
	macro_mySQLQueryErrorTextExit;

	//Make complete users
	MakeUsersFile(0,uServer,"");
	//Reload radiusd
	ReloadRadius(0,uServer,0);

}//void PlaceOnHold()


void PlaceOnHold(const unsigned uUser, const unsigned uServer)
{
	//debug only
	printf("\toff-hours place on-hold\n");

	sprintf(gcQuery,"UPDATE tUser SET uOnHold=1 WHERE uUser=%u",uUser);
	macro_mySQLQueryErrorTextExit;

	//Make complete users
	MakeUsersFile(0,uServer,"");
	//Reload radiusd
	ReloadRadius(0,uServer,0);

}//void PlaceOnHold()

// vim:tw=78
