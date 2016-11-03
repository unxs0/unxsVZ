/*
 * encrattr.c	Handles encryption and decryption of string attributes based
 *		on the encryption style flag in VALUE_PAIRs.
 *
 *		This could perhaps all be a little more generic and even more 
 *		geared towards arbitrary encryption types, but as far as I know 
 *		those aren't used yet anyway.
 *
 *		Perhaps we can move password hiding/recoding here as well?
 *
 * 2000/11/22 - EvB - Created for the tunneling support patch v5 against 1.6.4
 * 2001/05/14 - EvB - Updated a little when porting to 1.6.pre5,
 * 		      mainly comments and formatting.
 */
char encrattr_rcsid[] =
"svn ID removed

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "radiusd.h"


static void encrypt_attr_style_1(char *secret, char *vector, VALUE_PAIR *vp);
static int decrypt_attr_style_1(char *secret, char *vector, VALUE_PAIR *vp);


/*
 * void encrypt_attr(char *secret, char *vector, VALUE_PAIR *vp);
 *
 * Encrypts vp->strvalue using style vp->flags.encrypt, possibly using 
 * a request authenticator passed in vector and the shared secret.
 *
 * This should always succeed.
 */

void encrypt_attr(char *secret, char *vector, VALUE_PAIR *vp)
{
	switch(vp->flags.encrypt)
	{
		case 0:
			/* Normal, cleartext. */
			break;

		case 1:
			/* Tunnel Password (see RFC 2868, section 3.5). */
			encrypt_attr_style_1(secret, vector, vp);
			break;

		default:
			/* Unknown style - don't send the cleartext! */
			vp->length = 19;
			memcpy(vp->strvalue, "UNKNOWN_ENCR_METHOD", vp->length); 
			radlog(L_ERR, "decryption style %d is not implemented - "
				   "check dictionary flags for attribute %d", 
				    vp->flags.encrypt, vp->attribute);
	}
}


/*
 * int decrypt_attr(char *secret, char *vector, VALUE_PAIR *vp);
 *
 * Decrypts vp->strvalue using style vp->flags.encrypt, possibly using 
 * a request authenticator passed in vector and the shared secret.
 *
 * Returns 0 if success.
 */


int decrypt_attr(char *secret, char *vector, VALUE_PAIR *vp)
{
	int ret;

	ret = -1;

	switch(vp->flags.encrypt) 
	{
	case 0:
		/* Normal, cleartext */
		ret = 0;
		break;

	case 1:
		/* Style: Tunnel-Password (see RFC 2868, section 3.5). */
		ret = decrypt_attr_style_1(secret, vector, vp);
		break;

	default:
		/* Unknown style - don't use the cyphertext */
		vp->length = 19;
		memcpy(vp->strvalue, "UNKNOWN_ENCR_METHOD", vp->length); 
		radlog(L_ERR, "decryption style %d is not implemented - "
			   "check dictionary flags for attribute %d", 
		    vp->flags.encrypt, vp->attribute);

	}

	if (ret) {
		vp->length = 14;
		memcpy(vp->strvalue, "FAILED_DECRYPT", vp->length);
		radlog(L_ERR, "decryption style %d failed for attribute %d",
		      vp->flags.encrypt, vp->attribute);
	}

	return ret;
}


/*
 * Encrypt/decrypt string attributes, style 1.
 *
 * See RFC 2868, section 3.5 for details. Currently probably indeed 
 * only useful for Tunnel-Password, but why make it a special case, 
 * now we have a generic flags mechanism in place anyway...
 *
 * It's optimized a little for speed, but it could probably be better.
 */

#define	CLEAR_STRING_LEN	256 	/* The RFC says it is */
#define	SECRET_LEN		32	/* Max. in client.c */
#define	MD5_LEN			16	/* The algorithm specifies it */
#define	SALT_LEN		2	/* The RFC says it is */


void encrypt_attr_style_1(char *secret, char *vector, VALUE_PAIR *vp)
{
	char clear_buf[CLEAR_STRING_LEN];
	char work_buf[SECRET_LEN + AUTH_VECTOR_LEN + SALT_LEN];
	char digest[MD5_LEN];
	char *i,*o;
	unsigned short salt;		/* salt in network order */
	int clear_len;
	int work_len;
	int secret_len;
	int n;

	/* Create the string we'll actually be processing by copying up to 255
	   bytes of original cleartext, padding it with zeroes to a multiple of
	   16 bytes and inserting a length octet in front. */

	/* Limit length */
	clear_len = vp->length;
	if (clear_len > CLEAR_STRING_LEN - 1) clear_len = CLEAR_STRING_LEN - 1;

	/* Write the 'original' length byte and copy the buffer */
	*clear_buf = clear_len;
	memcpy(clear_buf + 1, vp->strvalue, clear_len);

	/* From now on, the length byte is included with the byte count */
	clear_len++;

	/* Pad the string to a multiple of 1 chunk */
	if (clear_len % MD5_LEN) {
		memset(clear_buf+clear_len, 0, MD5_LEN - (clear_len % MD5_LEN));
	}

	/* Define input and number of chunks to process */
	i = clear_buf;
	clear_len = (clear_len + (MD5_LEN - 1)) / MD5_LEN;	

	/* Define output and starting length */
	o = vp->strvalue;
	vp->length = sizeof(salt);

	/*
	 * Fill in salt. Must be unique per attribute that uses it in the same 
	 * packet, and the most significant bit must be set - see RFC 2868.
	 *
	 * FIXME: this _may_ be too simple. For now we just take the vp 
	 * pointer, which should be different between attributes, xor-ed with 
	 * the first longword of the vector to make it a little more unique.
	 *
	 * Oh, and sizeof(long) always == sizeof(void*) in our part of the
	 * universe, right? (*BSD, Solaris, Linux, DEC Unix...)
	 */
	salt = htons( ( ((long)vp ^ *(long *)vector) & 0xffff ) | 0x8000 );
	memcpy(o, &salt, sizeof(salt));
	o += sizeof(salt);

	/* Create a first working buffer to calc the MD5 hash over */
	secret_len = strlen(secret);	/* already limited by read_clients */
#if 0
	printf("secret = %s, vector = 0x%08Lx %08Lx, salt = 0x%04x\n",
		secret, ((long long *)vector)[0], ((long long *)vector)[1],
		ntohs(salt));
#endif
	memcpy(work_buf, secret, secret_len);
	memcpy(work_buf + secret_len, vector, AUTH_VECTOR_LEN);
	memcpy(work_buf + secret_len + AUTH_VECTOR_LEN, &salt, sizeof(salt));
	work_len = secret_len + AUTH_VECTOR_LEN + sizeof(salt);

	for( ; clear_len; clear_len--) {

		/* Get the digest */
		md5_calc((u_char *)digest,(u_char *) work_buf, work_len);

		/* Xor the clear text to get the output chunk and next buffer */
		for(n = 0; n < MD5_LEN; n++) {
			*(work_buf + secret_len + n) = *o++ = *i++ ^ digest[n];
		}

		/* This is the size of the next working buffer */
		work_len = secret_len + MD5_LEN;

		/* Increment the output length */
		vp->length += MD5_LEN;
	}
}


int decrypt_attr_style_1(char *secret, char *vector, VALUE_PAIR *vp)
{
	char work_buf[SECRET_LEN + AUTH_VECTOR_LEN + SALT_LEN];
	char digest[MD5_LEN];
	char *i,*o;
	u_short salt;		/* salt in network order */
	int chunks;
	int secret_len;
	int n;

	/* Check the length; the salt and at least one chunk must be present,
	   and partial chunks are not allowed. */
	if (((vp->length - SALT_LEN) < MD5_LEN) || 
	    ((vp->length - SALT_LEN) % MD5_LEN)) {
		DEBUG("decrypt_attr_style_1: bogus cyphertext length %d", 
		       vp->length);
		return -1;
	}

	/* Define input */
	i = vp->strvalue;
	chunks = ((vp->length - SALT_LEN) + (MD5_LEN - 1)) / MD5_LEN;

	/* Define output - it's done in place, overwriting the salt! */
	o = vp->strvalue;

	/* Get the salt from the input and check it */
	memcpy(&salt, i, sizeof(salt)); i += sizeof(salt);
	if (!(ntohs(salt) & 0x8000)) {
		DEBUG("decrypt_attr_style_1: invalid salt 0x%04x", 
		       ntohs(salt));
		return -1;
	}

	/* Create a first working buffer to calculate the MD5 hash over */
	secret_len = strlen(secret);	/* already limited by read_clients */
	memcpy(work_buf, secret, secret_len);
	memcpy(work_buf + secret_len, vector, AUTH_VECTOR_LEN);
	memcpy(work_buf + secret_len + AUTH_VECTOR_LEN, &salt, sizeof(salt));
#if 0
	printf("secret = %s, vector = 0x%08Lx %08Lx, salt = 0x%04x\n",
		secret, ((long long *)vector)[0], ((long long *)vector)[1],
		ntohs(salt));
#endif

	/* Calculate the digest */
	md5_calc((u_char *)digest,(u_char *)work_buf, secret_len + AUTH_VECTOR_LEN + sizeof(salt));

	/* Process the first byte: the cleartext length */
	*(work_buf + secret_len + 0) = *i;
	vp->length = *i++ ^ digest[0];

	if ((vp->length > CLEAR_STRING_LEN) ||
	    (vp->length > MD5_LEN * chunks) ||
	    (vp->length < MD5_LEN * (chunks - 1))) {
		DEBUG("decrypt_attr_style_1: bogus decrypted length %d", 
		       vp->length);
		return -1;
	}

	/* Process the rest of the first chunk */
	for(n = 1; n < MD5_LEN; n++) {
		*(work_buf + secret_len + n) = *i;
		*o++ = *i++ ^ digest[n];
	}

	/* One full chunk is already done. */
	chunks--;

	/* Loop over the rest of the chunks */
	for( ; chunks >= 0; chunks--) {

		/* Get the digest */
		md5_calc((u_char *)digest,(u_char *) work_buf, secret_len + MD5_LEN);

		/* Fill the work buffer with the next cyphertext chunk and
		   xor the it with the digest to get the output segment */
		for(n = 0; n < MD5_LEN; n++) {
			*(work_buf + secret_len + n) = *i;
			*o++ = *i++ ^ digest[n];
		}
	}

	/* Add a zero byte. Only convenient for logging and other functions
	   that don't pay attention to vp->length instead, as they should. 
	   This can't go wrong as vp->length was limited earlier anyway. */
	vp->strvalue[vp->length] = 0;

	/* If debug flag > 1 then show decrypted string */
	DEBUG2("  style 1 decrypted attribute %d:%d = \"%s\" (%d)",
	       vp->attribute, vp->flags.tag, vp->strvalue, vp->length);

	return 0;
}
