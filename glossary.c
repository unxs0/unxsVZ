/*
FILE 
	glossary.c
	svn ID removed
AUTHOR
	(C) 2006-2010 Gary Wallis and Hugo Urquiza for Unixservice, LLC.
	GPLv2 license applies see LICENSE file included.
PURPOSE
	For tGlossary content html popups.
*/

#include "mysqlrad.h"

static char cLabel[33]={""};
static char *cText="";

//Protos
void StyleSheet(void);//main.c

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
	printf("Content-type: text/html\n\n");
	StyleSheet();
	printf("<title>%s Glossary Entry</title>\n",cLabel);
	htmlGlossaryPage();
	exit(0);

}//void htmlGlossary(void)


void htmlGlossaryPage(void)
{
	printf("<b>%s Glossary Entry</b><br><br>\n"
		"%s",
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

