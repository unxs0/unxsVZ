/*
FILE
	$Id: modulefunc.h 1956 2012-05-24 21:38:56Z Colin $
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

{{ModuleFunctionProtos}}

void {{cTableName}}NavList(void);

void ExtProcess{{cTableName}}Vars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcess{{cTableName}}Vars(pentry entries[], int x)


void Ext{{cTableName}}Commands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"{{cTableName}}Tools"))
	{
		{{ModuleFunctionProcess}}
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        Process{{cTableName}}Vars(entries,x);
                        	guMode=2000;
	                        {{cTableName}}(LANG_NB_CONFIRMNEW);
			}
			else
				{{cTableName}}("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	Process{{cTableName}}Vars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				{{cTableKey}}=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				New{{cTableName}}(0);
			}
			else
				{{cTableName}}("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        Process{{cTableName}}Vars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				{{cTableName}}(LANG_NB_CONFIRMDEL);
			}
			else
				{{cTableName}}("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        Process{{cTableName}}Vars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				Delete{{cTableName}}();
			}
			else
				{{cTableName}}("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        Process{{cTableName}}Vars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				{{cTableName}}(LANG_NB_CONFIRMMOD);
			}
			else
				{{cTableName}}("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        Process{{cTableName}}Vars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				Mod{{cTableName}}();
			}
			else
				{{cTableName}}("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void Ext{{cTableName}}Commands(pentry entries[], int x)


void Ext{{cTableName}}Buttons(void)
{
	OpenFieldSet("{{cTableName}} Aux Panel",100);
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
			printf("<p><u>Operations</u><br>");
			printf("<br><input type=submit class=largeButton title='Sample button help'"
					" name=gcCommand value='Sample Button'>");
			{{cTableName}}NavList();
	}
	CloseFieldSet();

}//void Ext{{cTableName}}Buttons(void)


void Ext{{cTableName}}AuxTable(void)
{

}//void Ext{{cTableName}}AuxTable(void)


void Ext{{cTableName}}GetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"{{cTableKey}}"))
		{
			sscanf(gentries[i].val,"%u",&{{cTableKey}});
			guMode=6;
		}
	}
	{{cTableName}}("");

}//void Ext{{cTableName}}GetHook(entry gentries[], int x)


void Ext{{cTableName}}Select(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM {{cTableName}} ORDER BY"
				" {{cTableKey}}",
				VAR_LIST_{{cTableName}});
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM {{cTableName}},tClient WHERE {{cTableName}}.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY {{cTableKey}}",
					VAR_LIST_{{cTableName}},uContactParentCompany,uContactParentCompany);
					

}//void Ext{{cTableName}}Select(void)


void Ext{{cTableName}}SelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM {{cTableName}} WHERE {{cTableKey}}=%u",
			VAR_LIST_{{cTableName}},{{cTableKey}});
	else
                sprintf(gcQuery,"SELECT %s FROM {{cTableName}},tClient"
                                " WHERE {{cTableName}}.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND {{cTableName}}.{{cTableKey}}=%u",
                        		VAR_LIST_{{cTableName}}
					,uContactParentCompany,uContactParentCompany
					,{{cTableKey}});

}//void Ext{{cTableName}}SelectRow(void)


void Ext{{cTableName}}ListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM {{cTableName}}",
				VAR_LIST_{{cTableName}});
	else
		sprintf(gcQuery,"SELECT %s FROM {{cTableName}},tClient"
				" WHERE {{cTableName}}.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_{{cTableName}}
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in Ext{{cTableName}}ListFilter()
        if(!strcmp(gcFilter,"{{cTableKey}}"))
        {
                sscanf(gcCommand,"%u",&{{cTableKey}});
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"{{cTableName}}.{{cTableKey}}=%u"
						" ORDER BY {{cTableKey}}",
						{{cTableKey}});
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY {{cTableKey}}");
        }

}//void Ext{{cTableName}}ListSelect(void)


void Ext{{cTableName}}ListFilter(void)
{
        //Filter
        printf("   Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"{{cTableKey}}"))
                printf("<option>{{cTableKey}}</option>");
        else
                printf("<option selected>{{cTableKey}}</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void Ext{{cTableName}}ListFilter(void)


void Ext{{cTableName}}NavBar(void)
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
	printf("   \n");

}//void Ext{{cTableName}}NavBar(void)


void {{cTableName}}NavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT {{cTableKey}},cLabel FROM {{cTableName}} ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT {{cTableName}}.{{cTableKey}},"
				" {{cTableName}}.cLabel"
				" FROM {{cTableName}},tClient"
				" WHERE {{cTableName}}.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>{{cTableName}}NavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>{{cTableName}}NavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href={{cProject}}.cgi?gcFunction={{cTableName}}"
				"&{{cTableKey}}=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void {{cTableName}}NavList(void)



