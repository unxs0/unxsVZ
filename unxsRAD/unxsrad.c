/*
FILE 
	unxsrad.c
PURPOSE
	unxsRAD job queue CLI standalone program	
LEGAL
	(C) 2012-2017 Gary Wallis for Unixservice, LLC. All Rights Reserved.
	LICENSE file should be included in distribution.
OTHER
	TODO every single embeded mini templates (the printf's with code in them) 
	in this file can and should be eventually moved in to the 
	general tTemplate system.
HELP

*/

#include "unxsrad.h"
#include <openisp/template.h>

//Global vars
static unsigned guDebug=0;
MYSQL gMysql;
char gcQuery[8192]={""};
static char *sgcBuildInfo=dsGitVersion;
static FILE *gLfp=NULL;//log file
char gcHostname[100]={""};
unsigned guTable=0;
static char gcTableNameBS[32]={""};
static char gcTableName[32]={""};
static char gcTableNameLC[64]={""};
static char gcTableKey[33]={""};
static char gcProjectStatus[33]={""};
unsigned guProject=0;
unsigned guTemplateType=0;
char gcProject[32]={"Project"};
char gcProjectLC[32]={"project"};
char gcRADStatus[32]={"Unknown"};
char gcDirectory[256]={""};
char gcRADDataDir[100]={"/home/unxs/unxsVZ/unxsRAD/appdata/"};
char gcAppSummary[256]={"No application summary"};
char gcuJs[3]={"0"};

//prototype TOC
void ProcessJobQueue(void);
void MakeSourceCodeJob(unsigned uJob,char const *cJobData);
void logfileLine(const char *cFunction,const char *cLogline);
unsigned CreateFile(unsigned uTemplateSet,unsigned uTable,char *cTable,unsigned uSourceLock,unsigned uTemplateType);
unsigned CreateGenericFile(unsigned uTemplate,unsigned uTable,unsigned uSourceLock,char const *cFileName);
unsigned CreateFileFromTemplate(unsigned uTemplate,unsigned uTable);
void funcModuleListPrint(FILE *fp);
void funcBashEnvProject(FILE *fp);
void funcModuleListTable(FILE *fp);
void funcModuleLoadVars(FILE *fp);
void funcModuleProcVars(FILE *fp);
void funcModuleInput(FILE *fp);
void funcModuleVars(FILE *fp);
void funcModuleVarList(FILE *fp);
void funcModuleUpdateQuery(FILE *fp);
void funcModuleInsertQuery(FILE *fp);
void funcModuleCreateQuery(FILE *fp);
void AppFunctions(FILE *fp,char *cFunction);
void StripQuotes(char *cLine);
char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);
char *WordToLower(char *cInput);
void funcMakefileObjects(FILE *fp);
void funcMakefileRules(FILE *fp);
void funcModulePrototypes(FILE *fp);
void funcBootstrapModulePrototypes(FILE *fp);
void funcMainGetMenu(FILE *fp);
void funcBootstrapMainGetMenu(FILE *fp);
void funcMainNavBars(FILE *fp);
void funcMainPostFunctions(FILE *fp);
void funcBootstrapMainPostFunctions(FILE *fp);
void funcBSGetHookAdditionalGentries(FILE *fp);
void funcBSGetHookPrePages(FILE *fp);
void funcBSGetHookPages(FILE *fp);
void funcMainTabMenu(FILE *fp);
void funcMainInitTableList(FILE *fp);
void funcMainCreateTables(FILE *fp);
void funcModuleLanguage(FILE *fp);
void GetRADConfiguration(const char *cName,char *cValue,unsigned uValueSize, unsigned uServer);
void funcConfiguration(FILE *fp,char *cFunction);
void Template2(char *cTemplate, struct t_template *template, FILE *fp);
void funcBootstrapNavItems(FILE *fp);
void funcBootstrapFindFields(FILE *fp);
void funcBootstrapConcat(FILE *fp);
void funcBootstrapValueFields(FILE *fp);
void funcBootstrapEditorFields(FILE *fp);
//
void funcBootstrapRowVars(FILE *fp);
void funcBootstrapRowFields(FILE *fp);
void funcBootstrapRowFormats(FILE *fp);
//
void funcBootstrapRowReportVars(FILE *fp);
void funcBootstrapRowReportFields(FILE *fp);
void funcBootstrapRowReportFormats(FILE *fp);
//
void funcBootstrapCols(FILE *fp);
void funcBootstrapColsReport(FILE *fp);
//
void funcBSFKJsonRowsSelects(FILE *fp);
//
void funcBSTemplateFKNVPairs(FILE *fp);
//
void funcBootstrapRetEditor(FILE *fp);
//table.c Latest Linked Data
void funcBSLDTables(FILE *fp);
void funcBSLDContextVar1(FILE *fp);//uPaciente
void funcBSLDContextVar2(FILE *fp);//uConsulta


//external prototypes
void ConnectDb(MYSQL *spMysql);

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
	ConnectDb(&gMysql);

	if(iArgc==2)
	{
		if(!strcmp(cArgv[1],"ProcessJobQueue"))
			ProcessJobQueue();
		if(!strcmp(cArgv[1],"ProcessJobQueueDebug"))
		{
			guDebug=1;
			ProcessJobQueue();
		}
	}
	else if(1)
	{
		printf("usage: %s ProcessJobQueue|ProcessJobQueueDebug\n",cArgv[0]);
		printf("\tgcHostname: %s\n",gcHostname);
		printf("\tgit build info: %s\n",sgcBuildInfo);
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

	if(guDebug)
		logfileLine("ProcessJobQueue","debug mode on");

	//Explicit queue
	sprintf(gcQuery,"SELECT tJob.uJob,tJob.cJobName,tJob.cJobData FROM tJob,tServer"
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

	//Any server queue
	sprintf(gcQuery,"SELECT tJob.uJob,tJob.cJobName,tJob.cJobData FROM tJob"
			" WHERE tJob.uServer=0"
			" AND tJob.uJobStatus=1");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessJobQueue2",mysql_error(&gMysql));
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
	unsigned uProjectStatus=0;
	unsigned uTable=0;
	unsigned uSourceLock=0;
	unsigned uTemplateSet=0;
	unsigned uTemplateType=0;
	unsigned uOnce=1;

	logfileLine("MakeSourceCodeJob","start");

	sscanf(cJobData,"uProject=%u;",&uProject);
	guProject=uProject;
	sprintf(gcProject,"%.31s",ForeignKey("tProject","cLabel",guProject));

	sprintf(gcProjectLC,"%.31s",ForeignKey("tProject","cLabel",guProject));
	WordToLower(gcProjectLC);

	sprintf(gcAppSummary,"%.255s",ForeignKey("tProject","cDescription",guProject));

	char cDirectory[100]={""};
	sprintf(cDirectory,"%.99s",ForeignKey("tProject","cDirectory",guProject));
	if(cDirectory[0])
		sprintf(gcDirectory,"/var/local/unxsRAD/apps/%.99s",cDirectory);
	else
		sprintf(gcDirectory,"/var/local/unxsRAD/apps/%.99s",ForeignKey("tProject","cLabel",guProject));
	sprintf(gcQuery,"mkdir -p %s/data",gcDirectory);
	system(gcQuery);
	if(guDebug)
		logfileLine("MakeSourceCodeJob",gcQuery);

	sscanf(ForeignKey("tProject","uProjectStatus",guProject),"%u",&uProjectStatus);
	sprintf(gcRADStatus,"%.31s",ForeignKey("tProjectStatus","cLabel",uProjectStatus));

	//All tTables that do not have uSourceLock=yes
	//If uSourceLock==1 then if file does not exist we create if not we do not replace
	sprintf(gcQuery,"SELECT tProject.cLabel,tTable.cLabel,tTable.uTable,"
			"tProject.uTemplateSet,tTable.uSourceLock,"
			"tProjectStatus.cLabel,tTable.uTemplateType"
			" FROM tProject,tTable,tProjectStatus"
			" WHERE tTable.uProject=tProject.uProject AND"
			" tTable.uTemplateType!=0 AND"
			" tProject.uProjectStatus=tProjectStatus.uStatus AND"
			" tProject.uProject=%u",
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
		if(uOnce)
		{
			sscanf(field[3],"%u",&uTemplateSet);
			if(guDebug)
			{
				sprintf(gcQuery,"%s uTemplateSet:%u",field[0],uTemplateSet);
				logfileLine("MakeSourceCodeJob",gcQuery);
			}
			uOnce=0;
		}
		sscanf(field[2],"%u",&uTable);
		sscanf(field[4],"%u",&uSourceLock);
		sscanf(field[6],"%u",&uTemplateType);
		if(guDebug)
		{
			sprintf(gcQuery,"%s uTable:%u:%u",field[1],uTable,uSourceLock);
			logfileLine("MakeSourceCodeJob",gcQuery);
		}
		sprintf(gcProjectStatus,"%.32s",field[5]);
		if(CreateFile(uTemplateSet,uTable,field[1],uSourceLock,uTemplateType))
			break;
		else
			logfileLine("CreateFile","end");
	}
	mysql_free_result(res);

	//Add other files and dirs
	//GetRADConfiguration("gcRADDataDir",gcRADDataDir,100,0);
	//sprintf(gcQuery,"mkdir -p %s/data;cp %s/*.txt %s/data/",gcDirectory,gcRADDataDir,gcDirectory);
	//system(gcQuery);
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


unsigned CreateFile(unsigned uTemplateSet,unsigned uTable,char *cTable,unsigned uSourceLock,unsigned uTemplateType)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uRetVal=0;
	unsigned uTemplate=0;
	char cFileName[100]={""};
	char cTableName[100]={""};

	logfileLine("CreateFile","start");
	if(guDebug) logfileLine("CreateFile",cTable);

	sprintf(cTableName,"%.99s",cTable);
	WordToLower(cTable);

	//for funcXXXXX
	guTemplateType=uTemplateType;

	//Here we decide which template to use exact match 
	//else if using uTemplate != 0;
	//e.g. makefile, e.g. footable.bootstrap.min.css
	sprintf(gcQuery,"SELECT uTemplate,cLabel FROM tTemplate"
				" WHERE cLabel='%s'"
				" AND uTemplateSet=%u"
				" AND uTemplateType=%u"
				" AND uTemplateType!=0"
					,cTable,uTemplateSet,uTemplateType);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("CreateFile",mysql_error(&gMysql));
		return(-1);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(guDebug) logfileLine("CreateFile1",field[1]);
		sscanf(field[0],"%u",&uTemplate);
		if(uTemplate && uTable)
			uRetVal=CreateGenericFile(uTemplate,uTable,uSourceLock,field[1]);
	}
	mysql_free_result(res);

	if(uTemplate || uRetVal)
		return(uRetVal);

	//e.g. template based tConfiguration where template tconfiguration.c exists
	sprintf(gcQuery,"SELECT uTemplate,cLabel FROM tTemplate"
				" WHERE cLabel='%s.c'"
				" AND uTemplateSet=%u"
				" AND uTemplateType=%u"
				" AND uTemplateType!=0"
					,cTable,uTemplateSet,uTemplateType);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("CreateFile",mysql_error(&gMysql));
		return(-1);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(guDebug) logfileLine("CreateFile2",field[1]);
		sscanf(field[0],"%u",&uTemplate);
		if(uTemplate && uTable)
			uRetVal=CreateGenericFile(uTemplate,uTable,uSourceLock,field[1]);
	}
	mysql_free_result(res);

	//Do the func.h and the .c 
	if(uRetVal)
		return(uRetVal);

	//e.g. template based tClient where tclientfunc.h template exists
	sprintf(gcQuery,"SELECT uTemplate,cLabel FROM tTemplate"
				" WHERE cLabel='%sfunc.h'"
				" AND uTemplateSet=%u"
				" AND uTemplateType=%u"
				" AND uTemplateType!=0"
					,cTable,uTemplateSet,uTemplateType);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessJobQueue",mysql_error(&gMysql));
		return(-1);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		logfileLine("CreateFile5",field[1]);
		sscanf(field[0],"%u",&uTemplate);
		if(uTemplate && uTable)
			uRetVal=CreateGenericFile(uTemplate,uTable,uSourceLock,field[1]);
	}
	mysql_free_result(res);

	if(uTemplate || uRetVal)
		return(uRetVal);

	//No specific template found for table use module.c and modulefunc.h
	sprintf(gcQuery,"SELECT uTemplate FROM tTemplate"
				" WHERE cLabel='modulefunc.h'"
				" AND uTemplateSet=%u"
				" AND uTemplateType=%u"
				" AND uTemplateType!=0"
					,uTemplateSet,uTemplateType);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessJobQueue",mysql_error(&gMysql));
		return(-1);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cFileName,"%.93sfunc.h",cTable);
		if(guDebug) logfileLine("CreateFile3",cFileName);
		sscanf(field[0],"%u",&uTemplate);
		if(uTemplate && uTable)
			uRetVal=CreateGenericFile(uTemplate,uTable,uSourceLock,cFileName);
	}
	mysql_free_result(res);

	//Do the func.h and the .c 
	if(uRetVal)
		return(uRetVal);

	if(!strstr(cTableName,".Body"))
	{
		sprintf(gcQuery,"SELECT uTemplate FROM tTemplate"
				" WHERE (cLabel='module.c' OR cLabel='table.c')"
				" AND uTemplateSet=%u"
				" AND uTemplateType=%u"
				" AND uTemplateType!=0"
					,uTemplateSet,uTemplateType);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessJobQueue",mysql_error(&gMysql));
			return(-1);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(cFileName,"%.97s.c",cTable);
			if(guDebug) logfileLine("CreateFile4",cFileName);
			sscanf(field[0],"%u",&uTemplate);
			if(uTemplate && uTable)
				uRetVal=CreateGenericFile(uTemplate,uTable,uSourceLock,cFileName);
		}
	}
	else
	{
		//interface templates Template2 tTable.Body
		sprintf(gcQuery,"SELECT uTemplate FROM tTemplate"
				" WHERE cLabel='tTable.Body'"
				" AND uTemplateSet=%u"
				" AND uTemplateType=%u"
				" AND uTemplateType!=0"
					,uTemplateSet,uTemplateType);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessJobQueue",mysql_error(&gMysql));
			return(-1);
		}
        	res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(cFileName,"%.99s",cTableName);
			if(guDebug) logfileLine("CreateFile5",cFileName);
			sscanf(field[0],"%u",&uTemplate);
			if(uTemplate && uTable)
				uRetVal=CreateGenericFile(uTemplate,uTable,uSourceLock,cFileName);
		}
		//interface templates Template2 tTableReport.Body
		sprintf(gcQuery,"SELECT uTemplate FROM tTemplate"
				" WHERE cLabel='tTableReport.Body'"
				" AND uTemplateSet=%u"
				" AND uTemplateType=%u"
				" AND uTemplateType!=0"
					,uTemplateSet,uTemplateType);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessJobQueue",mysql_error(&gMysql));
			return(-1);
		}
        	res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			char *cp;
			if((cp=strstr(cTableName,".Body")))
			{
				*cp=0;
				sprintf(cFileName,"%.99sReport.Body",cTableName);
				*cp='.';
			}
			else
			{
				sprintf(cFileName,"%.99s",cTableName);
			}
			if(guDebug) logfileLine("CreateFile5",cFileName);
			sscanf(field[0],"%u",&uTemplate);
			if(uTemplate && uTable)
				uRetVal=CreateGenericFile(uTemplate,uTable,uSourceLock,cFileName);
		}
		//interface templates Template2 tTableFilter.Body
		sprintf(gcQuery,"SELECT uTemplate FROM tTemplate"
				" WHERE cLabel='tTableFilter.Body'"
				" AND uTemplateSet=%u"
				" AND uTemplateType=%u"
				" AND uTemplateType!=0"
					,uTemplateSet,uTemplateType);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessJobQueue",mysql_error(&gMysql));
			return(-1);
		}
        	res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			char *cp;
			if((cp=strstr(cTableName,".Body")))
			{
				*cp=0;
				sprintf(cFileName,"%.99sFilter.Body",cTableName);
				*cp='.';
			}
			else
			{
				sprintf(cFileName,"%.99s",cTableName);
			}
			if(guDebug) logfileLine("CreateFile5",cFileName);
			sscanf(field[0],"%u",&uTemplate);
			if(uTemplate && uTable)
				uRetVal=CreateGenericFile(uTemplate,uTable,uSourceLock,cFileName);
		}
	}
	mysql_free_result(res);
	if(uTemplate || uRetVal)
		return(uRetVal);

	logfileLine("CreateFile","no template found for given table!");
	return(uRetVal);

}//unsigned CreateFile()


unsigned CreateGenericFile(unsigned uTemplate,unsigned uTable,unsigned uSourceLock,char const *cFileName)
{
	unsigned uRetVal= -1;
	logfileLine("CreateGenericFile",cFileName);

	if(!uTable || !uTemplate) return(uRetVal);

       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cTemplate,uTemplateType,cLabel FROM tTemplate WHERE uTemplate=%u",uTemplate);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		logfileLine("CreateGenericFile",mysql_error(&gMysql));
		return(uRetVal);
	}
	res=mysql_store_result(&gMysql);
	unsigned uTemplateType=0;
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uTemplateType);
		struct t_template template;

		char cSubDir[101]={""};
		sprintf(cSubDir,"%.100s",ForeignKey("tTable","cSubDir",uTable));

		sprintf(gcTableName,"%.31s",ForeignKey("tTable","cLabel",uTable));
		sprintf(gcTableNameLC,"%.63s",gcTableName);
		WordToLower(gcTableNameLC);
		guTable=uTable;


		FILE *fp=NULL;
		char cFile[512]={""};
		if(cSubDir[0])
		{
			sprintf(gcQuery,"mkdir -p %s/%s",gcDirectory,cSubDir);
			if(system(gcQuery))
				logfileLine("CreateGenericFile",gcQuery);
			sprintf(cFile,"%.200s/%.100s/%.200s",gcDirectory,cSubDir,cFileName);
		}
		else
		{
			sprintf(cFile,"%.200s/%.200s",gcDirectory,cFileName);
		}

		if(uSourceLock)
		{
			if((fp=fopen(cFile,"r"))!=NULL)
			{
				logfileLine("CreateGenericFile","uSourceLock==1 not replacing");
				mysql_free_result(res);
				return(0);
			}
		}
		if((fp=fopen(cFile,"w"))==NULL)
		{
			logfileLine("CreateGenericFile",cFile);
			mysql_free_result(res);
			return(1);
		}
		

		//Do not parse data templates. We need to pass on {{cVars/funcXXX}}.
		if(uTemplateType==uTEMPLATETYPE_DATA)
		{
			if(guDebug)
				logfileLine("CreateGenericFile","uTEMPLATETYPE_DATA not parsing");
			fprintf(fp,field[0]);
		}
		else
		{	
			template.cpName[0]="cProject";
			template.cpValue[0]=gcProject;
			
			template.cpName[1]="cProjectLC";
			template.cpValue[1]=gcProjectLC;
			
			template.cpName[2]="gcRADStatus";
			template.cpValue[2]=gcRADStatus;
			
			template.cpName[3]="gcAppSummary";
			char *cp;
			if((cp=strchr(gcAppSummary,'\n')))
				*cp=0;
			if((cp=strchr(gcAppSummary,'\r')))
				*cp=0;
			template.cpValue[3]=gcAppSummary;

			template.cpName[4]="cTableName";
			template.cpValue[4]=gcTableName;
			
			template.cpName[5]="cTableNameLC";
			template.cpValue[5]=gcTableNameLC;
		
			if(gcTableName[0]=='t')	
				sprintf(gcTableKey,"u%.31s",gcTableName+1);//New table name includes table type t prefix
			else
				sprintf(gcTableKey,"u%.31s",gcTableName);//New table name includes table type t prefix
			
			if((cp=strchr(gcTableKey,'.')))
				*cp=0;
			template.cpName[6]="cTableKey";
			template.cpValue[6]=gcTableKey;
	
			char cTableTitle[32]={"Title"};
			sprintf(cTableTitle,"%.31s",ForeignKey("tTable","cLegend",uTable));
			template.cpName[7]="cTableTitle";
			template.cpValue[7]=cTableTitle;
				
			char cToolTip[256]={"cToolTip"};
			sprintf(cToolTip,"%.255s",ForeignKey("tTable","cToolTip",uTable));
			template.cpName[8]="cToolTip";
			template.cpValue[8]=cToolTip;
	
			template.cpName[9]="sgcBuildInfo";
			template.cpValue[9]=sgcBuildInfo;
	
			template.cpName[10]="gcProjectStatus";
			template.cpValue[10]=gcProjectStatus;
	
			template.cpName[11]="uJs";
			template.cpValue[11]=gcuJs;
	
			template.cpName[12]="cTable";
			char cTable[100]={""};
			sprintf(cTable,"%.99s",gcTableName);
			if((cp=strchr(cTable,'.')))
				*cp=0;
			template.cpValue[12]=cTable;
			
			sprintf(gcTableNameBS,"%.31s",gcTableName+1);//New table name includes table type t prefix
			template.cpName[13]="cTableNameBS";
			template.cpValue[13]=gcTableNameBS;
	
			template.cpName[14]="cTemplateLabel";
			template.cpValue[14]=field[2];
	
			template.cpName[15]="";

			//Place special keyword in template file at top
			//to use [[Vars]] instead of {{Vars}}
			//this is useful for parsing templates that are templates
			//themselves
			if(strstr(field[0],"unxsRAD.Template2"))
			{
				logfileLine("CreateGenericFile","Template2");
				Template2(field[0],&template,fp);
			}
			else
			{
				Template(field[0],&template,fp);
			}
		}

		fclose(fp);
		uRetVal=0;
	}
	else
	{
		logfileLine("CreateGenericFile","No such uTemplate");
		uRetVal=2;
	}
	mysql_free_result(res);

	logfileLine("CreateGenericFile","end");
	return(uRetVal);

}//unsigned CreateGenericFile()


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
		else if(uFieldType==COLTYPE_FOREIGNKEY ||
				uFieldType==COLTYPE_SELECTTABLE ||
				uFieldType==COLTYPE_SELECTTABLE_OWNER )//Index into another table
		{
			char cTableName[32]={""};
			char cTableNameKey[32]={""};
			char cFieldName[32]={""};
			char *cp;
			//cFKSpec
			if((cp=strchr(field[2],',')))
			{
				*cp=0;
				sprintf(cTableName,"%.31s",field[2]);
				StripQuotes(cTableName);
				sprintf(cTableNameKey,"u%.30s",cTableName+1);//RAD4 primary key standard naming convention
				sprintf(cFieldName,"%.31s",cp+1);
				if((cp=strchr(cFieldName,',')))
					*cp=0;
				StripQuotes(cFieldName);
			}
			fprintf(fp,"\t\tchar cBuf%d[128];\n",i);
			fprintf(fp,"\t\tsprintf(cBuf%d,\"<a class=darkLink href=?gcFunction=%s&%s=%%.32s>%%.32s</a>\",\n",i,cTableName,cTableNameKey);
			fprintf(fp,"\t\t\tfield[%d],\n",i);
			fprintf(fp,"\t\t\tForeignKey(\"%s\",\"%s\",strtoul(field[%d],NULL,10)));\n",cTableName,cFieldName,i);
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
			fprintf(fp,"<td><a class=darkLink href=?gcFunction=%s&%s=%%s>%%s</a>",gcTableName,field[1]);
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

		if(uFieldType == COLTYPE_UNIXTIME ||
				uFieldType==COLTYPE_FOREIGNKEY ||
				uFieldType==COLTYPE_SELECTTABLE ||
				uFieldType==COLTYPE_SELECTTABLE_OWNER ||
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

}//void funcModuleListPrint(FILE *fp)


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

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType,tField.uSQLSize"
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
	unsigned uRADType=0;
	char cField[32]={""};
	fprintf(fp,"\n");//Cancel out tab placed func
	while((field=mysql_fetch_row(res)))
	{
		sprintf(cField,"%.31s",field[0]);
		sscanf(field[1],"%u",&uRADType);
		sscanf(field[2],"%u",&uSQLSize);

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
					,cField,uSQLSize,i);
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
			case COLTYPE_DATEEUR:
			case COLTYPE_TIMESTAMP:
			case COLTYPE_VARCHAR:
			case COLTYPE_VARCHARUKEY:
				//cExtIndex can be used for input validation functions. Fix this TODO
				if(field[8][0])
					fprintf(fp,"\t\t\tsprintf(%s,\"%%.%us\",%s(entries[i].val));\n",cField,uSQLSize,field[8]);
				else
					fprintf(fp,"\t\t\tsprintf(%s,\"%%.%us\",entries[i].val);\n",cField,uSQLSize);
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
				fprintf(fp,"\t\t\tsscanf(entries[i].val,\"%%u\",&%s);\n",cField);
			break;

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
				fprintf(fp,"\t\t\n\t\t{\n");
				fprintf(fp,"\t\t\tsscanf(entries[i].val,\"%%u\",&%s);\n",cField);
				fprintf(fp,"\t\t\tif(!%s)\n",cField);
				fprintf(fp,"\t\t\t\t%s=ReadPullDown(%s,%s,entries[i].val);\n",cField,cTableName,cFieldName);
				fprintf(fp,"\t\t}\n");
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


void funcModuleInput(FILE *fp)
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
					fprintf(fp,"\tprintf(\"<input title='%%s' type=text name=%s value='%%lu' size=16 maxlength=10 \"\n",cField);
				else
					fprintf(fp,"\tprintf(\"<input title='%%s' type=text name=%s value='%%u' size=16 maxlength=10 \"\n",cField);
				fprintf(fp,"\t\t,LANG_FT_%s_%s,%s);\n",gcTableName,cField,cField);
				fprintf(fp,"\tif(guPermLevel>=%u && uMode)\n\t{\n",uModLevel);
				fprintf(fp,"\t\tprintf(\"></td></tr>\\n\");\n\t}\n\telse\n\t{\n");
				fprintf(fp,"\t\tprintf(\"disabled></td></tr>\\n\");\n");
				fprintf(fp,"\t\tprintf(\"<input type=hidden name=%s value='%%u' >\\n\",%s);\n\t}\n",cField,cField);
			break;

			case(COLTYPE_DECIMAL):
                        case(COLTYPE_MONEY):
                        case(COLTYPE_CHAR):
                        case(COLTYPE_VARCHARUKEY):
			case(COLTYPE_VARCHAR):
				fprintf(fp,"\t//%s uRADType=%u\n",cField,uRADType);
				fprintf(fp,"\tOpenRow(LANG_FL_%s_%s,\"black\");\n",gcTableName,cField);
				fprintf(fp,"\tprintf(\"<input title='%%s' type=text name=%s value='%%s' size=%u maxlength=%u \"\n",
							cField,uHtmlXSize,uHtmlMax);
				if(uRADType==COLTYPE_MONEY)
					fprintf(fp,"\t\t,LANG_FT_%s_%s,cMoneyDisplay(%s));\n",gcTableName,cField,cField);
				else
					fprintf(fp,"\t\t,LANG_FT_%s_%s,EncodeDoubleQuotes(%s));\n",gcTableName,cField,cField);
				fprintf(fp,"\tif(guPermLevel>=%u && uMode)\n\t{\n",uModLevel);
				fprintf(fp,"\t\tprintf(\"></td></tr>\\n\");\n\t}\n\telse\n\t{\n");
				fprintf(fp,"\t\tprintf(\"disabled></td></tr>\\n\");\n");
				fprintf(fp,"\t\tprintf(\"<input type=hidden name=%s value='%%s'>\\n\",EncodeDoubleQuotes(%s));\n\t}\n",cField,cField);
			break;

			case COLTYPE_DATETIME:
				fprintf(fp,"\t//%s COLTYPE_DATETIME\n",cField);
				fprintf(fp,"\tOpenRow(LANG_FL_%s_%s,\"black\");\n",gcTableName,cField);
				fprintf(fp,"\tif(guPermLevel>=%u && uMode)\n\t{\n",uModLevel);
				fprintf(fp,"\t\tjsCalendarInput(\"%s\",EncodeDoubleQuotes(%s),1);\n\t}\n",cField,cField);
				fprintf(fp,"\telse\n\t{\n");
				fprintf(fp,"\t\tjsCalendarInput(\"%s\",EncodeDoubleQuotes(%s),0);\n",cField,cField);
				fprintf(fp,"\t\tprintf(\"<input type=hidden name=%s value='%%s'>\\n\",EncodeDoubleQuotes(%s));\n\t}\n",cField,cField);
			break;

			case COLTYPE_DATEEUR:
				fprintf(fp,"\t//%s COLTYPE_DATEEUR\n",cField);
				fprintf(fp,"\tOpenRow(LANG_FL_%s_%s,\"black\");\n",gcTableName,cField);
				fprintf(fp,"\tif(guPermLevel>=%u && uMode)\n\t{\n",uModLevel);
				fprintf(fp,"\t\tjsCalendarInput(\"%s\",EncodeDoubleQuotes(%s),1);\n\t}\n",cField,cField);
				fprintf(fp,"\telse\n\t{\n");
				fprintf(fp,"\t\tjsCalendarInput(\"%s\",EncodeDoubleQuotes(%s),0);\n",cField,cField);
				fprintf(fp,"\t\tprintf(\"<input type=hidden name=%s value='%%s'>\\n\",EncodeDoubleQuotes(%s));\n\t}\n",cField,cField);
			break;

			case COLTYPE_UNIXTIME:
				fprintf(fp,"\t//%s COLTYPE_UNIXTIME\n",cField);
				fprintf(fp,"\tOpenRow(LANG_FL_%s_%s,\"black\");\n",gcTableName,cField);
				fprintf(fp,"\tif(%s)\n",cField);
				fprintf(fp,"\t\tprintf(\"<input type=text name=c%s value='%%s' disabled>\\n\",ctime(&%s));\n",cField,cField);
				fprintf(fp,"\telse\n");
				fprintf(fp,"\t\tprintf(\"<input type=text name=c%s value='---' disabled>\\n\");\n",cField);
				fprintf(fp,"\tprintf(\"<input type=hidden name=%s value='%%lu'>\\n\",%s);\n",cField,cField);	
			break;
			case COLTYPE_UNIXTIMECREATE:
			case COLTYPE_UNIXTIMEUPDATE:
				fprintf(fp,"\t//%s COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE\n",cField);
				fprintf(fp,"\tOpenRow(LANG_FL_%s_%s,\"black\");\n",gcTableName,cField);
				fprintf(fp,"\tif(%s)\n",cField);
				fprintf(fp,"\t\tprintf(\"%%s\\n\\n\",ctime(&%s));\n",cField);
				fprintf(fp,"\telse\n");
				fprintf(fp,"\t\tprintf(\"---\\n\\n\");\n");
				fprintf(fp,"\tprintf(\"<input type=hidden name=%s value='%%lu' >\\n\",%s);\n",cField,cField);
			break;

			case COLTYPE_FOREIGNKEY:
				fprintf(fp,"\t//%s COLTYPE_FOREIGNKEY\n",cField);
				fprintf(fp,"\tOpenRow(LANG_FL_%s_%s,\"black\");\n",gcTableName,cField);
				if(uModLevel<=12)
				{
					fprintf(fp,"\tif(guPermLevel>=%u && uMode)\n",uModLevel);
					fprintf(fp,"\t\tprintf(\"<!--FK AllowMod-->\\n"
					"<input title='%%s' type=text size=16 maxlength=20 name=%s value='%%u' >\\n\",LANG_FT_%s_%s,%s);\n"
							,cField,gcTableName,cField,cField);
				}
				else
					fprintf(fp,"\tprintf(\"%%s<input type=hidden name=%s value='%%u' >\\n"
					"\",ForeignKey(%s),%s);\n",cField,field[6],cField);
				if(uModLevel<=12)
				{
					fprintf(fp,"\telse\n");
					fprintf(fp,"\t\tprintf(\"<input title='%%s' type=text value='%%s' size=%u disabled>"
					"<input type=hidden name='%s' value='%%u' >\\n\",LANG_FT_%s_%s,"
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
				fprintf(fp,"\tOpenRow(LANG_FL_%s_%s,\"black\");\n",gcTableName,cField);
				fprintf(fp,"\tif(guPermLevel>=%u && uMode)\n",uModLevel);
				fprintf(fp,"\t\tYesNoPullDown(\"%s\",%s,1);\n",cField,cField);
				fprintf(fp,"\telse\n");
				fprintf(fp,"\t\tYesNoPullDown(\"%s\",%s,0);\n",cField,cField);
			break;

			//Text Area
			case COLTYPE_TEXT:
				fprintf(fp,"\t//%s COLTYPE_TEXT\n",cField);
				fprintf(fp,"\tOpenRow(LANG_FL_%s_%s,\"black\");\n",gcTableName,cField);
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
				fprintf(fp,"\tOpenRow(LANG_FL_%s_%s,\"black\");\n",gcTableName,cField);
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
				fprintf(fp,"\tOpenRow(LANG_FL_%s_%s,\"black\");\n",gcTableName,cField);
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
				fprintf(fp,"\t\ttTablePullDown(\"%s;c%sPullDown\",\"%s\",\"%s\",%s,1);\n"
					,cTableName,cField,cFieldName,cFieldName,cField);
				//fprintf(fp,"\tprintf(\"<input title='%%s' type=text size=20 maxlength=20 name=%s value='%%u' >\\n\",LANG_FT_%s_%s,%s);\n"
				//	,cField,gcTableName,cField,cField);
				//We don't want admin or root users to be clobbered by a giant select ??? TODO
				//so we use a modifiable FK
				fprintf(fp,"\telse if(1)\n\t{\n");
				fprintf(fp,"\t\tprintf(\"<input type=text size=20 value='%%s' disabled>\\n\",ForeignKey(\"%s\",\"%s\",%s));\n"
					,cTableName,cFieldName,cField);
				fprintf(fp,"\t\tprintf(\"<input type=hidden size=20 maxlength=20 name=%s value='%%u' >\\n\",%s);\n"
					,cField,cField);
				fprintf(fp,"\t}\n");
			break;

		}//switch

	}//while
	fprintf(fp,"\tprintf(\"</tr>\\n\");");

}//void funcModuleInput(FILE *fp)


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
		if(!strcmp(field[0],"uCreatedBy"))
			fprintf(fp,"#define StandardFields\n");
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

			case COLTYPE_DATEEUR:
			if(field[3][0])
				fprintf(fp,"static char %s[20]={\"%s\"};\n",
					field[0],field[3]);
			else
				fprintf(fp,"static char %s[20]={\"01/01/1900\"};\n",field[0]);
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

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType"
			" FROM tField,tTable,tFieldType"
			" WHERE tField.uTable=tTable.uTable"
			" AND tField.uFieldType=tFieldType.uFieldType"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
	if(guDebug)
		logfileLine("funcModuleVarList",gcQuery);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=0;
	unsigned uRADType=0;
	sprintf(gcuJs,"0");
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uRADType);

		if(uFirst) fprintf(fp,",");

		//testing a simple hack
		if(uRADType==COLTYPE_DATETIME)
			sprintf(gcuJs,"1");
		if(uRADType==COLTYPE_DATEEUR)
		{
			sprintf(gcuJs,"1");
			fprintf(fp,"DATE_FORMAT(%s.%s,'%%d/%%m/%%Y')",gcTableName,field[0]);
		}	
		else
		{
			fprintf(fp,"%s.%s",gcTableName,field[0]);
		}


		uFirst=1;
	}
	mysql_free_result(res);

}//void funcModuleVarList(FILE *fp)


void funcModuleUpdateQuery(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	fprintf(fp,"sprintf(gcQuery,\"UPDATE %s SET \"\n",gcTableName);

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType,tField.cOtherOptions,tTable.uTemplateType"
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
	unsigned uTemplateType=0;
	char *cp1;
	while((field=mysql_fetch_row(res)))
	{
		//Special internal fields
		if(!strcmp(field[0],"uOwner"))
			continue;
		if(!strcmp(field[0],"uCreatedBy"))
			continue;
		if(!strcmp(field[0],"uCreatedDate"))
			continue;
		sscanf(field[1],"%u",&uRADType);
		if(!uTemplateType) sscanf(field[3],"%u",&uTemplateType);
		if(uRADType==COLTYPE_RADPRI)
			continue;
		if(uFirst)
			fprintf(fp,",\"\n");
		switch(uRADType)
		{
			default:
				fprintf(fp,"\t\t\"%s=%%u",field[0]);
			break;

			case COLTYPE_DATETIME:
			case COLTYPE_TIMESTAMP:
			case COLTYPE_VARCHARUKEY:
			case COLTYPE_CHAR:
			case COLTYPE_TEXT:
				fprintf(fp,"\t\t\"%s='%%s'",field[0]);
			break;

			case COLTYPE_VARCHAR:
				if((cp1=strstr(field[2],"CONCAT:")) && uTemplateType==uTEMPLATETYPE_BOOTSTRAP)
				{
					char cConcatField1[100]={""};
					char cConcatField2[100]={""};
					char *cp2;
					if((cp2=strchr(cp1+strlen("CONCAT:"),';')))
					{
						*cp2=0;	
						sprintf(cConcatField1,"%.99s",cp1+strlen("CONCAT:"));
						if((cp1=strchr(cp2+1,';')))
						{
							*cp1=0;
							sprintf(cConcatField2,"%.99s",cp2+1);
							*cp1=';';
						}
						*cp2=';';	
					}
					if(strstr(field[2],"UNSIGNED_CONCAT:"))
					{
						if(cConcatField1[0] && cConcatField2[0])
							fprintf(fp,"\t\t\"%s=CONCAT('%c:%%lu',' ','%c:%%lu')",
								field[0],cConcatField1[1],cConcatField2[1]);
						else if(cConcatField1[0])
							fprintf(fp,"\t\t\"%s='%c:%%u'",field[0],cConcatField1[1]);
						break;
					}
					else
					{
						if(cConcatField1[0] && cConcatField2[0])
							fprintf(fp,"\t\t\"%s=CONCAT('%%s',' ','%%s')",field[0]);
						else if(cConcatField1[0])
							fprintf(fp,"\t\t\"%s='%%s'",
								field[0]);
						break;
					}
				}
				fprintf(fp,"\t\t\"%s='%%s'",field[0]);
			break;

			case COLTYPE_UNIXTIMEUPDATE:
				fprintf(fp,"\t\t\"%s=UNIX_TIMESTAMP(NOW())",field[0]);
			break;

			case COLTYPE_DATEEUR:
				//STR_TO_DATE('20/2/2011','%d/%m/%Y')
				fprintf(fp,"\t\t\"%s=STR_TO_DATE('%%s','%%%%d/%%%%m/%%%%Y')",field[0]);
			break;
		}
		uFirst=1;
	}

	fprintf(fp,"\"\n\t\t\" WHERE _rowid=%%s\"\n");
	uRADType=0;
	mysql_data_seek(res,0);
	while((field=mysql_fetch_row(res)))
	{
		//Special internal fields
		if(!strcmp(field[0],"uOwner"))
			continue;
		if(!strcmp(field[0],"uCreatedBy"))
			continue;
		if(!strcmp(field[0],"uCreatedDate"))
			continue;
		sscanf(field[1],"%u",&uRADType);
		switch(uRADType)
		{
			default:
				fprintf(fp,"\t\t\t,%s\n",field[0]);
			break;

			case(COLTYPE_RADPRI):
			break;

			case COLTYPE_CHAR:
			case COLTYPE_DATETIME:
			case COLTYPE_DATEEUR:
			case COLTYPE_TIMESTAMP:
			case COLTYPE_VARCHAR:
			case COLTYPE_VARCHARUKEY:
			case COLTYPE_TEXT:
				if((cp1=strstr(field[2],"CONCAT:")) && uTemplateType==uTEMPLATETYPE_BOOTSTRAP)
				{
					char cConcatField1[64]={""};
					char cConcatField2[64]={""};
					char *cp2;
					if((cp2=strchr(cp1+strlen("CONCAT:"),';')))
					{
						*cp2=0;	
						sprintf(cConcatField1,"%.63s",cp1+strlen("CONCAT:"));
						if((cp1=strchr(cp2+1,';')))
						{
							*cp1=0;
							sprintf(cConcatField2,"%.63s",cp2+1);
							*cp1=';';
						}
						*cp2=';';	
					}
					if(strstr(field[2],"UNSIGNED_CONCAT:"))
					{
						if(cConcatField1[0])
							fprintf(fp,"\t\t\t,(long unsigned)%s\n",cConcatField1);
						if(cConcatField2[0])
							fprintf(fp,"\t\t\t,(long unsigned)%s\n",cConcatField2);
					}
					else
					{
						if(cConcatField1[0])
							fprintf(fp,"\t\t\t,cEndAtSpace(TextAreaSave(%s))\n",
								cConcatField1);
						if(cConcatField2[0])
							fprintf(fp,"\t\t\t,cEndAtSpace(TextAreaSave(%s))\n",
								cConcatField2);
					}
					break;
				}
				fprintf(fp,"\t\t\t,TextAreaSave(%s)\n",field[0]);
			break;

			case(COLTYPE_UNIXTIMEUPDATE):
			break;
		}
	}
	mysql_free_result(res);

	fprintf(fp,"\t\t\t,cRowid\n\t\t);\n");

}//void funcModuleUpdateQuery(FILE *fp)


void funcModuleInsertQuery(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	fprintf(fp,"sprintf(gcQuery,\"INSERT INTO %s SET \"\n",gcTableName);

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType,tField.cOtherOptions,tTable.uTemplateType"
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
	unsigned uTemplateType=0;
	char *cp1;
	while((field=mysql_fetch_row(res)))
	{
		//Special internal fields
		if(!strcmp(field[0],"uModBy"))
			continue;
		if(!strcmp(field[0],"uModDate"))
			continue;
		sscanf(field[1],"%u",&uRADType);
		if(!uTemplateType) sscanf(field[3],"%u",&uTemplateType);
		if(uRADType==COLTYPE_RADPRI)
			continue;
		if(uFirst)
			fprintf(fp,",\"\n");
		switch(uRADType)
		{
			default:
				fprintf(fp,"\t\t\"%s=%%u",field[0]);
			break;

			case(COLTYPE_RADPRI):
			break;

			case COLTYPE_CHAR:
			case COLTYPE_DATETIME:
			case COLTYPE_TIMESTAMP:
			case COLTYPE_VARCHAR:
			case COLTYPE_VARCHARUKEY:
			case COLTYPE_TEXT:
				if((cp1=strstr(field[2],"CONCAT:")) && uTemplateType==uTEMPLATETYPE_BOOTSTRAP)
				{
					char cConcatField1[64]={""};
					char cConcatField2[64]={""};
					char *cp2;
					if((cp2=strchr(cp1+strlen("CONCAT:"),';')))
					{
						*cp2=0;	
						sprintf(cConcatField1,"%.63s",cp1+strlen("CONCAT:"));
						if((cp1=strchr(cp2+1,';')))
						{
							*cp1=0;
							sprintf(cConcatField2,"%.63s",cp2+1);
							*cp1=';';
						}
						*cp2=';';	
					}
					if(cConcatField1[0] && cConcatField2[0])
					{
						if(strstr(field[2],"UNSIGNED_CONCAT:"))
							fprintf(fp,"\t\t\"%s=CONCAT('%c:%%lu',' ','%c:%%lu')",
								field[0],cConcatField1[1],cConcatField2[1]);
						else
							fprintf(fp,"\t\t\"%s=CONCAT('%%s',' ','%%s')",
									field[0]);
						break;
					}
					else if(cConcatField1[0])
					{
						if(strstr(field[2],"UNSIGNED_CONCAT:"))
							fprintf(fp,"\t\t\"%s='%c:%%lu'",field[0],cConcatField1[1]);
						else
							fprintf(fp,"\t\t\"%s='%%s'",field[0]);
						break;
					}
				}
				fprintf(fp,"\t\t\"%s='%%s'",field[0]);
			break;

			case(COLTYPE_UNIXTIMECREATE):
				fprintf(fp,"\t\t\"%s=UNIX_TIMESTAMP(NOW())",field[0]);
			break;

			case COLTYPE_DATEEUR:
				//STR_TO_DATE('20/2/2011','%d/%m/%Y')
				fprintf(fp,"\t\t\"%s=STR_TO_DATE('%%s','%%%%d/%%%%m/%%%%Y')",field[0]);
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
				fprintf(fp,"\t\t\t,%s\n",field[0]);
			break;
			case COLTYPE_RADPRI:
			break;
			case COLTYPE_CHAR:
			case COLTYPE_DATETIME:
			case COLTYPE_DATEEUR:
			case COLTYPE_TIMESTAMP:
			case COLTYPE_VARCHAR:
			case COLTYPE_VARCHARUKEY:
			case COLTYPE_TEXT:
				if((cp1=strstr(field[2],"CONCAT:")) && uTemplateType==uTEMPLATETYPE_BOOTSTRAP)
				{
					char cConcatField1[64]={""};
					char cConcatField2[64]={""};
					char *cp2;
					if((cp2=strchr(cp1+strlen("CONCAT:"),';')))
					{
						*cp2=0;	
						sprintf(cConcatField1,"%.63s",cp1+strlen("CONCAT:"));
						if((cp1=strchr(cp2+1,';')))
						{
							*cp1=0;
							sprintf(cConcatField2,"%.63s",cp2+1);
							*cp1=';';
						}
						*cp2=';';	
					}
					if(cConcatField1[0] && cConcatField2[0])
					{
						if(strstr(field[2],"UNSIGNED_CONCAT:"))
							fprintf(fp,"\t\t\t,(long unsigned)%s,(long unsigned)%s\n",cConcatField1,cConcatField2);
						else
							fprintf(fp,"\t\t\t,cEndAtSpace(TextAreaSave(%s)),cEndAtSpace(TextAreaSave(%s))\n",
										cConcatField1,cConcatField2);
						break;
					}
					else if(cConcatField1[0])
					{
						if(strstr(field[2],"UNSIGNED_CONCAT:"))
							fprintf(fp,"\t\t\t,(long unsigned)%s\n",cConcatField1);
						else
							fprintf(fp,"\t\t\t,cEndAtSpace(TextAreaSave(%s))\n",cConcatField1);
						break;
					}
				}
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
			case(COLTYPE_TEXT):
				fprintf(fp,"\t\t\"%s TEXT %s",field[0],field[1]);
			break;
			case COLTYPE_DATETIME:
			case COLTYPE_DATEEUR:
				fprintf(fp,"\t\t\"%s DATETIME %s",field[0],field[1]);
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


//libtemplate.a required call back hook
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
	else if(!strcmp(cFunction,"funcModuleInput"))
		funcModuleInput(fp);
	else if(!strcmp(cFunction,"funcModuleUpdateQuery"))
		funcModuleUpdateQuery(fp);
	else if(!strcmp(cFunction,"funcModuleVars"))
		funcModuleVars(fp);
	else if(!strcmp(cFunction,"funcModuleVarList"))
		funcModuleVarList(fp);
	else if(!strcmp(cFunction,"funcMakefileObjects"))
		funcMakefileObjects(fp);
	else if(!strcmp(cFunction,"funcMakefileRules"))
		funcMakefileRules(fp);
	else if(!strcmp(cFunction,"funcModulePrototypes"))
		funcModulePrototypes(fp);
	else if(!strcmp(cFunction,"funcBootstrapModulePrototypes"))
		funcBootstrapModulePrototypes(fp);
	else if(!strcmp(cFunction,"funcMainGetMenu"))
		funcMainGetMenu(fp);
	else if(!strcmp(cFunction,"funcBootstrapMainGetMenu"))
		funcBootstrapMainGetMenu(fp);
	else if(!strcmp(cFunction,"funcMainNavBars"))
		funcMainNavBars(fp);
	else if(!strcmp(cFunction,"funcMainPostFunctions"))
		funcMainPostFunctions(fp);
	else if(!strcmp(cFunction,"funcBootstrapMainPostFunctions"))
		funcBootstrapMainPostFunctions(fp);
	else if(!strcmp(cFunction,"funcMainTabMenu"))
		funcMainTabMenu(fp);
	else if(!strcmp(cFunction,"funcMainInitTableList"))
		funcMainInitTableList(fp);
	else if(!strcmp(cFunction,"funcMainCreateTables"))
		funcMainCreateTables(fp);
	else if(!strcmp(cFunction,"funcModuleLanguage"))
		funcModuleLanguage(fp);
	else if(!strcmp(cFunction,"funcBashEnvProject"))
		funcBashEnvProject(fp);
	else if(!strcmp(cFunction,"funcBootstrapNavItems"))
		funcBootstrapNavItems(fp);
	else if(!strcmp(cFunction,"funcBootstrapEditorFields"))
		funcBootstrapEditorFields(fp);
	else if(!strcmp(cFunction,"funcBootstrapFindFields"))
		funcBootstrapFindFields(fp);
	else if(!strcmp(cFunction,"funcBootstrapValueFields"))
		funcBootstrapValueFields(fp);
	else if(!strcmp(cFunction,"funcBootstrapRowVars"))
		funcBootstrapRowVars(fp);
	else if(!strcmp(cFunction,"funcBootstrapRowFields"))
		funcBootstrapRowFields(fp);
	else if(!strcmp(cFunction,"funcBootstrapRowFormats"))
		funcBootstrapRowFormats(fp);
	else if(!strcmp(cFunction,"funcBootstrapRowReportVars"))
		funcBootstrapRowReportVars(fp);
	else if(!strcmp(cFunction,"funcBootstrapRowReportFields"))
		funcBootstrapRowReportFields(fp);
	else if(!strcmp(cFunction,"funcBootstrapRowReportFormats"))
		funcBootstrapRowReportFormats(fp);
	else if(!strcmp(cFunction,"funcBootstrapCols"))
		funcBootstrapCols(fp);
	else if(!strcmp(cFunction,"funcBootstrapColsReport"))
		funcBootstrapColsReport(fp);
	else if(!strcmp(cFunction,"funcBootstrapConcat"))
		funcBootstrapConcat(fp);
	else if(!strcmp(cFunction,"funcBSGetHookAdditionalGentries"))
		funcBSGetHookAdditionalGentries(fp);
	else if(!strcmp(cFunction,"funcBSGetHookPrePages"))
		funcBSGetHookPrePages(fp);
	else if(!strcmp(cFunction,"funcBSGetHookPages"))
		funcBSGetHookPages(fp);
	else if(!strcmp(cFunction,"funcBSFKJsonRowsSelects"))
		funcBSFKJsonRowsSelects(fp);
	else if(!strcmp(cFunction,"funcBSTemplateFKNVPairs"))
		funcBSTemplateFKNVPairs(fp);
	else if(!strcmp(cFunction,"funcBootstrapRetEditor"))
		funcBootstrapRetEditor(fp);
	else if(!strcmp(cFunction,"funcBSLDTables"))
		funcBSLDTables(fp);
	else if(!strcmp(cFunction,"funcBSLDContextVar1"))
		funcBSLDContextVar1(fp);
	else if(!strcmp(cFunction,"funcBSLDContextVar2"))
		funcBSLDContextVar2(fp);
	//special func that has variants
	else if(!strncmp(cFunction,"funcConfiguration",17))
		funcConfiguration(fp,cFunction);
	else if(1)
		logfileLine("AppFunctions.missing",cFunction);
		

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


char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

	static char gcQuery[256];

        sprintf(gcQuery,"SELECT %s FROM %s WHERE _rowid=%u",
                        cFieldName,cTableName,uKey);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql)) return("FK error");

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

}//char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey)


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


void funcBashEnvProject(FILE *fp)
{
	//Need to improve Template lib to handle triple {${{cProject}}}
	//substitution
	fprintf(fp,"${%s}",gcProject);

}//void funcBashEnvProject(FILE *fp)


void funcMakefileObjects(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel"
			" FROM tTable"
			" WHERE uProject=%u"
			" AND uTemplateType=%u"
			" AND SUBSTR(cLabel,1,1)='t'"
			" AND cLabel NOT LIKE '%%.txt'"
			" ORDER BY uTableOrder",guProject,guTemplateType);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"%s.o ",WordToLower(field[0]));
	mysql_free_result(res);

}//void funcMakefileObjects(FILE *fp)


void funcMakefileRules(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel"
			" FROM tTable"
			" WHERE uProject=%u"
			" AND uTemplateType=%u"
			" AND SUBSTR(cLabel,1,1)='t'"
			" AND cLabel NOT LIKE '%%.txt'"
			" ORDER BY uTableOrder",guProject,guTemplateType);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		WordToLower(field[0]);
		if(guTemplateType==14)//bootstrap fix asap
			fprintf(fp,"%1$s.o: %1$s.c interface.h local.h ../../mysqlrad.h\n",field[0]);
		else
			fprintf(fp,"%1$s.o: %1$s.c mysqlrad.h language.h %1$sfunc.h local.h\n",field[0]);
		fprintf(fp,"\tcc -c %1$s.c -o %1$s.o $(CFLAGS)\n\n",field[0]);
	}
	mysql_free_result(res);

}//void funcMakefileRules(FILE *fp)


void funcBootstrapModulePrototypes(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	fprintf(fp,"//funcBootstrapModulePrototypes()\n");

	sprintf(gcQuery,"SELECT cLabel"
			" FROM tTable"
			" WHERE uProject=%u"
			" AND SUBSTR(cLabel,1,1)='t'"
			" AND cLabel NOT LIKE '%%.txt'"
			" AND uTemplateType=%u"
			" ORDER BY uTableOrder",guProject,uTEMPLATETYPE_BOOTSTRAP);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"//%s\n",field[0]);
		fprintf(fp,"void %sCommands(pentry entries[], int x);\n",field[0]);
		fprintf(fp,"void %s(const char *results);\n",field[0]);
		fprintf(fp,"void Process%sVars(pentry entries[], int x);\n",field[0]);
		fprintf(fp,"void %sContent(void);\n",field[0]);
		fprintf(fp,"void %sInputContent(void);\n",field[0]);
		fprintf(fp,"void %sInput(unsigned uMode);\n",field[0]);
		fprintf(fp,"void %sList(void);\n",field[0]);
		fprintf(fp,"void New%s(unsigned uMode);\n",field[0]);
		fprintf(fp,"void Mod%s(void);\n",field[0]);
		fprintf(fp,"void Create%s(void);\n",field[0]);
		fprintf(fp,"void Delete%s(void);\n",field[0]);
		fprintf(fp,"void %sGetHook(entry gentries[], int x);\n",field[0]);
		fprintf(fp,"void Ext%sNavBar(void);\n",field[0]);
		fprintf(fp,"\n");
	}
	mysql_free_result(res);

}//void funcBootstrapModulePrototypes(FILE *fp)


void funcModulePrototypes(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel"
			" FROM tTable"
			" WHERE uProject=%u"
			" AND SUBSTR(cLabel,1,1)='t'"
			" AND cLabel NOT LIKE '%%.txt'"
			" AND uTemplateType=%u"
			" ORDER BY uTableOrder",guProject,uTEMPLATETYPE_RAD4);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"//%s\n",field[0]);
		fprintf(fp,"int %sCommands(pentry entries[], int x);\n",field[0]);
		fprintf(fp,"void %s(const char *results);\n",field[0]);
		fprintf(fp,"void Process%sVars(pentry entries[], int x);\n",field[0]);
		fprintf(fp,"void %sContent(void);\n",field[0]);
		fprintf(fp,"void %sInputContent(void);\n",field[0]);
		fprintf(fp,"void %sInput(unsigned uMode);\n",field[0]);
		fprintf(fp,"void %sList(void);\n",field[0]);
		fprintf(fp,"void New%s(unsigned uMode);\n",field[0]);
		fprintf(fp,"void Mod%s(void);\n",field[0]);
		fprintf(fp,"void Create%s(void);\n",field[0]);
		fprintf(fp,"void Delete%s(void);\n",field[0]);
		fprintf(fp,"void Ext%sGetHook(entry gentries[], int x);\n",field[0]);
		fprintf(fp,"void Ext%sNavBar(void);\n",field[0]);
		fprintf(fp,"\n");
	}
	mysql_free_result(res);

}//void funcModulePrototypes(FILE *fp)


void funcBootstrapMainGetMenu(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel,SUBSTR(cLabel,2)"
			" FROM tTable"
			" WHERE uProject=%u"
			" AND SUBSTR(cLabel,1,1)='t'"
			" AND uTemplateType=%u"
			" ORDER BY uTableOrder",guProject,uTEMPLATETYPE_BOOTSTRAP);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	fprintf(fp,"//funcBootstrapMainGetMenu()\n");
	unsigned uFirst=1;
	while((field=mysql_fetch_row(res)))
	{
		if(uFirst)
			fprintf(fp,"\t\t\tif(!strcmp(gcPage,\"%s\"))\n",field[1]);
		else
			fprintf(fp,"\t\t\telse if(!strcmp(gcPage,\"%s\"))\n",field[1]);
		fprintf(fp,"\t\t\t\t%sGetHook(gentries,x);\n",field[0]);
		uFirst=0;
	}
        mysql_free_result(res);

}//void funcBootstrapMainGetMenu(FILE *fp)


void funcMainGetMenu(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel"
			" FROM tTable"
			" WHERE uProject=%u"
			" AND SUBSTR(cLabel,1,1)='t'"
			" AND cLabel NOT LIKE '%%.txt'"
			" AND uTemplateType=%u"
			" ORDER BY uTableOrder",guProject,uTEMPLATETYPE_RAD4);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	fprintf(fp,"//funcMainGetMenu()\n");
	unsigned uFirst=1;
	while((field=mysql_fetch_row(res)))
	{
		if(uFirst)
			fprintf(fp,"\t\t\tif(!strcmp(gcFunction,\"%s\"))\n",field[0]);
		else
			fprintf(fp,"\t\t\telse if(!strcmp(gcFunction,\"%s\"))\n",field[0]);
		fprintf(fp,"\t\t\t\tExt%sGetHook(gentries,x);\n",field[0]);
		uFirst=0;
	}
        mysql_free_result(res);

}//void funcMainGetMenu(FILE *fp)


void funcMainNavBars(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel"
			" FROM tTable"
			" WHERE uProject=%u"
			" AND SUBSTR(cLabel,1,1)='t'"
			" AND cLabel NOT LIKE '%%.txt'"
			" AND uTemplateType=%u"
			" ORDER BY uTableOrder",guProject,uTEMPLATETYPE_RAD4);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=1;
	fprintf(fp,"//funcMainNavBars()\n");
	while((field=mysql_fetch_row(res)))
	{
		if(uFirst)
			fprintf(fp,"\tif");
		else
			fprintf(fp,"\telse if");
		fprintf(fp,"(!strcmp(gcFunction,\"%s\") || !strcmp(gcFunction,\"%sTools\") ||\n"
				"\t\t\t!strcmp(gcFunction,\"%sList\"))\n",field[0],field[0],field[0]);
		fprintf(fp,"\t\tExt%sNavBar();\n",field[0]);
		uFirst=0;
	}
	mysql_free_result(res);

}//void funcMainNavBars(FILE *ofp)


void funcBootstrapMainPostFunctions(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel"
			" FROM tTable"
			" WHERE uProject=%u"
			" AND SUBSTR(cLabel,1,1)='t'"
			" AND uTemplateType=%u"
			" ORDER BY uTableOrder",guProject,uTEMPLATETYPE_BOOTSTRAP);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	fprintf(fp,"//funcBootstrapMainPostFunctions()\n");
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"\t%sCommands(entries,x);\n",field[0]);
        mysql_free_result(res);

}//void funcBootstrapMainPostFunctions(FILE *fp)


void funcMainPostFunctions(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel"
			" FROM tTable"
			" WHERE uProject=%u"
			" AND SUBSTR(cLabel,1,1)='t'"
			" AND cLabel NOT LIKE '%%.txt'"
			" AND uTemplateType=%u"
			" ORDER BY uTableOrder",guProject,uTEMPLATETYPE_RAD4);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	fprintf(fp,"//funcMainPostFunctions()\n");
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"\t%sCommands(entries,x);\n",field[0]);
        mysql_free_result(res);

}//void funcMainPostFunctions(FILE *fp)


void funcMainTabMenu(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel,uReadLevel,cDescription"
			" FROM tTable"
			" WHERE uProject=%u"
			" AND SUBSTR(cLabel,1,1)='t'"
			" AND cLabel NOT LIKE '%%.txt'"
			" AND uTemplateType=%u"
			" ORDER BY uTableOrder",guProject,uTEMPLATETYPE_RAD4);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	fprintf(fp,"//funcMainTabMenu()\n");
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"\t//%s\n",field[0]);
		fprintf(fp,"\tif(guPermLevel>=%s)\n",field[1]);
		fprintf(fp,"\t{\n");
		fprintf(fp,"\t  printf(\"\\t\\t\\t<li\");\n");
		fprintf(fp,"\t  if(strcmp(gcFunction,\"%s\") && strcmp(gcFunction,\"%sTools\") &&\n"
				"\t\t\tstrcmp(gcFunction,\"%sList\"))\n",field[0],field[0],field[0]);
		fprintf(fp,"\t\t  printf(\">\\n\");\n");
		fprintf(fp,"\t  else\n");
		fprintf(fp,"\t\t  printf(\" id=current>\\n\");\n");
		fprintf(fp,"\t  printf(\"\\t\\t\\t<a title='%s' href=?gcFunction=%s>%s</a>\\n\");\n",field[2],field[0],field[0]);
		fprintf(fp,"\t}\n");
	}
        mysql_free_result(res);

}//void funcMainTabMenu(FILE *fp)


void funcMainInitTableList(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel,uReadLevel,cDescription"
			" FROM tTable"
			" WHERE uProject=%u"
			" AND SUBSTR(cLabel,1,1)='t'"
			" AND cLabel NOT LIKE '%%.txt'"
			" AND uTemplateType=%u"
			" ORDER BY uTableOrder",guProject,uTEMPLATETYPE_RAD4);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
        if(mysql_num_rows(res)>0)
        {
                fprintf(fp,"{");
                while((field=mysql_fetch_row(res)))
                {
                        fprintf(fp,"\"%s\",",field[0]);
                }
                fprintf(fp,"\"\"};\n");
        }
        mysql_free_result(res);

}//void funcMainInitTableList(FILE *fp)


void funcMainCreateTables(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel"
			" FROM tTable"
			" WHERE uProject=%u"
			" AND SUBSTR(cLabel,1,1)='t'"
			" AND cLabel NOT LIKE '%%.txt'"
			" AND uTemplateType=%u"
			" ORDER BY uTableOrder",guProject,uTEMPLATETYPE_RAD4);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	fprintf(fp,"//funcMainCreateTables()\n");
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"\tCreate%s();\n",field[0]);
        mysql_free_result(res);

}//void funcMainCreateTables(FILE *fp)


void funcModuleLanguage(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel,tField.cTitle,tTable.cLabel"
			" FROM tField,tTable"
			" WHERE tField.uTable=tTable.uTable"
			" AND tTable.uProject=%u"
			" AND uTemplateType=%u"
			" ORDER BY uTableOrder",guProject,uTEMPLATETYPE_RAD4);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	fprintf(fp,"//funcModuleLanguage()\n");
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"#define LANG_FL_%s_%s \"%s\"\n",field[2],field[0],field[0]);
		fprintf(fp,"#define LANG_FT_%s_%s \"%s\"\n",field[2],field[0],field[1]);
        }
        mysql_free_result(res);

}//void funcModuleLanguage(FILE *fp)


void GetRADConfiguration(const char *cName,char *cValue,unsigned uValueSize, unsigned uServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        char cQuery[1024];
	char cExtra[100]={""};

        sprintf(cQuery,"SELECT cValue,cComment FROM tConfiguration WHERE cLabel='%s'",
			cName);
	if(uServer)
	{
		sprintf(cExtra," AND uServer=%u",uServer);
		strcat(cQuery,cExtra);
	}
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		char cFormat[16];
        	sprintf(cFormat,"%%.%us",uValueSize-1);
        	sprintf(cValue,cFormat,field[0]);
		if(!strncmp(cValue,"cComment",8))
        		sprintf(cValue,cFormat,field[1]);
	}
        mysql_free_result(res);

}//void GetRADConfiguration()


void funcConfiguration(FILE *fp,char *cFunction)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cComment"
			" FROM tConfiguration"
			" WHERE cLabel='%s'",cFunction);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	fprintf(fp,"//funcConfiguration(%s)\n",cFunction);
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"%.1024s",field[0]);
        mysql_free_result(res);

}//void funcConfiguration(FILE *fp,char *cFunction)


//Pass the template contents in one buffer: cTemplate
//Variables [[cLikeThis]] in the cTemplate are replaced if t_template
//setup correctly.
void Template2(char *cTemplate, struct t_template *template, FILE *fp)
{
	register int i,uState=0,j=0,k=0;
	char cVarName[256]={""};
	char cVarNameS1[1024]={""};

	for(i=0;cTemplate[i];i++)
	{
		if(cTemplate[i]=='[' && uState==0 && cTemplate[i+1]=='[')
		{
			uState=1;
			continue;
		}
		else if(cTemplate[i]=='[' && uState==1 )
		{
			uState=2;
			continue;
		}
		else if(cTemplate[i]==']' && uState==2 )
		{
			uState=3;
			continue;
		}
		else if(cTemplate[i]==']' && uState==1 )
		{
			uState=0;
			cVarNameS1[k]=0;
			fprintf(fp,"[%s]",cVarNameS1);
			k=0;
			continue;
		}
		else if(cTemplate[i]==']' && uState==3 )
		{
			register int n=0,uMatch=0;

			uState=0;
			cVarName[j]=0;
			while(template->cpName[n][0])
			{
				if(!strcmp(template->cpName[n],cVarName))
				{
					fprintf(fp,"%s",template->cpValue[n]);
					uMatch=1;
				}
				n++;
				if(n>99) break;
			}
			if(!uMatch)
			{
				if(cVarName[0]=='f')
				{
					//Every application that links to this lib
					//needs to supply this function
					AppFunctions(fp,cVarName);
				}
			}
			j=0;
			continue;
		}
		if(uState==1)
		{
			if(k>1023) return;
			cVarNameS1[k++]=cTemplate[i];
			continue;
		}
		else if(uState==2)
		{
			if(j>254) return;
			cVarName[j++]=cTemplate[i];
			continue;
		}
		fputc((int)cTemplate[i],fp);
	}

}//int Template2()


void funcBootstrapNavItems(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	fprintf(fp,"<!-- funcBootstrapNavItems() -->\n");

	sprintf(gcQuery,"SELECT SUBSTR(cLabel,2)"
			" FROM tTable"
			" WHERE uProject=%u"
			" AND SUBSTR(cLabel,1,1)='t'"
			" AND uTemplateType=%u"
			" AND uReadLevel>0"
			" ORDER BY uTableOrder",guProject,uTEMPLATETYPE_BOOTSTRAP);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
            	fprintf(fp,"\t\t<li {{cNavBarClass%1$s}}><a href=\"?gcPage=%1$s\">%1$s</a></li>\n",field[0]);
	}
	mysql_free_result(res);

}//void funcBootstrapNavItems(FILE *fp)


//
//Group of related Bootstrap func's
//

void funcBootstrapFindFields(FILE *fp)
{
/*
 * 	$editor.find('#cLabel').val(values.cLabel);
 * 	$editor.find('#cOwner').val(values.cOwner);
 */
	if(!guTable) return;

       	MYSQL_RES *res;
        MYSQL_ROW field;
	char cFieldName[100];

	//If named table get parent guTable
	sprintf(gcQuery,"SELECT cDescription FROM tTable"
			" WHERE uTable=%u"
			" AND cDescription!=''",
					guTable);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"<!-- Error: %s -->\n",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"uParentTable=%u;",&guTable);
	else
	{
		fprintf(fp,"<!-- No uParentTable for %u -->\n",guTable);
		mysql_free_result(res);
		return;
	}

	sprintf(gcQuery,"SELECT cLabel,cOtherOptions FROM tField"
			" WHERE uTable=%u"
			" AND uFieldType!=1"//not equal Rad Primary
			" AND cLabel!='cLabel'"
			" AND cLabel!='uOwner'"
			" AND cLabel!='uCreatedBy'"
			" AND cLabel!='uCreatedDate'"
			" AND cLabel!='uModBy'"
			" AND cLabel!='uModDate'"
			" ORDER BY uOrder",
					guTable);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"<!-- Error: %s -->\n",mysql_error(&gMysql));
		return;
	}

        res=mysql_store_result(&gMysql);
	unsigned uFirst=1;
	while((field=mysql_fetch_row(res)))
	{
		if(strstr(field[1],"FooTable:Report:"))
			continue;

		sprintf(cFieldName,"%.99s",field[0]);
		if(!uFirst) fprintf(fp,"\t\t\t\t");
		fprintf(fp,"$editor.find('#%1$s').val(values.%1$s);\n",cFieldName);
		uFirst=0;
	}
	mysql_free_result(res);

}//void funcBootstrapFindFields(FILE *fp)


void funcBootstrapConcat(FILE *fp)
{
/*
 *	values.cLabel=values.cApellido+' '+values.cNombre;
 * 	
 */
	if(!guTable) return;

       	MYSQL_RES *res;
        MYSQL_ROW field;
	char cFieldName[100];

	sprintf(gcQuery,"SELECT cLabel,cOtherOptions FROM tField"
			" WHERE uTable=%u"
			" AND cOtherOptions LIKE '%%CONCAT:%%'"
			" AND uFieldType!=1"//not equal Rad Primary
			" AND cLabel!='uOwner'"
			" AND cLabel!='uCreatedBy'"
			" AND cLabel!='uCreatedDate'"
			" AND cLabel!='uModBy'"
			" AND cLabel!='uModDate'"
			" ORDER BY uOrder",
					guTable);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"<!-- Error: %s -->\n",mysql_error(&gMysql));
		return;
	}

        res=mysql_store_result(&gMysql);
	char *cp1;
	fprintf(fp,"//funcBootstrapConcat() %u",guTable);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(cFieldName,"%.99s",field[0]);
		fprintf(fp,"\n\t\t\t\t\t\t");
		if((cp1=strstr(field[1],"CONCAT:")))
		{
			char cConcatField1[64]={""};
			char cConcatField2[64]={""};
			char *cp2;
			if((cp2=strchr(cp1+strlen("CONCAT:"),';')))
			{
				*cp2=0;	
				sprintf(cConcatField1,"%.63s",cp1+strlen("CONCAT:"));
				if((cp1=strchr(cp2+1,';')))
				{
					*cp1=0;
					sprintf(cConcatField2,"%.63s",cp2+1);
					*cp1=';';
				}
				*cp2=',';	
			}
			if(cConcatField1[0] && cConcatField2[0])
			{
				//values.cLabel = values.cApellido+' '+values.cNombre;
				fprintf(fp,"values.%s = values.%s+' '+values.%s;",cFieldName,cConcatField1,cConcatField2);
			}
			else if(cConcatField1[0])
			{
				//values.cLabel = values.cApellido;
				fprintf(fp,"values.%s = values.%s.substring(0,32);",cFieldName,cConcatField1);
			}
		}
	}
	mysql_free_result(res);
}//void funcBootstrapConcat(FILE *fp)


void funcBootstrapValueFields(FILE *fp)
{
/*
 *	cLabel: $editor.find('#cLabel').val(),
 *	cOwner: $editor.find('#cOwner').val()	
 * 	
 */
	if(!guTable) return;

       	MYSQL_RES *res;
        MYSQL_ROW field;
	char cFieldName[100];

	//If named table get parent guTable
	sprintf(gcQuery,"SELECT cDescription FROM tTable"
			" WHERE uTable=%u"
			" AND cDescription!=''",
					guTable);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"<!-- Error: %s -->\n",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"uParentTable=%u;",&guTable);
	else
	{
		fprintf(fp,"<!-- No uParentTable for %u -->\n",guTable);
		mysql_free_result(res);
		return;
	}

	sprintf(gcQuery,"SELECT cLabel,cOtherOptions FROM tField"
			" WHERE uTable=%u"
			" AND uFieldType!=1"//not equal Rad Primary
			" AND cLabel!='uOwner'"
			" AND cLabel!='uCreatedBy'"
			" AND cLabel!='uCreatedDate'"
			" AND cLabel!='uModBy'"
			" AND cLabel!='uModDate'"
			" ORDER BY uOrder",
					guTable);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"<!-- Error: %s -->\n",mysql_error(&gMysql));
		return;
	}

        res=mysql_store_result(&gMysql);
	unsigned uFirst=1;
	char *cp1;
	while((field=mysql_fetch_row(res)))
	{
		if(strstr(field[1],"FooTable:Report:"))
			continue;

		sprintf(cFieldName,"%.99s",field[0]);
		if(!uFirst) fprintf(fp,",\n\t\t\t\t");
		if((cp1=strstr(field[1],"CONCAT:")))
		{
			char cConcatField1[64]={""};
			char cConcatField2[64]={""};
			char *cp2;
			if((cp2=strchr(cp1+strlen("CONCAT:"),';')))
			{
				*cp2=0;	
				sprintf(cConcatField1,"%.63s",cp1+strlen("CONCAT:"));
				if((cp1=strchr(cp2+1,';')))
				{
					*cp1=0;
					sprintf(cConcatField2,"%.63s",cp2+1);
					*cp1=';';
				}
				*cp2=';';	
			}
			if(cConcatField1[0])
			{
				fprintf(fp,"%s: $editor.find('#%s').val().substring(0,32)",cFieldName,cConcatField1);
			}
		}
		else
		{
			fprintf(fp,"%1$s: $editor.find('#%1$s').val()",cFieldName);
		}
		uFirst=0;
	}
	fprintf(fp,"\n");
	mysql_free_result(res);

}//void funcBootstrapValueFields(FILE *fp)


//Used by special template table requires parent
void funcBootstrapEditorFields(FILE *fp)
{
	if(!guTable) return;
       	MYSQL_RES *res;
        MYSQL_ROW field;

	//If named table get parent guTable
	sprintf(gcQuery,"SELECT cDescription FROM tTable"
			" WHERE uTable=%u"
			" AND cDescription!=''",
					guTable);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"<!-- Error: %s -->\n",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"uParentTable=%u;",&guTable);
	else
	{
		fprintf(fp,"<!-- No uParentTable for %u -->\n",guTable);
		mysql_free_result(res);
		return;
	}


	char cFieldName[100];
	char cBSLabelClass[64]={"col-sm-6"};
	char cBSInputClass[64]={"col-sm-6"};

	sprintf(gcQuery,"SELECT tField.cLabel,tField.cTitle,"
			" tField.cOtherOptions,tFieldType.uRADType,tField.cFormDefault"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tField.uFieldType!=1"//not equal Rad Primary
			" AND tField.cLabel!='cLabel'"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",
					guTable);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"<!-- Error: %s -->\n",mysql_error(&gMysql));
		return;
	}

	fprintf(fp,"<!-- funcBootstrapEditorFields() %u -->\n",guTable);
	fprintf(fp,"\t\t\t\t<input type=\"number\" id=\"%1$s\" name=\"%1$s\" class=\"hidden\"/>\n",gcTableKey);

        res=mysql_store_result(&gMysql);
	char *cRequired="";
	while((field=mysql_fetch_row(res)))
	{
		if(strstr(field[2],"FooTable:Report:"))
			continue;

		sprintf(cFieldName,"%.99s",field[0]);
		if(strstr(field[2],"FooTable:Required;"))
			cRequired=" required";
		else
			cRequired="";
		//fprintf(fp,"\t\t\t\t<div class=\"col-lg-4 col-xs-6\">\n");
		fprintf(fp,"\t\t\t\t<div class=\"form-group%s\">\n",cRequired);
		fprintf(fp,"\t\t\t\t\t<label for=\"%s\" class=\"%s control-label\">%s</label>\n",cFieldName,cBSLabelClass,cFieldName+1);
		fprintf(fp,"\t\t\t\t\t<div class=\"%s\">\n",cBSInputClass);
		unsigned uRADType=0;
		sscanf(field[3],"%u",&uRADType);
		switch(uRADType)
		{
			case COLTYPE_DATEEUR:
				fprintf(fp,"\t\t\t\t\t\t<input%1$s type=\"text\" class=\"form-control\""
					" id=\"%2$s\" name=\"%2$s\" title=\"%3$s\">\n",
						cRequired,cFieldName,field[1]);
			break;

			case COLTYPE_SELECTTABLE:
			case COLTYPE_SELECTTABLE_OWNER:
				fprintf(fp,"\t\t\t\t\t\t{{funcSelect(t%s)}}\n",cFieldName+1);
			break;

			case COLTYPE_TEXT:
				fprintf(fp,"\t\t\t\t\t\t<textarea%1$s class=\"form-control\""
					" id=\"%2$s\" name=\"%2$s\" title=\"%3$s\"></textarea>\n",
						cRequired,cFieldName,field[1]);
			break;

			case COLTYPE_FOREIGNKEY:
				fprintf(fp,"\t\t\t\t\t\t<input value=\"{{%1$s}}\" type=\"number\""
					" id=\"%1$s\" name=\"%1$s\" class=\"hidden\"/>\n",
					cFieldName);
				fprintf(fp,"\t\t\t\t\t\t<input disabled value=\"{{%1$s}}\" type=\"number\""
					" class=\"form-control\" id=\"%1$s\" name=\"%1$s\" title=\"%2$s\">\n",
					cFieldName,field[1]);
			break;

			default:
				fprintf(fp,"\t\t\t\t\t\t<input%1$s type=\"text\" class=\"form-control\""
					" id=\"%2$s\" name=\"%2$s\" title=\"%3$s\">\n",
						cRequired,cFieldName,field[1]);
		}
		//fprintf(fp,"\t\t\t\t\t\t\tplaceholder=\"%s\" required>\n",cFieldName);
		fprintf(fp,"\t\t\t\t\t</div>\n");
		fprintf(fp,"\t\t\t\t</div>\n");
		//fprintf(fp,"\t\t\t\t</div>\n");
	}
	mysql_free_result(res);

}//void funcBootstrapEditorFields(FILE *fp)


void funcBootstrapRowVars(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType,tField.cOtherOptions"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBootstrapRowVars",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=1;
	unsigned uRADType=0;
	while((field=mysql_fetch_row(res)))
	{

		sscanf(field[1],"%u",&uRADType);

		if(!uFirst) fprintf(fp,",");

		switch(uRADType)
		{
			case COLTYPE_DATEEUR:
				fprintf(fp,"'%1$s',DATE_FORMAT(%1$s,'%%%%d/%%%%m/%%%%Y')",field[0]);
			break;

			case COLTYPE_TEXT:
				fprintf(fp,"'%1$s',REPLACE(%1$s,'\\n','\\\\\\\\n')",field[0]);
			break;
			default:
				if(!strcmp(field[0],"cLabel"))
					fprintf(fp,"'cLabel',cLabel,"
						"'cLink',CONCAT('<a href=?gcPage=%s&u%s=',u%s,'>"
						"<span class=\\\\\\\\\\\"glyphicon glyphicon-check\\\\\\\\\\\"></span></a>')",
							gcTableName+1,gcTableName+1,gcTableName+1);
				else if(strstr(field[2],"FooTable:Report:"))
					fprintf(fp,"'%s',CONCAT('<a href=?gcPage=%s&u%s=',u%s,'>"
						"<span class=\\\\\\\\\\\"glyphicon glyphicon-edit\\\\\\\\\\\"></span></a>')",
						field[0],field[0]+1,gcTableName+1,gcTableName+1);
				else
					fprintf(fp,"'%1$s',%1$s",field[0]);
		}

		uFirst=0;
	}
	mysql_free_result(res);

}//void funcBootstrapRowVars(FILE *fp)



void funcBootstrapRowFormats(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType,tField.cOtherOptions"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBootstrapRowFormats",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=0;
	unsigned uRADType=0;
	while((field=mysql_fetch_row(res)))
	{

		sscanf(field[1],"%u",&uRADType);
		if(uFirst++)
			fprintf(fp,",");
		switch(uRADType)
		{
			//Number data
			case COLTYPE_RADPRI:
			case COLTYPE_PRIKEY:
			case COLTYPE_INTUNSIGNED:
			case COLTYPE_BIGINT:
				fprintf(fp,"\\\"%%s\\\": %%s");
			break;

			case COLTYPE_DATEEUR:
			case COLTYPE_DATETIME:
			default:
				if(!strcmp(field[0],"cLabel"))
					fprintf(fp,"\\\"%%s\\\": \\\"%%s\\\",\\\"%%s\\\": \\\"%%s\\\"");
				else
					fprintf(fp,"\\\"%%s\\\": \\\"%%s\\\"");
		}
	}
	mysql_free_result(res);

}//void funcBootstrapRowFormats(FILE *fp)


void funcBootstrapRowFields(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType,tField.cOtherOptions"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBootstrapRowFields",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=0;
	unsigned uRADType=0;
	while((field=mysql_fetch_row(res)))
	{

		sscanf(field[1],"%u",&uRADType);
		if(uFirst)
			fprintf(fp,",");
		switch(uRADType)
		{
			case COLTYPE_SELECTTABLE:
			case COLTYPE_SELECTTABLE_OWNER:
				fprintf(fp,"field[%u],cForeignKeyStr(\"t%s\",\"cLabel\",field[%u])",uFirst,field[0]+1,uFirst+1);
			break;

			default:
				if(!strcmp(field[0],"cLabel"))
				{
					fprintf(fp,"field[%u],field[%u],",uFirst,uFirst+1);
					uFirst+=2;
					fprintf(fp,"field[%u],field[%u]",uFirst,uFirst+1);
				}
				else
				{
					fprintf(fp,"field[%u],field[%u]",uFirst,uFirst+1);
				}
		}
		uFirst+=2;
	}
	mysql_free_result(res);

}//void funcBootstrapRowFields(FILE *fp)


void funcBootstrapCols(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel,tField.cTitle,tField.cOtherOptions,tFieldType.uRADType"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBootstrapCols",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uRADType=0;
	char *cBreakpoints={""};
	unsigned uCount=0;
	unsigned uOnce=1;
	char *cFilterable="false";
	char *cDataType="text";
	fprintf(fp,"\n");
	unsigned uRows=mysql_num_rows(res);
	char *cComma=",";
	while((field=mysql_fetch_row(res)))
	{
		uCount++;

		sscanf(field[3],"%u",&uRADType);

		unsigned uHtml=0;
		switch(uRADType)
		{
			case COLTYPE_RADPRI:
			case COLTYPE_PRIKEY:
			case COLTYPE_INTUNSIGNED:
			case COLTYPE_BIGINT:
				cDataType="number";
			break;

			case COLTYPE_FOREIGNKEY:
				cDataType="html";
				uHtml=1;
			break;

			default:
				cDataType="text";
		}

		if(strstr(field[2],"FooTable:Filterable") || uRADType==COLTYPE_RADPRI)
			cFilterable="true";
		else
			cFilterable="false";
		if(uCount>=uRows) cComma="";
		if(uHtml)
			fprintf(fp,"\tprintf(\"\\t{\\\"name\\\": \\\"%s\\\", \\\"title\\\": \\\"%s\\\","
					" \\\"filterable\\\": %s,"
					" \\\"data-type\\\": \\\"html\\\","
					" \\\"sortable\\\": false,"
					" \\\"breakpoints\\\": \\\"%s\\\"}%s\\n\");\n"
				,field[0],field[0]+1,cFilterable,cBreakpoints,cComma);
		else
			fprintf(fp,"\tprintf(\"\\t{\\\"name\\\": \\\"%s\\\", \\\"title\\\": \\\"%s\\\","
					" \\\"filterable\\\": %s,"
					" \\\"data-type\\\": \\\"%s\\\","
					" \\\"breakpoints\\\": \\\"%s\\\"}%s\\n\");\n"
				,field[0],field[0]+1,cFilterable,cDataType,cBreakpoints,cComma);
		if(!strcmp(field[0],"cLabel"))
		{
			fprintf(fp,"\tprintf(\"\\t{\\\"name\\\": \\\"cLink\\\", \\\"title\\\": \\\"Link\\\","
					" \\\"filterable\\\": false,"
					" \\\"data-type\\\": \\\"html\\\","
					" \\\"sortable\\\": false,"
					" \\\"breakpoints\\\": \\\"\\\"}%s\\n\");\n"
						,cComma);
		}
		if(uOnce && uCount>1)
		{
			cBreakpoints="all";
			uOnce=0;
		}
	}
	mysql_free_result(res);

}//void funcBootstrapCols(FILE *fp)

//
//End of Group of related Bootstrap func's
//

//
//End of Group of related Bootstrap func's
//

void funcBootstrapRowReportVars(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType,tField.cOtherOptions"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBootstrapRowReportVars",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=1;
	unsigned uRADType=0;
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uRADType);

		if(!uFirst) fprintf(fp,",");

		switch(uRADType)
		{
			case COLTYPE_DATEEUR:
				fprintf(fp,"'%1$s',DATE_FORMAT(%1$s,'%%%%d/%%%%m/%%%%Y')",field[0]);
			break;

			case COLTYPE_TEXT:
				fprintf(fp,"'%1$s',REPLACE(%1$s,'\\n','\\\\\\\\n')",field[0]);
			break;

			default:
				if(strstr(field[2],"FooTable:Report:"))
					fprintf(fp,"'%s',CONCAT('<a href=?gcPage=%s&u%s=',u%s,'>"
						"<span class=\\\\\\\\\\\"glyphicon glyphicon-edit\\\\\\\\\\\"></span></a>')",
						field[0],field[0]+1,gcTableName+1,gcTableName+1);
				else
					fprintf(fp,"'%1$s',%1$s",field[0]);
		}

		uFirst=0;
	}
	mysql_free_result(res);

}//void funcBootstrapRowReportVars(FILE *fp)



void funcBootstrapRowReportFormats(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType,tField.cOtherOptions"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBootstrapRowReportFormats",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=0;
	unsigned uRADType=0;
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uRADType);
		if(uFirst++)
			fprintf(fp,",");
		switch(uRADType)
		{
			//Number data
			case COLTYPE_RADPRI:
			case COLTYPE_PRIKEY:
			case COLTYPE_INTUNSIGNED:
			case COLTYPE_BIGINT:
				fprintf(fp,"\\\"%%s\\\": %%s");
			break;

			default:
				if(strstr(field[2],"FooTable:Report:"))
					fprintf(fp,"\\\"%%s\\\": \\\"%%s\\\"");
				else
					fprintf(fp,"\\\"%%s\\\": \\\"%%s\\\"");
		}
	}
	mysql_free_result(res);

}//void funcBootstrapRowReportFormats(FILE *fp)


void funcBootstrapRowReportFields(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType,tField.cOtherOptions"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBootstrapRowReportFields",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=0;
	unsigned uRADType=0;
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uRADType);
		if(uFirst)
			fprintf(fp,",");
		switch(uRADType)
		{
			case COLTYPE_SELECTTABLE:
			case COLTYPE_SELECTTABLE_OWNER:
				fprintf(fp,"field[%u],cForeignKeyStr(\"t%s\",\"cLabel\",field[%u])",uFirst,field[0]+1,uFirst+1);
			break;

			default:
				if(strstr(field[2],"FooTable:Report:"))
					fprintf(fp,"field[%u],field[%u]",uFirst,uFirst+1);
				else
					fprintf(fp,"field[%u],field[%u]",uFirst,uFirst+1);
		}
		uFirst+=2;
	}
	mysql_free_result(res);

}//void funcBootstrapRowReportFields(FILE *fp)


void funcBootstrapColsReport(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel,tField.cTitle,tField.cOtherOptions,tFieldType.uRADType"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBootstrapColsReport",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uRADType=0;
	char *cBreakpoints={""};
	unsigned uCount=0;
	unsigned uOnce=1;
	char *cFilterable="false";
	char *cDataType="text";
	fprintf(fp,"\n");
	unsigned uRows=mysql_num_rows(res);
	char *cComma=",";
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[3],"%u",&uRADType);

		switch(uRADType)
		{
			case COLTYPE_RADPRI:
			case COLTYPE_PRIKEY:
			case COLTYPE_INTUNSIGNED:
			case COLTYPE_BIGINT:
			case COLTYPE_DECIMAL:
			case COLTYPE_MONEY:
				cDataType="number";
			break;

			default:
				cDataType="text";
		}

		if(strstr(field[2],"FooTable:Filterable") || uRADType==COLTYPE_RADPRI)
			cFilterable="true";
		else
			cFilterable="false";
		if(strstr(field[2],"FooTable:Report:") || !strcmp(field[0],"cLabel"))
		{
				cDataType="html";
		}
		uCount++;
		if(uCount>=uRows) cComma="";
		fprintf(fp,"\tprintf(\"\\t{\\\"name\\\": \\\"%s\\\", \\\"title\\\": \\\"%s\\\","
					" \\\"filterable\\\": %s,"
					" \\\"data-type\\\": \\\"%s\\\","
					" \\\"breakpoints\\\": \\\"%s\\\"}%s\\n\");\n"
				,field[0],field[0]+1,cFilterable,cDataType,cBreakpoints,cComma);
		if(uOnce && uCount>2)
		{
			cBreakpoints="all";
			uOnce=0;
		}
	}
	mysql_free_result(res);

}//void funcBootstrapColsReport(FILE *fp)


void funcBSGetHookAdditionalGentries(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tFieldType.uRADType=%u"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable,COLTYPE_FOREIGNKEY);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBSGetHookAdditionalGentries",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uNotFirst=0;
	while((field=mysql_fetch_row(res)))
	{
/*                else if(!strcmp(gentries[i].name,"uPaciente"))
 *                                        sscanf(gentries[i].val,"%u",&uPaciente);
 */
		if(uNotFirst++) fprintf(fp,"\t\t");
		fprintf(fp,"else if(!strcmp(gentries[i].name,\"%1$s\"))\n"
				"\t\t\tsscanf(gentries[i].val,\"%%u\",&%1$s);\n"
					,field[0]);
	}

}//void funcBSGetHookAdditionalGentries(FILE *fp)


void funcBSGetHookPrePages(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	//Context generator: Bootstrap table
	char cTableOptions[64]={""};
	sprintf(cTableOptions,"%.63s",ForeignKey("tTable","cDescription",guTable));
	sprintf(gcQuery,"SELECT tField.cLabel"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND (tFieldType.uRADType=%u OR tFieldType.uRADType=%u)"
			" AND tField.cOtherOptions LIKE '%%Bootstrap:Context;%%'"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder LIMIT 2",guTable,COLTYPE_FOREIGNKEY,COLTYPE_RADPRI);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBSGetHookPages",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uNotFirst=0;
	char cContextVar[2][64]={"",""};
	while((field=mysql_fetch_row(res)))
		sprintf(cContextVar[uNotFirst++],"%.63s",field[0]);

	if(strstr(cTableOptions,"Bootstrap:Level1"))
	{
		fprintf(fp,"//funcBSGetHookPrePages Bootstrap:Level1\n");
	
		fprintf(fp,"\tgcContext[0]=0;\n");
//if({{cTableKey}})
//{
//	sprintf(gcContext,"<a href=?gcPage={{cTableNameBS}}&{{cTableKey}}=%u>%s</a>",
//				{{cTableKey}},cForeignKey("{{cTableName}}","cLabel",{{cTableKey}}));
//	uSetSessionConfig("{{cTableKey}}",{{cTableKey}});
//	uSetSessionConfig("{{cContextVar}}",0);
//	html{{cTableName}}Report();
//}
		if(cContextVar[0][0] || cContextVar[1][0])
		{
			char *cp=cContextVar[1];
			if(!cContextVar[1][0])
				cp=cContextVar[0];
			fprintf(fp,"\tif(%s)\n"
			"\t{\n"
			"\t\tsprintf(gcContext,\"<a href=?gcPage=%s&%s=%%u>%%s</a>\",\n"
			"\t\t\t\t%s,cForeignKey(\"%s\",\"cLabel\",%s));\n"
			"\t\tuSetSessionConfig(\"%s\",%s);\n"
			"\t\tuSetSessionConfig(\"%s\",0);\n"
			"\t\thtml%sReport();\n"
			"\t}\n",
				gcTableKey,
				gcTableNameBS,gcTableKey,
				gcTableKey,gcTableName,gcTableKey,
				gcTableKey,gcTableKey,
				cp,
				gcTableName);
		}
		else
			fprintf(fp,"\tif(%s)\n"
			"\t{\n"
			"\t\tsprintf(gcContext,\"<a href=?gcPage=%s&%s=%%u>%%s</a>\",\n"
			"\t\t\t\t%s,cForeignKey(\"%s\",\"cLabel\",%s));\n"
			"\t\tuSetSessionConfig(\"%s\",%s);\n"
			"\t\thtml%sReport();\n"
			"\t}\n",
				gcTableKey,
				gcTableNameBS,gcTableKey,
				gcTableKey,gcTableName,gcTableKey,
				gcTableKey,gcTableKey,
				gcTableName);
	}//Bootstrap:Level1 end
	else if(strstr(cTableOptions,"Bootstrap:Level2"))
	{
		fprintf(fp,"//funcBSGetHookPrePages Bootstrap:Level2\n");
		if(cContextVar[0][0])
			fprintf(fp,"\tif(!%1$s) %1$s=uGetSessionConfig(\"%1$s\");\n",cContextVar[0]);
		if(cContextVar[1][0])
			fprintf(fp,"\tif(!%1$s) %1$s=uGetSessionConfig(\"%1$s\");\n",cContextVar[1]);
	}//Bootstrap:Level2 end
	else
	{
		fprintf(fp,"//funcBSGetHookPrePages Bootstrap:Level3\n");
		if(cContextVar[0][0])
			fprintf(fp,"\tif(!%1$s) %1$s=uGetSessionConfig(\"%1$s\");\n",cContextVar[0]);
		if(cContextVar[1][0])
			fprintf(fp,"\tif(!%1$s) %1$s=uGetSessionConfig(\"%1$s\");\n",cContextVar[1]);
	
	}//Bootstrap:Level3 implicit end
	
}//void funcBSGetHookPrePages(FILE *fp)


void funcBSGetHookPages(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	//Context generator: Bootstrap table
	char cTableOptions[64]={""};
	sprintf(cTableOptions,"%.63s",ForeignKey("tTable","cDescription",guTable));
	sprintf(gcQuery,"SELECT tField.cLabel"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND (tFieldType.uRADType=%u OR tFieldType.uRADType=%u)"
			" AND tField.cOtherOptions LIKE '%%Bootstrap:Context;%%'"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder LIMIT 2",guTable,COLTYPE_FOREIGNKEY,COLTYPE_RADPRI);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBSGetHookPages",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uNotFirst=0;
	char cContextVar[2][64]={"",""};
	while((field=mysql_fetch_row(res)))
		sprintf(cContextVar[uNotFirst++],"%.63s",field[0]);
/*
 *      if(uConsulta && uPaciente)
 *      {
 *      	sprintf(gcContext,"Consulta:<a href=?gcPage=Consulta&uConsulta=%u>%s</a>"
 *      			" Paciente:<a href=?gcPage=Paciente&uPaciente=%u>%s</a>",
 *			uConsulta,ForeignKey("tConsulta","cLabel",uConsulta),
 *			uPaciente,ForeignKey("tPaciente","cLabel",uPaciente));
 *	}
 */

	if(strstr(cTableOptions,"Bootstrap:Level1"))
	{
		fprintf(fp,"//funcBSGetHookPages Bootstrap:Level1\n");
	
	}
	else if(strstr(cTableOptions,"Bootstrap:Level2"))
	{
		fprintf(fp,"//funcBSGetHookPages Bootstrap:Level2\n");
		if(cContextVar[1][0] && cContextVar[0][0])
		{
	
		fprintf(fp,"\tif(%s && %s)\n",cContextVar[0],cContextVar[1]);
		fprintf(fp,"\t{\n");
		fprintf(fp,"\t\tsprintf(gcContext,\"%s:<a href=?gcPage=%s&u%s=%%u>%%s</a>\"\n",cContextVar[0]+1,cContextVar[0]+1,cContextVar[0]+1);
		fprintf(fp,"\t\t\t\t\" %s:<a href=?gcPage=%s&u%s=%%u>%%s</a>\",\n",cContextVar[1]+1,cContextVar[1]+1,cContextVar[1]+1);
		fprintf(fp,"\t\t\tu%s,cForeignKey(\"t%s\",\"cLabel\",%s),\n",cContextVar[0]+1,cContextVar[0]+1,cContextVar[0]);
		fprintf(fp,"\t\t\tu%s,cForeignKey(\"t%s\",\"cLabel\",%s));\n",cContextVar[1]+1,cContextVar[1]+1,cContextVar[1]);
		fprintf(fp,"\t\tsprintf(gcFilterRows,\"&%s=%%u\",%s);\n",cContextVar[0],cContextVar[0]);
		fprintf(fp,"\t\tsprintf(gcFilterCols,\"&%s=%%u\",%s);\n",cContextVar[0],cContextVar[0]);
		fprintf(fp,"\t\tuSetSessionConfig(\"%s\",%s);\n",cContextVar[0],cContextVar[0]);
		fprintf(fp,"\t\thtml%sFilter();\n",gcTableName);
		fprintf(fp,"\t}\n\n");
		}

		sprintf(gcQuery,"SELECT tField.cLabel"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tFieldType.uRADType=%u"
			" AND tField.cOtherOptions LIKE '%%Bootstrap:Context;%%'"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable,COLTYPE_FOREIGNKEY);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
		{
			if(guDebug)
				logfileLine("funcBSGetHookPages",gcQuery);
			fprintf(fp,"%s",mysql_error(&gMysql));
			return;
		}
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
/*
 *         else if(uPaciente)
 *         {
 *         	if(!gcContext[0]) sprintf(gcContext,"Paciente <a href=?gcPage=Paciente&uPaciente=%u>%s</a>",
 *         					uPaciente,ForeignKey("tPaciente","cLabel",uPaciente));
 *         	sprintf(gcFilterRows,"&uPaciente=%u",uPaciente);
 *         	sprintf(gcFilterCols,"&uPaciente=%u",uPaciente);
 *         	uSetSessionConfig("uPaciente",uPaciente);
 *         	uSetSessionConfig("uConsulta",0);
 *         	htmltConsultaFilter();
 *         }
 */

			fprintf(fp,"\telse if(%s)\n"
			"\t{\n"
			"\t\tif(!gcContext[0]) sprintf(gcContext,\"%s <a href=?gcPage=%s&%s=%%u>%%s</a>\",\n"
			"\t\t\t\t%s,cForeignKey(\"t%s\",\"cLabel\",%s));\n"
			"\t\tsprintf(gcFilterRows,\"&%s=%%u\",%s);\n"
			"\t\tsprintf(gcFilterCols,\"&%s=%%u\",%s);\n"
			"\t\tuSetSessionConfig(\"%s\",%s);\n"
			"\t\thtml%sFilter();\n"
			"\t}\n",
				field[0],
				field[0]+1,field[0]+1,field[0],
				field[0],field[0]+1,field[0],
				field[0],field[0],
				field[0],field[0],
				field[0],field[0],
				gcTableName);
		}
	}
	else
	{
		fprintf(fp,"//funcBSGetHookPages Bootstrap:Level3\n");
		if(cContextVar[1][0] && cContextVar[0][0])
		{
	
		fprintf(fp,"\tif(%s && %s && %s)\n",cContextVar[0],cContextVar[1],gcTableKey);
		fprintf(fp,"\t{\n");
		fprintf(fp,"\t\tsprintf(gcContext,\"%s:<a href=?gcPage=%s&u%s=%%u>%%s</a>\"\n",cContextVar[0]+1,cContextVar[0]+1,cContextVar[0]+1);
		fprintf(fp,"\t\t\t\t\" %s:<a href=?gcPage=%s&u%s=%%u>%%s</a>\",\n",cContextVar[1]+1,cContextVar[1]+1,cContextVar[1]+1);
		fprintf(fp,"\t\t\tu%s,cForeignKey(\"t%s\",\"cLabel\",%s),\n",cContextVar[0]+1,cContextVar[0]+1,cContextVar[0]);
		fprintf(fp,"\t\t\tu%s,cForeignKey(\"t%s\",\"cLabel\",%s));\n",cContextVar[1]+1,cContextVar[1]+1,cContextVar[1]);
		fprintf(fp,"\t\tsprintf(gcFilterRows,\"&%s=%%u\",%s);\n",gcTableKey,gcTableKey);
		fprintf(fp,"\t\tsprintf(gcFilterCols,\"&%s=%%u\",%s);\n",gcTableKey,gcTableKey);
		fprintf(fp,"\t\tuSetSessionConfig(\"%s\",%s);\n",gcTableKey,gcTableKey);
		fprintf(fp,"\t\thtml%sFilter();\n",gcTableName);
		fprintf(fp,"\t}\n\n");
		fprintf(fp,"\telse if(%s && %s)\n",cContextVar[0],cContextVar[1]);
		fprintf(fp,"\t{\n");
		fprintf(fp,"\t\tsprintf(gcContext,\"%s:<a href=?gcPage=%s&u%s=%%u>%%s</a>\"\n",cContextVar[0]+1,cContextVar[0]+1,cContextVar[0]+1);
		fprintf(fp,"\t\t\t\t\" %s:<a href=?gcPage=%s&u%s=%%u>%%s</a>\",\n",cContextVar[1]+1,cContextVar[1]+1,cContextVar[1]+1);
		fprintf(fp,"\t\t\tu%s,cForeignKey(\"t%s\",\"cLabel\",%s),\n",cContextVar[0]+1,cContextVar[0]+1,cContextVar[0]);
		fprintf(fp,"\t\t\tu%s,cForeignKey(\"t%s\",\"cLabel\",%s));\n",cContextVar[1]+1,cContextVar[1]+1,cContextVar[1]);
		fprintf(fp,"\t\tsprintf(gcFilterRows,\"&%s=%%u\",%s);\n",cContextVar[0],cContextVar[0]);
		fprintf(fp,"\t\tsprintf(gcFilterCols,\"&%s=%%u\",%s);\n",cContextVar[0],cContextVar[0]);
		fprintf(fp,"\t\tuSetSessionConfig(\"%s\",%s);\n",cContextVar[0],cContextVar[0]);
		fprintf(fp,"\t\thtml%sFilter();\n",gcTableName);
		fprintf(fp,"\t}\n\n");
		}

		sprintf(gcQuery,"SELECT tField.cLabel"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tFieldType.uRADType=%u"
			" AND tField.cOtherOptions LIKE '%%Bootstrap:Context;%%'"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable,COLTYPE_FOREIGNKEY);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
		{
			if(guDebug)
				logfileLine("funcBSGetHookPages",gcQuery);
			fprintf(fp,"%s",mysql_error(&gMysql));
			return;
		}
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
/*
 *         else if(uPaciente)
 *         {
 *         	if(!gcContext[0]) sprintf(gcContext,"Paciente <a href=?gcPage=Paciente&uPaciente=%u>%s</a>",
 *         					uPaciente,ForeignKey("tPaciente","cLabel",uPaciente));
 *         	sprintf(gcFilterRows,"&uPaciente=%u",uPaciente);
 *         	sprintf(gcFilterCols,"&uPaciente=%u",uPaciente);
 *         	uSetSessionConfig("uPaciente",uPaciente);
 *         	uSetSessionConfig("uConsulta",0);
 *         	htmltConsultaFilter();
 *         }
 */

			fprintf(fp,"\telse if(%s)\n"
			"\t{\n"
			"\t\tif(!gcContext[0]) sprintf(gcContext,\"%s <a href=?gcPage=%s&%s=%%u>%%s</a>\",\n"
			"\t\t\t\t%s,cForeignKey(\"t%s\",\"cLabel\",%s));\n"
			"\t\tsprintf(gcFilterRows,\"&%s=%%u\",%s);\n"
			"\t\tsprintf(gcFilterCols,\"&%s=%%u\",%s);\n"
			"\t\tuSetSessionConfig(\"%s\",%s);\n"
			"\t\thtml%sFilter();\n"
			"\t}\n",
				field[0],
				field[0]+1,field[0]+1,field[0],
				field[0],field[0]+1,field[0],
				field[0],field[0],
				field[0],field[0],
				field[0],field[0],
				gcTableName);
		}

	}//All other implicit Bootstrap:Level3

}//void funcBSGetHookPages(FILE *fp)


void funcBSFKJsonRowsSelects(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tFieldType.uRADType=%u"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable,COLTYPE_FOREIGNKEY);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBSFKJsonRowsSelects",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uNotFirst=0;
	while((field=mysql_fetch_row(res)))
	{
/*
 *	else if(uPaciente)
 *		sprintf(cQuery,"SELECT {{funcBootstrapRowReportVars}}"
 *			" FROM {{cTableName}}"
 *			" WHERE uOwner=%1$u AND {{cTableKey}}=%2$u",guOrg,{{cTableKey}});
 *
 *	else if(uPaciente)
 *		sprintf(cQuery,"SELECT 'uConsulta',uConsulta,'cLabel',cLabel,'uPaciente',uPaciente,'cNotas',REPLACE(cNotas,'\n','\\\\n'),'uTipoConsulta',uTipoConsulta,'uVitales',uVitales,'uLaboratorio',uLaboratorio"
 *			" FROM tConsulta"
 *		" WHERE uOwner=%1$u AND uPaciente=%2$u",guOrg,uPaciente);
 */

		if(uNotFirst++) fprintf(fp,"\t");
		fprintf(fp,"else if(%s)\n"
				"\t\tsprintf(cQuery,\"SELECT ",
					field[0]);
		funcBootstrapRowVars(fp);
		fprintf(fp,"\"\n\t\t\" FROM %s\"\n"
				"\t\t\" WHERE uOwner=%%1$u AND %s=%%2$u\",guOrg,%s);\n",
					gcTableName,field[0],field[0]);
	}

}//void funcBSFKJsonRowsSelects(FILE *fp)


void funcBSTemplateFKNVPairs(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tFieldType.uRADType=%u"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable,COLTYPE_FOREIGNKEY);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBSTemplateFKNVPairs",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uNotFirst=0;
	unsigned i=15;
	while((field=mysql_fetch_row(res)))
	{

/*
 *	template.cpName[14]="uPaciente";
 *	char cuPaciente[16];
 *	if(!uPaciente) uPaciente=uGetSessionConfig("uPaciente");
 *	sprintf(cuPaciente,"%u",uPaciente);
 *	template.cpValue[14]=cuPaciente;
 *
 *	template.cpName[15]="";
 */
		if(uNotFirst++) fprintf(fp,"\t\t\t");
		fprintf(fp,"template.cpName[%u]=\"%s\";\n",i,field[0]);
		fprintf(fp,"\t\t\tchar c%s[16];\n",field[0]);
		fprintf(fp,"\t\t\tif(!%1$s) %1$s=uGetSessionConfig(\"%1$s\");\n",field[0]);
		fprintf(fp,"\t\t\tsprintf(c%1$s,\"%%u\",%1$s);\n",field[0]);
		fprintf(fp,"\t\t\ttemplate.cpValue[%u]=c%s;\n\n",i,field[0]);
		fprintf(fp,"\t\t\ttemplate.cpName[%u]=\"\";\n",++i);
	}

}//void funcBSTemplateFKNVPairs(FILE *fp)


void funcBootstrapRetEditor(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tField.cLabel"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tFieldType.uRADType=%u"
			" AND tField.cOtherOptions LIKE '%%FooTable:Report:%%'"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",guTable,COLTYPE_FOREIGNKEY);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBootstrapRetEditor",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uNotFirst=0;
	while((field=mysql_fetch_row(res)))
	{

	

/*
 * 	//table = Consulta.Body field=uAntecedente
 * 	values.uAntecedente= '<a href=?gcPage=Antecedente&uConsulta='+values.uConsulta+'><span class="glyphicon glyphicon-edit"></span></a>';
 */
		if(uNotFirst++) fprintf(fp,"\t\t\t\t\t\t");
		fprintf(fp,"values.%s= '<a href=?gcPage=%s&%s='+values.%s+'><span class=\"glyphicon glyphicon-edit\"></span></a>';\n",
			field[0],field[0]+1,gcTableKey,gcTableKey);
	}

}//void funcBootstrapRetEditor(FILE *fp)


void funcBSLDTables(FILE *fp)
{
	char cTableOptions[64]={""};
	sprintf(cTableOptions,"%.63s",ForeignKey("tTable","cDescription",guTable));
	if(!strstr(cTableOptions,"Bootstrap:Level1;"))
		return;

       	MYSQL_RES *res;
        MYSQL_ROW field;

	//First get the Level2 table
	sprintf(gcQuery,"SELECT tField.cFKSpec"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tFieldType.uRADType=%u"
			" AND tField.cOtherOptions LIKE '%%Bootstrap:Context;Root;%%'",
				guTable,COLTYPE_FOREIGNKEY);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBSLDTables",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	char cLevel2TableName[100]={""};
	unsigned uLevel2Table=0;
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"\"%99[a-zA-Z0-9\\.]\"",cLevel2TableName);
		if(cLevel2TableName[0])
		{
			sprintf(gcQuery,"SELECT uTable FROM tTable"
					" WHERE cLabel='%s' AND cDescription LIKE '%%Bootstrap:Level2;%%'"
					" AND uProject=%u",cLevel2TableName,guProject);
        		mysql_query(&gMysql,gcQuery);
        		if(mysql_errno(&gMysql))
			{
				if(guDebug)
					logfileLine("funcBSLDTables",gcQuery);
               			 fprintf(fp,"%s",mysql_error(&gMysql));
               			 return;
        		}
        		res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sscanf(field[0],"%u",&uLevel2Table);
		}
		else
			return;
	}
	else
		return;

	if(!uLevel2Table)
		return;

	sprintf(gcQuery,"SELECT tField.cLabel"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND tFieldType.uRADType=%u"
			" AND tField.cOtherOptions LIKE '%%FooTable:Report:%%'"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder",uLevel2Table,COLTYPE_FOREIGNKEY);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBSLDTables",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		//fprintf(fp,"Antecedente\",\"CultivoCateter\",\"CultivoEsputo\",\"CultivoHueso\",\"CultivoLCR\",\"CultivoMF\",\"CultivoPB\",\"Hemocultivo\",\"Inmunizacion\",\"Medicacion\",\"Orina\",\"Sangre\",\"Serologia\",\"Urocultivo\",\"Virologia\",\"Vitales\",\"");
		fprintf(fp,"%s\",\"",field[0]+1);
	}

}//void funcBSLDTables(FILE *fp)


void funcBSLDContextVar1(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	//Context generator: Bootstrap table
	sprintf(gcQuery,"SELECT tField.cLabel"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND (tFieldType.uRADType=%u OR tFieldType.uRADType=%u)"
			" AND tField.cOtherOptions LIKE '%%Bootstrap:Context;%%'"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder LIMIT 1",guTable,COLTYPE_FOREIGNKEY,COLTYPE_RADPRI);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBSGetHookPages",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	char cContextVar[64]={"uNoContextVar1"};
	if((field=mysql_fetch_row(res)))
		sprintf(cContextVar,"%.63s",field[0]);

	//fprintf(fp,"uPaciente");
	fprintf(fp,"%s",cContextVar);

}//void funcBSLDContextVar1(FILE *fp)


void funcBSLDContextVar2(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	//Context generator: Bootstrap table
	sprintf(gcQuery,"SELECT tField.cLabel"
			" FROM tField,tFieldType"
			" WHERE tField.uFieldType=tFieldType.uFieldType"
			" AND tField.uTable=%u"
			" AND (tFieldType.uRADType=%u OR tFieldType.uRADType=%u)"
			" AND tField.cOtherOptions LIKE '%%Bootstrap:Context;%%'"
			" AND tField.cLabel!='uOwner'"
			" AND tField.cLabel!='uCreatedBy'"
			" AND tField.cLabel!='uCreatedDate'"
			" AND tField.cLabel!='uModBy'"
			" AND tField.cLabel!='uModDate'"
			" ORDER BY tField.uOrder DESC LIMIT 1",guTable,COLTYPE_FOREIGNKEY,COLTYPE_RADPRI);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		if(guDebug)
			logfileLine("funcBSGetHookPages",gcQuery);
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	char cContextVar[64]={"uNoContextVar2"};
	if((field=mysql_fetch_row(res)))
		sprintf(cContextVar,"%.63s",field[0]);

	//fprintf(fp,"uConsulta");
	fprintf(fp,"%s",cContextVar);

}//void funcBSLDContextVar2(FILE *fp)
