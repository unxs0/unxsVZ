/*
FILE
	template.c
	svn ID removed
PURPOSE
	Template function library to use with RAD3 back-office
	applications that have tTemplate and tTemplateSet model.
NOTES
	Applications that link to this static lib must provide:
	1-. A valid MYSQL gMysql structure connected to a mySQL server.
	2-. A function called AppFunctions() that may provide
	special {{funcName}} processing to FILE *fp;
AUTHOR/LEGAL
	(C) 2006-2010 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/file.h>
#include <mysql/mysql.h>
#include <unistd.h>

#include "template.h"

//Returns rows via &gMysql
void TemplateSelectWithQuery(char *cTemplateName)
{
	char cQuery[1024];

		sprintf(cQuery,"SELECT cTemplate,cQuery FROM tTemplate WHERE cLabel='%s'"
				,cTemplateName);
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
                fprintf(stderr,"%s\n",mysql_error(&gMysql));

}//void TemplateSelectWithQuery(char *cTemplateName)


//Returns rows via &gMysql
void TemplateSelectInterfaceWithQuery(char *cTemplateName,unsigned uTemplateSet, unsigned uTemplateType)
{
	char cQuery[1024];

		sprintf(cQuery,"SELECT cTemplate,cQuery FROM tTemplate WHERE cLabel='%s' AND uTemplateSet=%u AND"
				" uTemplateType=%u",cTemplateName,uTemplateSet,uTemplateType);
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
                fprintf(stderr,"%s\n",mysql_error(&gMysql));

}//void TemplateSelectInterfaceWithQuery()



//Output to FILE fp the template with name/value pair substitution done via
//t_template 
int fileStructTemplateWithQuery(FILE *fp, char *cTemplateName,struct t_template *ptrTemplate)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	if(!fp) return(1);
	if(!ptrTemplate) return(1);
	if(!cTemplateName[0]) return(1);

	TemplateSelectWithQuery(cTemplateName);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
			if(field[1][0] && TemplatePairsQuery(field[1],ptrTemplate))
			{
				fprintf(stderr,"TemplatePairsQuery() non SQL error\n");
        			if(mysql_errno(&gMysql))
                			fprintf(stderr,"%s\n",mysql_error(&gMysql));
				return(0);
			}
			
			Template(field[0],ptrTemplate,fp);
	}
	else
	{
			fprintf(fp,"Template %s not found\n",cTemplateName);
	}
	mysql_free_result(res);
	
	return(0);

}//int fileStructTemplateWithQuery()


int fileStructTemplateInterfaceWithQuery(FILE *fp, char *cTemplateName,struct t_template *ptrTemplate,
		unsigned uTemplateSet, unsigned uTemplateType)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	if(!fp) return(1);
	if(!ptrTemplate) return(1);
	if(!cTemplateName[0]) return(1);

	TemplateSelectInterfaceWithQuery(cTemplateName,uTemplateSet,uTemplateType);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
			if(field[1][0] && TemplatePairsQuery(field[1],ptrTemplate))
			{
				fprintf(stderr,"TemplatePairsQuery() non SQL error\n");
        			if(mysql_errno(&gMysql))
                			fprintf(stderr,"%s\n",mysql_error(&gMysql));
				return(0);
			}
			
			Template(field[0],ptrTemplate,fp);
	}
	else
	{
			fprintf(fp,"Template %s not found\n",cTemplateName);
	}
	mysql_free_result(res);
	
	return(0);

}//int fileStructTemplateInterfaceWithQuery()


//Pass the template contents in one buffer: cTemplate
//Variables {{cLikeThis}} in the cTemplate are replaced if t_template
//setup correctly.
void Template(char *cTemplate, struct t_template *template, FILE *fp)
{
	register int i,uState=0,j=0,k=0;
	char cVarName[256]={""};
	char cVarNameS1[1024]={""};

	for(i=0;cTemplate[i];i++)
	{
		if(cTemplate[i]=='{' && uState==0 && cTemplate[i+1]=='{')
		{
			uState=1;
			continue;
		}
		else if(cTemplate[i]=='{' && uState==1 )
		{
			uState=2;
			continue;
		}
		else if(cTemplate[i]=='}' && uState==2 )
		{
			uState=3;
			continue;
		}
		else if(cTemplate[i]=='}' && uState==1 )
		{
			uState=0;
			cVarNameS1[k]=0;
			fprintf(fp,"{%s}",cVarNameS1);
			k=0;
			continue;
		}
		else if(cTemplate[i]=='}' && uState==3 )
		{
			register int n=0,uMatch=0;

			uState=0;
			cVarName[j]=0;
			while(template->cpName[n][0])
			{
				if(!strcmp(template->cpName[n],cVarName))
				{
					fprintf(fp,"%s",template->cpValue[n]);
					uMatch=1;
				}
				n++;
				if(n>99) break;
			}
			if(!uMatch)
			{
				if(cVarName[0]=='f')
				{
					//Every application that links to this lib
					//needs to supply this function
					AppFunctions(fp,cVarName);
				}
			}
			j=0;
			continue;
		}
		if(uState==1)
		{
			if(k>1023) return;
			cVarNameS1[k++]=cTemplate[i];
			continue;
		}
		else if(uState==2)
		{
			if(j>254) return;
			cVarName[j++]=cTemplate[i];
			continue;
		}
		fputc((int)cTemplate[i],fp);
	}

}//int Template()


int TemplatePairsQuery(char *cSQLQuery,struct t_template *template)
{
	static MYSQL_RES *res;
	static MYSQL_ROW field;
	static MYSQL_FIELD *column;
	
	register int i;

	mysql_query(&gMysql,cSQLQuery);
	if(mysql_errno(&gMysql))
		return(1);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		for(i=0;i<mysql_num_fields(res);i++)
		{
			template->cpValue[i]=field[i];
			column = mysql_fetch_field_direct(res,i);

			template->cpName[i]=malloc(strlen(column->table)+strlen(column->name)+3);
			strcpy(template->cpName[i],column->table);
			strcat(template->cpName[i],".");
			strcat(template->cpName[i],column->name);
		}
		template->cpName[i]="";//End list!
	}
	else
	{
		return(2);
	}
	
	return(0);

}//int TemplatePairsQuery()


//Returns rows via &gMysql
void TemplateSelect(char *cTemplateName)
{
	char cQuery[1024];

		sprintf(cQuery,"SELECT cTemplate FROM tTemplate WHERE cLabel='%s'"
				,cTemplateName);
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
                fprintf(stderr,"%s\n",mysql_error(&gMysql));

}//void TemplateSelect(char *cTemplateName)


//Returns rows via &gMysql
void TemplateSelectInterface(char *cTemplateName,unsigned uTemplateSet,unsigned uTemplateType)
{
	char cQuery[1024];

	sprintf(cQuery,"SELECT cTemplate FROM tTemplate WHERE cLabel='%s' AND"
				" uTemplateSet=%u AND uTemplateType=%u",cTemplateName,uTemplateSet,uTemplateType);
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
                fprintf(stderr,"%s\n",mysql_error(&gMysql));

}//void TemplateSelectInterface()


//Output to FILE fp the template with name/value pair substitution done via
//t_template 
int fileStructTemplate(FILE *fp,char *cTemplateName,struct t_template *ptrTemplate,char *cQuery)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	if(!fp) return(1);
	if(!ptrTemplate) return(1);
	if(!cTemplateName[0]) return(1);

	TemplateSelect(cTemplateName);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
			if(TemplatePairsQuery(cQuery,ptrTemplate))
			{
				fprintf(stderr,"TemplatePairsQuery() non SQL error\n");
        			if(mysql_errno(&gMysql))
                			fprintf(stderr,"%s\n",mysql_error(&gMysql));
				return(0);
			}
			
			Template(field[0],ptrTemplate,fp);
	}
	else
	{
			fprintf(fp,"Template %s not found\n",cTemplateName);
	}
	mysql_free_result(res);
	
	return(0);

}//int fileStructTemplate()


//Output to FILE fp the template with name/value pair substitution done via
//t_template 
int fileStructTemplateInterface(FILE *fp,char *cTemplateName,struct t_template *ptrTemplate,char *cQuery,
			unsigned uTemplateSet,unsigned uTemplateType)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	if(!fp) return(1);
	if(!ptrTemplate) return(1);
	if(!cTemplateName[0]) return(1);

	TemplateSelectInterface(cTemplateName,uTemplateSet,uTemplateType);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
			if(TemplatePairsQuery(cQuery,ptrTemplate))
			{
				fprintf(stderr,"TemplatePairsQuery() non SQL error\n");
        			if(mysql_errno(&gMysql))
                			fprintf(stderr,"%s\n",mysql_error(&gMysql));
				return(0);
			}
			
			Template(field[0],ptrTemplate,fp);
	}
	else
	{
			fprintf(fp,"Template %s not found\n",cTemplateName);
	}
	mysql_free_result(res);
	
	return(0);

}//int fileStructTemplateInterface()


//Extreme danger with this alpha test code
//External code must check and/or allocate the correct size for cBuffer
void TemplateToBuffer(char *cTemplate, struct t_template *template, char *cBuffer)
{
	register int i,uState=0,j=0,k=0,l=0;
	char cVarName[256]={""};
	char cVarNameS1[1024]={""};

	for(i=0;cTemplate[i];i++)
	{
		if(cTemplate[i]=='{' && uState==0 )
		{
			uState=1;
			continue;
		}
		else if(cTemplate[i]=='{' && uState==1 )
		{
			uState=2;
			continue;
		}
		else if(cTemplate[i]=='}' && uState==2 )
		{
			uState=3;
			continue;
		}
		else if(cTemplate[i]=='}' && uState==1 )
		{
			uState=0;
			cVarNameS1[k]=0;
			//fprintf(fp,"{%s}",cVarNameS1);
			sprintf(cBuffer+l,"{%s}",cVarNameS1);
			l+=strlen(cVarNameS1)+2;
			k=0;
			continue;
		}
		else if(cTemplate[i]=='}' && uState==3 )
		{
			register int n=0,uMatch=0;

			uState=0;
			cVarName[j]=0;
			while(template->cpName[n][0])
			{
				if(!strcmp(template->cpName[n],cVarName))
				{
					//fprintf(fp,"%s",template->cpValue[n]);
					sprintf(cBuffer+l,"%s",template->cpValue[n]);
					l+=strlen(template->cpValue[n]);
					uMatch=1;
				}
				n++;
				if(n>99) break;
			}
			j=0;
			continue;
		}
		if(uState==1)
		{
			if(k>1023) return;
			cVarNameS1[k++]=cTemplate[i];
			continue;
		}
		else if(uState==2)
		{
			if(j>254) return;
			cVarName[j++]=cTemplate[i];
			continue;
		}
		//fputc((int)cTemplate[i],fp);
		cBuffer[l++]=cTemplate[i];
	}

}//int TemplateToBuffer()
