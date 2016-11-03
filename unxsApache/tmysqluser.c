/*
FILE
	tMySQLUser source code of unxsApache.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2009 for Unixservice
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tmysqluserfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uMySQLUser: Primary Key
static unsigned uMySQLUser=0;
//uMySQL: FK into tMySQL
static unsigned uMySQL=0;
static char cuMySQLPullDown[256]={""};
//cLogin: Login for MySQL db
static char cLogin[65]={""};
//cPasswd: Password for MySQL db
static char cPasswd[65]={""};
//cHost: Allowed host to connect to the MySQL db
static char cHost[65]={""};
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



#define VAR_LIST_tMySQLUser "tMySQLUser.uMySQLUser,tMySQLUser.uMySQL,tMySQLUser.cLogin,tMySQLUser.cPasswd,tMySQLUser.cHost,tMySQLUser.uOwner,tMySQLUser.uCreatedBy,tMySQLUser.uCreatedDate,tMySQLUser.uModBy,tMySQLUser.uModDate"

 //Local only
void Insert_tMySQLUser(void);
void Update_tMySQLUser(char *cRowid);
void ProcesstMySQLUserListVars(pentry entries[], int x);

 //In tMySQLUserfunc.h file included below
void ExtProcesstMySQLUserVars(pentry entries[], int x);
void ExttMySQLUserCommands(pentry entries[], int x);
void ExttMySQLUserButtons(void);
void ExttMySQLUserNavBar(void);
void ExttMySQLUserGetHook(entry gentries[], int x);
void ExttMySQLUserSelect(void);
void ExttMySQLUserSelectRow(void);
void ExttMySQLUserListSelect(void);
void ExttMySQLUserListFilter(void);
void ExttMySQLUserAuxTable(void);

#include "tmysqluserfunc.h"

 //Table Variables Assignment Function
void ProcesstMySQLUserVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uMySQLUser"))
			sscanf(entries[i].val,"%u",&uMySQLUser);
		else if(!strcmp(entries[i].name,"uMySQL"))
			sscanf(entries[i].val,"%u",&uMySQL);
		else if(!strcmp(entries[i].name,"cuMySQLPullDown"))
		{
			sprintf(cuMySQLPullDown,"%.255s",entries[i].val);
			uMySQL=ReadPullDown("tMySQL","cLabel",cuMySQLPullDown);
		}
		else if(!strcmp(entries[i].name,"cLogin"))
			sprintf(cLogin,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cPasswd"))
			sprintf(cPasswd,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cHost"))
			sprintf(cHost,"%.64s",entries[i].val);
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
	ExtProcesstMySQLUserVars(entries,x);

}//ProcesstMySQLUserVars()


void ProcesstMySQLUserListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uMySQLUser);
                        guMode=2002;
                        tMySQLUser("");
                }
        }
}//void ProcesstMySQLUserListVars(pentry entries[], int x)


int tMySQLUserCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttMySQLUserCommands(entries,x);

	if(!strcmp(gcFunction,"tMySQLUserTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tMySQLUserList();
		}

		//Default
		ProcesstMySQLUserVars(entries,x);
		tMySQLUser("");
	}
	else if(!strcmp(gcFunction,"tMySQLUserList"))
	{
		ProcessControlVars(entries,x);
		ProcesstMySQLUserListVars(entries,x);
		tMySQLUserList();
	}

	return(0);

}//tMySQLUserCommands()


void tMySQLUser(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttMySQLUserSelectRow();
		else
			ExttMySQLUserSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetMySQLUser();
				unxsApache("New tMySQLUser table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tMySQLUser WHERE uMySQLUser=%u"
						,uMySQLUser);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uMySQLUser);
		sscanf(field[1],"%u",&uMySQL);
		sprintf(cLogin,"%.64s",field[2]);
		sprintf(cPasswd,"%.64s",field[3]);
		sprintf(cHost,"%.64s",field[4]);
		sscanf(field[5],"%u",&uOwner);
		sscanf(field[6],"%u",&uCreatedBy);
		sscanf(field[7],"%lu",&uCreatedDate);
		sscanf(field[8],"%u",&uModBy);
		sscanf(field[9],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tMySQLUser",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tMySQLUserTools>");
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

        ExttMySQLUserButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tMySQLUser Record Data",100);

	if(guMode==2000 || guMode==2002)
		tMySQLUserInput(1);
	else
		tMySQLUserInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttMySQLUserAuxTable();

	Footer_ism3();

}//end of tMySQLUser();


void tMySQLUserInput(unsigned uMode)
{

//uMySQLUser
	OpenRow(LANG_FL_tMySQLUser_uMySQLUser,"black");
	printf("<input title='%s' type=text name=uMySQLUser value=%u size=16 maxlength=10 "
,LANG_FT_tMySQLUser_uMySQLUser,uMySQLUser);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uMySQLUser value=%u >\n",uMySQLUser);
	}
//uMySQL
	OpenRow(LANG_FL_tMySQLUser_uMySQL,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDown("tMySQL;cuMySQLPullDown","cLabel","cLabel",uMySQL,1);
	else
		tTablePullDown("tMySQL;cuMySQLPullDown","cLabel","cLabel",uMySQL,0);
//cLogin
	OpenRow(LANG_FL_tMySQLUser_cLogin,"black");
	printf("<input title='%s' type=text name=cLogin value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tMySQLUser_cLogin,EncodeDoubleQuotes(cLogin));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLogin value=\"%s\">\n",EncodeDoubleQuotes(cLogin));
	}
//cPasswd
	OpenRow(LANG_FL_tMySQLUser_cPasswd,"black");
	printf("<input title='%s' type=text name=cPasswd value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tMySQLUser_cPasswd,EncodeDoubleQuotes(cPasswd));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cPasswd value=\"%s\">\n",EncodeDoubleQuotes(cPasswd));
	}
//cHost
	OpenRow(LANG_FL_tMySQLUser_cHost,"black");
	printf("<input title='%s' type=text name=cHost value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tMySQLUser_cHost,EncodeDoubleQuotes(cHost));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cHost value=\"%s\">\n",EncodeDoubleQuotes(cHost));
	}
//uOwner
	OpenRow(LANG_FL_tMySQLUser_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tMySQLUser_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tMySQLUser_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tMySQLUser_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tMySQLUser_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tMySQLUserInput(unsigned uMode)


void NewtMySQLUser(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uMySQLUser FROM tMySQLUser\
				WHERE uMySQLUser=%u"
							,uMySQLUser);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tMySQLUser("<blink>Record already exists");
		tMySQLUser(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tMySQLUser();
	//sprintf(gcQuery,"New record %u added");
	uMySQLUser=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tMySQLUser",uMySQLUser);
	unxsApacheLog(uMySQLUser,"tMySQLUser","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uMySQLUser);
	tMySQLUser(gcQuery);
	}

}//NewtMySQLUser(unsigned uMode)


void DeletetMySQLUser(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tMySQLUser WHERE uMySQLUser=%u AND ( uOwner=%u OR %u>9 )"
					,uMySQLUser,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tMySQLUser WHERE uMySQLUser=%u"
					,uMySQLUser);
#endif
	macro_mySQLQueryHTMLError;
	//tMySQLUser("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsApacheLog(uMySQLUser,"tMySQLUser","Del");
#endif
		tMySQLUser(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsApacheLog(uMySQLUser,"tMySQLUser","DelError");
#endif
		tMySQLUser(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetMySQLUser(void)


void Insert_tMySQLUser(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tMySQLUser SET uMySQLUser=%u,uMySQL=%u,cLogin='%s',cPasswd='%s',cHost='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uMySQLUser
			,uMySQL
			,TextAreaSave(cLogin)
			,TextAreaSave(cPasswd)
			,TextAreaSave(cHost)
			,uOwner
			,uCreatedBy
			);

	macro_mySQLQueryHTMLError;

}//void Insert_tMySQLUser(void)


void Update_tMySQLUser(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tMySQLUser SET uMySQLUser=%u,uMySQL=%u,cLogin='%s',cPasswd='%s',cHost='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uMySQLUser
			,uMySQL
			,TextAreaSave(cLogin)
			,TextAreaSave(cPasswd)
			,TextAreaSave(cHost)
			,uModBy
			,cRowid);

	macro_mySQLQueryHTMLError;

}//void Update_tMySQLUser(void)


void ModtMySQLUser(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uMySQLUser,uModDate FROM tMySQLUser\
				WHERE uMySQLUser=%u"
						,uMySQLUser);

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tMySQLUser("<blink>Record does not exist");
	if(i<1) tMySQLUser(LANG_NBR_RECNOTEXIST);
	//if(i>1) tMySQLUser("<blink>Multiple rows!");
	if(i>1) tMySQLUser(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tMySQLUser(LANG_NBR_EXTMOD);
#endif

	Update_tMySQLUser(field[0]);
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tMySQLUser",uMySQLUser);
	unxsApacheLog(uMySQLUser,"tMySQLUser","Mod");
#endif
	tMySQLUser(gcQuery);

}//ModtMySQLUser(void)


void tMySQLUserList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttMySQLUserListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tMySQLUserList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttMySQLUserListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uMySQLUser<td><font face=arial,helvetica color=white>uMySQL<td><font face=arial,helvetica color=white>cLogin<td><font face=arial,helvetica color=white>cPasswd<td><font face=arial,helvetica color=white>cHost<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime7=strtoul(field[7],NULL,10);
		char cBuf7[32];
		if(luTime7)
			ctime_r(&luTime7,cBuf7);
		else
			sprintf(cBuf7,"---");
		time_t luTime9=strtoul(field[9],NULL,10);
		char cBuf9[32];
		if(luTime9)
			ctime_r(&luTime9,cBuf9);
		else
			sprintf(cBuf9,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,ForeignKey("tMySQL","cLabel",strtoul(field[1],NULL,10))
			,field[2]
			,field[3]
			,field[4]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,cBuf7
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[8],NULL,10))
			,cBuf9
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tMySQLUserList()


void CreatetMySQLUser(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tMySQLUser ( uMySQLUser INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uMySQL INT UNSIGNED NOT NULL DEFAULT 0, uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cLogin VARCHAR(64) NOT NULL DEFAULT '', cPasswd VARCHAR(64) NOT NULL DEFAULT '', cHost VARCHAR(64) NOT NULL DEFAULT '' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetMySQLUser()

