/*
FILE
        node.c
        $Id$

PURPOSE
        Create file or on the fly graphs for unxsVZ node related data.

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

#include "/usr/include/gdc.h"
#include "/usr/include/gdchart.h"

#include "../../local.h"

void ErrorMsg(const char *cErrorMsg);
void ConnectDb(void);

static MYSQL gMysql;

int main(int iArgc, char *cArgv[])
{
        float   a[24]={1,2,3,4,5,6,7,8,9,10,3,4,1,2,3,4,5,6,7,8,9,10,3,4};
        float   b[24]={4,6,8,9,3,3,2,9,8,7,2,1,4,6,8,9,3,3,2,9,8,7,2,1};
        float   c[24]={1,2,3,4,5,6,7,8,9,10,3,4,1,2,3,4,5,6,7,8,9,10,3,4};
        char    *t[24]={"101","102","103","104","105","106","107","108","109","110","111","112",
				"113","114","115","116","117","118","119","120","121","122","123","124"};
        unsigned long sc[2]={0xFF8080,0x8080FF};
        MYSQL_RES *res;
        MYSQL_ROW field;
        unsigned i=0;
        char cQuery[256];
        char cDatacenter[100]={"Wilshire1"};
        unsigned uNumNodes=24;

        ConnectDb();

/*
        sprintf(cQuery,"SELECT SUM(mTotal),DATE_FORMAT(FROM_UNIXTIME(uModDate), '%%Y-%%m') AS month FROM tInvoice WHERE uInvoiceStatus=3 AND DATE_FORMAT(FROM_UNIXTIME(uModDate), '%%Y')='%s' GROUP BY month ORDER BY month",cYear);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql)) ErrorMsg(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
                sscanf(field[0],"%f",&a[i]);
                i++;
        }
        mysql_free_result(res);


        sprintf(cYears,"%u/%u",nYear,nYear-1);
        sprintf(cYear,"%u",nYear-1);
        i=0;
        sprintf(cQuery,"SELECT SUM(mTotal),DATE_FORMAT(FROM_UNIXTIME(uModDate), '%%Y-%%m') AS month FROM tInvoice WHERE uInvoiceStatus=3 AND DATE_FORMAT(FROM_UNIXTIME(uModDate), '%%Y')='%s' GROUP BY month ORDER BY month",cYear);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql)) ErrorMsg(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
                sscanf(field[0],"%f",&b[i]);
                i++;
        }
        mysql_free_result(res);
*/

        GDC_BGColor= 0xFFFFFFL;/* backgound color (white) */
        GDC_SetColor= sc;/* assign set colors */
        GDC_title=cDatacenter;
        GDC_ytitle="Percent";
        GDC_xtitle="Hardware Nodes";
        GDC_bar_width = 40;

        if(getenv("REQUEST_METHOD")!=NULL)
                printf( "Content-Type: image/gif\n\n" );
        //x,y image, file, type, num of data points, array of x labels, number of data sets
        //data set 1..n float
        out_graph(1000,500,stdout,GDC_3DBAR,uNumNodes,t,3,a,b,c);

        return(0);
}

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

