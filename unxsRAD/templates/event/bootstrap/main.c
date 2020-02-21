/*
FILE 
	{{cProject}}/interfaces/bootstrap/main.c
	template unxsRAD/templates/new/bootstrap/main.c
AUTHOR/LEGAL
	(C) 2010-2020 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	Bootstrap interface program file
CURRENT WORK
*/

#include "interface.h"

//Global vars
//libtemplate.a required
MYSQL gMysql;
//Multipurpose buffer
char gcQuery[4096]={""};

//
//Template vars
char *gcMessage="";
char gcInputStatus[32]={"disabled"};
char gcModStep[32]={""};
char gcNewStep[32]={""};
char gcDelStep[32]={""};
unsigned guZone=0;
unsigned guView=0;

//APP VARS
//From cookies
unsigned guHeat=0;
unsigned guEvent=0;

//SSLLoginCookie()
char gcCookie[1024]={""};
char gcLogin[100]={""};
char gcEmailCode[32]={""};
char cLogKey[16]={"Ksdj458jssdUjf79"};
char gcPasswd[100]={""};
char gcPasswd2[100]={""};
unsigned guSSLCookieLogin=0;
unsigned guRequireOTPLogin=0;
unsigned guOTPExpired=0;
char gcOTP[16]={""};
char gcOTPInfo[64]={"Nothing yet"};
char gcOTPSecret[65]={""};

int guPermLevel=0;
char gcuPermLevel[4]={""};
unsigned guLoginClient=0;
unsigned guOrg=0;
char gcUser[100]={""};
char gcName[100]={""};
char gcOrgName[100]={""};
char gcHost[100]={""};
char gcHostname[100]={""};
char *gcBrand=INTERFACE_HEADER_TITLE;

char gcFunction[100]={""};
char gcPage[100]={""};

//
//Local only
int main(int argc, char *argv[]);
int iValidLogin(int mode);
void SSLCookieLogin(void);
void SetLogin(void);
void GetPLAndClient(char *cUser);
void htmlLogin(void);
void htmlLoginPage(char *cTitle, char *cTemplateName);
unsigned uGetSessionConfig(const char *cName);
unsigned uSetSessionConfig(const char *cName, unsigned uValue);
char *cEndAtSpace(char *cBuffer);

static char *sgcBuildInfo=dsGitVersion;

int main(int argc, char *argv[])
{
	pentry entries[MAXPOSTVARS];
	entry gentries[MAXGETVARS];
	char *gcl;
	register int x;
	int cl=0;

	InterfaceConnectDb();

	if(getenv("REMOTE_ADDR")!=NULL)
	{
		sprintf(gcHost,"%.99s",getenv("REMOTE_ADDR"));
	}
	else
	{
		printf("%s version: %s\n",argv[0],sgcBuildInfo);
		exit(0);
	}

	gethostname(gcHostname,98);

	char cTable[100]={""};
	if(strcmp(getenv("REQUEST_METHOD"),"POST"))
	{
		
		//Get	
		gcl = getenv("QUERY_STRING");
		for(x=0;gcl[0] != '\0' && x<MAXGETVARS;x++)
		{
			getword(gentries[x].val,gcl,'&');
			plustospace(gentries[x].val);
			unescape_url(gentries[x].val);
			getword(gentries[x].name,gentries[x].val,'=');

			if(!strcmp(gentries[x].name,"gcFunction"))
				sprintf(gcFunction,"%.99s",gentries[x].val);
			else if(!strcmp(gentries[x].name,"gcPage"))
				sprintf(gcPage,"%.99s",gentries[x].val);
                	else if(!strcmp(gentries[x].name,"cTable"))
				sprintf(cTable,"%.99s",gentries[x].val);
			else if(!strcmp(gentries[x].name,"gcLogin"))
				sprintf(gcLogin,"%.99s",WordToLower(gentries[x].val));
		}
		//No login required
        	if(!strcmp(gcFunction,"Heat")) 
			HeatGetHook(gentries,x);
        	else if(!strcmp(gcFunction,"HeatEnd")) 
			HeatEndGetHook(gentries,x);
        	else if(!strcmp(gcFunction,"Overlay")) 
			htmlOverlay();
        	else if(!strcmp(gcFunction,"Wind")) 
			htmlWind();
        	else if(!strcmp(gcFunction,"Tournament")) 
			htmlTournament();
        	else if(!strcmp(gcFunction,"BestTrick")) 
			htmlBestTrick();
		SSLCookieLogin();
		if(gcPage[0])
		{
			if(!strcmp(gcPage,"Admin"))
			{
				AdminGetHook(gentries,x);
			}
			else if(!strcmp(gcPage,"Event"))
			{
				EventGetHook(gentries,x);
			}
			else if(!strcmp(gcPage,"Judge"))
			{
				JudgeGetHook(gentries,x);
			}
			//Need to catch all pages here or else have default
			JudgeGetHook(gentries,x);
		}
		else if(!gcFunction[0])
		{
			JudgeGetHook(gentries,x);
		}
	}
	else
	{
		//Post with get
		gcl = getenv("QUERY_STRING");
		for(x=0;gcl[0] != '\0' && x<MAXGETVARS;x++)
		{
			getword(gentries[x].val,gcl,'&');
			plustospace(gentries[x].val);
			unescape_url(gentries[x].val);
			getword(gentries[x].name,gentries[x].val,'=');

			if(!strcmp(gentries[x].name,"gcFunction"))
				sprintf(gcFunction,"%.99s",gentries[x].val);
		}

		//Post
		cl = atoi(getenv("CONTENT_LENGTH"));
		for(x=0;cl && (!feof(stdin)) && x<MAXPOSTVARS ;x++)
		{
			entries[x].val = fmakeword(stdin,'&',&cl);
			plustospace(entries[x].val);
			unescape_url(entries[x].val);
			entries[x].name = makeword(entries[x].val,'=');
			
			if(!strcmp(entries[x].name,"gcFunction"))
				sprintf(gcFunction,"%.99s",entries[x].val);
			else if(!strcmp(entries[x].name,"gcPage"))
				sprintf(gcPage,"%.99s",entries[x].val);
			else if(!strcmp(entries[x].name,"gcLogin"))
				sprintf(gcLogin,"%.99s",WordToLower(entries[x].val));
			else if(!strcmp(entries[x].name,"gcEmailCode"))
				sprintf(gcEmailCode,"%.31s",entries[x].val);
			else if(!strcmp(entries[x].name,"gcPasswd"))
				sprintf(gcPasswd,"%.99s",entries[x].val);
			else if(!strcmp(entries[x].name,"gcPasswd2"))
				sprintf(gcPasswd2,"%.99s",entries[x].val);
                	else if(!strcmp(entries[x].name,"gcOTP"))
				sprintf(gcOTP,"%.15s",entries[x].val);
                	else if(!strcmp(entries[x].name,"cTable"))
				sprintf(cTable,"%.99s",entries[x].val);
		}
	}

	//Not required to be logged in sections
	//if(gcPage[0])
	//{
	//}
	if(gcFunction[0])
	{
		if(!strncmp(gcFunction,"Logout",5))
		{
			//8 idnsOrg log type, need to globally add 9 for OneLogin
			printf("Set-Cookie: {{cProject}}SessionId=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
			printf("Set-Cookie: {{cProject}}SessionHash=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
			printf("Set-Cookie: {{cProject}}Event=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
			printf("Set-Cookie: {{cProject}}Heat=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
			sprintf(gcQuery,"INSERT INTO tLog SET cLabel='logout %.99s',uLogType=8,uPermLevel=%u,"
					"uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=%u,"
					"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname,guOrg);
			mysql_query(&gMysql,gcQuery);
			if(gcOTPSecret[0])
			{
				//UpdateOTPExpire(0,guLoginClient);
				guRequireOTPLogin=1;
			}
        		guPermLevel=0;
			gcUser[0]=0;
			guLoginClient=0;
			htmlLogin();
		}
        	else if(!strcmp(gcFunction,"Login")) 
			SetLogin();
        	else if(!strcmp(gcFunction,"Heat")) 
			htmlHeat();
        	else if(!strcmp(gcFunction,"HeatEnd")) 
			htmlHeatEnd();
        	else if(!strcmp(gcFunction,"BestTrick")) 
			htmlBestTrick();
        	else if(!strcmp(gcFunction,"Wind")) 
			htmlWind();
        	else if(!strcmp(gcFunction,"Tournament")) 
			htmlTournament();
        	else if(!strcmp(gcFunction,"HeatEnd")) 
			htmlHeatEnd();
        	else if(!strcmp(gcFunction,"Event")) 
			htmlEvent();
        	else if(!strcmp(gcFunction,"Admin")) 
			htmlAdmin();
	}

        if(!guPermLevel || !gcUser[0] || !guLoginClient)
                SSLCookieLogin();

	//First page after valid login
	if(!strcmp(gcFunction,"Login"))
		htmlJudge();

	//Per page command tree
	JudgeCommands(entries,x);
	//Main Post Menu
	
	if(gcPage[0])
	{
		if(!strcmp(gcPage,"Judge"))
			htmlJudge();
	}
	//default logged in page
	htmlJudge();
	return(0);

}//end of main()


void htmlLogin(void)
{
	htmlHeader("{{cProject}}","Default.Header");
	if(guRequireOTPLogin)
		htmlLoginPage("{{cProject}}","LoginOTP.Body");
	else
		htmlLoginPage("{{cProject}}","Login.Body");
	htmlFooter("Default.Footer");

}//void htmlLogin(void)


void htmlLoginPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uDEFAULT,uBOOTSTRAP);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="";
			
			template.cpName[2]="cMessage";
			template.cpValue[2]=gcMessage;

			template.cpName[3]="gcBrand";
			template.cpValue[3]=gcBrand;

			template.cpName[4]="gcLogin";
			if(!gcLogin[0]) sprintf(gcLogin,"Login id");
			template.cpValue[4]=gcLogin;

			template.cpName[5]="";

			printf("\n<!-- Start htmlLoginPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlLoginPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>d1 %s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlLoginPage()


void htmlPlainTextError(const char *cError)
{
	char cQuery[1024];

	printf("Content-type: text/plain\n\n");
	printf("Please report this fatal error ASAP:\n%s\n",cError);

	//Attempt to report error in tLog
        sprintf(cQuery,"INSERT INTO tLog SET cLabel='htmlPlainTextError',uLogType=2,uPermLevel=%u,uLoginClient=%u,"
			"cLogin='%s',cHost='%s',cMessage=\"%s\",cServer='%s',uOwner=1,uCreatedBy=%u,"
			"uCreatedDate=UNIX_TIMESTAMP(NOW())",
				guPermLevel,guLoginClient,gcLogin,gcHost,cError,gcHostname,guLoginClient);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		printf("Another error occurred while attempting to log: %s\n",
				mysql_error(&gMysql));
	exit(0);

}//void htmlPlainTextError(const char *cError)


void htmlHeader(char *cTitle, char *cTemplateName)
{
	printf("Content-type: text/html\n\n");

	if(cTemplateName[0])
	{
		MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uDEFAULT,uBOOTSTRAP);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
		
			template.cpName[1]="";

			printf("\n<!-- Start htmlHeader(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlHeader(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr><center><font size=1>%s %s</font>",cTitle,cTemplateName);
		}
		mysql_free_result(res);
	}
	else
	{
		printf("<html><head><title>%s</title></head><body bgcolor=white><font face=Arial,Helvetica",cTitle);
	}

}//void htmlHeader()


void htmlFooter(char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uDEFAULT,uBOOTSTRAP);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			template.cpName[0]="cIspName";
			template.cpValue[0]=ISPNAME;
			template.cpName[1]="cIspUrl";
			template.cpValue[1]=ISPURL;
			template.cpName[2]="cCopyright";
			template.cpValue[2]=INTERFACE_COPYRIGHT;
			template.cpName[3]="";

			printf("\n<!-- Start htmlFooter(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlFooter(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}
	else
	{
		printf("<hr>");
		printf("<center><font size=1><a href=%s>%s</a>\n",
				ISPURL,ISPNAME);
		printf("</body></html>");
	}

	exit(0);

}//void htmlFooter()


void funcSelect(FILE *fp,char *cTable,unsigned uSelector)
{

	fprintf(fp,"<!-- funcSelect(%s) -->\n",cTable);
	MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT _rowid,cLabel FROM %s ORDER by cLabel",cTable);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"<!-- error: %s -->\n",mysql_error(&gMysql));
		return;
	}

	fprintf(fp,"\t\t<select class=\"form-control\" id=\"u%1$s\" name=\"u%1$s\">\n",cTable+1);
	res=mysql_store_result(&gMysql);
	//fprintf(fp,"<option>  </option>\n");
	while((field=mysql_fetch_row(res)))
	{
		unsigned uRowId=0;
		sscanf(field[0],"%u",&uRowId);
		fprintf(fp,"\t\t\t<option value='%u'",uRowId);
		if(uRowId==uSelector)
			fprintf(fp,"selected");
		fprintf(fp,">%s</option>\n",field[1]);
	}
	fprintf(fp,"\t\t</select>\n");

}//void funcSelect()


void funcSelectOwner(FILE *fp,char *cTable,unsigned uSelector)
{

	fprintf(fp,"<!-- funcSelectOwner(%s) -->\n",cTable);
	MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT _rowid,cLabel FROM %s WHERE uOwner=%u ORDER by cLabel",cTable,guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"<!-- error: %s -->\n",mysql_error(&gMysql));
		return;
	}

	fprintf(fp,"\t\t<select class=\"form-control\" id=\"u%1$s\" name=\"u%1$s\">\n",cTable+1);
	res=mysql_store_result(&gMysql);
	//fprintf(fp,"<option>  </option>\n");
	while((field=mysql_fetch_row(res)))
	{
		unsigned uRowId=0;
		sscanf(field[0],"%u",&uRowId);
		fprintf(fp,"\t\t\t<option value='%u'",uRowId);
		if(uRowId==uSelector)
			fprintf(fp,"selected");
		fprintf(fp,">%s</option>\n",field[1]);
	}
	fprintf(fp,"\t\t</select>\n");

}//void funcSelectOwner()


//libtemplate.a required
void AppFunctions(FILE *fp,char *cFunction)
{
	//Page
	if(!strcmp(cFunction,"funcJudge"))
		funcJudge(fp);
	else if(!strcmp(cFunction,"funcEvent"))
		funcEvent(fp);
	else if(!strcmp(cFunction,"funcAdmin"))
		funcAdmin(fp);

	//Overlays
	else if(!strcmp(cFunction,"funcHeat"))
		funcHeat(fp);
	else if(!strcmp(cFunction,"funcWind"))
		funcWind(fp);
	else if(!strcmp(cFunction,"funcHeatEnd"))
		funcHeatEnd(fp);
	else if(!strcmp(cFunction,"funcBestTrick"))
		funcBestTrick(fp);
	else if(!strcmp(cFunction,"funcOverlay"))
		funcOverlay(fp);

}//void AppFunctions(FILE *fp,char *cFunction)


//
//Login functions section
char *cGetPasswd(char *gcLogin,char *cOTPSecret,unsigned long *luOTPExpire,unsigned long *luSQLNow,unsigned *uAuthorize)
{
	static char cPasswd[100]={""};
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	char *cp;

	//SQL injection code
	if((cp=strchr(gcLogin,'\''))) *cp=0;

	//sprintf(gcQuery,"SELECT cPasswd,uPerm,cOTPSecret,uOTPExpire,UNIX_TIMESTAMP(NOW()),uAuthorize"
	sprintf(gcQuery,"SELECT cPasswd,uPerm,'',0,UNIX_TIMESTAMP(NOW()),uAuthorize"
			" FROM tAuthorize WHERE cLabel='%s'",
				gcLogin);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlPlainTextError(mysql_error(&gMysql));
	}
	mysqlRes=mysql_store_result(&gMysql);
	cPasswd[0]=0;
	if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sprintf(cPasswd,"%.99s",mysqlField[0]);
		//CHANGES A GLOBAL HERE!
		sscanf(mysqlField[1],"%u",&guPermLevel);
		if(mysqlField[2])
		{
			sprintf(cOTPSecret,"%.64s",mysqlField[2]);
			sscanf(mysqlField[3],"%lu",luOTPExpire);
			sscanf(mysqlField[4],"%lu",luSQLNow);
			sscanf(mysqlField[5],"%u",uAuthorize);
		}
	}
	mysql_free_result(mysqlRes);
	
	return(cPasswd);

}//char *cGetPasswd()


void SSLCookieLogin(void)
{
	char *cP,*cP2;

	//Parse out login and passwd from cookies
	if(getenv("HTTP_COOKIE")!=NULL)
		sprintf(gcCookie,"%.1022s",getenv("HTTP_COOKIE"));
	
	if(gcCookie[0])
	{
		if((cP=strstr(gcCookie,"{{cProject}}SessionId=")))
		{
			cP+=strlen("{{cProject}}SessionId=");
			if((cP2=strchr(cP,';')))
			{
				*cP2=0;
				sprintf(gcLogin,"%.99s",cP);
				*cP2=';';
			}
			else
			{
				sprintf(gcLogin,"%.99s",cP);
			}
		}
		if((cP=strstr(gcCookie,"{{cProject}}SessionHash=")))
		{
			cP+=strlen("{{cProject}}SessionHash=");
			if((cP2=strchr(cP,';')))
			{
				*cP2=0;
				sprintf(gcPasswd,"%.99s",cP);
				*cP2=';';
			}
			else
			{
				sprintf(gcPasswd,"%.99s",cP);
			}
		}
		if((cP=strstr(gcCookie,"{{cProject}}Heat=")))
		{
			cP+=strlen("{{cProject}}Heat=");
			sscanf(cP,"%u",&guHeat);
		}
		if((cP=strstr(gcCookie,"{{cProject}}Event=")))
		{
			cP+=strlen("{{cProject}}Event=");
			sscanf(cP,"%u",&guEvent);
		}
	}//if gcCookie[0] time saver


	//First try tClient/tAuthorize system
	if(!iValidLogin(1))
	{
		htmlLogin();
	}
	else
	{
		sprintf(gcUser,"%.41s",gcLogin);
		GetPLAndClient(gcUser);
	}

	//Don't allow Root login at this interface
	if(!guPermLevel || !guLoginClient)
	{
        	guPermLevel=0;
		gcUser[0]=0;
		guLoginClient=0;
		htmlLogin();
	}
	
        //if(!strcmp(gcFunction,"Login"))
	//	SetLogin();
	gcPasswd[0]=0;
	guSSLCookieLogin=1;

}//SSLCookieLogin()


void GetPLAndClient(char *cUser)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

	sprintf(gcQuery,"SELECT tAuthorize.uPerm,tAuthorize.uCertClient,tClient.cLabel,tClient.uOwner FROM"
			" tAuthorize,tClient WHERE tAuthorize.uCertClient=tClient.uClient AND tAuthorize.cLabel='%s'",
				cUser);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	mysqlRes=mysql_store_result(&gMysql);
	if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[0],"%d",&guPermLevel);
		sscanf(mysqlField[1],"%u",&guLoginClient);
		sprintf(gcName,"%.99s",mysqlField[2]);
		sscanf(mysqlField[3],"%u",&guOrg);
	}
	mysql_free_result(mysqlRes);

	if(guOrg==1)
		sprintf(gcOrgName,"ASP Provider");
	else
	{
		sprintf(gcQuery,"SELECT cLabel FROM tClient WHERE uClient=%u",guOrg);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		mysqlRes=mysql_store_result(&gMysql);
		if((mysqlField=mysql_fetch_row(mysqlRes)))
			sprintf(gcOrgName,"%.99s",mysqlField[0]);
		mysql_free_result(mysqlRes);
	}

}//void GetPLAndClient()


void EncryptPasswdWithSalt(char *pw, char *salt)
{
	char passwd[102]={""};
	char *cpw;
			
	sprintf(passwd,"%.101s",pw);
				
	cpw=crypt(passwd,salt);

	sprintf(pw,"%s",cpw);

}//void EncryptPasswdWithSalt(char *pw, char *salt)


int iValidLogin(int mode)
{
	char cSalt[16]={""};
	char cPassword[100]={""};

	//Notes:
	//Mode=1 means we have encrypted passwd from cookie

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
				return(1);
			}
		}
		else
		{
			if(!strcmp(gcPasswd,cPassword))
			{
				return(1);
			}
		}
	}

	if(!mode)
	{
		sprintf(gcQuery,"INSERT INTO tLog SET cLabel='login failed %.99s',uLogType=8,uPermLevel=%u,uLoginClient=%u,"
		"cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname);
		mysql_query(&gMysql,gcQuery);
	}
	return 0;

}//iValidLogin()


void SetLogin(void)
{
	if(iValidLogin(0))
	{
		sprintf(gcUser,"%.41s",gcLogin);
		GetPLAndClient(gcUser);
		if(guPermLevel>10)
		{
			sprintf(gcQuery,"INSERT INTO tLog SET cLabel='login2 NOT OK %.99s',uLogType=8,uPermLevel=%u,uLoginClient=%u,"
				"cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=%u,uCreatedBy=1,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",
				gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname,guOrg);
			mysql_query(&gMysql,gcQuery);
			guSSLCookieLogin=0;
			SSLCookieLogin();
		}
		printf("Set-Cookie: {{cProject}}SessionId=%s; secure; httponly; samesite=strict;\n",gcLogin);
		printf("Set-Cookie: {{cProject}}SessionHash=%s; secure; httponly; samesite=strict;\n",gcPasswd);
		guSSLCookieLogin=1;
		//tLogType.cLabel='org login'->uLogType=8
		sprintf(gcQuery,"INSERT INTO tLog SET cLabel='login2 ok %.99s',uLogType=8,uPermLevel=%u,uLoginClient=%u,"
				"cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=%u,uCreatedBy=1,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",
				gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname,guOrg);
		mysql_query(&gMysql,gcQuery);
	}
	else
	{
		guSSLCookieLogin=0;
		SSLCookieLogin();
	}
				
}//void SetLogin(void)

//
//End login fuctions section
//
//RAD / mysqlBind functions

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


const char *cUserLevel(unsigned uPermLevel)
{
	switch(uPermLevel)
	{
		case 12:
			return(BO_ROOT);
		break;
		case 10:
			return(BO_ADMIN);
		break;
		case 8:
			return(BO_RESELLER);
		break;
		case 7:
			return(BO_CUSTOMER);
		break;
		case 6:
			return(ORG_ADMIN);
			//debug login system
			//return(gcOTPInfo);
		break;
		case 5:
			return(ORG_ACCT);
		break;
		case 4:
			return(ORG_SERVICE);
		break;
		case 3:
			return(ORG_SALES);
		break;
		case 2:
			return(ORG_WEBMASTER);
		break;
		case 1:
			return(ORG_CUSTOMER);
		break;
		default:
			return("---");
		break;
	}

}//const char *cUserLevel(unsigned uPermLevel)


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

	if(!cCopy) htmlPlainTextError("TextAreaInput() malloc error");

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


char *FQDomainName(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
	{
		if(!isalnum(cInput[i]) && cInput[i]!='.'  && cInput[i]!='-' )
			break;
	}
	cInput[i]=0;

	return(cInput);

}//char *FQDomainName(char *cInput)


void unxsvzLog(unsigned uTablePK,char *cTableName,char *cLogEntry,unsigned guPermLevel,unsigned guLoginClient,char *gcLogin,char *gcHost)
{
        char cQuery[512];

	//uLogType==2 is this org interface
        sprintf(cQuery,"INSERT INTO tLog SET cLabel='%.63s',uLogType=2,uPermLevel=%u,uLoginClient=%u,cLogin='%.99s',"
			"cHost='%.99s',uTablePK=%u,cTableName='%.31s',"
			"cHash=MD5(CONCAT('%s','%u','%u','%s','%s','%u','%s','%s')),uOwner=%u,uCreatedBy=%u,"
			"uCreatedDate=UNIX_TIMESTAMP(NOW())",
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
			cLogKey,guOrg,guLoginClient);
        mysql_query(&gMysql,cQuery);

}//void unxsvzLog()


void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template)
{
	if(cTemplateName[0])
	{	
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uDEFAULT,uBOOTSTRAP);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"\n<!-- Start fpTemplate(%s) -->\n",cTemplateName); 
			Template(field[0], template, fp);
			fprintf(fp,"\n<!-- End fpTemplate(%s) -->\n",cTemplateName); 
		}
		else
		{
			fprintf(fp,"<hr>");
			fprintf(fp,"<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template)


const char *cForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

	static char cQuery[512];
	static char cKey[16];

        sprintf(cQuery,"SELECT %s FROM %s WHERE _rowid=%u",
                        cFieldName,cTableName,uKey);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		return(mysql_error(&gMysql));

        mysqlRes=mysql_store_result(&gMysql);
        if(mysql_num_rows(mysqlRes)==1)
        {
                mysqlField=mysql_fetch_row(mysqlRes);
		if(mysqlField[0][0])
		{
			return(mysqlField[0]);
		}
		else
		{
			sprintf(cKey,"%u",uKey);
			return(cKey);
		}
        }
	
	if(!uKey)
	{
        	return("---");
	}
	else
	{
		sprintf(cKey,"%u",uKey);
        	return(cKey);
	}

}//const char *cForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey)


const char *cForeignKeyStr(const char *cTableName, const char *cFieldName, const char *cKey)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

	static char cQuery[512];

        sprintf(cQuery,"SELECT %s FROM %s WHERE _rowid=%s",
                        cFieldName,cTableName,cKey);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		return(mysql_error(&gMysql));

        mysqlRes=mysql_store_result(&gMysql);
        if(mysql_num_rows(mysqlRes)==1)
        {
                mysqlField=mysql_fetch_row(mysqlRes);
                return(mysqlField[0]);
        }
	
	if(!cKey[0])
        	return("---");
	else
        	return(cKey);

}//const char *cForeignKeyStr(const char *cTableName, const char *cFieldName, const char *cKey)


int ReadPullDownOwner(const char *cTableName,const char *cFieldName,
				const char *cLabel,const unsigned uOwner)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

        unsigned int iRowid=0;//Not found

        sprintf(gcQuery,"SELECT _rowid FROM %s WHERE %s='%s' AND (uOwner=%u OR uOwner=%u)",
                        cTableName,cFieldName,TextAreaSave((char *) cLabel),uOwner,guOrg);
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


unsigned uGetSessionConfig(const char *cName)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uRetVal=0;

        sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='%s' AND uOwner=%u AND uCreatedBy=%u LIMIT 1",
		cName,guOrg,guLoginClient);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		return(uRetVal);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uRetVal);
        mysql_free_result(res);
	return(uRetVal);

}//unsigned uGetSessionConfig(const char *cName)


unsigned uSetSessionConfig(const char *cName, unsigned uValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        unsigned uConfiguration=0;

        sprintf(gcQuery,"SELECT uConfiguration FROM tConfiguration WHERE cLabel='%s' AND uOwner=%u AND uCreatedBy=%u LIMIT 1",
                cName,guOrg,guLoginClient);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                return(1);
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
                sscanf(field[0],"%u",&uConfiguration);
        mysql_free_result(res);
        if(uConfiguration)
                sprintf(gcQuery,"UPDATE tConfiguration SET cValue='%u',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
                                " WHERE uConfiguration=%u",uValue,guLoginClient,uConfiguration);
        else
                sprintf(gcQuery,"INSERT INTO tConfiguration SET cLabel='%.32s',cValue='%u',"
                                "uOwner=%u,uCreatedBy=%u,"
                                "uCreatedDate=UNIX_TIMESTAMP(NOW())",
                                                cName,uValue,
                                                guOrg,guLoginClient);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                return(1);
        return(0);

}//unsigned uSetSessionConfig(const char *cName,unsigned uValue)


//If possible within our defined constraints
char *cEndAtSpace(char *cBuffer)
{
#define uMaxStringLen 24
#define uSmallestWord 3
	static char *cReturn=NULL;
	cReturn=(char *)malloc(32);
        if(!cReturn) return("Malloc Error");

	cReturn[0]=0;

	if(!cBuffer[0]) return(cReturn);

	register int i;
	unsigned uStart=strlen(cBuffer);
	if(uStart>uMaxStringLen) uStart=uMaxStringLen;

	for(i=uStart;i>0;i--)
		if(cBuffer[i]==' ') break;

	if(i<uSmallestWord) i=uMaxStringLen;

	cBuffer[i]=0;
	sprintf(cReturn,"%.31s",cBuffer);
	cBuffer[i]=' ';

	return(cReturn);

}//char *cEndAtSpace(char *cBuffer)


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

