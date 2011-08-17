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
unsigned unxsBindARecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
		unsigned uOwner,unsigned uCreatedBy);
void ChangeGroup(unsigned uContainer, unsigned uGroup);
unsigned CommonCloneContainer(
		unsigned uContainer,
		unsigned uOSTemplate,
		unsigned uConfig,
		unsigned uNameserver,
		unsigned uSearchdomain,
		unsigned uDatacenter,
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
		unsigned uCloneStop);
static unsigned uHideProps=0;
static unsigned uTargetNode=0;
static char cuTargetNodePullDown[256]={""};
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
static char cNetmask[64]={""};
static unsigned uWizIPv4=0;
static char cuWizIPv4PullDown[32]={""};
static unsigned uAllPortsOpen=0;
static unsigned uCloneStop=0;
static unsigned uSyncPeriod=0;
static unsigned guNoClones=0;
static unsigned guOpOnClones=0;
static unsigned guInitOnly=0;
static char cSearch[32]={""};
static unsigned uGroupJobs=0;
//uGroup: Group type association
static unsigned uGroup=0;
static char cuGroupPullDown[256]={""};
//uForClient: Create for, on 'New;
static unsigned uForClient=0;
static unsigned uCreateDNSJob=0;
static char cForClientPullDown[256]={""};

//ModuleFunctionProtos()
void tContainerNavList(unsigned uNode, char *cSearch);
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
			unsigned uOwner, unsigned uLoginClient, unsigned uIPv4);
unsigned CloneContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,
				unsigned uTargetNode, unsigned uNewVeid, unsigned uPrevStatus,
				unsigned uOwner,unsigned uCreatedBy,unsigned uCloneStop);
void htmlHealth(unsigned uContainer,unsigned uType);
void htmlGroups(unsigned uNode, unsigned uContainer);
unsigned TemplateContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uStatus,
		unsigned uOwner,char *cConfigLabel);
unsigned HostnameContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,char *cPrevHostname);
unsigned IPContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,
			unsigned uOwner,unsigned uLoginClient,char const *cIPOld);
unsigned ActionScriptsJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned CloneNode(unsigned uSourceNode,unsigned uTargetNode,unsigned uWizIPv4,const char *cuWizIPv4PullDown);
char *cRatioColor(float *fRatio);
void htmlGenMountInputs(unsigned const uMountTemplate);
unsigned uCheckMountSettings(unsigned uMountTemplate);
void htmlMountTemplateSelect(unsigned uSelector);
void AddMountProps(unsigned uContainer);
void CopyContainerProps(unsigned uSource, unsigned uTarget);
//These two jobs are always done in pairs. Even though the second may run much later
//for example after hardware failure has been fixed.
unsigned FailoverToJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,unsigned uOwner,unsigned uLoginClient);
unsigned FailoverFromJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,
				unsigned uIPv4,char *cLabel,char *cHostname,unsigned uSource,
				unsigned uStatus,unsigned uFailToJob,unsigned uOwner,unsigned uLoginClient);
void htmlCloneInfo(unsigned uContainer);
void CreateDNSJob(unsigned uIPv4,unsigned uOwner,char const *cOptionalIPv4,
			char const *cHostname,unsigned uDatacenter,unsigned uCreatedBy);

//extern
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);//jobqueue.c
void DelProperties(unsigned uNode,unsigned uType);//tnodefunc.h

#include <openisp/ucidr.h>

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
	unsigned uOnlyOnce=1;
	char cConfBuffer[256]={""};
	char cAutoCloneIPClass[256]={""};

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
		else if(!strcmp(entries[i].name,"guOpOnClones"))
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
					if(!strcmp(gcCommand,"Group Stop"))
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
								uGroupJobs++;
							}
						}
					}
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
								uGroupJobs++;
							}
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
										uGroupJobs++;
									}
								}
							}
							mysql_free_result(res);
						}
					}
					else if(!strcmp(gcCommand,"Group Change"))
					{
						struct structContainer sContainer;

						InitContainerProps(&sContainer);
						GetContainerProps(uCtContainer,&sContainer);
						if( (sContainer.uOwner==guCompany || guCompany==1) && uGroup)
						{
							//Remove from all groups
							sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer=%u",
								uCtContainer);
							mysql_query(&gMysql,gcQuery);
							if(mysql_errno(&gMysql))
								htmlPlainTextError(mysql_error(&gMysql));
							sprintf(gcQuery,"INSERT INTO tGroupGlue SET uContainer=%u,uGroup=%u",
								uCtContainer,uGroup);
							mysql_query(&gMysql,gcQuery);
							if(mysql_errno(&gMysql))
								htmlPlainTextError(mysql_error(&gMysql));
							uGroupJobs++;
						}
					}
					else if(!strcmp(gcCommand,"Group Delete"))
					{
						struct structContainer sContainer;

						InitContainerProps(&sContainer);
						GetContainerProps(uCtContainer,&sContainer);
						if( (sContainer.uStatus==uINITSETUP)
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
							//81=Awaiting clone
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
								uGroupJobs++;
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
										uGroupJobs++;
								}
							}
							mysql_free_result(res);
						}
					}
					//Cancel
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
								uGroupJobs++;
							}
						}
					}
					//Clone selected containers
					else if(!strcmp(gcCommand,"Group Clone"))
					{
						struct structContainer sContainer;

						InitContainerProps(&sContainer);
						GetContainerProps(uCtContainer,&sContainer);
						if( (sContainer.uStatus==uACTIVE || sContainer.uStatus==uSTOPPED)
							&& (sContainer.uOwner==guCompany || guCompany==1))
						{
							//For complete clone run, these stay constant.
							if(uOnlyOnce)
							{

							//We can only fo this if tConfiguration has been setup
							//with datacenter wide cAutoCloneNode=node1,cAutoCloneSyncTime=600
							//for example.
							GetConfiguration("cAutoCloneNode",cConfBuffer,uDatacenter,0,0,0);
							if(cConfBuffer[0])
								uTargetNode=ReadPullDown("tNode","cLabel",cConfBuffer);
							//We need the cuSyncPeriod
							uSyncPeriod=0;//Never rsync
							GetConfiguration("cAutoCloneSyncTime",cConfBuffer,uDatacenter,0,0,0);
							if(cConfBuffer[0])
								sscanf(cConfBuffer,"%u",&uSyncPeriod);
							GetConfiguration("cAutoCloneIPClass",cAutoCloneIPClass,uDatacenter,
								0,0,0);

								uOnlyOnce=0;

							}
							//Basic conditions
							//We do not allow clones of clones yet.
							if(uTargetNode && !sContainer.uSource && cAutoCloneIPClass[0])
							{
								char cTargetNodeIPv4[256]={""};
								unsigned uNewVeid=0;
								MYSQL_RES *res;
								MYSQL_ROW field;

								//Get next available IP, set uIPv4
								uWizIPv4=0;
								//TODO we cant let root just grab anybodys IPs
								if(guCompany==1)
									sprintf(gcQuery,"SELECT uIP FROM tIP WHERE"
									" uAvailable=1 AND cLabel LIKE '%s.%%' LIMIT 1",
										cAutoCloneIPClass);
								else
									sprintf(gcQuery,"SELECT uIP FROM tIP WHERE"
										" uAvailable=1 AND cLabel LIKE '%s.%%' AND"
									" uOwner=%u LIMIT 1",cAutoCloneIPClass,guCompany);
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
									continue;
								if(uWizIPv4==sContainer.uIPv4)
									continue;

								//Target node can't match source node.
								if(uTargetNode==sContainer.uNode)
									continue;

								//We need the target nodes IP for the clone job data.
								GetNodeProp(uTargetNode,"cIPv4",cTargetNodeIPv4);
								if(!cTargetNodeIPv4[0])
									continue;

								//Check for sane sync periods
								if(!uSyncPeriod && uSyncPeriod<300 && uSyncPeriod>86400*30)
									continue;

								//If the container is VETH then target node must support it.
								if(sContainer.uVeth)
								{
									GetNodeProp(uNode,"Container-Type",cConfBuffer);
									if(!strstr(cConfBuffer,"VETH"))
										continue;
								}

								//We require group now.
								//Get group from source container
								uGroup=uGetGroup(0,uCtContainer);
								if(!uGroup)
									continue;

								//Finally we can create the clone container
								//and the clone job
								uNewVeid=CommonCloneContainer(
										uCtContainer,
										sContainer.uOSTemplate,
										sContainer.uConfig,
										sContainer.uNameserver,
										sContainer.uSearchdomain,
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
										uCloneStop);
								//Now that container exists we can assign group.
								if(!uNewVeid)
									continue;
								ChangeGroup(uNewVeid,uGroup);
								SetContainerStatus(uCtContainer,uAWAITCLONE);
								SetContainerStatus(uNewVeid,uAWAITCLONE);
								uGroupJobs++;
							}
						}
					}
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
									uGroupJobs++;
								}
							}
							else
							{
								uOwner=guCompany;
								if(CreateStartContainerJob(sContainer.uDatacenter,
									sContainer.uNode,uCtContainer,sContainer.uOwner))
								{
									SetContainerStatus(uCtContainer,uAWAITACT);
									uGroupJobs++;
								}
							}
						}
					}
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
								tContainer("<blink>Error:</blink> No tOSTemplate.cLabel!");
							if(!cConfigLabel[0])
								tContainer("<blink>Error:</blink> No tContainer.cLabel!");
							sprintf(gcQuery,"SELECT uConfig FROM tConfig WHERE cLabel='%s'",cConfigLabel);
							mysql_query(&gMysql,gcQuery);
							if(mysql_errno(&gMysql))
								htmlPlainTextError(mysql_error(&gMysql));
			        			res=mysql_store_result(&gMysql);
							if(mysql_num_rows(res)>0)
								tContainer("<blink>Error:</blink> tConfig.cLabel collision!");
							mysql_free_result(res);
							sprintf(gcQuery,"SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='%.67s-%.32s'",
									cOSTLabel,cConfigLabel);
							mysql_query(&gMysql,gcQuery);
							if(mysql_errno(&gMysql))
								htmlPlainTextError(mysql_error(&gMysql));
			        			res=mysql_store_result(&gMysql);
							if(mysql_num_rows(res)>0)
								tContainer("<blink>Error:</blink> tOSTemplate.cLabel collision!");

							if(TemplateContainerJob(sContainer.uDatacenter,
								sContainer.uNode,uCtContainer,sContainer.uStatus,
								sContainer.uOwner,cConfigLabel))
							{
								SetContainerStatus(uCtContainer,uAWAITTML);
								uGroupJobs++;
							}
						}
					}
					else if(!strcmp(gcCommand,"Group Switchover"))
					{
						struct structContainer sContainer;

						InitContainerProps(&sContainer);
						GetContainerProps(uCtContainer,&sContainer);
						if( sContainer.uSource!=0 &&
							 (sContainer.uStatus==uSTOPPED || sContainer.uStatus==uACTIVE)
							&& (sContainer.uOwner==guCompany || guCompany==1))
						{
							unsigned uFailToJob=0;
							if((uFailToJob=FailoverToJob(sContainer.uDatacenter,sContainer.uNode,
								uCtContainer,sContainer.uOwner,guLoginClient)))
							{
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
										uCtContainer,sContainer.uStatus,
										uFailToJob,sContainer.uOwner,guLoginClient))
								{
									SetContainerStatus(uCtContainer,uAWAITFAIL);
									SetContainerStatus(sContainer.uSource,uAWAITFAIL);
									uGroupJobs++;
								}
							}
						}
					}
				}
			}
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
		else if(!strcmp(gcCommand,"Select Datacenter/Org"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstContainerVars(entries,x);
                        	guMode=9001;
				if(!uDatacenter)
					tContainer("<blink>Error:</blink> Must select a datacenter.");
				GetDatacenterProp(uDatacenter,"NewContainerMode",cNCMDatacenter);
				if(cNCMDatacenter[0] && strcmp(cNCMDatacenter,"Active"))
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
				if(cNCMDatacenter[0] && strcmp(cNCMDatacenter,"Active"))
					tContainer("<blink>Error:</blink> Selected datacenter is full or not active. Select another.");

				GetNodeProp(uNode,"NewContainerMode",cNCMNode);
				if(cNCMNode[0] && strcmp(cNCMNode,"Active"))
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
				char cTargetNodeIPv4[32]={""};
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
				if(cNCMDatacenter[0] && strcmp(cNCMDatacenter,"Active"))
					tContainer("<blink>Error:</blink> Selected datacenter is full or not active. Select another.");

				GetNodeProp(uNode,"NewContainerMode",cNCMNode);
				if(cNCMNode[0] && strcmp(cNCMNode,"Active"))
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
				if(strstr(cHostname+(uHostnameLen-strlen(".cloneNN")-1),".clone"))
					tContainer("<blink>Error:</blink> cHostname can't end with '.cloneN'");
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
				GetConfiguration("cAutoCloneNode",cAutoCloneNode,uDatacenter,0,0,0);
				if(cAutoCloneNode[0])
				{

					if(uTargetNode==0)
						tContainer("<blink>Error:</blink> Please select a valid target node"
								" for the clone");
					if(uTargetNode==uNode)
						tContainer("<blink>Error:</blink> Can't clone to same node");

					GetNodeProp(uTargetNode,"NewContainerMode",cNCMNode);
					if(cNCMNode[0] && strcmp(cNCMNode,"Clone"))
					tContainer("<blink>Error:</blink> Selected clone target node is not configured for clone containers."
							"Select another.");

					sscanf(ForeignKey("tNode","uDatacenter",uTargetNode),"%u",&uNodeDatacenter);
					if(uDatacenter!=uNodeDatacenter)
						tContainer("<blink>Error:</blink> The specified clone uNode does not "
							"belong to the specified uDatacenter.");
					GetNodeProp(uTargetNode,"cIPv4",cTargetNodeIPv4);
					if(!cTargetNodeIPv4[0])
						tContainer("<blink>Error:</blink> Your target node is"
							" missing it's cIPv4 property");
					if(!uWizIPv4)
						tContainer("<blink>Error:</blink> You must select an IP for the clone");
					if(uWizIPv4==uIPv4)
						tContainer("<blink>Error:</blink> You must select a different IP for the"
										" clone");
					sscanf(ForeignKey("tIP","uDatacenter",uWizIPv4),"%u",&uIPv4Datacenter);
					if(uDatacenter!=uIPv4Datacenter)
						tContainer("<blink>Error:</blink> The specified uIPv4 does not "
							"belong to the specified uDatacenter.");
					if(cNCCloneRange[0] && !uIpv4InCIDR4(ForeignKey("tIP","cLabel",uWizIPv4),cNCCloneRange))
						tContainer("<blink>Error:</blink> Clone start uIPv4 must be in datacenter clone IP range");
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
					sprintf(gcQuery,"SELECT cLabel FROM tIP WHERE uIP=%u AND uAvailable=1"
						" AND uOwner=%u AND uDatacenter=%u",uWizIPv4,uForClient,uDatacenter);
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
								uForClient,cIPv4ClassCClone,uDatacenter);
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
						sprintf(gcQuery,"INSERT INTO tGroup SET cLabel='%s',uGroupType=1,"
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
						ChangeGroup(uApplianceContainer,uGroup);

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
					ChangeGroup(uContainer,uGroup);

				if(cAutoCloneNode[0])
				{
					uNewVeid=CommonCloneContainer(
									uContainer,
									uOSTemplate,
									uConfig,
									uNameserver,
									uSearchdomain,
									uDatacenter,
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
									uCloneStop);
					SetContainerStatus(uContainer,uINITSETUP);
					if(uGroup)
						ChangeGroup(uNewVeid,uGroup);

						//TODO cIPv4ClassCClone can't = cIPv4ClassC
						//Get next available uWizIPv4
					sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uAvailable=1 AND uOwner=%u"
							" AND cLabel LIKE '%s%%' AND uDatacenter=%u LIMIT 1",
								uForClient,cIPv4ClassCClone,uDatacenter);
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
				if(cNCMDatacenter[0] && strcmp(cNCMDatacenter,"Active"))
					tContainer("<blink>Error:</blink> Selected datacenter is full or not active. Select another.");

				GetNodeProp(uNode,"NewContainerMode",cNCMNode);
				if(cNCMNode[0] && strcmp(cNCMNode,"Active"))
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
				char cTargetNodeIPv4[32]={""};
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
				if(cNCMDatacenter[0] && strcmp(cNCMDatacenter,"Active"))
					tContainer("<blink>Error:</blink> Selected datacenter is full or not active. Select another.");

				GetNodeProp(uNode,"NewContainerMode",cNCMNode);
				if(cNCMNode[0] && strcmp(cNCMNode,"Active"))
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
				if(strstr(cHostname+(uHostnameLen-strlen(".cloneNN")-1),".clone"))
					tContainer("<blink>Error:</blink> cHostname can't end with '.cloneN'");
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
				GetConfiguration("cAutoCloneNode",cAutoCloneNode,uDatacenter,0,0,0);
				if(cAutoCloneNode[0])
				{

					if(uTargetNode==0)
						tContainer("<blink>Error:</blink> Please select a valid target node"
								" for the clone");
					if(uTargetNode==uNode)
						tContainer("<blink>Error:</blink> Can't clone to same node");

					GetNodeProp(uTargetNode,"NewContainerMode",cNCMNode);
					if(cNCMNode[0] && strcmp(cNCMNode,"Clone"))
					tContainer("<blink>Error:</blink> Selected node is not configured for clone containers."
							"Select another.");

					sscanf(ForeignKey("tNode","uDatacenter",uTargetNode),"%u",&uNodeDatacenter);
					if(uDatacenter!=uNodeDatacenter)
						tContainer("<blink>Error:</blink> The specified clone uNode does not "
							"belong to the specified uDatacenter.");
					GetNodeProp(uTargetNode,"cIPv4",cTargetNodeIPv4);
					if(!cTargetNodeIPv4[0])
						tContainer("<blink>Error:</blink> Your target node is"
							" missing it's cIPv4 property");
					if(!uWizIPv4)
						tContainer("<blink>Error:</blink> You must select an IP for the clone");
					if(uWizIPv4==uIPv4)
						tContainer("<blink>Error:</blink> You must select a different IP for the"
										" clone");
					sscanf(ForeignKey("tIP","uDatacenter",uWizIPv4),"%u",&uIPv4Datacenter);
					if(uDatacenter!=uIPv4Datacenter)
						tContainer("<blink>Error:</blink> The specified uIPv4 does not "
							"belong to the specified uDatacenter.");
					if(cNCCloneRange[0] && !uIpv4InCIDR4(ForeignKey("tIP","cLabel",uWizIPv4),cNCCloneRange))
						tContainer("<blink>Error:</blink> Clone start uIPv4 must be in datacenter clone IP range");
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
						" AND uOwner=%u AND uDatacenter=%u",uWizIPv4,uForClient,uDatacenter);
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
								uForClient,cIPv4ClassCClone,uDatacenter);
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
						sprintf(gcQuery,"INSERT INTO tGroup SET cLabel='%s',uGroupType=1,"
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
						ChangeGroup(uContainer,uGroup);

					if(cAutoCloneNode[0])
					{
						uNewVeid=CommonCloneContainer(
									uContainer,
									uOSTemplate,
									uConfig,
									uNameserver,
									uSearchdomain,
									uDatacenter,
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
									uCloneStop);
						SetContainerStatus(uContainer,uINITSETUP);
						if(uGroup)
							ChangeGroup(uNewVeid,uGroup);

						//TODO cIPv4ClassCClone can't = cIPv4ClassC
						//Get next available uWizIPv4
						sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uAvailable=1 AND uOwner=%u"
							" AND cLabel LIKE '%s%%' AND uDatacenter=%u LIMIT 1",
								uForClient,cIPv4ClassCClone,uDatacenter);
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
						CreateDNSJob(uIPv4,uForClient,NULL,cHostname,uDatacenter,guLoginClient);

					//Get next available uIPv4
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
				sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer=%u",uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				CancelContainerJob(uDatacenter,uNode,uContainer,0);
				//81=Awaiting clone
				sprintf(gcQuery,"DELETE FROM tContainer WHERE uStatus=81 AND uSource=%u",uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer="
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
				if(strstr(cHostname,".clone"))
					tContainer("<blink>Error:</blink> cHostname can't have '.clone'!");
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
					ChangeGroup(uContainer,uGroup);
						
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
				char cTargetNodeIPv4[32]={""};
				unsigned uNewVeid=0;

                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it");

                        	guMode=7001;
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
				GetNodeProp(uTargetNode,"cIPv4",cTargetNodeIPv4);
				if(!cTargetNodeIPv4[0])
					tContainer("<blink>Error:</blink> Unexpected problem, target node is"
							" missing it's cIPv4 property!");
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
					ChangeGroup(uContainer,uGroup);

				//Set local global cWizHostname
				//Insert clone container into tContainer
				uNewVeid=CommonCloneContainer(
					uContainer,
					uOSTemplate,
					uConfig,
					uNameserver,
					uSearchdomain,
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
					uCloneStop);

				//Set group of clone to group of source.
				uGroup=uGetGroup(0,uContainer);
				if(uGroup)
					ChangeGroup(uNewVeid,uGroup);
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
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
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
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
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
					ChangeGroup(uContainer,uGroup);

				if(MigrateContainerJob(uDatacenter,uNode,uContainer,uTargetNode,uOwner,guLoginClient,0))
				{
					uStatus=uAWAITMIG;
					SetContainerStatus(uContainer,21);//Awaiting Migration
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
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
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
			if(uStatus==uACTIVE && uAllowMod(uOwner,uCreatedBy))
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
				uHostnameLen=strlen(cHostname);
				if(!strstr(cHostname+(uHostnameLen-strlen(cunxsBindARecordJobZone)-1),cunxsBindARecordJobZone))
					tContainer("<blink>Error:</blink> cHostname must end with cunxsBindARecordJobZone");

                        	guMode=0;

				//Optional change group.
				if(uGroup)
					ChangeGroup(uContainer,uGroup);

				if(MigrateContainerJob(uDatacenter,uNode,uContainer,uTargetNode,uOwner,guLoginClient,uWizIPv4))
				{
					char cIPOld[32]={""};

					uStatus=uAWAITMIG;
					SetContainerStatus(uContainer,21);//Awaiting Migration

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
					//Note that job is for new node
					//Then this job must run after migration.
					if(IPContainerJob(uTargetDatacenter,uTargetNode,uContainer,uOwner,guLoginClient,cIPOld))

					//Create unxsBind DNS
					CreateDNSJob(uWizIPv4,uOwner,NULL,cHostname,uDatacenter,guLoginClient);

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
        			MYSQL_ROW field;
				char cOSTLabel[101]={""};

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
				sprintf(gcQuery,"SELECT cLabel FROM tOSTemplate WHERE uOSTemplate=%u",uOSTemplate);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
					sprintf(cOSTLabel,"%.100s",field[0]);
				else
					tContainer("<blink>Error:</blink> No tOSTemplate.cLabel!");
				mysql_free_result(res);
				sprintf(gcQuery,"SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='%.67s-%.32s'",
						cOSTLabel,cConfigLabel);
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
				if(!strcmp(cWizHostname,cHostname) && !strcmp(cWizLabel,cLabel))
					tContainer("<blink>Error:</blink> cHostname and cLabel are the same!");
				if((uHostnameLen=strlen(cWizHostname))<5)
					tContainer("<blink>Error:</blink> cHostname too short!");
				if((uLabelLen=strlen(cWizLabel))<2)
					tContainer("<blink>Error:</blink> cLabel too short!");
				if(strchr(cWizLabel,'.'))
					tContainer("<blink>Error:</blink> cLabel has at least one '.'!");
				if(strstr(cWizLabel,"-clone"))
					tContainer("<blink>Error:</blink> cLabel can't have '-clone'");
				if(strstr(cWizHostname+(uHostnameLen-strlen(".cloneNN")-1),".clone"))
					tContainer("<blink>Error:</blink> cHostname can't end with '.cloneN'");
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
						sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer=%u",uOfflineContainer);
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
					ChangeGroup(uContainer,uGroup);

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
					CreateDNSJob(uIPv4,uOwner,NULL,cHostname,uDatacenter,guLoginClient);
				if(HostnameContainerJob(uDatacenter,uNode,uContainer,cPrevHostname))
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
		}
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
				unsigned uOldIPv4;
				unsigned uHostnameLen=0;
				char cIPOld[32]={""};

                        	guMode=0;
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error:</blink> This record was modified. Reload it.");
                        	guMode=6001;
				if(!uWizIPv4)
					tContainer("<blink>Error:</blink> You must select an IP!");
				sscanf(ForeignKey("tIP","uDatacenter",uWizIPv4),"%u",&uIPv4Datacenter);
				if(uDatacenter!=uIPv4Datacenter)
					tContainer("<blink>Error:</blink> The specified target uIPv4 does not "
							"belong to the specified uDatacenter.");
				//DNS sanity check
				if(uCreateDNSJob)
				{
					cunxsBindARecordJobZone[0]=0;
					GetConfiguration("cunxsBindARecordJobZone",cunxsBindARecordJobZone,uDatacenter,0,0,0);
					if(!cunxsBindARecordJobZone[0])
						tContainer("<blink>Error:</blink> Create job for unxsBind,"
								" but no cunxsBindARecordJobZone");
				
					uHostnameLen=strlen(cHostname);
					if(!strstr(cHostname+(uHostnameLen-strlen(cunxsBindARecordJobZone)-1),cunxsBindARecordJobZone))
						tContainer("<blink>Error:</blink> cHostname must end with cunxsBindARecordJobZone");
				}
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
				//Optional change group.
				if(uGroup)
					ChangeGroup(uContainer,uGroup);
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
				tContainer("<blink>Error:</blink> Denied by permissions settings");
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
                else if(!strcmp(gcCommand,"Confirm Failover"))
                {
                        ProcesstContainerVars(entries,x);
			if((uStatus==uACTIVE || uStatus==uSTOPPED) && uAllowMod(uOwner,uCreatedBy))
			{
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
				if((uFailToJob=FailoverToJob(uDatacenter,uNode,uContainer,uOwner,guLoginClient)))
				{
					unsigned uSourceDatacenter=0;
					unsigned uSourceNode=0;

					sscanf(ForeignKey("tContainer","uDatacenter",uSource),"%u",&uSourceDatacenter);
					sscanf(ForeignKey("tContainer","uNode",uSource),"%u",&uSourceNode);

//These two jobs are always done in pairs. Even though the second may run much later
//for example after hardware failure has been fixed.
					if(FailoverFromJob(uSourceDatacenter,uSourceNode,uSource,uIPv4,
							cLabel,cHostname,uContainer,uStatus,uFailToJob,uOwner,guLoginClient))
					{
						uStatus=uAWAITFAIL;
						SetContainerStatus(uContainer,uAWAITFAIL);
						SetContainerStatus(uSource,uAWAITFAIL);
						sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
						tContainer("FailoverJob() Done");
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
                else if(!strncmp(gcCommand,"Group",5))
                {
			ExtProcesstContainerVars(entries,x);
			if(uGroupJobs)
				tContainer("'Group' jobs created/canceled");
			else
				tContainer("No 'Group' jobs created/canceled");
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
				" insure that any mount/umount scripts that reference the container IP"
				" are handled correctly."
				" <p>Container services may be affected or need reconfiguration for new IP.\n");
			printf("<p>Select new IPv4<br>");
			tTablePullDownAvail("tIP;cuWizIPv4PullDown","cLabel","cLabel",uWizIPv4,1);
			printf("<p><input title='Create an IP change job for the current container'"
					" type=submit class=lwarnButton"
					" name=gcCommand value='Confirm IP Change'>\n");
			printf("<p>Optional primary group change<br>");
			uGroup=uGetGroup(0,uContainer);//0=not for node
			tTablePullDown("tGroup;cuGroupPullDown","cLabel","cLabel",uGroup,1);
			GetConfiguration("cunxsBindARecordJobZone",cunxsBindARecordJobZone,uDatacenter,0,0,0);
			if(cunxsBindARecordJobZone[0])
			{
				printf("<p>Create job for unxsBind A record<br>");
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
			uGroup=uGetGroup(0,uContainer);//0=not for node
			tTablePullDown("tGroup;cuGroupPullDown","cLabel","cLabel",uGroup,1);
			GetConfiguration("cunxsBindARecordJobZone",cunxsBindARecordJobZone,uDatacenter,0,0,0);
			if(cunxsBindARecordJobZone[0])
			{
				printf("<p>Create job for unxsBind A record<br>");
				printf("<input type=checkbox name=uCreateDNSJob >");
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
			uGroup=uGetGroup(0,uContainer);//0=not for node
			tTablePullDown("tGroup;cuGroupPullDown","cLabel","cLabel",uGroup,1);
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
			uGroup=uGetGroup(0,uContainer);//0=not for node
			tTablePullDown("tGroup;cuGroupPullDown","cLabel","cLabel",uGroup,1);
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
			printf("<p><input type=checkbox name=uCloneStop checked> Initial state is stopped");
			printf("<p>cuSyncPeriod<br>");
			printf("<input title='Keep clone in sync every cuSyncPeriod seconds"
					". You can change this at any time via the property panel.'"
					" type=text size=10 maxlength=7"
					" name=uSyncPeriod value='%u'>\n",uSyncPeriod);
			printf("<p>Optional primary group change<br>");
			uGroup=uGetGroup(0,uContainer);//0=not for node
			tTablePullDown("tGroup;cuGroupPullDown","cLabel","cLabel",uGroup,1);
			if(uGroup)
				printf("<input type=hidden name=uGroup value='%u'>",uGroup);
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
			uGroup=uGetGroup(0,uContainer);//0=not for node
			tTablePullDown("tGroup;cuGroupPullDown","cLabel","cLabel",uGroup,1);
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
			GetConfiguration("cAutoCloneNode",cAutoCloneNode,uDatacenter,0,0,0);
			if(cAutoCloneNode[0])
				printf("<p>Auto-clone subsystem is enabled for selected datacenter: Clone target node"
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
			GetConfiguration("cAutoCloneNode",cAutoCloneNode,uDatacenter,0,0,0);
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
			printf("<br><input title='Remove clones from search nav list' type=checkbox name=guNoClones");
			if(guNoClones)
				printf(" checked");
			printf("> guNoClones");
			printf("<input title='-Initial Setup- status containers only' type=checkbox name=guInitOnly");
			if(guInitOnly)
				printf(" checked");
			printf("> guInitOnly");
			printf("<input title='Operate on clones in group functions when appropiate'"
					" type=checkbox name=guOpOnClones> guOpOnClones\n");
			printf("<p><u>Container NavList Filter by tGroup</u><br>");
			tTablePullDown("tGroup;cuGroupPullDown","cLabel","cLabel",uGroup,1);

			tContainerNavList(0,cSearch);
			if(uContainer && uAllowMod(uOwner,uCreatedBy) && guPermLevel>7)
			{
				if(uStatus==uACTIVE)
				{
					htmlHealth(uContainer,3);
					printf("<p><input title='Migrate container to another hardware node'"
					" type=submit class=largeButton"
					" name=gcCommand value='Migration Wizard'><br>\n");
					printf("<input title='Migrate container to another datacenter node'"
					" type=submit class=largeButton"
					" name=gcCommand value='Remote Migration'><br>\n");
					if(!strstr(cLabel,"-clone"))
					printf("<input title='Clone a container to this or another hardware node."
					" The clone will be an online container with another IP and hostname."
					" It will be kept updated via rsync on a configurable basis.'"
					" type=submit class=largeButton"
					" name=gcCommand value='Clone Wizard'><br>\n");
					printf("<input title='Change current container IPv4'"
					" type=submit class=largeButton"
					" name=gcCommand value='IP Change Wizard'><br>\n");
					printf("<p><input title='Creates a job for stopping an active container.'"
						" type=submit class=lwarnButton"
						" name=gcCommand value='Stop %.24s'><br>\n",cLabel);
					if(uSource)
						printf("<p><input title='Creates jobs for manual failover (switchover.)'"
						" type=submit class=lwarnButton"
						" name=gcCommand value='Failover %.25s'><br>\n",cLabel);
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
						printf("<p><input title='Creates jobs for manual failover (switchover.)'"
						" type=submit class=lwarnButton"
						" name=gcCommand value='Failover %.25s'><br>\n",cLabel);
				}

				if( uStatus==uSTOPPED || uStatus==uACTIVE )
				{
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


void tContainerNavList(unsigned uNode, char *cSearch)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uNumRows=0;
	unsigned uMySQLNumRows=0;
#define uLIMIT 32

	if(!uNode)
	{
	    if(guNoClones)
	    {
		if(cSearch[0] && !uGroup)
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
		else if(cSearch[0] && uGroup)
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus,tGroupGlue"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uSource=0"
					" AND tContainer.cLabel LIKE '%s%%'"
					" AND tContainer.uContainer=tGroupGlue.uContainer"
					" AND tGroupGlue.uGroup=%u"
					" ORDER BY tContainer.cLabel",cSearch,uGroup);
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus,tGroupGlue"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uSource=0"
					" AND tContainer.cLabel LIKE '%2$s%%'"
					" AND tContainer.uContainer=tGroupGlue.uContainer"
					" AND tGroupGlue.uGroup=%3$u"
					" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" ORDER BY tContainer.cLabel",guCompany,cSearch,uGroup);
		}
		else if(!cSearch[0] && uGroup)
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus,tGroupGlue"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uSource=0"
					" AND tContainer.uContainer=tGroupGlue.uContainer"
					" AND tGroupGlue.uGroup=%u"
					" ORDER BY tContainer.cLabel",uGroup);
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus,tGroupGlue"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uSource=0"
					" AND tContainer.uContainer=tGroupGlue.uContainer"
					" AND tGroupGlue.uGroup=%2$u"
					" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" ORDER BY tContainer.cLabel",guCompany,uGroup);
		}
		else if(1)
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uSource=0"
					" AND tContainer.uStatus=tStatus.uStatus"
					" ORDER BY tContainer.cLabel");
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uSource=0"
					" AND tContainer.uStatus=tStatus.uStatus"
					" ORDER BY tContainer.cLabel",guCompany);
		}
	   }
	   else if(guInitOnly)
	   {
		if(cSearch[0] && !uGroup)
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
		else if(cSearch[0] && uGroup)
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus,tGroupGlue"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uStatus=11"
					" AND tContainer.cLabel LIKE '%s%%'"
					" AND tContainer.uContainer=tGroupGlue.uContainer"
					" AND tGroupGlue.uGroup=%u"
					" ORDER BY tContainer.cLabel",cSearch,uGroup);
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus,tGroupGlue"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uStatus=11"
					" AND tContainer.cLabel LIKE '%2$s%%'"
					" AND tContainer.uContainer=tGroupGlue.uContainer"
					" AND tGroupGlue.uGroup=%3$u"
					" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" ORDER BY tContainer.cLabel",guCompany,cSearch,uGroup);
		}
		else if(!cSearch[0] && uGroup)
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus,tGroupGlue"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uStatus=11"
					" AND tContainer.uContainer=tGroupGlue.uContainer"
					" AND tGroupGlue.uGroup=%u"
					" ORDER BY tContainer.cLabel",uGroup);
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus,tGroupGlue"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uStatus=11"
					" AND tContainer.uContainer=tGroupGlue.uContainer"
					" AND tGroupGlue.uGroup=%2$u"
					" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" ORDER BY tContainer.cLabel",guCompany,uGroup);
		}
		else if(1)
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=11"
					" AND tContainer.uStatus=tStatus.uStatus"
					" ORDER BY tContainer.cLabel");
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=11"
					" AND tContainer.uStatus=tStatus.uStatus"
					" ORDER BY tContainer.cLabel",guCompany);
		}
	   }
	   else //guNoClones==0 and guInitOnly==0
	   {
		if(cSearch[0] && !uGroup)
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
		else if(cSearch[0] && uGroup)
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus,tGroupGlue"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.cLabel LIKE '%s%%'"
					" AND tContainer.uContainer=tGroupGlue.uContainer"
					" AND tGroupGlue.uGroup=%u"
					" ORDER BY tContainer.cLabel",cSearch,uGroup);
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus,tGroupGlue"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.cLabel LIKE '%2$s%%'"
					" AND tContainer.uContainer=tGroupGlue.uContainer"
					" AND tGroupGlue.uGroup=%3$u"
					" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" ORDER BY tContainer.cLabel",guCompany,cSearch,uGroup);
		}
		else if(!cSearch[0] && uGroup)
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus,tGroupGlue"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uContainer=tGroupGlue.uContainer"
					" AND tGroupGlue.uGroup=%u"
					" ORDER BY tContainer.cLabel",uGroup);
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus,tGroupGlue"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uContainer=tGroupGlue.uContainer"
					" AND tGroupGlue.uGroup=%2$u"
					" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" ORDER BY tContainer.cLabel",guCompany,uGroup);
		}
		else if(1)
		{
			if(guLoginClient==1 && guPermLevel>11)//Root can read access all
				sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel FROM tContainer,tNode,tStatus"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" ORDER BY tContainer.cLabel");
			else
				sprintf(gcQuery,"SELECT tContainer.uContainer"
					",tContainer.cLabel,tNode.cLabel,tStatus.cLabel"
					" FROM tContainer," TCLIENT ",tNode,tStatus"
					" WHERE tContainer.uOwner=" TCLIENT ".uClient"
					" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
					" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
					" AND tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus"
					" ORDER BY tContainer.cLabel",guCompany);
		}
	   }//if(guNoClones)
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
				sprintf(gcQuery,"SELECT tContainer.uContainer"
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
			if(guPermLevel>9 && uNode==0)
			{
				if(cSearch[0] && uGroup)
				printf("<input type=checkbox name=Ct%s checked><a class=darkLink href=unxsVZ.cgi?gcFunction="
					"tContainer&uContainer=%s&cSearch=%s&uGroup=%u>%s/%s/%s</a><br>\n",
						field[0],field[0],cURLEncode(cSearch),uGroup,field[1],field[2],field[3]);
				else if(cSearch[0])
				printf("<input type=checkbox name=Ct%s><a class=darkLink href=unxsVZ.cgi?gcFunction="
					"tContainer&uContainer=%s&cSearch=%s>%s/%s/%s</a><br>\n",
						field[0],field[0],cURLEncode(cSearch),field[1],field[2],field[3]);
				else if(uGroup)
				printf("<input type=checkbox name=Ct%s><a class=darkLink href=unxsVZ.cgi?gcFunction="
					"tContainer&uContainer=%s&uGroup=%u>%s/%s/%s</a><br>\n",
						field[0],field[0],uGroup,field[1],field[2],field[3]);
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
				printf("(Only %u containers shown use search/filters to shorten list.)<br>\n",
						uLIMIT);
				break;
			}
		}
		if(guPermLevel>9 && uNode==0)
		{
			printf("<input type=checkbox name=all onClick='checkAll(document.formMain,this)'> Check all<br>\n");

			printf("<input title='Cancels job(s) for container(s) waiting for activation, deletion or stop.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Cancel'>\n");
			printf("<br><input title='Creates job(s) for starting stopped or initial setup container(s).'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Start'>\n");
			printf("<br><input title='Creates job(s) for templating stopped or active container(s)."
				" Uses the container label for new tOSTemplate and tConfig entries."
				" If active, containers may be stopped for several minutes if not setup for vzdump snapshot.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Template'>\n");
			printf("<br><input title='Creates job(s) for deleting initial setup container(s).'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Delete'>\n");
			printf("<br><input title='Deletes any existing group association then adds selected group to selected containers'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Change'>\n");
			printf("<p><input title='Creates job(s) for cloning active or stopped container(s) that"
				" are not clones themselves.'"
				" type=submit class=lwarnButton"
				" name=gcCommand value='Group Clone'>\n");
			printf("<p><input title='Creates job(s) for stopping active container(s).'"
				" type=submit class=lwarnButton"
				" name=gcCommand value='Group Stop'>\n");
			printf("<p><input title='Creates job(s) for switching over cloned container(s).'"
				" type=submit class=lwarnButton"
				" name=gcCommand value='Group Switchover'>\n");
			printf("<p><input title='Creates job(s) for destroying active or stopped container(s).'"
				" type=submit class=lwarnButton"
				" name=gcCommand value='Group Destroy'>\n");
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

	//Cancel any jobs for uContainer 1=Waiting 10=RemoteWaiting
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=7 WHERE "
			"uDatacenter=%u AND uNode=%u AND uContainer=%u AND (uJobStatus=1 OR uJobStatus=10)"
				,uDatacenter,uNode,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uContainerJobsCanceled=mysql_affected_rows(&gMysql);

	//Also cancel any jobs for uContainer's clones 1=Waiting 10=RemoteWaiting
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=7 WHERE "
			"uDatacenter=%u AND uContainer=(SELECT uContainer FROM tContainer WHERE uSource=%u) AND uJobStatus=1"
				,uDatacenter,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uContainerCloneJobsCanceled=mysql_affected_rows(&gMysql);

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


unsigned MigrateContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer, unsigned uTargetNode,
			unsigned uOwner, unsigned uLoginClient, unsigned uIPv4)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='MigrateContainerJob(%u-->%u)',cJobName='MigrateContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",cJobData='uTargetNode=%u;\nuIPv4=%u;'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,uTargetNode,
				uDatacenter,uNode,uContainer,
				uTargetNode,
				uIPv4,
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


unsigned HostnameContainerJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,char *cPrevHostname)
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
				uOwner,guLoginClient);
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


//Can return 0,1,2,3 or 4
unsigned CloneNode(unsigned uSourceNode,unsigned uTargetNode,unsigned uWizIPv4,const char *cuWizIPv4PullDown)
{
//#define DEBUG_CLONENODE
#ifdef DEBUG_CLONENODE
	//debug only
	printf("Content-type: text/plain\n\n");
#endif

	//Loop for all containers that belong to source node and do not have clones on the uTargetNode.
	//For every loop iteration we must get a new available IP from tIP starting at uWizIPv4
        MYSQL_RES *res;
        MYSQL_RES *res2;
        MYSQL_ROW field;
        MYSQL_ROW field2;
	unsigned uContainer=0;
	unsigned uDatacenter=0;
	unsigned uNewVeid=0;
	unsigned uCount=0;
	unsigned uStatus=0;
	unsigned uOwner=0;
	unsigned uVeth=0;
	char cWizIPv4[32];
	
	sprintf(gcQuery,"SELECT cLabel,cHostname,uOSTemplate,uConfig,uNameserver,uSearchdomain,uDatacenter"
			",uContainer,uStatus,uOwner,uVeth FROM tContainer WHERE uNode=%u AND"
			" (uStatus=%u OR uStatus=%u)",uSourceNode,uSTOPPED,uACTIVE);
	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		//Don't clone already cloned container on target node.
		sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uNode=%u AND cLabel LIKE '%.25s-clone%%' LIMIT 1",
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
		sscanf(field[9],"%u",&uOwner);
		sscanf(field[10],"%u",&uVeth);
		if(!uDatacenter || !uContainer || !field[0][0] || !field[1][0])
		{
			mysql_free_result(res);
#ifdef DEBUG_CLONENODE
			//Debug only
			printf("No containers added, unexpected error.\n");
			exit(0);
#endif
			return(4);//no containers added, unexpected error.
		}

		//If container is uVeth=1 but the target node does not allow skip.
		if(uVeth)
		{
			char cContainerType[256]={""};

			GetNodeProp(uTargetNode,"Container-Type",cContainerType);
			if(!strstr(cContainerType,"VETH"))
			{
#ifdef DEBUG_CLONENODE
				//Debug only
				printf("Target node does not allow uVeth=1 containers.\n");
#endif
				continue;
			}
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
					"uStatus=%u,"
					"uOwner=%u,"
					"uCreatedBy=%u,"
					"uSource=%u,"
					"uVeth=%u,"
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
							uAWAITCLONE,
							uOwner,
							guLoginClient,
							uContainer,
							uVeth);
#ifdef DEBUG_CLONENODE
		//Debug only
		printf("%s\n",gcQuery);
#endif
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		uNewVeid=mysql_insert_id(&gMysql);
#ifdef DEBUG_CLONENODE
		//Debug only
		printf("uNewVeid=%u\n",uNewVeid);
#endif

		if(CloneContainerJob(uDatacenter,uSourceNode,uContainer,uTargetNode,uNewVeid,uStatus,uOwner,guLoginClient,uCloneStop))
		{
#ifdef DEBUG_CLONENODE
			//Debug only
			printf("CloneContainerJob(%u,%u,%u,%u,%u,%u).\n",
					uDatacenter,uSourceNode,uContainer,uTargetNode,uNewVeid,uStatus);
#endif
			sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
					" WHERE uIP=%u AND uAvailable=1 AND uOwner=%u",uWizIPv4,uOwner);
#ifdef DEBUG_CLONENODE
			//Debug only
			printf("%s\n",gcQuery);
#endif
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
			if(mysql_affected_rows(&gMysql)!=1)
			{
//This is just a quick fix for a complicated if/then/else situation.
//This will be resolved by providing a common function.
IPExhaustionExit:
				//Undo tJob
				sprintf(gcQuery,"DELETE FROM tJob WHERE cLabel='CloneContainer(%u)'",uContainer);
#ifdef DEBUG_CLONENODE
				//Debug only
				printf("%s\n",gcQuery);
#endif
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));

				//Undo tContainer
				sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",uNewVeid);
#ifdef DEBUG_CLONENODE
				//Debug only
				printf("%s\n",gcQuery);
#endif
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				mysql_free_result(res);
				//Operator did not select a large enough contiguous IP block starting 
				//at uWizIPv4. This is a temp hack. But allows operator to run 
				//operation again.
				if(uCount)
				{
#ifdef DEBUG_CLONENODE
					//Debug only
					printf("Some containers added, owned ips not enough. 1\n");
					exit(0);
#endif
					return(1);//some containers added, ips not enough.
				}
				else
				{
#ifdef DEBUG_CLONENODE
					//Debug only
					printf("No containers added, owned ips not enough. 3\n");
					exit(0);
#endif
					return(3);//no containers added, ips not enough.
				}
			}

			//We need to search for next available uIP from same class of IPs
			register int i;
			sprintf(cWizIPv4,"%.31s",cuWizIPv4PullDown);
			for(i=strlen(cWizIPv4);i>0;i--)
			{
				if(cWizIPv4[i]=='.')
				{
					cWizIPv4[i]=0;
					break;
				}
			}
			sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uAvailable=1 AND uIP>%u AND uOwner=%u"
					" AND cLabel LIKE '%s%%' LIMIT 1",uWizIPv4,uOwner,cWizIPv4);
#ifdef DEBUG_CLONENODE
			//Debug only
			printf("%s\n",gcQuery);
#endif
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res2=mysql_store_result(&gMysql);
			if((field2=mysql_fetch_row(res2)))
				sscanf(field2[0],"%u",&uWizIPv4);
			else
				//This is just a quick fix for a complicated if/then/else situation.
				//This will be resolved by providing a common function.
				goto IPExhaustionExit;
			
			CopyContainerProps(uContainer,uNewVeid);
			//replace Name in property uType=3 is container property type.
			sprintf(gcQuery,"DELETE FROM tProperty WHERE"
					" cName='Name' AND uKey=%u AND uType=3",uNewVeid);
#ifdef DEBUG_CLONENODE
			//Debug only
			printf("%s\n",gcQuery);
#endif
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
					",cName='Name',cValue='%.25s-clone0'",
							uNewVeid,guCompany,guLoginClient,field[0]);
#ifdef DEBUG_CLONENODE
			//Debug only
			printf("%s\n",gcQuery);
#endif
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			//default no sync period set
			sprintf(gcQuery,"DELETE FROM tProperty WHERE"
					" cName='cuSyncPeriod' AND uKey=%u AND uType=3",uNewVeid);
#ifdef DEBUG_CLONENODE
			//Debug only
			printf("%s\n",gcQuery);
#endif
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
					",cName='cuSyncPeriod',cValue='0'",
						uNewVeid,guCompany,guLoginClient);
#ifdef DEBUG_CLONENODE
			//Debug only
			printf("%s\n",gcQuery);
#endif
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			//from uContainer field[7] above, the source container VEID.
#ifdef DEBUG_CLONENODE
			//Debug only
			printf("SetContainerStatus(%u,%u)\n",uContainer,uAWAITCLONE);
#endif
			SetContainerStatus(uContainer,uAWAITCLONE);//Awaiting Clone
			uCount++;
		}
		else
		{
			sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",uNewVeid);
#ifdef DEBUG_CLONENODE
			//Debug only
			printf("CloneContainerJob() failed\n");
			printf("%s\n",gcQuery);
#endif
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
		}

	}
	mysql_free_result(res);


	if(uNewVeid)
	{
#ifdef DEBUG_CLONENODE
		//Debug only
		printf("All possible containers added as expected. 0\n");
		exit(0);
#endif
		return(0);//all possible containers added as expected.
	}
	else
	{
#ifdef DEBUG_CLONENODE
		//Debug only
		printf("No containers added. 2\n");
		exit(0);
#endif
		return(2);//no containers added
	}

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
unsigned FailoverToJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,unsigned uOwner,unsigned uLoginClient)
{
	unsigned uCount=0;

	if(!uDatacenter || !uNode || !uContainer) return(0);

	//This job should try to stop the uSource container if possible. Or at least
	//make sure it's IP is down. There are many failover scenarios it seems at first glance
	//we will try to handle all of them.
	//Remote datacenter failover seems to involve DNS changes. Since we can't use the VIP
	//method that should be available in a correctly configured unxsVZ datacenter.
	//debug only
	//sprintf(gcQuery,"INSERT INTO tJob SET cLabel='FailoverToJob(%u)',cJobName='FailoverToDEBUG'"
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
				unsigned uStatus,unsigned uFailToJob,unsigned uOwner,unsigned uLoginClient)
{
	unsigned uCount=0;

	if(!uDatacenter || !uNode || !uContainer) return(0);

	//If the source node is down this job will be stuck
	//When node is fixed and boots we must avoid the source continater coming up with same IP
	//as the new production container (the FailoverToJob container.)
	//debug only
	//sprintf(gcQuery,"INSERT INTO tJob SET cLabel='FailoverFromJob(%u)',cJobName='FailoverFromDEBUG'"
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


//Lowest uGroup. Which we define here as the primary group
//of a node or container.
unsigned uGetGroup(unsigned uNode, unsigned uContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uGroup=0;

	if(uNode)
		sprintf(gcQuery,"SELECT MIN(tGroup.uGroup) FROM tGroupGlue,tGroup WHERE tGroupGlue.uNode=%u"
				" AND tGroupGlue.uGroup=tGroup.uGroup",uNode);
	else
		sprintf(gcQuery,"SELECT MIN(tGroup.uGroup) FROM tGroupGlue,tGroup WHERE tGroupGlue.uContainer=%u"
				" AND tGroupGlue.uGroup=tGroup.uGroup",uContainer);
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


void ChangeGroup(unsigned uContainer, unsigned uGroup)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	if(uGroup)
	{
		unsigned uPrimaryGroup=0;

		//Get the PK of the primary group of this container.
		sprintf(gcQuery,"SELECT uGroupGlue,uGroup FROM tGroupGlue WHERE"
			" uContainer=%u ORDER BY uGroup LIMIT 1",uContainer);
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
}//void ChangeGroup(unsigned uContainer, unsigned uGroup)


unsigned CommonCloneContainer(
		unsigned uContainer,
		unsigned uOSTemplate,
		unsigned uConfig,
		unsigned uNameserver,
		unsigned uSearchdomain,
		unsigned uDatacenter,
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
		unsigned uCloneStop)
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
				uDatacenter,
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
					uWizIPv4,cClassC,uDatacenter);
			else
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
					" WHERE uIP=%u AND uAvailable=1 AND uOwner=%u AND cLabel LIKE '%s%%'"
					" AND uDatacenter=%u",
						uWizIPv4,uOwner,cClassC,uDatacenter);
		}
		//Here no such class c check
		else
		{
			if(guCompany==1)
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
				" WHERE uIP=%u AND uAvailable=1 AND uDatacenter=%u",
					uWizIPv4,uDatacenter);
			else
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
					" WHERE uIP=%u AND uAvailable=1 AND uOwner=%u AND uDatacenter=%u",
						uWizIPv4,uOwner,uDatacenter);
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

			tContainer("<blink>Error:</blink> Someone grabbed your clone IP"
					", No jobs created! (source container may have been created)");
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
		tContainer("<blink>Error:</blink> No jobs created!");
	}

	return(uNewVeid);

}//unsigned CommonCloneContainer()


void CreateDNSJob(unsigned uIPv4,unsigned uOwner,char const *cOptionalIPv4,char const *cHostname,unsigned uDatacenter,unsigned uCreatedBy)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cJobData[512];
	char cIPv4[32]={""};
	//char cOwner[32]={"Root"};
	//Get all these from tConfiguration once.
	static char cView[256]={"external"};
	static char cZone[256]={""};
	//static char cuTTL[256]={"0"};//for zone default we use 0
	//static char cNSSet[256]={""};//not supported yet
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
		//GetConfiguration("cunxsBindARecordJobTTL",cuTTL,uDatacenter,0,0,0);//not supported yet
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
	}

	//Sanity checks
	if(!cIPv4[0])
		return;

/*
	//not supported yet
	if(uOwner)
	{
		sprintf(gcQuery,"SELECT cLabel FROM tClient WHERE uClient=%u",uOwner);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sprintf(cOwner,"%.31s",field[0]);
	}
*/

	mysql_free_result(res);
	sprintf(cJobData,"cName=%.99s.;\n"//Note trailing dot
		//"cuTTL=%.15s;\n"
		"cIPv4=%.99s;\n"
		//"cNSSet=%.31s;\n"
		"cZone=%.99s;\n"
		"cView=%.31s;\n",
		//"cOwner=%.31s;\n",
			cHostname,cIPv4,cZone,cView);

	unxsBindARecordJob(uDatacenter,uNode,uContainer,cJobData,uOwner,uCreatedBy);

}//void CreateDNSJob()
