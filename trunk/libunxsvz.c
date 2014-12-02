/*
FILE 
	unxsVZ/libunxsvz.c
	$Id$
PURPOSE
	Start moving shared code into a library
NOTES
AUTHOR/LEGAL
	(C) 2011-2014 Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
FREE HELP
	support @ openisp . net
	supportgrp @ unixservice . com
	Join mailing list: https://lists.openisp.net/mailman/listinfo/unxsvz
*/

#include "libunxsvz.h"
#include <ctype.h>
char *strptime(const char *s, const char *format, struct tm *tm);

//file TOC
void ErrorMsg(const char *cText);
void SetContainerStatus(unsigned uContainer,unsigned uStatus);
void SetContainerNode(unsigned uContainer,unsigned uNode);
void SetContainerDatacenter(unsigned uContainer,unsigned uDatacenter);
unsigned uCheckMaxContainers(unsigned uNode);
unsigned uCheckMaxCloneContainers(unsigned uNode);
const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);
unsigned GetConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer,
		unsigned uHtml);
unsigned ConnectToOptionalUBCDb(unsigned uDatacenter,unsigned uPrivate);
void unxsVZ(const char *cText);
unsigned CreateDNSJob(unsigned uIPv4,unsigned uOwner,char const *cOptionalIPv4,char const *cHostname,
				unsigned uDatacenter,unsigned uCreatedBy,unsigned uContainer,unsigned uNode);
unsigned uNodeCommandJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,
			unsigned uOwner, unsigned uLoginClient, unsigned uConfiguration, char *cArgs);
void SetNodeProp(char const *cName,char const *cValue,unsigned uNode);
unsigned uGetPrimaryContainerGroup(unsigned uContainer);
time_t cStartDateToUnixTime(char *cStartDate);
time_t cStartTimeToUnixTime(char *cStartTime);
char *ToLower(char *cInput);
void unxsVZLog(unsigned uTablePK, char *cTableName, char *cLogEntry);
unsigned unxsBindPBXRecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
	unsigned uOwner,unsigned uCreatedBy);
unsigned unxsBindARecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
		unsigned uOwner,unsigned uCreatedBy);
unsigned unxsBindARecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
	unsigned uOwner,unsigned uCreatedBy);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);
void GetNodePropUBC(const unsigned uNode,const char *cName,char *cValue);
void GetDatacenterProp(const unsigned uDatacenter,const char *cName,char *cValue);
void logfileLine(const char *cFunction,const char *cLogline);
void GetContainerProp(const unsigned uContainer,const char *cName,char *cValue);
void GetContainerPropUBC(const unsigned uContainer,const char *cName,char *cValue);
unsigned SetContainerPropertyUBC(const unsigned uContainer,const char *cPropertyName,const  char *cPropertyValue);
unsigned SetContainerProperty(const unsigned uContainer,const char *cPropertyName,const  char *cPropertyValue);
void GetGroupProp(const unsigned uGroup,const char *cName,char *cValue);
void GetClientProp(const unsigned uClient,const char *cName,char *cValue);
void GetIPProp(const unsigned uIP,const char *cName,char *cValue);
void GetIPPropFromHost(const char *cHostIP,const char *cName,char *cValue);
void SetUpdateIPProp(char const *cName,char const *cValue,unsigned uIP);
void SetUpdateIPPropFromHost(char const *cName,char const *cValue,char const *cHostIP);
unsigned uGetOrAddLoginSessionHostIP(const char *cHostIP);
void CreateLoginSession(const char *cHostIP,const char *gcUser,const char *gcCompany);
void RemoveLoginSession(const char *cHostIP,const char *gcUser,const char *gcCompany);
void NewNoDupsIPProp(char const *cName,char const *cValue,unsigned uIP);
void GetIPPropFromHost(const char *cHostIP,const char *cName,char *cValue);

//This is a compatability function that should be deprecated and replaced.
void ErrorMsg(const char *cText)
{
	printf("%s\n",cText);
}//void ErrorMsg(const char *cText)

void SetContainerStatus(unsigned uContainer,unsigned uStatus)
{
	sprintf(gcQuery,"UPDATE tContainer SET uStatus=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uContainer=%u",
					uStatus,guLoginClient,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		ErrorMsg(mysql_error(&gMysql));

}//void SetContainerStatus(unsigned uContainer,unsigned uStatus)


void SetContainerNode(unsigned uContainer,unsigned uNode)
{
	sprintf(gcQuery,"UPDATE tContainer SET uNode=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uContainer=%u",
					uNode,guLoginClient,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		ErrorMsg(mysql_error(&gMysql));

}//void SetContainerNode(unsigned uContainer,unsigned uNode)


void SetContainerDatacenter(unsigned uContainer,unsigned uDatacenter)
{
	sprintf(gcQuery,"UPDATE tContainer SET uDatacenter=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uContainer=%u",
					uDatacenter,guLoginClient,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		ErrorMsg(mysql_error(&gMysql));

}//void SetContainerDatacenter(unsigned uContainer,unsigned uDatacenter)


//This is for select set type one at a time checking.
unsigned uCheckMaxContainers(unsigned uNode)
{
	unsigned uMaxContainers=0;
	char cBuffer[256]={""};
	GetNodeProp(uNode,"MaxContainers",cBuffer);
	if(cBuffer[0])
	{
		//for now we update here also
		MYSQL_RES *res;
		MYSQL_ROW field;
		char cBuffer2[128]={""};
		sprintf(cBuffer2,"SELECT COUNT(uContainer) FROM tContainer WHERE uNode=%u"
			" AND uStatus!=11 AND uStatus!=3 and uStatus!=7",uNode);
		mysql_query(&gMysql,cBuffer2);
		if(mysql_errno(&gMysql))
			ErrorMsg(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			SetNodeProp("ActiveContainers",field[0],uNode);
		mysql_free_result(res);

		unsigned uActiveContainers=0;
		sscanf(cBuffer,"%u",&uMaxContainers);
		cBuffer[0]=0;
		GetNodeProp(uNode,"ActiveContainers",cBuffer);
		//debug only tContainer(cBuffer);
		if(cBuffer[0])
			sscanf(cBuffer,"%u",&uActiveContainers);

		if(uActiveContainers && uMaxContainers && uActiveContainers>=uMaxContainers)
			return(1);
	}
	return(0);

}//unsigned uCheckMaxContainers(unsigned uNode)


//This is for select set type one at a time checking.
unsigned uCheckMaxCloneContainers(unsigned uNode)
{
	unsigned uMaxCloneContainers=0;
	char cBuffer[256]={""};
	GetNodeProp(uNode,"MaxCloneContainers",cBuffer);
	if(cBuffer[0])
	{
		//for now we update here also
		MYSQL_RES *res;
		MYSQL_ROW field;
		char cBuffer2[128]={""};
		sprintf(cBuffer2,"SELECT COUNT(uContainer) FROM tContainer WHERE uNode=%u AND uSource!=0",uNode);
		mysql_query(&gMysql,cBuffer2);
		if(mysql_errno(&gMysql))
			ErrorMsg(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			SetNodeProp("CloneContainers",field[0],uNode);
		mysql_free_result(res);

		unsigned uCloneContainers=0;
		sscanf(cBuffer,"%u",&uMaxCloneContainers);
		cBuffer[0]=0;
		GetNodeProp(uNode,"CloneContainers",cBuffer);
		//debug only tContainer(cBuffer);
		if(cBuffer[0])
			sscanf(cBuffer,"%u",&uCloneContainers);

		if(uCloneContainers && uMaxCloneContainers && uCloneContainers>=uMaxCloneContainers)
			return(1);
	}
	return(0);

}//unsigned uCheckMaxCloneContainers(unsigned uNode)


const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

	static char scKey[16];

        sprintf(gcQuery,"SELECT %s FROM %s WHERE _rowid=%u",
                        cFieldName,cTableName,uKey);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql)) return(mysql_error(&gMysql));

        mysqlRes=mysql_store_result(&gMysql);
        if(mysql_num_rows(mysqlRes)==1)
        {
                mysqlField=mysql_fetch_row(mysqlRes);
                return(mysqlField[0]);
        }

	if(!uKey)
	{
        	return("---");
	}
	else
	{
		sprintf(scKey,"%u",uKey);
        	return(scKey);
	}

}//const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey)


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
	{
        	        ErrorMsg(mysql_error(&gMysql));
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
        	sprintf(cValue,"%.255s",field[0]);
        	sscanf(field[1],"%u",&uConfiguration);
	}
        mysql_free_result(res);

	return(uConfiguration);

}//unsigned GetConfiguration(...)


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


void unxsVZFake(const char *cText)
{
	printf("%s\n",cText);
}//void unxsVZFake(const char *cText)


//Create DNS job for unxsBind based on tContainer type.
unsigned CreateDNSJob(unsigned uIPv4,unsigned uOwner,char const *cOptionalIPv4,char const *cHostname,
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
		unsigned uSource=0;
		unsigned uStatus=0;
		unsigned uA=0,uB=0,uC=0;

		//validation checks
		if(!uContainer)
		{
			unxsVZLog(uContainer,"tContainer","CreateDNSJob-err1");
			return 0;
		}
		if(!uDatacenter)
		{
			unxsVZLog(uContainer,"tContainer","CreateDNSJob-err2");
			return 0;
		}

		//Gather IPs and ports
		//Main container
		sprintf(gcQuery,"SELECT tIP.cLabel,tContainer.uSource,tContainer.uStatus FROM tIP,tContainer"
				" WHERE tIP.uIP=tContainer.uIPv4"
				" AND tContainer.uContainer=%u",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			ErrorMsg(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(cMainIPv4,"%.31s",field[0]);
			sscanf(field[1],"%u",&uSource);
			sscanf(field[2],"%u",&uStatus);
		}
		mysql_free_result(res);

		//Temp test hack
		//If the container is a stopped clone we do not need a job created.
		//Note broken reverse logic error
		if(uSource && uStatus==uAWAITDNSMIG)
		{
			unxsVZLog(uContainer,"tContainer","Temp test hack");
			return(1);
		}

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
				{
					unxsVZLog(uContainer,"tContainer","CreateDNSJob-err3");
					return(0);
				}
				else
				{
					sprintf(cMainIPv4,"%.31s",cOrg_PublicIP);
				}
			}
		}
		sprintf(gcQuery,"SELECT tProperty.cValue FROM tContainer,tProperty"
				" WHERE tProperty.uKey=tContainer.uContainer"
				" AND tProperty.uType=3"//tType Container
				" AND tProperty.cName='cOrg_SIPPort'"
				" AND tContainer.uContainer=%u",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			ErrorMsg(mysql_error(&gMysql));
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
			ErrorMsg(mysql_error(&gMysql));
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
			ErrorMsg(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uBackupPort);
		mysql_free_result(res);

		//standalone sub zone with DNS SRV records 
		//depending on remote datacenter clone use clone IP for backup
		//priority SRV record
		if(cOptionalIPv4!=NULL && cOptionalIPv4[0])
		{
			//special case we use the optional IP twice. But use any port configs that exist.
			sprintf(cJobData,
			"cZone=%.99s;\n"
			"cMainIPv4=%.15s;\n"
			"uMainPort=%u;\n"
			"cBackupIPv4=%.15s;\n"
			"uBackupPort=%u;\n"
				,cHostname,
				cOptionalIPv4,
				uMainPort,
				cOptionalIPv4,
				uBackupPort);
		}
		else
		{
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
		}
		unsigned uRetVal=0;
		if(!(uRetVal=unxsBindPBXRecordJob(uDatacenter,uNode,uContainer,cJobData,uOwner,uCreatedBy)))
		{
			unxsVZLog(uContainer,"tContainer","CreateDNSJob-err4");
			return(0);
		}
		return(uRetVal);
	}
	else
	{
		//standard A record job
		char cIPv4[32]={""};
		static char cView[256]={"external"};
		static char cZone[256]={""};

		//validation checks
		if(!cHostname[0])
		{
			unxsVZLog(uContainer,"tContainer","CreateDNSJob-err5");
			return(0);
		}
		if(!uIPv4 && !cOptionalIPv4[0])
		{
			unxsVZLog(uContainer,"tContainer","CreateDNSJob-err6");
			return(0);
		}

		//if called in loop be efficient.
		if(uOnlyOnce)
		{
			GetConfiguration("cunxsBindARecordJobZone",cZone,uDatacenter,0,0,0);
			GetConfiguration("cunxsBindARecordJobView",cView,uDatacenter,0,0,0);
			uOnlyOnce=0;
		}

		//another check
		if(!cZone[0])
		{
			unxsVZLog(uContainer,"tContainer","CreateDNSJob-err7");
			return(0);
		}

		if(cOptionalIPv4!=NULL && cOptionalIPv4[0])
		{
			sprintf(cIPv4,"%.31s",cOptionalIPv4);

		}
		else if(uIPv4)
		{
			sprintf(gcQuery,"SELECT cLabel FROM tIP WHERE uIP=%u",uIPv4);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				ErrorMsg(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sprintf(cIPv4,"%.31s",field[0]);
			mysql_free_result(res);
		}

		//sanity checks
		if(!cIPv4[0])
		{
			unxsVZLog(uContainer,"tContainer","CreateDNSJob-err8");
			return 0;
		}

		sprintf(cJobData,"cName=%.99s.;\n"//Note trailing dot
			"cIPv4=%.99s;\n"
			"cZone=%.99s;\n"
			"cView=%.31s;\n",
				cHostname,cIPv4,cZone,cView);
		unsigned uRetVal=0;
		if(!(uRetVal=(unxsBindARecordJob(uDatacenter,uNode,uContainer,cJobData,uOwner,uCreatedBy))));
		{
			unxsVZLog(uContainer,"tContainer","CreateDNSJob-err9");
			//mysql_insert_id() is not returning uJob for some reason.
			return(1);
		}
		return(uRetVal);

	}//standard A record job

}//unsigned CreateDNSJob()


unsigned uNodeCommandJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,
			unsigned uOwner, unsigned uLoginClient, unsigned uConfiguration, char *cArgs)
{
	unsigned uJob=0;
	long unsigned luJobDate=0;

	if(cStartTime[0] && cStartDate[0])
	{
		long unsigned luJobTime=0;
		luJobDate=cStartDateToUnixTime(cStartDate);
		if(luJobDate== -1)
			luJobDate=0;
		luJobTime=cStartTimeToUnixTime(cStartTime);
		if(luJobTime== -1)
			luJobTime=0;
		luJobDate+=luJobTime;
	}

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='NodeCommandJob',cJobName='NodeCommandJob'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=if(%lu,%lu,UNIX_TIMESTAMP(NOW())+60)"
			",uJobStatus=1"
			",cJobData='uConfiguration=%u;\n%.256s'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uDatacenter,uNode,uContainer,
				luJobDate,luJobDate,
				uConfiguration,cArgs,
				uOwner,uLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		ErrorMsg(mysql_error(&gMysql));
	uJob=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","NodeCommandJob");
	return(uJob);
}//unsigned uNodeCommandJob()


//UBC safe should not be used for UBCs	
void SetNodeProp(char const *cName,char const *cValue,unsigned uNode)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uKey=%u AND uType=2 AND cName='%s'",uNode,cName);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		ErrorMsg(mysql_error(&gMysql));
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
		ErrorMsg(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(field[0]!=NULL)
			sscanf(field[0],"%u",&uGroup);
	}
	mysql_free_result(res);

	return(uGroup);

}//unsigned uGetPrimaryContainerGroup(unsigned uContainer)


time_t cStartDateToUnixTime(char *cStartDate)
{
        struct  tm locTime;
        time_t  res;

        bzero(&locTime, sizeof(struct tm));
	if(strchr(cStartDate,'/'))
        	strptime(cStartDate,"%m/%d/%Y", &locTime);
        locTime.tm_isdst = -1; //forces mktime to determine if DST is in effect
        res = mktime(&locTime);
        return(res);
}//time_t cStartDateToUnixTime(char *cStartDate)


time_t cStartTimeToUnixTime(char *cStartTime)
{
        time_t  res;
	unsigned uHr=0,uMin=0,uSec=0;	

	sscanf(cStartTime,"%u:%u:%u",&uHr,&uMin,&uSec);
	res=uHr*3600+uMin*60+uSec;

        return(res);
}//time_t cStartTimeToUnixTime(char *cStartTime)

char *ToLower(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
		if(isupper(cInput[i])) cInput[i]=tolower(cInput[i]);
	cInput[i]=0;

	return(cInput);

}//char *ToLower(char *cInput)


void unxsVZLog(unsigned uTablePK, char *cTableName, char *cLogEntry)
{
	char cQuery[512]={""};

	//uLogType==1 is this back-office cgi by default tLogType install
        sprintf(cQuery,"INSERT INTO tLog SET cLabel='%.63s',uLogType=1,uPermLevel=%u,uLoginClient=%u,cLogin='%.99s',"
			" cHost='%.99s',uTablePK=%u,cTableName='%.31s',cHash=MD5(CONCAT('%s','%u','%u','%s','%s',"
			" '%u','%s','%s',UNIX_TIMESTAMP(NOW()))),uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
		cLogEntry,
		guPermLevel,
		guLoginClient,
		gcLogin,
		gcHost,
		uTablePK,
		cTableName,
		cLogEntry,
		guPermLevel,
		guLoginClient,
		gcLogin,
		gcHost,
		uTablePK,
		cTableName,
		cLogKey,
		guCompany);

	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql)) ErrorMsg(mysql_error(&gMysql));
}//void unxsVZLog()


unsigned unxsBindPBXRecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
	unsigned uOwner,unsigned uCreatedBy)
{
	unsigned uCount=0;
	long unsigned luJobDate=0;

	if(cStartTime[0] && cStartDate[0])
	{
		long unsigned luJobTime=0;
		luJobDate=cStartDateToUnixTime(cStartDate);
		if(luJobDate== -1)
			luJobDate=0;
		luJobTime=cStartTimeToUnixTime(cStartTime);
		if(luJobTime== -1)
			luJobTime=0;
		luJobDate+=luJobTime;
	}

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsBindPBXRecordJob(%u)',cJobName='unxsVZPBXSRVZone'"
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
		ErrorMsg(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","unxsBindPBXRecordJob");
	return(uCount);

}//unsigned unxsBindPBXRecordJob()


unsigned unxsBindARecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
	unsigned uOwner,unsigned uCreatedBy)
{
	unsigned uCount=0;
	long unsigned luJobDate=0;
	char gcQuery[512];

	if(cStartTime[0] && cStartDate[0])
	{
		long unsigned luJobTime=0;
		luJobDate=cStartDateToUnixTime(cStartDate);
		if(luJobDate== -1)
			luJobDate=0;
		luJobTime=cStartTimeToUnixTime(cStartTime);
		if(luJobTime== -1)
			luJobTime=0;
		luJobDate+=luJobTime;
	}

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
		ErrorMsg(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uContainer,"tContainer","unxsBindARecordJob");
	return(uCount);

}//unsigned unxsBindARecordJob()



//Not UBC safe. Use function below
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uNode==0) return;

	//2 is node
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=2 AND cName='%s'",
				uNode,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("GetNodeProp",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		if((cp=strchr(field[0],'\n')))
			*cp=0;
		sprintf(cValue,"%.255s",field[0]);
	}
	mysql_free_result(res);

}//void GetNodeProp()


//UBC safe
void GetNodePropUBC(const unsigned uNode,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uNode==0) return;

	unsigned uDatacenter=0;
	sscanf(ForeignKey("tNode","uDatacenter",uNode),"%u",&uDatacenter);
	if(!uDatacenter)
	{
		logfileLine("GetNodePropUBC","!uDatacenter error");
		return;
	}
	if(ConnectToOptionalUBCDb(uDatacenter,1))
	{
		logfileLine("GetNodePropUBC","ConnectToOptionalUBCDb error");
		return;
	}
	//2 is node
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=2 AND cName='%s'",
				uNode,cName);
	mysql_query(&gMysqlUBC,gcQuery);
	if(mysql_errno(&gMysqlUBC))
	{
		logfileLine("GetNodePropUBC",mysql_error(&gMysqlUBC));
		return;
	}
        res=mysql_store_result(&gMysqlUBC);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		if((cp=strchr(field[0],'\n')))
			*cp=0;
		sprintf(cValue,"%.255s",field[0]);
	}
	mysql_free_result(res);

}//void GetNodePropUBC()


//UBC safe
void GetDatacenterProp(const unsigned uDatacenter,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uDatacenter==0) return;

	//1 is datacenter
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=1 AND cName='%s'",
				uDatacenter,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(gLfp!=NULL)
		{
			logfileLine("GetDatacenterProp",mysql_error(&gMysql));
			exit(2);
		}
		else
		{
			ErrorMsg(mysql_error(&gMysql));
		}
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		if((cp=strchr(field[0],'\n')))
			*cp=0;
		sprintf(cValue,"%.255s",field[0]);
	}
	mysql_free_result(res);

}//void GetDatacenterProp()


void logfileLine(const char *cFunction,const char *cLogline)
{
	time_t luClock;
	char cTime[32];
	pid_t pidThis;
	const struct tm *tmTime;

	pidThis=getpid();

	time(&luClock);
	tmTime=localtime(&luClock);
	strftime(cTime,31,"%b %d %T",tmTime);

	if(gLfp==NULL)
		gLfp=stdout;
        fprintf(gLfp,"%s jobqueue.%s[%u]: %s\n",cTime,cFunction,pidThis,cLogline);
	fflush(gLfp);

}//void logfileLine(char *cLogline)


//Do not use for UBC props. Not distributed UBC safe. Use the below function.
void GetContainerProp(const unsigned uContainer,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0) return;

	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=3 AND cName='%s'",
				uContainer,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(gLfp!=NULL)
		{
			logfileLine("GetContainerProp",mysql_error(&gMysql));
			exit(2);
		}
		else
		{
			ErrorMsg(mysql_error(&gMysql));
		}
	}
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


//UBC safe
//Only for UBC properties
void GetContainerPropUBC(const unsigned uContainer,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0) return;

	unsigned uDatacenter=0;
	sscanf(ForeignKey("tContainer","uDatacenter",uContainer),"%u",&uDatacenter);
	if(!uDatacenter)
	{
		logfileLine("GetContainerPropUBC","!uDatacenter error");
		return;
	}
	unsigned uPrivate=0;
	if(guDatacenter && guDatacenter==uDatacenter)
		uPrivate=1;
	if(ConnectToOptionalUBCDb(uDatacenter,uPrivate))
	{
		logfileLine("GetContainerPropUBC","ConnectToOptionalUBCDb error");
		return;
	}
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=3 AND cName='%s'",
				uContainer,cName);
	mysql_query(&gMysqlUBC,gcQuery);
	if(mysql_errno(&gMysqlUBC))
	{
		logfileLine("GetContainerPropUBC",mysql_error(&gMysqlUBC));
		return;	
	}
        res=mysql_store_result(&gMysqlUBC);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		if((cp=strchr(field[0],'\n')))
			*cp=0;
		sprintf(cValue,"%.255s",field[0]);
	}
	mysql_free_result(res);
	mysql_close(&gMysqlUBC);

}//void GetContainerPropUBC()


//UBC safe
unsigned SetContainerPropertyUBC(const unsigned uContainer,const char *cPropertyName,const  char *cPropertyValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0 || cPropertyName[0]==0 || cPropertyValue[0]==0)
		return(1);

	unsigned uDatacenter=0;
	sscanf(ForeignKey("tContainer","uDatacenter",uContainer),"%u",&uDatacenter);
	if(!uDatacenter)
	{
		logfileLine("SetContainerPropertyUBC","!uDatacenter error");
		return(1);
	}
	if(ConnectToOptionalUBCDb(uDatacenter,1))
	{
		logfileLine("SetContainerPropertyUBC","ConnectToOptionalUBCDb error");
		return(1);
	}

	//UBC safe
	sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uType=3 AND uKey=%u AND cName='%s'",
					uContainer,cPropertyName);
	mysql_query(&gMysqlUBC,gcQuery);
	if(mysql_errno(&gMysqlUBC))
	{
		logfileLine("SetContainerPropertyUBC",mysql_error(&gMysqlUBC));
		return(2);
	}
        res=mysql_store_result(&gMysqlUBC);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tProperty SET cValue='%s' WHERE uProperty=%s",
					cPropertyValue,field[0]);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
		{
			mysql_free_result(res);
			logfileLine("SetContainerPropertyUBC",mysql_error(&gMysqlUBC));
			return(3);
		}
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tProperty SET cName='%s',cValue='%s',uType=3,uKey=%u,"
				"uOwner=(SELECT uOwner FROM tContainer WHERE uContainer=%u),"
				"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					cPropertyName,cPropertyValue,uContainer,uContainer);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
		{
			mysql_free_result(res);
			logfileLine("SetContainerPropertyUBC",mysql_error(&gMysqlUBC));
			return(4);
		}
	}
	mysql_free_result(res);

	return(0);

}//void SetContainerPropertyUBC()


//Not UBC safe use above function
unsigned SetContainerProperty(const unsigned uContainer,const char *cPropertyName,const  char *cPropertyValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0 || cPropertyName[0]==0 || cPropertyValue[0]==0)
		return(1);

	sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uType=3 AND uKey=%u AND cName='%s'",
					uContainer,cPropertyName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("SetContainerProperty",mysql_error(&gMysql));
		return(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tProperty SET cValue='%s' WHERE uProperty=%s",
					cPropertyValue,field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			mysql_free_result(res);
			logfileLine("SetContainerProperty",mysql_error(&gMysql));
			return(3);
		}
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tProperty SET cName='%s',cValue='%s',uType=3,uKey=%u,"
				"uOwner=(SELECT uOwner FROM tContainer WHERE uContainer=%u),"
				"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					cPropertyName,cPropertyValue,uContainer,uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			mysql_free_result(res);
			logfileLine("SetContainerProperty",mysql_error(&gMysql));
			return(4);
		}
	}
	mysql_free_result(res);

	return(0);

}//void SetContainerProperty()


//UBC safe
void GetGroupProp(const unsigned uGroup,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uGroup==0) return;

	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=11 AND cName='%s'",
				uGroup,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("GetGroupProp",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		if((cp=strchr(field[0],'\n')))
			*cp=0;
		sprintf(cValue,"%.255s",field[0]);
	}
	mysql_free_result(res);

}//void GetGroupProp()


//UBC safe
void GetClientProp(const unsigned uClient,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uClient==0) return;

	//41 is tClient
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=41 AND cName='%s'",
				uClient,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(gLfp!=NULL)
		{
			logfileLine("GetClientProp",mysql_error(&gMysql));
			exit(2);
		}
		else
		{
			ErrorMsg(mysql_error(&gMysql));
		}
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		if((cp=strchr(field[0],'\n')))
			*cp=0;
		sprintf(cValue,"%.255s",field[0]);
	}
	mysql_free_result(res);

}//void GetClientProp()


//UBC safe
void GetIPProp(const unsigned uIP,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uIP==0) return;

	//31 is tIP
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=31 AND cName='%s'",
				uIP,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(gLfp!=NULL)
		{
			logfileLine("GetIPProp",mysql_error(&gMysql));
			exit(2);
		}
		else
		{
			ErrorMsg(mysql_error(&gMysql));
		}
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		if((cp=strchr(field[0],'\n')))
			*cp=0;
		sprintf(cValue,"%.255s",field[0]);
	}
	mysql_free_result(res);

}//void GetIPProp()

//UBC safe
void GetIPPropFromHost(const char *cHostIP,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(!cHostIP[0]) return;

	//31 is tIP
	sprintf(gcQuery,"SELECT cValue FROM tProperty"
			" WHERE uKey IN (SELECT uIP FROM tIP WHERE uIPNum=INET_ATON('%.15s'))"
			" AND uType=31 AND cName='%s'",
				cHostIP,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(gLfp!=NULL)
		{
			logfileLine("GetIPProp",mysql_error(&gMysql));
			exit(2);
		}
		else
		{
			ErrorMsg(mysql_error(&gMysql));
		}
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		if((cp=strchr(field[0],'\n')))
			*cp=0;
		sprintf(cValue,"%.255s",field[0]);
	}
	mysql_free_result(res);

}//void GetIPPropFromHost()

