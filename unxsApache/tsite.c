/*
FILE
	tSite source code of unxsApache.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tsitefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uSite: Primary Key
static unsigned uSite=0;
//cDomain: Principal FQDN no www. please
static char cDomain[65]={""};
//uNameBased: Name based virtual site
static unsigned uNameBased=0;
static char cYesNouNameBased[32]={""};
//uStatus: Active, OnHold, Canceled
static unsigned uStatus=0;
static char cuStatusPullDown[256]={""};
//uServer: Pull down for target server
static unsigned uServer=0;
static char cuServerPullDown[256]={""};
//cParkedDomains: FQDN of all parked domains one per line
static char *cParkedDomains={""};
//cIP: IP number if not name based site
static char cIP[17]={""};
//cVirtualHost: Apache httpd.conf file section
static char *cVirtualHost={""};
//cSSLVirtualHost: Apache SSL httpd.conf file section
static char *cSSLVirtualHost={""};
//uSSLCert: SSL certificate to use for this site
static unsigned uSSLCert=0;
static char cuSSLCertPullDown[256]={""};
//uHDQuota: Hard disk usage quota in MB
static unsigned uHDQuota=0;
//uTrafficQuota: Traffic quota that triggers notice. In MB
static unsigned uTrafficQuota=0;
//uWebalizer: Webalizer usage enabled
static unsigned uWebalizer=0;
static char cYesNouWebalizer[32]={""};
//uMySQL: mySQL database enabled
static unsigned uMySQL=0;
static char cYesNouMySQL[32]={""};
//uMaxUsers: Maximum number of tSiteUsers
static unsigned uMaxUsers=2;
//Resource owner, i.e. end user uClient that gas the resource deployed for
static unsigned uClient=0;
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



#define VAR_LIST_tSite "tSite.uSite,tSite.cDomain,tSite.uNameBased,tSite.uStatus,tSite.uServer,tSite.cParkedDomains,tSite.cIP,tSite.cVirtualHost,tSite.cSSLVirtualHost,tSite.uSSLCert,tSite.uHDQuota,tSite.uTrafficQuota,tSite.uWebalizer,tSite.uMySQL,tSite.uMaxUsers,tSite.uClient,tSite.uOwner,tSite.uCreatedBy,tSite.uCreatedDate,tSite.uModBy,tSite.uModDate"

 //Local only
void Insert_tSite(void);
void Update_tSite(char *cRowid);
void ProcesstSiteListVars(pentry entries[], int x);

 //In tSitefunc.h file included below
void ExtProcesstSiteVars(pentry entries[], int x);
void ExttSiteCommands(pentry entries[], int x);
void ExttSiteButtons(void);
void ExttSiteNavBar(void);
void ExttSiteGetHook(entry gentries[], int x);
void ExttSiteSelect(void);
void ExttSiteSelectRow(void);
void ExttSiteListSelect(void);
void ExttSiteListFilter(void);
void ExttSiteAuxTable(void);

#include "tsitefunc.h"

 //Table Variables Assignment Function
void ProcesstSiteVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uSite"))
			sscanf(entries[i].val,"%u",&uSite);
		else if(!strcmp(entries[i].name,"cDomain"))
			sprintf(cDomain,"%.64s",FQDomainName(entries[i].val));
		else if(!strcmp(entries[i].name,"uNameBased"))
			sscanf(entries[i].val,"%u",&uNameBased);
		else if(!strcmp(entries[i].name,"cYesNouNameBased"))
		{
			sprintf(cYesNouNameBased,"%.31s",entries[i].val);
			uNameBased=ReadYesNoPullDown(cYesNouNameBased);
		}
		else if(!strcmp(entries[i].name,"uStatus"))
			sscanf(entries[i].val,"%u",&uStatus);
		else if(!strcmp(entries[i].name,"cuStatusPullDown"))
		{
			sprintf(cuStatusPullDown,"%.255s",entries[i].val);
			uStatus=ReadPullDown("tStatus","cLabel",cuStatusPullDown);
		}
		else if(!strcmp(entries[i].name,"uServer"))
			sscanf(entries[i].val,"%u",&uServer);
		else if(!strcmp(entries[i].name,"cuServerPullDown"))
		{
			sprintf(cuServerPullDown,"%.255s",entries[i].val);
			uServer=ReadPullDown("tServer","cLabel",cuServerPullDown);
		}
		else if(!strcmp(entries[i].name,"cParkedDomains"))
			cParkedDomains=entries[i].val;
		else if(!strcmp(entries[i].name,"cIP"))
			sprintf(cIP,"%.16s",IPNumber(entries[i].val));
		else if(!strcmp(entries[i].name,"cVirtualHost"))
			cVirtualHost=entries[i].val;
		else if(!strcmp(entries[i].name,"cSSLVirtualHost"))
			cSSLVirtualHost=entries[i].val;
		else if(!strcmp(entries[i].name,"uSSLCert"))
			sscanf(entries[i].val,"%u",&uSSLCert);
		else if(!strcmp(entries[i].name,"cuSSLCertPullDown"))
		{
			sprintf(cuSSLCertPullDown,"%.255s",entries[i].val);
			uSSLCert=ReadPullDown("tSSLCert","cDomain",cuSSLCertPullDown);
		}
		else if(!strcmp(entries[i].name,"uHDQuota"))
			sscanf(entries[i].val,"%u",&uHDQuota);
		else if(!strcmp(entries[i].name,"uTrafficQuota"))
			sscanf(entries[i].val,"%u",&uTrafficQuota);
		else if(!strcmp(entries[i].name,"uWebalizer"))
			sscanf(entries[i].val,"%u",&uWebalizer);
		else if(!strcmp(entries[i].name,"cYesNouWebalizer"))
		{
			sprintf(cYesNouWebalizer,"%.31s",entries[i].val);
			uWebalizer=ReadYesNoPullDown(cYesNouWebalizer);
		}
		else if(!strcmp(entries[i].name,"uMySQL"))
			sscanf(entries[i].val,"%u",&uMySQL);
		else if(!strcmp(entries[i].name,"cYesNouMySQL"))
		{
			sprintf(cYesNouMySQL,"%.31s",entries[i].val);
			uMySQL=ReadYesNoPullDown(cYesNouMySQL);
		}
		else if(!strcmp(entries[i].name,"uMaxUsers"))
			sscanf(entries[i].val,"%u",&uMaxUsers);
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

	}

	//After so we can overwrite form data if needed.
	ExtProcesstSiteVars(entries,x);

}//ProcesstSiteVars()


void ProcesstSiteListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uSite);
                        guMode=2002;
                        tSite("");
                }
        }
}//void ProcesstSiteListVars(pentry entries[], int x)


int tSiteCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttSiteCommands(entries,x);

	if(!strcmp(gcFunction,"tSiteTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tSiteList();
		}

		//Default
		ProcesstSiteVars(entries,x);
		tSite("");
	}
	else if(!strcmp(gcFunction,"tSiteList"))
	{
		ProcessControlVars(entries,x);
		ProcesstSiteListVars(entries,x);
		tSiteList();
	}

	return(0);

}//tSiteCommands()


void tSite(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttSiteSelectRow();
		else
			ExttSiteSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetSite();
				unxsApache("New tSite table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tSite WHERE uSite=%u"
						,uSite);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uSite);
		sprintf(cDomain,"%.64s",field[1]);
		sscanf(field[2],"%u",&uNameBased);
		sscanf(field[3],"%u",&uStatus);
		sscanf(field[4],"%u",&uServer);
		cParkedDomains=field[5];
		sprintf(cIP,"%.16s",field[6]);
		cVirtualHost=field[7];
		cSSLVirtualHost=field[8];
		sscanf(field[9],"%u",&uSSLCert);
		sscanf(field[10],"%u",&uHDQuota);
		sscanf(field[11],"%u",&uTrafficQuota);
		sscanf(field[12],"%u",&uWebalizer);
		sscanf(field[13],"%u",&uMySQL);
		sscanf(field[14],"%u",&uMaxUsers);
		sscanf(field[15],"%u",&uClient);
		sscanf(field[16],"%u",&uOwner);
		sscanf(field[17],"%u",&uCreatedBy);
		sscanf(field[18],"%lu",&uCreatedDate);
		sscanf(field[19],"%u",&uModBy);
		sscanf(field[20],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tSite",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tSiteTools>");
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

        ExttSiteButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tSite Record Data",100);

	if(guMode==2000 || guMode==2002)
		tSiteInput(1);
	else
		tSiteInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttSiteAuxTable();

	Footer_ism3();

}//end of tSite();


void tSiteInput(unsigned uMode)
{

//uSite
	OpenRow(LANG_FL_tSite_uSite,"black");
	printf("<input title='%s' type=text name=uSite value=%u size=16 maxlength=10 "
,LANG_FT_tSite_uSite,uSite);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uSite value=%u >\n",uSite);
	}
//cDomain
	OpenRow(LANG_FL_tSite_cDomain,"black");
	printf("<input title='%s' type=text name=cDomain value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tSite_cDomain,EncodeDoubleQuotes(cDomain));
	if( (guPermLevel>=20 && uMode) || (guMode==2000 && guPermLevel>6) )
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cDomain value=\"%s\">\n",EncodeDoubleQuotes(cDomain));
	}
//uNameBased
	OpenRow(LANG_FL_tSite_uNameBased,"black");
	if( (guPermLevel>=20 && uMode) || (guMode==2000 && guPermLevel>6) )
		YesNoPullDown("uNameBased",uNameBased,1);
	else
		YesNoPullDown("uNameBased",uNameBased,0);
//uStatus
	OpenRow(LANG_FL_tSite_uStatus,"black");
	if(guPermLevel>=20 && uMode)
		tTablePullDown("tStatus;cuStatusPullDown","cLabel","cLabel",uStatus,1);
	else
		tTablePullDown("tStatus;cuStatusPullDown","cLabel","cLabel",uStatus,0);
//uServer
	OpenRow(LANG_FL_tSite_uServer,"black");
	if( (guPermLevel>=20 && uMode) || (guMode==2000 && guPermLevel>6) )
		tTablePullDown("tServer;cuServerPullDown","cLabel","cLabel",uServer,1);
	else
		tTablePullDown("tServer;cuServerPullDown","cLabel","cLabel",uServer,0);
//cParkedDomains
	OpenRow(LANG_FL_tSite_cParkedDomains,"black");
	printf("<textarea title='%s' cols=38 wrap=hard rows=2 name=cParkedDomains "
,LANG_FT_tSite_cParkedDomains);
	//if( guPermLevel>=0 && uMode  && (uNameBased || guMode==2000 || guMode==3002 ))
	if( (guPermLevel>=20 && uMode) || (guMode==2002 && guPermLevel>6) )
	{
		printf(">%s</textarea></td></tr>\n",cParkedDomains);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cParkedDomains);
		printf("<input type=hidden name=cParkedDomains value=\"%s\" >\n",EncodeDoubleQuotes(cParkedDomains));
	}
//cIP
	OpenRow(LANG_FL_tSite_cIP,"black");
	printf("<input title='%s' type=text name=cIP value=\"%s\" size=40 maxlength=16 "
,LANG_FT_tSite_cIP,EncodeDoubleQuotes(cIP));
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cIP value=\"%s\">\n",EncodeDoubleQuotes(cIP));
	}
//cVirtualHost
	OpenRow(LANG_FL_tSite_cVirtualHost,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cVirtualHost "
,LANG_FT_tSite_cVirtualHost);
	if(guPermLevel>=20 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cVirtualHost);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cVirtualHost);
		printf("<input type=hidden name=cVirtualHost value=\"%s\" >\n",EncodeDoubleQuotes(cVirtualHost));
	}
//cSSLVirtualHost
	OpenRow(LANG_FL_tSite_cSSLVirtualHost,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cSSLVirtualHost "
,LANG_FT_tSite_cSSLVirtualHost);
	if(guPermLevel>=20 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cSSLVirtualHost);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cSSLVirtualHost);
		printf("<input type=hidden name=cSSLVirtualHost value=\"%s\" >\n",EncodeDoubleQuotes(cSSLVirtualHost));
	}
//uSSLCert
	OpenRow(LANG_FL_tSite_uSSLCert,"black");
	if( (guPermLevel>=20 && uMode) || (guMode==2002 && guPermLevel>6) )
		tTablePullDown("tSSLCert;cuSSLCertPullDown","cDomain","cDomain",uSSLCert,1);
	else
		tTablePullDown("tSSLCert;cuSSLCertPullDown","cDomain","cDomain",uSSLCert,0);
//uHDQuota
	OpenRow(LANG_FL_tSite_uHDQuota,"black");
	printf("<input title='%s' type=text name=uHDQuota value=%u size=16 maxlength=10 "
,LANG_FT_tSite_uHDQuota,uHDQuota);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uHDQuota value=%u >\n",uHDQuota);
	}
//uTrafficQuota
	OpenRow(LANG_FL_tSite_uTrafficQuota,"black");
	printf("<input title='%s' type=text name=uTrafficQuota value=%u size=16 maxlength=10 "
,LANG_FT_tSite_uTrafficQuota,uTrafficQuota);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uTrafficQuota value=%u >\n",uTrafficQuota);
	}
//uWebalizer
	OpenRow(LANG_FL_tSite_uWebalizer,"black");
	if(guPermLevel>=12 && uMode)
		YesNoPullDown("uWebalizer",uWebalizer,1);
	else
		YesNoPullDown("uWebalizer",uWebalizer,0);
//uMySQL
	OpenRow(LANG_FL_tSite_uMySQL,"black");
	if(guPermLevel>=12 && uMode)
		YesNoPullDown("uMySQL",uMySQL,1);
	else
		YesNoPullDown("uMySQL",uMySQL,0);
//uMaxUsers
	OpenRow(LANG_FL_tSite_uMaxUsers,"black");
	printf("<input title='%s' type=text name=uMaxUsers value=%u size=16 maxlength=10 "
,LANG_FT_tSite_uMaxUsers,uMaxUsers);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uMaxUsers value=%u >\n",uMaxUsers);
	}
//uClient
	OpenRow(LANG_FL_tSite_uClient,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
	else
	{
	printf("%s<input type=hidden name=uClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uClient),uClient);
	}
//uOwner
	OpenRow(LANG_FL_tSite_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tSite_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tSite_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tSite_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tSite_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tSiteInput(unsigned uMode)


void NewtSite(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uSite FROM tSite\
				WHERE uSite=%u"
							,uSite);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tSite("<blink>Record already exists");
		tSite(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tSite();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uSite=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tSite",uSite);
	unxsApacheLog(uSite,"tSite","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uSite);
	tSite(gcQuery);
	}

}//NewtSite(unsigned uMode)


void DeletetSite(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tSite WHERE uSite=%u AND ( uOwner=%u OR %u>9 )"
					,uSite,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tSite WHERE uSite=%u"
					,uSite);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tSite("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsApacheLog(uSite,"tSite","Del");
#endif
		tSite(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsApacheLog(uSite,"tSite","DelError");
#endif
		tSite(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetSite(void)


void Insert_tSite(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tSite SET uSite=%u,cDomain='%s',uNameBased=%u,uStatus=%u,uServer=%u,cParkedDomains='%s',cIP='%s',cVirtualHost='%s',cSSLVirtualHost='%s',uSSLCert=%u,uHDQuota=%u,uTrafficQuota=%u,uWebalizer=%u,uMySQL=%u,uMaxUsers=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uSite
			,TextAreaSave(cDomain)
			,uNameBased
			,uStatus
			,uServer
			,TextAreaSave(cParkedDomains)
			,TextAreaSave(cIP)
			,TextAreaSave(cVirtualHost)
			,TextAreaSave(cSSLVirtualHost)
			,uSSLCert
			,uHDQuota
			,uTrafficQuota
			,uWebalizer
			,uMySQL
			,uMaxUsers
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tSite(void)


void Update_tSite(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tSite SET uSite=%u,cDomain='%s',uNameBased=%u,uStatus=%u,uServer=%u,cParkedDomains='%s',cIP='%s',cVirtualHost='%s',cSSLVirtualHost='%s',uSSLCert=%u,uHDQuota=%u,uTrafficQuota=%u,uWebalizer=%u,uMySQL=%u,uMaxUsers=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uSite
			,TextAreaSave(cDomain)
			,uNameBased
			,uStatus
			,uServer
			,TextAreaSave(cParkedDomains)
			,TextAreaSave(cIP)
			,TextAreaSave(cVirtualHost)
			,TextAreaSave(cSSLVirtualHost)
			,uSSLCert
			,uHDQuota
			,uTrafficQuota
			,uWebalizer
			,uMySQL
			,uMaxUsers
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tSite(void)


void ModtSite(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uSite,uModDate FROM tSite WHERE uSite=%u"
						,uSite);
#else
	sprintf(gcQuery,"SELECT uSite FROM tSite WHERE uSite=%u"
						,uSite);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tSite("<blink>Record does not exist");
	if(i<1) tSite(LANG_NBR_RECNOTEXIST);
	//if(i>1) tSite("<blink>Multiple rows!");
	if(i>1) tSite(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tSite(LANG_NBR_EXTMOD);
#endif

	Update_tSite(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tSite",uSite);
	unxsApacheLog(uSite,"tSite","Mod");
#endif
	tSite(gcQuery);

}//ModtSite(void)


void tSiteList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttSiteListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tSiteList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttSiteListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uSite<td><font face=arial,helvetica color=white>cDomain<td><font face=arial,helvetica color=white>uNameBased<td><font face=arial,helvetica color=white>uStatus<td><font face=arial,helvetica color=white>uServer<td><font face=arial,helvetica color=white>cParkedDomains<td><font face=arial,helvetica color=white>cIP<td><font face=arial,helvetica color=white>cVirtualHost<td><font face=arial,helvetica color=white>cSSLVirtualHost<td><font face=arial,helvetica color=white>uSSLCert<td><font face=arial,helvetica color=white>uHDQuota<td><font face=arial,helvetica color=white>uTrafficQuota<td><font face=arial,helvetica color=white>uWebalizer<td><font face=arial,helvetica color=white>uMySQL<td><font face=arial,helvetica color=white>uMaxUsers<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luYesNo2=strtoul(field[2],NULL,10);
		char cBuf2[4];
		if(luYesNo2)
			sprintf(cBuf2,"Yes");
		else
			sprintf(cBuf2,"No");
		time_t luYesNo12=strtoul(field[12],NULL,10);
		char cBuf12[4];
		if(luYesNo12)
			sprintf(cBuf12,"Yes");
		else
			sprintf(cBuf12,"No");
		time_t luYesNo13=strtoul(field[13],NULL,10);
		char cBuf13[4];
		if(luYesNo13)
			sprintf(cBuf13,"Yes");
		else
			sprintf(cBuf13,"No");
		time_t luTime17=strtoul(field[17],NULL,10);
		char cBuf17[32];
		if(luTime17)
			ctime_r(&luTime17,cBuf17);
		else
			sprintf(cBuf17,"---");
		time_t luTime19=strtoul(field[19],NULL,10);
		char cBuf19[32];
		if(luTime19)
			ctime_r(&luTime19,cBuf19);
		else
			sprintf(cBuf19,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td><textarea disabled>%s</textarea><td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,cBuf2
			,ForeignKey("tStatus","cLabel",strtoul(field[3],NULL,10))
			,ForeignKey("tServer","cLabel",strtoul(field[4],NULL,10))
			,field[5]
			,field[6]
			,field[7]
			,field[8]
			,ForeignKey("tSSLCert","cDomain",strtoul(field[9],NULL,10))
			,field[10]
			,field[11]
			,cBuf12
			,cBuf13
			,field[14]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[15],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[16],NULL,10))
			,cBuf17
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[18],NULL,10))
			,cBuf19
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tSiteList()


void CreatetSite(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tSite ( uSite INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cDomain VARCHAR(64) NOT NULL DEFAULT '',unique (cDomain,uServer), uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cParkedDomains TEXT NOT NULL DEFAULT '', cIP VARCHAR(16) NOT NULL DEFAULT '',index (cIP), cVirtualHost TEXT NOT NULL DEFAULT '', uNameBased INT UNSIGNED NOT NULL DEFAULT 0, uHDQuota INT UNSIGNED NOT NULL DEFAULT 0, uTrafficQuota INT UNSIGNED NOT NULL DEFAULT 0, uWebalizer INT UNSIGNED NOT NULL DEFAULT 0, uMySQL INT UNSIGNED NOT NULL DEFAULT 0, uMaxUsers INT UNSIGNED NOT NULL DEFAULT 0, uStatus INT UNSIGNED NOT NULL DEFAULT 0, uServer INT UNSIGNED NOT NULL DEFAULT 0, cSSLVirtualHost TEXT NOT NULL DEFAULT '', uSSLCert INT UNSIGNED NOT NULL DEFAULT 0, uClient INT UNSIGNED NOT NULL DEFAULT 0)");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		unxsApache(mysql_error(&gMysql));
}//CreatetSite()

