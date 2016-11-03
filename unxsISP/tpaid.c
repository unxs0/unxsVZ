/*
FILE
	tPaid source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tpaidfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uPaid: Primary Key
static unsigned uPaid=0;
//uPayment: Payment type
static unsigned uPayment=0;
static char cuPaymentPullDown[256]={""};
//mAmount: Amount received
static char mAmount[16]={"0.00"};
//uInvoice: Payment corresponds to this invoice
static unsigned uInvoice=0;
//uClient: Payment corresponds to this customer
static unsigned uClient=0;
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



#define VAR_LIST_tPaid "tPaid.uPaid,tPaid.uPayment,tPaid.mAmount,tPaid.uInvoice,tPaid.uClient,tPaid.uOwner,tPaid.uCreatedBy,tPaid.uCreatedDate,tPaid.uModBy,tPaid.uModDate"

 //Local only
void Insert_tPaid(void);
void Update_tPaid(char *cRowid);
void ProcesstPaidListVars(pentry entries[], int x);

 //In tPaidfunc.h file included below
void ExtProcesstPaidVars(pentry entries[], int x);
void ExttPaidCommands(pentry entries[], int x);
void ExttPaidButtons(void);
void ExttPaidNavBar(void);
void ExttPaidGetHook(entry gentries[], int x);
void ExttPaidSelect(void);
void ExttPaidSelectRow(void);
void ExttPaidListSelect(void);
void ExttPaidListFilter(void);
void ExttPaidAuxTable(void);

#include "tpaidfunc.h"

 //Table Variables Assignment Function
void ProcesstPaidVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uPaid"))
			sscanf(entries[i].val,"%u",&uPaid);
		else if(!strcmp(entries[i].name,"uPayment"))
			sscanf(entries[i].val,"%u",&uPayment);
		else if(!strcmp(entries[i].name,"cuPaymentPullDown"))
		{
			sprintf(cuPaymentPullDown,"%.255s",entries[i].val);
			uPayment=ReadPullDown("tPayment","cLabel",cuPaymentPullDown);
		}
		else if(!strcmp(entries[i].name,"mAmount"))
			sprintf(mAmount,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"uInvoice"))
			sscanf(entries[i].val,"%u",&uInvoice);
		else if(!strcmp(entries[i].name,"uClient"))
			sscanf(entries[i].val,"%u",&uClient);
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
	ExtProcesstPaidVars(entries,x);

}//ProcesstPaidVars()


void ProcesstPaidListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uPaid);
                        guMode=2002;
                        tPaid("");
                }
        }
}//void ProcesstPaidListVars(pentry entries[], int x)


int tPaidCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttPaidCommands(entries,x);

	if(!strcmp(gcFunction,"tPaidTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tPaidList();
		}

		//Default
		ProcesstPaidVars(entries,x);
		tPaid("");
	}
	else if(!strcmp(gcFunction,"tPaidList"))
	{
		ProcessControlVars(entries,x);
		ProcesstPaidListVars(entries,x);
		tPaidList();
	}

	return(0);

}//tPaidCommands()


void tPaid(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttPaidSelectRow();
		else
			ExttPaidSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetPaid();
				unxsISP("New tPaid table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tPaid WHERE uPaid=%u"
						,uPaid);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uPaid);
		sscanf(field[1],"%u",&uPayment);
		sprintf(mAmount,"%.12s",field[2]);
		sscanf(field[3],"%u",&uInvoice);
		sscanf(field[4],"%u",&uClient);
		sscanf(field[5],"%u",&uOwner);
		sscanf(field[6],"%u",&uCreatedBy);
		sscanf(field[7],"%lu",&uCreatedDate);
		sscanf(field[8],"%u",&uModBy);
		sscanf(field[9],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tPaid",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tPaidTools>");
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

        ExttPaidButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tPaid Record Data",100);

	if(guMode==2000 || guMode==2002)
		tPaidInput(1);
	else
		tPaidInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttPaidAuxTable();

	Footer_ism3();

}//end of tPaid();


void tPaidInput(unsigned uMode)
{

//uPaid
	OpenRow(LANG_FL_tPaid_uPaid,"black");
	printf("<input title='%s' type=text name=uPaid value=%u size=16 maxlength=10 "
,LANG_FT_tPaid_uPaid,uPaid);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uPaid value=%u >\n",uPaid);
	}
//uPayment
	OpenRow(LANG_FL_tPaid_uPayment,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tPayment;cuPaymentPullDown","cLabel","cLabel",uPayment,1);
	else
		tTablePullDown("tPayment;cuPaymentPullDown","cLabel","cLabel",uPayment,0);
//mAmount
	OpenRow(LANG_FL_tPaid_mAmount,"black");
	printf("<input title='%s' type=text name=mAmount value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tPaid_mAmount,EncodeDoubleQuotes(mAmount));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mAmount value=\"%s\">\n",EncodeDoubleQuotes(mAmount));
	}
//uInvoice
	OpenRow(LANG_FL_tPaid_uInvoice,"black");
	printf("<input title='%s' type=text name=uInvoice value=%u size=16 maxlength=10 "
,LANG_FT_tPaid_uInvoice,uInvoice);
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
	OpenRow(LANG_FL_tPaid_uClient,"black");
	printf("<input title='%s' type=text name=uClient value=%u size=16 maxlength=10 "
,LANG_FT_tPaid_uClient,uClient);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uClient value=%u >\n",uClient);
	}
//uOwner
	OpenRow(LANG_FL_tPaid_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tPaid_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tPaid_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tPaid_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tPaid_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tPaidInput(unsigned uMode)


void NewtPaid(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uPaid FROM tPaid\
				WHERE uPaid=%u"
							,uPaid);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tPaid("<blink>Record already exists");
		tPaid(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tPaid();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uPaid=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tPaid",uPaid);
	unxsISPLog(uPaid,"tPaid","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uPaid);
	tPaid(gcQuery);
	}

}//NewtPaid(unsigned uMode)


void DeletetPaid(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tPaid WHERE uPaid=%u AND ( uOwner=%u OR %u>9 )"
					,uPaid,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tPaid WHERE uPaid=%u"
					,uPaid);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tPaid("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uPaid,"tPaid","Del");
#endif
		tPaid(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uPaid,"tPaid","DelError");
#endif
		tPaid(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetPaid(void)


void Insert_tPaid(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tPaid SET uPaid=%u,uPayment=%u,mAmount='%s',uInvoice=%u,uClient=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uPaid
			,uPayment
			,TextAreaSave(mAmount)
			,uInvoice
			,uClient
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tPaid(void)


void Update_tPaid(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tPaid SET uPaid=%u,uPayment=%u,mAmount='%s',uInvoice=%u,uClient=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uPaid
			,uPayment
			,TextAreaSave(mAmount)
			,uInvoice
			,uClient
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tPaid(void)


void ModtPaid(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uPaid,uModDate FROM tPaid WHERE uPaid=%u"
						,uPaid);
#else
	sprintf(gcQuery,"SELECT uPaid FROM tPaid WHERE uPaid=%u"
						,uPaid);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tPaid("<blink>Record does not exist");
	if(i<1) tPaid(LANG_NBR_RECNOTEXIST);
	//if(i>1) tPaid("<blink>Multiple rows!");
	if(i>1) tPaid(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tPaid(LANG_NBR_EXTMOD);
#endif

	Update_tPaid(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tPaid",uPaid);
	unxsISPLog(uPaid,"tPaid","Mod");
#endif
	tPaid(gcQuery);

}//ModtPaid(void)


void tPaidList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttPaidListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tPaidList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttPaidListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uPaid<td><font face=arial,helvetica color=white>uPayment<td><font face=arial,helvetica color=white>mAmount<td><font face=arial,helvetica color=white>uInvoice<td><font face=arial,helvetica color=white>uClient<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime7=strtoul(field[7],NULL,10);
		char cBuf7[32];
		if(luTime7)
			ctime_r(&luTime7,cBuf7);
		else
			sprintf(cBuf7,"---");
		time_t luTime9=strtoul(field[9],NULL,10);
		char cBuf9[32];
		if(luTime9)
			ctime_r(&luTime9,cBuf9);
		else
			sprintf(cBuf9,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,ForeignKey("tPayment","cLabel",strtoul(field[1],NULL,10))
			,field[2]
			,field[3]
			,field[4]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,cBuf7
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[8],NULL,10))
			,cBuf9
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tPaidList()


void CreatetPaid(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tPaid ( uPaid INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uPayment INT UNSIGNED NOT NULL DEFAULT 0, uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, mAmount DECIMAL(10,2) NOT NULL DEFAULT 0, uInvoice INT UNSIGNED NOT NULL DEFAULT 0,index (uInvoice), uClient INT UNSIGNED NOT NULL DEFAULT 0,index (uClient) )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetPaid()

