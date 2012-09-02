/*
FILE
	$Id: tdatacenterfunc.h 1860 2012-02-01 15:07:07Z Dylan $
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Unixservice, LLC.
*/

static unsigned uClone=0;
static unsigned uOldDatacenter=0;
static unsigned uTargetNode=0;
static char cuTargetNodePullDown[256]={""};

//ModuleFunctionProtos()
void tDatacenterNavList(void);

//tgroupfunc.h
void tGroupNavList(void);

//tclientfunc.h
static unsigned uForClient=0;
static char cForClientPullDown[256]={"---"};

void ExtProcesstDatacenterVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uClone")) 
			uClone=1;
		else if(!strcmp(entries[i].name,"cForClientPullDown"))
		{
			strcpy(cForClientPullDown,entries[i].val);
			uForClient=ReadPullDown(TCLIENT,"cLabel",cForClientPullDown);
		}
		else if(!strcmp(entries[i].name,"cuTargetNodePullDown"))
		{
			sprintf(cuTargetNodePullDown,"%.255s",entries[i].val);
			uTargetNode=ReadPullDown("tNode","cLabel",cuTargetNodePullDown);
		}
	}
}//void ExtProcesstDatacenterVars(pentry entries[], int x)


void ExttDatacenterCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tDatacenterTools"))
	{
        	MYSQL_RES *res;
		time_t uActualModDate= -1;

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
				uOldDatacenter=uDatacenter;
	                        ProcesstDatacenterVars(entries,x);
                        	guMode=2000;
	                        tDatacenter(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
                        	ProcesstDatacenterVars(entries,x);
				uOldDatacenter=uDatacenter;

                        	guMode=2000;
				//Check entries here
				if(strlen(cLabel)<3)
					tDatacenter("<blink>Error</blink>: Must supply valid cLabel. Min 3 chars.");
				sprintf(gcQuery,"SELECT uDatacenter FROM tDatacenter WHERE cLabel='%s'",
						cLabel);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tDatacenter("<blink>Error</blink>: Datacenter cLabel is used!");
				}
                        	guMode=0;

				if(!uForClient)
					uOwner=guCompany;
				else
					uOwner=uForClient;
				uDatacenter=0;
				uCreatedBy=guLoginClient;
				//GetClientOwner(guLoginClient,&guReseller);
				//uOwner=guReseller;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				uStatus=1;//Active
				NewtDatacenter(1);
				sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=1"
						",cName='Contact',uOwner=%u,uCreatedBy=%u"
						",uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,uDatacenter,guCompany,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				if(uDatacenter)
					tDatacenter("New datacenter created");
				else
					tDatacenter("<blink>Error</blink>: Datacenter not created!");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstDatacenterVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=0;
				sscanf(ForeignKey("tDatacenter","uModDate",uDatacenter),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tDatacenter("<blink>Error</blink>: This record was modified. Reload it.");
				sprintf(gcQuery,"SELECT uDatacenter FROM tContainer WHERE uDatacenter=%u",
									uDatacenter);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tDatacenter("<blink>Error</blink>: Can't delete a datacenter"
							" used by a container!");
				}
	                        guMode=2001;
				tDatacenter(LANG_NB_CONFIRMDEL);
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstDatacenterVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				sscanf(ForeignKey("tDatacenter","uModDate",uDatacenter),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tDatacenter("<blink>Error</blink>: This record was modified. Reload it.");
				sprintf(gcQuery,"SELECT uDatacenter FROM tContainer WHERE uDatacenter=%u",
									uDatacenter);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tDatacenter("<blink>Error</blink>: Can't delete a datacenter"
							" used by a container!");
				}
	                        guMode=0;
				DeletetDatacenter();
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstDatacenterVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{

				guMode=2002;
				tDatacenter(LANG_NB_CONFIRMMOD);
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstDatacenterVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
				if(strlen(cLabel)<3)
					tDatacenter("<blink>Error</blink>: Must supply valid cLabel. Min 3 chars.");
                        	guMode=0;

				uModBy=guLoginClient;
				if(uForClient)
				{
					sprintf(gcQuery,"UPDATE tDatacenter SET uOwner=%u WHERE uDatacenter=%u",
						uForClient,uDatacenter);
					mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
                				htmlPlainTextError(mysql_error(&gMysql));
					uOwner=uForClient;
				}
				ModtDatacenter();
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttDatacenterCommands(pentry entries[], int x)


void ExttDatacenterButtons(void)
{
	OpenFieldSet("tDatacenter Aux Panel",100);
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
				printf("<p>You change the record owner, just...");
				tTablePullDownResellers(guCompany,1);
			}
                break;

		default:
			printf("<u>Table Tips</u><br>");
			printf("A datacenter is a collection of hardware nodes."
					" These hardware nodes need not be located in the same"
					" physical datacenter. Hardware nodes host VZ containers."
					" It is in these containers that actual public services run."
					" uVeth='Yes' container traffic is not included"
					"in the datacenter graphs at this time.");
			tGroupNavList();
			tDatacenterNavList();
	}
	CloseFieldSet();

}//void ExttDatacenterButtons(void)


void ExttDatacenterAuxTable(void)
{
	if(!uDatacenter || guMode==2000 )//uMODE_NEW
		return;

        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"%s Property Panel",cLabel);
	OpenFieldSet(gcQuery,100);
	sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE uKey=%u AND uType="PROP_DATACENTER
			" ORDER BY cName",uDatacenter);

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
			printf("<td width=200 valign=top><a class=darkLink href=unxsSPS.cgi?"
					"gcFunction=tProperty&uProperty=%s&cReturn=tDatacenter_%u>"
					"%s</a></td><td valign=top><pre>%s</pre></td>\n",
						field[0],uDatacenter,field[1],field[2]);
			printf("</tr>\n");
		}
		printf("</table>");
	}

	CloseFieldSet();

}//void ExttDatacenterAuxTable(void)


void ExttDatacenterGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uDatacenter"))
		{
			sscanf(gentries[i].val,"%u",&uDatacenter);
			guMode=6;
		}
	}
	tDatacenter("");

}//void ExttDatacenterGetHook(entry gentries[], int x)


void ExttDatacenterSelect(void)
{
	ExtSelect("tDatacenter",VAR_LIST_tDatacenter);

}//void ExttDatacenterSelect(void)


void ExttDatacenterSelectRow(void)
{
	ExtSelectRow("tDatacenter",VAR_LIST_tDatacenter,uDatacenter);

}//void ExttDatacenterSelectRow(void)


void ExttDatacenterListSelect(void)
{
	char cCat[512];

	ExtListSelect("tDatacenter",VAR_LIST_tDatacenter);

	//Changes here must be reflected below in ExttDatacenterListFilter()
        if(!strcmp(gcFilter,"uDatacenter"))
        {
                sscanf(gcCommand,"%u",&uDatacenter);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tDatacenter.uDatacenter=%u ORDER BY uDatacenter",uDatacenter);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uDatacenter");
        }

}//void ExttDatacenterListSelect(void)


void ExttDatacenterListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uDatacenter"))
                printf("<option>uDatacenter</option>");
        else
                printf("<option selected>uDatacenter</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttDatacenterListFilter(void)


void ExttDatacenterNavBar(void)
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

}//void ExttDatacenterNavBar(void)


void tDatacenterNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tDatacenter","tDatacenter.uDatacenter,tDatacenter.cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tDatacenterNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tDatacenterNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tDatacenter&uDatacenter=%s>"
				"%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tDatacenterNavList(void)


void tTablePullDownAvail(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode)
{
        register int i,n;
        char cLabel[128];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[34]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTablePullDown()");
                return;
        }

        //Extended functionality
        strncpy(cLocalTableName,cTableName,255);
        if((cp=strchr(cLocalTableName,';')))
        {
                strncpy(cSelectName,cp+1,32);
                cSelectName[32]=0;
                *cp=0;
        }


	if(guCompany==1)
        	sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 ORDER BY %s",
                                cFieldName,cLocalTableName,cOrderby);
	else
		sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND uOwner=%u ORDER BY %s",
				cFieldName,cLocalTableName,guCompany,cOrderby);

	MYSQL_RUN_STORE_TEXT_RET_VOID(mysqlRes);
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
                printf("<select name=%s %s>\n",cLabel,cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                        {
                             printf("<option>%s</option>\n",mysqlField[1]);
                        }
                        else
                        {
                             printf("<option selected>%s</option>\n",mysqlField[1]);
			     if(!uMode)
			     sprintf(cHidden,"<input type=hidden name=%.32s value='%.32s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%.32s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDownAvail()


void tTablePullDownOwnerAvail(const char *cTableName, const char *cFieldName,
                        const char *cOrderby, unsigned uSelector, unsigned uMode)
{
        register int i,n;
        char cLabel[256];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[100]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTablePullDownAvail()");
                return;
        }

        //Extended functionality
        strncpy(cLocalTableName,cTableName,255);
        if((cp=strchr(cLocalTableName,';')))
        {
                strncpy(cSelectName,cp+1,99);
                cSelectName[99]=0;
                *cp=0;
        }


	if(guLoginClient==1)
        	sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 ORDER BY %s",
                                cFieldName,cLocalTableName,cOrderby);
	else
        	sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND ( uOwner=%u OR uOwner IN"
				" (SELECT uClient FROM " TCLIENT " WHERE uOwner=%u)) ORDER BY %s",
				cFieldName,cLocalTableName,guCompany,guCompany,cOrderby);

	MYSQL_RUN_STORE_TEXT_RET_VOID(mysqlRes);
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
		int unsigned field0;
                printf("<select name=%s %s>\n",cLabel,cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {

			field0=0;
                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                        {
                             printf("<option>%s</option>\n",mysqlField[1]);
                        }
                        else
                        {
                             printf("<option selected>%s</option>\n",mysqlField[1]);
			     if(!uMode)
			     sprintf(cHidden,"<input type=hidden name=%.99s value='%.99s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%99s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDownOwnerAvail()


void tTablePullDownDatacenter(const char *cTableName, const char *cFieldName,
		const char *cOrderby, unsigned uSelector, unsigned uMode, const char *cDatacenter,
		unsigned uType, unsigned uDatacenter)
{
        register int i,n;
        char cLabel[256];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[100]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTablePullDownDatacenter()");
                return;
        }

        //Extended functionality
        sprintf(cLocalTableName,"%.255s",cTableName);
        if((cp=strchr(cLocalTableName,';')))
        {
                sprintf(cSelectName,"%.99s",cp+1);
                *cp=0;
        }

	if(uType)
		//This does not work in 5.0.77
	       	//sprintf(gcQuery,"SELECT _rowid AS uRowid,%s FROM %s WHERE"
		//	" LOCATE('All Datacenters',"
		//	"(SELECT cValue FROM tProperty WHERE cName='cDatacenter' AND uType=%u AND uKey=uRowid))>0"
		//	" OR LOCATE('%s',"
		//	"(SELECT cValue FROM tProperty WHERE cName='cDatacenter' AND uType=%u AND uKey=uRowid))>0"
		//	" ORDER BY %s",
		//		cFieldName,cLocalTableName,uType,cDatacenter,uType,cOrderby);

		//SELECT _rowid,cLabel FROM tOSTemplate WHERE _rowid IN (SELECT uKey FROM tProperty WHERE cName='cDatacenter' AND uType=8 AND (cValue='All Datacenters' OR cValue='Wilshire1'));
	       	sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE"
				" _rowid IN"
				" (SELECT uKey FROM tProperty WHERE cName='cDatacenter' AND"
				" uType=%u AND (cValue='All Datacenters' OR LOCATE('%s',cValue)>0))"
				" ORDER BY %s",
					cFieldName,cLocalTableName,uType,cDatacenter,cOrderby);
	else
	       	sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uDatacenter=%u ORDER BY %s",
				cFieldName,cLocalTableName,uDatacenter,cOrderby);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		return;
	}
	mysqlRes=mysql_store_result(&gMysql);
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
		int unsigned field0;

		printf("<select name=%s %s>\n",cLabel,cMode);
		//Default no selection
       		printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {

			field0=0;
                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                        {
                             printf("<option>%s</option>\n",mysqlField[1]);
                        }
                        else
                        {
                             printf("<option selected>%s</option>\n",mysqlField[1]);
			     if(!uMode)
			     sprintf(cHidden,"<input type=hidden name=%.99s value='%.99s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%99s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDownDatacenter()


void tTablePullDownOwnerAvailDatacenter(const char *cTableName, const char *cFieldName,
	const char *cOrderby, unsigned uSelector, unsigned uMode,unsigned uDatacenter,unsigned uClient)
{
        register int i,n;
        char cLabel[256];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[100]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input tTablePullDownAvailDatacenter()");
                return;
        }

        //Extended functionality
        strncpy(cLocalTableName,cTableName,255);
        if((cp=strchr(cLocalTableName,';')))
        {
                strncpy(cSelectName,cp+1,99);
                cSelectName[99]=0;
                *cp=0;
        }


	sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uAvailable=1 AND uDatacenter=%u AND uOwner=%u ORDER BY %s",
				cFieldName,cLocalTableName,uDatacenter,uClient,cOrderby);

	MYSQL_RUN_STORE_TEXT_RET_VOID(mysqlRes);
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
		int unsigned field0;
                printf("<select name=%s %s>\n",cLabel,cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {

			field0=0;
                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                        {
                             printf("<option>%s</option>\n",mysqlField[1]);
                        }
                        else
                        {
                             printf("<option selected>%s</option>\n",mysqlField[1]);
			     if(!uMode)
			     sprintf(cHidden,"<input type=hidden name=%.99s value='%.99s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%99s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//tTablePullDownOwnerAvailDatacenter()

