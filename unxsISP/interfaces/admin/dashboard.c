/*
FILE
	dashboard.c
	svn ID removed
PURPOSE
	Provide the Dashboard tab functions
	of the ispAdmin interface.
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

		TemplateSelect(cTemplateName);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="ispAdmin.cgi";
			
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
	unsigned uCount=0;

	OpenRow("System Messages (Last 20)","black");
	sprintf(gcQuery,"SELECT cMessage,GREATEST(uCreatedDate,uModDate),cServer FROM tLog "
			"WHERE uLogType=4 ORDER BY GREATEST(uCreatedDate,uModDate) DESC LIMIT 20");
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
		fprintf(fp,"<td></td><td>%s</td><td colspan=2>%s...<a href=# title='%s' class=darkLink>(More)</a></td><td>%s</td></tr>\n",
			ctime(&luClock),cShortenText(mysqlField[0],10),mysqlField[0],mysqlField[2]);
	}
	mysql_free_result(mysqlRes);

	//1-3 backend org admin interfaces
	OpenRow("General Usage (Last 20)","black");
	sprintf(gcQuery,"SELECT tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),tLog.cLogin,"
			"tLog.cTableName,tLog.cHost,tLogType.cLabel FROM tLog,tLogType WHERE "
			"tLog.uLogType=tLogType.uLogType AND tLog.uLogType<=3 ORDER BY "
			"GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC LIMIT 20");
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
		fprintf(fp,"<td></td><td>%s</td><td>%s %s</td><td>%s %s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[3],mysqlField[2],mysqlField[5],mysqlField[4]);
	}
	mysql_free_result(mysqlRes);
	
	//Deployed products top-ten
	OpenRow("Top 10 Deployed Products","black");
	sprintf(gcQuery,"SELECT (SELECT cLabel FROM tProduct WHERE tProduct.uProduct=tInstance.uProduct),"
			"COUNT(uProduct) AS uHowMany FROM tInstance WHERE uStatus=4 GROUP BY uProduct "
			"ORDER BY uHowMany DESC LIMIT 10");
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
		uCount++;
		fprintf(fp,"<td></td><td>%u</td><td>%s</td><td>%s</td></tr>\n",
			uCount,mysqlField[0],mysqlField[1]);
	}
	mysql_free_result(mysqlRes);

	//Invoice stats
	OpenRow("Month Invoice Statistics","black");
	fprintf(fp,"</tr><tr bgcolor=#e9e9e9><td bgcolor=ffffff></td><td><b>Total Pending Invoices</b></td><td><b>Total Due Amount</b></td>"
		"<td><b>Total Paid Invoices</b></td><td><b>Total Paid Amount</b></td></tr>\n");
	ShowInvoiceStats();

	//login/logout activity
	OpenRow("Login Activity (Last 20)","black");
	sprintf(gcQuery,"SELECT tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),tLog.cServer,"
			"tLog.cHost,tLogType.cLabel FROM tLog,tLogType WHERE tLog.uLogType=tLogType.uLogType "
			"AND tLog.uLogType>=6 ORDER BY GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC LIMIT 20");
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

	OpenRow("Pending Jobs (Last 20)","black");
	sprintf(gcQuery,"SELECT tJob.cJobName,GREATEST(tJob.uCreatedDate,tJob.uModDate),tClient.cLabel,"
			"cServer FROM tJob,tClient WHERE tJob.uJobClient=tClient.uClient AND tJob.uJobStatus=%u "
			"ORDER BY GREATEST(tJob.uCreatedDate,tJob.uModDate) DESC LIMIT 20",mysqlISP_Waiting);
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
		fprintf(fp,"<td></td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[2],mysqlField[3]);
	}
	mysql_free_result(mysqlRes);

}//void funcDisplayDashBoard(FILE *fp)


void OpenRow(const char *cFieldLabel, const char *cColor)
{
	printf("<tr><td width=20%% valign=top><a class=inputLink href=\"#\" onClick=\"javascript:window.open"
		"('?gcPage=Glossary&cLabel=%.32s','Glossary','height=600,width=500,status=yes,toolbar=no,menubar=no,"
		"location=no,scrollbars=1')\"><strong>%.32s</strong></a></td><td>",cFieldLabel,cFieldLabel);
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

