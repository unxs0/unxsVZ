/*
FILE 
	main.c
	$Id: main.c 2828 2015-04-08 21:26:55Z Colin $
AUTHOR/LEGAL
	(C) 2010-2016 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	unxsVZ Portla Interface program file
REQUIRES
	OpenISP libtemplates.a and templates.h
	See makefile for more information.
CURRENT WORK
	Changing template system to require set and type.
	Set is for skin changes.
	Type identifies templates by interface or other function.
*/

#include "interface.h"

//Global vars
//libtemplate.a required
MYSQL gMysql;
//Multipurpose buffer
char gcQuery[4096]={""};

//
//Template vars
char *gcMessage="&nbsp;";
char gcInputStatus[32]={"disabled"};
char gcModStep[32]={""};
char gcNewStep[32]={""};
char gcDelStep[32]={""};

//SSLLoginCookie()
char gcCookie[1024]={""};
char gcLogin[100]={""};

//Create new account
static char gcNewEmail[100]={""};
static char gcNewLogin[100]={""};
static char gcNewPasswd[100]={""};
static char gcNewPasswd2[100]={""};
static char gcPhone[64]={""};
static char gcCompany[64]={""};
static char gcFullName[64]={""};
static char gcNewToken[64]={""};

char cLogKey[16]={"Ksdj458jssdUjf79"};
char gcPasswd[100]={""};
unsigned guSSLCookieLogin=0;
unsigned guRequireOTPLogin=0;
unsigned guOTPExpired=0;
unsigned guForked=0;
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

char gcFunction[100]={""};
char gcPage[100]={""};
unsigned guBrowserFirefox=0;

//
//Local only
int main(int argc, char *argv[]);
int iValidLogin(int mode);
void SSLCookieLogin(void);
void SetLogin(void);
void GetPLAndClient(char *cUser);
void htmlLogin(void);
unsigned uCreateAccount(void);
void htmlNewAccount(void);
void htmlCreateNewAccount(void);
void htmlActivateNewAccount(void);
void htmlLoginPage(char *cTitle, char *cTemplateName);
void UpdateOTPExpire(unsigned uAuthorize,unsigned uClient);
unsigned uNoUpper(const char *cPasswd);
unsigned uNoLower(const char *cPasswd);
unsigned uNoDigit(const char *cPasswd);
unsigned uValidPhoneNumber(const char *cPhone);
char *WordToLower(char *cInput);
char *EmailInput(char *cInput);
void SendEmail(char *cEmail,char *cSubject,char *cMsg);


int main(int argc, char *argv[])
{
	pentry entries[MAXPOSTVARS];
	entry gentries[MAXGETVARS];
	char *gcl;
	register int i;
	int cl=0;


	InterfaceConnectDb();

	if(getenv("REMOTE_ADDR")!=NULL)
		sprintf(gcHost,"%.99s",getenv("REMOTE_ADDR"));

	if(getenv("HTTP_USER_AGENT")!=NULL)
	{
		if(strstr(getenv("HTTP_USER_AGENT"),"Firefox"))
			guBrowserFirefox=1;
	}

	gethostname(gcHostname,98);

	if(strcmp(getenv("REQUEST_METHOD"),"POST"))
	{
		//Get	
		SSLCookieLogin();
		
		gcl = getenv("QUERY_STRING");
		for(i=0;gcl[0] != '\0' && i<MAXGETVARS;i++)
		{
			getword(gentries[i].val,gcl,'&');
			plustospace(gentries[i].val);
			unescape_url(gentries[i].val);
			getword(gentries[i].name,gentries[i].val,'=');

			if(!strcmp(gentries[i].name,"gcFunction"))
				sprintf(gcFunction,"%.99s",gentries[i].val);
			else if(!strcmp(gentries[i].name,"gcPage"))
				sprintf(gcPage,"%.99s",gentries[i].val);
		}
		if(gcPage[0])
		{
			if(!strcmp(gcPage,"Glossary"))
				GlossaryGetHook(gentries,i);
			else if(!strcmp(gcPage,"User"))
				UserGetHook(gentries,i);
		}
	}
	else
	{
		//Post
		
		cl = atoi(getenv("CONTENT_LENGTH"));
		for(i=0;cl && (!feof(stdin)) && i<MAXPOSTVARS ;i++)
		{
			entries[i].val = fmakeword(stdin,'&',&cl);
			plustospace(entries[i].val);
			unescape_url(entries[i].val);
			entries[i].name = makeword(entries[i].val,'=');
			
			if(!strcmp(entries[i].name,"gcFunction"))
				sprintf(gcFunction,"%.99s",entries[i].val);
			else if(!strcmp(entries[i].name,"gcPage"))
				sprintf(gcPage,"%.99s",entries[i].val);
			else if(!strcmp(entries[i].name,"gcLogin"))
				sprintf(gcLogin,"%.99s",entries[i].val);
			else if(!strcmp(entries[i].name,"gcPasswd"))
				sprintf(gcPasswd,"%.99s",entries[i].val);
                	else if(!strcmp(entries[i].name,"gcOTP"))
				sprintf(gcOTP,"%.15s",entries[i].val);
			else if(!strcmp(entries[i].name,"gcNewLogin"))
				sprintf(gcNewLogin,"%.99s",entries[i].val);
			else if(!strcmp(entries[i].name,"gcNewEmail"))
				sprintf(gcNewEmail,"%.99s",entries[i].val);
			else if(!strcmp(entries[i].name,"gcNewPasswd"))
				sprintf(gcNewPasswd,"%.99s",entries[i].val);
			else if(!strcmp(entries[i].name,"gcNewPasswd2"))
				sprintf(gcNewPasswd2,"%.99s",entries[i].val);
			else if(!strcmp(entries[i].name,"gcPhone"))
				sprintf(gcPhone,"%.63s",entries[i].val);
			else if(!strcmp(entries[i].name,"gcCompany"))
				sprintf(gcCompany,"%.63s",entries[i].val);
			else if(!strcmp(entries[i].name,"gcFullName"))
				sprintf(gcFullName,"%.63s",entries[i].val);
			else if(!strcmp(entries[i].name,"gcNewToken"))
				sprintf(gcNewToken,"%.63s",entries[i].val);
		}
	}

	//Not required to be logged in gcFunction section
	if(gcFunction[0])
	{
		if(!strncmp(gcFunction,"Logout",5))
		{
			//8 idnsOrg log type, need to globally add 9 for unxsPortal
			printf("Set-Cookie: unxsPortalLogin=; discard; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
			printf("Set-Cookie: unxsPortalPasswd=; discard; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
			sprintf(gcQuery,"INSERT INTO tLog SET cLabel='logout %.99s',uLogType=8,uPermLevel=%u,"
					"uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=%u,"
					"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname,guOrg);
			mysql_query(&gMysql,gcQuery);
			if(gcOTPSecret[0])
			{
				UpdateOTPExpire(0,guLoginClient);
				guRequireOTPLogin=1;
			}
        		guPermLevel=0;
			gcUser[0]=0;
			guLoginClient=0;
			htmlLogin();
		}
	}

        if(!strcmp(gcFunction,"Login")) 
		SetLogin();
        else if(!strcmp(gcFunction,"NewAccount")) 
		htmlNewAccount();
        else if(!strcmp(gcFunction,"CreateNewAccount")) 
		htmlCreateNewAccount();
        else if(!strcmp(gcFunction,"ActivateNewAccount")) 
		htmlActivateNewAccount();

        if(!guPermLevel || !gcUser[0] || !guLoginClient)
                SSLCookieLogin();

	//First page after valid login
	if(!strcmp(gcFunction,"Login"))
		htmlUser();

	//Per page command tree
	UserCommands(entries,i);
	
	//default logged in page
	htmlUser();
	return(0);

}//end of main()


unsigned uActivateNewAccount(void)
{
	if(!gcNewToken[0])
	{
		gcMessage="Token is required. Copy and paste from Email.";
		return(1);
	}
	if(strlen(gcNewToken)<20)
	{
		gcMessage="Token is missing characters";
		return(1);
	}
	if(strlen(gcNewToken)>20)
	{
		gcMessage="Token has extra characters";
		return(1);
	}

	sprintf(gcQuery,"SELECT uAuthorize FROM tAuthorize WHERE cLabel='%s' AND cIPMask='%s' AND uPerm=0",
				gcNewLogin,gcNewToken);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		gcMessage="System error try again later (1)";
		return(1);
	}
        MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uAuthorize=0;
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)!=1)
	{
		gcMessage="Token not recognized. Or account already activated.";
		return(1);
	}
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uAuthorize);
	mysql_free_result(res);

	if(uAuthorize)
	{
		sprintf(gcQuery,"UPDATE tAuthorize SET cIPMask='0.0.0.0',uPerm=6,uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uAuthorize=%u",
				uAuthorize);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
		{
			gcMessage="System error try again later (2)";
			return(1);
		}
	}
	else
	{
		gcMessage="System error try again later (3)";
		return(1);
	}

	return(0);
}//unsigned uActivateNewAccount(void)


unsigned uCreateAccount(void)
{
	EmailInput(gcNewEmail);
	WordToLower(gcNewLogin);

	if(!gcNewLogin[0])
	{
		gcMessage="Login is required";
		return(1);
	}
	if(!gcNewEmail[0])
	{
		gcMessage="Email is required";
		return(1);
	}
	if(!gcPhone[0])
	{
		gcMessage="Valid phone number is required";
		return(1);
	}
	if(!gcFullName[0])
	{
		gcMessage="Your full name is required";
		return(1);
	}
	if(!gcNewPasswd[0] || !gcNewPasswd2[0])
	{
		gcMessage="Passwords are required";
		return(1);
	}
	if(strcmp(gcNewPasswd,gcNewPasswd2))
	{
		gcMessage="Passwords do not match";
		return(1);
	}
	if(strlen(gcNewPasswd)<6)
	{
		gcMessage="New 'Password' must be at least 6 chars long";
		return(1);
	}
	if(uNoUpper(gcNewPasswd) || uNoLower(gcNewPasswd) || uNoDigit(gcNewPasswd))
	{
		gcMessage="New 'Password' must have some upper and lower case letters,"
						" and at least one number";
		return(1);
	}
	if(uNoUpper(gcFullName))
	{
		gcMessage="Your full name must have at least a first and last name capitalized";
		return(1);
	}
	if(!strchr(gcFullName,' '))
	{
		gcMessage="Your full name must have at least a first and last name capitalized";
		return(1);
	}
	if(!gcCompany[0])
	{
		sprintf(gcCompany,"%.63s",gcFullName);
	}
	if(strlen(gcPhone)<11)
	{
		gcMessage="Telephone number must be at least 11 chars long (E.g. 13105551212)";
		return(1);
	}
	if(!uValidPhoneNumber(gcPhone))
	{
		gcMessage="Your phone number can contain only digits, '+.-' characters and spaces.";
		return(1);
	}



	//do not allow same email or same login accounts
	MYSQL_RES *res;
	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cLabel='%s' OR cLabel='%s' OR cEmail='%s' OR cLabel='%s'",
				gcFullName,gcNewLogin,gcNewEmail,gcCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		gcMessage="System error try again later (1)";
		return(1);
	}
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
		gcMessage="Login, company name and/or email already in use";
		return(1);
	}
	mysql_free_result(res);

	//do not allow same login in tAuthorize
	sprintf(gcQuery,"SELECT uAuthorize FROM tAuthorize WHERE cLabel='%s'",gcNewLogin);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		gcMessage="System error try again later (1)";
		return(1);
	}
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
		gcMessage="Login already in use";
		return(1);
	}
	mysql_free_result(res);

	//create account as potential reseller
	sprintf(gcQuery,"INSERT INTO tClient SET cLabel='%s',"
			"cCode='Organization',cInfo='gcNewLogin=%s;\ngcFullName=%s;\ngcPhone=%s;\n',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				gcCompany,gcNewLogin,gcFullName,gcPhone);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		gcMessage="System error try again later (2)";
		return(1);
	}
	unsigned uOwner=mysql_insert_id(&gMysql);
	sprintf(gcQuery,"INSERT INTO tClient SET cLabel='%s',cEmail='%s',"
			"cCode='Contact',cInfo='gcCompany=%s;\ngcFullName=%s;\ngcPhone=%s;\n',"
				"uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				gcNewLogin,gcNewEmail,
				gcCompany,gcFullName,gcPhone,
				uOwner);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		gcMessage="System error try again later";
		//roll back
		sprintf(gcQuery,"DELETE FROM tClient WHERE uClient=%u",uOwner);
        	mysql_query(&gMysql,gcQuery);
		return(1);
	}

	//passwd encryption
	unsigned uClient=mysql_insert_id(&gMysql);
	char cBuffer[100]={""};
	char cSalt[16]={"$1$23abc123$"};
	(void)srand((int)time((time_t *)NULL));
	to64(&cSalt[3],rand(),8);
	sprintf(cBuffer,"%.99s",gcNewPasswd);
	EncryptPasswdWithSalt(cBuffer,cSalt);

	//uPerm=0 until token is input
	sprintf(gcQuery,"INSERT INTO tAuthorize SET cLabel='%s',cPasswd='%s',uPerm=0,uCertClient=%u,uOwner=%u,"
				"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW()),cIPMask=SHA1(RAND());",
					gcNewLogin,cBuffer,uClient,uOwner);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		gcMessage="System error try again later (3)";
		//roll back
		sprintf(gcQuery,"DELETE FROM tClient WHERE uClient=%u",uOwner);
        	mysql_query(&gMysql,gcQuery);
		sprintf(gcQuery,"DELETE FROM tClient WHERE uClient=%u",uClient);
        	mysql_query(&gMysql,gcQuery);
		return(1);
	}

	//send email with token and link 
	sprintf(gcQuery,"SELECT cIPMask FROM tAuthorize WHERE uAuthorize=%lu",(long unsigned) mysql_insert_id(&gMysql));
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		gcMessage="System error try again later (4)";
		//roll back
		sprintf(gcQuery,"DELETE FROM tClient WHERE uClient=%u",uOwner);
        	mysql_query(&gMysql,gcQuery);
		sprintf(gcQuery,"DELETE FROM tClient WHERE uClient=%u",uClient);
        	mysql_query(&gMysql,gcQuery);
		sprintf(gcQuery,"DELETE FROM tAuthorize WHERE uCertClient=%u",uClient);
        	mysql_query(&gMysql,gcQuery);
		return(1);
	}
	res=mysql_store_result(&gMysql);
	static char cToken[32]={""};
	MYSQL_ROW field;
	if((field=mysql_fetch_row(res)))
		sprintf(cToken,"%.31s",field[0]);
	mysql_free_result(res);
	SendEmail(gcNewEmail,"New account token",cToken);
	
	return(0);
}//unsigned uCreateAccount(void)


void htmlActivateNewAccount(void)
{
	htmlHeader("unxsPortal","LoginHeader");
	if(uActivateNewAccount())
		htmlLoginPage("unxsPortal","NewAccountCreated.Body");
	else
		htmlLoginPage("unxsPortal","NewAccountActivated.Body");
	htmlFooter("LoginFooter");

}//void htmlActivateNewAccount(void)


void htmlCreateNewAccount(void)
{
	htmlHeader("unxsPortal","LoginHeader");
	if(uCreateAccount())
		htmlLoginPage("unxsPortal","NewAccount.Body");
	else
		htmlLoginPage("unxsPortal","NewAccountCreated.Body");
	htmlFooter("LoginFooter");

}//void htmlCreateNewAccount(void)


void htmlNewAccount(void)
{
	htmlHeader("unxsPortal","LoginHeader");
	htmlLoginPage("unxsPortal","NewAccount.Body");
	htmlFooter("LoginFooter");

}//void htmlLogin(void)


void htmlLogin(void)
{
	htmlHeader("unxsPortal","LoginHeader");
	if(guRequireOTPLogin)
	{
		if(strstr(gcOTPInfo,"invalid gcOTP"))
			gcMessage="Two factor authentication six digit one time passcode incorrect or expired";
		htmlLoginPage("unxsPortal","ZLoginOTP.Body");
	}
	else
	{
		if(strstr(gcOTPInfo,"invalid login") && !strcmp(gcFunction,"Login"))
			gcMessage="Invalid login and/or password";
		htmlLoginPage("unxsPortal","ZLogin.Body");
	}
	htmlFooter("LoginFooter");

}//void htmlLogin(void)


void htmlLoginPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uPLAINSET,uPortal);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="";
			
			template.cpName[2]="gcMessage";
			template.cpValue[2]=gcMessage;

			template.cpName[3]="gcBrand";
			template.cpValue[3]=INTERFACE_HEADER_TITLE;

			template.cpName[4]="gcLogin";
			template.cpValue[4]=gcLogin;

			template.cpName[5]="gcNewLogin";
			template.cpValue[5]=gcNewLogin;

			template.cpName[6]="gcNewEmail";
			template.cpValue[6]=gcNewEmail;

			template.cpName[7]="gcFullName";
			template.cpValue[7]=gcFullName;

			template.cpName[8]="gcCompany";
			template.cpValue[8]=gcCompany;

			template.cpName[9]="gcPhone";
			template.cpValue[9]=gcPhone;

			template.cpName[10]="gcOTPInfo";
			template.cpValue[10]=gcOTPInfo;

			template.cpName[11]="";

			printf("\n<!-- Start htmlLoginPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlLoginPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlLoginPage()


void htmlPlainTextError(const char *cError)
{
	char cQuery[1024];

	printf("Content-type: text/plain\n\n");
	printf("Please report this unxsPortal fatal error ASAP:\n%s\n",cError);

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
	if(guForked)
		exit(0);
	printf("Content-type: text/html\n\n");

	if(cTemplateName[0])
	{
		MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uPLAINSET,uPortal);
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

		TemplateSelectInterface(cTemplateName,uPLAINSET,uPortal);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			template.cpName[0]="cIspName";
			template.cpValue[0]=ISPNAME;
			template.cpName[1]="cIspUrl";
			template.cpValue[1]=ISPURL;
			template.cpName[2]="cCopyright";
			template.cpValue[2]=LOCALCOPYRIGHT;
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

void funcPrintTemplateDirect(FILE *fp,char *cTemplateName);
void funcPrintTemplateDirect(FILE *fp,char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uPLAINSET,uPortal);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;

			template.cpName[0]="";

			printf("\n<!-- Start funcPrintTemplateDirect(%s) -->\n",cTemplateName); 
			Template(field[0],&template,stdout);
			printf("\n<!-- End funcPrintTemplateDirect(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf(" %s \n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void funcPrintTemplateDirect(FILE *fp,char *cTemplateName)


//libtemplate.a required
void AppFunctions(FILE *fp,char *cFunction)
{
	unsigned uLen=0;
	if(!strncmp(cFunction,"funcTemplate:",uLen=strlen("funcTemplate:")))
	{
		if(cFunction[uLen]!=0)
		{
			funcPrintTemplateDirect(fp,cFunction+uLen);
		}
	}
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

	sprintf(gcQuery,"SELECT cPasswd,uPerm,cOTPSecret,uOTPExpire,UNIX_TIMESTAMP(NOW()),uAuthorize FROM tAuthorize WHERE cLabel='%s'",
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
#ifdef SSLONLY
	if(getenv("HTTPS")==NULL) 
		htmlPlainTextError("Non SSL access denied");
#endif

	if(getenv("HTTP_COOKIE")!=NULL)
		sprintf(gcCookie,"%.1022s",getenv("HTTP_COOKIE"));
	
	if(gcCookie[0])
	{
		if((cP=strstr(gcCookie,"unxsPortalLogin=")))
		{
			cP+=strlen("unxsPortalLogin=");
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
		if((cP=strstr(gcCookie,"unxsPortalPasswd=")))
		{
			cP+=strlen("unxsPortalPasswd=");
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
		sprintf(gcName,"%.100s",mysqlField[2]);
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
			sprintf(gcOrgName,"%.100s",mysqlField[0]);
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


#include <liboath/oath.h>
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
		sprintf(gcQuery,"UPDATE " TAUTHORIZE " SET uOTPExpire=UNIX_TIMESTAMP(NOW()-1) WHERE uCertClient=%u",
			uClient);
	else
		sprintf(gcQuery,"UPDATE " TAUTHORIZE " SET uOTPExpire=(UNIX_TIMESTAMP(NOW())+28800) WHERE uAuthorize=%u",
			uAuthorize);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
}//void UpdateOTPExpire()


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

	if(!mode)
	{
		sprintf(gcQuery,"INSERT INTO tLog SET cLabel='login failed %.99s',uLogType=8,uPermLevel=%u,uLoginClient=%u,"
		"cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname);
		mysql_query(&gMysql,gcQuery);
	}
	if(guOTPExpired)
		guRequireOTPLogin=1;
	sprintf(gcOTPInfo,"{%s}/[%s] %u invalid login",gcOTPSecret,gcOTP,guOTPExpired);
	return 0;

}//iValidLogin()


void SetLogin(void)
{
	if(iValidLogin(0))
	{
		printf("Set-Cookie: unxsPortalLogin=%s; secure;\n",gcLogin);
		printf("Set-Cookie: unxsPortalPasswd=%s; secure;\n",gcPasswd);
		sprintf(gcUser,"%.41s",gcLogin);
		GetPLAndClient(gcUser);
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

		TemplateSelectInterface(cTemplateName,uPLAINSET,uPortal);
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


const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey)
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
                return(mysqlField[0]);
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

}//const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey)


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


void to64(register char *s,register long v,register int n)
{
	unsigned char itoa64[] =         /* 0 ... 63 => ascii - 64 */
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    while (--n >= 0) {
        *s++ = itoa64[v&0x3f];
        v >>= 6;
    }
}//void to64(register char *s,register long v,register int n)


void SendEmail(char *cEmail,char *cSubject,char *cMsg)
{
	pid_t pidChild;

	pidChild=fork();
	if(pidChild!=0)
		return;

	FILE *fp;
	char cFrom[100]={"support@unixservice.com"};

	if((fp=popen("/usr/lib/sendmail -t > /dev/null","w")))
	{
		fprintf(fp,"To: %s\n",cEmail);
		fprintf(fp,"From: %s\n",cFrom);
		fprintf(fp,"Subject: %s\n\n",cSubject);
		fprintf(fp,"%s\n",cMsg);
	}
	pclose(fp);
	close(1);
	exit(0);

}//void SendEmail()
