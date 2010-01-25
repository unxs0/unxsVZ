/*
FILE
	tServerGroup source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	$Id$
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tservergroupfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uServerGroup: Primary Key
static unsigned uServerGroup=0;
//cLabel: Short label
static char cLabel[33]={""};
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



#define VAR_LIST_tServerGroup "tServerGroup.uServerGroup,tServerGroup.cLabel,tServerGroup.uOwner,tServerGroup.uCreatedBy,tServerGroup.uCreatedDate,tServerGroup.uModBy,tServerGroup.uModDate"

 //Local only
void Insert_tServerGroup(void);
void Update_tServerGroup(char *cRowid);
void ProcesstServerGroupListVars(pentry entries[], int x);

 //In tServerGroupfunc.h file included below
void ExtProcesstServerGroupVars(pentry entries[], int x);
void ExttServerGroupCommands(pentry entries[], int x);
void ExttServerGroupButtons(void);
void ExttServerGroupNavBar(void);
void ExttServerGroupGetHook(entry gentries[], int x);
void ExttServerGroupSelect(void);
void ExttServerGroupSelectRow(void);
void ExttServerGroupListSelect(void);
void ExttServerGroupListFilter(void);
void ExttServerGroupAuxTable(void);

#include "tservergroupfunc.h"

 //Table Variables Assignment Function
void ProcesstServerGroupVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uServerGroup"))
			sscanf(entries[i].val,"%u",&uServerGroup);
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
	ExtProcesstServerGroupVars(entries,x);

}//ProcesstServerGroupVars()


void ProcesstServerGroupListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uServerGroup);
                        guMode=2002;
                        tServerGroup("");
                }
        }
}//void ProcesstServerGroupListVars(pentry entries[], int x)


int tServerGroupCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttServerGroupCommands(entries,x);

	if(!strcmp(gcFunction,"tServerGroupTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tServerGroupList();
		}

		//Default
		ProcesstServerGroupVars(entries,x);
		tServerGroup("");
	}
	else if(!strcmp(gcFunction,"tServerGroupList"))
	{
		ProcessControlVars(entries,x);
		ProcesstServerGroupListVars(entries,x);
		tServerGroupList();
	}

	return(0);

}//tServerGroupCommands()


void tServerGroup(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttServerGroupSelectRow();
		else
			ExttServerGroupSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetServerGroup();
				unxsMail("New tServerGroup table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tServerGroup WHERE uServerGroup=%u"
						,uServerGroup);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uServerGroup);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tServerGroup",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tServerGroupTools>");
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

        ExttServerGroupButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tServerGroup Record Data",100);

	if(guMode==2000 || guMode==2002)
		tServerGroupInput(1);
	else
		tServerGroupInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttServerGroupAuxTable();

	Footer_ism3();

}//end of tServerGroup();


void tServerGroupInput(unsigned uMode)
{

//uServerGroup
	OpenRow(LANG_FL_tServerGroup_uServerGroup,"black");
	printf("<input title='%s' type=text name=uServerGroup value=%u size=16 maxlength=10 "
,LANG_FT_tServerGroup_uServerGroup,uServerGroup);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uServerGroup value=%u >\n",uServerGroup);
	}
//cLabel
	OpenRow(LANG_FL_tServerGroup_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tServerGroup_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tServerGroup_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tServerGroup_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tServerGroup_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tServerGroup_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tServerGroup_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tServerGroupInput(unsigned uMode)


void NewtServerGroup(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uServerGroup FROM tServerGroup\
				WHERE uServerGroup=%u"
							,uServerGroup);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tServerGroup("<blink>Record already exists");
		tServerGroup(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tServerGroup();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uServerGroup=mysql_insert_id(&gMysql);

	uCreatedDate=luGetCreatedDate("tServerGroup",uServerGroup);
	unxsMailLog(uServerGroup,"tServerGroup","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uServerGroup);
	tServerGroup(gcQuery);
	}

}//NewtServerGroup(unsigned uMode)


void DeletetServerGroup(void)
{

	sprintf(gcQuery,"DELETE FROM tServerGroup WHERE uServerGroup=%u AND ( uOwner=%u OR %u>9 )"
					,uServerGroup,guLoginClient,guPermLevel);

	sprintf(gcQuery,"DELETE FROM tServerGroup WHERE uServerGroup=%u"
					,uServerGroup);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tServerGroup("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uServerGroup,"tServerGroup","Del");

		tServerGroup(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uServerGroup,"tServerGroup","DelError");

		tServerGroup(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetServerGroup(void)


void Insert_tServerGroup(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tServerGroup SET uServerGroup=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uServerGroup
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tServerGroup(void)


void Update_tServerGroup(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tServerGroup SET uServerGroup=%u,cLabel='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uServerGroup
			,TextAreaSave(cLabel)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tServerGroup(void)


void ModtServerGroup(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uServerGroup,uModDate FROM tServerGroup WHERE uServerGroup=%u"
						,uServerGroup);

	sprintf(gcQuery,"SELECT uServerGroup FROM tServerGroup\
				WHERE uServerGroup=%u"
						,uServerGroup);


	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tServerGroup("<blink>Record does not exist");
	if(i<1) tServerGroup(LANG_NBR_RECNOTEXIST);
	//if(i>1) tServerGroup("<blink>Multiple rows!");
	if(i>1) tServerGroup(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tServerGroup(LANG_NBR_EXTMOD);


	Update_tServerGroup(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);

	uModDate=luGetModDate("tServerGroup",uServerGroup);
	unxsMailLog(uServerGroup,"tServerGroup","Mod");

	tServerGroup(gcQuery);

}//ModtServerGroup(void)


void tServerGroupList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttServerGroupListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tServerGroupList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttServerGroupListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uServerGroup<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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

}//tServerGroupList()


void CreatetServerGroup(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tServerGroup ( uServerGroup INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetServerGroup()

