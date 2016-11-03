/*
FILE
	tContent source code of unxsBlog.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tcontentfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uContent: Primary Key
static unsigned uContent=0;
//uMonth: Month of publication
static unsigned uMonth=0;
//uYear: Year of publication
static unsigned uYear=0;
//uFeatured: Featured article
static unsigned uFeatured=0;
static char cYesNouFeatured[32]={""};
//uApproved: Approved article
static unsigned uApproved=0;
static char cYesNouApproved[32]={""};
//cTitle: Article title
static char cTitle[101]={""};
//uAllowSearch: Allow in search results
static unsigned uAllowSearch=0;
static char cYesNouAllowSearch[32]={""};
//uOrder: Order in table of contents
static unsigned uOrder=0;
//uContentType: Article content type
static unsigned uContentType=0;
static char cuContentTypePullDown[256]={""};
//cImage: Article image(s) seperated by ;
static char cImage[256]={""};
//cComment: Comment by editor
static char cComment[101]={""};
//cText: Article text
static char *cText={""};
//uOwner: Record owner
static unsigned uOwner=0;
//uCreatedBy: uClient for last insert
static unsigned uCreatedBy=0;
#define ISM3FIELDS
//uCreatedDate: Unix seconds date last insert
static time_t uCreatedDate=0;
//uModBy: uClient for last update
static unsigned uModBy=0;
//uModDate: Unix seconds date last update
static time_t uModDate=0;



#define VAR_LIST_tContent "tContent.uContent,tContent.uMonth,tContent.uYear,tContent.uFeatured,tContent.uApproved,tContent.cTitle,tContent.uAllowSearch,tContent.uOrder,tContent.uContentType,tContent.cImage,tContent.cComment,tContent.cText,tContent.uOwner,tContent.uCreatedBy,tContent.uCreatedDate,tContent.uModBy,tContent.uModDate"

 //Local only
void Insert_tContent(void);
void Update_tContent(char *cRowid);
void ProcesstContentListVars(pentry entries[], int x);

 //In tContentfunc.h file included below
void ExtProcesstContentVars(pentry entries[], int x);
void ExttContentCommands(pentry entries[], int x);
void ExttContentButtons(void);
void ExttContentNavBar(void);
void ExttContentGetHook(entry gentries[], int x);
void ExttContentSelect(void);
void ExttContentSelectRow(void);
void ExttContentListSelect(void);
void ExttContentListFilter(void);
void ExttContentAuxTable(void);

#include "tcontentfunc.h"

 //Table Variables Assignment Function
void ProcesstContentVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uContent"))
			sscanf(entries[i].val,"%u",&uContent);
		else if(!strcmp(entries[i].name,"uMonth"))
			sscanf(entries[i].val,"%u",&uMonth);
		else if(!strcmp(entries[i].name,"uYear"))
			sscanf(entries[i].val,"%u",&uYear);
		else if(!strcmp(entries[i].name,"uFeatured"))
			sscanf(entries[i].val,"%u",&uFeatured);
		else if(!strcmp(entries[i].name,"cYesNouFeatured"))
		{
			sprintf(cYesNouFeatured,"%.31s",entries[i].val);
			uFeatured=ReadYesNoPullDown(cYesNouFeatured);
		}
		else if(!strcmp(entries[i].name,"uApproved"))
			sscanf(entries[i].val,"%u",&uApproved);
		else if(!strcmp(entries[i].name,"cYesNouApproved"))
		{
			sprintf(cYesNouApproved,"%.31s",entries[i].val);
			uApproved=ReadYesNoPullDown(cYesNouApproved);
		}
		else if(!strcmp(entries[i].name,"cTitle"))
			sprintf(cTitle,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"uAllowSearch"))
			sscanf(entries[i].val,"%u",&uAllowSearch);
		else if(!strcmp(entries[i].name,"cYesNouAllowSearch"))
		{
			sprintf(cYesNouAllowSearch,"%.31s",entries[i].val);
			uAllowSearch=ReadYesNoPullDown(cYesNouAllowSearch);
		}
		else if(!strcmp(entries[i].name,"uOrder"))
			sscanf(entries[i].val,"%u",&uOrder);
		else if(!strcmp(entries[i].name,"uContentType"))
			sscanf(entries[i].val,"%u",&uContentType);
		else if(!strcmp(entries[i].name,"cuContentTypePullDown"))
		{
			sprintf(cuContentTypePullDown,"%.255s",entries[i].val);
			uContentType=ReadPullDown("tContentType","cLabel",cuContentTypePullDown);
		}
		else if(!strcmp(entries[i].name,"cImage"))
			sprintf(cImage,"%.255s",entries[i].val);
		else if(!strcmp(entries[i].name,"cComment"))
			sprintf(cComment,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cText"))
			cText=entries[i].val;
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
	ExtProcesstContentVars(entries,x);

}//ProcesstContentVars()


void ProcesstContentListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uContent);
                        guMode=2002;
                        tContent("");
                }
        }
}//void ProcesstContentListVars(pentry entries[], int x)


int tContentCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttContentCommands(entries,x);

	if(!strcmp(gcFunction,"tContentTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tContentList();
		}

		//Default
		ProcesstContentVars(entries,x);
		tContent("");
	}
	else if(!strcmp(gcFunction,"tContentList"))
	{
		ProcessControlVars(entries,x);
		ProcesstContentListVars(entries,x);
		tContentList();
	}

	return(0);

}//tContentCommands()


void tContent(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttContentSelectRow();
		else
			ExttContentSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetContent();
				unxsBlog("New tContent table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tContent WHERE uContent=%u"
						,uContent);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uContent);
		sscanf(field[1],"%u",&uMonth);
		sscanf(field[2],"%u",&uYear);
		sscanf(field[3],"%u",&uFeatured);
		sscanf(field[4],"%u",&uApproved);
		sprintf(cTitle,"%.100s",field[5]);
		sscanf(field[6],"%u",&uAllowSearch);
		sscanf(field[7],"%u",&uOrder);
		sscanf(field[8],"%u",&uContentType);
		sprintf(cImage,"%.255s",field[9]);
		sprintf(cComment,"%.100s",field[10]);
		cText=field[11];
		sscanf(field[12],"%u",&uOwner);
		sscanf(field[13],"%u",&uCreatedBy);
		sscanf(field[14],"%lu",&uCreatedDate);
		sscanf(field[15],"%u",&uModBy);
		sscanf(field[16],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tContent",1);
	printf("<script type='text/javascript' src='/js/ckeditor.js'></script>\n");


	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tContentTools>");
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

        ExttContentButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tContent Record Data",100);

	if(guMode==2000 || guMode==2002)
		tContentInput(1);
	else
		tContentInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttContentAuxTable();

	Footer_ism3();

}//end of tContent();


void tContentInput(unsigned uMode)
{

//uContent
	OpenRow(LANG_FL_tContent_uContent,"black");
	printf("<input title='%s' type=text name=uContent value=%u size=16 maxlength=10 "
,LANG_FT_tContent_uContent,uContent);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uContent value=%u >\n",uContent);
	}
//uMonth
	OpenRow(LANG_FL_tContent_uMonth,"black");
	printf("<input title='%s' type=text name=uMonth value=%u size=16 maxlength=10 "
,LANG_FT_tContent_uMonth,uMonth);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uMonth value=%u >\n",uMonth);
	}
//uYear
	OpenRow(LANG_FL_tContent_uYear,"black");
	printf("<input title='%s' type=text name=uYear value=%u size=16 maxlength=10 "
,LANG_FT_tContent_uYear,uYear);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uYear value=%u >\n",uYear);
	}
//uFeatured
	OpenRow(LANG_FL_tContent_uFeatured,"black");
	if(guPermLevel>=8 && uMode)
		YesNoPullDown("uFeatured",uFeatured,1);
	else
		YesNoPullDown("uFeatured",uFeatured,0);
//uApproved
	OpenRow(LANG_FL_tContent_uApproved,"black");
	if(guPermLevel>=8 && uMode)
		YesNoPullDown("uApproved",uApproved,1);
	else
		YesNoPullDown("uApproved",uApproved,0);
//cTitle
	OpenRow(LANG_FL_tContent_cTitle,"black");
	printf("<input title='%s' type=text name=cTitle value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tContent_cTitle,EncodeDoubleQuotes(cTitle));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cTitle value=\"%s\">\n",EncodeDoubleQuotes(cTitle));
	}
//uAllowSearch
	OpenRow(LANG_FL_tContent_uAllowSearch,"black");
	if(guPermLevel>=7 && uMode)
		YesNoPullDown("uAllowSearch",uAllowSearch,1);
	else
		YesNoPullDown("uAllowSearch",uAllowSearch,0);
//uOrder
	OpenRow(LANG_FL_tContent_uOrder,"black");
	printf("<input title='%s' type=text name=uOrder value=%u size=16 maxlength=10 "
,LANG_FT_tContent_uOrder,uOrder);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uOrder value=%u >\n",uOrder);
	}
//uContentType
	OpenRow(LANG_FL_tContent_uContentType,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDown("tContentType;cuContentTypePullDown","cLabel","cLabel",uContentType,1);
	else
		tTablePullDown("tContentType;cuContentTypePullDown","cLabel","cLabel",uContentType,0);
//cImage
	OpenRow(LANG_FL_tContent_cImage,"black");
	printf("<input title='%s' type=text name=cImage value=\"%s\" size=40 maxlength=255 "
,LANG_FT_tContent_cImage,EncodeDoubleQuotes(cImage));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cImage value=\"%s\">\n",EncodeDoubleQuotes(cImage));
	}
//cComment
	OpenRow(LANG_FL_tContent_cComment,"black");
	printf("<input title='%s' type=text name=cComment value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tContent_cComment,EncodeDoubleQuotes(cComment));
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cComment value=\"%s\">\n",EncodeDoubleQuotes(cComment));
	}
//cText
	OpenRow(LANG_FL_tContent_cText,"black");


	printf("<textarea title='%s' cols=80 id='cText' wrap=hard rows=16 name=cText "
,LANG_FT_tContent_cText);
	if(guPermLevel>=7 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cText);
		printf("<script type='text/javascript'>CKEDITOR.replace( 'cText' );</script>\n");
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cText);
		printf("<input type=hidden name=cText value=\"%s\" >\n",EncodeDoubleQuotes(cText));
	}

//uOwner
	OpenRow(LANG_FL_tContent_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tContent_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tContent_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tContent_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tContent_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tContentInput(unsigned uMode)


void NewtContent(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uContent FROM tContent\
				WHERE uContent=%u"
							,uContent);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tContent("<blink>Record already exists");
		tContent(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tContent();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uContent=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tContent",uContent);
	unxsBlogLog(uContent,"tContent","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uContent);
	tContent(gcQuery);
	}

}//NewtContent(unsigned uMode)


void DeletetContent(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tContent WHERE uContent=%u AND ( uOwner=%u OR %u>9 )"
					,uContent,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tContent WHERE uContent=%u"
					,uContent);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tContent("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsBlogLog(uContent,"tContent","Del");
#endif
		tContent(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsBlogLog(uContent,"tContent","DelError");
#endif
		tContent(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetContent(void)


void Insert_tContent(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tContent SET uContent=%u,uMonth=%u,uYear=%u,uFeatured=%u,uApproved=%u,cTitle='%s',uAllowSearch=%u,uOrder=%u,uContentType=%u,cImage='%s',cComment='%s',cText='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uContent
			,uMonth
			,uYear
			,uFeatured
			,uApproved
			,TextAreaSave(cTitle)
			,uAllowSearch
			,uOrder
			,uContentType
			,TextAreaSave(cImage)
			,TextAreaSave(cComment)
			,TextAreaSave(cText)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tContent(void)


void Update_tContent(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tContent SET uContent=%u,uMonth=%u,uYear=%u,uFeatured=%u,uApproved=%u,cTitle='%s',uAllowSearch=%u,uOrder=%u,uContentType=%u,cImage='%s',cComment='%s',cText='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uContent
			,uMonth
			,uYear
			,uFeatured
			,uApproved
			,TextAreaSave(cTitle)
			,uAllowSearch
			,uOrder
			,uContentType
			,TextAreaSave(cImage)
			,TextAreaSave(cComment)
			,TextAreaSave(cText)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tContent(void)


void ModtContent(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tContent.uContent,\
				tContent.uModDate\
				FROM tContent,tClient\
				WHERE tContent.uContent=%u\
				AND tContent.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uContent,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uContent,uModDate FROM tContent\
				WHERE uContent=%u"
						,uContent);
#else
	sprintf(gcQuery,"SELECT uContent FROM tContent\
				WHERE uContent=%u"
						,uContent);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tContent("<blink>Record does not exist");
	if(i<1) tContent(LANG_NBR_RECNOTEXIST);
	//if(i>1) tContent("<blink>Multiple rows!");
	if(i>1) tContent(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tContent(LANG_NBR_EXTMOD);
#endif

	Update_tContent(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tContent",uContent);
	unxsBlogLog(uContent,"tContent","Mod");
#endif
	tContent(gcQuery);

}//ModtContent(void)


void tContentList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttContentListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tContentList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttContentListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uContent<td><font face=arial,helvetica color=white>uMonth<td><font face=arial,helvetica color=white>uYear<td><font face=arial,helvetica color=white>uFeatured<td><font face=arial,helvetica color=white>uApproved<td><font face=arial,helvetica color=white>cTitle<td><font face=arial,helvetica color=white>uAllowSearch<td><font face=arial,helvetica color=white>uOrder<td><font face=arial,helvetica color=white>uContentType<td><font face=arial,helvetica color=white>cImage<td><font face=arial,helvetica color=white>cComment<td><font face=arial,helvetica color=white>cText<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luYesNo3=strtoul(field[3],NULL,10);
		char cBuf3[4];
		if(luYesNo3)
			sprintf(cBuf3,"Yes");
		else
			sprintf(cBuf3,"No");
		time_t luYesNo4=strtoul(field[4],NULL,10);
		char cBuf4[4];
		if(luYesNo4)
			sprintf(cBuf4,"Yes");
		else
			sprintf(cBuf4,"No");
		time_t luYesNo6=strtoul(field[6],NULL,10);
		char cBuf6[4];
		if(luYesNo6)
			sprintf(cBuf6,"Yes");
		else
			sprintf(cBuf6,"No");
		time_t luTime14=strtoul(field[14],NULL,10);
		char cBuf14[32];
		if(luTime14)
			ctime_r(&luTime14,cBuf14);
		else
			sprintf(cBuf14,"---");
		time_t luTime16=strtoul(field[16],NULL,10);
		char cBuf16[32];
		if(luTime16)
			ctime_r(&luTime16,cBuf16);
		else
			sprintf(cBuf16,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,cBuf3
			,cBuf4
			,field[5]
			,cBuf6
			,field[7]
			,ForeignKey("tContentType","cLabel",strtoul(field[8],NULL,10))
			,field[9]
			,field[10]
			,field[11]
			,ForeignKey("tClient","cLabel",strtoul(field[12],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[13],NULL,10))
			,cBuf14
			,ForeignKey("tClient","cLabel",strtoul(field[15],NULL,10))
			,cBuf16
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tContentList()


void CreatetContent(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tContent ( uContent INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cText TEXT NOT NULL DEFAULT '', cTitle VARCHAR(100) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cImage VARCHAR(255) NOT NULL DEFAULT '', uFeatured INT UNSIGNED NOT NULL DEFAULT 0,index (uFeatured), uAllowSearch INT UNSIGNED NOT NULL DEFAULT 0, uContentType INT UNSIGNED NOT NULL DEFAULT 0,index (uContentType), uApproved INT UNSIGNED NOT NULL DEFAULT 0, cComment VARCHAR(100) NOT NULL DEFAULT '', uMonth INT UNSIGNED NOT NULL DEFAULT 0, uYear INT UNSIGNED NOT NULL DEFAULT 0,index (uMonth,uYear), uOrder INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetContent()

