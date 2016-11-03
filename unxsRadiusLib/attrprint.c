/*
 * attrprint.c	Functions to print A/V pairs.
 *
 *		Copyright 1998-2001	Cistron Internet Services B.V.
 *		Copyright 2002		Cistron IP B.V.
 */
char attrprint_rcsid[] =
"svn ID removed

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/time.h>
#include	<netinet/in.h>

#include	<stdio.h>
#include	<netdb.h>
#include	<pwd.h>
#include	<time.h>
#include	<ctype.h>

#include	"radiusd.h"


/*
 *	Print the Attribute-value pair to the desired File.
 *	FIXME: work with logging routines in log.c
 */
void debug_pair(FILE *fd, VALUE_PAIR *pair)
{
	if(debug_flag) {
		fputs("    ", fd);
		fprint_attr_val(fd, pair);
		fputs("\n", fd);
	}
}


/*
 *	Write a whole list of A/V pairs.
 */
void fprint_attr_list(FILE *fd, VALUE_PAIR *pair)
{
	while(pair) {
		fprintf(fd, "    ");
		fprint_attr_val(fd, pair);
		fprintf(fd, "\n");
		pair = pair->next;
	}
}

static void cprint(FILE *fp, int c)
{
	if (c == '\\' || c == '"' || c < 32 || c > 127) switch (c) {
		case '\\':
			fputs("\\\\", fp);
			break;
		case '"':
			fputs("\\\"", fp);
			break;
		case '\r':
			fputs("\\r", fp);
			break;
		case '\n':
			fputs("\\n", fp);
			break;
		case '\t':
			fputs("\\t", fp);
			break;
		default:
			fprintf(fp, "\\%03o", c);
			break;
	} else
		fputc(c, fp);
}

/*
 *	Write a printable version of the attribute-value
 *	pair to the supplied File.
 */
void fprint_attr_val(FILE *fd, VALUE_PAIR *pair)
{
	DICT_VALUE	*dict_valget();
	DICT_VALUE	*dval;
	char		buffer[32];
	u_char		*ptr;
	UINT4		vendor;
	int		i, left;

	if (pair->flags.has_tag) {
#ifdef MERIT_TAG_FORMAT
		fprintf(fd, "%s = :%d:", pair->name,pair->flags.tag);
#else
		fprintf(fd, "%s:%d = ", pair->name,pair->flags.tag);
#endif
	}
	else {
		fprintf(fd, "%s = ",pair->name);
	}

	switch(pair->type) {

	case PW_TYPE_STRING:
		fputc('"',fd);
		ptr = (u_char *)pair->strvalue;
		if (pair->attribute != PW_VENDOR_SPECIFIC) {
			left = pair->length;
			while(left-- > 0) {
				/*
				 *	Ugh! Ascend gear sends "foo"
				 *	as "foo\0", length 4.
				 *	Suppress trailing zeros.
				 */
				if (left == 0 && *ptr == 0)
					break;
				cprint(fd, *ptr);
				ptr++;
			}
			fputc('"', fd);
			break;
		}
		/*
		 *	Special format, print out as much
		 *	info as we can.
		 */
		if (pair->length < 6) {
			fprintf(fd, "(invalid length: %d)\"", pair->length);
			break;
		}
		memcpy(&vendor, ptr, 4);
		ptr += 4;
		fprintf(fd, "V%d", (int)ntohl(vendor));
		left = pair->length - 4;
		while (left >= 2) {
#ifdef ATTRIB_NMC
			if (ntohl(vendor) == VENDORPEC_USR) {
				int type;
				if (left < 4) break;
				memcpy(&type, ptr, 4);
				left -= 4;
				i = left;
				fprintf(fd, ":T%d:L%d:",
					(int)ntohl(type), i);
				ptr += 4;
			} else
#endif
			{
				left -= 2;
				i = ptr[1] - 2;
				fprintf(fd, ":T%d:L%d:", ptr[0], i + 2);
				ptr += 2;
			}
			while (i > 0 && left > 0) {
				cprint(fd, *ptr++);
				i--;
				left--;
			}
		}
		fputc('"', fd);
		break;

	case PW_TYPE_INTEGER:
		dval = dict_valget(pair->lvalue, pair->name);
		if(dval != (DICT_VALUE *)NULL) {
			fprintf(fd, "%s", dval->name);
		}
		else {
			fprintf(fd, "%lu", (unsigned long)pair->lvalue);
		}
		break;

	case PW_TYPE_IPADDR:
		if (pair->lvalue == 0 && pair->strvalue[0] != 0) {
			/* Why is this??? - EvB */
			/* Because of the 192.168.1.1+ format -- miquels */
			fprintf(fd, "%s", pair->strvalue);
		}
		else {
			ipaddr2str(buffer, pair->lvalue);
			fprintf(fd, "%s", buffer);
		}
		break;

	case PW_TYPE_DATE:
		strftime(buffer, sizeof(buffer), "%b %e %Y",
					localtime((time_t *)&pair->lvalue));
		fprintf(fd, "\"%s\"", buffer);
		break;

	default:
		fprintf(fd, "Unknown type %d", pair->type);
		break;
	}
}
