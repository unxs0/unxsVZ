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
char gcCtHostname[100]={""};
static char gcSearch[100]={""};
unsigned guContainer=0;
//Container details
static char gcLabel[33]={""};


//TOC
void ProcessContainerVars(pentry entries[], int x);
void ContainerGetHook(entry gentries[],int x);
char *cGetHostname(unsigned uContainer);
void SelectContainer(void);

void ProcessContainerVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"guContainer"))
			sscanf(entries[i].val,"%u",&guContainer);
		else if(!strcmp(entries[i].name,"gcCtHostname"))
			sprintf(gcCtHostname,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"gcSearch"))
			sprintf(gcSearch,"%.99s",entries[i].val);
	}

}//void ProcessContainerVars(pentry entries[], int x)


void ContainerGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"guContainer"))
			sscanf(gentries[i].val,"%u",&guContainer);
	}

	if(guContainer)
	{
		SelectContainer();
		htmlContainer();
	}

	htmlContainer();

}//void ContainerGetHook(entry gentries[],int x)


void ContainerCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Container"))
	{
		ProcessContainerVars(entries,x);
		if(!strcmp(gcFunction,"Container Search"))
		{
			if(!gcSearch[0])
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
	if(guContainer)
		SelectContainer();
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

			template.cpName[8]="gcCtHostname";
			//template.cpValue[8]=gcCtHostname;
			template.cpValue[8]=(char *)cGetHostname(guContainer) ;

			template.cpName[9]="gcSearch";
			template.cpValue[9]=gcSearch;

			template.cpName[10]="guContainer";
			char cguContainer[16];
			sprintf(cguContainer,"%u",guContainer);
			template.cpValue[10]=cguContainer;

			template.cpName[11]="gcLabel";
			template.cpValue[11]=gcLabel;

			template.cpName[12]="cDisabled";
			if(guContainer)
				template.cpValue[12]="";
			else
				template.cpValue[12]="disabled";


			template.cpName[13]="";

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


void funcContainerImageTag(FILE *fp)
{
	if(guContainer)
		fprintf(fp,"<img src=/traffic/%u.png>",guContainer);

}//void funcContainerImageTag(FILE *fp)


void funcSelectContainer(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCount=1;
	unsigned uContainer=0;

	fprintf(fp,"<!-- funcSelectContainer(fp) Start -->\n");

	if(gcSearch[0])
		sprintf(gcQuery,"SELECT uContainer,cHostname FROM tContainer WHERE "
			"uSource=0 AND uOwner=%u AND cHostname LIKE '%s%%' "
			"ORDER BY cHostname LIMIT 101",guOrg,gcSearch);
	else
		sprintf(gcQuery,"SELECT uContainer,cHostname FROM tContainer WHERE "
			"uOwner=%u AND uSource=0 ORDER BY cHostname LIMIT 101",guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	fprintf(fp,"<select class=type_textarea title='Select the container you want to load with this dropdown'"
			" name=guContainer onChange='submit()'>\n");
	fprintf(fp,"<option>---</option>");
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContainer);
		fprintf(fp,"<option value=%s",field[0]);
		if(guContainer==uContainer)
			fprintf(fp," selected");
		if((uCount++)<=100)
			fprintf(fp,">%s</option>",field[1]);
		else
			fprintf(fp,">Limit reached! Use better filter</option>");
	}
	mysql_free_result(res);

	fprintf(fp,"</select>\n");

	fprintf(fp,"<!-- funcSelectContainer(fp) End -->\n");

}//void funcSelectContainer(FILE *fp)


char *cGetHostname(unsigned uContainer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	static char cHostname[100]={""};

	sprintf(gcQuery,"SELECT cHostname FROM tContainer WHERE uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return((char *)mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cHostname,"%.99s",field[0]);
	return(cHostname);
}//char *cGetHostname(unsigned uContainer)


void SelectContainer(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel FROM tContainer WHERE uContainer=%u"
			,guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcLabel,"%.32s",field[0]);

		if(!gcMessage[0]) gcMessage="Zone Selected";
	}
	else
	{
		gcLabel[0]=0;
		gcMessage="<blink>No Zone Selected</blink>";
	}
	
	mysql_free_result(res);
	
}//void SelectContainer(void)

