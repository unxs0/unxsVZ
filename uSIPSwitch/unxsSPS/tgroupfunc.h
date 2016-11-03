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



void tGroupNavList(void);
void tGroupMemberNavList(void);

void ExtProcesstGroupVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
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
#ifdef StandardFields
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
#endif
				NewtGroup(0);
			}
			else
				tGroup("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstGroupVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
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
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
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
#ifdef StandardFields
			if(uAllowMod(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
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
				ModtGroup();
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
			printf("This table is used to group GWs and PBXs, or similarly to assign a label (or property)"
				" with some programatic meaning in uSIPSwitch (etc) to a set of GWs or PBXs."
				" Note that there are two kinds of groups in use at this time:"
				" The groups here and \"hidden groups\" that work with tGroupGlue only.");
			printf("<p><u>Record Context Info</u><br>");
			tGroupMemberNavList();
			printf("<p><u>Operations</u><br>");
			//printf("<br><input type=submit class=largeButton title='Sample button help'"
			//		" name=gcCommand value='Sample Button'>");
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

#ifdef StandardFields
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
#else
	sprintf(gcQuery,"SELECT %s FROM tGroup ORDER BY uGroup",VAR_LIST_tGroup);
#endif
					

}//void ExttGroupSelect(void)


void ExttGroupSelectRow(void)
{
#ifdef StandardFields
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
#else
	sprintf(gcQuery,"SELECT %s FROM tGroup WHERE uGroup=%u",VAR_LIST_tGroup,uGroup);
#endif

}//void ExttGroupSelectRow(void)


void ExttGroupListSelect(void)
{
	char cCat[512];
#ifdef StandardFields
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
#else
	sprintf(gcQuery,"SELECT %s FROM tGroup",VAR_LIST_tGroup);
#endif

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

}//void ExttGroupNavBar(void)


void tGroupNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
#ifdef StandardFields
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
#else
	sprintf(gcQuery,"SELECT uGroup,cLabel FROM tGroup ORDER BY cLabel");
#endif
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


void tGroupMemberNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT tGroupGlue.uGroupGlue,tGroupType.cLabel,tGroupGlue.uKey FROM tGroup,tGroupGlue,tGroupType"
				" WHERE tGroup.uGroupType=tGroupType.uGroupType"
				" AND tGroup.uGroupType=tGroupGlue.uGroupType"
				" AND tGroupGlue.uGroup=tGroup.uGroup"
				" AND tGroupGlue.uGroup=%u"
				" ORDER BY tGroupType.cLabel LIMIT 33",
					uGroup);
	else
		sprintf(gcQuery,"SELECT tGroupGlue.uGroupGlue,tGroupType.cLabel,tGroupGlue.uKey FROM tGroup,tGroupGlue,tGroupType"
				" WHERE tGroup.uGroupType=tGroupType.uGroupType"
				" AND tGroup.uGroupType=tGroupGlue.uGroupType"
				" AND tGroupGlue.uGroup=tGroup.uGroup"
				" AND tGroupGlue.uGroup=%u"
				" AND tGroup.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY tGroupType.cLabel LIMIT 33",
					uGroup
					,uContactParentCompany
					,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tGroupMemberNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tGroupMemberNavList</u><br>\n");
		unsigned uCount=0;
	        while((field=mysql_fetch_row(res)))
		{
			unsigned uKey=0;
			sscanf(field[2],"%u",&uKey);
			//GW group 5 PBX group 6 TODO
			if(uGroupType==6)
				printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tGroupGlue"
				"&uGroupGlue=%s>%s/%s</a><br>\n",
				field[0],field[1],ForeignKey("tPBX","cLabel",uKey));
			else
				printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tGroupGlue"
				"&uGroupGlue=%s>%s/%s</a><br>\n",
				field[0],field[1],ForeignKey("tGateway","cLabel",uKey));
			if(++uCount>32)
			{
				printf("(only first 32 records shown)\n");
				break;
			}
		}
	}
        mysql_free_result(res);

}//void tGroupMemberNavList(void)


