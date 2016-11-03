/*
 * conf.h	Mostly definitions of filenames and locations.
 *
 * Version:	svn ID removed
 *
 */

/* Default Database File Names */
#define RADIUS_DIR		"/usr/local/etc/raddb"
#define RADLOG_DIR		"/var/log"

#ifdef aix
/*
 *	AIX puts its log files somewhere else.
 *	submitted by Ewan Leith <ejl@man.fraser-williams.com>
 */
#undef RADLOG_DIR
#define RADLOG_DIR		"/var/adm/logs"
#endif

#define RADACCT_DIR		(RADLOG_DIR "/radacct")


#define RADIUS_DICTIONARY	"dictionary"
#define RADIUS_CLIENTS		"clients"
#define RADIUS_NASLIST		"naslist"
#define RADIUS_USERS		"users"
#define RADIUS_HOLD		"holdusers"
#define RADIUS_LOG		"radius.log"
#define RADIUS_HINTS		"hints"
#define RADIUS_HUNTGROUPS	"huntgroups"
#define RADIUS_REALMS		"realms"

#define RADUTMP			(RADLOG_DIR "/radutmp")
#define RADWTMP			(RADLOG_DIR "/radwtmp")

#define RADIUS_PID		"/var/run/radiusd.pid"
#define RADRELAY_PID		"/var/run/radrelay.pid"

#ifdef aix
/*
 *  More AIX changes.  Apparently it doesn't have a '/var/run' directory.
 */
#undef RADIUS_PID
#define RADIUS_PID		(RADLOG_DIR "/radacct/radiusd.pid")
#endif

#define CHECKRAD1		"/usr/sbin/checkrad"
#define CHECKRAD2		"/usr/local/sbin/checkrad"

/* Hack for funky ascend ports on MAX 4048 (and probably others)
   The "NAS-Port" value is "xyyzz" where "x" = 1 for digital, 2 for analog;
   "yy" = line number (1 for first PRI/T1/E1, 2 for second, so on);
   "zz" = channel number (on the PRI or Channelized T1/E1).
    This should work with normal terminal servers, unless you have a TS with
        more than 9999 ports ;^).
    The "ASCEND_CHANNELS_PER_LINE" is the number of channels for each line into
        the unit.  For my US/PRI that's 23.  A US/T1 would be 24, and a
        European E1 would be 30 (I think ... never had one ;^).
    This will NOT change the "NAS-Port" reported in the detail log.  This
        is simply to fix the dynamic IP assignments a la Cistron.
    WARNING: This hack works for me, but I only have one PRI!!!  I've not
        tested it on 2 or more (or with models other than the Max 4048)
    Use at your own risk!
  -- dgreer@austintx.com
*/
#define ASCEND_PORT_HACK
#define ASCEND_CHANNELS_PER_LINE        23

/*
 *	Same for Cisco's: Cisco adds 20000 to the portnumber
 *	for ISDN connections. You cannot use ASCEND_PORT_HACK and
 *	CISCO_PORT_HACK at the same time!
 */
#undef CISCO_PORT_HACK

/*
 *	Hack for USR gear - uses a different Vendor-Specific attribute
 *	packet layout, argh.
 */
#define ATTRIB_NMC

/*
 *	Hack to enable use of 1.5.4.3 dictionaries.
 */
#define COMPAT_1543

/*
 *	Syslog support - standard under Linux.
 */
#if !defined(USE_SYSLOG) && defined(__linux__)
#  define USE_SYSLOG
#endif

/*
 *	Choose between two formats for tagged attributes - this is reflected
 *	when parsing the user file and when outputting accounting data.
 *
 *	Default format is attr:tag = value, as the draft says that
 *	tags can be used to associate multiple different attributes or
 *	distinguish between multiple instances of the same attribute.
 *
 *	Merit associates a tag with the value, which reflects the hackish
 *	way the tag is put in the packet. Their format is attr = :tag:value
 *	and is used if the next define is uncommented.
 */
/* #define MERIT_TAG_FORMAT */

