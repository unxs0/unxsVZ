/*
FILE
	tUser source code of unxsMail.cgi
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
//cLogin: Pop3 user login
static char cLogin[33]={""};
//uDomain: Domain if applicable
static unsigned uDomain=0;
static char cuDomainPullDown[256]={""};
//cPasswd: User password MD5 encrypted
static char cPasswd[65]={""};
//uUserType: General user type
static unsigned uUserType=0;
static char cuUserTypePullDown[256]={""};
//uServerGroup: Server account is on
static unsigned uServerGroup=0;
static char cuServerGroupPullDown[256]={""};
//uStatus: Status of user
static unsigned uStatus=0;
static char cuStatusPullDown[256]={""};
//cSecretQuestion: Security question for password update
static char cSecretQuestion[256]={""};
//cSecretAnswer: Security question answer for password update
static char cSecretAnswer[256]={""};
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



#define VAR_LIST_tUser "tUser.uUser,tUser.cLogin,tUser.uDomain,tUser.cPasswd,tUser.uUserType,tUser.uServerGroup,tUser.uStatus,tUser.cSecretQuestion,tUser.cSecretAnswer,tUser.uClient,tUser.uOwner,tUser.uCreatedBy,tUser.uCreatedDate,tUser.uModBy,tUser.uModDate"

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
			sprintf(cLogin,"%.32s",WordToLower(entries[i].val));
		else if(!strcmp(entries[i].name,"uDomain"))
			sscanf(entries[i].val,"%u",&uDomain);
		else if(!strcmp(entries[i].name,"cuDomainPullDown"))
		{
			sprintf(cuDomainPullDown,"%.255s",entries[i].val);
			uDomain=ReadPullDown("tDomain","cDomain",cuDomainPullDown);
		}
		else if(!strcmp(entries[i].name,"cPasswd"))
			sprintf(cPasswd,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uUserType"))
			sscanf(entries[i].val,"%u",&uUserType);
		else if(!strcmp(entries[i].name,"cuUserTypePullDown"))
		{
			sprintf(cuUserTypePullDown,"%.255s",entries[i].val);
			uUserType=ReadPullDown("tUserType","cLabel",cuUserTypePullDown);
		}
		else if(!strcmp(entries[i].name,"uServerGroup"))
			sscanf(entries[i].val,"%u",&uServerGroup);
		else if(!strcmp(entries[i].name,"cuServerGroupPullDown"))
		{
			sprintf(cuServerGroupPullDown,"%.255s",entries[i].val);
			uServerGroup=ReadPullDown("tServerGroup","cLabel",cuServerGroupPullDown);
		}
		else if(!strcmp(entries[i].name,"uStatus"))
			sscanf(entries[i].val,"%u",&uStatus);
		else if(!strcmp(entries[i].name,"cuStatusPullDown"))
		{
			sprintf(cuStatusPullDown,"%.255s",entries[i].val);
			uStatus=ReadPullDown("tStatus","cLabel",cuStatusPullDown);
		}
		else if(!strcmp(entries[i].name,"cSecretQuestion"))
			sprintf(cSecretQuestion,"%.255s",entries[i].val);
		else if(!strcmp(entries[i].name,"cSecretAnswer"))
			sprintf(cSecretAnswer,"%.255s",entries[i].val);
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
				unxsMail("New tUser table created");
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
		sprintf(cLogin,"%.32s",field[1]);
		sscanf(field[2],"%u",&uDomain);
		sprintf(cPasswd,"%.64s",field[3]);
		sscanf(field[4],"%u",&uUserType);
		sscanf(field[5],"%u",&uServerGroup);
		sscanf(field[6],"%u",&uStatus);
		sprintf(cSecretQuestion,"%.255s",field[7]);
		sprintf(cSecretAnswer,"%.255s",field[8]);
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
	printf("<input title='%s' type=text name=uUser value=%u size=16 maxlength=10 "
,LANG_FT_tUser_uUser,uUser);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uUser value=%u >\n",uUser);
	}
//cLogin
	OpenRow(LANG_FL_tUser_cLogin,"black");
	printf("<input title='%s' type=text name=cLogin value=\"%s\" size=40 maxlength=32 "
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
//uDomain
	OpenRow(LANG_FL_tUser_uDomain,"black");
	if(guPermLevel>=8 && uMode)
		tTablePullDown("tDomain;cuDomainPullDown","cDomain","cDomain",uDomain,1);
	else
		tTablePullDown("tDomain;cuDomainPullDown","cDomain","cDomain",uDomain,0);
//cPasswd
	OpenRow(LANG_FL_tUser_cPasswd,"black");
	printf("<input title='%s' type=text name=cPasswd value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tUser_cPasswd,EncodeDoubleQuotes(cPasswd));
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cPasswd value=\"%s\">\n",EncodeDoubleQuotes(cPasswd));
	}
//uUserType
	OpenRow(LANG_FL_tUser_uUserType,"black");
	if(guPermLevel>=8 && uMode)
		tTablePullDown("tUserType;cuUserTypePullDown","cLabel","cLabel",uUserType,1);
	else
		tTablePullDown("tUserType;cuUserTypePullDown","cLabel","cLabel",uUserType,0);
//uServerGroup
	OpenRow(LANG_FL_tUser_uServerGroup,"black");
	if(guPermLevel>=8 && uMode)
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,1);
	else
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,0);
//uStatus
	OpenRow(LANG_FL_tUser_uStatus,"black");
	if(guPermLevel>=20 && uMode)
		tTablePullDown("tStatus;cuStatusPullDown","cLabel","cLabel",uStatus,1);
	else
		tTablePullDown("tStatus;cuStatusPullDown","cLabel","cLabel",uStatus,0);
//cSecretQuestion
	OpenRow(LANG_FL_tUser_cSecretQuestion,"black");
	printf("<input title='%s' type=text name=cSecretQuestion value=\"%s\" size=40 maxlength=255 "
,LANG_FT_tUser_cSecretQuestion,EncodeDoubleQuotes(cSecretQuestion));
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cSecretQuestion value=\"%s\">\n",EncodeDoubleQuotes(cSecretQuestion));
	}
//cSecretAnswer
	OpenRow(LANG_FL_tUser_cSecretAnswer,"black");
	printf("<input title='%s' type=text name=cSecretAnswer value=\"%s\" size=40 maxlength=255 "
,LANG_FT_tUser_cSecretAnswer,EncodeDoubleQuotes(cSecretAnswer));
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cSecretAnswer value=\"%s\">\n",EncodeDoubleQuotes(cSecretAnswer));
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

	sprintf(gcQuery,"SELECT uUser FROM tUser\
				WHERE uUser=%u"
							,uUser);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
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
	unxsMailLog(uUser,"tUser","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uUser);
	tUser(gcQuery);
	}

}//NewtUser(unsigned uMode)


void DeletetUser(void)
{

	sprintf(gcQuery,"DELETE FROM tUser WHERE uUser=%u AND ( uOwner=%u OR %u>9 )"
					,uUser,guLoginClient,guPermLevel);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tUser("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uUser,"tUser","Del");

		tUser(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uUser,"tUser","DelError");

		tUser(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetUser(void)


void Insert_tUser(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tUser SET uUser=%u,cLogin='%s',uDomain=%u,cPasswd='%s',uUserType=%u,uServerGroup=%u,uStatus=%u,cSecretQuestion='%s',cSecretAnswer='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uUser
			,TextAreaSave(cLogin)
			,uDomain
			,TextAreaSave(cPasswd)
			,uUserType
			,uServerGroup
			,uStatus
			,TextAreaSave(cSecretQuestion)
			,TextAreaSave(cSecretAnswer)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tUser(void)


void Update_tUser(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tUser SET uUser=%u,cLogin='%s',uDomain=%u,cPasswd='%s',uUserType=%u,uServerGroup=%u,uStatus=%u,cSecretQuestion='%s',cSecretAnswer='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uUser
			,TextAreaSave(cLogin)
			,uDomain
			,TextAreaSave(cPasswd)
			,uUserType
			,uServerGroup
			,uStatus
			,TextAreaSave(cSecretQuestion)
			,TextAreaSave(cSecretAnswer)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tUser(void)


void ModtUser(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPreModDate=0;

	sprintf(gcQuery,"SELECT uUser,uModDate FROM tUser WHERE uUser=%u"
						,uUser);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
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
	unxsMailLog(uUser,"tUser","Mod");

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
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uUser<td><font face=arial,helvetica color=white>cLogin<td><font face=arial,helvetica color=white>uDomain<td><font face=arial,helvetica color=white>cPasswd<td><font face=arial,helvetica color=white>uUserType<td><font face=arial,helvetica color=white>uServerGroup<td><font face=arial,helvetica color=white>uStatus<td><font face=arial,helvetica color=white>cSecretQuestion<td><font face=arial,helvetica color=white>cSecretAnswer<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime12=strtoul(field[12],NULL,10);
		char cBuf12[32];
		if(luTime12)
			ctime_r(&luTime12,cBuf12);
		else
			sprintf(cBuf12,"---");
		time_t luTime14=strtoul(field[14],NULL,10);
		char cBuf14[32];
		if(luTime14)
			ctime_r(&luTime14,cBuf14);
		else
			sprintf(cBuf14,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey("tDomain","cDomain",strtoul(field[2],NULL,10))
			,field[3]
			,ForeignKey("tUserType","cLabel",strtoul(field[4],NULL,10))
			,ForeignKey("tServerGroup","cLabel",strtoul(field[5],NULL,10))
			,ForeignKey("tStatus","cLabel",strtoul(field[6],NULL,10))
			,field[7]
			,field[8]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[10],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[11],NULL,10))
			,cBuf12
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[13],NULL,10))
			,cBuf14
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tUserList()


void CreatetUser(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tUser ( uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), cLogin VARCHAR(32) NOT NULL DEFAULT '',index (cLogin,uServerGroup,uDomain), uUser INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cPasswd VARCHAR(64) NOT NULL DEFAULT '', uServerGroup INT UNSIGNED NOT NULL DEFAULT 0,index (uServerGroup), uStatus INT UNSIGNED NOT NULL DEFAULT 0, uDomain INT UNSIGNED NOT NULL DEFAULT 0,index (uDomain), uUserType INT UNSIGNED NOT NULL DEFAULT 0,index (uUserType), cSecretQuestion VARCHAR(255) NOT NULL DEFAULT '', cSecretAnswer VARCHAR(255) NOT NULL DEFAULT '', uClient INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetUser()

