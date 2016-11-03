/*
FILE
        ramutil.c
        svn ID removed

PURPOSE
        Create file or on the fly graphs for unxsVZ node container ram 
	related data.

AUTHOR
        (C) 2005-2009, Gary Wallis for Unixservice, LLC. GPL2 Licensed
REQUIRES
	CentOS 5.2+
	yum install gd gd-devel	
	Bruce Verderaime included gdchart0.11.5dev gd wrapper lib via 
	cd gdchart0.11.5dev, make, make install.
*/

#include <stdio.h>
#include <stdlib.h>
#include <values.h>
#include <mysql/mysql.h>
#include <string.h>

#include "/usr/include/gdc.h"
#include "/usr/include/gdchart.h"

#include "../../local.h"

#include "mysqlconnect.h"

void ErrorMsg(const char *cErrorMsg);
void ConnectDb(void);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);
unsigned GetDatacenterHealthData(unsigned uDatacenter, float *a, float *b,char *t[]);
void SendEmail(char *cSubject,char *cMsg);


unsigned GetDatacenterHealthData(unsigned uDatacenter,float *a,float *b,char *t[])
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;
	unsigned uPrevDatacenter=0;

	if((gLfp0=fopen(cUBCLOGFILE,"a"))==NULL)
		ErrorMsg(cUBCLOGFILE);

	if(uDatacenter)
		sprintf(gcQuery,"SELECT uNode,cLabel,uDatacenter FROM tNode WHERE uDatacenter=%u AND uStatus=1 ORDER BY uNode",uDatacenter);
	else
		sprintf(gcQuery,"SELECT uNode,cLabel,uDatacenter FROM tNode WHERE cLabel!='appliance' AND uStatus=1 ORDER BY uDatacenter,uNode");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		ErrorMsg(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{

        	MYSQL_RES *res2;
	        MYSQL_ROW field2;
		long unsigned luAllContainerPhyspages=0;
		long unsigned luAllContainer=0;
		long unsigned luTotalRAM=0;
		long unsigned luInstalledRam=0;
		char cluInstalledRam[256];
		char *cp;
		unsigned uNode=0;
		unsigned uDatacenter=0;

		sscanf(field[0],"%u",&uNode);
		sscanf(field[2],"%u",&uDatacenter);

		logfileLine0("GetDatacenterHealthData","for node",uNode);
		if(uDatacenter!=uPrevDatacenter)
		{
			ConnectToOptionalUBCDb0(uDatacenter);
		}

		t[uCount]=malloc(16);
		if((cp=strchr(field[1],'.')))
			*cp=0;
		sprintf(t[uCount],"%.15s",field[1]);

		GetNodeProp(uNode,"luInstalledRam",cluInstalledRam);
		sscanf(cluInstalledRam,"%lu",&luInstalledRam);
		if(luInstalledRam==0) luInstalledRam=1;

		//3-. Current
		//Total RAM utilization http://wiki.openvz.org/UBC_systemwide_configuration
		//3a-. All container pages times 4096
		sprintf(gcQuery,"SELECT SUM(CONVERT(tProperty.cValue,UNSIGNED)*4096) FROM tProperty,tContainer WHERE"
				" tProperty.cName='physpages.luHeld'"
				" AND tProperty.uType=3"
				" AND tProperty.uKey=tContainer.uContainer"
				" AND tContainer.uStatus!=11"//Probably active not initial setup
				" AND tContainer.uStatus!=31"// and not stopped
				" AND tContainer.uNode=%u",uNode);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
			ErrorMsg(mysql_error(&gMysqlUBC));
		res2=mysql_store_result(&gMysqlUBC);
		if((field2=mysql_fetch_row(res2)))
		{
			if(field2[0]!=NULL)
				sscanf(field2[0],"%lu",&luAllContainerPhyspages);
		}
		mysql_free_result(res2);
		//3b-. All container kmemsize + othersockbuf.luHeld + tcpsndbuf.luHeld + tcprcvbuf.luHeld +
		// dgramrcvbuf.luHeld
		sprintf(gcQuery,"SELECT SUM(CONVERT(tProperty.cValue,UNSIGNED)) FROM tProperty,tContainer WHERE"
				" ( tProperty.cName='kmemsize.luHeld' OR"
				" tProperty.cName='othersockbuf.luHeld' OR "
				" tProperty.cName='tcpsndbuf.luHeld' OR "
				" tProperty.cName='tcprcvbuf.luHeld' OR "
				" tProperty.cName='dgramrcvbuf.luHeld' )"
				" AND tProperty.uType=3"
				" AND tProperty.uKey=tContainer.uContainer"
				" AND tContainer.uStatus!=11"//Probably active not initial setup
				" AND tContainer.uStatus!=31"// and not stopped
				" AND tContainer.uNode=%u",uNode);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
			ErrorMsg(mysql_error(&gMysqlUBC));
		res2=mysql_store_result(&gMysqlUBC);
		if((field2=mysql_fetch_row(res2)))
		{
			if(field2[0]!=NULL)
				sscanf(field2[0],"%lu",&luAllContainer);
		}
		mysql_free_result(res2);
		luTotalRAM=(luAllContainerPhyspages+luAllContainer)/1000;
		a[uCount]=((float)luTotalRAM/(float)luInstalledRam) * 100.00;

		//send email warning
		if(a[uCount]>85.00)
		{
			sprintf(gcQuery,"%s %2.2f uDatacenter=%u,uNode=%u",t[uCount],a[uCount],uDatacenter,uNode);
			SendEmail("RamUtil Warning!",gcQuery);
		}

		//3-. Max held
		//Total RAM utilization http://wiki.openvz.org/UBC_systemwide_configuration
		//3a-. All container pages times 4096
		sprintf(gcQuery,"SELECT SUM(CONVERT(tProperty.cValue,UNSIGNED)*4096) FROM tProperty,tContainer WHERE"
				" tProperty.cName='physpages.luMaxheld'"
				" AND tProperty.uType=3"
				" AND tProperty.uKey=tContainer.uContainer"
				" AND tContainer.uStatus!=11"//Probably active not initial setup
				" AND tContainer.uStatus!=31"// and not stopped
				" AND tContainer.uNode=%u",uNode);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
			ErrorMsg(mysql_error(&gMysqlUBC));
		res2=mysql_store_result(&gMysqlUBC);
		if((field2=mysql_fetch_row(res2)))
		{
			if(field2[0]!=NULL)
				sscanf(field2[0],"%lu",&luAllContainerPhyspages);
		}
		mysql_free_result(res2);
		//3b-. All container kmemsize + othersockbuf.luHeld + tcpsndbuf.luHeld + tcprcvbuf.luHeld +
		// dgramrcvbuf.luHeld
		sprintf(gcQuery,"SELECT SUM(CONVERT(tProperty.cValue,UNSIGNED)) FROM tProperty,tContainer WHERE"
				" ( tProperty.cName='kmemsize.luMaxheld' OR"
				" tProperty.cName='othersockbuf.luMaxheld' OR "
				" tProperty.cName='tcpsndbuf.luMaxheld' OR "
				" tProperty.cName='tcprcvbuf.luMaxheld' OR "
				" tProperty.cName='dgramrcvbuf.luMaxheld' )"
				" AND tProperty.uType=3"
				" AND tProperty.uKey=tContainer.uContainer"
				" AND tContainer.uStatus!=11"//Probably active not initial setup
				" AND tContainer.uStatus!=31"// and not stopped
				" AND tContainer.uNode=%u",uNode);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
			ErrorMsg(mysql_error(&gMysqlUBC));
		res2=mysql_store_result(&gMysqlUBC);
		if((field2=mysql_fetch_row(res2)))
		{
			if(field2[0]!=NULL)
				sscanf(field2[0],"%lu",&luAllContainer);
		}
		mysql_free_result(res2);
		luTotalRAM=(luAllContainerPhyspages+luAllContainer)/1000;
		b[uCount]=((float)luTotalRAM/(float)luInstalledRam) * 100.00;

		uCount++;

		//Not first time
		if(uPrevDatacenter && uDatacenter!=uPrevDatacenter)
		{
			mysql_close(&gMysqlUBC);
			uPrevDatacenter=uDatacenter;
		}
	}
	mysql_free_result(res);

	return(uCount);

}//unsigned GetDatacenterHealthData()


int main(int iArgc, char *cArgv[])
{
        float   a[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        float   b[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        char    *t[32]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
			NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
			NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
        unsigned long sc[4]={0xFF8080,0x8080FF,0x33CC66,0x999933};
        char cDatacenter[256]={""};
        char cYTitle[64]={""};
        unsigned uNumNodes=32;
	unsigned uDatacenter=0;
	register unsigned i;
        MYSQL_RES *res;
        MYSQL_ROW field;

        ConnectDb();

	if(iArgc>1)
		sscanf(cArgv[1],"%u",&uDatacenter);

	if(uDatacenter)
	{
		sprintf(gcQuery,"SELECT cLabel,NOW() FROM tDatacenter WHERE uDatacenter=%u AND uStatus=1",uDatacenter);
	        mysql_query(&gMysql,gcQuery);
	        if(mysql_errno(&gMysql))
			ErrorMsg(mysql_error(&gMysql));
	        res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(cDatacenter,"%.99s",field[0]);
			sprintf(cYTitle,"Percent (%.31s)",field[1]);
		}
		mysql_free_result(res);

	}
	else
	{
		sprintf(gcQuery,"SELECT cLabel,NOW() FROM tDatacenter WHERE cLabel!='CustomerPremise' AND uStatus=1 ORDER BY uDatacenter");
	        mysql_query(&gMysql,gcQuery);
	        if(mysql_errno(&gMysql))
			ErrorMsg(mysql_error(&gMysql));
	        res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			strncat(cDatacenter,field[0],31);
			strcat(cDatacenter," ");
			if(strlen(cDatacenter)>(sizeof(cDatacenter)-32)) break;
			if(!cYTitle[0])
				sprintf(cYTitle,"Percent (%.31s)",field[1]);
		}
		mysql_free_result(res);
	}

	if(!cDatacenter[0])
		ErrorMsg("No such datacenter");

	uNumNodes=GetDatacenterHealthData(uDatacenter,a,b,t);

	if(uNumNodes>32) uNumNodes=32;

        GDC_BGColor= 0xFFFFFFL;/* backgound color (white) */
        GDC_SetColor= sc;/* assign set colors */
        GDC_title=cDatacenter;
        GDC_ytitle=cYTitle;
        GDC_xtitle="Hardware nodes: Current ram vs max held ram/installed-ram ratio.";
        GDC_bar_width = 5;

        if(getenv("REQUEST_METHOD")!=NULL)
                printf( "Content-Type: image/gif\n\n" );
        //x,y image, file, type, num of data points, array of x labels, number of data sets
        //data set 1..n float
        out_graph(1000,600,stdout,GDC_3DBAR,uNumNodes,t,2,a,b);
	for(i=0;i<32 && t[i]!=NULL;i++)
		free(t[i]);

        return(0);

}//main()


void ConnectDb(void)
{
        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,0,NULL,0))
        {
        	if (!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,0,NULL,0))
			ErrorMsg("Database server unavailable");
        }

}//end of ConnectDb()


void ErrorMsg(const char *cErrorMsg)
{
        if(getenv("REQUEST_METHOD")!=NULL)
        	printf( "Content-Type: text/plain\n\n%s\n",cErrorMsg);
	else
        	printf( "%s\n",cErrorMsg);
        exit(0);
}


void GetNodeProp(const unsigned uNode,const char *cName,char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uNode==0) return;

	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u AND uType=2 AND cName='%s'",
				uNode,cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		ErrorMsg(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		if((cp=strchr(field[0],'\n')))
			*cp=0;
		sprintf(cValue,"%.255s",field[0]);
	}
	mysql_free_result(res);

}//void GetNodeProp(...)


void SendEmail(char *cSubject,char *cMsg)
{
	FILE *fp;
	char cFrom[100]={"unxsvz-agent"};
	char cEmail[100]={"supportgrp@unixservice.com"};

	if((fp=popen("/usr/lib/sendmail -t > /dev/null","w")))
	{
		fprintf(fp,"To: %s\n",cEmail);
		fprintf(fp,"From: %s\n",cFrom);
		fprintf(fp,"Subject: %s\n\n",cSubject);
		fprintf(fp,"%s\n",cMsg);
	}
	pclose(fp);

}//void SendEmail()
