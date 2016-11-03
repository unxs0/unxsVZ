/*
FILE
	tWhiteList source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2009 for Unixservice
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in twhitelistfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uWhiteList: Primary Key
static unsigned uWhiteList=0;
//cLabel: Short label
static char cLabel[33]={""};
static unsigned uUser=0;
//uServerGroup: Server group the entry is at
static unsigned uServerGroup=0;
static char cuServerGroupPullDown[256]={""};
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



#define VAR_LIST_tWhiteList "tWhiteList.uWhiteList,tWhiteList.cLabel,tWhiteList.uServerGroup,tWhiteList.uOwner,tWhiteList.uCreatedBy,tWhiteList.uCreatedDate,tWhiteList.uModBy,tWhiteList.uModDate,tWhiteList.uUser"

 //Local only
void Insert_tWhiteList(void);
void Update_tWhiteList(char *cRowid);
void ProcesstWhiteListListVars(pentry entries[], int x);

 //In tWhiteListfunc.h file included below
void ExtProcesstWhiteListVars(pentry entries[], int x);
void ExttWhiteListCommands(pentry entries[], int x);
void ExttWhiteListButtons(void);
void ExttWhiteListNavBar(void);
void ExttWhiteListGetHook(entry gentries[], int x);
void ExttWhiteListSelect(void);
void ExttWhiteListSelectRow(void);
void ExttWhiteListListSelect(void);
void ExttWhiteListListFilter(void);
void ExttWhiteListAuxTable(void);

#include "twhitelistfunc.h"

 //Table Variables Assignment Function
void ProcesstWhiteListVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uWhiteList"))
			sscanf(entries[i].val,"%u",&uWhiteList);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uUser"))
			sscanf(entries[i].val,"%u",&uUser);
		else if(!strcmp(entries[i].name,"uServerGroup"))
			sscanf(entries[i].val,"%u",&uServerGroup);
		else if(!strcmp(entries[i].name,"cuServerGroupPullDown"))
		{
			sprintf(cuServerGroupPullDown,"%.255s",entries[i].val);
			uServerGroup=ReadPullDown("tServerGroup","cLabel",cuServerGroupPullDown);
		}
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
	ExtProcesstWhiteListVars(entries,x);

}//ProcesstWhiteListVars()


void ProcesstWhiteListListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uWhiteList);
                        guMode=2002;
                        tWhiteList("");
                }
        }
}//void ProcesstWhiteListListVars(pentry entries[], int x)


int tWhiteListCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttWhiteListCommands(entries,x);

	if(!strcmp(gcFunction,"tWhiteListTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tWhiteListList();
		}

		//Default
		ProcesstWhiteListVars(entries,x);
		tWhiteList("");
	}
	else if(!strcmp(gcFunction,"tWhiteListList"))
	{
		ProcessControlVars(entries,x);
		ProcesstWhiteListListVars(entries,x);
		tWhiteListList();
	}

	return(0);

}//tWhiteListCommands()


void tWhiteList(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttWhiteListSelectRow();
		else
			ExttWhiteListSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetWhiteList();
				unxsMail("New tWhiteList table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tWhiteList WHERE uWhiteList=%u"
						,uWhiteList);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uWhiteList);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uServerGroup);
		sscanf(field[3],"%u",&uOwner);
		sscanf(field[4],"%u",&uCreatedBy);
		sscanf(field[5],"%lu",&uCreatedDate);
		sscanf(field[6],"%u",&uModBy);
		sscanf(field[7],"%lu",&uModDate);
		sscanf(field[8],"%u",&uUser);
		
		}

	}//Internal Skip

	Header_ism3(":: tWhiteList",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tWhiteListTools>");
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

        ExttWhiteListButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tWhiteList Record Data",100);

	if(guMode==2000 || guMode==2002)
		tWhiteListInput(1);
	else
		tWhiteListInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttWhiteListAuxTable();

	Footer_ism3();

}//end of tWhiteList();


void tWhiteListInput(unsigned uMode)
{

//uWhiteList
	OpenRow(LANG_FL_tWhiteList_uWhiteList,"black");
	printf("<input title='%s' type=text name=uWhiteList value=%u size=16 maxlength=10 "
,LANG_FT_tWhiteList_uWhiteList,uWhiteList);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uWhiteList value=%u >\n",uWhiteList);
	}
//cLabel
	OpenRow(LANG_FL_tWhiteList_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tWhiteList_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uUser
	OpenRow(LANG_FL_tWhiteList_uUser,"black");
	printf("<input title='%s' type=text name=uUser value=\"%u\" size=40 maxlength=32 "
,LANG_FT_tWhiteList_uUser,uUser);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uUser value=\"%u\">\n",uUser);
	}
//uServerGroup
	OpenRow(LANG_FL_tWhiteList_uServerGroup,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,1);
	else
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,0);
//uOwner
	OpenRow(LANG_FL_tWhiteList_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tWhiteList_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tWhiteList_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tWhiteList_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tWhiteList_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tWhiteListInput(unsigned uMode)


void NewtWhiteList(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uWhiteList FROM tWhiteList\
				WHERE uWhiteList=%u"
							,uWhiteList);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tWhiteList("<blink>Record already exists");
		tWhiteList(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tWhiteList();
	//sprintf(gcQuery,"New record %u added");
	uWhiteList=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tWhiteList",uWhiteList);
	unxsMailLog(uWhiteList,"tWhiteList","New");

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uWhiteList);
	tWhiteList(gcQuery);
	}

}//NewtWhiteList(unsigned uMode)


void DeletetWhiteList(void)
{
	sprintf(gcQuery,"DELETE FROM tWhiteList WHERE uWhiteList=%u AND ( uOwner=%u OR %u>9 )"
					,uWhiteList,guLoginClient,guPermLevel);
	macro_mySQLQueryHTMLError;
	//tWhiteList("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsMailLog(uWhiteList,"tWhiteList","Del");
		tWhiteList(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsMailLog(uWhiteList,"tWhiteList","DelError");
		tWhiteList(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetWhiteList(void)


void Insert_tWhiteList(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tWhiteList SET uWhiteList=%u,cLabel='%s',uUser=%u,uServerGroup=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uWhiteList
			,TextAreaSave(cLabel)
			,uUser
			,uServerGroup
			,uOwner
			,uCreatedBy
			);

	macro_mySQLQueryHTMLError;

}//void Insert_tWhiteList(void)


void Update_tWhiteList(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tWhiteList SET uWhiteList=%u,cLabel='%s',uUser=%u,uServerGroup=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uWhiteList
			,TextAreaSave(cLabel)
			,uUser
			,uServerGroup
			,uModBy
			,cRowid);

	macro_mySQLQueryHTMLError;

}//void Update_tWhiteList(void)


void ModtWhiteList(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uWhiteList,uModDate FROM tWhiteList WHERE uWhiteList=%u",uWhiteList);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tWhiteList("<blink>Record does not exist");
	if(i<1) tWhiteList(LANG_NBR_RECNOTEXIST);
	//if(i>1) tWhiteList("<blink>Multiple rows!");
	if(i>1) tWhiteList(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tWhiteList(LANG_NBR_EXTMOD);

	Update_tWhiteList(field[0]);
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tWhiteList",uWhiteList);
	unxsMailLog(uWhiteList,"tWhiteList","Mod");
	tWhiteList(gcQuery);

}//ModtWhiteList(void)


void tWhiteListList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttWhiteListListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tWhiteListList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttWhiteListListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uWhiteList<td><font face=arial,helvetica color=white>cLabel<font face=arial,helvetica color=white>uUser<td><font face=arial,helvetica color=white>uServerGroup<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime5=strtoul(field[5],NULL,10);
		char cBuf5[32];
		if(luTime5)
			ctime_r(&luTime5,cBuf5);
		else
			sprintf(cBuf5,"---");
		time_t luTime7=strtoul(field[7],NULL,10);
		char cBuf7[32];
		if(luTime7)
			ctime_r(&luTime7,cBuf7);
		else
			sprintf(cBuf7,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey("tUser","cLogin",strtoul(field[8],NULL,10))
			,ForeignKey("tServerGroup","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[3],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[4],NULL,10))
			,cBuf5
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,cBuf7
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tWhiteListList()


void CreatetWhiteList(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tWhiteList ( uWhiteList INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uServerGroup INT UNSIGNED NOT NULL DEFAULT 0, uUser INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetWhiteList()

