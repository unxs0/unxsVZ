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


//extern
void tPBXtDIDNavList(unsigned uPBX);

void ExtProcesstDIDVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstDIDVars(pentry entries[], int x)


void ExttDIDCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tDIDTools"))
	{
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstDIDVars(entries,x);
                        	guMode=2000;
	                        tDID(LANG_NB_CONFIRMNEW);
			}
			else
				tDID("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstDIDVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uDID=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtDID(0);
			}
			else
				tDID("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstDIDVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tDID(LANG_NB_CONFIRMDEL);
			}
			else
				tDID("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstDIDVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetDID();
			}
			else
				tDID("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstDIDVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tDID(LANG_NB_CONFIRMMOD);
			}
			else
				tDID("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstDIDVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;
				uModBy=guLoginClient;
				ModtDID();
			}
			else
				tDID("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttDIDCommands(pentry entries[], int x)


void ExttDIDButtons(void)
{
	OpenFieldSet("tDID Aux Panel",100);
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
			printf("DIDs are used to route incoming SIP traffic from Carriers to PBXs. You need to make sure that"
				" your routing logic strips any leading chars like 1s or other chars that your Carriers might"
				" be sending.");
			printf("<p><u>Record Context Info</u><br>");
			printf("If possible we provide links to related items.");
			if(uPBX)
			{
				printf(" Like DIDs of same PBX:");
				tPBXtDIDNavList(uPBX);
			}
			printf("<p><u>Operations</u><br>");
			printf("This feature is not available at this time.<br>");
			//printf("<br><input type=submit class=largeButton title='Sample button help'"
			//		" name=gcCommand value='Sample Button'>");
	}
	CloseFieldSet();

}//void ExttDIDButtons(void)


void ExttDIDAuxTable(void)
{

}//void ExttDIDAuxTable(void)


void ExttDIDGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uDID"))
		{
			sscanf(gentries[i].val,"%u",&uDID);
			guMode=6;
		}
	}
	tDID("");

}//void ExttDIDGetHook(entry gentries[], int x)


void ExttDIDSelect(void)
{
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tDID ORDER BY"
				" uDID",
				VAR_LIST_tDID);
	else
		sprintf(gcQuery,"SELECT %s FROM tDID,tClient WHERE tDID.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uDID",
					VAR_LIST_tDID,guCompany,guCompany);
}//void ExttDIDSelect(void)


void ExttDIDSelectRow(void)
{
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tDID WHERE uDID=%u",
			VAR_LIST_tDID,uDID);
	else
                sprintf(gcQuery,"SELECT %s FROM tDID,tClient"
                                " WHERE tDID.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tDID.uDID=%u",
                        		VAR_LIST_tDID
					,guCompany,guCompany
					,uDID);
}//void ExttDIDSelectRow(void)


void ExttDIDListSelect(void)
{
	char cCat[512];

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tDID",
				VAR_LIST_tDID);
	else
		sprintf(gcQuery,"SELECT %s FROM tDID,tClient"
				" WHERE tDID.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tDID
				,guCompany
				,guCompany);

	//Changes here must be reflected below in ExttDIDListFilter()
        if(!strcmp(gcFilter,"uDID"))
        {
                sscanf(gcCommand,"%u",&uDID);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tDID.uDID=%u",uDID);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cDID"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tDID.cDID LIKE '%s%%' ORDER BY cDID",gcCommand);
		strcat(gcQuery,cCat);
	}
        else if(!strcmp(gcFilter,"uPBX"))
        {
                sscanf(gcCommand,"%u",&uPBX);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tDID.uPBX=%u",uPBX);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uDID");
        }

}//void ExttDIDListSelect(void)


void ExttDIDListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uDID"))
                printf("<option>uDID</option>");
        else
                printf("<option selected>uDID</option>");
        if(strcmp(gcFilter,"cDID"))
                printf("<option>cDID</option>");
        else
                printf("<option selected>cDID</option>");
        if(strcmp(gcFilter,"uPBX"))
                printf("<option>uPBX</option>");
        else
                printf("<option selected>uPBX</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttDIDListFilter(void)


void ExttDIDNavBar(void)
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

}//void ExttDIDNavBar(void)


