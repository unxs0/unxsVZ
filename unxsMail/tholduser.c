/*
FILE
	tHoldUser source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tholduserfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uHoldUser: Primary Key
static unsigned uHoldUser=0;
//cLogin: Pop3 user login
static char cLogin[33]={""};
//cPasswd: User password MD5 encrypted
static char cPasswd[65]={""};
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



#define VAR_LIST_tHoldUser "tHoldUser.uHoldUser,tHoldUser.cLogin,tHoldUser.cPasswd,tHoldUser.uOwner,tHoldUser.uCreatedBy,tHoldUser.uCreatedDate,tHoldUser.uModBy,tHoldUser.uModDate"

 //Local only
void Insert_tHoldUser(void);
void Update_tHoldUser(char *cRowid);
void ProcesstHoldUserListVars(pentry entries[], int x);

 //In tHoldUserfunc.h file included below
void ExtProcesstHoldUserVars(pentry entries[], int x);
void ExttHoldUserCommands(pentry entries[], int x);
void ExttHoldUserButtons(void);
void ExttHoldUserNavBar(void);
void ExttHoldUserGetHook(entry gentries[], int x);
void ExttHoldUserSelect(void);
void ExttHoldUserSelectRow(void);
void ExttHoldUserListSelect(void);
void ExttHoldUserListFilter(void);
void ExttHoldUserAuxTable(void);

#include "tholduserfunc.h"

 //Table Variables Assignment Function
void ProcesstHoldUserVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uHoldUser"))
			sscanf(entries[i].val,"%u",&uHoldUser);
		else if(!strcmp(entries[i].name,"cLogin"))
			sprintf(cLogin,"%.32s",WordToLower(entries[i].val));
		else if(!strcmp(entries[i].name,"cPasswd"))
			sprintf(cPasswd,"%.64s",entries[i].val);
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
	ExtProcesstHoldUserVars(entries,x);

}//ProcesstHoldUserVars()


void ProcesstHoldUserListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uHoldUser);
                        guMode=2002;
                        tHoldUser("");
                }
        }
}//void ProcesstHoldUserListVars(pentry entries[], int x)


int tHoldUserCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttHoldUserCommands(entries,x);

	if(!strcmp(gcFunction,"tHoldUserTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tHoldUserList();
		}

		//Default
		ProcesstHoldUserVars(entries,x);
		tHoldUser("");
	}
	else if(!strcmp(gcFunction,"tHoldUserList"))
	{
		ProcessControlVars(entries,x);
		ProcesstHoldUserListVars(entries,x);
		tHoldUserList();
	}

	return(0);

}//tHoldUserCommands()


void tHoldUser(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttHoldUserSelectRow();
		else
			ExttHoldUserSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetHoldUser();
				unxsMail("New tHoldUser table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tHoldUser WHERE uHoldUser=%u"
						,uHoldUser);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uHoldUser);
		sprintf(cLogin,"%.32s",field[1]);
		sprintf(cPasswd,"%.64s",field[2]);
		sscanf(field[3],"%u",&uOwner);
		sscanf(field[4],"%u",&uCreatedBy);
		sscanf(field[5],"%lu",&uCreatedDate);
		sscanf(field[6],"%u",&uModBy);
		sscanf(field[7],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tHoldUser",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tHoldUserTools>");
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

        ExttHoldUserButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tHoldUser Record Data",100);

	if(guMode==2000 || guMode==2002)
		tHoldUserInput(1);
	else
		tHoldUserInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttHoldUserAuxTable();

	Footer_ism3();

}//end of tHoldUser();


void tHoldUserInput(unsigned uMode)
{

//uHoldUser
	OpenRow(LANG_FL_tHoldUser_uHoldUser,"black");
	printf("<input title='%s' type=text name=uHoldUser value=%u size=16 maxlength=10 "
,LANG_FT_tHoldUser_uHoldUser,uHoldUser);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uHoldUser value=%u >\n",uHoldUser);
	}
//cLogin
	OpenRow(LANG_FL_tHoldUser_cLogin,"black");
	printf("<input title='%s' type=text name=cLogin value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tHoldUser_cLogin,EncodeDoubleQuotes(cLogin));
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLogin value=\"%s\">\n",EncodeDoubleQuotes(cLogin));
	}
//cPasswd
	OpenRow(LANG_FL_tHoldUser_cPasswd,"black");
	printf("<input title='%s' type=text name=cPasswd value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tHoldUser_cPasswd,EncodeDoubleQuotes(cPasswd));
	if(guPermLevel>=12 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cPasswd value=\"%s\">\n",EncodeDoubleQuotes(cPasswd));
	}
//uOwner
	OpenRow(LANG_FL_tHoldUser_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tHoldUser_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tHoldUser_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tHoldUser_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tHoldUser_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tHoldUserInput(unsigned uMode)


void NewtHoldUser(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uHoldUser FROM tHoldUser\
				WHERE uHoldUser=%u"
							,uHoldUser);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tHoldUser("<blink>Record already exists");
		tHoldUser(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tHoldUser();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uHoldUser=mysql_insert_id(&gMysql);

	uCreatedDate=luGetCreatedDate("tHoldUser",uHoldUser);
	unxsMailLog(uHoldUser,"tHoldUser","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uHoldUser);
	tHoldUser(gcQuery);
	}

}//NewtHoldUser(unsigned uMode)


void DeletetHoldUser(void)
{

	sprintf(gcQuery,"DELETE FROM tHoldUser WHERE uHoldUser=%u AND ( uOwner=%u OR %u>9 )"
					,uHoldUser,guLoginClient,guPermLevel);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tHoldUser("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uHoldUser,"tHoldUser","Del");

		tHoldUser(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uHoldUser,"tHoldUser","DelError");

		tHoldUser(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetHoldUser(void)


void Insert_tHoldUser(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tHoldUser SET uHoldUser=%u,cLogin='%s',cPasswd='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uHoldUser
			,TextAreaSave(cLogin)
			,TextAreaSave(cPasswd)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tHoldUser(void)


void Update_tHoldUser(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tHoldUser SET uHoldUser=%u,cLogin='%s',cPasswd='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uHoldUser
			,TextAreaSave(cLogin)
			,TextAreaSave(cPasswd)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tHoldUser(void)


void ModtHoldUser(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uHoldUser,uModDate FROM tHoldUser WHERE uHoldUser=%u"
						,uHoldUser);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tHoldUser("<blink>Record does not exist");
	if(i<1) tHoldUser(LANG_NBR_RECNOTEXIST);
	//if(i>1) tHoldUser("<blink>Multiple rows!");
	if(i>1) tHoldUser(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tHoldUser(LANG_NBR_EXTMOD);


	Update_tHoldUser(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);

	uModDate=luGetModDate("tHoldUser",uHoldUser);
	unxsMailLog(uHoldUser,"tHoldUser","Mod");

	tHoldUser(gcQuery);

}//ModtHoldUser(void)


void tHoldUserList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttHoldUserListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tHoldUserList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttHoldUserListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uHoldUser<td><font face=arial,helvetica color=white>cLogin<td><font face=arial,helvetica color=white>cPasswd<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[3],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[4],NULL,10))
			,cBuf5
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,cBuf7
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tHoldUserList()


void CreatetHoldUser(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tHoldUser ( cPasswd VARCHAR(64) NOT NULL DEFAULT '', uModDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uHoldUser INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLogin VARCHAR(32) NOT NULL DEFAULT '',index (cLogin), uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner) )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetHoldUser()

