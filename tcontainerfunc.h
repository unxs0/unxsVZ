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


static unsigned uTargetNode=0;
static char cuTargetNodePullDown[256]={""};
static char cConfigLabel[32]={""};
static char cWizHostname[100]={""};
static char cWizLabel[32]={""};
static char cIPOld[32]={""};
static unsigned uWizIPv4=0;
static char cuWizIPv4PullDown[32]={""};

//ModuleFunctionProtos()
void tContainerNavList(unsigned uNode);
unsigned CreateNewContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned CreateStartContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned DestroyContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned StopContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
void CancelContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
void SetContainerStatus(unsigned uContainer,unsigned uStatus);
void SetContainerNode(unsigned uContainer,unsigned uNode);
void htmlContainerNotes(unsigned uContainer);
unsigned MigrateContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer, unsigned uTargetNode);
unsigned CloneContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,
				unsigned uTargetNode, unsigned uNewVeid);
void htmlHealth(unsigned uContainer,unsigned uType);
void htmlGroups(unsigned uNode, unsigned uContainer);
unsigned TemplateContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned HostnameContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned IPContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned MountFilesJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer);
unsigned CloneNode(unsigned uSourceNode, unsigned uTargetNode, unsigned uWizIPv4);

//tnodefunc.h
void DelProperties(unsigned uNode,unsigned uType);

//jobqueue.c
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);

void ExtProcesstContainerVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cuTargetNodePullDown"))
		{
			sprintf(cuTargetNodePullDown,"%.255s",entries[i].val);
			uTargetNode=ReadPullDown("tNode","cLabel",cuTargetNodePullDown);
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
		else if(!strcmp(entries[i].name,"cuWizIPv4PullDown"))
		{
			sprintf(cuWizIPv4PullDown,"%.31s",entries[i].val);
			uWizIPv4=ReadPullDown("tIP","cLabel",cuWizIPv4PullDown);
		}
	}
}//void ExtProcesstContainerVars(pentry entries[], int x)


void ExttContainerCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tContainerTools"))
	{
		unsigned uNodeDatacenter=0;
        	MYSQL_RES *res;
		time_t uActualModDate= -1;


		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstContainerVars(entries,x);
                        	guMode=2000;
	                        tContainer(LANG_NB_CONFIRMNEW);
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
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

                        	guMode=0;

				uStatus=11;//Initial setup
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
						",cName='Name',cValue='%s'",
							uContainer,cLabel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				tContainer("New container created and default property created");
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
				DelProperties(uContainer,3);
				//Release IP
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=1"
						" WHERE uIP=%u and uAvailable=0",uIPv4);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				//Remove from any groups
				sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer=%u",uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
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
			if( (uStatus==11  || uStatus==31) && uAllowMod(uOwner,uCreatedBy))
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
			if( (uStatus==11  || uStatus==31) && uAllowMod(uOwner,uCreatedBy))
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
			if(uStatus==31 && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

				if(CreateStartContainerJob(uDatacenter,uNode,uContainer))
				{
					uStatus=6;
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
			if(uStatus==11 && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

				if(CreateNewContainerJob(uDatacenter,uNode,uContainer))
				{
					uStatus=6;
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
			if((uStatus==1 || uStatus==31) && uAllowDel(uOwner,uCreatedBy))
			{
                        	guMode=0;
					
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

				if(DestroyContainerJob(uDatacenter,uNode,uContainer))
				{
					uStatus=5;
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
			if(uStatus==1 && uAllowDel(uOwner,uCreatedBy))
			{
                        	guMode=0;
					
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

				if(StopContainerJob(uDatacenter,uNode,uContainer))
				{
					uStatus=41;
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
				if(uStatus!=5 && uStatus!=6 && uStatus!=41)
					tContainer("<blink>Error</blink>: Unexpected uStatus!");

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

				CancelContainerJob(uDatacenter,uNode,uContainer);
				if(uStatus==5 || uStatus==41)
				{
					uStatus=1;
					SetContainerStatus(uContainer,1);//Awaiting Deletion/Stop to Active
				}
				else if(uStatus==6)
				{
					uStatus=11;
					SetContainerStatus(uContainer,11);//Awaiting Activation to Initial
				}
				
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
				tContainer("CancelContainerJob() Done");
			}
			else
			{
				tContainer("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Create Mount Files"))
                {
                        ProcesstContainerVars(entries,x);
			if( (uStatus==31 || uStatus==11 ) && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");

				if(MountFilesJob(uDatacenter,uNode,uContainer))
				{
					sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uModDate);
					tContainer("MountFilesJob() Done");
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
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
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
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
			{
				unsigned uNewVeid=0;
				char cTargetNodeIPv4[32]={""};

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
                        	guMode=0;

				sprintf(cWizLabel,"%.25s-clone",cLabel);
				sprintf(cWizHostname,"%.93s.clone",cHostname);
				sprintf(gcQuery,"INSERT INTO tContainer SET cLabel='%s',"
							"cHostname='%s',"
							"uIPv4=%u,"
							"uOSTemplate=%u,"
							"uConfig=%u,"
							"uNameserver=%u,"
							"uSearchdomain=%u,"
							"uDatacenter=%u,"
							"uNode=%u,"
							"uStatus=81,"
							"uOwner=%u,"
							"uCreatedBy=%u,"
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
							guLoginClient);
        			mysql_query(&gMysql,gcQuery);
			        if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				uNewVeid=mysql_insert_id(&gMysql);


				if(CloneContainerJob(uDatacenter,uNode,uContainer,uTargetNode,uNewVeid))
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
					sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
							",cName='Name',cValue='%s'",
								uNewVeid,cWizLabel);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					uModBy=guLoginClient;
					uStatus=81;
					SetContainerStatus(uContainer,81);//Awaiting Clone
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
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
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
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
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
                        	guMode=0;

				if(MigrateContainerJob(uDatacenter,uNode,uContainer,uTargetNode))
				{
					uStatus=21;
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
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");
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
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;
				sscanf(ForeignKey("tContainer","uModDate",uContainer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tContainer("<blink>Error</blink>: This record was modified. Reload it.");
                        	guMode=4001;
				if(strlen(cConfigLabel)<2)
					tContainer("<blink>Error</blink>: Must provide valid tConfig.cLabel!");
				if(strchr(cConfigLabel,'-'))
					tContainer("<blink>Error</blink>: tConfig.cLabel can't contain '-' chars!");
                        	guMode=0;
				if(TemplateContainerJob(uDatacenter,uNode,uContainer))
				{
					uStatus=51;
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
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
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
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
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
					uStatus=61;
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
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
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
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
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
					uStatus=71;
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
				" used for the OS template and the inicial VZ conf file."
				" !Warning container will be stopped then restarted!"
				" This may take several minutes to accomplish. When"
				" job is finished the OSTemplate and it's associated base conf file"
				" will be available on every hardware node for immediate use."
				" The only difference from a backup container is that of the speed of"
				" deployment and that a backup container maybe rsync'd on a"
				" regular basis. <p>\n");
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
				" in the clone container via a 'cSyncSchedule' entry in it's properties table.<p>");
			printf("Select target node ");
			tTablePullDown("tNode;cuTargetNodePullDown","cLabel","cLabel",uTargetNode,1);
			printf("<br>Select new IPv4 ");
			tTablePullDownAvail("tIP;cuWizIPv4PullDown","cLabel","cLabel",uWizIPv4,1);
			printf("<p><input title='Create a clone job for the current container'"
					" type=submit class=largeButton"
					" name=gcCommand value='Confirm Clone'>\n");
                break;

                case 2000:
			printf("<p><u>Enter/mod data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review changes</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			printf("<u>Table Tips</u><br>");
			printf("Setup or modify a container by selecting a meaningful cLabel,"
				" a correct datacenter and hardware node. You usually should start with"
				" a similar container. Most properties are created by data collection agents"
				", others like <i>Notes</i> are user created and maintained.<p>"
				"Special properties cVEID.mount and cVEID.umount are used via"
				" their tTemplate matching values (see tTemplate for more info)"
				" to create /etc/vz/conf/VEID.(u)mount files.");
			printf("<p><u>Record Context Info</u><br>");
			if(uContainer && uNode)
			{
				if(uStatus==1)
				{
					printf("Container is running on ");
					printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tNode&uNode=%u>%s</a>",
						uNode,ForeignKey("tNode","cLabel",uNode));
				}
				htmlContainerNotes(uContainer);
				htmlGroups(0,uContainer);
			}
			tContainerNavList(0);
			if(uContainer)
			{
				if( uStatus==1)
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
					printf("<input title='Clone a container to current or another hardware node."
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
				}
				else if( uStatus==6 || uStatus==5 || uStatus==41 )
					printf("<p><input title='Cancel all waiting jobs for this container.'"
					" type=submit class=largeButton"
					" name=gcCommand value='Cancel Job'>\n");
				else if( uStatus==11)
					printf("<p><input title='Creates a job for deploying a new container."
					" Make sure you configure the properties you want beforehand!'"
					" type=submit class=lalertButton"
					" name=gcCommand value='Deploy %.25s'>\n",cLabel);
				else if( uStatus==31 )
					printf("<p><input title='Creates a job for starting a stopped container.'"
					" type=submit class=lalertButton"
					" name=gcCommand value='Start %.25s'>\n",cLabel);
				if( (uStatus==31 || uStatus==11) && !strstr(cLabel,"-clone"))
					printf("<br><input title='If container mount or umount files do not"
					" exist they will be created from tProperty data items: cVEID.mount"
					" and cVEID.umount'"
					" type=submit class=largeButton"
					" name=gcCommand value='Create Mount Files'>\n");
			}
	}
	CloseFieldSet();

}//void ExttContainerButtons(void)


void ExttContainerAuxTable(void)
{

	if(!uContainer || !uStatus) return;

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
			printf("<td width=100 valign=top><a class=darkLink href=unxsVZ.cgi?"
					"gcFunction=tProperty&uProperty=%s&cReturn=tContainer_%u>"
					"%s</a></td><td>%s</td>\n",
						field[0],uContainer,field[1],field[2]);
			printf("</tr>");
		}
		printf("</table>");
	}
	mysql_free_result(res);

	//Special per container buttons
	if((uStatus==1 || uStatus==31) && uAllowDel(uOwner,uCreatedBy))
		printf("<p><input title='Creates a job for stopping and destroying an active container."
			" No backups may be available, the container is gone for good!'"
			" type=submit class=lwarnButton"
			" name=gcCommand value='Destroy %.24s'>\n",cLabel);

	if(uStatus==1 && uAllowDel(uOwner,uCreatedBy))
		printf("<p><input title='Creates a job for stopping an active container."
			" For special administration operations.'"
			" type=submit class=lwarnButton"
			" name=gcCommand value='Stop %.24s'>\n",cLabel);

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
	}
	tContainer("");

}//void ExttContainerGetHook(entry gentries[], int x)


void ExttContainerSelect(void)
{
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
        if(strcmp(gcFilter,"uNode"))
                printf("<option>uNode</option>");
        else
                printf("<option selected>uNode</option>");
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
	if( (uStatus==11 || uStatus==31) && uAllowMod(uOwner,uCreatedBy) )
		printf(LANG_NBB_MODIFY);

	if( uStatus==11 && uAllowDel(uOwner,uCreatedBy) ) 
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttContainerNavBar(void)


void tContainerNavList(unsigned uNode)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(!uNode)
	{
		if(guLoginClient==1 && guPermLevel>11)//Root can read access all
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel,tIP.cLabel FROM tContainer,tNode,tStatus,tIP"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus AND tContainer.uIPv4=tIP.uIP ORDER BY tContainer.cLabel");
		else
			sprintf(gcQuery,"SELECT tContainer.uContainer"
				",tContainer.cLabel,tNode.cLabel,tStatus.cLabel,tIP.cLabel"
				" FROM tContainer," TCLIENT ",tNode,tStatus,tIP"
				" WHERE tContainer.uOwner=" TCLIENT ".uClient"
				" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
				" AND tContainer.uNode=tNode.uNode"
				" AND tContainer.uStatus=tStatus.uStatus AND tContainer.uIPv4=tIP.uIP"
				" ORDER BY tContainer.cLabel",guCompany);
	}
	else
	{
		if(guLoginClient==1 && guPermLevel>11)//Root can read access all
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,"
					"tNode.cLabel,tStatus.cLabel,tIP.cLabel"
					" FROM tContainer,tNode,tStatus,tIP"
					" WHERE tContainer.uNode=tNode.uNode"
					" AND tContainer.uStatus=tStatus.uStatus AND tContainer.uIPv4=tIP.uIP"
						" AND tContainer.uNode=%u ORDER BY tContainer.cLabel",uNode);
		else
			sprintf(gcQuery,"SELECT tContainer.uContainer"
				",tContainer.cLabel,tNode.cLabel,tStatus.cLabel,tIP.cLabel"
				" FROM tContainer," TCLIENT ",tNode,tStatus,tIP"
				" WHERE tContainer.uOwner=" TCLIENT ".uClient"
				" AND tContainer.uNode=%1$u"
				" AND (" TCLIENT ".uClient=%2$u OR " TCLIENT ".uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u OR uClient=%2$u))"
				" AND tContainer.uNode=tNode.uNode"
				" AND tContainer.uStatus=tStatus.uStatus AND tContainer.uIPv4=tIP.uIP"
				" ORDER BY tContainer.cLabel",uNode,guCompany);
	}

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tContainerNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tContainerNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer"
					"&uContainer=%s>%s/%s/%s(%s)</a><br>\n",
						field[0],field[1],field[2],field[3],field[4]);
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
	return(uCount);

}//unsigned StopContainerJob(...)


void CancelContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)
{
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=7 WHERE "
			"uDatacenter=%u AND uNode=%u AND uContainer=%u AND uJobStatus=1"
				,uDatacenter,uNode,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void CancelContainerJob(...)


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
	return(uCount);

}//unsigned MigrateContainerJob(...)


void htmlHealth(unsigned uContainer,unsigned uType)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

	long unsigned luTotalFailcnt=0;
	long unsigned luTotalUsage=0,luTotalSoftLimit=0;
	double fRatio;
	char *cColor={""};
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
		if(sscanf(mysqlField[0],"%lu",&luTotalSoftLimit)==0)
			return;	
	}
	mysql_free_result(mysqlRes);
	if(luTotalSoftLimit==0) luTotalSoftLimit=1;
	fRatio= ((float) luTotalUsage/ (float) luTotalSoftLimit) * 100.00 ;
	if(fRatio<20.00)
		cColor="green";
	else if(fRatio<30.00)
		cColor="teal";
	else if(fRatio<40.00)
		cColor="yellow";
	else if(fRatio<90.00)
		cColor="fuchsia";
	else if(fRatio>=90.00)
		cColor="red";
	printf("Container usage ratio %2.2f%%:"
		" %lu/%lu <font color=%s>[#######]</font><br>\n",
		fRatio,luTotalUsage,luTotalSoftLimit,cColor);

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
	return(uCount);

}//unsigned IPContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)


unsigned MountFilesJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)
{
	unsigned uCount=0;
	char cTemplateName[256]={""};

	GetContainerProp(uContainer,"cVEID.mount",cTemplateName);
	if(!cTemplateName[0]) return(0);
	GetContainerProp(uContainer,"cVEID.umount",cTemplateName);
	if(!cTemplateName[0]) return(0);

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='MountFilesJob(%u)',cJobName='MountFilesContainer'"
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

}//unsigned MountFilesJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)


unsigned CloneContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,
				unsigned uTargetNode, unsigned uNewVeid)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='CloneContainer(%u)',cJobName='CloneContainer'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",cJobData='"
			"uTargetNode=%u;\n"
			"uNewVeid=%u;\n'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				uTargetNode,
				uNewVeid,
				guCompany,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	return(uCount);

}//unsigned CloneContainerJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer)


unsigned CloneNode(unsigned uSourceNode, unsigned uTargetNode, unsigned uWizIPv4)
{

	//Loop for all containers that belong to source node and do not have clones on the uTargetNode.
	//For every loop iteration we must get a new available IP from tIP starting at uWizIPv4
        MYSQL_RES *res;
        MYSQL_RES *res2;
        MYSQL_ROW field;
	unsigned uContainer=0;
	unsigned uNewVeid=0;

	sprintf(gcQuery,"SELECT cLabel,cHostname,uOSTemplate,uConfig,uNameserver,uSearchdomain,uDatacenter"
			",uContainer FROM tContainer WHERE uNode=%u",uSourceNode);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		//Don't clone already cloned container on target node.
		sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uNode=%u AND cLabel='%.25s-clone'",
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

		sprintf(cWizLabel,"%.25s-clone",cLabel);
		sprintf(cWizHostname,"%.93s.clone",cHostname);
		sprintf(gcQuery,"INSERT INTO tContainer SET cLabel='%.25s-clone',"
					"cHostname='%93s.clone',"
					"uIPv4=%u,"
					"uOSTemplate=%s,"
					"uConfig=%s,"
					"uNameserver=%s,"
					"uSearchdomain=%s,"
					"uDatacenter=%s,"
					"uNode=%u,"
					"uStatus=81,"
					"uOwner=%u,"
					"uCreatedBy=%u,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW())",
							field[0],
							field[1],
							uWizIPv4++,
							field[2],
							field[3],
							field[4],
							field[5],
							field[6],
							uTargetNode,
							guCompany,
							guLoginClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		uNewVeid=mysql_insert_id(&gMysql);

		if(CloneContainerJob(uDatacenter,uNode,uContainer,uTargetNode,uNewVeid))
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
				if(uNewVeid)
					return(1);//some containers added.
				else
					return(3);//no containers added, ips not enough.
			}
			
			sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
					",cName='Name',cValue='%.25ss-clone'",
						uNewVeid,field[0]);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			sscanf(field[7],"%u",&uContainer);
			SetContainerStatus(uContainer,81);//Awaiting Clone
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
