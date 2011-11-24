/*
FILE 
	idns-importarpaptrs.c
	$Id$
PURPOSE
	Standalone CLI mass arpa zone PTRs import tool.
LEGAL
	(C) Gary Wallis 2001-2012. All Rights Reserved.
	LICENSE file should be included in distribution.
*/
#include "../../mysqlrad.h"
#include "../../local.h"
#include <dirent.h>
#include <ctype.h>

MYSQL gMysql;
unsigned guMode=0;
unsigned guLine;
unsigned guClient=0;
unsigned guView=0;
char gcQuery[1028];

//TOC
void ConnectDb(void);
unsigned uStrscan(char *cIn,char *cOut[]);
int SubmitJob(const char *cCommand, unsigned uNameServerArg, const char *cZoneArg,
				unsigned uPriorityArg, time_t luTimeArg);
int SubmitSingleJob(const char *cCommand,const char *cZoneArg, unsigned uNameServerArg,
		const char *cTargetServer, unsigned uPriorityArg, time_t luTimeArg
	       			,unsigned *uMasterJob);

int main(int iArgc, char *cArg[])
{

	if(iArgc!=5)
	{
		printf("usage :%s <import file spec> <tClient.uClient> <tView.cLabel>"
				" <mode: test|debug|commit|commitwjobs>\n",cArg[0]);
		exit(0);
	}

	if(strcmp(cArg[4],"test")==0)
		guMode=1;
	else if(strcmp(cArg[4],"debug")==0)
		guMode=2;
	else if(strcmp(cArg[4],"commit")==0)
		guMode=3;
	else if(strcmp(cArg[4],"commitwjobs")==0)
		guMode=4;

	if(!guMode)
	{
		printf("Incorrect mode specified <mode: test|debug|commit|commitwjobs>\n");
		exit(1);
	}

	sscanf(cArg[2],"%u",&guClient);
	if(!guClient)
	{
		printf("Could not sscanf for guClient %s\n",cArg[2]);
		exit(1);
	}

	MYSQL_RES *res;
	MYSQL_ROW field;

	ConnectDb();

	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE uClient=%u",guClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(20);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&guClient);
	mysql_free_result(res);
	if(!guClient)
	{
		printf("No such guClient=%s\n",cArg[2]);
		exit(1);
	}

	sprintf(gcQuery,"SELECT uView FROM tView WHERE cLabel='%s'",cArg[3]);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(20);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&guView);
	mysql_free_result(res);
	if(!guView)
	{
		printf("No such cView=%s\n",cArg[3]);
		exit(1);
	}

	printf("Starting import from file=%s uClient=%u guMode=%u\n",cArg[1],guClient,guMode);
	FILE *fp;

	if(!(fp=fopen(cArg[1],"r")))
	{
		printf("Error: Could not open: %s\n",gcQuery);
		exit(4);
	}

	char cParam1[256]={""};
	char cParam2[256]={""};
	char cParam3[256]={""};
	char cParam4[256]={""};
	static char cName[256]={""};
	static char cZoneName[100]={""};
	static char cPrevZoneName[100]={""};
	static char cPrevcName[100]={""};
	unsigned uItems=0;
	unsigned uZone=0;
	static unsigned uPrevTTL=0;//Has to be reset every new cZoneName
	unsigned uTTL=0;

	while(fgets(gcQuery,1027,fp)!=NULL)
	{
		guLine++;
		//skip empty lines and comments
		if(gcQuery[0]=='\n' || gcQuery[0]==';')
			continue;

		//echo all lines
		if(guMode==2) printf("%s",gcQuery);

		char *cOut[7]={NULL,NULL,NULL,NULL,NULL,NULL,NULL};
		uItems=uStrscan(gcQuery,cOut);
		if(uItems==0)
		{
			if(guMode==2) printf("uItems 0 case1\n");
			continue;
		}
		if(cOut[0]!=NULL)
			sprintf(cName,"%.255s",cOut[0]);
		if(cOut[1]!=NULL)
			sprintf(cParam1,"%.255s",cOut[1]);
		if(cOut[2]!=NULL)
			sprintf(cParam2,"%.255s",cOut[2]);
		if(cOut[3]!=NULL)
			sprintf(cParam3,"%.255s",cOut[3]);
		if(cOut[4]!=NULL)
			sprintf(cParam4,"%.255s",cOut[4]);
		if(guMode==2) printf("cName=%s cParam1=%s cParam2=%s cParam3=%s cParam4=%s\n",cName,cParam1,cParam2,cParam3,cParam4);


		//TODO
		//Initial support must have IN and no TTLs
		if(strcasecmp(cParam2,"PTR"))
			continue;

		//Parse zone name
		unsigned a,b,c,d;
		sscanf(cName,"%u.%u.%u.%u.in-addr.arpa",&d,&c,&b,&a);
		sprintf(cZoneName,"%u.%u.%u.in-addr.arpa",c,b,a);

		static char cPrevOrigin[100]={""};
		if(strcmp(cZoneName,cPrevZoneName))
		{
			sprintf(cPrevZoneName,"%.99s",cZoneName);
			uPrevTTL=0;
			cPrevOrigin[0]=0;
			cPrevcName[0]=0;
			if(guMode==2) printf("cZoneName=%s\n",cZoneName);
		}
		if(guMode<3) printf("%u.%s. IN PTR %s\n",d,cZoneName,cParam3);

		//If zone does not exist create. Add uZone if guMode>=commit
		//Get uZone

		//If RR exists continue

		//Add uResource

		//Depending on what was added and guMode create jobs
	
	}

	fclose(fp);
	return(0);
}//main()


void ConnectDb(void)
{
        mysql_init(&gMysql);
        if(!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
	{
		printf("Could not connect to MySQL server\n");
		exit(0);
	}

}//end of ConnectDb()


char *FQDomainName(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
	{
	
		if(!isalnum(cInput[i]) && cInput[i]!='.'  && cInput[i]!='-' && cInput[i]!='_' && cInput[i]!='@' && cInput[i]!='*')
			break;
		if(isupper(cInput[i])) cInput[i]=tolower(cInput[i]);
	}
	cInput[i]=0;

	return(cInput);

}//char *FQDomainName(char *cInput)


unsigned uStrscan(char *cIn,char *cOut[])
{
	unsigned register j=0,i,uFirstChar=1,uLen,uQuote=0;

	uLen=strlen(cIn);
	for(i=0;i<uLen;i++)
	{
		if(cIn[i]=='\"')
		{
			if(uQuote)
				uQuote=0;
			else
				uQuote=1;
		}

		if(!isspace(cIn[i]))
		{
			if(uFirstChar)
			{
				cOut[j++]=(char *)cIn+i;
				if(j>7) break;
				if(i>255) break;
				uFirstChar=0;
			}
		}
		else
		{
			if(!uQuote)
			{
				cIn[i]=0;
				uFirstChar=1;
			}
		}
	}

	cIn[i]=0;
	return(j);
}//unsigned uStrscan();


int SubmitJob(const char *cCommand, unsigned uNameServerArg, const char *cZoneArg,
				unsigned uPriorityArg, time_t luTimeArg)
{
	MYSQL_RES *res2;
	MYSQL_ROW field;
	static unsigned uMasterJob=0;
	static char cTargetServer[101]={""};

	//Submit one job per EACH NS in the list, group with
	//uMasterJob
	sprintf(gcQuery,"SELECT cList FROM tNameServer WHERE uNameServer=%u",
			uNameServerArg);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		printf("Error %s: %s\n",gcQuery,mysql_error(&gMysql));
		exit(1);
	}
	res2=mysql_store_result(&gMysql);
	
	if((field=mysql_fetch_row(res2)))
	{
		register int i,j=0;

		for(i=0;i<strlen(field[0]);i++)
		{
			if(field[0][i]!='\n' && field[0][i])
			{
				cTargetServer[j++]=field[0][i];
			}
			else
			{
				cTargetServer[j]=0;
				j=0;

				if(SubmitSingleJob(cCommand,cZoneArg,uNameServerArg,
					cTargetServer,uPriorityArg,luTimeArg,&uMasterJob))
				{
					printf("Error %s: %s\n",gcQuery,mysql_error(&gMysql));
					exit(1);
				}
			}//if else
		}//for
	}//if field
	mysql_free_result(res2);

	return(0);

}//int SubmitJob()


int SubmitSingleJob(const char *cCommand,const char *cZoneArg, unsigned uNameServerArg,
		const char *cTargetServer, unsigned uPriorityArg, time_t luTimeArg
	       			,unsigned *uMasterJob)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE cJob='%s' AND cZone='%s' AND uNameServer=%u AND cTargetServer='%s'",
				cCommand,cZoneArg,uNameServerArg,cTargetServer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		printf("Error %s: %s\n",gcQuery,mysql_error(&gMysql));
		exit(1);
	}
	res=mysql_store_result(&gMysql);
	
	if(mysql_num_rows(res)==0)
	{
		unsigned uJob;

		sprintf(gcQuery,"INSERT INTO tJob SET cJob='%s',cZone='%s',"
				"uNameServer=%u,cTargetServer='%s',uPriority=%u,uTime=%lu,"
				"uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			cCommand,
			cZoneArg,
			uNameServerArg,
			cTargetServer,
			uPriorityArg,
			luTimeArg,
			guClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_error(&gMysql)[0])
		{
			mysql_free_result(res);
			return(1);
		}

		if(*uMasterJob == 0)
		{
			uJob=*uMasterJob=mysql_insert_id(&gMysql);
			if(!strstr(cTargetServer,"MASTER"))
			{
				printf("Master must be first\n");
				exit(1);
			}
		}
		else
		{
			uJob=mysql_insert_id(&gMysql);
		}
	
		sprintf(gcQuery,"UPDATE tJob SET uMasterJob=%u WHERE uJob=%u",*uMasterJob,uJob);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql)) 
		{
			printf("Error %s: %s\n",gcQuery,mysql_error(&gMysql));
			exit(1);
		}
		if(mysql_affected_rows(&gMysql)==0)
		{
			if(guMode<3);
				printf("uMasterJob %u",*uMasterJob);
		}
	}
	mysql_free_result(res);

	return(0);

}//int SubmitSingleJob()

