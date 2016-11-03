/*
FILE 
	main.c
	svn ID removed
AUTHOR
	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	ispAdmin main module.
REQUIRES
	OpenISP libtemplates.a and templates.h
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
char gcPermInputStatus[32]={"disabled"};
char gcModStep[32]={""};
char gcNewStep[32]={""};
char gcDelStep[32]={""};
//SSLLoginCookie()
char gcCookie[1024]={""};
char gcLogin[100]={""};
char cLogKey[16]={"Ksdj458jssdUjf79"};
char gcPasswd[100]={""};
unsigned guSSLCookieLogin=0;

int guPermLevel=0;
char gcuPermLevel[4]={""};
unsigned guLoginClient=0;
unsigned guOrg=0;
unsigned guReseller=0;
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
void htmlLoginPage(char *cTitle, char *cTemplateName);
void htmlDashBoard(void);
void htmlDashBoardPage(char *cTitle, char *cTemplateName);
void EncryptPasswd(char *pw);


int main(int argc, char *argv[])
{
	pentry entries[MAXPOSTVARS];
	entry gentries[MAXGETVARS];
	char *gcl;
	register int i;
	int cl=0;


	ConnectDb();

	if(getenv("REMOTE_ADDR")!=NULL)
		sprintf(gcHost,"%.99s",getenv("REMOTE_ADDR"));

	if(getenv("HTTP_USER_AGENT")!=NULL)
	{
		if(strstr(getenv("HTTP_USER_AGENT"),"Firefox"))
			guBrowserFirefox=1;
	}
#if defined(Linux)
	gethostname(gcHostname, 98);
#else
	//Solaris
	sysinfo(SI_HOSTNAME, gcHostname, 98);
#endif

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
			if(!strcmp(gcPage,"Customer"))
				CustomerGetHook(gentries,i);
			else if(!strcmp(gcPage,"Dashboard"))
				htmlDashBoard();
			else if(!strcmp(gcPage,"Glossary"))
				GlossaryGetHook(gentries,i);
			else if(!strcmp(gcPage,"Invoice"))
				InvoiceGetHook(gentries,i);
			else if(!strcmp(gcPage,"Product"))
				ProductGetHook(gentries,i);
			else if(!strcmp(gcPage,"Instance"))
				InstanceGetHook(gentries,i);

		}
	}
	else
	{
		//Post
		
		cl = atoi(getenv("CONTENT_LENGTH"));
//printf("Content-type:text/plain\n\n");
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
//printf("%s=%s\n",entries[i].name,entries[i].val);
		}
//exit(0);
	}

	//Not required to be logged in gcFunction section
	if(gcFunction[0])
	{
		if(!strncmp(gcFunction,"Logout",5))
		{
		printf("Set-Cookie: ispAdminLogin=; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
		printf("Set-Cookie: ispAdminPasswd=; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
		sprintf(gcQuery,"INSERT INTO tLog SET cLabel='logout %.99s',uLogType=7,uPermLevel=%u,uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname,guOrg);
			mysql_query(&gMysql,gcQuery);
        		guPermLevel=0;
			gcUser[0]=0;
			guLoginClient=0;
			htmlLogin();
		}
	}

        if(!strcmp(gcFunction,"Login")) SetLogin();

        if(!guPermLevel || !gcUser[0] || !guLoginClient)
                SSLCookieLogin();

	//First page after valid login
	if(!strcmp(gcFunction,"Login"))
		htmlDashBoard();

	//Per page command tree
	CustomerCommands(entries,i);
	ProductCommands(entries,i);
	InvoiceCommands(entries,i);
	InstanceCommands(entries,i);

	//default logged in page
	htmlCustomer();
	return(0);

}//end of main()


void htmlLogin(void)
{
	htmlHeader("unxsISP System","Header");
	htmlLoginPage("","AdminLogin.Body");
	htmlFooter("Footer");

}//void htmlLogin(void)


void htmlLoginPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect(cTemplateName);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="ispAdmin.cgi";
			
			template.cpName[2]="cMessage";
			template.cpValue[2]=gcMessage;

			template.cpName[3]="";

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
	printf("Please report this ispAdmin fatal error ASAP:\n%s\n",cError);

	//Attempt to report error in tLog
        sprintf(cQuery,"INSERT INTO tLog SET cLabel='htmlPlainTextError',uLogType=4,uPermLevel=%u,uLoginClient=%u,cLogin='%s',cHost='%s',cMessage=\"%s\",cServer='%s',uOwner=1,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",guPermLevel,guLoginClient,gcLogin,gcHost,cError,gcHostname,guLoginClient);
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

		TemplateSelect(cTemplateName);
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

		TemplateSelect(cTemplateName);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			template.cpName[0]="cIspName";
			template.cpValue[0]=ISPNAME;
			template.cpName[1]="cIspUrl";
			template.cpValue[1]=ISPURL;
			template.cpName[2]="cCopyright";
			template.cpValue[2]="&copy; 2007-2009 Unixservice. All Rights Reserved.";
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


void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template)
{
	if(cTemplateName[0])
	{	
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect(cTemplateName);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"<!-- start %s -->\n",cTemplateName);
			Template(field[0], template, fp);
		}
		else
		{
			fprintf(fp,"<hr>");
			fprintf(fp,"<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}
}//void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template)


void ConnectDb(void)
{
        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,0,NULL,0))
        {
		if (!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
		{
			printf("Content-type: text/plain\n\n");
			printf("System is undergoing maintenance, please try again later.\n\n");
			printf("Debug info: %s\n",mysql_error(&gMysql));
			exit(0);
		}
        }

}//end of ConnectDb()


//libtemplate.a required
void AppFunctions(FILE *fp,char *cFunction)
{
	if(!strcmp(cFunction,"funcSelectProduct"))
		funcSelectProduct(fp);
	else if(!strcmp(cFunction,"funcProductParameterInput"))
		funcProductParameterInput(fp);
	else if(!strcmp(cFunction,"funcEnteredParameters"))
		funcEnteredParameters(fp);
	else if(!strcmp(cFunction,"funcCustomerProducts"))
		funcCustomerProducts(fp);
	else if(!strcmp(cFunction,"funcSelectPeriod"))
		funcSelectPeriod(fp);
	else if(!strcmp(cFunction,"funcSelectAvailable"))
		funcSelectAvailable(fp);
	else if(!strcmp(cFunction,"funcProductServices"))
		funcProductServices(fp);
	else if(!strcmp(cFunction,"funcServicesToAdd"))
		funcServicesToAdd(fp);
	else if(!strcmp(cFunction,"funcSelectExpMonth"))
		funcSelectExpMonth(fp);
	else if(!strcmp(cFunction,"funcSelectExpYear"))
		funcSelectExpYear(fp);
	else if(!strcmp(cFunction,"funcSelectCardType"))
		funcSelectCardType(fp);
	else if(!strcmp(cFunction,"funcSelectPayment"))
		funcSelectPayment(fp);
	else if(!strcmp(cFunction,"funcEditProductParams"))
		funcEditProductParams(fp);
	else if(!strcmp(cFunction,"funcInvoice"))
		funcInvoice(fp);
	else if(!strcmp(cFunction,"funcDateSelectTable"))
		funcDateSelectTable(fp);
	else if(!strcmp(cFunction,"funcInstanceOperations"))
		funcInstanceOperations(fp);
	else if(!strcmp(cFunction,"funcSelectAccountType"))
		funcSelectAccountType(fp);
	else if(!strcmp(cFunction,"funcDisplayDashBoard"))
		funcDisplayDashBoard(fp);
	else if(!strcmp(cFunction,"funcCustomerNavList"))
		funcCustomerNavList(fp);
	else if(!strcmp(cFunction,"funcProductNavList"))
		funcProductNavList(fp);
	else if(!strcmp(cFunction,"funcCustomerNavBar"))
		funcCustomerNavBar(fp);
	else if(!strcmp(cFunction,"funcProductNavBar"))
		funcProductNavBar(fp);
	else if(!strcmp(cFunction,"funcInvoiceNavList"))
		funcInvoiceNavList(fp);
	else if(!strcmp(cFunction,"funcSelectLanguage"))
		funcSelectLanguage(fp);
	else if(!strcmp(cFunction,"funcCustomerReport"))
		funcCustomerReport(fp);
	else if(!strcmp(cFunction,"funcTypeOfAccess"))
		funcTypeOfAccess(fp);

}//void AppFunctions(FILE *fp,char *cFunction)


//
//Login functions section
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
		htmlPlainTextError(mysql_error(&gMysql));
	}
	mysqlRes=mysql_store_result(&gMysql);
	cPasswd[0]=0;
	if((mysqlField=mysql_fetch_row(mysqlRes)))
		sprintf(cPasswd,"%.99s",mysqlField[0]);
	mysql_free_result(mysqlRes);

	
	return(cPasswd);

}//char *cGetPasswd(char *gcLogin)


void SSLCookieLogin(void)
{
	char *ptr,*ptr2;

	//Parse out login and passwd from cookies
#ifdef SSLONLY
	if(getenv("HTTPS")==NULL) 
		htmlPlainTextError("Non SSL access denied");
#endif

	if(getenv("HTTP_COOKIE")!=NULL)
		sprintf(gcCookie,"%.1022s",getenv("HTTP_COOKIE"));
	
	if(gcCookie[0])
	{

	if((ptr=strstr(gcCookie,"ispAdminLogin=")))
	{
		ptr+=strlen("ispAdminLogin=");
		if((ptr2=strchr(ptr,';')))
		{
			*ptr2=0;
			sprintf(gcLogin,"%.99s",ptr);
			*ptr2=';';
		}
		else
		{
			sprintf(gcLogin,"%.99s",ptr);
		}
	}
	if((ptr=strstr(gcCookie,"ispAdminPasswd=")))
	{
		ptr+=strlen("ispAdminPasswd=");
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
		htmlLogin();

	sprintf(gcUser,"%.41s",gcLogin);
	GetPLAndClient(gcUser);
	if(!guPermLevel || !guLoginClient)
		htmlPlainTextError("Unexpected guPermLevel or guLoginClient value");
	
	if(guPermLevel<9)
		htmlLogin();

	gcPasswd[0]=0;
	guSSLCookieLogin=1;

}//SSLCookieLogin()


void GetPLAndClient(char *cUser)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

	sprintf(gcQuery,"SELECT tAuthorize.uPerm,tAuthorize.uCertClient,tAuthorize.uOwner FROM tAuthorize,tClient WHERE tAuthorize.cLabel='%s'",
		cUser);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	mysqlRes=mysql_store_result(&gMysql);
	if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[0],"%d",&guPermLevel);
		sscanf(mysqlField[1],"%u",&guLoginClient);
		sscanf(mysqlField[2],"%u",&guOrg);
	}
	mysql_free_result(mysqlRes);
	
	sprintf(gcQuery,"SELECT tClient.cLabel FROM tClient WHERE tClient.uClient=%u",guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	mysqlRes=mysql_store_result(&gMysql);
	if((mysqlField=mysql_fetch_row(mysqlRes)))
		sprintf(gcName,"%s",mysqlField[0]);
	mysql_free_result(mysqlRes);

	sprintf(gcQuery,"SELECT cLabel FROM tClient WHERE uClient=%u",guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	mysqlRes=mysql_store_result(&gMysql);
	if((mysqlField=mysql_fetch_row(mysqlRes)))
		sprintf(gcOrgName,"%.100s",mysqlField[0]);
	mysql_free_result(mysqlRes);
	
}//void GetPLAndClient()


void EncryptPasswdWithSalt(char *cPassword, char *cSalt)
{
	//Notes:
	//	This should work with both MD5 and DES
	//	as long as it is called with the correct salt
	//
	char cPasswd[102]={""};
	char *cPw;
			
	sprintf(cPasswd,"%.99s",cPassword);
				
	cPw=crypt(cPasswd,cSalt);

	sprintf(cPassword,"%.99s",cPw);

}//void EncryptPasswdWithSalt(char *cPassword, char *cSalt)


int iValidLogin(int mode)
{
	char cSalt[16]={""};
	char cPassword[100]={""};

	//Notes:
	//Mode=1 means we have encrypted passwd from cookie

	sprintf(cPassword,"%.99s",cGetPasswd(gcLogin));

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
					return(1);
		}
	}
	if(!mode)
	{
		sprintf(gcQuery,"INSERT INTO tLog SET cLabel='login failed %.99s',uLogType=7,uPermLevel=%u,uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname);
		mysql_query(&gMysql,gcQuery);
	}
	return(0);

}//iValidLogin()


void SetLogin(void)
{
	if( iValidLogin(0) )
	{
		printf("Set-Cookie: ispAdminLogin=%s;\n",gcLogin);
		printf("Set-Cookie: ispAdminPasswd=%s;\n",gcPasswd);
		sprintf(gcUser,"%.41s",gcLogin);
		guSSLCookieLogin=1;
		GetPLAndClient(gcUser);	
		//tLogType.cLabel='admin login'->uLogType=7
		sprintf(gcQuery,"INSERT INTO tLog SET cLabel='login ok %.99s',uLogType=7,uPermLevel=%u,uLoginClient=%u,cLogin='%.99s',cHost='%.99s',cServer='%.99s',uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",gcLogin,guPermLevel,guLoginClient,gcLogin,gcHost,gcHostname,guOrg);
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

#define BO_CUSTOMER	"Backend Customer"
#define BO_RESELLER	"Backend Reseller"
#define BO_ADMIN 	"Backend Admin"
#define BO_ROOT 	"Backend Root"

#define ORG_CUSTOMER	"Organization Customer"
#define ORG_WEBMASTER	"Organization Webmaster"
#define ORG_SALES	"Organization Sales Force"
#define ORG_SERVICE	"Organization Customer Service"
#define ORG_ACCT	"Organization Bookkeeper"
#define ORG_ADMIN	"Organization Admin"
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
//		if(!isalnum(cInput[i]) && cInput[i]!='.'  && cInput[i]!='-' )
//			break;
		if(isupper(cInput[i])) cInput[i]=tolower(cInput[i]);
	}
	cInput[i]=0;

	return(cInput);

}//char *FQDomainName(char *cInput)


void unxsISPLog(unsigned uTablePK, char *cTableName, char *cLogEntry)
{
        char cQuery[512];

	//uLogType==3 is this admin interface
        sprintf(cQuery,"INSERT INTO tLog SET cLabel='%.63s',uLogType=3,uPermLevel=%u,uLoginClient=%u,cLogin='%.99s',cHost='%.99s',uTablePK='%u',cTableName='%.31s',cHash=MD5(CONCAT('%s','%u','%u','%s','%s','%u','%s','%s')),uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
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
			cLogKey,
			guOrg);

        mysql_query(&gMysql,cQuery);

}//void unxsISPLog(unsigned uTablePK, char *cTableName, char *cLogEntry)


int ReadPullDown(const char *cTableName,const char *cFieldName,const char *cLabel)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

        unsigned int iRowid=0;//Not found

        sprintf(gcQuery,"select _rowid from %s where %s='%s'",
                        cTableName,cFieldName,TextAreaSave((char *) cLabel));
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
        mysqlRes=mysql_store_result(&gMysql);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sscanf(mysqlField[0],"%u",&iRowid);
        mysql_free_result(mysqlRes);
        return(iRowid);

}//ReadPullDown(char *cTableName,char *cLabel)


const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey)
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


void FromMySQLDate(char *cMySQLDate)
{
	unsigned uYear,uMonth,uDay;
	
	sscanf(cMySQLDate,"%u-%u-%u",&uYear,&uMonth,&uDay);
#ifndef SPANISH_DATE
	sprintf(cMySQLDate,"%02u/%02u/%u",uMonth,uDay,uYear);
#else
	sprintf(cMySQLDate,"%02u/%02u/%u",uDay,uMonth,uYear);
#endif

}//void FromMySQLDate(char *cMySQLDate)



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


void EncryptPasswdMD5(char *pw)
{
	char cSalt[] = "$1$01234567$";
        char *cpw;

    	(void)srand((int)time((time_t *)NULL));
    	to64(&cSalt[3],rand(),8);
	
	cpw = crypt(pw,cSalt);
	strcpy(pw,cpw);

}//void EncryptPasswdMD5(char *pw)
//End passwd stuff ;)


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
	if(uOwner) GetClientOwner(uOwner,&guReseller);//Get owner of the owner
	
	
	if( (guPermLevel>11 && uOwner==guOrg) //r3
				|| (guPermLevel>11 && guOrg==guReseller) //r4
				|| (guPermLevel>9 && uCreatedBy==guLoginClient && uOwner==guOrg) //r5
				|| (guPermLevel>11 && guOrg==1) //r1
				|| (guPermLevel>11 && guLoginClient==1) )//r2
			return(1);
	return(0);
}//unsigned uAllowDel(...)


unsigned uAllowMod(const unsigned uOwner, const unsigned uCreatedBy)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);//Get owner of the owner
	if( (guPermLevel>9 && uOwner==guOrg) //r3
				|| (guPermLevel>11 && guOrg==guReseller) //r4
				|| (guPermLevel>6 && uCreatedBy==guLoginClient && uOwner==guOrg) //r5
				|| (guPermLevel>11 && guOrg==1) //r1
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
				" WHERE %3$s.uOwner=" TCLIENT ".uClient"
				" AND (" TCLIENT ".uClient=%2$u OR " TCLIENT ".uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))",
					cVarList,guOrg,cTable);
}//void ExtListSelect(...)


void ExtSelect(const char *cTable,const char *cVarList,unsigned uMaxResults)
{
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %1$s FROM %2$s ORDER BY %2$s._rowid",
					cVarList,cTable);
	else 
		sprintf(gcQuery,"SELECT %1$s FROM %3$s," TCLIENT
				" WHERE %3$s.uOwner=" TCLIENT ".uClient"
				" AND (" TCLIENT ".uClient=%2$u OR " TCLIENT ".uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))"
				" ORDER BY %3$s._rowid",
					cVarList,guOrg,
					cTable);
	if(uMaxResults)
	{
		char cLimit[33]={""};
		sprintf(cLimit," LIMIT %u",uMaxResults);
		strcat(gcQuery,cLimit);
	}

}//void ExtSelect(...)


void ExtSelectSearch(const char *cTable,const char *cVarList,const char *cSearchField,const char *cSearch,const char *cExtraCond,unsigned uMaxResults)
{
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
	{
		if(cExtraCond!=NULL)
			sprintf(gcQuery,"SELECT %1$s FROM %2$s WHERE %3$s LIKE '%4$s%%' AND %5$s ORDER BY %3$s",
						cVarList,cTable,cSearchField,cSearch,cExtraCond);
		else
			sprintf(gcQuery,"SELECT %1$s FROM %2$s WHERE %3$s LIKE '%4$s%%' ORDER BY %3$s",
						cVarList,cTable,cSearchField,cSearch);
	}
	else
	{
		if(cExtraCond!=NULL)
			sprintf(gcQuery,"SELECT %1$s FROM %3$s," TCLIENT
				 	" WHERE %4$s LIKE '%5$s%%' AND %6$s AND %3$s.uOwner=" TCLIENT ".uClient"
					" AND (" TCLIENT ".uClient=%2$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))"
					" ORDER BY %4$s",
						cVarList,guOrg,
						cTable,cSearchField,cSearch,cExtraCond);
		else
			sprintf(gcQuery,"SELECT %1$s FROM %3$s," TCLIENT
				 	" WHERE %4$s LIKE '%5$s%%' AND  %3$s.uOwner=" TCLIENT ".uClient"
					" AND (" TCLIENT ".uClient=%2$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))"
					" ORDER BY %4$s",
						cVarList,guOrg,
						cTable,cSearchField,cSearch);
	}

	if(uMaxResults)
	{
		char cLimit[33]={""};
		sprintf(cLimit," LIMIT %u",uMaxResults);
		strcat(gcQuery,cLimit);
	}

}//void ExtSelectSearch(...)


void ExtSelectRow(const char *cTable,const char *cVarList,unsigned uRow)
{
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM %s WHERE u%s=%u",
					cVarList,cTable,cTable+1,uRow);
	else 
		sprintf(gcQuery,"SELECT %1$s FROM %3$s," TCLIENT
				" WHERE %3$s.uOwner=" TCLIENT ".uClient"
                                " AND %3$s._rowid=%4$u"
				" AND (" TCLIENT ".uClient=%2$u OR " TCLIENT ".uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))",
					cVarList,guOrg,
					cTable,uRow);
}//void ExtSelectRow(...)


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

	GetConfiguration("cCryptMethod",cMethod);
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
	        strcpy(passwd,pw);
    		(void)srand((int)time((time_t *)NULL));
    		to64(&salt[0],rand(),2);
		cpw=crypt(passwd,salt);
	}	
	strcpy(pw,cpw);

}//void EncryptPasswd(char *pw)

