/*
FILE
	template.h
	svn ID removed
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

//
//These functions are deprecated and should not be used in new software
//
//For templates that have an associated tTemplate.cQuery
void TemplateSelect(char *cTemplateName);
void TemplateSelectWithQuery(char *cTemplateName);
int fileStructTemplate(FILE *fp,char *cTemplateName,struct t_template *ptrTemplate,char *cQuery);
int fileStructTemplateWithQuery(FILE *fp, char *cTemplateName,
		                struct t_template *ptrTemplate);


//
//New function names for backwards compatability with above now deprecated functions.
//
//Extended functions
void TemplateSelectInterface(char *cTemplateName,unsigned uTemplateSet,unsigned uTemplateType);
void TemplateSelectInterfaceWithQuery(char *cTemplateName,unsigned uTemplateSet, unsigned uTemplateType);
int fileStructTemplateInterface(FILE *fp,char *cTemplateName,struct t_template *ptrTemplate,char *cQuery,
			unsigned uTemplateSet,unsigned uTemplateType);
int fileStructTemplateInterfaceWithQuery(FILE *fp, char *cTemplateName,struct t_template *ptrTemplate,
		unsigned uTemplateSet, unsigned uTemplateType);


//
//These functions stay unchanged but should not be called directly
//
//General Purpose
int TemplatePairsQuery(char *cSQLQuery,struct t_template *template);
void Template(char *cTemplate, struct t_template *template, FILE *fp);
void TemplateToBuffer(char *cTemplate, struct t_template *template, char *cBuffer);

//Globals
extern MYSQL gMysql;
extern void AppFunctions(FILE *fp,char *cFunction);
