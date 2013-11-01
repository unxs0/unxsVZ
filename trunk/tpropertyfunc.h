/*
FILE
	$Id$
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis for Unixservice, LLC.
 
*/

static char cReturn[100]={""};
static char cUBCName[100]={""};
static long unsigned luBarrier=0;
static long unsigned luLimit=0;
static char cuDatacenterSelect[32]={""};
static char cDatacenterSelect[64]={""};//used in list in .c file
static unsigned uTargetDatacenter=0;

//ModuleFunctionProtos()
unsigned SetUBCJob(unsigned uContainer,char *cSet);
void htmlReturnLink(void);
void htmlGlossaryLink(char *cLabel);
void htmlUBCDatacenterSelect(char *cuDatacenterSelect,unsigned uMode);

//jobqueue.c
unsigned SetContainerProperty(const unsigned uContainer,const char *cPropertyName,const  char *cPropertyValue);


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
		else if(!strcmp(entries[i].name,"htmlUBCDatacenterSelect") && entries[i].val[0]!='-') 
		{
			sprintf(cDatacenterSelect,"%.63s",entries[i].val);
			uTargetDatacenter=ReadPullDown("tDatacenter","cLabel",entries[i].val);
			sprintf(cuDatacenterSelect,"%u",uTargetDatacenter);
			char cLogfile[64]={"/tmp/unxsvzlog"};
			if((gLfp=fopen(cLogfile,"a"))==NULL)
                		tProperty("Could not open logfile");
			if(uTargetDatacenter && ConnectToOptionalUBCDb(uTargetDatacenter,0))
				tProperty("ConnectToOptionalUBCDb() error");
			guUsingUBC=1;
		}
	}
}//void ExtProcesstPropertyVars(pentry entries[], int x)


void ExttPropertyCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tPropertyTools"))
	{
		//ModuleFunctionProcess()
		unsigned uForDC=0;

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

				if(guCompany!=1)
					uOwner=guCompany;
				else if(uType==1)
					sscanf(ForeignKey("tDatacenter","uOwner",uKey),"%u",&uOwner);
				else if(uType==2)
					sscanf(ForeignKey("tNode","uOwner",uKey),"%u",&uOwner);
				else if(uType==3)
					sscanf(ForeignKey("tContainer","uOwner",uKey),"%u",&uOwner);

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
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD) || (uForDC=!strcmp(gcCommand,"Confirm Modify Datacenter")))
                {
                        ProcesstPropertyVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				if(!cValue[0])
					tProperty("<blink>Error</blink>: cValue required");
				if(!cName[0])
					tProperty("<blink>Error</blink>: cName required");
				//Check entries here
                        	guMode=0;

				if(uForDC && uType==2)
				{
					unsigned uDatacenter=0;
					sscanf(ForeignKey("tNode","uDatacenter",uKey),"%u",&uDatacenter);
					if(!uDatacenter)
						tProperty("<blink>Error</blink>: CModD: uDatacenter required");
					sprintf(gcQuery,"UPDATE tProperty SET cValue='%s',uModDate=UNIX_TIMESTAMP(NOW()),uModBy=%u"
							" WHERE cName='%s' AND uType=2"
							" AND uKey IN (SELECT uNode FROM tNode WHERE uStatus=1 AND uDatacenter=%u)"
								,cValue,guLoginClient,cName,uDatacenter);
					//tProperty(gcQuery);
	        			mysql_query(&gMysql,gcQuery);
       		 			if(mysql_errno(&gMysql))
						tProperty("<blink>Error</blink>: gcQuery error");
					if(mysql_affected_rows(&gMysql))
						tProperty("Confirm Modify Datacenter: Ok");
					else
						tProperty("Confirm Modify Datacenter: Nothing changed");
				}
				
				uModBy=guLoginClient;
				ModtProperty();
			}
			else
				tProperty("<blink>Error</blink>: Insufficient permision to mod");
                }
                else if(!strcmp(gcCommand,"Set UBC/Diskspace"))
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
                else if(!strcmp(gcCommand,"Set UBC/Diskspace +20%"))
                {
                        ProcesstPropertyVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				char cMessage[256];
				char cluBarrier[32];
				char cluLimit[32];

                        	guMode=0;
				if(!uKey)
					tProperty("<blink>Error</blink>: uKey==0");
				if(uType!=3)
					tProperty("<blink>Error</blink>: uType!=VZContainer");
				if(luBarrier==0 || luLimit==0)
					tProperty("<blink>Error</blink>: luBarrier==0 || luLimit==0");

				luBarrier=(long unsigned)luBarrier*1.2;
				luLimit=(long unsigned)luLimit*1.2;
				sprintf(cluBarrier,"%lu",luBarrier);
				sprintf(cluLimit,"%lu",luLimit);
				sprintf(cMessage,"--%.99s %.31s:%.31s",cUBCName,cluBarrier,cluLimit);
				if(SetUBCJob(uKey,cMessage))
				{

					if(!strcmp(cUBCName,"diskspace"))
						sprintf(cMessage,"1k-blocks.luSoftlimit");
					else
						sprintf(cMessage,"%.99s.luLimit",cUBCName);
					if(SetContainerProperty(uKey,cMessage,cluBarrier))
						tProperty(cMessage);
						

					if(!strcmp(cUBCName,"diskspace"))
						sprintf(cMessage,"1k-blocks.luHardlimit");
					else
						sprintf(cMessage,"%.99s.luBarrier",cUBCName);
					if(SetContainerProperty(uKey,cMessage,cluLimit))
						tProperty(cMessage);

					sprintf(cMessage,"Set UBC --%s %s:%s job created ok",
							cUBCName,cluBarrier,cluLimit);
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
                else if(!strcmp(gcCommand,"Set UBC/Diskspace -20%"))
                {
                        ProcesstPropertyVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				char cMessage[256];
				char cluBarrier[32];
				char cluLimit[32];

                        	guMode=0;
				if(!uKey)
					tProperty("<blink>Error</blink>: uKey==0");
				if(uType!=3)
					tProperty("<blink>Error</blink>: uType!=VZContainer");
				if(luBarrier==0 || luLimit==0)
					tProperty("<blink>Error</blink>: luBarrier==0 || luLimit==0");

				luBarrier=(long unsigned)luBarrier*0.8;
				luLimit=(long unsigned)luLimit*0.8;
				sprintf(cluBarrier,"%lu",luBarrier);
				sprintf(cluLimit,"%lu",luLimit);
				sprintf(cMessage,"--%.99s %.31s:%.31s",cUBCName,cluBarrier,cluLimit);
				if(SetUBCJob(uKey,cMessage))
				{

					if(!strcmp(cUBCName,"diskspace"))
						sprintf(cMessage,"1k-blocks.luSoftlimit");
					else
						sprintf(cMessage,"%.99s.luLimit",cUBCName);
					if(SetContainerProperty(uKey,cMessage,cluBarrier))
						tProperty(cMessage);
						

					if(!strcmp(cUBCName,"diskspace"))
						sprintf(cMessage,"1k-blocks.luHardlimit");
					else
						sprintf(cMessage,"%.99s.luBarrier",cUBCName);
					if(SetContainerProperty(uKey,cMessage,cluLimit))
						tProperty(cMessage);

					sprintf(cMessage,"Set UBC --%s %s:%s job created ok",
							cUBCName,cluBarrier,cluLimit);
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
	//Expanded to allow setting of --diskspace

	if((cp=strstr(cUBCName,".luBarrier")) || (cp=strstr(cUBCName,".luLimit"))
			|| (cp=strstr(cUBCName,".luHardlimit")) 
			|| (cp=strstr(cUBCName,".luSoftlimit")) )
	{
        	MYSQL_RES *res;
        	MYSQL_ROW field;

		if(!guUsingUBC)
			gMysqlUBC=gMysql;

		*cp=0;
		sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE"
				" uType=3"
				" AND uKey=%1$u"
				" AND (cName='%2$s.luLimit' OR cName='%2$s.luBarrier' OR"
				" cName='%2$s.luHardlimit' OR cName='%2$s.luSoftlimit')"
				" ORDER BY cName DESC",uKey,cUBCName);
	        mysql_query(&gMysqlUBC,gcQuery);
       		 if(mysql_errno(&gMysqlUBC))
			htmlPlainTextError(mysql_error(&gMysqlUBC));

		res=mysql_store_result(&gMysqlUBC);
		if(mysql_num_rows(res))
		{
			register unsigned uFirst=0;
			char cLimit[32]={""};
			char cBarrier[32]={""};

			printf("<p><u>OpenVZ UBC/Disk Modifiable Properties</u><br>");

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
			if(!strcmp(cUBCName,"1k-blocks"))
				sprintf(cUBCName,"diskspace");
			printf("<p><input type=submit class=largeButton name=gcCommand"
				" title='Create job for setting container UBC/Disk at current settings'"
				" value='Set UBC/Diskspace'>\n");
			printf("<p><input type=submit class=lalertButton name=gcCommand"
				" title='Create job for setting container UBC/Disk at current settings +20%%'"
				" value='Set UBC/Diskspace +20%%'>\n");
			printf("<p><input type=submit class=lwarnButton name=gcCommand"
				" title='Create job for setting container UBC/Disk at current settings -20%%'"
				" value='Set UBC/Diskspace -20%%'>\n");
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

		if(!guUsingUBC)
			gMysqlUBC=gMysql;
			
		*cp=0;
		sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE"
				" uType=3"
				" AND uKey=%u"
				" AND cName LIKE '%s.lu%%'"
				" ORDER BY cName DESC",uKey,cUBCName);
	        mysql_query(&gMysqlUBC,gcQuery);
       		 if(mysql_errno(&gMysqlUBC))
			htmlPlainTextError(mysql_error(&gMysqlUBC));
		res=mysql_store_result(&gMysqlUBC);
		if(mysql_num_rows(res))
		{
			printf("<p><u>OpenVZ UBC Related Properties</u><br>");

			while((field=mysql_fetch_row(res)))
			{	
				if(cuDatacenterSelect[0])
					printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tProperty&uProperty=%s"
				 		"&cReturn=%s&cuDatacenterSelect=%s>"
				 		"%s=%s</a><br>\n",field[0],cReturn,cuDatacenterSelect,field[1],field[2]);
				else
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
			printf("<u>Select external UBC server</u><br>");
			htmlUBCDatacenterSelect(cuDatacenterSelect,0);
			printf("<p>");
			htmlReturnLink();
			printf("<p><u>Enter/mod data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
			printf("<u>Select external UBC server</u><br>");
			htmlUBCDatacenterSelect(cuDatacenterSelect,0);
			printf("<p>");
			htmlReturnLink();
                        printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<u>Select external UBC server</u><br>");
			htmlUBCDatacenterSelect(cuDatacenterSelect,0);
			printf("<p>");
			htmlReturnLink();
			printf("<p><u>Review changes</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
			if(uType==2 && guPermLevel>10)
				printf("<p><input type=submit class=lalertButton name=gcCommand"
				" title='!Triple check! Modify this node property and all others at the same datacenter'"
				" value='Confirm Modify Datacenter'>\n");
                break;

		default:
			printf("<u>Select external UBC server</u><br>");
			htmlUBCDatacenterSelect(cuDatacenterSelect,1);
			printf("<p>");
			htmlReturnLink();
			htmlGlossaryLink(cName);
			printf("<u>Table Tips</u><br>");
			printf("In general the only properties that make sense to edit"
				" are those that are not VZ UBC properties. The exception being"
				" editing barrier:limit pairs for container modification"
				" via [Set UBC/Diskspace] button. In this last case you must edit the barrier"
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
		else if(!strcmp(gentries[i].name,"cuDatacenterSelect"))
		{
			sscanf(gentries[i].val,"%u",&uTargetDatacenter);
			sprintf(cDatacenterSelect,"%.63s",ForeignKey("tDatacenter","cLabel",uTargetDatacenter));
			sprintf(cuDatacenterSelect,"%u",uTargetDatacenter);
			char cLogfile[64]={"/tmp/unxsvzlog"};
			if((gLfp=fopen(cLogfile,"a"))==NULL)
                		tProperty("Could not open logfile");
			if(uTargetDatacenter && ConnectToOptionalUBCDb(uTargetDatacenter,0))
				tProperty("ConnectToOptionalUBCDb() error");
			guUsingUBC=1;
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
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tProperty.uProperty=%u ORDER BY uProperty",uProperty);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cName") || !strcmp(gcFilter,"cName-NewFirst"))
        {
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");

        	if(gcAuxFilter[0])
		{
			sprintf(cCat," %.255s AND ",gcAuxFilter);
			strcat(gcQuery,cCat);
		}

        	if(strcmp(gcFilter,"cName-NewFirst"))
			sprintf(cCat,"tProperty.cName LIKE '%.99s' ORDER BY cName,uProperty",gcCommand);
		else
			sprintf(cCat,"tProperty.cName LIKE '%.99s' ORDER BY uProperty DESC",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uType"))
        {
                sscanf(gcCommand,"%u",&uType);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tProperty.uType=%u ORDER BY uType,uProperty",uType);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uKey"))
        {
                sscanf(gcCommand,"%u",&uKey);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tProperty.uKey=%u ORDER BY uKey,uProperty",uKey);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uOwner"))
        {
                sscanf(gcCommand,"%u",&uOwner);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tProperty.uOwner=%u ORDER BY uOwner,uProperty",uOwner);
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
        if(strcmp(gcFilter,"cName"))
                printf("<option>cName</option>");
        else
                printf("<option selected>cName</option>");
        if(strcmp(gcFilter,"cName-NewFirst"))
                printf("<option>cName-NewFirst</option>");
        else
                printf("<option selected>cName-NewFirst</option>");
        if(strcmp(gcFilter,"uType"))
                printf("<option>uType</option>");
        else
                printf("<option selected>uType</option>");
        if(strcmp(gcFilter,"uKey"))
                printf("<option>uKey</option>");
        else
                printf("<option selected>uKey</option>");
        if(strcmp(gcFilter,"uOwner"))
                printf("<option>uOwner</option>");
        else
                printf("<option selected>uOwner</option>");
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

	//By allow new in mod we can save time duplicating similar records.
	if(guPermLevel>=10)
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


void htmlGlossaryLink(char *cLabel)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(!cLabel[0]) return;

	sprintf(gcQuery,"SELECT uGlossary FROM tGlossary WHERE cLabel='%s'",cLabel);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		printf("Glossary entry for <a class=darkLink href=#"
			" onClick=\"open_popup('?gcFunction=Glossary&cLabel=%1$s')\">%1$s</a><p>",cLabel);

	}
	mysql_free_result(res);

}//void htmlGlossaryLink()


void htmlUBCDatacenterSelect(char *cuDatacenterSelect,unsigned uMode)
{
        register int i,n;
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;
	char *cMode="";
	char cHidden[128]={""};

	if(!uMode)
		cMode="disabled";
      
	sprintf(gcQuery,"SELECT DISTINCT tDatacenter.cLabel,tDatacenter.uDatacenter"
			" FROM tProperty,tDatacenter"
			" WHERE tProperty.uKey=tDatacenter.uDatacenter"
			" AND tProperty.cName LIKE 'gcUBCDBIP_' AND tProperty.uType=1");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",gcQuery);
		return;
	}
	mysqlRes=mysql_store_result(&gMysql);
	i=mysql_num_rows(mysqlRes);
        if(i>0)
        {
		char cuDatacenter[16]={""};
                printf("<select name=htmlUBCDatacenterSelect %s>\n",cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {
                        mysqlField=mysql_fetch_row(mysqlRes);
			unsigned uA=0;
			if(sscanf(mysqlField[1],"%u",&uA)==1)
			{
				sprintf(cuDatacenter,"%u",uA);
				if(strcmp(cuDatacenter,cuDatacenterSelect))
					printf("<option>%s</option>\n",mysqlField[0]);
                        	else
                        	{
					printf("<option selected>%s</option>\n",mysqlField[0]);
					if(!uMode)
						sprintf(cHidden,"<input type=hidden name=htmlUBCDatacenterSelect value='%s'>\n",mysqlField[0]);
                        	}
			}
                }
        }
        else
        {
		printf("<select name=htmlUBCDatacenterSelect %s><option title='No selection'>---</option></select>\n",cMode);
		if(!uMode)
			sprintf(cHidden,"<input type=hidden name=htmlUBCDatacenterSelect value='---'>\n");
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//void htmlUBCDatacenterSelect(char *cuDatacenterSelect,unsigned uMode)

