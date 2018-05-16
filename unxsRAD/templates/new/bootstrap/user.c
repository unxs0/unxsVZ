/*
FILE 
	{{cProject}}/interfaces/bootstrap/main.c
	template/new/bootstrap/main.c
AUTHOR/LEGAL
	(C) 2010-2018 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	bootstrap user default home page code
*/

#include "interface.h"

extern unsigned guBrowserFirefox;//main.c
extern char gcCtHostname[];
static char cCurPasswd[32]={""};
static char cPasswd[32]={""};
static char cPasswd2[32]={""};
static char *gcFiveIn="out";

//TOC
void ProcessUserVars(pentry entries[], int x);
void UserGetHook(entry gentries[],int x);
unsigned uNoUpper(const char *cPasswd);
unsigned uNoDigit(const char *cPasswd);
unsigned uNoLower(const char *cPasswd);
unsigned uChangePassword(const char *cPasswd);
void EncryptPasswdWithSalt(char *pw, char *salt);
char *cGetPasswd(char *gcLogin);
unsigned uValidPasswd(char *cPasswd,unsigned guLoginClient);
//extern
void unxsvzLog(unsigned uTablePK,char *cTableName,char *cLogEntry,unsigned guPermLevel,unsigned guLoginClient,char *gcLogin,char *gcHost);
void htmlOperationsInfo(void);
void htmlLoginInfo(void);

static unsigned uDay=0;


void SendEmail(char *cMsg,char *cMailTo,char *cFrom,char *cSubject,char *cBcc)
{
	FILE *pp;
	pid_t pidChild;

	pidChild=fork();
	if(pidChild!=0)
		return;

	fclose(stdout);
	pp=popen("/usr/lib/sendmail -t","w");
	if(pp==NULL)
		return;
			
	fprintf(pp,"To: %s\n",cMailTo);
	if(cBcc[0]) fprintf(pp,"Bcc: %s\n",cBcc);
	fprintf(pp,"From: %s\n",cFrom);
	fprintf(pp,"Subject: %s\n",cSubject);

	fprintf(pp,"%s\n",cMsg);

	fprintf(pp,".\n");

	pclose(pp);

}//void SendEmail()


void ProcessCalendarVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"guMonth"))
			sscanf(entries[i].val,"%u",&guMonth);
		else if(!strcmp(entries[i].name,"guYear"))
			sscanf(entries[i].val,"%u",&guYear);
		else if(!strcmp(entries[i].name,"uDay"))
			sscanf(entries[i].val,"%u",&uDay);
	}

}//void ProcessCalendarVars(pentry entries[], int x)


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
	}

}//void ProcessUserVars(pentry entries[], int x)


void ToggleAvailableDay(unsigned uYear,unsigned uMonth,unsigned uDay)
{
        MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT uCalendar FROM tCalendar"
				" WHERE (uVendor=%u OR uVendor=%u) AND dDate='%u-%u-%u'",guLoginClient,guOrg,uYear,uMonth,uDay);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return;
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"DELETE FROM tCalendar WHERE uCalendar=%s",field[0]);
		mysql_query(&gMysql,gcQuery);
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tCalendar"
				" SET uVendor=%u,uOwner=%u,dDate='%u-%u-%u',"
				"uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=%u",guLoginClient,guOrg,uYear,uMonth,uDay,guLoginClient);
		mysql_query(&gMysql,gcQuery);
	}
	mysql_free_result(res);
}//


void CalendarGetHook(entry gentries[],int x)
{
	register int i;
	unsigned uDay=0;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uMonth"))
			sscanf(gentries[i].val,"%u",&guMonth);
		else if(!strcmp(gentries[i].name,"uYear"))
			sscanf(gentries[i].val,"%u",&guYear);
		else if(!strcmp(gentries[i].name,"uDay"))
			sscanf(gentries[i].val,"%u",&uDay);
	}

	if(!strcmp(gcFunction,"ToggleDay")&&uDay&&guMonth&&guYear)
		ToggleAvailableDay(guYear,guMonth,uDay);

	htmlCalendar();

}//void UserGetHook(entry gentries[],int x)


void UserGetHook(entry gentries[],int x)
{
	if(!strcmp(gcFunction,"LoginInfo"))
		htmlLoginInfo();
	if(!strcmp(gcFunction,"LoginInfo"))
		htmlLoginInfo();

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


//Passwd stuff
static unsigned char itoa64[] =         /* 0 ... 63 => ascii - 64 */
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void to64(s, v, n)
  register char *s;
  register long long v;
  register int n;
{
    while (--n >= 0) {
        *s++ = itoa64[v&0x3f];
        v >>= 6;
    }
}//void to64(s, v, n)


unsigned uChangePassword(const char *cPasswd)
{
	char cBuffer[100]={""};
	char cSalt[16]={"$1$23abc123$"};//TODO set to random salt;
	(void)srand((long long)time((time_t *)NULL)*getpid());
	to64(&cSalt[3],rand(),8);

	sprintf(cBuffer,"%.99s",cPasswd);
	EncryptPasswdWithSalt(cBuffer,cSalt);

	//Here we allow private passwords
	sprintf(gcQuery,"UPDATE tAuthorize SET cIpMask='',cPasswd='%.99s',cClrPasswd='',"
			"uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE cLabel='%s'",
			cBuffer,guLoginClient,gcLogin);
	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		unxsvzLog(guLoginClient,"tAuthorize","Password Not Changed Error1",guPermLevel,guLoginClient,gcLogin,gcHost);
		return(1);
	}
	if(mysql_affected_rows(&gMysql)<1)
	{
		unxsvzLog(guLoginClient,"tAuthorize","Password Not Changed Error2",guPermLevel,guLoginClient,gcLogin,gcHost);
		return(1);
	}
	unxsvzLog(guLoginClient,"tAuthorize","Password Changed",guPermLevel,guLoginClient,gcLogin,gcHost);
	return(0);
}//unsigned uChangePassword(const char *cPasswd)


//CalendarCommands
void UserCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Calendar"))
	{
		ProcessCalendarVars(entries,x);
		if(!strcmp(gcFunction,"ToggleDay"))
		{
			if(guYear && guMonth && uDay);
			ToggleAvailableDay(guYear,guMonth,uDay);
			htmlCalendar();
		}
	}
	else if(!strcmp(gcPage,"User"))
	{
		ProcessUserVars(entries,x);
		if(!strcmp(gcFunction,"Change Password") || !strcmp(gcFunction,"ChangePassword"))
		{
			if(guPermLevel>10)
			{
				gcFiveIn="in";
				gcMessage="Error: Admin users not allowed to change password here";
				htmlUser();
			}
			if(!cCurPasswd[0])
			{
				gcFiveIn="in";
				gcMessage="Error: Must enter 'Current Password'";
				htmlUser();
			}
			if(!uValidPasswd(cCurPasswd,guLoginClient))
			{
				gcFiveIn="in";
				gcMessage="Error: Invalid current password entered";
				htmlUser();
			}
			if(!cPasswd[0] || !cPasswd2[0] || strcmp(cPasswd,cPasswd2))
			{
				gcFiveIn="in";
				gcMessage="Error: Must enter new 'Password' twice and they must match";
				htmlUser();
			}
			if(!strcmp(cCurPasswd,cPasswd))
			{
				gcFiveIn="in";
				gcMessage="Error: New 'Password' is the same as current password";
				htmlUser();
			}
			if(strlen(cPasswd)<8)
			{
				gcFiveIn="in";
				gcMessage="Error: New 'Password' must be at least 8 chars long";
				htmlUser();
			}
			if(strstr(cPasswd,gcLogin) || strstr(gcLogin,cPasswd) ||
				strstr(cPasswd,gcName) || strstr(gcName,cPasswd) ||
				strstr(gcOrgName,cPasswd) || strstr(cPasswd,gcOrgName))
			{
				gcFiveIn="in";
				gcMessage="Error: New 'Password' must not be related to your login or company";
				htmlUser();
			}
			if(uNoUpper(cPasswd) || uNoLower(cPasswd) || uNoDigit(cPasswd))
			{
				gcFiveIn="in";
				gcMessage="Error: New 'Password' must have some upper and lower case letters,"
						" and at least one number";
				htmlUser();
			}
			if(uChangePassword(cPasswd))
			{
				gcFiveIn="in";
				gcMessage="Error: Password not changed contact system admin";
				htmlUser();
			}
			gcMessage="Password changed you will need to login again";
			htmlUser();
		}
	}

}//void UserCommands(pentry entries[], int x)


void htmlSignUp(void)
{
	
	htmlHeader("Sign Up","Default.Header");
	htmlUserPage("Sign Up","SignUp.Body");
	htmlFooter("Default.Footer");

}//void htmlSignUp(void)


void NewCodeAndLinkEmail(unsigned uAuthorize,char *cEmail,char *cServer)
{
	char cMsg[512]={""};
	char cEmailCode[32]={""};

	(void)srand((long long)time((time_t *)NULL)*getpid()+77);
	register int i;
	for(i=0;i<16;i++)
		sprintf(cEmailCode+i,"%x",rand()%16);

	sprintf(gcQuery,"UPDATE tAuthorize SET cIpMask='%.20s',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uAuthorize=%u",cEmailCode,uAuthorize);
	mysql_query(&gMysql,gcQuery);

	sprintf(cMsg,"The link to change your password is:\n"
		"https://%s/{{cProject}}App/?gcFunction=ChangePassword&gcEmailCode=%.16s&gcLogin=%s\n",
				cServer,cEmailCode,cEmail);

	SendEmail(cMsg,cEmail,"unxsak@unxs.io","unxsAK New Account Confirmation","unxsak@unxs.io");

}//void NewCodeAndLinkEmail()


void NewCodeAndEmail(unsigned uAuthorize,char *cEmail)
{
	char cMsg[256]={""};
	char cEmailCode[32]={""};

	(void)srand((long long)time((time_t *)NULL)*getpid()+77);
	register int i;
	for(i=0;i<16;i++)
		sprintf(cEmailCode+i,"%x",rand()%16);

	sprintf(gcQuery,"UPDATE tAuthorize SET cIpMask='%.20s',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uAuthorize=%u",cEmailCode,uAuthorize);
	mysql_query(&gMysql,gcQuery);

	sprintf(cMsg,"The activation code is: %.16s\n",cEmailCode);

	SendEmail(cMsg,cEmail,"unxsak@unxs.io","unxsAK New Account Confirmation","unxsak@unxs.io");

}//void NewCodeAndEMail(unsigned uAuthorize,char *cEmail)


void htmlSignUpStep1(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPerm=0;
	unsigned uAuthorize=0;

	sprintf(gcQuery,"SELECT uAuthorize,uPerm FROM tAuthorize"
			" WHERE cLabel='%s' LIMIT 1",TextAreaSave(gcLogin));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unknown error try again later";
		htmlHeader("Sign Up","Default.Header");
		htmlUserPage("Sign Up","SignUp.Body");
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uAuthorize);
		sscanf(field[1],"%u",&uPerm);
	}
	mysql_free_result(res);

	if(uAuthorize && uPerm)
	{
		sleep(3);
		gcMessage="That user is already signed up and active.";
		htmlHeader("Sign Up","Default.Header");
		htmlUserPage("Sign Up","SignUp.Body");
		htmlFooter("Default.Footer");
	}
	else if(uAuthorize)
	{
		//sleep(3);
		NewCodeAndEmail(uAuthorize,TextAreaSave(gcLogin));
		htmlHeader("Sign Up","Default.Header");
		gcMessage="Login email is waiting code confirmation, another email with new code was sent.";
		htmlUserPage("Sign Up","SignUpStep1.Body");
		htmlFooter("Default.Footer");
	}
	else
	{
		if(!cPasswd[0] || !cPasswd2[0] || strcmp(cPasswd,cPasswd2))
		{
			gcMessage="Both passwords must match";
			htmlHeader("Sign Up","Default.Header");
			htmlUserPage("Sign Up","SignUp.Body");
			htmlFooter("Default.Footer");//Footer exits
		}
		if(strlen(cPasswd)<8)
		{
			gcMessage="Password must be at least 8 characters long";
			htmlHeader("Sign Up","Default.Header");
			htmlUserPage("Sign Up","SignUp.Body");
			htmlFooter("Default.Footer");//Footer exits
		}
		if(uNoUpper(cPasswd) || uNoLower(cPasswd) || uNoDigit(cPasswd))
		{
			gcMessage="Password have upper and lower case letters and at least one number";
			htmlHeader("Sign Up","Default.Header");
			htmlUserPage("Sign Up","SignUp.Body");
			htmlFooter("Default.Footer");//Footer exits
		}

		char cBuffer[100]={""};
		char cSalt[16]={"$1$23abc123$"};//TODO set to random salt;
		(void)srand((long long)time((time_t *)NULL)*getpid());
		to64(&cSalt[3],rand(),8);

		sprintf(cBuffer,"%.99s",cPasswd);
		EncryptPasswdWithSalt(cBuffer,cSalt);

		sprintf(gcQuery,"INSERT INTO tClient SET"
			" uOwner=4,"//end user
			" uCreatedBy=1,"
			" uCreatedDate=UNIX_TIMESTAMP(NOW()),"
			" cEmail='%1$.31s',"
			" cLabel='%1$.31s'"
				,TextAreaSave(gcLogin));
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			gcMessage="Unknown error try again later";
			htmlHeader("Sign Up","Default.Header");
			htmlUserPage("Sign Up","SignUp.Body");
			htmlFooter("Default.Footer");//Footer exits
		}

		unsigned uClient=mysql_insert_id(&gMysql);
		sprintf(gcQuery,"INSERT INTO tAuthorize SET"
			" cIpMask='',"
			" uPerm=0,"//waiting for activation
			" uCertClient=%u,"
			" cClrPasswd='%s',"
			" cPasswd='%s',"
			" uOwner=4,"//end user
			" uCreatedBy=1,"
			" uCreatedDate=UNIX_TIMESTAMP(NOW()),"
			" cLabel='%s'"
				,uClient
				,cPasswd
				,cBuffer
				,TextAreaSave(gcLogin));
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			gcMessage="Unknown error try again later";
			htmlHeader("Sign Up","Default.Header");
			htmlUserPage("Sign Up","SignUp.Body");
			htmlFooter("Default.Footer");
		}
		uAuthorize=mysql_insert_id(&gMysql);
		NewCodeAndEmail(uAuthorize,TextAreaSave(gcLogin));
		htmlHeader("Sign Up","Default.Header");
		htmlUserPage("Sign Up","SignUpStep1.Body");
		htmlFooter("Default.Footer");
	}

}//void htmlSignUpStep1(void)


void htmlSignUpDone(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPerm=0;
	unsigned uAuthorize=0;

	sprintf(gcQuery,"SELECT uAuthorize,uPerm FROM tAuthorize"
			" WHERE cLabel='%s' AND cIpMask='%s' AND uPerm=0 LIMIT 1",TextAreaSave(gcLogin),TextAreaSave(gcEmailCode));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql) || strlen(gcEmailCode)!=16)
	{
		gcMessage="Unexpected error try again later";
		htmlHeader("Sign Up","Default.Header");
		htmlUserPage("Sign Up","SignUpStep1.Body");
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uAuthorize);
		sscanf(field[1],"%u",&uPerm);
	}
	mysql_free_result(res);

	//check code
	//if valid update tAuthorize.uPerm
	if(!uAuthorize)
	{
		gcMessage="Code is wrong, or user does not exist, or user has already been activated.";
		htmlHeader("Sign Up","Default.Header");
		htmlUserPage("Sign Up","SignUpStep1.Body");
		htmlFooter("Default.Footer");
	}

	sprintf(gcQuery,"UPDATE tAuthorize SET uPerm=7,cIpMask='',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE uAuthorize=%u",uAuthorize);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unknown error try again later";
		htmlHeader("Sign Up","Default.Header");
		htmlUserPage("Sign Up","SignUpStep1.Body");
		htmlFooter("Default.Footer");
	}

	gcMessage="You can login now.";
	htmlHeader("Sign Up","Default.Header");
	htmlUserPage("Sign Up","SignUpDone.Body");
	htmlFooter("Default.Footer");

}//void htmlSignUpDone(void);


void htmlLostPasswordDone(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPerm=0;
	unsigned uAuthorize=0;

	if(!strcmp(gcFunction,"ChangePassword") && 
			gcLogin[0] && gcEmailCode[0] && strlen(gcEmailCode)==16 && gcPasswd[0] && gcPasswd2[0])
	{
		if(strcmp(gcPasswd,gcPasswd2))
		{
			gcMessage="Both passwords must match";
			htmlHeader("Lost Password","Default.Header");
			htmlUserPage("Lost Password","LostPasswordDone.Body");
			htmlFooter("Default.Footer");
		}
		if(strlen(gcPasswd)<8)
		{
			gcMessage="Password must be at least 8 characters long";
			htmlHeader("Lost Password","Default.Header");
			htmlUserPage("Lost Password","LostPasswordDone.Body");
			htmlFooter("Default.Footer");
		}
		if(uNoUpper(gcPasswd) || uNoLower(gcPasswd) || uNoDigit(gcPasswd))
		{
			gcMessage="Password must have upper and lower case letters and at least one number";
			htmlHeader("Lost Password","Default.Header");
			htmlUserPage("Lost Password","LostPasswordDone.Body");
			htmlFooter("Default.Footer");//Footer exits
		}

		char cBuffer[100]={""};
		char cSalt[16]={"$1$23abc123$"};//TODO set to random salt;
		(void)srand((long long)time((time_t *)NULL)*getpid());
		to64(&cSalt[3],rand(),8);

		sprintf(cBuffer,"%.99s",gcPasswd);
		EncryptPasswdWithSalt(cBuffer,cSalt);

		sprintf(gcQuery,"UPDATE tAuthorize SET"
			" cClrPasswd='%s',"
			" cPasswd='%s',"
			" cIpMask='',"
			" uModBy=1,"
			" uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE cLabel='%s' AND cIpMask='%s'"
				,gcPasswd
				,cBuffer
				,TextAreaSave(gcLogin),TextAreaSave(gcEmailCode));
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			gcMessage="Unknown error try again later";
			htmlHeader("Lost Password","Default.Header");
			htmlUserPage("Lost Password","LostPasswordDone.Body");
			htmlFooter("Default.Footer");
		}
		gcMessage="Password changed if code was valid.";
		htmlHeader("Lost Password","Default.Header");
		htmlUserPage("Lost Password","Login.Body");
		htmlFooter("Default.Footer");
	}
	else if(!strcmp(gcFunction,"ChangePassword") && gcLogin[0] && gcEmailCode[0])
	{
		gcMessage="Enter new password twice.";
		htmlHeader("Lost Password","Default.Header");
		htmlUserPage("Lost Password","LostPasswordDone.Body");//Almost Done ;)
		htmlFooter("Default.Footer");
	}

	//
	//check to see if active account if active send mail change link
	//
	sprintf(gcQuery,"SELECT uAuthorize,uPerm FROM tAuthorize"
			" WHERE cLabel='%s' LIMIT 1",TextAreaSave(gcLogin));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unknown error try again later";
		htmlHeader("Lost Password","Default.Header");
		htmlUserPage("Lost Password","LostPassword.Body");
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uAuthorize);
		sscanf(field[1],"%u",&uPerm);
	}
	mysql_free_result(res);

	if(uAuthorize && uPerm)
	{
		NewCodeAndLinkEmail(uAuthorize,TextAreaSave(gcLogin),"raddev.unxs.io:9333");
		sleep(3);
		gcMessage="Link sent to your email if it exists.";
		htmlHeader("Lost Password","Default.Header");
		htmlUserPage("Lost Password","LostPassword.Body");
		htmlFooter("Default.Footer");
	}
	else
	{
		gcMessage="Or user does not exist or it is not active yet.";
		htmlHeader("Lost Password","Default.Header");
		htmlUserPage("Lost Password","LostPassword.Body");
		htmlFooter("Default.Footer");
	}

}//void htmlLostPasswordDone(void);


void htmlLostPassword(void)
{
	gcMessage="Enter your login email.";
	htmlHeader("Lost Password","Default.Header");
	htmlUserPage("Lost Password","LostPassword.Body");
	htmlFooter("Default.Footer");

}//void htmlLostPassword(void)


void htmlJobOffer(void)
{
	htmlHeader("JobOffer","Default.Header");
	htmlUserPage("JobOffer","JobOffer.Body");
	htmlFooter("Default.Footer");

}//void htmlJobOffer(void)


void htmlCalendar(void)
{
	htmlHeader("Calendar","Default.Header");
	htmlUserPage("Calendar","Calendar.Body");
	htmlFooter("Default.Footer");

}//void htmlCalendar(void)


void htmlUser(void)
{
	htmlHeader("User","Default.Header");
	htmlUserPage("User","User.Body");
	htmlFooter("Default.Footer");

}//void htmlUser(void)


void htmlUserPage(char *cTitle, char *cTemplateName)
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
			template.cpValue[1]="/{{cProject}}App/";
			
			template.cpName[2]="gcLogin";
			if(gcUser[0])
				template.cpValue[2]=gcUser;
			else
				//Lost Login or Signup Login
				template.cpValue[2]=gcLogin;

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

			template.cpName[8]="gcBrand";
			template.cpValue[8]=INTERFACE_HEADER_TITLE;

			template.cpName[9]="gcCopyright";
			template.cpValue[9]=INTERFACE_COPYRIGHT;

			template.cpName[10]="gcFiveIn";
			template.cpValue[10]=gcFiveIn;

			template.cpName[11]="gcEmailCode";
			template.cpValue[11]=gcEmailCode;

			template.cpName[12]="";

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

	sprintf(gcQuery,"SELECT cComment FROM tConfiguration"
			" WHERE cLabel='cOrg_NewMOTD' LIMIT 1");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		fprintf(fp,"%s",field[0]);
	mysql_free_result(res);

	fprintf(fp,"<!-- funcMOTD(fp) End -->\n");

}//void funcMOTD(FILE *fp)


void funcOperationHistory(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcOperationHistory(fp) Start -->\n");

	sprintf(gcQuery,"SELECT cLabel,FROM_UNIXTIME(uCreatedDate),cHost,cServer"
			" FROM tLog"
			" WHERE uLogType!=8"
			" AND (uCreatedBy=%u OR uLoginClient=%u OR uOwner=%u)"
			" ORDER BY uCreatedDate DESC LIMIT 10",
				guLoginClient,guLoginClient,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"mysql error<br>");
		return;
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"%s %s %s %s<br>",field[1],field[0],field[2],field[3]);
	mysql_free_result(res);

	fprintf(fp,"<!-- funcOperationHistory(fp) End -->\n");

}//void funcOperationHistory(FILE *fp)


void funcLoginHistory(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcLoginHistory(fp) Start -->\n");

	sprintf(gcQuery,"SELECT cLabel,FROM_UNIXTIME(uCreatedDate),cHost,cServer"
			" FROM tLog"
			" WHERE uLoginClient=%u"
			" AND uLogType=8"
			" ORDER BY uCreatedDate DESC LIMIT 10",
				guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"mysql error<br>");
		return;
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"%s %s %s %s<br>",field[1],field[0],field[2],field[3]);
	mysql_free_result(res);

	fprintf(fp,"<!-- funcLoginHistory(fp) End -->\n");

}//void funcLoginHistory(FILE *fp)


unsigned uValidPasswd(char *cPasswd,unsigned guLoginClient)
{
	char cSalt[16]={""};
	char cPassword[100]={""};
	char gcPasswd[100]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cPasswd FROM tAuthorize WHERE uCertClient=%u",guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return(0);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cPassword,"%.99s",field[0]);
	mysql_free_result(res);

	sprintf(gcPasswd,"%.99s",cPasswd);
	if(cPassword[0])
	{
		//MD5 vs DES salt determination
		if(cPassword[0]=='$' && cPassword[2]=='$')
			sprintf(cSalt,"%.12s",cPassword);
		else
			sprintf(cSalt,"%.2s",cPassword);
		EncryptPasswdWithSalt(gcPasswd,cSalt);
		if(!strcmp(gcPasswd,cPassword))
			return(1);
		else
			return(0);
	}
	return(0);
}//unsigned uValidPasswd(char *cPasswd)


void htmlOperationsInfo(void)
{

	htmlHeader("User","Default.Header");

        MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT cLabel,FROM_UNIXTIME(uCreatedDate),cHost,cServer"
			" FROM tLog WHERE uLogType!=8 AND"
			" (uCreatedBy=%u OR uLoginClient=%u OR uOwner=%u)"
			" ORDER BY uCreatedDate DESC LIMIT 20",
				guLoginClient,guLoginClient,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("mysql error<br>");
		return;
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		printf("%s %s %s %s<br>",field[1],field[0],field[2],field[3]);
	mysql_free_result(res);


	exit(0);

}//void htmlOperationsInfo(void)


void htmlLoginInfo(void)
{

	htmlHeader("User","Default.Header");

        MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT cLabel,FROM_UNIXTIME(uCreatedDate),cHost,cServer"
			" FROM tLog WHERE uLoginClient=%u"
			" AND uLogType=8 ORDER BY uCreatedDate DESC LIMIT 20",guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("mysql error<br>");
		return;
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		printf("%s %s %s %s<br>",field[1],field[0],field[2],field[3]);
	mysql_free_result(res);


	exit(0);

}//void htmlLoginInfo(void)
