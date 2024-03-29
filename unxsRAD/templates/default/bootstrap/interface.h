/*
FILE
	{{cProject}}/interfaces/bootstrap/interface.h
	Template unxsRAD/templates/default/bootstrap/interface.h
AUTHOR
	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice
	(C) 2015-2017 Gary Wallis for Unixservice, LLC.

*/

#define INTERFACE_HEADER_TITLE "{{cProject}}"
#define INTERFACE_COPYRIGHT "<font color=gray>Mobile Centric Software by <a target='_blank' href='https://unixservice.com'>Unixservice, LLC.</a></font>"


#include "mysqlrad.h"
#include "local.h"
#include <ctype.h>
#include <openisp/template.h>

//libtemplate required
#define MAXPOSTVARS 64
#define MAXGETVARS 8

#define BO_CUSTOMER      "Backend Customer"
#define BO_RESELLER      "Backend Reseller"
#define BO_ADMIN         "Backend Admin"
#define BO_ROOT  "Backend Root"
#define BO_CUSTOMER      "Backend Customer"
#define BO_RESELLER      "Backend Reseller"
#define BO_ADMIN         "Backend Admin"
#define BO_ROOT  "Backend Root"
#define ORG_CUSTOMER     "Organization Customer"
#define ORG_WEBMASTER    "Organization Webmaster"
#define ORG_SALES        "Organization Sales Force"
#define ORG_SERVICE      "Organization Customer Service"
#define ORG_ACCT "Organization Bookkeeper"
#define ORG_ADMIN        "Organization Admin"
#define IP_BLOCK_CIDR 1
#define IP_BLOCK_DASH 2

//Depend on correctly preconfigured tTemplateSet
#define uDEFAULT	1
//and tTemplateType:
#define uRAD4		1	
#define uBOOTSTRAP	2

void InterfaceConnectDb(void);

//In main.c
const char *cUserLevel(unsigned uPermLevel);
void htmlFatalError(const char *cErrMsg);
void textFatalError(const char *cErrMsg);
void htmlHeader(char *cTitle, char *cTemplateName);
void htmlFooter(char *cTemplateName);
void htmlErrorPage(char *cTitle, char *cTemplateName);
void htmlForgotPage(char *cTitle, char *cTemplateName);
void htmlHelpPage(char *cTitle, char *cTemplateName);
char *IPNumber(char *cInput);
const char *cUserLevel(unsigned uPermLevel);
char *TextAreaSave(char *cField);
char *FQDomainName(char *cInput);
void iDNSLog(unsigned uTablePK, char *cTableName, char *cLogEntry);
const char *cForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);
const char *cForeignKeyStr(const char *cTableName, const char *cFieldName, const char *cKey);
void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template);
void InterfaceLogoutFirewallJobs(unsigned uLoginClient);
void InterfaceLoginFirewallJobs(unsigned uLoginClient);
char *cEndAtSpace(char *cBuffer);

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
extern unsigned guLoginClient;
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
extern unsigned guRequireOTPLogin;
extern char gcOTPSecret[];
extern char gcOTPInfo[];

//Menu
//

{{funcBootstrapModulePrototypes}}

//user.c
void ProcessUserVars(pentry entries[], int x);
void UserGetHook(entry gentries[],int x);
void UserCommands(pentry entries[], int x);
void htmlUser(void);
void htmlUserPage(char *cTitle, char *cTemplateName);
void funcMOTD(FILE *fp);
void funcOperationHistory(FILE *fp);
void funcLoginHistory(FILE *fp);
void unxsvzLog(unsigned uTablePK,char *cTableName,char *cLogEntry,unsigned guPermLevel,unsigned guLoginClient,char *gcLogin,char *gcHost);

void logfileLine(const char *cFunction,const char *cLogline);
