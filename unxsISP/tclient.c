/*
FILE
	tClient.c
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
AUTHOR
	(C) Gary Wallis and Hugo Urquiza 2001-2009
*/


#include "mysqlrad.h"

//Table Variables
//uClient: Primary Key
static unsigned uClient=0;
//cLabel: Short label
static char cLabel[33]={""};
//cFirstName: First Name
static char cFirstName[33]={""};
//cLastName: Last Name
static char cLastName[33]={""};
//cIDNumber: cIDNumber
static char cIDNumber[33]={""};
//cEmail: eMail
static char cEmail[101]={""};
//cAddr1: Street mailing address
static char cAddr1[101]={""};
//cAddr2: Street mailing address line 2
static char cAddr2[101]={""};
//cAddr3: Street mailing address line 3
static char cAddr3[101]={""};
//cCity: City
static char cCity[101]={""};
//cState: State, Province or Locality
static char cState[101]={""};
//cZip: Postal or Zip code
static char cZip[33]={""};
//cCountry: Country
static char cCountry[65]={""};
//uPayment: Preferred payment method
static unsigned uPayment=0;
static char cuPaymentPullDown[256]={""};
//cBankName: Bank name
static char cBankName[33]={""};
//cBranchName: Branch name
static char cBranchName[33]={""};
//cBranchCode: Branch code
static char cBranchCode[7]={""};
//cAccountHolder: Name of the account holder
static char cAccountHolder[101]={""};
//cAccountNumber: Account number
static char cAccountNumber[101]={""};
//uAccountType: Account type
static unsigned uAccountType=0;
static char cuAccountTypePullDown[256]={""};
//cCardType: Credit Card Type
static char cCardType[33]={""};
//cCardNumber: Credit Card Number
static char cCardNumber[33]={""};
//uExpMonth: Credit card expiration month
static unsigned uExpMonth=0;
//uExpYear: Credit card expiration year
static unsigned uExpYear=0;
//cCardName: Name as appears on credit card
static char cCardName[65]={""};
//cACHDebits: ABA routing number + account number
static char cACHDebits[65]={""};
//cShipName: Shipping address name
static char cShipName[101]={""};
//cShipAddr1: Shipping street address
static char cShipAddr1[101]={""};
//cShipAddr2: Shipping street address line 2
static char cShipAddr2[101]={""};
//cShipAddr3: Shipping street address line 3
static char cShipAddr3[101]={""};
//cShipCity: Shipping city
static char cShipCity[101]={""};
//cShipState: Shipping state or province
static char cShipState[101]={""};
//cShipZip: Shipping zip or postal code
static char cShipZip[33]={""};
//cShipCountry: Shipping country
static char cShipCountry[65]={""};
//cTelephone: Customer Telephone
static char cTelephone[33]={""};
//cMobile: Customer mobile phone number
static char cMobile[33]={""};
//cFax: Customer Fax
static char cFax[33]={""};
//mBalance: mBalance
static char mBalance[16]={"0.00"};
//mTotal: mTotal
static char mTotal[16]={"0.00"};
//cInfo: Unformatted info/address etc.
static char *cInfo={""};
//cLanguage: indicates customer language for invoice rendering
static char cLanguage[33]={""};
//uOwner: Record owner
static unsigned uOwner=0;
//uCreatedBy: uClient for last insert
static unsigned uCreatedBy=0;
//uCreatedDate: Unix seconds date last insert
static time_t uCreatedDate=0;
//uModBy: uClient for last update
static unsigned uModBy=0;
//uModDate: Unix seconds date last update
static time_t uModDate=0;



#define VAR_LIST_tClient "tClient.uClient,tClient.cLabel,tClient.cFirstName,tClient.cLastName,tClient.cIDNumber,tClient.cEmail,tClient.cAddr1,tClient.cAddr2,tClient.cAddr3,tClient.cCity,tClient.cState,tClient.cZip,tClient.cCountry,tClient.uPayment,tClient.cBankName,tClient.cBranchName,tClient.cBranchCode,tClient.cAccountHolder,tClient.cAccountNumber,tClient.uAccountType,tClient.cCardType,tClient.cCardNumber,tClient.uExpMonth,tClient.uExpYear,tClient.cCardName,tClient.cACHDebits,tClient.cShipName,tClient.cShipAddr1,tClient.cShipAddr2,tClient.cShipAddr3,tClient.cShipCity,tClient.cShipState,tClient.cShipZip,tClient.cShipCountry,tClient.cTelephone,tClient.cMobile,tClient.cFax,tClient.mBalance,tClient.mTotal,tClient.cInfo,tClient.cLanguage,tClient.uOwner,tClient.uCreatedBy,tClient.uCreatedDate,tClient.uModBy,tClient.uModDate"

 //Local only
void Insert_tClient(void);
void Update_tClient(char *cRowid);
void ProcesstClientListVars(pentry entries[], int x);

 //In tClientfunc.h file included below
void ExtProcesstClientVars(pentry entries[], int x);
void ExttClientCommands(pentry entries[], int x);
void ExttClientButtons(void);
void ExttClientNavBar(void);
void ExttClientGetHook(entry gentries[], int x);
void ExttClientSelect(void);
void ExttClientSelectRow(void);
void ExttClientListSelect(void);
void ExttClientListFilter(void);
void ExttClientAuxTable(void);

#include "tclientfunc.h"

 //Table Variables Assignment Function
void ProcesstClientVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uClient"))
			sscanf(entries[i].val,"%u",&uClient);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cFirstName"))
			sprintf(cFirstName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLastName"))
			sprintf(cLastName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cIDNumber"))
			sprintf(cIDNumber,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cEmail"))
			sprintf(cEmail,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cAddr1"))
			sprintf(cAddr1,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cAddr2"))
			sprintf(cAddr2,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cAddr3"))
			sprintf(cAddr3,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCity"))
			sprintf(cCity,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cState"))
			sprintf(cState,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cZip"))
			sprintf(cZip,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCountry"))
			sprintf(cCountry,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uPayment"))
			sscanf(entries[i].val,"%u",&uPayment);
		else if(!strcmp(entries[i].name,"cuPaymentPullDown"))
		{
			sprintf(cuPaymentPullDown,"%.255s",entries[i].val);
			uPayment=ReadPullDown("tPayment","cLabel",cuPaymentPullDown);
		}
		else if(!strcmp(entries[i].name,"cBankName"))
			sprintf(cBankName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cBranchName"))
			sprintf(cBranchName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cBranchCode"))
			sprintf(cBranchCode,"%.6s",entries[i].val);
		else if(!strcmp(entries[i].name,"cAccountHolder"))
			sprintf(cAccountHolder,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cAccountNumber"))
			sprintf(cAccountNumber,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"uAccountType"))
			sscanf(entries[i].val,"%u",&uAccountType);
		else if(!strcmp(entries[i].name,"cuAccountTypePullDown"))
		{
			sprintf(cuAccountTypePullDown,"%.255s",entries[i].val);
			uAccountType=ReadPullDown("tAccountType","cLabel",cuAccountTypePullDown);
		}
		else if(!strcmp(entries[i].name,"cCardType"))
			sprintf(cCardType,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCardNumber"))
			sprintf(cCardNumber,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uExpMonth"))
			sscanf(entries[i].val,"%u",&uExpMonth);
		else if(!strcmp(entries[i].name,"uExpYear"))
			sscanf(entries[i].val,"%u",&uExpYear);
		else if(!strcmp(entries[i].name,"cCardName"))
			sprintf(cCardName,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cACHDebits"))
			sprintf(cACHDebits,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipName"))
			sprintf(cShipName,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipAddr1"))
			sprintf(cShipAddr1,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipAddr2"))
			sprintf(cShipAddr2,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipAddr3"))
			sprintf(cShipAddr3,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipCity"))
			sprintf(cShipCity,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipState"))
			sprintf(cShipState,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipZip"))
			sprintf(cShipZip,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cShipCountry"))
			sprintf(cShipCountry,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cTelephone"))
			sprintf(cTelephone,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cMobile"))
			sprintf(cMobile,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cFax"))
			sprintf(cFax,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"mBalance"))
			sprintf(mBalance,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"mTotal"))
			sprintf(mTotal,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"cInfo"))
			cInfo=entries[i].val;
		else if(!strcmp(entries[i].name,"cLanguage"))
			sprintf(cLanguage,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uOwner"))
			sscanf(entries[i].val,"%u",&uOwner);
		else if(!strcmp(entries[i].name,"uCreatedBy"))
			sscanf(entries[i].val,"%u",&uCreatedBy);
		else if(!strcmp(entries[i].name,"uCreatedDate"))
			sscanf(entries[i].val,"%lu",&uCreatedDate);
		else if(!strcmp(entries[i].name,"uModBy"))
			sscanf(entries[i].val,"%u",&uModBy);
		else if(!strcmp(entries[i].name,"uModDate"))
			sscanf(entries[i].val,"%lu",&uModDate);

	}

	//After so we can overwrite form data if needed.
	ExtProcesstClientVars(entries,x);

}//ProcesstClientVars()


void ProcesstClientListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uClient);
                        guMode=2002;
                        tClient("");
                }
        }
}//void ProcesstClientListVars(pentry entries[], int x)


int tClientCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttClientCommands(entries,x);

	if(!strcmp(gcFunction,"tClientTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tClientList();
		}

		//Default
		ProcesstClientVars(entries,x);
		tClient("");
	}
	else if(!strcmp(gcFunction,"tClientList"))
	{
		ProcessControlVars(entries,x);
		ProcesstClientListVars(entries,x);
		tClientList();
	}

	return(0);

}//tClientCommands()


void tClient(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttClientSelectRow();
		else
			ExttClientSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetClient();
				unxsISP("New tClient table created");
                	}
			else
			{
				htmlPlainTextError(mysql_error(&gMysql));
			}
        	}

		res=mysql_store_result(&gMysql);
		if((guI=mysql_num_rows(res)))
		{
			if(guMode==6)
			{
			sprintf(gcQuery,"SELECT _rowid FROM tClient WHERE uClient=%u"
						,uClient);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			sscanf(field[0],"%u",&uClient);
			sprintf(cLabel,"%.32s",field[1]);
			sprintf(cFirstName,"%.32s",field[2]);
			sprintf(cLastName,"%.32s",field[3]);
			sprintf(cIDNumber,"%.32s",field[4]);
			sprintf(cEmail,"%.100s",field[5]);
			sprintf(cAddr1,"%.100s",field[6]);
			sprintf(cAddr2,"%.100s",field[7]);
			sprintf(cAddr3,"%.100s",field[8]);
			sprintf(cCity,"%.100s",field[9]);
			sprintf(cState,"%.100s",field[10]);
			sprintf(cZip,"%.32s",field[11]);
			sprintf(cCountry,"%.64s",field[12]);
			sscanf(field[13],"%u",&uPayment);
			sprintf(cBankName,"%.32s",field[14]);
			sprintf(cBranchName,"%.32s",field[15]);
			sprintf(cBranchCode,"%.6s",field[16]);
			sprintf(cAccountHolder,"%.100s",field[17]);
			sprintf(cAccountNumber,"%.100s",field[18]);
			sscanf(field[19],"%u",&uAccountType);
			sprintf(cCardType,"%.32s",field[20]);
			sprintf(cCardNumber,"%.32s",field[21]);
			sscanf(field[22],"%u",&uExpMonth);
			sscanf(field[23],"%u",&uExpYear);
			sprintf(cCardName,"%.64s",field[24]);
			sprintf(cACHDebits,"%.64s",field[25]);
			sprintf(cShipName,"%.100s",field[26]);
			sprintf(cShipAddr1,"%.100s",field[27]);
			sprintf(cShipAddr2,"%.100s",field[28]);
			sprintf(cShipAddr3,"%.100s",field[29]);
			sprintf(cShipCity,"%.100s",field[30]);
			sprintf(cShipState,"%.100s",field[31]);
			sprintf(cShipZip,"%.32s",field[32]);
			sprintf(cShipCountry,"%.64s",field[33]);
			sprintf(cTelephone,"%.32s",field[34]);
			sprintf(cMobile,"%.32s",field[35]);
			sprintf(cFax,"%.32s",field[36]);
			sprintf(mBalance,"%.32s",field[37]);
			sprintf(mTotal,"%.32s",field[38]);
			cInfo=field[39];
			sprintf(cLanguage,"%.32s",field[40]);
			sscanf(field[41],"%u",&uOwner);
			sscanf(field[42],"%u",&uCreatedBy);
			sscanf(field[43],"%lu",&uCreatedDate);
			sscanf(field[44],"%u",&uModBy);
			sscanf(field[45],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tClient",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tClientTools>");
	printf("<input type=hidden name=gluRowid value=%lu>",gluRowid);
	if(guI)
	{
		if(guMode==6)
			//printf(" Found");
			printf(LANG_NBR_FOUND);
		else if(guMode==5)
			//printf(" Modified");
			printf(LANG_NBR_MODIFIED);
		else if(guMode==4)
			//printf(" New");
			printf(LANG_NBR_NEW);
		printf(LANG_NBRF_SHOWING,gluRowid,guI);
	}
	else
	{
		if(!cResult[0])
		//printf(" No records found");
		printf(LANG_NBR_NORECS);
	}
	if(cResult[0]) printf("%s",cResult);
	printf("</td></tr>");
	printf("<tr><td valign=top width=25%%>");

        ExttClientButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tClient Record Data",100);

	if(guMode==2000 || guMode==2002)
		tClientInput(1);
	else
		tClientInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttClientAuxTable();

	Footer_ism3();

}//end of tClient();


void tClientInput(unsigned uMode)
{

//uClient
	OpenRow(LANG_FL_tClient_uClient,"black");
	printf("<input title='%s' type=text name=uClient value=%u size=16 maxlength=10 "
,LANG_FT_tClient_uClient,uClient);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uClient value=%u >\n",uClient);
	}
//cLabel
	OpenRow(LANG_FL_tClient_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//cFirstName
	OpenRow(LANG_FL_tClient_cFirstName,"black");
	printf("<input title='%s' type=text name=cFirstName value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cFirstName,EncodeDoubleQuotes(cFirstName));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cFirstName value=\"%s\">\n",EncodeDoubleQuotes(cFirstName));
	}
//cLastName
	OpenRow(LANG_FL_tClient_cLastName,"black");
	printf("<input title='%s' type=text name=cLastName value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cLastName,EncodeDoubleQuotes(cLastName));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLastName value=\"%s\">\n",EncodeDoubleQuotes(cLastName));
	}
//cIDNumber
	OpenRow(LANG_FL_tClient_cIDNumber,"black");
	printf("<input title='%s' type=text name=cIDNumber value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cIDNumber,EncodeDoubleQuotes(cIDNumber));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cIDNumber value=\"%s\">\n",EncodeDoubleQuotes(cIDNumber));
	}
//cEmail
	OpenRow(LANG_FL_tClient_cEmail,"black");
	printf("<input title='%s' type=text name=cEmail value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cEmail,EncodeDoubleQuotes(cEmail));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cEmail value=\"%s\">\n",EncodeDoubleQuotes(cEmail));
	}
//cAddr1
	OpenRow(LANG_FL_tClient_cAddr1,"black");
	printf("<input title='%s' type=text name=cAddr1 value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cAddr1,EncodeDoubleQuotes(cAddr1));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cAddr1 value=\"%s\">\n",EncodeDoubleQuotes(cAddr1));
	}
//cAddr2
	OpenRow(LANG_FL_tClient_cAddr2,"black");
	printf("<input title='%s' type=text name=cAddr2 value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cAddr2,EncodeDoubleQuotes(cAddr2));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cAddr2 value=\"%s\">\n",EncodeDoubleQuotes(cAddr2));
	}
//cAddr3
	OpenRow(LANG_FL_tClient_cAddr3,"black");
	printf("<input title='%s' type=text name=cAddr3 value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cAddr3,EncodeDoubleQuotes(cAddr3));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cAddr3 value=\"%s\">\n",EncodeDoubleQuotes(cAddr3));
	}
//cCity
	OpenRow(LANG_FL_tClient_cCity,"black");
	printf("<input title='%s' type=text name=cCity value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cCity,EncodeDoubleQuotes(cCity));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cCity value=\"%s\">\n",EncodeDoubleQuotes(cCity));
	}
//cState
	OpenRow(LANG_FL_tClient_cState,"black");
	printf("<input title='%s' type=text name=cState value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cState,EncodeDoubleQuotes(cState));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cState value=\"%s\">\n",EncodeDoubleQuotes(cState));
	}
//cZip
	OpenRow(LANG_FL_tClient_cZip,"black");
	printf("<input title='%s' type=text name=cZip value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cZip,EncodeDoubleQuotes(cZip));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cZip value=\"%s\">\n",EncodeDoubleQuotes(cZip));
	}
//cCountry
	OpenRow(LANG_FL_tClient_cCountry,"black");
	printf("<input title='%s' type=text name=cCountry value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tClient_cCountry,EncodeDoubleQuotes(cCountry));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cCountry value=\"%s\">\n",EncodeDoubleQuotes(cCountry));
	}
//uPayment
	OpenRow(LANG_FL_tClient_uPayment,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tPayment;cuPaymentPullDown","cLabel","cLabel",uPayment,1);
	else
		tTablePullDown("tPayment;cuPaymentPullDown","cLabel","cLabel",uPayment,0);
//cBankName
	OpenRow(LANG_FL_tClient_cBankName,"black");
	printf("<input title='%s' type=text name=cBankName value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cBankName,EncodeDoubleQuotes(cBankName));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cBankName value=\"%s\">\n",EncodeDoubleQuotes(cBankName));
	}
//cBranchName
	OpenRow(LANG_FL_tClient_cBranchName,"black");
	printf("<input title='%s' type=text name=cBranchName value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cBranchName,EncodeDoubleQuotes(cBranchName));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cBranchName value=\"%s\">\n",EncodeDoubleQuotes(cBranchName));
	}
//cBranchCode
	OpenRow(LANG_FL_tClient_cBranchCode,"black");
	printf("<input title='%s' type=text name=cBranchCode value=\"%s\" size=40 maxlength=6 "
,LANG_FT_tClient_cBranchCode,EncodeDoubleQuotes(cBranchCode));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cBranchCode value=\"%s\">\n",EncodeDoubleQuotes(cBranchCode));
	}
//cAccountHolder
	OpenRow(LANG_FL_tClient_cAccountHolder,"black");
	printf("<input title='%s' type=text name=cAccountHolder value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cAccountHolder,EncodeDoubleQuotes(cAccountHolder));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cAccountHolder value=\"%s\">\n",EncodeDoubleQuotes(cAccountHolder));
	}
//cAccountNumber
	OpenRow(LANG_FL_tClient_cAccountNumber,"black");
	printf("<input title='%s' type=text name=cAccountNumber value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cAccountNumber,EncodeDoubleQuotes(cAccountNumber));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cAccountNumber value=\"%s\">\n",EncodeDoubleQuotes(cAccountNumber));
	}
//uAccountType
	OpenRow(LANG_FL_tClient_uAccountType,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDown("tAccountType;cuAccountTypePullDown","cLabel","cLabel",uAccountType,1);
	else
		tTablePullDown("tAccountType;cuAccountTypePullDown","cLabel","cLabel",uAccountType,0);
//cCardType
	OpenRow(LANG_FL_tClient_cCardType,"black");
	printf("<input title='%s' type=text name=cCardType value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cCardType,EncodeDoubleQuotes(cCardType));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cCardType value=\"%s\">\n",EncodeDoubleQuotes(cCardType));
	}
//cCardNumber
	OpenRow(LANG_FL_tClient_cCardNumber,"black");
	printf("<input title='%s' type=text name=cCardNumber value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cCardNumber,EncodeDoubleQuotes(cCardNumber));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cCardNumber value=\"%s\">\n",EncodeDoubleQuotes(cCardNumber));
	}
//uExpMonth
	OpenRow(LANG_FL_tClient_uExpMonth,"black");
	printf("<input title='%s' type=text name=uExpMonth value=%u size=16 maxlength=10 "
,LANG_FT_tClient_uExpMonth,uExpMonth);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uExpMonth value=%u >\n",uExpMonth);
	}
//uExpYear
	OpenRow(LANG_FL_tClient_uExpYear,"black");
	printf("<input title='%s' type=text name=uExpYear value=%u size=16 maxlength=10 "
,LANG_FT_tClient_uExpYear,uExpYear);
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uExpYear value=%u >\n",uExpYear);
	}
//cCardName
	OpenRow(LANG_FL_tClient_cCardName,"black");
	printf("<input title='%s' type=text name=cCardName value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tClient_cCardName,EncodeDoubleQuotes(cCardName));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cCardName value=\"%s\">\n",EncodeDoubleQuotes(cCardName));
	}
//cACHDebits
	OpenRow(LANG_FL_tClient_cACHDebits,"black");
	printf("<input title='%s' type=text name=cACHDebits value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tClient_cACHDebits,EncodeDoubleQuotes(cACHDebits));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cACHDebits value=\"%s\">\n",EncodeDoubleQuotes(cACHDebits));
	}
//cShipName
	OpenRow(LANG_FL_tClient_cShipName,"black");
	printf("<input title='%s' type=text name=cShipName value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cShipName,EncodeDoubleQuotes(cShipName));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipName value=\"%s\">\n",EncodeDoubleQuotes(cShipName));
	}
//cShipAddr1
	OpenRow(LANG_FL_tClient_cShipAddr1,"black");
	printf("<input title='%s' type=text name=cShipAddr1 value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cShipAddr1,EncodeDoubleQuotes(cShipAddr1));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipAddr1 value=\"%s\">\n",EncodeDoubleQuotes(cShipAddr1));
	}
//cShipAddr2
	OpenRow(LANG_FL_tClient_cShipAddr2,"black");
	printf("<input title='%s' type=text name=cShipAddr2 value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cShipAddr2,EncodeDoubleQuotes(cShipAddr2));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipAddr2 value=\"%s\">\n",EncodeDoubleQuotes(cShipAddr2));
	}
//cShipAddr3
	OpenRow(LANG_FL_tClient_cShipAddr3,"black");
	printf("<input title='%s' type=text name=cShipAddr3 value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cShipAddr3,EncodeDoubleQuotes(cShipAddr3));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipAddr3 value=\"%s\">\n",EncodeDoubleQuotes(cShipAddr3));
	}
//cShipCity
	OpenRow(LANG_FL_tClient_cShipCity,"black");
	printf("<input title='%s' type=text name=cShipCity value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cShipCity,EncodeDoubleQuotes(cShipCity));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipCity value=\"%s\">\n",EncodeDoubleQuotes(cShipCity));
	}
//cShipState
	OpenRow(LANG_FL_tClient_cShipState,"black");
	printf("<input title='%s' type=text name=cShipState value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tClient_cShipState,EncodeDoubleQuotes(cShipState));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipState value=\"%s\">\n",EncodeDoubleQuotes(cShipState));
	}
//cShipZip
	OpenRow(LANG_FL_tClient_cShipZip,"black");
	printf("<input title='%s' type=text name=cShipZip value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cShipZip,EncodeDoubleQuotes(cShipZip));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipZip value=\"%s\">\n",EncodeDoubleQuotes(cShipZip));
	}
//cShipCountry
	OpenRow(LANG_FL_tClient_cShipCountry,"black");
	printf("<input title='%s' type=text name=cShipCountry value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tClient_cShipCountry,EncodeDoubleQuotes(cShipCountry));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cShipCountry value=\"%s\">\n",EncodeDoubleQuotes(cShipCountry));
	}
//cTelephone
	OpenRow(LANG_FL_tClient_cTelephone,"black");
	printf("<input title='%s' type=text name=cTelephone value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cTelephone,EncodeDoubleQuotes(cTelephone));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cTelephone value=\"%s\">\n",EncodeDoubleQuotes(cTelephone));
	}
//cMobile
	OpenRow(LANG_FL_tClient_cMobile,"black");
	printf("<input title='%s' type=text name=cMobile value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cMobile,EncodeDoubleQuotes(cMobile));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cMobile value=\"%s\">\n",EncodeDoubleQuotes(cMobile));
	}
//cFax
	OpenRow(LANG_FL_tClient_cFax,"black");
	printf("<input title='%s' type=text name=cFax value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cFax,EncodeDoubleQuotes(cFax));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cFax value=\"%s\">\n",EncodeDoubleQuotes(cFax));
	}
//mBalance
	OpenRow(LANG_FL_tClient_mBalance,"black");
	printf("<input title='%s' type=text name=mBalance value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tClient_mBalance,EncodeDoubleQuotes(mBalance));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mBalance value=\"%s\">\n",EncodeDoubleQuotes(mBalance));
	}
//mTotal
	OpenRow(LANG_FL_tClient_mTotal,"black");
	printf("<input title='%s' type=text name=mTotal value=\"%s\" size=20 maxlength=15 "
,LANG_FT_tClient_mTotal,EncodeDoubleQuotes(mTotal));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=mTotal value=\"%s\">\n",EncodeDoubleQuotes(mTotal));
	}
//cInfo
	OpenRow(LANG_FL_tClient_cInfo,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cInfo "
,LANG_FT_tClient_cInfo);
	if(guPermLevel>=12 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cInfo);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cInfo);
		printf("<input type=hidden name=cInfo value=\"%s\" >\n",EncodeDoubleQuotes(cInfo));
	}
//cLanguage
	OpenRow(LANG_FL_tClient_cLanguage,"black");
	printf("<input title='%s' type=text name=cLanguage value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tClient_cLanguage,EncodeDoubleQuotes(cLanguage));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLanguage value=\"%s\">\n",EncodeDoubleQuotes(cLanguage));
	}
//uOwner
	OpenRow(LANG_FL_tClient_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tClient_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tClient_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tClient_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tClient_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tClientInput(unsigned uMode)


void NewtClient(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uClient FROM " TCLIENT " WHERE uClient=%u",uClient);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tClient("<blink>Record already exists");
		tClient(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tClient();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uClient=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate(TCLIENT,uClient);
	unxsISPLog(uClient,TCLIENT,"New");

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uClient);
	tClient(gcQuery);
	}

}//NewtClient(unsigned uMode)


void DeletetClient(void)
{
	sprintf(gcQuery,"DELETE FROM " TCLIENT " WHERE uClient=%u",uClient);
	macro_mySQLQueryHTMLError;
	//tClient("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsISPLog(uClient,TCLIENT,"Del");
		tClient(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsISPLog(uClient,TCLIENT,"DelError");
		tClient(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetClient(void)


void Insert_tClient(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tClient SET uClient=%u,cLabel='%s',cFirstName='%s',cLastName='%s',"
		"cIDNumber='%s',cEmail='%s',cAddr1='%s',cAddr2='%s',cAddr3='%s',cCity='%s',cState='%s',"
		"cZip='%s',cCountry='%s',uPayment=%u,cBankName='%s',cBranchName='%s',cBranchCode='%s',"
		"cAccountHolder='%s',cAccountNumber='%s',uAccountType=%u,cCardType='%s',cCardNumber='%s',"
		"uExpMonth=%u,uExpYear=%u,cCardName='%s',cACHDebits='%s',cShipName='%s',cShipAddr1='%s',"
		"cShipAddr2='%s',cShipAddr3='%s',cShipCity='%s',cShipState='%s',cShipZip='%s',cShipCountry='%s',"
		"cTelephone='%s',cMobile='%s',cFax='%s',cPasswd='%s',mBalance='%s',mTotal='%s',cInfo='%s',cLanguage='%s',uOwner=%u,"
		"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uClient
			,TextAreaSave(cLabel)
			,TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cIDNumber)
			,TextAreaSave(cEmail)
			,TextAreaSave(cAddr1)
			,TextAreaSave(cAddr2)
			,TextAreaSave(cAddr3)
			,TextAreaSave(cCity)
			,TextAreaSave(cState)
			,TextAreaSave(cZip)
			,TextAreaSave(cCountry)
			,uPayment
			,TextAreaSave(cBankName)
			,TextAreaSave(cBranchName)
			,TextAreaSave(cBranchCode)
			,TextAreaSave(cAccountHolder)
			,TextAreaSave(cAccountNumber)
			,uAccountType
			,TextAreaSave(cCardType)
			,TextAreaSave(cCardNumber)
			,uExpMonth
			,uExpYear
			,TextAreaSave(cCardName)
			,TextAreaSave(cACHDebits)
			,TextAreaSave(cShipName)
			,TextAreaSave(cShipAddr1)
			,TextAreaSave(cShipAddr2)
			,TextAreaSave(cShipAddr3)
			,TextAreaSave(cShipCity)
			,TextAreaSave(cShipState)
			,TextAreaSave(cShipZip)
			,TextAreaSave(cShipCountry)
			,TextAreaSave(cTelephone)
			,TextAreaSave(cMobile)
			,TextAreaSave(cFax)
			,TextAreaSave(cPasswd)
			,TextAreaSave(mBalance)
			,TextAreaSave(mTotal)
			,TextAreaSave(cInfo)
			,TextAreaSave(cLanguage)
			,uOwner
			,uCreatedBy
			);

	macro_mySQLQueryHTMLError;

}//void Insert_tClient(void)


void Update_tClient(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tClient SET uClient=%u,cLabel='%s',cFirstName='%s',cLastName='%s',"
		"cIDNumber='%s',cEmail='%s',cAddr1='%s',cAddr2='%s',cAddr3='%s',cCity='%s',cState='%s',"
		"cZip='%s',cCountry='%s',uPayment=%u,cBankName='%s',cBranchName='%s',cBranchCode='%s',"
		"cAccountHolder='%s',cAccountNumber='%s',uAccountType=%u,cCardType='%s',cCardNumber='%s',"
		"uExpMonth=%u,uExpYear=%u,cCardName='%s',cACHDebits='%s',cShipName='%s',cShipAddr1='%s',"
		"cShipAddr2='%s',cShipAddr3='%s',cShipCity='%s',cShipState='%s',cShipZip='%s',"
		"cShipCountry='%s',cTelephone='%s',cMobile='%s',cFax='%s',cPasswd='%s',mBalance='%s',"
		"mTotal='%s',cInfo='%s',cLanguage='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uClient
			,TextAreaSave(cLabel)
			,TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cIDNumber)
			,TextAreaSave(cEmail)
			,TextAreaSave(cAddr1)
			,TextAreaSave(cAddr2)
			,TextAreaSave(cAddr3)
			,TextAreaSave(cCity)
			,TextAreaSave(cState)
			,TextAreaSave(cZip)
			,TextAreaSave(cCountry)
			,uPayment
			,TextAreaSave(cBankName)
			,TextAreaSave(cBranchName)
			,TextAreaSave(cBranchCode)
			,TextAreaSave(cAccountHolder)
			,TextAreaSave(cAccountNumber)
			,uAccountType
			,TextAreaSave(cCardType)
			,TextAreaSave(cCardNumber)
			,uExpMonth
			,uExpYear
			,TextAreaSave(cCardName)
			,TextAreaSave(cACHDebits)
			,TextAreaSave(cShipName)
			,TextAreaSave(cShipAddr1)
			,TextAreaSave(cShipAddr2)
			,TextAreaSave(cShipAddr3)
			,TextAreaSave(cShipCity)
			,TextAreaSave(cShipState)
			,TextAreaSave(cShipZip)
			,TextAreaSave(cShipCountry)
			,TextAreaSave(cTelephone)
			,TextAreaSave(cMobile)
			,TextAreaSave(cFax)
			,TextAreaSave(cPasswd)
			,TextAreaSave(mBalance)
			,TextAreaSave(mTotal)
			,TextAreaSave(cInfo)
			,TextAreaSave(cLanguage)
			,uModBy
			,cRowid);

	macro_mySQLQueryHTMLError;

}//void Update_tClient(void)


void ModtClient(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	sprintf(gcQuery,"SELECT uClient,uModDate FROM " TCLIENT " WHERE uClient=%u",uClient);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tClient("<blink>Record does not exist");
	if(i<1) tClient(LANG_NBR_RECNOTEXIST);
	//if(i>1) tClient("<blink>Multiple rows!");
	if(i>1) tClient(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tClient(LANG_NBR_EXTMOD);

	Update_tClient(field[0]);
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate(TCLIENT,uClient);
	unxsISPLog(uClient,TCLIENT,"Mod");
	tClient(gcQuery);

}//ModtClient(void)


void tClientList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttClientListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tClientList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttClientListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uClient<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>cFirstName<td><font face=arial,helvetica color=white>cLastName<td><font face=arial,helvetica color=white>cIDNumber<td><font face=arial,helvetica color=white>cEmail<td><font face=arial,helvetica color=white>cAddr1<td><font face=arial,helvetica color=white>cAddr2<td><font face=arial,helvetica color=white>cAddr3<td><font face=arial,helvetica color=white>cCity<td><font face=arial,helvetica color=white>cState<td><font face=arial,helvetica color=white>cZip<td><font face=arial,helvetica color=white>cCountry<td><font face=arial,helvetica color=white>uPayment<td><font face=arial,helvetica color=white>cBankName<td><font face=arial,helvetica color=white>cBranchName<td><font face=arial,helvetica color=white>cBranchCode<td><font face=arial,helvetica color=white>cAccountHolder<td><font face=arial,helvetica color=white>cAccountNumber<td><font face=arial,helvetica color=white>uAccountType<td><font face=arial,helvetica color=white>cCardType<td><font face=arial,helvetica color=white>cCardNumber<td><font face=arial,helvetica color=white>uExpMonth<td><font face=arial,helvetica color=white>uExpYear<td><font face=arial,helvetica color=white>cCardName<td><font face=arial,helvetica color=white>cACHDebits<td><font face=arial,helvetica color=white>cShipName<td><font face=arial,helvetica color=white>cShipAddr1<td><font face=arial,helvetica color=white>cShipAddr2<td><font face=arial,helvetica color=white>cShipAddr3<td><font face=arial,helvetica color=white>cShipCity<td><font face=arial,helvetica color=white>cShipState<td><font face=arial,helvetica color=white>cShipZip<td><font face=arial,helvetica color=white>cShipCountry<td><font face=arial,helvetica color=white>cTelephone<td><font face=arial,helvetica color=white>cMobile<td><font face=arial,helvetica color=white>cFax<td><font face=arial,helvetica color=white>mBalance<td><font face=arial,helvetica color=white>mTotal<td><font face=arial,helvetica color=white>cInfo<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



	mysql_data_seek(res,guStart-1);

	for(guN=0;guN<(guEnd-guStart+1);guN++)
	{
		field=mysql_fetch_row(res);
		if(!field)
		{
			printf("<tr><td><font face=arial,helvetica>End of data</table>");
			Footer_ism3();
		}
		if(guN % 2)
			printf("<tr bgcolor=#BBE1D3>");
		else
			printf("<tr>");
		time_t luTime42=strtoul(field[42],NULL,10);
		char cBuf42[32];
		if(luTime42)
			ctime_r(&luTime42,cBuf42);
		else
			sprintf(cBuf42,"---");
		time_t luTime44=strtoul(field[44],NULL,10);
		char cBuf44[32];
		if(luTime44)
			ctime_r(&luTime44,cBuf44);
		else
			sprintf(cBuf44,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,field[3]
			,field[4]
			,field[5]
			,field[6]
			,field[7]
			,field[8]
			,field[9]
			,field[10]
			,field[11]
			,field[12]
			,ForeignKey("tPayment","cLabel",strtoul(field[13],NULL,10))
			,field[14]
			,field[15]
			,field[16]
			,field[17]
			,field[18]
			,ForeignKey("tAccountType","cLabel",strtoul(field[19],NULL,10))
			,field[20]
			,field[21]
			,field[22]
			,field[23]
			,field[24]
			,field[25]
			,field[26]
			,field[27]
			,field[28]
			,field[29]
			,field[30]
			,field[31]
			,field[32]
			,field[33]
			,field[34]
			,field[35]
			,field[36]
			,field[37]
			,field[38]
			,field[39]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[40],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[41],NULL,10))
			,cBuf42
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[43],NULL,10))
			,cBuf44);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tClientList()


void CreatetClient(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tClient ( cLabel VARCHAR(32) NOT NULL DEFAULT '', uClient INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cPasswd VARCHAR(20) NOT NULL DEFAULT '', cFirstName VARCHAR(32) NOT NULL DEFAULT '', cLastName VARCHAR(32) NOT NULL DEFAULT '', cEmail VARCHAR(100) NOT NULL DEFAULT '',index (cEmail), cAddr1 VARCHAR(100) NOT NULL DEFAULT '', cAddr2 VARCHAR(100) NOT NULL DEFAULT '', cCity VARCHAR(100) NOT NULL DEFAULT '', cState VARCHAR(100) NOT NULL DEFAULT '', cZip VARCHAR(32) NOT NULL DEFAULT '', cCountry VARCHAR(64) NOT NULL DEFAULT '', cCardType VARCHAR(32) NOT NULL DEFAULT '', cCardNumber VARCHAR(32) NOT NULL DEFAULT '', uExpMonth INT UNSIGNED NOT NULL DEFAULT 0, uExpYear INT UNSIGNED NOT NULL DEFAULT 0, cCardName VARCHAR(64) NOT NULL DEFAULT '', cShipName VARCHAR(100) NOT NULL DEFAULT '', cShipAddr1 VARCHAR(100) NOT NULL DEFAULT '', cShipAddr2 VARCHAR(100) NOT NULL DEFAULT '', cShipCity VARCHAR(100) NOT NULL DEFAULT '', cShipState VARCHAR(100) NOT NULL DEFAULT '', cShipZip VARCHAR(32) NOT NULL DEFAULT '', cShipCountry VARCHAR(64) NOT NULL DEFAULT '', cTelephone VARCHAR(32) NOT NULL DEFAULT '', cFax VARCHAR(32) NOT NULL DEFAULT '', cACHDebits VARCHAR(64) NOT NULL DEFAULT '', uPayment INT UNSIGNED NOT NULL DEFAULT 0, cMobile VARCHAR(32) NOT NULL DEFAULT '', cBankName VARCHAR(32) NOT NULL DEFAULT '', cBranchName VARCHAR(32) NOT NULL DEFAULT '', cBranchCode VARCHAR(6) NOT NULL DEFAULT '', cAccountHolder VARCHAR(100) NOT NULL DEFAULT '', cAccountNumber VARCHAR(100) NOT NULL DEFAULT '', uAccountType INT UNSIGNED NOT NULL DEFAULT 0, mBalance DECIMAL(10,2) NOT NULL DEFAULT 0, mTotal DECIMAL(10,2) NOT NULL DEFAULT 0, cIDNumber VARCHAR(32) NOT NULL DEFAULT '', cAddr3 VARCHAR(100) NOT NULL DEFAULT '', cShipAddr3 VARCHAR(100) NOT NULL DEFAULT '', cInfo TEXT NOT NULL DEFAULT '', cCode VARCHAR(32) NOT NULL DEFAULT '', cLanguage VARCHAR(32) NOT NULL DEFAULT 'English' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetClient()

