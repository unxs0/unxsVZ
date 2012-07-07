/*
FILE
	natpbx.c
	$Id$
PURPOSE
	Create /etc/sysconfig/iptables nat table section.
	Create /etc/squid/squid.conf reverse proxy configuration file.
	Create /vz/root/VEID/etc/asterisk/rtp.conf files
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2011.
	GPLv2 License applies. See LICENSE file.
NOTES
	Must run from dir that has the sipsettings.MYI and .MYD files.
*/

#include "../../../mysqlrad.h"
#include <sys/sysinfo.h>

MYSQL gMysql;
char gcQuery[8192]={""};
char gcHostname[100]={""};
char gcProgram[100]={""};
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
void ChangeFreePBX(char *cExternalIP,char *cLAN);
void GetContainerProp(const unsigned uContainer,const char *cName,char *cValue);
void unxsBindARecordJob(unsigned uContainer,char const *cHostname,char const *cIPv4);
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
	}
	else if(iArgc==4)
	{
		if(!strncmp(cArgv[1],"ChangeFreePBX",13))
		{
			ChangeFreePBX(cArgv[2],cArgv[3]);
			goto CommonExit;
		}
	}

	printf("Usage: %s\nUpdateNonNatPBXContainers\nCreateIptablesData|CreateSquidData|UpdateBind|AddPublicIP <Source cIPv4>\n"
		"ChangeFreePBX <External cIPv4> <LAN E.g. 10.0.0.0/255.255.255.0>\nAddSIPSRV <uVEID>\nDelSIPSRV <cHostname>\n",gcProgram);

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

	sprintf(gcQuery,"SELECT tContainer.uContainer,tIP.cLabel FROM tIP,tContainer,tGroupGlue,tGroup"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tContainer.uIPv4=tIP.uIP"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel LIKE '%%NatPBX%%'"
			" AND tContainer.uNode=%u ORDER BY tIP.uIP",guNode);
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
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContainer);

		unsigned uD=0;
		sscanf(field[1],"%*u.%*u.%*u.%u",&uD);
		if(!uD)
		{
			fprintf(stderr,"cIPv4 scan error\n");
			logfileLine("CreateIptablesData",field[1],uContainer);
			continue;
		}

		//Admin web port
		uPort=8000+uD;
		//Also save port info in tProperty
		char cPort[32]={"0"};
		sprintf(cPort,"%u",uPort);
		SetContainerProp(uContainer,"cOrg_AdminPort",cPort);
		SetContainerProp(uContainer,"cOrg_PublicIP",cSourceIPv4);
		printf("#nat pbx %s\n-A PREROUTING -d %s -p tcp -m tcp --dport %u -j DNAT --to-destination %s:3321\n",
			field[1],cSourceIPv4,uPort,field[1]);
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
		printf("-A PREROUTING -p udp -m udp --dport %u -j DNAT --to-destination %s:%u\n",
			uPort,field[1],uPort);
		//Asterisk rtp port range (100 ports ~25 concurrent calls)
		uPort=10000+(uD-1)*100;
		uRangeEnd=uPort+99;
		sprintf(cPort,"%u:%u",uPort,uRangeEnd);
		SetContainerProp(uContainer,"cOrg_RTPRange",cPort);
		printf("-A PREROUTING -p udp -m udp --dport %u:%u -j DNAT --to-destination %s\n",
			uPort,uRangeEnd,field[1]);

	}
	mysql_free_result(res);

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
			" AND tGroup.cLabel LIKE '%%NatPBX%%'"
			" AND tContainer.uNode=%u",guNode);
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


void ChangeFreePBX(char *cExternalIP,char *cLAN)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContainer=0;
	char cMask[16];
	unsigned uA=0,uB=0,uC=0,uD=0,umA=0,umB=0,umC=0,umD=0;

	sscanf(cLAN,"%u.%u.%u.%u/%u.%u.%u.%u",&uA,&uB,&uC,&uD,&umA,&umB,&umC,&umD);
	sprintf(cLAN,"%u.%u.%u.%u",uA,uB,uC,uD);
	sprintf(cMask,"%u.%u.%u.%u",umA,umB,umC,umD);

	printf("ChangeFreePBX() %s/%s\n",cLAN,cMask);

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
		logfileLine("ChangeFreePBX",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
/*
;
; RTP Configuration
;
[general]
;
; RTP start and RTP end configure start and end addresses
; These are the addresses where your system will RECEIVE audio and video streams.
; If you have connections across a firewall, make sure that these are open.
;
rtpstart=10900
rtpend=10999
*/
	unsigned uPort;
	unsigned uRangeEnd;
	FILE *fp;
	char cCommand[128];
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContainer);

		unsigned uD=0;
		sscanf(field[1],"%*u.%*u.%*u.%u",&uD);
		if(!uD)
		{
			fprintf(stderr,"cIPv4 scan error\n");
			logfileLine("ChangeFreePBX",field[1],uContainer);
			continue;
		}

		char cFile[128];
		sprintf(cFile,"/vz/root/%u/etc/asterisk/rtp.conf",uContainer);
		if((fp=fopen(cFile,"w"))==NULL)
		{
			sprintf(cCommand,"%.99s file open error\n",cFile);
			fprintf(stderr,cCommand);
			logfileLine("ChangeFreePBX",cFile,uContainer);
			continue;
		}

		//rtp rtcp port ranges
		uPort=10000+(uD-1)*100;
		uRangeEnd=uPort+99;
		fprintf(fp,"[general]\n");
		fprintf(fp,"rtpstart=%u\n",uPort);
		fprintf(fp,"rtpend=%u\n",uRangeEnd);
		fclose(fp);


		//Change /etc/zabbix/zabbix_agentd.conf
		//Add or modify 
		//ListenPort=9010
		sprintf(cFile,"/vz/root/%u/etc/zabbix/zabbix_agentd.conf",uContainer);

		uPort=9000+uD;
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
		
		//restart 
		sprintf(cCommand,"vzctl exec2 %u 'service iptables stop'",uContainer);
		system(cCommand);
		sprintf(cCommand,"vzctl exec2 %u 'chkconfig --level 3 iptables off'",uContainer);
		system(cCommand);
		sprintf(cCommand,"vzctl exec2 %u 'service zabbix_agentd restart'",uContainer);
		system(cCommand);
		sprintf(cCommand,"/usr/sbin/UpdateZabbixHostPort.sh %s %u",field[2],uPort);
		system(cCommand);

		//Change FreePBX MySQL data
		//Having problems with weird FreePBX table order this is a fast way to get things done.
		//This will not port easy.
		sprintf(cCommand,"/bin/cp ./sipsettings.MYD /vz/root/%u/var/lib/mysql/asterisk/",uContainer);
		system(cCommand);
		sprintf(cCommand,"/bin/cp ./sipsettings.MYI /vz/root/%u/var/lib/mysql/asterisk/",uContainer);
		system(cCommand);
		uPort=6000+uD;
		sprintf(cCommand,"sed -i -e 's/6005/%u/' /vz/root/%u/var/lib/mysql/asterisk/sipsettings.MYD",uPort,uContainer);
		system(cCommand);
		sprintf(cCommand,"sed -i -e 's/10.0.4.0/%s/' /vz/root/%u/var/lib/mysql/asterisk/sipsettings.MYD",cLAN,uContainer);
		system(cCommand);
		sprintf(cCommand,"sed -i -e 's/255.255.255.0/%s/' /vz/root/%u/var/lib/mysql/asterisk/sipsettings.MYD",cMask,uContainer);
		system(cCommand);
		sprintf(cCommand,"sed -i -e 's/192.168.192.168/%s/' /vz/root/%u/var/lib/mysql/asterisk/sipsettings.MYD",cExternalIP,uContainer);
		system(cCommand);

		//pull conf files from db
		sprintf(cCommand,"vzctl exec2 %u '/var/lib/asterisk/bin/retrieve_conf'",uContainer);
		system(cCommand);

		//restart the whole thing ok since not in use supposedly
		sprintf(cCommand,"vzctl exec2 %u 'service asterisk restart'",uContainer);
		system(cCommand);
	}
	mysql_free_result(res);

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


void unxsBindARecordJob(unsigned uContainer,char const *cHostname,char const *cIPv4)
{
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

		unxsBindARecordJob(uContainer,field[2],cSourceIPv4);
		uSIPPort=6000+uD;
		unxsBindSIPSRVRecordJob(uContainer,field[2],uSIPPort);
	}
        mysql_free_result(res);

}//void UpdateBind(char *cSourceIPv4)


void unxsBindSIPSRVRecordJob(unsigned uContainer,char const *cHostname,unsigned uSIPPort)
{
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
	unxsBindSIPSRVDelRecordJob(0,cHostname,0);

}//void DelSIPSRV()


void unxsBindSIPSRVDelRecordJob(unsigned uContainer,char const *cHostname,unsigned uSIPPort)
{
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
		logfileLine("CreateIptablesData",mysql_error(&gMysql),uContainer);
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

