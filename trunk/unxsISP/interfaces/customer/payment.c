/*
FILE
	payment.c
PURPOSE
	Provide the payment gateway support functions
	to the unxsISP interfaces
AUTHOR
	(C) 2009 Hugo Urquiza for Unixservice
*/

#include "interface.h"

unsigned https(char const *cHost,unsigned uPort,char const *cURL,char const *cPost,char *cResult);


char *SubmitRequest(unsigned uInvoice)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cPost[4096]={""};
	char *cHost="";
	char *cURL="";
	unsigned uPort=443;
	char cResult[2048]={""};
	static char cRet[256]={""};
	unsigned uMerchantID=0;
	char cMerchantID[100]={""};

	GetConfiguration("cMerchantID",cMerchantID);
	sscanf(cMerchantID,"%u",&uMerchantID);

	sprintf(gcQuery,"SELECT uOwner,cFirstName,cLastName,cEmail,cAddr1,cAddr2,cCity,cZip,cTelephone,cFax,mTotal,"
			"cCardNumber,uExpMonth,uExpYear,cCardName,cState  "
			"FROM tInvoice WHERE uInvoice=%u",uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
	{
		 sprintf(cPost,"requestType=BACKEND&merchant_id=%u&trnCardOwner=%s&trnCardNumber=%s"
				"&trnExpMonth=%s&trnExpYear=%s&trnOrderNumber=%u&trnAmount=%s&ordEmailAddress=%s"
				"&ordName=%s %s&ordPhoneNumber=%s&ordAddress1=%s&ordAddress2=%s&ordCity=%s"
				"&ordProvince=%s&ordPostalCode=%s&ordCountry=CA",
				uMerchantID
				,field[14]
				,field[11]
				,field[12]
				,field[13]
				,uInvoice
				,field[10]
				,field[3]
				,field[1]
				,field[2]
				,field[8]
				,field[4]
				,field[5]
				,field[6]
				,field[15]
				,field[7]
			);

		if(!https(cHost,uPort,cURL,cPost,cResult))
		{
			//char cBuffer[1024];
			//Debug only
			//FILE *fp;
			//if((fp=fopen("/tmp/cLPResult.txt","w")))
			//{
			//	fprintf(fp,"%.1023s\n\n********************%.10239s\n",cPost,cResult);
			//}
			//fclose(fp);
			return(cRet);
		}
	}
	
	return("");
}

