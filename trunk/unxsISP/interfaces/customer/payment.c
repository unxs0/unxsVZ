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

	sprintf(gcQuery,"SELECT uOwner,cFirstName,cLastName,cEmail,cAddr1,cAddr2,cCity,cZip,cTelephone,cFax,mTotal "
			"FROM tInvoice WHERE uInvoice=%u",uInvoice);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlFatalError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
	{
		 sprintf(cPost,"requestType=BACKEND&merchant_id=109040000&trnCardOwner=%s&trnCardNumber=%s"
				"&trnExpMonth=%s&trnExpYear=%s&trnOrderNumber=%u&trnAmount=%s&ordEmailAddress=%s"
				"&ordName=%s&ordPhoneNumber=%s&ordAddress1=%s&ordAddress2=%s&ordCity=%s"
				"&ordProvince=%s&ordPostalCode=%s&ordCountry=CA"
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
			char *cp, *cp1;
			if((cp=strstr(cResult,"<string xmlns=\"http://tempuri.org/WebMerchant/MerchantService\">")))
			{
				//Error message case
				//Note, errors can also come in the form <string>Errcode</string>
				//Sample of correct response:
				/*
				<string>
				https://Chk.bancopopular.com/Checkout/payment?token=330AC6EC12E28642024E52CA76E80D12198DB61134426BF428CBD406240D17B693113D46&lang=en
				</string>
				*/
				cp+=strlen("<string xmlns=\"http://tempuri.org/WebMerchant/MerchantService\">");
				if((cp1=strstr(cp,"</string>"))) *cp1=0;
				sprintf(cRet,"%.255s",cp);
				//fprintf(fp,"cp=%s\n",cp);
			}
			//fclose(fp);
			return(cRet);
		}
	}
	
	return("");
}

