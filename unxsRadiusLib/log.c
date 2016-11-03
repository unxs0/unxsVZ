/*
 * log.c	Logging module.
 *
 *		Copyright 1997-2001 Cistron Internet Services B.V.
 */

char log_rcsid[] =
"svn ID removed

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<stdarg.h>
#include	<time.h>

#include	"radiusd.h"

#ifdef USE_SYSLOG
#include	<syslog.h>
#endif

char		*radlog_dir;
int		debug_flag;
extern char	*progname;

#ifdef USE_SYSLOG
int		syslog_facility = -1;
static int	facilities[] = {
	LOG_LOCAL0,
	LOG_LOCAL1,
	LOG_LOCAL2,
	LOG_LOCAL3,
	LOG_LOCAL4,
	LOG_LOCAL5,
	LOG_LOCAL6,
	LOG_LOCAL7
};
#endif
int		syslog_open = 0;

/*
 *	Log the message to the logfile. Include the severity and
 *	a time stamp.
 */
static int do_log(int lvl, char *fmt, va_list ap)
{
	FILE	*msgfd;
	unsigned char	*s = (u_char *)": ";
	char	buffer[8192];
	time_t	timeval;
	int	len;
#ifdef USE_SYSLOG
	int	level = LOG_INFO;
#endif

	if ((lvl & L_CONS) || radlog_dir == NULL || debug_flag) {
		lvl &= ~L_CONS;
		if (!debug_flag && radlog_dir)
			fprintf(stderr, "%s: ", progname);
		vfprintf(stderr, fmt, ap);
		fprintf(stderr, "\n");
	}
	if (radlog_dir == NULL || debug_flag) return 0;

	if (strcmp(radlog_dir, "stdout") == 0) {
		msgfd = stdout;
	} else if (strcmp(radlog_dir, "stderr") == 0) {
		msgfd = stderr;
#ifdef USE_SYSLOG
	} else if (strcmp(radlog_dir, "syslog") == 0) {
		msgfd = NULL;
#endif
	} else if (strcmp(radlog_dir, "none") == 0) {
		msgfd = NULL;
	} else {
		sprintf(buffer, "%.200s/%.50s", radlog_dir, RADIUS_LOG);
		if((msgfd = fopen(buffer, "a")) == NULL) {
			fprintf(stderr, "%s: Couldn't open %s for logging\n",
					progname, buffer);
			return -1;
		}
	}

	timeval = time(0);
	strcpy(buffer, ctime(&timeval));
	switch(lvl) {
		case L_DBG:
#ifdef USE_SYSLOG
			level = LOG_DEBUG;
#endif
			s = (u_char *)": Debug: ";
			break;
		case L_AUTH:
			s = (u_char *)": Auth: ";
			break;
		case L_PROXY:
			s = (u_char *)": Proxy: ";
			break;
		case L_INFO:
			s = (u_char *)": Info: ";
			break;
		case L_ERR:
#ifdef USE_SYSLOG
			level = LOG_WARNING;
#endif
			s = (u_char *)": Error: ";
			break;
	}
	strNcpy(buffer + 24,(char *)s, sizeof(buffer) - 24);
	len = strlen(buffer);

	vsprintf(buffer + len, fmt, ap);
	if (strlen(buffer) >= sizeof(buffer))
		/* What else can we do if we don't have vnsprintf */
		_exit(1);

	/*
	 *	Filter out characters not in Latin-1.
	 */
	for (s = (unsigned char *)buffer; *s; s++) {
		if (*s == '\r' || *s == '\n')
			*s = ' ';
		else if (*s < 32 || (*s >= 128 && *s <= 160))
			*s = '?';
	}

#ifdef USE_SYSLOG
	if (syslog_facility >= 0) {
		if (!syslog_open) {
			openlog("radiusd", 0, facilities[syslog_facility]);
			syslog_open = 1;
		}
		syslog(facilities[syslog_facility]|level, "%s", buffer + 26);
	}
#endif

	strcat(buffer, "\n");

	if (msgfd) fputs(buffer, msgfd);
	if (msgfd == stdout || msgfd == stderr)
		fflush(stdout);
	else if (msgfd != NULL)
		fclose(msgfd);

	return 0;
}

int log_debug(char *msg, ...)
{
	va_list ap;
	int r;

	va_start(ap, msg);
	r = do_log(L_DBG, msg, ap);
	va_end(ap);

	return r;
}

int radlog(int lvl, char *msg, ...)
{
	va_list ap;
	int r;

	va_start(ap, msg);
	r = do_log(lvl, msg, ap);
	va_end(ap);

	return r;
}

