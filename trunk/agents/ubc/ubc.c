/*
FILE
	ubc.c
	$Id$
PURPOSE
	Collection of diverse VZ operating parameters and other system vars.
AUTHOR
	Gary Wallis for Unxiservice (C) 2008-2009. GPL2 License applies.
NOTES
*/

#include "../../mysqlrad.h"

#define mysqlrad_Query_TextErr_Exit	mysql_query(&gMysql,gcQuery);\
					if(mysql_errno(&gMysql))\
					{\
						printf("%s\n",mysql_error(&gMysql));\
						exit(2);\
					}

MYSQL gMysql;
char gcQuery[8192]={""};
unsigned guLoginClient=1;//Root user
char cHostname[100]={""};

//local protos
void TextConnectDb(void);
void ProcessSingleUBC(unsigned uContainer, unsigned uNode);
void ProcessUBC(void);
void ProcessSingleQuota(unsigned uContainer);
void ProcessVZMemCheck(unsigned uContainer, unsigned uNode);
void ProcessVZCPUCheck(unsigned uContainer, unsigned uNode);
void UpdateContainerUBCJob(unsigned uContainer, char *cResource);

int main()
{
	ProcessUBC();//does vzquota and vzmemcheck also via other subsytems
	return(0);
}//main()


void TextConnectDb(void)
{
        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
        {
        	if (!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
		{
			fprintf(stderr,"Database server unavailable.\n");
			exit(1);
		}
        }
}//void TextConnectDb(void)


void ProcessSingleUBC(unsigned uContainer, unsigned uNode)
{
	FILE *fp;
	char cLine[256];
	char cContainerTag[64];
	register unsigned uStart=0;
	unsigned long luHeld,luMaxheld,luBarrier,luLimit,luFailcnt;
	char cResource[64];
	unsigned uType=3;//tContainter type

	sprintf(cContainerTag,"%u:  kmemsize",uContainer);
	if(uContainer)
	{
		printf("\tProcessSingleUBC(Container=%u)\n",uContainer);
	}
	else if(uNode)
	{
		printf("\tProcessSingleUBC(Node=%u)\n",uNode);
		uContainer=uNode;
		uType=2;
	}
	else if(1)
	{
		printf("\tProcessSingleUBC() Error: No container or node specified.\n");
		exit(1);
	}
		

	if((fp=fopen("/proc/user_beancounters","r")))
	{
		while(fgets(cLine,1024,fp)!=NULL)
		{
			cResource[0]=0;
			luHeld=0;
			luMaxheld=0;
			luBarrier=0;
			luLimit=0;
			luFailcnt=0;

			if(!uStart)
			{
				if(strstr(cLine,cContainerTag))
					uStart=1;
			}
			else
			{
				if(strchr(cLine,':')) break;
			}

			if(uStart==1)
			{
				sprintf(cResource,"kmemsize");
				sscanf(cLine,"%*u:  kmemsize %lu %lu %lu %lu %lu",
					&luHeld,&luMaxheld,&luBarrier,&luLimit,&luFailcnt);
				uStart++;
			}
			else if(uStart)
			{
				sscanf(cLine,"%s %lu %lu %lu %lu %lu",
					cResource,
					&luHeld,&luMaxheld,&luBarrier,&luLimit,&luFailcnt);
				if(!strcmp("dummy",cResource))
					continue;
				uStart++;
			}

			if(uStart)
			{
        			MYSQL_RES *res;
        			MYSQL_ROW field;
				register int i;
				char cKnownUBCVals[8][32]={"luHeld","luMaxheld","luBarrier","luLimit","luFailcnt","luFailDelta","",""};
				long unsigned luKnownUBCVals[8];

				//debug only
				//printf("\t%s luHeld=%lu luMaxheld=%lu luBarrier=%lu luLimit=%lu luFailcnt=%lu\n",
				//	cResource,
				//	luHeld,luMaxheld,luBarrier,luLimit,luFailcnt);
				//update or insert into tProperty for each cResource.lu<value label>
				luKnownUBCVals[0]=luHeld;
				luKnownUBCVals[1]=luMaxheld;
				luKnownUBCVals[2]=luBarrier;
				luKnownUBCVals[3]=luLimit;
				luKnownUBCVals[4]=luFailcnt;
				luKnownUBCVals[5]=0;

				for(i=0;i<6;i++)
				{
					sprintf(gcQuery,"SELECT uProperty,cValue FROM tProperty WHERE cName='%.63s.%.32s'"
								" AND uKey=%u AND uType=%u",
						cResource,cKnownUBCVals[i],uContainer,uType);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						printf("%s\n",mysql_error(&gMysql));
						exit(2);
					}
			        	res=mysql_store_result(&gMysql);
					if((field=mysql_fetch_row(res)))
					{
						//luFailDelta calculation
						//If fails appear at least once per sample time
						//luFailDelta will accumulate. Else will reset.
						//Event is logged. An email or other notice
						//could also be done here.
						if(i==4)
						{	
							unsigned long luPrevFailcnt=0;

							sscanf(field[1],"%lu",&luPrevFailcnt);

							if(luFailcnt>luPrevFailcnt)
							{
								luKnownUBCVals[5]+=luFailcnt;
								//Notify via log system message
								sprintf(gcQuery,"INSERT INTO tLog SET"
								" cLabel='UBC Agent: luFailcnt++',"
								"uLogType=4,uLoginClient=1,"
								"cLogin='UBC Agent',cMessage=\"%s %lu>%lu %u:%u\","
								"cServer='%s',uOwner=1,uCreatedBy=1,"
								"uCreatedDate=UNIX_TIMESTAMP(NOW())",
									cResource,luFailcnt,luPrevFailcnt,
									uNode,uContainer,cHostname);
								mysqlrad_Query_TextErr_Exit;
								//First autonomic foray
								UpdateContainerUBCJob(uContainer,cResource);
							}
							else
							{
								luKnownUBCVals[5]=0;
							}
						}
						sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
								"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1 WHERE"
								" cName='%.63s.%.32s' AND uProperty=%s"
									,luKnownUBCVals[i]
									,cResource
									,cKnownUBCVals[i]
									,field[0]);
						mysqlrad_Query_TextErr_Exit;
					}
					else
					{
						sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
								",cName='%.63s.%.32s'"
								",uType=%u"
								",uKey=%u"
								",uOwner=1"
								",uCreatedBy=1"
								",uCreatedDate=UNIX_TIMESTAMP(NOW())"
									,luKnownUBCVals[i]
									,cResource
									,cKnownUBCVals[i]
									,uType
									,uContainer);
						mysqlrad_Query_TextErr_Exit;
					}
					mysql_free_result(res);
				}
			}
		}
		fclose(fp);
	}
	else
	{
		printf("\tfopen() failed: aborted\n");
	}

}//void ProcessSingleUBC(unsigned uContainer, unsigned uNode)


void ProcessUBC(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uDatacenter=0;
	unsigned uNode=0;
	unsigned uContainer=0;

	if(gethostname(cHostname,99)!=0)
	{
		printf("gethostname() failed: aborted\n");
		exit(1);
	}

	TextConnectDb();//Uses login data from local.h
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
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
	mysql_free_result(res);


	if(!uNode)
	{
		printf("Could not determine uNode: aborted\n");
		exit(1);
	}

	printf("ProcessUBC() for %s (uNode=%u,uDatacenter=%u)\n",
			cHostname,uNode,uDatacenter);

	//Main loop
	sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uNode=%u"
				" AND uDatacenter=%u AND uStatus=1",
						uNode,uDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContainer);
		//Job dispatcher based on cJobName
		ProcessSingleUBC(uContainer,0);
		ProcessSingleQuota(uContainer);
		ProcessVZMemCheck(uContainer,0);
		ProcessVZCPUCheck(uContainer,0);
	}
	mysql_free_result(res);

	//Process  node
	ProcessSingleUBC(0,uNode);
	ProcessVZMemCheck(0,uNode);
	ProcessVZCPUCheck(0,uNode);

	printf("ProcessUBC() End\n");
	exit(0);

}//void ProcessUBC(void)


void ProcessSingleQuota(unsigned uContainer)
{
	FILE *fp;
	char cLine[256];
	char cContainerTag[64];
	register unsigned uStart=0;
	unsigned long luUsage,luSoftlimit,luHardlimit,luTime,luExpire;
	char cResource[64];
	unsigned uType=3;//tContainter type

	//qid: path            usage      softlimit      hardlimit       time     expire
	//111: /vz/private/111
	//  1k-blocks         876364        1048576        1153024          0          0
	//     inodes          29792         200000         220000          0          0


	sprintf(cContainerTag,"%u: /vz/private/%u",uContainer,uContainer);
	if(uContainer)
	{
		printf("\tProcessSingleQuota(Container=%u)\n",uContainer);
	}
	else
	{
		printf("\tProcessSingleQuota() Error: No container specified.\n");
		exit(1);
	}
		

	if((fp=fopen("/proc/vz/vzquota","r")))
	{
		while(fgets(cLine,1024,fp)!=NULL)
		{
			cResource[0]=0;
			luUsage=0;
			luSoftlimit=0;
			luHardlimit=0;
			luTime=0;
			luExpire=0;

			if(!uStart)
			{
				if(strstr(cLine,cContainerTag))
				{
					uStart=1;
					continue;
				}
				continue;
			}
			else
			{
				if(strchr(cLine,':')) break;
			}

			if(uStart==1)
				sprintf(cResource,"1k-blocks");
			else
				sprintf(cResource,"inodes");

				
			sscanf(cLine,"%s %lu %lu %lu %lu %lu",
				cResource,
				&luUsage,&luSoftlimit,&luHardlimit,&luTime,&luExpire);

			//debug only
			//printf("%s %lu %lu %lu %lu %lu\n",
			//	cResource,
			//	luUsage,luSoftlimit,luHardlimit,luTime,luExpire);

        		MYSQL_RES *res;
        		MYSQL_ROW field;
			register int i;
			char cKnownQuotaVals[8][32]={"luUsage","luSoftlimit","luHardlimit","luTime","luExpire"};
			long unsigned luKnownQuotaVals[8];

			luKnownQuotaVals[0]=luUsage;
			luKnownQuotaVals[1]=luSoftlimit;
			luKnownQuotaVals[2]=luHardlimit;
			luKnownQuotaVals[3]=luTime;
			luKnownQuotaVals[4]=luExpire;

			for(i=0;i<5;i++)
			{
				sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='%.63s.%.32s'"
							" AND uKey=%u AND uType=%u",
					cResource,cKnownQuotaVals[i],uContainer,uType);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(2);
				}
			       	res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
							"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1 WHERE"
							" cName='%.63s.%.32s' AND uProperty=%s"
								,luKnownQuotaVals[i]
								,cResource
								,cKnownQuotaVals[i]
								,field[0]);
					mysqlrad_Query_TextErr_Exit;
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
							",cName='%.63s.%.32s'"
							",uType=%u"
							",uKey=%u"
							",uOwner=1"
							",uCreatedBy=1"
							",uCreatedDate=UNIX_TIMESTAMP(NOW())"
								,luKnownQuotaVals[i]
								,cResource
								,cKnownQuotaVals[i]
								,uType
								,uContainer);
					mysqlrad_Query_TextErr_Exit;
				}
				mysql_free_result(res);
			}
		}
		fclose(fp);
	}
	else
	{
		printf("\tfopen() failed: aborted\n");
	}

}//void ProcessSingleQuota(unsigned uContainer)


/*
Output values in %
veid		LowMem          RAM     MemSwap		Alloc
		util  commit    util    util  commit    util  commit   limit
321		0.03    1.74    0.05    0.02    2.41    0.02    2.41    6.48
291		0.06    1.71    0.10    0.03    2.41    0.05    2.41    6.48
	-------------------------------------------------------------------------
Summary:	0.09    3.46    0.15    0.05    4.83    0.07    4.83   12.96
*/
void ProcessVZMemCheck(unsigned uContainer, unsigned uNode)
{
	FILE *fp;
	char cLine[256];
	char cContainerTag[64];
	float fLowMemUtil,fLowMemCommit,fRAMUtil,fMemSwapUtil;
	float fMemSwapCommit,fAllocUtil,fAllocCommit,fAllocLimit;
	char cResource[64]={"vzmemcheck"};
	unsigned uType=0;//tProperty type
	unsigned uKey=0;//tProperty key

	if(uContainer)
	{
		printf("\tProcessVZMemCheck(Container=%u)\n",uContainer);
		sprintf(cContainerTag,"%u",uContainer);
		uType=3;
		uKey=uContainer;
	}
	else if(uNode)
	{
		printf("\tProcessVZMemCheck(uNode=%u)\n",uNode);
		sprintf(cContainerTag,"Summary:");
		uType=2;
		uKey=uNode;
	}
	else
	{
		printf("\tProcessVZMemCheck() Error: No container or node specified.\n");
		exit(1);
	}
		

	if((fp=popen("/usr/sbin/vzmemcheck -v","r")))
	{
		while(fgets(cLine,255,fp)!=NULL)
		{
			fLowMemUtil=0.0;
			fLowMemCommit=0.0;
			fRAMUtil=0.0;
			fMemSwapUtil=0.0;
			fMemSwapCommit=0.0;
			fAllocUtil=0.0;
			fAllocCommit=0.0;
			fAllocLimit=0.0;

			if(strncmp(cLine,cContainerTag,strlen(cContainerTag)))
					continue;

			sscanf(cLine,"%*s %f %f %f %f %f %f %f %f",
				&fLowMemUtil,&fLowMemCommit,&fRAMUtil,&fMemSwapUtil,
				&fMemSwapCommit,&fAllocUtil,&fAllocCommit,&fAllocLimit);

        		MYSQL_RES *res;
        		MYSQL_ROW field;
			register int i;
			char cKnownVZVals[8][32]={"fLowMemUtil","fLowMemCommit","fRAMUtil","fMemSwapUtil",
							"fMemSwapCommit","fAllocUtil","fAllocCommit","fAllocLimit"};
			float fKnownVZVals[8];

			fKnownVZVals[0]=fLowMemUtil;
			fKnownVZVals[1]=fLowMemCommit;
			fKnownVZVals[2]=fRAMUtil;
			fKnownVZVals[3]=fMemSwapUtil;
			fKnownVZVals[4]=fMemSwapCommit;
			fKnownVZVals[5]=fAllocUtil;
			fKnownVZVals[6]=fAllocCommit;
			fKnownVZVals[7]=fAllocLimit;

			for(i=0;i<8;i++)
			{
				sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='%.63s.%.32s'"
							" AND uKey=%u AND uType=%u",
					cResource,cKnownVZVals[i],uKey,uType);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(2);
				}
			       	res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					sprintf(gcQuery,"UPDATE tProperty SET cValue=%2.2f,"
							"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1 WHERE"
							" cName='%.63s.%.32s' AND uProperty=%s"
								,fKnownVZVals[i]
								,cResource
								,cKnownVZVals[i]
								,field[0]);
					mysqlrad_Query_TextErr_Exit;
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%2.2f"
							",cName='%.63s.%.32s'"
							",uType=%u"
							",uKey=%u"
							",uOwner=1"
							",uCreatedBy=1"
							",uCreatedDate=UNIX_TIMESTAMP(NOW())"
								,fKnownVZVals[i]
								,cResource
								,cKnownVZVals[i]
								,uType
								,uKey);
					mysqlrad_Query_TextErr_Exit;
				}
				mysql_free_result(res);
			}
		}
		fclose(fp);
	}
	else
	{
		printf("\tpopen() failed: aborted\n");
	}

}//void ProcessVZMemCheck(unsigned uContainer, unsigned uNode)


/*
VEID            CPUUNITS
-------------------------
0               1000
311             1000
281             1000
Current CPU utilization: 3000
Power of the node: 184838
*/
void ProcessVZCPUCheck(unsigned uContainer, unsigned uNode)
{
	FILE *fp;
	char cLine[256];
	char cContainerTag[64];
	float fCPUUnits;
	char cResource[64]={"vzcpucheck"};
	unsigned uType=0;//tProperty type
	unsigned uKey=0;//tProperty key

	if(uContainer)
	{
		printf("\tProcessVZCPUCheck(Container=%u)\n",uContainer);
		sprintf(cContainerTag,"%u",uContainer);
		uType=3;
		uKey=uContainer;
	}
	else if(uNode)
	{
		printf("\tProcessVZCPUCheck(uNode=%u)\n",uNode);
		sprintf(cContainerTag,"0");
		uType=2;
		uKey=uNode;
	}
	else
	{
		printf("\tProcessVZCPUCheck() Error: No container or node specified.\n");
		exit(1);
	}
		

	if((fp=popen("/usr/sbin/vzcpucheck -v","r")))
	{
		unsigned uPower=0;
		while(fgets(cLine,255,fp)!=NULL)
		{
			fCPUUnits=0.0;

			if(strncmp(cLine,cContainerTag,strlen(cContainerTag)))
					continue;
			if(uPower)
				sscanf(cLine,"Power of the node: %f",&fCPUUnits);
			else
				sscanf(cLine,"%*s %f",&fCPUUnits);

        		MYSQL_RES *res;
        		MYSQL_ROW field;
			register int i;
			char cKnownVZVals[8][32]={"fCPUUnits","","","","","","",""};
			float fKnownVZVals[8];

			fKnownVZVals[0]=fCPUUnits;

			for(i=0;i<1;i++)
			{
				sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='%.63s.%.32s'"
							" AND uKey=%u AND uType=%u",
					cResource,cKnownVZVals[i],uKey,uType);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(2);
				}
			       	res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					sprintf(gcQuery,"UPDATE tProperty SET cValue=%2.2f,"
							"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1 WHERE"
							" cName='%.63s.%.32s' AND uProperty=%s"
								,fKnownVZVals[i]
								,cResource
								,cKnownVZVals[i]
								,field[0]);
					mysqlrad_Query_TextErr_Exit;
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%2.2f"
							",cName='%.63s.%.32s'"
							",uType=%u"
							",uKey=%u"
							",uOwner=1"
							",uCreatedBy=1"
							",uCreatedDate=UNIX_TIMESTAMP(NOW())"
								,fKnownVZVals[i]
								,cResource
								,cKnownVZVals[i]
								,uType
								,uKey);
					mysqlrad_Query_TextErr_Exit;
				}
				mysql_free_result(res);
			}
			//Special dual line for node
			if(uNode)
			{
				uPower=1;
				sprintf(cContainerTag,"Power of the node:");
				sprintf(cResource,"vzcpucheck-nodepwr");
			}
		}
		fclose(fp);
	}
	else
	{
		printf("\tpopen() failed: aborted\n");
	}

}//void ProcessVZCPUCheck(unsigned uContainer, unsigned uNode)


void UpdateContainerUBCJob(unsigned uContainer, char *cResource)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uDatacenter=0;
	unsigned uNode=0;

	if(!uContainer)
		return;

	sprintf(gcQuery,"SELECT uDatacenter,uNode FROM tContainer WHERE uContainer=%u",uContainer);
	mysqlrad_Query_TextErr_Exit;
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uDatacenter);
		sscanf(field[1],"%u",&uNode);
	}
	mysql_free_result(res);

	if(!uDatacenter || !uNode)
	{
		printf("(!uDatacenter || !uNode) error in UpdateContainerUBCJob()\n");
		return;
	}

	//Only one waiting job per resource please
	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uContainer=%u"
				" AND uNode=%u AND uDatacenter=%u"
				" AND uJobStatus=1"
				" AND cLabel='UpdateContainerUBCJob()'"
				" AND cJobData='cResource=%s;'"
					,uContainer,uNode,uDatacenter,cResource);
	mysqlrad_Query_TextErr_Exit;
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		mysql_free_result(res);
		return;
	}
	mysql_free_result(res);
	
	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='UpdateContainerUBCJob()',cJobName='UpdateContainerUBCJob'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+10"
			",uJobStatus=1"
			",cJobData='cResource=%s;'"
			",uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uDatacenter,uNode,uContainer,cResource);
	mysqlrad_Query_TextErr_Exit;

}//void UpdateContainerUBCJob(...)
