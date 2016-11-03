/*
FILE
	svn ID removed
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



void ExtProcesstGroupGlueVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstGroupGlueVars(pentry entries[], int x)


void ExttGroupGlueCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tGroupGlueTools"))
	{
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstGroupGlueVars(entries,x);
                        	guMode=2000;
	                        tGroupGlue(LANG_NB_CONFIRMNEW);
			}
			else
				tGroupGlue("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstGroupGlueVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uGroupGlue=0;
#ifdef StandardFields
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
#endif
				NewtGroupGlue(0);
			}
			else
				tGroupGlue("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstGroupGlueVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
	                        guMode=2001;
				tGroupGlue(LANG_NB_CONFIRMDEL);
			}
			else
				tGroupGlue("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstGroupGlueVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=5;
				DeletetGroupGlue();
			}
			else
				tGroupGlue("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstGroupGlueVars(entries,x);
#ifdef StandardFields
			if(uAllowMod(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=2002;
				tGroupGlue(LANG_NB_CONFIRMMOD);
			}
			else
				tGroupGlue("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstGroupGlueVars(entries,x);
#ifdef StandardFields
			if(uAllowMod(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

#ifdef StandardFields
				uModBy=guLoginClient;
#endif
				ModtGroupGlue();
			}
			else
				tGroupGlue("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttGroupGlueCommands(pentry entries[], int x)


void ExttGroupGlueButtons(void)
{
	OpenFieldSet("tGroupGlue Aux Panel",100);
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
			//printf("<br><input type=submit class=largeButton title='Sample button help'"
			//		" name=gcCommand value='Sample Button'>");
	}
	CloseFieldSet();

}//void ExttGroupGlueButtons(void)


void ExttGroupGlueAuxTable(void)
{

}//void ExttGroupGlueAuxTable(void)


void ExttGroupGlueGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uGroupGlue"))
		{
			sscanf(gentries[i].val,"%u",&uGroupGlue);
			guMode=6;
		}
	}
	tGroupGlue("");

}//void ExttGroupGlueGetHook(entry gentries[], int x)


void ExttGroupGlueSelect(void)
{

#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tGroupGlue ORDER BY"
				" uGroupGlue",
				VAR_LIST_tGroupGlue);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tGroupGlue,tClient WHERE tGroupGlue.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uGroupGlue",
					VAR_LIST_tGroupGlue,uContactParentCompany,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tGroupGlue ORDER BY uGroupGlue",VAR_LIST_tGroupGlue);
#endif
					

}//void ExttGroupGlueSelect(void)


void ExttGroupGlueSelectRow(void)
{
#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tGroupGlue WHERE uGroupGlue=%u",
			VAR_LIST_tGroupGlue,uGroupGlue);
	else
                sprintf(gcQuery,"SELECT %s FROM tGroupGlue,tClient"
                                " WHERE tGroupGlue.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tGroupGlue.uGroupGlue=%u",
                        		VAR_LIST_tGroupGlue
					,uContactParentCompany,uContactParentCompany
					,uGroupGlue);
#else
	sprintf(gcQuery,"SELECT %s FROM tGroupGlue WHERE uGroupGlue=%u",VAR_LIST_tGroupGlue,uGroupGlue);
#endif

}//void ExttGroupGlueSelectRow(void)


void ExttGroupGlueListSelect(void)
{
	char cCat[512];
#ifdef StandardFields
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tGroupGlue",
				VAR_LIST_tGroupGlue);
	else
		sprintf(gcQuery,"SELECT %s FROM tGroupGlue,tClient"
				" WHERE tGroupGlue.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tGroupGlue
				,uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tGroupGlue",VAR_LIST_tGroupGlue);
#endif

	//Changes here must be reflected below in ExttGroupGlueListFilter()
        if(!strcmp(gcFilter,"uGroupGlue"))
        {
                sscanf(gcCommand,"%u",&uGroupGlue);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tGroupGlue.uGroupGlue=%u"
						" ORDER BY uGroupGlue",
						uGroupGlue);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uGroupGlue");
        }

}//void ExttGroupGlueListSelect(void)


void ExttGroupGlueListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uGroupGlue"))
                printf("<option>uGroupGlue</option>");
        else
                printf("<option selected>uGroupGlue</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttGroupGlueListFilter(void)


void ExttGroupGlueNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
		printf(LANG_NBB_NEW);

#ifdef StandardFields
	if(uAllowMod(uOwner,uCreatedBy))
#else
	if(guPermLevel>=9)
#endif
		printf(LANG_NBB_MODIFY);

#ifdef StandardFields
	if(uAllowDel(uOwner,uCreatedBy)) 
#else
	if(guPermLevel>=9)
#endif
		printf(LANG_NBB_DELETE);

#ifdef StandardFields
	if(uOwner)
#else
	if(guPermLevel>=9)
#endif
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttGroupGlueNavBar(void)
