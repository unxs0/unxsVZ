/*
FILE
	$Id$
	This is an include file, see tcontainerfunc.h ExttContainerCommands().
PURPOSE
	New container gcCommand code
AUTHOR/LEGAL
	(C) 2001-2013 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
NOTES
*/


//step 1
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
}//LANG_NB_NEW


//step 2
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
			tContainer("<blink>Error:</blink> Must select an organization (company, NGO or similar.)");

		guMode=9002;
		tContainer("New container step 2");
	}
	else
	{
		tContainer("<blink>Error:</blink> Denied by permissions settings");
	}
}//Select Datacenter/Org


//step 3
else if(!strcmp(gcCommand,"Select Node"))
{
	if(guPermLevel>=9)
	{
		ProcesstContainerVars(entries,x);
		guMode=9002;
		if(!uDatacenter)
			tContainer("<blink>Error:</blink> Must select a datacenter.");
		if(!uForClient)
			tContainer("<blink>Error:</blink> Must select an organization (company, NGO or similar.)");
		if(!uNode)
			tContainer("<blink>Error:</blink> Must select a node.");

		GetDatacenterProp(uDatacenter,"NewContainerMode",cNCMDatacenter);
		if(cNCMDatacenter[0] && !strstr(cNCMDatacenter,"Active"))
			tContainer("<blink>Error:</blink> Selected datacenter is full or not active. Select another.");
	
		GetNodeProp(uNode,"NewContainerMode",cNCMNode);
		if(cNCMNode[0] && !strstr(cNCMNode,"Active"))
		tContainer("<blink>Error:</blink> Selected node is not configured for active containers. Select another.");
	
		CheckMaxContainers(1);//The argument is the number of containers to be created
	
		guMode=9003;
		tContainer("New container step 3");
	}
	else
	{
		tContainer("<blink>Error:</blink> Denied by permissions settings");
	}
}//Select Node


//appliance and single container creation
//lots of duplicate code in multiple container section
else if(!strcmp(gcCommand,"Single Container Creation") || !strcmp(gcCommand,"Appliance Creation"))
{
	if(guPermLevel>=9)
	{
		MYSQL_ROW field;
		unsigned uNewVeid=0;
		unsigned uHostnameLen=0;
		unsigned uLabelLen=0;
		unsigned uCreateAppliance=0;
		unsigned uCloneNode=uTargetNode;
		unsigned uCloneDatacenter=0;
		unsigned uRemoteNode=0;
		unsigned uRemoteDatacenter=0;
		unsigned uCloneIPv4=uWizIPv4;
		unsigned uRemoteIPv4=0;
		char cAutoCloneIPClass[256]={""};
		char cAutoCloneIPClassRemote[256]={""};
		char cAutoCloneNode[256]={""};
		char cAutoCloneNodeRemote[256]={""};

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
		CheckMaxContainers(1);//The argument is the number of containers to be created

		if((uLabelLen=strlen(cLabel))<2)
			tContainer("<blink>Error:</blink> cLabel is too short");
		if(strchr(cLabel,'.'))
			tContainer("<blink>Error:</blink> cLabel has at least one '.'");
		if(strstr(cLabel,"-clone"))
			tContainer("<blink>Error:</blink> cLabel can't have '-clone'");
		if(strstr(cLabel,"-backup"))
			tContainer("<blink>Error:</blink> cLabel can't have '-backup'");
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
		if(strstr(cHostname,"-backup"))
			tContainer("<blink>Error:</blink> cHostname can't have '-backup'");
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

		//tProperty based datacenter checks
		//VETH device
		if(uVeth)
		{
			GetNodeProp(uNode,"Container-Type",cContainerType);
			if(!strstr(cContainerType,"VETH"))
				tContainer("<blink>Error:</blink> uNode selected does not support VETH");
				
		}

		//clone code block start
		//

		//outline
		//	In container creation both local and remote clones are created
		//	as configured in tConfiguration.
		//	In single container creation we allow user to select clone IP
		//	but the backup IP is selected automatically from available pool.

		//If auto clone setup check required values
		GetConfiguration("cAutoCloneNode",cAutoCloneNode,uDatacenter,uNode,0,0);
		GetConfiguration("cAutoCloneNodeRemote",cAutoCloneNodeRemote,uDatacenter,uNode,0,0);

		if(cAutoCloneNode[0])
		{
			uCloneNode=ReadPullDown("tNode","cLabel",cAutoCloneNode);

			if(uCloneNode==0)
				tContainer("<blink>Error:</blink> Please select a valid target node"
						" for the clone.");
			if(uCloneNode==uNode)
				tContainer("<blink>Error:</blink> Can't clone to same node.");

			//extra check but maybe deprecated soon
			GetNodeProp(uCloneNode,"NewContainerMode",cNCMNode);
			if(cNCMNode[0] && !strstr(cNCMNode,"Clone"))
			tContainer("<blink>Error:</blink> Selected clone target node is not configured for clone containers."
					"Select another.");

			sscanf(ForeignKey("tNode","uDatacenter",uCloneNode),"%u",&uCloneDatacenter);
			if(!uCloneIPv4)
				tContainer("<blink>Error:</blink> You must select an IP for the clone");
			if(uCloneIPv4==uIPv4)
				tContainer("<blink>Error:</blink> You must select a different IP for the"
								" clone");
			unsigned uCloneIPv4Datacenter=0;
			sscanf(ForeignKey("tIP","uDatacenter",uCloneIPv4),"%u",&uCloneIPv4Datacenter);
			if(uCloneDatacenter!=uCloneIPv4Datacenter)
				tContainer("<blink>Error:</blink> The specified clone uIPv4 does not "
					"belong to the specified uNodeDatacenter.");

			GetConfiguration("cAutoCloneIPClass",cAutoCloneIPClass,uCloneDatacenter,uCloneNode,0,0);
			if(!cAutoCloneIPClass[0] || strncmp(ForeignKey("tIP","cLabel",uCloneIPv4),cAutoCloneIPClass,strlen(cAutoCloneIPClass)))
					tContainer("<blink>Error:</blink> Clone start uIPv4 must be in datacenter clone IP range");

			if(uSyncPeriod>86400*30 || (uSyncPeriod && uSyncPeriod<300))
				tContainer("<blink>Error:</blink> Clone uSyncPeriod out of range:"
						" Max 30 days, min 5 minutes or 0 off.");
		}//cAutoCloneNode

		if(cAutoCloneNodeRemote[0])
		{
			uRemoteNode=ReadPullDown("tNode","cLabel",cAutoCloneNodeRemote);

			if(uRemoteNode==0)
				tContainer("<blink>Error:</blink> cAutoCloneNodeRemote but no uRemoteNode");
			if(uRemoteNode==uNode)
				tContainer("<blink>Error:</blink> Can't remote clone to same node.");

			//extra check but maybe deprecated soon
			GetNodeProp(uRemoteNode,"NewContainerMode",cNCMNode);
			if(cNCMNode[0] && !strstr(cNCMNode,"Clone"))
			tContainer("<blink>Error:</blink> Configured remote clone node is not configured for clone containers.");

			sscanf(ForeignKey("tNode","uDatacenter",uRemoteNode),"%u",&uRemoteDatacenter);
			if(uRemoteDatacenter==uDatacenter)
				tContainer("<blink>Error:</blink> Configured remote datacenter same as local datacenter.");

			if(uSyncPeriod>86400*30 || (uSyncPeriod && uSyncPeriod<300))
				tContainer("<blink>Error:</blink> Clone uSyncPeriod out of range:"
						" Max 30 days, min 5 minutes or 0 off.");
		}//cAutoCloneNodeRemote

		//
		//clone code block end

		//TODO review this policy.
		//No same names or hostnames for same datacenter allowed.
		//TODO periods "." should be expanded to "[.]"
		//for correct cHostname REGEXP.
		sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE ("
					" cHostname REGEXP '^%s[0-9]+%s$'"
					" OR cLabel REGEXP '%s[0-9]+$'"
					" OR uContainer=%u"
					" ) AND uDatacenter=%u LIMIT 1",
						cLabel,cHostname,cLabel,uContainer,uDatacenter);
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

		sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uIP=%u AND uAvailable=1"
				" AND uOwner=%u AND uDatacenter=%u",uIPv4,uForClient,uDatacenter);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if(!(field=mysql_fetch_row(res)))
		{
			tContainer("<blink>Error:</blink> Someone grabbed your IP"
				", single container creation aborted -if Root select"
				" a company with IPs!");
		}
		mysql_free_result(res);

		//clone code block start
		//

		//Check for available IP for local datacenter clone if so configured
		if(cAutoCloneNode[0])
		{
			sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uAvailable=1"
				" AND uOwner=%u AND uDatacenter=%u AND uIP=%u",uForClient,uCloneDatacenter,uCloneIPv4);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			if(!(field=mysql_fetch_row(res)))
				tContainer("<blink>Error:</blink> Clone IP no longer available.");
			mysql_free_result(res);

			GetConfiguration("cAutoCloneIPClass",cAutoCloneIPClass,uCloneDatacenter,uCloneNode,0,0);
			if(!cAutoCloneIPClass[0])
				GetConfiguration("cAutoCloneIPClass",cAutoCloneIPClass,uCloneDatacenter,0,0,0);
		}//cAutoCloneNode

		//Check for available IP for backup remote clone if so configured
		if(cAutoCloneNodeRemote[0] && uRemoteDatacenter && uRemoteNode)
		{
			GetConfiguration("cAutoCloneIPClassRemote",cAutoCloneIPClassRemote,uRemoteDatacenter,uRemoteNode,0,0);
			if(!cAutoCloneIPClassRemote[0])
				GetConfiguration("cAutoCloneIPClassRemote",cAutoCloneIPClassRemote,uRemoteDatacenter,0,0,0);
			sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uAvailable=1"
				" AND uOwner=%u AND uDatacenter=%u AND INSTR(cLabel,'%s')=1",
					uForClient,uRemoteDatacenter,cAutoCloneIPClassRemote);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sscanf(field[0],"%u",&uRemoteIPv4);
			else
				tContainer("<blink>Error:</blink> No cAutoCloneIPClassRemote based IP available.");
			mysql_free_result(res);
		}//cAutoCloneNodeRemote

		//
		//clone code block end


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


		//appliance code block start
		//

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
		if(uCreateAppliance)
			uSource=uApplianceContainer;
		//
		//appliance code block end

		//Create the new main container
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


		//
		//clone code block start

		if(cAutoCloneNode[0] && uCloneDatacenter && uCloneNode && uCloneIPv4)
		{
			//TODO what about clone datacenter?
			uNewVeid=CommonCloneContainer(
							uContainer,
							uOSTemplate,
							uConfig,
							uNameserver,
							uSearchdomain,
							uDatacenter,
							uCloneDatacenter,
							uForClient,
							cLabel,
							uNode,
							uStatus,
							cHostname,
							"",//do not check class C
							uCloneIPv4,
							cWizLabel,
							cWizHostname,
							uCloneNode,
							uSyncPeriod,
							guLoginClient,
							uCloneStop,0);
			SetContainerStatus(uNewVeid,uINITSETUP);
			if(uGroup)
				UpdatePrimaryContainerGroup(uNewVeid,uGroup);

			//Create DNS job for clones also
			//rfc1918 control in CreateDNSJob() prevents non public IP clones from getting zones.
			if(uCreateDNSJob)
				CreateDNSJob(uCloneIPv4,uForClient,NULL,cWizHostname,uCloneDatacenter,guLoginClient,uNewVeid,uCloneNode);
		}//cAutoCloneNode

		if(cAutoCloneNodeRemote[0] && uRemoteDatacenter && uRemoteNode && uRemoteIPv4)
		{
			uCloneStop=HOT_CLONE;//the -backup we keep running as default
			uNewVeid=CommonCloneContainer(
							uContainer,
							uOSTemplate,
							uConfig,
							uNameserver,
							uSearchdomain,
							uDatacenter,
							uRemoteDatacenter,
							uForClient,
							cLabel,
							uNode,
							uStatus,
							cHostname,
							"",//do not check class C
							uRemoteIPv4,
							cWizLabel,
							cWizHostname,
							uRemoteNode,
							uSyncPeriod,
							guLoginClient,
							uCloneStop,0);
			if(uGroup)
				UpdatePrimaryContainerGroup(uNewVeid,uGroup);
			//change name to -backup
			char cRemoteHostname[100]={""};
			sprintf(cRemoteHostname,"%.99s",cWizHostname);
			if(uUpdateNamesFromCloneToBackup(uNewVeid))
			{
				sprintf(cRemoteHostname,"%.99s",ForeignKey("tContainer","cHostname",uNewVeid));
				if(HostnameContainerJob(uRemoteDatacenter,uRemoteNode,uNewVeid,cRemoteHostname,uForClient,guLoginClient))
						SetContainerStatus(uNewVeid,uAWAITHOST);
			}

			//Create DNS job for clones also
			//rfc1918 control in CreateDNSJob() prevents non public IP clones from getting zones.
			if(uCreateDNSJob)
				CreateDNSJob(uRemoteIPv4,uForClient,NULL,cRemoteHostname,uRemoteDatacenter,guLoginClient,uNewVeid,uRemoteNode);
		}//cAutoCloneNodeRemote


		//
		//clone code block end

		if(uCreateDNSJob)
			CreateDNSJob(uIPv4,uForClient,NULL,cHostname,uDatacenter,guLoginClient,uContainer,uNode);
		SetContainerStatus(uContainer,uINITSETUP);

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

		CheckMaxContainers(1);//The argument is the number of containers to be created

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
		char cIPv4ClassC[256]={""};
		char cIPv4ClassC2[256]={""};
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
		CheckMaxContainers(uNumContainer);//The argument is the number of containers to be created
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
		if(strstr(cLabel,"-backup"))
			tContainer("<blink>Error:</blink> cLabel can't have '-backup'");
		if((uHostnameLen=strlen(cHostname))<5)
			tContainer("<blink>Error:</blink> cHostname is too short");
		if(cHostname[uHostnameLen-1]=='.')
			tContainer("<blink>Error:</blink> cHostname can't end with a '.'");
		if(strstr(cHostname,"-clone"))
			tContainer("<blink>Error:</blink> cHostname can't have '-clone'");
		if(strstr(cHostname,"-backup"))
			tContainer("<blink>Error:</blink> cHostname can't have '-backup'");
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
					{
						char cAutoCloneIPClass[256]={""};
						GetConfiguration("cAutoCloneIPClass",cAutoCloneIPClass,
							uTargetDatacenter,uTargetNode,0,0);
						if(!cAutoCloneIPClass[0] || 
								strncmp(ForeignKey("tIP","cLabel",uWizIPv4),
									cAutoCloneIPClass,strlen(cAutoCloneIPClass)))
							tContainer("<blink>Error:</blink> Clone start uIPv4 must be in datacenter"
								" clone IP range (cmc)");
					}
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

		GetConfiguration("cAutoIPClass",cIPv4ClassC,uDatacenter,uNode,0,0);
		if(!cIPv4ClassC[0])
			GetConfiguration("cAutoIPClass",cIPv4ClassC,uDatacenter,0,0,0);
		GetConfiguration("cAutoIPClass2",cIPv4ClassC2,uDatacenter,uNode,0,0);
		if(!cIPv4ClassC2[0])
			GetConfiguration("cAutoIPClass2",cIPv4ClassC2,uDatacenter,0,0,0);
		if(!cIPv4ClassC[0] && !cIPv4ClassC2[0])
		{
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
		}//If no cAutoIPClass

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
				" AND (cLabel LIKE '%s%%' OR cLabel LIKE '%s%%') AND uDatacenter=%u",
						uForClient,cIPv4ClassC,cIPv4ClassC2,uDatacenter);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uAvailableIPs);
		mysql_free_result(res);
		if(uNumContainer>uAvailableIPs)
		{
			static char cMsg[256];
			sprintf(cMsg,"<blink>Error:</blink> Not enough IPs available!"
					" cIPv4ClassC=%.31s cIPv4ClassC2=%.31s",cIPv4ClassC,cIPv4ClassC2);
			tContainer(cMsg);
		}

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

				//rfc1918 control in CreateDNSJob() prevents non public IP clones from getting zones.
				if(uCreateDNSJob)
					CreateDNSJob(uWizIPv4,uForClient,NULL,cWizHostname,uTargetDatacenter,guLoginClient,uNewVeid,
								uTargetNode);
			}//cAutoCloneNode


			//For some reason cHostname is cLabel at this point. TODO debug.
			if(uCreateDNSJob)
				CreateDNSJob(uIPv4,uForClient,NULL,cHostname,uDatacenter,guLoginClient,uContainer,uNode);

			//Get next available uIPv4 only if not last loop iteration
			if((i+1)<uNumContainer)
			{
				sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uAvailable=1 AND uOwner=%u"
				" AND (cLabel LIKE '%s%%' OR  cLabel LIKE '%s%%') AND uDatacenter=%u LIMIT 1",
						uForClient,cIPv4ClassC,cIPv4ClassC2,uDatacenter);
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

