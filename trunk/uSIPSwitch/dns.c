/*
FILE
	uSIPSwitch/dns.c
PURPOSE
	Simple dns srv and a records resolver library
COMPILE
	gcc -Wall dns.c -o dns -lresolv
	CentOS 5 tested only
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

#include "dns.h"


static void *_a_rr(dns_packet_t packet, unsigned char *eom, unsigned char **scan)
{
	struct in_addr in;

	NS_GET32(in.s_addr, *scan);
	in.s_addr = ntohl(in.s_addr);

	return strdup(inet_ntoa(in));
}//static void *_a_rr(dns_packet_t packet, unsigned char *eom, unsigned char **scan)


static void *_srv_rr(dns_packet_t packet, unsigned char *eom, unsigned char **scan)
{
	unsigned int priority, weight, port;
	int len;
	char host[256];
	dns_srv_t srv;
	
	NS_GET16(priority,*scan);
	NS_GET16(weight,*scan);
	NS_GET16(port,*scan);
	len=dn_expand(packet.buf, eom,*scan,host,256);
	if(len<0)
		return NULL;
	*scan=(unsigned char *)(*scan+len);
	srv=(dns_srv_t)malloc(sizeof(struct dns_srv_st));
	srv->priority=priority;
	srv->weight=weight;
	srv->port=port;
	strcpy(srv->name, host);

	return (void *) srv;
}//static void *_srv_rr(dns_packet_t packet, unsigned char *eom, unsigned char **scan)


dns_host_t dns_resolve(const char *zone, int query_type)
{
	char host[256];
	dns_packet_t packet;
	int len, qdcount, ancount, an, n;
	unsigned char *eom, *scan;
	dns_host_t *reply, first;
	unsigned int t_type, type, class, ttl;

	if(zone==NULL || *zone=='\0')
		return NULL;

	switch(query_type)
	{
		case DNS_QUERY_TYPE_A:
			t_type = ns_t_a;
		break;

		case DNS_QUERY_TYPE_SRV:
			t_type = ns_t_srv;
		break;

		default:
		return NULL;
	}

	if((len=res_query(zone,ns_c_in,t_type,packet.buf,MAX_PACKET))==(-1) || len < sizeof(HEADER))
		return NULL;

	qdcount=ntohs(packet.hdr.qdcount);
	ancount=ntohs(packet.hdr.ancount);

	eom=(unsigned char *)(packet.buf + len);

	scan=(unsigned char *)(packet.buf + sizeof(HEADER));

	while(qdcount > 0 && scan < eom)
	{
		qdcount--;
		if((len=dn_expand(packet.buf,eom,scan,host,256))<0)
			return NULL;
		scan=(unsigned char *)(scan+len+NS_QFIXEDSZ);
	}

	reply=(dns_host_t *) malloc(sizeof(dns_host_t) * ancount);
	memset(reply, 0, sizeof(dns_host_t) * ancount);

	an=0;
	while(ancount > 0 && scan < eom )
	{
		ancount--;
		len=dn_expand(packet.buf,eom,scan,host,256);
		if(len < 0)
		{
			for(n = 0; n < an; n++)
			free(reply[n]);
			free(reply);
			return NULL;
		}

        	scan+=len;

		NS_GET16(type, scan);
		NS_GET16(class, scan);
		NS_GET32(ttl, scan);
		NS_GET16(len, scan);

		if(type != t_type)
		{
			scan=(unsigned char *) (scan + len);
			continue;
		}

		reply[an]=(dns_host_t) malloc(sizeof(struct dns_host_st));

		reply[an]->type = type;
		reply[an]->class = class;
		reply[an]->ttl = ttl;

		reply[an]->next = NULL;

		switch(type)
		{
			case ns_t_a:
				reply[an]->rr=_a_rr(packet,eom,&scan);
			break;

			case ns_t_srv:
				reply[an]->rr=_srv_rr(packet,eom,&scan);
			break;

			default:
				scan=(unsigned char *)(scan + len);
				continue;
		}

		if(reply[an]->rr == NULL)
		{
			free(reply[an]);
			reply[an] = NULL;
			break;
		}

		an++;
	}

	for(n=0;n<an-1;n++)
		reply[n]->next=reply[n+1];

	first=reply[0];

	free(reply);

	return first;
}//dns_host_t dns_resolve(const char *zone, int query_type)


void dns_free(dns_host_t dns)
{
	dns_host_t next;

	while(dns!=NULL)
	{
		next = dns->next;
		free(dns->rr);
		free(dns);
		dns = next;
	}
}//void dns_free(dns_host_t dns)
