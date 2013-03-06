/*
FILE
	ucidr.h
	$Id: ucidr.h 1193 2006-11-28 23:05:53Z ggw $
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
unsigned uInIpv6Format(const char *cIPv6);
unsigned uInCIDR6Format(const char *cCIDR6);
unsigned uIpv6InCIDR6(const char *cIPv6, const char *cCIDR6);

