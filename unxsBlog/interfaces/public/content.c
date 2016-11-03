/*
FILE 
	content.c
	svn ID removed
AUTHOR
	(C) 2007 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	unxsBlog program file.
*/


#include "interface.h"

static unsigned uContent=0;
static char cTitle[101]={""};
static char cSectionTitle[101]={""};
static char *cText={""};
static char cImage[101]={""};
static char cSearch[65]={""};


void htmlShowContent(unsigned uPlain);
void LoadContent(unsigned uContent);
unsigned uArticlesCount(unsigned uContentType);

void ProcessContentVars(pentry entries[], int x)
{

	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.64s",entries[i].val);
					
	}

}//void ProcessUserVars(pentry entries[], int x)


void SectionsGetHook(entry gentries[],int x)
{
	//At gcPage we will have the section label
	//which is a tContentType.cLabel
	//
	//The template for sections will be only one.
	//Sections will display a list of articles
	//Once clicked, the articles will be loaded
	//via ContentGetHook()
	//If the section has only one article, that single article
	//will be loaded, so that's the first check
	//
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	sprintf(gcQuery,"SELECT uContentType FROM tContentType WHERE cLabel='%s'",gcPage);
	mysql_query(&gMysql,gcQuery);
	
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&guContentType);
		sprintf(cTitle,"%.100s",gcPage);
			
		if(uArticlesCount(guContentType)==1)
		{
			MYSQL_RES *res2;
			MYSQL_ROW field2;

			sprintf(gcQuery,"SELECT uContent FROM tContent WHERE uContentType='%u'",guContentType);
			mysql_query(&gMysql,gcQuery);
			
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res2=mysql_store_result(&gMysql);
			if((field2=mysql_fetch_row(res2)))
			{
				sscanf(field2[0],"%u",&uContent);
				LoadContent(uContent);
				htmlShowContent(1);
			}
		}
		
		sprintf(gcQuery,"%s :: %s",SITE_NAME,gcPage);
		htmlHeader(gcQuery,"Header");
		htmlContentPage("","Section.Body");
		htmlFooter("Footer");
	}
	else
		gcMessage="<blink>Page not found!</blink>";
	
	htmlContent();
	
}//void SectionsGetHooktGetHook(entry gentries[],int x)


void ContentGetHook(entry gentries[],int x)
{
	//Via this function we wil handle the reading of 
	//magazine articles.
	//Let's quickly review an article URL
	//
	//?gcPage=Content&uContentType=2&uContent=14
	//
	//So, uContentType indicates which section(tab) we are
	//uContent indicates the tContent row id for the article
	//So if uContent, we will load the article and present it
	//with a different template, caling htmlShowContent()
	//
	//Prety simple :)
	
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uContent"))
			sscanf(gentries[i].val,"%u",&uContent);
		else if(!strcmp(gentries[i].name,"uContentType"))
			sscanf(gentries[i].val,"%u",&guContentType);
	}

	if(uContent)
	{
		LoadContent(uContent);		
		htmlShowContent(0);
	}
	
	htmlContent();

}//void ContentGetHook(entry gentries[],int x)


void LoadContent(unsigned uContent)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cTitle,cText,cImage,cLabel FROM tContent,tContentType WHERE uContent=%u AND tContent.uContentType=tContentType.uContentType",uContent);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
	{
		sprintf(cTitle,"%.100s",field[0]);
		cText=field[1];
		sprintf(cImage,"%.100s",field[2]);
		sprintf(cSectionTitle,"%.100s",field[3]);
	}
	else
		gcMessage="<blink>Content not available!</blink>";
	
}//void LoadContent(unsigned uContent)


unsigned uArticlesCount(unsigned uContentType)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCount=0;
	
	sprintf(gcQuery,"SELECT COUNT(uContent) FROM tContent WHERE uContentType='%u'",uContentType);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uCount);

	return(uCount);
	
}//unsigned uArticlesCount(unsigned uContentType)


void ContentCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Content"))
	{
		ProcessContentVars(entries,x);
		if(!strcmp(gcFunction,"Search"))
		{
			sprintf(gcQuery,"%s :: Search results",SITE_NAME);
			htmlHeader(gcQuery,"Header");
			htmlContentPage("","SearchResults.Body");
			htmlFooter("Footer");
		}
		htmlContent();
	}

}//void ContentCommands(pentry entries[], int x)


void htmlShowContent(unsigned uPlain)
{
	sprintf(gcQuery,"%s :: %s - %s",SITE_NAME,cSectionTitle,cTitle);
	htmlHeader(gcQuery,"Header");
	if(uPlain)
		htmlContentPage("","ContentPlain.Body");
	else
		htmlContentPage("","Content.Body");
	htmlFooter("Footer");
}//void htmlShowContent(void)


void htmlContent(void)
{
	if(guContentType==7)
	{
		MYSQL_RES *res;
		MYSQL_ROW field;

		sprintf(gcQuery,"SELECT uContent FROM tContent WHERE uContentType=7");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uContent);
			LoadContent(uContent);
			htmlShowContent(1);
		}
	}
	htmlHeader(SITE_NAME,"Header");
	htmlContentPage("","Section.Body");
	htmlFooter("Footer");

}//void htmlContent(void)


void htmlContentPage(char *cPageTitle, char *cTemplateName)
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
			char cMyImage[256]={""};
			
			template.cpName[0]="cPageTitle";
			template.cpValue[0]=cPageTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="magazine2.cgi";
			
			template.cpName[2]="gcHost";
			template.cpValue[2]=gcHost;

			template.cpName[3]="gcMessage";
			template.cpValue[3]=gcMessage;

			template.cpName[4]="cTitle";
			template.cpValue[4]=cTitle;

			template.cpName[5]="cText";
			template.cpValue[5]=cText;

			template.cpName[6]="cImage";
			if(cImage[0])
				sprintf(cMyImage,"<img src='%s' border=0>\n",cImage);
			template.cpValue[6]=cMyImage;

			template.cpName[7]="cSectionTitle";
			template.cpValue[7]=cSectionTitle;

			template.cpName[8]="cBoxTitle";
			template.cpValue[8]=BOX_TITLE;

			template.cpName[9]="";

			printf("\n<!-- Start htmlContentPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlContentPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlContentPage()


void funcNavBar(FILE *fp)
{
	//This function will display the top navigation
	//bar, getting the links from tContentType
	//We may need to extend tContent type
	//to store more stuff like:
	//
	//uOrder [DONE]
	//uVisible: will have a link yes/no [DONE]
	//cTitleText
	//cLink
	//
	//Section will be determined by the global var guContentType
	//
	
	MYSQL_RES *res;
	MYSQL_ROW field;
	struct t_template template;
	unsigned uContentType=0;
	
	fprintf(fp,"<!-- %u funcNavBar() start -->\n",guContentType);
	
	sprintf(gcQuery,"SELECT uContentType,cLabel FROM tContentType WHERE uVisible=1 ORDER BY uOrder");
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContentType);
		
		template.cpName[0]="cLabel";
		template.cpValue[0]=field[1];

		template.cpName[1]="cTDClass";
		template.cpName[2]="cAClass";
		
		if(uContentType==guContentType)
		{
			template.cpValue[1]=FOCUS_TAB_CLASS;
			template.cpValue[2]=FOCUS_TAB_LINK_CLASS;
		}
		else
		{
			template.cpValue[1]=DEFAULT_TAB_CLASS;
			template.cpValue[2]=DEFAULT_TAB_LINK_CLASS;
		}
		
		template.cpName[3]="";
		fpTemplate(fp,"NavBar.Item",&template);
	}

	fprintf(fp,"<!-- funcNavBar() end -->\n");
			
}//void funcNavBar(FILE *fp)


void funcFeaturedArticles(FILE *fp)
{
	//Will display a list of links with the titles of the articles
	//that are featured.
	//Initially just a list of links, no template support
	//

	MYSQL_RES *res;
	MYSQL_ROW field;
	
	fprintf(fp,"<!-- funcFeaturedArticles() start  -->\n");
	
	sprintf(gcQuery,"SELECT uContent,cTitle,uContentType FROM tContent WHERE uFeatured=1 ORDER BY uOrder");
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
		fprintf(fp,"<a href=?gcPage=Content&uContentType=%s&uContent=%s>%s</a><br>\n",
				field[2]
				,field[0]
				,field[1]
		       );
			
	fprintf(fp,"<!-- funcFeaturedArticles() end -->\n");

}//void funcFeaturedArticles(FILE *fp)


void funcSectionArticles(FILE *fp)
{
	//Will display a list of links with the titles of the articles
	//of the guContentType section
	//Initially just a list of links, no template support
	//

	MYSQL_RES *res;
	MYSQL_ROW field;
	
	fprintf(fp,"<!-- funcSectionArticles() start  -->\n");
	
	sprintf(gcQuery,"SELECT uContent,cTitle FROM tContent WHERE uContentType=%u ORDER BY uOrder",guContentType);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	if(!mysql_num_rows(res))
		fprintf(fp,"No articles in this section yet.<br>\n");
	
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"<a href=?gcPage=Content&uContentType=%u&uContent=%s>%s</a><br>\n",
				guContentType
				,field[0]
				,field[1]
		       );
			
	fprintf(fp,"<!-- funcSectionArticles() end -->\n");

}//void funcFeaturedArticles(FILE *fp)


void funcSearchResults(FILE *fp)
{
	//Will display a list of links with the titles of the articles
	//based on search term at cSearch
	//Initially just a list of links, no template support
	//

	MYSQL_RES *res;
	MYSQL_ROW field;
	
	fprintf(fp,"<!-- funcSearchResults() start  -->\n");
	
	sprintf(gcQuery,"SELECT uContent,cTitle,uContentType FROM tContent WHERE cTitle LIKE '%%%s%%' OR cText LIKE '%%%s%%' ORDER BY uOrder",cSearch,cSearch);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	if(!mysql_num_rows(res))
	{
		gcMessage="No pages found with your search query";
		fprintf(fp,"No results.");
	}

	while((field=mysql_fetch_row(res)))
		fprintf(fp,"<a href=?gcPage=Content&uContentType=%s&uContent=%s>%s</a><br>\n",
				field[2]
				,field[0]
				,field[1]
		       );
			
	fprintf(fp,"<!-- funcSearchResults() end -->\n");

}//void funcFeaturedArticles(FILE *fp)

