/*
FILE
	$Id$
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR/LEGAL
	(C) 2001-2010 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
NOTES
	mySQL 5.0+ now required
USEFUL SQL
	//Clean up SQL (rules/constraints) that should never be needed once code is stable.
	DELETE FROM tProperty WHERE uType=3 AND uKey NOT IN (SELECT uContainer FROM tContainer);
	UPDATE tIP SET uAvailable=1 WHERE uAvailable=0 AND uIP NOT IN (SELECT uIPv4 FROM tContainer); 
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
void GetDatacenterProp(const unsigned uDatacenter,const char *cName,char *cValue);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);
void InitContainerProps(struct structContainer *sContainer);
unsigned uGetGroup(unsigned uNode, unsigned uContainer);
unsigned uGetSearchGroup(const char *gcUser,unsigned uGroupType);
unsigned unxsBindARecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
		unsigned uOwner,unsigned uCreatedBy);
static unsigned uHideProps=0;
static unsigned uTargetNode=0;
static char cuTargetNodePullDown[256]={""};
static unsigned guCloneTargetNode=0;
static char cuCloneTargetNodePullDown[256]={""};
static unsigned uTargetDatacenter=0;
static char cuTargetDatacenterPullDown[256]={""};
static unsigned uMountTemplate=0;
static char cuTemplateDropDown[256]={""};
static char cConfigLabel[32]={""};
static char cWizHostname[100]={""};
static char cWizLabel[32]={""};
static char cService1[32]={""};//Also used for optional container password
static char cService2[32]={""};
static char cService3[32]={""};
static char cService4[32]={""};
static char cPrivateIPs[64]={""};
static char gcIPv4[16]={""};
static char gcReturnMsg[32]={""};
static char cNetmask[64]={""};
static unsigned uWizIPv4=0;
static char cuWizIPv4PullDown[32]={""};
static unsigned uWizContainer=0;
static char cuWizContainerPullDown[32]={""};
static unsigned uAllPortsOpen=0;
static unsigned uCloneStop=WARM_CLONE;
static unsigned uSyncPeriod=0;
static unsigned guSameNode=0;//Same node as right loaded container uNode.
static unsigned guNoClones=0;
static unsigned guOpOnClones=0;
static unsigned guInitOnly=0;
static char cSearch[32]={""};
//static unsigned uGroupJobs=0;
//uGroup: Group type association
static unsigned uGroup=0;
static char cuGroupPullDown[256]={""};
static unsigned uChangeGroup=0;
static char ctContainerGroupPullDown[256]={""};
//uForClient: Create for, on 'New;
static unsigned uForClient=0;
static unsigned uCreateDNSJob=0;
static unsigned uUsePublicIP=0;
static char gcUseThisIP[256]={""};
static char cForClientPullDown[256]={""};

//ModuleFunctionProtos()
void tContainerNavList(unsigned uNode, char *cSearch);//uNode is really a node mode
unsigned CreateNewContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uOwner);
unsigned CreateStartContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uOwner);
unsigned DestroyContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uOwner);
unsigned StopContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uOwner);
unsigned CancelContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uCancelMode);
void SetContainerStatus(unsigned uContainer,unsigned uStatus);
void SetContainerNode(unsigned uContainer,unsigned uNode);
void SetContainerDatacenter(unsigned uContainer,unsigned uDatacenter);
void htmlContainerNotes(unsigned uContainer);
void htmlContainerMount(unsigned uContainer);
unsigned MigrateContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer, unsigned uTargetNode,
			unsigned uOwner, unsigned uLoginClient, unsigned uIPv4,unsigned uPrevStatus);
unsigned CloneContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,
				unsigned uTargetNode, unsigned uNewVeid, unsigned uPrevStatus,
				unsigned uOwner,unsigned uCreatedBy,unsigned uCloneStop);
void htmlHealth(unsigned uContainer,unsigned uType);
void htmlGroups(unsigned uNode, unsigned uContainer);
char *cHtmlGroups(char const *cuContainer);
unsigned TemplateContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uStatus,
		unsigned uOwner,char *cConfigLabel);
unsigned HostnameContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,char *cPrevHostname,unsigned uOwner,unsigned uLoginClient);
unsigned IPContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,
			unsigned uOwner,unsigned uLoginClient,char const *cIPOld);
unsigned IPSameNodeContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer1,unsigned uContainer2,
			unsigned uOwner,unsigned uLoginClient,char const *cIPOld1,char const *cIPOld2);
unsigned ActionScriptsJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
char *cRatioColor(float *fRatio);
void htmlGenMountInputs(unsigned const uMountTemplate);
unsigned uCheckMountSettings(unsigned uMountTemplate);
void htmlMountTemplateSelect(unsigned uSelector);
void AddMountProps(unsigned uContainer);
void CopyContainerProps(unsigned uSource, unsigned uTarget);
//These two jobs are always done in pairs. Even though the second may run much later
//for example after hardware failure has been fixed.
unsigned FailoverToJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,unsigned uOwner,unsigned uLoginClient,unsigned uDebug);
unsigned FailoverFromJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,
				unsigned uIPv4,char *cLabel,char *cHostname,unsigned uSource,
				unsigned uStatus,unsigned uFailToJob,unsigned uOwner,unsigned uLoginClient,unsigned uDebug);
void htmlCloneInfo(unsigned uContainer);
void CreateDNSJob(unsigned uIPv4,unsigned uOwner,char const *cOptionalIPv4,
			char const *cHostname,unsigned uDatacenter,unsigned uCreatedBy);
unsigned CreateExecuteCommandsJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uOwner,char *cCommands);
unsigned uGetPrimaryContainerGroup(unsigned uContainer);
unsigned uGetPrimaryContainerGroupGlueRecord(unsigned uContainer);
unsigned UpdatePrimaryContainerGroup(unsigned uContainer,unsigned uNewGroup);

//extern
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);//jobqueue.c
void DelProperties(unsigned uNode,unsigned uType);//tnodefunc.h

#include <openisp/ucidr.h>
#include <ctype.h>

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
		//Some of these must go before group Ct operations
		if(!strcmp(entries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.31s",TextAreaSave(entries[i].val));
		}
		else if(!strcmp(entries[i].name,"uGroup"))
		{
			sscanf(entries[i].val,"%u",&uGroup);
		}
		else if(!strcmp(entries[i].name,"cuGroupPullDown"))
		{
			sprintf(cuGroupPullDown,"%.255s",entries[i].val);
			uGroup=ReadPullDown("tGroup","cLabel",cuGroupPullDown);
		}
		else if(!strcmp(entries[i].name,"ctContainerGroupPullDown"))
		{
			sprintf(ctContainerGroupPullDown,"%.255s",entries[i].val);
			uChangeGroup=ReadPullDown("tGroup","uGroupType=1 AND cLabel",ctContainerGroupPullDown);
			uGroup=uChangeGroup;//For legacy support TODO
		}
		else if(!strcmp(entries[i].name,"guOpOnClonesNoCA"))
		{
			guOpOnClones=1;
		}
		else if(!strncmp(entries[i].name,"Ct",2))
		{
			//insider xss protection
			if(guPermLevel<10)
				continue;

			unsigned uCtContainer=0;
			sscanf(entries[i].name,"Ct%u",&uCtContainer);
			if(uCtContainer)
			{
				if(!strcmp(gcFunction,"tContainerTools"))
				{
					;
				}
			}
		}
		else if(!strcmp(entries[i].name,"guSameNode"))
		{
			guSameNode=1;
		}
		else if(!strcmp(entries[i].name,"guNoClones"))
		{
			guNoClones=1;
		}
		else if(!strcmp(entries[i].name,"guInitOnly"))
		{
			guInitOnly=1;
		}
		else if(!strcmp(entries[i].name,"cuTargetNodePullDown"))
		{
			sprintf(cuTargetNodePullDown,"%.255s",entries[i].val);
			uTargetNode=ReadPullDown("tNode","cLabel",cuTargetNodePullDown);
		}
		else if(!strcmp(entries[i].name,"cuCloneTargetNodePullDown"))
		{
			sprintf(cuCloneTargetNodePullDown,"%.255s",entries[i].val);
			guCloneTargetNode=ReadPullDown("tNode","cLabel",cuCloneTargetNodePullDown);
		}
		else if(!strcmp(entries[i].name,"cuTargetDatacenterPullDown"))
		{
			sprintf(cuTargetDatacenterPullDown,"%.255s",entries[i].val);
			uTargetDatacenter=ReadPullDown("tDatacenter","cLabel",cuTargetDatacenterPullDown);
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
		else if(!strcmp(entries[i].name,"uCreateDNSJob"))
		{
			uCreateDNSJob=1;
		}
		else if(!strcmp(entries[i].name,"uUsePublicIP"))
		{
			uUsePublicIP=1;
		}
		else if(!strcmp(entries[i].name,"gcUseThisIP"))
		{
			sprintf(gcUseThisIP,"%.31s",IPNumber(entries[i].val));
		}
		else if(!strcmp(entries[i].name,"uCloneStop"))
		{
			sscanf(entries[i].val,"%u",&uCloneStop);
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
		else if(!strcmp(entries[i].name,"gcIPv4"))
		{
			sprintf(gcIPv4,"%.15s",IPNumber(entries[i].val));
		}
		else if(!strcmp(entries[i].name,"cuWizIPv4PullDown"))
		{
			sprintf(cuWizIPv4PullDown,"%.31s",entries[i].val);
			uWizIPv4=ReadPullDown("tIP","cLabel",cuWizIPv4PullDown);
		}
		else if(!strcmp(entries[i].name,"cForClientPullDown"))
		{
			sprintf(cForClientPullDown,"%.255s",entries[i].val);
			uForClient=ReadPullDown("tClient","cLabel",cForClientPullDown);
		}
		else if(!strcmp(entries[i].name,"cuWizContainerPullDown"))
		{
			sprintf(cuWizContainerPullDown,"%.31s",entries[i].val);
			uWizContainer=ReadPullDown("tContainer","cLabel",cuWizContainerPullDown);
		}
		else if(!strcmp(entries[i].name,"cHostnameSearch"))
		{
			sprintf(cHostnameSearch,"%.99s",entries[i].val);
		}
		else if(!strcmp(entries[i].name,"cIPv4Search"))
		{
			sprintf(cIPv4Search,"%.15s",entries[i].val);
		}
	}

}//void ExtProcesstContainerVars(pentry entries[], int x)


void ExttContainerCommands(pentry entries[], int x)
{
	MYSQL_RES *res;

	if(!strcmp(gcFunction,"tContainerTools"))
	{
		unsigned uIPv4Datacenter=0;
		unsigned uNodeDatacenter=0;
		time_t uActualModDate= -1;
		char cContainerType[256]={""};
		char cNCMDatacenter[256]={""};
		char cNCMNode[256]={""};
		char cNCCloneRange[256]={""};

		uHideProps=1;

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstContainerVars(entries,x);
                        	guMode=9001;
	                        tContainer("New container step 1");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,"Reload Search Set"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstContainerVars(entries,x);
                        	guMode=12002;
	                        tContainer("Search set reloaded");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
		else if(!strcmp(gcCommand,"Remove from Search Set"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstContainerVars(entries,x);
                        	guMode=12002;
				char cQuerySection[256];
				unsigned uLink=0;
				unsigned uNumber=0;

				if((uGroup=uGetSearchGroup(gcUser,2))==0)
		                        tContainer("No search set exists. Please create one first.");

				//We extend to this other optional list and ignore the other filter items
				if(cCommands[0])
				{
					register int i,j;
					char cHostname[64];

					for(i=0,j=0;cCommands[i];i++)
					{
						if( isspace(cCommands[i]) || cCommands[i]==';' || cCommands[i]=='#' || j>62)
						{
							cHostname[j]=0;
							j=0;

							sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroup=%u AND uContainer IN"
									" (SELECT uContainer FROM tContainer WHERE cHostname='%s')",
										uGroup,cHostname);
							mysql_query(&gMysql,gcQuery);
							if(mysql_errno(&gMysql))
								htmlPlainTextError(mysql_error(&gMysql));
							uNumber+=mysql_affected_rows(&gMysql);

							for(;cCommands[i];i++)
							{
								//jump to next line
								if( cCommands[i]=='\n' || cCommands[i]=='\r')
									break;
							}
						}
						else
						{
							cHostname[j++]=cCommands[i];
						}
					}
					//Last line case
					sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroup=%u AND uContainer IN"
								" (SELECT uContainer FROM tContainer WHERE cHostname='%s')",
									uGroup,cHostname);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					uNumber+=mysql_affected_rows(&gMysql);

	                        	sprintf(gcQuery,"%u container records removed via cHostname list",uNumber);
	                        	tContainer(gcQuery);
				}//if(cCommands[0])

				if(cHostnameSearch[0]==0 && cIPv4Search[0]==0 && uDatacenter==0 && uNode==0 && uSearchStatus==0
						&& uForClient==0 && uOSTemplate==0)
	                        	tContainer("You must specify at least one search parameter");


				//Initial query section
				sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroup=%u AND uContainer IN"
						" (SELECT uContainer FROM tContainer WHERE",uGroup);

				//Build AND query section
				if(cHostnameSearch[0])
				{
					sprintf(cQuerySection," cHostname LIKE '%s%%'",cHostnameSearch);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}
				else
				{
					uLink=0;
				}

				if(cIPv4Search[0])
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uIPv4 IN (SELECT uIP FROM tIP WHERE cLabel LIKE '%s%%')",cIPv4Search);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uDatacenter)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uDatacenter=%u",uDatacenter);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uNode)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uNode=%u",uNode);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uSearchStatus)
				{
					if(uLink)
						strcat(gcQuery," AND");
					if(uSearchStatusNot)
						sprintf(cQuerySection," uStatus!=%u",uSearchStatus);
					else
						sprintf(cQuerySection," uStatus=%u",uSearchStatus);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				//Clone YesNo tristate
				if(uSearchSource)
				{
					if(uLink)
						strcat(gcQuery," AND");
					if(uSearchSource==2)//Tri state Yes
						sprintf(cQuerySection," uSource>0");
					else
						sprintf(cQuerySection," uSource=0");
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uOSTemplate)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uOSTemplate=%u",uOSTemplate);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uForClient)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uOwner=%u",uForClient);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				strcat(gcQuery,")");
				//debug only
	                        //tContainer(gcQuery);

				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				if((uNumber=mysql_affected_rows(&gMysql))>0)
				{
	                        	sprintf(gcQuery,"%u container records were removed from your search set",uNumber);
	                        	tContainer(gcQuery);
				}
				else
				{
	                        	tContainer("No records were removed from your search set");
				}
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,"Add to Search Set") || !strcmp(gcCommand,"Create Search Set"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstContainerVars(entries,x);
                        	guMode=12002;
				char cQuerySection[256];
				unsigned uLink=0;
				unsigned uNumber=0;

				if(cHostnameSearch[0]==0 && cIPv4Search[0]==0 && uDatacenter==0 && uNode==0 && uSearchStatus==0
						&& uForClient==0 && uOSTemplate==0 && cCommands[0]==0)
	                        	tContainer("You must specify at least one search parameter");

				if((uGroup=uGetSearchGroup(gcUser,2))==0)
				{
					sprintf(gcQuery,"INSERT INTO tGroup SET cLabel='%s',uGroupType=2"//2 is search group
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							gcUser,guCompany,guLoginClient);//2=search set type TODO
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					if((uGroup=mysql_insert_id(&gMysql))==0)
		                        	tContainer("An error ocurred when attempting to create your search set");
				}
				else
				{
					if(!strcmp(gcCommand,"Create Search Set"))
					{
						sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroup=%u",uGroup);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					}
                		}

				//We extend to this other optional list and ignore the other filter items
				if(cCommands[0])
				{
					register int i,j;
					char cHostname[64];

					for(i=0,j=0;cCommands[i];i++)
					{
						if( isspace(cCommands[i]) || cCommands[i]==';' || cCommands[i]=='#' || j>62)
						{
							cHostname[j]=0;
							j=0;

							sprintf(gcQuery,"INSERT INTO tGroupGlue (uGroup,uContainer)"
									" SELECT %u,uContainer FROM tContainer WHERE cHostname='%s'",
												uGroup,cHostname);
							mysql_query(&gMysql,gcQuery);
							if(mysql_errno(&gMysql))
								htmlPlainTextError(mysql_error(&gMysql));
							uNumber+=mysql_affected_rows(&gMysql);

							for(;cCommands[i];i++)
							{
								//jump to next line
								if( cCommands[i]=='\n' || cCommands[i]=='\r')
									break;
							}
						}
						else
						{
							cHostname[j++]=cCommands[i];
						}
					}
					//Last line case
					sprintf(gcQuery,"INSERT INTO tGroupGlue (uGroup,uContainer)"
							" SELECT %u,uContainer FROM tContainer WHERE cHostname='%s'",
												uGroup,cHostname);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					uNumber+=mysql_affected_rows(&gMysql);

	                        	sprintf(gcQuery,"%u container records added via cHostname list",uNumber);
	                        	tContainer(gcQuery);
				}//if(cCommands[0])

				//Initial query section
				sprintf(gcQuery,"INSERT INTO tGroupGlue (uGroupGlue,uGroup,uNode,uContainer)"
						" SELECT 0,%u,0,uContainer FROM tContainer WHERE",uGroup);

				//Build AND query section

				if(cHostnameSearch[0])
				{
					sprintf(cQuerySection," cHostname LIKE '%s%%'",cHostnameSearch);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}
				else
				{
					uLink=0;
				}

				if(cIPv4Search[0])
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uIPv4 IN (SELECT uIP FROM tIP WHERE cLabel LIKE '%s%%')",cIPv4Search);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uDatacenter)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uDatacenter=%u",uDatacenter);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uNode)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uNode=%u",uNode);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uSearchStatus)
				{
					if(uLink)
						strcat(gcQuery," AND");
					if(uSearchStatusNot)
						sprintf(cQuerySection," uStatus!=%u",uSearchStatus);
					else
						sprintf(cQuerySection," uStatus=%u",uSearchStatus);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				//Clone YesNo tristate
				if(uSearchSource)
				{
					if(uLink)
						strcat(gcQuery," AND");
					if(uSearchSource==2)//Tri state Yes
						sprintf(cQuerySection," uSource>0");
					else
						sprintf(cQuerySection," uSource=0");
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uOSTemplate)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uOSTemplate=%u",uOSTemplate);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uForClient)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uOwner=%u",uForClient);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				//debug only
	                        //tContainer(gcQuery);

				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				if((uNumber=mysql_affected_rows(&gMysql))>0)
				{
	                        	sprintf(gcQuery,"%u container records were added to your search set",uNumber);
	                        	tContainer(gcQuery);
				}
				else
				{
	                        	tContainer("No records were added to your search set. Filter returned 0 records");
				}
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,"Search Set Operations"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstContainerVars(entries,x);
                        	guMode=12001;
	                        tContainer("Search Set Operations");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,"Select Datacenter/Org"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstContainerVars(entries,x);
                        	guMode=9001;
				if(!uDatacenter)
					tContainer("<blink>Error:</blink> Must select a datacenter.");
				GetDatacenterProp(uDatacenter,"NewContainerMode",cNCMDatacenter);
				if(cNCMDatacenter[0] && !strstr(cNCMDatacenter,"Active"))
					tContainer("<blink>Error:</blink> Selected datacenter is full or not active. Select another.");
				if(!uForClient)
					tContainer("<blink>Error:</blink> Must select an organization"
							" (company, NGO or similar.)");

                        	guMode=9002;
	                        tContainer("New container step 2");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,"Select Node"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstContainerVars(entries,x);
                        	guMode=9002;
				if(!uDatacenter)
					tContainer("<blink>Error:</blink> Must select a datacenter.");
				if(!uForClient)
					tContainer("<blink>Error:</blink> Must select an organization"
							" (company, NGO or similar.)");
				if(!uNode)
					tContainer("<blink>Error:</blink> Must select a node.");

				GetDatacenterProp(uDatacenter,"NewContainerMode",cNCMDatacenter);
				if(cNCMDatacenter[0] && !strstr(cNCMDatacenter,"Active"))
					tContainer("<blink>Error:</blink> Selected datacenter is full or not active. Select another.");

				GetNodeProp(uNode,"NewContainerMode",cNCMNode);
				if(cNCMNode[0] && !strstr(cNCMNode,"Active"))
					tContainer("<blink>Error:</blink> Selected node is not configured for active containers."
							"Select another.");

                        	guMode=9003;
	                        tContainer("New container step 3");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,"Cancel"))
                {
			guMode=0;
		}
		else if(!strcmp(gcCommand,"Single Container Creation") || !strcmp(gcCommand,"Appliance Creation"))
                {
			if(guPermLevel>=9)
			{
				MYSQL_ROW field;
				unsigned uNewVeid=0;
				register int i;
				char cIPv4ClassC[32]={""};
				char cIPv4ClassCClone[32]={""};
				unsigned uAvailableIPs=0;
				unsigned uHostnameLen=0;
				unsigned uLabelLen=0;
				unsigned uCreateAppliance=0;

                        	ProcesstContainerVars(entries,x);

				if(!strcmp(gcCommand,"Appliance Creation"))
					uCreateAppliance=1;

                        	guMode=9003;
				//Check entries here
				if(uDatacenter==0)
					tContainer("<blink>Error:</blink> Unexpected uDatacenter==0!");
				if(uNode==0)
					tContainer("<blink>Error:</blink> Unexpected uNode==0!");

				GetDatacenterProp(uDatacenter,"NewContainerMode",cNCMDatacenter);
				if(cNCMDatacenter[0] && !strstr(cNCMDatacenter,"Active"))
					tContainer("<blink>Error:</blink> Selected datacenter is full or not active. Select another.");

				GetNodeProp(uNode,"NewContainerMode",cNCMNode);
				if(cNCMNode[0] && !strstr(cNCMNode,"Active"))
					tContainer("<blink>Error:</blink> Selected node is not configured for active containers."
							"Select another.");

				if((uLabelLen=strlen(cLabel))<2)
					tContainer("<blink>Error:</blink> cLabel is too short");
				if(strchr(cLabel,'.'))
					tContainer("<blink>Error:</blink> cLabel has at least one '.'");
				if(strstr(cLabel,"-clone"))
					tContainer("<blink>Error:</blink> cLabel can't have '-clone'");
				if(uCreateAppliance)
				{
					if(!strstr(cLabel+(uLabelLen-strlen("-app")-1),"-app"))
						tContainer("<blink>Error:</blink> Appliance cLabel must end with '-app'.");
					if(strlen(gcIPv4)<7 || strlen(gcIPv4)>15)
						tContainer("<blink>Error:</blink> Appliance requires valid gcIPv4.");

					unsigned a=0,b=0,c=0,d=0;
					sscanf(gcIPv4,"%u.%u.%u.%u",&a,&b,&c,&d);
					if(a==0 || d==0)
						tContainer("<blink>Error:</blink> Appliance requires valid gcIPv4.");
				}

				if((uHostnameLen=strlen(cHostname))<5)
					tContainer("<blink>Error:</blink> cHostname is too short");
				if(cHostname[uHostnameLen-1]=='.')
					tContainer("<blink>Error:</blink> cHostname can't end with a '.'");
				if(strstr(cHostname,"-clone"))
					tContainer("<blink>Error:</blink> cHostname can't have '-clone'");
				//New rule: cLabel must be first part (first stop) of cHostname.
				if(strncmp(cLabel,cHostname,uLabelLen))
					tContainer("<blink>Error:</blink> cLabel must be first part of cHostname.");
				if(uIPv4==0)
					tContainer("<blink>Error:</blink> You must select a uIPv4");

				//Let's not allow same cLabel containers in our system for now.
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE cLabel='%s'",cLabel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
				{
					mysql_free_result(res);
					tContainer("<blink>Error:</blink> cLabel already in use");
				}
				mysql_free_result(res);

				GetDatacenterProp(uDatacenter,"NewContainerCloneRange",cNCCloneRange);
				if(cNCCloneRange[0] && uIpv4InCIDR4(ForeignKey("tIP","cLabel",uIPv4),cNCCloneRange))
					tContainer("<blink>Error:</blink> uIPv4 must not be in datacenter clone IP range");

				if(uOSTemplate==0)
					tContainer("<blink>Error:</blink> You must select a uOSTemplate");
				if(uConfig==0)
					tContainer("<blink>Error:</blink> You must select a uConfig");
				if(uNameserver==0)
					tContainer("<blink>Error:</blink> You must select a uNameserver");
				if(uSearchdomain==0)
					tContainer("<blink>Error:</blink> You must select a uSearchdomain");

				if(uGroup==0 && cService3[0]==0)
					tContainer("<blink>Error:</blink> Group is now required");
				if(uGroup!=0 && cService3[0]!=0)
					tContainer("<blink>Error:</blink> Or select a group or create a new one, not both");


				//DNS sanity check
				if(uCreateDNSJob)
				{
					cunxsBindARecordJobZone[0]=0;
					GetConfiguration("cunxsBindARecordJobZone",cunxsBindARecordJobZone,uDatacenter,0,0,0);
					if(!cunxsBindARecordJobZone[0])
						tContainer("<blink>Error:</blink> Create job for unxsBind,"
								" but no cunxsBindARecordJobZone");
					
					if(!strstr(cHostname+(uHostnameLen-strlen(cunxsBindARecordJobZone)-1),cunxsBindARecordJobZone))
						tContainer("<blink>Error:</blink> cHostname must end with cunxsBindARecordJobZone");
				}
					
				if(cService1[0] && strlen(cService1)<6)
					tContainer("<blink>Error:</blink> Optional password must be at least 6 chars");
				//Direct datacenter checks
				sscanf(ForeignKey("tIP","uDatacenter",uIPv4),"%u",&uIPv4Datacenter);
				if(uDatacenter!=uIPv4Datacenter)
					tContainer("<blink>Error:</blink> The specified uIPv4 does not "
							"belong to the specified uDatacenter.");
				sscanf(ForeignKey("tNode","uDatacenter",uNode),"%u",&uNodeDatacenter);
				if(uDatacenter!=uNodeDatacenter)
					tContainer("<blink>Error:</blink> The specified uNode does not "
							"belong to the specified uDatacenter.");
				//tProperty based datacenter checks
				//VETH device
				if(uVeth)
				{
					GetNodeProp(uNode,"Container-Type",cContainerType);
					if(!strstr(cContainerType,"VETH"))
						tContainer("<blink>Error:</blink> uNode selected does not support VETH");
						
				}

				//If auto clone setup check required values
				cAutoCloneNode[0]=0;
				GetConfiguration("cAutoCloneNode",cAutoCloneNode,uDatacenter,uNode,0,0);
				if(cAutoCloneNode[0])
				{

					if(uTargetNode==0)
						tContainer("<blink>Error:</blink> Please select a valid target node"
								" for the clone.");
					if(uTargetNode==uNode)
						tContainer("<blink>Error:</blink> Can't clone to same node.");

					GetNodeProp(uTargetNode,"NewContainerMode",cNCMNode);
					if(cNCMNode[0] && !strstr(cNCMNode,"Clone"))
					tContainer("<blink>Error:</blink> Selected clone target node is not configured for clone containers."
							"Select another.");

					sscanf(ForeignKey("tNode","uDatacenter",uTargetNode),"%u",&uNodeDatacenter);
					//if(uDatacenter!=uNodeDatacenter)
					//	tContainer("<blink>Error:</blink> The specified clone uNode does not "
					//		"belong to the specified uDatacenter.");
					if(!uWizIPv4)
						tContainer("<blink>Error:</blink> You must select an IP for the clone");
					if(uWizIPv4==uIPv4)
						tContainer("<blink>Error:</blink> You must select a different IP for the"
										" clone");
					sscanf(ForeignKey("tIP","uDatacenter",uWizIPv4),"%u",&uIPv4Datacenter);
					if(uNodeDatacenter!=uIPv4Datacenter)
						tContainer("<blink>Error:</blink> The specified clone uIPv4 does not "
							"belong to the specified uNodeDatacenter.");
					//Allow for two ranges to be configured
					if(cNCCloneRange[0] && !uIpv4InCIDR4(ForeignKey("tIP","cLabel",uWizIPv4),cNCCloneRange))
					{
						GetDatacenterProp(uDatacenter,"NewContainerCloneRange2",cNCCloneRange);
						if(cNCCloneRange[0] && !uIpv4InCIDR4(ForeignKey("tIP","cLabel",uWizIPv4),cNCCloneRange))
							tContainer("<blink>Error:</blink> Clone start uIPv4 must be in datacenter clone IP range");
					}
					if(uSyncPeriod>86400*30 || (uSyncPeriod && uSyncPeriod<300))
						tContainer("<blink>Error:</blink> Clone uSyncPeriod out of range:"
								" Max 30 days, min 5 minutes or 0 off.");
					//tContainer("<blink>Error:</blink> d1");
				}

				//TODO review this policy.
				//No same names or hostnames for same datacenter allowed.
				//TODO periods "." should be expanded to "[.]"
				//for correct cHostname REGEXP.
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE ("
							" cHostname REGEXP '^%s[0-9]+%s$'"
							" OR cLabel REGEXP '%s[0-9]+$'"
							" ) AND uDatacenter=%u LIMIT 1",
								cLabel,cHostname,cLabel,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
				{
					mysql_free_result(res);
					//debug only
					//tContainer(gcQuery);
					tContainer("<blink>Error:</blink> Single container, similar cHostname"
					" cLabel pattern already used at this datacenter!");
				}
				mysql_free_result(res);

				//Check to see if enough IPs are available early to avoid
				//complex cleanup/rollback below
				//First get class c mask will be used here and again in main loop below

				//Get container IP cIPv4ClassC
				sprintf(gcQuery,"SELECT cLabel FROM tIP WHERE uIP=%u AND uAvailable=1"
						" AND uOwner=%u AND uDatacenter=%u",uIPv4,uForClient,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					sprintf(cIPv4ClassC,"%.31s",field[0]);
				}
				else
				{
					tContainer("<blink>Error:</blink> Someone grabbed your IP"
						", single container creation aborted -if Root select"
						" a company with IPs!");
				}
				mysql_free_result(res);
				for(i=strlen(cIPv4ClassC);i>0;i--)
				{
					if(cIPv4ClassC[i]=='.')
					{
						cIPv4ClassC[i]=0;
						break;
					}
				}
				//Check IPs for clones first if so configured
				if(cAutoCloneNode[0])
				{
					//TODO clean...uNodeDatacenter is set above in clone section. So it is really
					//uCloneNodeDatacenter
					sprintf(gcQuery,"SELECT cLabel FROM tIP WHERE uIP=%u AND uAvailable=1"
						" AND uOwner=%u AND uDatacenter=%u",uWizIPv4,uForClient,uNodeDatacenter);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					res=mysql_store_result(&gMysql);
					if((field=mysql_fetch_row(res)))
					{
						sprintf(cIPv4ClassCClone,"%.31s",field[0]);
					}
					else
					{
						tContainer("<blink>Error:</blink> Someone grabbed your clone IP"
							", single container creation aborted -if Root select"
							" a company with IPs!");
					}
					mysql_free_result(res);
					for(i=strlen(cIPv4ClassCClone);i>0;i--)
					{
						if(cIPv4ClassCClone[i]=='.')
						{
							cIPv4ClassCClone[i]=0;
							break;
						}
					}
					//TODO --WHY can't they?
					if(!strcmp(cIPv4ClassCClone,cIPv4ClassC))
						tContainer("<blink>Error:</blink> Clone IPs must belong to a different"
								" class C");
					//Count clone IPs
					sprintf(gcQuery,"SELECT COUNT(uIP) FROM tIP WHERE uAvailable=1 AND uOwner=%u"
							" AND cLabel LIKE '%s%%' AND uDatacenter=%u",
								uForClient,cIPv4ClassCClone,uNodeDatacenter);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					res=mysql_store_result(&gMysql);
					if((field=mysql_fetch_row(res)))
						sscanf(field[0],"%u",&uAvailableIPs);
					mysql_free_result(res);
					if(!uAvailableIPs)
						tContainer("<blink>Error:</blink> No clone IP in given"
							" class C"
							" available!");
				}//cAutoCloneNode

				//Count main IPs
				uAvailableIPs=0;
				sprintf(gcQuery,"SELECT COUNT(uIP) FROM tIP WHERE uAvailable=1 AND uOwner=%u"
						" AND cLabel LIKE '%s%%' AND uDatacenter=%u",
								uForClient,cIPv4ClassC,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
					sscanf(field[0],"%u",&uAvailableIPs);
				mysql_free_result(res);
				if(!uAvailableIPs)
					tContainer("<blink>Error:</blink> No IP in given class C"
						" available!");

				//User chooses to create a new group
				if(cService3[0])
				{
					if(strlen(cService3)<3)
						tContainer("<blink>Error:</blink> New tGroup.cLabel too short!");
					sprintf(gcQuery,"SELECT uGroup FROM tGroup WHERE cLabel='%s'",cService3);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)>0)
					{
						tContainer("<blink>Error:</blink> New tGroup.cLabel already in use!");
					}
					else
					{
						sprintf(gcQuery,"INSERT INTO tGroup SET cLabel='%s',uGroupType=1,"//1 is container type
							"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
								cService3,uForClient,guLoginClient);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						uGroup=mysql_insert_id(&gMysql);
					}
					mysql_free_result(res);
				}


				unsigned uApplianceIPv4=0;
				unsigned uApplianceDatacenter=41;//My current default for testing
				unsigned uApplianceNode=81;//My current default for testing
				char cuApplianceDatacenter[256]={""};
				char cuApplianceNode[256]={""};
				GetConfiguration("uApplianceDatacenter",cuApplianceDatacenter,uDatacenter,0,0,0);
				sscanf(cuApplianceDatacenter,"%u",&uApplianceDatacenter);
				GetConfiguration("uApplianceNode",cuApplianceNode,uDatacenter,0,0,0);
				sscanf(cuApplianceNode,"%u",&uApplianceNode);
				if(uCreateAppliance)
				{

					//New tIP for remote appliance
					sprintf(gcQuery,"SELECT uIP FROM tIP WHERE cLabel='%s' AND uAvailable=1",gcIPv4);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)>0)
					{
						if((field=mysql_fetch_row(res)))
							sscanf(field[0],"%u",&uApplianceIPv4);
					}
					else
					{
						sprintf(gcQuery,"INSERT INTO tIP SET cLabel='%s',uDatacenter=%u,"
							"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
								gcIPv4,uApplianceDatacenter,uForClient,guLoginClient);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						uApplianceIPv4=mysql_insert_id(&gMysql);
					}
					if(!uApplianceIPv4)
					{
						tContainer("<blink>Error:</blink> uApplianceIPv4 not determined!!");
					}
				}

				unsigned uApplianceContainer=0;
				char cApplianceLabel[33]={""};
				char cApplianceHostname[100]={""};
				if(uCreateAppliance)
				{
					char *cp;

					if((cp=strstr(cLabel,"-app")))
					{
						*cp=0;
						sprintf(cApplianceLabel,"%.32s",cLabel);
						*cp='-';
					}

					if((cp=strstr(cHostname,"-app.")))
						sprintf(cApplianceHostname,"%.99s",cp+5);

					if(cApplianceLabel[0]==0 || cApplianceHostname[0]==0)
						tContainer("<blink>Error:</blink> cApplianceLabel/cApplianceHostname not defined");

					sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE cLabel='%s'",cApplianceLabel);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)>0)
					{
						mysql_free_result(res);
						tContainer("<blink>Error:</blink> cApplianceLabel already in use");
					}
					mysql_free_result(res);

#define uREMOTEAPPLIANCE 101
					sprintf(gcQuery,"INSERT INTO tContainer SET cLabel='%s',cHostname='%s.%s',"
							"uIPv4=%u,"
							"uDatacenter=%u,"
							"uNode=%u,"
							"uStatus=%u,"
							"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
								cApplianceLabel,cApplianceLabel,cApplianceHostname,
								uApplianceIPv4,
								uApplianceDatacenter,
								uApplianceNode,
								uREMOTEAPPLIANCE,
								uForClient,
								guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					uApplianceContainer=mysql_insert_id(&gMysql);

					if(!uApplianceContainer)
						tContainer("<blink>Error:</blink> uApplianceContainer not determined!!");

					//Add to appliance group
					if(uGroup)
						UpdatePrimaryContainerGroup(uApplianceContainer,uGroup);

					//Add properties
					//Name property
					sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='Name',cValue='%s'",
							uApplianceContainer,uForClient,guLoginClient,cApplianceLabel);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					//cOrg_FreePBXAdminPasswd property
					sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='cOrg_FreePBXAdminPasswd',cValue='unknown'",
							uApplianceContainer,uForClient,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					//cPasswd property
					sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='cPasswd',cValue='unknown'",
							uApplianceContainer,uForClient,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));

				}//end if uCreateAppliance

				//
				//debug after initial checks
				//char cBuffer[256];
				//sprintf(cBuffer,"uIPv4=%u uOSTemplate=%u uConfig=%u uNameserver=%u uSearchdomain=%u"
				//		" uNode=%u uDatacenter=%u",
				//			uIPv4,uOSTemplate,uConfig,uNameserver,uSearchdomain,
				//			uNode,uDatacenter);
				//tContainer(cBuffer);
				//

				//Checks done commited to create
                       		guMode=0;
				uStatus=uINITSETUP;//Initial setup
				uContainer=0;
				uCreatedBy=guLoginClient;
				guCompany=uForClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				//Convenience
				if(uCreateAppliance)
					sprintf(cSearch,"%.31s",cApplianceLabel);
				else
					sprintf(cSearch,"%.31s",cLabel);


				//This sets new file global uContainer
				uContainer=0;
				if(uCreateAppliance)
					uSource=uApplianceContainer;
				NewtContainer(1);

				//tIP
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
						" WHERE uIP=%u AND uAvailable=1 AND uOwner=%u AND uDatacenter=%u",
									uIPv4,uForClient,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				if(mysql_affected_rows(&gMysql)!=1)
				{
					sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",
						uContainer);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					tContainer("<blink>Error:</blink> Someone grabbed your IP"
						", container creation aborted!");
				}
				
				//Name property
				sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='Name',cValue='%s'",
							uContainer,uForClient,guLoginClient,cLabel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));

				//Optional passwd
				if(cService1[0])
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='cPasswd',cValue='%s'",
							uContainer,uForClient,guLoginClient,cService1);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				}

				//Optional timezone note the --- not selected value.
				if(gcNewContainerTZ[0]!='-')
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_TimeZone',cValue='%s',uType=3,uKey=%u"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							gcNewContainerTZ,uContainer,uForClient,guLoginClient);
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
							uContainer,uForClient,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));

					sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='cVEID.start',cValue='defaultVETH.start'",
							uContainer,uForClient,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				}

				if(uGroup)
					UpdatePrimaryContainerGroup(uContainer,uGroup);

				if(cAutoCloneNode[0])
				{
					//TODO what about clone datacenter?
					uNewVeid=CommonCloneContainer(
									uContainer,
									uOSTemplate,
									uConfig,
									uNameserver,
									uSearchdomain,
									uDatacenter,
									uNodeDatacenter,//uCloneNodeDatacenter
									uForClient,
									cLabel,
									uNode,
									uStatus,
									cHostname,
									cIPv4ClassCClone,
									uWizIPv4,
									cWizLabel,
									cWizHostname,
									uTargetNode,
									uSyncPeriod,
									guLoginClient,
									uCloneStop,0);
					SetContainerStatus(uContainer,uINITSETUP);
					if(uGroup)
						UpdatePrimaryContainerGroup(uNewVeid,uGroup);

						//TODO cIPv4ClassCClone can't = cIPv4ClassC
						//Get next available uWizIPv4
					sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uAvailable=1 AND uOwner=%u"
							" AND cLabel LIKE '%s%%' AND uDatacenter=%u LIMIT 1",
								uForClient,cIPv4ClassCClone,uNodeDatacenter);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					res=mysql_store_result(&gMysql);
					if((field=mysql_fetch_row(res)))
						sscanf(field[0],"%u",&uWizIPv4);
					else
						tContainer("<blink>Error:</blink> No clone IP available"
								", container creation aborted!");
					mysql_free_result(res);

					//Create DNS job for clones also
					if(uCreateDNSJob)
						CreateDNSJob(uWizIPv4,uForClient,NULL,cWizHostname,uNodeDatacenter,guLoginClient);
				}//cAutoCloneNode

				if(uCreateDNSJob)
					CreateDNSJob(uIPv4,uForClient,NULL,cHostname,uDatacenter,guLoginClient);

				tContainer("New container created");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}//Single and appliance container creation end
		else if(!strcmp(gcCommand,"Multiple Container Creation"))
                {
			if(guPermLevel>=9)
			{
                        	ProcesstContainerVars(entries,x);

                        	guMode=9003;
				if(!uDatacenter)
					tContainer("<blink>Error:</blink> Must select a datacenter.");
				if(!uForClient)
					tContainer("<blink>Error:</blink> Must select an organization"
							" (company, NGO or similar.)");
				if(!uNode)
					tContainer("<blink>Error:</blink> Must select a node.");

				GetDatacenterProp(uDatacenter,"NewContainerMode",cNCMDatacenter);
				if(cNCMDatacenter[0] && !strstr(cNCMDatacenter,"Active"))
					tContainer("<blink>Error:</blink> Selected datacenter is full or not active. Select another.");

				GetNodeProp(uNode,"NewContainerMode",cNCMNode);
				if(cNCMNode[0] && !strstr(cNCMNode,"Active"))
					tContainer("<blink>Error:</blink> Selected node is not configured for active containers."
							"Select another.");

                        	guMode=9004;
	                        tContainer("New container step 4");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
		else if(!strcmp(gcCommand,"Create Multiple Containers"))
                {
			if(guPermLevel>=9)
			{
				unsigned uNumContainer=0;
				MYSQL_ROW field;
				unsigned uNewVeid=0;
				register int i;
				char cIPv4ClassC[32]={""};
				char cIPv4ClassCClone[32]={""};
				unsigned uAvailableIPs=0;
				char cGenLabel[32]={""};
				char cGenHostname[100]={""};
				char cOrgLabel[32]={""};
				char cOrgHostname[100]={""};
				unsigned uHostnameLen=0;
				unsigned uLabelLen=0;
				unsigned uTargetDatacenter=0;

                        	ProcesstContainerVars(entries,x);
				sscanf(cService2,"%u",&uNumContainer);

                        	guMode=9004;
				//Check entries here
				if(uNumContainer==0)
					tContainer("<blink>Error:</blink> You must specify the number of containers");
				if(uNumContainer>64)
				{
					sprintf(cService2,"64");
					tContainer("<blink>Error:</blink> The maximum number of containers you can"
							" create at once is 64");
				}
				if(uDatacenter==0)
					tContainer("<blink>Error:</blink> Unexpected uDatacenter==0!");
				if(uNode==0)
					tContainer("<blink>Error:</blink> Unexpected uNode==0!");

				GetDatacenterProp(uDatacenter,"NewContainerMode",cNCMDatacenter);
				if(cNCMDatacenter[0] && !strstr(cNCMDatacenter,"Active"))
					tContainer("<blink>Error:</blink> Selected datacenter is full or not active. Select another.");

				GetNodeProp(uNode,"NewContainerMode",cNCMNode);
				if(cNCMNode[0] && !strstr(cNCMNode,"Active"))
					tContainer("<blink>Error:</blink> Selected node is not configured for active containers."
							"Select another.");

				if((uLabelLen=strlen(cLabel))<2)
					tContainer("<blink>Error:</blink> cLabel is too short");
				if(strchr(cLabel,'.'))
					tContainer("<blink>Error:</blink> cLabel has at least one '.'");
				if(strstr(cLabel,"-clone"))
					tContainer("<blink>Error:</blink> cLabel can't have '-clone'");
				if((uHostnameLen=strlen(cHostname))<5)
					tContainer("<blink>Error:</blink> cHostname is too short");
				if(cHostname[uHostnameLen-1]=='.')
					tContainer("<blink>Error:</blink> cHostname can't end with a '.'");
				if(strstr(cHostname,"-clone"))
					tContainer("<blink>Error:</blink> cHostname can't have '-clone'");
				if(cHostname[0]!='.')
					tContainer("<blink>Error:</blink> Multiple containers cHostname has"
							" to start with '.'");
				if((uHostnameLen+uLabelLen)>62)
					tContainer("<blink>Error:</blink> Combined length of cLabel+cHostname is too long");

				if(uIPv4==0)
					tContainer("<blink>Error:</blink> You must select a uIPv4");
				GetDatacenterProp(uDatacenter,"NewContainerCloneRange",cNCCloneRange);
				if(cNCCloneRange[0] && uIpv4InCIDR4(ForeignKey("tIP","cLabel",uIPv4),cNCCloneRange))
					tContainer("<blink>Error:</blink> uIPv4 must not be in datacenter clone IP range");

				if(uOSTemplate==0)
					tContainer("<blink>Error:</blink> You must select a uOSTemplate");
				if(uConfig==0)
					tContainer("<blink>Error:</blink> You must select a uConfig");
				if(uNameserver==0)
					tContainer("<blink>Error:</blink> You must select a uNameserver");
				if(uSearchdomain==0)
					tContainer("<blink>Error:</blink> You must select a uSearchdomain");
				if(uGroup==0 && cService3[0]==0)
					tContainer("<blink>Error:</blink> Group is now required");
				if(uGroup!=0 && cService3[0]!=0)
					tContainer("<blink>Error:</blink> Or select a group or create a new one, not both");

				//DNS sanity check
				if(uCreateDNSJob)
				{
					cunxsBindARecordJobZone[0]=0;
					GetConfiguration("cunxsBindARecordJobZone",cunxsBindARecordJobZone,uDatacenter,0,0,0);
					if(!cunxsBindARecordJobZone[0])
						tContainer("<blink>Error:</blink> Create job for unxsBind,"
								" but no cunxsBindARecordJobZone");
					
					if(!strstr(cHostname+(uHostnameLen-strlen(cunxsBindARecordJobZone)-1),cunxsBindARecordJobZone))
						tContainer("<blink>Error:</blink> cHostname must end with cunxsBindARecordJobZone");
				}
					
				if(cService1[0] && strlen(cService1)<6)
					tContainer("<blink>Error:</blink> Optional password must be at least 6 chars");
				//Direct datacenter checks
				sscanf(ForeignKey("tIP","uDatacenter",uIPv4),"%u",&uIPv4Datacenter);
				if(uDatacenter!=uIPv4Datacenter)
					tContainer("<blink>Error:</blink> The specified uIPv4 does not "
							"belong to the specified uDatacenter.");
				sscanf(ForeignKey("tNode","uDatacenter",uNode),"%u",&uNodeDatacenter);
				if(uDatacenter!=uNodeDatacenter)
					tContainer("<blink>Error:</blink> The specified uNode does not "
							"belong to the specified uDatacenter.");
				//tProperty based datacenter checks
				//VETH device
				if(uVeth)
				{
					GetNodeProp(uNode,"Container-Type",cContainerType);
					if(!strstr(cContainerType,"VETH"))
						tContainer("<blink>Error:</blink> uNode selected does not support VETH");
						
				}

				//If auto clone setup check required values
				GetConfiguration("cAutoCloneNode",cAutoCloneNode,uDatacenter,uNode,0,0);
				if(cAutoCloneNode[0])
				{

					if(uTargetNode==0)
						tContainer("<blink>Error:</blink> Please select a valid target node"
								" for the clone");
					if(uTargetNode==uNode)
						tContainer("<blink>Error:</blink> Can't clone to same node");

					GetNodeProp(uTargetNode,"NewContainerMode",cNCMNode);
					if(cNCMNode[0] && !strstr(cNCMNode,"Clone"))
					tContainer("<blink>Error:</blink> Selected node is not configured for clone containers."
							"Select another.");

					//We are moving to normal remote datacenter clone containers
					sscanf(ForeignKey("tNode","uDatacenter",uTargetNode),"%u",&uTargetDatacenter);
					//if(uDatacenter!=uTargetDatacenter)
					//	tContainer("<blink>Error:</blink> The specified clone uNode does not "
					//		"belong to the specified uDatacenter.");
					if(!uWizIPv4)
						tContainer("<blink>Error:</blink> You must select an IP for the clone");
					if(uWizIPv4==uIPv4)
						tContainer("<blink>Error:</blink> You must select a different IP for the"
										" clone");
					sscanf(ForeignKey("tIP","uDatacenter",uWizIPv4),"%u",&uIPv4Datacenter);
					if(uTargetDatacenter!=uIPv4Datacenter)
						tContainer("<blink>Error:</blink> The specified clone uIPv4 does not "
							"belong to the specified uTargetDatacenter.");
					GetDatacenterProp(uTargetDatacenter,"NewContainerCloneRange",cNCCloneRange);
					if(cNCCloneRange[0] && !uIpv4InCIDR4(ForeignKey("tIP","cLabel",uWizIPv4),cNCCloneRange))
					{
						GetDatacenterProp(uTargetDatacenter,"NewContainerCloneRange2",cNCCloneRange);
						if(cNCCloneRange[0] && !uIpv4InCIDR4(ForeignKey("tIP","cLabel",uWizIPv4),cNCCloneRange))
						{
							GetDatacenterProp(uTargetDatacenter,"NewContainerCloneRange3",cNCCloneRange);
							if(cNCCloneRange[0] && !uIpv4InCIDR4(ForeignKey("tIP","cLabel",uWizIPv4),cNCCloneRange))
								tContainer("<blink>Error:</blink> Clone start uIPv4 must be in datacenter"
										" clone IP range (cmc)");
						}
					}
					if(uSyncPeriod>86400*30 || (uSyncPeriod && uSyncPeriod<300))
						tContainer("<blink>Error:</blink> Clone uSyncPeriod out of range:"
								" Max 30 days, min 5 minutes or 0 off.");
				}

				//TODO review this policy.
				//No same names or hostnames for same datacenter allowed.
				//TODO periods "." should be expanded to "[.]"
				//for correct cHostname REGEXP.
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE ("
							" cHostname REGEXP '^%s[0-9]+%s$'"
							" OR cLabel REGEXP '%s[0-9]+$'"
							" ) AND uDatacenter=%u LIMIT 1",
								cLabel,cHostname,cLabel,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
				{
					mysql_free_result(res);
					//debug only
					//tContainer(gcQuery);
					tContainer("<blink>Error:</blink> Multiple containers, similar cHostname"
					" cLabel pattern already used at this datacenter!");
				}
				mysql_free_result(res);

				//Check to see if enough IPs are available early to avoid
				//complex cleanup/rollback below
				//First get class c mask will be used here and again in main loop below

				//Get container IP cIPv4ClassC
				sprintf(gcQuery,"SELECT cLabel FROM tIP WHERE uIP=%u AND uAvailable=1"
						" AND uOwner=%u AND uDatacenter=%u",uIPv4,uForClient,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					sprintf(cIPv4ClassC,"%.31s",field[0]);
				}
				else
				{
					tContainer("<blink>Error:</blink> Someone grabbed your IP"
						", multiple container creation aborted -if Root select"
						" a company with IPs!");
				}
				mysql_free_result(res);
				for(i=strlen(cIPv4ClassC);i>0;i--)
				{
					if(cIPv4ClassC[i]=='.')
					{
						cIPv4ClassC[i]=0;
						break;
					}
				}
				//Check IPs for clones first if so configured
				if(cAutoCloneNode[0])
				{
					sprintf(gcQuery,"SELECT cLabel FROM tIP WHERE uIP=%u AND uAvailable=1"
						" AND uOwner=%u AND uDatacenter=%u",uWizIPv4,uForClient,uTargetDatacenter);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					res=mysql_store_result(&gMysql);
					if((field=mysql_fetch_row(res)))
					{
						sprintf(cIPv4ClassCClone,"%.31s",field[0]);
					}
					else
					{
						tContainer("<blink>Error:</blink> Someone grabbed your clone IP"
							", multiple container creation aborted -if Root select"
							" a company with IPs!");
					}
					mysql_free_result(res);
					for(i=strlen(cIPv4ClassCClone);i>0;i--)
					{
						if(cIPv4ClassCClone[i]=='.')
						{
							cIPv4ClassCClone[i]=0;
							break;
						}
					}
					//TODO
					if(!strcmp(cIPv4ClassCClone,cIPv4ClassC))
						tContainer("<blink>Error:</blink> Clone IPs must belong to a different"
								" class C");
					//Count clone IPs
					sprintf(gcQuery,"SELECT COUNT(uIP) FROM tIP WHERE uAvailable=1 AND uOwner=%u"
							" AND cLabel LIKE '%s%%' AND uDatacenter=%u",
								uForClient,cIPv4ClassCClone,uTargetDatacenter);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					res=mysql_store_result(&gMysql);
					if((field=mysql_fetch_row(res)))
						sscanf(field[0],"%u",&uAvailableIPs);
					mysql_free_result(res);
					if(uNumContainer>uAvailableIPs)
						tContainer("<blink>Error:</blink> Not enough clone IPs in given"
							" class C"
							" available!");
				}//cAutoCloneNode

				//Count main IPs
				uAvailableIPs=0;
				sprintf(gcQuery,"SELECT COUNT(uIP) FROM tIP WHERE uAvailable=1 AND uOwner=%u"
						" AND cLabel LIKE '%s%%' AND uDatacenter=%u",
								uForClient,cIPv4ClassC,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
					sscanf(field[0],"%u",&uAvailableIPs);
				mysql_free_result(res);
				if(uNumContainer>uAvailableIPs)
					tContainer("<blink>Error:</blink> Not enough IPs in given class C"
						" available!");

				//User chooses to create a new group
				if(cService3[0])
				{
					if(strlen(cService3)<3)
						tContainer("<blink>Error:</blink> New tGroup.cLabel too short!");
					sprintf(gcQuery,"SELECT uGroup FROM tGroup WHERE cLabel='%s'",cService3);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)>0)
					{
						tContainer("<blink>Error:</blink> New tGroup.cLabel already in use!");
					}
					else
					{
						sprintf(gcQuery,"INSERT INTO tGroup SET cLabel='%s',uGroupType=1,"//1 is container type
							"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
								cService3,uForClient,guLoginClient);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						uGroup=mysql_insert_id(&gMysql);
					}
					mysql_free_result(res);
				}

				//
				//debug after initial checks
				//char cBuffer[256];
				//sprintf(cBuffer,"uIPv4=%u uOSTemplate=%u uConfig=%u uNameserver=%u uSearchdomain=%u"
				//		" uNode=%u uDatacenter=%u",
				//			uIPv4,uOSTemplate,uConfig,uNameserver,uSearchdomain,
				//			uNode,uDatacenter);
				//tContainer(cBuffer);
				//

				//Checks done commited to create
                       		guMode=0;
				uStatus=uINITSETUP;//Initial setup
				uContainer=0;
				uCreatedBy=guLoginClient;
				guCompany=uForClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				sprintf(cOrgLabel,"%.31s",cLabel);
				sprintf(cOrgHostname,"%.68s",cHostname);
				//Convenience
				sprintf(cSearch,"%.31s",cLabel);

				//Loop creation
				for(i=0;i<uNumContainer;i++)
				{

					//Create cLabel and cHostname
					sprintf(cGenLabel,"%.29s%d",cOrgLabel,i);//i is limited to 2 digits
					sprintf(cGenHostname,"%.29s%d%.68s",cOrgLabel,i,cOrgHostname);
					if(strlen(cGenLabel)>31)
						tContainer("<blink>Error:</blink> cLabel length exceeded"
							", multiple container creation aborted!");
					if(strlen(cGenHostname)>64)
						tContainer("<blink>Error:</blink> cHostname length exceeded"
							", multiple container creation aborted!");
					sprintf(cLabel,"%.31s",cGenLabel);
					sprintf(cHostname,"%.64s",cGenHostname);//TODO check above

					//This sets new file global uContainer
					uContainer=0;
					NewtContainer(1);

					//tIP
					sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
						" WHERE uIP=%u AND uAvailable=1 AND uOwner=%u AND uDatacenter=%u",
									uIPv4,uForClient,uDatacenter);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					if(mysql_affected_rows(&gMysql)!=1)
					{
						sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",
							uContainer);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						tContainer("<blink>Error:</blink> Someone grabbed your IP"
							", multiple container creation aborted!");
					}
				
					//Name property
					sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='Name',cValue='%s'",
							uContainer,uForClient,guLoginClient,cLabel);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));

					//Optional passwd
					if(cService1[0])
					{
						sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='cPasswd',cValue='%s'",
							uContainer,uForClient,guLoginClient,cService1);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					}

					//Optional timezone note the --- not selected value.
					if(gcNewContainerTZ[0]!='-')
					{
						sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_TimeZone',cValue='%s',"
								"uType=3,uKey=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							gcNewContainerTZ,uContainer,uForClient,guLoginClient);
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
							uContainer,uForClient,guLoginClient);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));

						sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='cVEID.start',cValue='defaultVETH.start'",
							uContainer,uForClient,guLoginClient);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					}

					if(uGroup)
						UpdatePrimaryContainerGroup(uContainer,uGroup);

					if(cAutoCloneNode[0])
					{
						uNewVeid=CommonCloneContainer(
									uContainer,
									uOSTemplate,
									uConfig,
									uNameserver,
									uSearchdomain,
									uDatacenter,
									uTargetDatacenter,
									uForClient,
									cLabel,
									uNode,
									uStatus,
									cHostname,
									cIPv4ClassCClone,
									uWizIPv4,
									cWizLabel,
									cWizHostname,
									uTargetNode,
									uSyncPeriod,
									guLoginClient,
									uCloneStop,0);
						SetContainerStatus(uContainer,uINITSETUP);
						if(uGroup)
							UpdatePrimaryContainerGroup(uNewVeid,uGroup);

						//Get next available clone uIPv4 only if not last loop iteration
						if((i+1)<uNumContainer)
						{
							//TODO cIPv4ClassCClone can't = cIPv4ClassC
							//Get next available uWizIPv4
							sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uAvailable=1 AND uOwner=%u"
								" AND cLabel LIKE '%s%%' AND uDatacenter=%u LIMIT 1",
									uForClient,cIPv4ClassCClone,uTargetDatacenter);
							mysql_query(&gMysql,gcQuery);
							if(mysql_errno(&gMysql))
								htmlPlainTextError(mysql_error(&gMysql));
							res=mysql_store_result(&gMysql);
							if((field=mysql_fetch_row(res)))
								sscanf(field[0],"%u",&uWizIPv4);
							else
								tContainer("<blink>Error:</blink> No more clone IPs available"
									", multiple container creation aborted!");
							mysql_free_result(res);
						}

						if(uCreateDNSJob)
							CreateDNSJob(uWizIPv4,uForClient,NULL,cWizHostname,uTargetDatacenter,guLoginClient);
					}//cAutoCloneNode


					//For some reason cHostname is cLabel at this point. TODO debug.
					if(uCreateDNSJob)
						CreateDNSJob(uIPv4,uForClient,NULL,cHostname,uDatacenter,guLoginClient);

					//Get next available uIPv4 only if not last loop iteration
					if((i+1)<uNumContainer)
					{
						sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uAvailable=1 AND uOwner=%u"
						" AND cLabel LIKE '%s%%' AND uDatacenter=%u LIMIT 1",
								uForClient,cIPv4ClassC,uDatacenter);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						res=mysql_store_result(&gMysql);
						if((field=mysql_fetch_row(res)))
							sscanf(field[0],"%u",&uIPv4);
						else
							tContainer("<blink>Error:</blink> No more IPs available"
							", multiple container creation aborted!");
						mysql_free_result(res);
					}

				}//end of loop

				tContainer("New containers created");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}//Multiple containers
		//New wizard section ends



		//TODO mount settings wizard
                else if(!strcmp(gcCommand,"Confirm Mount Settings") ||
			!strcmp(gcCommand,"Confirm Container Settings"))
                {
                        ProcesstContainerVars(entries,x);
			if(guPermLevel>=9)
			{

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it!");

                        	guMode=200;
				if(uStatus!=uINITSETUP)
					tContainer("<blink>Unexpected Error</blink>: uStatus not 'Initial Setup'");

                        	guMode=201;
				if(!uVeth)
				{
					if(uCheckMountSettings(uMountTemplate))
						tContainer("<blink>Error:</blink> Incorrect mount settings!");
					AddMountProps(uContainer);
				}
			
                        	guMode=0;
				tContainer("New container setup completed");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstContainerVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");

	                        guMode=2001;
				tContainer(LANG_NB_CONFIRMDEL);
			}
			else
			{
				tContainer("<blink>Error:</blink> Delete denied by permissions settings");
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstContainerVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");

	                        guMode=2001;
				//Special safety for root level cleanup dels
				if(uStatus!=uINITSETUP && uSource)
				{
					sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uContainer=%u",uSource);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        				res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)!=0)
						tContainer("<blink>Error:</blink> This container has a source. Delete it first.");
				}

				guMode=5;
				uStatus=0;//Internal hack for deploy button turn off
				//Release IPs
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=1,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
						" WHERE uIP=%u AND uAvailable=0",guLoginClient,uIPv4);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				//TODO this operation may not be datacenter safe review
				//Node IP if any MySQL5+
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=1,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE cLabel IN"
						" (SELECT cValue FROM tProperty WHERE uKey=%u"
						" AND uType=3 AND cName='cNodeIP')",guLoginClient,uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				//Now we can remove properties
				DelProperties(uContainer,3);
				//Remove from any groups
				sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer=%u",uContainer);//Ok to delete from all groups
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				CancelContainerJob(uDatacenter,uNode,uContainer,0);
				//81=Awaiting clone
				sprintf(gcQuery,"DELETE FROM tContainer WHERE uStatus=81 AND uSource=%u",uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer IN "
						"(SELECT uContainer FROM tContainer WHERE uSource=%u AND uStatus=81)",uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				//Release clone IP
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=1,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
						" WHERE uIP=(SELECT uIPv4 FROM tContainer WHERE uSource=%u AND uStatus=81)"
						" AND uAvailable=0",guLoginClient,uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				DeletetContainer();
			}
			else
			{
				tContainer("<blink>Error:</blink> Delete denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstContainerVars(entries,x);
			if( (uStatus==uINITSETUP) && uAllowMod(uOwner,uCreatedBy))
			{
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");

				guMode=2002;

				tContainer(LANG_NB_CONFIRMMOD);
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstContainerVars(entries,x);
			if( (uStatus==uINITSETUP) && uAllowMod(uOwner,uCreatedBy))
			{
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");

                        	guMode=2002;
				//Check entries here
				if(uContainer==0)
					tContainer("<blink>Error:</blink> uContainer==0!");
				if(uDatacenter==0)
					tContainer("<blink>Error:</blink> uDatacenter==0!");
				if(uNode==0)
					tContainer("<blink>Error:</blink> uNode==0!");
				sscanf(ForeignKey("tNode","uDatacenter",uNode),"%u",&uNodeDatacenter);
				if(uDatacenter!=uNodeDatacenter)
					tContainer("<blink>Error:</blink> The specified uNode does not "
							"belong to the specified uDatacenter.");
				if(uIPv4==0)
					tContainer("<blink>Error:</blink> uIPv4==0!");
				if(uOSTemplate==0)
					tContainer("<blink>Error:</blink> uOSTemplate==0!");
				if(uConfig==0)
					tContainer("<blink>Error:</blink> uConfig==0!");
				if(uNameserver==0)
					tContainer("<blink>Error:</blink> uNameserver==0!");
				if(uSearchdomain==0)
					tContainer("<blink>Error:</blink> uSearchdomain==0!");
				if(strlen(cHostname)<5)
					tContainer("<blink>Error:</blink> cHostname too short!");
				if(strlen(cLabel)<2)
					tContainer("<blink>Error:</blink> cLabel too short!");
				if(strstr(cLabel,"-clone"))
					tContainer("<blink>Error:</blink> cLabel can't have '-clone'!");
				if(strstr(cHostname,"-clone"))
					tContainer("<blink>Error:</blink> cHostname can't have '-clone'!");
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
					tContainer("<blink>Error:</blink> cHostname or cLabel already used at this"
							" datacenter!");
				}
				mysql_free_result(res);
                        	guMode=0;

				//Set the selected IP as not available upon modify
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
						" WHERE uIP=%u AND uAvailable=1",uIPv4);

				//Optional change group.
				if(uGroup)
					UpdatePrimaryContainerGroup(uContainer,uGroup);
						
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
					
				uModBy=guLoginClient;
				ModtContainer();
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
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
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");

				if(CreateStartContainerJob(uDatacenter,uNode,uContainer,uOwner))
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
					tContainer("<blink>Error:</blink> No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
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
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");

				if(CreateNewContainerJob(uDatacenter,uNode,uContainer,uOwner))
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
					tContainer("<blink>Error:</blink> No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
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
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");

				if(DestroyContainerJob(uDatacenter,uNode,uContainer,uOwner))
				{
					uStatus=uAWAITDEL;
					SetContainerStatus(uContainer,5);//Awaiting Deletion
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("DestroyContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error:</blink> No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strncmp(gcCommand,"Stop ",5))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;
					
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");

				if(StopContainerJob(uDatacenter,uNode,uContainer,guCompany))
				{
					uStatus=uAWAITSTOP;
					SetContainerStatus(uContainer,41);//Awaiting Stop
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("StopContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error:</blink> No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Cancel Job"))
                {
                        ProcesstContainerVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;
				if(uStatus!=uAWAITDEL && uStatus!=uAWAITACT && uStatus!=uAWAITSTOP)
					tContainer("<blink>Error:</blink> Unexpected uStatus!");

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Job may have run!");

				//Cancel any outstanding jobs.
				if(CancelContainerJob(uDatacenter,uNode,uContainer,1))
				{
					tContainer("<blink>Error:</blink> Unexpected no jobs canceled! Late?");
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
				tContainer("<blink>Error:</blink> Denied by permissions settings");
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
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");

				if(ActionScriptsJob(uDatacenter,uNode,uContainer))
				{
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("ActionScriptsJob() Done");
				}
				else
				{
					tContainer("<blink>Error:</blink> No jobs created!"
							" Make sure correct properties exist.");
				}
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Remote Clone Wizard"))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
				
				if(uSource)
					tContainer("<blink>Error:</blink> No clones of clones allowed");

				guMode=11001;
				tContainer("Select datacenter");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Select Clone Datacenter"))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
				
				guMode=11001;
				if(uSource)
					tContainer("<blink>Error:</blink> No clones of clones allowed");
				if(uTargetDatacenter==uDatacenter)
					tContainer("<blink>Error:</blink> Can't remote clone to same datacenter");

				guMode=11002;
				tContainer("Select node, uIPv4 and more");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Remote Clone"))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
				unsigned uNewVeid=0;

                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it");

                        	guMode=11002;
				if(uCloneStop>COLD_CLONE || uCloneStop<HOT_CLONE)
					tContainer("<blink>Error:</blink> Unexpected initial state");
				if(uTargetNode==0)
					tContainer("<blink>Error:</blink> Please select a valid target node");
				if(uTargetNode==uNode)
					tContainer("<blink>Error:</blink> Can't clone to same node");
				if(uTargetDatacenter==uDatacenter)
					tContainer("<blink>Error:</blink> Can't remote clone to same datacenter");
				if(uSource)
					tContainer("<blink>Error:</blink> No clones of clones allowed");
				if(!uWizIPv4)
					tContainer("<blink>Error:</blink> You must select an IP");
				sscanf(ForeignKey("tIP","uDatacenter",uWizIPv4),"%u",&uIPv4Datacenter);
				if(uTargetDatacenter!=uIPv4Datacenter)
					tContainer("<blink>Error:</blink> The specified target uIPv4 does not "
							"belong to the specified uDatacenter.");
				if(!uOSTemplate || !uConfig || !uNameserver || !uSearchdomain || !uDatacenter || !uTargetDatacenter )
					tContainer("<blink>Error:</blink> Unexpected problem with missing source container"
							" settings!");
				if(uSyncPeriod>86400*30 || (uSyncPeriod && uSyncPeriod<300))
						tContainer("<blink>Error:</blink> Clone uSyncPeriod seconds out of range:"
								" Max 30 days, min 5 minutes or 0 off.");
				if(uVeth)
				{
					GetNodeProp(uNode,"Container-Type",cContainerType);
					if(!strstr(cContainerType,"VETH"))
						tContainer("<blink>Error:</blink> uNode selected does not support VETH");
						
				}

				//we don't allow extra remote clones on same node --seems to be no reason to do so
				//	since you can always clone a local container if you need to make "copies."
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uSource=%u AND uDatacenter=%u"
							" AND uNode=%u",uContainer,uTargetDatacenter,uTargetNode);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
					tContainer("<blink>Error:</blink> A clone of this container already exists on selected node");
				mysql_free_result(res);

				//tContainer("debug break point");

				//Optional change group of source container.
				if(uGroup)
					UpdatePrimaryContainerGroup(uContainer,uGroup);

				//This is needed to distinguish destroyed clone containers from
				//initial setup clone containers for remote cold backup
				if(uCloneStop==COLD_CLONE)
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cName='cSyncMode',cValue='Cold'",
							uContainer,guCompany,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				}

				//Set local global cWizHostname
				//Insert clone container into tContainer
				uNewVeid=CommonCloneContainer(
					uContainer,
					uOSTemplate,
					uConfig,
					uNameserver,
					uSearchdomain,
					uDatacenter,
					uTargetDatacenter,
					uOwner,
					cLabel,
					uNode,
					uStatus,
					cHostname,
					"",//Do not check Class C
					uWizIPv4,
					cWizLabel,
					cWizHostname,
					uTargetNode,
					uSyncPeriod,
					guLoginClient,
					uCloneStop,0);

				//Set group of clone to group of source.
				uGroup=uGetPrimaryContainerGroup(uContainer);
				if(uGroup)
					UpdatePrimaryContainerGroup(uNewVeid,uGroup);
                        	guMode=0;
				tContainer("CloneContainerJob() Done");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
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
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
				
				guMode=7001;
				tContainer("Select target node, uIPv4 and set other settings");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Clone"))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
				unsigned uNewVeid=0;

                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it");

                        	guMode=7001;
				if(uCloneStop>WARM_CLONE || uCloneStop<HOT_CLONE)
					tContainer("<blink>Error:</blink> Unexpected initial state");
				if(uTargetNode==0)
					tContainer("<blink>Error:</blink> Please select a valid target node");
				if(uTargetNode==uNode)
					tContainer("<blink>Error:</blink> Can't clone to same node");
				if(uSource)
					tContainer("<blink>Error:</blink> No clones of clones allowed");
				if(!uWizIPv4)
					tContainer("<blink>Error:</blink> You must select an IP");
				sscanf(ForeignKey("tIP","uDatacenter",uWizIPv4),"%u",&uIPv4Datacenter);
				if(uDatacenter!=uIPv4Datacenter)
					tContainer("<blink>Error:</blink> The specified target uIPv4 does not "
							"belong to the specified uDatacenter.");
				if(!uOSTemplate || !uConfig || !uNameserver || !uSearchdomain || !uDatacenter )
					tContainer("<blink>Error:</blink> Unexpected problem with missing source container"
							" settings!");
				if(uSyncPeriod>86400*30 || (uSyncPeriod && uSyncPeriod<300))
						tContainer("<blink>Error:</blink> Clone uSyncPeriod seconds out of range:"
								" Max 30 days, min 5 minutes or 0 off.");
				if(uVeth)
				{
					GetNodeProp(uNode,"Container-Type",cContainerType);
					if(!strstr(cContainerType,"VETH"))
						tContainer("<blink>Error:</blink> uNode selected does not support VETH");
						
				}
                        	guMode=0;

				//Optional change group of source container.
				if(uGroup)
					UpdatePrimaryContainerGroup(uContainer,uGroup);

				//Set local global cWizHostname
				//Insert clone container into tContainer
				uNewVeid=CommonCloneContainer(
					uContainer,
					uOSTemplate,
					uConfig,
					uNameserver,
					uSearchdomain,
					uDatacenter,
					uDatacenter,
					uOwner,
					cLabel,
					uNode,
					uStatus,
					cHostname,
					"",//Do not check Class C
					uWizIPv4,
					cWizLabel,
					cWizHostname,
					uTargetNode,
					uSyncPeriod,
					guLoginClient,
					uCloneStop,0);

				//Set group of clone to group of source.
				uGroup=uGetPrimaryContainerGroup(uContainer);
				if(uGroup)
					UpdatePrimaryContainerGroup(uNewVeid,uGroup);
				tContainer("CloneContainerJob() Done");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Migration Wizard"))
                {
                        ProcesstContainerVars(entries,x);
			if( (uStatus==uACTIVE || uStatus==uSTOPPED ) && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
				guMode=3001;
				tContainer("Select Migration Target");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Migration"))
                {
                        ProcesstContainerVars(entries,x);
			if( (uStatus==uACTIVE || uStatus==uSTOPPED ) && uAllowMod(uOwner,uCreatedBy))
			{
				unsigned uTargetDatacenter=0;
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");

                        	guMode=3001;
				if(uTargetNode==uNode)
					tContainer("<blink>Error:</blink> Can't migrate to same node. Try 'Template Wizard'");
				if(uTargetNode==0)
					tContainer("<blink>Error:</blink> Please select a valid target node");
				sscanf(ForeignKey("tNode","uDatacenter",uTargetNode),"%u",&uTargetDatacenter);
				if(uTargetDatacenter!=uDatacenter)
					tContainer("<blink>Error:</blink> Can't migrate to different datacenter. Try 'Remote Migration'");
				if(uVeth)
				{
					GetNodeProp(uNode,"Container-Type",cContainerType);
					if(!strstr(cContainerType,"VETH"))
						tContainer("<blink>Error:</blink> uNode selected does not support VETH!");
						
				}
                        	guMode=0;

				//Optional change group.
				if(uGroup)
					UpdatePrimaryContainerGroup(uContainer,uGroup);

				if(MigrateContainerJob(uDatacenter,uNode,uContainer,uTargetNode,uOwner,guLoginClient,0,uStatus))
				{
					uStatus=uAWAITMIG;
					SetContainerStatus(uContainer,uAWAITMIG);//Awaiting Migration
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("MigrateContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error:</blink> No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Remote Migration"))
                {
                        ProcesstContainerVars(entries,x);
			if( (uStatus==uACTIVE||uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
				GetContainerProp(uContainer,"cOrg_PublicIP",gcUseThisIP);
				if(gcUseThisIP)
					tContainer("<blink>Error:</blink> NAT container remote migration not supported yet.");
				guMode=10001;
				tContainer("Select Migration Target");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Remote Migration"))
                {
                        ProcesstContainerVars(entries,x);
			if( (uStatus==uACTIVE||uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
				unsigned uTargetDatacenter=0;
				unsigned uIPv4Available=0;
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");

                        	guMode=10001;
				if(uTargetNode==uNode)
					tContainer("<blink>Error:</blink> Can't migrate to same node. Try 'Template Wizard'");
				if(uTargetNode==0)
					tContainer("<blink>Error:</blink> Please select a valid target node");
				sscanf(ForeignKey("tNode","uDatacenter",uTargetNode),"%u",&uTargetDatacenter);
				if(uTargetDatacenter==uDatacenter)
					tContainer("<blink>Error:</blink> Can't migrate to same datacenter. Try 'Migration Wizard'");
				if(uVeth)
				{
					GetNodeProp(uNode,"Container-Type",cContainerType);
					if(!strstr(cContainerType,"VETH"))
						tContainer("<blink>Error:</blink> uNode selected does not support VETH!");
						
				}
				if(uIPv4==0)
					tContainer("<blink>Error:</blink> Unexpected current uIPv4");
				if(uWizIPv4==0)
					tContainer("<blink>Error:</blink> Please select a valid uIPv4");
				sscanf(ForeignKey("tIP","uDatacenter",uWizIPv4),"%u",&uIPv4Datacenter);
				if(uTargetDatacenter!=uIPv4Datacenter)
					tContainer("<blink>Error:</blink> The specified uIPv4 does not "
							"belong to the remote datacenter.");
				sscanf(ForeignKey("tIP","uAvailable",uWizIPv4),"%u",&uIPv4Available);
				if(!uIPv4Available)
					tContainer("<blink>Error:</blink> The specified uIPv4 is no longer"
							"available, select another.");

				//External DNS job check
				unsigned uHostnameLen=0;
				cunxsBindARecordJobZone[0]=0;
				GetConfiguration("cunxsBindARecordJobZone",cunxsBindARecordJobZone,uDatacenter,0,0,0);
				if(!cunxsBindARecordJobZone[0])
					tContainer("<blink>Error:</blink> Create job for unxsBind,"
								" but no cunxsBindARecordJobZone");

				//This code should be compatible with new -clone hostname scheme
				char cPrevHostname[100]={""};
				if(uSource)
				{
					char cSourceHostname[100]={""};
					char cWizLabel[33]={""};
					char cNewLabel[33]={""};
					char *cp=NULL;
					unsigned uWizLabelLoop=1;
					unsigned uWizLabelSuffix=0;

					sprintf(cPrevHostname,"%s",cHostname);

					sprintf(cLabel,"%.25s",ForeignKey("tContainer","cLabel",uSource));
					//Get first available <cLabel>-clone<uNum>
					while(uWizLabelLoop)
					{
						uWizLabelSuffix++;
						if(uWizLabelSuffix>9) tContainer("<blink>Error:</blink> Clone limit reached");
						sprintf(cWizLabel,"%.25s-clone%u",cLabel,uWizLabelSuffix);
						sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE cLabel='%s'",cWizLabel);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						res=mysql_store_result(&gMysql);
						uWizLabelLoop=mysql_num_rows(res);
						mysql_free_result(res);
					}
					sprintf(cNewLabel,"%.25s-clone%u",cLabel,uWizLabelSuffix);
					sprintf(cLabel,"%.32s",cNewLabel);

					sprintf(cSourceHostname,"%.99s",ForeignKey("tContainer","cHostname",uSource));
					if((cp=strchr(cSourceHostname,'.')))
					{
						sprintf(cHostname,"%.32s.%.64s",cLabel,cp+1);
					}
				}

				uHostnameLen=strlen(cHostname);
				if(!strstr(cHostname+(uHostnameLen-strlen(cunxsBindARecordJobZone)-1),cunxsBindARecordJobZone))
						tContainer("<blink>Error:</blink> cHostname must end with cunxsBindARecordJobZone");
				if(uSource)
				{
					sprintf(gcQuery,"UPDATE tContainer SET cHostname='%s',cLabel='%s'"
								" WHERE uContainer=%u",cHostname,cLabel,uContainer);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				}

                        	guMode=0;

				//Optional change group.
				if(uGroup)
					UpdatePrimaryContainerGroup(uContainer,uGroup);

				if(MigrateContainerJob(uDatacenter,uNode,uContainer,uTargetNode,uOwner,guLoginClient,uWizIPv4,uStatus))
				{
					char cIPOld[32]={""};

					uStatus=uAWAITMIG;
					SetContainerStatus(uContainer,uAWAITMIG);//Awaiting Migration

					//Mark IP used
					sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
								" WHERE uIP=%u and uAvailable=1",uWizIPv4);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));

					//Change container IP
					sprintf(gcQuery,"UPDATE tContainer SET uIPv4=%u"
								" WHERE uContainer=%u",uWizIPv4,uContainer);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));

					//Create change IP job
					sprintf(cIPOld,"%.31s",ForeignKey("tIP","cLabel",uIPv4));
					if(!cIPOld[0])
						htmlPlainTextError("Unexpected !cIPOld");

					//Note that these jobs are for new node
					//Then this job must run after migration.
					IPContainerJob(uTargetDatacenter,uTargetNode,uContainer,uOwner,guLoginClient,cIPOld);
					//Create unxsBind DNS
					CreateDNSJob(uWizIPv4,uOwner,NULL,cHostname,uDatacenter,guLoginClient);

					//Create change hostname job
					if(uSource)
						HostnameContainerJob(uTargetDatacenter,uTargetNode,uContainer,cPrevHostname,uOwner,guLoginClient);

					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("MigrateContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error:</blink> No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}//Confirm Remote Migration
                else if(!strcmp(gcCommand,"Template Wizard"))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
				if(!uOSTemplate)
					tContainer("<blink>Error:</blink> No tOSTemplate.cLabel!");
				guMode=4001;
				tContainer("Select Template Name");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Template"))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
                        	guMode=4001;
				if(strlen(cConfigLabel)<2)
					tContainer("<blink>Error:</blink> Must provide valid tConfig.cLabel!");
				if(strchr(cConfigLabel,'-'))
					tContainer("<blink>Error:</blink> tConfig.cLabel can't contain '-' chars!");
				sprintf(gcQuery,"SELECT uConfig FROM tConfig WHERE cLabel='%s'",cConfigLabel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
					tContainer("<blink>Error:</blink> tConfig.cLabel already exists create another!");
				mysql_free_result(res);
				sprintf(gcQuery,"SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='%s'",cConfigLabel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
					tContainer("<blink>Error:</blink> tOSTemplate.cLabel collision."
							" Select another tConfig.cLabel!");
				mysql_free_result(res);
                        	guMode=0;
				if(TemplateContainerJob(uDatacenter,uNode,uContainer,uStatus,uOwner,cConfigLabel))
				{
					uStatus=uAWAITTML;
					SetContainerStatus(uContainer,uStatus);
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("TemplateContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error:</blink> No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Hostname Change Wizard"))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
				guMode=5001;
				tContainer("Provide container hostname and name");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Hostname Change"))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
				unsigned uHostnameLen=0;
				unsigned uLabelLen=0;

                        	guMode=0;
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
                        	guMode=5001;
				//debug only
				//if(uUsePublicIP && gcUseThisIP[0])
				//	tContainer(gcUseThisIP);
				if(!strcmp(cWizHostname,cHostname) && !strcmp(cWizLabel,cLabel))
					tContainer("<blink>Error:</blink> cHostname and cLabel are the same as original!");
				if((uHostnameLen=strlen(cWizHostname))<5)
					tContainer("<blink>Error:</blink> cHostname too short!");
				if((uLabelLen=strlen(cWizLabel))<2)
					tContainer("<blink>Error:</blink> cLabel too short!");
				if(strchr(cWizLabel,'.'))
					tContainer("<blink>Error:</blink> cLabel has at least one '.'!");
				if(strstr(cWizLabel,"-clone"))
					tContainer("<blink>Error:</blink> cLabel can't have '-clone'");
				if(strstr(cWizHostname,"-clone"))
					tContainer("<blink>Error:</blink> cHostname can't have '-clone'");
				//New rule: cLabel must be first part (first stop) of cHostname.
				if(strncmp(cWizLabel,cWizHostname,uLabelLen))
					tContainer("<blink>Error:</blink> cLabel must be first part of cHostname.");
				//DNS sanity check
				if(uCreateDNSJob)
				{
					cunxsBindARecordJobZone[0]=0;
					GetConfiguration("cunxsBindARecordJobZone",cunxsBindARecordJobZone,uDatacenter,0,0,0);
					if(!cunxsBindARecordJobZone[0])
						tContainer("<blink>Error:</blink> Create job for unxsBind,"
								" but no cunxsBindARecordJobZone");
					
					if(!strstr(cWizHostname+(uHostnameLen-strlen(cunxsBindARecordJobZone)-1),cunxsBindARecordJobZone))
						tContainer("<blink>Error:</blink> cHostname must end with cunxsBindARecordJobZone");
				}
				//No same names or hostnames for same XXXdatacenterXXX -now global restriction- allowed.
				sprintf(gcQuery,"SELECT uContainer,uStatus,uIPv4 FROM tContainer WHERE (cHostname='%s' OR cLabel='%s')"
						" AND uContainer!=%u",
							cWizHostname,cWizLabel,uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
				{
					unsigned uOfflineContainer=0;
					unsigned uOfflineStatusCheck=0;
					unsigned uOfflineIPv4=0;
        				MYSQL_ROW field;

					if((field=mysql_fetch_row(res)))
					{
						sscanf(field[0],"%u",&uOfflineContainer);
						sscanf(field[1],"%u",&uOfflineStatusCheck);
						sscanf(field[2],"%u",&uOfflineIPv4);
					}
					if(uOfflineStatusCheck==uOFFLINE && uOfflineContainer)
					{
						//Delete offline container with same name as
						//the change to name. Only the first one is removed for now.
						//Release IPs
						sprintf(gcQuery,"UPDATE tIP SET uAvailable=1"
								" WHERE uIP=%u and uAvailable=0",uOfflineIPv4);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						//Now we can remove properties
						DelProperties(uOfflineContainer,3);
						//Remove from any groups
						sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer=%u",uOfflineContainer);//Ok to remove
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",uOfflineContainer);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					}
					else
					{
						mysql_free_result(res);
						tContainer("<blink>Error:</blink> cHostname or cLabel already used"
							" in this unxsVZ system!");
					}
				}
				mysql_free_result(res);

                        	guMode=0;
				//Optional change group.
				if(uGroup)
					UpdatePrimaryContainerGroup(uContainer,uGroup);

				sprintf(gcQuery,"UPDATE tContainer SET cLabel='%s',cHostname='%s'"
						" WHERE uContainer=%u",cWizLabel,cWizHostname,uContainer);
        			mysql_query(&gMysql,gcQuery);
			        if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				sprintf(cLabel,"%.31s",cWizLabel);
				char cPrevHostname[100]={""};
				sprintf(cPrevHostname,"%.99s",cHostname);
				sprintf(cHostname,"%.99s",cWizHostname);
				if(uCreateDNSJob)
				{
					if(uUsePublicIP && gcUseThisIP[0])
						CreateDNSJob(uIPv4,uOwner,gcUseThisIP,cHostname,uDatacenter,guLoginClient);
					else
						CreateDNSJob(uIPv4,uOwner,NULL,cHostname,uDatacenter,guLoginClient);
				}
				if(HostnameContainerJob(uDatacenter,uNode,uContainer,cPrevHostname,uOwner,guLoginClient))
				{
					uStatus=uAWAITHOST;
					SetContainerStatus(uContainer,61);
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("HostnameContainerJob() Done");
				}
				else
				{
					tContainer("<blink>Error:</blink> No jobs created!");
				}
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}//Confirm Hostname Change 
                else if(!strcmp(gcCommand,"IP Change Wizard"))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
				guMode=6001;
				tContainer("Select new IPv4");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm IP Change"))
                {
                        ProcesstContainerVars(entries,x);
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
                        	guMode=6001;

				if(uWizIPv4 && uWizContainer)
					tContainer("<blink>Error:</blink> You must select either a swap or a new IP not both!");
				if(!uWizIPv4 && !uWizContainer)
					tContainer("<blink>Error:</blink> You must select an IP or a container to swap with!");
				if(uContainer==uWizContainer)
					tContainer("<blink>Error:</blink> You can't swap with the same container!");
				if(uWizIPv4)
				{
					sscanf(ForeignKey("tIP","uDatacenter",uWizIPv4),"%u",&uIPv4Datacenter);
					if(uDatacenter!=uIPv4Datacenter)
						tContainer("<blink>Error:</blink> The specified target uIPv4 does not "
							"belong to the specified uDatacenter.");
				}
				else if(uWizContainer)
				{
					if(uVeth)
						tContainer("<blink>Error:</blink> VETH device network containers"
									" not supported at this time!");
					sscanf(ForeignKey("tContainer","uDatacenter",uWizContainer),"%u",&uIPv4Datacenter);
					if(uDatacenter!=uIPv4Datacenter)
						tContainer("<blink>Error:</blink> The specified swap container does not "
							"belong to the same datacenter.");
					//we reuse uWizIPv4 for swap method
					uIPv4Datacenter=0;
					sscanf(ForeignKey("tContainer","uIPv4",uWizContainer),"%u",&uWizIPv4);
					sscanf(ForeignKey("tIP","uDatacenter",uWizIPv4),"%u",&uIPv4Datacenter);
					if(uDatacenter!=uIPv4Datacenter)
						tContainer("<blink>Error:</blink> The specified target uIPv4 does not "
							"belong to the specified uDatacenter.");
				}

				//tContainer("This function is not available yet!");

				unsigned uOldIPv4=0;
				char cIPOld[32]={""};

				if(!uWizContainer)
				{
					unsigned uHostnameLen=0;
					unsigned uAvailable=0;

					//Basic direct IP change
					sscanf(ForeignKey("tIP","uAvailable",uWizIPv4),"%u",&uAvailable);
					if(!uAvailable)
						tContainer("<blink>Error:</blink> The uIPv4 selected is not available anymore!");
					//DNS sanity check
					if(uCreateDNSJob)
					{
						cunxsBindARecordJobZone[0]=0;
						GetConfiguration("cunxsBindARecordJobZone",cunxsBindARecordJobZone,uDatacenter,0,0,0);
						if(!cunxsBindARecordJobZone[0])
							tContainer("<blink>Error:</blink> Create job for unxsBind,"
									" but no cunxsBindARecordJobZone");
					
						uHostnameLen=strlen(cHostname);
						if(!strstr(cHostname+(uHostnameLen-strlen(cunxsBindARecordJobZone)-1),
							cunxsBindARecordJobZone))
							tContainer("<blink>Error:</blink> cHostname must end with cunxsBindARecordJobZone");
					}
					guMode=0;
					//Fatal error section
					sscanf(ForeignKey("tContainer","uIPv4",uContainer),"%u",&uOldIPv4);
					if(!uOldIPv4)
						htmlPlainTextError("Unexpected !uIPv4 and !uOldIPv4");
					sprintf(cIPOld,"%.31s",ForeignKey("tIP","cLabel",uOldIPv4));
					if(!cIPOld[0])
						htmlPlainTextError("Unexpected !cIPOld");
	
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
					//Optional change group.
					if(uGroup)
						UpdatePrimaryContainerGroup(uContainer,uGroup);
					uIPv4=uWizIPv4;
					if(IPContainerJob(uDatacenter,uNode,uContainer,uOwner,guLoginClient,cIPOld))
					{
						uStatus=uAWAITIP;
						SetContainerStatus(uContainer,71);
						sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
						if(uCreateDNSJob)
							CreateDNSJob(uIPv4,uOwner,cuWizIPv4PullDown,cHostname,uDatacenter,guLoginClient);
						tContainer("IPContainerJob() Done");
					}
					else
					{
						tContainer("<blink>Error:</blink> No jobs created!");
					}
				}
				else
				{
					//Swap IP with selected uWizContainer
					unsigned uSwapNode=0;

					//Modify tContainer
					//Optional: Create two DNS
					//Optional: Change group for both containers. Study this.
					//Create two IP Container jobs

					//Return to main container tab
					guMode=0;

					//Two cases swap same node, swap different nodes
					sscanf(ForeignKey("tContainer","uNode",uWizContainer),"%u",&uSwapNode);
					if(uNode!=uSwapNode)
					{

						//Fatal error section loaded container
						sscanf(ForeignKey("tContainer","uIPv4",uContainer),"%u",&uOldIPv4);
						if(!uOldIPv4)
							htmlPlainTextError("Unexpected !uIPv4 and !uOldIPv4");
						sprintf(cIPOld,"%.31s",ForeignKey("tIP","cLabel",uOldIPv4));
						if(!cIPOld[0])
							htmlPlainTextError("Unexpected !cIPOld");
						sprintf(cuWizIPv4PullDown,"%.31s",ForeignKey("tIP","cLabel",uWizIPv4));
						if(!cuWizIPv4PullDown[0])
							htmlPlainTextError("Unexpected !cuWizIPv4PullDown");

						//New uIPv4
						sprintf(gcQuery,"UPDATE tContainer SET uIPv4=%u"
								" WHERE uContainer=%u",uWizIPv4,uContainer);
						mysql_query(&gMysql,gcQuery);
					        if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						//Optional change group.
						if(uGroup)
							UpdatePrimaryContainerGroup(uContainer,uGroup);


						if(IPContainerJob(uDatacenter,uNode,uContainer,uOwner,guLoginClient,cIPOld))
						{
							uStatus=uAWAITIP;
							SetContainerStatus(uContainer,71);
							sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
							if(uCreateDNSJob)
								CreateDNSJob(uWizIPv4,uOwner,cuWizIPv4PullDown,
									cHostname,uDatacenter,guLoginClient);
						}
						else
						{
							tContainer("<blink>Error:</blink> No jobs created!");
						}
	
						//Fatal error section swap container
						sscanf(ForeignKey("tContainer","uIPv4",uWizContainer),"%u",&uOldIPv4);
						if(!uOldIPv4)
							htmlPlainTextError("Unexpected !uIPv4 and !uOldIPv4");
						sprintf(cIPOld,"%.31s",ForeignKey("tIP","cLabel",uOldIPv4));
						if(!cIPOld[0])
							htmlPlainTextError("Unexpected !cIPOld");
						sprintf(cuWizIPv4PullDown,"%.31s",ForeignKey("tIP","cLabel",uIPv4));
						if(!cuWizIPv4PullDown[0])
							htmlPlainTextError("Unexpected !cuWizIPv4PullDown");
	
		
						//New uIPv4 for swap container
						sprintf(gcQuery,"UPDATE tContainer SET uIPv4=%u"
								" WHERE uContainer=%u",uIPv4,uWizContainer);
						mysql_query(&gMysql,gcQuery);
					        if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						//Optional change group.
						if(uGroup)
							UpdatePrimaryContainerGroup(uWizContainer,uGroup);
						uIPv4=uWizIPv4;
						if(IPContainerJob(uDatacenter,uSwapNode,uWizContainer,uOwner,guLoginClient,cIPOld))
						{
							uStatus=uAWAITIP;
							SetContainerStatus(uWizContainer,71);
							if(uCreateDNSJob)
							{
								CreateDNSJob(uIPv4,uOwner,cuWizIPv4PullDown,
										ForeignKey("tContainer","cHostname",uWizContainer),
											uDatacenter,guLoginClient);
							}
							tContainer("IPContainerJob() Done.");
						}
						else
						{
							tContainer("<blink>Error:</blink> No swap container jobs created! Check tJob.");
						}
	
					}
					else
					{
						unsigned uWizOldIPv4=0;
						char cWizIPOld[32]={""};

						//Fatal error section
						//Return to main container tab
						guMode=0;

						sscanf(ForeignKey("tContainer","uIPv4",uContainer),"%u",&uOldIPv4);
						if(!uOldIPv4)
							htmlPlainTextError("Unexpected !uOldIPv4");
						sprintf(cIPOld,"%.31s",ForeignKey("tIP","cLabel",uOldIPv4));
						if(!cIPOld[0])
							htmlPlainTextError("Unexpected !cIPOld");
						sscanf(ForeignKey("tContainer","uIPv4",uWizContainer),"%u",&uWizOldIPv4);
						if(!uWizOldIPv4)
							htmlPlainTextError("Unexpected !uWizOldIPv4");
						sprintf(cWizIPOld,"%.31s",ForeignKey("tIP","cLabel",uWizOldIPv4));
						if(!cWizIPOld[0])
							htmlPlainTextError("Unexpected !cWizIPOld");
						sprintf(cuWizIPv4PullDown,"%.31s",ForeignKey("tIP","cLabel",uWizIPv4));
						if(!cuWizIPv4PullDown[0])
							htmlPlainTextError("Unexpected !cuWizIPv4PullDown");

						//tContainer(cIPOld);
						//tContainer(cWizIPOld);
						//tContainer(cuWizIPv4PullDown);

						//Swap IPs in tContainer
						sprintf(gcQuery,"UPDATE tContainer SET uIPv4=%u"
								" WHERE uContainer=%u",uWizIPv4,uContainer);
						mysql_query(&gMysql,gcQuery);
					        if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						sprintf(gcQuery,"UPDATE tContainer SET uIPv4=%u"
								" WHERE uContainer=%u",uIPv4,uWizContainer);
						mysql_query(&gMysql,gcQuery);
					        if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						//Optional change group.
						if(uGroup)
						{
							UpdatePrimaryContainerGroup(uContainer,uGroup);
							UpdatePrimaryContainerGroup(uWizContainer,uGroup);
						}

						if(IPSameNodeContainerJob(uDatacenter,uNode,uContainer,uWizContainer,
											uOwner,guLoginClient,cIPOld,cWizIPOld))
						{
							//Update displayed data
							uStatus=uAWAITIP;
							sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);

							SetContainerStatus(uContainer,uAWAITIP);
							SetContainerStatus(uWizContainer,uAWAITIP);
							if(uCreateDNSJob)
							{
								CreateDNSJob(uWizIPv4,uOwner,cuWizIPv4PullDown,
									cHostname,uDatacenter,guLoginClient);
								CreateDNSJob(uIPv4,uOwner,
										ForeignKey("tIP","cLabel",uIPv4),
										ForeignKey("tContainer","cHostname",uWizContainer),
											uDatacenter,guLoginClient);
							}
							tContainer("SwapIPContainer job created");
						}
						else
						{
							tContainer("<blink>Error:</blink> No jobs created!");
						}
					}
				}
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strncmp(gcCommand,"Switchover",10))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
				if(!uDatacenter)
					tContainer("<blink>Error:</blink> Unexpected lack of uDatacenter.");
				if(!uNode)
					tContainer("<blink>Error:</blink> Unexpected lack of uNode.");
				if(!uSource)
					tContainer("<blink>Error:</blink> Unexpected lack of uSource.");
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uContainer=%u AND"
						" (uStatus=1 OR uStatus=31 OR uStatus=41)",uSource);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)==0)
					tContainer("<blink>Error:</blink> Unexpected source container status!");
				guMode=8001;
				tContainer("Review and confirm");
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strncmp(gcCommand,"Confirm Switchover",18))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
				unsigned uDebug=0;
                		if(strstr(gcCommand+18,"Debug")) uDebug=1;

                        	guMode=0;
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
                        	guMode=8001;
				if(!uSource)
					tContainer("<blink>Error:</blink> Unexpected lack of uSource!");
				if(!uDatacenter)
					tContainer("<blink>Error:</blink> Unexpected lack of uDatacenter.");
				if(!uNode)
					tContainer("<blink>Error:</blink> Unexpected lack of uNode.");
				if(!cLabel[0])
					tContainer("<blink>Error:</blink> Unexpected lack of cLabel.");
				if(!cHostname[0])
					tContainer("<blink>Error:</blink> Unexpected lack of cHostname.");
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uContainer=%u AND"
						" (uStatus=1 OR uStatus=31 OR uStatus=41)",uSource);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)==0)
					tContainer("<blink>Error:</blink> Unexpected source container status!");
                        	guMode=0;

				unsigned uFailToJob=0;
				if((uFailToJob=FailoverToJob(uDatacenter,uNode,uContainer,uOwner,guLoginClient,uDebug)))
				{
					unsigned uSourceDatacenter=0;
					unsigned uSourceNode=0;

					sscanf(ForeignKey("tContainer","uDatacenter",uSource),"%u",&uSourceDatacenter);
					sscanf(ForeignKey("tContainer","uNode",uSource),"%u",&uSourceNode);

//These two jobs are always done in pairs. Even though the second may run much later
//for example after hardware failure has been fixed.
					if(FailoverFromJob(uSourceDatacenter,uSourceNode,uSource,uIPv4,
							cLabel,cHostname,uContainer,uStatus,uFailToJob,uOwner,guLoginClient,uDebug))
					{
						if(!uDebug)
						{
							uStatus=uAWAITFAIL;
							SetContainerStatus(uContainer,uAWAITFAIL);
							SetContainerStatus(uSource,uAWAITFAIL);
							sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
							//Make sure group is the same as source
							unsigned uGroup=uGetPrimaryContainerGroup(uSource);
							if(uGroup)
								UpdatePrimaryContainerGroup(uContainer,uGroup);
							tContainer("FailoverJob() Done");
						}
						else
						{
							tContainer("Debug FailoverJob() Done");
						}
					}
					else
					{
						tContainer("<blink>Error:</blink> No FailoverFromJob() created!");
					}
				}
				else
				{
					tContainer("<blink>Error:</blink> No FailoverToJob created!");
				}
			}
			else
			{
				tContainer("<blink>Error:</blink> Denied by permissions settings");
			}
		}
                else if(!strncmp(gcCommand,"Set ",4) || !strncmp(gcCommand,"Group ",6) || !strncmp(gcCommand,"Delete Checked",14))
                {
			ProcesstContainerVars(entries,x);
                        guMode=12002;
			tContainer(gcCommand);
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
			printf("Here you will can change the container's IPv4 number."
				" <p>Container services may be affected or need reconfiguration for new IP.\n");
			printf("<p>Select new IPv4<br>");
			tTablePullDownOwnerAvailDatacenter("tIP;cuWizIPv4PullDown","cLabel","cLabel",uWizIPv4,1,
				uDatacenter,uOwner);
			printf("<p>Or swap IPs with this container<br>");
			tTablePullDownDatacenter("tContainer;cuWizContainerPullDown","cLabel","cLabel",uWizContainer,1,
					cuWizContainerPullDown,0,uDatacenter);
			printf("<p><input title='Create an IP change job for the current container'"
					" type=submit class=lwarnButton"
					" name=gcCommand value='Confirm IP Change'>\n");
			printf("<p>Optional primary group change (if swap changes for both)<br>");
			//uGroup=uGetPrimaryContainerGroup(uContainer);//0=not for node
			tContainerGroupPullDown(uChangeGroup,1);
			GetConfiguration("cunxsBindARecordJobZone",cunxsBindARecordJobZone,uDatacenter,0,0,0);
			if(cunxsBindARecordJobZone[0])
			{
				printf("<p>Create job(s) for unxsBind A record(s)<br>");
				printf("<input type=checkbox name=uCreateDNSJob >");
			}
                break;

                case 5001:
                        printf("<p><u>Hostname Change Wizard</u><br>");
			printf("Here you will change the container label (name) and hostname."
				" The container will not be stopped. Only operations depending on the /etc/hosts file"
				" and the hostname may be affected. It is required that cLabel be the first part"
				" of the cHostname. If an offline container with same target name exists it will be"
				" deleted, to help with multi node system migration. <p>\n");
			printf("<p>New cLabel<br>");
			printf("<input title='Short container name, almost always the first part of cHostname'"
					" type=text name=cWizLabel maxlength=31 value='%s'>\n",cLabel);
			printf("<p>New cHostname<br>");
			printf("<input title='FQDN container hostname, usually a DNS resolvable host.'"
					" type=text name=cWizHostname maxlength=99 value='%.99s'>\n",cHostname);
			printf("<p><input title='Create a hostname change job for the current container'"
					" type=submit class=lwarnButton"
					" name=gcCommand value='Confirm Hostname Change'>\n");
			printf("<p>Optional primary group change<br>");
			uGroup=uGetPrimaryContainerGroup(uContainer);//0=not for node
			tContainerGroupPullDown(uChangeGroup,1);
			GetConfiguration("cunxsBindARecordJobZone",cunxsBindARecordJobZone,uDatacenter,0,0,0);
			if(cunxsBindARecordJobZone[0])
			{
				GetContainerProp(uContainer,"cOrg_PublicIP",gcUseThisIP);
				printf("<p>Create job for unxsBind A record<br>"
						"<input type=checkbox name=uCreateDNSJob ");
				if(gcUseThisIP[0])
				{
					printf(" checked>");
					IPNumber(gcUseThisIP);
					printf("<p>Use tProperty cOrg_PublicIP %s<br>"
						"<input type=checkbox name=uUsePublicIP checked>"
						"<input type=hidden name=gcUseThisIP value='%s'>",gcUseThisIP,gcUseThisIP);
				}
				else
				{
					printf(" >");
				}
			}
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
			printf("<p>tConfig.cLabel<br>");
			printf("<input title='tOSTemplate tail and tConfig label will be set to"
					" the name provided.' type=text name=cConfigLabel maxlength=32>\n");
			printf("<p><input title='Create a template job for the current container'"
					" type=submit class=lwarnButton"
					" name=gcCommand value='Confirm Template'>\n");
                break;

                case 3001:
                        printf("<p><u>Migration Wizard</u><br>");
			printf("Here you will select the hardware node target (must be different from the current one.)"
				" If the selected node is"
				" oversubscribed, not available, or scheduled for maintenance. You will"
				" be informed at the next step\n<p>\n");
			printf("<p>Target node<br>");
			tTablePullDown("tNode;cuTargetNodePullDown","cLabel","cLabel",uTargetNode,1);
			printf("<p>Optional primary group change<br>");
			uGroup=uGetPrimaryContainerGroup(uContainer);//0=not for node
			tContainerGroupPullDown(uChangeGroup,1);
			printf("<p><input title='Create a migration job for the current container'"
					" type=submit class=largeButton"
					" name=gcCommand value='Confirm Migration'>\n");
                break;

                case 10001:
                        printf("<p><u>Remote Migration</u><br>");
			printf("Here you will select the hardware node target (must be on a different datacenter.)"
				" If the selected node is"
				" oversubscribed, not available, or scheduled for maintenance. You will"
				" be informed at the next step\n<p>\n");
			printf("<p>Target node<br>");
			tTablePullDown("tNode;cuTargetNodePullDown","cLabel","cLabel",uTargetNode,1);
			printf("<p>Select new IPv4<br>");
			tTablePullDownAvail("tIP;cuWizIPv4PullDown","cLabel","cLabel",uWizIPv4,1);
			printf("<p>Optional primary group change<br>");
			uGroup=uGetPrimaryContainerGroup(uContainer);//0=not for node
			tContainerGroupPullDown(uChangeGroup,1);
			printf("<p><input title='Create a migration job for the current container'"
					" type=submit class=largeButton"
					" name=gcCommand value='Confirm Remote Migration'>\n");
                break;

                case 7001:
                        printf("<p><u>Clone Wizard</u><br>");
			printf("Here you will select the hardware node target. If the selected node is"
				" oversubscribed, not available, or scheduled for maintenance. You will"
				" be informed at the next step.\n<p>\n"
				"You must also select a uIPv4 for the cloned container, and set the initial"
				" state of the clone."
				" Usually clones should be kept stopped to conserve resources and facilitate rsync."
				" Use the checkbox to change this default behavior."
				" Any mount/umount files of the source container will NOT be used"
				" by the new cloned container. This issue will be left for manual"
				" or automated failover to the cloned container.<p>If you wish to"
				" keep the source and clone container sync'ed you can specify a non zero"
				" value via the 'cuSyncPeriod' entry below.");
			printf("<p>Select target node<br>");
			tTablePullDownDatacenter("tNode;cuTargetNodePullDown","cLabel","cLabel",uTargetNode,1,
				cuTargetNodePullDown,0,uDatacenter);//0 does not use tProperty, uses uDatacenter
			printf("<p>Select new IPv4<br>");
			tTablePullDownOwnerAvailDatacenter("tIP;cuWizIPv4PullDown","cLabel","cLabel",uWizIPv4,1,
				uDatacenter,uOwner);

			printf("<p>Select clone state<br>");
			printf("<input type=radio name=uCloneStop value=%u",WARM_CLONE);
			if(uCloneStop==WARM_CLONE)
				printf(" checked");
			printf("> Stopped/Warm");
			printf("<input type=radio name=uCloneStop value=%u",HOT_CLONE);
			if(uCloneStop==HOT_CLONE)
				printf(" checked");
			printf("> Active/Hot");

			printf("<p>cuSyncPeriod<br>");
			printf("<input title='Keep clone in sync every cuSyncPeriod seconds"
					". You can change this at any time via the property panel.'"
					" type=text size=10 maxlength=7"
					" name=uSyncPeriod value='%u'>\n",uSyncPeriod);
			printf("<p>Optional primary group change<br>");
			uGroup=uGetPrimaryContainerGroup(uContainer);//0=not for node
			tContainerGroupPullDown(uChangeGroup,1);
			if(uGroup)
				printf("<input type=hidden name=uGroup value='%u'>",uGroup);
			printf("<p><input title='Create a clone job for the current container'"
					" type=submit class=largeButton"
					" name=gcCommand value='Confirm Clone'>\n");
                break;

                case 11001:
                        printf("<p><u>Remote Clone Wizard (Step 1/2)</u><br>");
			printf("Here you will select the remote datacenter. If it is oversubscribed or not"
				" configured for use, or otherwise unavailable you will have to select another.");
			printf("<p>Select remote datacenter<br>");
			tTablePullDown("tDatacenter;cuTargetDatacenterPullDown","cLabel","cLabel",uTargetDatacenter,1);
			if(uGroup)
				printf("<input type=hidden name=uGroup value='%u'>",uGroup);
			printf("<p><input title='Step one of remote clone wizard'"
					" type=submit class=largeButton"
					" name=gcCommand value='Select Clone Datacenter'>\n");
                break;

                case 11002:
                        printf("<p><u>Remote Clone Wizard (Step 2/2)</u><br>");
			printf("Here you will select the hardware node target. If the selected node is"
				" oversubscribed, not available, or scheduled for maintenance. You will"
				" be informed at the next step.\n<p>\n"
				"You must also select a uIPv4 for the cloned container, and set the initial"
				" state of the clone."
				" Usually clones should be kept stopped to conserve resources and facilitate rsync."
				" Use the checkbox to change this default behavior."
				" Any mount/umount files of the source container will NOT be used"
				" by the new cloned container. This issue will be left for manual"
				" or automated failover to the cloned container.<p>If you wish to"
				" keep the source and clone container sync'ed you can specify a non zero"
				" value via the 'cuSyncPeriod' entry below.");
			printf("<p>Selected datacenter<br>");
			tTablePullDown("tDatacenter;cuTargetDatacenterPullDown","cLabel","cLabel",uTargetDatacenter,0);
			printf("<p>Select target node<br>");
			tTablePullDownDatacenter("tNode;cuTargetNodePullDown","cLabel","cLabel",uTargetNode,1,
				cuTargetNodePullDown,0,uTargetDatacenter);//0 does not use tProperty, uses uDatacenter
			printf("<p>Select new IPv4<br>");
			tTablePullDownOwnerAvailDatacenter("tIP;cuWizIPv4PullDown","cLabel","cLabel",uWizIPv4,1,
				uTargetDatacenter,uOwner);

			printf("<p>Select clone state<br>");
			printf("<input type=radio name=uCloneStop value=%u",WARM_CLONE);
			if(uCloneStop==WARM_CLONE)
				printf(" checked");
			printf("> Stopped/Warm");
			printf("<input type=radio name=uCloneStop value=%u",COLD_CLONE);
			if(uCloneStop==COLD_CLONE)
				printf(" checked");
			printf("> Initial Setup/Cold");
			printf("<input type=radio name=uCloneStop value=%u",HOT_CLONE);
			if(uCloneStop==HOT_CLONE)
				printf(" checked");
			printf("> Active/Hot");

			printf("<p>cuSyncPeriod<br>");
			printf("<input title='Keep clone in sync every cuSyncPeriod seconds"
					". You can change this at any time via the property panel.'"
					" type=text size=10 maxlength=7"
					" name=uSyncPeriod value='%u'>\n",uSyncPeriod);
			printf("<p>Optional primary group change<br>");
			uGroup=uGetPrimaryContainerGroup(uContainer);//0=not for node
			tContainerGroupPullDown(uChangeGroup,1);
			if(uGroup)
				printf("<input type=hidden name=uGroup value='%u'>",uGroup);
			printf("<p><input title='Create a clone job for the current container'"
					" type=submit class=largeButton"
					" name=gcCommand value='Confirm Remote Clone'>\n");
                break;

                case 8001:
                        printf("<p><u>Switchover Container Pair</u><br>");
			printf("Confirm all the information presented for a manual failover to take place."
				"<p>Jobs will be created for the source and this container. If jobs run successfully,"
				" everything but the container VEIDs will be switched.<p>This clone (renamed to the"
				" source names) will be the new production container and the source container will"
				" be the clone container and kept in sync if cuSyncPeriod is changed to a non 0 value."
				" This should only be done after confirmation that switchover container works fine.");
                        printf("<p><u>Switchover Data</u>");
			if(uSource)
			{
				printf("<br>%s will replace ",cLabel);
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%u>"
					"%s</a>",uSource,ForeignKey("tContainer","cLabel",uSource));
			}
			printf("<p><input title='Creates manual failover jobs for the current container'"
					" type=submit class=lwarnButton"
					" name=gcCommand value='Confirm Switchover'>\n");
                break;

                case 2001:
                        printf("<p><u>Think twice</u>");
                        printf("<br>Container and it's properties will be removed from unxsVZ database for good.<br>Any jobs"
				" pending for this container or it's clone will be canceled. Any clone in <i>Awaiting clone</i>"
				" status, will also be deleted.<p>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review changes</u><br>");
			printf("If you change uIPv4 you will need to modify tIP<br>");
                        printf(LANG_NBB_CONFIRMMOD);
			printf("<p>Optional primary group change<br>");
			uGroup=uGetPrimaryContainerGroup(uContainer);//0=not for node
			tContainerGroupPullDown(uChangeGroup,1);
                break;

                case 12001:
                case 12002:
			printf("<u>Create or refine your user search set</u><br>");
			printf("In the right panel you can select your search criteria. When refining you do not need"
				" to reuse your initial search critieria. Your search set is persistent even across unxsVZ sessions.<p>");
			printf("<input type=submit class=largeButton title='Create an initial or replace an existing search set'"
				" name=gcCommand value='Create Search Set'>");
			printf("<input type=submit class=largeButton title='Add the results to your current search set. Do not add the same search"
				" over and over again it will not result in any change but may slow down processing.'"
				" name=gcCommand value='Add to Search Set'>");
			printf("<p><input type=submit class=largeButton title='Apply the right panel filter to refine your existing search set"
				" by removing set elements that match the filter settings.'"
				" name=gcCommand value='Remove from Search Set'>\n");
			printf("<p><input type=submit class=largeButton title='Reload current search set. Good for checking for any new status updates'"
				" name=gcCommand value='Reload Search Set'>");
			printf("<input type=submit class=largeButton title='Return to main tContainer tab page'"
				" name=gcCommand value='Cancel'>");
			printf("<p><u>Set Operation Options</u>");
			printf("<br>Target node");
			tTablePullDown("tNode;cuTargetNodePullDown","cLabel","cLabel",uTargetNode,1);
			printf(" Clone target node");
			tTablePullDown("tNode;cuCloneTargetNodePullDown","cLabel","cLabel",guCloneTargetNode,1);
			printf("<br>Group");
			tContainerGroupPullDown(uChangeGroup,1);
			printf("<br><input title='For supported set operations (like Group Delete, Destroy or Migration)"
				" apply same to their clone containers.'"
				" type=checkbox name=guOpOnClonesNoCA");
			if(guOpOnClones)
				printf(" checked");
			printf("> guOpOnClones");

                break;

                case 9001:
			printf("<u>New Container Datacenter and Organization</u><br>");
			printf("In conjunction with your master plan; review datacenter location, usage, traffic"
				" and problem stats, in order to make an optimal datacenter selection."
				" You must also chose the organization (a company, NGO or similar) that will own this"
				" resource.<p>");
			printf("<input type=submit class=largeButton title='Select datacenter and organization'"
				" name=gcCommand value='Select Datacenter/Org'>\n");
			printf("<p><input type=submit class=largeButton title='Cancel this operation'"
				" name=gcCommand value='Cancel'>\n");
			//if(!uOwner && uForClient)
			//	printf("<input type=hidden name=uOwner value='%u'>\n",uForClient);
                break;

                case 9002:
			printf("<u>New Container Node</u><br>");
			printf("In conjunction with your master plan; review node usage, traffic"
				" and problem stats, in order to make an optimal node selection. Only nodes that belong"
				" to the previously chosen datacenter are shown. Other factors may also limit the "
				"available node list, such as autonomic or configuration imposed restrictions.<p>");
			printf("<input type=submit class=largeButton title='Select hardware node'"
				" name=gcCommand value='Select Node'>\n");
			printf("<p><input type=submit class=largeButton title='Cancel this operation'"
				" name=gcCommand value='Cancel'>\n");
                break;

                case 9003:
			printf("<u>New Container Setup</u><br>");
			printf("Set container creation parameters. Choices are limited based on selected datacenter, node,"
				" and the organization that the container is being created for. We now require that the cLabel"
				" be the first part (first stop) of the cHostname (for multiple container creation this is automatic.)<p>");
			printf("The appliance container is a place holder container for an actual container (or non virtual server)"
				" than runs on a remote node (or appliance) not part of this unxsVZ system. The clone is kept sync'd,"
				" and it's uSource is, as usual, the uContainer of the container created."
				" This new type of container is identified, for now, via a special uStatus.");
			GetConfiguration("cAutoCloneNode",cAutoCloneNode,uDatacenter,uNode,0,0);
			if(cAutoCloneNode[0])
				printf("<p>Auto-clone subsystem is enabled for selected datacenter/node: Clone target node"
					" must not match selected node. Similarly, clone start uIPv4"
					" must not match uIPv4 or fall in same range -as defined per number of containers.");
			GetConfiguration("cunxsBindARecordJobZone",cunxsBindARecordJobZone,uDatacenter,0,0,0);
			if(cunxsBindARecordJobZone[0])
				printf("<p>unxsBind interface is configured for selected datacenter and <i>%s</i> zone: DNS will be setup"
					" automatically for you, unless you opt-out by un-checking the <i>Create job...</i>"
					" checkbox in the right data panel.",cunxsBindARecordJobZone);
			printf("<p><input type=submit class=largeButton"
				" title='Configure container and create a single container'"
				" name=gcCommand value='Single Container Creation'>\n");
			printf("<p><input type=submit class=largeButton"
				" title='Configure base container and continue to create multiple containers'"
				" name=gcCommand value='Multiple Container Creation'>\n");
			printf("<p><input type=submit class=largeButton"
				" title='Configure and create special remote appliance container set'"
				" name=gcCommand value='Appliance Creation'>\n");
			printf("<br><input type=text"
				" title='Appliance creation requires a valid IPv4 IP number be entered."
				" This IP is the IP of the remote appliance it may already exist in tIP"
				" but belong to special CustomerPremise datacenter'"
				" name=gcIPv4 value='%s'> Appliance gcIPv4\n",gcIPv4);
			printf("<p><input type=submit class=largeButton title='Cancel this operation'"
				" name=gcCommand value='Cancel'>\n");
                break;

                case 9004:
			printf("<u>Multiple Container Setup</u><br>");
			printf("Set special multiple container creation parameters. Choices are limited based on selected"
				" datacenter, node,"
				" and the organization that the container is being created for. We recommend that the cLabel"
				" be the short cHostname, in DNS terms.<p>");
			printf("When you create multiple containers you specify a special <i>cLabel</i>"
				" and <i>cHostname</i> that will be used to generate the multiple containers."
				" <p>For example, given <i>cLabel=ct</i> and <i>cHostname=.yourdomain.tld</i>,"
				" your containers will be <i>cLabel=ct0</i> and <i>cHostname=ct0.yourdomain.tld</i>"
				" through <i>cLabel=ctN</i> and <i>cHostname=ctN.yourdomain.tld</i>,"
				" where N is the number of containers specfied minus 1.<p>");
			GetConfiguration("cAutoCloneNode",cAutoCloneNode,uDatacenter,uNode,0,0);
			if(cAutoCloneNode[0])
				printf("Auto-clone subsystem is enabled for selected datacenter: Clone target node"
					" must not match selected node. Similarly, clone start uIPv4"
					" must not match uIPv4 or fall in same range -as defined per number of containers.<p>");
			GetConfiguration("cunxsBindARecordJobZone",cunxsBindARecordJobZone,uDatacenter,0,0,0);
			if(cunxsBindARecordJobZone[0])
				printf("unxsBind interface is configured for selected datacenter and <i>%s</i> zone: DNS will be setup"
					" automatically for you, unless you opt-out by un-checking the <i>Create job...</i>"
					" checkbox in the right data panel.<p>",cunxsBindARecordJobZone);
			printf("<input type=submit class=largeButton"
				" title='Commit to creating multiple containers'"
				" name=gcCommand value='Create Multiple Containers'>\n");
			printf("<p><input type=submit class=largeButton title='Cancel this operation'"
				" name=gcCommand value='Cancel'>\n");
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
		
			//Some buttons should be easier to get to than others.
			if(uContainer)
			{
				if( uStatus==uINITSETUP)
				{
					printf("<p><input title='Creates a job for deploying this new container."
					" Make sure you configure the properties you want beforehand!'"
					" type=submit class=lalertButton"
					" name=gcCommand value='Deploy %.25s'>\n",cLabel);
				}
				else if( uStatus==uAWAITACT || uStatus==uAWAITDEL ||
							uStatus==uAWAITSTOP || uStatus==uAWAITFAIL)
				{
					printf("<p><input title='Cancel all waiting jobs for this container.'"
					" type=submit class=largeButton"
					" name=gcCommand value='Cancel Job'>\n");
				}
			}

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
			printf("<p><u>Container Search by cLabel</u><br>");
			printf("<input title='Enter cLabel start or MySQL LIKE pattern (%% or _ allowed)' type=text"
					" name=cSearch value='%s'>",cSearch);

			printf("<input title='Remove clones from search nav list' type=checkbox name=guNoClones");
			if(guNoClones)
				printf(" checked");
			printf("> guNoClones");

			printf("<input title='Remove all but initial setup status containers from nav list."
				" Mutually exclusive with guNoClones option.' type=checkbox name=guInitOnly");
			if(guInitOnly)
				printf(" checked");
			printf("> guInitOnly");

			tContainerNavList(0,cSearch);//0 means container mode or tContainer tab mode.
			printf("<p><input type=submit class=largeButton title='Open user search set page. There you can create search sets and operate"
				" on selected containers of the loaded container set.'"
				" name=gcCommand value='Search Set Operations'>\n");
			if(uContainer && uAllowMod(uOwner,uCreatedBy) && guPermLevel>7)
			{
				if(uStatus==uACTIVE)
				{
					htmlHealth(uContainer,3);
					if(!strstr(cLabel,"-clone") && uSource==0)
					printf("<p><input title='Clone a container to this or another hardware node."
					" The clone will be an online container with another IP and hostname."
					" It will be kept updated via rsync on a configurable basis.'"
					" type=submit class=largeButton"
					" name=gcCommand value='Clone Wizard'><br>\n");
					printf("<input title='Clone container to a remote datacenter hardware node."
					" The clone will be an online container with another IP and hostname."
					" It will be kept updated via rsync on a configurable basis.'"
					" type=submit class=largeButton"
					" name=gcCommand value='Remote Clone Wizard'><br>\n");
					printf("<input title='Change current container IPv4'"
					" type=submit class=largeButton"
					" name=gcCommand value='IP Change Wizard'><br>\n");
					printf("<p><input title='Creates a job for stopping an active container.'"
						" type=submit class=lwarnButton"
						" name=gcCommand value='Stop %.24s'><br>\n",cLabel);
					if(uSource)
						printf("<p><input title='Creates jobs for manual failover.'"
						" type=submit class=lwarnButton"
						" name=gcCommand value='Switchover %.25s'><br>\n",cLabel);
				}
				else if( uStatus==uSTOPPED)
				{
					printf("<p><input title='Creates a job for starting a stopped container.'"
					" type=submit class=lalertButton"
					" name=gcCommand value='Start %.25s'><br>\n",cLabel);
					if(!strstr(cLabel,"-clone") && uSource==0)
					printf("<input title='Clone a container to this or another hardware node."
					" The clone will be an online container with another IP and hostname."
					" It will be kept updated via rsync on a configurable basis.'"
					" type=submit class=largeButton"
					" name=gcCommand value='Clone Wizard'><br>\n");
					printf("<input title='Clone container to a remote datacenter hardware node."
					" The clone will be an online container with another IP and hostname."
					" It will be kept updated via rsync on a configurable basis.'"
					" type=submit class=largeButton"
					" name=gcCommand value='Remote Clone Wizard'><br>\n");
					if(uSource)
						printf("<p><input title='Creates jobs for manual failover.'"
						" type=submit class=lwarnButton"
						" name=gcCommand value='Switchover %.25s'><br>\n",cLabel);
				}

				if( uStatus==uSTOPPED || uStatus==uACTIVE )
				{
					printf("<input title='Migrate container to another hardware node'"
					" type=submit class=largeButton"
					" name=gcCommand value='Migration Wizard'><br>\n");
					printf("<input title='Migrate container to another datacenter node'"
					" type=submit class=largeButton"
					" name=gcCommand value='Remote Migration'><br>\n");
					printf("<input title='Change current container name and hostname'"
					" type=submit class=largeButton"
					" name=gcCommand value='Hostname Change Wizard'><br>\n");
					printf("<input title='Template a container."
					" Creates and installs OS and VZ conf templates on all nodes.'"
					" type=submit class=largeButton"
					" name=gcCommand value='Template Wizard'><br>\n");
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
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uGroup=0;
	unsigned uNumRows=0;

	switch(guMode)
	{
		case 12001:
		case 12002:
			//Set operation buttons
			OpenFieldSet("Set Operations",100);
			printf("<input title='Delete checked containers from your search set. They will still be visible but will"
				" marked deleted and will not be used in any subsequent set operation'"
				" type=submit class=largeButton name=gcCommand value='Delete Checked'>\n");
			printf("&nbsp; <input title='Cancels job(s) for container(s) waiting for activation, deletion or stop.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Cancel'>\n");
			printf("&nbsp; <input title='Creates job(s) for starting stopped or initial setup container(s).'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Start'>\n");
			printf("&nbsp; <input title='Creates job(s) for templating stopped or active container(s)."
				" Uses the container label for new tOSTemplate and tConfig entries."
				" If active, containers may be stopped for several minutes if not setup for vzdump snapshot.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Template'>\n");
			printf("&nbsp; <input title='Deletes initial setup or awaiting intial setup clone container(s) and optionally their clones.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Delete'>\n");
			printf("&nbsp; <input title='Deletes any existing container group association then adds selected group to selected containers."
					" Requires that you select the new group with the group select in top left panel.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Replace All'>\n");
			printf("&nbsp; <input title='Adds the selected containers to a group."
					" Requires that you select the new group with the group select in top left panel.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Add Group'>\n");
			printf("&nbsp; <input title='Deletes the selected containers from a group."
					" Requires that you select the group with the group select in top left panel.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Del Group'>\n");
			printf("&nbsp; <input title='Replaces the primary group (first group as per uGroupGlue) for the selected containers with a new group."
					" If you need the replaced primary group as a secondary group you must add it."
					" If the selected containers are not associated with any group the selected group is added and is the primary."
					" Requires that you select the group with the group select in top left panel.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Replace Primary'>\n");

			printf("<p><input title='Creates job(s) for cloning active or stopped container(s) that"
				" are not clones themselves. Must configure tConfiguration AutoCloneNode, cAutoCloneSyncTime and cAutoCloneIPClass'"
				" type=submit class=lwarnButton"
				" name=gcCommand value='Group Clone'>\n");
			printf("&nbsp; <input title='Creates job(s) for stopping active container(s).'"
				" type=submit class=lwarnButton"
				" name=gcCommand value='Group Stop'>\n");
			printf("&nbsp; <input title='Creates job(s) for switching over cloned container(s) of same datacenter.'"
				" type=submit class=lwarnButton"
				" name=gcCommand value='Group Switchover'>\n");
			printf("&nbsp; <input title='Creates job(s) for migrating container(s) and optionally their clones."
				" Uses target and clone node selects above'"
				" type=submit class=lwarnButton"
				" name=gcCommand value='Group Migration'>\n");
			printf("&nbsp; <input title='Creates job(s) for destroying active or stopped container(s) and optionally their clones.'"
				" type=submit class=lwarnButton"
				" name=gcCommand value='Group Destroy'>\n");
			printf("&nbsp; <input title='Creates job(s) with given commands to run via vzctl exec2'"
				" type=submit class=lwarnButton"
				" name=gcCommand value='Group Execute'>\n");
			CloseFieldSet();

			sprintf(gcQuery,"Search Set Contents");
			OpenFieldSet(gcQuery,100);
			uGroup=uGetSearchGroup(gcUser,2);
			sprintf(gcQuery,"SELECT"
					" tC1.uContainer,tC1.cLabel,tC1.cHostname,tStatus.cLabel,"
					" tIP.cLabel,tNode.cLabel,tDatacenter.cLabel,tC1.uSource,"
					" tClient.cLabel,tOSTemplate.cLabel,"
					" FROM_UNIXTIME(tC1.uCreatedDate,'%%a %%b %%d %%T %%Y')"
					" FROM tContainer AS tC1"
					" LEFT JOIN tIP ON tC1.uIPv4=tIP.uIP"
					" LEFT JOIN tNode ON tC1.uNode=tNode.uNode"
					" LEFT JOIN tDatacenter ON tC1.uDatacenter=tDatacenter.uDatacenter"
					" LEFT JOIN tStatus ON tC1.uStatus=tStatus.uStatus"
					" LEFT JOIN tClient ON tC1.uOwner=tClient.uClient"
					" LEFT JOIN tOSTemplate ON tC1.uOSTemplate=tOSTemplate.uOSTemplate"
					" WHERE tC1.uContainer IN (SELECT uContainer FROM tGroupGlue WHERE uGroup=%u)",uGroup);
		        mysql_query(&gMysql,gcQuery);
		        if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		        res=mysql_store_result(&gMysql);
			if((uNumRows=mysql_num_rows(res)))
			{
				char cResult[100]={""};
				char cCtLabel[100]={""};
				unsigned uSourceContainer=0;
				unsigned uSourceNode=0;
				char cSourceNode[32]={""};
				char cSource[32]={""};

				printf("<table>");
				printf("<tr>");
				printf("<td valign=top><input type=checkbox name=all onClick='checkAll(document.formMain,this)'></td>"
					"<td valign=top><u>cLabel</u></td>"
					"<td valign=top><u>cHostname</u></td>"
					"<td valign=top><u>Status</u></td>"
					"<td valign=top><u>IPv4</u></td>"
					"<td valign=top><u>Node</u></td>"
					"<td valign=top><u>Datacenter</u></td>"
					"<td valign=top><u>Source</u></td>"
					"<td valign=top><u>SNode</u></td>"
					"<td valign=top><u>Owner</u></td>"
					"<td valign=top><u>OSTemplate</u></td>"
					"<td valign=top><u>uCreatedDate</u></td>"
					"<td valign=top><u>Groups</u></td>"
					"<td valign=top><u>Set operation result</u></td></tr>");
//Reset margin start
while((field=mysql_fetch_row(res)))
{
	if(guMode==12002)
	{
		register int i;
		unsigned uCtContainer=0;

		cResult[0]=0;
		sscanf(field[0],"%u",&uCtContainer);
		sprintf(cCtLabel,"Ct%u",uCtContainer);
		for(i=0;i<x;i++)
		{
			//insider xss protection
			if(guPermLevel<10)
				continue;

			if(!strcmp(entries[i].name,cCtLabel))
			{
				if(!strcmp(gcCommand,"Delete Checked"))
				{
					sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroup=%u AND uContainer=%u",uGroup,uCtContainer);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						sprintf(cResult,mysql_error(&gMysql));
						break;
					}
					if(mysql_affected_rows(&gMysql)>0)
						sprintf(cResult,"Deleted from set");
					else
						sprintf(cResult,"Unexpected non deletion");
					break;
				}//Delete Checked

				else if(!strcmp(gcCommand,"Group Cancel"))
				{
					struct structContainer sContainer;

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);

					if((sContainer.uStatus==uAWAITDEL || sContainer.uStatus==uAWAITACT ||
						sContainer.uStatus==uAWAITSTOP)
						&& (sContainer.uOwner==guCompany || guCompany==1))
					{
						if(!CancelContainerJob(sContainer.uDatacenter,
								sContainer.uNode,uCtContainer,1))
						{
							if(sContainer.uStatus==uAWAITDEL || 
									sContainer.uStatus==uAWAITSTOP)
								SetContainerStatus(uCtContainer,uACTIVE);
							else if(sContainer.uStatus==uAWAITACT)
								SetContainerStatus(uCtContainer,uINITSETUP);
							sprintf(cResult,"group cancel job created");
						}
						else
						{
							sprintf(cResult,"group cancel job not created!");
						}
					}
					else
					{
						sprintf(cResult,"group cancel request ignored");
					}
					break;
				}//Group Cancel

				else if(!strcmp(gcCommand,"Group Stop"))
				{
					struct structContainer sContainer;

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);
					if( (sContainer.uStatus==uACTIVE)
						&& (sContainer.uOwner==guCompany || guCompany==1))
					{
						if(StopContainerJob(sContainer.uDatacenter,
								sContainer.uNode,uCtContainer,guCompany))
						{
							SetContainerStatus(uCtContainer,uAWAITSTOP);
							sprintf(cResult,"group stop job created");
						}
						else
						{
							sprintf(cResult,"group stop job not created!");
						}
					}
					else
					{
						sprintf(cResult,"group stop request ignored");
					}
					break;
				}//Group Stop

				//Group Destroy Uses guOpOnClones
				else if(!strcmp(gcCommand,"Group Destroy"))
				{
					struct structContainer sContainer;

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);
					if( (sContainer.uStatus==uSTOPPED || sContainer.uStatus==uACTIVE)
						&& (sContainer.uOwner==guCompany || guCompany==1))
					{
						if(DestroyContainerJob(sContainer.uDatacenter,
								sContainer.uNode,uCtContainer,guCompany))
						{
							SetContainerStatus(uCtContainer,uAWAITDEL);
							sprintf(cResult,"group destroy job created");
						}
						else
						{
							sprintf(cResult,"group destroy job not created!");
						}
					}
					else
					{
						sprintf(cResult,"group destroy request ignored");
					}
					if(guOpOnClones)
					{
       						MYSQL_RES *res;
					        MYSQL_ROW field;
						unsigned uContainer=0;
						unsigned uNode=0;
						unsigned uDatacenter=0;
						unsigned uStatus=0;
						unsigned uOwner=0;

						sprintf(gcQuery,"SELECT uDatacenter,uNode,uContainer,uStatus,uOwner FROM tContainer"
								" WHERE uSource=%u",uCtContainer);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					        res=mysql_store_result(&gMysql);
						while((field=mysql_fetch_row(res)))
						{
							sscanf(field[0],"%u",&uDatacenter);
							sscanf(field[1],"%u",&uNode);
							sscanf(field[2],"%u",&uContainer);
							sscanf(field[3],"%u",&uStatus);
							sscanf(field[4],"%u",&uOwner);
							if((uStatus==uSTOPPED || uStatus==uACTIVE)
								&& (uOwner==guCompany || guCompany==1))
							{
								if(DestroyContainerJob(uDatacenter,uNode,uContainer,guCompany))
								{
									SetContainerStatus(uContainer,uAWAITDEL);
									strcat(cResult," +clone job");
								}
								else
								{
									strcat(cResult," +clone job error!");
								}
							}
							else
							{
								strcat(cResult," +no clone job");
							}
						}
						mysql_free_result(res);
					}//op on clones
					break;
				}//Group Destroy

				//Requires uGroup
				else if(!strcmp(gcCommand,"Group Replace All"))
				{
					struct structContainer sContainer;

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);
					if( (sContainer.uOwner==guCompany || guCompany==1) && uChangeGroup)
					{
						//Remove from all container type groups
						sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer=%u AND"
								" uGroup IN (SELECT uGroup FROM tGroup WHERE uGroupType=1)",//1=container type 
							uCtContainer);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						sprintf(gcQuery,"INSERT INTO tGroupGlue SET uContainer=%u,uGroup=%u",
							uCtContainer,uChangeGroup);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						sprintf(cResult,"group changed");
					}
					else
					{
						sprintf(cResult,"group change ignored");
					}
					break;
				}//Group Replace All

				//Requires uGroup
				else if(!strcmp(gcCommand,"Group Replace Primary"))
				{
					struct structContainer sContainer;

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);
					if( (sContainer.uOwner==guCompany || guCompany==1) && uChangeGroup)
					{
						if(!UpdatePrimaryContainerGroup(uCtContainer,uChangeGroup))
							sprintf(cResult,"primary group changed");
					}
					else
					{
						sprintf(cResult,"group change ignored");
					}
					break;
				}//Group Replace Primary

				//Requires uGroup
				else if(!strcmp(gcCommand,"Group Add Group"))
				{
					struct structContainer sContainer;

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);
					if( (sContainer.uOwner==guCompany || guCompany==1) && uChangeGroup)
					{
						sprintf(gcQuery,"INSERT INTO tGroupGlue SET uContainer=%u,uGroup=%u",
							uCtContainer,uChangeGroup);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						sprintf(cResult,"group added");
					}
					else
					{
						sprintf(cResult,"group change ignored");
					}
					break;
				}//Group Add Group

				//Requires uGroup
				else if(!strcmp(gcCommand,"Group Del Group"))
				{
					struct structContainer sContainer;

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);
					if( (sContainer.uOwner==guCompany || guCompany==1) && uChangeGroup)
					{
						sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer=%u AND uGroup=%u",
							uCtContainer,uChangeGroup);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						sprintf(cResult,"group deleted");
					}
					else
					{
						sprintf(cResult,"group change ignored");
					}
					break;
				}//Group Del Group

				//Group Delete Uses guOpOnClones
				else if(!strcmp(gcCommand,"Group Delete"))
				{
					struct structContainer sContainer;

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);
					if( (sContainer.uStatus==uINITSETUP || sContainer.uStatus==uAWAITCLONE)
						&& (sContainer.uOwner==guCompany || guCompany==1))
					{
						//Release IPs
						sprintf(gcQuery,"UPDATE tIP SET uAvailable=1"
							" WHERE uIP=%u and uAvailable=0",sContainer.uIPv4);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						//Node IP if any MySQL5+
						sprintf(gcQuery,"UPDATE tIP SET uAvailable=1 WHERE cLabel IN"
						" (SELECT cValue FROM tProperty WHERE uKey=%u"
						" AND uType=3 AND cName='cNodeIP')",uCtContainer);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						//Now we can remove properties
						DelProperties(uCtContainer,3);
						//Remove from any groups
						sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer=%u",
							uCtContainer);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						CancelContainerJob(sContainer.uDatacenter,sContainer.uNode,
							uCtContainer,0);//0 is not specific cancel job attempt
						//81=Awaiting clone. TODO properties of clones may
						//exist
						sprintf(gcQuery,"DELETE FROM tContainer WHERE uStatus=81 AND uSource=%u"
									,uCtContainer);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer="
						"(SELECT uContainer FROM tContainer WHERE uSource=%u AND uStatus=81)",uContainer);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",
							uCtContainer);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						if(mysql_affected_rows(&gMysql)>0)
							sprintf(cResult,"group delete done");
					}
					else
					{
						sprintf(cResult,"group delete ignored");
					}
					if(guOpOnClones)
					{
       						MYSQL_RES *res;
					        MYSQL_ROW field;
						unsigned uContainer=0;
						unsigned uNode=0;
						unsigned uDatacenter=0;
						unsigned uStatus=0;
						unsigned uOwner=0;
						unsigned uIPv4=0;

						sprintf(gcQuery,"SELECT uDatacenter,uNode,uContainer,uStatus,uOwner,uIPv4 FROM"
								" tContainer WHERE uSource=%u",uCtContainer);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					        res=mysql_store_result(&gMysql);
						while((field=mysql_fetch_row(res)))
						{
							sscanf(field[0],"%u",&uDatacenter);
							sscanf(field[1],"%u",&uNode);
							sscanf(field[2],"%u",&uContainer);
							sscanf(field[3],"%u",&uStatus);
							sscanf(field[4],"%u",&uOwner);
							sscanf(field[5],"%u",&uIPv4);
							if((uStatus==uINITSETUP) && (uOwner==guCompany || guCompany==1))
							{
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
								sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer=%u",
									uContainer);
								mysql_query(&gMysql,gcQuery);
								if(mysql_errno(&gMysql))
									htmlPlainTextError(mysql_error(&gMysql));
								//0 is not specific cancel job attempt
								CancelContainerJob(uDatacenter,uNode,uContainer,0);
								//81=Awaiting clone
								sprintf(gcQuery,"DELETE FROM tContainer WHERE"
										" uStatus=81 AND uSource=%u",uContainer);
								mysql_query(&gMysql,gcQuery);
								if(mysql_errno(&gMysql))
									htmlPlainTextError(mysql_error(&gMysql));
								sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE"
										" uContainer=(SELECT uContainer FROM tContainer"
										" WHERE uSource=%u AND uStatus=81)",uContainer);
								mysql_query(&gMysql,gcQuery);
								if(mysql_errno(&gMysql))
									htmlPlainTextError(mysql_error(&gMysql));
								sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",
									uContainer);
								mysql_query(&gMysql,gcQuery);
								if(mysql_errno(&gMysql))
									htmlPlainTextError(mysql_error(&gMysql));
								if(mysql_affected_rows(&gMysql)>0)
									strcat(cResult," +clone delete");
							}//if clone
							else
							{
								strcat(cResult," +no clone deleted");
							}
						}//while
						mysql_free_result(res);
					}//if op on clones
					break;
				}//Group Delete

				//Clone selected containers
				if(!strcmp(gcCommand,"Group Clone"))
				{
					struct structContainer sContainer;
					static unsigned uOnlyOnce=1;
					char cConfBuffer[256]={""};
					char cAutoCloneIPClass[256]={""};

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);
					if( (sContainer.uStatus==uACTIVE || sContainer.uStatus==uSTOPPED)
						&& (sContainer.uOwner==guCompany || guCompany==1))
					{
						//We can only fo this if tConfiguration has been setup
						//with datacenter wide cAutoCloneNode=node1,cAutoCloneSyncTime=600
						//for example.
						uTargetNode=0;
						GetConfiguration("cAutoCloneNode",cConfBuffer,sContainer.uDatacenter,sContainer.uNode,0,0);
						if(cConfBuffer[0])
							uTargetNode=ReadPullDown("tNode","cLabel",cConfBuffer);
						//We need the cuSyncPeriod
						uSyncPeriod=3600;//default 1 hour rsync
						GetConfiguration("cAutoCloneSyncTime",cConfBuffer,sContainer.uDatacenter,sContainer.uNode,0,0);
						if(cConfBuffer[0])
							sscanf(cConfBuffer,"%u",&uSyncPeriod);
						cAutoCloneIPClass[0]=0;
						GetConfiguration("cAutoCloneIPClass",cAutoCloneIPClass,sContainer.uDatacenter,sContainer.uNode,0,0);
						uOnlyOnce=0;

						//Some validation
						if(!cAutoCloneIPClass[0])
						{
							sprintf(cResult,"No cAutoCloneIPClass!");
							break;
						}
						if(!uTargetNode)
						{
							sprintf(cResult,"No uTargetNode!");
							break;
						}

						//Basic conditions
						//We do not allow clones of clones yet.
						if(uTargetNode && !sContainer.uSource && cAutoCloneIPClass[0])
						{
							unsigned uNewVeid=0;
							MYSQL_RES *res;
							MYSQL_ROW field;

							//Get next available IP, set uIPv4
							uWizIPv4=0;
							//TODO we cant let root just grab anybodys IPs
							if(guCompany==1)
								sprintf(gcQuery,"SELECT uIP FROM tIP WHERE"
								" uAvailable=1 AND cLabel LIKE '%s.%%' AND uDatacenter=%u LIMIT 1",
									cAutoCloneIPClass,sContainer.uDatacenter);
							else
								sprintf(gcQuery,"SELECT uIP FROM tIP WHERE"
									" uAvailable=1 AND cLabel LIKE '%s.%%' AND uDatacenter=%u AND"
								" uOwner=%u LIMIT 1",cAutoCloneIPClass,sContainer.uDatacenter,guCompany);
							mysql_query(&gMysql,gcQuery);
							if(mysql_errno(&gMysql))
								htmlPlainTextError(mysql_error(&gMysql));
							res=mysql_store_result(&gMysql);
							if((field=mysql_fetch_row(res)))
								sscanf(field[0],"%u",&uWizIPv4);
							mysql_free_result(res);

							//
							//We impose further abort conditions

							//Need valid clone IP
							if(!uWizIPv4)
							{
								sprintf(cResult,"Need valid clone IP");
								break;
							}
							if(uWizIPv4==sContainer.uIPv4)
							{
								sprintf(cResult,"Invalid clone IP");
								break;
							}

							//Target node can't match source node.
							if(uTargetNode==sContainer.uNode)
							{
								sprintf(cResult,"Target node can't match source node");
								break;
							}

							//Check for sane sync periods
							if(!uSyncPeriod && uSyncPeriod<300 && uSyncPeriod>86400*30)
							{
								sprintf(cResult,"Insane sync periods");
								break;
							}

							//If the container is VETH then target node must support it.
							if(sContainer.uVeth)
							{
								GetNodeProp(uNode,"Container-Type",cConfBuffer);
								if(!strstr(cConfBuffer,"VETH"))
								{
									sprintf(cResult,"Non VETH target node");
									break;
								}
							}

							//We require group now.
							//Get group from source container
							uGroup=uGetPrimaryContainerGroup(uCtContainer);
							if(!uGroup)
							{
								sprintf(cResult,"We require group");
								break;
							}

							//Finally we can create the clone container
							//and the clone job
							gcReturnMsg[0]=0;
							uNewVeid=CommonCloneContainer(
									uCtContainer,
									sContainer.uOSTemplate,
									sContainer.uConfig,
									sContainer.uNameserver,
									sContainer.uSearchdomain,
									sContainer.uDatacenter,
									sContainer.uDatacenter,
									sContainer.uOwner,
									sContainer.cLabel,
									sContainer.uNode,
									sContainer.uStatus,
									sContainer.cHostname,
									"",
									uWizIPv4,
									cWizLabel,
									cWizHostname,
									uTargetNode,
									uSyncPeriod,
									guLoginClient,
									uCloneStop,7);//mode==7 sprintf gcReturnMsg error messages
							//Now that container exists we can assign group.
							if(!uNewVeid)
							{
								if(gcReturnMsg[0])
									sprintf(cResult,gcReturnMsg);
								else
									sprintf(cResult,"uNewVeid error");
								break;
							}
							UpdatePrimaryContainerGroup(uNewVeid,uGroup);
							SetContainerStatus(uCtContainer,uAWAITCLONE);
							SetContainerStatus(uNewVeid,uAWAITCLONE);
							sprintf(cResult,"Clone job created");
						}
						else
						{
							sprintf(cResult,"Check target node, uStatus and cAutoCloneIPClass");
						}
					}
					else
					{
							sprintf(cResult,"Clone job not created");
					}
					break;
				}//Group Clone

				//Start or Create
				else if(!strcmp(gcCommand,"Group Start"))
				{
					struct structContainer sContainer;

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);

					if((sContainer.uStatus==uSTOPPED || sContainer.uStatus==uINITSETUP)
						&& (sContainer.uOwner==guCompany || guCompany==1))
					{
						if(sContainer.uStatus==uINITSETUP)
						{
							if(CreateNewContainerJob(sContainer.uDatacenter,
								sContainer.uNode,uCtContainer,sContainer.uOwner))
							{
								SetContainerStatus(uCtContainer,uAWAITACT);
								sprintf(cResult,"Start job created for init setup container");
							}
						}
						else
						{
							uOwner=guCompany;
							if(CreateStartContainerJob(sContainer.uDatacenter,
								sContainer.uNode,uCtContainer,sContainer.uOwner))
							{
								SetContainerStatus(uCtContainer,uAWAITACT);
								sprintf(cResult,"Start job created for stopped container");
							}
						}
					}
					else
					{
						sprintf(cResult,"Start job not created");
					}
					break;
				}//Group Start

				//execute any commands in container(s)
				else if(!strcmp(gcCommand,"Group Execute"))
				{
					struct structContainer sContainer;

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);

					if(	sContainer.uStatus==uACTIVE
						&& (sContainer.uOwner==guCompany || guCompany==1) && cCommands[0])
					{

						if(strlen(cCommands)>2047)
						{
							sprintf(cResult,"Script too long");
							break;
						}
						if(CreateExecuteCommandsJob(sContainer.uDatacenter,
								sContainer.uNode,uCtContainer,sContainer.uOwner,cCommands))
							sprintf(cResult,"Execute job created");
					}
					else
					{
						sprintf(cResult,"Execute job not created");
					}
					break;
				}//Group Execute

				else if(!strcmp(gcCommand,"Group Template"))
				{
					struct structContainer sContainer;

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);

					if((sContainer.uStatus==uSTOPPED || sContainer.uStatus==uACTIVE)
						&& (sContainer.uOwner==guCompany || guCompany==1))
					{
						MYSQL_RES *res;
						char cOSTLabel[101]={""};
						char cConfigLabel[33]={""};

						//Here we set the template to use the source container short name label
						sprintf(cConfigLabel,"%.31s",ForeignKey("tContainer","cLabel",uCtContainer));
						sprintf(cOSTLabel,"%.100s",ForeignKey("tOSTemplate","cLabel",sContainer.uOSTemplate));
						if(!cOSTLabel[0])
						{
							sprintf(cResult,"No tOSTemplate.cLabel");
							break;
						}
						if(!cConfigLabel[0])
						{
							sprintf(cResult,"No tContainer.cLabel");
							break;
						}
						sprintf(gcQuery,"SELECT uConfig FROM tConfig WHERE cLabel='%s'",cConfigLabel);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
		        			res=mysql_store_result(&gMysql);
						if(mysql_num_rows(res)>0)
						{
							sprintf(cResult,"tConfig.cLabel collision");
							break;
						}
						mysql_free_result(res);
						sprintf(gcQuery,"SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='%.67s-%.32s'",
								cOSTLabel,cConfigLabel);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
		        			res=mysql_store_result(&gMysql);
						if(mysql_num_rows(res)>0)
						{
							sprintf(cResult,"tOSTemplate.cLabel collision");
							break;
						}

						if(TemplateContainerJob(sContainer.uDatacenter,
							sContainer.uNode,uCtContainer,sContainer.uStatus,
							sContainer.uOwner,cConfigLabel))
						{
							SetContainerStatus(uCtContainer,uAWAITTML);
							sprintf(cResult,"Template job created");
						}
						else
						{
							sprintf(cResult,"Template job creation error");
						}
					}
					else
					{
						sprintf(cResult,"Template job ignored");
					}
					break;
				}//Group Template

				else if(!strcmp(gcCommand,"Group Switchover"))
				{
				//These two jobs are always done in pairs. Even though the second may run much later
				//for example after hardware failure has been fixed.
					struct structContainer sContainer;

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);
					if( sContainer.uSource!=0 &&
						 (sContainer.uStatus==uSTOPPED || sContainer.uStatus==uACTIVE)
						&& (sContainer.uOwner==guCompany || guCompany==1))
					{
						unsigned uFailToJob=0;
						if((uFailToJob=FailoverToJob(sContainer.uDatacenter,sContainer.uNode,
							uCtContainer,sContainer.uOwner,guLoginClient,0)))
						{
							unsigned uSourceDatacenter=0;
							unsigned uSourceNode=0;
							sscanf(ForeignKey("tContainer","uDatacenter",
								sContainer.uSource),
								"%u",&uSourceDatacenter);
							sscanf(ForeignKey("tContainer","uNode",
								sContainer.uSource),
								"%u",&uSourceNode);

							if(FailoverFromJob(uSourceDatacenter,uSourceNode,
								sContainer.uSource,sContainer.uIPv4,
								sContainer.cLabel,sContainer.cHostname,
									uCtContainer,sContainer.uStatus,
									uFailToJob,sContainer.uOwner,guLoginClient,0))
							{
								SetContainerStatus(uCtContainer,uAWAITFAIL);
								SetContainerStatus(sContainer.uSource,uAWAITFAIL);
								sprintf(cResult,"Switchover job created");
							}
							else
							{
								sprintf(cResult,"FailoverFromJob() failed");
							}
						}
						else
						{
							sprintf(cResult,"FailoverToJob() failed");
						}
					}
					else
					{
						sprintf(cResult,"Switchover job ignored");
					}
					break;
				}//Group Switchover

				//Group Migration Uses guOpOnClones, uTargetNode and guCloneTargetNode
				else if(!strcmp(gcCommand,"Group Migration"))
				{
				//This is a new type of group job that requires a uTargetNode and
				//optionally a guCloneTargetNode that must be set in the left panel
					struct structContainer sContainer;

					//debug
					//sprintf(gcQuery,"d1: target node %u clone target node %u",uTargetNode,guCloneTargetNode);
					//tContainer(gcQuery);
					if(uTargetNode==guCloneTargetNode)
					{
						sprintf(gcQuery,"<blink>Error</blink> target node (%u) is the same as clone target node!",
							uTargetNode);
						tContainer(gcQuery);
					}
					//More validation
					//Get most specific cAutoCloneNode
					unsigned uTargetDatacenter=0;
					sscanf(ForeignKey("tNode","uDatacenter",uTargetNode),"%u",&uTargetDatacenter);
					if(!uTargetDatacenter)
					{
						sprintf(gcQuery,"<blink>Error</blink> target node (%u) has no"
								" datacenter configured",uTargetNode);
						tContainer(gcQuery);
					}
					if(guCloneTargetNode && guOpOnClones)
					{
						cAutoCloneNode[0]=0;
						GetConfiguration("cAutoCloneNode",cAutoCloneNode,uTargetDatacenter,uTargetNode,0,0);
						if(!cAutoCloneNode[0])
							GetConfiguration("cAutoCloneNode",cAutoCloneNode,uTargetDatacenter,0,0,0);
						if(!cAutoCloneNode[0])
						{
							sprintf(gcQuery,"<blink>Error</blink> target node (%u) has no"
									" tConfiguration cAutoCloneNode configured",uTargetNode);
							tContainer(gcQuery);
						}
						unsigned uCloneTargetNode=0;
						uCloneTargetNode=ReadPullDown("tNode","cLabel",cAutoCloneNode);
						if(!uCloneTargetNode)
						{
							sprintf(gcQuery,"<blink>Error</blink> cAutoCloneNode %s has no"
									" tNode entry!",cAutoCloneNode);
							tContainer(gcQuery);
						}
						if(uCloneTargetNode!=guCloneTargetNode)
						{
							sprintf(gcQuery,"<blink>Error</blink> Target node auto clone target %s (%u)"
									" does not match provided"
									" guCloneTargetNode (%u)!",
										cAutoCloneNode,uCloneTargetNode,guCloneTargetNode);
							tContainer(gcQuery);
						}
					}

					InitContainerProps(&sContainer);
					GetContainerProps(uCtContainer,&sContainer);
					if( uTargetNode && (sContainer.uStatus==uSTOPPED || sContainer.uStatus==uACTIVE)
						&& (sContainer.uOwner==guCompany || guCompany==1))
					{

						if(MigrateContainerJob(sContainer.uDatacenter,
									sContainer.uNode,uCtContainer,uTargetNode,
									sContainer.uOwner,guLoginClient,0,sContainer.uStatus))
						{
							SetContainerStatus(uCtContainer,uAWAITMIG);
							sprintf(cResult,"Migration job created");

							if(guCloneTargetNode && guOpOnClones)
							{
								unsigned uCloneDatacenter=0;
								unsigned uCloneNode=0;
								unsigned uCloneContainer=0;
								unsigned uCloneOwner=0;
								unsigned uCloneStatus=0;
								MYSQL_RES *res;
								MYSQL_ROW field;

								sprintf(gcQuery,"SELECT uDatacenter,uNode,uContainer,uStatus,uOwner FROM tContainer"
									" WHERE uSource=%u",uCtContainer);
								mysql_query(&gMysql,gcQuery);
								if(mysql_errno(&gMysql))
									htmlPlainTextError(mysql_error(&gMysql));
							        res=mysql_store_result(&gMysql);
								while((field=mysql_fetch_row(res)))
								{
									sscanf(field[0],"%u",&uCloneDatacenter);
									sscanf(field[1],"%u",&uCloneNode);
									sscanf(field[2],"%u",&uCloneContainer);
									sscanf(field[3],"%u",&uCloneStatus);
									sscanf(field[4],"%u",&uCloneOwner);
									if((uCloneStatus==uSTOPPED || uCloneStatus==uACTIVE)
										&& (uCloneOwner==guCompany || guCompany==1)
										//Do not create useless job if clone is already on
										//target node
										&& (guCloneTargetNode!=uCloneNode) )
									{
										if(MigrateContainerJob(uCloneDatacenter,
											uCloneNode,uCloneContainer,guCloneTargetNode,
											uCloneOwner,guLoginClient,0,uCloneStatus))
										{
											SetContainerStatus(uCloneContainer,uAWAITMIG);
											strcat(cResult," +clone job");

										}
										else
										{
											strcat(cResult," +clone job error");
										}
									}
									else
									{
										strcat(cResult," +clone job ignored");
									}
								}
							}
						}
						else
						{
							sprintf(cResult,"Migration job failed");
						}
					}
					else
					{
						sprintf(cResult,"Migration job ignored");
					}
					break;
				}//Group Migration

				else if(1)
				{
					sprintf(cResult,"Unexpected gcCommand=%.64s",gcCommand);
					break;
				}
			}//end if Ct block
		}//end for()
	}


	cSource[0]=0;
	cSourceNode[0]=0;
	if(field[7][0]!='0')
	{
		uSourceContainer=0;
		uSourceNode=0;
		sscanf(field[7],"%u",&uSourceContainer);
		if(uSourceContainer)
		{
			sprintf(cSource,"%.31s",ForeignKey("tContainer","cLabel",uSourceContainer));
			sscanf(ForeignKey2("tContainer","uNode",uSourceContainer),"%u",&uSourceNode);
			if(uSourceNode)
				sprintf(cSourceNode,"%.31s",ForeignKey2("tNode","cLabel",uSourceNode));
		}
	}
	printf("<tr>");
	printf(
		"<td valign=top><input type=checkbox name=Ct%s ></td>"
		"<td valign=top><a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s</a></td>"
		"<td valign=top>%s</td>"
		"<td valign=top>%s</td>"
		"<td valign=top>%s</td>"
		"<td valign=top>%s</td>"
		"<td valign=top>%s</td>"
		"<td valign=top><a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%u>%s</a></td>"
		"<td valign=top>%s</td>"
		"<td valign=top>%s</td>"
		"<td valign=top>%s</td>"
		"<td valign=top>%s</td>"
		"<td valign=top>%s</td>"
		"<td valign=top>%s</td>\n",
			field[0],
			field[0],field[1],
			field[2],
			field[3],
			field[4],
			field[5],
			field[6],
			uSourceContainer,cSource,
			cSourceNode,
			field[8],
			field[9],
			field[10],
			cHtmlGroups(field[0]),
			cResult);
	printf("</tr>");

}//while()
//Reset margin end

			printf("<tr><td valign=top><input type=checkbox name=all onClick='checkAll(document.formMain,this)'></td>"
					"<td valign=top colspan=3>Check all %u containers</td></tr>\n",uNumRows);
			printf("</table>");

			}//If results
			mysql_free_result(res);
			CloseFieldSet();
		break;

		default:

			if(uHideProps || !uContainer || (guMode!=0 && guMode!=6)) return;

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

	}//switch(guMode)

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
		else if(!strcmp(gentries[i].name,"uGroup"))
		{
			sscanf(gentries[i].val,"%u",&uGroup);
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
        else if(!strcmp(gcFilter,"uGroup"))
        {
		unsigned uGroup=0;

                sscanf(gcCommand,"%u",&uGroup);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tContainer.uContainer IN (SELECT uContainer FROM tGroupGlue WHERE uGroup=%u) ORDER BY uOwner,uContainer",uGroup);
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
        if(strcmp(gcFilter,"uGroup"))
                printf("<option>uGroup</option>");
        else
                printf("<option selected>uGroup</option>");
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
	if(guMode<9000)
	{
		printf(LANG_NBB_SKIPFIRST);
		printf(LANG_NBB_SKIPBACK);
		printf(LANG_NBB_SEARCH);
	}

	if(guPermLevel>7 && !guListMode && guMode<9000)
		printf(LANG_NBB_NEW);

	//11 Initial setup 31 Stopped
	if( (uStatus==uINITSETUP) && uAllowMod(uOwner,uCreatedBy) && guMode<9000)
		printf(LANG_NBB_MODIFY);

	if( (uStatus==uINITSETUP && uAllowDel(uOwner,uCreatedBy) && guMode<9000) ||
		(uStatus==uOFFLINE && guPermLevel>11) ||
		(uStatus==uREMOTEAPPLIANCE && guPermLevel>11) ||
		(uStatus==uAWAITCLONE && guPermLevel>11) )
		printf(LANG_NBB_DELETE);

	if(uOwner && guMode<9000)
		printf(LANG_NBB_LIST);

	if(guMode<9000)
	{
		printf(LANG_NBB_SKIPNEXT);
		printf(LANG_NBB_SKIPLAST);
	}
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttContainerNavBar(void)

//uNode=0 means container mode or tContainer tab mode.
//uNode!=0 is used for a nav list of containers from same node.
void tContainerNavList(unsigned uNode, char *cSearch)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uNumRows=0;
	unsigned uMySQLNumRows=0;
#define uLIMIT 64

	if(!uNode)
	{
		if(!cSearch[0])
			return;

	    if(guNoClones)
	    {
			if(cSearch[0])
			{
				if(guLoginClient==1 && guPermLevel>11)//Root can read access all
					sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
						"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus"
						" WHERE tContainer.uNode=tNode.uNode"
						" AND tContainer.uStatus=tStatus.uStatus"
						" AND tContainer.uSource=0"
						" AND tContainer.cLabel LIKE '%s%%'"
						" ORDER BY tContainer.cLabel",cSearch);
				else
					sprintf(gcQuery,"SELECT tContainer.uContainer"
						",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
						" FROM tContainer," TCLIENT ",tNode,tStatus"
						" WHERE tContainer.uOwner=" TCLIENT ".uClient"
						" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
						" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
						" AND tContainer.uNode=tNode.uNode"
						" AND tContainer.uStatus=tStatus.uStatus"
						" AND tContainer.uSource=0"
						" AND tContainer.cLabel LIKE '%2$s%%'"
						" ORDER BY tContainer.cLabel",guCompany,cSearch);
			}
	   }
	   else if(guInitOnly)
	   {
		if(cSearch[0])
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uStatus=11"
					" AND tContainer.cLabel LIKE '%s%%'"
					" ORDER BY tContainer.cLabel",cSearch);
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uStatus=11"
					" AND tContainer.cLabel LIKE '%2$s%%'"
					" ORDER BY tContainer.cLabel",guCompany,cSearch);
		}
	   }
	   else //guNoClones==0 and guInitOnly==0
	   {
			if(cSearch[0])
			{
				if(guLoginClient==1 && guPermLevel>11)//Root can read access all
					sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
						"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus"
						" WHERE tContainer.uNode=tNode.uNode"
						" AND tContainer.uStatus=tStatus.uStatus"
						" AND tContainer.cLabel LIKE '%s%%'"
						" ORDER BY tContainer.cLabel",cSearch);
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
						" ORDER BY tContainer.cLabel",guCompany,cSearch);
			}
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
					" AND tContainer.uNode=%u ORDER BY tContainer.cLabel",cSearch,uNode);
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
					" ORDER BY tContainer.cLabel",uNode,guCompany,cSearch);
		}
		else
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel"
					" FROM tContainer,tNode,tStatus"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
						" AND tContainer.uNode=%u ORDER BY tContainer.cLabel",uNode);
			else
				sprintf(gcQuery,"SELECT DISTINCT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND tContainer.uNode=%1$u"
					" AND (" TCLIENT ".uClient=%2$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" ORDER BY tContainer.cLabel",uNode,guCompany);
		}
		//debug only
		//printf("%s",gcQuery);
                //return;

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

	        while((field=mysql_fetch_row(res)))
		{
			if(guPermLevel>9 && uNode==0)
			{
				if(cSearch[0] && uGroup)
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction="
					"tContainer&uContainer=%s&cSearch=%s&uGroup=%u>%s/%s/%s</a><br>\n",
						field[0],cURLEncode(cSearch),uGroup,field[1],field[2],field[3]);
				else if(cSearch[0])
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction="
					"tContainer&uContainer=%s&cSearch=%s>%s/%s/%s</a><br>\n",
						field[0],cURLEncode(cSearch),field[1],field[2],field[3]);
				else if(uGroup)
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction="
					"tContainer&uContainer=%s&uGroup=%u>%s/%s/%s</a><br>\n",
						field[0],uGroup,field[1],field[2],field[3]);
				else
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction="
					"tContainer&uContainer=%s>%s/%s/%s</a><br>\n",
						field[0],field[1],field[2],field[3]);
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
				printf("(Only %u containers shown use search/filters to shorten list.)<br>\n",
						uLIMIT);
				break;
			}
		}
	}
        mysql_free_result(res);

}//void tContainerNavList()


unsigned CreateNewContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uOwner)
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


unsigned CreateStartContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uOwner)
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


unsigned DestroyContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer, unsigned uOwner)
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

}//unsigned DestroyContainerJob()


unsigned StopContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer, unsigned uOwner)
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


//Keeps job container from staying stuck in an awaiting state,
//but is confusing in logs since the function below should be
//extended for this case.
unsigned CancelContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uCancelMode)
{
	unsigned uContainerJobsCanceled;
	unsigned uContainerCloneJobsCanceled;

	//Cancel any jobs for uContainer 1=Waiting 10=RemoteWaiting, 14=Error
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=7 WHERE "
			"uDatacenter=%u AND uNode=%u AND uContainer=%u AND (uJobStatus=1 OR uJobStatus=10 OR uJobStatus=14)"
				,uDatacenter,uNode,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uContainerJobsCanceled=mysql_affected_rows(&gMysql);

	//Also cancel any jobs for uContainer's clones 1=Waiting 10=RemoteWaiting, 14=Error
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=7 WHERE"
			" uContainer=(SELECT uContainer FROM tContainer WHERE uSource=%u) AND"
			" (uJobStatus=1 OR uJobStatus=10 OR uJobStatus=14)",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uContainerCloneJobsCanceled=mysql_affected_rows(&gMysql);

	//Also cancel master container clone jobs
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=7 WHERE"
			" uContainer=(SELECT uSource FROM tContainer WHERE uContainer=%u) AND"
			" (uJobStatus=1 OR uJobStatus=10 OR uJobStatus=14) AND cJobName='CloneContainer'",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	//This convoluted logic is needed for specifically attempting to cancel container jobs uCancelMode=1
	//versus general job queue clean up attempts.
	if(uContainerJobsCanceled || uContainerCloneJobsCanceled)
	{
		unxsVZLog(uContainer,"tContainer","CancelJob");
		if(uCancelMode && !uContainerJobsCanceled)
			return(1);
		else
			return(0);
	}
	else if(uCancelMode)
	{
		unxsVZLog(uContainer,"tContainer","CancelJobLate");
		return(1);
	}

	return(0);

}//unsigned CancelContainerJob()


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


void SetContainerDatacenter(unsigned uContainer,unsigned uDatacenter)
{
	sprintf(gcQuery,"UPDATE tContainer SET uDatacenter=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uContainer=%u",
					uDatacenter,guLoginClient,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void SetContainerDatacenter(unsigned uContainer,unsigned uDatacenter)


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
				" AND tGroupGlue.uGroup=tGroup.uGroup ORDER BY tGroupGlue.uGroupGlue",uNode);
	else
	sprintf(gcQuery,"SELECT tGroup.uGroup,tGroup.cLabel FROM tGroupGlue,tGroup WHERE tGroupGlue.uContainer=%u"
				" AND tGroupGlue.uGroup=tGroup.uGroup ORDER BY tGroupGlue.uGroupGlue",uContainer);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	unsigned uFirst=1;
	while((field=mysql_fetch_row(res)))
	{
		if(uNode)
			printf("<br>Node");
		else
			printf("<br>Container");
		printf(" is member of the <a class=darkLink");
		printf(" href=unxsVZ.cgi?gcFunction=tGroup&uGroup=%s>%s</a> ",field[0],field[1]);
		if(uFirst--) printf("(primary) ");
		printf("group\n");
	}
	mysql_free_result(res);

}//void htmlGroups()


char *cHtmlGroups(char const *cuContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	static char cReturn[386]={""};
	char cLine[128]={""};

	sprintf(gcQuery,"SELECT tGroup.uGroup,tGroup.cLabel FROM tGroupGlue,tGroup WHERE tGroupGlue.uContainer=%s"
				" AND tGroupGlue.uGroup=tGroup.uGroup AND tGroup.uGroupType=1 ORDER BY tGroupGlue.uGroupGlue LIMIT 3",
					cuContainer);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	cReturn[0]=0;
	while((field=mysql_fetch_row(res)))
	{
		sprintf(cLine,"<a class=darkLink href=unxsVZ.cgi?gcFunction=tGroup&uGroup=%.9s>%.31s</a><br>",field[0],field[1]);
		strcat(cReturn,cLine);
	}
	mysql_free_result(res);
	return(cReturn);

}//char *cHtmlGroups()


unsigned MigrateContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer, unsigned uTargetNode,
			unsigned uOwner, unsigned uLoginClient, unsigned uIPv4,unsigned uPrevStatus)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='MigrateContainerJob(%u-->%u)',cJobName='MigrateContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",cJobData='uTargetNode=%u;\nuIPv4=%u;\nuPrevStatus=%u;\n'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,uTargetNode,
				uDatacenter,uNode,uContainer,
				uTargetNode,
				uIPv4,
				uPrevStatus,
				uOwner,uLoginClient);
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


unsigned TemplateContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uStatus,unsigned uOwner,char *cConfigLabel)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='TemplateContainerJob(%u)',cJobName='TemplateContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",cJobData='tConfig.Label=%.31s;\n"
			"uPrevStatus=%u;\n'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				cConfigLabel,
				uStatus,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","Template");
	return(uCount);

}//unsigned TemplateContainerJob()


unsigned HostnameContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,char *cPrevHostname,unsigned uOwner,unsigned uLoginClient)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='ChangeHostnameContainer(%u)',cJobName='ChangeHostnameContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",cJobData='cPrevHostname=%s;'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				cPrevHostname,
				uOwner,uLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","Hostname");
	return(uCount);

}//unsigned HostnameContainerJob()


unsigned IPContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,
			unsigned uOwner,unsigned uLoginClient,char const *cIPOld)
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
				uOwner,uLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","ChangeIP");
	return(uCount);

}//unsigned IPContainerJob()


unsigned IPSameNodeContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer1,unsigned uContainer2,
			unsigned uOwner,unsigned uLoginClient,char const *cIPOld1,char const *cIPOld2)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='SwapIPContainer(%u)',cJobName='SwapIPContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",cJobData='uContainer2=%u;\ncIPOld1=%.31s;\ncIPOld2=%.31s;\n'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer1,uContainer2,cIPOld1,cIPOld2,
				uOwner,uLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","SwapIPContainer");
	return(uCount);

}//unsigned IPSameNodeContainerJob()


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
				unsigned uTargetNode, unsigned uNewVeid, unsigned uPrevStatus,
				unsigned uOwner,unsigned uCreatedBy,unsigned uCloneStop)
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
				uOwner,uCreatedBy);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","CloneContainer");
	return(uCount);

}//unsigned CloneContainerJob()




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
unsigned FailoverToJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uOwner,unsigned uLoginClient,unsigned uDebug)
{
	unsigned uCount=0;

	if(!uDatacenter || !uNode || !uContainer) return(0);

	//This job should try to stop the uSource container if possible. Or at least
	//make sure it's IP is down. There are many failover scenarios it seems at first glance
	//we will try to handle all of them.
	//Remote datacenter failover seems to involve DNS changes. Since we can't use the VIP
	//method that should be available in a correctly configured unxsVZ datacenter.
	if(uDebug)
		sprintf(gcQuery,"INSERT INTO tJob SET cLabel='FailoverToJob(%u)',cJobName='FailoverToDEBUG'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())"
			",uJobStatus=1"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uOwner,uLoginClient);
	else
		sprintf(gcQuery,"INSERT INTO tJob SET cLabel='FailoverToJob(%u)',cJobName='FailoverTo'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())"
			",uJobStatus=1"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uOwner,uLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	return(uCount);

}//unsigned FailoverToJob()


unsigned FailoverFromJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,
				unsigned uIPv4,char *cLabel,char *cHostname,unsigned uSource,
				unsigned uStatus,unsigned uFailToJob,unsigned uOwner,unsigned uLoginClient,unsigned uDebug)
{
	unsigned uCount=0;

	if(!uDatacenter || !uNode || !uContainer) return(0);

	//If the source node is down this job will be stuck
	//When node is fixed and boots we must avoid the source continater coming up with same IP
	//as the new production container (the FailoverToJob container.)
	if(uDebug)
		sprintf(gcQuery,"INSERT INTO tJob SET cLabel='FailoverFromJob(%u)',cJobName='FailoverFromDEBUG'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())"
			",uJobStatus=1"
			",cJobData='uIPv4=%u;\ncLabel=%s;\ncHostname=%s;\nuSource=%u;\nuStatus=%u;\n"
			"uFailToJob=%u;'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uIPv4,cLabel,cHostname,uSource,uStatus,uFailToJob,
				uOwner,uLoginClient);
	else
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
				uOwner,uLoginClient);
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

	while((field=mysql_fetch_row(res)))
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


unsigned uGetGroup(unsigned uNode, unsigned uContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uGroup=0;

	if(uNode)
		sprintf(gcQuery,"SELECT MIN(tGroup.uGroup) FROM tGroupGlue,tGroup WHERE tGroupGlue.uNode=%u"
				" AND tGroupGlue.uGroup=tGroup.uGroup AND tGroup.uGroupType=1",uNode);//1 is container type used here temporarily
	else
		sprintf(gcQuery,"SELECT MIN(tGroup.uGroup) FROM tGroupGlue,tGroup WHERE tGroupGlue.uContainer=%u"
				" AND tGroupGlue.uGroup=tGroup.uGroup AND tGroup.uGroupType=1",uContainer);//1 is container type
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(field[0]!=NULL)
			sscanf(field[0],"%u",&uGroup);
	}
	mysql_free_result(res);

	return(uGroup);

}//unsigned uGetGroup(unsigned uNode, unsigned uContainer)


//Pull job for unxsBind
//Sample cJobData
/*
cName=delme.zone.net.;
cuTTL=3600;
cRRType=A;
cParam1=174.121.136.102;
cNSSet=ns1-2.yourdomain.com;
cView=external;
*/
unsigned unxsBindARecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
	unsigned uOwner,unsigned uCreatedBy)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsBindARecordJob(%u)',cJobName='unxsVZContainerARR'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=%u"
			",cJobData='%s'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uREMOTEWAITING,
				cJobData,
				uOwner,uCreatedBy);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","unxsBindARecordJob");
	return(uCount);

}//unsigned unxsBindARecordJob(...)


void ChangeGroupXXX(unsigned uContainer, unsigned uGroup)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	if(uGroup)
	{
		unsigned uPrimaryGroup=0;

		//Get the PK of the primary group of this container.
		//So smallest (oldest/first) uGroup is the primary group? This does not seem correct.
		//The oldest/first uGroupGlue uGroup should be the primary group!
		sprintf(gcQuery,"SELECT tGroupGlue.uGroupGlue,tGroupGlue.uGroup FROM tGroupGlue,tGroup WHERE"
			" tGroupGlue.uGroup=tGroup.uGroup AND tGroupGlue.uContainer=%u"
			" AND tGroup.uGroupType=1 ORDER BY tGroupGlue.uGroup LIMIT 1",uContainer);//1 is container type
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
       		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)==0)
		{
			//If none insert
			sprintf(gcQuery,"INSERT INTO tGroupGlue SET uContainer=%u,uGroup=%u",
					uContainer,uGroup);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		}
		else if ((field=mysql_fetch_row(res)))
		{
			//Conditionally UPDATE save db work.
			sscanf(field[1],"%u",&uPrimaryGroup);
			if(uPrimaryGroup!=uGroup)
			{
				sprintf(gcQuery,"UPDATE tGroupGlue SET uGroup=%u WHERE"
						" uGroupGlue=%s",uGroup,field[0]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
			}
		}
		mysql_free_result(res);
	}

}//void ChangeGroupXXX(unsigned uContainer, unsigned uGroup)


unsigned CommonCloneContainer(
		unsigned uContainer,
		unsigned uOSTemplate,
		unsigned uConfig,
		unsigned uNameserver,
		unsigned uSearchdomain,
		unsigned uDatacenter,
		unsigned uTargetDatacenter,
		unsigned uOwner,
		const char *cLabel,
		unsigned uNode,
		unsigned uStatus,
		const char *cHostname,
		const char *cClassC,
		unsigned uWizIPv4,
		char *cWizLabel,
		char *cWizHostname,
		unsigned uTargetNode,
		unsigned uSyncPeriod,
		unsigned uLoginClient,
		unsigned uCloneStop,
		unsigned uMode)
{	
	MYSQL_RES *res;
	unsigned uNewVeid=0;
	unsigned uWizLabelSuffix=0;
	unsigned uWizLabelLoop=1;

	//Get first available <cLabel>-clone<uNum>
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
	//New clone naming convention
	char *cp=NULL;
	if((cp=strchr(cHostname,'.')))
	{
		*cp=0;
		sprintf(cWizHostname,"%.32s-clone%u.%.60s",cHostname,uWizLabelSuffix,cp+1);
		*cp='.';
	}
	else
	{
		sprintf(cWizHostname,"%.93s.clone%u",cHostname,uWizLabelSuffix);
	}

	sprintf(gcQuery,"INSERT INTO tContainer SET cLabel='%s',"
				"cHostname='%s',"
				"uIPv4=%u,"
				"uOSTemplate=%u,"
				"uConfig=%u,"
				"uNameserver=%u,"
				"uSearchdomain=%u,"
				"uDatacenter=%u,"
				"uNode=%u,"
				"uStatus=%u,"
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
				uTargetDatacenter,
				uTargetNode,
				uAWAITCLONE,
				uOwner,
				uLoginClient,
				uContainer);
       	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uNewVeid=mysql_insert_id(&gMysql);

	if(!uNewVeid)
		return(0);

	if(CloneContainerJob(uDatacenter,uNode,uContainer,uTargetNode,uNewVeid,uStatus,uOwner,uLoginClient,uCloneStop))
	{
		//TODO something is wrong here.
		//If we specify a class C mask then we check it here. Why?
		if(cClassC[0])
		{
			if(guCompany==1)
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
				" WHERE uIP=%u AND uAvailable=1 AND cLabel LIKE '%s%%' AND uDatacenter=%u",
					uWizIPv4,cClassC,uTargetDatacenter);
			else
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
					" WHERE uIP=%u AND uAvailable=1 AND uOwner=%u AND cLabel LIKE '%s%%'"
					" AND uDatacenter=%u",
						uWizIPv4,uOwner,cClassC,uTargetDatacenter);
		}
		//Here no such class c check
		else
		{
			if(guCompany==1)
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
				" WHERE uIP=%u AND uAvailable=1 AND uDatacenter=%u",
					uWizIPv4,uTargetDatacenter);
			else
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
					" WHERE uIP=%u AND uAvailable=1 AND uOwner=%u AND uDatacenter=%u",
						uWizIPv4,uOwner,uTargetDatacenter);
		}
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		if(mysql_affected_rows(&gMysql)!=1)
		{
			sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",uNewVeid);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));

			sprintf(gcQuery,"DELETE FROM tJob WHERE cLabel='CloneContainer(%u)'"
					,uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));

			if(uMode==0)
				tContainer("<blink>Error:</blink> No jobs created. Clone IP gone!");
			else if(uMode==1)
				tNode("<blink>Error:</blink> No jobs created. Clone IP gone!");
			else if(uMode==7)
			{
				sprintf(gcReturnMsg,"%.32s","No jobs. Clone IP gone!\n");
				return(0);
			}
		}
		CopyContainerProps(uContainer,uNewVeid);
		//Update NAME
		sprintf(gcQuery,"DELETE FROM tProperty WHERE"
				" cName='Name' AND uKey=%u AND uType=3",uNewVeid);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
				",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
				",cName='Name',cValue='%s'",
					uNewVeid,uOwner,uLoginClient,cWizLabel);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		//Default no sync period set
		sprintf(gcQuery,"DELETE FROM tProperty WHERE"
				" cName='cuSyncPeriod' AND uKey=%u AND uType=3",uNewVeid);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
				",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
				",cName='cuSyncPeriod',cValue='%u'",
					uNewVeid,uOwner,uLoginClient,uSyncPeriod);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		uModBy=uLoginClient;
		uStatus=uAWAITCLONE;
		SetContainerStatus(uContainer,uAWAITCLONE);
	}
	else
	{
		sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",uNewVeid);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		if(uMode==0)
			tContainer("<blink>Error:</blink> No jobs created!");
		else if(uMode==1)
			tNode("<blink>Error:</blink> No jobs created!");
		else if(uMode==7)
		{
			sprintf(gcReturnMsg,"%.32s","No jobs created!\n");
			return(0);
		}
	}
	return(uNewVeid);

}//unsigned CommonCloneContainer()


void CreateDNSJob(unsigned uIPv4,unsigned uOwner,char const *cOptionalIPv4,char const *cHostname,unsigned uDatacenter,unsigned uCreatedBy)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cJobData[512];
	char cIPv4[32]={""};
	static char cView[256]={"external"};
	static char cZone[256]={""};
	static unsigned uOnlyOnce=1;

	//Sanity checks
	if(!cHostname[0])
		return;
	if(!uIPv4 && !cOptionalIPv4[0])
		return;

	//If called in loop be efficient.
	if(uOnlyOnce)
	{
		GetConfiguration("cunxsBindARecordJobZone",cZone,uDatacenter,0,0,0);
		GetConfiguration("cunxsBindARecordJobView",cView,uDatacenter,0,0,0);
		uOnlyOnce=0;
	}

	//Sanity checks
	if(!cZone[0])
		return;

	if(cOptionalIPv4!=NULL && cOptionalIPv4[0])
	{
		sprintf(cIPv4,"%.31s",cOptionalIPv4);

	}
	else if(uIPv4)
	{
		sprintf(gcQuery,"SELECT cLabel FROM tIP WHERE uIP=%u",uIPv4);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sprintf(cIPv4,"%.31s",field[0]);
		mysql_free_result(res);
	}

	//Sanity checks
	if(!cIPv4[0])
		return;

	sprintf(cJobData,"cName=%.99s.;\n"//Note trailing dot
		"cIPv4=%.99s;\n"
		"cZone=%.99s;\n"
		"cView=%.31s;\n",
			cHostname,cIPv4,cZone,cView);

	unxsBindARecordJob(uDatacenter,uNode,uContainer,cJobData,uOwner,uCreatedBy);

}//void CreateDNSJob()


unsigned uGetSearchGroup(const char *gcUser,unsigned uGroupType)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uGroup=0;

	sprintf(gcQuery,"SELECT uGroup FROM tGroup WHERE cLabel='%s' AND uGroupType=%u",gcUser,uGroupType);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(field[0]!=NULL)
			sscanf(field[0],"%u",&uGroup);
	}
	mysql_free_result(res);

	return(uGroup);

}//unsigned uGetSearchGroup()


void YesNoPullDownTriState(char *cFieldName, unsigned uSelect, unsigned uMode)
{
	char cHidden[100]={""};
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
	printf("<select name=cYesNo%s %s>\n",cFieldName,cMode);

        if(uSelect==0)
		printf("<option selected>---</option>\n");
	else
		printf("<option>---</option>\n");

        if(uSelect==1)
                printf("<option selected>No</option>\n");
        else
                printf("<option>No</option>\n");

        if(uSelect==2)
	{
                printf("<option selected>Yes</option>\n");
		if(!uMode)
			sprintf(cHidden,"<input type=hidden name=cYesNo%s value='Yes'>\n",
			     		cFieldName);
	}
        else
	{
                printf("<option>Yes</option>\n");
	}

        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//YesNoPullDownTriState()


int ReadYesNoPullDownTriState(const char *cLabel)
{
        if(!strcmp(cLabel,"Yes"))
                return(2);
        else if(!strcmp(cLabel,"No"))
                return(1);
	else
                return(0);

}//ReadYesNoPullDownTriState(char *cLabel)


unsigned CreateExecuteCommandsJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uOwner,char *cCommands)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='ExecuteCommandsJob(%u)',cJobName='ExecuteCommands'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",cJobData='%.2047s'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				TextAreaSave(cCommands),
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	return(uCount);

}//unsigned CreateExecuteCommandsJob()


//DEFINITION
//	The primary group is the first group that was assigned to
//	a container based on the smallest uGroupGlue value.
unsigned uGetPrimaryContainerGroup(unsigned uContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uGroup=0;

	//requires mysql >= 5.?
	sprintf(gcQuery,"SELECT uGroup FROM tGroupGlue WHERE uGroupGlue=(SELECT MIN(tGroupGlue.uGroupGlue) FROM tGroupGlue,tGroup WHERE"
				" tGroupGlue.uGroup=tGroup.uGroup AND tGroup.uGroupType=1 AND tGroupGlue.uContainer=%u)",uContainer);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(field[0]!=NULL)
			sscanf(field[0],"%u",&uGroup);
	}
	mysql_free_result(res);

	return(uGroup);

}//unsigned uGetPrimaryContainerGroup(unsigned uContainer)


unsigned uGetPrimaryContainerGroupGlueRecord(unsigned uContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uGroup=0;

	//requires mysql >= 5.?
	sprintf(gcQuery,"SELECT MIN(tGroupGlue.uGroupGlue) FROM tGroupGlue,tGroup WHERE tGroupGlue.uGroup=tGroup.uGroup"
			" AND tGroup.uGroupType=1 AND tGroupGlue.uContainer=%u",uContainer);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(field[0]!=NULL)
			sscanf(field[0],"%u",&uGroup);
	}
	mysql_free_result(res);

	return(uGroup);

}//unsigned uGetPrimaryContainerGroupGlueRecord(unsigned uContainer)


unsigned UpdatePrimaryContainerGroup(unsigned uContainer,unsigned uNewGroup)
{
	unsigned uPrimaryGroupGlueRecord=uGetPrimaryContainerGroupGlueRecord(uContainer);

	if(!uPrimaryGroupGlueRecord)
	{
		sprintf(gcQuery,"INSERT INTO tGroupGlue SET uContainer=%u,uGroup=%u",uContainer,uNewGroup);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	else
	{
		sprintf(gcQuery,"UPDATE tGroupGlue SET uGroup=%u WHERE uGroupGlue=%u",uNewGroup,uPrimaryGroupGlueRecord);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}

	if(mysql_affected_rows(&gMysql))
		return(0);
	else
		return(1);

}//unsigned UpdatePrimaryContainerGroup(unsigned uContainer,unsigned uNewGroup)
