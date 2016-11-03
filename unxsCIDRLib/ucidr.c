/*
FILE
	ucidr.c
	svn ID removed
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
#include <ctype.h>
#include <string.h>

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
	unsigned a=0,b=0,c=0,d=0,e=0,uCount=0;
	uCount=sscanf(cCIDR4,"%u.%u.%u.%u/%u",&a,&b,&c,&d,&e);

	//debug only
	//printf("uInCIDR4Format() %s=%u.%u.%u.%u/%u\n",cCIDR4,a,b,c,d,e);

	//basic sanity checks
	if(uCount!=5)
		return(0);
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
	unsigned a=0,b=0,c=0,d=0,uCount=0;
	uCount=sscanf(cIPv4,"%u.%u.%u.%u",&a,&b,&c,&d);

	//debug only
	//printf("uInIpv4Format() %s=%u.%u.%u.%u\n",cIPv4,a,b,c,d);

	//basic sanity checks
	if(uCount!=4)
		return(0);
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


//New IPv6 functions
//

#ifdef __uint128_t

unsigned uInIpv6Format(const char *cIPv6,__uint128_t *uIPv6)
{
	register int i;
	unsigned h1=0;
	unsigned h2=0;
	unsigned h3=0;
	unsigned h4=0;
	unsigned h5=0;
	unsigned h6=0;
	unsigned h7=0;
	unsigned h8=0;
	unsigned uColonCount=0;
	unsigned uRead=0;

	for(i=0;cIPv6[i];i++)
	{
		if(cIPv6[i]==':')
			uColonCount++;
		if(cIPv6[i]!=':' && !isxdigit(cIPv6[i]))
			//IPv6 can only have colons and hex digits
			return(0);
	}

	switch(uColonCount)
	{
		case 0:
		case 1:
			//IPv6 too few colons: Min is 2!
			//debug only
			printf("err 1\n");
			return(0);
		break;

		case 2:
			uRead=sscanf(cIPv6,"%x::%x",&h1,&h8);
			if(uRead!=2 && !strncmp(cIPv6+strlen(cIPv6)-2,"::",2))
			{
				uRead=sscanf(cIPv6,"%x::",&h1);
				if(uRead!=1)
				{
					//debug only
					printf("err 2a\n");
					return(0);
				}
			}
			else if(uRead!=2)
			{
				//debug only
				printf("err 2b\n");
				return(0);
			}
		break;

		//2001:4D78::1
		case 3:
			uRead=sscanf(cIPv6,"%x::%x:%x",&h1,&h7,&h8);
			if(uRead!=3)
			{
				uRead=sscanf(cIPv6,"%x:%x::%x",&h1,&h2,&h8);
				if(uRead!=3 && !strncmp(cIPv6+strlen(cIPv6)-2,"::",2))
				{
					uRead=sscanf(cIPv6,"%x:%x::",&h1,&h2);
					if(uRead!=2)
					{
						//debug only
						printf("err 3a\n");
						return(0);
					}
				}
				else if(uRead!=3)
				{
					//debug only
					printf("err 3b\n");
					return(0);
				}
			}
		break;

		case 4:
			uRead=sscanf(cIPv6,"%x::%x:%x:%x",&h1,&h6,&h7,&h8);
			if(uRead!=4)
			{
				uRead=sscanf(cIPv6,"%x:%x::%x:%x",&h1,&h2,&h7,&h8);
				if(uRead!=4)
				{
					uRead=sscanf(cIPv6,"%x:%x:%x::%x",&h1,&h2,&h3,&h8);
					if(uRead!=4 && !strncmp(cIPv6+strlen(cIPv6)-2,"::",2))
					{
						uRead=sscanf(cIPv6,"%x:%x:%x::",&h1,&h2,&h3);
						if(uRead!=3)
						{
							//debug only
							printf("err 4a\n");
							return(0);
						}
					}
					else if(uRead!=4)
					{
						//debug only
						printf("err 4b\n");
						return(0);
					}
				}
			}
		break;

		case 5:
			uRead=sscanf(cIPv6,"%x::%x:%x:%x:%x",&h1,&h5,&h6,&h7,&h8);
			if(uRead!=5)
			{
				uRead=sscanf(cIPv6,"%x:%x::%x:%x:%x",&h1,&h2,&h6,&h7,&h8);
				if(uRead!=5)
				{
					uRead=sscanf(cIPv6,"%x:%x:%x::%x:%x",&h1,&h2,&h3,&h7,&h8);
					if(uRead!=5)
					{
						uRead=sscanf(cIPv6,"%x:%x:%x:%x::%x",&h1,&h2,&h3,&h4,&h8);
						if(uRead!=5 && !strncmp(cIPv6+strlen(cIPv6)-2,"::",2))
						{
							uRead=sscanf(cIPv6,"%x:%x:%x:%x::",&h1,&h2,&h3,&h4);
							if(uRead!=4)
							{
								//debug only
								printf("err 5a\n");
								return(0);
							}
						}
						else if(uRead!=5)
						{
							//debug only
							printf("err 5b\n");
							return(0);
						}
					}
				}
			}
		break;

		case 6:
			uRead=sscanf(cIPv6,"%x::%x:%x:%x:%x:%x",&h1,&h4,&h5,&h6,&h7,&h8);
			if(uRead!=6)
			{
				uRead=sscanf(cIPv6,"%x:%x::%x:%x:%x:%x",&h1,&h2,&h5,&h6,&h7,&h8);
				if(uRead!=6)
				{
					uRead=sscanf(cIPv6,"%x:%x:%x::%x:%x:%x",&h1,&h2,&h3,&h6,&h7,&h8);
					if(uRead!=6)
					{
						uRead=sscanf(cIPv6,"%x:%x:%x:%x::%x:%x",&h1,&h2,&h3,&h4,&h7,&h8);
						if(uRead!=6)
						{
							uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x::%x",
										&h1,&h2,&h3,&h4,&h5,&h8);
							if(uRead!=6 && !strncmp(cIPv6+strlen(cIPv6)-2,"::",2))
							{
								uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x::",&h1,&h2,&h3,&h4,&h5);
								if(uRead!=5)
								{
									//debug only
									printf("err 6a\n");
									return(0);
								}
							}
							else if(uRead!=6)
							{
								//debug only
								printf("err 6b\n");
								return(0);
							}
						}
					}
				}
			}
		break;


		//1::2:3:4:5:6:7
		//1:2::3:4:5:6:7
		//1:2:3::4:5:6:7
		//1:2:3:4::5:6:7
		//1:2:3:4:5::6:7
		//1:2:3:4:5:6::7
		//1:2:3:4:5:6:7:8
		//1:2:3:4:5:6::
		case 7:
			uRead=sscanf(cIPv6,"%x::%x:%x:%x:%x:%x:%x",&h1,&h3,&h4,&h5,&h6,&h7,&h8);
			if(uRead!=7)
			{
				uRead=sscanf(cIPv6,"%x:%x::%x:%x:%x:%x:%x",&h1,&h2,&h4,&h5,&h6,&h7,&h8);
				if(uRead!=7)
				{
					uRead=sscanf(cIPv6,"%x:%x:%x::%x:%x:%x:%x",&h1,&h2,&h3,&h5,&h6,&h7,&h8);
					if(uRead!=7)
					{
						uRead=sscanf(cIPv6,"%x:%x:%x:%x::%x:%x:%x",&h1,&h2,&h3,&h4,&h6,&h7,&h8);
						if(uRead!=7)
						{
							uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x::%x:%x",&h1,&h2,&h3,&h4,&h5,&h7,&h8);
							if(uRead!=7)
							{
								uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x:%x::%x",&h1,&h2,&h3,&h4,&h5,&h6,&h8);
								if(uRead!=7)
								{
									uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x:%x:%x:%x",&h1,&h2,&h3,&h4,&h5,&h6,&h7,&h8);
									if(uRead!=8 && !strncmp(cIPv6+strlen(cIPv6)-2,"::",2))
									{
										uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x:%x::",&h1,&h2,&h3,&h4,&h5,&h6);
										if(uRead!=6)
										{
											//debug only
											printf("err 7a\n");
											return(0);
										}
									}
									else if(uRead!=8)
									{
										//debug only
										printf("err 7b\n");
										return(0);
									}
								}
							}
						}
					}
				}
			}
		break;

		//1:2:3:4:5:6:7::
		case 8:
			uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x:%x:%x::",&h1,&h2,&h3,&h4,&h5,&h6,&h7);
			if(uRead!=7)
			{
				//debug only
				printf("err 8\n");
				return(0);
			}
		break;

		default:
			//debug only
			printf("err default\n");
			return(0);
		
	}

	//128 bit math 
	__uint128_t uh1=h1,uh2=h2,uh3=h3,uh4=h4,uh5=h5,uh6=h6,uh7=h7,uh8=h8;
	*uIPv6=uh8+(uh7<<16)+(uh6<<32)+(uh5<<48)+(uh4<<64)+(uh3<<80)+(uh2<<96)+(uh1<<112);

	//Passed all checks
	//debug only
	printf("uInIpv6Format: %x:%x:%x:%x:%x:%x:%x:%x\n",h1,h2,h3,h4,h5,h6,h7,h8);

	return(1);

}//unsigned uInIpv6Format(const char *cIPv6,__uint128_t *uIPv6)


unsigned uInCIDR6Format(const char *cCIDR6,__uint128_t *uIPv6,__uint128_t *uIPv6CIDR)
{
	char *cp;
	unsigned uCIDR=0;

	if((cp=strchr(cCIDR6,'/')))
	{
		unsigned uCount=0;

		uCount=sscanf(cp,"/%u",&uCIDR);
		if(!uCount || !isdigit(*(cp+1)))
		{
			//debug only
			printf("err /(invalid CIDR mask)\n");
			return(0);
		}
		else if(uCIDR>128)
		{
			//debug only
			printf("err CIDR mask out of range 0-128\n");
			return(0);
		}
		*cp=0;
	}
	else
	{
		//debug only
		printf("err no /\n");
		return(0);
	}

	if(!uInIpv6Format(cCIDR6,uIPv6))
	{
		//debug only
		printf("err !uInIpv6Format\n");
		return(0);
	}
	*cp='/';

	__uint128_t uCIDRMask= -1;	
	//uCIDRMask=!uCIDRMask;//set to all ones
	*uIPv6CIDR = uCIDRMask << (128-uCIDR);//shift left by max bits minus the CIDR mask value

	//debug only
	printf("uInCIDR6Format: /%u\n",uCIDR);	
	return(1);
}//unsigned uInCIDR6Format(const char *cCIDR6,__uint128_t *uIPv6,__uint128_t *uIPv6CIDR)


unsigned uIpv6InCIDR6(const char *cIPv6, const char *cCIDR6)
{
	__uint128_t uCIDR6Mask=0;	
	__uint128_t uCIDR6IP=0;	
	__uint128_t uIPv6=0;	

	__uint128_t uCIDR6Masked=0;	
	__uint128_t uIPv6Masked=0;	

	if(uInCIDR6Format(cCIDR6,&uCIDR6IP,&uCIDR6Mask) && uInIpv6Format(cIPv6,&uIPv6))
	{
		if(uCIDR6IP==0)
			printf("uIpv6InCIDR6: uCIDR6IP==0\n");	
		if(uCIDR6Mask==0)
			printf("uIpv6InCIDR6: uCIDR6Mask==0\n");	
		if(uIPv6==0)
			printf("uIpv6InCIDR6: uIPv6==0\n");	

		uCIDR6Masked=(uCIDR6IP & uCIDR6Mask);
		uIPv6Masked=(uIPv6 & uCIDR6Mask);

		if(uCIDR6Masked==0)
			printf("uIpv6InCIDR6: uCIDR6Masked==0\n");	
		if(uIPv6Masked==0)
			printf("uIpv6InCIDR6: uIPv6Masked==0\n");	

		if(uIPv6Masked==uCIDR6Masked)
			return(1);
		else
			return(0);
	}
	else
	{
			return(0);
	}

}//unsigned uIpv6InCIDR6(const char *cIPv6, const char *cCIDR6)

#endif

unsigned uInIpv6Format32(const char *cIPv6,unsigned *h1,unsigned *h2,unsigned *h3,unsigned *h4,
					unsigned *h5,unsigned *h6,unsigned *h7,unsigned *h8)
{
	register int i;
	unsigned uColonCount=0;
	unsigned uRead=0;

	for(i=0;cIPv6[i];i++)
	{
		if(cIPv6[i]==':')
			uColonCount++;
		if(cIPv6[i]!=':' && !isxdigit(cIPv6[i]))
			//IPv6 can only have colons and hex digits
			return(0);
	}

	switch(uColonCount)
	{
		case 0:
		case 1:
			//IPv6 too few colons: Min is 2!
			//debug only
			printf("err 1\n");
			return(0);
		break;

		case 2:
			uRead=sscanf(cIPv6,"%x::%x",h1,h8);
			if(uRead!=2 && !strncmp(cIPv6+strlen(cIPv6)-2,"::",2))
			{
				uRead=sscanf(cIPv6,"%x::",h1);
				if(uRead!=1)
				{
					//debug only
					printf("err 2a\n");
					return(0);
				}
			}
			else if(uRead!=2)
			{
				//debug only
				printf("err 2b\n");
				return(0);
			}
		break;

		//2001:4D78::1
		case 3:
			uRead=sscanf(cIPv6,"%x::%x:%x",h1,h7,h8);
			if(uRead!=3)
			{
				uRead=sscanf(cIPv6,"%x:%x::%x",h1,h2,h8);
				if(uRead!=3 && !strncmp(cIPv6+strlen(cIPv6)-2,"::",2))
				{
					uRead=sscanf(cIPv6,"%x:%x::",h1,h2);
					if(uRead!=2)
					{
						//debug only
						printf("err 3a\n");
						return(0);
					}
				}
				else if(uRead!=3)
				{
					//debug only
					printf("err 3b\n");
					return(0);
				}
			}
		break;

		case 4:
			uRead=sscanf(cIPv6,"%x::%x:%x:%x",h1,h6,h7,h8);
			if(uRead!=4)
			{
				uRead=sscanf(cIPv6,"%x:%x::%x:%x",h1,h2,h7,h8);
				if(uRead!=4)
				{
					uRead=sscanf(cIPv6,"%x:%x:%x::%x",h1,h2,h3,h8);
					if(uRead!=4 && !strncmp(cIPv6+strlen(cIPv6)-2,"::",2))
					{
						uRead=sscanf(cIPv6,"%x:%x:%x::",h1,h2,h3);
						if(uRead!=3)
						{
							//debug only
							printf("err 4a\n");
							return(0);
						}
					}
					else if(uRead!=4)
					{
						//debug only
						printf("err 4b\n");
						return(0);
					}
				}
			}
		break;

		case 5:
			uRead=sscanf(cIPv6,"%x::%x:%x:%x:%x",h1,h5,h6,h7,h8);
			if(uRead!=5)
			{
				uRead=sscanf(cIPv6,"%x:%x::%x:%x:%x",h1,h2,h6,h7,h8);
				if(uRead!=5)
				{
					uRead=sscanf(cIPv6,"%x:%x:%x::%x:%x",h1,h2,h3,h7,h8);
					if(uRead!=5)
					{
						uRead=sscanf(cIPv6,"%x:%x:%x:%x::%x",h1,h2,h3,h4,h8);
						if(uRead!=5 && !strncmp(cIPv6+strlen(cIPv6)-2,"::",2))
						{
							uRead=sscanf(cIPv6,"%x:%x:%x:%x::",h1,h2,h3,h4);
							if(uRead!=4)
							{
								//debug only
								printf("err 5a\n");
								return(0);
							}
						}
						else if(uRead!=5)
						{
							//debug only
							printf("err 5b\n");
							return(0);
						}
					}
				}
			}
		break;

		case 6:
			uRead=sscanf(cIPv6,"%x::%x:%x:%x:%x:%x",h1,h4,h5,h6,h7,h8);
			if(uRead!=6)
			{
				uRead=sscanf(cIPv6,"%x:%x::%x:%x:%x:%x",h1,h2,h5,h6,h7,h8);
				if(uRead!=6)
				{
					uRead=sscanf(cIPv6,"%x:%x:%x::%x:%x:%x",h1,h2,h3,h6,h7,h8);
					if(uRead!=6)
					{
						uRead=sscanf(cIPv6,"%x:%x:%x:%x::%x:%x",h1,h2,h3,h4,h7,h8);
						if(uRead!=6)
						{
							uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x::%x",
										h1,h2,h3,h4,h5,h8);
							if(uRead!=6 && !strncmp(cIPv6+strlen(cIPv6)-2,"::",2))
							{
								uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x::",h1,h2,h3,h4,h5);
								if(uRead!=5)
								{
									//debug only
									printf("err 6a\n");
									return(0);
								}
							}
							else if(uRead!=6)
							{
								//debug only
								printf("err 6b\n");
								return(0);
							}
						}
					}
				}
			}
		break;


		//1::2:3:4:5:6:7
		//1:2::3:4:5:6:7
		//1:2:3::4:5:6:7
		//1:2:3:4::5:6:7
		//1:2:3:4:5::6:7
		//1:2:3:4:5:6::7
		//1:2:3:4:5:6:7:8
		//1:2:3:4:5:6::
		case 7:
			uRead=sscanf(cIPv6,"%x::%x:%x:%x:%x:%x:%x",h1,h3,h4,h5,h6,h7,h8);
			if(uRead!=7)
			{
				uRead=sscanf(cIPv6,"%x:%x::%x:%x:%x:%x:%x",h1,h2,h4,h5,h6,h7,h8);
				if(uRead!=7)
				{
					uRead=sscanf(cIPv6,"%x:%x:%x::%x:%x:%x:%x",h1,h2,h3,h5,h6,h7,h8);
					if(uRead!=7)
					{
						uRead=sscanf(cIPv6,"%x:%x:%x:%x::%x:%x:%x",h1,h2,h3,h4,h6,h7,h8);
						if(uRead!=7)
						{
							uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x::%x:%x",h1,h2,h3,h4,h5,h7,h8);
							if(uRead!=7)
							{
								uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x:%x::%x",h1,h2,h3,h4,h5,h6,h8);
								if(uRead!=7)
								{
									uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x:%x:%x:%x",h1,h2,h3,h4,h5,h6,h7,h8);
									if(uRead!=8 && !strncmp(cIPv6+strlen(cIPv6)-2,"::",2))
									{
										uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x:%x::",h1,h2,h3,h4,h5,h6);
										if(uRead!=6)
										{
											//debug only
											printf("err 7a\n");
											return(0);
										}
									}
									else if(uRead!=8)
									{
										//debug only
										printf("err 7b\n");
										return(0);
									}
								}
							}
						}
					}
				}
			}
		break;

		//1:2:3:4:5:6:7::
		case 8:
			uRead=sscanf(cIPv6,"%x:%x:%x:%x:%x:%x:%x::",h1,h2,h3,h4,h5,h6,h7);
			if(uRead!=7)
			{
				//debug only
				printf("err 8\n");
				return(0);
			}
		break;

		default:
			//debug only
			printf("err default\n");
			return(0);
		
	}

	//Passed all checks
	//debug only
	printf("uInIpv6Format32: %x:%x:%x:%x:%x:%x:%x:%x\n",*h1,*h2,*h3,*h4,*h5,*h6,*h7,*h8);

	return(1);

}//unsigned uInIpv6Format32(const char *cIPv6,unsigned *h1,unsigned *h2,unsigned *h3,unsigned *h4,
//					unsigned *h5,unsigned *h6,unsigned *h7,unsigned *h8)


unsigned uInCIDR6Format32(const char *cCIDR6,unsigned *h1,unsigned *h2,unsigned *h3,unsigned *h4,
				unsigned *h5,unsigned *h6,unsigned *h7,unsigned *h8,unsigned *uCIDR)
{
	char *cp;

	if((cp=strchr(cCIDR6,'/')))
	{
		unsigned uCount=0;

		uCount=sscanf(cp,"/%u",uCIDR);
		if(!uCount || !isdigit(*(cp+1)))
		{
			//debug only
			printf("err /(invalid CIDR mask)\n");
			return(0);
		}
		else if(*uCIDR>128)
		{
			//debug only
			printf("err CIDR mask out of range 0-128\n");
			return(0);
		}
		*cp=0;
	}
	else
	{
		//debug only
		printf("err no /\n");
		return(0);
	}

	if(!uInIpv6Format32(cCIDR6,h1,h2,h3,h4,h5,h6,h7,h8))
	{
		//debug only
		printf("err !uInIpv6Format\n");
		return(0);
	}
	*cp='/';

	//debug only
	printf("uInCIDR6Format32: /%u\n",*uCIDR);	
	return(1);
}//unsigned uInCIDR6Format32(const char *cCIDR6,unsigned *h1,unsigned *h2,unsigned *h3,unsigned *h4,
//				unsigned *h5,unsigned *h6,unsigned *h7,unsigned *h8,unsigned *uCIDR)


unsigned uIpv6InCIDR632(const char *cIPv6, const char *cCIDR6)
{
	unsigned a1=0,a2=0,a3=0,a4=0,a5=0,a6=0,a7=0,a8=0,uCIDR=0;
	unsigned b1=0,b2=0,b3=0,b4=0,b5=0,b6=0,b7=0,b8=0;
	uInIpv6Format32(cIPv6,&a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8);
	uInCIDR6Format32(cCIDR6,&b1,&b2,&b3,&b4,&b5,&b6,&b7,&b8,&uCIDR);

	//based on uCIDR only mask a single word and ignore the ones to to the right of it.
	//compare the words to the left

	unsigned uMask=0;
	if(uCIDR<128 && uCIDR>112)
	{
		uMask=128-uCIDR;
		uMask=~uMask;
		a8=(a8&uMask);
		b8=(b8&uMask);
		uCIDR=128;
	}
	else if(uCIDR<112 && uCIDR>96)
	{
		uMask=112-uCIDR;
		uMask=~uMask;
		a7=(a7&uMask);
		b7=(b7&uMask);
		uCIDR=112;
	}
	else if(uCIDR<96 && uCIDR>80)
	{
		uMask=96-uCIDR;
		uMask=~uMask;
		a6=(a6&uMask);
		b6=(b6&uMask);
		uCIDR=96;
	}
	else if(uCIDR<80 && uCIDR>64)
	{
		uMask=80-uCIDR;
		uMask=~uMask;
		a5=(a5&uMask);
		b5=(b5&uMask);
		uCIDR=80;
	}
	else if(uCIDR<64 && uCIDR>48)
	{
		uMask=64-uCIDR;
		uMask=~uMask;
		a4=(a4&uMask);
		b4=(b4&uMask);
		uCIDR=64;
	}
	else if(uCIDR<48 && uCIDR>32)
	{
		uMask=48-uCIDR;
		uMask=~uMask;
		a3=(a3&uMask);
		b3=(b3&uMask);
		uCIDR=48;
	}
	else if(uCIDR<32 && uCIDR>16)
	{
		uMask=32-uCIDR;
		uMask=~uMask;
		a2=(a2&uMask);
		b2=(b2&uMask);
		uCIDR=32;
	}
	else if(uCIDR<16 && uCIDR>0)
	{
		uMask=16-uCIDR;
		uMask=~uMask;
		a1=(a1&uMask);
		b1=(b1&uMask);
		uCIDR=16;
	}
	else if(uCIDR==0)
	{
		//check this but /0 is sometimes defined as the whole range
		return(1);
	}
	if(uMask)
		//debug only
		printf("~uMask=%4.4x\n",uMask);

	//Once we mask if appliacable we can compare all the relevant words
	//but only on word boundaries.
	switch(uCIDR)
	{
		case 128:
			//we compare all words
			if(a1==b1 && a2==b2 && a3==b3 && a4==b4 && a5==b5 && a6==b6 && a7==b7 && a8==b8)
				return(1);
		break;

		case 112:
			//we compare all words except last
			if(a1==b1 && a2==b2 && a3==b3 && a4==b4 && a5==b5 && a6==b6 && a7==b7)
				return(1);
		break;

		case 96:
			//we compare all words except last two
			if(a1==b1 && a2==b2 && a3==b3 && a4==b4 && a5==b5 && a6==b6)
				return(1);
		break;
	
		case 80:
			//we compare all words except last three
			if(a1==b1 && a2==b2 && a3==b3 && a4==b4 && a5==b5)
				return(1);
		break;

		case 64:
			//we compare all words except last four
			if(a1==b1 && a2==b2 && a3==b3 && a4==b4)
				return(1);
		break;
	
		case 48:
			//we compare only first three words
			if(a1==b1 && a2==b2 && a3==b3)
				return(1);
		break;

		case 32:
			//we compare only first two words
			if(a1==b1 && a2==b2)
				return(1);
		break;
	
		case 16:
			//we compare only first word
			if(a1==b1)
				return(1);
	}

	return(0);

}//unsigned uIpv6InCIDR632(const char *cIPv6, const char *cCIDR6)
