/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis for Unixservice.
 
*/

//ModuleFunctionProtos()


static char *sgcBuildInfo=dsGitVersion;

void tProjectNavList(void);
void tProjectTableNavList(void);
void tProjectTableFieldNavList(void);
void AddTableFromDefaultTablesLine(char *cLine);
void AddDefaultTables(unsigned uProject);
void RemoveDefaultTables(unsigned uProject);
void RemoveTableFromDefaultTablesLine(char *cLine);
void GetConfiguration(const char *cName,char *cValue,unsigned uValueSize, unsigned uServer, unsigned uHtml);
unsigned CreateMakeSourceCodeJob(unsigned uProject,unsigned uServer,char *cProject);

void ExtProcesstProjectVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstProjectVars(pentry entries[], int x)


void ExttProjectCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tProjectTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstProjectVars(entries,x);
                        	guMode=2000;
	                        tProject(LANG_NB_CONFIRMNEW);
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstProjectVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uProject=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtProject(0);
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstProjectVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tProject(LANG_NB_CONFIRMDEL);
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstProjectVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetProject();
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstProjectVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tProject(LANG_NB_CONFIRMMOD);
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstProjectVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtProject();
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,"Select"))
                {
                        ProcesstProjectVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(uProject)
				{
					guCookieProject=uProject;
					guCookieTable=0;
					guCookieField=0;
					SetSessionCookie();
					tProject("This project selected for workflow");
				}
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Add/Update Default Tables"))
                {
                        ProcesstProjectVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(uProject)
					AddDefaultTables(uProject);
				tProject("Template tables added or updated");
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Remove Default Tables"))
                {
                        ProcesstProjectVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(uProject)
					RemoveDefaultTables(uProject);
				tProject("Template tables removed");
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Make Source Code"))
                {
                        ProcesstProjectVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(uProject)
				{
					if(CreateMakeSourceCodeJob(uProject,0,cLabel))//0 is any server
						tProject("A make source code job not done or canceled already exists for this project");
					else
						tProject("Make source code job created");
				}
				else
					tProject("<blink>Error</blink>: No project selected");
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
		}
	}

}//void ExttProjectCommands(pentry entries[], int x)


void ExttProjectButtons(void)
{
	OpenFieldSet("tProject Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter/mod data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review changes</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			printf("<u>Summary</u><br>");
			printf("Here we manage projects. Projects consists of a templates, tables,"
				" fields and their properties. Source code and even ready to run applications can be"
				" generated here if all the templates required exist."
				"<br>(<a target=github href=https://github.com/unxs0/unxsVZ/commit/%1$s>%1$s</a>).",sgcBuildInfo);
			printf("<p><u>Record Context Info</u><br>");
			if(guCookieProject && guCookieProject!=uProject)
				printf("Loaded is not the workflow project.");
			else if(guCookieProject && guCookieProject==uProject)
				printf("Loaded project belongs to current workflow project.");
			else if(!guCookieProject)
				printf("No workflow project has been selected");
			else if(!uProject)
				printf("No project has been created");
			printf("<p><u>Operations</u><br>");
			printf("<input type=submit class=largeButton"
				" title='Select and keep this project marked for current work flow. Releases any saved table and field'"
				" name=gcCommand value='Select'>");
			printf("<br><input type=submit class=largeButton"
				" title='Add or update to loaded project the list of default tables as defined per tTemplateSet"
				" and tConfiguration settings'"
				" name=gcCommand value='Add/Update Default Tables'>");
			printf("<br><input type=submit class=largeButton"
				" title='Remove from  loaded project the list of default tables as defined per tTemplateSet"
				" and tConfiguration settings'"
				" name=gcCommand value='Remove Default Tables'>");
			printf("<p><input type=submit class=largeButton"
				" title='Create a make source code job for loaded project. See tJob for status. Depending on tConfiguration"
				" settings the project may also be compiled and installed'"
				" name=gcCommand value='Make Source Code'>");
			tProjectNavList();
			tProjectTableNavList();
			tProjectTableFieldNavList();
	}
	CloseFieldSet();

}//void ExttProjectButtons(void)


void ExttProjectAuxTable(void)
{

}//void ExttProjectAuxTable(void)


void ExttProjectGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uProject"))
		{
			sscanf(gentries[i].val,"%u",&uProject);
			guMode=6;
		}
		else if(guCookieProject)
		{
			uProject=guCookieProject;
			guMode=7;
		}
	}
	tProject("");

}//void ExttProjectGetHook(entry gentries[], int x)


void ExttProjectSelect(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tProject ORDER BY"
				" uProject",
				VAR_LIST_tProject);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tProject,tClient WHERE tProject.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uProject",
					VAR_LIST_tProject,uContactParentCompany,uContactParentCompany);
					

}//void ExttProjectSelect(void)


void ExttProjectSelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tProject WHERE uProject=%u",
			VAR_LIST_tProject,uProject);
	else
                sprintf(gcQuery,"SELECT %s FROM tProject,tClient"
                                " WHERE tProject.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tProject.uProject=%u",
                        		VAR_LIST_tProject
					,uContactParentCompany,uContactParentCompany
					,uProject);

}//void ExttProjectSelectRow(void)


void ExttProjectListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tProject",
				VAR_LIST_tProject);
	else
		sprintf(gcQuery,"SELECT %s FROM tProject,tClient"
				" WHERE tProject.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tProject
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttProjectListFilter()
        if(!strcmp(gcFilter,"uProject"))
        {
                sscanf(gcCommand,"%u",&uProject);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tProject.uProject=%u"
						" ORDER BY uProject",
						uProject);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uProject");
        }

}//void ExttProjectListSelect(void)


void ExttProjectListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uProject"))
                printf("<option>uProject</option>");
        else
                printf("<option selected>uProject</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttProjectListFilter(void)


void ExttProjectNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
		printf(LANG_NBB_NEW);

	if(uAllowMod(uOwner,uCreatedBy))
		printf(LANG_NBB_MODIFY);

	if(uAllowDel(uOwner,uCreatedBy)) 
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttProjectNavBar(void)


void tProjectNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uProject,cLabel FROM tProject ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tProject.uProject,"
				" tProject.cLabel"
				" FROM tProject,tClient"
				" WHERE tProject.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tProjectNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		char *cColor;
        	printf("<p><u>tProjectNavList</u><br>\n");
	        while((field=mysql_fetch_row(res)))
		{
			if(atoi(field[0])==uProject)
				cColor="blue";
			else
				cColor="black";
			printf("<a class=darkLink href=unxsRAD.cgi?gcFunction=tProject"
				"&uProject=%s><font color=%s>%s</font></a><br>\n",
				field[0],cColor,field[1]);
		}
	}
        mysql_free_result(res);

}//void tProjectNavList(void)


void tProjectTableNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(!uProject) return;

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uTable,cLabel,uTableOrder FROM tTable WHERE uProject=%u ORDER BY uTableOrder,cLabel",uProject);
	else
		sprintf(gcQuery,"SELECT tTable.uTable,"
				" tTable.cLabel,tTable.uTableOrder"
				" FROM tTable,tClient"
				" WHERE tTable.uOwner=tClient.uClient"
				" AND tTable.uProject=%u"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY tTable.uTableOrder,tTable.cLabel",
					uProject,guCompany,guLoginClient);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tTableNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
		char *cColor;
        	printf("<p><u>tTableNavList</u><br>\n");
	        while((field=mysql_fetch_row(res)))
		{
			if(atoi(field[0])==guCookieTable)
				cColor="blue";
			else
				cColor="black";
			printf("<a class=darkLink href=unxsRAD.cgi?gcFunction=tTable"
				"&uTable=%s><font color=%s>%s/%s</font></a><br>\n",
					field[0],cColor,field[1],field[2]);
		}
	}
        mysql_free_result(res);

}//void tProjectTableNavList(void)


void tProjectTableFieldNavList(void)
{
	if(!guCookieTable) return;

        MYSQL_RES *res;
        MYSQL_ROW field;
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uField,cLabel FROM tField WHERE uTable=%u ORDER BY uOrder,cLabel",guCookieTable);
	else
		sprintf(gcQuery,"SELECT tField.uField,"
				" tField.cLabel"
				" FROM tField,tClient"
				" WHERE tField.uOwner=tClient.uClient"
				" AND tField.uTable=%u"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY tField.uOrder,tField.cLabel",
					guCookieTable,guCompany,guLoginClient);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tFieldNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tFieldNavList</u><br>\n");
		char *cColor;
	        while((field=mysql_fetch_row(res)))
		{	
			if(atoi(field[0])==guCookieField)
				cColor="blue";
			else
				cColor="black";
			printf("<a class=darkLink href=unxsRAD.cgi?gcFunction=tField"
				"&uField=%s><font color=%s>%s</font></a><br>\n",
					field[0],cColor,field[1]);
		}
	}
        mysql_free_result(res);

}//void tProjectTableFieldNavList(void)


char *ParseTextAreaLines(char *cTextArea)
{
	static unsigned uEnd=0;
	static unsigned uStart=0;
	static char cRetVal[512];
	
	uStart=uEnd;
	while(cTextArea[uEnd++])
	{
		if(cTextArea[uEnd]=='\n' || cTextArea[uEnd]=='\r' || cTextArea[uEnd]==0
				|| cTextArea[uEnd]==10 || cTextArea[uEnd]==13 )
		{
			if(cTextArea[uEnd]==0)
				break;

			cTextArea[uEnd]=0;
			sprintf(cRetVal,"%.511s",cTextArea+uStart);

			if(cRetVal[0]=='\n' || cRetVal[0]==13)
			{
				uStart=uEnd=0;
				return("");
			}

			if(cTextArea[uEnd+1]==10)
				uEnd+=2;
			else
				uEnd++;

			return(cRetVal);
		}
	}

	if(uStart!=uEnd)
	{
		sprintf(cRetVal,"%.511s",cTextArea+uStart);
		return(cRetVal);
	}

	uStart=uEnd=0;
	return("");

}//char *ParseTextAreaLines(char *cTextArea)


void RemoveDefaultTables(unsigned uProject)
{
	char cValue[2048];
	char cLine[256];

        sprintf(cLine,"cDefaultTablesList_TemplateSet_%s",ForeignKey("tTemplateSet","cLabel",uTemplateSet));
	GetConfiguration(cLine,cValue,2047,0,1);

	while(1)
	{
		sprintf(cLine,"%.255s",ParseTextAreaLines(cValue));
		if(cLine[0]=='#') continue;
		if(cLine[0]==' ') continue;
		if(!cLine[0]) break;
		RemoveTableFromDefaultTablesLine(cLine);
	}

}//void RemoveDefaultTables(unsigned uProject)


void AddDefaultTables(unsigned uProject)
{
	char cValue[2048];
	char cLine[256];

        sprintf(cLine,"cDefaultTablesList_TemplateSet_%s",ForeignKey("tTemplateSet","cLabel",uTemplateSet));
	GetConfiguration(cLine,cValue,2047,0,1);

	while(1)
	{
		sprintf(cLine,"%.255s",ParseTextAreaLines(cValue));
		if(cLine[0]=='#') continue;
		if(cLine[0]==' ') continue;
		if(!cLine[0]) break;
		AddTableFromDefaultTablesLine(cLine);
	}

}//void AddDefaultTables(unsigned uProject)


void RemoveTableFromDefaultTablesLine(char *cLine)
{

	char cTable[32]={""};
	unsigned uTable=0;
	int iCount=0;

	//tClient;1000;1;Organizations and their contacts;7;7;7;7;
	iCount=sscanf(cLine,"%31[a-zA-Z0-9\\.];%*u;%*u;%*99[a-zA-Z0-9 ];%*u;%*u;%*u;%*u;",cTable);
	if(!cTable[0] || iCount!=1)
	{
		char gcQuery[512];
		sprintf(gcQuery,"Error1 %s",cLine);
		tProject(gcQuery);
	}

        MYSQL_RES *res;
        MYSQL_ROW field;
	sprintf(gcQuery,"SELECT uTable FROM tTable WHERE cLabel='%.32s' AND uProject=%u",cTable,uProject);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        	htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uTable);
        mysql_free_result(res);


	if(uTable)
	{
		sprintf(gcQuery,"DELETE FROM tTable WHERE uTable=%u",uTable);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
        	        htmlPlainTextError(mysql_error(&gMysql));
	}

}//void RemoveTableFromDefaultTablesLine(char cLine)


void AddTableFromDefaultTablesLine(char *cLine)
{

	char cTable[32]={""};
	unsigned uOrder=0;
	unsigned uSourceLock=0;
	char cDescription[100]={""};
	char cSubDir[100]={""};
	unsigned uNewLevel=0;
	unsigned uModLevel=0;
	unsigned uDelLevel=0;
	unsigned uReadLevel=0;
	unsigned uTable=0;
	int iCount=0;

	//tClient;1000;1;Organizations and their contacts;7;7;7;7;
	iCount=sscanf(cLine,"%31[a-zA-Z0-9\\.];%u;%u;%99[a-zA-Z0-9 ];%u;%u;%u;%u;%99[a-zA-Z0-9\\./];",cTable,&uOrder,&uSourceLock,
						cDescription,&uNewLevel,&uModLevel,&uDelLevel,&uReadLevel,cSubDir);
	if(!cTable[0] || iCount<8)
	{
		char gcQuery[512];
		sprintf(gcQuery,"Error2 iCount=%d %s;%u;%u;%s;%u;%u;%u;%u; (%s)",
				iCount,cTable,uOrder,uSourceLock,cDescription,uNewLevel,uModLevel,uDelLevel,uReadLevel,cLine);
		tProject(gcQuery);
	}

        MYSQL_RES *res;
        MYSQL_ROW field;
	sprintf(gcQuery,"SELECT uTable FROM tTable WHERE cLabel='%.32s' AND uProject=%u",cTable,uProject);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        	htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uTable);
        mysql_free_result(res);


	if(uTable)
	{
		sprintf(gcQuery,"UPDATE tTable SET cLabel='%.32s',uTableOrder=%u,uSourceLock=%u,"
			"cDescription='%.100s',cSubDir='%.100s',"
			"uNewLevel=%u,uModLevel=%u,uDelLevel=%u,uReadLevel=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uTable=%u",
			cTable,uOrder,uSourceLock,
			cDescription,cSubDir,
			uNewLevel,uModLevel,uDelLevel,uReadLevel,guLoginClient,uTable);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
        	        htmlPlainTextError(mysql_error(&gMysql));
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tTable SET cLabel='%.32s',uTableOrder=%u,uSourceLock=%u,"
			"cDescription='%.100s',cSubDir='%.100s',"
			"cLegend='%.100s',cToolTip='%.100s',"
			"uNewLevel=%u,uModLevel=%u,uDelLevel=%u,uReadLevel=%u,uProject=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			cTable,uOrder,uSourceLock,
			cDescription,cSubDir,
			cDescription,cDescription,
			uNewLevel,uModLevel,uDelLevel,uReadLevel,uProject,guCompany,guLoginClient);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
        	        htmlPlainTextError(mysql_error(&gMysql));
	}


}//void AddTableFromDefaultTablesLine(char cLine)


void GetConfiguration(const char *cName,char *cValue,unsigned uValueSize, unsigned uServer, unsigned uHtml)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        char cQuery[1024];
	char cExtra[100]={""};

        sprintf(cQuery,"SELECT cValue,cComment FROM tConfiguration WHERE cLabel='%s'",
			cName);
	if(uServer)
	{
		sprintf(cExtra," AND uServer=%u",uServer);
		strcat(cQuery,cExtra);
	}
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
        	        htmlPlainTextError(mysql_error(&gMysql));
		else
        	        TextError(mysql_error(&gMysql),0);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		char cFormat[16];
        	sprintf(cFormat,"%%.%us",uValueSize-1);
        	sprintf(cValue,cFormat,field[0]);
		if(!strncmp(cValue,"cComment",8))
        		sprintf(cValue,cFormat,field[1]);
	}
        mysql_free_result(res);

}//void GetConfiguration()


unsigned CreateMakeSourceCodeJob(unsigned uProject,unsigned uServer,char *cProject)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uJob=0;

	//3 done ok, 7 canceled
	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE cLabel='MakeSourceCode.%.17s' AND uServer=%u AND uJobStatus!=3 AND uJobStatus!=7",
					cProject,uServer);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        	htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uJob);
        mysql_free_result(res);

	if(uJob)
	{
		return(uJob);
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tJob SET cLabel='MakeSourceCode.%.17s',"
			"cJobName='MakeSourceCodeJob',"
			"cJobData='uProject=%u;',"
			"uServer=%u,uOwner=%u,uCreatedBy=%u,uJobStatus=1,"//Waiting
			"uCreatedDate=UNIX_TIMESTAMP(NOW()),uJobDate=UNIX_TIMESTAMP(NOW())+60",
						cProject,uProject,uServer,guCompany,guLoginClient);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
        	        htmlPlainTextError(mysql_error(&gMysql));
	}
	return(0);
}//unsigned CreateMakeSourceCodeJob()
