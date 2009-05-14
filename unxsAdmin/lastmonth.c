/*
FILE	lastmonth.c 
	$Id: lastmonth.c 668 2007-07-02 13:50:59Z root $
AUTHOR	(C) 2002-2009, support @ openisp.net. GPL licensed.
PURPOSE
	Return last month with correct year and with wraparound
PROBLEMS
	Depends on setlocale()?
*/


#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

main()
{
	char cPrevYear[8]={""};
	char cThisYear[8]={""};
	char cThisMonth[4]={""};
	char *cPrevMonth="Error";
	unsigned uYear=0;
	time_t clock;
	struct tm *structTime;

	time(&clock);
	structTime=localtime(&clock);
	strftime(cThisYear,8,"%Y",structTime);
	strftime(cThisMonth,4,"%b",structTime);

	//printf("%s %s\n",cThisMonth,cThisYear);

	strcpy(cPrevYear,cThisYear);

	if(!strcmp(cThisMonth,"Jan"))
	{
		cPrevMonth="Dec";
		sscanf(cThisYear,"%u",&uYear);
		uYear--;
		sprintf(cPrevYear,"%u",uYear);
	}
	else if(!strcmp(cThisMonth,"Feb"))
	{
		cPrevMonth="Jan";
	}
	else if(!strcmp(cThisMonth,"Mar"))
	{
		cPrevMonth="Feb";
	}
	else if(!strcmp(cThisMonth,"Apr"))
	{
		cPrevMonth="Mar";
	}
	else if(!strcmp(cThisMonth,"May"))
	{
		cPrevMonth="Apr";
	}
	else if(!strcmp(cThisMonth,"Jun"))
	{
		cPrevMonth="May";
	}
	else if(!strcmp(cThisMonth,"Jul"))
	{
		cPrevMonth="Jun";
	}
	else if(!strcmp(cThisMonth,"Aug"))
	{
		cPrevMonth="Jul";
	}
	else if(!strcmp(cThisMonth,"Sep"))
	{
		cPrevMonth="Aug";
	}
	else if(!strcmp(cThisMonth,"Oct"))
	{
		cPrevMonth="Sep";
	}
	else if(!strcmp(cThisMonth,"Nov"))
	{
		cPrevMonth="Oct";
	}
	else if(!strcmp(cThisMonth,"Dec"))
	{
		cPrevMonth="Nov";
	}

	printf("%s %s\n",cPrevMonth,cPrevYear);
	exit(0);

}//Main()
