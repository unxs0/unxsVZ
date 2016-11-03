/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template and mysqlRAD2 author: (C) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details

 
*/

void ExtProcesstServerGroupVars(pentry entries[], int x)
{

	/*
	register int i;
	
	for(i=0;i<x;i++)
	{
	
	}
	*/

}//void ExtProcesstServerGroupVars(pentry entries[], int x)


void ExttServerGroupCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tServerGroupTools"))
	{
	}

}//void ExttServerGroupCommands(pentry entries[], int x)


void ExttServerGroupButtons(void)
{
	OpenFieldSet("tServerGroup Aux Panel",100);
	switch(guMode)
        {

	}

	CloseFieldSet();

}//void ExttServerGroupButtons(void)


void ExttServerGroupAuxTable(void)
{

}//void ExttServerGroupAuxTable(void)


void ExttServerGroupGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uUser"))
		{
			sscanf(gentries[i].val,"%u",&uUser);
			guMode=6;
		}
	}
	tServerGroup("");

}//void ExttServerGroupGetHook(entry gentries[], int x)


void ExttServerGroupSelect(void)
{
        //Set non search query here for tTableName()
	sprintf(gcQuery,"SELECT %s FROM tServerGroup ORDER BY\
					uUser",
					VAR_LIST_tServerGroup);

}//void ExttServerGroupSelect(void)


void ExttServerGroupSelectRow(void)
{
	sprintf(gcQuery,"SELECT %s FROM tServerGroup WHERE uUser=%u",
			VAR_LIST_tServerGroup,uUser);

}//void ExttServerGroupSelectRow(void)


void ExttServerGroupListSelect(void)
{
	char cCat[512];

	sprintf(gcQuery,"SELECT %s FROM tServerGroup", VAR_LIST_tServerGroup);

	//Changes here must be reflected below in ExttServerGroupListFilter()
        if(!strcmp(gcFilter,"uUser"))
        {
                sscanf(gcCommand,"%u",&uUser);
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tServerGroup.uUser=%u \
						ORDER BY uUser",
						uUser);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uUser");
        }

}//void ExttServerGroupListSelect(void)


void ExttServerGroupListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uUser"))
                printf("<option>uUser</option>");
        else
                printf("<option selected>uUser</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttServerGroupListFilter(void)


void ExttServerGroupNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttServerGroupNavBar(void)


