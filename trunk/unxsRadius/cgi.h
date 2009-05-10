/*
FILE
	$Id: cgi.h,v 1.11 2003/03/05 18:09:17 ggw Exp $
LEGAL
	Public Domain. See cgi.c file for more info
*/

typedef struct {
    char *name;
    char *val;
} pentry;

typedef struct {
    char name[128];
    char val[128];
} entry;

void getword(char *word, char *line, char stop);
char x2c(char *what);
void unescape_url(char *url);
void plustospace(char *str);
void spacetoplus(char *str);
char *makeword(char *line, char stop);
char *fmakeword(FILE *f, char stop, int *len); 
void escape_shell_cmd(char *cmd);

#include <string.h>
