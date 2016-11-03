/*
FILE 
	order.c
	svn ID removed
AUTHOR
	(C) 2007-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	ispClient  Interface
	program file.
*/


#define APPROVED_NOT_SHIPPED 4
#define APPROVED_AND_SHIPPED 6
#define VOID 7

#include "interface.h"

static unsigned uInvoice=0;
static char cSearch[100]={""};
static char cNavList[16384]={""};
static char cApprovedStep[32]={""};
static char cVoidStep[32]={""};
static char *gcPrintCurr="";
static char *gcPrintAll="";

void fileDirectTemplate(FILE *fp,char *cTemplateName);
unsigned GetPaymentValue(unsigned uInvoice,const char *cName,char *cValue);
MYSQL_RES *sqlresultClientInfo(void);
char *cGetCustomerEmail(unsigned uInvoice);
void EmailLoadedInvoice(void);
void EmailAllInvoices(void);
void PrintInvoices(void);
void PrintInvoice(void);
void htmlPayInvoice(void);
void PaymentFieldsOn(void);
unsigned ValidatePaymentInput(void);
void LoadPaymentData(void);
void UpdatePaymentData(void);
unsigned SubmitRequest(unsigned uInvoice); //payment.c
void htmlPostPayment(unsigned uMode);
void UpdateInvoice(void);

unsigned uSetupRB=0;
extern char *cCardNameStyle;
extern char *cCardNumberStyle;
extern char *cuExpMonthStyle;
extern char *cuExpYearStyle;
extern char *cCardTypeStyle;
extern char cCardName[];
extern char cCardNumber[];
extern unsigned uExpMonth;
extern unsigned uExpYear;
extern char cCardType[];
extern char cResult[]; //payment.c global w/transaction message

static char cAuthCode[100]={""};

void ProcessInvoiceVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uInvoice"))
			sscanf(entries[i].val,"%u",&uInvoice);
		else if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.99s",entries[i].val);
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
					
	}

}//void ProcessUserVars(pentry entries[], int x)


void InvoiceGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uInvoice"))
			sscanf(gentries[i].val,"%u",&uInvoice);
	}
	htmlInvoice();

}//void InvoiceGetHook(entry gentries[],int x)


void InvoiceCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Invoice"))
	{
		ProcessInvoiceVars(entries,x);
		if(!strcmp(gcFunction,"Show Invoice for Printing") ||
			!strcmp(gcFunction,"Voir le Facture pour Impression") ||
			!strcmp(gcFunction,"Mostrar Factura para Imprimir"))
			PrintInvoice();
		else if(!strcmp(gcFunction,"Pay Loaded Invoice") ||
			!strcmp(gcFunction,"Pagar la Factura Actual") ||
			!strcmp(gcFunction,"Payer le Facture Chargee"))
		{
			gcInputStatus[0]=0;
			PaymentFieldsOn();
			LoadPaymentData();
			htmlPayInvoice();
		}
		else if(!strcmp(gcFunction,"Complete Payment") ||
			!strcmp(gcFunction,"Completar Pago"))
		{
			if(!ValidatePaymentInput())			
			{
				gcInputStatus[0]=0;
				htmlPayInvoice();
			}
			UpdatePaymentData();
			if(SubmitRequest(uInvoice))
			{
				UpdateInvoice(); //Set paid status
				htmlPostPayment(1);
			}
			else
				htmlPostPayment(0);
		}
		htmlInvoice();
	}

}//void InvoiceCommands(pentry entries[], int x)


void htmlInvoice(void)
{
	htmlHeader("unxsISP Customer Interface","Header");
	if(uSetupRB)
		htmlInvoicePage("","MyInvoiceSubs.Body");
	else
	{
		FILE *fp;
		if((fp=fopen("/dev/null","w")))
		{
			funcInvoiceNavList(fp);
			fclose(fp);
		}
		htmlInvoicePage("","MyInvoice.Body");
	}
	htmlFooter("Footer");

}//void htmlInvoice(void)


void htmlPayInvoice(void)
{
	htmlHeader("unxsISP Customer Interface","Header");
	htmlInvoicePage("","PayInvoice.Body");
	htmlFooter("Footer");

}//void htmlPayInvoice(void)


void htmlPostPayment(unsigned uMode)
{
	htmlHeader("unxsISP Customer Interface","Header");
	if(uMode)
	{
		MYSQL_RES *res;
		MYSQL_ROW field;

		sprintf(gcQuery,"SELECT cAuthCode FROM tInvoice WHERE uInvoice=%u",uInvoice);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		field=mysql_fetch_row(res);
		sprintf(cAuthCode,"%.99s",field[0]);
		htmlInvoicePage("","PayInvoiceAp.Body");
	}
	else
		htmlInvoicePage("","PayInvoiceDe.Body");
	htmlFooter("Footer");

}//void htmlPostPayment(unsigned uMode)


void htmlInvoicePage(char *cTitle, char *cTemplateName)
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
			char cuInvoice[16]={""};
			char cuClient[16]={""};
			
			sprintf(cuClient,"%u",guLoginClient);
			sprintf(cuInvoice,"%u",uInvoice);

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

			template.cpName[8]="uInvoice";
			template.cpValue[8]=cuInvoice;

			template.cpName[9]="uClient";
			template.cpValue[9]=cuClient;

			template.cpName[10]="cNavList";
			template.cpValue[10]=cNavList;

			template.cpName[11]="gcInputStatus";
			if(uInvoice && guLoginClient)
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

			template.cpName[18]="cCardNameStyle";
			template.cpValue[18]=cCardNameStyle;

			template.cpName[19]="cCardNumberStyle";
			template.cpValue[19]=cCardNumberStyle;

			template.cpName[20]="cCardName";
			template.cpValue[20]=cCardName;

			template.cpName[21]="cCardNumber";
			template.cpValue[21]=cCardNumber;

			template.cpName[22]="cResult";
			template.cpValue[22]=cResult;

			template.cpName[23]="cAuthCode";
			template.cpValue[23]=cAuthCode;

			template.cpName[24]="";
			
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

	char cPaymentName[31]={"Unknown"};
	unsigned uRequireCreditCard=0;

	template.cpName[0]="";

	if(!uInvoice)
	{
		fprintf(fp,"No invoice loaded.");
		return;
	}

	uRequireCreditCard=GetPaymentValue(uInvoice,"","");
	sprintf(cInvoice,"%u-%u",guLoginClient,uInvoice);

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
				fpTemplate(fp,"cInvoiceTopMail",&template);
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
				"tInvoiceItems.uClient=%u AND tInvoice.uInvoice=%u", uInvoice,guLoginClient,uInvoice);
	else
		sprintf(gcQuery,"SELECT tInvoiceItems.uProduct,tProduct.cLabel,tInvoiceItems.uQuantity,"
				"tInvoiceItems.mPrice,tInvoiceItems.mTax,tInvoiceItems.mSH,tInvoiceItems.mTotal,"
				"tInvoice.mTotal,tInvoice.mSH FROM tInvoiceItems,tProduct,tInvoice,tClient WHERE "
				"tInvoiceItems.uProduct=tProduct.uProduct AND tInvoiceItems.uInvoice=%1$u AND "
				"tInvoiceItems.uClient=%2$u AND tInvoice.uInvoice=%1$u AND "
				"tInvoice.uOwner=tClient.uClient AND (tClient.uClient=%3$u OR tClient.uOwner IN "
				"(SELECT uClient FROM tClient WHERE uOwner=%3$u OR uClient=%3$u))", uInvoice,guLoginClient,guOrg);

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
		
		fileDirectTemplate(fp,"cInvoiceItemHeaderMail");
		
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
		fpTemplate(fp,"cInvoiceItemFooterMail",&template);
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

	TemplateSelect(cTemplateName,guTemplateSet);
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
		sprintf(gcQuery,"SELECT tPayment.uRequireCreditCard,tPayment.%s FROM tInvoice,tPayment "
				"WHERE tInvoice.uPayment=tPayment.uPayment AND tInvoice.uInvoice=%u",
				cName,uInvoice);
	else
		sprintf(gcQuery,"SELECT tPayment.uRequireCreditCard FROM tInvoice,tPayment WHERE "
				"tInvoice.uPayment=tPayment.uPayment AND tInvoice.uInvoice=%u",
				uInvoice);

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
				"cFax,uExpMonth,uExpYear,cLanguage FROM tClient WHERE uClient=%u",guLoginClient);
	else
		sprintf(gcQuery,"SELECT cLabel,cLastName,cEmail,cAddr1,cAddr2,cCity,cState,cZip,cCountry,cCardType,"
				"cCardNumber,cCardName,cShipName,cShipAddr1,cShipAddr2,cShipCity,cShipState,cShipZip,"
				"cShipCountry,cTelephone,cFax,uExpMonth,uExpYear,cLanguage FROM tClient "
				" WHERE uClient=%2$u AND (uClient=%1$u OR uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
				" ORDER BY uClient",guOrg,guLoginClient);
	
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	return(mysql_store_result(&gMysql));

}//MYSQL_RES *sqlresultClientInfo(void)


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
	static char cTopMessage[100]={""};
	char cExtra[100]={""};
	char *cLabel="";

	sprintf(cExtra,"tInvoice.uClient=%u",guLoginClient);
	
	ExtSelectSearch("tInvoice","tInvoice.uInvoice,FROM_UNIXTIME(GREATEST(tInvoice.uCreatedDate,tInvoice.uModDate)),"
			"(SELECT tInvoiceStatus.cLabel FROM tInvoiceStatus WHERE tInvoiceStatus.uInvoiceStatus=tInvoice.uInvoiceStatus)",
			"1","1",cExtra,0);
	//}
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	
	if(!(uFound=mysql_num_rows(res)))
	{
		if(guTemplateSet==2)
			fprintf(fp,"No records found.<br>\n");
		else if(guTemplateSet==3)
			fprintf(fp,"No se encontraron registros.<br>\n");
		else if(guTemplateSet==4)
			fprintf(fp,"No records found (french).<br>\n");

		mysql_free_result(res);
		return;
	}

	if(guTemplateSet==2)
	{
		cLabel="Invoice";
		sprintf(cTopMessage,"%u record(s) found.",uFound);
	}
	else if(guTemplateSet==3)
	{
		cLabel="Factura";
		sprintf(cTopMessage,"%u registro(s) encontrados.",uFound);
	}
	else if(guTemplateSet==4)
	{
		cLabel="Facture";
		sprintf(cTopMessage,"%u record(s) found. (french)",uFound);
	}

	gcMessage=cTopMessage;

	if(uFound==1)
	{
		if((field=mysql_fetch_row(res)))
		{
			//This 'loads' the invoice, via funcInvoice() ;)
			sscanf(field[0],"%u",&uInvoice);
			
			fprintf(fp,"<a href=ispClient.cgi?gcPage=Invoice&uInvoice=%s>%s #%s (%s) [%s]</a><br>\n"
			,field[0],cLabel,field[0],field[1],field[2]);
			mysql_free_result(res);
			return;
		}
	}

	while((field=mysql_fetch_row(res)))
	{
		if(uDisplayed==20)
		{
			if(guTemplateSet==2)
					fprintf(fp,"Only the first 20 records shown. If the invoice you are looking for is not in the list above please "
						"further refine your search.<br>\n");
			else if(guTemplateSet==3)
					fprintf(fp,"Solamente se muestran los primeros 20 registros. Si la factura que est&aacute; buscando no se aparece "
						"en la lista de arriba por favor refine su b&uacute;squeda.<br>\n");
			else if(guTemplateSet==4)
					fprintf(fp,"Only the first 20 records found. If the invoice you are looking for is not in the list above please "
						"further refine your search. (french)<br>\n");
			break;
		}
		FromMySQLDate(field[1]);
		fprintf(fp,"<a href=ispClient.cgi?gcPage=Invoice&uInvoice=%s>%s - %s</a><br>\n",field[0],field[2],field[1]);
		uDisplayed++;
	}

	mysql_free_result(res);

}//void funcInvoiceNavList(FILE *fp)


char *cGetClientLanguage(void)
{
	static char cLanguage[32]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLanguage FROM tClient WHERE tClient.uClient=%u",guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cLanguage,"%.3s",field[0]);
	else
		sprintf(cLanguage,"Eng");
	
	return(cLanguage);

}//char *cGetClientLanguage(void)


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


void PrintInvoice(void)
{
	printf("Content-type: text/html\n\n");
	funcInvoice(stdout);
	exit(0);

}//void PrintInvoice(void)



void funcProductList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	sprintf(gcQuery,"SELECT tInstance.uInstance,tInstance.cLabel,"
			"FROM_UNIXTIME(GREATEST(tInstance.uCreatedDate,tInstance.uModDate)),"
			"tStatus.cLabel FROM tInstance,tStatus WHERE tInstance.uClient='%u' "
			"AND tInstance.uStatus=tStatus.uStatus ORDER BY GREATEST(tInstance.uCreatedDate,"
			"tInstance.uModDate)",guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		FromMySQLDate(field[2]);
		fprintf(fp,"<tr><td>(Instance %s)</td><td align=center>%s</td><td align=right>%s</td>"
				"<td align=center><input type=checkbox name=uSubscribe.%s</tr>\n",
				field[1]
				,field[3]
				,field[2]
				,field[0]
		       );
	}

}//void funcProductList(FILE *fp)


void PaymentFieldsOn(void)
{
	cCardNameStyle="type_fields";
	cCardNumberStyle="type_fields";
	cuExpMonthStyle="type_fields";
	cuExpYearStyle="type_fields";
	cCardTypeStyle="type_fields";

}//void PaymentFieldsOn(void)


void LoadPaymentData(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cCardType,cCardName,cCardNumber,uExpMonth,uExpYear FROM tInvoice WHERE uInvoice=%u",uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cCardType,"%s",field[0]);
		sprintf(cCardName,"%s",field[1]);
		sprintf(cCardNumber,"%s",field[2]);
		sscanf(field[3],"%u",&uExpMonth);
		sscanf(field[4],"%u",&uExpYear);
	}
	mysql_free_result(res);

}//void LoadPaymentData(void)


void UpdatePaymentData(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cEmail,cAddr1,cAddr2,cCity,cState,cZip,cTelephone FROM tClient WHERE uClient=%u",guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	field=mysql_fetch_row(res);//will always return a row

	sprintf(gcQuery,"UPDATE tInvoice SET cCardType='%s',cCardName='%s',cCardNumber='%s',uExpMonth='%u',uExpYear='%u',"
			"cEmail='%s',cAddr1='%s',cAddr2='%s',cCity='%s',cState='%s',cZip='%s',cTelephone='%s' WHERE uInvoice=%u",
			cCardType
			,cCardName
			,cCardNumber
			,uExpMonth
			,uExpYear
			,field[0]
			,field[1]
			,field[2]
			,field[3]
			,field[4]
			,field[5]
			,field[6]
			,uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	mysql_free_result(res);

}//void UpdatePaymentData(void)


unsigned ValidatePaymentInput(void)
{
	if(!strcmp(cCardType,"---"))
	{
		PaymentFieldsOn();
		cCardTypeStyle="type_fields_req";
		if(guTemplateSet==2)
			gcMessage="<blink>Error: </blink>Must select credit card type";
		else if(guTemplateSet==3)
			gcMessage="<blink>Error: </blink>Debe seleccionar tipo de tarjeta de cr&eacute;dito";
		else if(guTemplateSet==4)
			gcMessage="<blink>Error: </blink>Must select credit card type (french)";

		return(0);
	}
	if(!cCardName[0])
	{
		PaymentFieldsOn();
		cCardNameStyle="type_fields_req";
		if(guTemplateSet==2)
			gcMessage="<blink>Error: </blink>Must enter credit card name";
		else if(guTemplateSet==3)
			gcMessage="<blink>Error: </blink>Debe ingresar el nombre en la tarjeta de cr&eacute;dito";
		else if(guTemplateSet==4)
			gcMessage="<blink>Error: </blink>Must enter credit card name (french)";

		return(0);
	}
	if(!cCardNumber[0])
	{
		PaymentFieldsOn();
		cCardNumberStyle="type_fields_req";
		if(guTemplateSet==2)
			gcMessage="<blink>Error: </blink>Must enter credit card number";
		else if(guTemplateSet==3)
			gcMessage="<blink>Error: </blink>Debe ingresar el n&uacute;mero de la tarjeta de cr&eacute;dito";
		else if(guTemplateSet==4)
			gcMessage="<blink>Error: </blink>Must enter credit card number (french)";
		return(0);
	}
	if(!uExpMonth)
	{
		PaymentFieldsOn();
		cuExpMonthStyle="type_fields_req";
		if(guTemplateSet==2)
			gcMessage="<blink>Error: </blink>Must select expiration month";
		else if(guTemplateSet==3)
			gcMessage="<blink>Error: </blink>Debe seleccionar el mes de expiraci&oacute;n";
		else if(guTemplateSet==4)
			gcMessage="<blink>Error: </blink>Must select expiration month (french)";

		return(0);
	}
	if(!uExpYear)
	{
		PaymentFieldsOn();
		cuExpYearStyle="type_fields_req";
		if(guTemplateSet==2)
			gcMessage="<blink>Error: </blink>Must select expiration year";
		else if(guTemplateSet==3)
			gcMessage="<blink>Error: </blink>Debe seleccionar el a&ntilde;o de expiraci&oacute;n";
		else if(guTemplateSet==4)
			gcMessage="<blink>Error: </blink>Must select expiration year (french)";

		return(0);
	}
	
	return(1);

}//unsigned ValidatePaymentInput(void)


void UpdateInvoice(void)
{
	sprintf(gcQuery,"UPDATE tInvoice SET uInvoiceStatus=2 WHERE uInvoice=%u",uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void UpdateInvoice(void)

