/*
FILE 
	user.c
	svn ID removed
AUTHOR
	(C) 2009 Hugo Urquiza for Unixservice
PURPOSE
	mail2Admin program file. Provides code 
	for user management
*/

#include "interface.h"


static unsigned uUser=0;
//cLogin: Pop3 user login
static char cLogin[33]={""};
static char *cLoginStyle="type_fields_off";
//uDomain: Domain if applicable
static unsigned uDomain=0;
static char *cuDomainStyle="type_fields_off";
//cPasswd: User password MD5 encrypted
static char cPasswd[65]={""};
static char *cPasswdStyle="type_fields_off";
//uUserType: General user type
static unsigned uUserType=0;
static char *cuUserTypeStyle="type_fields_off";
//uServerGroup: Server account is on
unsigned uServerGroup=0;
char *cuServerGroupStyle="type_fields_off";
//uStatus: Status of user
static char cStatus[33]={""};
//cSecretQuestion: Security question for password update
static char cSecretQuestion[256]={""};
static char *cSecretQuestionStyle="type_fields_off";
//cSecretAnswer: Security question answer for password update
static char cSecretAnswer[256]={""};
static char *cSecretAnswerStyle="type_fields_off";
//uOwner: Record owner
static unsigned uOwner=0;
//uCreatedBy: uClient for last insert
static unsigned uCreatedBy=0;
#define ISM3FIELDS
//uCreatedDate: Unix seconds date last insert
static time_t uCreatedDate=0;
//uModBy: uClient for last update
static unsigned uModBy=0;
//uModDate: Unix seconds date last update
static time_t uModDate=0;

static char cSearch[33]={""};
static char *cConfig={""};
static char cLabel[33]={""};

static unsigned uPage=0;

#define VAR_LIST_tUser "tUser.uUser,tUser.cLogin,tUser.uDomain,tUser.cPasswd,tUser.uUserType,tUser.uServerGroup,tUser.uStatus,tUser.cSecretQuestion,tUser.cSecretAnswer,tUser.uOwner,tUser.uCreatedBy,tUser.uCreatedDate,tUser.uModBy,tUser.uModDate"

 //Local only
void SetUserFieldsOn(void);
unsigned ValidateUserInput(void);
void InsertUser(void);
void UpdateUser(void);
void UserDiskUsageReport(void);


void ProcessUserVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uUser"))
			sscanf(entries[i].val,"%u",&uUser);
		else if(!strcmp(entries[i].name,"cLogin"))
			sprintf(cLogin,"%.32s",WordToLower(entries[i].val));
		else if(!strcmp(entries[i].name,"uDomain"))
			sscanf(entries[i].val,"%u",&uDomain);
		else if(!strcmp(entries[i].name,"cPasswd"))
			sprintf(cPasswd,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uUserType"))
			sscanf(entries[i].val,"%u",&uUserType);
		else if(!strcmp(entries[i].name,"uServerGroup"))
			sscanf(entries[i].val,"%u",&uServerGroup);
		else if(!strcmp(entries[i].name,"cStatus"))
			sprintf(cStatus,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cSecretQuestion"))
			sprintf(cSecretQuestion,"%.255s",entries[i].val);
		else if(!strcmp(entries[i].name,"cSecretAnswer"))
			sprintf(cSecretAnswer,"%.255s",entries[i].val);
		else if(!strcmp(entries[i].name,"uOwner"))
			sscanf(entries[i].val,"%u",&uOwner);
		else if(!strcmp(entries[i].name,"uCreatedBy"))
			sscanf(entries[i].val,"%u",&uCreatedBy);
		else if(!strcmp(entries[i].name,"uCreatedDate"))
			sscanf(entries[i].val,"%lu",&uCreatedDate);
		else if(!strcmp(entries[i].name,"uModBy"))
			sscanf(entries[i].val,"%u",&uModBy);
		else if(!strcmp(entries[i].name,"uModDate"))
			sscanf(entries[i].val,"%lu",&uModDate);
		else if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.32s",entries[i].val);

	}

}//void ProcessUserVars(pentry entries[], int x)



void LoadUser(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uStatus=0;
	
	//Row id defined by module globar uUser
	ExtSelectRow("tUser",VAR_LIST_tUser,uUser);
	macro_mySQLRunAndStore(res);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uUser);
		sprintf(cLogin,"%.32s",field[1]);
		sscanf(field[2],"%u",&uDomain);
		sprintf(cPasswd,"%.64s",field[3]);
		sscanf(field[4],"%u",&uUserType);
		sscanf(field[5],"%u",&uServerGroup);
		sscanf(field[6],"%u",&uStatus);
		sprintf(cSecretQuestion,"%.255s",field[7]);
		sprintf(cSecretAnswer,"%.255s",field[8]);
		sscanf(field[9],"%u",&uOwner);
		sscanf(field[10],"%u",&uCreatedBy);
		sscanf(field[11],"%lu",&uCreatedDate);
		sscanf(field[12],"%u",&uModBy);
		sscanf(field[13],"%lu",&uModDate);
		sprintf(cStatus,"%.32s",ForeignKey("tStatus","cLabel",uStatus));
	}
	else
		gcMessage="<blink>Error: </blink> Could not load record!";

	mysql_free_result(res);
}


void UserGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uUser"))
			sscanf(gentries[i].val,"%u",&uUser);
		else if(!strcmp(gentries[i].name,"cSearch"))
			sprintf(cSearch,"%.32s",gentries[i].val);
		else if(!strcmp(gentries[i].name,"uPage"))
			sscanf(gentries[i].val,"%u",&uPage);
	}
	if(uUser)
		LoadUser();
	else
	{
		FILE *fp;
		if((fp=fopen("/dev/null","w")))
			funcUserNavList(fp);
	}


	htmlUser();

}//void UserGetHook(entry gentries[],int x)


void UserCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"User"))
	{
		char cDomain[33]={""};
		char cServerGroup[33]={""};

		ProcessUserVars(entries,x);
		if(!strcmp(gcFunction,"New"))
		{
			if(guPermLevel>=10)
			{
				sprintf(gcNewStep,"Confirm ");
				gcInputStatus[0]=0;
				gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
				SetUserFieldsOn();
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Confirm New"))
		{
			if(guPermLevel>=10)
			{
				if(ValidateUserInput())
				{
					sprintf(gcNewStep,"Confirm ");
					gcInputStatus[0]=0;
					gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
					htmlUser();
				}
				InsertUser();

				sprintf(cDomain,"%s",ForeignKey("tDomain","cDomain",uDomain));
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				SubmitJob("NewUser",cDomain,cLogin,cServerGroup,"",
						uDomain,uUser,
						guCompany,guLoginClient);
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Modify"))
		{
			if(uAllowMod(uOwner,uCreatedBy))
			{
				sprintf(gcModStep,"Confirm ");
				gcInputStatus[0]=0;
				gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
				SetUserFieldsOn();
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Confirm Modify"))
		{
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(ValidateUserInput())
				{
					sprintf(gcModStep,"Confirm ");
					gcInputStatus[0]=0;
					gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
					htmlUser();
				}
				UpdateUser();
				sprintf(cDomain,"%s",ForeignKey("tDomain","cDomain",uDomain));
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				SubmitJob("ModUser",cDomain,cLogin,cServerGroup,"",
						uDomain,uUser,
						guCompany,guLoginClient);
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Delete"))
		{
			if(uAllowDel(uOwner,uCreatedBy))
			
			{
				sprintf(gcDelStep,"Confirm ");
				gcMessage="Double check you have selected the correct record to delete. Then confirm. Any other action to cancel.";
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Confirm Delete"))
		{
			if(uAllowDel(uOwner,uCreatedBy))
			{
				sprintf(gcQuery,"UPDATE tUser SET uStatus=3 WHERE uUser=%u AND uStatus=4",uUser);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				if(mysql_affected_rows(&gMysql))
				{
					sprintf(cDomain,"%s",ForeignKey("tDomain","cDomain",uDomain));
					sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
					SubmitJob("DelUser",cDomain,cLogin,cServerGroup,"",
						uDomain,uUser,
						guCompany,guLoginClient);
					sprintf(cStatus,"%.32s",ForeignKey("tStatus","cLabel",3));
					unxsMailLog(uUser,"tUser","Pending Del");
					gcMessage="User scheduled for deletion";
				}
				else
				{
					gcMessage="<blink>Error:</blink> User status error";
				}
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"See Disk Usage Report"))
			UserDiskUsageReport();

		htmlUser();
	}

}//void UserCommands(pentry entries[], int x)


void htmlUser(void)
{
	htmlHeader("unxsMail System","Header");
	htmlUserPage("unxsMail System","User.Body");
	htmlFooter("Footer");

}//void htmlUser(void)


void htmlUserPage(char *cTitle, char *cTemplateName)
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
			char cuOwner[16]={""};
			char cuCreatedBy[16]={""};
			char cuCreatedDate[16]={""};
			char cuModBy[16]={""};
			char cuModDate[16]={""};
			char cuUser[16]={""};

			sprintf(cuUser,"%u",uUser);
			sprintf(cuOwner,"%u",uOwner);
			sprintf(cuCreatedBy,"%u",uCreatedBy);
			sprintf(cuCreatedDate,"%lu",uCreatedDate);
			sprintf(cuModBy,"%u",uModBy);
			sprintf(cuModDate,"%lu",uModDate);

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="mail2Admin.cgi";
			
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

			template.cpName[8]="gcInputStatus";
			template.cpValue[8]=gcInputStatus;
				
			template.cpName[9]="gcNewStep";
			template.cpValue[9]=gcNewStep;

			template.cpName[10]="gcModStep";
			template.cpValue[10]=gcModStep;
			
			template.cpName[11]="gcDelStep";
			template.cpValue[11]=gcDelStep;

			template.cpName[12]="cLogin";
			template.cpValue[12]=cLogin;
			
			template.cpName[13]="cLoginStyle";
			template.cpValue[13]=cLoginStyle;

			template.cpName[14]="cPasswd";
			template.cpValue[14]=cPasswd;

			template.cpName[15]="cPasswdStyle";
			template.cpValue[15]=cPasswdStyle;

			template.cpName[16]="cStatus";
			template.cpValue[16]=cStatus;

			template.cpName[17]="uOwner";
			template.cpValue[17]=cuOwner;

			template.cpName[18]="uCreatedBy";
			template.cpValue[18]=cuCreatedBy;

			template.cpName[19]="uCreatedDate";
			template.cpValue[19]=cuCreatedDate;

			template.cpName[20]="uModBy";
			template.cpValue[20]=cuModBy;

			template.cpName[21]="uModDate";
			template.cpValue[21]=cuModDate;

			template.cpName[22]="cSearch";
			template.cpValue[22]=cSearch;

			template.cpName[23]="uUser";
			template.cpValue[23]=cuUser;

			template.cpName[24]="";

			printf("\n<!-- Start htmlUserPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
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


void funcSelectDomain(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cuDomain[16]={""};

	sprintf(cuDomain,"%u",uDomain);
	
	interface_ExtSelectSearch("tDomain","tDomain.uDomain,tDomain.cDomain","tDomain.cDomain","%",NULL,NULL);
	macro_mySQLRunAndStore(res);

	fprintf(fp,"<input type=hidden name=uDomain value=%u>\n",uDomain);
	fprintf(fp,"<select name=uDomain class=%s %s>\n",cuDomainStyle,gcInputStatus);
	fprintf(fp,"<option value=0>---</option>\n");
	
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<option value=%s ",field[0]);
		if(!strcmp(field[0],cuDomain))
			fprintf(fp,"selected");
		 fprintf(fp,">%s</option>\n",field[1]);
	}
	fprintf(fp,"</select>");
	
	mysql_free_result(res);

}//void funcSelectDomain(FILE *fp)


void funcSelectUserType(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cuUserType[16]={""};

	sprintf(cuUserType,"%u",uUserType);
	
	//tUserType should be a system wide table
	sprintf(gcQuery,"SELECT tUserType.uUserType,tUserType.cLabel FROM tUserType ORDER BY tUserType.cLabel");
	macro_mySQLRunAndStore(res);

	fprintf(fp,"<input type=hidden name=uUserType value=%u>\n",uUserType);
	fprintf(fp,"<select name=uUserType class=%s %s>\n",cuUserTypeStyle,gcInputStatus);
	fprintf(fp,"<option value=0>---</option>\n");
	
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<option value=%s ",field[0]);
		if(!strcmp(field[0],cuUserType))
			fprintf(fp,"selected");
		 fprintf(fp,">%s</option>\n",field[1]);
	}
	fprintf(fp,"</select>");
	
	mysql_free_result(res);

}//void funcSelectUserType(FILE *fp)


void funcSelectServerGroup(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cuServerGroup[16]={""};

	sprintf(cuServerGroup,"%u",uServerGroup);
	
	interface_ExtSelectSearch("tServerGroup","tServerGroup.uServerGroup,tServerGroup.cLabel","tServerGroup.cLabel","%",NULL,NULL);
	macro_mySQLRunAndStore(res);

	fprintf(fp,"<input type=hidden name=uServerGroup value=%u>\n",uServerGroup);
	fprintf(fp,"<select name=uServerGroup class=%s %s>\n",cuServerGroupStyle,gcInputStatus);
	fprintf(fp,"<option value=0>---</option>\n");
	
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<option value=%s ",field[0]);
		if(!strcmp(field[0],cuServerGroup))
			fprintf(fp,"selected");
		 fprintf(fp,">%s</option>\n",field[1]);
	}
	fprintf(fp,"</select>");
	
	mysql_free_result(res);

}//void funcSelectServerGroup(FILE *fp)


void funcUserNavList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uResults=0;
	unsigned uResultsPerPage=20;
	unsigned uPages=0;	

	interface_ExtSelectSearch("tUser","tUser.uUser,(SELECT tDomain.cDomain FROM tDomain WHERE uDomain=tUser.uDomain),tUser.cLogin","tUser.cLogin",
			cSearch,NULL,NULL);
	macro_mySQLRunAndStore(res);

	uResults=mysql_num_rows(res);
	uPages=(uResults/uResultsPerPage)+1;

	if(uResults==1)
	{
		if((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"<a href=\"mail2Admin.cgi?gcPage=User&uUser=%s&cSearch=%s\" class=darkLink >%s@%s</a><br>\n",
				field[0]
				,cURLEncode(cSearch)
				,field[2]
				,field[1]
				);
			sscanf(field[0],"%u",&uUser);
			mysql_free_result(res);

			LoadUser();
			return;
		}
	}
	else if(uResults>1)
	{
		char cLimit[35]={""};
		unsigned i;

		mysql_free_result(res);

		if(uPage>0) uPage--;
		sprintf(cLimit," LIMIT %u OFFSET %u",uResultsPerPage,(uPage*10));

		interface_ExtSelectSearch("tUser","tUser.uUser,(SELECT tDomain.cDomain FROM tDomain WHERE uDomain=tUser.uDomain),tUser.cLogin",
				"tUser.cLogin",cSearch,NULL,cLimit);
		macro_mySQLRunAndStore(res);
		while((field=mysql_fetch_row(res)))
			fprintf(fp,"<a href=\"mail2Admin.cgi?gcPage=User&uUser=%s&cSearch=%s\" class=darkLink >%s@%s</a><br>\n",
				field[0]
				,cURLEncode(cSearch)
				,field[2]
				,field[1]
				);
		
		if(uResults>uResultsPerPage)
		{
			uPage++;
			if(uPage>1)
			{
				for(i=1;i<uPage;i++)
					fprintf(fp,"<a href=\"mail2Admin.cgi?gcPage=User&cSearch=%s&uPage=%u\" class=darkLink >%u</a>",
						cURLEncode(cSearch)
						,i
						,i
						);
			}
			fprintf(fp,"<b>%u</b>",uPage);

			if(uPage<(uPages+2))
			{
				for(i=(uPage+1);i<(uPages+2);i++)
					fprintf(fp,"<a href=\"mail2Admin.cgi?gcPage=User&cSearch=%s&uPage=%u\" class=darkLink >%u</a>",
						cURLEncode(cSearch)
						,i
						,i
						);
			}
		}

	}
	else if(uResults==0)
		fprintf(fp,"No records found.");
	
	mysql_free_result(res);

}//void funcUserNavList(FILE *fp)


void SetUserFieldsOn(void)
{
	cLoginStyle="type_fields";
	cuDomainStyle="type_fields";
	cPasswdStyle="type_fields";
	cuUserTypeStyle="type_fields";
	cuServerGroupStyle="type_fields";
	cSecretQuestionStyle="type_fields";
	cSecretAnswerStyle="type_fields";

}//void SetUserFieldsOn(void)


unsigned ValidateUserInput(void)
{
	if(!cLogin[0])
	{
		SetUserFieldsOn();
		cLoginStyle="type_fields_req";
		gcMessage="<blink>Error: </blink> Must enter user login";
		return(1);
	}
	else
	{
		if(!strcmp(gcFunction,"Confirm New"))
		{
			MYSQL_RES *res;
			sprintf(gcQuery,"SELECT uUser FROM tUser WHERE cLogin='%s'",TextAreaSave(cLogin));
			macro_mySQLRunAndStore(res);

			if(mysql_num_rows(res))
			{
				mysql_free_result(res);
				SetUserFieldsOn();
				cLoginStyle="type_fields_req";
				gcMessage="<blink>Error: </blink> User login already in use";
				return(1);
			}
			mysql_free_result(res);
		}
	}
	
	if(!uDomain)
	{
		SetUserFieldsOn();
		cuDomainStyle="type_fields_req";
		gcMessage="<blink>Error: </blink> Must select domain name";
		return(1);
	}

	if(!cPasswd[0])
	{
		SetUserFieldsOn();
		cPasswdStyle="type_fields_req";
		gcMessage="<blink>Error: </blink> Must enter user password";
		return(1);
	}
	else
	{
		if(strncmp(cPasswd,"$1$",3))
			EncryptPasswdMD5(cPasswd);
	}

	if(!uUserType)
	{
		SetUserFieldsOn();
		cuUserTypeStyle="type_fields_req";
		gcMessage="<blink>Error: </blink> Must select user type";
		return(1);
	}

	if(!uServerGroup)
	{
		SetUserFieldsOn();
		cuServerGroupStyle="type_fields_req";
		gcMessage="<blink>Error: </blink> Must select server group";
		return(1);
	}

	return(0);

}//unsigned ValidateUserInput(void)


void InsertUser(void)
{
	sprintf(gcQuery,"INSERT INTO tUser SET cLogin='%s',uDomain=%u,cPasswd='%s',uUserType=%u,uServerGroup=%u,uStatus=%u,"
			"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			TextAreaSave(cLogin)
			,uDomain
			,cPasswd
			,uUserType
			,uServerGroup
			,1
			,guCompany
			,guLoginClient);
	macro_mySQLQueryHTMLError;

	if((uUser=mysql_insert_id(&gMysql)))
	{
		gcMessage="User added OK";
		unxsMailLog(uUser,"tUser","New");
	}
	else
		gcMessage="<blink>Error:</blink> Failed to add user. Contact support ASAP";

	//Lastly, we set the required ism3 fields, in case the user tries to modify the record after adding it
	//(Otherwise, the user will get the 'Denied by permission settings' error message.
	uOwner=guCompany;
	uCreatedBy=guLoginClient;

}//void InsertUser(void)


void UpdateUser(void)
{
	sprintf(gcQuery,"UPDATE tUser SET cLogin='%s',uDomain=%u,cPasswd='%s',uUserType=%u,uServerGroup=%u,uStatus=%u,"
			"uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uUser=%u",
			TextAreaSave(cLogin)
			,uDomain
			,cPasswd
			,uUserType
			,uServerGroup
			,2
			,guLoginClient
			,uUser);
	macro_mySQLQueryHTMLError;

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="User modified OK";
		unxsMailLog(uUser,"tUser","Mod");
	}
	else
		gcMessage="<blink>Error:</blink> Failed to modify user. Contact support ASAP";
	

}//void UpdateUser(void);


void CreateConfigs(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_RES *res3;
        MYSQL_ROW field2;

	if(!uUser || !uUserType)
		htmlPlainTextError("Unexpected CreateConfigs() error 1");

	sprintf(gcQuery,"SELECT tConfiguration.cValue FROM tConfiguration,tUserType WHERE "
			"tUserType.cLabel=tConfiguration.cLabel AND tUserType.uUserType=%u",uUserType);
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

					sprintf(gcQuery,"SELECT uUserConfig FROM tUserConfig WHERE uUser=%u AND uConfigSpec=%u "
							"AND cConfig!='' LIMIT 1",uUser,uConfigSpec);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
                				htmlPlainTextError(mysql_error(&gMysql));
					res3=mysql_store_result(&gMysql);
					if(mysql_num_rows(res3)==0)
					{

					sprintf(gcQuery,"INSERT INTO tUserConfig SET cLabel='%.32s',uUser=%u,uConfigSpec=%u,uOwner=%u,"
							"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
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

	sprintf(cOwner,"%.32s",ForeignKey("tClient","cLabel",uOwner));
	sprintf(cCreatedBy,"%.32s",ForeignKey("tClient","cLabel",uCreatedBy));

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


void funcUserSystemInfo(FILE *fp)
{
	//This function will show system info related to the loaded user.
	//Initially we will inform on the disk usage only
	FILE *procFP;
	char *cp;
	char cCmdOut[100]={""};
	char cDuBin[33]={"/usr/bin/du"}; //Can be overrided with tConfiguration

	if(!uUser) return;

	sprintf(gcQuery,"%s -h /var/mail/%s",cDuBin,cLogin);

	if((procFP=popen(gcQuery,"r")))
	{
		fgets(cCmdOut,99,procFP);
		pclose(procFP);
		//Yeah, we should have only a single line to read.
		//du sample output: "164K    /var/mail/eperez"
		if((cp=strchr(cCmdOut,'/')))
		{
			cp--;
			*cp=0;
			fprintf(fp,"Mailbox Disk Usage: %s<br>\n",cCmdOut);
		}
	}
	else
		fprintf(fp,"Mailbox Disk Usage: <font color=red>Couldn't stat, comand was %s</font><br>",gcQuery);
	
	sprintf(gcQuery,"%s -h /var/mail/%s.spam",cDuBin,cLogin);

	if((procFP=popen(gcQuery,"r")))
	{
		fgets(cCmdOut,99,procFP);
		pclose(procFP);
		if((cp=strchr(cCmdOut,'/')))
		{
			cp--;
			*cp=0;
			fprintf(fp,"Spam Mailbox Disk Usage: %s\n",cCmdOut);
		}
	}
	else
		fprintf(fp,"Spam Mailbox Disk Usage: <font color=red>Couldn't stat, comand was %s</font><br>",gcQuery);


}//void funcUserSystemInfo(FILE *procFP)


void UserDiskUsageReport(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	FILE *procFP;
	char *cp;
	char cCmdOut[100]={""};
	char cDuBin[33]={"/usr/bin/du"}; //Can be overrided with tConfiguration

	sprintf(gcQuery,"SELECT cLogin FROM tUser ORDER BY cLogin");
	macro_mySQLRunAndStore(res);

	printf("Content-type: text/html\n\n");
	printf("<table>\n"
		"<tr><th>Login</th><th>Mailbox Disk Usage</th><th>Spam Mailbox Disk Usage</th></tr>\n");

	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"%s -h /var/mail/%s",cDuBin,field[0]);

		if((procFP=popen(gcQuery,"r")))
		{
			fgets(cCmdOut,99,procFP);
			pclose(procFP);
			if((cp=strchr(cCmdOut,'/')))
			{
				cp--;
				*cp=0;
			}
			printf("<tr><td>%s</td><td align=right>%s</td>",field[0],cCmdOut);
		}
		else
		{
			printf("<tr><td colspan=3>Couldn't run %s. Aborted",gcQuery);
			exit(1);
		}

		sprintf(gcQuery,"%s -h /var/mail/%s.spam",cDuBin,field[0]);
		if((procFP=popen(gcQuery,"r")))
		{
			fgets(cCmdOut,99,procFP);
			pclose(procFP);
			if((cp=strchr(cCmdOut,'/')))
			{
				cp--;
				*cp=0;
			}
		}
		else
		{
			printf("<tr><td colspan=3>Couldn't run %s. Aborted",gcQuery);
			exit(1);
		}
		printf("<td align=right>%s</td></tr>\n",cCmdOut);
	}
	
	printf("</table>\n");
	
	exit(0);

}//void UserDiskUsageReport(void)

