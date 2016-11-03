/*
FILE
	tServiceGlue source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tservicegluefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uServiceGlue: Primary Key
static unsigned uServiceGlue=0;
//uServiceGroup: Product with one or more associated services
static unsigned uServiceGroup=0;
//uService: Service grouped by product
static unsigned uService=0;



#define VAR_LIST_tServiceGlue "tServiceGlue.uServiceGlue,tServiceGlue.uServiceGroup,tServiceGlue.uService"

 //Local only
void Insert_tServiceGlue(void);
void Update_tServiceGlue(char *cRowid);
void ProcesstServiceGlueListVars(pentry entries[], int x);

 //In tServiceGluefunc.h file included below
void ExtProcesstServiceGlueVars(pentry entries[], int x);
void ExttServiceGlueCommands(pentry entries[], int x);
void ExttServiceGlueButtons(void);
void ExttServiceGlueNavBar(void);
void ExttServiceGlueGetHook(entry gentries[], int x);
void ExttServiceGlueSelect(void);
void ExttServiceGlueSelectRow(void);
void ExttServiceGlueListSelect(void);
void ExttServiceGlueListFilter(void);
void ExttServiceGlueAuxTable(void);

#include "tservicegluefunc.h"

 //Table Variables Assignment Function
void ProcesstServiceGlueVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uServiceGlue"))
			sscanf(entries[i].val,"%u",&uServiceGlue);
		else if(!strcmp(entries[i].name,"uServiceGroup"))
			sscanf(entries[i].val,"%u",&uServiceGroup);
		else if(!strcmp(entries[i].name,"uService"))
			sscanf(entries[i].val,"%u",&uService);

	}

	//After so we can overwrite form data if needed.
	ExtProcesstServiceGlueVars(entries,x);

}//ProcesstServiceGlueVars()


void ProcesstServiceGlueListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uServiceGlue);
                        guMode=2002;
                        tServiceGlue("");
                }
        }
}//void ProcesstServiceGlueListVars(pentry entries[], int x)


int tServiceGlueCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttServiceGlueCommands(entries,x);

	if(!strcmp(gcFunction,"tServiceGlueTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tServiceGlueList();
		}

		//Default
		ProcesstServiceGlueVars(entries,x);
		tServiceGlue("");
	}
	else if(!strcmp(gcFunction,"tServiceGlueList"))
	{
		ProcessControlVars(entries,x);
		ProcesstServiceGlueListVars(entries,x);
		tServiceGlueList();
	}

	return(0);

}//tServiceGlueCommands()


void tServiceGlue(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttServiceGlueSelectRow();
		else
			ExttServiceGlueSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetServiceGlue();
				unxsISP("New tServiceGlue table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tServiceGlue WHERE uServiceGlue=%u"
						,uServiceGlue);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uServiceGlue);
		sscanf(field[1],"%u",&uServiceGroup);
		sscanf(field[2],"%u",&uService);

		}

	}//Internal Skip

	Header_ism3(":: tServiceGlue",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tServiceGlueTools>");
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

        ExttServiceGlueButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tServiceGlue Record Data",100);

	if(guMode==2000 || guMode==2002)
		tServiceGlueInput(1);
	else
		tServiceGlueInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttServiceGlueAuxTable();

	Footer_ism3();

}//end of tServiceGlue();


void tServiceGlueInput(unsigned uMode)
{

//uServiceGlue
	OpenRow(LANG_FL_tServiceGlue_uServiceGlue,"black");
	printf("<input title='%s' type=text name=uServiceGlue value=%u size=16 maxlength=10 "
,LANG_FT_tServiceGlue_uServiceGlue,uServiceGlue);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uServiceGlue value=%u >\n",uServiceGlue);
	}
//uServiceGroup
	OpenRow(LANG_FL_tServiceGlue_uServiceGroup,"black");
	printf("<input title='%s' type=text name=uServiceGroup value=%u size=16 maxlength=10 "
,LANG_FT_tServiceGlue_uServiceGroup,uServiceGroup);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uServiceGroup value=%u >\n",uServiceGroup);
	}
//uService
	OpenRow(LANG_FL_tServiceGlue_uService,"black");
	printf("<input title='%s' type=text name=uService value=%u size=16 maxlength=10 "
,LANG_FT_tServiceGlue_uService,uService);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uService value=%u >\n",uService);
	}
	printf("</tr>\n");



}//void tServiceGlueInput(unsigned uMode)


void NewtServiceGlue(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uServiceGlue FROM tServiceGlue\
				WHERE uServiceGlue=%u"
							,uServiceGlue);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tServiceGlue("<blink>Record already exists");
		tServiceGlue(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tServiceGlue();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uServiceGlue=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tServiceGlue",uServiceGlue);
	unxsISPLog(uServiceGlue,"tServiceGlue","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uServiceGlue);
	tServiceGlue(gcQuery);
	}

}//NewtServiceGlue(unsigned uMode)


void DeletetServiceGlue(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tServiceGlue WHERE uServiceGlue=%u AND ( uOwner=%u OR %u>9 )"
					,uServiceGlue,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tServiceGlue WHERE uServiceGlue=%u"
					,uServiceGlue);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tServiceGlue("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uServiceGlue,"tServiceGlue","Del");
#endif
		tServiceGlue(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uServiceGlue,"tServiceGlue","DelError");
#endif
		tServiceGlue(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetServiceGlue(void)


void Insert_tServiceGlue(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tServiceGlue SET uServiceGlue=%u,uServiceGroup=%u,uService=%u",
			uServiceGlue
			,uServiceGroup
			,uService
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tServiceGlue(void)


void Update_tServiceGlue(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tServiceGlue SET uServiceGlue=%u,uServiceGroup=%u,uService=%u WHERE _rowid=%s",
			uServiceGlue
			,uServiceGroup
			,uService
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tServiceGlue(void)


void ModtServiceGlue(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uServiceGlue,uModDate FROM tServiceGlue WHERE uServiceGlue=%u"
						,uServiceGlue);
#else
	sprintf(gcQuery,"SELECT uServiceGlue FROM tServiceGlue WHERE uServiceGlue=%u"
						,uServiceGlue);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tServiceGlue("<blink>Record does not exist");
	if(i<1) tServiceGlue(LANG_NBR_RECNOTEXIST);
	//if(i>1) tServiceGlue("<blink>Multiple rows!");
	if(i>1) tServiceGlue(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tServiceGlue(LANG_NBR_EXTMOD);
#endif

	Update_tServiceGlue(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tServiceGlue",uServiceGlue);
	unxsISPLog(uServiceGlue,"tServiceGlue","Mod");
#endif
	tServiceGlue(gcQuery);

}//ModtServiceGlue(void)


void tServiceGlueList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttServiceGlueListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tServiceGlueList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttServiceGlueListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uServiceGlue<td><font face=arial,helvetica color=white>uServiceGroup<td><font face=arial,helvetica color=white>uService</tr>");



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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tServiceGlueList()


void CreatetServiceGlue(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tServiceGlue ( uServiceGlue INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uService INT UNSIGNED NOT NULL DEFAULT 0, uServiceGroup INT UNSIGNED NOT NULL DEFAULT 0,index (uServiceGroup) )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetServiceGlue()

