/*
FILE
	svn ID removed
PURPOSE

	Archive RO myISAM tables and be able to use them at will.

AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details
	See LICENSE file included.

TODO
	Needs customer-reseller-admin-root system installed asap.

	Check Total issue should all be 0 in here. Never allow archiving non paid invoices?

 
*/

//Protos
time_t cDateToUnixTime(char *cDate);
//void htmlMonthYearSelectTable(void);

//there is another similar gcFunction in tinstancefunc.h
void CreatetInvoiceMonthTable(char *cTableName);

//Extern
char *strptime(char *buf, const char *format, const struct tm *tm);//system problem
time_t cDateToUnixTime(char *cDate);//tinstancefunc.h

//Data
time_t cDateToUnixTime(char *cDate);
static char cSearch[100]={""};

static unsigned uMonth=0;//not month number, but tMonth table index!
static char cuMonthPullDown[33]={""};//Has complete table name. See mainfunc.h Archive...

void InvoiceMonthItemList(unsigned uInvoice);

void ExtProcesstInvoiceMonthVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{

		if(!strcmp(entries[i].name,"cuMonthPullDown"))
		{
			strcpy(cuMonthPullDown,entries[i].val);
			uMonth=ReadPullDown("tMonth","cLabel",cuMonthPullDown);
		}
		else if(!strcmp(entries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.99s",entries[i].val);
		}

	}
}//void ExtProcesstInvoiceMonthVars(pentry entries[], int x)


void ExttInvoiceMonthCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tInvoiceMonthTools"))
	{
		if(!strcmp(gcCommand,"UnLoad"))
		{
			sprintf(gcQuery,"DELETE FROM tInvoiceMonth");
		        mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tInvoiceMonth(mysql_error(&gMysql));
		}
		else if(!strcmp(gcCommand,"Load"))
		{
			ExtProcesstInvoiceMonthVars(entries,x);

			if(!uMonth)
				tInvoiceMonth("Must specify valid month table");

			sprintf(gcQuery,"DELETE FROM tInvoiceMonth");
		        mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tInvoiceMonth(mysql_error(&gMysql));

			sprintf(gcQuery,"INSERT DELAYED tInvoiceMonth (cAuthCode,cComments,uInvoice,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate,uInvoiceStatus,mTotal,mSH,mTax,uShipping,cPnRef,uAvsAddr,uAvsZip,uResultCode,cFirstName,cLastName,cEmail,cAddr1,cAddr2,cCity,cState,cZip,cCountry,cCardType,cCardNumber,uExpMonth,uExpYear,cCardName,cShipName,cShipAddr1,cShipAddr2,cShipCity,cShipState,cShipZip,cShipCountry,cTelephone,cFax,uPayment,uClient) SELECT cAuthCode,cComments,uInvoice,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate,uInvoiceStatus,mTotal,mSH,mTax,uShipping,cPnRef,uAvsAddr,uAvsZip,uResultCode,cFirstName,cLastName,cEmail,cAddr1,cAddr2,cCity,cState,cZip,cCountry,cCardType,cCardNumber,uExpMonth,uExpYear,cCardName,cShipName,cShipAddr1,cShipAddr2,cShipCity,cShipState,cShipZip,cShipCountry,cTelephone,cFax,uPayment,uClient FROM %s",cuMonthPullDown);
		        mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tInvoiceMonth(mysql_error(&gMysql));

		}
		else if(!strcmp(gcCommand,"Customer Report"))
		{
			ProcesstInvoiceMonthVars(entries,x);
			tInvoiceMonth("Not implemented yet");
		}

	}

}//void ExttInvoiceMonthCommands(pentry entries[], int x)


void ExttInvoiceMonthButtons(void)
{
	OpenFieldSet("tInvoiceMonth Aux Panel",100);
	
	printf("<u>Archived Data Available</u><br>");
	tTablePullDown("tMonth;cuMonthPullDown","cLabel","cLabel",uMonth,1);

	printf("<p><u>Search Tools</u><br>");
	printf("<input title='Enter uClient or partial tClient.cLabel (customer name.) You may use %% and _ wildcards. For loaded month table data only.' type=text name=cSearch value=%s>",cSearch);

	printf("<p><u>tInvoiceMonth Table Ops</u><br>");
	printf("<input class=lalertButton title='Load compressed month data from pull down into this table for reports and other general queries' type=submit name=gcCommand value='Load'><br>");
	printf("<input class=lalertButton title='Remove data from tInvoiceMonth' type=submit name=gcCommand value='UnLoad'>");


	printf("<p><u>Report Tools</u><br>");

printf("<input title='Single client in-depth report. Mon/Year drop-down and cSearch apply if used' class=largeButton type=submit name=gcCommand value='Customer Report'><br>");

	if(uInvoice)
	{
		InvoiceMonthItemList(uInvoice);
	}

	CloseFieldSet();

}//void ExttInvoiceMonthButtons(void)


void ExttInvoiceMonthAuxTable(void)
{

}//void ExttInvoiceMonthAuxTable(void)


void ExttInvoiceMonthGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uInvoice"))
		{
			sscanf(gentries[i].val,"%u",&uInvoice);
			guMode=6;
		}
	}
	tInvoiceMonth("");

}//void ExttInvoiceMonthGetHook(entry gentries[], int x)


void ExttInvoiceMonthSelect(void)
{
	unsigned uSearch=0;

	sscanf(cSearch,"%u",&uSearch);

	if(uSearch)	
		sprintf(gcQuery,"SELECT %s FROM tInvoiceMonth WHERE uClient=%u ORDER BY uInvoice",
						VAR_LIST_tInvoiceMonth,uSearch);
	else if(cSearch[0])
		sprintf(gcQuery,"SELECT %s FROM tInvoiceMonth,tClient WHERE tInvoiceMonth.uClient=tClient.uClient AND tClient.cLabel LIKE '%s%%' ORDER BY tClient.cLabel",
						VAR_LIST_tInvoiceMonth,cSearch);
	else
		sprintf(gcQuery,"SELECT %s FROM tInvoiceMonth ORDER BY uInvoice",
						VAR_LIST_tInvoiceMonth);


}//void ExttInvoiceMonthSelect(void)


void ExttInvoiceMonthSelectRow(void)
{
	sprintf(gcQuery,"SELECT %s FROM tInvoiceMonth WHERE uInvoice=%u",
			VAR_LIST_tInvoiceMonth,uInvoice);

}//void ExttInvoiceMonthSelectRow(void)


void ExttInvoiceMonthListSelect(void)
{
	char cCat[512];

	sprintf(gcQuery,"SELECT %s FROM tInvoiceMonth",
				VAR_LIST_tInvoiceMonth);

	//Changes here must be reflected below in ExttInvoiceMonthListFilter()
        if(!strcmp(gcFilter,"uInvoice"))
        {
                sscanf(gcCommand,"%u",&uInvoice);
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"uInvoice=%u",uInvoice);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uClient"))
        {
                sscanf(gcCommand,"%u",&uClient);
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"uClient=%u",uClient);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uInvoice");
        }

}//void ExttInvoiceMonthListSelect(void)


void ExttInvoiceMonthListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uInvoice"))
                printf("<option>uInvoice</option>");
        else
                printf("<option selected>uInvoice</option>");
        if(strcmp(gcFilter,"uClient"))
                printf("<option>uClient</option>");
        else
                printf("<option selected>uClient</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttInvoiceMonthListFilter(void)


void ExttInvoiceMonthNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>7)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttInvoiceMonthNavBar(void)


//Insure exactly same schema as tInvoice see tinvoice.c and tinvoicemonth.c
//also check mainfunc.h Archive...
void CreatetInvoiceMonthTable(char *cTableName)
{
	sprintf(gcQuery,"CREATE TABLE %s ( cAuthCode VARCHAR(16) NOT NULL DEFAULT '', cComments TEXT NOT NULL DEFAULT '', uInvoice INT UNSIGNED DEFAULT 1 PRIMARY KEY AUTO_INCREMENT, uOwner INT UNSIGNED NOT NULL DEFAULT 0,INDEX (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uInvoiceStatus INT UNSIGNED NOT NULL DEFAULT 0, mTotal DECIMAL(10,2) NOT NULL DEFAULT 0, mSH DECIMAL(10,2) NOT NULL DEFAULT 0, mTax DECIMAL(10,2) NOT NULL DEFAULT 0, uShipping INT UNSIGNED NOT NULL DEFAULT 0, cPnRef VARCHAR(32) NOT NULL DEFAULT '', uAvsAddr INT UNSIGNED NOT NULL DEFAULT 0, uAvsZip INT UNSIGNED NOT NULL DEFAULT 0, uResultCode INT UNSIGNED NOT NULL DEFAULT 0, cFirstName VARCHAR(32) NOT NULL DEFAULT '', cLastName VARCHAR(32) NOT NULL DEFAULT '', cEmail VARCHAR(100) NOT NULL DEFAULT '', cAddr1 VARCHAR(100) NOT NULL DEFAULT '', cAddr2 VARCHAR(100) NOT NULL DEFAULT '', cCity VARCHAR(100) NOT NULL DEFAULT '', cState VARCHAR(100) NOT NULL DEFAULT '', cZip VARCHAR(32) NOT NULL DEFAULT '', cCountry VARCHAR(64) NOT NULL DEFAULT '', cCardType VARCHAR(32) NOT NULL DEFAULT '', cCardNumber VARCHAR(32) NOT NULL DEFAULT '', uExpMonth INT UNSIGNED NOT NULL DEFAULT 0, uExpYear INT UNSIGNED NOT NULL DEFAULT 0, cCardName VARCHAR(64) NOT NULL DEFAULT '', cShipName VARCHAR(100) NOT NULL DEFAULT '', cShipAddr1 VARCHAR(100) NOT NULL DEFAULT '', cShipAddr2 VARCHAR(100) NOT NULL DEFAULT '', cShipCity VARCHAR(100) NOT NULL DEFAULT '', cShipState VARCHAR(100) NOT NULL DEFAULT '', cShipZip VARCHAR(32) NOT NULL DEFAULT '', cShipCountry VARCHAR(64) NOT NULL DEFAULT '', cTelephone VARCHAR(32) NOT NULL DEFAULT '', cFax VARCHAR(32) NOT NULL DEFAULT '', uPayment INT UNSIGNED NOT NULL DEFAULT 0, uClient INT UNSIGNED NOT NULL DEFAULT 0,INDEX (uClient), uQueueStatus INT UNSIGNED NOT NULL DEFAULT 0, mPaid DECIMAL(10,2) NOT NULL DEFAULT 0, INDEX (uQueueStatus), INDEX (uInvoiceStatus) )",cTableName);
	mysql_query(&gMysql,gcQuery);

}//CreatetInvoiceMonthTable()

//Insure exactly same schema as tInvoiceItems see tinvoiceitems.c 
//also check mainfunc.h Archive...
void CreatetInvoiceItemsMonthTable(char *cTableName)
{
	sprintf(gcQuery,"CREATE TABLE %s ( uInvoiceItems int unsigned DEFAULT 1 PRIMARY KEY AUTO_INCREMENT, uInvoice int unsigned NOT NULL DEFAULT 0,index (uInvoice), uOwner int unsigned NOT NULL DEFAULT 0,index (uOwner), uCreatedBy int unsigned NOT NULL DEFAULT 0, uCreatedDate int unsigned NOT NULL DEFAULT 0, uModBy int unsigned NOT NULL DEFAULT 0, uModDate int unsigned NOT NULL DEFAULT 0, uClient int unsigned NOT NULL DEFAULT 0, uProduct int unsigned NOT NULL DEFAULT 0,index (uProduct), uQuantity int unsigned NOT NULL DEFAULT 0, mTax decimal(10,2) NOT NULL DEFAULT 0, mTotal decimal(10,2) NOT NULL DEFAULT 0, mSH decimal(10,2) NOT NULL DEFAULT 0, mPrice decimal(10,2) NOT NULL DEFAULT 0,cLabel varchar(255) NOT NULL DEFAULT '', uInstance int unsigned NOT NULL DEFAULT 0, index (uInstance) )",
	cTableName);
	mysql_query(&gMysql,gcQuery);

}//CreatetInvoiceItemsMonthTable()

//Insure exactly same schema as tPaid see tpaid.c 
//also check mainfunc.h Archive...
void CreatetPaidMonthTable(char *cTableName)
{
	sprintf(gcQuery,"CREATE TABLE %s ( uPaid int unsigned DEFAULT 1 PRIMARY KEY AUTO_INCREMENT, uPayment int unsigned NOT NULL DEFAULT 0, uOwner int unsigned NOT NULL DEFAULT 0,index (uOwner), uCreatedBy int unsigned NOT NULL DEFAULT 0, uCreatedDate int unsigned NOT NULL DEFAULT 0, uModBy int unsigned NOT NULL DEFAULT 0, uModDate int unsigned NOT NULL DEFAULT 0, mAmount decimal(10,2) NOT NULL DEFAULT 0, uInvoice int unsigned NOT NULL DEFAULT 0,index (uInvoice), uClient int unsigned NOT NULL DEFAULT 0, cComment text NOT NULL DEFAULT '', index (uClient) )",cTableName);
	mysql_query(&gMysql,gcQuery);

}//CreatetPaidMonthTable()


void InvoiceMonthItemList(unsigned uInvoice)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCount=0;
	double dTotal=0.0;
	double dPrice=0.0;
	char cMonth[4]={"Jun"};
	char cYear[5]={"2004"};


sprintf(gcQuery,"SELECT FROM_UNIXTIME(uCreatedDate,'%%b'),FROM_UNIXTIME(uCreatedDate,'%%Y') FROM tInvoiceMonth LIMIT 1");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cMonth,"%.3s",field[0]);
		sprintf(cYear,"%.4s",field[1]);
	}
	else
	{
		printf("No data in tInvoiceMonth<br>\n");
		mysql_free_result(res);
		return;
	}
	mysql_free_result(res);

sprintf(gcQuery,"SELECT tInvoiceItems%s%s.uInvoiceItems,tInvoiceItems%s%s.mPrice,tProduct.cLabel,tInvoiceItems%s%s.cLabel FROM tInvoiceItems%s%s,tProduct WHERE tProduct.uProduct=tInvoiceItems%s%s.uProduct AND tInvoiceItems%s%s.uInvoice=%u",cMonth,cYear,cMonth,cYear,cMonth,cYear,cMonth,cYear,cMonth,cYear,cMonth,cYear,uInvoice);
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
		printf("<p><u>InvoiceMonth Items</u><br>");

		while((field=mysql_fetch_row(res)))
		{
			uCount++;

			sscanf(field[1],"%lf",&dPrice);
			dTotal+=dPrice;

			printf("%u-. %s. $%s</a><blockquote>%s</blockquote>\n",
				uCount,field[2],field[1],field[3]);
		}
		mysql_free_result(res);

		printf("<u>Total</u><br>%2.2f\n",dTotal);

		if(uClient)
			printf("<p><a class=darkLink href=unxsISP.cgi?gcFunction=tClient&uClient=%u>[Client]</a><br>\n",uClient);

	}

}//void InvoiceMonthItemList(unsigned uInvoice)

