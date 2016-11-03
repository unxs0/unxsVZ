/*
 * sysdep.h	Include system dependant things, and define
 *		compatibility stuff.
 *
 *		Copyright 1997-2000	Cistron Internet Services B.V.
 *
 * Version:	svn ID removed
 */

#ifndef SYSDEP_H_INCLUDED
#define SYSDEP_H_INCLUDED

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(bsdi) || defined(M_UNIX)
#  ifndef NOSHADOW
#    define NOSHADOW
#  endif
#endif

#if defined(__alpha) && (defined(__osf__) || defined(__linux__))
typedef unsigned int	UINT4;
#else
typedef unsigned long	UINT4;
#endif

#ifdef BSD
#include        <strings.h>
#else
#include        <string.h>
#endif

#if defined(__FreeBSD__) || defined(bsdi)
# include        <stdlib.h>
#else
# include        <malloc.h>
#endif  /* FreeBSD */

#if defined(aix)
#include	<sys/select.h>
#include	<strings.h>
#define UT_NAMESIZE 8
#define UT_LINESIZE 12
#define UT_HOSTSIZE 16
#endif	/* aix 	*/

/* UTMP stuff. Uses utmpx on svr4 */
#ifdef __svr4__
#  include <utmpx.h>
#  include <sys/fcntl.h>
#  define utmp utmpx
#  define UT_NAMESIZE	32
#  define UT_LINESIZE	32
#  define UT_HOSTSIZE	257
#else
#  include <utmp.h>
#endif
#ifdef __osf__
#  define UT_NAMESIZE	32
#  define UT_LINESIZE	32
#  define UT_HOSTSIZE	64
#endif
#if defined(__hpux) || defined(hpux) || defined(__hpux__)
#  define UT_NAMESIZE   8
#  define UT_LINESIZE   12
#  define UT_HOSTSIZE   16
#  define setlinebuf(_s)        setvbuf(_s, (char *)0, _IOLBF, 0)
#  ifndef __hpux__
#    define __hpux__
#  endif
#endif
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(bsdi)
#  ifndef UTMP_FILE
#    define UTMP_FILE "/var/run/utmp"
#  endif
#  define ut_user ut_name
#endif

#endif /* SYSDEP_H_INCLUDED */
