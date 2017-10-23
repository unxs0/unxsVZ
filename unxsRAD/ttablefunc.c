/*
FILE
	ttablefunc.c
PURPOSE
	Aux function code for RAD4 table data
AUTHOR
	(C) 2001-2017 Gary Wallis for Unixservice, LLC.
*/

//ModuleFunctionProtos()
void AddDefaultFields(void);

void htmlTemplateInfo(const char *cLabel);
void htmlDescriptionInfo(const char *cDescription);
void htmlLabelInfo(const char *cLabel);
char *ParseTextAreaLines(char *cTextArea);//see tprojectfunc.c
void RemoveTableFields(char *cValue);
void AddTableFields(char *cValue);
void RemoveTableFieldLine(char *cLine);
void AddTableFieldLine(char *cLine);
void ExportTableFields(void);

void tTableNavList(void);
void tTableFieldNavList(void);
void RemoveAllFields(unsigned uTable,unsigned uProject);


void RemoveInterfaceTables(char *cValue);
void AddInterfaceTables(char *cValue);
void RemoveInterfaceTableLine(char *cLine);
void AddInterfaceTableLine(char *cLine);
void ExportInterfaceTables(void);
void CopyAllFields(unsigned uTargetTable, unsigned uSourceTable);
void CopyFieldsFromSameNameTable(unsigned uTargetTable,unsigned uProject);


void ExtProcesstTableVars(pentry entries[], int x)
{
	//register int i;
	//for(i=0;i<x;i++)
	//{
	//	if(!strcmp(entries[i].name,"cImport"))
	//		cImport=entries[i].val;
	//}
}//void ExtProcesstTableVars(pentry entries[], int x)


void ExttTableCommands(pentry entries[], int x)
{

	char cTextarea[4096]={""};
	if(!strcmp(gcFunction,"tTableTools"))
	{
		//ModuleFunctionProcess()
		long unsigned uActualModDate=0;

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstTableVars(entries,x);
                        	guMode=2000;
				if(guCookieProject)
					uProject=guCookieProject;
	                        tTable(LANG_NB_CONFIRMNEW);
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstTableVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
				if(!cLabel[0])
					tTable("<blink>Error</blink>: No cLabel");
				if(strchr(cLabel,' '))
					tTable("<blink>Error</blink>: No spaces in cLabel allowed");
                        	guMode=0;

				uTable=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtTable(1);
				if(uTable)
				{
					guCookieTable=uTable;
					guCookieField=0;
					SetSessionCookie();
				}
				tTable("New table created and selected from workflow operations");
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				sscanf(ForeignKey("tTable","uModDate",uTable),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tTable("<blink>Error</blink>: This record was modified. Reload it.");
	                        guMode=2001;
				tTable(LANG_NB_CONFIRMDEL);
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				sscanf(ForeignKey("tTable","uModDate",uTable),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tTable("<blink>Error</blink>: This record was modified. Reload it.");
				guMode=5;
				//We need to delete all fields also!
				DeletetTable();
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				sscanf(ForeignKey("tTable","uModDate",uTable),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tTable("<blink>Error</blink>: This record was modified. Reload it.");
				guMode=2002;
				tTable(LANG_NB_CONFIRMMOD);
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				sscanf(ForeignKey("tTable","uModDate",uTable),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tTable("<blink>Error</blink>: This record was modified. Reload it.");
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtTable();
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,"Add standard fields"))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;
				sscanf(ForeignKey("tTable","uModDate",uTable),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tTable("<blink>Error</blink>: This record was modified. Reload it.");
				if(!uTable)
					tTable("<blink>Error</blink>: No uTable");
				if(!uProject)
					tTable("<blink>Error</blink>: No uProject");
				AddDefaultFields();
				tTable("Standard fields added");
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Remove all fields"))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=0;
				sscanf(ForeignKey("tTable","uModDate",uTable),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tTable("<blink>Error</blink>: This record was modified. Reload it.");
				if(!uTable)
					tTable("<blink>Error</blink>: No uTable");
				if(!uProject)
					tTable("<blink>Error</blink>: No uProject");
                        	guMode=101;
				tTable("Make sure");
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Confirm Remove"))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=101;
				sscanf(ForeignKey("tTable","uModDate",uTable),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tTable("<blink>Error</blink>: This record was modified. Reload it.");
				if(!uTable)
					tTable("<blink>Error</blink>: No uTable");
				if(!uProject)
					tTable("<blink>Error</blink>: No uProject");
                        	guMode=0;
				RemoveAllFields(uTable,uProject);
				tTable("All fields removed");
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,"Select"))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(uTable)
				{
					guCookieTable=uTable;
					guCookieField=0;
					SetSessionCookie();
					tTable("This table selected");
				}
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
		}
                else if(!strcmp(gcCommand,"Import"))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) || guPermLevel>=7)
			{
				guMode=10000;
				tTable("Import");
			}
			else
			{
				tTable("Error: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Add Table Fields"))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) || guPermLevel>=7)
			{
				guMode=10000;
				//check
				if(!cImport[0])
					tTable("Error: cImport empty");
				if(!uTable)
					tTable("Error: no uTable");
				if(!uProject)
					tTable("Error: no uProject");
				guMode=10001;
				sprintf(cTextarea,"%.4095s",cImport);
				AddTableFields(cTextarea);
				tTable("Added Table Fields");
			}
			else
			{
				tTable("Error: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Remove Table Fields"))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) || guPermLevel>=7)
			{
				guMode=10000;
				//check
				if(!cImport[0])
					tTable("Error: cImport empty");
				if(!uTable)
					tTable("Error: no uTable");
				if(!uProject)
					tTable("Error: no uProject");
				guMode=10001;
				sprintf(cTextarea,"%.4095s",cImport);
				RemoveTableFields(cTextarea);
				tTable("Removed Table Fields");
			}
			else
			{
				tTable("Error: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Export Table Fields"))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) || guPermLevel>=7)
			{
				guMode=10000;
				//check
				if(!uTable)
					tTable("Error: no uTable");
				if(!uProject)
					tTable("Error: no uProject");
				if(!cImport[0])
				guMode=10001;
				ExportTableFields();
				tTable("Table Fields Exported");
			}
			else
			{
				tTable("Error: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Export Interface Tables"))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) || guPermLevel>=7)
			{
				guMode=10000;
				//check
				if(!uProject)
					tTable("Error: no uProject");
				if(!cImport[0])
				guMode=10001;
				ExportInterfaceTables();
				tTable("Tables Exported");
			}
			else
			{
				tTable("Error: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Add Interface Tables"))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) || guPermLevel>=7)
			{
				guMode=10000;
				//check
				if(!cImport[0])
					tTable("Error: cImport empty");
				if(!uProject)
					tTable("Error: no uProject");
				guMode=10001;
				sprintf(cTextarea,"%.4095s",cImport);
				AddInterfaceTables(cTextarea);
				tTable("Added Interface Tables");
			}
			else
			{
				tTable("Error: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Remove Interface Tables"))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) || guPermLevel>=7)
			{
				guMode=10000;
				//check
				if(!cImport[0])
					tTable("Error: cImport empty");
				if(!uProject)
					tTable("Error: no uProject");
				guMode=10001;
				sprintf(cTextarea,"%.4095s",cImport);
				RemoveInterfaceTables(cTextarea);
				tTable("Interface Tables Removed");
			}
			else
			{
				tTable("Error: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Copy Fields"))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) || guPermLevel>=7)
			{
                        	guMode=0;
				if(!uTable)
					tTable("No table");
				if(uTemplateType!=uTEMPLATETYPE_BOOTSTRAP)
					tTable("Not a boostrap table");
                        	guMode=201;
				tTable("Confirm Copy Fields");
			}
			else
			{
				tTable("Error: Denied by permissions settings");
			}
		}
                else if(!strcmp(gcCommand,"Confirm Copy Fields"))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) || guPermLevel>=7)
			{
                        	guMode=201;
				if(!uTable)
					tTable("No table");
				if(!uProject)
					tTable("No project");
				if(uTemplateType!=uTEMPLATETYPE_BOOTSTRAP)
					tTable("Not a boostrap table");
                        	guMode=0;
				CopyFieldsFromSameNameTable(uTable,uProject);
				tTable("Fields Copied");
			}
			else
			{
				tTable("Error: Denied by permissions settings");
			}
		}
	}
}//void ExttTableCommands(pentry entries[], int x)


void htmlLabelInfo(const char *cLabel)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char *cp;
	unsigned uNotFirst=0;
	if((cp=strchr(cLabel,'.')))
	{
		*cp=0;
		sprintf(gcQuery,"SELECT tTable.cLabel,tTemplateType.cLabel,tTable.uTable FROM tTable,tTemplateType"
			" WHERE tTable.cLabel LIKE '%%%s%%'"
			" AND tTable.uTable!=%u"
			" AND tTable.uTemplateType=tTemplateType.uTemplateType",cLabel,uTable);
		*cp='.';	
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			if(!uNotFirst++)
				printf("<br>Other tables that may be related:"
					"<br><a class=darkLink href=?gcFunction=tTable&uTable=%s>%s</a>/%s/%s\n",
				field[2],field[0],field[1],field[2]);
			else
				printf("<br><a class=darkLink href=?gcFunction=tTable&uTable=%s>%s</a>/%s/%s\n",
					field[2],field[0],field[1],field[2]);
		}
	}
	sprintf(gcQuery,"SELECT tTable.cLabel,tTemplateType.cLabel,tTable.uTable FROM tTable,tTemplateType"
			" WHERE tTable.cLabel LIKE '%%%s%%'"
			" AND tTable.uTable!=%u"
			" AND tTable.uTemplateType=tTemplateType.uTemplateType",cLabel+1,uTable);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	uNotFirst=0;
	while((field=mysql_fetch_row(res)))
	{
		if(!uNotFirst++)
			printf("<br>Other tables that may be related:<br><a class=darkLink href=?gcFunction=tTable&uTable=%s>%s</a>/%s/%s\n",
				field[2],field[0],field[1],field[2]);
		else
			printf("<br><a class=darkLink href=?gcFunction=tTable&uTable=%s>%s</a>/%s/%s\n",
				field[2],field[0],field[1],field[2]);
	}
        mysql_free_result(res);

}//void htmlLabelInfo(const char *cLabel)


void htmlDescriptionInfo(const char *cDescription)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uParentTable=0;

	sscanf(cDescription,"uParentTable=%u;",&uParentTable);
	if(!uParentTable) return;
	sprintf(gcQuery,"SELECT tTable.cLabel,tTemplateType.cLabel,tTable.uTable FROM tTable,tTemplateType"
			" WHERE tTable.uTable='%u'"
			" AND tTable.uTemplateType=tTemplateType.uTemplateType",uParentTable);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		printf("<br>Has parent table with: <a class=darkLink href=?gcFunction=tTable&uTable=%s>%s</a>/%s\n",
				field[2],field[0],field[1]);
	}
        mysql_free_result(res);

}//void htmlDescriptionInfo(const char *cDescription)


void htmlTemplateInfo(const char *cLabel)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tTemplateSet.cLabel,tTemplateType.cLabel FROM tTemplate,tTemplateSet,tTemplateType"
			" WHERE tTemplate.cLabel='%s'"
			" AND tTemplate.uTemplateSet=tTemplateSet.uTemplateSet"
			" AND tTemplate.uTemplateType=tTemplateType.uTemplateType",cLabel);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		printf("<br>Has matching template with: %s/%s\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void htmlTemplateInfo(const char *cLabel)


void ExttTableButtons(void)
{
	switch(guMode)
        {
		//Import Field
                case 10000:
                case 10001:
			OpenFieldSet("tTable Import Panel",100);
			if(cLabel[0] && uTable)
				printf("Back to tTable <a href=?gcFunction=tTable"
					"&uTable=%u>%s</a><br>\n",uTable,cLabel);
                        printf("<p><u>Import Operations</u><br>");
                        printf("<fieldset><legend>Fields</legend>");
			printf("<p><input title='Export current fields into cImport text area. Current contents will be lost!'"
					" type=submit class=largeButton"
					" name=gcCommand value='Export Table Fields'>\n");
			printf("<p><input title='Parse cImport and attempt to add fields to this table'"
					" type=submit class=largeButton"
					" name=gcCommand value='Add Table Fields'>\n");
			printf("<p><input title='Parse cImport and attempt to remove the fields from this table'"
					" type=submit class=largeButton"
					" name=gcCommand value='Remove Table Fields'>\n");
                        printf("</fieldset><fieldset><legend>Tables</legend>");
			printf("<p><input title='Export source code tables into cImport text area. Current contents will be lost!'"
					" type=submit class=largeButton"
					" name=gcCommand value='Export Interface Tables'>\n");
			printf("<p><input title='Parse cImport and attempt to add interface source code tables'"
					" type=submit class=largeButton"
					" name=gcCommand value='Add Interface Tables'>\n");
			printf("<p><input title='Parse cImport and attempt to remove interface source code tables'"
					" type=submit class=largeButton"
					" name=gcCommand value='Remove Interface Tables'>\n");
                        printf("</fieldset>\n");
			tTableFieldNavList();
                break;
                case 2000:
			OpenFieldSet("tTable Aux Panel",100);
			printf("<p><u>Enter/mod data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
			OpenFieldSet("tTable Aux Panel",100);
                        printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			OpenFieldSet("tTable Aux Panel",100);
			printf("<p><u>Review changes</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

                case 101:
			OpenFieldSet("tTable Aux Panel",100);
			printf("<p><u>No undo available</u><br>");
			printf("<br><input type=submit class=lwarnButton title='Remove all fields from the loaded table'"
				" name=gcCommand value='Confirm Remove'>");
                break;

                case 201:
			OpenFieldSet("tTable Aux Panel",100);
			printf("<p><u>Think twice!</u><br>");
			printf("<br><input type=submit class=lwarnButton title='Copy all fields from same name RAD4 table in this same project'"
				" name=gcCommand value='Confirm Copy Fields'>");
                break;

		default:
			OpenFieldSet("tTable Aux Panel",100);
			printf("<u>Table Tips</u><br>");
			printf("Here we manage mostly project tables. We also have included misc source code files"
				" and even data init .txt files. The other tables refer to templates that are"
				" required to build or initialize the target apps.");
			printf("<p><u>Record Context Info</u><br>");
			if(guCookieProject && guCookieProject!=uProject)
				printf("Current workflow project has no tables.");
			else if(guCookieProject && guCookieProject==uProject)
				printf("Loaded table belongs to current workflow project.");
			else if(!guCookieProject)
				printf("First of all tables (if any) loaded.");
			if(strstr(cDescription,"uParentTable="))
				htmlDescriptionInfo(cDescription);
			if(cLabel[0])
				htmlLabelInfo(cLabel);
			htmlTemplateInfo(cLabel);
			printf("<p><u>Operations</u><br>");
			if(uTable)
			{
				printf("<input type=submit class=largeButton title='Add standard primary key, cLabel and audit fields.'"
				" name=gcCommand value='Add standard fields'>");
				printf("<br><input type=submit class=lwarnButton title='Remove all fields from the loaded table.'"
				" name=gcCommand value='Remove all fields'>");
				printf("<br><input type=submit class=largeButton"
				" title='Select and keep this table marked for current work flow. Release any selected field.'"
				" name=gcCommand value='Select'>");
			}
			printf("<br><input type=submit class=largeButton title='Open the field and table import panel.'"
					" name=gcCommand value='Import'>");
			if(uTable && uTemplateType==uTEMPLATETYPE_BOOTSTRAP)
				printf("<br><input type=submit class=largeButton title='Copy Fields from RAD4 same name table in this same project'"
					" name=gcCommand value='Copy Fields'>");
			tTableFieldNavList();
			tTableNavList();
	}
	CloseFieldSet();

}//void ExttTableButtons(void)


void ExttTableAuxTable(void)
{

}//void ExttTableAuxTable(void)


void ExttTableGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uTable"))
		{
			sscanf(gentries[i].val,"%u",&uTable);
			guMode=6;
		}
		else if(guCookieTable)
		{
			uTable=guCookieTable;
			guMode=7;
		}
		else if(guCookieProject)
		{
        		MYSQL_RES *res;
        		MYSQL_ROW field;

			sprintf(gcQuery,"SELECT uTable FROM tTable WHERE uProject=%u ORDER BY uTableOrder LIMIT 1",guCookieProject);
        		mysql_query(&gMysql,gcQuery);
        		if(mysql_errno(&gMysql))
                		tTable(mysql_error(&gMysql));
        		res=mysql_store_result(&gMysql);
	        	if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&uTable);
				guMode=7;
			}
		}
	}
	tTable("");

}//void ExttTableGetHook(entry gentries[], int x)


void ExttTableSelect(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTable ORDER BY"
				" uTable",
				VAR_LIST_tTable);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tTable,tClient WHERE tTable.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uTable",
					VAR_LIST_tTable,uContactParentCompany,uContactParentCompany);
					

}//void ExttTableSelect(void)


void ExttTableSelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tTable WHERE uTable=%u",
			VAR_LIST_tTable,uTable);
	else
                sprintf(gcQuery,"SELECT %s FROM tTable,tClient"
                                " WHERE tTable.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tTable.uTable=%u",
                        		VAR_LIST_tTable
					,uContactParentCompany,uContactParentCompany
					,uTable);

}//void ExttTableSelectRow(void)


void ExttTableListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTable",
				VAR_LIST_tTable);
	else
		sprintf(gcQuery,"SELECT %s FROM tTable,tClient"
				" WHERE tTable.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tTable
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttTableListFilter()
        if(!strcmp(gcFilter,"uProject"))
        {
                sscanf(gcCommand,"%u",&uProject);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tTable.uProject=%u ORDER BY uTable",uProject);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cSubDir"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tTable.cSubDir LIKE '%s' ORDER BY tTable.cSubDir",gcCommand);
		strcat(gcQuery,cCat);
	}
        else if(!strcmp(gcFilter,"cDescription"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tTable.cDescription LIKE '%s' ORDER BY uTable",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uTable"))
        {
                sscanf(gcCommand,"%u",&uTable);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tTable.uTable=%u ORDER BY uTable",uTable);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uTable");
        }

}//void ExttTableListSelect(void)


void ExttTableListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uProject"))
                printf("<option>uProject</option>");
        else
                printf("<option selected>uProject</option>");
        if(strcmp(gcFilter,"cSubDir"))
                printf("<option>cSubDir</option>");
        else
                printf("<option selected>cSubDir</option>");
        if(strcmp(gcFilter,"cDescription"))
                printf("<option>cDescription</option>");
        else
                printf("<option selected>cDescription</option>");
        if(strcmp(gcFilter,"uTable"))
                printf("<option>uTable</option>");
        else
                printf("<option selected>uTable</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttTableListFilter(void)


void ExttTableNavBar(void)
{
	if(guMode==10000 || guMode==10001)
		return;
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
		printf(LANG_NBB_NEW);

	if(!guListMode && uAllowMod(uOwner,uCreatedBy))
		printf(LANG_NBB_MODIFY);

	if(!guListMode && uAllowDel(uOwner,uCreatedBy)) 
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttTableNavBar(void)


void tTableNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(guCookieProject)
	{
		if(guLoginClient==1 && guPermLevel>11)//Root can read access all
			sprintf(gcQuery,"SELECT uTable,cLabel,cSubDir FROM tTable WHERE uProject=%u "
					" ORDER BY uTemplateType,cSubDir,uTableOrder",
				guCookieProject);
		else
			sprintf(gcQuery,"SELECT tTable.uTable,"
				" tTable.cLabel,tTable.cSubDir"
				" FROM tTable,tClient"
				" WHERE tTable.uOwner=tClient.uClient"
				" AND tTable.uProject=%u"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY tTable.uTemplateType,tTable.cSubDir,tTable.uTableOrder,",
					guCookieProject,guCompany,guLoginClient);
	}
	else
	{
		if(guLoginClient==1 && guPermLevel>11)//Root can read access all
			sprintf(gcQuery,"SELECT uTable,cLabel,cSubDir FROM tTable ORDER BY uTemplateType,cSubDir,uTableOrder");
		else
			sprintf(gcQuery,"SELECT tTable.uTable,"
				" tTable.cLabel,tTable.cSubDir"
				" FROM tTable,tClient"
				" WHERE tTable.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY tTable.uTemplateType,tTable.cSubDir,tTable.uTableOrder,",
					guCompany,guLoginClient);
	}
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
		char cSubDir[100]={""};
			
        	printf("<p><u>tTableNavList</u><br>\n");
	        while((field=mysql_fetch_row(res)))
		{
			if(atoi(field[0])==uTable)
				cColor="blue";
			else
				cColor="black";
			if(field[2]!=NULL && field[2])
				sprintf(cSubDir,"%.99s",field[2]);
			printf("<a class=darkLink href=unxsRAD.cgi?gcFunction=tTable"
				"&uTable=%s><font color=%s>%s/%s</font></a><br>\n",
					field[0],cColor,cSubDir,field[1]);
		}
	}
        mysql_free_result(res);

}//void tTableNavList(void)


void tTableFieldNavList(void)
{
	if(!uTable) return;

        MYSQL_RES *res;
        MYSQL_ROW field;
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uField,cLabel,cOtherOptions FROM tField WHERE uTable=%u ORDER BY uOrder,cLabel",uTable);
	else
		sprintf(gcQuery,"SELECT tField.uField,"
				" tField.cLabel,tField.cOtherOptions"
				" FROM tField,tClient"
				" WHERE tField.uOwner=tClient.uClient"
				" AND tField.uTable=%u"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY tField.uOrder,tField.cLabel",
					uTable,guCompany,guLoginClient);
        mysql_query(&gMysql,gcQuery);
        printf("<p><u>tFieldNavList</u><br>\n");
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
		char *cColor;
	        while((field=mysql_fetch_row(res)))
		{
			if(atoi(field[0])==guCookieField)
				cColor="blue";
			else
				cColor="black";
			printf("<a class=darkLink href=unxsRAD.cgi?gcFunction=tField"
				"&uField=%s><font color=%s>%s %s</font></a><br>\n",
				field[0],cColor,field[1],field[2]);
		}
	}
	else
	{
		printf("Table has no fields, might be a special default table created directly from a \"named\" template");
	}
        mysql_free_result(res);

}//void tTableFieldNavList(void)




static unsigned uRADPRI;
static unsigned uVARCHAR;
static unsigned uFOREIGNKEY;
static unsigned uUNIXTIMECREATE;
static unsigned uUNIXTIMEUPDATE;
static unsigned uUNSIGNED;
void GetuFieldTypes(void)
{	
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uFieldType FROM tFieldType WHERE uRADType=%u LIMIT 1",COLTYPE_RADPRI);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uRADPRI);

	sprintf(gcQuery,"SELECT uFieldType FROM tFieldType WHERE uRADType=%u LIMIT 1",COLTYPE_VARCHAR);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uVARCHAR);

	sprintf(gcQuery,"SELECT uFieldType FROM tFieldType WHERE uRADType=%u LIMIT 1",COLTYPE_FOREIGNKEY);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uFOREIGNKEY);

	sprintf(gcQuery,"SELECT uFieldType FROM tFieldType WHERE uRADType=%u LIMIT 1",COLTYPE_UNIXTIMEUPDATE);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uUNIXTIMEUPDATE);

	sprintf(gcQuery,"SELECT uFieldType FROM tFieldType WHERE uRADType=%u LIMIT 1",COLTYPE_UNIXTIMECREATE);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uUNIXTIMECREATE);

	sprintf(gcQuery,"SELECT uFieldType FROM tFieldType WHERE uRADType=%u LIMIT 1",COLTYPE_INTUNSIGNED);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uUNSIGNED);

	mysql_free_result(res);

}//void GetuFieldTypes(void)


//Uses file scoped vars from form
void AddDefaultFields(void)
{
	GetuFieldTypes();

	//Primary key: Given tSomething then it will be uSomething	
	char cFieldName[32]={""};
	sprintf(cFieldName,"u%.30s",cLabel+1);
	sprintf(gcQuery,"INSERT INTO tField SET uTable=%u,uProject=%u,cLabel='%s',uOrder=1,uClass=%u,"
			"cTitle='Primary key',uFieldType=%u,uSQLSize=10,uModLevel=20,uOwner=%u,"
			"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uTable,
			uProject,
			cFieldName,
			uDEFAULTCLASS,
			uRADPRI,guCompany,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(gcQuery);

	//cLabel
	sprintf(gcQuery,"INSERT INTO tField SET uTable=%u,uProject=%u,cLabel='cLabel',uOrder=2,uClass=%u,"
			"cTitle='Short label',uFieldType=%u,uSQLSize=32,uHtmlXSize=40,uHtmlYSize=1,uHtmlMax=32,uOwner=%u,"
			"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uTable,
			uProject,
			uDEFAULTCLASS,
			uVARCHAR,guCompany,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(gcQuery);

	//uOwner
	sprintf(gcQuery,"INSERT INTO tField SET uModLevel=20,uTable=%u,uProject=%u,cLabel='uOwner',uOrder=1000,uClass=%u,"
			"cTitle='Record owner',uFieldType=%u,uSQLSize=10,cFKSpec='\"tClient\",\"cLabel\",uOwner',uOwner=%u,"
			"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uTable,
			uProject,
			uDEFAULTCLASS,
			uFOREIGNKEY,guCompany,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(gcQuery);

	//uCreatedBy
	sprintf(gcQuery,"INSERT INTO tField SET uModLevel=20,uTable=%u,uProject=%u,cLabel='uCreatedBy',uOrder=1001,uClass=%u,"
			"cTitle='Record created by',uFieldType=%u,uSQLSize=10,cFKSpec='\"tClient\",\"cLabel\",uCreatedBy',uOwner=%u,"
			"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uTable,
			uProject,
			uDEFAULTCLASS,
			uFOREIGNKEY,guCompany,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(gcQuery);

	//uCreatedDate
	sprintf(gcQuery,"INSERT INTO tField SET uModLevel=20,uTable=%u,uProject=%u,cLabel='uCreatedDate',uOrder=1002,uClass=%u,"
			"cTitle='Unix timestamp for creation date',uFieldType=%u,uSQLSize=10,uOwner=%u,"
			"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uTable,
			uProject,
			uDEFAULTCLASS,
			uUNIXTIMECREATE,guCompany,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(gcQuery);

	//uModBy
	sprintf(gcQuery,"INSERT INTO tField SET uModLevel=20,uTable=%u,uProject=%u,cLabel='uModBy',uOrder=1003,uClass=%u,"
			"cTitle='Record modifed by',uFieldType=%u,uSQLSize=10,cFKSpec='\"tClient\",\"cLabel\",uModBy',uOwner=%u,"
			"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uTable,
			uProject,
			uDEFAULTCLASS,
			uFOREIGNKEY,guCompany,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(gcQuery);

	//uModDate
	sprintf(gcQuery,"INSERT INTO tField SET uModLevel=20,uTable=%u,uProject=%u,cLabel='uModDate',uOrder=1004,uClass=%u,"
			"cTitle='Unix timestamp for last update',uFieldType=%u,uSQLSize=10,uOwner=%u,"
			"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uTable,
			uProject,
			uDEFAULTCLASS,
			uUNIXTIMEUPDATE,guCompany,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(gcQuery);

}//void AddDefaultFields(void)


void RemoveAllFields(unsigned uTable,unsigned uProject)
{
	sprintf(gcQuery,"DELETE FROM tField WHERE uTable=%u AND uProject=%u",
			uTable,
			uProject);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tTable(mysql_error(&gMysql));

}//void RemoveAllFields()


//Bulk editors
void RemoveTableFields(char *cValue)
{
	char cLine[256];
	while(1)
	{
		sprintf(cLine,"%.255s",ParseTextAreaLines(cValue));
		if(cLine[0]=='#') continue;
		if(cLine[0]==' ') continue;
		if(!cLine[0]) break;
		RemoveTableFieldLine(cLine);
	}
}//void RemoveTableFields(char *cValue)


void AddTableFields(char *cValue)
{
	char cLine[256];
	while(1)
	{
		sprintf(cLine,"%.255s",ParseTextAreaLines(cValue));
		if(cLine[0]=='#') continue;
		if(cLine[0]==' ') continue;
		if(!cLine[0]) break;
		AddTableFieldLine(cLine);
	}

}//void AddDefaultTables(unsigned uProject)


void RemoveTableFieldLine(char *cLine)
{

	char cLabel[32]={""};
	char cTitle[100]={""};
	char cFieldType[32]={""};
	unsigned uOrder=0;
	char cFKSpec[65]={""};

	int iCount=0;
	unsigned uField=0;

	//cLabel;cTitle;tFieldType.cLabel;uOrder;[cFKSpec]
	//cLastname;Last name;Varchar Unique Key;21;
	//uPaciente;Numero de paciente;Select Table Owner;10;"tPaciente","cLabel"
	iCount=sscanf(cLine,"%31[a-zA-Z0-9\\.];%99[a-zA-Z0-9/+\\.%% ];%31[a-zA-Z0-9 ];%u;%64[a-zA-Z0-9\",]",
					cLabel,cTitle,cFieldType,&uOrder,cFKSpec);
	//debug1
	//sprintf(gcQuery,"(%d) %s;%s;%s;%u;%s",iCount,cLabel,cTitle,cFieldType,uOrder,cFKSpec);
	//tTable(gcQuery);

	if(!cLabel[0] || iCount<4)
	{
		char gcQuery[512];
		sprintf(gcQuery,"Error1 %s",cLine);
		tTable(gcQuery);
	}

        MYSQL_RES *res;
        MYSQL_ROW field;
	sprintf(gcQuery,"SELECT uField FROM tField WHERE cLabel='%.32s' AND uTable=%u AND uProject='%u'",
			cLabel,uTable,uProject);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        	htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uField);
        mysql_free_result(res);
	if(uField)
	{
		sprintf(gcQuery,"DELETE FROM tField WHERE uField=%u",uField);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
		{
			sprintf(gcQuery,"%s",mysql_error(&gMysql));
			tTable(gcQuery);
		}
	}

}//void RemoveTableFieldLine(char *cLine)


void AddTableFieldLine(char *cLine)
{

	char cLabel[32]={""};
	char cTitle[100]={""};
	char cFieldType[32]={""};
	unsigned uFieldType=0;
	unsigned uOrder=0;
	char cFKSpec[65]={""};
	unsigned uSQLSize=10;//default for all except Varchar types

	int iCount=0;
	unsigned uField=0;

	char gcQuery[512];
	//cLabel;cTitle;tFieldType.cLabel;uOrder;[cFKSpec]/[uSQLSize for Varchars]
	//cLastname;Last name;Varchar Unique Key;21;
	//uPaciente;Numero de paciente;Select Table Owner;10;"tPaciente","cLabel"
	iCount=sscanf(cLine,"%31[a-zA-Z0-9\\.];%99[a-zA-Z0-9/+\\.%% ];%31[a-zA-Z0-9 ];%u;%64[a-zA-Z0-9\",]",
					cLabel,cTitle,cFieldType,&uOrder,cFKSpec);
	//debug1
	//sprintf(gcQuery,"(%d) %s;%s;%s;%u;%s",iCount,cLabel,cTitle,cFieldType,uOrder,cFKSpec);
	//tTable(gcQuery);

	if( iCount<4 || !cLabel[0] || !cTitle[0] || !cFieldType[0] || !uOrder)
	{
		sprintf(gcQuery,"Error1 %s",cLine);
		tTable(gcQuery);
	}

        MYSQL_RES *res;
        MYSQL_ROW field;
	sprintf(gcQuery,"SELECT uFieldType FROM tFieldType WHERE cLabel='%.32s'",cFieldType);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        	htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uFieldType);
        mysql_free_result(res);
	if(!uFieldType)
	{
		sprintf(gcQuery,"Error2 no such field type %s",cFieldType);
		tTable(gcQuery);
	}

	if(strstr(cFieldType,"Varchar"))
	{
		uSQLSize=0;
		sscanf(cFKSpec,"%u",&uSQLSize);
		if(!uSQLSize)
		{
			sprintf(gcQuery,"%s Varchar requires size in last col: ;[cFKSpec]/[uSQLSize];",cLabel);
			tTable(gcQuery);
		}
		cFKSpec[0]=0;
	}

	//debug2
	//sprintf(gcQuery,"(%d %u) %s;%s;%s;%u;%s",iCount,uFieldType,cLabel,cTitle,cFieldType,uOrder,cFKSpec);
	//tTable(gcQuery);


	sprintf(gcQuery,"SELECT uField FROM tField WHERE cLabel='%.32s' AND uProject=%u AND uTable=%u",
				cLabel,uProject,uTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        	htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uField);
        mysql_free_result(res);
	if(uField)
	{
		sprintf(gcQuery,"UPDATE tField SET"
				" cLabel='%.32s',"
				" uOrder=%u,"
				" uFieldType=%u,"
				" cTitle='%.100s',"
				" cFKSpec='%.99s',"
				" uSQLSize='%u',"
				" uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE uField=%u",
					cLabel,
					uOrder,
					uFieldType,
					cTitle,
					cFKSpec,
					uSQLSize,
					guLoginClient,
						uField);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
		{
			sprintf(gcQuery,"%s",mysql_error(&gMysql));
			tTable(gcQuery);
		}
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tField SET"
				" uTable=%u,"
				" uProject=%u,"
				" cLabel='%.32s',"
				" uOrder=%u,"
				" uFieldType=%u,"
				" cTitle='%.100s',"
				" cFKSpec='%.100s',"
				" uSQLSize=%u,"
				" uHtmlXSize='40',"
				" uHtmlYSize='1',"
				" uHtmlMax=%u,"
				" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uTable,
					uProject,
					cLabel,
					uOrder,
					uFieldType,
					cTitle,
					cFKSpec,
					uSQLSize,
					uSQLSize,
					guCompany,guLoginClient);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
		{
			sprintf(gcQuery,"%s",mysql_error(&gMysql));
			tTable(gcQuery);
		}
	}

}//void AddTableFieldLine(char *cLine)


void ExportTableFields(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char gcQuery[512];
	static char cImportBuffer[4096];
	sprintf(gcQuery,"SELECT "
				" tField.cLabel,"
				" tField.cTitle,"
				" tFieldType.cLabel,"
				" tField.uOrder,"
				" IF(tField.cFKSpec!='',tField.cFKSpec,tField.uSQLSize)"
				" FROM tField,tFieldType WHERE tFieldType.uFieldType=tField.uFieldType AND"
				" tField.uTable=%u AND"
				" tField.uProject=%u AND"
				" tField.uClass!=%u"
				" ORDER BY tField.uOrder",
					uTable,
					uProject,
					uDEFAULTCLASS);
       	mysql_query(&gMysql,gcQuery);
       	if(mysql_errno(&gMysql))
	{
		sprintf(gcQuery,"%s",mysql_error(&gMysql));
		tTable(gcQuery);
	}
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)) && strlen(cImportBuffer)<(4095-512))
	{
		sprintf(gcQuery,"%s;%s;%s;%s;%s;\n",field[0],field[1],field[2],field[3],field[4]);
		strcat(cImportBuffer,gcQuery);
	}
        mysql_free_result(res);
	cImport=cImportBuffer;
}//void ExportTableFields(void)


void RemoveInterfaceTables(char *cValue)
{
	char cLine[256];
	while(1)
	{
		sprintf(cLine,"%.255s",ParseTextAreaLines(cValue));
		if(cLine[0]=='#') continue;
		if(cLine[0]==' ') continue;
		if(!cLine[0]) break;
		RemoveInterfaceTableLine(cLine);
	}
}//void RemoveInterfaceTables(char *cValue)


void AddInterfaceTables(char *cValue)
{
	char cLine[256];
	while(1)
	{
		sprintf(cLine,"%.255s",ParseTextAreaLines(cValue));
		if(cLine[0]=='#') continue;
		if(cLine[0]==' ') continue;
		if(!cLine[0]) break;
		AddInterfaceTableLine(cLine);
	}

}//void AddInterfaceTables(unsigned uProject)


void RemoveInterfaceTableLine(char *cLine)
{
	char cLabel[32]={""};
	char cSubDir[100]={""};
	int iCount=0;

	char gcQuery[512];
	iCount=sscanf(cLine,"%31[a-zA-Z0-9\\.];%99[a-zA-Z0-9/+\\.];",
					cLabel,cSubDir);

	if( iCount<2 || !cLabel[0] || !cSubDir[0] )
	{
		sprintf(gcQuery,"Error1 %s",cLine);
		tTable(gcQuery);
	}

        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uTable=0;
	sprintf(gcQuery,"SELECT uTable FROM tTable WHERE"
			" cLabel='%.32s' AND"
			" cSubDir='%s' AND"
			" uProject=%u AND"
			" uTemplateType=%u"
				,cLabel,cSubDir,uProject,uTEMPLATETYPE_BOOTSTRAP);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		sprintf(gcQuery,"%s",mysql_error(&gMysql));
		tTable(gcQuery);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uTable);

		sprintf(gcQuery,"DELETE FROM tTable WHERE uTable=%u",uTable);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
		{
			sprintf(gcQuery,"%s",mysql_error(&gMysql));
			tTable(gcQuery);
		}
		if(mysql_affected_rows(&gMysql)>0)
			RemoveAllFields(uTable,uProject);
	}

}//void RemoveInterfaceTableLine(char *cLine)


void AddInterfaceTableLine(char *cLine)
{

	char cLabel[32]={""};
	char cSubDir[100]={""};

	int iCount=0;
	unsigned uTable=0;
	unsigned uTableOrder=0;

	char gcQuery[512];
	iCount=sscanf(cLine,"%31[a-zA-Z0-9\\.];%99[a-zA-Z0-9/+\\.];%u;",
					cLabel,cSubDir,&uTableOrder);

	if( iCount<2 || !cLabel[0] || !cSubDir[0] )
	{
		sprintf(gcQuery,"Error1 %s",cLine);
		tTable(gcQuery);
	}

        MYSQL_RES *res;
        MYSQL_ROW field;
	sprintf(gcQuery,"SELECT uTable FROM tTable WHERE cLabel='%.32s' AND uProject=%u AND uTemplateType=%u",
					cLabel,uProject,uTEMPLATETYPE_BOOTSTRAP);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        	htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uTable);
        mysql_free_result(res);
	if(uTable)
	{
		sprintf(gcQuery,"UPDATE tTable SET"
				" cSubDir='%.99s',"
				" uTableOrder=%u,"
				" uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE uTable=%u",
					cSubDir,
					uTableOrder,
					guLoginClient,
					uTable);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
		{
			sprintf(gcQuery,"%s",mysql_error(&gMysql));
			tTable(gcQuery);
		}
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tTable SET"
				" uProject=%u,"
				" cLabel='%.32s',"
				" cSubDir='%.99s',"
				" uTableOrder=%u,"
				" uTemplateType=%u,"
				" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uProject,
					cLabel,
					cSubDir,
					uTableOrder,
					uTEMPLATETYPE_BOOTSTRAP,
					guCompany,guLoginClient);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
		{
			sprintf(gcQuery,"%s",mysql_error(&gMysql));
			tTable(gcQuery);
		}
	}

}//void AddInterfaceTableLine(char *cLine)


void ExportInterfaceTables(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char gcQuery[512];
	static char cImportBuffer[4096];
	sprintf(gcQuery,"SELECT "
				" tTable.cLabel,"
				" tTable.cSubDir,"
				" tTable.uTableOrder"
				" FROM tTable WHERE"
				" tTable.uProject=%u"
				" AND tTable.uTemplateType=%u"
				" ORDER BY tTable.uTableOrder, tTable.uTable",
					uProject,uTEMPLATETYPE_BOOTSTRAP);
       	mysql_query(&gMysql,gcQuery);
       	if(mysql_errno(&gMysql))
	{
		sprintf(gcQuery,"%s",mysql_error(&gMysql));
		tTable(gcQuery);
	}
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)) && strlen(cImportBuffer)<(4095-512))
	{
		sprintf(gcQuery,"%s;%s;%s;\n",field[0],field[1],field[2]);
		strcat(cImportBuffer,gcQuery);
	}
        mysql_free_result(res);
	cImport=cImportBuffer;

}//void ExportInterfaceTables(void)


void CopyFieldsFromSameNameTable(unsigned uTargetTable,unsigned uProject)
{
	if(!uTable)
		return;

	char gcQuery[1024];
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uSourceTable=0;
	sprintf(gcQuery,"SELECT uTable FROM tTable"
			" WHERE cLabel=(SELECT cLabel FROM tTable WHERE uTable=%u LIMIT 1)"
			" AND uTemplateType=%u AND uProject=%u"
				,uTargetTable,uTEMPLATETYPE_RAD4,uProject);
       	mysql_query(&gMysql,gcQuery);
       	if(mysql_errno(&gMysql))
	{
		sprintf(gcQuery,"%s",mysql_error(&gMysql));
		tTable(gcQuery);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uSourceTable);
        mysql_free_result(res);

	if(uSourceTable)
	{
		CopyAllFields(uTargetTable,uSourceTable);
		sprintf(gcQuery,"UPDATE tTable SET uModDate=UNIX_TIMESTAMP(NOW()),uModBy=%u WHERE uTable=%u",
						guLoginClient,uTargetTable);
       		mysql_query(&gMysql,gcQuery);
       		if(mysql_errno(&gMysql))
		{
			sprintf(gcQuery,"%s",mysql_error(&gMysql));
			tTable(gcQuery);
		}
		uModDate=luGetModDate("tTable",uTable);
		uModBy=guLoginClient;
	}
	else
	{
		tTable("No source table found");
	}

}//void CopyFieldsFromSameNameTable()


void CopyAllFields(unsigned uTargetTable, unsigned uSourceTable)
{
	char gcQuery[1024];
	sprintf(gcQuery,"INSERT INTO tField"
			"("
			" cLabel,uProject,uTable,uOrder,uFieldType,uIndexType,cFKSpec,"
			" cExtIndex,cTitle,uSQLSize,uHtmlXSize,uHtmlYSize,uHtmlMax,cFormDefault,"
			" cSQLDefault,cOtherOptions,uReadLevel,uModLevel,uOwner,uCreatedBy,uCreatedDate"
			")"
			" SELECT "
			" cLabel,uProject,%u,uOrder,uFieldType,uIndexType,cFKSpec,"
			" cExtIndex,cTitle,uSQLSize,uHtmlXSize,uHtmlYSize,uHtmlMax,cFormDefault,"
			" cSQLDefault,cOtherOptions,uReadLevel,uModLevel,%u,%u,UNIX_TIMESTAMP(NOW())"
			" FROM tField WHERE uTable=%u",uTargetTable,guCompany,guLoginClient,uSourceTable);
       	mysql_query(&gMysql,gcQuery);
       	if(mysql_errno(&gMysql))
	{
		sprintf(gcQuery,"%s",mysql_error(&gMysql));
		tTable(gcQuery);
	}
	if(mysql_affected_rows(&gMysql)<1)
		tTable("No fields added!");

}//void CopyAllFields(unsigned uTargetTable, unsigned uSourceTable)
