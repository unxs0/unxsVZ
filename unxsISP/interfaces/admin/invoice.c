/*
FILE 
	order.c
	svn ID removed
AUTHOR
	(C) 2007-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	ispAdmin  Interface
	program file.
*/


#define APPROVED_NOT_SHIPPED 4
#define APPROVED_AND_SHIPPED 6
#define VOID 7

#include "interface.h"

static unsigned uInvoice=0;
static unsigned uClient=0;
static char cSearch[100]={""};
static char cNavList[16384]={""};
static char cApprovedStep[32]={""};
static char cVoidStep[32]={""};
static char *gcPrintCurr="";
static char *gcPrintAll="";

void fileDirectTemplate(FILE *fp,char *cTemplateName);
unsigned GetPaymentValue(unsigned uInvoice,const char *cName,char *cValue);
MYSQL_RES *sqlresultClientInfo(void);
void ReStockItems(unsigned uInvoice);
char *cGetInvoiceLanguage(unsigned uInvoice);
char *cGetCustomerEmail(unsigned uInvoice);
void EmailLoadedInvoice(void);
void EmailAllInvoices(void);
void PrintInvoices(void);
void PrintInvoice(void);


void ProcessInvoiceVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uInvoice"))
			sscanf(entries[i].val,"%u",&uInvoice);
		else if(!strcmp(entries[i].name,"uClient"))
			sscanf(entries[i].val,"%u",&uClient);
		else if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.99s",entries[i].val);
					
	}

}//void ProcessUserVars(pentry entries[], int x)


void InvoiceGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uInvoice"))
			sscanf(gentries[i].val,"%u",&uInvoice);
		else if(!strcmp(gentries[i].name,"uClient"))
			sscanf(gentries[i].val,"%u",&uClient);
	}

	htmlInvoice();

}//void InvoiceGetHook(entry gentries[],int x)


void InvoiceCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Invoice"))
	{
		ProcessInvoiceVars(entries,x);

		if(!strcmp(gcFunction,"Approved and Shipped"))
		{
			sprintf(cApprovedStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Approved and Shipped"))
		{
			sprintf(gcQuery,"UPDATE tInvoice SET uInvoiceStatus=%u WHERE uInvoice=%u AND uClient=%u",
					APPROVED_AND_SHIPPED
					,uInvoice
					,uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			if(mysql_affected_rows(&gMysql))
			{
				unxsISPLog(uInvoice,"tInvoice","Mod");
				gcMessage="Invoice status updated.";
			}
			else
			{
				unxsISPLog(uInvoice,"tInvoice","Mod Error");
				gcMessage="<blink>Invoice status not updated. Contact support.</blink>";
			}
		}
		 else if(!strcmp(gcFunction,"Void"))
		 {
			 sprintf(cVoidStep,"Confirm ");
		 }
		else if(!strcmp(gcFunction,"Confirm Void"))
		{
			sprintf(gcQuery,"UPDATE tInvoice SET uInvoiceStatus=%u WHERE uInvoice=%u AND uClient=%u",
					VOID
					,uInvoice
					,uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			if(mysql_affected_rows(&gMysql))
			{
				unxsISPLog(uInvoice,"tInvoice","Mod");
				gcMessage="Invoice voided.";
			}
			else
			{
				unxsISPLog(uInvoice,"tInvoice","Mod Error");
				gcMessage="<blink>Invoice status not updated. Contact support.</blink>";
			}
			ReStockItems(uInvoice);
			
		}
		else if(!strcmp(gcFunction,"Email Loaded Invoice"))
		{
			sprintf(gcNewStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Email Loaded Invoice"))
		{
			EmailLoadedInvoice();
			gcMessage="Invoice emailed OK";
		}
		else if(!strcmp(gcFunction,"Email All Invoices"))
		{
			sprintf(gcModStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Email All Invoices"))
		{
			EmailAllInvoices();
			gcMessage="All invoices emailed  OK";
		}
		else if(!strcmp(gcFunction,"Print Loaded Invoice"))
		{
			gcPrintCurr="Confirm ";
		}
		else if(!strcmp(gcFunction,"Confirm Print Loaded Invoice"))
		{
			PrintInvoice();
		}
		else if(!strcmp(gcFunction,"Print All Invoices"))
		{
			gcPrintAll="Confirm ";
		}
		else if(!strcmp(gcFunction,"Confirm Print All Invoices"))
		{
			PrintInvoices();
		}
		htmlInvoice();
	}

}//void InvoiceCommands(pentry entries[], int x)


void htmlInvoice(void)
{
	htmlHeader("unxsISP Admin","Header");
	htmlInvoicePage("","Invoice.Body");
	htmlFooter("Footer");

}//void htmlInvoice(void)


void htmlInvoicePage(char *cTitle, char *cTemplateName)
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
			char cuInvoice[16]={""};
			char cuClient[16]={""};
			
			sprintf(cuClient,"%u",uClient);
			sprintf(cuInvoice,"%u",uInvoice);

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

			template.cpName[8]="uInvoice";
			template.cpValue[8]=cuInvoice;

			template.cpName[9]="uClient";
			template.cpValue[9]=cuClient;

			template.cpName[10]="cNavList";
			template.cpValue[10]=cNavList;

			template.cpName[11]="gcInputStatus";
			if(uInvoice && uClient)
				template.cpValue[11]="";
			else
				template.cpValue[11]="disabled";

			template.cpName[12]="cApprovedStep";
			template.cpValue[12]=cApprovedStep;

			template.cpName[13]="cVoidStep";
			template.cpValue[13]=cVoidStep;

			template.cpName[14]="gcNewStep";
			template.cpValue[14]=gcNewStep;

			template.cpName[15]="gcModStep";
			template.cpValue[15]=gcModStep;

			template.cpName[16]="gcPrintCurr";
			template.cpValue[16]=gcPrintCurr;

			template.cpName[17]="gcPrintAll";
			template.cpValue[17]=gcPrintAll;

			template.cpName[18]="";
			
			printf("\n<!-- Start htmlInvoicePage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlInvoicePage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlInvoicePage()


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
				"(SELECT uClient FROM tClient WHERE uOwner=%3$u OR uClient=%3$u))", uInvoice,uClient,guOrg);

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
				" ORDER BY uClient",guOrg,uClient);
	
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	return(mysql_store_result(&gMysql));

}//MYSQL_RES *sqlresultClientInfo(void)


void ReStockItems(unsigned uInvoice)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uChange=0;

	//Re-stock items in tInvoiceItems for this invoice
	//Needs more error checking but should do for now for root level operator
	sprintf(gcQuery,"SELECT uProduct,uQuantity FROM tInvoiceItems WHERE uInvoice=%u",uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tProduct SET uInStock=uInStock+%s WHERE uProduct=%s",
				field[1],field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		uChange++;
	}
	mysql_free_result(res);

	if(!uChange)
		gcMessage="<blink>ReStockItems() failed no updates, Contact support</blink>";

}//void ReStockItems(unsigned uInvoice)


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


void funcInvoiceNavList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uDisplayed=0;
	unsigned uFound=0;
	char cTopMessage[100]={""};

	if(cSearch[0])
	{
	//Valid formats are:
	//1. uInvoice-uClient
	//2. uInvoice
	//3. Part of last name
		char cExtra[100]={""};
		if(strstr(cSearch,"-"))
		{
			unsigned uClient=0;

			sscanf(cSearch,"%u-%u",&uClient,&uInvoice);
			sprintf(cExtra,"tInvoice.uInvoice=%u AND tInvoice.uClient=%u",uInvoice,uClient);

			ExtSelectSearch("tInvoice","tInvoice.uInvoice,FROM_UNIXTIME(GREATEST(tInvoice.uCreatedDate,tInvoice.uModDate)),"
				"(SELECT CONCAT(cFirstName,' ',cLastName) FROM tClient WHERE tClient.uClient=tInvoice.uClient),tInvoice.uClient","1","1",
				cExtra,0);
		}
		else
		{
			sscanf(cSearch,"%u",&uInvoice);
			if(uInvoice)
			{
				sprintf(cExtra,"tInvoice.uInvoice=%u",uInvoice);
				ExtSelectSearch("tInvoice","tInvoice.uInvoice,FROM_UNIXTIME(GREATEST(tInvoice.uCreatedDate,tInvoice.uModDate)),"
					"(SELECT CONCAT(cFirstName,' ',cLastName) FROM tClient WHERE tClient.uClient=tInvoice.uClient),tInvoice.uClient",
					"1","1",cExtra,0);
			}
			else
				ExtSelectSearch("tInvoice","tInvoice.uInvoice,FROM_UNIXTIME(GREATEST(tInvoice.uCreatedDate,tInvoice.uModDate)),"
					"(SELECT CONCAT(cFirstName,' ',cLastName) FROM tClient WHERE tClient.uClient=tInvoice.uClient),tInvoice.uClient",
					"tInvoice.cLastName",cSearch,NULL,0);
		}
	}
	else
		ExtSelect("tInvoice","tInvoice.uInvoice,FROM_UNIXTIME(GREATEST(tInvoice.uCreatedDate,tInvoice.uModDate)),"
				"(SELECT CONCAT(cFirstName,' ',cLastName) FROM tClient WHERE tClient.uClient=tInvoice.uClient),tInvoice.uClient",0);
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
			//This 'loads' the invoice, via funcInvoice() ;)
			sscanf(field[0],"%u",&uInvoice);
			sscanf(field[3],"%u",&uClient);
			
			fprintf(fp,"<a href=ispAdmin.cgi?gcPage=Invoice&uInvoice=%s&uClient=%s>%s - %s</a><br>\n",field[0],field[3],field[2],field[1]);
			mysql_free_result(res);
			return;
		}
	}

	while((field=mysql_fetch_row(res)))
	{
		if(uDisplayed==20)
		{
			fprintf(fp,"Only the first 20 records found. If the invoice you are looking for is not in the list above please "
			"further refine your search.<br>\n");
			break;
		}
		FromMySQLDate(field[1]);
		fprintf(fp,"<a href=ispAdmin.cgi?gcPage=Invoice&uInvoice=%s&uClient=%s>%s - %s</a><br>\n",field[0],field[3],field[2],field[1]);
		uDisplayed++;
	}

	mysql_free_result(res);

}//void funcInvoiceNavList(FILE *fp)


void EmailLoadedInvoice(void)
{
	FILE *fp;
	char cFrom[256]={"root"};
	char cSubject[256]={""};
	char cSubjectLang[100]={""};
	char cBcc[256]={""};
	char cEmail[100]={""};

	cSubject[255]=0;
	
	GetConfiguration("cFromEmailAddr",cFrom);
	GetConfiguration("cInvoiceBccEmailAddr",cBcc);
	
	sprintf(gcQuery,"cSubjectLang%s",cGetInvoiceLanguage(uInvoice));
	GetConfiguration(gcQuery,cSubjectLang);
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

