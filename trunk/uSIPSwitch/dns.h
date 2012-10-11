/*
FILE
	uSIPSwitch/dns.h
PURPOSE
	Simple dns srv and a records resolver library header file.
AUTHOR/LICENSE
	Copyright (c) 2012 Gary Wallis for Unixservice, LLC. all except for:

	Based on jabberd - Jabber Open Source Server dns.c
	Copyright (c) 2002 Jeremie Miller, Thomas Muldowney,
                     Ryan Eatmon, Robert Norris
 
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
*/

#define DNS_QUERY_TYPE_A    (1)
#define DNS_QUERY_TYPE_SRV  (3)

typedef struct dns_host_st {
    struct dns_host_st  *next;

    unsigned int        type;
    unsigned int        class;
    unsigned int        ttl;

    void                *rr;
} *dns_host_t;

typedef struct dns_srv_st {
    unsigned int        priority;
    unsigned int        weight;
    unsigned int        port;
    unsigned int        rweight;

    char                name[256];
} *dns_srv_t;

dns_host_t dns_resolve(const char *zone, int type);
void dns_free(dns_host_t dns);
	

#include <string.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>


#define MAX_PACKET (1024)

typedef union {
    HEADER          hdr;
    unsigned char   buf[MAX_PACKET];
} dns_packet_t;
