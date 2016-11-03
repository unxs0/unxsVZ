/*
FILE
	tMotd source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tmotdfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uMotd: Primary Key
static unsigned uMotd=0;
//cMessage: MOTD Text
static char *cMessage={""};
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



#define VAR_LIST_tMotd "tMotd.uMotd,tMotd.cMessage,tMotd.uOwner,tMotd.uCreatedBy,tMotd.uCreatedDate,tMotd.uModBy,tMotd.uModDate"

 //Local only
void Insert_tMotd(void);
void Update_tMotd(char *cRowid);
void ProcesstMotdListVars(pentry entries[], int x);

 //In tMotdfunc.h file included below
void ExtProcesstMotdVars(pentry entries[], int x);
void ExttMotdCommands(pentry entries[], int x);
void ExttMotdButtons(void);
void ExttMotdNavBar(void);
void ExttMotdGetHook(entry gentries[], int x);
void ExttMotdSelect(void);
void ExttMotdSelectRow(void);
void ExttMotdListSelect(void);
void ExttMotdListFilter(void);
void ExttMotdAuxTable(void);

#include "tmotdfunc.h"

 //Table Variables Assignment Function
void ProcesstMotdVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uMotd"))
			sscanf(entries[i].val,"%u",&uMotd);
		else if(!strcmp(entries[i].name,"cMessage"))
			cMessage=entries[i].val;
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
	ExtProcesstMotdVars(entries,x);

}//ProcesstMotdVars()


void ProcesstMotdListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uMotd);
                        guMode=2002;
                        tMotd("");
                }
        }
}//void ProcesstMotdListVars(pentry entries[], int x)


int tMotdCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttMotdCommands(entries,x);

	if(!strcmp(gcFunction,"tMotdTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tMotdList();
		}

		//Default
		ProcesstMotdVars(entries,x);
		tMotd("");
	}
	else if(!strcmp(gcFunction,"tMotdList"))
	{
		ProcessControlVars(entries,x);
		ProcesstMotdListVars(entries,x);
		tMotdList();
	}

	return(0);

}//tMotdCommands()


void tMotd(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttMotdSelectRow();
		else
			ExttMotdSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetMotd();
				unxsISP("New tMotd table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tMotd WHERE uMotd=%u"
						,uMotd);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uMotd);
		cMessage=field[1];
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: Interfaces MOTD table",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tMotdTools>");
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

        ExttMotdButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tMotd Record Data",100);

	if(guMode==2000 || guMode==2002)
		tMotdInput(1);
	else
		tMotdInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttMotdAuxTable();

	Footer_ism3();

}//end of tMotd();


void tMotdInput(unsigned uMode)
{

//uMotd
	OpenRow(LANG_FL_tMotd_uMotd,"black");
	printf("<input title='%s' type=text name=uMotd value=%u size=16 maxlength=10 "
,LANG_FT_tMotd_uMotd,uMotd);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uMotd value=%u >\n",uMotd);
	}
//cMessage
	OpenRow(LANG_FL_tMotd_cMessage,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cMessage "
,LANG_FT_tMotd_cMessage);
	if(guPermLevel>=0 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cMessage);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cMessage);
		printf("<input type=hidden name=cMessage value=\"%s\" >\n",EncodeDoubleQuotes(cMessage));
	}
//uOwner
	OpenRow(LANG_FL_tMotd_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tMotd_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tMotd_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tMotd_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tMotd_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tMotdInput(unsigned uMode)


void NewtMotd(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uMotd FROM tMotd\
				WHERE uMotd=%u"
							,uMotd);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tMotd("<blink>Record already exists");
		tMotd(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tMotd();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uMotd=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tMotd",uMotd);
	unxsISPLog(uMotd,"tMotd","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uMotd);
	tMotd(gcQuery);
	}

}//NewtMotd(unsigned uMode)


void DeletetMotd(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tMotd WHERE uMotd=%u AND ( uOwner=%u OR %u>9 )"
					,uMotd,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tMotd WHERE uMotd=%u"
					,uMotd);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tMotd("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uMotd,"tMotd","Del");
#endif
		tMotd(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uMotd,"tMotd","DelError");
#endif
		tMotd(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetMotd(void)


void Insert_tMotd(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tMotd SET uMotd=%u,cMessage='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uMotd
			,TextAreaSave(cMessage)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tMotd(void)


void Update_tMotd(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tMotd SET uMotd=%u,cMessage='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uMotd
			,TextAreaSave(cMessage)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tMotd(void)


void ModtMotd(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uMotd,uModDate FROM tMotd WHERE uMotd=%u"
						,uMotd);
#else
	sprintf(gcQuery,"SELECT uMotd FROM tMotd WHERE uMotd=%u"
						,uMotd);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tMotd("<blink>Record does not exist");
	if(i<1) tMotd(LANG_NBR_RECNOTEXIST);
	//if(i>1) tMotd("<blink>Multiple rows!");
	if(i>1) tMotd(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tMotd(LANG_NBR_EXTMOD);
#endif

	Update_tMotd(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tMotd",uMotd);
	unxsISPLog(uMotd,"tMotd","Mod");
#endif
	tMotd(gcQuery);

}//ModtMotd(void)


void tMotdList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttMotdListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tMotdList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttMotdListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uMotd<td><font face=arial,helvetica color=white>cMessage<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
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

}//tMotdList()


void CreatetMotd(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tMotd ( uMotd INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cMessage TEXT NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetMotd()

