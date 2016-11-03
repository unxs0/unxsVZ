/*
FILE
	interface.h
	svn ID removed
AUTHOR
	(C) 2006 Gary Wallis for Unixservice

*/


#include "../../mysqlrad.h"
#include "../../local.h"
#include <ctype.h>
#include <errno.h>
#include <openisp/template.h>

//libtemplate required
#define MAXPOSTVARS 64
#define MAXGETVARS 8

#define FOCUS_TAB_CLASS "wpsSelectedPlace"
#define FOCUS_TAB_LINK_CLASS "wpsSelectedPlaceLink"

#define DEFAULT_TAB_CLASS "wpsUnSelectedPlace"
#define DEFAULT_TAB_LINK_CLASS "wpsUnSelectedPlaceLink"

//This definition is used in page titles, etc. Please update for your site
#define SITE_NAME "Unixservice"
//This definition is the first part of the title at the gray box
//E.g: Acme Widgets :: Home
#define BOX_TITLE "Unixservice"
//In main.c
void htmlHeader(char *cTitle, char *cTemplateName);
void htmlFooter(char *cTemplateName);
char *TextAreaSave(char *cField);
int ReadPullDown(const char *cTableName,const char *cFieldName,const char *cLabel);
void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template);	
void htmlPlainTextError(const char *cError);
	

//Global vars all declared in main.c
//libtemplate.a required
extern MYSQL gMysql;
//Multipurpose buffer
extern char gcQuery[];
extern char gcHost[];

//Cgi form commands and major area function
extern char gcFunction[];
extern char gcPage[];
extern char *gcMessage;
extern unsigned guContentType;
//Menu
//

//customer.c
void ProcessContentVars(pentry entries[], int x);
void ContentGetHook(entry gentries[],int x);
void ContentCommands(pentry entries[], int x);
void htmlContent(void);
void htmlContentPage(char *cTitle, char *cTemplateName);
void funcNavBar(FILE *fp);
void funcFeaturedArticles(FILE *fp);
void SectionsGetHook(entry gentries[],int x);
void funcSectionArticles(FILE *fp);
void funcSearchResults(FILE *fp);


