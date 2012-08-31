/*
FILE
	unxsSIPProxy/dns.c
PURPOSE
	Simple dns srv resolver testing
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


int main(int iArgc, char *cArgv[])
{
	dns_host_t sDnsHost;
	dns_host_t sDnsHostSave;
	dns_srv_t sDnsSrv;

	if(iArgc<2)
	{
		printf("usage %s: cHostname\n",cArgv[0]);
		exit(1);
	}

	sDnsHost=dns_resolve(cArgv[1],3);
	sDnsHostSave=sDnsHost;
	if(sDnsHost!=NULL)
	{
		printf("type:%d class:%d ttl:%d\n",sDnsHost->type,sDnsHost->class,sDnsHost->ttl);
		sDnsSrv=(dns_srv_t)sDnsHost->rr;
		printf("name:%s priority:%d weight:%d port:%d rweight:%d\n",
			sDnsSrv->name,sDnsSrv->priority,sDnsSrv->weight,sDnsSrv->port,sDnsSrv->rweight);
		while(sDnsHost->next!=NULL)
		{
			sDnsHost=sDnsHost->next;
			printf("type:%d class:%d ttl:%d\n",sDnsHost->type,sDnsHost->class,sDnsHost->ttl);
			sDnsSrv=(dns_srv_t)sDnsHost->rr;
			printf("name:%s priority:%d weight:%d port:%d rweight:%d\n",
				sDnsSrv->name,sDnsSrv->priority,sDnsSrv->weight,sDnsSrv->port,sDnsSrv->rweight);
		}
	}
	dns_free(sDnsHostSave);
	return(0);
}

/* compare two srv structures, order by priority then by randomised weight */
static int _srv_compare(const void *a, const void *b) {
    dns_host_t ah = * (dns_host_t *) a, bh = * (dns_host_t *) b;
    dns_srv_t arr, brr;

    if(ah == NULL) return 1;
    if(bh == NULL) return -1;

    arr = (dns_srv_t) ah->rr;
    brr = (dns_srv_t) bh->rr;

    if(arr->priority > brr->priority) return 1;
    if(arr->priority < brr->priority) return -1;

    if(arr->rweight > brr->rweight) return -1;
    if(arr->rweight < brr->rweight) return 1;
    
    return 0;
}


/* unix implementation */

/* the largest packet we'll send and receive */
#define MAX_PACKET (1024)

typedef union {
    HEADER          hdr;
    unsigned char   buf[MAX_PACKET];
} dns_packet_t;

static void *_a_rr(dns_packet_t packet, unsigned char *eom, unsigned char **scan) {
    struct in_addr in;

    NS_GET32(in.s_addr, *scan);
    in.s_addr = ntohl(in.s_addr);

    return strdup(inet_ntoa(in));
}

static void *_srv_rr(dns_packet_t packet, unsigned char *eom, unsigned char **scan) {
    unsigned int priority, weight, port;
    int len;
    char host[256];
    dns_srv_t srv;

    NS_GET16(priority, *scan);
    NS_GET16(weight, *scan);
    NS_GET16(port, *scan);

    len = dn_expand(packet.buf, eom, *scan, host, 256);
    if (len < 0)
        return NULL;
    *scan = (unsigned char *) (*scan + len);

    srv = (dns_srv_t) malloc(sizeof(struct dns_srv_st));

    srv->priority = priority;
    srv->weight = weight;
    srv->port = port;

    /* add a random factor to the weight, for load balancing and such */
    if(weight != 0)
        srv->rweight = 1 + rand() % (10000 * weight);
    else
        srv->rweight = 0;

    strcpy(srv->name, host);

    return (void *) srv;
}

/** the actual resolver function */
dns_host_t dns_resolve(const char *zone, int query_type) {
    char host[256];
    dns_packet_t packet;
    int len, qdcount, ancount, an, n;
    unsigned char *eom, *scan;
    dns_host_t *reply, first;
    unsigned int t_type, type, class, ttl;

    if(zone == NULL || *zone == '\0')
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

    /* do the actual query */
    if((len = res_query(zone, ns_c_in, t_type, packet.buf, MAX_PACKET)) == -1 || len < sizeof(HEADER))
        return NULL;

    /* we got a valid result, containing two types of records - packet
     * and answer .. we have to skip over the packet records */

    /* no. of packets, no. of answers */
    qdcount = ntohs(packet.hdr.qdcount);
    ancount = ntohs(packet.hdr.ancount);

    /* end of the returned message */
    eom = (unsigned char *) (packet.buf + len);

    /* our current location */
    scan = (unsigned char *) (packet.buf + sizeof(HEADER));

    /* skip over the packet records */
    while(qdcount > 0 && scan < eom) {
        qdcount--;
        if((len = dn_expand(packet.buf, eom, scan, host, 256)) < 0)
            return NULL;
        scan = (unsigned char *) (scan + len + NS_QFIXEDSZ);
    }

    /* create an array to store the replies in */
    reply = (dns_host_t *) malloc(sizeof(dns_host_t) * ancount);
    memset(reply, 0, sizeof(dns_host_t) * ancount);

    an = 0;
    /* loop through the answer buffer and extract SRV records */
    while(ancount > 0 && scan < eom ) {
        ancount--;
        len = dn_expand(packet.buf, eom, scan, host, 256);
        if(len < 0) {
            for(n = 0; n < an; n++)
                free(reply[n]);
            free(reply);
            return NULL;
        }

        scan += len;

        /* extract the various parts of the record */
        NS_GET16(type, scan);
        NS_GET16(class, scan);
        NS_GET32(ttl, scan);
        NS_GET16(len, scan);

        /* skip records we're not interested in */
        if(type != t_type) {
            scan = (unsigned char *) (scan + len);
            continue;
        }

        /* create a new reply structure to save it in */
        reply[an] = (dns_host_t) malloc(sizeof(struct dns_host_st));

        reply[an]->type = type;
        reply[an]->class = class;
        reply[an]->ttl = ttl;

        reply[an]->next = NULL;

        /* type-specific processing */
        switch(type)
        {
            case ns_t_a:
                reply[an]->rr = _a_rr(packet, eom, &scan);
                break;

            case ns_t_srv:
                reply[an]->rr = _srv_rr(packet, eom, &scan);
                break;

            default:
                scan = (unsigned char *) (scan + len);
                continue;
        }

        /* fell short, we're done */
        if(reply[an]->rr == NULL)
        {
            free(reply[an]);
            reply[an] = NULL;
            break;
        }

        /* on to the next one */
        an++;
    }

    /* sort srv records them */
    if(t_type == ns_t_srv)
        qsort(reply, an, sizeof(dns_host_t), _srv_compare);

    /* build a linked list out of the array elements */
    for(n = 0; n < an - 1; n++)
        reply[n]->next = reply[n + 1];

    first = reply[0];

    free(reply);

    return first;
}


void dns_free(dns_host_t dns) {
    dns_host_t next;

    while(dns != NULL) {
        next = dns->next;
        free(dns->rr);
        free(dns);
        dns = next;
    }
}
