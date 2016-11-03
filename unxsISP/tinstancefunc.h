/*
FILE
	svn ID removed
PURPOSE
	tinstance.c non-schema dependent application file

AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details
	See LICENSE included

NOTES
	Product can't have two identical services. Workaround create another 
	gcFunctionally identical service.

	Clarify uBilled usage in some spec somewhere. Right now seems to be
	that uBilled==0 when no invoice exists that references tInstance.uInstance.
	No invoice has been generated, that still exists in tInvoice or the
	archived tMonthInvoice sub-system. If new status invoices (i.e.
	invoices that have not been processed somehow) are removed then uBilled is
	reset to 0. How this interacts with interface already deployed option
	is not completly clear yet.

	NEW better specs! (I hope :) 7/2004

	Deployment or modification shouldn't touch tClient.mBalance only
	tInvoice generation or removal should do this.

	Canceling a product will just take it out of invoicing procedures.

	Unholding should generate a special billing only product equal to
	mReleaseFee.

	All billing only products should be one time only. After paid they
	should serve as place holders only until used again. The status of
	these products is then always waiting. Since not in queue no problem.
	May want to add a special status later for these.

	One time products behave in the same way but are deployed.

	tInstance should not have uPayment. This just adds confusion.
	The tInstance."uPeriod" comes via normalization via 
	tInstance.uProduct=tProduct.uPeriod

	This means we have to alter schema again...:( and also fix the
	current complicated invoicing query.

	In the end this will simplify things. I hope :)

 
*/

#include "mysqlisp.h"


//unxsISP remote server connections
MYSQL mysqlext;

//First Hold and Cancel pre status to WaitingCancel or WaitingHold
static unsigned uPrevStatus=0;
void ReaduPrevStatus(void);

#include <ctype.h>

//Move to final language.h file
#define LANG_NB_CONFIRMDEPLOY "Deploy Product"
#define LANG_NBB_CONFIRMDEPLOY "<input class=lrevButton title='Double check service parameters!' type=submit name=gcCommand value=\"Deploy Product\">"
#define LANG_NB_DEPLOY "Deploy"
#define LANG_NBB_DEPLOY "<input title=\"Deploy new or modified product\" type=submit name=gcCommand value=Deploy>"
#define LANG_NBB_CANCEL "<input class=warnButton title='Cancel product. May erase client files.' type=submit name=gcCommand value=Cancel>"
#define LANG_NBB_HOLD "<input class=alertButton title='Place product on hold. Keeps client files suspends service depending on service type.' type=submit name=gcCommand value=Hold>"
#define LANG_NBB_CONFIRMHOLD "<input class=lalertButton title='Confirm hold' type=submit name=gcCommand value='Confirm Hold'>"
#define LANG_NBB_CONFIRMCANCEL "<input class=lwarnButton title='Confirm Cancel' type=submit name=gcCommand value='Confirm Cancel'>"
#define LANG_NBB_RELEASE "<input class=alertButton title='Release product Hold or Cancel. Product reverts to pre Hold or Cancel status.' type=submit name=gcCommand value=Release>"
#define LANG_NBB_CONFIRMHOLD "<input class=lalertButton title='Confirm hold' type=submit name=gcCommand value='Confirm Hold'>"

//File scope global
static unsigned uProductType=0;
static unsigned uExtClient=0;
static unsigned uAlreadyDeployedChecked=0;
//local
void ClientProductConfigList(void);
void ConfigForm(void);
void QuickConfig(char *cName,char *cValue);
int CheckParameterValue(char *cValue, char *cParamType, char *cRange, unsigned uUnique,
		char *cExtVerify,char *cParameter, unsigned uDeployed);
void CheckAllParameters(unsigned uDeployed);

void EncryptPasswdWithSalt(char *pw, char *salt);
void SubmitServiceJob(unsigned uService,char *cJobName, char *cServer,unsigned uInstance,time_t uJobDate);
void SubmitInstanceJob(char *cJobType,time_t uJobDate);

void GetConfiguration(const char *cLabel,char *cValue,unsigned uHtml);
void ExtConnectDb(char *cDbName, char *cDbIp, char *cDbPwd, char *cDbLogin);

void SubmitSingleServiceJob(unsigned uService,unsigned uInstance,char *cJobType,unsigned uExtClient,time_t uJobDate);
unsigned GetInstanceStatus(unsigned uInstance);	
void htmlDateSelectTable(void);
char *strptime(char *buf, const char *format, const struct tm *tm);
time_t cDateToUnixTime(char *cDate);
void GetBillingHours(unsigned const uInstance,char *mHours);
void GetBillingTraffic(unsigned const uInstance,char *mTraffic);
void GetBillingUnits(unsigned const uInstance,char *cShortUnits,char *cLongUnits);

static char cDay[3]={""};
static char cMonth[4]={"1"};
static char cYear[5]={"2004"};

void ExtProcesstInstanceVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uExtClient") )
			sscanf(entries[i].val,"%u",&uExtClient);
		else if(!strncmp(entries[i].name,"CF_",3) )
			QuickConfig(entries[i].name,entries[i].val);
		else if(!strcmp(entries[i].name,"uAlreadyDeployedChecked") )
			uAlreadyDeployedChecked=1;

		else if(!strcmp(entries[i].name,"cDay") )
			sprintf(cDay,"%.2s",entries[i].val);
		else if(!strcmp(entries[i].name,"cMonth") )
			sprintf(cMonth,"%.3s",entries[i].val);
		else if(!strcmp(entries[i].name,"cYear") )
			sprintf(cYear,"%.4s",entries[i].val);
	}
}//void ExtProcesstInstanceVars(pentry entries[], int x)



void ExttInstanceCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tInstanceTools"))
	{
		time_t luClock=0;
		time_t uDeployTime=0;
		//Accounting vars
		char mPrice[16]={"0.0"};
		char mSetupFee[16]={"0.0"};
		char mReleaseFee[16]={"0.0"};
		unsigned uPeriod=0;

		if(!strcmp(gcCommand,LANG_NB_DEPLOY))
                {
                        ProcesstInstanceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				if(!uInstance)
					tInstance("Must supply valid uInstance");
				if(!uProduct)
					tInstance("Must supply valid uProduct");
				//Must refresh uStatus	
				uStatus=GetInstanceStatus(uInstance);	
				if(  uStatus!=unxsISP_NeverDeployed &&
						uStatus!=unxsISP_Canceled &&
						uStatus!=unxsISP_DeployedMod &&
						uStatus!=unxsISP_OnHold &&
						uStatus!=unxsISP_WaitingCancel &&
						uStatus!=unxsISP_WaitingHold)
					tInstance("Can't deploy incorrect uStatus");

				if(uStatus==unxsISP_Canceled ||
						uStatus==unxsISP_NeverDeployed)
					CheckAllParameters(0);
				else
					CheckAllParameters(1);

				GetProductAccountingVars(uProduct,mPrice,mSetupFee,
								mReleaseFee,&uProductType,&uPeriod);

				if(uProductType==unxsISP_BillingOnly)
                        		guMode=2999;
				else
                        		guMode=3000;
                        	tInstance(LANG_NB_CONFIRMDEPLOY);
			}
			else
				tInstance("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEPLOY))
                {

                        ProcesstInstanceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				if(!uInstance)
					tInstance("Must supply valid uInstance");
				if(!uProduct)
					tInstance("Must supply valid uProduct");

				time(&luClock);
				if(cDay[0] && cDay[0]!='N')
				{
					sprintf(gcQuery,"%s-%s-%s",cDay,cMonth,cYear);
					uDeployTime=cDateToUnixTime(gcQuery);
				}
				else
				{
					uDeployTime=luClock;
				}

				uStatus=GetInstanceStatus(uInstance);	
				if(  uStatus!=unxsISP_NeverDeployed &&
						uStatus!=unxsISP_Canceled &&
						uStatus!=unxsISP_DeployedMod &&
						uStatus!=unxsISP_OnHold &&
						uStatus!=unxsISP_WaitingCancel &&
						uStatus!=unxsISP_WaitingHold)
					tInstance("Can't deploy incorrect uStatus. uStatus may have just changed.");

                        	guMode=3000;

				if(uStatus==unxsISP_Canceled ||
					uStatus==unxsISP_NeverDeployed)
					CheckAllParameters(0);
				else
					CheckAllParameters(1);

				//Accounting rule
				GetProductAccountingVars(uProduct,mPrice,mSetupFee,
								mReleaseFee,&uProductType,&uPeriod);
				if(uProductType!=unxsISP_BillingOnly)
				{

					if(uStatus==unxsISP_NeverDeployed || 
							uStatus==unxsISP_Canceled)
						SubmitInstanceJob("New",uDeployTime);
					else if(uStatus==unxsISP_DeployedMod ||
							uStatus==unxsISP_OnHold )
						SubmitInstanceJob("Mod",uDeployTime);

					//Change status to appropiate waiting status
					if(uStatus==unxsISP_NeverDeployed ||
							uStatus==unxsISP_Canceled)
						uStatus=unxsISP_WaitingInitial;
					else
						uStatus=unxsISP_WaitingRedeployment;
				}
				else
				{
					uStatus=unxsISP_Deployed;
				}

				sprintf(gcQuery,"UPDATE tInstance SET uStatus=%u,uModBy=%u,uModDate=%lu WHERE uInstance=%u",
						uStatus,uModBy=guLoginClient,uModDate=luClock,uInstance);
		        	mysql_query(&gMysql,gcQuery);
		        	if(mysql_errno(&gMysql))
					tClientConfig(mysql_error(&gMysql));
				guMode=0;
				if(uProductType==unxsISP_BillingOnly)
					tInstance("'Billing Only' product deployed.");
				else
					tInstance("Job submitted.");
			}
			else
				tInstance("<blink>Error</blink>: Denied by permissions settings");   
		}

		else if(!strcmp(gcCommand,"Cancel"))
                {
                        ProcesstInstanceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				if(!uInstance)
					tInstance("Must supply valid uInstance");
				uStatus=GetInstanceStatus(uInstance);	
				if(  uStatus!=unxsISP_Deployed &&
						uStatus!=unxsISP_DeployedMod &&
						uStatus!=unxsISP_WaitingHold &&
						uStatus!=unxsISP_WaitingRedeployment 
						&& uStatus!=unxsISP_OnHold)
					tInstance("Can't cancel incorrect uStatus");
				GetProductAccountingVars(uProduct,mPrice,mSetupFee,
								mReleaseFee,&uProductType,&uPeriod);
				if(uProductType!=unxsISP_BillingOnly)
					guMode=4000;
				else
					guMode=3999;
				tInstance("Please confirm");
			}
			else
				tInstance("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,"Confirm Cancel"))
                {
                        ProcesstInstanceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				if(!uInstance)
					tInstance("Must supply valid uInstance");
				uStatus=GetInstanceStatus(uInstance);	
				if(  uStatus!=unxsISP_Deployed &&
						uStatus!=unxsISP_DeployedMod &&
						uStatus!=unxsISP_WaitingHold &&
						uStatus!=unxsISP_WaitingRedeployment 
						&& uStatus!=unxsISP_OnHold)
					tInstance("Can't cancel incorrect uStatus. uStatus may have changed.");
				time(&luClock);
				if(uProductType!=unxsISP_BillingOnly)
				{
					if(cDay[0] && cDay[0]!='N')
					{
						sprintf(gcQuery,"%s-%s-%s",cDay,cMonth,cYear);
						uDeployTime=cDateToUnixTime(gcQuery);
					}
					else
					{
						uDeployTime=luClock;
					}
					sprintf(gcQuery,"UPDATE tInstance SET uStatus=%u,uModBy=%u,uModDate=%lu WHERE uInstance=%u",
							uStatus=unxsISP_WaitingCancel,uModBy=guLoginClient,uModDate=luClock,uInstance);
			        	mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					SubmitInstanceJob("Cancel",uDeployTime);
				}
				else
				{
					sprintf(gcQuery,"UPDATE tInstance SET uStatus=%u,uModBy=%u,uModDate=%lu WHERE uInstance=%u",
							uStatus=unxsISP_Canceled,uModBy=guLoginClient,uModDate=luClock,uInstance);
			        	mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				}


				if(uProductType==unxsISP_BillingOnly)
					tInstance("'Billing Only' product canceled.");
				else
					tInstance("Cancel job submitted.");
			}
			else
				tInstance("<blink>Error</blink>: Denied by permissions settings");   
		}

		else if(!strcmp(gcCommand,"Hold"))
                {
                        ProcesstInstanceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				if(!uInstance)
					tInstance("Must supply valid uInstance");
				uStatus=GetInstanceStatus(uInstance);	
				if(  uStatus!=unxsISP_Deployed &&
						uStatus!=unxsISP_DeployedMod &&
						uStatus!=unxsISP_WaitingRedeployment 
						&& uStatus!=unxsISP_WaitingCancel)
					tInstance("Can't hold incorrect uStatus. uStatus may have changed.");
				guMode=5000;
				tInstance("Please confirm");
			}
			else
				tInstance("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,"Confirm Hold"))
                {
                        ProcesstInstanceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				if(!uInstance)
					tInstance("Must supply valid uInstance");
				uStatus=GetInstanceStatus(uInstance);	
				if(  uStatus!=unxsISP_Deployed &&
						uStatus!=unxsISP_DeployedMod &&
						uStatus!=unxsISP_WaitingRedeployment 
						&& uStatus!=unxsISP_WaitingCancel)
					tInstance("Can't hold incorrect uStatus. uStatus may have changed.");
				time(&luClock);
				if(cDay[0] && cDay[0]!='N')
				{
					sprintf(gcQuery,"%s-%s-%s",cDay,cMonth,cYear);
					uDeployTime=cDateToUnixTime(gcQuery);
				}
				else
				{
					uDeployTime=luClock;
				}
				uPrevStatus=uStatus;
				sprintf(gcQuery,"UPDATE tInstance SET uStatus=%u,uModBy=%u,uModDate=%lu WHERE uInstance=%u",
						uStatus=unxsISP_WaitingHold,uModBy=guLoginClient,uModDate=luClock,uInstance);
		        	mysql_query(&gMysql,gcQuery);
		        	if(mysql_errno(&gMysql))
					tClientConfig(mysql_error(&gMysql));
				SubmitInstanceJob("Hold",uDeployTime);
				tInstance("Hold job submitted");
			}
			else
				tInstance("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,"Release"))
                {
                        ProcesstInstanceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				if(!uInstance)
					tInstance("Must supply valid uInstance");
				uStatus=GetInstanceStatus(uInstance);	
				if(  uStatus!=unxsISP_WaitingHold 
						&& uStatus!=unxsISP_WaitingCancel)
					tInstance("Can't release incorrect uStatus. uStatus may have changed.");
				ReaduPrevStatus();
				//Support for jobless deployments
				if(!uPrevStatus)
				{
					switch(uStatus)
					{
						case unxsISP_WaitingHold:
						case unxsISP_WaitingCancel:
						default:
							uPrevStatus=unxsISP_Deployed;
					}
				}
				
				time(&luClock);
				if(cDay[0] && cDay[0]!='N')
				{
					sprintf(gcQuery,"%s-%s-%s",cDay,cMonth,cYear);
					uDeployTime=cDateToUnixTime(gcQuery);
				}
				else
				{
					uDeployTime=luClock;
				}
				sprintf(gcQuery,"UPDATE tInstance SET uStatus=%u,uModBy=%u,uModDate=%lu WHERE uInstance=%u",
						uStatus=uPrevStatus,uModBy=guLoginClient,uModDate=luClock,uInstance);
		        	mysql_query(&gMysql,gcQuery);
		        	if(mysql_errno(&gMysql))
					tClientConfig(mysql_error(&gMysql));

				switch(uPrevStatus)
				{
					//No jobs needed but keep error jobs: cRemoteMsg non empty string.
					case unxsISP_Deployed:
					case unxsISP_DeployedMod:
						sprintf(gcQuery,"DELETE FROM tJob WHERE uInstance=%u AND uJobStatus=%u AND cRemoteMsg=''",
								uInstance,unxsISP_Waiting);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
						tInstance("Product status restored. Job removed");
					break;

					//Replace jobs
					case unxsISP_WaitingHold:
						SubmitInstanceJob("Hold",uDeployTime);
					break;
					case unxsISP_WaitingCancel:
						SubmitInstanceJob("Cancel",uDeployTime);
					break;
					case unxsISP_WaitingRedeployment:
						SubmitInstanceJob("Mod",uDeployTime);
					break;
				}
				tInstance("Product status restored and job replaced");
			}
			else
				tInstance("<blink>Error</blink>: Denied by permissions settings");   
		}

		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstInstanceVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy) && (uStatus==unxsISP_NeverDeployed || uStatus==unxsISP_WaitingInitial))
			{
				MYSQL_RES *res;

				if(!uInstance)
					tInstance("Must supply valid uInstance");
				sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uInstance=%u AND uJobStatus=%u",uInstance,unxsISP_RemotelyQueued);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
					tInstance("One or more remotely queued jobs exist. Must wait");
	                        guMode=2001;
				tInstance(LANG_NB_CONFIRMDEL);
			}
			else
				tInstance("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstInstanceVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy) && (uStatus==unxsISP_NeverDeployed || uStatus==unxsISP_WaitingInitial))
			{
				MYSQL_RES *res;

				if(!uInstance)
					tInstance("Must supply valid uInstance");

				sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uInstance=%u AND uJobStatus=%u",uInstance,unxsISP_RemotelyQueued);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
					tInstance("One or more remotely queued jobs exist. Must wait");

				sprintf(gcQuery,"DELETE FROM tClientConfig WHERE uGroup=%u",uInstance);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));

				sprintf(gcQuery,"DELETE FROM tJob WHERE uInstance=%u AND uJobStatus=%u",uInstance,unxsISP_Waiting);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));

                        	guMode=5;
				DeletetInstance();
			}
			else
				tInstance("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstInstanceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				if(!uInstance)
					tInstance("Must supply valid uInstance");
				GetProductAccountingVars(uProduct,mPrice,mSetupFee,
								mReleaseFee,&uProductType,&uPeriod);
				guMode=2002;
				tInstance(LANG_NB_CONFIRMMOD);
			}
			else
				tInstance("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstInstanceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
                        	guMode=2002;
				if(!cLabel[0])
					tInstance("Must provide cLabel");
				if(!uClient)
					tInstance("Must provide uClient");
				if(!uInstance)
					tInstance("Must supply valid uInstance");
				if(!uProduct)
					tInstance("Must provide uProduct");

				if( (uAlreadyDeployedChecked && guPermLevel>9) && 
					( uStatus==unxsISP_NeverDeployed ||
					(uStatus==unxsISP_WaitingInitial && 
						guPermLevel>11 && guLoginClient==1) ) )
				{
					CheckAllParameters(0);
					uStatus=unxsISP_Deployed;

				}
                        	guMode=0;
				uModBy=guLoginClient;
				ModtInstance();
			}
			else
				tInstance("<blink>Error</blink>: Denied by permissions settings");   
                }
	}

}//void ExttInstanceCommands(pentry entries[], int x)


void ExttInstanceButtons(void)
{
	OpenFieldSet("tInstance Aux Panel",100);
	
	if(uExtClient)
	{
		printf("<a class=darkLink title='Return to current client page' href=unxsISP.cgi?gcFunction=tClient&uClient=%u>[Return]</a>\n",uExtClient);
		printf("<input type=hidden name=uExtClient value=%u>\n",uExtClient);
	}


	switch(guMode)
        {
		case 2999:
			printf("<p><u>Check billing parameters</u><br>");
                        printf(LANG_NBB_CONFIRMDEPLOY);
		break;

                case 3000:
			printf("<p><u>Check service parameters</u><br>");
			htmlDateSelectTable();
                        printf(LANG_NBB_CONFIRMDEPLOY);
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 3999:
                        printf("<p>");
                        printf(LANG_NBB_CONFIRMCANCEL);
                break;

                case 4000:
                        printf("<p>");
			htmlDateSelectTable();
                        printf(LANG_NBB_CONFIRMCANCEL);
                break;

                case 5000:
                        printf("<p>");
			htmlDateSelectTable();
                        printf(LANG_NBB_CONFIRMHOLD);
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                        printf("<br>\n");
			if( ( (guPermLevel>9 && uStatus==unxsISP_NeverDeployed) ||
				(guPermLevel>11 && uStatus==unxsISP_WaitingInitial 
					&& guLoginClient==1) )
					&& uProductType!=unxsISP_BillingOnly)
				printf("<font color=red>Already"
					" deployed</font> <input type=checkbox"
					" name=uAlreadyDeployed >"
					" (Check tJob and targets!)\n");
			if(uClient)
				ConfigForm();
                break;

	}

	ClientProductConfigList();
	if(uProduct)
	{
		char mPrice[16];
		char mSetupFee[16];
		char mReleaseFee[16];
		unsigned uProductType;
		unsigned uPeriod;
		

		GetProductAccountingVars(uProduct,mPrice,mSetupFee,mReleaseFee,
			&uProductType,&uPeriod);
		printf("<p><u>Product Accounting Info</u><br>\n");
		printf("mPrice: $%s<br>\n",mPrice);
		printf("mSetupFee: $%s<br>\n",mSetupFee);
		printf("mReleaseFee: $%s<br>\n",mReleaseFee);
	}

	CloseFieldSet();
}//void ExttInstanceButtons(void)


void ExttInstanceAuxTable(void)
{

}//void ExttInstanceAuxTable(void)


void ExttInstanceGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uInstance"))
		{
			sscanf(gentries[i].val,"%u",&uInstance);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"uExtClient"))
		{
			sscanf(gentries[i].val,"%u",&uExtClient);
		}
	}
	tInstance("");

}//void ExttInstanceGetHook(entry gentries[], int x)


void ExttInstanceSelect(void)
{
	if(uExtClient)
	{
		char cExtra[65]={""};

		sprintf(cExtra,"uClient=%u",uExtClient);
		//the two "1" args below add to the query 1 LIKE 1, which alwaus matchs true ;)
		ExtSelectSearch("tInstance",VAR_LIST_tInstance,"1","1",cExtra,0);
	}
	else
		ExtSelect("tInstance",VAR_LIST_tInstance,0);

}//void ExttInstanceSelect(void)


void ExttInstanceSelectRow(void)
{
	ExtSelectRow("tInstance",VAR_LIST_tInstance,uInstance);

}//void ExttInstanceSelectRow(void)


void ExttInstanceListSelect(void)
{
	char cCat[512];

	ExtListSelect("tInstance",VAR_LIST_tInstance);

	//Changes here must be reflected below in ExttInstanceListFilter()
        if(!strcmp(gcFilter,"uInstance"))
        {
                sscanf(gcCommand,"%u",&uInstance);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tInstance.uInstance=%u \
						ORDER BY uInstance",
						uInstance);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uInstance");
        }

}//void ExttInstanceListSelect(void)


void ExttInstanceListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uInstance"))
                printf("<option>uInstance</option>");
        else
                printf("<option selected>uInstance</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttInstanceListFilter(void)



void ExttInstanceNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(uAllowMod(uOwner,uCreatedBy))   
	{
		if(  uStatus==unxsISP_NeverDeployed ||
			uStatus==unxsISP_Canceled || 
			uStatus==unxsISP_DeployedMod ||
			uStatus==unxsISP_OnHold ||
			uStatus==unxsISP_WaitingCancel ||
			uStatus==unxsISP_WaitingHold)
				printf(LANG_NBB_DEPLOY);

		printf(LANG_NBB_MODIFY);

		if(uStatus==unxsISP_Deployed ||
			uStatus==unxsISP_DeployedMod ||
			uStatus==unxsISP_OnHold ||
			uStatus==unxsISP_WaitingHold ||
			uStatus==unxsISP_WaitingRedeployment )
				printf(LANG_NBB_CANCEL);

		if(uStatus==unxsISP_Deployed ||
			uStatus==unxsISP_DeployedMod ||
			uStatus==unxsISP_WaitingCancel ||
			uStatus==unxsISP_WaitingRedeployment )
				printf(LANG_NBB_HOLD);

		if(uStatus==unxsISP_WaitingCancel ||
			uStatus==unxsISP_WaitingHold )
				printf(LANG_NBB_RELEASE);
	}

	if(uAllowDel(uOwner,uCreatedBy) && (uStatus==unxsISP_NeverDeployed || uStatus==unxsISP_WaitingInitial))
		printf(LANG_NBB_DELETE);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttInstanceNavBar(void)


void ClientProductConfigList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cService[100]={"Z"};

	if(!uClient || !uInstance)
		return;

        sprintf(gcQuery,"SELECT tClientConfig.uConfig,tParameter.cParameter,tClientConfig.cValue,tService.cLabel FROM "
			"tClientConfig,tParameter,tService WHERE tClientConfig.uParameter=tParameter.uParameter AND "
			"tClientConfig.uGroup=%u AND tService.uService=tClientConfig.uService "
			"ORDER BY tService.cLabel,tParameter.cParameter",uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
        	printf("<p><u>Product Services</u>\n");
        	while((field=mysql_fetch_row(res)))
        	{
			if(strcmp(field[3],cService))
			{
				strcpy(cService,field[3]);
				printf("<br><u>%s</u><br><br>",cService);
			}
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tClientConfig&uConfig=%s"
				"&uInstance=%u&uExtClient=%u>%s=%s</a><br>\n",field[0],uInstance,uClient,field[1],field[2]);
        	}
	}
        mysql_free_result(res);

}//void ClientProductConfigList(void)


void ConfigForm(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char *cp,*cp2={""},*cp3={""};

        sprintf(gcQuery,"SELECT tClientConfig.cValue,tParamType.cLabel,tParameter.cRange,tParameter.uParameter FROM "
			"tClientConfig,tParameter,tInstance,tParamType WHERE tClientConfig.uParameter=tParameter.uParameter "
			"AND tInstance.uInstance=tClientConfig.uGroup AND tParameter.uParamType=tParamType.uParamType AND "
			"tInstance.uInstance=%u AND tClientConfig.cValue LIKE '{{%%}}' GROUP BY tClientConfig.cValue "
			"ORDER BY tClientConfig.cValue",uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s<br>\n",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		printf("<p><u>Quick Configure Form</u><br>\n");
		printf("<table width=150>");
        	while((field=mysql_fetch_row(res)))
        	{
			if((cp2=strchr(field[0],'{'))) cp2+=2;
			if((cp=strchr(field[0],'}'))) *cp=0;
			if((cp3=strchr(field[2],','))) cp3++;
			printf("<tr>");
			printf("<td><font face=arial,helvetica size=2>%s</td>"
				"<td><input title='Will replace all service parameters' "
				"type=text size=20 name='CF_%s_%s' maxlength=%s></td>\n",cp2,cp2,field[3],cp3);
			printf("</tr>");
        	}
		mysql_free_result(res);
	printf("</table>");
	
	}

}//void ConfigForm(void)


void QuickConfig(char *cName,char *cValue)
{
	char cCurlyVar[100]={""};
	unsigned uParameter=0;
	char *cp,*cp2,*cp3;
	unsigned uErr=0;
	structParameter structParam;
	

	if((cp=strchr(cName,'_'))) strcpy(cCurlyVar,cp+1);
	if((cp3=strchr(cCurlyVar,'_'))) *cp3=0;


	if((cp2=strchr(cp+1,'_'))) sscanf(cp2+1,"%u",&uParameter);

	GetParamInfo(uParameter,&structParam);	

	//uDeployed==7 Quick config tool should only be used for new product
	//instances.
	if(cValue[0] && !(uErr=CheckParameterValue(cValue,
					structParam.cParamType,
					structParam.cRange,
					structParam.uUnique,
					structParam.cExtVerify,
					structParam.cParameter,7)))
	{

        sprintf(gcQuery,"UPDATE tClientConfig SET cValue='%s' WHERE uGroup=%u AND cValue LIKE '%%{{%s}}'",cValue,uInstance,cCurlyVar);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                tInstance(mysql_error(&gMysql));
	}
	else
	{
		if(uErr==2)
			sprintf(gcQuery,"%s='%s' is out of range %s, type %s",structParam.cParameter,cValue,structParam.cRange,structParam.cParamType);
		else if(uErr==1)
			sprintf(gcQuery,"%s='%s' is not valid for type %s, range %s",structParam.cParameter,cValue,structParam.cParamType,structParam.cRange);
		else if(uErr==3)
			sprintf(gcQuery,"%s='%s' unconfigured type %s, range %s",structParam.cParameter,cValue,structParam.cParamType,structParam.cRange);
		else if(uErr==4)
			sprintf(gcQuery,"%s='%s' unique value required. (Already in use)",structParam.cParameter,cValue);
		else if(uErr==5)
			sprintf(gcQuery,"%s='%s' remote verify failed",structParam.cParameter,cValue);

		if(uErr) tInstance(gcQuery);
	}

}//void QuickConfig(char *cName,char *cValue)


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

	//Allow no value parameters. Don't waste time checking further
	if(!cValue[0]) return(0);

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
				cValue[i]!='-' && cValue[i]!='_'))  ) uErr=1;
			if(isupper(cValue[i])) cValue[i]=tolower(cValue[i]);
		}
	}
	else if(!strcmp(cParamType,"encryptedpasswd"))
	{
		if(strchr(cValue,' ')) uErr=1;
		if(!uErr && !(cValue[0]=='.' && cValue[1]=='.'))
			EncryptPasswdWithSalt(cValue,"..");
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
	
	//Quick configure hack: uDeployed is 7
	if(uUnique)
	{
	        sprintf(gcQuery,"SELECT tClientConfig.uConfig FROM tClientConfig,tParameter,tInstance WHERE "
				"tClientConfig.cValue='%s' AND tParameter.uParameter=tClientConfig.uParameter "
				"AND tClientConfig.uGroup=tInstance.uInstance AND tParameter.cParameter='%s' "
				"AND tInstance.uStatus!=%u",cValue,cParameter,unxsISP_Canceled);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
                	htmlPlainTextError(mysql_error(&gMysql));
        	res=mysql_store_result(&gMysql);
		if( uDeployed!=7 )//Not quick configure. Allow one
		{
			if(mysql_num_rows(res)>1)
				uErr=4;
		}
		else if(uDeployed==7)//Quick configure. Allow none
		{
			if(mysql_num_rows(res))
				uErr=4;
		}
        	mysql_free_result(res);

		if(uErr) return(uErr);
	}

	//TODO Quick configure hack: uDeployed=7. If deployed skip this check.
	//fix this lameass 7 hack for uDeployed>0 except 7 bullshit
	if(cExtVerify[0] && ( !uDeployed || uDeployed==7 ) )
	{
		if(uAlreadyDeployedChecked)
			return(uErr);

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
			GetConfiguration(cConfQuery,cIp,1);
			sprintf(cConfQuery,"%.200sName",cExtVerify);
			GetConfiguration(cConfQuery,cDb,1);
			sprintf(cConfQuery,"%.200sPwd",cExtVerify);
			GetConfiguration(cConfQuery,cPwd,1);
			sprintf(cConfQuery,"%.200sLogin",cExtVerify);
			GetConfiguration(cConfQuery,cLogin,1);
		
			//Must add tConfiguration parameters	
//debug only	
//			sprintf(gcQuery,"cExtVerify:%s: cConfQuery:%s: cIP:%s cDb:%s cPwd:%s cLogin:%s (%s)",cExtVerify,cConfQuery,cIp,cDb,cPwd,cLogin,cp+1);
//			htmlPlainTextError(gcQuery);	
			if(cDb[0]==0 || cPwd[0]==0 || cLogin[0]==0)
				return(uErr=5);
		
			ExtConnectDb(cDb,cIp,cPwd,cLogin);

			sprintf(gcQuery,cp+1,cValue);
			//debug only
			//htmlPlainTextError(gcQuery);	
			mysql_query(&mysqlext,gcQuery);
        		if(mysql_errno(&mysqlext))
                		htmlPlainTextError(mysql_error(&mysqlext));
			res=mysql_store_result(&mysqlext);

		
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


void CheckAllParameters(unsigned uDeployed)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uErr=0;
	unsigned uUnique=0;

        sprintf(gcQuery,"SELECT tParameter.cRange,tParamType.cLabel,tClientConfig.cValue,tParameter.cParameter,"
			"tParameter.uUnique,tParameter.cExtVerify FROM tClientConfig,tParameter,tParamType WHERE "
			"tClientConfig.uParameter=tParameter.uParameter AND tClientConfig.uGroup=%u AND "
			"tParamType.uParamType=tParameter.uParamType",uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                tInstance(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
	if(!mysql_num_rows(res))
		tInstance("Empty product instance, contact unxsISP admin");
        while((field=mysql_fetch_row(res)))
	{
		if(field[4][0]=='0')
			uUnique=0;
		else
			uUnique=1;
		if((uErr=CheckParameterValue(field[2],field[1],field[0],uUnique,
						field[5],field[3],uDeployed)))
		{	
			if(uErr==2)
			sprintf(gcQuery,"%s='%s' is out of range %s, type %s"
					,field[3],field[2],field[0],field[1]);
			else if(uErr==1)
				sprintf(gcQuery,"%s='%s' is not valid for type %s, range %s",field[3],field[2],field[1],field[0]);
			else if(uErr==3)
				sprintf(gcQuery,"%s='%s' unconfigured type %s, range %s",field[3],field[2],field[1],field[0]);
			else if(uErr==4)
				sprintf(gcQuery,"%s='%s' unique value required %s, range %s",field[3],field[2],field[1],field[0]);
			else if(uErr==5)
				sprintf(gcQuery,"%s='%s' remote verify failed",field[3],field[2]);
			tInstance(gcQuery);
		}
	}

        mysql_free_result(res);

}//void CheckAllParameters()


void ReaduPrevStatus(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char *cp;

        sprintf(gcQuery,"SELECT cJobData FROM tJob WHERE uInstance=%u AND uJobStatus=%u"
						,uInstance,unxsISP_Waiting);
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
#define unxsISP_ProductType_BILLINGONLY 5
void SubmitInstanceJob(char *cJobType,time_t uJobDate)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uService=0;
	unsigned uJob=0;
	unsigned uJobGroup=0;
	unsigned uProductType=0;
	unsigned uFirst=1;
	char cJobName[100];


	//Do not submit jobs for products of billing type
        sprintf(gcQuery,"SELECT uProductType FROM tProduct WHERE uProduct=%u",uProduct);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uProductType);
	mysql_free_result(res);

	if(uProductType==unxsISP_ProductType_BILLINGONLY)
		return;


	ReaduPrevStatus();

	//Remove all waiting jobs of same product instance
        sprintf(gcQuery,"DELETE FROM tJob WHERE uInstance=%u AND uJobStatus=%u"
						,uInstance,unxsISP_Waiting);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));

	//One job for each service that comprise the product
        sprintf(gcQuery,"SELECT tService.uService,tService.cJobName,tService.cServer FROM "
			"tService,tServiceGlue WHERE tServiceGlue.uService=tService.uService "
			"AND tServiceGlue.uServiceGroup=%u",uProduct);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uService);
		sprintf(cJobName,"%.81s.%.18s",field[1],cJobType);
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
		sprintf(gcQuery,"DELETE FROM tJob WHERE uInstance=%u AND uJobStatus=%u AND cJobName='%s'",uInstance,unxsISP_Waiting,cJobName);
		mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		uClient=uExtClient;
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
	

        sprintf(gcQuery,"SELECT tParameter.cParameter,tClientConfig.cValue FROM tClientConfig,tParameter WHERE "
			"tClientConfig.uParameter=tParameter.uParameter AND tClientConfig.uGroup=%u AND "
			"tClientConfig.uService=%u ORDER BY tParameter.cParameter",uInstance,uService);
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
					uClient,
					ForeignKey("tClient","cLabel",uClient));
	strcat(cJob,cLine);
	
	if(strstr(cJobName,".Hold") || strstr(cJobName,".Cancel"))
	{
		sprintf(cPrevStatus,"\nuPrevStatus=%u",uPrevStatus);
		strcat(cJob,cPrevStatus);
	}
	
	time(&luClock);
			
        sprintf(gcQuery,"INSERT INTO tJob SET uInstance=%u,cLabel='%s',cJobData='%s',cJobName='%s',"
			"cServer='%s',uJobClient=%u,uJobDate=%lu,uJobStatus=%u,uOwner=%u,uCreatedBy=%u,"
			"uCreatedDate=%lu",
			uInstance,
			"Priority=Normal",
			cJob,
			cJobName,
			cServer,
			uClient,
			uJobDate,
			unxsISP_Waiting,
			guLoginClient,
			guLoginClient,
			(unsigned long)luClock);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));

}//void SubmitServiceJob()


void GetConfiguration(const char *cName, char *cValue, unsigned uHtml)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cQuery[512]={""};

        sprintf(cQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='%s'",cName);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
		{
        	        tConfiguration(mysql_error(&gMysql));
		}
		else
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
                strcpy(cValue,field[0]);
        mysql_free_result(res);

}//void GetConfiguration(char *cName, char *cValue)


//Uses MYSQL mysqlext
void ExtConnectDb(char *cDbName, char *cDbIp, char *cDbPwd, char *cDbLogin)
{
	if(!cDbIp[0]) cDbIp=NULL;

        mysql_init(&mysqlext);
        if (!mysql_real_connect(&mysqlext,cDbIp,cDbLogin,cDbPwd,cDbName,0,NULL,0))
        {
		sprintf(gcQuery,"ExtConnectDb failed for cDbIp:%s: cDbName:%s: cDbLogin:%s: cDbPwd:%s:",cDbIp,cDbName,cDbLogin,cDbPwd);
                htmlPlainTextError(gcQuery);
        }

}//end of ExtConnectDb()


void htmlDateSelectTable(void)
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

	printf("<table width=150>\n");
	printf("<tr><td>Day</td><td>Month</td><td>Year</td></tr>\n");

	printf("<tr>\n");

	printf("<td><select name=cDay>\n");
	printf("<option>Now</option>");
	for(i=1;i<32;i++)
		printf("<option>%u</option>",i);
	printf("</select></td>\n");

	printf("<td><select name=cMonth>\n");

	printf("<option");
	if(!strcmp(cMonth,"Jan"))
		printf(" selected>");
	else
		printf(">");
	printf("Jan</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Feb"))
		printf(" selected>");
	else
		printf(">");
	printf("Feb</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Mar"))
		printf(" selected>");
	else
		printf(">");
	printf("Mar</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Apr"))
		printf(" selected>");
	else
		printf(">");
	printf("Apr</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"May"))
		printf(" selected>");
	else
		printf(">");
	printf("May</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Jun"))
		printf(" selected>");
	else
		printf(">");
	printf("Jun</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Jul"))
		printf(" selected>");
	else
		printf(">");
	printf("Jul</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Aug"))
		printf(" selected>");
	else
		printf(">");
	printf("Aug</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Sep"))
		printf(" selected>");
	else
		printf(">");
	printf("Sep</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Oct"))
		printf(" selected>");
	else
		printf(">");
	printf("Oct</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Nov"))
		printf(" selected>");
	else
		printf(">");
	printf("Nov</option>\n");

	printf("<option");
	if(!strcmp(cMonth,"Dec"))
		printf(" selected>");
	else
		printf(">");
	printf("Dec</option>\n");

	printf("</select></td>\n");

	printf("<td><font face=arial size=1><select name=cYear>\n");
	for(i=uThisYear;i<uThisYear+5;i++)
		printf("<option>%u</option>",i);
	printf("</select></td>\n");

	printf("</tr>\n");

	printf("</table>\n");

}//void htmlDateSelectTable(void)


void GetBillingHours(unsigned const uInstance,char *mHours)
{
	MYSQL_RES *res;
        MYSQL_ROW field;

	if(!uInstance)
		return;

        sprintf(gcQuery,"SELECT tClientConfig.cValue FROM tClientConfig,tParameter WHERE "
			"tClientConfig.uParameter=tParameter.uParameter AND tClientConfig.uGroup=%u "
			"AND tParameter.cParameter='Hours'",uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sprintf(mHours,"%.15s",field[0]);
	}
	mysql_free_result(res);

}//void GetBillingHours()


void GetBillingTraffic(unsigned const uInstance,char *mTraffic)
{
	MYSQL_RES *res;
        MYSQL_ROW field;

	if(!uInstance)
		return;

        sprintf(gcQuery,"SELECT tClientConfig.cValue FROM tClientConfig,tParameter WHERE "
			"tClientConfig.uParameter=tParameter.uParameter AND tClientConfig.uGroup=%u "
			"AND (tParameter.cParameter='Megabytes' OR tParameter.cParameter='Gigabytes')",uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		sprintf(mTraffic,"%.12s",field[0]);
	}
	mysql_free_result(res);

}//void GetBillingTraffic()


void GetBillingUnits(unsigned const uInstance,char *cShortUnits,char *cLongUnits)
{
	MYSQL_RES *res;
        MYSQL_ROW field;

	if(!uInstance)
		return;

        sprintf(gcQuery,"SELECT tClientConfig.cValue FROM tClientConfig,tParameter WHERE "
			"tClientConfig.uParameter=tParameter.uParameter AND "
			"tClientConfig.uGroup=%u AND tParameter.cParameter='ShortUnits'",uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sprintf(cShortUnits,"%.12s",field[0]);
	mysql_free_result(res);

        sprintf(gcQuery,"SELECT tClientConfig.cValue FROM tClientConfig,tParameter WHERE "
			"tClientConfig.uParameter=tParameter.uParameter AND "
			"tClientConfig.uGroup=%u AND tParameter.cParameter='LongUnits'",uInstance);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sprintf(cLongUnits,"%.32s",field[0]);
	mysql_free_result(res);

}//void GetBillingUnits(unsigned const uInstance,char *cShortUnits,char *cLongUnits)

time_t cDateToUnixTime(char *cDate)
{
        struct  tm locTime;
        time_t  res;

        bzero(&locTime, sizeof(struct tm));
	if(strchr(cDate,'-'))
        	strptime(cDate,"%d-%b-%Y", &locTime);
	else if(strchr(cDate,'/'))
        	strptime(cDate,"%d/%b/%Y", &locTime);
	else if(strchr(cDate,' '))
        	strptime(cDate,"%d %b %Y", &locTime);
        locTime.tm_sec = 0;
        locTime.tm_min = 0;
        locTime.tm_hour = 0;
        res = mktime(&locTime);
        return(res);
}


// vim:tw=78
