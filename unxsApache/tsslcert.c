/*
FILE
	tSSLCert source code of unxsApache.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tsslcertfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uSSLCert: Primary Key
static unsigned uSSLCert=0;
//cDomain: Certificate DN in FQDN format
static char cDomain[101]={""};
//cCert: Certificate in PEM format
static char *cCert={""};
//cKey: Key in PEM format
static char *cKey={""};
//cRequest: Request for certificate
static char *cRequest={""};
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



#define VAR_LIST_tSSLCert "tSSLCert.uSSLCert,tSSLCert.cDomain,tSSLCert.cCert,tSSLCert.cKey,tSSLCert.cRequest,tSSLCert.uOwner,tSSLCert.uCreatedBy,tSSLCert.uCreatedDate,tSSLCert.uModBy,tSSLCert.uModDate"

 //Local only
void Insert_tSSLCert(void);
void Update_tSSLCert(char *cRowid);
void ProcesstSSLCertListVars(pentry entries[], int x);

 //In tSSLCertfunc.h file included below
void ExtProcesstSSLCertVars(pentry entries[], int x);
void ExttSSLCertCommands(pentry entries[], int x);
void ExttSSLCertButtons(void);
void ExttSSLCertNavBar(void);
void ExttSSLCertGetHook(entry gentries[], int x);
void ExttSSLCertSelect(void);
void ExttSSLCertSelectRow(void);
void ExttSSLCertListSelect(void);
void ExttSSLCertListFilter(void);
void ExttSSLCertAuxTable(void);

#include "tsslcertfunc.h"

 //Table Variables Assignment Function
void ProcesstSSLCertVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uSSLCert"))
			sscanf(entries[i].val,"%u",&uSSLCert);
		else if(!strcmp(entries[i].name,"cDomain"))
			sprintf(cDomain,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCert"))
			cCert=entries[i].val;
		else if(!strcmp(entries[i].name,"cKey"))
			cKey=entries[i].val;
		else if(!strcmp(entries[i].name,"cRequest"))
			cRequest=entries[i].val;
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
	ExtProcesstSSLCertVars(entries,x);

}//ProcesstSSLCertVars()


void ProcesstSSLCertListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uSSLCert);
                        guMode=2002;
                        tSSLCert("");
                }
        }
}//void ProcesstSSLCertListVars(pentry entries[], int x)


int tSSLCertCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttSSLCertCommands(entries,x);

	if(!strcmp(gcFunction,"tSSLCertTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tSSLCertList();
		}

		//Default
		ProcesstSSLCertVars(entries,x);
		tSSLCert("");
	}
	else if(!strcmp(gcFunction,"tSSLCertList"))
	{
		ProcessControlVars(entries,x);
		ProcesstSSLCertListVars(entries,x);
		tSSLCertList();
	}

	return(0);

}//tSSLCertCommands()


void tSSLCert(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttSSLCertSelectRow();
		else
			ExttSSLCertSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetSSLCert();
				unxsApache("New tSSLCert table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tSSLCert WHERE uSSLCert=%u"
						,uSSLCert);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uSSLCert);
		sprintf(cDomain,"%.100s",field[1]);
		cCert=field[2];
		cKey=field[3];
		cRequest=field[4];
		sscanf(field[5],"%u",&uOwner);
		sscanf(field[6],"%u",&uCreatedBy);
		sscanf(field[7],"%lu",&uCreatedDate);
		sscanf(field[8],"%u",&uModBy);
		sscanf(field[9],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tSSLCert",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tSSLCertTools>");
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

        ExttSSLCertButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tSSLCert Record Data",100);

	if(guMode==2000 || guMode==2002)
		tSSLCertInput(1);
	else
		tSSLCertInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttSSLCertAuxTable();

	Footer_ism3();

}//end of tSSLCert();


void tSSLCertInput(unsigned uMode)
{

//uSSLCert
	OpenRow(LANG_FL_tSSLCert_uSSLCert,"black");
	printf("<input title='%s' type=text name=uSSLCert value=%u size=16 maxlength=10 "
,LANG_FT_tSSLCert_uSSLCert,uSSLCert);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uSSLCert value=%u >\n",uSSLCert);
	}
//cDomain
	OpenRow(LANG_FL_tSSLCert_cDomain,"black");
	printf("<input title='%s' type=text name=cDomain value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tSSLCert_cDomain,EncodeDoubleQuotes(cDomain));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cDomain value=\"%s\">\n",EncodeDoubleQuotes(cDomain));
	}
//cCert
	OpenRow(LANG_FL_tSSLCert_cCert,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cCert "
,LANG_FT_tSSLCert_cCert);
	if(guPermLevel>=0 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cCert);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cCert);
		printf("<input type=hidden name=cCert value=\"%s\" >\n",EncodeDoubleQuotes(cCert));
	}
//cKey
	OpenRow(LANG_FL_tSSLCert_cKey,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cKey "
,LANG_FT_tSSLCert_cKey);
	if(guPermLevel>=0 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cKey);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cKey);
		printf("<input type=hidden name=cKey value=\"%s\" >\n",EncodeDoubleQuotes(cKey));
	}
//cRequest
	OpenRow(LANG_FL_tSSLCert_cRequest,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cRequest "
,LANG_FT_tSSLCert_cRequest);
	if(guPermLevel>=0 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cRequest);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cRequest);
		printf("<input type=hidden name=cRequest value=\"%s\" >\n",EncodeDoubleQuotes(cRequest));
	}
//uOwner
	OpenRow(LANG_FL_tSSLCert_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tSSLCert_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tSSLCert_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tSSLCert_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tSSLCert_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tSSLCertInput(unsigned uMode)


void NewtSSLCert(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uSSLCert FROM tSSLCert\
				WHERE uSSLCert=%u"
							,uSSLCert);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tSSLCert("<blink>Record already exists");
		tSSLCert(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tSSLCert();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uSSLCert=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tSSLCert",uSSLCert);
	unxsApacheLog(uSSLCert,"tSSLCert","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uSSLCert);
	tSSLCert(gcQuery);
	}

}//NewtSSLCert(unsigned uMode)


void DeletetSSLCert(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tSSLCert WHERE uSSLCert=%u AND ( uOwner=%u OR %u>9 )"
					,uSSLCert,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tSSLCert WHERE uSSLCert=%u"
					,uSSLCert);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tSSLCert("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsApacheLog(uSSLCert,"tSSLCert","Del");
#endif
		tSSLCert(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsApacheLog(uSSLCert,"tSSLCert","DelError");
#endif
		tSSLCert(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetSSLCert(void)


void Insert_tSSLCert(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tSSLCert SET uSSLCert=%u,cDomain='%s',cCert='%s',cKey='%s',cRequest='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uSSLCert
			,TextAreaSave(cDomain)
			,TextAreaSave(cCert)
			,TextAreaSave(cKey)
			,TextAreaSave(cRequest)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tSSLCert(void)


void Update_tSSLCert(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tSSLCert SET uSSLCert=%u,cDomain='%s',cCert='%s',cKey='%s',cRequest='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uSSLCert
			,TextAreaSave(cDomain)
			,TextAreaSave(cCert)
			,TextAreaSave(cKey)
			,TextAreaSave(cRequest)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tSSLCert(void)


void ModtSSLCert(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT uSSLCert,uModDate FROM tSSLCert WHERE uSSLCert=%u"
						,uSSLCert);
#else
	sprintf(gcQuery,"SELECT uSSLCert FROM tSSLCert WHERE uSSLCert=%u"
						,uSSLCert);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tSSLCert("<blink>Record does not exist");
	if(i<1) tSSLCert(LANG_NBR_RECNOTEXIST);
	//if(i>1) tSSLCert("<blink>Multiple rows!");
	if(i>1) tSSLCert(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tSSLCert(LANG_NBR_EXTMOD);
#endif

	Update_tSSLCert(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tSSLCert",uSSLCert);
	unxsApacheLog(uSSLCert,"tSSLCert","Mod");
#endif
	tSSLCert(gcQuery);

}//ModtSSLCert(void)


void tSSLCertList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttSSLCertListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tSSLCertList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttSSLCertListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uSSLCert<td><font face=arial,helvetica color=white>cDomain<td><font face=arial,helvetica color=white>cCert<td><font face=arial,helvetica color=white>cKey<td><font face=arial,helvetica color=white>cRequest<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td><textarea disabled>%s</textarea><td><textarea disabled>%s</textarea><td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,field[3]
			,field[4]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,cBuf7
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[8],NULL,10))
			,cBuf9
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tSSLCertList()


void CreatetSSLCert(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tSSLCert ( uSSLCert INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cDomain VARCHAR(100) NOT NULL DEFAULT '',unique (cDomain), uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cCert TEXT NOT NULL DEFAULT '', cKey TEXT NOT NULL DEFAULT '', cRequest TEXT NOT NULL DEFAULT '' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		unxsApache(mysql_error(&gMysql));
}//CreatetSSLCert()

