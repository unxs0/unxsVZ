/*
FILE 
	customer.c
	svn ID removed
AUTHOR
	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	unxsISP Admin (Owner) Interface
	program file.
*/


#include "interface.h"
#define ORG_CUSTOMER    "Organization Customer"
#define ORG_WEBMASTER   "Organization Webmaster"
#define ORG_SALES       "Organization Sales Force"
#define ORG_SERVICE     "Organization Customer Service"
#define ORG_ACCT        "Organization Bookkeeper"
#define ORG_ADMIN       "Organization Admin"

#define VAR_LIST_tClient "cFirstName,cLastName,cEmail,cAddr1,cAddr2,cCity,cState,cZip,cCountry,uPayment,cCardType,cCardNumber,uExpMonth,uExpYear,cCardName,cACHDebits,cShipName,cShipAddr1,cShipAddr2,cShipCity,cShipState,cShipZip,cShipCountry,cTelephone,cFax,cPasswd,cMobile,cBankName,cBranchName,cBranchCode,cAccountHolder,cAccountNumber,uAccountType,cIDNumber,cAddr3,cShipAddr3,cLanguage,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate,cLabel"

unsigned uCustomer=0;

static unsigned uSearchInstance=0;

static char cLabel[33]={""};
static char cFirstName[33]={""};
static char *cFirstNameStyle="type_fields_off";

static char cLastName[33]={""};
static char *cLastNameStyle="type_fields_off";

static char cEmail[101]={""};
static char *cEmailStyle="type_fields_off";

static char cAddr1[101]={""};
static char *cAddr1Style="type_fields_off";

static char cAddr2[101]={""};
static char *cAddr2Style="type_fields_off";

static char cAddr3[101]={""};
static char *cAddr3Style="type_fields_off";

static char cCity[101]={""};
static char *cCityStyle="type_fields_off";

static char cState[101]={""};
static char *cStateStyle="type_fields_off";

static char cZip[33]={""};
static char *cZipStyle="type_fields_off";

static char cCountry[65]={""};
static char *cCountryStyle="type_fields_off";

static char cCardType[33]={""};
static char *cCardTypeStyle="type_fields_off";

static char cCardNumber[33]={""};
static char *cCardNumberStyle="type_fields_off";

static unsigned uExpMonth=0;
static char *cuExpMonthStyle="type_fields_off";

static unsigned uExpYear=0;
static char *cuExpYearStyle="type_fields_off";

static char cCardName[65]={""};
static char *cCardNameStyle="type_fields_off";

static char cShipName[101]={""};
static char *cShipNameStyle="type_fields_off";

static char cShipAddr1[101]={""};
static char *cShipAddr1Style="type_fields_off";

static char cShipAddr2[101]={""};
static char *cShipAddr2Style="type_fields_off";

static char cShipAddr3[101]={""};
static char *cShipAddr3Style="type_fields_off";

static char cShipCity[101]={""};
static char *cShipCityStyle="type_fields_off";

static char cShipState[101]={""};
static char *cShipStateStyle="type_fields_off";

static char cShipZip[33]={""};
static char *cShipZipStyle="type_fields_off";

static char cShipCountry[65]={""};
static char *cShipCountryStyle="type_fields_off";

static char cTelephone[33]={""};
static char *cTelephoneStyle="type_fields_off";

static char cMobile[33]={""};
static char *cMobileStyle="type_fields_off";

static char cFax[33]={""};
static char *cFaxStyle="type_fields_off";

static char cACHDebits[65]={""};
static char *cACHDebitsStyle="type_fields_off";

static unsigned uPerm=0;
static char *cuPermStyle="type_fields_off";

static char cPasswd[65]={""};
static char *cPasswdStyle="type_fields_off";

static char cIDNumber[33]={""};
static char *cIDNumberStyle="type_fields_off";

static char cBankName[33]={""};
static char *cBankNameStyle="type_fields_off";

static char cBranchName[33]={""};
static char *cBranchNameStyle="type_fields_off";

static char cBranchCode[33]={""};
static char *cBranchCodeStyle="type_fields_off";

static char cAccountHolder[101]={""};
static char *cAccountHolderStyle="type_fields_off";

static char cAccountNumber[33]={""};
static char *cAccountNumberStyle="type_fields_off";

static unsigned uAccountType=0;
static char *cuAccountTypeStyle="type_fields_off";

static char cLanguage[33]={""};
static char *cLanguageStyle="type_fields_off";

//uOwner: Record owner
static unsigned uOwner=0;
//uCreatedBy: uClient for last insert
static unsigned uCreatedBy=0;
//uCreatedDate: Unix seconds date last insert
static time_t uCreatedDate=0;
//uModBy: uClient for last update
static unsigned uModBy=0;
//uModDate: Unix seconds date last update
static time_t uModDate=0;


static char cSearch[100]={""};

static unsigned uPayment=0;
static char *cuPaymentStyle="type_fields_off";

static unsigned uProduct=0;
static unsigned uStep=0;

static unsigned uServices[100]={0};
static unsigned uServiceStartPoint[100]={0};
static unsigned uServiceEndPoint[100]={0};
static char cParameterInput[100][100]={""}; //tClientConfig.cValue
static char cParameterLabel[100][100]={""}; //tParameter.cISMName
static char cParameterName[100][100]={""};  //tParameter.cParameter
static char *cParameterStyle[100];

static unsigned uParameterCount=0;
static unsigned uServicesCount=0;
static unsigned uPrevStatus=0;
MYSQL MysqlExt;
static char cDay[10]={""};
static char cMonth[10]={""};
static char cYear[10]={""};


//
//Local only
unsigned ValidateCustomerInput(void);
void SearchCustomer(char *cLabel);
void NewCustomer(void);
void DelCustomer(void);
void ModCustomer(void);
unsigned CustomerHasProducts(void);
void UpdateAuthorization(void);

void SetCustomerFieldsOn(void);
void LoadCustomer(unsigned cuClient);
void SearchCustomer(char *cSearchTerm);
unsigned uCustomerExists(char *cFirstName,char *cLastName);

void CustomerReport(void);

//Product deployment wizard functions
void htmlProductDeployWizard(unsigned uStep);
char *cGetServiceLabel(unsigned uService);
char *cGetProductLabel(unsigned uProduct);
unsigned uGetParameter(char *cParameter);
void ProcessProductDeploymentVars(pentry entries[], int x);
void DeployProduct(void);
void SubmitSingleServiceJob(unsigned uService,unsigned uInstance,char *cJobType,unsigned uExtClient,time_t uJobDate);
void SubmitServiceJob(unsigned uService,char *cJobName, char *cServer,unsigned uInstance,time_t uJobDate);
void GetConfiguration(const char *cName, char *cValue);
void SetParamFieldsOn(void);
unsigned uProductHasServices(unsigned uProduct);
time_t cDateToUnixTime(char *cDate);
void CheckAllParameters(void);
unsigned uIsParameterInService(unsigned uService,char *cParamName);

void ProcessCustomerVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{

		if(!strcmp(entries[i].name,"uCustomer"))
			sscanf(entries[i].val,"%u",&uCustomer);
		else if(!strcmp(entries[i].name,"uStep"))
			sscanf(entries[i].val,"%u",&uStep);
		else if(!strcmp(entries[i].name,"uProduct"))
			sscanf(entries[i].val,"%u",&uProduct);
		else if(!strcmp(entries[i].name,"cFirstName"))
			sprintf(cFirstName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLastName"))
			sprintf(cLastName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cEmail"))
			sprintf(cEmail,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cAddr1"))
			sprintf(cAddr1,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cAddr2"))
			sprintf(cAddr2,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cAddr3"))
			sprintf(cAddr3,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCity"))
			sprintf(cCity,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cState"))
			sprintf(cState,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cZip"))
			sprintf(cZip,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCountry"))
			sprintf(cCountry,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uPayment"))
			sscanf(entries[i].val,"%u",&uPayment);
		else if(!strcmp(entries[i].name,"cCardType"))
			sprintf(cCardType,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCardNumber"))
			sprintf(cCardNumber,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uExpMonth"))
			sscanf(entries[i].val,"%u",&uExpMonth);
		else if(!strcmp(entries[i].name,"uExpYear"))
			sscanf(entries[i].val,"%u",&uExpYear);
		else if(!strcmp(entries[i].name,"cCardName"))
			sprintf(cCardName,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cACHDebits"))
			sprintf(cACHDebits,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipName"))
			sprintf(cShipName,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipAddr1"))
			sprintf(cShipAddr1,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipAddr2"))
			sprintf(cShipAddr2,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipAddr3"))
			sprintf(cShipAddr3,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipCity"))
			sprintf(cShipCity,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipState"))
			sprintf(cShipState,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipZip"))
			sprintf(cShipZip,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipCountry"))
			sprintf(cShipCountry,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cTelephone"))
			sprintf(cTelephone,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cMobile"))
			sprintf(cMobile,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cFax"))
			sprintf(cFax,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cPasswd"))
			sprintf(cPasswd,"%.20s",entries[i].val);
		else if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"cDay"))
			sprintf(cDay,"%.9s",entries[i].val);
		else if(!strcmp(entries[i].name,"cMonth"))
			sprintf(cMonth,"%.9s",entries[i].val);
		else if(!strcmp(entries[i].name,"cYear"))
			sprintf(cYear,"%.9s",entries[i].val);	
		else if(!strcmp(entries[i].name,"cBankName"))
			sprintf(cBankName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cBranchName"))
			sprintf(cBranchName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cBranchCode"))
			sprintf(cBranchCode,"%.6s",entries[i].val);
		else if(!strcmp(entries[i].name,"cAccountHolder"))
			sprintf(cAccountHolder,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cAccountNumber"))
			sprintf(cAccountNumber,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"uAccountType"))
			sscanf(entries[i].val,"%u",&uAccountType);
		else if(!strcmp(entries[i].name,"cIDNumber"))
			sprintf(cIDNumber,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);	
		else if(!strcmp(entries[i].name,"uOwner"))
			sscanf(entries[i].val,"%u",&uOwner);
		else if(!strcmp(entries[i].name,"uCreatedBy"))
			sscanf(entries[i].val,"%u",&uCreatedBy);
		else if(!strcmp(entries[i].name,"uModBy"))
			sscanf(entries[i].val,"%u",&uModBy);
		else if(!strcmp(entries[i].name,"uCreatedDate"))
			sscanf(entries[i].val,"%lu",&uCreatedDate);
		else if(!strcmp(entries[i].name,"uModDate"))
			sscanf(entries[i].val,"%lu",&uModDate);
		else if(!strcmp(entries[i].name,"uSearchInstance"))
			sscanf(entries[i].val,"%u",&uSearchInstance);
		else if(!strcmp(entries[i].name,"cLanguage"))
			sprintf(cLanguage,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uPerm"))
			sscanf(entries[i].val,"%u",&uPerm);
	}

}//void ProcessUserVars(pentry entries[], int x)


void ProcessProductDeploymentVars(pentry entries[], int x)
{
	register int i;
	unsigned uIndex=0;
	
	for(i=0;i<x;i++)
	{
		if(!strncmp(entries[i].name,"cParam.",7))
		{
			sscanf(entries[i].name,"cParam.%u",&uIndex);
			sprintf(cParameterInput[uIndex],"%.99s",entries[i].val);
			uParameterCount++;
		}
		else if(!strncmp(entries[i].name,"cParamLabel.",12))
		{
			sscanf(entries[i].name,"cParamLabel.%u",&uIndex);
			sprintf(cParameterLabel[uIndex],"%.99s",entries[i].val);
		}
		else if(!strncmp(entries[i].name,"cParamName.",11))
		{
			sscanf(entries[i].name,"cParamName.%u",&uIndex);
			sprintf(cParameterName[uIndex],"%.99s",entries[i].val);
		}
		else if(!strncmp(entries[i].name,"uService.",9))
		{
			sscanf(entries[i].name,"uService.%u",&uIndex);
			sscanf(entries[i].val,"%u",&uServices[uIndex]);
			uServiceStartPoint[uIndex]=uParameterCount;
			if(uIndex) uServiceEndPoint[uIndex-1]=uParameterCount;
			uServicesCount++;
		}
	}
	if(uServicesCount) uServiceEndPoint[uServicesCount-1]=uParameterCount;


}//void ProcessProductDeploymentVars(pentry entries[], int x)


void CustomerGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uCustomer"))
			sscanf(gentries[i].val,"%u",&uCustomer);
	}

	if(uCustomer)
		LoadCustomer(uCustomer);
	
	htmlCustomer();

}//void CustomerGetHook(entry gentries[],int x)


void LoadCustomer(unsigned uClient)
{
//	htmlPlainTextError("LoadCustomer");
	MYSQL_RES *res;
	MYSQL_ROW field;

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT " VAR_LIST_tClient " FROM tClient WHERE uClient=%u ORDER BY uClient",
			uClient);
	else 
		sprintf(gcQuery,"SELECT " VAR_LIST_tClient " FROM " TCLIENT
				" WHERE uClient=%2$u AND (uClient=%1$u OR uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
				" ORDER BY uClient",guOrg,uClient);
	
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cFirstName,"%.32s",field[0]);
		sprintf(cLastName,"%.32s",field[1]);
		sprintf(cEmail,"%.100s",field[2]);
		sprintf(cAddr1,"%.100s",field[3]);
		sprintf(cAddr2,"%.100s",field[4]);
		sprintf(cCity,"%.100s",field[5]);
		sprintf(cState,"%.100s",field[6]);
		sprintf(cZip,"%.32s",field[7]);
		sprintf(cCountry,"%.64s",field[8]);
		sscanf(field[9],"%u",&uPayment);
		sprintf(cCardType,"%.32s",field[10]);
		sprintf(cCardNumber,"%.32s",field[11]);
		sscanf(field[12],"%u",&uExpMonth);
		sscanf(field[13],"%u",&uExpYear);
		sprintf(cCardName,"%.64s",field[14]);
		sprintf(cACHDebits,"%.64s",field[15]);
		sprintf(cShipName,"%.100s",field[16]);
		sprintf(cShipAddr1,"%.100s",field[17]);
		sprintf(cShipAddr2,"%.100s",field[18]);
		sprintf(cShipCity,"%.100s",field[19]);
		sprintf(cShipState,"%.100s",field[20]);
		sprintf(cShipZip,"%.32s",field[21]);
		sprintf(cShipCountry,"%.64s",field[22]);
		sprintf(cTelephone,"%.32s",field[23]);
		sprintf(cFax,"%.32s",field[24]);
		//sprintf(cPasswd,"%.20s",field[25]);
		sprintf(cMobile,"%.32s",field[26]);
		sprintf(cBankName,"%.32s",field[27]);
		sprintf(cBranchName,"%.32s",field[28]);
		sprintf(cBranchCode,"%.6s",field[29]);
		sprintf(cAccountHolder,"%.100s",field[30]);
		sprintf(cAccountNumber,"%.32s",field[31]);
		sscanf(field[32],"%u",&uAccountType);
		sprintf(cIDNumber,"%.32s",field[33]);
		sprintf(cAddr3,"%.100s",field[34]);
		sprintf(cShipAddr3,"%.100s",field[35]);
		sprintf(cLanguage,"%.32s",field[36]);
		sscanf(field[37],"%u",&uOwner);
		sscanf(field[38],"%u",&uCreatedBy);
		sscanf(field[39],"%lu",&uCreatedDate);
		sscanf(field[40],"%u",&uModBy);
		sscanf(field[41],"%lu",&uModDate);
		sprintf(cLabel,"%.32s",field[42]);

		if(!cFirstName[0]) sprintf(cFirstName,"%s",cLabel);

		mysql_free_result(res);
		sprintf(gcQuery,"SELECT cPasswd,uPerm FROM tAuthorize WHERE uCertClient=%u",uClient);
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(cPasswd,"%s",field[0]);
			sscanf(field[1],"%u",&uPerm);
		}
	}
	else
		gcMessage="No records found.";

	mysql_free_result(res);

}//void LoadCustomer(char *cuClient)


unsigned uCustomerExists(char *cFirstName,char *cLastName)
{
	MYSQL_RES *res;
	
	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cLabel='%s %s'",cFirstName,cLastName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	return(unsigned)mysql_num_rows(res);
	
}//unsigned uCustomerExists(cFirstName,cLastName)


void CustomerCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Customer"))
	{
		ProcessCustomerVars(entries,x);

		if(!strcmp(gcFunction,"New"))
		{	
			if(guPermLevel>=10)
			{
				sprintf(gcNewStep,"Confirm ");
				gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
				uCustomer=0;
				gcInputStatus[0]=0;
				SetCustomerFieldsOn();
				htmlCustomer();
			}
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings.";
				htmlCustomer();
			}
		}
		else if(!strcmp(gcFunction,"Confirm New"))
		{
			if(guPermLevel>=10)
			{
				if(ValidateCustomerInput()==1)
				{
					if(!uCustomerExists(cFirstName,cLastName))
					{
						NewCustomer();
						htmlCustomer();
					}
					else
					{
						gcMessage="<blink>Error: </blink>Customer already exists!";
						SetCustomerFieldsOn();
						cFirstNameStyle="type_fields_req";
						cLastNameStyle="type_fields_req";
						gcInputStatus[0]=0;
						sprintf(gcNewStep,"Confirm ");
						htmlCustomer();
					}
				}
				else
				{
					sprintf(gcNewStep,"Confirm ");
					gcInputStatus[0]=0;
					htmlCustomer();
				}
			}
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings.";
				htmlCustomer();
			}
		}
		else if(!strcmp(gcFunction,"Modify"))
		{
			if(uAllowMod(uOwner,uCreatedBy))
			{
				sprintf(gcModStep,"Confirm ");
				gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
				SetCustomerFieldsOn();
				gcInputStatus[0]=0;
				htmlCustomer();
			}
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings.";
				htmlCustomer();
			}
		}
		else if(!strcmp(gcFunction,"Confirm Modify"))
		{
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(!ValidateCustomerInput())
				{
					sprintf(gcModStep,"Confirm ");
					gcInputStatus[0]=0;
					SetCustomerFieldsOn();
					htmlCustomer();
				}
				ModCustomer();
			
				htmlCustomer();
			}
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings.";
				htmlCustomer();
			}
		}
		else if(!strcmp(gcFunction,"Delete"))
		{
			if(uAllowDel(uOwner,uCreatedBy))
			{
				
				if(CustomerHasProducts())
				{
					gcMessage="<blink>Error: </blink>Can't delete a customer with active products";
					htmlCustomer();
				}
				
				sprintf(gcDelStep,"Confirm ");
				gcMessage="Double check you have selected the correct record to delete. Then confirm. Any other action to cancel.";
				htmlCustomer();
			}
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings.";
				htmlCustomer();
			}
		}
		else if(!strcmp(gcFunction,"Confirm Delete"))
		{
			if(uAllowDel(uOwner,uCreatedBy))
			{
				if(CustomerHasProducts())
				{
					gcMessage="<blink>Error: </blink>Can't delete a customer with active products";
					htmlCustomer();
				}
				DelCustomer();
				htmlCustomer();
			}
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings.";
				htmlCustomer();
			}
		}
		else if(!strcmp(gcFunction,"Add Product Wizard"))
		{
			if(uOwner==guOrg)
				htmlProductDeployWizard(1);
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings.";
				htmlCustomer();
			}
		}
		else if(!strcmp(gcFunction,"Next"))
		{
			if((uStep+1)==2 && !uProductHasServices(uProduct))
			{
				gcMessage="<blink>Error: </blink>The selected product has no services configured. Can't continue.";
				htmlProductDeployWizard(uStep);
			}
			
			SetParamFieldsOn();
			if(uStep==2)
			{
				ProcessProductDeploymentVars(entries,x);
				CheckAllParameters();
			}

			uStep++;
			if(uStep>3) uStep=3; //little sanity check

			htmlProductDeployWizard(uStep);
		}
		else if(!strcmp(gcFunction,"Back"))
		{
			uStep--;
			if(uStep==2) // from step 3 to 2, user may want to correct entered data.
				ProcessProductDeploymentVars(entries,x);
			if(uStep==0) uStep=1; //little sanity check
			SetParamFieldsOn();
			htmlProductDeployWizard(uStep);
		}
		else if(!strcmp(gcFunction,"Finish"))
		{
			//End of the product deployment wizard.
			//deploy the product and go back to customers screen.
			ProcessProductDeploymentVars(entries,x);
			DeployProduct();
			gcMessage="Product deployed and waiting for activation.";
			LoadCustomer(uCustomer);
		}
		else if(!strcmp(gcFunction,"Customer Report"))
			CustomerReport();

		htmlCustomer();
	}
	else if(!strcmp(gcPage,"CustomerReport"))
	{
		CustomerReport();
	}

}//void CustomerCommands(pentry entries[], int x)


void htmlProductDeployWizard(unsigned uStep)
{
	htmlHeader("unxsISP Admin Interface","Header");
	sprintf(gcQuery,"ProductDeploy.%u",uStep);
	htmlCustomerPage("",gcQuery);
	
	htmlFooter("Footer");
}
void htmlCustomer(void)
{
	if(cSearch[0])
	{
		FILE *fp;
		if((fp=fopen("/dev/null","w")))
		{
			funcCustomerNavList(fp);
			fclose(fp);
		}
	}

	htmlHeader("unxsISP Admin Interface","Header");
	htmlCustomerPage("","Customer.Body");
	htmlFooter("Footer");

}//void htmlCustomer(void)


void CustomerReport(void)
{
	htmlHeader("unxsISP Admin Interface","Header");
	htmlCustomerPage("","CustomerReport.Body");
	htmlFooter("Footer");

}//void CustomerReport(void)


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
			char cuExpMonth[10]={""};
			char cuExpYear[10]={""};
			char cuCustomer[16]={""};
			char cuProduct[16]={""};
			char cuPayment[16]={""};
			char cuAccountType[16]={""};
			char cuOwner[16]={""};
			char cuCreatedBy[16]={""};
			char cuCreatedDate[16]={""};
			char cuModBy[16]={""};
			char cuModDate[16]={""};
			char cCustomerName[255]={""};
			
			sprintf(cCustomerName,"for %.251s",cGetcCustomerName(uCustomer));
			sprintf(cuCustomer,"%u",uCustomer);
			sprintf(cuExpMonth,"%u",uExpMonth);
			sprintf(cuExpYear,"%u",uExpYear);
			sprintf(cuProduct,"%u",uProduct);
			sprintf(cuPayment,"%u",uPayment);
			sprintf(cuAccountType,"%u",uAccountType);

			sprintf(cuOwner,"%u",uOwner);
			sprintf(cuCreatedBy,"%u",uCreatedBy);
			sprintf(cuCreatedDate,"%lu",uCreatedDate);
			sprintf(cuModBy,"%u",uModBy);
			sprintf(cuModDate,"%lu",uModDate);
		
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="ispAdmin.cgi";
			
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

			template.cpName[12]="cFirstName";
			template.cpValue[12]=cFirstName;
			
			template.cpName[13]="cFirstNameStyle";
			template.cpValue[13]=cFirstNameStyle;
			
			template.cpName[14]="cLastName";
			template.cpValue[14]=cLastName;
			
			template.cpName[15]="cEmail";
			template.cpValue[15]=cEmail;
			
			template.cpName[16]="cEmailStyle";
			template.cpValue[16]=cEmailStyle;
			
			template.cpName[17]="cAddr1";
			template.cpValue[17]=cAddr1;
			
			template.cpName[18]="cAddr1Style";
			template.cpValue[18]=cAddr1Style;
			
			template.cpName[19]="cAddr2";
			template.cpValue[19]=cAddr2;
			
			template.cpName[20]="cAddr2Style";
			template.cpValue[20]=cAddr2Style;

			template.cpName[21]="cCity";
			template.cpValue[21]=cCity;
			
			template.cpName[22]="cCityStyle";
			template.cpValue[22]=cCityStyle;

			template.cpName[23]="cState";
			template.cpValue[23]=cState;

			template.cpName[24]="cStateStyle";
			template.cpValue[24]=cStateStyle;

			template.cpName[25]="cZip";
			template.cpValue[25]=cZip;

			template.cpName[26]="cZipStyle";
			template.cpValue[26]=cZipStyle;

			template.cpName[27]="cCountry";
			template.cpValue[27]=cCountry;
			
			template.cpName[28]="cCountryStyle";
			template.cpValue[28]=cCountryStyle;
			
			template.cpName[29]="cCardType";
			template.cpValue[29]=cCardType;
			
			template.cpName[30]="cCardTypeStyle";
			template.cpValue[30]=cCardTypeStyle;
			
			template.cpName[31]="cCardNumber";
			template.cpValue[31]=cCardNumber;
			
			template.cpName[32]="cCardNumberStyle";
			template.cpValue[32]=cCardNumberStyle;
			
			template.cpName[33]="uExpMonth";
			template.cpValue[33]=cuExpMonth;
			
			template.cpName[34]="cuExpMonthStyle";
			template.cpValue[34]=cuExpMonthStyle;
			
			template.cpName[35]="uExpYear";
			template.cpValue[35]=cuExpYear;
			
			template.cpName[36]="uExpYearStyle";
			template.cpValue[36]=cuExpYearStyle;
			
			template.cpName[37]="cCardName";
			template.cpValue[37]=cCardName;
			
			template.cpName[38]="cCardNameStyle";
			template.cpValue[38]=cCardNameStyle;
			
			template.cpName[39]="cShipAddr1";
			template.cpValue[39]=cShipAddr1;
			
			template.cpName[40]="cShipAddr1Style";
			template.cpValue[40]=cShipAddr1Style;
			
			template.cpName[41]="cShipAddr2";
			template.cpValue[41]=cShipAddr2;
			
			template.cpName[42]="cShipAddr2Style";
			template.cpValue[42]=cShipAddr2Style;
			
			template.cpName[43]="cShipCity";
			template.cpValue[43]=cShipCity;

			template.cpName[44]="cShipState";
			template.cpValue[44]=cShipState;

			template.cpName[45]="cShipZip";
			template.cpValue[45]=cShipZip;

			template.cpName[46]="cShipZipStyle";
			template.cpValue[46]=cShipZipStyle;

			template.cpName[47]="cShipCountry";
			template.cpValue[47]=cShipCountry;

			template.cpName[48]="cShipCountryStyle";
			template.cpValue[48]=cShipCountryStyle;

			template.cpName[49]="cTelephone";
			template.cpValue[49]=cTelephone;

			template.cpName[50]="cFax";
			template.cpValue[50]=cFax;
			
			template.cpName[51]="cFaxStyle";
			template.cpValue[51]=cFaxStyle;

			template.cpName[52]="cLastNameStyle";
			template.cpValue[52]=cLastNameStyle;

			template.cpName[53]="cShipName";
			template.cpValue[53]=cShipName;

			template.cpName[54]="cShipNameStyle";
			template.cpValue[54]=cShipNameStyle;

			template.cpName[55]="cShipCityStyle";
			template.cpValue[55]=cShipCityStyle;

			template.cpName[56]="cShipStateStyle";
			template.cpValue[56]=cShipStateStyle;
			
			template.cpName[57]="cTelephoneStyle";
			template.cpValue[57]=cTelephoneStyle;

			template.cpName[58]="uCustomer";
			template.cpValue[58]=cuCustomer;

			template.cpName[59]="cPasswd";
			template.cpValue[59]=cPasswd;

			template.cpName[60]="cPasswdStyle";
			template.cpValue[60]=cPasswdStyle;

			template.cpName[61]="cACHDebits";
			template.cpValue[61]=cACHDebits;

			template.cpName[62]="cACHDebitsStyle";
			template.cpValue[62]=cACHDebitsStyle;

			template.cpName[63]="cBtnStatus";
			if(uCustomer)
				template.cpValue[63]="";
			else
				template.cpValue[63]="disabled";

			template.cpName[64]="uProduct";
			template.cpValue[64]=cuProduct;

			template.cpName[65]="cProductLabel";
			template.cpValue[65]=cGetProductLabel(uProduct);
			
			template.cpName[66]="uPayment";
			template.cpValue[66]=cuPayment;
			
			template.cpName[67]="cIDNumber";
			template.cpValue[67]=cIDNumber;

			template.cpName[68]="cIDNumberStyle";
			template.cpValue[68]=cIDNumberStyle;

			template.cpName[69]="cBankName";
			template.cpValue[69]=cBankName;
			
			template.cpName[70]="cBankNameStyle";
			template.cpValue[70]=cBankNameStyle;

			template.cpName[71]="cBranchName";
			template.cpValue[71]=cBranchName;

			template.cpName[72]="cBranchCode";
			template.cpValue[72]=cBranchCode;

			template.cpName[73]="cBranchCodeStyle";
			template.cpValue[73]=cBranchCodeStyle;

			template.cpName[74]="cAccountHolder";
			template.cpValue[74]=cAccountHolder;

			template.cpName[75]="cAccountHolderStyle";
			template.cpValue[75]=cAccountHolderStyle;

			template.cpName[76]="cAccountNumber";
			template.cpValue[76]=cAccountNumber;

			template.cpName[77]="cAccountNumberStyle";
			template.cpValue[77]=cAccountNumberStyle;

			template.cpName[78]="cBranchNameStyle";
			template.cpValue[78]=cBranchNameStyle;

			template.cpName[79]="cMobile";
			template.cpValue[79]=cMobile;

			template.cpName[80]="cMobileStyle";
			template.cpValue[80]=cMobileStyle;

			template.cpName[81]="uAccountType";
			template.cpValue[81]=cuAccountType;

			template.cpName[82]="cAddr3";
			template.cpValue[82]=cAddr3;

			template.cpName[83]="cAddr3Style";
			template.cpValue[83]=cAddr3Style;

			template.cpName[84]="cShipAddr3";
			template.cpValue[84]=cShipAddr3;

			template.cpName[85]="cShipAddr3Style";
			template.cpValue[85]=cShipAddr3Style;

			template.cpName[86]="uOwner";
			template.cpValue[86]=cuOwner;

			template.cpName[87]="uCreatedBy";
			template.cpValue[87]=cuCreatedBy;

			template.cpName[88]="uCreatedDate";
			template.cpValue[88]=cuCreatedDate;

			template.cpName[89]="uModBy";
			template.cpValue[89]=cuModBy;

			template.cpName[90]="uModDate";
			template.cpValue[90]=cuModDate;

			template.cpName[91]="cCustomerName";
			template.cpValue[91]=cCustomerName;

			template.cpName[92]="cLanguage";
			template.cpValue[92]=cLanguage;

			template.cpName[93]="";

			printf("\n<!-- Start htmlCustomerPage(%s) -->\n",cTemplateName); 
//			fpTemplate(stdout,cTemplateName,&template);
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


void NewCustomer(void)
{
	sprintf(gcQuery,"INSERT INTO tClient SET cLabel='%s %s',cFirstName='%s',cLastName='%s',cEmail='%s',"
			"cAddr1='%s',cAddr2='%s',cAddr3='%s',cCity='%s',cState='%s',cZip='%s',cCountry='%s',"
			"uPayment='%u',cCardType='%s',cCardNumber='%s',uExpMonth='%u',uExpYear='%u',cCardName='%s',"
			"cACHDebits='%s',cShipName='%s',cShipAddr1='%s',cShipAddr2='%s',cShipAddr3='%s',cShipCity='%s',"
			"cShipState='%s',cShipZip='%s',cShipCountry='%s',cTelephone='%s',cFax='%s',cBankName='%s',"
			"cBranchName='%s',cBranchCode='%s',cAccountHolder='%s',cAccountNumber='%s',uAccountType='%u',"
			"cMobile='%s',cIDNumber='%s',cLanguage='%s',uOwner='%u',uCreatedBy='%u',uCreatedDate=UNIX_TIMESTAMP(NOW())",
			TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cEmail)
			,TextAreaSave(cAddr1)
			,TextAreaSave(cAddr2)
			,TextAreaSave(cAddr2)
			,TextAreaSave(cCity)
			,TextAreaSave(cState)
			,TextAreaSave(cZip)
			,TextAreaSave(cCountry)
			,uPayment
			,TextAreaSave(cCardType)
			,TextAreaSave(cCardNumber)
			,uExpMonth
			,uExpYear
			,TextAreaSave(cCardName)
			,TextAreaSave(cACHDebits)
			,TextAreaSave(cShipName)
			,TextAreaSave(cShipAddr1)
			,TextAreaSave(cShipAddr2)
			,TextAreaSave(cShipAddr3)
			,TextAreaSave(cShipCity)
			,TextAreaSave(cShipState)
			,TextAreaSave(cShipZip)
			,TextAreaSave(cShipCountry)
			,TextAreaSave(cTelephone)
			,TextAreaSave(cFax)
			,TextAreaSave(cBankName)
			,TextAreaSave(cBranchName)
			,TextAreaSave(cBranchCode)
			,TextAreaSave(cAccountHolder)
			,TextAreaSave(cAccountNumber)
			,uAccountType
			,TextAreaSave(cMobile)
			,TextAreaSave(cIDNumber)
			,TextAreaSave(cLanguage)
			,guOrg
			,guLoginClient
			);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCustomer=mysql_insert_id(&gMysql);

	if(uCustomer)
	{
		gcMessage="New Customer created OK";
		unxsISPLog(uCustomer,"tClient","New");
	}
	else
	{
		gcMessage="New Customer NOT created";
		unxsISPLog(uCustomer,"tClient","New Fail");
	}
	uOwner=guOrg;
	uCreatedBy=guLoginClient;
	time(&uCreatedDate);
	UpdateAuthorization();

}//void NewCustomer(void)


void ModCustomer(void)
{

	//Company affilitation can't be modified.
	//Neither company data, to do so, must load company
	
	sprintf(gcQuery,"UPDATE tClient SET cLabel='%s %s',cFirstName='%s',cLastName='%s',"
			"cEmail='%s',cAddr1='%s',cAddr2='%s',cAddr3='%s',cCity='%s',"
			"cState='%s',cZip='%s',cCountry='%s',uPayment='%u',cCardType='%s',"
			"cCardNumber='%s',uExpMonth='%u',uExpYear='%u',cCardName='%s',"
			"cACHDebits='%s',cShipName='%s',cShipAddr1='%s',cShipAddr2='%s',"
			"cShipAddr3='%s',cShipCity='%s',cShipState='%s',cShipZip='%s',"
			"cShipCountry='%s',cTelephone='%s',cFax='%s',cPasswd='%s',"
			"cBankName='%s',cBranchName='%s',cBranchCode='%s',cAccountHolder='%s',"
			"cAccountNumber='%s',uAccountType='%u',cMobile='%s',cIDNumber='%s',"
			"cLanguage='%s',uModBy='%u',uModDate=UNIX_TIMESTAMP(NOW()) WHERE uClient='%u'",
			TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cEmail)
			,TextAreaSave(cAddr1)
			,TextAreaSave(cAddr2)
			,TextAreaSave(cAddr3)
			,TextAreaSave(cCity)
			,TextAreaSave(cState)
			,TextAreaSave(cZip)
			,TextAreaSave(cCountry)
			,uPayment
			,TextAreaSave(cCardType)
			,TextAreaSave(cCardNumber)
			,uExpMonth
			,uExpYear
			,TextAreaSave(cCardName)
			,TextAreaSave(cACHDebits)
			,TextAreaSave(cShipName)
			,TextAreaSave(cShipAddr1)
			,TextAreaSave(cShipAddr2)
			,TextAreaSave(cShipAddr3)
			,TextAreaSave(cShipCity)
			,TextAreaSave(cShipState)
			,TextAreaSave(cShipZip)
			,TextAreaSave(cShipCountry)
			,TextAreaSave(cTelephone)
			,TextAreaSave(cFax)
			,TextAreaSave(cPasswd)
			,TextAreaSave(cBankName)
			,TextAreaSave(cBranchName)
			,TextAreaSave(cBranchCode)
			,TextAreaSave(cAccountHolder)
			,TextAreaSave(cAccountNumber)
			,uAccountType
			,TextAreaSave(cMobile)
			,TextAreaSave(cIDNumber)
			,TextAreaSave(cLanguage)
			,guLoginClient
			,uCustomer
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="Customer modified OK";
		unxsISPLog(uCustomer,"tClient","Mod");
	}
	else
	{
		gcMessage="Customer NOT modified";
		unxsISPLog(uCustomer,"tClient","Mod Fail");
	}
	uModBy=guLoginClient;
	time(&uModDate);
	UpdateAuthorization();

}//void ModCustomer(void)


void DelCustomer(void)
{
	//Add delete tInstance. Note that a customer can't be deleted if it has not cancelled 
	//products deployed!
	//
	
	sprintf(gcQuery,"DELETE FROM tClient WHERE uClient='%u'",uCustomer);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="Customer deleted OK";
		unxsISPLog(uCustomer,"tClient","Del");
	}
	else
	{
		gcMessage="Customer NOT deleted";
		unxsISPLog(uCustomer,"tClient","Del Fail");
	}
	sprintf(gcQuery,"DELETE FROM tInstance WHERE uOwner='%u'",uCustomer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	sprintf(gcQuery,"DELETE FROM tClientConfig WHERE uOwner='%u'",uCustomer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));


}//void DelCustomer(void)


unsigned ValidateCustomerInput(void)
{
	if(!cFirstName[0])
	{
		SetCustomerFieldsOn();
		cFirstNameStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter first name";
		return(0);
	}
	if(!cLastName[0])
	{
		SetCustomerFieldsOn();
		cLastNameStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter last name";
		return(0);
	}

	if(!cEmail[0])
	{
		SetCustomerFieldsOn();
		cEmailStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter email address";
		return(0);
	}
	else
	{
		if(strstr(cEmail,"@")==NULL || strstr(cEmail,".")==NULL)
		{
			SetCustomerFieldsOn();
			cEmailStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Email has to be a valid email address";
			return(0);
		}
	}
	if(!cAddr1[0])
	{
		SetCustomerFieldsOn();
		cAddr1Style="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter address information";
		return(0);
	}
	if(!cCity[0])
	{
		SetCustomerFieldsOn();
		cCityStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter city information";
		return(0);
	}
	if(!cState[0])
	{
		SetCustomerFieldsOn();
		cStateStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter state information";
		return(0);
	}
	if(!cZip[0])
	{
		SetCustomerFieldsOn();
		cZipStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter zip code";
		return(0);
	}
	if(!cCountry[0])
	{
		SetCustomerFieldsOn();
		cCountryStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter country information";
		return(0);
	}
	if(!cTelephone[0])
	{
		SetCustomerFieldsOn();
		cTelephoneStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter telephone information";
		return(0);
	}
	if(strcmp(cCardType,"---"))
	{
		if(!cCardNumber[0])
		{
			SetCustomerFieldsOn();
			cCardNumberStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Must enter credit card number";
			return(0);
		}
		if(!uExpMonth)
		{
			SetCustomerFieldsOn();
			cuExpMonthStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Must select expiration month";
			return(0);
		}
		if(!uExpYear)
		{
			SetCustomerFieldsOn();
			cuExpYearStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Must select expiration year";
			return(0);
		}
	}
	if(cShipName[0])
	{
		if(!cShipAddr1[0])
		{
			SetCustomerFieldsOn();
			cShipAddr1Style="type_fields_req";
			gcMessage="<blink>Error: </blink>Must enter shipping address information";
			return(0);
		}
		if(!cShipCity[0])
		{
			SetCustomerFieldsOn();
			cShipCityStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Must enter shipping city information";
			return(0);
		}
		if(!cShipState[0])
		{
			SetCustomerFieldsOn();
			cShipStateStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Must enter shipping state information";
			return(0);
		}
		if(!cShipZip[0])
		{
			SetCustomerFieldsOn();
			cShipZipStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Must enter shipping zip code";
			return(0);
		}
		if(!cShipCountry[0])
		{
			SetCustomerFieldsOn();
			cShipCountryStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Must enter shipping country information";
			return(0);
		}
		
	}
	if(cBankName[0])
	{
		if(!cBranchName[0])
		{
			SetCustomerFieldsOn();
			cBranchNameStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Must enter branch name";
			return(0);
		}
		if(!cBranchCode[0])
		{
			SetCustomerFieldsOn();
			cBranchCodeStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Must enter branch code";
			return(0);
		}
	        if(!cAccountHolder[0])
		{
			SetCustomerFieldsOn();
			cAccountHolderStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Must enter account holder name";
			return(0);
		}	
		if(!cAccountNumber[0])
		{
			SetCustomerFieldsOn();
			cAccountNumberStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Must enter account number";
			return(0);
		}
		if(!uAccountType)
		{
			SetCustomerFieldsOn();
			cuAccountTypeStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Must select account type";
			return(0);
		}
	}	
	return(1);
		
}//unsigned ValidateInput(void)


void SetCustomerFieldsOn(void)
{
	cFirstNameStyle="type_fields";
	cLastNameStyle="type_fields";
	cEmailStyle="type_fields";
	cAddr1Style="type_fields";
	cAddr2Style="type_fields";
	cAddr3Style="type_fields";
	cCityStyle="type_fields";
	cStateStyle="type_fields";
	cZipStyle="type_fields";
	cCountryStyle="type_fields";
	cCardTypeStyle="type_fields";
	cCardNumberStyle="type_fields";
	cuExpMonthStyle="type_fields";
	cuExpYearStyle="type_fields";
	cCardNameStyle="type_fields";
	cShipNameStyle="type_fields";
	cShipAddr1Style="type_fields";
	cShipAddr2Style="type_fields";
	cShipAddr3Style="type_fields";
	cShipCityStyle="type_fields";
	cShipStateStyle="type_fields";
	cShipZipStyle="type_fields";
	cShipCountryStyle="type_fields";
	cTelephoneStyle="type_fields";
	cFaxStyle="type_fields";
	cACHDebitsStyle="type_fields";
	cPasswdStyle="type_fields";
	cuPaymentStyle="type_fields";
	cIDNumberStyle="type_fields";
	cBankNameStyle="type_fields";
	cBranchNameStyle="type_fields";
	cBranchCodeStyle="type_fields";
	cAccountHolderStyle="type_fields";
	cAccountNumberStyle="type_fields";
	cuAccountTypeStyle="type_fields";
	cMobileStyle="type_fields";
	cLanguageStyle="type_fields";
	cuPermStyle="type_fields";

}//void SetCustomerFieldsOn(void)


void funcCustomerProducts(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	if(!uCustomer) return;

	fprintf(fp,"<!-- funcCustomerProducts(fp) start -->\n");

	sprintf(gcQuery,"SELECT tInstance.uInstance,tInstance.cLabel,FROM_UNIXTIME(GREATEST(tInstance.uCreatedDate,tInstance.uModDate)),tStatus.cLabel FROM tInstance,tStatus WHERE tInstance.uClient='%u' AND tInstance.uStatus=tStatus.uStatus ORDER BY GREATEST(tInstance.uCreatedDate,tInstance.uModDate)",
		uCustomer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		FromMySQLDate(field[2]);
		fprintf(fp,"<tr><td><a href=ispAdmin.cgi?gcPage=Instance&uInstance=%s&uCustomer=%u>%s (Instance %s)</a></td><td>%s</td><td>%s</td></tr>\n",
				field[0]
				,uCustomer
				,field[1]
				,field[0]
				,field[3]
				,field[2]
		       );
	}

	fprintf(fp,"<!-- funcCustomerProducts(fp) end -->\n");

}//void funcCustomerProducts(FILE *fp)


void funcSelectPayment(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	fprintf(fp,"<!-- funcSelectPayment(fp) start -->\n");
	
	sprintf(gcQuery,"SELECT uPayment,cLabel FROM tPayment ORDER BY cLabel");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
		
	fprintf(fp,"<select class=%s %s title='Select the customer payment method' name=uPayment>\n",cuPaymentStyle,gcInputStatus);

	sprintf(gcQuery,"%u",uPayment);

	fprintf(fp,"<option value=0 ");
	if(!uPayment)
		fprintf(fp,"selected");
	fprintf(fp,">---</option>\n");
	
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<option value=%s ",field[0]);
		if(!strcmp(field[0],gcQuery))
				fprintf(fp,"selected");
		fprintf(fp,">%s</option>\n",field[1]);
	}

	fprintf(fp,"</select>\n");

	fprintf(fp,"<!-- funcSelectPayment(fp) end -->\n");

}//void funcSelectPayment(FILE *fp)


void funcSelectAccountType(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	fprintf(fp,"<!-- funcSelectAccountType(fp) start -->\n");
	
	sprintf(gcQuery,"SELECT uAccountType,cLabel FROM tAccountType ORDER BY cLabel");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
		
	fprintf(fp,"<select class=%s %s title='Select the account type' name=uAccountType>\n",cuAccountTypeStyle,gcInputStatus);

	sprintf(gcQuery,"%u",uAccountType);

	fprintf(fp,"<option value=0 ");
	if(!uAccountType)
		fprintf(fp,"selected");
	fprintf(fp,">---</option>\n");
	
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<option value=%s ",field[0]);
		if(!strcmp(field[0],gcQuery))
				fprintf(fp,"selected");
		fprintf(fp,">%s</option>\n",field[1]);
	}
	
	fprintf(fp,"<!-- funcSelectAccountType(fp) end -->\n");
	
}//void funcSelectAccountType(FILE *fp)


void funcSelectCardType(FILE *fp)
{
	fprintf(fp,"<!-- funcSelectCardType(fp) start cCardType='%s' -->\n",cCardType);
	
	fprintf(fp,"<select class=%s name=cCardType %s>\n",cCardTypeStyle,gcInputStatus);
	
	fprintf(fp,"<option ");
	if(!cCardType[0] || !strcmp(cCardType,"---"))
		fprintf(fp,"selected");
	fprintf(fp,">---</option>\n");
	
	fprintf(fp,"<option ");
	if(!strcmp(cCardType,"American Express"))
		fprintf(fp,"selected");
	fprintf(fp,">American Express</option>\n");

	fprintf(fp,"<option ");
	if(!strcmp(cCardType,"Discover"))
		fprintf(fp,"selected");
	fprintf(fp,">Discover</option>\n");

	fprintf(fp,"<option ");
	if(!strcmp(cCardType,"Mastercard"))
		fprintf(fp,"selected");
	fprintf(fp,">Mastercard</option>\n");

	fprintf(fp,"<option ");
	if(!strcmp(cCardType,"Visa"))
		fprintf(fp,"selected");
	fprintf(fp,">Visa</option>\n");

	fprintf(fp,"</select>\n");
	
	
	fprintf(fp,"<!-- funcSelectCardType(fp) end -->\n");
	
}//void funcSelectCardType(FILE *fp)


void funcSelectExpMonth(FILE *fp)
{
	fprintf(fp,"<!-- funcSelectExpMonth(fp) start -->\n");

	fprintf(fp,"<select class=%s name=uExpMonth %s>\n",cuExpMonthStyle,gcInputStatus);
	
	fprintf(fp,"<option value=0 ");
	if(uExpMonth==0)
		fprintf(fp,"selected");
	fprintf(fp,">---</option>\n");

	fprintf(fp,"<option value=1 ");
	if(uExpMonth==1)
		fprintf(fp,"selected");
	fprintf(fp,">January</option>\n");

	fprintf(fp,"<option value=2 ");
	if(uExpMonth==2)
		fprintf(fp,"selected");
	fprintf(fp,">February</option>\n");

	fprintf(fp,"<option value=3 ");
	if(uExpMonth==3)
		fprintf(fp,"selected");
	fprintf(fp,">March</option>\n");

	fprintf(fp,"<option value=4 ");
	if(uExpMonth==4)
		fprintf(fp,"selected");
	fprintf(fp,">April</option>\n");

	fprintf(fp,"<option value=5 ");
	if(uExpMonth==5)
		fprintf(fp,"selected");
	fprintf(fp,">May</option>\n");

	fprintf(fp,"<option value=6 ");
	if(uExpMonth==6)
		fprintf(fp,"selected");
	fprintf(fp,">June</option>\n");

	fprintf(fp,"<option value=7 ");
	if(uExpMonth==7)
		fprintf(fp,"selected");
	fprintf(fp,">July</option>\n");

	fprintf(fp,"<option value=8 ");
	if(uExpMonth==8)
		fprintf(fp,"selected");
	fprintf(fp,">August</option>\n");

	fprintf(fp,"<option value=9 ");
	if(uExpMonth==9)
		fprintf(fp,"selected");
	fprintf(fp,">September</option>\n");

	fprintf(fp,"<option value=10 ");
	if(uExpMonth==10)
		fprintf(fp,"selected");
	fprintf(fp,">October</option>\n");

	fprintf(fp,"<option value=11 ");
	if(uExpMonth==11)
		fprintf(fp,"selected");
	fprintf(fp,">November</option>\n");

	fprintf(fp,"<option value=12 ");
	if(uExpMonth==12)
		fprintf(fp,"selected");
	fprintf(fp,">December</option>\n");

	fprintf(fp,"</select>\n");
	
	fprintf(fp,"<!-- funcSelectExpMonth(fp) end -->\n");
	
}//void funcSelectExpMonth(FILE *fp)


void funcSelectExpYear(FILE *fp)
{
	fprintf(fp,"<!-- funcSelectExpYear(fp) start -->\n");
	
	fprintf(fp,"<select class=%s name=uExpYear %s>\n",cuExpYearStyle,gcInputStatus);
	
	fprintf(fp,"<option value=0 ");
	if(uExpYear==0)
		fprintf(fp,"selected");
	fprintf(fp,">---</option>\n");

	fprintf(fp,"<option value=2007 ");
	if(uExpYear==2007)
		fprintf(fp,"selected");
	fprintf(fp,">2007</option>\n");

	fprintf(fp,"<option value=2008 ");
	if(uExpYear==2008)
		fprintf(fp,"selected");
	fprintf(fp,">2008</option>\n");

	fprintf(fp,"<option value=2009 ");
	if(uExpYear==2009)
		fprintf(fp,"selected");
	fprintf(fp,">2009</option>\n");

	fprintf(fp,"<option value=2010 ");
	if(uExpYear==2010)
		fprintf(fp,"selected");
	fprintf(fp,">2010</option>\n");

	fprintf(fp,"<option value=2011 ");
	if(uExpYear==2011)
		fprintf(fp,"selected");
	fprintf(fp,">2011</option>\n");

	fprintf(fp,"<option value=2012 ");
	if(uExpYear==2012)
		fprintf(fp,"selected");
	fprintf(fp,">2012</option>\n");

	fprintf(fp,"</select>\n");
	
	fprintf(fp,"<!-- funcSelectExpYear(fp) end -->\n");
	
}//void funcSelectExpYear(FILE *fp)


void funcSelectLanguage(FILE *fp)
{
	fprintf(fp,"<select name=cLanguage title='Select language for invoice rendering' class=%s %s>\n",cLanguageStyle,gcInputStatus);

	fprintf(fp,"<option ");
	if(!strcmp(cLanguage,"English"))
		fprintf(fp,"selected ");
	fprintf(fp,">English</option>\n");

	fprintf(fp,"<option ");
	if(!strcmp(cLanguage,"French"))
		fprintf(fp,"selected ");
	fprintf(fp,">French</option>\n");
	
	fprintf(fp,"<option ");
	if(!strcmp(cLanguage,"Spanish"))
		fprintf(fp,"selected ");
	fprintf(fp,">Spanish</option>\n");

	fprintf(fp,"</select>\n");

}//void funcSelectLanguage(FILE *fp)


void funcTypeOfAccess(FILE *fp)
{
	fprintf(fp,"<select name=uPerm title='Select type of access to the interfaces' class='%s' %s>\n",cuPermStyle,gcInputStatus);

	fprintf(fp,"<option value=1 ");
	if(uPerm==1)
		fprintf(fp,"selected ");
	fprintf(fp,">End-User</option>\n");

	fprintf(fp,"<option value=10 ");
	if(uPerm>=10)
		fprintf(fp,"selected ");
	fprintf(fp,">Admin</option>\n");

	fprintf(fp,"</select>\n");

}//void funcTypeOfAccess(FILE *fp)


unsigned CustomerHasProducts(void)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uInstance FROM tInstance WHERE uClient='%u' AND "
			"(uStatus='%u' OR uStatus='%u' OR uStatus='%u' OR uStatus='%u'"
			" OR uStatus='%u'  OR uStatus='%u')",
			uCustomer
			,mysqlISP_WaitingInitial 
			,mysqlISP_DeployedMod 
			,mysqlISP_OnHold 
			,mysqlISP_WaitingRedeployment 
			,mysqlISP_WaitingHold 
			,mysqlISP_Deployed 
	       );
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	return((unsigned)mysql_num_rows(res));
	
}//unsigned CustomerHasProducts(void)


void UpdateAuthorization(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uAuthorize=0;

	sprintf(gcQuery,"SELECT uAuthorize FROM tAuthorize WHERE uCertClient=%u",uCustomer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if(!mysql_num_rows(res))
	{
		sprintf(gcQuery,"INSERT INTO tAuthorize SET uCertClient=%u,"
				"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
				,uCustomer
				,guOrg
				,guLoginClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		uAuthorize=mysql_insert_id(&gMysql);
	}
	else
	{
		field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uAuthorize);
	}

	if(strncmp(cPasswd,"..",2))
		EncryptPasswdWithSalt(cPasswd,"..");

	sprintf(gcQuery,"UPDATE tAuthorize SET cLabel='%s %s',uPerm=%u,cPasswd='%s',uModBy=%u,"
			"uModDate=UNIX_TIMESTAMP(NOW()) WHERE uAuthorize=%u"
			,cFirstName
			,cLastName
			,uPerm
			,cPasswd
			,guLoginClient
			,uAuthorize);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void UpdateAuthorization(void)


//Product deployment wizard functions start.
//
void funcSelectProduct(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcSelectProduct(fp) start -->\n");

	sprintf(gcQuery,"SELECT uProduct,cLabel FROM tProduct WHERE uAvailable=1 ORDER BY cLabel");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
		
	fprintf(fp,"<select class=type_fields title='Select the product you want to deploy' name=uProduct>\n");

	sprintf(gcQuery,"%u",uProduct);
	
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<option value=%s ",field[0]);
		if(!strcmp(field[0],gcQuery))
				fprintf(fp,"selected");
		fprintf(fp,">%s</option>\n",field[1]);
	}

	fprintf(fp,"</select\n");

	fprintf(fp,"<!-- funcSelectProduct(fp) end -->\n");

}//void funcSelectProduct(FILE *fp)


unsigned uProductHasServices(unsigned uProduct)
{
	MYSQL_RES *res;
	sprintf(gcQuery,"SELECT tService.uService,tService.cLabel uService FROM tService,tServiceGlue WHERE tServiceGlue.uServiceGroup=%u AND tService.uService=tServiceGlue.uService AND tService.uAvailable=1 ORDER BY tService.cLabel",uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	return((unsigned)mysql_num_rows(res));
	
}//unsigned uProductHasServices(unsigned uProduct)


void funcProductParameterInput(FILE *fp)
{
	//
	//This function will output a table for the product parameters separated by service.
	//As a product can be made of more than one services and each service has its own parameters.
	MYSQL_RES *res;
	MYSQL_ROW field;

	MYSQL_RES *res2;
	MYSQL_ROW field2;

	struct t_template template;

	unsigned uServiceCount=0;
	unsigned uParamCount=0;
	unsigned uISMHide=0;

	char cParamName[100]={""};
	char cuParamCount[10]={""};

//from tproductfunc.h AddService:
//sprintf(gcQuery,"INSERT INTO tServiceGlue SET uServiceGroup=%u,uService=%u",uProduct,uService);
	
	fprintf(fp,"<!--funcProductParameterInput(fp) start -->\n");

	sprintf(gcQuery,"SELECT tService.uService,tService.cLabel uService FROM tService,tServiceGlue WHERE tServiceGlue.uServiceGroup=%u AND "
			"tService.uService=tServiceGlue.uService AND tService.uAvailable=1 ORDER BY tService.cLabel",uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		template.cpName[0]="cServiceLabel";
		template.cpValue[0]=field[1];
			
		template.cpName[1]="uService";
		template.cpValue[1]=field[0];

		sprintf(gcQuery,"%u",uServiceCount);
		template.cpName[2]="uServiceCount";
		template.cpValue[2]=gcQuery;

		template.cpName[3]="";
		
		fpTemplate(fp,"ParameterInputHeader",&template);
//from tservicefunc.h
//sprintf(gcQuery,"INSERT INTO tConfig SET uConfig=0,uGroup=%u,uParameter=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
//                                     uService,
//                                     uParameter,
		sprintf(gcQuery,"SELECT tParameter.cParameter,tParameter.cISMName,cISMHelp,tParamType.cLabel,"
				"tParameter.cExtQuery,tConfig.cValue,tParameter.uISMHide FROM "
				"tParameter,tConfig,tParamType WHERE tConfig.uGroup=%s AND tConfig.uParameter=tParameter.uParameter AND "
				"tParamType.uParamType=tParameter.uParamType ORDER BY tParameter.uISMOrder",
				field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res2=mysql_store_result(&gMysql);

		while((field2=mysql_fetch_row(res2)))
		{
			sprintf(cParamName,"cParam.%u",uParamCount);
			sscanf(field2[6],"%u",&uISMHide);

			template.cpName[0]="cParamLabel";
			template.cpValue[0]=field2[1];
			
			template.cpName[1]="cParamTitle";
			template.cpValue[1]=field2[2];

			template.cpName[2]="cParamName";
			template.cpValue[2]=cParamName;

			template.cpName[3]="cParamMaxLength";
			template.cpValue[3]="64"; //TODO: read cRange and decide based on uParamType

			template.cpName[4]="cParamValue";
			if(cParameterInput[uParamCount][0])
				template.cpValue[4]=cParameterInput[uParamCount];
			else
				template.cpValue[4]=field2[5];

			sprintf(cuParamCount,"%u",uParamCount);
			template.cpName[5]="uParamCount";
			template.cpValue[5]=cuParamCount;
			
			template.cpName[6]="cParameter";
			template.cpValue[6]=field2[0];

			template.cpName[7]="cParamStyle";
			template.cpValue[7]=cParameterStyle[uParamCount];

			template.cpName[8]="";
			
			if(strcmp(field2[3],"extselect"))
			{
				if(uISMHide)
					fpTemplate(fp,"ParameterHiddenInputField",&template);
				else
					fpTemplate(fp,"ParameterInputField",&template);
			}
			else
			{
			//extselect drop-down entry parameters
				char *cp;
				MYSQL_RES *res4;
				MYSQL_ROW field4;
				
				if((cp=strchr(field2[4],':')))
				{
					char cConfQuery[256];
					char cIp[256]={""};
					char cDb[256]={""};
					char cPwd[256]={""};
					char cLogin[256]={""};

					*cp=0;

					sprintf(cConfQuery,"%.200sIp",field2[4]);
					GetConfiguration(cConfQuery,cIp);
					sprintf(cConfQuery,"%.200sName",field2[4]);
					GetConfiguration(cConfQuery,cDb);
					sprintf(cConfQuery,"%.200sPwd",field2[4]);
					GetConfiguration(cConfQuery,cPwd);
					sprintf(cConfQuery,"%.200sLogin",field2[4]);
					GetConfiguration(cConfQuery,cLogin);
		
					//Must add tConfiguration parameters	
					//debug only	
					//sprintf(gcQuery,"cIP:%s\n cDb:%s\n cPwd:%s\n cLogin:%s\n",
					//cIp,cDb,cPwd,cLogin);
					//htmlPlainTextError(gcQuery);	
					if(cDb[0]==0 || cPwd[0]==0 || cLogin[0]==0)
					{
						fprintf(fp,"Can't display parameter input (%s). Configuration error",field2[1]);
						continue;
					}
		
					ExtConnectDb(cDb,cIp,cPwd,cLogin);

					sprintf(gcQuery,"%s",field2[5]);
						//debug only
					//htmlPlainTextError(gcQuery);	
					mysql_query(&MysqlExt,gcQuery);
        				if(mysql_errno(&MysqlExt))
                				htmlPlainTextError(mysql_error(&MysqlExt));
					
					res4=mysql_store_result(&MysqlExt);
					fpTemplate(fp,"ParameterInputSelectHeader",&template);	

					//Show select options
					while((field4=mysql_fetch_row(res4)))
					{
						fprintf(fp,"<option ");
						if(!strcmp(cParameterInput[uParamCount],field4[0]))
							fprintf(fp,"selected");
						fprintf(fp,">%s</option>\n",field4[0]);
					}

					template.cpName[0]="";
					fpTemplate(fp,"ParameterInputSelectFooter",&template);
				}

			}//end extselect drop-down entry parameters
			uParamCount++;
		}//Parameters loop

		template.cpName[0]="";
		fpTemplate(fp,"ParameterInputFooterWizard",&template);
		uServiceCount++;
		
	}//Services loop
	
	fprintf(fp,"<input type=hidden name=uParamCount value=%u>\n",uParamCount);
	fprintf(fp,"<!--funcProductParameterInput(fp) end -->\n");
	
}//void funcProductParameterInput(FILE *fp)


void funcEnteredParameters(FILE *fp)
{
	register int i=0,n=0;
	struct t_template template;
	char cuService[16]={""};

	fprintf(fp,"<!-- funcEnteredParameters(fp) start -->\n");
	
	i=0;
	for(n=0;n<uServicesCount;n++)
	{
		template.cpName[0]="cServiceLabel";
		template.cpValue[0]=cGetServiceLabel(uServices[n]);
		
		sprintf(cuService,"%u",uServices[n]);
		template.cpName[1]="uService";
		template.cpValue[1]=cuService;

		sprintf(gcQuery,"%i",n);
		template.cpName[2]="uServiceCount";
		template.cpValue[2]=gcQuery;

		template.cpName[3]="";
		fpTemplate(fp,"EnteredParametersHeader",&template);
		
		for(i=uServiceStartPoint[n];i<uServiceEndPoint[n];i++)
		{
			
			template.cpName[0]="cParamLabel";
			template.cpValue[0]=cParameterLabel[i];
		
			template.cpName[1]="cParamValue";
			template.cpValue[1]=cParameterInput[i];
			
			template.cpName[2]="cParameter";
			template.cpValue[2]=cParameterName[i];

			sprintf(gcQuery,"%i",i);
			template.cpName[3]="iParamIndex";
			template.cpValue[3]=gcQuery;
		
			template.cpName[4]="";
			fpTemplate(fp,"EnteredParametersItem",&template);
		}//Parameters loop
		
		template.cpName[0]="";	
		fpTemplate(fp,"EnteredParametersFooter",&template);

	}//Services loop
	
	fprintf(fp,"<!-- funcEnteredParameters(fp) end -->\n");
	
}//void funcEnteredParameters(FILE *fp)


unsigned uIsParameterInService(unsigned uService,char *cParamName)
{
	MYSQL_RES *res;
	
	if(!cParamName[0]) return(0);
	sprintf(gcQuery,"SELECT tConfig.uConfig FROM tConfig,tService,tParameter WHERE tParameter.uParameter=tConfig.uParameter AND tService.uService=tConfig.uGroup AND tParameter.cParameter='%s' AND tService.uService='%u'",cParamName,uService);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	return(unsigned)mysql_num_rows(res);

}//unsigned uIsParameterInService(unsigned uService,char *cParamName)


char *cGetServiceLabel(unsigned uService)
{
	static char cLabel[33]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel FROM tService WHERE uService='%u'",uService);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sprintf(cLabel,"%.32s",field[0]);

	mysql_free_result(res);

	return(cLabel);

}//char *cGetServiceLabel(unsigned uService)


char *cGetProductLabel(unsigned uProduct)
{
	static char cLabel[33]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel FROM tProduct WHERE uProduct='%u'",uProduct);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sprintf(cLabel,"%.32s",field[0]);

	mysql_free_result(res);

	return(cLabel);

}//char *cGetProductLabel(unsigned uProduct)


unsigned uGetParameter(char *cParameter)
{
	unsigned uParmeter;
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uParameter FROM tParameter WHERE cParameter='%s'",cParameter);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uParmeter);

	mysql_free_result(res);

	return(uParmeter);
	
}//unsigned uGetParameter(char *cParameter)


void DeployProduct(void)
{
	unsigned uGroup=0;
	register int i=0,n=0;
	time_t luClock;
	time_t uDeployTime;	

	time(&luClock);
	
	sprintf(gcQuery,"INSERT INTO tInstance SET uClient='%u',uProduct='%u',uOwner='%u',uCreatedBy='%u',uCreatedDate=UNIX_TIMESTAMP(NOW()),uStatus='%u',cLabel='%s'",
			uCustomer
			,uProduct
			,guLoginClient
			,guLoginClient
			,mysqlISP_WaitingInitial
			,cGetProductLabel(uProduct)
			);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	uGroup=mysql_insert_id(&gMysql);
	
	for(n=0;n<uServicesCount;n++)
	{
		for(i=uServiceStartPoint[n];i<uServiceEndPoint[n];i++)
		{
			sprintf(gcQuery,"INSERT INTO tClientConfig SET uParameter='%u',cValue='%s',uGroup='%u',uService='%u',uOwner='%u',uCreatedBy='%u',uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uGetParameter(cParameterName[i])
					,cParameterInput[i]
					,uGroup
					,uServices[n]
					,uCustomer
					,guLoginClient
			      		);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
						
		}//Parameters loop
	}//Services loop
	
	if(cDay[0] && cDay[0]!='N')
	{
		sprintf(gcQuery,"%s-%s-%s",cDay,cMonth,cYear);
		uDeployTime=cDateToUnixTime(gcQuery);
	}
	else
	{
		uDeployTime=luClock;
	}
	
	SubmitInstanceJob("New",uDeployTime,uGroup,1);
	
}//void DeployProduct(void)


void ReaduPrevStatus(unsigned uInstance)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char *cp;

        sprintf(gcQuery,"SELECT cJobData FROM tJob WHERE uInstance=%u AND uJobStatus=%u"
						,uInstance,mysqlISP_Waiting);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		if((cp=strstr(field[0],"uPrevStatus=")))
			sscanf(cp+12,"%u",&uPrevStatus);
	}
	mysql_free_result(res);

}//void ReaduPrevStatus(void)


//tProductType.txt dependent define
#define mysqlISP_ProductType_BILLINGONLY 5
void SubmitInstanceJob(char *cJobType,time_t uJobDate,unsigned uInstance,unsigned uExternalCall)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uService=0;
	unsigned uJob=0;
	unsigned uJobGroup=0;
	unsigned uProductType=0;
	unsigned uFirst=1;
	char cJobName[100];
	unsigned uProduct=0;

	//printf("Content-type:text/plain\n\nSubmitInstanceJob(%s,%lu,%u)",cJobType,uJobDate,uInstance);exit(0);
	//Do not submit jobs for products of billing type
	
	if(!uExternalCall)
	        sprintf(gcQuery,"SELECT uProductType FROM tProduct WHERE uProduct=%u",uProduct);
	else
		sprintf(gcQuery,"SELECT tProduct.uProduct,tProduct.uProductType FROM tProduct,tInstance WHERE tProduct.uProduct=tInstance.uProduct AND tInstance.uInstance='%u'",uInstance);
	
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		if(uExternalCall)
		{
			sscanf(field[0],"%u",&uProduct);
			sscanf(field[1],"%u",&uProductType);
		}
		else
			sscanf(field[0],"%u",&uProductType);
	}
	mysql_free_result(res);

	if(uProductType==mysqlISP_ProductType_BILLINGONLY)
		return;

	ReaduPrevStatus(uInstance);

	//Remove all waiting jobs of same product instance
        sprintf(gcQuery,"DELETE FROM tJob WHERE uInstance=%u AND uJobStatus=%u"
						,uInstance,mysqlISP_Waiting);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));

	//One job for each service that comprise the product
        sprintf(gcQuery,"SELECT tService.uService,tService.cJobName,tService.cServer FROM tService,tServiceGlue WHERE tServiceGlue.uService=tService.uService AND tServiceGlue.uServiceGroup=%u",uProduct);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);

        while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uService);
		sprintf(cJobName,"%.81s.%.18s",field[1],cJobType);
		if(strchr(field[2],';'))
		{
			//More that one server in tService.cServer. Wil submit one job per each
			char *cServer;
			
			cServer=strtok(field[2],";");
			//First server in list:
			SubmitServiceJob(uService,cJobName,cServer,uInstance,uJobDate);
			while((cServer=strtok(NULL,";"))!=NULL)
				SubmitServiceJob(uService,cJobName,cServer,uInstance,uJobDate);
		}
		else
			//Single server service
			SubmitServiceJob(uService,cJobName,field[2],uInstance,uJobDate);

		uJob=mysql_insert_id(&gMysql);
		if(uFirst)
		{
			uFirst=0;
			uJobGroup=uJob;
		}
		sprintf(gcQuery,"UPDATE tJob SET uJobGroup=%u WHERE uJob=%u",uJobGroup
				,uJob);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

}//void SubmitInstanceJob(char *cJobType)


void SubmitSingleServiceJob(unsigned uService,unsigned uInstance,char *cJobType,unsigned uExtClient,time_t uJobDate)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cJobName[100];
	unsigned uJob=0;
	
	//One job for specified service 
        sprintf(gcQuery,"SELECT cJobName,cServer FROM tService WHERE uService=%u",
						uService);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
	
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sprintf(cJobName,"%.81s.%.18s",field[0],cJobType);

		//Remove all waiting jobs of same product instance and service by 
		//jobname: Study this further
sprintf(gcQuery,"DELETE FROM tJob WHERE uInstance=%u AND uJobStatus=%u AND cJobName='%s'",uInstance,mysqlISP_Waiting,cJobName);
		mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		uCustomer=uExtClient;
		SubmitServiceJob(uService,cJobName,field[1],uInstance,uJobDate);

		uJob=mysql_insert_id(&gMysql);
		sprintf(gcQuery,"UPDATE tJob SET uJobGroup=%u WHERE uJob=%u",uJob,uJob);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

}//void SubmitSingleServiceJob(unsigned uService,char *cJobType)


void SubmitServiceJob(unsigned uService,char *cJobName, char *cServer,unsigned uInstance,time_t uJobDate)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cJob[2048]={""};
	char cLine[256]={""};
	char cPrevStatus[16]={""};
	time_t luClock;

        sprintf(gcQuery,"SELECT tParameter.cParameter,tClientConfig.cValue FROM tClientConfig,tParameter WHERE tClientConfig.uParameter=tParameter.uParameter AND tClientConfig.uGroup=%u AND tClientConfig.uService=%u ORDER BY tParameter.cParameter",uInstance,uService);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{
		sprintf(cLine,"%.125s=%.125s\n",field[0],field[1]);
		if(strlen(cJob)+strlen(cLine)<1980)
			strcat(cJob,cLine);
		else
                	htmlPlainTextError("cJob length exceeded. SubmitInstanceJob()");
	}
	mysql_free_result(res);

	//New for ISP wide uISPClient
	sprintf(cLine,"uISPClient=%u\ncClientName=%s",
					uCustomer,
					ForeignKey("tClient","cLabel",uCustomer));
	strcat(cJob,cLine);
	
	if(strstr(cJobName,".Hold") || strstr(cJobName,".Cancel"))
	{
		sprintf(cPrevStatus,"\nuPrevStatus=%u",uPrevStatus);
		strcat(cJob,cPrevStatus);
	}
	
	time(&luClock);
			
        sprintf(gcQuery,"INSERT INTO tJob SET uInstance=%u,cLabel='%s',cJobData='%s',cJobName='%s',cServer='%s',uJobClient=%u,uJobDate=%lu,uJobStatus=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
			uInstance,
			"Priority=Normal",
			cJob,
			cJobName,
			cServer,
			uCustomer,
			uJobDate,
			mysqlISP_Waiting,
			guOrg,
			guLoginClient,
			(unsigned long)luClock);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));

}//void SubmitServiceJob()


void GetConfiguration(const char *cName, char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cQuery[512]={""};

        sprintf(cQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='%s'",cName);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
    	      htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
                sprintf(cValue,"%s",field[0]);
        mysql_free_result(res);

}//void GetConfiguration(char *cName, char *cValue)


int CheckParameterValue(char *cValue, char *cParamType, char *cRange, unsigned uUnique,
		char *cExtVerify, char *cParameter, unsigned uDeployed)
{
	unsigned uLow=0,uHigh=0,uErr=0,uLen=0;
	unsigned register int i;
	char *cp;
	char query[512];
	MYSQL_RES *res;

	sscanf(cRange,"%u,%u",&uLow,&uHigh);
	if(!strcmp(cParamType,"integer") ||
			!strcmp(cParamType,"yes/no") ||
			!strcmp(cParamType,"on/off") ||
			!strcmp(cParamType,"megabytes")
	)
		sscanf(cValue,"%u",&uLen);
	else
		uLen=strlen(cValue);

	if(uLen>uHigh || uLen<uLow)
		return(2);

	if(strstr(cValue,"{{"))
		return(3);

	if((cp=strchr(cValue,'\''))) *cp='`';

	if(!strcmp(cParamType,"integer"))
	{
	}
	else if(!strcmp(cParamType,"lowercaselogin"))
	{
		for(i=0;i<strlen(cValue)&& !uErr;i++)
		{
			if(!isalnum(cValue[i]) && 
				(!i || ( cValue[i]!='.' && 
				cValue[i]!='/' && cValue[i]!='-' && cValue[i]!='_' && cValue[i]!=':'))  ) uErr=1;
			if(isupper(cValue[i])) cValue[i]=tolower(cValue[i]);
		}
	}
	else if(!strcmp(cParamType,"encryptedpasswd"))
	{
		if(strchr(cValue,' ')) uErr=1;
		if(!uErr && !(cValue[0]=='$' && cValue[1]=='1'))
			EncryptPasswdMD5(cValue);
	}
	else if(!strcmp(cParamType,"string"))
	{
		if((cp=strchr(cValue,'\''))) *cp='`';
	}
	else if(!strcmp(cParamType,"megabytes"))
	{
	}
	else if(!strcmp(cParamType,"word"))
	{
		if(strchr(cValue,' ')) uErr=1;
	}
	else if(!strcmp(cParamType,"ipv6"))
	{
		//Not supported yet
		uErr=1;
	}
	else if(!strcmp(cParamType,"ip"))
	{
		unsigned a=999,b=999,c=999,d=999;
		
		sscanf(cValue,"%u.%u.%u.%u",&a,&b,&c,&d);
		if( a>255 || b>255 || c>255 || d>255) uErr=1;
		if(strchr(cValue,' ')) uErr=1;
	}
	else if(!strcmp(cParamType,"fqdn"))
	{
		FQDomainName(cValue);
		if(!strchr(cValue,'.')) uErr=1;
	}
	else if(!strcmp(cParamType,"email"))
	{
		for(i=0;i<strlen(cValue)&& !uErr;i++)
		{
			if(!isalpha(cValue[i]) && 
				(!i || ( cValue[i]!='@' && 
				cValue[i]!='.'))  ) uErr=1;
			if(isupper(cValue[i])) cValue[i]=tolower(cValue[i]);
		}

		if(!strchr(cValue,'.')) uErr=1;
		if(!strchr(cValue,'@')) uErr=1;
	}
	else if(!strcmp(cParamType,"yes/no"))
	{
		WordToLower(cValue);
		if(strcmp(cValue,"yes") && strcmp(cValue,"no")) uErr=1;
	}
	else if(!strcmp(cParamType,"on/off"))
	{
		WordToLower(cValue);
		if(strcmp(cValue,"on") && strcmp(cValue,"off")) uErr=1;
	}

	
	//TODO Quick configure hack: uDeployed=7. If deployed skip this check.
	//fix this lameass 7 hack for uDeployed>0 except 7 bullshit
	if(cExtVerify[0] && ( !uDeployed || uDeployed==7 ) )
	{

		//Needs tConfiguration cExtmysql{{Whatever}}Ip/Db/Pwd/Name set.
		//Usual example cExtmysqlRadiusIp/Db/Pwd/Name set.
		if((cp=strchr(cExtVerify,':')))
		{
			char cConfQuery[256];
			char cIp[256]={""};
			char cDb[256]={""};
			char cPwd[256]={""};
			char cLogin[256]={""};

			*cp=0;

			sprintf(cConfQuery,"%.200sIp",cExtVerify);
			GetConfiguration(cConfQuery,cIp);
			sprintf(cConfQuery,"%.200sName",cExtVerify);
			GetConfiguration(cConfQuery,cDb);
			sprintf(cConfQuery,"%.200sPwd",cExtVerify);
			GetConfiguration(cConfQuery,cPwd);
			sprintf(cConfQuery,"%.200sLogin",cExtVerify);
			GetConfiguration(cConfQuery,cLogin);
		
			//Must add tConfiguration parameters	
//debug only	
//			sprintf(gcQuery,"cExtVerify:%s:\n cConfQuery:%s:\n cIP:%s\n cDb:%s\n cPwd:%s\n cLogin:%s\n Verify query:(%s)",cExtVerify,cConfQuery,cIp,cDb,cPwd,cLogin,cp+1);
//			htmlPlainTextError(gcQuery);	
			if(cDb[0]==0 || cPwd[0]==0 || cLogin[0]==0)
				return(uErr=5);
		
			ExtConnectDb(cDb,cIp,cPwd,cLogin);

			sprintf(gcQuery,cp+1,cValue);
			//debug only
			//htmlPlainTextError(gcQuery);	
			mysql_query(&MysqlExt,gcQuery);
        		if(mysql_errno(&MysqlExt))
                		htmlPlainTextError(mysql_error(&MysqlExt));
			res=mysql_store_result(&MysqlExt);

		
			if(strstr(query,"ReverseTest"))
			{
				//If no rows fail
				if(!mysql_num_rows(res))
					uErr=5;
			}
			else
			{
				//If at least one row fail
				if(mysql_num_rows(res))
					uErr=5;
			}
			mysql_free_result(res);
			*cp=':';
//debug Only
//sprintf(gcQuery,"uErr=%u",uErr);
//htmlPlainTextError(gcQuery);	

			if(uErr) return(uErr);
		}
	}

//debug Only
//sprintf(gcQuery,"uErr=%u",uErr);
//htmlPlainTextError(gcQuery);	

	return(uErr);

}//int CheckParameterValue()


void CheckAllParameters(void)
{
	unsigned uErr=0;
	register int i=0;
	structParameter ParamData;
	static char cMessage[1024]={""};
//printf("Content-type: text/plain\n\n");
	for(i=0;i<uParameterCount;i++)
	{
//int CheckParameterValue(char *cValue, char *cParamType, char *cRange, unsigned uUnique,char *cExtVerify
		GetParamInfo(cParameterName[i],&ParamData);
		if(!cParameterName[i][0]) continue;
/*		printf("ParamData.cParamType=%s ParamData.cRange=%s ParamData.uUnique=%u ParamData.cExtVerify=%s cParameterName=%s\n"
						,ParamData.cParamType
						,ParamData.cRange
						,ParamData.uUnique
						,ParamData.cExtVerify
						,cParameterName[i]);
*/
		if((uErr=CheckParameterValue(cParameterInput[i]
						,ParamData.cParamType
						,ParamData.cRange
						,ParamData.uUnique
						,ParamData.cExtVerify
						,cParameterName[i]
						,0)))
		{	
			gcQuery[0]=0;
			if(uErr==2)
			sprintf(cMessage,"%s='%s' is out of range %s, type %s (i=%u)"
					,cParameterLabel[i],cParameterInput[i],ParamData.cRange,ParamData.cParamType,i);
			else if(uErr==1)
sprintf(cMessage,"%s='%s' is not valid for type %s, range %s",cParameterLabel[i],cParameterInput[i],ParamData.cParamType,ParamData.cRange);
			else if(uErr==3)
sprintf(cMessage,"%s='%s' unconfigured type %s, range %s",cParameterLabel[i],cParameterInput[i],ParamData.cParamType,ParamData.cRange);
			else if(uErr==5)
sprintf(cMessage,"%s='%s' remote verify failed",cParameterLabel[i],cParameterInput[i]);
			gcMessage=cMessage;
			cParameterStyle[i]="type_fields_req";
			htmlProductDeployWizard(uStep);
		}
	}
//	exit(0);
}//void CheckAllParameters()


void ExtConnectDb(char *cDbName, char *cDbIp, char *cDbPwd, char *cDbLogin)
{
	if(!cDbIp[0]) cDbIp=NULL;

        mysql_init(&MysqlExt);
        if (!mysql_real_connect(&MysqlExt,cDbIp,cDbLogin,cDbPwd,cDbName,0,NULL,0))
        {
		sprintf(gcQuery,"ExtConnectDb failed for cDbIp:%s: cDbName:%s: cDbLogin:%s: cDbPwd:%s:",cDbIp,cDbName,cDbLogin,cDbPwd);
                htmlPlainTextError(gcQuery);
        }

}//end of ExtConnectDb()


void GetParamInfo(const char *cParameter, structParameter *structParam)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        sprintf(gcQuery,"SELECT tParamType.cLabel,tParameter.cRange,tParameter.cParameter,tParameter.uUnique,tParameter.cExtVerify,"
			"tParameter.uModPostDeploy,tParameter.uAllowMod FROM tParameter,tParamType WHERE cParameter='%s' AND "
			"tParameter.uParamType=tParamType.uParamType",cParameter);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
        
	res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		
		strcpy(structParam->cParamType,field[0]);
		strcpy(structParam->cRange,field[1]);
		strcpy(structParam->cParameter,field[2]);
		sscanf(field[3],"%u",&structParam->uUnique);
		strcpy(structParam->cExtVerify,field[4]);
		sscanf(field[5],"%u",&structParam->uModPostDeploy);
		sscanf(field[6],"%u",&structParam->uAllowMod);
	}
	mysql_free_result(res);

}//GetParamInfo()

void SetParamFieldsOn(void)
{
	register int i=0;
	
	for(i=0;i<100;i++)
		cParameterStyle[i]="type_fields";
}


void funcDateSelectTable(FILE *fp)
{
	register int i;
	time_t luClock;
	char cMonth[4]={""};
	char cYear[8]={""};
	struct tm *structTime;
	unsigned uThisYear=0;
	
	time(&luClock);
	structTime=localtime(&luClock);
	strftime(cMonth,4,"%b",structTime);
	strftime(cYear,8,"%Y",structTime);
	sscanf(cYear,"%u",&uThisYear);

	fprintf(fp,"<table width=150>\n");
	fprintf(fp,"<tr><td>Day</td><td>Month</td><td>Year</td></tr>\n");

	fprintf(fp,"<tr>\n");

	fprintf(fp,"<td><select name=cDay>\n");
	fprintf(fp,"<option>Now</option>");
	for(i=1;i<32;i++)
		fprintf(fp,"<option>%u</option>",i);
	fprintf(fp,"</select></td>\n");

	fprintf(fp,"<td><select name=cMonth>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Jan"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Jan</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Feb"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Feb</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Mar"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Mar</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Apr"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Apr</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"May"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"May</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Jun"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Jun</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Jul"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Jul</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Aug"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Aug</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Sep"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Sep</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Oct"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Oct</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Nov"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Nov</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Dec"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Dec</option>\n");

	fprintf(fp,"</select></td>\n");

	fprintf(fp,"<td><font face=arial size=1><select name=cYear>\n");
	for(i=uThisYear;i<uThisYear+5;i++)
		fprintf(fp,"<option>%u</option>",i);
	fprintf(fp,"</select></td>\n");

	fprintf(fp,"</tr>\n");

	fprintf(fp,"</table>\n");

}//void funcDateSelectTable()


time_t cDateToUnixTime(char *cDate)
{
        struct  tm locTime;
        time_t  res;

        bzero(&locTime, sizeof(struct tm));
	if(strchr(cDate,'-'))
        	strptime(cDate,"%d-%b-%Y", &locTime);
	else if(strchr(cDate,'/'))
        	strptime(cDate,"%d/%b/%Y", &locTime);
	else if(strchr(cDate,' '))
        	strptime(cDate,"%d %b %Y", &locTime);
        locTime.tm_sec = 0;
        locTime.tm_min = 0;
        locTime.tm_hour = 0;
        res = mktime(&locTime);
        return(res);
}

unsigned uHasWaitingForApproval(char *uClient);

void funcCustomerNavList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uFound=0;
	unsigned uDisplayed=0;
	
	char *cRowColor="";

	static char cTopMessage[64]={""};

	if(cSearch[0])
	{
		if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		{
			if(uSearchInstance)
				sprintf(gcQuery,"SELECT DISTINCT tClient.uClient,tClient.cLabel FROM " TCLIENT ",tClientConfig WHERE"
						" tClient.uClient=tClientConfig.uOwner AND ( (tClient.cFirstName LIKE '%1$s%%' OR "
						"tClient.cLastName LIKE '%1$s%%')"
						" OR tClientConfig.cValue LIKE '%1$s%%' OR"
						" tClient.cEmail LIKE '%1$s%%' OR"
						" tClient.cCity LIKE '%1$s%%' OR"
						" tClient.cState LIKE '%1$s%%' OR"
						" tClient.cCountry LIKE '%1$s%%' ) ORDER BY cLabel",cSearch);
			else
				sprintf(gcQuery,"SELECT DISTINCT tClient.uClient,tClient.cLabel FROM " TCLIENT " WHERE"
						" tClient.uClient=tClientConfig.uOwner AND ( (tClient.cFirstName LIKE '%1$s%%' OR "
						"tClient.cLastName LIKE '%1$s%%')"
						" OR tClient.cEmail LIKE '%1$s%%' OR"
						" tClient.cCity LIKE '%1$s%%' OR"
						" tClient.cState LIKE '%1$s%%' OR"
						" tClient.cCountry LIKE '%1$s%%' ) ORDER BY cLabel",cSearch);
		}
		else
		{
			if(uSearchInstance)
				sprintf(gcQuery,"SELECT DISTINCT tClient.uClient,tClient.cLabel FROM " TCLIENT ",tClientConfig"
					" WHERE (tClient.uClient=%1$u OR tClient.uOwner"
					" IN (SELECT tClient.uClient FROM " TCLIENT " WHERE tClient.uOwner=%1$u OR"
					" tClient.uClient=%1$u))"
					" AND ( (tClient.cFirstName LIKE '%2$s%%' OR tClient.cLastName LIKE '%2$s%%') OR ("
					"tClientConfig.cValue LIKE '%2$s%%' AND tClient.uClient=tClientConfig.uOwner) OR"
					" tClient.cEmail LIKE '%2$s%%' OR"
					" tClient.cCity LIKE '%2$s%%' OR"
					" tClient.cState LIKE '%2$s%%' OR tClient.cCountry LIKE '%2$s%%' )"
					" ORDER BY cLabel",guOrg,cSearch);
			else
				sprintf(gcQuery,"SELECT DISTINCT tClient.uClient,tClient.cLabel FROM " TCLIENT 
					" WHERE (tClient.uClient=%1$u OR tClient.uOwner"
					" IN (SELECT tClient.uClient FROM " TCLIENT " WHERE tClient.uOwner=%1$u OR"
					" tClient.uClient=%1$u))"
					" AND ( (tClient.cFirstName LIKE '%2$s%%' OR tClient.cLastName LIKE '%2$s%%') "
					" OR tClient.cEmail LIKE '%2$s%%' OR"
					" tClient.cCity LIKE '%2$s%%' OR"
					" tClient.cState LIKE '%2$s%%' OR tClient.cCountry LIKE '%2$s%%' )"
					" ORDER BY cLabel",guOrg,cSearch);
		}

	}
	else if(!cSearch[0] && uCustomer)
	{
		if(guLoginClient==1 && guPermLevel>11)//Root can read access all
			sprintf(gcQuery,"SELECT uClient,cLabel FROM " TCLIENT " WHERE uClient=%u",uCustomer);
		else 
			sprintf(gcQuery,"SELECT uClient,cLabel FROM " TCLIENT
					" WHERE (uClient=%1$u OR uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" AND uClient=%2$u"
					" ORDER BY cLabel",guOrg,uCustomer);
	}
	else
	{
		if(guLoginClient==1 && guPermLevel>11)//Root can read access all
			sprintf(gcQuery,"SELECT uClient,cLabel FROM " TCLIENT " ORDER BY uClient");
		else 
			sprintf(gcQuery,"SELECT uClient,cLabel FROM " TCLIENT
					" WHERE (uClient=%1$u OR uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" ORDER BY cLabel",guOrg);
	}
	//htmlPlainTextError(gcQuery);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	
	if(!(uFound=mysql_num_rows(res)))
	{
		fprintf(fp,"No records found.<br>\n");
		mysql_free_result(res);
		return;
	}

	sprintf(cTopMessage,"%u record(s) found.",uFound);
	gcMessage=cTopMessage;

	if(uFound==1)
	{
		if((field=mysql_fetch_row(res)))
		{
			if(strcmp(gcFunction,"Confirm Modify"))
			{
				sscanf(field[0],"%u",&uCustomer);
				LoadCustomer(uCustomer);
			}

			fprintf(fp,"<a href=ispAdmin.cgi?gcPage=Customer&uCustomer=%s>%s</a><br>\n",field[0],field[1]);
			mysql_free_result(res);
			return;
		}
	}

	while((field=mysql_fetch_row(res)))
	{
		if(uDisplayed==20)
		{
			fprintf(fp,"Only the first 20 records found. If the customer you are looking"
					" for is not in the list above please "
			"further refine your search.<br>\n");
			break;
		}
		if(uHasWaitingForApproval(field[0]))
			cRowColor="red";
		else
			cRowColor="black";

		fprintf(fp,"<a href=ispAdmin.cgi?gcPage=Customer&uCustomer=%s><font color=%s>%s</font>"
				"</a><br>\n",field[0],cRowColor,field[1]);
		uDisplayed++;
	}

	mysql_free_result(res);

}//void funcCustomerNavList(FILE *fp)


void funcCustomerNavBar(FILE *fp)
{
	if(guPermLevel>=10)
		fprintf(fp,"<input type=submit title='Customer modification with a two step procedure'"
				" class=largeButton name=gcFunction value='%sNew' />",gcNewStep);

	if(uAllowMod(uOwner,uCreatedBy))
		fprintf(fp,"<input type=submit title='Customer modification with a two step procedure'"
				" class=largeButton name=gcFunction value='%sModify' />",gcModStep);

	if(uAllowDel(uOwner,uCreatedBy))
		fprintf(fp,"<input type=submit title='Delete a customer with a two step procedure'"
				" class=largeButton name=gcFunction value='%sDelete' />",gcDelStep);

}//void funcCustomerNavBar(FILE *fp)


unsigned uHasWaitingForApproval(char *uClient)
{
	MYSQL_RES *res;
	unsigned uYes=0;

	sprintf(gcQuery,"SELECT uInstance FROM tInstance WHERE uClient=%s AND uStatus=%u",
								uClient,mysqlISP_WaitingApproval);
	mysql_query(&gMysql,gcQuery);

	res=mysql_store_result(&gMysql);
	uYes=mysql_num_rows(res);
	mysql_free_result(res);

	return(uYes);

}//unsigned uHasWaitingForApproval(char *uClient)


void funcCustomerReport(FILE *fp)
{
	//
	//Very simple report for start
	//Customer ID, customer name, products deployed
	MYSQL_RES *res;
	MYSQL_ROW field;

	MYSQL_RES *res2;
	MYSQL_ROW field2;

	sprintf(gcQuery,"SELECT uClient,cFirstName,cLastName FROM tClient WHERE uClient!=1 AND uOwner=%u ORDER BY cLabel",guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<tr><td align=center>%s</td><td align=center>%s %s</td>",field[0],field[1],field[2]);

		sprintf(gcQuery,"SELECT cLabel FROM tInstance WHERE uClient=%s AND uStatus=4",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res2=mysql_store_result(&gMysql);
		
		if((field2=mysql_fetch_row(res2)))
			fprintf(fp,"<td align=center>%s</td></tr>\n",field2[0]);
		else
			fprintf(fp,"<td align=center>None</td></tr>\n");

		while((field2=mysql_fetch_row(res2)))
			fprintf(fp,"<tr><td align=center>&nbsp;</td><td align=center>&nbsp;</td><td align=center>%s</td></tr>\n",field2[0]);
	}


}//void funcCustomerReport(FILE *fp)


