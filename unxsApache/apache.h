/*
FILE
	apache.h
	svn ID removed
PURPOSE
	Specific application defines and prototypes
AUTHOR/LEGAL
	(C) 2006-2007 Gary Wallis.
*/

#define SITE_GID_BASE 10000
#define SITE_UID_BASE 20000

#define JOBSTATUS_WAITING 1
#define JOBSTATUS_RUNNING 2
#define JOBSTATUS_DONE 3
#define JOBSTATUS_SUSPENDED 4
#define JOBSTATUS_FATAL_ERROR 5
#define JOBSTATUS_CANCELED 6
#define JOBSTATUS_REDUNDANT 6 // ??? fix me

#define STATUS_PENDING 1
#define STATUS_PENDING_MOD 2
#define STATUS_PENDING_DEL 3
#define STATUS_ACTIVE 4
#define STATUS_HOLD 5

#define MAXTEMPLATESIZE 2047


//mysqlISP constants
//tJob.uJobStatus
#define mysqlISP_RemotelyQueued 7
#define mysqlISP_Waiting 10
//tInstance.uStatus:
#define mysqlISP_Deployed 4
#define mysqlISP_Canceled 5
#define mysqlISP_OnHold 6

typedef struct {
	//tVirtualHost
	char cDomain[100];
	char cIPBasedRootSite[100];
	char cServer[33];
	unsigned uServer;
	char cParkedDomains[1024];
	unsigned uNameBased;
	unsigned uHDQuota;
	unsigned uTrafficQuota;
	unsigned uFrontPage;
      	unsigned uWebalizer;
	unsigned uMySQL;
	unsigned uMaxUsers; 

	//If present in cJobData marked =1 in Parse;
	unsigned uParamMaxUsers;
	unsigned uParamTrafficQuota;
	unsigned uParamHDQuota;
	unsigned uParamMySQL;
	unsigned uParamWebalizer;
	unsigned uParamFrontPage;
	unsigned uParamNameBased;
	unsigned uParamParkedDomains;
	unsigned uParamServer;
	unsigned uParamIPBasedRootSite;
	unsigned uParamDomain;

	//tClient
	unsigned uISPClient; 
	char cClientName[33];//cLabel
	unsigned uMaxSites;
	unsigned uMaxIPs;
	//If present in cJobData marked =1 in Parse;
	unsigned uParamClientName;

	//tSiteUsers	
	char cLogin[64];
	char cPasswd[16];
	unsigned uClearText;
	unsigned uAdmin;
	unsigned uSSH;
	unsigned uFTP;
	unsigned uPop3;
	unsigned uSMTP;
	//If present in cJobData marked =1 in Parse;
	unsigned uParamLogin;
	unsigned uParamPasswd;

} structExtJobParameters;

void ParseExtParams(structExtJobParameters *structExtParam, char *cJobData);
void InitializeParams(structExtJobParameters *structExtParam);
int SubmitExtSingleJob(const char *cJobName,char *cJobData,const char *cServer,unsigned uJobDate, unsigned uJob, unsigned uJobSite,const char *cResource,unsigned uOwner);
int SubmitISPJob(char *cJobName,char *cJobData,char *cServer,unsigned uJobDate);
int InformExtJob(char *cRemoteMsg,char *cServer,unsigned uJob,unsigned uJobStatus);
int SetupExtSiteUser(char *cLogin,char *cPasswd, unsigned uISPClient);
int SetupExtProFTP(char *cDomain, unsigned uISPClient);
int ChangeSystemPasswd(char *login,char *encpasswd);
int AddUser(char *cLogin, unsigned uMode,unsigned uVirtualHost);
int ModUser(char *cLogin,unsigned uVirtualHost);
int DelUser(char *cLogin,unsigned uVirtualHost);
int AddSite(unsigned uVirtualHost);
int DelSite(unsigned uVirtualHost);

int ExtModify(structExtJobParameters *structExtParam, unsigned uVirtualHost,
		unsigned uJob, char *cServer);
MYSQL mysqlext;
//void UpdateJobStatus(unsigned uJob,unsigned uJobStatus,char *cRemoteMsg);
void UpdateVHStatus(unsigned uVirtualHost,unsigned uVHStatus);
void UpdateUserStatus(char *cLogin,unsigned uVirtualHost,unsigned uVHStatus);

unsigned IPAvailable(unsigned uHtml);
void GetCurrentIPSites(unsigned uLoginClient,unsigned *uCurrentIPSites,unsigned uHtml,unsigned uServer);
void GetCurrentSites(unsigned uLoginClient,unsigned *uCurrentSites,unsigned uHtml,unsigned uServer);
void CreateNewClient(structExtJobParameters *structExtParam);
void GetIPVirtualHost(char *cName, char *cValue, unsigned uServer);
void GetTemplate(const char *cName, char *cValue, unsigned uHtml);
void ScheduleJob(unsigned uVirtualHost,unsigned uServer, char *cJobName, char *cJobData);
void ModifyMySQL(unsigned uMySQL,unsigned uPreMySQL,unsigned uVirtualHost,char *cDomain,unsigned uExtJob);
void ModifyWebalizer(unsigned uWebalizer,unsigned uPreWebalizer,unsigned uVirtualHost,char *cDomain,unsigned uExtJob);
void ModifyParkedDomains(char *cIP, char *cDomain, char *cParkedDomains,unsigned uVirtualHost,unsigned uHtml,unsigned uExtJob);
void HandleOtherPrevQueuedJobs(unsigned uVirtualHost,char *cServer);

//tsitefunc.h
void EscapeDots(char const *cDomain,char *cDomainEscDot,unsigned uMaxLength);

