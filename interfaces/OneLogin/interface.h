/*
FILE
	interface.h
	$Id$
AUTHOR
	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice

*/


#include "../../mysqlrad.h"
#include "../../local.h"
#include <ctype.h>
#include <openisp/template.h>
//#include <lber.h>
//#include <ldap.h>

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

//Depend on correctly preconfigured tTemplateSet and tTemplateType:
#define uPLAINSET	21
#define uOneLogin	22

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
const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);
void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template);
void LogoutFirewallJobs(unsigned uLoginClient);
void LoginFirewallJobs(unsigned uLoginClient);

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
extern char gcBrand[];
extern unsigned guBrowserFirefox;

//Cgi form commands and major area function
extern char gcFunction[];
extern char gcPage[];
extern char *gcMessage;
extern char gcModStep[];
extern char gcNewStep[];
extern char gcDelStep[];
extern char gcInputStatus[];
extern unsigned guZone;
extern unsigned guView;
extern char gcOTPSecret[];

//Menu
//

//container.c
void ProcessContainerVars(pentry entries[], int x);
void ContainerGetHook(entry gentries[],int x);
void ContainerCommands(pentry entries[], int x);
void htmlContainer(void);
void htmlContainerPage(char *cTitle, char *cTemplateName);
void funcSelectContainer(FILE *fp);
void funcContainerImageTag(FILE *fp);
void funcContainerInfo(FILE *fp);
void funcNewContainer(FILE *fp);
void funcContainer(FILE *fp);
void funcContainerList(FILE *fp);
void funcContainerQOS(FILE *fp);
void funcContainerBulk(FILE *fp);

//user.c
void ProcessUserVars(pentry entries[], int x);
void UserGetHook(entry gentries[],int x);
void UserCommands(pentry entries[], int x);
void htmlUser(void);
void htmlUserPage(char *cTitle, char *cTemplateName);
void funcMOTD(FILE *fp);
void funcOperationHistory(FILE *fp);
void funcLoginHistory(FILE *fp);

//glossary.c
void GlossaryGetHook(entry gentries[],int x);
void htmlGlossary(void);
void htmlGlossaryPage(char *cTitle, char *cTemplateName);

//ldap.c
//int iValidLDAPLogin(const char *cLogin, const char *cPasswd, char *cOrganization);
//void ldapErrorLog(char *cMessage,LDAP *ld);
void logfileLine(const char *cFunction,const char *cLogline);
