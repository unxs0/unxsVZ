/*
FILE
	$Id$
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2007 Gary Wallis.
 
*/

static char cReturn[100]={""};
static char cUBCName[100]={""};
static long unsigned luBarrier=0;
static long unsigned luLimit=0;

//ModuleFunctionProtos()
unsigned SetUBCJob(unsigned uContainer,char *cSet);
void htmlReturnLink(void);
void htmlGlossaryLink(void);


void ExtProcesstPropertyVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cReturn")) 
			sprintf(cReturn,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"luBarrier")) 
			sscanf(entries[i].val,"%lu",&luBarrier);
		else if(!strcmp(entries[i].name,"luLimit")) 
			sscanf(entries[i].val,"%lu",&luLimit);
		else if(!strcmp(entries[i].name,"cUBCName")) 
			sprintf(cUBCName,"%.99s",entries[i].val);
	}
}//void ExtProcesstPropertyVars(pentry entries[], int x)


void ExttPropertyCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tPropertyTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=7)
			{
	                        ProcesstPropertyVars(entries,x);
                        	guMode=2000;
	                        tProperty(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=7)
			{
                        	ProcesstPropertyVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uProperty=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtProperty(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstPropertyVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tProperty(LANG_NB_CONFIRMDEL);
			}
			else
				tProperty("<blink>Error</blink>: Insufficient permision to del");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstPropertyVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetProperty();
			}
			else
				tProperty("<blink>Error</blink>: Insufficient permision to del");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstPropertyVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tProperty(LANG_NB_CONFIRMMOD);
			}
			else
				tProperty("<blink>Error</blink>: Insufficient permision to mod");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstPropertyVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtProperty();
			}
			else
				tProperty("<blink>Error</blink>: Insufficient permision to mod");
                }
                else if(!strcmp(gcCommand,"Set UBC"))
                {
                        ProcesstPropertyVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				char cMessage[64];

                        	guMode=0;
				if(!uKey)
					tProperty("<blink>Error</blink>: uKey==0");
				if(uType!=3)
					tProperty("<blink>Error</blink>: uType!=VZContainer");
				if(luBarrier==0 || luLimit==0)
					tProperty("<blink>Error</blink>: luBarrier==0 || luLimit==0");

				sprintf(cMessage,"--%s %lu:%lu",cUBCName,luBarrier,luLimit);
				if(SetUBCJob(uKey,cMessage))
				{

					sprintf(cMessage,"Set UBC --%s %lu:%lu job created ok",
					cUBCName,luBarrier,luLimit);
					tProperty(cMessage);
				}
				else
				{
					tProperty("<blink>Error</blink>: No job created!");
				}
			}
			else
				tProperty("<blink>Error</blink>: Insufficient permision to mod");
                }
	}

}//void ExttPropertyCommands(pentry entries[], int x)


void htmlReturnLink(void)
{
	if(cReturn[0])
	{
		char *cp;
		char cTable[32]={""};
		char cTableKey[32]={""};
		unsigned uKey=0;

		if((cp=strchr(cReturn,'_')))
		{
			*cp=0;
			sprintf(cTable,"%.31s",cReturn);
			sprintf(cTableKey,"u%.30s",cReturn+1);
			sscanf(cp+1,"%u",&uKey);
			*cp='_';
		}
		printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=%s&%s=%u>Return to %s (%s %u)</a><p>\n",
				cTable,cTableKey,uKey,
				ForeignKey(cTable,"cLabel",uKey),
				cTable, uKey);
		printf("<input type=hidden name=cReturn value=%s>",cReturn);
	}

}//void htmlReturnLink(void)


void htmlUBCEdit(void)
{
	char cUBCName[100];
	char *cp;

	sprintf(cUBCName,"%.99s",cName);

	//Provide entrance to container conf file ubc editing

	if((cp=strstr(cUBCName,".luBarrier")) || (cp=strstr(cUBCName,".luLimit")))
	{
        	MYSQL_RES *res;
        	MYSQL_ROW field;


		*cp=0;
		sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE"
				" uType=3"
				" AND uKey=%u"
				" AND (cName='%s.luLimit' OR cName='%s.luBarrier')"
				" ORDER BY cName DESC",uKey,cUBCName,cUBCName);
	        mysql_query(&gMysql,gcQuery);
       		 if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res))
		{
			register unsigned uFirst=0;
			char cLimit[32]={""};
			char cBarrier[32]={""};

			printf("<p><u>OpenVZ UBC Modifiable Properties</u><br>");

			while((field=mysql_fetch_row(res)))
			{	
				 printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tProperty&uProperty=%s"
				 	"&cReturn=%s>"
				 	"%s=%s</a><br>\n",field[0],cReturn,field[1],field[2]);
				if(uFirst)
					sprintf(cLimit,"%.31s",field[2]);
				else
					sprintf(cBarrier,"%.31s",field[2]);
				uFirst++;
					
			}
			printf("<p><input type=submit class=largeButton name=gcCommand"
				" title='Create job for setting container UBC --%s %s:%s'"
				" value='Set UBC'>\n",cUBCName,cBarrier,cLimit);
			printf("<input type=hidden name=luBarrier value=%s>",cBarrier);
			printf("<input type=hidden name=luLimit value=%s>\n",cLimit);
			printf("<input type=hidden name=cUBCName value=%s>\n",cUBCName);
			printf("<input type=hidden name=cReturn value=%s>",cReturn);
		}
		mysql_free_result(res);
	}

}//void htmlUBCEdit(void)


void htmlUBCInfo(void)
{
	char cUBCName[100];
	char *cp;

	sprintf(cUBCName,"%.99s",cName);

	if((cp=strstr(cUBCName,".lu")))
	{
        	MYSQL_RES *res;
        	MYSQL_ROW field;

		*cp=0;
		sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE"
				" uType=3"
				" AND uKey=%u"
				" AND cName LIKE '%s.lu%%'"
				" ORDER BY cName DESC",uKey,cUBCName);
	        mysql_query(&gMysql,gcQuery);
       		 if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res))
		{
			printf("<p><u>OpenVZ UBC Related Properties</u><br>");

			while((field=mysql_fetch_row(res)))
			{	
				 printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tProperty&uProperty=%s"
				 	"&cReturn=%s>"
				 	"%s=%s</a><br>\n",field[0],cReturn,field[1],field[2]);
			}
		}
		mysql_free_result(res);
	}

}//void htmlUBCInfo(void)


void ExttPropertyButtons(void)
{
	OpenFieldSet("tProperty Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			htmlReturnLink();
			printf("<p><u>Enter/mod data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
			htmlReturnLink();
                        printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			htmlReturnLink();
			printf("<p><u>Review changes</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			htmlReturnLink();
			htmlGlossaryLink();
			printf("<u>Table Tips</u><br>");
			printf("In general the only properties that make sense to edit"
				" are those that are not VZ UBC properties. The exception being"
				" editing barrier:limit pairs for container modification"
				" via [Set UBC] button. In this last case you must edit the barrier"
				" and limit items before the system loads them again.\n");
			htmlUBCInfo();
			htmlUBCEdit();
	}
	CloseFieldSet();

}//void ExttPropertyButtons(void)


void ExttPropertyAuxTable(void)
{

}//void ExttPropertyAuxTable(void)


void ExttPropertyGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uProperty"))
		{
			sscanf(gentries[i].val,"%u",&uProperty);
			guMode=3002;
		}
		else if(!strcmp(gentries[i].name,"cReturn"))
		{
			sprintf(cReturn,"%.99s",gentries[i].val);
		}
	}
	tProperty("");

}//void ExttPropertyGetHook(entry gentries[], int x)


void ExttPropertySelect(void)
{
	ExtSelect("tProperty",VAR_LIST_tProperty);

}//void ExttPropertySelect(void)


void ExttPropertySelectRow(void)
{
	ExtSelectRow("tProperty",VAR_LIST_tProperty,uProperty);

}//void ExttPropertySelectRow(void)


void ExttPropertyListSelect(void)
{
	char cCat[512];

	ExtListSelect("tProperty",VAR_LIST_tProperty);
	
	//Changes here must be reflected below in ExttPropertyListFilter()
        if(!strcmp(gcFilter,"uProperty"))
        {
                sscanf(gcCommand,"%u",&uProperty);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tProperty.uProperty=%u \
						ORDER BY uProperty",
						uProperty);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uProperty");
        }

}//void ExttPropertyListSelect(void)


void ExttPropertyListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uProperty"))
                printf("<option>uProperty</option>");
        else
                printf("<option selected>uProperty</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttPropertyListFilter(void)


void ExttPropertyNavBar(void)
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

}//void ExttPropertyNavBar(void)


unsigned SetUBCJob(unsigned uContainer,char *cSet)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;
	unsigned uDatacenter=0;
	unsigned uNode=0;

	sprintf(gcQuery,"SELECT tNode.uDatacenter,tNode.uNode FROM tNode,tContainer WHERE"
			" tNode.uNode=tContainer.uNode"
			" AND tContainer.uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uDatacenter);
		sscanf(field[1],"%u",&uNode);
	}
	else
	{
		mysql_free_result(res);
		return(0);
	}
	mysql_free_result(res);

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='SetUBCJob(%u)',cJobName='SetUBCJob'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",cJobData='%.99s'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uContainer,
				uDatacenter,uNode,uContainer,
				cSet,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	return(uCount);

}//unsigned SetUBCJob(...)


void htmlGlossaryLink(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(!cName[0]) return;

	sprintf(gcQuery,"SELECT uGlossary FROM tGlossary WHERE cLabel='%s'",cName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		printf("Glossary entry for <a class=darkLink href=unxsVZ.cgi?gcFunction=tGlossary"
				"&uGlossary=%s>%s</a><p>\n",field[0],cName);
	}
	mysql_free_result(res);

}//void htmlGlossaryLink(void)
