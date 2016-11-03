/*
FILE
	tIP source code of unxsVZ.cgi
PURPOSE
	IP number inventory table
AUTHOR/LEGAL
	(C) 2001-2016 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
*/
//git describe version info
static char *cGitVersion="GitVersion:"GitVersion;


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uIP: Primary Key
static unsigned uIP=0;
//cLabel: Short label
static char cLabel[33]={""};
//uAvailable: IP is available for use
static unsigned uAvailable=0;
static char cYesNouAvailable[32]={""};
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
//uDatacenter: Belongs to this Datacenter
static unsigned uDatacenter=0;
static char cuDatacenterPullDown[256]={""};
//tIPType new table
static char cuIPTypePullDown[256]={""};

//cComment
static char *cComment={""};
static char *cIPList={""};

//new added post RAD
static unsigned uIPNum=0;
static unsigned uFWStatus=0;
static unsigned uFWRule=0;
static unsigned uCountryCode=0;
//For rfc1918, for hardware, containers, IPMI, etc.
//Just started figuring this out and planning.
//1 backend created
//11 asterisk extension
//7 snort agent
//17 backend firewall
static unsigned uIPType=0;

//Extensions for searching
static char cIPv4Search[16]={""};
static char cCommentSearch[33]={""};
static unsigned uAvailableSearch=0;
static char cYesNouAvailableSearch[8]={""};
static unsigned uNodeSearch=0;
static char cuNodeSearchPullDown[256]={""};
static unsigned uNodeSearchNot=0;
static unsigned uIPv4Exclude=0;
static unsigned uFirewallMode=0;
static unsigned u24Limit=0;
static unsigned u12Limit=0;
static unsigned u4Limit=0;
static unsigned uOwnerSearch=0;
static unsigned uDatacenterSearch=0;
static unsigned uCountryCodeSearch=0;
static unsigned uFWStatusSearch=0;
static unsigned uFWStatusAnySearch=0;
static char cuDatacenterSearchPullDown[256]={""};
static char cuCountryCodeSearchPullDown[256]={""};
static char cuFWStatusSearchPullDown[256]={""};
static unsigned uCountryCodeSearchNot=0;
int ReadYesNoPullDownTriState(const char *cLabel);
void YesNoPullDownTriState(char *cFieldName, unsigned uSelect, unsigned uMode);

#define VAR_LIST_tIP "tIP.uIP,tIP.cLabel,tIP.uAvailable,tIP.uOwner,tIP.uCreatedBy,\
			tIP.uCreatedDate,tIP.uModBy,tIP.uModDate,tIP.uDatacenter,tIP.cComment,\
			tIP.uIPNum,tIP.uFWStatus,tIP.uFWRule,tIP.uCountryCode,tIP.uIPType"

 //Local only
void tIPSearchSet(unsigned uStep);
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
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
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
		else if(!strcmp(entries[i].name,"uDatacenter"))
			sscanf(entries[i].val,"%u",&uDatacenter);
		else if(!strcmp(entries[i].name,"cComment"))
			cComment=entries[i].val;
		//new
		else if(!strcmp(entries[i].name,"uIPNum"))
			sscanf(entries[i].val,"%u",&uIPNum);
		else if(!strcmp(entries[i].name,"uFWStatus"))
			sscanf(entries[i].val,"%u",&uFWStatus);
		else if(!strcmp(entries[i].name,"uFWRule"))
			sscanf(entries[i].val,"%u",&uFWRule);
		else if(!strcmp(entries[i].name,"uCountryCode"))
			sscanf(entries[i].val,"%u",&uCountryCode);
		else if(!strcmp(entries[i].name,"uIPType"))
			sscanf(entries[i].val,"%u",&uIPType);
		else if(!strcmp(entries[i].name,"cIPList"))
			cIPList=entries[i].val;
		//
		else if(!strcmp(entries[i].name,"cuDatacenterPullDown"))
		{
			sprintf(cuDatacenterPullDown,"%.255s",entries[i].val);
			uDatacenter=ReadPullDown("tDatacenter","cLabel",cuDatacenterPullDown);
		}
		else if(!strcmp(entries[i].name,"cuIPTypePullDown"))
		{
			sprintf(cuIPTypePullDown,"%.255s",entries[i].val);
			uIPType=ReadPullDown("tIPType","cLabel",cuIPTypePullDown);
		}
		else if(!strcmp(entries[i].name,"uOwnerSearch"))
			sscanf(entries[i].val,"%u",&uOwnerSearch);
		else if(!strcmp(entries[i].name,"cForClientPullDown"))
		{
			sprintf(cForClientPullDown,"%.255s",entries[i].val);
			uOwnerSearch=ReadPullDown("tClient","cLabel",cForClientPullDown);
		}
		else if(!strcmp(entries[i].name,"uDatacenterSearch"))
			sscanf(entries[i].val,"%u",&uDatacenterSearch);
		else if(!strcmp(entries[i].name,"cuDatacenterSearchPullDown"))
		{
			sprintf(cuDatacenterSearchPullDown,"%.255s",entries[i].val);
			uDatacenterSearch=ReadPullDown("tDatacenter","cLabel",cuDatacenterSearchPullDown);
		}
		else if(!strcmp(entries[i].name,"uFWStatusSearch"))
			sscanf(entries[i].val,"%u",&uFWStatusSearch);
		else if(!strcmp(entries[i].name,"cuFWStatusSearchPullDown"))
		{
			sprintf(cuFWStatusSearchPullDown,"%.255s",entries[i].val);
			uFWStatusSearch=ReadPullDown("tFWStatus","cLabel",cuFWStatusSearchPullDown);
		}
		else if(!strcmp(entries[i].name,"uCountryCodeSearch"))
			sscanf(entries[i].val,"%u",&uCountryCodeSearch);
		else if(!strcmp(entries[i].name,"cuCountryCodeSearchPullDown"))
		{
			sprintf(cuCountryCodeSearchPullDown,"%.255s",entries[i].val);
			uCountryCodeSearch=ReadPullDown("tGeoIPCountryCode","cCountryCode",cuCountryCodeSearchPullDown);
		}
		else if(!strcmp(entries[i].name,"uNodeSearch"))
			sscanf(entries[i].val,"%u",&uNodeSearch);
		else if(!strcmp(entries[i].name,"cuNodeSearchPullDown"))
		{
			sprintf(cuNodeSearchPullDown,"%.255s",entries[i].val);
			uNodeSearch=ReadPullDown("tNode","cLabel",cuNodeSearchPullDown);
		}
		else if(!strcmp(entries[i].name,"cIPv4Search"))
			sprintf(cIPv4Search,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"cYesNouAvailableSearch"))
		{
			sprintf(cYesNouAvailableSearch,"%.8s",entries[i].val);
			uAvailableSearch=ReadYesNoPullDownTriState(cYesNouAvailableSearch);
		}
		else if(!strcmp(entries[i].name,"uNodeSearchNotNoCA"))
			uNodeSearchNot=1;
		else if(!strcmp(entries[i].name,"uCountryCodeSearchNotNoCA"))
			uCountryCodeSearchNot=1;
		else if(!strcmp(entries[i].name,"uIPv4ExcludeNoCA"))
			uIPv4Exclude=1;
		else if(!strcmp(entries[i].name,"uFirewallModeNoCA"))
			uFirewallMode=1;
		else if(!strcmp(entries[i].name,"u24LimitNoCA"))
			u24Limit=1;
		else if(!strcmp(entries[i].name,"u12LimitNoCA"))
			u12Limit=1;
		else if(!strcmp(entries[i].name,"u4LimitNoCA"))
			u4Limit=1;
		else if(!strcmp(entries[i].name,"cCommentSearch"))
			sprintf(cCommentSearch,"%.32s",entries[i].val);

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
				unxsVZ("New tIP table created");
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
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uIP);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uAvailable);
		sscanf(field[3],"%u",&uOwner);
		sscanf(field[4],"%u",&uCreatedBy);
		sscanf(field[5],"%lu",&uCreatedDate);
		sscanf(field[6],"%u",&uModBy);
		sscanf(field[7],"%lu",&uModDate);
		sscanf(field[8],"%u",&uDatacenter);
		cComment=field[9];
		//new
		sscanf(field[10],"%u",&uIPNum);
		sscanf(field[11],"%u",&uFWStatus);
		sscanf(field[12],"%u",&uFWRule);
		sscanf(field[13],"%u",&uCountryCode);
		sscanf(field[14],"%u",&uIPType);

		}

	}//Internal Skip

	Header_ism3(":: IPs used and reserved for use",2);//checkbox js = 2
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

	//Custom right panel for creating search sets
	if(guMode==12001)
		tIPSearchSet(1);
	else if(guMode==12002)
		tIPSearchSet(2);
	else if(guMode==2000 || guMode==2002)
		tIPInput(1);
	else if(1)
		tIPInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttIPAuxTable();

	Footer_ism3();

}//end of tIP();


void tIPSearchSet(unsigned uStep)
{
	OpenRow("<u>Set search parameters</u>","black");

	OpenRow("Display options","black");
	printf("<input title='Show firewall specific columns and filter options' type=checkbox name=uFirewallModeNoCA ");
	if(uFirewallMode)
		printf(" checked");
	printf("> Firewall mode");

	printf("<td><textarea title='List of IPs one per line. Leave one extra blank line at the end of list.'" 
			" cols=32 wrap=soft rows=1 name=cIPList>%s</textarea></td>",cIPList);

	OpenRow("Date Limits","black");
	printf("<input title='Only show last 24 hours FW entries' type=checkbox name=u24LimitNoCA");
	if(u24Limit)
		printf(" checked");
	printf("> Limit to last 24Hrs");
	printf("&nbsp; <input title='Only show last 12 hours FW entries' type=checkbox name=u12LimitNoCA");
	if(u12Limit)
		printf(" checked");
	printf("> Limit to last 12Hrs");
	printf("&nbsp; <input title='Only show last 4 hours FW entries' type=checkbox name=u4LimitNoCA");
	if(u4Limit)
		printf(" checked");
	printf("> Limit to last 4Hrs");

	OpenRow("IPv4 pattern","black");
	printf("<input title='SQL search pattern %% and _ allowed' type=text name=cIPv4Search"
			" value=\"%s\" size=40 maxlength=15 >",cIPv4Search);
	printf("<input title='Exclude 10/8, 172.16/12 and 192.168/16 IPs' type=checkbox name=uIPv4ExcludeNoCA ");
	if(uIPv4Exclude)
		printf(" checked");
	printf("> Exclude RFC1918 IPs");

	if(uFirewallMode)
	{
		OpenRow("Country Code","black");
		tTablePullDown("tGeoIPCountryCode;cuCountryCodeSearchPullDown","cCountryCode","cCountryCode",uCountryCodeSearch,1);
		printf("<input title='Logical NOT of selected country if any. Including default any country (no country)'"
			" type=checkbox name=uCountryCodeSearchNotNoCA ");
		if(uCountryCodeSearchNot)
			printf(" checked");
		printf("> Not");

		OpenRow("uFWStatus Code","black");
		tTablePullDown("tFWStatus;cuFWStatusSearchPullDown","cLabel","cLabel",uFWStatusSearch,1);
	}

	OpenRow("Datacenter","black");
	tTablePullDown("tDatacenter;cuDatacenterSearchPullDown","cLabel","cLabel",uDatacenterSearch,1);

	OpenRow("Node","black");
	tTablePullDown("tNode;cuNodeSearchPullDown","cLabel","cLabel",uNodeSearch,1);
	printf("<input title='Logical NOT of selected node if any. Including default any node (no node)' type=checkbox name=uNodeSearchNotNoCA ");
	if(uNodeSearchNot)
		printf(" checked");
	printf("> Not");

	OpenRow("Owner","black");
	tTablePullDownResellers(uOwnerSearch,0);

	OpenRow("Available","black");
	YesNoPullDownTriState("uAvailableSearch",uAvailableSearch,1);

	OpenRow("Comment pattern","black");
	printf("<input title='SQL search pattern %% and _ allowed' type=text name=cCommentSearch"
			" value=\"%s\" size=40 maxlength=15 >",cCommentSearch);

	OpenRow("uIPType","black");
	tTablePullDown("tIPType;cuIPTypePullDown","cLabel","cLabel",uIPType,1);
	//printf("<input title='uIPType code. E.g. \"11\" ClassC IP from Asterisk' type=text name=uIPType"
	//		" value=\"%u\" size=40 maxlength=15 >",uIPType);
	if(uStep==1)
	{
		;
	}
	else if(uStep==2)
	{
		;
	}

}//void tIPSearchSet(unsigned uStep)


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
//cLabel
	OpenRow(LANG_FL_tIP_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tIP_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uAvailable
	OpenRow(LANG_FL_tIP_uAvailable,"black");
	if(guPermLevel>=10 && uMode)
		YesNoPullDown("uAvailable",uAvailable,1);
	else
		YesNoPullDown("uAvailable",uAvailable,0);
//uDatacenter
	OpenRow(LANG_FL_tContainer_uDatacenter,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownOwner("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,1);
	else
		tTablePullDownOwner("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,0);
//new
//uIPNum
	OpenRow("uIPNum","black");
	printf("<input title='cLabel INET ATON' type=text name=uIPNum value=%u size=16 maxlength=10 ",uIPNum);
	printf("disabled></td></tr>\n");
	printf("<input type=hidden name=uIPNum value=%u >\n",uIPNum);
//uFWStatus
	OpenRow("uFWStatus","black");
	printf("<input title='tFWStatus.uFWStatus' type=text name=uFWStatus value='%s' size=16 maxlength=10 ",ForeignKey("tFWStatus","cLabel",uFWStatus));
	printf("disabled></td></tr>\n");
	printf("<input type=hidden name=uFWStatus value=%u >\n",uFWStatus);
//uFWRule
	OpenRow("uFWRule","black");
	printf("<input title='Last Snort IDS rule' type=text name=uFWRule value=%u size=16 maxlength=10 ",uFWRule);
	printf("disabled></td></tr>\n");
	printf("<input type=hidden name=uFWRule value=%u >\n",uFWRule);
//uCountryCode
	OpenRow("uCountryCode","black");
	printf("<input title='Internal country code' type=text name=uCountryCode value='%s' size=16 maxlength=10 ",
				ForeignKey("tGeoIPCountryCode","cCountryCode",uCountryCode));
	printf("disabled></td></tr>\n");
	printf("<input type=hidden name=uCountryCode value=%u >\n",uCountryCode);
//uIPType
	OpenRow("uIPType","black");
	if(guPermLevel>=7 && uMode)
		tTablePullDown("tIPType;cuIPTypePullDown","cLabel","cLabel",uIPType,1);
	else
		tTablePullDown("tIPType;cuIPTypePullDown","cLabel","cLabel",uIPType,0);
//cComment
	OpenRow("cComment","black");
	printf("<textarea title='Additional information about IP use' cols=80 wrap=hard rows=4 name=cComment ");
	if(guPermLevel>=7 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",TransformAngleBrackets(cComment));
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",TransformAngleBrackets(cComment));
		printf("<input type=hidden name=cComment value=\"%s\" >\n",EncodeDoubleQuotes(cComment));
	}
//uOwner
	OpenRow(LANG_FL_tIP_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tIP_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
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
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
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
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uIP=%u",uIP);
	MYSQL_RUN_STORE(res);
	if(mysql_num_rows(res)) 
		//tIP("<blink>Record already exists");
		tIP(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tIP();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uIP=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tIP",uIP);
	unxsVZLog(uIP,"tIP","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uIP);
		tIP(gcQuery);
	}

}//NewtIP(unsigned uMode)


void DeletetIP(void)
{
	sprintf(gcQuery,"DELETE FROM tIP WHERE uIP=%u AND ( uOwner=%u OR %u>9 )",uIP,guLoginClient,guPermLevel);
	MYSQL_RUN;
	//tIP("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsVZLog(uIP,"tIP","Del");
		tIP(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsVZLog(uIP,"tIP","DelError");
		tIP(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetIP(void)


void Insert_tIP(void)
{
	sprintf(gcQuery,"INSERT INTO tIP SET uIP=%u,uIPNum=INET_ATON('%s'),cLabel='%s',uAvailable=%u,uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW()),uDatacenter=%u,cComment='%s',uIPType=%u",
			uIP
			,TextAreaSave(cLabel)
			,TextAreaSave(cLabel)
			,uAvailable
			,uOwner
			,uCreatedBy
			,uDatacenter
			,cComment,uIPType);
	MYSQL_RUN;

}//void Insert_tIP(void)


void Update_tIP(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tIP SET uIP=%u,uIPNum=INET_ATON('%s'),cLabel='%s',uAvailable=%u,uModBy=%u,"
				"uModDate=UNIX_TIMESTAMP(NOW()),uDatacenter=%u,cComment='%s',uIPType=%u WHERE _rowid=%s",
			uIP
			,TextAreaSave(cLabel)
			,TextAreaSave(cLabel)
			,uAvailable
			,uModBy
			,uDatacenter
			,cComment
			,uIPType
			,cRowid);
	MYSQL_RUN;

}//void Update_tIP(void)


void ModtIP(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT tIP.uIP,tIP.uModDate FROM tIP,tClient WHERE tIP.uIP=%u"
				" AND tIP.uOwner=tClient.uClient AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uIP,guLoginClient,guLoginClient);
	else
		sprintf(gcQuery,"SELECT uIP,uModDate FROM tIP WHERE uIP=%u",uIP);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tIP("<blink>Record does not exist");
	if(i<1) tIP(LANG_NBR_RECNOTEXIST);
	//if(i>1) tIP("<blink>Multiple rows!");
	if(i>1) tIP(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tIP(LANG_NBR_EXTMOD);

	Update_tIP(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tIP",uIP);
	unxsVZLog(uIP,"tIP","Mod");
	tIP(gcQuery);

}//ModtIP(void)


void tIPList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttIPListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tIPList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttIPListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uIP"
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>uAvailable"
		"<td><font face=arial,helvetica color=white>uDatacenter"
		"<td><font face=arial,helvetica color=white>uIPNum"
		"<td><font face=arial,helvetica color=white>uFWStatus"
		"<td><font face=arial,helvetica color=white>uFWRule"
		"<td><font face=arial,helvetica color=white>uCountryCode"
		"<td><font face=arial,helvetica color=white>uIPType"
		"<td><font face=arial,helvetica color=white>cComment"
		"<td><font face=arial,helvetica color=white>uOwner"
		"<td><font face=arial,helvetica color=white>uCreatedBy"
		"<td><font face=arial,helvetica color=white>uCreatedDate"
		"<td><font face=arial,helvetica color=white>uModBy"
		"<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		long unsigned luYesNo2=strtoul(field[2],NULL,10);
		char cBuf2[4];
		if(luYesNo2)
			sprintf(cBuf2,"Yes");
		else
			sprintf(cBuf2,"No");
		time_t luTime5=strtoul(field[5],NULL,10);
		char cBuf5[32];
		if(luTime5)
			ctime_r(&luTime5,cBuf5);
		else
			sprintf(cBuf5,"---");
		time_t luTime7=strtoul(field[7],NULL,10);
		char cBuf7[32];
		if(luTime7)
			ctime_r(&luTime7,cBuf7);
		else
			sprintf(cBuf7,"---");
		printf("<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tIP&uIP=%s>%s</a>"
				"<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0],field[0]
			,field[1]
			,cBuf2
			,ForeignKey("tDatacenter","cLabel",strtoul(field[8],NULL,10))
			,field[10]
			,field[11]
			,field[12]
			,field[13]
			,field[14]
			,field[9]
			,ForeignKey("tClient","cLabel",strtoul(field[3],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[4],NULL,10))
			,cBuf5
			,ForeignKey("tClient","cLabel",strtoul(field[6],NULL,10))
			,cBuf7
					);
	}

	printf("</table></form>\n");
	Footer_ism3();

}//tIPList()


void CreatetIP(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tIP ( "
			"uIP INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
			"cComment VARCHAR(255) NOT NULL DEFAULT '',"
			"uOwner INT UNSIGNED NOT NULL DEFAULT 0,INDEX (uOwner),"
			//New
			"uIPNum INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uIPNum),"
			"uFWStatus TINYINT UNSIGNED NOT NULL DEFAULT 0,"
			"uFWRule INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uFWRule),"
			"uCountryCode TINYINT UNSIGNED NOT NULL DEFAULT 0, INDEX (uCountryCode),"
			"INDEX (uDatacenter),"
			"uIPType INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uIPType),"
			//end New
			"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			//Changed to tiny: storage only
			"uAvailable TINYINT UNSIGNED NOT NULL DEFAULT 0,"
			"uDatacenter INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetIP()
