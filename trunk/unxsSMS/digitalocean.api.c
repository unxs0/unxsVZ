/*
FILE
	unxsSMS/digitalocean.api.c
PURPOSE
	DevOps testing libcurl for DigitalOcean API.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2016.
	GPLv2 License applies. See LICENSE file.
NOTES
*/

#include <sys/sysinfo.h>
#include <ctype.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>


void vUsage(char *cPgm);
unsigned uDigitalOceanSimpleGet(char *cOAuth2Token, char *cNamespace);


void vUsage(char *cPgm)
{
	printf("Usage: %s <cOAuth2Token> <cNamespace> [<cPostData>|--delete]\n",cPgm);
	exit(0);
}//void vUsage(void)


unsigned uDigitalOceanSimpleDelete(char *cOAuth2Token,char *cNamespace)
{
	CURL *curl;
	CURLcode res;
	char cURL[256];

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl=curl_easy_init();
	if(curl)
	{
		struct curl_slist *curl_slistHeaders=NULL;
		char cHeader[100]={"Content-Type: application/json"};
		curl_slistHeaders=curl_slist_append(curl_slistHeaders,cHeader);
		sprintf(cHeader,"Authorization:  Bearer %s",cOAuth2Token);
		curl_slistHeaders=curl_slist_append(curl_slistHeaders,cHeader);

		sprintf(cURL,"https://api.digitalocean.com/v2/%s",cNamespace);
		curl_easy_setopt(curl,CURLOPT_URL,cURL);
		curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,0L);
		curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0L);
		curl_easy_setopt(curl,CURLOPT_HTTPHEADER,curl_slistHeaders);
		//curl_easy_setopt(curl,CURLOPT_POSTFIELDS,cPostData);
		curl_easy_setopt(curl,CURLOPT_CUSTOMREQUEST,"DELETE");

		res=curl_easy_perform(curl);

		if(res!=CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			return(2);
		}
 
		// always cleanup
		curl_easy_cleanup(curl);

		return(0);
	}

	return(1);

}//unsigned uDigitalOceanSimpleDelete(char *cOAuth2Token,char *cNamespace,char *cPostData)


unsigned uDigitalOceanSimplePost(char *cOAuth2Token,char *cNamespace,char *cPostData)
{
	CURL *curl;
	CURLcode res;
	char cURL[256];

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl=curl_easy_init();
	if(curl)
	{
		struct curl_slist *curl_slistHeaders=NULL;
		char cHeader[100]={"Content-Type: application/json"};
		curl_slistHeaders=curl_slist_append(curl_slistHeaders,cHeader);
		sprintf(cHeader,"Authorization:  Bearer %s",cOAuth2Token);
		curl_slistHeaders=curl_slist_append(curl_slistHeaders,cHeader);

		sprintf(cURL,"https://api.digitalocean.com/v2/%s",cNamespace);
		curl_easy_setopt(curl,CURLOPT_URL,cURL);
		curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,0L);
		curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0L);
		curl_easy_setopt(curl,CURLOPT_HTTPHEADER,curl_slistHeaders);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS,cPostData);
		res=curl_easy_perform(curl);

		if(res!=CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			return(2);
		}
 
		// always cleanup
		curl_easy_cleanup(curl);

		return(0);
	}

	return(1);

}//unsigned uDigitalOceanSimplePost(char *cOAuth2Token,char *cNamespace,char *cPostData)


unsigned uDigitalOceanSimpleGet(char *cOAuth2Token,char *cNamespace)
{
	CURL *curl;
	CURLcode res;
	char cURL[256];

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl=curl_easy_init();
	if(curl)
	{
		struct curl_slist *curl_slistHeaders=NULL;
		char cHeader[100]={"Content-Type: application/json"};
		curl_slistHeaders=curl_slist_append(curl_slistHeaders,cHeader);
		sprintf(cHeader,"Authorization:  Bearer %s",cOAuth2Token);
		curl_slistHeaders=curl_slist_append(curl_slistHeaders,cHeader);

		sprintf(cURL,"https://api.digitalocean.com/v2/%s",cNamespace);
		curl_easy_setopt(curl,CURLOPT_URL,cURL);
		curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,0L);
		curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0L);
		curl_easy_setopt(curl,CURLOPT_HTTPHEADER,curl_slistHeaders);
		curl_easy_setopt(curl,CURLOPT_HTTPGET,1);
		res=curl_easy_perform(curl);

		if(res!=CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			return(2);
		}
 
		// always cleanup
		curl_easy_cleanup(curl);

		return(0);
	}

	return(1);

}//unsigned uDigitalOceanSimpleGet(char *cOAuth2Token, char *cNamespace)


int main(int iArgc, char *cArgv[])
{
	if(iArgc<3)
		vUsage(cArgv[0]);

	if(iArgc==3)
		return(uDigitalOceanSimpleGet(cArgv[1],cArgv[2]));
	else if(!strcmp(cArgv[3],"--delete"))
		return(uDigitalOceanSimpleDelete(cArgv[1],cArgv[2]));
	else
		return(uDigitalOceanSimplePost(cArgv[1],cArgv[2],cArgv[3]));

}//main()
