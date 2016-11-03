/*
FILE 
	instance.c
	svn ID removed
AUTHOR
	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	Provides the functions and UI for product instance management.
*/

#include "interface.h"

extern MYSQL MysqlExt;//customer.c
extern unsigned uCustomer; //customer.c
static unsigned uInstance=0;

static struct structtClientConfig InstanceData; //Only 100 parameters supported
static unsigned uParamCount=0;

static char cDay[10]={""};
static char cMonth[10]={""};
static char cYear[10]={""};

structParameter GetParameterInfo(unsigned uClientConfig);
char *cGetValueFromStruct(struct structtClientConfig *data,unsigned uId,unsigned uElementCount);
void Update_tClientConfig(void);
void RedeployInstance(void);
void RemoveInstanceJobs(void);
unsigned uGetInstanceStatus(unsigned uInstance);
char *cGetExtQuery(unsigned uService,unsigned uParameter);
char *cGetcCustomerName(unsigned uRowId);

time_t cDateToUnixTime(char *cDate); //customer.c

void ProcessInstanceVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uCustomer"))
			sscanf(entries[i].val,"%u",&uCustomer);
		else if(!strcmp(entries[i].name,"uInstance"))
			sscanf(entries[i].val,"%u",&uInstance);
		else if(!strncmp(entries[i].name,"cClientConfig.",14))
		{
			unsigned uClientConfig=0;
			sscanf(entries[i].name,"cClientConfig.%u",&uClientConfig);
			InstanceData.uClientConfig[uParamCount]=uClientConfig;
			sprintf(InstanceData.cValue[uParamCount],"%.255s",entries[i].val);
			uParamCount++;
		}
		else if(!strcmp(entries[i].name,"cDay"))
			sprintf(cDay,"%.9s",entries[i].val);
		else if(!strcmp(entries[i].name,"cMonth"))
			sprintf(cMonth,"%.9s",entries[i].val);
		else if(!strcmp(entries[i].name,"cYear"))
			sprintf(cYear,"%.9s",entries[i].val);	
	}
	
}//void ProcessInstanceVars(pentry entries[], int x)


void InstanceGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uCustomer"))
			sscanf(gentries[i].val,"%u",&uCustomer);
		else if(!strcmp(gentries[i].name,"uInstance"))
			sscanf(gentries[i].val,"%u",&uInstance);
	}
	htmlInstance();

}//void InstanceGetHook(entry gentries[],int x)


void InstanceCommands(pentry entries[], int x)
{
	time_t uDeployTime=0;
	
	if(!strcmp(gcPage,"Instance"))
	{
		ProcessInstanceVars(entries,x);
		
		if(cDay[0] && cDay[0]!='N')
		{
			sprintf(gcQuery,"%s-%s-%s",cDay,cMonth,cYear);
			uDeployTime=cDateToUnixTime(gcQuery);
		}
		else
		{
			time(&uDeployTime); // job instance status modifications will happen immediately
		}

		if(!strcmp(gcFunction,"Modify"))
		{
			sprintf(gcModStep,"Confirm ");
			gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
		}
		else if(!strcmp(gcFunction,"Confirm Modify"))
		{
			//Data entry validation 
			Update_tClientConfig();
			RedeployInstance();
		}
		else if(!strcmp(gcFunction,"Cancel Instance"))
		{
			sprintf(gcNewStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Cancel Instance"))
		{
			sprintf(gcQuery,"UPDATE tInstance SET uStatus=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uInstance=%u",
					mysqlISP_WaitingCancel
					,guLoginClient
					,uInstance);
	        	mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			SubmitInstanceJob("Cancel",uDeployTime,uInstance,1);
		}
		else if(!strcmp(gcFunction,"Hold Instance"))
		{
			sprintf(gcNewStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Hold Instance"))
		{
			sprintf(gcQuery,"UPDATE tInstance SET uStatus=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uInstance=%u",
					mysqlISP_WaitingHold
					,guLoginClient
					,uInstance);
	        	mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			 SubmitInstanceJob("Hold",uDeployTime,uInstance,1);
		}
		else if(!strcmp(gcFunction,"Call Off Cancel Job"))
		{
			sprintf(gcNewStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Call Off Cancel Job"))
		{
			//Just return the instance to previous status (Deployed)
			//and remove any jobs pending for it
			RemoveInstanceJobs();
		}
		else if(!strcmp(gcFunction,"Cancel Deployment"))
		{
			sprintf(gcNewStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Cancel Deployment"))
		{
			RemoveInstanceJobs();
		}
		else if(!strcmp(gcFunction,"Cancel Hold"))
		{
			sprintf(gcNewStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Cancel Hold"))
		{
			RemoveInstanceJobs();
		}
		else if(!strcmp(gcFunction,"Cancel Redeployment"))
		{
			sprintf(gcNewStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Cancel Redeployment"))
		{
			RemoveInstanceJobs();
		}
		else if(!strcmp(gcFunction,"Deploy"))
		{
			sprintf(gcNewStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Deploy"))
		{
			//...
		}
			
		else if(!strcmp(gcFunction,"Redeploy Instance") || !strcmp(gcFunction,"Approve Instance"))
		{
			sprintf(gcNewStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Redeploy Instance") || !strcmp(gcFunction,"Confirm Approve Instance"))
		{
			sprintf(gcQuery,"UPDATE tInstance SET uStatus=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uInstance=%u",
					mysqlISP_WaitingRedeployment
					,guLoginClient
					,uInstance);
	        	mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			RedeployInstance();
		}
		else if(!strcmp(gcFunction,"Release Hold"))
		{
			sprintf(gcNewStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Release Hold"))
		{		
			sprintf(gcQuery,"UPDATE tInstance SET uStatus=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uInstance=%u",
					mysqlISP_WaitingRedeployment
					,guLoginClient
					,uInstance);
	        	mysql_query(&gMysql,gcQuery);
			SubmitInstanceJob("RemoveHold",uDeployTime,uInstance,1);
			
		}
		else if(!strcmp(gcFunction,"Remove Instance"))
			sprintf(gcNewStep,"Confirm ");
		else if(!strcmp(gcFunction,"Confirm Remove Instance"))
		{
			sprintf(gcQuery,"DELETE FROM tInstance  WHERE uInstance=%u",uInstance);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));

			sprintf(gcQuery,"DELETE FROM tClientConfig WHERE uGroup=%u",uInstance);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));

			uInstance=0;

		}
		else if(!strcmp(gcFunction,"Hold Instance"))
			sprintf(gcNewStep,"Confirm ");
		htmlInstance();
	}

}//void InstanceCommands(pentry entries[], int x)


void htmlInstance(void)
{
	htmlHeader("unxsISP Admin Interface","Header");
	if(gcNewStep[0])
		htmlInstancePage("","Instance-Step.Body");
	else
		htmlInstancePage("","Instance.Body");
		
	htmlFooter("Footer");

}//void htmlInstance(void)


void htmlInstancePage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect(cTemplateName);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			char cuCustomer[16]={""};
			char cuInstance[16]={""};

			sprintf(cuCustomer,"%u",uCustomer);
			sprintf(cuInstance,"%u",uInstance);

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="ispAdmin.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcLogin;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcMessage";
			template.cpValue[7]=gcMessage;

			template.cpName[8]="uCustomer";
			template.cpValue[8]=cuCustomer;

			template.cpName[9]="uInstance";
			template.cpValue[9]=cuInstance;

			template.cpName[10]="gcModStep";
			template.cpValue[10]=gcModStep;

			template.cpName[11]="cBtnStatus";
			if(uInstance && uCustomer)
				template.cpValue[11]="";
			else
				template.cpValue[11]="disabled";

			template.cpName[12]="cCustomerName";
			template.cpValue[12]=cGetcCustomerName(uCustomer);

			template.cpName[13]="gcDelStep";
			template.cpValue[13]=gcDelStep;

			template.cpName[14]="cInstance";
			template.cpValue[14]=(char *)ForeignKey("tInstance","cLabel",uInstance);

			template.cpName[15]="";

			
			printf("\n<!-- Start htmlInstancePage(%s) gcNewStep=%s-->\n",cTemplateName,gcNewStep); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlInstancePage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlInstancePage()


void funcEditProductParams(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	char cServiceLabel[33]={""};
	char cFieldValue[256]={""};;
	char cParamName[256]={""};;

	unsigned uClientConfig=0;
	unsigned uOnlyOnce=0;
	unsigned uModPostDeploy=0;
	unsigned uStatus=0;

	struct t_template template;
	
	fprintf(fp,"<!-- funcEditProductParams(fp) start -->\n");
	
	uStatus=uGetInstanceStatus(uInstance);

//	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
//Instance data limited by customer select
		sprintf(gcQuery,"SELECT tClientConfig.uConfig,tService.cLabel,tParameter.cISMName,tParameter.cISMHelp,tClientConfig.cValue,"
				"tParameter.uModPostDeploy,tParamType.cLabel,tParameter.cExtQuery,tParameter.uParameter,tService.uService "
				"FROM tClientConfig,tParameter,tService,tParamType WHERE tClientConfig.uParameter=tParameter.uParameter "
				"AND tClientConfig.uGroup=%u AND tService.uService=tClientConfig.uService AND tParameter.uParamType=tParamType.uParamType "
				"AND tClientConfig.uOwner=%u ORDER BY tService.cLabel,tParameter.uISMOrder",
				uInstance
				,uCustomer
				);
/*	else
		sprintf(gcQuery,"SELECT tClientConfig.uConfig,tService.cLabel,tParameter.cISMName,tParameter.cISMHelp,tClientConfig.cValue,"
				"tParameter.uModPostDeploy,tParamType.cLabel,tParameter.cExtQuery,tParameter.uParameter,tService.uService "
				"FROM tClientConfig,tParameter,tService,tParamType,tClient WHERE tClientConfig.uParameter=tParameter.uParameter "
				"AND tClientConfig.uGroup=%1$u AND tService.uService=tClientConfig.uService AND tParameter.uParamType=tParamType.uParamType "
				"AND tClientConfig.uOwner=%2$u AND tClientConfig.uOwner=tClient.uClient AND tClient.uOwner=%3$u "
				"ORDER BY tService.cLabel,tParameter.uISMOrder",
				uInstance
				,uCustomer
				,guOrg
				);*/
	//0: tClientConfig row id
	//1: service label
	//2: field label
	//3: field tool tip text
	//4: field value
	//5: uAllowMod
	//6: param type
	//7: ext query for extselect parameters
	//8: tParameter.cParameter (E.g. mysqlRadius.Login)
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	
	if(!mysql_num_rows(res)) 
	{
		uInstance=0;
		uCustomer=0;
		fprintf(fp,"No instance data available.");
		return;
	}

	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[5],"%u",&uModPostDeploy);

		if(strcmp(field[1],cServiceLabel))
		{
			if(uOnlyOnce)
			{
				template.cpName[0]="";
				fpTemplate(fp,"ParameterInputFooter",&template);
			}
			sprintf(cServiceLabel,"%.32s",field[1]);
			uOnlyOnce=1;
			template.cpName[0]="cServiceLabel";
			template.cpValue[0]=field[1];
			template.cpName[1]="";
			
			fpTemplate(fp,"ParameterInputHeader",&template);
		}
		
		template.cpName[0]="cParamLabel";
		template.cpValue[0]=field[2];

		sprintf(cParamName,"cClientConfig.%s",field[0]);
		template.cpName[1]="cParamName";
		template.cpValue[1]=cParamName;

		template.cpName[2]="cParamTitle";
		template.cpValue[2]=field[3];

		template.cpName[3]="cParamMaxLength";
		template.cpValue[3]="99"; //TODO: hardcoded!!
		
		template.cpName[4]="cParamValue";

		if(uParamCount && strcmp(gcFunction,"Confirm Modify"))
		{
			sscanf(field[0],"%u",&uClientConfig);

			sprintf(cFieldValue,"%s",cGetValueFromStruct(&InstanceData,uClientConfig,uParamCount));
			if(cFieldValue!=NULL)
				template.cpValue[4]=cFieldValue;
			else
				template.cpValue[4]=field[4];
		}
		else
			template.cpValue[4]=field[4];
		
		template.cpName[5]="cParamStyle";
		if(gcModStep[0] && (uModPostDeploy || (uStatus==mysqlISP_Canceled) || (uStatus==mysqlISP_WaitingApproval)))
			template.cpValue[5]="type_fields"; //TODO: hardcoded will change based on validation code
		else if(gcModStep[0] && !uModPostDeploy)
			template.cpValue[5]="type_fields_off";
		else if(!gcModStep[0])
			template.cpValue[5]="type_fields_off";

		template.cpName[6]="cFieldStatus";
		
		if(gcModStep[0] && (uModPostDeploy || (uStatus==mysqlISP_Canceled) || (uStatus==mysqlISP_WaitingApproval)))
			template.cpValue[6]="";
		else
			template.cpValue[6]="disabled";
		
		template.cpName[7]="";
		
		if(strcmp(field[6],"extselect"))
		{
			fpTemplate(fp,"InstanceParameterInputField",&template);
		}
		else
		{
		//extselect drop-down entry parameters
			char *cp;
			MYSQL_RES *res4;
			MYSQL_ROW field4;
			if((cp=strchr(field[7],':')))
			{
				char cConfQuery[256];
				char cIp[256]={""};
				char cDb[256]={""};
				char cPwd[256]={""};
				char cLogin[256]={""};
				unsigned uService=0;
				unsigned uParameter=0;


				*cp=0;

				sprintf(cConfQuery,"%.200sIp",field[7]);
				GetConfiguration(cConfQuery,cIp);
				sprintf(cConfQuery,"%.200sName",field[7]);
				GetConfiguration(cConfQuery,cDb);
				sprintf(cConfQuery,"%.200sPwd",field[7]);
				GetConfiguration(cConfQuery,cPwd);
				sprintf(cConfQuery,"%.200sLogin",field[7]);
				GetConfiguration(cConfQuery,cLogin);
	
				//Must add tConfiguration parameters	
				//debug only	
				//sprintf(gcQuery,"cExtVerify:%s:\n cConfQuery:%s:\n cIP:%s\n cDb:%s\n cPwd:%s\n cLogin:%s\n Verify query:(%s)",cExtVerify,cConfQuery,cIp,cDb,cPwd,cLogin,cp+1);
				//htmlPlainTextError(gcQuery);	
				if(cDb[0]==0 || cPwd[0]==0 || cLogin[0]==0)
				{
					fprintf(fp,"<tr><td>Can't display parameter input (%s). Configuration error</td></tr>",field[2]);
					continue;
				}
		
				ExtConnectDb(cDb,cIp,cPwd,cLogin);

				sscanf(field[8],"%u",&uParameter);
				sscanf(field[9],"%u",&uService);


				sprintf(gcQuery,"%s",cGetExtQuery(uService,uParameter));
				//debug only
				//htmlPlainTextError(gcQuery);	
				
				mysql_query(&MysqlExt,gcQuery);
       				if(mysql_errno(&MysqlExt))
               				htmlPlainTextError(mysql_error(&MysqlExt));
					
				res4=mysql_store_result(&MysqlExt);
				fpTemplate(fp,"InstParamInputSelHeader",&template);	
				//Show select options
				while((field4=mysql_fetch_row(res4)))
				{
					fprintf(fp,"<option ");
					if(uParamCount)
					{
						if(!strcmp(cFieldValue,field4[0]))
							fprintf(fp,"selected");
					}
					else
					{
						if(!strcmp(field[4],field4[0]))
							fprintf(fp,"selected");
					}
					fprintf(fp,">%s</option>\n",field4[0]);
					//fprintf(fp,"<!-- field[4]='%s' field4[0]='%s' -->\n",field[4],field4[0]);

				}

				template.cpName[0]="";
				fpTemplate(fp,"ParameterInputSelectFooter",&template);
			}
		}
	}
	template.cpName[0]="gcModStep";
	template.cpValue[0]=gcModStep;

	template.cpName[1]="";
	fpTemplate(fp,"ParameterInputFooter",&template);

			
	fprintf(fp,"<!-- funcEditProductParams(fp) end -->\n");
	
}//void funcEditProductParams(FILE *fp)


char *cGetExtQuery(unsigned uService,unsigned uParameter)
{
	static char cRet[513]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cValue FROM tConfig WHERE uGroup=%u AND uParameter=%u",uService,uParameter);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sprintf(cRet,"%.512s",field[0]);
	
	mysql_free_result(res);

	return(cRet);

}//char *cGetExtQuery(unsigned uService,unsigned uParameter)


char *cGetValueFromStruct(struct structtClientConfig *data,unsigned uId,unsigned uElementCount)
{
	register unsigned uCount=0;
	
	for(uCount=0;uCount<uElementCount;uCount++)
		if(data->uClientConfig[uCount]==uId)
			return(data->cValue[uCount]);
	return(NULL);
		
}//char *cGetValueFromStruct(struct structtClientConfig *data,unsigned uId,unsigned uElementCount)


void Update_tClientConfig(void)
{
	register unsigned uCount=0;
	structParameter ParamInfo;
	unsigned uErr=0;
	char cErrMsg[100]={""};

	for(uCount=0;uCount<uParamCount;uCount++)
	{
		//Validate before anything!
		ParamInfo=GetParameterInfo(InstanceData.uClientConfig[uCount]);
//ddddd
//int CheckParameterValue(char *cValue, char *cParamType, char *cRange, unsigned uUnique,
//                char *cExtVerify, char *cParameter, unsigned uDeployed)
		uErr=CheckParameterValue(InstanceData.cValue[uCount],ParamInfo.cParamType,ParamInfo.cRange,
			ParamInfo.uUnique,ParamInfo.cExtVerify,ParamInfo.cParameter,1);
		switch(uErr)
		{
			case 0:
				//No errors, update tClientConfig
				sprintf(gcQuery,"UPDATE tClientConfig SET cValue='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uConfig='%u'",
						InstanceData.cValue[uCount]
						,guLoginClient
						,InstanceData.uClientConfig[uCount]
						);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
			break;
			case 1:
			case 2:
			case 3:
			case 5:
				//Basic error reporting, no highleted fields nor message for now
				sprintf(cErrMsg,"<blink>Error: </blink> Review your entered data. (uErr=%u (uCount=%u ParamInfo.cRange=%s "
						"ParamInfo.cParameter=%s InstanceData.cValue[uCount]=%s)",
						uErr,uCount,ParamInfo.cRange,ParamInfo.cParameter,InstanceData.cValue[uCount]);
				//gcMessage="<blink>Review your entered data</blink>";
				gcMessage=cErrMsg;
				sprintf(gcModStep,"Confirm ");
				htmlInstance();
		}
	}
	
}//void Update_tClientConfig(void)


void RedeployInstance(void)
{
	time_t luClock;
	char *cJobName="";
	unsigned uStatus=0;

	time(&luClock);

	uStatus=uGetInstanceStatus(uInstance);

	if(uStatus==mysqlISP_WaitingInitial)
	{
		//If waiting for initial deployment, remove previous job(s) (more than one related to the same instance??)
		//and submit a new 'New' job (No instance status update required right?)
		//
		sprintf(gcQuery,"DELETE FROM tJob WHERE uInstance='%u' AND uJobClient='%u'",uInstance,uCustomer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}

	//Set cJobName based on instance status.
	switch(uStatus)
	{
		case mysqlISP_NeverDeployed:
		case mysqlISP_Canceled:
		case mysqlISP_Invalid:
		case mysqlISP_WaitingInitial:
		case mysqlISP_WaitingRedeployment:
			cJobName="New";
		break;

		case mysqlISP_DeployedMod:
		case mysqlISP_OnHold:
		case mysqlISP_Deployed:
			cJobName="Mod";
		break;
	}

	//Change status to appropiate waiting status
	if(uStatus==mysqlISP_NeverDeployed || uStatus==mysqlISP_Canceled)
		uStatus=mysqlISP_WaitingInitial;
	else
		uStatus=mysqlISP_WaitingRedeployment;
	
	sprintf(gcQuery,"UPDATE tInstance SET uStatus='%u' WHERE uInstance='%u' AND uClient='%u'",
			uStatus
			,uInstance
			,uCustomer
	       );
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	SubmitInstanceJob(cJobName,luClock,uInstance,1);

}//void RedeployInstance(void)

unsigned uGetInstanceStatusViaJob(unsigned uInstance)
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

}//unsigned uGetInstanceStatusViaJob(unsigned uInstance)


unsigned uGetInstanceStatus(unsigned uInstance)
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

}//unsigned uGetInstanceStatus(unsigned uInstance)


void funcInstanceOperations(FILE *fp)
{
	char *cTemplate="";
	char *cCancelStyle="style='width:190px; font-family:verdana, sans-serif; font-size:.97em;'";
	char *cHoldStyle="style='width:190px; font-family:verdana, sans-serif; font-size:.97em;'";


	struct t_template template;
	
	//Based on the loaded instance status, we will display different buttons and stuff
	//at the left panel.
	//This function handles all this.
	
	
	if(!uInstance || !uCustomer) return;

	switch(uGetInstanceStatus(uInstance))
	{
		case mysqlISP_NeverDeployed:
			cTemplate="InstanceOpDeploy";
		break;
		
		case mysqlISP_WaitingInitial:
			cTemplate="InstanceOpCancelDeployment";
		break;
		
		case mysqlISP_DeployedMod:
		case mysqlISP_Deployed:
			cTemplate="InstanceOpCancelAndHold";
		break;
		
		case mysqlISP_OnHold:
			cTemplate="InstanceOpRelease";

		break;
		
		case mysqlISP_Canceled:
		case mysqlISP_Invalid:
			cTemplate="InstanceOpRedeploy";
		break;

		case mysqlISP_WaitingRedeployment:
			cTemplate="InstanceOpCancelRedeploy";
		break;
		
		case mysqlISP_WaitingCancel:
			cTemplate="InstanceOpCancelCancel";
		break;
		
		case mysqlISP_WaitingHold:
			cTemplate="InstanceOpCancelHold";
		break;

		case mysqlISP_WaitingApproval:
			cTemplate="InstanceOpApprove";
		break;
		
		default:
			cTemplate="InstanceOpUnknown";
	
	}//switch(uGetInstanceStatus(uInstance))

	//Ticket #1312 workaround
	if(!strcmp(gcFunction,"Hold Instance") || !strcmp(gcFunction,"Redeploy Instance"))
		cCancelStyle="style='visibility: hidden;'";
	else if(!strcmp(gcFunction,"Cancel Instance") || !strcmp(gcFunction,"Remove Instance"))
		cHoldStyle="style='visibility: hidden;'";

	template.cpName[0]="gcNewStep";
	template.cpValue[0]=gcNewStep;

	template.cpName[1]="cCancelStyle";
	template.cpValue[1]=cCancelStyle;

	template.cpName[2]="cHoldStyle";
	template.cpValue[2]=cHoldStyle;

	template.cpName[3]="";

	fpTemplate(fp,cTemplate,&template);
				 
}//void funcInstanceOperations(FILE *fp)



structParameter GetParameterInfo(unsigned uClientConfig)
{
	//Using the uClientConfig value we will get information about the parameter and
	//return it for CheckParameters()
	
	static structParameter ReturnStruct;
	unsigned uI=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

/*
typedef struct {
       char cParameter[65];
        char cParamType[33];
        char cRange[17];
        char cExtVerify[256];
        unsigned uUnique;
        unsigned uAllowMod;
        unsigned uModPostDeploy;
} structParameter;
*/
	sprintf(gcQuery,"SELECT tParameter.cParameter,tParamType.cLabel,tParameter.cRange,tParameter.cExtVerify,tParameter.uUnique,tParameter.uAllowMod,tParameter.uModPostDeploy FROM tParameter,tParamType,tClientConfig WHERE tParamType.uParamType=tParameter.uParamType ANd tParameter.uParameter=tClientConfig.uParameter aND tClientConfig.uConfig='%u'",uClientConfig);

	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
	{
		sprintf(ReturnStruct.cParameter,"%.64s",field[0]);
		sprintf(ReturnStruct.cParamType,"%.32s",field[1]);
		sprintf(ReturnStruct.cRange,"%.16s",field[2]);
		sprintf(ReturnStruct.cExtVerify,"%.255s",field[3]);
		sscanf(field[4],"%u",&uI);
		ReturnStruct.uUnique=uI;
		sscanf(field[5],"%u",&uI);
		ReturnStruct.uAllowMod=uI;
		sscanf(field[6],"%u",&uI);
		ReturnStruct.uModPostDeploy=uI;
	}

	mysql_free_result(res);

	return(ReturnStruct);
}//struct structParameter GetParameterInfo(unsigned uClientConfig)


void RemoveInstanceJobs(void)
{
	//Simply remove all jobs that are waiting for the loaded instance. Wont delete historical data though ;)
	//This function is called by all 'Cancel' operations, do not confuse with 'Cancel Instance'
	
	sprintf(gcQuery,"DELETE FROM tJob WHERE uInstance='%u' AND (uJobStatus='%u' OR uJobStatus='%u' OR uJobStatus='%u' OR uJobStatus='%u')",
			uInstance
			,mysqlISP_WaitingInitial 
			,mysqlISP_WaitingRedeployment 
			,mysqlISP_WaitingCancel 
			,mysqlISP_WaitingHold 
		);
}//void RemoveInstanceJobs(void)


char *cGetcCustomerName(unsigned uRowId)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	static char cCustomerName[100]={""};

	sprintf(gcQuery,"SELECT cLabel FROM tClient WHERE uClient=%u",uRowId);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sprintf(cCustomerName,"%.99s",field[0]);
	
	mysql_free_result(res);
	
	return(cCustomerName);

}//char *cGetcCustomerName(unsigned uCustomer)

