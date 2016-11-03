/*
FILE
	tSiteUser source code of unxsApache.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tsiteuserfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uSiteUser: Primary Key
static unsigned uSiteUser=0;
//uSite: Website this login belongs to
static unsigned uSite=0;
//cLogin: ftp/ssh user login associated to a uVirtualHost
static char cLogin[33]={""};
//cPasswd: Encrypted password
static char cPasswd[65]={""};
//uAdmin: User is admin for site
static unsigned uAdmin=0;
static char cYesNouAdmin[32]={""};
//uSSH: Allow SSH access
static unsigned uSSH=0;
static char cYesNouSSH[32]={""};
//uFTP: Allow FTP access
static unsigned uFTP=0;
static char cYesNouFTP[32]={""};
//uPop3: Allow Pop3 access
static unsigned uPop3=0;
static char cYesNouPop3[32]={""};
//uSMTP: Allow SMTP access
static unsigned uSMTP=0;
static char cYesNouSMTP[32]={""};
//uStatus: Account status
static unsigned uStatus=0;
//Resource owner, i.e. end user uClient that gas the resource deployed for
static unsigned uClient=0;
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



#define VAR_LIST_tSiteUser "tSiteUser.uSiteUser,tSiteUser.uSite,tSiteUser.cLogin,tSiteUser.cPasswd,tSiteUser.uAdmin,tSiteUser.uSSH,tSiteUser.uFTP,tSiteUser.uPop3,tSiteUser.uSMTP,tSiteUser.uStatus,tSiteUser.uClient,tSiteUser.uOwner,tSiteUser.uCreatedBy,tSiteUser.uCreatedDate,tSiteUser.uModBy,tSiteUser.uModDate"

 //Local only
void Insert_tSiteUser(void);
void Update_tSiteUser(char *cRowid);
void ProcesstSiteUserListVars(pentry entries[], int x);

 //In tSiteUserfunc.h file included below
void ExtProcesstSiteUserVars(pentry entries[], int x);
void ExttSiteUserCommands(pentry entries[], int x);
void ExttSiteUserButtons(void);
void ExttSiteUserNavBar(void);
void ExttSiteUserGetHook(entry gentries[], int x);
void ExttSiteUserSelect(void);
void ExttSiteUserSelectRow(void);
void ExttSiteUserListSelect(void);
void ExttSiteUserListFilter(void);
void ExttSiteUserAuxTable(void);

#include "tsiteuserfunc.h"

 //Table Variables Assignment Function
void ProcesstSiteUserVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uSiteUser"))
			sscanf(entries[i].val,"%u",&uSiteUser);
		else if(!strcmp(entries[i].name,"uSite"))
			sscanf(entries[i].val,"%u",&uSite);
		else if(!strcmp(entries[i].name,"cLogin"))
			sprintf(cLogin,"%.32s",WordToLower(entries[i].val));
		else if(!strcmp(entries[i].name,"cPasswd"))
			sprintf(cPasswd,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uAdmin"))
			sscanf(entries[i].val,"%u",&uAdmin);
		else if(!strcmp(entries[i].name,"cYesNouAdmin"))
		{
			sprintf(cYesNouAdmin,"%.31s",entries[i].val);
			uAdmin=ReadYesNoPullDown(cYesNouAdmin);
		}
		else if(!strcmp(entries[i].name,"uSSH"))
			sscanf(entries[i].val,"%u",&uSSH);
		else if(!strcmp(entries[i].name,"cYesNouSSH"))
		{
			sprintf(cYesNouSSH,"%.31s",entries[i].val);
			uSSH=ReadYesNoPullDown(cYesNouSSH);
		}
		else if(!strcmp(entries[i].name,"uFTP"))
			sscanf(entries[i].val,"%u",&uFTP);
		else if(!strcmp(entries[i].name,"cYesNouFTP"))
		{
			sprintf(cYesNouFTP,"%.31s",entries[i].val);
			uFTP=ReadYesNoPullDown(cYesNouFTP);
		}
		else if(!strcmp(entries[i].name,"uPop3"))
			sscanf(entries[i].val,"%u",&uPop3);
		else if(!strcmp(entries[i].name,"cYesNouPop3"))
		{
			sprintf(cYesNouPop3,"%.31s",entries[i].val);
			uPop3=ReadYesNoPullDown(cYesNouPop3);
		}
		else if(!strcmp(entries[i].name,"uSMTP"))
			sscanf(entries[i].val,"%u",&uSMTP);
		else if(!strcmp(entries[i].name,"cYesNouSMTP"))
		{
			sprintf(cYesNouSMTP,"%.31s",entries[i].val);
			uSMTP=ReadYesNoPullDown(cYesNouSMTP);
		}
		else if(!strcmp(entries[i].name,"uStatus"))
			sscanf(entries[i].val,"%u",&uStatus);
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
	ExtProcesstSiteUserVars(entries,x);

}//ProcesstSiteUserVars()


void ProcesstSiteUserListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uSiteUser);
                        guMode=2002;
                        tSiteUser("");
                }
        }
}//void ProcesstSiteUserListVars(pentry entries[], int x)


int tSiteUserCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttSiteUserCommands(entries,x);

	if(!strcmp(gcFunction,"tSiteUserTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tSiteUserList();
		}

		//Default
		ProcesstSiteUserVars(entries,x);
		tSiteUser("");
	}
	else if(!strcmp(gcFunction,"tSiteUserList"))
	{
		ProcessControlVars(entries,x);
		ProcesstSiteUserListVars(entries,x);
		tSiteUserList();
	}

	return(0);

}//tSiteUserCommands()


void tSiteUser(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttSiteUserSelectRow();
		else
			ExttSiteUserSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetSiteUser();
				unxsApache("New tSiteUser table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tSiteUser WHERE uSiteUser=%u"
						,uSiteUser);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uSiteUser);
		sscanf(field[1],"%u",&uSite);
		sprintf(cLogin,"%.32s",field[2]);
		sprintf(cPasswd,"%.64s",field[3]);
		sscanf(field[4],"%u",&uAdmin);
		sscanf(field[5],"%u",&uSSH);
		sscanf(field[6],"%u",&uFTP);
		sscanf(field[7],"%u",&uPop3);
		sscanf(field[8],"%u",&uSMTP);
		sscanf(field[9],"%u",&uStatus);
		sscanf(field[10],"%u",&uClient);
		sscanf(field[11],"%u",&uOwner);
		sscanf(field[12],"%u",&uCreatedBy);
		sscanf(field[13],"%lu",&uCreatedDate);
		sscanf(field[14],"%u",&uModBy);
		sscanf(field[15],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tSiteUser",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tSiteUserTools>");
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

        ExttSiteUserButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tSiteUser Record Data",100);

	if(guMode==2000 || guMode==2002)
		tSiteUserInput(1);
	else
		tSiteUserInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttSiteUserAuxTable();

	Footer_ism3();

}//end of tSiteUser();


void tSiteUserInput(unsigned uMode)
{

//uSiteUser
	OpenRow(LANG_FL_tSiteUser_uSiteUser,"black");
	printf("<input title='%s' type=text name=uSiteUser value=%u size=16 maxlength=10 "
,LANG_FT_tSiteUser_uSiteUser,uSiteUser);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uSiteUser value=%u >\n",uSiteUser);
	}
//uSite
	OpenRow(LANG_FL_tSiteUser_uSite,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uSite value=%u >\n",ForeignKey("tSite","cDomain",uSite),uSite);
	}
	else
	{
	printf("%s<input type=hidden name=uSite value=%u >\n",ForeignKey("tSite","cDomain",uSite),uSite);
	}
//cLogin
	OpenRow(LANG_FL_tSiteUser_cLogin,"black");
	printf("<input title='%s' type=text name=cLogin value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tSiteUser_cLogin,EncodeDoubleQuotes(cLogin));
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLogin value=\"%s\">\n",EncodeDoubleQuotes(cLogin));
	}
//cPasswd
	OpenRow(LANG_FL_tSiteUser_cPasswd,"black");
	printf("<input title='%s' type=text name=cPasswd value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tSiteUser_cPasswd,EncodeDoubleQuotes(cPasswd));
	if((guPermLevel>=20 && uMode) || guMode==2002)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cPasswd value=\"%s\">\n",EncodeDoubleQuotes(cPasswd));
	}
//uAdmin
	OpenRow(LANG_FL_tSiteUser_uAdmin,"black");
	if(guPermLevel>=20 && uMode)
		YesNoPullDown("uAdmin",uAdmin,1);
	else
		YesNoPullDown("uAdmin",uAdmin,0);
//uSSH
	OpenRow(LANG_FL_tSiteUser_uSSH,"black");
	if(guPermLevel>=20 && uMode)
		YesNoPullDown("uSSH",uSSH,1);
	else
		YesNoPullDown("uSSH",uSSH,0);
//uFTP
	OpenRow(LANG_FL_tSiteUser_uFTP,"black");
	if(guPermLevel>=20 && uMode)
		YesNoPullDown("uFTP",uFTP,1);
	else
		YesNoPullDown("uFTP",uFTP,0);
//uPop3
	OpenRow(LANG_FL_tSiteUser_uPop3,"black");
	if(guPermLevel>=20 && uMode)
		YesNoPullDown("uPop3",uPop3,1);
	else
		YesNoPullDown("uPop3",uPop3,0);
//uSMTP
	OpenRow(LANG_FL_tSiteUser_uSMTP,"black");
	if(guPermLevel>=20 && uMode)
		YesNoPullDown("uSMTP",uSMTP,1);
	else
		YesNoPullDown("uSMTP",uSMTP,0);
//uStatus
	OpenRow(LANG_FL_tSiteUser_uStatus,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uStatus value=%u >\n",ForeignKey("tStatus","cLabel",uStatus),uStatus);
	}
	else
	{
	printf("%s<input type=hidden name=uStatus value=%u >\n",ForeignKey("tStatus","cLabel",uStatus),uStatus);
	}
//uClient
	OpenRow(LANG_FL_tSite_uClient,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
	else
	{
	printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
//uOwner
	OpenRow(LANG_FL_tSiteUser_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tSiteUser_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tSiteUser_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tSiteUser_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tSiteUser_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tSiteUserInput(unsigned uMode)


void NewtSiteUser(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uSiteUser FROM tSiteUser\
				WHERE uSiteUser=%u"
							,uSiteUser);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tSiteUser("<blink>Record already exists");
		tSiteUser(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tSiteUser();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uSiteUser=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tSiteUser",uSiteUser);
	unxsApacheLog(uSiteUser,"tSiteUser","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uSiteUser);
	tSiteUser(gcQuery);
	}

}//NewtSiteUser(unsigned uMode)


void DeletetSiteUser(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tSiteUser WHERE uSiteUser=%u AND ( uOwner=%u OR %u>9 )"
					,uSiteUser,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tSiteUser WHERE uSiteUser=%u"
					,uSiteUser);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tSiteUser("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsApacheLog(uSiteUser,"tSiteUser","Del");
#endif
		tSiteUser(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsApacheLog(uSiteUser,"tSiteUser","DelError");
#endif
		tSiteUser(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetSiteUser(void)


void Insert_tSiteUser(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tSiteUser SET uSiteUser=%u,uSite=%u,cLogin='%s',cPasswd='%s',uAdmin=%u,uSSH=%u,uFTP=%u,uPop3=%u,uSMTP=%u,uStatus=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uSiteUser
			,uSite
			,TextAreaSave(cLogin)
			,TextAreaSave(cPasswd)
			,uAdmin
			,uSSH
			,uFTP
			,uPop3
			,uSMTP
			,uStatus
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tSiteUser(void)


void Update_tSiteUser(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tSiteUser SET uSiteUser=%u,uSite=%u,cLogin='%s',cPasswd='%s',uAdmin=%u,uSSH=%u,uFTP=%u,uPop3=%u,uSMTP=%u,uStatus=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uSiteUser
			,uSite
			,TextAreaSave(cLogin)
			,TextAreaSave(cPasswd)
			,uAdmin
			,uSSH
			,uFTP
			,uPop3
			,uSMTP
			,uStatus
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tSiteUser(void)


void ModtSiteUser(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uSiteUser,uModDate FROM tSiteUser WHERE uSiteUser=%u"
						,uSiteUser);
#else
	sprintf(gcQuery,"SELECT uSiteUser FROM tSiteUser WHERE uSiteUser=%u"
						,uSiteUser);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tSiteUser("<blink>Record does not exist");
	if(i<1) tSiteUser(LANG_NBR_RECNOTEXIST);
	//if(i>1) tSiteUser("<blink>Multiple rows!");
	if(i>1) tSiteUser(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tSiteUser(LANG_NBR_EXTMOD);
#endif

	Update_tSiteUser(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tSiteUser",uSiteUser);
	unxsApacheLog(uSiteUser,"tSiteUser","Mod");
#endif
	tSiteUser(gcQuery);

}//ModtSiteUser(void)


void tSiteUserList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttSiteUserListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tSiteUserList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttSiteUserListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uSiteUser<td><font face=arial,helvetica color=white>uSite<td><font face=arial,helvetica color=white>cLogin<td><font face=arial,helvetica color=white>cPasswd<td><font face=arial,helvetica color=white>uAdmin<td><font face=arial,helvetica color=white>uSSH<td><font face=arial,helvetica color=white>uFTP<td><font face=arial,helvetica color=white>uPop3<td><font face=arial,helvetica color=white>uSMTP<td><font face=arial,helvetica color=white>uStatus<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luYesNo4=strtoul(field[4],NULL,10);
		char cBuf4[4];
		if(luYesNo4)
			sprintf(cBuf4,"Yes");
		else
			sprintf(cBuf4,"No");
		time_t luYesNo5=strtoul(field[5],NULL,10);
		char cBuf5[4];
		if(luYesNo5)
			sprintf(cBuf5,"Yes");
		else
			sprintf(cBuf5,"No");
		time_t luYesNo6=strtoul(field[6],NULL,10);
		char cBuf6[4];
		if(luYesNo6)
			sprintf(cBuf6,"Yes");
		else
			sprintf(cBuf6,"No");
		time_t luYesNo7=strtoul(field[7],NULL,10);
		char cBuf7[4];
		if(luYesNo7)
			sprintf(cBuf7,"Yes");
		else
			sprintf(cBuf7,"No");
		time_t luYesNo8=strtoul(field[8],NULL,10);
		char cBuf8[4];
		if(luYesNo8)
			sprintf(cBuf8,"Yes");
		else
			sprintf(cBuf8,"No");
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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,ForeignKey("tSite","cDomain",strtoul(field[1],NULL,10))
			,field[2]
			,field[3]
			,cBuf4
			,cBuf5
			,cBuf6
			,cBuf7
			,cBuf8
			,ForeignKey("tStatus","cLabel",strtoul(field[9],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[10],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[11],NULL,10))
			,cBuf12
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[13],NULL,10))
			,cBuf14
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tSiteUserList()


void CreatetSiteUser(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tSiteUser ( uSiteUser INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLogin VARCHAR(32) NOT NULL DEFAULT '',unique (cLogin,uSite), uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uSite INT UNSIGNED NOT NULL DEFAULT 0, uAdmin INT UNSIGNED NOT NULL DEFAULT 0, uSSH INT UNSIGNED NOT NULL DEFAULT 0, uFTP INT UNSIGNED NOT NULL DEFAULT 0, uPop3 INT UNSIGNED NOT NULL DEFAULT 0, uSMTP INT UNSIGNED NOT NULL DEFAULT 0, cPasswd VARCHAR(64) NOT NULL DEFAULT '', uStatus INT UNSIGNED NOT NULL DEFAULT 0, uClient INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetSiteUser()

//perlSAR patch1
