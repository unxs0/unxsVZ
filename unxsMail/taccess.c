/*
FILE
	tAccess source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in taccessfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uAccess: Primary Key
static unsigned uAccess=0;
//cDomainIP: Domain or IP: Class C class B of full IP
static char cDomainIP[65]={""};
//cRelayAttr: Allow, reject or other error message
static char cRelayAttr[65]={""};
//uServerGroup: ServerGroup account is on
static unsigned uServerGroup=0;
static char cuServerGroupPullDown[256]={""};
//uExpireDate: Expire this tAccess entry at this time
static time_t uExpireDate=0;
//uSource: Where this record came from
static unsigned uSource=0;
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



#define VAR_LIST_tAccess "tAccess.uAccess,tAccess.cDomainIP,tAccess.cRelayAttr,tAccess.uServerGroup,tAccess.uExpireDate,tAccess.uSource,tAccess.uOwner,tAccess.uCreatedBy,tAccess.uCreatedDate,tAccess.uModBy,tAccess.uModDate"

 //Local only
void Insert_tAccess(void);
void Update_tAccess(char *cRowid);
void ProcesstAccessListVars(pentry entries[], int x);

 //In tAccessfunc.h file included below
void ExtProcesstAccessVars(pentry entries[], int x);
void ExttAccessCommands(pentry entries[], int x);
void ExttAccessButtons(void);
void ExttAccessNavBar(void);
void ExttAccessGetHook(entry gentries[], int x);
void ExttAccessSelect(void);
void ExttAccessSelectRow(void);
void ExttAccessListSelect(void);
void ExttAccessListFilter(void);
void ExttAccessAuxTable(void);

#include "taccessfunc.h"

 //Table Variables Assignment Function
void ProcesstAccessVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uAccess"))
			sscanf(entries[i].val,"%u",&uAccess);
		else if(!strcmp(entries[i].name,"cDomainIP"))
			sprintf(cDomainIP,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cRelayAttr"))
			sprintf(cRelayAttr,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uServerGroup"))
			sscanf(entries[i].val,"%u",&uServerGroup);
		else if(!strcmp(entries[i].name,"cuServerGroupPullDown"))
		{
			sprintf(cuServerGroupPullDown,"%.255s",entries[i].val);
			uServerGroup=ReadPullDown("tServerGroup","cLabel",cuServerGroupPullDown);
		}
		else if(!strcmp(entries[i].name,"uExpireDate"))
			sscanf(entries[i].val,"%lu",&uExpireDate);
		else if(!strcmp(entries[i].name,"uSource"))
			sscanf(entries[i].val,"%u",&uSource);
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
	ExtProcesstAccessVars(entries,x);

}//ProcesstAccessVars()


void ProcesstAccessListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uAccess);
                        guMode=2002;
                        tAccess("");
                }
        }
}//void ProcesstAccessListVars(pentry entries[], int x)


int tAccessCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttAccessCommands(entries,x);

	if(!strcmp(gcFunction,"tAccessTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tAccessList();
		}

		//Default
		ProcesstAccessVars(entries,x);
		tAccess("");
	}
	else if(!strcmp(gcFunction,"tAccessList"))
	{
		ProcessControlVars(entries,x);
		ProcesstAccessListVars(entries,x);
		tAccessList();
	}

	return(0);

}//tAccessCommands()


void tAccess(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttAccessSelectRow();
		else
			ExttAccessSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetAccess();
				unxsMail("New tAccess table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tAccess WHERE uAccess=%u"
						,uAccess);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uAccess);
		sprintf(cDomainIP,"%.64s",field[1]);
		sprintf(cRelayAttr,"%.64s",field[2]);
		sscanf(field[3],"%u",&uServerGroup);
		sscanf(field[4],"%lu",&uExpireDate);
		sscanf(field[5],"%u",&uSource);
		sscanf(field[6],"%u",&uOwner);
		sscanf(field[7],"%u",&uCreatedBy);
		sscanf(field[8],"%lu",&uCreatedDate);
		sscanf(field[9],"%u",&uModBy);
		sscanf(field[10],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tAccess",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tAccessTools>");
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

        ExttAccessButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tAccess Record Data",100);

	if(guMode==2000 || guMode==2002)
		tAccessInput(1);
	else
		tAccessInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttAccessAuxTable();

	Footer_ism3();

}//end of tAccess();


void tAccessInput(unsigned uMode)
{

//uAccess
	OpenRow(LANG_FL_tAccess_uAccess,"black");
	printf("<input title='%s' type=text name=uAccess value=%u size=16 maxlength=10 "
,LANG_FT_tAccess_uAccess,uAccess);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uAccess value=%u >\n",uAccess);
	}
//cDomainIP
	OpenRow(LANG_FL_tAccess_cDomainIP,"black");
	printf("<input title='%s' type=text name=cDomainIP value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tAccess_cDomainIP,EncodeDoubleQuotes(cDomainIP));
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cDomainIP value=\"%s\">\n",EncodeDoubleQuotes(cDomainIP));
	}
//cRelayAttr
	OpenRow(LANG_FL_tAccess_cRelayAttr,"black");
	printf("<input title='%s' type=text name=cRelayAttr value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tAccess_cRelayAttr,EncodeDoubleQuotes(cRelayAttr));
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cRelayAttr value=\"%s\">\n",EncodeDoubleQuotes(cRelayAttr));
	}
//uServerGroup
	OpenRow(LANG_FL_tAccess_uServerGroup,"black");
	if(guPermLevel>=8 && uMode)
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,1);
	else
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,0);
//uExpireDate
	OpenRow(LANG_FL_tAccess_uExpireDate,"black");
	if(uExpireDate)
		printf("<input type=text name=cuExpireDate value='%s' disabled>\n",ctime(&uExpireDate));
	else
		printf("<input type=text name=cuExpireDate value='---' disabled>\n");
	printf("<input type=hidden name=uExpireDate value=%lu>\n",uExpireDate);
//uSource
	OpenRow(LANG_FL_tAccess_uSource,"black");
	printf("<input title='%s' type=text name=uSource value=%u size=16 maxlength=10 "
,LANG_FT_tAccess_uSource,uSource);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uSource value=%u >\n",uSource);
	}
//uOwner
	OpenRow(LANG_FL_tAccess_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tAccess_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tAccess_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tAccess_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tAccess_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tAccessInput(unsigned uMode)


void NewtAccess(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uAccess FROM tAccess\
				WHERE uAccess=%u"
							,uAccess);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tAccess("<blink>Record already exists");
		tAccess(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tAccess();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uAccess=mysql_insert_id(&gMysql);

	uCreatedDate=luGetCreatedDate("tAccess",uAccess);
	unxsMailLog(uAccess,"tAccess","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uAccess);
	tAccess(gcQuery);
	}

}//NewtAccess(unsigned uMode)


void DeletetAccess(void)
{

	sprintf(gcQuery,"DELETE FROM tAccess WHERE uAccess=%u AND ( uOwner=%u OR %u>9 )"
					,uAccess,guLoginClient,guPermLevel);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tAccess("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uAccess,"tAccess","Del");

		tAccess(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uAccess,"tAccess","DelError");

		tAccess(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetAccess(void)


void Insert_tAccess(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tAccess SET uAccess=%u,cDomainIP='%s',cRelayAttr='%s',uServerGroup=%u,uExpireDate=%lu,uSource=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uAccess
			,TextAreaSave(cDomainIP)
			,TextAreaSave(cRelayAttr)
			,uServerGroup
			,uExpireDate
			,uSource
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tAccess(void)


void Update_tAccess(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tAccess SET uAccess=%u,cDomainIP='%s',cRelayAttr='%s',uServerGroup=%u,uExpireDate=%lu,uSource=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uAccess
			,TextAreaSave(cDomainIP)
			,TextAreaSave(cRelayAttr)
			,uServerGroup
			,uExpireDate
			,uSource
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tAccess(void)


void ModtAccess(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPreModDate=0;
	sprintf(gcQuery,"SELECT uAccess,uModDate FROM tAccess WHERE uAccess=%u"
						,uAccess);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tAccess("<blink>Record does not exist");
	if(i<1) tAccess(LANG_NBR_RECNOTEXIST);
	//if(i>1) tAccess("<blink>Multiple rows!");
	if(i>1) tAccess(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tAccess(LANG_NBR_EXTMOD);


	Update_tAccess(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);

	uModDate=luGetModDate("tAccess",uAccess);
	unxsMailLog(uAccess,"tAccess","Mod");

	tAccess(gcQuery);

}//ModtAccess(void)


void tAccessList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttAccessListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tAccessList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttAccessListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uAccess<td><font face=arial,helvetica color=white>cDomainIP<td><font face=arial,helvetica color=white>cRelayAttr<td><font face=arial,helvetica color=white>uServerGroup<td><font face=arial,helvetica color=white>uExpireDate<td><font face=arial,helvetica color=white>uSource<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime8=strtoul(field[8],NULL,10);
		char cBuf8[32];
		if(luTime8)
			ctime_r(&luTime8,cBuf8);
		else
			sprintf(cBuf8,"---");
		time_t luTime10=strtoul(field[10],NULL,10);
		char cBuf10[32];
		if(luTime10)
			ctime_r(&luTime10,cBuf10);
		else
			sprintf(cBuf10,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,ForeignKey("tServerGroup","cLabel",strtoul(field[3],NULL,10))
			,cBuf4
			,field[5]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[7],NULL,10))
			,cBuf8
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[9],NULL,10))
			,cBuf10
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tAccessList()


void CreatetAccess(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tAccess ( uAccess INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cDomainIP VARCHAR(64) NOT NULL DEFAULT '',index (cDomainIP,uServerGroup), uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uServerGroup INT UNSIGNED NOT NULL DEFAULT 0,index (uServerGroup), cRelayAttr VARCHAR(64) NOT NULL DEFAULT '', uExpireDate INT UNSIGNED NOT NULL DEFAULT 0, uSource INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetAccess()

