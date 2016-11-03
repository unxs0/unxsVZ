/*
FILE 
	alias.c
	svn ID removed
AUTHOR
	(C) 2008-2009 Hugo Urquiza for Unixservice
PURPOSE
	mail2Admin program file. Provides code 
	for aliases management
*/

#include "interface.h"

static unsigned uAlias=0;
//cUser: Local mail user
static char cUser[33]={""};
static char *cUserStyle="type_fields_off";
//cTargetEmail: Local mail user destination email or program
static char cTargetEmail[256]={""};
static char *cTargetEmailStyle="type_fields_off";
//uServerGroup: Server account is on
extern unsigned uServerGroup;
extern char *cuServerGroupStyle;
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

#define VAR_LIST_tAlias "tAlias.uAlias,tAlias.cUser,tAlias.cTargetEmail,tAlias.uServerGroup,tAlias.uOwner,tAlias.uCreatedBy,tAlias.uCreatedDate,tAlias.uModBy,tAlias.uModDate"


//Local only
void SetAliasFieldsOn(void);
unsigned ValidateAliasInput(void);
void InsertAlias(void);
void UpdateAlias(void);
void DeleteAlias(void);


void ProcessAliasVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uAlias"))
			sscanf(entries[i].val,"%u",&uAlias);
		else if(!strcmp(entries[i].name,"cUser"))
			sprintf(cUser,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cTargetEmail"))
			sprintf(cTargetEmail,"%.255s",entries[i].val);
		else if(!strcmp(entries[i].name,"uServerGroup"))
			sscanf(entries[i].val,"%u",&uServerGroup);
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

}//void ProcessAliasVars(pentry entries[], int x)


void LoadAlias(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	//Row id defined by module globar uAlias
	ExtSelectRow("tAlias",VAR_LIST_tAlias,uAlias);
	macro_mySQLRunAndStore(res);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uAlias);
		sprintf(cUser,"%.32s",field[1]);
		sprintf(cTargetEmail,"%.255s",field[2]);
		sscanf(field[3],"%u",&uServerGroup);
		sscanf(field[4],"%u",&uOwner);
		sscanf(field[5],"%u",&uCreatedBy);
		sscanf(field[6],"%lu",&uCreatedDate);
		sscanf(field[7],"%u",&uModBy);
		sscanf(field[8],"%lu",&uModDate);
	}
	else
		gcMessage="<blink>Error: </blink> Could not load record!";

	mysql_free_result(res);

}//void LoadAlias(void)


void AliasGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uAlias"))
			sscanf(gentries[i].val,"%u",&uAlias);
		else if(!strcmp(gentries[i].name,"cSearch"))
			sprintf(cSearch,"%.32s",gentries[i].val);
	}
	
	if(uAlias) LoadAlias();

	htmlAlias();

}//void AliasGetHook(entry gentries[],int x)


void AliasCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Alias"))
	{
		char cServerGroup[100]={""};
		char cJobData[100]={""};

		ProcessAliasVars(entries,x);
		if(!strcmp(gcFunction,"New"))
		{
			if(guPermLevel>=10)
			{
				sprintf(gcNewStep,"Confirm ");
				gcInputStatus[0]=0;
				gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
				SetAliasFieldsOn();
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Confirm New"))
		{
			if(guPermLevel>=10)
			{
				if(ValidateAliasInput())
				{
					sprintf(gcNewStep,"Confirm ");
					gcInputStatus[0]=0;
					gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
					htmlAlias();
				}
				InsertAlias();

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uAlias=%u;\ncUser=%s;\ncTargetEmail=%s;\n",
						uAlias,cUser,cTargetEmail);
				SubmitJob("NewAlias","",cUser,cServerGroup,cJobData,0,0,
								guLoginClient,guLoginClient);
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
				SetAliasFieldsOn();
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Confirm Modify"))
		{
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(ValidateAliasInput())
				{
					sprintf(gcModStep,"Confirm ");
					gcInputStatus[0]=0;
					gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
					htmlAlias();
				}
				UpdateAlias();
			
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uAlias=%u;\ncUser=%s;\ncTargetEmail=%s;\n",
						uAlias,cUser,cTargetEmail);
				SubmitJob("ModAlias","",cUser,cServerGroup,cJobData,0,0,
								guLoginClient,guLoginClient);
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
				DeleteAlias();

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uAlias=%u;\ncUser=%s;\ncTargetEmail=%s;\n",
						uAlias,cUser,cTargetEmail);
				SubmitJob("DelAlias","",cUser,cServerGroup,cJobData,0,0,
								guLoginClient,guLoginClient);
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		htmlAlias();
	}

}//void AliasCommands(pentry entries[], int x)


void htmlAlias(void)
{
	htmlHeader("unxsMail System","Header");
	htmlAliasPage("unxsMail System","Alias.Body");
	htmlFooter("Footer");

}//void htmlAlias(void)


void htmlAliasPage(char *cTitle, char *cTemplateName)
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
			char cuAlias[16]={""};

			sprintf(cuAlias,"%u",uAlias);
			sprintf(cuOwner,"%u",uOwner);
			sprintf(cuCreatedBy,"%u",uCreatedBy);
			sprintf(cuCreatedDate,"%lu",uCreatedDate);
			sprintf(cuModBy,"%u",uModBy);
			sprintf(cuModDate,"%lu",uModDate);

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="mail2Admin.cgi";
			
			template.cpName[2]="gcUser";
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

			template.cpName[8]="gcInputStatus";
			template.cpValue[8]=gcInputStatus;
				
			template.cpName[9]="gcNewStep";
			template.cpValue[9]=gcNewStep;

			template.cpName[10]="gcModStep";
			template.cpValue[10]=gcModStep;
			
			template.cpName[11]="gcDelStep";
			template.cpValue[11]=gcDelStep;

			template.cpName[12]="cUser";
			template.cpValue[12]=cUser;
			
			template.cpName[13]="cUserStyle";
			template.cpValue[13]=cUserStyle;

			template.cpName[14]="uOwner";
			template.cpValue[14]=cuOwner;

			template.cpName[15]="uCreatedBy";
			template.cpValue[15]=cuCreatedBy;

			template.cpName[16]="uCreatedDate";
			template.cpValue[16]=cuCreatedDate;

			template.cpName[17]="uModBy";
			template.cpValue[17]=cuModBy;

			template.cpName[18]="uModDate";
			template.cpValue[18]=cuModDate;

			template.cpName[19]="cSearch";
			template.cpValue[19]=cSearch;

			template.cpName[20]="uAlias";
			template.cpValue[20]=cuAlias;

			template.cpName[21]="cTargetEmail";
			template.cpValue[21]=cTargetEmail;

			template.cpName[22]="cTargetEmailStyle";
			template.cpValue[22]=cTargetEmailStyle;

			template.cpName[23]="";

			printf("\n<!-- Start htmlAliasPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlAliasPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlAliasPage()


void funcAliasNavList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uResults=0;

	interface_ExtSelectSearch("tAlias","tAlias.uAlias,tAlias.cUser,tAlias.cTargetEmail","tAlias.cUser",
			cSearch,NULL," LIMIT 20");
//			printf("%s",gcQuery);return;
	macro_mySQLRunAndStore(res);

	uResults=mysql_num_rows(res);

	if(uResults==1)
	{
		if((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"<a href=\"mail2Admin.cgi?gcPage=Alias&uAlias=%s&cSearch=%s\" class=darkLink>%s:%s</a><br>\n",
				field[0]
				,cURLEncode(cSearch)
				,field[1]
				,field[2]
				);
			sscanf(field[0],"%u",&uAlias);
			mysql_free_result(res);

			LoadAlias();
			return;
		}
	}
	else if(uResults>1)
	{
		while((field=mysql_fetch_row(res)))
			fprintf(fp,"<a href=\"mail2Admin.cgi?gcPage=Alias&uAlias=%s&cSearch=%s\" class=darkLink>%s:%s</a><br>\n",
				field[0]
				,cURLEncode(cSearch)
				,field[1]
				,field[2]
				);
		if(uResults>=20)
			fprintf(fp,"<br>Only the first 20 records displayed, you might want to further refine your search.<br>\n");
	}
	else if(uResults==0)
		fprintf(fp,"No records found.");
	
	mysql_free_result(res);

}//void funcAliasNavList(FILE *fp)


void SetAliasFieldsOn(void)
{
	cUserStyle="type_fields";
	cTargetEmailStyle="type_fields";
	cuServerGroupStyle="type_fields";

}//void SetAliasFieldsOn(void)


unsigned ValidateAliasInput(void)
{
	if(!cTargetEmail[0])
	{
		SetAliasFieldsOn();
		cTargetEmailStyle="type_fields_req";
		gcMessage="<blink>Error:</blink> Target email is required";
		return(1);
	}
	if(!isalpha(cTargetEmail[0]))
	{
		SetAliasFieldsOn();
		cTargetEmailStyle="type_fields_req";
		gcMessage="<blink>Error:</blink> Target email must not start with a number";
		return(1);
	}

	if(strlen(cTargetEmail)<3)
	{
		SetAliasFieldsOn();
		cTargetEmailStyle="type_fields_req";
		gcMessage="<blink>Error:</blink> Target email must be at least 3 chars";
		return(1);
	}

	if(strstr(cTargetEmail,".."))
	{
		SetAliasFieldsOn();
		cTargetEmailStyle="type_fields_req";
		gcMessage="<blink>Error:</blink> Target email may not have two consecutive periods";
		return(1);
	}
	
	if(!cUser[0])
	{
		SetAliasFieldsOn();
		cUserStyle="type_fields_req";
		gcMessage="<blink>Error:</blink> Login is required";
		return(1);
	}
	if(!isalpha(cUser[0]))
	{
		SetAliasFieldsOn();
		cUserStyle="type_fields_req";
		gcMessage="<blink>Error:</blink> Login must not start with a number";
		return(1);
	}

	if(strlen(cUser)<3)
	{
		SetAliasFieldsOn();
		cUserStyle="type_fields_req";
		gcMessage="<blink>Error:</blink> Login must be at least 3 chars";
		return(1);
	}

	if(strstr(cUser,".."))
	{
		SetAliasFieldsOn();
		cUserStyle="type_fields_req";
		gcMessage="<blink>Error:</blink> Login may not have two consecutive periods";
		return(1);
	}

	if(!uServerGroup)
	{
		SetAliasFieldsOn();
		cuServerGroupStyle="type_fields_req";
		gcMessage="<blink>Error: </blink> Must select server group";
		return(1);
	}
	
	return(0);

}//unsigned ValidateAliasInput(void)


void InsertAlias(void)
{
	sprintf(gcQuery,"INSERT INTO tAlias SET cUser='%s',cTargetEmail='%s',uServerGroup=%u,uOwner=%u,"
			"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			cUser
			,cTargetEmail
			,uServerGroup
			,guCompany
			,guLoginClient);
	macro_mySQLQueryHTMLError;

	if((uAlias=mysql_insert_id(&gMysql)))
	{
		gcMessage="Alias added OK";
		unxsMailLog(uAlias,"tAlias","New");
	}
	else
	{
		gcMessage="<blink>Error:</blink> Failed to add domain. Contact support ASAP";
		return;
	}

}//void InsertAlias(void)


void UpdateAlias(void)
{
	sprintf(gcQuery,"UPDATE tAlias SET cUser='%s',cTargetEmail='%s',uServerGroup=%u,"
			"uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())",
			cUser
			,cTargetEmail
			,uServerGroup
			,guLoginClient);
	macro_mySQLQueryHTMLError;

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="Alias modified OK";
		unxsMailLog(uAlias,"tAlias","Mod");
	}
	else
	{
		gcMessage="<blink>Error:</blink> Failed to modify domain. Contact support ASAP";
		return;
	}

}//void UpdateAlias(void);


void DeleteAlias(void)
{
	sprintf(gcQuery,"DELETE FROM tAlias WHERE uAlias=%u",uAlias);
	macro_mySQLQueryHTMLError;

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="Alias deleted OK";
		unxsMailLog(uAlias,"tAlias","Del");
	}
	else
	{
		gcMessage="<blink>Error:</blink> Failed to delete domain. Contact support ASAP";
		return;
	}

}//void DeleteAlias(void)
