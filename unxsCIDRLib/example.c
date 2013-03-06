/*
FILE
	ucidr.c
	$Id: example.c 1193 2006-11-28 23:05:53Z ggw $
PURPOSE
	Illustrate the use of libucidr.a
AUTHOR
	(C) 2006 Gary Wallis for Unixservice
REQUIRES
	libucidr.a

*/

#include <openisp/ucidr.h>


int main(int iArgc, char *cArgv[])
{
	register unsigned i;
	unsigned uRetVal=0;
	char *cIPs[4096];//Warning max 8 class C's or /21
	unsigned uIPs=0;
	unsigned uMask=0;

	if(iArgc<3)
	{
		printf("Usage: %s <cIP in dotted quad format> <cIP/cCIDR>\nCIDR limits: 8-32\nIP limits no 255 values allowed\n",cArgv[0]);
		exit(0);
	}

	//Example 1
	//Given IPv4 in a given IPv4/CIDR4
/*
	uRetVal=uIpv4InCIDR4(cArgv[1],cArgv[2]);
	if(uRetVal==1)
		printf("%.16s is in %s\n",cArgv[1],cArgv[2]);
	else if(uRetVal==0)
		printf("%.16s is NOT in %s\n",cArgv[1],cArgv[2]);
	else if(uRetVal==2)
		printf("IPv4/CIDR format error in %s\n",cArgv[2]);
	else if(uRetVal==3)
		printf("IPv4 format error in %s\n",cArgv[1]);

*/
	if(!uInIpv6Format(cArgv[1]))
		printf("IPv6 Format error in %s\n",cArgv[1]);

/*
	//Example 2
	//Expand into array of IPs in string/0 format.
	sscanf(cArgv[2],"%*u.%*u.%*u.%*u/%u",&uMask);
	if(uMask<21)
	{
		printf("Example 2 is limited to 8 class C's or /21\n");
		exit(0);
	}
	uIPs=ExpandCIDR4(cArgv[2],&cIPs[0]);
	for(i=0;i<uIPs;i++)
	{
		printf("%s\n",cIPs[i]);
		free(cIPs[i]);
	}
*/

	return(0);

}//main()
