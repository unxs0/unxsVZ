/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
 
*/

#include "apache.h"

//External funcs
void EncryptPasswdMD5(char *cPasswd);
//mainfunc.h
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServer,
		unsigned uSite,unsigned uSiteUser,unsigned uOwner,unsigned uCreatedBy);

//Local new funcs
int LoginServerCount(unsigned uSite);
void tSiteUserContextInfo(void);
int LocalModtSiteUser(void);

void tSiteUserNavList(void);

void ExtProcesstSiteUserVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstSiteUserVars(pentry entries[], int x)


void ExttSiteUserCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tSiteUserTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstSiteUserVars(entries,x);
			if( uStatus==4 && uAllowDel(uOwner,uCreatedBy))
			{
				guMode=0;
				if(!uSite)
					tSiteUser("<blink>Unexpected uSite=0 error</blink>");
				if(LoginServerCount(uSite)<2);
					tSiteUser("<blink>Can't delete last user here.</blink>");
	                        guMode=2001;
				tSiteUser(LANG_NB_CONFIRMDEL);
			}
			else
				tSiteUser("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstSiteUserVars(entries,x);
			if( uStatus==4 && uAllowDel(uOwner,uCreatedBy)) 
			{
	                        guMode=2001;
				if(!uSite)
					tSiteUser("<blink>Unexpected uSite=0 error</blink>");
				if(LoginServerCount(uSite)<2);
					tSiteUser("<blink>Can't delete last user here.</blink>");
				guMode=5;
				DeletetSiteUser();
			}
			else
				tSiteUser("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstSiteUserVars(entries,x);
			if( uStatus==4 && uAllowMod(uOwner,uCreatedBy)) 
			{
				guMode=2002;
				tSiteUser(LANG_NB_CONFIRMMOD);
			}
			else
				tSiteUser("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstSiteUserVars(entries,x);
			if(uStatus==4 && uAllowMod(uOwner,uCreatedBy)) 
			{
				char cExtStatusMessage[64]={"Unexpected execution path"};
				char cServer[99]={"Error"};
				char cDomain[99]={"Error"};
				unsigned uServer=0;
				char cPrevPasswd[64]={"Error"};

                        	guMode=2002;
				//Check entries here
				if(!cPasswd[0] || strlen(cPasswd)<6)
					tSiteUser("<blink>Empty or cPasswd less than 6 chars<blink>");
				if(strncmp(cPasswd,"$1$",3))
				{
					EncryptPasswdMD5(cPasswd);
				}
				else
				{
					if(strlen(cPasswd)!=34)
				  		tSiteUser("<blink>MD5 supplied cPasswd not right size<blink>");
				}


                        	guMode=0;
				sprintf(cPrevPasswd,
					"%s",ForeignKey("tSiteUser","cPasswd",uSiteUser));
				if(!strcmp(cPrevPasswd,cPasswd))
					tSiteUser("<blink>Nothing changed</blink>");

				uStatus=STATUS_PENDING_MOD;//Pending
				uModBy=guLoginClient;
				sprintf(cDomain,"%s",ForeignKey("tSite","cDomain",uSite));
				sscanf(ForeignKey("tSite","uServer",uSite),"%u",&uServer);
				sprintf(cServer,"%s",ForeignKey("tServer","cLabel",uServer));
				switch(LocalModtSiteUser())
				{
					case 0:
						SubmitJob("ModSiteUser","",cLogin,
							cServer,uSite,uSiteUser,uOwner,uCreatedBy);
						sprintf(cExtStatusMessage,
							"Site user modified job(s) created.");
					break;

					case 1:
					case 2:
					case 3:
						tSiteUser(gcQuery);
					break;

					case 4:
						htmlPlainTextError(mysql_error(&gMysql));
					break;

					default:
						htmlPlainTextError("Unexpected result from LocalModtSite()!");
					break;
				}
				tSiteUser(cExtStatusMessage);
			}
			else
				tSiteUser("<blink>Error</blink>: Denied by permissions settings"); 
                }
	}

}//void ExttSiteUserCommands(pentry entries[], int x)


void ExttSiteUserButtons(void)
{
	OpenFieldSet("tSiteUser Aux Panel",100);
	switch(guMode)
        {
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
			printf("Here we centralize all website user data and user backoffice actions.\n");
			if(uSiteUser)
			{
				printf("<p><u>Record Context Info</u><br>");
				tSiteUserContextInfo();

				printf("<p>");
				tSiteUserNavList();
			}
			else
			{
				printf("No site user selected.");
			}
	}
	CloseFieldSet();

}//void ExttSiteUserButtons(void)


void ExttSiteUserAuxTable(void)
{

}//void ExttSiteUserAuxTable(void)


void ExttSiteUserGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uSiteUser"))
		{
			sscanf(gentries[i].val,"%u",&uSiteUser);
			guMode=6;
		}
	}
	tSiteUser("");

}//void ExttSiteUserGetHook(entry gentries[], int x)


void ExttSiteUserSelect(void)
{
	ExtSelect("tSiteUser",VAR_LIST_tSiteUser,0);
	
}//void ExttSiteUserSelect(void)


void ExttSiteUserSelectRow(void)
{
	ExtSelectRow("tSiteUser",VAR_LIST_tSiteUser,uSiteUser);

}//void ExttSiteUserSelectRow(void)


void ExttSiteUserListSelect(void)
{
	char cCat[512];

	ExtListSelect("tSiteUser",VAR_LIST_tSiteUser);

	//Changes here must be reflected below in ExttSiteUserListFilter()
        if(!strcmp(gcFilter,"uSiteUser"))
        {
                sscanf(gcCommand,"%u",&uSiteUser);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tSiteUser.uSiteUser=%u \
						ORDER BY uSiteUser",
						uSiteUser);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uSiteUser");
        }

}//void ExttSiteUserListSelect(void)


void ExttSiteUserListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uSiteUser"))
                printf("<option>uSiteUser</option>");
        else
                printf("<option selected>uSiteUser</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttSiteUserListFilter(void)


void ExttSiteUserNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(uAllowMod(uOwner,uCreatedBy))
		printf(LANG_NBB_MODIFY);
	
	if(uAllowDel(uOwner,uCreatedBy))
	printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");
	
}//void ExttSiteUserNavBar(void)


void tSiteUserNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tSiteUserNavList</u><br>\n");

	ExtSelect("tSiteUser","uSiteUser,cLogin,(SELECT tSite.cDomain FROM tSite WHERE tSite.uSite=tSiteUser.uSite)",0);

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
			printf("<a class=darkLink href=unxsApache.cgi?gcFunction=tSiteUser"
			"&uSiteUser=%s>%s (%s)</a><br>\n",field[0],field[1],field[2]);
	}
        mysql_free_result(res);

}//void tSiteUserNavList(void)


int LoginServerCount(unsigned uSite)
{
        MYSQL_RES *mysqlRes;         
	unsigned uCount=0;

        sprintf(gcQuery,"SELECT uSiteUser FROM tSiteUser WHERE uSite=%u",uSite);

        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));

	mysqlRes=mysql_store_result(&gMysql);
	uCount=mysql_num_rows(mysqlRes);
	mysql_free_result(mysqlRes);
	return(uCount);

}//int LoginServerCount()


void tSiteUserContextInfo(void)
{
	printf("No context information available\n");
}//void tSiteUserContextInfo(void)


int LocalModtSiteUser(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uSiteUser,uModDate FROM tSiteUser WHERE uSiteUser=%u"
						,uSiteUser);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i<1)
	{
		sprintf(gcQuery,LANG_NBR_RECNOTEXIST);
		mysql_free_result(res);
		return(1);
	}
	if(i>1)
	{
		sprintf(gcQuery,LANG_NBR_MULTRECS);
		mysql_free_result(res);
		return(2);
	}

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate)
	{
		sprintf(gcQuery,LANG_NBR_EXTMOD);
		mysql_free_result(res);
		return(3);
	}
	Update_tSiteUser(field[0]);
	mysql_free_result(res);

	if(mysql_errno(&gMysql))
		return(4);
	uModDate=luGetModDate("tSiteUser",uSiteUser);
	unxsApacheLog(uSiteUser,"tSiteUser","Mod");

	return(0);

}//int LocalModtSiteUser(void)
