/*
FILE 
	order.c
	$Id: invoice.c 508 2009-08-13 23:17:58Z Hugo $
AUTHOR
	(C) 2007-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	ispAdmin  Interface
	program file.
*/


#include "interface.h"
void htmlTicketPage(char *cTitle, char *cTemplateName);


void ProcessTicketVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
/*		if(!strcmp(entries[i].name,"uTicket"))
			sscanf(entries[i].val,"%u",&uTicket);
		else if(!strcmp(entries[i].name,"uClient"))
			sscanf(entries[i].val,"%u",&uClient);
		else if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.99s",entries[i].val);
*/					
	}

}//void ProcessUserVars(pentry entries[], int x)


void TicketGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
/*		if(!strcmp(gentries[i].name,"uTicket"))
			sscanf(gentries[i].val,"%u",&uTicket);
		else if(!strcmp(gentries[i].name,"uClient"))
			sscanf(gentries[i].val,"%u",&uClient);*/
	}

	htmlTicket();

}//void TicketGetHook(entry gentries[],int x)


void TicketCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Ticket"))
	{
		ProcessTicketVars(entries,x);
/*
		if(!strcmp(gcFunction,"Approved and Shipped"))
		{
			sprintf(cApprovedStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Approved and Shipped"))
		{
			sprintf(gcQuery,"UPDATE tTicket SET uTicketStatus=%u WHERE uTicket=%u AND uClient=%u",
					APPROVED_AND_SHIPPED
					,uTicket
					,uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			if(mysql_affected_rows(&gMysql))
			{
				unxsISPLog(uTicket,"tTicket","Mod");
				gcMessage="Ticket status updated.";
			}
			else
			{
				unxsISPLog(uTicket,"tTicket","Mod Error");
				gcMessage="<blink>Ticket status not updated. Contact support.</blink>";
			}
		}
		 else if(!strcmp(gcFunction,"Void"))
		 {
			 sprintf(cVoidStep,"Confirm ");
		 }
		else if(!strcmp(gcFunction,"Confirm Void"))
		{
			sprintf(gcQuery,"UPDATE tTicket SET uTicketStatus=%u WHERE uTicket=%u AND uClient=%u",
					VOID
					,uTicket
					,uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			if(mysql_affected_rows(&gMysql))
			{
				unxsISPLog(uTicket,"tTicket","Mod");
				gcMessage="Ticket voided.";
			}
			else
			{
				unxsISPLog(uTicket,"tTicket","Mod Error");
				gcMessage="<blink>Ticket status not updated. Contact support.</blink>";
			}
			ReStockItems(uTicket);
			
		}
		else if(!strcmp(gcFunction,"Email Loaded Ticket"))
		{
			sprintf(gcNewStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Email Loaded Ticket"))
		{
			EmailLoadedTicket();
			gcMessage="Ticket emailed OK";
		}
		else if(!strcmp(gcFunction,"Email All Tickets"))
		{
			sprintf(gcModStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Email All Tickets"))
		{
			EmailAllTickets();
			gcMessage="All invoices emailed  OK";
		}
		else if(!strcmp(gcFunction,"Print Loaded Ticket"))
		{
			gcPrintCurr="Confirm ";
		}
		else if(!strcmp(gcFunction,"Confirm Print Loaded Ticket"))
		{
			PrintTicket();
		}
		else if(!strcmp(gcFunction,"Print All Tickets"))
		{
			gcPrintAll="Confirm ";
		}
		else if(!strcmp(gcFunction,"Confirm Print All Tickets"))
		{
			PrintTickets();
		}*/
		htmlTicket();
	}

}//void TicketCommands(pentry entries[], int x)


void htmlTicket(void)
{
	htmlHeader("unxsISP Admin","Header");
	htmlTicketPage("","Ticket.Body");
	htmlFooter("Footer");

}//void htmlTicket(void)


void htmlTicketPage(char *cTitle, char *cTemplateName)
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
			char cuTicket[16]={""};
			
			//sprintf(cuTicket,"%u",uTicket);

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="ispAdmin.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcLogin;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcMessage";
			template.cpValue[7]=gcMessage;
/*
			template.cpName[8]="uTicket";
			template.cpValue[8]=cuTicket;

			template.cpName[9]="uClient";
			template.cpValue[9]=cuClient;

			template.cpName[10]="cNavList";
			template.cpValue[10]=cNavList;

			template.cpName[11]="gcInputStatus";
			if(uTicket && uClient)
				template.cpValue[11]="";
			else
				template.cpValue[11]="disabled";

			template.cpName[12]="cApprovedStep";
			template.cpValue[12]=cApprovedStep;

			template.cpName[13]="cVoidStep";
			template.cpValue[13]=cVoidStep;

			template.cpName[14]="gcNewStep";
			template.cpValue[14]=gcNewStep;

			template.cpName[15]="gcModStep";
			template.cpValue[15]=gcModStep;

			template.cpName[16]="gcPrintCurr";
			template.cpValue[16]=gcPrintCurr;

			template.cpName[17]="gcPrintAll";
			template.cpValue[17]=gcPrintAll;
*/
			template.cpName[8]="";
			
			printf("\n<!-- Start htmlTicketPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlTicketPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlTicketPage()


void funcTicketNavList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uDisplayed=0;
	unsigned uFound=0;
	char cTopMessage[100]={""};
/*
	if(cSearch[0])
	{
	//Valid formats are:
	//1. uTicket-uClient
	//2. uTicket
	//3. Part of last name
		char cExtra[100]={""};
		if(strstr(cSearch,"-"))
		{
			unsigned uClient=0;

			sscanf(cSearch,"%u-%u",&uClient,&uTicket);
			sprintf(cExtra,"tTicket.uTicket=%u AND tTicket.uClient=%u",uTicket,uClient);

			ExtSelectSearch("tTicket","tTicket.uTicket,FROM_UNIXTIME(GREATEST(tTicket.uCreatedDate,tTicket.uModDate)),"
				"(SELECT CONCAT(cFirstName,' ',cLastName) FROM tClient WHERE tClient.uClient=tTicket.uClient),tTicket.uClient","1","1",
				cExtra,0);
		}
		else
		{
			sscanf(cSearch,"%u",&uTicket);
			if(uTicket)
			{
				sprintf(cExtra,"tTicket.uTicket=%u",uTicket);
				ExtSelectSearch("tTicket","tTicket.uTicket,FROM_UNIXTIME(GREATEST(tTicket.uCreatedDate,tTicket.uModDate)),"
					"(SELECT CONCAT(cFirstName,' ',cLastName) FROM tClient WHERE tClient.uClient=tTicket.uClient),tTicket.uClient",
					"1","1",cExtra,0);
			}
			else
				ExtSelectSearch("tTicket","tTicket.uTicket,FROM_UNIXTIME(GREATEST(tTicket.uCreatedDate,tTicket.uModDate)),"
					"(SELECT CONCAT(cFirstName,' ',cLastName) FROM tClient WHERE tClient.uClient=tTicket.uClient),tTicket.uClient",
					"tTicket.cLastName",cSearch,NULL,0);
		}
	}
	else
		ExtSelect("tTicket","tTicket.uTicket,FROM_UNIXTIME(GREATEST(tTicket.uCreatedDate,tTicket.uModDate)),"
				"(SELECT CONCAT(cFirstName,' ',cLastName) FROM tClient WHERE tClient.uClient=tTicket.uClient),tTicket.uClient",0);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	
	if(!(uFound=mysql_num_rows(res)))
	{
		fprintf(fp,"No records found.<br>\n");
		mysql_free_result(res);
		return;
	}

	sprintf(cTopMessage,"%u record(s) found.",uFound);
	gcMessage=cTopMessage;

	if(uFound==1)
	{
		if((field=mysql_fetch_row(res)))
		{
			//This 'loads' the invoice, via funcTicket() ;)
			sscanf(field[0],"%u",&uTicket);
			sscanf(field[3],"%u",&uClient);
			
			fprintf(fp,"<a href=ispAdmin.cgi?gcPage=Ticket&uTicket=%s&uClient=%s>%s - %s</a><br>\n",field[0],field[3],field[2],field[1]);
			mysql_free_result(res);
			return;
		}
	}

	while((field=mysql_fetch_row(res)))
	{
		if(uDisplayed==20)
		{
			fprintf(fp,"Only the first 20 records found. If the invoice you are looking for is not in the list above please "
			"further refine your search.<br>\n");
			break;
		}
		FromMySQLDate(field[1]);
		fprintf(fp,"<a href=ispAdmin.cgi?gcPage=Ticket&uTicket=%s&uClient=%s>%s - %s</a><br>\n",field[0],field[3],field[2],field[1]);
		uDisplayed++;
	}

	mysql_free_result(res);
*/
}//void funcTicketNavList(FILE *fp)

