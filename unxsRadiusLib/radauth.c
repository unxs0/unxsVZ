/*
FILE
	radauth.c
	svn ID removed
PURPOSE
	uRadiusLib public API functions for authentication and authorization
AUTHOR
	(C) 2006, Gary Wallis. GPL2 License applies.
	Derivative work based on Cistron Radius: Copyright 2001, Cistron Internet Services B.V.
	That also was GPL2 licensed.

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
int result_recv(UINT4 host, u_short udp_port, char *buffer, int length, char *cRetAttr);
int RadiusAuth(const char *cLogin,const char *cPasswd,const char *cRadiusServer,
	const char *cSharedSecret,char *cAuxAttributes, unsigned short uAttempts);


//Receive and print the result.
//Printing is done way deep in a log.c function based on debug_flag>0.
int result_recv(UINT4 host, u_short udp_port, char *buffer, int length, char *cRetAttr)
{
	AUTH_HDR	*auth;
	int		totallen;
	char		reply_digest[AUTH_VECTOR_LEN];
	char		calc_digest[AUTH_VECTOR_LEN];
	int		secretlen;
	AUTH_REQ	*authreq;
	VALUE_PAIR	*vp;

	auth = (AUTH_HDR *)buffer;
	totallen = ntohs(auth->length);

	if(totallen != length)
	{
		fprintf(stderr,"Received invalid reply length from server (want %d/ got %d)\n", totallen, length);
		return(8);
	}

	//Verify the reply digest
	memcpy(reply_digest, auth->vector, AUTH_VECTOR_LEN);
	memcpy(auth->vector, vector, AUTH_VECTOR_LEN);
	secretlen = strlen(secretkey);
	memcpy(buffer + length, secretkey, secretlen);
	md5_calc((u_char *)calc_digest,(u_char *)auth, length + secretlen);

	if(memcmp(reply_digest, calc_digest, AUTH_VECTOR_LEN) != 0)
	{
		fprintf(stderr,"Warning: Received invalid reply digest from server\n");
	}

	authreq = radrecv(htonl(host), udp_port, (u_char *)buffer, length);
	for(vp = authreq->request; vp; vp = vp->next)
	{
		if(vp->flags.encrypt)
		{
			//Note: vector is the one from the request we sent
			decrypt_attr(secretkey, vector, vp);
		}
	}

	if(auth->code == PW_AUTHENTICATION_REJECT)
		return(1);

	//Return first two  attributes
	unsigned uCount=0;
	char cOneAttrPair[256]={""};
	cRetAttr[0]=0;
	for(vp = authreq->request; vp && uCount<2; vp = vp->next)
	{
		sprintf(cOneAttrPair,"%.99s=%.99s;\n",vp->name,vp->strvalue);
		strcat(cRetAttr,cOneAttrPair);
		uCount++;
	}

	return(0);

}//int result_recv()


//Seems to use /etc/services to get port number to use?
int getport(char *svc, int def)
{
	struct	servent		*svp;
	int			n;

	if ((n = atoi(svc)) > 0) return n;

	svp = getservbyname (svc, "udp");
	return svp ? ntohs((u_short)svp->s_port) : def;

}//int getport(char *svc, int def)


//Positive auth results sent back in non const char *cAuxAttributes.
int RadiusAuth(const char *cLogin,const char *cPasswd,const char *cRadiusServer,
	const char *cSharedSecret,char *cAuxAttributes, unsigned short uAttempts)
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

	unsigned uNASPort=12;
	char cHost[100]={"testnas0"};
	char cAttributes[2048]={""};
	char *cp,*cp2;

	radius_dir = RADIUS_DIR;
	port = 0;
	secretkey=(char *)cSharedSecret;
	progname="radauth";

	//echo "User-Name = \"$1\""
	//echo "Password = \"$2\""
	//echo "NAS-IP-Address = $nas"
	//echo "NAS-Port = $4"
	sprintf(cAttributes,"User-Name = \"%.31s\"\nUser-Password = \"%.63s\"\nNAS-IP-Address = %.99s\n"
				"NAS-Port = %u\n%.1023s",cLogin,cPasswd,cHost,uNASPort,cAuxAttributes);


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
	if (port == 0) port = getport("radius", PW_AUTH_UDP_PORT);
	code = PW_AUTHENTICATION_REQUEST;

        //Resolve hostname.
	server_ip = get_ipaddr((char *)cRadiusServer);
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
			if(userparse(cp,&vp,1)!=0)
			{
				fprintf(stderr,"cannot parse %s\n",cp);
				return(4);
			}
			pairadd(&req,vp);
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
					vp->strvalue, secretkey,(char *) auth->vector);
		}
	}

	//Build final radius packet.
	length = rad_build_packet(auth, sizeof(send_buffer),
		req, NULL, secretkey, (char *)auth->vector);
	memcpy(vector, (char *)auth->vector, sizeof(vector));

	//Send the request we've built.
	//printf("Sending request to server %s, port %d.\n",cRadiusServer,port);
	for(i = 0; i < uAttempts; i++)
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
			sizeof(recv_buffer), 0, &saremote,(unsigned *) &salen);
		if (result >= 0)
			break;
		sleep(tv.tv_sec);
	}

	if(result > 0 && i < uAttempts )
	{
		return(result_recv(sin->sin_addr.s_addr, sin->sin_port,
			(char *)recv_buffer, result, cAuxAttributes));
	}

	fprintf(stderr,"No answer.\n");
	close(sockfd);
	return(7);

}//int RadiusAuth()


