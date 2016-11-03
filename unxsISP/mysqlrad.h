/*
FILE
	svn ID removed
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza. 
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

void unxsISP(const char *cResult);
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
char *cURLEncode(char *cURL);
unsigned uAllowMod(const unsigned uOwner, const unsigned uCreatedBy);
unsigned uAllowDel(const unsigned uOwner, const unsigned uCreatedBy);
void ExtListSelect(const char *cTable,const char *cVarList);
void ExtSelect(const char *cTable,const char *cVarList,unsigned uMaxResults);
void ExtSelectSearch(const char *cTable,const char *cVarList,const char *cSearchField, const char *cSearch,const char *cExtraCond,unsigned uMaxResults);
void ExtSelectRow(const char *cTable,const char *cVarList,unsigned uRow);
void ExtListSelectAdmin(const char *cTable,const char *cVarList);
void ExtSelectAdmin(const char *cTable,const char *cVarList,unsigned uMaxResults);
void ExtSelectSearchAdmin(const char *cTable,const char *cVarList,const char *cSearchField,
				const char *cSearch,const char *cExtraCond,unsigned uMaxResults);
void ExtSelectRowAdmin(const char *cTable,const char *cVarList,unsigned uRow);


void unxsISPLog(unsigned uTablePK, char *cTableName, char *cLogEntry);

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

//tProduct
int tProductCommands(pentry entries[], int x);
void tProduct(const char *results);
void ProcesstProductVars(pentry entries[], int x);
void tProductContent(void);
void tProductInputContent(void);
void tProductInput(unsigned uMode);
void tProductList(void);
void NewtProduct(unsigned uMode);
void ModtProduct(void);
void CreatetProduct(void);
void DeletetProduct(void);
void ExttProductGetHook(entry gentries[], int x);
void ExttProductNavBar(void);

//tService
int tServiceCommands(pentry entries[], int x);
void tService(const char *results);
void ProcesstServiceVars(pentry entries[], int x);
void tServiceContent(void);
void tServiceInputContent(void);
void tServiceInput(unsigned uMode);
void tServiceList(void);
void NewtService(unsigned uMode);
void ModtService(void);
void CreatetService(void);
void DeletetService(void);
void ExttServiceGetHook(entry gentries[], int x);
void ExttServiceNavBar(void);

//tInstance
int tInstanceCommands(pentry entries[], int x);
void tInstance(const char *results);
void ProcesstInstanceVars(pentry entries[], int x);
void tInstanceContent(void);
void tInstanceInputContent(void);
void tInstanceInput(unsigned uMode);
void tInstanceList(void);
void NewtInstance(unsigned uMode);
void ModtInstance(void);
void CreatetInstance(void);
void DeletetInstance(void);
void ExttInstanceGetHook(entry gentries[], int x);
void ExttInstanceNavBar(void);

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

//tClientConfig
int tClientConfigCommands(pentry entries[], int x);
void tClientConfig(const char *results);
void ProcesstClientConfigVars(pentry entries[], int x);
void tClientConfigContent(void);
void tClientConfigInputContent(void);
void tClientConfigInput(unsigned uMode);
void tClientConfigList(void);
void NewtClientConfig(unsigned uMode);
void ModtClientConfig(void);
void CreatetClientConfig(void);
void DeletetClientConfig(void);
void ExttClientConfigGetHook(entry gentries[], int x);
void ExttClientConfigNavBar(void);

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

//tParameter
int tParameterCommands(pentry entries[], int x);
void tParameter(const char *results);
void ProcesstParameterVars(pentry entries[], int x);
void tParameterContent(void);
void tParameterInputContent(void);
void tParameterInput(unsigned uMode);
void tParameterList(void);
void NewtParameter(unsigned uMode);
void ModtParameter(void);
void CreatetParameter(void);
void DeletetParameter(void);
void ExttParameterGetHook(entry gentries[], int x);
void ExttParameterNavBar(void);

//tParamType
int tParamTypeCommands(pentry entries[], int x);
void tParamType(const char *results);
void ProcesstParamTypeVars(pentry entries[], int x);
void tParamTypeContent(void);
void tParamTypeInputContent(void);
void tParamTypeInput(unsigned uMode);
void tParamTypeList(void);
void NewtParamType(unsigned uMode);
void ModtParamType(void);
void CreatetParamType(void);
void DeletetParamType(void);
void ExttParamTypeGetHook(entry gentries[], int x);
void ExttParamTypeNavBar(void);

//tPeriod
int tPeriodCommands(pentry entries[], int x);
void tPeriod(const char *results);
void ProcesstPeriodVars(pentry entries[], int x);
void tPeriodContent(void);
void tPeriodInputContent(void);
void tPeriodInput(unsigned uMode);
void tPeriodList(void);
void NewtPeriod(unsigned uMode);
void ModtPeriod(void);
void CreatetPeriod(void);
void DeletetPeriod(void);
void ExttPeriodGetHook(entry gentries[], int x);
void ExttPeriodNavBar(void);

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

//tServiceGlue
int tServiceGlueCommands(pentry entries[], int x);
void tServiceGlue(const char *results);
void ProcesstServiceGlueVars(pentry entries[], int x);
void tServiceGlueContent(void);
void tServiceGlueInputContent(void);
void tServiceGlueInput(unsigned uMode);
void tServiceGlueList(void);
void NewtServiceGlue(unsigned uMode);
void ModtServiceGlue(void);
void CreatetServiceGlue(void);
void DeletetServiceGlue(void);
void ExttServiceGlueGetHook(entry gentries[], int x);
void ExttServiceGlueNavBar(void);

//tInvoice
int tInvoiceCommands(pentry entries[], int x);
void tInvoice(const char *results);
void ProcesstInvoiceVars(pentry entries[], int x);
void tInvoiceContent(void);
void tInvoiceInputContent(void);
void tInvoiceInput(unsigned uMode);
void tInvoiceList(void);
void NewtInvoice(unsigned uMode);
void ModtInvoice(void);
void CreatetInvoice(void);
void DeletetInvoice(void);
void ExttInvoiceGetHook(entry gentries[], int x);
void ExttInvoiceNavBar(void);

//tInvoiceMonth
int tInvoiceMonthCommands(pentry entries[], int x);
void tInvoiceMonth(const char *results);
void ProcesstInvoiceMonthVars(pentry entries[], int x);
void tInvoiceMonthContent(void);
void tInvoiceMonthInputContent(void);
void tInvoiceMonthInput(unsigned uMode);
void tInvoiceMonthList(void);
void NewtInvoiceMonth(unsigned uMode);
void ModtInvoiceMonth(void);
void CreatetInvoiceMonth(void);
void DeletetInvoiceMonth(void);
void ExttInvoiceMonthGetHook(entry gentries[], int x);
void ExttInvoiceMonthNavBar(void);

//tPaid
int tPaidCommands(pentry entries[], int x);
void tPaid(const char *results);
void ProcesstPaidVars(pentry entries[], int x);
void tPaidContent(void);
void tPaidInputContent(void);
void tPaidInput(unsigned uMode);
void tPaidList(void);
void NewtPaid(unsigned uMode);
void ModtPaid(void);
void CreatetPaid(void);
void DeletetPaid(void);
void ExttPaidGetHook(entry gentries[], int x);
void ExttPaidNavBar(void);

//tInvoiceItems
int tInvoiceItemsCommands(pentry entries[], int x);
void tInvoiceItems(const char *results);
void ProcesstInvoiceItemsVars(pentry entries[], int x);
void tInvoiceItemsContent(void);
void tInvoiceItemsInputContent(void);
void tInvoiceItemsInput(unsigned uMode);
void tInvoiceItemsList(void);
void NewtInvoiceItems(unsigned uMode);
void ModtInvoiceItems(void);
void CreatetInvoiceItems(void);
void DeletetInvoiceItems(void);
void ExttInvoiceItemsGetHook(entry gentries[], int x);
void ExttInvoiceItemsNavBar(void);

//tInvoiceStatus
int tInvoiceStatusCommands(pentry entries[], int x);
void tInvoiceStatus(const char *results);
void ProcesstInvoiceStatusVars(pentry entries[], int x);
void tInvoiceStatusContent(void);
void tInvoiceStatusInputContent(void);
void tInvoiceStatusInput(unsigned uMode);
void tInvoiceStatusList(void);
void NewtInvoiceStatus(unsigned uMode);
void ModtInvoiceStatus(void);
void CreatetInvoiceStatus(void);
void DeletetInvoiceStatus(void);
void ExttInvoiceStatusGetHook(entry gentries[], int x);
void ExttInvoiceStatusNavBar(void);

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

//tPayment
int tPaymentCommands(pentry entries[], int x);
void tPayment(const char *results);
void ProcesstPaymentVars(pentry entries[], int x);
void tPaymentContent(void);
void tPaymentInputContent(void);
void tPaymentInput(unsigned uMode);
void tPaymentList(void);
void NewtPayment(unsigned uMode);
void ModtPayment(void);
void CreatetPayment(void);
void DeletetPayment(void);
void ExttPaymentGetHook(entry gentries[], int x);
void ExttPaymentNavBar(void);

//tAccountType
int tAccountTypeCommands(pentry entries[], int x);
void tAccountType(const char *results);
void ProcesstAccountTypeVars(pentry entries[], int x);
void tAccountTypeContent(void);
void tAccountTypeInputContent(void);
void tAccountTypeInput(unsigned uMode);
void tAccountTypeList(void);
void NewtAccountType(unsigned uMode);
void ModtAccountType(void);
void CreatetAccountType(void);
void DeletetAccountType(void);
void ExttAccountTypeGetHook(entry gentries[], int x);
void ExttAccountTypeNavBar(void);

//tShipping
int tShippingCommands(pentry entries[], int x);
void tShipping(const char *results);
void ProcesstShippingVars(pentry entries[], int x);
void tShippingContent(void);
void tShippingInputContent(void);
void tShippingInput(unsigned uMode);
void tShippingList(void);
void NewtShipping(unsigned uMode);
void ModtShipping(void);
void CreatetShipping(void);
void DeletetShipping(void);
void ExttShippingGetHook(entry gentries[], int x);
void ExttShippingNavBar(void);

//tProductType
int tProductTypeCommands(pentry entries[], int x);
void tProductType(const char *results);
void ProcesstProductTypeVars(pentry entries[], int x);
void tProductTypeContent(void);
void tProductTypeInputContent(void);
void tProductTypeInput(unsigned uMode);
void tProductTypeList(void);
void NewtProductType(unsigned uMode);
void ModtProductType(void);
void CreatetProductType(void);
void DeletetProductType(void);
void ExttProductTypeGetHook(entry gentries[], int x);
void ExttProductTypeNavBar(void);

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

//tMotd
int tMotdCommands(pentry entries[], int x);
void tMotd(const char *results);
void ProcesstMotdVars(pentry entries[], int x);
void tMotdContent(void);
void tMotdInputContent(void);
void tMotdInput(unsigned uMode);
void tMotdList(void);
void NewtMotd(unsigned uMode);
void ModtMotd(void);
void CreatetMotd(void);
void DeletetMotd(void);
void ExttMotdGetHook(entry gentries[], int x);
void ExttMotdNavBar(void);


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

