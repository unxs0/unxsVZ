/*
FILE
	ucidr.c
	$Id: ucidr.c 2822 2009-03-30 15:38:41Z Gary $
PURPOSE
	Create and install openisp libucidr a very small
	basic cdir math lib developed initially for
	checking to see if a given ipv4 is in a
	given arbitrary ipv4 CIDR block such as 10.0.0.0/24
AUTHOR
	(C) 2006-2009 Gary Wallis for Unixservice, LLC.
	This program is distributed under the LGPL. See included LICENSE.
REQUIRES
CONTENTS
	See ucidr.h
TODO
	Document the provided functions.
	Create MAN pages.

*/

#include "ucidr.h"


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
