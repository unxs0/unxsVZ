/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2007 Gary Wallis.
 
*/

//ModuleFunctionProtos()


void tGroupNavList(void);
void tGroupMemberNavList(void);
void voidCopyGroupType(unsigned uGroupType,unsigned uGroup);	
void voidCopyGroupProps(unsigned uFromGroup,unsigned uGroup);	
void voidDelGroupProperties(unsigned uGroup);

unsigned uCopyProperties=0;

void ExtProcesstGroupVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strncmp(entries[i].name,"uCopyProperties",15))
			uCopyProperties=1;
	}
}//void ExtProcesstGroupVars(pentry entries[], int x)


void ExttGroupCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tGroupTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstGroupVars(entries,x);
                        	guMode=2000;
	                        tGroup(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,"Add Monitor Group"))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstGroupVars(entries,x);
				
                        	guMode=0;
				if(!uGroup)
					tGroup("<blink>Error</blink>: uGroup must be selected");
				if(!uGroupType==1)
					tGroup("<blink>Error</blink>: uGroupType must be container");
				char cMonitorAddGroupScript[256]={""};
				char cSystemCall[512]={""};
				GetConfiguration("cMonitorAddGroupScript",cMonitorAddGroupScript,0,0,0,0);
				if(!cMonitorAddGroupScript[0])
					tGroup("<blink>Error</blink>: tConfiguration cMonitorAddGroupScript does not exist");
				sprintf(cSystemCall,"%.255s \"%.31s\"",cMonitorAddGroupScript,cLabel);
				char cMsg[256];
				unsigned uRetVal=0;
				FILE *pp;
				if((pp=popen(cSystemCall,"r"))==NULL)
				{
					sprintf(cMsg,"<blink>Error 1</blink>: %s",cSystemCall);
					tGroup(cMsg);
				}
				char cResponse[256]={""};
				if(fscanf(pp,"%255s",cResponse)>0)
					sscanf(cResponse,"%u",&uRetVal);
				if(!uRetVal)	
				{
					sprintf(cMsg,"<blink>Error 2</blink>: %s",cSystemCall);
					tGroup(cMsg);
				}
				pclose(pp);
        			MYSQL_RES *res;
        			MYSQL_ROW field;
				sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='uMonitorGroupID' AND uType="
						PROP_GROUP
						" AND uKey=%u",uGroup);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					sprintf(gcQuery,"UPDATE tProperty SET"
						" cValue='%u',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
						" WHERE uProperty=%s",
							uRetVal,guLoginClient,
							field[0]);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						sprintf(cMsg,"%.240s:%u",mysql_error(&gMysql),uRetVal);
						tGroup(cMsg);
					}
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET"
						" cName='uMonitorGroupID',cValue='%u',uType="
						PROP_GROUP
						",uKey=%u,uOwner=%u,"
						"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							uRetVal,
							uGroup,guCompany,
							guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				}
        			mysql_free_result(res);
				sprintf(cMsg,"%s %s:%u",cMonitorAddGroupScript,cLabel,uRetVal);
				tGroup(cMsg);
			}
		}
		else if(!strcmp(gcCommand,"Create Property"))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstGroupVars(entries,x);
				
				if(!uGroup)
					tGroup("<blink>Error</blink>: uGroup must be selected");
				if(!uGroupType)
					tGroup("<blink>Error</blink>: uGroupType must be selected");
                        	guMode=0;
				sprintf(gcQuery,"INSERT INTO tProperty SET"
						" cName='Dummy',cValue='Dummy',uType="
						PROP_GROUP
						",uKey=%u,uOwner=%u,"
						"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							uGroup,guCompany,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				tGroup("New dummy property created");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstGroupVars(entries,x);
				unsigned uLoadedGroup=uGroup;	

                        	guMode=2000;
				//Check entries here
				if(strlen(cLabel)<4)
					tGroup("<blink>Error</blink>: cLabel too short");
				if(!strcmp(cLabel,ForeignKey("tGroup","cLabel",uGroup)))
					tGroup("<blink>Error</blink>: cLabel same as source group");
				if(!uGroupType)
					tGroup("<blink>Error</blink>: uGroupType must be selected");
                        	guMode=0;


				uGroup=0;
				uCreatedBy=guLoginClient;
				GetClientOwner(guLoginClient,&guReseller);
				uOwner=guReseller;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
		
				NewtGroup(1);
				//char cData[256];
				//sprintf(cData,"uGroup=%u uLoadedGroup=%u",uGroup,uLoadedGroup);
				//tGroup(cData);
				if(uCopyProperties && uLoadedGroup && uGroup)
				{
					voidCopyGroupProps(uLoadedGroup,uGroup);
					if(mysql_affected_rows(&gMysql)>0)
						tGroup("New group created, properties copied from previous group");
					else
						tGroup("New group created, no properties copied");
				}
				else if(!uCopyProperties && uGroupType && uGroup)
				{
					voidCopyGroupType(uGroupType,uGroup);
					if(mysql_affected_rows(&gMysql)>0)
						tGroup("New group created, properties copied from group type");
					else
						tGroup("New group created, no properties copied from group type");
				}
				tGroup("New group created");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstGroupVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
	                        guMode=2001;
				tGroup(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstGroupVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=5;
				voidDelGroupProperties(uGroup);	
				DeletetGroup();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstGroupVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=2002;
				tGroup(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstGroupVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
                        	guMode=2002;
				if(!uGroupType)
					tGroup("<blink>Error</blink>: uGroupType must be selected");
				if(strlen(cLabel)<4)
					tGroup("<blink>Error</blink>: cLabel too short");
				//debug only
				//char cData[256];
				//sprintf(cData,"uGroup=%u uGroupType=%u",uGroup,uGroupType);
				//tGroup(cData);
                        	guMode=0;

				uModBy=guLoginClient;
				ModtGroup();
			}
                }
	}

}//void ExttGroupCommands(pentry entries[], int x)


void ExttGroupButtons(void)
{
	OpenFieldSet("tGroup Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter/mod data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
			if(uGroup)
			printf("<p><input title='Copy the properties from the previously loaded group (uGroup=%u), if any,"
				" and as shown in property panel below'"
				" type=checkbox name=uCopyProperties checked> Copy properties from (uGroup=%u) below<br>",uGroup,uGroup);
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
			printf("tGroup (and it's related tables <a href=?gcFunction=tGroupGlue>"
				"tGroupGlue</a> and <a href=?gcFunction=tGroupType>tGroupType</a>)"
				" provide a way to create"
				" container and node groups of different types."
				" These groups can then be used for organizational and/or autonomic purposes.");
			if(uGroup) tGroupMemberNavList();
			tGroupNavList();
			if(guPermLevel>=10 && uGroupType==1)
			{
				char cMonitorAddGroupScript[256]={""};
				GetConfiguration("cMonitorAddGroupScript",cMonitorAddGroupScript,0,0,0,0);
				if(cMonitorAddGroupScript[0])
				{
					printf("<p><input title='Add this container group to external configured monitoring system'"
					" type=submit class=largeButton"
					" name=gcCommand value='Add Monitor Group'>\n");
				}
			}
	}
	CloseFieldSet();

}//void ExttGroupButtons(void)


void ExttGroupAuxTable(void)
{
	if(!uGroup) return;

        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"%s Property Panel",cLabel);
	OpenFieldSet(gcQuery,100);
	sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE uKey=%u AND uType="
			PROP_GROUP
			" ORDER BY cName",
				uGroup);

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
					"gcFunction=tProperty&uProperty=%s&cReturn=tGroup_%u>"
					"%s</a></td><td>%s</td>\n",
						field[0],uGroup,field[1],field[2]);
			printf("</tr>\n");
		}
		printf("</table>");
	}
	else
	{
		if(guPermLevel>=10)
		{
			printf("<input type=hidden name=uGroupType value=%u>",uGroupType);
			printf("<p><input title='Create a new dummy property for this group'"
					" type=submit class=largeButton"
					" name=gcCommand value='Create Property'>\n");
		}
	}

	CloseFieldSet();


}//void ExttGroupAuxTable(void)


void ExttGroupGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uGroup"))
		{
			sscanf(gentries[i].val,"%u",&uGroup);
			guMode=6;
		}
	}
	tGroup("");

}//void ExttGroupGetHook(entry gentries[], int x)


void ExttGroupSelect(void)
{
	ExtSelect("tGroup",VAR_LIST_tGroup);

}//void ExttGroupSelect(void)


void ExttGroupSelectRow(void)
{
	ExtSelectRow("tGroup",VAR_LIST_tGroup,uGroup);

}//void ExttGroupSelectRow(void)


void ExttGroupListSelect(void)
{
	char cCat[512];

	ExtListSelect("tGroup",VAR_LIST_tGroup);

	//Changes here must be reflected below in ExttGroupListFilter()
        if(!strcmp(gcFilter,"uGroup"))
        {
                sscanf(gcCommand,"%u",&uGroup);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tGroup.uGroup=%u \
						ORDER BY uGroup",
						uGroup);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uGroup");
        }

}//void ExttGroupListSelect(void)


void ExttGroupListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uGroup"))
                printf("<option>uGroup</option>");
        else
                printf("<option selected>uGroup</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttGroupListFilter(void)


void ExttGroupNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=10 && !guListMode)
		printf(LANG_NBB_NEW);

			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_MODIFY);

			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttGroupNavBar(void)


void tGroupNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        ExtSelect("tGroup","tGroup.uGroup,tGroup.cLabel");

	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tGroupNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tGroupNavList</u><br>\n");
	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tGroup"
				"&uGroup=%s>%s</a> &nbsp;\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tGroupNavList(void)


void tGroupMemberNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uContainer,uNode FROM tGroupGlue WHERE uGroup=%u LIMIT 33",uGroup);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tGroupMemberNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		unsigned uContainer,uNode,uLimit=0;
        	MYSQL_RES *res2;
        	MYSQL_ROW field2;

        	printf("<p><u>tGroupMemberNavList</u><br>\n");
	        while((field=mysql_fetch_row(res)))
		{	
			if(++uLimit>32)
			{
				printf("(Only first 32 shown)<br>");
				break;
			}

			sscanf(field[0],"%u",&uContainer);
			sscanf(field[1],"%u",&uNode);
			if(uContainer)
			{
				sprintf(gcQuery,"SELECT cLabel FROM tContainer WHERE uContainer=%u",uContainer);
        			mysql_query(&gMysql,gcQuery);
			        if(mysql_errno(&gMysql))
			        {
			        	printf("<p><u>tGroupMemberNavList</u><br>\n");
			                printf("%s",mysql_error(&gMysql));
			                return;
			        }
			        res2=mysql_store_result(&gMysql);
	        		if((field2=mysql_fetch_row(res2)))
					printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer"
					"&uContainer=%u>%s</a> &nbsp;\n",uContainer,field2[0]);
				mysql_free_result(res2);
			}
			else if(uNode)
			{
				sprintf(gcQuery,"SELECT cLabel FROM tNode WHERE uNode=%u",uNode);
        			mysql_query(&gMysql,gcQuery);
			        if(mysql_errno(&gMysql))
			        {
			        	printf("<p><u>tGroupMemberNavList</u><br>\n");
			                printf("%s",mysql_error(&gMysql));
			                return;
			        }
			        res2=mysql_store_result(&gMysql);
	        		if((field2=mysql_fetch_row(res2)))
					printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tNode"
					"&uNode=%u>%s</a> &nbsp;\n",uNode,field2[0]);
				mysql_free_result(res2);
			}
		}
	}
        mysql_free_result(res);

}//void tGroupMemberNavList(void)


void voidCopyGroupType(unsigned uGroupType,unsigned uGroup)
{
	//MySQL 5.0+ SQL
	sprintf(gcQuery,"INSERT INTO tProperty (cName,cValue,uType,uKey,uOwner,uCreatedBy,uCreatedDate)"
			" SELECT cName,cValue,"PROP_GROUP",%u,%u,%u,UNIX_TIMESTAMP(NOW()) FROM"
			" tProperty WHERE uKey=%u AND uType="
			PROP_GROUPTYPE
					,uGroup,guLoginClient,guCompany,uGroupType);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
		
}//void voidCopyGroupType()


void voidDelGroupProperties(unsigned uGroup)
{
	sprintf(gcQuery,"DELETE FROM tProperty WHERE uType="PROP_GROUP" AND uKey=%u",uGroup);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void voidDelGroupProperties(unsigned uGroup)


void voidCopyGroupProps(unsigned uFromGroup,unsigned uGroup)
{
	sprintf(gcQuery,"INSERT INTO tProperty (cName,cValue,uType,uKey,uOwner,uCreatedBy,uCreatedDate)"
			" SELECT cName,cValue,uType,%u,%u,%u,UNIX_TIMESTAMP(NOW()) FROM"
			" tProperty WHERE uKey=%u AND uType="
			PROP_GROUP
					,uGroup,guLoginClient,guCompany,uFromGroup);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
		
}//void voidCopyGroupProps(unsigned uFromGroup,unsigned uGroup)
