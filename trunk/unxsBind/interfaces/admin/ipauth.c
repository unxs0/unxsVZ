/*
FILE 
	bulkop.c
	$Id: bulkop.c 660 2008-12-18 18:48:48Z hus $
AUTHOR
	(C) 2006-2009 Gary Wallis  and Hugo Urquiza for Unixservice
PURPOSE
	iDNS Administration (ASP) Interface
	program file.
*/
#include "interface.h"
#include <openisp/ucidr.h>

static char *cMassList={""};
static char cImportMsg[32762]={""}; //A 32k buffer will be enough, if not, truncate the data.
static unsigned uFormat=0;

char *ParseTextAreaLines(char *cTextArea);//bulkop.c
void RIPEImport(void);

void ProcessIPAuthVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cMassList"))
			cMassList=entries[i].val;
		else if(!strcmp(entries[i].name,"uFormat"))
			sscanf(entries[i].val,"%u",&uFormat);
	}

}//void ProcessIPAuthVars(pentry entries[], int x)


void IPAuthGetHook(entry gentries[],int x)
{
/*	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"cZone"))
			sprintf(gcZone,"%.99s",gentries[i].val);
		else if(!strcmp(gentries[i].name,"uView"))
			sprintf(cuView,"%.15s",gentries[i].val);
	}
*/	
	htmlIPAuth();

}//void IPAuthGetHook(entry gentries[],int x)


void IPAuthCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"IPAuth"))
	{
		ProcessIPAuthVars(entries,x);
		if(!strcmp(gcFunction,"IP Auth Import"))
		{
			switch(uFormat)
			{
				case 1:
					RIPEImport();
				default:
					gcMessage="<blink>Error:</blink> I don't know how to handle that format";
			}
		}
		htmlIPAuth();
	}
}//void IPAuthCommands(pentry entries[], int x)


void htmlIPAuth(void)
{
	htmlHeader("DNS System","Header");
	htmlIPAuthPage("DNS System","IPAuth.Body");
	htmlFooter("Footer");

}//void htmlIPAuth(void)


void htmlIPAuthPage(char *cTitle, char *cTemplateName)
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
			char cuResource[16]={""};

			sprintf(cuResource,"%u",uResource);

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="idnsAdmin.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcUser;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcModStep";
			template.cpValue[7]=gcModStep;

			template.cpName[8]="cZone";
			template.cpValue[8]=gcZone;

			template.cpName[9]="gcMessage";
			template.cpValue[9]=gcMessage;

			template.cpName[10]="cuView";
			template.cpValue[10]=cuView;

			template.cpName[11]="cImportMsg";
			template.cpValue[11]=cImportMsg;

			template.cpName[12]="cCustomer";
			template.cpValue[12]=gcCustomer;

			template.cpName[13]="uResource";
			template.cpValue[13]=cuResource;

			template.cpName[14]="";

			printf("\n<!-- Start htmlIPAuthPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlIPAuthPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlIPAuthPage()



//Import functions code begins here
//Will extend in the future for ARIN and LACNIC
//

void RIPEImport(void)
{
	char cLine[512]={"ERROR"};
	char *cp;
	unsigned uLineNumber=0;
	unsigned uProcessed=0;
	unsigned uIgnored=0;
	char cIPBlockStart[64]={""};
	char cIPBlockEnd[64]={""};
	unsigned uCidr=0;
	unsigned uSize=0;
	unsigned uClient=0;
	unsigned uOther=0;
	unsigned uDate=0;

	printf("Content-type: text/plain\n\n");

	while(1)
	{

		uLineNumber++;
		sprintf(cLine,"%.255s",ParseTextAreaLines(cMassList));
		//ParseTextAreaLines() required break;
		if(cLine[0]==0) break;
		//Comments ignore
		if(cLine[0]=='#') continue;
		if(cLine[0]==';') continue;
		if(strstr(cLine,"CUST"))
		{
			//80.253.98.0 - 80.253.98.255 256 20060404 PKXG-CUST-1234-01
			uProcessed++;
			sscanf(cLine,"%s - %s %u %u PKXG-CUST-%u-%u",
				cIPBlockStart
				,cIPBlockEnd
				,&uSize
				,&uDate
				,&uClient
				,&uOther);

		}
		else if(strstr(cLine,"MRP"))
		{
			uProcessed++;
		}
		else if(strstr(cLine,"INFRA"))
		{
			uProcessed++;
		}
		else if(1)
		{
			//Invalid, skip
			uIgnored++;
			continue;
		}

	}

	exit(0);

}//void RIPEImport(void)

