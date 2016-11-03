/*
FILE
	tInvoice source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tinvoicefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uInvoice: Primary Key
static unsigned uInvoice=0;
//uShipping: Shipping method for all products on invoice
static unsigned uShipping=0;
static char cuShippingPullDown[256]={""};
//uPayment: Payment method for all products on invoice
static unsigned uPayment=0;
static char cuPaymentPullDown[256]={""};
//uInvoiceStatus: Invoice status
static unsigned uInvoiceStatus=0;
static char cuInvoiceStatusPullDown[256]={""};
//uQueueStatus: Queue Status
static unsigned uQueueStatus=0;
static char cuQueueStatusPullDown[256]={""};
//uClient: Client ID
static unsigned uClient=0;
//mTotal: Invoice total from tInvoiceItems
static char mTotal[16]={"0.00"};
//mSH: Shipping and handling total
static char mSH[16]={"0.00"};
//mTax: Total tax
static char mTax[16]={"0.00"};
//cComments: Customer or our comments about order
static char *cComments={""};
//cAuthCode: Credit card processor or other authorization code
static char cAuthCode[17]={""};
//cPnRef: Online payment processor tansaction reference
static char cPnRef[33]={""};
//uAvsAddr: AVS address verified
static unsigned uAvsAddr=0;
//uAvsZip: AVS zipcode verified
static unsigned uAvsZip=0;
//uResultCode: Payment processor result code
static unsigned uResultCode=0;
//cFirstName: First Name
static char cFirstName[33]={""};
//cLastName: Last Name
static char cLastName[33]={""};
//cEmail: eMail
static char cEmail[101]={""};
//cAddr1: Street mailing address
static char cAddr1[101]={""};
//cAddr2: Street mailing address line 2
static char cAddr2[101]={""};
//cCity: City
static char cCity[101]={""};
//cState: State, Province or Locality
static char cState[101]={""};
//cZip: Postal or Zip code
static char cZip[33]={""};
//cCountry: Country
static char cCountry[65]={""};
//cCardType: Credit Card Type
static char cCardType[33]={""};
//cCardNumber: Credit Card Number
static char cCardNumber[33]={""};
//uExpMonth: Credit card expiration month
static unsigned uExpMonth=0;
//uExpYear: Credit card expiration year
static unsigned uExpYear=0;
//cCardName: Name as appears on credit card
static char cCardName[65]={""};
//cShipName: Shipping address name
static char cShipName[101]={""};
//cShipAddr1: Shipping street address
static char cShipAddr1[101]={""};
//cShipAddr2: Shipping street address line 2
static char cShipAddr2[101]={""};
//cShipCity: Shipping city
static char cShipCity[101]={""};
//cShipState: Shipping state or province
static char cShipState[101]={""};
//cShipZip: Shipping zip or postal code
static char cShipZip[33]={""};
//cShipCountry: Shipping country
static char cShipCountry[65]={""};
//cTelephone: Customer Telephone
static char cTelephone[33]={""};
//cFax: Customer Fax
static char cFax[33]={""};
//uOwner: Record owner
static unsigned uOwner=0;
//uCreatedBy: uClient for last insert
static unsigned uCreatedBy=0;
#define ISM3FIELDS
//uCreatedDate: Unix seconds date last insert
static time_t uCreatedDate=0;
//uModBy: uClient for last update
static unsigned uModBy=0;
//uModDate: Unix seconds date last update
static time_t uModDate=0;



#define VAR_LIST_tInvoice "tInvoice.uInvoice,tInvoice.uShipping,tInvoice.uPayment,tInvoice.uInvoiceStatus,tInvoice.uQueueStatus,tInvoice.uClient,tInvoice.mTotal,tInvoice.mSH,tInvoice.mTax,tInvoice.cComments,tInvoice.cAuthCode,tInvoice.cPnRef,tInvoice.uAvsAddr,tInvoice.uAvsZip,tInvoice.uResultCode,tInvoice.cFirstName,tInvoice.cLastName,tInvoice.cEmail,tInvoice.cAddr1,tInvoice.cAddr2,tInvoice.cCity,tInvoice.cState,tInvoice.cZip,tInvoice.cCountry,tInvoice.cCardType,tInvoice.cCardNumber,tInvoice.uExpMonth,tInvoice.uExpYear,tInvoice.cCardName,tInvoice.cShipName,tInvoice.cShipAddr1,tInvoice.cShipAddr2,tInvoice.cShipCity,tInvoice.cShipState,tInvoice.cShipZip,tInvoice.cShipCountry,tInvoice.cTelephone,tInvoice.cFax,tInvoice.uOwner,tInvoice.uCreatedBy,tInvoice.uCreatedDate,tInvoice.uModBy,tInvoice.uModDate"

 //Local only
void Insert_tInvoice(void);
void Update_tInvoice(char *cRowid);
void ProcesstInvoiceListVars(pentry entries[], int x);

 //In tInvoicefunc.h file included below
void ExtProcesstInvoiceVars(pentry entries[], int x);
void ExttInvoiceCommands(pentry entries[], int x);
void ExttInvoiceButtons(void);
void ExttInvoiceNavBar(void);
void ExttInvoiceGetHook(entry gentries[], int x);
void ExttInvoiceSelect(void);
void ExttInvoiceSelectRow(void);
void ExttInvoiceListSelect(void);
void ExttInvoiceListFilter(void);
void ExttInvoiceAuxTable(void);

#include "tinvoicefunc.h"

 //Table Variables Assignment Function
void ProcesstInvoiceVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uInvoice"))
			sscanf(entries[i].val,"%u",&uInvoice);
		else if(!strcmp(entries[i].name,"uShipping"))
			sscanf(entries[i].val,"%u",&uShipping);
		else if(!strcmp(entries[i].name,"cuShippingPullDown"))
		{
			sprintf(cuShippingPullDown,"%.255s",entries[i].val);
			uShipping=ReadPullDown("tShipping","cLabel",cuShippingPullDown);
		}
		else if(!strcmp(entries[i].name,"uPayment"))
			sscanf(entries[i].val,"%u",&uPayment);
		else if(!strcmp(entries[i].name,"cuPaymentPullDown"))
		{
			sprintf(cuPaymentPullDown,"%.255s",entries[i].val);
			uPayment=ReadPullDown("tPayment","cLabel",cuPaymentPullDown);
		}
		else if(!strcmp(entries[i].name,"uInvoiceStatus"))
			sscanf(entries[i].val,"%u",&uInvoiceStatus);
		else if(!strcmp(entries[i].name,"cuInvoiceStatusPullDown"))
		{
			sprintf(cuInvoiceStatusPullDown,"%.255s",entries[i].val);
			uInvoiceStatus=ReadPullDown("tInvoiceStatus","cLabel",cuInvoiceStatusPullDown);
		}
		else if(!strcmp(entries[i].name,"uQueueStatus"))
			sscanf(entries[i].val,"%u",&uQueueStatus);
		else if(!strcmp(entries[i].name,"cuQueueStatusPullDown"))
		{
			sprintf(cuQueueStatusPullDown,"%.255s",entries[i].val);
			uQueueStatus=ReadPullDown("tInvoiceStatus","cLabel",cuQueueStatusPullDown);
		}
		else if(!strcmp(entries[i].name,"uClient"))
			sscanf(entries[i].val,"%u",&uClient);
		else if(!strcmp(entries[i].name,"mTotal"))
			sprintf(mTotal,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"mSH"))
			sprintf(mSH,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"mTax"))
			sprintf(mTax,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"cComments"))
			cComments=entries[i].val;
		else if(!strcmp(entries[i].name,"cAuthCode"))
			sprintf(cAuthCode,"%.16s",entries[i].val);
		else if(!strcmp(entries[i].name,"cPnRef"))
			sprintf(cPnRef,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uAvsAddr"))
			sscanf(entries[i].val,"%u",&uAvsAddr);
		else if(!strcmp(entries[i].name,"uAvsZip"))
			sscanf(entries[i].val,"%u",&uAvsZip);
		else if(!strcmp(entries[i].name,"uResultCode"))
			sscanf(entries[i].val,"%u",&uResultCode);
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
		else if(!strcmp(entries[i].name,"cCity"))
			sprintf(cCity,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cState"))
			sprintf(cState,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cZip"))
			sprintf(cZip,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCountry"))
			sprintf(cCountry,"%.64s",entries[i].val);
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
		else if(!strcmp(entries[i].name,"cShipName"))
			sprintf(cShipName,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipAddr1"))
			sprintf(cShipAddr1,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipAddr2"))
			sprintf(cShipAddr2,"%.100s",entries[i].val);
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
		else if(!strcmp(entries[i].name,"cFax"))
			sprintf(cFax,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uOwner"))
			sscanf(entries[i].val,"%u",&uOwner);
		else if(!strcmp(entries[i].name,"uCreatedBy"))
			sscanf(entries[i].val,"%u",&uCreatedBy);
		else if(!strcmp(entries[i].name,"uCreatedDate"))
			sscanf(entries[i].val,"%lu",&uCreatedDate);
		else if(!strcmp(entries[i].name,"uModBy"))
			sscanf(entries[i].val,"%u",&uModBy);
		else if(!strcmp(entries[i].name,"uModDate"))
			sscanf(entries[i].val,"%lu",&uModDate);

	}

	//After so we can overwrite form data if needed.
	ExtProcesstInvoiceVars(entries,x);

}//ProcesstInvoiceVars()


void ProcesstInvoiceListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uInvoice);
                        guMode=2002;
                        tInvoice("");
                }
        }
}//void ProcesstInvoiceListVars(pentry entries[], int x)


int tInvoiceCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttInvoiceCommands(entries,x);

	if(!strcmp(gcFunction,"tInvoiceTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tInvoiceList();
		}

		//Default
		ProcesstInvoiceVars(entries,x);
		tInvoice("");
	}
	else if(!strcmp(gcFunction,"tInvoiceList"))
	{
		ProcessControlVars(entries,x);
		ProcesstInvoiceListVars(entries,x);
		tInvoiceList();
	}

	return(0);

}//tInvoiceCommands()


void tInvoice(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttInvoiceSelectRow();
		else
			ExttInvoiceSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetInvoice();
				unxsISP("New tInvoice table created");
                	}
			else
			{
				htmlPlainTextError(mysql_error(&gMysql));
			}
        	}

		res=mysql_store_result(&gMysql);
		if((guI=mysql_num_rows(res)))
		{
			if(guMode==6)
			{
			sprintf(gcQuery,"SELECT _rowid FROM tInvoice WHERE uInvoice=%u"
						,uInvoice);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uInvoice);
		sscanf(field[1],"%u",&uShipping);
		sscanf(field[2],"%u",&uPayment);
		sscanf(field[3],"%u",&uInvoiceStatus);
		sscanf(field[4],"%u",&uQueueStatus);
		sscanf(field[5],"%u",&uClient);
		sprintf(mTotal,"%.12s",field[6]);
		sprintf(mSH,"%.12s",field[7]);
		sprintf(mTax,"%.12s",field[8]);
		cComments=field[9];
		sprintf(cAuthCode,"%.16s",field[10]);
		sprintf(cPnRef,"%.32s",field[11]);
		sscanf(field[12],"%u",&uAvsAddr);
		sscanf(field[13],"%u",&uAvsZip);
		sscanf(field[14],"%u",&uResultCode);
		sprintf(cFirstName,"%.32s",field[15]);
		sprintf(cLastName,"%.32s",field[16]);
		sprintf(cEmail,"%.100s",field[17]);
		sprintf(cAddr1,"%.100s",field[18]);
		sprintf(cAddr2,"%.100s",field[19]);
		sprintf(cCity,"%.100s",field[20]);
		sprintf(cState,"%.100s",field[21]);
		sprintf(cZip,"%.32s",field[22]);
		sprintf(cCountry,"%.64s",field[23]);
		sprintf(cCardType,"%.32s",field[24]);
		sprintf(cCardNumber,"%.32s",field[25]);
		sscanf(field[26],"%u",&uExpMonth);
		sscanf(field[27],"%u",&uExpYear);
		sprintf(cCardName,"%.64s",field[28]);
		sprintf(cShipName,"%.100s",field[29]);
		sprintf(cShipAddr1,"%.100s",field[30]);
		sprintf(cShipAddr2,"%.100s",field[31]);
		sprintf(cShipCity,"%.100s",field[32]);
		sprintf(cShipState,"%.100s",field[33]);
		sprintf(cShipZip,"%.32s",field[34]);
		sprintf(cShipCountry,"%.64s",field[35]);
		sprintf(cTelephone,"%.32s",field[36]);
		sprintf(cFax,"%.32s",field[37]);
		sscanf(field[38],"%u",&uOwner);
		sscanf(field[39],"%u",&uCreatedBy);
		sscanf(field[40],"%lu",&uCreatedDate);
		sscanf(field[41],"%u",&uModBy);
		sscanf(field[42],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tInvoice",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tInvoiceTools>");
	printf("<input type=hidden name=gluRowid value=%lu>",gluRowid);
	if(guI)
	{
		if(guMode==6)
			//printf(" Found");
			printf(LANG_NBR_FOUND);
		else if(guMode==5)
			//printf(" Modified");
			printf(LANG_NBR_MODIFIED);
		else if(guMode==4)
			//printf(" New");
			printf(LANG_NBR_NEW);
		printf(LANG_NBRF_SHOWING,gluRowid,guI);
	}
	else
	{
		if(!cResult[0])
		//printf(" No records found");
		printf(LANG_NBR_NORECS);
	}
	if(cResult[0]) printf("%s",cResult);
	printf("</td></tr>");
	printf("<tr><td valign=top width=25%%>");

        ExttInvoiceButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tInvoice Record Data",100);

	if(guMode==2000 || guMode==2002)
		tInvoiceInput(1);
	else
		tInvoiceInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttInvoiceAuxTable();

	Footer_ism3();

}//end of tInvoice();


void tInvoiceInput(unsigned uMode)
{

//uInvoice
	OpenRow(LANG_FL_tInvoice_uInvoice,"black");
	printf("<input title='%s' type=text name=uInvoice value=%u size=16 maxlength=10 "
,LANG_FT_tInvoice_uInvoice,uInvoice);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uInvoice value=%u >\n",uInvoice);
	}
//uShipping
	OpenRow(LANG_FL_tInvoice_uShipping,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tShipping;cuShippingPullDown","cLabel","cLabel",uShipping,1);
	else
		tTablePullDown("tShipping;cuShippingPullDown","cLabel","cLabel",uShipping,0);
//uPayment
	OpenRow(LANG_FL_tInvoice_uPayment,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tPayment;cuPaymentPullDown","cLabel","cLabel",uPayment,1);
	else
		tTablePullDown("tPayment;cuPaymentPullDown","cLabel","cLabel",uPayment,0);
//uInvoiceStatus
	OpenRow(LANG_FL_tInvoice_uInvoiceStatus,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tInvoiceStatus;cuInvoiceStatusPullDown","cLabel","cLabel",uInvoiceStatus,1);
	else
		tTablePullDown("tInvoiceStatus;cuInvoiceStatusPullDown","cLabel","cLabel",uInvoiceStatus,0);
//uQueueStatus
	OpenRow(LANG_FL_tInvoice_uQueueStatus,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tInvoiceStatus;cuQueueStatusPullDown","cLabel","cLabel",uQueueStatus,1);
	else
		tTablePullDown("tInvoiceStatus;cuQueueStatusPullDown","cLabel","cLabel",uQueueStatus,0);
//uClient
	OpenRow(LANG_FL_tInvoice_uClient,"black");
	if(guPermLevel>=0 && uMode)
	{
	printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
	else
	{
	printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
//mTotal
	OpenRow(LANG_FL_tInvoice_mTotal,"black");
	printf("<input title='%s' type=text name=mTotal value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tInvoice_mTotal,EncodeDoubleQuotes(mTotal));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mTotal value=\"%s\">\n",EncodeDoubleQuotes(mTotal));
	}
//mSH
	OpenRow(LANG_FL_tInvoice_mSH,"black");
	printf("<input title='%s' type=text name=mSH value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tInvoice_mSH,EncodeDoubleQuotes(mSH));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mSH value=\"%s\">\n",EncodeDoubleQuotes(mSH));
	}
//mTax
	OpenRow(LANG_FL_tInvoice_mTax,"black");
	printf("<input title='%s' type=text name=mTax value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tInvoice_mTax,EncodeDoubleQuotes(mTax));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mTax value=\"%s\">\n",EncodeDoubleQuotes(mTax));
	}
//cComments
	OpenRow(LANG_FL_tInvoice_cComments,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cComments "
,LANG_FT_tInvoice_cComments);
	if(guPermLevel>=0 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cComments);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cComments);
		printf("<input type=hidden name=cComments value=\"%s\" >\n",EncodeDoubleQuotes(cComments));
	}
//cAuthCode
	OpenRow(LANG_FL_tInvoice_cAuthCode,"black");
	printf("<input title='%s' type=text name=cAuthCode value=\"%s\" size=40 maxlength=16 "
,LANG_FT_tInvoice_cAuthCode,EncodeDoubleQuotes(cAuthCode));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cAuthCode value=\"%s\">\n",EncodeDoubleQuotes(cAuthCode));
	}
//cPnRef
	OpenRow(LANG_FL_tInvoice_cPnRef,"black");
	printf("<input title='%s' type=text name=cPnRef value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tInvoice_cPnRef,EncodeDoubleQuotes(cPnRef));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cPnRef value=\"%s\">\n",EncodeDoubleQuotes(cPnRef));
	}
//uAvsAddr
	OpenRow(LANG_FL_tInvoice_uAvsAddr,"black");
	printf("<input title='%s' type=text name=uAvsAddr value=%u size=16 maxlength=10 "
,LANG_FT_tInvoice_uAvsAddr,uAvsAddr);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uAvsAddr value=%u >\n",uAvsAddr);
	}
//uAvsZip
	OpenRow(LANG_FL_tInvoice_uAvsZip,"black");
	printf("<input title='%s' type=text name=uAvsZip value=%u size=16 maxlength=10 "
,LANG_FT_tInvoice_uAvsZip,uAvsZip);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uAvsZip value=%u >\n",uAvsZip);
	}
//uResultCode
	OpenRow(LANG_FL_tInvoice_uResultCode,"black");
	printf("<input title='%s' type=text name=uResultCode value=%u size=16 maxlength=10 "
,LANG_FT_tInvoice_uResultCode,uResultCode);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uResultCode value=%u >\n",uResultCode);
	}
//cFirstName
	OpenRow(LANG_FL_tInvoice_cFirstName,"black");
	printf("<input title='%s' type=text name=cFirstName value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tInvoice_cFirstName,EncodeDoubleQuotes(cFirstName));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cFirstName value=\"%s\">\n",EncodeDoubleQuotes(cFirstName));
	}
//cLastName
	OpenRow(LANG_FL_tInvoice_cLastName,"black");
	printf("<input title='%s' type=text name=cLastName value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tInvoice_cLastName,EncodeDoubleQuotes(cLastName));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLastName value=\"%s\">\n",EncodeDoubleQuotes(cLastName));
	}
//cEmail
	OpenRow(LANG_FL_tInvoice_cEmail,"black");
	printf("<input title='%s' type=text name=cEmail value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tInvoice_cEmail,EncodeDoubleQuotes(cEmail));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cEmail value=\"%s\">\n",EncodeDoubleQuotes(cEmail));
	}
//cAddr1
	OpenRow(LANG_FL_tInvoice_cAddr1,"black");
	printf("<input title='%s' type=text name=cAddr1 value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tInvoice_cAddr1,EncodeDoubleQuotes(cAddr1));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cAddr1 value=\"%s\">\n",EncodeDoubleQuotes(cAddr1));
	}
//cAddr2
	OpenRow(LANG_FL_tInvoice_cAddr2,"black");
	printf("<input title='%s' type=text name=cAddr2 value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tInvoice_cAddr2,EncodeDoubleQuotes(cAddr2));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cAddr2 value=\"%s\">\n",EncodeDoubleQuotes(cAddr2));
	}
//cCity
	OpenRow(LANG_FL_tInvoice_cCity,"black");
	printf("<input title='%s' type=text name=cCity value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tInvoice_cCity,EncodeDoubleQuotes(cCity));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cCity value=\"%s\">\n",EncodeDoubleQuotes(cCity));
	}
//cState
	OpenRow(LANG_FL_tInvoice_cState,"black");
	printf("<input title='%s' type=text name=cState value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tInvoice_cState,EncodeDoubleQuotes(cState));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cState value=\"%s\">\n",EncodeDoubleQuotes(cState));
	}
//cZip
	OpenRow(LANG_FL_tInvoice_cZip,"black");
	printf("<input title='%s' type=text name=cZip value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tInvoice_cZip,EncodeDoubleQuotes(cZip));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cZip value=\"%s\">\n",EncodeDoubleQuotes(cZip));
	}
//cCountry
	OpenRow(LANG_FL_tInvoice_cCountry,"black");
	printf("<input title='%s' type=text name=cCountry value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tInvoice_cCountry,EncodeDoubleQuotes(cCountry));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cCountry value=\"%s\">\n",EncodeDoubleQuotes(cCountry));
	}
//cCardType
	OpenRow(LANG_FL_tInvoice_cCardType,"black");
	printf("<input title='%s' type=text name=cCardType value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tInvoice_cCardType,EncodeDoubleQuotes(cCardType));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cCardType value=\"%s\">\n",EncodeDoubleQuotes(cCardType));
	}
//cCardNumber
	OpenRow(LANG_FL_tInvoice_cCardNumber,"black");
	printf("<input title='%s' type=text name=cCardNumber value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tInvoice_cCardNumber,EncodeDoubleQuotes(cCardNumber));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cCardNumber value=\"%s\">\n",EncodeDoubleQuotes(cCardNumber));
	}
//uExpMonth
	OpenRow(LANG_FL_tInvoice_uExpMonth,"black");
	printf("<input title='%s' type=text name=uExpMonth value=%u size=16 maxlength=10 "
,LANG_FT_tInvoice_uExpMonth,uExpMonth);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uExpMonth value=%u >\n",uExpMonth);
	}
//uExpYear
	OpenRow(LANG_FL_tInvoice_uExpYear,"black");
	printf("<input title='%s' type=text name=uExpYear value=%u size=16 maxlength=10 "
,LANG_FT_tInvoice_uExpYear,uExpYear);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uExpYear value=%u >\n",uExpYear);
	}
//cCardName
	OpenRow(LANG_FL_tInvoice_cCardName,"black");
	printf("<input title='%s' type=text name=cCardName value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tInvoice_cCardName,EncodeDoubleQuotes(cCardName));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cCardName value=\"%s\">\n",EncodeDoubleQuotes(cCardName));
	}
//cShipName
	OpenRow(LANG_FL_tInvoice_cShipName,"black");
	printf("<input title='%s' type=text name=cShipName value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tInvoice_cShipName,EncodeDoubleQuotes(cShipName));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipName value=\"%s\">\n",EncodeDoubleQuotes(cShipName));
	}
//cShipAddr1
	OpenRow(LANG_FL_tInvoice_cShipAddr1,"black");
	printf("<input title='%s' type=text name=cShipAddr1 value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tInvoice_cShipAddr1,EncodeDoubleQuotes(cShipAddr1));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipAddr1 value=\"%s\">\n",EncodeDoubleQuotes(cShipAddr1));
	}
//cShipAddr2
	OpenRow(LANG_FL_tInvoice_cShipAddr2,"black");
	printf("<input title='%s' type=text name=cShipAddr2 value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tInvoice_cShipAddr2,EncodeDoubleQuotes(cShipAddr2));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipAddr2 value=\"%s\">\n",EncodeDoubleQuotes(cShipAddr2));
	}
//cShipCity
	OpenRow(LANG_FL_tInvoice_cShipCity,"black");
	printf("<input title='%s' type=text name=cShipCity value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tInvoice_cShipCity,EncodeDoubleQuotes(cShipCity));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipCity value=\"%s\">\n",EncodeDoubleQuotes(cShipCity));
	}
//cShipState
	OpenRow(LANG_FL_tInvoice_cShipState,"black");
	printf("<input title='%s' type=text name=cShipState value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tInvoice_cShipState,EncodeDoubleQuotes(cShipState));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipState value=\"%s\">\n",EncodeDoubleQuotes(cShipState));
	}
//cShipZip
	OpenRow(LANG_FL_tInvoice_cShipZip,"black");
	printf("<input title='%s' type=text name=cShipZip value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tInvoice_cShipZip,EncodeDoubleQuotes(cShipZip));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipZip value=\"%s\">\n",EncodeDoubleQuotes(cShipZip));
	}
//cShipCountry
	OpenRow(LANG_FL_tInvoice_cShipCountry,"black");
	printf("<input title='%s' type=text name=cShipCountry value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tInvoice_cShipCountry,EncodeDoubleQuotes(cShipCountry));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipCountry value=\"%s\">\n",EncodeDoubleQuotes(cShipCountry));
	}
//cTelephone
	OpenRow(LANG_FL_tInvoice_cTelephone,"black");
	printf("<input title='%s' type=text name=cTelephone value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tInvoice_cTelephone,EncodeDoubleQuotes(cTelephone));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cTelephone value=\"%s\">\n",EncodeDoubleQuotes(cTelephone));
	}
//cFax
	OpenRow(LANG_FL_tInvoice_cFax,"black");
	printf("<input title='%s' type=text name=cFax value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tInvoice_cFax,EncodeDoubleQuotes(cFax));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cFax value=\"%s\">\n",EncodeDoubleQuotes(cFax));
	}
//uOwner
	OpenRow(LANG_FL_tInvoice_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tInvoice_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tInvoice_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tInvoice_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tInvoice_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tInvoiceInput(unsigned uMode)


void NewtInvoice(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uInvoice FROM tInvoice\
				WHERE uInvoice=%u"
							,uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tInvoice("<blink>Record already exists");
		tInvoice(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tInvoice();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uInvoice=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tInvoice",uInvoice);
	unxsISPLog(uInvoice,"tInvoice","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uInvoice);
	tInvoice(gcQuery);
	}

}//NewtInvoice(unsigned uMode)


void DeletetInvoice(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tInvoice WHERE uInvoice=%u AND ( uOwner=%u OR %u>9 )"
					,uInvoice,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tInvoice WHERE uInvoice=%u"
					,uInvoice);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tInvoice("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uInvoice,"tInvoice","Del");
#endif
		tInvoice(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uInvoice,"tInvoice","DelError");
#endif
		tInvoice(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetInvoice(void)


void Insert_tInvoice(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tInvoice SET uInvoice=%u,uShipping=%u,uPayment=%u,uInvoiceStatus=%u,uQueueStatus=%u,uClient=%u,mTotal='%s',mSH='%s',mTax='%s',cComments='%s',cAuthCode='%s',cPnRef='%s',uAvsAddr=%u,uAvsZip=%u,uResultCode=%u,cFirstName='%s',cLastName='%s',cEmail='%s',cAddr1='%s',cAddr2='%s',cCity='%s',cState='%s',cZip='%s',cCountry='%s',cCardType='%s',cCardNumber='%s',uExpMonth=%u,uExpYear=%u,cCardName='%s',cShipName='%s',cShipAddr1='%s',cShipAddr2='%s',cShipCity='%s',cShipState='%s',cShipZip='%s',cShipCountry='%s',cTelephone='%s',cFax='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uInvoice
			,uShipping
			,uPayment
			,uInvoiceStatus
			,uQueueStatus
			,uClient
			,TextAreaSave(mTotal)
			,TextAreaSave(mSH)
			,TextAreaSave(mTax)
			,TextAreaSave(cComments)
			,TextAreaSave(cAuthCode)
			,TextAreaSave(cPnRef)
			,uAvsAddr
			,uAvsZip
			,uResultCode
			,TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cEmail)
			,TextAreaSave(cAddr1)
			,TextAreaSave(cAddr2)
			,TextAreaSave(cCity)
			,TextAreaSave(cState)
			,TextAreaSave(cZip)
			,TextAreaSave(cCountry)
			,TextAreaSave(cCardType)
			,TextAreaSave(cCardNumber)
			,uExpMonth
			,uExpYear
			,TextAreaSave(cCardName)
			,TextAreaSave(cShipName)
			,TextAreaSave(cShipAddr1)
			,TextAreaSave(cShipAddr2)
			,TextAreaSave(cShipCity)
			,TextAreaSave(cShipState)
			,TextAreaSave(cShipZip)
			,TextAreaSave(cShipCountry)
			,TextAreaSave(cTelephone)
			,TextAreaSave(cFax)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tInvoice(void)


void Update_tInvoice(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tInvoice SET uInvoice=%u,uShipping=%u,uPayment=%u,uInvoiceStatus=%u,uQueueStatus=%u,uClient=%u,mTotal='%s',mSH='%s',mTax='%s',cComments='%s',cAuthCode='%s',cPnRef='%s',uAvsAddr=%u,uAvsZip=%u,uResultCode=%u,cFirstName='%s',cLastName='%s',cEmail='%s',cAddr1='%s',cAddr2='%s',cCity='%s',cState='%s',cZip='%s',cCountry='%s',cCardType='%s',cCardNumber='%s',uExpMonth=%u,uExpYear=%u,cCardName='%s',cShipName='%s',cShipAddr1='%s',cShipAddr2='%s',cShipCity='%s',cShipState='%s',cShipZip='%s',cShipCountry='%s',cTelephone='%s',cFax='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uInvoice
			,uShipping
			,uPayment
			,uInvoiceStatus
			,uQueueStatus
			,uClient
			,TextAreaSave(mTotal)
			,TextAreaSave(mSH)
			,TextAreaSave(mTax)
			,TextAreaSave(cComments)
			,TextAreaSave(cAuthCode)
			,TextAreaSave(cPnRef)
			,uAvsAddr
			,uAvsZip
			,uResultCode
			,TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cEmail)
			,TextAreaSave(cAddr1)
			,TextAreaSave(cAddr2)
			,TextAreaSave(cCity)
			,TextAreaSave(cState)
			,TextAreaSave(cZip)
			,TextAreaSave(cCountry)
			,TextAreaSave(cCardType)
			,TextAreaSave(cCardNumber)
			,uExpMonth
			,uExpYear
			,TextAreaSave(cCardName)
			,TextAreaSave(cShipName)
			,TextAreaSave(cShipAddr1)
			,TextAreaSave(cShipAddr2)
			,TextAreaSave(cShipCity)
			,TextAreaSave(cShipState)
			,TextAreaSave(cShipZip)
			,TextAreaSave(cShipCountry)
			,TextAreaSave(cTelephone)
			,TextAreaSave(cFax)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tInvoice(void)


void ModtInvoice(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uInvoice,uModDate FROM tInvoice WHERE uInvoice=%u"
						,uInvoice);
#else
	sprintf(gcQuery,"SELECT uInvoice FROM tInvoice WHERE uInvoice=%u"
						,uInvoice);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tInvoice("<blink>Record does not exist");
	if(i<1) tInvoice(LANG_NBR_RECNOTEXIST);
	//if(i>1) tInvoice("<blink>Multiple rows!");
	if(i>1) tInvoice(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tInvoice(LANG_NBR_EXTMOD);
#endif

	Update_tInvoice(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tInvoice",uInvoice);
	unxsISPLog(uInvoice,"tInvoice","Mod");
#endif
	tInvoice(gcQuery);

}//ModtInvoice(void)


void tInvoiceList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttInvoiceListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tInvoiceList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttInvoiceListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uInvoice<td><font face=arial,helvetica color=white>uShipping<td><font face=arial,helvetica color=white>uPayment<td><font face=arial,helvetica color=white>uInvoiceStatus<td><font face=arial,helvetica color=white>uQueueStatus<td><font face=arial,helvetica color=white>uClient<td><font face=arial,helvetica color=white>mTotal<td><font face=arial,helvetica color=white>mSH<td><font face=arial,helvetica color=white>mTax<td><font face=arial,helvetica color=white>cComments<td><font face=arial,helvetica color=white>cAuthCode<td><font face=arial,helvetica color=white>cPnRef<td><font face=arial,helvetica color=white>uAvsAddr<td><font face=arial,helvetica color=white>uAvsZip<td><font face=arial,helvetica color=white>uResultCode<td><font face=arial,helvetica color=white>cFirstName<td><font face=arial,helvetica color=white>cLastName<td><font face=arial,helvetica color=white>cEmail<td><font face=arial,helvetica color=white>cAddr1<td><font face=arial,helvetica color=white>cAddr2<td><font face=arial,helvetica color=white>cCity<td><font face=arial,helvetica color=white>cState<td><font face=arial,helvetica color=white>cZip<td><font face=arial,helvetica color=white>cCountry<td><font face=arial,helvetica color=white>cCardType<td><font face=arial,helvetica color=white>cCardNumber<td><font face=arial,helvetica color=white>uExpMonth<td><font face=arial,helvetica color=white>uExpYear<td><font face=arial,helvetica color=white>cCardName<td><font face=arial,helvetica color=white>cShipName<td><font face=arial,helvetica color=white>cShipAddr1<td><font face=arial,helvetica color=white>cShipAddr2<td><font face=arial,helvetica color=white>cShipCity<td><font face=arial,helvetica color=white>cShipState<td><font face=arial,helvetica color=white>cShipZip<td><font face=arial,helvetica color=white>cShipCountry<td><font face=arial,helvetica color=white>cTelephone<td><font face=arial,helvetica color=white>cFax<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



	mysql_data_seek(res,guStart-1);

	for(guN=0;guN<(guEnd-guStart+1);guN++)
	{
		field=mysql_fetch_row(res);
		if(!field)
		{
			printf("<tr><td><font face=arial,helvetica>End of data</table>");
			Footer_ism3();
		}
			if(guN % 2)
				printf("<tr bgcolor=#BBE1D3>");
			else
				printf("<tr>");
		time_t luTime40=strtoul(field[40],NULL,10);
		char cBuf40[32];
		if(luTime40)
			ctime_r(&luTime40,cBuf40);
		else
			sprintf(cBuf40,"---");
		time_t luTime42=strtoul(field[42],NULL,10);
		char cBuf42[32];
		if(luTime42)
			ctime_r(&luTime42,cBuf42);
		else
			sprintf(cBuf42,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,ForeignKey("tShipping","cLabel",strtoul(field[1],NULL,10))
			,ForeignKey("tPayment","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tInvoiceStatus","cLabel",strtoul(field[3],NULL,10))
			,ForeignKey("tInvoiceStatus","cLabel",strtoul(field[4],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,field[6]
			,field[7]
			,field[8]
			,field[9]
			,field[10]
			,field[11]
			,field[12]
			,field[13]
			,field[14]
			,field[15]
			,field[16]
			,field[17]
			,field[18]
			,field[19]
			,field[20]
			,field[21]
			,field[22]
			,field[23]
			,field[24]
			,field[25]
			,field[26]
			,field[27]
			,field[28]
			,field[29]
			,field[30]
			,field[31]
			,field[32]
			,field[33]
			,field[34]
			,field[35]
			,field[36]
			,field[37]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[38],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[39],NULL,10))
			,cBuf40
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[41],NULL,10))
			,cBuf42
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tInvoiceList()


void CreatetInvoice(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tInvoice ( uClient INT UNSIGNED NOT NULL DEFAULT 0,index (uClient), cAuthCode VARCHAR(16) NOT NULL DEFAULT '', cComments TEXT NOT NULL DEFAULT '', uInvoice INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uInvoiceStatus INT UNSIGNED NOT NULL DEFAULT 0, mTotal DECIMAL(10,2) NOT NULL DEFAULT 0, mSH DECIMAL(10,2) NOT NULL DEFAULT 0, mTax DECIMAL(10,2) NOT NULL DEFAULT 0, uShipping INT UNSIGNED NOT NULL DEFAULT 0, cPnRef VARCHAR(32) NOT NULL DEFAULT '', uAvsAddr INT UNSIGNED NOT NULL DEFAULT 0, uAvsZip INT UNSIGNED NOT NULL DEFAULT 0, uResultCode INT UNSIGNED NOT NULL DEFAULT 0, cFirstName VARCHAR(32) NOT NULL DEFAULT '', cLastName VARCHAR(32) NOT NULL DEFAULT '', cEmail VARCHAR(100) NOT NULL DEFAULT '', cAddr1 VARCHAR(100) NOT NULL DEFAULT '', cAddr2 VARCHAR(100) NOT NULL DEFAULT '', cCity VARCHAR(100) NOT NULL DEFAULT '', cState VARCHAR(100) NOT NULL DEFAULT '', cZip VARCHAR(32) NOT NULL DEFAULT '', cCountry VARCHAR(64) NOT NULL DEFAULT '', cCardType VARCHAR(32) NOT NULL DEFAULT '', cCardNumber VARCHAR(32) NOT NULL DEFAULT '', uExpMonth INT UNSIGNED NOT NULL DEFAULT 0, uExpYear INT UNSIGNED NOT NULL DEFAULT 0, cCardName VARCHAR(64) NOT NULL DEFAULT '', cShipName VARCHAR(100) NOT NULL DEFAULT '', cShipAddr1 VARCHAR(100) NOT NULL DEFAULT '', cShipAddr2 VARCHAR(100) NOT NULL DEFAULT '', cShipCity VARCHAR(100) NOT NULL DEFAULT '', cShipState VARCHAR(100) NOT NULL DEFAULT '', cShipZip VARCHAR(32) NOT NULL DEFAULT '', cShipCountry VARCHAR(64) NOT NULL DEFAULT '', cTelephone VARCHAR(32) NOT NULL DEFAULT '', cFax VARCHAR(32) NOT NULL DEFAULT '', uPayment INT UNSIGNED NOT NULL DEFAULT 0, uQueueStatus INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetInvoice()

