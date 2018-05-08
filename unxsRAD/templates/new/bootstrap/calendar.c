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
	guYear
	guMonth
*/

#include "interface.h"

unsigned uDayOpenForBids(unsigned uYear,unsigned uMonth, unsigned uDay, unsigned uVendor)
{
        MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uReturn=0;

	if(uYear && uMonth && uDay && uVendor)
	{
		sprintf(gcQuery,"SELECT uCalendar FROM tCalendar"
				" WHERE (uVendor=%u OR uVendor=%u) AND dDate='%u-%u-%u'",uVendor,guOrg,uYear,uMonth,uDay);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			return(0);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			uReturn=1;
		mysql_free_result(res);
	}

	return(uReturn);

}//unsigned uDayOpenForBids(unsigned uYear,unsigned uMonth, unsigned uDay)


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

        MYSQL_RES *res;
	MYSQL_ROW field;

	if(!guYear || !guMonth)
	{
		guYear=2018;
		guMonth=5;

		sprintf(gcQuery,"SELECT MONTH(NOW()),YEAR(NOW())");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&guMonth);
			sscanf(field[1],"%u",&guYear);
		}
	}

	//Get last day and month name and next and prev data
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
//<div class="container">
// <div class="row">
//   <div class="twelve columns">
//       <h1>Calendar of Events</h1>
//
	uMonthFirstDayWeek++;
//<form class="form-signin" role="form" method="post" action="/{{cProject}}App/">
//</form>
	printf("<!-- funcCalendar() guYear=%u guMonth=%u uPrevMonthLastDay=%u uMonthFirstDayWeek=%u -->\n",
				guYear,guMonth,uPrevMonthLastDay,uMonthFirstDayWeek);
	printf("<div class=\"container\"><div class=\"row\"><div class=\"twelve columns\"><div id=\"calendar\">\n");
	printf("  <div class=\"text-center\">\n");
	printf("    <a class=\"month-selector\" href=\"?gcPage=Calendar&uYear=%u&uMonth=%u\">%s %u</a>\n",
			uPrevYear,uPrevMonth,cPrevMonth,uPrevYear);
	printf("    <h2 class=\"month-header\">%s %s %u</h2>\n",gcName,cMonth,guYear);
	printf("    <a class=\"month-selector\" href=\"?gcPage=Calendar&uYear=%u&uMonth=%u\">%s %u</a>\n",
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

	//uMonthFirstDayWeek=2 e.g. May 1st 2018 is Tue
	//uPrevMonthLastDay=30 e.g. April 30th 2018  DayWeek is 2-1 = 1 Mon
	unsigned uStartDay=uPrevMonthLastDay-uMonthFirstDayWeek+1;//e.g. 30 - 2 + 1= 29
	unsigned uCount=0;
	for(uDay=uStartDay;uDay<=uPrevMonthLastDay;uDay++)
	{
		printf("    <div class=\"date yesterday col-xs-12 col-sm-6 col-md-3 sevencols col-lg-2\">\n");
		printf("      <div class=\"date-header\">\n");
		printf("        <h4>%u %s</h4>\n",uDay,cWeekDay[(uCount%7)]);
		printf("      </div>\n");
		if(uDayOpenForBids(guYear,uPrevMonth,uDay,guLoginClient))
		{
			printf("      <div class=\"date-item\">\n");
			printf("        <b data=\"no-items\" class=\"no-items\">Available</b>\n");
		}
		else
		{
			printf("      <div class=\"date-item none\">\n");
			printf("        <b data=\"no-items\" class=\"no-items\">X</b>\n");
		}
		printf("      </div>\n");
		printf("    </div>\n");
		uCount++;
	}

//<div class="date-item">
//  <b>12:00 am</b><br />
//  <a href="?gcPage=Calendar&uJobOffer=85794">Major Tear Switchblade 2016</a>
//</div>
//
	for(uDay=1;uDay<=uLastDay;uDay++)
	{
		//today
		if(uThisYear==guYear && uThisMonth==guMonth && uThisDay==uDay)
			printf("    <div class=\"date today col-xs-12 col-sm-6 col-md-3 sevencols col-lg-2\">\n");
		else
			printf("    <div class=\"date future col-xs-12 col-sm-6 col-md-3 sevencols col-lg-2\">\n");
		printf("      <div class=\"date-header\">\n");
		//printf("<h4>%u %s <div class='text-right'>"
		printf("<h4>%u %s</h4> "
				"<form method='post' action='/{{cProject}}App/'>"
				"<input type=hidden name=guMonth value=%u>"
				"<input type=hidden name=guYear value=%u>"
				"<input type=hidden name=gcPage value=Calendar>"
				"<input type=hidden name=uDay value=%u>"
				"<div class='text-right'><button type=submit name=gcFunction value=ToggleDay class='btn btn-link'>"
					"    <span class='glyphicon glyphicon-retweet'></span>"
				"</button></div>"
				"</form>",
			//"</div></h4>\n",
			//"</h4>\n",
					uDay,cWeekDay[(uCount%7)],
						guMonth,guYear,uDay);
		printf("      </div>\n");
		if(uDayOpenForBids(guYear,guMonth,uDay,guLoginClient))
		{
			printf("      <div class=\"date-item\">\n");
			printf("        <b data=\"no-items\" class=\"no-items\">Available</b>\n");
		}
		else
		{
			printf("      <div class=\"date-item\">\n");
			printf("        <b data=\"no-items\" class=\"no-items\">X</b>\n");
		}
		printf("      </div>\n");
		printf("    </div>\n");
		uCount++;
	}
	printf("  </div>\n");
	printf("</div></div></div></div>\n");
	printf("<!-- end of funcCalendar -->\n");//used by get style buttons


}//void funcCalendar()
