/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template and mysqlRAD2 author: (c) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details

 
*/

void ExtProcesstNASGroupVars(pentry entries[], int x)
{

}//void ExtProcesstNASGroupVars(pentry entries[], int x)


void ExttNASGroupCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tNASGroupTools"))
	{
	}

}//void ExttNASGroupCommands(pentry entries[], int x)


void ExttNASGroupButtons(void)
{
	OpenFieldSet("tNasGroup Aux Panel",100);

	switch(guMode)
        {

	}

	CloseFieldSet();

}//void ExttNASGroupButtons(void)


void ExttNASGroupAuxTable(void)
{

}//void ExttNASGroupAuxTable(void)


void ExttNASGroupGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uNAS"))
		{
			sscanf(gentries[i].val,"%u",&uNAS);
			guMode=6;
		}
	}
	tNASGroup("");

}//void ExttNASGroupGetHook(entry gentries[], int x)


void ExttNASGroupSelect(void)
{
        //Set non search query here for tTableName()
	sprintf(gcQuery,"SELECT %s FROM tNASGroup ORDER BY\
					uNAS",
					VAR_LIST_tNASGroup);

}//void ExttNASGroupSelect(void)


void ExttNASGroupSelectRow(void)
{
	sprintf(gcQuery,"SELECT %s FROM tNASGroup WHERE uNAS=%u",
			VAR_LIST_tNASGroup,uNAS);

}//void ExttNASGroupSelectRow(void)


void ExttNASGroupListSelect(void)
{
	char cCat[512];

	sprintf(gcQuery,"SELECT %s FROM tNASGroup",VAR_LIST_tNASGroup);

	//Changes here must be reflected below in ExttNASGroupListFilter()
        if(!strcmp(gcFilter,"uNAS"))
        {
                sscanf(gcCommand,"%u",&uNAS);
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tNASGroup.uNAS=%u ORDER BY uNAS",uNAS);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uNAS");
        }

}//void ExttNASGroupListSelect(void)


void ExttNASGroupListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uNAS"))
                printf("<option>uNAS</option>");
        else
                printf("<option selected>uNAS</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttNASGroupListFilter(void)


void ExttNASGroupNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttNASGroupNavBar(void)



