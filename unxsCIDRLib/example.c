/*
FILE
	ucidr.c
	svn ID removed
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
	//register unsigned i;
	//unsigned uRetVal=0;
	//char *cIPs[4096];//Warning max 8 class C's or /21
	//unsigned uIPs=0;
	//unsigned uMask=0;

#ifdef __uint128_t
	__uint128_t uIPv6=0,uCIDR6IP=0,uCIDR6Mask=0;
#endif

	if(iArgc<3)
	{
		printf("Usage: %s <cIP in dotted quad format> <cIP/cCIDR>\nCIDR limits: 8-32\nIP limits no 255 values allowed\n",cArgv[0]);
		exit(0);
	}

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
//#ifdef __uint128_t
#ifdef notdefined
	if(!uInIpv6Format(cArgv[1],&uIPv6))
		printf("IPv6 format error in %s\n",cArgv[1]);

	if(!uInCIDR6Format(cArgv[2],&uCIDR6IP,&uCIDR6Mask))
		printf("IPv6 with /CIDR format error in %s\n",cArgv[2]);

	if(uIpv6InCIDR6(cArgv[1],cArgv[2]))
		printf("%s is in %s\n",cArgv[1],cArgv[2]);
	else
		 printf("not\n");
#else
	//unsigned a1=0,a2=0,a3=0,a4=0,a5=0,a6=0,a7=0,a8=0,uCIDR=0;
	//if(!uInIpv6Format32(cArgv[1],&a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8))
	//	printf("IPv6 format error in %s\n",cArgv[1]);
	//if(!uInCIDR6Format32(cArgv[2],&a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&uCIDR))
	//	printf("IPv6 with /CIDR format error in %s\n",cArgv[2]);
	if(uIpv6InCIDR632(cArgv[1],cArgv[2]))
		printf("%s is in %s\n",cArgv[1],cArgv[2]);
	else
		 printf("%s is NOT in %s\n",cArgv[1],cArgv[2]);
#endif
	

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
