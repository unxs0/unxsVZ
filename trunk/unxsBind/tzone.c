/*
FILE
	tZone source code of iDNS.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	$Id$
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tzonefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uZone: Primary Key
static unsigned uZone=0;
//cZone: Zone name
static char cZone[101]={"domain.tld"};
//uNSSet: Pulldown of configured name server groups
static unsigned uNSSet=0;
static char cuNSSetPullDown[256]={""};
//cHostmaster: eMail address of responsible person
static char cHostmaster[101]={""};
//uSerial: Zone serial number
static unsigned uSerial=0;
//uExpire: Seconds to expire for slave name servers
static unsigned uExpire=604800;
//uRefresh: How often slaves should contact master
static unsigned uRefresh=28800;
//uTTL: Default minimum TTL for RRs
static unsigned uTTL=86400;
//uRetry: Slave server retry interval after uRefresh
static unsigned uRetry=7200;
//uZoneTTL: Zone TTL
static unsigned uZoneTTL=86400;
//uMailServers: Pulldown of configured mail server groups
static unsigned uMailServers=0;
static char cuMailServersPullDown[256]={""};
//uView: Optional tView Group
static unsigned uView=1;
static char cuViewPullDown[256]={""};
//cMainAddress: Zones main A record
static char cMainAddress[17]={""};
//uRegistrar: Registrar of Record for this Zone if Applies
static unsigned uRegistrar=0;
static char cuRegistrarPullDown[256]={""};
//uSecondaryOnly: Secondary DNS Service Only. cMasterIPs Would be External Customer Provided Master
static unsigned uSecondaryOnly=0;
static char cYesNouSecondaryOnly[32]={""};
//cMasterIPs: Slave zones master IP source(s)
static char cMasterIPs[256]={""};
//cOptions: slave.zones and master.zones zone options
static char *cOptions={""};
//Resource owner, i.e. end user uClient that has the resource deployed for
static unsigned uClient=0;
//uOwner: Record owner
static unsigned uOwner=0;
//uCreatedBy: uClient for last insert
static unsigned uCreatedBy=0;
#define ISM3FIELDS
//uCreatedDate: Unix seconds date last insert
static long uCreatedDate=0;
//uModBy: uClient for last update
static unsigned uModBy=0;
//uModDate: Unix seconds date last update
static long uModDate=0;



#define VAR_LIST_tZone "tZone.uZone,tZone.cZone,tZone.uNSSet,tZone.cHostmaster,tZone.uSerial,tZone.uExpire,tZone.uRefresh,tZone.uTTL,tZone.uRetry,tZone.uZoneTTL,tZone.uMailServers,tZone.uView,tZone.cMainAddress,tZone.uRegistrar,tZone.uSecondaryOnly,tZone.cOptions,tZone.uClient,tZone.uOwner,tZone.uCreatedBy,tZone.uCreatedDate,tZone.uModBy,tZone.uModDate"

 //Local only
void Insert_tZone(void);
void Update_tZone(char *cRowid);
void ProcesstZoneListVars(pentry entries[], int x);
static char cZoneSearch[64]={""};
void tZoneSearchSet(unsigned uStep);

 //In tZonefunc.h file included below
void ExtProcesstZoneVars(pentry entries[], int x);
void ExttZoneCommands(pentry entries[], int x);
void ExttZoneButtons(void);
void ExttZoneNavBar(void);
void ExttZoneGetHook(entry gentries[], int x);
void ExttZoneSelect(void);
void ExttZoneSelectRow(void);
void ExttZoneListSelect(void);
void ExttZoneListFilter(void);
void ExttZoneAuxTable(void);

#include "tzonefunc.h"

 //Table Variables Assignment Function
void ProcesstZoneVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uZone"))
			sscanf(entries[i].val,"%u",&uZone);
		else if(!strcmp(entries[i].name,"cZone"))
			sprintf(cZone,"%.100s",FQDomainName(entries[i].val));
		else if(!strcmp(entries[i].name,"uNSSet"))
			sscanf(entries[i].val,"%u",&uNSSet);
		else if(!strcmp(entries[i].name,"cuNSSetPullDown"))
		{
			sprintf(cuNSSetPullDown,"%.255s",entries[i].val);
			uNSSet=ReadPullDown("tNSSet","cLabel",cuNSSetPullDown);
		}
		else if(!strcmp(entries[i].name,"cHostmaster"))
			sprintf(cHostmaster,"%.100s",FQDomainName(entries[i].val));
		else if(!strcmp(entries[i].name,"uSerial"))
			sscanf(entries[i].val,"%u",&uSerial);
		else if(!strcmp(entries[i].name,"uExpire"))
			sscanf(entries[i].val,"%u",&uExpire);
		else if(!strcmp(entries[i].name,"uRefresh"))
			sscanf(entries[i].val,"%u",&uRefresh);
		else if(!strcmp(entries[i].name,"uTTL"))
			sscanf(entries[i].val,"%u",&uTTL);
		else if(!strcmp(entries[i].name,"uRetry"))
			sscanf(entries[i].val,"%u",&uRetry);
		else if(!strcmp(entries[i].name,"uZoneTTL"))
			sscanf(entries[i].val,"%u",&uZoneTTL);
		else if(!strcmp(entries[i].name,"uMailServers"))
			sscanf(entries[i].val,"%u",&uMailServers);
		else if(!strcmp(entries[i].name,"cuMailServersPullDown"))
		{
			sprintf(cuMailServersPullDown,"%.255s",entries[i].val);
			uMailServers=ReadPullDown("tMailServer","cLabel",cuMailServersPullDown);
		}
		else if(!strcmp(entries[i].name,"uView"))
			sscanf(entries[i].val,"%u",&uView);
		else if(!strcmp(entries[i].name,"cuViewPullDown"))
		{
			sprintf(cuViewPullDown,"%.255s",entries[i].val);
			uView=ReadPullDown("tView","cLabel",cuViewPullDown);
		}
		else if(!strcmp(entries[i].name,"cMainAddress"))
			sprintf(cMainAddress,"%.16s",IPNumber(entries[i].val));
		else if(!strcmp(entries[i].name,"uRegistrar"))
			sscanf(entries[i].val,"%u",&uRegistrar);
		else if(!strcmp(entries[i].name,"cuRegistrarPullDown"))
		{
			sprintf(cuRegistrarPullDown,"%.255s",entries[i].val);
			uRegistrar=ReadPullDown("tRegistrar","cLabel",cuRegistrarPullDown);
		}
		else if(!strcmp(entries[i].name,"uSecondaryOnly"))
			sscanf(entries[i].val,"%u",&uSecondaryOnly);
		else if(!strcmp(entries[i].name,"cYesNouSecondaryOnly"))
		{
			sprintf(cYesNouSecondaryOnly,"%.31s",entries[i].val);
			uSecondaryOnly=ReadYesNoPullDown(cYesNouSecondaryOnly);
		}
		else if(!strcmp(entries[i].name,"cMasterIPs"))
			sprintf(cMasterIPs,"%.255s",entries[i].val);
		else if(!strcmp(entries[i].name,"cOptions"))
			cOptions=entries[i].val;
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
		else if(!strcmp(entries[i].name,"uClient"))
			sscanf(entries[i].val,"%u",&uClient);
		else if(!strcmp(entries[i].name,"cZoneSearch"))
			sprintf(cZoneSearch,"%.63s",entries[i].val);

	}

	//After so we can overwrite form data if needed.
	ExtProcesstZoneVars(entries,x);

}//ProcesstZoneVars()


void ProcesstZoneListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uZone);
                        guMode=2002;
                        tZone("");
                }
        }
}//void ProcesstZoneListVars(pentry entries[], int x)


int tZoneCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttZoneCommands(entries,x);

	if(!strcmp(gcFunction,"tZoneTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tZoneList();
		}

		//Default
		ProcesstZoneVars(entries,x);
		tZone("");
	}
	else if(!strcmp(gcFunction,"tZoneList"))
	{
		ProcessControlVars(entries,x);
		ProcesstZoneListVars(entries,x);
		tZoneList();
	}

	return(0);

}//tZoneCommands()


void tZone(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttZoneSelectRow();
		else
			ExttZoneSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetZone();
				iDNS("New tZone table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tZone WHERE uZone=%u"
						,uZone);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uZone);
		sprintf(cZone,"%.100s",field[1]);
		sscanf(field[2],"%u",&uNSSet);
		sprintf(cHostmaster,"%.100s",field[3]);
		sscanf(field[4],"%u",&uSerial);
		sscanf(field[5],"%u",&uExpire);
		sscanf(field[6],"%u",&uRefresh);
		sscanf(field[7],"%u",&uTTL);
		sscanf(field[8],"%u",&uRetry);
		sscanf(field[9],"%u",&uZoneTTL);
		sscanf(field[10],"%u",&uMailServers);
		sscanf(field[11],"%u",&uView);
		sprintf(cMainAddress,"%.16s",field[12]);
		sscanf(field[13],"%u",&uRegistrar);
		sscanf(field[14],"%u",&uSecondaryOnly);
		cOptions=field[15];
		sscanf(field[16],"%u",&uClient);
		sscanf(field[17],"%u",&uOwner);
		sscanf(field[18],"%u",&uCreatedBy);
		sscanf(field[19],"%lu",&uCreatedDate);
		sscanf(field[20],"%u",&uModBy);
		sscanf(field[21],"%lu",&uModDate);
		//TODO Unresolved tNSSet vs tNSSet issue.
		sprintf(cMasterIPs,"%.255s",ForeignKey("tNSSet","cMasterIPs",uNSSet));

		}

	}//Internal Skip

	Header_ism3(":: tZone",2);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tZoneTools>");
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

        ExttZoneButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tZone Record Data",100);

	//Custom right panel for creating search sets
	if(guMode==12001)
		tZoneSearchSet(1);
	else if(guMode==12002)
		tZoneSearchSet(2);
	else if(guMode==2000 || guMode==2002)
		tZoneInput(1);
	else if(1)
		tZoneInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttZoneAuxTable();

	Footer_ism3();

}//end of tZone();

void tZoneSearchSet(unsigned uStep)
{
	printf("<tr><td><u>Set search parameters</u></td></tr>");

	OpenRow("cZone pattern","black");
	//Usability: Transfer from main tContainer page any current search pattern
	if(cSearch[0])
		sprintf(cZoneSearch,"%.31s",cSearch);
	printf("<input title='SQL search pattern %% and _ allowed' type=text name=cZoneSearch"
			" value=\"%s\" size=40 maxlength=63 >",cZoneSearch);

	OpenRow("NSSet","black");
	tTablePullDown("tNSSet;cuNSSetPullDown","cLabel","cLabel",uSelectNSSet,1);

	OpenRow("View","black");
	tTablePullDown("tView;cuViewPullDown","cLabel","cLabel",uView,1);

	if(uStep==1)
	{
		;
	}
	else if(uStep==2)
	{
		;
	}

}//void tZoneSearchSet(unsigned uStep)


void tZoneInput(unsigned uMode)
{
//cZone optional graph
        if(cZone[0])
        {
                char cConfigBuffer[256]={""};
                GetConfiguration(cZone,cConfigBuffer,0);
                if(cConfigBuffer[0])
                {
                        OpenRow("Optional Graph","black");
                        printf("</td></tr>\n");
                        printf("<tr><td></td><td colspan=3><a title='Click to enlarge' href=%1$s>"
                                        "<img src=%1$s border=0 width=400></a></td>\n",
                                                        cConfigBuffer);
                }
        }

//uZone
	OpenRow(LANG_FL_tZone_uZone,"black");
	printf("<input title='%s' type=text name=uZone value=%u size=16 maxlength=10 "
,LANG_FT_tZone_uZone,uZone);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uZone value=%u >\n",uZone);
	}
//cZone
	OpenRow(LANG_FL_tZone_cZone,EmptyString(cZone));
	printf("<input title='%s' type=text name=cZone value=\"%s\" size=40 maxlength=99 "
,LANG_FT_tZone_cZone,EncodeDoubleQuotes(cZone));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cZone value=\"%s\">\n",EncodeDoubleQuotes(cZone));
	}
//uNSSet
	OpenRow(LANG_FL_tZone_uNSSet,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tNSSet;cuNSSetPullDown","cLabel","cLabel",uNSSet,1);
	else
		tTablePullDown("tNSSet;cuNSSetPullDown","cLabel","cLabel",uNSSet,0);
//cHostmaster
	OpenRow(LANG_FL_tZone_cHostmaster,EmptyString(cHostmaster));
	printf("<input title='%s' type=text name=cHostmaster value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tZone_cHostmaster,EncodeDoubleQuotes(cHostmaster));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cHostmaster value=\"%s\">\n",EncodeDoubleQuotes(cHostmaster));
	}
//uSerial
	OpenRow(LANG_FL_tZone_uSerial,IsZero(uSerial));
	printf("<input title='%s' type=text name=uSerial value=%u size=16 maxlength=10 "
,LANG_FT_tZone_uSerial,uSerial);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uSerial value=%u >\n",uSerial);
	}
//uExpire
	OpenRow(LANG_FL_tZone_uExpire,IsZero(uExpire));
	printf("<input title='%s' type=text name=uExpire value=%u size=16 maxlength=10 "
,LANG_FT_tZone_uExpire,uExpire);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uExpire value=%u >\n",uExpire);
	}
//uRefresh
	OpenRow(LANG_FL_tZone_uRefresh,IsZero(uRefresh));
	printf("<input title='%s' type=text name=uRefresh value=%u size=16 maxlength=10 "
,LANG_FT_tZone_uRefresh,uRefresh);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uRefresh value=%u >\n",uRefresh);
	}
//uTTL
	OpenRow(LANG_FL_tZone_uTTL,IsZero(uTTL));
	printf("<input title='%s' type=text name=uTTL value=%u size=16 maxlength=10 "
,LANG_FT_tZone_uTTL,uTTL);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uTTL value=%u >\n",uTTL);
	}
//uRetry
	OpenRow(LANG_FL_tZone_uRetry,IsZero(uRetry));
	printf("<input title='%s' type=text name=uRetry value=%u size=16 maxlength=10 "
,LANG_FT_tZone_uRetry,uRetry);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uRetry value=%u >\n",uRetry);
	}
//uZoneTTL
	OpenRow(LANG_FL_tZone_uZoneTTL,IsZero(uZoneTTL));
	printf("<input title='%s' type=text name=uZoneTTL value=%u size=16 maxlength=10 "
,LANG_FT_tZone_uZoneTTL,uZoneTTL);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uZoneTTL value=%u >\n",uZoneTTL);
	}
//uMailServers
	OpenRow(LANG_FL_tZone_uMailServers,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tMailServer;cuMailServersPullDown","cLabel","cLabel",uMailServers,1);
	else
		tTablePullDown("tMailServer;cuMailServersPullDown","cLabel","cLabel",uMailServers,0);
//uView
	OpenRow(LANG_FL_tZone_uView,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tView;cuViewPullDown","cLabel","cLabel",uView,1);
	else
		tTablePullDown("tView;cuViewPullDown","cLabel","cLabel",uView,0);
//cMainAddress
	OpenRow(LANG_FL_tZone_cMainAddress,"black");
	printf("<input title='%s' type=text name=cMainAddress value=\"%s\" size=40 maxlength=16 "
,LANG_FT_tZone_cMainAddress,EncodeDoubleQuotes(cMainAddress));
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cMainAddress value=\"%s\">\n",EncodeDoubleQuotes(cMainAddress));
	}
//uRegistrar
	OpenRow(LANG_FL_tZone_uRegistrar,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tRegistrar;cuRegistrarPullDown","cLabel","cLabel",uRegistrar,1);
	else
		tTablePullDown("tRegistrar;cuRegistrarPullDown","cLabel","cLabel",uRegistrar,0);
//uSecondaryOnly
	OpenRow(LANG_FL_tZone_uSecondaryOnly,"black");
	if(guPermLevel>=0 && uMode)
		YesNoPullDown("uSecondaryOnly",uSecondaryOnly,1);
	else
		YesNoPullDown("uSecondaryOnly",uSecondaryOnly,0);
//cMasterIPs
	OpenRow(LANG_FL_tZone_cMasterIPs,"black");
	printf("<input title='%s' type=text name=cMasterIPs value=\"%s\" size=40 maxlength=255 "
,LANG_FT_tZone_cMasterIPs,EncodeDoubleQuotes(cMasterIPs));
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cMasterIPs value=\"%s\">\n",EncodeDoubleQuotes(cMasterIPs));
	}
//cOptions
	OpenRow(LANG_FL_tZone_cOptions,"black");
	printf("<textarea title='%s' cols=80 rows=3 name=cOptions ",LANG_FT_tZone_cOptions);
	if(guPermLevel>=7 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cOptions);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cOptions);
		printf("<input type=hidden name=cOptions value=\"%s\" >\n",EncodeDoubleQuotes(cOptions));
	}
//uClient
	OpenRow(LANG_FL_tZone_uClient,"black");
	if(guPermLevel>=20 && uMode)
	{
		printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
	else
	{
		printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
//uOwner
	OpenRow(LANG_FL_tZone_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tZone_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tZone_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tZone_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tZone_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tZoneInput(unsigned uMode)


void NewtZone(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uZone FROM tZone\
				WHERE uZone=%u"
							,uZone);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tZone("<blink>Record already exists");
		tZone(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tZone();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uZone=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tZone",uZone);
	iDNSLog(uZone,"tZone","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uZone);
	tZone(gcQuery);
	}

}//NewtZone(unsigned uMode)


void DeletetZone(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tZone WHERE uZone=%u AND ( uOwner=%u OR %u>9 )"
					,uZone,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tZone WHERE uZone=%u"
					,uZone);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tZone("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		iDNSLog(uZone,"tZone","Del");
#endif
		tZone(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		iDNSLog(uZone,"tZone","DelError");
#endif
		tZone(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetZone(void)


void Insert_tZone(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tZone SET uZone=%u,cZone='%s',uNSSet=%u,cHostmaster='%s',uSerial=%u,uExpire=%u,uRefresh=%u,uTTL=%u,uRetry=%u,uZoneTTL=%u,uMailServers=%u,uView=%u,cMainAddress='%s',uRegistrar=%u,uSecondaryOnly=%u,cOptions='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uZone
			,TextAreaSave(cZone)
			,uNSSet
			,TextAreaSave(cHostmaster)
			,uSerial
			,uExpire
			,uRefresh
			,uTTL
			,uRetry
			,uZoneTTL
			,uMailServers
			,uView
			,TextAreaSave(cMainAddress)
			,uRegistrar
			,uSecondaryOnly
			,TextAreaSave(cOptions)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tZone(void)


void Update_tZone(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tZone SET uZone=%u,"
				"cZone='%s',"
				"uNSSet=%u,"
				"cHostmaster='%s',"
				"uSerial=%u,"
				"uExpire=%u,"
				"uRefresh=%u,"
				"uTTL=%u,"
				"uRetry=%u,"
				"uZoneTTL=%u,"
				"uMailServers=%u,"
				"uView=%u,"
				"cMainAddress='%s',"
				"uRegistrar=%u,"
				"uSecondaryOnly=%u,"
				"cOptions='%s',"
				"uModBy=%u,"
				"uModDate=UNIX_TIMESTAMP(NOW()),"
				"uOwner=%u"
				" WHERE _rowid=%s"
			,uZone
			,TextAreaSave(cZone)
			,uNSSet
			,TextAreaSave(cHostmaster)
			,uSerial
			,uExpire
			,uRefresh
			,uTTL
			,uRetry
			,uZoneTTL
			,uMailServers
			,uView
			,TextAreaSave(cMainAddress)
			,uRegistrar
			,uSecondaryOnly
			,TextAreaSave(cOptions)
			,uModBy
			,uOwner
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tZone(void)


void ModtZone(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	sprintf(gcQuery,"SELECT uZone,uModDate FROM tZone WHERE uZone=%u"
			,uZone);
#else
	sprintf(gcQuery,"SELECT uZone FROM tZone WHERE uZone=%u"
			,uZone);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tZone("<blink>Record does not exist");
	if(i<1) tZone(LANG_NBR_RECNOTEXIST);
	//if(i>1) tZone("<blink>Multiple rows!");
	if(i>1) tZone(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tZone(LANG_NBR_EXTMOD);
#endif

	Update_tZone(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tZone",uZone);
	iDNSLog(uZone,"tZone","Mod");
#endif
	tZone(gcQuery);

}//ModtZone(void)


void tZoneList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttZoneListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tZoneList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttZoneListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uZone<td><font face=arial,helvetica color=white>cZone<td><font face=arial,helvetica color=white>uNSSet<td><font face=arial,helvetica color=white>cHostmaster<td><font face=arial,helvetica color=white>uSerial<td><font face=arial,helvetica color=white>uExpire<td><font face=arial,helvetica color=white>uRefresh<td><font face=arial,helvetica color=white>uTTL<td><font face=arial,helvetica color=white>uRetry<td><font face=arial,helvetica color=white>uZoneTTL<td><font face=arial,helvetica color=white>uMailServers<td><font face=arial,helvetica color=white>uView<td><font face=arial,helvetica color=white>cMainAddress<td><font face=arial,helvetica color=white>uRegistrar<td><font face=arial,helvetica color=white>uSecondaryOnly<td><font face=arial,helvetica color=white>cMasterIPs<td><font face=arial,helvetica color=white>uClient<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		long luYesNo14=strtoul(field[14],NULL,10);
		char cBuf14[4];
		if(luYesNo14)
			sprintf(cBuf14,"Yes");
		else
			sprintf(cBuf14,"No");
		long luTime19=strtoul(field[19],NULL,10);
		char cBuf19[32];
		if(luTime19)
			ctime_r(&luTime19,cBuf19);
		else
			sprintf(cBuf19,"---");
		long luTime21=strtoul(field[21],NULL,10);
		char cBuf21[32];
		if(luTime21)
			ctime_r(&luTime21,cBuf21);
		else
			sprintf(cBuf21,"---");
		printf("<td><a class=darkLink href=iDNS.cgi?gcFunction=tZone&uZone=%s>%s</a>"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
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
			,ForeignKey("tNSSet","cLabel",strtoul(field[2],NULL,10))
			,field[3]
			,field[4]
			,field[5]
			,field[6]
			,field[7]
			,field[8]
			,field[9]
			,ForeignKey("tMailServer","cLabel",strtoul(field[10],NULL,10))
			,ForeignKey("tView","cLabel",strtoul(field[11],NULL,10))
			,field[12]
			,ForeignKey("tRegistrar","cLabel",strtoul(field[13],NULL,10))
			,cBuf14
			,ForeignKey("tNSSet","cMasterIPs",strtoul(field[2],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[16],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[17],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[18],NULL,10))
			,cBuf19
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[20],NULL,10))
			,cBuf21
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tZoneList()


void CreatetZone(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tZone ("
			" uZone INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			" cZone VARCHAR(100) NOT NULL DEFAULT '',UNIQUE (cZone,uView),"
			" uOwner INT UNSIGNED NOT NULL DEFAULT 0,INDEX (uOwner),"
			" uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			" uNSSet INT UNSIGNED NOT NULL DEFAULT 0,INDEX (uNSSet),"
			" cHostmaster VARCHAR(100) NOT NULL DEFAULT '',"
			" uSerial INT UNSIGNED NOT NULL DEFAULT 0,"
			" uExpire INT UNSIGNED NOT NULL DEFAULT 0,"
			" uRefresh INT UNSIGNED NOT NULL DEFAULT 0,"
			" uTTL INT UNSIGNED NOT NULL DEFAULT 0,"
			" uRetry INT UNSIGNED NOT NULL DEFAULT 0,"
			" uZoneTTL INT UNSIGNED NOT NULL DEFAULT 0,"
			" uMailServers INT UNSIGNED NOT NULL DEFAULT 0,"
			" cMainAddress VARCHAR(16) NOT NULL DEFAULT '',"
			" uView INT UNSIGNED NOT NULL DEFAULT 0,"
			" uRegistrar INT UNSIGNED NOT NULL DEFAULT 0,"
			" cOptions TEXT NOT NULL DEFAULT '',"
			" uSecondaryOnly INT UNSIGNED NOT NULL DEFAULT 0,"
			" uClient INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetZone()

