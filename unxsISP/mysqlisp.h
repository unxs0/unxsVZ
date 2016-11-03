/*
FILE
	svn ID removed
PURPOSE
	Shared constants, data and prototypes.

*/

//Instance status only
#define unxsISP_NeverDeployed 1
#define unxsISP_WaitingInitial 2
#define unxsISP_DeployedMod 3
#define unxsISP_OnHold 6
#define unxsISP_WaitingRedeployment 9 
#define unxsISP_WaitingCancel 11 
#define unxsISP_WaitingHold 12 
//Job status Only
#define unxsISP_RemotelyQueued 7
#define unxsISP_Waiting 10
//Both
#define unxsISP_Deployed 4
#define unxsISP_Canceled 5

//Product related
//Tied to tProductType
#define unxsISP_BillingOnly 5
//Tied to tPeriod
#define unxsISP_BillingHourly 7
#define unxsISP_BillingStorage 9
#define unxsISP_BillingTraffic 10

//These values must match tInvoiceStatus.uInvoiceStatus
//Invoice related
//Open invoices start at 1
#define unxsISP_NewInvoice 1
#define unxsISP_SentInvoice 2
#define unxsISP_PartialInvoice 4
#define unxsISP_PastDueInvoice 6
#define unxsISP_BadDebtInvoice 7
#define unxsISP_CollectionInvoice 8
//Closed invoices start at 100
#define unxsISP_PaidInvoice 110
#define unxsISP_VoidInvoice 210
#define unxsISP_ReceiptSentInvoice 211

//Invoice job queue related
#define unxsISP_MailQueuedInvoice 10
#define unxsISP_PrintQueuedInvoice 11


typedef struct {
	char cParameter[65];
	char cParamType[33];
	char cRange[17];
	char cExtVerify[256];
	unsigned uUnique;
	unsigned uAllowMod;
	unsigned uModPostDeploy;
} structParameter;
void GetParamInfo(unsigned uParameter,structParameter *structParam);

void SubmitSingleServiceJob(unsigned uService,unsigned uInstance,char *cJobType,unsigned uExtClient,time_t uJobDate);


void GetProductAccountingVars(unsigned const uProduct, char *mPrice, char *mSetupFee,
                                char *mReleaseFee, unsigned *uProductType, unsigned *uPeriod);

//Application specific standard functions
void fileInvoice(FILE *fp);
void filePackingSlip(FILE *fp);
void textFatalError(char const *cErrorText);
void GetConfiguration(char const *cDefault,char *cValue,unsigned uHtml);

//mysqlisp.c
void ProcessJobQueue(unsigned const uDebug, char const *cServer);

//tinstancefunc.h
void GetBillingHours(unsigned const uInstance,char *mHours);
void GetBillingTraffic(unsigned const uInstance,char *mTraffic);
void GetBillingUnits(unsigned const uInstance,char *cShortUnits,char *cLongUnits);

#include <openisp/template.h>

