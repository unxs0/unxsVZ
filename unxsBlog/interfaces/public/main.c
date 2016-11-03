/*
FILE 
	main.c
	svn ID removed
AUTHOR
	(C) 2007 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	unxsBlog public interface main module
REQUIRES
	OpenISP libtemplates.a and templates.h
*/

#include "interface.h"

//Global vars
//libtemplate.a required
MYSQL gMysql;
//Multipurpose buffer
char gcQuery[4096]={""};

//
//Template vars
char *gcMessage="&nbsp;";
//SSLLoginCookie()
char cLogKey[16]={"Ksdj458jssdUjf79"};
char gcHost[100]={""};
char gcHostname[100]={""};

char gcFunction[100]={""};
char gcPage[100]={""};
unsigned guBrowserFirefox=0;
unsigned guContentType=7;

//
//Local only
int main(int argc, char *argv[]);
	

int main(int argc, char *argv[])
{
	pentry entries[MAXPOSTVARS];
	entry gentries[MAXGETVARS];
	char *gcl;
	register int i;
	int cl=0;


	ConnectDb();

	if(getenv("REMOTE_ADDR")!=NULL)
		sprintf(gcHost,"%.99s",getenv("REMOTE_ADDR"));

	if(getenv("HTTP_USER_AGENT")!=NULL)
	{
		if(strstr(getenv("HTTP_USER_AGENT"),"Firefox"))
			guBrowserFirefox=1;
	}
#if defined(Linux)
	gethostname(gcHostname, 98);
#else
	//Solaris
	sysinfo(SI_HOSTNAME, gcHostname, 98);
#endif

	if(strcmp(getenv("REQUEST_METHOD"),"POST"))
	{
		//Get	
	
		gcl = getenv("QUERY_STRING");
		for(i=0;gcl[0] != '\0' && i<MAXGETVARS;i++)
		{
			getword(gentries[i].val,gcl,'&');
			plustospace(gentries[i].val);
			unescape_url(gentries[i].val);
			getword(gentries[i].name,gentries[i].val,'=');

			if(!strcmp(gentries[i].name,"gcFunction"))
				sprintf(gcFunction,"%.99s",gentries[i].val);
			else if(!strcmp(gentries[i].name,"gcPage"))
				sprintf(gcPage,"%.99s",gentries[i].val);
			else if(!strcmp(gentries[i].name,"guContentType"))
				sscanf(gentries[i].val,"%u",&guContentType);
		}
		if(gcPage[0])
		{
			if(!strcmp(gcPage,"Content"))
				ContentGetHook(gentries,i);
			if(!strcmp(gcPage,"Home"))
			{
				guContentType=7;
				htmlContent();
			}
			else
				SectionsGetHook(gentries,i);
		}
	}
	else
	{
		//Post
		
		cl = atoi(getenv("CONTENT_LENGTH"));
		for(i=0;cl && (!feof(stdin)) && i<MAXPOSTVARS ;i++)
		{
			entries[i].val = fmakeword(stdin,'&',&cl);
			plustospace(entries[i].val);
			unescape_url(entries[i].val);
			entries[i].name = makeword(entries[i].val,'=');
			
			if(!strcmp(entries[i].name,"gcFunction"))
				sprintf(gcFunction,"%.99s",entries[i].val);
			else if(!strcmp(entries[i].name,"gcPage"))
				sprintf(gcPage,"%.99s",entries[i].val);
			else if(!strcmp(entries[i].name,"guContentType"))
				sscanf(entries[i].val,"%u",&guContentType);
		}
	}

	//Not required to be logged in gcFunction section
	ContentCommands(entries,i);
	htmlContent();
	return(0);

}//end of main()


void htmlPlainTextError(const char *cError)
{
	char cQuery[1024];

	printf("Content-type: text/plain\n\n");
	printf("Please report this idnsAdmin fatal error ASAP:\n%s\n",cError);

	//Attempt to report error in tLog
        sprintf(cQuery,"INSERT INTO tLog SET cLabel='htmlPlainTextError',uLogType=4,uPermLevel=%u,uLoginClient=%u,cLogin='%s',cHost='%s',cMessage=\"%s\",cServer='%s',uOwner=1,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",0,0,"No login",gcHost,cError,gcHostname,0);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		printf("Another error occurred while attempting to log: %s\n",
				mysql_error(&gMysql));
	exit(0);

}//void htmlPlainTextError(const char *cError)


void htmlHeader(char *cTitle, char *cTemplateName)
{
	printf("Content-type: text/html\n\n");

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
		
			template.cpName[1]="";

			printf("\n<!-- Start htmlHeader(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlHeader(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr><center><font size=1>%s %s</font>",cTitle,cTemplateName);
		}
		mysql_free_result(res);
	}
	else
	{
	printf("<html><head><title>%s</title></head><body bgcolor=white><font face=Arial,Helvetica",cTitle);
	}

}//void htmlHeader()


void htmlFooter(char *cTemplateName)
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
			template.cpName[0]="cIspName";
			template.cpValue[0]=ISPNAME;
			template.cpName[1]="cIspUrl";
			template.cpValue[1]=ISPURL;
			template.cpName[2]="cCopyright";
			sprintf(gcQuery,"&copy; 2007 %s. All Rights Reserved.",SITE_NAME);
			template.cpValue[2]=gcQuery;
			template.cpName[3]="";

			printf("\n<!-- Start htmlFooter(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlFooter(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}
	else
	{
		printf("<hr>");
		printf("<center><font size=1><a href=%s>%s</a>\n",
				ISPURL,ISPNAME);
		printf("</body></html>");
	}

	exit(0);

}//void htmlFooter()


void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template)
{
	if(cTemplateName[0])
	{	
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect(cTemplateName);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"\n<!-- Start fpTemplate(%s) -->\n",cTemplateName); 
			Template(field[0], template, fp);
			fprintf(fp,"\n<!-- End fpTemplate(%s) -->\n",cTemplateName); 
		}
		else
		{
			fprintf(fp,"<hr>");
			fprintf(fp,"<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}
}//void fpTemplate(FILE *fp,char *cTemplateName,struct t_template *template)


void ConnectDb(void)
{
        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,DBIP,DBLOGIN,DBPASSWD,DBNAME,0,NULL,0))
        {
		htmlHeader("idnsAdmin.cgi Error","Header");
                printf("Database server <u>unavailable</u>\n");
		htmlFooter("Footer");
        }

}//end of ConnectDb()


//libtemplate.a required
void AppFunctions(FILE *fp,char *cFunction)
{
	if(!strcmp(cFunction,"funcNavBar"))
		funcNavBar(fp);
	else if(!strcmp(cFunction,"funcFeaturedArticles"))
		funcFeaturedArticles(fp);
	else if(!strcmp(cFunction,"funcSectionArticles"))
		funcSectionArticles(fp);
	else if(!strcmp(cFunction,"funcSearchResults"))
		funcSearchResults(fp);
	
}//void AppFunctions(FILE *fp,char *cFunction)


char *TextAreaSave(char *cField)
{
	register unsigned int i=0,j=0,uNum=0,uCtrlM=0;
	char *cCopy=NULL;

	for(i=0;cField[i];i++)
	{
		if(cField[i]=='\'' || cField[i]=='\\') uNum++;
		if(cField[i]=='\r') uCtrlM++;
	}
	if(!uNum && !uCtrlM) return(cField);

	if(uNum)
		cCopy=(char *)malloc( ( (strlen(cField)) + (uNum*2) + 1 ));
	else
		cCopy=(char *)cField;

	if(!cCopy) htmlPlainTextError("TextAreaInput() malloc error");

	i=0;
	while(cField[i])
	{
		if( cField[i]=='\'' )
		{
			cCopy[j++]='\\';
			cCopy[j++]='\'';
		}
		else if( cField[i]=='\\')
		{
			cCopy[j++]='\\';
			cCopy[j++]='\\';
		}
		
		//Remove nasty ctrl-m's. fsck /u Bill!
		else if(cField[i]!='\r')
		{
			cCopy[j++]=cField[i];
		}
		i++;
	}

	cCopy[j]=0;
	return(cCopy);

}//char *TextAreaSave(char *cField)


int ReadPullDown(const char *cTableName,const char *cFieldName,const char *cLabel)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

        unsigned int iRowid=0;//Not found

        sprintf(gcQuery,"select _rowid from %s where %s='%s'",
                        cTableName,cFieldName,TextAreaSave((char *) cLabel));
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
        mysqlRes=mysql_store_result(&gMysql);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sscanf(mysqlField[0],"%u",&iRowid);
        mysql_free_result(mysqlRes);
        return(iRowid);

}//ReadPullDown(char *cTableName,char *cLabel)


char *cShortenText(char *cText)
{
	//Return the first n word from cText
	//will use the spaces for word counting.
	unsigned uCount=0;
	register int i=0;
	static char cResult[100];
	
	for(i=0;i<strlen(cText);i++)
	{
		cResult[i]=cText[i];
		if(cText[i]==' ')
			uCount++;
		if(uCount>=8) break;
	}

	cResult[i]='\0';
	return(cResult);
	
}//char *cShortenText(char *cText)
