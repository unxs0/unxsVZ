/*
 *
 * pair.c	Routines to deal with value-pairs.
 *
 *		Copyright 1997-2001	Cistron Internet Services B.V.
 */

char pair_rcsid[] =
"svn ID removed

#include	<sys/types.h>
#include	<sys/time.h>
#include	<time.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>

#include	"radiusd.h"


static char *months[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

#define PARSE_MODE_NAME		0
#define PARSE_MODE_EQUAL	1
#define PARSE_MODE_VALUE	2
#define PARSE_MODE_INVALID	3

/*
 *	Create a new pair.
 */
VALUE_PAIR *paircreate(int attr, int type)
{
	VALUE_PAIR	*vp;
	DICT_ATTR	*d;

	if ((vp = malloc(sizeof(VALUE_PAIR))) == NULL)
		return NULL;
	memset(vp, 0, sizeof(VALUE_PAIR));
	vp->attribute = attr;
	vp->operator = PW_OPERATOR_EQUAL;
	vp->type = type;
	if ((d = dict_attrget(attr)) != NULL)
		strcpy(vp->name, d->name);
	else
		sprintf(vp->name, "Attr-%d", attr);
	switch (vp->type) {
		case PW_TYPE_INTEGER:
		case PW_TYPE_IPADDR:
		case PW_TYPE_DATE:
			vp->length = 4;
			break;
		default:
			vp->length = 0;
			break;
	}

	return vp;
}

/*
 *	Release the memory used by a list of attribute-value
 *	pairs.
 */
void pairfree(VALUE_PAIR *pair)
{
	VALUE_PAIR	*next;

	while(pair != NULL) {
		next = pair->next;
		free(pair);
		pair = next;
	}
}


/*
 *	Find the pair with the mathing attribute
 */
VALUE_PAIR * pairfind(VALUE_PAIR *first, int attr)
{
	while(first && first->attribute != attr)
		first = first->next;
	return first;
}


/*
 *	Delete the pair(s) with the mathing attribute
 */
void pairdelete(VALUE_PAIR **first, int attr)
{
	VALUE_PAIR *i, *next, *last = NULL;

	for(i = *first; i; i = next) {
		next = i->next;
		if (i->attribute == attr) {
			if (last)
				last->next = next;
			else
				*first = next;
			free(i);
		} else
			last = i;
	}
}

/*
 *	Add a pair at the end of a VALUE_PAIR list.
 */
void pairadd(VALUE_PAIR **first, VALUE_PAIR *new)
{
	VALUE_PAIR *i;

	if (*first == NULL) {
		*first = new;
		return;
	}
	for(i = *first; i->next; i = i->next)
		;
	i->next = new;
}


/*
 *	Move attributes from one list to the other
 *	if not already present.
 */
void checkpair_move(VALUE_PAIR **to, VALUE_PAIR **from)
{
	VALUE_PAIR	*tailto, *i, *next;
	VALUE_PAIR	*tailfrom = NULL;
	int		has_password = 0;

	if (*to == NULL) {
		*to = *from;
		*from = NULL;
		return;
	}

	/*
	 *	First, see if there are any passwords here, and
	 *	point "tailto" to the end of the "to" list.
	 */
	tailto = *to;
	for(i = *to; i; i = i->next) {
		if (i->attribute == PW_PASSWORD ||
		/*
		 *	FIXME: this seems to be needed with PAM support
		 *	to keep it around the Auth-Type = Pam stuff.
		 *	Perhaps we should only do this if Auth-Type = Pam?
		 */
#ifdef PAM
		    i->attribute == PAM_AUTH_ATTR ||
#endif
		    i->attribute == PW_CRYPT_PASSWORD)
			has_password = 1;
		tailto = i;
	}

	/*
	 *	Loop over the "from" list.
	 */
	for(i = *from; i; i = next) {
		next = i->next;
		/*
		 *	If there was a password in the "to" list,
		 *	do not move any other password from the
		 *	"from" to the "to" list.
		 */
		if (has_password &&
		    (i->attribute == PW_PASSWORD ||
#ifdef PAM
		     i->attribute == PAM_AUTH_ATTR ||
#endif
		     i->attribute == PW_CRYPT_PASSWORD)) {
			tailfrom = i;
			continue;
		}

		/*
		 *	If the attribute is already present in "to",
		 *	do not move it from "from" to "to".
		 *
		 *	However, we always move "Hint" to the "to" list.
		 */
		if (i->attribute != PW_HINT &&
		    pairfind(*to, i->attribute) != NULL &&
		    (i->flags.has_tag == 0 ||
		     i->flags.tag == (*to)->flags.tag)) {
			DEBUG2("WARNING: Duplicate check attribute %s is being ignored!", i->name);
			tailfrom = i;
			continue;
		}

		if (tailfrom)
			tailfrom->next = next;
		else
			*from = next;
		tailto->next = i;
		i->next = NULL;
		tailto = i;
	}
}

/*
 *	Move attributes from one list to the other
 *	if not already present.
 */
void replypair_move(VALUE_PAIR **to, VALUE_PAIR **from, int group)
{
	VALUE_PAIR	*tailto, *i, *next, *vp;
	VALUE_PAIR	*tailfrom = NULL;

	if (*to == NULL) {
		*to = *from;
		*from = NULL;
		return;
	}
	tailto = *to;
	while (tailto && tailto->next)
		tailto = tailto->next;

	/*
	 *	Loop over the "from" list.
	 */
	for (i = *from; i; i = next) {
		next = i->next;
		i->group = group;

		/*
		 *	We never move "Fall-Through" to the "to" list.
		 */
		if (i->attribute == PW_FALL_THROUGH) {
			tailfrom = i;
			continue;
		}

#if 1
		/*
		 *	We always move "Framed-Route" to the "to" list.
		 *
		 *	If the attribute is already present in "to",
		 *	do not move it from "from" to "to", unless it
		 *	is part of the same group (i.e. was defined in
		 *	the same radius profile)
		 *
		 *	NOTE: THIS MAY CAUSE PROBLEMS FOR YOU.
		 *	Comment out this section of code if you don't
		 *	like it, by changing the previous
		 *	'#if 1' to '#if 0'
		 */
		if (i->attribute != PW_FRAMED_ROUTE &&
		    (vp = pairfind(*to, i->attribute)) != NULL &&
		    vp->group != i->group &&
		    (i->flags.has_tag == 0 || 
		     i->flags.tag == (*to)->flags.tag)) {
			DEBUG2("WARNING: Duplicate reply attribute %s is being ignored!", i->name);
			tailfrom = i;
			continue;
		}
#endif
		if (tailfrom)
			tailfrom->next = next;
		else
			*from = next;
		tailto->next = i;
		i->next = NULL;
		tailto = i;
	}
}

/*
 *	Set all pairs to a certain group.
 */
void pairgroup_set(VALUE_PAIR *vp, int group)
{
	while (vp) {
		vp->group = group;
		vp = vp->next;
	}
}

/*
 *	Move one kind of attributes from one list to the other
 */
void pairmove2(VALUE_PAIR **to, VALUE_PAIR **from, int attr)
{
	VALUE_PAIR *to_tail, *i, *next;
	VALUE_PAIR *iprev = NULL;

	/*
	 *	Find the last pair in the "to" list and put it in "to_tail".
	 */
	if (*to != NULL) {
		to_tail = *to;
		for(i = *to; i; i = i->next)
			to_tail = i;
	} else
		to_tail = NULL;

	for(i = *from; i; i = next) {
		next = i->next;

		if (i->attribute != attr) {
			iprev = i;
			continue;
		}

		/*
		 *	Remove the attribute from the "from" list.
		 */
		if (iprev)
			iprev->next = next;
		else
			*from = next;

		/*
		 *	Add the attribute to the "to" list.
		 */
		if (to_tail)
			to_tail->next = i;
		else
			*to = i;
		to_tail = i;
		i->next = NULL;
	}
}


/*
 *	Calculate values of \t, \r, \010 etc
 */
static int escape(char **uptr)
{
	int	c;
	char	*p;
	char	buf[4];

	p = *uptr;
	if (isdigit(p[0]) && isdigit(p[1]) && isdigit(p[2])) {
		memcpy(buf, p, 3);
		buf[4] = 0;
		p += 3;
		*uptr = p;
		return strtol(buf, NULL, 8);
	}

	c = *p++;
	switch (c) {
		case 'r': c = '\r'; break;
		case 'n': c = '\n'; break;
		case 't': c = '\t'; break;
	}
	*uptr = p;

	return c;
}

/*
 *	Copy a data field from the buffer.  Advance the buffer
 *	past the data field.
 */
static void fieldcpy(char *string, int len, char **uptr)
{
	char		*ptr, *foo;
	int		quote = 0;
	int		c;

	ptr = *uptr;
	foo = string;

	while (*ptr) {
		if (ptr[0] == '\\' && ptr[1]) {
			ptr++;
			c = escape(&ptr);
			if (len-- > 0) *string++ = c;
			continue;
		}
		if (*ptr == '"') {
			quote = !quote;
			ptr++;
			continue;
		}
		if (!quote && strchr(" \t\r\n=,<>!", *ptr))
			break;
		if (len-- > 0) *string++ = *ptr;
		ptr++;
	}
	*string = '\0';
	*uptr = ptr;
}

/*
 *	Turn printable string into correct tm struct entries
 */
static void user_gettime(char *valstr, struct tm *tm)
{
	int	i;

	/* Get the month */
	for(i = 0; i < 12; i++) {
		if(strncmp(months[i], valstr, 3) == 0) {
			tm->tm_mon = i;
			i = 13;
		}
	}

	/* Get the Day */
	tm->tm_mday = atoi(&valstr[4]);

	/* Now the year */
	tm->tm_year = atoi(&valstr[7]) - 1900;
}

/*
 *	Parse the buffer to extract the attribute-value pairs.
 */
int userparse(char *buffer, VALUE_PAIR **first_pair, int resolve)
{
	int		mode;
	char		attrstr[256];
	char		valstr[256];
	char		*s, *c;
	DICT_ATTR	*attr = NULL;
	DICT_VALUE	*dval;
	VALUE_PAIR	*pair;
	struct tm	*tm;
	time_t		timeval;
	int		operator;
	int		rcode;
	int		tag;

	rcode = USERPARSE_EOS;
	mode = PARSE_MODE_NAME;
	while(*buffer != '\n' && *buffer != '\r' && *buffer != '\0') {

		if (*buffer == ',') {
			rcode = USERPARSE_COMMA;
		}

		if(*buffer == ' ' || *buffer == '\t' || *buffer == ',') {
			buffer++;
			continue;
		}

		rcode = USERPARSE_EOS;
		switch(mode) {

		case PARSE_MODE_NAME:
			/* Attribute Name */
			fieldcpy(attrstr, sizeof(attrstr), &buffer);

#ifndef MERIT_TAG_FORMAT
			/* 
			 * Handle tagged attributes in our own format,
			 * attribute:tag.
			 */ 
			tag = 0;
			s = strrchr(attrstr, ':');
			if (s && s[1]) {

				/* Colon found with something behind it */
				if (s[1] == '*' && s[2] == 0) {

					/* Wildcard tag for check items */
					tag = TAG_ANY;
					*s = 0;
				}
				else {

					/* Real tag */
					tag = strtol(s + 1, &c, 0);
					if (c && !*c && TAG_VALID_ZERO(tag))
						*s=0;
					else tag = 0;
				}
			}
#endif

			/* Now look up the (possibly stripped) attribute name */
			if ((attr = dict_attrfind(attrstr)) == NULL) {
				radlog(L_ERR|L_CONS, "Unknown attribute \"%s\"",
					attrstr);
				return(-1);
			}
			mode = PARSE_MODE_EQUAL;
			break;

		case PARSE_MODE_EQUAL:
			mode = PARSE_MODE_VALUE;
			/* '=' Equal sign */
			if (*buffer == '=') {
				buffer++;
				operator = PW_OPERATOR_EQUAL;
			} else
			/* '<' */
			if (*buffer == '<' && buffer[1] != '=') {
				buffer++;
				operator = PW_OPERATOR_LESS_THAN;
			} else
			/* '>' */
			if (*buffer == '>' && buffer[1] != '=') {
				buffer++;
				operator = PW_OPERATOR_GREATER_THAN;
			} else
			/* '!=' */
			if (memcmp(buffer, "!=", 2) == 0) {
				buffer += 2;
				operator = PW_OPERATOR_NOT_EQUAL;
			} else
			/* '<=' */
			if (memcmp(buffer, "<=", 2) == 0) {
				buffer += 2;
				operator = PW_OPERATOR_LESS_EQUAL;
			} else
			/* '>=' */
			if (memcmp(buffer, ">=", 2) == 0) {
				buffer += 2;
				operator = PW_OPERATOR_GREATER_EQUAL;
			} else
			/* ':=' */
			if (memcmp(buffer, ":=", 2) == 0) {
				buffer += 2;
				operator = PW_OPERATOR_SET;
			} else
			/* '+=' */
			if (memcmp(buffer, "+=", 2) == 0) {
				buffer += 2;
				operator = PW_OPERATOR_ADD;
			} else
			/* '-=' */
			if (memcmp(buffer, "-=", 2) == 0) {
				buffer += 2;
				operator = PW_OPERATOR_SUB;
			}
			else {
				radlog(L_ERR|L_CONS,
					"Unexpected character `%c' (0x%02x)",
					*buffer, *buffer);
				return -1;
			}
			break;

		case PARSE_MODE_VALUE:
			/* Value */
#ifdef MERIT_TAG_FORMAT
			/* 
			 * Handle tagged attributes in Merit's format, as in
			 * attribute = :tag:value. This is done before fieldcpy,
			 * so we can support String = :1:"Hello World".
			 */ 
			tag = 0;
			if (*buffer == ':' && attr->flags.has_tag) {

				/* Colon found and attribute allows a tag */
				if (buffer[1] == '*' && buffer[2] == ':') {

					/* Wildcard tag for check items */
					tag = TAG_ANY;
					buffer += 3;
				}
				else {

					/* Real tag */
					tag = strtol(buffer + 1, &c, 0);
					if (c && *c==':' && TAG_VALID_ZERO(tag))
						buffer = c + 1;
					else tag = 0;
				}
			}
#endif
			/* Copy value from (possibly advanced) buffer */
			fieldcpy(valstr, sizeof(valstr), &buffer);
			if((pair = (VALUE_PAIR *)malloc(sizeof(VALUE_PAIR))) ==
						(VALUE_PAIR *)NULL) {
				radlog(L_CONS|L_ERR, "no memory");
				exit(1);
			}
			memset(pair, 0, sizeof(VALUE_PAIR));
			strNcpy(pair->name, attr->name, sizeof(pair->name));
			pair->attribute = attr->value;
			pair->type = attr->type;
			pair->flags = attr->flags;
			pair->operator = operator;

			/* Fill in the tag - better do it via 'tag', not via 
			   attr->flags itself, in the case of our own syntax. 
			   Keep 'attr' sort of readonly, because it really 
			   belongs to the dictionary, so to speak. */
			pair->flags.tag = tag;

			/* _Always_ copy value as string. */
			strNcpy(pair->strvalue, valstr, sizeof(pair->strvalue));

			switch(pair->type) {

			case PW_TYPE_STRING:
				/* Pair->strvalue has been set already */
				pair->length = strlen(pair->strvalue);
				break;

			case PW_TYPE_INTEGER:
				/*
				 *	If it starts with a digit, it must
				 *	be a number (or a range).
				 */
				if (isdigit(*valstr)) {
					pair->lvalue = strtoul(valstr, NULL, 10);
					pair->length = 4;
				}
				else if ((dval = dict_valfind(valstr, attr->name)) == NULL) {
					free(pair);
					radlog(L_ERR|L_CONS,
					"Unknown value \"%s\" for attribute %s",
						valstr, attr->name);
					return(-1);
				}
				else {
					pair->lvalue = dval->value;
					pair->length = 4;
				}
				break;

			case PW_TYPE_IPADDR:
				if (resolve == 0) {
					pair->lvalue = 0;
					pair->length = 4;
					break;
				}
				if (pair->attribute != PW_FRAMED_IP_ADDRESS) {
					pair->lvalue = get_ipaddr(valstr);
					pair->length = 4;
					break;
				}

				/*
				 *	We allow a "+" at the end to
				 *	indicate that we should add the
				 *	portno. to the IP address.
				 */
				if (valstr[0]) {
					for(s = valstr; s[1]; s++)
						;
					if (*s == '+') {
						*s = 0;
						pair->flags.addport = 1;
					}
				}
				pair->lvalue = get_ipaddr(valstr);
				pair->length = 4;

				break;

			case PW_TYPE_DATE:
				timeval = time(0);
				tm = localtime(&timeval);
				user_gettime(valstr, tm);
#ifdef TIMELOCAL
				pair->lvalue = (UINT4)timelocal(tm);
#else /* TIMELOCAL */
				pair->lvalue = (UINT4)mktime(tm);
#endif /* TIMELOCAL */
				pair->length = 4;
				break;

			default:
				free(pair);
				radlog(L_ERR|L_CONS, "Unknown attribute type %d",
					pair->type);
				return(-1);
			}
			pairadd(first_pair, pair);
			mode = PARSE_MODE_NAME;
			break;

		default:
			mode = PARSE_MODE_NAME;
			break;
		}
	}

	/*
	 *	Double-check that we've parsed a complete Attribute=value
	 */
	if (mode != PARSE_MODE_NAME) {
		radlog(L_ERR|L_CONS, "Got end of line while still parsing last attribute");
		return(-1);
	}

	return rcode;
}


