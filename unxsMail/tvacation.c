/*
FILE
	tVacation source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2009 for Unixservice
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tvacationfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uVacation: Primary Key
static unsigned uVacation=0;
//uUser: tUser row id
static unsigned uUser=0;
//uHTML: Is html yes/no
static unsigned uHTML=0;
static char cYesNouHTML[32]={""};
//cText: Autoreply text
static char *cText={""};
//uOwner: Record owner
static unsigned uOwner=0;
//uCreatedBy: uClient for last insert
static unsigned uCreatedBy=0;
//uCreatedDate: Unix seconds date last insert
static time_t uCreatedDate=0;
//uModBy: uClient for last update
static unsigned uModBy=0;
//uModDate: Unix seconds date last update
static time_t uModDate=0;



#define VAR_LIST_tVacation "tVacation.uVacation,tVacation.uUser,tVacation.uHTML,tVacation.cText,tVacation.uOwner,tVacation.uCreatedBy,tVacation.uCreatedDate,tVacation.uModBy,tVacation.uModDate"

 //Local only
void Insert_tVacation(void);
void Update_tVacation(char *cRowid);
void ProcesstVacationListVars(pentry entries[], int x);

 //In tVacationfunc.h file included below
void ExtProcesstVacationVars(pentry entries[], int x);
void ExttVacationCommands(pentry entries[], int x);
void ExttVacationButtons(void);
void ExttVacationNavBar(void);
void ExttVacationGetHook(entry gentries[], int x);
void ExttVacationSelect(void);
void ExttVacationSelectRow(void);
void ExttVacationListSelect(void);
void ExttVacationListFilter(void);
void ExttVacationAuxTable(void);

#include "tvacationfunc.h"

 //Table Variables Assignment Function
void ProcesstVacationVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uVacation"))
			sscanf(entries[i].val,"%u",&uVacation);
		else if(!strcmp(entries[i].name,"uUser"))
			sscanf(entries[i].val,"%u",&uUser);
		else if(!strcmp(entries[i].name,"uHTML"))
			sscanf(entries[i].val,"%u",&uHTML);
		else if(!strcmp(entries[i].name,"cYesNouHTML"))
		{
			sprintf(cYesNouHTML,"%.31s",entries[i].val);
			uHTML=ReadYesNoPullDown(cYesNouHTML);
		}
		else if(!strcmp(entries[i].name,"cText"))
			cText=entries[i].val;
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
	ExtProcesstVacationVars(entries,x);

}//ProcesstVacationVars()


void ProcesstVacationListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uVacation);
                        guMode=2002;
                        tVacation("");
                }
        }
}//void ProcesstVacationListVars(pentry entries[], int x)


int tVacationCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttVacationCommands(entries,x);

	if(!strcmp(gcFunction,"tVacationTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tVacationList();
		}

		//Default
		ProcesstVacationVars(entries,x);
		tVacation("");
	}
	else if(!strcmp(gcFunction,"tVacationList"))
	{
		ProcessControlVars(entries,x);
		ProcesstVacationListVars(entries,x);
		tVacationList();
	}

	return(0);

}//tVacationCommands()


void tVacation(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttVacationSelectRow();
		else
			ExttVacationSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetVacation();
				unxsMail("New tVacation table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tVacation WHERE uVacation=%u"
						,uVacation);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uVacation);
		sscanf(field[1],"%u",&uUser);
		sscanf(field[2],"%u",&uHTML);
		cText=field[3];
		sscanf(field[4],"%u",&uOwner);
		sscanf(field[5],"%u",&uCreatedBy);
		sscanf(field[6],"%lu",&uCreatedDate);
		sscanf(field[7],"%u",&uModBy);
		sscanf(field[8],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tVacation",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tVacationTools>");
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

        ExttVacationButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tVacation Record Data",100);

	if(guMode==2000 || guMode==2002)
		tVacationInput(1);
	else
		tVacationInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttVacationAuxTable();

	Footer_ism3();

}//end of tVacation();


void tVacationInput(unsigned uMode)
{

//uVacation
	OpenRow(LANG_FL_tVacation_uVacation,"black");
	printf("<input title='%s' type=text name=uVacation value=%u size=16 maxlength=10 "
,LANG_FT_tVacation_uVacation,uVacation);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uVacation value=%u >\n",uVacation);
	}
//uUser
	OpenRow(LANG_FL_tVacation_uUser,"black");
	printf("<input title='%s' type=text name=uUser value=%u size=16 maxlength=32 "
,LANG_FT_tVacation_uUser,uUser);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uUser value=%u >\n",uUser);
	}
//uHTML
	OpenRow(LANG_FL_tVacation_uHTML,"black");
	if(guPermLevel>=0 && uMode)
		YesNoPullDown("uHTML",uHTML,1);
	else
		YesNoPullDown("uHTML",uHTML,0);
//cText
	OpenRow(LANG_FL_tVacation_cText,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cText "
,LANG_FT_tVacation_cText);
	if(guPermLevel>=0 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cText);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cText);
		printf("<input type=hidden name=cText value=\"%s\" >\n",EncodeDoubleQuotes(cText));
	}
//uOwner
	OpenRow(LANG_FL_tVacation_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tVacation_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tVacation_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tVacation_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tVacation_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tVacationInput(unsigned uMode)


void NewtVacation(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uVacation FROM tVacation\
				WHERE uVacation=%u"
							,uVacation);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tVacation("<blink>Record already exists");
		tVacation(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tVacation();
	//sprintf(gcQuery,"New record %u added");
	uVacation=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tVacation",uVacation);
	unxsMailLog(uVacation,"tVacation","New");

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uVacation);
	tVacation(gcQuery);
	}

}//NewtVacation(unsigned uMode)


void DeletetVacation(void)
{
	sprintf(gcQuery,"DELETE FROM tVacation WHERE uVacation=%u"
					,uVacation);
	macro_mySQLQueryHTMLError;
	//tVacation("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsMailLog(uVacation,"tVacation","Del");
		tVacation(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsMailLog(uVacation,"tVacation","DelError");
		tVacation(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetVacation(void)


void Insert_tVacation(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tVacation SET uVacation=%u,uUser=%u,uHTML=%u,cText='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uVacation
			,uUser
			,uHTML
			,TextAreaSave(cText)
			,uOwner
			,uCreatedBy
			);

	macro_mySQLQueryHTMLError;

}//void Insert_tVacation(void)


void Update_tVacation(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tVacation SET uVacation=%u,uUser=%u,uHTML=%u,cText='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uVacation
			,uUser
			,uHTML
			,TextAreaSave(cText)
			,uModBy
			,cRowid);

	macro_mySQLQueryHTMLError;

}//void Update_tVacation(void)


void ModtVacation(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	sprintf(gcQuery,"SELECT uVacation,uModDate FROM tVacation WHERE uVacation=%u",uVacation);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tVacation("<blink>Record does not exist");
	if(i<1) tVacation(LANG_NBR_RECNOTEXIST);
	//if(i>1) tVacation("<blink>Multiple rows!");
	if(i>1) tVacation(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tVacation(LANG_NBR_EXTMOD);

	Update_tVacation(field[0]);
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tVacation",uVacation);
	unxsMailLog(uVacation,"tVacation","Mod");
	tVacation(gcQuery);

}//ModtVacation(void)


void tVacationList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttVacationListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tVacationList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttVacationListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uVacation<td><font face=arial,helvetica color=white>uUser<td><font face=arial,helvetica color=white>uHTML<td><font face=arial,helvetica color=white>cText<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		long unsigned luYesNo2=strtoul(field[2],NULL,10);
		char cBuf2[4];
		if(luYesNo2)
			sprintf(cBuf2,"Yes");
		else
			sprintf(cBuf2,"No");
		time_t luTime6=strtoul(field[6],NULL,10);
		char cBuf6[32];
		if(luTime6)
			ctime_r(&luTime6,cBuf6);
		else
			sprintf(cBuf6,"---");
		time_t luTime8=strtoul(field[8],NULL,10);
		char cBuf8[32];
		if(luTime8)
			ctime_r(&luTime8,cBuf8);
		else
			sprintf(cBuf8,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,cBuf2
			,field[3]
			,ForeignKey("tClient","cLabel",strtoul(field[4],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[5],NULL,10))
			,cBuf6
			,ForeignKey("tClient","cLabel",strtoul(field[7],NULL,10))
			,cBuf8
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tVacationList()


void CreatetVacation(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tVacation ( uVacation INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uUser INT UNSIGNED NOT NULL DEFAULT 0, uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uHTML INT UNSIGNED NOT NULL DEFAULT 0, cText TEXT NOT NULL DEFAULT '' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetVacation()

