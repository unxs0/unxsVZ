/*
FILE
	tUser source code of unxsRadius.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tuserfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uUser: Primary Key
static unsigned uUser=0;
//cLogin: Radius user login
static char cLogin[256]={""};
//uOnHold: Users OnHold are not in raddb/users file
static unsigned uOnHold=0;
static char cYesNouOnHold[32]={""};
//cEnterPasswd: Clear text passwd input field
static char cEnterPasswd[65]={""};
//uClearText: Clear text passwd for CHAP use
static unsigned uClearText=0;
static char cYesNouClearText[32]={""};
//cPasswd: Encrypted passwd
static char cPasswd[65]={""};
//uProfileName: Personal profile if set
static unsigned uProfileName=0;
static char cuProfileNamePullDown[256]={""};
//uSimulUse: Simultaneous use
static unsigned uSimulUse=1;
//cIP: Static IP if matches profile
static char cIP[19]={""};
//Resource owner, i.e. end user uClient that gas the resource deployed for
static unsigned uClient=0;
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



#define VAR_LIST_tUser "tUser.uUser,tUser.cLogin,tUser.uOnHold,tUser.cEnterPasswd,tUser.uClearText,tUser.cPasswd,tUser.uProfileName,tUser.uSimulUse,tUser.cIP,tUser.uClient,tUser.uOwner,tUser.uCreatedBy,tUser.uCreatedDate,tUser.uModBy,tUser.uModDate"

 //Local only
void Insert_tUser(void);
void Update_tUser(char *cRowid);
void ProcesstUserListVars(pentry entries[], int x);

 //In tUserfunc.h file included below
void ExtProcesstUserVars(pentry entries[], int x);
void ExttUserCommands(pentry entries[], int x);
void ExttUserButtons(void);
void ExttUserNavBar(void);
void ExttUserGetHook(entry gentries[], int x);
void ExttUserSelect(void);
void ExttUserSelectRow(void);
void ExttUserListSelect(void);
void ExttUserListFilter(void);
void ExttUserAuxTable(void);

#include "tuserfunc.h"

 //Table Variables Assignment Function
void ProcesstUserVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uUser"))
			sscanf(entries[i].val,"%u",&uUser);
		else if(!strcmp(entries[i].name,"cLogin"))
			sprintf(cLogin,"%.255s",WordToLower(entries[i].val));
		else if(!strcmp(entries[i].name,"uOnHold"))
			sscanf(entries[i].val,"%u",&uOnHold);
		else if(!strcmp(entries[i].name,"cYesNouOnHold"))
		{
			sprintf(cYesNouOnHold,"%.31s",entries[i].val);
			uOnHold=ReadYesNoPullDown(cYesNouOnHold);
		}
		else if(!strcmp(entries[i].name,"cEnterPasswd"))
			sprintf(cEnterPasswd,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uClearText"))
			sscanf(entries[i].val,"%u",&uClearText);
		else if(!strcmp(entries[i].name,"cYesNouClearText"))
		{
			sprintf(cYesNouClearText,"%.31s",entries[i].val);
			uClearText=ReadYesNoPullDown(cYesNouClearText);
		}
		else if(!strcmp(entries[i].name,"cPasswd"))
			sprintf(cPasswd,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uProfileName"))
			sscanf(entries[i].val,"%u",&uProfileName);
		else if(!strcmp(entries[i].name,"cuProfileNamePullDown"))
		{
			sprintf(cuProfileNamePullDown,"%.255s",entries[i].val);
			uProfileName=ReadPullDown("tProfileName","cLabel",cuProfileNamePullDown);
		}
		else if(!strcmp(entries[i].name,"uSimulUse"))
			sscanf(entries[i].val,"%u",&uSimulUse);
		else if(!strcmp(entries[i].name,"cIP"))
			//sprintf(cIP,"%.18s",IPNumber(entries[i].val));
			//Allow MAC also
			sprintf(cIP,"%.18s",entries[i].val);
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
		else if(!strcmp(entries[i].name,"uClient"))
			sscanf(entries[i].val,"%u",&uClient);
	}

	//After so we can overwrite form data if needed.
	ExtProcesstUserVars(entries,x);

}//ProcesstUserVars()


void ProcesstUserListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uUser);
                        guMode=2002;
                        tUser("");
                }
        }
}//void ProcesstUserListVars(pentry entries[], int x)


int tUserCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttUserCommands(entries,x);

	if(!strcmp(gcFunction,"tUserTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tUserList();
		}

		//Default
		ProcesstUserVars(entries,x);
		tUser("");
	}
	else if(!strcmp(gcFunction,"tUserList"))
	{
		ProcessControlVars(entries,x);
		ProcesstUserListVars(entries,x);
		tUserList();
	}

	return(0);

}//tUserCommands()


void tUser(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttUserSelectRow();
		else
			ExttUserSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetUser();
				unxsRadius("New tUser table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tUser WHERE uUser=%u"
						,uUser);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uUser);
		sprintf(cLogin,"%.255s",field[1]);
		sscanf(field[2],"%u",&uOnHold);
		sprintf(cEnterPasswd,"%.64s",field[3]);
		sscanf(field[4],"%u",&uClearText);
		sprintf(cPasswd,"%.64s",field[5]);
		sscanf(field[6],"%u",&uProfileName);
		sscanf(field[7],"%u",&uSimulUse);
		sprintf(cIP,"%.18s",field[8]);
		sscanf(field[9],"%u",&uClient);
		sscanf(field[10],"%u",&uOwner);
		sscanf(field[11],"%u",&uCreatedBy);
		sscanf(field[12],"%lu",&uCreatedDate);
		sscanf(field[13],"%u",&uModBy);
		sscanf(field[14],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tUser",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tUserTools>");
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

        ExttUserButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tUser Record Data",100);

	if(guMode==2000 || guMode==2002)
		tUserInput(1);
	else
		tUserInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttUserAuxTable();

	Footer_ism3();

}//end of tUser();


void tUserInput(unsigned uMode)
{

//uUser
	OpenRow(LANG_FL_tUser_uUser,"black");
	printf("%u",uUser);
	printf("</td></tr>\n");
	printf("<input type=hidden name=uUser value=%u >\n",uUser);
//cLogin
	OpenRow(LANG_FL_tUser_cLogin,"black");
	printf("<input title='%s' type=text name=cLogin value=\"%s\" size=40 maxlength=255 "
,LANG_FT_tUser_cLogin,EncodeDoubleQuotes(cLogin));
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLogin value=\"%s\">\n",EncodeDoubleQuotes(cLogin));
	}
//uOnHold
	OpenRow(LANG_FL_tUser_uOnHold,"black");
	if(guPermLevel>=8 && uMode)
		YesNoPullDown("uOnHold",uOnHold,1);
	else
		YesNoPullDown("uOnHold",uOnHold,0);
//cEnterPasswd
	OpenRow(LANG_FL_tUser_cEnterPasswd,"black");
	printf("<input title='%s' type=text name=cEnterPasswd value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tUser_cEnterPasswd,EncodeDoubleQuotes(cEnterPasswd));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cEnterPasswd value=\"%s\">\n",EncodeDoubleQuotes(cEnterPasswd));
	}
//uClearText
	OpenRow(LANG_FL_tUser_uClearText,"black");
	if(guPermLevel>=8 && uMode)
		YesNoPullDown("uClearText",uClearText,1);
	else
		YesNoPullDown("uClearText",uClearText,0);
//cPasswd
	OpenRow(LANG_FL_tUser_cPasswd,"black");
	printf("<input title='%s' type=text name=cPasswd value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tUser_cPasswd,EncodeDoubleQuotes(cPasswd));
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cPasswd value=\"%s\">\n",EncodeDoubleQuotes(cPasswd));
	}
//uProfileName
	OpenRow(LANG_FL_tUser_uProfileName,"black");
	if(guPermLevel>=8 && uMode)
		tTablePullDown("tProfileName;cuProfileNamePullDown","cLabel","cLabel",uProfileName,1);
	else
		tTablePullDown("tProfileName;cuProfileNamePullDown","cLabel","cLabel",uProfileName,0);
//uSimulUse
	OpenRow(LANG_FL_tUser_uSimulUse,"black");
	printf("<input title='%s' type=text name=uSimulUse value=%u size=16 maxlength=10 "
,LANG_FT_tUser_uSimulUse,uSimulUse);
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uSimulUse value=%u >\n",uSimulUse);
	}
//cIP
	OpenRow(LANG_FL_tUser_cIP,"black");
	printf("<input title='%s' type=text name=cIP value=\"%s\" size=40 maxlength=18 "
,LANG_FT_tUser_cIP,EncodeDoubleQuotes(cIP));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cIP value=\"%s\">\n",EncodeDoubleQuotes(cIP));
	}
//uClient
	OpenRow(LANG_FL_tUser_uClient,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
	else
	{
	printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
//uOwner
	OpenRow(LANG_FL_tUser_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tUser_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tUser_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tUser_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tUser_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tUserInput(unsigned uMode)


void NewtUser(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uUser FROM tUser WHERE uUser=%u",uUser);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tUser("<blink>Record already exists");
		tUser(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tUser();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uUser=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tUser",uUser);
	unxsRadiusLog(uUser,"tUser","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uUser);
		tUser(gcQuery);
	}

}//NewtUser(unsigned uMode)


void DeletetUser(void)
{
	sprintf(gcQuery,"DELETE FROM tUser WHERE uUser=%u",uUser);
	macro_mySQLQueryHTMLError;

	//tUser("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsRadiusLog(uUser,"tUser","Del");
		tUser(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsRadiusLog(uUser,"tUser","DelError");
		tUser(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetUser(void)


void Insert_tUser(void)
{
	sprintf(gcQuery,"INSERT INTO tUser SET uUser=%u,cLogin='%s',uOnHold=%u,cEnterPasswd='%s',"
			"uClearText=%u,cPasswd='%s',uProfileName=%u,uSimulUse=%u,cIP='%s',uOwner=%u,"
			"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uUser
				,TextAreaSave(cLogin)
				,uOnHold
				,TextAreaSave(cEnterPasswd)
				,uClearText
				,TextAreaSave(cPasswd)
				,uProfileName
				,uSimulUse
				,TextAreaSave(cIP)
				,uOwner
				,uCreatedBy);
	macro_mySQLQueryHTMLError;

}//void Insert_tUser(void)


void Update_tUser(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tUser SET uUser=%u,cLogin='%s',uOnHold=%u,cEnterPasswd='%s',uClearText=%u,"
			"cPasswd='%s',uProfileName=%u,uSimulUse=%u,cIP='%s',uModBy=%u,"
			"uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
				uUser
				,TextAreaSave(cLogin)
				,uOnHold
				,TextAreaSave(cEnterPasswd)
				,uClearText
				,TextAreaSave(cPasswd)
				,uProfileName
				,uSimulUse
				,TextAreaSave(cIP)
				,uModBy
				,cRowid);
	macro_mySQLQueryHTMLError;

}//void Update_tUser(void)


void ModtUser(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uUser,uModDate FROM tUser WHERE uUser=%u",uUser);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tUser("<blink>Record does not exist");
	if(i<1) tUser(LANG_NBR_RECNOTEXIST);
	//if(i>1) tUser("<blink>Multiple rows!");
	if(i>1) tUser(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tUser(LANG_NBR_EXTMOD);

	Update_tUser(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tUser",uUser);
	unxsRadiusLog(uUser,"tUser","Mod");
	tUser(gcQuery);

}//ModtUser(void)


void tUserList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttUserListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tUserList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttUserListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uUser<td><font face=arial,helvetica color=white>cLogin<td><font face=arial,helvetica color=white>uOnHold<td><font face=arial,helvetica color=white>cEnterPasswd<td><font face=arial,helvetica color=white>uClearText<td><font face=arial,helvetica color=white>cPasswd<td><font face=arial,helvetica color=white>uProfileName<td><font face=arial,helvetica color=white>uSimulUse<td><font face=arial,helvetica color=white>cIP<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luYesNo2=strtoul(field[2],NULL,10);
		char cBuf2[4];
		if(luYesNo2)
			sprintf(cBuf2,"Yes");
		else
			sprintf(cBuf2,"No");
		time_t luYesNo4=strtoul(field[4],NULL,10);
		char cBuf4[4];
		if(luYesNo4)
			sprintf(cBuf4,"Yes");
		else
			sprintf(cBuf4,"No");
		time_t luTime11=strtoul(field[11],NULL,10);
		char cBuf11[32];
		if(luTime11)
			ctime_r(&luTime11,cBuf11);
		else
			sprintf(cBuf11,"---");
		time_t luTime13=strtoul(field[13],NULL,10);
		char cBuf13[32];
		if(luTime13)
			ctime_r(&luTime13,cBuf13);
		else
			sprintf(cBuf13,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,cBuf2
			,field[3]
			,cBuf4
			,field[5]
			,ForeignKey("tProfileName","cLabel",strtoul(field[6],NULL,10))
			,field[7]
			,field[8]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[9],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[10],NULL,10))
			,cBuf11
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[12],NULL,10))
			,cBuf13
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tUserList()


void CreatetUser(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tUser ( uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uUser INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLogin VARCHAR(255) NOT NULL DEFAULT '', UNIQUE (cLogin,uOwner), cPasswd VARCHAR(64) NOT NULL DEFAULT '', cEnterPasswd VARCHAR(64) NOT NULL DEFAULT '', uSimulUse INT UNSIGNED NOT NULL DEFAULT 0, uProfileName INT UNSIGNED NOT NULL DEFAULT 0, cIP VARCHAR(18) NOT NULL DEFAULT '', uClearText INT UNSIGNED NOT NULL DEFAULT 0, uOnHold INT UNSIGNED NOT NULL DEFAULT 0, uClient INT UNSIGNED NOT NULL DEFAULT 0 )");
	macro_mySQLQueryHTMLError;
}//CreatetUser()

