/*
FILE
	$Id: mysqlisp.h,v 1.14 2004/07/29 17:44:53 ggw Exp $
PURPOSE
	Shared constants, data and prototypes.

*/

//Instance status only
#define mysqlISP_NeverDeployed 1
#define mysqlISP_WaitingInitial 2
#define mysqlISP_DeployedMod 3
#define mysqlISP_OnHold 6
#define mysqlISP_WaitingRedeployment 9 
#define mysqlISP_WaitingCancel 11 
#define mysqlISP_WaitingHold 12 
//Job status Only
#define mysqlISP_RemotelyQueued 7
#define mysqlISP_Waiting 10
//Both
#define mysqlISP_Deployed 4
#define mysqlISP_Canceled 5

//Product related
//Tied to tProductType
#define mysqlISP_BillingOnly 5
//Tied to tPeriod
#define mysqlISP_BillingHourly 7
#define mysqlISP_BillingStorage 9
#define mysqlISP_BillingTraffic 10

//These values must match tInvoiceStatus.uInvoiceStatus
//Invoice related
//Open invoices start at 1
#define mysqlISP_NewInvoice 1
#define mysqlISP_SentInvoice 2
#define mysqlISP_PartialInvoice 4
#define mysqlISP_PastDueInvoice 6
#define mysqlISP_BadDebtInvoice 7
#define mysqlISP_CollectionInvoice 8
//Closed invoices start at 100
#define mysqlISP_PaidInvoice 110
#define mysqlISP_VoidInvoice 210
#define mysqlISP_ReceiptSentInvoice 211

//Invoice job queue related
#define mysqlISP_MailQueuedInvoice 10
#define mysqlISP_PrintQueuedInvoice 11


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

//template.c
//Data
typedef struct t_template {
        char *name[100];//pointers to var_name strings
        char *value[100];//pointers to substitution strings
} t_template ;
//Globals
//Used in cases where end user can select from set of templates.
//Usually from a site cookie.
extern unsigned guSkin;


int TemplateTable(char *cSQLQuery, char *cTableName,struct t_template *template);
void fileDirectTemplate(FILE *fp,char *cTemplateName);
void htmlDirectTemplate(char *cTemplateName);
int htmlTableTemplate(char *cTemplateName,char *cSQLQuery, char *cTableName);
int htmlStructTemplate(char *cTemplateName,struct t_template *ptrTemplate);
int fileStructTemplate(FILE *fp, char *cTemplateName,struct t_template *ptrTemplate);
void Template(char *cTemplate, struct t_template *template, FILE *fp);
void eMailInvoice(char *cConfigEmail, char *cSubjectHeader, char *cSubjectEnd, 
		char *cTemplateName, struct t_template *template);
void eMailPackingSlip(char *cConfigEmail, char *cSubjectHeader, char *cSubjectEnd, 
		char *cTemplateName, struct t_template *template);
void eMailTemplate(char *cEmail, char *cSubjectHeader, char *cSubjectEnd, 
		char *cTemplateName, struct t_template *template);
void eMailNotice(char *cConfigEmail, char *cSubjectHeader, char *cSubjectEnd, 
		char *cTemplateName, struct t_template *template);
void TemplateSelectQuery(char *cTemplateName);
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

