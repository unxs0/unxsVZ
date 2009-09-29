/*
FILE
        privmem.c
        $Id$

PURPOSE
        Create file or on the fly graphs for unxsVZ node privmem related data.

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

void ErrorMsg(const char *cErrorMsg);
void ConnectDb(void);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);
unsigned GetDatacenterHealthData(unsigned uDatacenter, float *a, float *b, char *t[]);

static MYSQL gMysql;
static char gcQuery[1024];


unsigned GetDatacenterHealthData(unsigned uDatacenter,float *a,float *b,char *t[])
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;
	unsigned uNode=0;

	sprintf(gcQuery,"SELECT uNode,cLabel FROM tNode WHERE uDatacenter=%u",uDatacenter);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		ErrorMsg(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{

        	MYSQL_RES *res2;
	        MYSQL_ROW field2;
		long unsigned luInstalledRam=0;
		char cluInstalledRam[256];

		sscanf(field[0],"%u",&uNode);
		t[uCount]=malloc(16);
		sprintf(t[uCount],"%.15s",field[1]);

		GetNodeProp(uNode,"luInstalledRam",cluInstalledRam);
		sscanf(cluInstalledRam,"%lu",&luInstalledRam);
		if(luInstalledRam==0) luInstalledRam=1;

		//1a-. (current privvmpages vs RAM )
		sprintf(gcQuery,"SELECT SUM(CONVERT(tProperty.cValue,UNSIGNED)) FROM tProperty,tContainer WHERE"
				" tProperty.cName='privvmpages.luHeld'"
				" AND tProperty.uType=3"
				" AND tProperty.uKey=tContainer.uContainer"
				" AND tContainer.uStatus!=11"//Probably active not initial setup
				" AND tContainer.uStatus!=31"// and not stopped
				" AND tContainer.uNode=%u",uNode);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			ErrorMsg(mysql_error(&gMysql));
		res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			long unsigned luContainerPrivvmpagesMaxHeld=0;

			sscanf(field2[0],"%lu",&luContainerPrivvmpagesMaxHeld);
			a[uCount]=((float)luContainerPrivvmpagesMaxHeld/ (float) luInstalledRam) * 100.00 ;
		}
		mysql_free_result(res2);

		//1b-. (Max privvmpages vs RAM )
		sprintf(gcQuery,"SELECT SUM(CONVERT(tProperty.cValue,UNSIGNED)) FROM tProperty,tContainer WHERE"
				" tProperty.cName='privvmpages.luMaxheld'"
				" AND tProperty.uType=3"
				" AND tProperty.uKey=tContainer.uContainer"
				" AND tContainer.uStatus!=11"//Probably active not initial setup
				" AND tContainer.uStatus!=31"// and not stopped
				" AND tContainer.uNode=%u",uNode);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			ErrorMsg(mysql_error(&gMysql));
		res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			long unsigned luContainerPrivvmpagesMaxHeld=0;

			sscanf(field2[0],"%lu",&luContainerPrivvmpagesMaxHeld);
			b[uCount]=((float)luContainerPrivvmpagesMaxHeld/ (float) luInstalledRam) * 100.00 ;
		}
		mysql_free_result(res2);

		uCount++;
	}
	mysql_free_result(res);

	return(uCount);

}//unsigned GetDatacenterHealthData()



int main(int iArgc, char *cArgv[])
{
        float   a[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        float   b[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        char    *t[24]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
			NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
			NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
        unsigned long sc[2]={0xFF8080,0x8080FF};
        char cDatacenter[100]={""};
        unsigned uNumNodes=24;
	unsigned uDatacenter=1;
	register unsigned i;
        MYSQL_RES *res;
        MYSQL_ROW field;

        ConnectDb();

	if(iArgc>1)
		sscanf(cArgv[1],"%u",&uDatacenter);

	sprintf(gcQuery,"SELECT cLabel FROM tDatacenter WHERE uDatacenter=%u",uDatacenter);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		ErrorMsg(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cDatacenter,"%.99s",field[0]);
	mysql_free_result(res);

	if(!cDatacenter[0])
		ErrorMsg("No such datacenter");

	uNumNodes=GetDatacenterHealthData(uDatacenter,a,b,t);

	if(uNumNodes>24) uNumNodes=24;

        GDC_BGColor= 0xFFFFFFL;/* backgound color (white) */
        GDC_SetColor= sc;/* assign set colors */
        GDC_title=cDatacenter;
        GDC_ytitle="Percent";
        GDC_xtitle="Hardware nodes: Current vs max held privmem-pages/installed-ram ratio.";
        GDC_bar_width = 10;

        if(getenv("REQUEST_METHOD")!=NULL)
                printf( "Content-Type: image/gif\n\n" );
        //x,y image, file, type, num of data points, array of x labels, number of data sets
        //data set 1..n float
        out_graph(1000,500,stdout,GDC_3DBAR,uNumNodes,t,2,a,b);
	for(i=0;i<24 && t[i]!=NULL;i++)
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

