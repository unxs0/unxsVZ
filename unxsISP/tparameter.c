/*
FILE
	tParameter source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tparameterfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uParameter: Primary Key
static unsigned uParameter=0;
//cParameter: Parameter name
static char cParameter[65]={""};
//uParamType: Parameter type
static unsigned uParamType=0;
static char cuParamTypePullDown[256]={""};
//cRange: Parameter range
static char cRange[17]={""};
//uUnique: Enforce unique value in tClientConfig
static unsigned uUnique=0;
static char cYesNouUnique[32]={""};
//cExtVerify: External synchronous database verification server and query
static char cExtVerify[256]={""};
//uModPostDeploy: Allow after deployment modification
static unsigned uModPostDeploy=0;
static char cYesNouModPostDeploy[32]={""};
//uAllowMod: Allow user modification of parameter value
static unsigned _uAllowMod=0;
static char cYesNouAllowMod[32]={""};
//uISMOrder: The display order for the field
static unsigned uISMOrder=0;
//uISMHide: If TRUE=YES then will not appear on new ism3 form
static unsigned uISMHide=0;
static char cYesNouISMHide[32]={""};
//cISMName: ism3 field name displayed for product mod form
static char cISMName[65]={""};
//cISMHelp: ism3 field help (title) for product mod form
static char cISMHelp[65]={""};
//uProductType: Main product-service type
static unsigned uProductType=0;
static char cuProductTypePullDown[256]={""};
//cExtQuery: External query for extselect parameters
static char cExtQuery[256]={""};
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



#define VAR_LIST_tParameter "tParameter.uParameter,tParameter.cParameter,tParameter.uParamType,tParameter.cRange,tParameter.uUnique,tParameter.cExtVerify,tParameter.uModPostDeploy,tParameter.uAllowMod,tParameter.uISMOrder,tParameter.uISMHide,tParameter.cISMName,tParameter.cISMHelp,tParameter.uProductType,tParameter.cExtQuery,tParameter.uOwner,tParameter.uCreatedBy,tParameter.uCreatedDate,tParameter.uModBy,tParameter.uModDate"

 //Local only
void Insert_tParameter(void);
void Update_tParameter(char *cRowid);
void ProcesstParameterListVars(pentry entries[], int x);

 //In tParameterfunc.h file included below
void ExtProcesstParameterVars(pentry entries[], int x);
void ExttParameterCommands(pentry entries[], int x);
void ExttParameterButtons(void);
void ExttParameterNavBar(void);
void ExttParameterGetHook(entry gentries[], int x);
void ExttParameterSelect(void);
void ExttParameterSelectRow(void);
void ExttParameterListSelect(void);
void ExttParameterListFilter(void);
void ExttParameterAuxTable(void);

#include "tparameterfunc.h"

 //Table Variables Assignment Function
void ProcesstParameterVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uParameter"))
			sscanf(entries[i].val,"%u",&uParameter);
		else if(!strcmp(entries[i].name,"cParameter"))
			sprintf(cParameter,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uParamType"))
			sscanf(entries[i].val,"%u",&uParamType);
		else if(!strcmp(entries[i].name,"cuParamTypePullDown"))
		{
			sprintf(cuParamTypePullDown,"%.255s",entries[i].val);
			uParamType=ReadPullDown("tParamType","cLabel",cuParamTypePullDown);
		}
		else if(!strcmp(entries[i].name,"cRange"))
			sprintf(cRange,"%.16s",entries[i].val);
		else if(!strcmp(entries[i].name,"uUnique"))
			sscanf(entries[i].val,"%u",&uUnique);
		else if(!strcmp(entries[i].name,"cYesNouUnique"))
		{
			sprintf(cYesNouUnique,"%.31s",entries[i].val);
			uUnique=ReadYesNoPullDown(cYesNouUnique);
		}
		else if(!strcmp(entries[i].name,"cExtVerify"))
			sprintf(cExtVerify,"%.255s",entries[i].val);
		else if(!strcmp(entries[i].name,"uModPostDeploy"))
			sscanf(entries[i].val,"%u",&uModPostDeploy);
		else if(!strcmp(entries[i].name,"cYesNouModPostDeploy"))
		{
			sprintf(cYesNouModPostDeploy,"%.31s",entries[i].val);
			uModPostDeploy=ReadYesNoPullDown(cYesNouModPostDeploy);
		}
		else if(!strcmp(entries[i].name,"uAllowMod"))
			sscanf(entries[i].val,"%u",&_uAllowMod);
		else if(!strcmp(entries[i].name,"cYesNouAllowMod"))
		{
			sprintf(cYesNouAllowMod,"%.31s",entries[i].val);
			_uAllowMod=ReadYesNoPullDown(cYesNouAllowMod);
		}
		else if(!strcmp(entries[i].name,"uISMOrder"))
			sscanf(entries[i].val,"%u",&uISMOrder);
		else if(!strcmp(entries[i].name,"uISMHide"))
			sscanf(entries[i].val,"%u",&uISMHide);
		else if(!strcmp(entries[i].name,"cYesNouISMHide"))
		{
			sprintf(cYesNouISMHide,"%.31s",entries[i].val);
			uISMHide=ReadYesNoPullDown(cYesNouISMHide);
		}
		else if(!strcmp(entries[i].name,"cISMName"))
			sprintf(cISMName,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cISMHelp"))
			sprintf(cISMHelp,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uProductType"))
			sscanf(entries[i].val,"%u",&uProductType);
		else if(!strcmp(entries[i].name,"cuProductTypePullDown"))
		{
			sprintf(cuProductTypePullDown,"%.255s",entries[i].val);
			uProductType=ReadPullDown("tProductType","cLabel",cuProductTypePullDown);
		}
		else if(!strcmp(entries[i].name,"cExtQuery"))
			sprintf(cExtQuery,"%.255s",entries[i].val);
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
	ExtProcesstParameterVars(entries,x);

}//ProcesstParameterVars()


void ProcesstParameterListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uParameter);
                        guMode=2002;
                        tParameter("");
                }
        }
}//void ProcesstParameterListVars(pentry entries[], int x)


int tParameterCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttParameterCommands(entries,x);

	if(!strcmp(gcFunction,"tParameterTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tParameterList();
		}

		//Default
		ProcesstParameterVars(entries,x);
		tParameter("");
	}
	else if(!strcmp(gcFunction,"tParameterList"))
	{
		ProcessControlVars(entries,x);
		ProcesstParameterListVars(entries,x);
		tParameterList();
	}

	return(0);

}//tParameterCommands()


void tParameter(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttParameterSelectRow();
		else
			ExttParameterSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetParameter();
				unxsISP("New tParameter table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tParameter WHERE uParameter=%u"
						,uParameter);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uParameter);
		sprintf(cParameter,"%.64s",field[1]);
		sscanf(field[2],"%u",&uParamType);
		sprintf(cRange,"%.16s",field[3]);
		sscanf(field[4],"%u",&uUnique);
		sprintf(cExtVerify,"%.255s",field[5]);
		sscanf(field[6],"%u",&uModPostDeploy);
		sscanf(field[7],"%u",&_uAllowMod);
		sscanf(field[8],"%u",&uISMOrder);
		sscanf(field[9],"%u",&uISMHide);
		sprintf(cISMName,"%.64s",field[10]);
		sprintf(cISMHelp,"%.64s",field[11]);
		sscanf(field[12],"%u",&uProductType);
		sprintf(cExtQuery,"%.255s",field[13]);
		sscanf(field[14],"%u",&uOwner);
		sscanf(field[15],"%u",&uCreatedBy);
		sscanf(field[16],"%lu",&uCreatedDate);
		sscanf(field[17],"%u",&uModBy);
		sscanf(field[18],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tParameter",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tParameterTools>");
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

        ExttParameterButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tParameter Record Data",100);

	if(guMode==2000 || guMode==2002)
		tParameterInput(1);
	else
		tParameterInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttParameterAuxTable();

	Footer_ism3();

}//end of tParameter();


void tParameterInput(unsigned uMode)
{

//uParameter
	OpenRow(LANG_FL_tParameter_uParameter,"black");
	printf("<input title='%s' type=text name=uParameter value=%u size=16 maxlength=10 "
,LANG_FT_tParameter_uParameter,uParameter);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uParameter value=%u >\n",uParameter);
	}
//cParameter
	OpenRow(LANG_FL_tParameter_cParameter,"black");
	printf("<input title='%s' type=text name=cParameter value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tParameter_cParameter,EncodeDoubleQuotes(cParameter));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cParameter value=\"%s\">\n",EncodeDoubleQuotes(cParameter));
	}
//uParamType
	OpenRow(LANG_FL_tParameter_uParamType,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tParamType;cuParamTypePullDown","cLabel","cLabel",uParamType,1);
	else
		tTablePullDown("tParamType;cuParamTypePullDown","cLabel","cLabel",uParamType,0);
//cRange
	OpenRow(LANG_FL_tParameter_cRange,"black");
	printf("<input title='%s' type=text name=cRange value=\"%s\" size=40 maxlength=16 "
,LANG_FT_tParameter_cRange,EncodeDoubleQuotes(cRange));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cRange value=\"%s\">\n",EncodeDoubleQuotes(cRange));
	}
//uUnique
	OpenRow(LANG_FL_tParameter_uUnique,"black");
	if(guPermLevel>=0 && uMode)
		YesNoPullDown("uUnique",uUnique,1);
	else
		YesNoPullDown("uUnique",uUnique,0);
//cExtVerify
	OpenRow(LANG_FL_tParameter_cExtVerify,"black");
	printf("<input title='%s' type=text name=cExtVerify value=\"%s\" size=100 maxlength=255 "
,LANG_FT_tParameter_cExtVerify,EncodeDoubleQuotes(cExtVerify));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cExtVerify value=\"%s\">\n",EncodeDoubleQuotes(cExtVerify));
	}
//uModPostDeploy
	OpenRow(LANG_FL_tParameter_uModPostDeploy,"black");
	if(guPermLevel>=0 && uMode)
		YesNoPullDown("uModPostDeploy",uModPostDeploy,1);
	else
		YesNoPullDown("uModPostDeploy",uModPostDeploy,0);
//uAllowMod
	OpenRow(LANG_FL_tParameter_uAllowMod,"black");
	if(guPermLevel>=0 && uMode)
		YesNoPullDown("uAllowMod",_uAllowMod,1);
	else
		YesNoPullDown("uAllowMod",_uAllowMod,0);
//uISMOrder
	OpenRow(LANG_FL_tParameter_uISMOrder,"black");
	printf("<input title='%s' type=text name=uISMOrder value=%u size=16 maxlength=10 "
,LANG_FT_tParameter_uISMOrder,uISMOrder);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uISMOrder value=%u >\n",uISMOrder);
	}
//uISMHide
	OpenRow(LANG_FL_tParameter_uISMHide,"black");
	if(guPermLevel>=7 && uMode)
		YesNoPullDown("uISMHide",uISMHide,1);
	else
		YesNoPullDown("uISMHide",uISMHide,0);
//cISMName
	OpenRow(LANG_FL_tParameter_cISMName,"black");
	printf("<input title='%s' type=text name=cISMName value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tParameter_cISMName,EncodeDoubleQuotes(cISMName));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cISMName value=\"%s\">\n",EncodeDoubleQuotes(cISMName));
	}
//cISMHelp
	OpenRow(LANG_FL_tParameter_cISMHelp,"black");
	printf("<input title='%s' type=text name=cISMHelp value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tParameter_cISMHelp,EncodeDoubleQuotes(cISMHelp));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cISMHelp value=\"%s\">\n",EncodeDoubleQuotes(cISMHelp));
	}
//uProductType
	OpenRow(LANG_FL_tParameter_uProductType,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDown("tProductType;cuProductTypePullDown","cLabel","cLabel",uProductType,1);
	else
		tTablePullDown("tProductType;cuProductTypePullDown","cLabel","cLabel",uProductType,0);
//cExtQuery
	OpenRow(LANG_FL_tParameter_cExtQuery,"black");
	printf("<input title='%s' type=text name=cExtQuery value=\"%s\" size=40 maxlength=255 "
,LANG_FT_tParameter_cExtQuery,EncodeDoubleQuotes(cExtQuery));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cExtQuery value=\"%s\">\n",EncodeDoubleQuotes(cExtQuery));
	}
//uOwner
	OpenRow(LANG_FL_tParameter_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tParameter_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tParameter_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tParameter_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tParameter_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tParameterInput(unsigned uMode)


void NewtParameter(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uParameter FROM tParameter\
				WHERE uParameter=%u"
							,uParameter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tParameter("<blink>Record already exists");
		tParameter(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tParameter();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uParameter=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tParameter",uParameter);
	unxsISPLog(uParameter,"tParameter","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uParameter);
	tParameter(gcQuery);
	}

}//NewtParameter(unsigned uMode)


void DeletetParameter(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tParameter WHERE uParameter=%u AND ( uOwner=%u OR %u>9 )"
					,uParameter,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tParameter WHERE uParameter=%u"
					,uParameter);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tParameter("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uParameter,"tParameter","Del");
#endif
		tParameter(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uParameter,"tParameter","DelError");
#endif
		tParameter(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetParameter(void)


void Insert_tParameter(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tParameter SET uParameter=%u,cParameter='%s',uParamType=%u,cRange='%s',uUnique=%u,cExtVerify='%s',uModPostDeploy=%u,uAllowMod=%u,uISMOrder=%u,uISMHide=%u,cISMName='%s',cISMHelp='%s',uProductType=%u,cExtQuery='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uParameter
			,TextAreaSave(cParameter)
			,uParamType
			,TextAreaSave(cRange)
			,uUnique
			,TextAreaSave(cExtVerify)
			,uModPostDeploy
			,_uAllowMod
			,uISMOrder
			,uISMHide
			,TextAreaSave(cISMName)
			,TextAreaSave(cISMHelp)
			,uProductType
			,TextAreaSave(cExtQuery)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tParameter(void)


void Update_tParameter(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tParameter SET uParameter=%u,cParameter='%s',uParamType=%u,cRange='%s',uUnique=%u,cExtVerify='%s',uModPostDeploy=%u,uAllowMod=%u,uISMOrder=%u,uISMHide=%u,cISMName='%s',cISMHelp='%s',uProductType=%u,cExtQuery='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uParameter
			,TextAreaSave(cParameter)
			,uParamType
			,TextAreaSave(cRange)
			,uUnique
			,TextAreaSave(cExtVerify)
			,uModPostDeploy
			,_uAllowMod
			,uISMOrder
			,uISMHide
			,TextAreaSave(cISMName)
			,TextAreaSave(cISMHelp)
			,uProductType
			,TextAreaSave(cExtQuery)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tParameter(void)


void ModtParameter(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uParameter,uModDate FROM tParameter WHERE uParameter=%u"
						,uParameter);
#else
	sprintf(gcQuery,"SELECT uParameter FROM tParameter WHERE uParameter=%u"
						,uParameter);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tParameter("<blink>Record does not exist");
	if(i<1) tParameter(LANG_NBR_RECNOTEXIST);
	//if(i>1) tParameter("<blink>Multiple rows!");
	if(i>1) tParameter(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tParameter(LANG_NBR_EXTMOD);
#endif

	Update_tParameter(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tParameter",uParameter);
	unxsISPLog(uParameter,"tParameter","Mod");
#endif
	tParameter(gcQuery);

}//ModtParameter(void)


void tParameterList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttParameterListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tParameterList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttParameterListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uParameter<td><font face=arial,helvetica color=white>cParameter<td><font face=arial,helvetica color=white>uParamType<td><font face=arial,helvetica color=white>cRange<td><font face=arial,helvetica color=white>uUnique<td><font face=arial,helvetica color=white>cExtVerify<td><font face=arial,helvetica color=white>uModPostDeploy<td><font face=arial,helvetica color=white>uAllowMod<td><font face=arial,helvetica color=white>uISMOrder<td><font face=arial,helvetica color=white>uISMHide<td><font face=arial,helvetica color=white>cISMName<td><font face=arial,helvetica color=white>cISMHelp<td><font face=arial,helvetica color=white>uProductType<td><font face=arial,helvetica color=white>cExtQuery<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luYesNo7=strtoul(field[7],NULL,10);
		char cBuf7[4];
		if(luYesNo7)
			sprintf(cBuf7,"Yes");
		else
			sprintf(cBuf7,"No");
		time_t luYesNo9=strtoul(field[9],NULL,10);
		char cBuf9[4];
		if(luYesNo9)
			sprintf(cBuf9,"Yes");
		else
			sprintf(cBuf9,"No");
		time_t luTime16=strtoul(field[16],NULL,10);
		char cBuf16[32];
		if(luTime16)
			ctime_r(&luTime16,cBuf16);
		else
			sprintf(cBuf16,"---");
		time_t luTime18=strtoul(field[18],NULL,10);
		char cBuf18[32];
		if(luTime18)
			ctime_r(&luTime18,cBuf18);
		else
			sprintf(cBuf18,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey("tParamType","cLabel",strtoul(field[2],NULL,10))
			,field[3]
			,cBuf4
			,field[5]
			,cBuf6
			,cBuf7
			,field[8]
			,cBuf9
			,field[10]
			,field[11]
			,ForeignKey("tProductType","cLabel",strtoul(field[12],NULL,10))
			,field[13]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[14],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[15],NULL,10))
			,cBuf16
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[17],NULL,10))
			,cBuf18
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tParameterList()


void CreatetParameter(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tParameter ( uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), cParameter VARCHAR(64) NOT NULL DEFAULT '', UNIQUE (cParameter), uParameter INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uParamType INT UNSIGNED NOT NULL DEFAULT 0, cRange VARCHAR(16) NOT NULL DEFAULT '', uUnique INT UNSIGNED NOT NULL DEFAULT 0, cExtVerify VARCHAR(255) NOT NULL DEFAULT '', uModPostDeploy INT UNSIGNED NOT NULL DEFAULT 0, uAllowMod INT UNSIGNED NOT NULL DEFAULT 0, uISMHide INT UNSIGNED NOT NULL DEFAULT 0,index (uISMHide), cISMName VARCHAR(64) NOT NULL DEFAULT '', cISMHelp VARCHAR(64) NOT NULL DEFAULT '', uProductType INT UNSIGNED NOT NULL DEFAULT 0, uISMOrder INT UNSIGNED NOT NULL DEFAULT 0, cExtQuery VARCHAR(255) NOT NULL DEFAULT '' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetParameter()

