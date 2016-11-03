/*
FILE
	tVUT source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tvutfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uVUT: Primary Key
static unsigned uVUT=0;
//cDomain: FQ Domain Name
static char cDomain[65]={""};
//uServerGroup: ServerGroup account is on
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



#define VAR_LIST_tVUT "tVUT.uVUT,tVUT.cDomain,tVUT.uServerGroup,tVUT.uOwner,tVUT.uCreatedBy,tVUT.uCreatedDate,tVUT.uModBy,tVUT.uModDate"

 //Local only
void Insert_tVUT(void);
void Update_tVUT(char *cRowid);
void ProcesstVUTListVars(pentry entries[], int x);

 //In tVUTfunc.h file included below
void ExtProcesstVUTVars(pentry entries[], int x);
void ExttVUTCommands(pentry entries[], int x);
void ExttVUTButtons(void);
void ExttVUTNavBar(void);
void ExttVUTGetHook(entry gentries[], int x);
void ExttVUTSelect(void);
void ExttVUTSelectRow(void);
void ExttVUTListSelect(void);
void ExttVUTListFilter(void);
void ExttVUTAuxTable(void);

#include "tvutfunc.h"

 //Table Variables Assignment Function
void ProcesstVUTVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uVUT"))
			sscanf(entries[i].val,"%u",&uVUT);
		else if(!strcmp(entries[i].name,"cDomain"))
			sprintf(cDomain,"%.64s",FQDomainName(entries[i].val));
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
	ExtProcesstVUTVars(entries,x);

}//ProcesstVUTVars()


void ProcesstVUTListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uVUT);
                        guMode=2002;
                        tVUT("");
                }
        }
}//void ProcesstVUTListVars(pentry entries[], int x)


int tVUTCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttVUTCommands(entries,x);

	if(!strcmp(gcFunction,"tVUTTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tVUTList();
		}

		//Default
		ProcesstVUTVars(entries,x);
		tVUT("");
	}
	else if(!strcmp(gcFunction,"tVUTList"))
	{
		ProcessControlVars(entries,x);
		ProcesstVUTListVars(entries,x);
		tVUTList();
	}

	return(0);

}//tVUTCommands()


void tVUT(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttVUTSelectRow();
		else
			ExttVUTSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetVUT();
				unxsMail("New tVUT table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tVUT WHERE uVUT=%u"
						,uVUT);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uVUT);
		sprintf(cDomain,"%.64s",field[1]);
		sscanf(field[2],"%u",&uServerGroup);
		sscanf(field[3],"%u",&uOwner);
		sscanf(field[4],"%u",&uCreatedBy);
		sscanf(field[5],"%lu",&uCreatedDate);
		sscanf(field[6],"%u",&uModBy);
		sscanf(field[7],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tVUT",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tVUTTools>");
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

        ExttVUTButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tVUT Record Data",100);

	if(guMode==2000 || guMode==2002)
		tVUTInput(1);
	else
		tVUTInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttVUTAuxTable();

	Footer_ism3();

}//end of tVUT();


void tVUTInput(unsigned uMode)
{

//uVUT
	OpenRow(LANG_FL_tVUT_uVUT,"black");
	printf("<input title='%s' type=text name=uVUT value=%u size=16 maxlength=10 "
,LANG_FT_tVUT_uVUT,uVUT);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uVUT value=%u >\n",uVUT);
	}
//cDomain
	OpenRow(LANG_FL_tVUT_cDomain,"black");
	printf("<input title='%s' type=text name=cDomain value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tVUT_cDomain,EncodeDoubleQuotes(cDomain));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cDomain value=\"%s\">\n",EncodeDoubleQuotes(cDomain));
	}
//uServerGroup
	OpenRow(LANG_FL_tVUT_uServerGroup,"black");
	if(guPermLevel>=8 && uMode)
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,1);
	else
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,0);
//uOwner
	OpenRow(LANG_FL_tVUT_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tVUT_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tVUT_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tVUT_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tVUT_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tVUTInput(unsigned uMode)


void NewtVUT(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uVUT FROM tVUT\
				WHERE uVUT=%u"
							,uVUT);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tVUT("<blink>Record already exists");
		tVUT(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tVUT();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uVUT=mysql_insert_id(&gMysql);

	uCreatedDate=luGetCreatedDate("tVUT",uVUT);
	unxsMailLog(uVUT,"tVUT","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uVUT);
	tVUT(gcQuery);
	}

}//NewtVUT(unsigned uMode)


void DeletetVUT(void)
{

	sprintf(gcQuery,"DELETE FROM tVUT WHERE uVUT=%u AND ( uOwner=%u OR %u>9 )"
					,uVUT,guLoginClient,guPermLevel);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tVUT("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uVUT,"tVUT","Del");

		tVUT(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uVUT,"tVUT","DelError");

		tVUT(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetVUT(void)


void Insert_tVUT(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tVUT SET uVUT=%u,cDomain='%s',uServerGroup=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uVUT
			,TextAreaSave(cDomain)
			,uServerGroup
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tVUT(void)


void Update_tVUT(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tVUT SET uVUT=%u,cDomain='%s',uServerGroup=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uVUT
			,TextAreaSave(cDomain)
			,uServerGroup
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tVUT(void)


void ModtVUT(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uVUT,uModDate FROM tVUT WHERE uVUT=%u"
						,uVUT);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tVUT("<blink>Record does not exist");
	if(i<1) tVUT(LANG_NBR_RECNOTEXIST);
	//if(i>1) tVUT("<blink>Multiple rows!");
	if(i>1) tVUT(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tVUT(LANG_NBR_EXTMOD);


	Update_tVUT(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);

	uModDate=luGetModDate("tVUT",uVUT);
	unxsMailLog(uVUT,"tVUT","Mod");

	tVUT(gcQuery);

}//ModtVUT(void)


void tVUTList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttVUTListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tVUTList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttVUTListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uVUT<td><font face=arial,helvetica color=white>cDomain<td><font face=arial,helvetica color=white>uServerGroup<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime5=strtoul(field[5],NULL,10);
		char cBuf5[32];
		if(luTime5)
			ctime_r(&luTime5,cBuf5);
		else
			sprintf(cBuf5,"---");
		time_t luTime7=strtoul(field[7],NULL,10);
		char cBuf7[32];
		if(luTime7)
			ctime_r(&luTime7,cBuf7);
		else
			sprintf(cBuf7,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey("tServerGroup","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[3],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[4],NULL,10))
			,cBuf5
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,cBuf7
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tVUTList()


void CreatetVUT(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tVUT ( uVUT INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uServerGroup INT UNSIGNED NOT NULL DEFAULT 0,index (uServerGroup), cDomain VARCHAR(64) NOT NULL DEFAULT '',index (cDomain,uServerGroup) )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetVUT()

