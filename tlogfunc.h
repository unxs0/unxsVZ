/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR/LEGAL
	(C) 2007-2010 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
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

	printf("<u>Table Tips (%s)</u><br>",cGitVersion);
	printf("This table holds the non-archived logged and MD5 signed operations that have taken place in the system."
		" Usually data is available here only for the current month. When possible context related info is"
		" provided below. Current unxsVZ version does not save delete (Del) operation data.<p>"
		"<a href=unxsVZ.cgi?gcFunction=tLogMonth>tLogMonth</a> allows access to all archived (read-only and"
		" compressed) monthly tLog data sets. These archives are created from the command line usually by crontab"
		" operation.");
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
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tLog ORDER BY uLog",VAR_LIST_tLog);
        else
                sprintf(gcQuery,"SELECT %s FROM tLog," TCLIENT
                                " WHERE tLog.uOwner=tClient.uClient"
                                " AND (tClient.uClient=%u OR tClient.uOwner"
                                " IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%u OR uClient=%u)) ORDER BY uLog",
                                        VAR_LIST_tLog,guLoginClient,guCompany,guCompany);
}//void ExttLogSelect(void)


void ExttLogSelectRow(void)
{
	ExtSelectRow("tLog",VAR_LIST_tLog,uLog);

}//void ExttLogSelectRow(void)


void ExttLogListSelect(void)
{
	char cCat[512];

	if(guPermLevel<12)
		return;

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
		sprintf(cCat,"tLog.cLabel LIKE '%s' ORDER BY uLog",gcCommand);
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

	if(guPermLevel>=12 && uLog)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttLogNavBar(void)


void LogSummary(void)
{

	if(!uLog || !cLabel[0] || guPermLevel<10)
		return;
	//table based summaries
	if(uTablePK[0] && cTableName[0])
	{
		printf("<p><u>LogSummary table based</u><br>\n");
		unsigned uTPK=0;
		sscanf(uTablePK,"%u",&uTPK);

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

				printf("<a title='Jump to tResource entry' href=unxsVZ.cgi?gcFunction=tResource&"
					"uResource=%u>tResource</a><blockquote>\n",uTPK);
				printf("cZone=%s<br>\n",ForeignKey("tZone","cZone",uZone));
				printf("cName=%s<br>\n",ForeignKey("tResource","cName",uTPK));
				printf("RRType=%s<br>\n",ForeignKey("tRRType","cLabel",uRRType));
				printf("cParam1=%s<br>\n",ForeignKey("tResource","cParam1",uTPK));
				printf("cParam2=%s<br>\n",ForeignKey("tResource","cParam2",uTPK));
				printf("cComment=%s<br>\n",ForeignKey("tResource","cComment",uTPK));
			}

			if(uLoginClient)
			{
				printf("Contact=<a title='Jump to tClient entry' href=unxsVZ.cgi?gcFunction="
					"tClient&uClient=%u>%s</a><br></blockquote>\n",uLoginClient,
				ForeignKey("tClient","cLabel",uLoginClient));
			}
		}
		else if(1)
		{
			printf("No summary available for %s\n",cTableName);
		}
	}
	else if(uLoginClient && !strncmp(cLabel,"login",5))
	{
		MYSQL_RES *res;
		MYSQL_ROW field;
		
		sprintf(gcQuery,"SELECT COUNT(tLog.uLog),tClient.cLabel,FROM_UNIXTIME(MAX(tLog.uCreatedDate))"
				" FROM tLog,tClient WHERE tClient.uClient=tLog.uLoginClient"
				" AND tLog.cLabel LIKE 'login%%' AND tLog.uLoginClient>0 AND tLog.uLogType=8"
				" GROUP BY tLog.uLoginClient ORDER BY COUNT(uLog) DESC");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("<p>Error: %s",mysql_error(&gMysql));
			return;
		}
        	res=mysql_store_result(&gMysql);
		unsigned uUniqueLogins=mysql_num_rows(res);
		printf("<p><u>LogSummary Customer Logins (%u unique)</u><br>\n",uUniqueLogins);
		while((field=mysql_fetch_row(res)))
			printf("%s %s %s<br>\n",field[0],field[1],field[2]);

		sprintf(gcQuery,"SELECT COUNT(uLog),tClient.cLabel,FROM_UNIXTIME(MAX(tLog.uCreatedDate))"
				" FROM tLog,tClient WHERE tClient.uClient=tLog.uLoginClient"
				" AND tLog.cLabel LIKE 'login%%' AND tLog.uLoginClient>0 AND tLog.uLogType=6"
				" GROUP BY tLog.uLoginClient ORDER BY COUNT(uLog) DESC");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s",mysql_error(&gMysql));
			return;
		}
        	res=mysql_store_result(&gMysql);
		uUniqueLogins=mysql_num_rows(res);
		printf("<p><u>LogSummary Backend Logins (%u unique)</u><br>\n",uUniqueLogins);
		while((field=mysql_fetch_row(res)))
			printf("%s %s %s<br>\n",field[0],field[1],field[2]);
		mysql_free_result(res);

		
	}
}//void LogSummary(void);

