/*
FILE
	ttable.c
PURPOSE
	RAD4 table data
*/

#include "mysqlrad.h"

//Table Variables
//Table Variables
//uTable: Primary Key
static unsigned uTable=0;
//cLabel: Short label
static char cLabel[33]={""};
//uProject: Table Belongs to this Project
static unsigned uProject=0;
static char cuProjectPullDown[256]={""};
//uTableOrder: Table Menu Order
static unsigned uTableOrder=0;
//uSourceLock: Allow overwrite of table source code
static unsigned uSourceLock=0;
static char cYesNouSourceLock[32]={""};
//cDescription: Description of table function in project context
static char cDescription[256]={""};
//cSubDir: Optional subdir where source code is created for this table
static char cSubDir[101]={""};
//cLegend: Optional legend for table record group
static char cLegend[101]={""};
//cToolTip: Optional title link tool tip
static char cToolTip[101]={""};
//uNewLevel: New Table Creation Minimum uPermLevel
static unsigned uNewLevel=0;
//uModLevel: Table Modification Minimum uPermLevel
static unsigned uModLevel=0;
//uDelLevel: Table Deletion Minimum uPermLevel
static unsigned uDelLevel=0;
//uReadLevel: Table Read Minimum uPermLevel
static unsigned uReadLevel=0;
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


//cDescription: Description of table function in project context
static char *cImport="#import data format\n\n#one field per line:\n#cLabel;cTitle;tFieldType.cLabel;uOrder;[cFKSpec]\n\n#tFieldType.cLabel can be:\n#BigInt Unsigned\n#Date Time\n#Decimal\n#Foreign Key\n#Int Unsigned\n#Select Table\n#Select Table Owner\n#Text\n#Time Stamp\n#Unixtime\n#Varchar\n#Varchar Unique Key\n#Yes/No\n#Empty lines and lines starting with a # are ignored\n\n";

#define VAR_LIST_tTable "tTable.uTable,tTable.cLabel,tTable.uProject,tTable.uTableOrder,tTable.uSourceLock,tTable.cDescription,tTable.cSubDir,tTable.cLegend,tTable.cToolTip,tTable.uNewLevel,tTable.uModLevel,tTable.uDelLevel,tTable.uReadLevel,tTable.uOwner,tTable.uCreatedBy,tTable.uCreatedDate,tTable.uModBy,tTable.uModDate"

 //Local only
void Insert_tTable(void);
void Update_tTable(char *cRowid);
void ProcesstTableListVars(pentry entries[], int x);
void tTableFieldEntry(unsigned uMode);

 //In tTablefunc.c file included below
void ExtProcesstTableVars(pentry entries[], int x);
void ExttTableCommands(pentry entries[], int x);
void ExttTableButtons(void);
void ExttTableNavBar(void);
void ExttTableGetHook(entry gentries[], int x);
void ExttTableSelect(void);
void ExttTableSelectRow(void);
void ExttTableListSelect(void);
void ExttTableListFilter(void);
void ExttTableAuxTable(void);

#include "ttablefunc.c"

 //Table Variables Assignment Function
void ProcesstTableVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uTable"))
			sscanf(entries[i].val,"%u",&uTable);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uProject"))
			sscanf(entries[i].val,"%u",&uProject);
		else if(!strcmp(entries[i].name,"cuProjectPullDown"))
		{
			sprintf(cuProjectPullDown,"%.255s",entries[i].val);
			uProject=ReadPullDown("tProject","cLabel",cuProjectPullDown);
		}
		else if(!strcmp(entries[i].name,"uTableOrder"))
			sscanf(entries[i].val,"%u",&uTableOrder);
		else if(!strcmp(entries[i].name,"uSourceLock"))
			sscanf(entries[i].val,"%u",&uSourceLock);
		else if(!strcmp(entries[i].name,"cYesNouSourceLock"))
		{
			sprintf(cYesNouSourceLock,"%.31s",entries[i].val);
			uSourceLock=ReadYesNoPullDown(cYesNouSourceLock);
		}
		else if(!strcmp(entries[i].name,"cDescription"))
			sprintf(cDescription,"%.255s",entries[i].val);
		else if(!strcmp(entries[i].name,"cSubDir"))
			sprintf(cSubDir,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLegend"))
			sprintf(cLegend,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cToolTip"))
			sprintf(cToolTip,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"uNewLevel"))
			sscanf(entries[i].val,"%u",&uNewLevel);
		else if(!strcmp(entries[i].name,"uModLevel"))
			sscanf(entries[i].val,"%u",&uModLevel);
		else if(!strcmp(entries[i].name,"uDelLevel"))
			sscanf(entries[i].val,"%u",&uDelLevel);
		else if(!strcmp(entries[i].name,"uReadLevel"))
			sscanf(entries[i].val,"%u",&uReadLevel);
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
		else if(!strcmp(entries[i].name,"cImport"))
			cImport=entries[i].val;
	}

	//After so we can overwrite form data if needed.
	ExtProcesstTableVars(entries,x);

}//ProcesstTableVars()


void ProcesstTableListVars(pentry entries[], int x)
{
}//void ProcesstTableListVars(pentry entries[], int x)


int tTableCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttTableCommands(entries,x);

	if(!strcmp(gcFunction,"tTableTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tTableList();
		}

		//Default
		ProcesstTableVars(entries,x);
		tTable("");
	}
	else if(!strcmp(gcFunction,"tTableList"))
	{
		ProcessControlVars(entries,x);
		ProcesstTableListVars(entries,x);
		tTableList();
	}

	return(0);

}//tTableCommands()


void tTable(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttTableSelectRow();
		else
			ExttTableSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetTable();
				unxsRAD("New tTable table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tTable WHERE uTable=%u"
						,uTable);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uTable);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uProject);
		sscanf(field[3],"%u",&uTableOrder);
		sscanf(field[4],"%u",&uSourceLock);
		sprintf(cDescription,"%.100s",field[5]);
		sprintf(cSubDir,"%.100s",field[6]);
		sprintf(cLegend,"%.100s",field[7]);
		sprintf(cToolTip,"%.100s",field[8]);
		sscanf(field[9],"%u",&uNewLevel);
		sscanf(field[10],"%u",&uModLevel);
		sscanf(field[11],"%u",&uDelLevel);
		sscanf(field[12],"%u",&uReadLevel);
		sscanf(field[13],"%u",&uOwner);
		sscanf(field[14],"%u",&uCreatedBy);
		sscanf(field[15],"%lu",&uCreatedDate);
		sscanf(field[16],"%u",&uModBy);
		sscanf(field[17],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tTable",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tTableTools>");
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

        ExttTableButtons();

        printf("</td><td valign=top>");
	//

	if(guMode==10000)
		tTableFieldEntry(1);
	else if(guMode==10001)
		tTableFieldEntry(0);
	else if(guMode==2000 || guMode==2002)
		tTableInput(1);
	else
		tTableInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttTableAuxTable();

	Footer_ism3();

}//end of tTable();


void tTableFieldEntry(unsigned uMode)
{

	OpenFieldSet("tTable Field Import Data",100);
//uTable
	OpenRow(LANG_FL_tTable_uTable,"black");
	printf("<input title='%s' type=text name=uTable value=%u size=16 maxlength=10 ",LANG_FT_tTable_uTable,uTable);
	printf("disabled></td></tr>\n");
	printf("<input type=hidden name=uTable value=%u >\n",uTable);
//cLabel
	OpenRow(LANG_FL_tTable_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 ",LANG_FT_tTable_cLabel,EncodeDoubleQuotes(cLabel));
	printf("disabled></td></tr>\n");
	printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
//uProject
	OpenRow(LANG_FL_tTable_uProject,"black");
	printf("<input type=text size=20 value='%s' disabled>\n",ForeignKey("tProject","cLabel",uProject));
	printf("<input type=hidden size=20 maxlength=20 name=uProject value=%u >\n",uProject);
//cImport
	OpenRow(LANG_FL_tTable_cImport,"black");
	printf("<textarea title='%s' cols=80 wrap=soft rows=16 name=cImport ",LANG_FT_tTable_cImport);
	printf(">%s</textarea></td></tr>\n",cImport);

}//void tTableFieldEntry(unsigned uMode)


void tTableInput(unsigned uMode)
{

	OpenFieldSet("tTable Record Data",100);
//uTable
	OpenRow(LANG_FL_tTable_uTable,"black");
	printf("<input title='%s' type=text name=uTable value=%u size=16 maxlength=10 ",LANG_FT_tTable_uTable,uTable);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uTable value=%u >\n",uTable);
	}
//cLabel
	OpenRow(LANG_FL_tTable_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 ",LANG_FT_tTable_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tTable_uProject,"black");
	//if(guPermLevel>=7 && guPermLevel<10 && uMode)
	//	tTablePullDownOwner("tProject;cuProjectPullDown","cLabel","cLabel",uProject,1);
	//else if(guPermLevel<10 && !uMode)
	//	tTablePullDownOwner("tProject;cuProjectPullDown","cLabel","cLabel",uProject,0);
	//else if(uMode)
	//printf("<input title='%s' type=text size=20 maxlength=20 name=uProject value=%u >\n",LANG_FT_tTable_uProject,uProject);
	//else if(1)
	//{
		printf("<input type=text size=20 value='%s' disabled>\n",ForeignKey("tProject","cLabel",uProject));
		printf("<input type=hidden size=20 maxlength=20 name=uProject value=%u >\n",uProject);
	//}
//uTableOrder
	OpenRow(LANG_FL_tTable_uTableOrder,"black");
	printf("<input title='%s' type=text name=uTableOrder value=%u size=16 maxlength=10 ",LANG_FT_tTable_uTableOrder,uTableOrder);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uTableOrder value=%u >\n",uTableOrder);
	}
//uSourceLock
	OpenRow(LANG_FL_tTable_uSourceLock,"black");
	if(guPermLevel>=7 && uMode)
		YesNoPullDown("uSourceLock",uSourceLock,1);
	else
		YesNoPullDown("uSourceLock",uSourceLock,0);
//cDescription
	OpenRow(LANG_FL_tTable_cDescription,"black");
	printf("<input title='%s' type=text name=cDescription value=\"%s\" size=40 maxlength=100 ",
		LANG_FT_tTable_cDescription,EncodeDoubleQuotes(cDescription));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cDescription value=\"%s\">\n",EncodeDoubleQuotes(cDescription));
	}
//cSubDir
	OpenRow(LANG_FL_tTable_cSubDir,"black");
	printf("<input title='%s' type=text name=cSubDir value=\"%s\" size=40 maxlength=100 ",
		LANG_FT_tTable_cSubDir,EncodeDoubleQuotes(cSubDir));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cSubDir value=\"%s\">\n",EncodeDoubleQuotes(cSubDir));
	}
//cLegend
	OpenRow(LANG_FL_tTable_cLegend,"black");
	printf("<input title='%s' type=text name=cLegend value=\"%s\" size=40 maxlength=100 "
		,LANG_FT_tTable_cLegend,EncodeDoubleQuotes(cLegend));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLegend value=\"%s\">\n",EncodeDoubleQuotes(cLegend));
	}
//cToolTip
	OpenRow(LANG_FL_tTable_cToolTip,"black");
	printf("<input title='%s' type=text name=cToolTip value=\"%s\" size=40 maxlength=100 "
		,LANG_FT_tTable_cToolTip,EncodeDoubleQuotes(cToolTip));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cToolTip value=\"%s\">\n",EncodeDoubleQuotes(cToolTip));
	}
//uNewLevel
	OpenRow(LANG_FL_tTable_uNewLevel,"black");
	printf("<input title='%s' type=text name=uNewLevel value=%u size=16 maxlength=10 "
		,LANG_FT_tTable_uNewLevel,uNewLevel);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uNewLevel value=%u >\n",uNewLevel);
	}
//uModLevel
	OpenRow(LANG_FL_tTable_uModLevel,"black");
	printf("<input title='%s' type=text name=uModLevel value=%u size=16 maxlength=10 "
		,LANG_FT_tTable_uModLevel,uModLevel);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uModLevel value=%u >\n",uModLevel);
	}
//uDelLevel
	OpenRow(LANG_FL_tTable_uDelLevel,"black");
	printf("<input title='%s' type=text name=uDelLevel value=%u size=16 maxlength=10 "
		,LANG_FT_tTable_uDelLevel,uDelLevel);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uDelLevel value=%u >\n",uDelLevel);
	}
//uReadLevel
	OpenRow(LANG_FL_tTable_uReadLevel,"black");
	printf("<input title='%s' type=text name=uReadLevel value=%u size=16 maxlength=10 "
		,LANG_FT_tTable_uReadLevel,uReadLevel);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uReadLevel value=%u >\n",uReadLevel);
	}
//uOwner
	OpenRow(LANG_FL_tTable_uOwner,"black");
	if(guPermLevel>=20 && uMode)
		printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	else
		printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
//uCreatedBy
	OpenRow(LANG_FL_tTable_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
		printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	else
		printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
//uCreatedDate
	OpenRow(LANG_FL_tTable_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tTable_uModBy,"black");
	if(guPermLevel>=20 && uMode)
		printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	else
		printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
//uModDate
	OpenRow(LANG_FL_tTable_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");

}//void tTableInput(unsigned uMode)


void NewtTable(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uTable FROM tTable WHERE uTable=%u",uTable);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tTable("<blink>Record already exists");
		tTable(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tTable();
	//sprintf(gcQuery,"New record %u added");
	uTable=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tTable",uTable);
	unxsRADLog(uTable,"tTable","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uTable);
		tTable(gcQuery);
	}

}//NewtTable(unsigned uMode)


void DeletetTable(void)
{
	sprintf(gcQuery,"DELETE FROM tTable WHERE uTable=%u AND ( uOwner=%u OR %u>9 )",uTable,guLoginClient,guPermLevel);
	macro_mySQLQueryHTMLError;
	//tTable("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsRADLog(uTable,"tTable","Del");
		tTable(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsRADLog(uTable,"tTable","DelError");
		tTable(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetTable(void)


void Insert_tTable(void)
{
	//insert query
	sprintf(gcQuery,"INSERT INTO tTable SET uTable=%u,cLabel='%s',uProject=%u,uTableOrder=%u,uSourceLock=%u,"
			"cDescription='%s',cSubDir='%s',cLegend='%s',cToolTip='%s',uNewLevel=%u,uModLevel=%u,uDelLevel=%u,"
			"uReadLevel=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uTable
			,TextAreaSave(cLabel)
			,uProject
			,uTableOrder
			,uSourceLock
			,TextAreaSave(cDescription)
			,TextAreaSave(cSubDir)
			,TextAreaSave(cLegend)
			,TextAreaSave(cToolTip)
			,uNewLevel
			,uModLevel
			,uDelLevel
			,uReadLevel
			,uOwner
			,uCreatedBy
			);

	macro_mySQLQueryHTMLError;

}//void Insert_tTable(void)


void Update_tTable(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tTable SET uTable=%u,cLabel='%s',uProject=%u,uTableOrder=%u,uSourceLock=%u,"
			"cDescription='%s',cSubDir='%s',cLegend='%s',cToolTip='%s',uNewLevel=%u,uModLevel=%u,uDelLevel=%u,"
			"uReadLevel=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uTable
			,TextAreaSave(cLabel)
			,uProject
			,uTableOrder
			,uSourceLock
			,TextAreaSave(cDescription)
			,TextAreaSave(cSubDir)
			,TextAreaSave(cLegend)
			,TextAreaSave(cToolTip)
			,uNewLevel
			,uModLevel
			,uDelLevel
			,uReadLevel
			,uModBy
			,cRowid);

	macro_mySQLQueryHTMLError;

}//void Update_tTable(void)


void ModtTable(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tTable.uTable,\
				tTable.uModDate\
				FROM tTable,tClient\
				WHERE tTable.uTable=%u\
				AND tTable.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uTable,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uTable,uModDate FROM tTable\
				WHERE uTable=%u"
						,uTable);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tTable("<blink>Record does not exist");
	if(i<1) tTable(LANG_NBR_RECNOTEXIST);
	//if(i>1) tTable("<blink>Multiple rows!");
	if(i>1) tTable(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tTable(LANG_NBR_EXTMOD);

	Update_tTable(field[0]);
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tTable",uTable);
	unxsRADLog(uTable,"tTable","Mod");
	tTable(gcQuery);

}//ModtTable(void)


void tTableList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttTableListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tTableList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttTableListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
			"<td><font color=white>uTable"
			"<td><font color=white>cLabel"
			"<td><font color=white>uProject"
			"<td><font color=white>uTableOrder"
			"<td><font color=white>uSourceLock"
			"<td><font color=white>cDescription"
			"<td><font color=white>cSubDir"
			"<td><font color=white>cLegend"
			"<td><font color=white>cToolTip"
			"<td><font color=white>uNewLevel"
			"<td><font color=white>uModLevel"
			"<td><font color=white>uDelLevel"
			"<td><font color=white>uReadLevel"
			"<td><font color=white>uOwner"
			"<td><font color=white>uCreatedBy"
			"<td><font color=white>uCreatedDate"
			"<td><font color=white>uModBy"
			"<td><font color=white>uModDate"
		"</tr>");

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
		long unsigned luYesNo4=strtoul(field[4],NULL,10);
		char cBuf4[4];
		if(luYesNo4)
			sprintf(cBuf4,"Yes");
		else
			sprintf(cBuf4,"No");
		time_t luTime15=strtoul(field[15],NULL,10);
		char cBuf15[32];
		if(luTime15)
			ctime_r(&luTime15,cBuf15);
		else
			sprintf(cBuf15,"---");
		time_t luTime17=strtoul(field[17],NULL,10);
		char cBuf17[32];
		if(luTime17)
			ctime_r(&luTime17,cBuf17);
		else
			sprintf(cBuf17,"---");
		char cBuf2[100];
		sprintf(cBuf2,"%.99s",ForeignKey("tProject","cLabel",strtoul(field[2],NULL,10)));
		char cBuf13[100];
		sprintf(cBuf13,"%.99s",ForeignKey("tClient","cLabel",strtoul(field[13],NULL,10)));
		char cBuf14[100];
		sprintf(cBuf14,"%.99s",ForeignKey("tClient","cLabel",strtoul(field[14],NULL,10)));
		char cBuf16[100];
		sprintf(cBuf16,"%.99s",ForeignKey("tClient","cLabel",strtoul(field[16],NULL,10)));
		printf("<td><a class=darkLink href=?gcFunction=tTemplate&uTemplate=%s>%s</a>"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,cBuf2
			,field[3]
			,cBuf4
			,field[5]
			,field[6]
			,field[7]
			,field[8]
			,field[9]
			,field[10]
			,field[11]
			,field[12]
			,cBuf13
			,cBuf14
			,cBuf15
			,cBuf16
			,cBuf17
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tTableList()


void CreatetTable(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tTable ("
			"uTable INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
			"uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner),"
			"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uNewLevel INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModLevel INT UNSIGNED NOT NULL DEFAULT 0,"
			"uDelLevel INT UNSIGNED NOT NULL DEFAULT 0,"
			"uReadLevel INT UNSIGNED NOT NULL DEFAULT 0,"
			"uTableOrder INT UNSIGNED NOT NULL DEFAULT 0,"
			"uSourceLock INT UNSIGNED NOT NULL DEFAULT 0,"
			"cLegend VARCHAR(100) NOT NULL DEFAULT '',"
			"cToolTip VARCHAR(100) NOT NULL DEFAULT '',"
			"cDescription VARCHAR(255) NOT NULL DEFAULT '',"
			"cSubDir VARCHAR(100) NOT NULL DEFAULT '',"
			"uProject INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetTable()

