/*
FILE
	apache.c
	svn ID removed
PURPOSE
	Mostly command line functions and common functions that are used in more than
	one tXfunc.h file.
AUTHOR/LEGAL
	(C) 2006-2009 Gary Wallis and Hugo Urquiza.
*/


#include "mysqlrad.h"
#include <shadow.h>
#include <grp.h>
#include "apache.h"
#include <openisp/template.h>

void ProcessJobQueue(char *cServer);
int NewSiteJob(unsigned uJob,unsigned uSite);
int NewSiteUserJob(unsigned uJob,unsigned uSite);
int ChangeSystemPasswd(char *cLogin, char *cPasswd);
void GetConfiguration(const char *cName, char *cValue, unsigned uServer, unsigned uHtml);
unsigned GetuServer(const char *cLabel);
void CrontabConnectDb(void);
void UpdateJobStatus(unsigned uJob,unsigned uJobStatus);
void UpdateSiteStatus(unsigned uSite,unsigned uStatus);
void UpdateSiteUserStatus(unsigned uSiteUser,unsigned uStatus);
void TextError(const char *cError, unsigned uContinue);
int MakeAndCheckConfFiles(const char *cServer);
int CheckConfFiles(void);

//These are all local server OPs
int RestartHTTPDaemons(void);
int CommandLineRestart(void);
int CommandLineCheckConfFiles(void);

int SitePerms(char *cMainSiteUser, char *cWebSiteDir, char *cDomain);
int ModSiteJob(unsigned uJob, unsigned uSite);
int ModSiteUserJob(unsigned uJob,unsigned uSiteUser);
int DelSiteJob(unsigned uJob, unsigned uSite);
int HoldSite(unsigned uSite,unsigned uServer);
int RemoveHold(unsigned uSite,unsigned uServer);

void ExtConnectDb(unsigned uHtml);
void ProcessExtJobQueue(char *cServer);
unsigned uGetClientOwner(unsigned uClient);
unsigned uGetSiteUser(char *cUser);

void ParseExtParams(structExtJobParameters *structExtParam, char *cJobData)
{
	char *cp,*cp2;

	//tSite
	if((cp=strstr(cJobData,"Domain=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+7,'\n'))) *cp2=0;
		sprintf(structExtParam->cDomain,"%.99s",cp+7);
		if(cp2) *cp2='\n';
		structExtParam->uParamDomain=1;
	}
	if((cp=strstr(cJobData,"IPBasedRootSite=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+16,'\n'))) *cp2=0;
		//Allow use of this parameter as alternative to NameBased
		if(strncmp(cp+16,"Default",7))
		{
			sprintf(structExtParam->cIPBasedRootSite,"%.99s",cp+16);
			structExtParam->uParamIPBasedRootSite=1;
			structExtParam->uNameBased=0; //testing
		}
		else
		{
			structExtParam->uNameBased=1;
			//Not sure about this. But not used yet anyway.
			structExtParam->uParamNameBased=1;
		}
		if(cp2) *cp2='\n';

	}
	if((cp=strstr(cJobData,"Server=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+7,'\n'))) *cp2=0;
		sprintf(structExtParam->cServer,"%.32s",cp+7);
		if(cp2) *cp2='\n';
		structExtParam->uParamServer=1;
	}
	if((cp=strstr(cJobData,"ParkedDomains=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+14,'\n'))) *cp2=0;
		sprintf(structExtParam->cParkedDomains,"%.1023s",cp+14);
		if(cp2) *cp2='\n';
		structExtParam->uParamParkedDomains=1;
	}
	if((cp=strstr(cJobData,"NameBased=")))
	{
		cp+=10;
		if( *cp=='Y' || *cp=='y') structExtParam->uNameBased=1;
		structExtParam->uParamNameBased=1;
	}
	if((cp=strstr(cJobData,"FrontPage=")))
	{
		cp+=10;
		if( *cp=='Y' || *cp=='y') structExtParam->uFrontPage=1;
		structExtParam->uParamFrontPage=1;
	}
	if((cp=strstr(cJobData,"Webalizer=")))
	{
		cp+=10;
		if( *cp=='Y' || *cp=='y') structExtParam->uWebalizer=1;
		structExtParam->uParamWebalizer=1;
	}
	if((cp=strstr(cJobData,"MySQL=")))
	{
		cp+=6;
		if( *cp=='Y' || *cp=='y') structExtParam->uMySQL=1;
		structExtParam->uParamMySQL=1;
	}
	if((cp=strstr(cJobData,"HDQuota=")))
	{
		sscanf(cp+8,"%u",&structExtParam->uHDQuota);
		structExtParam->uParamHDQuota=1;
	}
	if((cp=strstr(cJobData,"TrafficQuota=")))
	{
		sscanf(cp+13,"%u",&structExtParam->uTrafficQuota);
		structExtParam->uParamTrafficQuota=1;
	}
	if((cp=strstr(cJobData,"MaxUsers=")))
	{
		sscanf(cp+9,"%u",&structExtParam->uMaxUsers);
		structExtParam->uParamMaxUsers=1;
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
	

	//tSiteUser	
	if((cp=strstr(cJobData,"Passwd=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+7,'\n'))) *cp2=0;
		sprintf(structExtParam->cPasswd,"%.64s",cp+7);
		if(cp2) *cp2='\n';
		structExtParam->uParamPasswd=1;
	}
	if((cp=strstr(cJobData,"Login=")))
	{
		cp2=NULL;
		if((cp2=strchr(cp+6,'\n'))) *cp2=0;
		sprintf(structExtParam->cLogin,"%.63s",cp+6);
		if(cp2) *cp2='\n';
		structExtParam->uParamLogin=1;
	}

	//Not implemented yet
	//Supposedly we could add more users to a site with more
	//mysqlISP product instances that require a valid VH first
	//with the initial admin user
	if((cp=strstr(cJobData,"ClearText=")))
	{
		cp+=10;
		if( *cp=='Y' || *cp=='y') structExtParam->uClearText=1;
	}
	else if(strstr(cJobData,"ClearPasswd="))
	{
		structExtParam->uClearText=1;
	}
	if((cp=strstr(cJobData,"Admin=")))
	{
		cp+=6;
		if( *cp=='Y' || *cp=='y') structExtParam->uAdmin=1;
	}
	if((cp=strstr(cJobData,"SSH=")))
	{
		cp+=4;
		if( *cp=='Y' || *cp=='y') structExtParam->uSSH=1;
	}
	if((cp=strstr(cJobData,"FTP=")))
	{
		cp+=4;
		if( *cp=='Y' || *cp=='y') structExtParam->uFTP=1;
	}
	if((cp=strstr(cJobData,"Pop3=")))
	{
		cp+=5;
		if( *cp=='Y' || *cp=='y') structExtParam->uPop3=1;
	}
	if((cp=strstr(cJobData,"SMTP=")))
	{
		cp+=5;
		if( *cp=='Y' || *cp=='y') structExtParam->uSMTP=1;
	}
	
	//Debug only
	printf("cDomain=%s cLogin=%s cPasswd=%s uHDQuota=%u\n",
			structExtParam->cDomain,
			structExtParam->cLogin,
			structExtParam->cPasswd,
			structExtParam->uHDQuota);

}//void ParseExtParams(structExtJobParameters *structExtParam)


void InitializeParams(structExtJobParameters *structExtParam)
{
	static unsigned uOnlyOnce=1;
	static char cValue[256];
	static unsigned uHDQuota;
	static unsigned uTrafficQuota;
	static unsigned uMaxUsers;
	static unsigned uWebalizer;
	static unsigned uFrontPage;
	static unsigned uMySQL;
	static unsigned uMaxSites;
	static unsigned uMaxIPs;
	static char cIPBasedRootSite[100];

	if(uOnlyOnce)
	{
		//Load presets
		strcpy(cValue,"20");
		//Temporarily commented the GetConfiguration() lines, the new version
		//of this function uses the uServer value.
		//
		//GetConfiguration("cDefaultHDQuota",cValue,1);
		sscanf(cValue,"%u",&uHDQuota);

		strcpy(cValue,"30");
		//GetConfiguration("cDefaultTrafficQuota",cValue,1);
		sscanf(cValue,"%u",&uTrafficQuota);

		strcpy(cValue,"2");
		//GetConfiguration("cDefaultMaxUsers",cValue,1);
		sscanf(cValue,"%u",&uMaxUsers);

		strcpy(cValue,"No");
		//GetConfiguration("cDefaultWebalizer",cValue,1);
		if(cValue[0]=='y' || cValue[0]=='Y') uWebalizer=1;

		strcpy(cValue,"No");
		//GetConfiguration("cDefaultFrontPage",cValue,1);
		if(cValue[0]=='y' || cValue[0]=='Y') uFrontPage=1;

		strcpy(cValue,"No");
		//GetConfiguration("cDefaultMySQL",cValue,1);
		if(cValue[0]=='y' || cValue[0]=='Y') uMySQL=1;

		strcpy(cValue,"2");
		//GetConfiguration("cDefaultMaxSites",cValue,1);
		sscanf(cValue,"%u",&uMaxSites);

		strcpy(cValue,"2");
		//GetConfiguration("cDefaultMaxIPs",cValue,1);
		sscanf(cValue,"%u",&uMaxIPs);

		//Multiple website hosting servers running off a single unxsApache.
		//Note: tConfiguration: Must be in dummy staging area the same domain for all
		//servers
		//GetConfiguration("cIPBasedRootSite",cIPBasedRootSite,1);
		
		uOnlyOnce=0;
	}

	structExtParam->cDomain[0]=0;
	sprintf(structExtParam->cIPBasedRootSite,"%.99s",cIPBasedRootSite);
	strcpy(structExtParam->cServer,"localhost");
	structExtParam->uServer=1;
	structExtParam->cParkedDomains[0]=0;
	structExtParam->uNameBased=1;
	//Loaded via GetConfiguration()
	structExtParam->uHDQuota=uHDQuota;
	structExtParam->uTrafficQuota=uTrafficQuota;
	structExtParam->uFrontPage=uFrontPage;
	structExtParam->uWebalizer=uWebalizer;
	structExtParam->uMySQL=uMySQL;
	structExtParam->uMaxUsers=uMaxUsers;

	structExtParam->uISPClient=0;
	structExtParam->cClientName[0]=0;
	//Loaded via GetConfiguration()
	structExtParam->uMaxSites=uMaxSites;
	structExtParam->uMaxIPs=uMaxIPs;

	structExtParam->cLogin[0]=0;
	structExtParam->cPasswd[0]=0;
	structExtParam->uClearText=0;
	structExtParam->uAdmin=1;
	structExtParam->uSSH=1;
	structExtParam->uFTP=1;
	structExtParam->uPop3=0;
	structExtParam->uSMTP=0;

	//Presence data
	structExtParam->uParamMaxUsers=0;
	structExtParam->uParamTrafficQuota=0;
	structExtParam->uParamHDQuota=0;
	structExtParam->uParamMySQL=0;
	structExtParam->uParamWebalizer=0;
	structExtParam->uParamFrontPage=0;
	structExtParam->uParamNameBased=0;
	structExtParam->uParamParkedDomains=0;
	structExtParam->uParamServer=0;
	structExtParam->uParamIPBasedRootSite=0;
	structExtParam->uParamDomain=0;

	structExtParam->uParamClientName=0;

	structExtParam->uParamLogin=0;
	structExtParam->uParamPasswd=0;

}//void InitializeParams(structExtJobParameters *structExtParam)


int InformExtJob(char *cRemoteMsg,char *cServer,unsigned uJob,unsigned uJobStatus)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uJobGroup=0;
	unsigned uInstance=0;
	time_t clock;

	time(&clock);

	sprintf(gcQuery,"UPDATE tJob SET cServer='%s',cRemoteMsg='%.32s',uModBy=1,uModDate=UNIX_TIMESTAMP_NOW(),uJobStatus=%u WHERE uJob=%u",
			cServer,TextAreaSave(cRemoteMsg),uJobStatus,uJob);
	printf("InformExtJob(): %s\n",gcQuery);
	mysql_query(&mysqlext,gcQuery);
	if(mysql_errno(&mysqlext))
	{
		fprintf(stderr,"InformExtJob():%s\n%s\n",mysql_error(&mysqlext),gcQuery);
		SubmitISPJob("unxsApache.InformExtJob.Failed",
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
	printf("InformExtJob(): %s\n",gcQuery);
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
	printf("InformExtJob(): %s\n",gcQuery);
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

				sprintf(gcQuery,"UPDATE tInstance SET uStatus=%u,uModBy=1,uModDate=UNIX_TIMESTAMP_NOW() WHERE uInstance=%u",
					uInstanceStatus,uInstance);
				printf("InformExtJob(): %s\n",gcQuery);
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
	
	sprintf(gcQuery,"INSERT INTO tJob SET  cServer='%s', cJobName='%s', cJobData='%.1024s',"
			" uJobDate=%u, uOwner=1, uCreatedBy=1, uCreatedDate=%lu, uJobStatus=%u,"
			"cLabel='unxsApache.SubmitISPJob'",
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

}//int SubmitISPJob()


int SubmitExtSingleJob(const char *cJobName,char *cJobData,const char *cServer,unsigned uJobDate,
			unsigned uJob, unsigned uJobSite,const char *cResource,unsigned uOwner)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE cJobName='%s' AND cJobData='%s' AND cServer='%s'"
			" AND uJobSite=%u",cJobName,cJobData,cServer,uJobSite);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	
	if(mysql_num_rows(res)==0)
	{
		time_t clock;

		time(&clock);
	
		sprintf(gcQuery,"INSERT INTO tJob SET cLabel='%s %s',cServer='%s',cJobName='%s',cJobData='%.1024s\nmysqlISP2.tJob.uJob=%u',"
				"uJobDate=%u,uOwner=%u,uCreatedBy=1,uCreatedDate=%lu,uJobSite=%u,uJobStatus=1",
				cJobName
				,cResource
				,cServer
				,cJobName
				,TextAreaSave(cJobData)
				,uJob
				,uJobDate
				,uOwner
				,clock
				,uJobSite);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

	return(0);

}//int SubmitExtSingleJob()


int SetupExtSiteUser(char *cLogin,char *cPasswd, unsigned uISPClient)
{
	time_t clock;
	unsigned uRetVal=0;
	unsigned uOwner=0;

	time(&clock);
	if(!(uOwner=uGetClientOwner(uISPClient))) uOwner=1; //Safe default. Should inform? How?

	sprintf(gcQuery,"INSERT INTO tSiteUser SET  uSite=0,cLogin='%s',cPasswd='%s',uAdmin=1, uSSH=1,uFTP=1,uOwner=%u,uCreatedBy=1,"
			"uCreatedDate=%lu, uStatus=%u",
			cLogin
			,cPasswd
			,uOwner
			,clock
			,STATUS_PENDING);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(!strncmp(mysql_error(&gMysql),"Duplicate entry",14))
			uRetVal=2;
		else
			uRetVal=3;
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
	}
	
	return(uRetVal);

}//int SetupExtSiteUser()


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
	MYSQL_RES *res;
	MYSQL_ROW field;
	MYSQL_RES *res2;
	MYSQL_ROW field2;

	unsigned uTargetServer=0;//target server for final local jobqueue


	structExtJobParameters structExtParam;

	unsigned uJob=0;
	unsigned uJobClient=0;
	unsigned uOwner=0;
	unsigned uSite=0;

	time_t clock;
		
	ExtConnectDb(0);

	time(&clock);


	sprintf(gcQuery,"SELECT cJobName,cJobData,uJob,uJobClient FROM tJob WHERE (cServer='Any'"
			" OR cServer='%s') AND (uJobStatus=%u OR uJobStatus=%u "
			"OR uJobStatus=%u) AND uJobDate<=%lu AND cJobName LIKE 'unxsApache.%%'",
			cServer
			,9
			,10
			,11 //Will replace for macros after testing!
			,clock);

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

		//mysqlISP provides new website domain name, one admin login and
		//password. Also provides other service params and the mysqlISP 
		//tClient.uClient generally referred to system wide as the uISPClient
		//unique customer ID ISP wide in scope.
		//Here we need to create a new tClient if needed.
		//Need to create new tSiteUser and tProFTP entries
		//Then the tSite entries
		if(!strcmp("unxsApache.VH.New",field[0]))
		{
			unsigned uCurrentIPSites=0;
			unsigned uCurrentSites=0;
			struct t_template template;
			char cVirtualHost2[4096];
			char cVirtualHost3[4096];
			char *cVirtualHost;
			char cIP[16]={""};
			char cCondListen[64]={""};
			char cDomainEscDot[100]={""};
			char cWebRoot[256]={"/var/local/web"};
			char cCGI[256]={"cgi-bin"};
			char cSSLCertFile[256]={"/var/local/web/myhostingco.com/conf/www.myhostingco.com.pem"};
			char cWebmaster[256]={"webmaster"};

			printf("\n%s(%s):\n",field[0],field[2]);
		
			ParseExtParams(&structExtParam,field[1]);

			if(!structExtParam.uParamServer || !structExtParam.cServer[0])
			{
				fprintf(stderr,"We now require 'Server' parameter\n");
				InformExtJob("Need Server",cServer,uJob,
						mysqlISP_Waiting);
				continue;
			}

			uTargetServer=GetuServer(structExtParam.cServer);
			//Target server can diff than ProcessExtJobQueue server
			//The local jobqueue has to be the same!


			//Check everything before commiting to new site
			//0-. Site can't already exist. Admin user can't already exist
			//1-. Client limits
			//2-. cIP stuff
			//3-. Admin account stuff
			//
			sprintf(gcQuery,"SELECT uSite FROM tSite WHERE cDomain='%s' AND uServer=%u",
				structExtParam.cDomain,uTargetServer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("Select-1 failed",cServer,uJob,
						mysqlISP_Waiting);
				mysql_free_result(res2);
				continue;
			}
			res2=mysql_store_result(&gMysql);
			if(mysql_num_rows(res2))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("cDomain exists!",cServer,uJob,
						mysqlISP_Waiting);
				mysql_free_result(res2);
				continue;
			}
			mysql_free_result(res2);

			sprintf(gcQuery,"SELECT tSiteUser.uSiteUser FROM tSiteUser,tSite WHERE "
					"tSiteUser.cLogin='%s' AND tSiteUser.uSite=tSite.uSite AND tSite.uServer=%u",
					structExtParam.cLogin,uTargetServer);

			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("Select-2 failed",cServer,uJob,
						mysqlISP_Waiting);
				continue;
			}
			res2=mysql_store_result(&gMysql);
			if(mysql_num_rows(res2))
        		{
				sprintf(gcQuery,"cLogin exists!");
				fprintf(stderr,"%s\n",gcQuery);
				InformExtJob(gcQuery,cServer,uJob,
						mysqlISP_Waiting);
				mysql_free_result(res2);
				continue;
			}
			mysql_free_result(res2);
		
			//Create new tClient if needed
			time(&clock);
			/*if(!structExtParam.uISPClient)
				structExtParam.uISPClient=uJobClient;
			CreateNewClient(&structExtParam);
			*/
			//Enforce per client limits non root sites
			//Should sync check via mysqlISP also on Ext tJob creation
			GetCurrentIPSites(structExtParam.uISPClient,
							&uCurrentIPSites,0,uTargetServer);
			GetCurrentSites(structExtParam.uISPClient,&uCurrentSites,0,uTargetServer);

			//Get cIP
			if(!structExtParam.uNameBased)
			{
				//IP Based Site
				if(!IPAvailable(0))
        			{
					fprintf(stderr,"No IPs available\n");
					InformExtJob("No IPs available",
							cServer,uJob,mysqlISP_Waiting);
					continue;
				}
				GetIPVirtualHost("",cIP,uTargetServer);//This makes one IP not 
							//available
				if(!cIP[0])
        			{
					fprintf(stderr,"Could not get cIP ip based\n");
					InformExtJob("Could not get cIP ip based",
							cServer,uJob,mysqlISP_Waiting);
					continue;
				}
			}
			else
			{
				if(!structExtParam.cIPBasedRootSite[0])
        			{
					fprintf(stderr,"cIPBasedRootSite not set.\n");
					InformExtJob("cIPBasedRootSite not set",
							cServer,uJob,mysqlISP_Waiting);
					continue;
				}

				//Special requirements for multiple server 
				//based unxsApache.system
				GetIPVirtualHost(structExtParam.cIPBasedRootSite,
						cIP,uTargetServer);
				if(!cIP[0])
       				{
					fprintf(stderr,"Could not get cIP name based\n");
					InformExtJob("Could not get cIP name based",
						cServer,uJob,mysqlISP_Waiting);
					continue;
				}
			}
			//debug only
			//printf("%s\n",cIP);

			//Prepare for tSite entry
			//Initial record values. Must update locally determined
			//fields later
			sprintf(structExtParam.cParkedDomains,"ServerAlias www.%.64s",
							structExtParam.cDomain);
			
			//The template for site config setup is always the same
			//The difference between name based and IP based sites
			//is at the {{cCondListen}} template variable.

			GetTemplate("StandardPort80VirtualHost",cVirtualHost2,0);
			
			if(!cVirtualHost2[0])
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("Could not get VH template",
						cServer,uJob,mysqlISP_Waiting);
				continue;
			}

			//From tsitefunc.h code
			
			GetConfiguration("cWebRoot",cWebRoot,0,1);
			GetConfiguration("cCGI",cCGI,0,1);
			GetConfiguration("cWebmaster",cWebmaster,0,1);
			EscapeDots(structExtParam.cDomain,cDomainEscDot,100);
			
			//Temporarily commented
		        //if(!structExtParam.uNameBased)
			//	sprintf(cSSLCertFile,"%.120s/%.64s/conf/%.64s.pem",
		        //					cWebRoot,cDomain,cSSLCert);

			template.cpName[0]="cDomain";
			template.cpValue[0]=structExtParam.cDomain;

			template.cpName[1]="cDomainEscDot";
			template.cpValue[1]=cDomainEscDot;

			template.cpName[2]="cIP";
			template.cpValue[2]=cIP;

			template.cpName[3]="cWebRoot";
			template.cpValue[3]=cWebRoot;

			template.cpName[4]="cCGI";
			template.cpValue[4]=cCGI;

			template.cpName[5]="cWebmaster";
			template.cpValue[5]=cWebmaster;

			if(!structExtParam.uNameBased)
			{
				sprintf(cCondListen,"NameVirtualHost %.15s:80\nListen %.15s:80",
						cIP,cIP);
				template.cpName[6]="cCondListen";
				template.cpValue[6]=cCondListen;

				template.cpName[7]="cSSLCertFile";
				template.cpValue[7]=cSSLCertFile;

				template.cpName[8]="";
			}
			else
			{
				template.cpName[6]="cParkedDomains";
				template.cpValue[6]=structExtParam.cParkedDomains;
			
				template.cpName[7]="";
			}

			TemplateToBuffer(cVirtualHost2,&template,
					cVirtualHost3);
			cVirtualHost=cVirtualHost3;
			
			if(strlen(structExtParam.cParkedDomains)+
					strlen(cVirtualHost)> (8191-500))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("Out of template buffer space",
						cServer,uJob,mysqlISP_Waiting);
				continue;
			}
			//debug only
			//printf("%s\n",cVirtualHost);
			
			sprintf(gcQuery,"INSERT INTO tSite SET  cDomain='%s',uNameBased=%u,uServer=%u,cParkedDomains='%s',"
					"cVirtualHost='%s',uHDQuota=%u,uTrafficQuota=%u,uWebalizer=%u,uMySQL=%u,uMaxUsers=%u,"
					"uClient=%u,uOwner=%u,uCreatedBy=1,uStatus=%u,uCreatedDate=%lu, cIP='%s'"
					,structExtParam.cDomain
					,structExtParam.uNameBased
					,uTargetServer
					,structExtParam.cParkedDomains
					,TextAreaSave(cVirtualHost3)
					,structExtParam.uHDQuota
					,structExtParam.uTrafficQuota
					,structExtParam.uWebalizer
					,structExtParam.uMySQL
					,structExtParam.uMaxUsers
					,uJobClient
					,uOwner
					,STATUS_PENDING
					,clock
					,cIP);
			//Debug only	
			//printf("%s\n",gcQuery);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("tSite Insert failed",cServer,uJob,
						mysqlISP_Waiting);
				continue;
			}

			//Attach admin login
			uSite=mysql_insert_id(&gMysql);

			sprintf(gcQuery,"INSERT INTO tSiteUser SET  uSite=%u,cLogin='%s',cPasswd='%s',uAdmin=1, uSSH=1,uFTP=1,uOwner=%u,uCreatedBy=1,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW()), uStatus=%u",
					uSite
					,structExtParam.cLogin
					,structExtParam.cPasswd
					,uOwner
					,STATUS_PENDING);
			
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("Update error-1",
						cServer,uJob,mysqlISP_Waiting);
				continue;
			}
			//Admin User must be first
			SubmitExtSingleJob("ExtNewSite",structExtParam.cDomain,structExtParam.cServer,clock,uJob,uSite,structExtParam.cDomain,uOwner);
			SubmitExtSingleJob("ExtNewSiteUser",structExtParam.cLogin,structExtParam.cServer,clock,uJob,uSite,structExtParam.cLogin,uOwner);
			
			InformExtJob("unxsApache.ExtNewSite",cServer,uJob,mysqlISP_RemotelyQueued);

		}//VH.New
		//Wow that is long....

		else if(!strcmp("unxsApache.VH.Cancel",field[0]))
		{
			printf("\n%s(%s):\n",field[0],field[2]);
			ParseExtParams(&structExtParam,field[1]);

			if(!structExtParam.uParamServer || !structExtParam.cServer[0])
			{
				fprintf(stderr,"We now require 'Server' parameter\n");
				InformExtJob("Need Server",cServer,uJob,
						mysqlISP_Waiting);
				continue;
			}
			uTargetServer=GetuServer(structExtParam.cServer);

			//Allow cancel of any status site
			sprintf(gcQuery,"SELECT uSite FROM tSite WHERE uServer=%u AND cDomain='%s'",
					uTargetServer,structExtParam.cDomain);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("Select query VH.Cancel failed",
						cServer,uJob,mysqlISP_Waiting);
			}
			else
			{
        			res2=mysql_store_result(&gMysql);
        			if((field2=mysql_fetch_row(res2)))
				{
					sscanf(field2[0],"%u",&uSite);
					
					HandleOtherPrevQueuedJobs(uSite,
									cServer);

					SubmitExtSingleJob("ExtDelSite",
						structExtParam.cLogin,structExtParam.cServer,
							clock,uJob,uSite,structExtParam.cLogin,uOwner);
					InformExtJob("unxsApache.ExtDelSite",
						cServer,uJob,mysqlISP_RemotelyQueued);
				}
				else
				{
					InformExtJob("No such site for cancel",
						cServer,uJob,mysqlISP_Waiting);
				}
				mysql_free_result(res2);
			}
		}//VH.Cancel

		else if(!strcmp("unxsApache.VH.Mod",field[0]))
		{	
			printf("\n%s(%s):\n",field[0],field[2]);
			ParseExtParams(&structExtParam,field[1]);
			if(!structExtParam.uParamServer || !structExtParam.cServer[0])
			{
				fprintf(stderr,"We now require 'Server' parameter\n");
				InformExtJob("Need Server",cServer,uJob,
						mysqlISP_Waiting);
				continue;
			}
			uTargetServer=GetuServer(structExtParam.cServer);

			sprintf(gcQuery,"SELECT uSite FROM tSite WHERE uServer=%u AND cDomain='%s'",
			uTargetServer,structExtParam.cDomain);

			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("Select query VH.Cancel failed",
						cServer,uJob,mysqlISP_Waiting);
			}
			else
			{
        			res2=mysql_store_result(&gMysql);
        			if((field2=mysql_fetch_row(res2)))
				{
					sscanf(field2[0],"%u",&uSite);

					//Queues one or more jobs for each
					//modification needed.
					if(!ExtModify(&structExtParam,
							uSite,uJob,cServer))
					InformExtJob("unxsApache.ExtModify",
						cServer,uJob,mysqlISP_RemotelyQueued);
					else
					InformExtJob("unxsApache.ExtModify error",
						cServer,uJob,mysqlISP_Waiting);
				}
				else
				{
					InformExtJob("No such site for mod",
						cServer,uJob,mysqlISP_Waiting);
				}
				mysql_free_result(res2);
			}
		}//VH.Mod

		else if(!strcmp("unxsApache.VH.Hold",field[0]))
		{	
			printf("\n%s(%s):\n",field[0],field[2]);
			ParseExtParams(&structExtParam,field[1]);
			if(!structExtParam.uParamServer || !structExtParam.cServer[0])
			{
				fprintf(stderr,"We now require 'Server' parameter\n");
				InformExtJob("Need Server",cServer,uJob,
						mysqlISP_Waiting);
				continue;
			}
			uTargetServer=GetuServer(structExtParam.cServer);

			//Check everything before commiting to new site
			sprintf(gcQuery,"SELECT uSite FROM tSite WHERE uServer=%u AND cDomain='%s'",
					uTargetServer,structExtParam.cDomain);
			printf("%s\n",gcQuery);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("Select query VH.Hold failed",
						cServer,uJob,mysqlISP_Waiting);
			}
			else
			{
        			res2=mysql_store_result(&gMysql);
        			if((field2=mysql_fetch_row(res2)))
				{
					sscanf(field2[0],"%u",&uSite);

					HandleOtherPrevQueuedJobs(uSite,
									cServer);

					SubmitExtSingleJob("ExtHoldSite",
						structExtParam.cLogin,structExtParam.cServer,
							clock,uJob,uSite,structExtParam.cLogin,uOwner);
					InformExtJob("unxsApache.ExtHoldSite",
						cServer,uJob,mysqlISP_RemotelyQueued);
				}
				else
				{
					InformExtJob("No such site for hold",
						cServer,uJob,mysqlISP_Waiting);
				}
				mysql_free_result(res2);
			}

		}//VH.Hold

		else if(!strcmp("unxsApache.VH.RemoveHold",field[0]))
		{	
			printf("\n%s(%s):\n",field[0],field[2]);
			ParseExtParams(&structExtParam,field[1]);
			if(!structExtParam.uParamServer || !structExtParam.cServer[0])
			{
				fprintf(stderr,"We now require 'Server' parameter\n");
				InformExtJob("Need Server",cServer,uJob,
						mysqlISP_Waiting);
				continue;
			}
			uTargetServer=GetuServer(structExtParam.cServer);

			sprintf(gcQuery,"SELECT uSite FROM tSite WHERE uServer=%u AND"
				" cDomain='%s' AND uStatus=%u",
				uTargetServer,
				structExtParam.cDomain,
				STATUS_HOLD);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
        		{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				InformExtJob("Select query VH.RemoveHold failed",
						cServer,uJob,mysqlISP_Waiting);
			}
			else
			{
        			res2=mysql_store_result(&gMysql);
        			if((field2=mysql_fetch_row(res2)))
				{
					sscanf(field2[0],"%u",&uSite);

					HandleOtherPrevQueuedJobs(uSite,
									cServer);

					SubmitExtSingleJob("ExtRemoveHold",
						structExtParam.cLogin,structExtParam.cServer,
							clock,uJob,uSite,structExtParam.cLogin,uOwner);
					InformExtJob("unxsApache.ExtRemoveHold",
						cServer,uJob,mysqlISP_RemotelyQueued);
				}
				else
				{
					InformExtJob("No such site for removing hold",
						cServer,uJob,mysqlISP_Waiting);
				}
				mysql_free_result(res2);
			}

		}//VH.RemoveHold

		else if(1)
		{
			fprintf(stderr,"Unknown job:%s\n",field[0]);
		}
	}
	mysql_free_result(res);

}//ProcessExtJobQueue()


void HandleOtherPrevQueuedJobs(unsigned uSite,char *cServer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char *cp;

	//When a previous hold or cancel job has been queued locally
	//we must clean up.
	//Cases:
	//1-. Hold job then we get a cancel job: 'Cancel' the hold job locally and
	//remotely
	//2-. Cancel job then we get a hold job: 'Cancel' the cancel job locally and
	//remotely
	//Generalizing: All we need to do is cancel previous jobs for the same
	//site. This is not meant for mod jobs however.

	//We need the list of all local waiting jobs
	sprintf(gcQuery,"SELECT uJob,cJobData FROM tJob WHERE uJobSite=%u AND "
			"uJobStatus=%u AND (cJobName='ExtHoldSite' OR cJobName='ExtDelSite')",
			uSite,JOBSTATUS_WAITING);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
               	return;
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{

		//Cancel local job
		sprintf(gcQuery,"UPDATE tJob SET uJobStatus=%u WHERE uJob=%s",JOBSTATUS_REDUNDANT,field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			fprintf(stderr,"%s\n",mysql_error(&gMysql));

		//Cancel ext job
		if((cp=strstr(field[1],"uJob=")))
		{
			char cMsg[33];
			unsigned uExtJob=0;

			sprintf(cMsg,"unxsApache.extra uJob=%s",field[0]);
			sscanf(cp+5,"%u",&uExtJob);
			InformExtJob(cMsg,cServer,uExtJob,mysqlISP_Canceled);
		}
	
	}
	mysql_free_result(res);

}//void HandleOtherPrevQueuedJobs(unsigned uSite,char *cServer)


int ExtModify(structExtJobParameters *structExtParam, unsigned uSite,
		unsigned uJob, char *cServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	time_t clock;
	unsigned uPD=0,uFP=0,uWZ=0,uSQ=0;
	unsigned uPreFrontPage=0;
	unsigned uPreWebalizer=0;
	unsigned uPreMySQL=0;
	char cMsg[100]={"ExtModify() modified"};
	char cIP[16]={""};
	char cDomain[100]={""};
	unsigned uOwner=0;

        time(&clock);

	if(structExtParam->uParamLogin && structExtParam->uParamPasswd)
	{
		//Find out what changed for admin account.
		sprintf(gcQuery,"SELECT cPasswd,uOwner FROM tSiteUser WHERE uSite=%u AND cLogin='%s'"
					,uSite,structExtParam->cLogin);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			return(1);
		}
        	res=mysql_store_result(&gMysql);
        	if((field=mysql_fetch_row(res)))
		{
			sscanf(field[1],"%u",&uOwner);

			if(strcmp(structExtParam->cPasswd,field[0]))
			{
				sprintf(gcQuery,"UPDATE tSiteUser SET cPasswd='%s',uModBy=1,uModDate=UNIX_TIMESTAMP_NOW() WHERE uSite=%u AND cLogin='%s'",
						structExtParam->cPasswd,uSite,structExtParam->cLogin);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					fprintf(stderr,"%s\n",mysql_error(&gMysql));
					return(1);
				}
				SubmitExtSingleJob("ExtModSiteUser",
					structExtParam->cLogin,structExtParam->cServer,clock,uJob,
					uSite,structExtParam->cLogin,uOwner);
				//Debug only
				printf("Password change job submitted\n");
			}
		}
	}

	//Find out what changed for site. Also get fresh data just in case.
	sprintf(gcQuery,"SELECT cParkedDomains,uWebalizer,uMySQL,cDomain,cIP FROM tSite WHERE uSite=%u",uSite);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		return(1);
	}

        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		//
		//Compare but only if actually passed to us via cJobData
		
		sprintf(cDomain,"%.99s",field[4]);

		if(structExtParam->uParamParkedDomains)
		{
			if(strcmp(structExtParam->cParkedDomains,field[0]))
			{
				uPD=1;
				sprintf(cIP,"%.15s",field[5]);
			}
		}

		if(structExtParam->uParamFrontPage)
		{
			sscanf(field[1],"%u",&uPreFrontPage);
			if(uPreFrontPage!=structExtParam->uFrontPage) uFP=1;
		}

		if(structExtParam->uParamWebalizer)
		{
			sscanf(field[2],"%u",&uPreWebalizer);
			if(uPreWebalizer!=structExtParam->uWebalizer) uWZ=1;
		}

		if(structExtParam->uParamWebalizer)
		{
			sscanf(field[3],"%u",&uPreMySQL);
			if(uPreMySQL!=structExtParam->uMySQL) uSQ=1;
		}
	}
	else
	{
		fprintf(stderr,"Unexpected no record found uSite=%u\n",
					uSite);
		return(1);
	}

        mysql_free_result(res);

	
	if(uPD || uFP || uWZ || uSQ )
	{
		if(uPD)
		{
			sprintf(gcQuery,"UPDATE tSite SET uModDate=UNIX_TIMESTAMP_NOW(),uModBy=1 WHERE uSite=%u",
					uSite);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				return(1);
			}
			//This sets the new cParkedDomains
			ModifyParkedDomains(cIP,cDomain,structExtParam->cParkedDomains,
					uSite,0,uJob);
			strcat(cMsg," ParkedDomains");
		}
		if(uWZ)
		{
			sprintf(gcQuery,"UPDATE tSite SET uWebalizer=%u,uModDate=UNIX_TIMESTAMP_NOW(),"
					"uModBy=1 WHERE uSite=%u",structExtParam->uWebalizer,uSite);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				return(1);
			}
			ModifyWebalizer(structExtParam->uWebalizer,uPreWebalizer,
					uSite,cDomain,uJob);
			strcat(cMsg," Webalizer");
		}
		if(uSQ)
		{
			sprintf(gcQuery,"UPDATE tSite SET uMySQL=%u,uModDate=UNIX_TIMESTAMP_NOW(),"
					"uModBy=1 WHERE uSite=%u",structExtParam->uMySQL,uSite);

			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				return(1);
			}
			ModifyMySQL(structExtParam->uMySQL,uPreMySQL,
						uSite,cDomain,uJob);
			strcat(cMsg," MySQL");
		}

		//Debug only
		printf("%s\n",cMsg);
	}

	return(0);

}//int ExtModify()


void ModifyParkedDomains(char *cIP, char *cDomain, char *cParkedDomains, 
			unsigned uSite,unsigned uHtml,unsigned uExtJob)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	register int i;
	char *cp,*cp2;

	char cPreInsVirtualHost[4096]={""};
	char cPostInsVirtualHost[4096]={""};
	char cServerAliasSection[4096]={""};
	char cServerAliasLine[256];
	char *cVirtualHost="";
	unsigned uStatus=0;
	unsigned uServer=0;

	cp=cParkedDomains;
	for(i=0;cParkedDomains[i];i++)
	{
		if(cParkedDomains[i]=='\n' || !cParkedDomains[i+1])
		{
			if(cParkedDomains[i+1])
				cParkedDomains[i]=0;
			if(*cp)
			{
				sprintf(cServerAliasLine,"ServerAlias %.255s\n",cp);
				if((strlen(cServerAliasSection)+strlen(cServerAliasLine))<4095)
					strcat(cServerAliasSection,cServerAliasLine);
			}
			if(cParkedDomains[i+1])
				cParkedDomains[i]='\n';
			cp=cParkedDomains+i+1;
		}
	}
	
	sprintf(gcQuery,"SELECT cVirtualHost FROM tSite WHERE uSite=%u",uSite);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			return;
		}
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		//Save any pre ServerAlias part
		if((cp=strstr(field[0],"\nServerAlias")))
		{
			*cp=0;
			sprintf(cPreInsVirtualHost,"%.4095s\n",field[0]);
		}
	
		cp+=13;//strlen("\nServerAlias");	
		//All ServerAlias lines must be grouped else we wipe out other sections
		//Eliminate all ServerAlias lines
		while((cp2=strstr(cp,"\nServerAlias")))
		{
			cp=cp2+13;
		}
	
		//TODO 1.0
		//Please fix this section for the following non supported cases:
		//ServerAlias x.net
		//...
		//AddType text/html .shtml
		//...
		//ServerAlias y.net
		

		if((cp2=strchr(cp,'\n')))
			strncpy(cPostInsVirtualHost,cp2+1,4095);

		cVirtualHost=(char *)malloc(4096);
		sprintf(cVirtualHost,"%.4095s%.4095s%.4095s",
				cPreInsVirtualHost,cServerAliasSection,cPostInsVirtualHost);
		//For debug only
		//sprintf(cVirtualHost,"<pre>%.4095s%.4095s",cPreInsVirtualHost,
		//			cServerAliasSection);
	}
        mysql_free_result(res);
	
	//For debug only
	//htmlPlainTextError(cVirtualHost);


	sprintf(gcQuery,"UPDATE tSite SET cVirtualHost='%s' WHERE uSite=%u"
			,TextAreaSave(cVirtualHost)
			,uSite);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			return;
		}
	}

	uStatus=STATUS_PENDING_MOD;
	if(uExtJob)
	{
		time_t clock;
		char cServer[33]={"localhost"};//fix asap

		time(&clock);
		SubmitExtSingleJob("ExtModParkedDomains",cDomain,cServer,clock,uExtJob,
					uSite,cDomain,1); //Update uOwner dude!
	}
	else
		ScheduleJob(uSite,uServer,"ModParkedDomains",cDomain);

}//void ModifyParkedDomains()


void ModifyFrontPage(unsigned uFrontPage,unsigned uPreFrontPage,unsigned uSite,
		char *cDomain,unsigned uExtJob)
{
	time_t clock;
	char cServer[33]={"localhost"};//fix asap
	unsigned uStatus=0;
	unsigned uServer=1; //right ?
	
	time(&clock);
	uStatus=STATUS_PENDING_MOD;
	UpdateVHStatus(uSite,STATUS_PENDING_MOD);
	if(uPreFrontPage>uFrontPage)
	{
		if(uExtJob)
			SubmitExtSingleJob("ExtDisableFrontPage",cDomain,
						cServer,clock,uExtJob,uSite,cDomain,1);//Update uOwner dude!
		else
			ScheduleJob(uSite,uServer,"DisableFrontPage",cDomain);
	}
	else
	{
		if(uExtJob)
			SubmitExtSingleJob("ExtEnableFrontPage",cDomain,
						cServer,clock,uExtJob,uSite,cDomain,1);//Update uOwner dude!
		else
			ScheduleJob(uSite,uServer,"EnableFrontPage",cDomain);
	}

}//void ModifyFrontPage()


void ModifyWebalizer(unsigned uWebalizer,unsigned uPreWebalizer,unsigned uSite,
		char *cDomain,unsigned uExtJob)
{
	time_t clock;
	char cServer[33]={"localhost"};//fix asap
	unsigned uStatus=0;
	unsigned uServer=1; //right ?
	
	time(&clock);
	uStatus=STATUS_PENDING_MOD;
	UpdateVHStatus(uSite,STATUS_PENDING_MOD);
	if(uPreWebalizer>uWebalizer)
	{
		if(uExtJob)
			SubmitExtSingleJob("ExtDisableWebalizer",cDomain,
						cServer,clock,uExtJob,uSite,cDomain,1);//Update uOwner dude!
		else
			ScheduleJob(uSite,uServer,"DisableWebalizer",cDomain);
	}
	else
	{
		if(uExtJob)
			SubmitExtSingleJob("ExtEnableWebalizer",cDomain,
						cServer,clock,uExtJob,uSite,cDomain,1);//Update uOwner dude!
		else
			ScheduleJob(uSite,uServer,"EnableWebalizer",cDomain);
	}
}//void ModifyWebalizer(unsigned uPreWebalizer)


void ModifyMySQL(unsigned uMySQL,unsigned uPreMySQL,unsigned uSite,
		char *cDomain,unsigned uExtJob)
{
	time_t clock;
	char cServer[33]={"localhost"};//fix asap
	unsigned uStatus=0;
	unsigned uServer=1; //right ?

	time(&clock);
	uStatus=STATUS_PENDING_MOD;
	UpdateVHStatus(uSite,STATUS_PENDING_MOD);
	if(uPreMySQL>uMySQL)
	{
		if(uExtJob)
			SubmitExtSingleJob("ExtDisableMySQL",cDomain,
						cServer,clock,uExtJob,uSite,cDomain,1);//Update uOwner dude!
		else
			ScheduleJob(uSite,uServer,"DisableMySQL",cDomain);
	}
	else
	{
		if(uExtJob)
			SubmitExtSingleJob("ExtEnableMySQL",cDomain,
						cServer,clock,uExtJob,uSite,cDomain,1);//Update uOwner dude!
		else
			ScheduleJob(uSite,uServer,"EnableMySQL",cDomain);
	}
}//void ModifyMySQL()


void ScheduleJob(unsigned uSite, 
		unsigned uServer, char *cJobName, char *cJobData)
{
	time_t clock;
	
	time(&clock);

        sprintf(gcQuery,"INSERT INTO tJob (cServer,cLabel,cJobData,cJobName,uJobSite,"
			"uJobDate,uJobStatus,uOwner,uCreatedBy,uCreatedDate) "
			"SELECT cLabel,'%s','%s','%s',%u,%lu,%u,%u,%u,%lu FROM tServer WHERE uServer=%u",
			"Priority=Normal",
			cJobData,
			cJobName,
			uSite,//uJobSite
			(unsigned long)clock,
			1,//uJobStatus 1=waiting
			guLoginClient,
			guLoginClient,
			(unsigned long)clock,
			uServer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void ScheduleJob()


void UpdateVHStatus(unsigned uSite,unsigned uVHStatus)
{
	time_t clock;

	time(&clock);
	sprintf(gcQuery,"UPDATE tSite SET uStatus=%u,uModBy=1,uModDate=UNIX_TIMESTAMP_NOW() WHERE uSite=%u",uVHStatus,uSite);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                fprintf(stderr,"%s\n",mysql_error(&gMysql));

}//void UpdateVHStatus(unsigned uJob,unsigned uJobStatus)


//Does not allow empty lines...this may need reviewing ;) to say the least.
char *ParseTextAreaLines(char *cTextArea)
{
	static unsigned uEnd=0;
	static unsigned uStart=0;
	static char cRetVal[512];

	uStart=uEnd;
	while(cTextArea[uEnd++])
	{
		if(cTextArea[uEnd]=='\n' || cTextArea[uEnd]=='\r' || cTextArea[uEnd]==0
				|| cTextArea[uEnd]==10 || cTextArea[uEnd]==13 )
		{
			if(cTextArea[uEnd]==0)
				break;

			cTextArea[uEnd]=0;
			sprintf(cRetVal,"%.511s",cTextArea+uStart);

			if(cRetVal[0]=='\n' || cRetVal[0]==13)
			{
				uStart=uEnd=0;
				return("");
			}

			if(cTextArea[uEnd+1]==10)
				uEnd+=2;
			else
				uEnd++;

			return(cRetVal);
		}
	}

	if(uStart!=uEnd)
	{
		sprintf(cRetVal,"%.511s",cTextArea+uStart);
		return(cRetVal);
	}

	uStart=uEnd=0;
	return("");

}//char *ParseTextAreaLines(char *cTextArea)


void ProcessJobQueue(char *cServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	time_t luClock=0;
	unsigned uServer=0;
	unsigned uJobSiteUser=0;
	unsigned uJobSite=0;
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

	//The order by is to insure that sites are done first. This is needed
	//to insure site server group id needs to exist to create the site users.
        sprintf(gcQuery,"SELECT uJob,cJobName,uJobSite,uJobSiteUser,cJobData,uJobDate,"
			"uJobStatus,cLabel FROM tJob WHERE cServer='%s' AND uJobDate<%lu "
			"AND uJobStatus=1 ORDER BY uJobSiteUser LIMIT 20",cServer,luClock);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uJob);
		UpdateJobStatus(uJob,JOBSTATUS_RUNNING);

		//Debug only
		printf("cJobName=%s; cLabel=%s;\n",field[1],field[7]);

		//Common mysqlISP2.uJob patameter read from cJobData if external job
		if(!strncmp(field[1],"Ext",3))
		{
			char cLine[256]={""};
			sprintf(cLine,"%.255s",ParseTextAreaLines(field[4])); //First line contains the site name or the user name
			sprintf(cLine,"%.255s",ParseTextAreaLines(field[4]));
			sscanf(cLine,"mysqlISP2.tJob.uJob=%u",&uExtJob);
		}
			
		printf("Running: %s\n",field[1]);		
		if(!strcmp(field[1],"NewSiteUser") || !strcmp(field[1],"ExtNewSiteUser"))
		{
			if(!strcmp(field[1],"ExtNewSiteUser"))
			{
				char cSiteUser[100]={""};

				sscanf(field[7],"ExtNewSiteUser %s",cSiteUser);
				uJobSiteUser=uGetSiteUser(cSiteUser);
			}
			else
				sscanf(field[3],"%u",&uJobSiteUser);

			if(NewSiteUserJob(uJob,uJobSiteUser))
			{
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
				if(!strcmp(field[1],"ExtNewSiteUser"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtNewSiteUser() Failed",cServer,uExtJob,mysqlISP_RemotelyQueued);
				}
			}
			else
			{
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
				//External mysqlISP2 tJob instance update if required
				if(!strcmp(field[1],"ExtNewSiteUser"))
				{
					//printf("cServer=%s\nuExtJob=%u\nuStatus=%u\n",cServer,uExtJob,mysqlISP_Deployed);
					ExtConnectDb(0);
					InformExtJob("ExtNewSiteUser() OK",cServer,uExtJob,mysqlISP_Deployed);
				}
			}
		}
		else if(!strcmp(field[1],"NewSite") || !strcmp(field[1],"ExtNewSite"))
		{
			sscanf(field[2],"%u",&uJobSite);

			if(NewSiteJob(uJob,uJobSite))
			{
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
				if(!strcmp(field[1],"ExtNewSite"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtNewSite() Failed",cServer,uExtJob,mysqlISP_RemotelyQueued);
				}
			}
			else
			{
				//The following have to do with webfarm status
				//and not with individual site status. 
				//So we set the site status here since it is needed
				//for building working httpd.conf files.
				UpdateSiteStatus(uJobSite,STATUS_ACTIVE);

				if(MakeAndCheckConfFiles(cServer))
				{
					UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
				}
				else
				{
					if(RestartHTTPDaemons())
					{
						UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
						//Email 24x7 emergency maintenance?
					}
					else
					{
						UpdateJobStatus(uJob,JOBSTATUS_DONE);
						//External mysqlISP2 tJob instance update if required
						if(!strcmp(field[1],"ExtNewSite"))
						{
							ExtConnectDb(0);
							InformExtJob("ExtNewSite() OK",cServer,uExtJob,mysqlISP_Deployed);
						}
					}
				}
			}
		}
		else if(!strcmp(field[1],"ModSite") || !strcmp(field[1],"ExtModSite"))
		{
			sscanf(field[2],"%u",&uJobSite);

			if(ModSiteJob(uJob,uJobSite))
			{
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
				if(!strcmp(field[1],"ExtModSite"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtModSite Fatal Error",cServer,uExtJob,mysqlISP_RemotelyQueued);
				}
					
			}
			else
			{
				UpdateSiteStatus(uJobSite,STATUS_ACTIVE);

				if(MakeAndCheckConfFiles(cServer))
				{
					UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
				}
				else
				{
					if(RestartHTTPDaemons())
					{
						UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
						if(!strcmp(field[1],"ExtModSite"))
						{
							ExtConnectDb(0);
							InformExtJob("ExtModSite Fatal Error",cServer,uExtJob,mysqlISP_RemotelyQueued);
						}
						//Email 24x7 emergency maintenance?
					}
					else
					{
						UpdateJobStatus(uJob,JOBSTATUS_DONE);
						if(!strcmp(field[1],"ExtModSite"))
						{
							ExtConnectDb(0);
							InformExtJob("ExtModSite OK",cServer,uExtJob,mysqlISP_Deployed);
						}
							
					}
				}
			}
		}
		else if(!strcmp(field[1],"DelSite") || !strcmp(field[1],"ExtDelSite"))
		{
			sscanf(field[2],"%u",&uJobSite);

			if(DelSiteJob(uJob,uJobSite))
			{
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
				if(!strcmp(field[1],"ExtDelSite"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtDelSite Fatal Error",cServer,uExtJob,mysqlISP_RemotelyQueued);
				}
			}
			else
			{
				if(MakeAndCheckConfFiles(cServer))
				{
					UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
					if(!strcmp(field[1],"ExtDelSite"))
					{
						ExtConnectDb(0);
						InformExtJob("ExtDelSite Fatal Error",cServer,uExtJob,mysqlISP_RemotelyQueued);
					}
				}
				else
				{
					if(RestartHTTPDaemons())
					{
						UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
						if(!strcmp(field[1],"ExtDelSite"))
						{
							ExtConnectDb(0);
							InformExtJob("ExtDelSite Fatal Error",cServer,uExtJob,mysqlISP_RemotelyQueued);
						}
						//Email 24x7 emergency maintenance?
					}
					else
					{
						UpdateJobStatus(uJob,JOBSTATUS_DONE);
						if(!strcmp(field[1],"ExtDelSite"))
						{
							ExtConnectDb(0);
							InformExtJob("ExtDelSite OK",cServer,uExtJob,mysqlISP_Deployed);
						}
					}
				}
			}
		}
		else if(!strcmp(field[1],"ModSiteUser") || !strcmp(field[1],"ExtModSiteUser"))
		{
			if(!strcmp(field[1],"ExtModSiteUser"))
			{
				char cSiteUser[100]={""};

				sscanf(field[7],"ExtModSiteUser %s",cSiteUser);
				uJobSiteUser=uGetSiteUser(cSiteUser);
			}
			else
				sscanf(field[3],"%u",&uJobSiteUser);
			if(ModSiteUserJob(uJob,uJobSiteUser))
			{
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
				if(!strcmp(field[1],"ExtModSiteUser"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtModSiteUser Fatal Error",cServer,uExtJob,mysqlISP_RemotelyQueued);
				}
			}	
			else
			{
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
				if(!strcmp(field[1],"ExtModSiteUser"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtModSiteUser OK",cServer,uExtJob,mysqlISP_Deployed);
				}
			}
		}
		else if(!strcmp(field[1],"HoldSite") || !strcmp(field[1],"ExtHoldSite"))
		{
			sscanf(field[2],"%u",&uJobSite);
			if(HoldSite(uJobSite,uServer))
			{
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
				if(!strcmp(field[1],"ExtHoldSite"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtHoldSite Fatal Error",cServer,uExtJob,mysqlISP_RemotelyQueued);
				}
			}
			else
			{
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
				if(!strcmp(field[1],"ExtHoldSite"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtHoldSite OK",cServer,uExtJob,mysqlISP_Deployed);
				}
			}

		}
		else if(!strcmp(field[1],"RemoveHold") || !strcmp(field[1],"ExtRemoveHold"))
		{
			sscanf(field[2],"%u",&uJobSite);
			if(RemoveHold(uJobSite,uServer))
			{
				UpdateJobStatus(uJob,JOBSTATUS_FATAL_ERROR);
				if(!strcmp(field[1],"ExtRemoveHold"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtRemoveHold Fatal Error",cServer,uExtJob,mysqlISP_RemotelyQueued);
				}
			}
			else
			{
				UpdateJobStatus(uJob,JOBSTATUS_DONE);
				if(!strcmp(field[1],"ExtRemoveHold"))
				{
					ExtConnectDb(0);
					InformExtJob("ExtRemoveHold OK",cServer,uExtJob,mysqlISP_Deployed);
				}
			}
		}
		
	}//End of while for each job queue entry select

}//void ProcessJobQueue(const char *cServer)


unsigned uGetSiteUser(char *cUser)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uSiteUser=0;

	sprintf(gcQuery,"SELECT uSiteUser FROM tSiteUser WHERE cLogin='%s'",cUser);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uSiteUser);

	mysql_free_result(res);

	return(uSiteUser);

}//unsigned uGetSiteUser(char *cUser)


//Uses MYSQL mysqlext
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

}//end of ExtConnectDb()


void GetCurrentIPSites(unsigned uLoginClient,unsigned *uCurrentIPSites,
				unsigned uHtml,unsigned uServer)
{
        MYSQL_RES *res;

	if(guPermLevel>=10)
	{
		if(uServer)
			sprintf(gcQuery,"SELECT uSite FROM tSite WHERE uServer=%u AND uNameBased=0",uServer);
		else
			sprintf(gcQuery,"SELECT uSite FROM tSite WHERE uNameBased=0");
	}
	else
	{
		if(uServer)
			sprintf(gcQuery,"SELECT uSite FROM tSite WHERE uServer=%u AND uOwner=%u AND uNameBased=0",uServer,uLoginClient);
		else
			sprintf(gcQuery,"SELECT uSite FROM tSite WHERE uOwner=%u AND uNameBased=0",uLoginClient);
	}

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
                	htmlPlainTextError(mysql_error(&gMysql));
		else
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			return;
		}
	}
        res=mysql_store_result(&gMysql);
	*uCurrentIPSites=mysql_num_rows(res);
        mysql_free_result(res);
	
}//void GetCurrentIPSites()


void GetCurrentSites(unsigned uLoginClient,unsigned *uCurrentSites,
						unsigned uHtml,unsigned uServer)
{
        MYSQL_RES *res;

	if(guPermLevel>=10)
	{
		if(uServer)
        		sprintf(gcQuery,"SELECT uSite FROM tSite WHERE uServer=%u",uServer);
		else
        		sprintf(gcQuery,"SELECT uSite FROM tSite");
	}
	else
	{
		if(uServer)
        		sprintf(gcQuery,"SELECT uSite FROM tSite WHERE uOwner=%u AND uServer=%u",uLoginClient,uServer);
		else
        		sprintf(gcQuery,"SELECT uSite FROM tSite WHERE uOwner=%u",uLoginClient);
	}

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
                	htmlPlainTextError(mysql_error(&gMysql));
		else
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			return;
		}
	}

        res=mysql_store_result(&gMysql);
	*uCurrentSites=mysql_num_rows(res);
        mysql_free_result(res);
	
}//void GetCurrentSites()


unsigned IPAvailable(unsigned uHtml)
{
        MYSQL_RES *res;
	unsigned uRetVal=0;

        sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uAvailable=1");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
                	htmlPlainTextError(mysql_error(&gMysql));
		else
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			return(uRetVal);
		}
	}
        res=mysql_store_result(&gMysql);
	uRetVal=mysql_num_rows(res);
        mysql_free_result(res);
	
	return(uRetVal);
	
}//unsigned IPAvailable(unsigned uHtml)


void GetIPVirtualHost(char *cName, char *cValue, unsigned uServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	time_t clock;

	time(&clock);

	if(cName[0])
        sprintf(gcQuery,"SELECT cIP FROM tSite WHERE cDomain='%s' AND uServer=%u",
			cName,uServer);
	else
        sprintf(gcQuery,"SELECT cIP,uIP FROM tIP WHERE uAvailable=1");
	
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
                strcpy(cValue,field[0]);
		if(!cName[0])
		{
			sprintf(gcQuery,"UPDATE tIP SET uAvailable=0,uModBy=%u,"
					"uModDate=UNIX_TIMESTAMP_NOW() WHERE uIP=%s",
					guLoginClient,field[1]);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		}
	}
	
        mysql_free_result(res);

}//void GetIPVirtualHost(char *cName, char *cValue, unsigned uServer)


void GetTemplate(const char *cName, char *cValue, unsigned uHtml)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	//char *cCopy=NULL;

        sprintf(gcQuery,"SELECT cTemplate FROM tTemplate WHERE cLabel='%s'",cName);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
        	        htmlPlainTextError(mysql_error(&gMysql));
		else
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			return;
		}
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		//cCopy=(char *)malloc(strlen(field[0]));
		//if(!cCopy) tSite("malloc error");
		strncpy(cValue,field[0],MAXTEMPLATESIZE);
		cValue[MAXTEMPLATESIZE]=0;
		//cValue=cCopy;
	}
        mysql_free_result(res);

}//void GetTemplate(const char *cName, char *cValue, unsigned uHtml)


int HoldSite(unsigned uSite,unsigned uServer)
{
	struct t_template template;
	char cWebDir[256]={"/var/local/web"};
	char cWebSiteDir[512]={""};
	char cWebSiteDirOnHold[512]={""};
	FILE *fp;
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uRetVal=0;

	sprintf(gcQuery,"SELECT tSiteUser.cLogin FROM tSiteUser,tSite WHERE "
			"tSiteUser.uSite=tSite.uSite AND tSite.uSite=%u",uSite);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		return(2);
	}

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{
		//TODO random MD5. Enable MD5 everywhere like in mysqlSendmail
		//Meanwhile go ahead and crack this...if you are bored :)
		if((uRetVal+=ChangeSystemPasswd(field[0],"jfk38fcnsk38d")))
                	fprintf(stderr,"ChangeSystemPasswd() error: %s\n",field[0]);

	}
        mysql_free_result(res);



      sprintf(gcQuery,
		"SELECT cDomain FROM tSite WHERE uSite=%u",uSite);
      mysql_query(&gMysql,gcQuery);
      if(mysql_errno(&gMysql))
      {
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
               	return(2);
      }

      res=mysql_store_result(&gMysql);
      if((field=mysql_fetch_row(res)))
      {
	
	 //Change htdocs dir to hold dir
	 GetConfiguration("cWebDir",cWebDir,0,uServer);
	 sprintf(cWebSiteDir,"%.255s/%.255s/htdocs",cWebDir,field[0]);
	 sprintf(cWebSiteDirOnHold,"%.255s/%.255s/htdocs_onhold",cWebDir,field[0]);
	 if(rename(cWebSiteDir,cWebSiteDirOnHold))
         {
		fprintf(stderr,
			"rename() %s %s failed\n",cWebSiteDir,cWebSiteDirOnHold);
		return(3);
         }

	 //Install customized onhold index.html from template
	umask(000);
	mkdir(cWebSiteDir,0755);
	sprintf(cWebSiteDir,"%.255s/%.255s/htdocs/index.html",cWebDir,field[0]);
	if((fp=fopen(cWebSiteDir,"w")))
	{
		template.cpName[0]="cDomain";
		template.cpValue[0]=field[0];

		template.cpName[1]="";
		if((uRetVal=fileStructTemplate(fp,"HoldSite index.html",&template,0)))
                	fprintf(stderr,"fileStructTemplate() error: %s\n",cWebDir);
		fclose(fp);
	 }
	
	}
	else
	{
                fprintf(stderr,"cDomain not found?\n");
		return(312);
	}
	
	//Update tSite.uStatus
	sprintf(gcQuery,"UPDATE tSite SET uStatus='%u' WHERE uSite='%u'",STATUS_HOLD,uSite);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
               	return(2);
        }
	return(uRetVal);

}//int HoldSite(unsigned uSite)


int RemoveHold(unsigned uSite,unsigned uServer)
{
	char cWebDir[256]={"/var/local/web"};
	char cWebSiteDir[512]={""};
	char cWebSiteDirOnHold[512]={""};
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uRetVal=0;

	sprintf(gcQuery,"SELECT tSiteUser.cLogin,tSiteUser.cPasswd FROM "
			"tSiteUser,tSite WHERE tSiteUser.uSite=tSite.uSite "
			"AND tSite.uSite=%u",uSite);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		return(2);
	}

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{
		if((uRetVal+=ChangeSystemPasswd(field[0],field[1])))
                	fprintf(stderr,"ChangeSystemPasswd() error: %s\n",field[0]);

	}
        mysql_free_result(res);



      sprintf(gcQuery,
		"SELECT cDomain FROM tSite WHERE uSite=%u",uSite);
      mysql_query(&gMysql,gcQuery);
      if(mysql_errno(&gMysql))
      {
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
               	return(2);
      }

      res=mysql_store_result(&gMysql);
      if((field=mysql_fetch_row(res)))
      {
	
	 //Change htdocs dir to hold dir
	 GetConfiguration("cWebDir",cWebDir,0,uServer);
	 sprintf(cWebSiteDir,"%.255s/%.255s/htdocs/index.html",cWebDir,field[0]);
	 unlink(cWebSiteDir);
	 sprintf(cWebSiteDir,"%.255s/%.255s/htdocs",cWebDir,field[0]);
	 rmdir(cWebSiteDir);
	 sprintf(cWebSiteDirOnHold,"%.255s/%.255s/htdocs_onhold",cWebDir,field[0]);
	 if(rename(cWebSiteDirOnHold,cWebSiteDir))
         {
		fprintf(stderr,
			"rename() %s %s failed\n",cWebSiteDirOnHold,cWebSiteDir);
		return(3);
         }

	}
	else
	{
                fprintf(stderr,"cDomain not found?\n");
		return(313);
	}

	//Update tSite.uStatus
	sprintf(gcQuery,"UPDATE tSite SET uStatus='%u' WHERE uSite='%u'",STATUS_ACTIVE,uSite);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
               	return(2);
	}

	return(uRetVal);

}//int RemoveHold(unsigned uSite)


int NewSiteJob(unsigned uJob, unsigned uSite)
{
	//debug only
	printf("NewSiteJob(%u,%u)\n",uJob,uSite);
	//return(0);

	char cNewSiteTar[256]={"newwebsite/newwebsite.tar"};
	char cSiteTar[512]={""};
	char cWebRoot[256]={"/var/www/unxsapache/sites"};
	char cApacheDir[256]={"/var/www/unxsapache/conf.d"};
	//char cApacheUser[256]={"openisp"};
	char cWebSiteDir[512]={""};
	char cDomain[100]={""};
	struct stat info;
	FILE *fp;
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cQuery[1024];

	sprintf(cQuery,"SELECT tSite.cDomain FROM tSiteUser,tSite WHERE "
			"tSiteUser.uSite=tSite.uSite AND tSite.uSite=%u "
			"AND tSiteUser.uAdmin=1",uSite);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
                TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		strcpy(cDomain,field[0]);
	}
	else
	{
        	mysql_free_result(res);
                sprintf(cQuery,"NewSiteJob() No admin tSiteUser found for %s\n",cDomain);
		TextError(cQuery,1);
               	return(1);
	}
        mysql_free_result(res);

	//Check for required installation files
	GetConfiguration("cApacheDir",cApacheDir,0,0);
	GetConfiguration("cNewSiteTar",cNewSiteTar,0,0);
	sprintf(cSiteTar,"%s/%s",cWebRoot,cNewSiteTar);
	if(stat(cSiteTar,&info))
	{
		sprintf(cQuery,"NewSiteJob() No:\n%s\n",cSiteTar);
		TextError(cQuery,1);
		return(3);
	}
	
	GetConfiguration("cWebRoot",cWebRoot,0,0);
	sprintf(cWebSiteDir,"%s/%s",cWebRoot,cDomain);
	umask(000);
	if(stat(cWebSiteDir,&info))
	{
		if(mkdir(cWebSiteDir,0751))
		{
			sprintf(cQuery,"NewSiteJob() Could not mkdir(%s,0751)\n",cWebSiteDir);
			TextError(cQuery,1);
			return(5);
		}
		if(chmod(cWebSiteDir,0751))
		{
			sprintf(cQuery,"NewSiteJob() Could not chmod(%s,0751)\n",cWebSiteDir);
			TextError(cQuery,1);
			return(51);
		}
	}

	sprintf(cQuery,"tar -C %s -xf %s",cWebSiteDir,cSiteTar);
	if(system(cQuery))
	{
		TextError(cQuery,1);
		return(6);
	}

	sprintf(cQuery,"%s/htdocs/index.html",cWebSiteDir);
	if(!(fp=fopen(cQuery,"w")))
	{
		TextError(cQuery,1);
		return(7);
	}
	else
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect("NewSiteIndexHtml");
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;

			template.cpName[0]="cDomain";
			template.cpValue[0]=cDomain;

			template.cpName[1]="";

			Template(field[0],&template,fp);
		}
		mysql_free_result(res);

		fprintf(fp,"\n");
		fclose(fp);
	}
	return(0);

}//int NewSiteJob(unsigned uJob,unsigned uSite)


int NewSiteUserJob(unsigned uJob,unsigned uSiteUser)
{
	//debug only
	printf("NewSiteUserJob(%u,%u)\n",uJob,uSiteUser);
	//UpdateSiteUserStatus(uSiteUser,STATUS_ACTIVE);
	//return(0);

        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uSite=0;
	unsigned uGID=0;
	unsigned uUID=0;
	char cWebRoot[256]={"/var/www/unxsapache/sites"};
	char cWebSiteDir[256]={""};
	struct passwd *structPasswd;
	unsigned uRetVal;

        sprintf(gcQuery,"SELECT tSiteUser.cLogin,tSiteUser.cPasswd,tSiteUser.uSite,tSite.cDomain FROM "
			"tSiteUser,tSite WHERE tSiteUser.uSite=tSite.uSite AND tSiteUser.uSiteUser=%u "
			"AND tSiteUser.uAdmin=1",uSiteUser);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
                sscanf(field[2],"%u",&uSite);

		uGID=uSite+SITE_GID_BASE;
		if((structPasswd=getpwuid(uGID)))
		{
			sprintf(gcQuery,"NewSiteUserJob() No uGID for uSiteUser:%u and uSite:%u\n",
					uSiteUser,uSite);
			TextError(gcQuery,1);
			return(1);
		}

		uUID=uSiteUser+SITE_UID_BASE;
		if((structPasswd=getpwuid(uUID)))
		{
			sprintf(gcQuery,"NewSiteUserJob() User with uid:%u already exists\n",uUID);
			TextError(gcQuery,1);
			return(1);
		}

		GetConfiguration("cWebRoot",cWebRoot,0,0);
		sprintf(cWebSiteDir,"%s/%s",cWebRoot,field[3]);
		sprintf(gcQuery,"/usr/sbin/groupadd -g %u %s",uGID,field[3]);
		if(system(gcQuery))
		{
			TextError(gcQuery,1);
			return(1);
		}
		sprintf(gcQuery,"/usr/sbin/useradd -u %u -d %s -g %u -c %s -s %s %s > /dev/null 2>&1",
			uUID,cWebSiteDir,uGID,field[3],"/bin/bash",field[0]);
		if(system(gcQuery))
		{
			TextError(gcQuery,1);
			return(1);
		}
		if((uRetVal=ChangeSystemPasswd(field[0],field[1])))
		{
			sprintf(gcQuery,"NewSiteUserJob() ChangeSystemPasswd() Error:%u\n",uRetVal);
			TextError(gcQuery,1);
			return(1);
		}
		//SitePerms(char *cMainSiteUser, char *cWebSiteDir, char *cDomain);
		if(SitePerms(field[0],cWebSiteDir,field[3]))
		{
			TextError("SitePerms() Returned error(s)",1);
			return(1);
		}
		UpdateSiteUserStatus(uSiteUser,STATUS_ACTIVE);

	}
	mysql_free_result(res);
	return(0);

}//int NewSiteUserJob(unsigned uJob,unsigned uSiteUser)


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
                        fprintf(ofp,"%.64s:%.64s:10556::::::\n",cLogin,cPasswd);
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
		if (!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
		{
			printf("Database server unavailable\n");
			exit(1);	
		}
	}
			
}//end of CrontabConnectDb()


void UpdateJobStatus(unsigned uJob,unsigned uJobStatus)
{
	char cQuery[256];

        sprintf(cQuery,"UPDATE tJob SET uJobStatus=%u WHERE uJob=%u",
			uJobStatus,uJob);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);

}//void UpdateJobStatus(unsigned uJob,unsigned uJobStatus)


void UpdateSiteStatus(unsigned uSite,unsigned uStatus)
{
	char cQuery[256];

        sprintf(cQuery,"UPDATE tSite SET uStatus=%u WHERE uSite=%u",
			uStatus,uSite);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);

}//void UpdateSiteStatus(unsigned uSite,unsigned uJobStatus)


void UpdateSiteUserStatus(unsigned uSiteUser,unsigned uStatus)
{
	char cQuery[256];

        sprintf(cQuery,"UPDATE tSiteUser SET uStatus=%u WHERE uSiteUser=%u",
			uStatus,uSiteUser);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);

}//void UpdateSiteUserStatus(unsigned uSite,unsigned uJobStatus)


void TextError(const char *cError, unsigned uContinue)
{
	char cQuery[1024];

	printf("\nPlease report this unxsApache. error ASAP:\n%s\n",cError);

	//Attempt to report error in tLog
        sprintf(cQuery,"INSERT INTO tLog SET cLabel='TextError',uLogType=4,"
			"uPermLevel=%u,uLoginClient=%u,cLogin='%s',cHost='%s',"
			"cMessage=\"%s\",cServer='%s',uOwner=1,uCreatedBy=%u,"
			"uCreatedDate=UNIX_TIMESTAMP(NOW())",
			guPermLevel,guLoginClient,gcUser,gcHost,cError,gcHostname,guLoginClient);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		printf("Another error occurred while attempting to log: %s\n",
				mysql_error(&gMysql));
	if(!uContinue) exit(0);

}//void TextError(const char *cError, unsigned uContinue)


int MakeAndCheckConfFiles(const char *cServer)
{
        MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uServer=0;
	FILE *fp;
	char cApacheDir[256]={"/var/www/unxsapache/conf.d/"};
	char cApacheSSLDir[256]={"/var/www/unxsapache/conf.d"};
	char cServerIP[256]={"10.0.0.7"};
	char cServerDomain[256]={"myisp.net"};
	char cServerAdmin[256]={"support@myisp.net"};
	unsigned uRetVal=0;

	if(!cServer[0])
	{
		TextError("MakeAndCheckConfFiles() null cServer",1);
		return(1);
	}
	uServer=GetuServer(cServer);
	if(!uServer)
	{
		TextError("MakeAndCheckConfFiles() no such cServer",1);
		return(1);
	}

	GetConfiguration("cServerIP",cServerIP,uServer,0);
	GetConfiguration("cServerDomain",cServerDomain,uServer,0);
	GetConfiguration("cServerAdmin",cServerAdmin,uServer,0);
	GetConfiguration("cApacheDir",cApacheDir,uServer,0);
	
	sprintf(gcQuery,"/sbin/ifconfig -a | grep inet | grep %s > /dev/null 2>&1",cServerIP);
	if(system(gcQuery))
	{
		printf("%s\n",gcQuery);
		TextError("cServerIP may not be up",1);
		return(1);
	}

        sprintf(gcQuery,"SELECT cIP,uNameBased FROM tSite WHERE uServer=%u AND uStatus=%u AND cIP!='0.0.0.0' "
			"ORDER BY cDomain",uServer,STATUS_ACTIVE);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[1],"0"))
		{
			sprintf(gcQuery,"/sbin/ifconfig -a | grep inet | grep %s > /dev/null 2>&1",field[0]);
			if(system(gcQuery))
			{
				sprintf(gcQuery,"Required IP %s may not be up!",field[0]);
				TextError(gcQuery,1);
				uRetVal++;
			}
		}
	}
	mysql_free_result(res);
	if(uRetVal)
	{
		TextError("!MakeAndCheckConfFiles() aborted!",1);
		return(uRetVal);
	}


        sprintf(gcQuery,"SELECT cVirtualHost,cDomain FROM tSite WHERE uServer=%u AND uStatus=%u ORDER BY uNameBased",uServer,STATUS_ACTIVE);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"%s/%s.conf",cApacheDir,field[1]);
		if(!(fp=fopen(gcQuery,"w")))
		{
			TextError(gcQuery,1);
			return(uRetVal);
		}
		fprintf(fp,"\n#Start cVirtualHost section %s\n",field[1]);
		fprintf(fp,"%s",field[0]);
		fprintf(fp,"#End cVirtualHost section %s\n",field[1]);
		fprintf(fp,"\n#Built by webfarm automation system unxsApache.\n"
			"#svn ID removed
		fclose(fp);
		
	}
	mysql_free_result(res);

	GetConfiguration("cApacheSSLDir",cApacheSSLDir,0,0);

        sprintf(gcQuery,"SELECT cSSLVirtualHost,cDomain FROM tSite WHERE uServer=%u AND uStatus=%u "
			"AND cSSLVirtualHost!='' AND uNameBased=0 ORDER BY cDomain",uServer,STATUS_ACTIVE);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"%s/%s.conf",cApacheSSLDir,field[1]);
		if(!(fp=fopen(gcQuery,"a")))
		{
			TextError(gcQuery,1);
			return(uRetVal);
		}
		fprintf(fp,"\n#Start cSSLVirtualHost section %s\n",field[1]);
		fprintf(fp,"%s",field[0]);
		fprintf(fp,"#End cSSLVirtualHost section %s\n",field[1]);
		fclose(fp);
		fprintf(fp,"\n#Built by webfarm automation system unxsApache.\n"
			"#svn ID removed
		fclose(fp);
	}
	mysql_free_result(res);

/*
	//Here we create all the SSL cert files
	GetConfiguration("cWebRoot",cWebRoot,0,0);
	sprintf(gcQuery,"SELECT tSite.cDomain,tSSLCert.cDomain,tSSLCert.cCert,tSSLCert.cKey FROM "
			"tSite,tSSLCert WHERE tSite.uSSLCert=tSSLCert.uSSLCert AND tSite.uNameBased=0 "
			"AND tSite.uServer=%u AND tSite.uStatus=%u",uServer,STATUS_ACTIVE);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"%s/%s/conf/%s.pem",cWebRoot,field[0],field[1]);
		if((fp=fopen(gcQuery,"w")))
		{
			fprintf(fp,"%s\n%s",field[2],field[3]);
			fclose(fp);
			chmod(gcQuery,0400);//Protect it has the key in it
		}
	}
	mysql_free_result(res);
*/
	uRetVal+=CheckConfFiles();

	
	//Restart httpd daemons should be done after same check and check for
	//critical items like server certificates and log directories
	//that will bring down services for all sites :(
	return(uRetVal);

}//int MakeAndCheckConfFiles(const char *cServer)


int CheckConfFiles(void)
{
	unsigned uRetVal=0;

	//Use httpd -t
	//error checking.
	sprintf(gcQuery,"/usr/sbin/httpd -t");
	if(system(gcQuery))
	{
		TextError("httpd.conf configuration error",1);
		uRetVal++;
	}

	//Now check for warnings
	sprintf(gcQuery,"/usr/sbin/httpd -t 2>&1 | grep -i warn > /dev/null 2>&1");
	if(!system(gcQuery))
	{
		TextError("httpd.conf configuration warning",1);
		uRetVal++;
	}

	return(uRetVal);

}//int CheckConfFiles(char *cApacheDir, char *cApacheSSLDir)


//We assume that at least CheckConfFiles() has run and returned no errors or warnings!
int RestartHTTPDaemons(void)
{
	struct stat statInfo;
	unsigned uRetVal=0;
	unsigned uApacheRestart=0;
	//Let's do some tests to see how apache2 with mod_ssl handles graceful
	//restarts.
	//
	//First. If the daemons are not running we better start them.

	//Check to see if apache is running. Two scenarios no PID in assumed place
	//and if PID file no root process with assumed sig
	if(!stat("/var/run/httpd.pid",&statInfo))
	{
		sprintf(gcQuery,"ps -ef | grep `cat /var/run/httpd.pid` | grep httpd | grep -w 1 >/dev/null 2>&1");
		if(system(gcQuery))
			uApacheRestart=1;
	}
	else
	{
		uApacheRestart=1;
	}

	if(uApacheRestart)
	{
		//Not running start
		TextError("Apache server not running. Trying to start...",1);

		if(system("/etc/init.d/httpd start"))
		{
			TextError("!Apache server not running. Failed to start!",1);
			uApacheRestart=2;
			uRetVal++;
		}

	}

	if(uApacheRestart)
	{
		if(uApacheRestart==1)
			TextError("Apache server was started",1);
		return(uRetVal);
	}

	//Second. If the daemons are running graceful restart
	sprintf(gcQuery,"kill -USR1 `cat /var/run/httpd.pid` >/dev/null 2>&1");
	if(system(gcQuery))
	{
		TextError("Apache server graceful restart error",1);
		uRetVal++;
	}

	if(stat("/var/run/httpd.pid",&statInfo))
	{
		TextError("Apache server httpd.pid not found!",1);
		uRetVal++;
	}


	return(uRetVal);

}//int RestartHTTPDaemons(void)


int CommandLineRestart(void)
{
	if(!CheckConfFiles())
		return(RestartHTTPDaemons());

	printf("CheckConfFiles() returned non zero value\n");
	return(1);

}//int CommandLineRestart(void)


int CommandLineCheckConfFiles(void)
{
	return(CheckConfFiles());

}//int CommandLineCheckConfFiles(void)


int SitePerms(char *cMainSiteUser, char *cWebSiteDir, char *cDomain)
{
	struct stat statInfo;
	char cQuery[512];
	int uRetVal=0;

	//change ownerships
	sprintf(cQuery,"chown -hR %s %s",cMainSiteUser,cWebSiteDir);
	if(system(cQuery))
	{
		TextError(cQuery,1);
		uRetVal++;
	}

	sprintf(cQuery,"chgrp -hR %s %s",cDomain,cWebSiteDir);
	if(system(cQuery))
	{
		TextError(cQuery,1);
		uRetVal++;
	}

	//Protect logs dir
	sprintf(cQuery,"chown -hR root %s/logs",cWebSiteDir);
	if(system(cQuery))
	{
		TextError(cQuery,1);
		uRetVal++;
	}
	sprintf(cQuery,"chgrp -hR root %s/logs",cWebSiteDir);
	if(system(cQuery))
	{
		TextError(cQuery,1);
		uRetVal++;
	}
	sprintf(cQuery,"chmod -R o-rwx %s/logs",cWebSiteDir);
	if(system(cQuery))
	{
		TextError(cQuery,1);
		uRetVal++;
	}

	//Protect usage (webalizer) directory if in new site tar
	sprintf(cQuery,"%s/htdocs/usage",cWebSiteDir);
	if(!stat(cQuery,&statInfo))
	{
		sprintf(cQuery,"chown -hR root %s/htdocs/usage",cWebSiteDir);
		if(system(cQuery))
		{
			TextError(cQuery,1);
			uRetVal++;
		}
		sprintf(cQuery,"chgrp -hR root %s/htdocs/usage",cWebSiteDir);
		if(system(cQuery))
		{
			TextError(cQuery,1);
			uRetVal++;
		}
	}

	//Insure access for nobody, via /etc/group website group
	sprintf(cQuery,"chmod -R ug+rX %s",cWebSiteDir);
	if(system(cQuery))
	{
		TextError(cQuery,1);
		uRetVal++;
	}

	//user 'nobody' runs cgi's that need to write to cgi-bin usually
	//This shouldn't hurt ln -s files
	//NOTE Security problem: Other webfarm users can write cgi's to
	//snoop-add to cgi-bin dir files. Please fix asap.
	sprintf(cQuery,"%s/cgi-bin",cWebSiteDir);
	if(chmod(cQuery,0771))
	{
		TextError(cQuery,1);
		uRetVal++;
	}

	//Allow group members write access to public html dir
	sprintf(cQuery,"%s/htdocs",cWebSiteDir);
	if(chmod(cQuery,0771))
	{
		TextError(cQuery,1);
		uRetVal++;
	}
	if(chmod(cWebSiteDir,0751))
	{
		TextError(cQuery,1);
		uRetVal++;
	}

	return(uRetVal);

}//int SitePerms(char *cMainSiteUser, char *cWebSiteDir, char *cDomain)


int ModSiteJob(unsigned uJob, unsigned uSite)
{
	//debug only
	printf("ModSiteJob(%u,%u)\n",uJob,uSite);
	return(0);

}//int ModSiteJob(unsigned uJob, unsigned uSite)


int ModSiteUserJob(unsigned uJob,unsigned uSiteUser)
{
	//debug only
	printf("ModSiteUserJob(%u,%u)\n",uJob,uSiteUser);
	//UpdateSiteUserStatus(uSiteUser,STATUS_ACTIVE);
	//return(0);

        MYSQL_RES *res;
        MYSQL_ROW field;

        sprintf(gcQuery,"SELECT cLogin,cPasswd FROM tSiteUser WHERE uSiteUser=%u",uSiteUser);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		unsigned uRetVal=0;

		if((uRetVal=ChangeSystemPasswd(field[0],field[1])))
		{
			sprintf(gcQuery,"ModSiteUserJob() ChangeSystemPasswd() Error:%u\n",uRetVal);
			TextError(gcQuery,1);
			return(1);
		}
		UpdateSiteUserStatus(uSiteUser,STATUS_ACTIVE);

	}
	mysql_free_result(res);
	return(0);

}//int ModSiteUserJob(unsigned uJob,unsigned uSiteUser)


int DelSiteJob(unsigned uJob, unsigned uSite)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uOnlyOnce=1;
	unsigned uRetVal=0;
	char cApacheDir[256]={"/var/www/unxsapache/conf.d"};

	//debug only
	printf("DelSiteJob(%u,%u)\n",uJob,uSite);

	//Delete all site users from system and from tSiteUser
        sprintf(gcQuery,"SELECT tSiteUser.cLogin,tSite.cDomain,tSite.uNameBased,tSite.cIP FROM "
			"tSiteUser,tSite WHERE tSiteUser.uSite=tSite.uSite AND tSiteUser.uSite=%u",uSite);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		TextError(mysql_error(&gMysql),0);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"/usr/sbin/userdel %s\n",field[0]);
		if(system(gcQuery))
		{
			TextError("Error removing user in DelSiteJob()",1);
			uRetVal++;
		}
		//Only once per site
		if(uOnlyOnce)
		{
			uOnlyOnce=0;
			sprintf(gcQuery,"/usr/sbin/groupdel %s\n",field[1]);
			if(system(gcQuery))
			{
				TextError("Error removing group in DelSiteJob()",1);
				uRetVal++;
			}
		}
		//If IP based site release the now not used IP
		if(!strcmp(field[2],"0") && field[3][0])
		{
        		sprintf(gcQuery,"UPDATE tIP SET uAvailable=1 WHERE cIP='%.15s'",field[3]);
        		mysql_query(&gMysql,gcQuery);
        		if(mysql_errno(&gMysql))
			{
				TextError(mysql_error(&gMysql),1);
				uRetVal++;
			}
		}
	}
	mysql_free_result(res);

        sprintf(gcQuery,"DELETE FROM tSiteUser WHERE uSite=%u",uSite);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		TextError(mysql_error(&gMysql),1);
		uRetVal++;
	}
	
	//Delete tSite record
        sprintf(gcQuery,"DELETE FROM tSite WHERE uSite=%u",uSite);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		TextError(mysql_error(&gMysql),1);
		uRetVal++;
	}

	//Delete the site .conf file
	sprintf(gcQuery,"%s/%s.conf",cApacheDir,field[1]);
	if(unlink(gcQuery))
	{
		TextError("unlink() failed",1);
		uRetVal++;
	}

	//Do not delete site dir now. We may need the files. This can be done monthly.
	//Further we can keep the files 30 days after this job ran.
	//Should we create a remove site files job abd schedule +30 days?

	return(uRetVal);

}//int DelSiteJob(unsigned uJob, unsigned uSite)

