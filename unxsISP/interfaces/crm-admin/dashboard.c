/*
FILE
	dashboard.c
	svn ID removed
PURPOSE
	Provide the Dashboard tab functions
	of the ispCRM interface.
AUTHOR
	(C) 2007-2009 Hugo Urquiza for Unixservice.
*/

#include "interface.h"

void htmlDashBoardPage(char *cTitle, char *cTemplateName);
void ShowInvoiceStats(void);


void htmlDashBoard(void)
{
	htmlHeader("unxsISP System","Header");
	htmlDashBoardPage("","DashBoard.Body");
	htmlFooter("Footer");

}//void htmlDashBoard(void)


void htmlDashBoardPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect(cTemplateName,guTemplateSet);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="ispCRM.cgi";
			
			template.cpName[2]="cMessage";
			template.cpValue[2]=gcMessage;

			template.cpName[3]="gcLogin";
			template.cpValue[3]=gcLogin;

			template.cpName[4]="gcName";
			template.cpValue[4]=gcName;

			template.cpName[5]="gcOrgName";
			template.cpValue[5]=gcOrgName;

			template.cpName[6]="cUserLevel";
			template.cpValue[6]=(char *)cUserLevel(guPermLevel);

			template.cpName[7]="gcHost";
			template.cpValue[7]=gcHost;
			
			template.cpName[8]="";

			printf("\n<!-- Start htmlDashBoardPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlDashBoardPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void void htmlDashBoardPage(char *cTitle, char *cTemplateName)


void funcDisplayDashBoard(FILE *fp)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	time_t luClock;
	time_t luClock2;
	unsigned uCreatedBy=0;
	unsigned uScheduleDate=0;
	char cScheduleDate[100]={""};
	//customer tickets
	OpenRow("Assigned To You Tickets (Last 10)","black");
	fprintf(fp,"</tr><tr bgcolor=#e9e9e9><td bgcolor=ffffff></td><td><b>Created Date</b></td><td><b>Request/Issue</b></td>"
		"<td><b>Customer</b></td><td><b>Schedule Date</b></td></tr>\n");
	sprintf(gcQuery,"SELECT uCreatedBy,uScheduleDate,cText,uCreatedDate FROM tTicket "
			"WHERE uOwner=%u AND uTicketOwner=%u ORDER BY uCreatedDate DESC LIMIT 10",
			guOrg
			,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s\n",mysql_error(&gMysql));
		exit(1);
	}
	mysqlRes=mysql_store_result(&gMysql);
	fprintf(fp,"</td></tr>\n");
	while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[3],"%lu",&luClock);
		sscanf(mysqlField[0],"%u",&uCreatedBy);
		sscanf(mysqlField[1],"%u",&uScheduleDate);
		if(uScheduleDate)
			sprintf(cScheduleDate,"%.99s",ctime(&luClock2));
		else
			sprintf(cScheduleDate,"---");

		fprintf(fp,"<td></td><td>%s</td><td>%s ...<a href=# class=darkLink>(More)</td><td>%s</td><td>%s</td></tr>\n",
			ctime(&luClock)
			,cShortenText(mysqlField[2],5)
			,ForeignKey("tClient","cLabel",uCreatedBy)
			,cScheduleDate);
	}
	mysql_free_result(mysqlRes);

	OpenRow("Not Assigned Tickets (Last 10)","black");
	sprintf(gcQuery,"SELECT uCreatedBy,uScheduleDate,cText,uCreatedDate FROM tTicket "
			"WHERE (uOwner=%u OR uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u)) AND uTicketOwner=0 ORDER BY uCreatedDate DESC LIMIT 10",
			guOrg
			,guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s\n",mysql_error(&gMysql));
		exit(1);
	}
	mysqlRes=mysql_store_result(&gMysql);
	fprintf(fp,"</td></tr>\n");
	while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[3],"%lu",&luClock);
		sscanf(mysqlField[0],"%u",&uCreatedBy);
		sscanf(mysqlField[1],"%u",&uScheduleDate);
		if(uScheduleDate)
			sprintf(cScheduleDate,"%.99s",ctime(&luClock2));
		else
			sprintf(cScheduleDate,"---");
		fprintf(fp,"<td></td><td>%s</td><td>%s ...<a href=# class=darkLink>(More)</a></td><td>%s</td><td>%s</td></tr>\n",
			ctime(&luClock)
			,cShortenText(mysqlField[2],5)
			,ForeignKey("tClient","cLabel",uCreatedBy)
			,cScheduleDate);
	}
	mysql_free_result(mysqlRes);

	//login/logout activity
	OpenRow("Login Activity (Last 20)","black");
	sprintf(gcQuery,"SELECT tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),tLog.cServer,"
			"tLog.cHost,tLogType.cLabel FROM tLog,tLogType WHERE tLog.uLogType=tLogType.uLogType "
			"AND tLog.uLogType=8 ORDER BY GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC LIMIT 20");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s\n",mysql_error(&gMysql));
		exit(1);
	}
        mysqlRes=mysql_store_result(&gMysql);
	fprintf(fp,"</td></tr>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[1],"%lu",&luClock);
		fprintf(fp,"<td></td><td>%s</td><td>%s %s</td><td>%s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[4],mysqlField[2],mysqlField[3]);
	}
	mysql_free_result(mysqlRes);


}//void funcDisplayDashBoard(FILE *fp)


void OpenRow(const char *cFieldLabel, const char *cColor)
{
	printf("<tr><td width=20%% valign=top><a class=inputLink href=\"#\" onClick=\"javascript:window.open"
		"('?gcPage=Glossary&cLabel=%s','Glossary','height=600,width=500,status=yes,toolbar=no,menubar=no,"
		"location=no,scrollbars=1')\"><strong>%s</strong></a></td><td>",cFieldLabel,cFieldLabel);
}


void ShowInvoiceStats(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPendingInvoice=0;
	unsigned uPaidInvoice=0;

	char mTotalDue[32]={"0.00"};
	char mTotalPaid[32]={"0.00"};

	sprintf(gcQuery,"SELECT COUNT(uInvoice) FROM tInvoice WHERE uInvoiceStatus=1");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uPendingInvoice);
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT COUNT(uInvoice) FROM tInvoice WHERE uInvoiceStatus=2");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uPaidInvoice);
	
	sprintf(gcQuery,"SELECT SUM(mTotal) FROM tInvoice WHERE uInvoiceStatus=1");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(field[0]!=NULL)
			sprintf(mTotalDue,"%.31s",field[0]);
	}
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT SUM(mTotal) FROM tInvoice WHERE uInvoiceStatus=2");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(field[0]!=NULL)
			sprintf(mTotalPaid,"%.31s",field[0]);
	}
	mysql_free_result(res);


	printf("<tr><td></td><td>%u</td><td>%s</td><td>%u</td><td>%s</td></tr>\n",
		uPendingInvoice
		,mTotalDue
		,uPaidInvoice
		,mTotalPaid
		);
}//void ShowInvoiceStats(void)

