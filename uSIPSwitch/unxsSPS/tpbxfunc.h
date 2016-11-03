/*
FILE
	svn ID removed
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


static unsigned uInGroup=0;
static char cuInGroupPullDown[256]={""};

void tPBXtAddressNavList(unsigned uPBX);
void tPBXtGroupNavList(void);
void tPBXtDIDNavList(unsigned uPBX);

void ExtProcesstPBXVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uInGroup"))
			sscanf(entries[i].val,"%u",&uInGroup);
		else if(!strcmp(entries[i].name,"cuInGroupPullDown"))
		{
			sprintf(cuInGroupPullDown,"%.255s",entries[i].val);
			uInGroup=ReadPullDown("tGroup","cLabel",cuInGroupPullDown);
		}
	}
}//void ExtProcesstPBXVars(pentry entries[], int x)


void ExttPBXCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tPBXTools"))
	{
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstPBXVars(entries,x);
                        	guMode=2000;
	                        tPBX(LANG_NB_CONFIRMNEW);
			}
			else
				tPBX("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstPBXVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uPBX=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtPBX(0);
			}
			else
				tPBX("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstPBXVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tPBX(LANG_NB_CONFIRMDEL);
			}
			else
				tPBX("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstPBXVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetPBX();
			}
			else
				tPBX("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstPBXVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tPBX(LANG_NB_CONFIRMMOD);
			}
			else
				tPBX("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstPBXVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtPBX();
			}
			else
				tPBX("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,"Add/Remove Group"))
		{
                        ProcesstPBXVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
        			MYSQL_RES *res;

				if(!uInGroup)
					tPBX("<blink>Error</blink>: Must select a group");
				sprintf(gcQuery,"SELECT uKey FROM tGroupGlue WHERE uKey=%u AND uGroup=%u AND uGroupType=5",uPBX,uInGroup);
				mysql_query(&gMysql,gcQuery);
        			if(mysql_errno(&gMysql))
					tPBX("<blink>Error</blink>: Query error");
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					//remove
					sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uKey=%u AND uGroup=%u AND uGroupType=5",uPBX,uInGroup);
					mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
						tPBX("<blink>Error</blink>: Query error");
					tPBX("PBX deleted from group");
				}
				else
				{
					//add
					sprintf(gcQuery,"INSERT INTO tGroupGlue SET uKey=%u,uGroup=%u,uGroupType=5",uPBX,uInGroup);
					mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
						tPBX("<blink>Error</blink>: Query error");
					tPBX("PBX added to group");
				}
			}
			else
				tPBX("<blink>Error</blink>: Denied by permissions settings");
		}
	}

}//void ExttPBXCommands(pentry entries[], int x)


void ExttPBXButtons(void)
{
	OpenFieldSet("tPBX Aux Panel",100);
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
			printf("uSIPSwitch is usually used for switching SIP traffic between Carriers and hosted business PBXs."
				" PBXs are SIP gateways, usually B2BUA Asterisk servers or similar, that are"
				" associated with DIDs.<br>"
				" PBXs have IP addresses that are manually set or added by our DNS subsystem. A record IPs are added"
				" along with standard udp SRV record IPs if the hostname has them. This means that if a PBX has a backup"
				" or failover PBX, the IP will also show up here.<br>"
				" If our end point health system is running the IPs will be checked for SIP responses to OPTIONS requests"
				" and will be marked in tAddress as available. An uptime counter is incremented giving an adhoc reliability"
				" number."
					);

			printf("<p><u>Operations</u><br>");
			printf("No tJob data feature is available at this time.");

			printf("<p>");
			tTablePullDown("tGroup;cuInGroupPullDown","cLabel","cLabel",uInGroup,1);
			printf("<br><input type=submit class=largeButton title='Add/remove this PBX to/from the selected group above'"
					" name=gcCommand value='Add/Remove Group'>");
			if(uPBX)
			{
				tPBXtAddressNavList(uPBX);
				tPBXtGroupNavList();
				tPBXtDIDNavList(uPBX);
			}
	}
	CloseFieldSet();

}//void ExttPBXButtons(void)


void ExttPBXAuxTable(void)
{

}//void ExttPBXAuxTable(void)


void ExttPBXGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uPBX"))
		{
			sscanf(gentries[i].val,"%u",&uPBX);
			guMode=6;
		}
	}
	tPBX("");

}//void ExttPBXGetHook(entry gentries[], int x)


void ExttPBXSelect(void)
{

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tPBX ORDER BY"
				" uPBX",
				VAR_LIST_tPBX);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tPBX,tClient WHERE tPBX.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uPBX",
					VAR_LIST_tPBX,guCompany,guCompany);

}//void ExttPBXSelect(void)


void ExttPBXSelectRow(void)
{
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tPBX WHERE uPBX=%u",
			VAR_LIST_tPBX,uPBX);
	else
                sprintf(gcQuery,"SELECT %s FROM tPBX,tClient"
                                " WHERE tPBX.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tPBX.uPBX=%u",
                        		VAR_LIST_tPBX
					,guCompany,guCompany
					,uPBX);

}//void ExttPBXSelectRow(void)


void ExttPBXListSelect(void)
{
	char cCat[512];

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tPBX",
				VAR_LIST_tPBX);
	else
		sprintf(gcQuery,"SELECT %s FROM tPBX,tClient"
				" WHERE tPBX.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tPBX,guCompany,guCompany);

	//Changes here must be reflected below in ExttPBXListFilter()
        if(!strcmp(gcFilter,"uPBX"))
        {
                sscanf(gcCommand,"%u",&uPBX);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tPBX.uPBX=%u ORDER BY uPBX",uPBX);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cHostname"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tPBX.cHostname LIKE '%s%%' ORDER BY cHostname",gcCommand);
		strcat(gcQuery,cCat);
	}
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uPBX");
        }

}//void ExttPBXListSelect(void)


void ExttPBXListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uPBX"))
                printf("<option>uPBX</option>");
        else
                printf("<option selected>uPBX</option>");
        if(strcmp(gcFilter,"cHostname"))
                printf("<option>cHostname</option>");
        else
                printf("<option selected>cHostname</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttPBXListFilter(void)


void ExttPBXNavBar(void)
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

}//void ExttPBXNavBar(void)


void tPBXtAddressNavList(unsigned uPBX)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uAddress,cLabel,cIP,uPort,uPriority,uWeight FROM tAddress WHERE uPBX=%u",uPBX);
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

}//void tPBXtAddressNavList()


void tPBXtGroupNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tGroup.uGroup,cLabel FROM tGroupGlue,tGroup"
			" WHERE tGroupGlue.uGroup=tGroup.uGroup"
			" AND tGroupGlue.uKey=%u"
			" AND tGroupGlue.uGroupType=5",uPBX);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tPBXtGroupNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tPBXtGroupNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tGroup"
				"&uGroup=%s>%s</a><br>\n",
				field[0],field[1]);
	}

        mysql_free_result(res);

}//void tPBXtGroupNavList(void)


void tPBXtDIDNavList(unsigned uPBX)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uDID,cLabel FROM tDID"
			" WHERE uPBX=%u",uPBX);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tPBXtDIDNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tPBXtDIDNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tDID"
				"&uDID=%s>%s</a><br>\n",
				field[0],field[1]);
	}

        mysql_free_result(res);

}//void tPBXtDIDNavList(unsigned uPBX)

