/*
FILE
	interface.h
	$Id: interface.h 209 2007-11-21 01:13:26Z Hugo $
AUTHOR
	(C) 2006-2008 Gary Wallis for Unixservice

*/


#include "../../mysqlrad.h"
#include "../../local.h"
#include <ctype.h>
#include <errno.h>
#include <openisp/template.h>
/*#include <uradius.h> //Our uRadiusLib
#include <openisp/servermonitor.h>
*/
//libtemplate required
#define MAXPOSTVARS 128
#define MAXGETVARS 8
//Controls the date format returned by FromMySQLDate()
#define SPANISH_DATE 
//Instance status only
#define mysqlISP_NeverDeployed 1
#define mysqlISP_WaitingInitial 2
#define mysqlISP_DeployedMod 3
#define mysqlISP_OnHold 6
#define mysqlISP_WaitingRedeployment 9 
#define mysqlISP_WaitingCancel 11 
#define mysqlISP_WaitingHold 12 
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

//These values must match tInvoiceStatus.uInvoiceStatus
//Invoice related
//Open invoices start at 1
#define mysqlISP_NewInvoice 1
#define mysqlISP_SentInvoice 2
#define mysqlISP_PartialInvoice 4
#define mysqlISP_PastDueInvoice 6
#define mysqlISP_BadDebtInvoice 7
#define mysqlISP_CollectionInvoice 8
//Closed invoices start at 100
#define mysqlISP_PaidInvoice 110
#define mysqlISP_VoidInvoice 210
#define mysqlISP_ReceiptSentInvoice 211

//Invoice job queue related
#define mysqlISP_MailQueuedInvoice 10
#define mysqlISP_PrintQueuedInvoice 11


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
void funcSelectVPN(FILE *fp);
void funcRegTerms(FILE *fp);

//glossary.c
void GlossaryGetHook(entry gentries[],int x);
void htmlGlossary(void);
void htmlGlossaryPage(char *cTitle, char *cTemplateName);

