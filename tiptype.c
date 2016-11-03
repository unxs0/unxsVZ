/*
FILE
	tIPType source code of unxsVZ.cgi
	svn ID removed
PURPOSE
	IP number type for fine grained classification.
AUTHOR/LEGAL
	(C) 2001-2016 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
*/
//git describe version info
static char *cGitVersion="GitVersion:"GitVersion;


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uIPType: Primary Key
static unsigned uIPType=0;
//cLabel: Short label
static char cLabel[128]={""};
//cComment: Definition of the label referenced in cLabel
static char *cComment={""};
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



#define VAR_LIST_tIPType "tIPType.uIPType,tIPType.cLabel,tIPType.cComment,tIPType.uOwner,tIPType.uCreatedBy,tIPType.uCreatedDate,tIPType.uModBy,tIPType.uModDate"

 //Local only
void Insert_tIPType(void);
void Update_tIPType(char *cRowid);
void ProcesstIPTypeListVars(pentry entries[], int x);

 //In tIPTypefunc.h file included below
void ExtProcesstIPTypeVars(pentry entries[], int x);
void ExttIPTypeCommands(pentry entries[], int x);
void ExttIPTypeButtons(void);
void ExttIPTypeNavBar(void);
void ExttIPTypeGetHook(entry gentries[], int x);
void ExttIPTypeSelect(void);
void ExttIPTypeSelectRow(void);
void ExttIPTypeListSelect(void);
void ExttIPTypeListFilter(void);
void ExttIPTypeAuxTable(void);

#include "tiptypefunc.h"

 //Table Variables Assignment Function
void ProcesstIPTypeVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uIPType"))
			sscanf(entries[i].val,"%u",&uIPType);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.127s",entries[i].val);
		else if(!strcmp(entries[i].name,"cComment"))
			cComment=entries[i].val;
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
	ExtProcesstIPTypeVars(entries,x);

}//ProcesstIPTypeVars()


void ProcesstIPTypeListVars(pentry entries[], int x)
{
	/*
        register int i;

        for(i=0;i<x;i++)
        {
        }
	*/
}//void ProcesstIPTypeListVars(pentry entries[], int x)


int tIPTypeCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttIPTypeCommands(entries,x);

	if(!strcmp(gcFunction,"tIPTypeTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tIPTypeList();
		}

		//Default
		ProcesstIPTypeVars(entries,x);
		tIPType("");
	}
	else if(!strcmp(gcFunction,"tIPTypeList"))
	{
		ProcessControlVars(entries,x);
		ProcesstIPTypeListVars(entries,x);
		tIPTypeList();
	}

	return(0);

}//tIPTypeCommands()


void tIPType(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttIPTypeSelectRow();
		else
			ExttIPTypeSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetIPType();
				unxsVZ("New tIPType table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tIPType WHERE uIPType=%u"
						,uIPType);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uIPType);
		sprintf(cLabel,"%.127s",field[1]);
		cComment=field[2];
		sscanf(field[3],"%u",&uOwner);
		sscanf(field[4],"%u",&uCreatedBy);
		sscanf(field[5],"%lu",&uCreatedDate);
		sscanf(field[6],"%u",&uModBy);
		sscanf(field[7],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tIPType",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tIPTypeTools>");
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

        ExttIPTypeButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tIPType Record Data",100);

	if(guMode==2000 || guMode==2002)
		tIPTypeInput(1);
	else
		tIPTypeInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttIPTypeAuxTable();

	Footer_ism3();

}//end of tIPType();


void tIPTypeInput(unsigned uMode)
{

	//uIPType
	OpenRow(LANG_FL_tIPType_uIPType,"black");
	printf("<input title='%s' type=text name=uIPType value=%u size=16 maxlength=10 ",LANG_FT_tIPType_uIPType,uIPType);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uIPType value=%u >\n",uIPType);
	}
	//cLabel
	OpenRow(LANG_FL_tIPType_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=80 maxlength=127 ",LANG_FT_tIPType_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
	//cComment
	OpenRow(LANG_FL_tIPType_cComment,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cComment ",LANG_FT_tIPType_cComment);
	if(guPermLevel>=7 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cComment);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cComment);
		printf("<input type=hidden name=cComment value=\"%s\" >\n",EncodeDoubleQuotes(cComment));
	}
	//uOwner
	OpenRow(LANG_FL_tIPType_uOwner,"black");
	if(guPermLevel>=20 && uMode)
		printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	else
		printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	//uCreatedBy
	OpenRow(LANG_FL_tIPType_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
		printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	else
		printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	//uCreatedDate
	OpenRow(LANG_FL_tIPType_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
	//uModBy
	OpenRow(LANG_FL_tIPType_uModBy,"black");
	if(guPermLevel>=20 && uMode)
		printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	else
		printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	//uModDate
	OpenRow(LANG_FL_tIPType_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");

}//void tIPTypeInput(unsigned uMode)


void NewtIPType(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uIPType FROM tIPType WHERE uIPType=%u",uIPType);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tIPType("<blink>Record already exists");
		tIPType(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tIPType();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uIPType=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tIPType",uIPType);
	unxsVZLog(uIPType,"tIPType","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uIPType);
		tIPType(gcQuery);
	}

}//NewtIPType(unsigned uMode)


void DeletetIPType(void)
{
	sprintf(gcQuery,"DELETE FROM tIPType WHERE uIPType=%u AND ( uOwner=%u OR %u>9 )"
					,uIPType,guLoginClient,guPermLevel);
	MYSQL_RUN;
	//tIPType("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsVZLog(uIPType,"tIPType","Del");
		tIPType(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsVZLog(uIPType,"tIPType","DelError");
		tIPType(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetIPType(void)


void Insert_tIPType(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tIPType SET uIPType=%u,cLabel='%s',cComment='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uIPType
			,TextAreaSave(cLabel)
			,TextAreaSave(cComment)
			,uOwner
			,uCreatedBy
			);

	MYSQL_RUN;
	
}//void Insert_tIPType(void)


void Update_tIPType(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tIPType SET uIPType=%u,cLabel='%s',cComment='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uIPType
			,TextAreaSave(cLabel)
			,TextAreaSave(cComment)
			,uModBy
			,cRowid);

	MYSQL_RUN;

}//void Update_tIPType(void)


void ModtIPType(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tIPType.uIPType,\
				tIPType.uModDate\
				FROM tIPType,tClient\
				WHERE tIPType.uIPType=%u\
				AND tIPType.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uIPType,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uIPType,uModDate FROM tIPType\
				WHERE uIPType=%u"
						,uIPType);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tIPType("<blink>Record does not exist");
	if(i<1) tIPType(LANG_NBR_RECNOTEXIST);
	//if(i>1) tIPType("<blink>Multiple rows!");
	if(i>1) tIPType(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tIPType(LANG_NBR_EXTMOD);

	Update_tIPType(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tIPType",uIPType);
	unxsVZLog(uIPType,"tIPType","Mod");
	tIPType(gcQuery);

}//ModtIPType(void)


void tIPTypeList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttIPTypeListSelect();
	
	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tIPTypeList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttIPTypeListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
			"<td><font face=arial,helvetica color=white>uIPType"
			"<td><font face=arial,helvetica color=white>cLabel"
			"<td><font face=arial,helvetica color=white>cComment"
			"<td><font face=arial,helvetica color=white>uOwner"
			"<td><font face=arial,helvetica color=white>uCreatedBy"
			"<td><font face=arial,helvetica color=white>uCreatedDate"
			"<td><font face=arial,helvetica color=white>uModBy"
			"<td><font face=arial,helvetica color=white>uModDate</tr>");

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
		printf("<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tIPType&uIPType=%s>%s</a>"
			,field[0]
			,field[0]);
		printf("<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[1]
			,field[2]
			,ForeignKey("tClient","cLabel",strtoul(field[3],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[4],NULL,10))
			,cBuf5
			,ForeignKey("tClient","cLabel",strtoul(field[6],NULL,10))
			,cBuf7
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tIPTypeList()


void CreatetIPType(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tIPType ("
				" uIPType INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
				" cLabel VARCHAR(128) NOT NULL DEFAULT '',"
				" uOwner INT UNSIGNED NOT NULL DEFAULT 0,INDEX (uOwner),"
				" uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
				" uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
				" uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
				" uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
				" cComment TEXT NOT NULL DEFAULT '' )");
	MYSQL_RUN;

}//CreatetIPType()

