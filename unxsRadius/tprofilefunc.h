/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template code and mysqlRAD2/RAD3 software: (C) 2001-2009 Gary Wallis.

LEGAL
	File (C) 2001-2009 Gary Wallis and Hugo Urquiza
	LICENCE included in distribution
	GPL License applies, see www.fsf.org for details
*/

void tProfileNavList(unsigned uUser, unsigned uServer);
void CopyToAllNames(void);

void BasictProfileCheck(void);
void tTablePullDownCompanies(unsigned uSelector);
unsigned uProfileNameUsed(const unsigned uProfileName,const unsigned uOwner);

//Aux drop/pull downs
static char cForCompanyPullDown[256]={"---"};
static unsigned uForCompany=0;


void ExtProcesstProfileVars(pentry entries[], int x)
{

	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cForCompanyPullDown"))
		{
			strcpy(cForCompanyPullDown,entries[i].val);
			uForCompany=ReadPullDown(TCLIENT,"cLabel",cForCompanyPullDown);
		}
	}

}//void ExtProcesstProfileVars(pentry entries[], int x)


void ExttProfileCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tProfileTools"))
	{
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
				ProcesstProfileVars(entries,x);
				guMode=2000;
				tProfile(LANG_NB_CONFIRMNEW);
			}
			else
				tProfile("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
				ProcesstProfileVars(entries,x);
				guMode=2000;
				BasictProfileCheck();
				guMode=0;
				uProfile=0;
				uCreatedBy=guLoginClient;
				uModDate=0;
				if(uForCompany)
					uOwner=uForCompany;
				else
					uOwner=guCompany;
				uModBy=0;//Never modified
				NewtProfile(0);
			}
			else
				tProfile("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstProfileVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))  
			{
				guMode=2001;
				tProfile(LANG_NB_CONFIRMDEL);
			}
			else
				tProfile("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstProfileVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))  
			{
				guMode=2001;
				if(uProfileNameUsed(uProfileName,uOwner))
					tProfile("<blink>Error</blink>: Can't delete, profile is being used.");
				guMode=5;
				DeletetProfile();
			}
			else
				tProfile("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstProfileVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
				guMode=2002;
				tProfile(LANG_NB_CONFIRMMOD);
			}
			else
				tProfile("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstProfileVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
				BasictProfileCheck();
				uModBy=guLoginClient;
				if(uForCompany && uForCompany!=guCompany)
				{
					if(uProfileNameUsed(uProfileName,uOwner))
						tProfile("<blink>Error</blink>: Can't change owner, profile name"
								" is being used.");
					uOwner=uForCompany;
				}
				else
					uOwner=guCompany;
				ModtProfile();
			}
			else
				tProfile("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,"CopyToAllNames"))
                {
                        ProcesstProfileVars(entries,x);
			if(guPermLevel>=10)
			{
				CopyToAllNames();
				if(mysql_affected_rows(&gMysql)>0)
					tProfile("CopyToAllNames() done");
				else
					tProfile("<blink>Error</blink>: CopyToAllNames() did not insert any records!");
			}
			else
				tProfile("<blink>Error</blink>: Denied by permissions settings");
		}
	}

}//void ExttProfileCommands(pentry entries[], int x)


void ExttProfileButtons(void)
{
	OpenFieldSet("tProfile Aux Panel",100);
	
	switch(guMode)
        {
                case 2000:
			printf("Enter required data<br>");
			if(guPermLevel>7)
					tTablePullDownCompanies(guCompany);
                        printf(LANG_NBB_CONFIRMNEW);
			
			if(guPermLevel>9)
				printf("<p><input type=submit class=lwarnButton title='Makes new profiles based "
				"on this one for all tProfileName entries of same uOwner'"
				" name=gcCommand value='CopyToAllNames'><br>\n");
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
			printf("<br>\n");
                break;

                case 2002:
			printf("Review record data<br>");
			if(guPermLevel>7)
					tTablePullDownCompanies(guCompany);
                        printf(LANG_NBB_CONFIRMMOD);
			printf("<br>\n");
                break;

		default:
			printf("<u>Table Tips</u><br>");
			printf("The tProfile table is populated by all the RADIUS user profile templates available to the different companies via uOwner. There should be a CHAP and a PAP profile for any given tProfileName. tProfileName entries should be created first. You should be familiar with your RADIUS server user file format, RADIUS attributes, tokens and dictionaries before modifying or creating your own templates. The first %%s is replaced by the tUser.cLogin, the second %%s is replaced by the password (clear or encrypted depending on tProfile.uClearText value. Optionally a third %%s is replaced by tUser.cIP if uStaticIP is yes (1). Of course cIP does not have to be an IP it could be a callerID value etc."
			"<p>Leave uUser blank for allowing profile to be used by any tUser.uSer, similarly uServer.");
			tProfileNavList(0,0);
		break;

	}


	CloseFieldSet();

}//void ExttProfileButtons(void)


void ExttProfileAuxTable(void)
{

}//void ExttProfileAuxTable(void)


void ExttProfileGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uProfile"))
		{
			sscanf(gentries[i].val,"%u",&uProfile);
			guMode=6;
		}
	}
	tProfile("");

}//void ExttProfileGetHook(entry gentries[], int x)


void ExttProfileSelect(void)
{
	ExtSelect("tProfile",VAR_LIST_tProfile,0);

}//void ExttProfileSelect(void)


void ExttProfileSelectRow(void)
{
	ExtSelectRow("tProfile",VAR_LIST_tProfile,uProfile);

}//void ExttProfileSelectRow(void)


void ExttProfileListSelect(void)
{
	char cCat[512];

	ExtListSelect("tProfile",VAR_LIST_tProfile);

	//Changes here must be reflected below in ExttProfileListFilter()
        if(!strcmp(gcFilter,"uProfile"))
        {
                sscanf(gcCommand,"%u",&uProfile);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tProfile.uProfile=%u \
						ORDER BY uProfile",
						uProfile);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uProfile");
        }

}//void ExttProfileListSelect(void)


void ExttProfileListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uProfile"))
                printf("<option>uProfile</option>");
        else
                printf("<option selected>uProfile</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttProfileListFilter(void)


void ExttProfileNavBar(void)
{
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

}//void ExttProfileNavBar(void)


void ExtSelectSearch(const char *cTable,const char *cVarList,const char *cSearchField,
		const char *cSearch,const char *cExtraCond,unsigned uMaxResults);

void tProfileNavList(unsigned uUser, unsigned uServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uExtUser,uExtServer;
	char cClrTxt[16];
	char cStaticIP[16];
	char cExtra[65]={""};
	
	if(uUser)
	{
		sprintf(cExtra,"tProfile.uProfileName=tProfileName.uProfileName AND tProfile.uUser=%u",uUser);
		ExtSelectSearch("tProfile,tProfileName","tProfile.uProfile,tProfileName.cLabel,tProfile.uUser,tProfile.uServer,tProfile.uClearText,tProfile.uStaticIP","1","1",cExtra,0);
	}
	else if(uServer)
	{
		sprintf(cExtra,"tProfile.uProfileName=tProfileName.uProfileName AND tProfile.uServer=%u",uServer);
		ExtSelectSearch("tProfile,tProfileName","tProfile.uProfile,tProfileName.cLabel,tProfile.uUser,tProfile.uServer,tProfile.uClearText,tProfile.uStaticIP","1","1",cExtra,0);
	}
	else if(1)
	{
		sprintf(cExtra,"tProfile.uProfileName=tProfileName.uProfileName");
		ExtSelectSearch("tProfile,tProfileName","tProfile.uProfile,tProfileName.cLabel,tProfile.uUser,tProfile.uServer,tProfile.uClearText,tProfile.uStaticIP","1","1",cExtra,0);
	}
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tProfileNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
        	printf("<p><u>tProfileNavList</u><br>\n");
	}
        while((field=mysql_fetch_row(res)))
        {
		sscanf(field[2],"%u",&uExtUser);
		sscanf(field[3],"%u",&uExtServer);
		if(field[4][0]=='0')
			strcpy(cClrTxt,"Pap");
		else
			strcpy(cClrTxt,"Chap");
		if(field[5][0]=='0')
			strcpy(cStaticIP,"");
		else
			strcpy(cStaticIP,"StaticIP");
                printf("<a class=darkLink href=unxsRadius.cgi?gcFunction=tProfile"
			"&uProfile=%s>%s (%s %s %s %s)</a><br>\n",field[0],field[1],
			ForeignKey("tUser","cLogin",uExtUser),
			ForeignKey("tServer","cLabel",uExtServer),
			cClrTxt,cStaticIP);
        }
        mysql_free_result(res);

}//void tProfileNavList(void)


void CopyToAllNames(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	time_t luClock;

	if(!uProfile || cProfile[0]==0)
		tProfile("Unexpected error");

	time(&luClock);

	sprintf(gcQuery,"SELECT uProfileName FROM tProfileName WHERE uProfileName!=%u AND uOwner=%u",
			uProfileName,uOwner);
	macro_mySQLRunAndStore(res);
        while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"INSERT INTO tProfile SET uUser=%u,uServer=%u,uProfileName=%s,"
				"uClearText=%u,uStaticIP=%u,cComment='CopyToAllNames from uProfile=%u',"
				"cProfile='%s',uHourStart=%u,uHourEnd=%u,uWeekDayStart=%u,uWeekDayEnd=%u,"
				"uActivationDate=%lu,uDeactivationDate=%lu,uCreatedDate=%lu,uCreatedBy=%u,"
				"uOwner=%u",
				uUser
				,uServer
				,field[0]
				,uClearText
				,uStaticIP
				,uProfile
				,cProfile
				,uHourStart
				,uHourEnd
				,uWeekDayStart
				,uWeekDayEnd
				,uActivationDate
				,uDeactivationDate
				,luClock
				,guLoginClient
				,uOwner);
		macro_mySQLQueryHTMLError;
	}
        mysql_free_result(res);

}//void CopyToAllNames(void)


void BasictProfileCheck(void)
{
	if(!uProfileName)
		tProfile("<blink>Error:</blink> Must select uProfileName from the drop-down");

	if(!cProfile[0])
		tProfile("<blink>Error:</blink> cProfile is required");

}//void BasictProfileCheck(void)


void tTablePullDownCompanies(unsigned uSelector)
{
	if(guPermLevel<10)
		return;

        MYSQL_RES *res;         
        MYSQL_ROW field;

        register int i,n;
    
	sprintf(gcQuery,"SELECT uClient,cLabel FROM " TCLIENT
				" WHERE cLabel!='%s'"
				" AND uClient IN"
				" (SELECT uClient FROM " TCLIENT " WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY cLabel",
					gcUser,
					guCompany,
					guCompany);

        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

        if(i>0)
        {
		printf("<p><u>Optionally Select an Existing Company</u><br>");
                printf("<select name=cForCompanyPullDown>\n");

                //Default no selection
                printf("<option>---</option>\n");

                for(n=0;n<i;n++)
                {
                        unsigned ufield0=0;

                        field=mysql_fetch_row(res);
                        sscanf(field[0],"%u",&ufield0);

                        if(uSelector != ufield0)
                        {
                                printf("<option>%s</option>\n",field[1]);
                        }
                        else
                        {
                                printf("<option selected>%s</option>\n",field[1]);
                        }
                }
        	printf("</select><p>\n");
        }

}//tTablePullDownCompanies()


//Must be used to insure users are not lost, not only when deleting a profile (or profile name) but when modifying
//the owner of one as well.
unsigned uProfileNameUsed(const unsigned uProfileName,const unsigned uOwner)
{
        MYSQL_RES *res;         

	sprintf(gcQuery,"SELECT tUser.uUser FROM tUser,tProfileName"
			" WHERE tUser.uProfileName=tProfileName.uProfileName"
			" AND tProfileName.uProfileName=%u AND tProfileName.uOwner=%u",uProfileName,uOwner);
	macro_mySQLRunAndStore(res);

	if(mysql_num_rows(res)==0)
	{
		mysql_free_result(res);
		return(0);
	}
	mysql_free_result(res);
	return(1);

}//unsigned uProfileNameUsed()
