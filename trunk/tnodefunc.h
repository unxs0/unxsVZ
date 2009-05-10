/*
FILE
	$Id: tnodefunc.h 2076 2008-10-23 15:17:55Z Gary $
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2007 Gary Wallis.
 
*/

static unsigned uClone=0;

//ModuleFunctionProtos()
void CopyProperties(unsigned uOldNode,unsigned uNewNode,unsigned uType);
void DelProperties(unsigned uNode,unsigned uType);
void tNodeNavList(unsigned uDataCenter);
void htmlHealth(unsigned uContainer,unsigned uType);

void tContainerNavList(unsigned uNode);//tcontainerfunc.h
void htmlGroups(unsigned uNode, unsigned uContainer);

void ExtProcesstNodeVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uClone")) 
			uClone=1;
	}

}//void ExtProcesstNodeVars(pentry entries[], int x)


void ExttNodeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tNodeTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstNodeVars(entries,x);
                        	guMode=2000;
	                        tNode(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
                        	ProcesstNodeVars(entries,x);
				unsigned uOldNode=uNode;

                        	guMode=2000;
				//Check entries here
				if(strlen(cLabel)<3)
					tNode("<blink>Error</blink>: Must supply valid cLabel. Min 3 chars!");
				if(!uDatacenter)
					tNode("<blink>Error</blink>: Must supply valid uDatacenter!");
                        	guMode=0;

				uNode=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uStatus=1;//Initially active
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtNode(1);//Come back here uNode should be set
				if(!uNode)
					tNode("<blink>Error</blink>: New node was not created!");

				sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=2"
						",cName='Name',cValue='%s',uOwner=%u,uCreatedBy=%u"
						",uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,uNode,cLabel,guCompany,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));

				if(uNode && uClone && uOldNode)
				{
					CopyProperties(uOldNode,uNode,2);
					tNode("New node created and properties copied");
				}
				tNode("New node created");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstNodeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tNode(LANG_NB_CONFIRMDEL);
			}
			else
				tNode("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstNodeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DelProperties(uNode,2);
				DeletetNode();
			}
			else
				tNode("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstNodeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tNode(LANG_NB_CONFIRMMOD);
			}
			else
				tNode("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstNodeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				if(strlen(cLabel)<3)
					tNode("<blink>Error</blink>: Must supply valid cLabel. Min 3 chars!");
				if(!uDatacenter)
					tNode("<blink>Error</blink>: Must supply valid uDatacenter!");
                        	guMode=0;

				uModBy=guLoginClient;
				ModtNode();
			}
			else
				tNode("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttNodeCommands(pentry entries[], int x)


void ExttNodeButtons(void)
{
	OpenFieldSet("tNode Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter/mod data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
			if(uNode)
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
			printf("Hardware nodes are defined here. Hardware nodes host containers, and allow"
				" for the autonomic migration to other nodes that may be better suited"
				" at specific points in time to accomplish QoS or other system admin"
				" created policies herein.");
			printf("<p><u>Record Context Info</u><br>");
			if(uDatacenter && uNode)
			{
				printf("Node belongs to ");
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tDatacenter&uDatacenter=%u>%s<a>",
						uDatacenter,ForeignKey("tDatacenter","cLabel",uDatacenter));
				htmlGroups(uNode,0);
			}
			tContainerNavList(uNode);
			tNodeNavList(0);
			if(uNode)
				htmlHealth(uNode,2);
	}
	CloseFieldSet();

}//void ExttNodeButtons(void)


void ExttNodeAuxTable(void)
{
	if(!uNode) return;

        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"%s Property Panel",cLabel);
	OpenFieldSet(gcQuery,100);
	sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE uKey=%u AND uType=2 ORDER BY cName",uNode);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		printf("<table>");
		while((field=mysql_fetch_row(res)))
		{
			printf("<tr>");
			printf("<td width=100 valign=top><a class=darkLink href=unxsVZ.cgi?"
					"gcFunction=tProperty&uProperty=%s&cReturn=tNode_%u>"
					"%s</a></td><td>%s</td>\n",
						field[0],uNode,field[1],field[2]);
			printf("</tr>");
		}
		printf("</table>");
	}

	CloseFieldSet();

}//void ExttNodeAuxTable(void)


void ExttNodeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uNode"))
		{
			sscanf(gentries[i].val,"%u",&uNode);
			guMode=6;
		}
	}
	tNode("");

}//void ExttNodeGetHook(entry gentries[], int x)


void ExttNodeSelect(void)
{
	ExtSelect("tNode",VAR_LIST_tNode);

}//void ExttNodeSelect(void)


void ExttNodeSelectRow(void)
{
	ExtSelectRow("tNode",VAR_LIST_tNode,uNode);

}//void ExttNodeSelectRow(void)


void ExttNodeListSelect(void)
{
	char cCat[512];

	ExtListSelect("tNode",VAR_LIST_tNode);
	
	//Changes here must be reflected below in ExttNodeListFilter()
        if(!strcmp(gcFilter,"uNode"))
        {
                sscanf(gcCommand,"%u",&uNode);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tNode.uNode=%u \
						ORDER BY uNode",
						uNode);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uNode");
        }

}//void ExttNodeListSelect(void)


void ExttNodeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uNode"))
                printf("<option>uNode</option>");
        else
                printf("<option selected>uNode</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttNodeListFilter(void)


void ExttNodeNavBar(void)
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

}//void ExttNodeNavBar(void)


void tNodeNavList(unsigned uDatacenter)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uDatacenter)
		sprintf(gcQuery,"SELECT uNode,cLabel FROM tNode WHERE uDatacenter=%u AND (uOwner=%u OR"
				" uOwner IN (SELECT uClient FROM " TCLIENT
				" WHERE uOwner=%u)) ORDER BY cLabel",
					uDatacenter,guCompany,guCompany);
	else
		sprintf(gcQuery,"SELECT uNode,cLabel FROM tNode WHERE (uOwner=%u OR"
				" uOwner IN (SELECT uClient FROM " TCLIENT
				" WHERE uOwner=%u)) ORDER BY cLabel",
					guCompany,guCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tNodeNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tNodeNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tNode"
				"&uNode=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tNodeNavList()



void CopyProperties(unsigned uOldNode,unsigned uNewNode,unsigned uType)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uOldNode==0 || uNewNode==0 || uType==0) return;

	sprintf(gcQuery,"SELECT cName,cValue FROM tProperty WHERE uKey=%u AND uType=%u",
					uOldNode,uType);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,cName='%s',cValue='%s',uType=%u,uOwner=%u,"
				"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uNewNode,
					field[0],TextAreaSave(field[1]),
					uType,
					guLoginClient,guLoginClient);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

}//void CopyProperties(uNode,uNewNode)


void DelProperties(unsigned uNode,unsigned uType)
{
	if(uNode==0 || uType==0) return;
	sprintf(gcQuery,"DELETE FROM tProperty WHERE uType=%u AND uKey=%u",
				uType,uNode);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void DelProperties()
