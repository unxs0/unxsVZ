/*
FILE
	tPayment source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tpaymentfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uPayment: Primary Key
static unsigned uPayment=0;
//cLabel: Short label
static char cLabel[33]={""};
//uAvailable: Make availabe to shoppers
static unsigned uAvailable=0;
static char cYesNouAvailable[32]={""};
//uRequireCreditCard: Credit card data required for checkout
static unsigned uRequireCreditCard=0;
static char cYesNouRequireCreditCard[32]={""};
//uOnlineProcess: Use online payment processing
static unsigned uOnlineProcess=0;
static char cYesNouOnlineProcess[32]={""};
//uEmailNotice: Notify tConfiguration cEmailOrderDesk
static unsigned uEmailNotice=0;
static char cYesNouEmailNotice[32]={""};
//uShipOnOrder: Ship when order is placed
static unsigned uShipOnOrder=0;
static char cYesNouShipOnOrder[32]={""};
//cHelpTemplate: tTemplate cLabel name
static char cHelpTemplate[33]={""};
//cInvoiceFooter: tTemplate cLabel name for invoice footer
static char cInvoiceFooter[33]={""};
//uBillAgain: Auto rebill every uPeriod
static unsigned uBillAgain=0;
static char cYesNouBillAgain[32]={""};
//uTimes: Auto rebill uTimes. 0 is forever
static unsigned uTimes=0;
//uPeriod: Auto rebill every uPeriod
static unsigned uPeriod=0;
static char cuPeriodPullDown[256]={""};
//mAdjust: Adjust by a negative or positive percentage
static char mAdjust[16]={"0.00"};
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



#define VAR_LIST_tPayment "tPayment.uPayment,tPayment.cLabel,tPayment.uAvailable,tPayment.uRequireCreditCard,tPayment.uOnlineProcess,tPayment.uEmailNotice,tPayment.uShipOnOrder,tPayment.cHelpTemplate,tPayment.cInvoiceFooter,tPayment.uBillAgain,tPayment.uTimes,tPayment.uPeriod,tPayment.mAdjust,tPayment.uOwner,tPayment.uCreatedBy,tPayment.uCreatedDate,tPayment.uModBy,tPayment.uModDate"

 //Local only
void Insert_tPayment(void);
void Update_tPayment(char *cRowid);
void ProcesstPaymentListVars(pentry entries[], int x);

 //In tPaymentfunc.h file included below
void ExtProcesstPaymentVars(pentry entries[], int x);
void ExttPaymentCommands(pentry entries[], int x);
void ExttPaymentButtons(void);
void ExttPaymentNavBar(void);
void ExttPaymentGetHook(entry gentries[], int x);
void ExttPaymentSelect(void);
void ExttPaymentSelectRow(void);
void ExttPaymentListSelect(void);
void ExttPaymentListFilter(void);
void ExttPaymentAuxTable(void);

#include "tpaymentfunc.h"

 //Table Variables Assignment Function
void ProcesstPaymentVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uPayment"))
			sscanf(entries[i].val,"%u",&uPayment);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uAvailable"))
			sscanf(entries[i].val,"%u",&uAvailable);
		else if(!strcmp(entries[i].name,"cYesNouAvailable"))
		{
			sprintf(cYesNouAvailable,"%.31s",entries[i].val);
			uAvailable=ReadYesNoPullDown(cYesNouAvailable);
		}
		else if(!strcmp(entries[i].name,"uRequireCreditCard"))
			sscanf(entries[i].val,"%u",&uRequireCreditCard);
		else if(!strcmp(entries[i].name,"cYesNouRequireCreditCard"))
		{
			sprintf(cYesNouRequireCreditCard,"%.31s",entries[i].val);
			uRequireCreditCard=ReadYesNoPullDown(cYesNouRequireCreditCard);
		}
		else if(!strcmp(entries[i].name,"uOnlineProcess"))
			sscanf(entries[i].val,"%u",&uOnlineProcess);
		else if(!strcmp(entries[i].name,"cYesNouOnlineProcess"))
		{
			sprintf(cYesNouOnlineProcess,"%.31s",entries[i].val);
			uOnlineProcess=ReadYesNoPullDown(cYesNouOnlineProcess);
		}
		else if(!strcmp(entries[i].name,"uEmailNotice"))
			sscanf(entries[i].val,"%u",&uEmailNotice);
		else if(!strcmp(entries[i].name,"cYesNouEmailNotice"))
		{
			sprintf(cYesNouEmailNotice,"%.31s",entries[i].val);
			uEmailNotice=ReadYesNoPullDown(cYesNouEmailNotice);
		}
		else if(!strcmp(entries[i].name,"uShipOnOrder"))
			sscanf(entries[i].val,"%u",&uShipOnOrder);
		else if(!strcmp(entries[i].name,"cYesNouShipOnOrder"))
		{
			sprintf(cYesNouShipOnOrder,"%.31s",entries[i].val);
			uShipOnOrder=ReadYesNoPullDown(cYesNouShipOnOrder);
		}
		else if(!strcmp(entries[i].name,"cHelpTemplate"))
			sprintf(cHelpTemplate,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cInvoiceFooter"))
			sprintf(cInvoiceFooter,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uBillAgain"))
			sscanf(entries[i].val,"%u",&uBillAgain);
		else if(!strcmp(entries[i].name,"cYesNouBillAgain"))
		{
			sprintf(cYesNouBillAgain,"%.31s",entries[i].val);
			uBillAgain=ReadYesNoPullDown(cYesNouBillAgain);
		}
		else if(!strcmp(entries[i].name,"uTimes"))
			sscanf(entries[i].val,"%u",&uTimes);
		else if(!strcmp(entries[i].name,"uPeriod"))
			sscanf(entries[i].val,"%u",&uPeriod);
		else if(!strcmp(entries[i].name,"cuPeriodPullDown"))
		{
			sprintf(cuPeriodPullDown,"%.255s",entries[i].val);
			uPeriod=ReadPullDown("tPeriod","cLabel",cuPeriodPullDown);
		}
		else if(!strcmp(entries[i].name,"mAdjust"))
			sprintf(mAdjust,"%.15s",entries[i].val);
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
	ExtProcesstPaymentVars(entries,x);

}//ProcesstPaymentVars()


void ProcesstPaymentListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uPayment);
                        guMode=2002;
                        tPayment("");
                }
        }
}//void ProcesstPaymentListVars(pentry entries[], int x)


int tPaymentCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttPaymentCommands(entries,x);

	if(!strcmp(gcFunction,"tPaymentTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tPaymentList();
		}

		//Default
		ProcesstPaymentVars(entries,x);
		tPayment("");
	}
	else if(!strcmp(gcFunction,"tPaymentList"))
	{
		ProcessControlVars(entries,x);
		ProcesstPaymentListVars(entries,x);
		tPaymentList();
	}

	return(0);

}//tPaymentCommands()


void tPayment(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttPaymentSelectRow();
		else
			ExttPaymentSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetPayment();
				unxsISP("New tPayment table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tPayment WHERE uPayment=%u"
						,uPayment);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uPayment);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uAvailable);
		sscanf(field[3],"%u",&uRequireCreditCard);
		sscanf(field[4],"%u",&uOnlineProcess);
		sscanf(field[5],"%u",&uEmailNotice);
		sscanf(field[6],"%u",&uShipOnOrder);
		sprintf(cHelpTemplate,"%.32s",field[7]);
		sprintf(cInvoiceFooter,"%.32s",field[8]);
		sscanf(field[9],"%u",&uBillAgain);
		sscanf(field[10],"%u",&uTimes);
		sscanf(field[11],"%u",&uPeriod);
		sprintf(mAdjust,"%.12s",field[12]);
		sscanf(field[13],"%u",&uOwner);
		sscanf(field[14],"%u",&uCreatedBy);
		sscanf(field[15],"%lu",&uCreatedDate);
		sscanf(field[16],"%u",&uModBy);
		sscanf(field[17],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tPayment",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tPaymentTools>");
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

        ExttPaymentButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tPayment Record Data",100);

	if(guMode==2000 || guMode==2002)
		tPaymentInput(1);
	else
		tPaymentInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttPaymentAuxTable();

	Footer_ism3();

}//end of tPayment();


void tPaymentInput(unsigned uMode)
{

//uPayment
	OpenRow(LANG_FL_tPayment_uPayment,"black");
	printf("<input title='%s' type=text name=uPayment value=%u size=16 maxlength=10 "
,LANG_FT_tPayment_uPayment,uPayment);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uPayment value=%u >\n",uPayment);
	}
//cLabel
	OpenRow(LANG_FL_tPayment_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tPayment_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uAvailable
	OpenRow(LANG_FL_tPayment_uAvailable,"black");
	if(guPermLevel>=0 && uMode)
		YesNoPullDown("uAvailable",uAvailable,1);
	else
		YesNoPullDown("uAvailable",uAvailable,0);
//uRequireCreditCard
	OpenRow(LANG_FL_tPayment_uRequireCreditCard,"black");
	if(guPermLevel>=0 && uMode)
		YesNoPullDown("uRequireCreditCard",uRequireCreditCard,1);
	else
		YesNoPullDown("uRequireCreditCard",uRequireCreditCard,0);
//uOnlineProcess
	OpenRow(LANG_FL_tPayment_uOnlineProcess,"black");
	if(guPermLevel>=0 && uMode)
		YesNoPullDown("uOnlineProcess",uOnlineProcess,1);
	else
		YesNoPullDown("uOnlineProcess",uOnlineProcess,0);
//uEmailNotice
	OpenRow(LANG_FL_tPayment_uEmailNotice,"black");
	if(guPermLevel>=0 && uMode)
		YesNoPullDown("uEmailNotice",uEmailNotice,1);
	else
		YesNoPullDown("uEmailNotice",uEmailNotice,0);
//uShipOnOrder
	OpenRow(LANG_FL_tPayment_uShipOnOrder,"black");
	if(guPermLevel>=0 && uMode)
		YesNoPullDown("uShipOnOrder",uShipOnOrder,1);
	else
		YesNoPullDown("uShipOnOrder",uShipOnOrder,0);
//cHelpTemplate
	OpenRow(LANG_FL_tPayment_cHelpTemplate,"black");
	printf("<input title='%s' type=text name=cHelpTemplate value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tPayment_cHelpTemplate,EncodeDoubleQuotes(cHelpTemplate));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cHelpTemplate value=\"%s\">\n",EncodeDoubleQuotes(cHelpTemplate));
	}
//cInvoiceFooter
	OpenRow(LANG_FL_tPayment_cInvoiceFooter,"black");
	printf("<input title='%s' type=text name=cInvoiceFooter value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tPayment_cInvoiceFooter,EncodeDoubleQuotes(cInvoiceFooter));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cInvoiceFooter value=\"%s\">\n",EncodeDoubleQuotes(cInvoiceFooter));
	}
//uBillAgain
	OpenRow(LANG_FL_tPayment_uBillAgain,"black");
	if(guPermLevel>=0 && uMode)
		YesNoPullDown("uBillAgain",uBillAgain,1);
	else
		YesNoPullDown("uBillAgain",uBillAgain,0);
//uTimes
	OpenRow(LANG_FL_tPayment_uTimes,"black");
	printf("<input title='%s' type=text name=uTimes value=%u size=16 maxlength=10 "
,LANG_FT_tPayment_uTimes,uTimes);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uTimes value=%u >\n",uTimes);
	}
//uPeriod
	OpenRow(LANG_FL_tPayment_uPeriod,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tPeriod;cuPeriodPullDown","cLabel","cLabel",uPeriod,1);
	else
		tTablePullDown("tPeriod;cuPeriodPullDown","cLabel","cLabel",uPeriod,0);
//mAdjust
	OpenRow(LANG_FL_tPayment_mAdjust,"black");
	printf("<input title='%s' type=text name=mAdjust value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tPayment_mAdjust,EncodeDoubleQuotes(mAdjust));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mAdjust value=\"%s\">\n",EncodeDoubleQuotes(mAdjust));
	}
//uOwner
	OpenRow(LANG_FL_tPayment_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tPayment_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tPayment_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tPayment_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tPayment_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tPaymentInput(unsigned uMode)


void NewtPayment(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uPayment FROM tPayment\
				WHERE uPayment=%u"
							,uPayment);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tPayment("<blink>Record already exists");
		tPayment(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tPayment();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uPayment=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tPayment",uPayment);
	unxsISPLog(uPayment,"tPayment","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uPayment);
	tPayment(gcQuery);
	}

}//NewtPayment(unsigned uMode)


void DeletetPayment(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tPayment WHERE uPayment=%u AND ( uOwner=%u OR %u>9 )"
					,uPayment,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tPayment WHERE uPayment=%u"
					,uPayment);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tPayment("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uPayment,"tPayment","Del");
#endif
		tPayment(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uPayment,"tPayment","DelError");
#endif
		tPayment(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetPayment(void)


void Insert_tPayment(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tPayment SET uPayment=%u,cLabel='%s',uAvailable=%u,uRequireCreditCard=%u,uOnlineProcess=%u,uEmailNotice=%u,uShipOnOrder=%u,cHelpTemplate='%s',cInvoiceFooter='%s',uBillAgain=%u,uTimes=%u,uPeriod=%u,mAdjust='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uPayment
			,TextAreaSave(cLabel)
			,uAvailable
			,uRequireCreditCard
			,uOnlineProcess
			,uEmailNotice
			,uShipOnOrder
			,TextAreaSave(cHelpTemplate)
			,TextAreaSave(cInvoiceFooter)
			,uBillAgain
			,uTimes
			,uPeriod
			,TextAreaSave(mAdjust)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tPayment(void)


void Update_tPayment(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tPayment SET uPayment=%u,cLabel='%s',uAvailable=%u,uRequireCreditCard=%u,uOnlineProcess=%u,uEmailNotice=%u,uShipOnOrder=%u,cHelpTemplate='%s',cInvoiceFooter='%s',uBillAgain=%u,uTimes=%u,uPeriod=%u,mAdjust='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uPayment
			,TextAreaSave(cLabel)
			,uAvailable
			,uRequireCreditCard
			,uOnlineProcess
			,uEmailNotice
			,uShipOnOrder
			,TextAreaSave(cHelpTemplate)
			,TextAreaSave(cInvoiceFooter)
			,uBillAgain
			,uTimes
			,uPeriod
			,TextAreaSave(mAdjust)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tPayment(void)


void ModtPayment(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uPayment,uModDate FROM tPayment WHERE uPayment=%u"
						,uPayment);
#else
	sprintf(gcQuery,"SELECT uPayment FROM tPayment WHERE uPayment=%u"
						,uPayment);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tPayment("<blink>Record does not exist");
	if(i<1) tPayment(LANG_NBR_RECNOTEXIST);
	//if(i>1) tPayment("<blink>Multiple rows!");
	if(i>1) tPayment(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tPayment(LANG_NBR_EXTMOD);
#endif

	Update_tPayment(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tPayment",uPayment);
	unxsISPLog(uPayment,"tPayment","Mod");
#endif
	tPayment(gcQuery);

}//ModtPayment(void)


void tPaymentList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttPaymentListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tPaymentList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttPaymentListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uPayment<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uAvailable<td><font face=arial,helvetica color=white>uRequireCreditCard<td><font face=arial,helvetica color=white>uOnlineProcess<td><font face=arial,helvetica color=white>uEmailNotice<td><font face=arial,helvetica color=white>uShipOnOrder<td><font face=arial,helvetica color=white>cHelpTemplate<td><font face=arial,helvetica color=white>cInvoiceFooter<td><font face=arial,helvetica color=white>uBillAgain<td><font face=arial,helvetica color=white>uTimes<td><font face=arial,helvetica color=white>uPeriod<td><font face=arial,helvetica color=white>mAdjust<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luYesNo2=strtoul(field[2],NULL,10);
		char cBuf2[4];
		if(luYesNo2)
			sprintf(cBuf2,"Yes");
		else
			sprintf(cBuf2,"No");
		time_t luYesNo3=strtoul(field[3],NULL,10);
		char cBuf3[4];
		if(luYesNo3)
			sprintf(cBuf3,"Yes");
		else
			sprintf(cBuf3,"No");
		time_t luYesNo4=strtoul(field[4],NULL,10);
		char cBuf4[4];
		if(luYesNo4)
			sprintf(cBuf4,"Yes");
		else
			sprintf(cBuf4,"No");
		time_t luYesNo5=strtoul(field[5],NULL,10);
		char cBuf5[4];
		if(luYesNo5)
			sprintf(cBuf5,"Yes");
		else
			sprintf(cBuf5,"No");
		time_t luYesNo6=strtoul(field[6],NULL,10);
		char cBuf6[4];
		if(luYesNo6)
			sprintf(cBuf6,"Yes");
		else
			sprintf(cBuf6,"No");
		time_t luYesNo9=strtoul(field[9],NULL,10);
		char cBuf9[4];
		if(luYesNo9)
			sprintf(cBuf9,"Yes");
		else
			sprintf(cBuf9,"No");
		time_t luTime15=strtoul(field[15],NULL,10);
		char cBuf15[32];
		if(luTime15)
			ctime_r(&luTime15,cBuf15);
		else
			sprintf(cBuf15,"---");
		time_t luTime17=strtoul(field[17],NULL,10);
		char cBuf17[32];
		if(luTime17)
			ctime_r(&luTime17,cBuf17);
		else
			sprintf(cBuf17,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,cBuf2
			,cBuf3
			,cBuf4
			,cBuf5
			,cBuf6
			,field[7]
			,field[8]
			,cBuf9
			,field[10]
			,ForeignKey("tPeriod","cLabel",strtoul(field[11],NULL,10))
			,field[12]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[13],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[14],NULL,10))
			,cBuf15
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[16],NULL,10))
			,cBuf17
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tPaymentList()


void CreatetPayment(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tPayment ( uModDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), cLabel VARCHAR(32) NOT NULL DEFAULT '', UNIQUE (cLabel), uPayment INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uRequireCreditCard INT UNSIGNED NOT NULL DEFAULT 0, uEmailNotice INT UNSIGNED NOT NULL DEFAULT 0, cHelpTemplate VARCHAR(32) NOT NULL DEFAULT '', uShipOnOrder INT UNSIGNED NOT NULL DEFAULT 0, uBillAgain INT UNSIGNED NOT NULL DEFAULT 0, uTimes INT UNSIGNED NOT NULL DEFAULT 0, uPeriod INT UNSIGNED NOT NULL DEFAULT 0, mAdjust DECIMAL(10,2) NOT NULL DEFAULT 0, cInvoiceFooter VARCHAR(32) NOT NULL DEFAULT '', uAvailable INT UNSIGNED NOT NULL DEFAULT 0, uOnlineProcess INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetPayment()

