/*
FILE
	tContainer source code of unxsVZ.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	$Id$
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tcontainerfunc.h while 
	RAD is still to be used.
*/


void GetConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer,
		unsigned uHtml);
void tTablePullDownAvail(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode);
#include "mysqlrad.h"

//Table Variables
//Table Variables
//uContainer: Primary Key
static unsigned uContainer=0;
//cLabel: Short label
static char cLabel[33]={""};
//cHostname: FQDN Hostname
static char cHostname[65]={""};
//uVeth default is no which means VENET not VETH networked container
static unsigned uVeth=0;
static char cYesNouVeth[32]={""};
//uIPv4: First IPv4 Number
static unsigned uIPv4=0;
static char cuIPv4PullDown[256]={""};
//uOSTemplate: Container OS Template
static unsigned uOSTemplate=0;
static char cuOSTemplatePullDown[256]={""};
//uConfig: Container Config File Used on Creation
static unsigned uConfig=0;
static char cuConfigPullDown[256]={""};
//uNameserver: Container Resolver Nameserver Set
static unsigned uNameserver=0;
static char cuNameserverPullDown[256]={""};
//uSearchdomain: Container Resolver Searchdomain
static unsigned uSearchdomain=0;
static char cuSearchdomainPullDown[256]={""};
//uDatacenter: Belongs to this Datacenter
static unsigned uDatacenter=0;
static char cuDatacenterPullDown[256]={""};
//uNode: Running on this Node
static unsigned uNode=0;
static char cuNodePullDown[256]={""};
//uStatus: Status of Container
static unsigned uStatus=0;
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
//uSource semi HIDDEN for now
static unsigned uSource=0;



#define VAR_LIST_tContainer "tContainer.uContainer,tContainer.cLabel,tContainer.cHostname,tContainer.uVeth,tContainer.uIPv4,tContainer.uOSTemplate,tContainer.uConfig,tContainer.uNameserver,tContainer.uSearchdomain,tContainer.uDatacenter,tContainer.uNode,tContainer.uStatus,tContainer.uOwner,tContainer.uCreatedBy,tContainer.uCreatedDate,tContainer.uModBy,tContainer.uModDate,tContainer.uSource"

 //Local only
void Insert_tContainer(void);
void Update_tContainer(char *cRowid);
void ProcesstContainerListVars(pentry entries[], int x);

 //In tContainerfunc.h file included below
void ExtProcesstContainerVars(pentry entries[], int x);
void ExttContainerCommands(pentry entries[], int x);
void ExttContainerButtons(void);
void ExttContainerNavBar(void);
void ExttContainerGetHook(entry gentries[], int x);
void ExttContainerSelect(void);
void ExttContainerSelectRow(void);
void ExttContainerListSelect(void);
void ExttContainerListFilter(void);
void ExttContainerAuxTable(void);

#include "tcontainerfunc.h"

 //Table Variables Assignment Function
void ProcesstContainerVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uContainer"))
			sscanf(entries[i].val,"%u",&uContainer);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",FQDomainName(entries[i].val));
		else if(!strcmp(entries[i].name,"cHostname"))
			sprintf(cHostname,"%.64s",FQDomainName(entries[i].val));
		else if(!strcmp(entries[i].name,"uVeth"))
			sscanf(entries[i].val,"%u",&uVeth);
		else if(!strcmp(entries[i].name,"cYesNouVeth"))
		{
			sprintf(cYesNouVeth,"%.31s",entries[i].val);
			uVeth=ReadYesNoPullDown(cYesNouVeth);
		}
		else if(!strcmp(entries[i].name,"uIPv4"))
			sscanf(entries[i].val,"%u",&uIPv4);
		else if(!strcmp(entries[i].name,"cuIPv4PullDown"))
		{
			sprintf(cuIPv4PullDown,"%.255s",entries[i].val);
			uIPv4=ReadPullDown("tIP","cLabel",cuIPv4PullDown);
		}
		else if(!strcmp(entries[i].name,"uOSTemplate"))
			sscanf(entries[i].val,"%u",&uOSTemplate);
		else if(!strcmp(entries[i].name,"cuOSTemplatePullDown"))
		{
			sprintf(cuOSTemplatePullDown,"%.255s",entries[i].val);
			uOSTemplate=ReadPullDown("tOSTemplate","cLabel",cuOSTemplatePullDown);
		}
		else if(!strcmp(entries[i].name,"uConfig"))
			sscanf(entries[i].val,"%u",&uConfig);
		else if(!strcmp(entries[i].name,"cuConfigPullDown"))
		{
			sprintf(cuConfigPullDown,"%.255s",entries[i].val);
			uConfig=ReadPullDown("tConfig","cLabel",cuConfigPullDown);
		}
		else if(!strcmp(entries[i].name,"uNameserver"))
			sscanf(entries[i].val,"%u",&uNameserver);
		else if(!strcmp(entries[i].name,"cuNameserverPullDown"))
		{
			sprintf(cuNameserverPullDown,"%.255s",entries[i].val);
			uNameserver=ReadPullDown("tNameserver","cLabel",cuNameserverPullDown);
		}
		else if(!strcmp(entries[i].name,"uSearchdomain"))
			sscanf(entries[i].val,"%u",&uSearchdomain);
		else if(!strcmp(entries[i].name,"cuSearchdomainPullDown"))
		{
			sprintf(cuSearchdomainPullDown,"%.255s",entries[i].val);
			uSearchdomain=ReadPullDown("tSearchdomain","cLabel",cuSearchdomainPullDown);
		}
		else if(!strcmp(entries[i].name,"uDatacenter"))
			sscanf(entries[i].val,"%u",&uDatacenter);
		else if(!strcmp(entries[i].name,"cuDatacenterPullDown"))
		{
			sprintf(cuDatacenterPullDown,"%.255s",entries[i].val);
			uDatacenter=ReadPullDown("tDatacenter","cLabel",cuDatacenterPullDown);
		}
		else if(!strcmp(entries[i].name,"uNode"))
			sscanf(entries[i].val,"%u",&uNode);
		else if(!strcmp(entries[i].name,"cuNodePullDown"))
		{
			sprintf(cuNodePullDown,"%.255s",entries[i].val);
			uNode=ReadPullDown("tNode","cLabel",cuNodePullDown);
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
		else if(!strcmp(entries[i].name,"uSource"))
			sscanf(entries[i].val,"%u",&uSource);

	}

	//After so we can overwrite form data if needed.
	ExtProcesstContainerVars(entries,x);

}//ProcesstContainerVars()


void ProcesstContainerListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uContainer);
                        //guMode=2002;//uMODE_MOD
                        guMode=6;//uMODE_EDIT
                        tContainer("");
                }
        }
}//void ProcesstContainerListVars(pentry entries[], int x)


int tContainerCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttContainerCommands(entries,x);

	if(!strcmp(gcFunction,"tContainerTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tContainerList();
		}

		//Default
		ProcesstContainerVars(entries,x);
		tContainer("");
	}
	else if(!strcmp(gcFunction,"tContainerList"))
	{
		ProcessControlVars(entries,x);
		ProcesstContainerListVars(entries,x);
		tContainerList();
	}

	return(0);

}//tContainerCommands()


void tContainer(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttContainerSelectRow();
		else
			ExttContainerSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetContainer();
				unxsVZ("New tContainer table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tContainer WHERE uContainer=%u"
						,uContainer);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uContainer);
		sprintf(cLabel,"%.32s",field[1]);
		sprintf(cHostname,"%.64s",field[2]);
		sscanf(field[3],"%u",&uVeth);
		sscanf(field[4],"%u",&uIPv4);
		sscanf(field[5],"%u",&uOSTemplate);
		sscanf(field[6],"%u",&uConfig);
		sscanf(field[7],"%u",&uNameserver);
		sscanf(field[8],"%u",&uSearchdomain);
		sscanf(field[9],"%u",&uDatacenter);
		sscanf(field[10],"%u",&uNode);
		sscanf(field[11],"%u",&uStatus);
		sscanf(field[12],"%u",&uOwner);
		sscanf(field[13],"%u",&uCreatedBy);
		sscanf(field[14],"%lu",&uCreatedDate);
		sscanf(field[15],"%u",&uModBy);
		sscanf(field[16],"%lu",&uModDate);
		sscanf(field[17],"%u",&uSource);

		}

	}//Internal Skip

	Header_ism3(":: OpenVZ Container",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tContainerTools>");
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

        ExttContainerButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tContainer Record Data",100);

	if(guMode==2000 || guMode==2002)
		tContainerInput(1);
	else
		tContainerInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttContainerAuxTable();

	Footer_ism3();

}//end of tContainer();


void tContainerInput(unsigned uMode)
{

	if(uContainer && uDatacenter)
	{
		char cConfigBuffer[256]={""};

		GetConfiguration("cTrafficDirURL",cConfigBuffer,uDatacenter,0,0,0);
		if(cConfigBuffer[0])
		{
	
			OpenRow("Graph","black");
			printf("<a href=%s/%u.png><img src=%s/%u.png border=0></a>\n",
					cConfigBuffer,uContainer,cConfigBuffer,uContainer);
			printf("</td></tr>\n");
		}
	}

//uContainer
	OpenRow(LANG_FL_tContainer_uContainer,"black");
	printf("<input title='%s' type=text name=uContainer value=%u size=16 maxlength=10 "
,LANG_FT_tContainer_uContainer,uContainer);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uContainer value=%u >\n",uContainer);
	}
//cLabel
	OpenRow(LANG_FL_tContainer_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tContainer_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//cHostname
	OpenRow(LANG_FL_tContainer_cHostname,"black");
	printf("<input title='%s' type=text name=cHostname value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tContainer_cHostname,EncodeDoubleQuotes(cHostname));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cHostname value=\"%s\">\n",EncodeDoubleQuotes(cHostname));
	}
//uVeth
	OpenRow(LANG_FL_tContainer_uVeth,"black");
	if(guPermLevel>=7 && uMode)
		YesNoPullDown("uVeth",uVeth,1);
	else
		YesNoPullDown("uVeth",uVeth,0);
//uIPv4
	OpenRow(LANG_FL_tContainer_uIPv4,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownAvail("tIP;cuIPv4PullDown","cLabel","cLabel",uIPv4,1);
	else
		tTablePullDownOwner("tIP;cuIPv4PullDown","cLabel","cLabel",uIPv4,0);
//uOSTemplate
	OpenRow(LANG_FL_tContainer_uOSTemplate,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownOwner("tOSTemplate;cuOSTemplatePullDown","cLabel","cLabel",uOSTemplate,1);
	else
		tTablePullDownOwner("tOSTemplate;cuOSTemplatePullDown","cLabel","cLabel",uOSTemplate,0);
//uConfig
	OpenRow(LANG_FL_tContainer_uConfig,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownOwner("tConfig;cuConfigPullDown","cLabel","cLabel",uConfig,1);
	else
		tTablePullDownOwner("tConfig;cuConfigPullDown","cLabel","cLabel",uConfig,0);
//uNameserver
	OpenRow(LANG_FL_tContainer_uNameserver,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownOwner("tNameserver;cuNameserverPullDown","cLabel","cLabel",uNameserver,1);
	else
		tTablePullDownOwner("tNameserver;cuNameserverPullDown","cLabel","cLabel",uNameserver,0);
//uSearchdomain
	OpenRow(LANG_FL_tContainer_uSearchdomain,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownOwner("tSearchdomain;cuSearchdomainPullDown","cLabel","cLabel",uSearchdomain,1);
	else
		tTablePullDownOwner("tSearchdomain;cuSearchdomainPullDown","cLabel","cLabel",uSearchdomain,0);
//uDatacenter
	OpenRow(LANG_FL_tContainer_uDatacenter,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownOwner("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,1);
	else
		tTablePullDownOwner("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,0);
//uNode
	OpenRow(LANG_FL_tContainer_uNode,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownOwner("tNode;cuNodePullDown","cLabel","cLabel",uNode,1);
	else
		tTablePullDownOwner("tNode;cuNodePullDown","cLabel","cLabel",uNode,0);
//uStatus
	OpenRow(LANG_FL_tContainer_uStatus,"black");
	printf("%s<input type=hidden name=uStatus value=%u >\n",
		ForeignKey("tStatus","cLabel",uStatus),uStatus);
//uSource
	OpenRow(LANG_FL_tContainer_uSource,"black");
	printf("%s<input type=hidden name=uSource value=%u >\n",
		ForeignKey("tContainer","cHostname",uSource),uSource);
//uOwner
	OpenRow(LANG_FL_tContainer_uOwner,"black");
	printf("%s<input type=hidden name=uOwner value=%u >\n",
		ForeignKey("tClient","cLabel",uOwner),uOwner);
//uCreatedBy
	OpenRow(LANG_FL_tContainer_uCreatedBy,"black");
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",
		ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
//uCreatedDate
	OpenRow(LANG_FL_tContainer_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tContainer_uModBy,"black");
	printf("%s<input type=hidden name=uModBy value=%u >\n",
		ForeignKey("tClient","cLabel",uModBy),uModBy);
//uModDate
	OpenRow(LANG_FL_tContainer_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");

}//void tContainerInput(unsigned uMode)


void NewtContainer(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uContainer=%u",uContainer);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tContainer("<blink>Record already exists");
		tContainer(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tContainer();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uContainer=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tContainer",uContainer);
	unxsVZLog(uContainer,"tContainer","New");
	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uContainer);
		tContainer(gcQuery);
	}

}//NewtContainer(unsigned uMode)


void DeletetContainer(void)
{
	sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u AND ( uOwner=%u OR %u>9 )"
					,uContainer,guLoginClient,guPermLevel);
	MYSQL_RUN;
	//tContainer("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsVZLog(uContainer,"tContainer","Del");
		tContainer(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsVZLog(uContainer,"tContainer","DelError");
		tContainer(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetContainer(void)


void Insert_tContainer(void)
{
	sprintf(gcQuery,"INSERT INTO tContainer SET uContainer=%u,cLabel='%s',cHostname='%s',uVeth=%u,"
				"uIPv4=%u,uOSTemplate=%u,uConfig=%u,uNameserver=%u,uSearchdomain=%u,"
				"uDatacenter=%u,uNode=%u,uStatus=%u,uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uContainer
			,TextAreaSave(cLabel)
			,TextAreaSave(cHostname)
			,uVeth
			,uIPv4
			,uOSTemplate
			,uConfig
			,uNameserver
			,uSearchdomain
			,uDatacenter
			,uNode
			,uStatus
			,uOwner
			,uCreatedBy
			);
	MYSQL_RUN;

}//void Insert_tContainer(void)


void Update_tContainer(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tContainer SET uContainer=%u,cLabel='%s',cHostname='%s',uVeth=%u,uIPv4=%u,"
				"uOSTemplate=%u,uConfig=%u,uNameserver=%u,uSearchdomain=%u,uDatacenter=%u,"
				"uNode=%u,uStatus=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uContainer
			,TextAreaSave(cLabel)
			,TextAreaSave(cHostname)
			,uVeth
			,uIPv4
			,uOSTemplate
			,uConfig
			,uNameserver
			,uSearchdomain
			,uDatacenter
			,uNode
			,uStatus
			,uModBy
			,cRowid);
	MYSQL_RUN;
}//void Update_tContainer(void)


void ModtContainer(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tContainer.uContainer,\
				tContainer.uModDate\
				FROM tContainer,tClient\
				WHERE tContainer.uContainer=%u\
				AND tContainer.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uContainer,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uContainer,uModDate FROM tContainer\
				WHERE uContainer=%u"
						,uContainer);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tContainer("<blink>Record does not exist");
	if(i<1) tContainer(LANG_NBR_RECNOTEXIST);
	//if(i>1) tContainer("<blink>Multiple rows!");
	if(i>1) tContainer(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tContainer(LANG_NBR_EXTMOD);

	Update_tContainer(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tContainer",uContainer);
	unxsVZLog(uContainer,"tContainer","Mod");
	tContainer(gcQuery);

}//ModtContainer(void)


void tContainerList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttContainerListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tContainerList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttContainerListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uContainer<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>cHostname<td><font face=arial,helvetica color=white>uVeth<td><font face=arial,helvetica color=white>uIPv4<td><font face=arial,helvetica color=white>uOSTemplate<td><font face=arial,helvetica color=white>uConfig<td><font face=arial,helvetica color=white>uNameserver<td><font face=arial,helvetica color=white>uSearchdomain<td><font face=arial,helvetica color=white>uDatacenter<td><font face=arial,helvetica color=white>uNode<td><font face=arial,helvetica color=white>uStatus<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime14=strtoul(field[14],NULL,10);
		char cBuf14[32];
		if(luTime14)
			ctime_r(&luTime14,cBuf14);
		else
			sprintf(cBuf14,"---");
		time_t luTime16=strtoul(field[16],NULL,10);
		char cBuf16[32];
		if(luTime16)
			ctime_r(&luTime16,cBuf16);
		else
			sprintf(cBuf16,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,field[3]
			,ForeignKey("tIP","cLabel",strtoul(field[4],NULL,10))
			,ForeignKey("tOSTemplate","cLabel",strtoul(field[5],NULL,10))
			,ForeignKey("tConfig","cLabel",strtoul(field[6],NULL,10))
			,ForeignKey("tNameserver","cLabel",strtoul(field[7],NULL,10))
			,ForeignKey("tSearchdomain","cLabel",strtoul(field[8],NULL,10))
			,ForeignKey("tDatacenter","cLabel",strtoul(field[9],NULL,10))
			,ForeignKey("tNode","cLabel",strtoul(field[10],NULL,10))
			,ForeignKey("tStatus","cLabel",strtoul(field[11],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[12],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[13],NULL,10))
			,cBuf14
			,ForeignKey("tClient","cLabel",strtoul(field[15],NULL,10))
			,cBuf16
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tContainerList()


void CreatetContainer(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tContainer ( uContainer INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uDatacenter INT UNSIGNED NOT NULL DEFAULT 0, INDEX(uDatacenter), UNIQUE (cLabel,uDatacenter), uNode INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uNode), uStatus INT UNSIGNED NOT NULL DEFAULT 0, uOSTemplate INT UNSIGNED NOT NULL DEFAULT 0, cHostname VARCHAR(64) NOT NULL DEFAULT '', uIPv4 INT UNSIGNED NOT NULL DEFAULT 0, uNameserver INT UNSIGNED NOT NULL DEFAULT 0, uSearchdomain INT UNSIGNED NOT NULL DEFAULT 0, uConfig INT UNSIGNED NOT NULL DEFAULT 0, uVeth INT UNSIGNED NOT NULL DEFAULT 0, uSource INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uSource) )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetContainer()


void tTablePullDownAvail(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode)
{
        register int i,n;
        char cLabel[128];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[34]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTablePullDown()");
                return;
        }

        //Extended functionality
        strncpy(cLocalTableName,cTableName,255);
        if((cp=strchr(cLocalTableName,';')))
        {
                strncpy(cSelectName,cp+1,32);
                cSelectName[32]=0;
                *cp=0;
        }


	if(guCompany==1)
        	sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 ORDER BY %s",
                                cFieldName,cLocalTableName,cOrderby);
	else
		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND uOwner=%u ORDER BY %s",
				cFieldName,cLocalTableName,guCompany,cOrderby);

	MYSQL_RUN_STORE_TEXT_RET_VOID(mysqlRes);
	
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
                printf("<select name=%s %s>\n",cLabel,cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                        {
                             printf("<option>%s</option>\n",mysqlField[1]);
                        }
                        else
                        {
                             printf("<option selected>%s</option>\n",mysqlField[1]);
			     if(!uMode)
			     sprintf(cHidden,"<input type=hidden name=%.32s value='%.32s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%.32s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDownAvail()



