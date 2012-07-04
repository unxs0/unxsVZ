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
	Missing
	/etc/asterisk/sip_general_additional.conf
	modification:

	bindport=6010 ADD
	nat=route MOD
	externip=50.97.32.50 ADD
	localnet=172.16.0.0/255.255.255.0 ADD

*/

#include "../../../mysqlrad.h"
#include <sys/sysinfo.h>

MYSQL gMysql;
char gcQuery[8192]={""};
char gcHostname[100]={""};
char gcProgram[100]={""};
unsigned guNode=0;

//dir protos
void TextConnectDb(void);

//local protos
void logfileLine(const char *cFunction,const char *cLogline,const unsigned uContainer);
void CreateIptablesData(char *cSourceIPv4);
void CreateSquidData(char *cSourceIPv4);
void ChangeFreePBX(char *cExternalIP,char *cLAN);
void GetContainerProp(const unsigned uContainer,const char *cName,char *cValue);

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
	sprintf(gcQuery,"SELECT uNode FROM tNode"
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
		sscanf(field[0],"%u",&guNode);
	mysql_free_result(res);
	if(!guNode)
	{
		fprintf(stderr,"could not determine node for %s\n",gcHostname);
		logfileLine("main","could not determine node",0);
		goto CommonExit;
	}

	if(iArgc==3)
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
	}
	else if(iArgc==4)
	{
		if(!strncmp(cArgv[1],"ChangeFreePBX",13))
		{
			ChangeFreePBX(cArgv[2],cArgv[3]);
			goto CommonExit;
		}
	}

	printf("Usage: %s CreateIptablesData|CreateSquidData <Source cIPv4>"
		" | ChangeFreePBX <External cIPv4> <LAN E.g. 10.0.0.0/255.255.255.0>\n",gcProgram);

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
		unsigned uD=0;
		sscanf(field[1],"%*u.%*u.%*u.%u",&uD);
		if(!uD)
		{
			sscanf(field[0],"%u",&uContainer);
			fprintf(stderr,"cIPv4 scan error\n");
			logfileLine("CreateIptablesData",field[1],uContainer);
			continue;
		}

		//Admin web port
		uPort=8000+uD;
		printf("#nat pbx %s\n-A PREROUTING -d %s -p tcp -m tcp --dport %u -j DNAT --to-destination %s:3321\n",
			field[1],cSourceIPv4,uPort,field[1]);
		//Zabbix port
		uPort=9000+uD;
		printf("-A PREROUTING -d %s -p tcp -m tcp --dport %u -j DNAT --to-destination %s:%u\n",
			cSourceIPv4,uPort,field[1],uPort);
		//Asterisk sip port
		uPort=6000+uD;
		printf("-A PREROUTING -p udp -m udp --dport %u -j DNAT --to-destination %s:%u\n",
			uPort,field[1],uPort);
		//Asterisk rtp port range (100 ports ~25 concurrent calls)
		uPort=10000+(uD-1)*100;
		uRangeEnd=uPort+99;
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

	printf("%s/%s\n",cLAN,cMask);

	sprintf(gcQuery,"SELECT tContainer.uContainer,tIP.cLabel FROM tIP,tContainer,tGroupGlue,tGroup"
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

		//Admin web port
		uPort=10000+(uD-1)*100;
		uRangeEnd=uPort+99;
		fprintf(fp,"[general]\n");
		fprintf(fp,"rtpstart=%u\n",uPort);
		fprintf(fp,"rtpend=%u\n",uRangeEnd);
		fclose(fp);

		//Change FreePBX MySQL data
/*
mysql> select * from sipsettings; natbx test with changes 1.2.r10
+-------------------+--------------------------+-----+------+
| keyword           | data                     | seq | type |
+-------------------+--------------------------+-----+------+
| nat               | route                    |  39 |    0 |
| nat_mode          | externip                 |  10 |    0 |
| externip_val      | 50.97.32.50              |  40 |    0 |
| externhost_val    |                          |  40 |    0 |
| externrefresh     | 120                      |  41 |    0 |
| localnet_0        | 172.16.0.0               |  42 |    0 |
| netmask_0         | 255.255.255.0            |   0 |    0 |
| canreinvite       | no                       |  10 |    0 |
| bindaddr          |                          |   2 |    0 |
| bindport          | 6010                     |   1 |    0 |
| srvlookup         | no                       |  10 |    0 |
...
mysql> select * from sipsettings; no changes 1.2.r10
+-------------------+-------------------------+-----+------+
| keyword           | data                    | seq | type |
+-------------------+-------------------------+-----+------+
| nat               | yes                     |  39 |    0 |
| nat_mode          | public                  |  10 |    0 |
| externip_val      |                         |  40 |    0 |
| externhost_val    |                         |  40 |    0 |
| externrefresh     | 120                     |  41 |    0 |
| canreinvite       | no                      |  10 |    0 |
| bindaddr          |                         |   2 |    0 |
| bindport          |                         |   1 |    0 |
| srvlookup         | no                      |  10 |    0 |
...
+---------+--------------+------+-----+---------+-------+
| Field   | Type         | Null | Key | Default | Extra |
+---------+--------------+------+-----+---------+-------+
| keyword | varchar(50)  | NO   | PRI |         |       |
| data    | varchar(255) | NO   |     |         |       |
| seq     | tinyint(1)   | NO   | PRI | 0       |       |
| type    | tinyint(1)   | NO   | PRI | 0       |       |
+---------+--------------+------+-----+---------+-------+

Conclusion need to mod/add the following:

	bindport=6010 1 0
	nat=route 39 0
	nat_mode=externip 10 0
	externip_val=50.97.32.50 40 0
	localnet_0=172.16.0.0 42 0
	netmask_0=255.255.255.0 0 0
*/
		MYSQL gMysqlExt;
		mysql_init(&gMysqlExt);
		char cLogin[256]={""};
		char cPasswd[256]={""};
		GetContainerProp(uContainer,"cOrg_AMPDBUSER",cLogin);
		GetContainerProp(uContainer,"cOrg_MySQLAsteriskPasswd",cPasswd);
		sprintf(cFile,"/vz/private/%u/var/lib/mysql/mysql.sock",uContainer);
		//debug only
		printf("%u %s %s %s\n",uContainer,cLogin,cPasswd,cFile);
		//mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
		if(mysql_real_connect(&gMysqlExt,NULL,cLogin,cPasswd,"asterisk",0,cFile,0))
		{
        		MYSQL_RES *res;
        		MYSQL_ROW field;

			uPort=6000+uD;

			//bindport
			sprintf(gcQuery,"SELECT data FROM sipsettings WHERE keyword='bindport'");
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				fprintf(stderr,gcQuery);
				logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysqlExt);
				exit(2);
			}
		        res=mysql_store_result(&gMysqlExt);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(gcQuery,"UPDATE sipsettings SET data='%u' WHERE keyword='bindport'",uPort);
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					fprintf(stderr,gcQuery);
					logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO sipsettings SET data='%u',keyword='bindport',seq=1,type=0",uPort);
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					fprintf(stderr,gcQuery);
					logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			mysql_free_result(res);

			//nat
			sprintf(gcQuery,"SELECT data FROM sipsettings WHERE keyword='nat'");
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				fprintf(stderr,gcQuery);
				logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysqlExt);
				exit(2);
			}
		        res=mysql_store_result(&gMysqlExt);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(gcQuery,"UPDATE sipsettings SET data='route' WHERE keyword='nat'");
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					fprintf(stderr,gcQuery);
					logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO sipsettings SET data='route',keyword='nat',seq=39,type=0");
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					fprintf(stderr,gcQuery);
					logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			mysql_free_result(res);

			//nat_mode
			sprintf(gcQuery,"SELECT data FROM sipsettings WHERE keyword='nat_mode'");
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				fprintf(stderr,gcQuery);
				logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysqlExt);
				exit(2);
			}
		        res=mysql_store_result(&gMysqlExt);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(gcQuery,"UPDATE sipsettings SET data='externip' WHERE keyword='nat_mode'");
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					fprintf(stderr,gcQuery);
					logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO sipsettings SET data='externip',keyword='nat_mode',seq=10,type=0");
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					fprintf(stderr,gcQuery);
					logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			mysql_free_result(res);

			//externip_val
			sprintf(gcQuery,"SELECT data FROM sipsettings WHERE keyword='externip_val'");
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				fprintf(stderr,gcQuery);
				logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysqlExt);
				exit(2);
			}
		        res=mysql_store_result(&gMysqlExt);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(gcQuery,"UPDATE sipsettings SET data='%s' WHERE keyword='externip_val'",cExternalIP);
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					fprintf(stderr,gcQuery);
					logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO sipsettings SET data='%s',keyword='externip_val',seq=40,type=0",cExternalIP);
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					fprintf(stderr,gcQuery);
					logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			mysql_free_result(res);

			//localhost_0
			sprintf(gcQuery,"SELECT data FROM sipsettings WHERE keyword='localhost_0'");
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				fprintf(stderr,gcQuery);
				logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysqlExt);
				exit(2);
			}
		        res=mysql_store_result(&gMysqlExt);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(gcQuery,"UPDATE sipsettings SET data='%s' WHERE keyword='localhost_0'",cLAN);
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					fprintf(stderr,gcQuery);
					logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO sipsettings SET data='%s',keyword='localhost_0',seq=42,type=0",cLAN);
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					fprintf(stderr,gcQuery);
					logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			mysql_free_result(res);

			//netmask_0
			sprintf(gcQuery,"SELECT data FROM sipsettings WHERE keyword='netmask_0'");
			mysql_query(&gMysqlExt,gcQuery);
			if(mysql_errno(&gMysqlExt))
			{
				fprintf(stderr,gcQuery);
				logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
				mysql_close(&gMysqlExt);
				exit(2);
			}
		        res=mysql_store_result(&gMysqlExt);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(gcQuery,"UPDATE sipsettings SET data='%s' WHERE keyword='netmask_0'",cMask);
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					fprintf(stderr,gcQuery);
					logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO sipsettings SET data='%s',keyword='netmask_0',seq=0,type=0",cMask);
				mysql_query(&gMysqlExt,gcQuery);
				if(mysql_errno(&gMysqlExt))
				{
					fprintf(stderr,gcQuery);
					logfileLine("ChangeFreePBX",mysql_error(&gMysqlExt),uContainer);
					mysql_close(&gMysqlExt);
					exit(2);
				}
			}
			mysql_free_result(res);
			mysql_close(&gMysqlExt);
		}
		else
		{
			sprintf(cCommand,"%.99s mysql_real_connect() error\n",cFile);
			fprintf(stderr,cCommand);
			logfileLine("ChangeFreePBX",cFile,uContainer);
			continue;
		}

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

}//void GetContainerProp(...)

