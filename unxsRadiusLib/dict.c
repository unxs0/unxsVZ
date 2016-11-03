/*
 * dict.c	Handle the radius dictionary file.
 *
 *		Copyright 1998-2001	Cistron Internet Services B.V.
 *		Copyright 2000		Emile van Bergen (tagged attributes)
 *		Copyright 2002		Cistron IP B.V.
 */
char dict_rcsid[] =
"svn ID removed

#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<pwd.h>
#include	<ctype.h>

#include	"radiusd.h"

static DICT_ATTR	*dictionary_attributes;
static DICT_VALUE	*dictionary_values;
static DICT_VENDOR	*dictionary_vendors;

#ifdef NOCASE
#define DICT_STRCMP strcasecmp
#else
#define DICT_STRCMP strcmp
#endif

/*
 *	Free the dictionary_attributes and dictionary_values lists.
 */
static void dict_free(void)
{
	DICT_ATTR	*dattr, *anext;
	DICT_VALUE	*dval, *vnext;
	DICT_VENDOR	*dvend, *enext;

	for (dattr = dictionary_attributes; dattr; dattr = anext) {
		anext = dattr->next;
		free(dattr);
	}
	for (dval = dictionary_values; dval; dval = vnext) {
		vnext = dval->next;
		free(dval);
	}
	for (dvend = dictionary_vendors; dvend; dvend = enext) {
		enext = dvend->next;
		free(dvend);
	}
	dictionary_attributes = NULL;
	dictionary_values = NULL;
	dictionary_vendors = NULL;
}

/*
 *	Add vendor to the list.
 */
static int addvendor(char *name, int value)
{
	DICT_VENDOR *vval;

	if ((vval =(DICT_VENDOR *)malloc(sizeof(DICT_VENDOR))) ==
	    (DICT_VENDOR *)NULL) {
		radlog(L_ERR|L_CONS, "dict_init: out of memory");
		return(-1);
	}
	strNcpy(vval->vendorname, name, sizeof(vval->vendorname));
	vval->vendorpec  = value;

	/* Insert at front. */
	vval->next = dictionary_vendors;
	dictionary_vendors = vval;

	return 0;
}

/*
 *	See if we know a vendor.
 */
int dict_vendor(UINT4 vendorpec)
{
	DICT_VENDOR *v;

	for (v = dictionary_vendors; v; v = v->next)
		if (v->vendorpec == vendorpec)
			break;
	return v ? 1 : 0;
}

/*
 *	Parse an ATTRIBUTE line.
 */
static DICT_ATTR *parse_attr(char *buffer, int line_no, char *dict_file,
				int is_nmc, int *vendor_usr_seen)
{
	ATTR_FLAGS	flags;		/* parsed options */
	DICT_ATTR	*attr;
	DICT_VENDOR	*v;
	char		dummystr[64];
	char		namestr[64];
	char		valstr[64];
	char		typestr[64];
	char		optstr[64];	/* Options, incl. vendors */
	char		*s, *c;
	int		value;
	int		type;
	int		vendor;

	/* Read the ATTRIBUTE line */
	vendor = 0;
	optstr[0] = 0;
	if(sscanf(buffer, "%63s%63s%63s%63s%63s", dummystr,
		namestr, valstr, typestr, optstr) < 4) {
		radlog(L_ERR,
	"%s: Invalid attribute on line %d of %s\n",
			progname, line_no, dict_file);
		return NULL;
	}

#ifdef ATTRIB_NMC
	/*
	 *	Convert ATTRIB_NMC into our format.
	 *	We might need to add USR to the list of
	 *	vendors first.
	 */
	if (is_nmc && optstr[0] == 0) {
		if (!*vendor_usr_seen) {
			if (addvendor("USR", VENDORPEC_USR) < 0)
				return NULL;
			*vendor_usr_seen = 1;
		}
		strcpy(optstr, "USR");
	}
#endif

	/*
	 *	Validate all entries
	 */
	if (strlen(namestr) > 39) {
		radlog(L_ERR|L_CONS,
		"dict_init: Invalid name length on line %d of %s",
				line_no, dict_file);
			return NULL;
	}

	if (!isdigit(*valstr)) {
		radlog(L_ERR|L_CONS,
	"dict_init: Invalid value on line %d of %s",
			line_no, dict_file);
		return NULL;
	}
	if (valstr[0] != '0')
		value = atoi(valstr);
	else
		sscanf(valstr, "%i", &value);

	if (strcmp(typestr, "string") == 0) {
		type = PW_TYPE_STRING;
	}
	else if(strcmp(typestr, "integer") == 0) {
		type = PW_TYPE_INTEGER;
	}
	else if(strcmp(typestr, "ipaddr") == 0) {
		type = PW_TYPE_IPADDR;
	}
	else if(strcmp(typestr, "date") == 0) {
		type = PW_TYPE_DATE;
	}
	else {
		radlog(L_ERR|L_CONS,
	"dict_init: Invalid type on line %d of %s",
			line_no, dict_file);
		return NULL;
	}

	/*
	 * Now parse the options string. It is formatted
	 * as a comma-separated string of flags and
	 * optionally values. A vendor is used as a flag
	 * without a value to prevent breaking existing
	 * dictionaries, though it would be nicer to use
	 * 'vendor=xyz' instead, which is also supported.
	 *
	 * The fact that there will never be any spaces
	 * in the options string (sscanf), makes life
	 * a little easier here...
	 */
        memset(&flags, 0, sizeof(flags));
	s = strtok(optstr, ",");
	while(s) {
		if (strcmp(s, "has_tag") == 0 ||
		    strcmp(s, "has_tag=1") == 0) {
			/* Boolean flag, means this is a
			   tagged attribute */
			flags.has_tag = 1;
		}
		else if (strncmp(s, "len+=", 5) == 0 ||
			 strncmp(s, "len-=", 5) == 0) {
			/* Length difference, to accomodate
			   braindead NASes & their vendors */
			flags.len_disp = strtol(s + 5, &c, 0);
			if (*c) {
dict_opterr:
				radlog(L_CONS|L_ERR, "dict_init: "
				"invalid option %s on line %d of %s",
						s, line_no, dict_file);
				return NULL;
			}
			if (s[3] == '-') {
				flags.len_disp = -flags.len_disp;
			}
		}
		else if (strncmp(s, "encrypt=", 8) == 0) {
			/* Encryption method, defaults to 0 (none).
			   Currently valid is just type 1,
			   Tunnel-Password style, which can only
			   be applied to strings. */
			flags.encrypt = strtol(s + 8, &c, 0);
			if (*c) {
				/* shortcut if non-numeric */
				goto dict_opterr;
			}
		}
		else {
			/* Must be a vendor 'flag'... */
			if (strncmp(s, "vendor=", 5) == 0) {
				/* New format */
				s += 5;
			}

			for (v = dictionary_vendors; v; v = v->next) {
				if (strcmp(s, v->vendorname) == 0)
					vendor = v->vendorpec;
			}
			if (!vendor) {
				radlog(L_ERR|L_CONS,
			"dict_init: unknown vendor %s on line %d of %s",
					s, line_no, dict_file);
				return NULL;
			}
		}
		s = strtok(NULL, ",");
	}

#ifdef COMPAT_1543
	/*
	 *	Convert old values 221,1036-1039 to the new
	 *	ones, in case the dictionary is still the
	 *	one from 1.5.4.3
	 *
	 *	XXX - this is a HACK !!
	 */
	switch (value) {
		case 221:
			value = PW_HUNTGROUP_NAME;
			break;
		case 1036:
			value = PW_FALL_THROUGH;
			break;
		case 1038:
			value = PW_EXEC_PROGRAM;
			break;
		case 1039:
			value = PW_EXEC_PROGRAM_WAIT;
			break;
	}
#endif
	/* Create a new attribute for the list */
	if((attr = (DICT_ATTR *)malloc(sizeof(DICT_ATTR))) ==
			(DICT_ATTR *)NULL) {
		radlog(L_ERR|L_CONS, "dict_init: out of memory");
		return NULL;
	}
	strNcpy(attr->name, namestr, sizeof(attr->name));
	attr->value = value;
	attr->type = type;
	attr->flags = flags;
	if (vendor)
		attr->value |= (vendor << 16);

	return attr;
}

/*
 *	Initialize the dictionary.  Read all ATTRIBUTES into
 *	the dictionary_attributes list.  Read all VALUES into
 *	the dictionary_values list.
 */
int dict_init(char *dir, char *fn)
{
	DICT_ATTR	*attr;
	DICT_VALUE	*dval;
	FILE		*dictfd;
	char		dummystr[64];
	char		namestr[64];
	char		valstr[64];
	char		attrstr[64];
	char		buffer[256];
	char		filenm[256];
	char		*dict_file;
	int		line_no;
	int		value;
	int		is_attrib;
#if defined(ATTRIB_NMC) || 1 /* Makes code simpler */
	int		is_nmc = 0;
	int		vendor_usr_seen = 0;
#endif

	if (fn == NULL) dict_free();

	if (fn) {
		if (fn[0] == '/')
			strNcpy(filenm, fn, sizeof(filenm));
		else
			sprintf(filenm, "%.127s/%.127s", dir, fn);
	} else
		sprintf(filenm, "%.200s/%.50s", dir, RADIUS_DICTIONARY);

	if ((dictfd = fopen(filenm, "r")) == NULL) {
		radlog(L_CONS|L_ERR, "dict_init: Couldn't open dictionary: %s",
			filenm);
		return -1;
	}

	line_no = 0;
	dict_file = filenm;
	while (fgets(buffer, sizeof(buffer), dictfd) != NULL) {
		line_no++;
		
		/* Skip empty space */
		if (*buffer == '#' || *buffer == '\0' || *buffer == '\n') {
			continue;
		}

		if (strncasecmp(buffer, "$INCLUDE", 8) == 0) {

			/* Read the $INCLUDE line */
			if(sscanf(buffer, "%63s%63s", dummystr, valstr) != 2) {
				radlog(L_ERR,
			"%s: Invalid filename on line %d of %s\n",
					progname, line_no, dict_file);
				return(-1);
			}
			if (dict_init(dir, valstr) < 0)
				return -1;
			continue;
		}

		is_attrib = 0;
		if (strncmp(buffer, "ATTRIBUTE", 9) == 0)
			is_attrib = 1;
#ifdef ATTRIB_NMC
		is_nmc = 0;
		if (strncmp(buffer, "ATTRIB_NMC", 10) == 0)
			is_attrib = is_nmc = 1;
#endif
		if (is_attrib) {
			attr = parse_attr(buffer, line_no, dict_file,
				is_nmc, &vendor_usr_seen);
			if (attr == NULL)
				return -1;
			/*
			 *	Add to the front of the list, so that
			 *	values at the end of the file override
			 *	those in the begin.
			 */
			attr->next = dictionary_attributes;
			dictionary_attributes = attr;
		}
		else if (strncmp(buffer, "VALUE", 5) == 0) {

			/* Read the VALUE line */
			if(sscanf(buffer, "%63s%63s%63s%63s", dummystr, attrstr,
						namestr, valstr) != 4) {
				radlog(L_ERR|L_CONS,
		"dict_init: Invalid value entry on line %d of %s",
					line_no, dict_file);
				return(-1);
			}

			/*
			 * Validate all entries
			 */
			if(strlen(attrstr) > 39) {
				radlog(L_ERR|L_CONS,
		"dict_init: Invalid attribute length on line %d of %s",
					line_no, dict_file);
				return(-1);
			}

			if(strlen(namestr) > 39) {
				radlog(L_ERR|L_CONS,
		"dict_init: Invalid name length on line %d of d%s",
					line_no, dict_file);
				return(-1);
			}

			if(!isdigit(*valstr)) {
				radlog(L_ERR|L_CONS,
			"dict_init: Invalid value on line %d of %s",
					line_no, dict_file);
				return(-1);
			}
			value = atoi(valstr);

			/* Create a new VALUE entry for the list */
			if((dval = (DICT_VALUE *)malloc(sizeof(DICT_VALUE))) ==
					(DICT_VALUE *)NULL) {
				radlog(L_ERR|L_CONS, "dict_init: out of memory");
				return(-1);
			}
			strNcpy(dval->attrname, attrstr, sizeof(dval->attrname));
			strNcpy(dval->name, namestr, sizeof(dval->name));
			dval->value = value;

			/* Insert at front. */
			dval->next = dictionary_values;
			dictionary_values = dval;
		}
		else if(strncmp(buffer, "VENDOR", 6) == 0) {

			/* Read the VENDOR line */
			if(sscanf(buffer, "%63s%63s%63s", dummystr, attrstr,
						valstr) != 3) {
				radlog(L_ERR|L_CONS,
		"dict_init: Invalid vendor entry on line %d of %s",
					line_no, dict_file);
				return(-1);
			}

			/*
			 * Validate all entries
			 */
			if(strlen(attrstr) > 39) {
				radlog(L_ERR|L_CONS,
		"dict_init: Invalid attribute length on line %d of %s",
					line_no, dict_file);
				return(-1);
			}

			if(!isdigit(*valstr)) {
				radlog(L_ERR|L_CONS,
			"dict_init: Invalid value on line %d of %s",
					line_no, dict_file);
				return(-1);
			}
			value = atoi(valstr);

			/* Create a new VENDOR entry for the list */
			if (addvendor(attrstr, value) < 0)
				return -1;
#ifdef ATTRIB_NMC
			if (value == VENDORPEC_USR)
				vendor_usr_seen = 1;
#endif
		}
	}
	fclose(dictfd);
	return(0);
}

/*
 *	Return the full attribute structure based on the
 *	attribute id number.
 */
DICT_ATTR *dict_attrget(int attribute)
{
	DICT_ATTR	*attr;

	for (attr = dictionary_attributes; attr; attr = attr->next) {
		if (attr->value == attribute)
			break;
	}
	return attr;
}

/*
 *	Return the full attribute structure based on the
 *	attribute name.
 */
DICT_ATTR *dict_attrfind(char *attrname)
{
	DICT_ATTR	*attr;

	for (attr = dictionary_attributes; attr; attr = attr->next) {
		if (DICT_STRCMP(attr->name, attrname) == 0)
			break;
	}
	return attr;
}

/*
 *	Return the full value structure based on the
 *	value name.
 */
DICT_VALUE *dict_valfind(char *valname, char *attrname)
{
	DICT_VALUE	*val;

	for (val = dictionary_values; val; val = val->next) {
		if (DICT_STRCMP(val->attrname, attrname) == 0 &&
		    DICT_STRCMP(val->name, valname) == 0)
			break;
	}
	return val;
}

/*
 *	Return the full value structure based on the
 *	actual value and the associated attribute name.
 */
DICT_VALUE *dict_valget(UINT4 value, char *attrname)
{
	DICT_VALUE	*val;

	for (val = dictionary_values; val; val = val->next) {
		if (DICT_STRCMP(val->attrname, attrname) == 0 &&
		    val->value == value)
			break;
	}
	return val;
}

