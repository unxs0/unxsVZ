/*
FILE
	ubc.c
	$Id$
PURPOSE
	Collection of diverse VZ operating parameters and other system vars.
AUTHOR
	Gary Wallis for Unxiservice (C) 2008-2009. GPL2 License applies.
NOTES
	For latest autonomic functions we need to collect one week of data.
	This weekly data wil be limited to only a few UBC values, and disk usage
	quota data. The basic idea is to provide trend information for better
	informed elastic autonomics.
*/

#include "../../mysqlrad.h"

MYSQL gMysql;
char gcQuery[8192]={""};
unsigned guLoginClient=1;//Root user
char cHostname[100]={""};
char gcProgram[32]={""};
unsigned guNodeOwner=1;
unsigned guContainerOwner=1;
unsigned guStatus=0;//not a valid status

//local protos
void TextConnectDb(void);
void ProcessSingleUBC(unsigned uContainer, unsigned uNode);
void ProcessUBC(void);
void ProcessSingleHDUsage(unsigned uContainer);
void ProcessSingleQuota(unsigned uContainer);
void ProcessSingleStatus(unsigned uContainer);
void ProcessSingleTraffic(unsigned uContainer);
void ProcessVZMemCheck(unsigned uContainer, unsigned uNode);
void ProcessVZCPUCheck(unsigned uContainer, unsigned uNode);
void UpdateContainerUBCJob(unsigned uContainer, char *cResource);
void ProcessSingleTraffic(unsigned uContainer);

static FILE *gLfp;
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

        fprintf(gLfp,"%s %s[%u]: %s\n",cTime,cFunction,pidThis,cLogline);
	fflush(gLfp);

}//void logfileLine(char *cLogline)



int main(int iArgc, char *cArgv[])
{

	sprintf(gcProgram,"%.31s",cArgv[0]);

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
			logfileLine("TextConnectDb","Database server unavailable");
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
	unsigned uType=3;//tContainer type

	sprintf(cContainerTag,"%u:  kmemsize",uContainer);
	if(uContainer)
	{
		//debug only
		//printf("\tProcessSingleUBC(Container=%u)\n",uContainer);
		;
	}
	else if(uNode)
	{
		//debug only
		//printf("\tProcessSingleUBC(Node=%u)\n",uNode);
		uContainer=uNode;
		guContainerOwner=guNodeOwner;
		uType=2;
	}
	else if(1)
	{
		logfileLine("ProcessSingleUBC","No container or node specified");
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
				char cKnownUBCVals[8][32]={"luHeld","luMaxheld","luBarrier",
					"luLimit","luFailcnt","luFailDelta","",""};
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
						logfileLine("ProcessSingleUBC",mysql_error(&gMysql));
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
								"cServer='%s',uOwner=%u,uCreatedBy=1,"
								"uCreatedDate=UNIX_TIMESTAMP(NOW())",
									cResource,luFailcnt,luPrevFailcnt,
									uNode,uContainer,cHostname,guContainerOwner);
								mysql_query(&gMysql,gcQuery);
								if(mysql_errno(&gMysql))
								{
									logfileLine("ProcessSingleUBC",mysql_error(&gMysql));
									exit(2);
								}
								//First autonomic foray
								UpdateContainerUBCJob(uContainer,cResource);
							}
							else
							{
								luKnownUBCVals[5]=0;
							}
						}
						sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
								"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
								" cName='%.63s.%.32s' AND uProperty=%s"
									,luKnownUBCVals[i]
									,guContainerOwner
									,cResource
									,cKnownUBCVals[i]
									,field[0]);
								mysql_query(&gMysql,gcQuery);
								if(mysql_errno(&gMysql))
								{
									logfileLine("ProcessSingleUBC",mysql_error(&gMysql));
									exit(2);
								}
					}
					else
					{
						sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
								",cName='%.63s.%.32s'"
								",uType=%u"
								",uKey=%u"
								",uOwner=%u"
								",uCreatedBy=1"
								",uCreatedDate=UNIX_TIMESTAMP(NOW())"
									,luKnownUBCVals[i]
									,cResource
									,cKnownUBCVals[i]
									,uType
									,uContainer,guContainerOwner);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							logfileLine("ProcessSingleUBC",mysql_error(&gMysql));
							exit(2);
						}
					}
					mysql_free_result(res);
				}
			}
		}
		fclose(fp);
	}
	else
	{
		logfileLine("ProcessSingleUBC","fopen() failed");
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
		logfileLine("ProcessUBC","gethostname() failed");
		exit(1);
	}
	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		logfileLine("ProcessUBC","fopen logfile failed");
		exit(1);
	}
		

	TextConnectDb();//Uses login data from local.h
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT uNode,uDatacenter,uOwner FROM tNode WHERE cLabel='%.99s'",cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessUBC",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
		sscanf(field[2],"%u",&guNodeOwner);
	}
	mysql_free_result(res);
	if(!uNode)
	{
		char *cp;

		//FQDN vs short name of 2nd NIC mess
		if((cp=strchr(cHostname,'.')))
			*cp=0;
		sprintf(gcQuery,"SELECT uNode,uDatacenter,uOwner FROM tNode WHERE cLabel='%.99s'",cHostname);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessUBC",mysql_error(&gMysql));
			mysql_close(&gMysql);
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uNode);
			sscanf(field[1],"%u",&uDatacenter);
			sscanf(field[2],"%u",&guNodeOwner);
		}
		mysql_free_result(res);
	}

	if(!uNode)
	{
		logfileLine("ProcessUBC","Could not determine uNode");
		mysql_close(&gMysql);
		exit(1);
	}

	//debug only
	//printf("ProcessUBC() for %s (uNode=%u,uDatacenter=%u)\n",
	//		cHostname,uNode,uDatacenter);

	//Main loop. TODO use defines for tStatus.uStatus values.
	sprintf(gcQuery,"SELECT uContainer,uOwner,uStatus FROM tContainer WHERE uNode=%u"
				" AND uDatacenter=%u"
				" AND uStatus!=11"//Initial Setup
				" AND uStatus!=6"//Awating Activation
						,uNode,uDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessUBC",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContainer);
		sscanf(field[1],"%u",&guContainerOwner);
		sscanf(field[2],"%u",&guStatus);
		//Job dispatcher based on cJobName
		if(guStatus==uSTOPPED)
		{
			ProcessSingleHDUsage(uContainer);
		}
		else
		{
			ProcessSingleUBC(uContainer,0);
			ProcessSingleQuota(uContainer);
			ProcessSingleStatus(uContainer);
			ProcessSingleHDUsage(uContainer);
			ProcessVZMemCheck(uContainer,0);
			ProcessVZCPUCheck(uContainer,0);
			ProcessSingleTraffic(uContainer);
		}
	}
	mysql_free_result(res);

	//Process  node
	ProcessSingleUBC(0,uNode);
	ProcessVZMemCheck(0,uNode);
	ProcessVZCPUCheck(0,uNode);

	//debug only
	//printf("ProcessUBC() End\n");
	mysql_close(&gMysql);
	exit(0);

}//void ProcessUBC(void)


void ProcessSingleHDUsage(unsigned uContainer)
{
	char cCommand[64];
	char cLine[256];
	unsigned long luUsage=0;
	FILE *fp;

	if(uContainer)
	{
		//debug only
		//printf("\tProcessSingleHDQuota(Container=%u)\n",uContainer);
		;
	}
	else
	{
		logfileLine("ProcessSingleHDUsage","No container specified");
		exit(1);
	}
		
	sprintf(cCommand,"/usr/bin/du -ks /vz/private/%u/ 2> /dev/null",uContainer);

	if((fp=popen(cCommand,"r")))
	{
        	MYSQL_RES *res;
        	MYSQL_ROW field;

		if(fgets(cLine,255,fp)!=NULL)
		{
			sscanf(cLine,"%lu ",&luUsage);
		}
		else
		{
			logfileLine("ProcessSingleHDUsage","No lines from popen");
			exit(1);
		}

		if(luUsage==0)
		{
			logfileLine("ProcessSingleHDUsage","Unexpected luUsage==0");
			exit(1);
		}

		sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='1k-hdblocks.luUsage'"
							" AND uKey=%u AND uType=3",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessSingleHDUsage",mysql_error(&gMysql));
			exit(2);
		}
	       	res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
					"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
					" uProperty=%s"
							,luUsage
							,guContainerOwner
							,field[0]);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleHDUsage",mysql_error(&gMysql));
				exit(2);
			}
		}
		else
		{
			sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
					",cName='1k-hdblocks.luUsage'"
					",uType=3"
					",uKey=%u"
					",uOwner=%u"
					",uCreatedBy=1"
					",uCreatedDate=UNIX_TIMESTAMP(NOW())"
						,luUsage
						,uContainer
						,guContainerOwner);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleHDUsage",mysql_error(&gMysql));
				exit(2);
			}
		}
		mysql_free_result(res);
		pclose(fp);
	}
	else
	{
		logfileLine("ProcessSingleHDUsage","popen() failed");
	}

}//void ProcessSingleHDUsage(unsigned uContainer)


void ProcessSingleQuota(unsigned uContainer)
{
	FILE *fp;
	char cLine[1024];
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
		;
	}
	else
	{
		logfileLine("ProcessSingleQuota","No container specified");
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
					logfileLine("ProcessSingleQuota",mysql_error(&gMysql));
					exit(2);
				}
			       	res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
							"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
							" uProperty=%s"
								,luKnownQuotaVals[i]
								,guContainerOwner
								,field[0]);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						logfileLine("ProcessSingleQuota",mysql_error(&gMysql));
						exit(2);
					}
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
							",cName='%.63s.%.32s'"
							",uType=%u"
							",uKey=%u"
							",uOwner=%u"
							",uCreatedBy=1"
							",uCreatedDate=UNIX_TIMESTAMP(NOW())"
								,luKnownQuotaVals[i]
								,cResource
								,cKnownQuotaVals[i]
								,uType
								,uContainer
								,guContainerOwner);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						logfileLine("ProcessSingleQuota",mysql_error(&gMysql));
						exit(2);
					}
				}
				mysql_free_result(res);
			}

			//Keep one week of usage data for trend analysis
			for(i=0;i<1;i++)
			{
				sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE"
						" cName=CONCAT('%.63s.%.32s.',DAYOFWEEK(NOW()))"
							" AND uKey=%u AND uType=%u",
					cResource,cKnownQuotaVals[i],uContainer,uType);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("ProcessSingleQuota",mysql_error(&gMysql));
					exit(2);
				}
			       	res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					//Average
					sprintf(gcQuery,"UPDATE tProperty SET cValue=CONVERT((%lu+cValue)/2,UNSIGNED)"
							",uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
							" uProperty=%s"
								,luKnownQuotaVals[i]
								,guContainerOwner
								,field[0]);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						logfileLine("ProcessSingleQuota",mysql_error(&gMysql));
						exit(2);
					}
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
							",cName=CONCAT('%.63s.%.32s.',DAYOFWEEK(NOW()))"
							",uType=%u"
							",uKey=%u"
							",uOwner=%u"
							",uCreatedBy=1"
							",uCreatedDate=UNIX_TIMESTAMP(NOW())"
								,luKnownQuotaVals[i]
								,cResource
								,cKnownQuotaVals[i]
								,uType
								,uContainer
								,guContainerOwner);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						logfileLine("ProcessSingleQuota",mysql_error(&gMysql));
						exit(2);
					}
				}
				mysql_free_result(res);
			}
		}
		fclose(fp);
	}
	else
	{
		logfileLine("ProcessSingleQuota","fopen() failed");
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
		//debug only
		//printf("\tProcessVZMemCheck(Container=%u)\n",uContainer);
		sprintf(cContainerTag,"%u",uContainer);
		uType=3;
		uKey=uContainer;
	}
	else if(uNode)
	{
		//debug only
		//printf("\tProcessVZMemCheck(uNode=%u)\n",uNode);
		sprintf(cContainerTag,"Summary:");
		uType=2;
		uKey=uNode;
		guContainerOwner=guNodeOwner;
	}
	else
	{
		logfileLine("ProcessVZMemCheck","No container or node specified");
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
					logfileLine("ProcessVZMemCheck",mysql_error(&gMysql));
					exit(2);
				}
			       	res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					sprintf(gcQuery,"UPDATE tProperty SET cValue=%2.2f,"
							"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
							" cName='%.63s.%.32s' AND uProperty=%s"
								,fKnownVZVals[i]
								,guContainerOwner
								,cResource
								,cKnownVZVals[i]
								,field[0]);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						logfileLine("ProcessVZMemCheck",mysql_error(&gMysql));
						exit(2);
					}
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%2.2f"
							",cName='%.63s.%.32s'"
							",uType=%u"
							",uKey=%u"
							",uOwner=%u"
							",uCreatedBy=1"
							",uCreatedDate=UNIX_TIMESTAMP(NOW())"
								,fKnownVZVals[i]
								,cResource
								,cKnownVZVals[i]
								,uType
								,uKey
								,guContainerOwner);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						logfileLine("ProcessVZMemCheck",mysql_error(&gMysql));
						exit(2);
					}
				}
				mysql_free_result(res);
			}
		}
		fclose(fp);
	}
	else
	{
		logfileLine("ProcessVZMemCheck","popen() failed");
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
		//debug only
		//printf("\tProcessVZCPUCheck(Container=%u)\n",uContainer);
		sprintf(cContainerTag,"%u",uContainer);
		uType=3;
		uKey=uContainer;
	}
	else if(uNode)
	{
		//debug only
		//printf("\tProcessVZCPUCheck(uNode=%u)\n",uNode);
		sprintf(cContainerTag,"0");
		uType=2;
		uKey=uNode;
		guContainerOwner=guNodeOwner;
	}
	else
	{
		logfileLine("ProcessVZCPUCheck","No container or node specified");
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
					logfileLine("ProcessVZCPUCheck",mysql_error(&gMysql));
					exit(2);
				}
			       	res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					sprintf(gcQuery,"UPDATE tProperty SET cValue=%2.2f,"
							"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
							" cName='%.63s.%.32s' AND uProperty=%s"
								,fKnownVZVals[i]
								,guContainerOwner
								,cResource
								,cKnownVZVals[i]
								,field[0]);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						logfileLine("ProcessVZCPUCheck",mysql_error(&gMysql));
						exit(2);
					}
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%2.2f"
							",cName='%.63s.%.32s'"
							",uType=%u"
							",uKey=%u"
							",uOwner=%u"
							",uCreatedBy=1"
							",uCreatedDate=UNIX_TIMESTAMP(NOW())"
								,fKnownVZVals[i]
								,cResource
								,cKnownVZVals[i]
								,uType
								,uKey
								,guContainerOwner);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						logfileLine("ProcessVZCPUCheck",mysql_error(&gMysql));
						exit(2);
					}
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
		logfileLine("ProcessVZCPUCheck","popen() failed");
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
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("UpdateContainerUBCJob",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uDatacenter);
		sscanf(field[1],"%u",&uNode);
	}
	mysql_free_result(res);

	if(!uDatacenter || !uNode)
	{
		logfileLine("UpdateContainerUBCJob","!uDatacenter || !uNode");
		return;
	}

	//Only one waiting job per resource please
	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uContainer=%u"
				" AND uNode=%u AND uDatacenter=%u"
				" AND uJobStatus=1"
				" AND cLabel='UpdateContainerUBCJob()'"
				" AND cJobData='cResource=%s;'"
					,uContainer,uNode,uDatacenter,cResource);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("UpdateContainerUBCJob",mysql_error(&gMysql));
		exit(2);
	}
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
			",uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uDatacenter,uNode,uContainer,cResource,guContainerOwner);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("UpdateContainerUBCJob",mysql_error(&gMysql));
		exit(2);
	}

}//void UpdateContainerUBCJob(...)


//Not uVETH=1 compatible function
void ProcessSingleTraffic(unsigned uContainer)
{
	char cCommand[128];
	char cLine[512];
	unsigned long luIn=0;
	unsigned long luOut=0;
	FILE *fp;

	if(!uContainer)
	{
		logfileLine("ProcessSingleTraffic","No container specified");
		return;
	}
		
	sprintf(cCommand,"/usr/sbin/vzctl exec %u \"grep venet0 /proc/net/dev\" 2> /dev/null",uContainer);

	if((fp=popen(cCommand,"r")))
	{
        	MYSQL_RES *res;
        	MYSQL_ROW field;

		if(fgets(cLine,512,fp)!=NULL)
		{
//Current format
//        1          2       3    4    5     6          7         8  9          10      11   12   13    14      15         16
//venet0: 1053860    5317    0    0    0     0          0         0  9780415    7987    0    0    0     0       0          0
			if(sscanf(cLine,
				"venet0: %lu %*u %*u %*u %*u %*u %*u %*u %lu %*u %*u %*u %*u %*u %*u %*u",
					&luIn,&luOut)!=2)
			{
				logfileLine("ProcessSingleTraffic","sscanf failed");
				//debug only
				printf("sscanf failed\n");
				return;
			}

		}
		else
		{
			logfileLine("ProcessSingleTraffic","No lines from popen");
			return;
		}


		//1-. IN Bytes
		sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='Venet0.luInDelta'"
							" AND uKey=%u AND uType=3",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessSingleTraffic",mysql_error(&gMysql));
			exit(2);
		}
	       	res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			long unsigned luNewInDelta=0;
			long unsigned luPrevIn=0;
			unsigned uProperty=0;
        		MYSQL_RES *res2;
        		MYSQL_ROW field2;

			//New delta is based on current traffic counter and previous Venet0.luOut counter
			sprintf(gcQuery,"SELECT cValue,uProperty FROM tProperty WHERE cName='Venet0.luIn'"
							" AND uKey=%u AND uType=3",uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleTraffic",mysql_error(&gMysql));
				exit(2);
			}
			res2=mysql_store_result(&gMysql);
			if((field2=mysql_fetch_row(res2)))
			{
				sscanf(field2[0],"%lu",&luPrevIn);
				sscanf(field2[1],"%u",&uProperty);
			}
			mysql_free_result(res2);

			if(luIn>luPrevIn)
				luNewInDelta=luIn-luPrevIn;
			else
				luNewInDelta=0;

			//debug only
			//printf("luNewInDelta=%lu = luIn=%lu - luPrevIn=%lu\n",luNewInDelta,luIn,luPrevIn);

			//Update delta
			sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
					"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
					" uProperty=%s"
							,luNewInDelta
							,guContainerOwner
							,field[0]);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleTraffic",mysql_error(&gMysql));
				exit(2);
			}
			//Update traffic counter
			sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
					"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
					" uProperty=%u"
							,luIn
							,guContainerOwner
							,uProperty);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleTraffic",mysql_error(&gMysql));
				exit(2);
			}
		}
		else
		{
			//First sample delta is 0
			sprintf(gcQuery,"INSERT INTO tProperty SET cValue=0"
					",cName='Venet0.luInDelta'"
					",uType=3"
					",uKey=%u"
					",uOwner=%u"
					",uCreatedBy=1"
					",uCreatedDate=UNIX_TIMESTAMP(NOW())"
						,uContainer
						,guContainerOwner);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleTraffic",mysql_error(&gMysql));
				exit(2);
			}

			//First sample
			sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
					",cName='Venet0.luIn'"
					",uType=3"
					",uKey=%u"
					",uOwner=%u"
					",uCreatedBy=1"
					",uCreatedDate=UNIX_TIMESTAMP(NOW())"
						,luIn
						,uContainer
						,guContainerOwner);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleTraffic",mysql_error(&gMysql));
				exit(2);
			}
		}
		mysql_free_result(res);

		//2-. OUT Bytes
		sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='Venet0.luOutDelta'"
							" AND uKey=%u AND uType=3",uContainer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessSingleTraffic",mysql_error(&gMysql));
			exit(2);
		}
	       	res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			long unsigned luNewOutDelta=0;
			long unsigned luPrevOut=0;
			unsigned uProperty=0;
        		MYSQL_RES *res2;
        		MYSQL_ROW field2;

			//New delta is based on current traffic counter and previous Venet0.luOut counter
			sprintf(gcQuery,"SELECT cValue,uProperty FROM tProperty WHERE cName='Venet0.luOut'"
							" AND uKey=%u AND uType=3",uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleTraffic",mysql_error(&gMysql));
				exit(2);
			}
			res2=mysql_store_result(&gMysql);
			if((field2=mysql_fetch_row(res2)))
			{
				sscanf(field2[0],"%lu",&luPrevOut);
				sscanf(field2[1],"%u",&uProperty);
			}
			mysql_free_result(res2);

			if(luOut>luPrevOut)
				luNewOutDelta=luOut-luPrevOut;
			else
				luNewOutDelta=0;

			//debug only
			//printf("luNewOutDelta=%lu = luOut=%lu  - luPrevOut=%lu\n",luNewOutDelta,luOut,luPrevOut);

			//Update delta
			sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
					"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
					" uProperty=%s"
							,luNewOutDelta
							,guContainerOwner
							,field[0]);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleTraffic",mysql_error(&gMysql));
				exit(2);
			}
			//Update traffic counter
			sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
					"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
					" uProperty=%u"
							,luOut
							,guContainerOwner
							,uProperty);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleTraffic",mysql_error(&gMysql));
				exit(2);
			}
		}
		else
		{
			//First sample delta is 0
			sprintf(gcQuery,"INSERT INTO tProperty SET cValue=0"
					",cName='Venet0.luOutDelta'"
					",uType=3"
					",uKey=%u"
					",uOwner=%u"
					",uCreatedBy=1"
					",uCreatedDate=UNIX_TIMESTAMP(NOW())"
						,uContainer
						,guContainerOwner);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleTraffic",mysql_error(&gMysql));
				exit(2);
			}

			//First sample
			sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
					",cName='Venet0.luOut'"
					",uType=3"
					",uKey=%u"
					",uOwner=%u"
					",uCreatedBy=1"
					",uCreatedDate=UNIX_TIMESTAMP(NOW())"
						,luOut
						,uContainer
						,guContainerOwner);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleTraffic",mysql_error(&gMysql));
				exit(2);
			}
		}
		mysql_free_result(res);

		pclose(fp);
	}
	else
	{
		logfileLine("ProcessSingleTraffic","popen() failed");
	}

	//debug only
	//logfileLine("ProcessSingleTraffic","End");

}//void ProcessSingleTraffic(unsigned uContainer)


void ProcessSingleStatus(unsigned uContainer)
{
	FILE *fp;

	if(uContainer)
	{
		;
	}
	else
	{
		logfileLine("ProcessSingleStatus","No container specified");
		exit(1);
	}
		

	//[root@node2vm ~]# cat /proc/vz/veinfo
	// uVEID uNotKnown uProcesses cIP
	//	391     0     6   192.168.22.22
	//	0     0    66

	if((fp=fopen("/proc/vz/veinfo","r")))
	{
        	MYSQL_RES *res;
        	MYSQL_ROW field;
		char cLine[1024];
		char cIP[32];
		unsigned uProcesses;
		unsigned uVEID;

		while(fgets(cLine,1024,fp)!=NULL)
		{
			cIP[0]=0;
			uProcesses=0;
			uVEID=0;

			sscanf(cLine,"%u %*u %u %s",&uVEID,&uProcesses,cIP);

			if(uVEID!=uContainer)
				continue;

			//debug only
			//printf("uContainer=%u uVEID=%u uProcesses=%u cIP=%s\n",
			//		uContainer,uVEID,uProcesses,cIP);

			//1-. vzinfo.uProcesses
			sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='vzinfo.uProcesses'"
							" AND uKey=%u AND uType=3",uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleStatus",mysql_error(&gMysql));
				exit(2);
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(gcQuery,"UPDATE tProperty SET cValue=%u,"
						"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
						" uProperty=%s"
							,uProcesses
							,guContainerOwner
							,field[0]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("ProcessSingleStatus",mysql_error(&gMysql));
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%u"
						",cName='vzinfo.uProcesses'"
						",uType=3"
						",uKey=%u"
						",uOwner=%u"
						",uCreatedBy=1"
						",uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,uProcesses
							,uContainer
							,guContainerOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("ProcessSingleStatus",mysql_error(&gMysql));
					exit(2);
				}
			}
			mysql_free_result(res);

			//1-. vzinfo.cIP
			sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='vzinfo.cIP'"
							" AND uKey=%u AND uType=3",uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessSingleStatus",mysql_error(&gMysql));
				exit(2);
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(gcQuery,"UPDATE tProperty SET cValue='%s',"
						"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
						" uProperty=%s"
							,cIP
							,guContainerOwner
							,field[0]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("ProcessSingleStatus",mysql_error(&gMysql));
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO tProperty SET cValue='%s'"
						",cName='vzinfo.cIP'"
						",uType=3"
						",uKey=%u"
						",uOwner=%u"
						",uCreatedBy=1"
						",uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,cIP
							,uContainer
							,guContainerOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("ProcessSingleStatus",mysql_error(&gMysql));
					exit(2);
				}
			}
			mysql_free_result(res);
		}
	}
	else
	{
		logfileLine("ProcessSingleStatus","fopen() failed");
	}

}//void ProcessSingleStatus(unsigned uContainer)
