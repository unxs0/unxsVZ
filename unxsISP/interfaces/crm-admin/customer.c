/*
FILE 
	customer.c
	svn ID removed
AUTHOR
	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	unxsISP CRM (Owner) Interface
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


//
//Local only
unsigned ValidateCustomerInput(void);
void SearchCustomer(char *cLabel);
void NewCustomer(void);
void DelCustomer(void);
void ModCustomer(void);
unsigned CustomerHasProducts(void);

void SetCustomerFieldsOn(void);
void LoadCustomer(unsigned cuClient);
void SearchCustomer(char *cSearchTerm);
unsigned uCustomerExists(char *cFirstName,char *cLastName);


void ProcessCustomerVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{

		if(!strcmp(entries[i].name,"uCustomer"))
			sscanf(entries[i].val,"%u",&uCustomer);
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
	}

}//void ProcessUserVars(pentry entries[], int x)


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
		sprintf(cPasswd,"%.20s",field[25]);
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
		htmlCustomer();
	}

}//void CustomerCommands(pentry entries[], int x)


void htmlProductDeployWizard(unsigned uStep)
{
	htmlHeader("unxsISP CRM Interface","Header");
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

	htmlHeader("unxsISP CRM Interface","Header");
	htmlCustomerPage("","Customer.Body");
	htmlFooter("Footer");

}//void htmlCustomer(void)


void htmlCustomerPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect(cTemplateName,guTemplateSet);
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
			
			sprintf(cCustomerName,"for %.251s",ForeignKey("tClient","cLabel",uCustomer));
			sprintf(cuCustomer,"%u",uCustomer);
			sprintf(cuExpMonth,"%u",uExpMonth);
			sprintf(cuExpYear,"%u",uExpYear);
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
			template.cpValue[1]="ispCRM.cgi";
			
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
			template.cpValue[65]="";
			
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

}//void SetCustomerFieldsOn(void)


void funcCustomerTickets(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	time_t uScheduleDate=0;
	time_t luClock;
	char cDue[30]={""};

	if(!uCustomer) return;
	
	time(&luClock);

	fprintf(fp,"<!-- funcCustomerTickets(fp) start -->\n");

	sprintf(gcQuery,"SELECT uTicket,IF(uScheduleDate!=0,FROM_UNIXTIME(uScheduleDate),'Not scheduled yet'),cText,FROM_UNIXTIME(tTicket.uCreatedDate),"
			"tTicketStatus.cLabel,uScheduleDate,tTicket.uCreatedDate "
			"FROM tTicket,tTicketStatus WHERE tTicket.uOwner=%u AND uTicketOwner=%u "
			"AND tTicketStatus.uTicketStatus=tTicket.uTicketStatus "
			"ORDER BY tTicket.uCreatedDate DESC LIMIT 10",
			guOrg
			,uCustomer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[5],"%lu",&uScheduleDate);
		if(uScheduleDate)
		{
			if(uScheduleDate>luClock)
				sprintf(cDue,"Due in %lu hours",((uScheduleDate-luClock)/3600));
			else
				sprintf(cDue,"Overdue by %lu hours",((luClock-uScheduleDate)/3600));
		}
		else
			sprintf(cDue,"Not scheduled yet");

		fprintf(fp,"<tr><td><a href=ispCRM.cgi?gcPage=Ticket&uTicket=%s>Ticket #%s</a></td>"
				"<td>%s ...</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
				field[0]
				,field[0]
				,cShortenText(field[2],5)
				,field[3]
				,field[1]
				,field[4]
				,cDue
		       );
	}

	fprintf(fp,"<!-- funcCustomerTickets(fp) end -->\n");

}//void funcCustomerTickets(FILE *fp)


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

			fprintf(fp,"<a href=ispCRM.cgi?gcPage=Customer&uCustomer=%s>%s</a><br>\n",field[0],field[1]);
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

		fprintf(fp,"<a href=ispCRM.cgi?gcPage=Customer&uCustomer=%s><font color=%s>%s</font>"
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


char *cShortenText(char *cText,unsigned uWords)
{
	//Return the first n word from cText
	//will use the spaces for word counting.
	unsigned uCount=0;
	register int i=0;
	static char cResult[100];
	
	for(i=0;i<strlen(cText);i++)
	{
		cResult[i]=cText[i];
		if(cText[i]==' ')
			uCount++;
		if(uCount>=uWords) break;
	}

	cResult[i]='\0';
	return(cResult);
		
}//char *cShortenText(char *cText)


