/*
FILE 
	vut.c
	svn ID removed
AUTHOR
	(C) 2008-2009 Hugo Urquiza for Unixservice
PURPOSE
	mail2Admin program file. Provides code 
	for VUT management
*/

#include "interface.h"


unsigned uVUT=0;
//cDomain: FQ Domain Name
static char cDomain[65]={""};
static char *cDomainStyle="type_fields_off";
//uServerGroup: ServerGroup account is on
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


#define VAR_LIST_tVUT "tVUT.uVUT,tVUT.cDomain,tVUT.uServerGroup,tVUT.uOwner,tVUT.uCreatedBy,tVUT.uCreatedDate,tVUT.uModBy,tVUT.uModDate"


//Local only
void SetVUTFieldsOn(void);
unsigned ValidateVUTInput(void);
void InsertVUT(void);
void UpdateVUT(void);
void DeleteVUT(void);

void SetVUTEntriesFieldsOn(void);

unsigned uVUTHasEntries(void);

void ProcessVUTVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uVUT"))
			sscanf(entries[i].val,"%u",&uVUT);
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
	}

}//void ProcessVUTVars(pentry entries[], int x)


void LoadVUT(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	//Row id defined by module globar uVUT
	ExtSelectRow("tVUT",VAR_LIST_tVUT,uVUT);
	macro_mySQLRunAndStore(res);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uVUT);
		sprintf(cDomain,"%.64s",field[1]);
		sscanf(field[2],"%u",&uServerGroup);		
		sscanf(field[3],"%u",&uOwner);
		sscanf(field[4],"%u",&uCreatedBy);
		sscanf(field[5],"%lu",&uCreatedDate);
		sscanf(field[6],"%u",&uModBy);
		sscanf(field[7],"%lu",&uModDate);
	}
	else
		gcMessage="<blink>Error: </blink> Could not load record!";

	mysql_free_result(res);

}//void LoadVUT(void)


void VUTGetHook(entry gentries[],int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uVUT"))
			sscanf(gentries[i].val,"%u",&uVUT);
		else if(!strcmp(gentries[i].name,"cSearch"))
			sprintf(cSearch,"%.32s",gentries[i].val);
	}
	
	if(uVUT)
		LoadVUT();
	else
	{
		//If we have only a singlt tVUT record,it wont properly load uVUT var value, unless we call this function before htmlVUT
		//Thus we opened a file pointer to /dev/null, else we would crash the applicaction
		FILE *fp;
		if((fp=fopen("/dev/null","w")))
			funcVUTNavList(fp);
	}
	htmlVUT();

}//void VUTGetHook(entry gentries[],int x)


void VUTCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"VUT"))
	{
		ProcessVUTVars(entries,x);
		if(!strcmp(gcFunction,"New"))
		{
			if(guPermLevel>=10)
			{
				sprintf(gcNewStep,"Confirm ");
				gcInputStatus[0]=0;
				SetVUTFieldsOn();
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Confirm New"))
		{
			if(guPermLevel>=10)
			{
				if(ValidateVUTInput())
				{
					sprintf(gcNewStep,"Confirm ");
					gcInputStatus[0]=0;
					htmlVUT();
				}
				InsertVUT();

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
				SetVUTFieldsOn();
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Confirm Modify"))
		{
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(ValidateVUTInput())
				{
					sprintf(gcModStep,"Confirm ");
					gcInputStatus[0]=0;
					htmlVUT();
				}
				UpdateVUT();
			
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Delete"))
		{
			if(uAllowDel(uOwner,uCreatedBy))
			{
				if(!(uVUTHasEntries()))
				{
					sprintf(gcDelStep,"Confirm ");
					gcMessage="Double check you have selected the correct record to delete. Then confirm. Any other action to cancel.";
				}
				else
					gcMessage="<blink>Error:</blink> Can't delete VUT if it has entries";
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Confirm Delete"))
		{
			if(uAllowDel(uOwner,uCreatedBy))
			{
				if(!(uVUTHasEntries()))
					DeleteVUT();
				else
					gcMessage="<blink>Error:</blink> Can't delete VUT if it has entries";
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Add VUT Entry"))
		{
			sprintf(gcNewStep,"Confirm ");
			gcInputStatus[0]=0;
			SetVUTEntriesFieldsOn();
			htmlVUTEntries();
		}
		htmlVUT();
	}

}//void VUTCommands(pentry entries[], int x)


void htmlVUT(void)
{
	htmlHeader("unxsMail System","Header");
	htmlVUTPage("unxsMail System","VUT.Body");
	htmlFooter("Footer");

}//void htmlVUT(void)


void htmlVUTPage(char *cTitle, char *cTemplateName)
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
			char cuVUT[16]={""};


			sprintf(cuVUT,"%u",uVUT);
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

			template.cpName[20]="uVUT";
			template.cpValue[20]=cuVUT;

			template.cpName[21]="";

			printf("\n<!-- Start htmlVUTPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlVUTPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlVUTPage()


void funcVUTNavList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uResults=0;
	
	interface_ExtSelectSearch("tVUT","tVUT.uVUT,tVUT.cDomain","tVUT.cDomain",
			cSearch,NULL,"LIMIT 20");
	macro_mySQLRunAndStore(res);

	uResults=mysql_num_rows(res);

	if(uResults==1)
	{
		if((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"<a href=\"mail2Admin.cgi?gcPage=VUT&uVUT=%s&cSearch=%s\" class=darkLink>%s</a><br>\n",
				field[0]
				,cURLEncode(cSearch)
				,field[1]
				);
			sscanf(field[0],"%u",&uVUT);
			mysql_free_result(res);
			LoadVUT();
			return;
		}
	}
	else if(uResults>1)
	{
		while((field=mysql_fetch_row(res)))
			fprintf(fp,"<a href=\"mail2Admin.cgi?gcPage=VUT&uVUT=%s&cSearch=%s\" class=darkLink>%s</a><br>\n",
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

}//void funcVUTNavList(FILE *fp)


void SetVUTFieldsOn(void)
{
	cDomainStyle="type_fields";
	cuServerGroupStyle="type_fields";

}//void SetVUTFieldsOn(void)


unsigned ValidateVUTInput(void)
{
	if(!cDomain[0])
	{
		SetVUTFieldsOn();
		cDomainStyle="type_fields_req";
		gcMessage="<blink>Error: </blink> Must enter domain name";
		return(1);
	}
	else
	{
		MYSQL_RES *res;
		sprintf(gcQuery,"SELECT uDomain FROM tDomain WHERE cDomain='%s'",TextAreaSave(cDomain));
		macro_mySQLRunAndStore(res);
		if(!mysql_num_rows(res))
		{
			mysql_free_result(res);
			SetVUTFieldsOn();
			cDomainStyle="type_fields_req";
			gcMessage="<blink>Error: </blink> Domain name does not exist";
			return(1);
		}
		
		mysql_free_result(res);

	}

	if(!uServerGroup)
	{
		SetVUTFieldsOn();
		cuServerGroupStyle="type_fields_req";
		gcMessage="<blink>Error: </blink> Must select server group";
		return(1);
	}

	return(0);

}//unsigned ValidateVUTInput(void)


void InsertVUT(void)
{
	sprintf(gcQuery,"INSERT INTO tVUT SET cDomain='%s',uServerGroup=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			TextAreaSave(cDomain)
			,uServerGroup
			,guCompany
			,guLoginClient);
	macro_mySQLQueryHTMLError;

	if((uVUT=mysql_insert_id(&gMysql)))
	{
		gcMessage="VUT added OK";
		unxsMailLog(uVUT,"tVUT","New");
	}
	else
	{
		gcMessage="<blink>Error:</blink> Failed to add domain. Contact support ASAP";
		return;
	}

	//Lastly, we set the required ism3 fields, in case the user tries to modify the record after adding it
	//(Otherwise, the user will get the 'Denied by permission settings' error message.
	uOwner=guCompany;
	uCreatedBy=guLoginClient;

}//void InsertVUT(void)


void UpdateVUT(void)
{
	sprintf(gcQuery,"UPDATE tVUT SET cDomain='%s',uServerGroup=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uVUT=%u",
			TextAreaSave(cDomain)
			,uServerGroup
			,guLoginClient
			,uVUT);
	macro_mySQLQueryHTMLError;

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="VUT modified OK";
		unxsMailLog(uVUT,"tVUT","Mod");
	}
	else
	{
		gcMessage="<blink>Error:</blink> Failed to modify domain. Contact support ASAP";
		return;
	}

}//void UpdateVUT(void);


void DeleteVUT(void)
{
	sprintf(gcQuery,"DELETE FROM tVUT WHERE uVUT=%u",uVUT);
	macro_mySQLQueryHTMLError;

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="VUT deleted OK";
		unxsMailLog(uVUT,"tVUT","Del");
	}
	else
	{
		gcMessage="<blink>Error:</blink> Failed to delete domain. Contact support ASAP";
		return;
	}

}//void DeleteVUT(void)


unsigned uVUTHasEntries(void)
{
	MYSQL_RES *res;
	unsigned uI=0;
	
	sprintf(gcQuery,"SELECT uVUTEntries FROM tVUTEntries WHERE uVUT=%u",uVUT);
	macro_mySQLRunAndStore(res);

	uI=mysql_num_rows(res);
	mysql_free_result(res);

	return(uI);

}//void unsigned uVUTHasEntries(void)

