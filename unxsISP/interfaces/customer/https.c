/*
FILE
	svn ID removed
PURPOSE
	Simple SSL C client function. Connects to remote SSL HTTPS server
	and returns response in a small buffer.
AUTHOR
	(C) Gary Wallis 2004
	Based on OpenSSL library calls 0.9.7c
	and examples by


*/


#include "https.h"
#include <openssl/err.h>
#include "interface.h"

BIO *bio_err=0;

/*
static char *pass;
static int password_cb(char *buf,int num,int rwflag,void *userdata);
*/

//static char *ciphers=0;

static int http_request(SSL *ssl,char const *cHost,unsigned uPort,
				char const *cURL,char const *cPost,char *cAnswer);
unsigned https(char const *cHost,unsigned uPort,char const *cURL,
                        		char const *cPost,char *cResult);
void error_exit(void);

void error_exit(void)
{
	//Other apps would use someother return or exit method from
	//this socket kit.
	//exit(0);

	//for example
	htmlPlainTextError("https() connection error");
}

/*
int main(void)
{
	char cAnswer[BUFSIZE+2];

	if(https(REQUEST_TEMPLATE,cAnswer))
		printf("ssl socket/response failure\n");
	else
		printf("%s\n",cAnswer);
	
	return(0);

}//int main(void)
*/


void err_exit(char *string)
{
	fprintf(stderr,"%s\n",string);
	error_exit();
}


void berr_exit(char *string)
{
	BIO_printf(bio_err,"%s\n",string);
	ERR_print_errors(bio_err);
	error_exit();
}


/*
static int password_cb(char *buf,int num, int rwflag,void *userdata)
{
	if(num<strlen(pass)+1)
		return(0);

	strcpy(buf,pass);
	return(strlen(pass));
}
*/


SSL_CTX *initialize_ctx(char *keyfile,char *password)
{
	SSL_METHOD *meth;
	SSL_CTX *ctx;
    
	if(!bio_err)
	{
		SSL_library_init();
		SSL_load_error_strings();
      
		bio_err=BIO_new_fp(stderr,BIO_NOCLOSE);
	}

	//This context 
	meth=SSLv23_method();
	ctx=SSL_CTX_new(meth);

/*
	if(!(SSL_CTX_use_certificate_chain_file(ctx,keyfile)))
		berr_exit("Can't read certificate file");

	pass=password;
	SSL_CTX_set_default_passwd_cb(ctx,password_cb);

	if(!(SSL_CTX_use_PrivateKey_file(ctx,keyfile,SSL_FILETYPE_PEM)))
		berr_exit("Can't read key file");

	if(!(SSL_CTX_load_verify_locations(ctx,CA_LIST,0)))
		berr_exit("Can't read CA list");
*/

#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
		SSL_CTX_set_verify_depth(ctx,1);
#endif
    
	return ctx;

}//SSL_CTX *initialize_ctx(char *keyfile,char *password)

     
void destroy_ctx(SSL_CTX *ctx)
{
	SSL_CTX_free(ctx);
}


static int http_request(SSL *ssl,char const *cHost,unsigned uPort,
				char const *cURL,char const *cPost,char *cAnswer)
{
	char *request=0;
	char buf[BUFSIZE];
	int r;
	int len, request_len;

static char *REQUEST_TEMPLATE=
	"POST %s HTTP/1.0\r\n"
	"User-Agent:mysqlShop\r\n"
	"Host: %s:%d\r\n"
	"Content-Type: application/x-www-form-urlencoded\r\n"
	"Content-Length: %d\r\n"
	"\r\n"
	"%s\r\n";

    
	//Now construct request define the 6 constant please TODO port number max length?
	request_len=strlen(REQUEST_TEMPLATE)+strlen(cHost)+6+strlen(cURL)+strlen(cPost);
	if(!(request=(char *)malloc(request_len)))
		err_exit("Couldn't allocate request");
	snprintf(request,request_len,REQUEST_TEMPLATE,cURL,cHost,uPort,strlen(cPost),cPost);

	request_len=strlen(request);

	r=SSL_write(ssl,request,request_len);

	switch(SSL_get_error(ssl,r))
	{      
		case SSL_ERROR_NONE:
			if(request_len!=r)
			err_exit("Incomplete write!");
		break;
		default:
			berr_exit("SSL write problem");
	}


	while(1)
	{
		r=SSL_read(ssl,buf,BUFSIZE);

		switch(SSL_get_error(ssl,r))
		{
			case SSL_ERROR_NONE:
				len=r;
			break;

			case SSL_ERROR_WANT_READ:
				continue;

			case SSL_ERROR_ZERO_RETURN:
				goto shutdown;

			case SSL_ERROR_SYSCALL:
				fprintf(stderr,"SSL Error: Premature close\n");
				goto done;
			default:
				berr_exit("SSL read problem");
		}
		if(buf[0])
		{
			if(len>BUFSIZE) len=BUFSIZE;
			strncat(cAnswer,buf,len);
			//cAnswer[len]=0;
		}
	}
    
shutdown:
	r=SSL_shutdown(ssl);
	switch(r)
	{
		case 1:
		break;

		case 0:
		case -1:

		default:
			berr_exit("Shutdown failed");
	}
    
done:
	SSL_free(ssl);
	free(request);
	return(0);

}//static int http_request()

    
unsigned https(char const *cHost,unsigned uPort,char const *cURL,
					char const *cPost,char *cAnswer)
{
	SSL_CTX *ctx;
	SSL *ssl;
	BIO *sbio;
	int sock;
    
	ctx=initialize_ctx(KEYFILE,PASSWORD);
    
	//if(ciphers)
	//	SSL_CTX_set_cipher_list(ctx,ciphers);

	sock=tcp_connect(cHost,uPort);

	ssl=SSL_new(ctx);
	sbio=BIO_new_socket(sock,BIO_NOCLOSE);
	SSL_set_bio(ssl,sbio,sbio);

	if(SSL_connect(ssl)<=0)
		berr_exit("SSL connect error");

	http_request(ssl,cHost,uPort,cURL,cPost,cAnswer);

	destroy_ctx(ctx);
	close(sock);

	return(0);

}//unsigned https(char *cRequest,char *cAnswer)


int tcp_connect(const char *host,unsigned port)
{
	struct hostent *hp;
	struct sockaddr_in addr;
	int sock;
	char cErrorMsg[256];
    
	if(!(hp=gethostbyname(host)))
		berr_exit("Couldn't resolve host");

	memset(&addr,0,sizeof(addr));
	addr.sin_addr=*(struct in_addr*)
	hp->h_addr_list[0];
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);

	if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		err_exit("Couldn't create socket");

	if(connect(sock,(struct sockaddr *)&addr,sizeof(addr))<0)
	{
		sprintf(cErrorMsg,"Couldn't connect socket: %u %s",port,host);
		err_exit(cErrorMsg);
	}
    
	return sock;

}//int tcp_connect(char *host,int port)

