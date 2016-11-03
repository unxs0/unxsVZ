/*
FILE
	tVUTEntries source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tvutentriesfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uVUTEntries: Primary Key
static unsigned uVUTEntries=0;
//uVUT: VUT domain
static unsigned uVUT=0;
//cVirtualEmail: Virtual email address no @FQDN
static char cVirtualEmail[33]={""};
//cTargetEmail: Target email address with @FQDN
static char cTargetEmail[101]={""};
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



#define VAR_LIST_tVUTEntries "tVUTEntries.uVUTEntries,tVUTEntries.uVUT,tVUTEntries.cVirtualEmail,tVUTEntries.cTargetEmail,tVUTEntries.uClient,tVUTEntries.uOwner,tVUTEntries.uCreatedBy,tVUTEntries.uCreatedDate,tVUTEntries.uModBy,tVUTEntries.uModDate"

 //Local only
void Insert_tVUTEntries(void);
void Update_tVUTEntries(char *cRowid);
void ProcesstVUTEntriesListVars(pentry entries[], int x);

 //In tVUTEntriesfunc.h file included below
void ExtProcesstVUTEntriesVars(pentry entries[], int x);
void ExttVUTEntriesCommands(pentry entries[], int x);
void ExttVUTEntriesButtons(void);
void ExttVUTEntriesNavBar(void);
void ExttVUTEntriesGetHook(entry gentries[], int x);
void ExttVUTEntriesSelect(void);
void ExttVUTEntriesSelectRow(void);
void ExttVUTEntriesListSelect(void);
void ExttVUTEntriesListFilter(void);
void ExttVUTEntriesAuxTable(void);

#include "tvutentriesfunc.h"

 //Table Variables Assignment Function
void ProcesstVUTEntriesVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uVUTEntries"))
			sscanf(entries[i].val,"%u",&uVUTEntries);
		else if(!strcmp(entries[i].name,"uVUT"))
			sscanf(entries[i].val,"%u",&uVUT);
		else if(!strcmp(entries[i].name,"cVirtualEmail"))
		{
			if(strncmp(entries[i].val,"DEFAULT",7))
				sprintf(cVirtualEmail,"%.32s",WordToLower(entries[i].val));
			else
				sprintf(cVirtualEmail,"DEFAULT");
		}
		else if(!strcmp(entries[i].name,"cTargetEmail"))
			sprintf(cTargetEmail,"%.100s",entries[i].val);
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
	ExtProcesstVUTEntriesVars(entries,x);

}//ProcesstVUTEntriesVars()


void ProcesstVUTEntriesListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uVUTEntries);
                        guMode=2002;
                        tVUTEntries("");
                }
        }
}//void ProcesstVUTEntriesListVars(pentry entries[], int x)


int tVUTEntriesCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttVUTEntriesCommands(entries,x);

	if(!strcmp(gcFunction,"tVUTEntriesTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tVUTEntriesList();
		}

		//Default
		ProcesstVUTEntriesVars(entries,x);
		tVUTEntries("");
	}
	else if(!strcmp(gcFunction,"tVUTEntriesList"))
	{
		ProcessControlVars(entries,x);
		ProcesstVUTEntriesListVars(entries,x);
		tVUTEntriesList();
	}

	return(0);

}//tVUTEntriesCommands()


void tVUTEntries(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttVUTEntriesSelectRow();
		else
			ExttVUTEntriesSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetVUTEntries();
				unxsMail("New tVUTEntries table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tVUTEntries WHERE uVUTEntries=%u"
						,uVUTEntries);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uVUTEntries);
		sscanf(field[1],"%u",&uVUT);
		sprintf(cVirtualEmail,"%.32s",field[2]);
		sprintf(cTargetEmail,"%.100s",field[3]);
		sscanf(field[4],"%u",&uClient);
		sscanf(field[5],"%u",&uOwner);
		sscanf(field[6],"%u",&uCreatedBy);
		sscanf(field[7],"%lu",&uCreatedDate);
		sscanf(field[8],"%u",&uModBy);
		sscanf(field[9],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tVUTEntries",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tVUTEntriesTools>");
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

        ExttVUTEntriesButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tVUTEntries Record Data",100);

	if(guMode==2000 || guMode==2002)
		tVUTEntriesInput(1);
	else
		tVUTEntriesInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttVUTEntriesAuxTable();

	Footer_ism3();

}//end of tVUTEntries();


void tVUTEntriesInput(unsigned uMode)
{

//uVUTEntries
	OpenRow(LANG_FL_tVUTEntries_uVUTEntries,"black");
	printf("<input title='%s' type=text name=uVUTEntries value=%u size=16 maxlength=10 "
,LANG_FT_tVUTEntries_uVUTEntries,uVUTEntries);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uVUTEntries value=%u >\n",uVUTEntries);
	}
//uVUT
	OpenRow(LANG_FL_tVUTEntries_uVUT,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uVUT value=%u >\n",ForeignKey("tVUT","cDomain",uVUT),uVUT);
	}
	else
	{
	printf("%s<input type=hidden name=uVUT value=%u >\n",ForeignKey("tVUT","cDomain",uVUT),uVUT);
	}
//cVirtualEmail
	OpenRow(LANG_FL_tVUTEntries_cVirtualEmail,"black");
	printf("<input title='%s' type=text name=cVirtualEmail value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tVUTEntries_cVirtualEmail,EncodeDoubleQuotes(cVirtualEmail));
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cVirtualEmail value=\"%s\">\n",EncodeDoubleQuotes(cVirtualEmail));
	}
//cTargetEmail
	OpenRow(LANG_FL_tVUTEntries_cTargetEmail,"black");
	printf("<input title='%s' type=text name=cTargetEmail value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tVUTEntries_cTargetEmail,EncodeDoubleQuotes(cTargetEmail));
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cTargetEmail value=\"%s\">\n",EncodeDoubleQuotes(cTargetEmail));
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
	OpenRow(LANG_FL_tVUTEntries_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tVUTEntries_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tVUTEntries_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tVUTEntries_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tVUTEntries_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tVUTEntriesInput(unsigned uMode)


void NewtVUTEntries(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uVUTEntries FROM tVUTEntries\
				WHERE uVUTEntries=%u"
							,uVUTEntries);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tVUTEntries("<blink>Record already exists");
		tVUTEntries(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tVUTEntries();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uVUTEntries=mysql_insert_id(&gMysql);

	uCreatedDate=luGetCreatedDate("tVUTEntries",uVUTEntries);
	unxsMailLog(uVUTEntries,"tVUTEntries","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uVUTEntries);
	tVUTEntries(gcQuery);
	}

}//NewtVUTEntries(unsigned uMode)


void DeletetVUTEntries(void)
{

	sprintf(gcQuery,"DELETE FROM tVUTEntries WHERE uVUTEntries=%u AND ( uOwner=%u OR %u>9 )"
					,uVUTEntries,guLoginClient,guPermLevel);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tVUTEntries("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uVUTEntries,"tVUTEntries","Del");

		tVUTEntries(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uVUTEntries,"tVUTEntries","DelError");

		tVUTEntries(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetVUTEntries(void)


void Insert_tVUTEntries(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tVUTEntries SET uVUTEntries=%u,uVUT=%u,cVirtualEmail='%s',cTargetEmail='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uVUTEntries
			,uVUT
			,TextAreaSave(cVirtualEmail)
			,TextAreaSave(cTargetEmail)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tVUTEntries(void)


void Update_tVUTEntries(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tVUTEntries SET uVUTEntries=%u,uVUT=%u,cVirtualEmail='%s',cTargetEmail='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uVUTEntries
			,uVUT
			,TextAreaSave(cVirtualEmail)
			,TextAreaSave(cTargetEmail)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tVUTEntries(void)


void ModtVUTEntries(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uVUTEntries,uModDate FROM tVUTEntries WHERE uVUTEntries=%u"
						,uVUTEntries);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tVUTEntries("<blink>Record does not exist");
	if(i<1) tVUTEntries(LANG_NBR_RECNOTEXIST);
	//if(i>1) tVUTEntries("<blink>Multiple rows!");
	if(i>1) tVUTEntries(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tVUTEntries(LANG_NBR_EXTMOD);


	Update_tVUTEntries(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);

	uModDate=luGetModDate("tVUTEntries",uVUTEntries);
	unxsMailLog(uVUTEntries,"tVUTEntries","Mod");

	tVUTEntries(gcQuery);

}//ModtVUTEntries(void)


void tVUTEntriesList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttVUTEntriesListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tVUTEntriesList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttVUTEntriesListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uVUTEntries<td><font face=arial,helvetica color=white>uVUT<td><font face=arial,helvetica color=white>cVirtualEmail<td><font face=arial,helvetica color=white>cTargetEmail<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime7=strtoul(field[7],NULL,10);
		char cBuf7[32];
		if(luTime7)
			ctime_r(&luTime7,cBuf7);
		else
			sprintf(cBuf7,"---");
		time_t luTime9=strtoul(field[9],NULL,10);
		char cBuf9[32];
		if(luTime9)
			ctime_r(&luTime9,cBuf9);
		else
			sprintf(cBuf9,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,ForeignKey("tVUT","cDomain",strtoul(field[1],NULL,10))
			,field[2]
			,field[3]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,cBuf7
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[8],NULL,10))
			,cBuf9
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tVUTEntriesList()


void CreatetVUTEntries(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tVUTEntries ( uVUTEntries INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cVirtualEmail VARCHAR(32) NOT NULL DEFAULT '',index (cVirtualEmail,uVUT), uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cTargetEmail VARCHAR(100) NOT NULL DEFAULT '', uVUT INT UNSIGNED NOT NULL DEFAULT 0,index (uVUT), uClient INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetVUTEntries()

