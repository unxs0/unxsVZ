/*
FILE 
	domain.c
	svn ID removed
AUTHOR
	(C) 2008-2009 Hugo Urquiza for Unixservice
PURPOSE
	mail2Admin program file. Provides code 
	for domain.management
*/

#include "interface.h"



//Table Variables
//uDomain: Primary Key
static unsigned uDomain=0;
//cDomain: FQ Domain Name
static char cDomain[65]={""};
static char *cDomainStyle="type_fields_off";
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

static unsigned uLocal=0;

static char cSearch[33]={""};

extern unsigned uServerGroup;
extern char *cuServerGroupStyle;

#define VAR_LIST_tDomain "tDomain.uDomain,tDomain.cDomain,tDomain.uOwner,tDomain.uCreatedBy,tDomain.uCreatedDate,tDomain.uModBy,tDomain.uModDate,(SELECT tLocal.uLocal FROM tLocal WHERE cDomain=tDomain.cDomain),(SELECT tLocal.uServerGroup FROM tLocal WHERE tLocal.cDomain=tDomain.cDomain)"

 //Local only
void SetDomainFieldsOn(void);
unsigned ValidateDomainInput(void);
void InsertDomain(void);
void UpdateDomain(void);
void DeleteDomain(void);

void ProcessDomainVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uDomain"))
			sscanf(entries[i].val,"%u",&uDomain);
		else if(!strcmp(entries[i].name,"cDomain"))
			sprintf(cDomain,"%.64s",FQDomainName(entries[i].val));
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
		else if(!strcmp(entries[i].name,"uServerGroup"))
			sscanf(entries[i].val,"%u",&uServerGroup);
		else if(!strcmp(entries[i].name,"uLocal"))
			sscanf(entries[i].val,"%u",&uLocal);
	}

}//void ProcessDomainVars(pentry entries[], int x)



void LoadDomain(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	//Row id defined by module globar uDomain
	ExtSelectRow("tDomain",VAR_LIST_tDomain,uDomain);
	macro_mySQLRunAndStore(res);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uDomain);
		sprintf(cDomain,"%.64s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);
		sscanf(field[7],"%u",&uLocal);
		sscanf(field[8],"%u",&uServerGroup);
	}
	else
		gcMessage="<blink>Error: </blink> Could not load record!";

	mysql_free_result(res);
}


void DomainGetHook(entry gentries[],int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uDomain"))
			sscanf(gentries[i].val,"%u",&uDomain);
		else if(!strcmp(gentries[i].name,"cSearch"))
			sprintf(cSearch,"%.32s",gentries[i].val);
	}
	
	if(uDomain)
		LoadDomain();
	else
	{
		FILE *fp;
		if((fp=fopen("/dev/null","w")))
			funcDomainNavList(fp);
	}

	
	htmlDomain();

}//void DomainGetHook(entry gentries[],int x)


void DomainCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Domain"))
	{
		char cJobData[100]={""};
		char cServerGroup[33]={""};

		ProcessDomainVars(entries,x);
		if(!strcmp(gcFunction,"New"))
		{
			if(guPermLevel>=10)
			{
				sprintf(gcNewStep,"Confirm ");
				gcInputStatus[0]=0;
				gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
				SetDomainFieldsOn();
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Confirm New"))
		{
			if(guPermLevel>=10)
			{
				if(ValidateDomainInput())
				{
					sprintf(gcNewStep,"Confirm ");
					gcInputStatus[0]=0;
					htmlDomain();
				}
				InsertDomain();

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uLocal=%u;\ncDomain=%s;\n",
						uLocal,cDomain);
				SubmitJob("NewLocal",cDomain,"",cServerGroup,cJobData,0,0,
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
				SetDomainFieldsOn();
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Confirm Modify"))
		{
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(ValidateDomainInput())
				{
					sprintf(gcModStep,"Confirm ");
					gcInputStatus[0]=0;
					htmlDomain();
				}
				UpdateDomain();
			
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uLocal=%u;\ncDomain=%s;\n",
						uLocal,cDomain);
				SubmitJob("ModLocal",cDomain,"",cServerGroup,cJobData,0,0,
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
				DeleteDomain();

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uLocal=%u;\ncDomain=%s;\n",
						uLocal,cDomain);
				SubmitJob("DelLocal",cDomain,"",cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		htmlDomain();
	}

}//void DomainCommands(pentry entries[], int x)


void htmlDomain(void)
{
	htmlHeader("unxsMail System","Header");
	htmlDomainPage("unxsMail System","Domain.Body");
	htmlFooter("Footer");

}//void htmlDomain(void)


void htmlDomainPage(char *cTitle, char *cTemplateName)
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
			char cuDomain[16]={""};
			char cuLocal[16]={""};

			sprintf(cuDomain,"%u",uDomain);
			sprintf(cuLocal,"%u",uLocal);
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

			template.cpName[12]="cDomain";
			template.cpValue[12]=cDomain;
			
			template.cpName[13]="cDomainStyle";
			template.cpValue[13]=cDomainStyle;

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

			template.cpName[20]="uDomain";
			template.cpValue[20]=cuDomain;

			template.cpName[21]="uLocal";
			template.cpValue[21]=cuLocal;

			template.cpName[22]="";

			printf("\n<!-- Start htmlDomainPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlDomainPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlDomainPage()


void funcDomainNavList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uResults=0;

	interface_ExtSelectSearch("tDomain","tDomain.uDomain,tDomain.cDomain","tDomain.cDomain",
			cSearch,NULL," LIMIT 20");
	macro_mySQLRunAndStore(res);

	uResults=mysql_num_rows(res);

	if(uResults==1)
	{
		if((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"<a href=\"mail2Admin.cgi?gcPage=Domain&uDomain=%s&cSearch=%s\" class=darkLink>%s</a><br>\n",
				field[0]
				,cURLEncode(cSearch)
				,field[1]
				);
			sscanf(field[0],"%u",&uDomain);
			mysql_free_result(res);

			LoadDomain();
			return;
		}
	}
	else if(uResults>1)
	{
		while((field=mysql_fetch_row(res)))
			fprintf(fp,"<a href=\"mail2Admin.cgi?gcPage=Domain&uDomain=%s&cSearch=%s\" class=darkLink>%s</a><br>\n",
				field[0]
				,cURLEncode(cSearch)
				,field[1]
				);
		if(uResults>=20)
			fprintf(fp,"<br>Only the first 20 records displayed, you might want to further refine your search.<br>\n");
	}
	else if(uResults==0)
		fprintf(fp,"No records found.");
	
	mysql_free_result(res);

}//void funcDomainNavList(FILE *fp)


void SetDomainFieldsOn(void)
{
	cDomainStyle="type_fields";
	cuServerGroupStyle="type_fields";

}//void SetDomainFieldsOn(void)


unsigned ValidateDomainInput(void)
{
	if(!cDomain[0])
	{
		SetDomainFieldsOn();
		cDomainStyle="type_fields_req";
		gcMessage="<blink>Error: </blink> Must enter domain name";
		return(1);
	}
	else
	{
		if(!strcmp(gcFunction,"Confirm New"))
		{
			MYSQL_RES *res;
			sprintf(gcQuery,"SELECT uDomain FROM tDomain WHERE cDomain='%s'",TextAreaSave(cDomain));
			macro_mySQLRunAndStore(res);
			if(mysql_num_rows(res))
			{
				mysql_free_result(res);
				SetDomainFieldsOn();
				cDomainStyle="type_fields_req";
				gcMessage="<blink>Error: </blink> Domain name already in use";
				return(1);
			}
			mysql_free_result(res);
		}
	}
	
	if(!uServerGroup)
	{
		SetDomainFieldsOn();
		cuServerGroupStyle="type_fields_req";
		gcMessage="<blink>Error: </blink> Must select server group";
		return(1);
	}

	return(0);

}//unsigned ValidateDomainInput(void)


void InsertDomain(void)
{
	sprintf(gcQuery,"INSERT INTO tDomain SET cDomain='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			TextAreaSave(cDomain)
			,guCompany
			,guLoginClient);
	macro_mySQLQueryHTMLError;

	if((uDomain=mysql_insert_id(&gMysql)))
	{
		gcMessage="Domain added OK";
		unxsMailLog(uDomain,"tDomain","New");
	}
	else
	{
		gcMessage="<blink>Error:</blink> Failed to add domain. Contact support ASAP";
		return;
	}

	sprintf(gcQuery,"INSERT INTO tLocal SET cDomain='%s',uServerGroup=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			TextAreaSave(cDomain)
			,uServerGroup
			,guCompany
			,guLoginClient);
	macro_mySQLQueryHTMLError;

	if((uLocal=mysql_insert_id(&gMysql)))
	{
		gcMessage="Domain added OK";
		unxsMailLog(uLocal,"tLocal","New");
	}

	//Lastly, we set the required ism3 fields, in case the user tries to modify the record after adding it
	//(Otherwise, the user will get the 'Denied by permission settings' error message.
	uOwner=guCompany;
	uCreatedBy=guLoginClient;

}//void InsertDomain(void)


void UpdateDomain(void)
{
	sprintf(gcQuery,"UPDATE tDomain SET cDomain='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uDomain=%u",
			TextAreaSave(cDomain)
			,guLoginClient
			,uDomain);
	macro_mySQLQueryHTMLError;

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="Domain modified OK";
		unxsMailLog(uDomain,"tDomain","Mod");
	}
	else
	{
		gcMessage="<blink>Error:</blink> Failed to modify domain. Contact support ASAP";
		return;
	}

	sprintf(gcQuery,"UPDATE tLocal SET cDomain='%s',uServerGroup=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uLocal=%u",
			TextAreaSave(cDomain)
			,uServerGroup
			,guLoginClient
			,uLocal);
	macro_mySQLQueryHTMLError;

	if(mysql_affected_rows(&gMysql))
	{
		unxsMailLog(uLocal,"tLocal","Mod");
		gcMessage="Domain modified OK";
	}
	else
		gcMessage="tLocal not updated";


}//void UpdateDomain(void);


void DeleteDomain(void)
{
	sprintf(gcQuery,"DELETE FROM tDomain WHERE uDomain=%u",uDomain);
	macro_mySQLQueryHTMLError;

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="Domain deleted OK";
		unxsMailLog(uDomain,"tDomain","Del");
	}
	else
	{
		gcMessage="<blink>Error:</blink> Failed to delete domain. Contact support ASAP";
		return;
	}

	sprintf(gcQuery,"DELETE FROM tLocal WHERE uLocal=%u",uLocal);
	macro_mySQLQueryHTMLError;

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="Domain deleted OK";
		unxsMailLog(uLocal,"tLocal","Del");
	}
	else
		gcMessage="tLocal not deleted";

}//void DeleteDomain(void)

