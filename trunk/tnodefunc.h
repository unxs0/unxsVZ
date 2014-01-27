/*
FILE
	$Id$
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR/LEGAL
	(C) 2001-2009 Unixservice, LLC. GPLv2 license applies.
 
*/

static unsigned uClone=0;
static unsigned uTargetNode=0;
static char cuTargetNodePullDown[256]={""};
static unsigned uWizIPv4=0;
static char cuWizIPv4PullDown[32]={""};
static char cSearch[32]={""};
static unsigned uTargetDatacenter=0;
static char cuTargetDatacenterPullDown[256]={""};
static unsigned uCloneStop=WARM_CLONE;
static unsigned uSyncPeriod=0;


//ModuleFunctionProtos()
void CopyProperties(unsigned uOldNode,unsigned uNewNode,unsigned uType);
void DelProperties(unsigned uNode,unsigned uType);
void tNodeNavList(unsigned uDataCenter);
void tContainerNavList(unsigned uNode, char *cSearch);//tcontainerfunc.h
void htmlGroups(unsigned uNode, unsigned uContainer);
unsigned FailoverCloneContainer(unsigned uDatacenter, unsigned uNode, unsigned uContainer, unsigned uSource,
			unsigned uSourceNode, unsigned uSourceDatacenter, unsigned uIPv4, unsigned uStatus,
			char *cLabel, char *cHostname,unsigned uOwner,unsigned uDebug);
unsigned CloneNode(unsigned uSourceNode,unsigned uTargetNode,unsigned uWizIPv4,const char *cuWizIPv4PullDown,
			unsigned uSyncPeriod,unsigned uCloneStop,unsigned uTargetDatacenter);
void SetNodeProp(char const *cName,char const *cValue,unsigned uNode);
unsigned ConnectToOptionalUBCDb(unsigned uDatacenter,unsigned uPrivate);
void tNodeHealth(void);

//external
//tcontainerfunc.h
void htmlHealth(unsigned uElement,unsigned uDatacenter,unsigned uType);
//void htmlNodeHealth(unsigned uNode);
char *cRatioColor(float *fRatio);
void SetContainerStatus(unsigned uContainer,unsigned uStatus);
unsigned FailoverToJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,unsigned uOwner,unsigned uLoginClient,unsigned uDebug);
unsigned FailoverFromJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,
				unsigned uIPv4,char *cLabel,char *cHostname,unsigned uSource,
				unsigned uStatus,unsigned uFailToJob,unsigned uOwner,unsigned uLoginClient,unsigned uDebug);
//tcontainer.c
void tTablePullDownAvail(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode);
//tclientfunc.h
static unsigned uForClient=0;
static char cForClientPullDown[256]={"---"};

void ExtProcesstNodeVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.31s",TextAreaSave(entries[i].val));
		else if(!strcmp(entries[i].name,"cForClientPullDown"))
		{
			strcpy(cForClientPullDown,entries[i].val);
			uForClient=ReadPullDown(TCLIENT,"cLabel",cForClientPullDown);
		}
		else if(!strcmp(entries[i].name,"uClone")) 
			uClone=1;
		else if(!strcmp(entries[i].name,"cuTargetNodePullDown"))
		{
			sprintf(cuTargetNodePullDown,"%.255s",entries[i].val);
			uTargetNode=ReadPullDown("tNode","cLabel",cuTargetNodePullDown);
		}
		else if(!strcmp(entries[i].name,"cuWizIPv4PullDown"))
		{
			sprintf(cuWizIPv4PullDown,"%.31s",entries[i].val);
			uWizIPv4=ReadPullDown("tIP","cLabel",cuWizIPv4PullDown);
		}
		else if(!strcmp(entries[i].name,"cuTargetDatacenterPullDown"))
		{
			sprintf(cuTargetDatacenterPullDown,"%.255s",entries[i].val);
			uTargetDatacenter=ReadPullDown("tDatacenter","cLabel",cuTargetDatacenterPullDown);
		}
		else if(!strcmp(entries[i].name,"uCloneStop"))
			sscanf(entries[i].val,"%u",&uCloneStop);
		else if(!strcmp(entries[i].name,"uSyncPeriod"))
			sscanf(entries[i].val,"%u",&uSyncPeriod);
		else if(!strcmp(entries[i].name,"cVendor"))
			sprintf(cVendor,"%.32s",TextAreaSave(entries[i].val));
		else if(!strcmp(entries[i].name,"cPurchaseOrder"))
			sprintf(cPurchaseOrder,"%.64s",TextAreaSave(entries[i].val));
		else if(!strcmp(entries[i].name,"cMACeth0"))
			sprintf(cMACeth0,"%.32s",TextAreaSave(entries[i].val));
		else if(!strcmp(entries[i].name,"cMACeth1"))
			sprintf(cMACeth1,"%.32s",TextAreaSave(entries[i].val));
		else if(!strcmp(entries[i].name,"cOtherName"))
			sprintf(cOtherName,"%.32s",TextAreaSave(entries[i].val));
		else if(!strcmp(entries[i].name,"cProcCPUInfo"))
			sprintf(cProcCPUInfo,"%.32s",TextAreaSave(entries[i].val));
		else if(!strcmp(entries[i].name,"uMaxCloneContainers"))
		{
			sscanf(entries[i].val,"%u",&uMaxCloneContainers);
			sprintf(cuMaxCloneContainers,"%u",uMaxCloneContainers);
		}
		else if(!strcmp(entries[i].name,"uMaxContainers"))
		{
			sscanf(entries[i].val,"%u",&uMaxContainers);
			sprintf(cuMaxContainers,"%u",uMaxContainers);
		}
		else if(!strcmp(entries[i].name,"cNewContainerMode"))
			sprintf(cNewContainerMode,"%.32s",TextAreaSave(entries[i].val));
		else if(!strcmp(entries[i].name,"luInstalledRam"))
		{
			sscanf(entries[i].val,"%lu",&luInstalledRam);
			sprintf(cluInstalledRam,"%lu",luInstalledRam);
		}
		else if(!strcmp(entries[i].name,"luInstalledDiskSpace"))
		{
			sscanf(entries[i].val,"%lu",&luInstalledDiskSpace);
			sprintf(cluInstalledDiskSpace,"%lu",luInstalledDiskSpace);
		}
	}

}//void ExtProcesstNodeVars(pentry entries[], int x)


void ExttNodeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tNodeTools"))
	{
        	MYSQL_RES *res;
		time_t uActualModDate= -1;

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstNodeVars(entries,x);
                        	guMode=2000;
	                        tNode(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
                        	ProcesstNodeVars(entries,x);
				unsigned uOldNode=uNode;

                        	guMode=2000;
				//Check entries here
				if(strlen(cLabel)<3)
					tNode("<blink>Error</blink>: Must supply valid cLabel. Min 3 chars!");
				if(!uDatacenter)
					tNode("<blink>Error</blink>: Must supply valid uDatacenter!");
				sprintf(gcQuery,"SELECT uNode FROM tNode WHERE cLabel='%s'",
						cLabel);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tNode("<blink>Error</blink>: Node cLabel is used!");
				}
                        	guMode=0;

				if(!uForClient)
					uOwner=guCompany;
				else
					uOwner=uForClient;
				uNode=0;
				uCreatedBy=guLoginClient;
				uStatus=1;//Initially active
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtNode(1);//Come back here uNode should be set
				if(!uNode)
					tNode("<blink>Error</blink>: New node was not created!");

				sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=2"
						",cName='Name',cValue='%s',uOwner=%u,uCreatedBy=%u"
						",uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,uNode,cLabel,guCompany,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));

				if(uNode && uClone && uOldNode)
				{
					CopyProperties(uOldNode,uNode,2);
					tNode("New node created and properties copied");
				}
				tNode("New node created");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstNodeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=0;
				sprintf(gcQuery,"SELECT uNode FROM tContainer WHERE uNode=%u",
									uNode);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tNode("<blink>Error</blink>: Can't delete a node"
							" used by a container!");
				}
	                        guMode=2001;
				tNode(LANG_NB_CONFIRMDEL);
			}
			else
				tNode("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstNodeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				sscanf(ForeignKey("tNode","uModDate",uNode),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tNode("<blink>Error</blink>: This record was modified. Reload it.");

				sprintf(gcQuery,"SELECT uNode FROM tContainer WHERE uNode=%u",
									uNode);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tNode("<blink>Error</blink>: Can't delete a node"
							" used by a container!");
				}
	                        guMode=0;
				DelProperties(uNode,2);
				DeletetNode();
			}
			else
				tNode("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstNodeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tNode(LANG_NB_CONFIRMMOD);
			}
			else
				tNode("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstNodeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				sscanf(ForeignKey("tNode","uModDate",uNode),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tNode("<blink>Error</blink>: This record was modified. Reload it.");
				if(strlen(cLabel)<3)
					tNode("<blink>Error</blink>: Must supply valid cLabel. Min 3 chars!");
				if(!uDatacenter)
					tNode("<blink>Error</blink>: Must supply valid uDatacenter!");
                        	guMode=0;

				if(uForClient)
				{
					sprintf(gcQuery,"UPDATE tNode SET uOwner=%u WHERE uNode=%u",
						uForClient,uNode);
					mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
                				htmlPlainTextError(mysql_error(&gMysql));
					uOwner=uForClient;
				}
				uModBy=guLoginClient;
				ModtNode();
			}
			else
				tNode("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,"Node Container Report"))
                {
                        ProcesstNodeVars(entries,x);
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tNode","uModDate",uNode),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tNode("<blink>Error</blink>: This record was modified. Reload it.");
				
				guMode=9001;
				tNode("");
			}
			else if(uAllowMod(uOwner,uCreatedBy))
			{
				tNode("<blink>Error</blink>: Denied by node status");
			}
			else
			{
				tNode("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Hardware Information"))
                {
                        ProcesstNodeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) || guPermLevel>=6)
			{
				guMode=10000;
				tNode("Hardware inventory information");
			}
			else
			{
				tNode("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Save Hardware Information"))
                {
                        ProcesstNodeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) || guPermLevel>=6)
			{
				guMode=10000;
				//check
				if(!uNode)
					tNode("uNode not specified");
				if(!cVendor[0])
					tNode("cVendor not specified");
				if(!cPurchaseOrder[0])
					tNode("cPurchaseOrder not specified");
				if(!cProcCPUInfo[0])
					tNode("cProcCPUInfo not specified");
				if(!cMACeth0[0])
					tNode("cMACeth0 not specified");
				if(!cMACeth1[0])
					tNode("cMACeth1 not specified");
				if(!cluInstalledDiskSpace[0])
					tNode("cluInstalledDiskSpace not specified");
				if(!cluInstalledRam[0])
					tNode("cluInstalledRam not specified");
				if(!cuMaxContainers[0])
					tNode("cuMaxContainers not specified");
				if(!cNewContainerMode[0])
					tNode("cNewContainerMode not specified");

				SetNodeProp("cVendor",cVendor,uNode);
				SetNodeProp("cPurchaseOrder",cPurchaseOrder,uNode);
				SetNodeProp("cProcCPUInfo",cProcCPUInfo,uNode);
				SetNodeProp("cMACeth0",cMACeth0,uNode);
				SetNodeProp("cMACeth1",cMACeth1,uNode);
				SetNodeProp("cOtherName",cOtherName,uNode);
				SetNodeProp("luInstalledDiskSpace",cluInstalledDiskSpace,uNode);
				SetNodeProp("luInstalledRam",cluInstalledRam,uNode);
				SetNodeProp("MaxCloneContainers",cuMaxCloneContainers,uNode);
				SetNodeProp("MaxContainers",cuMaxContainers,uNode);
				SetNodeProp("NewContainerMode",cNewContainerMode,uNode);
				guMode=10001;
				tNode("Hardware inventory information saved");
			}
			else
			{
				tNode("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Clone Node Wizard"))
                {
                        ProcesstNodeVars(entries,x);
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tNode","uModDate",uNode),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tNode("<blink>Error</blink>: This record was modified. Reload it.");
				
				guMode=7001;
				tNode("Select datacenter");
			}
			else if(uAllowMod(uOwner,uCreatedBy))
			{
				tNode("<blink>Error</blink>: Denied by node status");
			}
			else
			{
				tNode("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Select Clone Datacenter"))
                {
                        ProcesstNodeVars(entries,x);
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tNode","uModDate",uNode),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tNode("<blink>Error</blink>: This record was modified. Reload it.");

				guMode=7001;
				if(!uTargetDatacenter)
					tNode("<blink>Error:</blink> You must select a datacenter");
				
				guMode=7002;
				tNode("Select node, uIPv4 and more");
			}
			else if(uAllowMod(uOwner,uCreatedBy))
			{
				tNode("<blink>Error</blink>: Denied by node status");
			}
			else
			{
				tNode("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Node Clone"))
                {
                        ProcesstNodeVars(entries,x);
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
			{
				char cTargetNodeIPv4[32]={""};
				unsigned uRetVal=0;
				unsigned uIPv4Datacenter=0;

                        	guMode=0;

				sscanf(ForeignKey("tNode","uModDate",uNode),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tNode("<blink>Error</blink>: This record was modified. Reload it.");

                        	guMode=7002;
				//tNode("Function not available. Try again later");

				if(!uWizIPv4)
					tNode("<blink>Error</blink>: You must select a start IP!");
				if(uTargetNode==0)
					tNode("<blink>Error</blink>: Please select a valid target node!");
				if(uTargetNode==uNode)
					tNode("<blink>Error</blink>: Can't clone to same node!");
				GetNodeProp(uTargetNode,"cIPv4",cTargetNodeIPv4);
				if(!cTargetNodeIPv4[0])
					tNode("<blink>Error</blink>: Your target node is"
							" missing it's cIPv4 property!");
				if(uCloneStop>COLD_CLONE || uCloneStop<HOT_CLONE)
					tNode("<blink>Error:</blink> Unexpected initial state");
				sscanf(ForeignKey("tIP","uDatacenter",uWizIPv4),"%u",&uIPv4Datacenter);
				if(uTargetDatacenter!=uIPv4Datacenter)
					tNode("<blink>Error:</blink> The specified target uIPv4 does not "
							"belong to the specified uDatacenter.");
				if(!uDatacenter || !uTargetDatacenter )
					tNode("<blink>Error:</blink> Unexpected problem with missing source node"
							" settings!");
				if(uSyncPeriod>86400*30 || (uSyncPeriod && uSyncPeriod<300))
						tNode("<blink>Error:</blink> Clone uSyncPeriod seconds out of range:"
								" Max 30 days, min 5 minutes or 0 off.");
                        	guMode=0;

				
				uRetVal=CloneNode(uNode,uTargetNode,uWizIPv4,cuWizIPv4PullDown,uSyncPeriod,uCloneStop,
							uTargetDatacenter);
				if(uRetVal==5)
					tNode("<blink>Operation not completed</blink>: Not enough IPs are available");
				else if(uRetVal==2)
					tNode("<blink>Error</blink>: Unexpected CommonCloneContainer() error! Check tJob");
				else if(uRetVal)
					tNode("<blink>Error</blink>: Unexpected CloneNode() error! Check tJob");
				else if(!uRetVal)
					tNode("Clone node container jobs created");
					

			}
		}
                else if(!strcmp(gcCommand,"Failover Node Wizard"))
                {
                        ProcesstNodeVars(entries,x);
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tNode","uModDate",uNode),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tNode("<blink>Error</blink>: This record was modified. Reload it.");
				
				guMode=8001;
				tNode("Candiate failover jobs listed below, double check.");
			}
			else
			{
				tNode("<blink>Error</blink>: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Node Failover"))
                {
                        ProcesstNodeVars(entries,x);
			if(uStatus==1 && uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;

				sscanf(ForeignKey("tNode","uModDate",uNode),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tNode("<blink>Error</blink>: This record was modified. Reload it.");

                        	guMode=8002;

				tNode("For failover jobs created see clone panel below");

			}
		}
	}

}//void ExttNodeCommands(pentry entries[], int x)


void ExttNodeButtons(void)
{
	switch(guMode)
        {
		//Hardware Information
                case 10000:
                case 10001:
			OpenFieldSet("tNode Hardware Panel",100);
			if(cLabel[0] && uNode)
				printf("Back to tNode <a href=unxsVZ.cgi?gcFunction=tNode"
					"&uNode=%u>%s</a><br>\n",uNode,cLabel);
                        printf("<p><u>Hardare Inventory Operations</u><br>");
			printf("<p><input title='Commit hardware data database'"
					" type=submit class=largeButton"
					" name=gcCommand value='Save Hardware Information'>\n");
			printf("<br><input type=submit class=largeButton title='Hardware inventory information and data entry for selected node.'"
					" name=gcCommand value='Hardware Information'>");
                break;

                case 9001:
			OpenFieldSet("tNode Aux Panel",100);
                        printf("<p><u>Node Container Report</u><br>");
			printf("See bottom panel.");
                break;

                case 7001:
			OpenFieldSet("tNode Aux Panel",100);
                        printf("<p><u>Node Clone Wizard (Step 1/2)</u><br>");
			printf("Here you will select the datacenter. If it is oversubscribed or not"
				" configured for use, or otherwise unavailable you will have to select another.");
			printf("<p>Select datacenter<br>");
			tTablePullDown("tDatacenter;cuTargetDatacenterPullDown","cLabel","cLabel",uTargetDatacenter,1);
			printf("<p><input title='Step one of remote clone wizard'"
					" type=submit class=largeButton"
					" name=gcCommand value='Select Clone Datacenter'>\n");
                break;

                case 7002:
			OpenFieldSet("tNode Aux Panel",100);
                        printf("<p><u>Node Clone Wizard (Step 2/2)</u><br>");
			printf("Here you will select the hardware node target. If the selected node is"
				" oversubscribed, not available, or scheduled for maintenance. You will"
				" be informed at the next step.\n<p>\n"
				"You must also select a start uIPv4 for the cloned containers, and set the initial"
				" state of the clone."
				" Usually clones should be kept stopped to conserve resources and facilitate rsync."
				" Use the checkbox to change this default behavior."
				" Any mount/umount files of the source container will NOT be used"
				" by the new cloned container. This issue will be left for manual"
				" or automated failover to the cloned container.<p>If you wish to"
				" keep the source and clone container sync'd you can specify a non zero"
				" value via the 'cuSyncPeriod' entry below.");
			printf("<p>Selected datacenter<br>");
			tTablePullDown("tDatacenter;cuTargetDatacenterPullDown","cLabel","cLabel",uTargetDatacenter,0);
			printf("<p>Select target node<br>");
			tTablePullDownDatacenter("tNode;cuTargetNodePullDown","cLabel","cLabel",uTargetNode,1,
				cuTargetNodePullDown,0,uTargetDatacenter);//0 does not use tProperty, uses uDatacenter
			printf("<p>Select start uIPv4<br>");
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
			printf("<p><input title='Create a clone job for the current container'"
					" type=submit class=largeButton"
					" name=gcCommand value='Confirm Node Clone'>\n");
                break;

                case 8001:
			OpenFieldSet("tNode Aux Panel",100);
                        printf("<p><u>Failover Node Wizard</u><br>");
			printf("The current node should be down (or offline.)"
				" Other nodes should have \"-clone\" (uSource!=0) containers that have been kept updated."
				" The candidate containers are presented below for your approval.<p>");
			printf("<p><input title='Create failover jobs for all the current node`s master containers"
					" and for other node`s corresponding clone container`s'"
					" type=submit class=lwarnButton"
					" name=gcCommand value='Confirm Node Failover'>\n");
                break;

                case 8002:
			OpenFieldSet("tNode Aux Panel",100);
                        printf("<p><u>Failover Node Wizard</u><br>");
			printf("Failover jobs have been attempted to be created see panel below for results."
				" See <a href=unxsVZ.cgi?gcFunction=tJob>tJob</a> for details.<p>");
                break;

                case 2000:
			OpenFieldSet("tNode Aux Panel",100);
			printf("<p><u>Enter/mod data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
			if(uNode)
				printf("<p><input title='Copies all properties'"
					" type=checkbox name=uClone checked> Copy properties from property panel below.\n");
			if(guPermLevel>11)
				tTablePullDownResellers(uForClient,1);
                break;

                case 2001:
			OpenFieldSet("tNode Aux Panel",100);
                        printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			OpenFieldSet("tNode Aux Panel",100);
			printf("<p><u>Review changes</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
			if(guPermLevel>11)
			{
				printf("<p>To change the record owner, just...");
				tTablePullDownResellers(guCompany,1);
			}
                break;

		default:
			OpenFieldSet("tNode Aux Panel",100);
			printf("<u>Table Tips</u><br>");
			printf("Hardware nodes are defined here. Hardware nodes host containers, and allow"
				" for the autonomic migration to other nodes that may be better suited"
				" at specific points in time to accomplish QoS or other system admin"
				" created policies herein. uVeth='Yes' container traffic is not included"
				"in the node graphs at this time.");
			if(uNode && uStatus==1)
				printf("<br><input type=submit class=largeButton title='Hardware inventory information and data entry for selected node.'"
					" name=gcCommand value='Hardware Information'>");
			printf("<p><u>Record Context Info</u><br>");
			if(uDatacenter && uNode)
			{
				printf("Node belongs to ");
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tDatacenter&uDatacenter=%u>%s</a>",
						uDatacenter,ForeignKey("tDatacenter","cLabel",uDatacenter));
				htmlGroups(uNode,0);
			}
			printf("<p><u>Node Container Search</u><br>");
			printf("<input title='Enter pattern then click on node link. Use [Search], enter cLabel start or MySQL LIKE pattern"
					" (%% or _ allowed) to show a navgation list with containers that match pattern for a given node.'"
					" type=text name=cSearch value='%s'>",cSearch);
			tNodeNavList(0);
			tContainerNavList(uNode,cSearch);
			if(uNode)
			{
				tNodeHealth();
				//htmlHealth(uNode,uDatacenter,2);
				//hide health if node selected via get
				//if(guMode!=6)
				//	htmlNodeHealth(uNode);
				printf("<p><input type=submit class=largeButton title='Display node container"
					" report'"
					" name=gcCommand value='Node Container Report'><br>");
				printf("<p><input type=submit class=lwarnButton title='Clone all containers on this clone to another node.'"
					" name=gcCommand value='Clone Node Wizard'><br>");
				printf("<input type=submit class=lwarnButton title='Failover to this node`s clone containers"
					" the master containers`s of down node.'"
					" name=gcCommand value='Failover Node Wizard'><br>");
			}
	}
	CloseFieldSet();

}//void ExttNodeButtons(void)


//UBC work required
void ExttNodeAuxTable(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	switch(guMode)
	{
		//Node Container Report
		case 9001:
		{
			//UBC support
			char cLogfile[64]={"/tmp/unxsvzlog"};
			if(gLfp==NULL)
			{
				if( (gLfp=fopen(cLogfile,"a"))==NULL)
                			tContainer("Could not open logfile");
			}
			if(uDatacenter && ConnectToOptionalUBCDb(uDatacenter,0))
			{
				printf("<p>ConnectToOptionalUBCDb() error<p>");
				return;
			}
			if(gcUBCDBIP0==DBIP0 && gcUBCDBIP1==DBIP1)
				gMysqlUBC=gMysql;

				sprintf(gcQuery,"Distributed UBC %s Property Panel",cLabel);
			sprintf(gcQuery,"Non clone %s Containers",cLabel);
			OpenFieldSet(gcQuery,100);
			printf("<table>");
			printf("<tr>"
				//"<td><u>select</u></td>"
				"<td><u>master label</u></td>"
				"<td><u>master hostname</u></td>"
				"<td><u>template</u></td>"
				"<td><u>status</u></td>"
				"<td><u>clone label</u></td>"
				"<td><u>clone hostname</u></td>"
				"<td><u>seconds since rsync</u></td>"
				"</tr>");
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cLabel,tContainer.cHostname,"
					"tContainer.uNode,tContainer.uDatacenter,tOSTemplate.cLabel,tStatus.cLabel"
					" FROM tContainer,tOSTemplate,tStatus"
					" WHERE tContainer.uNode=%u"
					" AND tContainer.uOSTemplate=tOSTemplate.uOSTemplate"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uSource=0"
					" ORDER BY tContainer.cLabel",uNode);
		        mysql_query(&gMysql,gcQuery);
		        if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		        res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res))
			{
				MYSQL_RES *res2;
				MYSQL_ROW field2;
				long unsigned luTotalDiskSpace=0;
				long unsigned luDiskSpace;

				while((field=mysql_fetch_row(res)))
				{
					printf(	"<tr>"
						//"<td><input type=checkbox name=CNW%s</td>"
						"<td>"
						"<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s</a>"
						"</td>"
						"<td>%s</td>"
						"<td>%s</td>"
						"<td>%s</td>",
							//field[0],
							field[0],field[1],field[2],field[5],field[6]);
					sprintf(gcQuery,"SELECT uContainer,cLabel,cHostname,(UNIX_TIMESTAMP(NOW())-uBackupDate),"
							"uDatacenter,uNode,uIPv4,uStatus,uOwner"
							" FROM tContainer WHERE uSource=%s",field[0]);
				        mysql_query(&gMysql,gcQuery);
				        if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				        res2=mysql_store_result(&gMysql);
					if((field2=mysql_fetch_row(res2)))
					{
						printf("<td><a class=darkLink href=unxsVZ.cgi?"
							"gcFunction=tContainer&uContainer=%s>"
							"%s</a></td><td>%s</td><td>%s</td>",
								field2[0],field2[1],field2[2],field2[3]);
					}
					else
					{
						printf("<td>---</a></td><td>---</td><td>---</td>");
					}
					mysql_free_result(res2);
					printf("</tr>\n");

					sprintf(gcQuery,"SELECT cValue FROM tProperty"
							" WHERE cName='1k-blocks.luUsage' AND uType=3 AND uKey=%s",field[0]);
				        mysql_query(&gMysqlUBC,gcQuery);
				        if(mysql_errno(&gMysqlUBC))
						htmlPlainTextError(mysql_error(&gMysqlUBC));
				        res2=mysql_store_result(&gMysqlUBC);
					if((field2=mysql_fetch_row(res2)))
					{
						luDiskSpace=0;
						sscanf(field2[0],"%lu",&luDiskSpace);
						luTotalDiskSpace+=luDiskSpace;
					}
					mysql_free_result(res2);

				}
				printf("<tr><td>Total disk space used: %lu</td></tr>",luTotalDiskSpace);
			}
			printf("</table>");
			CloseFieldSet();
		}
		break;

		//Hardware Information
		case 10000:
		case 10001:
			OpenFieldSet("Hardware Inventory",100);
			printf("<table>");
			printf("<tr>"
				"<td width=100><u>Node</u></td>"
				"<td width=200><u>cName</u></td>"
				"<td width=400><u>cValue</u></td>"
				"</tr>");
			sprintf(gcQuery,"SELECT tNode.cLabel,tProperty.cName,tProperty.cValue,tNode.uNode,tProperty.uProperty"
						" FROM tNode,tProperty"
						" WHERE tProperty.uKey=tNode.uNode AND tProperty.uType=2"
						" AND tNode.uStatus=1"
						" AND ("
						" tProperty.cName LIKE 'c%%' OR tProperty.cName LIKE 'Max%%'"
						" OR tProperty.cName LIKE 'luInstalled%%'"
						" OR tProperty.cName='Name'"
						" OR tProperty.cName='NewContainerMode'"
						" OR tProperty.cName LIKE 'Max%%Containers'"
						")"
						" AND tNode.cLabel!='appliance'"
						" ORDER BY tNode.uNode,tProperty.cName");
		        mysql_query(&gMysql,gcQuery);
		        if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		        res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res))
			{
				char cPrevLabel[32]={""};
				char cLabel[32]={""};
				while((field=mysql_fetch_row(res)))
				{
					sprintf(cLabel,"%.31s",field[0]);
					if(strcmp(cPrevLabel,field[0]))
						sprintf(cPrevLabel,"%.31s",field[0]);
					else
						cLabel[0]=0;
					printf("<tr>"
						"<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tNode&uNode=%s&uHardware>%s</a></td>"
						"<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tProperty&uProperty=%s>%s</a></td>"
						"<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tProperty&uProperty=%s>%s</a></td>"
						"</tr>",field[3],cLabel,field[4],field[1],field[4],field[2]);
				}
			}
			mysql_free_result(res);
			printf("</table>");
			CloseFieldSet();
		break;

		case 8001:
		case 8002:
			sprintf(gcQuery,"%s Clone Panel",cLabel);
			OpenFieldSet(gcQuery,100);
			printf("<table>");
			printf("<tr>"
				"<td><u>master label</u></td>"
				"<td><u>master hostname</u></td>"
				"<td><u>clone label</u></td>"
				"<td><u>clone hostname</u></td>"
				"<td><u>seconds since rsync</u></td>"
				"<td><u>job created</u></td>"
				"</tr>");
			sprintf(gcQuery,"SELECT uContainer,cLabel,cHostname,uNode,uDatacenter FROM tContainer WHERE"
							" uNode=%u AND uSource=0 AND uStatus=%u ORDER BY cLabel",uNode,uACTIVE);
		        mysql_query(&gMysql,gcQuery);
		        if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		        res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res))
			{
				MYSQL_RES *res2;
				MYSQL_ROW field2;

				while((field=mysql_fetch_row(res)))
				{
					printf("<tr>");
					printf("<td><a class=darkLink href=unxsVZ.cgi?"
							"gcFunction=tContainer&uContainer=%s>"
							"%s</a></td><td>%s</td>",
								field[0],field[1],field[2]);
					sprintf(gcQuery,"SELECT uContainer,cLabel,cHostname,(UNIX_TIMESTAMP(NOW())-uBackupDate),"
							"uDatacenter,uNode,uIPv4,uStatus,uOwner"
							" FROM tContainer WHERE uSource=%s",field[0]);
				        mysql_query(&gMysql,gcQuery);
				        if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				        res2=mysql_store_result(&gMysql);
					if((field2=mysql_fetch_row(res2)))
					{
						printf("<td><a class=darkLink href=unxsVZ.cgi?"
							"gcFunction=tContainer&uContainer=%s>"
							"%s</a></td><td>%s</td><td>%s</td>",
								field2[0],field2[1],field2[2],field2[3]);
						if(guMode==8002)
						{ 
							unsigned uRetVal=0;

							unsigned uDatacenter=0;
							unsigned uNode=0;
							unsigned uContainer=0;
							unsigned uIPv4=0;
							unsigned uStatus=0;

							unsigned uSource=0;
							unsigned uSourceNode=0;
							unsigned uSourceDatacenter=0;

							sscanf(field2[0],"%u",&uContainer);
							sscanf(field2[4],"%u",&uDatacenter);
							sscanf(field2[5],"%u",&uNode);
							sscanf(field2[6],"%u",&uIPv4);
							sscanf(field2[7],"%u",&uStatus);

							//TODO note file scope global
							//Used in tcontainerfunc failover funcs.
							sscanf(field2[8],"%u",&uOwner);

							sscanf(field[0],"%u",&uSource);
							sscanf(field[3],"%u",&uSourceNode);
							sscanf(field[4],"%u",&uSourceDatacenter);

							uRetVal=FailoverCloneContainer(uDatacenter,uNode,uContainer,uSource,
								uSourceNode,uSourceDatacenter,uIPv4,uStatus,field2[1],field2[2],
								uOwner,1);//1 debug on
							if(uRetVal==0)
								printf("<td>X</td>");
							else if(uRetVal==1)
								printf("<td>!</td>");
						}
					}
					mysql_free_result(res2);
					printf("</tr>\n");
				}
			}
			printf("</table>");
			CloseFieldSet();
		break;

		default:
			if(!uNode || guMode!=6) return;

			sprintf(gcQuery,"Global %s Property Panel",cLabel);
			OpenFieldSet(gcQuery,100);

			sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE"
							" uKey=%u AND uType=2 ORDER BY cName",uNode);
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
							"gcFunction=tProperty&uProperty=%s&cReturn=tNode_%u>"
							"%s</a></td><td>%s</td>\n",
								field[0],uNode,field[1],field[2]);
					printf("</tr>");
				}
				printf("</table>");
			}
			CloseFieldSet();


			char cLogfile[64]={"/tmp/unxsvzlog"};
			if(gLfp==NULL)
			{
				if( (gLfp=fopen(cLogfile,"a"))==NULL)
                			tContainer("Could not open logfile");
			}
			if(uDatacenter && ConnectToOptionalUBCDb(uDatacenter,0))
			{
				printf("<p>ConnectToOptionalUBCDb() error<p>");
				return;
			}
			if(gcUBCDBIP0!=DBIP0 || gcUBCDBIP1!=DBIP1)
			{
				sprintf(gcQuery,"Distributed UBC %s Property Panel",cLabel);
				OpenFieldSet(gcQuery,100);
				//UBC safe
				sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty"
						" WHERE uKey=%u AND uType=2 ORDER BY cName",uNode);
		        	mysql_query(&gMysqlUBC,gcQuery);
		        	if(mysql_errno(&gMysqlUBC))
					htmlPlainTextError(mysql_error(&gMysqlUBC));
		        	res=mysql_store_result(&gMysqlUBC);
				if(mysql_num_rows(res))
				{
					printf("<table>");
					while((field=mysql_fetch_row(res)))
					{
						printf("<tr>");
						printf("<td width=200 valign=top><a class=darkLink href=unxsVZ.cgi?"
						"gcFunction=tProperty&uProperty=%s&cReturn=tNode_%u&cuDatacenterSelect=%u>"
						"%s</a></td><td>%s</td>\n",
							field[0],uNode,uDatacenter,field[1],field[2]);
						printf("</tr>");
					}
					printf("</table>");
				}
				mysql_free_result(res);
				CloseFieldSet();
			}
	}

}//void ExttNodeAuxTable(void)


void ExttNodeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uNode"))
		{
			sscanf(gentries[i].val,"%u",&uNode);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
			sprintf(cSearch,"%.31s",gentries[i].val);
		else if(!strcmp(gentries[i].name,"uHardware"))
			guMode=10000;
	}
	tNode("");

}//void ExttNodeGetHook(entry gentries[], int x)


void ExttNodeSelect(void)
{
	ExtSelectPublic("tNode",VAR_LIST_tNode);

}//void ExttNodeSelect(void)


void ExttNodeSelectRow(void)
{
	ExtSelectRowPublic("tNode",VAR_LIST_tNode,uNode);

}//void ExttNodeSelectRow(void)


void ExttNodeListSelect(void)
{
	char cCat[512];

	ExtListSelectPublic("tNode",VAR_LIST_tNode);
	
	//Changes here must be reflected below in ExttNodeListFilter()
        if(!strcmp(gcFilter,"uNode"))
        {
                sscanf(gcCommand,"%u",&uNode);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tNode.uNode=%u ORDER BY uNode",uNode);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uStatus"))
        {
                sscanf(gcCommand,"%u",&uStatus);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tNode.uStatus=%u ORDER BY uNode",uStatus);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uNode");
        }

}//void ExttNodeListSelect(void)


void ExttNodeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select title='Optionally filter results. E.g. uStatus=3 is offline' name=gcFilter>");
        if(strcmp(gcFilter,"uNode"))
                printf("<option>uNode</option>");
        else
                printf("<option selected>uNode</option>");
        if(strcmp(gcFilter,"uStatus"))
                printf("<option>uStatus</option>");
        else
                printf("<option selected>uStatus</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttNodeListFilter(void)


void ExttNodeNavBar(void)
{
	printf(LANG_NBB_SEARCH);
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);

	if(guMode<10000)
	{
		if(guPermLevel>=9 && !guListMode)
			printf(LANG_NBB_NEW);

		if(uAllowMod(uOwner,uCreatedBy))
			printf(LANG_NBB_MODIFY);

		if(uAllowDel(uOwner,uCreatedBy))
			printf(LANG_NBB_DELETE);
	}

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttNodeNavBar(void)


//Nodes are shared infrastructure
void tNodeNavList(unsigned uDatacenter)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uMysqlNumRows;
	unsigned uNumRows=0;
#define LIMIT " LIMIT 33"
#define uLIMIT 32

	if(uDatacenter)
		sprintf(gcQuery,"SELECT uNode,cLabel FROM tNode WHERE uDatacenter=%u AND uStatus=1"
				" ORDER BY cLabel" 
				LIMIT,uDatacenter);
	else
		sprintf(gcQuery,"SELECT uNode,cLabel FROM tNode WHERE uStatus=1 ORDER BY cLabel" 
				LIMIT);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tNodeNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if((uMysqlNumRows=mysql_num_rows(res)))
	{	
        	printf("<p><u>Active tNodeNavList(%u)</u><br>\n",uMysqlNumRows);

	        while((field=mysql_fetch_row(res)))
		{

			
			if(cSearch[0])
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tNode"
					"&uNode=%s&cSearch=%s>%s</a><br>\n",
							field[0],cURLEncode(cSearch),field[1]);
			else
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tNode"
					"&uNode=%s>%s</a><br>\n",
							field[0],field[1]);

			if(++uNumRows>=uLIMIT)
			{
				printf("Only %u of %u nodes shown use [List] to view all.<br>\n",
						uLIMIT,uMysqlNumRows);
				break;
			}
		}
	}
        mysql_free_result(res);

}//void tNodeNavList()



//UBC safe
void CopyProperties(unsigned uOldNode,unsigned uNewNode,unsigned uType)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uOldNode==0 || uNewNode==0 || uType==0) return;

	sprintf(gcQuery,"SELECT cName,cValue FROM tProperty WHERE uKey=%u AND uType=%u",
					uOldNode,uType);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,cName='%s',cValue='%s',uType=%u,uOwner=%u,"
				"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uNewNode,
					field[0],TextAreaSave(field[1]),
					uType,
					guLoginClient,guLoginClient);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

}//void CopyProperties(uNode,uNewNode)


//UBC safe will not del UBCs if remote
void DelProperties(unsigned uNode,unsigned uType)
{
	if(uNode==0 || uType==0) return;
	sprintf(gcQuery,"DELETE FROM tProperty WHERE uType=%u AND uKey=%u",
				uType,uNode);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void DelProperties()


//UBC work required
void htmlNodeHealth(unsigned uNode)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char *cColor;
	float fRatio;
	long unsigned luAllContainerPhyspages=0;
	long unsigned luAllContainer=0;
	long unsigned luTotalRAM=0;
	long unsigned luInstalledRam=0;
	char cluInstalledRam[256];
	char cValue[256];

	GetNodeProp(uNode,"luInstalledRam",cluInstalledRam);
	sscanf(cluInstalledRam,"%lu",&luInstalledRam);
	if(luInstalledRam==0) luInstalledRam=1;


	//printf("<u>Critical Node Usage Data</u><br>\n");

	//1-. (privvmpages exceeds RAM )
	//Check fundamental memory constraints of containers per node:
	//The memory gap between privvmpages and the two resource guarantees (vmguarpages and
	// oomguarpages) is not safe to use in an ongoing basis if the sum of all container
	// privvmpages exceeds RAM + swap of the hardware node.
	sprintf(gcQuery,"SELECT SUM(CONVERT(tProperty.cValue,UNSIGNED)) FROM tProperty,tContainer WHERE"
				" tProperty.cName='privvmpages.luMaxheld'"
				" AND tProperty.uType=3"
				" AND tProperty.uKey=tContainer.uContainer"
				" AND tContainer.uStatus!=11"//Probably active not initial setup
				" AND tContainer.uStatus!=31"// and not stopped
				" AND tContainer.uNode=%u",uNode);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		printf("%s",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		long unsigned luContainerPrivvmpagesMaxHeld=0;

		if(field[0]==NULL)
		{
			printf("No data available, no active containers?<br>\n");
			goto NextSection;
		}

		sscanf(field[0],"%lu",&luContainerPrivvmpagesMaxHeld);
		fRatio= ((float) luContainerPrivvmpagesMaxHeld/ (float) luInstalledRam) * 100.00 ;
		cColor=cRatioColor(&fRatio);
		printf("Max held privvmpages ratio %2.2f%%:"
			" <font color=%s>%lu/%lu</font><br>\n",
				fRatio,cColor,luContainerPrivvmpagesMaxHeld,luInstalledRam);
		sprintf(cValue,"%2.2f%%",fRatio);
		SetNodeProp("cMaxHeldVmpagesRatio",cValue,uNode);
	}
NextSection:
	mysql_free_result(res);


	//2-.
	//Inform if hardware node is overcommitted: 1-. cpu power
	sprintf(gcQuery,"SELECT SUM(CONVERT(tProperty.cValue,UNSIGNED)) FROM tProperty,tContainer WHERE"
				" tProperty.cName='vzcpucheck.fCPUUnits'"
				" AND tProperty.uType=3"
				" AND tProperty.uKey=tContainer.uContainer"
				" AND tContainer.uStatus!=11"//Probably active not initial setup
				" AND tContainer.uStatus!=31"// and not stopped
				" AND tContainer.uNode=%u",uNode);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		printf("%s",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		float fNodeCPUUnits=1.0;
		char cfNodeCPUUnits[256];
		float fAllContainerCPUUnits=0.0;

		if(field[0]==NULL)
		{
			printf("No power data available<br>\n");
			goto NextSection2;
		}

		GetNodeProp(uNode,"vzcpucheck-nodepwr.fCPUUnits",cfNodeCPUUnits);
		sscanf(cfNodeCPUUnits,"%f",&fNodeCPUUnits);
		sscanf(field[0],"%f",&fAllContainerCPUUnits);
		fRatio= (fAllContainerCPUUnits/fNodeCPUUnits) * 100.00 ;
		cColor=cRatioColor(&fRatio);
		printf("Container/Node power %2.2f%%:"
			" <font color=%s>%2.0f/%2.0f</font><br>\n",
				fRatio,cColor,fAllContainerCPUUnits,fNodeCPUUnits);
		sprintf(cValue,"%2.2f%%",fRatio);
		SetNodeProp("cNodePowerRatio",cValue,uNode);
	}
NextSection2:
	mysql_free_result(res);

	//3-.
	//Total RAM utilization http://wiki.openvz.org/UBC_systemwide_configuration
	//3a-. All container pages times 4096
	sprintf(gcQuery,"SELECT SUM(CONVERT(tProperty.cValue,UNSIGNED)*4096) FROM tProperty,tContainer WHERE"
				" tProperty.cName='physpages.luMaxheld'"
				" AND tProperty.uType=3"//Container type
				" AND tProperty.uKey=tContainer.uContainer"
				" AND tContainer.uStatus!=11"//uINITSETUP==11
				" AND tContainer.uStatus!=31"//uSTOPPED==31
				" AND tContainer.uNode=%u",uNode);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		printf("%s",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(field[0]!=NULL)
			sscanf(field[0],"%lu",&luAllContainerPhyspages);
	}
	mysql_free_result(res);
	//3b-. All container kmemsize + othersockbuf.luMaxheld + tcpsndbuf.luMaxheld + tcprcvbuf.luMaxheld +
	// dgramrcvbuf.luMaxheld
	//physpages.luHeld * 4096 needs to also be added from above
	sprintf(gcQuery,"SELECT SUM(CONVERT(tProperty.cValue,UNSIGNED)) FROM tProperty,tContainer WHERE"
				" ( tProperty.cName='kmemsize.luMaxheld' OR"
				" tProperty.cName='othersockbuf.luMaxheld' OR "
				" tProperty.cName='tcpsndbuf.luMaxheld' OR "
				" tProperty.cName='tcprcvbuf.luMaxheld' OR "
				" tProperty.cName='dgramrcvbuf.luMaxheld' )"
				//" ( tProperty.cName='kmemsize.luHeld' OR"
				//" tProperty.cName='othersockbuf.luHeld' OR "
				//" tProperty.cName='tcpsndbuf.luHeld' OR "
				//" tProperty.cName='tcprcvbuf.luHeld' OR "
				//" tProperty.cName='dgramrcvbuf.luHeld' )"
				" AND tProperty.uType=3"
				" AND tProperty.uKey=tContainer.uContainer"
				" AND tContainer.uStatus!=11"//Probably active not initial setup
				" AND tContainer.uStatus!=31"// and not stopped
				" AND tContainer.uNode=%u",uNode);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		printf("%s",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(field[0]!=NULL)
			sscanf(field[0],"%lu",&luAllContainer);
	}
	mysql_free_result(res);
	luTotalRAM=(luAllContainerPhyspages+luAllContainer)/1000;
	fRatio= ((float)luTotalRAM/(float)luInstalledRam) * 100.00;
		cColor=cRatioColor(&fRatio);
		printf("Max RAM Util %2.2f%%:"
			" <font color=%s>%lu/%lu</font><br>\n",
				fRatio,cColor,luTotalRAM,luInstalledRam);
	sprintf(cValue,"%2.2f%%",fRatio);
	SetNodeProp("cRAMUsageRatio",cValue,uNode);

	//4-.
	//Check all active node activity via tProperty
        MYSQL_RES *res2;
        MYSQL_ROW field2;
	unsigned uPrevDatacenter=0;

	char cLogfile[64]={"/tmp/unxsvzlog"};

	if((gLfp=fopen(cLogfile,"a"))==NULL)
        {
                printf("Could not open logfile: %s\n",cLogfile);
		return;
        }

	//Distributed UBC data may exist
	//Check tDatacenter property gcUBCDBIP0/1
	sprintf(gcQuery,"SELECT cLabel,uDatacenter,uNode"
			" FROM tNode"
			" WHERE uStatus=1"
			" AND cLabel!='appliance'"
			" ORDER BY uDatacenter,uNode");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		printf("%s",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
		printf("<u>Node Roll Call</u><br>\n");
	while((field=mysql_fetch_row(res)))
	{
		unsigned uDatacenter=0;
		unsigned uNode=0;
		static unsigned uCount=0;

		uCount++;

		//The following will associate gMysqlUBC with the main db or with an external UBC db server
		sscanf(field[1],"%u",&uDatacenter);
		sscanf(field[2],"%u",&uNode);
		if(!uDatacenter) break;
		if(!uNode) break;
		//Note that we set the prev datacenter at the bottom of the loop
		if(uPrevDatacenter!=uDatacenter)
		{
			if(uPrevDatacenter)
			{
				mysql_close(&gMysqlUBC);
				//debug
				//printf("closed %s %s<br>\n",gcUBCDBIP0Buffer,gcUBCDBIP1Buffer);
			}
			if(ConnectToOptionalUBCDb(uDatacenter,0))
			{
				printf("%s connect error %s %s<br>\n",field[0],gcUBCDBIP0Buffer,gcUBCDBIP1Buffer);
				continue;
			}
			//debug
			//printf("connected %s %s<br>\n",gcUBCDBIP0Buffer,gcUBCDBIP1Buffer);
			uPrevDatacenter=uDatacenter;
		}

		sprintf(gcQuery,"SELECT FROM_UNIXTIME(MAX(tProperty.uModDate)),(UNIX_TIMESTAMP(NOW()) - MAX(tProperty.uModDate) > 900 )"
			" FROM tProperty"
			" WHERE tProperty.uKey=%u"
			" AND tProperty.uType=2"
			" AND tProperty.cName='numiptent.luFailcnt'",uNode);
		mysql_query(&gMysqlUBC,gcQuery);
        	if(mysql_errno(&gMysqlUBC))
		{
			printf("%s",mysql_error(&gMysqlUBC));
			return;
		}
        	res2=mysql_store_result(&gMysqlUBC);

		if((field2=mysql_fetch_row(res2)))
		{
			printf("<font color=");
			if(field2[1]==NULL || field2[0]==NULL)
			{
				printf("red>%s no data</font><br>\n",field[0]);
			}
			else
			{
				if(field2[1][0]=='1') 
					printf("red>");
				else
					printf("black>");
				printf("%s last contact: %s</font><br>\n",field[0],field2[0]);
			}
		}
		mysql_free_result(res2);


	}//each node w/datacenter
	mysql_free_result(res);

}//void htmlNodeHealth(unsigned uNode)


unsigned FailoverCloneContainer(unsigned uDatacenter, unsigned uNode, unsigned uContainer, unsigned uSource,
			unsigned uSourceNode, unsigned uSourceDatacenter, unsigned uIPv4, unsigned uStatus,
			char *cLabel, char *cHostname,unsigned uOwner,unsigned uDebug)
{
	unsigned uRetVal=2;
	unsigned uFailToJob=0;

	if((uFailToJob=FailoverToJob(uDatacenter,uNode,uContainer,uOwner,1,uDebug)))
	{
		if(FailoverFromJob(uSourceDatacenter,uSourceNode,uSource,uIPv4,
				cLabel,cHostname,uContainer,uStatus,uFailToJob,uOwner,1,uDebug))
		{
			if(!uDebug)
			{
				SetContainerStatus(uContainer,uAWAITFAIL);
				SetContainerStatus(uSource,uAWAITFAIL);
			}
			uRetVal=0;
		}
		else
		{
			uRetVal=1;
		}
	}

	return(uRetVal);

}//unsigned FailoverCloneContainer()


//Can return 0,1,2,3 or 4
unsigned CloneNode(unsigned uSourceNode,unsigned uTargetNode,unsigned uWizIPv4,const char *cuWizIPv4PullDown,
			unsigned uSyncPeriod,unsigned uCloneStop,unsigned uTargetDatacenter)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContainer=0;
	unsigned uDatacenter=0;
	unsigned uOSTemplate=0;
	unsigned uConfig=0;
	unsigned uNameserver=0;
	unsigned uSearchdomain=0;
	unsigned uNewVeid=0;
	unsigned uStatus=0;
	unsigned uOwner=0;
	unsigned uVeth=0;
	unsigned uGroup=0;
	char cWizLabel[32]={""};
	char cWizHostname[100]={""};


	//First lets make sure we have enough IPs available of same class C on target datacenter
	//Get container IP cIPv4ClassC
	char cIPv4ClassC[32]={""};
	int i;
	unsigned uIPsAvailable=0;
	unsigned uContainers=0;
	sprintf(gcQuery,"SELECT cLabel FROM tIP WHERE uIP=%u AND uAvailable=1"
					" AND uDatacenter=%u",uWizIPv4,uTargetDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cIPv4ClassC,"%.31s",field[0]);
	else
		return(1);
	mysql_free_result(res);
	for(i=strlen(cIPv4ClassC);i>0;i--)
	{
		if(cIPv4ClassC[i]=='.')
		{
			cIPv4ClassC[i]=0;
			break;
		}
	}
	//Count number of IPs available in same class C
	if(guCompany==1)
		sprintf(gcQuery,"SELECT COUNT(uIP) FROM tIP WHERE cLabel LIKE '%s.%%' AND uAvailable=1"
					" AND uDatacenter=%u",cIPv4ClassC,uTargetDatacenter);
	else
		sprintf(gcQuery,"SELECT COUNT(uIP) FROM tIP WHERE cLabel LIKE '%s.%%' AND uAvailable=1"
					" AND uOwner=%u AND uDatacenter=%u",cIPv4ClassC,guCompany,uTargetDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uIPsAvailable);
	else
		return(1);
	mysql_free_result(res);
	//Count number of containers to clone
	sprintf(gcQuery,"SELECT COUNT(uContainer) FROM tContainer WHERE uNode=%u AND"
			" (uStatus=%u OR uStatus=%u) AND uSource=0",uSourceNode,uSTOPPED,uACTIVE);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uContainers);
	else
		return(1);
	mysql_free_result(res);
	//Test
	if(uIPsAvailable<uContainers)
	{
		char cMsg[100];
		sprintf(cMsg,"<blink>Not enough IPs</blink>: uIPsAvailable(%u)&lt;uContainers(%u) for cIPv4ClassC=%s, uOwner=%u",
			uIPsAvailable,uContainers,cIPv4ClassC,guCompany);
		tNode(cMsg);
	}

	sprintf(gcQuery,"SELECT cLabel,cHostname,uOSTemplate,uConfig,uNameserver,uSearchdomain,uDatacenter"
			",uContainer,uStatus,uOwner,uVeth FROM tContainer WHERE uNode=%u AND"
			" (uStatus=%u OR uStatus=%u) AND uSource=0",uSourceNode,uSTOPPED,uACTIVE);
	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[2],"%u",&uOSTemplate);
		sscanf(field[3],"%u",&uConfig);
		sscanf(field[4],"%u",&uNameserver);
		sscanf(field[5],"%u",&uSearchdomain);
		sscanf(field[6],"%u",&uDatacenter);
		sscanf(field[7],"%u",&uContainer);
		sscanf(field[8],"%u",&uStatus);
		sscanf(field[9],"%u",&uOwner);
		sscanf(field[10],"%u",&uVeth);

		if(!uDatacenter || !uContainer || !uOSTemplate || !uConfig || ! uNameserver ||
							!uSearchdomain || !field[0][0] || !field[1][0])
		{
			mysql_free_result(res);
			return(1);//unexpected error.
		}

		uNewVeid=CommonCloneContainer(
						uContainer,
						uOSTemplate,
						uConfig,
						uNameserver,
						uSearchdomain,
						uDatacenter,
						uTargetDatacenter,
						uOwner,
						field[0],
						uNode,
						uStatus,
						field[1],
						"",//No cClassC
						uWizIPv4,
						cWizLabel,
						cWizHostname,
						uTargetNode,
						uSyncPeriod,
						guLoginClient,
						uCloneStop,1);//uMode 1 tNode errors
		if(!uNewVeid)
		{
			mysql_free_result(res);
			return(2);//unexpected error from CommonCloneContainer)(.
		}
		SetContainerStatus(uContainer,uStatus);//undo CommonCloneContainer() set
		uGroup=uGetGroup(0,uContainer);
		if(uGroup) UpdatePrimaryContainerGroup(uNewVeid,uGroup);

		//debug only
		//tNode(cWizHostname);
	}
	mysql_free_result(res);

	return(0);

}//unsigned CloneNode()


//UBC safe should not be used for UBCs	
void SetNodeProp(char const *cName,char const *cValue,unsigned uNode)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uKey=%u AND uType=2 AND cName='%s'",uNode,cName);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tProperty SET cValue='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uProperty=%s",
			cValue,guLoginClient,field[0]);
        	mysql_query(&gMysql,gcQuery);
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,cName='%s',cValue='%s',uType=2,uOwner=%u,"
				"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uNode,
					cName,cValue,guLoginClient,guLoginClient);
        	mysql_query(&gMysql,gcQuery);
	}
}//void SetNodeProp(char const *cName,char const *cValue,unsigned uNode);


unsigned ConnectToOptionalUBCDb(unsigned uDatacenter,unsigned uPrivate)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(!uDatacenter) return(2);

	//If no deal use main db as default
	gcUBCDBIP0=DBIP0;
	gcUBCDBIP1=DBIP1;

	//UBC MySQL server per datacenter option. Get db IPs
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u"
				" AND uType=1"
				" AND cName='gcUBCDBIP0'"
						,uDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("unxsVZ:ConnectToOptionalUBCDb",mysql_error(&gMysql));
		return(1);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		unsigned uA=0,uB=0,uC=0,uD=0;
		char *cScan={"%*u.%*u.%*u.%*u Public %u.%u.%u.%u"};
		if(uPrivate) cScan="%u.%u.%u.%u";
		if(sscanf(field[0],cScan,&uA,&uB,&uC,&uD)==4)
		{
			sprintf(gcUBCDBIP0Buffer,"%u.%u.%u.%u",uA,uB,uC,uD);
			gcUBCDBIP0=gcUBCDBIP0Buffer;
			logfileLine("unxsVZ:ConnectToOptionalUBCDb",gcUBCDBIP0Buffer);
		}
	}
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u"
				" AND uType=1"
				" AND cName='gcUBCDBIP1'"
						,uDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("unxsVZ:ConnectToOptionalUBCDb",mysql_error(&gMysql));
		return(1);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		unsigned uA=0,uB=0,uC=0,uD=0;
		char *cScan={"%*u.%*u.%*u.%*u Public %u.%u.%u.%u"};
		if(uPrivate) cScan="%u.%u.%u.%u";
		if(sscanf(field[0],cScan,&uA,&uB,&uC,&uD)==4)
		{
			sprintf(gcUBCDBIP1Buffer,"%u.%u.%u.%u",uA,uB,uC,uD);
			gcUBCDBIP1=gcUBCDBIP1Buffer;
			logfileLine("unxsVZ:ConnectToOptionalUBCDb",gcUBCDBIP1Buffer);
		}
	}
	//If gcUBCDBIP0 or gcUBCDBIP1 exist then we will use another MySQL db for UBC tProperty
	//	data
	//debug
	//printf("connecting to %s %s\n",gcUBCDBIP0Buffer,gcUBCDBIP1Buffer);
	return(ConnectDbUBC());
		

}//unsigned ConnectToOptionalUBCDb()


void tNodeHealth(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;


	char cLogfile[64]={"/tmp/unxsvzlog"};
	if(gLfp==NULL)
	{
		if( (gLfp=fopen(cLogfile,"a"))==NULL)
			tDatacenter("Could not open logfile");
	}
	if(uDatacenter && ConnectToOptionalUBCDb(uDatacenter,0))
	{
		printf("<p>ConnectToOptionalUBCDb() error<p>");
		return;
	}
	if(gcUBCDBIP0!=DBIP0 || gcUBCDBIP1!=DBIP1)
        	printf("<p><u>tNodeHealth Distributed UBC</u><br>\n");

	//1-. Disk space usage/soft limit ratio
	//1a-. Create temp table
	sprintf(gcQuery,"CREATE TEMPORARY TABLE tDiskUsage (uContainer INT UNSIGNED NOT NULL DEFAULT 0,"
			" luUsage INT UNSIGNED NOT NULL DEFAULT 0, luSoftlimit INT UNSIGNED NOT NULL DEFAULT 0,"
			" cLabel VARCHAR(32) NOT NULL DEFAULT '')");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tNodeHealth</u><br>\n");
                printf("a-. %s",mysql_error(&gMysql));
                return;
        }

	//1b-. Populate with data per container
	sprintf(gcQuery,"SELECT tProperty.uKey,tProperty.cValue,tContainer.cLabel FROM tProperty,tContainer"
			" WHERE tProperty.uKey=tContainer.uContainer AND tProperty.uType=3 AND"
			" tProperty.cName='1k-blocks.luUsage' AND tContainer.uDatacenter=%u"
			" AND tContainer.uNode=%u"
			" AND tContainer.uStatus=1",uDatacenter,uNode);
        mysql_query(&gMysqlUBC,gcQuery);
        if(mysql_errno(&gMysqlUBC))
        {
        	printf("<p><u>tNodeHealth</u><br>\n");
                printf("0-. %s",mysql_error(&gMysqlUBC));
                return;
        }
        res=mysql_store_result(&gMysqlUBC);
	while((field=mysql_fetch_row(res)))
	{	
		sprintf(gcQuery,"INSERT INTO tDiskUsage SET uContainer=%s,luUsage=%s,cLabel='%.32s'",
			field[0],field[1],field[2]);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
        	{
        		printf("<p><u>tNodeHealth</u><br>\n");
			printf("1-. %s",mysql_error(&gMysql));
			return;
		}
	}
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT tProperty.uKey,tProperty.cValue FROM tProperty,tContainer"
			" WHERE tProperty.uKey=tContainer.uContainer AND tProperty.uType=3 AND"
			" tProperty.cName='1k-blocks.luSoftLimit' AND tContainer.uDatacenter=%u"
			" AND tContainer.uNode=%u"
			" AND tContainer.uStatus=1",uDatacenter,uNode);
        mysql_query(&gMysqlUBC,gcQuery);
        if(mysql_errno(&gMysqlUBC))
        {
        	printf("<p><u>tNodeHealth</u><br>\n");
                printf("2-. %s",mysql_error(&gMysqlUBC));
                return;
        }
        res=mysql_store_result(&gMysqlUBC);
	while((field=mysql_fetch_row(res)))
	{	
		sprintf(gcQuery,"UPDATE tDiskUsage SET luSoftlimit=%s WHERE uContainer=%s",field[1],field[0]);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
        	{
        		printf("<p><u>tNodeHealth</u><br>\n");
			printf("3-. %s",mysql_error(&gMysql));
			return;
		}
	}
	mysql_free_result(res);

	//1d-. Report
	unsigned luSoftlimit;
	unsigned luUsage;
	float fRatio;
	char *cColor;
		
	sprintf(gcQuery,"SELECT luSoftlimit,luUsage,uContainer,cLabel FROM tDiskUsage"
			" WHERE ((luUsage/luSoftlimit)>0.5)"
			" ORDER BY (luUsage/luSoftlimit) DESC LIMIT 20");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
		printf("4-. %s",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
        	printf("<p><u>Top 20 Containers by Usage Ratio (50%%+)</u><br>\n");
	while((field=mysql_fetch_row(res)))
	{
		luSoftlimit=0;
		luUsage=0;
		sscanf(field[0],"%u",&luSoftlimit);
		sscanf(field[1],"%u",&luUsage);
		//Strange values hack
		if(!luUsage)
			luUsage=1;
		if(!luSoftlimit)
			luSoftlimit=luUsage;
		fRatio= ((float) luUsage/ (float) luSoftlimit) * 100.00 ;
		cColor=cRatioColor(&fRatio);

		printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>"
				"<font color=%s>%2.2f%% %s</font></a><br>\n",field[2],cColor,fRatio,field[3]);
	}
        mysql_free_result(res);


	//2-. None zero historic fail counters
	sprintf(gcQuery,"SELECT cValue,uKey,cLabel,cName FROM tProperty,tContainer WHERE"
			" tProperty.uKey=tContainer.uContainer AND"
			" tContainer.uDatacenter=%u AND"
			" tContainer.uNode=%u AND"
			" cValue!='0' AND uType=3 AND cName LIKE '%%.luFailcnt'"
			" ORDER BY CONVERT(cValue,UNSIGNED) DESC LIMIT 10",uDatacenter,uNode);
        mysql_query(&gMysqlUBC,gcQuery);
        if(mysql_errno(&gMysqlUBC))
        {
        	printf("<p><u>tNodeHealth</u><br>\n");
                printf("5-. %s",mysql_error(&gMysqlUBC));
                return;
        }

        res=mysql_store_result(&gMysqlUBC);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>Top 10 Containers by X.luFailcnt</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>"
				"%s %s=%s</a><br>\n",field[1],field[2],field[3],field[0]);
	}
        mysql_free_result(res);

	//3a-. Todays top in
	sprintf(gcQuery,"SELECT FORMAT(SUM(cValue/1000),2),uKey,cHostname,TIME(FROM_UNIXTIME(tProperty.uModDate)) FROM"
			" tProperty,tContainer WHERE"
			" tProperty.uKey=tContainer.uContainer AND cValue!='0' AND uType=3 AND"
			" tContainer.uStatus=%u AND"
			" tContainer.uDatacenter=%u AND"
			" tContainer.uNode=%u AND"
			" cName='Venet0.luMaxDailyInDelta'"
			" GROUP BY uKey ORDER BY CONVERT(cValue,UNSIGNED) DESC LIMIT 10",uACTIVE,uDatacenter,uNode);
        mysql_query(&gMysqlUBC,gcQuery);
        if(mysql_errno(&gMysqlUBC))
        {
        	printf("<p><u>tNodeHealth</u><br>\n");
                printf("5-. %s",mysql_error(&gMysqlUBC));
                return;
        }
        res=mysql_store_result(&gMysqlUBC);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>Today's peak in talkers</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>"
				"%s/%s %sKB/s</a><br>\n",field[1],field[2],field[3],field[0]);
	}
        mysql_free_result(res);

	//3b-. Todays top out
	sprintf(gcQuery,"SELECT FORMAT(SUM(cValue/1000),2),uKey,cHostname,TIME(FROM_UNIXTIME(tProperty.uModDate)) FROM"
			" tProperty,tContainer WHERE"
			" tProperty.uKey=tContainer.uContainer AND cValue!='0' AND uType=3 AND"
			" tContainer.uStatus=%u AND"
			" tContainer.uDatacenter=%u AND"
			" tContainer.uNode=%u AND"
			" cName='Venet0.luMaxDailyOutDelta'"
			" GROUP BY uKey ORDER BY CONVERT(cValue,UNSIGNED) DESC LIMIT 10",uACTIVE,uDatacenter,uNode);
        mysql_query(&gMysqlUBC,gcQuery);
        if(mysql_errno(&gMysqlUBC))
        {
        	printf("<p><u>tNodeHealth</u><br>\n");
                printf("5-. %s",mysql_error(&gMysqlUBC));
                return;
        }
        res=mysql_store_result(&gMysqlUBC);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>Today's peak out talkers</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>"
				"%s/%s %sKB/s</a><br>\n",field[1],field[2],field[3],field[0]);
	}
        mysql_free_result(res);

	//4-. Last 5 min top talkers
	sprintf(gcQuery,"SELECT FORMAT(SUM(cValue/2000),2),uKey,cHostname FROM"
			" tProperty,tContainer WHERE"
			" tProperty.uKey=tContainer.uContainer AND cValue!='0' AND uType=3 AND"
			" tContainer.uStatus=%u AND"
			" tContainer.uDatacenter=%u AND"
			" tContainer.uNode=%u AND"
			" (cName='Venet0.luInDelta' OR cName='Venet0.luOutDelta')"
			" GROUP BY uKey ORDER BY CONVERT(cValue,UNSIGNED) DESC LIMIT 10",uACTIVE,uDatacenter,uNode);
        mysql_query(&gMysqlUBC,gcQuery);
        if(mysql_errno(&gMysqlUBC))
        {
        	printf("<p><u>tNodeHealth</u><br>\n");
                printf("5-. %s",mysql_error(&gMysqlUBC));
                return;
        }
        res=mysql_store_result(&gMysqlUBC);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>Last 5min top talkers</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>"
				"%s %sKB/s</a><br>\n",field[1],field[2],field[0]);
	}
        mysql_free_result(res);

	//4b-. Last 5 min top diff talkers
	sprintf(gcQuery,"SELECT ABS(CONVERT(t2.cValue,SIGNED)-CONVERT(t1.cValue,SIGNED)),t1.uKey,t1.uKey FROM"
			" tProperty AS t1, tProperty AS t2 WHERE"
			" t1.uKey IN (SELECT uContainer from tContainer where uStatus=%u AND uDatacenter=%u AND uNode=%u) AND"
			" t1.uKey=t2.uKey AND t1.uType=3 AND t1.cName='Venet0.luInDelta' AND"
			" t2.uKey=t2.uKey AND t2.uType=3 AND t2.cName='Venet0.luOutDelta'"
			" ORDER BY ABS(CONVERT(t2.cValue,SIGNED)-CONVERT(t1.cValue,SIGNED)) DESC LIMIT 10",uACTIVE,uDatacenter,uNode);
        mysql_query(&gMysqlUBC,gcQuery);
        if(mysql_errno(&gMysqlUBC))
        {
        	printf("<p><u>tNodeHealth</u><br>\n");
                printf("5-. %s",mysql_error(&gMysqlUBC));
                return;
        }
        res=mysql_store_result(&gMysqlUBC);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>Last 5min top diff</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>"
				"%s %sB/s</a><br>\n",field[1],field[2],field[0]);
	}
        mysql_free_result(res);

	//4-. Top talkers
	sprintf(gcQuery,"SELECT FORMAT(SUM(cValue/1000000000),2),uKey,cHostname FROM"
			" tProperty,tContainer WHERE"
			" tContainer.uStatus=%u AND"
			" tContainer.uDatacenter=%u AND"
			" tContainer.uNode=%u AND"
			" tProperty.uKey=tContainer.uContainer AND cValue!='0' AND uType=3 AND"
			" (cName='Venet0.luIn' OR cName='Venet0.luOut')"
			" GROUP BY uKey ORDER BY CONVERT(cValue,UNSIGNED) DESC LIMIT 10",uACTIVE,uDatacenter,uNode);
        mysql_query(&gMysqlUBC,gcQuery);
        if(mysql_errno(&gMysqlUBC))
        {
        	printf("<p><u>tNodeHealth</u><br>\n");
                printf("5-. %s",mysql_error(&gMysqlUBC));
                return;
        }

        res=mysql_store_result(&gMysqlUBC);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>Historic top talkers</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>"
				"%s %sGB</a><br>\n",field[1],field[2],field[0]);
	}
        mysql_free_result(res);

}//void tNodeHealth(void)


