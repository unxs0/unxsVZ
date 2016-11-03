/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template and mysqlRAD2 author: (C) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details

TODO
	DONE. 6 tests passed. Debug payment algo. Add more vars to simplfy understanding.
	---
	Work in Progress
		Use tPaid credits to pay for things.

	TODO: Do not adjust tClient.mBalance for billing only usage items.

 
*/


//#define DEBUG


//Local
void CheckNewtPaid(void);
void ProcessPayment(void);
void InternalPaymentProcessing(unsigned const uExtPaid);
void UpdateClientPaymentInfo(unsigned const uExtPaid);
void UpdateInvoicePaymentInfo(unsigned const uInvoiceStatus,
			unsigned const uExtPaid, unsigned const uInvoicePayment);
void UpdateSplitPaidRecord(unsigned const uPaid, unsigned const uParent);
void UpdateExistingPaidCreditItem(unsigned const uInvoice);
void RecreateAllClientTotals(void);


void ExtProcesstPaidVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uCreditClient"))
			sscanf(entries[i].val,"%u",&uClient);
	}
}//void ExtProcesstPaidVars(pentry entries[], int x)


void ExttPaidCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tPaidTools"))
	{
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=8)
			{
	                        ProcesstPaidVars(entries,x);
                        	guMode=2000;
	                        tPaid(LANG_NB_CONFIRMNEW);
			}
			else
				tPaid("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=8)
			{
                        	ProcesstPaidVars(entries,x);

                        	guMode=2000;

				//Provides error messages on case by case basis
				CheckNewtPaid();

				//Now update tClient and tInvoice accordingly
				ProcessPayment();

			}
			else
				tPaid("<blink>Error</blink>: Denied by permissions settings");   
		}
                else if(!strcmp(gcCommand,"PayWithCredits"))
                {
			if( guPermLevel>=8 )
			{
                        	guMode=2003;
				tPaid("Check uClient then [Confirm PayWithCredits]");
			}
			else
				tPaid("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,"Confirm PayWithCredits"))
                {
			if( guPermLevel>=8 )
			{
                        	ProcesstPaidVars(entries,x);
                        	guMode=2003;
				if(!uClient)
					tPaid("Specifiy a client with credits and open invoice(s)");
				if(strcmp(mAmount,"0.00"))
					tPaid("Must specify mAmount=0.00");
				guMode=0;
				ProcessPayment();
			}
			else
				tPaid("<blink>Error</blink>: Denied by permissions settings");   
		}
                else if(!strcmp(gcCommand,"Recreate mTotals"))
                {
			if( guPermLevel>=12 )
			{
				RecreateAllClientTotals();
				tPaid("All tClient.mTotals recreated from tPaid records");
			}
			else
				tPaid("<blink>Error</blink>: Denied by permissions settings");   
                }
	}

}//void ExttPaidCommands(pentry entries[], int x)


void ExttPaidButtons(void)
{
	OpenFieldSet("tPaid Aux Panel",100);
	
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter required data</u><br>");
                        printf("<font size=1>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<p><font size=1>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf("<font size=1>");
                        printf(LANG_NBB_CONFIRMMOD);
		break;

                case 2003:
			printf("<p>uClient <input title='uClient to pay open invoices for from her credits' type=text "
				"name=uCreditClient size=10 maxlength=6><br>");
                        printf("<font size=1>");
                        printf("<input title='Must enter the correct uClient above. Sorry you have to know the uClient number!' "
				"class=largeButton type=submit name=gcCommand value='Confirm PayWithCredits'>\n");
		break;


		default:
                        printf("<font size=1>");
                        printf("<input title='Must be able to enter the correct uClient. Sorry you have to know the uClient number!' "
				"class=largeButton type=submit name=gcCommand value='PayWithCredits'>\n");
			if(guPermLevel>=12)
                        	printf("<p><input class=lwarnButton type=submit name=gcCommand value='Recreate mTotals'><p>");
                        printf("<p><font size=2>");
			if(uInvoice)
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tInvoice&uInvoice=%u>[Invoice]</a><br>\n",uInvoice);
			if(uClient)
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tClient&uClient=%u>[Client]</a><br>\n",uClient);

	}

	CloseFieldSet();
	
}//void ExttPaidButtons(void)


void ExttPaidAuxTable(void)
{

}//void ExttPaidAuxTable(void)


void ExttPaidGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uPaid"))
		{
			sscanf(gentries[i].val,"%u",&uPaid);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"uClient"))
		{
			sscanf(gentries[i].val,"%u",&uClient);
		}
		else if(!strcmp(gentries[i].name,"uPayment"))
		{
			sscanf(gentries[i].val,"%u",&uPayment);
		}
		else if(!strcmp(gentries[i].name,"mTotal"))
		{
			sprintf(mAmount,"%.12s",gentries[i].val);
		}

	}
	if(uClient && uPayment && mAmount[0])
		guMode=2000;
	tPaid("");

}//void ExttPaidGetHook(entry gentries[], int x)


void ExttPaidSelect(void)
{
        //Set non search query here for tTableName()
	ExtSelect("tPaid",VAR_LIST_tPaid,0);

}//void ExttPaidSelect(void)


void ExttPaidSelectRow(void)
{
	ExtSelectRow("tPaid",VAR_LIST_tPaid,uPaid);

}//void ExttPaidSelectRow(void)


void ExttPaidListSelect(void)
{
	char cCat[512];

	ExtListSelect("tPaid",VAR_LIST_tPaid);
	
	//Changes here must be reflected below in ExttPaidListFilter()
        if(!strcmp(gcFilter,"uPaid"))
        {
                sscanf(gcCommand,"%u",&uPaid);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tPaid.uPaid=%u ORDER BY uPaid",uPaid);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uInvoice"))
        {
                sscanf(gcCommand,"%u",&uInvoice);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tPaid.uInvoice=%u ORDER BY uPaid",uInvoice);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uClient"))
        {
                sscanf(gcCommand,"%u",&uClient);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tPaid.uClient=%u ORDER BY uPaid",uClient);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uPayment"))
        {
                sscanf(gcCommand,"%u",&uPayment);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tPaid.uPayment=%u ORDER BY uPaid",uPayment);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uPaid");
        }

}//void ExttPaidListSelect(void)


void ExttPaidListFilter(void)
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
        if(strcmp(gcFilter,"uPayment"))
                printf("<option>uPayment</option>");
        else
                printf("<option selected>uPayment</option>");
        if(strcmp(gcFilter,"uPaid"))
                printf("<option>uPaid</option>");
        else
                printf("<option selected>uPaid</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttPaidListFilter(void)


void ExttPaidNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=8)
		printf(LANG_NBB_NEW);

	if( guPermLevel>=8)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttPaidNavBar(void)

#include "mysqlisp.h"

void CheckNewtPaid(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	double dAmount=0.0;
	unsigned uLocalInvoice=0;

	if(!uPayment)
		tPaid("<blink>Error</blink> Must provide valid uPayment");

	sscanf(mAmount,"%lf",&dAmount);
	if(dAmount<=0.0)
		tPaid("<blink>Error</blink> Must provide valid mAmount");

	if(!uClient)
		tPaid("<blink>Error</blink> Must provide valid uClient");

	if(uInvoice)
		tPaid("<blink>Error</blink> Can't provide uInvoice for now");

	sprintf(gcQuery,"SELECT uClient,MAX(uInvoice) FROM tInvoice WHERE uClient=%u AND uInvoiceStatus<100 GROUP BY uInvoice",uClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(!mysql_num_rows(res))
	{
		tPaid("<blink>Error</blink> uClient must have an open invoice!");
		mysql_free_result(res);
	}
	else
	{
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[1],"%u",&uLocalInvoice);
		}
	}
	mysql_free_result(res);


	
	if(!uInvoice && !uLocalInvoice)
	{
		tPaid("<blink>Error</blink> Internal could not get a valid uInvoice");
	}
	else if(uInvoice)
	{
		//Check it out
sprintf(gcQuery,"SELECT uInvoice FROM tInvoice WHERE uClient=%u AND uInvoice=%u AND uInvoiceStatus<100",uClient,uInvoice);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if(!mysql_num_rows(res))
		{
			tPaid("<blink>Error</blink> Provided uClient,uInvoice does not match an open invoice!");
			mysql_free_result(res);
		}
	}


}//void CheckNewtPaid(void)


void ProcessPayment(void)
{
	//If tPaid amount is larger than last uClient tInvoice mTotal+mPaid and this uClient 
	//has other outstanding invoices then we have to auto-split this tPaid entry 
	//into as many entries as it takes to close the outstanding invoices. And if any
	//monies are left over create a tPaid "credit" entry that will be applied to 
	//future invoices. This temporary "credit" tPaid entry is easily identifiable 
	//since the uInvoice==0.

	MYSQL_RES *res;
	MYSQL_ROW field;


	double dPaidAmount=0.0;//Initial paid amount. If 0 we try to apply any credits
	unsigned uPaid=0;//Used for tPaid item

#ifdef DEBUG
	//Only for debug
	Header_ism3("ProcessPayment()",0);
	printf("</center><p>");
#endif


	sscanf(mAmount,"%lf",&dPaidAmount);

	if(dPaidAmount<=0.0)
	{

		//Use exisiting uClient balance of tPaid "credits."
		sprintf(gcQuery,"SELECT uPaid,mAmount FROM tPaid WHERE uClient=%u AND uInvoice=0 AND mAmount>0",uClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if(!mysql_num_rows(res))
		{
			mysql_free_result(res);
#ifdef DEBUG
			printf("No credits found for %s",ForeignKey("tClient","cLabel",uClient));
			Footer_ism3();
#else
			sprintf(gcQuery,"No credits found for %s",
				ForeignKey("tClient","cLabel",uClient));
			tPaid(gcQuery);
#endif
		}
		else
		{
			while((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&uPaid);
				sprintf(mAmount,"%.12s",field[1]);
				InternalPaymentProcessing(uPaid);
			}
			mysql_free_result(res);


		}
	}
	else
	{
		InternalPaymentProcessing(0);
	}

#ifdef DEBUG
			printf("<p>END %s\n",gcQuery);
			if(uPaid)
				printf("<p>Paid with uClients tPaid credits");
			Footer_ism3();
#else
			tPaid(gcQuery);
#endif

}//void ProcessPayment(void)



//uPaid=0 normal uPaid>0 paying from credits
void InternalPaymentProcessing(unsigned const uExtPaid)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	double dInvoiceAmount=0.0;//Owed on invoice amount
	unsigned uInvoicePayment=0;//uPayment as appears on tInvoice
	double dPaidAmount=0.0;//Initial paid amount that decreases as invoices are paid
	unsigned uClosed=0;//Number of invoices closed. 100% paid.
	unsigned uPartial=0;//Number of invoices not 100% paid.
	unsigned uSplit=0;//When an invoice is paid 100% and uPaidAmount>0 a new
				//tPaid item is created for another invoice or for a credit.
	unsigned uParent=0;//When tPaid items are created this is the prev uPaid id.
				//If uParent is 0 then the tPaid item is the root.
	unsigned uCredit=0;//Number of credit items. Should always be 0 or 1.

#ifdef DEBUG

	printf("<p>Starting InternalPaymentProcessing(%u)",uExtPaid);
#endif
	//Part Two: Operator input valid mAmount for new tPaid items
	sscanf(mAmount,"%lf",&dPaidAmount);

	if(dPaidAmount<=0.0)
	{
#ifdef DEBUG
		printf("<blink>Error</blink> Must provide valid mAmount");
		Footer_ism3();
#else
		tPaid("<blink>Error</blink> Must provide valid mAmount");
#endif
	}


//Closed invoices start at uInvoiceStatus==100. See mysqlisp.h
	sprintf(gcQuery,"SELECT uInvoice,(mTotal+mPaid),uPayment FROM tInvoice WHERE uClient=%u AND uInvoiceStatus<100 ORDER BY uInvoice DESC",uClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(!mysql_num_rows(res))
	{
		mysql_free_result(res);
#ifdef DEBUG
		printf("<blink>Error</blink> Must provide uClient with an open invoice!");
		Footer_ism3();
#else
		tPaid("<blink>Error</blink> Must provide uClient with an open invoice!");
#endif
	}
	else
	{
		while((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uInvoice);
			sscanf(field[1],"%lf",&dInvoiceAmount);
			sscanf(field[2],"%u",&uInvoicePayment);
#ifdef DEBUG
			printf("<p>IN uInvoice:%u dInvoiceAmount:%lf dPaidAmount:%lf mAmount:%s<br>\n"
				"uClosed:%u uPartial:%u uSplit:%u uParent:%u uCredit:%u<p>\n",
				uInvoice,dInvoiceAmount,dPaidAmount,mAmount,
					uClosed,uPartial,uSplit,uParent,uCredit);
#endif

			if(dPaidAmount<=0.0)
			{
#ifdef DEBUG
				printf("break on dPaidAmount&lt;=0<br>\n");
#endif
				break;
			}

			if(dInvoiceAmount==dPaidAmount)
			{
				//1 single tPaid entry close invoice
				sprintf(mAmount,"%2.2lf",dPaidAmount);
				dPaidAmount=0.0;
				UpdateClientPaymentInfo(uExtPaid);
				UpdateInvoicePaymentInfo(unxsISP_PaidInvoice,
								uExtPaid,uInvoicePayment);

				if(uExtPaid)
				{
					uPaid=uExtPaid;
					UpdateExistingPaidCreditItem(uInvoice);
				}
				else
				{

                        		guMode=0;
					uPaid=0;
					uCreatedBy=guLoginClient;
					uOwner=guCompany;
					uModBy=0;//Never modified
					uModDate=0;//Never modified
					NewtPaid(1);
				}
#ifdef DEBUG
				printf("NewtPaid(=) uPaid:%u<br>\n",uPaid);
#endif

				uClosed++;

			}
			else if(dInvoiceAmount>dPaidAmount)
			{
				//2 single tPaid entry partial payment
				sprintf(mAmount,"%2.2lf",dPaidAmount);
				dPaidAmount=0.0;
				UpdateClientPaymentInfo(uExtPaid);
				UpdateInvoicePaymentInfo(unxsISP_PartialInvoice,
								uExtPaid,uInvoicePayment);

				if(uExtPaid)
				{
					uPaid=uExtPaid;
					UpdateExistingPaidCreditItem(uInvoice);
				}
				else
				{
                        		guMode=0;
					uPaid=0;
					uCreatedBy=guLoginClient;
					uOwner=guCompany;
					uModBy=0;//Never modified
					uModDate=0;//Never modified
					NewtPaid(1);
				}
#ifdef DEBUG
				printf("NewtPaid(&gt;) uPaid:%u<br>\n",uPaid);
#endif

				uPartial++;

			}
			else if(dInvoiceAmount<dPaidAmount)
			{
				//3 Must split. First close this invoice
				sprintf(mAmount,"%2.2lf",dInvoiceAmount);
				dPaidAmount-=dInvoiceAmount;
				UpdateClientPaymentInfo(uExtPaid);
				UpdateInvoicePaymentInfo(unxsISP_PaidInvoice,
								uExtPaid,uInvoicePayment);

				if(uExtPaid)
				{
					uPaid=uExtPaid;
					UpdateExistingPaidCreditItem(uInvoice);
				}
				else
				{
                        		guMode=0;
					uPaid=0;
					uCreatedBy=guLoginClient;
					uOwner=guCompany;
					uModBy=0;//Never modified
					uModDate=0;//Never modified
					NewtPaid(1);
				}

				uClosed++;
#ifdef DEBUG
				printf("NewtPaid(&lt;) uPaid:%u<br>\n",uPaid);
#endif

				UpdateSplitPaidRecord(uPaid,uParent);
				uParent=uPaid;

				uSplit++;
			}

			//TODO *23
			//trick way of rounding to cents
			//study this more...seems like just truncation
			//3.334 = 3.33
			//9.978 = 9.97
			sprintf(mAmount,"%2.2lf",dPaidAmount);
			sscanf(mAmount,"%lf",&dPaidAmount);
#ifdef DEBUG
			printf("<br>ENDLOOP dInvoiceAmount:%lf dPaidAmount:%lf mAmount:%s<br>\n"
				"uClosed:%u uPartial:%u uSplit:%u uParent:%u uCredit:%u<p>\n"
				,dInvoiceAmount,dPaidAmount,mAmount,
					uClosed,uPartial,uSplit,uParent,uCredit);
#endif
		}

		if(dPaidAmount>0.0)
		{
				//4 Create "open" tPaid credit item.
				sprintf(mAmount,"%2.2lf",dPaidAmount);

				UpdateClientPaymentInfo(uExtPaid);

				uInvoice=0;//Open

                        	guMode=0;
				uPaid=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtPaid(1);
#ifdef DEBUG
				printf("Credit NewtPaid(&gt;0) uPaid:%u<br>\n",uPaid);
#endif
			
				UpdateSplitPaidRecord(uPaid,uParent);

				dPaidAmount=0.00;
				uCredit++;
		}
#ifdef DEBUG
		printf("<br>Exit-loop dInvoiceAmount:%lf dPaidAmount:%lf mAmount:%s<br>\n"
			"uClosed:%u uPartial:%u uSplit:%u uParent:%u uCredit:%u<p>\n"
				,dInvoiceAmount,dPaidAmount,mAmount,
					uClosed,uPartial,uSplit,uParent,uCredit);
#endif
	}
	mysql_free_result(res);


	if(dPaidAmount!=0.00) 
	{
		sprintf(gcQuery,
			"<blink>Error</blink> dPaidAmount=%lf left. Contact sysadmin ASAP!",
					dPaidAmount);
#ifdef DEBUG
		printf("%s\n",gcQuery);
		Footer_ism3();
#else
		tPaid(gcQuery);
#endif
	}

	sprintf(gcQuery,"New complete payment for invoice %u",uInvoice);
	if(uSplit)
		sprintf(gcQuery,"Split payment for %u invoice(s). %u closed invoice(s). %u "
				"credit item(s). %u partially paid invoice(s).",uSplit,uClosed,uCredit,uPartial);
	else if(uPartial)
		sprintf(gcQuery,"New parcial payment for invoice %u",uInvoice);

#ifdef DEBUG
		printf("%s<p>\n",gcQuery);
#endif
	
}//void InternalPaymentProcessing(unsigned const uExtPaid)


void UpdateClientPaymentInfo(unsigned const uExtPaid)
{
	time_t luClock;

	//If are paying from already split credits. These have already been added to
	//clients balance and totals.
	if(uExtPaid) return;

	time(&luClock);


	sprintf(gcQuery,"UPDATE tClient SET mBalance=mBalance-%s, mTotal=mTotal+%s, uLastPayment=%lu, "
			"uModBy=%u, uModDate=%lu WHERE uClient=%u",mAmount,mAmount,luClock,guLoginClient,luClock,uClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	//Add checking here
#ifdef DEBUG
	printf("UpdateClientPaymentInfo() %s<br>\n",gcQuery);
#endif


	//Update tClient's products invoices and now paid tInstance's
	sprintf(gcQuery,"UPDATE tInstance,tInvoiceItems,tInvoice SET tInstance.uLastPaid=%lu WHERE "
			"tInstance.uInstance=tInvoiceItems.uInstance AND tInvoiceItems.uInvoice=tInvoice.uInvoice "
			"AND tInvoice.uInvoice=%u",luClock,uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));


}//void UpdateClientPaymentInfo(unsigned const uExtPaid)


void UpdateInvoicePaymentInfo(unsigned const uInvoiceStatus, 
			unsigned const uExtPaid, unsigned const uInvoicePayment)
{
	char cuPaidCreditProduct[256]={"57"};
	char cNewInvoiceItemLabel[256];
	time_t luClock;

	time(&luClock);

	sprintf(gcQuery,"UPDATE tInvoice SET uInvoiceStatus=%u, mPaid=mPaid-%s, uModBy=%u, uModDate=%lu WHERE uInvoice=%u",
			uInvoiceStatus,mAmount,guLoginClient,luClock,uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	//Add checking here
#ifdef DEBUG
	printf("UpdateInvoicePaymentInfo-1() %s<br>\n",gcQuery);
#endif

	//Add invoice item credit
	if(uExtPaid && uInvoicePayment)
		sprintf(cNewInvoiceItemLabel,"Credit from %.200s",
			ForeignKey("tPayment","cLabel",uInvoicePayment));
	else
		sprintf(cNewInvoiceItemLabel,"%.200s",ForeignKey("tPayment","cLabel",uPayment));
	
	GetConfiguration("cuPaidCreditProduct",cuPaidCreditProduct,0);
	sprintf(gcQuery,"INSERT INTO tInvoiceItems SET uQuantity=1,uInvoice=%u,uClient=%u,cLabel='%s',"
			"uProduct=%s,mPrice=-%s,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
					uInvoice,
					uClient,
					cNewInvoiceItemLabel,	
					cuPaidCreditProduct,
					mAmount,
					guLoginClient,
					guLoginClient,
					luClock);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	//Add checking here
#ifdef DEBUG
	printf("UpdateInvoicePaymentInfo-2() %s<br>\n",gcQuery);
#endif

}//void UpdateInvoicePaymentInfo()


void UpdateSplitPaidRecord(unsigned const uPaid, unsigned const uParent)
{
	time_t luClock;

	time(&luClock);

	sprintf(gcQuery,"UPDATE tPaid SET cComment=CONCAT(cComment,'\n(Split uParent=%u)\n'),uModDate=%lu,uModBy=%u WHERE uPaid=%u",
			uParent,luClock,guLoginClient,uPaid);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	//Add checking here
#ifdef DEBUG
	printf("UpdateSplitPaidRecord() %s<br>\n",gcQuery);
#endif

}//void UpdateSplitPaidRecord()


void UpdateExistingPaidCreditItem(unsigned const uInvoice)
{
	time_t luClock;

	time(&luClock);

	sprintf(gcQuery,"UPDATE tPaid SET uInvoice=%u, mAmount=%s, cComment=CONCAT(cComment,'\n(Was credit)\n'), uModBy=%u, uModDate=%lu WHERE uPaid=%u",
			uInvoice,mAmount,guLoginClient,luClock,uPaid);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	//Add checking here
#ifdef DEBUG
	printf("UpdateExistingPaidCreditItem() %s<br>\n",gcQuery);
#endif

}//void UpdateExistingPaidCreditItem(unsigned const uInvoice)


void RecreateAllClientTotals(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT SUM(tPaid.mAmount),tClient.uClient FROM tClient,tPaid WHERE tClient.uClient=tPaid.uClient GROUP BY tPaid.uClient");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tClient SET mTotal=%s WHERE uClient=%s",field[0],field[1]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

}//void RecreateAllClientTotals(void)
