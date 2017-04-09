/*
FILE 
	unxsRAD/interfaces/bootstrap/project.c
AUTHOR/LEGAL
	(C) 2010-2017 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	For developing bootstrap/footable templates
	for unxsRAD RAD$ generated web apps.
*/

#include "interface.h"

//TOC
void ProcessProjectVars(pentry entries[], int x);
void ProjectGetHook(entry gentries[],int x);
void ProjectCommands(pentry entries[], int x);
void htmlProject(void);
void htmlProjectPage(char *cTitle, char *cTemplateName);
void jsonProjectRows(void);
void jsonProjectCols(void);
void jsonTableRows(char const *cTable);

static char cName[64]={""};
static char cStatus[64]={""};
static char cTemplate[64]={""};
unsigned uProject=0;

void ProcessProjectVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cName"))
			sprintf(cName,"%.63s",entries[i].val);
		else if(!strcmp(entries[i].name,"cStatus"))
			sprintf(cStatus,"%.63s",entries[i].val);
		else if(!strcmp(entries[i].name,"cTemplate"))
			sprintf(cTemplate,"%.63s",entries[i].val);
		else if(!strcmp(entries[i].name,"uProject"))
			sscanf(entries[i].val,"%u",&uProject);
	}

}//void ProcessProjectVars(pentry entries[], int x)


void ProjectGetHook(entry gentries[],int x)
{
	//register int i;
	//for(i=0;i<x;i++)
	//{
	//}

	if(!strcmp(gcFunction,"ProjectRows"))
		jsonProjectRows();
	else if(!strcmp(gcFunction,"ProjectCols"))
		jsonProjectCols();
	htmlProject();

}//void ProjectGetHook(entry gentries[],int x)


void ProjectCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Project"))
	{
		ProcessProjectVars(entries,x);
		htmlProject();
	}
	if(!strcmp(gcFunction,"ProjectRows"))
		jsonProjectRows();
	else if(!strcmp(gcFunction,"ProjectCols"))
		jsonProjectCols();
	else if(!strcmp(gcFunction,"AddProject"))
	{
		ProcessProjectVars(entries,x);
		printf("Content-type: text/plain\n\n");
		printf("cName: %s; cStatus: %s; cTemplate: %s; uProject: %u;\n",cName,cStatus,cTemplate,uProject);
		exit(0);
	}

}//void ProjectCommands(pentry entries[], int x)


void htmlProject(void)
{
	htmlHeader("Project","Project.Header");
	htmlProjectPage("Project","Project.Body");
	htmlFooter("Project.Footer");

}//void htmlProject(void)


void htmlProjectPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uPLAINSET,uOneLogin);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="mobile.cgi";
			
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

			template.cpName[8]="gcBrand";
			template.cpValue[8]=INTERFACE_HEADER_TITLE;

			template.cpName[9]="gcCopyright";
			template.cpValue[9]=INTERFACE_COPYRIGHT;

			template.cpName[10]="";

			printf("\n<!-- Start htmlProjectPage(%s) -->\n",cTemplateName); 
			Template(field[0],&template,stdout);
			printf("\n<!-- End htmlProjectPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlProjectPage()


void jsonProjectRows(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cQuery[1028]={""};

	printf("Content-type: text/json\n\n");
	printf("[\n");

	sprintf(cQuery,"SELECT tProject.uProject,tProject.cLabel,tProjectStatus.cLabel,tTemplateSet.cLabel"
			" FROM tProject,tProjectStatus,tTemplateSet"
			" WHERE tProject.uProjectStatus=tProjectStatus.uStatus"
			" AND tProject.uTemplateSet=tTemplateSet.uTemplateSet"
			" AND tProject.uOwner=%u",guOrg);
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
	{
		printf("\t{\n");
		printf("\t\t'status' : 'error',\n");
		printf("\t\t'message' : 'error: %s',\n",mysql_error(&gMysql));
		printf("\t}\n");
		printf("]\n");
		exit(0);
	}
	res=mysql_store_result(&gMysql);
	unsigned uNumRows=0;
	if((uNumRows=mysql_num_rows(res))>0)
	{
		unsigned uLast=0;
		while((field=mysql_fetch_row(res)))
		{
			printf("\t{");
			printf("\"uProject\": \"%s\","
				" \"cName\": \"%s\","
				" \"cStatus\": \"%s\","
				" \"cTemplate\": \"%s\""
					,field[0],field[1],field[2],field[3]);
			printf("}");
			if((++uLast)<uNumRows)
				printf(",\n");
			else
				printf("\n");
		}
	}
	mysql_free_result(res);
	printf("]\n");
	exit(0);

}//void jsonProjectRows(void)


void jsonProjectCols(void)
{
	printf("Content-type: text/json\n\n");
	printf("[\n");
	printf("\t{\"name\": \"uProject\", \"title\": \"Unique Project ID\"},\n");
	printf("\t{\"name\": \"cName\", \"title\": \"Name\"},\n");
	printf("\t{\"name\": \"cStatus\", \"title\": \"Status\"},\n");
	printf("\t{\"name\": \"cTemplate\", \"title\": \"Template\", \"breakpoints\": \"xs sm\"}\n");
	printf("]\n");
	exit(0);
}//void jsonProjectCols(void)


void jsonTableRows(char const *cTable)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cQuery[1028]={""};
	char cColumnName[64][64];
	unsigned uColumn=0;

	printf("Content-type: text/json\n\n");
	printf("[\n");

	sprintf(cQuery,"SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS"
					" WHERE TABLE_SCHEMA='unxsrad' AND TABLE_NAME='%s';",cTable);
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
	{
		printf("\t{\n");
		printf("\t\t'status' : 'error',\n");
		printf("\t\t'message' : 'error: %s',\n",mysql_error(&gMysql));
		printf("\t}\n");
		printf("]\n");
		exit(0);
	}
	res=mysql_store_result(&gMysql);
	unsigned uNumRows=0;
	if((uNumRows=mysql_num_rows(res))>0)
	{
		while((field=mysql_fetch_row(res)) && uColumn<64)
		{
			sprintf(cColumnName[uColumn],"%.63s",field[0]);
			uColumn++;
		}
	}
	mysql_free_result(res);

	sprintf(cQuery,"SELECT * FROM %.99s",cTable);
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
	{
		printf("\t{\n");
		printf("\t\t'status' : 'error',\n");
		printf("\t\t'message' : 'error: %s',\n",mysql_error(&gMysql));
		printf("\t}\n");
		printf("]\n");
		exit(0);
	}
	res=mysql_store_result(&gMysql);
	uNumRows=0;
	if((uNumRows=mysql_num_rows(res))>0)
	{
		unsigned uLast=0;
		unsigned register i;
		unsigned uFirst;
		while((field=mysql_fetch_row(res)))
		{
			printf("\t{");
			uFirst=1;
			for(i=0;i<uColumn;i++)
			{
				if(!uFirst)
					printf(", ");
				printf("\"%s\": \"%s\"",cColumnName[i],field[i]);
				uFirst=0;
			}
			printf("}");
			if((++uLast)<uNumRows)
				printf(",\n");
			else
				printf("\n");
		}
	}
	mysql_free_result(res);
	printf("]\n");
	exit(0);

}//void jsonTableRows(char const *cTable)

