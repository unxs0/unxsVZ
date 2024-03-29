/*
FILE 
	user.c
	svn ID removed
AUTHOR/LEGAL
	(C) 2010, 2011 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	unxsvzOrg program file.
	User tab functions.
*/

#include "interface.h"

extern unsigned guBrowserFirefox;//main.c
extern char gcCtHostname[];
static char cCurPasswd[32]={""};
static char cPasswd[32]={""};
static char cPasswd2[32]={""};

//TOC
void ProcessUserVars(pentry entries[], int x);
void UserGetHook(entry gentries[],int x);
unsigned uNoUpper(const char *cPasswd);
unsigned uNoDigit(const char *cPasswd);
unsigned uNoLower(const char *cPasswd);
unsigned uChangePassword(const char *cPasswd);
void EncryptPasswdWithSalt(char *pw, char *salt);
char *cGetPasswd(char *gcLogin);
unsigned uValidLoginPasswd(const char *cPasswd);
extern unsigned guContainer;


void ProcessUserVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cCurPasswd"))
			sprintf(cCurPasswd,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cPasswd"))
			sprintf(cPasswd,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cPasswd2"))
			sprintf(cPasswd2,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"gcCtHostname"))
			sprintf(gcCtHostname,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"guContainer"))
			sscanf(entries[i].val,"%u",&guContainer);
	}

}//void ProcessUserVars(pentry entries[], int x)


void UserGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"guContainer"))
			sscanf(gentries[i].val,"%u",&guContainer);
	}

	htmlUser();

}//void UserGetHook(entry gentries[],int x)


unsigned uNoUpper(const char *cPasswd)
{
	register int i;
	for(i=0;cPasswd[i];i++)
		if(isupper(cPasswd[i])) return(0);
	return(1);
}//unsigned uNoUpper(const char *cPasswd)


unsigned uNoLower(const char *cPasswd)
{
	register int i;
	for(i=0;cPasswd[i];i++)
		if(islower(cPasswd[i])) return(0);
	return(1);
}//unsigned uNoLower(const char *cPasswd)


unsigned uNoDigit(const char *cPasswd)
{
	register int i;
	for(i=0;cPasswd[i];i++)
		if(isdigit(cPasswd[i])) return(0);
	return(1);
}//unsigned uNoDigit(const char *cPasswd)


unsigned uChangePassword(const char *cPasswd)
{
	char cBuffer[100]={""};
	char cSalt[16]={"$1$23abc123$"};//TODO set to random salt;

	sprintf(cBuffer,"%.99s",cPasswd);
	EncryptPasswdWithSalt(cBuffer,cSalt);

	sprintf(gcQuery,"UPDATE tAuthorize SET cPasswd='%.99s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE cLabel='%s'",
			cBuffer,guLoginClient,gcLogin);
	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		return(1);
	if(mysql_affected_rows(&gMysql)<1)
		return(1);
	return(0);
}//unsigned uChangePassword(const char *cPasswd)


unsigned uValidLoginPasswd(const char *cPasswd)
{
	char cSalt[16]={""};
	char cPassword[100]={""};
	char cBuffer[100]={""};

	sprintf(cPassword,"%.99s",cGetPasswd(gcLogin));
	sprintf(cBuffer,"%.99s",cPasswd);
	if(cPassword[0])
	{
		//MD5 vs DES salt determination
		if(cPassword[0]=='$' && cPassword[2]=='$')
			sprintf(cSalt,"%.12s",cPassword);
		else
			sprintf(cSalt,"%.2s",cPassword);
		EncryptPasswdWithSalt(cBuffer,cSalt);
		if(!strcmp(cBuffer,cPassword))
				return 1;
	}

	return 0;

}//unsigned uValidLoginPasswd(const char *cPasswd)


void UserCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"User"))
	{
		ProcessUserVars(entries,x);
		if(!strcmp(gcFunction,"Change Password"))
		{
			if(!cCurPasswd[0])
			{
				gcMessage="<blink>Error</blink>: Must enter 'Current Password'";
				htmlUser();
			}
			if(!cPasswd[0] || !cPasswd2[0] || strcmp(cPasswd,cPasswd2))
			{
				gcMessage="<blink>Error</blink>: Must enter new 'Password' twice and they must match";
				htmlUser();
			}
			if(!strcmp(cCurPasswd,cPasswd))
			{
				gcMessage="<blink>Error</blink>: New 'Password' is the same as current password";
				htmlUser();
			}
			if(strlen(cPasswd)<6)
			{
				gcMessage="<blink>Error</blink>: New 'Password' must be at least 6 chars long";
				htmlUser();
			}
			if(strstr(cPasswd,gcLogin) || strstr(gcLogin,cPasswd) ||
				strstr(cPasswd,gcName) || strstr(gcName,cPasswd) ||
				strstr(gcOrgName,cPasswd) || strstr(cPasswd,gcOrgName))
			{
				gcMessage="<blink>Error</blink>: New 'Password' must not be related to your login or company";
				htmlUser();
			}
			if(uNoUpper(cPasswd) || uNoLower(cPasswd) || uNoDigit(cPasswd))
			{
				gcMessage="<blink>Error</blink>: New 'Password' must have some upper and lower case letters,"
						" and at least one number";
				htmlUser();
			}
			if(!uValidLoginPasswd(cCurPasswd))
			{
				gcMessage="<blink>Error</blink>: Current Password is not correct";
				htmlUser();
			}
			if(uChangePassword(cPasswd))
			{
				gcMessage="<blink>Error</blink>: Password not changed contact system admin";
				htmlUser();
			}
			gcMessage="Password changed you will need to login again";
			htmlUser();
		}
	}

}//void UserCommands(pentry entries[], int x)


void htmlUser(void)
{
	htmlHeader("unxsvzOrg","Header");
	htmlUserPage("unxsvzOrg","User.Body");
	htmlFooter("Footer");

}//void htmlUser(void)


void htmlUserPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uPLAINSET,uVDNSORGTYPE);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="unxsvzOrg.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcUser;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);//Safe?

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcMessage";
			template.cpValue[7]=gcMessage;

			template.cpName[8]="gcCtHostname";
			template.cpValue[8]=gcCtHostname;

			char cguContainer[16];
			sprintf(cguContainer,"%u",guContainer);
			template.cpName[9]="guContainer";
			template.cpValue[9]=cguContainer;

			template.cpName[10]="";

//debug only
//printf("Content-type: text/html\n\n");
//printf("d6 %s",gcUser);
//exit(0);
	
			printf("\n<!-- Start htmlUserPage(%s) -->\n",cTemplateName); 
			Template(field[0],&template,stdout);
			printf("\n<!-- End htmlUserPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlUserPage()


void funcMOTD(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcMOTD(fp) Start -->\n");

	sprintf(gcQuery,"SELECT cComment FROM tConfiguration WHERE uDatacenter=0 AND uNode=0 AND uContainer=0"
			" AND cLabel='cOrg_MOTD' LIMIT 1");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		fprintf(fp,"<tr><td valign=top ><strong><u>Message of the Day</u></strong></td><td><br>%s</td></tr>",field[0]);
	mysql_free_result(res);

	fprintf(fp,"<!-- funcMOTD(fp) End -->\n");

}//void funcMOTD(FILE *fp)
