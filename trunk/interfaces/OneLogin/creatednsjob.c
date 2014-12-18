/*
FILE 
	creatednsjob.c
	$Id$
AUTHOR/LEGAL
	(C) 2010-2013 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	OneLogin program file.
	DNS job function
*/

#include "interface.h"
unsigned uGetPrimaryContainerGroup(unsigned uContainer);
void GetConfigurationValue(char const *cName,char *cValue,unsigned uDatacenter,unsigned uNode,unsigned uContainer);
void GetContainerProp(const unsigned uContainer,const char *cName,char *cValue);
unsigned unxsBindPBXRecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,
		const char *cJobData,unsigned uOwner,unsigned uCreatedBy);
unsigned unxsBindRemoveContainer(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
	unsigned uOwner,unsigned uCreatedBy);

unsigned CreateOrgDNSJob(unsigned uIPv4,unsigned uOwner,char const *cOptionalIPv4,char const *cHostname,
				unsigned uDatacenter,unsigned uCreatedBy,unsigned uContainer,unsigned uNode)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cJobData[512];
	char cContainerPrimaryGroup[100]={""};
	static unsigned uOnlyOnce=1;


	//Get container type
	unsigned uContainerPrimaryGroup=uGetPrimaryContainerGroup(uContainer);
#define uPBXType 1
	unsigned uContainerType=0;

	sprintf(cContainerPrimaryGroup,"%.99s",ForeignKey("tGroup","cLabel",uContainerPrimaryGroup));
	ToLower(cContainerPrimaryGroup);
	if(strstr(cContainerPrimaryGroup,"pbx"))
		uContainerType=uPBXType;

	//Create DNS job based on type
	if(uContainerType==uPBXType)
	{
		unsigned uMainPort=5060;
		unsigned uBackupPort=5060;
		char cMainIPv4[32]={""};
		char cBackupIPv4[32]={""};
		unsigned uCloneContainer=0;
		unsigned uA=0,uB=0,uC=0;

		//validation checks
		if(!uContainer)
			return 0;
		if(!uDatacenter)
			return 0;

		//Gather IPs and ports
		//Main container
		sprintf(gcQuery,"SELECT tIP.cLabel FROM tIP,tContainer"
				" WHERE tIP.uIP=tContainer.uIPv4"
				" AND tContainer.uContainer=%u",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sprintf(cMainIPv4,"%.31s",field[0]);
		mysql_free_result(res);
		//exclude rfc1918 IP clones from creating wasteful dns entries.
		if(sscanf(cMainIPv4,"%u.%u.%u.%*u",&uA,&uB,&uC)==3)
		{
			if( (uA==172 && uB>=16 && uB<=31) || (uA==192 && uB==168) || (uA==10) ) 
			{
				//a rfc1918 IP has been detected check for containter property
				//	cOrg_PublicIP
				char cOrg_PublicIP[256]={""};
				GetContainerProp(uContainer,"cOrg_PublicIP",cOrg_PublicIP);
				if(!cOrg_PublicIP[0])
					return(0);
				else
					sprintf(cMainIPv4,"%.31s",cOrg_PublicIP);
			}
		}
		sprintf(gcQuery,"SELECT tProperty.cValue FROM tContainer,tProperty"
				" WHERE tProperty.uKey=tContainer.uContainer"
				" AND tProperty.uType=3"//tType Container
				" AND tProperty.cName='cOrg_SIPPort'"
				" AND tContainer.uContainer=%u",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uMainPort);
		mysql_free_result(res);

		//Clone (backup) container
		sprintf(cBackupIPv4,"%.31s",cMainIPv4);
		sprintf(gcQuery,"SELECT tIP.cLabel,tContainer.uContainer FROM tIP,tContainer"
				" WHERE tIP.uIP=tContainer.uIPv4"
				" AND tContainer.uDatacenter!=%u"//only first remote clone
				" AND tContainer.uSource=%u",uDatacenter,uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(cBackupIPv4,"%.31s",field[0]);
			sscanf(field[1],"%u",&uCloneContainer);
		}
		mysql_free_result(res);
		//exclude rfc1918 IP clones from creating wasteful dns entries.
		if(sscanf(cBackupIPv4,"%u.%u.%u.%*u",&uA,&uB,&uC)==3)
		{
			if( (uA==172 && uB>=16 && uB<=31) || (uA==192 && uB==168) || (uA==10) ) 
			{
				//a rfc1918 IP has been detected check for containter property
				//	cOrg_PublicIP
				char cOrg_PublicIP[256]={""};
				GetContainerProp(uCloneContainer,"cOrg_PublicIP",cOrg_PublicIP);
				if(cOrg_PublicIP[0])
					sprintf(cBackupIPv4,"%.31s",cOrg_PublicIP);
				else
					sprintf(cBackupIPv4,"%.31s",cMainIPv4);
			}
		}
		sprintf(gcQuery,"SELECT tProperty.cValue FROM tContainer,tProperty"
				" WHERE tProperty.uKey=tContainer.uContainer"
				" AND tProperty.uType=3"//tType Container
				" AND tProperty.cName='cOrg_SIPPort'"
				" AND tContainer.uContainer=%u",uCloneContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uBackupPort);
		mysql_free_result(res);

		//standalone sub zone with DNS SRV records 
		//depending on remote datacenter clone use clone IP for backup
		//priority SRV record
		sprintf(cJobData,
			"cZone=%.99s;\n"
			"cMainIPv4=%.15s;\n"
			"uMainPort=%u;\n"
			"cBackupIPv4=%.15s;\n"
			"uBackupPort=%u;\n"
				,cHostname,
				cMainIPv4,
				uMainPort,
				cBackupIPv4,
				uBackupPort);
		return(unxsBindPBXRecordJob(uDatacenter,uNode,uContainer,cJobData,uOwner,uCreatedBy));
	}
	else
	{
		//standard A record job
		char cIPv4[32]={""};
		static char cView[256]={"external"};
		static char cZone[256]={""};

		//validation checks
		if(!cHostname[0])
			return 0;
		if(!uIPv4 && !cOptionalIPv4[0])
			return 0;

		//if called in loop be efficient.
		if(uOnlyOnce)
		{
			GetConfigurationValue("cunxsBindARecordJobZone",cZone,uDatacenter,0,0);
			GetConfigurationValue("cunxsBindARecordJobView",cView,uDatacenter,0,0);
			uOnlyOnce=0;
		}

		//another check
		if(!cZone[0])
			return 0;

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

		//sanity checks
		if(!cIPv4[0])
			return 0;

		sprintf(cJobData,"cName=%.99s.;\n"//Note trailing dot
			"cIPv4=%.99s;\n"
			"cZone=%.99s;\n"
			"cView=%.31s;\n",
				cHostname,cIPv4,cZone,cView);
		return(unxsBindARecordJob(uDatacenter,uNode,uContainer,cJobData,uOwner,uCreatedBy));

	}//standard A record job

}//unsigned CreateOrgDNSJob()


unsigned unxsBindPBXRecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,
		const char *cJobData,unsigned uOwner,unsigned uCreatedBy)
{
	unsigned uCount=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsBindPBXRecordJob(%u)',cJobName='unxsVZPBXSRVZone'"
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
	//unxsVZLog(uContainer,"tContainer","unxsBindPBXRecordJob");
	return(uCount);

}//unsigned unxsBindPBXRecordJob()


unsigned unxsBindARecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
	unsigned uOwner,unsigned uCreatedBy)
{
	unsigned uCount=0;
	long unsigned luJobDate=0;

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsBindARecordJob(%u)',cJobName='unxsVZContainerARR'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=if(%lu,%lu,UNIX_TIMESTAMP(NOW())+60)"
			",uJobStatus=%u"
			",cJobData='%s'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				luJobDate,luJobDate,
				uREMOTEWAITING,
				cJobData,
				uOwner,uCreatedBy);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	//unxsVZLog(uContainer,"tContainer","unxsBindARecordJob");
	return(uCount);

}//unsigned unxsBindARecordJob(...)


void GetContainerProp(const unsigned uContainer,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0) return;

	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=3 AND cName='%s'",
				uContainer,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return;
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		if((cp=strchr(field[0],'\n')))
			*cp=0;
		sprintf(cValue,"%.255s",field[0]);
	}
	mysql_free_result(res);

}//void GetContainerProp(...)


//Pull job for unxsBind
//Sample cJobData
/*
cZone=delthis.ZoneOrArecord.net.;
cView=external;
*/
unsigned unxsBindRemoveContainer(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
	unsigned uOwner,unsigned uCreatedBy)
{
	unsigned uCount=0;
	char gcQuery[512];

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsBindRemoveContainer(%u)',cJobName='unxsVZRemoveContainer'"
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
		return(0);
	uCount=mysql_insert_id(&gMysql);
	//unxsVZLog(uContainer,"tContainer","unxsBindRemoveContainer");
	return(uCount);

}//unsigned unxsBindRemoveContainer()

