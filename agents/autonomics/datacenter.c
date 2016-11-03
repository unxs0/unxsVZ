/*
FILE
	datacenter.c
	svn ID removed
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
			logfileLine("DatacenterAutonomics",mysql_error(&gMysql));
			sighandlerLeave(400);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&guDatacenter);
			sscanf(field[1],"%u",&guNode);
			sprintf(gcQuery,"guDatacenter=%u guNode=%u",guDatacenter,guNode);
			logfileLine("DatacenterAutonomics",gcQuery);
		}
		mysql_free_result(res);
	}

	if(!gsAutoState.cDatacenterWarnEmail[0] && guDatacenter)
	{
		//TODO define 1 type datacenter
		sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
			" cName='WarningEmail' AND tProperty.uKey=%u AND tProperty.uType=1",guDatacenter);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("DatacenterAutonomics",mysql_error(&gMysql));
			sighandlerLeave(400);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(gsAutoState.cDatacenterWarnEmail,"%.99s",field[0]);
			sprintf(gcQuery,"cDatacenterWarnEmail=%s",field[0]);
			logfileLine("DatacenterAutonomics",gcQuery);
		}
		mysql_free_result(res);
	}

	if(!gsAutoState.cDatacenterAutonomics[0])
	{
		//TODO define 1 type datacenter
		sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
			" cName='Autonomics' AND tProperty.uKey=%u AND tProperty.uType=1",guDatacenter);
		mysqlQuery_Err_Exit;
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(gsAutoState.cDatacenterAutonomics,"%.99s",field[0]);
			sprintf(gcQuery,"cDatacenterAutonomics=%s",field[0]);
			logfileLine("DatacenterAutonomics",gcQuery);
		}
		mysql_free_result(res);
	}


	return(0);

}//int DatacenterAutonomics(void)
