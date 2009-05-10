/*
FILE
	tBlock source code of iDNS.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	$Id: tblock.c 657 2008-12-18 15:56:33Z hus $
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tblockfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uBlock: Primary Key
static unsigned uBlock=0;
//cLabel: Short label
static char cLabel[33]={""};
//cComment: Short comment about this block
static char cComment[101]={""};
//uOwner: Record owner
static unsigned uOwner=0;
//uCreatedBy: uClient for last insert
static unsigned uCreatedBy=0;
#define ISM3FIELDS
//uCreatedDate: Unix seconds date last insert
static long uCreatedDate=0;
//uModBy: uClient for last update
static unsigned uModBy=0;
//uModDate: Unix seconds date last update
static long uModDate=0;



#define VAR_LIST_tBlock "tBlock.uBlock,tBlock.cLabel,tBlock.cComment,tBlock.uOwner,tBlock.uCreatedBy,tBlock.uCreatedDate,tBlock.uModBy,tBlock.uModDate"

 //Local only
void Insert_tBlock(void);
void Update_tBlock(char *cRowid);
void ProcesstBlockListVars(pentry entries[], int x);

 //In tBlockfunc.h file included below
void ExtProcesstBlockVars(pentry entries[], int x);
void ExttBlockCommands(pentry entries[], int x);
void ExttBlockButtons(void);
void ExttBlockNavBar(void);
void ExttBlockGetHook(entry gentries[], int x);
void ExttBlockSelect(void);
void ExttBlockSelectRow(void);
void ExttBlockListSelect(void);
void ExttBlockListFilter(void);
void ExttBlockAuxTable(void);

#include "tblockfunc.h"

 //Table Variables Assignment Function
void ProcesstBlockVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uBlock"))
			sscanf(entries[i].val,"%u",&uBlock);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cComment"))
			sprintf(cComment,"%.100s",entries[i].val);
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
	ExtProcesstBlockVars(entries,x);

}//ProcesstBlockVars()


void ProcesstBlockListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uBlock);
                        guMode=2002;
                        tBlock("");
                }
        }
}//void ProcesstBlockListVars(pentry entries[], int x)


int tBlockCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttBlockCommands(entries,x);

	if(!strcmp(gcFunction,"tBlockTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tBlockList();
		}

		//Default
		ProcesstBlockVars(entries,x);
		tBlock("");
	}
	else if(!strcmp(gcFunction,"tBlockList"))
	{
		ProcessControlVars(entries,x);
		ProcesstBlockListVars(entries,x);
		tBlockList();
	}

	return(0);

}//tBlockCommands()


void tBlock(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttBlockSelectRow();
		else
			ExttBlockSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetBlock();
				iDNS("New tBlock table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tBlock WHERE uBlock=%u"
						,uBlock);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uBlock);
		sprintf(cLabel,"%.32s",field[1]);
		sprintf(cComment,"%.100s",field[2]);
		sscanf(field[3],"%u",&uOwner);
		sscanf(field[4],"%u",&uCreatedBy);
		sscanf(field[5],"%lu",&uCreatedDate);
		sscanf(field[6],"%u",&uModBy);
		sscanf(field[7],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tBlock",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tBlockTools>");
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

        ExttBlockButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tBlock Record Data",100);

	if(guMode==2000 || guMode==2002)
		tBlockInput(1);
	else
		tBlockInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttBlockAuxTable();

	Footer_ism3();

}//end of tBlock();


void tBlockInput(unsigned uMode)
{

//uBlock
	OpenRow(LANG_FL_tBlock_uBlock,"black");
	printf("<input title='%s' type=text name=uBlock value=%u size=16 maxlength=10 "
,LANG_FT_tBlock_uBlock,uBlock);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uBlock value=%u >\n",uBlock);
	}
//cLabel
	OpenRow(LANG_FL_tBlock_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tBlock_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tBlock_cComment,"black");
	printf("<input title='%s' type=text name=cComment value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tBlock_cComment,EncodeDoubleQuotes(cComment));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cComment value=\"%s\">\n",EncodeDoubleQuotes(cComment));
	}
//uOwner
	OpenRow(LANG_FL_tBlock_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tBlock_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tBlock_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tBlock_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tBlock_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tBlockInput(unsigned uMode)


void NewtBlock(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uBlock FROM tBlock\
				WHERE uBlock=%u"
							,uBlock);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tBlock("<blink>Record already exists");
		tBlock(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tBlock();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uBlock=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tBlock",uBlock);
	iDNSLog(uBlock,"tBlock","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uBlock);
	tBlock(gcQuery);
	}

}//NewtBlock(unsigned uMode)


void DeletetBlock(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tBlock WHERE uBlock=%u AND ( uOwner=%u OR %u>9 )"
					,uBlock,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tBlock WHERE uBlock=%u"
					,uBlock);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tBlock("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		iDNSLog(uBlock,"tBlock","Del");
#endif
		tBlock(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		iDNSLog(uBlock,"tBlock","DelError");
#endif
		tBlock(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetBlock(void)


void Insert_tBlock(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tBlock SET uBlock=%u,cLabel='%s',cComment='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uBlock
			,TextAreaSave(cLabel)
			,TextAreaSave(cComment)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tBlock(void)


void Update_tBlock(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tBlock SET uBlock=%u,cLabel='%s',cComment='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uBlock
			,TextAreaSave(cLabel)
			,TextAreaSave(cComment)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tBlock(void)


void ModtBlock(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;
	
	sprintf(gcQuery,"SELECT uBlock,uModDate FROM tBlock WHERE uBlock=%u"
			,uBlock);
#else
	sprintf(gcQuery,"SELECT uBlock FROM tBlock WHERE uBlock=%u"
			,uBlock);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tBlock("<blink>Record does not exist");
	if(i<1) tBlock(LANG_NBR_RECNOTEXIST);
	//if(i>1) tBlock("<blink>Multiple rows!");
	if(i>1) tBlock(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tBlock(LANG_NBR_EXTMOD);
#endif

	Update_tBlock(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tBlock",uBlock);
	iDNSLog(uBlock,"tBlock","Mod");
#endif
	tBlock(gcQuery);

}//ModtBlock(void)


void tBlockList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttBlockListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tBlockList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttBlockListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uBlock<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>cComment<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		long luTime5=strtoul(field[5],NULL,10);
		char cBuf5[32];
		if(luTime5)
			ctime_r(&luTime5,cBuf5);
		else
			sprintf(cBuf5,"---");
		long luTime7=strtoul(field[7],NULL,10);
		char cBuf7[32];
		if(luTime7)
			ctime_r(&luTime7,cBuf7);
		else
			sprintf(cBuf7,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[3],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[4],NULL,10))
			,cBuf5
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,cBuf7
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tBlockList()


void CreatetBlock(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tBlock ( uBlock INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cComment VARCHAR(100) NOT NULL DEFAULT '' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetBlock()

