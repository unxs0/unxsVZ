/*
 * radiusd.h	Definitions of structures, definitions, globals etc
 *		used throughout the server.
 *
 *		Copyright 1996-2001	Cistron Internet Services B.V.
 *		Copyright 2002		Cistron IP B.V.
 *
 * Version:	svn ID removed
 */

#include "sysdep.h"
#include "radius.h"
#include "conf.h"
#ifdef USEMYSQL
#include <mysql/mysql.h>
#endif

/* Server data structures */

typedef struct attr_flags {
	char			addport;	/* Add port to IP address */
	char			has_tag;	/* attribute allows tags */
	signed char		tag;
	char			encrypt;	/* encryption method */
	signed char		len_disp;	/* length displacement */
} ATTR_FLAGS;

typedef struct dict_attr {
	char			name[40];
	int			value;
	int			type;
	int			vendor;
	ATTR_FLAGS		flags;
	struct dict_attr	*next;
} DICT_ATTR;

typedef struct dict_value {
	char			attrname[40];
	char			name[40];
	int			value;
	struct dict_value	*next;
} DICT_VALUE;

typedef struct dict_vendor {
	char			vendorname[40];
	int			vendorpec;
	struct dict_vendor	*next;
} DICT_VENDOR;

typedef struct value_pair {
	char			name[40];
	int			attribute;
	int			type;
	int			length; /* of strvalue */
	ATTR_FLAGS		flags;
	UINT4			lvalue;
	int			operator;
	char			strvalue[AUTH_STRING_LEN];
	int			group;
	struct value_pair	*next;
} VALUE_PAIR;

typedef struct pair_list {
	char *name;
	VALUE_PAIR *check;
	VALUE_PAIR *reply;
	int lineno;
	int recno;
	struct pair_list *next;
} PAIR_LIST;

typedef struct auth_req {
	UINT4			ipaddr;
	u_short			udp_port;
	u_char			id;
	u_char			code;
	u_char			vector[16];
	u_char			secret[16];
	u_char			username[AUTH_STRING_LEN];
	VALUE_PAIR		*request;
	int			child_pid;	/* Process ID of child */
	UINT4			timestamp;
	u_char			*data;		/* Raw received data */
	int			data_len;
	VALUE_PAIR		*proxy_pairs;
	/* Proxy support fields */
	char			realm[64];
	int			validated;	/* Already md5 checked */
	UINT4			server_ipaddr;
	UINT4			server_id;
	VALUE_PAIR		*server_reply;	/* Reply from other server */
	int			server_code;	/* Reply code from other srv */
	struct auth_req		*next;		/* Next active request */
} AUTH_REQ;

typedef struct radclient {
	UINT4			ipaddr;
	char			longname[256];
	u_char			secret[16];
	char			shortname[32];
	struct radclient	*next;
} RADCLIENT;

typedef struct nas {
	UINT4			ipaddr;
	char			longname[256];
	char			shortname[32];
	char			nastype[32];
	struct nas		*next;
} NAS;

typedef struct realm_opts {
	unsigned int		nostrip:1;
	unsigned int		dohints:1;
	unsigned int		noauth:1;
	unsigned int		noacct:1;
	unsigned int		trusted:1;
} REALM_OPTS;

typedef struct realm {
	char			realm[64];
	char			server[64];
	UINT4			ipaddr;
	REALM_OPTS		opts;
	int			auth_port;
	int			acct_port;
	struct realm		*next;
} REALM;

enum {
  PW_OPERATOR_EQUAL = 0,	/* = */
  PW_OPERATOR_NOT_EQUAL,	/* != */
  PW_OPERATOR_LESS_THAN,	/* < */
  PW_OPERATOR_GREATER_THAN,	/* > */
  PW_OPERATOR_LESS_EQUAL,	/* <= */
  PW_OPERATOR_GREATER_EQUAL,	/* >= */
  PW_OPERATOR_SET,		/* := */
  PW_OPERATOR_ADD,		/* += */
  PW_OPERATOR_SUB,		/* -= */
};

#define DEBUG	if(debug_flag)log_debug
#define DEBUG2  if (debug_flag > 1)log_debug

#define SECONDS_PER_DAY		86400
#define MAX_REQUEST_TIME	30
#define CLEANUP_DELAY		3
#define MAX_REQUESTS		255

#define L_DBG			1
#define L_AUTH			2
#define L_INFO			3
#define L_ERR			4
#define L_PROXY			5
#define L_CONS			128

#define USERPARSE_EOS		0
#define USERPARSE_COMMA		1

#if defined(USE_DBM) || defined(USE_NDBM) || defined(USE_GDBM) || defined(USE_DB1) || defined(USE_DB2) || defined(USE_DB3)
#  define USE_DBX 1
#  if defined(USE_DB1)
#    define USE_DB
#    define DBIF_DB
#    define DBIF_DB1
#  elif defined(USE_DB2)
#    define USE_DB
#    define DBIF_DB
#    define DBIF_DB2
#  elif defined(USE_DB3)
#    define USE_DB
#    define DBIF_DB
#    define DBIF_DB3
#  elif defined(USE_NDBM) || defined(USE_GDBM)
#    define DBIF_NDBM
#  else
#    define DBIF_DBM
#  endif
#endif

#define VENDOR(x) (x >> 16)

 /*
  *	This defines for tagged string attrs whether the tag
  *	is actually inserted or not...! Stupid IMHO, but
  *	that's what the draft says...
  */
#define TAG_VALID(x)   ((x) > 0 && (x) < 0x20)
#define TAG_VALID_ZERO(x)   ((x) >= 0 && (x) < 0x20)
/* 
 *	This defines a TAG_ANY, the value for the tag if
 *	a wildcard ('*') was specified in a check item.
 */
#define TAG_ANY                -128     /* SCHAR_MIN */


/*
 *	Global variables.
 */
extern char		*progname;
extern int		debug_flag;
extern char		*radacct_dir;
extern char		*radius_dir;
extern char		*radutmp_path;
extern char		*radwtmp_path;
extern UINT4		expiration_seconds;
extern UINT4		warning_seconds;
extern int		radius_pid;
extern int		use_dbm;
extern int		use_dns;
extern int		use_wtmp;
extern int		log_stripped_names;
extern int		cache_passwd;
extern UINT4		myip;
extern UINT4		warning_seconds;
extern char		*log_auth_detail;
extern int		log_auth;
extern int		log_auth_badpass;
extern int		log_auth_goodpass;
extern int		auth_port;
extern int		acct_port;

/*
 *	Function prototypes.
 */

/* acct.c */
int		rad_add_detail(char *detail_file);
int		rad_accounting(AUTH_REQ *, int);
int		rad_accounting_detail(AUTH_REQ *, int, char *);
int		radzap(UINT4 nas, int port, char *user, time_t t, int dowtmp);
char		*uue(void *);
int		rad_check_multi(char *name, VALUE_PAIR *request, 
			VALUE_PAIR *reply, int maxsimul);

/* attrprint.c */
void		fprint_attr_list(FILE *, VALUE_PAIR *);
void		fprint_attr_val(FILE *, VALUE_PAIR *);
void		debug_pair(FILE *, VALUE_PAIR *);

/* dict.c */
int		dict_init(char *, char *);
int		dict_vendor(UINT4 vendorpec);
DICT_ATTR	*dict_attrget(int);
DICT_ATTR	*dict_attrfind(char *);
DICT_VALUE	*dict_valfind(char *, char *);
DICT_VALUE	*dict_valget(UINT4 value, char *);
int		dict_vendorcode(int);
int		dict_vendorpec(int);

/* md5.c */

void		md5_calc(u_char *, u_char *, u_int);

/* radiusd.c */
int		radius_exec_program(char *, VALUE_PAIR *, VALUE_PAIR **, int, char *reply_msg);
int		log_err (char *);
void		sig_cleanup(int);

/* pair.c */
VALUE_PAIR	*paircreate(int attr, int type);
void		pairfree(VALUE_PAIR *);
void		pairdelete(VALUE_PAIR **, int);
void		pairadd(VALUE_PAIR **, VALUE_PAIR *);
VALUE_PAIR	*pairfind(VALUE_PAIR *, int);
void		checkpair_move(VALUE_PAIR **to, VALUE_PAIR **from);
void		replypair_move(VALUE_PAIR **to, VALUE_PAIR **from, int group);
void		pairgroup_set(VALUE_PAIR *vp, int group);
void		pairmove2(VALUE_PAIR **to, VALUE_PAIR **from, int attr);
int		userparse(char *buffer, VALUE_PAIR **first_pair, int resolve);

/* readusers.c */
int		read_entry(char *buf, FILE *fp, char *fn, int *li, char *entry,
			VALUE_PAIR **check, VALUE_PAIR **reply, int resolve);
void		auth_type_fixup(VALUE_PAIR *check);
int		file_read(char *file, char *dir, PAIR_LIST **ret, int resolve);

/* util.c */
char *		ip_hostname (UINT4);
UINT4		get_ipaddr (char *);
int		good_ipaddr(char *);
void		ipaddr2str(char *, UINT4);
UINT4		ipstr2long(char *);
struct passwd	*rad_getpwnam(char *);
void		authfree(AUTH_REQ *authreq);
#if (defined (sun) && defined(__svr4__)) || defined(__hpux__) || defined(aix)
void		(*sun_signal(int signo, void (*func)(int)))(int);
#define signal sun_signal
#endif
char		*strNcpy(char *dest, char *src, int n);

/* radius.c */
void		random_vector(unsigned char *vector);
int		rad_build_packet(AUTH_HDR *, int, VALUE_PAIR *, char *, char *,
				 char *);
int		rad_send_reply(int, AUTH_REQ *, VALUE_PAIR *, char *, int);
AUTH_REQ	*radrecv (UINT4, u_short, u_char *, int);
int		calc_digest (u_char *, AUTH_REQ *);
int		calc_acctdigest(u_char *digest, AUTH_REQ *authreq);
int		rad_pwencode(char *pwd, char *pwd_out, char *secr, char *vec);
int		rad_pwdecode(char *in, char *out, int in_len, char *digest);

/* encrattr.c */
void 		encrypt_attr(char *secret, char *vector, VALUE_PAIR *vp);
int 		decrypt_attr(char *secret, char *vector, VALUE_PAIR *vp);

/* files.c */
int		user_find(char *name, VALUE_PAIR *,
				VALUE_PAIR **, VALUE_PAIR **);
void		presuf_setup(VALUE_PAIR *request_pairs);
int		hints_setup(VALUE_PAIR *request_pairs);
int		huntgroup_access(VALUE_PAIR *request_pairs);
RADCLIENT	*client_find(UINT4 ipno);
char		*client_name(UINT4 ipno);
int		read_clients_file(char *);
REALM		*realm_find(char *, int);
NAS		*nas_find(UINT4 ipno);
char		*nas_name(UINT4 ipno);
char		*nas_name2(AUTH_REQ *r);
char		*auth_name(AUTH_REQ *authreq, int do_cid);
int		read_naslist_file(char *);
int		read_config_files(void);
int		presufcmp(VALUE_PAIR *check, char *name, char *rest, int rl);
#ifdef USE_DBX
int		rad_dbm_open(void);
#endif

/* version.c */
void		version();

/* log.c */
int		radlog(int, char *, ...);
int		log_debug(char *, ...);

/* pam.c */
#ifdef PAM
int		pam_pass(char *name, char *passwd, const char *pamauth,
			char *reply_msg);
#define PAM_DEFAULT_TYPE    "radius"
#endif

/* proxy.c */
int proxy_send(AUTH_REQ *authreq, int activefd);
int proxy_receive(AUTH_REQ *authreq, int activefd);

/* auth.c */
int		rad_auth_init(AUTH_REQ *authreq, int activefd);
int		rad_authenticate (AUTH_REQ *, int);

/* exec.c */
char		*radius_xlate(char *, VALUE_PAIR *req, VALUE_PAIR *reply);

/* timestr.c */
int		timestr_match(char *, time_t);

/* mysql.c */
#ifdef USEMYSQL
#define MAX_AUTH_QUERY_LEN              256

typedef struct mysql_rec
{
	char 		cSessionID[33];
	char 		cLogin[101];
	unsigned 	uStatus;
	unsigned 	uPort;
	unsigned 	uProtocol;
	unsigned long 	uConnectTime;
	char 		cNAS[20];
	char 		cIP[20];
	unsigned	uService;
	unsigned long 	uInBytes;
	unsigned long 	uOutBytes;
	unsigned 	uTermCause;
	char            cLine[20];
	char            cCallerID[20];
	unsigned	uPortType;
	unsigned long	uDelay;
	char            cInfo[20];
} MYSQL_RECORD;


void		save_mysql_record(MYSQL_RECORD *);
void		build_mysql_record(VALUE_PAIR *, MYSQL_RECORD *);
int		mysql_rad_init();
struct passwd   *mysql_getpwnam(char *);
int             mysql_pass(char *, char *);
#   ifdef SQLPWTYPE
struct passwd	*mysql_getuserrec(char *);
#   endif
#endif

