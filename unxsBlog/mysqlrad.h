/*
FILE
	svn ID removed
AUTHOR
	(C) 2001-2007 Gary Wallis. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
//This is an example tune for your system
#ifndef Linux
        #include <sys/systeminfo.h>
#else
        #ifndef FreeBSD
                #include <crypt.h>
        #endif
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/file.h>

#include "cgi.h"

//This is an example tune for your system
#ifdef FreeBSD
	#include "/usr/local/include/mysql/mysql.h"
#else
	#include <mysql/mysql.h>
#endif


#include <unistd.h>
#include <locale.h>
#include <monetary.h>

#include "language.h"
#include "local.h"

extern char gcHost[];
extern char gcHostname[];
extern char gcUser[];
#define PERMLEVEL
extern int guPermLevel; 
extern unsigned guLoginClient; 
extern unsigned guReseller; 

extern char gcFunction[];
extern unsigned guListMode; 
extern char gcQuery[];
extern char *gcQstr;
extern char *gcBuildInfo;
extern char *gcRADStatus;
extern MYSQL gMysql; 
extern MYSQL gMysql2; 
extern unsigned long gluRowid;
extern unsigned guStart;
extern unsigned guEnd;
extern unsigned guI;
extern unsigned guN;
extern char gcCommand[];
extern char gcFilter[];
extern char gcFind[];
extern char gcTable[];
extern unsigned guMode;
extern int guError;
extern char gcErrormsg[];

void unxsBlog(const char *cResult);
void ConnectDb(void);
void Footer_ism3(void);
void Header_ism3(char *cMsg, int iJs);
void ProcessControlVars(pentry entries[], int x);
void OpenRow(const char *cFieldLabel, const char *cColor);
void OpenFieldSet(char *cLabel, unsigned uWidth);
void CloseFieldSet(void);
void LoadConf(void);
void NoSuchFunction(void);
void tTablePullDown(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode);
void tTablePullDownOwner(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode);
void tTablePullDownReadOnly(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector);
int ReadPullDown(const char *cTableName,const char *cFieldName,const char *cLabel);
char *TextAreaSave(char *cField);
char *TransformAngleBrackets(char *cField);
char *EncodeDoubleQuotes(char *cField);
void YesNoPullDown(char *cFieldName,unsigned uSelect,unsigned uMode);
void YesNo(unsigned uSelect);
int ReadYesNoPullDown(const char *cLabel);
const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);
void GetClientOwner(unsigned uClient, unsigned *uOwner);
void ExtMainShell(int argc, char *argv[]);
void jsCalendarInput(char *cInputName,char *cValue,unsigned uMode);
long unsigned luGetModDate(char *cTableName, unsigned uTablePK);
long unsigned luGetCreatedDate(char *cTableName, unsigned uTablePK);
void DashBoard(const char *cOptionalMsg);
void htmlPlainTextError(const char *cError);

void unxsBlogLog(unsigned uTablePK, char *cTableName, char *cLogEntry);

 //Standard tInputFunc functions
char *WordToLower(char *cInput);
char *IPNumber(char *cInput);
char *IPv4All(char *cInput);
char *IPv4Range(char *cInput);
char *IPv4CIDR(char *cInput);
char *FQDomainName(char *cInput);
char *EmailInput(char *cInput);
char *cMoneyInput(char *cInput);
char *cMoneyDisplay(char *cInput);

 //Standard tValidFunc functions
const char *EmptyString(const char *cInput);
const char *BadIPNum(const char *cInput);
const char *IsZero(const unsigned uInput);

 //External pagination form processing vars
void PageMachine(char *cFuncName, int iLmode, char *cMsg);

 //Place ModuleCommands() and Module() prototypes here
#define ISPNAME "OpenISP"
#define ISPURL "www.openisp.net"
#define ADMIN 9

//tContent
int tContentCommands(pentry entries[], int x);
void tContent(const char *results);
void ProcesstContentVars(pentry entries[], int x);
void tContentContent(void);
void tContentInputContent(void);
void tContentInput(unsigned uMode);
void tContentList(void);
void NewtContent(unsigned uMode);
void ModtContent(void);
void CreatetContent(void);
void DeletetContent(void);
void ExttContentGetHook(entry gentries[], int x);
void ExttContentNavBar(void);

//tContentType
int tContentTypeCommands(pentry entries[], int x);
void tContentType(const char *results);
void ProcesstContentTypeVars(pentry entries[], int x);
void tContentTypeContent(void);
void tContentTypeInputContent(void);
void tContentTypeInput(unsigned uMode);
void tContentTypeList(void);
void NewtContentType(unsigned uMode);
void ModtContentType(void);
void CreatetContentType(void);
void DeletetContentType(void);
void ExttContentTypeGetHook(entry gentries[], int x);
void ExttContentTypeNavBar(void);

//tClient
int tClientCommands(pentry entries[], int x);
void tClient(const char *results);
void ProcesstClientVars(pentry entries[], int x);
void tClientContent(void);
void tClientInputContent(void);
void tClientInput(unsigned uMode);
void tClientList(void);
void NewtClient(unsigned uMode);
void ModtClient(void);
void CreatetClient(void);
void DeletetClient(void);
void ExttClientGetHook(entry gentries[], int x);
void ExttClientNavBar(void);

//tAuthorize
int tAuthorizeCommands(pentry entries[], int x);
void tAuthorize(const char *results);
void ProcesstAuthorizeVars(pentry entries[], int x);
void tAuthorizeContent(void);
void tAuthorizeInputContent(void);
void tAuthorizeInput(unsigned uMode);
void tAuthorizeList(void);
void NewtAuthorize(unsigned uMode);
void ModtAuthorize(void);
void CreatetAuthorize(void);
void DeletetAuthorize(void);
void ExttAuthorizeGetHook(entry gentries[], int x);
void ExttAuthorizeNavBar(void);

//tTemplate
int tTemplateCommands(pentry entries[], int x);
void tTemplate(const char *results);
void ProcesstTemplateVars(pentry entries[], int x);
void tTemplateContent(void);
void tTemplateInputContent(void);
void tTemplateInput(unsigned uMode);
void tTemplateList(void);
void NewtTemplate(unsigned uMode);
void ModtTemplate(void);
void CreatetTemplate(void);
void DeletetTemplate(void);
void ExttTemplateGetHook(entry gentries[], int x);
void ExttTemplateNavBar(void);

//tTemplateSet
int tTemplateSetCommands(pentry entries[], int x);
void tTemplateSet(const char *results);
void ProcesstTemplateSetVars(pentry entries[], int x);
void tTemplateSetContent(void);
void tTemplateSetInputContent(void);
void tTemplateSetInput(unsigned uMode);
void tTemplateSetList(void);
void NewtTemplateSet(unsigned uMode);
void ModtTemplateSet(void);
void CreatetTemplateSet(void);
void DeletetTemplateSet(void);
void ExttTemplateSetGetHook(entry gentries[], int x);
void ExttTemplateSetNavBar(void);

//tTemplateType
int tTemplateTypeCommands(pentry entries[], int x);
void tTemplateType(const char *results);
void ProcesstTemplateTypeVars(pentry entries[], int x);
void tTemplateTypeContent(void);
void tTemplateTypeInputContent(void);
void tTemplateTypeInput(unsigned uMode);
void tTemplateTypeList(void);
void NewtTemplateType(unsigned uMode);
void ModtTemplateType(void);
void CreatetTemplateType(void);
void DeletetTemplateType(void);
void ExttTemplateTypeGetHook(entry gentries[], int x);
void ExttTemplateTypeNavBar(void);

//tLog
int tLogCommands(pentry entries[], int x);
void tLog(const char *results);
void ProcesstLogVars(pentry entries[], int x);
void tLogContent(void);
void tLogInputContent(void);
void tLogInput(unsigned uMode);
void tLogList(void);
void NewtLog(unsigned uMode);
void ModtLog(void);
void CreatetLog(void);
void DeletetLog(void);
void ExttLogGetHook(entry gentries[], int x);
void ExttLogNavBar(void);

//tLogType
int tLogTypeCommands(pentry entries[], int x);
void tLogType(const char *results);
void ProcesstLogTypeVars(pentry entries[], int x);
void tLogTypeContent(void);
void tLogTypeInputContent(void);
void tLogTypeInput(unsigned uMode);
void tLogTypeList(void);
void NewtLogType(unsigned uMode);
void ModtLogType(void);
void CreatetLogType(void);
void DeletetLogType(void);
void ExttLogTypeGetHook(entry gentries[], int x);
void ExttLogTypeNavBar(void);

//tLogMonth
int tLogMonthCommands(pentry entries[], int x);
void tLogMonth(const char *results);
void ProcesstLogMonthVars(pentry entries[], int x);
void tLogMonthContent(void);
void tLogMonthInputContent(void);
void tLogMonthInput(unsigned uMode);
void tLogMonthList(void);
void NewtLogMonth(unsigned uMode);
void ModtLogMonth(void);
void CreatetLogMonth(void);
void DeletetLogMonth(void);
void ExttLogMonthGetHook(entry gentries[], int x);
void ExttLogMonthNavBar(void);

//tMonth
int tMonthCommands(pentry entries[], int x);
void tMonth(const char *results);
void ProcesstMonthVars(pentry entries[], int x);
void tMonthContent(void);
void tMonthInputContent(void);
void tMonthInput(unsigned uMode);
void tMonthList(void);
void NewtMonth(unsigned uMode);
void ModtMonth(void);
void CreatetMonth(void);
void DeletetMonth(void);
void ExttMonthGetHook(entry gentries[], int x);
void ExttMonthNavBar(void);

//tGlossary
int tGlossaryCommands(pentry entries[], int x);
void tGlossary(const char *results);
void ProcesstGlossaryVars(pentry entries[], int x);
void tGlossaryContent(void);
void tGlossaryInputContent(void);
void tGlossaryInput(unsigned uMode);
void tGlossaryList(void);
void NewtGlossary(unsigned uMode);
void ModtGlossary(void);
void CreatetGlossary(void);
void DeletetGlossary(void);
void ExttGlossaryGetHook(entry gentries[], int x);
void ExttGlossaryNavBar(void);

//tJob
int tJobCommands(pentry entries[], int x);
void tJob(const char *results);
void ProcesstJobVars(pentry entries[], int x);
void tJobContent(void);
void tJobInputContent(void);
void tJobInput(unsigned uMode);
void tJobList(void);
void NewtJob(unsigned uMode);
void ModtJob(void);
void CreatetJob(void);
void DeletetJob(void);
void ExttJobGetHook(entry gentries[], int x);
void ExttJobNavBar(void);

//tServer
int tServerCommands(pentry entries[], int x);
void tServer(const char *results);
void ProcesstServerVars(pentry entries[], int x);
void tServerContent(void);
void tServerInputContent(void);
void tServerInput(unsigned uMode);
void tServerList(void);
void NewtServer(unsigned uMode);
void ModtServer(void);
void CreatetServer(void);
void DeletetServer(void);
void ExttServerGetHook(entry gentries[], int x);
void ExttServerNavBar(void);

//tJobStatus
int tJobStatusCommands(pentry entries[], int x);
void tJobStatus(const char *results);
void ProcesstJobStatusVars(pentry entries[], int x);
void tJobStatusContent(void);
void tJobStatusInputContent(void);
void tJobStatusInput(unsigned uMode);
void tJobStatusList(void);
void NewtJobStatus(unsigned uMode);
void ModtJobStatus(void);
void CreatetJobStatus(void);
void DeletetJobStatus(void);
void ExttJobStatusGetHook(entry gentries[], int x);
void ExttJobStatusNavBar(void);

//tStatus
int tStatusCommands(pentry entries[], int x);
void tStatus(const char *results);
void ProcesstStatusVars(pentry entries[], int x);
void tStatusContent(void);
void tStatusInputContent(void);
void tStatusInput(unsigned uMode);
void tStatusList(void);
void NewtStatus(unsigned uMode);
void ModtStatus(void);
void CreatetStatus(void);
void DeletetStatus(void);
void ExttStatusGetHook(entry gentries[], int x);
void ExttStatusNavBar(void);

//tConfiguration
int tConfigurationCommands(pentry entries[], int x);
void tConfiguration(const char *results);
void ProcesstConfigurationVars(pentry entries[], int x);
void tConfigurationContent(void);
void tConfigurationInputContent(void);
void tConfigurationInput(unsigned uMode);
void tConfigurationList(void);
void NewtConfiguration(unsigned uMode);
void ModtConfiguration(void);
void CreatetConfiguration(void);
void DeletetConfiguration(void);
void ExttConfigurationGetHook(entry gentries[], int x);
void ExttConfigurationNavBar(void);


