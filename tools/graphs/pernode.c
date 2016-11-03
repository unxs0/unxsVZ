/*
FILE
        pernode.c
        svn ID removed

PURPOSE
        Create file or on the fly graphs for unxsVZ node container cpu power related data.

AUTHOR
        (C) 2005-2013, Gary Wallis for Unixservice, LLC. GPL2 Licensed
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

void ErrorMsg(const char *cErrorMsg);
unsigned GetDatacenterContainerData(unsigned uDatacenter,float *a,float *b,char *t[]);

#include "mysqlconnect.h"


unsigned GetDatacenterContainerData(unsigned uDatacenter,float *a,float *b,char *t[])
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_ROW field2;
	unsigned uCount=0;

	if((gLfp0=fopen(cUBCLOGFILE,"a"))==NULL)
		ErrorMsg(cUBCLOGFILE);

	if(uDatacenter)
		sprintf(gcQuery,"SELECT uNode,cLabel FROM tNode"
				" WHERE uDatacenter=%u"
				" AND uStatus=1"
				" ORDER BY uDatacenter,uNode",uDatacenter);
	else
		sprintf(gcQuery,"SELECT uNode,cLabel FROM tNode"
				" WHERE uStatus=1"
				" ORDER BY uDatacenter,uNode");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		ErrorMsg(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		char *cp;
		float fNumberOfActiveContainers=0.0;
		float fNumberOfStoppedContainers=0.0;


		t[uCount]=malloc(16);
		if((cp=strchr(field[1],'.')))
			*cp=0;
		sprintf(t[uCount],"%.15s",field[1]);

		sprintf(gcQuery,"SELECT COUNT(uContainer) FROM tContainer"
				" WHERE uNode=%s"
				" AND uStatus=1",field[0]);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			ErrorMsg(mysql_error(&gMysql));
        	res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			sscanf(field2[0],"%f",&fNumberOfActiveContainers);
			a[uCount]=fNumberOfActiveContainers;
		}
		sprintf(gcQuery,"SELECT COUNT(uContainer) FROM tContainer"
				" WHERE uNode=%s"
				" AND uStatus=31",field[0]);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			ErrorMsg(mysql_error(&gMysql));
        	res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			sscanf(field2[0],"%f",&fNumberOfStoppedContainers);
			b[uCount]=fNumberOfStoppedContainers;
		}
		mysql_free_result(res2);
		uCount++;
	}
	mysql_free_result(res);

	return(uCount);

}//unsigned GetDatacenterContainerData()



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
			sprintf(cYTitle,"Number of Containers (%.31s)",field[1]);
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
				sprintf(cYTitle,"Number of Containers (%.31s)",field[1]);
		}
		mysql_free_result(res);
	}

	if(!cDatacenter[0])
		ErrorMsg("No such datacenter");

	uNumNodes=GetDatacenterContainerData(uDatacenter,a,b,t);

	if(uNumNodes>32) uNumNodes=32;

        GDC_BGColor= 0xFFFFFFL;/* backgound color (white) */
        GDC_SetColor= sc;/* assign set colors */
        GDC_title=cDatacenter;
        GDC_ytitle=cYTitle;
        GDC_xtitle="Hardware nodes: Active and stopped containers";
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


void ErrorMsg(const char *cErrorMsg)
{
        if(getenv("REQUEST_METHOD")!=NULL)
        	printf( "Content-Type: text/plain\n\n%s\n",cErrorMsg);
	else
        	printf( "%s\n",cErrorMsg);
        exit(0);
}


