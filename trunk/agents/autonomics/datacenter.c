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
	}

	return(0);

}//int DatacenterAutonomics(void)
