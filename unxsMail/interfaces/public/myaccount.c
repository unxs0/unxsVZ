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

static unsigned uUser=0;
static unsigned uUserType=0;

static char cId[65]={""};
static char cLogin[65]={""};
static char cPasswd[65]={""};
static char cConfirmPasswd[65]={""};
static char cSecretQuestion[256]={""};
static char cSecretAnswer[256]={""};

//
//Local only functions
void ResetPassword(char *cEmail);
void ResetPassword(char *cEmail);
void LowerCase(char *cString);
void MyGetConfiguration(MYSQL *gMysql,char *cName,char *cValue);
void to64(register char *s, register long v, register int n);
void EncryptPasswd(char *pw);
void LoadSecretQuestion(void);
void LoadAccountDefaultData(void);
void htmlPasswordReset(unsigned uStep);
void htmlCmdOutput(void);
void htmlCreateAccount(void);
void CreateConfigs(void);
void CreateConfigFromTemplate(unsigned uUserConfig, unsigned uUser, unsigned uConfigSpec);

unsigned AnswerIsCorrect(void);
unsigned Update_tUser(char *cPasswd);
unsigned uIdStatus(char *cId);
unsigned uValidUser(char *cEmail);
unsigned ValidateInput(void);
unsigned CreateEmailAccount(void);

char *GenerateRandomPassword(void);


void ProcessMyAccountVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cId"))
			sprintf(cId,"%.65s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLogin"))
			sprintf(cLogin,"%.65s",entries[i].val);
		else if(!strcmp(entries[i].name,"cPasswd"))
			sprintf(cPasswd,"%.65s",entries[i].val);
		else if(!strcmp(entries[i].name,"cConfirmPasswd"))
			sprintf(cConfirmPasswd,"%.65s",entries[i].val);
		else if(!strcmp(entries[i].name,"cSecretQuestion"))
			sprintf(cSecretQuestion,"%.255s",entries[i].val);
		else if(!strcmp(entries[i].name,"cSecretAnswer"))
			sprintf(cSecretAnswer,"%.255s",entries[i].val);
	}

}//void ProcessMyAccountVars(pentry entries[], int x)


void MyAccountGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"cId"))
			sprintf(cId,"%.64s",gentries[i].val);
	}

	if(gcFunction[0])
	{
		if(!strcmp(gcFunction,"Create") && cId[0])
		{
			//
			//First, check if we have a valid cId argument
			if(!uIdStatus(cId))
			{
				gcMessage="The entered ID is invalid. Couldn't process request.";
				htmlCmdOutput();
			}
			LoadAccountDefaultData();
			htmlCreateAccount();
			
		}
		else if(!strcmp(gcFunction,"PasswordReset"))
		{
			htmlPasswordReset(1);
		}
	}

		

}//void MyAccountGetHook(entry gentries[],int x)


void MyAccountCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"MyAccount"))
	{
		ProcessMyAccountVars(entries,x);
		if(!strcmp(gcFunction,"Ask the secret question"))
		{
			LoadSecretQuestion();
			if(!cSecretQuestion[0])
			{
				gcMessage="There's no such email account";
				htmlCmdOutput();
			}
			htmlPasswordReset(2);
			
		}
		else if(!strcmp(gcFunction,"Reset my password"))
		{
			if(!AnswerIsCorrect())
			{
				gcMessage="The answer is incorrect. Please use back to retry";
				htmlCmdOutput();
			}
			ResetPassword(cLogin);
			htmlCmdOutput(); //gcMessage sey by previous ResetPassword() call
		}
		else if(!strcmp(gcFunction,"Create my email account"))
		{
			if(ValidateInput())
			{
				static char cMsg[1024]={""};
				
				if((CreateEmailAccount()))
				{
					sprintf(cMsg,"Your email account %s@airnetworks.com.pr has been created OK.",cLogin);
					gcMessage=cMsg;
				}
				else
					gcMessage="Your email account could not be created. Please contact support";

				htmlCmdOutput();
			}
			htmlCreateAccount();
		}
		
		htmlMyAccount();
	}

}//void MyAccountCommands(pentry entries[], int x)


void htmlPasswordReset(unsigned uStep)
{
	htmlHeader("unxsMail User Interface","Header");
	sprintf(gcQuery,"MyAccountPwdReset.%u",uStep);
	htmlMyAccountPage("unxsMail User Interface",gcQuery);
	htmlFooter("Footer");
	
}//void htmlPasswordReset(void)


void htmlCreateAccount(void)
{
	htmlHeader("unxsMail User Interface","Header");
	htmlMyAccountPage("unxsMail User Interface","MyAccountCreate");
	htmlFooter("Footer");
	
}//void htmlCreateAccount(void)


void htmlCmdOutput(void)
{
	 htmlHeader("unxsMail User Interface","Header");
	 htmlMyAccountPage("unxsMail User Interface","MyAccountPwdMsg");
	 htmlFooter("Footer");
}//void htmlCmdOutput(void)


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
			template.cpValue[1]="index.cgi";
			
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

			template.cpName[7]="gcModStep";
			template.cpValue[7]=gcModStep;

			template.cpName[8]="cMessage";
			template.cpValue[8]=gcMessage;
			
			template.cpName[9]="cLogin";
			template.cpValue[9]=cLogin;

			template.cpName[10]="cId";
			template.cpValue[10]=cId;

			template.cpName[11]="cSecretQuestion";
			template.cpValue[11]=cSecretQuestion;

			template.cpName[12]="cSecretAnswer";
			template.cpValue[12]=cSecretAnswer;

			template.cpName[13]="";

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


unsigned CreateEmailAccount(void)
{
	//Be careful with the defaults below!! You rather use tConfiguration
	char cuDomain[16]={"1"};
	char cuUserType[16]={"1"};
	char cuServerGroup[16]={"1"};
	char cServerGroup[16]={""};
	char cDomain[16]={""};
	
	unsigned uDomain=1;
	unsigned uServerGroup=1;

	GetConfiguration("cuDefaultDomain",cuDomain,0,1);
	GetConfiguration("cuDefaultUserType",cuUserType,0,1);
	GetConfiguration("cuDefaultServerGroup",cuServerGroup,0,1);
	
	EncryptPasswd(cPasswd);

	sprintf(gcQuery,"INSERT INTO tUser SET cLogin='%s',uDomain='%s',cPasswd='%s',uUserType='%s',uServerGroup='%s',uStatus='%u',cSecretQuestion='%s',\
			cSecretAnswer='%s',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			cLogin
			,cuDomain
			,cPasswd
			,cuUserType
			,cuServerGroup
			,STATUS_PENDING
			,TextAreaSave(cSecretQuestion)
			,TextAreaSave(cSecretAnswer)
		);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	uUser=mysql_insert_id(&gMysql);
	
	sscanf(cuDomain,"%u",&uDomain);
	sscanf(cuServerGroup,"%u",&uServerGroup);
	sscanf(cuUserType,"%u",&uUserType);

	sprintf(cDomain,"%s",ForeignKey("tDomain","cDomain",uDomain));
	sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
											
	SubmitJob("NewUser",cDomain,cLogin,cServerGroup,"",
					uDomain,uUser,
					guLoginClient,guLoginClient);
	CreateConfigs();
	
	return(1);

}//unsigned CreateEmailAccount(void)


unsigned ValidateInput(void)
{
	if(!cLogin[0])
	{
		gcMessage="<blink>You must specify your login</blink>";
		return(0);
	}
	else
	{
		MYSQL_RES *res;
		MYSQL_ROW field;
		
		sprintf(gcQuery,"SELECT uUser FROM tUser WHERE cLogin='%s'",TextAreaSave(cLogin));
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);

		if((field=mysql_fetch_row(res)))
		{
			gcMessage="<blink>Your login is already taken</blink>";
			return(0);
		}
	}
	if(!cPasswd[0])
	{
		gcMessage="<blink>You must enter a password</blink>";
		return(0);
	}
	if(!cConfirmPasswd[0])
	{
		gcMessage="<blink>You must confirm your password</blink>";
		return(0);
	}
	if(strcmp(cPasswd,cConfirmPasswd))
	{
		gcMessage="<blink>The entered passwords do not match</blink>";
		return(0);
	}
	if(strlen(cPasswd)<5)
	{
		gcMessage="<blink>Password must have 5 characters as minimum length</blink>";
		return(0);
	}
	if(!cSecretQuestion[0])
	{
		 gcMessage="<blink>You must enter the secret question</blink>";
		 return(0);
	}
	if(!cSecretAnswer[0])
	{
		gcMessage="<blink>You must enter the secret answer</blink>";
		return(0);
	}
		
	return(1);
	
	
}//unsigned ValidateInput(void)


void LoadAccountDefaultData(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLogin FROM tRequest WHERE cId='%s'",cId);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sprintf(cLogin,"%.64s",field[0]);

}//void LoadAccountDefaultData(void)


unsigned AnswerIsCorrect(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	sprintf(gcQuery,"SELECT cSecretAnswer FROM tUser WHERE cLogin='%s'",TextAreaSave(cLogin));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	
	if((field=mysql_fetch_row(res)))
	{
		if(!strcmp(cSecretAnswer,field[0])) return(1);
	}

	return(0);
	
}//unsigned AnswerIsCorrect(void)


void LoadSecretQuestion(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char *cp;

	//
	//Strip the '@' from entered email if any
	if((cp=strchr(cLogin,'@'))) *cp=0;
	
	sprintf(gcQuery,"SELECT cSecretQuestion FROM tUser WHERE cLogin='%s'",TextAreaSave(cLogin));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sprintf(cSecretQuestion,"%.255s",field[0]);
		
}//void LoadSecretQuestion(void)


unsigned uIdStatus(char *cId)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uRequest FROM tRequest WHERE cId='%s'",cId);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	return((unsigned)mysql_num_rows(res));

}//unsigned uIdStatus(char *cId)


void ResetPassword(char *cEmail)
{
	//This function will reset the password of the user pointed by the email address at cEmail
	//For that we need to:
	//1. Generate a random password
	//2. Update unxsMail.tUser record
	//3. Submit a unxsMail 'Mod' job
	//4. Email the customer with her new login information.
	//
	
	char cPasswd[64]={""};
	char cPwdSave[16]={""};

	//3. Generate a random password
	sprintf(cPwdSave,"%.15s",GenerateRandomPassword());
	sprintf(cPasswd,"%s",cPwdSave);
	EncryptPasswd(cPasswd);

	//4. Update unxsMail.tUser record
	if((Update_tUser(cPasswd)))
	{
		MYSQL_RES *res;
		MYSQL_ROW field;
		unsigned uDomain=0;
		unsigned uUser=0;
		
		sprintf(gcQuery,"SELECT (SELECT tDomain.cDomain FROM tDomain WHERE tDomain.uDomain=tUser.uDomain),\
					(SELECT tServerGroup.cLabel FROM tServerGroup WHERE tServerGroup.uServerGroup=tUser.uServerGroup),\
					tUser.uDomain,tUser.uUser FROM tUser WHERE cLogin='%s'",cLogin);
		mysql_query(&gMysql,gcQuery);
		 if(mysql_errno(&gMysql))
		 	htmlPlainTextError(mysql_error(&gMysql));

		res=mysql_store_result(&gMysql);

		if((field=mysql_fetch_row(res)))
		{
			static char cEndMesaage[100]={""};
			
			sscanf(field[2],"%u",&uDomain);
			sscanf(field[3],"%u",&uUser);
			SubmitJob("ModUser",field[0],cLogin,field[1],"",uDomain,uUser,
					1,1);

			sprintf(cEndMesaage,"Your email password has been reset OK.<br>Your new password is: %s<br>\n",cPwdSave);
			gcMessage=cEndMesaage;
			
		}
		else
			gcMessage="<font color=red>Could not submit modify job. Please contact support</font>";
	}
	else
	{
		gcMessage="<font color=red>Update of the unxsMail.tUser record failed. Please contact support with this message</font>";
	}

}//void ResetPassword(char *cEmail)


unsigned Update_tUser(char *cPasswd)
{
	sprintf(gcQuery,"UPDATE tUser SET cPasswd='%s',uStatus=%u,uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE cLogin='%s'",
			cPasswd
			,STATUS_PENDING_MOD
			,cLogin);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	return((unsigned)mysql_affected_rows(&gMysql));

}//unsigned Update_tUser(unsigned uUser,char *cPasswd)


char *GenerateRandomPassword(void)
{
	//This function generates a random string containing alphanumeric characters
	//that will be used a the new account password upon reset
	static char cPassword[10]={""};

	(void)srand((int)time((time_t *)NULL));

	to64(&cPassword[0],rand(),6);
	cPassword[6]=0;

	return(cPassword);

}//char *GenerateRandomPassword(void);


static unsigned char itoa64[] =
"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
void to64(register char *s, register long v, register int n)
{
	while (--n >= 0)
	{
		*s++ = itoa64[v&0x3f];
		v >>= 2;
	}
}//void to64


void LowerCase(char *cString)
{
	register int iX;

	for(iX=0;iX<strlen(cString);iX++)
		cString[iX]=tolower(cString[iX]);	

}//void LowerCase(char *cString)


void EncryptPasswd(char *pw)
{
	char cSalt[] = "$1$01234567$";
        char *cpw;

    	(void)srand((int)time((time_t *)NULL));
    	to64(&cSalt[3],rand(),8);
	
	cpw = crypt(pw,cSalt);
	strcpy(pw,cpw);

}//void EncryptPasswdMD5(char *pw)


void CreateConfigs(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_RES *res3;
        MYSQL_ROW field2;

	if(!uUser || !uUserType)
		htmlPlainTextError("Unexpected CreateConfigs() error 1");

	sprintf(gcQuery,"SELECT tConfiguration.cValue FROM tConfiguration,tUserType WHERE tUserType.cLabel=tConfiguration.cLabel AND tUserType.uUserType=%u",uUserType);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		unsigned uConfigSpec;
		unsigned register i=0;

		cp=field[0];
		for(i=0;field[0][i];i++)
		{
			if(field[0][i]==';')
			{
				field[0][i]=0;
				sscanf(cp,"%u",&uConfigSpec);
				cp=field[0]+i+1;
				sprintf(gcQuery,"SELECT cLabel FROM tConfigSpec WHERE uConfigSpec=%u",uConfigSpec);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
                			htmlPlainTextError(mysql_error(&gMysql));
				res2=mysql_store_result(&gMysql);
				if((field2=mysql_fetch_row(res2)))
				{
					unsigned uUserConfig=0;

					sprintf(gcQuery,"SELECT uUserConfig FROM tUserConfig WHERE uUser=%u AND uConfigSpec=%u AND cConfig!='' LIMIT 1",uUser,uConfigSpec);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
                				htmlPlainTextError(mysql_error(&gMysql));
					res3=mysql_store_result(&gMysql);
					if(mysql_num_rows(res3)==0)
					{

					sprintf(gcQuery,"INSERT INTO tUserConfig SET cLabel='%.32s',uUser=%u,uConfigSpec=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							field2[0],
							uUser,
							uConfigSpec,
							guLoginClient,
							guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					uUserConfig=mysql_insert_id(&gMysql);
					if(!uUserConfig)
						htmlPlainTextError("Unexpected uUserConfig error");
					CreateConfigFromTemplate(uUserConfig,uUser,uConfigSpec);

					}
					mysql_free_result(res3);
				}
				mysql_free_result(res2);
			}
		}
	}
	mysql_free_result(res);

}//void CreateConfigs(void)


void CreateConfigFromTemplate(unsigned uUserConfig, unsigned uUser, unsigned uConfigSpec)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	struct t_template template;
	char cLabel[33]={""};
	char *cConfig={""};
	char cTemplateName[64]={""};
	char cLogin[64]={""};
	unsigned uDomain=0;
	char cDomain[100]={""};
	unsigned uUserType=0;
	char cUserType[33]={""};
	unsigned uServer=0;
	char cServer[64]={""};
	unsigned uStatus=0;
	char cStatus[64]={""};
	char cOwner[33]={""};
	char cCreatedBy[33]={""};

	sprintf(cTemplateName,"%.63s",ForeignKey("tConfigSpec","cLabel",uConfigSpec));
	sprintf(cLogin,"%.63s",ForeignKey("tUser","cLogin",uUser));

	sscanf(ForeignKey("tUser","uDomain",uUser),"%u",&uDomain);
	sprintf(cDomain,"%.99s",ForeignKey("tDomain","cDomain",uDomain));

	sscanf(ForeignKey("tUser","uUserType",uUser),"%u",&uUserType);
	sprintf(cUserType,"%.32s",ForeignKey("tUserType","cLabel",uUserType));

	sscanf(ForeignKey("tUser","uServer",uUser),"%u",&uServer);
	sprintf(cServer,"%.32s",ForeignKey("tServer","cLabel",uServer));

	sscanf(ForeignKey("tUser","uStatus",uUser),"%u",&uStatus);
	sprintf(cStatus,"%.32s",ForeignKey("tStatus","cLabel",uStatus));

	sprintf(cOwner,"%.32s",ForeignKey("tClient","cLabel",1));
	sprintf(cCreatedBy,"%.32s",ForeignKey("tClient","cLabel",1));

	//Setup configuration sections
	TemplateSelect(cTemplateName);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		//This is pure alpha template lib stuff...need to pass buffer size for safety
		char cBuffer[10000]={""};
		char cQuery[10240]={""};

		template.cpName[0]="cLogin";
		template.cpValue[0]=cLogin;

		template.cpName[1]="cDomain";
		template.cpValue[1]=cDomain;

		template.cpName[2]="cUserType";
		template.cpValue[2]=cUserType;

		template.cpName[3]="cServer";
		template.cpValue[3]=cServer;

		template.cpName[4]="cStatus";
		template.cpValue[4]=cStatus;

		template.cpName[5]="cOwner";
		template.cpValue[5]=cOwner;

		template.cpName[6]="cCreatedBy";
		template.cpValue[6]=cCreatedBy;

		template.cpName[7]="";

		TemplateToBuffer(field[0],&template,cBuffer);
		cConfig=cBuffer;
		sprintf(cLabel,"%.32s",cTemplateName);
		sprintf(cQuery,"UPDATE tUserConfig SET cConfig='%s',cLabel='%s' WHERE uUserConfig=%u",TextAreaSave(cBuffer),cTemplateName,uUserConfig);
        	mysql_query(&gMysql,cQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

}//void CreateConfigFromTemplate(unsigned uUserConfig, unsigned uUser, unsigned uConfigSpec)

