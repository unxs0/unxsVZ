/*
FILE
	interface.h
	$Id: interface.h 483 2009-08-12 20:29:56Z Hugo $
AUTHOR
	(C) 2006-2009 Gary Wallis for Unixservice

*/


#include "../../mysqlrad.h"
#include "../../local.h"
#include <ctype.h>
#include <errno.h>
#include <openisp/template-branch.h>
/*#include <uradius.h> //Our uRadiusLib
#include <openisp/servermonitor.h>
*/
//libtemplate required
#define MAXPOSTVARS 128
#define MAXGETVARS 8
//Controls the date format returned by FromMySQLDate()
//#define SPANISH_DATE 
//Instance status only
#define mysqlISP_NeverDeployed 1
#define mysqlISP_WaitingInitial 2
#define mysqlISP_DeployedMod 3
#define mysqlISP_OnHold 6
#define mysqlISP_WaitingRedeployment 9 
#define mysqlISP_WaitingCancel 10 
#define mysqlISP_WaitingHold 11 
#define mysqlISP_WaitingApproval 21
#define mysqlISP_Invalid 31
//Job status Only
#define mysqlISP_RemotelyQueued 7
#define mysqlISP_Waiting 10
//Both
#define mysqlISP_Deployed 4
#define mysqlISP_Canceled 5

//Product related
//Tied to tProductType
#define mysqlISP_BillingOnly 5
//Tied to tPeriod
#define mysqlISP_BillingHourly 7
#define mysqlISP_BillingStorage 9
#define mysqlISP_BillingTraffic 10

//These values must match tTicketStatus.uInvoiceStatus
//Ticket related
//Open invoices start at 1
#define mysqlISP_NewTicket 1
#define mysqlISP_SentTicket 2
#define mysqlISP_PartialTicket 4
#define mysqlISP_PastDueTicket 6
#define mysqlISP_BadDebtTicket 7
#define mysqlISP_CollectionTicket 8
//Closed invoices start at 100
#define mysqlISP_PaidTicket 110
#define mysqlISP_VoidTicket 210
#define mysqlISP_ReceiptSentTicket 211

//Ticket job queue related
#define mysqlISP_MailQueuedTicket 10
#define mysqlISP_PrintQueuedTicket 11


//In main.c
const char *cUserLevel(unsigned uPermLevel);
void htmlHeader(char *cTitle, char *cTemplateName);
void htmlFooter(char *cTemplateName);
void htmlErrorPage(char *cTitle, char *cTemplateName);
void htmlForgotPage(char *cTitle, char *cTemplateName);
void htmlHelpPage(char *cTitle, char *cTemplateName);
char *IPNumber(char *cInput);
const char *cUserLevel(unsigned uPermLevel);
char *TextAreaSave(char *cField);
char *FQDomainName(char *cInput);
void mysqlISPLog(unsigned uTablePK, char *cTableName, char *cLogEntry);
int ReadPullDown(const char *cTableName,const char *cFieldName,const char *cLabel);
void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template);	
void htmlPlainTextError(const char *cError);
char *cShortenText(char *cText,unsigned uWords);	
const char *cForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);
void FromMySQLDate(char *cMySQLDate);
char *WordToLower(char *cInput);
void ExtConnectDb(char *cDbName, char *cDbIp, char *cDbPwd, char *cDbLogin);
void EncryptPasswdWithSalt(char *pw, char *salt);
void GetConfiguration(const char *cName, char *cValue);
void funcDisplayDashBoard(FILE *fp);
void EncryptPasswdMD5(char *pw);
unsigned uGetAdminCompany(void);
void GetClientOwner(unsigned uClient, unsigned *uOwner);

//Global vars all declared in main.c
//libtemplate.a required
extern MYSQL gMysql;
//Multipurpose buffer
extern char gcQuery[];
//Login related
extern char gcCookie[];
extern char gcLogin[];
extern char gcPasswd[];
extern unsigned guSSLCookieLogin;
extern int guPermLevel;
extern char gcuPermLevel[];
extern unsigned guLoginUser;
extern unsigned guOrg;
extern char gcUser[];
extern char gcName[];
extern char gcOrgName[];
extern char gcHost[];
extern unsigned guBrowserFirefox;
extern unsigned guTemplateSet;

//Cgi form commands and major area function
extern char gcFunction[];
extern char gcPage[];
extern char *gcMessage;
extern char gcModStep[];
extern char gcNewStep[];
extern char gcDelStep[];
extern char gcInputStatus[];
extern char gcPermInputStatus[];

//Menu
//

//customer.c
void ProcessCustomerVars(pentry entries[], int x);
void CustomerGetHook(entry gentries[],int x);
void CustomerCommands(pentry entries[], int x);
void htmlCustomer(void);
void htmlCustomerPage(char *cTitle, char *cTemplateName);
void funcSelectProduct(FILE *fp);
void funcProductParameterInput(FILE *fp);
void funcEnteredParameters(FILE *fp);
void funcCustomerTickets(FILE *fp);
void funcSelectAccountType(FILE *fp);
void funcSelectLanguage(FILE *fp);

typedef struct {
       char cParameter[65];
        char cParamType[33];
        char cRange[17];
        char cExtVerify[256];
        unsigned uUnique;
        unsigned uAllowMod;
        unsigned uModPostDeploy;
} structParameter;
void GetParamInfo(const char *cParameter,structParameter *structParam);
void funcSelectExpMonth(FILE *fp);
void funcSelectExpYear(FILE *fp);
void funcSelectCardType(FILE *fp);
void funcSelectPayment(FILE *fp);
void SubmitInstanceJob(char *cJobType,time_t uJobDate,unsigned uInstance,unsigned uExternalCall);
void funcDateSelectTable(FILE *fp);
void funcCustomerNavList(FILE *fp);
char *strptime(const char *s, const char *format, struct tm *tm);
int CheckParameterValue(char *cValue, char *cParamType, char *cRange, unsigned uUnique, char *cExtVerify, char *cParameter, unsigned uDeployed);
void funcCustomerNavBar(FILE *fp);


//glossary.c
void GlossaryGetHook(entry gentries[],int x);
void htmlGlossary(void);
void htmlGlossaryPage(char *cTitle, char *cTemplateName);

//ticket.c
void ProcessTicketVars(pentry entries[], int x);
void TicketGetHook(entry gentries[],int x);
void TicketCommands(pentry entries[], int x);
void htmlTicket(void);
void htmlTicketPage(char *cTitle, char *cTemplateName);
void funcTicketNavList(FILE *fp);
void funcAssignedTo(FILE *fp);
void funcTicketStatus(FILE *fp);
void funcTicketNavBar(FILE *fp);
void funcTicketComments(FILE *fp);


//product.c
void ProcessProductVars(pentry entries[], int x);
void ProductGetHook(entry gentries[],int x);
void ProductCommands(pentry entries[], int x);
void htmlProduct(void);
void htmlProductPage(char *cTitle, char *cTemplateName);
void funcSelectAvailable(FILE *fp);
void funcSelectPeriod(FILE *fp);
void funcProductServices(FILE *fp);
void funcServicesToAdd(FILE *fp);
void funcProductNavList(FILE *fp);
void funcProductNavBar(FILE *fp);

//instance.c
void ProcessInstanceVars(pentry entries[], int x);
void InstanceGetHook(entry gentries[],int x);
void InstanceCommands(pentry entries[], int x);
void htmlInstance(void);
void htmlInstancePage(char *cTitle, char *cTemplateName);
void funcEditProductParams(FILE *fp);
void funcInstanceOperations(FILE *fp);
char *cGetcCustomerName(unsigned uRowId);

typedef struct structtClientConfig {
	char cValue[256][100];
	unsigned uClientConfig[100];
} structtClientConfig;


//dasboard.c
void htmlDashBoard(void);

