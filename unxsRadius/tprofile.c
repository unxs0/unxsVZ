/*
FILE
	tProfile source code of unxsRadius.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tprofilefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uProfile: Primary Key
static unsigned uProfile=0;
//uUser: If not 0 then only for this tUser.uUser
static unsigned uUser=0;
static char cuUserPullDown[256]={""};
//uServer: If not 0 then only for this tServer.uServer
static unsigned uServer=0;
static char cuServerPullDown[256]={""};
//uProfileName: Profile Name
static unsigned uProfileName=0;
static char cuProfileNamePullDown[256]={""};
//uStaticIP: Static IP version of uProfileName
static unsigned uStaticIP=0;
static char cYesNouStaticIP[32]={""};
//uClearText: CHAP or PAP version of uProfileName
static unsigned uClearText=0;
static char cYesNouClearText[32]={""};
//cComment: Comments about profile
static char *cComment={""};
//cProfile: Profile sprintf() template
static char *cProfile={""};
//uHourStart: Allow service starting at this daily hour
static unsigned uHourStart=0;
//uHourEnd: Allow service ending at this daily hour
static unsigned uHourEnd=0;
//uWeekDayStart: Allow service starting on week day (sunday=0)
static unsigned uWeekDayStart=0;
//uWeekDayEnd: Allow service ending on this week day
static unsigned uWeekDayEnd=0;
//uActivationDate: Activate service this date
static time_t uActivationDate=0;
//uDeactivationDate: Deactivate service this date
static time_t uDeactivationDate=0;
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



#define VAR_LIST_tProfile "tProfile.uProfile,tProfile.uUser,tProfile.uServer,tProfile.uProfileName,tProfile.uStaticIP,tProfile.uClearText,tProfile.cComment,tProfile.cProfile,tProfile.uHourStart,tProfile.uHourEnd,tProfile.uWeekDayStart,tProfile.uWeekDayEnd,tProfile.uActivationDate,tProfile.uDeactivationDate,tProfile.uOwner,tProfile.uCreatedBy,tProfile.uCreatedDate,tProfile.uModBy,tProfile.uModDate"

 //Local only
void Insert_tProfile(void);
void Update_tProfile(char *cRowid);
void ProcesstProfileListVars(pentry entries[], int x);

 //In tProfilefunc.h file included below
void ExtProcesstProfileVars(pentry entries[], int x);
void ExttProfileCommands(pentry entries[], int x);
void ExttProfileButtons(void);
void ExttProfileNavBar(void);
void ExttProfileGetHook(entry gentries[], int x);
void ExttProfileSelect(void);
void ExttProfileSelectRow(void);
void ExttProfileListSelect(void);
void ExttProfileListFilter(void);
void ExttProfileAuxTable(void);

#include "tprofilefunc.h"

 //Table Variables Assignment Function
void ProcesstProfileVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uProfile"))
			sscanf(entries[i].val,"%u",&uProfile);
		else if(!strcmp(entries[i].name,"uUser"))
			sscanf(entries[i].val,"%u",&uUser);
		else if(!strcmp(entries[i].name,"cuUserPullDown"))
		{
			sprintf(cuUserPullDown,"%.255s",entries[i].val);
			uUser=ReadPullDown("tUser","cLogin",cuUserPullDown);
		}
		else if(!strcmp(entries[i].name,"uServer"))
			sscanf(entries[i].val,"%u",&uServer);
		else if(!strcmp(entries[i].name,"cuServerPullDown"))
		{
			sprintf(cuServerPullDown,"%.255s",entries[i].val);
			uServer=ReadPullDown("tServer","cLabel",cuServerPullDown);
		}
		else if(!strcmp(entries[i].name,"uProfileName"))
			sscanf(entries[i].val,"%u",&uProfileName);
		else if(!strcmp(entries[i].name,"cuProfileNamePullDown"))
		{
			sprintf(cuProfileNamePullDown,"%.255s",entries[i].val);
			uProfileName=ReadPullDown("tProfileName","cLabel",cuProfileNamePullDown);
		}
		else if(!strcmp(entries[i].name,"uStaticIP"))
			sscanf(entries[i].val,"%u",&uStaticIP);
		else if(!strcmp(entries[i].name,"cYesNouStaticIP"))
		{
			sprintf(cYesNouStaticIP,"%.31s",entries[i].val);
			uStaticIP=ReadYesNoPullDown(cYesNouStaticIP);
		}
		else if(!strcmp(entries[i].name,"uClearText"))
			sscanf(entries[i].val,"%u",&uClearText);
		else if(!strcmp(entries[i].name,"cYesNouClearText"))
		{
			sprintf(cYesNouClearText,"%.31s",entries[i].val);
			uClearText=ReadYesNoPullDown(cYesNouClearText);
		}
		else if(!strcmp(entries[i].name,"cComment"))
			cComment=entries[i].val;
		else if(!strcmp(entries[i].name,"cProfile"))
			cProfile=entries[i].val;
		else if(!strcmp(entries[i].name,"uHourStart"))
			sscanf(entries[i].val,"%u",&uHourStart);
		else if(!strcmp(entries[i].name,"uHourEnd"))
			sscanf(entries[i].val,"%u",&uHourEnd);
		else if(!strcmp(entries[i].name,"uWeekDayStart"))
			sscanf(entries[i].val,"%u",&uWeekDayStart);
		else if(!strcmp(entries[i].name,"uWeekDayEnd"))
			sscanf(entries[i].val,"%u",&uWeekDayEnd);
		else if(!strcmp(entries[i].name,"uActivationDate"))
			sscanf(entries[i].val,"%lu",&uActivationDate);
		else if(!strcmp(entries[i].name,"uDeactivationDate"))
			sscanf(entries[i].val,"%lu",&uDeactivationDate);
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
	ExtProcesstProfileVars(entries,x);

}//ProcesstProfileVars()


void ProcesstProfileListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uProfile);
                        guMode=2002;
                        tProfile("");
                }
        }
}//void ProcesstProfileListVars(pentry entries[], int x)


int tProfileCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttProfileCommands(entries,x);

	if(!strcmp(gcFunction,"tProfileTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tProfileList();
		}

		//Default
		ProcesstProfileVars(entries,x);
		tProfile("");
	}
	else if(!strcmp(gcFunction,"tProfileList"))
	{
		ProcessControlVars(entries,x);
		ProcesstProfileListVars(entries,x);
		tProfileList();
	}

	return(0);

}//tProfileCommands()


void tProfile(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttProfileSelectRow();
		else
			ExttProfileSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetProfile();
				unxsRadius("New tProfile table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tProfile WHERE uProfile=%u"
						,uProfile);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uProfile);
		sscanf(field[1],"%u",&uUser);
		sscanf(field[2],"%u",&uServer);
		sscanf(field[3],"%u",&uProfileName);
		sscanf(field[4],"%u",&uStaticIP);
		sscanf(field[5],"%u",&uClearText);
		cComment=field[6];
		cProfile=field[7];
		sscanf(field[8],"%u",&uHourStart);
		sscanf(field[9],"%u",&uHourEnd);
		sscanf(field[10],"%u",&uWeekDayStart);
		sscanf(field[11],"%u",&uWeekDayEnd);
		sscanf(field[12],"%lu",&uActivationDate);
		sscanf(field[13],"%lu",&uDeactivationDate);
		sscanf(field[14],"%u",&uOwner);
		sscanf(field[15],"%u",&uCreatedBy);
		sscanf(field[16],"%lu",&uCreatedDate);
		sscanf(field[17],"%u",&uModBy);
		sscanf(field[18],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tProfile",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tProfileTools>");
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

        ExttProfileButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tProfile Record Data",100);

	if(guMode==2000 || guMode==2002)
		tProfileInput(1);
	else
		tProfileInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttProfileAuxTable();

	Footer_ism3();

}//end of tProfile();


void tProfileInput(unsigned uMode)
{

//uProfile
	OpenRow(LANG_FL_tProfile_uProfile,"black");
	printf("<input title='%s' type=text name=uProfile value=%u size=16 maxlength=10 "
,LANG_FT_tProfile_uProfile,uProfile);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uProfile value=%u >\n",uProfile);
	}
//uUser
	OpenRow(LANG_FL_tProfile_uUser,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tUser;cuUserPullDown","cLogin","cLogin",uUser,1);
	else
		tTablePullDown("tUser;cuUserPullDown","cLogin","cLogin",uUser,0);
//uServer
	OpenRow(LANG_FL_tProfile_uServer,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tServer;cuServerPullDown","cLabel","cLabel",uServer,1);
	else
		tTablePullDown("tServer;cuServerPullDown","cLabel","cLabel",uServer,0);
//uProfileName
	OpenRow(LANG_FL_tProfile_uProfileName,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tProfileName;cuProfileNamePullDown","cLabel","cLabel",uProfileName,1);
	else
		tTablePullDown("tProfileName;cuProfileNamePullDown","cLabel","cLabel",uProfileName,0);
//uStaticIP
	OpenRow(LANG_FL_tProfile_uStaticIP,"black");
	if(guPermLevel>=10 && uMode)
		YesNoPullDown("uStaticIP",uStaticIP,1);
	else
		YesNoPullDown("uStaticIP",uStaticIP,0);
//uClearText
	OpenRow(LANG_FL_tProfile_uClearText,"black");
	if(guPermLevel>=10 && uMode)
		YesNoPullDown("uClearText",uClearText,1);
	else
		YesNoPullDown("uClearText",uClearText,0);
//cComment
	OpenRow(LANG_FL_tProfile_cComment,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cComment "
,LANG_FT_tProfile_cComment);
	if(guPermLevel>=10 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cComment);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cComment);
		printf("<input type=hidden name=cComment value=\"%s\" >\n",EncodeDoubleQuotes(cComment));
	}
//cProfile
	OpenRow(LANG_FL_tProfile_cProfile,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cProfile "
,LANG_FT_tProfile_cProfile);
	if(guPermLevel>=10 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cProfile);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cProfile);
		printf("<input type=hidden name=cProfile value=\"%s\" >\n",EncodeDoubleQuotes(cProfile));
	}
//uHourStart
	OpenRow(LANG_FL_tProfile_uHourStart,"black");
	printf("<input title='%s' type=text name=uHourStart value=%u size=16 maxlength=10 "
,LANG_FT_tProfile_uHourStart,uHourStart);
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uHourStart value=%u >\n",uHourStart);
	}
//uHourEnd
	OpenRow(LANG_FL_tProfile_uHourEnd,"black");
	printf("<input title='%s' type=text name=uHourEnd value=%u size=16 maxlength=10 "
,LANG_FT_tProfile_uHourEnd,uHourEnd);
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uHourEnd value=%u >\n",uHourEnd);
	}
//uWeekDayStart
	OpenRow(LANG_FL_tProfile_uWeekDayStart,"black");
	printf("<input title='%s' type=text name=uWeekDayStart value=%u size=16 maxlength=10 "
,LANG_FT_tProfile_uWeekDayStart,uWeekDayStart);
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uWeekDayStart value=%u >\n",uWeekDayStart);
	}
//uWeekDayEnd
	OpenRow(LANG_FL_tProfile_uWeekDayEnd,"black");
	printf("<input title='%s' type=text name=uWeekDayEnd value=%u size=16 maxlength=10 "
,LANG_FT_tProfile_uWeekDayEnd,uWeekDayEnd);
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uWeekDayEnd value=%u >\n",uWeekDayEnd);
	}
//uActivationDate
	OpenRow(LANG_FL_tProfile_uActivationDate,"black");
	if(uActivationDate)
		printf("<input type=text name=cuActivationDate value='%s' disabled>\n",ctime(&uActivationDate));
	else
		printf("<input type=text name=cuActivationDate value='---' disabled>\n");
	printf("<input type=hidden name=uActivationDate value=%lu>\n",uActivationDate);
//uDeactivationDate
	OpenRow(LANG_FL_tProfile_uDeactivationDate,"black");
	if(uDeactivationDate)
		printf("<input type=text name=cuDeactivationDate value='%s' disabled>\n",ctime(&uDeactivationDate));
	else
		printf("<input type=text name=cuDeactivationDate value='---' disabled>\n");
	printf("<input type=hidden name=uDeactivationDate value=%lu>\n",uDeactivationDate);
//uOwner
	OpenRow(LANG_FL_tProfile_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tProfile_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tProfile_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tProfile_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tProfile_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tProfileInput(unsigned uMode)


void NewtProfile(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uProfile FROM tProfile WHERE uProfile=%u",uProfile);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tProfile("<blink>Record already exists");
		tProfile(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tProfile();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uProfile=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tProfile",uProfile);
	unxsRadiusLog(uProfile,"tProfile","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uProfile);
		tProfile(gcQuery);
	}

}//NewtProfile(unsigned uMode)


void DeletetProfile(void)
{
	sprintf(gcQuery,"DELETE FROM tProfile WHERE uProfile=%u",uProfile);
	macro_mySQLQueryHTMLError;

	//tProfile("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsRadiusLog(uProfile,"tProfile","Del");
		tProfile(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsRadiusLog(uProfile,"tProfile","DelError");
		tProfile(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetProfile(void)


void Insert_tProfile(void)
{
	sprintf(gcQuery,"INSERT INTO tProfile SET uProfile=%u,uUser=%u,uServer=%u,uProfileName=%u,"
			"uStaticIP=%u,uClearText=%u,cComment='%s',cProfile='%s',uHourStart=%u,uHourEnd=%u,"
			"uWeekDayStart=%u,uWeekDayEnd=%u,uActivationDate=%lu,uDeactivationDate=%lu,uOwner=%u,"
			"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uProfile
				,uUser
				,uServer
				,uProfileName
				,uStaticIP
				,uClearText
				,TextAreaSave(cComment)
				,TextAreaSave(cProfile)
				,uHourStart
				,uHourEnd
				,uWeekDayStart
				,uWeekDayEnd
				,uActivationDate
				,uDeactivationDate
				,uOwner
				,uCreatedBy);
	macro_mySQLQueryHTMLError;
}//void Insert_tProfile(void)


void Update_tProfile(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tProfile SET uProfile=%u,uUser=%u,uServer=%u,uProfileName=%u,uStaticIP=%u,"
			"uClearText=%u,cComment='%s',cProfile='%s',uHourStart=%u,uHourEnd=%u,"
			"uWeekDayStart=%u,uWeekDayEnd=%u,uActivationDate=%lu,uDeactivationDate=%lu,uModBy=%u,"
			"uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
				uProfile
				,uUser
				,uServer
				,uProfileName
				,uStaticIP
				,uClearText
				,TextAreaSave(cComment)
				,TextAreaSave(cProfile)
				,uHourStart
				,uHourEnd
				,uWeekDayStart
				,uWeekDayEnd
				,uActivationDate
				,uDeactivationDate
				,uModBy
				,cRowid);
	macro_mySQLQueryHTMLError;
}//void Update_tProfile(void)


void ModtProfile(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uProfile,uModDate FROM tProfile WHERE uProfile=%u",uProfile);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tProfile("<blink>Record does not exist");
	if(i<1) tProfile(LANG_NBR_RECNOTEXIST);
	//if(i>1) tProfile("<blink>Multiple rows!");
	if(i>1) tProfile(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tProfile(LANG_NBR_EXTMOD);

	Update_tProfile(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tProfile",uProfile);
	unxsRadiusLog(uProfile,"tProfile","Mod");
	tProfile(gcQuery);

}//ModtProfile(void)


void tProfileList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttProfileListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tProfileList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttProfileListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uProfile<td><font face=arial,helvetica color=white>uUser<td><font face=arial,helvetica color=white>uServer<td><font face=arial,helvetica color=white>uProfileName<td><font face=arial,helvetica color=white>uStaticIP<td><font face=arial,helvetica color=white>uClearText<td><font face=arial,helvetica color=white>cComment<td><font face=arial,helvetica color=white>cProfile<td><font face=arial,helvetica color=white>uHourStart<td><font face=arial,helvetica color=white>uHourEnd<td><font face=arial,helvetica color=white>uWeekDayStart<td><font face=arial,helvetica color=white>uWeekDayEnd<td><font face=arial,helvetica color=white>uActivationDate<td><font face=arial,helvetica color=white>uDeactivationDate<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime12=strtoul(field[12],NULL,10);
		char cBuf12[32];
		if(luTime12)
			ctime_r(&luTime12,cBuf12);
		else
			sprintf(cBuf12,"---");
		time_t luTime13=strtoul(field[13],NULL,10);
		char cBuf13[32];
		if(luTime13)
			ctime_r(&luTime13,cBuf13);
		else
			sprintf(cBuf13,"---");
		time_t luTime16=strtoul(field[16],NULL,10);
		char cBuf16[32];
		if(luTime16)
			ctime_r(&luTime16,cBuf16);
		else
			sprintf(cBuf16,"---");
		time_t luTime18=strtoul(field[18],NULL,10);
		char cBuf18[32];
		if(luTime18)
			ctime_r(&luTime18,cBuf18);
		else
			sprintf(cBuf18,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td><textarea disabled>%s</textarea><td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,ForeignKey("tUser","cLogin",strtoul(field[1],NULL,10))
			,ForeignKey("tServer","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tProfileName","cLabel",strtoul(field[3],NULL,10))
			,cBuf4
			,cBuf5
			,field[6]
			,field[7]
			,field[8]
			,field[9]
			,field[10]
			,field[11]
			,cBuf12
			,cBuf13
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[14],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[15],NULL,10))
			,cBuf16
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[17],NULL,10))
			,cBuf18
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tProfileList()


void CreatetProfile(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tProfile ( uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner), uProfileName INT UNSIGNED NOT NULL DEFAULT 0, uProfile INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cProfile TEXT NOT NULL DEFAULT '', uWeekDayStart INT UNSIGNED NOT NULL DEFAULT 0, uWeekDayEnd INT UNSIGNED NOT NULL DEFAULT 0, uHourStart INT UNSIGNED NOT NULL DEFAULT 0, uHourEnd INT UNSIGNED NOT NULL DEFAULT 0, uServer INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uServer), uActivationDate INT UNSIGNED NOT NULL DEFAULT 0, uDeactivationDate INT UNSIGNED NOT NULL DEFAULT 0, uUser INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uServer), uStaticIP INT UNSIGNED NOT NULL DEFAULT 0, uClearText INT UNSIGNED NOT NULL DEFAULT 0, cComment TEXT NOT NULL DEFAULT '' )");
	macro_mySQLQueryHTMLError;
}//CreatetProfile()

