/*
FILE
	oathtool.c
	svn ID removed
PURPOSE
	initial tests for integrating 2 step login to web apps.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2013
	GPLv2 License applies. See LICENSE file.
NOTES
*/

#include <liboath/oath.h>
#include <stdio.h>
#include <stdlib.h>

int main (int iArgc, char *cArgv[])
{
	char *secret;
	size_t secretlen = 0;
	int rc;
	char otp[10];
	time_t now;

	if(iArgc<3)
	{
		printf("usage: %s <base32 secret> <otp> [<version>]\n",cArgv[0]);
		return EXIT_SUCCESS;
	}

	rc=oath_init();
	if(rc!=OATH_OK)
		printf("liboath initialization failed: %s",oath_strerror(rc));

	now=time(NULL);
	rc=oath_base32_decode(cArgv[1],strlen(cArgv[1]),&secret,&secretlen);
	if(rc!=OATH_OK)
		printf("base32 decoding failed: %s",oath_strerror(rc));

	rc=oath_totp_generate(secret,secretlen,now-30,30,0,6,otp);
	if(rc!=OATH_OK)
		printf("generating one-time password failed (%d)",rc);
	if(!strcmp(cArgv[2],otp))
	{
		printf("valid 1 %s\n",otp);
		return EXIT_SUCCESS;
	}

	rc=oath_totp_generate(secret,secretlen,now,30,0,6,otp);
	if(rc!=OATH_OK)
		printf("generating one-time password failed (%d)",rc);
	if(!strcmp(cArgv[2],otp))
	{
		printf("valid 2 %s\n",otp);
		return EXIT_SUCCESS;
	}

	rc=oath_totp_generate(secret,secretlen,now+30,30,0,6,otp);
	if(rc!=OATH_OK)
		printf("generating one-time password failed (%d)",rc);
	if(!strcmp(cArgv[2],otp))
	{
		printf("valid 3 %s\n",otp);
		return EXIT_SUCCESS;
	}
	printf("failed\n");

	return EXIT_SUCCESS;

	free(secret);
	oath_done();

	return EXIT_SUCCESS;
}//main()
