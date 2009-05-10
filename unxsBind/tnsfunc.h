/*
FILE
	$Id: tnsfunc.h 713 2009-02-14 14:59:46Z dylan $
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza for Unixservice.
 
*/

//ModuleFunctionProtos()


void tNSNavList(void);
void tNSContextInfo(void);

void ExtProcesstNSVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstNSVars(pentry entries[], int x)


void ExttNSCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tNSTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstNSVars(entries,x);
                        	guMode=2000;
	                        tNS(LANG_NB_CONFIRMNEW);
			}
			else
				tNS("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstNSVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uNS=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtNS(0);
			}
			else
				tNS("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstNSVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tNS(LANG_NB_CONFIRMDEL);
			}
			else
				tNS("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstNSVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetNS();
			}
			else
				tNS("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstNSVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tNS(LANG_NB_CONFIRMMOD);
			}
			else
				tNS("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstNSVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtNS();
			}
			else
				tNS("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttNSCommands(pentry entries[], int x)


void ExttNSButtons(void)
{
	OpenFieldSet("tNS Aux Panel",100);
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
			tNSContextInfo();
			tNSNavList();
	}
	CloseFieldSet();

}//void ExttNSButtons(void)


void ExttNSAuxTable(void)
{

}//void ExttNSAuxTable(void)


void ExttNSGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uNS"))
		{
			sscanf(gentries[i].val,"%u",&uNS);
			guMode=6;
		}
	}
	tNS("");

}//void ExttNSGetHook(entry gentries[], int x)


void ExttNSSelect(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tNS ORDER BY"
				" uNS",
				VAR_LIST_tNS);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tNS,tClient WHERE tNS.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uNS",
					VAR_LIST_tNS,uContactParentCompany,uContactParentCompany);
					

}//void ExttNSSelect(void)


void ExttNSSelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tNS WHERE uNS=%u",
			VAR_LIST_tNS,uNS);
	else
                sprintf(gcQuery,"SELECT %s FROM tNS,tClient"
                                " WHERE tNS.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tNS.uNS=%u",
                        		VAR_LIST_tNS
					,uContactParentCompany,uContactParentCompany
					,uNS);

}//void ExttNSSelectRow(void)


void ExttNSListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tNS",
				VAR_LIST_tNS);
	else
		sprintf(gcQuery,"SELECT %s FROM tNS,tClient"
				" WHERE tNS.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tNS
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttNSListFilter()
        if(!strcmp(gcFilter,"uNS"))
        {
                sscanf(gcCommand,"%u",&uNS);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tNS.uNS=%u"
						" ORDER BY uNS",
						uNS);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uNS");
        }

}//void ExttNSListSelect(void)


void ExttNSListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uNS"))
                printf("<option>uNS</option>");
        else
                printf("<option selected>uNS</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttNSListFilter(void)


void ExttNSNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
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

}//void ExttNSNavBar(void)


void tNSNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uNS,cFQDN FROM tNS ORDER BY cFQDN");
	else
		sprintf(gcQuery,"SELECT tNS.uNS,"
				" tNS.cFQDN"
				" FROM tNS,tClient"
				" WHERE tNS.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tNSNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tNSNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=iDNS.cgi?gcFunction=tNS"
				"&uNS=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tNSNavList(void)


void tNSContextInfo(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(!uNS) return;

	sprintf(gcQuery,"SELECT COUNT(tZone.uZone) FROM tNSSet,tNS,tZone WHERE"
			" tZone.uNSSet=tNSSet.uNSSet AND tNSSet.uNSSet=tNS.uNSSet AND"
			" tNS.uNS=%u",uNS);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("tNSContextInfo:<br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
        	printf("tNSContextInfo:<br>\n");
		printf("%s zones are assigned to NS sets that contain this NS.",field[0]);
	}
        mysql_free_result(res);

}//void tNSContextInfo(void)


