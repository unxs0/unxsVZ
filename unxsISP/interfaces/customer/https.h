/*
FILE
	svn ID removed
PURPOSE
	https.c header file
AUTHOR
	(C) Gary Wallis 2004.

*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define OPENSSL_NO_KRB5

#include <openssl/ssl.h>

#define CA_LIST "root.pem"
#define HOST	"secure.netbilling.com"
#define RANDOM  "random.pem"
#define PORT	1402
#define BUFSIZE 512

extern BIO *bio_err;
void berr_exit (char *string);
void err_exit(char *string);

SSL_CTX *initialize_ctx(char *keyfile, char *password);
void destroy_ctx(SSL_CTX *ctx);

#ifndef ALLOW_OLD_VERSIONS
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
#error "Must use OpenSSL 0.9.6 or later"
#endif
#endif


#define KEYFILE "client.pem"
#define PASSWORD "password"

int tcp_connect(const char *host,unsigned port);
void check_cert(SSL *ssl,char *host);

//This app interfacing
void htmlFatalError(const char *cErrMsg);

