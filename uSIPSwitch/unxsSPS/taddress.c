/*
FILE
	$Id: module.c 2115 2012-09-19 14:11:03Z Gary $
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality can be developed in taddressfunc.h
	while unxsSPS can still to be used to change this schema dependent file.
AUTHOR
	(C) 2001-2012 Gary Wallis for Unixservice, LLC.
TEMPLATE VARS AND FUNCTIONS
	ModuleCreateQuery
	ModuleInsertQuery
	ModuleListPrint
	ModuleListTable
	ModuleLoadVars
	ModuleProcVars
	ModuleInput
	ModuleUpdateQuery
	ModuleVars
	ModuleVarList
	cProject
	cTableKey
	cTableName
	cTableNameLC
	cTableTitle
*/


#include "mysqlrad.h"

//Table Variables
static unsigned uAddress=0;
static char cLabel[65]={""};
static char cIP[33]={""};
static unsigned uPort=0;
static unsigned uPriority=0;
static unsigned uWeight=0;
static unsigned uPBX=0;
static unsigned uGateway=0;
static char cuGatewayPullDown[256]={""};
static unsigned uOwner=0;
#define StandardFields
static unsigned uCreatedBy=0;
static time_t uCreatedDate=0;
static unsigned uModBy=0;
static time_t uModDate=0;
static time_t uHealthCheckedDate=0;
static short unsigned uAvailable=0;
static long long unsigned uUptime=0;
static char cYesNouAvailable[32]={""};


#define VAR_LIST_tAddress "tAddress.uAddress,tAddress.cLabel,tAddress.cIP,tAddress.uPort,tAddress.uPriority,tAddress.uWeight,tAddress.uPBX,tAddress.uGateway,tAddress.uOwner,tAddress.uCreatedBy,tAddress.uCreatedDate,tAddress.uModBy,tAddress.uModDate,tAddress.uHealthCheckedDate,tAddress.uAvailable,tAddress.uUptime"

 //Local only
void Insert_tAddress(void);
void Update_tAddress(char *cRowid);
void ProcesstAddressListVars(pentry entries[], int x);

 //In tAddressfunc.h file included below
void ExtProcesstAddressVars(pentry entries[], int x);
void ExttAddressCommands(pentry entries[], int x);
void ExttAddressButtons(void);
void ExttAddressNavBar(void);
void ExttAddressGetHook(entry gentries[], int x);
void ExttAddressSelect(void);
void ExttAddressSelectRow(void);
void ExttAddressListSelect(void);
void ExttAddressListFilter(void);
void ExttAddressAuxTable(void);

#include "taddressfunc.h"

 //Table Variables Assignment Function
void ProcesstAddressVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		
		if(!strcmp(entries[i].name,"uAddress"))
			sscanf(entries[i].val,"%u",&uAddress);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"cIP"))
			sprintf(cIP,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"uPort"))
			sscanf(entries[i].val,"%u",&uPort);
		else if(!strcmp(entries[i].name,"uPriority"))
			sscanf(entries[i].val,"%u",&uPriority);
		else if(!strcmp(entries[i].name,"uWeight"))
			sscanf(entries[i].val,"%u",&uWeight);
		else if(!strcmp(entries[i].name,"uPBX"))
			sscanf(entries[i].val,"%u",&uPBX);
		else if(!strcmp(entries[i].name,"uGateway"))
			sscanf(entries[i].val,"%u",&uGateway);
		else if(!strcmp(entries[i].name,"cuGatewayPullDown"))
		{
			sprintf(cuGatewayPullDown,"%.255s",entries[i].val);
			uGateway=ReadPullDown("tGateway","cLabel",cuGatewayPullDown);
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
		else if(!strcmp(entries[i].name,"uHealthCheckedDate"))
			sscanf(entries[i].val,"%lu",&uHealthCheckedDate);
		else if(!strcmp(entries[i].name,"uAvailable"))
			sscanf(entries[i].val,"%hi",&uAvailable);
		else if(!strcmp(entries[i].name,"cYesNouAvailable"))
		{
			sprintf(cYesNouAvailable,"%.31s",entries[i].val);
			uAvailable=ReadYesNoPullDown(cYesNouAvailable);
		}

	}

	//After so we can overwrite form data if needed.
	ExtProcesstAddressVars(entries,x);

}//ProcesstAddressVars()


void ProcesstAddressListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uAddress);
                        guMode=2002;
                        tAddress("");
                }
        }
}//void ProcesstAddressListVars(pentry entries[], int x)


int tAddressCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttAddressCommands(entries,x);

	if(!strcmp(gcFunction,"tAddressTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tAddressList();
		}

		//Default
		ProcesstAddressVars(entries,x);
		tAddress("");
	}
	else if(!strcmp(gcFunction,"tAddressList"))
	{
		ProcessControlVars(entries,x);
		ProcesstAddressListVars(entries,x);
		tAddressList();
	}

	return(0);

}//tAddressCommands()


void tAddress(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttAddressSelectRow();
		else
			ExttAddressSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetAddress();
				unxsSPS("New tAddress table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tAddress WHERE uAddress=%u"
						,uAddress);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			
		sscanf(field[0],"%u",&uAddress);
		sprintf(cLabel,"%.64s",field[1]);
		sprintf(cIP,"%.32s",field[2]);
		sscanf(field[3],"%u",&uPort);
		sscanf(field[4],"%u",&uPriority);
		sscanf(field[5],"%u",&uWeight);
		sscanf(field[6],"%u",&uPBX);
		sscanf(field[7],"%u",&uGateway);
		sscanf(field[8],"%u",&uOwner);
		sscanf(field[9],"%u",&uCreatedBy);
		sscanf(field[10],"%lu",&uCreatedDate);
		sscanf(field[11],"%u",&uModBy);
		sscanf(field[12],"%lu",&uModDate);
		sscanf(field[13],"%lu",&uHealthCheckedDate);
		sscanf(field[14],"%hu",&uAvailable);
		sscanf(field[15],"%llu",&uUptime);

		}

	}//Internal Skip

	Header_ism3(":: IP addresses",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tAddressTools>");
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

        ExttAddressButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tAddress Record Data",100);

	if(guMode==2000 || guMode==2002)
		tAddressInput(1);
	else
		tAddressInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttAddressAuxTable();

	Footer_ism3();

}//end of tAddress();


void tAddressInput(unsigned uMode)
{

	
	//uAddress uRADType=1001
	OpenRow(LANG_FL_tAddress_uAddress,"black");
	printf("<input title='%s' type=text name=uAddress id=uAddress value='%u' size=16 maxlength=10 "
		,LANG_FT_tAddress_uAddress,uAddress);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uAddress id=uAddress value='%u' >\n",uAddress);
	}
	//cLabel uRADType=253
	OpenRow(LANG_FL_tAddress_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel id=cLabel value='%s' size=40 maxlength=64 "
		,LANG_FT_tAddress_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel id=cLabel value='%s'>\n",EncodeDoubleQuotes(cLabel));
	}
	//cIP uRADType=253
	OpenRow(LANG_FL_tAddress_cIP,"black");
	printf("<input title='%s' type=text name=cIP id=cIP value='%s' size=40 maxlength=32 "
		,LANG_FT_tAddress_cIP,EncodeDoubleQuotes(cIP));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cIP id=cIP value='%s'>\n",EncodeDoubleQuotes(cIP));
	}
	//uPort uRADType=3
	OpenRow(LANG_FL_tAddress_uPort,"black");
	printf("<input title='%s' type=text name=uPort id=uPort value='%u' size=16 maxlength=10 "
		,LANG_FT_tAddress_uPort,uPort);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uPort id=uPort value='%u' >\n",uPort);
	}
	//uPriority uRADType=3
	OpenRow(LANG_FL_tAddress_uPriority,"black");
	printf("<input title='%s' type=text name=uPriority id=uPriority value='%u' size=16 maxlength=10 "
		,LANG_FT_tAddress_uPriority,uPriority);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uPriority id=uPriority value='%u' >\n",uPriority);
	}
	//uWeight uRADType=3
	OpenRow(LANG_FL_tAddress_uWeight,"black");
	printf("<input title='%s' type=text name=uWeight id=uWeight value='%u' size=16 maxlength=10 "
		,LANG_FT_tAddress_uWeight,uWeight);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uWeight id=uWeight value='%u' >\n",uWeight);
	}
	//uPBX COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tAddress_uPBX,"black");
	if(guPermLevel>=0 && uMode)
		printf("<!--FK AllowMod-->\n<input title='%s' type=text size=16 maxlength=20 name=uPBX id=uPBX value='%u' >\n",LANG_FT_tAddress_uPBX,uPBX);
	else
		printf("<input title='%s' type=text value='%s' size=40 disabled><input type=hidden name='uPBX' value='%u' >\n",LANG_FT_tAddress_uPBX,ForeignKey("tPBX","cLabel",uPBX),uPBX);
	//uGateway COLTYPE_SELECTTABLE
	OpenRow(LANG_FL_tAddress_uGateway,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tGateway;cuGatewayPullDown","cLabel","cLabel",uGateway,1);
	else
		tTablePullDown("tGateway;cuGatewayPullDown","cLabel","cLabel",uGateway,0);
	//uOwner COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tAddress_uOwner,"black");
	printf("%s<input type=hidden name=uOwner id=uOwner value='%u' >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	//uCreatedBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tAddress_uCreatedBy,"black");
	printf("%s<input type=hidden name=uCreatedBy id=uCreatedBy value='%u' >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	//uCreatedDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tAddress_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate id=uCreatedDate value='%lu' >\n",uCreatedDate);
	//uModBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tAddress_uModBy,"black");
	printf("%s<input type=hidden name=uModBy id=uModBy value='%u' >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	//uModDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tAddress_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate id=uModDate value='%lu' >\n",uModDate);
	//uHealthCheckedDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tAddress_uHealthCheckedDate,"black");
	if(uHealthCheckedDate)
		printf("%s\n\n",ctime(&uHealthCheckedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uHealthCheckedDate id=uHealthCheckedDate value='%lu' >\n",uHealthCheckedDate);

	//uAvailable
	OpenRow(LANG_FL_tAddress_uAvailable,"black");
	if(guPermLevel>=10 && uMode)
		YesNoPullDown("uAvailable",uAvailable,1);
	else
		YesNoPullDown("uAvailable",uAvailable,0);

	printf("</tr>\n");

}//void tAddressInput(unsigned uMode)


void NewtAddress(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uAddress FROM tAddress WHERE uAddress=%u",uAddress);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		tAddress(LANG_NBR_RECEXISTS);

	Insert_tAddress();
	uAddress=mysql_insert_id(&gMysql);
#ifdef StandardFields
	uCreatedDate=luGetCreatedDate("tAddress",uAddress);
#endif
	unxsSPSLog(uAddress,"tAddress","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uAddress);
		tAddress(gcQuery);
	}

}//NewtAddress(unsigned uMode)


void DeletetAddress(void)
{
#ifdef StandardFields
	sprintf(gcQuery,"DELETE FROM tAddress WHERE uAddress=%u AND ( uOwner=%u OR %u>9 )"
					,uAddress,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tAddress WHERE uAddress=%u AND %u>9 )"
					,uAddress,guPermLevel);
#endif
	macro_mySQLQueryHTMLError;
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsSPSLog(uAddress,"tAddress","Del");
		tAddress(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsSPSLog(uAddress,"tAddress","DelError");
		tAddress(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetAddress(void)


void Insert_tAddress(void)
{
	sprintf(gcQuery,"INSERT INTO tAddress SET "
		"cLabel='%s',"
		"cIP='%s',"
		"uPort=%u,"
		"uPriority=%u,"
		"uWeight=%u,"
		"uPBX=%u,"
		"uGateway=%u,"
		"uOwner=%u,"
		"uCreatedBy=%u,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,TextAreaSave(cLabel)
			,TextAreaSave(cIP)
			,uPort
			,uPriority
			,uWeight
			,uPBX
			,uGateway
			,uOwner
			,uCreatedBy
		);

	macro_mySQLQueryHTMLError;

}//void Insert_tAddress(void)


void Update_tAddress(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tAddress SET "
		"cLabel='%s',"
		"cIP='%s',"
		"uPort=%u,"
		"uPriority=%u,"
		"uWeight=%u,"
		"uPBX=%u,"
		"uGateway=%u,"
		"uOwner=%u,"
		"uModBy=%u,"
		"uModDate=UNIX_TIMESTAMP(NOW())"
		" WHERE _rowid=%s"
			,TextAreaSave(cLabel)
			,TextAreaSave(cIP)
			,uPort
			,uPriority
			,uWeight
			,uPBX
			,uGateway
			,uOwner
			,uModBy
			,cRowid
		);

	macro_mySQLQueryHTMLError;

}//void Update_tAddress(void)


void ModtAddress(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

#ifdef StandardFields
	unsigned uPreModDate=0;
	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tAddress.uAddress,"
				" tAddress.uModDate"
				" FROM tAddress,tClient"
				" WHERE tAddress.uAddress=%u"
				" AND tAddress.uOwner=tClient.uClient"
				" AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uAddress,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uAddress,uModDate FROM tAddress"
				" WHERE uAddress=%u"
					,uAddress);
#else
	sprintf(gcQuery,"SELECT uAddress FROM tAddress"
				" WHERE uAddress=%u"
					,uAddress);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i<1) tAddress(LANG_NBR_RECNOTEXIST);
	if(i>1) tAddress(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef StandardFields
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tAddress(LANG_NBR_EXTMOD);
#endif

	Update_tAddress(field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef StandardFields
	uModDate=luGetModDate("tAddress",uAddress);
#endif
	unxsSPSLog(uAddress,"tAddress","Mod");
	tAddress(gcQuery);

}//ModtAddress(void)


void tAddressList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttAddressListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tAddressList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttAddressListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uAddress"
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>cIP"
		"<td><font face=arial,helvetica color=white>uPort"
		"<td><font face=arial,helvetica color=white>uPriority"
		"<td><font face=arial,helvetica color=white>uWeight"
		"<td><font face=arial,helvetica color=white>uPBX"
		"<td><font face=arial,helvetica color=white>uGateway"
		"<td><font face=arial,helvetica color=white>uOwner"
		"<td><font face=arial,helvetica color=white>uCreatedBy"
		"<td><font face=arial,helvetica color=white>uCreatedDate"
		"<td><font face=arial,helvetica color=white>uModBy"
		"<td><font face=arial,helvetica color=white>uModDate"
		"<td><font face=arial,helvetica color=white>uHealthCheckedDate"
		"<td><font face=arial,helvetica color=white>uAvailable"
		"<td><font face=arial,helvetica color=white>uUptime"
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
				time_t luTime10=strtoul(field[10],NULL,10);
		char cBuf10[32];
		if(luTime10)
			ctime_r(&luTime10,cBuf10);
		else
			sprintf(cBuf10,"---");
		time_t luTime12=strtoul(field[12],NULL,10);
		char cBuf12[32];
		if(luTime12)
			ctime_r(&luTime12,cBuf12);
		else
			sprintf(cBuf12,"---");
		time_t luTime13=strtoul(field[13],NULL,10);
		char cBuf13[32];
		if(luTime13)
			ctime_r(&luTime13,cBuf13);
		else
			sprintf(cBuf13,"---");
		printf("<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tAddress&uAddress=%s>%s</a>"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tPBX&uPBX=%s>%s</a>"
				"<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tGateway&uGateway=%s>%s</a>"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s</tr>"
			,field[0],field[0]
			,field[1]
			,field[2]
			,field[3]
			,field[4]
			,field[5]
			,field[6],ForeignKey("tPBX","cLabel",strtoul(field[6],NULL,10))
			,field[7],ForeignKey("tGateway","cLabel",strtoul(field[7],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[8],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[9],NULL,10))
			,cBuf10
			,ForeignKey("tClient","cLabel",strtoul(field[11],NULL,10))
			,cBuf12
			,cBuf13
			,field[14]
			,field[15]
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tAddressList()


void CreatetAddress(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tAddress ("
		"uAddress INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
		"cLabel VARCHAR(64) NOT NULL DEFAULT '', INDEX (cLabel),"
		"cIP VARCHAR(32) NOT NULL DEFAULT '',"
		"uPort INT UNSIGNED NOT NULL DEFAULT 0,"
		"uPriority INT UNSIGNED NOT NULL DEFAULT 0,"
		"uWeight INT UNSIGNED NOT NULL DEFAULT 0,"
		"uPBX INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uPBX),"
		"uGateway INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uGateway),"
		"uOwner INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uHealthCheckedDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uAvailable TINYINT UNSIGNED NOT NULL DEFAULT 1,"
		"uUptime BIGINT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void CreatetAddress(void)


