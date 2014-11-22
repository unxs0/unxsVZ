/*
FILE
	tContainer source code of unxsVZ.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	$Id$
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tcontainerfunc.h while 
	RAD is still to be used.
AUTHOR/LEGAL
	(C) 2001-2010 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
*/


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
static unsigned uSourceNode=0;
static unsigned uSourceDatacenter=0;
static char cuSourceNodePullDown[256]={""};
static char cuSourceDatacenterPullDown[256]={""};
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
//uBackupDate: Unix seconds date last insert
static time_t uBackupDate=0;

//Extensions for searching
//uStatus: Container tStatus.uStatus
static unsigned uSearchStatus=0;
static char cuSearchStatusPullDown[256]={""};
static unsigned uSearchStatusNot=0;
static char cHostnameSearch[64]={""};
static char cLabelSearch[64]={""};
static char cIPv4Search[16]={""};
int ReadYesNoPullDownTriState(const char *cLabel);
void YesNoPullDownTriState(char *cFieldName, unsigned uSelect, unsigned uMode);
static unsigned uSearchSource=0;
static char cYesNouSearchSource[32]={""};

static char cuClientPullDown[256]={""};
static char cAutoCloneNode[256]={""};
static char cAutoCloneNodeRemote[256]={""};
static char cunxsBindARecordJobZone[256]={""};
static char gcNewContainerTZ[64]={"PST8PDT"};
static unsigned uEnableNAT=0;

static char *cCommands={""};

#define VAR_LIST_tContainer "tContainer.uContainer,tContainer.cLabel,tContainer.cHostname,tContainer.uVeth,tContainer.uIPv4,tContainer.uOSTemplate,tContainer.uConfig,tContainer.uNameserver,tContainer.uSearchdomain,tContainer.uDatacenter,tContainer.uNode,tContainer.uStatus,tContainer.uOwner,tContainer.uCreatedBy,tContainer.uCreatedDate,tContainer.uModBy,tContainer.uModDate,tContainer.uSource,tContainer.uBackupDate"

 //Local only
void tContainerSearchSet(unsigned uStep);
void tContainerNewStep(unsigned uStep);
void Insert_tContainer(void);
void Update_tContainer(char *cRowid);
void ProcesstContainerListVars(pentry entries[], int x);
void tTablePullDownDatacenterCloneIPs(const char *cTableName, const char *cFieldName,
	const char *cOrderby, unsigned uSelector, unsigned uMode,unsigned uDatacenter,unsigned uClient,unsigned uNode);
void tTablePullDownOwnerAnyAvailDatacenter(const char *cTableName, const char *cFieldName,
	const char *cOrderby, unsigned uSelector, unsigned uMode,unsigned uDatacenter,unsigned uClient);

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

void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);//jobqueue.c
void GeneratePasswd(char *pw);//main.c

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
		else if(!strcmp(entries[i].name,"cCommands"))
			cCommands=entries[i].val;
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
		else if(!strcmp(entries[i].name,"uForClient"))
			sscanf(entries[i].val,"%u",&uForClient);
		else if(!strcmp(entries[i].name,"cuClientPullDown"))
		{
			sprintf(cuClientPullDown,"%.255s",entries[i].val);
			uForClient=ReadPullDown("tClient","cLabel",cuClientPullDown);
		}
		else if(!strcmp(entries[i].name,"uNode"))
			sscanf(entries[i].val,"%u",&uNode);
		else if(!strcmp(entries[i].name,"cuNodePullDown"))
		{
			sprintf(cuNodePullDown,"%.255s",entries[i].val);
			uNode=ReadPullDown("tNode","cLabel",cuNodePullDown);
		}
		else if(!strcmp(entries[i].name,"uSourceDatacenter"))
			sscanf(entries[i].val,"%u",&uSourceDatacenter);
		else if(!strcmp(entries[i].name,"cuSourceDatacenterPullDown"))
		{
			sprintf(cuSourceDatacenterPullDown,"%.255s",entries[i].val);
			uSourceDatacenter=ReadPullDown("tDatacenter","cLabel",cuSourceDatacenterPullDown);
		}
		else if(!strcmp(entries[i].name,"uSourceNode"))
			sscanf(entries[i].val,"%u",&uSourceNode);
		else if(!strcmp(entries[i].name,"cuSourceNodePullDown"))
		{
			sprintf(cuSourceNodePullDown,"%.255s",entries[i].val);
			uSourceNode=ReadPullDown("tNode","cLabel",cuSourceNodePullDown);
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
		else if(!strcmp(entries[i].name,"uBackupDate"))
			sscanf(entries[i].val,"%lu",&uBackupDate);
		else if(!strcmp(entries[i].name,"gcNewContainerTZ"))
			sprintf(gcNewContainerTZ,"%.63s",entries[i].val);

		else if(!strcmp(entries[i].name,"cuSearchStatusPullDown"))
		{
			sprintf(cuSearchStatusPullDown,"%.255s",entries[i].val);
			uSearchStatus=ReadPullDown("tStatus","cLabel",cuSearchStatusPullDown);
		}
		else if(!strcmp(entries[i].name,"uSearchSource"))
			sscanf(entries[i].val,"%u",&uSearchSource);
		else if(!strcmp(entries[i].name,"uSearchStatusNotNoCA"))
			uSearchStatusNot=1;
		else if(!strcmp(entries[i].name,"cYesNouSearchSource"))
		{
			sprintf(cYesNouSearchSource,"%.31s",entries[i].val);
			uSearchSource=ReadYesNoPullDownTriState(cYesNouSearchSource);
		}

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
		sscanf(field[18],"%lu",&uBackupDate);

		}

	}//Internal Skip

	Header_ism3(":: OpenVZ Container",3);//3 toggle checkboxes js + calendar
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

	//Custom right panel for creating search sets
	if(guMode==12001)
		tContainerSearchSet(1);
	else if(guMode==12002)
		tContainerSearchSet(2);
	//Custom right panel for new containers
	else if(guMode==9001)
		tContainerNewStep(1);
	else if(guMode==9002)
		tContainerNewStep(2);
	else if(guMode==9003)
		tContainerNewStep(3);
	else if(guMode==9004)
		tContainerNewStep(4);
	else if(guMode==2002 || guMode==2000)
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


void tContainerSearchSet(unsigned uStep)
{
	OpenRow("<u>Set search parameters</u>","black");

	OpenRow("Label pattern/Special commands","black");
	printf("<input title='SQL search pattern %% and _ allowed' type=text name=cLabelSearch"
			" value=\"%s\" size=40 maxlength=32 >",cLabelSearch);
	//Temp placement
	printf("<td><textarea title='Special commands or cHostname list." 
					"\n\n"
					"\tcHostname list for remove/add (parameters are ignored) example:"
					"\n\thostname.yourisp.net"
					"\n\tanotherhost.yourisp.net"
					"\n\t..."
					"\n\tlasthost.yourisp.net"
					"\n\n"
					"Special commands are:\n1)\tSpecialSearchSet=NoRemoteClones;"
					"\n\t(must specify \"Datacenter\", optionally specify \"Primary Group\", only for create)."
					"\n2)\ttProperty=<cName>[,<cValue>];"
					"\n\t(Only for create, add and remove. Ignores other parameters.)'"
			" cols=80 wrap=soft rows=1 name=cCommands>%s</textarea></td>",cCommands);

	OpenRow("Hostname pattern","black");
	//Usability: Transfer from main tContainer page any current search pattern
	if(cSearch[0])
		sprintf(cHostnameSearch,"%.31s",cSearch);
	printf("<input title='SQL search pattern %% and _ allowed' type=text name=cHostnameSearch"
			" value=\"%s\" size=40 maxlength=63 >",cHostnameSearch);


	OpenRow("IPv4 pattern","black");
	printf("<input title='SQL search pattern %% and _ allowed' type=text name=cIPv4Search"
			" value=\"%s\" size=40 maxlength=15 >",cIPv4Search);
	OpenRow("Datacenter","black");
	//tTablePullDown("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,1);
	tTablePullDownActive("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,1);
	OpenRow("Node","black");
	//tTablePullDown("tNode;cuNodePullDown","cLabel","cLabel",uNode,1);
	tTablePullDownActive("tNode;cuNodePullDown","cLabel","cLabel",uNode,1);
	OpenRow("Owner","black");
	tTablePullDownResellers(uForClient,0);
	OpenRow("Clone","black");
	YesNoPullDownTriState("uSearchSource",uSearchSource,1);
	OpenRow("Status","black");
	tTablePullDown("tStatus;cuSearchStatusPullDown","cLabel","cLabel",uSearchStatus,1);
	printf("<input title='Logical NOT of selected status if any' type=checkbox name=uSearchStatusNotNoCA ");
	if(uSearchStatusNot)
		printf(" checked");
	printf("> Not");
	OpenRow("OSTemplate","black");
	tTablePullDown("tOSTemplate;cuOSTemplatePullDown","cLabel","cLabel",uOSTemplate,1);
	OpenRow("Primary group","black");
	tContainerGroupPullDown(uSearchGroup,1,"ctContainerSearchGroupPullDown");
	OpenRow("Source node","black");
	tTablePullDown("tNode;cuSourceNodePullDown","cLabel","cLabel",uSourceNode,1);
	OpenRow("Source datacenter","black");
	tTablePullDown("tDatacenter;cuSourceDatacenterPullDown","cLabel","cLabel",uSourceDatacenter,1);

	if(uStep==1)
	{
		;
	}
	else if(uStep==2)
	{
		;
	}

}//void tContainerSearchSet(unsigned uStep)


void tContainerNewStep(unsigned uStep)
{

	if(uStep==1)
	{
		OpenRow("Select an available datacenter","black");
		tTablePullDownActive("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,1);
		//Helper
		if(uNode)
			printf("<input type=hidden name=uNode value=%u >\n",uNode);
		OpenRow("Select an organization","black");
		uForClient=uOwner;
		tTablePullDownResellers(uForClient,0);
		//Helper
		if(uOwner)
			printf("<input type=hidden name=uOwner value=%u >\n",uOwner);
	}
	else if(uStep==2)
	{
		OpenRow("Selected datacenter","black");
		tTablePullDown("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,0);

		OpenRow("Selected organization","black");
		tTablePullDown("tClient;cuClientPullDown","cLabel","cLabel",uForClient,0);

		OpenRow("Select an available node","black");
		tTablePullDownDatacenter("tNode;cuNodePullDown","cLabel","cLabel",uNode,1,
			cuDatacenterPullDown,0,uDatacenter);//0 does not use tProperty, uses uDatacenter
	}
	else if(uStep==3 || uStep==4)
	{
		OpenRow("Selected datacenter","black");
		tTablePullDown("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,0);

		OpenRow("Selected organization","black");
		tTablePullDown("tClient;cuClientPullDown","cLabel","cLabel",uForClient,0);

		OpenRow("Selected node","black");
		tTablePullDown("tNode;cuNodePullDown","cLabel","cLabel",uNode,0);

		//uContainer
		if(uStep==3)
		{
			OpenRow(LANG_FL_tContainer_uContainer,"black");
			printf("<input title='Optional uContainer leave as 0 in almost all cases'"
				" type=text name=uContainer value=%u size=16 maxlength=10 ></td></tr>\n",uContainer);
		}

		//cLabel
		OpenRow(LANG_FL_tContainer_cLabel,"black");
		printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 ",
			LANG_FT_tContainer_cLabel,EncodeDoubleQuotes(cLabel));
		printf("></td></tr>\n");

		//cHostname
		OpenRow(LANG_FL_tContainer_cHostname,"black");
		printf("<input title='%s' type=text name=cHostname value=\"%s\" size=40 maxlength=64 "
			,LANG_FT_tContainer_cHostname,EncodeDoubleQuotes(cHostname));
		printf("></td></tr>\n");

		//uIPv4
		OpenRow(LANG_FL_tContainer_uIPv4,"black");
		tTablePullDownOwnerAvailDatacenter("tIP;cuIPv4PullDown","cLabel","cLabel",uIPv4,1,uDatacenter,uForClient);

		//uOSTemplate
		OpenRow(LANG_FL_tContainer_uOSTemplate,"black");
		tTablePullDownDatacenter("tOSTemplate;cuOSTemplatePullDown","cLabel","cLabel",uOSTemplate,1,
			cuDatacenterPullDown,uPROP_OSTEMPLATE,0);

		//uConfig
		OpenRow(LANG_FL_tContainer_uConfig,"black");
		tTablePullDownDatacenter("tConfig;cuConfigPullDown","cLabel","cLabel",uConfig,1,
			cuDatacenterPullDown,uPROP_CONFIG,0);

		//uNameserver
		OpenRow(LANG_FL_tContainer_uNameserver,"black");
		tTablePullDownDatacenter("tNameserver;cuNameserverPullDown","cLabel","cLabel",uNameserver,1,
			cuDatacenterPullDown,uPROP_NAMESERVER,0);

		//uSearchdomain
		OpenRow(LANG_FL_tContainer_uSearchdomain,"black");
		tTablePullDownDatacenter("tSearchdomain;cuSearchdomainPullDown","cLabel","cLabel",uSearchdomain,1,
			cuDatacenterPullDown,uPROP_SEARCHDOMAIN,0);

		//Select group or allow creation of new one.
		OpenRow("Select group","black");
		tTablePullDown("tGroup;cuGroupPullDown","cLabel","cLabel",uGroup,1);
		printf(" Create new group <input title='Instead of selecting an existing group optionally enter"
			" a new tGroup.cLabel' type=text name=cService3 value='%s' maxlength=31><br>",cService3);

		if(cunxsBindARecordJobZone[0])
		{
			OpenRow("Create job(s) for unxsBind","black");
			printf("<input type=checkbox name=uCreateDNSJob title=\"You should usually leave this checked. If you don't"
				" you will probably need to create DNS entries manually\"");
			if(uCreateDNSJob || uStep==3)
				printf("checked>");
			else
				printf(">");
		}

		//Select group or allow creation of new one.
		OpenRow("Optional password","black");
		if(!cService1[0])
			GeneratePasswd(cService1);
		printf("<input title='Optional container password set on deployment and saved in"
			" container property table' type=text name=cService1 value='%s' maxlength=31><br>",cService1);

		//Time zone
		MYSQL_RES *res;
		MYSQL_ROW field;
		OpenRow("Time Zone","black");
		sprintf(gcQuery,"SELECT cValue,cComment FROM tConfiguration WHERE cLabel='cTimeZone' ORDER BY uConfiguration");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		printf("<select class=type_textarea title='Select the time zone you want to use.'"
			" name=gcNewContainerTZ >\n");
		printf("<option>---</option>");
		while((field=mysql_fetch_row(res)))
		{
			printf("<option value=%s",field[0]);
			if(!strcmp(gcNewContainerTZ,field[0]))
				printf(" selected");
			printf(">%s</option>",field[1]);
		}
		mysql_free_result(res);
		printf("</select>\n");

		if(cAutoCloneNode[0])
		{
			char cAutoCloneSyncTime[256]={""};


/*
			char cAutoCloneNode[256]={""};
			unsigned uTargetDatacenter=0;
			MYSQL_RES *res;
			MYSQL_ROW field;
			OpenRow("Clone target node","black");
			GetConfiguration("cAutoCloneNode",cAutoCloneNode,uDatacenter,uNode,0,0);
			if(!cAutoCloneNode[0])
				GetConfiguration("cAutoCloneNode",cAutoCloneNode,uDatacenter,0,0,0);//All nodes
			sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%s'",cAutoCloneNode);
			MYSQL_RUN_STORE(res);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&uTargetNode);
				sscanf(field[1],"%u",&uTargetDatacenter);
				tTablePullDown("tNode;cuTargetNodePullDown","cLabel","cLabel",uTargetNode,0);//disabled
			}
			else
			{
				printf("Configuration error: '%s' %u %u",cAutoCloneNode,uDatacenter,uNode);
			}

			GetConfiguration("cAutoCloneNodeRemote",cAutoCloneNodeRemote,uDatacenter,uNode,0,0);
			if(!cAutoCloneNodeRemote[0])
				GetConfiguration("cAutoCloneNodeRemote",cAutoCloneNodeRemote,uDatacenter,0,0,0);
			if(cAutoCloneNodeRemote[0])
			{
				OpenRow("Backup target node","black");
				printf("%s",cAutoCloneNodeRemote);
			}

			OpenRow("Clone start uIPv4","black");
			tTablePullDownDatacenterCloneIPs("tIP;cuWizIPv4PullDown","cLabel","cLabel",uWizIPv4,1,
				uTargetDatacenter,uForClient,uTargetNode);

			OpenRow("Keep clone stopped","black");
			printf("<input type=checkbox name=uCloneStop ");
			if(uCloneStop || uStep==3)
				printf("checked>");
			else
				printf(">");
*/

			OpenRow("Clone cuSyncPeriod","black");
			GetConfiguration("cAutoCloneSyncTime",cAutoCloneSyncTime,uDatacenter,0,0,0);
			if(cAutoCloneSyncTime[0])
				sscanf(cAutoCloneSyncTime,"%u",&uSyncPeriod);
			printf("<input title='Keep clone in sync every cuSyncPeriod seconds"
					". You can change this at any time via the property panel.'"
					" type=text size=10 maxlength=7"
					" name=uSyncPeriod value='%u'>\n",uSyncPeriod);

			char cAutoNATIPClass[256]={""};
			unsigned uRemoteDatacenter=0;
			sscanf(ForeignKey("tNode","uDatacenter",uRemoteNode),"%u",&uRemoteDatacenter);
			GetConfiguration("cAutoNATIPClass",cAutoNATIPClass,uRemoteDatacenter,uRemoteNode,0,0);
			if(!cAutoNATIPClass[0])
				GetConfiguration("cAutoNATIPClass",cAutoNATIPClass,uRemoteDatacenter,0,0,0);
			if(cAutoNATIPClass[0] && uRemoteDatacenter)
			{
				OpenRow("Enable NAT","black");
				printf("<input type=checkbox name=uEnableNAT title='Enable NAT for remote clone container'");
				if(uEnableNAT || uStep==3)
					printf("checked>");
				else
					printf(">");
			}
		}

		if(uStep==4)
		{
			OpenRow("Number of containers","black");
			char cActiveContainers[256]={""};
			unsigned uActiveContainers=0;
			unsigned uMaxContainers=0;
			GetNodeProp(uNode,"ActiveContainers",cActiveContainers);
			sscanf(cActiveContainers,"%u",&uActiveContainers);
			GetNodeProp(uNode,"MaxContainers",cActiveContainers);
			sscanf(cActiveContainers,"%u",&uMaxContainers);
			if(!uMaxContainers) uMaxContainers=uActiveContainers;
			printf("<input title='Number of containers to be created'"
				" type=text name=cService2 value='%s' maxlength=2> Recommended max:%u",cService2,uMaxContainers-uActiveContainers);
		}

	}//step 3 or 4

}//void tContainerNewStep(unsigned uStep)


void tContainerInput(unsigned uMode)
{

	if(uContainer && uDatacenter)
	{
		char cConfigBuffer[256]={""};

		GetConfiguration("cTrafficDirURL",cConfigBuffer,uDatacenter,0,0,0);
		if(cConfigBuffer[0])
		{
	
			OpenRow("Graph","black");
			printf("<a href=%s/%u.html><img src=%s/%u.png border=0 onerror=\"this.src='/images/noimage.jpg'\"></a>\n",
					cConfigBuffer,uContainer,cConfigBuffer,uContainer);
			printf("</td></tr>\n");
		}
	}

//uContainer
	OpenRow(LANG_FL_tContainer_uContainer,"black");
	printf("<input title='%s' type=text name=uContainer value=%u size=16 maxlength=10 ",LANG_FT_tContainer_uContainer,uContainer);
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
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 ",LANG_FT_tContainer_cLabel,
		EncodeDoubleQuotes(cLabel));
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
	printf("<input title='%s' type=text name=cHostname value=\"%s\" size=40 maxlength=64 ",LANG_FT_tContainer_cHostname,
		EncodeDoubleQuotes(cHostname));
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
		//tTablePullDownOwnerAvail("tIP;cuIPv4PullDown","cLabel","cLabel",uIPv4,1);
		tTablePullDownOwnerAvailDatacenter("tIP;cuIPv4PullDown","cLabel","cLabel",uIPv4,1,uDatacenter,uForClient);
	else
		tTablePullDown("tIP;cuIPv4PullDown","cLabel","cLabel",uIPv4,0);
//uOSTemplate
	OpenRow(LANG_FL_tContainer_uOSTemplate,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownDatacenter("tOSTemplate;cuOSTemplatePullDown","cLabel","cLabel",uOSTemplate,1,
			cuDatacenterPullDown,uPROP_OSTEMPLATE,0);
	else
		tTablePullDown("tOSTemplate;cuOSTemplatePullDown","cLabel","cLabel",uOSTemplate,0);
//uConfig
	OpenRow(LANG_FL_tContainer_uConfig,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownDatacenter("tConfig;cuConfigPullDown","cLabel","cLabel",uConfig,1,
			cuDatacenterPullDown,uPROP_CONFIG,0);
	else
		tTablePullDown("tConfig;cuConfigPullDown","cLabel","cLabel",uConfig,0);
//uNameserver
	OpenRow(LANG_FL_tContainer_uNameserver,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownDatacenter("tNameserver;cuNameserverPullDown","cLabel","cLabel",uNameserver,1,
			cuDatacenterPullDown,uPROP_NAMESERVER,0);
	else
		tTablePullDown("tNameserver;cuNameserverPullDown","cLabel","cLabel",uNameserver,0);
//uSearchdomain
	OpenRow(LANG_FL_tContainer_uSearchdomain,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownDatacenter("tSearchdomain;cuSearchdomainPullDown","cLabel","cLabel",uSearchdomain,1,
			cuDatacenterPullDown,uPROP_SEARCHDOMAIN,0);
	else
		tTablePullDown("tSearchdomain;cuSearchdomainPullDown","cLabel","cLabel",uSearchdomain,0);
//uDatacenter
	OpenRow(LANG_FL_tContainer_uDatacenter,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDown("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,1);
	else
		tTablePullDown("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,0);
//uNode
	OpenRow(LANG_FL_tContainer_uNode,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDown("tNode;cuNodePullDown","cLabel","cLabel",uNode,1);
	else
		tTablePullDown("tNode;cuNodePullDown","cLabel","cLabel",uNode,0);
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
//uBackupDate
	OpenRow(LANG_FL_tContainer_uBackupDate,"black");
	if(uBackupDate)
		printf("%s\n\n",ctime(&uBackupDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uBackupDate value=%lu >\n",uBackupDate);
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
				"uCreatedDate=UNIX_TIMESTAMP(NOW()),uSource=%u",
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
			,uSource
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
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uContainer"
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>cHostname"
		"<td><font face=arial,helvetica color=white>uVeth"
		"<td><font face=arial,helvetica color=white>uIPv4"
		"<td><font face=arial,helvetica color=white>uOSTemplate"
		"<td><font face=arial,helvetica color=white>uConfig"
		"<td><font face=arial,helvetica color=white>uNameserver"
		"<td><font face=arial,helvetica color=white>uSearchdomain"
		"<td><font face=arial,helvetica color=white>uDatacenter"
		"<td><font face=arial,helvetica color=white>uNode"
		"<td><font face=arial,helvetica color=white>uStatus"
		"<td><font face=arial,helvetica color=white>uOwner"
		"<td><font face=arial,helvetica color=white>uCreatedBy"
		"<td><font face=arial,helvetica color=white>uCreatedDate"
		"<td><font face=arial,helvetica color=white>uModBy"
		"<td><font face=arial,helvetica color=white>uModDate"
		"<td><font face=arial,helvetica color=white>uBackupDate</tr>");

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
		time_t luTime18=strtoul(field[18],NULL,10);
		char cBuf18[32];
		if(luTime18)
			ctime_r(&luTime18,cBuf18);
		else
			sprintf(cBuf18,"---");
		printf("<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>"
			"%s<a><td>%s<td>%s<td>%s<td>%s<td>%s"
			"<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
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
				//17 is uSource
			,cBuf18
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tContainerList()


void CreatetContainer(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tContainer ( "
			"uContainer INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
			"uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner),"
			"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uBackupDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uDatacenter INT UNSIGNED NOT NULL DEFAULT 0, INDEX(uDatacenter),"
			"UNIQUE (cLabel,uDatacenter),"
			"uNode INT UNSIGNED NOT NULL DEFAULT 0,INDEX (uNode),"
			"uStatus INT UNSIGNED NOT NULL DEFAULT 0,"
			"uOSTemplate INT UNSIGNED NOT NULL DEFAULT 0,"
			"cHostname VARCHAR(64) NOT NULL DEFAULT '',"
			"uIPv4 INT UNSIGNED NOT NULL DEFAULT 0,"
			"uNameserver INT UNSIGNED NOT NULL DEFAULT 0,"
			"uSearchdomain INT UNSIGNED NOT NULL DEFAULT 0,"
			"uConfig INT UNSIGNED NOT NULL DEFAULT 0,"
			"uVeth INT UNSIGNED NOT NULL DEFAULT 0,"
			"uSource INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uSource) )");
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
                printf("Invalid input tTablePullDownAvail()");
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


	if(uSelector && !uMode)
	{
		if(guCompany==1)
        		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND _rowid=%u",
                                cFieldName,cLocalTableName,uSelector);
		else
			sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND uOwner=%u AND _rowid=%u",
				cFieldName,cLocalTableName,guCompany,uSelector);
	}
	else
	{
		if(guCompany==1)
        		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 ORDER BY %s",
                                cFieldName,cLocalTableName,cOrderby);
		else
			sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND uOwner=%u ORDER BY %s",
				cFieldName,cLocalTableName,guCompany,cOrderby);
	}

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


void tTablePullDownOwnerAvail(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode)
{
        register int i,n;
        char cLabel[256];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[100]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTablePullDownAvail()");
                return;
        }

        //Extended functionality
        strncpy(cLocalTableName,cTableName,255);
        if((cp=strchr(cLocalTableName,';')))
        {
                strncpy(cSelectName,cp+1,99);
                cSelectName[99]=0;
                *cp=0;
        }


	if(uSelector && !uMode)
	{
		if(guLoginClient==1)
        		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND _rowid=%u",
                                cFieldName,cLocalTableName,uSelector);
		else
        		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND ( uOwner=%u OR uOwner IN"
				" (SELECT uClient FROM " TCLIENT " WHERE uOwner=%u)) AND _rowid=%u",
				cFieldName,cLocalTableName,guCompany,guCompany,uSelector);
	}
	else
	{
		if(guLoginClient==1)
        		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 ORDER BY %s",
                                cFieldName,cLocalTableName,cOrderby);
		else
        		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND ( uOwner=%u OR uOwner IN"
				" (SELECT uClient FROM " TCLIENT " WHERE uOwner=%u)) ORDER BY %s",
				cFieldName,cLocalTableName,guCompany,guCompany,cOrderby);
	}

	MYSQL_RUN_STORE_TEXT_RET_VOID(mysqlRes);
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
		int unsigned field0;
                printf("<select name=%s %s>\n",cLabel,cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {

			field0=0;
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
			     sprintf(cHidden,"<input type=hidden name=%.99s value='%.99s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%99s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDownOwnerAvail()


void tTablePullDownDatacenter(const char *cTableName, const char *cFieldName,
		const char *cOrderby, unsigned uSelector, unsigned uMode, const char *cDatacenter,
		unsigned uType, unsigned uDatacenter)
{
        register int i,n;
        char cLabel[256];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[100]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";
	unsigned utNode=0;
	unsigned utDatacenter=0;

	if(!uMode)
		cMode="disabled";
	else if(uMode==2)
		cMode="onChange='submit()'";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTablePullDownDatacenter()");
                return;
        }

        //Extended functionality
        sprintf(cLocalTableName,"%.255s",cTableName);
        if((cp=strchr(cLocalTableName,';')))
        {
                sprintf(cSelectName,"%.99s",cp+1);
                *cp=0;
        }
	if(!strncmp(cTableName,"tNode",5))
		utNode=1;
	if(!strncmp(cTableName,"tDatacenter",11))
		utDatacenter=1;

	if(uSelector && !uMode)
	{
		if(uType)
			//UBC safe
	       		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE"
				" _rowid IN"
				" (SELECT uKey FROM tProperty WHERE cName='cDatacenter' AND"
				" uType=%u AND (cValue='All Datacenters' OR LOCATE('%s',cValue)>0))"
				" AND _rowid=%u",
					cFieldName,cLocalTableName,uType,cDatacenter,uSelector);
		else if(uDatacenter && (utDatacenter || utNode))
	       		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uDatacenter=%u AND uStatus=1 AND _rowid=%u",
				cFieldName,cLocalTableName,uDatacenter,uSelector);
		else if(uDatacenter)
	       		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uDatacenter=%u AND _rowid=%u",
				cFieldName,cLocalTableName,uDatacenter,uSelector);
		else if(1)
	       		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uStatus=1 AND _rowid=%u",
				cFieldName,cLocalTableName,uSelector);
	}
	else
	{
		if(uType)
			//UBC safe
	       		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE"
				" _rowid IN"
				" (SELECT uKey FROM tProperty WHERE cName='cDatacenter' AND"
				" uType=%u AND (cValue='All Datacenters' OR LOCATE('%s',cValue)>0))"
				" ORDER BY %s",
					cFieldName,cLocalTableName,uType,cDatacenter,cOrderby);
		else if(uDatacenter && (utDatacenter || utNode))
	       		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uDatacenter=%u and uStatus=1 ORDER BY %s",
				cFieldName,cLocalTableName,uDatacenter,cOrderby);
		else if(uDatacenter)
	       		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uDatacenter=%u ORDER BY %s",
				cFieldName,cLocalTableName,uDatacenter,cOrderby);
		else if(1)
	       		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uStatus=1 ORDER BY %s",
				cFieldName,cLocalTableName,cOrderby);
	}

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		return;
	}
	mysqlRes=mysql_store_result(&gMysql);
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
		int unsigned uField0;
		char cStyle[32]={""};
		char cValue[256]={""};

		if(utNode)
			printf("<select title='If selecting nodes, light yellow items are configured as clone only."
					" Greener nodes have more empty slots.' name=%s %s>\n",cLabel,cMode);
		else
			printf("<select title='no title content at this time' name=%s %s>\n",cLabel,cMode);
		//Default no selection
       		printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {
			uField0=0;
                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&uField0);

			if(utNode)
			{
				cValue[0]=0;
				GetNodeProp(uField0,"NewContainerMode",cValue);
				if(strstr(cValue,"Clone Only"))
				{
					sprintf(cStyle," style='background-color: #ffff99'");
				}
				else
				{
					cStyle[0]=0;
					unsigned uMaxContainers=0,uActiveContainers=0;
					//set background color based on how many free slots the node has.
					GetNodeProp(uField0,"MaxContainers",cValue);
					sscanf(cValue,"%u",&uMaxContainers);
					GetNodeProp(uField0,"ActiveContainers",cValue);
					sscanf(cValue,"%u",&uActiveContainers);
					if(uMaxContainers)
					{
						if((uMaxContainers-uActiveContainers)>16)
							//light green
							sprintf(cStyle," style='background-color: #009900'");
						else if((uMaxContainers-uActiveContainers)>8)
							//lighter green
							sprintf(cStyle," style='background-color: #00CC33'");
						else if((uMaxContainers-uActiveContainers)>4)
							//lightest green
							sprintf(cStyle," style='background-color: #00FF66'");
					}
					
				}
			}


			//Preselect if only one option.
                        if(uSelector==0 && i==1)
                        {
                             printf("<option%s selected>%s</option>\n",cStyle,mysqlField[1]);
			     if(!uMode)
			     sprintf(cHidden,"<input type=hidden name=%.99s value='%.99s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                        else if(uSelector != uField0)
                        {
                             printf("<option%s>%s</option>\n",cStyle,mysqlField[1]);
                        }
                        else if(1)
                        {
                             printf("<option%s selected>%s</option>\n",cStyle,mysqlField[1]);
			     if(!uMode)
			     sprintf(cHidden,"<input type=hidden name=%.99s value='%.99s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%99s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDownDatacenter()


void tTablePullDownOwnerAvailDatacenter(const char *cTableName, const char *cFieldName,
	const char *cOrderby, unsigned uSelector, unsigned uMode,unsigned uDatacenter,unsigned uClient)
{
        register int i,n;
        char cLabel[256];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[100]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTablePullDownAvailDatacenter()");
                return;
        }

        //Extended functionality
        strncpy(cLocalTableName,cTableName,255);
        if((cp=strchr(cLocalTableName,';')))
        {
                strncpy(cSelectName,cp+1,99);
                cSelectName[99]=0;
                *cp=0;
        }


	//Check for datacenter property NewContainerIPRange e.g. 12.23.34.0/24
	char cAutoIPClass[256]={""};
	GetConfiguration("cAutoIPClass",cAutoIPClass,uDatacenter,uNode,0,0);
	if(!cAutoIPClass[0])
		GetConfiguration("cAutoIPClass",cAutoIPClass,uDatacenter,0,0,0);
	//Allow optional 2nd class C
	char cAutoIPClass2[256]={""};
	GetConfiguration("cAutoIPClass2",cAutoIPClass2,uDatacenter,uNode,0,0);
	if(!cAutoIPClass2[0])
		GetConfiguration("cAutoIPClass2",cAutoIPClass2,uDatacenter,0,0,0);
	if(cAutoIPClass[0])
	{
		char *cp;
		if((cp=strstr(cAutoIPClass,".0/24"))) *cp=0;

		if(uSelector && !uMode)			
		{
			if(cAutoIPClass2[0])
				sprintf(gcQuery,"SELECT _rowid,%s FROM %s "
					" WHERE uAvailable=1"
					" AND (LOCATE('%s',tIP.cLabel)=1 OR LOCATE('%s',tIP.cLabel)=1)"
					" AND uDatacenter=%u"
					" AND uOwner=%u AND _rowid=%u",
						cFieldName,cLocalTableName,
						cAutoIPClass,cAutoIPClass2,
						uDatacenter,
						uClient,uSelector);
			else
				sprintf(gcQuery,"SELECT _rowid,%s FROM %s "
					" WHERE uAvailable=1"
					" AND LOCATE('%s',tIP.cLabel)=1"
					" AND uDatacenter=%u"
					" AND uOwner=%u AND _rowid=%u",
						cFieldName,cLocalTableName,
						cAutoIPClass,
						uDatacenter,
						uClient,uSelector);
		}
		else
		{
			if(cAutoIPClass2[0])
				sprintf(gcQuery,"SELECT _rowid,%s FROM %s "
					" WHERE uAvailable=1"
					" AND (LOCATE('%s',tIP.cLabel)=1 OR LOCATE('%s',tIP.cLabel)=1)"
					" AND uDatacenter=%u"
					" AND uOwner=%u ORDER BY %s",
						cFieldName,cLocalTableName,
						cAutoIPClass,cAutoIPClass2,
						uDatacenter,
						uClient,cOrderby);
			else
				sprintf(gcQuery,"SELECT _rowid,%s FROM %s "
					" WHERE uAvailable=1"
					" AND LOCATE('%s',tIP.cLabel)=1"
					" AND uDatacenter=%u"
					" AND uOwner=%u ORDER BY %s",
						cFieldName,cLocalTableName,
						cAutoIPClass,
						uDatacenter,
						uClient,cOrderby);
		}
	}
	else
	{
		if(uSelector && !uMode)			
			sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND uDatacenter=%u AND uOwner=%u AND _rowid=%u",
				cFieldName,cLocalTableName,uDatacenter,uClient,uSelector);
		else
			sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND uDatacenter=%u AND uOwner=%u ORDER BY %s",
				cFieldName,cLocalTableName,uDatacenter,uClient,cOrderby);
	}

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",gcQuery);
		return;
	}
	mysqlRes=mysql_store_result(&gMysql);
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
		int unsigned field0;
                printf("<select name=%s %s>\n",cLabel,cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {

			field0=0;
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
			     sprintf(cHidden,"<input type=hidden name=%.99s value='%.99s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%99s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDownOwnerAvailDatacenter()



void tTablePullDownDatacenterCloneIPs(const char *cTableName, const char *cFieldName,
	const char *cOrderby, unsigned uSelector, unsigned uMode,unsigned uDatacenter,unsigned uClient,unsigned uNode)
{
        register int i,n;
        char cLabel[256];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[100]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTablePullDownDatacenterCloneIPs()");
                return;
        }

        //Extended functionality
        strncpy(cLocalTableName,cTableName,255);
        if((cp=strchr(cLocalTableName,';')))
        {
                strncpy(cSelectName,cp+1,99);
                cSelectName[99]=0;
                *cp=0;
        }


	//Check for node property NewContainerCloneIPRange e.g. 172.17.0.0/24
	char cAutoCloneIPClass[256]={""};
	GetConfiguration("cAutoCloneIPClass",cAutoCloneIPClass,uDatacenter,uNode,0,0);
	if(!cAutoCloneIPClass[0])
		GetConfiguration("cAutoCloneIPClass",cAutoCloneIPClass,uDatacenter,0,0,0);
	//Allow for a second IP class
	char cAutoCloneIPClass2[256]={""};
	GetConfiguration("cAutoCloneIPClass2",cAutoCloneIPClass2,uDatacenter,uNode,0,0);
	if(!cAutoCloneIPClass2[0])
		GetConfiguration("cAutoCloneIPClass2",cAutoCloneIPClass2,uDatacenter,0,0,0);

	//debug only
	//printf("%s",cAutoCloneIPClass);
	//return;
	if(cAutoCloneIPClass[0])
	{
		char *cp;
		if((cp=strstr(cAutoCloneIPClass,".0/24"))) *cp=0;
		
		if(uSelector && !uMode)
		{	
			if(cAutoCloneIPClass2[0])
				sprintf(gcQuery,"SELECT _rowid,%s FROM %s "
				" WHERE uAvailable=1"
				" AND (LOCATE('%s',tIP.cLabel)=1 OR LOCATE('%s',tIP.cLabel)=1)"
				" AND uDatacenter=%u"
				" AND uOwner=%u AND _rowid=%u",
					cFieldName,cLocalTableName,
					cAutoCloneIPClass,cAutoCloneIPClass2,
					uDatacenter,
					uClient,uSelector);
			else
				sprintf(gcQuery,"SELECT _rowid,%s FROM %s "
				" WHERE uAvailable=1"
				" AND LOCATE('%s',tIP.cLabel)=1"
				" AND uDatacenter=%u"
				" AND uOwner=%u AND _rowid=%u",
					cFieldName,cLocalTableName,
					cAutoCloneIPClass,
					uDatacenter,
					uClient,uSelector);
		}
		else
		{	
			if(cAutoCloneIPClass2[0])
				sprintf(gcQuery,"SELECT _rowid,%s FROM %s "
				" WHERE uAvailable=1"
				" AND (LOCATE('%s',tIP.cLabel)=1 OR LOCATE('%s',tIP.cLabel)=1)"
				" AND uDatacenter=%u"
				" AND uOwner=%u ORDER BY %s",
					cFieldName,cLocalTableName,
					cAutoCloneIPClass,cAutoCloneIPClass2,
					uDatacenter,
					uClient,cOrderby);
			else
				sprintf(gcQuery,"SELECT _rowid,%s FROM %s "
				" WHERE uAvailable=1"
				" AND LOCATE('%s',tIP.cLabel)=1"
				" AND uDatacenter=%u"
				" AND uOwner=%u ORDER BY %s",
					cFieldName,cLocalTableName,
					cAutoCloneIPClass,
					uDatacenter,
					uClient,cOrderby);
		}
	}
	else
	{
		if(uSelector && !uMode)
			sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND uDatacenter=%u AND uOwner=%u AND _rowid=%u",
				cFieldName,cLocalTableName,uDatacenter,uClient,uSelector);
		else
			sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND uDatacenter=%u AND uOwner=%u ORDER BY %s",
				cFieldName,cLocalTableName,uDatacenter,uClient,cOrderby);
	}
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	//if(1)
	{
		printf("%s\n",gcQuery);
		return;
	}
	mysqlRes=mysql_store_result(&gMysql);
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
		int unsigned field0;
                printf("<select name=%s %s>\n",cLabel,cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {

			field0=0;
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
			     sprintf(cHidden,"<input type=hidden name=%.99s value='%.99s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%99s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDownDatacenterCloneIPs()


void tTablePullDownOwnerAnyAvailDatacenter(const char *cTableName, const char *cFieldName,
	const char *cOrderby, unsigned uSelector, unsigned uMode,unsigned uDatacenter,unsigned uClient)
{
        register int i,n;
        char cLabel[256];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[100]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTablePullDownOwnerAnyAvailDatacenter()");
                return;
        }

        //Extended functionality
        strncpy(cLocalTableName,cTableName,255);
        if((cp=strchr(cLocalTableName,';')))
        {
                strncpy(cSelectName,cp+1,99);
                cSelectName[99]=0;
                *cp=0;
        }

	if(uSelector && !uMode)
		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND uDatacenter=%u AND uOwner=%u AND _rowid=%u",
				cFieldName,cLocalTableName,uDatacenter,uClient,uSelector);
	else
		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND uDatacenter=%u AND uOwner=%u ORDER BY %s",
				cFieldName,cLocalTableName,uDatacenter,uClient,cOrderby);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",gcQuery);
		return;
	}
	mysqlRes=mysql_store_result(&gMysql);
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
		int unsigned field0;
                printf("<select name=%s %s>\n",cLabel,cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {

			field0=0;
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
			     sprintf(cHidden,"<input type=hidden name=%.99s value='%.99s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%99s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDownOwnerAnyAvailDatacenter()

