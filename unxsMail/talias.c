/*
FILE
	tAlias source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in taliasfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uAlias: Primary Key
static unsigned uAlias=0;
//cUser: Local mail user
static char cUser[33]={""};
//cTargetEmail: Local mail user destination email or program
static char cTargetEmail[256]={""};
//uServerGroup: Server account is on
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



#define VAR_LIST_tAlias "tAlias.uAlias,tAlias.cUser,tAlias.cTargetEmail,tAlias.uServerGroup,tAlias.uOwner,tAlias.uCreatedBy,tAlias.uCreatedDate,tAlias.uModBy,tAlias.uModDate"

 //Local only
void Insert_tAlias(void);
void Update_tAlias(char *cRowid);
void ProcesstAliasListVars(pentry entries[], int x);

 //In tAliasfunc.h file included below
void ExtProcesstAliasVars(pentry entries[], int x);
void ExttAliasCommands(pentry entries[], int x);
void ExttAliasButtons(void);
void ExttAliasNavBar(void);
void ExttAliasGetHook(entry gentries[], int x);
void ExttAliasSelect(void);
void ExttAliasSelectRow(void);
void ExttAliasListSelect(void);
void ExttAliasListFilter(void);
void ExttAliasAuxTable(void);

#include "taliasfunc.h"

 //Table Variables Assignment Function
void ProcesstAliasVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uAlias"))
			sscanf(entries[i].val,"%u",&uAlias);
		else if(!strcmp(entries[i].name,"cUser"))
			sprintf(cUser,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cTargetEmail"))
			sprintf(cTargetEmail,"%.255s",entries[i].val);
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
	ExtProcesstAliasVars(entries,x);

}//ProcesstAliasVars()


void ProcesstAliasListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uAlias);
                        guMode=2002;
                        tAlias("");
                }
        }
}//void ProcesstAliasListVars(pentry entries[], int x)


int tAliasCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttAliasCommands(entries,x);

	if(!strcmp(gcFunction,"tAliasTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tAliasList();
		}

		//Default
		ProcesstAliasVars(entries,x);
		tAlias("");
	}
	else if(!strcmp(gcFunction,"tAliasList"))
	{
		ProcessControlVars(entries,x);
		ProcesstAliasListVars(entries,x);
		tAliasList();
	}

	return(0);

}//tAliasCommands()


void tAlias(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttAliasSelectRow();
		else
			ExttAliasSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetAlias();
				unxsMail("New tAlias table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tAlias WHERE uAlias=%u"
						,uAlias);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uAlias);
		sprintf(cUser,"%.32s",field[1]);
		sprintf(cTargetEmail,"%.255s",field[2]);
		sscanf(field[3],"%u",&uServerGroup);
		sscanf(field[4],"%u",&uOwner);
		sscanf(field[5],"%u",&uCreatedBy);
		sscanf(field[6],"%lu",&uCreatedDate);
		sscanf(field[7],"%u",&uModBy);
		sscanf(field[8],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tAlias",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tAliasTools>");
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

        ExttAliasButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tAlias Record Data",100);

	if(guMode==2000 || guMode==2002)
		tAliasInput(1);
	else
		tAliasInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttAliasAuxTable();

	Footer_ism3();

}//end of tAlias();


void tAliasInput(unsigned uMode)
{

//uAlias
	OpenRow(LANG_FL_tAlias_uAlias,"black");
	printf("<input title='%s' type=text name=uAlias value=%u size=16 maxlength=10 "
,LANG_FT_tAlias_uAlias,uAlias);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uAlias value=%u >\n",uAlias);
	}
//cUser
	OpenRow(LANG_FL_tAlias_cUser,"black");
	printf("<input title='%s' type=text name=cUser value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tAlias_cUser,EncodeDoubleQuotes(cUser));
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cUser value=\"%s\">\n",EncodeDoubleQuotes(cUser));
	}
//cTargetEmail
	OpenRow(LANG_FL_tAlias_cTargetEmail,"black");
	printf("<input title='%s' type=text name=cTargetEmail value=\"%s\" size=40 maxlength=255 "
,LANG_FT_tAlias_cTargetEmail,EncodeDoubleQuotes(cTargetEmail));
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cTargetEmail value=\"%s\">\n",EncodeDoubleQuotes(cTargetEmail));
	}
//uServerGroup
	OpenRow(LANG_FL_tAlias_uServerGroup,"black");
	if(guPermLevel>=8 && uMode)
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,1);
	else
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,0);
//uOwner
	OpenRow(LANG_FL_tAlias_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tAlias_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tAlias_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tAlias_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tAlias_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tAliasInput(unsigned uMode)


void NewtAlias(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uAlias FROM tAlias\
				WHERE uAlias=%u"
							,uAlias);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tAlias("<blink>Record already exists");
		tAlias(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tAlias();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uAlias=mysql_insert_id(&gMysql);

	uCreatedDate=luGetCreatedDate("tAlias",uAlias);
	unxsMailLog(uAlias,"tAlias","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uAlias);
	tAlias(gcQuery);
	}

}//NewtAlias(unsigned uMode)


void DeletetAlias(void)
{

	sprintf(gcQuery,"DELETE FROM tAlias WHERE uAlias=%u AND ( uOwner=%u OR %u>9 )"
					,uAlias,guLoginClient,guPermLevel);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tAlias("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uAlias,"tAlias","Del");

		tAlias(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uAlias,"tAlias","DelError");

		tAlias(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetAlias(void)


void Insert_tAlias(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tAlias SET uAlias=%u,cUser='%s',cTargetEmail='%s',uServerGroup=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uAlias
			,TextAreaSave(cUser)
			,TextAreaSave(cTargetEmail)
			,uServerGroup
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tAlias(void)


void Update_tAlias(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tAlias SET uAlias=%u,cUser='%s',cTargetEmail='%s',uServerGroup=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uAlias
			,TextAreaSave(cUser)
			,TextAreaSave(cTargetEmail)
			,uServerGroup
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tAlias(void)


void ModtAlias(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPreModDate=0;
	sprintf(gcQuery,"SELECT uAlias,uModDate FROM tAlias WHERE uAlias=%u"
						,uAlias);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tAlias("<blink>Record does not exist");
	if(i<1) tAlias(LANG_NBR_RECNOTEXIST);
	//if(i>1) tAlias("<blink>Multiple rows!");
	if(i>1) tAlias(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tAlias(LANG_NBR_EXTMOD);


	Update_tAlias(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);

	uModDate=luGetModDate("tAlias",uAlias);
	unxsMailLog(uAlias,"tAlias","Mod");

	tAlias(gcQuery);

}//ModtAlias(void)


void tAliasList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttAliasListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tAliasList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttAliasListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uAlias<td><font face=arial,helvetica color=white>cUser<td><font face=arial,helvetica color=white>cTargetEmail<td><font face=arial,helvetica color=white>uServerGroup<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime6=strtoul(field[6],NULL,10);
		char cBuf6[32];
		if(luTime6)
			ctime_r(&luTime6,cBuf6);
		else
			sprintf(cBuf6,"---");
		time_t luTime8=strtoul(field[8],NULL,10);
		char cBuf8[32];
		if(luTime8)
			ctime_r(&luTime8,cBuf8);
		else
			sprintf(cBuf8,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,ForeignKey("tServerGroup","cLabel",strtoul(field[3],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[4],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,cBuf6
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[7],NULL,10))
			,cBuf8
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tAliasList()


void CreatetAlias(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tAlias ( uAlias INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cUser VARCHAR(32) NOT NULL DEFAULT '',index (cUser,uServerGroup), uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uServerGroup INT UNSIGNED NOT NULL DEFAULT 0,index (uServerGroup), cTargetEmail VARCHAR(255) NOT NULL DEFAULT '' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetAlias()

