/*
FILE 
	{{cProject}}/interfaces/calendar.c
	template/new/bootstrap/calendar.c
AUTHOR/LEGAL
	(C) 2010-2018 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	Aux code for generating bootstrap calendar
REQUIRES
	calendargrid.css
*/

#include "interface.h"

void funcCalendar(FILE *fp)
{

	register unsigned uDay;
	unsigned uThisDay=0;
	unsigned uThisMonth=0;
	unsigned uThisYear=0;
	unsigned uLastDay=0;
	unsigned uMonthFirstDayWeek=0;
	unsigned uPrevMonthLastDay=0;
	char cMonth[32]={""};
	char cPrevMonth[32]={""};
	char cNextMonth[32]={""};
	unsigned uPrevMonth=0;
	unsigned uNextMonth=0;
	unsigned uPrevYear=0;
	unsigned uNextYear=0;

	//Get last day and month name and next and prev data
        MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT DAY(LAST_DAY('%1$u-%2$u-10')),"
				"MONTHNAME('%1$u-%2$u-10'),"
				"MONTHNAME(DATE_SUB('%1$u-%2$u-10',INTERVAL 1 MONTH)),"
				"MONTHNAME(DATE_ADD('%1$u-%2$u-10',INTERVAL 1 MONTH)),"
				"MONTH(DATE_SUB('%1$u-%2$u-10',INTERVAL 1 MONTH)),"
				"MONTH(DATE_ADD('%1$u-%2$u-10',INTERVAL 1 MONTH)),"
				"YEAR(DATE_SUB('%1$u-%2$u-10',INTERVAL 1 MONTH)),"
				"YEAR(DATE_ADD('%1$u-%2$u-10',INTERVAL 1 MONTH)),"
				"DAY(NOW()),"
				"MONTH(NOW()),"
				"YEAR(NOW()),"
				"WEEKDAY('%1$u-%2$u-1'),"
				"DAY(LAST_DAY(DATE_SUB('%1$u-%2$u-10',INTERVAL 1 MONTH)))"
					,guYear,guMonth);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uLastDay);
		sprintf(cMonth,"%.31s",field[1]);
		sprintf(cPrevMonth,"%.31s",field[2]);
		sprintf(cNextMonth,"%.31s",field[3]);
		sscanf(field[4],"%u",&uPrevMonth);
		sscanf(field[5],"%u",&uNextMonth);
		sscanf(field[6],"%u",&uPrevYear);
		sscanf(field[7],"%u",&uNextYear);
		sscanf(field[8],"%u",&uThisDay);
		sscanf(field[9],"%u",&uThisMonth);
		sscanf(field[10],"%u",&uThisYear);
		sscanf(field[11],"%u",&uMonthFirstDayWeek);
		sscanf(field[12],"%u",&uPrevMonthLastDay);
	}
	mysql_free_result(res);

	printf("<div id=\"calendar\">\n");
	printf("  <div class=\"text-center\">\n");
	printf("    <a class=\"month-selector\" href=\"?gcFunction=Calendar&uYear=%u&uMonth=%u\">%s %u</a>\n",
			uPrevYear,uPrevMonth,cPrevMonth,uPrevYear);
	printf("    <h2 class=\"month-header\">%s %u</h2>\n",cMonth,guYear);
	printf("    <a class=\"month-selector\" href=\"?gcFunction=Calendar&uYear=%u&uMonth=%u\">%s %u</a>\n",
			uNextYear,uNextMonth,cNextMonth,uNextYear);
	printf("  </div>\n");
	printf("  <div id=\"cal-wrapper\">\n");

//<div class="date future col-xs-12 col-sm-6 col-md-3 sevencols col-lg-2">
//  <div class="date-header">
//    <h4>2 Sat</h4>
//  </div>
//
//  <div class="date-item none">
//    <b data="no-items" class="no-items">No Events Scheduled</b>
//  </div>
//</div>
//
	char cWeekDay[7][4]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

	uMonthFirstDayWeek++;
	//uMonthFirstDayWeek=2 e.g. May 1st 2018 is Tue
	//uPrevMonthLastDay=30 e.g. April 30th 2018  DayWeek is 2-1 = 1 Mon
	unsigned uStartDay=uPrevMonthLastDay-uMonthFirstDayWeek+1;//e.g. 30 - 2 + 1= 29
	unsigned uCount=0;
	for(uDay=uStartDay;uDay<=uPrevMonthLastDay;uDay++)
	{
		printf("    <div class=\"date yesterday col-xs-12 col-sm-6 col-md-3 sevencols col-lg-2\">\n");
		printf("      <div class=\"date-header\">\n");
		printf("        <h4>%u %s (%u %u)</h4>\n",uDay,cWeekDay[(uCount%7)],uPrevMonthLastDay,uMonthFirstDayWeek);
		printf("      </div>\n");
		printf("      <div class=\"date-item none\">\n");
		printf("        <b data=\"no-items\" class=\"no-items\">No Events Scheduled</b>\n");
		printf("      </div>\n");
		printf("    </div>\n");
		uCount++;
	}

	for(uDay=1;uDay<=uLastDay;uDay++)
	{
		//today
		if(uThisYear==guYear && uThisMonth==guMonth && uThisDay==uDay)
			printf("    <div class=\"date today col-xs-12 col-sm-6 col-md-3 sevencols col-lg-2\">\n");
		else
			printf("    <div class=\"date future col-xs-12 col-sm-6 col-md-3 sevencols col-lg-2\">\n");
		printf("      <div class=\"date-header\">\n");
		printf("        <h4>%u %s</h4>\n",uDay,cWeekDay[(uCount%7)]);
		printf("      </div>\n");
		printf("      <div class=\"date-item none\">\n");
		printf("        <b data=\"no-items\" class=\"no-items\">No Events Scheduled</b>\n");
		printf("      </div>\n");
		printf("    </div>\n");
		uCount++;
	}
	printf("  </div>\n");
	printf("</div>\n");


}//void funcCalendar()
