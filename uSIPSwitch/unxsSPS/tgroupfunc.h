/*
FILE
	$Id: modulefunc.h 2116 2012-09-19 23:00:28Z Gary $
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2012 Gary Wallis for Unixservice, LLC.
TEMPLATE VARS AND FUNCTIONS
	ModuleFunctionProcess
	ModuleFunctionProtos
	cProject
	cTableKey
	cTableName
*/



static unsigned uKey=0;
static char cuKeyPullDown[256]={""};

void tGroupNavList(void);
void tGroupGlueNavList(void);//tgroupgluefunc.h

void ExtProcesstGroupVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uKey"))
			sscanf(entries[i].val,"%u",&uKey);
		else if(!strcmp(entries[i].name,"cuKeyPullDown"))
		{
			sprintf(cuKeyPullDown,"%.255s",entries[i].val);
			uKey=ReadPullDown(ForeignKey("tGroupType","cLabel",uGroupType),"cLabel",cuKeyPullDown);
		}
	}
}//void ExtProcesstGroupVars(pentry entries[], int x)


void ExttGroupCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tGroupTools"))
	{
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstGroupVars(entries,x);
                        	guMode=2000;
	                        tGroup(LANG_NB_CONFIRMNEW);
			}
			else
				tGroup("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstGroupVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uGroup=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtGroup(0);
			}
			else
				tGroup("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstGroupVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tGroup(LANG_NB_CONFIRMDEL);
			}
			else
				tGroup("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstGroupVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetGroup();
			}
			else
				tGroup("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstGroupVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tGroup(LANG_NB_CONFIRMMOD);
			}
			else
				tGroup("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstGroupVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtGroup();
			}
			else
				tGroup("<blink>Error</blink>: Denied by permissions settings");
                }
		//Custom operations
		else if(!strcmp(gcCommand,"Add"))
                {
                        ProcesstGroupVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) && uGroup && uGroupType)
			{
				if(uKey)
				{
        				MYSQL_RES *res;

					sprintf(gcQuery,"SELECT uGroupGlue FROM tGroupGlue WHERE uKey=%u AND uTable=%u AND uGroup=%u",
						uKey,uGroupType,uGroup);
        				mysql_query(&gMysql,gcQuery);
				        res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)>0)
						tGroup("Key already had been added to tGroup");
					sprintf(gcQuery,"INSERT INTO tGroupGlue SET uKey=%u,uTable=%u,uGroup=%u",uKey,uGroupType,uGroup);
        				mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
                				tGroup(mysql_error(&gMysql));
					tGroup("Key Added to tGroup");
				}
				tGroup("No key selected");
			}
			else
				tGroup("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Del"))
                {
                        ProcesstGroupVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) && uGroup && uGroupType)
			{
				if(uKey)
				{
					sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uKey=%u AND uTable=%u AND uGroup=%u LIMIT 1",
						uKey,uGroupType,uGroup);
        				mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
                				tGroup(mysql_error(&gMysql));
					tGroup("Key deleted from tGroup");
				}
				tGroup("No key selected");
			}
			else
				tGroup("<blink>Error</blink>: Denied by permissions settings");
		}
	}

}//void ExttGroupCommands(pentry entries[], int x)


void ExttGroupButtons(void)
{
	OpenFieldSet("tGroup Aux Panel",100);
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
			printf("<u>Table Tips</u><br>");
			printf("<p><u>Record Context Info</u><br>");
			tGroupGlueNavList();
			printf("<p><u>Operations</u><br>");
			if(uGroup && uGroupType)
			{
				char cTable[33]={"tRule"};
				sprintf(cTable,"%.32s;cuKeyPullDown",ForeignKey("tGroupType","cLabel",uGroupType));
				tTablePullDown(cTable,"cLabel","cLabel",uKey,1);
				printf("<br><input type=submit class=largeButton title='Add records via select to this group'"
					"name=gcCommand value='Add'>");
				printf("<br><input type=submit class=largeButton title='Del record via select to this group'"
					"name=gcCommand value='Del'>");
			}
			tGroupNavList();
	}
	CloseFieldSet();

}//void ExttGroupButtons(void)


void ExttGroupAuxTable(void)
{

}//void ExttGroupAuxTable(void)


void ExttGroupGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uGroup"))
		{
			sscanf(gentries[i].val,"%u",&uGroup);
			guMode=6;
		}
	}
	tGroup("");

}//void ExttGroupGetHook(entry gentries[], int x)


void ExttGroupSelect(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tGroup ORDER BY"
				" uGroup",
				VAR_LIST_tGroup);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tGroup,tClient WHERE tGroup.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uGroup",
					VAR_LIST_tGroup,uContactParentCompany,uContactParentCompany);

}//void ExttGroupSelect(void)


void ExttGroupSelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tGroup WHERE uGroup=%u",
			VAR_LIST_tGroup,uGroup);
	else
                sprintf(gcQuery,"SELECT %s FROM tGroup,tClient"
                                " WHERE tGroup.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tGroup.uGroup=%u",
                        		VAR_LIST_tGroup
					,uContactParentCompany,uContactParentCompany
					,uGroup);
}//void ExttGroupSelectRow(void)


void ExttGroupListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tGroup",
				VAR_LIST_tGroup);
	else
		sprintf(gcQuery,"SELECT %s FROM tGroup,tClient"
				" WHERE tGroup.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tGroup
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttGroupListFilter()
        if(!strcmp(gcFilter,"uGroup"))
        {
                sscanf(gcCommand,"%u",&uGroup);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tGroup.uGroup=%u"
						" ORDER BY uGroup",
						uGroup);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uGroup");
        }

}//void ExttGroupListSelect(void)


void ExttGroupListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uGroup"))
                printf("<option>uGroup</option>");
        else
                printf("<option selected>uGroup</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttGroupListFilter(void)


void ExttGroupNavBar(void)
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

}//void ExttGroupNavBar(void)


void tGroupNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uGroup,cLabel FROM tGroup ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tGroup.uGroup,"
				" tGroup.cLabel"
				" FROM tGroup,tClient"
				" WHERE tGroup.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tGroupNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tGroupNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tGroup"
				"&uGroup=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tGroupNavList(void)


void tGroupGlueNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cTable[33]={""};
	sprintf(cTable,"%.32s",ForeignKey("tGroupType","cLabel",uGroupType));
	sprintf(gcQuery,"SELECT uGroupGlue,cLabel FROM tGroupGlue,%s WHERE uGroup=%u AND tGroupGlue.uKey=%s._rowid",
			cTable,uGroup,cTable);
        //printf("%s\n",gcQuery);
	//return;
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tGroupGlueNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tGroupGlueNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tGroupGlue"
				"&uGroupGlue=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tGroupGlueNavList(void)

