/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template and mysqlRAD2 author: (C) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details

 
*/

#include "mysqlisp.h"

void tClientConfigNavList(void);
static unsigned uExtClient=0;
static unsigned uInstance=0;

void ClientServiceConfigList(void);
unsigned GetInstanceStatus(unsigned uInstance);
unsigned GetInstanceStatusViaJob(unsigned uInstance);

//Extern tinstancefunc.h
int CheckParameterValue(char *cValue, char *cParamType, char *cRange, unsigned uUnique,
		                char *cExtVerify,char *cParameter,unsigned uDeployed);


void ExtProcesstClientConfigVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uInstance"))
			sscanf(entries[i].val,"%u",&uInstance);
		else if(!strcmp(entries[i].name,"uExtClient"))
			sscanf(entries[i].val,"%u",&uExtClient);
	}
}//void ExtProcesstClientConfigVars(pentry entries[], int x)


void ExttClientConfigCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tClientConfigTools"))
	{
		structParameter structParam;
		unsigned uInstanceStatus=0;
		unsigned uDeployed=1;

		if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstClientConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				if(!uInstance)
				tClientConfig("No uInstance, start with tInstance");

				if(strstr(cValue,"(NoMod)") && guPermLevel<12 )
					tClientConfig("No permission to modify this parameter");

				uInstanceStatus=GetInstanceStatus(uInstance);
				//sprintf(gcQuery,"%u %u",uInstance,uInstanceStatus);
				//tClientConfig(gcQuery);
				if(uInstanceStatus==unxsISP_NeverDeployed)
					uDeployed=0;
				GetParamInfo(uParameter,&structParam);
				if(!structParam.uAllowMod)
					tClientConfig("Can't modify this locked parameter");
				if(!structParam.uModPostDeploy && uDeployed)
				{
					tClientConfig("Can't modify this parameter after the product service has been deployed");
					//sprintf(gcQuery,"<font size=1>Can't modify this parameter after the product service has been deployed. uDeployed=%u structParam.uModPostDeploy=%u uInstanceStatus=%u uInstance=%u",uDeployed,structParam.uModPostDeploy,uInstanceStatus,uInstance);
				//tClientConfig(gcQuery);
				}

	
				guMode=2002;
				tClientConfig(LANG_NB_CONFIRMMOD);
			}
			else
				tClientConfig("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstClientConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				unsigned uErr=0;
				time_t clock;
				
				time(&clock);

                        	guMode=2002;
				//Check entries here
				if(!uInstance)
					tClientConfig("No uInstance, start with tInstance");

				uInstanceStatus=GetInstanceStatus(uInstance);
				if(uInstanceStatus==unxsISP_NeverDeployed)
					uDeployed=0;
				GetParamInfo(uParameter,&structParam);
				if(!structParam.uModPostDeploy && uDeployed)
					tClientConfig("Can't modify this parameter after the product service has been deployed");
				if(!structParam.uAllowMod)
					tClientConfig("Can't modify this locked parameter");
				if(strstr(cValue,"(NoMod)") && guPermLevel<12 )
					tClientConfig("Can't modify this parameter");
				if((uErr=CheckParameterValue(cValue,
							structParam.cParamType,
							structParam.cRange,
							structParam.uUnique,
							structParam.cExtVerify,
							structParam.cParameter,
							uDeployed)))
				{
					if(uErr==2)
						sprintf(gcQuery,"%s='%s' is out of range %s, type %s",
								structParam.cParameter,cValue,structParam.cRange,structParam.cParamType);
					else if(uErr==1)
						sprintf(gcQuery,"%s='%s' is not valid for type %s, range %s",
								structParam.cParameter,cValue,structParam.cParamType,structParam.cRange);
					else if(uErr==3)
						sprintf(gcQuery,"%s='%s' unconfigured type %s, range %s",
								structParam.cParameter,cValue,structParam.cParamType,structParam.cRange);
					else if(uErr==4)
						sprintf(gcQuery,"%s='%s' unique value required %s, range %s",
								structParam.cParameter,cValue,structParam.cParamType,structParam.cRange);
					else if(uErr==5)
						sprintf(gcQuery,"%s='%s' external verify failed",structParam.cParameter,cValue);
					else if(1)
						sprintf(gcQuery,"%s='%s' unknown error type %s, range %s, uUnique=%u, cExtVerify=%s",
								structParam.cParameter,cValue,structParam.cParamType,structParam.cRange
								,structParam.uUnique,structParam.cExtVerify);

					tClientConfig(gcQuery);
				}

	                        guMode=0;

				//Update in parts to determine if update really takes place
				sprintf(gcQuery,"UPDATE tClientConfig SET cValue='%s' WHERE uConfig=%u",
							TextAreaSave(cValue)
							,uConfig);
				mysql_query(&gMysql,gcQuery);
			        if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				
				if(mysql_affected_rows(&gMysql))
				{
					//cValue was really changed: Finish update
					sprintf(gcQuery,"UPDATE tClientConfig SET uModBy=%u,uModDate=%lu WHERE uConfig=%u",
						uModBy=guLoginClient
						,uModDate=clock
						,uConfig);
					mysql_query(&gMysql,gcQuery);
				        if(mysql_errno(&gMysql))
			                	htmlPlainTextError(mysql_error(&gMysql));
				
					if(uInstanceStatus!=unxsISP_Deployed)
					{
						tClientConfig("Product instance service parameter updated. Must redeploy for change to take effect.");
					}
					else
					{

						sprintf(gcQuery,"UPDATE tInstance SET uStatus=%u,uModBy=%u,uModDate=%lu WHERE uStatus=%u AND uInstance=%u",
								unxsISP_WaitingRedeployment,
								guLoginClient,
								clock,
								unxsISP_Deployed,
								uInstance);

			        		mysql_query(&gMysql,gcQuery);
					        if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));

						SubmitSingleServiceJob(uService,uInstance,"Mod",uOwner,clock);
						tClientConfig("Product instance service parameter updated. Service job created for redeployment");
					}

			}
			else
			{
				tClientConfig("No Mod: cValue was not changed");
			}

			}
                }
	}

}//void ExttClientConfigCommands(pentry entries[], int x)


void GetParamInfo(unsigned uParameter, structParameter *structParam)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        sprintf(gcQuery,"SELECT tParamType.cLabel,tParameter.cRange,tParameter.cParameter,"
			"tParameter.uUnique,tParameter.cExtVerify,tParameter.uModPostDeploy,"
			"tParameter.uAllowMod FROM tParameter,tParamType WHERE uParameter=%u "
			"AND tParameter.uParamType=tParamType.uParamType",uParameter);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                tClientConfig(mysql_error(&gMysql));
        
	res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		strcpy(structParam->cParamType,field[0]);
		strcpy(structParam->cRange,field[1]);
		strcpy(structParam->cParameter,field[2]);
		sscanf(field[3],"%u",&structParam->uUnique);
		strcpy(structParam->cExtVerify,field[4]);
		sscanf(field[5],"%u",&structParam->uModPostDeploy);
		sscanf(field[6],"%u",&structParam->uAllowMod);
	}
	mysql_free_result(res);

}//GetParamInfo()


void ExttClientConfigButtons(void)
{
	OpenFieldSet("tClientConfig Aux Panel",100);
	
	if(uInstance && uExtClient)
	        printf("<a class=darkLink title='Return to editing other service parameters' "
			"href=unxsISP.cgi?gcFunction=tInstance&uInstance=%u&uExtClient=%u>[Return]</a><p>\n",uInstance,uExtClient);

	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter required data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf("<font size=1>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

	}


	if(uInstance && uExtClient)
	{

		printf("<p><u><a class=darkLink title='Client this parameter belongs to. Short cut back to tClient' "
			"href=unxsISP.cgi?gcFunction=tClient&uClient=%u>%s</a></u>",uExtClient,ForeignKey("tClient","cLabel",uExtClient));
		printf("<input type=hidden name=uExtClient value=%u>",
				uExtClient);
		printf("<input type=hidden name=uInstance value=%u>\n",
				uInstance);

		printf("<p><u>Parameters NavList</u><br>");
		ClientServiceConfigList();
	}

	CloseFieldSet();

}//void ExttClientConfigButtons(void)


void ExttClientConfigAuxTable(void)
{

}//void ExttClientConfigAuxTable(void)


void ExttClientConfigGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uConfig"))
		{
			sscanf(gentries[i].val,"%u",&uConfig);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"uInstance"))
			sscanf(gentries[i].val,"%u",&uInstance);
		else if(!strcmp(gentries[i].name,"uExtClient"))
			sscanf(gentries[i].val,"%u",&uExtClient);
	}
	tClientConfig("");

}//void ExttClientConfigGetHook(entry gentries[], int x)


void ExttClientConfigSelect(void)
{
	char cExtra[100]={""};

	if(uInstance && uExtClient)
	{
		sprintf(cExtra,"uGroup=%u AND uService=%u",uGroup,uService);
		ExtSelectSearch("tClientConfig",VAR_LIST_tClientConfig,"1","1",cExtra,0);
	}
	else
		ExtSelect("tClientConfig",VAR_LIST_tClientConfig,0);

}//void ExttClientConfigSelect(void)


void ExttClientConfigSelectRow(void)
{
	ExtSelectRow("tClientConfig",VAR_LIST_tClientConfig,uConfig);

}//void ExttClientConfigSelectRow(void)


void ExttClientConfigListSelect(void)
{
	char cCat[512];

	ExtListSelect("tClientConfig",VAR_LIST_tClientConfig);

	//Changes here must be reflected below in ExttClientConfigListFilter()
        if(!strcmp(gcFilter,"uConfig"))
        {
                sscanf(gcCommand,"%u",&uConfig);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tClientConfig.uConfig=%u \
						ORDER BY uConfig",
						uConfig);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uConfig");
        }

}//void ExttClientConfigListSelect(void)


void ExttClientConfigListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uConfig"))
                printf("<option>uConfig</option>");
        else
                printf("<option selected>uConfig</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttClientConfigListFilter(void)


void ExttClientConfigNavBar(void)
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

}//void ExttClientConfigNavBar(void)


void tClientConfigNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tClientConfig","tClientConfig.uConfig,tClientConfig.cLabel",0);

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
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tClientConfig"
				"&uConfig=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tClientConfigNavList(void)


void ClientServiceConfigList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        sprintf(gcQuery,"SELECT tClientConfig.uConfig,tParameter.cParameter,tClientConfig.cValue FROM "
			"tClientConfig,tParameter,tService WHERE tClientConfig.uParameter=tParameter.uParameter "
			"AND tService.uService=tClientConfig.uService AND tClientConfig.uGroup=%u AND "
			"tClientConfig.uService=%u ORDER BY tParameter.cParameter",uGroup,uService);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
                printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tClientConfig&uConfig=%s"
			"&uInstance=%u&uExtClient=%u>%s=%s</a><br>\n",field[0],uInstance,uExtClient,field[1],field[2]);
        
	mysql_free_result(res);

}//void ClientProductServiceConfigList(void)


unsigned GetInstanceStatusViaJob(unsigned uInstance)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uRetVal=0;

        //If any service of product instance is deployed return deployed.
        sprintf(gcQuery,"SELECT min(tJob.uJobStatus) FROM tJob WHERE uInstance=%u GROUP BY uInstance",uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uRetVal);
        mysql_free_result(res);
	
	return(uRetVal);

}//unsigned GetInstanceStatusViaJob(unsigned uInstance)


unsigned GetInstanceStatus(unsigned uInstance)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uRetVal=0;

        sprintf(gcQuery,"SELECT uStatus FROM tInstance WHERE uInstance=%u",uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uRetVal);
        mysql_free_result(res);
	
	return(uRetVal);

}//unsigned GetInstanceStatus(unsigned uInstance)
