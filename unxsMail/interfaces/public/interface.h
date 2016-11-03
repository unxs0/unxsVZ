/*
FILE
	interface.h
	svn ID removed
AUTHOR
	(C) 2006i-2009 Gary Wallis and Hugo Urquiza for Unixservice

*/


#include "../../mysqlrad.h"
#include "../../local.h"
#include <ctype.h>
#include <openisp/template.h>

//libtemplate required
#define MAXPOSTVARS 64
#define MAXGETVARS 8


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
void unxsRadiusLog(unsigned uTablePK, char *cTableName, char *cLogEntry);
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServerGroup,char *cJobData,
		unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy);
const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);

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
extern char gcPassword[];


//Menu
//

//home.c
void ProcessHomeVars(pentry entries[], int x);
void HomeCommands(pentry entries[], int x);
void HomeGetHook(entry gentries[],int x);
void htmlHome(void);
void htmlHomePage(char *cTitle, char *cTemplateName);
void funcSelectLogin(FILE *fp);

//myaccount.c
void ProcessMyAccountVars(pentry entries[], int x);
void MyAccountGetHook(entry gentries[],int x);
void MyAccountCommands(pentry entries[], int x);
void htmlMyAccount(void);
void htmlMyAccountPage(char *cTitle, char *cTemplateName);

//glossary.c
void GlossaryGetHook(entry gentries[],int x);
void htmlGlossary(void);
void htmlGlossaryPage(char *cTitle, char *cTemplateName);

