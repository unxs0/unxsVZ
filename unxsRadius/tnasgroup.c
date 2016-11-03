/*
FILE
	tNASGroup source code of unxsRadius.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tnasgroupfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uNAS: Servers per uNAS glue table
static unsigned uNAS=0;
//uServer: Index into tServer
static unsigned uServer=0;



#define VAR_LIST_tNASGroup "tNASGroup.uNAS,tNASGroup.uServer"

 //Local only
void Insert_tNASGroup(void);
void Update_tNASGroup(char *cRowid);
void ProcesstNASGroupListVars(pentry entries[], int x);

 //In tNASGroupfunc.h file included below
void ExtProcesstNASGroupVars(pentry entries[], int x);
void ExttNASGroupCommands(pentry entries[], int x);
void ExttNASGroupButtons(void);
void ExttNASGroupNavBar(void);
void ExttNASGroupGetHook(entry gentries[], int x);
void ExttNASGroupSelect(void);
void ExttNASGroupSelectRow(void);
void ExttNASGroupListSelect(void);
void ExttNASGroupListFilter(void);
void ExttNASGroupAuxTable(void);

#include "tnasgroupfunc.h"

 //Table Variables Assignment Function
void ProcesstNASGroupVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uNAS"))
			sscanf(entries[i].val,"%u",&uNAS);
		else if(!strcmp(entries[i].name,"uServer"))
			sscanf(entries[i].val,"%u",&uServer);

	}

	//After so we can overwrite form data if needed.
	ExtProcesstNASGroupVars(entries,x);

}//ProcesstNASGroupVars()


void ProcesstNASGroupListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uNAS);
                        guMode=2002;
                        tNASGroup("");
                }
        }
}//void ProcesstNASGroupListVars(pentry entries[], int x)


int tNASGroupCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttNASGroupCommands(entries,x);

	if(!strcmp(gcFunction,"tNASGroupTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tNASGroupList();
		}

		//Default
		ProcesstNASGroupVars(entries,x);
		tNASGroup("");
	}
	else if(!strcmp(gcFunction,"tNASGroupList"))
	{
		ProcessControlVars(entries,x);
		ProcesstNASGroupListVars(entries,x);
		tNASGroupList();
	}

	return(0);

}//tNASGroupCommands()


void tNASGroup(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttNASGroupSelectRow();
		else
			ExttNASGroupSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetNASGroup();
				unxsRadius("New tNASGroup table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tNASGroup WHERE uNAS=%u"
						,uNAS);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uNAS);
		sscanf(field[1],"%u",&uServer);

		}

	}//Internal Skip

	Header_ism3(":: tNASGroup",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tNASGroupTools>");
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

        ExttNASGroupButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tNASGroup Record Data",100);

	if(guMode==2000 || guMode==2002)
		tNASGroupInput(1);
	else
		tNASGroupInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttNASGroupAuxTable();

	Footer_ism3();

}//end of tNASGroup();


void tNASGroupInput(unsigned uMode)
{

//uNAS
	OpenRow(LANG_FL_tNASGroup_uNAS,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uNAS value=%u >\n",ForeignKey("tNAS","cLabel",uNAS),uNAS);
	}
	else
	{
	printf("%s<input type=hidden name=uNAS value=%u >\n",ForeignKey("tNAS","cLabel",uNAS),uNAS);
	}
//uServer
	OpenRow(LANG_FL_tNASGroup_uServer,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uServer value=%u >\n",ForeignKey("tServer","cLabel",uServer),uServer);
	}
	else
	{
	printf("%s<input type=hidden name=uServer value=%u >\n",ForeignKey("tServer","cLabel",uServer),uServer);
	}
	printf("</tr>\n");



}//void tNASGroupInput(unsigned uMode)


void NewtNASGroup(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uNAS FROM tNASGroup WHERE uNAS=%u",uNAS);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tNASGroup("<blink>Record already exists");
		tNASGroup(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tNASGroup();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uNAS=mysql_insert_id(&gMysql);

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uNAS);
		tNASGroup(gcQuery);
	}

}//NewtNASGroup(unsigned uMode)


void DeletetNASGroup(void)
{
	sprintf(gcQuery,"DELETE FROM tNASGroup WHERE uNAS=%u",uNAS);
	macro_mySQLQueryHTMLError;

	//tNASGroup("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
		tNASGroup(LANG_NBR_RECDELETED);
	else
		tNASGroup(LANG_NBR_RECNOTDELETED);
}//void DeletetNASGroup(void)


void Insert_tNASGroup(void)
{
	sprintf(gcQuery,"INSERT INTO tNASGroup SET uNAS=%u,uServer=%u",
			uNAS
			,uServer);
	macro_mySQLQueryHTMLError;

}//void Insert_tNASGroup(void)


void Update_tNASGroup(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tNASGroup SET uNAS=%u,uServer=%u WHERE _rowid=%s",
			uNAS
			,uServer
			,cRowid);
	macro_mySQLQueryHTMLError;
}//void Update_tNASGroup(void)


void ModtNASGroup(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT uNAS FROM tNASGroup WHERE uNAS=%u",uNAS);

	mysql_query(&gMysql,gcQuery);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tNASGroup("<blink>Record does not exist");
	if(i<1) tNASGroup(LANG_NBR_RECNOTEXIST);
	//if(i>1) tNASGroup("<blink>Multiple rows!");
	if(i>1) tNASGroup(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	Update_tNASGroup(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	tNASGroup(gcQuery);

}//ModtNASGroup(void)


void tNASGroupList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttNASGroupListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tNASGroupList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttNASGroupListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uNAS<td><font face=arial,helvetica color=white>uServer</tr>");



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
			,ForeignKey("tNAS","cLabel",strtoul(field[0],NULL,10))
			,ForeignKey("tServer","cLabel",strtoul(field[1],NULL,10))
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tNASGroupList()


void CreatetNASGroup(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tNASGroup ( uNAS INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uNAS), uServer INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uServer) )");
	macro_mySQLQueryHTMLError;
}//CreatetNASGroup()

