/*
FILE 
	customer.c
	$Id: customer.c 729 2009-03-12 19:50:23Z hus-admin $
AUTHOR
	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	iDNS Admin (Owner) Interface
	program file.
*/


#define BO_ADMIN 	"Backend Admin"
#define BO_ROOT 	"Backend Root"

#include "interface.h"

static unsigned uClient=0;

static char cCompanyName[100]={""};
static char *cLabelStyle="type_fields_off";

static char cEmail[100]={""};
static char *cEmailStyle="type_fields_off";

static char cInfo[513]={""};
static char *cInfoStyle="type_textarea_off";

static char cCode[100]={""};
static char *cCodeStyle="type_fields_off";

static char cNavList[8192]={"No results."};

static char cSearch[100]={""};
static char *cSearchStyle="type_fields";
static unsigned uStep=0;

static unsigned uOwner=0;
static unsigned uCreatedBy=0;
static time_t uCreatedDate=0;
static unsigned uModBy=0;
static time_t uModDate=0;

//
//Local only
unsigned ValidateCustomerInput(void);
void SelectCustomer(char *cLabel, unsigned uMode);
void NewCustomer(void);
void DelCustomer(void);
void ModCustomer(void);

unsigned uHasZones(char *cLabel);
unsigned uHasBlocks(char *cLabel);
unsigned uHasContacts(char *cLabel);

void SetCustomerFieldsOn(void);

void LoadCustomer(void);
	

void ProcessCustomerVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cInfo"))
			sprintf(cInfo,"%.513s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cCompanyName,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"cEmail"))
			sprintf(cEmail,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCode"))
			sprintf(cCode,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"uClient"))
			sscanf(entries[i].val,"%u",&uClient);
		else if(!strcmp(entries[i].name,"uStep"))
			sscanf(entries[i].val,"%u",&uStep);
		else if(!strcmp(entries[i].name,"cNavList"))
			sprintf(cNavList,"%.8191s",entries[i].val);			
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
	}

}//void ProcessUserVars(pentry entries[], int x)


void CustomerGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uClient"))
			sscanf(gentries[i].val,"%u",&uClient);
	}

	if(uClient)
		LoadCustomer();
	else if(gcCustomer[0])
	{
		uClient=uGetClient(gcCustomer);
		LoadCustomer();
	}
	
	htmlCustomer();

}//void CustomerGetHook(entry gentries[],int x)


void LoadCustomer(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uClient,cLabel,cInfo,cEmail,cCode,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate FROM tClient WHERE uClient=%u",uClient);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uClient);
		sprintf(cCompanyName,"%.100s",field[1]);			
		sprintf(cInfo,"%.513s",field[2]);
		sprintf(cEmail,"%.100s",field[3]);
		sprintf(cCode,"%.100s",field[4]);
		sscanf(field[5],"%u",&uOwner);
		sscanf(field[6],"%u",&uCreatedBy);
		sscanf(field[7],"%lu",&uCreatedDate);
		sscanf(field[8],"%u",&uModBy);
		sscanf(field[9],"%lu",&uModDate);
		
		if(strcmp(gcCustomer,cCompanyName))
		{
			sprintf(gcCustomer,"%.99s",cCompanyName);
			gcZone[0]=0;
			cuView[0]=0;
			uResource=0;
			guContact=0;
			sys_SetSessionCookie();
		}
		gcMessage="1 record(s) found";
	}
	else
		gcMessage="<blink>No records found.</blink>";

	mysql_free_result(res);

}//void LoadCustomer(char *cuClient)


void CustomerCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Customer"))
	{
		ProcessCustomerVars(entries,x);
		if(!strcmp(gcFunction,"New"))
		{			
			sprintf(gcNewStep,"Confirm ");
			gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
			gcInputStatus[0]=0;
			SetCustomerFieldsOn();
		}
		else if(!strcmp(gcFunction,"Confirm New"))
		{
			if(!ValidateCustomerInput())
			{
				sprintf(gcNewStep,"Confirm ");
				gcInputStatus[0]=0;
				htmlCustomer();
			}
			NewCustomer();
		}
		else if(!strcmp(gcFunction,"Modify"))
		{
			sprintf(gcModStep,"Confirm ");
			gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
			SetCustomerFieldsOn();
			gcInputStatus[0]=0;
		}
		else if(!strcmp(gcFunction,"Confirm Modify"))
		{
			if(!uClient)
			{
				gcMessage="<blink>Can't modify. No record selected</blink>";
				htmlCustomer();
			}
			if(!ValidateCustomerInput())
			{
				sprintf(gcModStep,"Confirm ");
				gcInputStatus[0]=0;
				htmlCustomer();
			}
			ModCustomer();
		}
		else if(!strcmp(gcFunction,"Delete"))
		{
			if(uHasZones(cCompanyName))
			{
				gcMessage="<blink>Can't delete a Company  with zones</blink>";
				htmlCustomer();
			}
			else if(uHasBlocks(cCompanyName))
			{
				gcMessage="<blink>Can't delete a Company with blocks</blink>";
				htmlCustomer();
			}
			else if(uHasContacts(cCompanyName))
			{
				gcMessage="<blink>Can't delete a Company with contacts</blink>";
				htmlCustomer();
			}
			
			sprintf(gcDelStep,"Confirm ");
			gcMessage="Double check you have selected the correct record to delete. Then confirm. Any other action to cancel.";
		}
		else if(!strcmp(gcFunction,"Confirm Delete"))
		{
			if(!uClient)
			{
				gcMessage="<blink>No record selected</blink>";
				htmlCustomer();
			}
			DelCustomer();
		}
		else if(!strcmp(gcFunction,"Add Company Wizard"))
		{
			SetCustomerFieldsOn();
			htmlCustomerWizard(1);
		}
		else if(!strcmp(gcFunction,"Next"))
		{
			MYSQL_RES *res;
			
			SetCustomerFieldsOn();
			
			switch(uStep)
			{
				case 1:
					if(!cCompanyName[0])
					{
						gcMessage="<blink>Company name can't be empty</blink>";
						cLabelStyle="type_fields_req";
						htmlCustomerWizard(1);
					}

					SelectCustomer(cCompanyName,0);
					res=mysql_store_result(&gMysql);
			
					if(mysql_num_rows(res))
					{
						mysql_free_result(res);
						gcMessage="<blink>Company already exists</blink>";
						cLabelStyle="type_fields_req";
						htmlCustomerWizard(1);
					}
					
					break;
				case 2:
					if(cEmail[0])
					{
						if(strstr(cEmail,"@")==NULL || strstr(cEmail,".")==NULL)
						{
							cEmailStyle="type_fields_req";
							gcMessage="<blink>Email has to be a valid email address</blink>";
							htmlCustomerWizard(2);
						}
					}
					break;
				case 3:
					if(cCode[0])
					{
						sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cCode='%s'",cCode);
						mysql_query(&gMysql,gcQuery);
						res=mysql_store_result(&gMysql);

						if(mysql_num_rows(res))
						{
							gcMessage="<blink>Company code already used</blink>";
							cCodeStyle="type_fields_req";
							htmlCustomerWizard(3);
						}
					}
					break;
			}
			uStep++;
			htmlCustomerWizard(uStep);			
		}
		else if(!strcmp(gcFunction,"Finish"))
		{
			MYSQL_RES *res;
			SelectCustomer(cCompanyName,0);
			res=mysql_store_result(&gMysql);
			
			if(mysql_num_rows(res)==0)
			{
				SetCustomerFieldsOn();
				if(!ValidateCustomerInput())
				{					
					htmlCustomerWizard(4);
				}
				NewCustomer();
				htmlCustomer();
			}
			else
			{
				gcMessage="<blink>Company already exists!</blink>";
				cLabelStyle="type_fields_req";
			}
			mysql_free_result(res);

			SetCustomerFieldsOn();
			htmlCustomerWizard(4);		
		}
		else if(!strcmp(gcFunction,"Cancel Wizard"))
		{
			cCompanyName[0]=0;
			cEmail[0]=0;
			cCode[0]=0;
			cInfo[0]=0;
		}
		htmlCustomer();
	}

}//void CustomerCommands(pentry entries[], int x)


void htmlCustomerWizard(unsigned uStep)
{
	htmlHeader("DNS System","Header");
	sprintf(gcQuery,"CustomerWizard.%u",uStep);
	htmlCustomerPage("DNS System",gcQuery);
	htmlFooter("Footer");
	
}//void htmlCustomerWizard(unsigned uStep)


void htmlCustomer(void)
{
	if(cSearch[0])
	{
		FILE *fp;

		if((fp=fopen("/dev/null","w")))
		{
			funcCompanyNavList(fp,1);
			fclose(fp);
		}
	}

	htmlHeader("DNS System","Header");
	htmlCustomerPage("DNS System","Customer.Body");
	htmlFooter("Footer");

}//void htmlCustomer(void)


void htmlCustomerPage(char *cTitle, char *cTemplateName)
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
			char cuClient[16]={""};
			char cuResource[16]={""};
			//These vars will render the data in human readable form
			char cuCreatedDate[100]={""};
			char cuModDate[100]={""};
			char cuOwner[100]={""};
			char cuCreatedBy[100]={""};
			char cuModBy[100]={""};
			//These vars are for the form hidden fields
			char cuCreatedDateForm[100]={""};
			char cuModDateForm[100]={""};
			char cuOwnerForm[100]={""};
			char cuCreatedByForm[100]={""};
			char cuModByForm[100]={""};
			
			sprintf(cuClient,"%u",uClient);
			sprintf(cuResource,"%u",uResource);
			sprintf(cuCreatedDateForm,"%lu",uCreatedDate);
			sprintf(cuModDateForm,"%lu",uModDate);
			sprintf(cuOwnerForm,"%u",uOwner);
			sprintf(cuCreatedByForm,"%u",uCreatedBy);
			sprintf(cuModByForm,"%u",uModBy);

			if(uOwner)
				sprintf(cuOwner,"%s",ForeignKey(TCLIENT,"tClient.cLabel",uOwner));
			else
				sprintf(cuOwner,"---");

			if(uCreatedBy)
				sprintf(cuCreatedBy,"%s",ForeignKey(TCLIENT,"tClient.cLabel",uCreatedBy));
			else
				sprintf(cuCreatedBy,"---");

			if(uCreatedDate)
				sprintf(cuCreatedDate,"%s",ctime(&uCreatedDate));
			else
				sprintf(cuCreatedDate,"---");

			if(uModBy)
				sprintf(cuModBy,"%s",ForeignKey(TCLIENT,"tClient.cLabel",uModBy));
			else
				sprintf(cuModBy,"---");

			if(uModDate)
				sprintf(cuModDate,"%s",ctime(&uModDate));
			else
				sprintf(cuModDate,"---");
				
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="idnsAdmin.cgi";
			
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

			template.cpName[12]="cLabel";
			template.cpValue[12]=cCompanyName;

			template.cpName[13]="cEmail";
			template.cpValue[13]=cEmail;
			
			template.cpName[14]="cInfo";
			template.cpValue[14]=cInfo;

			template.cpName[15]="cCode";
			template.cpValue[15]=cCode;

			template.cpName[16]="cLabelStyle";
			template.cpValue[16]=cLabelStyle;

			template.cpName[17]="cEmailStyle";
			template.cpValue[17]=cEmailStyle;

			template.cpName[18]="cNavList";
			template.cpValue[18]=cNavList;

			template.cpName[19]="cSearchStyle";
			template.cpValue[19]=cSearchStyle;
			
			template.cpName[20]="cuClient";
			template.cpValue[20]=cuClient;

			template.cpName[21]="cCodeStyle";
			template.cpValue[21]=cCodeStyle;

			template.cpName[22]="cSearch";
			template.cpValue[22]=cSearch;

			template.cpName[23]="cInfoStyle";
			template.cpValue[23]=cInfoStyle;

			template.cpName[24]="cBtnStatus";
			if(uClient)
				template.cpValue[24]="";
			else
				template.cpValue[24]="disabled";

			template.cpName[25]="cCustomer";
			template.cpValue[25]=cCompanyName;

			template.cpName[26]="gcZone";
			template.cpValue[26]=gcZone;

			template.cpName[27]="uView";
			template.cpValue[27]=cuView;

			template.cpName[28]="uResource";
			template.cpValue[28]=cuResource;

			template.cpName[29]="uOwner";
			template.cpValue[29]=cuOwner;

			template.cpName[30]="uCreatedBy";
			template.cpValue[30]=cuCreatedBy;

			template.cpName[31]="uCreatedDate";
			template.cpValue[31]=cuCreatedDate;

			template.cpName[32]="uModBy";
			template.cpValue[32]=cuModBy;

			template.cpName[33]="uModDate";
			template.cpValue[33]=cuModDate;

			template.cpName[34]="uOwnerForm";
			template.cpValue[34]=cuOwnerForm;

			template.cpName[35]="uCreatedByForm";
			template.cpValue[35]=cuCreatedByForm;

			template.cpName[36]="uCreatedDateForm";
			template.cpValue[36]=cuCreatedDateForm;

			template.cpName[37]="uModByForm";
			template.cpValue[37]=cuModByForm;

			template.cpName[38]="uModDateForm";
			template.cpValue[38]=cuModDateForm;

			template.cpName[39]="";

			printf("\n<!-- Start htmlCustomerPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlCustomerPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlCustomerPage()

void funcCustomerContacts(FILE *fp)
{
	MYSQL_RES *res;
        MYSQL_ROW field;

	fprintf(fp,"<!-- funcCustomerContacts(fp) Start -->\n");

	if(!uClient && !gcCustomer[0]) return;

	if(!uClient && gcCustomer[0])
		uClient=uGetClient(gcCustomer);


	sprintf(gcQuery,"SELECT tClient.uClient,tClient.cLabel,tClient.cEmail,tClient.cInfo,"
			"tAuthorize.uPerm FROM tClient,tAuthorize WHERE tClient.uOwner=%u AND "
			"tAuthorize.uCertClient=tClient.uClient ORDER BY cLabel",uClient);
		
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
       	while((field=mysql_fetch_row(res)))
		fprintf(fp,"<tr><td><a class=darkLink href=\"idnsAdmin.cgi?gcPage=CustomerUser"
			"&uClient=%s&cCustomer=%s&cZone=%s&uView=%s&uResource=%u\">%s</a></td><td>%s</td><td>%s</td></tr>",
			field[0],cCompanyName,gcZone,cuView,uResource,field[1],field[2],field[3]);
	mysql_free_result(res);

	fprintf(fp,"<!-- funcCustomerContacts(fp)End -->\n");
	
}//void funcCustomerContacts(FILE *fp)


void SelectCustomer(char *cLabel,unsigned uMode)
{
	if(!uMode)
	{
		if(strstr(cLabel,"%")==NULL)
			sprintf(gcQuery,"SELECT %s FROM tClient WHERE cLabel='%s' AND uOwner=1",VAR_LIST_tClient,cLabel);
		else
			sprintf(gcQuery,"SELECT %s FROM tClient WHERE cLabel LIKE '%s' AND uOwner=1 ORDER BY cLabel",VAR_LIST_tClient,cLabel);
	}
	else
	{
		sprintf(gcQuery,"SELECT %s FROM tClient WHERE uClient=%s",VAR_LIST_tClient,cLabel);
	}

	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	
}//void SelectCustomer(char *cLabel)


void NewCustomer(void)
{
	time(&uCreatedDate);

	sprintf(gcQuery,"INSERT INTO tClient SET cLabel='%s',cEmail='%s',cInfo='%s',cCode='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
			cCompanyName
			,cEmail
			,cInfo
			,cCode
			,1
			,guLoginClient
			,uCreatedDate);

	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uClient=mysql_insert_id(&gMysql);

	if(uClient)
	{
		gcMessage="New Company created OK";
		iDNSLog(uClient,"tClient","New");
	}
	else
	{
		gcMessage="New Company NOT created";
		iDNSLog(uClient,"tClient","New Fail");
	}
	uOwner=guOrg;
	uCreatedBy=guLoginClient;
	
	//Set session cookie after creating new company
	sprintf(gcCustomer,"%.99s",cCompanyName);
	//If there's a zone or a RR selected, will unselect
	gcZone[0]=0;
	cuView[0]=0;
	uResource=0;
	guContact=0;
	sys_SetSessionCookie();

}//void NewCustomer(void)


void ModCustomer(void)
{
	time(&uModDate);

	sprintf(gcQuery,"UPDATE tClient SET cLabel='%s',cEmail='%s',cInfo='%s',cCode='%s',uModBy=%u,uModDate=%lu WHERE uClient=%u",
			cCompanyName
			,cEmail
			,cInfo
			,cCode
			,guLoginClient
			,uModDate
			,uClient);
	mysql_query(&gMysql,gcQuery);
	
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="Company modified OK";
		iDNSLog(uClient,"tClient","Mod");
	}
	else
	{
		gcMessage="Company NOT modified";
		iDNSLog(uClient,"tClient","Mod Fail");
	}

	uModBy=guLoginClient;
	
	sprintf(gcCustomer,"%.99s",cCompanyName);
	sys_SetSessionCookie();

}//void ModCustomer(void)


void DelCustomer(void)
{
	sprintf(gcQuery,"DELETE FROM tClient WHERE uClient=%u",uClient);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="Company deleted OK";
		iDNSLog(uClient,"tClient","Del");
	}
	else
	{
		gcMessage="Company NOT deleted";
		iDNSLog(uClient,"tClient","Del Fail");
	}
	
	gcCustomer[0]=0;
	gcZone[0]=0;
	cuView[0]=0;
	uResource=0;
	guContact=0;
	sys_SetSessionCookie();

}//void DelCustomer(void)


unsigned uHasZones(char *cLabel)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cLabel='%s'",cLabel);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		mysql_free_result(res);

		sprintf(gcQuery,"SELECT uZone FROM tZone WHERE uOwner=%s",field[0]);
		mysql_query(&gMysql,gcQuery);

		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);

		if(mysql_num_rows(res))
		{
			mysql_free_result(res);
			return(1);
		}
	}
	mysql_free_result(res);

	return(0);	
				
}//unsigned uHasZones(char *cLabel)


unsigned uHasBlocks(char *cLabel)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cLabel='%s'",cLabel);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		mysql_free_result(res);

		sprintf(gcQuery,"SELECT uBlock FROM tBlock WHERE uOwner=%s",field[0]);
		mysql_query(&gMysql,gcQuery);

		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);

		if(mysql_num_rows(res))
		{
			mysql_free_result(res);
			return(1);
		}
	}
	mysql_free_result(res);

	return(0);
	
}//unsigned uHasBlocks(char *cLabel)


unsigned uHasContacts(char *cLabel)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cLabel='%s'",cLabel);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		mysql_free_result(res);

		sprintf(gcQuery,"SELECT uClient FROM tClient WHERE uOwner=%s",field[0]);
		mysql_query(&gMysql,gcQuery);

		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);

		if(mysql_num_rows(res))
		{
			mysql_free_result(res);
			return(1);
		}
	}
	mysql_free_result(res);

	return(0);
	
}//unsigned uHasContacts(char *cLabel)


unsigned ValidateCustomerInput(void)
{
	MYSQL_RES *res;

	if(!cCompanyName[0])
	{
		cLabelStyle="type_fields_req";
		gcMessage="<blink>Customer Name can't be empty</blink>";
		cEmailStyle="type_fields";
		cCodeStyle="type_fields";
		cInfoStyle="type_textarea";
		return(0);
	}
	else
	{
		//
		//Check for valid characters, no punctuation symbols allowed except '.' in Customer Name
		register int i;

		for(i=0;cCompanyName[i];i++)
		{
			if(!isalnum(cCompanyName[i]) && cCompanyName[i]!='.' && cCompanyName[i]!=' ' && cCompanyName[i]!='-')
			{
				cLabelStyle="type_fields_req";
				cEmailStyle="type_fields";
				cCodeStyle="type_fields";
				cInfoStyle="type_textarea";
				gcMessage="<blink>Customer Name contains invalid characters</blink>";
				return(0);
			}
		}
	}
	if(cEmail[0])
	{
		if(strstr(cEmail,"@")==NULL || strstr(cEmail,".")==NULL)
		{
			cEmailStyle="type_fields_req";
			cLabelStyle="type_fields";
			cCodeStyle="type_fields";
			cInfoStyle="type_textarea";
			gcMessage="<blink>Email has to be a valid email address</blink>";
			return(0);
		}
	}
	if(cCode[0])
	{
		sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cCode='%s'",cCode);
		mysql_query(&gMysql,gcQuery);

		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		res=mysql_store_result(&gMysql);


		if(mysql_num_rows(res))
		{
			if(strcmp(gcFunction,"Confirm Modify"))
			{
				cCodeStyle="type_fields_req";
				cLabelStyle="type_fields";
				cEmailStyle="type_fields";
				cInfoStyle="type_textarea";
				gcMessage="<blink>Company Code already exists</blink>";
				return(0);
			}
			else
			{
				MYSQL_ROW field;
				field=mysql_fetch_row(res);
				//
				//If we are modyfing a record this condition indicates that we are trying 
				//to use an existent Company Code 
				unsigned uDbClient=0;

				sscanf(field[0],"%u",&uDbClient);
				if(uClient!=uDbClient)
				{
					cCodeStyle="type_fields_req";
					cLabelStyle="type_fields";
					cEmailStyle="type_fields";
					cInfoStyle="type_textarea";
					gcMessage="<blink>Company Code already exists</blink>";
					return(0);
				}
			}
		}
		mysql_free_result(res);
	}
	return(1);
		
}//unsigned ValidateInput(void)


void SetCustomerFieldsOn(void)
{
	cLabelStyle="type_fields";
	cEmailStyle="type_fields";
	cCodeStyle="type_fields";
	cInfoStyle="type_textarea";

}//void SetCustomerFieldsOn(void)


void funcZoneList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCount=0;
	unsigned uTotal=0;

	if(!uClient) return;
	

	sprintf(gcQuery,"SELECT tZone.cZone,tView.cLabel,tView.uView FROM tZone,tView WHERE "
			"tZone.uView=tView.uView AND tZone.uOwner='%u' ORDER BY tZone.cZone",uClient);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	
	uTotal=mysql_num_rows(res);
	
	if(!uTotal)
	{
		mysql_free_result(res);
		return;
	}
	fprintf(fp,"<a title='List of zones owned by the loaded company' class=inputLink href=\"#\" "
			"onClick=\"open_popup('?gcPage=Glossary&cLabel=Zone List')\"><strong>Zone List</strong></a><br>\n");

	while((field=mysql_fetch_row(res)))
	{
		uCount++;
		fprintf(fp,"<a href=\"idnsAdmin.cgi?gcPage=Zone&cZone=%s&uView=%s&cCustomer=%s\" class=darkLink>%s[%s]</a><br>\n",
			field[0]
			,field[2]
			,cCompanyName
			,field[0]
			,field[1]
			);
		if(uCount>50)
		{
			fprintf(fp,"<br>Only the first 50 zones displayed (%u zones)\n",uTotal);
			break;
		}
	}
	
	mysql_free_result(res);
	
	
}//void funcZoneList(FILE *fp)


void funcCompanyNavList(FILE *fp,unsigned uSetCookie)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uResults=0;
	
	if(!cSearch[0]) return;

        sprintf(gcQuery,"SELECT uClient,cLabel FROM tClient WHERE tClient.uClient!=1 AND "
			"tClient.cLabel LIKE '%s%%' AND tClient.uOwner=1 ORDER BY cLabel",cSearch);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);

	uResults=mysql_num_rows(res);

	if(uResults==1)
	{
		//Load single record, free result, return
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uClient);
			LoadCustomer();
			sprintf(gcCustomer,"%.99s",cCompanyName);
			if(uSetCookie)
				sys_SetSessionCookie();
			mysql_free_result(res);

			fprintf(fp,"<a class=darkLink href=\"idnsAdmin.cgi?gcPage=Customer&uClient==%s\">%s</a><br>\n",
				field[0]
				,field[1]);
				
			return;
		}
	}
	else if(uResults>1)
	{
		//Display bunch of records, free result, return
		unsigned uCount=0;
		
		while((field=mysql_fetch_row(res)))
		{
			uCount++;
			fprintf(fp,"<a class=darkLink href=\"idnsAdmin.cgi?gcPage=Customer&uClient=%1$s\">%2$s</a><br>\n",field[0],field[1]);

			if(uCount==MAX_RESULTS) break;
		}
		if(uCount<uResults)
			fprintf(fp,"Only the first %u shown (%u results). If the company you are looking for is not in the list above "
				"please further refine your search.<br>",uCount,uResults);
		//We free result and return outside this if
		
	}
	else if(!uResults)
	{
		//Show no rcds found msg, free result, return}
		fprintf(fp,"No records found.<br>\n");
	}

	mysql_free_result(res);
}//void funcCompanyNavList(FILE *fp)
