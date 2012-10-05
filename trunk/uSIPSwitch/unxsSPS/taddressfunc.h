/*
FILE
	$Id: modulefunc.h 2116 2012-09-19 23:00:28Z Gary $
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



void tAddressNavList(void);

void ExtProcesstAddressVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstAddressVars(pentry entries[], int x)


void ExttAddressCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tAddressTools"))
	{
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstAddressVars(entries,x);
                        	guMode=2000;
	                        tAddress(LANG_NB_CONFIRMNEW);
			}
			else
				tAddress("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstAddressVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uAddress=0;
#ifdef StandardFields
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
#endif
				NewtAddress(0);
			}
			else
				tAddress("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstAddressVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
	                        guMode=2001;
				tAddress(LANG_NB_CONFIRMDEL);
			}
			else
				tAddress("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstAddressVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=5;
				DeletetAddress();
			}
			else
				tAddress("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstAddressVars(entries,x);
#ifdef StandardFields
			if(uAllowMod(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=2002;
				tAddress(LANG_NB_CONFIRMMOD);
			}
			else
				tAddress("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstAddressVars(entries,x);
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
				ModtAddress();
			}
			else
				tAddress("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttAddressCommands(pentry entries[], int x)


void ExttAddressButtons(void)
{
	OpenFieldSet("tAddress Aux Panel",100);
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
			printf("<p><u>Operations</u><br>");
			//printf("<br><input type=submit class=largeButton title='Sample button help'"
			//		" name=gcCommand value='Sample Button'>");
			tAddressNavList();
	}
	CloseFieldSet();

}//void ExttAddressButtons(void)


void ExttAddressAuxTable(void)
{

}//void ExttAddressAuxTable(void)


void ExttAddressGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uAddress"))
		{
			sscanf(gentries[i].val,"%u",&uAddress);
			guMode=6;
		}
	}
	tAddress("");

}//void ExttAddressGetHook(entry gentries[], int x)


void ExttAddressSelect(void)
{

#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tAddress ORDER BY"
				" uAddress",
				VAR_LIST_tAddress);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tAddress,tClient WHERE tAddress.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uAddress",
					VAR_LIST_tAddress,uContactParentCompany,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tAddress ORDER BY uAddress",VAR_LIST_tAddress);
#endif
					

}//void ExttAddressSelect(void)


void ExttAddressSelectRow(void)
{
#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tAddress WHERE uAddress=%u",
			VAR_LIST_tAddress,uAddress);
	else
                sprintf(gcQuery,"SELECT %s FROM tAddress,tClient"
                                " WHERE tAddress.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tAddress.uAddress=%u",
                        		VAR_LIST_tAddress
					,uContactParentCompany,uContactParentCompany
					,uAddress);
#else
	sprintf(gcQuery,"SELECT %s FROM tAddress WHERE uAddress=%u",VAR_LIST_tAddress,uAddress);
#endif

}//void ExttAddressSelectRow(void)


void ExttAddressListSelect(void)
{
	char cCat[512];
#ifdef StandardFields
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tAddress",
				VAR_LIST_tAddress);
	else
		sprintf(gcQuery,"SELECT %s FROM tAddress,tClient"
				" WHERE tAddress.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tAddress
				,uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tAddress",VAR_LIST_tAddress);
#endif

	//Changes here must be reflected below in ExttAddressListFilter()
        if(!strcmp(gcFilter,"uAddress"))
        {
                sscanf(gcCommand,"%u",&uAddress);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tAddress.uAddress=%u"
						" ORDER BY uAddress",
						uAddress);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uAddress");
        }

}//void ExttAddressListSelect(void)


void ExttAddressListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uAddress"))
                printf("<option>uAddress</option>");
        else
                printf("<option selected>uAddress</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttAddressListFilter(void)


void ExttAddressNavBar(void)
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

}//void ExttAddressNavBar(void)


void tAddressNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	//Only show gateways	
	if(guLoginClient==1 && guPermLevel>11)
		sprintf(gcQuery,"SELECT tAddress.uAddress,tAddress.cLabel,tAddress.cIP,tAddress.uPort,tGateway.cLabel FROM tAddress,tGateway"
				" WHERE tAddress.uGateway=tGateway.uGateway"
				" ORDER BY tAddress.cLabel");
	else
		sprintf(gcQuery,"SELECT tAddress.uAddress,"
				" tAddress.cLabel,tAddress.cIP,tAddress.uPort,tGateway.cLabel"
				" FROM tAddress,tClient,tGateway"
				" WHERE tAddress.uOwner=tClient.uClient"
				" AND tAddress.uGateway=tGateway.uGateway"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
					guCompany,guCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tAddressNavList GWs only</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tAddressNavList GWs only</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tAddress"
				"&uAddress=%s>%s/%s/%s/%s</a><br>\n",
				field[0],field[1],field[2],field[3],field[4]);
	}
        mysql_free_result(res);

}//void tAddressNavList(void)


