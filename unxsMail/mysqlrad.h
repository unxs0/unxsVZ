/*
FILE
	svn ID removed
AUTHOR
	(C) 2001-2010 Gary Wallis and Hugo Urquiza for Unixservice, LLC. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/file.h>
#include <crypt.h>

#include "cgi.h"
#include <mysql/mysql.h>

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
extern unsigned guCompany;

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

void unxsMail(const char *cResult);
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

void GetConfiguration(const char *cName, char *cValue, unsigned uServer, unsigned uHtml);
unsigned uAllowMod(const unsigned uOwner, const unsigned uCreatedBy);
unsigned uAllowDel(const unsigned uOwner, const unsigned uCreatedBy);
void ExtListSelect(const char *cTable,const char *cVarList);
void ExtSelect(const char *cTable,const char *cVarList,unsigned uMaxResults);
void ExtSelectSearch(const char *cTable,const char *cVarList,const char *cSearchField, const char *cSearch,const char *cExtraCond,unsigned uMaxResults);
void ExtSelectRow(const char *cTable,const char *cVarList,unsigned uRow);


void unxsMailLog(unsigned uTablePK, char *cTableName, char *cLogEntry);
char *cURLEncode(char *cURL);

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

//tUser
int tUserCommands(pentry entries[], int x);
void tUser(const char *results);
void ProcesstUserVars(pentry entries[], int x);
void tUserContent(void);
void tUserInputContent(void);
void tUserInput(unsigned uMode);
void tUserList(void);
void NewtUser(unsigned uMode);
void ModtUser(void);
void CreatetUser(void);
void DeletetUser(void);
void ExttUserGetHook(entry gentries[], int x);
void ExttUserNavBar(void);

//tAlias
int tAliasCommands(pentry entries[], int x);
void tAlias(const char *results);
void ProcesstAliasVars(pentry entries[], int x);
void tAliasContent(void);
void tAliasInputContent(void);
void tAliasInput(unsigned uMode);
void tAliasList(void);
void NewtAlias(unsigned uMode);
void ModtAlias(void);
void CreatetAlias(void);
void DeletetAlias(void);
void ExttAliasGetHook(entry gentries[], int x);
void ExttAliasNavBar(void);

//tDomain
int tDomainCommands(pentry entries[], int x);
void tDomain(const char *results);
void ProcesstDomainVars(pentry entries[], int x);
void tDomainContent(void);
void tDomainInputContent(void);
void tDomainInput(unsigned uMode);
void tDomainList(void);
void NewtDomain(unsigned uMode);
void ModtDomain(void);
void CreatetDomain(void);
void DeletetDomain(void);
void ExttDomainGetHook(entry gentries[], int x);
void ExttDomainNavBar(void);

//tVUT
int tVUTCommands(pentry entries[], int x);
void tVUT(const char *results);
void ProcesstVUTVars(pentry entries[], int x);
void tVUTContent(void);
void tVUTInputContent(void);
void tVUTInput(unsigned uMode);
void tVUTList(void);
void NewtVUT(unsigned uMode);
void ModtVUT(void);
void CreatetVUT(void);
void DeletetVUT(void);
void ExttVUTGetHook(entry gentries[], int x);
void ExttVUTNavBar(void);

//tVUTEntries
int tVUTEntriesCommands(pentry entries[], int x);
void tVUTEntries(const char *results);
void ProcesstVUTEntriesVars(pentry entries[], int x);
void tVUTEntriesContent(void);
void tVUTEntriesInputContent(void);
void tVUTEntriesInput(unsigned uMode);
void tVUTEntriesList(void);
void NewtVUTEntries(unsigned uMode);
void ModtVUTEntries(void);
void CreatetVUTEntries(void);
void DeletetVUTEntries(void);
void ExttVUTEntriesGetHook(entry gentries[], int x);
void ExttVUTEntriesNavBar(void);

//tAccess
int tAccessCommands(pentry entries[], int x);
void tAccess(const char *results);
void ProcesstAccessVars(pentry entries[], int x);
void tAccessContent(void);
void tAccessInputContent(void);
void tAccessInput(unsigned uMode);
void tAccessList(void);
void NewtAccess(unsigned uMode);
void ModtAccess(void);
void CreatetAccess(void);
void DeletetAccess(void);
void ExttAccessGetHook(entry gentries[], int x);
void ExttAccessNavBar(void);

//tRelay
int tRelayCommands(pentry entries[], int x);
void tRelay(const char *results);
void ProcesstRelayVars(pentry entries[], int x);
void tRelayContent(void);
void tRelayInputContent(void);
void tRelayInput(unsigned uMode);
void tRelayList(void);
void NewtRelay(unsigned uMode);
void ModtRelay(void);
void CreatetRelay(void);
void DeletetRelay(void);
void ExttRelayGetHook(entry gentries[], int x);
void ExttRelayNavBar(void);

//tLocal
int tLocalCommands(pentry entries[], int x);
void tLocal(const char *results);
void ProcesstLocalVars(pentry entries[], int x);
void tLocalContent(void);
void tLocalInputContent(void);
void tLocalInput(unsigned uMode);
void tLocalList(void);
void NewtLocal(unsigned uMode);
void ModtLocal(void);
void CreatetLocal(void);
void DeletetLocal(void);
void ExttLocalGetHook(entry gentries[], int x);
void ExttLocalNavBar(void);

//tUserType
int tUserTypeCommands(pentry entries[], int x);
void tUserType(const char *results);
void ProcesstUserTypeVars(pentry entries[], int x);
void tUserTypeContent(void);
void tUserTypeInputContent(void);
void tUserTypeInput(unsigned uMode);
void tUserTypeList(void);
void NewtUserType(unsigned uMode);
void ModtUserType(void);
void CreatetUserType(void);
void DeletetUserType(void);
void ExttUserTypeGetHook(entry gentries[], int x);
void ExttUserTypeNavBar(void);

//tUserConfig
int tUserConfigCommands(pentry entries[], int x);
void tUserConfig(const char *results);
void ProcesstUserConfigVars(pentry entries[], int x);
void tUserConfigContent(void);
void tUserConfigInputContent(void);
void tUserConfigInput(unsigned uMode);
void tUserConfigList(void);
void NewtUserConfig(unsigned uMode);
void ModtUserConfig(void);
void CreatetUserConfig(void);
void DeletetUserConfig(void);
void ExttUserConfigGetHook(entry gentries[], int x);
void ExttUserConfigNavBar(void);

//tServerConfig
int tServerConfigCommands(pentry entries[], int x);
void tServerConfig(const char *results);
void ProcesstServerConfigVars(pentry entries[], int x);
void tServerConfigContent(void);
void tServerConfigInputContent(void);
void tServerConfigInput(unsigned uMode);
void tServerConfigList(void);
void NewtServerConfig(unsigned uMode);
void ModtServerConfig(void);
void CreatetServerConfig(void);
void DeletetServerConfig(void);
void ExttServerConfigGetHook(entry gentries[], int x);
void ExttServerConfigNavBar(void);

//tConfigSpec
int tConfigSpecCommands(pentry entries[], int x);
void tConfigSpec(const char *results);
void ProcesstConfigSpecVars(pentry entries[], int x);
void tConfigSpecContent(void);
void tConfigSpecInputContent(void);
void tConfigSpecInput(unsigned uMode);
void tConfigSpecList(void);
void NewtConfigSpec(unsigned uMode);
void ModtConfigSpec(void);
void CreatetConfigSpec(void);
void DeletetConfigSpec(void);
void ExttConfigSpecGetHook(entry gentries[], int x);
void ExttConfigSpecNavBar(void);

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

//tServerGroup
int tServerGroupCommands(pentry entries[], int x);
void tServerGroup(const char *results);
void ProcesstServerGroupVars(pentry entries[], int x);
void tServerGroupContent(void);
void tServerGroupInputContent(void);
void tServerGroupInput(unsigned uMode);
void tServerGroupList(void);
void NewtServerGroup(unsigned uMode);
void ModtServerGroup(void);
void CreatetServerGroup(void);
void DeletetServerGroup(void);
void ExttServerGroupGetHook(entry gentries[], int x);
void ExttServerGroupNavBar(void);

//tServerGroupGlue
int tServerGroupGlueCommands(pentry entries[], int x);
void tServerGroupGlue(const char *results);
void ProcesstServerGroupGlueVars(pentry entries[], int x);
void tServerGroupGlueContent(void);
void tServerGroupGlueInputContent(void);
void tServerGroupGlueInput(unsigned uMode);
void tServerGroupGlueList(void);
void NewtServerGroupGlue(unsigned uMode);
void ModtServerGroupGlue(void);
void CreatetServerGroupGlue(void);
void DeletetServerGroupGlue(void);
void ExttServerGroupGlueGetHook(entry gentries[], int x);
void ExttServerGroupGlueNavBar(void);

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

//tHoldUser
int tHoldUserCommands(pentry entries[], int x);
void tHoldUser(const char *results);
void ProcesstHoldUserVars(pentry entries[], int x);
void tHoldUserContent(void);
void tHoldUserInputContent(void);
void tHoldUserInput(unsigned uMode);
void tHoldUserList(void);
void NewtHoldUser(unsigned uMode);
void ModtHoldUser(void);
void CreatetHoldUser(void);
void DeletetHoldUser(void);
void ExttHoldUserGetHook(entry gentries[], int x);
void ExttHoldUserNavBar(void);

//tRequest
int tRequestCommands(pentry entries[], int x);
void tRequest(const char *results);
void ProcesstRequestVars(pentry entries[], int x);
void tRequestContent(void);
void tRequestInputContent(void);
void tRequestInput(unsigned uMode);
void tRequestList(void);
void NewtRequest(unsigned uMode);
void ModtRequest(void);
void CreatetRequest(void);
void DeletetRequest(void);
void ExttRequestGetHook(entry gentries[], int x);
void ExttRequestNavBar(void);

//tWhiteList
int tWhiteListCommands(pentry entries[], int x);
void tWhiteList(const char *results);
void ProcesstWhiteListVars(pentry entries[], int x);
void tWhiteListContent(void);
void tWhiteListInputContent(void);
void tWhiteListInput(unsigned uMode);
void tWhiteListList(void);
void NewtWhiteList(unsigned uMode);
void ModtWhiteList(void);
void CreatetWhiteList(void);
void DeletetWhiteList(void);
void ExttWhiteListGetHook(entry gentries[], int x);
void ExttWhiteListNavBar(void);

//tBlackList
int tBlackListCommands(pentry entries[], int x);
void tBlackList(const char *results);
void ProcesstBlackListVars(pentry entries[], int x);
void tBlackListContent(void);
void tBlackListInputContent(void);
void tBlackListInput(unsigned uMode);
void tBlackListList(void);
void NewtBlackList(unsigned uMode);
void ModtBlackList(void);
void CreatetBlackList(void);
void DeletetBlackList(void);
void ExttBlackListGetHook(entry gentries[], int x);
void ExttBlackListNavBar(void);

//tVacation
int tVacationCommands(pentry entries[], int x);
void tVacation(const char *results);
void ProcesstVacationVars(pentry entries[], int x);
void tVacationContent(void);
void tVacationInputContent(void);
void tVacationInput(unsigned uMode);
void tVacationList(void);
void NewtVacation(unsigned uMode);
void ModtVacation(void);
void CreatetVacation(void);
void DeletetVacation(void);
void ExttVacationGetHook(entry gentries[], int x);
void ExttVacationNavBar(void);

//In-line code macros

//Comon - This macro shouldn't be use directly, as is part of the others only
#define macro_mySQLQueryBasic mysql_query(&gMysql,gcQuery);\
				if(mysql_errno(&gMysql))

//MySQL run query only w/error checking
//HTML
#define macro_mySQLQueryHTMLError macro_mySQLQueryBasic \
					htmlPlainTextError(mysql_error(&gMysql))
//Text
#define macro_mySQLQueryTextError macro_mySQLQueryBasic\
					{\
						fprintf(stderr,"%s\n",mysql_error(&gMysql));\
						exit(1);\
					}

//Text with return() instead of exit()
//return(1); if MySQL error
#define macro_mySQLRunReturnInt macro_mySQLQueryBasic\
				{\
					fprintf(stderr,"%s\n",mysql_error(&gMysql));\
					return(1);\
				}
//return void; if MySQL error
#define macro_mySQLRunReturnVoid macro_mySQLQueryBasic\
				{\
					fprintf(stderr,"%s\n",mysql_error(&gMysql));\
					return;\
				}

//MySQL run query and store result w/error checking
//HTML
#define macro_mySQLRunAndStore(res) macro_mySQLQueryHTMLError;\
					res=mysql_store_result(&gMysql)
//Text
#define macro_mySQLRunAndStoreText(res) macro_mySQLQueryBasic;\
					res=mysql_store_result(&gMysql)

//MySQL run query and store result w/error checking (Text); uses return() call instead of exit()
//return(1); if MySQL error
#define macro_mySQLRunAndStoreTextIntRet(res) macro_mySQLRunReturnInt;\
						res=mysql_store_result(&gMysql)
//return; if MySQL error
#define macro_mySQLRunAndStoreTextVoidRet(res) macro_mySQLRunReturnVoid;\
						res=mysql_store_result(&gMysql)

