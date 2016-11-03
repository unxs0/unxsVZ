/*
FILE
	tConfigSpec source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tconfigspecfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uConfigSpec: Primary Key
static unsigned uConfigSpec=0;
//cLabel: Short label
static char cLabel[33]={""};
//cPath: File path added to tConfiguration root path
static char cPath[128]={""};
//cOwner: System file owner if applicable
static char cOwner[33]={""};
//cGroup: System file group if applicable
static char cGroup[33]={""};
//cPerms: System file chmod perms if applicable
static char cPerms[17]={""};
//uTemplate: Source of downstream cConfig values
static unsigned uTemplate=0;
static char cuTemplatePullDown[256]={""};
//cNewExec: System call on NewServerConfigJob()
static char cNewExec[101]={""};
//cModExec: System call on ModServerConfigJob()
static char cModExec[101]={""};
//cDelExec: System call on DelServerConfigJob()
static char cDelExec[101]={""};
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



#define VAR_LIST_tConfigSpec "tConfigSpec.uConfigSpec,tConfigSpec.cLabel,tConfigSpec.cPath,tConfigSpec.cOwner,tConfigSpec.cGroup,tConfigSpec.cPerms,tConfigSpec.uTemplate,tConfigSpec.cNewExec,tConfigSpec.cModExec,tConfigSpec.cDelExec,tConfigSpec.uOwner,tConfigSpec.uCreatedBy,tConfigSpec.uCreatedDate,tConfigSpec.uModBy,tConfigSpec.uModDate"

 //Local only
void Insert_tConfigSpec(void);
void Update_tConfigSpec(char *cRowid);
void ProcesstConfigSpecListVars(pentry entries[], int x);

 //In tConfigSpecfunc.h file included below
void ExtProcesstConfigSpecVars(pentry entries[], int x);
void ExttConfigSpecCommands(pentry entries[], int x);
void ExttConfigSpecButtons(void);
void ExttConfigSpecNavBar(void);
void ExttConfigSpecGetHook(entry gentries[], int x);
void ExttConfigSpecSelect(void);
void ExttConfigSpecSelectRow(void);
void ExttConfigSpecListSelect(void);
void ExttConfigSpecListFilter(void);
void ExttConfigSpecAuxTable(void);

#include "tconfigspecfunc.h"

 //Table Variables Assignment Function
void ProcesstConfigSpecVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uConfigSpec"))
			sscanf(entries[i].val,"%u",&uConfigSpec);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cPath"))
			sprintf(cPath,"%.127s",entries[i].val);
		else if(!strcmp(entries[i].name,"cOwner"))
			sprintf(cOwner,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cGroup"))
			sprintf(cGroup,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cPerms"))
			sprintf(cPerms,"%.16s",entries[i].val);
		else if(!strcmp(entries[i].name,"uTemplate"))
			sscanf(entries[i].val,"%u",&uTemplate);
		else if(!strcmp(entries[i].name,"cuTemplatePullDown"))
		{
			sprintf(cuTemplatePullDown,"%.255s",entries[i].val);
			uTemplate=ReadPullDown("tTemplate","cLabel",cuTemplatePullDown);
		}
		else if(!strcmp(entries[i].name,"cNewExec"))
			sprintf(cNewExec,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cModExec"))
			sprintf(cModExec,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cDelExec"))
			sprintf(cDelExec,"%.100s",entries[i].val);
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
	ExtProcesstConfigSpecVars(entries,x);

}//ProcesstConfigSpecVars()


void ProcesstConfigSpecListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uConfigSpec);
                        guMode=2002;
                        tConfigSpec("");
                }
        }
}//void ProcesstConfigSpecListVars(pentry entries[], int x)


int tConfigSpecCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttConfigSpecCommands(entries,x);

	if(!strcmp(gcFunction,"tConfigSpecTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tConfigSpecList();
		}

		//Default
		ProcesstConfigSpecVars(entries,x);
		tConfigSpec("");
	}
	else if(!strcmp(gcFunction,"tConfigSpecList"))
	{
		ProcessControlVars(entries,x);
		ProcesstConfigSpecListVars(entries,x);
		tConfigSpecList();
	}

	return(0);

}//tConfigSpecCommands()


void tConfigSpec(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttConfigSpecSelectRow();
		else
			ExttConfigSpecSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetConfigSpec();
				unxsMail("New tConfigSpec table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tConfigSpec WHERE uConfigSpec=%u"
						,uConfigSpec);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uConfigSpec);
		sprintf(cLabel,"%.32s",field[1]);
		sprintf(cPath,"%.127s",field[2]);
		sprintf(cOwner,"%.32s",field[3]);
		sprintf(cGroup,"%.32s",field[4]);
		sprintf(cPerms,"%.16s",field[5]);
		sscanf(field[6],"%u",&uTemplate);
		sprintf(cNewExec,"%.100s",field[7]);
		sprintf(cModExec,"%.100s",field[8]);
		sprintf(cDelExec,"%.100s",field[9]);
		sscanf(field[10],"%u",&uOwner);
		sscanf(field[11],"%u",&uCreatedBy);
		sscanf(field[12],"%lu",&uCreatedDate);
		sscanf(field[13],"%u",&uModBy);
		sscanf(field[14],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tConfigSpec",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tConfigSpecTools>");
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

        ExttConfigSpecButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tConfigSpec Record Data",100);

	if(guMode==2000 || guMode==2002)
		tConfigSpecInput(1);
	else
		tConfigSpecInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttConfigSpecAuxTable();

	Footer_ism3();

}//end of tConfigSpec();


void tConfigSpecInput(unsigned uMode)
{

//uConfigSpec
	OpenRow(LANG_FL_tConfigSpec_uConfigSpec,"black");
	printf("<input title='%s' type=text name=uConfigSpec value=%u size=16 maxlength=10 "
,LANG_FT_tConfigSpec_uConfigSpec,uConfigSpec);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uConfigSpec value=%u >\n",uConfigSpec);
	}
//cLabel
	OpenRow(LANG_FL_tConfigSpec_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tConfigSpec_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//cPath
	OpenRow(LANG_FL_tConfigSpec_cPath,"black");
	printf("<input title='%s' type=text name=cPath value=\"%s\" size=40 maxlength=127 "
,LANG_FT_tConfigSpec_cPath,EncodeDoubleQuotes(cPath));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cPath value=\"%s\">\n",EncodeDoubleQuotes(cPath));
	}
//cOwner
	OpenRow(LANG_FL_tConfigSpec_cOwner,"black");
	printf("<input title='%s' type=text name=cOwner value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tConfigSpec_cOwner,EncodeDoubleQuotes(cOwner));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cOwner value=\"%s\">\n",EncodeDoubleQuotes(cOwner));
	}
//cGroup
	OpenRow(LANG_FL_tConfigSpec_cGroup,"black");
	printf("<input title='%s' type=text name=cGroup value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tConfigSpec_cGroup,EncodeDoubleQuotes(cGroup));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cGroup value=\"%s\">\n",EncodeDoubleQuotes(cGroup));
	}
//cPerms
	OpenRow(LANG_FL_tConfigSpec_cPerms,"black");
	printf("<input title='%s' type=text name=cPerms value=\"%s\" size=40 maxlength=16 "
,LANG_FT_tConfigSpec_cPerms,EncodeDoubleQuotes(cPerms));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cPerms value=\"%s\">\n",EncodeDoubleQuotes(cPerms));
	}
//uTemplate
	OpenRow(LANG_FL_tConfigSpec_uTemplate,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tTemplate;cuTemplatePullDown","cLabel","cLabel",uTemplate,1);
	else
		tTablePullDown("tTemplate;cuTemplatePullDown","cLabel","cLabel",uTemplate,0);
//cNewExec
	OpenRow(LANG_FL_tConfigSpec_cNewExec,"black");
	printf("<input title='%s' type=text name=cNewExec value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tConfigSpec_cNewExec,EncodeDoubleQuotes(cNewExec));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cNewExec value=\"%s\">\n",EncodeDoubleQuotes(cNewExec));
	}
//cModExec
	OpenRow(LANG_FL_tConfigSpec_cModExec,"black");
	printf("<input title='%s' type=text name=cModExec value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tConfigSpec_cModExec,EncodeDoubleQuotes(cModExec));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cModExec value=\"%s\">\n",EncodeDoubleQuotes(cModExec));
	}
//cDelExec
	OpenRow(LANG_FL_tConfigSpec_cDelExec,"black");
	printf("<input title='%s' type=text name=cDelExec value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tConfigSpec_cDelExec,EncodeDoubleQuotes(cDelExec));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cDelExec value=\"%s\">\n",EncodeDoubleQuotes(cDelExec));
	}
//uOwner
	OpenRow(LANG_FL_tConfigSpec_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tConfigSpec_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tConfigSpec_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tConfigSpec_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tConfigSpec_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tConfigSpecInput(unsigned uMode)


void NewtConfigSpec(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uConfigSpec FROM tConfigSpec\
				WHERE uConfigSpec=%u"
							,uConfigSpec);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tConfigSpec("<blink>Record already exists");
		tConfigSpec(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tConfigSpec();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uConfigSpec=mysql_insert_id(&gMysql);

	uCreatedDate=luGetCreatedDate("tConfigSpec",uConfigSpec);
	unxsMailLog(uConfigSpec,"tConfigSpec","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uConfigSpec);
	tConfigSpec(gcQuery);
	}

}//NewtConfigSpec(unsigned uMode)


void DeletetConfigSpec(void)
{

	sprintf(gcQuery,"DELETE FROM tConfigSpec WHERE uConfigSpec=%u AND ( uOwner=%u OR %u>9 )"
					,uConfigSpec,guLoginClient,guPermLevel);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tConfigSpec("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uConfigSpec,"tConfigSpec","Del");

		tConfigSpec(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uConfigSpec,"tConfigSpec","DelError");

		tConfigSpec(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetConfigSpec(void)


void Insert_tConfigSpec(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tConfigSpec SET uConfigSpec=%u,cLabel='%s',cPath='%s',cOwner='%s',cGroup='%s',cPerms='%s',uTemplate=%u,cNewExec='%s',cModExec='%s',cDelExec='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uConfigSpec
			,TextAreaSave(cLabel)
			,TextAreaSave(cPath)
			,TextAreaSave(cOwner)
			,TextAreaSave(cGroup)
			,TextAreaSave(cPerms)
			,uTemplate
			,TextAreaSave(cNewExec)
			,TextAreaSave(cModExec)
			,TextAreaSave(cDelExec)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tConfigSpec(void)


void Update_tConfigSpec(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tConfigSpec SET uConfigSpec=%u,cLabel='%s',cPath='%s',cOwner='%s',cGroup='%s',cPerms='%s',uTemplate=%u,cNewExec='%s',cModExec='%s',cDelExec='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uConfigSpec
			,TextAreaSave(cLabel)
			,TextAreaSave(cPath)
			,TextAreaSave(cOwner)
			,TextAreaSave(cGroup)
			,TextAreaSave(cPerms)
			,uTemplate
			,TextAreaSave(cNewExec)
			,TextAreaSave(cModExec)
			,TextAreaSave(cDelExec)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tConfigSpec(void)


void ModtConfigSpec(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uConfigSpec,uModDate FROM tConfigSpec WHERE uConfigSpec=%u"
						,uConfigSpec);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tConfigSpec("<blink>Record does not exist");
	if(i<1) tConfigSpec(LANG_NBR_RECNOTEXIST);
	//if(i>1) tConfigSpec("<blink>Multiple rows!");
	if(i>1) tConfigSpec(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tConfigSpec(LANG_NBR_EXTMOD);


	Update_tConfigSpec(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);

	uModDate=luGetModDate("tConfigSpec",uConfigSpec);
	unxsMailLog(uConfigSpec,"tConfigSpec","Mod");

	tConfigSpec(gcQuery);

}//ModtConfigSpec(void)


void tConfigSpecList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttConfigSpecListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tConfigSpecList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttConfigSpecListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uConfigSpec<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>cPath<td><font face=arial,helvetica color=white>cOwner<td><font face=arial,helvetica color=white>cGroup<td><font face=arial,helvetica color=white>cPerms<td><font face=arial,helvetica color=white>uTemplate<td><font face=arial,helvetica color=white>cNewExec<td><font face=arial,helvetica color=white>cModExec<td><font face=arial,helvetica color=white>cDelExec<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime12=strtoul(field[12],NULL,10);
		char cBuf12[32];
		if(luTime12)
			ctime_r(&luTime12,cBuf12);
		else
			sprintf(cBuf12,"---");
		time_t luTime14=strtoul(field[14],NULL,10);
		char cBuf14[32];
		if(luTime14)
			ctime_r(&luTime14,cBuf14);
		else
			sprintf(cBuf14,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,field[3]
			,field[4]
			,field[5]
			,ForeignKey("tTemplate","cLabel",strtoul(field[6],NULL,10))
			,field[7]
			,field[8]
			,field[9]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[10],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[11],NULL,10))
			,cBuf12
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[13],NULL,10))
			,cBuf14
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tConfigSpecList()


void CreatetConfigSpec(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tConfigSpec ( uConfigSpec INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cPath VARCHAR(127) NOT NULL DEFAULT '', cOwner VARCHAR(32) NOT NULL DEFAULT '', cGroup VARCHAR(32) NOT NULL DEFAULT '', cPerms VARCHAR(16) NOT NULL DEFAULT '', uTemplate INT UNSIGNED NOT NULL DEFAULT 0,index (uTemplate), cNewExec VARCHAR(100) NOT NULL DEFAULT '', cModExec VARCHAR(100) NOT NULL DEFAULT '', cDelExec VARCHAR(100) NOT NULL DEFAULT '' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetConfigSpec()

