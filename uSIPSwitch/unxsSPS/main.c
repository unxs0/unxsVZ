/*
FILE 
	main.c
	$Id: main.c 2082 2012-09-06 01:44:03Z Dylan $
	unxsRAD created application file for unxsSPS.cgi
PURPOSE
	Main cgi interface and common functions used for all the other
	table tx.c files and their schema independent txfunc.h files -until
	you mess with them in non standard ways.
	
LEGAL
	(C) Gary Wallis 2001-2012. All Rights Reserved.
	LICENSE file should be included in distribution.
OTHER
	Only tested on CentOS 5
HELP
	support @ unixservice . com

*/

#include "mysqlrad.h"
#include <ctype.h>

#include "language.h"
#include "local.h"

//Global vars

#define SHOWPAGE 30
MYSQL gMysql;
MYSQL gMysql2;
unsigned long gluRowid;
unsigned guStart;
unsigned guEnd;
unsigned guI;
unsigned guN=SHOWPAGE;
char gcCommand[100];
static char cLogKey[16]={"Ksdj458jssdUjf79"};
char gcFilter[100];
char gcFind[100];
unsigned guMode;

int guPermLevel=0;
unsigned guLoginClient=0;
unsigned guReseller=0;
char gcUser[100]={""};
char gcHost[100]={""};
char gcHostname[100]={""};
unsigned guCompany=0;
char gcCompany[100]={""};

//SSLLoginCookie()
char gcCookie[1024]={""};
char gcLogin[100]={""};
char gcPasswd[100]={""};
unsigned guSSLCookieLogin=0;

unsigned guCookieProject=0,guCookieTable=0,guCookieField=0;
void SetSessionCookie(void);
void GetSessionCookie(void);

char gcFunction[100]={""};
unsigned guListMode=0;
char gcQuery[8192]={""};
char *gcQstr=gcQuery;
char *gcBuildInfo="$Id: main.c 2082 2012-09-06 01:44:03Z Dylan $";
char *gcRADStatus="Start";

//Local
void Footer_ism3(void);
void Header_ism3(char *cMsg, int iJs);
const char *cForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);
char *cEmailInput(char *cInput);
void GetClientOwner(unsigned uClient, unsigned *uOwner);
void htmlPlainTextError(const char *cError);

//Ext
int iExtMainCommands(pentry entries[], int x);
void ExtMainContent(void);
void ExtMainShell(int iArgc, char *cArgv[]);
void DashBoard(const char *cOptionalMsg);

//Only local
void ConnectDb(void);
void NoSuchFunction(void);
void unxsSPS(const char *cResult);
void InitialInstall(void);
const char *cUserLevel(unsigned uPermLevel);

int iValidLogin(int iMode);
void SSLCookieLogin(void);
void SetLogin(void);
void EncryptPasswdWithSalt(char *cPw, char *cSalt);
void GetPLAndClient(char *cUser);
void htmlSSLLogin(void);

//mainfunc.h for symbolic links to this program
void CalledByAlias(int iArgc,char *cArgv[]);

int main(int iArgc, char *cArgv[])
{
	pentry entries[64];
	entry gentries[16];
	char *gcl;
	register int x;
	int cl=0;

	gethostname(gcHostname,98);

	if(!strstr(cArgv[0],"unxsSPS.cgi"))
		CalledByAlias(iArgc,cArgv);

	if(getenv("REMOTE_HOST")!=NULL)
		sprintf(gcHost,"%.99s",getenv("REMOTE_HOST"));

	else if(getenv("REMOTE_ADDR")!=NULL)
	{
		ConnectDb();
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

	        for(x=0;gcl[0] != '\0' && x<8;x++)
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
				unxsSPS("");
			else if(!strcmp(gcFunction,"Logout"))
			{
				printf("Set-Cookie: unxsSPSLogin=; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
				printf("Set-Cookie: unxsSPSPasswd=; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
				printf("Set-Cookie: unxsSPSSessionCookie=; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
				sprintf(gcQuery,"INSERT INTO tLog SET cLabel='logout %.99s',uLogType=6,uPermLevel=%u,"
						"uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=1,"
						"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname);
				macro_mySQLQueryHTMLError;
				gcCookie[0]=0;
                                guPermLevel=0;
                                guLoginClient=0;
                                gcUser[0]=0;
                                gcCompany[0]=0;
                                guSSLCookieLogin=0;
                                htmlSSLLogin();
			}

			//funcMainGetMenu()
			if(!strcmp(gcFunction,"tPBX"))
				ExttPBXGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tDID"))
				ExttDIDGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tGateway"))
				ExttGatewayGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tRule"))
				ExttRuleGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tPrefix"))
				ExttPrefixGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tCarrier"))
				ExttCarrierGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tCDR"))
				ExttCDRGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tAddress"))
				ExttAddressGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tCluster"))
				ExttClusterGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tGatewayType"))
				ExttGatewayTypeGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tTimeInterval"))
				ExttTimeIntervalGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tGroup"))
				ExttGroupGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tGroupGlue"))
				ExttGroupGlueGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tGroupType"))
				ExttGroupTypeGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tClient"))
				ExttClientGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tAuthorize"))
				ExttAuthorizeGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tServer"))
				ExttServerGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tStatus"))
				ExttStatusGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tConfiguration"))
				ExttConfigurationGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tJob"))
				ExttJobGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tJobStatus"))
				ExttJobStatusGetHook(gentries,x);
			else if(!strcmp(gcFunction,"tGlossary"))
				ExttGlossaryGetHook(gentries,x);
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


		}

		unxsSPS(ISPNAME);

	}//end get method interface section

	//Post method interface
	cl = atoi(getenv("CONTENT_LENGTH"));
	for(x=0;cl && (!feof(stdin)) && x<64 ;x++)
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
	}

	//SSLCookieLogin()
        if(!strcmp(gcFunction,"Login")) SetLogin();

        if(!guPermLevel || !gcUser[0] || !guLoginClient)
                SSLCookieLogin();

	//Main Post Menu
	//funcMainPostFunctions()
	tPBXCommands(entries,x);
	tDIDCommands(entries,x);
	tGatewayCommands(entries,x);
	tRuleCommands(entries,x);
	tPrefixCommands(entries,x);
	tCarrierCommands(entries,x);
	tCDRCommands(entries,x);
	tAddressCommands(entries,x);
	tClusterCommands(entries,x);
	tGatewayTypeCommands(entries,x);
	tTimeIntervalCommands(entries,x);
	tGroupCommands(entries,x);
	tGroupGlueCommands(entries,x);
	tGroupTypeCommands(entries,x);
	tClientCommands(entries,x);
	tAuthorizeCommands(entries,x);
	tServerCommands(entries,x);
	tStatusCommands(entries,x);
	tConfigurationCommands(entries,x);
	tJobCommands(entries,x);
	tJobStatusCommands(entries,x);
	tGlossaryCommands(entries,x);
	tTemplateCommands(entries,x);
	tTemplateSetCommands(entries,x);
	tTemplateTypeCommands(entries,x);
	tLogCommands(entries,x);
	tLogTypeCommands(entries,x);
	tLogMonthCommands(entries,x);
	tMonthCommands(entries,x);


	iExtMainCommands(entries,x);

	NoSuchFunction();

	return(0);

}//end of main()

#include "mainfunc.h"

void unxsSPS(const char *cResult)
{
	Header_ism3("Main",0);

        if(cResult[0])
		DashBoard(cResult);
        else
                ExtMainContent();

	Footer_ism3();

}//void unxsSPS(const char *cResult)


void StyleSheet(void)
{
	printf("<style type=\"text/css\">\n");
	printf( "input.first {\n"
		"background-image: url(/images/buttons/first.png);\n "
		"background-color: transparent;\n"
		"background-repeat: no-repeat;\n"
		"border: none;\n"
		"cursor: pointer;        /* make the cursor like hovering over an <a> element */\n"
		"height: 32px;\n"
		"width: 32px;\n"
		"color: transparent;\n"
		"}\n"
		"input.back {\n"
		"background-image: url(/images/buttons/back.png);\n "
		"background-color: transparent;\n"
		"background-repeat: no-repeat;\n"
		"border: none;\n"
		"cursor: pointer;\n"
		"height: 32px;\n"
		"width: 32px;\n"
		"color: transparent;\n"
		"}\n"
		"input.next {\n"
		"background-image: url(/images/buttons/next.png);\n "
		"background-color: transparent;\n"
		"background-repeat: no-repeat;\n"
		"border: none;\n"
		"cursor: pointer;\n"
		"height: 32px;\n"
		"width: 32px;\n"
		"color: transparent;\n"
		"}\n"
		"input.last {\n"
		"background-image: url(/images/buttons/last.png);\n "
		"background-color: transparent;\n"
		"background-repeat: no-repeat;\n"
		"border: none;\n"
		"cursor: pointer;\n"
		"height: 32px;\n"
		"width: 32px;\n"
		"color: transparent;\n"
		"}\n"
		"input.new {\n"
		"background-image: url(/images/buttons/new.png);\n "
		"background-color: transparent;\n"
		"background-repeat: no-repeat;\n"
		"border: none;\n"
		"cursor: pointer;\n"
		"height: 32px;\n"
		"width: 32px;\n"
		"color: transparent;\n"
		"}\n"
		"input.modify {\n"
		"background-image: url(/images/buttons/edit.png);\n "
		"background-color: transparent;\n"
		"background-repeat: no-repeat;\n"
		"border: none;\n"
		"cursor: pointer;\n"
		"height: 32px;\n"
		"width: 32px;\n"
		"color: transparent;\n"
		"}\n"
		"input.delete {\n"
		"background-image: url(/images/buttons/delete.png);\n "
		"background-color: transparent;\n"
		"background-repeat: no-repeat;\n"
		"border: none;\n"
		"cursor: pointer;\n"
		"height: 32px;\n"
		"width: 32px;\n"
		"color: transparent;\n"
		"}\n"
		"input.list {\n"
		"background-image: url(/images/buttons/list.png);\n "
		"background-color: transparent;\n"
		"background-repeat: no-repeat;\n"
		"border: none;\n"
		"cursor: pointer;\n"
		"height: 32px;\n"
		"width: 32px;\n"
		"color: transparent;\n"
		"}\n"
		"input.refresh {\n"
		"background-image: url(/images/buttons/refresh.png);\n "
		"background-color: transparent;\n"
		"background-repeat: no-repeat;\n"
		"border: none;\n"
		"cursor: pointer;\n"
		"height: 32px;\n"
		"width: 32px;\n"
		"color: transparent;\n"
		"}\n"
	);

	printf("input.smallButton {width:50px;}\n");
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
	//Behind tabs background e.g. to the right of menu tabs
	//printf("\tbackground: #e5e5e5 url('/images/hairline.gif') repeat-x bottom center;\n");
	printf("\tbackground: #EAF3C1 url('/images/hairline.gif') repeat-x bottom center;\n");
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
	printf("\tbackground: url('/images/left_grn.gif') no-repeat left top;\n");
	printf("\tmargin: 0;\n");
	printf("\tpadding: 0 0 0 9px;\n");
	printf("}\n");

	printf("#menutab a, #menutab a.last\n");
	printf("{\n");
	printf("\tfloat:left;\n");
	printf("\tdisplay: block;\n");
	printf("\tbackground: url('/images/right_grn.gif') no-repeat right top;\n");
	printf("\tpadding: 5px 15px 2px 6px;\n");
	printf("\tcolor: #222;\n");//tab text color
	printf("\ttext-decoration: none;\n");
	printf("}\n");

	printf("/* Commented Backslash Hack\n");
	printf("\thides rule from IE5-Mac \\*/\n");
	printf("\t#menutab a, #menutab a.last {float:none;}\n");
	printf("/* End IE5-Mac hack */\n");

	printf("#menutab a.last\n");
	printf("{\n");
	printf("\tbackground: url('/images/right_last_grn.gif') no-repeat right top;\n");
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

	printf("</style>\n");

}//void StyleSheet(void)


void jsCalendarHeader(void)
{
        printf("<link rel='stylesheet' type='text/css' media='all' href='/js/calendar-blue.css'/>\n");
        printf("<script type='text/javascript' src='/js/calendar.js'></script>\n");
        printf("<script type='text/javascript' src='/js/calendar-en.js'></script>\n");
        printf("<script type='text/javascript' src='/js/calendar-setup.js'></script>\n");

}//void jsCalendarHeader(void)


void jsCalendarInput(char *cInputName,char *cValue,unsigned uMode)
{
        char cMode[16]={""};
        if(!uMode)
                sprintf(cMode,"disabled");

        printf("<input id='%s' class='field_input' type='text' name='%s' value='%s' size=40 style='display: ; \
                vertical-align: middle; ' %s >\n",cInputName,cInputName,cValue,cMode);

        if(uMode)
        {
                printf("<img date_trigger='1' class='record_button' date_field='%s' id='date_trigger_%s_501' src='/images/calendar.gif' \
                        onmouseout='swapClass(event); this.src='/images/calendar.gif' ' onmouseover='swapClass(event); this.src='/images/calendar_mo.gif' \
                        onmousedown='this.style.top = '1px'; this.style.left = '1px'' onmouseup='this.style.top = '0px'; this.style.left = '0px'' \
                        style='position: relative; vertical-align: middle; display: ; ' title='Date selector'/>\n",cInputName,cInputName);
                printf("<script type='text/javascript'>\n \
                        Calendar.setup({\n \
                        inputField     :    '%s',\n \
                        ifFormat : '%%Y-%%m-%%d',\n \
                        button         :    'date_trigger_%s_501',\n \
                        align          :    'bR',\n \
                        singleClick    :    true,\n \
                        weekNumbers    :    false,\n \
                        step           :    1,\n \
                        timeFormat : 12\n \
                        });</script>\n",cInputName,cInputName);
        }
        else
                printf("<input type=hidden name='%s' value='%s'>\n",cInputName,cValue);

}//void jsCalendarInput(char *cInputName,char *cValue,unsigned uMode)


void Header_ism3(char *title, int js)
{
	printf("Content-type: text/html\n\n");
	printf("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
        printf("<html><head><title>vcSPC %s </title>",title);
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n");
	StyleSheet();
        if(js)
                jsCalendarHeader();

	printf("<link rel=\"shortcut icon\" type=image/x-icon href=/images/rad.ico>"
		"</head><body><form action=unxsSPS.cgi method=post><blockquote>\n");

	//Open header table
	printf("<table width=100%% cellpadding=0 cellspacing=0 ><tr><td valign=bottom>\n");

	//funcMainNavBars()
	if(!strcmp(gcFunction,"tPBX") || !strcmp(gcFunction,"tPBXTools") ||
			!strcmp(gcFunction,"tPBXList"))
		ExttPBXNavBar();
	else if(!strcmp(gcFunction,"tDID") || !strcmp(gcFunction,"tDIDTools") ||
			!strcmp(gcFunction,"tDIDList"))
		ExttDIDNavBar();
	else if(!strcmp(gcFunction,"tGateway") || !strcmp(gcFunction,"tGatewayTools") ||
			!strcmp(gcFunction,"tGatewayList"))
		ExttGatewayNavBar();
	else if(!strcmp(gcFunction,"tRule") || !strcmp(gcFunction,"tRuleTools") ||
			!strcmp(gcFunction,"tRuleList"))
		ExttRuleNavBar();
	else if(!strcmp(gcFunction,"tPrefix") || !strcmp(gcFunction,"tPrefixTools") ||
			!strcmp(gcFunction,"tPrefixList"))
		ExttPrefixNavBar();
	else if(!strcmp(gcFunction,"tCarrier") || !strcmp(gcFunction,"tCarrierTools") ||
			!strcmp(gcFunction,"tCarrierList"))
		ExttCarrierNavBar();
	else if(!strcmp(gcFunction,"tCDR") || !strcmp(gcFunction,"tCDRTools") ||
			!strcmp(gcFunction,"tCDRList"))
		ExttCDRNavBar();
	else if(!strcmp(gcFunction,"tAddress") || !strcmp(gcFunction,"tAddressTools") ||
			!strcmp(gcFunction,"tAddressList"))
		ExttAddressNavBar();
	else if(!strcmp(gcFunction,"tCluster") || !strcmp(gcFunction,"tClusterTools") ||
			!strcmp(gcFunction,"tClusterList"))
		ExttClusterNavBar();
	else if(!strcmp(gcFunction,"tGatewayType") || !strcmp(gcFunction,"tGatewayTypeTools") ||
			!strcmp(gcFunction,"tGatewayTypeList"))
		ExttGatewayTypeNavBar();
	else if(!strcmp(gcFunction,"tTimeInterval") || !strcmp(gcFunction,"tTimeIntervalTools") ||
			!strcmp(gcFunction,"tTimeIntervalList"))
		ExttTimeIntervalNavBar();
	else if(!strcmp(gcFunction,"tGroup") || !strcmp(gcFunction,"tGroupTools") ||
			!strcmp(gcFunction,"tGroupList"))
		ExttGroupNavBar();
	else if(!strcmp(gcFunction,"tGroupGlue") || !strcmp(gcFunction,"tGroupGlueTools") ||
			!strcmp(gcFunction,"tGroupGlueList"))
		ExttGroupGlueNavBar();
	else if(!strcmp(gcFunction,"tGroupType") || !strcmp(gcFunction,"tGroupTypeTools") ||
			!strcmp(gcFunction,"tGroupTypeList"))
		ExttGroupTypeNavBar();
	else if(!strcmp(gcFunction,"tClient") || !strcmp(gcFunction,"tClientTools") ||
			!strcmp(gcFunction,"tClientList"))
		ExttClientNavBar();
	else if(!strcmp(gcFunction,"tAuthorize") || !strcmp(gcFunction,"tAuthorizeTools") ||
			!strcmp(gcFunction,"tAuthorizeList"))
		ExttAuthorizeNavBar();
	else if(!strcmp(gcFunction,"tServer") || !strcmp(gcFunction,"tServerTools") ||
			!strcmp(gcFunction,"tServerList"))
		ExttServerNavBar();
	else if(!strcmp(gcFunction,"tStatus") || !strcmp(gcFunction,"tStatusTools") ||
			!strcmp(gcFunction,"tStatusList"))
		ExttStatusNavBar();
	else if(!strcmp(gcFunction,"tConfiguration") || !strcmp(gcFunction,"tConfigurationTools") ||
			!strcmp(gcFunction,"tConfigurationList"))
		ExttConfigurationNavBar();
	else if(!strcmp(gcFunction,"tJob") || !strcmp(gcFunction,"tJobTools") ||
			!strcmp(gcFunction,"tJobList"))
		ExttJobNavBar();
	else if(!strcmp(gcFunction,"tJobStatus") || !strcmp(gcFunction,"tJobStatusTools") ||
			!strcmp(gcFunction,"tJobStatusList"))
		ExttJobStatusNavBar();
	else if(!strcmp(gcFunction,"tGlossary") || !strcmp(gcFunction,"tGlossaryTools") ||
			!strcmp(gcFunction,"tGlossaryList"))
		ExttGlossaryNavBar();
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


	//Login info
	printf("<font size=3><b>VoiceCarrier SIPProxy Controller</b></font> \n ");
	printf("&nbsp;&nbsp;&nbsp;<font color=red>%s logged in from %s [%s]</font>",
							gcUser,gcHost,cUserLevel(guPermLevel));

	//Logout link
	if(guSSLCookieLogin)
		printf(" <a title='Erase login cookies' href=unxsSPS.cgi?gcFunction=Logout>Logout</a> ");

	//close first col
	printf("</td><td align=right valign=bottom>\n");

	//Workflow cookie display
	//funcConfiguration(funcConfiguration:MainCookieDisplay)
        printf("%s %s %s",
                ForeignKey("tPBX","cHostname",guCookieField),
                ForeignKey("tServer","cLabel",guCookieTable),
                ForeignKey("tDatacenter","cLabel",guCookieProject));


	//Close header table
	printf("</td></tr></table>\n");

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
		printf("\t\t\t<a title='Home start page' href=unxsSPS.cgi?gcFunction=Main>Main</a>\n");
	printf("\t\t\t</li>\n");

	//funcMainTabMenu()
	//tPBX
	if(guPermLevel>=7)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tPBX") && strcmp(gcFunction,"tPBXTools") &&
			strcmp(gcFunction,"tPBXList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='PBX Gateways' href=unxsSPS.cgi?gcFunction=tPBX>tPBX</a>\n");
	}
	//tDID
	if(guPermLevel>=7)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tDID") && strcmp(gcFunction,"tDIDTools") &&
			strcmp(gcFunction,"tDIDList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Direct inward dialing numbers' href=unxsSPS.cgi?gcFunction=tDID>tDID</a>\n");
	}
	//tGateway
	if(guPermLevel>=7)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tGateway") && strcmp(gcFunction,"tGatewayTools") &&
			strcmp(gcFunction,"tGatewayList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Outgoing termination provider gateways' href=unxsSPS.cgi?gcFunction=tGateway>tGateway</a>\n");
	}
	//tRule
	if(guPermLevel>=7)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tRule") && strcmp(gcFunction,"tRuleTools") &&
			strcmp(gcFunction,"tRuleList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Rules for routing traffic to termination gateways' href=unxsSPS.cgi?gcFunction=tRule>tRule</a>\n");
	}
	//tPrefix
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tPrefix") && strcmp(gcFunction,"tPrefixTools") &&
			strcmp(gcFunction,"tPrefixList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='DID prefixes for cost and other classification' href=unxsSPS.cgi?gcFunction=tPrefix>tPrefix</a>\n");
	}
	//tCarrier
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tCarrier") && strcmp(gcFunction,"tCarrierTools") &&
			strcmp(gcFunction,"tCarrierList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Carrier information' href=unxsSPS.cgi?gcFunction=tCarrier>tCarrier</a>\n");
	}
	//tCDR
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tCDR") && strcmp(gcFunction,"tCDRTools") &&
			strcmp(gcFunction,"tCDRList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Call detail records' href=unxsSPS.cgi?gcFunction=tCDR>tCDR</a>\n");
	}
	//tAddress
	if(guPermLevel>=7)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tAddress") && strcmp(gcFunction,"tAddressTools") &&
			strcmp(gcFunction,"tAddressList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='IP addresses' href=unxsSPS.cgi?gcFunction=tAddress>tAddress</a>\n");
	}
	//tCluster
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tCluster") && strcmp(gcFunction,"tClusterTools") &&
			strcmp(gcFunction,"tClusterList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Datacenter or other logical grouping of SIP servers' href=unxsSPS.cgi?gcFunction=tCluster>tCluster</a>\n");
	}
	//tGatewayType
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tGatewayType") && strcmp(gcFunction,"tGatewayTypeTools") &&
			strcmp(gcFunction,"tGatewayTypeList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Type of gateway' href=unxsSPS.cgi?gcFunction=tGatewayType>tGatewayType</a>\n");
	}
	//tTimeInterval
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tTimeInterval") && strcmp(gcFunction,"tTimeIntervalTools") &&
			strcmp(gcFunction,"tTimeIntervalList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Define time intervals with recurrence' href=unxsSPS.cgi?gcFunction=tTimeInterval>tTimeInterval</a>\n");
	}
	//tGroup
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tGroup") && strcmp(gcFunction,"tGroupTools") &&
			strcmp(gcFunction,"tGroupList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Group names' href=unxsSPS.cgi?gcFunction=tGroup>tGroup</a>\n");
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
	  printf("\t\t\t<a title='Group glue table' href=unxsSPS.cgi?gcFunction=tGroupGlue>tGroupGlue</a>\n");
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
	  printf("\t\t\t<a title='Group types' href=unxsSPS.cgi?gcFunction=tGroupType>tGroupType</a>\n");
	}
	//tClient
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tClient") && strcmp(gcFunction,"tClientTools") &&
			strcmp(gcFunction,"tClientList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Organizations and their contacts' href=unxsSPS.cgi?gcFunction=tClient>tClient</a>\n");
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
	  printf("\t\t\t<a title='Contact credentials' href=unxsSPS.cgi?gcFunction=tAuthorize>tAuthorize</a>\n");
	}
	//tServer
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tServer") && strcmp(gcFunction,"tServerTools") &&
			strcmp(gcFunction,"tServerList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Servers' href=unxsSPS.cgi?gcFunction=tServer>tServer</a>\n");
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
	  printf("\t\t\t<a title='General status labels' href=unxsSPS.cgi?gcFunction=tStatus>tStatus</a>\n");
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
	  printf("\t\t\t<a title='Program settings' href=unxsSPS.cgi?gcFunction=tConfiguration>tConfiguration</a>\n");
	}
	//tJob
	if(guPermLevel>=20)
	{
	  printf("\t\t\t<li");
	  if(strcmp(gcFunction,"tJob") && strcmp(gcFunction,"tJobTools") &&
			strcmp(gcFunction,"tJobList"))
		  printf(">\n");
	  else
		  printf(" id=current>\n");
	  printf("\t\t\t<a title='Job queue' href=unxsSPS.cgi?gcFunction=tJob>tJob</a>\n");
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
	  printf("\t\t\t<a title='Job queue status' href=unxsSPS.cgi?gcFunction=tJobStatus>tJobStatus</a>\n");
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
	  printf("\t\t\t<a title='Glossary and help' href=unxsSPS.cgi?gcFunction=tGlossary>tGlossary</a>\n");
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
	  printf("\t\t\t<a title='Templates' href=unxsSPS.cgi?gcFunction=tTemplate>tTemplate</a>\n");
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
	  printf("\t\t\t<a title='Template sets' href=unxsSPS.cgi?gcFunction=tTemplateSet>tTemplateSet</a>\n");
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
	  printf("\t\t\t<a title='Template types' href=unxsSPS.cgi?gcFunction=tTemplateType>tTemplateType</a>\n");
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
	  printf("\t\t\t<a title='Log' href=unxsSPS.cgi?gcFunction=tLog>tLog</a>\n");
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
	  printf("\t\t\t<a title='Log type' href=unxsSPS.cgi?gcFunction=tLogType>tLogType</a>\n");
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
	  printf("\t\t\t<a title='Archive of logs' href=unxsSPS.cgi?gcFunction=tLogMonth>tLogMonth</a>\n");
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
	  printf("\t\t\t<a title='Month of year' href=unxsSPS.cgi?gcFunction=tMonth>tMonth</a>\n");
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

	exit(0);

}//Footer_ism3(void)


void NoSuchFunction(void)
{
	 
	sprintf(gcQuery,"[%s] Not Recognized",gcFunction);
	unxsSPS(gcQuery);
}


void ProcessControlVars(pentry entries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"gcFilter"))
			strcpy(gcFilter,entries[i].val);
		else if(!strcmp(entries[i].name,"gcCommand"))
			strcpy(gcCommand,entries[i].val);
		else if(!strcmp(entries[i].name,"gcFind"))
			strcpy(gcFind,entries[i].val);
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
	printf("<tr><td width=20%% valign=top><label for=%.32s><font color=%.32s>%.32s</font></label></td><td>",cFieldLabel,cColor,cFieldLabel);

}//void OpenRow()



void tTablePullDownOwner(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode)
{
        register int i,n;
        char cLabel[128];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[34]={""};
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
                strncpy(cSelectName,cp+1,32);
                cSelectName[32]=0;
                *cp=0;
        }


	if(guLoginClient==1)
        	sprintf(gcQuery,"SELECT _rowid,%s FROM %s ORDER BY %s",
                                cFieldName,cLocalTableName,cOrderby);
	else
        	sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uOwner=%u OR uOwner IN"
				" (SELECT uClient FROM " TCLIENT " WHERE uOwner=%u) ORDER BY %s",
				cFieldName,cLocalTableName,guCompany,guCompany,cOrderby);

	macro_mySQLRunAndStoreTextVoidRet(mysqlRes);
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
			     sprintf(cHidden,"<input type=hidden name=%.32s value='%.32s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%.32s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDownOwner()


void tTablePullDown(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode)
{
        register int i,n;
        char cLabel[128];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[34]={""};
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
                strncpy(cSelectName,cp+1,32);
                cSelectName[32]=0;
                *cp=0;
        }


        sprintf(gcQuery,"SELECT _rowid,%s FROM %s ORDER BY %s",
                                cFieldName,cLocalTableName,cOrderby);

	macro_mySQLRunAndStoreTextVoidRet(mysqlRes);
	
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
			     sprintf(cHidden,"<input type=hidden name=%.32s value='%.32s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%.32s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDown()


int ReadPullDownOwner(const char *cTableName,const char *cFieldName,
				const char *cLabel,const unsigned uOwner)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

        unsigned int iRowid=0;//Not found

        sprintf(gcQuery,"SELECT _rowid FROM %s WHERE %s='%s' AND (uOwner=%u OR uOwner=%u)",
                        cTableName,cFieldName,TextAreaSave((char *) cLabel),uOwner,guCompany);
        macro_mySQLRunAndStore(mysqlRes);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sscanf(mysqlField[0],"%u",&iRowid);
        mysql_free_result(mysqlRes);
        return(iRowid);

}//ReadPullDownOwner()


int ReadPullDown(const char *cTableName,const char *cFieldName,const char *cLabel)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

        unsigned int iRowid=0;//Not found

        sprintf(gcQuery,"select _rowid from %s where %s='%s'",
                        cTableName,cFieldName,TextAreaSave((char *) cLabel));
        macro_mySQLRunAndStore(mysqlRes);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sscanf(mysqlField[0],"%u",&iRowid);
        mysql_free_result(mysqlRes);
        return(iRowid);

}//ReadPullDown(char *cTableName,char *cLabel)


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

	if(!cCopy) unxsSPS("TextAreaInput() malloc error");

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

	if(!cCopy) unxsSPS("TransformAngleBrackets() malloc error");

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

	if(!cCopy) unxsSPS("EncodeDoubleQuotes() malloc error");

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


const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

	static char gcQuery[256];

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

	if(!uKey)
	{
        	return("---");
	}
	else
	{
		sprintf(gcQuery,"%u",uKey);
        	return(gcQuery);
	}

}//const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey)


void InitialInstall(void)
{
	unxsSPS("Please run\
		unxsSPS.cgi Initialize &lt;mysqlpwd&gt; from the command line");

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
		printf("Set-Cookie: unxsSPSLogin=%s;\n",gcLogin);
		printf("Set-Cookie: unxsSPSPasswd=%s;\n",gcPasswd);
		strncpy(gcUser,gcLogin,41);
		GetPLAndClient(gcUser);
		guSSLCookieLogin=1;
		unxsSPS("DashBoard");
	}
	else
	{
		guSSLCookieLogin=0;
		SSLCookieLogin();
	}
				
}//void SetLogin(void)


char *cGetPasswd(char *gcLogin);
int iValidLogin(int mode)
{
	char salt[3]={'/','D',0};
	char cPassword[100]={""};

	//Notes:
	//Mode=1 means we have encrypted passwd from cookie

	strcpy(cPassword,cGetPasswd(gcLogin));
	if(cPassword[0])
	{
		if(!mode)
		{
			strncpy(salt,cPassword,2);
			EncryptPasswdWithSalt(gcPasswd,salt);
			if(!strcmp(gcPasswd,cPassword)) 
			{
				//tLogType.cLabel='backend login'->uLogType=6
				sprintf(gcQuery,"INSERT INTO tLog SET cLabel='login ok %.99s',uLogType=6,uPermLevel=%u,uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname);
				macro_mySQLQueryHTMLError;
				return 1;
			}
		}
		else
		{
			if(!strcmp(gcPasswd,cPassword)) return 1;
		}
	}
	if(!mode)
	{
		sprintf(gcQuery,"INSERT INTO tLog SET cLabel='login failed %.99s',uLogType=6,uPermLevel=%u,uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname);
		macro_mySQLQueryHTMLError;
	}
	return 0;

}//iValidLogin()


char *cGetPasswd(char *gcLogin)
{
	static char cPasswd[100]={""};
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	char *cp;

	//SQL injection code
	if((cp=strchr(gcLogin,'\''))) *cp=0;

	sprintf(gcQuery,"SELECT cPasswd FROM tAuthorize WHERE cLabel='%s'",
			gcLogin);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		char cError[100];

                sprintf(cError,"Table '%s.tAuthorize' doesn't exist",DBNAME);
		if(!strcmp(mysql_error(&gMysql),cError))
			InitialInstall();
                else
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysqlRes=mysql_store_result(&gMysql);
	cPasswd[0]=0;
	if((mysqlField=mysql_fetch_row(mysqlRes)))
		strcpy(cPasswd,mysqlField[0]);
	mysql_free_result(mysqlRes);

	
	return(cPasswd);

}//char *cGetPasswd(char *gcLogin)


void SSLCookieLogin(void)
{

	GetSessionCookie();

	char *ptr,*ptr2;

	//Parse out login and passwd from cookies
#ifdef SSLONLY
	if(getenv("HTTPS")==NULL) 
		unxsSPS("Non SSL access denied");
#endif

	if(getenv("HTTP_COOKIE")!=NULL)
		strncpy(gcCookie,getenv("HTTP_COOKIE"),1022);
	
	if(gcCookie[0])
	{

	if((ptr=strstr(gcCookie,"unxsSPSLogin=")))
	{
		ptr+=strlen("unxsSPSLogin=");
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
	if((ptr=strstr(gcCookie,"unxsSPSPasswd=")))
	{
		ptr+=strlen("unxsSPSPasswd=");
		if((ptr2=strchr(ptr,';')))
		{
			*ptr2=0;
			strncpy(gcPasswd,ptr,20);
			*ptr2=';';
		}
		else
		{
			strncpy(gcPasswd,ptr,20);
		}
	}
	
	}//if gcCookie[0] time saver

	if(!iValidLogin(1))
		htmlSSLLogin();

	strncpy(gcUser,gcLogin,41);
	GetPLAndClient(gcUser);
	if(!guPermLevel || !guLoginClient)
		unxsSPS("Access denied");
	gcPasswd[0]=0;
	guSSLCookieLogin=1;

}//SSLCookieLogin()


void htmlSSLLogin(void)
{
        Header_ism3("",0);

	printf("<p>\n");
	printf("Login: <input type=text size=20 maxlength=98 name=gcLogin>\n");
	printf(" Passwd: <input type=password size=20 maxlength=20 name=gcPasswd>\n");
	printf("<font size=1> <input type=submit name=gcFunction value=Login>\n");

        Footer_ism3();

}//void htmlSSLLogin(void)


void EncryptPasswdWithSalt(char *pw, char *salt)
{
	char passwd[102]={""};
	char *cpw;
			
	strcpy(passwd,pw);
				
	cpw=crypt(passwd,salt);

	strcpy(pw,cpw);

}//void EncryptPasswdWithSalt(char *pw, char *salt)



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
	macro_mySQLRunAndStore(mysqlRes);
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
	
        sprintf(cQuery,"SELECT uOwner FROM tClient WHERE uClient=%u",uClient);
        mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	mysqlRes=mysql_store_result(&gMysql);

	*uOwner=0;
        if((mysqlField=mysql_fetch_row(mysqlRes)))
                sscanf(mysqlField[0],"%u",uOwner);
        mysql_free_result(mysqlRes);

}//void GetClientOwner(unsigned uClient, unsigned *uOwner)


void unxsSPSLog(unsigned uTablePK, char *cTableName, char *cLogEntry)
{
	char cQuery[512]={""};

	//uLogType==1 is this back-office cgi by default tLogType install
        sprintf(cQuery,"INSERT INTO tLog SET cLabel='%.63s',uLogType=1,uPermLevel=%u,uLoginClient=%u,cLogin='%.99s',cHost='%.99s',uTablePK=%u,cTableName='%.31s',cHash=MD5(CONCAT('%s','%u','%u','%s','%s','%u','%s','%s',UNIX_TIMESTAMP(NOW()))),uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
		cLogEntry,
		guPermLevel,
		guLoginClient,
		gcLogin,
		gcHost,
		uTablePK,
		cTableName,
		cLogEntry,
		guPermLevel,
		guLoginClient,
		gcLogin,
		gcHost,
		uTablePK,
		cTableName,
		cLogKey);

	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

}//void unxsSPSLog(unsigned uTablePK, char *cTableName, char *cLogEntry)


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
	printf("Please report this unxsSPS fatal error ASAP:\n%s\n",cError);

	//Attempt to report error in tLog
        sprintf(gcQuery,"INSERT INTO tLog SET cLabel='htmlPlainTextError',uLogType=4,uPermLevel=%u,uLoginClient=%u,cLogin='%s',cHost='%s',cMessage=\"%s (%.24s)\",cServer='%s',cHash=MD5(CONCAT('%u','%u','%s','%s',\"%s (%.24s)\",'%s',UNIX_TIMESTAMP(NOW()),'%s')),uOwner=1,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uTablePK=%u,cTableName='errno'",
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


unsigned uAllowDel(const unsigned uOwner, const unsigned uCreatedBy)
{
	//Set guReseller for tests to see if the owner of the owner is guCompany.
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	if( (guPermLevel>11 && uOwner==guCompany) //r3
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


void ExtSelect(const char *cTable,const char *cVarList)
{
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM %s ORDER BY u%s",
					cVarList,cTable,cTable+1);
	else 
		sprintf(gcQuery,"SELECT %1$s FROM %3$s," TCLIENT
				" WHERE %3$s.uOwner=tClient.uClient"
				" AND (tClient.uClient=%2$u OR tClient.uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))"
				" ORDER BY u%4$s",
					cVarList,guCompany,
					cTable,cTable+1);
}//void ExtSelect(...)


void ExtSelectRow(const char *cTable,const char *cVarList,unsigned uRow)
{
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM %s WHERE u%s=%u ORDER BY u%s",
					cVarList,cTable,cTable+1,uRow,cTable+1);
	else 
		sprintf(gcQuery,"SELECT %1$s FROM %3$s," TCLIENT
				" WHERE %3$s.uOwner=tClient.uClient"
                                " AND %3$s.u%4$s=%5$u"
				" AND (tClient.uClient=%2$u OR tClient.uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))",
					cVarList,guCompany,
					cTable,cTable+1,uRow);
}//void ExtSelectRow(...)


void SetSessionCookie(void)
{
	printf("Set-Cookie: unxsSPSSessionCookie=uProject=%u|uTable=%u|uField=%u|; Secure; HttpOnly\n",
					guCookieProject,guCookieTable,guCookieField);
};


void GetSessionCookie(void)
{
	char *ptr;
	char *ptr2;
	char cunxsSPSSessionCookie[512]={""};

	if(getenv("HTTP_COOKIE")!=NULL)
		sprintf(gcCookie,"%.1022s",getenv("HTTP_COOKIE"));
	if(gcCookie[0])
	{
		if((ptr=strstr(gcCookie,"unxsSPSSessionCookie=")))
		{
			ptr+=strlen("unxsSPSSessionCookie=");
			if((ptr2=strchr(ptr,';')))
			{
				*ptr2=0;
				sprintf(cunxsSPSSessionCookie,"%.511s",ptr);
			}
			else
				 sprintf(cunxsSPSSessionCookie,"%.511s",ptr);

		}
	}


	if(cunxsSPSSessionCookie[0])
	{
		if((ptr=strstr(cunxsSPSSessionCookie,"uProject=")))
		{
			ptr+=strlen("uProject=");
			if((ptr2=strchr(ptr,'|')))
			{
				*ptr2=0;
				sscanf(ptr,"%u",&guCookieProject);
				*ptr2='|';
			}
		}
		if((ptr=strstr(cunxsSPSSessionCookie,"uTable=")))
		{
			ptr+=strlen("uTable=");
			if((ptr2=strchr(ptr,'|')))
			{
				*ptr2=0;
				sscanf(ptr,"%u",&guCookieTable);
				*ptr2='|';
			}
		}
		if((ptr=strstr(cunxsSPSSessionCookie,"uField=")))
		{
			ptr+=strlen("uField=");
			if((ptr2=strchr(ptr,'|')))
			{
				*ptr2=0;
				sscanf(ptr,"%u",&guCookieField);
				*ptr2='|';
			}
		}
	}
}//void GetSessionCookie(void)

