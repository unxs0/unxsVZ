/*
FILE
	template.h
	$Id: template.h 1282 2007-07-03 23:44:44Z Gary $
PURPOSE
	Template function library header file to use with RAD3 back-office
	applications that have tTemplate and tTemplateSet model.
NOTES
	Applications that link to this static lib must provide:
	1-. A valid MYSQL gMysql structure connected to a mySQL server.
	2-. A function called AppFunctionProcessing() that may provide
	special {{funcName}} processing to FILE *fp;
AUTHOR
	(C) 2006 Gary Wallis.
 
*/



//Template system items
typedef struct t_template {
        char *cpName[100];//pointers to var_name strings
        char *cpValue[100];//pointers to substitution strings
} t_template ;

//For templates that have an associated tTemplate.cQuery
int fileStructTemplateWithQuery(FILE *fp, char *cTemplateName,
		                struct t_template *ptrTemplate);
void TemplateSelectWithQuery(char *cTemplateName);

//General Purpose
int TemplatePairsQuery(char *cSQLQuery,struct t_template *template);
void Template(char *cTemplate, struct t_template *template, FILE *fp);
void TemplateSelect(char *cTemplateName);
int fileStructTemplate(FILE *fp,char *cTemplateName,struct t_template *ptrTemplate,char *cQuery);
void TemplateToBuffer(char *cTemplate, struct t_template *template, char *cBuffer);

//Globals
extern MYSQL gMysql;
extern void AppFunctions(FILE *fp,char *cFunction);

