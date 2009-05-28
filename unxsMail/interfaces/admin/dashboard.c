/*
FILE
	dashboard.c
PURPOSE
	Provide the functions for the mail2Admin
	interface dashboard
AUTHOR
	(C) Hugo Urquiza 2008-2009 for Unixservice.
*/

#include "interface.h"

unsigned uConnTest(char *cHost,unsigned uPort);
void init_sockaddr (struct sockaddr_in *name,const char *hostname,uint16_t port);


void htmlDashBoard(void)
{
	htmlHeader("unxsMail System","Header");
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
			template.cpValue[1]="mail2Admin.cgi";
			
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


void funcDashBoard(FILE *fp)
{
	ServerStatus();
	JobQueueStatus();	
	MailGraphImages();

}//void funcDashBoard(FILE *fp)


void JobQueueStatus(void)
{
	printf("<fieldset><legend><b>Job Queue Overall Status</b></legend>\n<table><tr><td>\n");
	funcJobInfo(stdout,"%");
	printf("</td></tr></table></fieldset>\n<br>\n");
}//void JobQueueStatus(void)


void MailGraphImages(void)
{
	char cShowMailgraph[10]={""};
	char cLastDay0[100]={"/images/mailgraph-lastday0.png"};
	char cLastDay1[100]={"/images/mailgraph-lastday1.png"};
	char cLastWeek0[100]={"/images/mailgraph-lastweek0.png"};
	char cLastWeek1[100]={"/images/mailgraph-lastweek1.png"};

	GetConfiguration("cShowMailgraph",cShowMailgraph,0,1);

	if(strcmp(cShowMailgraph,"yes")) return;

	//Get image paths
	GetConfiguration("cLastDay0",cLastDay0,0,1);
	GetConfiguration("cLastDay1",cLastDay1,0,1);

	GetConfiguration("cLastWeek0",cLastWeek0,0,1);
	GetConfiguration("cLastWeek1",cLastWeek1,0,1);

	printf("<fieldset><legend><b>Mail System Graphs</b></legend>\n"
		"<table>\n"
		"<tr><td>Last Day</td></tr>\n"
		);
	printf("<tr><td><img src='%s'></td></tr>\n",cLastDay0);
	printf("<tr><td><img src='%s'></td></tr>\n",cLastDay1);

	printf("<tr><td>Last Week</td></tr>\n");
	printf("<tr><td><img src='%s'></td></tr>\n",cLastWeek0);
	printf("<tr><td><img src='%s'></td></tr>\n",cLastWeek1);
	printf("</table>\n</fieldset>\n");

}//void MailGraphImages(void)


void ServerStatus(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uI=0;

	printf("<fieldset><legend><b>Server Status</b></legend>\n<table>\n\n");

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT cValue,cComment FROM tConfiguration WHERE cLabel LIKE 'cMailServer%%' ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT cValue,cComment FROM tConfiguration," TCLIENT
				" WHERE tConfiguration.uOwner=" TCLIENT ".uClient"
				" AND (" TCLIENT ".uClient=%1$u OR " TCLIENT ".uOwner"
				" IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%1$u OR uClient=%1$u))"
				" AND tConfiguration.cLabel LIKE 'cMailServer%%' "
				" ORDER BY tConfiguration.cLabel",
					guCompany);
	macro_mySQLRunAndStore(res);

	if(mysql_num_rows(res))
	{
		while((field=mysql_fetch_row(res)))
		{
			if(strstr(field[1],"SMTP"))
			{
				printf("<tr><td>\n");
				uI=uConnTest(field[0],25);
				if(uI && uI<3)
					printf("<font color=red><blink><b>Alert!</b></blink> %s SMTP server is down (Status code:%u)</font><br>\n",
						field[0],uI);
				else if(uI>=3)
					printf("<font color=yellow><b>Warning</b> %s SMTP server didn't answer in time (Status code:%u)</font><br>\n",
						field[0],uI);
				else
					printf("<font color=green>%s SMTP server is up</font><br>\n",field[0]);
				printf("</td></tr>\n");
			}

			if(strstr(field[1],"POP"))
			{
				printf("<tr><td>\n");
				uI=uConnTest(field[0],110);
				if(uI && uI<3)
					printf("<font color=red><blink><b>Alert!</b></blink> %s POP3 server is down (Status code:%u)</font><br>\n",
						field[0],uI);
				else if(uI>=3)
					printf("<font color=yellow><b>Warning</b> %s POP3 server didn't answer in time (Status code:%u)</font><br>\n",
						field[0],uI);
				else
					 printf("<font color=green>%s POP3 server is up</font><br>\n",field[0]);
				printf("</td></tr>\n");
			}
			if(strstr(field[1],"IMAP"))
			{
				printf("<tr><td>\n");
				uI=uConnTest(field[0],110);
				if(uI && uI<3)
					printf("<font color=red><blink><b>Alert!</b></blink> %s IMAP server is down (Status code:%u)</font><br>\n",
						field[0],uI);
				else if(uI>=3)
					printf("<font color=yellow><b>Warning</b> %s IMAP server didn't answer in time (Status code:%u)</font><br>\n",
						field[0],uI);
				else
					printf("<font color=green>%s IMAP server is up</font><br>\n",field[0]);
				printf("</td></tr>\n");
			}

		}
	}
	else
		printf("<tr><td>No mail servers configured to be monitored</td></tr>\n");
	printf("</table></fieldset>\n<br>\n");

	mysql_free_result(res);

	
}//void ServerStatus(void)


unsigned uConnTest(char *cHost,unsigned uPort)
{
	int iSocket;
	struct sockaddr_in sAddrInfo;
	char cRead[101]={""};
	struct pollfd fdPoll;

	iSocket=socket (PF_INET, SOCK_STREAM, 0);
	if(iSocket<0)
		return(1);

	init_sockaddr(&sAddrInfo,cHost,25);
	if((connect(iSocket,(struct sockaddr *) &sAddrInfo,sizeof (sAddrInfo))))
		return(2);
	
	fdPoll.fd=iSocket;
	fdPoll.events=POLLIN;
	
	//Wait up to 5 secs for the SMTP banner
	if((poll(&fdPoll,1,2000))==-1)
		return(3);

	if(fdPoll.revents!=POLLIN)
		return(4);
	else
	{
		if((read(iSocket,cRead,100))==-1)
			return(5);
	}
	//If we reach this point, it means:
	//1. The connection could be stablished
	//2. We read the SMTP banner
	//So, we close the socket and inform that everything seems OK.
	close (iSocket);
	
	return(0);

}//unsigned uConnTest(char *cHost)


void init_sockaddr (struct sockaddr_in *name,const char *hostname,uint16_t port)
{
	struct hostent *hostinfo;

	name->sin_family=AF_INET;
	name->sin_port=htons(port);
	hostinfo=gethostbyname(hostname);
	if(hostinfo==NULL)
		return;
	name->sin_addr=*(struct in_addr *)hostinfo->h_addr;

}//void init_sockaddr (struct sockaddr_in *name,const char *hostname,uint16_t port)



