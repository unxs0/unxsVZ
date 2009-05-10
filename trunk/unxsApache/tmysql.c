/*
FILE
	tMySQL source code of mysqlApache2.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2009 for Unixservice
	$Id: module.c.template 2459 2009-02-11 12:04:10Z Gary $
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tmysqlfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uMySQL: Primary Key
static unsigned uMySQL=0;
//cLabel: Short label
static char cLabel[33]={""};
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



#define VAR_LIST_tMySQL "tMySQL.uMySQL,tMySQL.cLabel,tMySQL.uOwner,tMySQL.uCreatedBy,tMySQL.uCreatedDate,tMySQL.uModBy,tMySQL.uModDate"

 //Local only
void Insert_tMySQL(void);
void Update_tMySQL(char *cRowid);
void ProcesstMySQLListVars(pentry entries[], int x);

 //In tMySQLfunc.h file included below
void ExtProcesstMySQLVars(pentry entries[], int x);
void ExttMySQLCommands(pentry entries[], int x);
void ExttMySQLButtons(void);
void ExttMySQLNavBar(void);
void ExttMySQLGetHook(entry gentries[], int x);
void ExttMySQLSelect(void);
void ExttMySQLSelectRow(void);
void ExttMySQLListSelect(void);
void ExttMySQLListFilter(void);
void ExttMySQLAuxTable(void);

#include "tmysqlfunc.h"

 //Table Variables Assignment Function
void ProcesstMySQLVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uMySQL"))
			sscanf(entries[i].val,"%u",&uMySQL);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
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
	ExtProcesstMySQLVars(entries,x);

}//ProcesstMySQLVars()


void ProcesstMySQLListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uMySQL);
                        guMode=2002;
                        tMySQL("");
                }
        }
}//void ProcesstMySQLListVars(pentry entries[], int x)


int tMySQLCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttMySQLCommands(entries,x);

	if(!strcmp(gcFunction,"tMySQLTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tMySQLList();
		}

		//Default
		ProcesstMySQLVars(entries,x);
		tMySQL("");
	}
	else if(!strcmp(gcFunction,"tMySQLList"))
	{
		ProcessControlVars(entries,x);
		ProcesstMySQLListVars(entries,x);
		tMySQLList();
	}

	return(0);

}//tMySQLCommands()


void tMySQL(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttMySQLSelectRow();
		else
			ExttMySQLSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetMySQL();
				mysqlApache2("New tMySQL table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tMySQL WHERE uMySQL=%u"
						,uMySQL);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uMySQL);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tMySQL",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tMySQLTools>");
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

        ExttMySQLButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tMySQL Record Data",100);

	if(guMode==2000 || guMode==2002)
		tMySQLInput(1);
	else
		tMySQLInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttMySQLAuxTable();

	Footer_ism3();

}//end of tMySQL();


void tMySQLInput(unsigned uMode)
{

//uMySQL
	OpenRow(LANG_FL_tMySQL_uMySQL,"black");
	printf("<input title='%s' type=text name=uMySQL value=%u size=16 maxlength=10 "
,LANG_FT_tMySQL_uMySQL,uMySQL);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uMySQL value=%u >\n",uMySQL);
	}
//cLabel
	OpenRow(LANG_FL_tMySQL_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tMySQL_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uOwner
	OpenRow(LANG_FL_tMySQL_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tMySQL_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tMySQL_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tMySQL_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tMySQL_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tMySQLInput(unsigned uMode)


void NewtMySQL(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uMySQL FROM tMySQL\
				WHERE uMySQL=%u"
							,uMySQL);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tMySQL("<blink>Record already exists");
		tMySQL(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tMySQL();
	//sprintf(gcQuery,"New record %u added");
	uMySQL=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tMySQL",uMySQL);
	mysqlApache2Log(uMySQL,"tMySQL","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uMySQL);
	tMySQL(gcQuery);
	}

}//NewtMySQL(unsigned uMode)


void DeletetMySQL(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tMySQL WHERE uMySQL=%u AND ( uOwner=%u OR %u>9 )"
					,uMySQL,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tMySQL WHERE uMySQL=%u"
					,uMySQL);
#endif
	macro_mySQLQueryHTMLError;
	//tMySQL("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		mysqlApache2Log(uMySQL,"tMySQL","Del");
#endif
		tMySQL(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		mysqlApache2Log(uMySQL,"tMySQL","DelError");
#endif
		tMySQL(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetMySQL(void)


void Insert_tMySQL(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tMySQL SET uMySQL=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uMySQL
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
			);

	macro_mySQLQueryHTMLError;

}//void Insert_tMySQL(void)


void Update_tMySQL(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tMySQL SET uMySQL=%u,cLabel='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uMySQL
			,TextAreaSave(cLabel)
			,uModBy
			,cRowid);

	macro_mySQLQueryHTMLError;

}//void Update_tMySQL(void)


void ModtMySQL(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	sprintf(gcQuery,"SELECT uMySQL,uModDate FROM tMySQL\
				WHERE uMySQL=%u"
						,uMySQL);

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tMySQL("<blink>Record does not exist");
	if(i<1) tMySQL(LANG_NBR_RECNOTEXIST);
	//if(i>1) tMySQL("<blink>Multiple rows!");
	if(i>1) tMySQL(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tMySQL(LANG_NBR_EXTMOD);
#endif

	Update_tMySQL(field[0]);
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tMySQL",uMySQL);
	mysqlApache2Log(uMySQL,"tMySQL","Mod");
#endif
	tMySQL(gcQuery);

}//ModtMySQL(void)


void tMySQLList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttMySQLListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tMySQLList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttMySQLListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uMySQL<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime4=strtoul(field[4],NULL,10);
		char cBuf4[32];
		if(luTime4)
			ctime_r(&luTime4,cBuf4);
		else
			sprintf(cBuf4,"---");
		time_t luTime6=strtoul(field[6],NULL,10);
		char cBuf6[32];
		if(luTime6)
			ctime_r(&luTime6,cBuf6);
		else
			sprintf(cBuf6,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[2],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[3],NULL,10))
			,cBuf4
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,cBuf6
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tMySQLList()


void CreatetMySQL(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tMySQL ( uMySQL INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetMySQL()

