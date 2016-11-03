/*
FILE
	svn ID removed
AUTHOR
	(C) 2001-2008 Gary Wallis and Hugo Urquiza. 
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

 //Native MySQL fields
#define COLTYPE_DECIMAL 0
#define COLTYPE_MONEY 1102 //For BCD in future. Now allows $250,000.00 style input and output.
#define COLTYPE_INTUNSIGNED 3
#define COLTYPE_TIMESTAMP 7
#define COLTYPE_DATETIME 12
#define COLTYPE_CHAR 254
#define COLTYPE_VARCHAR 253
#define COLTYPE_TEXT 252
#define COLTYPE_IMAGE 1101
#define COLTYPE_FKIMAGE 1200
#define COLTYPE_BIGINT 8

 //Our special gui fields
#define COLTYPE_SELECTTABLE 1000
#define COLTYPE_SELECTTABLE_OWNER 1100
#define COLTYPE_RADPRI 1001
#define COLTYPE_PRIKEY 1002
#define COLTYPE_YESNO 1003
#define COLTYPE_UNIXTIMECREATE 1004
#define COLTYPE_UNIXTIMEUPDATE 1005
#define COLTYPE_EXTFUNC 1006
#define COLTYPE_FOREIGNKEY 1007
#define COLTYPE_UINTUKEY 1008
#define COLTYPE_VARCHARUKEY 1009
#define COLTYPE_UNIXTIME 2000

extern char gcHost[];
extern char gcHostname[];
extern char gcUser[];
#define PERMLEVEL
extern int guPermLevel; 
extern unsigned guLoginClient; 
extern unsigned guReseller; 
extern unsigned guCompany; 
extern unsigned guCookieProject,guCookieTable,guCookieField;
void SetSessionCookie(void);

extern char gcFunction[];
extern unsigned guListMode; 
extern char gcQuery[];
extern char *gcQstr;
extern char *gcBuildInfo;
extern char gcRADStatus[];
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

void unxsRAD(const char *cResult);
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
void TextError(const char *cError, unsigned uContinue);
unsigned uAllowMod(const unsigned uOwner, const unsigned uCreatedBy);
unsigned uAllowDel(const unsigned uOwner, const unsigned uCreatedBy);

void unxsRADLog(unsigned uTablePK, char *cTableName, char *cLogEntry);

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

//tProject
int tProjectCommands(pentry entries[], int x);
void tProject(const char *results);
void ProcesstProjectVars(pentry entries[], int x);
void tProjectContent(void);
void tProjectInputContent(void);
void tProjectInput(unsigned uMode);
void tProjectList(void);
void NewtProject(unsigned uMode);
void ModtProject(void);
void CreatetProject(void);
void DeletetProject(void);
void ExttProjectGetHook(entry gentries[], int x);
void ExttProjectNavBar(void);
void GetConfiguration(const char *cName,char *cValue,unsigned uValueSize, unsigned uServer, unsigned uHtml);

//tTable
int tTableCommands(pentry entries[], int x);
void tTable(const char *results);
void ProcesstTableVars(pentry entries[], int x);
void tTableContent(void);
void tTableInputContent(void);
void tTableInput(unsigned uMode);
void tTableList(void);
void NewtTable(unsigned uMode);
void ModtTable(void);
void CreatetTable(void);
void DeletetTable(void);
void ExttTableGetHook(entry gentries[], int x);
void ExttTableNavBar(void);

//tField
int tFieldCommands(pentry entries[], int x);
void tField(const char *results);
void ProcesstFieldVars(pentry entries[], int x);
void tFieldContent(void);
void tFieldInputContent(void);
void tFieldInput(unsigned uMode);
void tFieldList(void);
void NewtField(unsigned uMode);
void ModtField(void);
void CreatetField(void);
void DeletetField(void);
void ExttFieldGetHook(entry gentries[], int x);
void ExttFieldNavBar(void);

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

//tProjectStatus
int tProjectStatusCommands(pentry entries[], int x);
void tProjectStatus(const char *results);
void ProcesstProjectStatusVars(pentry entries[], int x);
void tProjectStatusContent(void);
void tProjectStatusInputContent(void);
void tProjectStatusInput(unsigned uMode);
void tProjectStatusList(void);
void NewtProjectStatus(unsigned uMode);
void ModtProjectStatus(void);
void CreatetProjectStatus(void);
void DeletetProjectStatus(void);
void ExttProjectStatusGetHook(entry gentries[], int x);
void ExttProjectStatusNavBar(void);

//tFieldType
int tFieldTypeCommands(pentry entries[], int x);
void tFieldType(const char *results);
void ProcesstFieldTypeVars(pentry entries[], int x);
void tFieldTypeContent(void);
void tFieldTypeInputContent(void);
void tFieldTypeInput(unsigned uMode);
void tFieldTypeList(void);
void NewtFieldType(unsigned uMode);
void ModtFieldType(void);
void CreatetFieldType(void);
void DeletetFieldType(void);
void ExttFieldTypeGetHook(entry gentries[], int x);
void ExttFieldTypeNavBar(void);

//tIndexType
int tIndexTypeCommands(pentry entries[], int x);
void tIndexType(const char *results);
void ProcesstIndexTypeVars(pentry entries[], int x);
void tIndexTypeContent(void);
void tIndexTypeInputContent(void);
void tIndexTypeInput(unsigned uMode);
void tIndexTypeList(void);
void NewtIndexType(unsigned uMode);
void ModtIndexType(void);
void CreatetIndexType(void);
void DeletetIndexType(void);
void ExttIndexTypeGetHook(entry gentries[], int x);
void ExttIndexTypeNavBar(void);

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



//In-line code macros

//Comon - This macro shouldn't be used directly, as is part of the others only
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

