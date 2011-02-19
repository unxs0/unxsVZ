/*
FILE 
	container.c
	$Id: container.c 1478 2010-05-25 02:43:12Z Gary $
AUTHOR/LEGAL
	(C) 2010, 2011 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	unxsvzOrg program file.
	Container tab functions.
*/

#include "interface.h"

extern unsigned guBrowserFirefox;//main.c
char gcCtHostname[100]={""};
static char gcSearch[100]={""};
unsigned guContainer=0;
unsigned guStatus=0;
unsigned guNewContainer=0;
static char gcNewContainerTZ[64]={"PST8PDT"};
//Container details
static char gcLabel[33]={""};
static char gcNewHostname[33]={""};
static char gcNewHostParam0[33]={""};
static char gcNewHostParam1[33]={""};
static char gcDID[17]={""};
static char gcCustomerName[33]={""};


//TOC
void ProcessContainerVars(pentry entries[], int x);
void ContainerGetHook(entry gentries[],int x);
char *cGetHostname(unsigned uContainer);
char *cGetImageHost(unsigned uContainer);
void SelectContainer(void);
char *CustomerName(char *cInput);
char *NameToLower(char *cInput);
char *cNumbersOnly(char *cInput);
void SetContainerStatus(unsigned uContainer,unsigned uStatus);

void ProcessContainerVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"guContainer"))
			sscanf(entries[i].val,"%u",&guContainer);
		else if(!strcmp(entries[i].name,"gcCtHostname"))
			sprintf(gcCtHostname,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"gcSearch"))
			sprintf(gcSearch,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"guNewContainer"))
			sscanf(entries[i].val,"%u",&guNewContainer);
		else if(!strcmp(entries[i].name,"gcNewHostname"))
			sprintf(gcNewHostname,"%.32s",NameToLower(entries[i].val));
		else if(!strcmp(entries[i].name,"gcNewContainerTZ"))
			sprintf(gcNewContainerTZ,"%.63s",entries[i].val);
		else if(!strcmp(entries[i].name,"gcNewHostParam0"))
			sprintf(gcNewHostParam0,"%.32s",NameToLower(entries[i].val));
		else if(!strcmp(entries[i].name,"gcNewHostParam1"))
			sprintf(gcNewHostParam1,"%.32s",CustomerName(entries[i].val));
		else if(!strcmp(entries[i].name,"gcDID"))
			sprintf(gcDID,"%.16s",cNumbersOnly(entries[i].val));
		else if(!strcmp(entries[i].name,"gcCustomerName"))
			sprintf(gcCustomerName,"%.32s",CustomerName(entries[i].val));
	}

}//void ProcessContainerVars(pentry entries[], int x)


void ContainerGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"guContainer"))
			sscanf(gentries[i].val,"%u",&guContainer);
	}

	if(guContainer)
	{
		SelectContainer();
		htmlContainer();
	}

	htmlContainer();

}//void ContainerGetHook(entry gentries[],int x)


void ContainerCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Container"))
	{
		unsigned uLen=0;

		ProcessContainerVars(entries,x);
		if(!strcmp(gcFunction,"Repurpose Container"))
		{
        		MYSQL_RES *res;
	        	MYSQL_ROW field;

			//Validate target container
			if(!guNewContainer)
			{
				gcMessage="Must select a container.";
				htmlContainer();
			}
			//1-. Validate gcNewHostname
			if(strstr(gcNewHostname,"."))
			{
				gcMessage="New container name can not have anymore \"stops\" (periods.)";
				htmlContainer();
			}
			if((uLen=strlen(gcNewHostname))<2)
			{
				gcMessage="New container name must have at least two chars.";
				htmlContainer();
			}
			if(uLen>32)
			{
				gcMessage="New container name must have at most 32 chars.";
				htmlContainer();
			}
			//Check uniqueness
			sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE cLabel='%s'",gcNewHostname);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uContainer/cLabel error, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)>0)
			{
				gcMessage="New container name must not be already in use.";
				htmlContainer();
			}

			//Check job queue
			//Waiting, Running, Done Error, Remote waiting, Error
			sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uContainer=%u AND"
					" (uJobStatus=1 OR uJobStatus=2 OR uJobStatus=4 OR uJobStatus=10 OR uJobStatus=14)",
						guNewContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uJob error, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)>0)
			{
				gcMessage="Selected container is being used by another process. Please try another.";
				htmlContainer();
			}
			//uContainer must still exist
			sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uContainer=%u",guNewContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uContainer error, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)!=1)
			{
				gcMessage="Selected container does not exist. Please try another.";
				htmlContainer();
			}
			//uStatus must still be active
			unsigned uNode=0;
			unsigned uDatacenter=0;
			sprintf(gcQuery,"SELECT uContainer,uNode,uDatacenter FROM tContainer WHERE uContainer=%u"
					" AND uStatus=1",guNewContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uNode error, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[1],"%u",&uNode);
				sscanf(field[2],"%u",&uDatacenter);
			}
			if(mysql_num_rows(res)<1)
			{
				gcMessage="Selected container is not active. Please try another.";
				htmlContainer();
			}
			if(!uNode || !uDatacenter)
			{
				gcMessage="No uNode or no uDatacenter error. Contact your sysadmin!";
				htmlContainer();
			}

			//Check for configuration problems
			//1-. unxsBind zone and view
			//2-. Datacenter and node availability
			char cunxsBindARecordJobZone[65]={""};
			sprintf(gcQuery,"SELECT cValue FROM tConfiguration"
					" WHERE uDatacenter=(SELECT uDatacenter FROM tContainer WHERE uContainer=%u LIMIT 1)"
					" AND uContainer=0 AND uNode=0 AND cLabel='cunxsBindARecordJobZone'",guNewContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select cunxsBindARecordJobZone failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sprintf(cunxsBindARecordJobZone,"%.63s",field[0]);
			if(strlen(cunxsBindARecordJobZone)<2)
			{
				gcMessage="Configuration error contact your sysadmin: cunxsBindARecordJobZone.";
				htmlContainer();
			}
			char cunxsBindARecordJobView[65]={""};
			sprintf(gcQuery,"SELECT cValue FROM tConfiguration"
					" WHERE uDatacenter=(SELECT uDatacenter FROM tContainer WHERE uContainer=%u LIMIT 1)"
					" AND uContainer=0 AND uNode=0 AND cLabel='cunxsBindARecordJobView'",guNewContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select cunxsBindARecordJobView failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sprintf(cunxsBindARecordJobView,"%.63s",field[0]);
			if(strlen(cunxsBindARecordJobView)<2)
			{
				gcMessage="Configuration error contact your sysadmin: cunxsBindARecordJobView.";
				htmlContainer();
			}
			char cIPv4[32]={""};
			sprintf(gcQuery,"SELECT cLabel FROM tIP"
					" WHERE uIP=(SELECT uIPv4 FROM tContainer WHERE uContainer=%u LIMIT 1)",guNewContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select cIPv4 failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sprintf(cIPv4,"%.31s",field[0]);
			if(strlen(cIPv4)<7)
			{
				gcMessage="Configuration error contact your sysadmin: cIPv4.";
				htmlContainer();
			}

			if(gcNewHostParam0[0])
			{
				unsigned uProperty=0;
				char cOrgPropName[64]={"Unknown"};
				char *cp;

				sprintf(gcQuery,"SELECT cComment FROM tConfiguration WHERE cLabel='cNewHostParam0'");
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="Select cComment failed, contact sysadmin!";
					htmlContainer();
				}
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					if((cp=strstr(field[0],"cOrgPropName=")))
					{
						sprintf(cOrgPropName,"%.63s",cp+strlen("cOrgPropName="));
						if((cp=strchr(cOrgPropName,';')))
							*cp=0;
					}
				}

				sprintf(gcQuery,"SELECT uProperty FROM tProperty"
					" WHERE uKey=%u AND uType=3 AND cName='cOrg_%s'",guNewContainer,cOrgPropName);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="Select uProperty failed, contact sysadmin!";
					htmlContainer();
				}
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
					sscanf(field[0],"%u",&uProperty);
				if(uProperty)
				{
					sprintf(gcQuery,"UPDATE tProperty SET cValue='%s' WHERE uProperty=%u",
						gcNewHostParam0,uProperty);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						gcMessage="Update tProperty failed, contact sysadmin!";
						htmlContainer();
					}
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_%s',cValue='%s',uType=3,uKey=%u"
							",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						cOrgPropName,gcNewHostParam0,guNewContainer,guOrg,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						gcMessage="Update tProperty failed, contact sysadmin!";
						htmlContainer();
					}
				}
			}

			if(gcNewHostParam1[0])
			{
				unsigned uProperty=0;
				char cOrgPropName[64]={"Unknown"};
				char *cp;

				sprintf(gcQuery,"SELECT cComment FROM tConfiguration WHERE cLabel='cNewHostParam1'");
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="Select cComment failed, contact sysadmin!";
					htmlContainer();
				}
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					if((cp=strstr(field[0],"cOrgPropName=")))
					{
						sprintf(cOrgPropName,"%.63s",cp+strlen("cOrgPropName="));
						if((cp=strchr(cOrgPropName,';')))
							*cp=0;
					}
				}

				sprintf(gcQuery,"SELECT uProperty FROM tProperty"
					" WHERE uKey=%u AND uType=3 AND cName='cOrg_%s'",guNewContainer,cOrgPropName);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="Select uProperty failed, contact sysadmin!";
					htmlContainer();
				}
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
					sscanf(field[0],"%u",&uProperty);
				if(uProperty)
				{
					sprintf(gcQuery,"UPDATE tProperty SET cValue='%s' WHERE uProperty=%u",
						gcNewHostParam1,uProperty);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						gcMessage="Update tProperty failed, contact sysadmin!";
						htmlContainer();
					}
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_%s',cValue='%s',uType=3,uKey=%u"
							",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						cOrgPropName,gcNewHostParam1,guNewContainer,guOrg,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						gcMessage="Update tProperty failed, contact sysadmin!";
						htmlContainer();
					}
				}
			}


			//Always update GMT
			unsigned uProperty=0;
			sprintf(gcQuery,"SELECT uProperty FROM tProperty"
					" WHERE uKey=%u AND uType=3 AND cName='cOrg_TimeZone'",guNewContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uProperty failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sscanf(field[0],"%u",&uProperty);
			if(uProperty)
			{
				sprintf(gcQuery,"UPDATE tProperty SET cValue='%s' WHERE uProperty=%u",
						gcNewContainerTZ,uProperty);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="Update tProperty failed, contact sysadmin!";
					htmlContainer();
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_TimeZone',cValue='%s',uType=3,uKey=%u"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							gcNewContainerTZ,guNewContainer,guOrg,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="Update tProperty failed, contact sysadmin!";
					htmlContainer();
				}
			}

			//for all the above
			mysql_free_result(res);

			//debug only
			//gcMessage="Test mode";
			//htmlContainer();

			//Change the target container's names
			//Special created by non standard usage
			sprintf(gcQuery,"UPDATE tContainer SET cLabel='%s',cHostname='%s.%s',"
					" uCreatedBy=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
						" WHERE uContainer=%u",
						gcNewHostname,gcNewHostname,cunxsBindARecordJobZone,
						guLoginClient,
						guLoginClient,
						guNewContainer);
        		mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="UPDATE tContainer failed, contact sysadmin!";
				htmlContainer();
			}

			//Change hostname job
			sprintf(gcQuery,"INSERT INTO tJob SET cLabel='ChangeHostnameContainer(%u)',cJobName='ChangeHostnameContainer'"
					",uDatacenter=%u,uNode=%u,uContainer=%u"
					",uJobDate=UNIX_TIMESTAMP(NOW())+60"
					",uJobStatus=1"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						guNewContainer,
						uDatacenter,uNode,guNewContainer,
						guOrg,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="ChangeHostnameContainer insert failed, contact sysadmin!";
				htmlContainer();
			}
			SetContainerStatus(guNewContainer,61);

			/*
				cName=london32.your.net.;
				cIPv4=81.82.83.84;
				cZone=your.net;
				cView=external;
			*/
			//DNS job
			sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsBindARecordJob(%u)',cJobName='unxsVZContainerARR'"
					",uDatacenter=%u,uNode=%u,uContainer=%u"
					",uJobDate=UNIX_TIMESTAMP(NOW())+60"
					",uJobStatus=%u"
					",cJobData='"
					"cName=%s.%s.;\n"
					"cIPv4=%s;\n"
					"cZone=%s;\n"
					"cView=%s;\n'"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						guNewContainer,
						uDatacenter,uNode,guNewContainer,
						uREMOTEWAITING,
						gcNewHostname,cunxsBindARecordJobZone,cIPv4,cunxsBindARecordJobZone,cunxsBindARecordJobView,
						guOrg,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="unxsBindARecordJob insert failed, contact sysadmin!";
				htmlContainer();
			}

			//Change group
			char cOrg_NewGroupLabel[32]={""};
			char cOrg_AfterNewGroupLabel[32]={""};
			unsigned uGroup=0;
			sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uDatacenter=0"
					" AND uContainer=0 AND uNode=0 AND cLabel='cOrg_NewGroupLabel'");
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select cOrg_NewGroupLabel failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sprintf(cOrg_NewGroupLabel,"%.32s",field[0]);

			sprintf(gcQuery,"SELECT uGroup FROM tGroup WHERE cLabel='%s'",cOrg_NewGroupLabel);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select cOrg_AfterNewGroupLabel failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sscanf(field[0],"%u",&uGroup);

			sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uDatacenter=0"
					" AND uContainer=0 AND uNode=0 AND cLabel='cOrg_AfterNewGroupLabel'");
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select cOrg_AfterNewGroupLabel failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sprintf(cOrg_AfterNewGroupLabel,"%.32s",field[0]);

			if(!uGroup || !cOrg_AfterNewGroupLabel[0] || !cOrg_NewGroupLabel[0])
			{
				gcMessage="Group configuration problem-1, contact sysadmin!";
				htmlContainer();
			}

			sprintf(gcQuery,"UPDATE tGroupGlue SET uGroup=(SELECT uGroup FROM tGroup WHERE cLabel='%s' LIMIT 1)"
					" WHERE uContainer=%u AND uGroup=%u",cOrg_AfterNewGroupLabel,guNewContainer,uGroup);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Update tGroupGlue failed, contact sysadmin!";
				htmlContainer();
			}
			if(mysql_affected_rows(&gMysql)<1)
				gcMessage="Hostname change being attempted. Group not changed. Review new container status in a few minutes.";
			else
				gcMessage="Hostname change being attempted. Review new container status in a few minutes.";

			guContainer=guNewContainer;
			htmlContainer();
		}
		else if(!strcmp(gcFunction,"Container Report"))
		{
        		MYSQL_RES *res;
	        	MYSQL_ROW field;
        		MYSQL_RES *res2;
	        	MYSQL_ROW field2;


			printf("Content-type: text/plain\n\n");
			printf("uContainer,cLabel,cHostname,cDatacenter,cNode,cGroup\n");

			sprintf(gcQuery,"SELECT tContainer.uContainer,"
					" tContainer.cLabel,"
					" tContainer.cHostname,"
					" tDatacenter.cLabel,"
					" tNode.cLabel"
					" FROM tContainer,tDatacenter,tNode"
					" WHERE tContainer.uDatacenter=tDatacenter.uDatacenter"
					" AND tContainer.uNode=tNode.uNode AND tContainer.uSource=0");
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			while((field=mysql_fetch_row(res)))
			{
				char cGroup[33]={"NoGroup"};

				sprintf(gcQuery,"SELECT tGroup.cLabel"
					" FROM tGroup,tGroupGlue"
					" WHERE tGroup.uGroup=tGroupGlue.uGroup"
					" AND tGroupGlue.uContainer=%.15s",field[0]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				res2=mysql_store_result(&gMysql);
				if((field2=mysql_fetch_row(res2)))
					sprintf(cGroup,"%.32s",field2[0]);
				mysql_free_result(res2);

				printf("%s,%s,%s,%s,%s,%s\n",field[0],field[1],field[2],field[3],field[4],cGroup);
			}
			mysql_free_result(res);
			exit(0);
		}
		else if(!strcmp(gcFunction,"Mod CustomerName") && gcCustomerName[0])
		{
			char gcQuery[1024];
        		MYSQL_RES *res;
	        	MYSQL_ROW field;

			if(!guContainer)
			{
				gcMessage="Must select a container.";
				htmlContainer();
			}
			if((uLen=strlen(gcCustomerName))<3)
			{
				gcMessage="Customer name must be at least 3 characters long.";
				htmlContainer();
			}
			if(uLen>32)
			{
				gcMessage="Customer name length exceeded.";
				htmlContainer();
			}

			//uStatus must be active or offline or appliance
			unsigned uNode=0;
			unsigned uDatacenter=0;
			sprintf(gcQuery,"SELECT uContainer,uNode,uDatacenter FROM tContainer WHERE uContainer=%u"
					" AND (uStatus=1 OR uStatus=3 OR uStatus=101)",guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uNode error, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[1],"%u",&uNode);
				sscanf(field[2],"%u",&uDatacenter);
			}
			if(mysql_num_rows(res)<1)
			{
				gcMessage="Selected container status is incorrect.";
				htmlContainer();
			}
			if(!uNode || !uDatacenter)
			{
				gcMessage="No uNode or no uDatacenter error. Contact your sysadmin!";
				htmlContainer();
			}

			//Must be already registered with OpenSIPS or have LinesContracted value
			sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uType=3 AND uKey=%u AND"
					" (cName='cOrg_OpenSIPS_Attrs' OR cName='cOrg_LinesContracted')",
								guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Check for cOrg_OpenSIPS_Attrs failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)<1)
			{
				mysql_free_result(res);
				gcMessage="Container must be registered with OpenSIPS or have LinesContracted value.";
				htmlContainer();
			}
			mysql_free_result(res);

			//Check to see if customer name is already in property table
			sprintf(gcQuery,"SELECT uProperty FROM tProperty"
					" WHERE uKey=%u AND uType=3 AND cName='cOrg_CustomerName'"
					" AND cValue='%s'",guContainer,gcCustomerName);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Check for cOrg_CustomerName failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)>0)
			{
				mysql_free_result(res);
				gcMessage="Customer name not changed, already in property table.";
				htmlContainer();
			}
			mysql_free_result(res);

			sprintf(gcQuery,"INSERT INTO tProperty"
					" SET cValue='%s',"
					"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),"
					"uKey=%u,uType=3,cName='cOrg_CustomerMod'"
						,gcCustomerName,guOrg,guLoginClient,guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="INSERT for cOrg_CustomerMod failed, contact sysadmin!";
				htmlContainer();
			}

			//unxsSIPS job
			sprintf(gcCtHostname,"%.99s",(char *)cGetHostname(guContainer));
			sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsSIPSModCustomerName(%u)',cJobName='unxsSIPSModCustomerName'"
					",uDatacenter=%u,uNode=%u,uContainer=%u"
					",uJobDate=UNIX_TIMESTAMP(NOW())+60"
					",uJobStatus=%u"
					",cJobData='"
					"cCustomerName=%s;\n"
					"cHostname=%s;\n'"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						guContainer,
						uDatacenter,
						uNode,
						guContainer,
						uREMOTEWAITING,
						gcCustomerName,
						gcCtHostname,
						guOrg,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="unxsSIPSModCustomerName tJob insert failed, contact sysadmin!";
				htmlContainer();
			}

			gcMessage="Remote 'Mod CustomerName' task created for OpenSIPS.";
			htmlContainer();
		}//Mod CustomerName
		else if(!strcmp(gcFunction,"Add DID") && gcDID[0])
		{
			char gcQuery[1024];
        		MYSQL_RES *res;
	        	MYSQL_ROW field;

			if(!guContainer)
			{
				gcMessage="Must select a container.";
				htmlContainer();
			}
			if((uLen=strlen(gcDID))<10)
			{
				gcMessage="DID must have at least 10 numbers.";
				htmlContainer();
			}
			if(uLen>11)
			{
				gcMessage="DID must have at most 11 numbers.";
				htmlContainer();
			}
			if(uLen==11 && gcDID[0]!='1')
			{
				gcMessage="11 digit DID must start with digit number 1.";
				htmlContainer();
			}
			if(uLen==10 && gcDID[0]=='1')
			{
				gcMessage="10 digit DID can not start with digit number 1.";
				htmlContainer();
			}

			//uStatus must be active or offline or appliance
			unsigned uNode=0;
			unsigned uDatacenter=0;
			sprintf(gcQuery,"SELECT uContainer,uNode,uDatacenter FROM tContainer WHERE uContainer=%u"
					" AND (uStatus=1 OR uStatus=3 OR uStatus=101)",guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uNode error, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[1],"%u",&uNode);
				sscanf(field[2],"%u",&uDatacenter);
			}
			if(mysql_num_rows(res)<1)
			{
				gcMessage="Selected container status is incorrect.";
				htmlContainer();
			}
			if(!uNode || !uDatacenter)
			{
				gcMessage="No uNode or no uDatacenter error. Contact your sysadmin!";
				htmlContainer();
			}

			//Must be already registered with OpenSIPS or have LinesContracted value
			sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uType=3 AND uKey=%u AND"
					" (cName='cOrg_OpenSIPS_Attrs' OR cName='cOrg_LinesContracted')",
								guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Check for cOrg_OpenSIPS_Attrs failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)<1)
			{
				mysql_free_result(res);
				gcMessage="Container must be registered with OpenSIPS or have LinesContracted value.";
				htmlContainer();
			}
			mysql_free_result(res);

			//Check to see if DID is already in property table
			sprintf(gcQuery,"SELECT uProperty FROM tProperty"
					" WHERE uKey=%u AND uType=3 AND (cName='cOrg_OpenSIPS_DID' OR cName='cOrg_Pending_DID')"
					" AND cValue='%s'",guContainer,gcDID);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Check for cOrg_OpenSIPS_DID failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)>0)
			{
				mysql_free_result(res);
				gcMessage="DID not added, already in property table.";
				htmlContainer();
			}
			mysql_free_result(res);

			sprintf(gcQuery,"INSERT INTO tProperty"
					" SET uKey=%u,uType=3,cName='cOrg_Pending_DID',cValue='%s'"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						guContainer,gcDID,guOrg,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="INSERT for new cOrg_OpenSIPS_DID failed, contact sysadmin!";
				htmlContainer();
			}

			//unxsSIPS job
			sprintf(gcCtHostname,"%.99s",(char *)cGetHostname(guContainer));
			sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsSIPSNewDID(%u)',cJobName='unxsSIPSNewDID'"
					",uDatacenter=%u,uNode=%u,uContainer=%u"
					",uJobDate=UNIX_TIMESTAMP(NOW())+60"
					",uJobStatus=%u"
					",cJobData='"
					"cDID=%s;\n"
					"cHostname=%s;\n'"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						guContainer,
						uDatacenter,
						uNode,
						guContainer,
						uREMOTEWAITING,
						gcDID,
						gcCtHostname,
						guOrg,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="unxsSIPSNewDID tJob insert failed, contact sysadmin!";
				htmlContainer();
			}

			gcMessage="Remote 'Add DID' task created for OpenSIPS.";
			htmlContainer();
		}//Add DID
		else if(!strcmp(gcFunction,"Remove DID") && gcDID[0])
		{
			char gcQuery[1024];
        		MYSQL_RES *res;
	        	MYSQL_ROW field;

			if(!guContainer)
			{
				gcMessage="Must select a container.";
				htmlContainer();
			}
			if((uLen=strlen(gcDID))<10)
			{
				gcMessage="DID must have at least 10 numbers.";
				htmlContainer();
			}
			if(uLen>11)
			{
				gcMessage="DID must have at most 11 numbers.";
				htmlContainer();
			}
			if(uLen==11 && gcDID[0]!='1')
			{
				gcMessage="11 digit DID must start with digit number 1.";
				htmlContainer();
			}
			if(uLen==10 && gcDID[0]=='1')
			{
				gcMessage="10 digit DID can not start with digit number 1.";
				htmlContainer();
			}

			//uStatus must be active or offline or appliance
			unsigned uNode=0;
			unsigned uDatacenter=0;
			sprintf(gcQuery,"SELECT uContainer,uNode,uDatacenter FROM tContainer WHERE uContainer=%u"
					" AND (uStatus=1 OR uStatus=3 OR uStatus=101)",guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uNode error, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[1],"%u",&uNode);
				sscanf(field[2],"%u",&uDatacenter);
			}
			if(mysql_num_rows(res)<1)
			{
				gcMessage="Selected container status is incorrect.";
				htmlContainer();
			}
			if(!uNode || !uDatacenter)
			{
				gcMessage="No uNode or no uDatacenter error. Contact your sysadmin!";
				htmlContainer();
			}

			//Must be already registered with OpenSIPS or have LinesContracted value
			sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uType=3 AND uKey=%u AND"
					" (cName='cOrg_OpenSIPS_Attrs' OR cName='cOrg_LinesContracted')",
								guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Check for cOrg_OpenSIPS_Attrs failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)<1)
			{
				mysql_free_result(res);
				gcMessage="Container must be registered with OpenSIPS or have LinesContracted value.";
				htmlContainer();
			}
			mysql_free_result(res);

			//Check to see if DID is already in property table
			sprintf(gcQuery,"SELECT uProperty FROM tProperty"
					" WHERE uKey=%u AND uType=3 AND cName='cOrg_OpenSIPS_DID' AND cValue='%s'",guContainer,gcDID);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Check for cOrg_OpenSIPS_DID failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)<1)
			{
				mysql_free_result(res);
				gcMessage="DID not removed, not in property table.";
				htmlContainer();
			}
			mysql_free_result(res);

			sprintf(gcQuery,"UPDATE tProperty SET cName='cOrg_Remove_DID',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
					" WHERE uKey=%u AND uType=3 AND cValue='%s' AND cName='cOrg_OpenSIPS_DID'",
						guLoginClient,guContainer,gcDID);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="UPDATE for cOrg_OpenSIPS_DID failed, contact sysadmin!";
				htmlContainer();
			}

			//unxsSIPS job
			sprintf(gcCtHostname,"%.99s",(char *)cGetHostname(guContainer));
			sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsSIPSRemoveDID(%u)',cJobName='unxsSIPSRemoveDID'"
					",uDatacenter=%u,uNode=%u,uContainer=%u"
					",uJobDate=UNIX_TIMESTAMP(NOW())+60"
					",uJobStatus=%u"
					",cJobData='"
					"cDID=%s;\n"
					"cHostname=%s;\n'"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						guContainer,
						uDatacenter,
						uNode,
						guContainer,
						uREMOTEWAITING,
						gcDID,
						gcCtHostname,
						guOrg,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="unxsSIPSRemoveDID tJob insert failed, contact sysadmin!";
				htmlContainer();
			}

			gcMessage="Remote 'Remove DID' task created for OpenSIPS.";
			htmlContainer();
		}//Remove DID

		htmlContainer();
	}

}//void ContainerCommands(pentry entries[], int x)


void htmlContainer(void)
{
	htmlHeader("unxsvzOrg","Header");
	if(guContainer)
		SelectContainer();
	htmlContainerPage("unxsvzOrg","Container.Body");
	htmlFooter("Footer");

}//void htmlContainer(void)


void htmlContainerPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uPLAINSET,uVDNSORGTYPE);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="unxsvzOrg.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcUser;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);//Safe?

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcMessage";
			template.cpValue[7]=gcMessage;

			template.cpName[8]="gcCtHostname";
			//template.cpValue[8]=gcCtHostname;
			template.cpValue[8]=(char *)cGetHostname(guContainer) ;

			template.cpName[9]="gcSearch";
			template.cpValue[9]=gcSearch;

			template.cpName[10]="guContainer";
			char cguContainer[16];
			sprintf(cguContainer,"%u",guContainer);
			template.cpValue[10]=cguContainer;

			template.cpName[11]="gcLabel";
			template.cpValue[11]=gcLabel;

			template.cpName[12]="cDisabled";
			if(guContainer)
				template.cpValue[12]="";
			else
				template.cpValue[12]="disabled";


			template.cpName[13]="";

			printf("\n<!-- Start htmlContainerPage(%s) -->\n",cTemplateName); 
			Template(field[0],&template,stdout);
			printf("\n<!-- End htmlContainerPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlContainerPage()


void funcContainerImageTag(FILE *fp)
{
	if(!guContainer)
		return;

	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uStatus FROM tContainer WHERE uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&guStatus);
	mysql_free_result(res);
	if(guStatus!=uREMOTEAPPLIANCE && guStatus!=uAWAITACT)
		fprintf(fp,"<a href=https://%s/admin ><img src=%s/traffic/%u.png border=0 ></a>",
			cGetHostname(guContainer),cGetImageHost(guContainer),guContainer);

}//void funcContainerImageTag(FILE *fp)


void funcSelectContainer(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCount=1;
	unsigned uContainer=0;

	fprintf(fp,"<!-- funcSelectContainer(fp) Start -->\n");

	if(gcSearch[0])
		sprintf(gcQuery,"SELECT uContainer,cHostname FROM tContainer WHERE "
			"uSource=0 AND uOwner=%u AND cHostname LIKE '%s%%' "
			"ORDER BY cHostname  LIMIT 301",guOrg,gcSearch);
	else
		sprintf(gcQuery,"SELECT uContainer,cHostname FROM tContainer WHERE "
			"uOwner=%u AND uSource=0 ORDER BY cHostname LIMIT 301",guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	fprintf(fp,"<select class=type_textarea title='Select the container you want to load with this dropdown'"
			" name=guContainer onChange='submit()'>\n");
	fprintf(fp,"<option>---</option>");
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContainer);
		fprintf(fp,"<option value=%s",field[0]);
		if(guContainer==uContainer)
			fprintf(fp," selected");
		if((uCount++)<=300)
			fprintf(fp,">%s</option>",field[1]);
		else
			fprintf(fp,">Limit reached! Use better filter</option>");
	}
	mysql_free_result(res);

	fprintf(fp,"</select>\n");

	fprintf(fp,"<!-- funcSelectContainer(fp) End -->\n");

}//void funcSelectContainer(FILE *fp)


char *cGetHostname(unsigned uContainer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	static char cHostname[100]={""};

	sprintf(gcQuery,"SELECT cHostname FROM tContainer WHERE uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return((char *)mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cHostname,"%.99s",field[0]);
	return(cHostname);
}//char *cGetHostname(unsigned uContainer)


void SelectContainer(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel FROM tContainer WHERE uContainer=%u"
			,guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcLabel,"%.32s",field[0]);

		if(!gcMessage[0]) gcMessage="Zone Selected";
	}
	else
	{
		gcLabel[0]=0;
		gcMessage="<blink>No Zone Selected</blink>";
	}
	
	mysql_free_result(res);
	
}//void SelectContainer(void)



void funcContainerInfo(FILE *fp)
{
	if(!guContainer)
		return;

	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcContainerInfo (fp) Start -->\n");

	//Datacenter
	sprintf(gcQuery,"SELECT tDatacenter.cLabel FROM tContainer,tDatacenter WHERE tContainer.uDatacenter=tDatacenter.uDatacenter"
			" AND tContainer.uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=uDatacenter')\">"
			" <strong>Datacenter</strong></a></td><td><input type=text name='cDatacenter' value='%s' size=40 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
	mysql_free_result(res);

	//Node
	sprintf(gcQuery,"SELECT tNode.cLabel FROM tContainer,tNode WHERE tContainer.uNode=tNode.uNode"
			" AND tContainer.uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=uNode')\">"
			" <strong>Node</strong></a></td><td><input type=text name='cNode' value='%s' size=40 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
	mysql_free_result(res);

	//Status
	sprintf(gcQuery,"SELECT tStatus.cLabel FROM tContainer,tStatus WHERE tContainer.uStatus=tStatus.uStatus"
			" AND tContainer.uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=uStatus')\">"
			" <strong>Status</strong></a></td><td><input type=text name='cStatus' value='%s' size=40 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
	mysql_free_result(res);

	//OSTemplate
	sprintf(gcQuery,"SELECT tOSTemplate.cLabel FROM tContainer,tOSTemplate WHERE tContainer.uOSTemplate=tOSTemplate.uOSTemplate"
			" AND tContainer.uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=uOSTemplate')\">"
			" <strong>OSTemplate</strong></a></td><td><input type=text name='uOSTemplate' value='%s' size=40 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
	mysql_free_result(res);

	//IP
	sprintf(gcQuery,"SELECT tIP.cLabel FROM tContainer,tIP WHERE tIP.uIP=tContainer.uIPv4 AND uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=uIPv4')\">"
			" <strong>IPv4</strong></a></td><td><input type=text name='cIPv4' value='%s' size=40 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
	mysql_free_result(res);

	//Groups
	sprintf(gcQuery,"SELECT tGroup.cLabel FROM tGroup,tGroupGlue WHERE tGroup.uGroup=tGroupGlue.uGroup AND tGroupGlue.uContainer=%u",
		guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=Group')\">"
			" <strong>Group</strong></a></td><td><input type=text name='cGroup' value='%s' size=40 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
	mysql_free_result(res);

/*
	//Owner
	sprintf(gcQuery,"SELECT tClient.cLabel FROM tContainer,tClient WHERE tContainer.uOwner=tClient.uClient AND"
			" tContainer.uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=uOwner')\">"
			" <strong>Owner</strong></a></td><td><input type=text name='cOwner' value='%s' size=40 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
*/

	//CreatedBy
	sprintf(gcQuery,"SELECT tClient.cLabel FROM tContainer,tClient WHERE tContainer.uCreatedBy=tClient.uClient AND"
			" tContainer.uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=uCreatedBy')\">"
			" <strong>Created By</strong></a></td><td><input type=text name='cCreatedBy' value='%s' size=40 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}

	//CreatedDate
	sprintf(gcQuery,"SELECT FROM_UNIXTIME(uCreatedDate,'%%a %%b %%d %%T %%Y') FROM tContainer WHERE uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=uCreatedDate')\">"
			" <strong>Date Created</strong></a></td><td><input type=text name='cCreatedDate' value='%s'" " size=40 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}

/*
	//ModBy
	sprintf(gcQuery,"SELECT tClient.cLabel FROM tContainer,tClient WHERE tContainer.uModBy=tClient.uClient AND"
			" tContainer.uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=uModBy')\">"
			" <strong>Modified By</strong></a></td><td><input type=text name='cModBy'");
	if((field=mysql_fetch_row(res)))
		printf(" value='%s'",field[0]);
	else
		printf(" value='---'");
	printf(" disabled size=40 maxlength=32 class=\"type_fields_off\"> </td></tr>\n");
	mysql_free_result(res);
*/

	//cPasswd
	sprintf(gcQuery,"SELECT cName,cValue FROM tProperty WHERE uType=3 AND uKey=%u AND cName='cPasswd'",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=%s')\">"
			" <strong>SSH Passwd</strong></a></td><td><input type=text name='%s' value='%s' size=40 maxlength=32"
			" class=\"type_fields_off\"> </td></tr>\n",field[0],field[0],field[1]);
	}
	mysql_free_result(res);

	//SUBSTR based on 5 char cOrg_ prefix
	sprintf(gcQuery,"SELECT SUBSTR(cName,6),cValue FROM tProperty WHERE uType=3 AND uKey=%u AND cName LIKE 'cOrg_%%'"
			" ORDER BY cName",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=%s')\">"
			" <strong>%s</strong></a></td><td><input type=text name='%s' value='%s' size=40 maxlength=32"
			" class=\"type_fields_off\"> </td></tr>\n",field[0],field[0],field[0],field[1]);
	}
	mysql_free_result(res);

	fprintf(fp,"<!-- funcContainerInfo(fp) End -->\n");

}//void funcContainerInfo(FILE *fp)


char *cGetImageHost(unsigned uContainer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	static char cHostname[100]={""};
	char cOrg_ImageNodeDomain[64]={""};

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uDatacenter=0"
			" AND uContainer=0 AND uNode=0 AND cLabel='cOrg_ImageNodeDomain'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return((char *)mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cOrg_ImageNodeDomain,"%.63s",field[0]);

	if(!cOrg_ImageNodeDomain[0])
		return(cHostname);

	sprintf(gcQuery,"SELECT tNode.cLabel FROM tContainer,tNode WHERE tContainer.uNode=tNode.uNode AND uContainer=%u",
		uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return((char *)mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cHostname,"https://%.32s.%.63s",field[0],cOrg_ImageNodeDomain);

	return(cHostname);

}//char *cGetImageHost(unsigned uContainer)


//Func for left panel "new" container "creation" 
//Safe and fast for non tech-users. Just select and provide new name.
//Special "pre-spinned stand-by" containers that can simply 
//be name changed for actual use.
//We use special reserved group name "Stand-by" to identify

void funcNewContainer(FILE *fp)
{
	if(guPermLevel<6)
		return;

	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCount=1;
	unsigned uContainer=0;
	char cOrg_NewGroupLabel[33]={"Pre-Spinned"};
	char cOrg_NewHostname[65]={"somedomain.tld"};
	char cTitle[64]={""};
	char *cp;

	fprintf(fp,"<!-- funcNewContainer(fp) Start -->\n");

	fprintf(fp,"<tr><td valign=\"top\"><a class=inputLink href=\"#\""
		" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=Special+OPs')\""
		" <strong><u>Special OPs</u></strong></a></td><td>\n");

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uDatacenter=0"
			" AND uContainer=0 AND uNode=0 AND cLabel='cOrg_NewGroupLabel'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlPlainTextError(mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cOrg_NewGroupLabel,"%.32s",field[0]);

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uDatacenter=0"
			" AND uContainer=0 AND uNode=0 AND cLabel='cOrg_NewHostname'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlPlainTextError(mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cOrg_NewHostname,"%.64s",field[0]);

	sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cHostname FROM tContainer,tGroupGlue,tGroup WHERE "
			"tContainer.uContainer=tGroupGlue.uContainer AND "
			"tGroupGlue.uGroup=tGroup.uGroup AND tGroup.cLabel='%s' AND "
			"tContainer.uStatus=1 AND "
			"tContainer.uOwner=%u AND tContainer.uSource=0 "
			"ORDER BY tContainer.cHostname LIMIT 301",
			//"ORDER BY LENGTH(tContainer.cHostname),tContainer.cHostname LIMIT 301",
				cOrg_NewGroupLabel,guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlPlainTextError(mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	fprintf(fp,"<br><select class=type_textarea title='Select the container you want to use.'"
			" name=guNewContainer >\n");
	fprintf(fp,"<option>---</option>");
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContainer);
		fprintf(fp,"<option value=%s",field[0]);
		if(guNewContainer==uContainer)
			fprintf(fp," selected");
		if((uCount++)<=300)
			fprintf(fp,">%s</option>",field[1]);
		else
			fprintf(fp,">Limit reached. Contact your sysadmin ASAP!</option>");
	}
	mysql_free_result(res);
	fprintf(fp,"</select> Select container\n");

	//Time zone
	sprintf(gcQuery,"SELECT cValue,cComment FROM tConfiguration WHERE cLabel='cTimeZone' ORDER BY uConfiguration");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlPlainTextError(mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	fprintf(fp,"<p><select class=type_textarea title='Select the time zone you want to use.'"
			" name=gcNewContainerTZ >\n");
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<option value=%s",field[0]);
		if(!strcmp(gcNewContainerTZ,field[0]))
			fprintf(fp," selected");
		fprintf(fp,">%s</option>",field[1]);
	}
	mysql_free_result(res);
	fprintf(fp,"</select> Time zone\n");

	//Optional inputs
	sprintf(gcQuery,"SELECT cValue,cComment FROM tConfiguration WHERE cLabel='cNewHostParam0'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlPlainTextError(mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if((cp=strstr(field[1],"cTitle=")))
		{
			sprintf(cTitle,"%.63s",cp+7);
			if((cp=strchr(cTitle,';')))
				*cp=0;
		}
		fprintf(fp,"<p><input type=text class=type_fields"
			" title='%s'"
			" name=gcNewHostParam0 value='%s' size=16 maxlength=32> %s",cTitle,field[0],cTitle);
	}
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT cValue,cComment FROM tConfiguration WHERE cLabel='cNewHostParam1'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlPlainTextError(mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if((cp=strstr(field[1],"cTitle=")))
		{
			sprintf(cTitle,"%.63s",cp+7);
			if((cp=strchr(cTitle,';')))
				*cp=0;
		}
		fprintf(fp,"<p><input type=text class=type_fields"
			" title='%s'"
			" name=gcNewHostParam1 value='%s' size=16 maxlength=32> %s",cTitle,field[0],cTitle);
	}
	mysql_free_result(res);

	//New hostname
	fprintf(fp,"<p><input type=text class=type_fields"
			" title='Enter new name for the above selected container'"
			" name=gcNewHostname value='%s' size=16 maxlength=32>",gcNewHostname);

	fprintf(fp,".%s\n",cOrg_NewHostname);

	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Select a container, base time zone, then enter the first part of the new FQDN hostname."
			" Other options (as configured) may apply.'"
			" name=gcFunction value='Repurpose Container'>\n");

	if(guPermLevel>=6)
		fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Generate a cvs report of all PBX containers direct to browser'"
			" name=gcFunction value='Container Report'>\n");

	fprintf(fp,"</td></tr>\n");

	fprintf(fp,"<!-- funcNewContainer(fp) End -->\n");

}//void funcNewContainer(FILE *fp)


char *CustomerName(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
		if(!isalnum(cInput[i]) && cInput[i]!=' ' 
			&& cInput[i]!='.' && cInput[i]!=',') break;
	cInput[i]=0;

	return(cInput);

}//char *CustomerName(char *cInput)


char *NameToLower(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
	{
	
		if(!isalnum(cInput[i]) ) break;
		if(isupper(cInput[i])) cInput[i]=tolower(cInput[i]);
	}
	cInput[i]=0;

	return(cInput);

}//char *NameToLower(char *cInput)


char *cNumbersOnly(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
	{
	
		if(!isdigit(cInput[i]) ) break;
	}
	cInput[i]=0;

	return(cInput);

}//char *cNumbersOnly(char *cInput)


void SetContainerStatus(unsigned uContainer,unsigned uStatus)
{
	sprintf(gcQuery,"UPDATE tContainer SET uStatus=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uContainer=%u",
					uStatus,guLoginClient,uContainer);
	mysql_query(&gMysql,gcQuery);

}//void SetContainerStatus(unsigned uContainer,unsigned uStatus)


//functions that operate on loaded container
void funcContainer(FILE *fp)
{
	if(guPermLevel<6 || guContainer==0)
		return;

	fprintf(fp,"<!-- funcContainer(fp) Start -->\n");

	fprintf(fp,"<tr><td valign=\"top\"><a class=inputLink href=\"#\""
		" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=Container+OPs')\""
		" <strong><u>Container OPs</u></strong></a></td><td>\n");

	//DID
	fprintf(fp,"<p><br><input type=text class=type_fields"
			" title='Enter a valid DID number'"
			" name=gcDID size=16 maxlength=16> DID");
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Add a DID to currently loaded PBX container that has OpenSIPS_Attrs or LinesContracted values'"
			" name=gcFunction value='Add DID'>\n");
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Remove a DID from currently loaded PBX container'"
			" name=gcFunction value='Remove DID'>\n");
	fprintf(fp,"<p><br><input type=text class=type_fields"
			" title='Enter a valid customer name'"
			" name=gcCustomerName value='%s' size=16 maxlength=32> Customer name",gcCustomerName);
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Modify currently loaded PBX container customer name'"
			" name=gcFunction value='Mod CustomerName'>\n");

	fprintf(fp,"</td></tr>\n");

	fprintf(fp,"<!-- funcContainer(fp) End -->\n");

}//void funcContainer(FILE *fp)
