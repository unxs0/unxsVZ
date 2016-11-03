/*
FILE
	interface.h
	svn ID removed
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
void TicketCommentGetHook(entry gentries[],int x);


//register.c
void ProcessRegisterVars(pentry entries[], int x);
void RegisterGetHook(entry gentries[],int x);
void RegisterCommands(pentry entries[], int x);
void htmlRegister(void);
void htmlRegisterPage(char *cTitle, char *cTemplateName);

