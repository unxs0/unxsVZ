/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
 
*/

#include <ctype.h>
static char cSearch[64]={""};

//Local
void tServerNavList(void);
void BasicServerCheck(void);
void tServerContextInfo(void);
int ServerUsed(unsigned uServer);


void ExtProcesstServerVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",entries[i].val);
	}
}//void ExtProcesstServerVars(pentry entries[], int x)


void ExttServerCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tServerTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstServerVars(entries,x);
                        	guMode=2000;
	                        tServer(LANG_NB_CONFIRMNEW);
			}
			else
				tServer("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstServerVars(entries,x);

                        	guMode=2000;
				//Check entries here
				BasicServerCheck();
                        	guMode=0;

				uServer=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtServer(0);
			}
			else
				tServer("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstServerVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
                        	guMode=0;
				if(ServerUsed(uServer))
					tServer("<blink>This server is in use.</blink");
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
                        	guMode=0;
				if(ServerUsed(uServer))
					tServer("<blink>This server is in use.</blink");

				guMode=5;
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
				//Check entries here
				BasicServerCheck();
                        	guMode=0;

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
			printf("This is where keep track of all servers used in our mail cluster. "
				"Not all have to be active or belong to any tServerGroup's via tServerGroupGlue\n");

			if(uServer)
			{
				printf("<p><u>Record Context Info</u><br>");
				tServerContextInfo();
				printf("<p><input title='Enter a mySQL search pattern for cLabel to "
					"limit tServerNavList' type=text name=cSearch value='%s' maxlength=63 size=20> cSearch\n",
						cSearch);
				tServerNavList();
			}
	}
	CloseFieldSet();

}//void ExttServerButtons(void)


void ExttServerAuxTable(void)
{

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
			sprintf(cSearch,"%.63s",gentries[i].val);
		}
	}
	tServer("");

}//void ExttServerGetHook(entry gentries[], int x)


void ExttServerSelect(void)
{
	if(cSearch[0])
		ExtSelectSearch("tServer",VAR_LIST_tServer,"tServer.cLabel",cSearch,NULL,0);
	else
		ExtSelect("tServer",VAR_LIST_tServer,0);
	
}//void ExttServerSelect(void)


void ExttServerSelectRow(void)
{
	ExtSelectRow("tServer",VAR_LIST_tServer,uServer);
	

}//void ExttServerSelectRow(void)


void ExttServerListSelect(void)
{
	char cCat[512];

	ExtListSelect("tServer",VAR_LIST_tServer);

	//Changes here must be reflected below in ExttServerListFilter()
        if(!strcmp(gcFilter,"uServer"))
        {
                sscanf(gcCommand,"%u",&uServer);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tServer.uServer=%u \
						ORDER BY uServer",
						uServer);
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
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=10 && !guListMode)
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


void tServerNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;

	printf("<p><u>tServerNavList</u><br>\n");
	
	//NavList variation where we expect less than a 100 to be common
	//but still want cSearch functionality
	if(!cSearch[0])
		ExtSelect("tServer","tServer.uServer,tServer.cLabel",101);
	else
		ExtSelectSearch("tServer","tServer.uServer,tServer.cLabel","tServer.cLabel",cSearch,NULL,101);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
	        while((field=mysql_fetch_row(res)))
		{
			printf("<a class=darkLink href=unxsApache.cgi?gcFunction=tServer"
				"&uServer=%s&cSearch=%s>%s</a><br>\n",field[0],cURLEncode(cSearch),field[1]);
			if(++uCount>=100)
			{
				printf("More than 100 records: You must refine your search further<br>\n");
				break;
			}
	        }
	}
        mysql_free_result(res);

}//void tServerNavList(void)


void BasicServerCheck(void)
{
	if(!cLabel[0])
		tServer("<blink>cLabel is required</blink>");
	if(!isalpha(cLabel[0]))
		tServer("<blink>cLabel must not start with a number</blink>");
	if(strlen(cLabel)<3)
		tServer("<blink>cLabel must be at least 3 chars</blink>");
	if(strstr(cLabel,".."))
		tServer("<blink>cLabel may not have two consecutive periods</blink>");

}//void BasicServerCheck(void)


void tServerContextInfo(void)
{
}//void tServerContextInfo(void)


int ServerUsed(unsigned uServer)
{
        MYSQL_RES *res;
	unsigned uRetVal=0;

	sprintf(gcQuery,"SELECT uServer FROM tServerGroupGlue WHERE uServer=%u",uServer);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
		uRetVal=1;
	mysql_free_result(res);

	return(uRetVal);

}//int ServerUsed(unsigned uServer)
