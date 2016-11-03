/*
FILE 
	vutentries.c
	svn ID removed
AUTHOR
	(C) 2008-2009 Hugo Urquiza for Unixservice
PURPOSE
	mail2Admin program file. Provides code 
	for VUT entries management
*/

#include "interface.h"


extern unsigned uVUT;
static unsigned uVUTEntries=0;
static char cVirtualEmail[33]={""};
static char *cVirtualEmailStyle="type_fields_off";
//cTargetEmail: Target email address with @FQDN
static char cTargetEmail[101]={""};
static char *cTargetEmailStyle="type_fields_off";
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


#define VAR_LIST_tVUTEntries "tVUTEntries.uVUTEntries,tVUTEntries.uVUT,tVUTEntries.cVirtualEmail,tVUTEntries.cTargetEmail,tVUTEntries.uOwner,tVUTEntries.uCreatedBy,tVUTEntries.uCreatedDate,tVUTEntries.uModBy,tVUTEntries.uModDate"

void LoadVUT(void);

//Local only
void SetVUTEntriesFieldsOn(void);
unsigned ValidateVUTEntriesInput(void);
void InsertVUTEntries(void);
void UpdateVUTEntries(void);
void DeleteVUTEntries(void);


void ProcessVUTEntriesVars(pentry entries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uVUT"))
			sscanf(entries[i].val,"%u",&uVUT);
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
		else if(!strcmp(entries[i].name,"uVUTEntries"))
			sscanf(entries[i].val,"%u",&uVUTEntries);
		else if(!strcmp(entries[i].name,"cVirtualEmail"))
		{
			if(strncmp(entries[i].val,"DEFAULT",7))
				sprintf(cVirtualEmail,"%.32s",WordToLower(entries[i].val));
			else
				sprintf(cVirtualEmail,"DEFAULT");
		}
		else if(!strcmp(entries[i].name,"cTargetEmail"))
			sprintf(cTargetEmail,"%.100s",entries[i].val);
	}

}//void ProcessVUTVars(pentry entries[], int x)


void LoadVUTEntries(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	//Row id defined by module globar uVUTEntries
	ExtSelectRow("tVUTEntries",VAR_LIST_tVUTEntries,uVUTEntries);
	macro_mySQLRunAndStore(res);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uVUTEntries);
		sscanf(field[1],"%u",&uVUT);
		sprintf(cVirtualEmail,"%.32s",field[2]);
		sprintf(cTargetEmail,"%.100s",field[3]);
		sscanf(field[4],"%u",&uOwner);
		sscanf(field[5],"%u",&uCreatedBy);
		sscanf(field[6],"%lu",&uCreatedDate);
		sscanf(field[7],"%u",&uModBy);
		sscanf(field[8],"%lu",&uModDate);

	}

	mysql_free_result(res);

}//void LoadVUTEntries(void)


void VUTEntriesGetHook(entry gentries[],int x)
{
	register int i;
	char cAction[33]={""};

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uVUT"))
			sscanf(gentries[i].val,"%u",&uVUT);
		else if(!strcmp(gentries[i].name,"uVUTEntries"))
			sscanf(gentries[i].val,"%u",&uVUTEntries);
		else if(!strcmp(gentries[i].name,"cSearch"))
			sprintf(cSearch,"%.32s",gentries[i].val);
		else if(!strcmp(gentries[i].name,"cAction"))
			sprintf(cAction,"%.32s",gentries[i].val);
	}
	if(cAction[0] && uVUT && uVUTEntries)
	{
		if(!strcmp(cAction,"Delete"))
		{
			DeleteVUTEntries();
			LoadVUT();
			htmlVUT();
		}
		else if(!strcmp(cAction,"Edit"))
		{
			sprintf(gcModStep,"Confirm ");
			gcInputStatus[0]=0;
			SetVUTEntriesFieldsOn();
		}
	}
	
	if(uVUT && uVUTEntries) LoadVUTEntries();

	htmlVUTEntries();

}//void VUTEntriesGetHook(entry gentries[],int x)


void VUTEntriesCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"VUTEntries"))
	{
		char cServerGroup[100]={""};
		char cDomain[100]={""};
		char cJobData[100]={""};
		unsigned uServerGroup=0;

		ProcessVUTEntriesVars(entries,x);

		if(!strcmp(gcFunction,"New"))
		{
			if(guPermLevel>=10)
			{
				sprintf(gcNewStep,"Confirm ");
				gcInputStatus[0]=0;
				gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
				SetVUTEntriesFieldsOn();
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Confirm New"))
		{
			if(guPermLevel>=10)
			{
				if(ValidateVUTEntriesInput())
				{
					sprintf(gcNewStep,"Confirm ");
					gcInputStatus[0]=0;
					gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
					htmlVUTEntries();
				}
				InsertVUTEntries();

				sscanf(ForeignKey("tVUT","uServerGroup",uVUT),"%u",&uServerGroup);
				sprintf(cDomain,ForeignKey("tVUT","cDomain",uVUT));
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uVUTEntries=%u;\ncVirtualEmail=%s;\ncTargetEmail=%s;\ncDomain=%s;\n"
						,uVUTEntries,cVirtualEmail,cTargetEmail,cDomain);
				SubmitJob("NewVUT",cDomain,cVirtualEmail,cServerGroup,cJobData,0,0,
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
				SetVUTEntriesFieldsOn();
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}
		else if(!strcmp(gcFunction,"Confirm Modify"))
		{
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(ValidateVUTEntriesInput())
				{
					sprintf(gcModStep,"Confirm ");
					gcInputStatus[0]=0;
					gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
					htmlVUTEntries();
				}
				UpdateVUTEntries();
			
				sscanf(ForeignKey("tVUT","uServerGroup",uVUT),"%u",&uServerGroup);
				sprintf(cDomain,ForeignKey("tVUT","cDomain",uVUT));
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uVUTEntries=%u;\ncVirtualEmail=%s;\ncTargetEmail=%s;\ncDomain=%s;\n"
						,uVUTEntries,cVirtualEmail,cTargetEmail,cDomain);
				SubmitJob("ModVUT",cDomain,cVirtualEmail,cServerGroup,cJobData,0,0,
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
				DeleteVUTEntries();

				sscanf(ForeignKey("tVUT","uServerGroup",uVUT),"%u",&uServerGroup);
				sprintf(cDomain,ForeignKey("tVUT","cDomain",uVUT));
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uVUTEntries=%u;\ncVirtualEmail=%s;\ncTargetEmail=%s;\ncDomain=%s;\n"
						,uVUTEntries,cVirtualEmail,cTargetEmail,cDomain);
				SubmitJob("DelVUT",cDomain,cVirtualEmail,cServerGroup,cJobData,0,0,
								guLoginClient,guLoginClient);
			}
			else
				gcMessage="<blink>Error:</blink> Denied by permission settings";
		}

		htmlVUTEntries();
	}

}//void VUTEntriesCommands(pentry entries[], int x)


void htmlVUTEntries(void)
{
	htmlHeader("unxsMail System","Header");
	htmlVUTEntriesPage("unxsMail System","VUTEntries.Body");
	htmlFooter("Footer");

}//void htmlVUTEntries(void)


void htmlVUTEntriesPage(char *cTitle, char *cTemplateName)
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
			char cuVUTEntries[16]={""};

			sprintf(cuVUTEntries,"%u",uVUTEntries);
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

			template.cpName[12]="uVUTEntries";
			template.cpValue[12]=cuVUTEntries;

			template.cpName[13]="cTargetEmailStyle";
			template.cpValue[13]=cTargetEmailStyle;

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

			template.cpName[21]="cVirtualEmail";
			template.cpValue[21]=cVirtualEmail;

			template.cpName[22]="cVirtualEmailStyle";
			template.cpValue[22]=cVirtualEmailStyle;

			template.cpName[23]="cTargetEmail";
			template.cpValue[23]=cTargetEmail;

			template.cpName[24]="";

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


void funcVUTEntriesNavList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uResults=0;

	if(!cSearch[0]) return;

	interface_ExtSelectSearch("tVUTEntries","tVUTEntries.uVUTEntries,tVUTEntries.cVUTEntries","tVUTEntries.cVUTEntries",
			cSearch,NULL," LIMIT 20");
	macro_mySQLRunAndStore(res);

	uResults=mysql_num_rows(res);

	if(uResults==1)
	{
		if((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"<a href=\"mail2Admin.cgi?gcPage=VUTEntries&uVUTEntries=%s&cSearch=%s\" class=darkLink>%s</a><br>\n",
				field[0]
				,cURLEncode(cSearch)
				,field[1]
				);
			sscanf(field[0],"%u",&uVUTEntries);
			mysql_free_result(res);

			LoadVUTEntries();
			return;
		}
	}
	else if(uResults>1)
	{
		while((field=mysql_fetch_row(res)))
			fprintf(fp,"<a href=\"mail2Admin.cgi?gcPage=VUTEntries&uVUTEntries=%s&cSearch=%s\" class=darkLink>%s</a><br>\n",
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

}//void funcVUTEntriesNavList(FILE *fp)


void funcVUTEntries(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	if(!uVUT) return;

	sprintf(gcQuery,"SELECT uVUTEntries,cVirtualEmail,cTargetEmail FROM tVUTEntries WHERE uVUT=%u ORDER BY cVirtualEmail",uVUT);
	macro_mySQLRunAndStore(res);

	if(!mysql_num_rows(res))
	{
		mysql_free_result(res);
		return;
	}

	while((field=mysql_fetch_row(res)))
		fprintf(fp,"<tr><td>%s</td><td>%s</td>"
			"<td><a class=darkLink href=mail2Admin.cgi?gcPage=VUTEntries&uVUTEntries=%s&uVUT=%u&cAction=Edit>Edit</a></td>"
			"<td><a onClick=\"return confirm('Are you sure you wish to delete this entry?');\" "
			"class=darkLink href=mail2Admin.cgi?gcPage=VUTEntries&uVUTEntries=%s&uVUT=%u&cAction=Delete>Delete</a></td></tr>\n",
			field[1]
			,field[2]
			,field[0]
			,uVUT
			,field[0]
			,uVUT);

	mysql_free_result(res);

}//void funcVUTEntries(FILE *fp);


void SetVUTEntriesFieldsOn(void)
{
	cVirtualEmailStyle="type_fields";
	cTargetEmailStyle="type_fields";

}//void SetVUTEntriesFieldsOn(void)


unsigned ValidateVUTEntriesInput(void)
{
	MYSQL_RES *res;
	char cDomain[100];
	char *cp;
	char *cp2;
	unsigned uServerGroup=0;
	register int x;

	if(!uVUT)
	{
		gcMessage="No uVUT. Contact support";
		return(1);
	}

	if(!cVirtualEmail[0])
	{
		SetVUTEntriesFieldsOn();
		cVirtualEmailStyle="type_fields_req";
		gcMessage="<blink>Error:</blink> Must enter virtual email";
		return(1);
	}
	else
	{
		for (x=0;x<strlen(cVirtualEmail);x++)
		{
			if(!isalnum(cVirtualEmail[x]) && cVirtualEmail[x]!='.')
			{
				SetVUTEntriesFieldsOn();
				cVirtualEmailStyle="type_fields_req";
				gcMessage="<blink>Error:</blink> Invalid chars in virtual email, valid (a-z) and '.'";
				return(1);
			}
		}
	}

	if(!cTargetEmail[0])
	{
		SetVUTEntriesFieldsOn();
		cTargetEmailStyle="type_fields_req";
		gcMessage="<blink>Error:</blink> Must target email";
		return(1);
	}
	for (x=0;x<strlen(cVirtualEmail);x++)
	{
		if(!isalnum(cVirtualEmail[x]) && cVirtualEmail[x]!='.' && cVirtualEmail[x]!='@')
		{
			SetVUTEntriesFieldsOn();
			cTargetEmailStyle="type_fields_req";
			gcMessage="<blink>Error:</blink> Invalid chars in virtual email, valid (a-z), ',' and '@'";
			return(1);
		}
	}
	
	
	sscanf(ForeignKey("tVUT","uServerGroup",uVUT),"%u",&uServerGroup);

	if(!(cp=strchr(cTargetEmail,'@')))
	{
		//Allow special error:nouser lines for DEFAULT only
		if(!strcmp(cVirtualEmail,"DEFAULT") && !strncmp(cTargetEmail,"error:nouser",12))
			return(0);

		//Check tAliases
		sprintf(gcQuery,"SELECT uAlias FROM tAlias WHERE cUser='%s' AND uServerGroup=%u",cTargetEmail,uServerGroup);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
        	res=mysql_store_result(&gMysql);
		if(!mysql_num_rows(res))
		{

			//Check tUser
			sprintf(gcQuery,"SELECT uUser FROM tUser WHERE cLogin='%s' AND uServerGroup=%u",cTargetEmail,uServerGroup);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			if(!mysql_num_rows(res))
			{
				SetVUTEntriesFieldsOn();
				cTargetEmailStyle="type_fields_req";
				gcMessage="<blink>Error:</blink> Target email not in tAlias or tUser.";
				return(1);
			}
		}

	}
	else
	{
		if(strlen(cTargetEmail)<6)
		{
			SetVUTEntriesFieldsOn();
			cTargetEmailStyle="type_fields_req";
			gcMessage="<blink>Error:</blink> Target email is too short to be a valid email";
			return(1);
		}
		cp++;
		if(*cp==0)
		{
			SetVUTEntriesFieldsOn();
			cTargetEmailStyle="type_fields_req";
			gcMessage="<blink>Error:</blink> Target email must not end with @";
			return(1);
		}
		if(!(cp2=strchr(cp,'.')))
		{
			SetVUTEntriesFieldsOn();
			cTargetEmailStyle="type_fields_req";
			gcMessage="<blink>Error:</blink> Target email must have a '.' after the '@'";
			return(1);
		}
		if(strlen(cp)<4)
		{
			SetVUTEntriesFieldsOn();
			cTargetEmailStyle="type_fields_req";
			gcMessage="<blink>Error:</blink> Target email after '@' part is too short";
			return(1);
		}
		cp2++;
		if(*cp2==0)
		{
			SetVUTEntriesFieldsOn();
			cTargetEmailStyle="type_fields_req";
			gcMessage="<blink>Error:</blink> Target email must not end with .";
			return(1);
		}
		if(strlen(cp2)<2)
		{
			SetVUTEntriesFieldsOn();
			cTargetEmailStyle="type_fields_req";
			gcMessage="<blink>Error:</blink> Target email after '@' and '.' part is too short";
			return(1);
		}
	}

	if((cp=strchr(cTargetEmail,'@')))
	{
		strncpy(cDomain,ForeignKey("tVUT","cDomain",uVUT),99);
		cDomain[99]=0;
		if(!strcasecmp(cp+1,cDomain))
		{
			SetVUTEntriesFieldsOn();
			cTargetEmailStyle="type_fields_req";
			gcMessage="<blink>Error:</blink> Target email must not be tVUT.cDomain";
			return(1);
		}
	}
        else
        {
                sprintf(gcQuery,"SELECT uUser from tUser WHERE cLogin='%s' AND uServerGroup=%u",cTargetEmail,uServerGroup);
                mysql_query(&gMysql,gcQuery);
                if(mysql_errno(&gMysql))
                        htmlPlainTextError(mysql_error(&gMysql));
                res=mysql_store_result(&gMysql);
                if(!mysql_num_rows(res))
                {

                        sprintf(gcQuery,"SELECT uAlias from tAlias WHERE cUser='%s' AND uServerGroup=%u",cTargetEmail,uServerGroup);
                        mysql_query(&gMysql,gcQuery);
                        if(mysql_errno(&gMysql))
                                htmlPlainTextError(mysql_error(&gMysql));
                        res=mysql_store_result(&gMysql);
                        if(!mysql_num_rows(res))
                        {
                                mysql_free_result(res);
				SetVUTEntriesFieldsOn();
				cTargetEmailStyle="type_fields_req";
				gcMessage="<blink>Error:</blink> Missing '@' in cTargetEmail. Or cTargetEmail not in tUser or tAlias";
				return(1);
                        }
                }
                mysql_free_result(res);
        }
	
	return(0);

}//unsigned ValidateVUTEntriesInput(void)

void InsertVUTEntries(void)
{
	sprintf(gcQuery,"INSERT INTO tVUTEntries SET uVUT=%u,cVirtualEmail='%s',"
			"cTargetEmail='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uVUT
			,TextAreaSave(cVirtualEmail)
			,TextAreaSave(cTargetEmail)
			,guCompany
			,guLoginClient
		);
	macro_mySQLQueryHTMLError;

	if((uVUTEntries=mysql_insert_id(&gMysql)))
	{
		unxsMailLog(uVUTEntries,"tVUTEntries","New");
		gcMessage="VUT entry created  OK";
	}
	else
		gcMessage="VUT entry not created. Contact support";

	//Lastly, we set the required ism3 fields, in case the user tries to modify the record after adding it
	//(Otherwise, the user will get the 'Denied by permission settings' error message.
	uOwner=guCompany;
	uCreatedBy=guLoginClient;

}//void InsertVUTEntries(void)


void UpdateVUTEntries(void)
{
	sprintf(gcQuery,"UPDATE tVUTEntries SET cVirtualEmail='%s',"
			"cTargetEmail='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uVUTEntries=%u",
			TextAreaSave(cVirtualEmail)
			,TextAreaSave(cTargetEmail)
			,guLoginClient
			,uVUTEntries
		);
	macro_mySQLQueryHTMLError;
	
	if(mysql_affected_rows(&gMysql))
	{
		unxsMailLog(uVUTEntries,"tVUTEntries","Mod");
		gcMessage="VUT entry modified OK";
	}
	else
		gcMessage="VUT entry not modified. Contact support";

}//void UpdateVUTEntries(void)



void DeleteVUTEntries(void)
{
	sprintf(gcQuery,"DELETE FROM tVUTEntries WHERE uVUTEntries=%u",uVUTEntries);
	macro_mySQLQueryHTMLError;

	if(mysql_affected_rows(&gMysql))
	{
		unxsMailLog(uVUTEntries,"tVUTEntries","Del");
		gcMessage="VUT entry deleted OK";
	}
	else
		gcMessage="VUT entry not deleted. Contact support";

}//void DeleteVUTEntries(void)

