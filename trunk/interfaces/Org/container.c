/*
FILE 
	container.c
	$Id: container.c 1478 2010-05-25 02:43:12Z Gary $
AUTHOR/LEGAL
	(C) 2010, 2011 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	unxsvzOrg program file.
	Container tab functions.
*/

#include "interface.h"

extern unsigned guBrowserFirefox;//main.c
char cHostname[100]={""};
static char cSearch[100]={""};

//TOC
void ProcessContainerVars(pentry entries[], int x);
void ContainerGetHook(entry gentries[],int x);

void ProcessContainerVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"cHostname"))
			sprintf(cHostname,"%.99s",entries[i].val);
	}

}//void ProcessContainerVars(pentry entries[], int x)


void ContainerGetHook(entry gentries[],int x)
{

	htmlContainer();

}//void ContainerGetHook(entry gentries[],int x)


void ContainerCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Container"))
	{
		ProcessContainerVars(entries,x);
		if(!strcmp(gcFunction,"Container Search"))
		{
			if(!cSearch[0])
			{
				gcMessage="<blink>Error</blink>: Search pattern must be specified";
				htmlContainer();
			}
			gcMessage="Search performed";
			htmlContainer();
		}
		htmlContainer();
	}

}//void ContainerCommands(pentry entries[], int x)


void htmlContainer(void)
{
	htmlHeader("unxsvzOrg","Header");
	htmlContainerPage("unxsvzOrg","Container.Body");
	htmlFooter("Footer");

}//void htmlContainer(void)


void htmlContainerPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uPLAINSET,uVDNSORGTYPE);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="unxsvzOrg.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcUser;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);//Safe?

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcMessage";
			template.cpValue[7]=gcMessage;

			template.cpName[8]="cHostname";
			template.cpValue[8]=cHostname;

			template.cpName[9]="cSearch";
			template.cpValue[9]=cSearch;

			template.cpName[10]="";

			printf("\n<!-- Start htmlContainerPage(%s) -->\n",cTemplateName); 
			Template(field[0],&template,stdout);
			printf("\n<!-- End htmlContainerPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlContainerPage()
