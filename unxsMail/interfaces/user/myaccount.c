/*
FILE 
	myaccount.c
	svn ID removed
AUTHOR
	(C) 2008-2009 Hugo Urquiza for Unixservice
PURPOSE
	unxsMail User Interface Interface
	program file.
	This module handles the account adding request
	and the password reset request.
*/

#include "interface.h"

#define STATUS_PENDING 1
#define STATUS_PENDING_MOD 2

static char cOldPassword[65]={""};
static char cPasswd[65]={""};
static char cConfirmPasswd[65]={""};

//
//Local only functions
void LowerCase(char *cString);
void to64(register char *s, register long v, register int n);
void EncryptPasswd(char *pw);
void UpdatePassword(void);
unsigned ValidateInput(void);
void EncryptPasswdMD5(char *pw);
void EncryptPasswdWithSalt(char *cPasswd,char *cSalt);

void ProcessMyAccountVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cPassword"))
			sprintf(cPasswd,"%.65s",entries[i].val);
		else if(!strcmp(entries[i].name,"cConfirmPassword"))
			sprintf(cConfirmPasswd,"%.65s",entries[i].val);
		else if(!strcmp(entries[i].name,"cOldPassword"))
			sprintf(cOldPassword,"%.65s",entries[i].val);

	}

}//void ProcessMyAccountVars(pentry entries[], int x)


void MyAccountGetHook(entry gentries[],int x)
{
	/*
	register int i;
	
	for(i=0;i<x;i++)
	{
	}

	*/
	htmlMyAccount();
}//void MyAccountGetHook(entry gentries[],int x)


void MyAccountCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"MyAccount"))
	{
		ProcessMyAccountVars(entries,x);
		if(!strcmp(gcFunction,"Update your password"))
		{
			if(!ValidateInput())
				htmlMyAccount();
			UpdatePassword();

			MySubmitJob("ModUser","");
		}
		htmlMyAccount();
	}

}//void MyAccountCommands(pentry entries[], int x)


void htmlMyAccount(void)
{
	htmlHeader("unxsMail User Interface","Header");
	htmlMyAccountPage("unxsMail User Interface","MyAccount.Body");
	htmlFooter("Footer");

}//void htmlMyAccount(void)


void htmlMyAccountPage(char *cTitle, char *cTemplateName)
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
			template.cpValue[1]="unxsMailUser.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcUser;

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

			template.cpName[8]="";

			printf("\n<!-- Start htmlMyAccountPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlMyAccountPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlMyAccountPage()


unsigned ValidateInput(void)
{
	if(!cOldPassword[0])
	{
		gcMessage="<blink>Error: </blink>You must enter your current password";
		return(0);
	}
	else
	{
		MYSQL_RES *res;
		MYSQL_ROW field;

		sprintf(gcQuery,"SELECT cPasswd FROM tUser WHERE uUser=%u",guLoginClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			char cSalt[16]={""};
			char cOldPasswordSave[100]={""};

			sprintf(cSalt,"%.12s",field[0]);
			sprintf(cOldPasswordSave,"%.99s",cOldPassword);
			EncryptPasswdWithSalt(cOldPasswordSave,cSalt);

			if(strcmp(cOldPasswordSave,field[0]))
			{
				gcMessage="<blink>Error: </blink>Your current password isn't valid";
				return(0);
			}

		}
	}
	if(!cPasswd[0])
	{
		gcMessage="<blink>Error: </blink>You must enter a password";
		return(0);
	}
	if(!cConfirmPasswd[0])
	{
		gcMessage="<blink>Error: </blink>You must confirm your password";
		return(0);
	}
	if(strcmp(cPasswd,cConfirmPasswd))
	{
		gcMessage="<blink>Error: </blink>The entered passwords do not match";
		return(0);
	}
	if(strlen(cPasswd)<5)
	{
		gcMessage="<blink>Error: </blink>Password must have 5 characters as minimum length";
		return(0);
	}
		
	return(1);
	
	
}//unsigned ValidateInput(void)


unsigned Update_tUser(char *cPasswd)
{
	sprintf(gcQuery,"UPDATE tUser SET cPasswd='%s',uStatus=%u,uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uUser=%u",
			cPasswd
			,STATUS_PENDING_MOD
			,guLoginClient);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	return((unsigned)mysql_affected_rows(&gMysql));

}//unsigned Update_tUser(unsigned uUser,char *cPasswd)


void LowerCase(char *cString)
{
	register int iX;

	for(iX=0;iX<strlen(cString);iX++)
		cString[iX]=tolower(cString[iX]);	

}//void LowerCase(char *cString)


void UpdatePassword(void)
{
	EncryptPasswdMD5(cPasswd);
	if(Update_tUser(cPasswd))
		gcMessage="Your account password was updated sucessfully. You need to re-login";
	else
		gcMessage="Your account password could not be updated. Contact support ASAP";


}//void UpdatePassword(void)

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


void MySubmitJob(char *cJobName,char *cJobData)
{
	char cDomain[100]={""};
	char cServerGroup[100]={""};
	unsigned uDomain=0;
	unsigned uServerGroup=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uDomain,uServerGroup FROM tUser WHERE uUser=%u",guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uDomain);
		sscanf(field[1],"%u",&uServerGroup);
	}
	mysql_free_result(res);

	sprintf(cDomain,"%s",ForeignKey("tDomain","cDomain",uDomain));
	sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
	SubmitJob(cJobName,cDomain,gcLogin,cServerGroup,cJobData,uDomain,guLoginClient,1,1);

}//void MySubmitJob(char *cJobName)

