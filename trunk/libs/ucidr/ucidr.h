/*
FILE
	ucidr.h
	$Id: ucidr.h 2822 2009-03-30 15:38:41Z Gary $
PURPOSE
	Header for libucidr.a micro CIDR library.
AUTHOR
	(C) 2006-2009 Gary Wallis for Unixservice, LLC.
	This software is distributed under the LGPL.
	See included LICENSE.
*/

#include <stdio.h>
#include <stdlib.h>

unsigned ExpandCIDR4(const char *cCIDR4, char *cIPs[]);
unsigned uIpv4InCIDR4(const char *cIPv4, const char *cCIDR4);
unsigned uInCIDR4Format(const char *cCIDR4,unsigned *uIPv4,unsigned *uCIDR4Mask);
unsigned uInIpv4Format(const char *cIPv4,unsigned *uIPv4);
