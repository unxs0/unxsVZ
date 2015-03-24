/*
FILE 
	unxsVZ/unxsvz.c
	$Id$
PURPOSE
	Move the jobqueue out of the cgi.
NOTES
AUTHOR/LEGAL
	(C) 2011-2015 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
FREE HELP
	support @ openisp . net
	supportgrp @ unixservice . com
	Join mailing list: https://lists.openisp.net/mailman/listinfo/unxsvz
*/

#include "mysqlrad.h"
#include <ctype.h>

//Global vars

MYSQL gMysql;
MYSQL gMysqlUBC;
char *gcUBCDBIP0;//must be set to DBIP0 to be used as alternative
char *gcUBCDBIP1;
char gcUBCDBIP0Buffer[32]={""};
char gcUBCDBIP1Buffer[32]={""};
FILE *gLfp=NULL;
char gcCommand[100];
char cLogKey[16]={"Ksdj458jssdUjf79"};
char cStartDate[32]={""};
char cStartTime[32]={""};

int guPermLevel=0;
unsigned guLoginClient=0;
unsigned guReseller=0;
unsigned guCompany=0;
char gcCompany[100]={""};
char gcUser[100]={""};
char gcLogin[100]={""};
char gcHost[100]={""};
char gcHostname[100]={""};

unsigned guDatacenter=0;
char gcFunction[100]={""};
char gcQuery[8192]={""};

int main(int iArgc, char *cArgv[])
{
	gethostname(gcHostname,98);

	ExtMainShell(iArgc,cArgv);

	return(0);

}//end of main()

#include "local.h"
#include <dirent.h>
#include <openisp/ucidr.h>
void GetDatacenterProp(const unsigned uDatacenter,const char *cName,char *cValue);//tcontainerfunc.h
void SetContainerStatus(unsigned uContainer,unsigned uStatus);
void ChangeGroup(unsigned uContainer, unsigned uGroup);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);//jobqueue.c
char *strptime(const char *s, const char *format, struct tm *tm);
void TestJob(char const *cJobname);

//jobqueue.c
void ProcessJobQueue(unsigned uDebug);


//here
void ExtMainShell(int argc, char *argv[]);
void AddHardwareNode(char *cDatacenter);

static char cRELEASE[64]={"$Id$"};

void ExtMainShell(int argc, char *argv[])
{
	if(getuid())
	{
		printf("Only root can run this command!\n");
		exit(0);
	}

        if(argc==2 && !strcmp(argv[1],"ProcessJobQueue"))
                ProcessJobQueue(0);
        else if(argc==2 && !strcmp(argv[1],"ProcessJobQueueDebug"))
                ProcessJobQueue(1);
        else if(argc==3 && !strcmp(argv[1],"AddHardwareNode"))
                AddHardwareNode(argv[2]);
        else if(argc==3 && !strcmp(argv[1],"TestJob"))
                TestJob(argv[2]);
        else
	{
		printf("\n%s %s\nMenu\n",argv[0],cRELEASE);
		printf("\tAddHardwareNode <Datacenter cLabel>\n");
		printf("\tProcessJobQueue\n");
		printf("\tProcessJobQueueDebug\n");
		printf("\tTestJob <special cJobname>\n");
		printf("\n");
	}
	mysql_close(&gMysql);
        exit(0);


}//void ExtMainShell(int argc, char *argv[])


void AddHardwareNode(char *cDatacenter)
{
	char cNode[100]={""};
	if(gethostname(cNode,99)!=0)
	{
		printf("gethostname() failed\n");
		exit(1);
	}

	if(TextConnectDb())
		exit(1);

	//Global data
	guLoginClient=2;//ASP uClient

	unsigned uDatacenter=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT uDatacenter FROM tDatacenter WHERE cLabel='%s'",cDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uDatacenter);

	//does datacenter exist?
	if(!uDatacenter)
	{
		//if it does NOT confirm new name.
		char cConfirm[2]={""};
		printf("Confirm new datacenter %s name? [y/n]: ",cDatacenter);
		fscanf(stdin,"%1s",cConfirm);
		if(cConfirm[0]!='y' && cConfirm[0]!='Y')
		{
			printf("Canceling AddHardwareNode()\n");
			return;	
		}
		printf("Adding new datacenter %s\n",cDatacenter);
		//If no such datacenter add it.
		sprintf(gcQuery,"INSERT INTO tDatacenter SET cLabel='%s',"
					"uStatus=1,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",cDatacenter);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			return;
		}
		uDatacenter=mysql_insert_id(&gMysql);
		if(uDatacenter)
		{
			SetDatacenterProp("Contact","Edit ASAP",uDatacenter);
			printf("Basic datacenter properties for %s added\n",cNode);
		}
		else
		{
			printf("Unexpected no uDatacenter error\n");
			return;
		}
	}

	//add tNode
	unsigned uNode=0;
	sprintf(gcQuery,"SELECT uNode FROM tNode WHERE cLabel=SUBSTRING_INDEX('%s','.',1)",cNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uNode);

	if(!uNode)
	{
		sprintf(gcQuery,"INSERT INTO tNode SET cLabel=SUBSTRING_INDEX('%s','.',1),uDatacenter=%u"
				",uStatus=1,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",cNode,uDatacenter);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			return;
		}
		uNode=mysql_insert_id(&gMysql);
		printf("New hardware node for %s added\n",cNode);
	}

	if(uNode)
	{
		SetNodeProp("Hostname",cNode,uNode);
		SetNodeProp("FailoverStatus","Normal",uNode);
		printf("Basic hardware node properties for %s added\n",cNode);
	}

}//void AddHardwareNode(char *cDatacenter)
