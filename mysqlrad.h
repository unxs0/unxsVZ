/*
FILE
	unxsVZ/mysqlrad.h
AUTHOR
	(C) 2001-2013 Gary Wallis for Unixservice, LLC.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <crypt.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/file.h>

#include "cgi.h"

#include <mysql/mysql.h>
#include <liboath/oath.h>

#include <unistd.h>
#include <locale.h>
#include <monetary.h>

#include "language.h"
#include "local.h"

//tIP uFWStatus codes
#define uFWWAITINGBLOCK 1
#define uFWWAITINGACCESS 2
#define uFWBLOCKED 3
#define uFWACCESS 4
#define uFWREMOVED 5
#define uFWWAITINGREMOVAL 6
#define uFWWHITELISTED 7


//tStatus constants
#define uACTIVE		1
#define uONHOLD		2
#define uOFFLINE	3
#define uAWAITMOD	4
#define uAWAITDEL	5
#define uAWAITACT	6
#define uCANCELED	7
#define uMODIFIED	8
#define uMODPROB	9
#define uINITSETUP	11
#define uAWAITMIG	21
#define uSTOPPED	31
#define uAWAITSTOP	41
#define uAWAITTML	51
#define uAWAITHOST	61
#define uAWAITIP	71
#define uAWAITCLONE	81
#define uAWAITFAIL	91
#define uREMOTEAPPLIANCE 101
#define uAWAITRESTART	111
#define uAWAITDNSMIG	121

//tProperty fixed types aka constants
//Actually tType constants
#define PROP_DATACENTER "1"
#define PROP_NODE "2"
#define PROP_CONTAINER "3"
#define PROP_GROUP "4"
#define PROP_GROUPTYPE "5"
#define PROP_CONFIG "6"
#define PROP_NAMESERVER "7"
#define PROP_OSTEMPLATE "8"
#define PROP_SEARCHDOMAIN "9"
#define PROP_IP "31"
#define PROP_CLIENT "41"
#define PROP_AUTHORIZE "42"
#define uPROP_DATACENTER 1
#define uPROP_NODE 2
#define uPROP_CONTAINER 3
#define uPROP_GROUP 4
#define uPROP_GROUPTYPE 5
#define uPROP_CONFIG 6
#define uPROP_NAMESERVER 7
#define uPROP_OSTEMPLATE 8
#define uPROP_SEARCHDOMAIN 9
#define uPROP_RECJOB	10	
#define uPROP_IP	31
#define uPROP_CLIENT	41
#define uPROP_AUTHORIZE	42

//tJobStatus constants
#define uWAITING 	1
#define uRUNNING 	2
#define uDONEOK		3
#define uDONEERROR	4
#define uSUSPENDED	5
#define uREDUNDANT	6
#define uCANCELED	7
#define uREMOTEWAITING	10
#define uERROR		14


#define cLOGFILE "/var/log/unxsvzlog"

extern char gcHost[];
extern char gcHostname[];
extern char gcUser[];
extern int guPermLevel; 
extern unsigned guLoginClient; 
extern unsigned guReseller; 
extern unsigned guCompany;

extern FILE *gLfp;
extern char gcFunction[];
extern unsigned guListMode; 
extern char gcQuery[];
extern char *gcQstr;
extern char *gcBuildInfo;
extern char *gcRADStatus;
extern MYSQL gMysql; 
extern MYSQL gMysqlUBC;
extern char *gcUBCDBIP0;
extern char *gcUBCDBIP1;
extern char gcUBCDBIP0Buffer[];
extern char gcUBCDBIP1Buffer[];

extern unsigned long gluRowid;
extern unsigned guStart;
extern unsigned guEnd;
extern unsigned guI;
extern unsigned guN;
extern char gcCommand[];
extern char gcFilter[];
extern char gcAuxFilter[];
extern char gcFind[];
extern char gcTable[];
extern unsigned guMode;
extern int guError;
extern char gcErrormsg[];

extern pentry entries[];
extern entry gentries[];
extern int x;

void unxsVZ(const char *cResult);
void ConnectDb(char *cMessage);
unsigned TextConnectDb(void);
unsigned ConnectDbUBC(void);
void logfileLine(const char *cFunction,const char *cLogline);
void Footer_ism3(void);
void Header_ism3(const char *cMsg, int iJs);
void ProcessControlVars(pentry entries[], int x);
void OpenRow(const char *cFieldLabel, const char *cColor);
void OpenFieldSet(char *cLabel, unsigned uWidth);
void CloseFieldSet(void);
void LoadConf(void);
void NoSuchFunction(void);
void tContainerGroupPullDown(unsigned uGroup, unsigned uMode, char *cSelectName);
void tTablePullDown(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode);
void tTablePullDownActive(const char *cTableName, const char *cFieldName,
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
const char *ForeignKey2(const char *cTableName, const char *cFieldName, unsigned uKey);
void GetClientOwner(unsigned uClient, unsigned *uOwner);
void ExtMainShell(int argc, char *argv[]);
void jsCalendarInput(char *cInputName,char *cValue,unsigned uMode);
long unsigned luGetModDate(char *cTableName, unsigned uTablePK);
long unsigned luGetCreatedDate(char *cTableName, unsigned uTablePK);
void DashBoard(const char *cOptionalMsg);
void htmlPlainTextError(const char *cError);
unsigned uAllowMod(const unsigned uOwner, const unsigned uCreatedBy);
unsigned uAllowDel(const unsigned uOwner, const unsigned uCreatedBy);
void ExtListSelect(const char *cTable,const char *cVarList);
void ExtSelect(const char *cTable,const char *cVarList);
void ExtSelectSearch(const char *cTable,const char *cVarList,const char *cSearchField,const char *cSearch);
void ExtSelectRow(const char *cTable,const char *cVarList,unsigned uRow);
void ExtListSelectPublic(const char *cTable,const char *cVarList);
void ExtSelectPublic(const char *cTable,const char *cVarList);
void ExtSelectPublicOrder(const char *cTable,const char *cVarList,const char *cOrderBy);
void ExtSelectSearchPublic(const char *cTable,const char *cVarList,const char *cSearchField,const char *cSearch);
void ExtSelectRowPublic(const char *cTable,const char *cVarList,unsigned uRow);
void CloneReport(const char *cOptionalMsg);
void SetNodeProp(char const *cName,char const *cValue,unsigned uNode);
void SetDatacenterProp(char const *cName,char const *cValue,unsigned uDatacenter);
unsigned SetContainerProperty(const unsigned uContainer,const char *cPropertyName,const  char *cPropertyValue);
void tTablePullDownResellers(unsigned uSelector,unsigned uBanner);
unsigned GetConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer,
		unsigned uHtml);
void tTablePullDownAvail(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode);
void tTablePullDownOwnerAvail(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode);
void tTablePullDownDatacenter(const char *cTableName, const char *cFieldName,
		const char *cOrderby, unsigned uSelector, unsigned uMode, const char *cDatacenter,
		unsigned uType, unsigned uDatacenter,unsigned uContainerType);
void tTablePullDownOwnerAvailDatacenter(const char *cTableName, const char *cFieldName,
	const char *cOrderby, unsigned uSelector, unsigned uMode,unsigned uDatacenter,unsigned uClient);

char *cURLEncode(char *cURL);
void unxsVZLog(unsigned uTablePK, char *cTableName, char *cLogEntry);
unsigned ConnectToOptionalUBCDb(unsigned uDatacenter,unsigned uPrivate);//tnodefunc.h

 //Standard tInputFunc functions
char *WordToLower(char *cInput);
char *ToLower(char *cInput);
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

//tDatacenter
int tDatacenterCommands(pentry entries[], int x);
void tDatacenter(const char *results);
void ProcesstDatacenterVars(pentry entries[], int x);
void tDatacenterContent(void);
void tDatacenterInputContent(void);
void tDatacenterInput(unsigned uMode);
void tDatacenterList(void);
void NewtDatacenter(unsigned uMode);
void ModtDatacenter(void);
void CreatetDatacenter(void);
void DeletetDatacenter(void);
void ExttDatacenterGetHook(entry gentries[], int x);
void ExttDatacenterNavBar(void);

//tNode
int tNodeCommands(pentry entries[], int x);
void tNode(const char *results);
void ProcesstNodeVars(pentry entries[], int x);
void tNodeContent(void);
void tNodeInputContent(void);
void tNodeInput(unsigned uMode);
void tNodeList(void);
void NewtNode(unsigned uMode);
void ModtNode(void);
void CreatetNode(void);
void DeletetNode(void);
void ExttNodeGetHook(entry gentries[], int x);
void ExttNodeNavBar(void);

//tContainer
int tContainerCommands(pentry entries[], int x);
void tContainer(const char *results);
void ProcesstContainerVars(pentry entries[], int x);
void tContainerContent(void);
void tContainerInputContent(void);
void tContainerInput(unsigned uMode);
void tContainerList(void);
void NewtContainer(unsigned uMode);
void ModtContainer(void);
void CreatetContainer(void);
void DeletetContainer(void);
void ExttContainerGetHook(entry gentries[], int x);
void ExttContainerNavBar(void);
unsigned CommonCloneContainer(
		unsigned uContainer,
		unsigned uOSTemplate,
		unsigned uConfig,
		unsigned uNameserver,
		unsigned uSearchdomain,
		unsigned uDatacenter,
		unsigned uTargetDatacenter,
		unsigned uOwner,
		const char *cLabel,
		unsigned uNode,
		unsigned uStatus,
		const char *cHostname,
		const char *cClassC,
		unsigned uWizIPv4,
		char *cWizLabel,
		char *cWizHostname,
		unsigned uTargetNode,
		unsigned uSyncPeriod,
		unsigned uLoginClient,
		unsigned uCloneStop,
		unsigned uMode);
void CopyContainerProps(unsigned uSource, unsigned uTarget);
unsigned CloneContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,
				unsigned uTargetNode, unsigned uNewVeid, unsigned uPrevStatus,
				unsigned uOwner,unsigned uCreatedBy,unsigned uCloneStop);
unsigned CreateDNSJob(unsigned uIPv4,unsigned uOwner,char const *cOptionalIPv4,char const *cHostname,
				unsigned uDatacenter,unsigned uCreatedBy,unsigned uContainer,unsigned uNode);

//tProperty
int tPropertyCommands(pentry entries[], int x);
void tProperty(const char *results);
void ProcesstPropertyVars(pentry entries[], int x);
void tPropertyContent(void);
void tPropertyInputContent(void);
void tPropertyInput(unsigned uMode);
void tPropertyList(void);
void NewtProperty(unsigned uMode);
void ModtProperty(void);
void CreatetProperty(void);
void DeletetProperty(void);
void ExttPropertyGetHook(entry gentries[], int x);
void ExttPropertyNavBar(void);

//tType
int tTypeCommands(pentry entries[], int x);
void tType(const char *results);
void ProcesstTypeVars(pentry entries[], int x);
void tTypeContent(void);
void tTypeInputContent(void);
void tTypeInput(unsigned uMode);
void tTypeList(void);
void NewtType(unsigned uMode);
void ModtType(void);
void CreatetType(void);
void DeletetType(void);
void ExttTypeGetHook(entry gentries[], int x);
void ExttTypeNavBar(void);

//tContainerType
int tContainerTypeCommands(pentry entries[], int x);
void tContainerType(const char *results);
void ProcesstContainerTypeVars(pentry entries[], int x);
void tContainerTypeContent(void);
void tContainerTypeInputContent(void);
void tContainerTypeInput(unsigned uMode);
void tContainerTypeList(void);
void NewtContainerType(unsigned uMode);
void ModtContainerType(void);
void CreatetContainerType(void);
void DeletetContainerType(void);
void ExttContainerTypeGetHook(entry gentries[], int x);
void ExttContainerTypeNavBar(void);

//tPerm
int tPermCommands(pentry entries[], int x);
void tPerm(const char *results);
void ProcesstPermVars(pentry entries[], int x);
void tPermContent(void);
void tPermInputContent(void);
void tPermInput(unsigned uMode);
void tPermList(void);
void NewtPerm(unsigned uMode);
void ModtPerm(void);
void CreatetPerm(void);
void DeletetPerm(void);
void ExttPermGetHook(entry gentries[], int x);
void ExttPermNavBar(void);

//tOSTemplate
int tOSTemplateCommands(pentry entries[], int x);
void tOSTemplate(const char *results);
void ProcesstOSTemplateVars(pentry entries[], int x);
void tOSTemplateContent(void);
void tOSTemplateInputContent(void);
void tOSTemplateInput(unsigned uMode);
void tOSTemplateList(void);
void NewtOSTemplate(unsigned uMode);
void ModtOSTemplate(void);
void CreatetOSTemplate(void);
void DeletetOSTemplate(void);
void ExttOSTemplateGetHook(entry gentries[], int x);
void ExttOSTemplateNavBar(void);

//tNameserver
int tNameserverCommands(pentry entries[], int x);
void tNameserver(const char *results);
void ProcesstNameserverVars(pentry entries[], int x);
void tNameserverContent(void);
void tNameserverInputContent(void);
void tNameserverInput(unsigned uMode);
void tNameserverList(void);
void NewtNameserver(unsigned uMode);
void ModtNameserver(void);
void CreatetNameserver(void);
void DeletetNameserver(void);
void ExttNameserverGetHook(entry gentries[], int x);
void ExttNameserverNavBar(void);

//tSearchdomain
int tSearchdomainCommands(pentry entries[], int x);
void tSearchdomain(const char *results);
void ProcesstSearchdomainVars(pentry entries[], int x);
void tSearchdomainContent(void);
void tSearchdomainInputContent(void);
void tSearchdomainInput(unsigned uMode);
void tSearchdomainList(void);
void NewtSearchdomain(unsigned uMode);
void ModtSearchdomain(void);
void CreatetSearchdomain(void);
void DeletetSearchdomain(void);
void ExttSearchdomainGetHook(entry gentries[], int x);
void ExttSearchdomainNavBar(void);

//tConfig
int tConfigCommands(pentry entries[], int x);
void tConfig(const char *results);
void ProcesstConfigVars(pentry entries[], int x);
void tConfigContent(void);
void tConfigInputContent(void);
void tConfigInput(unsigned uMode);
void tConfigList(void);
void NewtConfig(unsigned uMode);
void ModtConfig(void);
void CreatetConfig(void);
void DeletetConfig(void);
void ExttConfigGetHook(entry gentries[], int x);
void ExttConfigNavBar(void);

//tIP
int tIPCommands(pentry entries[], int x);
void tIP(const char *results);
void ProcesstIPVars(pentry entries[], int x);
void tIPContent(void);
void tIPInputContent(void);
void tIPInput(unsigned uMode);
void tIPList(void);
void NewtIP(unsigned uMode);
void ModtIP(void);
void CreatetIP(void);
void DeletetIP(void);
void ExttIPGetHook(entry gentries[], int x);
void ExttIPNavBar(void);

//tGroupType
int tGroupTypeCommands(pentry entries[], int x);
void tGroupType(const char *results);
void ProcesstGroupTypeVars(pentry entries[], int x);
void tGroupTypeContent(void);
void tGroupTypeInputContent(void);
void tGroupTypeInput(unsigned uMode);
void tGroupTypeList(void);
void NewtGroupType(unsigned uMode);
void ModtGroupType(void);
void CreatetGroupType(void);
void DeletetGroupType(void);
void ExttGroupTypeGetHook(entry gentries[], int x);
void ExttGroupTypeNavBar(void);

//tGroup
int tGroupCommands(pentry entries[], int x);
void tGroup(const char *results);
void ProcesstGroupVars(pentry entries[], int x);
void tGroupContent(void);
void tGroupInputContent(void);
void tGroupInput(unsigned uMode);
void tGroupList(void);
void NewtGroup(unsigned uMode);
void ModtGroup(void);
void CreatetGroup(void);
void DeletetGroup(void);
void ExttGroupGetHook(entry gentries[], int x);
void ExttGroupNavBar(void);

//tGroupGlue
int tGroupGlueCommands(pentry entries[], int x);
void tGroupGlue(const char *results);
void ProcesstGroupGlueVars(pentry entries[], int x);
void tGroupGlueContent(void);
void tGroupGlueInputContent(void);
void tGroupGlueInput(unsigned uMode);
void tGroupGlueList(void);
void NewtGroupGlue(unsigned uMode);
void ModtGroupGlue(void);
void CreatetGroupGlue(void);
void DeletetGroupGlue(void);
void ExttGroupGlueGetHook(entry gentries[], int x);
void ExttGroupGlueNavBar(void);

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

//tFWStatus
int tFWStatusCommands(pentry entries[], int x);
void tFWStatus(const char *results);
void ProcesstFWStatusVars(pentry entries[], int x);
void tFWStatusContent(void);
void tFWStatusInputContent(void);
void tFWStatusInput(unsigned uMode);
void tFWStatusList(void);
void NewtFWStatus(unsigned uMode);
void ModtFWStatus(void);
void CreatetFWStatus(void);
void DeletetFWStatus(void);
void ExttFWStatusGetHook(entry gentries[], int x);
void ExttFWStatusNavBar(void);

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
unsigned unxsBindARecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
		unsigned uOwner,unsigned uCreatedBy);
unsigned unxsBindPBXRecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
		unsigned uOwner,unsigned uCreatedBy);

//glossary.c
void GlossaryGetHook(entry gentries[],int x);


//In-line code macros

//Common
#define _RUN_QUERY mysql_query(&gMysql,gcQuery);if(mysql_errno(&gMysql))

//MySQL run query only w/error checking
//HTML
#define MYSQL_RUN _RUN_QUERY htmlPlainTextError(mysql_error(&gMysql))
//Text
#define MYSQL_RUN_TEXT _RUN_QUERY{fprintf(stderr,"%s\n",mysql_error(&gMysql));exit(1);}

//Text with return() instead of exit()
//return(1); if MySQL error
#define MYSQL_RUN_TEXT_RETURN _RUN_QUERY{fprintf(stderr,"%s\n",mysql_error(&gMysql));return(1);}
//return; if MySQL error
#define MYSQL_RUN_TEXT_RET_VOID _RUN_QUERY{fprintf(stderr,"%s\n",mysql_error(&gMysql));return;}

//MySQL run query and store result w/error checking
//HTML
#define MYSQL_RUN_STORE(res) MYSQL_RUN;res=mysql_store_result(&gMysql)
//Text
#define MYSQL_RUN_STORE_TEXT(res) MYSQL_RUN_TEXT;res=mysql_store_result(&gMysql)

//MySQL run query and store result w/error checking (Text); uses return() call instead of exit()
//return(1); if MySQL error
#define MYSQL_RUN_STORE_TEXT_RETURN(res) MYSQL_RUN_TEXT_RETURN res=mysql_store_result(&gMysql)
//return; if MySQL error
#define MYSQL_RUN_STORE_TEXT_RET_VOID(res) MYSQL_RUN_TEXT_RET_VOID res=mysql_store_result(&gMysql)

//In-line code macros rev 2

//Common - This macro shouldn't be used directly, as is part of the others only
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
#define macro_mySQLRunAndStoreText(res) macro_mySQLQueryTextError;\
					res=mysql_store_result(&gMysql)

//MySQL run query and store result w/error checking (Text); uses return() call instead of exit()
//return(1); if MySQL error
#define macro_mySQLRunAndStoreTextIntRet(res) macro_mySQLRunReturnInt;\
						res=mysql_store_result(&gMysql)
//return; if MySQL error
#define macro_mySQLRunAndStoreTextVoidRet(res) macro_mySQLRunReturnVoid;\
						res=mysql_store_result(&gMysql)

//Backup container constants
#define HOT_CLONE 0
#define WARM_CLONE 1
#define COLD_CLONE 2

//Table Variables


void GetContainerProp(const unsigned uContainer,const char *cName,char *cValue);
unsigned UpdatePrimaryContainerGroup(unsigned uContainer, unsigned uGroup);
unsigned uGetGroup(unsigned uNode, unsigned uContainer);
