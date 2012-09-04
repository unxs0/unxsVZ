/*
FILE
	$Id: tserverfunc.h 2065 2012-08-30 20:07:49Z Colin $
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR/LEGAL
	(C) 2001-2009 Unixservice, LLC. GPLv2 license applies.
 
*/

static unsigned uTargetServer=0;
static char cuTargetServerPullDown[256]={""};
static unsigned uWizIPv4=0;
static char cuWizIPv4PullDown[32]={""};
static char cSearch[32]={""};
static unsigned uTargetDatacenter=0;
static char cuTargetDatacenterPullDown[256]={""};
static unsigned uSyncPeriod=0;


//ModuleFunctionProtos()
void tServerNavList(unsigned uDataCenter);

//external
//tclientfunc.h
static unsigned uForClient=0;
static char cForClientPullDown[256]={"---"};

void ExtProcesstServerVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.31s",TextAreaSave(entries[i].val));
		}
		else if(!strcmp(entries[i].name,"cForClientPullDown"))
		{
			strcpy(cForClientPullDown,entries[i].val);
			uForClient=ReadPullDown(TCLIENT,"cLabel",cForClientPullDown);
		}
		else if(!strcmp(entries[i].name,"cuTargetServerPullDown"))
		{
			sprintf(cuTargetServerPullDown,"%.255s",entries[i].val);
			uTargetServer=ReadPullDown("tServer","cLabel",cuTargetServerPullDown);
		}
		else if(!strcmp(entries[i].name,"cuWizIPv4PullDown"))
		{
			sprintf(cuWizIPv4PullDown,"%.31s",entries[i].val);
			uWizIPv4=ReadPullDown("tIP","cLabel",cuWizIPv4PullDown);
		}
		else if(!strcmp(entries[i].name,"cuTargetDatacenterPullDown"))
		{
			sprintf(cuTargetDatacenterPullDown,"%.255s",entries[i].val);
			uTargetDatacenter=ReadPullDown("tDatacenter","cLabel",cuTargetDatacenterPullDown);
		}
		else if(!strcmp(entries[i].name,"uSyncPeriod"))
		{
			sscanf(entries[i].val,"%u",&uSyncPeriod);
		}
	}

}//void ExtProcesstServerVars(pentry entries[], int x)


void ExttServerCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tServerTools"))
	{
        	MYSQL_RES *res;
		time_t uActualModDate= -1;

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstServerVars(entries,x);
                        	guMode=2000;
	                        tServer(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
                        	ProcesstServerVars(entries,x);

                        	guMode=2000;
				//Check entries here
				if(strlen(cLabel)<3)
					tServer("<blink>Error</blink>: Must supply valid cLabel. Min 3 chars!");
				if(!uDatacenter)
					tServer("<blink>Error</blink>: Must supply valid uDatacenter!");
				sprintf(gcQuery,"SELECT uServer FROM tServer WHERE cLabel='%s'",
						cLabel);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tServer("<blink>Error</blink>: Node cLabel is used!");
				}
                        	guMode=0;

				if(!uForClient)
					uOwner=guCompany;
				else
					uOwner=uForClient;
				uServer=0;
				uCreatedBy=guLoginClient;
				uStatus=1;//Initially active
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtServer(1);//Come back here uServer should be set
				if(!uServer)
					tServer("<blink>Error</blink>: New server was not created!");

				tServer("New server created");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstServerVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=0;
				sprintf(gcQuery,"SELECT uServer FROM tContainer WHERE uServer=%u",
									uServer);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tServer("<blink>Error</blink>: Can't delete a server"
							" used by a container!");
				}
	                        guMode=2001;
				tServer(LANG_NB_CONFIRMDEL);
			}
			else
				tServer("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstServerVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				sscanf(ForeignKey("tServer","uModDate",uServer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tServer("<blink>Error</blink>: This record was modified. Reload it.");

				sprintf(gcQuery,"SELECT uServer FROM tContainer WHERE uServer=%u",
									uServer);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tServer("<blink>Error</blink>: Can't delete a server"
							" used by a container!");
				}
	                        guMode=0;
				DeletetServer();
			}
			else
				tServer("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstServerVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tServer(LANG_NB_CONFIRMMOD);
			}
			else
				tServer("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstServerVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				sscanf(ForeignKey("tServer","uModDate",uServer),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tServer("<blink>Error</blink>: This record was modified. Reload it.");
				if(strlen(cLabel)<3)
					tServer("<blink>Error</blink>: Must supply valid cLabel. Min 3 chars!");
				if(!uDatacenter)
					tServer("<blink>Error</blink>: Must supply valid uDatacenter!");
                        	guMode=0;

				if(uForClient)
				{
					sprintf(gcQuery,"UPDATE tServer SET uOwner=%u WHERE uServer=%u",
						uForClient,uServer);
					mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
                				htmlPlainTextError(mysql_error(&gMysql));
					uOwner=uForClient;
				}
				uModBy=guLoginClient;
				ModtServer();
			}
			else
				tServer("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttServerCommands(pentry entries[], int x)


void ExttServerButtons(void)
{
	OpenFieldSet("tServer Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter/mod data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
			if(guPermLevel>11)
				tTablePullDownResellers(uForClient,1);
                break;

                case 2001:
                        printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review changes</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
			if(guPermLevel>11)
			{
				printf("<p>To change the record owner, just...");
				tTablePullDownResellers(guCompany,1);
			}
                break;

		default:
			printf("<u>Table Tips</u><br>");
			printf("SIP server servers are defined here.");
			printf("<p><u>Record Context Info</u><br>");
			if(uDatacenter && uServer)
			{
				printf("Node belongs to ");
				printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tDatacenter&uDatacenter=%u>%s</a>",
						uDatacenter,ForeignKey("tDatacenter","cLabel",uDatacenter));
				//htmlGroups(uServer,0);
			}
			printf("<p><u>Container Search</u><br>");
			printf("<input title='Use [Search], enter cLabel start or MySQL LIKE pattern (%% or _ allowed)'"
					" type=text name=cSearch value='%s'>",cSearch);
			tServerNavList(0);
			if(uServer)
			{
			}
	}
	CloseFieldSet();

}//void ExttServerButtons(void)


void ExttServerAuxTable(void)
{
	switch(guMode)
	{
	}

}//void ExttServerAuxTable(void)


void ExttServerGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uServer"))
		{
			sscanf(gentries[i].val,"%u",&uServer);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.31s",gentries[i].val);
		}
	}
	tServer("");

}//void ExttServerGetHook(entry gentries[], int x)


void ExttServerSelect(void)
{
	ExtSelectPublic("tServer",VAR_LIST_tServer);

}//void ExttServerSelect(void)


void ExttServerSelectRow(void)
{
	ExtSelectRowPublic("tServer",VAR_LIST_tServer,uServer);

}//void ExttServerSelectRow(void)


void ExttServerListSelect(void)
{
	char cCat[512];

	ExtListSelectPublic("tServer",VAR_LIST_tServer);
	
	//Changes here must be reflected below in ExttServerListFilter()
        if(!strcmp(gcFilter,"uServer"))
        {
                sscanf(gcCommand,"%u",&uServer);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tServer.uServer=%u ORDER BY uServer",uServer);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uServer");
        }

}//void ExttServerListSelect(void)


void ExttServerListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uServer"))
                printf("<option>uServer</option>");
        else
                printf("<option selected>uServer</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttServerListFilter(void)


void ExttServerNavBar(void)
{
	printf(LANG_NBB_SEARCH);
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);

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

}//void ExttServerNavBar(void)


//Nodes are shared infrastructure
void tServerNavList(unsigned uDatacenter)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uMysqlNumRows;
	unsigned uNumRows=0;
#define LIMIT " LIMIT 25"
#define uLIMIT 24

	if(uDatacenter)
		sprintf(gcQuery,"SELECT uServer,cLabel FROM tServer WHERE uDatacenter=%u"
				" ORDER BY cLabel" 
				LIMIT,uDatacenter);
	else
		sprintf(gcQuery,"SELECT uServer,cLabel FROM tServer ORDER BY cLabel" 
				LIMIT);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tServerNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if((uMysqlNumRows=mysql_num_rows(res)))
	{	
        	printf("<p><u>tServerNavList(%u)</u><br>\n",uMysqlNumRows);

	        while((field=mysql_fetch_row(res)))
		{

			
			if(cSearch[0])
				printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tServer"
					"&uServer=%s&cSearch=%s>%s</a><br>\n",
							field[0],cURLEncode(cSearch),field[1]);
			else
				printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tServer"
					"&uServer=%s>%s</a><br>\n",
							field[0],field[1]);

			if(++uNumRows>=uLIMIT)
			{
				printf("Only %u of %u servers shown use [List] to view all.<br>\n",
						uLIMIT,uMysqlNumRows);
				break;
			}
		}
	}
        mysql_free_result(res);

}//void tServerNavList()

