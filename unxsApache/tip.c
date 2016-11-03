/*
FILE
	tIP source code of unxsApache.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tipfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uIP: Primary key and IP alias number
static unsigned uIP=0;
//cIP: IP Number
static char cIP[17]={""};
//uServer: Pull down for target server
static unsigned uServer=0;
static char cuServerPullDown[256]={""};
//uAvailable: Number available for websites
static unsigned uAvailable=0;
static char cYesNouAvailable[32]={""};
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



#define VAR_LIST_tIP "tIP.uIP,tIP.cIP,tIP.uServer,tIP.uAvailable,tIP.uOwner,tIP.uCreatedBy,tIP.uCreatedDate,tIP.uModBy,tIP.uModDate"

 //Local only
void Insert_tIP(void);
void Update_tIP(char *cRowid);
void ProcesstIPListVars(pentry entries[], int x);

 //In tIPfunc.h file included below
void ExtProcesstIPVars(pentry entries[], int x);
void ExttIPCommands(pentry entries[], int x);
void ExttIPButtons(void);
void ExttIPNavBar(void);
void ExttIPGetHook(entry gentries[], int x);
void ExttIPSelect(void);
void ExttIPSelectRow(void);
void ExttIPListSelect(void);
void ExttIPListFilter(void);
void ExttIPAuxTable(void);

#include "tipfunc.h"

 //Table Variables Assignment Function
void ProcesstIPVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uIP"))
			sscanf(entries[i].val,"%u",&uIP);
		else if(!strcmp(entries[i].name,"cIP"))
			sprintf(cIP,"%.16s",entries[i].val);
		else if(!strcmp(entries[i].name,"uServer"))
			sscanf(entries[i].val,"%u",&uServer);
		else if(!strcmp(entries[i].name,"cuServerPullDown"))
		{
			sprintf(cuServerPullDown,"%.255s",entries[i].val);
			uServer=ReadPullDown("tServer","cLabel",cuServerPullDown);
		}
		else if(!strcmp(entries[i].name,"uAvailable"))
			sscanf(entries[i].val,"%u",&uAvailable);
		else if(!strcmp(entries[i].name,"cYesNouAvailable"))
		{
			sprintf(cYesNouAvailable,"%.31s",entries[i].val);
			uAvailable=ReadYesNoPullDown(cYesNouAvailable);
		}
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
	ExtProcesstIPVars(entries,x);

}//ProcesstIPVars()


void ProcesstIPListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uIP);
                        guMode=2002;
                        tIP("");
                }
        }
}//void ProcesstIPListVars(pentry entries[], int x)


int tIPCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttIPCommands(entries,x);

	if(!strcmp(gcFunction,"tIPTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tIPList();
		}

		//Default
		ProcesstIPVars(entries,x);
		tIP("");
	}
	else if(!strcmp(gcFunction,"tIPList"))
	{
		ProcessControlVars(entries,x);
		ProcesstIPListVars(entries,x);
		tIPList();
	}

	return(0);

}//tIPCommands()


void tIP(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttIPSelectRow();
		else
			ExttIPSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetIP();
				unxsApache("New tIP table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tIP WHERE uIP=%u"
						,uIP);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uIP);
		sprintf(cIP,"%.16s",field[1]);
		sscanf(field[2],"%u",&uServer);
		sscanf(field[3],"%u",&uAvailable);
		sscanf(field[4],"%u",&uOwner);
		sscanf(field[5],"%u",&uCreatedBy);
		sscanf(field[6],"%lu",&uCreatedDate);
		sscanf(field[7],"%u",&uModBy);
		sscanf(field[8],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tIP",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tIPTools>");
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

        ExttIPButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tIP Record Data",100);

	if(guMode==2000 || guMode==2002)
		tIPInput(1);
	else
		tIPInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttIPAuxTable();

	Footer_ism3();

}//end of tIP();


void tIPInput(unsigned uMode)
{

//uIP
	OpenRow(LANG_FL_tIP_uIP,"black");
	printf("<input title='%s' type=text name=uIP value=%u size=16 maxlength=10 "
,LANG_FT_tIP_uIP,uIP);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uIP value=%u >\n",uIP);
	}
//cIP
	OpenRow(LANG_FL_tIP_cIP,"black");
	printf("<input title='%s' type=text name=cIP value=\"%s\" size=40 maxlength=20 "
,LANG_FT_tIP_cIP,EncodeDoubleQuotes(cIP));
	if(guPermLevel>=9 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cIP value=\"%s\">\n",EncodeDoubleQuotes(cIP));
	}
//uServer
	OpenRow(LANG_FL_tIP_uServer,"black");
	if(guPermLevel>=9 && uMode)
		tTablePullDown("tServer;cuServerPullDown","cLabel","cLabel",uServer,1);
	else
		tTablePullDown("tServer;cuServerPullDown","cLabel","cLabel",uServer,0);
//uAvailable
	OpenRow(LANG_FL_tIP_uAvailable,"black");
	if(guPermLevel>=9 && uMode)
		YesNoPullDown("uAvailable",uAvailable,1);
	else
		YesNoPullDown("uAvailable",uAvailable,0);
//uOwner
	OpenRow(LANG_FL_tIP_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tIP_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tIP_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tIP_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tIP_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tIPInput(unsigned uMode)


void NewtIP(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uIP FROM tIP\
				WHERE uIP=%u"
							,uIP);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tIP("<blink>Record already exists");
		tIP(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tIP();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uIP=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tIP",uIP);
	unxsApacheLog(uIP,"tIP","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uIP);
	tIP(gcQuery);
	}

}//NewtIP(unsigned uMode)


void DeletetIP(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tIP WHERE uIP=%u AND ( uOwner=%u OR %u>9 )"
					,uIP,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tIP WHERE uIP=%u"
					,uIP);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tIP("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsApacheLog(uIP,"tIP","Del");
#endif
		tIP(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsApacheLog(uIP,"tIP","DelError");
#endif
		tIP(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetIP(void)


void Insert_tIP(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tIP SET uIP=%u,cIP='%s',uServer=%u,uAvailable=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uIP
			,TextAreaSave(cIP)
			,uServer
			,uAvailable
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tIP(void)


void Update_tIP(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tIP SET uIP=%u,cIP='%s',uServer=%u,uAvailable=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uIP
			,TextAreaSave(cIP)
			,uServer
			,uAvailable
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tIP(void)


void ModtIP(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	sprintf(gcQuery,"SELECT uIP,uModDate FROM tIP WHERE uIP=%u"
						,uIP);
#else
	sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uIP=%u"
						,uIP);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tIP("<blink>Record does not exist");
	if(i<1) tIP(LANG_NBR_RECNOTEXIST);
	//if(i>1) tIP("<blink>Multiple rows!");
	if(i>1) tIP(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tIP(LANG_NBR_EXTMOD);
#endif

	Update_tIP(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tIP",uIP);
	unxsApacheLog(uIP,"tIP","Mod");
#endif
	tIP(gcQuery);

}//ModtIP(void)


void tIPList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttIPListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tIPList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttIPListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uIP<td><font face=arial,helvetica color=white>cIP<td><font face=arial,helvetica color=white>uServer<td><font face=arial,helvetica color=white>uAvailable<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime6=strtoul(field[6],NULL,10);
		char cBuf6[32];
		if(luTime6)
			ctime_r(&luTime6,cBuf6);
		else
			sprintf(cBuf6,"---");
		time_t luTime8=strtoul(field[8],NULL,10);
		char cBuf8[32];
		if(luTime8)
			ctime_r(&luTime8,cBuf8);
		else
			sprintf(cBuf8,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey("tServer","cLabel",strtoul(field[2],NULL,10))
			,cBuf3
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[4],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,cBuf6
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[7],NULL,10))
			,cBuf8
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tIPList()


void CreatetIP(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tIP ( uIP INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cIP VARCHAR(16) NOT NULL DEFAULT '', UNIQUE (cIP), uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uAvailable INT UNSIGNED NOT NULL DEFAULT 0, uServer INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetIP()

