/*
FILE 
	glossary.c
	$Id: glossary.c 660 2008-12-18 18:48:48Z hus $
AUTHOR
	(C) 2006-2010 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	iDNS Admin (Owner) Interface, modified for backend usage
	program file.
*/

#include "mysqlrad.h"

static char cLabel[33]={""};
static char *cText="";

//
//Local only
void SelectGlossary(char *cLabel);
void htmlGlossaryPage(void);
void htmlGlossary(void);

void GlossaryGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",gentries[i].val);
	}

	if(cLabel[0])
	{
		MYSQL_RES *res;
		MYSQL_ROW field;
		
		SelectGlossary(cLabel);
		res=mysql_store_result(&gMysql);
		
		if((field=mysql_fetch_row(res)))
			cText=field[0];		
		else
			cText="No description available";
	}
		
	htmlGlossary();
	

}//void GlossaryGetHook(entry gentries[],int x)


void htmlGlossary(void)
{
	Header_ism3(":: Glossary",0);
	htmlGlossaryPage();
	Footer_ism3();

}//void htmlGlossary(void)


void htmlGlossaryPage(void)
{
	printf("<fieldset><legend><b>%s Glossary Entry</b>\n"
		"<table>\n"
		"<tr><td>%s</td></tr>\n"
		"</table>\n",
		cLabel
		,cText
		);

}//void htmlGlossaryPage()


void SelectGlossary(char *cLabel)
{
	sprintf(gcQuery,"SELECT cText FROM tGlossary WHERE cLabel='%s'",cLabel);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
}//void SelectGlossary(char *cLabel)

