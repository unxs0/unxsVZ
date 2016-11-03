/*
FILE
	clonetool.c
	svn ID removed
PURPOSE
	Provide a set of misc unxsVZ tools for managing datacenter issues
	with standard rsync stopped container clones.
	First tools needed are for creating all the files required
	for recreating a clone server node. Such as the /etc/vz/conf/VEID.conf files.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2011.
	GPLv2 License applies. See LICENSE file.
NOTES
*/

#include "../../../mysqlrad.h"
#include <sys/sysinfo.h>

MYSQL gMysql;
char gcQuery[8192]={""};
char gcHostname[100]={""};
char gcProgram[100]={""};
unsigned guNode=0;
unsigned guDatacenter=0;

//dir protos
void TextConnectDb(void);

//local protos
void logfileLine(const char *cFunction,const char *cLogline,const unsigned uContainer);
void CreateVZConfFiles(void);

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

		fprintf(gLfp,"%s unxsCloneTool:%s[%u]: %s. uContainer=%u\n",cTime,cFunction,pidThis,cLogline,uContainer);
		fflush(gLfp);
	}
	else
	{
		fprintf(stdout,"%s: unxsCloneTool::%s. uContainer=%u\n",cFunction,cLogline,uContainer);
	}

}//void logfileLine()


int main(int iArgc, char *cArgv[])
{
	struct sysinfo structSysinfo;

	sprintf(gcProgram,"%.99s",cArgv[0]);

	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"fopen logfile failed");
		logfileLine("main","fopen logfile failed",0);
		exit(1);
	}

	if(sysinfo(&structSysinfo))
	{
		fprintf(stderr,"sysinfo() failed");
		logfileLine("main","sysinfo() failed",0);
		exit(1);
	}
#define LINUX_SYSINFO_LOADS_SCALE 65536
#define JOBQUEUE_MAXLOAD 20 //This is equivalent to uptime 20.00 last 1 min avg load
	if(structSysinfo.loads[0]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
	{
		fprintf(stderr,"structSysinfo.loads[0] larger than JOBQUEUE_MAXLOAD");
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
		logfileLine("main","could not determine node",0);
		goto CommonExit;
	}

	if(iArgc==2)
	{
		if(!strncmp(cArgv[1],"CreateVZConfFiles",16))
		{
			CreateVZConfFiles();
			goto CommonExit;
		}
	}

	printf("Usage: %s CreateVZConfFiles\n",gcProgram);

CommonExit:
	mysql_close(&gMysql);
	fclose(gLfp);
	return(0);

}//main()


void CreateVZConfFiles(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_ROW field2;
	char cSystem[256];

	//Only stopped container with uSource.
	sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.uSource,tContainer.cLabel,"
			" tContainer.cHostname,tIP.cLabel"
			" FROM tNode,tIP,tContainer"
			" WHERE tContainer.uIPv4=tIP.uIP"
			" AND tNode.uNode=tContainer.uNode"
			" AND tContainer.uSource!=0"
			" AND tContainer.uStatus=31"//stopped
			" AND tContainer.uNode=%u ORDER BY tContainer.uContainer",guNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("CreateVZConfFiles",mysql_error(&gMysql),0);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		unsigned uContainer=0;
		unsigned uSource=0;
		char cSourceNode[32]={""};

		
		sscanf(field[0],"%u",&uContainer);
		sscanf(field[1],"%u",&uSource);

		if( !uContainer || !uSource )
		{
			logfileLine("CreateVZConfFiles","Missing data",uContainer);
			continue;
		}


		sprintf(gcQuery,"SELECT tNode.cLabel FROM tNode,tContainer WHERE tNode.uNode=tContainer.uNode AND tContainer.uContainer=%u",
					uSource);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,gcQuery);
			logfileLine("CreateVZConfFiles",mysql_error(&gMysql),0);
			mysql_close(&gMysql);
			exit(2);
		}
        	res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
			sprintf(cSourceNode,"%.31s",field2[0]);
		mysql_free_result(res2);

		sprintf(gcQuery,"%u %u/%s %s %s %s\n",
			uContainer,uSource,cSourceNode,field[2],field[3],field[4]);
		logfileLine("CreateVZConfFiles",gcQuery,uContainer);
		if(!cSourceNode[0])
		{
			logfileLine("CreateVZConfFiles","Missing data-2",uContainer);
			continue;
		}

		//Get conf file
		sprintf(cSystem,"scp %s:/etc/vz/conf/%u.conf /etc/vz/conf/%u.conf",cSourceNode,uSource,uContainer);
		if(system(cSystem))
		{
			logfileLine("CreateVZConfFiles",cSystem,uContainer);
			continue;
		}

		//Patch file with clone data
		/*
			#replace these lines with clone data
			IP_ADDRESS="a.b.c.d"
			HOSTNAME="n.isp.net"
			NAME="n"
			#replace this line
			ONBOOT="yes"
			#with
			ONBOOT="no"
		*/
		sprintf(cSystem,"sed -i s/^ONBOOT=.*$/ONBOOT=\\\"no\\\"/ /etc/vz/conf/%u.conf",uContainer);
		if(system(cSystem))
			logfileLine("CreateVZConfFiles",cSystem,uContainer);

		sprintf(cSystem,"sed -i s/^IP_ADDRESS=.*$/IP_ADDRESS=\\\"%s\\\"/ /etc/vz/conf/%u.conf",field[4],uContainer);
		if(system(cSystem))
			logfileLine("CreateVZConfFiles",cSystem,uContainer);

		sprintf(cSystem,"sed -i s/^HOSTNAME=.*$/HOSTNAME=\\\"%s\\\"/ /etc/vz/conf/%u.conf",field[3],uContainer);
		if(system(cSystem))
			logfileLine("CreateVZConfFiles",cSystem,uContainer);

		sprintf(cSystem,"sed -i s/^NAME=.*$/NAME=\\\"%s\\\"/ /etc/vz/conf/%u.conf",field[2],uContainer);
		if(system(cSystem))
			logfileLine("CreateVZConfFiles",cSystem,uContainer);
	}
	mysql_free_result(res);

}//void CreateVZConfFiles(void)
