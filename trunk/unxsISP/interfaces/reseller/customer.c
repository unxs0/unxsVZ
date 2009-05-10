/*
FILE 
	customer.c
	$Id: customer.c 294 2008-05-28 23:43:51Z Hugo $
AUTHOR
	(C) 2006 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	mysqlISP2 Admin (Owner) Interface
	program file.
*/


#include "interface.h"
#define ORG_CUSTOMER    "Organization Customer"
#define ORG_WEBMASTER   "Organization Webmaster"
#define ORG_SALES       "Organization Sales Force"
#define ORG_SERVICE     "Organization Customer Service"
#define ORG_ACCT        "Organization Bookkeeper"
#define ORG_ADMIN       "Organization Admin"

unsigned uCustomer=0;

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

static char cPasswd[65]={""};
static char *cPasswdStyle="type_fields_off";

static char cCompanyName[33]={""};
static char *cCompanyNameStyle="type_fields_off";

static char cCompanyRegistration[33]={""};
static char *cCompanyRegistrationStyle="type_fields_off";

static char cVAT[17]={""};
static char *cVATStyle="type_fields_off";

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

static char cResellerCode[33]={""};
static char *cResellerCodeStyle="type_fields_off";

static char cNavList[8192]={""};
static char cSearch[100]={""};

static unsigned uPayment=0;
static char *cuPaymentStyle="type_fields_off";

static unsigned uPerm=0;
static char cuPerm[100]={""};
static char *cPermPullDownStyle="type_fields_off";

static unsigned uIsCompany=0;

static unsigned uProduct=0;
static unsigned uStep=0;

static unsigned uServices[100]={0};
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
unsigned uCompanyExists(char *cCompany);
unsigned uCreateCompany(char *cCompany);

void SetCustomerFieldsOn(void);
void LoadCustomer(unsigned cuClient);
void SearchCustomer(char *cSearchTerm);
unsigned uCustomerExists(char *cFirstName,char *cLastName);

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
		else if(!strcmp(entries[i].name,"cResellerCode"))
			sprintf(cResellerCode,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCompanyName"))
			sprintf(cCompanyName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCompanyRegistration"))
			sprintf(cCompanyRegistration,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cVAT"))
			sprintf(cVAT,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"cIDNumber"))
			sprintf(cIDNumber,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uIsCompany"))
			sscanf(entries[i].val,"%u",&uIsCompany);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);	
		else if(!strcmp(entries[i].name,"cuPerm"))
		{
			sprintf(cuPerm,"%.99s",entries[i].val);
			if(!strcmp(cuPerm,ORG_ADMIN))
				uPerm=6;
			else if(!strcmp(cuPerm,ORG_ACCT))
				uPerm=5;
			else if(!strcmp(cuPerm,ORG_SERVICE))
				uPerm=4;
			else if(!strcmp(cuPerm,ORG_SALES))
				uPerm=3;
			else if(!strcmp(cuPerm,ORG_WEBMASTER))
				uPerm=2;
			else if(!strcmp(cuPerm,ORG_CUSTOMER))
				uPerm=1;
		}
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
			uServicesCount++;
		}
	}

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
	
	MYSQL_RES *res;
	MYSQL_ROW field;
/*
cFirstName, 0
cLastName,1
cEmail,2
cAddr1,3
cAddr2,4
cCity,5
cState,6
cZip,7
cCountry,8
uPayment,9
cCardType,10
cCardNumber,11
uExpMonth,12
uExpYear,13
cCardName,14
cACHDebits,15
cShipName,16
cShipAddr1,17
cShipAddr2,18
cShipCity,19
cShipState,20
cShipZip,21
cShipCountry,22
cTelephone,23
cFax,24
cPasswd25
cResellerCode 26
cCompanyName 27
cCompanyRegistration 28
cVAT 29
cMobile 30
cBankName 31
cBranchName 32
cBranchCode 33
cAccountHolder 34
cAccountNumber 35
uAccountType 36
cIDNumber 37
*/
	sprintf(gcQuery,"SELECT cFirstName,cLastName,cEmail,cAddr1,cAddr2,cCity,cState,cZip,cCountry,uPayment,cCardType,cCardNumber,uExpMonth,uExpYear,cCardName,cACHDebits,cShipName,cShipAddr1,cShipAddr2,cShipCity,cShipState,cShipZip,cShipCountry,cTelephone,cFax,cPasswd,cResellerCode,cCompanyName,cCompanyRegistration,cVAT,cMobile,cBankName,cBranchName,cBranchCode,cAccountHolder,cAccountNumber,uAccountType,cIDNumber,uOwner,cLabel,cAddr3,cShipAddr3 FROM tClient WHERE uClient='%u'",uClient);
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
		sprintf(cPasswd,"%.20s",field[25]);
		sprintf(cResellerCode,"%.32s",field[26]);
		sprintf(cCompanyName,"%.32s",field[27]);
		sprintf(cCompanyRegistration,"%.32s",field[28]);
		sprintf(cVAT,"%.16s",field[29]);
		sprintf(cMobile,"%.32s",field[30]);
		sprintf(cBankName,"%.32s",field[31]);
		sprintf(cBranchName,"%.32s",field[32]);
		sprintf(cBranchCode,"%.6s",field[33]);
		sprintf(cAccountHolder,"%.100s",field[34]);
		sprintf(cAccountNumber,"%.32s",field[35]);
		sscanf(field[36],"%u",&uAccountType);
		sprintf(cIDNumber,"%.32s",field[37]);
		
		if(!strcmp(field[38],"1"))
			uIsCompany=1;
		sprintf(cLabel,"%.32s",field[39]);
		sprintf(cAddr3,"%.100s",field[40]);
		sprintf(cShipAddr3,"%.100s",field[41]);

		gcMessage="1 record(s) found.";
		if(!cNavList[0])
			sprintf(cNavList,
"<a class=inputLink href=\"#\" onClick=\"javascript:window.open('?gcPage=Glossary&cLabel=Search Customer','Glossary','height=600,width=500,status=yes,toolbar=no,menubar=no,location=no,scrollbars=1')\"><strong>Search Results</strong></a><br><br><a href=ispReseller.cgi?gcPage=Customer&uCustomer=%u>%s</a><br>\n",
				uCustomer
				,field[39]
				);
		
	}
	else
		gcMessage="No records found.";

	mysql_free_result(res);

}//void LoadCustomer(char *cuClient)


void SearchCustomer(char *cSearchTerm)
{
	//Don't show Root
	
	sprintf(gcQuery,"SELECT tClient.uClient,tClient.cLabel FROM tClient WHERE (cLabel LIKE '%s%%' OR cLastName LIKE '%s%%') AND uReseller='%u'",
		cSearchTerm
		,cSearchTerm
		,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void SearchCustomer(char *cSearchTerm)


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

		if(uCustomer)
		{
			gcMessage="1 record(s) found.";
			sprintf(cNavList,"<a class=inputLink href=\"#\" onClick=\"javascript:window.open('?gcPage=Glossary&cLabel=Search Customer','Glossary','height=600,width=500,status=yes,toolbar=no,menubar=no,location=no,scrollbars=1')\"><strong>Search Results</strong></a><br><br><a href=ispReseller.cgi?gcPage=Customer&uCustomer=%u>%s</a><br>\n",uCustomer,ForeignKey("tClient","cLabel",uCustomer));
		}
		
		if(!strcmp(gcFunction,"New"))
		{			
			sprintf(gcNewStep,"Confirm ");
			gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
			gcInputStatus[0]=0;
			SetCustomerFieldsOn();
			htmlCustomer();
		}
		else if(!strcmp(gcFunction,"Confirm New"))
		{
			if(ValidateCustomerInput()==1)
			{
				if(!uCustomerExists(cFirstName,cLastName))
					NewCustomer();
				else
				{
					gcMessage="<blink>Customer already exists!</blink>";
					SetCustomerFieldsOn();
					cFirstNameStyle="type_fields_req";
					cLastNameStyle="type_fields_req";
					gcInputStatus[0]=0;
					sprintf(gcNewStep,"Confirm ");
				}
			}
			else
			{
				sprintf(gcNewStep,"Confirm ");
				gcInputStatus[0]=0;
				htmlCustomer();
			}

			htmlCustomer();
		}
		else if(!strcmp(gcFunction,"Modify"))
		{
			sprintf(gcModStep,"Confirm ");
			gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
			SetCustomerFieldsOn();
			gcInputStatus[0]=0;
			htmlCustomer();
		}
		else if(!strcmp(gcFunction,"Confirm Modify"))
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
		else if(!strcmp(gcFunction,"Delete"))
		{
			/*
			if(uHasActiveProducts(uCustomer))
			{
				gcMessage="<blink>Can't delete a customer with active products</blink>";
				htmlCustomer();
			}
			*/
			sprintf(gcDelStep,"Confirm ");
			gcMessage="Double check you have selected the correct record to delete. Then confirm. Any other action to cancel.";
			htmlCustomer();
		}
		else if(!strcmp(gcFunction,"Confirm Delete"))
			DelCustomer();
		else if(!strcmp(gcFunction,"Search"))
		{
			MYSQL_RES *res;
			MYSQL_ROW field;
			char cTmp[512]={""};
			unsigned uRows=0;
			unsigned uDisplayCount=0;
		
			if(!strcmp(cSearch,"Search Customers") || !cSearch[0])
				htmlCustomer();
	
			SearchCustomer(cSearch);
			res=mysql_store_result(&gMysql);

			uRows=mysql_num_rows(res);
			
			if(uRows)
				sprintf(cNavList,
					"<a class=inputLink href=\"#\" onClick=\"javascript:window.open('?gcPage=Glossary&cLabel=Search Customer','Glossary','height=600,width=500,status=yes,toolbar=no,menubar=no,location=no,scrollbars=1')\"><strong>Search Results</strong></a><br><br>");
			
			if(uRows==1)
			{
				field=mysql_fetch_row(res);
				sscanf(field[0],"%u",&uCustomer);
				LoadCustomer(uCustomer);
				sprintf(cTmp,"<a href=ispReseller.cgi?gcPage=Customer&uCustomer=%s>%s</a><br>\n",field[0],field[1]);
				strcat(cNavList,cTmp);
				gcMessage="1 record(s) found.";
				htmlCustomer();
			}
			else if(uRows>1)
			{
				sprintf(cNavList,"<!-- Search NavList Start -->\n");
	
				while((field=mysql_fetch_row(res)))
				{
					if(strlen(cNavList)>8000 || (uDisplayCount==20)) break;
					sprintf(cTmp,"<a href=ispReseller.cgi?gcPage=Customer&uCustomer=%s>%s</a><br>\n",field[0],field[1]);
					strcat(cNavList,cTmp);
					uDisplayCount++;
				}
				if(uDisplayCount<uRows)
				{
					sprintf(cTmp,"<br>Only the first %u shown (%u results). If the customer you are looking for is not in the list above please further refine your search.<br>",uDisplayCount,uRows);
					strcat(cNavList,cTmp);
				}
				strcat(cNavList,"<!-- Search NavList End -->\n");
				sprintf(cTmp,"%u record(s) found.",uRows);
				gcMessage=cTmp;
				htmlCustomer();
			}
			else
			{
				gcMessage="No records found";
				htmlCustomer();
			}
			
				
		}
		else if(!strcmp(gcFunction,"Add Product Wizard"))
		{
			htmlProductDeployWizard(1);
		}
		else if(!strcmp(gcFunction,"Next"))
		{
			if((uStep+1)==2 && !uProductHasServices(uProduct))
			{
				gcMessage="<blink>The selected product has no services configured. Can't continue.</blink>";
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
			LoadCustomer(uCustomer);
		}
		htmlCustomer();
	}

}//void CustomerCommands(pentry entries[], int x)


void htmlProductDeployWizard(unsigned uStep)
{
	htmlHeader("mysqlISP2 Reseller Interface","Header");
	sprintf(gcQuery,"Reseller.ProductDeploy.%u",uStep);
	htmlCustomerPage("",gcQuery);
	
	htmlFooter("Footer");
}
void htmlCustomer(void)
{
	htmlHeader("mysqlISP2 Reseller Interface","Header");
	htmlCustomerPage("","Reseller.Customer.Body");
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
			char cuExpMonth[10]={""};
			char cuExpYear[10]={""};
			char cuCustomer[16]={""};
			char cuProduct[16]={""};
			char cuPayment[16]={""};
			char cuAccountType[16]={""};
			char cuIsCompany[2]={""};
		
			sprintf(cuCustomer,"%u",uCustomer);
			sprintf(cuExpMonth,"%u",uExpMonth);
			sprintf(cuExpYear,"%u",uExpYear);
			sprintf(cuProduct,"%u",uProduct);
			sprintf(cuPayment,"%u",uPayment);
			sprintf(cuAccountType,"%u",uAccountType);
			sprintf(cuIsCompany,"%u",uIsCompany);
			
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="ispReseller.cgi";
			
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

			template.cpName[12]="";

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

			template.cpName[63]="cNavList";
			template.cpValue[63]=cNavList;

			template.cpName[64]="cBtnStatus";
			if(uCustomer)
				template.cpValue[64]="";
			else
				template.cpValue[64]="disabled";

			template.cpName[65]="uProduct";
			template.cpValue[65]=cuProduct;

			template.cpName[66]="cProductLabel";
			template.cpValue[66]=cGetProductLabel(uProduct);
			
			template.cpName[67]="uPayment";
			template.cpValue[67]=cuPayment;
			
			template.cpName[68]="cCompanyName";
			template.cpValue[68]=cCompanyName;

			template.cpName[69]="cCompanyNameStyle";
			template.cpValue[69]=cCompanyNameStyle;

			template.cpName[70]="cVAT";
			template.cpValue[70]=cVAT;
			
			template.cpName[71]="cVATStyle";
			template.cpValue[71]=cVATStyle;

			template.cpName[72]="cIDNumber";
			template.cpValue[72]=cIDNumber;

			template.cpName[73]="cIDNumberStyle";
			template.cpValue[73]=cIDNumberStyle;

			template.cpName[74]="cBankName";
			template.cpValue[74]=cBankName;

			template.cpName[75]="cBankNameStyle";
			template.cpValue[75]=cBankNameStyle;

			template.cpName[76]="cBranchName";
			template.cpValue[76]=cBranchName;

			template.cpName[77]="cBranchCode";
			template.cpValue[77]=cBranchCode;

			template.cpName[78]="cBranchCodeStyle";
			template.cpValue[78]=cBranchCodeStyle;

			template.cpName[79]="cAccountHolder";
			template.cpValue[79]=cAccountHolder;

			template.cpName[80]="cAccountHolderStyle";
			template.cpValue[80]=cAccountHolderStyle;

			template.cpName[81]="cAccountNumber";
			template.cpValue[81]=cAccountNumber;

			template.cpName[82]="cAccountNumberStyle";
			template.cpValue[82]=cAccountNumberStyle;

			template.cpName[83]="cResellerCode";
			template.cpValue[83]=cResellerCode;

			template.cpName[84]="cResellerCodeStyle";
			template.cpValue[84]=cResellerCodeStyle;

			template.cpName[85]="cCompanyRegistrationStyle";
			template.cpValue[85]=cCompanyRegistrationStyle;

			template.cpName[86]="cBranchNameStyle";
			template.cpValue[86]=cBranchNameStyle;

			template.cpName[87]="cMobile";
			template.cpValue[87]=cMobile;

			template.cpName[88]="cMobileStyle";
			template.cpValue[88]=cMobileStyle;

			template.cpName[89]="cCompanyRegistration";
			template.cpValue[89]=cCompanyRegistration;

			template.cpName[90]="uAccountType";
			template.cpValue[90]=cuAccountType;

			template.cpName[91]="uIsCompany";
			template.cpValue[91]=cuIsCompany;

			template.cpName[92]="cCompanyStatus";
			if(!uIsCompany && gcModStep[0])
				template.cpValue[92]="disabled";
			else
				template.cpValue[92]="";

			template.cpName[93]="cLabel";
			template.cpValue[93]=cLabel;

			template.cpName[94]="cAddr3Style";
			template.cpValue[94]=cAddr3Style;

			template.cpName[95]="cShipAddr3";
			template.cpValue[95]=cShipAddr3;

			template.cpName[96]="cShipAddr3Style";
			template.cpValue[96]=cShipAddr3Style;

			template.cpName[97]="";
			
			
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


unsigned uCompanyExists(char *cCompany)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCompany=0;

	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cLabel='%s' AND uOwner=1",cCompany);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uCompany);
	
	mysql_free_result(res);

	return(uCompany);
	
}//unsigned uCompanyExists(char *cCompany)


unsigned uCreateCompany(char *cCompany)
{
	sprintf(gcQuery,"INSERT INTO tClient SET cLabel='%s',cCompanyName='%s',cCompanyRegistration='%s',cVAT='%s',uOwner=1,uCreatedBy='%u',uCreatedDate=UNIX_TIMESTAMP(NOW())",
		TextAreaSave(cCompany)
		,TextAreaSave(cCompany)
		,TextAreaSave(cCompanyRegistration)
		,TextAreaSave(cVAT)
		,guLoginClient);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	return((unsigned)mysql_insert_id(&gMysql));

}//unsigned uCreateCompany(char *cCompany)


unsigned uGetCompany(unsigned uCustomer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCompany=0;

	sprintf(gcQuery,"SELECT uOwner FROM tClient WHERE uClient='%u'",uCustomer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uCompany);
	
	if(uCompany==1) uCompany=uCustomer;
	
	return(uCompany);

}//unsigned uGetCompany(unsigned uCustomer)


void NewCustomer(void)
{
	unsigned uCompany=0;

	uCompany=uCompanyExists(TextAreaSave(cCompanyName));

	if(!uCompany)
		uCompany=uCreateCompany(TextAreaSave(cCompanyName));
	
	sprintf(gcQuery,"INSERT INTO tClient SET cLabel='%s %s',cFirstName='%s',cLastName='%s',cEmail='%s',cAddr1='%s',cAddr2='%s',cAddr3='%s',cCity='%s',cState='%s',cZip='%s',cCountry='%s',uPayment='%u',cCardType='%s',cCardNumber='%s',uExpMonth='%u',uExpYear='%u',cCardName='%s',cACHDebits='%s',cShipName='%s',cShipAddr1='%s',cShipAddr2='%s',cShipAddr3='%s',cShipCity='%s',cShipState='%s',cShipZip='%s',cShipCountry='%s',cTelephone='%s',cFax='%s',cPasswd='%s',cCompanyName='%s',cCompanyRegistration='%s',cVAT='%s',cBankName='%s',cBranchName='%s',cBranchCode='%s',cAccountHolder='%s',cAccountNumber='%s',uAccountType='%u',cMobile='%s',cIDNumber='%s',uOwner='%u',uCreatedBy='%u',uCreatedDate=UNIX_TIMESTAMP(NOW()),uReseller='%u'",
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
			,TextAreaSave(cPasswd)
			,TextAreaSave(cCompanyName)
			,TextAreaSave(cCompanyRegistration)
			,TextAreaSave(cVAT)
			,TextAreaSave(cBankName)
			,TextAreaSave(cBranchName)
			,TextAreaSave(cBranchCode)
			,TextAreaSave(cAccountHolder)
			,TextAreaSave(cAccountNumber)
			,uAccountType
			,TextAreaSave(cMobile)
			,TextAreaSave(cIDNumber)
			,uCompany
			,guLoginClient
			,guLoginClient
			);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCustomer=mysql_insert_id(&gMysql);

	if(uCustomer)
	{
		gcMessage="New Customer created OK";
		mysqlISP2Log(uCustomer,"tClient","New");
	}
	else
	{
		gcMessage="New Customer NOT created";
		mysqlISP2Log(uCustomer,"tClient","New Fail");
	}

	//If entered a password authorize the contact with the select permission level.
	if(cPasswd[0])
	{
		if(strncmp(cPasswd,"..",2))
			EncryptPasswdWithSalt(cPasswd,"..");

		sprintf(gcQuery,"INSERT INTO tAuthorize SET cLabel='%s %s',cPasswd='%s',uCertClient='%u',uPerm='%u',uOwner='%u',uCreatedBy='%u',uCreatedDate=UNIX_TIMESTAMP(NOW())",
			TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,cPasswd
			,uCustomer
			,uPerm
			,uCompany
			,guLoginClient
			);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		mysqlISP2Log(mysql_insert_id(&gMysql),"tAuthorize","New");
	}
	
}//void NewCustomer(void)


void ModCustomer(void)
{

	//Company affilitation can't be modified.
	//Neither company data, to do so, must load company
	
	sprintf(gcQuery,"UPDATE tClient SET cLabel='%s %s',cFirstName='%s',cLastName='%s',cEmail='%s',cAddr1='%s',cAddr2='%s',cAddr3='%s',cCity='%s',cState='%s',cZip='%s',cCountry='%s',uPayment='%u',cCardType='%s',cCardNumber='%s',uExpMonth='%u',uExpYear='%u',cCardName='%s',cACHDebits='%s',cShipName='%s',cShipAddr1='%s',cShipAddr2='%s'cShipAddr3='%s',cShipCity='%s',cShipState='%s',cShipZip='%s',cShipCountry='%s',cTelephone='%s',cFax='%s',cPasswd='%s',cBankName='%s',cBranchName='%s',cBranchCode='%s',cAccountHolder='%s',cAccountNumber='%s',uAccountType='%u',cMobile='%s',cIDNumber='%s',uModBy='%u',uModDate=UNIX_TIMESTAMP(NOW()) WHERE uClient='%u' AND uReseller='%u'",
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
			,guLoginClient
			,uCustomer
			,guLoginClient
			);
	mysql_query(&gMysql,gcQuery);

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="Customer modified OK";
		mysqlISP2Log(uCustomer,"tClient","Mod");
	}
	else
	{
		gcMessage="Customer NOT modified";
		mysqlISP2Log(uCustomer,"tClient","Mod Fail");
	}

	//If entered a password authorize the contact with the select permission level.
	if(cPasswd[0])
	{
		unsigned uAuthorize=0;
		MYSQL_RES *res;
		MYSQL_ROW field;

		sprintf(gcQuery,"SELECT uAuthorize FROM tAuthorize,tClient WHERE uCertClient='%u' \
				AND tClient.uClient=tAuthorize.uCertClient AND tClient.uReseller='%u'",
				uCustomer
				,guLoginClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		res=mysql_store_result(&gMysql);

		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uAuthorize);
		
		if(strncmp(cPasswd,"..",2))
			EncryptPasswdWithSalt(cPasswd,"..");

		if(uAuthorize)
			sprintf(gcQuery,"UPDATE tAuthorize SET cLabel='%s %s',cPasswd='%s',uPerm='%u',uModBy='%u',uCreatedDate=UNIX_TIMESTAMP(NOW()) WHERE uCertClient='%u'",
				TextAreaSave(cFirstName)
				,TextAreaSave(cLastName)
				,cPasswd
				,uPerm
				,guLoginClient
				,uCustomer
				);
		else	
			sprintf(gcQuery,"INSERT INTO tAuthorize SET cLabel='%s %s',cPasswd='%s',uCertClient='%u',uPerm='%u',uOwner='%u',uCreatedBy='%u',uCreatedDate=UNIX_TIMESTAMP(NOW())",
				TextAreaSave(cFirstName)
				,TextAreaSave(cLastName)
				,cPasswd
				,uCustomer
				,uPerm
				,uCompanyExists(TextAreaSave(cCompanyName))
				,guLoginClient
				);
				
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		if(uAuthorize)
			mysqlISP2Log(uAuthorize,"Mod","tAuthorize");			
		else
			mysqlISP2Log(mysql_insert_id(&gMysql),"New","tAuthorize");

	}

}//void ModCustomer(void)


void DelCustomer(void)
{
	//Add delete tInstance. Note that a customer can't be deleted if it has not cancelled 
	//products deployed!
	//
	
	sprintf(gcQuery,"DELETE FROM tClient WHERE uClient='%u' AND uReseller='%u'",uCustomer,guLoginClient);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="Customer deleted OK";
		mysqlISP2Log(uCustomer,"tClient","Del");
	}
	else
	{
		gcMessage="Customer NOT deleted";
		mysqlISP2Log(uCustomer,"tClient","Del Fail");
	}

}//void DelCustomer(void)


unsigned ValidateCustomerInput(void)
{
	if(!uIsCompany)
	{
	if(!cFirstName[0])
	{
		SetCustomerFieldsOn();
		cFirstNameStyle="type_fields_req";
		gcMessage="<blink>Must enter first name</blink>";
		return(0);
	}
	if(!cLastName[0])
	{
		SetCustomerFieldsOn();
		cLastNameStyle="type_fields_req";
		gcMessage="<blink>Must enter lastname</blink>";
		return(0);
	}
	}//if(!uIsCompany)

	if(!cEmail[0])
	{
		SetCustomerFieldsOn();
		cEmailStyle="type_fields_req";
		gcMessage="<blink>Must enter email address</blink>";
		return(0);
	}
	else
	{
		//Validate email
	}
	if(!cAddr1[0])
	{
		SetCustomerFieldsOn();
		cAddr1Style="type_fields_req";
		gcMessage="<blink>Must enter address information</blink>";
		return(0);
	}
	if(!cCity[0])
	{
		SetCustomerFieldsOn();
		cCityStyle="type_fields_req";
		gcMessage="<blink>Must enter city information</blink>";
		return(0);
	}
	if(!cState[0])
	{
		SetCustomerFieldsOn();
		cStateStyle="type_fields_req";
		gcMessage="<blink>Must enter state information</blink>";
		return(0);
	}
	if(!cZip[0])
	{
		SetCustomerFieldsOn();
		cZipStyle="type_fields_req";
		gcMessage="<blink>Must enter zip code</blink>";
		return(0);
	}
	if(!cCountry[0])
	{
		SetCustomerFieldsOn();
		cCountryStyle="type_fields_req";
		gcMessage="<blink>Must enter country information</blink>";
		return(0);
	}
	if(!cTelephone[0])
	{
		SetCustomerFieldsOn();
		cTelephoneStyle="type_fields_req";
		gcMessage="<blink>Must enter telephone information</blink>";
		return(0);
	}
	if(cCompanyName[0])
	{
		if(!cCompanyRegistration[0])
		{
			SetCustomerFieldsOn();
			cCompanyRegistrationStyle="type_fields_req";
			gcMessage="<blink>Must enter company registration</blink>";
			return(0);
		}
		if(!cVAT[0])
		{
			SetCustomerFieldsOn();
			cVATStyle="type_fields_req";
			gcMessage="<blink>Must enter VAT number</blink>";
			return(0);
		}
	}
	if(strcmp(cCardType,"---"))
	{
		if(!cCardNumber[0])
		{
			SetCustomerFieldsOn();
			cCardNumberStyle="type_fields_req";
			gcMessage="<blink>Must enter credit card number</blink>";
			return(0);
		}
		if(!uExpMonth)
		{
			SetCustomerFieldsOn();
			cuExpMonthStyle="type_fields_req";
			gcMessage="<blink>Must select expiration month</blink>";
			return(0);
		}
		if(!uExpYear)
		{
			SetCustomerFieldsOn();
			cuExpYearStyle="type_fields_req";
			gcMessage="<blink>Must select expiration year</blink>";
			return(0);
		}
	}
	if(cShipName[0])
	{
		if(!cShipAddr1[0])
		{
			SetCustomerFieldsOn();
			cShipAddr1Style="type_fields_req";
			gcMessage="<blink>Must enter shipping address information</blink>";
			return(0);
		}
		if(!cShipCity[0])
		{
			SetCustomerFieldsOn();
			cShipCityStyle="type_fields_req";
			gcMessage="<blink>Must enter shipping city information</blink>";
			return(0);
		}
		if(!cShipState[0])
		{
			SetCustomerFieldsOn();
			cShipStateStyle="type_fields_req";
			gcMessage="<blink>Must enter shipping state information</blink>";
			return(0);
		}
		if(!cShipZip[0])
		{
			SetCustomerFieldsOn();
			cShipZipStyle="type_fields_req";
			gcMessage="<blink>Must enter shipping zip code/blink>";
			return(0);
		}
		if(!cShipCountry[0])
		{
			SetCustomerFieldsOn();
			cShipCountryStyle="type_fields_req";
			gcMessage="<blink>Must enter shipping countryinformation</blink>";
			return(0);
		}
		
	}
	if(cBankName[0])
	{
		if(!cBranchName[0])
		{
			SetCustomerFieldsOn();
			cBranchNameStyle="type_fields_req";
			gcMessage="<blink>Must enter branch name</blink>";
			return(0);
		}
		if(!cBranchCode[0])
		{
			SetCustomerFieldsOn();
			cBranchCodeStyle="type_fields_req";
			gcMessage="<blink>Must enter branch code</blink>";
			return(0);
		}
	        if(!cAccountHolder[0])
		{
			SetCustomerFieldsOn();
			cAccountHolderStyle="type_fields_req";
			gcMessage="<blink>Must enter account holder name</blink>";
			return(0);
		}	
		if(!cAccountNumber[0])
		{
			SetCustomerFieldsOn();
			cAccountNumberStyle="type_fields_req";
			gcMessage="<blink>Must enter account number</blink>";
			return(0);
		}
		if(!uAccountType)
		{
			SetCustomerFieldsOn();
			cuAccountTypeStyle="type_fields_req";
			gcMessage="<blink>Must select account type</blink>";
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
	if(uIsCompany)
	{
		cCompanyNameStyle="type_fields";
		cCompanyRegistrationStyle="type_fields";
		cVATStyle="type_fields";
	}
	cIDNumberStyle="type_fields";
	cBankNameStyle="type_fields";
	cBranchNameStyle="type_fields";
	cBranchCodeStyle="type_fields";
	cAccountHolderStyle="type_fields";
	cAccountNumberStyle="type_fields";
	cuAccountTypeStyle="type_fields";
	cMobileStyle="type_fields";
	cPermPullDownStyle="type_fields";

}//void SetCustomerFieldsOn(void)


void funcCustomerProducts(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	if(!uCustomer) return;

	fprintf(fp,"<!-- funcCustomerProducts(fp) start -->\n");

	sprintf(gcQuery,"SELECT tInstance.uInstance,tInstance.cLabel,FROM_UNIXTIME(GREATEST(tInstance.uCreatedDate,tInstance.uModDate)),tStatus.cLabel FROM tInstance,tStatus WHERE tInstance.uClient='%u' AND tInstance.uStatus=tStatus.uStatus ORDER BY GREATEST(tInstance.uCreatedDate,tInstance.uModDate)",
		uGetCompany(uCustomer));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		FromMySQLDate(field[2]);
		fprintf(fp,"<tr><td><a href=ispReseller.cgi?gcPage=Instance&uInstance=%s&uCustomer=%u>%s</a></td><td>%s</td><td>%s</td></tr>\n",
				field[0]
				,uCustomer
				,field[1]
				,field[3]
				,field[2]
		       );
	}

	fprintf(fp,"<!-- funcCustomerProducts(fp) end -->\n");

}//void funcCustomerProducts(FILE *fp)


void funcSelectPermLevel(FILE *fp)
{
	fprintf(fp,"<!-- funcPermLevelDropDown(fp) end -->\n");
	
	fprintf(fp,"<select title='Select the permission level for the contact' name=cuPerm class=%s %s>\n",cPermPullDownStyle,gcInputStatus);

	fprintf(fp,"<option ");
	if(!strcmp(cuPerm,ORG_CUSTOMER))
		fprintf(fp,"selected");
	fprintf(fp,">%s</option>\n",ORG_CUSTOMER);
		
	fprintf(fp,"<option ");
	if(!strcmp(cuPerm,ORG_WEBMASTER))
		fprintf(fp,"selected");
	fprintf(fp,">%s</option>\n",ORG_WEBMASTER);

	fprintf(fp,"<option ");
	if(!strcmp(cuPerm,ORG_SALES))
		fprintf(fp,"selected");
	fprintf(fp,">%s</option>\n",ORG_SALES);
		
	fprintf(fp,"<option ");
	if(!strcmp(cuPerm,ORG_SERVICE))
		fprintf(fp,"selected");
	fprintf(fp,">%s</option>\n",ORG_SERVICE);

	fprintf(fp,"<option ");
	if(!strcmp(cuPerm,ORG_ACCT))
		fprintf(fp,"selected");
	fprintf(fp,">%s</option>\n",ORG_ACCT);

	fprintf(fp,"<option ");
	if(!strcmp(cuPerm,ORG_ADMIN))
		fprintf(fp,"selected");
	fprintf(fp,">%s</option>\n",ORG_ADMIN);
	
	fprintf(fp,"</select>\n");

}//void funcSelectPermLevel(FILE *fp)


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


unsigned CustomerHasProducts(void)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uInstance FROM tInstance WHERE uClient='%u' AND (uStatus='%u' OR uStatus='%u' OR uStatus='%u' OR uStatus='%u' OR uStatus='%u'  OR uStatus='%u'",
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


//Product deployment wizard functions start.
//
void funcSelectProduct(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcSelectProduct(fp) start -->\n");

	sprintf(gcQuery,"SELECT uProduct,cLabel FROM tProduct ORDER BY cLabel");
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
	char cParamName[100]={""};
	char cuParamCount[10]={""};

//from tproductfunc.h AddService:
//sprintf(gcQuery,"INSERT INTO tServiceGlue SET uServiceGroup=%u,uService=%u",uProduct,uService);
	
	fprintf(fp,"<!--funcProductParameterInput(fp) start -->\n");

	sprintf(gcQuery,"SELECT tService.uService,tService.cLabel uService FROM tService,tServiceGlue WHERE tServiceGlue.uServiceGroup=%u AND tService.uService=tServiceGlue.uService AND tService.uAvailable=1 ORDER BY tService.cLabel",uProduct);
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
sprintf(gcQuery,"SELECT tParameter.cParameter,tParameter.cISMName,cISMHelp,tParamType.cLabel,tParameter.cExtQuery FROM tParameter,tConfig,tParamType WHERE tConfig.uGroup=%s AND tConfig.uParameter=tParameter.uParameter AND tParameter.uISMHide=0 AND tParamType.uParamType=tParameter.uParamType ORDER BY tParameter.uISMOrder",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res2=mysql_store_result(&gMysql);

		while((field2=mysql_fetch_row(res2)))
		{
			sprintf(cParamName,"cParam.%u",uParamCount);
			
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
				template.cpValue[4]="";

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
					//sprintf(gcQuery,"cExtVerify:%s:\n cConfQuery:%s:\n cIP:%s\n cDb:%s\n cPwd:%s\n cLogin:%s\n Verify query:(%s)",cExtVerify,cConfQuery,cIp,cDb,cPwd,cLogin,cp+1);
					//htmlPlainTextError(gcQuery);	
					if(cDb[0]==0 || cPwd[0]==0 || cLogin[0]==0)
					{
						fprintf(fp,"Can't display parameter input (%s). Configuration error",field2[1]);
						continue;
					}
		
					ExtConnectDb(cDb,cIp,cPwd,cLogin);

					sprintf(gcQuery,"%s",cp+1);
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
		fpTemplate(fp,"ParameterInputFooter",&template);
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
		
		for(i=0;i<uParameterCount;i++)
		{
			if(uIsParameterInService(uServices[n],cParameterName[i]))
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
			}

		}//Parameters loop
		template.cpName[0]="";
		fpTemplate(fp,"EnteredParametersFooter",&template);
	}//Services loop
	
	fprintf(fp,"<!-- funcEnteredParameters(fp) end -->\n");
	
}//void funcEnteredParameters(FILE *fp)


unsigned uIsParameterInService(unsigned uService,char *cParamName)
{
	MYSQL_RES *res;
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
	//Deploy the product for the company which the contact belongs to, always.
	unsigned uGroup=0;
	register int i=0,n=0;
	time_t luClock;
	time_t uDeployTime;	
	unsigned uCompany=0;

	uCompany=uGetCompany(uCustomer);
	time(&luClock);
	
	sprintf(gcQuery,"INSERT INTO tInstance SET uClient='%u',uProduct='%u',uOwner='%u',uCreatedBy='%u',uCreatedDate=UNIX_TIMESTAMP(NOW()),uStatus='%u',cLabel='%s'",
			uCompany
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
		for(i=0;i<uParameterCount;i++)
		{
			if(uIsParameterInService(uServices[n],cParameterName[i]))
			{
				sprintf(gcQuery,"INSERT INTO tClientConfig SET uParameter='%u',cValue='%s',uGroup='%u',uService='%u',uOwner='%u',uCreatedBy='%u',uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uGetParameter(cParameterName[i])
						,cParameterInput[i]
						,uGroup
						,uServices[n]
						,uCompany
						,guLoginClient
			       		);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
			}
					
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
	unsigned uCompany=0;

	uCompany=uGetCompany(uCustomer);
	
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
					uCompany,
					ForeignKey("tClient","cLabel",uCompany));
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
			uCompany,
			uJobDate,
			mysqlISP_Waiting,
			guLoginClient,
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

        sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='%s'",cName);
        mysql_query(&gMysql,gcQuery);
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
				cValue[i]!='-' && cValue[i]!='_'))  ) uErr=1;
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

	for(i=0;i<uParameterCount;i++)
	{
//int CheckParameterValue(char *cValue, char *cParamType, char *cRange, unsigned uUnique,char *cExtVerify
		GetParamInfo(cParameterName[i],&ParamData);
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
			sprintf(cMessage,"%s='%s' is out of range %s, type %s"
					,cParameterLabel[i],cParameterInput[i],ParamData.cRange,ParamData.cParamType);
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

        sprintf(gcQuery,"SELECT tParamType.cLabel,tParameter.cRange,tParameter.cParameter,tParameter.uUnique,tParameter.cExtVerify,tParameter.uModPostDeploy,tParameter.uAllowMod FROM tParameter,tParamType WHERE cParameter='%s' AND tParameter.uParamType=tParamType.uParamType",cParameter);
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


