/*
FILE 
	unxsrad.c
	$Id$
PURPOSE
	unxsRAD job queue CLI standalone program	
LEGAL
	(C) Gary Wallis 2012 for Unixservice, LLC. All Rights Reserved.
	LICENSE file should be included in distribution.
OTHER
HELP

*/

#include "unxsrad.h"

//Global vars
MYSQL gMysql;
char gcQuery[8192]={""};
char *gcBuildInfo="$Id$";
static FILE *gLfp=NULL;//log file
char gcHostname[100]={""};
static unsigned guTable=0;
static char gcTableName[32]={""};
static char gcTableNameLC[64]={""};
static char gcTableKey[33]={""};
unsigned guCookieProject=0;
unsigned guCookieTable=0;

//prototype TOC
void ProcessJobQueue(void);
void MakeSourceCodeJob(unsigned uJob,char const *cJobData);
void logfileLine(const char *cFunction,const char *cLogline);
unsigned CreateFileFromTemplate(unsigned uTemplate,unsigned uTable);
unsigned CreateModuleFile(unsigned uTemplate, unsigned uTable);
unsigned CreateModuleFuncFile(unsigned uTemplate, unsigned uTable);
void funcModuleListPrint(FILE *fp);
void funcModuleListTable(FILE *fp);
void funcModuleLoadVars(FILE *fp);
void funcModuleProcVars(FILE *fp);
void funcModuleRAD3Input(FILE *fp);
void funcModuleVars(FILE *fp);
void funcModuleVarList(FILE *fp);
void funcModuleUpdateQuery(FILE *fp);
void funcModuleInsertQuery(FILE *fp);
void funcModuleCreateQuery(FILE *fp);
void AppFunctions(FILE *fp,char *cFunction);
void StripQuotes(char *cLine);
const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);
char *WordToLower(char *cInput);


//external prototypes
unsigned TextConnectDb(void); //mysqlconnect.c

int main(int iArgc, char *cArgv[])
{
	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"Could not open logfile: %s\n",cLOGFILE);
		exit(300);
       	}

	if(gethostname(gcHostname,99)!=0)
	{
		logfileLine("ProcessJobQueue","gethostname() failed");
		exit(1);
	}
	logfileLine("main",gcHostname);

	//Uses login data from local.h
	if(TextConnectDb())
		exit(1);

	if(iArgc==2)
	{
		if(!strcmp(cArgv[1],"ProcessJobQueue"))
			ProcessJobQueue();
	}
	else if(1)
	{
		printf("usage: %s ProcessJobQueue\n",cArgv[0]);
	}
	fclose(gLfp);
	mysql_close(&gMysql);
	return(0);
}//main()


void ProcessJobQueue(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uJob=0;

	sprintf(gcQuery,"SELECT uJob,cJobName,cJobData FROM tJob,tServer"
			" WHERE tJob.uServer=tServer.uServer AND tServer.cLabel='%s'"
			" AND tJob.uJobStatus=1",
					gcHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessJobQueue",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uJob);
		if(!strcmp(field[1],"MakeSourceCodeJob"))
			MakeSourceCodeJob(uJob,field[2]);
	}
	mysql_free_result(res);

}//void ProcessJobQueue(void)


void MakeSourceCodeJob(unsigned uJob,char const *cJobData)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uProject=0;
	unsigned uTable=0;

	logfileLine("MakeSourceCodeJob","start");

	sscanf(cJobData,"uProject=%u;",&uProject);
	guCookieProject=uProject;

	sprintf(gcQuery,"SELECT tProject.cLabel,tTable.cLabel,tTable.uTable FROM tProject,tTable"
			" WHERE tTable.uProject=tProject.uProject AND tProject.uProject=%u",
					uProject);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessJobQueue",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("%s %s\n",field[0],field[1]);
		sscanf(field[2],"%u",&uTable);
		guCookieTable=uTable;
		CreateFileFromTemplate(1,uTable);
	}

	logfileLine("MakeSourceCodeJob","end");

}//void MakeSourceCodeJob(unsigned uJob,char const *cJobData)


void logfileLine(const char *cFunction,const char *cLogline)
{
	time_t luClock;
	char cTime[32];
	pid_t pidThis;
	const struct tm *tmTime;

	pidThis=getpid();

	time(&luClock);
	tmTime=localtime(&luClock);
	strftime(cTime,31,"%b %d %T",tmTime);

        fprintf(gLfp,"%s unxsRAD[%u].%s: %s\n",cTime,pidThis,cFunction,cLogline);
	fflush(gLfp);

}//void logfileLine(char *cLogline)


#include <openisp/template.h>

unsigned CreateFileFromTemplate(unsigned uTemplate,unsigned uTable)
{
	char cFile[32];
	unsigned uRetVal= -1;

	if(!uTable || !uTemplate) return(uRetVal);

	sprintf(cFile,"%.31s",ForeignKey("tTemplate","cLabel",uTemplate));

	if(!strcmp(cFile,"module.c"))
		uRetVal=CreateModuleFile(uTemplate,uTable);
	else if(!strcmp(cFile,"modulefunc.h"))
		uRetVal=CreateModuleFuncFile(uTemplate,uTable);

	return(uRetVal);

}//unsigned CreateFileFromTemplate()


unsigned CreateModuleFile(unsigned uTemplate, unsigned uTable)
{
	FILE *fp;
	unsigned uRetVal= -1;

	if(!uTable || !uTemplate) return(uRetVal);

	char cFile[100]={""};

	sprintf(gcTableName,"%.31s",ForeignKey("tTable","cLabel",uTable));
	sprintf(gcTableNameLC,"%.63s",gcTableName);
	WordToLower(gcTableNameLC);

	sprintf(cFile,"/tmp/%.63s.c",gcTableNameLC);
	if((fp=fopen(cFile,"w"))==NULL)
		return(1);

       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cTemplate FROM tTemplate WHERE uTemplate=%u",uTemplate);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s\n",mysql_error(&gMysql));
		return(uRetVal);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		struct t_template template;

/*
	funcModuleCreateQuery
	funcModuleInsertQuery
	funcModuleListPrint
	funcModuleListTable
	funcModuleLoadVars
	funcModuleProcVars
	funcModuleRAD3Input
	funcModuleUpdateQuery
	funcModuleVars
	funcModuleVarList
	cProject
	cTableKey
	cTableName
	cTableNameLC
	cTableTitle
*/
			
		template.cpName[0]="cTableName";
		template.cpValue[0]=gcTableName;
			
		template.cpName[1]="cTableNameLC";
		template.cpValue[1]=gcTableNameLC;
			
		template.cpName[2]="cTableKey";
		sprintf(gcTableKey,"u%.31s",gcTableName+1);//New table name includes table type t prefix
		template.cpValue[2]=gcTableKey;

		char cProject[32]={"Project"};
		sprintf(cProject,"%.31s",ForeignKey("tProject","cLabel",guCookieProject));
		template.cpName[3]="cProject";
		template.cpValue[3]=cProject;
			
		char cTableTitle[32]={"Title"};
		sprintf(cTableTitle,"%.31s",ForeignKey("tTable","cLegend",guCookieTable));
		template.cpName[4]="cTableTitle";
		template.cpValue[4]=cTableTitle;
			
		template.cpName[5]="";

		Template(field[0],&template,fp);
		uRetVal=0;
	}
	else
	{
		fprintf(fp,"No such uTemplate %u\n",uTemplate);
		uRetVal=2;
	}
	mysql_free_result(res);
	fclose(fp);

	return(uRetVal);

}//unsigned CreateModuleFile()


unsigned CreateModuleFuncFile(unsigned uTemplate, unsigned uTable)
{
	FILE *fp;
	unsigned uRetVal= -1;

	if(!uTable || !uTemplate) return(uRetVal);

	char cFile[100]={""};

	sprintf(gcTableName,"%.31s",ForeignKey("tTable","cLabel",uTable));
	sprintf(gcTableNameLC,"%.63s",gcTableName);
	WordToLower(gcTableNameLC);

	sprintf(cFile,"/tmp/%.63sfunc.h",gcTableNameLC);
	if((fp=fopen(cFile,"w"))==NULL)
		return(1);

       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cTemplate FROM tTemplate WHERE uTemplate=%u",uTemplate);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s\n",mysql_error(&gMysql));
		return(uRetVal);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		struct t_template template;

/*
	cTableKey
	cTableName
	cTableNameLC
*/
			
		template.cpName[0]="cTableName";
		template.cpValue[0]=gcTableName;
			
		template.cpName[1]="cTableNameLC";
		template.cpValue[1]=gcTableNameLC;
			
		template.cpName[2]="cTableKey";
		sprintf(gcTableKey,"u%.31s",gcTableName+1);//New table name includes table type t prefix
		template.cpValue[2]=gcTableKey;
			
		template.cpName[3]="";

		Template(field[0],&template,fp);
		uRetVal=0;
	}
	else
	{
		fprintf(fp,"No such uTemplate %u\n",uTemplate);
		uRetVal=2;
	}
	mysql_free_result(res);
	fclose(fp);

	return(uRetVal);


}//unsigned CreateModuleFuncFile()



void funcModuleListPrint(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tFieldType.uRADType,tField.cLabel,tField.cFKSpec"
			" FROM tField,tTable,tFieldType"
			" WHERE tField.uTable=tTable.uTable"
			" AND tField.uFieldType=tFieldType.uFieldType"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFieldType=0;
	register int i=0,first=1;
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uFieldType);
		if(uFieldType == COLTYPE_UNIXTIME ||
				uFieldType == COLTYPE_UNIXTIMECREATE ||
				uFieldType == COLTYPE_UNIXTIMEUPDATE )
		{
			fprintf(fp,"\t\ttime_t luTime%d=strtoul(field[%d],NULL,10);\n",
					i,i);
			fprintf(fp,"\t\tchar cBuf%d[32];\n",i);
			fprintf(fp,"\t\tif(luTime%d)\n\t\t\tctime_r(&luTime%d,cBuf%d);\n",i,i,i);
			fprintf(fp,"\t\telse\n\t\t\tsprintf(cBuf%d,\"---\");\n",i);

		}
		else if(uFieldType == COLTYPE_YESNO )
		{
			fprintf(fp,"\t\tlong unsigned luYesNo%d=strtoul(field[%d],NULL,10);\n",
					i,i);
			fprintf(fp,"\t\tchar cBuf%d[4];\n",i);
			fprintf(fp,"\t\tif(luYesNo%d)\n\t\t\tsprintf(cBuf%d,\"Yes\");\n",i,i);
			fprintf(fp,"\t\telse\n\t\t\tsprintf(cBuf%d,\"No\");\n",i);
		}
		i++;
	}


	fprintf(fp,"\t\tprintf(\"");
	mysql_data_seek(res,0);//rewind
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uFieldType);
		if( first && (uFieldType == COLTYPE_RADPRI ||
				uFieldType == COLTYPE_PRIKEY ))
		{
			fprintf(fp,"<td><input type=submit name=ED%%s value=Edit> %%s");
			first=0;
		}
		else if(uFieldType == COLTYPE_TEXT )
		{
			fprintf(fp,"<td><textarea disabled>%%s</textarea>");
		}
		else if(uFieldType == COLTYPE_IMAGE )
		{
			;
		}
		else if(1)
		{
			fprintf(fp,"<td>%%s");
		}
	}
	fprintf(fp,"</tr>\"\n");

	mysql_data_seek(res,0);//rewind
	i=0;
	first=1;
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uFieldType);
		if( first && (uFieldType == COLTYPE_RADPRI ||
				uFieldType == COLTYPE_PRIKEY ))
		{
			fprintf(fp,"\t\t\t,field[0]\n");
			first=0;
		}

		if(uFieldType==COLTYPE_FOREIGNKEY ||
				uFieldType==COLTYPE_SELECTTABLE ||
				uFieldType==COLTYPE_SELECTTABLE_OWNER )//Index into another table
		{
			char cTableName[32]={""};
			char cFieldName[32]={""};
			char *cp;
			if((cp=strchr(field[2],',')))
			{
				*cp=0;
				sprintf(cTableName,"%.31s",field[2]);
				sprintf(cFieldName,"%.31s",cp+1);
				if((cp=strchr(cFieldName,',')))
					*cp=0;
			}
			fprintf(fp,"\t\t\t,ForeignKey(%s,%s,strtoul(field[%d],NULL,10))\n",
							cTableName,cFieldName,i++);
		}
		else if(uFieldType == COLTYPE_UNIXTIME ||
				uFieldType == COLTYPE_UNIXTIMECREATE ||
				uFieldType == COLTYPE_UNIXTIMEUPDATE ||
				uFieldType ==COLTYPE_YESNO )
		{
			fprintf(fp,"\t\t\t,cBuf%d\n",i);
			i++;
		}
		else if(uFieldType == COLTYPE_IMAGE)
		{
			//skip see above
			//fprintf(fp,"\t\t\t,field[0]\n");
			i++;
		}
		else if(1)
		{
			fprintf(fp,"\t\t\t,field[%d]\n",i++);
		}
	}
	fprintf(fp,"\t\t\t\t);\n");
	mysql_free_result(res);

}//void ModuleListPrint(FILE *fp)


void funcModuleListTable(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType"
			" FROM tField,tTable,tFieldType"
			" WHERE tField.uTable=tTable.uTable"
			" AND tField.uFieldType=tFieldType.uFieldType"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);

	unsigned uRADType=0;

	fprintf(fp,"printf(\"<tr bgcolor=black>\"\n");
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uRADType);

		switch(uRADType)
		{
			case COLTYPE_IMAGE:
				//skip
			break;

			default:
				fprintf(fp,"\t\t\"<td><font face=arial,helvetica color=white>%s\"\n",field[0]);
		}
	}
	mysql_free_result(res);
	fprintf(fp,"\t\t\"</tr>\");\n\n");
}//void funcModuleListTable(FILE *fp)


void funcModuleLoadVars(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;
	register int i=0;

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType,tField.uHtmlXSize"
			" FROM tField,tTable,tFieldType"
			" WHERE tField.uTable=tTable.uTable"
			" AND tField.uFieldType=tFieldType.uFieldType"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);

	unsigned uHtmlXSize=0;
	unsigned uRADType=0;
	char cField[32]={""};
	fprintf(fp,"\n");//Cancel out tab placed func
	while((field=mysql_fetch_row(res)))
	{
		sprintf(cField,"%.31s",field[0]);
		sscanf(field[1],"%u",&uRADType);
		sscanf(field[2],"%u",&uHtmlXSize);

		switch(uRADType)
		{
			case COLTYPE_INTUNSIGNED:
			case COLTYPE_SELECTTABLE:
			case COLTYPE_SELECTTABLE_OWNER:
			case COLTYPE_RADPRI:
			case COLTYPE_PRIKEY:
			case COLTYPE_UINTUKEY:
			case COLTYPE_YESNO:
			case COLTYPE_EXTFUNC:
			case COLTYPE_FOREIGNKEY:
			case COLTYPE_FKIMAGE:
			fprintf(fp,"\t\tsscanf(field[%d],\"%%u\",&%s);\n"
					,i,cField);
			break;

			case COLTYPE_UNIXTIME:
			case COLTYPE_UNIXTIMECREATE:
			case COLTYPE_UNIXTIMEUPDATE:
			case COLTYPE_BIGINT:
			fprintf(fp,"\t\tsscanf(field[%d],\"%%lu\",&%s);\n"
					,i,cField);
			break;
			
			case COLTYPE_TEXT:
				fprintf(fp,"\t\t%s=field[%d];\n",cField,i);
			break;
			
			case COLTYPE_IMAGE:
				//skip
				i--;
			break;
			
			case COLTYPE_MONEY:
				fprintf(fp,"\t\tsprintf(%s,\"%%.31s\",field[%d]);\n"
					,cField,i);
			break;

			default:
				fprintf(fp,"\t\tsprintf(%s,\"%%.%us\",field[%d]);\n"
					,cField,uHtmlXSize,i);
			break;
		}
		i++;
	}
	mysql_free_result(res);

}//void funcModuleLoadVars(FILE *fp)


void funcModuleProcVars(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType,"
			" tField.uSQLSize,tField.uModLevel,"
			" tField.uHtmlXSize,tField.uHtmlMax,"
			" tField.cFKSpec,tField.uHtmlYSize,"
			" tField.cExtIndex"
			" FROM tField,tTable,tFieldType"
			" WHERE tField.uTable=tTable.uTable"
			" AND tField.uFieldType=tFieldType.uFieldType"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);

	unsigned uSQLSize=0;
	unsigned uHtmlMax=0;
	unsigned uHtmlXSize=0;
	unsigned uHtmlYSize=0;
	char welse[6]={"else "};
	char empty[2]={""};
	char *temp=empty;
	unsigned uRADType=0;
	unsigned uModLevel=0;
	char cTableName[32]={""};
	char cFieldName[32]={""};
	char cField[32]={""};
	char *cp;
	fprintf(fp,"\n");//Cancel out tab placed func
	while((field=mysql_fetch_row(res)))
	{
		sprintf(cField,"%.31s",field[0]);
		sscanf(field[1],"%u",&uRADType);
		sscanf(field[2],"%u",&uSQLSize);
		sscanf(field[3],"%u",&uModLevel);
		sscanf(field[4],"%u",&uHtmlXSize);
		sscanf(field[5],"%u",&uHtmlMax);
		sscanf(field[7],"%u",&uHtmlYSize);

		//Top
		switch(uRADType)
		{
			case COLTYPE_IMAGE:
				//skip. below also
			break;

			default:
			fprintf(fp,"\t\t%sif(!strcmp(entries[i].name,\"%s\"))\n",temp,cField);
		}

		//Main
		switch(uRADType)
		{

			case COLTYPE_CHAR:
			case COLTYPE_DATETIME:
			case COLTYPE_TIMESTAMP:
			case COLTYPE_VARCHAR:
			case COLTYPE_VARCHARUKEY:
				//cExtIndex can be used for input validation functions. Fix this TODO
				if(field[8][0])
					fprintf(fp,"\t\t\tsprintf(%s,\"%%.%us\",%s(entries[i].val));\n",cField,uHtmlXSize,field[8]);
				else
					fprintf(fp,"\t\t\tsprintf(%s,\"%%.%us\",entries[i].val);\n",cField,uHtmlXSize);
			break;

			case COLTYPE_DECIMAL:
				if(field[8][0])
					fprintf(fp,"\t\t\tsprintf(%s,\"%%.15s\",%s(entries[i].val));\n",cField,field[8]);
				else
					fprintf(fp,"\t\t\tsprintf(%s,\"%%.15s\",entries[i].val);\n",cField);
			break;

			case COLTYPE_MONEY:
				if(field[8][0])
					fprintf(fp,"\t\t\tsprintf(%s,\"%%.15s\",%s(entries[i].val));\n",cField,field[8]);
				else
					//Default input function Strip $ and , 
					//Ex. $250,000.00 should internally be 250000.00
					fprintf(fp,"\t\t\tsprintf(%s,\"%%.31s\",cMoneyInput(entries[i].val));\n",cField);
			break;
			
			case COLTYPE_TEXT:
				fprintf(fp,"\t\t\t%s=entries[i].val;\n",cField);
			break;
			
			case COLTYPE_IMAGE:
				//skip
			break;
			
			case COLTYPE_INTUNSIGNED:
			case COLTYPE_YESNO:
			case COLTYPE_RADPRI:
			case COLTYPE_PRIKEY:
			case COLTYPE_UINTUKEY:
			case COLTYPE_FOREIGNKEY:
			case COLTYPE_FKIMAGE:
			case COLTYPE_EXTFUNC:
			case COLTYPE_SELECTTABLE:
			case COLTYPE_SELECTTABLE_OWNER:
				fprintf(fp,"\t\t\tsscanf(entries[i].val,\"%%u\",&%s);\n",cField);
			break;
			
			case COLTYPE_UNIXTIME:
			case COLTYPE_UNIXTIMECREATE:
			case COLTYPE_UNIXTIMEUPDATE:
			case COLTYPE_BIGINT:
				fprintf(fp,"\t\t\tsscanf(entries[i].val,\"%%lu\",&%s);\n",cField);
			break;

		}

		//Extra	stuff
		switch(uRADType)
		{
			case COLTYPE_SELECTTABLE:
			case COLTYPE_SELECTTABLE_OWNER:
				if((cp=strchr(field[6],',')))
				{
					*cp=0;
					sprintf(cTableName,"%.31s",field[6]);
					sprintf(cFieldName,"%.31s",cp+1);
					if((cp=strchr(cFieldName,',')))
						*cp=0;
				}
				fprintf(fp,"\t\t%sif(!strcmp(entries[i].name,\"c%sPullDown\"))\n\t\t{\n" ,temp ,cField);
				fprintf(fp,"\t\t\tsprintf(c%sPullDown,\"%%.255s\",entries[i].val);\n",cField);
				fprintf(fp,"\t\t\t%s=ReadPullDown(%s,%s,c%sPullDown);\n",cField,cTableName,cFieldName,cField);
				fprintf(fp,"\t\t}\n");
			break;
			
			case COLTYPE_YESNO:
				fprintf(fp,"\t\t%sif(!strcmp(entries[i].name,\"cYesNo%s\"))\n\t\t{\n" ,temp,cField);
				fprintf(fp,"\t\t\tsprintf(cYesNo%s,\"%%.31s\",entries[i].val);\n",cField);
				fprintf(fp,"\t\t\t%s=ReadYesNoPullDown(cYesNo%s);\n",cField,cField);
				fprintf(fp,"\t\t}\n");
			break;
		}

		temp=welse;
	}
	mysql_free_result(res);

}//void funcModuleProcVars(FILE *fp)


void funcModuleRAD3Input(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType,"
			" tField.uSQLSize,tField.uModLevel,"
			" tField.uHtmlXSize,tField.uHtmlMax,"
			" tField.cFKSpec,tField.uHtmlYSize"
			" FROM tField,tTable,tFieldType"
			" WHERE tField.uTable=tTable.uTable"
			" AND tField.uFieldType=tFieldType.uFieldType"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);

	unsigned uSQLSize=0;
	unsigned uHtmlMax=0;
	unsigned uHtmlXSize=0;
	unsigned uHtmlYSize=0;
	char *cWrap="hard";
	unsigned uRADType=0;
	unsigned uModLevel=0;
	char cTableName[32]={""};
	char cFieldName[32]={""};
	char cField[32]={""};
	char *cp;
	fprintf(fp,"\n");//Cancel out tab placed func
	while((field=mysql_fetch_row(res)))
	{
		sprintf(cField,"%.31s",field[0]);
		sscanf(field[1],"%u",&uRADType);
		sscanf(field[2],"%u",&uSQLSize);
		sscanf(field[3],"%u",&uModLevel);
		sscanf(field[4],"%u",&uHtmlXSize);
		sscanf(field[5],"%u",&uHtmlMax);
		sscanf(field[7],"%u",&uHtmlYSize);
		switch(uRADType)
		{
			default:
			case(COLTYPE_RADPRI):
                        case(COLTYPE_PRIKEY):
                        case(COLTYPE_INTUNSIGNED):
                        case(COLTYPE_UINTUKEY):
			case(COLTYPE_BIGINT):
				fprintf(fp,"\t//%s uRADType=%u\n",cField,uRADType);
				fprintf(fp,"\tOpenRow(LANG_FL_%s_%s,\"black\");\n",gcTableName,cField);
				if(uRADType==COLTYPE_BIGINT)
					fprintf(fp,"\tprintf(\"<input title='%%s' type=text name=uTable value=%%lu size=16 maxlength=10 \"\n");
				else
					fprintf(fp,"\tprintf(\"<input title='%%s' type=text name=uTable value=%%u size=16 maxlength=10 \"\n");
				fprintf(fp,"\t\t,LANG_FT_%s_%s,%s);\n",gcTableName,cField,cField);
				fprintf(fp,"\tif(guPermLevel>=%u && uMode)\n\t{\n",uModLevel);
				fprintf(fp,"\t\tprintf(\"></td></tr>\\n\");\n\t}\n\telse\n\t{\n");
				fprintf(fp,"\t\tprintf(\"disabled></td></tr>\\n\");\n");
				fprintf(fp,"\t\tprintf(\"<input type=hidden name=uTable value=%%u >\\n\",%s);\n\t}\n",cField);
			break;

			case(COLTYPE_DECIMAL):
                        case(COLTYPE_MONEY):
                        case(COLTYPE_CHAR):
                        case(COLTYPE_VARCHARUKEY):
			case(COLTYPE_VARCHAR):
				fprintf(fp,"\t//%s uRADType=%u\n",cField,uRADType);
				fprintf(fp,"\tOpenRow(LANG_FL_%s_%s,\"black\");\n",gcTableName,cField);
				fprintf(fp,"\tprintf(\"<input title='%%s' type=text name=uTable value=%%s size=%u maxlength=%u \"\n",
							uHtmlXSize,uHtmlMax);
				if(uRADType==COLTYPE_MONEY)
					fprintf(fp,"\t\t,LANG_FT_%s_%s,cMoneyDisplay(%s));\n",gcTableName,cField,cField);
				else
					fprintf(fp,"\t\t,LANG_FT_%s_%s,EncodeDoubleQuotes(%s));\n",gcTableName,cField,cField);
				fprintf(fp,"\tif(guPermLevel>=%u && uMode)\n\t{\n",uModLevel);
				fprintf(fp,"\t\tprintf(\"></td></tr>\\n\");\n\t}\n\telse\n\t{\n");
				fprintf(fp,"\t\tprintf(\"disabled></td></tr>\\n\");\n");
				fprintf(fp,"\t\tprintf(\"<input type=hidden name=uTable value='%%s'>\\n\",EncodeDoubleQuotes(%s));\n\t}\n",cField);
			break;

			case COLTYPE_DATETIME:
				fprintf(fp,"\t//%s COLTYPE_UNIXTIME\n",cField);
				fprintf(fp,"\tif(guPermLevel>=%u && uMode)\n",uModLevel);
				fprintf(fp,"\t\tjsCalendarInput(\"%s\",EncodeDoubleQuotes(%s),1);\n",cField,cField);
				fprintf(fp,"\telse\n");
				fprintf(fp,"\t\tjsCalendarInput(\"%s\",EncodeDoubleQuotes(%s),0);\n",cField,cField);
			break;

			case COLTYPE_UNIXTIME:
				fprintf(fp,"\t//%s COLTYPE_UNIXTIME\n",cField);
				fprintf(fp,"\tif(%s)\n",cField);
				fprintf(fp,"\t\tprintf(\"<input type=text name=c%s value='%%s' disabled>\\n\",ctime(&%s));\n",cField,cField);
				fprintf(fp,"\telse\n");
				fprintf(fp,"\t\tprintf(\"<input type=text name=c%s value='---' disabled>\\n\");\n",cField);
				fprintf(fp,"\tprintf(\"<input type=hidden name=%s value=%%lu>\\n\",%s);\n",cField,cField);	
			break;
			case COLTYPE_UNIXTIMECREATE:
			case COLTYPE_UNIXTIMEUPDATE:
				fprintf(fp,"\t//%s COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE\n",cField);
				fprintf(fp,"\tif(%s)\n",cField);
				fprintf(fp,"\t\tprintf(\"%%s\\n\\n\",ctime(&%s));\n",cField);
				fprintf(fp,"\telse\n");
				fprintf(fp,"\t\tprintf(\"---\\n\\n\");\n");
				fprintf(fp,"\tprintf(\"<input type=hidden name=%s value=%%lu >\\n\",%s);\n",cField,cField);
			break;

			case COLTYPE_FOREIGNKEY:
				fprintf(fp,"\t//%s COLTYPE_FOREIGNKEY\n",cField);
				if(uModLevel<=12)
				{
					fprintf(fp,"\tif(guPermLevel>=%u && uMode)\n",uModLevel);
					fprintf(fp,"\t\tprintf(\"<!--FK AllowMod-->\\n"
					"<input title='%%s' type=text size=16 maxlength=20 name=%s value=%%u >\\n\",LANG_FT_%s_%s,%s);\n"
							,cField,gcTableName,cField,cField);
				}
				else
					fprintf(fp,"\tprintf(\"%%s<input type=hidden name=%s value=%%u >\\n"
					"\",ForeignKey(%s),%s);\n",cField,field[6],cField);
				if(uModLevel<=12)
				{
					fprintf(fp,"\telse\n");
					fprintf(fp,"\t\tprintf(\"<input title='%%s' type=text value='%%s' size=%u disabled>"
					"<input type=hidden name='%s' value=%%u >\\n\",LANG_FT_%s_%s,"
					"ForeignKey(%s),%s);\n"
						,uHtmlXSize,
						cField,gcTableName,cField,
						field[6],cField);
				}
			break;//COLTYPE_FOREIGNKEY

			case COLTYPE_EXTFUNC:
				fprintf(fp,"\t//%s COLTYPE_EXTFUNC\n",cField);
			//function must exist in some .h or ext file see project
			//void {{field6}}(void)
				fprintf(fp,"\t%s(%s);\n",field[6],cField);
			break;

			case COLTYPE_YESNO:
				fprintf(fp,"\t//%s COLTYPE_YESNO\n",cField);
				fprintf(fp,"\tif(guPermLevel>=%u && uMode)\n",uModLevel);
				fprintf(fp,"\t\tYesNoPullDown(\"%s\",%s,1);\n",cField,cField);
				fprintf(fp,"\telse\n");
				fprintf(fp,"\t\tYesNoPullDown(\"%s\",%s,0);\n",cField,cField);
			break;

			//Text Area
			case COLTYPE_TEXT:
				fprintf(fp,"\t//%s COLTYPE_TEXT\n",cField);
				if(!uHtmlXSize) uHtmlXSize=80;
				if(!uHtmlYSize) uHtmlYSize=16;

				if(strstr(field[6],"textarea.wrap=off") || !strcmp(cField,"cTemplate"))
					cWrap="off";
				if(strstr(field[6],"textarea.wrap=soft"))
					cWrap="soft";

				fprintf(fp,"\tprintf(\"<textarea title='%%s' cols=%u wrap=%s rows=%u name=%s \"\n,LANG_FT_%s_%s);\n",
					uHtmlXSize,cWrap,uHtmlYSize,cField,gcTableName,cField);
				fprintf(fp,"\tif(guPermLevel>=%u && uMode)\n",uModLevel);
				fprintf(fp,"\t{\n");
				fprintf(fp,"\t\tprintf(\">%%s</textarea></td></tr>\\n\",%s);\n",cField);
				fprintf(fp,"\t}\n");
				fprintf(fp,"\telse\n");
				fprintf(fp,"\t{\n");
				fprintf(fp,"\t\tprintf(\"disabled>%%s</textarea></td></tr>\\n\",%s);\n",cField);
				fprintf(fp,"\t\tprintf(\"<input type=hidden name=%s value=\\\"%%s\\\" >\\n\",EncodeDoubleQuotes(%s));\n",
					cField,cField);
				fprintf(fp,"\t}\n");

			break;
			
			case COLTYPE_SELECTTABLE:
				fprintf(fp,"\t//%s COLTYPE_SELECTTABLE\n",cField);
				if((cp=strchr(field[6],',')))
				{
					char *cp2=NULL;

					*cp=0;
					sprintf(cTableName,"%.31s",field[6]);
					if((cp2=strchr(cp+1,',')))
						*cp2=0;
					sprintf(cFieldName,"%.31s",cp+1);
					*cp=',';
					if(cp2) *cp2=',';
					StripQuotes(cFieldName);
					StripQuotes(cTableName);
				}
				fprintf(fp,"\tif(guPermLevel>=%u && uMode)\n",uModLevel);
				fprintf(fp,"\t\ttTablePullDown(\"%s;c%sPullDown\",\"%s\",\"%s\",%s,1);\n"
					,cTableName,cField,cFieldName,cFieldName,cField);
				fprintf(fp,"\telse\n");
				fprintf(fp,"\t\ttTablePullDown(\"%s;c%sPullDown\",\"%s\",\"%s\",%s,0);\n"
					,cTableName,cField,cFieldName,cFieldName,cField);
			break;

			case COLTYPE_SELECTTABLE_OWNER:
				fprintf(fp,"\t//%s COLTYPE_SELECTTABLE_OWNER\n",cField);
				if((cp=strchr(field[6],',')))
				{
					char *cp2=NULL;

					*cp=0;
					sprintf(cTableName,"%.31s",field[6]);
					if((cp2=strchr(cp+1,',')))
						*cp2=0;
					sprintf(cFieldName,"%.31s",cp+1);
					*cp=',';
					if(cp2) *cp2=',';
					StripQuotes(cFieldName);
					StripQuotes(cTableName);
				}
				fprintf(fp,"\tif(guPermLevel>=%u && guPermLevel<10 && uMode)\n",uModLevel);
				fprintf(fp,"\t\ttTablePullDownOwner(\"%s;c%sPullDown\",\"%s\",\"%s\",%s,1);\n"
					,cTableName,cField,cFieldName,cFieldName,cField);
				fprintf(fp,"\telse if(guPermLevel<10 && !uMode)\n");
				fprintf(fp,"\t\ttTablePullDownOwner(\"%s;c%sPullDown\",\"%s\",\"%s\",%s,0);\n"
					,cTableName,cField,cFieldName,cFieldName,cField);
				fprintf(fp,"\telse if(uMode)\n");
				fprintf(fp,"\tprintf(\"<input title='%%s' type=text size=20 maxlength=20 name=%s value=%%u >\\n\",LANG_FT_%s_%s,%s);\n"
					,cField,gcTableName,cField,cField);
				//We don't want admin or root users to be clobbered by a giant select
				//so we use a modifiable FK
				fprintf(fp,"\telse if(1)\n\t{\n");
				fprintf(fp,"\t\tprintf(\"<input type=text size=20 value='%%s' disabled>\\n\",ForeignKey(\"%s\",\"%s\",%s));\n"
					,cTableName,cFieldName,cField);
				fprintf(fp,"\t\tprintf(\"<input type=hidden size=20 maxlength=20 name=%s value=%%u >\\n\",%s);\n"
					,cField,cField);
				fprintf(fp,"\t}\n");
			break;

		}//switch

	}//while

}//void funcModuleRAD3Input(FILE *fp)


void funcModuleVars(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType,"
			" tField.uSQLSize,tField.cFormDefault,tField.cFKSpec"
			" FROM tField,tTable,tFieldType"
			" WHERE tField.uTable=tTable.uTable"
			" AND tField.uFieldType=tFieldType.uFieldType"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFieldType=0;
	unsigned uFieldSize=0;
	unsigned uDefault=0;
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uFieldType);
		sscanf(field[2],"%u",&uFieldSize);
		switch(uFieldType)
		{
			default:
			case COLTYPE_CHAR:
			case COLTYPE_VARCHAR:
			case COLTYPE_VARCHARUKEY:
			fprintf(fp,"static char %s[%u]={\"%s\"};\n",
					field[0],uFieldSize+1,field[3]);
			break;

			case COLTYPE_MONEY:
			if(field[3][0])
				fprintf(fp,"static char %s[32]={\"%s\"};\n",
					field[0],field[3]);
			else
				fprintf(fp,"static char %s[32]={\"0.00\"};\n",field[0]);
			break;

			case COLTYPE_DECIMAL:
			if(field[3][0])
				fprintf(fp,"static char %s[16]={\"%s\"};\n",
					field[0],field[3]);
			else
				fprintf(fp,"static char %s[16]={\"0.00\"};\n",field[0]);
			break;

			case COLTYPE_DATETIME:
			if(field[3][0])
				fprintf(fp,"static char %s[20]={\"%s\"};\n",
					field[0],field[3]);
			else
				fprintf(fp,"static char %s[20]={\"2000-01-01 00:00:00\"};\n",field[0]);
			break;

			case COLTYPE_TIMESTAMP:
				fprintf(fp,"static char %s[20]={\"Never modified\"};\n",
					field[0]);
			break;

			case COLTYPE_TEXT:
				fprintf(fp,"static char *%s={\"%s\"};\n",
					field[0],field[3]);
			break;

			case COLTYPE_IMAGE:
				//skip
			break;
			
			
			//Special
			case COLTYPE_RADPRI:
			case COLTYPE_YESNO:
			case COLTYPE_PRIKEY:
			case COLTYPE_INTUNSIGNED:
			case COLTYPE_SELECTTABLE:
			case COLTYPE_SELECTTABLE_OWNER:
			case COLTYPE_EXTFUNC:
			case COLTYPE_FOREIGNKEY:
			case COLTYPE_FKIMAGE:
			case COLTYPE_UINTUKEY:

			sscanf(field[3],"%u",&uDefault);
			fprintf(fp,"static unsigned %s=%u;\n",
					field[0],uDefault);
			if(uFieldType==COLTYPE_SELECTTABLE || uFieldType==COLTYPE_SELECTTABLE_OWNER)
			{
				fprintf(fp,"static char c%sPullDown[256]={\"\"};\n",field[0]);
			}
			else if(uFieldType==COLTYPE_YESNO)
				fprintf(fp,"static char cYesNo%s[32]={\"\"};\n",
						field[0]);
			else if(uFieldType==COLTYPE_EXTFUNC)
				//Sneak in a prototype
				fprintf(fp,"void %s(unsigned %s);\n",
						field[4],field[0]);
			break;

			case COLTYPE_UNIXTIME:
			case COLTYPE_UNIXTIMECREATE:
			case COLTYPE_UNIXTIMEUPDATE:
				fprintf(fp,"static time_t %s=0;\n",
					field[0]);
			break;

			case COLTYPE_BIGINT:
				fprintf(fp,"static long unsigned %s=0;\n",
					field[0]);
			break;
		}
	}
	mysql_free_result(res);

}//void funcModuleVars(FILE *fp)


void funcModuleVarList(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel"
			" FROM tField,tTable"
			" WHERE tField.uTable=tTable.uTable"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=0;
	while((field=mysql_fetch_row(res)))
	{
		if(uFirst) fprintf(fp,",");
		fprintf(fp,"%s.%s",gcTableName,field[0]);
		uFirst=1;
	}
	mysql_free_result(res);

}//void funcModuleVarList(FILE *fp)


void funcModuleUpdateQuery(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	fprintf(fp,"sprintf(gcQuery,\"UPDATE %s SET\"\n",gcTableName);

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType"
			" FROM tField,tTable,tFieldType"
			" WHERE tField.uTable=tTable.uTable"
			" AND tField.uFieldType=tFieldType.uFieldType"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=0;
	unsigned uRADType=0;
	while((field=mysql_fetch_row(res)))
	{
		//Special internal fields
		if(!strcmp(field[0],"uCreatedBy"))
			continue;
		if(!strcmp(field[0],"uCreatedDate"))
			continue;
		if(uFirst)
			fprintf(fp,",\"\n");
		sscanf(field[1],"%u",&uRADType);
		switch(uRADType)
		{
			default:
			case(COLTYPE_RADPRI):
				fprintf(fp,"\t\t\"%s=%%u",field[0]);
			break;

			case COLTYPE_CHAR:
			case COLTYPE_DATETIME:
			case COLTYPE_TIMESTAMP:
			case COLTYPE_VARCHAR:
			case COLTYPE_VARCHARUKEY:
			case(COLTYPE_TEXT):
				fprintf(fp,"\t\t\"%s='%%s'",field[0]);
			break;

			case(COLTYPE_UNIXTIMEUPDATE):
				fprintf(fp,"\t\t\"%s=UNIX_TIMESTAMP(NOW())",field[0]);
			break;
		}
		uFirst=1;
	}

	fprintf(fp,"\"\n");
	uRADType=0;
	mysql_data_seek(res,0);
	while((field=mysql_fetch_row(res)))
	{
		//Special internal fields
		if(!strcmp(field[0],"uCreatedBy"))
			continue;
		if(!strcmp(field[0],"uCreatedDate"))
			continue;
		sscanf(field[1],"%u",&uRADType);
		switch(uRADType)
		{
			default:
			case(COLTYPE_RADPRI):
				fprintf(fp,"\t\t\t,%s\n",field[0]);
			break;

			case COLTYPE_CHAR:
			case COLTYPE_DATETIME:
			case COLTYPE_TIMESTAMP:
			case COLTYPE_VARCHAR:
			case COLTYPE_VARCHARUKEY:
			case(COLTYPE_TEXT):
				fprintf(fp,"\t\t\t,TextAreaSave(%s)\n",field[0]);
			break;

			case(COLTYPE_UNIXTIMEUPDATE):
			break;
		}
	}
	mysql_free_result(res);

	fprintf(fp,"\t\t);\n");
}//void funcModuleUpdateQuery(FILE *fp)


void funcModuleInsertQuery(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	fprintf(fp,"sprintf(gcQuery,\"INSERT INTO %s SET\"\n",gcTableName);

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType"
			" FROM tField,tTable,tFieldType"
			" WHERE tField.uTable=tTable.uTable"
			" AND tField.uFieldType=tFieldType.uFieldType"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=0;
	unsigned uRADType=0;
	while((field=mysql_fetch_row(res)))
	{
		//Special internal fields
		if(!strcmp(field[0],"uModBy"))
			continue;
		if(!strcmp(field[0],"uModDate"))
			continue;
		if(uFirst)
			fprintf(fp,",\"\n");
		sscanf(field[1],"%u",&uRADType);
		switch(uRADType)
		{
			default:
			case(COLTYPE_RADPRI):
				fprintf(fp,"\t\t\"%s=%%u",field[0]);
			break;

			case COLTYPE_CHAR:
			case COLTYPE_DATETIME:
			case COLTYPE_TIMESTAMP:
			case COLTYPE_VARCHAR:
			case COLTYPE_VARCHARUKEY:
			case(COLTYPE_TEXT):
				fprintf(fp,"\t\t\"%s='%%s'",field[0]);
			break;

			case(COLTYPE_UNIXTIMECREATE):
				fprintf(fp,"\t\t\"%s=UNIX_TIMESTAMP(NOW())",field[0]);
			break;
		}
		uFirst=1;
	}

	fprintf(fp,"\"\n");
	uRADType=0;
	mysql_data_seek(res,0);
	while((field=mysql_fetch_row(res)))
	{
		//Special internal fields
		if(!strcmp(field[0],"uModBy"))
			continue;
		if(!strcmp(field[0],"uModDate"))
			continue;
		sscanf(field[1],"%u",&uRADType);
		switch(uRADType)
		{
			default:
			case(COLTYPE_RADPRI):
				fprintf(fp,"\t\t\t,%s\n",field[0]);
			break;
			case COLTYPE_CHAR:
			case COLTYPE_DATETIME:
			case COLTYPE_TIMESTAMP:
			case COLTYPE_VARCHAR:
			case COLTYPE_VARCHARUKEY:
			case(COLTYPE_TEXT):
				fprintf(fp,"\t\t\t,TextAreaSave(%s)\n",field[0]);
			break;
			case(COLTYPE_UNIXTIMECREATE):
			break;
		}
	}
	mysql_free_result(res);

	fprintf(fp,"\t\t);\n");

}//void funcModuleInsertQuery(FILE *fp)


void funcModuleCreateQuery(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	fprintf(fp,"void Create%s(void)\n{\n",gcTableName);
	fprintf(fp,"\tsprintf(gcQuery,\"CREATE TABLE IF NOT EXISTS %s (\"\n",gcTableName);

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.cSQLCreatePart,"
			"tFieldType.uRADType,tField.uSQLSize,"
			"tField.uIndexType,tField.cExtIndex"
			" FROM tField,tTable,tFieldType"
			" WHERE tField.uTable=tTable.uTable"
			" AND tField.uFieldType=tFieldType.uFieldType"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=0;
	unsigned uRADType=0;
	unsigned uIndexType=0;
	while((field=mysql_fetch_row(res)))
	{
		if(uFirst)
			fprintf(fp,",\"\n");
		sscanf(field[2],"%u",&uRADType);
		sscanf(field[4],"%u",&uIndexType);
		switch(uRADType)
		{
			default:
			case(COLTYPE_RADPRI):
				fprintf(fp,"\t\t\"%s INT UNSIGNED %s",field[0],field[1]);
			break;
			case(COLTYPE_VARCHAR):
				fprintf(fp,"\t\t\"%s VARCHAR(%s) %s",field[0],field[3],field[1]);
			break;
		}
		switch(uIndexType)
		{
			case(1):
				fprintf(fp,", INDEX (%s)",field[0]);
			break;
			case(2):
				fprintf(fp,", UNIQUE (%s)",field[0]);
			break;
			default:
			break;
		}
		//cExtIndex
		if(field[5][0])
			fprintf(fp,", %s",field[5]);
		uFirst=1;
	}
	fprintf(fp," )\");\n");
	mysql_free_result(res);

	fprintf(fp,"\tmysql_query(&gMysql,gcQuery);\n");
	fprintf(fp,"\tif(mysql_errno(&gMysql))\n");
	fprintf(fp,"\t\thtmlPlainTextError(mysql_error(&gMysql));\n");

	fprintf(fp,"}//void Create%s(void)\n\n",gcTableName);

}//void funcModuleCreateQuery(FILE *fp)


//libtemplate.a required
void AppFunctions(FILE *fp,char *cFunction)
{
	if(!strcmp(cFunction,"funcModuleCreateQuery"))
		funcModuleCreateQuery(fp);
	else if(!strcmp(cFunction,"funcModuleInsertQuery"))
		funcModuleInsertQuery(fp);
	else if(!strcmp(cFunction,"funcModuleListPrint"))
		funcModuleListPrint(fp);
	else if(!strcmp(cFunction,"funcModuleListTable"))
		funcModuleListTable(fp);
	else if(!strcmp(cFunction,"funcModuleLoadVars"))
		funcModuleLoadVars(fp);
	else if(!strcmp(cFunction,"funcModuleProcVars"))
		funcModuleProcVars(fp);
	else if(!strcmp(cFunction,"funcModuleRAD3Input"))
		funcModuleRAD3Input(fp);
	else if(!strcmp(cFunction,"funcModuleUpdateQuery"))
		funcModuleUpdateQuery(fp);
	else if(!strcmp(cFunction,"funcModuleVars"))
		funcModuleVars(fp);
	else if(!strcmp(cFunction,"funcModuleVarList"))
		funcModuleVarList(fp);

}//void AppFunctions(FILE *fp,char *cFunction)


//Public domain snippet
void StripQuotes(char *cLine)
{
	register int j=0,i=0;
	for(i=0;cLine[i];i++)
	{
		if(cLine[i]!='"'&&cLine[i]!='\\')
		{ 
			cLine[j++] = cLine[i];
		}
		else if(cLine[i+1]== '"'&&cLine[i]=='\\')
		{ 
			cLine[j++] = '"';
		}
		else if(cLine[i+1]!='"'&&cLine[i]=='\\')
		{ 
			cLine[j++] = '\\';
		}
	}
	if(j>0) cLine[j]=0;
}//void StripQuotes(char *cLine)


const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

	static char gcQuery[256];

        sprintf(gcQuery,"SELECT %s FROM %s WHERE _rowid=%u",
                        cFieldName,cTableName,uKey);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql)) return(mysql_error(&gMysql));

        mysqlRes=mysql_store_result(&gMysql);
        if(mysql_num_rows(mysqlRes)==1)
        {
                mysqlField=mysql_fetch_row(mysqlRes);
                return(mysqlField[0]);
        }

	if(!uKey)
	{
        	return("---");
	}
	else
	{
		sprintf(gcQuery,"%u",uKey);
        	return(gcQuery);
	}

}//const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey)


char *WordToLower(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
	{
	
		if(!isalnum(cInput[i]) && cInput[i]!='_' && cInput[i]!='-'
				&& cInput[i]!='@' && cInput[i]!='.' ) break;
		if(isupper(cInput[i])) cInput[i]=tolower(cInput[i]);
	}
	cInput[i]=0;

	return(cInput);

}//char *WordToLower(char *cInput)

