/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Ism5 and mysqlRAD2 author: (C) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details

 
*/

//ModuleFunctionProtos()


void tIsm5NavList(void);
static char cSearch[33]={""};

void ExtProcesstIsm5Vars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.32s",entries[i].val);
	}
}//void ExtProcesstIsm5Vars(pentry entries[], int x)


void ExttIsm5Commands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tIsm5Tools"))
	{
		//ModuleFunctionProcess()

		//Default wizard like two step creation and deletion
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstIsm5Vars(entries,x);
                        	guMode=2000;
	                        tIsm5(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstIsm5Vars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uIsm5=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtIsm5(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstIsm5Vars(entries,x);

	                        guMode=2001;
				tIsm5(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstIsm5Vars(entries,x);

				guMode=5;
				DeletetIsm5();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstIsm5Vars(entries,x);

				guMode=2002;
				tIsm5(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstIsm5Vars(entries,x);

                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtIsm5();
			}
                }
	}

}//void ExttIsm5Commands(pentry entries[], int x)


void ExttIsm5Buttons(void)
{
	printf("<table width=260 border=0 bgcolor=#9BC1B3 valign=center>\n");
        printf("<tr><td>");

	printf("<font face=Arial,Helvetica>");
	printf("<font size=2>");
	printf("<u>Search Tools</u><br>");
	printf("<input type=text title='Restrict search on cLabel to this value. Can use SQL %% and _ query pattern matching.' name=cSearch value='%s' maxlength=99 size=20> cLabel",cSearch);

	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter required data</u><br>");
                        printf("<font size=1>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<p><font size=1>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf("<font size=1>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			tIsm5NavList();
	}


        printf("</td></tr>\n");
        printf("</table>\n");

}//void ExttIsm5Buttons(void)


void ExttIsm5AuxTable(void)
{

}//void ExttIsm5AuxTable(void)


void ExttIsm5GetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uIsm5"))
		{
			sscanf(gentries[i].val,"%u",&uIsm5);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.32s",gentries[i].val);
		}
	}
	tIsm5("");

}//void ExttIsm5GetHook(entry gentries[], int x)


void ExttIsm5Select(void)
{
        //Set non search query here for tTableName()
	if(cSearch[0])
		sprintf(gcQuery,"SELECT %s FROM tIsm5 WHERE cLabel LIKE '%s%%' ORDER BY cLabel",VAR_LIST_tIsm5,cSearch);
	else
		sprintf(gcQuery,"SELECT %s FROM tIsm5 ORDER BY cLabel",VAR_LIST_tIsm5);
}//void ExttIsm5Select(void)


void ExttIsm5SelectRow(void)
{
	sprintf(gcQuery,"SELECT %s FROM tIsm5 WHERE uIsm5=%u",VAR_LIST_tIsm5,uIsm5);

}//void ExttIsm5SelectRow(void)


void ExttIsm5ListSelect(void)
{
	char cCat[512];
	sprintf(gcQuery,"SELECT %s FROM tIsm5",VAR_LIST_tIsm5);

	//Changes here must be reflected below in ExttIsm5ListFilter()
        if(!strcmp(gcFilter,"uIsm5"))
        {
                sscanf(gcCommand,"%u",&uIsm5);
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tIsm5.uIsm5=%u",uIsm5);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
        }

}//void ExttIsm5ListSelect(void)


void ExttIsm5ListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uIsm5"))
                printf("<option>uIsm5</option>");
        else
                printf("<option selected>uIsm5</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttIsm5ListFilter(void)


void ExttIsm5NavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=12)
	{
		printf(LANG_NBB_NEW);
		printf(LANG_NBB_MODIFY);
		printf(LANG_NBB_DELETE);
		printf(LANG_NBB_LIST);
	}

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttIsm5NavBar(void)


void tIsm5NavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(cSearch[0])
		sprintf(gcQuery,"SELECT uIsm5,cLabel,uVersion,uPerm FROM tIsm5 WHERE cLabel LIKE '%s%%' ORDER BY cLabel",cSearch);
	else
		sprintf(gcQuery,"SELECT uIsm5,cLabel,uVersion,uPerm FROM tIsm5 ORDER BY cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
		printf("<font size=2>");
        	printf("<p><u>tIsm5NavList");
		if(cSearch[0])
        		printf(" cLabel limited</u><br>\n");
		else
        		printf("</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
			if(cSearch[0])
printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tIsm5&uIsm5=%s&cSearch=%s>%s/%s/%s</a><br>\n",
			field[0],cSearch,field[1],field[2],field[3]);
			else
printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tIsm5&uIsm5=%s>%s/%s/%s</a><br>\n",
				field[0],field[1],field[2],field[3]);
	        }
	}
        mysql_free_result(res);

}//void tIsm5NavList(void)


