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
void GatherHardwareInfo(unsigned uNode);
void AddContainers(void);
void CreateDNSJobAPI(const char *cIPv4,const char *cHostname,const char *cuContainer);

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
        else if(argc==2 && !strcmp(argv[1],"AddContainers"))
                AddContainers();
        else if(argc==3 && !strcmp(argv[1],"TestJob"))
                TestJob(argv[2]);
        else if(argc==5 && !strcmp(argv[1],"CreateDNSJobAPI"))
                CreateDNSJobAPI(argv[2],argv[3],argv[4]);
        else
	{
		printf("\n%s %s\nMenu\n",argv[0],cRELEASE);
		printf("\tAddHardwareNode <Datacenter cLabel>\n");
		printf("\tAddContainers\n");
		printf("\tProcessJobQueue\n");
		printf("\tProcessJobQueueDebug\n");
		printf("\tCreateDNSJobAPI <cIPv4> <cHostname> <uVEID>\n");
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

		GatherHardwareInfo(uNode);
	}

}//void AddHardwareNode(char *cDatacenter)


void GatherHardwareInfo(unsigned uNode)
{

	if(!uNode) return;

	FILE *pfp;
	char cResponse[256]={""};

	sprintf(gcQuery,"grep 'model name' /proc/cpuinfo | head -n 1 | cut -f 2 -d : | tr -d ' ';");
	if((pfp=popen(gcQuery,"r"))!=NULL)
	{
		if(fscanf(pfp,"%255s",cResponse)>0)
		{
			SetNodeProp("cProcCPU",cResponse,uNode);
			printf("cProcCPU=%s added\n",cResponse);
		}
		else
			printf("Error cProcCPU NOT added!\n");
		pclose(pfp);
	}

	unsigned uResponse=0;
	sprintf(gcQuery,"grep -c 'model name' /proc/cpuinfo");
	if((pfp=popen(gcQuery,"r"))!=NULL)
	{
		if(fscanf(pfp,"%u",&uResponse)>0)
		{
			sprintf(cResponse,"%u",uResponse);
			SetNodeProp("uNumCPUs",cResponse,uNode);
			printf("uNumCPUs=%s added\n",cResponse);
		}
		else
			printf("Error uNumCPUs NOT added!\n");
		pclose(pfp);
	}

	long unsigned luResponse=0;
	sprintf(gcQuery,"free | grep Mem: | awk -F' ' '{ print $2 }'");
	if((pfp=popen(gcQuery,"r"))!=NULL)
	{
		if(fscanf(pfp,"%lu",&luResponse)>0)
		{
			sprintf(cResponse,"%lu",luResponse);
			SetNodeProp("luInstalledRAM",cResponse,uNode);
			printf("luInstalledRAM=%s added\n",cResponse);
		}
		else
			printf("Error luInstalledRAM NOT added!\n");
		pclose(pfp);
	}

	sprintf(gcQuery,"df | grep '/vz' | awk -F' ' '{print $2}'");
	if((pfp=popen(gcQuery,"r"))!=NULL)
	{
		if(fscanf(pfp,"%lu",&luResponse)>0)
		{
			sprintf(cResponse,"%lu",luResponse);
			SetNodeProp("luInstalledDiskSpace",cResponse,uNode);
			printf("luInstalledDiskSpace=%s added\n",cResponse);
		}
		else
			printf("Error luInstalledDiskSpace NOT added!\n");
		pclose(pfp);
	}

	sprintf(gcQuery,"ifconfig eth0 | grep HWaddr | awk -F' ' '{print $5}'");
	if((pfp=popen(gcQuery,"r"))!=NULL)
	{
		if(fscanf(pfp,"%255s",cResponse)>0)
		{
			SetNodeProp("cMACeth0",cResponse,uNode);
			printf("cMACeth0=%s added\n",cResponse);
		}
		else
			printf("Error cMACeth0 NOT added!\n");
		pclose(pfp);
	}

	sprintf(gcQuery,"ifconfig eth1 | grep HWaddr | awk -F' ' '{print $5}'");
	if((pfp=popen(gcQuery,"r"))!=NULL)
	{
		if(fscanf(pfp,"%255s",cResponse)>0)
		{
			SetNodeProp("cMACeth1",cResponse,uNode);
			printf("cMACeth1=%s added\n",cResponse);
		}
		else
			printf("Error cMACeth1 NOT added!\n");
		pclose(pfp);
	}

	sprintf(gcQuery,"uname -r");
	if((pfp=popen(gcQuery,"r"))!=NULL)
	{
		if(fscanf(pfp,"%255s",cResponse)>0)
		{
			SetNodeProp("cKernel",cResponse,uNode);
			printf("cKernel=%s added\n",cResponse);
		}
		else
			printf("Error cKernel NOT added!\n");
		pclose(pfp);
	}

	sprintf(gcQuery,"/usr/sbin/dmidecode -s system-manufacturer");
	if((pfp=popen(gcQuery,"r"))!=NULL)
	{
		if(fscanf(pfp,"%255s",cResponse)>0)
		{
			SetNodeProp("cdmiSystemManufacturer",cResponse,uNode);
			printf("cdmiSystemManufacturer=%s added\n",cResponse);
		}
		else
			printf("Error cdmiSystemManufacturer NOT added!\n");
		pclose(pfp);
	}

	sprintf(gcQuery,"/usr/sbin/dmidecode -s system-product-name");
	if((pfp=popen(gcQuery,"r"))!=NULL)
	{
		if(fscanf(pfp,"%255s",cResponse)>0)
		{
			SetNodeProp("cdmiSystemProductName",cResponse,uNode);
			printf("cdmiSystemProductName=%s added\n",cResponse);
		}
		else
			printf("Error cdmiSystemProductName NOT added!\n");
		pclose(pfp);
	}

}//void GatherHardwareInfo(unsigned uNode)


void AddContainers(void)
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


	//Check for node
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uNode=0;
	unsigned uDatacenter=0;
	sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel=SUBSTRING_INDEX('%s','.',1)",cNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
	}

	if(!uNode)
	{
		printf("Hardware node %s not found. First AddHardwareNode if applicable.\n",cNode);
		return;
	}

	FILE *pfp;
	unsigned uContainer=0;
	char cBuf[256]={""};
	char cVal[8][256]={"","","","","","","",""};
	sprintf(gcQuery,"vzlist -H -o veid,hostname,name,ostemplate,ip,nameserver,searchdomain");
	if((pfp=popen(gcQuery,"r"))!=NULL)
	{
		while(fgets(cBuf,255,pfp)!=NULL)
		{
			char *cToken=NULL,*cHostname=NULL,*cName=NULL,*cOSTemplate=NULL,*cIPv4=NULL,*cNameserver=NULL,*cSearchdomain=NULL;
			register int n=0;
			cToken=strtok(cBuf," \n");
			sprintf(cVal[n++],"%.255s",cToken);
			while((cToken=strtok(NULL," \n"))!=NULL && n<7)
			{
				sprintf(cVal[n++],"%.255s",cToken);
			}
			cHostname=cVal[1];
			cName=cVal[2];
			cOSTemplate=cVal[3];
			cIPv4=cVal[4];
			cNameserver=cVal[5];
			cSearchdomain=cVal[6];

			sscanf(cVal[0],"%u",&uContainer);
			if(uContainer && cHostname[0] && cName[0] && cOSTemplate[0] && cIPv4[0])
			{
				//
				//debug only
				printf("Adding veid=%u hostname=\"%s\" name=\"%s\" ostemplate=\"%s\" ip=\"%s\" nameserver=\"%s\" searchdomain=\"%s\"\n",
					uContainer,cHostname,cName,cOSTemplate,cIPv4,cVal[5],cVal[6]);
				//

				//
				//add ip :: must not exist OR be available
				unsigned uIPv4=0;
				sprintf(gcQuery,"SELECT uIP FROM tIP WHERE cLabel='%s' AND uDatacenter=%u",
					cIPv4,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					return;
				}
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
					sscanf(field[0],"%u",&uIPv4);
				if(!uIPv4)
				{
					sprintf(gcQuery,"INSERT INTO tIP SET cLabel='%s',uAvailable=0,"
							"uDatacenter=%u,cComment='AddContainer %s',"
							"uIPNum=INET_ATON('%s'),uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=%u",
								cIPv4,uDatacenter,cNode,cIPv4,guLoginClient,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						printf("%s\n",mysql_error(&gMysql));
						return;
					}
					uIPv4=mysql_insert_id(&gMysql);
				}
				else
				{
					sprintf(gcQuery,"SELECT uIP FROM tIP WHERE uIP=%u AND uAvailable=0",uIPv4);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						printf("%s\n",mysql_error(&gMysql));
						return;
					}
					res=mysql_store_result(&gMysql);
					if((field=mysql_fetch_row(res)))
					{
						printf("The IP being used (%s) is not available. Aborting!\n",cIPv4);
						return;
					}

					//cComment may grow out of control fix later
					sprintf(gcQuery,"UPDATE tIP SET"
							" uAvailable=0,"
							" uModBy=%u,"
							" uModDate=UNIX_TIMESTAMP(NOW()),"
							" cComment=IF(INSTR(cComment,'AddContainer %s'),"
								"cComment,CONCAT(cComment,' AddContainer %s'))"
							" WHERE uIP=%u",guLoginClient,cNode,cNode,uIPv4);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						printf("%s\n",mysql_error(&gMysql));
						return;
					}
				}
				//add ip end
				//

				//
				//add template :: add if does not exist
				unsigned uOSTemplate=0;
				sprintf(gcQuery,"SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='%s'",cOSTemplate);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					return;
				}
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
					sscanf(field[0],"%u",&uOSTemplate);
				if(!uOSTemplate)
				{
					sprintf(gcQuery,"INSERT INTO tOSTemplate SET"
							" cLabel='%s',"
							" uOwner=%u,"
							" uCreatedDate=UNIX_TIMESTAMP(NOW()),"
							" uCreatedBy=%u"
									,cOSTemplate,guLoginClient,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						printf("%s\n",mysql_error(&gMysql));
						return;
					}
					uOSTemplate=mysql_insert_id(&gMysql);
				}
				//add template end
				//
			
				//	
				//add nameserver :: if "-" default (the first?) if not add or use existing match
				unsigned uNameserver=0;
				if(cNameserver[0]=='-')
				{
					sprintf(gcQuery,"SELECT uNameserver FROM tNameserver ORDER BY uNameserver");
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						printf("%s\n",mysql_error(&gMysql));
						return;
					}
					res=mysql_store_result(&gMysql);
					if((field=mysql_fetch_row(res)))
						sscanf(field[0],"%u",&uNameserver);
					if(!uNameserver)
					{
						printf("There is no default Nameserver. Aborting!\n");
						return;
					}
				}
				else
				{
					sprintf(gcQuery,"SELECT uNameserver FROM tNameserver WHERE cLabel='%s'",cNameserver);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						printf("%s\n",mysql_error(&gMysql));
						return;
					}
					res=mysql_store_result(&gMysql);
					if((field=mysql_fetch_row(res)))
						sscanf(field[0],"%u",&uNameserver);
					if(!uNameserver)
					{
						sprintf(gcQuery,"INSERT INTO tNameserver SET"
								" cLabel='%s',"
								" uOwner=%u,"
								" uCreatedDate=UNIX_TIMESTAMP(NOW()),"
								" uCreatedBy=%u"
											,cNameserver,guLoginClient,guLoginClient);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							printf("%s\n",mysql_error(&gMysql));
							return;
						}
						uNameserver=mysql_insert_id(&gMysql);
					}
				}
				//add nameserver end
				//

				//
				//default uConfig
				unsigned uConfig=0;
				sprintf(gcQuery,"SELECT uConfig FROM tConfig ORDER BY uConfig");
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					return;
				}
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
					sscanf(field[0],"%u",&uConfig);
				if(!uConfig)
				{
					printf("There is no default uConfig. Aborting!\n");
					return;
				}
				//default uConfig end
				//

				//
				//add searchdomain :: if "-" default (the first?) if not add or use existing match
				unsigned uSearchdomain=0;
				if(cSearchdomain[0]=='-')
				{
					sprintf(gcQuery,"SELECT uSearchdomain FROM tSearchdomain ORDER BY uSearchdomain");
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						printf("%s\n",mysql_error(&gMysql));
						return;
					}
					res=mysql_store_result(&gMysql);
					if((field=mysql_fetch_row(res)))
						sscanf(field[0],"%u",&uSearchdomain);
					if(!uSearchdomain)
					{
						printf("There is no default Searchdomain. Aborting!\n");
						return;
					}
				}
				else
				{
					sprintf(gcQuery,"SELECT uSearchdomain FROM tSearchdomain WHERE cLabel='%s'",cSearchdomain);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						printf("%s\n",mysql_error(&gMysql));
						return;
					}
					res=mysql_store_result(&gMysql);
					if((field=mysql_fetch_row(res)))
						sscanf(field[0],"%u",&uSearchdomain);
					if(!uNameserver)
					{
						sprintf(gcQuery,"INSERT INTO tSearchdomain SET"
								" cLabel='%s',"
								" uOwner=%u,"
								" uCreatedDate=UNIX_TIMESTAMP(NOW()),"
								" uCreatedBy=%u"
											,cSearchdomain,guLoginClient,guLoginClient);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							printf("%s\n",mysql_error(&gMysql));
							return;
						}
						uSearchdomain=mysql_insert_id(&gMysql);
					}
				}
				//add searchdomain end
				//
				
				//hostname must not exist
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE cHostname='%s'",cHostname);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					return;
				}
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					printf("cHostname %s exists. Aborting!\n",cHostname);
					return;
				}

				//name must not exist
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE cLabel='%s'",cName);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					return;
				}
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					printf("cName %s exists. Aborting!\n",cName);
					return;
				}

				//hostname must be name.something...
				if(strncmp(cHostname,cName,strlen(cName)))
				{
					printf("cHostname %s does not start with cName %s. Aborting!\n",cHostname,cName);
					return;
				}

				//veid must not exist
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uContainer=%u",uContainer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					return;
				}
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					printf("uContainer %u exists. Aborting!\n",uContainer);
					return;
				}

				//
				//timezone :: get from container add if does not exist already.
				char cTimeZone[256]={"CST6CDT"};//default
				GetConfiguration("cTimeZone",cTimeZone,0,0,0,0);//sys conf default global


				//add container
				sprintf(gcQuery,"INSERT INTO tContainer SET"
							" uContainer=%u,"
							" cLabel='%s',"
							" cHostname='%s',"
							" uIPv4=%u,"
							" uOSTemplate=%u,"
							" uConfig=%u,"
							" uNameserver=%u,"
							" uSearchdomain=%u,"
							" uDatacenter=%u,"
							" uNode=%u,"
							" uStatus=%u,"
							" uOwner=%u,"
							" uCreatedDate=UNIX_TIMESTAMP(NOW()),"
							" uCreatedBy=%u"
									,uContainer,cName,cHostname,uIPv4,uOSTemplate,uConfig,uNameserver,uSearchdomain,
									uDatacenter,uNode,uACTIVE,
									guLoginClient,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					return;
				}

				//add properties
				SetContainerProperty(uContainer,"cOrg_TimeZone",cTimeZone);
				SetContainerProperty(uContainer,"unxsVZOPNote","Added by unxsVZ AddContainers");
			}
		}
		pclose(pfp);
	}

}//void AddContainers(void)


void CreateDNSJobAPI(const char *cIPv4,const char *cHostname,const char *cuContainer)
{
	unsigned uContainer=0;
	sscanf(cuContainer,"%u",&uContainer);
	if(!uContainer)
	{
		printf("!uContainer\n");
		exit(1);
	}

	if(TextConnectDb())
	{
		printf("!TextConnectDb()\n");
		exit(1);
	}

	//Check for node
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uNode=0;
	unsigned uDatacenter=0;
	sprintf(gcQuery,"SELECT tContainer.uNode,tContainer.uDatacenter,tContainer.cHostname FROM tContainer"
			" WHERE tContainer.uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
	}

	if(!uNode || !uDatacenter)
	{
		printf("Container not found for VEID %u.\n",uContainer);
		exit(1);
	}

	if(strcmp(cHostname,field[2]))
	{
		printf("Container not found for %s.\n",cHostname);
		exit(1);
	}

	printf("CreateDNSJob(0,1,%s,%s,%u,1,%u,%u)",cIPv4,cHostname,uDatacenter,uContainer,uNode);
	if(!CreateDNSJob(0,1,cIPv4,cHostname,uDatacenter,1,uContainer,uNode))
	{
		printf(" failed\n");
		exit(1);
        }
	printf(" ok\n");
	exit(0);
}//void CreateDNSJobAPI(const char *cIPv4,const char *cHostname,const char *cuContainer)

