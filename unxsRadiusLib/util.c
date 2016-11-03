/*
 *
 * util.c	Several miscellaneous functions. For example to
 *		deal with hostnames and IP addresses (in host-order,
 *		how braindead can you get...)
 *
 *		Copyright 1996-2001	Cistron Internet Services B.V.
 *		Copyright 2002		Cistron IP B.V.
 */

char util_rcsid[] =
"svn ID removed

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/time.h>
#include	<netinet/in.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<netdb.h>
#include	<pwd.h>
#include	<time.h>
#include	<ctype.h>
#include	<signal.h>

#include	"radiusd.h"

int	use_dns = 1;

/*
 *	Return a printable host name (or IP address in dot notation)
 *	for the supplied IP address.
 */
char * ip_hostname(UINT4 ipaddr)
{
	struct		hostent *hp;
	static char	hstname[128];
	UINT4		n_ipaddr;

	n_ipaddr = htonl(ipaddr);
	if (use_dns)
		hp = gethostbyaddr((char *)&n_ipaddr,
			sizeof (struct in_addr), AF_INET);
	else
		hp = NULL;
	if (hp == NULL) {
		ipaddr2str(hstname, ipaddr);
		return(hstname);
	}
	return (char *)hp->h_name;
}


/*
 *	Return an IP address in host long notation from a host
 *	name or address in dot notation.
 */
UINT4 get_ipaddr(char *host)
{
	struct hostent	*hp;
	UINT4		ipstr2long();

	if(good_ipaddr(host) == 0) {
		return(ipstr2long(host));
	}
	else if((hp = gethostbyname(host)) == (struct hostent *)NULL) {
		return((UINT4)0);
	}
	return(ntohl(*(UINT4 *)hp->h_addr));
}


/*
 *	Check for valid IP address in standard dot notation.
 */
int good_ipaddr(char *addr)
{
	int	dot_count;
	int	digit_count;

	dot_count = 0;
	digit_count = 0;
	while(*addr != '\0' && *addr != ' ') {
		if(*addr == '.') {
			dot_count++;
			digit_count = 0;
		}
		else if(!isdigit(*addr)) {
			dot_count = 5;
		}
		else {
			digit_count++;
			if(digit_count > 3) {
				dot_count = 5;
			}
		}
		addr++;
	}
	if(dot_count != 3) {
		return(-1);
	}
	else {
		return(0);
	}
}


/*
 *	Return an IP address in standard dot notation for the
 *	provided address in host long notation.
 */
void ipaddr2str(char *buffer, UINT4 ipaddr)
{
	int	addr_byte[4];
	int	i;
	UINT4	xbyte;

	for(i = 0;i < 4;i++) {
		xbyte = ipaddr >> (i*8);
		xbyte = xbyte & (UINT4)0x000000FF;
		addr_byte[i] = xbyte;
	}
	sprintf(buffer, "%u.%u.%u.%u", addr_byte[3], addr_byte[2],
		addr_byte[1], addr_byte[0]);
}


/*
 *	Return an IP address in host long notation from
 *	one supplied in standard dot notation.
 */
UINT4 ipstr2long(char *ip_str)
{
	char	buf[6];
	char	*ptr;
	int	i;
	int	count;
	UINT4	ipaddr;
	int	cur_byte;

	ipaddr = (UINT4)0;
	for(i = 0;i < 4;i++) {
		ptr = buf;
		count = 0;
		*ptr = '\0';
		while(*ip_str != '.' && *ip_str != '\0' && count < 4) {
			if(!isdigit(*ip_str)) {
				return((UINT4)0);
			}
			*ptr++ = *ip_str++;
			count++;
		}
		if(count >= 4 || count == 0) {
			return((UINT4)0);
		}
		*ptr = '\0';
		cur_byte = atoi(buf);
		if(cur_byte < 0 || cur_byte > 255) {
			return((UINT4)0);
		}
		ip_str++;
		ipaddr = ipaddr << 8 | (UINT4)cur_byte;
	}
	return(ipaddr);
}


/*
 *	Call getpwnam but cache the result.
 */
struct passwd *rad_getpwnam(char *name)
{
	static struct passwd *lastpwd;
	static char lastname[64];
	static time_t lasttime = 0;
	time_t now;

	now = time(NULL);

	if ((now <= lasttime + 5 ) && strncmp(name, lastname, 64) == 0)
		return lastpwd;

	strncpy(lastname, name, 63);
	lastname[63] = 0;
	lastpwd = getpwnam(name);
	lasttime = now;

	return lastpwd;
}

/*
 *	Free an AUTHREQ struct.
 */
void authfree(AUTH_REQ *authreq)
{
	pairfree(authreq->request);
	pairfree(authreq->proxy_pairs);
	pairfree(authreq->server_reply);
	memset(authreq, 0, sizeof(AUTH_REQ));
	free(authreq);
}

#if (defined (sun) && defined(__svr4__)) || defined(__hpux__) || defined(aix)
/*
 *	The signal() function in Solaris 2.5.1 sets SA_NODEFER in
 *	sa_flags, which causes grief if signal() is called in the
 *	handler before the cause of the signal has been cleared.
 *	(Infinite recursion).
 */
void (*sun_signal(int signo, void (*func)(int)))(int)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
#ifdef  SA_INTERRUPT		/* SunOS */
	act.sa_flags |= SA_INTERRUPT;
#endif
	if (sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;
	return oact.sa_handler;
}
#endif

/*
 *	Like strncpy, but makes sure that the string
 *	always ends with a trailing \0
 */
char *strNcpy(char *dest, char *src, int n)
{
	if (n > 0)
		strncpy(dest, src, n);
	else
		n = 1;
	dest[n - 1] = 0;

	return dest;
}

