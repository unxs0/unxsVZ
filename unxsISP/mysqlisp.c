/*
FILE
	mysqlisp.c
	$Id$
PURPOSE
	Jobqueue and other special gcCommand line processing.
LEGAL
	(C) 2004-2009 Gary Wallis and Hugo Urquiza. GPL License Applies.
WORK IN PROGRESS
	Using print invoice gcFunction to develop basic templates and procedures.
	Then once that is working ok. Will do the mail invoice gcFunction.

*/

#include "mysqlrad.h"
#include "mysqlisp.h"


//Contents
void ProcessJobQueue(unsigned const uDebug, char const *cServer);
void UpdateJobStatus(unsigned uJob,unsigned uJobStatus,char *cRemoteMsg);
void ApplicationFunctions(FILE *fp,char const *cVarName);

void PrintInvoicesFromJobQueue(unsigned const uJob);
void PrintInvoiceItems(FILE *fp);
void PrintPaymentMethod(FILE *fp);
void PrintCardInfo(FILE *fp);

void EmailInvoicesFromJobQueue(unsigned const uJob);
void TextConnectDb(void);
void AnalyzeTables(void);

//File global
static unsigned guDebug=0;
static unsigned guInvoice=0;

void ProcessJobQueue(unsigned const uDebug, char const *cServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	time_t luClock;
	unsigned uJob=0;
	
	if(uDebug) guDebug=1;

	TextConnectDb();
	
	time(&luClock);

	if(uDebug) printf("\nProcessJobQueue(%u,%s)\n",uDebug,cServer);
	
	//Limit is for crontab period adjust for your situacion.
	sprintf(gcQuery,"SELECT uJob,cJobName FROM tJob WHERE (cServer='Any' OR cServer='%s') AND uJobStatus=%u AND uJobDate<%lu ORDER BY uJob LIMIT 10",cServer,unxsISP_Waiting,luClock);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                fprintf(stderr,"%s\n",mysql_error(&gMysql));
               	return;
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uJob);

		if(uDebug) printf("uJob:%u cJobName:%s\n",uJob,field[1]);
		
		if(!strncmp(field[1],"unxsISP.tInvoice.Email",23))
			EmailInvoicesFromJobQueue(uJob);
		else if(!strncmp(field[1],"unxsISP.tInvoice.Print",23))
			PrintInvoicesFromJobQueue(uJob);

	}

	if(uDebug) printf("\nDone\n");
	mysql_close(&gMysql);
	exit(0);
	
}//void ProcessJobQueue(unsigned const uDebug, char const *cServer)


void UpdateJobStatus(unsigned uJob,unsigned uJobStatus,char *cRemoteMsg)
{
	time_t luClock;

	time(&luClock);
	sprintf(gcQuery,"UPDATE tJob SET uJobStatus=%u,uModBy=1,uModDate=%lu,cRemoteMsg='%.32s' WHERE uJob=%u",uJobStatus,luClock,TextAreaSave(cRemoteMsg),uJob);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                fprintf(stderr,"%s\n",mysql_error(&gMysql));

}//void UpdateJobStatus(unsigned uJob,unsigned uJobStatus,char *cRemoteMsg)


//template.c required gcFunction for application specific template {{funcX}}
void ApplicationFunctions(FILE *fp,char const *cVarName)
{
	if(guDebug)
		printf("ApplicationFunctions(fp,%s)\n",cVarName);

	if(!strcmp("funcPrintInvoiceItems",cVarName))
		PrintInvoiceItems(fp);
	else if(!strcmp("funcPrintPaymentMethod",cVarName))
		PrintPaymentMethod(fp);
	else if(!strcmp("funcPrintCardInfo",cVarName))
		PrintCardInfo(fp);

}//void ApplicationFunctions(FILE *fp,char const *cVarName)


void PrintInvoicesFromJobQueue(unsigned const uJob)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	register unsigned uPrinted=0;
	char cInvoiceBaseDir[256]={"/home/openisp/unxsISP/invoices"};
	char cInvoiceDir[384];
	char cInvoicePath[384];
	char cTodayDir[16]={"error"};
	FILE *fp;
	struct stat statInfo;
	time_t luClock;
	struct tm *structTime;
	struct t_template template;


	if(guDebug) printf("PrintInvoicesFromJobQueue(%u)\n",uJob);

	GetConfiguration("cInvoiceBaseDir",cInvoiceBaseDir,0);

	//Check base dir
	if(stat(cInvoiceBaseDir,&statInfo))
	{
		fprintf(stderr,"cInvoiceBaseDir: %s does not exist\n",cInvoiceBaseDir);
		mysql_close(&gMysql);
		exit(1);
	}


	//Make todays date <year><month><day> sub dir
	time(&luClock);
        structTime=localtime(&luClock);
	strftime(cTodayDir,16,"%Y-%m-%d",structTime);
	sprintf(cInvoiceDir,"%.255s/%.15s",cInvoiceBaseDir,cTodayDir);
	if(stat(cInvoiceDir,&statInfo))
	{
		umask(022);
		if(mkdir(cInvoiceDir,0700))
		{
			fprintf(stderr,"cInvoiceDir: %s can't mkdir\n",cInvoiceDir);
			mysql_close(&gMysql);
			exit(1);
		}
	}

	if(guDebug) printf("cInvoiceDir: %s\n",cInvoiceDir);


	sprintf(gcQuery,"SELECT tInvoice.uInvoice,tClient.uClient,tInvoice.cFirstName,tInvoice.cLastName,tClient.cLabel,tInvoice.cEmail,tInvoice.cAddr1,tInvoice.cAddr2,tInvoice.cCity,tInvoice.cState,tInvoice.cZip,tInvoice.cCountry,tInvoice.mTotal,tInvoice.mSH,tInvoice.mTax,tInvoice.cComments FROM tInvoice,tClient WHERE tInvoice.uClient=tClient.uClient AND tInvoice.uQueueStatus=%u ORDER BY uInvoice",unxsISP_PrintQueuedInvoice);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(1);
	}

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{
		sprintf(cInvoicePath,"%s/%s-%s.txt",cInvoiceDir,field[1],field[0]);
		umask(066);
		if((fp=fopen(cInvoicePath,"w"))==NULL)
		{
			fprintf(stderr,"Could not fopen: %s for writing\n",cInvoicePath);
			mysql_free_result(res);
			exit(1);
		}
		if(guDebug) printf("cInvoicePath: %s\n",cInvoicePath);

		//Use the cInvoicePrintTop template
		//fprintf(fp,"%s-%s\n",field[1],field[0]);
		//fprintf(fp,"%s %s\n\n",field[2],field[3]);

		template.name[0]="uInvoice";
		template.value[0]=field[0];
		//Needed for template {{funcPrintInvoiceItems}}
		sscanf(field[0],"%u",&guInvoice);

		template.name[1]="uClient";
		template.value[1]=field[1];

		template.name[2]="cFirstName";
		template.value[2]=field[2];

		template.name[3]="cLastName";
		template.value[3]=field[3];

		template.name[4]="cLabel";
		template.value[4]=field[4];
/*
00 tInvoice.uInvoice
01 tClient.uClient
02 tInvoice.cFirstName
03 tInvoice.cLastName
04 tClient.cLabel
05 tInvoice.cEmail
06 tInvoice.cAddr1
07 tInvoice.cAddr2
08 tInvoice.cCity
09 tInvoice.cState
10 tInvoice.cZip
11 tInvoice.cCountry
12 tInvoice.mTotal
13 tInvoice.mSH
14 tInvoice.mTax
15 tInvoice.cComments
*/

		template.name[5]="cEmail";
		template.value[5]=field[5];

		template.name[6]="cAddr1";
		template.value[6]=field[6];

		template.name[7]="cAddr2";
		template.value[7]=field[7];

		template.name[8]="cCity";
		template.value[8]=field[8];

		template.name[9]="cState";
		template.value[9]=field[9];

		template.name[10]="cZip";
		template.value[10]=field[10];

		template.name[11]="cCountry";
		template.value[11]=field[11];

		template.name[12]="mTotal";
		template.value[12]=field[12];

		template.name[13]="mSH";
		template.value[13]=field[13];

		template.name[14]="mTax";
		template.value[14]=field[14];

		template.name[15]="cComments";
		template.value[15]=field[15];

		template.name[16]="cDate";
		template.value[16]=cTodayDir;

		template.name[17]="";//Close
		fileStructTemplate(fp,"cInvoicePrintTop",&template);

		sprintf(gcQuery,"UPDATE tInvoice SET uQueueStatus=0 WHERE uInvoice=%s",field[0]);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			mysql_close(&gMysql);
			exit(1);
		}

		fprintf(fp,"\n");
		fprintf(fp,"\n\n(Invoice generated by unxsISP: %s)\n",RELEASE);
		fclose(fp);

		uPrinted++;
	}
	mysql_free_result(res);


	sprintf(gcQuery,"Printed: %u",uPrinted);
	UpdateJobStatus(uJob,unxsISP_Deployed,gcQuery);

	if(guDebug) printf("Done with PrintInvoicesFromJobQueue()\n");

}//void PrintInvoicesFromJobQueue(unsigned const uJob)


void PrintInvoiceItems(FILE *fp)
{
	MYSQL_RES *res;
        MYSQL_ROW field;
	char cQuery[512];
	struct t_template template;
	unsigned uItem=1;
	char cuItem[16];
	char cExtProductPrice[33]={"$0.0 (p-error)"};
	char cExtInvoicePrice[64]={"$0.0 (i-error)"};
	char cExtProductLabel[33]={"(cExtProductLabel error)"};
	char cExtInvoiceLabel[256]={"(cExtInvoiceLabel error)"};
	double dTotal=0.0;
	double dSubTotal=0.0;
	unsigned uPeriod=0;
	unsigned uProductType=0;
	unsigned uInstance=0;
	char cShortUnits[13];
	char cLongUnits[33];

sprintf(cQuery,"SELECT tProduct.mPrice,tInvoiceItems.mPrice,tProduct.cLabel,tInvoiceItems.cLabel,tProduct.uProductType,tProduct.uPeriod,tInvoiceItems.uInstance FROM tInvoiceItems,tProduct WHERE tInvoiceItems.uProduct=tProduct.uProduct AND tInvoiceItems.uInvoice=%u ORDER BY tInvoiceItems.uInvoiceItems",guInvoice);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		return;
	}

	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{

/*
{{uItem}}
{{mProductPrice}}
{{cProductLabel}}
{{cInvoiceLabel}}
{{mInvoicePrice}}

*/
		sscanf(field[4],"%u",&uProductType);
		sscanf(field[5],"%u",&uPeriod);
		sscanf(field[6],"%u",&uInstance);
		cShortUnits[0]=0;
		cLongUnits[0]=0;

		if( uProductType==unxsISP_BillingOnly )
			GetBillingUnits(uInstance,cShortUnits,cLongUnits);

		if(strlen(field[3])>60)
		{
			register unsigned i,j,uWrapTime;

			//wrap at space at ~60 chars
			for(i=0,j=0,uWrapTime=0;field[3][i] && j<255;i++)
			{

				if(j>0 && !(j%60)) uWrapTime=1;
					
				if(field[3][i]==' ' && uWrapTime)
				{
					cExtInvoiceLabel[j++]=field[3][i];
					cExtInvoiceLabel[j++]='\n';
					//TODO the real solution would be to strip white
					//space then to add the fixed 8 space indentation
					//Also needs to adjust according to item number width.
					if(field[3][i+1]!=' ')
						cExtInvoiceLabel[j++]=' ';
					cExtInvoiceLabel[j++]=' ';
					cExtInvoiceLabel[j++]=' ';
					cExtInvoiceLabel[j++]=' ';
					cExtInvoiceLabel[j++]=' ';
					cExtInvoiceLabel[j++]=' ';
					cExtInvoiceLabel[j++]=' ';
					cExtInvoiceLabel[j++]=' ';

					uWrapTime=0;
				}
				else
				{
					cExtInvoiceLabel[j++]=field[3][i];
				}
			}
			cExtInvoiceLabel[j]=0;
		}
		else
		{
			sprintf(cExtInvoiceLabel,"%.61s",field[3]);
		}

		sprintf(cExtProductLabel,"%.32s",field[2]);
		if(cShortUnits[0])
			sprintf(cExtProductPrice,"$%.12s/%.12s",field[0],cShortUnits);
		else
			sprintf(cExtProductPrice,"$%.12s",field[0]);
		sprintf(cExtInvoicePrice,"$%.12s",field[1]);

		template.name[0]="mProductPrice";
		template.value[0]=cExtProductPrice;

		template.name[1]="mInvoicePrice";
		template.value[1]=cExtInvoicePrice;

		template.name[2]="cProductLabel";
		template.value[2]=cExtProductLabel;

		template.name[3]="cInvoiceLabel";
		template.value[3]=cExtInvoiceLabel;

		template.name[4]="uItem";
		sprintf(cuItem,"%u",uItem++);
		template.value[4]=cuItem;


		template.name[5]="";//Close
		fileStructTemplate(fp,"cInvoiceItemsPrint",&template);

		fprintf(fp,"\n\n");

		sscanf(field[1],"%lf",&dSubTotal);
		dTotal+=dSubTotal;
	}
	mysql_free_result(res);

	fprintf(fp,"\nTotal: $%.2lf\n",dTotal);

}//void PrintInvoiceItems(FILE *fp)


void PrintPaymentMethod(FILE *fp)
{
	MYSQL_RES *res;
        MYSQL_ROW field;
	char cQuery[256];
	char cTemplate[256];
	struct t_template template;

	sprintf(cQuery,"SELECT tPayment.cInvoiceFooter FROM tPayment,tClient,tInvoice WHERE tPayment.uPayment=tClient.uPayment AND tClient.uClient=tInvoice.uClient AND tInvoice.uInvoice=%u",guInvoice);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		return;
	}

	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cTemplate,"%.220s_Print",field[0]);

		template.name[0]="";//Close
		fileStructTemplate(fp,cTemplate,&template);
	}
	mysql_free_result(res);

}//void PrintPaymentMethod(FILE *fp)


void PrintCardInfo(FILE *fp)
{
	MYSQL_RES *res;
        MYSQL_ROW field;
	char cQuery[256];

	sprintf(cQuery,"SELECT cCardName,cCardType,cCardNumber,uExpMonth,uExpYear FROM tInvoice WHERE tInvoice.uInvoice=%u",guInvoice);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		return;
	}

	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"%s\n%s xxxx.xxxx.xxxx.%.4s\nExp %s/%s",
			field[0],
			field[1],
			field[2]+(strlen(field[2])-4),
			field[3],
			field[4]);

	}
	else
	{
		fprintf(fp,"Unknown error in PrintCardInfo()\n");
	}
	mysql_free_result(res);

}//void PrintCardInfo(FILE *fp)


void EmailInvoicesFromJobQueue(unsigned const uJob)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	register unsigned uEmailed=0;
	char cSendmail[256]={"/usr/lib/sendmail -t"};
	char cInvoiceBccEmailAddr[256]={""};
	char cFromEmailAddr[256]={"acctdept@isp.net"};
	char cInvoiceSubject[256]={"Invoice "};
	char cInvoiceTemplate[100]={""};
	char cToday[16]={"0-0-0"};
	FILE *fp;
	time_t luClock;
	struct tm *structTime;
	struct t_template template;
	unsigned uInvoiceStatus=0;
	unsigned uNewInvoiceStatus=0;


	if(guDebug) printf("EmailInvoicesFromJobQueue(%u)\n",uJob);

	//Make todays date <year><month><day>
	time(&luClock);
        structTime=localtime(&luClock);
	strftime(cToday,16,"%Y-%m-%d",structTime);

	if(guDebug) printf("cToday: %s\n",cToday);

	GetConfiguration("cSendmail",cSendmail,0);
	GetConfiguration("cInvoiceBccEmailAddr",cInvoiceBccEmailAddr,0);
	GetConfiguration("cFromEmailAddr",cFromEmailAddr,0);
	GetConfiguration("cInvoiceSubject",cInvoiceSubject,0);

	sprintf(gcQuery,"SELECT tInvoice.uInvoice,tClient.uClient,tInvoice.cFirstName,tInvoice.cLastName,tClient.cLabel,tInvoice.cEmail,tInvoice.cAddr1,tInvoice.cAddr2,tInvoice.cCity,tInvoice.cState,tInvoice.cZip,tInvoice.cCountry,tInvoice.mTotal,tInvoice.mSH,tInvoice.mTax,tInvoice.cComments,tInvoice.uInvoiceStatus FROM tInvoice,tClient WHERE tInvoice.uClient=tClient.uClient AND tInvoice.uQueueStatus=%u ORDER BY uInvoice",unxsISP_MailQueuedInvoice);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
	{
/*
0  tInvoice.uInvoice,
1  tClient.uClient,
2  tInvoice.cFirstName,
3  tInvoice.cLastName,
4  tClient.cLabel,
5  tInvoice.cEmail,
6  tInvoice.cAddr1,
7  tInvoice.cAddr2,
8  tInvoice.cCity,
9  tInvoice.cState,
10 tInvoice.cZip,
11 tInvoice.cCountry,
12 tInvoice.mTotal,
13 tInvoice.mSH,
14 tInvoice.mTax,
15 tInvoice.cComments,
16 tInvoice.uInvoiceStatus

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

*/

		//No email skip
		if(!field[5][0])
		{
			if(guDebug)
				printf("Skipping no cEmail. uInvoice=%s\n",field[0]);
			continue;
		}

		sscanf(field[16],"%u",&uInvoiceStatus);
		if(guDebug) printf("cMail: %s uInvoiceStatus:%u\n",field[5],uInvoiceStatus);

		switch(uInvoiceStatus)
		{
			case unxsISP_NewInvoice:
			case unxsISP_SentInvoice:
				sprintf(cInvoiceTemplate,"cInvoiceEmailTop");
				sprintf(cInvoiceSubject,"Invoice ");
				uNewInvoiceStatus=unxsISP_SentInvoice;
			break;

			case unxsISP_PaidInvoice:
				sprintf(cInvoiceTemplate,"cInvoiceEmailPaidTop");
				sprintf(cInvoiceSubject,"Paid in Full. Invoice ");
				uNewInvoiceStatus=unxsISP_ReceiptSentInvoice;
			break;

			case unxsISP_PastDueInvoice:
				sprintf(cInvoiceTemplate,"cInvoiceEmailPastTop");
				sprintf(cInvoiceSubject,"Past Due. Invoice ");
				uNewInvoiceStatus=unxsISP_PastDueInvoice;
			break;

			case unxsISP_PartialInvoice:
				sprintf(cInvoiceTemplate,"cInvoiceEmailPartTop");
				sprintf(cInvoiceSubject,"Partial Payment Received. Invoice ");
				uNewInvoiceStatus=unxsISP_PartialInvoice;
			break;

			default:
				if(guDebug)
					printf("Skipping unsupported uInvoiceStatus=%u\n",
						uInvoiceStatus);
				continue;
		}


		if((fp=popen(cSendmail,"w"))==NULL)
		{
			fprintf(stderr,"Could not popen: %s for writing\n",cSendmail);
			mysql_free_result(res);
			mysql_close(&gMysql);
			exit(1);
		}

		fprintf(fp,"To: %s\n",field[5]);
		fprintf(fp,"From: %s\n",cFromEmailAddr);
		fprintf(fp,"Reply-to: %s\n",cFromEmailAddr);
		if(cInvoiceBccEmailAddr[0] && strcmp(cInvoiceBccEmailAddr,field[5]))
			fprintf(fp, "Bcc: %s\n",cInvoiceBccEmailAddr);
		fprintf(fp,"Subject: %s%s-%s-%s\n",cInvoiceSubject,
						cToday,field[0],field[1]);
		fprintf(fp,"MIME-Version: 1.0\nContent-Type: text/html;\n");
		fprintf(fp,"MIME-Version: 1.0\nContent-Type: text/html;\n\tcharset=\"iso-8859-1\"\n\n");
		template.name[0]="uInvoice";
		template.value[0]=field[0];
		//Needed for template {{funcPrintInvoiceItems}}
		sscanf(field[0],"%u",&guInvoice);

		template.name[1]="uClient";
		template.value[1]=field[1];

		template.name[2]="cFirstName";
		template.value[2]=field[2];

		template.name[3]="cLastName";
		template.value[3]=field[3];

		template.name[4]="cLabel";
		template.value[4]=field[4];
/*
00 tInvoice.uInvoice
01 tClient.uClient
02 tInvoice.cFirstName
03 tInvoice.cLastName
04 tClient.cLabel
05 tInvoice.cEmail
06 tInvoice.cAddr1
07 tInvoice.cAddr2
08 tInvoice.cCity
09 tInvoice.cState
10 tInvoice.cZip
11 tInvoice.cCountry
12 tInvoice.mTotal
13 tInvoice.mSH
14 tInvoice.mTax
15 tInvoice.cComments
*/

		template.name[5]="cEmail";
		template.value[5]=field[5];

		template.name[6]="cAddr1";
		template.value[6]=field[6];

		template.name[7]="cAddr2";
		template.value[7]=field[7];

		template.name[8]="cCity";
		template.value[8]=field[8];

		template.name[9]="cState";
		template.value[9]=field[9];

		template.name[10]="cZip";
		template.value[10]=field[10];

		template.name[11]="cCountry";
		template.value[11]=field[11];

		template.name[12]="mTotal";
		template.value[12]=field[12];

		template.name[13]="mSH";
		template.value[13]=field[13];

		template.name[14]="mTax";
		template.value[14]=field[14];

		template.name[15]="cComments";
		template.value[15]=field[15];

		template.name[16]="cDate";
		template.value[16]=cToday;

		template.name[17]="";//Close
		fileStructTemplate(fp,cInvoiceTemplate,&template);

		//Queue out of. Change to new invoice status.
		sprintf(gcQuery,"UPDATE tInvoice SET uQueueStatus=0,uInvoiceStatus=%u WHERE uInvoice=%s",uNewInvoiceStatus,field[0]);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			mysql_close(&gMysql);
			exit(1);
		}

		fprintf(fp,"\n");
		fclose(fp);

		uEmailed++;
	}
	mysql_free_result(res);


	sprintf(gcQuery,"Emailed: %u",uEmailed);
	UpdateJobStatus(uJob,unxsISP_Deployed,gcQuery);

	if(guDebug) printf("Done with EmailInvoicesFromJobQueue()\n");


}//void EmailInvoicesFromJobQueue(unsigned const uJob)


void TextConnectDb(void)
{
        mysql_init(&gMysql);
	if(!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
	{
		if (!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
		{
	                fprintf(stderr,"Database server unavailable.\n");
			mysql_close(&gMysql);
			exit(1);
		}
	}

}//end of TextConnectDb()


void AnalyzeTables(void)
{
	TextConnectDb();

	mysql_query(&gMysql,"ANALYZE TABLE tInstance");
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	mysql_query(&gMysql,"ANALYZE TABLE tProduct");
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	mysql_query(&gMysql,"ANALYZE TABLE tClient");
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	mysql_query(&gMysql,"ANALYZE TABLE tPeriod");
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	mysql_close(&gMysql);
	exit(0);

}//void AnalyzeTables(void)


//libtemplate.a required
void AppFunctions(char *cFunction)
{
	//Empty function
}//void AppFunctions(char *cFunction)

