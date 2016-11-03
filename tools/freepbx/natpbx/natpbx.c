/*
FILE
	natpbx.c
	svn ID removed
PURPOSE
	Create /etc/sysconfig/iptables nat table section.
	Create /etc/squid/squid.conf reverse proxy configuration file.
	Create /vz/root/VEID/etc/asterisk/rtp.conf files
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2011-2013
	GPLv2 License applies. See LICENSE file.
NOTES
	Must run from dir that has the sipsettings.MYI and .MYD files.
	Started changes for new remote backup PBX system.
	Maximum number of NATs is 100 for now.
*/

#include "../../../mysqlrad.h"
#include <sys/sysinfo.h>

MYSQL gMysql;
char gcQuery[8192]={""};
char gcHostname[100]={""};
char gcProgram[100]={""};
unsigned guContainer=0;
unsigned guNode=0;
unsigned guDatacenter=0;

//Get all these from tConfiguration once.
char gcView[256]={"external"};
char gcZone[256]={""};
unsigned guOnlyOnce=1;

//dir protos
void TextConnectDb(void);

//local protos
void logfileLine(const char *cFunction,const char *cLogline,const unsigned uContainer);
void CreateIptablesData(char *cSourceIPv4);
void CreateSquidData(char *cSourceIPv4);
void UpdateBind(char *cSourceIPv4);
void ChangeFreePBX(unsigned uContainer);
void GetContainerProp(const unsigned uContainer,const char *cName,char *cValue);
void unxsBindARecordJob2(unsigned uContainer,char const *cHostname,char const *cIPv4);
void unxsBindSIPSRVRecordJob(unsigned uContainer,char const *cHostname,unsigned uSIPPort);
void UpdateNonNatPBXContainers(void);
void GetTextConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer);
void SetContainerProp(const unsigned uContainer,const char *cName,const char *cValue);
void AddSIPSRV(char *cuContainer);
void DelSIPSRV(char *cHostname);
void unxsBindSIPSRVDelRecordJob(unsigned uContainer,char const *cHostname,unsigned uSIPPort);
void AddPublicIP(char *cSourceIPv4);
void GetZabbixPort(char *cHostname);
const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);

static FILE *gLfp=NULL;
void logfileLine(const char *cFunction,const char *cLogline,const unsigned uContainer)
{
	if(gLfp!=NULL)
	{
		time_t luClock;
		char cTime[32];
		pid_t pidThis;
		const struct tm *tmTime;

		pidThis=getpid();

		time(&luClock);
		tmTime=localtime(&luClock);
		strftime(cTime,31,"%b %d %T",tmTime);

		fprintf(gLfp,"%s unxsNATPBX::%s[%u]: %s. uContainer=%u\n",cTime,cFunction,pidThis,cLogline,uContainer);
		fflush(gLfp);
	}
	else
	{
		fprintf(stderr,"%s: unxsNATPBX::%s. uContainer=%u\n",cFunction,cLogline,uContainer);
	}

}//void logfileLine()


unsigned GetConfiguration2(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer)
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
		fprintf(stderr,cQuery);
		logfileLine("main",mysql_error(&gMysql),0);
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
        	sprintf(cValue,"%.255s",field[0]);
        	sscanf(field[1],"%u",&uConfiguration);
	}
        mysql_free_result(res);

	return(uConfiguration);

}//unsigned GetConfiguration2(...)



int main(int iArgc, char *cArgv[])
{
	struct sysinfo structSysinfo;

	sprintf(gcProgram,"%.99s",cArgv[0]);

	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		logfileLine("main","fopen logfile failed",0);
		exit(1);
	}

	if(sysinfo(&structSysinfo))
	{
		logfileLine("main","sysinfo() failed",0);
		exit(1);
	}
#define LINUX_SYSINFO_LOADS_SCALE 65536
#define JOBQUEUE_MAXLOAD 20 //This is equivalent to uptime 20.00 last 1 min avg load
	if(structSysinfo.loads[0]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
	{
		logfileLine("main","structSysinfo.loads[0] larger than JOBQUEUE_MAXLOAD",0);
		exit(1);
	}
	//Check to see if this program is still running. If it is exit.
	//This may mean losing data gathering data points. But it
	//will avoid runaway du and other unexpected high load
	//situations. See #120.

	//Uses login data from ../../../local.h
	TextConnectDb();

	gethostname(gcHostname,98);
	//short hostname
	char *cp;
	if((cp=strchr(gcHostname,'.')))
		*cp=0;
        MYSQL_RES *res;
        MYSQL_ROW field;
	sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode"
			" WHERE cLabel='%s'",gcHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("main",mysql_error(&gMysql),0);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&guNode);
		sscanf(field[1],"%u",&guDatacenter);
	}
	mysql_free_result(res);
	if(!guNode)
	{
		fprintf(stderr,"could not determine node for %s\n",gcHostname);
		logfileLine("main","could not determine node",0);
		goto CommonExit;
	}

	if(iArgc==2)
	{
		if(!strncmp(cArgv[1],"UpdateNonNatPBXContainers",25))
		{
			UpdateNonNatPBXContainers();
			goto CommonExit;
		}
	}
	else if(iArgc==3)
	{
		if(!strncmp(cArgv[1],"CreateIptablesData",18))
		{
			CreateIptablesData(cArgv[2]);
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"CreateSquidData",15))
		{
			CreateSquidData(cArgv[2]);
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"UpdateBind",15))
		{
			UpdateBind(cArgv[2]);
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"AddSIPSRV",9))
		{
			AddSIPSRV(cArgv[2]);
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"AddPublicIP",11))
		{
			AddPublicIP(cArgv[2]);
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"DelSIPSRV",9))
		{
			DelSIPSRV(cArgv[2]);
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"GetZabbixPort",13))
		{
			GetZabbixPort(cArgv[2]);
			goto CommonExit;
		}
		else if(!strncmp(cArgv[1],"ChangeFreePBX",13))
		{
			sscanf(cArgv[2],"%u",&guContainer);
			ChangeFreePBX(guContainer);
			goto CommonExit;
		}
	}
	printf("Usage: %s\nUpdateNonNatPBXContainers\nCreateIptablesData|CreateSquidData|UpdateBind|AddPublicIP <Source cIPv4>\n"
		"ChangeFreePBX <uVEID>\nAddSIPSRV <uVEID>\nDelSIPSRV <cHostname>\n"
		"GetZabbixPort <cHostname>\n",gcProgram);
CommonExit:
	mysql_close(&gMysql);
	fclose(gLfp);
	return(0);

}//main()


void CreateIptablesData(char *cSourceIPv4)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContainer=0;
	unsigned uStatus=0;

	logfileLine("CreateIptablesData","start",uContainer);

	char cAutoNATIPClass[256]={""};
	GetConfiguration2("cAutoNATIPClass",cAutoNATIPClass,guDatacenter,guNode,0);//First try node specific
	if(!cAutoNATIPClass[0])
		GetConfiguration2("cAutoNATIPClass",cAutoNATIPClass,guDatacenter,0,0);//Then datacenter


	//Only for remote datacenter backup containers that belong to a PBX group
	sprintf(gcQuery,"SELECT tContainer.uContainer,tIP.cLabel,tContainer.uSource,tContainer.cLabel,tContainer.uStatus"
			" FROM tIP,tContainer,tGroupGlue,tGroup"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tContainer.uIPv4=tIP.uIP"
			" AND INSTR(tIP.cLabel,'%s')=1"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel LIKE '%%PBX%%'"
			" AND tContainer.uSource!=0"
			" AND tContainer.uSource NOT IN (SELECT uContainer FROM tContainer WHERE uSource=0 AND uDatacenter=%u)"
			" AND tContainer.uNode=%u ORDER BY tContainer.uContainer LIMIT 101",cAutoNATIPClass,guDatacenter,guNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("CreateIptablesData",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	//debug only
	//if((field=mysql_fetch_row(res)))
	unsigned uPort;
	unsigned uRangeEnd;
	unsigned uOnlyOnce=1;
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContainer);
		sscanf(field[4],"%u",&uStatus);
		//Only nat rfc1918 IPs
		unsigned uA=0,uB=0,uC=0;
		unsigned uD=0;
		if(sscanf(field[1],"%u.%u.%u.%u",&uA,&uB,&uC,&uD)==4)
		{
			if( !((uA==172 && uB>=16 && uB<=31) || (uA==192 && uB==168) || (uA==10)) ) 
				continue;	
		}

		//We may need to reference this later
		//sprintf(cPort,"%u",uD);
		//SetContainerProp(uContainer,"cOrg_uPortOffset",cPort);

		//Admin web port
		uPort=8000+uD;
		//Also save port info in tProperty
		char cPort[32]={"0"};
		sprintf(cPort,"%u",uPort);
		SetContainerProp(uContainer,"cOrg_AdminPort",cPort);
		SetContainerProp(uContainer,"cOrg_PublicIP",cSourceIPv4);
		printf("#nat pbx %s (%u)\n-A PREROUTING -d %s -p tcp -m tcp --dport %u -j DNAT --to-destination %s:3321\n",
			field[3],uD,cSourceIPv4,uPort,field[1]);
		//Zabbix port
		uPort=9000+uD;
		sprintf(cPort,"%u",uPort);
		SetContainerProp(uContainer,"cOrg_ZabbixPort",cPort);
		printf("-A PREROUTING -d %s -p tcp -m tcp --dport %u -j DNAT --to-destination %s:%u\n",
			cSourceIPv4,uPort,field[1],uPort);
		//Asterisk sip port
		uPort=6000+uD;
		sprintf(cPort,"%u",uPort);
		SetContainerProp(uContainer,"cOrg_SIPPort",cPort);
		//changed to std 5060
		printf("-A PREROUTING -d %s -p udp -m udp --dport %u -j DNAT --to-destination %s:%u\n",
			cSourceIPv4,uPort,field[1],uPort);
		//Asterisk rtp port range (100 ports ~25 concurrent calls)
		uPort=10000+(uD-1)*100;
		uRangeEnd=uPort+99;
		sprintf(cPort,"%u:%u",uPort,uRangeEnd);
		SetContainerProp(uContainer,"cOrg_RTPRange",cPort);
		printf("-A PREROUTING -d %s -p udp -m udp --dport %u:%u -j DNAT --to-destination %s\n",
			cSourceIPv4,uPort,uRangeEnd,field[1]);
		//special ping fwd to a single active container
		//iptables -t nat -A PREROUTING -p ICMP  -d 67.210.243.22 -j DNAT --to-destination 10.4.0.51
		if(uStatus==1 && uOnlyOnce)
		{
			printf("#\tall pings here\n-A PREROUTING -p ICMP -d %s -j DNAT --to-destination %s\n",cSourceIPv4,field[1]);
			uOnlyOnce=0;
		}

		if(uD==101)
		{
			logfileLine("CreateIptablesData","number of allowed NATs exceeded",uContainer);
			printf("#nat pbx %s (%u) exceeds limit of 100 per node\n",field[3],uD);
			break;
		}
	}

	//Only for remote datacenter backup containers that belong to a PBX group
	sprintf(gcQuery,"SELECT tIP.cLabel,tContainer.uContainer"
			" FROM tIP,tContainer,tGroupGlue,tGroup"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tContainer.uIPv4=tIP.uIP"
			" AND INSTR(tIP.cLabel,'%s')=1"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel LIKE '%%PBX%%'"
			" AND tContainer.uSource!=0"
			" AND tContainer.uSource NOT IN (SELECT uContainer FROM tContainer WHERE uSource=0 AND uDatacenter=%u)"
			" AND tContainer.uNode=%u ORDER BY tIP.cLabel LIMIT 101",cAutoNATIPClass,guDatacenter,guNode);
			//previous port based on last octet of IP number
			//" AND tContainer.uNode=%u ORDER BY tIP.uIP",guNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("CreateIptablesData",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	char cPrevClassC[32]={""};
	printf("#postrouting for each class C\n");
	while((field=mysql_fetch_row(res)))
	{
		//-A POSTROUTING -s 10.0.4.0/255.255.255.0 -j SNAT --to-source $1
		unsigned uA=0,uB=0,uC=0;
		char cClassC[32]={""};
		unsigned uCount=sscanf(field[0],"%u.%u.%u.%*u",&uA,&uB,&uC);
		//Only nat rfc1918 IPs
		if( !((uA==172 && uB>=16 && uB<=31) || (uA==192 && uB==168) || (uA==10)) ) 
				continue;	
		if(uCount!=3)
		{
			sscanf(field[1],"%u",&uContainer);
			logfileLine("CreateIptablesData","IP uCount!=3",uContainer);
			continue;
		}
		sprintf(cClassC,"%u.%u.%u.0",uA,uB,uC);
		if(strcmp(cClassC,cPrevClassC))
		{
			printf("-A POSTROUTING -s %s/24 -j SNAT --to-source %s\n",cClassC,cSourceIPv4);
			strncpy(cPrevClassC,cClassC,31);
		}
	}

	mysql_free_result(res);

	logfileLine("CreateIptablesData","end",uContainer);

}//void CreateIptablesData()


void CreateSquidData(char *cSourceIPv4)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContainer=0;

	sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cHostname,tContainer.cLabel,tIP.cLabel FROM tIP,tContainer,tGroupGlue,tGroup"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tContainer.uIPv4=tIP.uIP"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel LIKE '%%PBX%%'"
			" AND tContainer.uSource!=0"
			" AND tContainer.uSource NOT IN (SELECT uContainer FROM tContainer WHERE uSource=0 AND uDatacenter=%u)"
			" AND tContainer.uNode=%u",guDatacenter,guNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("CreateSquidData",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);

/*
#pbx 1
cache_peer 10.0.4.1 parent 443 0 no-query no-digest ssl sslflags=DONT_VERIFY_PEER originserver name=natpbx login=PASS
acl sites_natpbx dstdomain natpbx.callingcloud.net
cache_peer_access natpbx allow sites_natpbx
#...
#pbx 2
cache_peer 10.0.4.2 parent 443 0 no-query no-digest ssl sslflags=DONT_VERIFY_PEER originserver name=aolock login=PASS
acl sites_aolock dstdomain aolock.callingcloud.net
cache_peer_access aolock allow sites_aolock
#...
#pbx 100
cache_peer 10.0.4.100 parent 443 0 no-query no-digest ssl sslflags=DONT_VERIFY_PEER originserver name=n100 login=PASS
acl sites_n100 dstdomain n100.callingcloud.net
cache_peer_access n100 allow sites_n100

#pbx 1
acl our_sites dstdomain natpbx.callingcloud.net
#pbx 2
acl our_sites dstdomain aolock.callingcloud.net
#...
#pbx 100
acl our_sites dstdomain n100.callingcloud.net
#global
acl our_sites dstdomain 72.52.75.235
*/

	while((field=mysql_fetch_row(res)))
	{
		printf("#pbx %s\n",field[3]);
		printf("cache_peer %s parent 443 0 no-query no-digest ssl sslflags=DONT_VERIFY_PEER originserver name=%s login=PASS\n",
			field[3],field[2]);
		printf("acl sites_%s dstdomain %s\n",
			field[2],field[1]);
		printf("cache_peer_access %s allow sites_%s\n",
			field[2],field[2]);
	}
	mysql_data_seek(res,0);//rewind
	printf("#\n");
	while((field=mysql_fetch_row(res)))
	{
		printf("#pbx %s\nacl our_sites dstdomain %s\n",field[3],field[1]);
	}
	printf("#\n#global\nacl our_sites dstdomain %s\n",cSourceIPv4);
	mysql_free_result(res);

}//void CreateSquidData()


//This new version must be run AFTER CreateIptablesData()
void ChangeFreePBX(unsigned uContainer)
{
	logfileLine("ChangeFreePBX","start",uContainer);

	sprintf(gcQuery,"SELECT tContainer.uContainer,tIP.cLabel,tContainer.cHostname FROM tIP,tContainer,tGroupGlue,tGroup"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tContainer.uIPv4=tIP.uIP"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel LIKE '%%PBX%%'"
			" AND tContainer.uSource!=0"
			" AND tContainer.uStatus=1"
			" AND tContainer.uSource NOT IN (SELECT uContainer FROM tContainer WHERE uSource=0 AND uDatacenter=%u)"
			" AND tContainer.uNode=%u"
			" AND tContainer.uContainer=%u LIMIT 1"
					,guDatacenter,guNode,uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ChangeFreePBX",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        MYSQL_RES *res;
        MYSQL_ROW field;
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		//Data for asterisk rtp range file
		FILE *fp;
		char cFile[128];
		sprintf(cFile,"/vz/root/%u/etc/unxsvz",uContainer);
		mkdir(cFile,S_IRWXU);
		sprintf(cFile,"/vz/root/%u/etc/unxsvz/asterisk",uContainer);
		mkdir(cFile,S_IRWXU);
		sprintf(cFile,"/vz/root/%u/etc/unxsvz/asterisk/rtp.conf",uContainer);
		if((fp=fopen(cFile,"w"))==NULL)
		{
			logfileLine("ChangeFreePBX",cFile,uContainer);
			continue;
		}
		//rtp rtcp port ranges
		char cPort[32]={""};
		unsigned uPort;
		unsigned uRangeEnd;
		uPort=10000;
		uRangeEnd=20000;
		GetContainerProp(uContainer,"cOrg_RTPRange",cPort);
		sscanf(cPort,"%u:%u",&uPort,&uRangeEnd);
		fprintf(fp,"[general]\n");
		fprintf(fp,"rtpstart=%u\n",uPort);
		fprintf(fp,"rtpend=%u\n",uRangeEnd);
		fclose(fp);


		//Data for /etc/zabbix/zabbix_agentd.conf
		sprintf(cFile,"/vz/root/%u/etc/unxsvz/zabbix",uContainer);
		mkdir(cFile,S_IRWXU);
		sprintf(cFile,"/vz/root/%u/etc/unxsvz/zabbix/zabbix_agentd.conf",uContainer);
		uPort=10050;
		cPort[0]=0;
		GetContainerProp(uContainer,"cOrg_ZabbixPort",cPort);
		sscanf(cPort,"%u",&uPort);
		if((fp=fopen(cFile,"w"))==NULL)
		{
			logfileLine("ChangeFreePBX",cFile,uContainer);
			continue;
		}
		fprintf(fp,"ListenPort=%u\n",uPort);
		fclose(fp);

		//Change /etc/zabbix/zabbix_agentd.conf
		//Add or modify 
		//ListenPort=9010
		sprintf(cFile,"/vz/root/%u/etc/zabbix/zabbix_agentd.conf",uContainer);

		char cCommand[256];//used in other system calls below
		sprintf(cCommand,"grep -w ListenPort %s",cFile);
		if(system(cCommand))
		{
			sprintf(cCommand,"echo 'ListenPort=%u' >> %s",uPort,cFile);
			system(cCommand);
		}
		else
		{
			sprintf(cCommand,"sed -i -e 's/ListenPort=.*/ListenPort=%u/' %s",uPort,cFile);
			system(cCommand);
		}

		//Data for NAT configuration of asterisk
		char cOrg_PublicIP[256]={""};
		sprintf(cFile,"/vz/root/%u/etc/unxsvz/asterisk/externip",uContainer);
		GetContainerProp(uContainer,"cOrg_PublicIP",cOrg_PublicIP);
		if((fp=fopen(cFile,"w"))==NULL)
		{
			logfileLine("ChangeFreePBX",cFile,uContainer);
			continue;
		}
		fprintf(fp,"%s\n",cOrg_PublicIP);
		fclose(fp);

		char cOrg_SIPPort[256]={""};
		sprintf(cFile,"/vz/root/%u/etc/unxsvz/asterisk/bindport",uContainer);
		GetContainerProp(uContainer,"cOrg_SIPPort",cOrg_SIPPort);
		if((fp=fopen(cFile,"w"))==NULL)
		{
			logfileLine("ChangeFreePBX",cFile,uContainer);
			continue;
		}
		fprintf(fp,"%s\n",cOrg_SIPPort);
		fclose(fp);

		char cPrivateNATNetwork[256]={""};
		sprintf(cFile,"/vz/root/%u/etc/unxsvz/asterisk/localnet",uContainer);
		GetNodeProp(guNode,"cPrivateNATNetwork",cPrivateNATNetwork);
		if((fp=fopen(cFile,"w"))==NULL)
		{
			logfileLine("ChangeFreePBX",cFile,uContainer);
			continue;
		}
	 	unsigned uA=0,uB=0,uC=0;
		sscanf(field[1],"%u.%u.%u.%*u",&uA,&uB,&uC);
		fprintf(fp,"%u.%u.%u.0/%s\n",uA,uB,uC,cPrivateNATNetwork);
		fclose(fp);

		//debug only
		//logfileLine("ChangeFreePBX","container",uContainer);
		//break;

		//Turn off things not required or in the way
		sprintf(cCommand,"/usr/sbin/vzctl exec2 %u '/sbin/service iptables stop'",uContainer);
		system(cCommand);
		sprintf(cCommand,"/usr/sbin/vzctl exec2 %u '/sbin/chkconfig --level 3 iptables off'",uContainer);
		system(cCommand);
		sprintf(cCommand,"/usr/sbin/vzctl exec2 %u '/sbin/service fail2ban stop'",uContainer);
		system(cCommand);
		sprintf(cCommand,"/usr/sbin/vzctl exec2 %u '/sbin/chkconfig --level 3 fail2ban off'",uContainer);
		system(cCommand);
		sprintf(cCommand,"/usr/sbin/vzctl exec2 %u '/sbin/service sshd stop'",uContainer);
		system(cCommand);
		sprintf(cCommand,"/usr/sbin/vzctl exec2 %u '/sbin/chkconfig --level 3 sshd off'",uContainer);
		system(cCommand);
		sprintf(cCommand,"/usr/sbin/vzctl exec2 %u '/sbin/service zabbix_agentd change-hostname'",uContainer);
		system(cCommand);
		sprintf(cCommand,"/usr/sbin/vzctl exec2 %u '/sbin/service zabbix_agentd restart'",uContainer);
		system(cCommand);

		pid_t pidChild;
		pidChild=fork();
		if(pidChild!=0)
			return;
		sleep(200);
		sprintf(cCommand,"/usr/sbin/UpdateZabbixHostPort.sh %s %u > /tmp/UpdateZabbixHostPort.sh.log 2>&1",field[2],uPort);
		system(cCommand);
	}
	mysql_free_result(res);

	logfileLine("ChangeFreePBX","end",uContainer);

}//void ChangeFreePBX()


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
			fprintf(stderr,gcQuery);
			logfileLine("GetContainerProp",mysql_error(&gMysql),0);
			exit(2);
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

}//void GetContainerProp()


void SetContainerProp(const unsigned uContainer,const char *cName,const char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uContainer==0 || !cName[0] || !cValue[0]) return;

	sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uKey=%u AND uType=3 AND cName='%s'",
				uContainer,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		if(gLfp!=NULL)
		{
			fprintf(stderr,gcQuery);
			logfileLine("SetContainerProp",mysql_error(&gMysql),0);
			exit(2);
		}
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tProperty SET cValue='%s' WHERE uProperty=%s",cValue,field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			if(gLfp!=NULL)
			{
				fprintf(stderr,gcQuery);
				logfileLine("SetContainerProp",mysql_error(&gMysql),0);
				exit(2);
			}
		}
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tProperty SET cName='%s',cValue='%s',uKey=%u,uType=3,uOwner=1,uCreatedBy=1,"
										"uCreatedDate=UNIX_TIMESTAMP(NOW())",cName,cValue,uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			if(gLfp!=NULL)
			{
				fprintf(stderr,gcQuery);
				logfileLine("SetContainerProp",mysql_error(&gMysql),0);
				exit(2);
			}
		}
	}
	mysql_free_result(res);

}//void SetContainerProp()


void unxsBindARecordJob2(unsigned uContainer,char const *cHostname,char const *cIPv4)
{
	return;//this should not be used yet.
#define uREMOTEWAITING 10
	char cJobData[512]={""};

	//If called in loop be efficient.
	if(guOnlyOnce)
	{
		GetTextConfiguration("cunxsBindARecordJobZone",gcZone,guDatacenter,0,0);
		GetTextConfiguration("cunxsBindARecordJobView",gcView,guDatacenter,0,0);
		guOnlyOnce=0;
	}

	if(!uContainer || !guNode || !guDatacenter)
	{
		fprintf(stderr,"No uContainer etc. aborting!");
		logfileLine("unxsBindARecordJob","No uContainer etc. aborting!",uContainer);
		exit(2);
	}

	if(!gcView[0] || !gcZone[0])
	{
		fprintf(stderr,"No gcZone or no gcView aborting!");
		logfileLine("unxsBindARecordJob","No gcZone or no gcView aborting!",uContainer);
		exit(2);
	}

	if(!cHostname[0] || !cIPv4[0])
	{
		fprintf(stderr,"No cIPv4 or no cHostname aborting!");
		logfileLine("unxsBindARecordJob","No cIPv4 or no cHostname aborting!",uContainer);
		exit(2);
	}

	sprintf(cJobData,"cName=%.99s.;\n"//Note trailing dot
		"cIPv4=%.99s;\n"
		"cZone=%.99s;\n"
		"cView=%.31s;\n",
			cHostname,cIPv4,gcZone,gcView);

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsBindARecordJob(%u)',cJobName='unxsVZContainerARR'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=%u"
			",cJobData='%s'"
			",uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				guDatacenter,
				guNode,
				uContainer,
				uREMOTEWAITING,
				cJobData);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("unxsBindARecordJob",mysql_error(&gMysql),0);
		exit(2);
	}

}//unsigned unxsBindARecordJob()


void GetTextConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        char cQuery[1024];
	char cExtra[100]={""};

        sprintf(cQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='%s'",
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
		fprintf(stderr,gcQuery);
		logfileLine("GetTextConfiguration",mysql_error(&gMysql),0);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
        	sprintf(cValue,"%.255s",field[0]);
        mysql_free_result(res);

}//void GetTextConfiguration()


void UpdateBind(char *cSourceIPv4)
{
	return;//this should not be used yet.
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContainer=0;
	unsigned uSIPPort=0;

	sprintf(gcQuery,"SELECT tContainer.uContainer,tIP.cLabel,tContainer.cHostname FROM tIP,tContainer,tGroupGlue,tGroup"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tContainer.uIPv4=tIP.uIP"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel LIKE '%%NatPBX%%'"
			" AND tContainer.uNode=%u",guNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("UpdateBind",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		unsigned uD=0;

		sscanf(field[0],"%u",&uContainer);
		sscanf(field[1],"%*u.%*u.%*u.%u",&uD);
		if(!uD)
		{
			fprintf(stderr,"cIPv4 scan error\n");
			logfileLine("UpdateBind",field[1],uContainer);
			continue;
		}

		//No dns required should be already setup via unxsVZ new zone system
		//unxsBindARecordJob2(uContainer,field[2],cSourceIPv4);
		uSIPPort=6000+uD;
		//unxsBindSIPSRVRecordJob(uContainer,field[2],uSIPPort);
	}
        mysql_free_result(res);

}//void UpdateBind(char *cSourceIPv4)


void unxsBindSIPSRVRecordJob(unsigned uContainer,char const *cHostname,unsigned uSIPPort)
{
	return;//this should not be used yet.
#define uREMOTEWAITING 10
	char cJobData[512]={""};

	//If called in loop be efficient.
	if(guOnlyOnce)
	{
		GetTextConfiguration("cunxsBindARecordJobZone",gcZone,guDatacenter,0,0);
		GetTextConfiguration("cunxsBindARecordJobView",gcView,guDatacenter,0,0);
		guOnlyOnce=0;
	}

	if(!uContainer || !guNode || !guDatacenter)
	{
		fprintf(stderr,"No uContainer etc. aborting!");
		logfileLine("unxsBindSIPSRVRecordJob","No uContainer etc. aborting!",uContainer);
		exit(2);
	}

	if(!gcView[0] || !gcZone[0])
	{
		fprintf(stderr,"No gcZone or no gcView aborting!");
		logfileLine("unxsBindSIPSRVRecordJob","No gcZone or no gcView aborting!",uContainer);
		exit(2);
	}

	if(!cHostname[0] || !uSIPPort)
	{
		fprintf(stderr,"No uSIPPort or no cHostname aborting!");
		logfileLine("unxsBindSIPSRVRecordJob","No uSIPPort or no cHostname aborting!",uContainer);
		exit(2);
	}

//_sip._udp.natpbx.callingcloud.net. 	SRV 10	1	6010	natpbx.callingcloud.net.	Testing UDP DNS SRV transport for nat PBX
	sprintf(cJobData,"cName=_sip._udp.%.99s.;\n"//Note trailing dot
		"uPriority=%u;\n"
		"uWeight=%u;\n"
		"uPort=%u;\n"
		"cTarget=%.99s.;\n"
		"cZone=%.99s;\n"
		"cView=%.31s;\n",
			cHostname,10,1,uSIPPort,cHostname,gcZone,gcView);

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsBindSIPSRVRecordJob(%u)',cJobName='unxsVZContainerSRVRR'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=%u"
			",cJobData='%s'"
			",uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				guDatacenter,
				guNode,
				uContainer,
				uREMOTEWAITING,
				cJobData);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("unxsBindSIPRSVRecordJob",mysql_error(&gMysql),0);
		exit(2);
	}

}//unsigned unxsBindSIPRSVRecordJob()


void UpdateNonNatPBXContainers(void)
{
	return;//this should not be used yet.
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContainer=0;

	sprintf(gcQuery,"SELECT tContainer.uContainer FROM tContainer,tGroupGlue,tGroup"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel NOT LIKE '%%NatPBX%%'"
			" AND tGroup.cLabel LIKE '%%PBX%%' AND (tContainer.uStatus=1 OR tContainer.uStatus=31)");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("UpdateNonNatPBXContainers",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContainer);
		printf("%u\n",uContainer);
		SetContainerProp(uContainer,"cOrg_AdminPort","3321");
                SetContainerProp(uContainer,"cOrg_ZabbixPort","10050");
                SetContainerProp(uContainer,"cOrg_SIPPort","5060");
                SetContainerProp(uContainer,"cOrg_RTPRange","10000:20000");
                SetContainerProp(uContainer,"cOrg_SSHPort","14432");
	}
	mysql_free_result(res);

}//void UpdateNonNatPBXContainers()


void AddSIPSRV(char *cuContainer)
{
	return;//this should not be used yet.
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContainer=0;
	unsigned uSIPPort=0;

	sscanf(cuContainer,"%u",&uContainer);
	sprintf(gcQuery,"SELECT tContainer.uContainer,tIP.cLabel,tContainer.cHostname FROM tIP,tContainer,tGroupGlue,tGroup"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tContainer.uIPv4=tIP.uIP"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel LIKE '%%NatPBX%%'"
			" AND tContainer.uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("AddSIPSRV",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		unsigned uD=0;

		sscanf(field[0],"%u",&uContainer);
		sscanf(field[1],"%*u.%*u.%*u.%u",&uD);
		if(!uD)
		{
			fprintf(stderr,"cIPv4 scan error\n");
			logfileLine("AddSIPSRV",field[1],uContainer);
			return;
		}

		uSIPPort=6000+uD;
		unxsBindSIPSRVRecordJob(uContainer,field[2],uSIPPort);
	}
        mysql_free_result(res);

}//void AddSIPSRV()


void DelSIPSRV(char *cHostname)
{
	return;//this should not be used yet.
	unxsBindSIPSRVDelRecordJob(0,cHostname,0);

}//void DelSIPSRV()


void unxsBindSIPSRVDelRecordJob(unsigned uContainer,char const *cHostname,unsigned uSIPPort)
{
	return;//this should not be used yet.
	char cJobData[512]={""};

	//If called in loop be efficient.
	if(guOnlyOnce)
	{
		GetTextConfiguration("cunxsBindARecordJobZone",gcZone,guDatacenter,0,0);
		GetTextConfiguration("cunxsBindARecordJobView",gcView,guDatacenter,0,0);
		guOnlyOnce=0;
	}

	if(!guNode || !guDatacenter)
	{
		fprintf(stderr,"No guNode aborting!");
		logfileLine("unxsBindSIPSRVDelRecordJob","No guNode aborting!",uContainer);
		exit(2);
	}

	if(!gcView[0] || !gcZone[0])
	{
		fprintf(stderr,"No gcZone or no gcView aborting!");
		logfileLine("unxsBindSIPSRVDelRecordJob","No gcZone or no gcView aborting!",uContainer);
		exit(2);
	}

	if(!cHostname[0])
	{
		fprintf(stderr,"No cHostname aborting!");
		logfileLine("unxsBindSIPSRVDelRecordJob","No cHostname aborting!",uContainer);
		exit(2);
	}

//_sip._udp.natpbx.callingcloud.net. 	SRV 10	1	6010	natpbx.callingcloud.net.	Testing UDP DNS SRV transport for nat PBX
	sprintf(cJobData,"cName=_sip._udp.%.99s.;\n"//Note trailing dot
		"uPriority=%u;\n"
		"uWeight=%u;\n"
		"uPort=%u;\n"
		"cTarget=%.99s.;\n"
		"cZone=%.99s;\n"
		"cView=%.31s;\n",
			cHostname,10,1,uSIPPort,cHostname,gcZone,gcView);

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='unxsBindSIPSRVDelRecordJob(%u)',cJobName='unxsVZContainerDelSRVRR'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=%u"
			",cJobData='%s'"
			",uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				guDatacenter,
				guNode,
				uContainer,
				uREMOTEWAITING,
				cJobData);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("unxsBindSIPRSVDelRecordJob",mysql_error(&gMysql),0);
		exit(2);
	}

}//unsigned unxsBindSIPRSVDelRecordJob()


void AddPublicIP(char *cSourceIPv4)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContainer=0;

	return;//this should not be used yet.

	sprintf(gcQuery,"SELECT tContainer.uContainer FROM tIP,tContainer,tGroupGlue,tGroup"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tContainer.uIPv4=tIP.uIP"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel LIKE '%%NatPBX%%'"
			" AND tContainer.uNode=%u",guNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("AddPublicIP",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContainer);

		SetContainerProp(uContainer,"cOrg_PublicIP",cSourceIPv4);

	}
	mysql_free_result(res);

}//void AddPublicIP()


void GetZabbixPort(char *cHostname)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uA=0,uB=0,uC=0;
	unsigned uD=0;

	sprintf(gcQuery,"SELECT tIP.cLabel FROM tIP,tContainer,tGroupGlue,tGroup"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tContainer.uIPv4=tIP.uIP"
			" AND tContainer.cHostname='%s'"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel LIKE '%%PBX%%'"
			" AND tContainer.uNode=%u ORDER BY tIP.uIP LIMIT 1",cHostname,guNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("GetZabbixPort",mysql_error(&gMysql),0);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(sscanf(field[0],"%u.%u.%u.%u",&uA,&uB,&uC,&uD)!=4)
		{
			fprintf(stderr,"cIPv4 scan error %s\n",cHostname);
			logfileLine("GetZabbixPort",field[0],0);
		}
	}
	mysql_free_result(res);

	//Only nat rfc1918 IPs can have non standard port
	if( !((uA==172 && uB>=16 && uB<=31) || (uA==192 && uB==168) || (uA==10)) ) 
		uD=0;

	if(uD)
		printf("%u\n",uD+9000);
	else
		printf("10050\n");
}//void GetZabbixPort(char *cHostname)


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
		logfileLine("GetNodeProp",mysql_error(&gMysql),0);
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
