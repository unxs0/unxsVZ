/*
FILE
	tProduct source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tproductfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uProduct: Primary Key
static unsigned uProduct=0;
//cLabel: Short label
static char cLabel[33]={""};
//uAvailable: Available for use
static unsigned uAvailable=0;
static char cYesNouAvailable[32]={""};
//uPeriod: One-time or recurring billing period
static unsigned uPeriod=0;
static char cuPeriodPullDown[256]={""};
//mPrice: Billing price per period
static char mPrice[16]={"0.00"};
//mCost: Cost per period
static char mCost[16]={"0.00"};
//mSetupFee: One time setup fee
static char mSetupFee[16]={"0.00"};
//cComment: Product description comments
static char *cComment={""};
//mReleaseFee: One time release fee. For release of a hold etc.
static char mReleaseFee[16]={"0.00"};
//uProductType: Main service area for this product
static unsigned uProductType=0;
static char cuProductTypePullDown[256]={""};
//cTitle: html title for select in ism3
static char cTitle[33]={""};
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



#define VAR_LIST_tProduct "tProduct.uProduct,tProduct.cLabel,tProduct.uAvailable,tProduct.uPeriod,tProduct.mPrice,tProduct.mCost,tProduct.mSetupFee,tProduct.cComment,tProduct.mReleaseFee,tProduct.uProductType,tProduct.cTitle,tProduct.uOwner,tProduct.uCreatedBy,tProduct.uCreatedDate,tProduct.uModBy,tProduct.uModDate"

 //Local only
void Insert_tProduct(void);
void Update_tProduct(char *cRowid);
void ProcesstProductListVars(pentry entries[], int x);

 //In tProductfunc.h file included below
void ExtProcesstProductVars(pentry entries[], int x);
void ExttProductCommands(pentry entries[], int x);
void ExttProductButtons(void);
void ExttProductNavBar(void);
void ExttProductGetHook(entry gentries[], int x);
void ExttProductSelect(void);
void ExttProductSelectRow(void);
void ExttProductListSelect(void);
void ExttProductListFilter(void);
void ExttProductAuxTable(void);

#include "tproductfunc.h"

 //Table Variables Assignment Function
void ProcesstProductVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uProduct"))
			sscanf(entries[i].val,"%u",&uProduct);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uAvailable"))
			sscanf(entries[i].val,"%u",&uAvailable);
		else if(!strcmp(entries[i].name,"cYesNouAvailable"))
		{
			sprintf(cYesNouAvailable,"%.31s",entries[i].val);
			uAvailable=ReadYesNoPullDown(cYesNouAvailable);
		}
		else if(!strcmp(entries[i].name,"uPeriod"))
			sscanf(entries[i].val,"%u",&uPeriod);
		else if(!strcmp(entries[i].name,"cuPeriodPullDown"))
		{
			sprintf(cuPeriodPullDown,"%.255s",entries[i].val);
			uPeriod=ReadPullDown("tPeriod","cLabel",cuPeriodPullDown);
		}
		else if(!strcmp(entries[i].name,"mPrice"))
			sprintf(mPrice,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"mCost"))
			sprintf(mCost,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"mSetupFee"))
			sprintf(mSetupFee,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"cComment"))
			cComment=entries[i].val;
		else if(!strcmp(entries[i].name,"mReleaseFee"))
			sprintf(mReleaseFee,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"uProductType"))
			sscanf(entries[i].val,"%u",&uProductType);
		else if(!strcmp(entries[i].name,"cuProductTypePullDown"))
		{
			sprintf(cuProductTypePullDown,"%.255s",entries[i].val);
			uProductType=ReadPullDown("tProductType","cLabel",cuProductTypePullDown);
		}
		else if(!strcmp(entries[i].name,"cTitle"))
			sprintf(cTitle,"%.32s",entries[i].val);
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
	ExtProcesstProductVars(entries,x);

}//ProcesstProductVars()


void ProcesstProductListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uProduct);
                        guMode=2002;
                        tProduct("");
                }
        }
}//void ProcesstProductListVars(pentry entries[], int x)


int tProductCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttProductCommands(entries,x);

	if(!strcmp(gcFunction,"tProductTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tProductList();
		}

		//Default
		ProcesstProductVars(entries,x);
		tProduct("");
	}
	else if(!strcmp(gcFunction,"tProductList"))
	{
		ProcessControlVars(entries,x);
		ProcesstProductListVars(entries,x);
		tProductList();
	}

	return(0);

}//tProductCommands()


void tProduct(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttProductSelectRow();
		else
			ExttProductSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetProduct();
				unxsISP("New tProduct table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tProduct WHERE uProduct=%u"
						,uProduct);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uProduct);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uAvailable);
		sscanf(field[3],"%u",&uPeriod);
		sprintf(mPrice,"%.12s",field[4]);
		sprintf(mCost,"%.12s",field[5]);
		sprintf(mSetupFee,"%.12s",field[6]);
		cComment=field[7];
		sprintf(mReleaseFee,"%.12s",field[8]);
		sscanf(field[9],"%u",&uProductType);
		sprintf(cTitle,"%.32s",field[10]);
		sscanf(field[11],"%u",&uOwner);
		sscanf(field[12],"%u",&uCreatedBy);
		sscanf(field[13],"%lu",&uCreatedDate);
		sscanf(field[14],"%u",&uModBy);
		sscanf(field[15],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tProduct",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tProductTools>");
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

        ExttProductButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tProduct Record Data",100);

	if(guMode==2000 || guMode==2002)
		tProductInput(1);
	else
		tProductInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttProductAuxTable();

	Footer_ism3();

}//end of tProduct();


void tProductInput(unsigned uMode)
{

//uProduct
	OpenRow(LANG_FL_tProduct_uProduct,"black");
	printf("<input title='%s' type=text name=uProduct value=%u size=16 maxlength=10 "
,LANG_FT_tProduct_uProduct,uProduct);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uProduct value=%u >\n",uProduct);
	}
//cLabel
	OpenRow(LANG_FL_tProduct_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tProduct_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tProduct_uAvailable,"black");
	if(guPermLevel>=8 && uMode)
		YesNoPullDown("uAvailable",uAvailable,1);
	else
		YesNoPullDown("uAvailable",uAvailable,0);
//uPeriod
	OpenRow(LANG_FL_tProduct_uPeriod,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tPeriod;cuPeriodPullDown","cLabel","cLabel",uPeriod,1);
	else
		tTablePullDown("tPeriod;cuPeriodPullDown","cLabel","cLabel",uPeriod,0);
//mPrice
	OpenRow(LANG_FL_tProduct_mPrice,"black");
	printf("<input title='%s' type=text name=mPrice value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tProduct_mPrice,EncodeDoubleQuotes(mPrice));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mPrice value=\"%s\">\n",EncodeDoubleQuotes(mPrice));
	}
//mCost
	OpenRow(LANG_FL_tProduct_mCost,"black");
	printf("<input title='%s' type=text name=mCost value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tProduct_mCost,EncodeDoubleQuotes(mCost));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mCost value=\"%s\">\n",EncodeDoubleQuotes(mCost));
	}
//mSetupFee
	OpenRow(LANG_FL_tProduct_mSetupFee,"black");
	printf("<input title='%s' type=text name=mSetupFee value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tProduct_mSetupFee,EncodeDoubleQuotes(mSetupFee));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mSetupFee value=\"%s\">\n",EncodeDoubleQuotes(mSetupFee));
	}
//cComment
	OpenRow(LANG_FL_tProduct_cComment,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cComment "
,LANG_FT_tProduct_cComment);
	if(guPermLevel>=0 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cComment);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cComment);
		printf("<input type=hidden name=cComment value=\"%s\" >\n",EncodeDoubleQuotes(cComment));
	}
//mReleaseFee
	OpenRow(LANG_FL_tProduct_mReleaseFee,"black");
	printf("<input title='%s' type=text name=mReleaseFee value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tProduct_mReleaseFee,EncodeDoubleQuotes(mReleaseFee));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mReleaseFee value=\"%s\">\n",EncodeDoubleQuotes(mReleaseFee));
	}
//uProductType
	OpenRow(LANG_FL_tProduct_uProductType,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tProductType;cuProductTypePullDown","cLabel","cLabel",uProductType,1);
	else
		tTablePullDown("tProductType;cuProductTypePullDown","cLabel","cLabel",uProductType,0);
//cTitle
	OpenRow(LANG_FL_tProduct_cTitle,"black");
	printf("<input title='%s' type=text name=cTitle value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tProduct_cTitle,EncodeDoubleQuotes(cTitle));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cTitle value=\"%s\">\n",EncodeDoubleQuotes(cTitle));
	}
//uOwner
	OpenRow(LANG_FL_tProduct_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tProduct_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tProduct_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tProduct_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tProduct_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tProductInput(unsigned uMode)


void NewtProduct(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uProduct FROM tProduct\
				WHERE uProduct=%u"
							,uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tProduct("<blink>Record already exists");
		tProduct(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tProduct();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uProduct=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tProduct",uProduct);
	unxsISPLog(uProduct,"tProduct","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uProduct);
	tProduct(gcQuery);
	}

}//NewtProduct(unsigned uMode)


void DeletetProduct(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tProduct WHERE uProduct=%u AND ( uOwner=%u OR %u>9 )"
					,uProduct,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tProduct WHERE uProduct=%u"
					,uProduct);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tProduct("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uProduct,"tProduct","Del");
#endif
		tProduct(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uProduct,"tProduct","DelError");
#endif
		tProduct(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetProduct(void)


void Insert_tProduct(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tProduct SET uProduct=%u,cLabel='%s',uAvailable=%u,uPeriod=%u,mPrice='%s',mCost='%s',mSetupFee='%s',cComment='%s',mReleaseFee='%s',uProductType=%u,cTitle='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uProduct
			,TextAreaSave(cLabel)
			,uAvailable
			,uPeriod
			,TextAreaSave(mPrice)
			,TextAreaSave(mCost)
			,TextAreaSave(mSetupFee)
			,TextAreaSave(cComment)
			,TextAreaSave(mReleaseFee)
			,uProductType
			,TextAreaSave(cTitle)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tProduct(void)


void Update_tProduct(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tProduct SET uProduct=%u,cLabel='%s',uAvailable=%u,uPeriod=%u,mPrice='%s',mCost='%s',mSetupFee='%s',cComment='%s',mReleaseFee='%s',uProductType=%u,cTitle='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uProduct
			,TextAreaSave(cLabel)
			,uAvailable
			,uPeriod
			,TextAreaSave(mPrice)
			,TextAreaSave(mCost)
			,TextAreaSave(mSetupFee)
			,TextAreaSave(cComment)
			,TextAreaSave(mReleaseFee)
			,uProductType
			,TextAreaSave(cTitle)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tProduct(void)


void ModtProduct(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uProduct,uModDate FROM tProduct WHERE uProduct=%u"
						,uProduct);
#else
	sprintf(gcQuery,"SELECT uProduct FROM tProduct WHERE uProduct=%u"
						,uProduct);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tProduct("<blink>Record does not exist");
	if(i<1) tProduct(LANG_NBR_RECNOTEXIST);
	//if(i>1) tProduct("<blink>Multiple rows!");
	if(i>1) tProduct(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tProduct(LANG_NBR_EXTMOD);
#endif

	Update_tProduct(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tProduct",uProduct);
	unxsISPLog(uProduct,"tProduct","Mod");
#endif
	tProduct(gcQuery);

}//ModtProduct(void)


void tProductList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttProductListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tProductList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttProductListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uProduct<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uAvailable<td><font face=arial,helvetica color=white>uPeriod<td><font face=arial,helvetica color=white>mPrice<td><font face=arial,helvetica color=white>mCost<td><font face=arial,helvetica color=white>mSetupFee<td><font face=arial,helvetica color=white>cComment<td><font face=arial,helvetica color=white>mReleaseFee<td><font face=arial,helvetica color=white>uProductType<td><font face=arial,helvetica color=white>cTitle<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime13=strtoul(field[13],NULL,10);
		char cBuf13[32];
		if(luTime13)
			ctime_r(&luTime13,cBuf13);
		else
			sprintf(cBuf13,"---");
		time_t luTime15=strtoul(field[15],NULL,10);
		char cBuf15[32];
		if(luTime15)
			ctime_r(&luTime15,cBuf15);
		else
			sprintf(cBuf15,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,cBuf2
			,ForeignKey("tPeriod","cLabel",strtoul(field[3],NULL,10))
			,field[4]
			,field[5]
			,field[6]
			,field[7]
			,field[8]
			,ForeignKey("tProductType","cLabel",strtoul(field[9],NULL,10))
			,field[10]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[11],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[12],NULL,10))
			,cBuf13
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[14],NULL,10))
			,cBuf15
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tProductList()


void CreatetProduct(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tProduct ( uProduct INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uPeriod INT UNSIGNED NOT NULL DEFAULT 0, mPrice DECIMAL(10,2) NOT NULL DEFAULT 0, mCost DECIMAL(10,2) NOT NULL DEFAULT 0, uAvailable INT UNSIGNED NOT NULL DEFAULT 0, mSetupFee DECIMAL(10,2) NOT NULL DEFAULT 0, cComment TEXT NOT NULL DEFAULT '', uProductType INT UNSIGNED NOT NULL DEFAULT 0, cTitle VARCHAR(32) NOT NULL DEFAULT '', mReleaseFee DECIMAL(10,2) NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetProduct()

