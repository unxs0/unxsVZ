/*
FILE
	tRule source code of unxsSPS.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	$Id: tmonth.c 166 2009-06-05 22:10:35Z Dylan $
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tmonthfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uRule: Primary Key
static unsigned uRule=0;
//cLabel: Name of Archive Table Ex. tNov2009
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



#define VAR_LIST_tRule "tRule.uRule,tRule.cLabel,tRule.uOwner,tRule.uCreatedBy,tRule.uCreatedDate,tRule.uModBy,tRule.uModDate"

 //Local only
void Insert_tRule(void);
void Update_tRule(char *cRowid);
void ProcesstRuleListVars(pentry entries[], int x);

 //In tRulefunc.h file included below
void ExtProcesstRuleVars(pentry entries[], int x);
void ExttRuleCommands(pentry entries[], int x);
void ExttRuleButtons(void);
void ExttRuleNavBar(void);
void ExttRuleGetHook(entry gentries[], int x);
void ExttRuleSelect(void);
void ExttRuleSelectRow(void);
void ExttRuleListSelect(void);
void ExttRuleListFilter(void);
void ExttRuleAuxTable(void);

#include "trulefunc.h"

 //Table Variables Assignment Function
void ProcesstRuleVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uRule"))
			sscanf(entries[i].val,"%u",&uRule);
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
	ExtProcesstRuleVars(entries,x);

}//ProcesstRuleVars()


void ProcesstRuleListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uRule);
                        guMode=2002;
                        tRule("");
                }
        }
}//void ProcesstRuleListVars(pentry entries[], int x)


int tRuleCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttRuleCommands(entries,x);

	if(!strcmp(gcFunction,"tRuleTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tRuleList();
		}

		//Default
		ProcesstRuleVars(entries,x);
		tRule("");
	}
	else if(!strcmp(gcFunction,"tRuleList"))
	{
		ProcessControlVars(entries,x);
		ProcesstRuleListVars(entries,x);
		tRuleList();
	}

	return(0);

}//tRuleCommands()


void tRule(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttRuleSelectRow();
		else
			ExttRuleSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetRule();
				unxsSPS("New tRule table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tRule WHERE uRule=%u"
						,uRule);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uRule);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tRule",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tRuleTools>");
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

        ExttRuleButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tRule Record Data",100);

	if(guMode==2000 || guMode==2002)
		tRuleInput(1);
	else
		tRuleInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttRuleAuxTable();

	Footer_ism3();

}//end of tRule();


void tRuleInput(unsigned uMode)
{

//uRule
	OpenRow(LANG_FL_tRule_uRule,"black");
	printf("<input title='%s' type=text name=uRule value=%u size=16 maxlength=10 "
,LANG_FT_tRule_uRule,uRule);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uRule value=%u >\n",uRule);
	}
//cLabel
	OpenRow(LANG_FL_tRule_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tRule_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tRule_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tRule_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tRule_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tRule_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tRule_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tRuleInput(unsigned uMode)


void NewtRule(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uRule FROM tRule\
				WHERE uRule=%u"
							,uRule);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tRule("<blink>Record already exists");
		tRule(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tRule();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uRule=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tRule",uRule);
	unxsSPSLog(uRule,"tRule","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uRule);
	tRule(gcQuery);
	}

}//NewtRule(unsigned uMode)


void DeletetRule(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tRule WHERE uRule=%u AND ( uOwner=%u OR %u>9 )"
					,uRule,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tRule WHERE uRule=%u"
					,uRule);
#endif
	MYSQL_RUN;
	//tRule("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsSPSLog(uRule,"tRule","Del");
#endif
		tRule(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsSPSLog(uRule,"tRule","DelError");
#endif
		tRule(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetRule(void)


void Insert_tRule(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tRule SET uRule=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uRule
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
			);

	MYSQL_RUN;

}//void Insert_tRule(void)


void Update_tRule(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tRule SET uRule=%u,cLabel='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uRule
			,TextAreaSave(cLabel)
			,uModBy
			,cRowid);

	MYSQL_RUN;

}//void Update_tRule(void)


void ModtRule(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tRule.uRule,\
				tRule.uModDate\
				FROM tRule,tClient\
				WHERE tRule.uRule=%u\
				AND tRule.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uRule,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uRule,uModDate FROM tRule\
				WHERE uRule=%u"
						,uRule);
#else
	sprintf(gcQuery,"SELECT uRule FROM tRule\
				WHERE uRule=%u"
						,uRule);
#endif

	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tRule("<blink>Record does not exist");
	if(i<1) tRule(LANG_NBR_RECNOTEXIST);
	//if(i>1) tRule("<blink>Multiple rows!");
	if(i>1) tRule(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tRule(LANG_NBR_EXTMOD);
#endif

	Update_tRule(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tRule",uRule);
	unxsSPSLog(uRule,"tRule","Mod");
#endif
	tRule(gcQuery);

}//ModtRule(void)


void tRuleList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttRuleListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tRuleList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttRuleListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uRule<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
			,ForeignKey("tClient","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[3],NULL,10))
			,cBuf4
			,ForeignKey("tClient","cLabel",strtoul(field[5],NULL,10))
			,cBuf6
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tRuleList()


void CreatetRule(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tRule ( uRule INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', UNIQUE (cLabel), uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	MYSQL_RUN;

}//CreatetRule()

