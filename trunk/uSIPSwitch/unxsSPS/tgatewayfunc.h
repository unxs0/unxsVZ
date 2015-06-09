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


static unsigned uGroup=0;
static char cuGroupPullDown[256]={""};

void tGatewayNavList(void);
void tGatewaytAddressNavList(void);
void tGatewaytGroupNavList(void);

void ExtProcesstGatewayVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uGroup"))
			sscanf(entries[i].val,"%u",&uGroup);
		else if(!strcmp(entries[i].name,"cuGroupPullDown"))
		{
			sprintf(cuGroupPullDown,"%.255s",entries[i].val);
			uGroup=ReadPullDown("tGroup","cLabel",cuGroupPullDown);
		}
	}
}//void ExtProcesstGatewayVars(pentry entries[], int x)


void ExttGatewayCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tGatewayTools"))
	{
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstGatewayVars(entries,x);
                        	guMode=2000;
	                        tGateway(LANG_NB_CONFIRMNEW);
			}
			else
				tGateway("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstGatewayVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uGateway=0;
#ifdef StandardFields
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
#endif
				NewtGateway(0);
			}
			else
				tGateway("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstGatewayVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
	                        guMode=2001;
				tGateway(LANG_NB_CONFIRMDEL);
			}
			else
				tGateway("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstGatewayVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=5;
				DeletetGateway();
			}
			else
				tGateway("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstGatewayVars(entries,x);
#ifdef StandardFields
			if(uAllowMod(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=2002;
				tGateway(LANG_NB_CONFIRMMOD);
			}
			else
				tGateway("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstGatewayVars(entries,x);
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
				ModtGateway();
			}
			else
				tGateway("<blink>Error</blink>: Denied by permissions settings");
                }
		//Custom operations
		else if(!strcmp(gcCommand,"Add to Group"))
                {
                        ProcesstGatewayVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) && uGateway)
			{
				if(uGroup)
				{
        				MYSQL_RES *res;

					sprintf(gcQuery,"SELECT uGroupGlue FROM tGroupGlue WHERE uKey=%u"
							" AND uGroupType=5"
							" AND uGroup=%u",
								uGateway,uGroup);
        				mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
                				tRule(gcQuery);
				        res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)>0)
						tGateway("Gateway has already been added");
					sprintf(gcQuery,"INSERT INTO tGroupGlue SET uKey=%u,"
							"uGroupType=5,"
							"uGroup=%u",uGateway,uGroup);
        				mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
                				tRule(gcQuery);
					tGateway("Gateway added");
				}
				tGateway("No group selected");
			}
			else
				tGateway("<blink>Error</blink>: Denied by permissions settings");
		}
	}

}//void ExttGatewayCommands(pentry entries[], int x)


void ExttGatewayButtons(void)
{
	OpenFieldSet("tGateway Aux Panel",100);
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
			printf("Gateways are the carrier SIP proxies (or other servers of our own, like CLEC or CPE SIP equipment)"
				" usually for DID or PSTN services.");
			printf("<p><u>Record Context Info</u><br>");
			if(uGateway)
				tGatewaytGroupNavList();
			printf("<p><u>Operations</u><br>");
			tTablePullDown("tGroup;cuGroupPullDown","cLabel","cLabel",uGroup,1);
			printf("<br><input type=submit class=largeButton title='Add this GW to the above GW tGroup'"
					" name=gcCommand value='Add to Group'>");
			if(uGateway)
				tGatewaytAddressNavList();
			tGatewayNavList();
	}
	CloseFieldSet();

}//void ExttGatewayButtons(void)


void ExttGatewayAuxTable(void)
{

}//void ExttGatewayAuxTable(void)


void ExttGatewayGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uGateway"))
		{
			sscanf(gentries[i].val,"%u",&uGateway);
			guMode=6;
		}
	}
	tGateway("");

}//void ExttGatewayGetHook(entry gentries[], int x)


void ExttGatewaySelect(void)
{

#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tGateway ORDER BY"
				" uGateway",
				VAR_LIST_tGateway);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tGateway,tClient WHERE tGateway.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uGateway",
					VAR_LIST_tGateway,uContactParentCompany,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tGateway ORDER BY uGateway",VAR_LIST_tGateway);
#endif
					

}//void ExttGatewaySelect(void)


void ExttGatewaySelectRow(void)
{
#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tGateway WHERE uGateway=%u",
			VAR_LIST_tGateway,uGateway);
	else
                sprintf(gcQuery,"SELECT %s FROM tGateway,tClient"
                                " WHERE tGateway.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tGateway.uGateway=%u",
                        		VAR_LIST_tGateway
					,uContactParentCompany,uContactParentCompany
					,uGateway);
#else
	sprintf(gcQuery,"SELECT %s FROM tGateway WHERE uGateway=%u",VAR_LIST_tGateway,uGateway);
#endif

}//void ExttGatewaySelectRow(void)


void ExttGatewayListSelect(void)
{
	char cCat[512];
#ifdef StandardFields
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tGateway",
				VAR_LIST_tGateway);
	else
		sprintf(gcQuery,"SELECT %s FROM tGateway,tClient"
				" WHERE tGateway.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tGateway
				,uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tGateway",VAR_LIST_tGateway);
#endif

	//Changes here must be reflected below in ExttGatewayListFilter()
        if(!strcmp(gcFilter,"uGateway"))
        {
                sscanf(gcCommand,"%u",&uGateway);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tGateway.uGateway=%u"
						" ORDER BY uGateway",
						uGateway);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uGateway");
        }

}//void ExttGatewayListSelect(void)


void ExttGatewayListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uGateway"))
                printf("<option>uGateway</option>");
        else
                printf("<option selected>uGateway</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttGatewayListFilter(void)


void ExttGatewayNavBar(void)
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

}//void ExttGatewayNavBar(void)


void tGatewayNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
#ifdef StandardFields
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uGateway,cLabel FROM tGateway ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tGateway.uGateway,"
				" tGateway.cLabel"
				" FROM tGateway,tClient"
				" WHERE tGateway.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT uGateway,cLabel FROM tGateway ORDER BY cLabel");
#endif
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tGatewayNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tGatewayNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tGateway"
				"&uGateway=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tGatewayNavList(void)



void tGatewaytAddressNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uAddress,cLabel,cIP,uPort,uPriority,uWeight FROM tAddress WHERE uGateway=%u",uGateway);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>IP Addresses</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>IP Addresses</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tAddress"
				"&uAddress=%s>%s/%s/%s/%s/%s</a><br>\n",
				field[0],field[1],field[2],field[3],field[4],field[5]);
	}

        mysql_free_result(res);

}//void tGatewayAddressNavList(void)


void tGatewaytGroupNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tGroup.uGroup,tGroup.cLabel"
			" FROM tGroup,tGroupGlue"
			" WHERE tGroupGlue.uKey=%u"
			" AND tGroupGlue.uGroupType=5"
			" AND tGroupGlue.uGroup=tGroup.uGroup"
						,uGateway);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>Group Membership</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>Group Membership</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tGroup"
				"&uGroup=%s>%s</a><br>\n",
				field[0],field[1]);
	}

        mysql_free_result(res);

}//void tGatewaytGroupNavList(void)

