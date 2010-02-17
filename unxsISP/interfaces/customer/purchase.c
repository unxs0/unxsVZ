/*
FILE
	purchase.c
PURPOSE
	Handles RADIUS account deployment,
	invoice setup and payment after first time loginm
AUTHOR
	(C) 2010 Hugo Urquiza for Unixservice, LLC.
*/
#include "interface.h"

static unsigned uInvoice=0;

static char *cuExpMonthStyle="type_fields";
static char *cuExpYearStyle="type_fields";

static unsigned uExpMonth=0;
static unsigned uExpYear=0;

static char cAddr1[101]={""};
static char *cAddr1Style="type_fields";

static char cAddr2[101]={""};
static char *cAddr2Style="type_fields";

static char cCity[101]={""};
static char *cCityStyle="type_fields";

static char cState[101]={""};
static char *cStateStyle="type_fields";

static char cZip[33]={""};
static char *cZipStyle="type_fields";

static char cCardNumber[101]={""};
static char *cCardNumberStyle="type_fields";

static char cCardName[101]={""};
static char *cCardNameStyle="type_fields";

void GeneratePurchaseInvoice(void);
void ShowConfirmPurchasePage(void);
void htmlPurchasePage(char *cTitle,char *cTemplateName);
unsigned ValidPurchaseInput(void);
unsigned SubmitRequest(unsigned uInvoice); //payment.c


void ProcessPurchaseVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cAddr1"))
			sprintf(cAddr1,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cAddr2"))
			sprintf(cAddr2,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCity"))
			sprintf(cCity,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cState"))
			sprintf(cState,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cZip"))
			sprintf(cZip,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCardNumber"))
			sprintf(cCardNumber,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uExpMonth"))
			sscanf(entries[i].val,"%u",&uExpMonth);
		else if(!strcmp(entries[i].name,"uExpYear"))
			sscanf(entries[i].val,"%u",&uExpYear);
		else if(!strcmp(entries[i].name,"cCardName"))
			sprintf(cCardName,"%.100s",entries[i].val);

	}
}//void ProcessPurchaseVars(pentry entries[], int x)


void PurchaseCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Purchase"))
	{
		ProcessPurchaseVars(entries,x);

		if(!strcmp(gcFunction,"Skip"))
		{
			sprintf(gcNewStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Skip"))
		{
			Logout();
		}
		else if(!strcmp(gcFunction,"Proceed"))
		{
			sprintf(gcModStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Proceed"))
		{
			ShowConfirmPurchasePage();
		}
		else if(!strcmp(gcFunction,"Complete Payment"))
		{
			//Validate input
			if(!ValidPurchaseInput())
				ShowConfirmPurchasePage();

			//Generate invoice
			GeneratePurchaseInvoice();
			//Proccess invoice
			if(SubmitRequest(uInvoice))
			{
				//If goes OK, create unxsISP data for radius account deployment
				//and submit job
			}
		}

	}
}//void PurchaseCommands(pentry entries[], int x)


void ShowPurchaseRadiusPage(void)
{
	htmlHeader("unxsISP Client Interface","Header");
	htmlPurchasePage("","FirstTime.Body");
	htmlFooter("Footer");

}//void ShowPurchaseRadiusPage(void)


void ShowConfirmPurchasePage(void)
{
	htmlHeader("unxsISP Client Interface","Header");
	htmlPurchasePage("","Purchase.Body");
	htmlFooter("Footer");

}//void ShowConfirmPurchasePage(void)


void htmlPurchasePage(char *cTitle,char *cTemplateName)
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
			
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="ispClient.cgi";
			
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

			template.cpName[11]="cISPName";
			template.cpValue[11]="AstraQom";

			template.cpName[12]="mProductPrice";
			template.cpValue[12]="15.95";
			
			template.cpName[13]="cAddr1";
			template.cpValue[13]=cAddr1;
			
			template.cpName[14]="cAddr1Style";
			template.cpValue[14]=cAddr1Style;
			
			template.cpName[15]="cAddr2";
			template.cpValue[15]=cAddr2;
			
			template.cpName[16]="cAddr2Style";
			template.cpValue[16]=cAddr2Style;

			template.cpName[17]="cCity";
			template.cpValue[17]=cCity;

			template.cpName[18]="cCityStyle";
			template.cpValue[18]=cCityStyle;

			template.cpName[19]="cState";
			template.cpValue[19]=cState;

			template.cpName[20]="cStateStyle";
			template.cpValue[20]=cStateStyle;

			template.cpName[21]="cZip";
			template.cpValue[21]=cZip;

			template.cpName[22]="cZipStyle";
			template.cpValue[22]=cZipStyle;

			template.cpName[23]="cCardNumberStyle";
			template.cpValue[23]=cCardNumberStyle;

			template.cpName[24]="cCardNameStyle";
			template.cpValue[24]=cCardNameStyle;
			
			template.cpName[25]="cCardNumber";
			template.cpValue[25]=cCardNumber;

			template.cpName[26]="cCardName";
			template.cpValue[26]=cCardName;

			template.cpName[27]="";
			
			printf("\n<!-- Start htmlPurchasePage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlPurchasePage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlPurchasePage(char *cTemplateName)


unsigned IsFirstTimeLogin(void)
{
	MYSQL_RES *res;
	unsigned uRet=0;

	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cCode='NeverLogin' AND uClient=%u",guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	uRet=mysql_num_rows(res);

	mysql_free_result(res);
	return(uRet);

}//unsigned IsFirstTimeLogin(void)


void funcPurchaseExpMonth(FILE *fp)
{
	sysfuncSelectExpMonth(fp,cuExpMonthStyle,uExpMonth,0);

}//void funcPurchaseExpMonth(FILE *fp)


void funcPurchaseExpYear(FILE *fp)
{
	sysfuncSelectExpYear(fp,cuExpYearStyle,uExpYear,0);

}//void funcPurchaseExpMonth(FILE *fp)


void GeneratePurchaseInvoice(void)
{
	//Pretty much hard-coded function to generate an invoice for the new customer
	sprintf(gcQuery,"INSERT INTO tInvoice (cFirstName,cLastName,cEmail,cAddr1,cAddr2,"
			"cCity,cState,cZip,cCountry,cCardType,cCardNumber,uExpMonth,uExpYear,"
			"cCardName,uOwner,uCreatedBy,uCreatedDate) "
			"VALUES (SELECT cFirstName,cLastName,cEmail,cAddr1,cAddr2,"
			"cCity,cState,cZip,cCountry,cCardType,cCardNumber,uExpMonth,uExpYear,"
			"cCardName,uOwner,1,UNIX_TIMESTAMP(NOW())");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	uInvoice=mysql_insert_id(&gMysql);
	
	sprintf(gcQuery,"INSERT INTO tInvoiceItems SET uInvoice=%u,uClient=%u,uProduct=1,"
			"uQuantity=1,mPrice='15.95',mTotal='15.95',uOwner=%u,uCreatedBy=1,"
			"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,uInvoice
			,guLoginClient
			,guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void GeneratePurchaseInvoice(void)


unsigned ValidPurchaseInput(void)
{
	//All form fields required
	
	if(!cAddr1[0])
	{
		gcMessage="<blink>Error:</blink> Address is a required field";
		cAddr1Style="type_fields_req";
		return(0);
	}
	
	if(!cCity[0])
	{
		gcMessage="<blink>Error:</blink> City is a required field";
		cCityStyle="type_fields_req";
		return(0);
	}
	
	if(!cState[0])
	{
		gcMessage="<blink>Error:</blink> State is a required field";
		cStateStyle="type_fields_req";
		return(0);
	}
	
	if(!cZip[0])
	{
		gcMessage="<blink>Error:</blink> Zip is a required field";
		cZipStyle="type_fields_req";
		return(0);
	}

	if(!cCardName[0])
	{
		gcMessage="<blink>Error:</blink> Card name is a required field";
		cCardNameStyle="type_fields_req";
		return(0);
	}

	if(!cCardNumber[0])
	{
		gcMessage="<blink>Error:</blink> Card number is a required field";
		cCardNumberStyle="type_fields_req";
		return(0);
	}

	if(!uExpMonth)
	{
		gcMessage="<blink>Error:</blink> Expiration month is a required field";
		cuExpMonthStyle="type_fields_req";
		return(0);
	}

	if(!uExpYear)
	{
		gcMessage="<blink>Error:</blink> Expiration year is a required field";
		cuExpYearStyle="type_fields_req";
		return(0);
	}
	return(1);

}//unsigned ValidPurchaseInput(void)


