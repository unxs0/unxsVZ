/*
FILE
	radacct.c
	svn ID removed
PURPOSE
	uRadiusLib public API functions for accounting
AUTHOR
	(C) 2006, Gary Wallis. GPL License applies.
	Derivative work based on Cistron Radius: Copyright 2001, Cistron Internet Services B.V.

*/

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/time.h>
#include	<netinet/in.h>

#include	<stdio.h>
#include	<time.h>
#include	<unistd.h>
#include	<netdb.h>
#include	<pwd.h>
#include	<stdlib.h>
#include	<errno.h>
#include	<ctype.h>

#include	"radiusd.h"
#include	"conf.h"

int		send_buffer[512];
int		recv_buffer[512];
char		vector[AUTH_VECTOR_LEN];
char		*secretkey;
char		*progname;

//local proto
int RadiusAcct(char *cRadiusServer,char *cSharedSecret, char *cAttributes);

//Receive and print the result.
//Printing is done way deep in a log.c function based on debug_flag>0.
int result_recv(UINT4 host, u_short udp_port, char *buffer, int length);
int getport(char *svc, int def);


int RadiusAcct(char *cRadiusServer,char *cSharedSecret,char *cAttributes)
{
	AUTH_HDR		*auth;
	VALUE_PAIR		*req, *vp;
	UINT4			local_ip=0;
	struct	sockaddr	salocal;
	struct	sockaddr	saremote;
	struct	sockaddr_in	*sin;
	struct timeval		tv;
	fd_set			readfds;
	char			*radius_dir;
	UINT4			server_ip;
	int			port;
	int			code;
	int			sockfd;
	int			salen;
	int			result;
	int			length;
	int			i;

	char *cp,*cp2,*cp3;

	radius_dir = RADIUS_DIR;
	port = 0;
	secretkey=cSharedSecret;
	progname="radacct";


	//Initialize dictionary.
	if(dict_init(radius_dir, NULL) < 0)
	{
		fprintf(stderr,"Could not initialize radius directory\n");
		return(2);
	}


        //Strip port from hostname if needed.
        if((cp=strchr(cRadiusServer,':'))!=NULL)
	{
		*cp++ = 0;
                port=getport(cp,0);
        }

        //Set the kind of request we want to send.
	if (port == 0) port = getport("radacct", PW_ACCT_UDP_PORT);
		code = PW_ACCOUNTING_REQUEST;

        //Resolve hostname.
	server_ip = get_ipaddr(cRadiusServer);
	if(server_ip == 0)
	{
		fprintf(stderr,"unknown host %s\n",cRadiusServer);
		return(3);
	}


	//Single line for now
	req = NULL;
	cp=cAttributes;
	while((*cp)!=0)
	{
		vp = NULL;
		if((cp2=strchr(cp,'\n')))
		{
			//Maybe we should cut the line out?
			//like so...
			cp2++;
			cp3=cp2;
			*cp2=0;
			if(userparse(cp,&vp,1)!=0)
			{
				fprintf(stderr,"cannot parse %s\n",cp);
				return(4);
			}
			pairadd(&req,vp);
			*cp2=*cp3;
		}
		cp=cp2;
		cp++;
	}


	//Open a socket.
	sockfd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		fprintf(stderr,"socket\n");
		return(5);
	}

	if(local_ip)
	{
		int n;
		char b[32];

		sin=(struct sockaddr_in *) &salocal;
       		memset(sin, 0, sizeof (salocal));
		sin->sin_family = AF_INET;
		sin->sin_addr.s_addr = htonl(local_ip);

		for(n = 1024; n < 65000; n++)
		{
			sin->sin_port = htons(n);
			if(bind(sockfd, &salocal, sizeof(salocal)) == 0)
				break;
			if(errno != EADDRINUSE)
			{
				n = 65000;
				break;
			}
		}
		if(n == 65000)
		{
			ipaddr2str(b,local_ip);
			fprintf(stderr,"can't bind to local address %s\n",b);
			return(6);
		}
	}

	sin=(struct sockaddr_in *) &saremote;
        memset (sin, 0, sizeof (saremote));
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = htonl(server_ip);
	sin->sin_port = htons(port);

	//Set up AUTH structure.
	memset(send_buffer, 0, sizeof(send_buffer));
	auth = (AUTH_HDR *)send_buffer;
	auth->code = code;
	if (code == PW_AUTHENTICATION_REQUEST ||
	    code == PW_STATUS_SERVER)
		random_vector(auth->vector);
	auth->id = getpid() & 255;

	//Find password and encode it.
	for (vp = req; vp; vp = vp->next)
	{
		if(vp->attribute == PW_PASSWORD)
		{
			vp->length = rad_pwencode(vp->strvalue,
					vp->strvalue, (char *)secretkey,(char *) auth->vector);
		}
	}

	//Build final radius packet.
	length = rad_build_packet(auth, sizeof(send_buffer),
		req, NULL, secretkey,(char *) auth->vector);
	memcpy(vector, auth->vector, sizeof(vector));

	//Send the request we've built.
	//printf("Sending request to server %s, port %d.\n",cRadiusServer,port);
	for(i = 0; i < 10; i++)
	{
		if (i > 0) fprintf(stderr,"Re-sending request.\n");
		sendto(sockfd, (char *)auth, length, 0,
			&saremote, sizeof(struct sockaddr_in));

		tv.tv_sec = 3;
		tv.tv_usec = 0;
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		if (select(sockfd + 1, &readfds, NULL, NULL, &tv) == 0)
			continue;
		salen = sizeof (saremote);
		result = recvfrom (sockfd, (char *)recv_buffer,
			sizeof(recv_buffer), 0, &saremote, (unsigned *)&salen);
		if (result >= 0)
			break;
		sleep(tv.tv_sec);
	}

	if(result > 0 && i < 10)
	{
		return(result_recv(sin->sin_addr.s_addr, sin->sin_port,
			(char *)recv_buffer, result));
	}

	fprintf(stderr,"No answer.\n");
	close(sockfd);
	return(7);

}//int RadiusAcct()
