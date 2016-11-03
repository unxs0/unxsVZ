/*
FILE
	tInvoiceItems source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tinvoiceitemsfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uInvoiceItems: Primary Key
static unsigned uInvoiceItems=0;
//uInvoice: FKey
static unsigned uInvoice=0;
//uClient: FKey
static unsigned uClient=0;
//uProduct: FKey
static unsigned uProduct=0;
//uQuantity: Quantity of uProduct
static unsigned uQuantity=0;
//mPrice: Price at purchase time
static char mPrice[16]={"0.00"};
//mTax: Tax for this item (x uQuantity) if applicable
static char mTax[16]={"0.00"};
//mSH: Shipping and handling for this item
static char mSH[16]={"0.00"};
//mTotal: Total for this line: Price at purchase x quan + mTax + mSH
static char mTotal[16]={"0.00"};
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



#define VAR_LIST_tInvoiceItems "tInvoiceItems.uInvoiceItems,tInvoiceItems.uInvoice,tInvoiceItems.uClient,tInvoiceItems.uProduct,tInvoiceItems.uQuantity,tInvoiceItems.mPrice,tInvoiceItems.mTax,tInvoiceItems.mSH,tInvoiceItems.mTotal,tInvoiceItems.uOwner,tInvoiceItems.uCreatedBy,tInvoiceItems.uCreatedDate,tInvoiceItems.uModBy,tInvoiceItems.uModDate"

 //Local only
void Insert_tInvoiceItems(void);
void Update_tInvoiceItems(char *cRowid);
void ProcesstInvoiceItemsListVars(pentry entries[], int x);

 //In tInvoiceItemsfunc.h file included below
void ExtProcesstInvoiceItemsVars(pentry entries[], int x);
void ExttInvoiceItemsCommands(pentry entries[], int x);
void ExttInvoiceItemsButtons(void);
void ExttInvoiceItemsNavBar(void);
void ExttInvoiceItemsGetHook(entry gentries[], int x);
void ExttInvoiceItemsSelect(void);
void ExttInvoiceItemsSelectRow(void);
void ExttInvoiceItemsListSelect(void);
void ExttInvoiceItemsListFilter(void);
void ExttInvoiceItemsAuxTable(void);

#include "tinvoiceitemsfunc.h"

 //Table Variables Assignment Function
void ProcesstInvoiceItemsVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uInvoiceItems"))
			sscanf(entries[i].val,"%u",&uInvoiceItems);
		else if(!strcmp(entries[i].name,"uInvoice"))
			sscanf(entries[i].val,"%u",&uInvoice);
		else if(!strcmp(entries[i].name,"uClient"))
			sscanf(entries[i].val,"%u",&uClient);
		else if(!strcmp(entries[i].name,"uProduct"))
			sscanf(entries[i].val,"%u",&uProduct);
		else if(!strcmp(entries[i].name,"uQuantity"))
			sscanf(entries[i].val,"%u",&uQuantity);
		else if(!strcmp(entries[i].name,"mPrice"))
			sprintf(mPrice,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"mTax"))
			sprintf(mTax,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"mSH"))
			sprintf(mSH,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"mTotal"))
			sprintf(mTotal,"%.15s",entries[i].val);
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
	ExtProcesstInvoiceItemsVars(entries,x);

}//ProcesstInvoiceItemsVars()


void ProcesstInvoiceItemsListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uInvoiceItems);
                        guMode=2002;
                        tInvoiceItems("");
                }
        }
}//void ProcesstInvoiceItemsListVars(pentry entries[], int x)


int tInvoiceItemsCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttInvoiceItemsCommands(entries,x);

	if(!strcmp(gcFunction,"tInvoiceItemsTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tInvoiceItemsList();
		}

		//Default
		ProcesstInvoiceItemsVars(entries,x);
		tInvoiceItems("");
	}
	else if(!strcmp(gcFunction,"tInvoiceItemsList"))
	{
		ProcessControlVars(entries,x);
		ProcesstInvoiceItemsListVars(entries,x);
		tInvoiceItemsList();
	}

	return(0);

}//tInvoiceItemsCommands()


void tInvoiceItems(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttInvoiceItemsSelectRow();
		else
			ExttInvoiceItemsSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetInvoiceItems();
				unxsISP("New tInvoiceItems table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tInvoiceItems WHERE uInvoiceItems=%u"
						,uInvoiceItems);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uInvoiceItems);
		sscanf(field[1],"%u",&uInvoice);
		sscanf(field[2],"%u",&uClient);
		sscanf(field[3],"%u",&uProduct);
		sscanf(field[4],"%u",&uQuantity);
		sprintf(mPrice,"%.12s",field[5]);
		sprintf(mTax,"%.12s",field[6]);
		sprintf(mSH,"%.12s",field[7]);
		sprintf(mTotal,"%.12s",field[8]);
		sscanf(field[9],"%u",&uOwner);
		sscanf(field[10],"%u",&uCreatedBy);
		sscanf(field[11],"%lu",&uCreatedDate);
		sscanf(field[12],"%u",&uModBy);
		sscanf(field[13],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tInvoiceItems",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tInvoiceItemsTools>");
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

        ExttInvoiceItemsButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tInvoiceItems Record Data",100);

	if(guMode==2000 || guMode==2002)
		tInvoiceItemsInput(1);
	else
		tInvoiceItemsInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttInvoiceItemsAuxTable();

	Footer_ism3();

}//end of tInvoiceItems();


void tInvoiceItemsInput(unsigned uMode)
{

//uInvoiceItems
	OpenRow(LANG_FL_tInvoiceItems_uInvoiceItems,"black");
	printf("<input title='%s' type=text name=uInvoiceItems value=%u size=16 maxlength=10 "
,LANG_FT_tInvoiceItems_uInvoiceItems,uInvoiceItems);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uInvoiceItems value=%u >\n",uInvoiceItems);
	}
//uInvoice
	OpenRow(LANG_FL_tInvoiceItems_uInvoice,"black");
	printf("<input title='%s' type=text name=uInvoice value=%u size=16 maxlength=10 "
,LANG_FT_tInvoiceItems_uInvoice,uInvoice);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uInvoice value=%u >\n",uInvoice);
	}
//uClient
	OpenRow(LANG_FL_tInvoiceItems_uClient,"black");
	if(guPermLevel>=0 && uMode)
	{
	printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
	else
	{
	printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
//uProduct
	OpenRow(LANG_FL_tInvoiceItems_uProduct,"black");
	if(guPermLevel>=0 && uMode)
	{
	printf("%s<input type=hidden name=uProduct value=%u >\n",ForeignKey("tProduct","cLabel",uProduct),uProduct);
	}
	else
	{
	printf("%s<input type=hidden name=uProduct value=%u >\n",ForeignKey("tProduct","cLabel",uProduct),uProduct);
	}
//uQuantity
	OpenRow(LANG_FL_tInvoiceItems_uQuantity,"black");
	printf("<input title='%s' type=text name=uQuantity value=%u size=16 maxlength=10 "
,LANG_FT_tInvoiceItems_uQuantity,uQuantity);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uQuantity value=%u >\n",uQuantity);
	}
//mPrice
	OpenRow(LANG_FL_tInvoiceItems_mPrice,"black");
	printf("<input title='%s' type=text name=mPrice value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tInvoiceItems_mPrice,EncodeDoubleQuotes(mPrice));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mPrice value=\"%s\">\n",EncodeDoubleQuotes(mPrice));
	}
//mTax
	OpenRow(LANG_FL_tInvoiceItems_mTax,"black");
	printf("<input title='%s' type=text name=mTax value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tInvoiceItems_mTax,EncodeDoubleQuotes(mTax));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mTax value=\"%s\">\n",EncodeDoubleQuotes(mTax));
	}
//mSH
	OpenRow(LANG_FL_tInvoiceItems_mSH,"black");
	printf("<input title='%s' type=text name=mSH value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tInvoiceItems_mSH,EncodeDoubleQuotes(mSH));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mSH value=\"%s\">\n",EncodeDoubleQuotes(mSH));
	}
//mTotal
	OpenRow(LANG_FL_tInvoiceItems_mTotal,"black");
	printf("<input title='%s' type=text name=mTotal value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tInvoiceItems_mTotal,EncodeDoubleQuotes(mTotal));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mTotal value=\"%s\">\n",EncodeDoubleQuotes(mTotal));
	}
//uOwner
	OpenRow(LANG_FL_tInvoiceItems_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tInvoiceItems_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tInvoiceItems_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tInvoiceItems_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tInvoiceItems_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tInvoiceItemsInput(unsigned uMode)


void NewtInvoiceItems(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uInvoiceItems FROM tInvoiceItems\
				WHERE uInvoiceItems=%u"
							,uInvoiceItems);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tInvoiceItems("<blink>Record already exists");
		tInvoiceItems(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tInvoiceItems();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uInvoiceItems=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tInvoiceItems",uInvoiceItems);
	unxsISPLog(uInvoiceItems,"tInvoiceItems","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uInvoiceItems);
	tInvoiceItems(gcQuery);
	}

}//NewtInvoiceItems(unsigned uMode)


void DeletetInvoiceItems(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tInvoiceItems WHERE uInvoiceItems=%u AND ( uOwner=%u OR %u>9 )"
					,uInvoiceItems,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tInvoiceItems WHERE uInvoiceItems=%u"
					,uInvoiceItems);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tInvoiceItems("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uInvoiceItems,"tInvoiceItems","Del");
#endif
		tInvoiceItems(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uInvoiceItems,"tInvoiceItems","DelError");
#endif
		tInvoiceItems(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetInvoiceItems(void)


void Insert_tInvoiceItems(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tInvoiceItems SET uInvoiceItems=%u,uInvoice=%u,uClient=%u,uProduct=%u,uQuantity=%u,mPrice='%s',mTax='%s',mSH='%s',mTotal='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uInvoiceItems
			,uInvoice
			,uClient
			,uProduct
			,uQuantity
			,TextAreaSave(mPrice)
			,TextAreaSave(mTax)
			,TextAreaSave(mSH)
			,TextAreaSave(mTotal)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tInvoiceItems(void)


void Update_tInvoiceItems(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tInvoiceItems SET uInvoiceItems=%u,uInvoice=%u,uClient=%u,uProduct=%u,uQuantity=%u,mPrice='%s',mTax='%s',mSH='%s',mTotal='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uInvoiceItems
			,uInvoice
			,uClient
			,uProduct
			,uQuantity
			,TextAreaSave(mPrice)
			,TextAreaSave(mTax)
			,TextAreaSave(mSH)
			,TextAreaSave(mTotal)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tInvoiceItems(void)


void ModtInvoiceItems(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uInvoiceItems,uModDate FROM tInvoiceItems WHERE uInvoiceItems=%u"
						,uInvoiceItems);
#else
	sprintf(gcQuery,"SELECT uInvoiceItems FROM tInvoiceItems WHERE uInvoiceItems=%u"
						,uInvoiceItems);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tInvoiceItems("<blink>Record does not exist");
	if(i<1) tInvoiceItems(LANG_NBR_RECNOTEXIST);
	//if(i>1) tInvoiceItems("<blink>Multiple rows!");
	if(i>1) tInvoiceItems(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tInvoiceItems(LANG_NBR_EXTMOD);
#endif

	Update_tInvoiceItems(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tInvoiceItems",uInvoiceItems);
	unxsISPLog(uInvoiceItems,"tInvoiceItems","Mod");
#endif
	tInvoiceItems(gcQuery);

}//ModtInvoiceItems(void)


void tInvoiceItemsList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttInvoiceItemsListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tInvoiceItemsList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttInvoiceItemsListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uInvoiceItems<td><font face=arial,helvetica color=white>uInvoice<td><font face=arial,helvetica color=white>uClient<td><font face=arial,helvetica color=white>uProduct<td><font face=arial,helvetica color=white>uQuantity<td><font face=arial,helvetica color=white>mPrice<td><font face=arial,helvetica color=white>mTax<td><font face=arial,helvetica color=white>mSH<td><font face=arial,helvetica color=white>mTotal<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime11=strtoul(field[11],NULL,10);
		char cBuf11[32];
		if(luTime11)
			ctime_r(&luTime11,cBuf11);
		else
			sprintf(cBuf11,"---");
		time_t luTime13=strtoul(field[13],NULL,10);
		char cBuf13[32];
		if(luTime13)
			ctime_r(&luTime13,cBuf13);
		else
			sprintf(cBuf13,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tProduct","cLabel",strtoul(field[3],NULL,10))
			,field[4]
			,field[5]
			,field[6]
			,field[7]
			,field[8]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[9],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[10],NULL,10))
			,cBuf11
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[12],NULL,10))
			,cBuf13
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tInvoiceItemsList()


void CreatetInvoiceItems(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tInvoiceItems ( uInvoiceItems INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uInvoice INT UNSIGNED NOT NULL DEFAULT 0,index (uInvoice), uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uClient INT UNSIGNED NOT NULL DEFAULT 0, uProduct INT UNSIGNED NOT NULL DEFAULT 0,index (uProduct), uQuantity INT UNSIGNED NOT NULL DEFAULT 0, mTax DECIMAL(10,2) NOT NULL DEFAULT 0, mTotal DECIMAL(10,2) NOT NULL DEFAULT 0, mSH DECIMAL(10,2) NOT NULL DEFAULT 0, mPrice DECIMAL(10,2) NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetInvoiceItems()

