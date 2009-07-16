/*
FILE
	node.c
	$Id$
AUTHOR
	(C) 2009, Gary Wallis for Unixservice USA
PURPOSE
	unxsvz-autonomics node rule calculation and actions.
NOTES
*/

#include "autonomics.h"


int NodeAutonomics(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	if(!gcNodeWarnEmail[0] && guNode)
	{
		//TODO define 2 type node
		sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
			" cName='Warning email' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine((char *)mysql_error(&gMysql));
			sighandlerLeave(400);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(gcNodeWarnEmail,"%.99s",field[0]);
			sprintf(gcQuery,"gcNodeWarnEmail=%s",field[0]);
			logfileLine(gcQuery);
		}
		mysql_free_result(res);
	}

	return(0);

}//int NodeAutonomics(void)
