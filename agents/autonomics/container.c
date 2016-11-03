/*
FILE
	container.c
	svn ID removed
AUTHOR
	(C) 2009, Gary Wallis for Unixservice, LLC.
PURPOSE
	unxsvz-autonomics container rule calculation and actions.
NOTES
*/

#include "autonomics.h"


int ContainerAutonomics(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	if(!guContainer) return(0);

	if(!gsAutoState.cContainerWarnEmail[0] && guContainer)
	{
		//TODO define 3 type container
		sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
			" cName='WarningEmail' AND tProperty.uKey=%u AND tProperty.uType=3",guContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ContainerAutonomics",mysql_error(&gMysql));
			sighandlerLeave(400);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(gsAutoState.cContainerWarnEmail,"%.99s",field[0]);
			sprintf(gcQuery,"gsAutoState.cContainerWarnEmail=%s",field[0]);
			logfileLine("ContainerAutonomics",gcQuery);
		}
		mysql_free_result(res);
	}

	return(0);

}//int ContainerAutonomics(void)
