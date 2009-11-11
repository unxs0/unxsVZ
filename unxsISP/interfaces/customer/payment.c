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

char cResult[2048]={""}; //Contains html code with error info ;)


unsigned SubmitRequest(unsigned uInvoice)
{
	//Return 1 on approved transaction, 0 on failed

	MYSQL_RES *res;
	MYSQL_ROW field;
	char cPost[4096]={""};
	//Define for your payment gateway, response parsing may of course change 
	//depending on the payment gateway processor
	char *cHost="www.beanstream.com";
	char *cURL="/scripts/process_transaction.asp";
	unsigned uPort=443;
	unsigned uMerchantID=0;
	unsigned uApproved=0;
	unsigned uExpYear=0;
	unsigned uExpMonth=0;

	char cMerchantID[100]={""};

	GetConfiguration("cMerchantID",cMerchantID);
	sscanf(cMerchantID,"%u",&uMerchantID);
	
	if(!uMerchantID)
		htmlPlainTextError("Fatal: No merchant ID configured, can't continue");

	sprintf(gcQuery,"SELECT uOwner,cFirstName,cLastName,cEmail,cAddr1,cAddr2,cCity,cZip,cTelephone,cFax,mTotal,"
			"cCardNumber,uExpMonth,uExpYear,cCardName,cState  "
			"FROM tInvoice WHERE uInvoice=%u",uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[12],"%u",&uExpMonth);
		sscanf(field[13],"%u",&uExpYear);
		
		//Customer credit card expiry year. The year must be
		//entered as a number less than 50
		if(uExpYear>2000) uExpYear=uExpYear-2000;

		sprintf(cPost,"requestType=BACKEND&merchant_id=%u&trnCardOwner=%s&trnCardNumber=%s"
				"&trnExpMonth=%02u&trnExpYear=%u&trnOrderNumber=%u&trnAmount=%s&ordEmailAddress=%s"
				"&ordName=%s %s&ordPhoneNumber=%s&ordAddress1=%s&ordAddress2=%s&ordCity=%s"
				"&ordProvince=%s&ordPostalCode=%s&ordCountry=CA",
				uMerchantID
				,field[14]
				,field[11]
				,uExpMonth
				,uExpYear
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
		/*
		Sample aproved response:
		trnApproved=1&trnId=10003067&messageId=1&messageText=Approved&trnOrderNumber=E40089&aut
		hCode=TEST&errorType=N&errorFields=&responseType=T&trnAmount=10%2E00&trnDate=1%2F17%2F
		2008+11%3A36%3A34+AM&avsProcessed=0&avsId=0&avsResult=0&avsAddrMatch=0&avsPostalMatch=
		0&avsMessage=Address+Verification+not+performed+for+this+transaction%2E&rspCodeCav=0&rspCavR
		esult=0&rspCodeCredit1=0&rspCodeCredit2=0&rspCodeCredit3=0&rspCodeCredit4=0&rspCodeAddr1=0&r
		spCodeAddr2=0&rspCodeAddr3=0&rspCodeAddr4=0&rspCodeDob=0&rspCustomerDec=&trnType=P&pay
		mentMethod=CC&ref1=&ref2=&ref3=&ref4=&ref5=
		
		
		Sample declined response:
		trnApproved=0&trnId=10003068&messageId=16&messageText=Duplicate+Transaction+%2D+This+tran
		saction+has+already+been+approved&trnOrderNumber=E40089&authCode=&errorType=N&errorFields=
		&responseType=T&trnAmount=10%2E00&trnDate=1%2F17%2F2008+11%3A38%3A10+AM&avsProcesse
		d=0&avsId=0&avsResult=0&avsAddrMatch=0&avsPostalMatch=0&avsMessage=Address+Verification+not
		+performed+for+this+transaction%2E&rspCodeCav=0&rspCavResult=0&rspCodeCredit1=0&rspCodeCredi
		t2=0&rspCodeCredit3=0&rspCodeCredit4=0&rspCodeAddr1=0&rspCodeAddr2=0&rspCodeAddr3=0&rspCo
		deAddr4=0&rspCodeDob=0&rspCustomerDec=&trnType=P&paymentMethod=CC&ref1=&ref2=&ref3=&ref
		4=&ref5=
		*/
			if(strstr(cResult,"trnApproved=1"))
				uApproved=1;
			else
				uApproved=0;
			char *cp;
			char *cp2;
			if((cp=strstr(cResult,"messageText=")))
			{
				cp2=strchr(cp,'&');
				*cp2=0;
				plustospace(cp);
				unescape_url(cp);
				sprintf(cResult,"%s",cp+12);
			}
			return(uApproved);
		}
	}
	
	return(0); //should be never reached

}//unsigned SubmitRequest(unsigned uInvoice)

