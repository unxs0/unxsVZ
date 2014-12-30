/*
FILE 
	container.c
	$Id$
AUTHOR/LEGAL
	(C) 2010, 2011 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	OneLogin program file.
	Container tab functions.
*/

#include "interface.h"

extern unsigned guBrowserFirefox;//main.c
char gcCtHostname[100]={""};
static char gcSearch[100]={""};
static char gcSearchAux[32]={""};
static char *gcSearchName={""};
unsigned guContainer=0;
unsigned guStatus=0;
unsigned guNewContainer=0;
unsigned guReseller=0;
static char gcNewContainerTZ[64]={"PST8PDT"};
static unsigned guNode=0;
static unsigned guDatacenter=0;
//Container details
//static unsigned guFlag=0;
static char gcLabel[33]={""};
static char gcNewHostname[33]={""};
static char gcNewHostParam0[33]={""};
static char gcNewHostParam1[33]={""};
static char gcServer[32]={""};
static char gcDID[17]={""};
static char *gcBulkData={""};
static char gcCustomerName[33]={""};
static char gcCustomerLimit[16]={""};
static char gcNewLogin[33]={"John Doe"};
static char gcNewPasswd[33]={""};
static char gcAuthCode[33]={""};
static char *gcShowDetails="";
char gcAdminPort[16]=":3321";

unsigned guMode;

#define uMAX_DIDs_ALLOWED 128
static char cReply[(33*uMAX_DIDs_ALLOWED)]={""};

void EncryptPasswdWithSalt(char *pw, char *salt);

//TOC incomplete TODO
void ProcessContainerVars(pentry entries[], int x);
void htmlContainerList(void);
void htmlContainerQOS(void);
void htmlContainerBulk(void);
void ContainerGetHook(entry gentries[],int x);
char *cGetHostname(unsigned uContainer);
char *cGetImageHost(unsigned uContainer);
void SelectContainer(void);
char *CustomerName(char *cInput);
char *NameToLower(char *cInput);
char *cNumbersOnly(char *cInput);
void SetContainerStatus(unsigned uContainer,unsigned uStatus);
void funcContainerList(FILE *fp);
void SendAlertEmail(char *cMsg);
void funcContainerBulk(FILE *fp);
char *ParseTextAreaLines(char *cTextArea);
void BulkDIDAdd(void);
void BulkDIDRemove(void);
void DIDOpsCommonChecking(void);
void LoadAllDIDs(void);
char *random_pw(char *dest);
void GetConfigurationValue(char const *cName,char *cValue,unsigned uDatacenter,unsigned uNode,unsigned uContainer);
unsigned CreateOrgDNSJob(unsigned uIPv4,unsigned uOwner,char const *cOptionalIPv4,char const *cHostname,
				unsigned uDatacenter,unsigned uCreatedBy,unsigned uContainer,unsigned uNode);
unsigned unxsBindPBXRecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,
		const char *cJobData,unsigned uOwner,unsigned uCreatedBy);
unsigned uGetPrimaryContainerGroup(unsigned uContainer);
unsigned uUpdateNamesFromCloneToBackup(unsigned uContainer);
unsigned uUpdateNamesFromCloneToClone(unsigned uContainer);
void GetContainerPropertyValue(char const *cName,char *cValue,unsigned uContainer);
void GetSIPProxyList(char *cSIPProxyList,unsigned guDatacenter,unsigned guNode,unsigned guContainer);

unsigned unxsBindRemoveContainer(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
	unsigned uOwner,unsigned uCreatedBy);

void htmlAuxPage(char *cTitle, char *cTemplateName);
void htmlAbout(void);
void htmlContact(void);

unsigned uPower10(unsigned uI)
{
	static unsigned uTotal=1;
	register int i;

	for(i=0;uI>0;uI--)
		uTotal=uTotal*10;

	return(uTotal);

}//unsigned uPower10(unsigned uI)


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
		else if(!strcmp(entries[i].name,"gcSearchAux"))
			sprintf(gcSearchAux,"%.31s",entries[i].val);
		else if(!strcmp(entries[i].name,"guNewContainer"))
			sscanf(entries[i].val,"%u",&guNewContainer);
		else if(!strcmp(entries[i].name,"guReseller"))
			sscanf(entries[i].val,"%u",&guReseller);
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
		else if(!strcmp(entries[i].name,"gcCustomerLimit"))
			sprintf(gcCustomerLimit,"%.15s",CustomerName(entries[i].val));
		else if(!strcmp(entries[i].name,"gcAuthCode"))
			sprintf(gcAuthCode,"%.32s",CustomerName(entries[i].val));
		else if(!strcmp(entries[i].name,"gcShowDetails"))
			gcShowDetails="checked";
		else if(!strcmp(entries[i].name,"gcNewLogin"))
			sprintf(gcNewLogin,"%.31s",CustomerName(entries[i].val));
		else if(!strcmp(entries[i].name,"gcNewPasswd"))
			sprintf(gcNewPasswd,"%.31s",CustomerName(entries[i].val));
		else if(!strcmp(entries[i].name,"gcBulkData"))
			gcBulkData=entries[i].val;
	}

}//void ProcessContainerVars(pentry entries[], int x)


void ContainerGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"guContainer"))
			sscanf(gentries[i].val,"%u",&guContainer);
		else if(!strcmp(gentries[i].name,"gcFunction"))
			sprintf(gcFunction,"%.99s",gentries[i].val);
	}

	if(guContainer && !gcFunction[0])
	{
		SelectContainer();
	}
	else if(guContainer && !strcmp(gcFunction,"QOSReport"))
	{
		SelectContainer();
		htmlContainerQOS();
	}

	htmlContainer();

}//void ContainerGetHook(entry gentries[],int x)


void ContainerCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"About"))
	{
		x=1;//this needs to be debugged
		ProcessContainerVars(entries,x);
		htmlAbout();
	}
	else if(!strcmp(gcPage,"Contact"))
	{
		x=1;
		ProcessContainerVars(entries,x);
		htmlContact();
	}
	else if(!strcmp(gcPage,"Container"))
	{
		unsigned uLen=0;
        	MYSQL_RES *res;
	        MYSQL_ROW field;
        	MYSQL_RES *res2;
	        MYSQL_ROW field2;

		ProcessContainerVars(entries,x);
		if(!strcmp(gcFunction,"Repurpose Container") && guPermLevel>5)
		{
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
				gcMessage="Selected container is being used by another process. Please try another or wait.";
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
			char cPrevHostname[100]={""};
			sprintf(gcQuery,"SELECT uContainer,uNode,uDatacenter,cHostname FROM tContainer WHERE uContainer=%u"
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
				sscanf(field[1],"%u",&guNode);
				sscanf(field[2],"%u",&guDatacenter);
				sprintf(cPrevHostname,"%.99s",field[3]);
			}
			if(mysql_num_rows(res)<1)
			{
				gcMessage="Selected container is not active. Please try another.";
				htmlContainer();
			}
			if(!guNode || !guDatacenter)
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
			sprintf(gcQuery,"SELECT cValue FROM tProperty"
				" WHERE uKey=%u AND uType=3 AND cName='cOrg_PublicIP'",guNewContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uProperty failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sprintf(cIPv4,"%.31s",field[0]);
			if(!cIPv4[0])
			{
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
			}
			if(strlen(cIPv4)<7)
			{
				gcMessage="Configuration error contact your sysadmin: cIPv4.";
				htmlContainer();
			}
			//debug only
			//char cDebugMsg[32]={"d1"};
			//sprintf(cDebugMsg,"%s",cIPv4);
			//gcMessage=cDebugMsg;
			//htmlContainer();

			unsigned uProperty=0;
			unsigned uOrgPropMaxLength=0;
			unsigned uOrgPropMinLength=0;
			char cOrgPropName[64]={"Unknown"};
			char cOrgPropType[64]={"Unknown"};
			char cMessage[255];
			char *cp;

			//
			//cNewHostParam0
			//
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
				if((cp=strstr(field[0],"uOrgPropMaxLength=")))
					sscanf(cp+strlen("uOrgPropMaxLength="),"%u",&uOrgPropMaxLength);
				if((cp=strstr(field[0],"uOrgPropMinLength=")))
					sscanf(cp+strlen("uOrgPropMinLength="),"%u",&uOrgPropMinLength);
				if((cp=strstr(field[0],"cOrgPropType=")))
				{
					sprintf(cOrgPropType,"%.63s",cp+strlen("cOrgPropType="));
					if((cp=strchr(cOrgPropType,';')))
						*cp=0;
				}
			}

			//Min
			if(strlen(gcNewHostParam0)<uOrgPropMinLength)
			{
				sprintf(cMessage,"%s must be at least %u characters long",cOrgPropName,uOrgPropMinLength);
				gcMessage=cMessage;
				htmlContainer();
			}
			//Max
			if(strlen(gcNewHostParam0)>uOrgPropMaxLength)
			{
				sprintf(cMessage,"%s can only be %u characters long",cOrgPropName,uOrgPropMaxLength);
				gcMessage=cMessage;
				htmlContainer();
			}
			//Check value if prop type is unsigned
			if(!strcmp(cOrgPropType,"unsigned"))
			{
				unsigned ugcNewHostParam0=0;
				unsigned ugcNewHostParam0Max=0;

				sscanf(gcNewHostParam0,"%u",&ugcNewHostParam0);
		
				ugcNewHostParam0Max=uPower10(uOrgPropMaxLength)-1;	
				if(ugcNewHostParam0==0 || ugcNewHostParam0>ugcNewHostParam0Max )
				{
					sprintf(cMessage,"%s does not appear to be a valid number or is larger than %u",
						cOrgPropName,ugcNewHostParam0Max);
					gcMessage=cMessage;
					htmlContainer();
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

			uOrgPropMaxLength=0;
			uOrgPropMinLength=0;
			//
			//cNewHostParam1
			//
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
				if((cp=strstr(field[0],"uOrgPropMaxLength=")))
					sscanf(cp+strlen("uOrgPropMaxLength="),"%u",&uOrgPropMaxLength);
				if((cp=strstr(field[0],"uOrgPropMinLength=")))
					sscanf(cp+strlen("uOrgPropMinLength="),"%u",&uOrgPropMinLength);
			}

			//Min
			if(strlen(gcNewHostParam1)<uOrgPropMinLength)
			{
				sprintf(cMessage,"%s must be at least %u characters long",cOrgPropName,uOrgPropMinLength);
				gcMessage=cMessage;
				htmlContainer();
			}
			//Max
			if(strlen(gcNewHostParam1)>uOrgPropMaxLength)
			{
				sprintf(cMessage,"%s can only be %u characters long",cOrgPropName,uOrgPropMaxLength);
				gcMessage=cMessage;
				htmlContainer();
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
		
			//Always update GMT
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
			//See HostnameContainerJob() in tcontainerfunc.h
			sprintf(gcQuery,"INSERT INTO tJob SET cLabel='CHCOrgMain(%u)',cJobName='ChangeHostnameContainer'"
					",uDatacenter=%u,uNode=%u,uContainer=%u"
					",uJobDate=UNIX_TIMESTAMP(NOW())+60"
					",uJobStatus=1"
					",cJobData='cPrevHostname=%.99s;'"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						guNewContainer,
						guDatacenter,guNode,guNewContainer,
						cPrevHostname,
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
			//CreateOrgDNSJob(unsigned uIPv4,unsigned uOwner,char const *cOptionalIPv4,char const *cHostname,
			//	unsigned uDatacenter,unsigned uCreatedBy,unsigned uContainer,unsigned uNode)
			char cZone[100]={""};
			sprintf(cZone,"%.31s.%.67s",gcNewHostname,cunxsBindARecordJobZone);
			CreateOrgDNSJob(0,guOrg,cIPv4,cZone,guDatacenter,guLoginClient,guNewContainer,guNode);

			//remove old dns entry
			char cView[32]={"external"};
			char cJobData[256]={""};
			GetConfiguration("cunxsBindARecordJobView",cView,guDatacenter,0,0,0);
			sprintf(cJobData,"cZone=%.99s;\n"
				"cView=%.31s;\n",
				cPrevHostname,cView);
			unxsBindRemoveContainer(guDatacenter,guNode,guNewContainer,cJobData,guOrg,guLoginClient);

			//If container has a remote datacenter backup change it's name also
			//	this will require hostname and dns jobs.
			unsigned uRemoteBackupContainer=0;
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.uDatacenter,tContainer.uNode,tIP.cLabel"
					" FROM tContainer,tIP"
					" WHERE tContainer.uIPv4=tIP.uIP"
					" AND tContainer.uSource=%u AND tContainer.uDatacenter!=%u",guNewContainer,guDatacenter);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select remote backup failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				unsigned uRemoteDatacenter=0;
				unsigned uRemoteNode=0;
				sscanf(field[0],"%u",&uRemoteBackupContainer);
				sscanf(field[1],"%u",&uRemoteDatacenter);
				sscanf(field[2],"%u",&uRemoteNode);
				if(uRemoteBackupContainer)
				{
					sprintf(cPrevHostname,"%.63s",ForeignKey("tContainer","cHostname",uRemoteBackupContainer));
					if(uUpdateNamesFromCloneToBackup(uRemoteBackupContainer))
					{
						
						CreateOrgDNSJob(0,guOrg,field[3],ForeignKey("tContainer","cHostname",uRemoteBackupContainer),
							uRemoteDatacenter,guLoginClient,uRemoteBackupContainer,uRemoteNode);
						sprintf(gcQuery,"INSERT INTO tJob SET cLabel='CHCOrgRemote(%u)',cJobName='ChangeHostnameContainer'"
							",uDatacenter=%u,uNode=%u,uContainer=%u"
							",uJobDate=UNIX_TIMESTAMP(NOW())+60"
							",uJobStatus=1"
							",cJobData='cPrevHostname=%.99s;'"
							",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
								uRemoteBackupContainer,
								uRemoteDatacenter,uRemoteNode,uRemoteBackupContainer,
								cPrevHostname,
								guOrg,guLoginClient);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							gcMessage="ChangeHostnameContainer insert failed, contact sysadmin!";
							htmlContainer();
						}
						SetContainerStatus(uRemoteBackupContainer,61);
					}
				}
			}

			//If container has a local datacenter clone change it's name also
			//	this will require hostname and dns jobs.
			unsigned uLocalCloneContainer=0;
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.uDatacenter,tContainer.uNode,tIP.cLabel"
					" FROM tContainer,tIP"
					" WHERE tContainer.uIPv4=tIP.uIP"
					" AND tContainer.uSource=%u AND tContainer.uDatacenter=%u",guNewContainer,guDatacenter);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select local clone failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				unsigned uLocalDatacenter=0;
				unsigned uLocalNode=0;
				sscanf(field[0],"%u",&uLocalCloneContainer);
				sscanf(field[1],"%u",&uLocalDatacenter);
				sscanf(field[2],"%u",&uLocalNode);
				if(uLocalCloneContainer)
				{
					sprintf(cPrevHostname,"%.63s",ForeignKey("tContainer","cHostname",uLocalCloneContainer));
					if(uUpdateNamesFromCloneToClone(uLocalCloneContainer))
					{
						CreateOrgDNSJob(0,guOrg,field[3],ForeignKey("tContainer","cHostname",uLocalCloneContainer),
							uLocalDatacenter,guLoginClient,uLocalCloneContainer,uLocalNode);
						sprintf(gcQuery,"INSERT INTO tJob SET cLabel='CHCOrgLocal(%u)',cJobName='ChangeHostnameContainer'"
							",uDatacenter=%u,uNode=%u,uContainer=%u"
							",uJobDate=UNIX_TIMESTAMP(NOW())+60"
							",uJobStatus=1"
							",cJobData='cPrevHostname=%.99s;'"
							",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
								uLocalCloneContainer,
								uLocalDatacenter,uLocalNode,uLocalCloneContainer,
								cPrevHostname,
								guOrg,guLoginClient);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							gcMessage="ChangeHostnameContainer insert failed, contact sysadmin!";
							htmlContainer();
						}
						SetContainerStatus(uLocalCloneContainer,61);
					}
				}
			}
			//local clone update dns and name

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

			//Attempt to change clone groups also.
			if(uRemoteBackupContainer)
			{
				sprintf(gcQuery,"UPDATE tGroupGlue SET uGroup=(SELECT uGroup FROM tGroup WHERE cLabel='%s' LIMIT 1)"
					" WHERE uContainer=%u AND uGroup=%u",cOrg_AfterNewGroupLabel,uRemoteBackupContainer,uGroup);
				mysql_query(&gMysql,gcQuery);
			}
			if(uLocalCloneContainer)
			{
				sprintf(gcQuery,"UPDATE tGroupGlue SET uGroup=(SELECT uGroup FROM tGroup WHERE cLabel='%s' LIMIT 1)"
					" WHERE uContainer=%u AND uGroup=%u",cOrg_AfterNewGroupLabel,uLocalCloneContainer,uGroup);
				mysql_query(&gMysql,gcQuery);
			}

			//if(guFlag)
			//{
			//	static char cMessage[256];
			//	sprintf(cMessage,"PBX repurpose in progress. guFlag=%u",guFlag);
			//	gcMessage=cMessage;
			//}
			if(uRemoteBackupContainer && uLocalCloneContainer)
				gcMessage="PBX repurpose in progress for master, clone and backup PBXs.";
			else if(uRemoteBackupContainer)
				gcMessage="PBX repurpose in progress for master and backup PBXs.";
			else if(uLocalCloneContainer)
				gcMessage="PBX repurpose in progress for master and clone PBXs.";
			else if(1)
				gcMessage="PBX repurpose in progress for master PBXs only.";

			//for all the above
			mysql_free_result(res);

			guContainer=guNewContainer;
			htmlContainer();
		}//Repurpose Container ~480 lines
		else if((!strcmp(gcFunction,"Container Report")||!strcmp(gcFunction,"Show Containers"))&&guPermLevel>5)
		{

			printf("Content-type: text/plain\n\n");
			printf("uContainer,cLabel,cHostname,cDatacenter,cNode,cGroup,cTemplate,cStatus\n");

			if(guReseller)
			sprintf(gcQuery,"SELECT tContainer.uContainer,"
					" tContainer.cLabel,"
					" tContainer.cHostname,"
					" tDatacenter.cLabel,"
					" tNode.cLabel,"
					" tOSTemplate.cLabel,"
					" tStatus.cLabel"
					" FROM tContainer,tDatacenter,tNode,tOSTemplate,tStatus"
					" WHERE tContainer.uDatacenter=tDatacenter.uDatacenter AND"
					" tContainer.uNode=tNode.uNode AND"
					" tContainer.uStatus=tStatus.uStatus AND"
					" tOSTemplate.uOSTemplate=tContainer.uOSTemplate AND"
					" tContainer.uSource=0 AND tContainer.uCreatedBy=%u",guReseller);
			else
			sprintf(gcQuery,"SELECT tContainer.uContainer,"
					" tContainer.cLabel,"
					" tContainer.cHostname,"
					" tDatacenter.cLabel,"
					" tNode.cLabel,"
					" tOSTemplate.cLabel,"
					" tStatus.cLabel"
					" FROM tContainer,tDatacenter,tNode,tOSTemplate,tStatus"
					" WHERE tContainer.uDatacenter=tDatacenter.uDatacenter AND"
					" tContainer.uNode=tNode.uNode AND"
					" tContainer.uStatus=tStatus.uStatus AND"
					" tOSTemplate.uOSTemplate=tContainer.uOSTemplate AND"
					" tContainer.uSource=0");
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

				printf("%s,%s,%s,%s,%s,%s,%s,%s\n",
					field[0],field[1],field[2],field[3],field[4],cGroup,field[5],field[6]);
			}
			mysql_free_result(res);
			exit(0);
		}
		else if(!strcmp(gcFunction,"Upgrade Container") && guPermLevel>5)
		{
			char cAuthCode[32]={"PsW3jGd"};
			char cTag[32]={""};

			sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uDatacenter=0"
					" AND uContainer=0 AND uNode=0 AND cLabel='cOrg_UpgradeAuthCode'");
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select cOrg_UpgradeAuthCode failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sprintf(cAuthCode,"%.31s",field[0]);

			if(strcmp(gcAuthCode,cAuthCode))
			{
				gcMessage="Authorization code is not valid";
				htmlContainer();
			}

			sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uType=3 AND uKey=%u AND"
					" cName='cOrg_LifecycleTag'",guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Check for cOrg_LifecycleTag failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sprintf(cTag,"%.31s",field[0]);
			mysql_free_result(res);
			if(strcmp(cTag,"Upgrade"))
			{
				//Tag container for upgrade
				sprintf(gcQuery,"INSERT INTO tProperty"
					" SET cValue='Upgrade',"
					"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),"
					"uKey=%u,uType=3,cName='cOrg_LifecycleTag'"
						,guOrg,guLoginClient,guContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="INSERT for cOrg_LifecycleTag failed, contact sysadmin!";
					htmlContainer();
				}
			}
			else
			{
				gcMessage="Container already tagged for upgrade";
				htmlContainer();
			}

			sprintf(gcCtHostname,"%.99s",(char *)cGetHostname(guContainer));
			sprintf(gcQuery,"Container %s (%u) flagged for upgrade by %s\n",gcCtHostname,guContainer,gcName);
			SendAlertEmail(gcQuery);

			gcMessage="upgrade break point 1";
			htmlContainer();
		}
		else if(!strcmp(gcFunction,"Cancel Container") && guPermLevel>5)
		{
			char cAuthCode[32]={"lwXM722Ki"};
			char cTag[32]={""};

			sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uDatacenter=0"
					" AND uContainer=0 AND uNode=0 AND cLabel='cOrg_CancelAuthCode'");
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select cOrg_UpgradeAuthCode failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sprintf(cAuthCode,"%.31s",field[0]);

			if(strcmp(gcAuthCode,cAuthCode))
			{
				gcMessage="Authorization code is not valid";
				htmlContainer();
			}

			sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uType=3 AND uKey=%u AND"
					" cName='cOrg_LifecycleTag'",guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Check for cOrg_LifecycleTag failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sprintf(cTag,"%.31s",field[0]);
			mysql_free_result(res);
			if(strcmp(cTag,"Cancel"))
			{
				//Tag container for upgrade
				sprintf(gcQuery,"INSERT INTO tProperty"
					" SET cValue='Cancel',"
					"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),"
					"uKey=%u,uType=3,cName='cOrg_LifecycleTag'"
						,guOrg,guLoginClient,guContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="INSERT for cOrg_LifecycleTag failed, contact sysadmin!";
					htmlContainer();
				}
			}
			else
			{
				gcMessage="Container already tagged for cancel";
				htmlContainer();
			}

			sprintf(gcCtHostname,"%.99s",(char *)cGetHostname(guContainer));
			sprintf(gcQuery,"Container %s (%u) flagged for cancel by %s\n",gcCtHostname,guContainer,gcName);
			SendAlertEmail(gcQuery);

			gcMessage="cancel break point 1";
			htmlContainer();
		}
		else if(!strcmp(gcFunction,"Mod CustomerName") && ( gcCustomerName[0] || gcCustomerLimit[0]) && guPermLevel>5)
		{
			char gcQuery[1024];

			if(!guContainer)
			{
				gcMessage="Must select a container.";
				htmlContainer();
			}
			unsigned uCustomerLimit=0;
			sscanf(gcCustomerLimit,"%u",&uCustomerLimit);
			if((uLen=strlen(gcCustomerName))<3 && !uCustomerLimit)
			{
				gcMessage="Customer name must be at least 3 characters long.";
				htmlContainer();
			}
			if(!gcCustomerName[0] && !uCustomerLimit)
			{
				gcMessage="Must provide valid numeric customer line limit if not providing new customer name.";
				htmlContainer();
			}
			if(uCustomerLimit>32)
			{
				gcMessage="Special authorization required for limits above 32";
				htmlContainer();
			}
			if(uLen>32)
			{
				gcMessage="Customer name length exceeded.";
				htmlContainer();
			}

			//uStatus must be active or offline or appliance
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
				sscanf(field[1],"%u",&guNode);
				sscanf(field[2],"%u",&guDatacenter);
			}
			if(mysql_num_rows(res)<1)
			{
				gcMessage="Selected container status is incorrect.";
				htmlContainer();
			}
			if(!guNode || !guDatacenter)
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

			//check to stop extra work
			if(!uCustomerLimit && gcCustomerName[0])
			{
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
			}
			else if(uCustomerLimit && !gcCustomerName[0])
			{
				//Check to see if customer limit is already in property table
				sprintf(gcQuery,"SELECT uProperty FROM tProperty"
						" WHERE uKey=%u AND uType=3 AND cName='cOrg_LinesContracted'"
						" AND cValue='%u'",guContainer,uCustomerLimit);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="Check for cOrg_CustomerLimit failed, contact sysadmin!";
					htmlContainer();
				}
				res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
				{
					mysql_free_result(res);
					gcMessage="Customer limit not changed, already in property table.";
					htmlContainer();
				}
				mysql_free_result(res);
			}
			else if(uCustomerLimit && gcCustomerName[0])
			{
				//Check to see if customer name is already in property table
				sprintf(gcQuery,"SELECT uProperty FROM tProperty"
						" WHERE uKey=%u AND uType=3 AND cName='cOrg_LinesContracted'"
						" AND cValue='%u'",guContainer,uCustomerLimit);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="Check for cOrg_CustomerLimit failed, contact sysadmin!";
					htmlContainer();
				}
				res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
				{
					mysql_free_result(res);
					gcMessage="Customer limit not changed, already in property table.";
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
						gcMessage="Customer name and limit have not changed, already in property table.";
						htmlContainer();
					}
					mysql_free_result(res);
				}
				mysql_free_result(res);
			}

			char cSIPProxyList[256]={""};
			GetSIPProxyList(cSIPProxyList,guDatacenter,guNode,guContainer);
			if(!cSIPProxyList[0])
			{
				gcMessage="No cSIPProxyList, contact sysadmin!";
				htmlContainer();
			}

			//both cases if not specifying new gcCustomerName we use the old one.
			if(gcCustomerName[0])
				sprintf(gcQuery,"INSERT INTO tProperty"
					" SET cValue='%s',"
					"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),"
					"uKey=%u,uType=3,cName='cOrg_CustomerMod'"
						,gcCustomerName,guOrg,guLoginClient,guContainer);
			else
				sprintf(gcQuery,"INSERT INTO tProperty"
					" SET cValue='new limit=%u',"
					"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),"
					"uKey=%u,uType=3,cName='cOrg_CustomerMod'"
						,uCustomerLimit,guOrg,guLoginClient,guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="INSERT for cOrg_CustomerMod failed, contact sysadmin!";
				htmlContainer();
			}

			if(uCustomerLimit)
			{
				sprintf(gcQuery,"UPDATE tProperty"
					" SET cValue='%u'"
					" WHERE uKey=%u AND uType=3 AND cName='cOrg_LinesContracted'"
						,uCustomerLimit,guContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="UPDATE for cOrg_LinesContracted failed, contact sysadmin!";
					htmlContainer();
				}
			}

			//unxsSIPS jobs customer mod limit change for example
			register int i,j=0;
			unsigned uDelayInSecs=0;
			for(i=0;cSIPProxyList[i] && cSIPProxyList[i]!='#' && cSIPProxyList[i]!='\r';i++)
			{
				if(cSIPProxyList[i]!=';')
					continue;
				cSIPProxyList[i]=0;
				sprintf(gcServer,"%.31s",cSIPProxyList+j);
				j=i+1;//next beg if app
				cSIPProxyList[i]=';';
				sprintf(gcCtHostname,"%.99s",(char *)cGetHostname(guContainer));
				uDelayInSecs+=60;//space cluster one minute to help avoid replication fail issues in master-master systems.
				sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsSIPSModCustomerName(%u)',cJobName='unxsSIPSModCustomerName'"
						",uDatacenter=%u,uNode=%u,uContainer=%u"
						",uJobDate=UNIX_TIMESTAMP(NOW())+%u"
						",uJobStatus=%u"
						",cJobData='"
						"cServer=%s;\n"
						"cCustomerName=%s;\n"
						"uCustomerLimit=%u;\n"
						"cHostname=%s;\n'"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							guContainer,
							guDatacenter,
							guNode,
							guContainer,
							uDelayInSecs,
							uREMOTEWAITING,
							gcServer,
							gcCustomerName,
							uCustomerLimit,
							gcCtHostname,
							guOrg,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="unxsSIPSModCustomerName tJob insert failed, contact sysadmin!";
					htmlContainer();
				}
			}

			gcMessage="Remote 'Mod CustomerName' task created for OpenSIPS.";
			htmlContainer();
		}//Mod CustomerName

		else if(!strcmp(gcFunction,"Bulk Operations") && guContainer && guPermLevel>5)
		{
			gcMessage="Bulk Operations";
			htmlContainerBulk();
		}//Bulk Operations
		else if(!strcmp(gcFunction,"Load All DIDs") && guContainer && guPermLevel>5)
		{
			gcMessage="All DIDs loaded";
			LoadAllDIDs();
		}//Load All DIDs
		else if(!strcmp(gcFunction,"Bulk Remove DIDs") && guContainer && guPermLevel>5)
		{
			if(strlen(gcBulkData)<10)
			{
				gcMessage="You must provide a valid list of DIDs to remove";
				htmlContainerBulk();
			}
			DIDOpsCommonChecking();
			guMode=1;
			htmlContainerBulk();
		}
		else if(!strcmp(gcFunction,"Confirm Bulk Remove DIDs") && guContainer && guPermLevel>5)
		{
			if(strlen(gcBulkData)<10)
			{
				gcMessage="You must provide a valid list of DIDs to remove";
			}
			DIDOpsCommonChecking();
			guMode=0;
			BulkDIDRemove();
		}//Bulk Remove DIDs
		else if(!strcmp(gcFunction,"Bulk Add DIDs") && guContainer && guPermLevel>5)
		{
			if(strlen(gcBulkData)<10)
			{
				gcMessage="You must provide a valid list of DIDs to add";
				htmlContainerBulk();
			}
			DIDOpsCommonChecking();
			BulkDIDAdd();
		}//Bulk Add DIDs

		else if(!strcmp(gcFunction,"Add DID") && gcDID[0] && guPermLevel>5)
		{
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

			DIDOpsCommonChecking();

			//Check to see if DID is already in property table
			unsigned uInsert=1;//normally insert
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
				uInsert=0;
				//mysql_free_result(res);
				//gcMessage="DID not added, already in property table.";
				//htmlContainer();
			}

			//business logic rule: no two containers can deploy same DID
			sprintf(gcQuery,"SELECT uProperty,uKey FROM tProperty"
					" WHERE uKey!=%u AND uType=3 AND (cName='cOrg_OpenSIPS_DID' OR cName='cOrg_Pending_DID')"
					" AND cValue='%s'",guContainer,gcDID);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				strncat(cReply," select error 1\n",15);
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)>0)
			{
				mysql_free_result(res);
				gcMessage="DUP DID not added, already in property table for other container.";
				htmlContainer();
			}

			sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tContainer WHERE uContainer=%u",guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uNode error, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&guNode);
				sscanf(field[1],"%u",&guDatacenter);
			}
			mysql_free_result(res);

			char cSIPProxyList[256]={""};
			GetSIPProxyList(cSIPProxyList,guDatacenter,guNode,guContainer);
			if(!cSIPProxyList[0])
			{
				gcMessage="No cSIPProxyList, contact sysadmin!";
				htmlContainer();
			}

			if(uInsert)
			{
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
			}

			//unxsSIPS jobs "Add DID"
			register int i,j=0;
			unsigned uDelayInSecs=0;
			for(i=0;cSIPProxyList[i] && cSIPProxyList[i]!='#' && cSIPProxyList[i]!='\r';i++)
			{
				if(cSIPProxyList[i]!=';')
					continue;
				cSIPProxyList[i]=0;
				sprintf(gcServer,"%.31s",cSIPProxyList+j);
				j=i+1;//next beg if app
				cSIPProxyList[i]=';';
				sprintf(gcCtHostname,"%.99s",(char *)cGetHostname(guContainer));
				uDelayInSecs+=60;
				sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsSIPSNewDID(%u)',cJobName='unxsSIPSNewDID'"
						",uDatacenter=%u,uNode=%u,uContainer=%u"
						",uJobDate=UNIX_TIMESTAMP(NOW())+%u"
						",uJobStatus=%u"
						",cJobData='"
						"cServer=%s;\n"
						"cDID=%s;\n"
						"cHostname=%s;\n'"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							guContainer,
							guDatacenter,
							guNode,
							guContainer,
							uDelayInSecs,
							uREMOTEWAITING,
							gcServer,
							gcDID,
							gcCtHostname,
							guOrg,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="unxsSIPSNewDID tJob insert failed, contact sysadmin!";
					htmlContainer();
				}
			}

			gcMessage="Remote 'Add DID' task created for OpenSIPS.";
			htmlContainer();
		}//Add DID
		else if(!strcmp(gcFunction,"Remove from Proxy") && guPermLevel>5)
		{
			char gcQuery[1024];

			if(!guContainer)
			{
				gcMessage="Must select a container.";
				htmlContainer();
			}

			DIDOpsCommonChecking();

			sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tContainer WHERE uContainer=%u",guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uNode error, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&guNode);
				sscanf(field[1],"%u",&guDatacenter);
			}
			mysql_free_result(res);

			char cSIPProxyList[256]={""};
			GetSIPProxyList(cSIPProxyList,guDatacenter,guNode,guContainer);
			if(!cSIPProxyList[0])
			{
				gcMessage="No cSIPProxyList, contact sysadmin!";
				htmlContainer();
			}

			//unxsSIPS jobs Remove
			register int i,j=0;
			unsigned uDelayInSecs=0;

			for(i=0;cSIPProxyList[i] && cSIPProxyList[i]!='#' && cSIPProxyList[i]!='\r';i++)
			{
				if(cSIPProxyList[i]!=';')
					continue;
				cSIPProxyList[i]=0;
				sprintf(gcServer,"%.31s",cSIPProxyList+j);
				j=i+1;//next beg if app
				cSIPProxyList[i]=';';
				sprintf(gcCtHostname,"%.99s",(char *)cGetHostname(guContainer));
				uDelayInSecs+=60;
				sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsSIPSRemoveFromProxy(%u)',cJobName='unxsSIPSRemoveFromProxy'"
						",uDatacenter=%u,uNode=%u,uContainer=%u"
						",uJobDate=UNIX_TIMESTAMP(NOW())+%u"
						",uJobStatus=%u"
						",cJobData='"
						"cServer=%s;\n"
						"cHostname=%s;\n'"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							guContainer,
							guDatacenter,
							guNode,
							guContainer,
							uDelayInSecs,
							uREMOTEWAITING,
							gcServer,
							gcCtHostname,
							guOrg,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="unxsSIPSRemoveFromProxy tJob insert failed, contact sysadmin!";
					htmlContainer();
				}
			}

			gcMessage="Remote 'RemoveFromProxy' task created for OpenSIPS.";
			htmlContainer();

		}
		else if(!strcmp(gcFunction,"Remove DID") && gcDID[0] && guPermLevel>5)
		{
			char gcQuery[1024];

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

			DIDOpsCommonChecking();

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

			sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tContainer WHERE uContainer=%u",guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uNode error, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&guNode);
				sscanf(field[1],"%u",&guDatacenter);
			}
			mysql_free_result(res);

			char cSIPProxyList[256]={""};
			GetSIPProxyList(cSIPProxyList,guDatacenter,guNode,guContainer);
			if(!cSIPProxyList[0])
			{
				gcMessage="No cSIPProxyList, contact sysadmin!";
				htmlContainer();
			}

			sprintf(gcQuery,"UPDATE tProperty SET cName='cOrg_Remove_DID',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
					" WHERE uKey=%u AND uType=3 AND cValue='%s' AND cName='cOrg_OpenSIPS_DID'",
						guLoginClient,guContainer,gcDID);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="UPDATE for cOrg_OpenSIPS_DID failed, contact sysadmin!";
				htmlContainer();
			}

			//unxsSIPS jobs Remove DID
			register int i,j=0;
			unsigned uDelayInSecs=0;

			for(i=0;cSIPProxyList[i] && cSIPProxyList[i]!='#' && cSIPProxyList[i]!='\r';i++)
			{
				if(cSIPProxyList[i]!=';')
					continue;
				cSIPProxyList[i]=0;
				sprintf(gcServer,"%.31s",cSIPProxyList+j);
				j=i+1;//next beg if app
				cSIPProxyList[i]=';';
				sprintf(gcCtHostname,"%.99s",(char *)cGetHostname(guContainer));
				uDelayInSecs+=60;
				sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsSIPSRemoveDID(%u)',cJobName='unxsSIPSRemoveDID'"
						",uDatacenter=%u,uNode=%u,uContainer=%u"
						",uJobDate=UNIX_TIMESTAMP(NOW())+%u"
						",uJobStatus=%u"
						",cJobData='"
						"cServer=%s;\n"
						"cDID=%s;\n"
						"cHostname=%s;\n'"
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							guContainer,
							guDatacenter,
							guNode,
							guContainer,
							uDelayInSecs,
							uREMOTEWAITING,
							gcServer,
							gcDID,
							gcCtHostname,
							guOrg,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="unxsSIPSRemoveDID tJob insert failed, contact sysadmin!";
					htmlContainer();
				}
			}

			gcMessage="Remote 'Remove DID' task created for OpenSIPS.";
			htmlContainer();
		}//Remove DID
		else if(!strcmp(gcFunction,"Search DID") && gcSearchAux[0] && guPermLevel>5)
		{
			char gcQuery[256];
			unsigned uNumRows=0;

			gcSearchName="cOrg_OpenSIPS_DID";
			sprintf(gcQuery,"SELECT uKey FROM tProperty,tContainer WHERE uKey=tContainer.uContainer AND"
					" (uStatus=1 OR uStatus=3 OR uStatus=101) AND"
					" cValue LIKE '%.31s%%' AND"
					" cName='%s' AND uType=3 GROUP BY tContainer.cHostname LIMIT 2",gcSearchAux,gcSearchName);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uContainer error, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((uNumRows=mysql_num_rows(res))<1)
			{
				gcMessage="No container with specied DID pattern found.";
				htmlContainer();
			}
			if(uNumRows>1)
			{
				htmlContainerList();
			}
			else
			{
				if((field=mysql_fetch_row(res)))
				{
					sscanf(field[0],"%u",&guContainer);
				}
			}
			mysql_free_result(res);
			gcMessage="Found single container based on DID pattern.";
		}//Search DID
		else if(!strcmp(gcFunction,"Search Customer") && gcSearchAux[0] && guPermLevel>5)
		{
			char gcQuery[256];
			unsigned uNumRows=0;

			gcSearchName="cOrg_CustomerName";
			sprintf(gcQuery,"SELECT uKey FROM tProperty,tContainer WHERE uKey=tContainer.uContainer AND"
					" (uStatus=1 OR uStatus=3 OR uStatus=101) AND"
					" cValue LIKE '%.31s%%' AND"
					" cName='%s' AND uType=3 GROUP BY tContainer.cHostname LIMIT 2",gcSearchAux,gcSearchName);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uContainer error, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((uNumRows=mysql_num_rows(res))<1)
			{
				gcMessage="No container with specied customer pattern found.";
				htmlContainer();
			}
			if(uNumRows>1)
			{
				htmlContainerList();
			}
			else
			{
				if((field=mysql_fetch_row(res)))
				{
					sscanf(field[0],"%u",&guContainer);
				}
			}
			mysql_free_result(res);
			gcMessage="Found single container based on customer pattern.";
		}//Search Customer
		else if(!strcmp(gcFunction,"DID Report") && guPermLevel>=10)
		{
			printf("Content-type: text/plain\n\n");

			printf("cHostname,cDID\n");
			sprintf(gcQuery,"SELECT"
					" tContainer.cHostname,"
					" tProperty.cValue"
					" FROM tContainer,tProperty"
					" WHERE tContainer.uSource=0 AND"
					" tContainer.uContainer=tProperty.uKey AND"
					" tProperty.uType=3 AND"
					" tProperty.cName='cOrg_OpenSIPS_DID' ORDER BY tContainer.cHostname");
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			while((field=mysql_fetch_row(res)))
			{
				printf("%s,%s\n",field[0],field[1]);
			}
			mysql_free_result(res);

			printf("\ncHostname,cExtension\n");
			sprintf(gcQuery,"SELECT"
					" tContainer.cHostname,"
					" tProperty.cValue"
					" FROM tContainer,tProperty"
					" WHERE tContainer.uSource=0 AND"
					" tContainer.uContainer=tProperty.uKey AND"
					" tProperty.uType=3 AND"
					" tProperty.cName='cOrg_Extension' ORDER BY tContainer.cHostname");
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			while((field=mysql_fetch_row(res)))
			{
				printf("%s,%s\n",field[0],field[1]);
			}
			mysql_free_result(res);

			printf("\ncHostname,cTrunk\n");
			sprintf(gcQuery,"SELECT"
					" tContainer.cHostname,"
					" tProperty.cValue"
					" FROM tContainer,tProperty"
					" WHERE tContainer.uSource=0 AND"
					" tContainer.uContainer=tProperty.uKey AND"
					" tProperty.uType=3 AND"
					" tProperty.cName='cOrg_SIPTrunk' ORDER BY tContainer.cHostname");
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			while((field=mysql_fetch_row(res)))
			{
				printf("%s,%s\n",field[0],field[1]);
			}
			mysql_free_result(res);

			printf("\ncHostname,cFreePBXVersion\n");
			sprintf(gcQuery,"SELECT"
					" tContainer.cHostname,"
					" tProperty.cValue"
					" FROM tContainer,tProperty"
					" WHERE tContainer.uSource=0 AND"
					" tContainer.uContainer=tProperty.uKey AND"
					" tProperty.uType=3 AND"
					" tProperty.cName='cOrg_FreePBXVersion' ORDER BY tContainer.cHostname");
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			while((field=mysql_fetch_row(res)))
			{
				printf("%s,%s\n",field[0],field[1]);
			}
			mysql_free_result(res);

			exit(0);
		}//DID Report
		else if(!strcmp(gcFunction,"QOS Report"))
		{
				htmlContainerQOS();
		}
		else if(!strcmp(gcFunction,"Add Container") && guPermLevel>5 && guContainer && guReseller)
		{
			//uStatus must be active or offline or appliance
			sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tContainer WHERE uContainer=%u",guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Select uNode error, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&guNode);
				sscanf(field[1],"%u",&guDatacenter);
			}
			if(mysql_num_rows(res)<1)
			{
				gcMessage="Selected container status is incorrect.";
				htmlContainer();
			}
			if(!guNode || !guDatacenter)
			{
				gcMessage="No uNode or no uDatacenter error. Contact your sysadmin!";
				htmlContainer();
			}

			sprintf(gcQuery,"UPDATE tContainer"
					" SET uCreatedBy=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uContainer=%u",
						guReseller,guLoginClient,guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="UPDATE tContainer failed, contact sysadmin!";
				htmlContainer();
			}

			gcMessage="Current container added to user account";
			htmlContainer();
		}
		else if(!strcmp(gcFunction,"Del Container") && guPermLevel>5 && guContainer && guReseller)
		{
			sprintf(gcQuery,"UPDATE tContainer"
					" SET uCreatedBy=1,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uContainer=%u AND uCreatedBy=%u",
						guLoginClient,guContainer,guReseller);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="UPDATE tContainer failed, contact sysadmin!";
				htmlContainer();
			}

			gcMessage="Current container removed from user account";
			htmlContainer();
		}
		else if(!strcmp(gcFunction,"Create Account") && gcNewLogin[0] && gcNewPasswd[0] && guPermLevel>5)
		{
			char gcQuery[1024];

			if((uLen=strlen(gcNewLogin))<5 || strchr(gcNewLogin,' ')==NULL || gcNewLogin[strlen(gcNewLogin)-1]==' ')
			{
				gcMessage="Customer login name must be at least two words, total min 5 characters long"
						" and have no trailing spaces.";
				htmlContainer();
			}
			if(uLen>31)
			{
				gcMessage="Customer login length exceeded.";
				htmlContainer();
			}
			if((uLen=strlen(gcNewPasswd))<6 || strchr(gcNewPasswd,' ')!=NULL)
			{
				gcMessage="Customer password must be at least 6 characters long and have no spaces.";
				htmlContainer();
			}
			if(uLen>31)
			{
				gcMessage="Customer password length exceeded.";
				htmlContainer();
			}

			sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cLabel='%s'",gcNewLogin);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="SELECT FROM tClient failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)>0)
			{
				gcMessage="Account login already exists choose another";
				htmlContainer();
			}

			sprintf(gcQuery,"SELECT uAuthorize FROM tAuthorize WHERE cLabel='%s'",gcNewLogin);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="SELECT FROM tAuthorize failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)>0)
			{
				gcMessage="Account login already exists choose another";
				htmlContainer();
			}

			sprintf(gcQuery,"INSERT INTO tClient"
					" SET cLabel='%.31s',cCode='Contact',cInfo='Org interface created',"
					"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						gcNewLogin,guOrg,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="INSERT INTO tClient failed, contact sysadmin!";
				htmlContainer();
			}

			unsigned uClient=0;
			uClient=mysql_insert_id(&gMysql);

			if(!uClient)
			{
				gcMessage="No uClient, contact sysadmin!";
				htmlContainer();
			}

			char cBuffer[100]={""};
			char cSalt[16]={"$1$23ABC823$"};//TODO set to random salt;

			sprintf(cBuffer,"%.99s",gcNewPasswd);
			EncryptPasswdWithSalt(cBuffer,cSalt);

			sprintf(gcQuery,"INSERT INTO tAuthorize"
					" SET cLabel='%.31s',cIpMask='%.15s',uPerm=1,uCertClient=%u,cPasswd='%s',cClrPasswd='%.31s',"
					"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						gcNewLogin,gcHost,uClient,cBuffer,gcNewPasswd,
						guOrg,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="INSERT INTO tAuthorize failed, contact sysadmin!";
				htmlContainer();
			}

			guReseller=uClient;
			gcMessage="New account created you can now add containers";
			htmlContainer();
		}//Create Account
		else if(!strcmp(gcFunction,"Remove Account") && guReseller && guPermLevel>=10)
		{
			sprintf(gcQuery,"SELECT uClient FROM tClient WHERE uClient=%u",guReseller);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="SELECT FROM tClient failed, contact sysadmin!";
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)!=1)
			{
				gcMessage="Account does not exist!";
				htmlContainer();
			}

			sprintf(gcQuery,"UPDATE tContainer SET uCreatedBy=1 WHERE uCreatedBy=%u",guReseller);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="UPDATE tContainer failed, contact sysadmin!";
				htmlContainer();
			}
			sprintf(gcQuery,"DELETE FROM tClient WHERE uClient=%u",guReseller);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="DELETE FROM tClient failed, contact sysadmin!";
				htmlContainer();
			}
			sprintf(gcQuery,"DELETE FROM tAuthorize WHERE uCertClient=%u",guReseller);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="DELETE FROM tAuthorized failed, contact sysadmin!";
				htmlContainer();
			}

			guReseller=0;
			gcMessage="Account removed!";
			htmlContainer();
		}//Remove Account

		htmlContainer();
	}

}//void ContainerCommands(pentry entries[], int x)


void htmlAbout(void)
{
	htmlHeader("OneLogin","ContainerHeader");
	htmlAuxPage("OneLogin","About.Body");
	htmlFooter("ContainerFooter");

}//void htmlAbout(void)


void htmlContact(void)
{
	htmlHeader("OneLogin","ContainerHeader");
	htmlAuxPage("OneLogin","Contact.Body");
	htmlFooter("ContainerFooter");

}//void htmlContact(void)


void htmlContainer(void)
{
	htmlHeader("OneLogin","ContainerHeader");
	if(guContainer)
		SelectContainer();
	htmlContainerPage("OneLogin","Container.Body");
	htmlFooter("ContainerFooter");

}//void htmlContainer(void)


void htmlContainerList(void)
{
	htmlHeader("OneLogin","Header");
	htmlContainerPage("OneLogin","ContainerList.Body");
	htmlFooter("Footer");

}//void htmlContainerList(void)


void htmlAuxPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;
		unsigned uNoShow=0;

		TemplateSelectInterface(cTemplateName,uPLAINSET,uOneLogin);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="OneLogin.cgi";
			
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
			if(!uNoShow)
				template.cpValue[8]=(char *)cGetHostname(guContainer) ;
			else
				template.cpValue[8]="no container selected";

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

			template.cpName[13]="gcSearchAux";
			template.cpValue[13]=gcSearchAux;

			template.cpName[14]="";

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

}//void htmlAuxPage()


void htmlContainerBulk(void)
{
	htmlHeader("OneLogin","Header");
	htmlContainerPage("OneLogin","ContainerBulk.Body");
	htmlFooter("Footer");

}//void htmlContainerBulk(void)


void htmlContainerQOS(void)
{
	htmlHeader("OneLogin","Header");
	htmlContainerPage("OneLogin","ContainerQOS.Body");
	htmlFooter("Footer");

}//void htmlContainerQOS(void)


void htmlContainerPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;
		unsigned uNoShow=0;

/*
		if(guPermLevel<6)
		{
			sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uCreatedBy=%u AND uContainer=%u",guLoginClient,guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)<1)
				uNoShow=1;
		}
*/

		TemplateSelectInterface(cTemplateName,uPLAINSET,uOneLogin);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="OneLogin.cgi";
			
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
			if(!uNoShow)
				template.cpValue[8]=(char *)cGetHostname(guContainer) ;
			else
				template.cpValue[8]="no container selected";

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

			template.cpName[13]="gcSearchAux";
			template.cpValue[13]=gcSearchAux;

			MYSQL_RES *res0;
			MYSQL_ROW field0;
			char cQuery[128];
			sprintf(cQuery,"SELECT cValue FROM tProperty WHERE uType=3 AND uKey=%u AND cName='cOrg_AdminPort'",guContainer);
			mysql_query(&gMysql,cQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res0=mysql_store_result(&gMysql);
			if((field0=mysql_fetch_row(res0)))
				sprintf(gcAdminPort,":%.14s",field0[0]);
			mysql_free_result(res0);
			template.cpName[14]="gcAdminPort";
			template.cpValue[14]=gcAdminPort;

			template.cpName[15]="";

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

	guStatus=0;
	sprintf(gcQuery,"SELECT uStatus FROM tContainer WHERE uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&guStatus);
	mysql_free_result(res);
	if(guStatus && guStatus!=uREMOTEAPPLIANCE && guStatus!=uAWAITACT)
		//fprintf(fp,"<a href=https://%s%s/admin ><img src=%s/traffic/%u.png border=0 onerror=\"this.src='/images/noimage.jpg'\"></a>",
		//	cGetHostname(guContainer),gcAdminPort,cGetImageHost(guContainer),guContainer);
		fprintf(fp,"<a href=https://%s%s/admin ><img src=/traffic/%u.png border=0 onerror=\"this.src='/images/noimage.jpg'\"></a>",
			cGetHostname(guContainer),gcAdminPort,guContainer);

}//void funcContainerImageTag(FILE *fp)


void funcSelectContainer(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCount=1;
	unsigned uContainer=0;

	fprintf(fp,"<!-- funcSelectContainer(fp) Start -->\n");

	if(guPermLevel>5)
	{
		if(gcSearch[0])
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cHostname FROM tContainer,tNode WHERE"
			" tContainer.uNode=tNode.uNode AND tNode.uStatus=1 AND"
			" (tContainer.uSource=0 OR tContainer.uStatus=1) AND"
			" (tContainer.uOwner IN"
			" (SELECT uClient FROM tClient WHERE"
			" 	((uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u) OR uOwner=%u) AND cCode='Organization'))"
			" OR tContainer.uOwner=%u) AND"
			" tContainer.cHostname LIKE '%s%%'"
			" AND tContainer.uStatus!=91"
			" ORDER BY tContainer.cHostname LIMIT 301",guOrg,guOrg,guOrg,gcSearch);
		else if(guContainer)
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cHostname FROM tContainer,tNode WHERE"
			" tContainer.uNode=tNode.uNode AND tNode.uStatus=1 AND"
			" (tContainer.uSource=0 OR tContainer.uStatus=1) AND"
			" (tContainer.uOwner IN"
			" (SELECT uClient FROM tClient WHERE ((uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u)"
			" 	OR uOwner=%u) AND " " 	cCode='Organization'))"
			" OR tContainer.uOwner=%u) AND"
			" tContainer.uStatus!=91 AND"
			" tContainer.uContainer=%u"
			" ORDER BY tContainer.cHostname LIMIT 301",guOrg,guOrg,guOrg,guContainer);
		else
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cHostname FROM tContainer,tNode WHERE"
			" tContainer.uNode=tNode.uNode AND tNode.uStatus=1 AND"
			" (tContainer.uSource=0 OR tContainer.uStatus=1) AND"
			" (tContainer.uOwner IN"
			" (SELECT uClient FROM tClient WHERE ((uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u)"
			" 	OR uOwner=%u) AND " " 	cCode='Organization'))"
			" OR tContainer.uOwner=%u) AND"
			" tContainer.uStatus!=91"
			" ORDER BY tContainer.cHostname LIMIT 301",guOrg,guOrg,guOrg);
	}
	else if(guOrg!=2)//ASP uClient hack
	{
		if(gcSearch[0])
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cHostname FROM tContainer,tNode WHERE"
			" tContainer.uNode=tNode.uNode AND tNode.uStatus=1 AND"
			" tContainer.uSource=0 AND"
			" (tContainer.uOwner IN"
			" (SELECT uClient FROM tClient WHERE (((uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u) OR uOwner=%u) AND "
			" 	cCode='Organization')) OR uOwner=%u)"
			" OR (tContainer.uOwner=%u OR tContainer.uCreatedBy=%u)) AND"
			" tContainer.cHostname LIKE '%s%%' AND"
			" tContainer.uStatus!=91"
			" ORDER BY tContainer.cHostname LIMIT 301",guOrg,guOrg,guOrg,guOrg,guLoginClient,gcSearch);
		else if(guContainer)
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cHostname FROM tContainer,tNode WHERE"
			" tContainer.uNode=tNode.uNode AND tNode.uStatus=1 AND"
			" tContainer.uSource=0 AND"
			" (tContainer.uOwner IN"
			" (SELECT uClient FROM tClient WHERE (((uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u) OR uOwner=%u) AND "
			" 	cCode='Organization')) OR uOwner=%u)"
			" OR (tContainer.uOwner=%u OR tContainer.uCreatedBy=%u)) AND"
			" tContainer.uStatus!=91 AND"
			" tContainer.uContainer=%u"
			" ORDER BY tContainer.cHostname LIMIT 301",guOrg,guOrg,guOrg,guOrg,guLoginClient,guContainer);
		else
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cHostname FROM tContainer,tNode WHERE"
			" tContainer.uNode=tNode.uNode AND tNode.uStatus=1 AND"
			" tContainer.uSource=0 AND"
			" (tContainer.uOwner IN"
			" (SELECT uClient FROM tClient WHERE (((uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u) OR uOwner=%u) AND "
			" 	cCode='Organization')) OR uOwner=%u)"
			" OR (tContainer.uOwner=%u OR tContainer.uCreatedBy=%u)) AND"
			" tContainer.uStatus!=91"
			" ORDER BY tContainer.cHostname LIMIT 301",guOrg,guOrg,guOrg,guOrg,guLoginClient);
	}
	else
	{
		if(gcSearch[0])
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cHostname FROM tContainer,tNode WHERE"
			" tContainer.uNode=tNode.uNode AND tNode.uStatus=1 AND"
			" tContainer.uSource=0 AND"
			" tContainer.uCreatedBy=%u AND tContainer.cHostname LIKE '%s%%'"
			" AND tContainer.uStatus!=91"
			" ORDER BY tContainer.cHostname LIMIT 301",guLoginClient,gcSearch);
		else if(guContainer)
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cHostname FROM tContainer,tNode WHERE"
			" tContainer.uNode=tNode.uNode AND tNode.uStatus=1 AND"
			" tContainer.uSource=0 AND"
			" tContainer.uCreatedBy=%u AND"
			" tContainer.uStatus!=91 AND"
			" tContainer.uContainer=%u"
			" ORDER BY tContainer.cHostname LIMIT 301",guLoginClient,guContainer);
		else
			sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cHostname FROM tContainer,tNode WHERE"
			" tContainer.uNode=tNode.uNode AND tNode.uStatus=1 AND"
			" tContainer.uSource=0 AND"
			" tContainer.uCreatedBy=%u AND"
			" tContainer.uStatus!=91"
			" ORDER BY tContainer.cHostname LIMIT 301",guLoginClient);
	}

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

	sprintf(gcQuery,"SELECT cLabel FROM tContainer WHERE uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcLabel,"%.32s",field[0]);

		if(!gcMessage[0]) gcMessage="Container Selected";
	}
	else
	{
		gcLabel[0]=0;
		gcMessage="<blink>No Container Selected</blink>";
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

	//Temporary hack using uCreatedBy for access control
	sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE"
				" ("
				" uCreatedBy=%u OR uCreatedBy=%u OR"
				" (uOwner IN (SELECT uClient FROM tClient WHERE ((uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u) OR uOwner=%u) AND "
				" cCode='Organization')) OR uOwner=%u)"
				" ) AND"
				" uContainer=%u",
					guLoginClient,guOrg,guOrg,guOrg,guOrg,guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)<1)
		return;


	//Datacenter
	sprintf(gcQuery,"SELECT tDatacenter.cLabel FROM tContainer,tDatacenter WHERE tContainer.uDatacenter=tDatacenter.uDatacenter"
			" AND tContainer.uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=uDatacenter')\">"
			" <strong>Datacenter</strong></a></td><td><input type=text name='cDatacenter' value='%s' size=80 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
	mysql_free_result(res);

	//Node
	unsigned uAppliance=0;
	sprintf(gcQuery,"SELECT tNode.cLabel FROM tContainer,tNode WHERE tContainer.uNode=tNode.uNode"
			" AND tContainer.uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=uNode')\">"
			" <strong>Node</strong></a></td><td><input type=text name='cNode' value='%s' size=80 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0]);
		if(!strncmp(field[0],"appliance",9))
			uAppliance=1;
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
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=uStatus')\">"
			" <strong>Status</strong></a></td><td><input type=text name='cStatus' value='%s' size=80 maxlength=32"
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
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=uOSTemplate')\">"
			" <strong>OSTemplate</strong></a></td><td><input type=text name='uOSTemplate' value='%s' size=80 maxlength=32"
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
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=uIPv4')\">"
			" <strong>IPv4</strong></a></td><td><input type=text name='cIPv4' value='%s' size=80 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
	mysql_free_result(res);

	//Groups
	sprintf(gcQuery,"SELECT tGroup.cLabel FROM tGroup,tGroupGlue WHERE tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.uGroupType=1 AND tGroupGlue.uContainer=%u",
		guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=Group')\">"
			" <strong>Group</strong></a></td><td><input type=text name='cGroup' value='%s' size=80 maxlength=32"
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
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=uOwner')\">"
			" <strong>Owner</strong></a></td><td><input type=text name='cOwner' value='%s' size=80 maxlength=32"
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
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=uCreatedBy')\">"
			" <strong>Created By</strong></a></td><td><input type=text name='cCreatedBy' value='%s' size=80 maxlength=32"
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
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=uCreatedDate')\">"
			" <strong>Date Created</strong></a></td><td><input type=text name='cCreatedDate' value='%s'" " size=80 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}

	//BackupDate
	sprintf(gcQuery,"SELECT FROM_UNIXTIME(tContainer.uBackupDate,'%%a %%b %%d %%T %%Y'),tContainer.cLabel,tDatacenter.uDatacenter"
			" FROM tContainer,tDatacenter"
			" WHERE uContainer IN (SELECT uContainer FROM tContainer WHERE uSource=%u)"
			" AND tContainer.uDatacenter=tDatacenter.uDatacenter",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
		while((field=mysql_fetch_row(res)))
		{
			printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=uBackupDate')\">"
			" <strong>Backup Date</strong></a></td><td><input type=text name='cBackupDate' value='%s (%s/%s)'"
			" size=80 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n",field[0],field[1],field[2]);
		}
	}
	else
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=uBackupDate')\">"
			" <strong>Backup Date</strong></a></td><td><input type=text name='cBackupDate' value='!No backup available!'"
			" size=80 maxlength=32"
			" disabled class=\"type_fields_off\"> </td></tr>\n");
	}

/*
	//ModBy
	sprintf(gcQuery,"SELECT tClient.cLabel FROM tContainer,tClient WHERE tContainer.uModBy=tClient.uClient AND"
			" tContainer.uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=uModBy')\">"
			" <strong>Modified By</strong></a></td><td><input type=text name='cModBy'");
	if((field=mysql_fetch_row(res)))
		printf(" value='%s'",field[0]);
	else
		printf(" value='---'");
	printf(" disabled size=80 maxlength=32 class=\"type_fields_off\"> </td></tr>\n");
	mysql_free_result(res);
*/

	//cPasswd
	if((!uAppliance || guPermLevel>=10) &&  (guPermLevel>=6 || guPermLevel==2) )
	{
		sprintf(gcQuery,"SELECT cName,cValue FROM tProperty WHERE uType=3 AND uKey=%u AND cName='cPasswd'",guContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=%s')\">"
			" <strong>SSH Passwd</strong></a></td><td><input type=text name='%s' value='%s' size=80 maxlength=32"
			" class=\"type_fields_off\"> </td></tr>\n",field[0],field[0],field[1]);
		}
		mysql_free_result(res);
	}

	//SUBSTR based on 5 char cOrg_ prefix
	sprintf(gcQuery,"SELECT SUBSTR(cName,6),cValue FROM tProperty WHERE uType=3 AND uKey=%u AND cName LIKE 'cOrg_%%'"
			" AND cName!='cOrg_Extension' AND cName!='cOrg_OpenSIPS_DID' AND cName!='cOrg_SIPTrunk'"
			" AND cName NOT LIKE 'cOrg_MCS%%' AND cName NOT LIKE 'cOrg_QOS%%' ORDER BY cName",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
			 if( 
				(strncmp(field[0],"FreePBXEngPasswd",16) || guPermLevel>=10 || guPermLevel==2) &&
				(strncmp(field[0],"FreePBXOperatorPasswd",21) || guPermLevel>=1) &&
				(strncmp(field[0],"MySQLAsteriskPasswd",19) || guPermLevel>=6 || guPermLevel==2) &&
				(strncmp(field[0],"SSHPort",7) || guPermLevel>=6 || guPermLevel==2) &&
				(strncmp(field[0],"FreePBXAdminPasswd",18) || guPermLevel>=5 || guPermLevel==2)
			)
			printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=%s')\">"
			" <strong>%s</strong></a></td><td><input type=text name='%s' value='%s' size=80 maxlength=32"
			" class=\"type_fields_off\"> </td></tr>\n",field[0],field[0],field[0],field[1]);
	}
	mysql_free_result(res);

	//Show very little to unprivilged users
	if(guPermLevel<6)
	{
		fprintf(fp,"<!-- funcContainerInfo(fp) low permissions end-->\n");
		return;
	}

	if(gcShowDetails[0])
	{
		sprintf(gcQuery,"SELECT SUBSTR(cName,6),cValue FROM tProperty WHERE uType=3 AND uKey=%u"
			" AND (cName='cOrg_Extension' OR cName='cOrg_OpenSIPS_DID' OR cName='cOrg_SIPTrunk') ORDER BY cName",guContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=%s')\">"
			" <strong>%s</strong></a></td><td><input type=text name='%s' value='%s' size=80 maxlength=32"
			" class=\"type_fields_off\"> </td></tr>\n",field[0],field[0],field[0],field[1]);
		}
		mysql_free_result(res);
	}

	if(uAppliance)
	printf("<tr></tr><tr><td><a href=OneLogin.cgi?gcPage=Container&gcFunction=QOSReport&guContainer=%u title='QOS Report'>"
			"<img src=/images/plus.png border=0 ></a></td></tr>\n",
			guContainer);

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
		" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=Special+OPs')\""
		" <strong>Special OPs</strong></a></td><td>\n");

	printf("<fieldset><legend>Search</b></legend>");
	//Search DID
	fprintf(fp,"<input type=text class=type_fields"
			" title='Search for a container by DID or customer name. %% wildcard can be used.'"
			" name=gcSearchAux value='%.31s' size=16 maxlength=31>",gcSearchAux);
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Enter DID above, then use this function to search for a container'"
			" name=gcFunction value='Search DID'>\n");
	//Search customer name
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Enter customer above, then use this function to search for a container'"
			" name=gcFunction value='Search Customer'>\n");
	printf("</fieldset>");

	//Customer accounts
	printf("<fieldset><legend>Customer accounts</b></legend>");
	//Add end user account to this container
	fprintf(fp,"<p><input type=text class=type_fields"
			" title='New account login'"
			" name=gcNewLogin value='%.31s' size=16 maxlength=31>",gcNewLogin);
	if(!gcNewPasswd[0])
		random_pw(gcNewPasswd);
	fprintf(fp," <input type=text class=type_fields"
			" title='New account password'"
			" name=gcNewPasswd value='%.31s' size=16 maxlength=31>",gcNewPasswd);
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Enter customer name and password above, then use this function to create a customer account'"
			" name=gcFunction value='Create Account'>\n");

	//reseller select
	unsigned uAuthorize=0;
	sprintf(gcQuery,"SELECT tClient.uClient,tAuthorize.cLabel FROM tAuthorize,tClient"
				" WHERE tAuthorize.uPerm=1 AND tClient.uClient=tAuthorize.uCertClient ORDER BY tAuthorize.cLabel LIMIT 501");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlPlainTextError(mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	fprintf(fp,"<p><select class=type_textarea title='Select the reseller you want to use.'"
			" name=guReseller>\n");
	fprintf(fp,"<option>---</option>");
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uAuthorize);
		fprintf(fp,"<option value=%s",field[0]);
		if(guReseller == uAuthorize)
			fprintf(fp," selected");
		if((uCount++)<=500)
			fprintf(fp,">%s</option>",field[1]);
		else
			fprintf(fp,">Limit reached. Contact your sysadmin ASAP!</option>");
	}
	mysql_free_result(res);
	fprintf(fp,"</select> Select customer\n");

	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Select reseller from above and add currently loaded container to their account'"
			" name=gcFunction value='Add Container'>\n");
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Select reseller from above and remove currently loaded container from their account'"
			" name=gcFunction value='Del Container'>\n");
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Select reseller from above and provide list of their containers'"
			" name=gcFunction value='Show Containers'>\n");
	if(guPermLevel>=10)
		fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Select reseller from above and think twice, no undo!'"
			" name=gcFunction value='Remove Account'>\n");
	printf("</fieldset>");//Customer accounts


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
	printf("<fieldset><legend>Repurpose</b></legend>");
	fprintf(fp,"<select class=type_textarea title='Select the container you want to use.'"
			" name=guNewContainer >\n");
	fprintf(fp,"<option>---</option>");
	uCount=0;
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContainer);
		fprintf(fp,"<option value=%s",field[0]);
		if(guNewContainer==uContainer)
			fprintf(fp," selected");
		if((uCount++)<=300)
		{
			fprintf(fp,">%s</option>",field[1]);
		}
		else
		{
			fprintf(fp,">Limit reached. Contact your sysadmin ASAP!</option>");
			break;
		}
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
	printf("</fieldset>");

	fprintf(fp,"<p><input type=checkbox name=gcShowDetails %s> DID/Extension/Trunk Details",gcShowDetails);
	fprintf(fp,"<br><input type=submit class=largeButton"
			" title='Reload current container info with options as set above'"
			" name=gcFunction value='Reload'>\n");
	if(guPermLevel>=10)
	{
		fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Generate a cvs report of all PBX containers direct to browser'"
			" name=gcFunction value='Container Report'>\n");
		fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Generate a cvs report of all DID/Trunk PBX container data direct to browser'"
			" name=gcFunction value='DID Report'>\n");
	}
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Quality of service report. Do not select a container for \"all container\" report only.'"
			" name=gcFunction value='QOS Report'>\n");

	fprintf(fp,"</td></tr>\n");

	fprintf(fp,"<!-- funcNewContainer(fp) End -->\n");

}//void funcNewContainer(FILE *fp)


char *CustomerName(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
		if(!isalnum(cInput[i]) && cInput[i]!=' ' 
			&& cInput[i]!='.' && cInput[i]!=',' && cInput[i]!='-') break;
	cInput[i]=0;

	return(cInput);

}//char *CustomerName(char *cInput)


char *NameToLower(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
	{
	
		if(!isalnum(cInput[i]) && cInput[i]!='-' ) break;
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
		" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=Container+OPs')\""
		" <strong>Container OPs</strong></a></td><td>\n");

	//DID
	printf("<fieldset><legend>DID OPs</b></legend>");
	fprintf(fp,"<input type=text class=type_fields"
			" title='Enter a valid DID number'"
			" name=gcDID size=16 maxlength=16> DID");
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Add a DID to currently loaded PBX container that has OpenSIPS_Attrs or LinesContracted values'"
			" name=gcFunction value='Add DID'>\n");
	fprintf(fp,"<input type=submit class=largeButton"
			" title='Open special page for bulk operations on DIDs and similar'"
			" name=gcFunction value='Bulk Operations'>\n");
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Remove a DID from currently loaded PBX container'"
			" name=gcFunction value='Remove DID'>\n");
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Remove container from SIP proxy or SIP proxy cluster'"
			" name=gcFunction value='Remove from Proxy'>\n");
	printf("</fieldset>");

	printf("<fieldset><legend>CustomerName OPs</b></legend>");
	fprintf(fp,"<input type=text class=type_fields"
			" title='Enter a valid customer name'"
			" name=gcCustomerName value='%s' size=16 maxlength=32> Customer name",gcCustomerName);
	fprintf(fp,"<br><input type=text class=type_fields"
			" title='Enter new number of lines limit'"
			" name=gcCustomerLimit value='%s' size=16 maxlength=32> Line limit",gcCustomerLimit);
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Modify currently loaded PBX container customer name and/or number of lines limit'"
			" name=gcFunction value='Mod CustomerName'>\n");
	printf("</fieldset>");

	printf("<fieldset><legend>Lifecycle OPs</b></legend>");
	fprintf(fp,"<input type=password class=type_fields"
			" title='Special authorization code required to cancel a container'"
			" name=gcAuthCode size=16 maxlength=32> Authorization Code");
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Immediately destroys all container data and functionality'"
			" name=gcFunction value='Cancel Container'>\n");
	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Schedules a job for container upgrade to latest version'"
			" name=gcFunction value='Upgrade Container'>\n");
	printf("</fieldset>");


	fprintf(fp,"</td></tr>\n");

	fprintf(fp,"<!-- funcContainer(fp) End -->\n");

}//void funcContainer(FILE *fp)


void funcContainerList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCount=0;

	fprintf(fp,"<!-- funcContainerList(fp) Start -->\n");

	if(!gcSearchAux[0] || !gcSearchName[0]) return;

	sprintf(gcQuery,"SELECT uKey,tContainer.cHostname FROM tProperty,tContainer WHERE uKey=tContainer.uContainer AND"
				" (uStatus=1 OR uStatus=3 OR uStatus=101) AND"
				" cValue LIKE '%.31s%%' AND"
				" cName='%.32s' AND uType=3 GROUP BY tContainer.cHostname LIMIT 33",gcSearchAux,gcSearchName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(++uCount>32)
		{
			printf("<tr><td>Search returned more than 32 records. Refine search pattern.</td></tr>\n");
			break;
		}
		else
		{
			printf("<tr><td><a class=inputLink href=OneLogin.cgi?gcPage=Container&guContainer=%s>%s</a></td></tr>\n",
					field[0],field[1]);
		}
	}
	mysql_free_result(res);

	fprintf(fp,"<!-- funcContainerList(fp) End -->\n");

}//void funcContainerList(FILE *fp)


void funcContainerQOS(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcContainerQOS(fp) Start --><tr> </tr>\n");

	if(!guContainer)
	{
		sprintf(gcQuery,"SELECT cHostname,cName,cValue FROM tProperty,tContainer"
				" WHERE tContainer.uContainer=tProperty.uKey"
				" AND cName LIKE 'cOrg_QOS%%'"
				" AND uType=3"
				" ORDER BY uProperty"
				" DESC LIMIT 25");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		printf("<p><b>Latest QOS issues for all containers. Select a container for specific container QOS report.</b>\n");
		while((field=mysql_fetch_row(res)))
		{
			printf("<p>%s %s %s\n",field[0],field[1],field[2]);
		}
		mysql_free_result(res);

		fprintf(fp,"<!-- funcContainerQOS(fp) no container End -->\n");
		return;
	}
	else
	{
		sprintf(gcQuery,"SELECT cName,cValue FROM tProperty WHERE uKey=%u AND"
				" cName LIKE 'cOrg_QOS%%' AND uType=3 ORDER BY uProperty DESC",guContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)>0)
		{
			printf("<font size=1><p><b>Specific container QOS issues. Unselect container for latest QOS issues report.</b>\n");
			while((field=mysql_fetch_row(res)))
			{
				printf("<p>%s %s\n",field[0],field[1]);
			}
			mysql_free_result(res);
			fprintf(fp,"<!-- funcContainerQOS(fp) End new QOS only-->\n");
			return;
		}
	
		fprintf(fp,"<tr></tr>\n");
		sprintf(gcQuery,"SELECT SUBSTR(cName,6),cValue,FROM_UNIXTIME(uModDate) FROM tProperty WHERE uKey=%u AND"
					" cName LIKE 'cOrg_MCS_%%MOS' AND uType=3 ORDER BY cName DESC",guContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
				printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=%s')\">"
				" <strong>%s</strong></a></td>"
				"<td><input type=text name='%s' value='%s' size=80 maxlength=32 disabled class=\"type_fields_off\"> </td>\n"
				"<td><input type=text name='%s' value='%s' size=80 maxlength=32 disabled class=\"type_fields_off\"> </td></tr>\n",
							field[0],field[0],
							field[0],field[1],
							field[0],field[2]);
		}
		fprintf(fp,"<tr></tr>\n");
		sprintf(gcQuery,"SELECT SUBSTR(cName,6),cValue,FROM_UNIXTIME(uModDate) FROM tProperty WHERE uKey=%u AND"
					" cName LIKE 'cOrg_MCS_%%Jitter%%' AND uType=3 ORDER BY cName DESC",guContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
				printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=%s')\">"
				" <strong>%s</strong></a></td>"
				"<td><input type=text name='%s' value='%s' size=80 maxlength=32 disabled class=\"type_fields_off\"> </td>\n"
				"<td><input type=text name='%s' value='%s' size=80 maxlength=32 disabled class=\"type_fields_off\"> </td></tr>\n",
							field[0],field[0],
							field[0],field[1],
							field[0],field[2]);
		}
		fprintf(fp,"<tr></tr>\n");
		sprintf(gcQuery,"SELECT SUBSTR(cName,6),cValue,FROM_UNIXTIME(uModDate) FROM tProperty WHERE uKey=%u AND"
					" cName LIKE 'cOrg_MCS_%%Loss%%' AND uType=3 ORDER BY cName DESC",guContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
				printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('OneLogin.cgi?gcPage=Glossary&cLabel=%s')\">"
				" <strong>%s</strong></a></td>"
				"<td><input type=text name='%s' value='%s' size=80 maxlength=32 disabled class=\"type_fields_off\"> </td>\n"
				"<td><input type=text name='%s' value='%s' size=80 maxlength=32 disabled class=\"type_fields_off\"> </td></tr>\n",
							field[0],field[0],
							field[0],field[1],
							field[0],field[2]);
		}
		mysql_free_result(res);
	}

	fprintf(fp,"<!-- funcContainerQOS(fp) End -->\n");

}//void funcContainerQOS(FILE *fp)


void SendAlertEmail(char *cMsg)
{
	FILE *pp;
	pid_t pidChild;

	pidChild=fork();
	if(pidChild!=0)
		return;

	pp=popen("/usr/lib/sendmail -t","w");
	if(pp==NULL)
	{
		logfileLine("SendAlertEmail","popen() /usr/lib/sendmail");
		return;
	}
			
	//should be defined in local.h
	fprintf(pp,"To: %s\n",cMAILTO);
	if(cBCC!=NULL)
	{
		char cBcc[512]={""};
		sprintf(cBcc,"%.511s",cBCC);
		if(cBcc[0])
			fprintf(pp,"Bcc: %s\n",cBcc);
	}
	fprintf(pp,"From: %s\n",cFROM);
	fprintf(pp,"Subject: %s\n",cSUBJECT);

	fprintf(pp,"%s\n",cMsg);

	fprintf(pp,".\n");

	pclose(pp);

	logfileLine("SendAlertEmail","email attempt ok");

}//void SendAlertEmail(char *cMsg)


//functions that operate on loaded container
void funcContainerBulk(FILE *fp)
{
	if(guPermLevel<6 || guContainer==0)
		return;

	fprintf(fp,"<!-- funcContainerBulk(fp) Start -->\n");

	//DID
	printf("<fieldset><legend>DID Bulk OPs</b></legend>");

	fprintf(fp,"<textarea rows=10 cols=31"
			" title='Enter a valid bulk data as per operation specs'"
			" name=gcBulkData >%s</textarea>",gcBulkData);
	if(guMode==0)
	{
		fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Add DIDs to current PBX, one per line."
			" Lines that start with anything other than a number are ignored.'"
			" name=gcFunction value='Bulk Add DIDs'>\n");
		fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Remove DIDs specified one per line from currently loaded PBX container."
			" Lines that start with anything other than a number are ignored.'"
			" name=gcFunction value='Bulk Remove DIDs'>\n");
		fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Load all active and pending backend registered DIDs into bulk data panel.'"
			" name=gcFunction value='Load All DIDs'>\n");
	}
	else if(guMode==1)
	{
		fprintf(fp,"<p><input type=submit class=lwarnButton"
			" title='Remove DIDs specified one per line from currently loaded PBX container."
			" Lines that start with anything other than a number are ignored.'"
			" name=gcFunction value='Confirm Bulk Remove DIDs'>\n");
	}
	printf("</fieldset>");

	fprintf(fp,"<!-- funcContainerBulk(fp) End -->\n");

}//void funcContainerBulk(FILE *fp)


//Does not allow empty lines...this may need reviewing ;) to say the least.
char *ParseTextAreaLines(char *cTextArea)
{
	static unsigned uEnd=0;
	static unsigned uStart=0;
	static char cRetVal[512];

	uStart=uEnd;
	while(cTextArea[uEnd++])
	{
		if(cTextArea[uEnd]=='\n' || cTextArea[uEnd]=='\r' || cTextArea[uEnd]==0
				|| cTextArea[uEnd]==10 || cTextArea[uEnd]==13 )
		{
			char cSave[1];

			/*
			if(cTextArea[uEnd+1]=='\n' || cTextArea[uEnd+1]=='\r' || cTextArea[uEnd+1]==0
				|| cTextArea[uEnd+1]==10 || cTextArea[uEnd+1]==13 )
			{
				uEnd++;
				memcpy(cSave,cTextArea+uEnd,1);
				cTextArea[uEnd]=0;
				sprintf(cRetVal,"%.511s",cTextArea+uStart);
				memcpy(cTextArea+uEnd,cSave,1);
				return(cRetVal);
			}
			*/

			if(cTextArea[uEnd]==0)
				break;

			memcpy(cSave,cTextArea+uEnd,1);
			cTextArea[uEnd]=0;
			sprintf(cRetVal,"%.511s",cTextArea+uStart);
			memcpy(cTextArea+uEnd,cSave,1);

			if(cTextArea[uEnd+1]==10)
				uEnd+=2;
			else
				uEnd++;

			//empty line call again recursively
			//or comment line anything that starts without a number
			if(cRetVal[0]=='\n' || cRetVal[0]==13 || !isdigit(cRetVal[0]) )
			{
				ParseTextAreaLines(cTextArea);
			}

			return(cRetVal);
		}
	}

	if(uStart!=uEnd)
	{
		sprintf(cRetVal,"%.511s",cTextArea+uStart);
		return(cRetVal);
	}

	uStart=uEnd=0;
	return("");

}//char *ParseTextAreaLines(char *cTextArea)


void BulkDIDAdd(void)
{
        MYSQL_RES *res;
	unsigned uCount=0;
	unsigned uLen=0;
	unsigned uProcessedCount=0;
	char cMsg[128];

	cReply[0]=0;

	while(uCount<uMAX_DIDs_ALLOWED)
	{

		sprintf(gcDID,"%.16s",cNumbersOnly(ParseTextAreaLines(gcBulkData)));
		if(gcDID[0]==0) break;
		uCount++;

		strncat(cReply,gcDID,16);

		if((uLen=strlen(gcDID))<10)
		{
			strncat(cReply," err len 10-11\n",15);
			continue;
		}
		if(uLen>11)
		{
			strncat(cReply," err maxlen 11\n",15);
			continue;
		}
		if(uLen==11 && gcDID[0]!='1')
		{
			strncat(cReply," err start w/1\n",15);
			continue;
		}
		if(uLen==10 && gcDID[0]=='1')
		{
			strncat(cReply," err starts w/1\n",16);
			continue;
		}

		unsigned uInsert=1;//normally insert
		sprintf(gcQuery,"SELECT uProperty FROM tProperty"
					" WHERE uKey=%u AND uType=3 AND (cName='cOrg_OpenSIPS_DID' OR cName='cOrg_Pending_DID')"
					" AND cValue='%s'",guContainer,gcDID);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			strncat(cReply," select error 1\n",15);
			htmlContainerBulk();
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)>0)
		{
			uInsert=0;//do not insert
			//force adding again. why not!
			//strncat(cReply," already added\n",15);
			//mysql_free_result(res);
			//continue;
		}
		mysql_free_result(res);

		//business logic rule: no two containers can deploy same DID
		sprintf(gcQuery,"SELECT uProperty,uKey FROM tProperty"
					" WHERE uKey!=%u AND uType=3 AND (cName='cOrg_OpenSIPS_DID' OR cName='cOrg_Pending_DID')"
					" AND cValue='%s'",guContainer,gcDID);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			strncat(cReply," select error 1\n",15);
			htmlContainerBulk();
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)>0)
		{
	        	MYSQL_ROW field;
			uInsert=0;//do not insert
			strncat(cReply," dup uContainer=",16);
			if((field=mysql_fetch_row(res)))
				strncat(cReply,field[1],10);
			mysql_free_result(res);
			continue;
		}
		mysql_free_result(res);

		if(!guNode || !guDatacenter)
		{
        		MYSQL_RES *res;
	        	MYSQL_ROW field;

			sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tContainer WHERE uContainer=%u",guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				strncat(cReply," select error 2\n",15);
				htmlContainerBulk();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&guNode);
				sscanf(field[1],"%u",&guDatacenter);
			}
			mysql_free_result(res);
		}

		char cSIPProxyList[256]={""};
		GetSIPProxyList(cSIPProxyList,guDatacenter,guNode,guContainer);
		if(!cSIPProxyList[0])
		{
			strncat(cReply," No cSIPProxyList!\n",18);
			continue;
		}

		if(uInsert)
		{
			sprintf(gcQuery,"INSERT INTO tProperty"
					" SET uKey=%u,uType=3,cName='cOrg_Pending_DID',cValue='%s'"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						guContainer,gcDID,guOrg,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				strncat(cReply," insert error 1\n",15);
				continue;
			}
		}

		//unxsSIPS jobs mass update Add DID
		register int i,j=0;
		unsigned uDelayInSecs=0;

		for(i=0;cSIPProxyList[i] && cSIPProxyList[i]!='#' && cSIPProxyList[i]!='\r';i++)
		{
			if(cSIPProxyList[i]!=';')
				continue;
			cSIPProxyList[i]=0;
			sprintf(gcServer,"%.31s",cSIPProxyList+j);
			j=i+1;//next beg if app
			cSIPProxyList[i]=';';
			sprintf(gcCtHostname,"%.99s",(char *)cGetHostname(guContainer));
			uDelayInSecs+=60;
			sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsSIPSNewDID(%u)',cJobName='unxsSIPSNewDID'"
					",uDatacenter=%u,uNode=%u,uContainer=%u"
					",uJobDate=UNIX_TIMESTAMP(NOW())+%u"
					",uJobStatus=%u"
					",cJobData='"
					"cServer=%s;\n"
					"cDID=%s;\n"
					"cHostname=%s;\n'"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						guContainer,
						guDatacenter,
						guNode,
						guContainer,
						uDelayInSecs,
						uREMOTEWAITING,
						gcServer,
						gcDID,
						gcCtHostname,
						guOrg,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				strncat(cReply," ie\n",4);
				continue;
			}
		}
		strncat(cReply," added\n",15);
		uProcessedCount++;
	}
	sprintf(cMsg,"%u DIDs found %u added.\n",uCount,uProcessedCount);
	gcMessage=cMsg;
	gcBulkData=cReply;
	htmlContainerBulk();

}//void BulkDIDAdd(void)


void BulkDIDRemove(void)
{

        MYSQL_RES *res;
	unsigned uCount=0;
	unsigned uLen=0;
	unsigned uProcessedCount=0;
	char cMsg[128];

	cReply[0]=0;

	while(uCount<uMAX_DIDs_ALLOWED)
	{

		sprintf(gcDID,"%.16s",cNumbersOnly(ParseTextAreaLines(gcBulkData)));
		if(gcDID[0]==0) break;
		uCount++;

		strncat(cReply,gcDID,16);

		if((uLen=strlen(gcDID))<10)
		{
			strncat(cReply," err len 10-11\n",15);
			continue;
		}
		if(uLen>11)
		{
			strncat(cReply," err maxlen 11\n",15);
			continue;
		}
		if(uLen==11 && gcDID[0]!='1')
		{
			strncat(cReply," err start w/1\n",15);
			continue;
		}
		if(uLen==10 && gcDID[0]=='1')
		{
			strncat(cReply," err starts w/1\n",16);
			continue;
		}

		sprintf(gcQuery,"SELECT uProperty FROM tProperty"
					" WHERE uKey=%u AND uType=3 AND (cName='cOrg_OpenSIPS_DID' OR cName='cOrg_Pending_DID')"
					" AND cValue='%s'",guContainer,gcDID);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			strncat(cReply," select error 1\n",15);
			continue;
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)<1)
		{
			mysql_free_result(res);
			strncat(cReply," not found\n",15);
			continue;
		}
		mysql_free_result(res);

		//Update for temporary status
		sprintf(gcQuery,"UPDATE tProperty SET cName='cOrg_Remove_DID',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
					" WHERE uKey=%u AND uType=3 AND cValue='%s' AND cName='cOrg_OpenSIPS_DID'",
						guLoginClient,guContainer,gcDID);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			strncat(cReply," update error 1\n",15);
			continue;
		}

		//Remove pending. We let any pending jobs continue? Yeah why not. The new job created here will wipe SIP server
		sprintf(gcQuery,"DELETE FROM tProperty WHERE uKey=%u AND uType=3 AND cValue='%s' AND cName='cOrg_Pending_DID'",
						guContainer,gcDID);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			strncat(cReply," delete error 1\n",15);
			continue;
		}

		if(!guNode || !guDatacenter)
		{
        		MYSQL_RES *res;
	        	MYSQL_ROW field;

			sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tContainer WHERE uContainer=%u",guContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				strncat(cReply," select error 2\n",15);
				htmlContainerBulk();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&guNode);
				sscanf(field[1],"%u",&guDatacenter);
			}
			mysql_free_result(res);
		}

		//unxsSIPS jobs mass update Remove DIDs
		register int i,j=0;
		unsigned uDelayInSecs=0;
		char cSIPProxyList[256]={""};
		GetSIPProxyList(cSIPProxyList,guDatacenter,guNode,guContainer);
		if(!cSIPProxyList[0])
		{
			strncat(cReply," No cSIPProxyList!\n",18);
			continue;
		}

		for(i=0;cSIPProxyList[i] && cSIPProxyList[i]!='#' && cSIPProxyList[i]!='\r';i++)
		{
			if(cSIPProxyList[i]!=';')
				continue;
			cSIPProxyList[i]=0;
			sprintf(gcServer,"%.31s",cSIPProxyList+j);
			j=i+1;//next beg if app
			cSIPProxyList[i]=';';
			//unxsSIPS job
			sprintf(gcCtHostname,"%.99s",(char *)cGetHostname(guContainer));
			uDelayInSecs+=60;
			sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsSIPSRemoveDID(%u)',cJobName='unxsSIPSRemoveDID'"
					",uDatacenter=%u,uNode=%u,uContainer=%u"
					",uJobDate=UNIX_TIMESTAMP(NOW())+%u"
					",uJobStatus=%u"
					",cJobData='"
					"cServer=%s;\n"
					"cDID=%s;\n"
					"cHostname=%s;\n'"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						guContainer,
						guDatacenter,
						guNode,
						guContainer,
						uDelayInSecs,
						uREMOTEWAITING,
						gcServer,
						gcDID,
						gcCtHostname,
						guOrg,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				strncat(cReply," ie\n",4);
		}

		strncat(cReply," removed\n",15);
		uProcessedCount++;
	}
	sprintf(cMsg,"%u DIDs found %u removed.\n",uCount,uProcessedCount);
	gcMessage=cMsg;
	gcBulkData=cReply;
	htmlContainerBulk();

}//void BulkDIDRemove(void)


void DIDOpsCommonChecking(void)
{
	char gcQuery[1024];
        MYSQL_RES *res;
	MYSQL_ROW field;

	if(!guContainer)
	{
		gcMessage="Must select a container.";
		htmlContainerBulk();
	}

	//uStatus must be active or offline or appliance
	sprintf(gcQuery,"SELECT uContainer,uNode,uDatacenter FROM tContainer WHERE uContainer=%u"
					" AND (uStatus=1 OR uStatus=3 OR uStatus=101)",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Select uNode error, contact sysadmin!";
		htmlContainerBulk();
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&guNode);
		sscanf(field[2],"%u",&guDatacenter);
	}
	if(mysql_num_rows(res)<1)
	{
		gcMessage="Selected container status is incorrect.";
		htmlContainerBulk();
	}
	if(!guNode || !guDatacenter)
	{
		gcMessage="No uNode or no uDatacenter error. Contact your sysadmin!";
		htmlContainerBulk();
	}

	//Must be already registered with OpenSIPS or have LinesContracted value
	sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uType=3 AND uKey=%u AND"
			" (cName='cOrg_OpenSIPS_Attrs' OR cName='cOrg_LinesContracted')",
								guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Check for cOrg_OpenSIPS_Attrs failed, contact sysadmin!";
		htmlContainerBulk();
	}
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)<1)
			{
		mysql_free_result(res);
		gcMessage="Container must be registered with OpenSIPS or have LinesContracted value.";
		htmlContainerBulk();
	}
	mysql_free_result(res);

}//void DIDOpsCommonChecking(void);


void LoadAllDIDs(void)
{
	char gcQuery[1024];
        MYSQL_RES *res;
	MYSQL_ROW field;

	if(!guContainer)
	{
		gcMessage="Must select a container.";
		htmlContainerBulk();
	}

	cReply[0]=0;

	sprintf(gcQuery,"SELECT cValue FROM tProperty"
			" WHERE uKey=%u AND uType=3 AND (cName='cOrg_OpenSIPS_DID' OR cName='cOrg_Pending_DID')",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cReply,"%.255s\n",mysql_error(&gMysql));
		goto Exit;
	}
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)<1)
	{
		mysql_free_result(res);
		strcat(cReply,"No DIDs found\n");
		goto Exit;
	}
	while((field=mysql_fetch_row(res)))
	{
		if(field[0])
		{
			strcat(cReply,field[0]);
			strcat(cReply,"\n");
		}
	}
	mysql_free_result(res);


Exit:
	gcBulkData=cReply;
	htmlContainerBulk();

}//void LoadAllDIDs(void);


//(C) issue: public domain code from unix.com

static int srand_called=0;

char *random_pw(char *dest)
{
    size_t len=0;
    char *p=dest;
    int three_in_a_row=0;
    int arr[128]={0x0};

	/* be sure to have called srand exactly one time */
	if(!srand_called)
	{
		srandom(time(NULL));
		srand_called=1;
	}
	*dest=0x0; /* int the destination string*/
	for(len=6 + rand()%3; len; len--, p++) /* gen characters */
	{
		char *q=dest;
		*p=(rand()%2)? rand()%26 + 97: rand()%10 + 48;
		p[1]=0x0;
		arr[(int) *p]++;                         /* check values */
		if(arr[(int) *p]==3)
		{
			for(q=dest; q[2]>0 && !three_in_a_row; q++)	
				if(*q==q[1] && q[1]==q[2])
			   		three_in_a_row=1;
		}
		if(three_in_a_row || arr[(int) *p]> 3 )
			return random_pw(dest);        /* values do not pass try again */
	}
    return dest;
}//char *random_pw(char *dest)


void GetConfigurationValue(char const *cName,char *cValue,unsigned uDatacenter,unsigned uNode,unsigned uContainer)
{
        MYSQL_RES *res;
	MYSQL_ROW field;

	cValue[0]=0;
	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uDatacenter=%u"
			" AND uNode=%u AND uContainer=%u AND cLabel='%s'",uDatacenter,uNode,uContainer,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return;
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cValue,"%.255s",field[0]);

}//void GetConfigurationValue()


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


char *ToLower(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
		if(isupper(cInput[i])) cInput[i]=tolower(cInput[i]);
	cInput[i]=0;

	return(cInput);

}//char *ToLower(char *cInput)


unsigned GetConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer,
		unsigned uHtml)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        char cQuery[1024];
	char cExtra[100]={""};
	unsigned uConfiguration=0;

        sprintf(cQuery,"SELECT cValue,uConfiguration FROM tConfiguration WHERE cLabel='%s'",
			cName);
	if(uDatacenter)
	{
		sprintf(cExtra," AND uDatacenter=%u",uDatacenter);
		strcat(cQuery,cExtra);
	}
	if(uNode)
	{
		sprintf(cExtra," AND uNode=%u",uNode);
		strcat(cQuery,cExtra);
	}
	if(uContainer)
	{
		sprintf(cExtra," AND uContainer=%u",uContainer);
		strcat(cQuery,cExtra);
	}
        mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
        	sprintf(cValue,"%.255s",field[0]);
        	sscanf(field[1],"%u",&uConfiguration);
	}
        mysql_free_result(res);

	return(uConfiguration);

}//unsigned GetConfiguration(...)


unsigned uUpdateNamesFromCloneToBackup(unsigned uContainer)
{
	if(!uContainer)
		return(0);

	unsigned uSource=0;
	unsigned uSourceDatacenter=0;
	char cSourceLabel[32]={""};
	char cSourceHostname[64]={""};
	char *cp;
	char cSourceDomain[64]={""};

	sscanf(ForeignKey("tContainer","uSource",uContainer),"%u",&uSource);
	if(!uSource)
		return(0);

	sscanf(ForeignKey("tContainer","uDatacenter",uSource),"%u",&uSourceDatacenter);
	if(!uSourceDatacenter)
		return(0);

	sprintf(cSourceLabel,"%.31s",ForeignKey("tContainer","cLabel",uSource));
	sprintf(cSourceHostname,"%.63s",ForeignKey("tContainer","cHostname",uSource));
	if(!cSourceHostname[0] || !cSourceLabel[0])
		return(0);

	//clean up (chop off) -m at end for dns move containers that require review.
	if((cp=strstr(cSourceLabel+strlen(cSourceLabel)-2,"-m")))
		*cp=0;

	//first dot
	if((cp=strchr(cSourceHostname,'.')))
		sprintf(cSourceDomain,"%.63s",cp+1);
	else
		return(0);

	//Note how we may clobber the front of the names but leave the -backup suffix intact
	sprintf(gcQuery,"UPDATE tContainer"
			" SET cLabel='%.24s-backup',"
			" cHostname='%.24s-backup.%s'"
			" WHERE uContainer=%u"
			" AND uSource!=0"
			" AND uDatacenter!=%u",
				cSourceLabel,
				cSourceLabel,cSourceDomain,
				uContainer,
				uSourceDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return(0);
		//unxsVZLog(uContainer,"tContainer","uUpdateNamesFromCloneToBackup() mysql_error");

	return(mysql_affected_rows(&gMysql));

}//unsigned uUpdateNamesFromCloneToBackup(uContainer)


unsigned uUpdateNamesFromCloneToClone(unsigned uContainer)
{
	if(!uContainer)
		return(0);

	unsigned uSource=0;
	unsigned uCloneNum=0;
	char cuCloneNum[16]={""};
	unsigned uSourceDatacenter=0;
	char cSourceLabel[32]={""};
	char cLabel[32]={""};
	char cSourceHostname[64]={""};
	char *cp;
	char cSourceDomain[64]={""};

	//guFlag=1;

	sscanf(ForeignKey("tContainer","uSource",uContainer),"%u",&uSource);
	if(!uSource)
		return(0);

	sscanf(ForeignKey("tContainer","uDatacenter",uSource),"%u",&uSourceDatacenter);
	if(!uSourceDatacenter)
		return(0);

	//guFlag=2;

	sprintf(cSourceLabel,"%.31s",ForeignKey("tContainer","cLabel",uSource));
	sprintf(cLabel,"%.31s",ForeignKey("tContainer","cLabel",uContainer));
	sprintf(cSourceHostname,"%.63s",ForeignKey("tContainer","cHostname",uSource));
	if(!cSourceHostname[0] || !cSourceLabel[0])
		return(0);

	//guFlag=3;

	//clean up (chop off) -m at end for dns move containers that require review.
	if((cp=strstr(cSourceLabel+strlen(cSourceLabel)-2,"-m")))
		*cp=0;

	//first dot
	if((cp=strchr(cSourceHostname,'.')))
		sprintf(cSourceDomain,"%.63s",cp+1);
	else
		return(0);

	//guFlag=4;

	//use clone number if any
	if((cp=strstr(cLabel,"-clone")))
	{
		sscanf(cp,"-clone%u",&uCloneNum);
		if(uCloneNum)
			sprintf(cuCloneNum,"%u",uCloneNum);
	}

	//guFlag=5;

	//Note how we may clobber the front of the names but leave the -backup suffix intact
	sprintf(gcQuery,"UPDATE tContainer"
			" SET cLabel='%.24s-clone%s',"
			" cHostname='%.24s-clone%s.%s'"
			" WHERE uContainer=%u"
			" AND uSource!=0"
			" AND uDatacenter=%u",
				cSourceLabel,cuCloneNum,
				cSourceLabel,cuCloneNum,cSourceDomain,
				uContainer,
				uSourceDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return(0);

	//guFlag=6;

	return(mysql_affected_rows(&gMysql));

}//unsigned uUpdateNamesFromCloneToClone(uContainer)


void GetContainerPropertyValue(char const *cName,char *cValue,unsigned uContainer)
{
        MYSQL_RES *res;
	MYSQL_ROW field;

	cValue[0]=0;
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=3 AND cName='%s'",uContainer,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return;
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cValue,"%.255s",field[0]);
	mysql_free_result(res);

}//void GetContainerPropertyValue()


void GetSIPProxyList(char *cSIPProxyList,unsigned guDatacenter,unsigned guNode,unsigned guContainer)
{
	//Provide flexible but complete config control.

	//1 container property
	GetContainerPropertyValue("cSIPProxyList",cSIPProxyList,guContainer);
	if(!cSIPProxyList[0])
	{
		//2 container configuration
		GetConfigurationValue("cSIPProxyList",cSIPProxyList,0,0,guContainer);
		if(!cSIPProxyList[0])
		{
			//3 node configuration
			GetConfigurationValue("cSIPProxyList",cSIPProxyList,0,guNode,0);
			if(!cSIPProxyList[0])
			{
				//4 datacenter configuration
				GetConfigurationValue("cSIPProxyList",cSIPProxyList,guDatacenter,0,0);
				if(!cSIPProxyList[0])
				{
					//5 global configuration
					GetConfigurationValue("cSIPProxyList",cSIPProxyList,0,0,0);
				}
			}
		}
	}

}//void GetSIPProxyList(char *cSIPProxyList,unsigned guDatacenter,unsigned guNode,unsigned guContainer)
