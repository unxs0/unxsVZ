/*
 * radius.c	Miscellanous generic functions.
 *
 *		Copyright 1998-2001	Cistron Internet Services B.V.
 *		Copyright 2002		Cistron IP B.V.
 *
 */

char radius_rcsid[] =
"svn ID removed

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/time.h>
#include	<netinet/in.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<pwd.h>
#include	<time.h>
#include	<ctype.h>

#include	"radiusd.h"

/*
 *	Make sure our buffer is aligned.
 */
static int	i_send_buffer[1024];
static char	*send_buffer = (char *)i_send_buffer;

/*
 *	Generate AUTH_VECTOR_LEN worth of random data.
 */
void random_vector(unsigned char *vector)
{
	static int	did_srand = 0;
	unsigned int	i;
	int		n;

#if defined(__linux__) || defined(BSD)
	if (!did_srand) {
		/*
		 *	Try to use /dev/urandom to seed the
		 *	random generator. Not all *BSDs have it
		 *	but it doesn't hurt to try.
		 */
		if ((n = open("/dev/urandom", O_RDONLY)) >= 0 &&
		     read(n, (char *)&i, sizeof(i)) == sizeof(i)) {
			srandom(i);
			did_srand = 1;
		}
		if (n >= 0) close(n);
	}

	if (!did_srand) {
		/*
		 *	Use more traditional way to seed.
		 */
		srandom(time(NULL) + getpid());
		did_srand = 1;
	}

	for (i = 0; i < AUTH_VECTOR_LEN;) {
		n = random();
		memcpy(vector, &n, sizeof(int));
		vector += sizeof(int);
		i += sizeof(int);
	}
#else

#ifndef RAND_MAX
#  define RAND_MAX 32768
#endif
	/*
	 *	Assume the system has neither /dev/urandom
	 *	nor random()/srandom() but just the old
	 *	rand() / srand() functions.
	 */
	if (!did_srand) {
		char garbage[8];
		i = time(NULL) + getpid();
		for (n = 0; n < 8; n++) i+= (garbage[n] << i);
		srand(i);
		did_srand = 1;
	}

	for (i = 0; i < AUTH_VECTOR_LEN;) {
		unsigned char c;
		/*
		 *	Don't use the lower bits, also don't use
		 *	parts > RAND_MAX since they are zero.
		 */
		n = rand() / (RAND_MAX / 256);
		c = n;
		memcpy(vector, &c, sizeof(c));
		vector += sizeof(c);
		i += sizeof(c);
	}
#endif
}

/*
 *	Build radius packet. We assume that the header part
 *	of AUTH_HDR has already been filled in, we just
 *	fill auth->data with the A/V pairs from reply.
 */
int rad_build_packet(AUTH_HDR *auth, int auth_len,
	VALUE_PAIR *reply, char *msg, char *secret, char *vector)
{
	VALUE_PAIR		*vp;
	u_short			total_length;
	u_char			*ptr, *length_ptr;
	char			digest[16];
	int			vendorpec;
	int			len;
	UINT4			lvalue;

	total_length = AUTH_HDR_LEN;

	/*
	 *	Load up the configuration values for the user
	 */
	ptr = auth->data;
	for (vp = reply; vp; vp = vp->next) {
		/*
		 *	Check for overflow.
		 */
		if (total_length + vp->length + 16 >= auth_len)
			break;
		debug_pair(stdout, vp);

		/*
		 *	This could be a vendor-specific attribute.
		 */
		length_ptr = NULL;
		if ((vendorpec = VENDOR(vp->attribute)) > 0) {
			*ptr++ = PW_VENDOR_SPECIFIC;
			length_ptr = ptr;
			*ptr++ = 6;
			lvalue = htonl(vendorpec);
			memcpy(ptr, &lvalue, 4);
			ptr += 4;
			total_length += 6;
		} else if (vp->attribute > 0xff) {
			/*
			 *	Ignore attributes > 0xff
			 */
			continue;
		} else
			vendorpec = 0;

#ifdef ATTRIB_NMC
		if (vendorpec == VENDORPEC_USR) {
			lvalue = htonl(vp->attribute & 0xFFFF);
			memcpy(ptr, &lvalue, 4);
			total_length += 2;
			*length_ptr  += 2;
			ptr          += 4;
		} else
#endif
		*ptr++ = (vp->attribute & 0xFF);

		switch(vp->type) {

		case PW_TYPE_STRING:
			/*
			 *	FIXME: this is just to make sure but
			 *	should NOT be needed. In fact I have no
			 *	idea if it is needed :)
			 */
			if (vp->length == 0 && vp->strvalue[0] != 0) {
				vp->length = strlen(vp->strvalue);
			}
			if (vp->length >= AUTH_STRING_LEN) {
				vp->length = AUTH_STRING_LEN - 1;
			}

			/*
			 * If the flags indicate a encrypted attribute, handle 
			 * it here. I don't want to go through the reply list 
			 * another time just for transformations like this.
			 */
			if (vp->flags.encrypt) encrypt_attr(secret, vector, vp);
			
			/*
			 * vp->length is the length of the string value; len
			 * is the length of the string field in the packet.
			 * Normally, these are the same, but if a tag is
			 * inserted only len will reflect this.
			 *
			 * Bug fixed: for tagged attributes with 'tunnel-pwd'
			 * encryption, the tag is *always* inserted, regardless
			 * of its value! (Another strange thing in RFC 2868...)
			 */
			len = vp->length + (vp->flags.has_tag &&
					    (TAG_VALID(vp->flags.tag) ||
					     vp->flags.encrypt == 1));
			    
#ifdef ATTRIB_NMC
			if (vendorpec != VENDORPEC_USR)
#endif
				*ptr++ = len + 2;
			if (length_ptr) *length_ptr += len + 2;

			/* Insert the tag (sorry about the fast ugly test...) */
			if (len > vp->length) *ptr++ = vp->flags.tag;

			/* Use the original length of the string value */
			memcpy(ptr, vp->strvalue, vp->length);
			ptr += vp->length; 		/* here too */
			total_length += len + 2;
			break;

		case PW_TYPE_INTEGER:
		case PW_TYPE_DATE:
		case PW_TYPE_IPADDR:
			len = sizeof(UINT4) + (vp->flags.has_tag && 
					       vp->type != PW_TYPE_INTEGER);
#ifdef ATTRIB_NMC
			if (vendorpec != VENDORPEC_USR)
#endif
				*ptr++ = len + 2;
			if (length_ptr) *length_ptr += len + 2;
			
			/* Handle tags */
			lvalue = vp->lvalue;
			if (vp->flags.has_tag) {
				if (vp->type == PW_TYPE_INTEGER) {
					/* Tagged integer: MSB is tag */
					lvalue = (lvalue & 0xffffff) |
						 ((vp->flags.tag & 0xff) << 24);
				}
				else {
					/* Something else: insert the tag */
					*ptr++ = vp->flags.tag;
				}
			}
			lvalue = htonl(lvalue);
			memcpy(ptr, &lvalue, sizeof(UINT4));
			ptr += sizeof(UINT4);
			total_length += len + 2;
			break;

		default:
			break;
		}
	}

	/*
	 *	Append the user message
	 *	FIXME: add multiple PW_REPLY_MESSAGEs if it
	 *	doesn't fit into one.
	 */
	if (msg && msg[0]) {
		len = strlen(msg);
		if (len > 0 && len < AUTH_STRING_LEN-1) {
			*ptr++ = PW_REPLY_MESSAGE;
			*ptr++ = len + 2;
			memcpy(ptr, msg, len);
			ptr += len;
			total_length += len + 2;
		}
	}

	auth->length = htons(total_length);

	if (auth->code != PW_AUTHENTICATION_REQUEST &&
	    auth->code != PW_STATUS_SERVER) {
		/*
		 *	Append secret and calculate the response digest
		 */
		len = strlen(secret);
		if (total_length + len < auth_len) {
			memcpy((char *)auth + total_length, secret, len);
			md5_calc((unsigned char *)digest, (unsigned char *)auth, total_length + len);
			memcpy(auth->vector, digest, AUTH_VECTOR_LEN);
			memset(send_buffer + total_length, 0, len);
		}
	}

	return total_length;
}

/*
 *	Reply to the request.  Also attach
 *	reply attribute value pairs and any user message provided.
 */
int rad_send_reply(int code, AUTH_REQ *authreq, VALUE_PAIR *oreply,
			char *msg, int activefd)
{
	AUTH_HDR		*auth;
	VALUE_PAIR		*reply;
	struct	sockaddr	saremote;
	struct	sockaddr_in	*sin;
	u_short			total_length;
	char			*what;
	char			hn[16];

	auth = (AUTH_HDR *)send_buffer;
	reply = oreply;

	switch(code) {
		case PW_PASSWORD_REJECT:
		case PW_AUTHENTICATION_REJECT:
			what = "Reject";
			/*
			 *	Also delete all reply attributes
			 *	except proxy-pair and port-message.
			 */
			reply = NULL;
			pairmove2(&reply, &oreply, PW_REPLY_MESSAGE);
			break;
		case PW_ACCESS_CHALLENGE:
			what = "Challenge";
			break;
		case PW_AUTHENTICATION_ACK:
			what = "Ack";
			break;
		case PW_ACCOUNTING_RESPONSE:
			what = "Accounting Ack";
			break;
		default:
			what = "Reply";
			break;
	}

	/* this could be more efficient, but it works... */
	pairmove2(&reply, &authreq->proxy_pairs, PW_PROXY_STATE);

	/*
	 *	Build standard header
	 */
	auth->code = code;
	auth->id = authreq->id;
	memcpy(auth->vector, authreq->vector, AUTH_VECTOR_LEN);

	ipaddr2str(hn, authreq->ipaddr);
	DEBUG("Sending %s of id %d to %s",
		what, authreq->id, hn);

	total_length = rad_build_packet(auth, sizeof(i_send_buffer),
				reply, msg, (char *)authreq->secret, (char *)authreq->vector);

	sin = (struct sockaddr_in *) &saremote;
        memset ((char *) sin, '\0', sizeof (saremote));
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = htonl(authreq->ipaddr);
	sin->sin_port = htons(authreq->udp_port);

	/*
	 *	Send it to the user
	 */
	sendto(activefd, (char *)auth, (int)total_length, (int)0,
			&saremote, sizeof(struct sockaddr_in));

	/*
	 *	Just to be tidy move pairs back.
	 */
	pairmove2(&authreq->proxy_pairs, &reply, PW_PROXY_STATE);
	if (reply != oreply)
		pairmove2(&oreply, &reply, PW_REPLY_MESSAGE);

	return 0;
}

/*
 *	Validates the requesting client NAS.  Calculates the
 *	digest to be used for decrypting the users password
 *	based on the clients private key.
 */
int calc_digest(u_char *digest, AUTH_REQ *authreq)
{
	u_char		buffer[128];
	int		secretlen;

	/*
	 *	Use the secret to setup the decryption digest
	 */
	secretlen = strlen((char *)authreq->secret);
	strNcpy((char *)buffer, (char *)authreq->secret, sizeof(buffer));
	memcpy(buffer + secretlen, authreq->vector, AUTH_VECTOR_LEN);
	md5_calc(digest, buffer, secretlen + AUTH_VECTOR_LEN);
	memset(buffer, 0, sizeof(buffer));

	return(0);
}

/*
 *	Validates the requesting client NAS.  Calculates the
 *	signature based on the clients private key.
 */
int calc_acctdigest(u_char *digest, AUTH_REQ *authreq)
{
	char		zero[AUTH_VECTOR_LEN];
	u_char		*recvbuf = authreq->data;
	u_char		*tmpbuf;
	int		secretlen;
	int		len = authreq->data_len;

	/*
	 *	Older clients have the authentication vector set to
	 *	all zeros. Return `1' in that case.
	 */
	memset(zero, 0, sizeof(zero));
	if (memcmp(authreq->vector, zero, AUTH_VECTOR_LEN) == 0)
		return 1;

	/*
	 *	Zero out the auth_vector in the received packet.
	 *	Then append the shared secret to the received packet,
	 *	and calculate the MD5 sum. This must be the same
	 *	as the original MD5 sum (authreq->vector).
	 */
	secretlen = strlen((char *)authreq->secret);
	memset(recvbuf + 4, 0, AUTH_VECTOR_LEN);

	if ((tmpbuf = malloc(len + secretlen)) == NULL) {
		radlog(L_ERR|L_CONS, "no memory");
		exit(1);
	}
	memcpy(tmpbuf, recvbuf, len);
	memcpy(tmpbuf + len, authreq->secret, secretlen);
	md5_calc(digest, tmpbuf, len + secretlen);
	free(tmpbuf);

	/*
	 *	Return 0 if OK, 2 if not OK.
	 */
	return memcmp(digest, authreq->vector, AUTH_VECTOR_LEN) ? 2 : 0;
}

/*
 *	Decode just one attribute.
 */
static VALUE_PAIR *decode_attr(AUTH_REQ *authreq, u_char *ptr,
				int attribute, int attrlen)
{
	VALUE_PAIR	*pair;
	DICT_ATTR	*attr;
	UINT4		lvalue;

	if ((pair = (VALUE_PAIR *)malloc(sizeof(VALUE_PAIR))) == NULL) {
		radlog(L_ERR|L_CONS, "no memory");
		exit(1);
	}
	memset(pair, 0, sizeof(VALUE_PAIR));

	if ((attr = dict_attrget(attribute)) == NULL) {
		/*DEBUG("Received unknown attribute %d", attribute);*/
		if (attribute > 0xffff)
			sprintf(pair->name, "Attr-%d-%d", attribute >> 16,
				attribute & 0xffff);
		else
			sprintf(pair->name, "Attr-%d", attribute);
		pair->type = PW_TYPE_STRING;
	} else {
		strcpy(pair->name, attr->name);
		pair->type = attr->type;
		pair->flags = attr->flags;	/* copy flags too */
	}

	if ( attrlen >= AUTH_STRING_LEN-1 ) {
		DEBUG("attribute %d too long, %d >= %d", attribute,
			attrlen, AUTH_STRING_LEN);
		free(pair);
		return NULL;
	}

	pair->attribute = attribute;
	pair->length = attrlen;
	pair->next = NULL;
	pair->operator = PW_OPERATOR_EQUAL;
	pair->strvalue[0] = '\0';

	switch (pair->type) {

		case PW_TYPE_STRING:
			/* attrlen always < AUTH_STRING_LEN */
			memset(pair->strvalue, 0, AUTH_STRING_LEN);

			/* Support tags */
			if (pair->flags.has_tag &&
			    (TAG_VALID(*ptr)||pair->flags.encrypt==1)) {
				pair->flags.tag = *ptr;
				pair->length--;
				memcpy(pair->strvalue, ptr + 1, 
				       pair->length);
				/* I don't want to touch ptr or attrlen,
				   that's why it's done like this... */
			}
			else {
				memcpy(pair->strvalue, ptr, attrlen);
				pair->flags.tag = 0;
			}

			/*
			 * Decrypt string attributes that have a nonzero
			 * encryption style flag. We only do this here 
			 * for *original requests*, not for responses 
			 * from servers we proxied to, as we don't have 
			 * access here to the original request we sent 
			 * to the server, of which we would need the 
			 * request authenticator to be able to decrypt.
			 * (Ugh! Yes sir.)
			 */
			if (pair->flags.encrypt &&
			    (authreq->code == PW_AUTHENTICATION_REQUEST
			  || authreq->code == PW_ACCOUNTING_REQUEST)) {
				decrypt_attr((char*)authreq->secret, 
					     (char*)authreq->vector, pair);
			}

			debug_pair(stdout, pair);
			break;

		case PW_TYPE_INTEGER:
		case PW_TYPE_IPADDR:
			/* Is pair->length nowhere needed anymore? */ 
	
			/* Support *inserted* tags for IP addresses */
			if (pair->flags.has_tag && 
			    pair->type != PW_TYPE_INTEGER) {
				pair->flags.tag = *ptr;
				memcpy(&lvalue, ptr + 1, sizeof(UINT4));
			}
			else {
				memcpy(&lvalue, ptr, sizeof(UINT4));
			}
			pair->lvalue = ntohl(lvalue);

			/* Normal integers have the tag in the MSB */
			if (pair->flags.has_tag &&
			    pair->type == PW_TYPE_INTEGER) {
				pair->flags.tag = (pair->lvalue >> 24) & 0xff;
				pair->lvalue &= 0xffffff;
			}

			debug_pair(stdout, pair);
			break;
		
		default:
			DEBUG("	   %s (Unknown Type %d)",
				pair->name,pair->type);
			free(pair);
			pair = NULL;
			break;
	}
	return pair;
}

/*
 *	Decode a list of attributes.
 *	On the Vendor-Specific attribute, recurse (once).
 */
static VALUE_PAIR *decode_attrs(AUTH_REQ *authreq, u_char *ptr,
				int length, int vendorpec)
{
	VALUE_PAIR	*first_pair, *prev;
	VALUE_PAIR	*pair;
	UINT4		lvalue;
	UINT4		pec;
	int		attribute;
	int		attrlen;

	first_pair = prev = NULL;

	while (length > 0) {
#ifdef ATTRIB_NMC
		if (vendorpec == VENDORPEC_USR) {
			if (length < 4) {
				DEBUG("VSA(USR) attribute too short (%d),"
					" skipped", length);
				length = 0;
				continue;
			}
			memcpy(&lvalue, ptr, 4);
			attribute = (ntohl(lvalue) & 0xFFFF) |
					(vendorpec << 16);
			ptr += 4;
			length -= 4;
			attrlen = length;
		} else
#endif
		{
			attribute = *ptr++ | (vendorpec << 16);
			attrlen   = *ptr++;
			attrlen -= 2;
			length  -= 2;
		}
		if (attrlen < 0) {
			if (vendorpec) {
				DEBUG("VSA(%d) attribute %d too short (%d),"
				" skipped", vendorpec, attribute&0xffff,
				attrlen+2);
			} else {
				DEBUG("Attribute %d too short (%d),"
				" skipped", attribute, attrlen+2);
			}
			length = 0;
			continue;
		}
		if ( attrlen > length ) {
			DEBUG("attribute %d longer as buffer left, %d > %d",
				attribute, attrlen, length);
			length = 0;
			continue;
		}

		/*
		 *	This could be a Vendor-Specific attribute.
		 *	In that case, recurse.
		 *
		 */
		pec = 0;
		if (vendorpec == 0 &&
		    attribute == PW_VENDOR_SPECIFIC && attrlen > 6) {
			memcpy(&lvalue, ptr, 4);
			pec = ntohl(lvalue);
			if (dict_vendor(pec)) {
				ptr += 4;
				attrlen -= 4;
				length -= 4;
			} else
				pec = 0;
		}

		if (pec > 0)
			pair = decode_attrs(authreq, ptr, attrlen, pec);
		else
			pair = decode_attr(authreq, ptr, attribute, attrlen);

		if (pair) {
			if (first_pair == NULL)
				first_pair = pair;
			else
				prev->next = pair;
			prev = pair;
		}

		ptr += attrlen;
		length -= attrlen;
	}
	return first_pair;
}

/*
 *	Receive UDP client requests, build an authorization request
 *	structure, and attach attribute-value pairs contained in
 *	the request to the new structure.
 */
AUTH_REQ	*radrecv(UINT4 host, u_short udp_port,
			u_char *buffer, int length)
{
	AUTH_HDR	*auth;
	AUTH_REQ	*authreq;
	VALUE_PAIR	*pair;
	u_char		*ptr;
	char		hn[16];
	int		totallen;

	/*
	 *	Pre-allocate the new request data structure
	 */

	if ((authreq = (AUTH_REQ *)malloc(sizeof(AUTH_REQ))) ==
						(AUTH_REQ *)NULL) {
		radlog(L_ERR|L_CONS, "no memory");
		exit(1);
	}
	memset(authreq, 0, sizeof(AUTH_REQ));

	auth = (AUTH_HDR *)buffer;
	totallen = ntohs(auth->length);
	if (length > totallen) length = totallen;

	ipaddr2str(hn, host);
	DEBUG("radrecv: Packet from host %s code=%d, id=%d, length=%d",
				hn, auth->code, auth->id, totallen);

	/*
	 *	Fill header fields
	 */
	authreq->ipaddr = host;
	authreq->udp_port = udp_port;
	authreq->id = auth->id;
	authreq->code = auth->code;
	memcpy(authreq->vector, auth->vector, AUTH_VECTOR_LEN);
	authreq->data = buffer;
	authreq->data_len = length;
	authreq->proxy_pairs = NULL;
	authreq->timestamp = time(NULL);

	/*
	 *	Extract attribute-value pairs
	 */
	ptr = auth->data;
	length -= AUTH_HDR_LEN;

	authreq->request = decode_attrs(authreq, ptr, length, 0);

	/* hide the proxy pairs from the rest of the server */
	pairmove2(&authreq->proxy_pairs, &authreq->request, PW_PROXY_STATE);

	/*
	 *	Create a Client-IP-Address attribute, and add it to the
	 *	user's request.
	 */
	if ((pair = malloc(sizeof(VALUE_PAIR))) == NULL) {
		radlog(L_CONS|L_ERR, "no memory");
		exit(1);
	}
	memset(pair, 0, sizeof(VALUE_PAIR));
	strcpy(pair->name, "Client-IP-Address");
	pair->attribute = PW_CLIENT_IP_ADDRESS;
	pair->type = PW_TYPE_IPADDR;
	pair->lvalue = host;
	pairadd(&authreq->request, pair);

	return(authreq);
}


/*
 *	Encode password.
 *
 *	Assume that "pwd_out" points to a buffer of
 *	at least AUTH_PASS_LEN bytes.
 *
 *	Returns new length.
 */
int rad_pwencode(char *pwd_in, char *pwd_out, char *secret, char *vector)
{
	char	passbuf[AUTH_PASS_LEN];
	char	md5buf[256];
	int	i, secretlen;

	i = strlen(pwd_in);
	memset(passbuf, 0, AUTH_PASS_LEN);
	memcpy(passbuf, pwd_in, i > AUTH_PASS_LEN ? AUTH_PASS_LEN : i);

	secretlen = strlen(secret);
	if (secretlen + AUTH_VECTOR_LEN > 256)
		secretlen = 256 - AUTH_VECTOR_LEN;
	strNcpy(md5buf, secret, sizeof(md5buf));
	memcpy(md5buf + secretlen, vector, AUTH_VECTOR_LEN);
	md5_calc((u_char *)pwd_out,(u_char *)md5buf, secretlen + AUTH_VECTOR_LEN);

	for (i = 0; i < AUTH_PASS_LEN; i++)
		*pwd_out++ ^= passbuf[i];

	return AUTH_PASS_LEN;
}

/*
 *	Decode password.
 *
 *	Assume that "pwd_out" points to a buffer of
 *	at least pwlen + 1 bytes.
 *
 *	Returns new length.
 */
int rad_pwdecode(char *pwd_in, char *pwd_out, int pwlen, char *digest)
{
	int	i, n;

	i = (pwlen > AUTH_PASS_LEN) ? AUTH_PASS_LEN : pwlen;
	memcpy(pwd_out, pwd_in, i);
	for (n = 0; n < i; n++)
		pwd_out[n] ^= digest[n];
	pwd_out[n] = 0;

	return strlen(pwd_out);
}


#if 0 /* Not used anymore */
/*
 *	Reply to the request with a CHALLENGE.  Also attach
 *	any user message provided and a state value.
 */
static void send_challenge(AUTH_REQ *authreq, char *msg,
				char *state, int activefd)
{
	AUTH_HDR		*auth;
	struct	sockaddr	saremote;
	struct	sockaddr_in	*sin;
	char			digest[AUTH_VECTOR_LEN];
	int			secretlen;
	int			total_length;
	u_char			*ptr;
	int			len;

	auth = (AUTH_HDR *)send_buffer;

	/*
	 *	Build standard response header
	 */
	auth->code = PW_ACCESS_CHALLENGE;
	auth->id = authreq->id;
	memcpy(auth->vector, authreq->vector, AUTH_VECTOR_LEN);
	total_length = AUTH_HDR_LEN;

	/*
	 *	Append the user message
	 */
	if (msg && msg[0]) {
		len = strlen(msg);
		if(len > 0 && len < AUTH_STRING_LEN-1) {
			ptr = auth->data;
			*ptr++ = PW_REPLY_MESSAGE;
			*ptr++ = len + 2;
			memcpy(ptr, msg, len);
			ptr += len;
			total_length += len + 2;
		}
	}

	/*
	 *	Append the state info
	 */
	if((state != (char *)NULL) && (strlen(state) > 0)) {
		len = strlen(state);
		*ptr++ = PW_STATE;
		*ptr++ = len + 2;
		memcpy(ptr, state, len);
		ptr += len;
		total_length += len + 2;
	}

	/*
	 *	Set total length in the header
	 */
	auth->length = htons(total_length);

	/*
	 *	Calculate the response digest
	 */
	secretlen = strlen(authreq->secret);
	memcpy(send_buffer + total_length, authreq->secret, secretlen);
	md5_calc(digest, (char *)auth, total_length + secretlen);
	memcpy(auth->vector, digest, AUTH_VECTOR_LEN);
	memset(send_buffer + total_length, 0, secretlen);

	sin = (struct sockaddr_in *) &saremote;
        memset ((char *) sin, '\0', sizeof (saremote));
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = htonl(authreq->ipaddr);
	sin->sin_port = htons(authreq->udp_port);

	DEBUG("Sending Challenge of id %d to %lx",
		authreq->id, (u_long)authreq->ipaddr);
	
	/*
	 *	Send it to the user
	 */
	sendto(activefd, (char *)auth, (int)total_length, (int)0,
			&saremote, sizeof(struct sockaddr_in));
}
#endif


