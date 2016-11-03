/*
FILE 
	ticket.c
	svn ID removed
AUTHOR
	(C) 2007-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	ispHelp  Interface
	program file.
*/


#include "interface.h"


static unsigned uClient=0;

static char cFirstName[33]={""};
static char *cFirstNameStyle="type_fields";

static char cLastName[33]={""};
static char *cLastNameStyle="type_fields";

static char cEmail[101]={""};
static char *cEmailStyle="type_fields";

static char cTelephone[33]={""};
static char *cTelephoneStyle="type_fields";

static char cMobile[33]={""};
static char *cMobileStyle="type_fields";

static char cFax[33]={""};
static char *cFaxStyle="type_fields";

static char cSavePasswd[10]={""};


unsigned ValidRegisterInput(void);
void CommitRegister(void);
void EmailConfirmation(void);
void ShowSuccessPage(void);
void AuthorizeRegister(char *cConfirmHash);
void ShowConfirmedRegistration(void);


void ProcessRegisterVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cFirstName"))
			sprintf(cFirstName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLastName"))
			sprintf(cLastName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cEmail"))
			sprintf(cEmail,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cTelephone"))
			sprintf(cTelephone,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cMobile"))
			sprintf(cMobile,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cFax"))
			sprintf(cFax,"%.32s",entries[i].val);
	}

}//void ProcessUserVars(pentry entries[], int x)


void RegisterGetHook(entry gentries[],int x)
{
	register int i;
	char cConfirmHash[65]={""};

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"cConfirmHash"))
			sprintf(cConfirmHash,"%.64s",gentries[i].val);
	}
	
	if(cConfirmHash[0])
	{
		AuthorizeRegister(cConfirmHash);
		ShowConfirmedRegistration();
	}
	htmlRegister();

}//void RegisterGetHook(entry gentries[],int x)


void RegisterCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Register"))
	{
		ProcessRegisterVars(entries,x);
		if(!strcmp(gcFunction,"Register"))
		{
			if(ValidRegisterInput())
				htmlRegister();

			CommitRegister();
			EmailConfirmation();
			ShowSuccessPage();
		}

		htmlRegister();
	}

}//void RegisterCommands(pentry entries[], int x)


void htmlRegister(void)
{
	htmlHeader("unxsISP CRM","Header");
	htmlRegisterPage("","Register.Body");
	htmlFooter("Footer");

}//void htmlRegister(void)


void htmlRegisterPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;
		
		TemplateSelect(cTemplateName,guTemplateSet);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="ispHelp.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcLogin;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcMessage";
			template.cpValue[7]=gcMessage;
			
			template.cpName[8]="cFirstName";
			template.cpValue[8]=cFirstName;

			template.cpName[9]="cFirstNameStyle";
			template.cpValue[9]=cFirstNameStyle;

			template.cpName[10]="cLastName";
			template.cpValue[10]=cLastName;

			template.cpName[11]="cLastNameStyle";
			template.cpValue[11]=cLastNameStyle;

			template.cpName[12]="cEmail";
			template.cpValue[12]=cEmail;

			template.cpName[13]="cEmailStyle";
			template.cpValue[13]=cEmailStyle;

			template.cpName[14]="cTelephone";
			template.cpValue[14]=cTelephone;

			template.cpName[15]="cMobile";
			template.cpValue[15]=cMobile;

			template.cpName[16]="cMobileStyle";
			template.cpValue[16]=cMobileStyle;

			template.cpName[17]="cFax";
			template.cpValue[17]=cFax;

			template.cpName[18]="cFaxStyle";
			template.cpValue[18]=cFaxStyle;

			template.cpName[19]="cTelephoneStyle";
			template.cpValue[19]=cTelephoneStyle;

			template.cpName[20]="";

			printf("\n<!-- Start htmlRegisterPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlRegisterPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlRegisterPage()


unsigned ValidRegisterInput(void)
{
	MYSQL_RES *res;

	if(!cFirstName[0])
	{
		cFirstNameStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter first name";
		return(1);
	}
	if(!cLastName[0])
	{
		cLastNameStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter last name";
		return(1);
	}

	if(!cEmail[0])
	{
		cEmailStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter email address";
		return(1);
	}
	else
	{
		if(strstr(cEmail,"@")==NULL || strstr(cEmail,".")==NULL)
		{
			cEmailStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Email has to be a valid email address";
			return(1);
		}
	}

	if(!cTelephone[0])
	{
		cTelephoneStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter telephone number";
		return(1);
	}

	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cFirstName='%s' AND cLastName='%s'",
			TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if(mysql_num_rows(res))
	{
		cFirstNameStyle="type_fields_req";
		cLastNameStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Name already registered in the database";
		return(1);
	}

	mysql_free_result(res);

	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cEmail='%s'",TextAreaSave(cEmail));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if(mysql_num_rows(res))
	{
		cEmailStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Email already registered in the database";
		return(1);
	}

	mysql_free_result(res);

	return(0);

}//unsigned ValidRegisterInput(void)


void CommitRegister(void)
{
#define DEFAULT_OWNER 1 //Later from tConfiguration!
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tRegister "
			"(uRegister INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, "
			"uClient INT UNSIGNED NOT NULL DEFAULT 0,"
			"cHash VARCHAR(32) NOT NULL DEFAULT '',"
			"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0)");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	sprintf(gcQuery,"INSERT INTO tClient SET cLabel='%s %s',cFirstName='%s',"
			" cLastName='%s',cEmail='%s',cTelephone='%s',"
			" cMobile='%s',cFax='%s',uOwner=%u,uCreatedBy=1,"
			"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,
			TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cEmail)
			,TextAreaSave(cTelephone)
			,TextAreaSave(cMobile)
			,TextAreaSave(cFax)
			,DEFAULT_OWNER
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uClient=mysql_insert_id(&gMysql);

	sprintf(gcQuery,"INSERT INTO tRegister SET uClient=%u,cHash=MD5(CONCAT('%s','%s','%s','%u')),"
			"uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uClient
			,TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cEmail)
			,uClient
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void CommitRegister(void)


void EmailConfirmation(void)
{
	FILE *fp;
	MYSQL_RES *res;
	MYSQL_ROW field;
	struct t_template template;
	char cFrom[256]={"root"};
	
	GetConfiguration("cRegisterMailFrom",cFrom);

	sprintf(gcQuery,"SELECT cHash FROM tRegister WHERE uClient=%u",uClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	field=mysql_fetch_row(res); //Will always have data

	template.cpName[0]="cFirstName";
	template.cpValue[0]=cFirstName;

	template.cpName[1]="cHash";
	template.cpValue[1]=field[0];

	template.cpName[2]="";

	//debug only
	//if((fp=fopen("/tmp/eMailInvoice","w")))
	if((fp=popen("/usr/lib/sendmail -t > /dev/null","w")))
	{
		fprintf(fp,"To: %s\n",cEmail);
		fprintf(fp,"From: %s\n",cFrom);
		fprintf(fp, "Reply-to: %s\n",cFrom);
		fprintf(fp,"Subject: Please confirm your registration\n");
		fprintf(fp,"MIME-Version: 1.0\n");
		fprintf(fp,"Content-type: text/plain\n\n");
		fpTemplate(fp,"RegisterEmail",&template);	
		//fclose(fp);
		pclose(fp);
	}	

}//void EmailConfirmation(void)


void ShowSuccessPage(void)
{
	htmlHeader("unxsISP CRM","Header");
	htmlRegisterPage("","RegisterOK.Body");
	htmlFooter("Footer");

}//void ShowSuccessPage(void)

char *cGetRandomPassword(void);
void EncryptPasswd(char *cPasswd);
void EmailLogin(void);

void AuthorizeRegister(char *cConfirmHash)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cPasswd[10]={""};

	sprintf(gcQuery,"SELECT uClient,cFirstName,cLastName,cEmail FROM tClient WHERE uClient IN "
			"(SELECT uClient FROM tRegister WHERE cHash='%s')",cConfirmHash);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uClient);
		sprintf(cFirstName,"%s",field[1]);
		sprintf(cLastName,"%s",field[2]);
		sprintf(cEmail,"%s",field[3]);
	}
	else
	{
		printf("Content-type: text/plain\n\n");
		printf("Invalid registration hash\n");
		exit(0);
	}

	sprintf(cPasswd,"%s",cGetRandomPassword());
	sprintf(cSavePasswd,"%s",cPasswd);
	EncryptPasswd(cPasswd);
	sprintf(gcQuery,"INSERT INTO tAuthorize SET cLabel='%s %s',uCertClient=%u,"
			"uOwner=%u,cPasswd='%s',cIpMask='0.0.0.0',"
			"uPerm=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			cFirstName
			,cLastName
			,uClient
			,DEFAULT_OWNER
			,cPasswd
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	EmailLogin();

	sprintf(gcQuery,"DELETE FROM tRegister WHERE cHash='%s'",cConfirmHash);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void AuthorizeRegister(char *cConfirmHash)


void ShowConfirmedRegistration(void)
{
	htmlHeader("unxsISP CRM","Header");
	htmlRegisterPage("","RegisterConf.Body");
	htmlFooter("Footer");

}//void ShowConfirmedRegistration(void)


char *cGetRandomPassword(void)
{
	static char cPasswd[10]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;

	mysql_query(&gMysql,"DROP function if exists generate_alpha");
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	mysql_query(&gMysql,"CREATE FUNCTION generate_alpha () RETURNS CHAR(1) "
			"RETURN ELT(FLOOR(1 + (RAND() * (50-1))), 'a','b','c','d'"
			",'e','f','g','h','i','j','k','l','m  ','n','o','p','q','r'"
			",'s','t','u','v','w','x','y',  'z','A','B','C','D','E','F',"
			"'G','H','I','J','K','L','M  ','N','O','P','Q','R','S','T','U'"
			",'V','W','X','Y',  'Z' )");
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	mysql_query(&gMysql,"SELECT CONCAT(generate_alpha (),generate_alpha (),generate_alpha (),"
			"generate_alpha (),generate_alpha (),generate_alpha (),generate_alpha (),"
			"generate_alpha ())");
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	field=mysql_fetch_row(res); //We will always have a row if the above queries didn't fail
	
	sprintf(cPasswd,"%s",field[0]);

	return(cPasswd);

}//char *cGetRandomPassword(void)


void EmailLogin(void)
{
	FILE *fp;
	struct t_template template;
	char cFrom[256]={"root"};
	
	GetConfiguration("cRegisterMailFrom",cFrom);

	template.cpName[0]="cFirstName";
	template.cpValue[0]=cFirstName;

	template.cpName[1]="cLastName";
	template.cpValue[1]=cLastName;

	template.cpName[2]="cPasswd";
	template.cpValue[2]=cSavePasswd;

	template.cpName[3]="";

	//debug only
	//if((fp=fopen("/tmp/eMailInvoice","w")))
	if((fp=popen("/usr/lib/sendmail -t > /dev/null","w")))
	{
		fprintf(fp,"To: %s\n",cEmail);
		fprintf(fp,"From: %s\n",cFrom);
		fprintf(fp, "Reply-to: %s\n",cFrom);
		fprintf(fp,"Subject: Registration confirmed\n");
		fprintf(fp,"MIME-Version: 1.0\n");
		fprintf(fp,"Content-type: text/plain\n\n");
		fpTemplate(fp,"LoginEmail",&template);	
		//fclose(fp);
		pclose(fp);
	}	

}//void EmailLogin(void)
