/*
FILE
	$Id$
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Unixservice, LLC.
*/

static unsigned uClone=0;
static unsigned uOldDatacenter=0;
//ModuleFunctionProtos()
void tDatacenterNavList(void);
void tDatacenterHealth(void);

//tnodefunc.h
void CopyProperties(unsigned uOldNode,unsigned uNewNode,unsigned uType);
void DelProperties(unsigned uNode,unsigned uType);
void tNodeNavList(unsigned uDataCenter);

//tgroupfunc.h
void tGroupNavList(void);

//tcontainerfunc.h
char *cRatioColor(float *fRatio);

void ExtProcesstDatacenterVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uClone")) 
			uClone=1;
	}
}//void ExtProcesstDatacenterVars(pentry entries[], int x)


void ExttDatacenterCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tDatacenterTools"))
	{
        	MYSQL_RES *res;
		time_t uActualModDate= -1;

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
				uOldDatacenter=uDatacenter;
	                        ProcesstDatacenterVars(entries,x);
                        	guMode=2000;
	                        tDatacenter(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
                        	ProcesstDatacenterVars(entries,x);
				uOldDatacenter=uDatacenter;

                        	guMode=2000;
				//Check entries here
				if(strlen(cLabel)<3)
					tDatacenter("<blink>Error</blink>: Must supply valid cLabel. Min 3 chars.");
				sprintf(gcQuery,"SELECT uDatacenter FROM tDatacenter WHERE cLabel='%s'",
						cLabel);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tDatacenter("<blink>Error</blink>: Datacenter cLabel is used!");
				}
                        	guMode=0;

				uDatacenter=0;
				uCreatedBy=guLoginClient;
				GetClientOwner(guLoginClient,&guReseller);
				uOwner=guReseller;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				uStatus=1;//Active
				NewtDatacenter(1);
				sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=1"
						",cName='Contact',uOwner=%u,uCreatedBy=%u"
						",uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,uDatacenter,guCompany,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				if(uDatacenter && uOldDatacenter && uClone)
					CopyProperties(uOldDatacenter,uDatacenter,1);
				if(uDatacenter)
					tDatacenter("New datacenter created");
				else
					tDatacenter("<blink>Error</blink>: Datacenter not created!");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstDatacenterVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=0;
				sscanf(ForeignKey("tDatacenter","uModDate",uDatacenter),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tDatacenter("<blink>Error</blink>: This record was modified. Reload it.");
				sprintf(gcQuery,"SELECT uDatacenter FROM tContainer WHERE uDatacenter=%u",
									uDatacenter);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tDatacenter("<blink>Error</blink>: Can't delete a datacenter"
							" used by a container!");
				}
	                        guMode=2001;
				tDatacenter(LANG_NB_CONFIRMDEL);
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstDatacenterVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				sscanf(ForeignKey("tDatacenter","uModDate",uDatacenter),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tDatacenter("<blink>Error</blink>: This record was modified. Reload it.");
				sprintf(gcQuery,"SELECT uDatacenter FROM tContainer WHERE uDatacenter=%u",
									uDatacenter);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tDatacenter("<blink>Error</blink>: Can't delete a datacenter"
							" used by a container!");
				}
	                        guMode=0;
				DelProperties(uDatacenter,2);
				DeletetDatacenter();
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstDatacenterVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{

				guMode=2002;
				tDatacenter(LANG_NB_CONFIRMMOD);
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstDatacenterVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
				if(strlen(cLabel)<3)
					tDatacenter("<blink>Error</blink>: Must supply valid cLabel. Min 3 chars.");
                        	guMode=0;

				uModBy=guLoginClient;
				ModtDatacenter();
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttDatacenterCommands(pentry entries[], int x)


void ExttDatacenterButtons(void)
{
	OpenFieldSet("tDatacenter Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter/mod data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
			if(uOldDatacenter)
				printf("<p>Copy properties <input title='Copies all properties'"
					" type=checkbox name=uClone checked>\n");
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
			printf("A datacenter is a collection of hardware nodes."
					" These hardware nodes need not be located in the same"
					" physical datacenter. Hardware nodes host VZ containers."
					" It is in these containers that actual public services run.");
			tGroupNavList();
			tNodeNavList(uDatacenter);
			tDatacenterNavList();
			tDatacenterHealth();
	}
	CloseFieldSet();

}//void ExttDatacenterButtons(void)


void ExttDatacenterAuxTable(void)
{
	if(!uDatacenter || guMode==2000 )//uMODE_NEW
		return;

        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"%s Property Panel",cLabel);
	OpenFieldSet(gcQuery,100);
	sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE uKey=%u AND uType=1 ORDER BY cName",uDatacenter);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		printf("<table cols=2>");
		while((field=mysql_fetch_row(res)))
		{
			printf("<tr>\n");
			printf("<td width=200 valign=top><a class=darkLink href=unxsVZ.cgi?"
					"gcFunction=tProperty&uProperty=%s&cReturn=tDatacenter_%u>"
					"%s</a></td><td valign=top><pre>%s</pre></td>\n",
						field[0],uDatacenter,field[1],field[2]);
			printf("</tr>\n");
		}
		printf("</table>");
	}

	CloseFieldSet();

}//void ExttDatacenterAuxTable(void)


void ExttDatacenterGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uDatacenter"))
		{
			sscanf(gentries[i].val,"%u",&uDatacenter);
			guMode=6;
		}
	}
	tDatacenter("");

}//void ExttDatacenterGetHook(entry gentries[], int x)


void ExttDatacenterSelect(void)
{
	ExtSelect("tDatacenter",VAR_LIST_tDatacenter);

}//void ExttDatacenterSelect(void)


void ExttDatacenterSelectRow(void)
{
	ExtSelectRow("tDatacenter",VAR_LIST_tDatacenter,uDatacenter);

}//void ExttDatacenterSelectRow(void)


void ExttDatacenterListSelect(void)
{
	char cCat[512];

	ExtListSelect("tDatacenter",VAR_LIST_tDatacenter);

	//Changes here must be reflected below in ExttDatacenterListFilter()
        if(!strcmp(gcFilter,"uDatacenter"))
        {
                sscanf(gcCommand,"%u",&uDatacenter);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tDatacenter.uDatacenter=%u ORDER BY uDatacenter",uDatacenter);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uDatacenter");
        }

}//void ExttDatacenterListSelect(void)


void ExttDatacenterListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uDatacenter"))
                printf("<option>uDatacenter</option>");
        else
                printf("<option selected>uDatacenter</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttDatacenterListFilter(void)


void ExttDatacenterNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=9 && !guListMode)
		printf(LANG_NBB_NEW);

	if(uAllowMod(uOwner,uCreatedBy))
		printf(LANG_NBB_MODIFY);

	if(uAllowDel(uOwner,uCreatedBy))
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttDatacenterNavBar(void)


void tDatacenterNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tDatacenter","tDatacenter.uDatacenter,tDatacenter.cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tDatacenterNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tDatacenterNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tDatacenter&uDatacenter=%s>"
				"%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tDatacenterNavList(void)


void tDatacenterHealth(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;


	//1-. Disk space usage/soft limit ratio
	//1a-. Create temp table
	sprintf(gcQuery,"CREATE TEMPORARY TABLE tDiskUsage (uContainer INT UNSIGNED NOT NULL DEFAULT 0,"
			" luUsage INT UNSIGNED NOT NULL DEFAULT 0, luSoftlimit INT UNSIGNED NOT NULL DEFAULT 0,"
			" cLabel VARCHAR(32) NOT NULL DEFAULT '')");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tDatacenterHealth</u><br>\n");
                printf("a-. %s",mysql_error(&gMysql));
                return;
        }

	//1b-. Populate with data per container
	sprintf(gcQuery,"SELECT tProperty.uKey,tProperty.cValue,tContainer.cLabel FROM tProperty,tContainer"
			" WHERE tProperty.uKey=tContainer.uContainer AND tProperty.uType=3 AND"
			" tProperty.cName='1k-blocks.luUsage' AND tContainer.uDatacenter=%u"
			" AND tContainer.uStatus=1",uDatacenter);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tDatacenterHealth</u><br>\n");
                printf("0-. %s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{	
		sprintf(gcQuery,"INSERT INTO tDiskUsage SET uContainer=%s,luUsage=%s,cLabel='%.32s'",
			field[0],field[1],field[2]);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
        	{
        		printf("<p><u>tDatacenterHealth</u><br>\n");
			printf("1-. %s",mysql_error(&gMysql));
			return;
		}
	}
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT tProperty.uKey,tProperty.cValue FROM tProperty,tContainer"
			" WHERE tProperty.uKey=tContainer.uContainer AND tProperty.uType=3 AND"
			" tProperty.cName='1k-blocks.luSoftLimit' AND tContainer.uDatacenter=%u"
			" AND tContainer.uStatus=1",uDatacenter);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tDatacenterHealth</u><br>\n");
                printf("2-. %s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{	
		sprintf(gcQuery,"UPDATE tDiskUsage SET luSoftlimit=%s WHERE uContainer=%s",field[1],field[0]);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
        	{
        		printf("<p><u>tDatacenterHealth</u><br>\n");
			printf("3-. %s",mysql_error(&gMysql));
			return;
		}
	}
	mysql_free_result(res);

	//1d-. Report
	unsigned luSoftlimit;
	unsigned luUsage;
	float fRatio;
	char *cColor;
		
	sprintf(gcQuery,"SELECT luSoftlimit,luUsage,uContainer,cLabel FROM tDiskUsage"
			" WHERE ((luUsage/luSoftlimit)>0.5)"
			" ORDER BY (luUsage/luSoftlimit) DESC LIMIT 20");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
		printf("4-. %s",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
        	printf("<p><u>Top 20 Containers by Usage Ratio (50%%+)</u><br>\n");
	while((field=mysql_fetch_row(res)))
	{
		luSoftlimit=0;
		luUsage=0;
		sscanf(field[0],"%u",&luSoftlimit);
		sscanf(field[1],"%u",&luUsage);
		//Strange values hack
		if(!luUsage)
			luUsage=1;
		if(!luSoftlimit)
			luSoftlimit=luUsage;
		fRatio= ((float) luUsage/ (float) luSoftlimit) * 100.00 ;
		cColor=cRatioColor(&fRatio);

		printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>"
				"<font color=%s>%2.2f%% %s</font></a><br>\n",field[2],cColor,fRatio,field[3]);
	}
        mysql_free_result(res);


	//2-. None zero historic fail counters
	sprintf(gcQuery,"SELECT cValue,uKey,cLabel,cName FROM tProperty,tContainer WHERE"
			" tProperty.uKey=tContainer.uContainer AND cValue!='0' AND uType=3 AND cName LIKE '%%.luFailcnt'"
			" ORDER BY CONVERT(cValue,UNSIGNED) DESC LIMIT 10");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tDatacenterHealth</u><br>\n");
                printf("5-. %s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>Top 10 Containers by X.luFailcnt</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>"
				"%s %s=%s</a><br>\n",field[1],field[2],field[3],field[0]);
	}
        mysql_free_result(res);

}//void tDatacenterHealth(void)


