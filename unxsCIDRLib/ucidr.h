/*
FILE
	ucidr.h
	svn ID removed
PURPOSE
	Create and install openisp libucidr a very small
	basic cdir math lib developed initially for
	checking to see if a given ipv4 is in a
	given arbitrary ipv4 CIDR block such as 10.0.0.0/24
AUTHOR
	(C) 2006-2008 Gary Wallis and Hugo Urquiza for Unixservice
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

unsigned ExpandCIDR4(const char *cCIDR4, char *cIPs[]);
unsigned uIpv4InCIDR4(const char *cIPv4, const char *cCIDR4);
unsigned uInCIDR4Format(const char *cCIDR4,unsigned *uIPv4,unsigned *uCIDR4Mask);
unsigned uInIpv4Format(const char *cIPv4,unsigned *uIPv4);
unsigned uGetNumIPs(char *cCIDR4);
unsigned uGetNumNets(char *cCIDR4);

//New IPv6 functions
#ifdef __uint128_t
unsigned uIpv6InCIDR6(const char *cIPv6, const char *cCIDR6);
unsigned uInCIDR6Format(const char *cCIDR6,__uint128_t *uIPv6,__uint128_t *uIPv6CIDR);
unsigned uInIpv6Format(const char *cIPv6,__uint128_t *uIPv6);
#endif
unsigned uInIpv6Format32(const char *cIPv6,unsigned *h1,unsigned *h2,unsigned *h3,unsigned *h4,
				unsigned *h5,unsigned *h6,unsigned *h7,unsigned *h8);
unsigned uInCIDR6Format32(const char *cCIDR6,unsigned *h1,unsigned *h2,unsigned *h3,unsigned *h4,
				unsigned *h5,unsigned *h6,unsigned *h7,unsigned *h8,unsigned *uCIDR);
unsigned uIpv6InCIDR632(const char *cIPv6, const char *cCIDR6);

