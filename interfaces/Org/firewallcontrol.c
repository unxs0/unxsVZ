/*
FILE 
	unxsVZ/interface/Org/firewallcontrol.c
	svn ID removed
PURPOSE
	
AUTHOR/LEGAL
	(C) 2001-2014 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
OTHER
	Only tested on CentOS 5 with Firefox and Chrome browsers.
FREE HELP
	support @ openisp . net
	supportgrp @ unixservice . com
	Join mailing list: https://lists.openisp.net/mailman/listinfo/unxsvz
*/

#include "interface.h"
#include "../../local.h"

//firewallcontrol TOC
//from unxsvz library
void GetClientProp(const unsigned uClient,const char *cName,char *cValue);
void GetIPPropFromHost(const char *cHostIP,const char *cName,char *cValue);
void SetUpdateIPProp(char const *cName,char const *cValue,unsigned uIP);
void SetUpdateIPPropFromHost(char const *cName,char const *cValue,char const *cHostIP);
unsigned uGetOrAddLoginSessionHostIP(const char *cHostIP);
void CreateLoginSession(const char *gcHost,const char *gcUser,const char *gcCompany);
void RemoveLoginSession(const char *gcHost,const char *gcUser,const char *gcCompany);
void NewNoDupsIPProp(char const *cName,char const *cValue,unsigned uIP);
void LogoutFirewallJobs(unsigned uLoginClient);
void LoginFirewallJobs(unsigned uLoginClient);

FILE *gLfp=NULL;

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
			htmlPlainTextError(mysql_error(&gMysql));
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
			htmlPlainTextError(mysql_error(&gMysql));
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


void SetUpdateIPProp(char const *cName,char const *cValue,unsigned uIP)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uKey=%u AND uType=31 AND cName='%s'",uIP,cName);
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
		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,cName='%s',cValue='%s',uType=31,uOwner=%u,"
				"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uIP,
					cName,cValue,guLoginClient,guLoginClient);
        	mysql_query(&gMysql,gcQuery);
	}
}//void SetUpdateIPProp();


//UBC safe should not be used for UBCs	
void SetUpdateIPPropFromHost(char const *cName,char const *cValue,char const *cHostIP)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uProperty FROM tProperty"
			" WHERE uKey IN (SELECT uIP FROM tIP WHERE uIPNum=INET_ATON('%s'))"
			" AND uType=31 AND cName='%s'",cHostIP,cName);
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
		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=INET_ATON('%s'),cName='%s',cValue='%s',uType=31,uOwner=%u,"
				"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					cHostIP,
					cName,cValue,guLoginClient,guLoginClient);
        	mysql_query(&gMysql,gcQuery);
	}
}//void SetUpdateIPPropFromHost()


#define uLOGIN_IPTYPE 7
unsigned uGetOrAddLoginSessionHostIP(const char *cHostIP)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uIP=0;

	sprintf(gcQuery,"SELECT uIP FROM tIP"
			" WHERE uIPNum=INET_ATON('%s') LIMIT 1",cHostIP);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uIP);
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tIP"
				" SET cLabel='%.15s',"
				" uIPNum=INET_ATON('%s'),"
				" uIPType=%u,"
				" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					cHostIP,cHostIP,uLOGIN_IPTYPE,guOrg,guLoginClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		uIP=mysql_insert_id(&gMysql);
	}
	return(uIP);
}//unsigned uGetOrAddLoginSessionHostIP(const char *cHostIP)


void CreateLoginSession(const char *cHostIP,const char *gcUser,const char *gcCompany)
{
	unsigned uIP=0;

	if((uIP=uGetOrAddLoginSessionHostIP(cHostIP)))
	{
		char cValue[256]={""};
		//must match RemoveLoginSession() SQL query
		sprintf(cValue,"gcUser=%.31s;gcCompany=%.31s;",gcUser,gcOrgName);
		NewNoDupsIPProp("cLoginSession",cValue,uIP);
	}
}//void CreateLoginSession()


void RemoveLoginSession(const char *cHostIP,const char *gcUser,const char *gcCompany)
{
	if(!cHostIP[0] || !gcUser[0]) return;

	//31 is tIP
	sprintf(gcQuery,"DELETE FROM tProperty"
			" WHERE uKey IN (SELECT uIP FROM tIP WHERE uIPNum=INET_ATON('%.15s'))"
			" AND uType=31 AND cValue='gcUser=%.31s;gcCompany=%.31s;'",
				cHostIP,gcUser,gcOrgName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void RemoveLoginSession()


//UBC safe should not be used for UBCs	
void NewNoDupsIPProp(char const *cName,char const *cValue,unsigned uIP)
{
        MYSQL_RES *res;

	//31 is tIP property type
	sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uKey=%u AND uType=31 AND cName='%s' AND cValue='%s'",uIP,cName,cValue);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)<1)
	{
		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,cName='%s',cValue='%s',uType=31,uOwner=%u,"
				"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uIP,
					cName,cValue,guOrg,guLoginClient);
        	mysql_query(&gMysql,gcQuery);
	}
}//void NewNoDupsIPProp();


void LoginFirewallJobs(unsigned uLoginClient)
{
	//Only users with two factor authentication via OTP
	//can access hardware nodes at this time.
	if(!gcOTPSecret[0])
		return;
        MYSQL_RES *res;
        MYSQL_RES *res2;
	MYSQL_ROW field;

	//Master condition
	char cCreateLoginJobs[256]={""};
	GetConfiguration("cCreateLoginJobs",cCreateLoginJobs,0,0,0,0);
	if(strncmp(cCreateLoginJobs,"Via tNode::tProperty:cCreateLoginJobs",sizeof("Via tNode::tProperty:cCreateLoginJobs")))
		return;

	//Check tClient for what kind of LoginFirewallJobs to create
	char cEnableSSHOnLogin[256]={""};
	char cJobName[32]={"LoginFirewallJobHTTP"};
	GetClientProp(uLoginClient,"cEnableSSHOnLogin",cEnableSSHOnLogin);
	if(!strncmp(cEnableSSHOnLogin,"Yes",sizeof("Yes")))
		sprintf(cJobName,"%.31s","LoginFirewallJob");

	//Check for cLoginSession
	char cLoginSession[256]={""};
	char gcUserSection[64]={""};
	GetIPPropFromHost(gcHost,"cLoginSession",cLoginSession);
	if(cLoginSession[0])
	{
		sprintf(gcUserSection,"gcUser=%s;",gcLogin);
		if(strstr(cLoginSession,gcUserSection))
		//already logged in do nothing
		return;
	}

	//For existing or creates new tIP entry
	//For each unique IP or NAT user we create a session
	CreateLoginSession(gcHost,gcLogin,gcOrgName);

	//If NAT user:
	//No need to create login jobs since session already exists for another user same IP?
	if(cLoginSession[0])
		return;

	//Only for nodes with tProperty.cName=cCreateLoginJobs
	sprintf(gcQuery,"SELECT DISTINCT tNode.uNode,tNode.uDatacenter"
				" FROM tNode,tDatacenter,tProperty"
				" WHERE tNode.uDatacenter=tDatacenter.uDatacenter"
				" AND tProperty.uKey=tNode.uNode"
				" AND tProperty.cName='cCreateLoginJobs'"
				" AND tProperty.cValue='Yes'"
				" AND tDatacenter.uStatus=1"
				" AND tNode.uStatus=1"
				" AND tNode.cLabel!='appliance'");
	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		return;
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		unsigned uNode=0;
		unsigned uDatacenter=0;

		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);

		//avoid extra job queue traffic
		//delete any pending logout jobs
		sprintf(gcQuery,"DELETE FROM tJob WHERE cLabel='LogoutFirewallJobs(%u)'"
					" AND uDatacenter=%u AND uNode=%u AND uContainer=0"
					" AND uJobStatus=1"
					" AND cJobData='cIPv4=%.15s;\ntConfiguration:cCreateLoginJobs=Via tNode::tProperty:cCreateLoginJobs;'",
						uLoginClient,
						uDatacenter,
						uNode,
						gcHost);
		mysql_query(&gMysql,gcQuery);

		//do not add again
		sprintf(gcQuery,"SELECT uJob FROM tJob"
				" WHERE cLabel='LoginFirewallJobs(%u)' AND cJobName='%s' AND uJobStatus=1"
				" AND uDatacenter=%u AND uNode=%u AND uContainer=0"
				" AND cJobData='cIPv4=%.15s;\ntConfiguration:cCreateLoginJobs=Via tNode::tProperty:cCreateLoginJobs;'",
							uLoginClient,cJobName,uDatacenter,uNode,gcHost);
		mysql_query(&gMysql,gcQuery);
        	if(!mysql_errno(&gMysql))
		{
			res2=mysql_store_result(&gMysql);
        		if(mysql_num_rows(res2)==0)
			{
				sprintf(gcQuery,"INSERT INTO tJob SET cLabel='LoginFirewallJobs(%u)',cJobName='%s'"
					",uDatacenter=%u,uNode=%u,uContainer=0"
					",uJobDate=UNIX_TIMESTAMP(NOW())"
					",uJobStatus=1"
					",cJobData='cIPv4=%.15s;\ntConfiguration:cCreateLoginJobs=Via tNode::tProperty:cCreateLoginJobs;'"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uLoginClient,
						cJobName,
						uDatacenter,
						uNode,
						gcHost,
						guOrg,guLoginClient);
				mysql_query(&gMysql,gcQuery);
			}
		}
	}
}//void LoginFirewallJobs(unsigned uLoginClient)


void LogoutFirewallJobs(unsigned uLoginClient)
{
        MYSQL_RES *res;
        MYSQL_RES *res2;
	MYSQL_ROW field;

	char cCreateLoginJobs[256]={""};
	GetConfiguration("cCreateLoginJobs",cCreateLoginJobs,0,0,0,0);
	if(strncmp(cCreateLoginJobs,"Via tNode::tProperty:cCreateLoginJobs",sizeof("Via tNode::tProperty:cCreateLoginJobs")))
		return;

	//Check tClient for what kind of LoginFirewallJobs to create
	char cEnableSSHOnLogin[256]={""};
	char cJobName[32]={"LogoutFirewallJobHTTP"};
	GetClientProp(uLoginClient,"cEnableSSHOnLogin",cEnableSSHOnLogin);
	if(!strncmp(cEnableSSHOnLogin,"Yes",sizeof("Yes")))
		sprintf(cJobName,"%.31s","LogoutFirewallJob");

	//Remove corresponding session
	RemoveLoginSession(gcHost,gcLogin,gcOrgName);
	//Check for any remaining cLoginSession for given IP
	char cLoginSession[256]={""};
	GetIPPropFromHost(gcHost,"cLoginSession",cLoginSession);
	if(cLoginSession[0])
		//someone else logged in from same IP
		return;

	sprintf(gcQuery,"SELECT DISTINCT tNode.uNode,tNode.uDatacenter"
				" FROM tNode,tDatacenter,tProperty"
				" WHERE tNode.uDatacenter=tDatacenter.uDatacenter"
				" AND tProperty.uKey=tNode.uNode"
				" AND tProperty.cName='cCreateLoginJobs'"
				" AND tProperty.cValue='Yes'"
				" AND tDatacenter.uStatus=1"
				" AND tNode.uStatus=1"
				" AND tNode.cLabel!='appliance'");
	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		return;
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		unsigned uNode=0;
		unsigned uDatacenter=0;

		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);

		//avoid extra job queue traffic
		//delete any pending login jobs
		sprintf(gcQuery,"DELETE FROM tJob WHERE cLabel='LoginFirewallJobs(%u)' AND cJobName='%s'"
					" AND uDatacenter=%u AND uNode=%u AND uContainer=0"
					" AND uJobStatus=1"
					" AND cJobData='cIPv4=%.15s;\ntConfiguration:cCreateLoginJobs=Via tNode::tProperty:cCreateLoginJobs;'",
						uLoginClient,
						cJobName,
						uDatacenter,
						uNode,
						gcHost);
		mysql_query(&gMysql,gcQuery);

		//do not add again
		sprintf(gcQuery,"SELECT uJob FROM tJob"
				" WHERE cLabel='LogoutFirewallJobs(%u)' AND cJobName='%s' AND uJobStatus=1"
				" AND uDatacenter=%u AND uNode=%u AND uContainer=0"
				" AND cJobData='cIPv4=%.15s;\ntConfiguration:cCreateLoginJobs=Via tNode::tProperty:cCreateLoginJobs;'",
							uLoginClient,cJobName,uDatacenter,uNode,gcHost);
		mysql_query(&gMysql,gcQuery);
        	if(!mysql_errno(&gMysql))
		{
			res2=mysql_store_result(&gMysql);
        		if(mysql_num_rows(res2)==0)
			{
				sprintf(gcQuery,"INSERT INTO tJob SET cLabel='LogoutFirewallJobs(%u)',cJobName='%s'"
					",uDatacenter=%u,uNode=%u,uContainer=0"
					",uJobDate=UNIX_TIMESTAMP(NOW())"
					",uJobStatus=1"
					",cJobData='cIPv4=%.15s;\ntConfiguration:cCreateLoginJobs=Via tNode::tProperty:cCreateLoginJobs;'"
					",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uLoginClient,
						cJobName,
						uDatacenter,
						uNode,
						gcHost,
						guOrg,guLoginClient);
				mysql_query(&gMysql,gcQuery);
			}
		}
	}
}//void LogoutFirewallJobs(unsigned uLoginClient)
