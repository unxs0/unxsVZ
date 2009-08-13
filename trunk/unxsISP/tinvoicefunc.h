/*
FILE
	$Id$
PURPOSE
	Table where invoices with different statuses are kept usually
	closed invoices will be extracted along with their tInvoiceItems
	to RO compressed myISAM tables called <Month><Year>tInvoice.
	This set of tables is accessed/used with tInvoiceMonth interface page and
	the hidden tInvoiceItemsMonth parallel table.

AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details
	License included see LICENSE file.

TODO
	See TODOs in this file and...

	tClient summary mBalance mess...continues...lol
	Must work on creating new month debits on pre-existing monthly products.

	Where and when should this be done? [Generate Invoices]? Elsewhere?
	Remember about gcCommand line auto invoice generation and processing.

 
*/

//Clean up this mess of conflicting debug ifdef and uHtml etc...
//Provides a test report button. Has hardcoded uInstance!
//#define DEBUG_USAGE
//Provides Generate Invoice debug report page
//#define DEBUG 0
static unsigned guDebug=0;

#include "mysqlisp.h"

unsigned GenerateInvoicesCommand(unsigned uHtml);//Called from both mainfunc.h and webmin
void ClearForm(void);
void RemoveInvoices(void);
void InvoiceItemList(unsigned uInvoice);
void EmailInvoicesJob(time_t luJobTime);
void PrintInvoices(void);
unsigned GetCurrentNewInvoices(void);
unsigned GetNonQueuedInvoices(void);
void SubmitInvoiceJob(char *cJobType,time_t luJobDate);
unsigned GetInvoiceStatus(unsigned const uInvoice, unsigned *uQueueStatus);

//template.c hooks
void fileInvoice(FILE *fp);
void filePackingSlip(FILE *fp);
void textFatalError(char const *cErrorText);

//external
void GetBillingHours(unsigned const uInstance,char *mHours);
void GetBillingTraffic(unsigned const uInstance,char *mHours);
//tinstancefunc.h
void htmlDateSelectTable(void);
time_t cDateToUnixTime(char *cDate);
//unxsISP remote server connections
extern MYSQL mysqlext;
void ExtConnectDb(char *cDbName, char *cDbIp, char *cDbPwd, char *cDbLogin);
void TextConnectDb(void);

//file scope vars
static unsigned uSearch=0;
static unsigned uOpenOnly=0;
static unsigned uNewInvoices=0;
static char cDay[3]={""};
static char cMonth[4]={"1"};
static char cYear[5]={"2004"};


//Object in 2.x series software
typedef struct {

	//Everything should be in Megabytes
	double dHDQuota;
	double dTrafficQuota;
	double dFTPQuota;
	double dMailQuota;

	double dHD;
	double dTraffic;
	double dFTP;
	double dMail;

	unsigned uInstance;
	char cDomain[100];

	//These come from the tProduct
	double dTrafficPrice;//Price per traffic or storage MB over quota
	//tProduct.mCost hack: If mCost>mPrice and uPeriod=Storage or Traffic
	//then it is used as a divider to change mPrice into small number for
	//per MB cost calculation.
	double dTrafficDivisor;

	//Similarly
	double dFTPPrice;
	double dFTPDivisor;

	//Storage price is per MB so no need for divisor
	double dHDPrice;
	double dMailPrice;

	//Setup fee's: Per excess fixed charge not based on usage. If any.
	double dHDSetup;
	double dMailSetup;
	double dTrafficSetup;
	double dFTPSetup;

	//Calculated results: 
	//Example dHDTotal= ( (dHD-dHDQuota) * dHDPrice ) + dHDSetup
	//	If dHD>dHDQuota else dHDTotal=0;
	double dHDTotal;
	double dMailTotal;
	double dTrafficTotal;
	double dFTPTotal;

} structWebsiteUsage;
void InitializestructWebsiteUsage(structWebsiteUsage *structWebUsage);
unsigned WebSiteUsage(structWebsiteUsage *structWebUsage,unsigned uHtml);
#ifdef DEBUG_USAGE
void PrintWebSiteUsage(structWebsiteUsage *structWebUsage,unsigned uHtml);
#endif
void ProcessClientWebUsageProduct(structWebsiteUsage *structWebUsage,
	unsigned uClient,char *cLabel,unsigned uMode);

unsigned uQuarterOf(unsigned uMonth);
unsigned uSemesterOf(unsigned uMonth);
unsigned uCaseAnalyzer(unsigned const uProductPeriod, unsigned const uClientPeriod,
				unsigned const uLastPaidMonth, unsigned const uLastPaidYear,
				unsigned const uCreatedDateMonth, unsigned const uCreatedDateYear,
				unsigned const uMonth, unsigned const uYear, unsigned *uCase);
unsigned CreateNewInvoice(unsigned uClient, unsigned uHtml);
void fileDirectTemplate(FILE *fp,char *cTemplateName);
unsigned GetPaymentValue(unsigned uInvoice,const char *cName,char *cValue);
MYSQL_RES *sqlresultClientInfo(void);
void ReStockItems(unsigned uInvoice);
char *cGetInvoiceLanguage(unsigned uInvoice);
char *cGetCustomerEmail(unsigned uInvoice);
void EmailLoadedInvoice(void);
void EmailAllInvoices(void);


void ExtProcesstInvoiceVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uSearch"))
			sscanf(entries[i].val,"%u",&uSearch);
		else if(!strcmp(entries[i].name,"uOpenOnly"))
			uOpenOnly=1;
		else if(!strcmp(entries[i].name,"cDay") )
			sprintf(cDay,"%.2s",entries[i].val);
		else if(!strcmp(entries[i].name,"cMonth") )
			sprintf(cMonth,"%.3s",entries[i].val);
		else if(!strcmp(entries[i].name,"cYear") )
			sprintf(cYear,"%.4s",entries[i].val);
		else if(!strcmp(entries[i].name,"uNewInvoices") )
			sscanf(entries[i].val,"%u",&uNewInvoices);
	}
}//void ExtProcesstInvoiceVars(pentry entries[], int x)


void ExttInvoiceCommands(pentry entries[], int x)
{
	time_t luClock;
	time_t luJobTime;

	if(!strcmp(gcFunction,"tInvoiceTools"))
	{
		if(!strcmp(gcCommand,"Generate Invoices"))
                {
			if(guPermLevel>10)
			{
                        	ProcesstInvoiceVars(entries,x);
				if(!GenerateInvoicesCommand(1))
                        		tInvoice("This months invoices generated. Hit [Search].");
				else
                        		tInvoice("No invoices generated. No un-invoiced product instances found.");
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");   
		}
#ifdef DEBUG_USAGE
		else if(!strcmp(gcCommand,"TestWebSiteUsage"))
                {
			if(guPermLevel>10)
			{
				structWebsiteUsage structWebUsage;

                        	ProcesstInvoiceVars(entries,x);
				structWebUsage.uInstance=999948;//From accttestadd.mysql
				if(WebSiteUsage(&structWebUsage,1))
					PrintWebSiteUsage(&structWebUsage,1);
				else
                        		tInvoice("Test failed");
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");   
		}
#endif
		else if(!strcmp(gcCommand,"Email Invoices"))
                {
			if(guPermLevel>10)
			{
                        	ProcesstInvoiceVars(entries,x);

				uNewInvoices=GetNonQueuedInvoices();

				if(uNewInvoices)
				{
                        		guMode=3002;
                        		tInvoice("First Step...");
				}
				else
				{
					tInvoice("No invoices to email. Or existing are already queued.");
				}
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,"Confirm Email Invoices"))
                {
			if(guPermLevel>10)
			{
                        	ProcesstInvoiceVars(entries,x);

				if(uNewInvoices!=GetNonQueuedInvoices())
					tInvoice("A new invoice was found. Check this out.");

				time(&luClock);
				if(cDay[0] && cDay[0]!='N')
				{
					sprintf(gcQuery,"%s-%s-%s",cDay,cMonth,cYear);
					luJobTime=cDateToUnixTime(gcQuery);
				}
				else
				{
					luJobTime=luClock;
				}

				EmailInvoicesJob(luJobTime);
                        	tInvoice("Job created for invoices to be sent by email");
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,"Print Invoices"))
                {
			if(guPermLevel>10)
			{
                        	ProcesstInvoiceVars(entries,x);

				uNewInvoices=GetNonQueuedInvoices();

				if(uNewInvoices)
				{
                        		guMode=4002;
                        		tInvoice("First Step...");
				}
				else
				{
					tInvoice("No invoices to print. Or existing are already queued.");
				}
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,"Confirm Print Invoices"))
                {
			if(guPermLevel>10)
			{
                        	ProcesstInvoiceVars(entries,x);

				if(uNewInvoices!=GetNonQueuedInvoices())
					tInvoice("A new invoice was found. Check this out.");

				PrintInvoices();
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,"Remove Invoices"))
                {
			if(guPermLevel>10)
			{
                        	ProcesstInvoiceVars(entries,x);
				uNewInvoices=GetCurrentNewInvoices();

				if(uNewInvoices)
				{
					RemoveInvoices();
                        		tInvoice("'New' Invoices removed. Hit [Search] to refresh.");
				}
				else
				{
					tInvoice("No invoices to remove");
				}
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstInvoiceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				if(!uInvoice) 
					tInvoice("No uInvoice?");
				uInvoiceStatus=GetInvoiceStatus(uInvoice,&uQueueStatus);
				if(uInvoiceStatus!=unxsISP_NewInvoice) 
					tInvoice("Can't modify wrong status. Might have just changed.");
                        	guMode=2002;
				tInvoice(LANG_NB_CONFIRMMOD);
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstInvoiceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
                        	guMode=2002;
				if(!uInvoice) 
					tInvoice("No uInvoice?");
				uInvoiceStatus=GetInvoiceStatus(uInvoice,&uQueueStatus);
				if(uInvoiceStatus!=unxsISP_NewInvoice) 
					tInvoice("Can't modify wrong status. Might have just changed.");
                        	guMode=0;

				uModBy=guLoginClient;
				ModtInvoice();
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");   
                }
	}

}//void ExttInvoiceCommands(pentry entries[], int x)


void ExttInvoiceButtons(void)
{
	unsigned uDefault=0;

	OpenFieldSet("tInvoice Aux Panel",100);
	
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter required data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

                case 3002:
			printf("<p><u>Review Invoices First</u><br>They will be emailed as is. No undo available. "
				"You can also customize each invoice now, before committing. Press [Search] to backout. "
				"Do not email invoices again if you do not want to!<p>");
			htmlDateSelectTable();
                        printf("<input type=hidden name=uNewInvoices value=%u>",uNewInvoices);
			printf("<input class=lwarnButton title='Email invoices not already "
				"queued for emailing or printing.' type=submit name=gcCommand value='Confirm Email Invoices'><br>");
                break;


                case 4002:
			printf("<p><u>Review Invoices First</u><br>You can customize each invoice now. Press [Search] to backout.</u><p>");
                        printf("<input type=hidden name=uNewInvoices value=%u>",uNewInvoices);
			htmlDateSelectTable();
			printf("<input class=lalertButton title='Print invoices not already queued for emailing or printing.' "
				"type=submit name=gcCommand value='Confirm Print Invoices'><br>");
                break;


		default:
			uDefault=1;

			printf("<u>Search Tools</u><br>");
			printf("<input type=text title='Restrict search to invoices of this tClient.uClient only (customer id).' "
				"name=uSearch value=%u maxlength=99 size=10>",uSearch);
			printf("<br>Open Invoices Only <input type=checkbox title='Restrict search to open invoices.' name=uOpenOnly ");
			if(uOpenOnly)
				printf("Checked>\n");
			else
				printf(">\n");

			if(guPermLevel>10)
			{
				printf("<p><u>Billing Functions</u><br>");
				printf("<input class=largeButton title='Generate this months invoices. Your company will own them. "
					"Only the owner can remove \"New\" ones, mail or print them.' type=submit "
					"name=gcCommand value='Generate Invoices'><br>");
				printf("<input class=largeButton title='Remove the \"New\" status [Generate Invoices] "
					"created invoices you own. mySQL 4.0.4+.' type=submit name=gcCommand value='Remove Invoices'><br>");
				printf("<input class=lwarnButton title='Email all not already queued invoices your company owns. "
					"You better check them all first!' type=submit name=gcCommand value='Email Invoices'><br>");
				printf("<input class=lalertButton title='Print all not already queued invoices your company owns.'"
					" type=submit name=gcCommand value='Print Invoices'><br>");

			}
#ifdef DEBUG_USAGE
			if(guPermLevel>=12)
			{
				printf("<input class=largeButton title='Test'"
				" type=submit name=gcCommand value='TestWebSiteUsage'><br>");
			}
#endif

			if(uInvoice)
			{
				InvoiceItemList(uInvoice);
			}
	}

	if(!uDefault)
	{
		printf("<input type=hidden name=uSearch value=%u>",uSearch);
		printf("<input type=hidden name=uOpenOnly value=%u>",uOpenOnly);
	}

	CloseFieldSet();
	
}//void ExttInvoiceButtons(void)


void ExttInvoiceAuxTable(void)
{

}//void ExttInvoiceAuxTable(void)


void ExttInvoiceGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uInvoice"))
		{
			sscanf(gentries[i].val,"%u",&uInvoice);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"uSearch"))
		{
			sscanf(gentries[i].val,"%u",&uSearch);
		}
		else if(!strcmp(gentries[i].name,"uOpenOnly"))
		{
			sscanf(gentries[i].val,"%u",&uOpenOnly);
		}
	}
	tInvoice("");

}//void ExttInvoiceGetHook(entry gentries[], int x)


void ExttInvoiceSelect(void)
{
	char cExtra[65]={""};
	
	if(uSearch && uOpenOnly)
	{
		sprintf(cExtra,"uClient=%u AND uInvoiceStatus<100",uSearch);
		ExtSelectSearch("tInvoice",VAR_LIST_tInvoice,"1","1",cExtra,0);
	}
	else if(uSearch)
	{
		sprintf(cExtra,"uClient=%u",uSearch);
		ExtSelectSearch("tInvoice",VAR_LIST_tInvoice,"1","1",cExtra,0);
	}
	else if(uOpenOnly)
		ExtSelectSearch("tInvoice",VAR_LIST_tInvoice,"1","1","uInvoiceStatus<100",0);
	else if(1)
		ExtSelect("tInvoice",VAR_LIST_tInvoice,0);
	
}//void ExttInvoiceSelect(void)


void ExttInvoiceSelectRow(void)
{
	ExtSelectRow("tInvoice",VAR_LIST_tInvoice,uInvoice);

}//void ExttInvoiceSelectRow(void)


void ExttInvoiceListSelect(void)
{
	char cCat[512];

	ExtListSelect("tInvoice",VAR_LIST_tInvoice);

	//Changes here must be reflected below in ExttInvoiceListFilter()
        if(!strcmp(gcFilter,"uInvoice"))
        {
                sscanf(gcCommand,"%u",&uInvoice);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tInvoice.uInvoice=%u ORDER BY uInvoice",uInvoice);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uClient"))
        {
                sscanf(gcCommand,"%u",&uClient);
		sprintf(cCat,"AND tInvoice.uClient=%u ORDER BY uInvoice",uClient);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uInvoice");
        }

}//void ExttInvoiceListSelect(void)


void ExttInvoiceListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uClient"))
                printf("<option>uClient</option>");
        else
                printf("<option selected>uClient</option>");
        if(strcmp(gcFilter,"uInvoice"))
                printf("<option>uInvoice</option>");
        else
                printf("<option selected>uInvoice</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttInvoiceListFilter(void)


void ExttInvoiceNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if( (guPermLevel>7 && uOwner==guLoginClient)
			|| (guPermLevel>11)
			|| (guPermLevel>9 && uOwner!=1)
			|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_MODIFY);

	printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttInvoiceNavBar(void)


void ClearForm(void)
{
	uInvoice=0;
	uInvoiceStatus=0;
	uOwner=0;
	uCreatedBy=0;
	uCreatedDate=0;
	cFirstName[0]=0;
	cLastName[0]=0;
	cEmail[0]=0;
	cAddr1[0]=0;
	cAddr2[0]=0;
	cCity[0]=0;
	cState[0]=0;
	cZip[0]=0;
	cCountry[0]=0;
	cCardType[0]=0;
	cCardNumber[0]=0;
	uExpMonth=0;
	uExpYear=0;
	cCardName[0]=0;
	cShipName[0]=0;
	cShipAddr1[0]=0;
	cShipAddr2[0]=0;
	cShipCity[0]=0;
	cShipState[0]=0;
	cShipZip[0]=0;
	cShipCountry[0]=0;
	cTelephone[0]=0;
	cFax[0]=0;
	uClient=0;
	uPayment=0;
	mTotal[0]=0;
	mTax[0]=0;
	mSH[0]=0;
	cComments[0]=0;
	cAuthCode[0]=0;
	cPnRef[0]=0;
	uAvsAddr=0;
	uAvsZip=0;
	uResultCode=0;
	
}//void ClearForm(void)


//TODO Need to clarify and normalize this mess
void RemoveInvoices(void)
{

	unsigned uMainVersion=0,uMinorVersion=0,uSubVersion=0;

	sscanf(mysql_get_server_info(&gMysql),"%u.%u.%u",&uMainVersion,&uMinorVersion,&uSubVersion);

	if(uMainVersion<4 || (uMainVersion==4 && uSubVersion<4 && uMinorVersion<1) )
		tInvoice("RemoveInvoices() Needs at least mySQL 4.0.4 server");
	//This gcFunction is only mySQL 4.0.4+ compatible: Multiple table UPDATE and DELETE

	//Only remove new from this month. Any new from previous months will stay and
	//have to be handled by special execeptional admin procedure?
	//EXTRACT(MONTH FROM FROM_UNIXTIME(tInstance.uBilled))!=EXTRACT(MONTH FROM NOW())
	//EXTRACT(YEAR FROM FROM_UNIXTIME(tInstance.uBilled))!=EXTRACT(YEAR FROM NOW())
	if(guPermLevel>9)
		sprintf(gcQuery,"UPDATE tInstance,tClient,tInvoice SET tInstance.uBilled=0,tClient.mBalance=tClient.mBalance-tInvoice.mTotal WHERE "
				"tInstance.uClient=tClient.uClient AND tInvoice.uClient=tClient.uClient AND tInstance.uClient=tClient.uClient AND "
				"( tInstance.uStatus=%u OR tInstance.uStatus=%u OR tInstance.uStatus=%u OR tInstance.uStatus=%u OR tInstance.uStatus=%u )"
				" AND tInvoice.uInvoiceStatus=%u AND tInvoice.uQueueStatus=0 AND ( tInstance.uBilled=0 OR "
				"(EXTRACT(MONTH FROM FROM_UNIXTIME(tInstance.uBilled))=EXTRACT(MONTH FROM NOW()) "
				"AND EXTRACT(YEAR FROM FROM_UNIXTIME(tInstance.uBilled))=EXTRACT(YEAR FROM NOW())) )",
				unxsISP_Deployed,unxsISP_DeployedMod,unxsISP_OnHold,unxsISP_WaitingRedeployment,unxsISP_WaitingHold,unxsISP_NewInvoice);
	else
		sprintf(gcQuery,"UPDATE tInstance,tClient,tInvoice SET tInstance.uBilled=0,tClient.mBalance=tClient.mBalance-tInvoice.mTotal "
				"WHERE tInstance.uClient=tClient.uClient AND tInvoice.uClient=tClient.uClient AND tInstance.uClient=tClient.uClient "
				"AND (tClient.uClient=%u OR tClient.uOwner=%u) AND ( tInstance.uStatus=%u OR tInstance.uStatus=%u OR "
				"tInstance.uStatus=%u OR tInstance.uStatus=%u OR tInstance.uStatus=%u ) AND tInvoice.uInvoiceStatus=%u AND "
				"tInvoice.uQueueStatus=0 AND tInvoice.uOwner=%u AND ( tInstance.uBilled=0 OR (EXTRACT(MONTH FROM "
				"FROM_UNIXTIME(tInstance.uBilled))=EXTRACT(MONTH FROM NOW()) AND EXTRACT(YEAR FROM "
				"FROM_UNIXTIME(tInstance.uBilled))!=EXTRACT(YEAR FROM NOW())) )"
				,guLoginClient,guLoginClient,unxsISP_Deployed,unxsISP_DeployedMod,unxsISP_OnHold,
				unxsISP_WaitingRedeployment,unxsISP_WaitingHold,unxsISP_NewInvoice,guLoginClient);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tInvoice(mysql_error(&gMysql));

	if(guPermLevel>9)
		sprintf(gcQuery,"DELETE tInvoice,tInvoiceItems FROM tInvoice,tInvoiceItems WHERE "
				"tInvoice.uInvoice=tInvoiceItems.uInvoice AND tInvoice.uInvoiceStatus=%u "
				"AND tInvoice.uQueueStatus=0 AND EXTRACT(MONTH FROM FROM_UNIXTIME(tInvoice.uCreatedDate))"
				"=EXTRACT(MONTH FROM NOW()) AND EXTRACT(YEAR FROM FROM_UNIXTIME(tInvoice.uCreatedDate))"
				"=EXTRACT(YEAR FROM NOW())",unxsISP_NewInvoice);
	else
		sprintf(gcQuery,"DELETE tInvoice,tInvoiceItems FROM tInvoice,tInvoiceItems WHERE "
				"tInvoice.uInvoice=tInvoiceItems.uInvoice AND tInvoice.uOwner=%u "
				"AND tInvoice.uInvoiceStatus=%u AND tInvoice.uQueueStatus=0 AND "
				"EXTRACT(MONTH FROM FROM_UNIXTIME(tInvoice.uCreatedDate))="
				"EXTRACT(MONTH FROM NOW()) AND EXTRACT(YEAR FROM "
				"FROM_UNIXTIME(tInvoice.uCreatedDate))=EXTRACT(YEAR FROM NOW())",guLoginClient,unxsISP_NewInvoice);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tInvoice(mysql_error(&gMysql));

	ClearForm();

}//void RemoveInvoices()



void InvoiceItemList(unsigned uInvoice)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCount=0;
	double dTotal=0.0000;
	double dPrice=0.0;

	sprintf(gcQuery,"SELECT tInvoiceItems.uInvoiceItems,tInvoiceItems.mPrice,tProduct.cLabel FROM "
		"tInvoiceItems,tProduct WHERE tProduct.uProduct=tInvoiceItems.uProduct AND tInvoiceItems.uInvoice=%u "
		"ORDER BY tInvoiceItems.uInvoiceItems",uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s",mysql_error(&gMysql));
		return;
	}

	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		printf("<font size=2>");
		printf("<p><u>Invoice Items</u><br>");

		while((field=mysql_fetch_row(res)))
		{
			uCount++;

			sscanf(field[1],"%lf",&dPrice);
			dTotal+=dPrice;

			if(uSearch)
				printf("%u-. <a class=darkLink href=unxsISP.cgi?gcFunction=tInvoiceItems"
					"&uInvoiceItems=%s&uSearch=%u>%s. $%s</a>\n",
					uCount,field[0],uSearch,field[2],field[1]);
			else
				printf("%u-. <a class=darkLink href=unxsISP.cgi?gcFunction=tInvoiceItems"
					"&uInvoiceItems=%s>%s. $%s</a>\n",
					uCount,field[0],field[2],field[1]);
			printf("<br>\n");
		}
		mysql_free_result(res);

		printf("<u>Total</u><br>$%2.2f\n",dTotal);

		if(dTotal > 0.0001)
			printf("<p><a class=darkLink href=unxsISP.cgi?gcFunction=tPaid"
				"&uClient=%u&uPayment=%u&mTotal=%2.2lf>[Post Manual Payment]</a>\n",uClient,uPayment,dTotal);

		if(uClient)
			printf("<p><a class=darkLink href=unxsISP.cgi?gcFunction=tClient&uClient=%u>[Client]</a><br>\n",uClient);

	}

}//void InvoiceItemList(unsigned uInvoice)


void EmailInvoicesJob(time_t luJobTime)
{
	//First change tInvoiceStatus
	if(guPermLevel>9)
		sprintf(gcQuery,"UPDATE tInvoice SET uQueueStatus=%u WHERE uQueueStatus=0",unxsISP_MailQueuedInvoice);
	else
		sprintf(gcQuery,"UPDATE tInvoice SET uQueueStatus=%u WHERE uQueueStatus=0 AND uOwner=%u",unxsISP_MailQueuedInvoice,guLoginClient);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tInvoice(mysql_error(&gMysql));

	//Submit job
	SubmitInvoiceJob("Email",luJobTime);

}//void EmailInvoicesJob()


void PrintInvoices(void)
{

}//void PrintInvoices()


//Count current 'New' invoices for this user or all if admin/root user.
//Also must not have a job queue status. The tJob will be fscked up.
unsigned GetCurrentNewInvoices(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uInvoices=0;

	if(guPermLevel<10)
	//Remove only your own "new" invoices and the corresponding invoice items
		sprintf(gcQuery,"SELECT COUNT(uInvoice) FROM tInvoice WHERE uOwner=%u AND "
				"uInvoiceStatus=%u AND uQueueStatus=0 AND "
				"EXTRACT(MONTH FROM FROM_UNIXTIME(tInvoice.uCreatedDate))=EXTRACT(MONTH FROM NOW()) "
				"AND EXTRACT(YEAR FROM FROM_UNIXTIME(tInvoice.uCreatedDate))=EXTRACT(YEAR FROM NOW())",
				guLoginClient,unxsISP_NewInvoice);
	else
		sprintf(gcQuery,"SELECT COUNT(uInvoice) FROM tInvoice WHERE uInvoiceStatus=%u AND uQueueStatus=0 AND "
				"EXTRACT(MONTH FROM FROM_UNIXTIME(tInvoice.uCreatedDate))=EXTRACT(MONTH FROM NOW()) "
				"AND EXTRACT(YEAR FROM FROM_UNIXTIME(tInvoice.uCreatedDate))=EXTRACT(YEAR FROM NOW())",unxsISP_NewInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tInvoice(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uInvoices);
	mysql_free_result(res);

	return(uInvoices);

}//unsigned GetCurrentNewInvoices(void)


unsigned GetNonQueuedInvoices(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uInvoices=0;

	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT COUNT(uInvoice) FROM tInvoice WHERE uOwner=%u AND uQueueStatus=0",guLoginClient);
	else
	sprintf(gcQuery,"SELECT COUNT(uInvoice) FROM tInvoice WHERE uQueueStatus=0");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tInvoice(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uInvoices);
	mysql_free_result(res);

	return(uInvoices);

}//unsigned GetNonQueuedInvoices(void)


//Simple prototype: Future issues, jobs for only a single customer but done by root.
void SubmitInvoiceJob(char *cJobType,time_t luJobDate)
{
	char cJobName[33]={""};
	time_t luClock;
	
	sprintf(cJobName,"unxsISP.tInvoice.%.20s",cJobType);

	//Remove all waiting jobs by jobname and authorized user: Study this further
	sprintf(gcQuery,"DELETE FROM tJob WHERE uInstance=0 AND uJobStatus=%u AND cJobName='%s' AND uOwner=%u AND uJobClient=%u",
			unxsISP_Waiting,cJobName,guLoginClient,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	//Create the new job. Note uInstance=0 this is a jopb not based on a product deployment.
	time(&luClock);
	sprintf(gcQuery,"INSERT INTO tJob SET uJobGroup=uJob,cLabel='Priority=Low',cServer='Any',cJobName='%s',"
			"uJobClient=%u,uInstance=0,cJobData='%.20s all tInvoice records with the corresponding uQueueStatus.\n"
			"Once done reset uQueueStatus to 0.',uJobDate=%lu,uJobStatus=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
				cJobName,
				guLoginClient,
				cJobType,
				luJobDate,
				unxsISP_Waiting,
				guLoginClient,
				guLoginClient,
				luClock);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void SubmitInvoiceJob()


unsigned GetInvoiceStatus(unsigned const uInvoice, unsigned *uQueueStatus)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uStatus=0;

	sprintf(gcQuery,"SELECT uInvoiceStatus FROM tInvoice WHERE uInvoice=%u",uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tInvoice(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uStatus);
		sscanf(field[1],"%u",uQueueStatus);
	}
	mysql_free_result(res);

	return(uStatus);

}//unsigned GetInvoiceStatus()



//template.c "library" hooks


void fileInvoice(FILE *fp)
{
}//void fileInvoice(FILE *fp)


void filePackingSlip(FILE *fp)
{
}//void filePackingSlip(FILE *fp)


void textFatalError(char const *cErrorText)
{
	htmlPlainTextError(cErrorText);
}//void textFatalError(char const *cErrorText)


void InitializestructWebsiteUsage(structWebsiteUsage *structWebUsage)
{
	
	structWebUsage->dHDQuota=0.0;
	structWebUsage->dTrafficQuota=0.0;
	structWebUsage->dFTPQuota=0.0;
	structWebUsage->dMailQuota=0.0;

	structWebUsage->dHD=0.0;
	structWebUsage->dTraffic=0.0;
	structWebUsage->dFTP=0.0;
	structWebUsage->dMail=0.0;

	//This one should be set on entry
	//structWebUsage->uInstance=0;
	structWebUsage->cDomain[0]=0;

	structWebUsage->dTrafficPrice=0.0;
	structWebUsage->dTrafficDivisor=1.0;

	structWebUsage->dFTPPrice=0.0;
	structWebUsage->dFTPDivisor=1.0;

	structWebUsage->dHDPrice=0.0;
	structWebUsage->dMailPrice=0.0;

	structWebUsage->dHDSetup=0.0;
	structWebUsage->dMailSetup=0.0;
	structWebUsage->dTrafficSetup=0.0;
	structWebUsage->dFTPSetup=0.0;

	structWebUsage->dHDTotal=0.0;
	structWebUsage->dMailTotal=0.0;
	structWebUsage->dTrafficTotal=0.0;
	structWebUsage->dFTPTotal=0.0;

}//void InitializestructWebsiteUsage()


#ifdef DEBUG_USAGE
void PrintWebSiteUsage(structWebsiteUsage *structWebUsage,unsigned uHtml)
{

	if(uHtml) printf("Content-type: text/plain\n\n");

	printf("uInstance=%u\n\ncDomain=%s\n",structWebUsage->uInstance,structWebUsage->cDomain);

	printf("\nHD Storage\ndHD=%lf dHDQuota=%lf dHDPrice=%lf\ndHDSetup=%lf dHDTotal=%lf\n",
		structWebUsage->dHD,
		structWebUsage->dHDQuota,
		structWebUsage->dHDPrice,
		structWebUsage->dHDSetup,
		structWebUsage->dHDTotal);

	printf("\nMail Storage\ndMail=%lf dMailQuota=%lf dMailPrice=%lf\ndMailSetup=%lf dMailTotal=%lf\n",
		structWebUsage->dMail,
		structWebUsage->dMailQuota,
		structWebUsage->dMailPrice,
		structWebUsage->dMailSetup,
		structWebUsage->dMailTotal);

	printf("\nHTTP Traffic\ndTraffic=%lf dTrafficQuota=%lf dTrafficPrice=%lf\n"
			"dTrafficDivisor=%lf dTrafficSetup=%lf dTrafficTotal=%lf\n",
		structWebUsage->dTraffic,
		structWebUsage->dTrafficQuota,
		structWebUsage->dTrafficPrice,
		structWebUsage->dTrafficDivisor,
		structWebUsage->dTrafficSetup,
		structWebUsage->dTrafficTotal);

	printf("\nFTP Traffic\ndFTP=%lf dFTPQuota=%lf dFTPPrice=%lf\n"
			"dFTPDivisor=%lf dFTPSetup=%lf dFTPTotal=%lf\n",
		structWebUsage->dFTP,
		structWebUsage->dFTPQuota,
		structWebUsage->dFTPPrice,
		structWebUsage->dFTPDivisor,
		structWebUsage->dFTPSetup,
		structWebUsage->dFTPTotal);


	if(uHtml)
		exit(0);

}//void PrintWebSiteUsage()
#endif


unsigned WebSiteUsage(structWebsiteUsage *structWebUsage, unsigned uHtml)
{
	MYSQL_RES *res;
        MYSQL_ROW field;

	char cConfQuery[256];
	char cIp[100]={""};
	char cDb[256]={""};
	char cPwd[256]={""};
	char cLogin[256]={""};
	char cProductFamily[33]={""};
	char *cp;
	char cExtQueryHeader[256]={""};
	MYSQL_RES *res2;
       	MYSQL_ROW field2;

	if(!structWebUsage->uInstance)
		return(0);

	//Basic deployed product data
        sprintf(gcQuery,"SELECT tClientConfig.cValue FROM tClientConfig,tParameter WHERE "
			"tClientConfig.uParameter=tParameter.uParameter AND tClientConfig.uGroup=%u "
			"AND tParameter.cParameter='mysqlApache.AutoUsage'",structWebUsage->uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sprintf(cProductFamily,"%.32s",field[0]);
	mysql_free_result(res);

	if(!cProductFamily[0])
		return(0);

        sprintf(gcQuery,"SELECT tClientConfig.cValue FROM tClientConfig,tParameter WHERE "
			"tClientConfig.uParameter=tParameter.uParameter AND "
			"tClientConfig.uGroup=%u AND tParameter.cParameter='mysqlApache.IP'",structWebUsage->uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sprintf(cIp,"%.99s",field[0]);
		//If not in customer product or if localhost: Use mySQL socket
		if(!strncmp(cIp,"localhost",9)) cIp[0]=0;
	}
	mysql_free_result(res);


	InitializestructWebsiteUsage(structWebUsage);


	//Special product data
        sprintf(gcQuery,"SELECT tProduct.mPrice,tProduct.mCost,tProduct.mSetupFee FROM"
			" tProduct,tPeriod WHERE"
			" tProduct.uPeriod=tPeriod.uPeriod AND"
			" tProduct.cLabel='%s Traffic' AND"
			" tPeriod.cLabel='Traffic'",cProductFamily);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%lf",&structWebUsage->dTrafficPrice);
		sscanf(field[1],"%lf",&structWebUsage->dTrafficDivisor);
		sscanf(field[2],"%lf",&structWebUsage->dTrafficSetup);
		if(structWebUsage->dTrafficDivisor<=structWebUsage->dTrafficPrice)
				structWebUsage->dTrafficDivisor=1.0;
	}
	mysql_free_result(res);

        sprintf(gcQuery,"SELECT tProduct.mPrice,tProduct.mCost,tProduct.mSetupFee FROM"
			" tProduct,tPeriod WHERE"
			" tProduct.uPeriod=tPeriod.uPeriod AND"
			" tProduct.cLabel='%s FTP Traffic' AND"
			" tPeriod.cLabel='Traffic'",cProductFamily);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%lf",&structWebUsage->dFTPPrice);
		sscanf(field[1],"%lf",&structWebUsage->dFTPDivisor);
		sscanf(field[2],"%lf",&structWebUsage->dFTPSetup);
		if(structWebUsage->dFTPDivisor<=structWebUsage->dFTPPrice)
				structWebUsage->dFTPDivisor=1.0;
	}
	mysql_free_result(res);

        sprintf(gcQuery,"SELECT tProduct.mPrice,tProduct.mSetupFee FROM"
			" tProduct,tPeriod WHERE"
			" tProduct.uPeriod=tPeriod.uPeriod AND"
			" tProduct.cLabel='%s Storage' AND"
			" tPeriod.cLabel='Storage'",cProductFamily);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%lf",&structWebUsage->dHDPrice);
		sscanf(field[1],"%lf",&structWebUsage->dHDSetup);
	}
	mysql_free_result(res);

        sprintf(gcQuery,"SELECT tProduct.mPrice,tProduct.mSetupFee FROM"
			" tProduct,tPeriod WHERE"
			" tProduct.uPeriod=tPeriod.uPeriod AND"
			" tProduct.cLabel='%s Mail Storage' AND"
			" tPeriod.cLabel='Storage'",cProductFamily);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%lf",&structWebUsage->dMailPrice);
		sscanf(field[1],"%lf",&structWebUsage->dMailSetup);
	}
	mysql_free_result(res);


	//ClientConfig data
        sprintf(gcQuery,"SELECT tClientConfig.cValue FROM tClientConfig,tParameter WHERE "
			"tClientConfig.uParameter=tParameter.uParameter AND tClientConfig.uGroup=%u AND "
			"tParameter.cParameter='mysqlApache.Domain'",structWebUsage->uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sprintf(structWebUsage->cDomain,"%.99s",field[0]);
	mysql_free_result(res);

        sprintf(gcQuery,"SELECT tClientConfig.cValue FROM tClientConfig,tParameter WHERE "
			"tClientConfig.uParameter=tParameter.uParameter AND "
			"tClientConfig.uGroup=%u AND tParameter.cParameter='mysqlApache.HDQuota'",structWebUsage->uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%lf",&structWebUsage->dHDQuota);
	mysql_free_result(res);

        sprintf(gcQuery,"SELECT tClientConfig.cValue FROM tClientConfig,tParameter WHERE "
			"tClientConfig.uParameter=tParameter.uParameter AND tClientConfig.uGroup=%u "
			"AND tParameter.cParameter='mysqlApache.TrafficQuota'",structWebUsage->uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%lf",&structWebUsage->dTrafficQuota);
	mysql_free_result(res);

        sprintf(gcQuery,"SELECT tClientConfig.cValue FROM tClientConfig,tParameter WHERE "
			"tClientConfig.uParameter=tParameter.uParameter AND tClientConfig.uGroup=%u "
			"AND tParameter.cParameter='mysqlApache.FTPQuota'",structWebUsage->uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%lf",&structWebUsage->dFTPQuota);
	mysql_free_result(res);

        sprintf(gcQuery,"SELECT tClientConfig.cValue FROM tClientConfig,tParameter WHERE "
			"tClientConfig.uParameter=tParameter.uParameter AND tClientConfig.uGroup=%u "
			"AND tParameter.cParameter='mysqlApache.MailQuota'",structWebUsage->uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%lf",&structWebUsage->dMailQuota);
	mysql_free_result(res);


	//Working with special products from here...

	//Website HD usage special product
	//This schema mess needs definite cleanup...
	//This mess allows for little flexibility.
	//After this is running we need to look at a better solution.
        sprintf(gcQuery,"SELECT tParameter.cExtVerify FROM tProduct,"
			"tServiceGlue, tConfig, tParameter"
			" WHERE tProduct.uProduct=tServiceGlue.uServiceGroup AND"
			" tServiceGlue.uService=tConfig.uGroup AND"
			" tConfig.uParameter=tParameter.uParameter AND"
			" tProduct.cLabel='%s Storage' AND"
			" tParameter.cParameter='HDStorage'",cProductFamily);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{

		//Connect to external mysqlApache db
		//Use query to get data
		if((cp=strchr(field[0],':')))
		{
			*cp=0;

			sprintf(cConfQuery,"%.200sName",field[0]);
			GetConfiguration(cConfQuery,cDb,1);
			sprintf(cConfQuery,"%.200sPwd",field[0]);
			GetConfiguration(cConfQuery,cPwd,1);
			sprintf(cConfQuery,"%.200sLogin",field[0]);
			GetConfiguration(cConfQuery,cLogin,1);
		
			if(cDb[0]==0 || cPwd[0]==0 || cLogin[0]==0)
				return(0);
			ExtConnectDb(cDb,cIp,cPwd,cLogin);

			//Save this to save time and mySQL connections later
			strcpy(cExtQueryHeader,field[0]);


			sprintf(cConfQuery,cp+1,structWebUsage->cDomain);
			mysql_query(&mysqlext,cConfQuery);
        		if(mysql_errno(&mysqlext))
			{
				if(uHtml)
					htmlPlainTextError(mysql_error(&mysqlext));
				else
					printf("%s\n",mysql_error(&mysqlext));
				exit(1);
			}
			res2=mysql_store_result(&mysqlext);
        		if((field2=mysql_fetch_row(res2)))
				sscanf(field2[0],"%lf",&structWebUsage->dHD);
			mysql_free_result(res2);
		}
	}
	mysql_free_result(res);

	//Website traffic usage special product
        sprintf(gcQuery,"SELECT tParameter.cExtVerify FROM tProduct,"
			"tServiceGlue, tConfig, tParameter"
			" WHERE tProduct.uProduct=tServiceGlue.uServiceGroup AND"
			" tServiceGlue.uService=tConfig.uGroup AND"
			" tConfig.uParameter=tParameter.uParameter AND"
			" tProduct.cLabel='%s Traffic' AND"
			" tParameter.cParameter='HTTPTraffic'",cProductFamily);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		//Connect to external mysqlApache db
		//Use query to get data
		if((cp=strchr(field[0],':')))
		{

			*cp=0;

			if(strcmp(field[0],cExtQueryHeader))
			{
				mysql_close(&mysqlext);

				sprintf(cConfQuery,"%.200sName",field[0]);
				GetConfiguration(cConfQuery,cDb,1);
				sprintf(cConfQuery,"%.200sPwd",field[0]);
				GetConfiguration(cConfQuery,cPwd,1);
				sprintf(cConfQuery,"%.200sLogin",field[0]);
				GetConfiguration(cConfQuery,cLogin,1);
		
				if(cDb[0]==0 || cPwd[0]==0 || cLogin[0]==0)
					return(0);
				ExtConnectDb(cDb,cIp,cPwd,cLogin);
				strcpy(cExtQueryHeader,field[0]);
			}


			sprintf(cConfQuery,cp+1,structWebUsage->cDomain);
			mysql_query(&mysqlext,cConfQuery);
        		if(mysql_errno(&mysqlext))
			{
				if(uHtml)
					htmlPlainTextError(mysql_error(&mysqlext));
				else
					printf("%s\n",mysql_error(&mysqlext));
				exit(1);
			}
			res2=mysql_store_result(&mysqlext);
        		if((field2=mysql_fetch_row(res2)))
				sscanf(field2[0],"%lf",&structWebUsage->dTraffic);
			mysql_free_result(res2);
		}
	}
	mysql_free_result(res);

	//Website ftp traffic usage special product
        sprintf(gcQuery,"SELECT tParameter.cExtVerify FROM tProduct,"
			"tServiceGlue, tConfig, tParameter"
			" WHERE tProduct.uProduct=tServiceGlue.uServiceGroup AND"
			" tServiceGlue.uService=tConfig.uGroup AND"
			" tConfig.uParameter=tParameter.uParameter AND"
			" tProduct.cLabel='%s FTP Traffic' AND"
			" tParameter.cParameter='FTPTraffic'",cProductFamily);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		//Connect to external mysqlApache db
		//Use query to get data
		if((cp=strchr(field[0],':')))
		{

			*cp=0;

			if(strcmp(field[0],cExtQueryHeader))
			{
				mysql_close(&mysqlext);

				sprintf(cConfQuery,"%.200sName",field[0]);
				GetConfiguration(cConfQuery,cDb,1);
				sprintf(cConfQuery,"%.200sPwd",field[0]);
				GetConfiguration(cConfQuery,cPwd,1);
				sprintf(cConfQuery,"%.200sLogin",field[0]);
				GetConfiguration(cConfQuery,cLogin,1);
		
				if(cDb[0]==0 || cPwd[0]==0 || cLogin[0]==0)
					return(0);
				ExtConnectDb(cDb,cIp,cPwd,cLogin);
				strcpy(cExtQueryHeader,field[0]);
			}


			sprintf(cConfQuery,cp+1,structWebUsage->cDomain);
			mysql_query(&mysqlext,cConfQuery);
        		if(mysql_errno(&mysqlext))
			{
				if(uHtml)
					htmlPlainTextError(mysql_error(&mysqlext));
				else
					printf("%s\n",mysql_error(&mysqlext));
				exit(1);
			}
			res2=mysql_store_result(&mysqlext);
        		if((field2=mysql_fetch_row(res2)))
				sscanf(field2[0],"%lf",&structWebUsage->dFTP);
			mysql_free_result(res2);
		}
	}
	mysql_free_result(res);

	//Website mail HD usage special product
        sprintf(gcQuery,"SELECT tParameter.cExtVerify FROM tProduct,"
			"tServiceGlue, tConfig, tParameter"
			" WHERE tProduct.uProduct=tServiceGlue.uServiceGroup AND"
			" tServiceGlue.uService=tConfig.uGroup AND"
			" tConfig.uParameter=tParameter.uParameter AND"
			" tProduct.cLabel='%s Mail Storage' AND"
			" tParameter.cParameter='MailStorage'",cProductFamily);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		//Connect to external mysqlApache db
		//Use query to get data
		if((cp=strchr(field[0],':')))
		{

			*cp=0;

			if(strcmp(field[0],cExtQueryHeader))
			{
				mysql_close(&mysqlext);

				sprintf(cConfQuery,"%.200sName",field[0]);
				GetConfiguration(cConfQuery,cDb,1);
				sprintf(cConfQuery,"%.200sPwd",field[0]);
				GetConfiguration(cConfQuery,cPwd,1);
				sprintf(cConfQuery,"%.200sLogin",field[0]);
				GetConfiguration(cConfQuery,cLogin,1);
		
				if(cDb[0]==0 || cPwd[0]==0 || cLogin[0]==0)
					return(0);
				ExtConnectDb(cDb,cIp,cPwd,cLogin);
				strcpy(cExtQueryHeader,field[0]);
			}


			sprintf(cConfQuery,cp+1,structWebUsage->cDomain);
			mysql_query(&mysqlext,cConfQuery);
        		if(mysql_errno(&mysqlext))
			{
				if(uHtml)
					htmlPlainTextError(mysql_error(&mysqlext));
				else
					printf("%s\n",mysql_error(&mysqlext));
				exit(1);
			}
			res2=mysql_store_result(&mysqlext);
        		if((field2=mysql_fetch_row(res2)))
				sscanf(field2[0],"%lf",&structWebUsage->dMail);
			mysql_free_result(res2);
		}
	}
	mysql_free_result(res);


	//Calculate totals to invoice
	if(structWebUsage->dHD>structWebUsage->dHDQuota)
		structWebUsage->dHDTotal=((structWebUsage->dHD-structWebUsage->dHDQuota)*structWebUsage->dHDPrice)+structWebUsage->dHDSetup;

	if(structWebUsage->dMail>structWebUsage->dMailQuota)
		structWebUsage->dMailTotal=((structWebUsage->dMail-structWebUsage->dMailQuota)*structWebUsage->dMailPrice)+structWebUsage->dMailSetup;

	if(structWebUsage->dFTP>structWebUsage->dFTPQuota)
		structWebUsage->dFTPTotal=((structWebUsage->dFTP-structWebUsage->dFTPQuota)*(structWebUsage->dFTPPrice/structWebUsage->dFTPDivisor))
			+structWebUsage->dFTPSetup;

	if(structWebUsage->dTraffic>structWebUsage->dTrafficQuota)
		structWebUsage->dTrafficTotal=
			((structWebUsage->dTraffic-structWebUsage->dTrafficQuota)*(structWebUsage->dTrafficPrice/structWebUsage->dTrafficDivisor))
			+structWebUsage->dTrafficSetup;


	return(1);

}//unsigned WebSiteUsage()


void ProcessClientWebUsageProduct(structWebsiteUsage *structWebUsage,
		unsigned uClient,char *cLabel, unsigned uMode)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	char cuWebsiteUsageProduct[256]={"0"};
	unsigned uProduct=0;
	unsigned uParameter=0;
	unsigned uInstance=0;
	unsigned uHtml=1;
	time_t luClock;
	double dTotal=0.0;
	unsigned uGroup=0;
	double dAmountChanged=0.0;//Diff between dTotal and dAmount.
	double dAmount=0.0;//Prev mysqlApache.Amount if any.


#ifdef DEBUG
	uHtml=0;
	printf("<p>ProcessClientWebUsageProduct(uClient=%u,"
				"cLabel=%s,uMode=%u)<br>\n",
			uPayment,uClient,cLabel,uMode);
#endif
	GetConfiguration("cuWebsiteUsageProduct",cuWebsiteUsageProduct,uHtml);
	sscanf(cuWebsiteUsageProduct,"%u",&uProduct);
	if(!uProduct)
#ifndef DEBUG
		tInvoice("cuWebsiteUsageProduct not set");
#else
	{
		printf("cuWebsiteUsageProduct not set\n");
		Footer_ism3();
	}
#endif

	time(&luClock);

	//New. When same product of same month/year does not exist.
	sprintf(gcQuery,"SELECT uInstance FROM tInstance WHERE cLabel='%s %s'"
		" AND EXTRACT(MONTH FROM FROM_UNIXTIME(uCreatedDate))="
		"EXTRACT(MONTH FROM NOW()) AND EXTRACT(YEAR FROM "
		"FROM_UNIXTIME(uCreatedDate))=EXTRACT(YEAR FROM NOW())"
		" AND uClient=%u AND uProduct=%u",
			structWebUsage->cDomain,cLabel,uClient,uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
#ifndef DEBUG
		tInvoice(mysql_error(&gMysql));
#else
	{
		printf("%s\n",mysql_error(&gMysql));
		Footer_ism3();
	}
#endif
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uInstance);
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tInstance SET"
			" uClient=%u,uProduct=%u,uOwner=%u,uCreatedBy=%u,"
			"uCreatedDate=%lu,cLabel='%s %s',uStatus=%u,uBilled=%lu",
					uClient,
					uProduct,
					uClient,
					guLoginClient,
					luClock,
					structWebUsage->cDomain,cLabel,
					unxsISP_Waiting,
					luClock);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
#ifndef DEBUG
			tInvoice(mysql_error(&gMysql));
#else
		{
			printf("%s\n",mysql_error(&gMysql));
			Footer_ism3();
		}
#endif

		uGroup=mysql_insert_id(&gMysql);

		sprintf(gcQuery,"INSERT INTO tClientConfig (uParameter,cValue,"
			"uGroup,uService,uOwner,uCreatedBy,uCreatedDate) SELECT"
			" tConfig.uParameter,tConfig.cValue,%u,uGroup,%u,%u,%lu FROM"
			" tConfig,tService,tServiceGlue WHERE tConfig.uGroup="
			"tService.uService AND tServiceGlue.uService=tService.uService AND"
			" tServiceGlue.uServiceGroup=%u",
					uGroup,
					uClient,
					guLoginClient,
					luClock,
					uProduct);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
#ifndef DEBUG
		tInvoice(mysql_error(&gMysql));
#else
		{
			printf("%s\n",mysql_error(&gMysql));
			Footer_ism3();
		}
#endif
	}
	mysql_free_result(res);

	switch(uMode)
	{
		case 0:
			dTotal=structWebUsage->dHDTotal;
		break;
		case 1:
			dTotal=structWebUsage->dMailTotal;
		break;
		case 2:
			dTotal=structWebUsage->dTrafficTotal;
		break;
		case 3:
			dTotal=structWebUsage->dFTPTotal;
		break;
	}

	sprintf(gcQuery,"SELECT uParameter FROM tParameter WHERE cParameter='mysqlApache.Amount'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
#ifndef DEBUG
		tInvoice(mysql_error(&gMysql));
#else
		{
			printf("%s\n",mysql_error(&gMysql));
			Footer_ism3();
		}
#endif
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uParameter);
	mysql_free_result(res);
	if(!uParameter)
#ifndef DEBUG
		tInvoice("tParameter mysqlApache.Amount not configured");
#else
		{
			printf("tParameter mysqlApache.Amount not configured");
			Footer_ism3();
		}
#endif

	//Get old value if !uGroup
	if(!uGroup)
	{
		sprintf(gcQuery,"SELECT cValue FROM tClientConfig WHERE"
				" uGroup=%u AND uParameter=%u",
					uInstance,
					uParameter);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
#ifndef DEBUG
			tInvoice(mysql_error(&gMysql));
#else
		{
			printf("%s\n",mysql_error(&gMysql));
			Footer_ism3();
		}
#endif
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%lf",&dAmount);
		else
#ifndef DEBUG
			tInvoice("tClientConfig mysqlApache.Amount not configured");
#else
		{
			printf("tClientConfig mysqlApache.Amount not configured");
			Footer_ism3();
		}
#endif
		mysql_free_result(res);
	}
	dAmountChanged = dTotal - dAmount;
#ifdef DEBUG
	printf("dAmountChanged = dTotal - dAmount/%lf = %lf - %lf<br>\n",
			dAmountChanged,dTotal,dAmount);
#endif

	if(!uInstance) uInstance=uGroup;

	//Update parameter values and instance billed date
	sprintf(gcQuery,"UPDATE tClientConfig SET cValue='%2.2lf',uModDate=%lu,uModBy=%u WHERE"
			" uGroup=%u AND uParameter=%u",
				dTotal,
				luClock,
				guLoginClient,
				uInstance,
				uParameter);
#ifdef DEBUG
	printf("%s<br>\n",gcQuery);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
#ifndef DEBUG
		tInvoice(mysql_error(&gMysql));
#else
		{
			printf("%s\n",mysql_error(&gMysql));
			Footer_ism3();
		}
#endif

	if(!uGroup)
	{
		sprintf(gcQuery,"UPDATE tInstance SET uBilled=%lu,uModDate=%lu,uModBy=%u WHERE uInstance=%u",
				luClock,
				luClock,
				guLoginClient,
				uInstance);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
#ifndef DEBUG
			tInvoice(mysql_error(&gMysql));
#else
			{
				printf("%s\n",mysql_error(&gMysql));
				Footer_ism3();
			}
#endif
	}


	sprintf(gcQuery,"SELECT uParameter FROM tParameter WHERE cParameter='mysqlApache.Domain'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
#ifndef DEBUG
		tInvoice(mysql_error(&gMysql));
#else
		{
			printf("%s\n",mysql_error(&gMysql));
			Footer_ism3();
		}
#endif
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uParameter);
	mysql_free_result(res);
	if(!uParameter)
#ifndef DEBUG
		tInvoice("tParameter mysqlApache.Domain not configured");
#else
		{
			printf("tParameter mysqlApache.Domain not configured");
			Footer_ism3();
		}
#endif
	//Update parameter 
	sprintf(gcQuery,"UPDATE tClientConfig SET cValue='%s',uModDate=%lu,uModBy=%u WHERE"
			" uGroup=%u AND uParameter=%u",
				structWebUsage->cDomain,
				luClock,
				guLoginClient,
				uInstance,
				uParameter);
#ifdef DEBUG
	printf("%s<br>\n",gcQuery);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
#ifndef DEBUG
		tInvoice(mysql_error(&gMysql));
#else
		{
			printf("%s\n",mysql_error(&gMysql));
			Footer_ism3();
		}
#endif



}//void ProcessClientWebUsageProduct()


unsigned GenerateInvoicesCommand(unsigned uHtml)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	time_t luLastPaid;
	time_t luCreatedDate;
	char cluLastPaid[32];
	char cluCreatedDate[32];

	unsigned uOnlyOnce=1;//For Now sscanf
	unsigned uMonth=0;//Now
	unsigned uYear=0;
	unsigned uClientPeriod;
	unsigned uProductPeriod;
	unsigned uLastPaidMonth;//tInstance
	unsigned uLastPaidYear;
	unsigned uCreatedDateMonth;
	unsigned uCreatedDateYear;
	unsigned uPriceMult;
	unsigned uCase=0;

	unsigned uClient;
	unsigned uLastClient=0;//Like NULL for us
	unsigned uNewInvoice;//New invoice should be created below if >0
	unsigned uNewInvoiceNum=0;//The new tInvoice.uInvoice
	double dTotal=0.0;

	char cuPermCreditProduct[16]={"0"};
	char cuTempCreditProduct[16]={"0"};

	if(!uHtml)
	{
		printf("GenerateInvoicesCommand()\n\n");
		TextConnectDb();
	}

	GetConfiguration("cuTempCreditProduct",cuTempCreditProduct,uHtml);
	GetConfiguration("cuPermCreditProduct",cuPermCreditProduct,uHtml);

	//Outside while loop: For getting for all cases possibly valid uInstances and 
	//tProduct and tClient based tPeriod data for different cases that will require different inner while loop selects.

	//1-. Create while loop of valid tClient tInstance tProducts
	//Everything should be apparent. Hard part is the period and date stuff.
	sprintf(gcQuery,"SELECT tInstance.uInstance,tProductPeriod.cLabel,tClientPeriod.cLabel,tClient.uClient,"
			"tClient.cLabel,tProduct.cLabel,tInstance.cLabel,tInstance.uLastPaid,tInstance.uCreatedDate,"
			"tProductPeriod.uPeriod,tClientPeriod.uPeriod,EXTRACT(MONTH FROM FROM_UNIXTIME(tInstance.uLastPaid)),"
			"EXTRACT(YEAR FROM FROM_UNIXTIME(tInstance.uLastPaid)),EXTRACT(MONTH FROM FROM_UNIXTIME(tInstance.uCreatedDate)),"
			"EXTRACT(YEAR FROM FROM_UNIXTIME(tInstance.uCreatedDate)),EXTRACT(MONTH FROM NOW()),EXTRACT(YEAR FROM NOW()),"
			"tProduct.mPrice,tProduct.mSetupFee,tClient.mTempCredit,tClient.mPermCredit,tProduct.uProduct,tProduct.uProductType "
			"FROM tInstance,tProduct,tClient,tPeriod AS tProductPeriod,tPeriod AS tClientPeriod,tPayment AS tClientPayment "
			"WHERE tProductPeriod.uPeriod=tProduct.uPeriod AND tClientPeriod.uPeriod=tClientPayment.uPeriod AND "
			"tClient.uPayment=tClientPayment.uPayment AND tProduct.uProduct=tInstance.uProduct AND "
			"tInstance.uClient=tClient.uClient AND tInstance.uBilled=0 AND ( tInstance.uStatus=%u OR "
			"tInstance.uStatus=%u OR tInstance.uStatus=%u OR tInstance.uStatus=%u OR tInstance.uStatus=%u ) ORDER BY tClient.uClient",
				unxsISP_Deployed,
				unxsISP_DeployedMod,
				unxsISP_OnHold,
				unxsISP_WaitingRedeployment,
				unxsISP_WaitingHold);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(!uHtml)
			printf("%s\n",mysql_error(&gMysql));
		else
			tInvoice(mysql_error(&gMysql));
		exit(1);
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		luLastPaid=0;
		luCreatedDate=0;
		uLastPaidYear=0;
		uLastPaidMonth=0;
		uProductPeriod=0;
		uClientPeriod=0;
		uCreatedDateMonth=0;
		uCreatedDateYear=0;
		strcpy(cluLastPaid,"---\n");
		strcpy(cluCreatedDate,"---\n");

	
		sscanf(field[3],"%u",&uClient);
		if(uClient!=uLastClient)
		{
			uNewInvoice=1;
			if(!uHtml)
				printf("\n\nNew Invoice: %s\n\n",field[4]);
		}

		sscanf(field[7],"%lu",&luLastPaid);
		sscanf(field[8],"%lu",&luCreatedDate);

		sscanf(field[9],"%u",&uProductPeriod);
		sscanf(field[10],"%u",&uClientPeriod);

		if(luLastPaid)
		{
			sscanf(field[11],"%u",&uLastPaidMonth);
			sscanf(field[12],"%u",&uLastPaidYear);
		}

		if(luCreatedDate)
		{
			sscanf(field[13],"%u",&uCreatedDateMonth);
			sscanf(field[14],"%u",&uCreatedDateYear);
		}

		if(uOnlyOnce)
		{
			sscanf(field[15],"%u",&uMonth);
			sscanf(field[16],"%u",&uYear);
		}

		if(luCreatedDate)
			strcpy(cluCreatedDate,ctime(&luCreatedDate));
		if(luLastPaid)
			strcpy(cluLastPaid,ctime(&luLastPaid));

#ifdef DEBUG
		if(!uHtml)
		printf("tClient.cLabel=%s\ntProduct.cLabel=%s\ntInstance.cLabel=%s\n"
			"tClient.uClient=%s tInstance.uInstance=%s\n"
			"tInstance.uLastPaid=%stInstance.uCreatedDate=%s"
			"tProductPeriod.cLabel=%s tClientPeriod.cLabel=%s\n"
			"uProductPeriod=%u uClientPeriod=%u\n"
			"tInstance\nuLastPaidMonth=%u uLastPaidYear=%u\n"
			"uCreatedDateMonth=%u uCreatedDateYear=%u\n"
			"Now\nuMonth=%u uYear=%u\n\n",
				field[4],
				field[5],
				field[6],
				field[3],
				field[0],
				cluLastPaid,
				cluCreatedDate,
				field[1],
				field[2],
				uProductPeriod,
				uClientPeriod,
				uLastPaidMonth,
				uLastPaidYear,
				uCreatedDateMonth,
				uCreatedDateYear,
				uMonth,
				uYear);
#endif

		if(uHtml) guDebug=0;
		uPriceMult=uCaseAnalyzer(uProductPeriod,uClientPeriod,
					uLastPaidMonth,uLastPaidYear,
					uCreatedDateMonth,uCreatedDateYear,
					uMonth,uYear,&uCase);
	
		if(uPriceMult)
		{	
			time_t luClock;

			if(!uHtml)
			printf("\tuPriceMult=%u for %s(%u) (Case=%u)\n",
					uPriceMult,field[4],uClient,uCase);
			if(uNewInvoice)
			{
				if(uNewInvoiceNum)
				{
					//This section is repeated similarly when leaving while
					//loop. See below...

					//Close prev invoice update tInvoice and tClient 
					//with total etc.
					if(!uHtml)
					printf("\tUpdating prev tInvoice=%u mTotal=%2.2lf\n",
									uNewInvoiceNum,dTotal);
					sprintf(gcQuery,"UPDATE tInvoice SET mTotal=%2.2f WHERE uInvoice=%u",dTotal,uNewInvoiceNum);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						if(!uHtml)
							printf("%s\n",mysql_error(&gMysql));
						else
							tInvoice(mysql_error(&gMysql));
						exit(1);
					}

				
					//This means we must adjust if we remove invoices!
					//We also must adjust when applying payments.

					if(!uHtml)
					printf("\tUpdating prev uClient=%u mBalance\n",uLastClient);

					sprintf(gcQuery,"UPDATE tClient SET mBalance=mBalance+%2.2lf WHERE uClient=%u",dTotal,uLastClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						if(!uHtml)
							printf("%s\n",mysql_error(&gMysql));
						else
							tInvoice(mysql_error(&gMysql));
						exit(1);
					}

				}

				uNewInvoiceNum=CreateNewInvoice(uClient,uHtml);
				dTotal=0.0;
				uNewInvoice=0;
				uLastClient=uClient;
			}

			if(uNewInvoiceNum)
			{
				structWebsiteUsage structWebUsage;
				double dLineTotal;
				double dItemPrice;
				double dHours;
				double dSetupFee;
				double dTempCredit;
				double dPermCredit;
				unsigned uProductType;
				unsigned uInstance;
				register unsigned uOnlyOneTempCreditPerClient;
				register unsigned uOnlyOnePermCreditPerClient;
				char cShortUnits[13]={"Hr"};
				char cLongUnits[33]={"Hours"};
				char cInvoiceItemLabel[512];


				uOnlyOneTempCreditPerClient=1;
				uOnlyOnePermCreditPerClient=1;

				if(!uHtml)
				{
					printf("\tAdding Item To Invoice %u\n",uNewInvoiceNum);
					printf("\n");
					printf("\t(start) dTotal=%2.2lf\n",dTotal);
				}

				sscanf(field[17],"%lf",&dItemPrice);
				sscanf(field[18],"%lf",&dSetupFee);
				sscanf(field[19],"%lf",&dTempCredit);
				sscanf(field[20],"%lf",&dPermCredit);
				sscanf(field[22],"%u",&uProductType);
				sscanf(field[0],"%u",&uInstance);

				if( uProductType==unxsISP_BillingOnly && 
					( uProductPeriod==unxsISP_BillingHourly ||
						uProductPeriod==unxsISP_BillingTraffic ) )
				{
					//Used for both hours and traffic. GB or MB? Does it
					//matter?
					char mHours[16]={"1.0"};

					if(uProductPeriod==unxsISP_BillingHourly)
						GetBillingHours(uInstance,mHours);
					else
						GetBillingTraffic(uInstance,mHours);
					sscanf(mHours,"%lf",&dHours);
					dLineTotal=(dItemPrice*dHours);
					GetBillingUnits(uInstance,cShortUnits,cLongUnits);
				}
				else
				{
					//Only use multiple period adustment for
					//normal products?
					//TODO
					dLineTotal=(dItemPrice * uPriceMult) ;
				}
				if(!uHtml)
					printf("\t1-. dLineTotal=%2.2lf\n",dLineTotal);
/*
0  tInstance.uInstance
1  tProductPeriod.cLabel
2  tClientPeriod.cLabel
3  tClient.uClient
4  tClient.cLabel
5  tProduct.cLabel
6  tInstance.cLabel
7  tInstance.uLastPaid
8  tInstance.uCreatedDate
9  tProductPeriod.uPeriod
10 tClientPeriod.uPeriod
11 EXTRACT(MONTH FROM FROM_UNIXTIME(tInstance.uLastPaid))
12 EXTRACT(YEAR FROM FROM_UNIXTIME(tInstance.uLastPaid))
13 EXTRACT(MONTH FROM FROM_UNIXTIME(tInstance.uCreatedDate))
14 EXTRACT(YEAR FROM FROM_UNIXTIME(tInstance.uCreatedDate))
15 EXTRACT(MONTH FROM NOW())
16 EXTRACT(YEAR FROM NOW())
17 tProduct.mPrice
18 tProduct.mSetupFee
19 tClient.mTempCredit
20 tClient.mPermCredit
21 tProduct.uProduct
22 tProduct.uProductType

Derived:
uProductPeriod
uClientPeriod
uLastPaidMonth
uLastPaidYear
uCreatedDateMonth
uCreatedDateYear
uMonth
uYear
dItemPrice 17
dSetupFee 18
dTempCredit 19
dPermCredit 20
uProductType 22
uInstance 0 

//Product related
//Tied to tProductType
#define unxsISP_BillingOnly 5
//Tied to tPeriod
#define unxsISP_BillingHourly 7
#define unxsISP_BillingStorage 9
#define unxsISP_BillingTraffic 10


*/

		
				if(uProductPeriod==unxsISP_BillingHourly ||
					uProductPeriod==unxsISP_BillingTraffic )
					sprintf(cInvoiceItemLabel,"%.255s. %2.2lf %.32s at $%.12s/%.12s. %s i"
							"billing-only product. Created %u/%u. Last payment %u/%u.",
						field[6],
						dHours,
						cLongUnits,
						field[17],
						cShortUnits,
						field[1],
						uCreatedDateMonth,
						uCreatedDateYear,
						uLastPaidMonth,
						uLastPaidYear);
				else
					sprintf(cInvoiceItemLabel,"%.255s ($%2.2lf x %u period units). %s product. Created %u/%u. Last payment %u/%u.",
							field[6],dItemPrice,uPriceMult,field[1],uCreatedDateMonth,
							uCreatedDateYear,uLastPaidMonth,uLastPaidYear);

				if(!uHtml)
				printf("\tItem %s (%s) (dLineTotal:%.2lf mSetupFee:%.2lf mTempCredit:%.2lf mPermCredit:%.2lf)\n",
						cInvoiceItemLabel,field[2],dLineTotal,dSetupFee,dTempCredit,dPermCredit);

				//Add normal invoice items here. 
				//Including hourly and special traffic based
				sprintf(gcQuery,"INSERT INTO tInvoiceItems SET uQuantity=1,uInvoice=%u,uClient=%s,cLabel='%s',"
						"uInstance=%u,uProduct=%s,mPrice=%2.2lf,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
					uNewInvoiceNum,
					field[3],
					cInvoiceItemLabel,
					uInstance,
					field[21],
					dLineTotal,
					guLoginClient,
					guLoginClient,
					luClock);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						if(!uHtml)
						{
							printf("%s\n",gcQuery);
							printf("%s\n",mysql_error(&gMysql));
						}
						else
						{
							tInvoice(mysql_error(&gMysql));
						}
						exit(1);
					}



				//Add auto website usage product items?
				structWebUsage.uInstance=uInstance;
				if(WebSiteUsage(&structWebUsage,0))
				{

					char cTrunc[16]={""};

					#ifdef DEBUG_USAGE
						if(!uHtml)
						PrintWebSiteUsage(&structWebUsage,0);
					#endif

					if(structWebUsage.dHDTotal>0.0)
					{
						sprintf(gcQuery,"INSERT INTO tInvoiceItems SET uQuantity=1,"
								"cLabel='Excess %s storage: %2.2lfMB. Allowed "
								"%2.2lfMB. Excess %2.2lfMB at $%2.2lf/MB. "
								"Per incident flat fee: $%2.2lf.',uInvoice=%u,"
								"uClient=%s,uProduct=%s," "mPrice=%2.2lf,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
						structWebUsage.cDomain,
						structWebUsage.dHD,
						structWebUsage.dHDQuota,
						structWebUsage.dHD-structWebUsage.dHDQuota,
						structWebUsage.dHDPrice,
						structWebUsage.dHDSetup,
						uNewInvoiceNum,
						field[3],//uClient
						field[21],//uProduct
						structWebUsage.dHDTotal,//mPrice
						guCompany,
						guLoginClient,
						luClock);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							if(!uHtml)
								printf("%s\n",mysql_error(&gMysql));
							else
								tInvoice(mysql_error(&gMysql));
							exit(1);
						}
						sprintf(cTrunc,"%2.2lf",structWebUsage.dHDTotal);
						sscanf(cTrunc,"%lf",&structWebUsage.dHDTotal);
						dLineTotal+=structWebUsage.dHDTotal;
						if(!uHtml)
							printf("\t2-. dLineTotal=%2.2lf\n",dLineTotal);

						//Add tClient product instance if needed
						//0=HD,1=Mail,2=Traffic and 3=FTP -->TODO defines
						sscanf(field[3],"%u",&uClient);
						ProcessClientWebUsageProduct(&structWebUsage,uClient,
							 "Excess HDStorage",0);
					}//structWebUsage.dHDTotal

					if(structWebUsage.dMailTotal>0.0)
					{
						sprintf(gcQuery,"INSERT INTO tInvoiceItems SET uQuantity=1,"
								"cLabel='Excess %s mail storage: %2.2lfMB. Allowed %2.2lfMB. "
								"Excess %2.2lfMB at $%2.2lf/MB. Per incident flat fee: $%2.2lf.'"
								",uInvoice=%u,uClient=%s,uProduct=%s,mPrice=%2.2lf,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
						structWebUsage.cDomain,
						structWebUsage.dMail,
						structWebUsage.dMailQuota,
						structWebUsage.dMail-structWebUsage.dMailQuota,
						structWebUsage.dMailPrice,
						structWebUsage.dMailSetup,
						uNewInvoiceNum,
						field[3],
						field[21],//uProduct
						structWebUsage.dMailTotal,//mPrice
						guCompany,
						guLoginClient,
						luClock);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							if(!uHtml)
								printf("%s\n",mysql_error(&gMysql));
							else
								tInvoice(mysql_error(&gMysql));
							exit(1);
						}
						sprintf(cTrunc,"%2.2lf",structWebUsage.dMailTotal);
						sscanf(cTrunc,"%lf",&structWebUsage.dMailTotal);
						dLineTotal+=structWebUsage.dMailTotal;
						if(!uHtml)
							printf("\t3-. dLineTotal=%2.2lf\n",dLineTotal);

						sscanf(field[3],"%u",&uClient);
						ProcessClientWebUsageProduct(&structWebUsage,uClient,
								"Excess MailStorage",1);
					}//structWebUsage.dMailTotal

					if(structWebUsage.dTrafficTotal>0.0)
					{
						sprintf(gcQuery,"INSERT INTO tInvoiceItems SET uQuantity=1,"
								"cLabel='Excess %s HTTP traffic: %2.2lfMB. "
								"Allowed %2.2lfMB. Excess %2.2lfMB at $%2.2lf/GB. "
								"Per incident flat fee: $%2.2lf.',uInvoice=%u,"
								"uClient=%s,uProduct=%s,mPrice=%2.2lf,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
							structWebUsage.cDomain,
							structWebUsage.dTraffic,
							structWebUsage.dTrafficQuota,
							structWebUsage.dTraffic-structWebUsage.dTrafficQuota,
							structWebUsage.dTrafficPrice,//Note non standard divisor
							structWebUsage.dTrafficSetup,
							uNewInvoiceNum,
							field[3],
							field[21],//uProduct
							structWebUsage.dTrafficTotal,//mPrice
							guCompany,
							guLoginClient,
							luClock);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							if(!uHtml)
								printf("%s\n",mysql_error(&gMysql));
							else
								tInvoice(mysql_error(&gMysql));
							exit(1);
						}
						sprintf(cTrunc,"%2.2lf",structWebUsage.dTrafficTotal);
						sscanf(cTrunc,"%lf",&structWebUsage.dTrafficTotal);
						dLineTotal+=structWebUsage.dTrafficTotal;
						if(!uHtml)
							printf("\t4-. dLineTotal=%2.2lf\n",dLineTotal);

						sscanf(field[3],"%u",&uClient);
						ProcessClientWebUsageProduct(&structWebUsage,uClient,
							"Excess HTTPTraffic",2);

					}//structWebUsage.dTrafficTotal>0.0

					if(structWebUsage.dFTPTotal>0.0)
					{
						sprintf(gcQuery,"INSERT INTO tInvoiceItems SET uQuantity=1,"
								"cLabel='Excess %s FTP traffic:  %2.2lfMB. "
								"Allowed %2.2lfMB. Excess %2.2lfMB at $%2.2lf/GB. "
								"Per incident flat fee: $%2.2lf.',"
								"uInvoice=%u,uClient=%s,uProduct=%s,mPrice=%2.2lf,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
								structWebUsage.cDomain,
								structWebUsage.dFTP,
								structWebUsage.dFTPQuota,
								structWebUsage.dFTP-structWebUsage.dFTPQuota,
								structWebUsage.dFTPPrice,//Note non standard divisor
								structWebUsage.dFTPSetup,
								uNewInvoiceNum,
								field[3],
								field[21],//uProduct
								structWebUsage.dFTPTotal,//mPrice+mSetup
								guCompany,
								guLoginClient,
								luClock);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							if(!uHtml)
								printf("%s\n",mysql_error(&gMysql));
							else
								tInvoice(mysql_error(&gMysql));
							exit(1);
						}
							sprintf(cTrunc,"%2.2lf",structWebUsage.dFTPTotal);
							sscanf(cTrunc,"%lf",&structWebUsage.dFTPTotal);
							dLineTotal+=structWebUsage.dFTPTotal;
						if(!uHtml)
							printf("\t5-. dLineTotal=%2.2lf\n",dLineTotal);

						sscanf(field[3],"%u",&uClient);
						ProcessClientWebUsageProduct(&structWebUsage,
							uClient,"Excess FTPTraffic",3);
					}//structWebUsage.dFTPTotal>0.0

				}//WebSiteUsage



				//If already paid no setup fee again...duh
				if(dSetupFee!=0 && !uLastPaidYear)
				{
					sprintf(gcQuery,"INSERT INTO tInvoiceItems SET uQuantity=1,"
							"cLabel='New product setup fee: %s',uInvoice=%u,"
							"uClient=%s,uProduct=%s,mPrice=%s,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
					field[6],
					uNewInvoiceNum,
					field[3],
					field[21],
					field[18],//dSetupFee
					guCompany,
					guLoginClient,
					luClock);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						if(!uHtml)
							printf("%s\n",mysql_error(&gMysql));
						else
							tInvoice(mysql_error(&gMysql));
						exit(1);
					}
					dLineTotal+=dSetupFee;
					if(!uHtml)
					printf("\t6-. dLineTotal=%2.2lf\n",dLineTotal);
				}//dSetupFee!=0

				if(dTempCredit!=0 && uOnlyOneTempCreditPerClient)
				{
					uOnlyOneTempCreditPerClient=0;

					sprintf(gcQuery,"INSERT INTO tInvoiceItems SET uQuantity=1,"
							"cLabel='One time client credit',uInvoice=%u,"
							"uClient=%s,uProduct=%s,mPrice=-%s,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
					uNewInvoiceNum,
					field[3],
					cuTempCreditProduct,
					field[19],//dTempCredit
					guCompany,
					guLoginClient,
					luClock);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						if(!uHtml)
							printf("%s\n",mysql_error(&gMysql));
						else
							tInvoice(mysql_error(&gMysql));
						exit(1);
					}
					dLineTotal-=dTempCredit;
					if(!uHtml)
					printf("\t7-. dLineTotal=%2.2lf\n",dLineTotal);
				}

				if(dPermCredit!=0 && uOnlyOnePermCreditPerClient)
				{
					uOnlyOnePermCreditPerClient=0;

					sprintf(gcQuery,"INSERT INTO tInvoiceItems SET uQuantity=1,"
							"cLabel='Client permanent credit',uInvoice=%u,"
							"uClient=%s,uProduct=%s,mPrice=-%s,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
					uNewInvoiceNum,
					field[3],
					cuPermCreditProduct,
					field[20],
					guCompany,
					guLoginClient,
					luClock);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						if(!uHtml)
							printf("%s\n",mysql_error(&gMysql));
						else
							tInvoice(mysql_error(&gMysql));
						exit(1);
					}
					dLineTotal-=dPermCredit;
					if(!uHtml)
					printf("\t8-. dLineTotal=%2.2lf\n",dLineTotal);
				}

				dTotal+=dLineTotal;
				if(!uHtml)
					printf("\t(end) dTotal=%2.2lf\n",dTotal);

				//After everything is done ok
				if(!uHtml)
					printf("\tUpdating uInstance=%s\n",field[0]);
				time(&luClock);
				sprintf(gcQuery,"UPDATE tInstance SET uBilled=%lu WHERE uInstance=%s",
					luClock,field[0]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					if(!uHtml)
						printf("%s\n",mysql_error(&gMysql));
					else
						tInvoice(mysql_error(&gMysql));
					exit(1);
				}
				if(!uHtml) printf("\n");

			}//uNewInvoiceNum


		}//uPriceMult>0

		//debug only: only one tClient
		//exit(0);

		uOnlyOnce=0;//Should be last thing
	}//Outer while

	//Handle last case
	if(uNewInvoiceNum && dTotal>0.0)
	{
		//Close invoice update with total etc.
		if(!uHtml)
		printf("\tUpdating last tInvoice=%u mTotal=%2.2lf\n",
					uNewInvoiceNum,dTotal);
		sprintf(gcQuery,"UPDATE tInvoice SET mTotal=%2.2f WHERE uInvoice=%u",
					dTotal,
					uNewInvoiceNum);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			if(!uHtml)
				printf("%s\n",mysql_error(&gMysql));
			else
				tInvoice(mysql_error(&gMysql));
			exit(1);
		}

		if(!uHtml)
			printf("\tUpdating uClient=%u mBalance\n",uClient);
		sprintf(gcQuery,"UPDATE tClient SET mBalance=mBalance+%2.2lf WHERE uClient=%u",
								dTotal,uClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			if(!uHtml)
				printf("%s\n",mysql_error(&gMysql));
			else
				tInvoice(mysql_error(&gMysql));
			exit(1);
		}
	}

	mysql_free_result(res);

	if(!uHtml)
		exit(0);
	else if(uNewInvoiceNum)
		return(0);
	return(1);

}//void GenerateInvoicesCommand(void)


unsigned uQuarterOf(unsigned uMonth)
{
	unsigned uQuarter=4;

	if(uMonth<4)
		uQuarter=1;
	else if(uMonth<7)
		uQuarter=2;
	else if(uMonth<10)
		uQuarter=3;

	return(uQuarter);

}//unsigned uQuarterOf(unsigned uMonth)


unsigned uSemesterOf(unsigned uMonth)
{
	unsigned uSemester=2;

	if(uMonth<7)
		uSemester=1;

	return(uSemester);

}//unsigned uSemesterOf(unsigned uMonth)


//Returns uPriceMult if 0 do not invoice current tInstance item
unsigned uCaseAnalyzer(unsigned const uProductPeriod, unsigned const uClientPeriod,
				unsigned const uLastPaidMonth, unsigned const uLastPaidYear,
				unsigned const uCreatedDateMonth, unsigned const uCreatedDateYear,
				unsigned const uMonth, unsigned const uYear, unsigned *uCase)
{
	unsigned uPriceMult=1;

	unsigned uValidCase=0;


	//Basic error conditions: bail
	if(uLastPaidYear>uYear || uCreatedDateYear>uYear || uCreatedDateYear<1970
		|| uCreatedDateMonth<1 || uCreatedDateMonth>12
		|| (uLastPaidYear==uYear && uLastPaidMonth>uMonth) )
	{
		if(guDebug)
			printf("uCaseAnalyzer current date error condition!\n");
		return(0);
	}

	//Determine by corresponding date math the item product price
	//multiplier. If zero do not invoice this tInstance item.
/*
tPeriod table dependency!
Monthly 1
Quarterly 2
Yearly 3
One-time 4
Weekly 5
Daily 6
Hourly 7
Semesterly 8
Storage 9
Traffic 10
*/
	//If valid case then run inner select
	if(uProductPeriod==uClientPeriod && (uClientPeriod<4 || uClientPeriod==8))
		uValidCase=uProductPeriod;//1-3,8

	else if( (uProductPeriod>=4 && uProductPeriod<8) 
			|| (uProductPeriod>=9 && uProductPeriod<10))
		uValidCase=(uProductPeriod*10);//40,50,60,70,90,100
	//One-Time. Also covers Weekly, 
	//Daily, Hourly, Storage and Traffic

	else if(uProductPeriod==1 && uClientPeriod==2)
		uValidCase=5;//Month-Quarter
	else if(uProductPeriod==1 && uClientPeriod==3)
		uValidCase=6;//Month-Year
	else if(uProductPeriod==1 && uClientPeriod==8)
		uValidCase=7;//Month-Semester

	else if(uProductPeriod==2 && uClientPeriod==1)
		uValidCase=80;//Quarter-Month*
	//Missing 9 on purpose
	else if(uProductPeriod==2 && uClientPeriod==3)
		uValidCase=10;//Quarter-Year
	else if(uProductPeriod==2 && uClientPeriod==8)
		uValidCase=11;//Quarter-Semester

	else if(uProductPeriod==3 && uClientPeriod==1)
		uValidCase=12;//Year-Month*
	else if(uProductPeriod==3 && uClientPeriod==2)
		uValidCase=13;//Year-Quarter*
	else if(uProductPeriod==3 && uClientPeriod==8)
		uValidCase=14;//Year-Semester*

	else if(uProductPeriod==8 && uClientPeriod==1)
		uValidCase=15;//Semester-Month*
	else if(uProductPeriod==8 && uClientPeriod==2)
		uValidCase=16;//Semester-Quarter*
	else if(uProductPeriod==8 && uClientPeriod==3)
		uValidCase=17;//Semester-Yearl

	//*These 6 cases are uProductPeriod in months > uClientPeriod in months
	//What does this mean?
	//
	//Ex1: Joe's uClientPeriod is monthly (1) and he has
	//a produt that is yearly (3) (uValidCase==12). We run 
	//invoicing on March 12th 2004 and from the tInstance.uLastPaid
	//we know that he last paid this product on June 14th 2003.
	//So what should the line item on the invoice generated say?
	//It should say the he owes only the current year price.
	//
	//Ex2: Same as Ex1 but uLastPaid=0. Same result.
	//Ex3: Same as Ex1 but uLastPaid=June 14th 2000. 
	//	He owes x4: 2001-2004 (or 2004-2000=4.)
	//
	//Ex4: Same as Ex1 but uClientPeriod is quarterly and uProductPeriod is
	//semesterly.
	//	uLastPaid is Q2/S1 2003 and now is Q1/S1 2004 we need to
	//	know the diff in months between to mon/year tuples
	//	in this case 3,2004 - 6,2003 = 3 + 6  = 9 months then we
	//	9/6 = 1 + if(9 % 6) then +1. Since 9 mod 6 is 3. 2x
	//	== 0 then we dont invoice this line item.
	//
	//Ex5: Rev of Ex1. uProductPeriod=Monthly AND uClientPeriod=Yearly 
	//	He pays once for a whole year. 12x
	//Ex6: Rev of Ex2.
	//	Same result 12x
	//Ex7 Rev of Ex3.
	//	He paid 2000. So He owes 2004-2000 * 12 = 4 * 12 = 48x.

	switch(uValidCase)
	{

		default:
		case(0):
			if(guDebug)
				printf("Unknown uCaseAnalyzer condition=%u!\n",uValidCase);
			return(0);
		break;


		case(1)://Month-Month
		case(5)://Month-Quarter
		case(6)://Month-Year
		case(7)://Month-Semester
			if(uLastPaidMonth)
			{
				if(uYear==uLastPaidYear)
				{
					uPriceMult=uMonth-uLastPaidMonth;
				}
				else
				{
					uPriceMult=(uMonth+(12*(uYear-uLastPaidYear-1)))
							+ (12-uLastPaidMonth);
				}
			}
			else
			{
				if(uYear==uCreatedDateYear)
				{
					uPriceMult=uMonth-uCreatedDateMonth+1;
				}
				else
				{
					uPriceMult=(uMonth+(12*(uYear-uCreatedDateYear)))
							+ (12-uCreatedDateMonth);
				}
			}
		break;


		case(2)://Quarter-Quarter
		case(10)://Quarter-Year
		case(11)://Quarter-Semester
		case(80)://Quarter-Month*
			if(uLastPaidMonth)
			{
				if(uYear==uLastPaidYear)
				{
					uPriceMult=
						uQuarterOf(uMonth)-
						uQuarterOf(uLastPaidMonth);
				}
				else
				{
					uPriceMult=
						(uQuarterOf(uMonth)+
							(4*(uYear-uLastPaidYear-1)))
							+ (4-uQuarterOf(uLastPaidMonth));
				}
			}
			else
			{
				if(uYear==uCreatedDateYear)
				{
					uPriceMult=
						uQuarterOf(uMonth)-
						uQuarterOf(uCreatedDateMonth)+1;
				}
				else
				{
					uPriceMult=
						(uQuarterOf(uMonth)+
							(4*(uYear-uCreatedDateYear)))
							+ (4-uQuarterOf(uCreatedDateMonth));
				}
			}
		break;
		break;


		case(3)://Year-Year
		case(12)://Year-Month*
		case(13)://Year-Quarter*
		case(14)://Year-Semester*
			if(uLastPaidYear)
				uPriceMult=uYear-uLastPaidYear;
			else
				uPriceMult=uYear-uCreatedDateYear+1;
		break;


		case(8)://Semester-Semester
		case(17)://Semester-Year
		case(15)://Semester-Month*
		case(16)://Semester-Quarter*
			if(uLastPaidMonth)
			{
				if(uYear==uLastPaidYear)
				{
					uPriceMult=
						uSemesterOf(uMonth)-
						uSemesterOf(uLastPaidMonth);
				}
				else
				{
					uPriceMult=
						(uSemesterOf(uMonth)+(2*(uYear-uLastPaidYear-1)))
							+ (2-uSemesterOf(uLastPaidMonth));
				}
			}
			else
			{
				if(uYear==uCreatedDateYear)
				{
					uPriceMult=
						uSemesterOf(uMonth)-
						uSemesterOf(uCreatedDateMonth)+1;
				}
				else
				{
					uPriceMult=
						(uSemesterOf(uMonth)+(2*(uYear-uCreatedDateYear)))
							+ (2-uSemesterOf(uCreatedDateMonth));
				}
			}
		break;


		case(40)://One-time
		case(50)://Weekly
		case(60)://Daily
		case(70)://Hourly
		case(90)://Storage
		case(100)://Traffic
			uPriceMult=1;
		break;
	}

	*uCase=uValidCase;
	return(uPriceMult);

}//unsigned uCaseAnalyzer()



unsigned CreateNewInvoice(unsigned uClient, unsigned uHtml)
{
	time_t luClock;
	unsigned uNewInvoiceNum=0;

	if(!uClient) return(0);

	if(!uHtml)
		printf("\tCreateNewInvoice(%u)\n",uClient);

	time(&luClock);

	sprintf(gcQuery,"INSERT tInvoice (uInvoiceStatus,uOwner,uCreatedBy,uCreatedDate,cFirstName,cLastName,cEmail,cAddr1,"
			"cAddr2,cCity,cState,cZip,cCountry,cCardType,cCardNumber,uExpMonth,uExpYear,cCardName,cShipName,"
			"cShipAddr1,cShipAddr2,cShipCity,cShipState,cShipZip,cShipCountry,cTelephone,cFax,uClient,uPayment) "
			"SELECT %u,%u,%u,%lu,cFirstName,cLastName,cEmail,cAddr1,cAddr2,cCity,cState,cZip,cCountry,cCardType,"
			"cCardNumber,uExpMonth,uExpYear,cCardName,cShipName,cShipAddr1,cShipAddr2,cShipCity,cShipState,"
			"cShipZip,cShipCountry,cTelephone,cFax,uClient,uPayment FROM tClient WHERE uClient=%u",
					unxsISP_NewInvoice,
					guCompany,
					guLoginClient,
					luClock,
					uClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(uHtml)
			tInvoice(mysql_error(&gMysql));
		else
			printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	uNewInvoiceNum=mysql_insert_id(&gMysql);


	return(uNewInvoiceNum);

}//unsigned CreateNewInvoice(unsigned uClient)



// vimoff:twoff=78


/*
	//Review this whole section in context. Seems completely wrong!
	//Update tClient.mBalance only if new or if mysqlApache.Amount has changed.
	if(uGroup || dAmountChanged!=0.0)
	{
		char mBalance[16]={"0.0"};

		sprintf(gcQuery,"SELECT mBalance FROM tClient WHERE"
				" uClient=%u",uClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
#ifndef DEBUG
			tInvoice(mysql_error(&gMysql));
#else
		{
			printf("%s\n",mysql_error(&gMysql));
			Footer_ism3();
		}
#endif
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%lf",&dBalance);
		}
		else
		{
#ifndef DEBUG
			tInvoice("tClient not found!");
#else
			printf("tClient not found!");
			Footer_ism3();
#endif
		}
		mysql_free_result(res);

		//This is a (charge)
		sprintf(mBalance,"%2.2lf",dBalance-dAmountChanged);
		sprintf(gcQuery,"UPDATE tClient SET mBalance=%s,"
			"uModDate=%lu,uModBy=%u WHERE uClient=%u",
				mBalance,luClock,guLoginClient,uClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
#ifndef DEBUG
			tInvoice(mysql_error(&gMysql));
#else
		{
			printf("%s\n",mysql_error(&gMysql));
			Footer_ism3();
		}
#endif

	}//Update tClient.mBalance only if new, or if mysqlApache.Amount has changed.

*/
void funcInvoice(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	struct t_template template;
	char cCardExp[42]={""};
	char cInvoice[32]={""};
	char cLanguage[32]={""};
	char cTemplateName[100]={""};

	char cPaymentName[31]={"Unknown"};
	unsigned uRequireCreditCard=0;

	template.cpName[0]="";

	if(!uInvoice || !uClient)
	{
		fprintf(fp,"No invoice loaded.");
		return;
	}

	uRequireCreditCard=GetPaymentValue(uInvoice,"","");
	sprintf(cInvoice,"%u-%u",uClient,uInvoice);

	res=sqlresultClientInfo();

	if((field=mysql_fetch_row(res)))
	{
		char cCardNumber[20]={"Error"};
		unsigned uCardNumberLen=0;
		
		sprintf(cLanguage,"%.3s",field[23]);

		template.cpName[0]="cLabel";
		template.cpValue[0]=field[0];
		
		template.cpName[1]="cLastName";
		template.cpValue[1]=field[1];

		template.cpName[2]="cEmail";
		template.cpValue[2]=field[2];

		template.cpName[3]="cAddr1";
		template.cpValue[3]=field[3];

		template.cpName[4]="cAddr2";
		template.cpValue[4]=field[4];

		template.cpName[5]="cCity";
		template.cpValue[5]=field[5];

		template.cpName[6]="cState";
		template.cpValue[6]=field[6];

		template.cpName[7]="cZip";
		template.cpValue[7]=field[7];

		template.cpName[8]="cCountry";
		template.cpValue[8]=field[8];

		template.cpName[9]="cCardType";
		template.cpValue[9]=field[9];//see below

		template.cpName[10]="cCardNumber";
		uCardNumberLen=strlen(field[10]);
		if(field[9][0]=='A' && uCardNumberLen>14)
			sprintf(cCardNumber,"XXXXXXXXXXX%.4s",field[10]+11);
		else if(uCardNumberLen>15)
			sprintf(cCardNumber,"XXXXXXXXXXXX%.4s",field[10]+12);
		template.cpValue[10]=cCardNumber;

		template.cpName[11]="cCardName";
		if(uRequireCreditCard)
		{
			template.cpValue[11]=field[11];
		}
		else
		{
			GetPaymentValue(uInvoice,"cLabel",cPaymentName);
			template.cpValue[11]=cPaymentName;
		}


		template.cpName[12]="cTelephone";
		template.cpValue[12]=field[19];

		template.cpName[13]="cFax";
		template.cpValue[13]=field[20];

		//Template index not equal to field index from here on
		template.cpName[14]="cCardExp";
		sprintf(cCardExp,"%s/%s",field[21],field[22]);
		template.cpValue[14]=cCardExp;

		if(field[12][0])
		{
		template.cpName[15]="cShipName";
		template.cpValue[15]=field[12];//see below
		template.cpName[16]="cShipAddr1";
		template.cpValue[16]=field[13];
		template.cpName[17]="cShipAddr2";
		template.cpValue[17]=field[14];
		template.cpName[18]="cShipCity";
		template.cpValue[18]=field[15];
		template.cpName[19]="cShipState";
		template.cpValue[19]=field[16];
		template.cpName[20]="cShipZip";
		template.cpValue[20]=field[17];
		template.cpName[21]="cShipCountry";
		template.cpValue[21]=field[18];
		template.cpName[22]="cInvoice";
		template.cpValue[22]=cInvoice;
		template.cpName[23]="";
		}
		else
		{
		template.cpName[15]="cInvoice";
		template.cpValue[15]=cInvoice;
		template.cpName[16]="";
		}
	}

	if(field)
	{	
		if(uRequireCreditCard)
		{
			if(field[12][0])
			{
				fpTemplate(fp,"cInvoiceCardShipTopMail",&template);
			}	
			else
			{
				fpTemplate(fp,"cInvoiceCardTopMail",&template);
			}
		}
		else
		{
			if(field[12][0])
			{
				fpTemplate(fp,"cInvoiceShipTopMail",&template);
			}
			else
			{
				sprintf(cTemplateName,"cInvoiceTopMail%s",cLanguage);
				fpTemplate(fp,cTemplateName,&template);
			}
		}
	}
	else
	{
		fprintf(fp,"Unexpected error no invoice found. funcInvoice()");
		return;
	}
	mysql_free_result(res);

	//cInvoiceBody: Products and total
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT tInvoiceItems.uProduct,tProduct.cLabel,tInvoiceItems.uQuantity,"
				"tInvoiceItems.mPrice,tInvoiceItems.mTax,tInvoiceItems.mSH,tInvoiceItems.mTotal,"
				"tInvoice.mTotal,tInvoice.mSH FROM tInvoiceItems,tProduct,tInvoice WHERE "
				"tInvoiceItems.uProduct=tProduct.uProduct AND tInvoiceItems.uInvoice=%u AND "
				"tInvoiceItems.uClient=%u AND tInvoice.uInvoice=%u", uInvoice,uClient,uInvoice);
	else
		sprintf(gcQuery,"SELECT tInvoiceItems.uProduct,tProduct.cLabel,tInvoiceItems.uQuantity,"
				"tInvoiceItems.mPrice,tInvoiceItems.mTax,tInvoiceItems.mSH,tInvoiceItems.mTotal,"
				"tInvoice.mTotal,tInvoice.mSH FROM tInvoiceItems,tProduct,tInvoice,tClient WHERE "
				"tInvoiceItems.uProduct=tProduct.uProduct AND tInvoiceItems.uInvoice=%1$u AND "
				"tInvoiceItems.uClient=%2$u AND tInvoice.uInvoice=%1$u AND "
				"tInvoice.uOwner=tClient.uClient AND (tClient.uClient=%3$u OR tClient.uOwner IN "
				"(SELECT uClient FROM tClient WHERE uOwner=%3$u OR uClient=%3$u))", uInvoice,uClient,guCompany);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);


	if(mysql_num_rows(res))
	{
		char cNum[100]={""};
		char cmInvoiceSH[100];
		char cmInvoiceTax[100];
		unsigned uNum=1;
		double lfTotal=0.0,lfInvoiceTotal=0.0;
		double lfInvoiceTax=0.0,lfInvoiceSH=0.0;
		double lfTax=0.0;
		char *cp;
		char cProduct[100]={""};
		
		sprintf(cTemplateName,"cInvoiceItemHeaderMail%s",cLanguage);
		fileDirectTemplate(fp,cTemplateName);
		
		while((field=mysql_fetch_row(res)))
		{
			template.cpName[0]="uProduct";
			template.cpValue[0]=field[0];
			
			sprintf(cProduct,"%s",field[1]);
			if((cp=strchr(cProduct,'.'))) *cp=0;
			
			template.cpName[1]="cProduct";
			template.cpValue[1]=cProduct;
			
			template.cpName[2]="uQuantity";
			template.cpValue[2]=field[2];
			
			template.cpName[3]="mPrice";
			template.cpValue[3]=field[3];
			
			template.cpName[4]="mTax";
			template.cpValue[4]=field[4];
			sscanf(field[4],"%lf",&lfTax);	
			lfInvoiceTax+=lfTax;

			template.cpName[5]="mSH";
			template.cpValue[5]=field[5];
		
			sscanf(field[6],"%lf",&lfTotal);	
			lfInvoiceTotal+=lfTotal;//tInvoiceItems total
			template.cpName[6]="mTotal";
			template.cpValue[6]=field[6];

			sprintf(cNum,"%.3u",uNum++);
			template.cpName[7]="uNum";
			template.cpValue[7]=cNum;
			
			template.cpName[9]="";
			fpTemplate(fp,"cInvoiceItemMail",&template);
			
			if(uNum==2)
				sscanf(field[8]," %lf",&lfInvoiceSH);
		}


		lfInvoiceTotal+=lfInvoiceSH;
		//lfInvoiceTotal+=lfInvoiceTax;

		sprintf(cNum,"%2.2f",lfInvoiceTotal);
		template.cpName[0]="mInvoiceTotal";
		template.cpValue[0]=cNum;

		sprintf(cmInvoiceTax,"%2.2f",lfInvoiceTax);
		template.cpName[1]="mInvoiceTax";
		template.cpValue[1]=cmInvoiceTax;

		//sprintf(cmInvoiceSH,"%.2f",lfInvoiceSH);
		sprintf(cmInvoiceSH,"%2.2f",lfInvoiceSH);
		template.cpName[2]="mInvoiceSH";
		template.cpValue[2]=cmInvoiceSH;

		template.cpName[3]="";
		sprintf(cTemplateName,"cInvoiceItemFooterMail%s",cLanguage);
		fpTemplate(fp,cTemplateName,&template);
	}
	else
	{
		fprintf(fp,"Unexpected error no invoice items. funcInvoice()\n");
	}
	mysql_free_result(res);

}//void funcInvoice(FILE *fp)



void fileDirectTemplate(FILE *fp,char *cTemplateName)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	if(!fp) return;

	TemplateSelect(cTemplateName);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		fprintf(fp,"%s",field[0]);
	else
		fprintf(fp,"Template %s not found\n",cTemplateName);
	mysql_free_result(res);

}//void fileDirectTemplate(FILE *fp,char *cTemplateName)


unsigned GetPaymentValue(unsigned uInvoice,const char *cName,char *cValue)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	unsigned uRequireCreditCard=0;

	if(cName[0])
	sprintf(gcQuery,"SELECT tPayment.uRequireCreditCard,tPayment.%s FROM tInvoice,tPayment WHERE tInvoice.uPayment=tPayment.uPayment AND tInvoice.uInvoice=%u",
					cName,uInvoice);
	else
	sprintf(gcQuery,"SELECT tPayment.uRequireCreditCard FROM tInvoice,tPayment WHERE tInvoice.uPayment=tPayment.uPayment AND tInvoice.uInvoice=%u",uInvoice);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uRequireCreditCard);
		if(cName[0]) sprintf(cValue,"%.63s",field[1]);
	}
	mysql_free_result(res);

	return(uRequireCreditCard);

}//unsigned GetPaymentValue(unsigned uInvoice,const char *cName,char *cValue)


MYSQL_RES *sqlresultClientInfo(void)
{
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT cLabel,cLastName,cEmail,cAddr1,cAddr2,cCity,cState,cZip,cCountry,cCardType,cCardNumber,"
				"cCardName,cShipName,cShipAddr1,cShipAddr2,cShipCity,cShipState,cShipZip,cShipCountry,cTelephone,"
				"cFax,uExpMonth,uExpYear,cLanguage FROM tClient WHERE uClient=%u",uClient);
	else
		sprintf(gcQuery,"SELECT cLabel,cLastName,cEmail,cAddr1,cAddr2,cCity,cState,cZip,cCountry,cCardType,"
				"cCardNumber,cCardName,cShipName,cShipAddr1,cShipAddr2,cShipCity,cShipState,cShipZip,"
				"cShipCountry,cTelephone,cFax,uExpMonth,uExpYear,cLanguage FROM tClient "
				" WHERE uClient=%2$u AND (uClient=%1$u OR uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
				" ORDER BY uClient",guCompany,uClient);
	
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	return(mysql_store_result(&gMysql));

}//MYSQL_RES *sqlresultClientInfo(void)

