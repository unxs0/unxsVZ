/*
FILE
	$Id: tlogfunc.h 676 2007-07-04 15:48:09Z Gary $
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2006 Gary Wallis.
 
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
	printf("This table holds the non-archived logged and MD5 signed operations that have taken place in the system. "
		"Usually data is available here only for the current month. When possible context related info is provided below. "
		"Current unxsISP version does not save delete (Del) operation data.<p><a href=iDNS.cgi?gcFunction=tLogMonth>tLogMonth</a> "
		"allows access to all archived (read-only and compressed) monthly tLog data sets. These archives are created from the "
		"command line usually by crontab operation.");

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
	ExtSelect("tLog",VAR_LIST_tLog,0);

}//void ExttLogSelect(void)


void ExttLogSelectRow(void)
{
	ExtSelectRow("tLog",VAR_LIST_tLog,uLog);

}//void ExttLogSelectRow(void)


void ExttLogListSelect(void)
{
	char cCat[512];

	ExtListSelect("tLog",VAR_LIST_tLog);
	if(guPermLevel>11)
		strcat(gcQuery," WHERE ");
	else
		strcat(gcQuery," AND ");

	//Changes here must be reflected below in ExttLogListFilter()
        if(!strcmp(gcFilter,"uLog"))
        {
                sscanf(gcCommand,"%u",&uLog);
		if(guPermLevel>11)
		sprintf(cCat,"tLog.uLog=%u ORDER BY uLog",uLog);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cLabel"))
        {
		sprintf(cCat,"tLog.cLabel='%s' ORDER BY uLog",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uLogType"))
        {
                sscanf(gcCommand,"%u",&uLogType);
		sprintf(cCat,"tLog.uLogType=%u ORDER BY uLog",uLogType);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uLoginClient"))
        {
                sscanf(gcCommand,"%u",&uLoginClient);
		sprintf(cCat,"tLog.uLoginClient=%u ORDER BY uLog",uLoginClient);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cLogin"))
        {
		sprintf(cCat,"tLog.cLogin LIKE '%s' ORDER BY cLogin,uLog",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cHost"))
        {
		sprintf(cCat,"tLog.cHost LIKE '%s' ORDER BY cHost,uLog",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uTablePK"))
        {
		sprintf(cCat,"tLog.uTablePK=%s ORDER BY cTableName,uTablePK,uLog",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		//The uLog>0 is a fake condition to avoid duplicated code everywhere above
		strcat(gcQuery,"uLog>0 ORDER BY uLog");
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
        if(strcmp(gcFilter,"cTableName"))
                printf("<option>cTableName</option>");
        else
                printf("<option selected>cTableName</option>");
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
			char *cResourceTable="tResource";
			if(!strcmp(cLabel,"Del"))
				cResourceTable="tDeletedResource";

			sscanf(ForeignKey(cResourceTable,"uZone",uTPK),"%u",&uZone);
			sscanf(ForeignKey(cResourceTable,"uRRType",uTPK),"%u",&uRRType);

			if( uTPK && uZone && uRRType)
			{

				if(!strcmp(cLabel,"Del"))
					printf("<a title='Jump to tDeletedResource entry' href=unxsISP.cgi?gcFunction=tDeletedResource&uDeletedResource=%u>"
						"tDeletedResource</a><blockquote>\n",uTPK);
				else
					printf("<a title='Jump to tResource entry' href=unxsISP.cgi?gcFunction=tResource&uResource=%u>tResource</a>"
						"<blockquote>\n",uTPK);

				printf("cZone=%s<br>\n",ForeignKey("tZone","cZone",uZone));
				printf("cName=%s<br>\n",ForeignKey(cResourceTable,"cName",uTPK));
				printf("RRType=%s<br>\n",ForeignKey("tRRType","cLabel",uRRType));
				printf("cParam1=%s<br>\n",ForeignKey(cResourceTable,"cParam1",
							uTPK));
				printf("cParam2=%s<br>\n",ForeignKey(cResourceTable,"cParam2",
							uTPK));
				printf("cComment=%s<br>\n",ForeignKey(cResourceTable,"cComment",
							uTPK));
			}

			if(uLoginClient)
			{
				printf("Contact=<a title='Jump to tClient entry' href=unxsISP.cgi?gcFunction=tClient&uClient=%u>%s</a><br></blockquote>\n",
					uLoginClient,
					ForeignKey(TCLIENT,"cLabel",uLoginClient));
			}
		}
		else if(1)
		{
			printf("No summary available for %s\n",cTableName);
		}
	}
}//void LogSummary(void);

