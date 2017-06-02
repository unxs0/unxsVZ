/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR/LEGAL
	(C) 2001-2010 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
*/

//ModuleFunctionProtos()
void tTableMultiplePullDown(const char *cTableName,const char *cFieldName,const char *cOrderby);


//uContainer: Glue into tContainer
static char cuMulContainerPullDown[256]={""};

void ExtProcesstGroupGlueVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cuMulContainerPullDown") && !strcmp(gcCommand,"Add Multiple Containers"))
		{
			sprintf(cuMulContainerPullDown,"%.255s",entries[i].val);
			uContainer=ReadPullDown("tContainer","cLabel",cuMulContainerPullDown);
			uGroupGlue=0;
			uNode=0;
			if(uContainer && uGroup) NewtGroupGlue(1);
		}
		else if(!strcmp(entries[i].name,"cuMulContainerPullDown") && !strcmp(gcCommand,"Del Multiple Containers"))
		{
			sprintf(cuMulContainerPullDown,"%.255s",entries[i].val);
			uContainer=ReadPullDown("tContainer","cLabel",cuMulContainerPullDown);
			if(uContainer && uGroup)
			{
				sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uContainer=%u AND uGroup=%u",
						uContainer,uGroup);
				MYSQL_RUN;
			}
		
		}
	}

}//void ExtProcesstGroupGlueVars(pentry entries[], int x)


void ExttGroupGlueCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tGroupGlueTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstGroupGlueVars(entries,x);
                        	guMode=2000;
	                        tGroupGlue(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstGroupGlueVars(entries,x);

                        	guMode=2000;
				if(!uGroup || (uNode==0 && uContainer==0))
	                        	tGroupGlue("Must Supply a uGroup and a uNode or uContainer");
                        	guMode=0;

				uGroupGlue=0;
				NewtGroupGlue(0);
			}
		}
		else if(!strcmp(gcCommand,"Add Multiple Containers"))
		{
			if(guPermLevel>=12)
			{
                        	ProcesstGroupGlueVars(entries,x);
                        	guMode=0;
				if(mysql_insert_id(&gMysql)>0)
	                        	tGroupGlue("Multiple containers added");
				else
	                        	tGroupGlue("No containers added");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstGroupGlueVars(entries,x);
	                        guMode=2001;
				tGroupGlue(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstGroupGlueVars(entries,x);
				guMode=5;
				DeletetGroupGlue();
			}
                }
		else if(!strcmp(gcCommand,"Del Multiple Containers"))
		{
			if(guPermLevel>=12)
			{
                        	ProcesstGroupGlueVars(entries,x);
                        	guMode=0;
				if(mysql_affected_rows(&gMysql)>0)
	                        	tGroupGlue("Multiple containers deleted");
				else
	                        	tGroupGlue("No containers deleted");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstGroupGlueVars(entries,x);
				guMode=2002;
				tGroupGlue(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstGroupGlueVars(entries,x);
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				ModtGroupGlue();
			}
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
			printf("<p><u>Add multiple containers</u><br>");
			printf("Select with shift and ctrl multiple containers from list below"
				" to the uGroup selected in right panel. The uContainer and uNode are ignored."
				" Warning: You may add inadvertently the same containers multiple times no"
				" checking takes place.<p>");
			tTableMultiplePullDown("tContainer;cuMulContainerPullDown","cLabel","cLabel");
                        printf("<p><input type=submit class=largeButton name=gcCommand value='Add Multiple Containers'>");
                break;

                case 2001:
                        printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
			printf("<p><u>Delete multiple containers</u><br>");
			printf("Select with shift and ctrl multiple containers from list below"
				" to the uGroup selected in right panel. The uContainer and uNode are ignored.<p>");
			tTableMultiplePullDown("tContainer;cuMulContainerPullDown","cLabel","cLabel");
                        printf("<p><input type=submit class=largeButton name=gcCommand value='Del Multiple Containers'>");
                break;

                case 2002:
			printf("<p><u>Review changes</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			printf("<u>Table Tips (%s)</u><br>",cGitVersion);
			printf("This is just a glue table, see [New], [Delete] and <a href=?gcFunction=tGroup>tGroup</a>"
				" for more info.<br>");
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
	sprintf(gcQuery,"SELECT %s FROM tGroupGlue ORDER BY\
					uGroupGlue",
					VAR_LIST_tGroupGlue);

}//void ExttGroupGlueSelect(void)


void ExttGroupGlueSelectRow(void)
{
	sprintf(gcQuery,"SELECT %s FROM tGroupGlue WHERE uGroupGlue=%u",
			VAR_LIST_tGroupGlue,uGroupGlue);

}//void ExttGroupGlueSelectRow(void)


void ExttGroupGlueListSelect(void)
{
	char cCat[512];

	sprintf(gcQuery,"SELECT %s FROM tGroupGlue",VAR_LIST_tGroupGlue);

	//Changes here must be reflected below in ExttGroupGlueListFilter()
        if(!strcmp(gcFilter,"uGroupGlue"))
        {
                sscanf(gcCommand,"%u",&uGroupGlue);
		sprintf(cCat," WHERE tGroupGlue.uGroupGlue=%u ORDER BY uGroupGlue",uGroupGlue);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uGroup"))
        {
                sscanf(gcCommand,"%u",&uGroup);
		sprintf(cCat," WHERE tGroupGlue.uGroup=%u ORDER BY uGroup",uGroup);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uNode"))
        {
                sscanf(gcCommand,"%u",&uNode);
		sprintf(cCat," WHERE tGroupGlue.uNode=%u ORDER BY uNode",uNode);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uContainer"))
        {
                sscanf(gcCommand,"%u",&uContainer);
		sprintf(cCat," WHERE tGroupGlue.uContainer=%u ORDER BY uContainer",uContainer);
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
        if(strcmp(gcFilter,"uGroup"))
                printf("<option>uGroup</option>");
        else
                printf("<option selected>uGroup</option>");
        if(strcmp(gcFilter,"uNode"))
                printf("<option>uNode</option>");
        else
                printf("<option selected>uNode</option>");
        if(strcmp(gcFilter,"uContainer"))
                printf("<option>uContainer</option>");
        else
                printf("<option selected>uContainer</option>");
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

	if(guPermLevel>=12 && !guListMode)
		printf(LANG_NBB_NEW);

	if(guPermLevel>=12 && !guListMode)
		printf(LANG_NBB_MODIFY);

	if(guPermLevel>=12 && !guListMode)
		printf(LANG_NBB_DELETE);

	if(guPermLevel>=12)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttGroupGlueNavBar(void)


void tTableMultiplePullDown(const char *cTableName,const char *cFieldName,const char *cOrderby)
{
        register int i,n;
        char cLabel[256];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[100]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;

        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTableMultiplePullDown()");
                return;
        }

        //Extended functionality
        strncpy(cLocalTableName,cTableName,255);
        if((cp=strchr(cLocalTableName,';')))
        {
                strncpy(cSelectName,cp+1,99);
                cSelectName[99]=0;
                *cp=0;
        }


        sprintf(gcQuery,"SELECT _rowid,%s FROM %s ORDER BY %s",
                                cFieldName,cLocalTableName,cOrderby);

	MYSQL_RUN_STORE_TEXT_RET_VOID(mysqlRes);
	
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
                printf("<select multiple size=%u name=%s >\n",(i>16)?16:i,cLabel);

                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);
			printf("<option>%s</option>\n",mysqlField[1]);
                }
        }
        else
        {
		printf("<select multiple size=1 name=%s><option title='No selection'>---</option></select>\n"
                        ,cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTableMultiplePullDown()

