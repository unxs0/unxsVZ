/*
FILE
	tDomain source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tdomainfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uDomain: Primary Key
static unsigned uDomain=0;
//cDomain: FQ Domain Name
static char cDomain[65]={""};
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



#define VAR_LIST_tDomain "tDomain.uDomain,tDomain.cDomain,tDomain.uOwner,tDomain.uCreatedBy,tDomain.uCreatedDate,tDomain.uModBy,tDomain.uModDate"

 //Local only
void Insert_tDomain(void);
void Update_tDomain(char *cRowid);
void ProcesstDomainListVars(pentry entries[], int x);

 //In tDomainfunc.h file included below
void ExtProcesstDomainVars(pentry entries[], int x);
void ExttDomainCommands(pentry entries[], int x);
void ExttDomainButtons(void);
void ExttDomainNavBar(void);
void ExttDomainGetHook(entry gentries[], int x);
void ExttDomainSelect(void);
void ExttDomainSelectRow(void);
void ExttDomainListSelect(void);
void ExttDomainListFilter(void);
void ExttDomainAuxTable(void);

#include "tdomainfunc.h"

 //Table Variables Assignment Function
void ProcesstDomainVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uDomain"))
			sscanf(entries[i].val,"%u",&uDomain);
		else if(!strcmp(entries[i].name,"cDomain"))
			sprintf(cDomain,"%.64s",FQDomainName(entries[i].val));
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
	ExtProcesstDomainVars(entries,x);

}//ProcesstDomainVars()


void ProcesstDomainListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uDomain);
                        guMode=2002;
                        tDomain("");
                }
        }
}//void ProcesstDomainListVars(pentry entries[], int x)


int tDomainCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttDomainCommands(entries,x);

	if(!strcmp(gcFunction,"tDomainTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tDomainList();
		}

		//Default
		ProcesstDomainVars(entries,x);
		tDomain("");
	}
	else if(!strcmp(gcFunction,"tDomainList"))
	{
		ProcessControlVars(entries,x);
		ProcesstDomainListVars(entries,x);
		tDomainList();
	}

	return(0);

}//tDomainCommands()


void tDomain(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttDomainSelectRow();
		else
			ExttDomainSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetDomain();
				unxsMail("New tDomain table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tDomain WHERE uDomain=%u"
						,uDomain);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uDomain);
		sprintf(cDomain,"%.64s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tDomain",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tDomainTools>");
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

        ExttDomainButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tDomain Record Data",100);

	if(guMode==2000 || guMode==2002)
		tDomainInput(1);
	else
		tDomainInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttDomainAuxTable();

	Footer_ism3();

}//end of tDomain();


void tDomainInput(unsigned uMode)
{

//uDomain
	OpenRow(LANG_FL_tDomain_uDomain,"black");
	printf("<input title='%s' type=text name=uDomain value=%u size=16 maxlength=10 "
,LANG_FT_tDomain_uDomain,uDomain);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uDomain value=%u >\n",uDomain);
	}
//cDomain
	OpenRow(LANG_FL_tDomain_cDomain,"black");
	printf("<input title='%s' type=text name=cDomain value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tDomain_cDomain,EncodeDoubleQuotes(cDomain));
	if(guPermLevel>=8 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cDomain value=\"%s\">\n",EncodeDoubleQuotes(cDomain));
	}
//uOwner
	OpenRow(LANG_FL_tDomain_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tDomain_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tDomain_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tDomain_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tDomain_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tDomainInput(unsigned uMode)


void NewtDomain(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uDomain FROM tDomain\
				WHERE uDomain=%u"
							,uDomain);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tDomain("<blink>Record already exists");
		tDomain(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tDomain();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uDomain=mysql_insert_id(&gMysql);

	uCreatedDate=luGetCreatedDate("tDomain",uDomain);
	unxsMailLog(uDomain,"tDomain","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uDomain);
	tDomain(gcQuery);
	}

}//NewtDomain(unsigned uMode)


void DeletetDomain(void)
{

	sprintf(gcQuery,"DELETE FROM tDomain WHERE uDomain=%u AND ( uOwner=%u OR %u>9 )"
					,uDomain,guLoginClient,guPermLevel);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tDomain("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uDomain,"tDomain","Del");

		tDomain(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uDomain,"tDomain","DelError");

		tDomain(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetDomain(void)


void Insert_tDomain(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tDomain SET uDomain=%u,cDomain='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uDomain
			,TextAreaSave(cDomain)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tDomain(void)


void Update_tDomain(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tDomain SET uDomain=%u,cDomain='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uDomain
			,TextAreaSave(cDomain)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tDomain(void)


void ModtDomain(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uDomain,uModDate FROM tDomain WHERE uDomain=%u"
						,uDomain);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tDomain("<blink>Record does not exist");
	if(i<1) tDomain(LANG_NBR_RECNOTEXIST);
	//if(i>1) tDomain("<blink>Multiple rows!");
	if(i>1) tDomain(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tDomain(LANG_NBR_EXTMOD);


	Update_tDomain(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);

	uModDate=luGetModDate("tDomain",uDomain);
	unxsMailLog(uDomain,"tDomain","Mod");

	tDomain(gcQuery);

}//ModtDomain(void)


void tDomainList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttDomainListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tDomainList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttDomainListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uDomain<td><font face=arial,helvetica color=white>cDomain<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime4=strtoul(field[4],NULL,10);
		char cBuf4[32];
		if(luTime4)
			ctime_r(&luTime4,cBuf4);
		else
			sprintf(cBuf4,"---");
		time_t luTime6=strtoul(field[6],NULL,10);
		char cBuf6[32];
		if(luTime6)
			ctime_r(&luTime6,cBuf6);
		else
			sprintf(cBuf6,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[2],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[3],NULL,10))
			,cBuf4
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,cBuf6
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tDomainList()


void CreatetDomain(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tDomain ( uModDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uDomain INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cDomain VARCHAR(64) NOT NULL DEFAULT '', UNIQUE (cDomain) )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetDomain()

