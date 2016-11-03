/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template and mysqlRAD2 author: (C) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details

 
*/

//Protos
time_t cDateToUnixTime(char *cDate);
void htmlMonthYearSelectTable(void);
void htmlDateSelectTable(void);
void CreatetRadacctMonthTable(char *cTableName);

//Extern
char *strptime(char *buf, const char *format, const struct tm *tm);
void UserReport(char *cLogin, char *cTableName);
void TopUsersReport(char *cTableName);
void ErrorReport(char *cTableName);
void GlobalUseReport(char *cTableName);

//Data
time_t cDateToUnixTime(char *cDate);
static char cDay[3]={"1"};
static char cMonth[4]={"Jan"};
static char cYear[5]={"2002"};
static char cSearch[100]={""};

static unsigned uMonth=0;
static char cuMonthPullDown[33]={""};


void ExtProcesstRadacctMonthVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cDay") )
			sprintf(cDay,"%.2s",entries[i].val);
		else if(!strcmp(entries[i].name,"cMonth") )
			sprintf(cMonth,"%.3s",entries[i].val);
		else if(!strcmp(entries[i].name,"cYear") )
			sprintf(cYear,"%.4s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLogin") )
			sprintf(cLogin,"%.100s",entries[i].val);

		else if(!strcmp(entries[i].name,"cuMonthPullDown"))
		{
			strcpy(cuMonthPullDown,entries[i].val);
			uMonth=ReadPullDown("tMonth","cLabel",cuMonthPullDown);
		}

		else if(!strcmp(entries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.99s",entries[i].val);
		}

	}
}//void ExtProcesstRadacctMonthVars(pentry entries[], int x)


void ExttRadacctMonthCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tRadacctMonthTools"))
	{
		if(!strcmp(gcCommand,"Global Use Report"))
		{
			ExtProcesstRadacctMonthVars(entries,x);
			if(!uMonth)
				GlobalUseReport("tRadacctMonth");
			else
				GlobalUseReport(cuMonthPullDown);
		}
		else if(!strcmp(gcCommand,"Error Report"))
		{
			ExtProcesstRadacctMonthVars(entries,x);

			if(!uMonth)
				ErrorReport("tRadacctMonth");
			else
				ErrorReport(cuMonthPullDown);

		}
		else if(!strcmp(gcCommand,"User Report"))
		{
			ExtProcesstRadacctMonthVars(entries,x);

			if(!uMonth)
				strcpy(cuMonthPullDown,"tRadacctMonth");

			if(cLogin[0])
				UserReport(cLogin,cuMonthPullDown);
			else if(cSearch[0])
				UserReport(cSearch,cuMonthPullDown);
			else if(1)
				tRadacctMonth("Must specify cLogin");
		}
		else if(!strcmp(gcCommand,"Top Users Report"))
		{
			ExtProcesstRadacctMonthVars(entries,x);
			if(!uMonth)
				TopUsersReport("tRadacctMonth");
			else
				TopUsersReport(cuMonthPullDown);
		}
		else if(!strcmp(gcCommand,"UnLoad"))
		{
			sprintf(gcQuery,"DELETE FROM tRadacctMonth");
		        mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tRadacctMonth(mysql_error(&gMysql));
		}
		else if(!strcmp(gcCommand,"Load"))
		{
			ExtProcesstRadacctMonthVars(entries,x);

			if(!uMonth)
				tRadacctMonth("Must specify valid month table");

			sprintf(gcQuery,"DELETE FROM tRadacctMonth");
		        mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tRadacctMonth(mysql_error(&gMysql));

			sprintf(gcQuery,"INSERT DELAYED tRadacctMonth (uRadacct,cLogin,"
					"cSessionID,cNAS,uConnectTime,uPort,uPortType,"
					"uService,uProtocol,cIP,cLine,cCallerID,uInBytes,"
					"uOutBytes,uTermCause,uStartTime,uStopTime,cInfo) "
					"SELECT uRadacct,cLogin,cSessionID,cNAS,uConnectTime,"
					"uPort,uPortType,uService,uProtocol,cIP,cLine,cCallerID,"
					"uInBytes,uOutBytes,uTermCause,uStartTime,uStopTime,cInfo FROM %s",cuMonthPullDown);
		        mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tRadacctMonth(mysql_error(&gMysql));

		}

	}

}//void ExttRadacctMonthCommands(pentry entries[], int x)


void ExttRadacctMonthButtons(void)
{
	OpenFieldSet("tRadacctMonth Aux Panel",100);
	printf("<u>Table Tips</u><br>");
	printf("tRadacctMonth contains CLI archived tRadacct data organized by year and month. You can load any archived month into this table for browsing. Most reports do not require that the archived data be loaded, but that the correct archive be selected from the drop down below. See tRadacct table tips for more info on fields.");
	printf("<p><u>Archived Data Available</u><br>");
	tTablePullDown("tMonth;cuMonthPullDown","cLabel","cLabel",uMonth,1);

	printf("<p><u>Search Tool</u><br>");
	printf("You may use %% and standard SQL _ . wildcards for CLIKE matching of cLogin<br>\n");
	printf("<input title='Enter partial cLogin. You may use %% and _ wildcards. "
		"Drop down above applies if selection made' type=text name=cSearch value=%s>",cSearch);

	printf("<p><u>tRadacctMonth Table Ops</u><br>");
	printf("<input class=lalertButton title='Load compressed month data from pull "
		"down into this table for reports and other general queries' type=submit name=gcCommand value='Load'><br>");
	printf("<input class=lalertButton title='Remove data from tRadacctMonth' type=submit name=gcCommand value='UnLoad'>");


	printf("<p><u>Report Tools</u><br>");
	printf("<input title='Single user in-depth report. Pull down and search or cLogin apply'"
		" class=largeButton type=submit name=gcCommand value='User Report'><br>");
	printf("<input title='Current month top users. Pull down or if no selection made then "
		"tRadacctMonth table used' class=largeButton type=submit name=gcCommand value='Top Users Report'><br>");
	printf("<input title='Error or incorrect records report. Pull down or if no selection made "
		"then tRadacctMonth table used' class=largeButton type=submit name=gcCommand value='Error Report'><br>");
	printf("<input title='Global time used and number of distinct users. Pull down or if no selection made then "
		"tRadacctMonth table used' class=largeButton type=submit name=gcCommand value='Global Use Report'>");

	CloseFieldSet();
	
}//void ExttRadacctMonthButtons(void)


void ExttRadacctMonthAuxTable(void)
{

}//void ExttRadacctMonthAuxTable(void)


void ExttRadacctMonthGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uRadacct"))
		{
			sscanf(gentries[i].val,"%u",&uRadacct);
			guMode=6;
		}
	}
	tRadacctMonth("");

}//void ExttRadacctMonthGetHook(entry gentries[], int x)


void ExttRadacctMonthSelect(void)
{
	if(cSearch[0])
		ExtSelectSearch("tRadacctMonth",VAR_LIST_tRadacctMonth,"cLogin",cSearch,NULL,0);
	else
		ExtSelect("tRadacctMonth",VAR_LIST_tRadacctMonth,0);

}//void ExttRadacctMonthSelect(void)


void ExttRadacctMonthSelectRow(void)
{
	ExtSelectRow("tRadacctMonth",VAR_LIST_tRadacctMonth,uRadacct);

}//void ExttRadacctMonthSelectRow(void)


void ExttRadacctMonthListSelect(void)
{
	char cCat[512];

	ExtListSelect("tRadacctMonth",VAR_LIST_tRadacctMonth);

	//Changes here must be reflected below in ExttRadacctMonthListFilter()
        if(!strcmp(gcFilter,"uRadacct"))
        {
                sscanf(gcCommand,"%u",&uRadacct);
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tRadacctMonth.uRadacct=%u",uRadacct);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uRadacct");
        }

}//void ExttRadacctMonthListSelect(void)


void ExttRadacctMonthListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uRadacct"))
                printf("<option>uRadacct</option>");
        else
                printf("<option selected>uRadacct</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttRadacctMonthListFilter(void)


void ExttRadacctMonthNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttRadacctMonthNavBar(void)


void htmlMonthYearSelectTable(void)
{
	register int i;
	char cMonth[4]={""};
	char cYear[8]={""};
	struct tm *structTime;
	unsigned uThisYear=0;
	time_t clock;

	time(&clock);
	structTime=localtime(&clock);
	//strftime(cMonth,4,"%b",structTime);
	strftime(cYear,8,"%Y",structTime);
	sscanf(cYear,"%u",&uThisYear);

	printf("<table>\n");
	printf("<tr><td>Month</td><td>Year</td></tr>\n");

	printf("<tr>\n");

	printf("<td><select name=cMonth>\n");

	printf("<option");
	if(!strcmp(cMonth,"Jan"))
		printf(" selected>");
	else
		printf(">");
	printf("Jan</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Feb"))
		printf(" selected>");
	else
		printf(">");
	printf("Feb</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Mar"))
		printf(" selected>");
	else
		printf(">");
	printf("Mar</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Apr"))
		printf(" selected>");
	else
		printf(">");
	printf("Apr</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"May"))
		printf(" selected>");
	else
		printf(">");
	printf("May</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Jun"))
		printf(" selected>");
	else
		printf(">");
	printf("Jun</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Jul"))
		printf(" selected>");
	else
		printf(">");
	printf("Jul</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Aug"))
		printf(" selected>");
	else
		printf(">");
	printf("Aug</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Sep"))
		printf(" selected>");
	else
		printf(">");
	printf("Sep</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Oct"))
		printf(" selected>");
	else
		printf(">");
	printf("Oct</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Nov"))
		printf(" selected>");
	else
		printf(">");
	printf("Nov</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Dec"))
		printf(" selected>");
	else
		printf(">");
	printf("Dec</option>\n");

	printf("</select></td>\n");

	//Ten year window
	printf("<td><select name=cYear>\n");
	for(i=uThisYear;i>uThisYear-5;i--)
		printf("<option>%u</option>",i);
	printf("</select></td>\n");

	printf("</tr>\n");

	printf("</table>\n");

}//void htmlMonthYearSelectTable(void)


void htmlDateSelectTable(void)
{
	register int i;
	char cMonth[4]={""};
	char cYear[8]={""};
	struct tm *structTime;
	unsigned uThisYear=0;
	time_t clock;

	time(&clock);
	structTime=localtime(&clock);
	//strftime(cMonth,4,"%b",structTime);
	strftime(cYear,8,"%Y",structTime);
	sscanf(cYear,"%u",&uThisYear);

	printf("<table width=150>\n");
	printf("<tr><td>Day</td><td>Month</td><td>Year</td></tr>\n");

	printf("<tr>\n");

	printf("<td><font face=arial size=1><select name=cDay>\n");
	printf("<option>Now</option>");
	for(i=1;i<32;i++)
		printf("<option>%u</option>",i);
	printf("</select></td>\n");

	printf("<td><font size=1><select name=cMonth>\n");

	printf("<option");
	if(!strcmp(cMonth,"Jan"))
		printf(" selected>");
	else
		printf(">");
	printf("Jan</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Feb"))
		printf(" selected>");
	else
		printf(">");
	printf("Feb</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Mar"))
		printf(" selected>");
	else
		printf(">");
	printf("Mar</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Apr"))
		printf(" selected>");
	else
		printf(">");
	printf("Apr</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"May"))
		printf(" selected>");
	else
		printf(">");
	printf("May</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Jun"))
		printf(" selected>");
	else
		printf(">");
	printf("Jun</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Jul"))
		printf(" selected>");
	else
		printf(">");
	printf("Jul</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Aug"))
		printf(" selected>");
	else
		printf(">");
	printf("Aug</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Sep"))
		printf(" selected>");
	else
		printf(">");
	printf("Sep</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Oct"))
		printf(" selected>");
	else
		printf(">");
	printf("Oct</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Nov"))
		printf(" selected>");
	else
		printf(">");
	printf("Nov</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Dec"))
		printf(" selected>");
	else
		printf(">");
	printf("Dec</option>\n");

	printf("</select></td>\n");

	//5 year window
	printf("<td><font face=arial size=1><select name=cYear>\n");
	for(i=uThisYear;i>uThisYear-5;i--)
		printf("<option>%u</option>",i);
	printf("</select></td>\n");

	printf("</tr>\n");

	printf("</table>\n");

}//void htmlDateSelectTable(void)


time_t cDateToUnixTime(char *cDate)
{
        struct  tm locTime;
        time_t  res;

        bzero(&locTime, sizeof(struct tm));
	if(strchr(cDate,'-'))
        	strptime(cDate,"%d-%b-%Y", &locTime);
	else if(strchr(cDate,'/'))
        	strptime(cDate,"%d/%b/%Y", &locTime);
	else if(strchr(cDate,' '))
        	strptime(cDate,"%d %b %Y", &locTime);
        locTime.tm_sec = 0;
        locTime.tm_min = 0;
        locTime.tm_hour = 0;
        res = mktime(&locTime);
        return(res);
}//time_t cDateToUnixTime(char *cDate)


void CreatetRadacctMonthTable(char *cTableName)
{
	sprintf(gcQuery,"CREATE TABLE %s ( uPortType int unsigned NOT NULL DEFAULT 0, uPort int unsigned NOT NULL DEFAULT 0, cNAS varchar(15) NOT NULL DEFAULT '',index (cNAS), cLogin varchar(100) NOT NULL DEFAULT '',index (cLogin), uConnectTime bigint unsigned NOT NULL DEFAULT 0,index (uConnectTime), cSessionID varchar(32) NOT NULL DEFAULT '',index (cSessionID), uService int unsigned NOT NULL DEFAULT 0, uProtocol int unsigned NOT NULL DEFAULT 0, cIP varchar(15) NOT NULL DEFAULT '', cLine varchar(15) NOT NULL DEFAULT '', cCallerID varchar(15) NOT NULL DEFAULT '', uInBytes bigint unsigned NOT NULL DEFAULT 0, uOutBytes int unsigned NOT NULL DEFAULT 0, uTermCause int unsigned NOT NULL DEFAULT 0, uStopTime int unsigned NOT NULL DEFAULT 0,index (uStopTime), uStartTime int unsigned NOT NULL DEFAULT 0,index (uStartTime), cInfo varchar(20) NOT NULL DEFAULT '', uRadacct int unsigned NOT NULL DEFAULT 0 PRIMARY KEY)", cTableName);
	mysql_query(&gMysql,gcQuery);

}//CreatetRadacctMonthTable()

