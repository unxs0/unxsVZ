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


void htmlPrintJobOffers(unsigned uYear,unsigned uMonth, unsigned uDay, unsigned uVendor);
void htmlPrintJobOffers(unsigned uYear,unsigned uMonth, unsigned uDay, unsigned uVendor)
{
        MYSQL_RES *res;
	MYSQL_ROW field;
	char cBg[6][16]={"bg-info","bg-success","bg-warning","bg-danger","bg-dark","bg-secondary"};
	if(uYear && uMonth && uDay && uVendor)
	{
		sprintf(gcQuery,"SELECT tJobOffer.uJobOffer,tJobOffer.cLabel FROM tCalendar,tJobOffer"
				" WHERE tCalendar.uJobOffer=tJobOffer.uJobOffer"
				" AND tCalendar.uVendor=%u AND tCalendar.dDate='%u-%u-%u'",uVendor,uYear,uMonth,uDay);
		mysql_query(&gMysql,gcQuery);
		printf("<font size=-2>");
		if(mysql_errno(&gMysql))
		{
			printf("<a class='event d-block p-1 pl-2 pr-2 mb-1 text-truncate small bg-danger text-white'"
				">%s</a>\n",mysql_error(&gMysql));
			return;
		}
		res=mysql_store_result(&gMysql);
		unsigned uCount=0;
		while((field=mysql_fetch_row(res)))
			printf("<a href='?gcPage=JobOffer&uJobOffer=%s'"
				" class='event d-block p-1 pl-2 pr-2 mb-1 rounded text-truncate %s text-white'"
				" title='%s'>%s</a>\n",field[0],cBg[(uCount++)%6],field[1],field[1]);
		mysql_free_result(res);
		printf("</font>");
	}
}//void htmlPrintJobOffers()


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
	uMonthFirstDayWeek++;

	//Start debug info
	printf("<!-- funcCalendar() guYear=%u guMonth=%u uPrevMonthLastDay=%u uMonthFirstDayWeek=%u -->\n",
				guYear,guMonth,uPrevMonthLastDay,uMonthFirstDayWeek);
	//Menu: We need this here so we can keep current month and prev/next buttons on top.

	printf("<nav class='navbar navbar-expand-md navbar-dark fixed-top bg-dark'>");

	printf("    <a class='navbar-brand' href='/%sApp/'>%s</a>",gcBrand,gcBrand);
	printf("    <button class='navbar-toggler' type='button' data-toggle='collapse' data-target='#navbarsCalendar' >");
	printf("        <span class='navbar-toggler-icon'></span>");
	printf("    </button>");

	printf("    <div class='collapse navbar-collapse' id='navbarsCalendar'>");
	printf("      <ul class='navbar-nav mr-auto'>");
	printf("        <li class='nav-item'>");
	printf("          <a class='nav-link' href='/%sApp/?gcPage=JobOffer'>Jobs</a>",gcBrand);
	printf("        </li>");
	printf("        <li class='nav-item active'>");
	printf("          <a class='nav-link' href='/%sApp/?gcPage=Calendar'>Calendar</a>",gcBrand);
	printf("        </li>");
	printf("        <li class='nav-item'>");
	printf("          <a class='nav-link' href='/%sApp/?gcPage=User'>User</a>",gcBrand);
	printf("        </li>");
	printf("        <li class='nav-item'>");
	printf("          <a class='nav-link' href='/%sApp/?gcPage=Calendar&uMonth=%u&uYear=%u'>-</a>",gcBrand,uPrevMonth,uPrevYear);
	printf("        </li>");
	printf("        <li class='nav-item'>");
	printf("          <a class='nav-link'>%s %u</a>",cMonth,guYear);
	printf("        </li>");
	printf("        <li class='nav-item'>");
	printf("          <a class='nav-link' href='/%sApp/?gcPage=Calendar&uMonth=%u&uYear=%u'>+</a>",gcBrand,uNextMonth,uNextYear);
	printf("        </li>");
	printf("      </ul>");
	printf("    </div>");

	printf("</nav>");

	//Header
	//BS 4: Html page must have <meta name='viewport' content='width=device-width, initial-scale=1'>
	printf("<div class='container-fluid'>\n");
	printf("<header>\n");
	//printf("  <h4 class='display-4 mb-4 text-center'>"
	//		"<a href='?gcPage=Calendar&uMonth=%u&uYear=%u'><font size='-1'>%s</font></a>"
	//		" %s %u "
	//		"<a href='?gcPage=Calendar&uMonth=%u&uYear=%u'><font size='-1'>%s</font></a></h4>\n",
	//			uPrevMonth,uPrevYear,cPrevMonth,
	//			cMonth,guYear,
	//			uNextMonth,uNextYear,cNextMonth);
	printf("<h4 class='display-4 mb-4 text-center'>&nbsp;</h4>");
	printf("<div class='row d-none d-sm-flex p-1 bg-dark text-white'>\n");
	printf("    <h5 class='col-sm p-1 text-center'>Sunday</h5>\n");
	printf("    <h5 class='col-sm p-1 text-center'>Monday</h5>\n");
	printf("    <h5 class='col-sm p-1 text-center'>Tuesday</h5>\n");
	printf("    <h5 class='col-sm p-1 text-center'>Wednesday</h5>\n");
	printf("    <h5 class='col-sm p-1 text-center'>Thursday</h5>\n");
	printf("    <h5 class='col-sm p-1 text-center'>Friday</h5>\n");
	printf("    <h5 class='col-sm p-1 text-center'>Saturday</h5>\n");
	printf("  </div>\n");
	printf("</header>\n");
	printf("<div class='row border border-right-0 border-bottom-0'>\n");

	char cWeekDay[7][4]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

	//uMonthFirstDayWeek=2 e.g. May 1st 2018 is Tue
	//uPrevMonthLastDay=30 e.g. April 30th 2018  DayWeek is 2-1 = 1 Mon
	unsigned uStartDay=uPrevMonthLastDay-uMonthFirstDayWeek+1;//e.g. 30 - 2 + 1= 29
	unsigned uCount=0;

	//Previous month lead into current month days
	for(uDay=uStartDay;uDay<=uPrevMonthLastDay;uDay++)
	{
		printf("<div class='day col-sm p-2 border border-left-0"
				" border-top-0 text-truncate d-none d-sm-inline-block bg-light text-muted'>\n");
		printf("  <h5 class='row align-items-center'>\n");
		printf("    <span class='date col-1'>%u</span>\n",uDay);
		printf("    <small class='col d-sm-none text-center text-muted'>%s %s %u</small>\n",cWeekDay[(uCount%7)],cMonth,guYear);
		printf("    <span class='col-1'></span>\n");
		printf("  </h5>\n");
		if(uDayOpenForBids(guYear,guMonth,uDay,guLoginClient))
			printf("<a class='event d-block p-1 pl-2 pr-2 mb-1 text-truncate small bg-danger text-white'"
				" title='Test Event 3'>Test Event 3</a>\n");
		else
			printf("<p class='d-sm-none'>No events</p>\n");
		printf("</div>\n");
		if((uCount%7)==6)
			printf("<div class='w-100'></div>\n");
		uCount++;
	}

	//Current month
	for(uDay=1;uDay<=uLastDay;uDay++)
	{
		//today
		if(uThisYear==guYear && uThisMonth==guMonth && uThisDay==uDay)
			printf("<div class='day col-sm p-2 border border-left-0 border-top-0 text-truncate'>\n");
		else
			printf("<div class='day col-sm p-2 border border-left-0 border-top-0 text-truncate'>\n");
		printf("  <h5 class='row align-items-center'>\n");
		printf("    <span class='date col-1'>%u</span>\n",uDay);
		printf("    <small class='col d-sm-none text-right text-muted'>%s %s %u</small>\n",cWeekDay[(uCount%7)],cMonth,guYear);
		printf("    <span class='col-1'></span>\n");
		printf("  </h5>\n");
		if(uDayOpenForBids(guYear,guMonth,uDay,guLoginClient))
			htmlPrintJobOffers(guYear,guMonth,uDay,guLoginClient);
		else
			printf("<p class='d-sm-none'>No events</p>\n");
		printf("</div>\n");
		if((uCount%7)==6)
			printf("<div class='w-100'></div>\n");
		uCount++;
	}
	
	//Next month lead out of current month days
	unsigned uLastDayOfWeek=(uCount%7);
	for(uDay=1;uDay<=(7-uLastDayOfWeek);uDay++)
	//for(uDay=1;uDay<=2;uDay++)
	{
		printf("<div class='day col-sm p-2 border border-left-0"
				" border-top-0 text-truncate d-none d-sm-inline-block bg-light text-muted'>\n");
		printf("  <h5 class='row align-items-center'>\n");
		printf("    <span class='date col-1'>%u</span>\n",uDay);
		printf("    <small class='col d-sm-none text-center text-muted'>%s %s %u</small>\n",cWeekDay[(uCount%7)],cMonth,guYear);
		printf("    <span class='col-1'></span>\n");
		printf("  </h5>\n");
		if(uDayOpenForBids(guYear,guMonth,uDay,guLoginClient))
			printf("<a class='event d-block p-1 pl-2 pr-2 mb-1 text-truncate small bg-danger text-white'"
				" title='Test Event 3'>Test Event 3</a>\n");
		else
			printf("<p class='d-sm-none'>No events</p>\n");
		printf("</div>\n");
		if((uCount%7)==6)
			printf("<div class='w-100'></div>\n");
		uCount++;
	}
	printf("</div>\n");
	printf("<!-- end of funcCalendar -->\n");

}//void funcCalendar()
