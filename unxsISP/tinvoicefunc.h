/*
FILE
	svn ID removed
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
*/

static unsigned uOpenOnly=0;

#include "mysqlisp.h"

void GenerateInvoices(void);//Called from both mainfunc.h and webmin
void ClearForm(void);
void RemoveInvoices(void);
void InvoiceItemList(unsigned uInvoice);
void PrintInvoices(void);
unsigned GetCurrentNewInvoices(void);
unsigned GetNonQueuedInvoices(void);
void SubmitInvoiceJob(char *cJobType,time_t luJobDate);
unsigned GetInvoiceStatus(unsigned const uInvoice, unsigned *uQueueStatus);

//template.c hooks
void fileInvoice(FILE *fp);
void filePackingSlip(FILE *fp);
void textFatalError(char const *cErrorText);

//file scope vars
static unsigned uSearch=0;
unsigned GetPaymentValue(unsigned uInvoice,const char *cName,char *cValue);
MYSQL_RES *sqlresultClientInfo(void);
void ReStockItems(unsigned uInvoice);
char *cGetInvoiceLanguage(unsigned uInvoice);
char *cGetCustomerEmail(unsigned uInvoice);
void EmailLoadedInvoice(void);
void EmailAllInvoices(void);
void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template);
void PrintInvoice(void);
void fileDirectTemplate(FILE *fp,char *cTemplateName);
void funcInvoice(FILE *fp);

void ExtProcesstInvoiceVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uSearch"))
			sscanf(entries[i].val,"%u",&uSearch);
		else if(!strcmp(entries[i].name,"uOpenOnly"))
			sscanf(entries[i].val,"%u",&uOpenOnly);
	}
}//void ExtProcesstInvoiceVars(pentry entries[], int x)


void ExttInvoiceCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tInvoiceTools"))
	{
		if(!strcmp(gcCommand,"Generate Invoices"))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstInvoiceVars(entries,x);
				printf("Content-type: text/html\n\n");
				GenerateInvoices();
				printf("This months invoices generated. <a href=unxsISP.cgi?gcFunction=tInvoice>Click here to return</a>");
				exit(0);
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,"Email Invoices"))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstInvoiceVars(entries,x);

                        	guMode=3002;
                        	tInvoice("First Step...");
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,"Confirm Email Invoices"))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstInvoiceVars(entries,x);

				EmailAllInvoices();
                        	tInvoice("Invoices emailed");
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,"Email Loaded Invoice"))
		{
			if(guPermLevel>=10)
			{
				ProcesstInvoiceVars(entries,x);
				guMode=3003;
				tInvoice("First Step...");
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Confirm Email Loaded Invoice"))
		{
			if(guPermLevel>=10)
			{
				ProcesstInvoiceVars(entries,x);
				EmailLoadedInvoice();
				tInvoice("Invoice emailed");
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Print All Invoices"))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstInvoiceVars(entries,x);
				PrintInvoices();
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,"Print Loaded Invoice"))
		{
			if(guPermLevel>=10)
			{
				ProcesstInvoiceVars(entries,x);
				PrintInvoice();
			}
			else
				tInvoice("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Remove Invoices"))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstInvoiceVars(entries,x);
				RemoveInvoices();
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
			printf("<input class=lwarnButton title='Email all pending invoices'"
				" type=submit name=gcCommand value='Confirm Email Invoices'><br>");
                break;
		
		case 3003:
			printf("<p><u>Review Invoice First</u><br>It will be emailed as is. No undo available. "
				"You can also customize each invoice now, before committing. Press [Search] to backout. "
				"Do not email invoice again if you do not want to!<p>");
			printf("<input class=lwarnButton title='Email the current loaded invoice'"
				" type=submit name=gcCommand value='Confirm Email Loaded Invoice'><br>");
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

			if(guPermLevel>=10)
			{
				printf("<p><u>Billing Functions</u><br>");
				printf("<input class=largeButton title='Generate this months invoices. Your company will own them. "
					"Only the owner can remove \"New\" ones, mail or print them.' type=submit "
					"name=gcCommand value='Generate Invoices'><br>");
				printf("<input class=largeButton title='Remove the \"New\" status [Generate Invoices] "
					"created invoices you own. mySQL 4.0.4+.' type=submit name=gcCommand value='Remove Invoices'><br>");
				printf("<input class=lwarnButton title='Email the current loaded invoice.' "
					"type=submit name=gcCommand value='Email Loaded Invoice'><br>");
				printf("<input class=lwarnButton title='Email all not already queued invoices your company owns. "
					"You better check them all first!' type=submit name=gcCommand value='Email Invoices'><br>");
				printf("<input class=largeButton title='Prints the loaded invoice'"
					" type=submit name=gcCommand value='Print Loaded Invoice'><br>");
				printf("<input class=largeButton title='Print all not invoices your company owns.'"
					" type=submit name=gcCommand value='Print All Invoices'><br>");


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


void EmailLoadedInvoice(void)
{
	FILE *fp;
	char cFrom[256]={"root"};
	char cSubject[256]={""};
	char cSubjectLang[100]={""};
	char cBcc[256]={""};
	char cEmail[100]={""};

	cSubject[255]=0;
	
	GetConfiguration("cFromEmailAddr",cFrom,1);
	GetConfiguration("cInvoiceBccEmailAddr",cBcc,1);
	
	sprintf(gcQuery,"cSubjectLang%s",cGetInvoiceLanguage(uInvoice));
	GetConfiguration(gcQuery,cSubjectLang,1);
	if(!cSubjectLang[0])
		sprintf(cSubjectLang,"Invoice #");
	
	sprintf(cSubject,"%s %u-%u",cSubjectLang,uClient,uInvoice);
	sprintf(cEmail,"%s",cGetCustomerEmail(uInvoice));

	if((fp=popen("/usr/lib/sendmail -t > /dev/null","w")))
	//debug only
	//if((fp=fopen("/tmp/eMailInvoice","w")))
	{
		fprintf(fp,"To: %s\n",cEmail);
		fprintf(fp,"From: %s\n",cFrom);
		fprintf(fp, "Reply-to: %s\n",cFrom);
		if(cBcc[0]) fprintf(fp, "Bcc: %s\n",cBcc);
		fprintf(fp,"Subject: %s\n",cSubject);
		fprintf(fp,"MIME-Version: 1.0\n");
		fprintf(fp,"Content-type: text/html\n\n");
		funcInvoice(fp);
	}
	pclose(fp);

}//void EmailLoadedInvoice(void)


void EmailAllInvoices(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	sprintf(gcQuery,"SELECT uInvoice,uClient FROM tInvoice WHERE uInvoiceStatus!=2");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uInvoice);
		sscanf(field[1],"%u",&uClient);
		EmailLoadedInvoice();
	}
	uInvoice=0;
	uClient=0;

}//void EmailAllInvoices(void)

char *cGetInvoiceLanguage(unsigned uInvoice)
{
	static char cLanguage[32]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLanguage FROM tClient,tInvoice WHERE tClient.uClient=tInvoice.uClient AND tInvoice.uInvoice=%u",uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cLanguage,"%.3s",field[0]);
	else
		sprintf(cLanguage,"Eng");
	
	return(cLanguage);

}//char *cGetInvoiceLanguage(unsigned uInvoice)


char *cGetCustomerEmail(unsigned uInvoice)
{
	static char cEmail[100]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cEmail FROM tInvoice WHERE uInvoice=%u",uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cEmail,"%.99s",field[0]);
	
	return(cEmail);

}//char cGetCustomerEmail(unsigned uInvoice)


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


void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template)
{
	if(cTemplateName[0])
	{	
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect(cTemplateName);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"<!-- start %s -->\n",cTemplateName);
			Template(field[0], template, fp);
		}
		else
		{
			fprintf(fp,"<hr>");
			fprintf(fp,"<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}
}//void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template)


void PrintInvoices(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	printf("Content-type: text/html\n\n");
	printf("<html>\n"
		"<head>\n"
		"<style>\n"
		"@page { size 8.5in 11in; margin: 2cm }\n"
		"div.page { page-break-after: always }\n"
		"</style>\n"
		"</head>\n"
		"<body>\n");

	sprintf(gcQuery,"SELECT uInvoice,uClient FROM tInvoice WHERE uInvoiceStatus!=2");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uInvoice);
		sscanf(field[1],"%u",&uClient);
		printf("<div class=page>\n");
		funcInvoice(stdout);	
		printf("</div>\n");
	}
	uInvoice=0;
	uClient=0;
	exit(0);

}//void PrintInvoices(void)


void PrintInvoice(void)
{
	printf("Content-type: text/html\n\n");
	funcInvoice(stdout);
	exit(0);
}//void PrintInvoice(void)

