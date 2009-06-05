/*
FILE
	tGroupGlue source code of unxsVZ.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	$Id$
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tgroupgluefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uGroupGlue: Primary Key
static unsigned uGroupGlue=0;
//uGroup: Glue into tGroup
static unsigned uGroup=0;
static char cuGroupPullDown[256]={""};
//uNode: Glue into tNode
static unsigned uNode=0;
static char cuNodePullDown[256]={""};
//uContainer: Glue into tContainer
static unsigned uContainer=0;
static char cuContainerPullDown[256]={""};



#define VAR_LIST_tGroupGlue "tGroupGlue.uGroupGlue,tGroupGlue.uGroup,tGroupGlue.uNode,tGroupGlue.uContainer"

 //Local only
void Insert_tGroupGlue(void);
void Update_tGroupGlue(char *cRowid);
void ProcesstGroupGlueListVars(pentry entries[], int x);

 //In tGroupGluefunc.h file included below
void ExtProcesstGroupGlueVars(pentry entries[], int x);
void ExttGroupGlueCommands(pentry entries[], int x);
void ExttGroupGlueButtons(void);
void ExttGroupGlueNavBar(void);
void ExttGroupGlueGetHook(entry gentries[], int x);
void ExttGroupGlueSelect(void);
void ExttGroupGlueSelectRow(void);
void ExttGroupGlueListSelect(void);
void ExttGroupGlueListFilter(void);
void ExttGroupGlueAuxTable(void);

#include "tgroupgluefunc.h"

 //Table Variables Assignment Function
void ProcesstGroupGlueVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uGroupGlue"))
			sscanf(entries[i].val,"%u",&uGroupGlue);
		else if(!strcmp(entries[i].name,"uGroup"))
			sscanf(entries[i].val,"%u",&uGroup);
		else if(!strcmp(entries[i].name,"cuGroupPullDown"))
		{
			sprintf(cuGroupPullDown,"%.255s",entries[i].val);
			uGroup=ReadPullDown("tGroup","cLabel",cuGroupPullDown);
		}
		else if(!strcmp(entries[i].name,"uNode"))
			sscanf(entries[i].val,"%u",&uNode);
		else if(!strcmp(entries[i].name,"cuNodePullDown"))
		{
			sprintf(cuNodePullDown,"%.255s",entries[i].val);
			uNode=ReadPullDown("tNode","cLabel",cuNodePullDown);
		}
		else if(!strcmp(entries[i].name,"uContainer"))
			sscanf(entries[i].val,"%u",&uContainer);
		else if(!strcmp(entries[i].name,"cuContainerPullDown"))
		{
			sprintf(cuContainerPullDown,"%.255s",entries[i].val);
			uContainer=ReadPullDown("tContainer","cLabel",cuContainerPullDown);
		}

	}

	//After so we can overwrite form data if needed.
	ExtProcesstGroupGlueVars(entries,x);

}//ProcesstGroupGlueVars()


void ProcesstGroupGlueListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uGroupGlue);
                        guMode=2002;
                        tGroupGlue("");
                }
        }
}//void ProcesstGroupGlueListVars(pentry entries[], int x)


int tGroupGlueCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttGroupGlueCommands(entries,x);

	if(!strcmp(gcFunction,"tGroupGlueTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tGroupGlueList();
		}

		//Default
		ProcesstGroupGlueVars(entries,x);
		tGroupGlue("");
	}
	else if(!strcmp(gcFunction,"tGroupGlueList"))
	{
		ProcessControlVars(entries,x);
		ProcesstGroupGlueListVars(entries,x);
		tGroupGlueList();
	}

	return(0);

}//tGroupGlueCommands()


void tGroupGlue(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttGroupGlueSelectRow();
		else
			ExttGroupGlueSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetGroupGlue();
				unxsVZ("New tGroupGlue table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tGroupGlue WHERE uGroupGlue=%u"
						,uGroupGlue);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uGroupGlue);
		sscanf(field[1],"%u",&uGroup);
		sscanf(field[2],"%u",&uNode);
		sscanf(field[3],"%u",&uContainer);

		}

	}//Internal Skip

	Header_ism3(":: Glues uContainers or uNodes to uGroups",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tGroupGlueTools>");
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

        ExttGroupGlueButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tGroupGlue Record Data",100);

	if(guMode==2000 || guMode==2002)
		tGroupGlueInput(1);
	else
		tGroupGlueInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttGroupGlueAuxTable();

	Footer_ism3();

}//end of tGroupGlue();


void tGroupGlueInput(unsigned uMode)
{

//uGroupGlue
	OpenRow(LANG_FL_tGroupGlue_uGroupGlue,"black");
	printf("<input title='%s' type=text name=uGroupGlue value=%u size=16 maxlength=10 "
,LANG_FT_tGroupGlue_uGroupGlue,uGroupGlue);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uGroupGlue value=%u >\n",uGroupGlue);
	}
//uGroup
	OpenRow(LANG_FL_tGroupGlue_uGroup,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDownOwner("tGroup;cuGroupPullDown","cLabel","cLabel",uGroup,1);
	else
		tTablePullDownOwner("tGroup;cuGroupPullDown","cLabel","cLabel",uGroup,0);
//uNode
	OpenRow(LANG_FL_tGroupGlue_uNode,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tNode;cuNodePullDown","cLabel","cLabel",uNode,1);
	else
		tTablePullDown("tNode;cuNodePullDown","cLabel","cLabel",uNode,0);
//uContainer
	OpenRow(LANG_FL_tGroupGlue_uContainer,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDownOwner("tContainer;cuContainerPullDown","cLabel","cLabel",uContainer,1);
	else
		tTablePullDownOwner("tContainer;cuContainerPullDown","cLabel","cLabel",uContainer,0);
	printf("</tr>\n");



}//void tGroupGlueInput(unsigned uMode)


void NewtGroupGlue(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uGroupGlue FROM tGroupGlue\
				WHERE uGroupGlue=%u"
							,uGroupGlue);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tGroupGlue("<blink>Record already exists");
		tGroupGlue(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tGroupGlue();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uGroupGlue=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tGroupGlue",uGroupGlue);
	unxsVZLog(uGroupGlue,"tGroupGlue","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uGroupGlue);
	tGroupGlue(gcQuery);
	}

}//NewtGroupGlue(unsigned uMode)


void DeletetGroupGlue(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroupGlue=%u AND ( uOwner=%u OR %u>9 )"
					,uGroupGlue,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroupGlue=%u"
					,uGroupGlue);
#endif
	MYSQL_RUN;
	//tGroupGlue("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsVZLog(uGroupGlue,"tGroupGlue","Del");
#endif
		tGroupGlue(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsVZLog(uGroupGlue,"tGroupGlue","DelError");
#endif
		tGroupGlue(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetGroupGlue(void)


void Insert_tGroupGlue(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tGroupGlue SET uGroupGlue=%u,uGroup=%u,uNode=%u,uContainer=%u",
			uGroupGlue
			,uGroup
			,uNode
			,uContainer
			);

	MYSQL_RUN;

}//void Insert_tGroupGlue(void)


void Update_tGroupGlue(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tGroupGlue SET uGroupGlue=%u,uGroup=%u,uNode=%u,uContainer=%u WHERE _rowid=%s",
			uGroupGlue
			,uGroup
			,uNode
			,uContainer
			,cRowid);

	MYSQL_RUN;

}//void Update_tGroupGlue(void)


void ModtGroupGlue(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tGroupGlue.uGroupGlue,\
				tGroupGlue.uModDate\
				FROM tGroupGlue,tClient\
				WHERE tGroupGlue.uGroupGlue=%u\
				AND tGroupGlue.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uGroupGlue,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uGroupGlue,uModDate FROM tGroupGlue\
				WHERE uGroupGlue=%u"
						,uGroupGlue);
#else
	sprintf(gcQuery,"SELECT uGroupGlue FROM tGroupGlue\
				WHERE uGroupGlue=%u"
						,uGroupGlue);
#endif

	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tGroupGlue("<blink>Record does not exist");
	if(i<1) tGroupGlue(LANG_NBR_RECNOTEXIST);
	//if(i>1) tGroupGlue("<blink>Multiple rows!");
	if(i>1) tGroupGlue(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tGroupGlue(LANG_NBR_EXTMOD);
#endif

	Update_tGroupGlue(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tGroupGlue",uGroupGlue);
	unxsVZLog(uGroupGlue,"tGroupGlue","Mod");
#endif
	tGroupGlue(gcQuery);

}//ModtGroupGlue(void)


void tGroupGlueList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttGroupGlueListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tGroupGlueList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttGroupGlueListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uGroupGlue<td><font face=arial,helvetica color=white>uGroup<td><font face=arial,helvetica color=white>uNode<td><font face=arial,helvetica color=white>uContainer</tr>");



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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,ForeignKey("tGroup","cLabel",strtoul(field[1],NULL,10))
			,ForeignKey("tNode","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tContainer","cLabel",strtoul(field[3],NULL,10))
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tGroupGlueList()


void CreatetGroupGlue(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tGroupGlue ( uGroup INT UNSIGNED NOT NULL DEFAULT 0,index (uGroup), uNode INT UNSIGNED NOT NULL DEFAULT 0,index (uNode), uContainer INT UNSIGNED NOT NULL DEFAULT 0,index (uContainer), uGroupGlue INT UNSIGNED PRIMARY KEY AUTO_INCREMENT )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetGroupGlue()

