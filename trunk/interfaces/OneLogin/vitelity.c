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

static char cVitelityAPILogin[256]={""};
static char cVitelityAPIPwd[256]={""};

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
	printf("<select type='DIDStateSelect' id='DIDStateSelect' class='form-control'"
			" title='Select the state to use for DID inventory'"
			" name='gcDIDState' onChange='submit()'>\n");
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
				printf("<option");
				if(!strcmp(cpLineStart,gcDIDState))
					printf(" selected");
				printf(">%s</option>",cpLineStart);
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
		if(!cVitelityAPILogin[0]) GetConfiguration("cVitelityAPILogin",cVitelityAPILogin,0,0,0,0);
		if(!cVitelityAPIPwd[0]) GetConfiguration("cVitelityAPIPwd",cVitelityAPIPwd,0,0,0,0);
		if(cVitelityAPILogin[0] && cVitelityAPIPwd[0])
		{
			char cURL[256]={""};
			sprintf(cURL,"api.vitelity.net/api.php?login=%s&pass=%s&cmd=listavailstates&provider=l3",
				cVitelityAPILogin,cVitelityAPIPwd);
			curl_easy_setopt(curl,CURLOPT_URL,cURL);
			curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,htmlVitelityAvailStateSelect);
			curl_easy_perform(curl);
		}
		curl_easy_cleanup(curl);
	}
}//void GetVitelityAvailStates(void)


void htmlVitelityAvailRatecenterSelect(void *ptr, size_t size, size_t nmemb, void *stream)
{
	//parse
	//find first [[
	//then until last [[
	//line by line STATE 2 letter code
	printf("<select type='DIDRatecenterSelect' id='DIDRatecenterSelect' class='form-control'"
			" title='Select the ratecenter to use for DID inventory'"
			" name='gcDIDRatecenter' onChange='submit()'>\n");
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
				printf("<option");
				if(!strcmp(cpLineStart,gcDIDRatecenter))
					printf(" selected");
				printf(">%s</option>",cpLineStart);
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
		if(!cVitelityAPILogin[0]) GetConfiguration("cVitelityAPILogin",cVitelityAPILogin,0,0,0,0);
		if(!cVitelityAPIPwd[0]) GetConfiguration("cVitelityAPIPwd",cVitelityAPIPwd,0,0,0,0);
		if(cVitelityAPILogin[0] && cVitelityAPIPwd[0])
		{
			char cURL[256]={""};
			sprintf(cURL,"api.vitelity.net/api.php?login=%s&pass=%s&cmd=listavailratecenters&state=%.2s",
				cVitelityAPILogin,cVitelityAPIPwd,cState);
			curl_easy_setopt(curl,CURLOPT_URL,cURL);
			curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,htmlVitelityAvailRatecenterSelect);
			curl_easy_perform(curl);
		}
		curl_easy_cleanup(curl);
	}
}//void GetVitelityAvailRatecentersPerState()


void htmlVitelityAvailLocalDIDs(void *ptr, size_t size, size_t nmemb, void *stream)
{
	//parse
	//find first [[
	//then until last [[
	//line by line STATE 2 letter code
	printf("<select type='DIDNewSelect' id='DIDNewSelect' class='form-control'"
			" title='Select the state to use for DID inventory'"
			" name='gcDIDNew' onChange='submit()'>\n");
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
				long long unsigned lluDID=0;
				float fMinRate=0.0,fMonthRate=0.0,fSetup=0.0;
				char cRatecenter[64]={""};
				if(5==sscanf(cpLineStart,"%llu,%[A-Z],%f:%f:%f",&lluDID,cRatecenter,&fMinRate,&fMonthRate,&fSetup))
				{
					printf("<option");
					if(gcDIDNew[0] && !strncmp(cpLineStart,gcDIDNew,strlen(gcDIDNew)))
						printf(" selected");
					printf(" value='%llu'>%llu %s (MinRate=$%2.2f,MonthRate=$%2.2f,Setup=$%2.2f)</option>\n",
							lluDID,lluDID,cRatecenter,fMinRate,fMonthRate,fSetup);
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
		if(!cVitelityAPILogin[0]) GetConfiguration("cVitelityAPILogin",cVitelityAPILogin,0,0,0,0);
		if(!cVitelityAPIPwd[0]) GetConfiguration("cVitelityAPIPwd",cVitelityAPIPwd,0,0,0,0);
		if(cVitelityAPILogin[0] && cVitelityAPIPwd[0])
		{
			char cURL[256]={""};
			sprintf(cURL,"api.vitelity.net/api.php?login=%s&pass=%s"
				"&cmd=listlocal&state=%.2s&ratecenter=%.99s&withrates=yes",
					cVitelityAPILogin,cVitelityAPIPwd,cState,cRatecenter);
			curl_easy_setopt(curl,CURLOPT_URL,cURL);
			curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,htmlVitelityAvailLocalDIDs);
			curl_easy_perform(curl);
		}
		curl_easy_cleanup(curl);
	}
}//void GetVitelityAvailLocalDIDsPerRatecenter()
