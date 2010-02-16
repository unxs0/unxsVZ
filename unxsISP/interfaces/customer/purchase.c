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

void ShowConfirmPurchasePage(void);
void htmlPurchasePage(char *cTitle,char *cTemplateName);


void ProcessPurchaseVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{

//		if(!strcmp(entries[i].name,"cFirstName"))
//			sprintf(cFirstName,"%.32s",entries[i].val);
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

			template.cpName[23]="";
			
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



