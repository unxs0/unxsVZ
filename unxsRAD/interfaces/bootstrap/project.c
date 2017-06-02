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
static unsigned uProject=0;

void ProcessProjectVars(pentry entries[], int x)
{
	register int i;
	
	char *cp;
	for(i=0;i<x;i++)
	{
		//no sql injection
		if((cp=strchr(entries[i].val,'\''))) *cp=0;

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

	//API Get
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

	//API Post
        MYSQL_RES *res;
	MYSQL_ROW field;
	if(!strcmp(gcFunction,"ProjectRows"))
		jsonProjectRows();
	else if(!strcmp(gcFunction,"ProjectCols"))
		jsonProjectCols();
	else if(!strcmp(gcFunction,"AddProject") || !strcmp(gcFunction,"ModProject"))
	{
		ProcessProjectVars(entries,x);
		printf("Content-type: text/plain\n\n");
		if(guPermLevel<7)
		{
			printf("insufficient permissions for add/mod\n");
			exit(0);
		}
		if(cName[0] && cStatus[0] && cTemplate[0])
		{
			unsigned uProjectStatus=0;
			unsigned uTemplateSet=0;
			sprintf(gcQuery,"SELECT uStatus FROM tProjectStatus WHERE cLabel='%.31s'",cStatus);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sscanf(field[0],"%u",&uProjectStatus);
			if(!uProjectStatus)
			{
				printf("No such project status: %s\n",cStatus);
				exit(0);
			}
			sprintf(gcQuery,"SELECT uTemplateSet FROM tTemplateSet WHERE cLabel='%.31s'",cTemplate);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sscanf(field[0],"%u",&uTemplateSet);
			if(!uTemplateSet)
			{
				printf("No such template set: %s\n",cTemplate);
				exit(0);
			}
			if(uProject)
			{
				sprintf(gcQuery,"SELECT uProject FROM tProject WHERE uProject=%u"
					" AND (uCreatedBy=%u OR (uOwner=%u AND %u>=10))",
						uProject,guLoginClient,guOrg,guPermLevel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(0);
				}
				res=mysql_store_result(&gMysql);
				uProject=0;
				if((field=mysql_fetch_row(res)))
				{
					sscanf(field[0],"%u",&uProject);
				}
				else
				{
					printf("Insufficient permission to modify project %u\n",uProject);
					exit(0);
				}
			}
			if(!uProject)
			{
				sprintf(gcQuery,"INSERT INTO tProject SET cLabel='%.31s',"
					"uProjectStatus=%u,"
					"uTemplateSet=%u,"
					"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						,cName,uProjectStatus,uTemplateSet,guOrg,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(0);
				}
				printf("%llu\n",mysql_insert_id(&gMysql));
			}
			else
			{
				sprintf(gcQuery,"UPDATE tProject SET cLabel='%.31s',"
					"uProjectStatus=%u,"
					"uTemplateSet=%u,"
					"uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
					" WHERE uProject=%u"
						,cName,uProjectStatus,uTemplateSet,guLoginClient,uProject);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(0);
				}
				printf("%u\n",uProject);
			}
		}
		else
		{
			printf("No cName etc.\n");
		}
		exit(0);
	}
	else if(!strcmp(gcFunction,"DelProject"))
	{
		ProcessProjectVars(entries,x);

		printf("Content-type: text/plain\n\n");
		if(guPermLevel<10)
		{
			printf("insufficient permissions for delete\n");
			exit(0);
		}
		if(uProject && cName[0])
		{
			sprintf(gcQuery,"SELECT uProject FROM tTable WHERE uProject=%u",uProject);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				printf("Project has tables not deleted\n");
				exit(0);
			}
			sprintf(gcQuery,"SELECT uProject FROM tProject"
					" WHERE uProject=%u AND cLabel='%s'"
					" AND (uCreatedBy=%u OR (uOwner=%u AND %u>=10))",
						uProject,cName,guLoginClient,guOrg,guPermLevel);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(gcQuery,"DELETE FROM tProject"
					" WHERE uProject=%u AND cLabel='%s'"
					" AND (uCreatedBy=%u OR (uOwner=%u AND %u>=10))",
						uProject,cName,guLoginClient,guOrg,guPermLevel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(0);
				}
				if(mysql_affected_rows(&gMysql)>0)
					printf("%u\n",uProject);
				else
					printf("Project not deleted. Unexpected mysql_affected_rows() error\n");
			}
			else
			{
				printf("Project not deleted. Insuficient permissions. guOrg=%u guPermLevel=%u\n",guOrg,guPermLevel);
			}
		}
		else
		{
			printf("Project not found %u/%s\n",uProject,cName);
		}
		exit(0);
	}

}//void ProjectCommands(pentry entries[], int x)


void htmlProject(void)
{
	htmlHeader("Project","Default.Header");
	htmlProjectPage("Project","Project.Body");
	htmlFooter("Default.Footer");

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
	//printf("\t{\"name\": \"uProject\", \"title\": \"Unique Project ID\", \"filterable\": false },\n");
	printf("\t{\"name\": \"uProject\", \"title\": \"Project ID\" },\n");
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

