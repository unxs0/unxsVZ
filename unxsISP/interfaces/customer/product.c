/*
FILE 
	product.c
	svn ID removed
AUTHOR
	(C) 2007-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	unxsISP Admin (Owner) Interface
	program file.
*/


#include "interface.h"
unsigned uProduct=0;
static unsigned uStep=0;

unsigned uServices[100]={0};
unsigned uServiceStartPoint[100]={0};
unsigned uServiceEndPoint[100]={0};
char cParameterInput[100][100]={""}; //tClientConfig.cValue
static char cParameterLabel[100][100]={""}; //tParameter.cISMName
char cParameterName[100][100]={""};  //tParameter.cParameter
static char *cParameterStyle[100];

unsigned uParameterCount=0;
unsigned uServicesCount=0;
static unsigned uPrevStatus=0;
MYSQL MysqlExt;
static char cDay[10]={""};
static char cMonth[10]={""};
static char cYear[10]={""};
void htmlProductDeployWizard(unsigned uStep);
char *cGetServiceLabel(unsigned uService);
char *cGetProductLabel(unsigned uProduct);
unsigned uGetParameter(char *cParameter);
void ProcessProductDeploymentVars(pentry entries[], int x);
void DeployProduct(void);
void SubmitSingleServiceJob(unsigned uService,unsigned uInstance,char *cJobType,unsigned uExtClient,time_t uJobDate);
void SubmitServiceJob(unsigned uService,char *cJobName, char *cServer,unsigned uInstance,time_t uJobDate);
void GetConfiguration(const char *cName, char *cValue);
void SetParamFieldsOn(void);
unsigned uProductHasServices(unsigned uProduct);
time_t cDateToUnixTime(char *cDate);
void CheckAllParameters(void);
unsigned uIsParameterInService(unsigned uService,char *cParamName);


void ProcessProductDeploymentVars(pentry entries[], int x)
{
	register int i;
	unsigned uIndex=0;

	for(i=0;i<x;i++)
	{
		if(!strncmp(entries[i].name,"cParam.",7))
		{
			sscanf(entries[i].name,"cParam.%u",&uIndex);
			sprintf(cParameterInput[uIndex],"%.99s",entries[i].val);
			uParameterCount++;
		}
		else if(!strncmp(entries[i].name,"cParamLabel.",12))
		{
			sscanf(entries[i].name,"cParamLabel.%u",&uIndex);
			sprintf(cParameterLabel[uIndex],"%.99s",entries[i].val);
		}
		else if(!strncmp(entries[i].name,"cParamName.",11))
		{
			sscanf(entries[i].name,"cParamName.%u",&uIndex);
			sprintf(cParameterName[uIndex],"%.99s",entries[i].val);
		}
		else if(!strncmp(entries[i].name,"uService.",9))
		{
			sscanf(entries[i].name,"uService.%u",&uIndex);
			sscanf(entries[i].val,"%u",&uServices[uIndex]);
			uServiceStartPoint[uIndex]=uParameterCount;
			if(uIndex) uServiceEndPoint[uIndex-1]=uParameterCount;
			uServicesCount++;
		}
		else if(!strcmp(entries[i].name,"uStep"))
			sscanf(entries[i].val,"%u",&uStep);
		else if(!strcmp(entries[i].name,"uProduct"))
			sscanf(entries[i].val,"%u",&uProduct);
	}
	if(uServicesCount) uServiceEndPoint[uServicesCount-1]=uParameterCount;

}//void ProcessProductDeploymentVars(pentry entries[], int x)


void htmlProductDeployWizard(unsigned uStep)
{
	htmlHeader("unxsISP Customer Interface","Header");
	sprintf(gcQuery,"CustomerProductDeploy.%u",uStep);
	htmlProductPage("",gcQuery);
	htmlFooter("Footer");

}//void htmlProductDeployWizard(unsigned uStep)


void ProductCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Product"))
	{
		ProcessProductDeploymentVars(entries,x);
		
		if(!strcmp(gcFunction,"Next") 
		|| !strcmp(gcFunction,"Siguiente"))
		{
			if((uStep+1)==2 && !uProductHasServices(uProduct))
			{
				gcMessage="<blink>Error: </blink>The selected product has no services configured. Can't continue.";
				htmlProductDeployWizard(uStep);
			}
			
			SetParamFieldsOn();
			if(uStep==2)
			{
				CheckAllParameters();
			}

			uStep++;
			if(uStep>3) uStep=3; //little sanity check

			htmlProductDeployWizard(uStep);
		}
		else if(!strcmp(gcFunction,"Back")
		|| !strcmp(gcFunction,"Volver"))
		{
			uStep--;
			if(uStep==0) uStep=1; //little sanity check
			SetParamFieldsOn();
			htmlProductDeployWizard(uStep);
		}
		else if(!strcmp(gcFunction,"Finish")
		|| !strcmp(gcFunction,"Terminar"))
		{
			//End of the product deployment wizard.
			//deploy the product and go back to customers screen.
			DeployProduct();
			gcMessage="Product deployed and waiting for activation.";
			htmlCustomer();
		}
		htmlCustomer();
	}
}


void htmlProductPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect(cTemplateName,guTemplateSet);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			char cuProduct[16]={""};

			sprintf(cuProduct,"%u",uProduct);

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="ispClient.cgi";
			
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

			template.cpName[8]="gcInputStatus";
			template.cpValue[8]=gcInputStatus;
				
			template.cpName[9]="uProduct";
			template.cpValue[9]=cuProduct;

			template.cpName[10]="cProductLabel";
			template.cpValue[10]=cGetProductLabel(uProduct);

			template.cpName[11]="";

			printf("\n<!-- Start htmlProductPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlProductPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlProductPage()


unsigned uProductHasServices(unsigned uProduct)
{
	MYSQL_RES *res;
	sprintf(gcQuery,"SELECT tService.uService,tService.cLabel uService FROM "
			"tService,tServiceGlue WHERE tServiceGlue.uServiceGroup=%u "
			"AND tService.uService=tServiceGlue.uService AND "
			"tService.uAvailable=1 ORDER BY tService.cLabel",uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	return((unsigned)mysql_num_rows(res));
	
}//unsigned uProductHasServices(unsigned uProduct)


void funcProductParameterInput(FILE *fp)
{
	//
	//This function will output a table for the product parameters separated by service.
	//As a product can be made of more than one services and each service has its own parameters.
	MYSQL_RES *res;
	MYSQL_ROW field;

	MYSQL_RES *res2;
	MYSQL_ROW field2;

	struct t_template template;

	unsigned uServiceCount=0;
	unsigned uParamCount=0;
	unsigned uISMHide=0;

	char cParamName[100]={""};
	char cuParamCount[10]={""};

//from tproductfunc.h AddService:
//sprintf(gcQuery,"INSERT INTO tServiceGlue SET uServiceGroup=%u,uService=%u",uProduct,uService);
	
	fprintf(fp,"<!--funcProductParameterInput(fp) start -->\n");

	sprintf(gcQuery,"SELECT tService.uService,tService.cLabel uService FROM tService,tServiceGlue WHERE tServiceGlue.uServiceGroup=%u AND "
			"tService.uService=tServiceGlue.uService AND tService.uAvailable=1 ORDER BY tService.cLabel",uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		template.cpName[0]="cServiceLabel";
		template.cpValue[0]=field[1];
			
		template.cpName[1]="uService";
		template.cpValue[1]=field[0];

		sprintf(gcQuery,"%u",uServiceCount);
		template.cpName[2]="uServiceCount";
		template.cpValue[2]=gcQuery;

		template.cpName[3]="";
		
		fpTemplate(fp,"ParameterInputHeader",&template);
//from tservicefunc.h
//sprintf(gcQuery,"INSERT INTO tConfig SET uConfig=0,uGroup=%u,uParameter=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
//                                     uService,
//                                     uParameter,
		sprintf(gcQuery,"SELECT tParameter.cParameter,tParameter.cISMName,cISMHelp,tParamType.cLabel,"
				"tParameter.cExtQuery,tConfig.cValue,tParameter.uISMHide FROM "
				"tParameter,tConfig,tParamType WHERE tConfig.uGroup=%s AND tConfig.uParameter=tParameter.uParameter AND "
				"tParamType.uParamType=tParameter.uParamType ORDER BY tParameter.uISMOrder",
				field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res2=mysql_store_result(&gMysql);

		while((field2=mysql_fetch_row(res2)))
		{
			sprintf(cParamName,"cParam.%u",uParamCount);
			sscanf(field2[6],"%u",&uISMHide);

			template.cpName[0]="cParamLabel";
			template.cpValue[0]=field2[1];
			
			template.cpName[1]="cParamTitle";
			template.cpValue[1]=field2[2];

			template.cpName[2]="cParamName";
			template.cpValue[2]=cParamName;

			template.cpName[3]="cParamMaxLength";
			template.cpValue[3]="64"; //TODO: read cRange and decide based on uParamType

			template.cpName[4]="cParamValue";
			if(cParameterInput[uParamCount][0])
				template.cpValue[4]=cParameterInput[uParamCount];
			else
				template.cpValue[4]=field2[5];

			sprintf(cuParamCount,"%u",uParamCount);
			template.cpName[5]="uParamCount";
			template.cpValue[5]=cuParamCount;
			
			template.cpName[6]="cParameter";
			template.cpValue[6]=field2[0];

			template.cpName[7]="cParamStyle";
			template.cpValue[7]=cParameterStyle[uParamCount];

			template.cpName[8]="";
			
			if(strcmp(field2[3],"extselect"))
			{
				if(uISMHide)
					fpTemplate(fp,"ParameterHiddenInputField",&template);
				else
					fpTemplate(fp,"ParameterInputField",&template);
			}
			else
			{
			//extselect drop-down entry parameters
				char *cp;
				MYSQL_RES *res4;
				MYSQL_ROW field4;
				
				if((cp=strchr(field2[4],':')))
				{
					char cConfQuery[256];
					char cIp[256]={""};
					char cDb[256]={""};
					char cPwd[256]={""};
					char cLogin[256]={""};

					*cp=0;

					sprintf(cConfQuery,"%.200sIp",field2[4]);
					GetConfiguration(cConfQuery,cIp);
					sprintf(cConfQuery,"%.200sName",field2[4]);
					GetConfiguration(cConfQuery,cDb);
					sprintf(cConfQuery,"%.200sPwd",field2[4]);
					GetConfiguration(cConfQuery,cPwd);
					sprintf(cConfQuery,"%.200sLogin",field2[4]);
					GetConfiguration(cConfQuery,cLogin);
		
					//Must add tConfiguration parameters	
					//debug only	
					//sprintf(gcQuery,"cIP:%s\n cDb:%s\n cPwd:%s\n cLogin:%s\n",
					//cIp,cDb,cPwd,cLogin);
					//htmlPlainTextError(gcQuery);	
					if(cDb[0]==0 || cPwd[0]==0 || cLogin[0]==0)
					{
						fprintf(fp,"Can't display parameter input (%s). Configuration error",field2[1]);
						continue;
					}
		
					ExtConnectDb(cDb,cIp,cPwd,cLogin);

					sprintf(gcQuery,"%s",field2[5]);
						//debug only
					//htmlPlainTextError(gcQuery);	
					mysql_query(&MysqlExt,gcQuery);
        				if(mysql_errno(&MysqlExt))
                				htmlPlainTextError(mysql_error(&MysqlExt));
					
					res4=mysql_store_result(&MysqlExt);
					fpTemplate(fp,"ParameterInputSelectHeader",&template);	

					//Show select options
					while((field4=mysql_fetch_row(res4)))
					{
						fprintf(fp,"<option ");
						if(!strcmp(cParameterInput[uParamCount],field4[0]))
							fprintf(fp,"selected");
						fprintf(fp,">%s</option>\n",field4[0]);
					}

					template.cpName[0]="";
					fpTemplate(fp,"ParameterInputSelectFooter",&template);
				}

			}//end extselect drop-down entry parameters
			uParamCount++;
		}//Parameters loop

		template.cpName[0]="";
		fpTemplate(fp,"ParameterInputFooterWizard",&template);
		uServiceCount++;
		
	}//Services loop
	
	fprintf(fp,"<input type=hidden name=uParamCount value=%u>\n",uParamCount);
	fprintf(fp,"<!--funcProductParameterInput(fp) end -->\n");
	
}//void funcProductParameterInput(FILE *fp)


void funcEnteredParameters(FILE *fp)
{
	register int i=0,n=0;
	struct t_template template;
	char cuService[16]={""};

	fprintf(fp,"<!-- funcEnteredParameters(fp) start -->\n");
	
	i=0;
	for(n=0;n<uServicesCount;n++)
	{
		template.cpName[0]="cServiceLabel";
		template.cpValue[0]=cGetServiceLabel(uServices[n]);
		
		sprintf(cuService,"%u",uServices[n]);
		template.cpName[1]="uService";
		template.cpValue[1]=cuService;

		sprintf(gcQuery,"%i",n);
		template.cpName[2]="uServiceCount";
		template.cpValue[2]=gcQuery;

		template.cpName[3]="";
		fpTemplate(fp,"EnteredParametersHeader",&template);
		
		for(i=uServiceStartPoint[n];i<uServiceEndPoint[n];i++)
		{
			
			template.cpName[0]="cParamLabel";
			template.cpValue[0]=cParameterLabel[i];
		
			template.cpName[1]="cParamValue";
			template.cpValue[1]=cParameterInput[i];
			
			template.cpName[2]="cParameter";
			template.cpValue[2]=cParameterName[i];

			sprintf(gcQuery,"%i",i);
			template.cpName[3]="iParamIndex";
			template.cpValue[3]=gcQuery;
		
			template.cpName[4]="";
			fpTemplate(fp,"EnteredParametersItem",&template);
		}//Parameters loop
		
		template.cpName[0]="";	
		fpTemplate(fp,"EnteredParametersFooter",&template);

	}//Services loop
	
	fprintf(fp,"<!-- funcEnteredParameters(fp) end -->\n");
	
}//void funcEnteredParameters(FILE *fp)


unsigned uIsParameterInService(unsigned uService,char *cParamName)
{
	MYSQL_RES *res;
	
	if(!cParamName[0]) return(0);
	sprintf(gcQuery,"SELECT tConfig.uConfig FROM tConfig,tService,tParameter WHERE tParameter.uParameter=tConfig.uParameter AND tService.uService=tConfig.uGroup AND tParameter.cParameter='%s' AND tService.uService='%u'",cParamName,uService);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	return(unsigned)mysql_num_rows(res);

}//unsigned uIsParameterInService(unsigned uService,char *cParamName)


char *cGetServiceLabel(unsigned uService)
{
	static char cLabel[33]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel FROM tService WHERE uService='%u'",uService);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sprintf(cLabel,"%.32s",field[0]);

	mysql_free_result(res);

	return(cLabel);

}//char *cGetServiceLabel(unsigned uService)


char *cGetProductLabel(unsigned uProduct)
{
	static char cLabel[33]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel FROM tProduct WHERE uProduct='%u'",uProduct);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sprintf(cLabel,"%.32s",field[0]);

	mysql_free_result(res);

	return(cLabel);

}//char *cGetProductLabel(unsigned uProduct)


unsigned uGetParameter(char *cParameter)
{
	unsigned uParmeter;
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uParameter FROM tParameter WHERE cParameter='%s'",cParameter);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uParmeter);

	mysql_free_result(res);

	return(uParmeter);
	
}//unsigned uGetParameter(char *cParameter)


void DeployProduct(void)
{
	unsigned uGroup=0;
	register int i=0,n=0;
	time_t luClock;
	time_t uDeployTime;	

	time(&luClock);
	
	sprintf(gcQuery,"INSERT INTO tInstance SET uClient='%u',uProduct='%u',uOwner='%u',uCreatedBy='%u',uCreatedDate=UNIX_TIMESTAMP(NOW()),uStatus='%u',cLabel='%s'",
			guLoginClient
			,uProduct
			,guLoginClient
			,guLoginClient
			,mysqlISP_WaitingInitial
			,cGetProductLabel(uProduct)
			);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	uGroup=mysql_insert_id(&gMysql);
	
	for(n=0;n<uServicesCount;n++)
	{
		for(i=uServiceStartPoint[n];i<uServiceEndPoint[n];i++)
		{
			sprintf(gcQuery,"INSERT INTO tClientConfig SET uParameter='%u',cValue='%s',uGroup='%u',uService='%u',uOwner='%u',uCreatedBy='%u',uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uGetParameter(cParameterName[i])
					,cParameterInput[i]
					,uGroup
					,uServices[n]
					,guLoginClient
					,guLoginClient
			      		);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
						
		}//Parameters loop
	}//Services loop
	
	if(cDay[0] && cDay[0]!='N')
	{
		sprintf(gcQuery,"%s-%s-%s",cDay,cMonth,cYear);
		uDeployTime=cDateToUnixTime(gcQuery);
	}
	else
	{
		uDeployTime=luClock;
	}
	
	SubmitInstanceJob("New",uDeployTime,uGroup,1);
	
}//void DeployProduct(void)


void ReaduPrevStatus(unsigned uInstance)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char *cp;

        sprintf(gcQuery,"SELECT cJobData FROM tJob WHERE uInstance=%u AND uJobStatus=%u"
						,uInstance,mysqlISP_Waiting);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		if((cp=strstr(field[0],"uPrevStatus=")))
			sscanf(cp+12,"%u",&uPrevStatus);
	}
	mysql_free_result(res);

}//void ReaduPrevStatus(void)


//tProductType.txt dependent define
#define mysqlISP_ProductType_BILLINGONLY 5
void SubmitInstanceJob(char *cJobType,time_t uJobDate,unsigned uInstance,unsigned uExternalCall)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uService=0;
	unsigned uJob=0;
	unsigned uJobGroup=0;
	unsigned uProductType=0;
	unsigned uFirst=1;
	char cJobName[100];
	unsigned uProduct=0;

	//printf("Content-type:text/plain\n\nSubmitInstanceJob(%s,%lu,%u)",cJobType,uJobDate,uInstance);exit(0);
	//Do not submit jobs for products of billing type
	
	if(!uExternalCall)
	        sprintf(gcQuery,"SELECT uProductType FROM tProduct WHERE uProduct=%u",uProduct);
	else
		sprintf(gcQuery,"SELECT tProduct.uProduct,tProduct.uProductType FROM tProduct,tInstance WHERE tProduct.uProduct=tInstance.uProduct AND tInstance.uInstance='%u'",uInstance);
	
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		if(uExternalCall)
		{
			sscanf(field[0],"%u",&uProduct);
			sscanf(field[1],"%u",&uProductType);
		}
		else
			sscanf(field[0],"%u",&uProductType);
	}
	mysql_free_result(res);

	if(uProductType==mysqlISP_ProductType_BILLINGONLY)
		return;

	ReaduPrevStatus(uInstance);

	//Remove all waiting jobs of same product instance
        sprintf(gcQuery,"DELETE FROM tJob WHERE uInstance=%u AND uJobStatus=%u"
						,uInstance,mysqlISP_Waiting);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));

	//One job for each service that comprise the product
        sprintf(gcQuery,"SELECT tService.uService,tService.cJobName,tService.cServer FROM tService,tServiceGlue WHERE tServiceGlue.uService=tService.uService AND tServiceGlue.uServiceGroup=%u",uProduct);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);

        while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uService);
		sprintf(cJobName,"%.81s.%.18s",field[1],cJobType);
		if(strchr(field[2],';'))
		{
			//More that one server in tService.cServer. Wil submit one job per each
			char *cServer;
			
			cServer=strtok(field[2],";");
			//First server in list:
			SubmitServiceJob(uService,cJobName,cServer,uInstance,uJobDate);
			while((cServer=strtok(NULL,";"))!=NULL)
				SubmitServiceJob(uService,cJobName,cServer,uInstance,uJobDate);
		}
		else
			//Single server service
			SubmitServiceJob(uService,cJobName,field[2],uInstance,uJobDate);

		uJob=mysql_insert_id(&gMysql);
		if(uFirst)
		{
			uFirst=0;
			uJobGroup=uJob;
		}
		sprintf(gcQuery,"UPDATE tJob SET uJobGroup=%u WHERE uJob=%u",uJobGroup
				,uJob);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

}//void SubmitInstanceJob(char *cJobType)


void SubmitSingleServiceJob(unsigned uService,unsigned uInstance,char *cJobType,unsigned uExtClient,time_t uJobDate)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cJobName[100];
	unsigned uJob=0;
	
	//One job for specified service 
        sprintf(gcQuery,"SELECT cJobName,cServer FROM tService WHERE uService=%u",
						uService);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
	
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sprintf(cJobName,"%.81s.%.18s",field[0],cJobType);

		//Remove all waiting jobs of same product instance and service by 
		//jobname: Study this further
sprintf(gcQuery,"DELETE FROM tJob WHERE uInstance=%u AND uJobStatus=%u AND cJobName='%s'",uInstance,mysqlISP_Waiting,cJobName);
		mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		guLoginClient=uExtClient;
		SubmitServiceJob(uService,cJobName,field[1],uInstance,uJobDate);

		uJob=mysql_insert_id(&gMysql);
		sprintf(gcQuery,"UPDATE tJob SET uJobGroup=%u WHERE uJob=%u",uJob,uJob);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

}//void SubmitSingleServiceJob(unsigned uService,char *cJobType)


void SubmitServiceJob(unsigned uService,char *cJobName, char *cServer,unsigned uInstance,time_t uJobDate)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cJob[2048]={""};
	char cLine[256]={""};
	char cPrevStatus[16]={""};
	time_t luClock;

        sprintf(gcQuery,"SELECT tParameter.cParameter,tClientConfig.cValue FROM tClientConfig,tParameter WHERE tClientConfig.uParameter=tParameter.uParameter AND tClientConfig.uGroup=%u AND tClientConfig.uService=%u ORDER BY tParameter.cParameter",uInstance,uService);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{
		sprintf(cLine,"%.125s=%.125s\n",field[0],field[1]);
		if(strlen(cJob)+strlen(cLine)<1980)
			strcat(cJob,cLine);
		else
                	htmlPlainTextError("cJob length exceeded. SubmitInstanceJob()");
	}
	mysql_free_result(res);

	//New for ISP wide uISPClient
	sprintf(cLine,"uISPClient=%u\ncClientName=%s",
					guLoginClient,
					ForeignKey("tClient","cLabel",guLoginClient));
	strcat(cJob,cLine);
	
	if(strstr(cJobName,".Hold") || strstr(cJobName,".Cancel"))
	{
		sprintf(cPrevStatus,"\nuPrevStatus=%u",uPrevStatus);
		strcat(cJob,cPrevStatus);
	}
	
	time(&luClock);
			
        sprintf(gcQuery,"INSERT INTO tJob SET uInstance=%u,cLabel='%s',cJobData='%s',cJobName='%s',cServer='%s',uJobClient=%u,uJobDate=%lu,uJobStatus=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
			uInstance,
			"Priority=Normal",
			cJob,
			cJobName,
			cServer,
			guLoginClient,
			uJobDate,
			mysqlISP_Waiting,
			guOrg,
			guLoginClient,
			(unsigned long)luClock);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));

}//void SubmitServiceJob()


int CheckParameterValue(char *cValue, char *cParamType, char *cRange, unsigned uUnique,
		char *cExtVerify, char *cParameter, unsigned uDeployed)
{
	unsigned uLow=0,uHigh=0,uErr=0,uLen=0;
	unsigned register int i;
	char *cp;
	char query[512];
	MYSQL_RES *res;

	sscanf(cRange,"%u,%u",&uLow,&uHigh);
	if(!strcmp(cParamType,"integer") ||
			!strcmp(cParamType,"yes/no") ||
			!strcmp(cParamType,"on/off") ||
			!strcmp(cParamType,"megabytes")
	)
		sscanf(cValue,"%u",&uLen);
	else
		uLen=strlen(cValue);

	if(uLen>uHigh || uLen<uLow)
		return(2);

	if(strstr(cValue,"{{"))
		return(3);

	if((cp=strchr(cValue,'\''))) *cp='`';

	if(!strcmp(cParamType,"integer"))
	{
	}
	else if(!strcmp(cParamType,"lowercaselogin"))
	{
		for(i=0;i<strlen(cValue)&& !uErr;i++)
		{
			if(!isalnum(cValue[i]) && 
				(!i || ( cValue[i]!='.' && 
				cValue[i]!='-' && cValue[i]!='_' && cValue[i]!=':'))  ) uErr=1;
			if(isupper(cValue[i])) cValue[i]=tolower(cValue[i]);
		}
	}
	else if(!strcmp(cParamType,"encryptedpasswd"))
	{
		if(strchr(cValue,' ')) uErr=1;
		if(!uErr && !(cValue[0]=='$' && cValue[1]=='1'))
			EncryptPasswdMD5(cValue);
	}
	else if(!strcmp(cParamType,"string"))
	{
		if((cp=strchr(cValue,'\''))) *cp='`';
	}
	else if(!strcmp(cParamType,"megabytes"))
	{
	}
	else if(!strcmp(cParamType,"word"))
	{
		if(strchr(cValue,' ')) uErr=1;
	}
	else if(!strcmp(cParamType,"ipv6"))
	{
		//Not supported yet
		uErr=1;
	}
	else if(!strcmp(cParamType,"ip"))
	{
		unsigned a=999,b=999,c=999,d=999;
		
		sscanf(cValue,"%u.%u.%u.%u",&a,&b,&c,&d);
		if( a>255 || b>255 || c>255 || d>255) uErr=1;
		if(strchr(cValue,' ')) uErr=1;
	}
	else if(!strcmp(cParamType,"fqdn"))
	{
		FQDomainName(cValue);
		if(!strchr(cValue,'.')) uErr=1;
	}
	else if(!strcmp(cParamType,"email"))
	{
		for(i=0;i<strlen(cValue)&& !uErr;i++)
		{
			if(!isalpha(cValue[i]) && 
				(!i || ( cValue[i]!='@' && 
				cValue[i]!='.'))  ) uErr=1;
			if(isupper(cValue[i])) cValue[i]=tolower(cValue[i]);
		}

		if(!strchr(cValue,'.')) uErr=1;
		if(!strchr(cValue,'@')) uErr=1;
	}
	else if(!strcmp(cParamType,"yes/no"))
	{
		WordToLower(cValue);
		if(strcmp(cValue,"yes") && strcmp(cValue,"no")) uErr=1;
	}
	else if(!strcmp(cParamType,"on/off"))
	{
		WordToLower(cValue);
		if(strcmp(cValue,"on") && strcmp(cValue,"off")) uErr=1;
	}

	
	//TODO Quick configure hack: uDeployed=7. If deployed skip this check.
	//fix this lameass 7 hack for uDeployed>0 except 7 bullshit
	if(cExtVerify[0] && ( !uDeployed || uDeployed==7 ) )
	{

		//Needs tConfiguration cExtmysql{{Whatever}}Ip/Db/Pwd/Name set.
		//Usual example cExtmysqlRadiusIp/Db/Pwd/Name set.
		if((cp=strchr(cExtVerify,':')))
		{
			char cConfQuery[256];
			char cIp[256]={""};
			char cDb[256]={""};
			char cPwd[256]={""};
			char cLogin[256]={""};

			*cp=0;

			sprintf(cConfQuery,"%.200sIp",cExtVerify);
			GetConfiguration(cConfQuery,cIp);
			sprintf(cConfQuery,"%.200sName",cExtVerify);
			GetConfiguration(cConfQuery,cDb);
			sprintf(cConfQuery,"%.200sPwd",cExtVerify);
			GetConfiguration(cConfQuery,cPwd);
			sprintf(cConfQuery,"%.200sLogin",cExtVerify);
			GetConfiguration(cConfQuery,cLogin);
		
			//Must add tConfiguration parameters	
//debug only	
//			sprintf(gcQuery,"cExtVerify:%s:\n cConfQuery:%s:\n cIP:%s\n cDb:%s\n cPwd:%s\n cLogin:%s\n Verify query:(%s)",cExtVerify,cConfQuery,cIp,cDb,cPwd,cLogin,cp+1);
//			htmlPlainTextError(gcQuery);	
			if(cDb[0]==0 || cPwd[0]==0 || cLogin[0]==0)
				return(uErr=5);
		
			ExtConnectDb(cDb,cIp,cPwd,cLogin);

			sprintf(gcQuery,cp+1,cValue);
			//debug only
			//htmlPlainTextError(gcQuery);	
			mysql_query(&MysqlExt,gcQuery);
        		if(mysql_errno(&MysqlExt))
                		htmlPlainTextError(mysql_error(&MysqlExt));
			res=mysql_store_result(&MysqlExt);

		
			if(strstr(query,"ReverseTest"))
			{
				//If no rows fail
				if(!mysql_num_rows(res))
					uErr=5;
			}
			else
			{
				//If at least one row fail
				if(mysql_num_rows(res))
					uErr=5;
			}
			mysql_free_result(res);
			*cp=':';
//debug Only
//sprintf(gcQuery,"uErr=%u",uErr);
//htmlPlainTextError(gcQuery);	

			if(uErr) return(uErr);
		}
	}

//debug Only
//sprintf(gcQuery,"uErr=%u",uErr);
//htmlPlainTextError(gcQuery);	

	return(uErr);

}//int CheckParameterValue()


void CheckAllParameters(void)
{
	unsigned uErr=0;
	register int i=0;
	structParameter ParamData;
	static char cMessage[1024]={""};
//printf("Content-type: text/plain\n\n");
	for(i=0;i<uParameterCount;i++)
	{
//int CheckParameterValue(char *cValue, char *cParamType, char *cRange, unsigned uUnique,char *cExtVerify
		GetParamInfo(cParameterName[i],&ParamData);
		if(!cParameterName[i][0]) continue;
/*		printf("ParamData.cParamType=%s ParamData.cRange=%s ParamData.uUnique=%u ParamData.cExtVerify=%s cParameterName=%s\n"
						,ParamData.cParamType
						,ParamData.cRange
						,ParamData.uUnique
						,ParamData.cExtVerify
						,cParameterName[i]);
*/
		if((uErr=CheckParameterValue(cParameterInput[i]
						,ParamData.cParamType
						,ParamData.cRange
						,ParamData.uUnique
						,ParamData.cExtVerify
						,cParameterName[i]
						,0)))
		{	
			gcQuery[0]=0;
			if(uErr==2)
			sprintf(cMessage,"%s='%s' is out of range %s, type %s (i=%u)"
					,cParameterLabel[i],cParameterInput[i],ParamData.cRange,ParamData.cParamType,i);
			else if(uErr==1)
sprintf(cMessage,"%s='%s' is not valid for type %s, range %s",cParameterLabel[i],cParameterInput[i],ParamData.cParamType,ParamData.cRange);
			else if(uErr==3)
sprintf(cMessage,"%s='%s' unconfigured type %s, range %s",cParameterLabel[i],cParameterInput[i],ParamData.cParamType,ParamData.cRange);
			else if(uErr==5)
sprintf(cMessage,"%s='%s' remote verify failed",cParameterLabel[i],cParameterInput[i]);
			gcMessage=cMessage;
			cParameterStyle[i]="type_fields_req";
			htmlProductDeployWizard(uStep);
		}
	}
//	exit(0);
}//void CheckAllParameters()


void ExtConnectDb(char *cDbName, char *cDbIp, char *cDbPwd, char *cDbLogin)
{
	if(!cDbIp[0]) cDbIp=NULL;

        mysql_init(&MysqlExt);
        if (!mysql_real_connect(&MysqlExt,cDbIp,cDbLogin,cDbPwd,cDbName,0,NULL,0))
        {
		sprintf(gcQuery,"ExtConnectDb failed for cDbIp:%s: cDbName:%s: cDbLogin:%s: cDbPwd:%s:",cDbIp,cDbName,cDbLogin,cDbPwd);
                htmlPlainTextError(gcQuery);
        }

}//end of ExtConnectDb()


void GetParamInfo(const char *cParameter, structParameter *structParam)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        sprintf(gcQuery,"SELECT tParamType.cLabel,tParameter.cRange,tParameter.cParameter,tParameter.uUnique,tParameter.cExtVerify,"
			"tParameter.uModPostDeploy,tParameter.uAllowMod FROM tParameter,tParamType WHERE cParameter='%s' AND "
			"tParameter.uParamType=tParamType.uParamType",cParameter);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
        
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

void SetParamFieldsOn(void)
{
	register int i=0;
	
	for(i=0;i<100;i++)
		cParameterStyle[i]="type_fields";
}


void funcDateSelectTable(FILE *fp)
{
	register int i;
	time_t luClock;
	char cMonth[4]={""};
	char cYear[8]={""};
	struct tm *structTime;
	unsigned uThisYear=0;
	
	time(&luClock);
	structTime=localtime(&luClock);
	strftime(cMonth,4,"%b",structTime);
	strftime(cYear,8,"%Y",structTime);
	sscanf(cYear,"%u",&uThisYear);

	fprintf(fp,"<table width=150>\n");
	fprintf(fp,"<tr><td>Day</td><td>Month</td><td>Year</td></tr>\n");

	fprintf(fp,"<tr>\n");

	fprintf(fp,"<td><select name=cDay>\n");
	fprintf(fp,"<option>Now</option>");
	for(i=1;i<32;i++)
		fprintf(fp,"<option>%u</option>",i);
	fprintf(fp,"</select></td>\n");

	fprintf(fp,"<td><select name=cMonth>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Jan"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Jan</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Feb"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Feb</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Mar"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Mar</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Apr"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Apr</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"May"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"May</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Jun"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Jun</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Jul"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Jul</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Aug"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Aug</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Sep"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Sep</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Oct"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Oct</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Nov"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Nov</option>\n");

	fprintf(fp,"<option");
	if(!strcmp(cMonth,"Dec"))
		fprintf(fp," selected>");
	else
		fprintf(fp,">");
	fprintf(fp,"Dec</option>\n");

	fprintf(fp,"</select></td>\n");

	fprintf(fp,"<td><font face=arial size=1><select name=cYear>\n");
	for(i=uThisYear;i<uThisYear+5;i++)
		fprintf(fp,"<option>%u</option>",i);
	fprintf(fp,"</select></td>\n");

	fprintf(fp,"</tr>\n");

	fprintf(fp,"</table>\n");

}//void funcDateSelectTable()


void funcSelectProduct(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcSelectProduct(fp) start -->\n");

	sprintf(gcQuery,"SELECT uProduct,cLabel FROM tProduct WHERE uAvailable=1 ORDER BY cLabel");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
		
	fprintf(fp,"<select class=type_fields title='Select the product you want to deploy' name=uProduct>\n");

	sprintf(gcQuery,"%u",uProduct);
	
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<option value=%s ",field[0]);
		if(!strcmp(field[0],gcQuery))
				fprintf(fp,"selected");
		fprintf(fp,">%s</option>\n",field[1]);
	}

	fprintf(fp,"</select\n");

	fprintf(fp,"<!-- funcSelectProduct(fp) end -->\n");

}//void funcSelectProduct(FILE *fp)

