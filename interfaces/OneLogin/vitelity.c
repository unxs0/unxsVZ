/*
FILE 
	vitelity.c
	$Id$
AUTHOR/LEGAL
	(C) 2015 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	OneLogin program file.
	Vitelity DID ordering functions.
*/

#include "interface.h"
#include <curl/curl.h>

#define cVitelityPwd "123123"
#define cVitelityLogin "johndoe"

//TOC
void htmlVitelityAvailStatesSelect(void *ptr, size_t size, size_t nmemb, void *stream);
void GetVitelityAvailStates(void);
void htmlVitelityAvailRatecenterSelect(void *ptr, size_t size, size_t nmemb, void *stream);
void GetVitelityAvailRatecentersPerState(char *cState);
void htmlVitelityAvailLocalDIDs(void *ptr, size_t size, size_t nmemb, void *stream);
void GetVitelityAvailLocalDIDsPerRatecenter(char *cState,char *cRatecenter);

void htmlVitelityAvailStateSelect(void *ptr, size_t size, size_t nmemb, void *stream)
{
	//parse
	//find first [[
	//then until last [[
	//line by line STATE 2 letter code
	printf("<select name=AvailStateSelect >\n");
	printf("<option>---</option>");
	char *cpBuffer;
	if((cpBuffer=strstr((char *)ptr,"[["))!=NULL)
	{
		//printf("%u %s\n",(unsigned)nmemb,cpBuffer+2);
		char *cpLineStart;
		int register i;
		cpLineStart=cpBuffer+2;
		for(i=2;cpBuffer[i]!=0 && i<nmemb;i++)
		{
			if(cpBuffer[i]=='\n' || cpBuffer[i]=='\r')
			{
				cpBuffer[i]=0;
				printf("<option>%s</option>",cpLineStart);
				cpLineStart=cpBuffer+i+1;
			}
		}
	}
	printf("</select>\n");
}//void htmlVitelityAvailStateSelect()


void GetVitelityAvailStates(void)
{
	CURL *curl;
	curl=curl_easy_init();
	if(curl)
	{
		curl_easy_setopt(curl,CURLOPT_URL,"api.vitelity.net/api.php?login="
					""cVitelityLogin"&pass="cVitelityPwd"&cmd=listavailstates&provider=l3");
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,htmlVitelityAvailStateSelect);
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
}//void GetVitelityAvailStates(void)


void htmlVitelityAvailRatecenterSelect(void *ptr, size_t size, size_t nmemb, void *stream)
{
	//parse
	//find first [[
	//then until last [[
	//line by line STATE 2 letter code
	printf("<select name=AvailRatecenterSelect >\n");
	printf("<option>---</option>");
	char *cpBuffer;
	if((cpBuffer=strstr((char *)ptr,"[["))!=NULL)
	{
		//printf("%u %s\n",(unsigned)nmemb,cpBuffer+2);
		char *cpLineStart;
		int register i;
		cpLineStart=cpBuffer+2;
		for(i=2;cpBuffer[i]!=0 && i<nmemb;i++)
		{
			if(cpBuffer[i]=='\n' || cpBuffer[i]=='\r')
			{
				cpBuffer[i]=0;
				printf("<option>%s</option>",cpLineStart);
				cpLineStart=cpBuffer+i+1;
			}
		}
	}
	printf("</select>\n");
}//void htmlVitelityAvailRatecenterSelect()


void GetVitelityAvailRatecentersPerState(char *cState)
{
	CURL *curl;
	curl=curl_easy_init();
	if(curl)
	{
		char cURL[256]={""};
		sprintf(cURL,"api.vitelity.net/api.php?login="cVitelityLogin"&pass="cVitelityPwd"&cmd=listavailratecenters&state=%.2s",cState);
		curl_easy_setopt(curl,CURLOPT_URL,cURL);
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,htmlVitelityAvailRatecenterSelect);
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
}//void GetVitelityAvailRatecentersPerState()


//testing
//gcc -Wall vitelity.c -o vitelity -L/usr/lib64 -lcurl
int main(void)
{
	char cState[3]={"CA"};
	char cRatecenter[64]={"ALHAMBRA"};
	//GetVitelityAvailStates();
	//GetVitelityAvailRatecentersPerState(cState);
	GetVitelityAvailLocalDIDsPerRatecenter(cState,cRatecenter);
	return(0);

}//int main(void)

void htmlVitelityAvailLocalDIDs(void *ptr, size_t size, size_t nmemb, void *stream)
{
	//parse
	//find first [[
	//then until last [[
	//line by line STATE 2 letter code
	printf("<select name=AvailLocalDIDs >\n");
	printf("<option>---</option>");
	char *cpBuffer;
	if((cpBuffer=strstr((char *)ptr,"[["))!=NULL)
	{
		//printf("%u %s\n",(unsigned)nmemb,cpBuffer+2);
		char *cpLineStart;
		int register i;
		cpLineStart=cpBuffer+2;
		for(i=2;cpBuffer[i]!=0 && i<nmemb;i++)
		{
			if(cpBuffer[i]=='\n' || cpBuffer[i]=='\r')
			{
				cpBuffer[i]=0;
				long unsigned luDID=0;
				float fMinRate=0.0,fMonthRate=0.0,fSetup=0.0;
				char cRatecenter[64]={""};
				if(5==sscanf(cpLineStart,"%lu,%[A-Z],%f:%f:%f",&luDID,cRatecenter,&fMinRate,&fMonthRate,&fSetup))
				{
					printf("<option name='%lu'>%lu %s (MinRate=$%2.2f,MonthRate=$%2.2f,Setup=$%2.2f)</option>\n",
							luDID,luDID,cRatecenter,fMinRate,fMonthRate,fSetup);
				}
				cpLineStart=cpBuffer+i+1;
			}
		}
	}
	printf("</select>\n");
}//void htmlVitelityAvailLocalDIDs()


void GetVitelityAvailLocalDIDsPerRatecenter(char *cState,char *cRatecenter)
{
	CURL *curl;
	curl=curl_easy_init();
	if(curl)
	{
		char cURL[256]={""};
		sprintf(cURL,"api.vitelity.net/api.php?login="cVitelityLogin"&pass="cVitelityPwd""
				"&cmd=listlocal&state=%.2s&ratecenter=%.99s&withrates=yes",
				cState,cRatecenter);
		curl_easy_setopt(curl,CURLOPT_URL,cURL);
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,htmlVitelityAvailLocalDIDs);
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
}//void GetVitelityAvailLocalDIDsPerRatecenter()
