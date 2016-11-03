/*
FILE
        diskutil.c
        svn ID removed

PURPOSE
        Create file or on the fly graphs for unxsVZ node container disk space
	related data.

AUTHOR
        (C) 2005-2013, Gary Wallis for Unixservice, LLC. GPLv2 Licensed
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
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>



#include "/usr/include/gdc.h"
#include "/usr/include/gdchart.h"

#include "../../local.h"

void ErrorMsg(const char *cErrorMsg);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);
unsigned GetDatacenterHealthData(unsigned uDatacenter,float *a,float *b,float *c,float *d,char *t[]);

#include "mysqlconnect.h"


unsigned GetDatacenterHealthData(unsigned uDatacenter,float *a,float *b,float *c,float *d,char *t[])
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_ROW field2;
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
		long unsigned luInstalledDiskSpace=0;
		long unsigned luContainerDiskSpace=0;
		long unsigned luHDDiskSpace=0;
		long unsigned luContainerHardLimit=0;
		char cluInstalledDiskSpace[256]={""};
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
		if((cp=strchr(field[1],'.'))) *cp=0;
		sprintf(t[uCount],"%.15s",field[1]);
	
		GetNodeProp(uNode,"luInstalledDiskSpace",cluInstalledDiskSpace);
		sscanf(cluInstalledDiskSpace,"%lu",&luInstalledDiskSpace);

		//1-.
		//Sum all node container disk usage
		sprintf(gcQuery,"SELECT SUM(CONVERT(tProperty.cValue,UNSIGNED)) FROM tProperty,tContainer WHERE"
				" tProperty.cName='1k-blocks.luUsage'"
				" AND tProperty.uType=3"
				" AND tProperty.uKey=tContainer.uContainer"
				" AND tContainer.uNode=%u",uNode);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
			ErrorMsg(mysql_error(&gMysqlUBC));
		res2=mysql_store_result(&gMysqlUBC);
		if((field2=mysql_fetch_row(res2)))
		{
			if(field2[0]!=NULL)
				sscanf(field2[0],"%lu",&luContainerDiskSpace);
		}
		mysql_free_result(res2);

		//2-.
		//Sum all node container hardlimit disk usage
		sprintf(gcQuery,"SELECT SUM(CONVERT(tProperty.cValue,UNSIGNED)) FROM tProperty,tContainer WHERE"
				" tProperty.cName='1k-blocks.luHardlimit'"
				" AND tProperty.uType=3"
				" AND tProperty.uKey=tContainer.uContainer"
				" AND tContainer.uNode=%u",uNode);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
			ErrorMsg(mysql_error(&gMysqlUBC));
		res2=mysql_store_result(&gMysqlUBC);
		if((field2=mysql_fetch_row(res2)))
		{
			if(field2[0]!=NULL)
				sscanf(field2[0],"%lu",&luContainerHardLimit);
		}
		mysql_free_result(res2);

		//3-.
		//Sum all node actual disk usage
		sprintf(gcQuery,"SELECT SUM(CONVERT(tProperty.cValue,UNSIGNED)) FROM tProperty,tContainer WHERE"
				" tProperty.cName='1k-blocks.luUsage'"
				" AND tProperty.uType=3"
				" AND tProperty.uKey=tContainer.uContainer"
				" AND tContainer.uNode=%u",uNode);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
			ErrorMsg(mysql_error(&gMysqlUBC));
		res2=mysql_store_result(&gMysqlUBC);
		if((field2=mysql_fetch_row(res2)))
		{
			if(field2[0]!=NULL)
				sscanf(field2[0],"%lu",&luHDDiskSpace);
		}
		mysql_free_result(res2);

	
		a[uCount]+=(float)luContainerDiskSpace/(float)(1024*1024);
		b[uCount]+=(float)luHDDiskSpace/(float)(1024*1024);
		c[uCount]+=(float)luContainerHardLimit/(float)(1024*1024);
		d[uCount]=(float)luInstalledDiskSpace/(float)(1024*1024);
		uCount++;

		//Not first time
		if(uPrevDatacenter && uDatacenter!=uPrevDatacenter)
		{
			mysql_close(&gMysqlUBC);
			uPrevDatacenter=uDatacenter;
		}

	}//for each node
	mysql_free_result(res);

	return(uCount);

}//unsigned GetDatacenterHealthData()


int main(int iArgc, char *cArgv[])
{
        float   a[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        float   b[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        float   c[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        float   d[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        char    *t[32]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
			NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
			NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
        unsigned long sc[4]={0xFF8080,0x8080FF,0x33CC66,0x999933};
        char cDatacenter[256]={""};
        char cYTitle[64]={""};
        unsigned uNumNodes=32;
	unsigned uDatacenter=0;//0 is all datacenters with active non appliance nodes
	register unsigned i;
        MYSQL_RES *res;
        MYSQL_ROW field;

	TextConnectDb0();

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
			sprintf(cYTitle,"GB (%.31s)",field[1]);
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
				sprintf(cYTitle,"GB (%.31s)",field[1]);
		}
		mysql_free_result(res);
	}

	if(!cDatacenter[0])
		ErrorMsg("No such datacenter");

	uNumNodes=GetDatacenterHealthData(uDatacenter,a,b,c,d,t);

	if(uNumNodes>32) uNumNodes=32;

        GDC_BGColor= 0xFFFFFFL;/* backgound color (white) */
        GDC_SetColor= sc;/* assign set colors */
        GDC_title=cDatacenter;
        GDC_ytitle=cYTitle;
        GDC_xtitle="Current container usage, hd usage, hard limit and installed disk space.";
        GDC_bar_width = 5;

        if(getenv("REQUEST_METHOD")!=NULL)
                printf( "Content-Type: image/gif\n\n" );
        //x,y image, file, type, num of data points, array of x labels, number of data sets
        //data set 1..n float
        out_graph(1000,600,stdout,GDC_3DBAR,uNumNodes,t,4,a,b,c,d);
	for(i=0;i<32 && t[i]!=NULL;i++)
		free(t[i]);

        return(0);

}//main()


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


