/*
FILE
	tServerGroupGlue source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tservergroupgluefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uServerGroupGlue: Primary Key
static unsigned uServerGroupGlue=0;
//uServer: tServer.uServer
static unsigned uServer=0;
static char cuServerPullDown[256]={""};
//uServerGroup: tServerGroup.uServerGroup
static unsigned uServerGroup=0;
static char cuServerGroupPullDown[256]={""};



#define VAR_LIST_tServerGroupGlue "tServerGroupGlue.uServerGroupGlue,tServerGroupGlue.uServer,tServerGroupGlue.uServerGroup"

 //Local only
void Insert_tServerGroupGlue(void);
void Update_tServerGroupGlue(char *cRowid);
void ProcesstServerGroupGlueListVars(pentry entries[], int x);

 //In tServerGroupGluefunc.h file included below
void ExtProcesstServerGroupGlueVars(pentry entries[], int x);
void ExttServerGroupGlueCommands(pentry entries[], int x);
void ExttServerGroupGlueButtons(void);
void ExttServerGroupGlueNavBar(void);
void ExttServerGroupGlueGetHook(entry gentries[], int x);
void ExttServerGroupGlueSelect(void);
void ExttServerGroupGlueSelectRow(void);
void ExttServerGroupGlueListSelect(void);
void ExttServerGroupGlueListFilter(void);
void ExttServerGroupGlueAuxTable(void);

#include "tservergroupgluefunc.h"

 //Table Variables Assignment Function
void ProcesstServerGroupGlueVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uServerGroupGlue"))
			sscanf(entries[i].val,"%u",&uServerGroupGlue);
		else if(!strcmp(entries[i].name,"uServer"))
			sscanf(entries[i].val,"%u",&uServer);
		else if(!strcmp(entries[i].name,"cuServerPullDown"))
		{
			sprintf(cuServerPullDown,"%.255s",entries[i].val);
			uServer=ReadPullDown("tServer","cLabel",cuServerPullDown);
		}
		else if(!strcmp(entries[i].name,"uServerGroup"))
			sscanf(entries[i].val,"%u",&uServerGroup);
		else if(!strcmp(entries[i].name,"cuServerGroupPullDown"))
		{
			sprintf(cuServerGroupPullDown,"%.255s",entries[i].val);
			uServerGroup=ReadPullDown("tServerGroup","cLabel",cuServerGroupPullDown);
		}

	}

	//After so we can overwrite form data if needed.
	ExtProcesstServerGroupGlueVars(entries,x);

}//ProcesstServerGroupGlueVars()


void ProcesstServerGroupGlueListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uServerGroupGlue);
                        guMode=2002;
                        tServerGroupGlue("");
                }
        }
}//void ProcesstServerGroupGlueListVars(pentry entries[], int x)


int tServerGroupGlueCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttServerGroupGlueCommands(entries,x);

	if(!strcmp(gcFunction,"tServerGroupGlueTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tServerGroupGlueList();
		}

		//Default
		ProcesstServerGroupGlueVars(entries,x);
		tServerGroupGlue("");
	}
	else if(!strcmp(gcFunction,"tServerGroupGlueList"))
	{
		ProcessControlVars(entries,x);
		ProcesstServerGroupGlueListVars(entries,x);
		tServerGroupGlueList();
	}

	return(0);

}//tServerGroupGlueCommands()


void tServerGroupGlue(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttServerGroupGlueSelectRow();
		else
			ExttServerGroupGlueSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetServerGroupGlue();
				unxsMail("New tServerGroupGlue table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tServerGroupGlue WHERE uServerGroupGlue=%u"
						,uServerGroupGlue);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uServerGroupGlue);
		sscanf(field[1],"%u",&uServer);
		sscanf(field[2],"%u",&uServerGroup);

		}

	}//Internal Skip

	Header_ism3(":: tServerGroupGlue",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tServerGroupGlueTools>");
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

        ExttServerGroupGlueButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tServerGroupGlue Record Data",100);

	if(guMode==2000 || guMode==2002)
		tServerGroupGlueInput(1);
	else
		tServerGroupGlueInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttServerGroupGlueAuxTable();

	Footer_ism3();

}//end of tServerGroupGlue();


void tServerGroupGlueInput(unsigned uMode)
{

//uServerGroupGlue
	OpenRow(LANG_FL_tServerGroupGlue_uServerGroupGlue,"black");
	printf("<input title='%s' type=text name=uServerGroupGlue value=%u size=16 maxlength=10 "
,LANG_FT_tServerGroupGlue_uServerGroupGlue,uServerGroupGlue);
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uServerGroupGlue value=%u >\n",uServerGroupGlue);
	}
//uServer
	OpenRow(LANG_FL_tServerGroupGlue_uServer,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tServer;cuServerPullDown","cLabel","cLabel",uServer,1);
	else
		tTablePullDown("tServer;cuServerPullDown","cLabel","cLabel",uServer,0);
//uServerGroup
	OpenRow(LANG_FL_tServerGroupGlue_uServerGroup,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,1);
	else
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,0);
	printf("</tr>\n");



}//void tServerGroupGlueInput(unsigned uMode)


void NewtServerGroupGlue(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uServerGroupGlue FROM tServerGroupGlue\
				WHERE uServerGroupGlue=%u"
							,uServerGroupGlue);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tServerGroupGlue("<blink>Record already exists");
		tServerGroupGlue(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tServerGroupGlue();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uServerGroupGlue=mysql_insert_id(&gMysql);

	unxsMailLog(uServerGroupGlue,"tServerGroupGlue","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uServerGroupGlue);
	tServerGroupGlue(gcQuery);
	}

}//NewtServerGroupGlue(unsigned uMode)


void DeletetServerGroupGlue(void)
{
	sprintf(gcQuery,"DELETE FROM tServerGroupGlue WHERE uServerGroupGlue=%u"
					,uServerGroupGlue);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tServerGroupGlue("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uServerGroupGlue,"tServerGroupGlue","Del");

		tServerGroupGlue(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uServerGroupGlue,"tServerGroupGlue","DelError");

		tServerGroupGlue(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetServerGroupGlue(void)


void Insert_tServerGroupGlue(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tServerGroupGlue SET uServerGroupGlue=%u,uServer=%u,uServerGroup=%u",
			uServerGroupGlue
			,uServer
			,uServerGroup
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tServerGroupGlue(void)


void Update_tServerGroupGlue(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tServerGroupGlue SET uServerGroupGlue=%u,uServer=%u,uServerGroup=%u WHERE _rowid=%s",
			uServerGroupGlue
			,uServer
			,uServerGroup
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tServerGroupGlue(void)


void ModtServerGroupGlue(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uServerGroupGlue FROM tServerGroupGlue\
				WHERE uServerGroupGlue=%u"
						,uServerGroupGlue);


	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tServerGroupGlue("<blink>Record does not exist");
	if(i<1) tServerGroupGlue(LANG_NBR_RECNOTEXIST);
	//if(i>1) tServerGroupGlue("<blink>Multiple rows!");
	if(i>1) tServerGroupGlue(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	Update_tServerGroupGlue(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);

	unxsMailLog(uServerGroupGlue,"tServerGroupGlue","Mod");

	tServerGroupGlue(gcQuery);

}//ModtServerGroupGlue(void)


void tServerGroupGlueList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttServerGroupGlueListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tServerGroupGlueList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttServerGroupGlueListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uServerGroupGlue<td><font face=arial,helvetica color=white>uServer<td><font face=arial,helvetica color=white>uServerGroup</tr>");



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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,ForeignKey("tServer","cLabel",strtoul(field[1],NULL,10))
			,ForeignKey("tServerGroup","cLabel",strtoul(field[2],NULL,10))
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tServerGroupGlueList()


void CreatetServerGroupGlue(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tServerGroupGlue ( uServer INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uServer), uServerGroup INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uServerGroup), uServerGroupGlue INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, UNIQUE (uServer,uServerGroup) )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetServerGroupGlue()

