/*
FILE
	datacenter.c
	$Id$
AUTHOR
	(C) 2009, Gary Wallis for Unixservice USA
PURPOSE
	unxsvz-autonomics datacenter rule calculation and actions.
NOTES
*/

#include "autonomics.h"


int DatacenterAutonomics(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	if(!guDatacenter || !guNode)
	{
		sprintf(gcQuery,"SELECT tDatacenter.uDatacenter,tNode.uNode FROM tDatacenter,tNode WHERE"
			" tNode.cLabel='%s' AND tNode.uDatacenter=tDatacenter.uDatacenter",gcHostname);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine((char *)mysql_error(&gMysql));
			sighandlerLeave(400);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&guDatacenter);
			sscanf(field[1],"%u",&guNode);
			sprintf(gcQuery,"guDatacenter=%u guNode=%u",guDatacenter,guNode);
			logfileLine(gcQuery);
		}
		mysql_free_result(res);
	}

	if(!gcDatacenterWarnEmail[0] && guDatacenter)
	{
		//TODO define 1 type datacenter
		sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
			" cName='WarningEmail' AND tProperty.uKey=%u AND tProperty.uType=1",guDatacenter);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine((char *)mysql_error(&gMysql));
			sighandlerLeave(400);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(gcDatacenterWarnEmail,"%.99s",field[0]);
			sprintf(gcQuery,"gcDatacenterWarnEmail=%s",field[0]);
			logfileLine(gcQuery);
		}
		mysql_free_result(res);
	}

	if(!gcDatacenterAutonomics[0])
	{
		//TODO define 1 type datacenter
		sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
			" cName='Autonomics' AND tProperty.uKey=%u AND tProperty.uType=1",guDatacenter);
		mysqlQuery_Err_Exit;
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(gcDatacenterAutonomics,"%.99s",field[0]);
			sprintf(gcQuery,"gcDatacenterAutonomics=%s",field[0]);
			logfileLine(gcQuery);
		}
		mysql_free_result(res);
	}


	return(0);

}//int DatacenterAutonomics(void)
