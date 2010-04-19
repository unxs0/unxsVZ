/*
FILE
	ucidr.c
	$Id: ucidr.c 1193 2006-11-28 23:05:53Z ggw $
PURPOSE
	Create and install openisp libucidr a very small
	basic cdir math lib developed initially for
	checking to see if a given ipv4 is in a
	given arbitrary ipv4 CIDR block such as 10.0.0.0/24
AUTHOR
	(C) 2006-2008 Gary Wallis and Huguo Urquiza for Unixservice
REQUIRES
CONTENTS
	See ucidr.h
TODO
	Document the provided functions.
	Create MAN pages.

*/

#include "ucidr.h"

static unsigned Version_2_0;


unsigned ExpandCIDR4(const char *cCIDR4, char *cIPs[])
{
	unsigned a=0,b=0,c=0,d=0;
	unsigned uCIDR4IP=0;
	unsigned uCIDR4Mask=0;
	unsigned uTmp=0;

	register unsigned i;

	if(!uInCIDR4Format(cCIDR4,&uCIDR4IP,&uCIDR4Mask))
		return(0);

	for(i=0;i<(~uCIDR4Mask)+1;i++)
	{

		uTmp=uCIDR4IP & 0xff000000;
		a= uTmp >> 24;

		uTmp= uCIDR4IP & 0x00ff0000;
		b= uTmp >> 16;

		uTmp= uCIDR4IP & 0x0000ff00;
		c= uTmp >> 8;

		d=    uCIDR4IP & 0x000000ff;

		cIPs[i]=malloc(16);
		sprintf(cIPs[i],"%u.%u.%u.%u",a,b,c,d);

		uCIDR4IP++;
	}

	return(i);

}//unsigned ExpandCIDR4(const char *cCIDR4,  char *cIPs[])


unsigned uIpv4InCIDR4(const char *cIPv4, const char *cCIDR4)
{
	unsigned uIPv4=0;
	unsigned uCIDR4IP=0;
	unsigned uCIDR4Mask=0;
	unsigned uCIDR4Masked=0;
	unsigned uIPv4Masked=0;

	if(!uInCIDR4Format(cCIDR4,&uCIDR4IP,&uCIDR4Mask))
		return(2);
	if(!uInIpv4Format(cIPv4,&uIPv4))
		return(3);

	//debug only
	//printf("uIpv4InCIDR4():\n");
	//printf("\tuIPv4=       0x%X\n",uIPv4);
	//printf("\tuCIDR4IP=    0x%X\n",uCIDR4IP);
	//printf("\tuCIDR4Mask=  0x%X\n",uCIDR4Mask);
	uCIDR4Masked=(uCIDR4IP & uCIDR4Mask);
	//printf("\tuCIDR4Masked=0x%X\n",uCIDR4Masked);
	uIPv4Masked=(uIPv4 & uCIDR4Mask);
	//printf("\tuIPv4Masked= 0x%X\n",uIPv4Masked);

	if(uIPv4Masked==uCIDR4Masked)
		return(1);
	else
		return(0);

}//unsigned uIpv4InCIDR4(const char *cIPv4, const char *cCIDR4)


unsigned uInCIDR4Format(const char *cCIDR4,unsigned *uIPv4,unsigned *uCIDR4Mask)
{
	unsigned a=0,b=0,c=0,d=0,e=0;
	sscanf(cCIDR4,"%u.%u.%u.%u/%u",&a,&b,&c,&d,&e);

	//debug only
	//printf("uInCIDR4Format() %s=%u.%u.%u.%u/%u\n",cCIDR4,a,b,c,d,e);

	//basic sanity checks
	if(!a || !e)
		return(0);
	if(b>254)
		return(0);
	if(c>254)
		return(0);
	if(d>254)
		return(0);
	//Single IP or old class A max
	if(e>32 || e<8)
		return(0);

	*uIPv4=d+(c<<8)+(b<<16)+(a<<24);
	*uCIDR4Mask= (0xffffffff) << (32-e);
	return(1);

}//unsigned uInCIDRFormat()


unsigned uInIpv4Format(const char *cIPv4,unsigned *uIPv4)
{
	unsigned a=0,b=0,c=0,d=0;
	sscanf(cIPv4,"%u.%u.%u.%u",&a,&b,&c,&d);

	//debug only
	//printf("uInIpv4Format() %s=%u.%u.%u.%u\n",cIPv4,a,b,c,d);

	//basic sanity checks
	if(!a)
		return(0);
	if(b>254)
		return(0);
	if(c>254)
		return(0);
	if(d>254)
		return(0);

	*uIPv4=d+(c<<8)+(b<<16)+(a<<24);
	return(1);

}//unsigned uInIpv4Format()


unsigned uGetNumIPs(char *cCIDR4)
{
	//
	//Given a CIDR block, this function returns how many host IP addresses are contained in the block
	// 
	unsigned uNumIPs=0;
	unsigned uA=0;
	unsigned uB=0;
	unsigned uC=0;
	unsigned uD=0;
	unsigned uE=0;
	
	Version_2_0=0; //This has no effect, it just avoid a compiler warning.

	if((uA>255)||(uB>255)||(uC>255)||(uD>255)) return(0);
	
	sscanf(cCIDR4,"%u.%u.%u.%u/%u",&uA,&uB,&uC,&uD,&uE);
	
	uNumIPs=(unsigned)2<<(32-uE-1);
	
	if(!uNumIPs)
		uNumIPs=1;
	else
		uNumIPs=uNumIPs-2; //minus network address and broadcast address
	
	return(uNumIPs);

}//unsigned uGetNumIPs(char *cCIDR4)


unsigned uGetNumNets(char *cCIDR4)
{
	//
	//Given a CIDR block, this function returns how many networks are within the block
	//
	unsigned uNumIPs=0;
	unsigned uNumNets=0;
	
	uNumIPs=uGetNumIPs(cCIDR4);
	
	uNumNets=(unsigned)ceil((uNumIPs/255));

	if(!uNumNets) uNumNets=1;
	
	return(uNumNets);

}//unsigned uGetuNumNets(char *cCIDR4)

