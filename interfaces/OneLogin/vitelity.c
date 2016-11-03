/*
FILE 
	vitelity.c
	svn ID removed
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
static CURL *curl=NULL;
static CURLcode curlRes;

//TOC
static size_t htmlVitelityAvailStateSelect(void *ptr, size_t size, size_t nmemb, void *stream);
static void GetVitelityAvailStates(void);
static size_t htmlVitelityAvailRatecenterSelect(void *ptr, size_t size, size_t nmemb, void *stream);
static void GetVitelityAvailRatecentersPerState(char *cState);
static size_t htmlVitelityAvailLocalDIDs(void *ptr, size_t size, size_t nmemb, void *stream);
static void GetVitelityAvailLocalDIDsPerRatecenter(char *cState,char *cRatecenter);
//public functions exposed
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
			" title='Select a new DID from our inventory for immediate use'"
			" name='gcDIDNew' onChange='submit()'>\n");
	printf("<option>---</option>");
	GetVitelityAvailLocalDIDsPerRatecenter(cState,cRatecenter);
	printf("</select>");
}//void htmlAvailLocalDIDs(char *cState,char *cRatecenter)


//private functions

static size_t htmlVitelityAvailStateSelect(void *ptr, size_t size, size_t nmemb, void *stream)
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
				printf(" value='%s'>%s</option>",cpLineStart,cpLineStart);
				cpLineStart=cpBuffer+i+1;
			}
		}
	}
	//always return what was passed to us.
	uNumCalls++;
	return(nmemb);
}//size_t htmlVitelityAvailStateSelect()


static void GetVitelityAvailStates(void)
{
	if(curl==NULL) curl=curl_easy_init();
	if(curl)
	{
		if(!cVitelityAPILogin[0]) GetConfiguration("cVitelityAPILogin",cVitelityAPILogin,0,0,0,0);
		if(!cVitelityAPIPwd[0]) GetConfiguration("cVitelityAPIPwd",cVitelityAPIPwd,0,0,0,0);
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


static size_t htmlVitelityAvailRatecenterSelect(void *ptr, size_t size, size_t nmemb, void *stream)
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
				printf(" value='%s'>%s</option>",cpLineStart,cpLineStart);
				cpLineStart=cpBuffer+i+1;
			}
		}
	}

	//always return what was passed to us.
	uNumCalls++;
	return(nmemb);

}//size_t htmlVitelityAvailRatecenterSelect()


static void GetVitelityAvailRatecentersPerState(char *cState)
{
	if(curl==NULL) curl=curl_easy_init();
	if(curl)
	{
		if(!cVitelityAPILogin[0]) GetConfiguration("cVitelityAPILogin",cVitelityAPILogin,0,0,0,0);
		if(!cVitelityAPIPwd[0]) GetConfiguration("cVitelityAPIPwd",cVitelityAPIPwd,0,0,0,0);
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


static size_t htmlVitelityAvailLocalDIDs(void *ptr, size_t size, size_t nmemb, void *stream)
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
				if(5==sscanf(cpLineStart,"%llu,%[A-Z ],%f:%f:%f",&lluDID,cRatecenter,&fMinRate,&fMonthRate,&fSetup))
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


static void GetVitelityAvailLocalDIDsPerRatecenter(char *cState,char *cRatecenter)
{
	if(curl==NULL) curl=curl_easy_init();
	if(curl)
	{
		if(!cVitelityAPILogin[0]) GetConfiguration("cVitelityAPILogin",cVitelityAPILogin,0,0,0,0);
		if(!cVitelityAPIPwd[0]) GetConfiguration("cVitelityAPIPwd",cVitelityAPIPwd,0,0,0,0);
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
