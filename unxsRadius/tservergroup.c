/*
FILE
	tServerGroup source code of unxsRadius.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tservergroupfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uUser: Servers per uUser glue table
static unsigned uUser=0;
//uServer: Index into tServer
static unsigned uServer=0;



#define VAR_LIST_tServerGroup "tServerGroup.uUser,tServerGroup.uServer"

 //Local only
void Insert_tServerGroup(void);
void Update_tServerGroup(char *cRowid);
void ProcesstServerGroupListVars(pentry entries[], int x);

 //In tServerGroupfunc.h file included below
void ExtProcesstServerGroupVars(pentry entries[], int x);
void ExttServerGroupCommands(pentry entries[], int x);
void ExttServerGroupButtons(void);
void ExttServerGroupNavBar(void);
void ExttServerGroupGetHook(entry gentries[], int x);
void ExttServerGroupSelect(void);
void ExttServerGroupSelectRow(void);
void ExttServerGroupListSelect(void);
void ExttServerGroupListFilter(void);
void ExttServerGroupAuxTable(void);

#include "tservergroupfunc.h"

 //Table Variables Assignment Function
void ProcesstServerGroupVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uUser"))
			sscanf(entries[i].val,"%u",&uUser);
		else if(!strcmp(entries[i].name,"uServer"))
			sscanf(entries[i].val,"%u",&uServer);

	}

	//After so we can overwrite form data if needed.
	ExtProcesstServerGroupVars(entries,x);

}//ProcesstServerGroupVars()


void ProcesstServerGroupListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uUser);
                        guMode=2002;
                        tServerGroup("");
                }
        }
}//void ProcesstServerGroupListVars(pentry entries[], int x)


int tServerGroupCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttServerGroupCommands(entries,x);

	if(!strcmp(gcFunction,"tServerGroupTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tServerGroupList();
		}

		//Default
		ProcesstServerGroupVars(entries,x);
		tServerGroup("");
	}
	else if(!strcmp(gcFunction,"tServerGroupList"))
	{
		ProcessControlVars(entries,x);
		ProcesstServerGroupListVars(entries,x);
		tServerGroupList();
	}

	return(0);

}//tServerGroupCommands()


void tServerGroup(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttServerGroupSelectRow();
		else
			ExttServerGroupSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetServerGroup();
				unxsRadius("New tServerGroup table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tServerGroup WHERE uUser=%u"
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
		sscanf(field[1],"%u",&uServer);

		}

	}//Internal Skip

	Header_ism3(":: tServerGroup",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tServerGroupTools>");
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

        ExttServerGroupButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tServerGroup Record Data",100);

	if(guMode==2000 || guMode==2002)
		tServerGroupInput(1);
	else
		tServerGroupInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttServerGroupAuxTable();

	Footer_ism3();

}//end of tServerGroup();


void tServerGroupInput(unsigned uMode)
{

//uUser
	OpenRow(LANG_FL_tServerGroup_uUser,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uUser value=%u >\n",ForeignKey("tUser","cLogin",uUser),uUser);
	}
	else
	{
	printf("%s<input type=hidden name=uUser value=%u >\n",ForeignKey("tUser","cLogin",uUser),uUser);
	}
//uServer
	OpenRow(LANG_FL_tServerGroup_uServer,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uServer value=%u >\n",ForeignKey("tServer","cLabel",uServer),uServer);
	}
	else
	{
	printf("%s<input type=hidden name=uServer value=%u >\n",ForeignKey("tServer","cLabel",uServer),uServer);
	}
	printf("</tr>\n");



}//void tServerGroupInput(unsigned uMode)


void NewtServerGroup(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uUser FROM tServerGroup WHERE uUser=%u",uUser);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tServerGroup("<blink>Record already exists");
		tServerGroup(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tServerGroup();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uUser=mysql_insert_id(&gMysql);

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uUser);
		tServerGroup(gcQuery);
	}

}//NewtServerGroup(unsigned uMode)


void DeletetServerGroup(void)
{
	sprintf(gcQuery,"DELETE FROM tServerGroup WHERE uUser=%u",uUser);
	macro_mySQLQueryHTMLError;

	//tServerGroup("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
		tServerGroup(LANG_NBR_RECDELETED);
	else
		tServerGroup(LANG_NBR_RECNOTDELETED);

}//void DeletetServerGroup(void)


void Insert_tServerGroup(void)
{
	sprintf(gcQuery,"INSERT INTO tServerGroup SET uUser=%u,uServer=%u",uUser,uServer);
	macro_mySQLQueryHTMLError;

}//void Insert_tServerGroup(void)


void Update_tServerGroup(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tServerGroup SET uUser=%u,uServer=%u WHERE _rowid=%s",uUser,uServer,cRowid);
	macro_mySQLQueryHTMLError;

}//void Update_tServerGroup(void)


void ModtServerGroup(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uUser FROM tServerGroup WHERE uUser=%u",uUser);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tServerGroup("<blink>Record does not exist");
	if(i<1) tServerGroup(LANG_NBR_RECNOTEXIST);
	//if(i>1) tServerGroup("<blink>Multiple rows!");
	if(i>1) tServerGroup(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	Update_tServerGroup(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	tServerGroup(gcQuery);

}//ModtServerGroup(void)


void tServerGroupList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttServerGroupListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tServerGroupList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttServerGroupListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uUser<td><font face=arial,helvetica color=white>uServer</tr>");



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
		printf("<td>%s<td>%s</tr>"
			,ForeignKey("tUser","cLogin",strtoul(field[0],NULL,10))
			,ForeignKey("tServer","cLabel",strtoul(field[1],NULL,10))
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tServerGroupList()


void CreatetServerGroup(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tServerGroup ( uUser INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uUser), uServer INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uServer) )");
	macro_mySQLQueryHTMLError;
}//CreatetServerGroup()

