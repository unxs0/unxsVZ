/*
FILE
	interface.h
	svn ID removed
AUTHOR
	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice

*/


#include "../../mysqlrad.h"
#include "../../local.h"
#include <ctype.h>
#include <errno.h>
#include <openisp/template.h>
#include <poll.h>

//libtemplate required
#define MAXPOSTVARS 64
#define MAXGETVARS 8
#define MAX_RESULTS 100


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
void unxsMailLog(unsigned uTablePK, char *cTableName, char *cLogEntry);
int ReadPullDown(const char *cTableName,const char *cFieldName,const char *cLabel);
void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template);	
void htmlPlainTextError(const char *cError);
const char *cForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);
void interface_ExtSelectSearch(const char *cTable,const char *cVarList,const char *cSearchField,const char *cSearch,const char *cExtraCond,char *cLimit);
void ExtSelectRow(const char *cTable,const char *cVarList,unsigned uRow);
char *WordToLower(char *cInput);
void EncryptPasswdMD5(char *pw);
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServerGroup,char *cJobData,
		unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy);
void funcJobInfo(FILE *fp,char *cJobName);
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
extern unsigned guCompany;
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
extern char gcZone[];
extern char gcUser[];
extern char gcInputStatus[];
extern char gcPermInputStatus[];

//Menu
//

//user.c
void ProcessUserVars(pentry entries[], int x);
void UserGetHook(entry gentries[],int x);
void UserCommands(pentry entries[], int x);
void htmlUser(void);
void htmlUserPage(char *cTitle, char *cTemplateName);
void funcSelectDomain(FILE *fp);
void funcSelectUserType(FILE *fp);
void funcSelectServerGroup(FILE *fp);
void funcUserNavList(FILE *fp);
void CreateConfigFromTemplate(unsigned uUserConfig, unsigned uUser, unsigned uConfigSpec);
void CreateConfigs(void);
void funcUserSystemInfo(FILE *fp);

//domain.c
void ProcessDomainVars(pentry entries[], int x);
void DomainGetHook(entry gentries[],int x);
void DomainCommands(pentry entries[], int x);
void htmlDomain(void);
void htmlDomainPage(char *cTitle, char *cTemplateName);
void funcDomainNavList(FILE *fp);

//glossary.c
void GlossaryGetHook(entry gentries[],int x);
void htmlGlossary(void);
void htmlGlossaryPage(char *cTitle, char *cTemplateName);

//vut.c
void ProcessVUTVars(pentry entries[], int x);
void VUTGetHook(entry gentries[],int x);
void VUTCommands(pentry entries[], int x);
void htmlVUT(void);
void htmlVUTPage(char *cTitle, char *cTemplateName);
void funcVUTNavList(FILE *fp);

//vutentries.c
void ProcessVUTEntriesVars(pentry entries[], int x);
void VUTEntriesGetHook(entry gentries[],int x);
void VUTEntriesCommands(pentry entries[], int x);
void htmlVUTEntries(void);
void htmlVUTEntriesPage(char *cTitle, char *cTemplateName);
void funcVUTEntries(FILE *fp);
void funcVUTEntriesNavList(FILE *fp);

//alias.c
void ProcessAliasVars(pentry entries[], int x);
void AliasGetHook(entry gentries[],int x);
void AliasCommands(pentry entries[], int x);
void htmlAlias(void);
void htmlAliasPage(char *cTitle, char *cTemplateName);
void funcAliasNavList(FILE *fp);

//dashboard.c
void htmlDashBoard(void);
void htmlDashBoardPage(char *cTitle, char *cTemplateName);
void funcDashBoard(FILE *fp);
void ServerStatus(void);
void JobQueueStatus(void);
void MailGraphImages(void);

