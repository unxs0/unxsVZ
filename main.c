/*
FILE 
	unxsVZ/main.c
	$Id$
PURPOSE
	Main cgi interface and common functions used for all the other
	table tx.c files and their schema independent txfunc.h files -until
	you mess with them in non standard ways...lol.
	
AUTHOR/LEGAL
	(C) 2001-2012 Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
OTHER
	Only tested on CentOS 5 with Firefox and Chrome browsers.
FREE HELP
	support @ openisp . net
	supportgrp @ unixservice . com
	Join mailing list: https://lists.openisp.net/mailman/listinfo/unxsvz
*/

#include "mysqlrad.h"
#include <ctype.h>

#include "language.h"
#include "local.h"

//Global vars
#define SHOWPAGE 30
MYSQL gMysql;
MYSQL gMysqlUBC;
char *gcUBCDBIP0;//must be set to DBIP0 to be used as alternative
char *gcUBCDBIP1;
char gcUBCDBIP0Buffer[32]={""};
char gcUBCDBIP1Buffer[32]={""};
FILE *gLfp=NULL;

unsigned long gluRowid;
unsigned guStart;
unsigned guEnd;
unsigned guI;
unsigned guN=SHOWPAGE;
char gcCommand[100];
char gcFilter[100];
char gcAuxFilter[256];
char gcFind[100];
unsigned guMode;

int guPermLevel=0;
unsigned guLoginClient=0;
unsigned guReseller=0;
unsigned guCompany=0;
char gcCompany[100]={""};
char gcUser[100]={""};
char gcHost[100]={""};
char gcHostname[100]={""};

//libunxsvz required
char cStartDate[32]={""};
char cStartTime[32]={""};
unsigned guDatacenter=0;
char cLogKey[16]={"Ksdj458jssdUjf79"};

//SSLLoginCookie()
char gcCookie[1024]={""};
char gcLogin[100]={""};
char gcPasswd[100]={""};
unsigned guSSLCookieLogin=0;
unsigned guRequireOTPLogin=0;
unsigned guOTPExpired=0;
char gcOTP[16]={""};
char gcOTPInfo[64]={"Nothing yet"};
char gcOTPSecret[65]={""};

char gcFunction[100]={""};
unsigned guListMode=0;
char gcQuery[8192]={""};
char *gcQstr=gcQuery;
char *gcBuildInfo="$Id$";
char *gcRADStatus="Forked";

//Local
void Footer_ism3(void);
void Header_ism3(const char *cMsg, int iJs);
const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);
const char *ForeignKey2(const char *cTableName, const char *cFieldName, unsigned uKey);
char *cEmailInput(char *cInput);
void GetClientOwner(unsigned uClient, unsigned *uOwner);
void htmlPlainTextError(const char *cError);

//Ext
int iExtMainCommands(pentry entries[], int x);
void ExtMainContent(void);
void ExtMainShell(int iArgc, char *cArgv[]);
void DashBoard(const char *cOptionalMsg);

//Only local
void ConnectDb(char *cMessage);
void jsToggleCheckboxes(void);
void NoSuchFunction(void);
void unxsVZ(const char *cResult);
void InitialInstall(void);
const char *cUserLevel(unsigned uPermLevel);
int iValidLogin(int iMode);
void SSLCookieLogin(void);
void SetLogin(void);
void EncryptPasswdWithSalt(char *gcPasswd, char *cSalt);
void GetPLAndClient(char *cUser);
void htmlSSLLogin(void);
void GeneratePasswd(char *pw);
void UpdateOTPExpire(unsigned uAuthorize,unsigned uClient);
void LoginFirewallJobs(unsigned uLoginClient);
void LogoutFirewallJobs(unsigned uLoginClient);

//mainfunc.h for symbolic links to this program
void CalledByAlias(int iArgc,char *cArgv[]);

//Testing globalization
pentry entries[512];
entry gentries[16];
int x;

int main(int iArgc, char *cArgv[])
{
	char *gcl;
	int cl=0;

	gethostname(gcHostname, 98);

	if(!strstr(cArgv[0],"unxsVZ.cgi"))
		CalledByAlias(iArgc,cArgv);

	if(getenv("REMOTE_HOST")!=NULL)
		sprintf(gcHost,"%.99s",getenv("REMOTE_HOST"));

	else if(getenv("REMOTE_ADDR")!=NULL)
	{
		char cMessage[256]={""};
		ConnectDb(cMessage);
		if(cMessage[0]) unxsVZ(cMessage);
		sprintf(gcHost,"%.99s",getenv("REMOTE_ADDR"));
	}
	else
	{
		ExtMainShell(iArgc,cArgv);
	}


	if(strcmp(getenv("REQUEST_METHOD"),"POST"))
	{

		SSLCookieLogin();

	        gcl = getenv("QUERY_STRING");

	        for(x=0;gcl[0] != '\0' && x<16;x++)
		{
               		getword(gentries[x].val,gcl,'&');
               		plustospace(gentries[x].val);
               		unescape_url(gentries[x].val);
               		getword(gentries[x].name,gentries[x].val,'=');

			//basic anti hacker
			escape_shell_cmd(gentries[x].val);

			//Local vars
			if(!strcmp(gentries[x].name,"gcFunction")) 
				sprintf(gcFunction,"%.99s",gentries[x].val);
		}

		if(gcFunction[0])
		{
			if(!strcmp(gcFunction,"Main"))
				unxsVZ("");
			else if(!strcmp(gcFunction,"Logout"))
			{
				printf("Set-Cookie: unxsVZLogin=; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
				printf("Set-Cookie: unxsVZPasswd=; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
				sprintf(gcQuery,"INSERT INTO tLog SET cLabel='logout %.99s',uLogType=6,uPermLevel=%u,"
				" uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=%u,uCreatedBy=1,"
				" uCreatedDate=UNIX_TIMESTAMP(NOW()) ON DUPLICATE KEY UPDATE "
				" cLabel='logout %.99s',uLogType=6,uPermLevel=%u,"
				" uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=%u,uCreatedBy=1,"
				" uCreatedDate=UNIX_TIMESTAMP(NOW())",
					gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname,guCompany,
					gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname,guCompany);
				MYSQL_RUN;
				LogoutFirewallJobs(guLoginClient);
				if(gcOTPSecret[0])
				{
					UpdateOTPExpire(0,guLoginClient);
					guRequireOTPLogin=1;
				}
				gcCookie[0]=0;
                                guPermLevel=0;
                                guLoginClient=0;
                                gcUser[0]=0;
                                gcCompany[0]=0;
                                guSSLCookieLogin=0;
                                htmlSSLLogin();
			}

			else if(!strcmp(gcFunction,"tDatacenter"))
				ExttDatacenterGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tNode"))
				ExttNodeGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tContainer"))
				ExttContainerGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tProperty"))
				ExttPropertyGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tType"))
				ExttTypeGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tOSTemplate"))
				ExttOSTemplateGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tNameserver"))
				ExttNameserverGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tSearchdomain"))
				ExttSearchdomainGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tConfig"))
				ExttConfigGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tIP"))
				ExttIPGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tGroupType"))
				ExttGroupTypeGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tGroup"))
				ExttGroupGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tGroupGlue"))
				ExttGroupGlueGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tClient"))
				ExttClientGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tAuthorize"))
				ExttAuthorizeGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tTemplate"))
				ExttTemplateGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tTemplateSet"))
				ExttTemplateSetGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tTemplateType"))
				ExttTemplateTypeGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tLog"))
				ExttLogGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tLogType"))
				ExttLogTypeGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tLogMonth"))
				ExttLogMonthGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tMonth"))
				ExttMonthGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tGlossary"))
				ExttGlossaryGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tJob"))
				ExttJobGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tJobStatus"))
				ExttJobStatusGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tStatus"))
				ExttStatusGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tConfiguration"))
				ExttConfigurationGetHook(gentries,x);
			else if(!strcmp(gcFunction,"Dashboard"))
				unxsVZ("DashBoard");
			else if(!strcmp(gcFunction,"Glossary"))
				GlossaryGetHook(gentries,x);
		}

		unxsVZ(ISPNAME);

	}//end get method interface section

	//Post method interface
	cl = atoi(getenv("CONTENT_LENGTH"));
	for(x=0;cl && (!feof(stdin)) && x<512 ;x++)
	{
		entries[x].val = fmakeword(stdin,'&',&cl);
		plustospace(entries[x].val);
		unescape_url(entries[x].val);
		entries[x].name = makeword(entries[x].val,'=');

		//basic anti hacker
		//Allow posted page cursors. See PageMachine()
		//escape_shell_cmd(entries[x].val);

		if(!strcmp(entries[x].name,"gcFunction")) 
			sprintf(gcFunction,"%.99s",entries[x].val);
		else if(!strcmp(entries[x].name,"guListMode")) 
			sscanf(entries[x].val,"%u",&guListMode);
		else if(!strcmp(entries[x].name,"gcLogin"))
			sprintf(gcLogin,"%.99s",entries[x].val);
                else if(!strcmp(entries[x].name,"gcPasswd"))
			sprintf(gcPasswd,"%.99s",entries[x].val);
                else if(!strcmp(entries[x].name,"gcOTP"))
			sprintf(gcOTP,"%.15s",entries[x].val);
	}

	//SSLCookieLogin()
        if(!strcmp(gcFunction,"Login")) SetLogin();

        if(!guPermLevel || !gcUser[0] || !guLoginClient)
                SSLCookieLogin();

	//Main Post Menu
	tDatacenterCommands(entries,x);
	tNodeCommands(entries,x);
	tContainerCommands(entries,x);
	tPropertyCommands(entries,x);
	tTypeCommands(entries,x);
	tOSTemplateCommands(entries,x);
	tNameserverCommands(entries,x);
	tSearchdomainCommands(entries,x);
	tConfigCommands(entries,x);
	tIPCommands(entries,x);
	tGroupTypeCommands(entries,x);
	tGroupCommands(entries,x);
	tGroupGlueCommands(entries,x);
	tClientCommands(entries,x);
	tAuthorizeCommands(entries,x);
	tTemplateCommands(entries,x);
	tTemplateSetCommands(entries,x);
	tTemplateTypeCommands(entries,x);
	tLogCommands(entries,x);
	tLogTypeCommands(entries,x);
	tLogMonthCommands(entries,x);
	tMonthCommands(entries,x);
	tGlossaryCommands(entries,x);
	tJobCommands(entries,x);
	tJobStatusCommands(entries,x);
	tStatusCommands(entries,x);
	tConfigurationCommands(entries,x);


	iExtMainCommands(entries,x);

	NoSuchFunction();

	return(0);

}//end of main()

#include "mainfunc.h"

void unxsVZ(const char *cResult)
{
	Header_ism3("Main",0);

        if(cResult[0])
	{
		if(!strcmp(cResult,"CloneReport"))
			CloneReport(cResult);
		else if(!strcmp(cResult,"ContainerReport"))
			ContainerReport(cResult);
		else if(!strcmp(cResult,"CapacityReport"))
			CapacityReport(cResult);
		else
			DashBoard(cResult);
	}
        else
	{
                ExtMainContent();
	}

	Footer_ism3();

}//void unxsVZ(const char *cResult)


void StyleSheet(void)
{
	printf("<style type=\"text/css\">\n");
	printf("<!--\n");

	printf("input.smallButton {width:50px;}\n");
	printf("input.medButton {width:100px;}\n");
	printf("input.largeButton {width:150px;}\n");
	printf("input.warnButton {color:red;}\n");
	printf("input.lwarnButton {color:red;width:150px;}\n");
	printf("input.alertButton {color:#DAA520;}\n");
	printf("input.lalertButton {color:#DAA520;width:150px;}\n");
	printf("input.revButton {color:white;background:black;}\n");
	printf("input.lrevButton {color:white;background:black;width:150px;}\n");
	printf("A.darkLink {color:black; text-decoration:none;}\n");
	printf("A.darkLink:hover {color:blue; text-decoration:underline;}\n");
	printf("A:hover {color:blue; text-decoration:underline;}\n");

	printf("textarea {font-family: Arial,Helvetica; font-size: 11px;}\n");
	printf("pre {font-family: Arial,Helvetica; font-size: 11px;}\n");
	printf("input {font-family: Arial,Helvetica; font-size: 11px;}\n");
	printf("select {font-family: Arial,Helvetica; font-size: 11px;}\n");

	printf("body\n");
	printf("{\n");
	printf("\tmargin-top: 0px;\n");
	printf("\tmargin-bottom: 12px;\n");
	printf("\tmargin-left: 12px;\n");
	printf("\tmargin-right: 10px;\n");
	printf("\tfont-family: Arial,Helvetica;\n");
	printf("\t#font-size: 85%%;\n");
	printf("\tfont-size: 65%%;\n");
	printf("\tline-height: 135%%;\n");
	printf("\tpadding: 0px;\n");
	printf("}\n");

	printf("br.clearall\n");
	printf("{\n");
	printf("\tclear: both;\n");
	printf("}\n");

	printf("td\n");
	printf("{\n");
	printf("\tfont-family: Arial,Helvetica;\n");
	printf("}\n");

	printf("#menuholder\n");
	printf("{\n");
	printf("\t#width: 100%%;\n");
	printf("}\n");

	printf("#menutab\n");
	printf("{\n");
	printf("\tbackground: #e5e5e5 url('/images/hairline.gif') repeat-x bottom center;\n");
	printf("\tborder-top: 0px solid #BEBFBF;\n");
	printf("\tborder-right: 1px solid #BEBFBF;\n");
	printf("\tmargin-top: 8px;\n");
	printf("}\n");

	printf("#topline\n");
	printf("{\n");
	printf("\t#width: 100%%;\n");
	printf("\tbackground: transparent url('/images/hairline.gif') repeat-x top center;\n");
	printf("}\n");

	printf("#menutab ul, #menutab ol\n");
	printf("{\n");
	printf("\tmargin: 0;\n");
	printf("\tpadding: 0px 0px 0;\n");
	printf("\tlist-style: none;\n");
	printf("}\n");

	printf("#menutab li\n");
	printf("{\n");
	printf("\tfloat: left;\n");
	printf("\tbackground: url('/images/left.gif') no-repeat left top;\n");
	printf("\tmargin: 0;\n");
	printf("\tpadding: 0 0 0 9px;\n");
	printf("}\n");

	printf("#menutab a, #menutab a.last\n");
	printf("{\n");
	printf("\tfloat:left;\n");
	printf("\tdisplay: block;\n");
	printf("\tbackground: url('/images/right.gif') no-repeat right top;\n");
	printf("\tpadding: 5px 15px 2px 6px;\n");
	printf("\tcolor: #222;\n");
	printf("\ttext-decoration: none;\n");
	printf("}\n");

	printf("/* Commented Backslash Hack\n");
	printf("\thides rule from IE5-Mac \\*/\n");
	printf("\t#menutab a, #menutab a.last {float:none;}\n");
	printf("/* End IE5-Mac hack */\n");

	printf("#menutab a.last\n");
	printf("{\n");
	printf("\tbackground: url('/images/right_last.gif') no-repeat right top;\n");
	printf("\tpadding: 5px 17px 2px 6px;\n");
	printf("}\n");

	printf("#menutab a:hover\n");
	printf("{\n");
	printf("\tcolor: black;\n");
	printf("\ttext-decoration: underline;\n");
	printf("}\n");

	printf("#menutab #current\n");
	printf("{\n");
	printf("\tbackground-image: url('/images/left_on.gif');\n");
	printf("}\n");

	printf("#menutab #current a\n");
	printf("{\n");
	printf("\tbackground-image: url('/images/right_on.gif');\n");
	printf("\tpadding-bottom: 3px;\n");
	printf("\ttext-decoration: none;\n");
	printf("}\n");

	printf("-->\n");
	printf("</style>\n");

}//void StyleSheet(void)


/*
* Uses Clean Calendar
* Copyright 2007-2009 Marc Grabanski (m@marcgrabanski.com) http://marcgrabanski.com
* Project Page: http://marcgrabanski.com/article/clean-calendar
* Under the MIT License
*
* Install only two files in your html/js dir
*/
void jsCalendarHeader(void)
{
        printf("<link rel='stylesheet' type='text/css' media='all' href='/js/calendar.css'/>\n");
        printf("<script type='text/javascript' src='/js/calendar.js'></script>\n");
}//void jsCalendarHeader(void)


void jsCalendarInput(char *cInputName,char *cValue,unsigned uMode)
{
        char cMode[16]={""};
        if(!uMode)
                sprintf(cMode,"disabled");

	printf("<input type=text title='Enter date US style month/day/full-year'"
			" class=calendarSelectDate name='%s' value='%s' %s ><div id=calendarDiv></div>",cInputName,cValue,cMode);

}//void jsCalendarInput(char *cInputName,char *cValue,unsigned uMode)


void jsToggleCheckboxes(void)
{
        printf("<script>"
		"function checkAll(checkname, toggle)"
		"{"
		"	for (i = 0; i < checkname.length; i++)"
		"	if( checkname[i].name.indexOf(\"NoCA\")==(-1) )"
		"	{"
		"		checkname[i].checked = toggle.checked? true:false"
		"	}"
		"}"
		"</script>");
}//void jsToggleCheckboxes(void)




void Header_ism3(const char *title, int iJs)
{
	printf("Content-type: text/html\n\n");
	printf("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
			" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
        printf("<html><head><title>"HEADER_TITLE" %s %s </title>",gcHostname,title);
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n");
        if(iJs==1)
	{
                jsCalendarHeader();
	}
        else if(iJs==2)
	{
		jsToggleCheckboxes();
	}
        else if(iJs==3)
	{
                jsCalendarHeader();
		jsToggleCheckboxes();
	}
	StyleSheet();
	printf("<script language='JavaScript' src='/css/popups.js'></script>\n");
	printf("<link rel=\"shortcut icon\" type=image/x-icon href=/images/unxsvz.ico>\n");
	printf("</head><body><form name=formMain action=unxsVZ.cgi method=post><blockquote>\n");
	printf("<img src=/images/unxslogo.gif>&nbsp;&nbsp;\n");

	//ModuleRAD3NavBars()
	if(!strcmp(gcFunction,"tDatacenter") || !strcmp(gcFunction,"tDatacenterTools") ||
			!strcmp(gcFunction,"tDatacenterList"))
		ExttDatacenterNavBar();
	else if(!strcmp(gcFunction,"tNode") || !strcmp(gcFunction,"tNodeTools") ||
			!strcmp(gcFunction,"tNodeList"))
		ExttNodeNavBar();
	else if(!strcmp(gcFunction,"tContainer") || !strcmp(gcFunction,"tContainerTools") ||
			!strcmp(gcFunction,"tContainerList"))
		ExttContainerNavBar();
	else if(!strcmp(gcFunction,"tProperty") || !strcmp(gcFunction,"tPropertyTools") ||
			!strcmp(gcFunction,"tPropertyList"))
		ExttPropertyNavBar();
	else if(!strcmp(gcFunction,"tType") || !strcmp(gcFunction,"tTypeTools") ||
			!strcmp(gcFunction,"tTypeList"))
		ExttTypeNavBar();
	else if(!strcmp(gcFunction,"tOSTemplate") || !strcmp(gcFunction,"tOSTemplateTools") ||
			!strcmp(gcFunction,"tOSTemplateList"))
		ExttOSTemplateNavBar();
	else if(!strcmp(gcFunction,"tNameserver") || !strcmp(gcFunction,"tNameserverTools") ||
			!strcmp(gcFunction,"tNameserverList"))
		ExttNameserverNavBar();
	else if(!strcmp(gcFunction,"tSearchdomain") || !strcmp(gcFunction,"tSearchdomainTools") ||
			!strcmp(gcFunction,"tSearchdomainList"))
		ExttSearchdomainNavBar();
	else if(!strcmp(gcFunction,"tConfig") || !strcmp(gcFunction,"tConfigTools") ||
			!strcmp(gcFunction,"tConfigList"))
		ExttConfigNavBar();
	else if(!strcmp(gcFunction,"tIP") || !strcmp(gcFunction,"tIPTools") ||
			!strcmp(gcFunction,"tIPList"))
		ExttIPNavBar();
	else if(!strcmp(gcFunction,"tGroupType") || !strcmp(gcFunction,"tGroupTypeTools") ||
			!strcmp(gcFunction,"tGroupTypeList"))
		ExttGroupTypeNavBar();
	else if(!strcmp(gcFunction,"tGroup") || !strcmp(gcFunction,"tGroupTools") ||
			!strcmp(gcFunction,"tGroupList"))
		ExttGroupNavBar();
	else if(!strcmp(gcFunction,"tGroupGlue") || !strcmp(gcFunction,"tGroupGlueTools") ||
			!strcmp(gcFunction,"tGroupGlueList"))
		ExttGroupGlueNavBar();
	else if(!strcmp(gcFunction,"tClient") || !strcmp(gcFunction,"tClientTools") ||
			!strcmp(gcFunction,"tClientList"))
		ExttClientNavBar();
	else if(!strcmp(gcFunction,"tAuthorize") || !strcmp(gcFunction,"tAuthorizeTools") ||
			!strcmp(gcFunction,"tAuthorizeList"))
		ExttAuthorizeNavBar();
	else if(!strcmp(gcFunction,"tTemplate") || !strcmp(gcFunction,"tTemplateTools") ||
			!strcmp(gcFunction,"tTemplateList"))
		ExttTemplateNavBar();
	else if(!strcmp(gcFunction,"tTemplateSet") || !strcmp(gcFunction,"tTemplateSetTools") ||
			!strcmp(gcFunction,"tTemplateSetList"))
		ExttTemplateSetNavBar();
	else if(!strcmp(gcFunction,"tTemplateType") || !strcmp(gcFunction,"tTemplateTypeTools") ||
			!strcmp(gcFunction,"tTemplateTypeList"))
		ExttTemplateTypeNavBar();
	else if(!strcmp(gcFunction,"tLog") || !strcmp(gcFunction,"tLogTools") ||
			!strcmp(gcFunction,"tLogList"))
		ExttLogNavBar();
	else if(!strcmp(gcFunction,"tLogType") || !strcmp(gcFunction,"tLogTypeTools") ||
			!strcmp(gcFunction,"tLogTypeList"))
		ExttLogTypeNavBar();
	else if(!strcmp(gcFunction,"tLogMonth") || !strcmp(gcFunction,"tLogMonthTools") ||
			!strcmp(gcFunction,"tLogMonthList"))
		ExttLogMonthNavBar();
	else if(!strcmp(gcFunction,"tMonth") || !strcmp(gcFunction,"tMonthTools") ||
			!strcmp(gcFunction,"tMonthList"))
		ExttMonthNavBar();
	else if(!strcmp(gcFunction,"tGlossary") || !strcmp(gcFunction,"tGlossaryTools") ||
			!strcmp(gcFunction,"tGlossaryList"))
		ExttGlossaryNavBar();
	else if(!strcmp(gcFunction,"tJob") || !strcmp(gcFunction,"tJobTools") ||
			!strcmp(gcFunction,"tJobList"))
		ExttJobNavBar();
	else if(!strcmp(gcFunction,"tJobStatus") || !strcmp(gcFunction,"tJobStatusTools") ||
			!strcmp(gcFunction,"tJobStatusList"))
		ExttJobStatusNavBar();
	else if(!strcmp(gcFunction,"tStatus") || !strcmp(gcFunction,"tStatusTools") ||
			!strcmp(gcFunction,"tStatusList"))
		ExttStatusNavBar();
	else if(!strcmp(gcFunction,"tConfiguration") || !strcmp(gcFunction,"tConfigurationTools") ||
			!strcmp(gcFunction,"tConfigurationList"))
		ExttConfigurationNavBar();


	//Login info
	//printf("<font size=3><b>unxsVZ</b></font> \n ");
	if(!guPermLevel)
	{
		printf("&nbsp;&nbsp;&nbsp;<font color=red>Your IP address %s has been logged</font>",gcHost);
		//debug only
		//printf("&nbsp;&nbsp;&nbsp;<font color=red>%s ",gcUser);
		//if(strcmp(gcUser,gcCompany)) printf("(%s) ",gcCompany);
		//printf("logged in from %s [%s/%u (%s)]</font>",gcHost,cUserLevel(guPermLevel),guReseller,gcOTPInfo);
	}
	else
	{
		printf("&nbsp;&nbsp;&nbsp;<font color=red>%s ",gcUser);
		if(strcmp(gcUser,gcCompany)) printf("(%s) ",gcCompany);
		printf("logged in from %s [%s/%u]</font>",gcHost,cUserLevel(guPermLevel),guReseller);
		//printf("logged in from %s [%s/%u (%s)]</font>",gcHost,cUserLevel(guPermLevel),guReseller,gcOTPInfo);
	}

	//Logout link
	if(guSSLCookieLogin)
	{
		printf(" <a title='Erase login cookies' href=unxsVZ.cgi?gcFunction=Logout>Logout</a> ");
		printf(" <a title='Quick dashboard link' href=unxsVZ.cgi?gcFunction=Dashboard>Dashboard</a> ");
	}

	//Generate Menu Items
	printf("\n<!-- tab menu -->\n");
	printf("<div id=menuholder>\n");
	printf("\t<div id=menutab>\n");

	printf("\t\t<div id=topline>\n");
	printf("\t\t\t<ol>\n");


	//Main always exists for RAD
	printf("\t\t\t<li");
	if(strncmp(gcFunction,"Main",4))
		printf(">\n");
	else
		printf(" id=current>\n");
		printf("\t\t\t<a title='Home start page' href=unxsVZ.cgi?gcFunction=Main>Main</a>\n");
	printf("\t\t\t</li>\n");

if(guSSLCookieLogin)
{
	//tDatacenter
	if(guPermLevel>=7)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tDatacenter") && strcmp(gcFunction,"tDatacenterTools") &&
			strcmp(gcFunction,"tDatacenterList"))
		printf(">\n");
	  else
		printf(" id=current>\n");
	  printf("\t\t\t<a title='Datacenter' href=unxsVZ.cgi?gcFunction=tDatacenter>tDatacenter</a>\n");
	}
	//tNode
	if(guPermLevel>=6)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tNode") && strcmp(gcFunction,"tNodeTools") &&
			strcmp(gcFunction,"tNodeList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Hardware node' href=unxsVZ.cgi?gcFunction=tNode>tNode</a>\n");
	}
	//tContainer
	if(guPermLevel>=7)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tContainer") && strcmp(gcFunction,"tContainerTools") &&
			strcmp(gcFunction,"tContainerList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='OpenVZ Container' href=unxsVZ.cgi?gcFunction=tContainer>tContainer</a>\n");
	}
	//tProperty
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tProperty") && strcmp(gcFunction,"tPropertyTools") &&
			strcmp(gcFunction,"tPropertyList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Shared property table' href=unxsVZ.cgi?gcFunction=tProperty>tProperty</a>\n");
	}
	//tType
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tType") && strcmp(gcFunction,"tTypeTools") &&
			strcmp(gcFunction,"tTypeList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Property Type' href=unxsVZ.cgi?gcFunction=tType>tType</a>\n");
	}
	//tOSTemplate
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tOSTemplate") && strcmp(gcFunction,"tOSTemplateTools") &&
			strcmp(gcFunction,"tOSTemplateList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Container OS Template' href=unxsVZ.cgi?gcFunction=tOSTemplate>tOSTemplate</a>\n");
	}
	//tNameserver
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tNameserver") && strcmp(gcFunction,"tNameserverTools") &&
			strcmp(gcFunction,"tNameserverList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Container Resolver Nameserver Set' href=unxsVZ.cgi?gcFunction=tNameserver>tNameserver</a>\n");
	}
	//tSearchdomain
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tSearchdomain") && strcmp(gcFunction,"tSearchdomainTools") &&
			strcmp(gcFunction,"tSearchdomainList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Container Resolver Searchdomain' href=unxsVZ.cgi?gcFunction=tSearchdomain>tSearchdomain</a>\n");
	}
	//tConfig
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tConfig") && strcmp(gcFunction,"tConfigTools") &&
			strcmp(gcFunction,"tConfigList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Config File Used on Creation' href=unxsVZ.cgi?gcFunction=tConfig>tConfig</a>\n");
	}
	//tIP
	if(guPermLevel>=10)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tIP") && strcmp(gcFunction,"tIPTools") &&
			strcmp(gcFunction,"tIPList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='IPs used and reserved for use' href=unxsVZ.cgi?gcFunction=tIP>tIP</a>\n");
	}
	//tGroupType
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tGroupType") && strcmp(gcFunction,"tGroupTypeTools") &&
			strcmp(gcFunction,"tGroupTypeList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Container or node group type' href=unxsVZ.cgi?gcFunction=tGroupType>tGroupType</a>\n");
	}
	//tGroup
	if(guPermLevel>=10)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tGroup") && strcmp(gcFunction,"tGroupTools") &&
			strcmp(gcFunction,"tGroupList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Container or node group' href=unxsVZ.cgi?gcFunction=tGroup>tGroup</a>\n");
	}
	//tGroupGlue
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tGroupGlue") && strcmp(gcFunction,"tGroupGlueTools") &&
			strcmp(gcFunction,"tGroupGlueList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Glues uContainers or uNodes to uGroups' href=unxsVZ.cgi?gcFunction=tGroupGlue>tGroupGlue</a>\n");
	}
	//tClient
	if(guPermLevel>=10)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tClient") && strcmp(gcFunction,"tClientTools") &&
			strcmp(gcFunction,"tClientList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Client Info' href=unxsVZ.cgi?gcFunction=tClient>tClient</a>\n");
	}
	//tAuthorize
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tAuthorize") && strcmp(gcFunction,"tAuthorizeTools") &&
			strcmp(gcFunction,"tAuthorizeList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Login Authorization' href=unxsVZ.cgi?gcFunction=tAuthorize>tAuthorize</a>\n");
	}
	//tTemplate
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tTemplate") && strcmp(gcFunction,"tTemplateTools") &&
			strcmp(gcFunction,"tTemplateList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='tTemplate' href=unxsVZ.cgi?gcFunction=tTemplate>tTemplate</a>\n");
	}
	//tTemplateSet
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tTemplateSet") && strcmp(gcFunction,"tTemplateSetTools") &&
			strcmp(gcFunction,"tTemplateSetList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='tTemplateSet' href=unxsVZ.cgi?gcFunction=tTemplateSet>tTemplateSet</a>\n");
	}
	//tTemplateType
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tTemplateType") && strcmp(gcFunction,"tTemplateTypeTools") &&
			strcmp(gcFunction,"tTemplateTypeList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='tTemplateType' href=unxsVZ.cgi?gcFunction=tTemplateType>tTemplateType</a>\n");
	}
	//tLog
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tLog") && strcmp(gcFunction,"tLogTools") &&
			strcmp(gcFunction,"tLogList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='tLog' href=unxsVZ.cgi?gcFunction=tLog>tLog</a>\n");
	}
	//tLogType
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tLogType") && strcmp(gcFunction,"tLogTypeTools") &&
			strcmp(gcFunction,"tLogTypeList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='tLogType' href=unxsVZ.cgi?gcFunction=tLogType>tLogType</a>\n");
	}
	//tLogMonth
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tLogMonth") && strcmp(gcFunction,"tLogMonthTools") &&
			strcmp(gcFunction,"tLogMonthList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='tLogMonth' href=unxsVZ.cgi?gcFunction=tLogMonth>tLogMonth</a>\n");
	}
	//tMonth
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tMonth") && strcmp(gcFunction,"tMonthTools") &&
			strcmp(gcFunction,"tMonthList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='tMonth' href=unxsVZ.cgi?gcFunction=tMonth>tMonth</a>\n");
	}
	//tGlossary
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tGlossary") && strcmp(gcFunction,"tGlossaryTools") &&
			strcmp(gcFunction,"tGlossaryList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='tGlossary' href=unxsVZ.cgi?gcFunction=tGlossary>tGlossary</a>\n");
	}
	//tJob
	if(guPermLevel>=7)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tJob") && strcmp(gcFunction,"tJobTools") &&
			strcmp(gcFunction,"tJobList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='tJob' href=unxsVZ.cgi?gcFunction=tJob>tJob</a>\n");
	}
	//tJobStatus
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tJobStatus") && strcmp(gcFunction,"tJobStatusTools") &&
			strcmp(gcFunction,"tJobStatusList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='tJobStatus' href=unxsVZ.cgi?gcFunction=tJobStatus>tJobStatus</a>\n");
	}
	//tStatus
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tStatus") && strcmp(gcFunction,"tStatusTools") &&
			strcmp(gcFunction,"tStatusList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='tStatus' href=unxsVZ.cgi?gcFunction=tStatus>tStatus</a>\n");
	}
	//tConfiguration
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tConfiguration") && strcmp(gcFunction,"tConfigurationTools") &&
			strcmp(gcFunction,"tConfigurationList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='tConfiguration' href=unxsVZ.cgi?gcFunction=tConfiguration>tConfiguration</a>\n");
	}
}

	
	printf("\t\t\t</ol>\n");

	printf("\t\t\t<br class=clearall >\n");
	printf("\t\t</div>\n");
	printf("\t</div>\n");
	printf("</div>\n");

}//Header_ism3(char *title, int js)


void Footer_ism3(void)
{
	printf("</blockquote>");

	mysql_close(&gMysql);
	exit(0);

}//Footer_ism3(void)


void NoSuchFunction(void)
{
	 
	sprintf(gcQuery,"[%s] Not Recognized",gcFunction);
	unxsVZ(gcQuery);
}

void ProcessControlVars(pentry entries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"gcFilter"))
			sprintf(gcFilter,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"gcAuxFilter"))
			sprintf(gcAuxFilter,"%.255s",entries[i].val);
		else if(!strcmp(entries[i].name,"gcCommand"))
			sprintf(gcCommand,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"gcFind"))
			sprintf(gcFind,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"gluRowid"))
			sscanf(entries[i].val,"%lu",&gluRowid);
	}
}

//guMode=0 page guMode, guMode=1 list auto header mode
void PageMachine(char *cFuncName, int iLmode, char *cMsg)
{
	
	if(iLmode)
	{
		//List mode
	
		if(gluRowid<1) gluRowid=1;
		if(gluRowid>(guN=((guI/SHOWPAGE)+1))) gluRowid=guN;

		if(!strcmp(gcFind," >"))
		{
			//If NOT on last page show next page
			if( gluRowid >= guN-1 )
			{
				//If on last page adjust end
				guStart= ((guI/SHOWPAGE)*SHOWPAGE) + 1;
				guEnd=guI;
			}
			else
			{
				guStart=((gluRowid)*SHOWPAGE)+1;
				guEnd=guStart+SHOWPAGE-1;
				gluRowid++;
			}
		}
		else if(!strcmp(gcFind,"< "))
		{
			if(gluRowid>1 )
			{
				gluRowid--;
				guStart=(gluRowid)*SHOWPAGE-SHOWPAGE+1;
				guEnd=guStart+SHOWPAGE-1;
			}
			else
			{
				guStart=1;
				if(guI > SHOWPAGE)
				{
					guEnd=SHOWPAGE;
				}
				else
				{
					guEnd=guI;
				}
				gluRowid=1;
			}
		}
		else if(!strcmp(gcFind,">>"))
		{
			guStart= ((guI/SHOWPAGE)*SHOWPAGE) + 1;
			guEnd=guI;
			gluRowid=guN;
		}
		else if(1)
		{
			guStart=1;
			if(guI > SHOWPAGE)
			{
				guEnd=SHOWPAGE;
			}
			else
			{
				guEnd=guI;
			}
			gluRowid=1;
		}
	
		guListMode=1;
		Header_ism3(cFuncName,0);
	
		if(!guI) 
	        {
	                printf(LANG_PAGEMACHINE_HINT);
	
	        }
	 
	
	        printf(LANG_PAGEMACHINE_SHOWING,1+(guStart/SHOWPAGE),guN,guStart,guEnd,guI);
	
	
		printf("<input type=hidden name=gluRowid value=%lu>",gluRowid);
		printf("<input type=hidden name=gcFunction value=%s >",cFuncName);
		printf("<input type=hidden name=guListMode value=1 >\n");
	}
	else
	{
		//Page mode
		//on entry guI has number of rows
		//on entry gluRowid has current position unless guI=1
		//if guI=1 then we need to figure out real guI

		guN=guI;
	
		if(gluRowid<1)
		{
			gluRowid=1;
			return;
		}
		
		if(!strcmp(gcFind," >"))
		{
			//If on last page stay there
			if( gluRowid >= guI )
			{
				//If on last page adjust guEnd
				gluRowid=guI;
			}
			else
			{
				gluRowid++;
			}
		}
		else if(!strcmp(gcFind,"< "))
		{
			if(gluRowid>1 )
			{
				gluRowid--;
			}
			else
			{
				gluRowid=1;
			}
		}
		else if(!strcmp(gcFind,">>"))
		{
			gluRowid=guI;
		}
		else if(!strcmp(gcFind,"<<"))
		{
			gluRowid=1;
		}
		else if(1)
		{
			//If on last page stay there
			if( gluRowid >= guI )
			{
				gluRowid=guI;
			}
		}

	}//guEnd iLmode

}//PageMachine()


void OpenFieldSet(char *cLabel, unsigned uWidth)
{
	printf("<fieldset><legend><b>%s</b></legend><table width=%u%%>\n",cLabel,uWidth);

}//void OpenFieldSet()


void CloseFieldSet(void)
{
	printf("</table></fieldset>\n");

}//void CloseFieldSet(void)


void OpenRow(const char *cFieldLabel, const char *cColor)
{
	//Skip underlined cFieldLabel for tGlossary.cLabel use.
	if(cFieldLabel[0]=='<')
	{
		register int j;
		char cGlossary[33]="";

		for(j=3;cFieldLabel[j]&&j<32;j++)
		{
			if(!isalnum(cFieldLabel[j]) && cFieldLabel[j]!=' ')
			{
				cGlossary[j]=0;
				break;
			}
			else
			{
				cGlossary[j]=cFieldLabel[j];
			}
		}
		printf("<tr><td width=20%% valign=top><a class=darkLink "
			"href=# onClick=\"open_popup('?gcFunction=Glossary&cLabel=%2$s')\">"
			"<font color=%1$s>%3$s</font></td><td>",cColor,cGlossary+3,cFieldLabel);
	}
	else
	{
		printf("<tr><td width=20%% valign=top><a class=darkLink "
			"href=# onClick=\"open_popup('?gcFunction=Glossary&cLabel=%2$s')\">"
			"<font color=%1$s>%2$s</font></td><td>",cColor,cFieldLabel);
	}

}//void OpenRow()



void tTablePullDownOwner(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode)
{
        register int i,n;
        char cLabel[256];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[100]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTablePullDownOwner()");
                return;
        }

        //Extended functionality
        strncpy(cLocalTableName,cTableName,255);
        if((cp=strchr(cLocalTableName,';')))
        {
                strncpy(cSelectName,cp+1,99);
                cSelectName[99]=0;
                *cp=0;
        }


	if(uSelector && !uMode)
	{
		if(guLoginClient==1)
        		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE _rowid=%u",
                                cFieldName,cLocalTableName,uSelector);
		else
        		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uOwner=%u OR uOwner IN"
				" (SELECT uClient FROM " TCLIENT " WHERE uOwner=%u) AND _rowid=%u",
				cFieldName,cLocalTableName,guCompany,guCompany,uSelector);
	}
	else
	{
		if(guLoginClient==1)
        		sprintf(gcQuery,"SELECT _rowid,%s FROM %s ORDER BY %s",
                                cFieldName,cLocalTableName,cOrderby);
		else
        		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uOwner=%u OR uOwner IN"
				" (SELECT uClient FROM " TCLIENT " WHERE uOwner=%u) ORDER BY %s",
				cFieldName,cLocalTableName,guCompany,guCompany,cOrderby);
	}

	MYSQL_RUN_STORE_TEXT_RET_VOID(mysqlRes);
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
                printf("<select name=%s %s>\n",cLabel,cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                        {
                             printf("<option>%s</option>\n",mysqlField[1]);
                        }
                        else
                        {
                             printf("<option selected>%s</option>\n",mysqlField[1]);
			     if(!uMode)
			     sprintf(cHidden,"<input type=hidden name=%.99s value='%.99s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%99s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDownOwner()


void tTablePullDown(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode)
{
        register int i,n;
        char cLabel[256];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[100]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTablePullDown()");
                return;
        }

        //Extended functionality
        strncpy(cLocalTableName,cTableName,255);
        if((cp=strchr(cLocalTableName,';')))
        {
                strncpy(cSelectName,cp+1,99);
                cSelectName[99]=0;
                *cp=0;
        }


	if(uSelector && !uMode)
        	sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE _rowid=%u",
                                cFieldName,cLocalTableName,uSelector);
	else
        	sprintf(gcQuery,"SELECT _rowid,%s FROM %s ORDER BY %s",
                                cFieldName,cLocalTableName,cOrderby);

	MYSQL_RUN_STORE_TEXT_RET_VOID(mysqlRes);
	
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
                printf("<select name=%s %s>\n",cLabel,cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                        {
                             printf("<option>%s</option>\n",mysqlField[1]);
                        }
                        else
                        {
                             printf("<option selected>%s</option>\n",mysqlField[1]);
			     if(!uMode)
			     sprintf(cHidden,"<input type=hidden name=%.99s value='%.99s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%.99s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDown()


int ReadPullDown(const char *cTableName,const char *cFieldName,const char *cLabel)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

        unsigned int iRowid=0;//Not found

        sprintf(gcQuery,"SELECT _rowid FROM %s WHERE %s='%s'",
                        cTableName,cFieldName,TextAreaSave((char *) cLabel));
        MYSQL_RUN_STORE(mysqlRes);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sscanf(mysqlField[0],"%u",&iRowid);
        mysql_free_result(mysqlRes);
        return(iRowid);

}//ReadPullDown(char *cTableName,char *cLabel)


int ReadPullDownOwner(const char *cTableName,const char *cFieldName,
				const char *cLabel,const unsigned uOwner)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

        unsigned int iRowid=0;//Not found

        sprintf(gcQuery,"SELECT _rowid FROM %s WHERE %s='%s' AND (uOwner=%u OR uOwner=%u)",
                        cTableName,cFieldName,TextAreaSave((char *) cLabel),uOwner,guCompany);
        MYSQL_RUN_STORE(mysqlRes);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sscanf(mysqlField[0],"%u",&iRowid);
        mysql_free_result(mysqlRes);
        return(iRowid);

}//ReadPullDownOwner()


char *TextAreaSave(char *cField)
{
	register unsigned int i=0,j=0,uNum=0,uCtrlM=0;
	char *cCopy=NULL;

	for(i=0;cField[i];i++)
	{
		if(cField[i]=='\'' || cField[i]=='\\') uNum++;
		if(cField[i]=='\r') uCtrlM++;
	}
	if(!uNum && !uCtrlM) return(cField);

	if(uNum)
		cCopy=(char *)malloc( ( (strlen(cField)) + (uNum*2) + 1 ));
	else
		cCopy=(char *)cField;

	if(!cCopy) unxsVZ("TextAreaInput() malloc error");

	i=0;
	while(cField[i])
	{
		if( cField[i]=='\'' )
		{
			cCopy[j++]='\\';
			cCopy[j++]='\'';
		}
		else if( cField[i]=='\\')
		{
			cCopy[j++]='\\';
			cCopy[j++]='\\';
		}
		
		//Remove nasty ctrl-m's. fsck /u Bill!
		else if(cField[i]!='\r')
		{
			cCopy[j++]=cField[i];
		}
		i++;
	}

	cCopy[j]=0;
	return(cCopy);

}//char *TextAreaSave(char *cField)


char *TransformAngleBrackets(char *cField)
{
	register unsigned int i=0,j=0,uNum=0;
	char *cCopy=NULL;

	for(i=0;cField[i];i++)
		if(cField[i]=='<' || cField[i]=='>') uNum++;
	if(!uNum) return(cField);
	cCopy=(char *)malloc( ( (strlen(cField)) + (uNum*4) + 1 ));

	if(!cCopy) unxsVZ("TransformAngleBrackets() malloc error");

	i=0;
	while(cField[i])
	{
		//Expand angle brackets into HTML codes
		if( cField[i]=='<' )
		{
			cCopy[j++]='&';
			cCopy[j++]='l';
			cCopy[j++]='t';
			cCopy[j++]=';';
		}
		else if( cField[i]=='>' )
		{
			cCopy[j++]='&';
			cCopy[j++]='g';
			cCopy[j++]='t';
			cCopy[j++]=';';
		}
		else if(1)
                {
                        cCopy[j++]=cField[i];
                }
		i++;
	}

	cCopy[j]=0;
	return(cCopy);

}//char *TransformAngleBrackets(char *cField)


char *EncodeDoubleQuotes(char *cField)
{
	register unsigned int i=0,j=0,uNum=0;
	char *cCopy=NULL;

	for(i=0;cField[i];i++)
		if(cField[i]=='"') uNum++;
	if(!uNum) return(cField);
	cCopy=(char *)malloc( ( (strlen(cField)) + (uNum*5) + 1 ));

	if(!cCopy) unxsVZ("EncodeDoubleQuotes() malloc error");

	i=0;
	while(cField[i])
	{
		//Expand double quote into HTML codes
		if( cField[i]=='"' )
		{
			cCopy[j++]='&';
			cCopy[j++]='q';
			cCopy[j++]='u';
			cCopy[j++]='o';
			cCopy[j++]='t';
			cCopy[j++]=';';
		}
		else if(1)
                {
                        cCopy[j++]=cField[i];
                }
		i++;
	}

	cCopy[j]=0;
	return(cCopy);

}//char *EncodeDoubleQuotes(char *cField)


void YesNo(unsigned uSelect)
{
        if(uSelect)
                printf("Yes");
        else
                printf("No");

}//YesNo()


void YesNoPullDown(char *cFieldName, unsigned uSelect, unsigned uMode)
{
	char cHidden[100]={""};
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
	printf("<select name=cYesNo%s %s>\n",cFieldName,cMode);

        if(uSelect==0)
                printf("<option selected>No</option>\n");
        else
                printf("<option>No</option>\n");

        if(uSelect==1)
	{
                printf("<option selected>Yes</option>\n");
		if(!uMode)
			sprintf(cHidden,"<input type=hidden name=cYesNo%s value='Yes'>\n",
			     		cFieldName);
	}
        else
	{
                printf("<option>Yes</option>\n");
	}

        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//YesNoPullDown()


int ReadYesNoPullDown(const char *cLabel)
{
        if(!strcmp(cLabel,"Yes"))
                return(1);
        else
                return(0);

}//ReadYesNoPullDown(char *cLabel)


const char *ForeignKey2(const char *cTableName, const char *cFieldName, unsigned uKey)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

        sprintf(gcQuery,"SELECT %s FROM %s WHERE _rowid=%u",
                        cFieldName,cTableName,uKey);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql)) return(mysql_error(&gMysql));

        mysqlRes=mysql_store_result(&gMysql);
        if(mysql_num_rows(mysqlRes)==1)
        {
                mysqlField=mysql_fetch_row(mysqlRes);
                return(mysqlField[0]);
        }
	else
	{
        	return("---");
	}

}//const char *ForeignKey2(const char *cTableName, const char *cFieldName, unsigned uKey)


void InitialInstall(void)
{
	unxsVZ("Please run\
		unxsVZ.cgi Initialize &lt;mysqlpwd&gt; from the command line");

}//void InitialInstall(void)



 //tValidFunc functions: Form validation feedback

const char *IsZero(unsigned uInput)
{
        if(uInput)
                return("black");
        else
                return("red");

}//const char *IsZero(unsigned uInput)

 
const char *BadIPNum(const char *cInput)
{ 
        if( cInput!=NULL && cInput[0] && strcmp(cInput,"0.0.0.0"))
                return("black");
        else
                return("red");

}//const char *BadIPNum(const char *cInput)


const char *EmptyString(const char *cInput)
{
        if(cInput!=NULL && cInput[0])
                return("black");
        else
                return("red");

}//const char *EmptyString(const char *cInput) 


//tInputFunc functions: Convert data on cgi form post

char *IPNumber(char *cInput)
{
	unsigned a=0,b=0,c=0,d=0;

	sscanf(cInput,"%u.%u.%u.%u",&a,&b,&c,&d);

	if(a>255) a=0;
	if(b>255) b=0;
	if(c>255) c=0;
	if(d>255) d=0;

	sprintf(cInput,"%u.%u.%u.%u",a,b,c,d);

	return(cInput);

}//char *IPNumber(char *cInput)


char *IPv4CIDR(char *cInput)
{
	unsigned a=0,b=0,c=0,d=0,e=0;

	sscanf(cInput,"%u.%u.%u.%u/%u",&a,&b,&c,&d,&e);

	if(a>255) a=0;
	if(b>255) b=0;
	if(c>255) c=0;
	if(d>255) d=0;
	if(e>32) e=32;
	if(e<20) e=20;

	//TODO ggw 8/2003 CIDR alignment math for a.b.c.d vs CIDR range 20-32
	//16 class C's - one IP

	sprintf(cInput,"%u.%u.%u.%u/%u",a,b,c,d,e);

	return(cInput);

}//char *IPv4CIDR(char *cInput)


char *IPv4Range(char *cInput)
{
	unsigned a=0,b=0,c=0,d=0,e=0;

	sscanf(cInput,"%u.%u.%u.%u-%u",&a,&b,&c,&d,&e);

	if(a>255) a=0;
	if(b>255) b=0;
	if(c>255) c=0;
	if(d>255) d=0;
	if(e>255) e=0;
	if(e<d) e=d;

	sprintf(cInput,"%u.%u.%u.%u-%u",a,b,c,d,e);

	return(cInput);

}//char *IPv4Range(char *cInput)


char *IPv4All(char *cInput)
{
	if(strchr(cInput,'-'))
		IPv4Range(cInput);
	else if(strchr(cInput,'/'))
		IPv4CIDR(cInput);
	else if(1)
		IPNumber(cInput);

	return(cInput);

}//char *IPv4All(char *cInput)


char *EmailInput(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
	{
	
		if(!isalnum(cInput[i]) && cInput[i]!='.'  && cInput[i]!='-' 
				&& cInput[i]!='@' && cInput[i]!='_')
			break;
		if(isupper(cInput[i])) cInput[i]=tolower(cInput[i]);
	}
	cInput[i]=0;

	return(cInput);

}//char *EmailInput(char *cInput)


char *cMoneyInput(char *cInput)
{
	register int i,j;
	char cOutput[32];

	//Allow only certain chars nums $ , and . Ex. $250,000.00
	for(i=0;cInput[i];i++)
	{
		if(!isdigit(cInput[i]) && cInput[i]!='.'  && cInput[i]!=',' 
				&& cInput[i]!='$')
			break;
	}
	cInput[i]=0;

	//Strip non internal chars
	for(i=0,j=0;cInput[i];i++)
	{
		if(cInput[i]==',' || cInput[i]=='$')
		{
			;
		}
		else
		{
			cOutput[j++]=cInput[i];
		}
	}
	cOutput[j]=0;
	sprintf(cInput,"%.31s",cOutput);

	//Only allow one .
	unsigned uCountPeriods=0;
	for(i=strlen(cInput);i>0;i--)
	{
		if(cInput[i]=='.')
		{
			uCountPeriods++;
			if(uCountPeriods>1)
				cInput[i]=',';
		}
	}
	if(uCountPeriods>1)
		cMoneyInput(cInput);

	return(cInput);

}//char *cMoneyInput(char *cInput)

	
char *cMoneyDisplay(char *cInput)
{
	double fBuffer;
	int i;
	//
	//We need to convert to double before calling strfmon
	fBuffer=atof(cInput);
	i=strlen(cInput);
	i+=3;
	//Of course you may change the locale if appropiate.
        setlocale(LC_MONETARY, "en_US");
	strfmon(cInput,i, "%n",fBuffer); 
	
	return(cInput);

}//char *cMoneyDisplay(char *cInput)


char *FQDomainName(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
	{
	
		if(!isalnum(cInput[i]) && cInput[i]!='.'  && cInput[i]!='-' )
			break;
		if(isupper(cInput[i])) cInput[i]=tolower(cInput[i]);
	}
	cInput[i]=0;

	return(cInput);

}//char *FQDomainName(char *cInput)


char *WordToLower(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
	{
	
		if(!isalnum(cInput[i]) && cInput[i]!='_' && cInput[i]!='-'
				&& cInput[i]!='@' && cInput[i]!='.' ) break;
		if(isupper(cInput[i])) cInput[i]=tolower(cInput[i]);
	}
	cInput[i]=0;

	return(cInput);

}//char *WordToLower(char *cInput)


//SSLCookieLogin() Functions
void SetLogin(void)
{
	if( iValidLogin(0) )
	{
		printf("Set-Cookie: unxsVZLogin=%s;\n",gcLogin);
		printf("Set-Cookie: unxsVZPasswd=%s;\n",gcPasswd);
		strncpy(gcUser,gcLogin,41);
		GetPLAndClient(gcUser);
		guSSLCookieLogin=1;
		if(guPermLevel>6)
			unxsVZ("DashBoard");
		else if(guPermLevel==6)
			unxsVZ("Welcome Organization Admin");
		else
			unxsVZ("Welcome");
	}
	else
	{
		guSSLCookieLogin=0;
		SSLCookieLogin();
	}
				
}//void SetLogin(void)


unsigned uValidOTP(char *cOTPSecret,char *cOTP)
{
	char *secret;
	size_t secretlen = 0;
	int rc;
	char otp[10];
	time_t now;

	rc=oath_init();
	if(rc!=OATH_OK)
		return(0);

	now=time(NULL);

	rc=oath_base32_decode(cOTPSecret,strlen(cOTPSecret),&secret,&secretlen);
	if(rc!=OATH_OK)
		goto gotoFail;

	//2 min time skew window
	rc=oath_totp_generate(secret,secretlen,now-60,30,0,6,otp);
	if(rc!=OATH_OK)
		goto gotoFail;
	if(!strcmp(cOTP,otp))
		goto gotoMatch;

	rc=oath_totp_generate(secret,secretlen,now-30,30,0,6,otp);
	if(rc!=OATH_OK)
		goto gotoFail;
	if(!strcmp(cOTP,otp))
		goto gotoMatch;

	rc=oath_totp_generate(secret,secretlen,now,30,0,6,otp);
	if(rc!=OATH_OK)
		goto gotoFail;
	if(!strcmp(cOTP,otp))
		goto gotoMatch;

	rc=oath_totp_generate(secret,secretlen,now+30,30,0,6,otp);
	if(rc!=OATH_OK)
		goto gotoFail;
	if(!strcmp(cOTP,otp))
		goto gotoMatch;

	rc=oath_totp_generate(secret,secretlen,now+60,30,0,6,otp);
	if(rc!=OATH_OK)
		goto gotoFail;
	if(!strcmp(cOTP,otp))
		goto gotoMatch;

gotoFail:
	free(secret);
	oath_done();
	return(0);

gotoMatch:
	free(secret);
	oath_done();
	return(1);

}//unsigned uValidOTP(char *cOTPSecret,char *cOTP)


//with uAuthorize==0 it expires the OTP for a given guLoginClient
void UpdateOTPExpire(unsigned uAuthorize,unsigned uClient)
{

	//OTP login OK for 4 more hours. Change to configurable TODO.
	if(!uAuthorize)
		sprintf(gcQuery,"UPDATE " TAUTHORIZE " SET uOTPExpire=0 WHERE uCertClient=%u",
			uClient);
	else
		sprintf(gcQuery,"UPDATE " TAUTHORIZE " SET uOTPExpire=(UNIX_TIMESTAMP(NOW())+28800) WHERE uAuthorize=%u",
			uAuthorize);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
}//void UpdateOTPExpire()


char *cGetPasswd(char *gcLogin,char *cOTPSecret,unsigned long *luOTPExpire,unsigned long *luSQLNow,unsigned *uAuthorize)
{
	static char cPasswd[100]={""};
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	char *cp;

	//SQL injection code
	if((cp=strchr(gcLogin,'\''))) *cp=0;

	sprintf(gcQuery,"SELECT cPasswd,cOTPSecret,uOTPExpire,UNIX_TIMESTAMP(NOW()),uAuthorize"
				" FROM " TAUTHORIZE " WHERE cLabel='%s'",gcLogin);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	mysqlRes=mysql_store_result(&gMysql);
	cPasswd[0]=0;
	if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sprintf(cPasswd,"%.99s",mysqlField[0]);

		if(mysqlField[1])
		{
			sprintf(cOTPSecret,"%.64s",mysqlField[1]);
			sscanf(mysqlField[2],"%lu",luOTPExpire);
			sscanf(mysqlField[3],"%lu",luSQLNow);
			sscanf(mysqlField[4],"%u",uAuthorize);
		}
	}
	mysql_free_result(mysqlRes);
	
	return(cPasswd);

}//char *cGetPasswd()


int iValidLogin(int mode)
{
	char cSalt[16]={""};
	char cPassword[100]={""};

	//Notes:
	//mode=1 means we have encrypted passwd from cookie

	unsigned uAuthorize=0;
	long unsigned luOTPExpire=0;
	long unsigned luSQLNow=0;
	strcpy(cPassword,cGetPasswd(gcLogin,gcOTPSecret,&luOTPExpire,&luSQLNow,&uAuthorize));
	//If user has OTP secret then they must login with OTP every so often.
	if(luOTPExpire<=luSQLNow && gcOTPSecret[0])
		guOTPExpired=1;
	sprintf(gcOTPInfo,"{%s}/[%s] %u unexpected case",gcOTPSecret,gcOTP,guOTPExpired);
	if(cPassword[0])
	{
		//No cookies!
		if(!mode)
		{

			//MD5 vs DES salt determination
			if(cPassword[0]=='$' && cPassword[2]=='$')
				sprintf(cSalt,"%.12s",cPassword);
			else
				sprintf(cSalt,"%.2s",cPassword);
			EncryptPasswdWithSalt(gcPasswd,cSalt);
			if(!strcmp(gcPasswd,cPassword)) 
			{
				guCompany=1;//If next line does not work
				GetPLAndClient(gcLogin);
				//guPermLevel=0;
				//guLoginClient=0;
				//tLogType.cLabel='backend login'->uLogType=6
				//Alpha testing ON DUPLICATE KEY UPDATE to avoid some replication problems
				//that I have seen in logfiles.
				sprintf(gcQuery,"INSERT INTO tLog SET cLabel='login ok %.99s',uLogType=6,uPermLevel=%u,"
					" uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=%u,"
					" uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW()) ON DUPLICATE KEY UPDATE"
					" cLabel='login ok %.99s',uLogType=6,uPermLevel=%u,"
					" uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=%u,"
					" uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname,guCompany,
						gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname,guCompany);
				MYSQL_RUN;
				LoginFirewallJobs(guLoginClient);
				if(guOTPExpired && gcOTP[0] && gcOTPSecret[0])
				{
					if(!uValidOTP(gcOTPSecret,gcOTP))
					{
						guRequireOTPLogin=1;
						sprintf(gcOTPInfo,"{%s}/[%s] %u login invalid gcOTP",gcOTPSecret,gcOTP,guOTPExpired);
						return(0);
					}
					else
					{
						guRequireOTPLogin=0;
						guOTPExpired=0;
						UpdateOTPExpire(uAuthorize,0);
						sprintf(gcOTPInfo,"{%s}/[%s] %u login valid gcOTP",gcOTPSecret,gcOTP,guOTPExpired);
						return(1);
					}
				}
				else if(guOTPExpired)
				{
					guRequireOTPLogin=1;
					sprintf(gcOTPInfo,"{%s}/[%s] %u login valid but expired",gcOTPSecret,gcOTP,guOTPExpired);
					return(0);
				}
				sprintf(gcOTPInfo,"{%s}/[%s] %u login valid",gcOTPSecret,gcOTP,guOTPExpired);
				return(1);
			}
		}
		//Cookies supplied gcPasswd
		else
		{
			if(!strcmp(gcPasswd,cPassword))
			{
				if(guOTPExpired && gcOTP[0] && gcOTPSecret[0])
				{
					if(!uValidOTP(gcOTPSecret,gcOTP))
					{
						guRequireOTPLogin=1;
						sprintf(gcOTPInfo,"{%s}/[%s] %u cookie login expired invalid gcOTP",gcOTPSecret,gcOTP,guOTPExpired);
						return(0);
					}
					else
					{
						guOTPExpired=0;
						guRequireOTPLogin=0;
						UpdateOTPExpire(uAuthorize,0);
						sprintf(gcOTPInfo,"{%s}/[%s] %u cookie login valid gcOTP",gcOTPSecret,gcOTP,guOTPExpired);
						return(1);
					}
				}
				else if(guOTPExpired)
				{
					guRequireOTPLogin=1;
					sprintf(gcOTPInfo,"{%s}/[%s] %u cookie login expired no gcOTP",gcOTPSecret,gcOTP,guOTPExpired);
					return(0);
				}
				sprintf(gcOTPInfo,"{%s}/[%s] %u cookie login valid",gcOTPSecret,gcOTP,guOTPExpired);
				return(1);
			}
		}
	}

	//No cookies and passwords did not match OR no password at all
	if(!mode)
	{
		guCompany=1;//If next line does not work
		GetPLAndClient(gcLogin);
		//guPermLevel=0;
		//guLoginClient=0;
		sprintf(gcQuery,"INSERT INTO tLog SET cLabel='login failed %.99s',uLogType=6,uPermLevel=%u,"
				" uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=%u,"
				" uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW()) ON DUPLICATE KEY UPDATE"
				" cLabel='login failed %.99s',uLogType=6,uPermLevel=%u,"
				" uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=%u,"
				" uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname,guCompany,
					gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname,guCompany);
		MYSQL_RUN;
	}

	if(guOTPExpired)
		guRequireOTPLogin=1;
	sprintf(gcOTPInfo,"{%s}/[%s] %u invalid login",gcOTPSecret,gcOTP,guOTPExpired);
	return 0;

}//iValidLogin()



void SSLCookieLogin(void)
{
	char *ptr,*ptr2;

	//Parse out login and passwd from cookies
#ifdef SSLONLY
	if(getenv("HTTPS")==NULL) 
		unxsVZ("Non SSL access denied");
#endif

	if(getenv("HTTP_COOKIE")!=NULL)
		strncpy(gcCookie,getenv("HTTP_COOKIE"),1022);
	
	if(gcCookie[0])
	{

	if((ptr=strstr(gcCookie,"unxsVZLogin=")))
	{
		ptr+=strlen("unxsVZLogin=");
		if((ptr2=strchr(ptr,';')))
		{
			*ptr2=0;
			strncpy(gcLogin,ptr,99);
			*ptr2=';';
		}
		else
		{
			strncpy(gcLogin,ptr,99);
		}
	}
	if((ptr=strstr(gcCookie,"unxsVZPasswd=")))
	{
		ptr+=strlen("unxsVZPasswd=");
		if((ptr2=strchr(ptr,';')))
		{
			*ptr2=0;
			sprintf(gcPasswd,"%.99s",ptr);
			*ptr2=';';
		}
		else
		{
			sprintf(gcPasswd,"%.99s",ptr);
		}
	}
	
	}//if gcCookie[0] time saver

	if(!iValidLogin(1))
		htmlSSLLogin();

	strncpy(gcUser,gcLogin,41);
	GetPLAndClient(gcUser);
	if(!guPermLevel || !guLoginClient)
		unxsVZ("Access denied");
	gcPasswd[0]=0;
	guSSLCookieLogin=1;

}//SSLCookieLogin()


void htmlSSLLogin(void)
{
        Header_ism3("",0);

	printf("<p>\n");
	printf("Login: <input type=text title='Enter your login name' size=20 maxlength=98 name=gcLogin>\n");
	printf(" Passwd: <input type=password title='Enter your password' size=20 maxlength=20 name=gcPasswd>\n");
	if(guRequireOTPLogin)
		printf(" Validation code: <input type=text size=8 maxlength=8"
			" title='Enter your 6 digit one time password. Download google authenticator"
			" or similar. Ask your admin for the barcode or secret.' name=gcOTP autocomplete=off >\n");
	printf("<font size=1> <input type=submit name=gcFunction value=Login>\n");

        Footer_ism3();

}//void htmlSSLLogin(void)


void EncryptPasswdWithSalt(char *gcPasswd, char *cSalt)
{
	char cPasswd[100]={""};
	char *cp;
			
	sprintf(cPasswd,"%.99s",gcPasswd);
	cp=crypt(cPasswd,cSalt);
	sprintf(gcPasswd,"%.99s",cp);

//Debug only
//printf("Content-type: text/html\n\n");
//printf("gcPasswd=(%s),cSalt=(%s)",gcPasswd,cSalt);
//exit(0);

}//void EncryptPasswdWithSalt(char *gcPasswd, char *cSalt)


void GetPLAndClient(char *cUser)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

	//SQL FROM the defined external tables must provide db.tAuthorize and db.tClient for the other SQL
	// to work.
	sprintf(gcQuery,"SELECT tAuthorize.uPerm,tAuthorize.uCertClient,tAuthorize.uOwner,"
				"tClient.cLabel"
				" FROM " TAUTHORIZE "," TCLIENT
				" WHERE tAuthorize.uOwner=tClient.uClient"
				" AND tAuthorize.cLabel='%s'",cUser);
	MYSQL_RUN_STORE(mysqlRes);
	if(mysql_num_rows(mysqlRes))
	{
		mysqlField=mysql_fetch_row(mysqlRes);
		sscanf(mysqlField[0],"%d",&guPermLevel);
		sscanf(mysqlField[1],"%u",&guLoginClient);
		sscanf(mysqlField[2],"%u",&guCompany);
		sprintf(gcCompany,"%.99s",mysqlField[3]);
	}
	mysql_free_result(mysqlRes);

}//void GetPLAndClient()


void GetClientOwner(unsigned uClient, unsigned *uOwner)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	char cQuery[512]={""};
	
        sprintf(cQuery,"SELECT uOwner FROM " TCLIENT " WHERE uClient=%u",uClient);
        mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	mysqlRes=mysql_store_result(&gMysql);

	*uOwner=0;
        if((mysqlField=mysql_fetch_row(mysqlRes)))
                sscanf(mysqlField[0],"%u",uOwner);
        mysql_free_result(mysqlRes);

}//void GetClientOwner(unsigned uClient, unsigned *uOwner)


long unsigned luGetCreatedDate(char *cTableName, unsigned uTablePK)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	long unsigned luCreatedDate=0;
	char cQuery[512]={""};
	
        sprintf(cQuery,"SELECT uCreatedDate FROM %s WHERE _rowid=%u",
						cTableName,uTablePK);
        mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	mysqlRes=mysql_store_result(&gMysql);
	
	if((mysqlField=mysql_fetch_row(mysqlRes)))
                sscanf(mysqlField[0],"%lu",&luCreatedDate);
        mysql_free_result(mysqlRes);

	return(luCreatedDate);

}//long unsigned luGetCreatedDate(char *cTableName, unsigned uTablePK)


long unsigned luGetModDate(char *cTableName, unsigned uTablePK)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	long unsigned luModDate=0;
	char cQuery[512]={""};
	
        sprintf(cQuery,"SELECT uModDate FROM %s WHERE _rowid=%u",
						cTableName,uTablePK);
        mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	mysqlRes=mysql_store_result(&gMysql);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
                sscanf(mysqlField[0],"%lu",&luModDate);
        mysql_free_result(mysqlRes);

	return(luModDate);

}//long unsigned luGetModDate(char *cTableName, unsigned uTablePK)


void htmlPlainTextError(const char *cError)
{
	printf("Content-type: text/plain\n\n");
	printf("Please report this unxsVZ fatal error ASAP:\n%s\n",cError);

	//Attempt to report error in tLog
        sprintf(gcQuery,"INSERT INTO tLog SET cLabel='htmlPlainTextError',uLogType=4,uPermLevel=%u,"
			" uLoginClient=%u,cLogin='%s',cHost='%s',cMessage=\"%s (%.24s)\",cServer='%s',"
			" cHash=MD5(CONCAT('%u','%u','%s','%s',\"%s (%.24s)\",'%s',UNIX_TIMESTAMP(NOW()),"
			" '%s')),uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uTablePK=%u,cTableName='errno'",
			guPermLevel,
			guLoginClient,
			gcLogin,
			gcHost,
			cError,
			gcQuery,
			gcHostname,
			guPermLevel,
			guLoginClient,
			gcLogin,
			gcHost,
			cError,
			gcQuery,
			gcHostname,
			cLogKey,
			guCompany,
			guLoginClient,
			mysql_errno(&gMysql));
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		printf("Another error occurred while attempting to log: %s\n",
				mysql_error(&gMysql));
	exit(0);

}//void htmlPlainTextError(const char *cError)
			

//This algo is based on GPL work in php-4.0.4p11 url.c and FSF face file urlencode.c
//Then modified to suit our needs and readability and UK style guidelines
char *cURLEncode(char *cURL)
{
        register int x,y;
        unsigned char *cp;
        int len=strlen(cURL);

        static unsigned char hexchars[] = "0123456789ABCDEF";

        cp=(unsigned char *)malloc(3*strlen(cURL)+1);
        for(x=0,y=0;len--;x++,y++)
        {
                cp[y]=(unsigned char)cURL[x];
                if(cp[y]==' ')
                {
                        cp[y]='+';
                }
                else if( (cp[y]<'0' && cp[y]!='-' && cp[y]!='.') ||
                                (cp[y]<'A' && cp[y]>'9') ||
                                (cp[y]>'Z' && cp[y]<'a' && cp[y]!='_') ||
                                (cp[y]>'z') )
                {
                        cp[y++]='%';
                        cp[y++]=hexchars[(unsigned char) cURL[x] >> 4];
                        cp[y]=hexchars[(unsigned char) cURL[x] & 15];
                }
        }

        cp[y]='\0';
        return((char *)cp);

}//char *cURLEncode(char *cURL)

//Starting cleanup of current client contact role permissions model
//Description
//1-. Root user == 1. Initial install super user, sets up root tClient ASP company
//	and thus the uOwner of this tClient entry is root. There should be only one
//	such company but this is not mandatory. This company will have the only 
//	contacts that may have access to everything.
//	Root user if owner of a record blocks delete operations done by others.
//2-. If record uOwner is 0 something is broken and only Root can delete or modify.
//3-. Any contact of a company with perm level >= admin can mod any record
//	owned by parent company. For delete with perm level >= root.
//4-. Any company contact with perm level >= root can delete or mod any record
//	of companies owned by parent company.
//5-. Any contact of perm level >=user that created a record and is owned by parent company can mod that record.
//	Similarly for delete if user is admin level
//
//7 user level
//10 admin level
//12 root level
unsigned uAllowDel(const unsigned uOwner, const unsigned uCreatedBy)
{
	//Set guReseller for tests to see if the owner of the owner is guCompany.
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	if( (guPermLevel>10 && uOwner==guCompany) //r3
				|| (guPermLevel>9 && guCompany==guReseller) //r4
				|| (guPermLevel>9 && uCreatedBy==guLoginClient && 
					(uOwner==guCompany || guCompany==guReseller)) //r5
				|| (guPermLevel>11 && guCompany==1) //r1
				|| (guPermLevel>11 && guLoginClient==1) )//r2
			return(1);
	return(0);
}//unsigned uAllowDel(...)


unsigned uAllowMod(const unsigned uOwner, const unsigned uCreatedBy)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);
	if( (guPermLevel>9 && uOwner==guCompany) //r3
				|| (guPermLevel>9 && guCompany==guReseller) //r4
				|| (guPermLevel>6 && uCreatedBy==guLoginClient && 
					(uOwner==guCompany || guCompany==guReseller)) //r5
				|| (guPermLevel>11 && guCompany==1) //r1
				|| (guPermLevel>11 && guLoginClient==1) )//r2
			return(1);
	return(0);
}//unsigned uAllowMod(...)


void ExtListSelect(const char *cTable,const char *cVarList)
{
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM %s",
					cVarList,cTable);
	else 
		sprintf(gcQuery,"SELECT %1$s FROM %3$s," TCLIENT
				" WHERE %3$s.uOwner=tClient.uClient"
				" AND (tClient.uClient=%2$u OR tClient.uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))",
					cVarList,guCompany,cTable);
}//void ExtListSelect(...)


void ExtListSelectPublic(const char *cTable,const char *cVarList)
{
		sprintf(gcQuery,"SELECT %s FROM %s",cVarList,cTable);
}//void ExtListSelectPublic(...)


void ExtSelect(const char *cTable,const char *cVarList)
{
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		//sprintf(gcQuery,"SELECT %s FROM %s ORDER BY u%s",
		sprintf(gcQuery,"SELECT %s FROM %s",
					//cVarList,cTable,cTable+1);
					cVarList,cTable);
	else 
		sprintf(gcQuery,"SELECT %1$s FROM %3$s," TCLIENT
				" WHERE %3$s.uOwner=tClient.uClient"
				" AND (tClient.uClient=%2$u OR tClient.uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))",
				//" ORDER BY u%4$s",
					cVarList,guCompany,
					cTable);
					//cTable,cTable+1);
}//void ExtSelect(...)


//For those tables you want anybody to be able to read. Like tStatus.
void ExtSelectPublic(const char *cTable,const char *cVarList)
{
	sprintf(gcQuery,"SELECT %.1024s FROM %.99s ORDER BY _rowid",cVarList,cTable);

}//void ExtSelectPublic(...)


void ExtSelectPublicOrder(const char *cTable,const char *cVarList,const char *cOrderBy)
{
	sprintf(gcQuery,"SELECT %.1024s FROM %.99s ORDER BY %.99s",cVarList,cTable,cOrderBy);

}//void ExtSelectPublicOrder(...)



void ExtSelectSearchPublic(const char *cTable,const char *cVarList,const char *cSearchField,const char *cSearch)
{
	sprintf(gcQuery,"SELECT %s FROM %s WHERE %s LIKE '%s%%' ORDER BY %s",
					cVarList,cTable,cSearchField,cSearch,cSearchField);
}//void ExtSelectSearchPublic(...)


void ExtSelectSearch(const char *cTable,const char *cVarList,const char *cSearchField,const char *cSearch)
{
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM %s WHERE %s LIKE '%s%%' ORDER BY %s",
					cVarList,cTable,cSearchField,cSearch,cSearchField);
	else 
		sprintf(gcQuery,"SELECT %1$s FROM %3$s," TCLIENT
				" WHERE %3$s.uOwner=tClient.uClient"
				" AND (tClient.uClient=%2$u OR tClient.uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))"
				"AND %3$s.%4$s LIKE '%5$s%%' ORDER BY %4$s",
					cVarList,guCompany,cTable,cSearchField,cSearch);
}//void ExtSelectSearch(...)


void ExtSelect2(const char *cTable,const char *cVarList,unsigned uMaxResults)
{
	if(guPermLevel>11 && guLoginClient==1)//Root can read access all
		sprintf(gcQuery,"SELECT %1$s FROM %2$s ORDER BY %2$s._rowid",
					cVarList,cTable);
	else 
		sprintf(gcQuery,"SELECT %1$s FROM %3$s," TCLIENT
				" WHERE %3$s.uOwner=" TCLIENT ".uClient"
				" AND (" TCLIENT ".uClient=%2$u OR " TCLIENT ".uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))"
				" ORDER BY %3$s._rowid",
					cVarList,guCompany,
					cTable);
	if(uMaxResults)
	{
		char cLimit[33]={""};
		sprintf(cLimit," LIMIT %u",uMaxResults);
		strcat(gcQuery,cLimit);
	}

}//void ExtSelect2(...)


void ExtSelectRow(const char *cTable,const char *cVarList,unsigned uRow)
{
	if(guLoginClient==1 && guPermLevel>11)//Root users can read access all
		//sprintf(gcQuery,"SELECT %s FROM %s WHERE u%s=%u ORDER BY u%s",
		sprintf(gcQuery,"SELECT %s FROM %s WHERE u%s=%u",
					//cVarList,cTable,cTable+1,uRow,cTable+1);
					cVarList,cTable,cTable+1,uRow);
	else 
		sprintf(gcQuery,"SELECT %1$s FROM %3$s," TCLIENT
				" WHERE %3$s.uOwner=tClient.uClient"
                                " AND %3$s.u%4$s=%5$u"
				" AND (tClient.uClient=%2$u OR tClient.uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))",
					cVarList,guCompany,
					cTable,cTable+1,uRow);
}//void ExtSelectRow(...)


void ExtSelectRowPublic(const char *cTable,const char *cVarList,unsigned uRow)
{
		sprintf(gcQuery,"SELECT %s FROM %s WHERE u%s=%u",cVarList,cTable,cTable+1,uRow);
}//void ExtSelectRowPublic(...)


//Passwd stuff
static unsigned char itoa64[] =         /* 0 ... 63 => ascii - 64 */
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void to64(s, v, n)
  register char *s;
  register long v;
  register int n;
{
    while (--n >= 0) {
        *s++ = itoa64[v&0x3f];
        v >>= 6;
    }
}//void to64(s, v, n)


void EncryptPasswd(char *pw)
{
	//Notes:
	//	We should change time based salt 
	//	(could be used for faster dictionary attack)
	//	to /dev/random based system.

        char salt[3];
        char passwd[102]={""};
        char *cpw;
	char cMethod[16] ={""}; 

	GetConfiguration("cCryptMethod",cMethod,0,0,0,0);
	if(!strcmp(cMethod,"MD5"))
	{
		char cSalt[] = "$1$01234567$";
	    	(void)srand((int)time((time_t *)NULL));
    		to64(&cSalt[3],rand(),8);
		cpw = crypt(pw,cSalt);
		// error not verified, str NULL ("") returned	
	}
	else
	{
		// default DES method
	        sprintf(passwd,"%.99s",pw);
    		(void)srand((int)time((time_t *)NULL));
    		to64(&salt[0],rand(),2);
		cpw=crypt(passwd,salt);
	}	
	sprintf(pw,"%.99s",cpw);

}//void EncryptPasswd(char *pw)


void GeneratePasswd(char *dest)
{
	static int srand_called=0;
	size_t len=0;
	char *p=dest;
	int three_in_a_row=0;
	int arr[128]={0x0};

	if(!srand_called)
	{
		srandom(time(NULL));
		srand_called=1;
	}
	*dest=0x0; /* int the destination string*/
	for(len=6 + rand()%3; len; len--, p++) /* gen characters */
	{
		char *q=dest;
		*p=(rand()%2)? rand()%26 + 97: rand()%10 + 48;
		p[1]=0x0;
		arr[(int) *p]++;                         /* check values */
		if(arr[(int) *p]==3)
		{
			for(q=dest; q[2]>0 && !three_in_a_row; q++)	
				if(*q==q[1] && q[1]==q[2])
			   		three_in_a_row=1;
		}
		if(three_in_a_row || arr[(int) *p]> 3 )
			GeneratePasswd(dest);        /* values do not pass try again */
	}

}//void GeneratePasswd(char *dest)


void tContainerGroupPullDown(unsigned uGroup, unsigned uMode, char *cSelectName)
{
        register int i,n;
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

	char cHidden[100]={""};
	char *cMode="";

	if(!uMode)
		cMode="disabled";

	char *cName="ctContainerGroupPullDown";
	if(cSelectName[0])
		cName=cSelectName;
      
        sprintf(gcQuery,"SELECT uGroup,cLabel FROM tGroup WHERE uGroupType=1 ORDER BY cLabel");
	MYSQL_RUN_STORE_TEXT_RET_VOID(mysqlRes);
	i=mysql_num_rows(mysqlRes);
        if(i>0)
        {
                printf("<select name=%s %s>\n",cName,cMode);

                printf("<option title='No selection'>---</option>\n");
                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);
                        if(uGroup != field0)
                        {
                             printf("<option>%s</option>\n",mysqlField[1]);
                        }
                        else
                        {
                             printf("<option selected>%s</option>\n",mysqlField[1]);
			     if(!uMode)
				     sprintf(cHidden,"<input type=hidden name=%s value='%.99s'>\n",
							cName,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n",cName,cMode);
		if(!uMode)
			sprintf(cHidden,"<input type=hidden name=%s value='0'>\n",cName);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tContainerGroupPullDown()


void tTablePullDownActive(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode)
{
        register int i,n;
        char cLabel[256];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[100]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTablePullDownActive()");
                return;
        }

        //Extended functionality
        strncpy(cLocalTableName,cTableName,255);
        if((cp=strchr(cLocalTableName,';')))
        {
                strncpy(cSelectName,cp+1,99);
                cSelectName[99]=0;
                *cp=0;
        }


	if(uSelector && !uMode)
        	sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uStatus=1 AND _rowid=%u",
                                cFieldName,cLocalTableName,uSelector);
	else
        	sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uStatus=1 ORDER BY %s",
                                cFieldName,cLocalTableName,cOrderby);

	MYSQL_RUN_STORE_TEXT_RET_VOID(mysqlRes);
	
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
                printf("<select name=%s %s>\n",cLabel,cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                        {
                             printf("<option>%s</option>\n",mysqlField[1]);
                        }
                        else
                        {
                             printf("<option selected>%s</option>\n",mysqlField[1]);
			     if(!uMode)
			     sprintf(cHidden,"<input type=hidden name=%.99s value='%.99s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%.99s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDownActive()


//Per hardware node iptables FW control. 
//You must login to access hardware node http, ssh and other servers from your current
//IP.
void LoginFirewallJobs(unsigned uLoginClient)
{
        MYSQL_RES *res;
	MYSQL_ROW field;

	char cCreateLoginJobs[256]={""};
	GetConfiguration("cCreateLoginJobs",cCreateLoginJobs,0,0,0,0);
	if(strncmp(cCreateLoginJobs,"Via tNode::tProperty:cCreateLoginJobs",sizeof("Via tNode::tProperty:cCreateLoginJobs")))
		return;

	sprintf(gcQuery,"SELECT DISTINCT tNode.uNode,tNode.uDatacenter"
				" FROM tNode,tDatacenter,tProperty"
				" WHERE tNode.uDatacenter=tDatacenter.uDatacenter"
				" AND tProperty.uKey=tNode.uNode"
				" AND tProperty.cName='cCreateLoginJobs'"
				" AND tProperty.cValue='Yes'"
				" AND tDatacenter.uStatus=1"
				" AND tNode.uStatus=1"
				" AND tNode.cLabel!='appliance'");
	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		return;
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		unsigned uNode=0;
		unsigned uDatacenter=0;

		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);

		sprintf(gcQuery,"INSERT INTO tJob SET cLabel='LoginFirewallJobs()',cJobName='LoginFirewallJob'"
					",uDatacenter=%u,uNode=%u,uContainer=0"
					",uJobDate=UNIX_TIMESTAMP(NOW())"
					",uJobStatus=1"
					",cJobData='cIPv4=%.15s;\ntConfiguration:cCreateLoginJobs=Via tNode::tProperty:cCreateLoginJobs;'"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uDatacenter,
						uNode,
						gcHost,
						guCompany,guLoginClient);
		mysql_query(&gMysql,gcQuery);
	}
}//void LoginFirewallJobs(void)


void LogoutFirewallJobs(unsigned uLoginClient)
{
        MYSQL_RES *res;
	MYSQL_ROW field;

	char cCreateLoginJobs[256]={""};
	GetConfiguration("cCreateLoginJobs",cCreateLoginJobs,0,0,0,0);
	if(strncmp(cCreateLoginJobs,"Via tNode::tProperty:cCreateLoginJobs",sizeof("Via tNode::tProperty:cCreateLoginJobs")))
		return;

	sprintf(gcQuery,"SELECT DISTINCT tNode.uNode,tNode.uDatacenter"
				" FROM tNode,tDatacenter,tProperty"
				" WHERE tNode.uDatacenter=tDatacenter.uDatacenter"
				" AND tProperty.uKey=tNode.uNode"
				" AND tProperty.cName='cCreateLoginJobs'"
				" AND tProperty.cValue='Yes'"
				" AND tDatacenter.uStatus=1"
				" AND tNode.uStatus=1"
				" AND tNode.cLabel!='appliance'");
	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		return;
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		unsigned uNode=0;
		unsigned uDatacenter=0;

		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);

		sprintf(gcQuery,"INSERT INTO tJob SET cLabel='LogoutFirewallJobs()',cJobName='LogoutFirewallJob'"
					",uDatacenter=%u,uNode=%u,uContainer=0"
					",uJobDate=UNIX_TIMESTAMP(NOW())"
					",uJobStatus=1"
					",cJobData='cIPv4=%.15s;\ntConfiguration:cCreateLoginJobs=Via tNode::tProperty:cCreateLoginJobs;'"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uDatacenter,
						uNode,
						gcHost,
						guCompany,guLoginClient);
		mysql_query(&gMysql,gcQuery);
	}
}//void LogoutFirewallJobs(void)
