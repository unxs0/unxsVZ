/*
FILE 
	testvitelity.c
	svn ID removed
AUTHOR/LEGAL
	(C) 2015 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	Standalone test Vitelity API.
COMPILE AND RUN
	cp -i didapi.h.default didapi.h (only once)
	gcc -Wall testvitelity.c -o vitelity ../../cgi.o -L/usr/lib64 -lcurl;
	./vitelity
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "didapi.h"

static char cVitelityAPILogin[256]={cAPILOGIN};
static char cVitelityAPIPwd[256]={cAPIPWD};
static CURL *curl=NULL;
static CURLcode curlRes;
char gcDIDState[8]={""};
char gcDIDRatecenter[64]={""};
char gcDIDNew[32]={""};

//TOC
int main(int iArgc, char *cArgv[]);
size_t htmlVitelityAvailStateSelect(void *ptr, size_t size, size_t nmemb, void *stream);
void GetVitelityAvailStates(void);
size_t htmlVitelityAvailRatecenterSelect(void *ptr, size_t size, size_t nmemb, void *stream);
void GetVitelityAvailRatecentersPerState(char *cState);
size_t htmlVitelityAvailLocalDIDs(void *ptr, size_t size, size_t nmemb, void *stream);
void GetVitelityAvailLocalDIDsPerRatecenter(char *cState,char *cRatecenter);
void htmlAvailRatecentersPerState(char *cState);
void htmlAvailStates(void);
void htmlAvailLocalDIDs(char *cState,char *cRatecenter);

void htmlAvailRatecentersPerState(char *cState)
{
	printf("<select type='DIDRatecenterSelect' id='DIDRatecenterSelect' class='form-control'"
			" title='Select the ratecenter to use for DID inventory'"
			" name='gcDIDRatecenter' onChange='submit()'>\n");
	printf("<option>---</option>");
	GetVitelityAvailRatecentersPerState(cState);
	printf("</select>");
}//void htmlAvailRatecentersPerState(char *cState)


void htmlAvailStates(void)
{
	printf("<select type='DIDStateSelect' id='DIDStateSelect' class='form-control'"
			" title='Select the state to use for DID inventory'"
			" name='gcDIDState' onChange='submit()'>\n");
	printf("<option>---</option>");
	GetVitelityAvailStates();
	printf("</select>");
}//void htmlAvailStates(void)


void htmlAvailLocalDIDs(char *cState,char *cRatecenter)
{
	printf("<select type='DIDNewSelect' id='DIDNewSelect' class='form-control'"
			" title='Select the state to use for DID inventory'"
			" name='gcDIDNew' onChange='submit()'>\n");
	printf("<option>---</option>");
	GetVitelityAvailLocalDIDsPerRatecenter(cState,cRatecenter);
	printf("</select>");
}//void htmlAvailLocalDIDs(char *cState,char *cRatecenter)


int main(int iArgc, char *cArgv[])
{
	//htmlAvailStates();
	htmlAvailRatecentersPerState("CA");
	htmlAvailLocalDIDs("CA","EL CAJON");
	if(curl)
		curl_easy_cleanup(curl);
	return(0);
}//int main()


size_t htmlVitelityAvailStateSelect(void *ptr, size_t size, size_t nmemb, void *stream)
{
	//parse
	//find first [[
	//then until last [[
	//line by line STATE 2 letter code
	char *cpBuffer;
	static unsigned uNumCalls=0;
	if(uNumCalls || (cpBuffer=strstr((char *)ptr,"[["))!=NULL)
	{
		if(uNumCalls) cpBuffer=(char *)ptr;
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
	//always return what was passed to us.
	uNumCalls++;
	return(nmemb);
}//size_t htmlVitelityAvailStateSelect()


void GetVitelityAvailStates(void)
{
	if(curl==NULL) curl=curl_easy_init();
	if(curl)
	{
		if(cVitelityAPILogin[0] && cVitelityAPIPwd[0])
		{
			char cURL[256]={""};
			sprintf(cURL,"api.vitelity.net/api.php?login=%s&pass=%s&cmd=listavailstates&provider=l3",
				cVitelityAPILogin,cVitelityAPIPwd);
			curlRes=curl_easy_setopt(curl,CURLOPT_URL,cURL);
			if(curlRes!=CURLE_OK)
			{
      				fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(curlRes));
				exit(1);
			}
			curlRes=curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,htmlVitelityAvailStateSelect);
			if(curlRes!=CURLE_OK)
			{
      				fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(curlRes));
				exit(1);
			}
			curlRes=curl_easy_perform(curl);
			if(curlRes!=CURLE_OK)
			{
      				fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(curlRes));
				exit(1);
			}
		}
	}
}//void GetVitelityAvailStates(void)


size_t htmlVitelityAvailRatecenterSelect(void *ptr, size_t size, size_t nmemb, void *stream)
{
	//parse
	//find first [[
	//then until last [[
	//line by line 
	char *cpBuffer=NULL;
	static unsigned uNumCalls=0;
	if(uNumCalls || (cpBuffer=strstr((char *)ptr,"[["))!=NULL)
	{
		if(uNumCalls) cpBuffer=(char *)ptr;
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

	//always return what was passed to us.
	uNumCalls++;
	return(nmemb);

}//size_t htmlVitelityAvailRatecenterSelect()


void GetVitelityAvailRatecentersPerState(char *cState)
{
	if(curl==NULL) curl=curl_easy_init();
	if(curl)
	{
		if(cVitelityAPILogin[0] && cVitelityAPIPwd[0])
		{
			char cURL[256]={""};
			sprintf(cURL,"api.vitelity.net/api.php?login=%s&pass=%s&cmd=listavailratecenters&state=%.2s",
				cVitelityAPILogin,cVitelityAPIPwd,cState);
			curlRes=curl_easy_setopt(curl,CURLOPT_URL,cURL);
			if(curlRes!=CURLE_OK)
			{
      				fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(curlRes));
				exit(1);
			}
			curlRes=curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,htmlVitelityAvailRatecenterSelect);
			if(curlRes!=CURLE_OK)
			{
      				fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(curlRes));
				exit(1);
			}
			curlRes=curl_easy_perform(curl);
			if(curlRes!=CURLE_OK)
			{
      				fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(curlRes));
				exit(1);
			}
		}
	}
}//void GetVitelityAvailRatecentersPerState()


size_t htmlVitelityAvailLocalDIDs(void *ptr, size_t size, size_t nmemb, void *stream)
{
	//parse
	//find first [[
	//then until last [[
	//line by line 
	char *cpBuffer;
	static unsigned uNumCalls=0;
	if(uNumCalls || (cpBuffer=strstr((char *)ptr,"[["))!=NULL)
	{
		if(uNumCalls) cpBuffer=(char *)ptr;
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
				printf("%s\n",cpLineStart);
				if(sscanf(cpLineStart,"%llu,%[A-Z ],%f:%f:%f",&lluDID,cRatecenter,&fMinRate,&fMonthRate,&fSetup))
				{
					printf("<option");
					if(gcDIDNew[0] && !strncmp(cpLineStart,gcDIDNew,strlen(gcDIDNew)))
						printf(" selected");
					printf(" value='%llu'>%llu %s (Minute=$%2.2f,Monthly=$%2.2f,Setup=$%2.2f)</option>\n",
							lluDID,lluDID,cRatecenter,fMinRate,fMonthRate,fSetup);
				}
				cpLineStart=cpBuffer+i+1;
			}
		}
	}
	//always return what was passed to us.
	uNumCalls++;
	return(nmemb);
}//size_t htmlVitelityAvailLocalDIDs()


void GetVitelityAvailLocalDIDsPerRatecenter(char *cState,char *cRatecenter)
{
	if(curl==NULL) curl=curl_easy_init();
	if(curl)
	{
		if(cVitelityAPILogin[0] && cVitelityAPIPwd[0])
		{
			char cURL[256]={""};
			char *output = curl_easy_escape(curl,cRatecenter,0);
			sprintf(cURL,"api.vitelity.net/api.php?login=%s&pass=%s"
				"&cmd=listlocal&state=%.2s&ratecenter=%.99s&withrates=yes",
						cVitelityAPILogin,cVitelityAPIPwd,cState,output);
			curl_free(output);
			curlRes=curl_easy_setopt(curl,CURLOPT_URL,cURL);
			if(curlRes!=CURLE_OK)
			{
      				fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(curlRes));
				exit(1);
			}
			curlRes=curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,htmlVitelityAvailLocalDIDs);
			if(curlRes!=CURLE_OK)
			{
      				fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(curlRes));
				exit(1);
			}
			curlRes=curl_easy_perform(curl);
			if(curlRes!=CURLE_OK)
			{
      				fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(curlRes));
				exit(1);
			}
		}
	}
}//void GetVitelityAvailLocalDIDsPerRatecenter()
