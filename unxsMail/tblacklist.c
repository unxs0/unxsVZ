/*
FILE
	tBlackList source code of unxsMail.cgi
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
//uBlackList: Primary Key
static unsigned uBlackList=0;
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



#define VAR_LIST_tBlackList "tBlackList.uBlackList,tBlackList.cLabel,tBlackList.uServerGroup,tBlackList.uOwner,tBlackList.uCreatedBy,tBlackList.uCreatedDate,tBlackList.uModBy,tBlackList.uModDate,tBlackList.uUser"

 //Local only
void Insert_tBlackList(void);
void Update_tBlackList(char *cRowid);
void ProcesstBlackListListVars(pentry entries[], int x);

 //In tBlackListfunc.h file included below
void ExtProcesstBlackListVars(pentry entries[], int x);
void ExttBlackListCommands(pentry entries[], int x);
void ExttBlackListButtons(void);
void ExttBlackListNavBar(void);
void ExttBlackListGetHook(entry gentries[], int x);
void ExttBlackListSelect(void);
void ExttBlackListSelectRow(void);
void ExttBlackListListSelect(void);
void ExttBlackListListFilter(void);
void ExttBlackListAuxTable(void);

#include "tblacklistfunc.h"

 //Table Variables Assignment Function
void ProcesstBlackListVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uBlackList"))
			sscanf(entries[i].val,"%u",&uBlackList);
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
	ExtProcesstBlackListVars(entries,x);

}//ProcesstBlackListVars()


void ProcesstBlackListListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uBlackList);
                        guMode=2002;
                        tBlackList("");
                }
        }
}//void ProcesstBlackListListVars(pentry entries[], int x)


int tBlackListCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttBlackListCommands(entries,x);

	if(!strcmp(gcFunction,"tBlackListTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tBlackListList();
		}

		//Default
		ProcesstBlackListVars(entries,x);
		tBlackList("");
	}
	else if(!strcmp(gcFunction,"tBlackListList"))
	{
		ProcessControlVars(entries,x);
		ProcesstBlackListListVars(entries,x);
		tBlackListList();
	}

	return(0);

}//tBlackListCommands()


void tBlackList(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttBlackListSelectRow();
		else
			ExttBlackListSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetBlackList();
				unxsMail("New tBlackList table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tBlackList WHERE uBlackList=%u"
						,uBlackList);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uBlackList);
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

	Header_ism3(":: tBlackList",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tBlackListTools>");
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

        ExttBlackListButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tBlackList Record Data",100);

	if(guMode==2000 || guMode==2002)
		tBlackListInput(1);
	else
		tBlackListInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttBlackListAuxTable();

	Footer_ism3();

}//end of tBlackList();


void tBlackListInput(unsigned uMode)
{

//uBlackList
	OpenRow(LANG_FL_tBlackList_uBlackList,"black");
	printf("<input title='%s' type=text name=uBlackList value=%u size=16 maxlength=10 "
,LANG_FT_tBlackList_uBlackList,uBlackList);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uBlackList value=%u >\n",uBlackList);
	}
//cLabel
	OpenRow(LANG_FL_tBlackList_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tBlackList_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tBlackList_uUser,"black");
	printf("<input title='%s' type=text name=uUser value=\"%u\" size=40 maxlength=32 "
,LANG_FT_tBlackList_uUser,uUser);
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
	OpenRow(LANG_FL_tBlackList_uServerGroup,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,1);
	else
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,0);
//uOwner
	OpenRow(LANG_FL_tBlackList_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tBlackList_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tBlackList_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tBlackList_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tBlackList_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tBlackListInput(unsigned uMode)


void NewtBlackList(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uBlackList FROM tBlackList\
				WHERE uBlackList=%u"
							,uBlackList);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tBlackList("<blink>Record already exists");
		tBlackList(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tBlackList();
	//sprintf(gcQuery,"New record %u added");
	uBlackList=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tBlackList",uBlackList);
	unxsMailLog(uBlackList,"tBlackList","New");

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uBlackList);
	tBlackList(gcQuery);
	}

}//NewtBlackList(unsigned uMode)


void DeletetBlackList(void)
{
	sprintf(gcQuery,"DELETE FROM tBlackList WHERE uBlackList=%u AND ( uOwner=%u OR %u>9 )"
					,uBlackList,guLoginClient,guPermLevel);
	macro_mySQLQueryHTMLError;
	//tBlackList("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsMailLog(uBlackList,"tBlackList","Del");
		tBlackList(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsMailLog(uBlackList,"tBlackList","DelError");
		tBlackList(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetBlackList(void)


void Insert_tBlackList(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tBlackList SET uBlackList=%u,cLabel='%s',uUser=%u,uServerGroup=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uBlackList
			,TextAreaSave(cLabel)
			,uUser
			,uServerGroup
			,uOwner
			,uCreatedBy
			);

	macro_mySQLQueryHTMLError;

}//void Insert_tBlackList(void)


void Update_tBlackList(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tBlackList SET uBlackList=%u,cLabel='%s',uUser=%u,uServerGroup=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uBlackList
			,TextAreaSave(cLabel)
			,uUser
			,uServerGroup
			,uModBy
			,cRowid);

	macro_mySQLQueryHTMLError;

}//void Update_tBlackList(void)


void ModtBlackList(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uBlackList,uModDate FROM tBlackList WHERE uBlackList=%u",uBlackList);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tBlackList("<blink>Record does not exist");
	if(i<1) tBlackList(LANG_NBR_RECNOTEXIST);
	//if(i>1) tBlackList("<blink>Multiple rows!");
	if(i>1) tBlackList(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tBlackList(LANG_NBR_EXTMOD);

	Update_tBlackList(field[0]);
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tBlackList",uBlackList);
	unxsMailLog(uBlackList,"tBlackList","Mod");
	tBlackList(gcQuery);

}//ModtBlackList(void)


void tBlackListList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttBlackListListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tBlackListList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttBlackListListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uBlackList<td><font face=arial,helvetica color=white>cLabel<font face=arial,helvetica color=white>uUser<td><font face=arial,helvetica color=white>uServerGroup<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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

}//tBlackListList()


void CreatetBlackList(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tBlackList ( uBlackList INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uServerGroup INT UNSIGNED NOT NULL DEFAULT 0, uUser INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetBlackList()

