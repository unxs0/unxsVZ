/*
FILE
	tField source code of unxsRAD.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2009 for Unixservice
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tfieldfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uField: Primary Key
static unsigned uField=0;
//cLabel: Short label
static char cLabel[33]={""};
//uProject: Field Belongs to this Project
static unsigned uProject=0;
static char cuProjectPullDown[256]={""};
//uTable: Field Belongs to this Table
static unsigned uTable=0;
static char cuTablePullDown[256]={""};
//uOrder: Order in Form
static unsigned uOrder=0;
//uFieldType: Both SQL and Form Field Type
static unsigned uFieldType=0;
static char cuFieldTypePullDown[256]={""};
//uIndexType: SQL Index If Any
static unsigned uIndexType=0;
static char cuIndexTypePullDown[256]={""};
//cFKSpec: Foreign Key Table and Field
static char cFKSpec[101]={""};
//cExtIndex: Optional Multiple Index SQL Create Code
static char cExtIndex[101]={""};
//cTitle: Tool Tip Text
static char cTitle[101]={""};
//uSQLSize: SQL Data Storage Size if Applies
static unsigned uSQLSize=0;
//uHtmlXSize: HTML X/Width Size if Applies
static unsigned uHtmlXSize=0;
//uHtmlYSize: HTML Y/Height Size if Applies
static unsigned uHtmlYSize=0;
//uHtmlMax: HTML Max/Length if Applies
static unsigned uHtmlMax=0;
//cFormDefault: HTML Form Default Value if Applies
static char cFormDefault[101]={""};
//cSQLDefault: SQL Default Value if Applies
static char cSQLDefault[101]={""};
//uReadLevel: Row Read/Display Level
static unsigned uReadLevel=0;
//uModLevel: Row Mod Level
static unsigned uModLevel=0;
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



#define VAR_LIST_tField "tField.uField,tField.cLabel,tField.uProject,tField.uTable,tField.uOrder,tField.uFieldType,tField.uIndexType,tField.cFKSpec,tField.cExtIndex,tField.cTitle,tField.uSQLSize,tField.uHtmlXSize,tField.uHtmlYSize,tField.uHtmlMax,tField.cFormDefault,tField.cSQLDefault,tField.uReadLevel,tField.uModLevel,tField.uOwner,tField.uCreatedBy,tField.uCreatedDate,tField.uModBy,tField.uModDate"

 //Local only
void Insert_tField(void);
void Update_tField(char *cRowid);
void ProcesstFieldListVars(pentry entries[], int x);

 //In tFieldfunc.h file included below
void ExtProcesstFieldVars(pentry entries[], int x);
void ExttFieldCommands(pentry entries[], int x);
void ExttFieldButtons(void);
void ExttFieldNavBar(void);
void ExttFieldGetHook(entry gentries[], int x);
void ExttFieldSelect(void);
void ExttFieldSelectRow(void);
void ExttFieldListSelect(void);
void ExttFieldListFilter(void);
void ExttFieldAuxTable(void);

#include "tfieldfunc.h"

 //Table Variables Assignment Function
void ProcesstFieldVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uField"))
			sscanf(entries[i].val,"%u",&uField);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uProject"))
			sscanf(entries[i].val,"%u",&uProject);
		else if(!strcmp(entries[i].name,"cuProjectPullDown"))
		{
			sprintf(cuProjectPullDown,"%.255s",entries[i].val);
			uProject=ReadPullDown("tProject","cLabel",cuProjectPullDown);
		}
		else if(!strcmp(entries[i].name,"uTable"))
			sscanf(entries[i].val,"%u",&uTable);
		else if(!strcmp(entries[i].name,"cuTablePullDown"))
		{
			sprintf(cuTablePullDown,"%.255s",entries[i].val);
			uTable=ReadPullDown("tTable","cLabel",cuTablePullDown);
		}
		else if(!strcmp(entries[i].name,"uOrder"))
			sscanf(entries[i].val,"%u",&uOrder);
		else if(!strcmp(entries[i].name,"uFieldType"))
			sscanf(entries[i].val,"%u",&uFieldType);
		else if(!strcmp(entries[i].name,"cuFieldTypePullDown"))
		{
			sprintf(cuFieldTypePullDown,"%.255s",entries[i].val);
			uFieldType=ReadPullDown("tFieldType","cLabel",cuFieldTypePullDown);
		}
		else if(!strcmp(entries[i].name,"uIndexType"))
			sscanf(entries[i].val,"%u",&uIndexType);
		else if(!strcmp(entries[i].name,"cuIndexTypePullDown"))
		{
			sprintf(cuIndexTypePullDown,"%.255s",entries[i].val);
			uIndexType=ReadPullDown("tIndexType","cLabel",cuIndexTypePullDown);
		}
		else if(!strcmp(entries[i].name,"cFKSpec"))
			sprintf(cFKSpec,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cExtIndex"))
			sprintf(cExtIndex,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cTitle"))
			sprintf(cTitle,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"uSQLSize"))
			sscanf(entries[i].val,"%u",&uSQLSize);
		else if(!strcmp(entries[i].name,"uHtmlXSize"))
			sscanf(entries[i].val,"%u",&uHtmlXSize);
		else if(!strcmp(entries[i].name,"uHtmlYSize"))
			sscanf(entries[i].val,"%u",&uHtmlYSize);
		else if(!strcmp(entries[i].name,"uHtmlMax"))
			sscanf(entries[i].val,"%u",&uHtmlMax);
		else if(!strcmp(entries[i].name,"cFormDefault"))
			sprintf(cFormDefault,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cSQLDefault"))
			sprintf(cSQLDefault,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"uReadLevel"))
			sscanf(entries[i].val,"%u",&uReadLevel);
		else if(!strcmp(entries[i].name,"uModLevel"))
			sscanf(entries[i].val,"%u",&uModLevel);
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
	ExtProcesstFieldVars(entries,x);

}//ProcesstFieldVars()


void ProcesstFieldListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uField);
                        guMode=2002;
                        tField("");
                }
        }
}//void ProcesstFieldListVars(pentry entries[], int x)


int tFieldCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttFieldCommands(entries,x);

	if(!strcmp(gcFunction,"tFieldTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tFieldList();
		}

		//Default
		ProcesstFieldVars(entries,x);
		tField("");
	}
	else if(!strcmp(gcFunction,"tFieldList"))
	{
		ProcessControlVars(entries,x);
		ProcesstFieldListVars(entries,x);
		tFieldList();
	}

	return(0);

}//tFieldCommands()


void tField(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttFieldSelectRow();
		else
			ExttFieldSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetField();
				unxsRAD("New tField table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tField WHERE uField=%u"
						,uField);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uField);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uProject);
		sscanf(field[3],"%u",&uTable);
		sscanf(field[4],"%u",&uOrder);
		sscanf(field[5],"%u",&uFieldType);
		sscanf(field[6],"%u",&uIndexType);
		sprintf(cFKSpec,"%.100s",field[7]);
		sprintf(cExtIndex,"%.100s",field[8]);
		sprintf(cTitle,"%.100s",field[9]);
		sscanf(field[10],"%u",&uSQLSize);
		sscanf(field[11],"%u",&uHtmlXSize);
		sscanf(field[12],"%u",&uHtmlYSize);
		sscanf(field[13],"%u",&uHtmlMax);
		sprintf(cFormDefault,"%.100s",field[14]);
		sprintf(cSQLDefault,"%.100s",field[15]);
		sscanf(field[16],"%u",&uReadLevel);
		sscanf(field[17],"%u",&uModLevel);
		sscanf(field[18],"%u",&uOwner);
		sscanf(field[19],"%u",&uCreatedBy);
		sscanf(field[20],"%lu",&uCreatedDate);
		sscanf(field[21],"%u",&uModBy);
		sscanf(field[22],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tField",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tFieldTools>");
	printf("<input type=hidden name=gluRowid value=%lu>",gluRowid);
	if(guI)
	{
		if(guMode==6)
			//printf(" Found");
			printf(LANG_NBR_FOUND);
		else if(guMode==7)
			printf("Workflow loaded");
		else if(guMode==5)
			//printf(" Modified");
			printf(LANG_NBR_MODIFIED);
		else if(guMode==4)
			//printf(" New");
			printf(LANG_NBR_NEW);
		if(guMode!=7)
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

        ExttFieldButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tField Record Data",100);

	if(guMode==2000 || guMode==2002)
		tFieldInput(1);
	else
		tFieldInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttFieldAuxTable();

	Footer_ism3();

}//end of tField();


void tFieldInput(unsigned uMode)
{

//uField
	OpenRow(LANG_FL_tField_uField,"black");
	printf("<input title='%s' type=text name=uField value=%u size=16 maxlength=10 "
,LANG_FT_tField_uField,uField);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uField value=%u >\n",uField);
	}
//cLabel
	OpenRow(LANG_FL_tField_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tField_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uProject
	OpenRow(LANG_FL_tField_uProject,"black");
	printf("<input type=text size=20 value='%s' disabled>\n",ForeignKey("tProject","cLabel",uProject));
	printf("<input type=hidden size=20 maxlength=20 name=uProject value=%u >\n",uProject);
//uTable
	OpenRow(LANG_FL_tField_uTable,"black");
	printf("<input type=text size=20 value='%s' disabled>\n",ForeignKey("tTable","cLabel",uTable));
	printf("<input type=hidden size=20 maxlength=20 name=uTable value=%u >\n",uTable);
//uOrder
	OpenRow(LANG_FL_tField_uOrder,"black");
	printf("<input title='%s' type=text name=uOrder value=%u size=16 maxlength=10 "
,LANG_FT_tField_uOrder,uOrder);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uOrder value=%u >\n",uOrder);
	}
//uFieldType
	OpenRow(LANG_FL_tField_uFieldType,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDown("tFieldType;cuFieldTypePullDown","cLabel","cLabel",uFieldType,1);
	else
		tTablePullDown("tFieldType;cuFieldTypePullDown","cLabel","cLabel",uFieldType,0);
//uIndexType
	OpenRow(LANG_FL_tField_uIndexType,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDown("tIndexType;cuIndexTypePullDown","cLabel","cLabel",uIndexType,1);
	else
		tTablePullDown("tIndexType;cuIndexTypePullDown","cLabel","cLabel",uIndexType,0);
//cFKSpec
	OpenRow(LANG_FL_tField_cFKSpec,"black");
	printf("<input title='%s' type=text name=cFKSpec value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tField_cFKSpec,EncodeDoubleQuotes(cFKSpec));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cFKSpec value=\"%s\">\n",EncodeDoubleQuotes(cFKSpec));
	}
//cExtIndex
	OpenRow(LANG_FL_tField_cExtIndex,"black");
	printf("<input title='%s' type=text name=cExtIndex value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tField_cExtIndex,EncodeDoubleQuotes(cExtIndex));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cExtIndex value=\"%s\">\n",EncodeDoubleQuotes(cExtIndex));
	}
//cTitle
	OpenRow(LANG_FL_tField_cTitle,"black");
	printf("<input title='%s' type=text name=cTitle value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tField_cTitle,EncodeDoubleQuotes(cTitle));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cTitle value=\"%s\">\n",EncodeDoubleQuotes(cTitle));
	}
//uSQLSize
	OpenRow(LANG_FL_tField_uSQLSize,"black");
	printf("<input title='%s' type=text name=uSQLSize value=%u size=16 maxlength=10 "
,LANG_FT_tField_uSQLSize,uSQLSize);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uSQLSize value=%u >\n",uSQLSize);
	}
//uHtmlXSize
	OpenRow(LANG_FL_tField_uHtmlXSize,"black");
	printf("<input title='%s' type=text name=uHtmlXSize value=%u size=16 maxlength=10 "
,LANG_FT_tField_uHtmlXSize,uHtmlXSize);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uHtmlXSize value=%u >\n",uHtmlXSize);
	}
//uHtmlYSize
	OpenRow(LANG_FL_tField_uHtmlYSize,"black");
	printf("<input title='%s' type=text name=uHtmlYSize value=%u size=16 maxlength=10 "
,LANG_FT_tField_uHtmlYSize,uHtmlYSize);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uHtmlYSize value=%u >\n",uHtmlYSize);
	}
//uHtmlMax
	OpenRow(LANG_FL_tField_uHtmlMax,"black");
	printf("<input title='%s' type=text name=uHtmlMax value=%u size=16 maxlength=10 "
,LANG_FT_tField_uHtmlMax,uHtmlMax);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uHtmlMax value=%u >\n",uHtmlMax);
	}
//cFormDefault
	OpenRow(LANG_FL_tField_cFormDefault,"black");
	printf("<input title='%s' type=text name=cFormDefault value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tField_cFormDefault,EncodeDoubleQuotes(cFormDefault));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cFormDefault value=\"%s\">\n",EncodeDoubleQuotes(cFormDefault));
	}
//cSQLDefault
	OpenRow(LANG_FL_tField_cSQLDefault,"black");
	printf("<input title='%s' type=text name=cSQLDefault value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tField_cSQLDefault,EncodeDoubleQuotes(cSQLDefault));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cSQLDefault value=\"%s\">\n",EncodeDoubleQuotes(cSQLDefault));
	}
//uReadLevel
	OpenRow(LANG_FL_tField_uReadLevel,"black");
	printf("<input title='%s' type=text name=uReadLevel value=%u size=16 maxlength=10 "
,LANG_FT_tField_uReadLevel,uReadLevel);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uReadLevel value=%u >\n",uReadLevel);
	}
//uModLevel
	OpenRow(LANG_FL_tField_uModLevel,"black");
	printf("<input title='%s' type=text name=uModLevel value=%u size=16 maxlength=10 "
,LANG_FT_tField_uModLevel,uModLevel);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uModLevel value=%u >\n",uModLevel);
	}
//uOwner
	OpenRow(LANG_FL_tField_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tField_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tField_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tField_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tField_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tFieldInput(unsigned uMode)


void NewtField(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uField FROM tField\
				WHERE uField=%u"
							,uField);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tField("<blink>Record already exists");
		tField(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tField();
	//sprintf(gcQuery,"New record %u added");
	uField=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tField",uField);
	unxsRADLog(uField,"tField","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uField);
	tField(gcQuery);
	}

}//NewtField(unsigned uMode)


void DeletetField(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tField WHERE uField=%u AND ( uOwner=%u OR %u>9 )"
					,uField,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tField WHERE uField=%u"
					,uField);
#endif
	macro_mySQLQueryHTMLError;
	//tField("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsRADLog(uField,"tField","Del");
#endif
		tField(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsRADLog(uField,"tField","DelError");
#endif
		tField(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetField(void)


void Insert_tField(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tField SET uField=%u,cLabel='%s',uProject=%u,uTable=%u,uOrder=%u,uFieldType=%u,uIndexType=%u,cFKSpec='%s',cExtIndex='%s',cTitle='%s',uSQLSize=%u,uHtmlXSize=%u,uHtmlYSize=%u,uHtmlMax=%u,cFormDefault='%s',cSQLDefault='%s',uReadLevel=%u,uModLevel=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uField
			,TextAreaSave(cLabel)
			,uProject
			,uTable
			,uOrder
			,uFieldType
			,uIndexType
			,TextAreaSave(cFKSpec)
			,TextAreaSave(cExtIndex)
			,TextAreaSave(cTitle)
			,uSQLSize
			,uHtmlXSize
			,uHtmlYSize
			,uHtmlMax
			,TextAreaSave(cFormDefault)
			,TextAreaSave(cSQLDefault)
			,uReadLevel
			,uModLevel
			,uOwner
			,uCreatedBy
			);

	macro_mySQLQueryHTMLError;

}//void Insert_tField(void)


void Update_tField(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tField SET uField=%u,cLabel='%s',uProject=%u,uTable=%u,uOrder=%u,uFieldType=%u,uIndexType=%u,cFKSpec='%s',cExtIndex='%s',cTitle='%s',uSQLSize=%u,uHtmlXSize=%u,uHtmlYSize=%u,uHtmlMax=%u,cFormDefault='%s',cSQLDefault='%s',uReadLevel=%u,uModLevel=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uField
			,TextAreaSave(cLabel)
			,uProject
			,uTable
			,uOrder
			,uFieldType
			,uIndexType
			,TextAreaSave(cFKSpec)
			,TextAreaSave(cExtIndex)
			,TextAreaSave(cTitle)
			,uSQLSize
			,uHtmlXSize
			,uHtmlYSize
			,uHtmlMax
			,TextAreaSave(cFormDefault)
			,TextAreaSave(cSQLDefault)
			,uReadLevel
			,uModLevel
			,uModBy
			,cRowid);

	macro_mySQLQueryHTMLError;

}//void Update_tField(void)


void ModtField(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tField.uField,\
				tField.uModDate\
				FROM tField,tClient\
				WHERE tField.uField=%u\
				AND tField.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uField,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uField,uModDate FROM tField\
				WHERE uField=%u"
						,uField);
#else
	sprintf(gcQuery,"SELECT uField FROM tField\
				WHERE uField=%u"
						,uField);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tField("<blink>Record does not exist");
	if(i<1) tField(LANG_NBR_RECNOTEXIST);
	//if(i>1) tField("<blink>Multiple rows!");
	if(i>1) tField(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tField(LANG_NBR_EXTMOD);
#endif

	Update_tField(field[0]);
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tField",uField);
	unxsRADLog(uField,"tField","Mod");
#endif
	tField(gcQuery);

}//ModtField(void)


void tFieldList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttFieldListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tFieldList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttFieldListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uField<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uProject<td><font face=arial,helvetica color=white>uTable<td><font face=arial,helvetica color=white>uOrder<td><font face=arial,helvetica color=white>uFieldType<td><font face=arial,helvetica color=white>uIndexType<td><font face=arial,helvetica color=white>cFKSpec<td><font face=arial,helvetica color=white>cExtIndex<td><font face=arial,helvetica color=white>cTitle<td><font face=arial,helvetica color=white>uSQLSize<td><font face=arial,helvetica color=white>uHtmlXSize<td><font face=arial,helvetica color=white>uHtmlYSize<td><font face=arial,helvetica color=white>uHtmlMax<td><font face=arial,helvetica color=white>cFormDefault<td><font face=arial,helvetica color=white>cSQLDefault<td><font face=arial,helvetica color=white>uReadLevel<td><font face=arial,helvetica color=white>uModLevel<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime20=strtoul(field[20],NULL,10);
		char cBuf20[32];
		if(luTime20)
			ctime_r(&luTime20,cBuf20);
		else
			sprintf(cBuf20,"---");
		time_t luTime22=strtoul(field[22],NULL,10);
		char cBuf22[32];
		if(luTime22)
			ctime_r(&luTime22,cBuf22);
		else
			sprintf(cBuf22,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey("tProject","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tTable","cLabel",strtoul(field[3],NULL,10))
			,field[4]
			,ForeignKey("tFieldType","cLabel",strtoul(field[5],NULL,10))
			,ForeignKey("tIndexType","cLabel",strtoul(field[6],NULL,10))
			,field[7]
			,field[8]
			,field[9]
			,field[10]
			,field[11]
			,field[12]
			,field[13]
			,field[14]
			,field[15]
			,field[16]
			,field[17]
			,ForeignKey("tClient","cLabel",strtoul(field[18],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[19],NULL,10))
			,cBuf20
			,ForeignKey("tClient","cLabel",strtoul(field[21],NULL,10))
			,cBuf22
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tFieldList()


void CreatetField(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tField ( uField INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uTable INT UNSIGNED NOT NULL DEFAULT 0, uFieldType INT UNSIGNED NOT NULL DEFAULT 0, uProject INT UNSIGNED NOT NULL DEFAULT 0, uIndexType INT UNSIGNED NOT NULL DEFAULT 0, cFKSpec VARCHAR(100) NOT NULL DEFAULT '', cExtIndex VARCHAR(100) NOT NULL DEFAULT '', uOrder INT UNSIGNED NOT NULL DEFAULT 0, cTitle VARCHAR(100) NOT NULL DEFAULT '', uSQLSize INT UNSIGNED NOT NULL DEFAULT 0, uHtmlXSize INT UNSIGNED NOT NULL DEFAULT 0, uHtmlYSize INT UNSIGNED NOT NULL DEFAULT 0, uHtmlMax INT UNSIGNED NOT NULL DEFAULT 0, cFormDefault VARCHAR(100) NOT NULL DEFAULT '', cSQLDefault VARCHAR(100) NOT NULL DEFAULT '', uModLevel INT UNSIGNED NOT NULL DEFAULT 0, uReadLevel INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetField()

