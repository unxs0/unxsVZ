/*
FILE
	$Id$
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis.
NOTES
	mySQL 5.0+ now required
 
*/

#define macro_mySQLQueryExitText	mysql_query(&gMysql,gcQuery);\
					if(mysql_errno(&gMysql))\
					{\
						printf("%s\n",mysql_error(&gMysql));\
						exit(1);\
					}\
					mysqlRes=mysql_store_result(&gMysql);

struct structContainer
{
	char cLabel[32];
	char cHostname[64];
	unsigned uVeth;
	unsigned uSource;
	unsigned uIPv4;
	unsigned uOSTemplate;
	unsigned uConfig;
	unsigned uNameserver;
	unsigned uSearchdomain;
	unsigned uDatacenter;
	unsigned uNode;
	unsigned uStatus;
	unsigned uOwner;
	unsigned uCreatedBy;
	unsigned long uCreatedDate;
	unsigned uModBy;
	unsigned long uModDate;
};
void GetContainerProps(unsigned uContainer,struct structContainer *sContainer);
void InitContainerProps(struct structContainer *sContainer);

static unsigned uHideProps=0;
static unsigned uTargetNode=0;
static char cuTargetNodePullDown[256]={""};
static unsigned uMountTemplate=0;
static char cuTemplateDropDown[256]={""};
static char cConfigLabel[32]={""};
static char cWizHostname[100]={""};
static char cWizLabel[32]={""};
static char cIPOld[32]={""};
static char cService1[32]={""};//Also used for optional container password
static char cService2[32]={""};
static char cService3[32]={""};
static char cService4[32]={""};
static char cPrivateIPs[64]={""};
static char cNetmask[64]={""};
static unsigned uWizIPv4=0;
static char cuWizIPv4PullDown[32]={""};
static unsigned uAllPortsOpen=0;
static unsigned uCloneStop=0;
static unsigned uSyncPeriod=0;
static char cSearch[32]={""};
static unsigned uGroupJobs=0;

//ModuleFunctionProtos()
void tContainerNavList(unsigned uNode, char *cSearch);
unsigned CreateNewContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned CreateStartContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned DestroyContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned StopContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned CancelContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
void SetContainerStatus(unsigned uContainer,unsigned uStatus);
void SetContainerNode(unsigned uContainer,unsigned uNode);
void htmlContainerNotes(unsigned uContainer);
void htmlContainerMount(unsigned uContainer);
unsigned MigrateContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer, unsigned uTargetNode);
unsigned CloneContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,
				unsigned uTargetNode, unsigned uNewVeid, unsigned uPrevStatus);
void htmlHealth(unsigned uContainer,unsigned uType);
void htmlGroups(unsigned uNode, unsigned uContainer);
unsigned TemplateContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned HostnameContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned IPContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned ActionScriptsJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned CloneNode(unsigned uSourceNode, unsigned uTargetNode, unsigned uWizIPv4);
char *cRatioColor(float *fRatio);
void htmlGenMountInputs(unsigned const uMountTemplate);
unsigned uCheckMountSettings(unsigned uMountTemplate);
void htmlMountTemplateSelect(unsigned uSelector);
void AddMountProps(unsigned uContainer);
void CopyContainerProps(unsigned uSource, unsigned uTarget);
//These two jobs are always done in pairs. Even though the second may run much later
//for example after hardware failure has been fixed.
unsigned FailoverToJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned FailoverFromJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,
				unsigned uIPv4,char *cLabel,char *cHostname,unsigned uSource,
				unsigned uStatus,unsigned uFailToJob);
void htmlCloneInfo(unsigned uContainer);

//extern
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);//jobqueue.c
void DelProperties(unsigned uNode,unsigned uType);//tnodefunc.h




void htmlGenMountInputs(unsigned const uMountTemplate)
{
	char *cService1;
	char *cService2;
	char *cService3;
	char *cService4;

	cService1="";
	cService2="";
	cService3="";
	cService4="";

	printf("Fill in according to template and container purpose."
		" If not sure leave blank or with suggested default value.\n");
	if(strstr(cuTemplateDropDown,"dns"))
	{
		cService1="53";
	}
	else if(strstr(cuTemplateDropDown,"web"))
	{
		cService1="80";
		cService2="443";
	}
	else if(strstr(cuTemplateDropDown,"mailstore"))
	{
		cService1="25";
		cService2="995";
		cService3="110";
	}
	else if(strstr(cuTemplateDropDown,"mx"))
	{
		cService1="25";
		cService2="587";
	}
	else if(strstr(cuTemplateDropDown,"smtp"))
	{
		cService1="25";
		cService2="587";
	}
	else if(strstr(cuTemplateDropDown,"mysql"))
	{
		cService1="3306";
	}
	else if(strstr(cuTemplateDropDown,"open"))
	{
		uAllPortsOpen=1;
		printf("<p>All ports open for uIPv4 container IP.");
	}

	if(!uAllPortsOpen)
	{
		printf("<p><input type=text name=cService1 value='%s'> Service1 Port<br>",cService1);
		printf("<input type=text name=cService2 value='%s'> Service2 Port<br>",cService2);
		printf("<input type=text name=cService3 value='%s'> Service3 Port<br>",cService3);
		printf("<input type=text name=cService4 value='%s'> Service4 Port<br>",cService4);
	}
	if(strstr(cuTemplateDropDown,"VE"))
	{
		printf("<p>DNAT/SNAT Mount Settings<p>Public or datacenter private IP<br>");
		tTablePullDownAvail("tIP;cuWizIPv4PullDown","cLabel","cLabel",uWizIPv4,1);
		printf("<br>Netmask for IP above<br>\n");
		printf("<input type=text name=cNetmask value='255.255.255.0'>");
		printf("<br>Datacenter wide private IP CIDR block<br>");
		printf("<input type=text name=cPrivateIPs value='10.0.0.0/24'>\n");
	}

}//void htmlGenMountInputs(unsigned const uMountTemplate)


unsigned uCheckMountSettings(unsigned uMountTemplate)
{

	//No mount/firewall option
	if(!uMountTemplate)
		return(0);

	if(!cuTemplateDropDown[0] && uMountTemplate)
	{
		MYSQL_RES *res;
		MYSQL_ROW field;

		sprintf(gcQuery,"SELECT cLabel FROM tTemplate WHERE uTemplate=%u",uMountTemplate);
	        mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
        	res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sprintf(cuTemplateDropDown,"%.99s",field[0]);	
		mysql_free_result(res);
		if(strstr(cuTemplateDropDown,"open"))
			uAllPortsOpen=1;
	}

	//Very basic check...add the rest TODO
	if(uWizIPv4 || uAllPortsOpen)
		return(0);
	else
		return(1);

}//unsigned uCheckMountSettings(unsigned uMountTemplate)


void ExtProcesstContainerVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.31s",entries[i].val);
		}
		else if(!strncmp(entries[i].name,"Ct",2))
		{
			//insider xss protection
			if(guPermLevel<12)
				continue;

			sscanf(entries[i].name,"Ct%u",&uContainer);
			if(uContainer)
			{
				if(!strcmp(gcFunction,"tContainerTools"))
				{
					if(!strcmp(gcCommand,"Group Stop"))
					{
						struct structContainer sContainer;

						InitContainerProps(&sContainer);
						GetContainerProps(uContainer,&sContainer);
						if(sContainer.uStatus==uACTIVE)
						{
							if(StopContainerJob(sContainer.uDatacenter,
									sContainer.uNode,uContainer))
							{
								SetContainerStatus(uContainer,uAWAITSTOP);
								uGroupJobs++;
							}
						}
					}
					else if(!strcmp(gcCommand,"Group Start"))
					{
						struct structContainer sContainer;

						InitContainerProps(&sContainer);
						GetContainerProps(uContainer,&sContainer);
						if(sContainer.uStatus==uSTOPPED)
						{
							if(CreateStartContainerJob(sContainer.uDatacenter,
									sContainer.uNode,uContainer))
							{
								SetContainerStatus(uContainer,uAWAITACT);
								uGroupJobs++;
							}
						}
					}
					else if(!strcmp(gcCommand,"Group Switchover"))
					{
						struct structContainer sContainer;

						InitContainerProps(&sContainer);
						GetContainerProps(uContainer,&sContainer);
						if( sContainer.uSource!=0 &&
							 (sContainer.uStatus==uSTOPPED || sContainer.uStatus==uACTIVE))
						{
							if(FailoverToJob(sContainer.uDatacenter,sContainer.uNode,
								uContainer))
							{
								unsigned uFailToJob=mysql_insert_id(&gMysql);
								unsigned uSourceDatacenter=0;
								unsigned uSourceNode=0;
								sscanf(ForeignKey("tContainer","uDatacenter",
									sContainer.uSource),
									"%u",&uSourceDatacenter);
								sscanf(ForeignKey("tContainer","uNode",
									sContainer.uSource),
									"%u",&uSourceNode);

//These two jobs are always done in pairs. Even though the second may run much later
//for example after hardware failure has been fixed.
								if(FailoverFromJob(uSourceDatacenter,uSourceNode,
									sContainer.uSource,sContainer.uIPv4,
									sContainer.cLabel,sContainer.cHostname,
										uContainer,sContainer.uStatus,
										uFailToJob))
								{
									SetContainerStatus(uContainer,uAWAITFAIL);
									SetContainerStatus(sContainer.uSource,uAWAITFAIL);
									uGroupJobs++;
								}
							}
						}
					}
				}
			}
		}
		else if(!strcmp(entries[i].name,"cuTargetNodePullDown"))
		{
			sprintf(cuTargetNodePullDown,"%.255s",entries[i].val);
			uTargetNode=ReadPullDown("tNode","cLabel",cuTargetNodePullDown);
		}
		else if(!strcmp(entries[i].name,"MountTemplateSelect"))
		{
			sprintf(cuTemplateDropDown,"%.255s",entries[i].val);
			uMountTemplate=ReadPullDown("tTemplate","cLabel",cuTemplateDropDown);
		}
		else if(!strcmp(entries[i].name,"uMountTemplate"))
		{
			sscanf(entries[i].val,"%u",&uMountTemplate);
		}
		else if(!strcmp(entries[i].name,"uCloneStop"))
		{
			uCloneStop=1;
		}
		else if(!strcmp(entries[i].name,"uSyncPeriod"))
		{
			sscanf(entries[i].val,"%u",&uSyncPeriod);
		}
		else if(!strcmp(entries[i].name,"cConfigLabel"))
		{
			sprintf(cConfigLabel,"%.31s",WordToLower(entries[i].val));
		}
		else if(!strcmp(entries[i].name,"cWizLabel"))
		{
			sprintf(cWizLabel,"%.31s",WordToLower(entries[i].val));
		}
		else if(!strcmp(entries[i].name,"cWizHostname"))
		{
			sprintf(cWizHostname,"%.99s",WordToLower(entries[i].val));
		}
		else if(!strcmp(entries[i].name,"cService1"))
		{
			sprintf(cService1,"%.31s",entries[i].val);
		}
		else if(!strcmp(entries[i].name,"cService2"))
		{
			sprintf(cService2,"%.31s",entries[i].val);
		}
		else if(!strcmp(entries[i].name,"cService3"))
		{
			sprintf(cService3,"%.31s",entries[i].val);
		}
		else if(!strcmp(entries[i].name,"cService4"))
		{
			sprintf(cService4,"%.31s",entries[i].val);
		}
		else if(!strcmp(entries[i].name,"cNetmask"))
		{
			sprintf(cNetmask,"%.63s",entries[i].val);
		}
		else if(!strcmp(entries[i].name,"cPrivateIPs"))
		{
			sprintf(cPrivateIPs,"%.63s",entries[i].val);
		}
		else if(!strcmp(entries[i].name,"cuWizIPv4PullDown"))
		{
			sprintf(cuWizIPv4PullDown,"%.31s",entries[i].val);
			uWizIPv4=ReadPullDown("tIP","cLabel",cuWizIPv4PullDown);
		}
	}

}//void ExtProcesstContainerVars(pentry entries[], int x)


void ExttContainerCommands(pentry entries[], int x)
{
	MYSQL_RES *res;

	if(!strcmp(gcFunction,"tContainerTools"))
	{
		unsigned uNodeDatacenter=0;
		time_t uActualModDate= -1;
		char cContainerType[256]={""};


		uHideProps=1;

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstContainerVars(entries,x);
                        	guMode=2000;
	                        tContainer("New container step 1/3");
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
                }
                else if(!strcmp(gcCommand,"Continue"))
                {
                        ProcesstContainerVars(entries,x);
			if(guPermLevel>=9)
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it!");

                        	guMode=200;
				//Check here

	                       	guMode=201;
				tContainer("New container step 3/3");
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Mount Settings") ||
			!strcmp(gcCommand,"Confirm Container Settings"))
                {
                        ProcesstContainerVars(entries,x);
			if(guPermLevel>=9)
			{

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it!");

                        	guMode=200;
				if(uStatus!=uINITSETUP)
					tContainer("<blink>Unexpected Error</blink>: uStatus not 'Initial Setup'");

                        	guMode=201;
				if(!uVeth)
				{
					if(uCheckMountSettings(uMountTemplate))
						tContainer("<blink>Error</blink>: Incorrect mount settings!");
					AddMountProps(uContainer);
				}
			
                        	guMode=0;
				tContainer("New container setup completed");
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
		//else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
		else if(!strcmp(gcCommand,"Confirm Container Fields"))
                {
			if(guPermLevel>=9)
			{
                        	ProcesstContainerVars(entries,x);

                        	guMode=2000;
				//Check entries here
				if(uDatacenter==0)
					tContainer("<blink>Error</blink>: uDatacenter==0!");
				if(uNode==0)
					tContainer("<blink>Error</blink>: uNode==0!");
				sscanf(ForeignKey("tNode","uDatacenter",uNode),"%u",&uNodeDatacenter);
				if(uDatacenter!=uNodeDatacenter)
					tContainer("<blink>Error</blink>: The specified uNode does not "
							"belong to the specified uDatacenter.");

				if(uConfig==0)
					tContainer("<blink>Error</blink>: uConfig==0!");
				if(uNameserver==0)
					tContainer("<blink>Error</blink>: uNameserver==0!");
				if(uSearchdomain==0)
					tContainer("<blink>Error</blink>: uSearchdomain==0!");
				if(strlen(cHostname)<5)
					tContainer("<blink>Error</blink>: cHostname too short!");
				if(strlen(cLabel)<2)
					tContainer("<blink>Error</blink>: cLabel too short!");
				if(strchr(cLabel,'.'))
					tContainer("<blink>Error</blink>: cLabel has at least one '.'!");
				if(strstr(cLabel,"-clone"))
					tContainer("<blink>Error</blink>: cLabel can't have '-clone'!");
				if(strstr(cHostname,".clone"))
					tContainer("<blink>Error</blink>: cHostname can't have '.clone'!");
				if(uVeth)
				{
					GetNodeProp(uNode,"Container-Type",cContainerType);
					if(!strstr(cContainerType,"VETH"))
						tContainer("<blink>Error</blink>: uNode selected does not support VETH!");
						
				}
				//No same names or hostnames for same datacenter allowed.
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE (cHostname='%s' OR cLabel='%s')"
						" AND uDatacenter=%u",cHostname,cLabel,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
				{
					mysql_free_result(res);
					tContainer("<blink>Error</blink>: cHostname or cLabel already used at this"
							" datacenter!");
				}
				mysql_free_result(res);

				if(uIPv4==0)
					tContainer("<blink>Error</blink>: uIPv4==0!");
				
				if(uOSTemplate==0)
					tContainer("<blink>Error</blink>: uOSTemplate==0!");

                        	guMode=200;

				uStatus=uINITSETUP;//Initial setup
				uContainer=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtContainer(1);
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
						" WHERE uIP=%u AND uAvailable=1",uIPv4);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				if(mysql_affected_rows(&gMysql)!=1)
				{
					sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",uContainer);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					tContainer("<blink>Error</blink>: Someone grabbed your IP"
							", No container created!");
				}
				sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='Name',cValue='%s'",
							uContainer,guCompany,guLoginClient,cLabel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				if(cService1[0])
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='cPasswd',cValue='%s'",
							uContainer,guCompany,guLoginClient,cService1);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				}

				//Add property template entries for VETH device based container
				if(uVeth)
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='cVEID.mount',cValue='defaultVETH.mount'",
							uContainer,guCompany,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));

					sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='cVEID.start',cValue='defaultVETH.start'",
							uContainer,guCompany,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				}

				tContainer("New container created and default properties created");
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstContainerVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

	                        guMode=2001;
				tContainer(LANG_NB_CONFIRMDEL);
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstContainerVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

				guMode=5;
				uStatus=0;//Internal hack for deploy button
				//Release IPs
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=1"
						" WHERE uIP=%u and uAvailable=0",uIPv4);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				//Node IP if any MySQL5+
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=1 WHERE cLabel IN"
						" (SELECT cValue FROM tProperty WHERE uKey=%u"
						" AND uType=3 AND cName='cNodeIP')",uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				//Now we can remove properties
				DelProperties(uContainer,3);
				//Remove from any groups
				sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer=%u",uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				//Cancel any outstanding jobs. TODO review this further
				CancelContainerJob(uDatacenter,uNode,uContainer);
				DeletetContainer();
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstContainerVars(entries,x);
			if( (uStatus==uINITSETUP) && uAllowMod(uOwner,uCreatedBy))
			{
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

				guMode=2002;

				//UI trick: Set the selected IP as available, otherwise it doesn't show up
				//at the IP dropdown. Upon confirm modify it will be set unavailable again
				//or if the IP is changed, it will be 'released' by this query below ;)
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=1"
						" WHERE uIP=%u AND uAvailable=0",uIPv4);
						
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				tContainer(LANG_NB_CONFIRMMOD);
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstContainerVars(entries,x);
			if( (uStatus==uINITSETUP) && uAllowMod(uOwner,uCreatedBy))
			{
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

                        	guMode=2002;
				//Check entries here
				if(uContainer==0)
					tContainer("<blink>Error</blink>: uContainer==0!");
				if(uDatacenter==0)
					tContainer("<blink>Error</blink>: uDatacenter==0!");
				if(uNode==0)
					tContainer("<blink>Error</blink>: uNode==0!");
				sscanf(ForeignKey("tNode","uDatacenter",uNode),"%u",&uNodeDatacenter);
				if(uDatacenter!=uNodeDatacenter)
					tContainer("<blink>Error</blink>: The specified uNode does not "
							"belong to the specified uDatacenter.");
				if(uIPv4==0)
					tContainer("<blink>Error</blink>: uIPv4==0!");
				if(uOSTemplate==0)
					tContainer("<blink>Error</blink>: uOSTemplate==0!");
				if(uConfig==0)
					tContainer("<blink>Error</blink>: uConfig==0!");
				if(uNameserver==0)
					tContainer("<blink>Error</blink>: uNameserver==0!");
				if(uSearchdomain==0)
					tContainer("<blink>Error</blink>: uSearchdomain==0!");
				if(strlen(cHostname)<5)
					tContainer("<blink>Error</blink>: cHostname too short!");
				if(strlen(cLabel)<2)
					tContainer("<blink>Error</blink>: cLabel too short!");
				if(strstr(cLabel,"-clone"))
					tContainer("<blink>Error</blink>: cLabel can't have '-clone'!");
				if(strstr(cHostname,".clone"))
					tContainer("<blink>Error</blink>: cHostname can't have '.clone'!");
				//No same names or hostnames for same datacenter allowed.
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE (cHostname='%s' OR cLabel='%s')"
						" AND uContainer!=%u",
							cHostname,cLabel,uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
				{
					mysql_free_result(res);
					tContainer("<blink>Error</blink>: cHostname or cLabel already used at this"
							" datacenter!");
				}
				mysql_free_result(res);
                        	guMode=0;

				//Set the selected IP as not available upon modify
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
						" WHERE uIP=%u AND uAvailable=1",uIPv4);
						
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
					
				uModBy=guLoginClient;
				ModtContainer();
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
                }
                else if(!strncmp(gcCommand,"Start ",6))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uSTOPPED && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

				if(CreateStartContainerJob(uDatacenter,uNode,uContainer))
				{
					uStatus=uAWAITACT;
					SetContainerStatus(uContainer,6);//Awaiting Activation
					//Since the above function changes the mod date
					//and we want to avoid frivoulous reload error msgs:
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("CreateStartContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error</blink>: No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strncmp(gcCommand,"Deploy ",7))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uINITSETUP && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

				if(CreateNewContainerJob(uDatacenter,uNode,uContainer))
				{
					uStatus=uAWAITACT;
					SetContainerStatus(uContainer,6);//Awaiting Activation
					//Since the above function changes the mod date
					//and we want to avoid frivoulous reload error msgs:
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					uModBy=guLoginClient;
					tContainer("CreateNewContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error</blink>: No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strncmp(gcCommand,"Destroy ",8))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowDel(uOwner,uCreatedBy))
			{
                        	guMode=0;
					
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

				if(DestroyContainerJob(uDatacenter,uNode,uContainer))
				{
					uStatus=uAWAITDEL;
					SetContainerStatus(uContainer,5);//Awaiting Deletion
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("DestroyContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error</blink>: No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strncmp(gcCommand,"Stop ",5))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uACTIVE && uAllowDel(uOwner,uCreatedBy))
			{
                        	guMode=0;
					
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

				if(StopContainerJob(uDatacenter,uNode,uContainer))
				{
					uStatus=uAWAITSTOP;
					SetContainerStatus(uContainer,41);//Awaiting Stop
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("StopContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error</blink>: No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Cancel Job"))
                {
                        ProcesstContainerVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;
				if(uStatus!=uAWAITDEL && uStatus!=uAWAITACT && uStatus!=uAWAITSTOP)
					tContainer("<blink>Error</blink>: Unexpected uStatus!");

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Job may have run!");

				if(CancelContainerJob(uDatacenter,uNode,uContainer))
				{
					tContainer("<blink>Error</blink>: Unexpected no jobs canceled! Late?");
				}
				else
				{
					if(uStatus==uAWAITDEL || uStatus==uAWAITSTOP)
					{
						uStatus=uACTIVE;
						SetContainerStatus(uContainer,1);//Awaiting Deletion/Stop to Active
					}
					else if(uStatus==uAWAITACT)
					{
						uStatus=uINITSETUP;
						SetContainerStatus(uContainer,uINITSETUP);//Awaiting Activation to Initial
					}
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("CancelContainerJob() Done");
				}
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Create Action Scripts"))
                {
                        ProcesstContainerVars(entries,x);
			if( (uStatus==uSTOPPED || uStatus==uINITSETUP ) && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

				if(ActionScriptsJob(uDatacenter,uNode,uContainer))
				{
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("ActionScriptsJob() Done");
				}
				else
				{
					tContainer("<blink>Error</blink>: No jobs created!"
							" Make sure correct properties exist.");
				}
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Clone Wizard"))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");
				
				guMode=7001;
				tContainer("Select Target Node and IPv4");
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Clone"))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
				unsigned uNewVeid=0;
				char cTargetNodeIPv4[32]={""};
				unsigned uWizLabelSuffix=0;
				unsigned uWizLabelLoop=1;


                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it!");

                        	guMode=7001;
				if(!uWizIPv4)
					tContainer("<blink>Error</blink>: You must select an IP!");
				if(uTargetNode==0)
					tContainer("<blink>Error</blink>: Please select a valid target node!");
				if(uTargetNode==uNode)
					tContainer("<blink>Error</blink>: Can't clone to same node!");
				GetNodeProp(uTargetNode,"cIPv4",cTargetNodeIPv4);
				if(!cTargetNodeIPv4[0])
					tContainer("<blink>Error</blink>: Your target node is"
							" missing it's cIPv4 property!");
				if(uSyncPeriod>86400*30)
					tContainer("<blink>Error</blink>: uSyncPeriod out of range, max 30 days!");
				if(uVeth)
				{
					GetNodeProp(uNode,"Container-Type",cContainerType);
					if(!strstr(cContainerType,"VETH"))
						tContainer("<blink>Error</blink>: uNode selected does not support VETH!");
						
				}
                        	guMode=0;
				
				while(uWizLabelLoop)
				{
					uWizLabelSuffix++;
					sprintf(cWizLabel,"%.25s-clone%u",cLabel,uWizLabelSuffix);
					sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE cLabel='%s'",cWizLabel);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					res=mysql_store_result(&gMysql);
					uWizLabelLoop=mysql_num_rows(res);
					mysql_free_result(res);
				}
				sprintf(cWizHostname,"%.93s.clone%u",cHostname,uWizLabelSuffix);

				sprintf(gcQuery,"INSERT INTO tContainer SET cLabel='%s',"
							"cHostname='%s',"
							"uIPv4=%u,"
							"uOSTemplate=%u,"
							"uConfig=%u,"
							"uNameserver=%u,"
							"uSearchdomain=%u,"
							"uDatacenter=%u,"
							"uNode=%u,"
							"uStatus=81,"//uAWAITCLONE
							"uOwner=%u,"
							"uCreatedBy=%u,"
							"uSource=%u,"
							"uCreatedDate=UNIX_TIMESTAMP(NOW())",
							cWizLabel,
							cWizHostname,
							uWizIPv4,
							uOSTemplate,
							uConfig,
							uNameserver,
							uSearchdomain,
							uDatacenter,
							uTargetNode,
							guCompany,
							guLoginClient,
							uContainer);
        			mysql_query(&gMysql,gcQuery);
			        if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				uNewVeid=mysql_insert_id(&gMysql);


				if(CloneContainerJob(uDatacenter,uNode,uContainer,uTargetNode,uNewVeid,uStatus))
				{
					sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
							" WHERE uIP=%u AND uAvailable=1",uWizIPv4);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					if(mysql_affected_rows(&gMysql)!=1)
					{
						sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",uNewVeid);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						tContainer("<blink>Error</blink>: Someone grabbed your IP"
								", No jobs created!");
					}
					CopyContainerProps(uContainer,uNewVeid);
					sprintf(gcQuery,"DELETE FROM tProperty WHERE"
							" cName='Name' AND uKey=%u AND uType=3",uNewVeid);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
							",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
							",cName='Name',cValue='%s'",
								uNewVeid,guCompany,guLoginClient,cWizLabel);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					//default no sync period set
					sprintf(gcQuery,"DELETE FROM tProperty WHERE"
							" cName='cuSyncPeriod' AND uKey=%u AND uType=3",uNewVeid);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
							",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
							",cName='cuSyncPeriod',cValue='%u'",
								uNewVeid,guCompany,guLoginClient,uSyncPeriod);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					uModBy=guLoginClient;
					uStatus=uAWAITCLONE;
					SetContainerStatus(uContainer,uAWAITCLONE);
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("CloneContainerJob() Done");
				}
				else
				{
					sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",uNewVeid);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					tContainer("<blink>Error</blink>: No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Migration Wizard"))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");
				guMode=3001;
				tContainer("Select Migration Target");
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Migration"))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

                        	guMode=3001;
				if(uTargetNode==uNode)
					tContainer("<blink>Error</blink>: Can't migrate to same node. Try 'Template Wizard'");
				if(uTargetNode==0)
					tContainer("<blink>Error</blink>: Please select a valid target node");
				if(uVeth)
				{
					GetNodeProp(uNode,"Container-Type",cContainerType);
					if(!strstr(cContainerType,"VETH"))
						tContainer("<blink>Error</blink>: uNode selected does not support VETH!");
						
				}
                        	guMode=0;

				if(MigrateContainerJob(uDatacenter,uNode,uContainer,uTargetNode))
				{
					uStatus=uAWAITMIG;
					SetContainerStatus(uContainer,21);//Awaiting Migration
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("MigrateContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error</blink>: No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Template Wizard"))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");
				if(!uOSTemplate)
					tContainer("<blink>Error</blink>: No tOSTemplate.cLabel!");
				guMode=4001;
				tContainer("Select Template Name");
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Template"))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
			{
        			MYSQL_ROW field;
				char cOSTLabel[101]={""};

                        	guMode=0;
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");
                        	guMode=4001;
				if(strlen(cConfigLabel)<2)
					tContainer("<blink>Error</blink>: Must provide valid tConfig.cLabel!");
				if(strchr(cConfigLabel,'-'))
					tContainer("<blink>Error</blink>: tConfig.cLabel can't contain '-' chars!");
				sprintf(gcQuery,"SELECT uConfig FROM tConfig WHERE cLabel='%s'",cConfigLabel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
					tContainer("<blink>Error</blink>: tConfig.cLabel already exists create another!");
				mysql_free_result(res);
				sprintf(gcQuery,"SELECT cLabel FROM tOSTemplate WHERE uOSTemplate=%u",uOSTemplate);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
					sprintf(cOSTLabel,"%.100s",field[0]);
				else
					tContainer("<blink>Error</blink>: No tOSTemplate.cLabel!");
				mysql_free_result(res);
				sprintf(gcQuery,"SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='%.67s-%.32s'",
						cOSTLabel,cConfigLabel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
					tContainer("<blink>Error</blink>: tOSTemplate.cLabel collision."
							" Select another tConfig.cLabel!");
				mysql_free_result(res);
                        	guMode=0;
				if(TemplateContainerJob(uDatacenter,uNode,uContainer))
				{
					uStatus=uAWAITTML;
					SetContainerStatus(uContainer,51);
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("TemplateContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error</blink>: No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Hostname Change Wizard"))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");
				guMode=5001;
				tContainer("Provide container hostname and name");
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Hostname Change"))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");
                        	guMode=5001;
				if(strlen(cWizHostname)<5)
					tContainer("<blink>Error</blink>: cHostname too short!");
				if(strlen(cWizLabel)<2)
					tContainer("<blink>Error</blink>: cLabel too short!");
				if(strchr(cWizLabel,'.'))
					tContainer("<blink>Error</blink>: cLabel has at least one '.'!");
				//No same names or hostnames for same datacenter allowed.
				//TODO must be fixed to allow only label or only hostname changes
				//this involves ignoring existing label or hostname for this container.
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE (cHostname='%s' OR cLabel='%s')"
						" AND uDatacenter=%u",cWizHostname,cWizLabel,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
				{
					mysql_free_result(res);
					tContainer("<blink>Error</blink>: cHostname or cLabel already used at this"
							" datacenter!");
				}
				mysql_free_result(res);
                        	guMode=0;

				sprintf(gcQuery,"UPDATE tContainer SET cLabel='%s',cHostname='%s'"
						" WHERE uContainer=%u",cWizLabel,cWizHostname,uContainer);
        			mysql_query(&gMysql,gcQuery);
			        if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				sprintf(cLabel,"%.31s",cWizLabel);
				sprintf(cHostname,"%.99s",cWizHostname);
				if(HostnameContainerJob(uDatacenter,uNode,uContainer))
				{
					uStatus=uAWAITHOST;
					SetContainerStatus(uContainer,61);
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("HostnameContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error</blink>: No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"IP Change Wizard"))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");
				guMode=6001;
				tContainer("Select new IPv4");
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm IP Change"))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
			{
				unsigned uOldIPv4;

                        	guMode=0;
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");
                        	guMode=6001;
				if(!uWizIPv4)
					tContainer("<blink>Error</blink>: You must select an IP!");
                        	guMode=0;

				//Fatal error section
				sscanf(ForeignKey("tContainer","uIPv4",uContainer),"%u",&uOldIPv4);
				if(!uOldIPv4)
					htmlPlainTextError("Unexpected !uIPv4 and !uOldIPv4");
				sprintf(cIPOld,"%.31s",ForeignKey("tIP","cLabel",uOldIPv4));
				if(!cIPOld[0])
					htmlPlainTextError("Unexpected !cIPOldv4");

				//New uIPv4
				sprintf(gcQuery,"UPDATE tContainer SET uIPv4=%u"
						" WHERE uContainer=%u",uWizIPv4,uContainer);
        			mysql_query(&gMysql,gcQuery);
			        if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
						" WHERE uIP=%u",uWizIPv4);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				uIPv4=uWizIPv4;
				if(IPContainerJob(uDatacenter,uNode,uContainer))
				{
					uStatus=uAWAITIP;
					SetContainerStatus(uContainer,71);
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("IPContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error</blink>: No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strncmp(gcCommand,"Failover",8))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");
				if(!uDatacenter)
					tContainer("<blink>Error</blink>: Unexpected lack of uDatacenter.");
				if(!uNode)
					tContainer("<blink>Error</blink>: Unexpected lack of uNode.");
				if(!uSource)
					tContainer("<blink>Error</blink>: Unexpected lack of uSource.");
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uContainer=%u AND"
						" (uStatus=1 OR uStatus=31 OR uStatus=41)",uSource);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)==0)
					tContainer("<blink>Error</blink>: Unexpected source container status!");
				guMode=8001;
				tContainer("Review and confirm");
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Failover"))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");
                        	guMode=8001;
				if(!uSource)
					tContainer("<blink>Error</blink>: Unexpected lack of uSource!");
				if(!uDatacenter)
					tContainer("<blink>Error</blink>: Unexpected lack of uDatacenter.");
				if(!uNode)
					tContainer("<blink>Error</blink>: Unexpected lack of uNode.");
				if(!cLabel[0])
					tContainer("<blink>Error</blink>: Unexpected lack of cLabel.");
				if(!cHostname[0])
					tContainer("<blink>Error</blink>: Unexpected lack of cHostname.");
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uContainer=%u AND"
						" (uStatus=1 OR uStatus=31 OR uStatus=41)",uSource);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)==0)
					tContainer("<blink>Error</blink>: Unexpected source container status!");
                        	guMode=0;

				if(FailoverToJob(uDatacenter,uNode,uContainer))
				{
					unsigned uFailToJob=mysql_insert_id(&gMysql);
					unsigned uSourceDatacenter=0;
					unsigned uSourceNode=0;

					sscanf(ForeignKey("tContainer","uDatacenter",uSource),"%u",&uSourceDatacenter);
					sscanf(ForeignKey("tContainer","uNode",uSource),"%u",&uSourceNode);

//These two jobs are always done in pairs. Even though the second may run much later
//for example after hardware failure has been fixed.
					if(FailoverFromJob(uSourceDatacenter,uSourceNode,uSource,uIPv4,
							cLabel,cHostname,uContainer,uStatus,uFailToJob))
					{
						uStatus=uAWAITFAIL;
						SetContainerStatus(uContainer,uAWAITFAIL);
						SetContainerStatus(uSource,uAWAITFAIL);
						sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
						tContainer("FailoverJob() Done");
					}
					else
					{
						tContainer("<blink>Error</blink>: No FailoverFromJob() created!");
					}
				}
				else
				{
					tContainer("<blink>Error</blink>: No FailoverToJob created!");
				}
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strncmp(gcCommand,"Group",5))
                {
			ExtProcesstContainerVars(entries,x);
			if(uGroupJobs)
				tContainer("'Group' jobs created");
			else
				tContainer("No 'Group' jobs created");
		}
	}

}//void ExttContainerCommands(pentry entries[], int x)


void ExttContainerButtons(void)
{
	OpenFieldSet("tContainer Aux Panel",100);
	switch(guMode)
        {
                case 6001:
                        printf("<p><u>IP Change Wizard</u><br>");
			printf("Here you will change the container IPv4 number."
				" The container will be stopped then started after the IP is changed to"
				" insure that any mount/umount scripts that reference container IP"
				" are handled correctly."
				" <p>Container services may be affected or need reconfiguration for new IP.<p>\n");
			printf("Select new IPv4 ");
			tTablePullDownAvail("tIP;cuWizIPv4PullDown","cLabel","cLabel",uWizIPv4,1);
			printf("<p><input title='Create an IP change job for the current container'"
					" type=submit class=lwarnButton"
					" name=gcCommand value='Confirm IP Change'>\n");
                break;

                case 5001:
                        printf("<p><u>Hostname Change Wizard</u><br>");
			printf("Here you will change the container label (name) and hostname."
				" The container will not be stopped. Only operations depending on the /etc/hosts file"
				" and the hostname may be affected.<p>\n");
			printf("<p>New cLabel <input title='Short container name'"
					" type=text name=cWizLabel maxlength=31>\n");
			printf("<p>New cHostname <input title='FQDN container hostname'"
					" type=text name=cWizHostname maxlength=99>\n");
			printf("<p><input title='Create a hostname change job for the current container'"
					" type=submit class=lwarnButton"
					" name=gcCommand value='Confirm Hostname Change'>\n");
                break;

                case 4001:
                        printf("<p><u>Template Wizard</u><br>");
			printf("Here you will select the tConfig.cLabel. This label will be"
				" be used to create a new OS template based on the source container's OS template"
				" and the tConfig.cLabel as a dash suffix. This label will also be used for a new"
				" VZ conf file.<p>If you are not using LVM: <font color=red>!The container"
				" will be stopped for several minutes!</font>"
				"<p>When the job is finished the tOSTemplate.cLabel and it's associated base conf file"
				" (tConfig.cLabel) will be available on every hardware node for immediate use"
				" (the /usr/sbin/allnodescp.sh has to be installed and configured correctly.)"
				" The all node /vz/template/cache/ file that is created can also be likened"
				" to a snapshot backup of the running container.<p>\n");
			printf("<p>tConfig.cLabel <input title='tOSTemplate tail and tConfig label will be set to"
					" the name provided.' type=text name=cConfigLabel maxlength=32>\n");
			printf("<p><input title='Create a template job for the current container'"
					" type=submit class=lwarnButton"
					" name=gcCommand value='Confirm Template'>\n");
                break;

                case 3001:
                        printf("<p><u>Migration Wizard</u><br>");
			printf("Here you will select the hardware node target. If the selected node is"
				" oversubscribed, not available, or scheduled for maintenance. You will"
				" be informed at the next step\n<p>\n");
			tTablePullDown("tNode;cuTargetNodePullDown","cLabel","cLabel",uTargetNode,1);
			printf("<p><input title='Create a migration job for the current container'"
					" type=submit class=largeButton"
					" name=gcCommand value='Confirm Migration'>\n");
                break;

                case 7001:
                        printf("<p><u>Clone Wizard</u><br>");
			printf("Here you will select the hardware node target. If the selected node is"
				" oversubscribed, not available, or scheduled for maintenance. You will"
				" be informed at the next step.\n<p>\n"
				"You also must carefully select a new IP that usually will be of the same type"
				" as the source container. Caveat-emptor.<p>"
				"Any mount/umount files of the source container will not be used"
				" by the new cloned container. This issue will be left for manual"
				" or automated failover to the cloned container. If you wish to"
				" keep the source and clone container sync'ed you must specify that"
				" in the cloned container via the 'cuSyncPeriod' entry in it's properties table.<p>"
				"Usually clones should be kept stopped to conserve resources until required"
				" for failover. Use the checkbox to change this default behavior.<p>");
			printf("Select target node ");
			tTablePullDown("tNode;cuTargetNodePullDown","cLabel","cLabel",uTargetNode,1);
			printf("<br>Select new IPv4 ");
			tTablePullDownAvail("tIP;cuWizIPv4PullDown","cLabel","cLabel",uWizIPv4,1);
			printf("<br>Keep clone stopped <input type=checkbox name=uCloneStop checked>");
			printf("<br>cuSyncPeriod <input title='Keep clone in sync every cuSyncPeriod seconds"
					". You can change this at any time via the property panel.'"
					" type=text size=10 maxlength=7"
					" name=uSyncPeriod value='%u'>\n",uSyncPeriod);
			
			printf("<p><input title='Create a clone job for the current container'"
					" type=submit class=largeButton"
					" name=gcCommand value='Confirm Clone'>\n");
                break;

                case 8001:
                        printf("<p><u>Failover</u><br>");
			printf("Confirm all the information presented for a manual failover (switchover) to take place."
				"<p>Jobs will be created for the source and this container. If jobs run successfully,"
				" everything but the container VEIDs will be switched.<p>This clone (renamed to the"
				" source names) will be the new production container and the source container will"
				" be the clone container and kept in sync if cuSyncPeriod is changed to a non 0 value."
				" This should only be done after confirmation that switchover container works fine.");
                        printf("<p><u>Failover Data</u>");
			if(uSource)
			{
				printf("<br>%s will replace ",cLabel);
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%u>"
					"%s</a>",uSource,ForeignKey("tContainer","cLabel",uSource));
			}
			printf("<p><input title='Creates manual failover (switchover) jobs for the current container'"
					" type=submit class=lwarnButton"
					" name=gcCommand value='Confirm Failover'>\n");
                break;

                case 200:
			printf("<p><u>New container step 2/3</u><br>");
			if(!uVeth)
			{
				printf("Optionally select node mount template for new container.");
				printf("Please select node mount template for new container."
					" Or '---' for none.<p>");
				htmlMountTemplateSelect(uMountTemplate);
			}
			else
			{
				printf("You have selected a VETH device based container."
					" make sure you understand the template, network and host security implications."
					"<p>Default mount and start /etc/vz/conf/ action scripts will be configured."
					" These can be changed later for other script templates if the need"
					" arises.");
				
			}
			printf("<p><input title='Continue to step 3 of new container creation'"
					" type=submit class=largeButton"
					" name=gcCommand value='Continue'>\n");
			printf("<p>If you wish to abort this container creation use the [Delete] button above.");
                break;

                case 201:
			printf("<p><u>New container step 3/3</u><br>");
			if(uMountTemplate)
			{
				printf("Complete required mount settings for <i>%s</i> template.<p>",cuTemplateDropDown);
				htmlGenMountInputs(uMountTemplate);
				printf("<input type=hidden name=uMountTemplate value='%u'>\n",uMountTemplate);
				printf("<p><input title='Finish container creation'"
					" type=submit class=largeButton"
					" name=gcCommand value='Confirm Mount Settings'>\n");
			}
			else if(uVeth)
			{
				printf("Default VETH device container mount and start scripts will be created.");
				printf("<p><input title='Finish container creation'"
					" type=submit class=largeButton"
					" name=gcCommand value='Confirm Container Settings'>\n");
			}
			else if(1)
			{
				printf("You have chosen to not create a mount template."
					" If correct confirm below, else go back and select a template.<p>");
				printf("<p><input title='Finish container creation'"
					" type=submit class=largeButton"
					" name=gcCommand value='Confirm Container Settings'>\n");
			}
			printf("<p>If you wish to abort this container creation use the [Delete] button above.");
                break;

                case 2000:
			printf("<p><u>New container step 1/3</u><br>");
			
			printf("Complete required container fields in the record data panel to your right.<p>uVeth='Yes'"
				" containers should only be used when layer 2 connectivity is absolutely required"
				" (The uNode selected must support VETH containers -See tNode Property.)"
				" uVeth='Yes' container tOSTemplate should be reviewed to make sure it's"
				" network settings will not interfere with standard OpenVZ VETH device usage.");
			if(uVeth)
				printf("<p>Make sure you understand the implications of using uVeth='Yes' containers"
				" before proceeding. Alternatively change uVeth to 'No' now.");
			printf("<p>\n");
			printf("<input title='Optional container password set on deployment and saved in"
				" container property table' type=text name=cService1> Optional Password<br>");
			printf("<p><input title='Enter/Mod tContainer record data, then continue"
					" to step 2 of new container creation'"
					" type=submit class=largeButton"
					" name=gcCommand value='Confirm Container Fields'>\n");
                        //printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<p><u>Think twice</u>");
                        printf("<br>Container and it's properties will be removed from unxsVZ database for good.<br>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review changes</u><br>");
			printf("If you change uIPv4 you will need to modify tIP<br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			printf("<u>Table Tips</u><br>");
			printf("Setup or modify a container by selecting a meaningful cLabel,"
				" a correct datacenter and hardware node. You usually should start with"
				" a similar container. Most properties are created by data collection agents"
				", others like <i>Notes</i> are user created and maintained.<p>"
				"Special properties cVEID.mount, cVEID.umount, cVEID.start and cVEID.stop are used via"
				" their tTemplate matching values (see tTemplate for more info)"
				" to create /etc/vz/conf/VEID.x OpenVZ action scripts (on new container creation you will"
				" be able to optionally select a mount/umount template set for this feature, or VETH"
				" device based containers will have default action scripts installed for you.)");
			
			if(uContainer && uNode)
			{
				printf("<p><u>Record Context Info</u>");
				if(uSource)
				{
					printf("<br>%s is a clone of ",cLabel);
					printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%u>"
						"%s</a>",uSource,ForeignKey("tContainer","cLabel",uSource));
				}
				htmlCloneInfo(uContainer);
				htmlContainerNotes(uContainer);
				htmlContainerMount(uContainer);
				htmlGroups(0,uContainer);
			}
			printf("<p><u>Container Search</u><br>");
			printf("<input title='Enter cLabel start or MySQL LIKE pattern (%% or _ allowed)' type=text"
					" name=cSearch value='%s'>",cSearch);

			tContainerNavList(0,cSearch);
			if(uContainer)
			{
				if(uStatus==uACTIVE)
				{
					htmlHealth(uContainer,3);
					printf("<p><input title='Migrate container to another hardware node'"
					" type=submit class=largeButton"
					" name=gcCommand value='Migration Wizard'><br>\n");
					printf("<input title='Template a container."
					" Creates and installs OS and VZ conf templates on all nodes.'"
					" type=submit class=largeButton"
					" name=gcCommand value='Template Wizard'><br>\n");
					if(!strstr(cLabel,"-clone"))
					printf("<input title='Clone a container to this or another hardware node."
					" The clone will be an online container with another IP and hostname."
					" It will be kept updated via rsync on a configurable basis.'"
					" type=submit class=largeButton"
					" name=gcCommand value='Clone Wizard'><br>\n");
					printf("<input title='Change current container IPv4'"
					" type=submit class=largeButton"
					" name=gcCommand value='IP Change Wizard'><br>\n");
					printf("<input title='Change current container name and hostname'"
					" type=submit class=largeButton"
					" name=gcCommand value='Hostname Change Wizard'><br>\n");
					printf("<input title='Creates a job for stopping an active container.'"
						" type=submit class=lwarnButton"
						" name=gcCommand value='Stop %.24s'><br>\n",cLabel);
					if(uSource)
						printf("<input title='Creates jobs for manual failover (switchover.)'"
						" type=submit class=lwarnButton"
						" name=gcCommand value='Failover %.25s'>\n",cLabel);
				}
				else if( uStatus==uAWAITACT || uStatus==uAWAITDEL || uStatus==uAWAITSTOP 
											|| uStatus==uAWAITFAIL)
				{
					printf("<p><input title='Cancel all waiting jobs for this container.'"
					" type=submit class=largeButton"
					" name=gcCommand value='Cancel Job'>\n");
				}
				else if( uStatus==uINITSETUP)
				{
					printf("<p><input title='Creates a job for deploying a new container."
					" Make sure you configure the properties you want beforehand!'"
					" type=submit class=lalertButton"
					" name=gcCommand value='Deploy %.25s'>\n",cLabel);
				}
				else if( uStatus==uSTOPPED)
				{
					printf("<p><input title='Creates a job for starting a stopped container.'"
					" type=submit class=lalertButton"
					" name=gcCommand value='Start %.25s'><br>\n",cLabel);
					if(!strstr(cLabel,"-clone"))
					printf("<input title='Clone a container to this or another hardware node."
					" The clone will be an online container with another IP and hostname."
					" It will be kept updated via rsync on a configurable basis.'"
					" type=submit class=largeButton"
					" name=gcCommand value='Clone Wizard'><br>\n");
					if(uSource)
						printf("<input title='Creates jobs for manual failover (switchover.)'"
						" type=submit class=lwarnButton"
						" name=gcCommand value='Failover %.25s'>\n",cLabel);
				}

				char cVEIDMount[256]={""};
				GetContainerProp(uContainer,"cVEID.mount",cVEIDMount);
				if( cVEIDMount[0] && (uStatus==uSTOPPED || uStatus==uINITSETUP) && !strstr(cLabel,"-clone"))
					printf("<br><input title='If container mount,umount,start or stop files do not"
					" exist they will be created from tProperty data items: cVEID.mount"
					" ,cVEID.umount, cVEID.start and cVEID.stop'"
					" type=submit class=largeButton"
					" name=gcCommand value='Create Action Scripts'>\n");
			}
	}
	CloseFieldSet();

}//void ExttContainerButtons(void)




void ExttContainerAuxTable(void)
{

	if(uHideProps || !uContainer || (guMode!=0 && guMode!=6)) return;

        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"%s Property Panel",cLabel);
	OpenFieldSet(gcQuery,100);
	sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE uKey=%u AND uType=3"
				" AND cName!='Notes' ORDER BY cName",uContainer);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		printf("<table>");
		while((field=mysql_fetch_row(res)))
		{
			printf("<tr>");
			printf("<td width=200 valign=top><a class=darkLink href=unxsVZ.cgi?"
					"gcFunction=tProperty&uProperty=%s&cReturn=tContainer_%u>"
					"%s</a></td><td>%s</td>\n",
						field[0],uContainer,field[1],field[2]);
			printf("</tr>");
		}
		printf("</table>");
	}
	mysql_free_result(res);

	//Special per container buttons
	if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowDel(uOwner,uCreatedBy))
		printf("<p><input title='Creates a job for stopping and destroying an active container."
			" No backups may be available, the container is gone for good!'"
			" type=submit class=lwarnButton"
			" name=gcCommand value='Destroy %.24s'>\n",cLabel);

	CloseFieldSet();

}//void ExttContainerAuxTable(void)


void ExttContainerGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uContainer"))
		{
			sscanf(gentries[i].val,"%u",&uContainer);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.31s",gentries[i].val);
		}
	}
	tContainer("");

}//void ExttContainerGetHook(entry gentries[], int x)


void ExttContainerSelect(void)
{
	if(cSearch[0])
		ExtSelectSearch("tContainer",VAR_LIST_tContainer,"cLabel",cSearch);
	else
		ExtSelect("tContainer",VAR_LIST_tContainer);

}//void ExttContainerSelect(void)


void ExttContainerSelectRow(void)
{
	ExtSelectRow("tContainer",VAR_LIST_tContainer,uContainer);

}//void ExttContainerSelectRow(void)


void ExttContainerListSelect(void)
{
	char cCat[512];

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tContainer",
				VAR_LIST_tContainer);
	else
		sprintf(gcQuery,"SELECT %1$s FROM tContainer," TCLIENT
				" WHERE tContainer.uOwner=" TCLIENT ".uClient"
				" AND (" TCLIENT ".uClient=%2$u OR " TCLIENT ".uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))",
				VAR_LIST_tContainer,guCompany);

	//Changes here must be reflected below in ExttContainerListFilter()
        if(!strcmp(gcFilter,"uContainer"))
        {
                sscanf(gcCommand,"%u",&uContainer);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tContainer.uContainer=%u ORDER BY uContainer",uContainer);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cLabel"))
        {
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tContainer.cLabel LIKE '%s' ORDER BY cLabel,uContainer",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cHostname"))
        {
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tContainer.cHostname LIKE '%s' ORDER BY cHostname,uContainer",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uDatacenter"))
        {
                sscanf(gcCommand,"%u",&uDatacenter);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tContainer.uDatacenter=%u ORDER BY uDatacenter,uContainer",uDatacenter);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uNode"))
        {
                sscanf(gcCommand,"%u",&uNode);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tContainer.uNode=%u ORDER BY uContainer",uNode);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uStatus"))
        {
                sscanf(gcCommand,"%u",&uStatus);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tContainer.uStatus=%u ORDER BY uStatus,uContainer",uStatus);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uOwner"))
        {
                sscanf(gcCommand,"%u",&uOwner);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tContainer.uOwner=%u ORDER BY uOwner,uContainer",uOwner);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uContainer");
        }

}//void ExttContainerListSelect(void)


void ExttContainerListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uContainer"))
                printf("<option>uContainer</option>");
        else
                printf("<option selected>uContainer</option>");
        if(strcmp(gcFilter,"cLabel"))
                printf("<option>cLabel</option>");
        else
                printf("<option selected>cLabel</option>");
        if(strcmp(gcFilter,"cHostname"))
                printf("<option>cHostname</option>");
        else
                printf("<option selected>cHostname</option>");
        if(strcmp(gcFilter,"uDatacenter"))
                printf("<option>uDatacenter</option>");
        else
                printf("<option selected>uDatacenter</option>");
        if(strcmp(gcFilter,"uNode"))
                printf("<option>uNode</option>");
        else
                printf("<option selected>uNode</option>");
        if(strcmp(gcFilter,"uStatus"))
                printf("<option>uStatus</option>");
        else
                printf("<option selected>uStatus</option>");
        if(strcmp(gcFilter,"uOwner"))
                printf("<option>uOwner</option>");
        else
                printf("<option selected>uOwner</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttContainerListFilter(void)


void ExttContainerNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
		printf(LANG_NBB_NEW);

	//11 Initial setup 31 Stopped
	if( (uStatus==uINITSETUP) && uAllowMod(uOwner,uCreatedBy) )
		printf(LANG_NBB_MODIFY);

	if( uStatus==uINITSETUP && uAllowDel(uOwner,uCreatedBy) ) 
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttContainerNavBar(void)


void tContainerNavList(unsigned uNode, char *cSearch)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uNumRows=0;
	unsigned uMySQLNumRows=0;
#define LIMIT " LIMIT 25"
#define uLIMIT 24

	if(!uNode)
	{
		if(cSearch[0])
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.cLabel LIKE '%s%%'"
					" ORDER BY tContainer.cLabel" LIMIT,cSearch);
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.cLabel LIKE '%2$s%%'"
					" ORDER BY tContainer.cLabel" LIMIT,guCompany,cSearch);
		}
		else
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" ORDER BY tContainer.cLabel" LIMIT);
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" ORDER BY tContainer.cLabel" LIMIT,guCompany);
		}
	}
	else
	{

		//debug only
		//printf("<p>(uNode=%u cSearch=%s)",uNode,cSearch);

		if(cSearch[0])
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel"
					" FROM tContainer,tNode,tStatus"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.cLabel LIKE '%s%%'"
					" AND tContainer.uNode=%u ORDER BY tContainer.cLabel" LIMIT,cSearch,uNode);
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND tContainer.uNode=%1$u"
					" AND (" TCLIENT ".uClient=%2$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.cLabel LIKE '%3$s%%'"
					" ORDER BY tContainer.cLabel" LIMIT,uNode,guCompany,cSearch);
		}
		else
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel"
					" FROM tContainer,tNode,tStatus"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
						" AND tContainer.uNode=%u ORDER BY tContainer.cLabel" LIMIT,uNode);
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND tContainer.uNode=%1$u"
					" AND (" TCLIENT ".uClient=%2$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" ORDER BY tContainer.cLabel" LIMIT,uNode,guCompany);
		}
	}

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tContainerNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if((uMySQLNumRows=mysql_num_rows(res)))
	{	
        	printf("<p><u>tContainerNavList(%d)</u><br>\n",uMySQLNumRows);
		if(guPermLevel>11 && uNode==0)
		{
        		printf("You may select containers and then appply a group"
				" function. !No confirmation step is provided!<br>\n");
		}

	        while((field=mysql_fetch_row(res)))
		{
			if(guPermLevel>11 && uNode==0)
			{
				if(cSearch[0])
				printf("<input type=checkbox name=Ct%s><a class=darkLink href=unxsVZ.cgi?gcFunction="
					"tContainer&uContainer=%s&cSearch=%s>%s/%s/%s</a><br>\n",
						field[0],field[0],cURLEncode(cSearch),field[1],field[2],field[3]);
				else
				printf("<input type=checkbox name=Ct%s><a class=darkLink href=unxsVZ.cgi?gcFunction="
					"tContainer&uContainer=%s>%s/%s/%s</a><br>\n",
						field[0],field[0],field[1],field[2],field[3]);
			}
			else
			{
				if(cSearch[0])
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer"
					"&uContainer=%s&cSearch=%s>%s/%s/%s</a><br>\n",
						field[0],cURLEncode(cSearch),field[1],field[2],field[3]);
				else
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer"
					"&uContainer=%s>%s/%s/%s</a><br>\n",
						field[0],field[1],field[2],field[3]);
			}
			if(++uNumRows>uLIMIT)
			{
				printf("Only %u containers shown use 'Container Search' to narrow.<br>\n",
						uLIMIT);
				break;
			}
		}
		if(guPermLevel>11 && uNode==0)
		{
			printf("<input title='Creates job(s) for stopping active container(s).'"
			" type=submit class=lalertButton"
			" name=gcCommand value='Group Stop'>\n");
			printf("<br><input title='Creates job(s) for starting stopped container(s).'"
			" type=submit class=largeButton"
			" name=gcCommand value='Group Start'>\n");
			printf("<br><input title='Creates job(s) for switching over cloned container(s).'"
			" type=submit class=lwarnButton"
			" name=gcCommand value='Group Switchover'>\n");
		}

	}
        mysql_free_result(res);

}//void tContainerNavList()


unsigned CreateNewContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='CreateNewContainerJob(%u)',cJobName='NewContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	return(uCount);

}//unsigned CreateNewContainerJob(...)


unsigned CreateStartContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='CreateStartContainerJob(%u)',cJobName='StartContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","Start");
	return(uCount);

}//unsigned CreateStartContainerJob(...)


unsigned DestroyContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='DestroyContainerJob(%u)',cJobName='DestroyContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","Destroy");
	return(uCount);

}//unsigned DestroyContainerJob(...)


unsigned StopContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='StopContainerJob(%u)',cJobName='StopContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","Stop");
	return(uCount);

}//unsigned StopContainerJob(...)


unsigned CancelContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)
{
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=7 WHERE "
			"uDatacenter=%u AND uNode=%u AND uContainer=%u AND uJobStatus=1"
				,uDatacenter,uNode,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsVZLog(uContainer,"tContainer","CancelJob");
		return(0);
	}
	unxsVZLog(uContainer,"tContainer","CancelJobLate");
	return(1);

}//unsigned CancelContainerJob(...)


void SetContainerStatus(unsigned uContainer,unsigned uStatus)
{
	sprintf(gcQuery,"UPDATE tContainer SET uStatus=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uContainer=%u",
					uStatus,guLoginClient,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void SetContainerStatus(unsigned uContainer,unsigned uStatus)


void SetContainerNode(unsigned uContainer,unsigned uNode)
{
	sprintf(gcQuery,"UPDATE tContainer SET uNode=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uContainer=%u",
					uNode,guLoginClient,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void SetContainerNode(unsigned uContainer,unsigned uNode)


void htmlContainerNotes(unsigned uContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uProperty,cValue FROM tProperty WHERE uKey=%u AND uType=3"
				" AND cName='Notes' ORDER BY uCreatedDate",uContainer);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		printf("<br><a class=darkLink href=unxsVZ.cgi?"
				"gcFunction=tProperty&uProperty=%s&cReturn=tContainer_%u>"
				"Notes</a>:%s\n",field[0],uContainer,field[1]);

	mysql_free_result(res);

}//void htmlContainerNotes(unsigned uContainer)


void htmlContainerMount(unsigned uContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uProperty,cValue FROM tProperty WHERE uKey=%u AND uType=3"
				" AND cName LIKE 'cVEID.mount' ORDER BY uCreatedDate",uContainer);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		printf("<br><a class=darkLink href=unxsVZ.cgi?"
				"gcFunction=tProperty&uProperty=%s&cReturn=tContainer_%u>"
				"Mount template</a>:%s\n",field[0],uContainer,field[1]);

	mysql_free_result(res);

}//void htmlContainerMount(unsigned uContainer)


void htmlGroups(unsigned uNode, unsigned uContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uNode)
	sprintf(gcQuery,"SELECT tGroup.uGroup,tGroup.cLabel FROM tGroupGlue,tGroup WHERE tGroupGlue.uNode=%u"
				" AND tGroupGlue.uGroup=tGroup.uGroup",uNode);
	else
	sprintf(gcQuery,"SELECT tGroup.uGroup,tGroup.cLabel FROM tGroupGlue,tGroup WHERE tGroupGlue.uContainer=%u"
				" AND tGroupGlue.uGroup=tGroup.uGroup",uContainer);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(uNode)
			printf("<br>Node");
		else
			printf("<br>Container");
		printf(" is member of the <a class=darkLink");
		printf(" href=unxsVZ.cgi?gcFunction=tGroup&uGroup=%s>%s</a> group</a>\n",
					field[0],field[1]);
	}
	mysql_free_result(res);

}//void htmlGroups(...)


unsigned MigrateContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer, unsigned uTargetNode)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='MigrateContainerJob(%u-->%u)',cJobName='MigrateContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",cJobData='uTargetNode=%u;'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,uTargetNode,
				uDatacenter,uNode,uContainer,
				uTargetNode,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","Migrate");
	return(uCount);

}//unsigned MigrateContainerJob(...)


void htmlHealth(unsigned uContainer,unsigned uType)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

	long unsigned luTotalFailcnt=0;
	long unsigned luTotalUsage=0,luTotalSoftLimit=0;
	float fRatio;
	char *cTable={"tContainer"};

	if(uType==3)
		printf("<p><u>Container Health</u>");
	else if(uType==2)
		printf("<p><u>Node and Container(s) Health</u>");
	else if(uType==1)
		printf("<p><u>Datacenter Health</u>");
	printf("<br>");

	if(uType==2)
	sprintf(gcQuery,"SELECT COUNT(uProperty) FROM tProperty WHERE cName LIKE '%%.luFailcnt'"
				" AND (uKey IN (SELECT uContainer FROM tContainer WHERE uNode=%u)"
				" OR uKey=%u)"
				,uContainer,uContainer);
	else
	sprintf(gcQuery,"SELECT COUNT(uProperty) FROM tProperty WHERE cName LIKE '%%.luFailcnt'"
				" AND uKey=%u AND uType=%u",uContainer,uType);
	macro_mySQLQueryExitText
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[0],"%lu",&luTotalFailcnt);
	}
	mysql_free_result(mysqlRes);

	if(uType==2)
	sprintf(gcQuery,"SELECT cName,cValue,uProperty,uKey FROM tProperty,tType WHERE"
				" tProperty.uType=tType.uType AND"
				" cName LIKE '%%.luFail%%' AND cValue!='0'"
				" AND (uKey IN (SELECT uContainer FROM tContainer WHERE uNode=%u)"
				" OR uKey=%u)"
				,uContainer,uContainer);
	else
	sprintf(gcQuery,"SELECT cName,cValue,uProperty,uKey FROM tProperty,tType WHERE"
				" tProperty.uType=tType.uType AND"
				" cName LIKE '%%.luFail%%' AND cValue!='0'"
				" AND uKey=%u AND tProperty.uType=%u",uContainer,uType);
	macro_mySQLQueryExitText
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		if(strstr(mysqlField[4],"Node")) cTable="tNode";
		printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tProperty&uProperty=%s&cReturn=%s_%s>%s=%s"
				"</a><br>\n",mysqlField[2],cTable,mysqlField[3],mysqlField[0],mysqlField[1]);
	}
	mysql_free_result(mysqlRes);


	if(uType==2)
	sprintf(gcQuery,"SELECT SUM(CONVERT(cValue,UNSIGNED)) FROM tProperty WHERE"
				" cName='1k-blocks.luUsage'"
				" AND uType=3"
				" AND uKey IN (SELECT uContainer FROM tContainer WHERE uNode=%u)"
					,uContainer);
	else if(uType==3)
	sprintf(gcQuery,"SELECT SUM(CONVERT(cValue,UNSIGNED)) FROM tProperty WHERE"
				" cName='1k-blocks.luUsage'"
				" AND uKey=%u AND uType=%u",uContainer,uType);
	else if(1)
		return;
	macro_mySQLQueryExitText
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		if(mysqlField[0]==NULL) return;

		if(mysqlField[0]==0 || sscanf(mysqlField[0],"%lu",&luTotalUsage)!=1)
			return;
	}
	mysql_free_result(mysqlRes);

	if(uType==2)
	sprintf(gcQuery,"SELECT SUM(CONVERT(cValue,UNSIGNED)) FROM tProperty WHERE"
				" cName='1k-blocks.luSoftlimit'"
				" AND uType=3"
				" AND uKey IN (SELECT uContainer FROM tContainer WHERE uNode=%u)"
					,uContainer);
	else if(uType==3)
	sprintf(gcQuery,"SELECT SUM(CONVERT(cValue,UNSIGNED)) FROM tProperty WHERE"
				" cName='1k-blocks.luSoftlimit'"
				" AND uKey=%u AND uType=%u",uContainer,uType);
	else if(1)
		return;

	macro_mySQLQueryExitText
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		if(mysqlField[0]==NULL) return;

		if(sscanf(mysqlField[0],"%lu",&luTotalSoftLimit)==0)
			return;	
	}
	mysql_free_result(mysqlRes);
	if(luTotalSoftLimit==0) luTotalSoftLimit=1;
	fRatio= ((float) luTotalUsage/ (float) luTotalSoftLimit) * 100.00 ;

	char *cColor;
	cColor=cRatioColor(&fRatio);

	printf("Container usage ratio %2.2f%%:"
		" <font color=%s>%lu/%lu</font><br>\n",
		fRatio,cColor,luTotalUsage,luTotalSoftLimit);

}//void htmlHealth(...)


unsigned TemplateContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='TemplateContainerJob(%u)',cJobName='TemplateContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",cJobData='tConfig.Label=%.31s;'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				cConfigLabel,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","Template");
	return(uCount);

}//unsigned TemplateContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)


unsigned HostnameContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='ChangeHostnameContainer(%u)',cJobName='ChangeHostnameContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","Hostname");
	return(uCount);

}//unsigned HostnameContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)


unsigned IPContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='ChangeIPContainer(%u)',cJobName='ChangeIPContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",cJobData='cIPOld=%.31s;'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,cIPOld,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","ChangeIP");
	return(uCount);

}//unsigned IPContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)


unsigned ActionScriptsJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)
{
	unsigned uCount=0;
	char cTemplateName[256]={""};

	GetContainerProp(uContainer,"cVEID.mount",cTemplateName);
	if(!cTemplateName[0]) return(0);
	GetContainerProp(uContainer,"cVEID.umount",cTemplateName);
	if(!cTemplateName[0]) return(0);

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='ActionScripts(%u)',cJobName='ActionScripts'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","MountFiles");
	return(uCount);

}//unsigned ActionScriptsJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)


unsigned CloneContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,
				unsigned uTargetNode, unsigned uNewVeid, unsigned uPrevStatus)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='CloneContainer(%u)',cJobName='CloneContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",cJobData='"
			"uTargetNode=%u;\n"
			"uNewVeid=%u;\n"
			"uCloneStop=%u;\n"
			"uPrevStatus=%u;\n'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uTargetNode,
				uNewVeid,
				uCloneStop,
				uPrevStatus,
				guCompany,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","CloneContainer");
	return(uCount);

}//unsigned CloneContainerJob()


unsigned CloneNode(unsigned uSourceNode, unsigned uTargetNode, unsigned uWizIPv4)
{

	//Loop for all containers that belong to source node and do not have clones on the uTargetNode.
	//For every loop iteration we must get a new available IP from tIP starting at uWizIPv4
        MYSQL_RES *res;
        MYSQL_RES *res2;
        MYSQL_ROW field;
	unsigned uContainer=0;
	unsigned uDatacenter=0;
	unsigned uNewVeid=0;
	unsigned uCount=0;
	unsigned uStatus=0;

	sprintf(gcQuery,"SELECT cLabel,cHostname,uOSTemplate,uConfig,uNameserver,uSearchdomain,uDatacenter"
			",uContainer,uStatus FROM tContainer WHERE uNode=%u",uSourceNode);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		//Don't clone already cloned container on target node.
		sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uNode=%u AND cLabel LIKE '%.25s-clone%%'",
					uTargetNode,field[0]);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
        	res2=mysql_store_result(&gMysql);
		if(mysql_fetch_row(res2))
		{
			mysql_free_result(res2);
			continue;
		}
		mysql_free_result(res2);

		sscanf(field[6],"%u",&uDatacenter);
		sscanf(field[7],"%u",&uContainer);
		sscanf(field[8],"%u",&uStatus);
		if(!uDatacenter || !uContainer || !field[0][0] || !field[1][0])
		{
			mysql_free_result(res);
			return(4);//no containers added, unexpected error.
		}
		sprintf(gcQuery,"INSERT INTO tContainer SET cLabel='%.25s-clone0',"
					"cHostname='%.93s.clone',"
					"uIPv4=%u,"
					"uOSTemplate=%s,"
					"uConfig=%s,"
					"uNameserver=%s,"
					"uSearchdomain=%s,"
					"uDatacenter=%u,"
					"uNode=%u,"
					"uStatus=81,"//uAWAITCLONE
					"uOwner=%u,"
					"uCreatedBy=%u,"
					"uSource=%u,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW())",
							field[0],
							field[1],
							uWizIPv4,
							field[2],
							field[3],
							field[4],
							field[5],
							uDatacenter,
							uTargetNode,
							guCompany,
							uContainer,
							guLoginClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		uNewVeid=mysql_insert_id(&gMysql);

		if(CloneContainerJob(uDatacenter,uSourceNode,uContainer,uTargetNode,uNewVeid,uStatus))
		{
			sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
					" WHERE uIP=%u AND uAvailable=1",uWizIPv4);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
			if(mysql_affected_rows(&gMysql)!=1)
			{
				sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",uNewVeid);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				mysql_free_result(res);
				//Operator did not select a large enough contiguous IP block starting 
				//at uWizIPv4. This is a temp hack. But allows operator to run 
				//operation again.
				if(uCount)
					return(1);//some containers added, ips not enough.
				else
					return(3);//no containers added, ips not enough.
			}
			uWizIPv4++;//Now we can increment safely
			
			CopyContainerProps(uContainer,uNewVeid);
			sprintf(gcQuery,"DELETE FROM tProperty WHERE"
					" cName='Name' AND uKey=%u AND uType=3",uNewVeid);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
					",cName='Name',cValue='%.25s-clone0'",
							uNewVeid,guCompany,guLoginClient,field[0]);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			//default no sync period set
			sprintf(gcQuery,"DELETE FROM tProperty WHERE"
					" cName='cuSyncPeriod' AND uKey=%u AND uType=3",uNewVeid);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
					",cName='cuSyncPeriod',cValue='0'",
						uNewVeid,guCompany,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			//sscanf(field[7],"%u",&uContainer);
			SetContainerStatus(uContainer,81);//Awaiting Clone
			uCount++;
		}
		else
		{
			sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",uNewVeid);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
		}

	}
	mysql_free_result(res);

	if(uNewVeid)
		return(0);//all possible containers added as expected.
	else
		return(2);//no containers added

}//unsigned CloneNode()


char *cRatioColor(float *fRatio)
{
	char *cColor={"black"};

	if(*fRatio<40.00)
		return(cColor);
	else if(*fRatio<70.00)
		cColor="#DAA520";
	else if(*fRatio<80.00)
		cColor="fuchsia";
	else if(*fRatio>=80.00)
		cColor="red";

	return(cColor);

}//char *cRatioColor(float *fRatio)


void htmlMountTemplateSelect(unsigned uSelector)
{
        register int i,n;
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        sprintf(gcQuery,"SELECT uTemplate,cLabel FROM tTemplate WHERE cLabel LIKE '%%.mount' ORDER BY cLabel");
	MYSQL_RUN_STORE_TEXT_RET_VOID(mysqlRes);
	i=mysql_num_rows(mysqlRes);
        if(i>0)
        {
                printf("<select name=MountTemplateSelect>\n");
                //Default no selection
                printf("<option title='No selection'>---</option>\n");
                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                             printf("<option>%s</option>\n",mysqlField[1]);
                        else
                             printf("<option selected>%s</option>\n",mysqlField[1]);
                }
        }
        else
        {
		printf("<select name=MountTemplateSelect>"
				"<option title='No selection'>---</option></select>\n");
        }
        printf("</select>\n");

}//void htmlMountTemplateSelect(unsigned uSelector)


void AddMountProps(unsigned uContainer)
{
	if(cService1[0])
	{
		sprintf(gcQuery,"DELETE FROM tProperty WHERE uKey=%u AND uType=3"
				" AND cName='cService1'",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
			",cName='cService1',cValue='%s'",
				uContainer,guCompany,guLoginClient,cService1);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}

	if(cService2[0])
	{
		sprintf(gcQuery,"DELETE FROM tProperty WHERE uKey=%u AND uType=3"
				" AND cName='cService2'",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
			",cName='cService2',cValue='%s'",
				uContainer,guCompany,guLoginClient,cService2);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}

	if(cService3[0])
	{
		sprintf(gcQuery,"DELETE FROM tProperty WHERE uKey=%u AND uType=3"
				" AND cName='cService3'",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
			",cName='cService3',cValue='%s'",
				uContainer,guCompany,guLoginClient,cService3);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}

	if(cService4[0])
	{
		sprintf(gcQuery,"DELETE FROM tProperty WHERE uKey=%u AND uType=3"
				" AND cName='cService4'",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
			",cName='cService4',cValue='%s'",
				uContainer,guCompany,guLoginClient,cService4);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}

	if(cNetmask[0])
	{
		sprintf(gcQuery,"DELETE FROM tProperty WHERE uKey=%u AND uType=3"
				" AND cName='cNetmask'",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
			",cName='cNetmask',cValue='%s'",
				uContainer,guCompany,guLoginClient,cNetmask);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}

	if(cPrivateIPs[0])
	{
		sprintf(gcQuery,"DELETE FROM tProperty WHERE uKey=%u AND uType=3"
				" AND cName='cPrivateIPs'",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
			",cName='cPrivateIPs',cValue='%s'",
				uContainer,guCompany,guLoginClient,cPrivateIPs);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}

	if(cuWizIPv4PullDown[0])
	{
		sprintf(gcQuery,"DELETE FROM tProperty WHERE uKey=%u AND uType=3"
				" AND cName='cNodeIP'",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
			",cName='cNodeIP',cValue='%s'",
				uContainer,guCompany,guLoginClient,cuWizIPv4PullDown);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		//This needs the corresponding release code on delete/mod and delete container
		//ops
		sprintf(gcQuery,"UPDATE tIP SET uAvailable=0 WHERE cLabel='%s'",cuWizIPv4PullDown);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}

	if(cuTemplateDropDown[0])
	{
		char *cp;

		sprintf(gcQuery,"DELETE FROM tProperty WHERE uKey=%u AND uType=3"
				" AND (cName='cVEID.mount' OR cName='cVEID.umount')",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
			",cName='cVEID.mount',cValue='%s'",
				uContainer,guCompany,guLoginClient,cuTemplateDropDown);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		if((cp=strchr(cuTemplateDropDown,'.')))
			sprintf(cp,".umount");
		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
			",cName='cVEID.umount',cValue='%s'",
				uContainer,guCompany,guLoginClient,cuTemplateDropDown);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}


}//void AddMountProps(unsigned uContainer)


void CopyContainerProps(unsigned uSource, unsigned uTarget)
{
	if(!uSource || !uTarget) return;

        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cName,cValue FROM tProperty WHERE uKey=%u AND uType=3",uSource);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"INSERT INTO tProperty SET cName='%s',cValue='%s',uKey=%u,uType=3,uOwner=%u,"
				"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					field[0],
					field[1],
					uTarget,guCompany,guLoginClient);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

}//void CopyContainerProps(unsigned uSource, unsigned uTarget)


//Both these failover jobs must in some sense check each other's status it seems at first blush.
unsigned FailoverToJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)
{
	unsigned uCount=0;

	if(!uDatacenter || !uNode || !uContainer) return(0);

	if(!uOwner) uOwner=1;
	if(!guLoginClient) guLoginClient=1;

	//This job should try to stop the uSource container if possible. Or at least
	//make sure it's IP is down. There are many failover scenarios it seems at first glance
	//we will try to handle all of them.
	//Remote datacenter failover seems to involve DNS changes. Since we can't use the VIP
	//method that should be available in a correctly configured unxsVZ datacenter.
	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='FailoverToJob(%u)',cJobName='FailoverTo'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())"
			",uJobStatus=1"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	return(uCount);

}//unsigned FailoverToJob()


unsigned FailoverFromJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,
				unsigned uIPv4,char *cLabel,char *cHostname,unsigned uSource,
				unsigned uStatus,unsigned uFailToJob)
{
	unsigned uCount=0;

	if(!uDatacenter || !uNode || !uContainer) return(0);

	if(!uOwner) uOwner=1;
	if(!guLoginClient) guLoginClient=1;

	//If the source node is down this job will be stuck
	//When node is fixed and boots we must avoid the source continater coming up with same IP
	//as the new production container (the FailoverToJob container.)
	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='FailoverFromJob(%u)',cJobName='FailoverFrom'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())"
			",uJobStatus=1"
			",cJobData='uIPv4=%u;\ncLabel=%s;\ncHostname=%s;\nuSource=%u;\nuStatus=%u;\n"
			"uFailToJob=%u;'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uIPv4,cLabel,cHostname,uSource,uStatus,uFailToJob,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	return(uCount);

}//unsigned FailoverFromJob()


void htmlCloneInfo(unsigned uContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel,uContainer FROM tContainer WHERE uSource=%u",uContainer);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
		printf("<br>Clone(s) of current container:\n");

	if((field=mysql_fetch_row(res)))
		printf("<br> &nbsp; <a class=darkLink href=unxsVZ.cgi?"
				"gcFunction=tContainer&uContainer=%s>"
				"%s</a>\n",field[1],field[0]);

	mysql_free_result(res);

}//void htmlCloneInfo(unsigned uContainer)


void GetContainerProps(unsigned uContainer,struct structContainer *sContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0)
		return;

	sprintf(gcQuery,"SELECT cLabel,cHostname,uVeth,uSource,uIPv4,uOSTemplate,uConfig,uNameserver,"
			"uSearchdomain,uDatacenter,uNode,uStatus,uOwner,uCreatedBy,uCreatedDate,uModBy,"
			"uModDate FROM tContainer WHERE uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(sContainer->cLabel,"%.31s",field[0]);
		sprintf(sContainer->cHostname,"%.63s",field[1]);
		sscanf(field[2],"%u",&sContainer->uVeth);
		sscanf(field[3],"%u",&sContainer->uSource);
		sscanf(field[4],"%u",&sContainer->uIPv4);
		sscanf(field[5],"%u",&sContainer->uOSTemplate);
		sscanf(field[6],"%u",&sContainer->uConfig);
		sscanf(field[7],"%u",&sContainer->uNameserver);
		sscanf(field[8],"%u",&sContainer->uSearchdomain);
		sscanf(field[9],"%u",&sContainer->uDatacenter);
		sscanf(field[10],"%u",&sContainer->uNode);
		sscanf(field[11],"%u",&sContainer->uStatus);
		sscanf(field[12],"%u",&sContainer->uOwner);
		sscanf(field[13],"%u",&sContainer->uCreatedBy);
		sscanf(field[14],"%lu",&sContainer->uCreatedDate);
		sscanf(field[15],"%u",&sContainer->uModBy);
		sscanf(field[16],"%lu",&sContainer->uModDate);
	}
	mysql_free_result(res);

}//void GetContainerProps()


void InitContainerProps(struct structContainer *sContainer)
{
	sContainer->cLabel[0]=0;
	sContainer->cHostname[0]=0;
	sContainer->uVeth=0;
	sContainer->uSource=0;
	sContainer->uIPv4=0;
	sContainer->uOSTemplate=0;
	sContainer->uConfig=0;
	sContainer->uNameserver=0;
	sContainer->uSearchdomain=0;
	sContainer->uDatacenter=0;
	sContainer->uNode=0;
	sContainer->uStatus=0;
	sContainer->uOwner=0;
	sContainer->uCreatedBy=0;
	sContainer->uCreatedDate=0;
	sContainer->uModBy=0;
	sContainer->uModDate=0;

}//void InitContainerProps(struct structContainer *sContainer)
