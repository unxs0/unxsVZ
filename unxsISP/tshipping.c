/*
FILE
	tShipping source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tshippingfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uShipping: Primary Key
static unsigned uShipping=0;
//cLabel: Short label
static char cLabel[33]={""};
//cComment: Comment about this shipping method
static char *cComment={""};
//mCost: Shipping cost. Depends on calculation method
static char mCost[16]={"0.00"};
//uMethod: Calculation method (not used yet)
static unsigned uMethod=0;
//uParam1: Calculation method parameter 1
static unsigned uParam1=0;
//uParam2: Calculation method parameter 2
static unsigned uParam2=0;
//cNumDays: Customer should expect shipment in this number of days
static char cNumDays[33]={""};
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



#define VAR_LIST_tShipping "tShipping.uShipping,tShipping.cLabel,tShipping.cComment,tShipping.mCost,tShipping.uMethod,tShipping.uParam1,tShipping.uParam2,tShipping.cNumDays,tShipping.uOwner,tShipping.uCreatedBy,tShipping.uCreatedDate,tShipping.uModBy,tShipping.uModDate"

 //Local only
void Insert_tShipping(void);
void Update_tShipping(char *cRowid);
void ProcesstShippingListVars(pentry entries[], int x);

 //In tShippingfunc.h file included below
void ExtProcesstShippingVars(pentry entries[], int x);
void ExttShippingCommands(pentry entries[], int x);
void ExttShippingButtons(void);
void ExttShippingNavBar(void);
void ExttShippingGetHook(entry gentries[], int x);
void ExttShippingSelect(void);
void ExttShippingSelectRow(void);
void ExttShippingListSelect(void);
void ExttShippingListFilter(void);
void ExttShippingAuxTable(void);

#include "tshippingfunc.h"

 //Table Variables Assignment Function
void ProcesstShippingVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uShipping"))
			sscanf(entries[i].val,"%u",&uShipping);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cComment"))
			cComment=entries[i].val;
		else if(!strcmp(entries[i].name,"mCost"))
			sprintf(mCost,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"uMethod"))
			sscanf(entries[i].val,"%u",&uMethod);
		else if(!strcmp(entries[i].name,"uParam1"))
			sscanf(entries[i].val,"%u",&uParam1);
		else if(!strcmp(entries[i].name,"uParam2"))
			sscanf(entries[i].val,"%u",&uParam2);
		else if(!strcmp(entries[i].name,"cNumDays"))
			sprintf(cNumDays,"%.32s",entries[i].val);
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
	ExtProcesstShippingVars(entries,x);

}//ProcesstShippingVars()


void ProcesstShippingListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uShipping);
                        guMode=2002;
                        tShipping("");
                }
        }
}//void ProcesstShippingListVars(pentry entries[], int x)


int tShippingCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttShippingCommands(entries,x);

	if(!strcmp(gcFunction,"tShippingTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tShippingList();
		}

		//Default
		ProcesstShippingVars(entries,x);
		tShipping("");
	}
	else if(!strcmp(gcFunction,"tShippingList"))
	{
		ProcessControlVars(entries,x);
		ProcesstShippingListVars(entries,x);
		tShippingList();
	}

	return(0);

}//tShippingCommands()


void tShipping(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttShippingSelectRow();
		else
			ExttShippingSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetShipping();
				unxsISP("New tShipping table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tShipping WHERE uShipping=%u"
						,uShipping);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uShipping);
		sprintf(cLabel,"%.32s",field[1]);
		cComment=field[2];
		sprintf(mCost,"%.12s",field[3]);
		sscanf(field[4],"%u",&uMethod);
		sscanf(field[5],"%u",&uParam1);
		sscanf(field[6],"%u",&uParam2);
		sprintf(cNumDays,"%.32s",field[7]);
		sscanf(field[8],"%u",&uOwner);
		sscanf(field[9],"%u",&uCreatedBy);
		sscanf(field[10],"%lu",&uCreatedDate);
		sscanf(field[11],"%u",&uModBy);
		sscanf(field[12],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tShipping",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tShippingTools>");
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

        ExttShippingButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tShipping Record Data",100);

	if(guMode==2000 || guMode==2002)
		tShippingInput(1);
	else
		tShippingInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttShippingAuxTable();

	Footer_ism3();

}//end of tShipping();


void tShippingInput(unsigned uMode)
{

//uShipping
	OpenRow(LANG_FL_tShipping_uShipping,"black");
	printf("<input title='%s' type=text name=uShipping value=%u size=16 maxlength=10 "
,LANG_FT_tShipping_uShipping,uShipping);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uShipping value=%u >\n",uShipping);
	}
//cLabel
	OpenRow(LANG_FL_tShipping_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tShipping_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//cComment
	OpenRow(LANG_FL_tShipping_cComment,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cComment "
,LANG_FT_tShipping_cComment);
	if(guPermLevel>=0 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cComment);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cComment);
		printf("<input type=hidden name=cComment value=\"%s\" >\n",EncodeDoubleQuotes(cComment));
	}
//mCost
	OpenRow(LANG_FL_tShipping_mCost,"black");
	printf("<input title='%s' type=text name=mCost value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tShipping_mCost,EncodeDoubleQuotes(mCost));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mCost value=\"%s\">\n",EncodeDoubleQuotes(mCost));
	}
//uMethod
	OpenRow(LANG_FL_tShipping_uMethod,"black");
	printf("<input title='%s' type=text name=uMethod value=%u size=16 maxlength=10 "
,LANG_FT_tShipping_uMethod,uMethod);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uMethod value=%u >\n",uMethod);
	}
//uParam1
	OpenRow(LANG_FL_tShipping_uParam1,"black");
	printf("<input title='%s' type=text name=uParam1 value=%u size=16 maxlength=10 "
,LANG_FT_tShipping_uParam1,uParam1);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uParam1 value=%u >\n",uParam1);
	}
//uParam2
	OpenRow(LANG_FL_tShipping_uParam2,"black");
	printf("<input title='%s' type=text name=uParam2 value=%u size=16 maxlength=10 "
,LANG_FT_tShipping_uParam2,uParam2);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uParam2 value=%u >\n",uParam2);
	}
//cNumDays
	OpenRow(LANG_FL_tShipping_cNumDays,"black");
	printf("<input title='%s' type=text name=cNumDays value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tShipping_cNumDays,EncodeDoubleQuotes(cNumDays));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cNumDays value=\"%s\">\n",EncodeDoubleQuotes(cNumDays));
	}
//uOwner
	OpenRow(LANG_FL_tShipping_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tShipping_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tShipping_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tShipping_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tShipping_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tShippingInput(unsigned uMode)


void NewtShipping(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uShipping FROM tShipping\
				WHERE uShipping=%u"
							,uShipping);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tShipping("<blink>Record already exists");
		tShipping(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tShipping();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uShipping=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tShipping",uShipping);
	unxsISPLog(uShipping,"tShipping","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uShipping);
	tShipping(gcQuery);
	}

}//NewtShipping(unsigned uMode)


void DeletetShipping(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tShipping WHERE uShipping=%u AND ( uOwner=%u OR %u>9 )"
					,uShipping,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tShipping WHERE uShipping=%u"
					,uShipping);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tShipping("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uShipping,"tShipping","Del");
#endif
		tShipping(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uShipping,"tShipping","DelError");
#endif
		tShipping(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetShipping(void)


void Insert_tShipping(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tShipping SET uShipping=%u,cLabel='%s',cComment='%s',mCost='%s',uMethod=%u,uParam1=%u,uParam2=%u,cNumDays='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uShipping
			,TextAreaSave(cLabel)
			,TextAreaSave(cComment)
			,TextAreaSave(mCost)
			,uMethod
			,uParam1
			,uParam2
			,TextAreaSave(cNumDays)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tShipping(void)


void Update_tShipping(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tShipping SET uShipping=%u,cLabel='%s',cComment='%s',mCost='%s',uMethod=%u,uParam1=%u,uParam2=%u,cNumDays='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uShipping
			,TextAreaSave(cLabel)
			,TextAreaSave(cComment)
			,TextAreaSave(mCost)
			,uMethod
			,uParam1
			,uParam2
			,TextAreaSave(cNumDays)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tShipping(void)


void ModtShipping(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uShipping,uModDate FROM tShipping WHERE uShipping=%u"
						,uShipping);
#else
	sprintf(gcQuery,"SELECT uShipping FROM tShipping WHERE uShipping=%u"
						,uShipping);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tShipping("<blink>Record does not exist");
	if(i<1) tShipping(LANG_NBR_RECNOTEXIST);
	//if(i>1) tShipping("<blink>Multiple rows!");
	if(i>1) tShipping(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tShipping(LANG_NBR_EXTMOD);
#endif

	Update_tShipping(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tShipping",uShipping);
	unxsISPLog(uShipping,"tShipping","Mod");
#endif
	tShipping(gcQuery);

}//ModtShipping(void)


void tShippingList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttShippingListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tShippingList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttShippingListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uShipping<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>cComment<td><font face=arial,helvetica color=white>mCost<td><font face=arial,helvetica color=white>uMethod<td><font face=arial,helvetica color=white>uParam1<td><font face=arial,helvetica color=white>uParam2<td><font face=arial,helvetica color=white>cNumDays<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime10=strtoul(field[10],NULL,10);
		char cBuf10[32];
		if(luTime10)
			ctime_r(&luTime10,cBuf10);
		else
			sprintf(cBuf10,"---");
		time_t luTime12=strtoul(field[12],NULL,10);
		char cBuf12[32];
		if(luTime12)
			ctime_r(&luTime12,cBuf12);
		else
			sprintf(cBuf12,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,field[3]
			,field[4]
			,field[5]
			,field[6]
			,field[7]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[8],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[9],NULL,10))
			,cBuf10
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[11],NULL,10))
			,cBuf12
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tShippingList()


void CreatetShipping(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tShipping ( cLabel VARCHAR(32) NOT NULL DEFAULT '', UNIQUE (cLabel), uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uShipping INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, mCost DECIMAL(10,2) NOT NULL DEFAULT 0, uMethod INT UNSIGNED NOT NULL DEFAULT 0, uParam1 INT UNSIGNED NOT NULL DEFAULT 0, uParam2 INT UNSIGNED NOT NULL DEFAULT 0, cComment TEXT NOT NULL DEFAULT '', cNumDays VARCHAR(32) NOT NULL DEFAULT '' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetShipping()

