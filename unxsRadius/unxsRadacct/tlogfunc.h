/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
 
*/

//ModuleFunctionProtos()

void LogSummary(void);


void ExtProcesstLogVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstLogVars(pentry entries[], int x)


void ExttLogCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tLogTools"))
	{

	}

}//void ExttLogCommands(pentry entries[], int x)


void ExttLogButtons(void)
{
	OpenFieldSet("tLog Aux Panel",100);

	printf("<u>Table Tips</u><br>");
	printf("This table holds the non-archived logged and MD5 signed operations that have taken place in the system. Usually data is available here only for the current month. When possible context related info is provided below. Current unxsRadacct version does not save delete (Del) operation data.<p><a href=unxsRadacct.cgi?gcFunction=tLogMonth>tLogMonth</a> allows access to all archived (read-only and compressed) monthly tLog data sets. These archives are created from the command line usually by crontab operation.");

	LogSummary();

	CloseFieldSet();

}//void ExttLogButtons(void)


void ExttLogAuxTable(void)
{

}//void ExttLogAuxTable(void)


void ExttLogGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uLog"))
		{
			sscanf(gentries[i].val,"%u",&uLog);
			guMode=6;
		}
	}
	tLog("");

}//void ExttLogGetHook(entry gentries[], int x)


void ExttLogSelect(void)
{
        //Set non search gcQuery here for tTableName()
	if(guPermLevel>=9)
	sprintf(gcQuery,"SELECT %s FROM tLog ORDER BY\
					uLog",
					VAR_LIST_tLog);
	else
	sprintf(gcQuery,"SELECT %s FROM tLog WHERE uOwner=%u ORDER BY\
					uLog",
					VAR_LIST_tLog,guLoginClient);

}//void ExttLogSelect(void)


void ExttLogSelectRow(void)
{
	if(guPermLevel<10)
                sprintf(gcQuery,"SELECT %s FROM tLog,tClient \
                                WHERE tLog.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)\
                                AND tLog.uLog=%u",
                        		VAR_LIST_tLog,
					guLoginClient,guLoginClient,uLog);
	else
                sprintf(gcQuery,"SELECT %s FROM tLog WHERE uLog=%u",
			VAR_LIST_tLog,uLog);

}//void ExttLogSelectRow(void)


void ExttLogListSelect(void)
{
	char cCat[512];

	sprintf(gcQuery,"SELECT %s FROM tLog",
				VAR_LIST_tLog);

	//Changes here must be reflected below in ExttLogListFilter()
        if(!strcmp(gcFilter,"uLog"))
        {
                sscanf(gcCommand,"%u",&uLog);
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tLog.uLog=%u ORDER BY uLog",uLog);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cLabel"))
        {
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tLog.cLabel='%s' ORDER BY uLog",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uLogType"))
        {
                sscanf(gcCommand,"%u",&uLogType);
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tLog.uLogType=%u ORDER BY uLog",uLogType);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uLoginClient"))
        {
                sscanf(gcCommand,"%u",&uLoginClient);
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tLog.uLoginClient=%u ORDER BY uLog",uLoginClient);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cLogin"))
        {
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tLog.cLogin LIKE '%s' ORDER BY cLogin,uLog",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cHost"))
        {
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tLog.cHost LIKE '%s' ORDER BY cHost,uLog",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uTablePK"))
        {
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tLog.uTablePK=%s ORDER BY cTableName,uTablePK,uLog",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uZone"))
        {
		unsigned uZone=0;

                sscanf(gcCommand,"%u",&uZone);
		sprintf(cCat,",tResource WHERE tLog.uTablePK=tResource.uResource AND cTableName='tResource' AND tResource.uZone=%u",uZone);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uLog");
        }

}//void ExttLogListSelect(void)


void ExttLogListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uLog"))
                printf("<option>uLog</option>");
        else
                printf("<option selected>uLog</option>");
        if(strcmp(gcFilter,"cLabel"))
                printf("<option>cLabel</option>");
        else
                printf("<option selected>cLabel</option>");
        if(strcmp(gcFilter,"uLogType"))
                printf("<option>uLogType</option>");
        else
                printf("<option selected>uLogType</option>");
        if(strcmp(gcFilter,"uLoginClient"))
                printf("<option>uLoginClient</option>");
        else
                printf("<option selected>uLoginClient</option>");
        if(strcmp(gcFilter,"cLogin"))
                printf("<option>cLogin</option>");
        else
                printf("<option selected>cLogin</option>");
        if(strcmp(gcFilter,"cHost"))
                printf("<option>cHost</option>");
        else
                printf("<option selected>cHost</option>");
        if(strcmp(gcFilter,"uTablePK"))
                printf("<option>uTablePK</option>");
        else
                printf("<option selected>uTablePK</option>");
        if(strcmp(gcFilter,"uZone"))
                printf("<option>uZone</option>");
        else
                printf("<option selected>uZone</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttLogListFilter(void)


void ExttLogNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttLogNavBar(void)


void LogSummary(void)
{

	if(uLog && cLabel[0] && guPermLevel>10 && uTablePK[0] && cTableName[0])
	{
		unsigned uTPK=0;
		sscanf(uTablePK,"%u",&uTPK);

		printf("<p><u>LogSummary</u><br>\n");
		if(!strcmp(cTableName,"tZone"))
		{
			printf("tZone<blockquote>\n");
			printf("%s<br>\n",ForeignKey("tZone","cZone",uTPK));
			printf("</blockquote>\n");
		}
		else if(!strcmp(cTableName,"tResource"))
		{
			unsigned uZone=0;
			unsigned uRRType=0;

			sscanf(ForeignKey("tResource","uZone",uTPK),"%u",&uZone);
			sscanf(ForeignKey("tResource","uRRType",uTPK),"%u",&uRRType);

			if( strcmp(cLabel,"Del") && uTPK && uZone && uRRType)
			{

				printf("<a title='Jump to tResource entry' href=unxsRadacct.cgi?gcFunction=tResource&uResource=%u>tResource</a><blockquote>\n",uTPK);
				printf("cZone=%s<br>\n",ForeignKey("tZone","cZone",uZone));
				printf("cName=%s<br>\n",ForeignKey("tResource","cName",uTPK));
				printf("RRType=%s<br>\n",ForeignKey("tRRType","cLabel",uRRType));
				printf("cParam1=%s<br>\n",ForeignKey("tResource","cParam1",uTPK));
				printf("cParam2=%s<br>\n",ForeignKey("tResource","cParam2",uTPK));
				printf("cComment=%s<br>\n",ForeignKey("tResource","cComment",uTPK));
			}

			if(uLoginClient)
			{
				printf("Contact=<a title='Jump to tClient entry' href=unxsRadacct.cgi?gcFunction=tClient&uClient=%u>%s</a><br></blockquote>\n",uLoginClient,
				ForeignKey("tClient","cLabel",uLoginClient));
			}
		}
		else if(1)
		{
			printf("No summary available for %s\n",cTableName);
		}
	}
}//void LogSummary(void);

