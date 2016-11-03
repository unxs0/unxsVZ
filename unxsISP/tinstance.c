/*
FILE
	tInstance source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tinstancefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uInstance: Primary key
static unsigned uInstance=0;
//uClient: Client index
static unsigned uClient=0;
//uProduct: Product
static unsigned uProduct=0;
//cLabel: Instance label
static char cLabel[33]={""};
//uBilled: Last unixtime billed
static time_t uBilled=0;
//uPayment: Invoice and charge with this payment type
static unsigned uPayment=0;
static char cuPaymentPullDown[256]={""};
//uStatus: Product status for this client instance
static unsigned uStatus=0;
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



#define VAR_LIST_tInstance "tInstance.uInstance,tInstance.uClient,tInstance.uProduct,tInstance.cLabel,tInstance.uBilled,tInstance.uPayment,tInstance.uStatus,tInstance.uOwner,tInstance.uCreatedBy,tInstance.uCreatedDate,tInstance.uModBy,tInstance.uModDate"

 //Local only
void Insert_tInstance(void);
void Update_tInstance(char *cRowid);
void ProcesstInstanceListVars(pentry entries[], int x);

 //In tInstancefunc.h file included below
void ExtProcesstInstanceVars(pentry entries[], int x);
void ExttInstanceCommands(pentry entries[], int x);
void ExttInstanceButtons(void);
void ExttInstanceNavBar(void);
void ExttInstanceGetHook(entry gentries[], int x);
void ExttInstanceSelect(void);
void ExttInstanceSelectRow(void);
void ExttInstanceListSelect(void);
void ExttInstanceListFilter(void);
void ExttInstanceAuxTable(void);

#include "tinstancefunc.h"

 //Table Variables Assignment Function
void ProcesstInstanceVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uInstance"))
			sscanf(entries[i].val,"%u",&uInstance);
		else if(!strcmp(entries[i].name,"uClient"))
			sscanf(entries[i].val,"%u",&uClient);
		else if(!strcmp(entries[i].name,"uProduct"))
			sscanf(entries[i].val,"%u",&uProduct);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uBilled"))
			sscanf(entries[i].val,"%lu",&uBilled);
		else if(!strcmp(entries[i].name,"uPayment"))
			sscanf(entries[i].val,"%u",&uPayment);
		else if(!strcmp(entries[i].name,"cuPaymentPullDown"))
		{
			sprintf(cuPaymentPullDown,"%.255s",entries[i].val);
			uPayment=ReadPullDown("tPayment","cLabel",cuPaymentPullDown);
		}
		else if(!strcmp(entries[i].name,"uStatus"))
			sscanf(entries[i].val,"%u",&uStatus);
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
	ExtProcesstInstanceVars(entries,x);

}//ProcesstInstanceVars()


void ProcesstInstanceListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uInstance);
                        guMode=2002;
                        tInstance("");
                }
        }
}//void ProcesstInstanceListVars(pentry entries[], int x)


int tInstanceCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttInstanceCommands(entries,x);

	if(!strcmp(gcFunction,"tInstanceTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tInstanceList();
		}

		//Default
		ProcesstInstanceVars(entries,x);
		tInstance("");
	}
	else if(!strcmp(gcFunction,"tInstanceList"))
	{
		ProcessControlVars(entries,x);
		ProcesstInstanceListVars(entries,x);
		tInstanceList();
	}

	return(0);

}//tInstanceCommands()


void tInstance(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttInstanceSelectRow();
		else
			ExttInstanceSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetInstance();
				unxsISP("New tInstance table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tInstance WHERE uInstance=%u"
						,uInstance);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uInstance);
		sscanf(field[1],"%u",&uClient);
		sscanf(field[2],"%u",&uProduct);
		sprintf(cLabel,"%.32s",field[3]);
		sscanf(field[4],"%lu",&uBilled);
		sscanf(field[5],"%u",&uPayment);
		sscanf(field[6],"%u",&uStatus);
		sscanf(field[7],"%u",&uOwner);
		sscanf(field[8],"%u",&uCreatedBy);
		sscanf(field[9],"%lu",&uCreatedDate);
		sscanf(field[10],"%u",&uModBy);
		sscanf(field[11],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tInstance",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tInstanceTools>");
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

        ExttInstanceButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tInstance Record Data",100);

	if(guMode==2000 || guMode==2002)
		tInstanceInput(1);
	else
		tInstanceInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttInstanceAuxTable();

	Footer_ism3();

}//end of tInstance();


void tInstanceInput(unsigned uMode)
{

//uInstance
	OpenRow(LANG_FL_tInstance_uInstance,"black");
	printf("<input title='%s' type=text name=uInstance value=%u size=16 maxlength=10 "
,LANG_FT_tInstance_uInstance,uInstance);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uInstance value=%u >\n",uInstance);
	}
//uClient
	OpenRow(LANG_FL_tInstance_uClient,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
	else
	{
	printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
//uProduct
	OpenRow(LANG_FL_tInstance_uProduct,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uProduct value=%u >\n",ForeignKey("tProduct","cLabel",uProduct),uProduct);
	}
	else
	{
	printf("%s<input type=hidden name=uProduct value=%u >\n",ForeignKey("tProduct","cLabel",uProduct),uProduct);
	}
//cLabel
	OpenRow(LANG_FL_tInstance_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tInstance_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uBilled
	OpenRow(LANG_FL_tInstance_uBilled,"black");
	if(uBilled)
		printf("<input type=text name=cuBilled value='%s' disabled>\n",ctime(&uBilled));
	else
		printf("<input type=text name=cuBilled value='---' disabled>\n");
	printf("<input type=hidden name=uBilled value=%lu>\n",uBilled);
//uPayment
	OpenRow(LANG_FL_tInstance_uPayment,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tPayment;cuPaymentPullDown","cLabel","cLabel",uPayment,1);
	else
		tTablePullDown("tPayment;cuPaymentPullDown","cLabel","cLabel",uPayment,0);
//uStatus
	OpenRow(LANG_FL_tInstance_uStatus,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uStatus value=%u >\n",ForeignKey("tStatus","cLabel",uStatus),uStatus);
	}
	else
	{
	printf("%s<input type=hidden name=uStatus value=%u >\n",ForeignKey("tStatus","cLabel",uStatus),uStatus);
	}
//uOwner
	OpenRow(LANG_FL_tInstance_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tInstance_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tInstance_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tInstance_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tInstance_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tInstanceInput(unsigned uMode)


void NewtInstance(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uInstance FROM tInstance\
				WHERE uInstance=%u"
							,uInstance);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tInstance("<blink>Record already exists");
		tInstance(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tInstance();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uInstance=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tInstance",uInstance);
	unxsISPLog(uInstance,"tInstance","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uInstance);
	tInstance(gcQuery);
	}

}//NewtInstance(unsigned uMode)


void DeletetInstance(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tInstance WHERE uInstance=%u AND ( uOwner=%u OR %u>9 )"
					,uInstance,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tInstance WHERE uInstance=%u"
					,uInstance);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tInstance("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uInstance,"tInstance","Del");
#endif
		tInstance(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uInstance,"tInstance","DelError");
#endif
		tInstance(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetInstance(void)


void Insert_tInstance(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tInstance SET uInstance=%u,uClient=%u,uProduct=%u,cLabel='%s',uBilled=%lu,uPayment=%u,uStatus=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uInstance
			,uClient
			,uProduct
			,TextAreaSave(cLabel)
			,uBilled
			,uPayment
			,uStatus
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tInstance(void)


void Update_tInstance(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tInstance SET uInstance=%u,uClient=%u,uProduct=%u,cLabel='%s',uBilled=%lu,uPayment=%u,uStatus=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uInstance
			,uClient
			,uProduct
			,TextAreaSave(cLabel)
			,uBilled
			,uPayment
			,uStatus
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tInstance(void)


void ModtInstance(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uInstance,uModDate FROM tInstance WHERE uInstance=%u"
						,uInstance);
#else
	sprintf(gcQuery,"SELECT uInstance FROM tInstance WHERE uInstance=%u"
						,uInstance);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tInstance("<blink>Record does not exist");
	if(i<1) tInstance(LANG_NBR_RECNOTEXIST);
	//if(i>1) tInstance("<blink>Multiple rows!");
	if(i>1) tInstance(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tInstance(LANG_NBR_EXTMOD);
#endif

	Update_tInstance(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tInstance",uInstance);
	unxsISPLog(uInstance,"tInstance","Mod");
#endif
	tInstance(gcQuery);

}//ModtInstance(void)


void tInstanceList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttInstanceListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tInstanceList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttInstanceListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uInstance<td><font face=arial,helvetica color=white>uClient<td><font face=arial,helvetica color=white>uProduct<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uBilled<td><font face=arial,helvetica color=white>uPayment<td><font face=arial,helvetica color=white>uStatus<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime9=strtoul(field[9],NULL,10);
		char cBuf9[32];
		if(luTime9)
			ctime_r(&luTime9,cBuf9);
		else
			sprintf(cBuf9,"---");
		time_t luTime11=strtoul(field[11],NULL,10);
		char cBuf11[32];
		if(luTime11)
			ctime_r(&luTime11,cBuf11);
		else
			sprintf(cBuf11,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[1],NULL,10))
			,ForeignKey("tProduct","cLabel",strtoul(field[2],NULL,10))
			,field[3]
			,cBuf4
			,ForeignKey("tPayment","cLabel",strtoul(field[5],NULL,10))
			,ForeignKey("tStatus","cLabel",strtoul(field[6],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[7],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[8],NULL,10))
			,cBuf9
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[10],NULL,10))
			,cBuf11
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tInstanceList()


void CreatetInstance(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tInstance ( uInstance INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uClient INT UNSIGNED NOT NULL DEFAULT 0,index (uClient), uProduct INT UNSIGNED NOT NULL DEFAULT 0, uBilled INT UNSIGNED NOT NULL DEFAULT 0, uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cLabel VARCHAR(32) NOT NULL DEFAULT '', uStatus INT UNSIGNED NOT NULL DEFAULT 0, uPayment INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetInstance()

