/*
FILE
	node.c
	$Id$
AUTHOR
	(C) 2009, Gary Wallis for Unixservice USA
PURPOSE
	unxsvz-autonomics node rule calculation and actions.
NOTES
	1-.  Check fundamental memory constraints of containers per node:
	The memory gap between privvmpages and the two resource guarantees (vmguarpages and
	oomguarpages) is not safe to use in an ongoing basis if the sum of all container
	privvmpages exceeds RAM + swap of the hardware node.
*/

#include "autonomics.h"

//toc
int NodeAutonomics(void);
unsigned uNodeMemConstraints(void);
unsigned uRamUtilConstraints(void);
unsigned uHDUtilConstraints(void);

int NodeAutonomics(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	
	if(!guNode)
		return(0);

	//Load pertinent system settings but only every so often
	//If these change a SIGHUP must be issued to this daemon

	//Global section
	if(!gsAutoState.cNodeAutonomics[0])
	{
		//TODO define 2 type node
		sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
			" cName='Autonomics' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
		mysqlQuery_Err_Exit;
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(gsAutoState.cNodeAutonomics,"%.99s",field[0]);
			sprintf(gcQuery,"cNodeAutonomics=%s",field[0]);
			logfileLine("NodeAutonomics",gcQuery);
		}
		mysql_free_result(res);
	}

	if(gsAutoState.cNodeAutonomics[0]=='Y')
	{

		//Common section
		if(!gsAutoState.cNodeWarnEmail[0])
		{
			//TODO define 2 type node
			sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
				" cName='WarningEmail' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
			mysqlQuery_Err_Exit;
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(gsAutoState.cNodeWarnEmail,"%.99s",field[0]);
				sprintf(gcQuery,"cNodeWarnEmail=%s",gsAutoState.cNodeWarnEmail);
				logfileLine("NodeAutonomics",gcQuery);
			}
			mysql_free_result(res);
		}

		if(!gsAutoState.luNodeInstalledRam)
		{
			//TODO define 2 type node
			sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
				" cName='luInstalledRam' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
			mysqlQuery_Err_Exit;
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%lu",&gsAutoState.luNodeInstalledRam);
				sprintf(gcQuery,"luNodeInstalledRam=%lu",gsAutoState.luNodeInstalledRam);
				logfileLine("NodeAutonomics",gcQuery);
			}
			mysql_free_result(res);
		}

		if(!gsAutoState.luNodeInstalledDiskSpace)
		{
			//TODO define 2 type node
			sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
				" cName='luInstalledDiskSpace' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
			mysqlQuery_Err_Exit;
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%lu",&gsAutoState.luNodeInstalledDiskSpace);
				sprintf(gcQuery,"luNodeInstalledDiskSpace=%lu",gsAutoState.luNodeInstalledDiskSpace);
				logfileLine("NodeAutonomics",gcQuery);
			}
			mysql_free_result(res);
		}
	
	
	
		//PrivMem section
		if(!gsAutoState.uNodePrivPagesWarnRatio)
		{
			//TODO define 2 type node
			sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
				" cName='Autonomics-PrivPagesWarnRatio' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
			mysqlQuery_Err_Exit;
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&gsAutoState.uNodePrivPagesWarnRatio);
				sprintf(gcQuery,"uNodePrivPagesWarnRatio=%u",gsAutoState.uNodePrivPagesWarnRatio);
				logfileLine("NodeAutonomics",gcQuery);
			}
			mysql_free_result(res);
		}
	
		if(!gsAutoState.uNodePrivPagesActRatio)
		{
			//TODO define 2 type node
			sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
				" cName='Autonomics-PrivPagesActRatio' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
			mysqlQuery_Err_Exit;
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&gsAutoState.uNodePrivPagesActRatio);
				sprintf(gcQuery,"uNodePrivPagesActRatio=%u",gsAutoState.uNodePrivPagesActRatio);
				logfileLine("NodeAutonomics",gcQuery);
			}
			mysql_free_result(res);
		}
	
		if(gsAutoState.luNodeInstalledRam && gsAutoState.cNodeAutonomics[0]=='Y')
		{
			if(uNodeMemConstraints())
			{
				Log("uNodeMemConstraints() error");
				logfileLine("NodeAutonomics","uNodeMemConstraints() error");
				//We locally turn off autonomics for this node until
				//the error is fixed. Or the daemon is reloaded via SIGHUP
				gsAutoState.cNodeAutonomics[0]='N';
			}
		}
	
		//RamUtil section
		if(!gsAutoState.uNodeRamUtilWarnRatio)
		{
			//TODO define 2 type node
			sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
				" cName='Autonomics-RamUtilWarnRatio' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
			mysqlQuery_Err_Exit;
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&gsAutoState.uNodeRamUtilWarnRatio);
				sprintf(gcQuery,"uNodeRamUtilWarnRatio=%u",gsAutoState.uNodeRamUtilWarnRatio);
				logfileLine("NodeAutonomics",gcQuery);
			}
			mysql_free_result(res);
		}
	
		if(!gsAutoState.uNodeRamUtilActRatio)
		{
			//TODO define 2 type node
			sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
				" cName='Autonomics-RamUtilActRatio' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
			mysqlQuery_Err_Exit;
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&gsAutoState.uNodeRamUtilActRatio);
				sprintf(gcQuery,"uNodeRamUtilActRatio=%u",gsAutoState.uNodeRamUtilActRatio);
				logfileLine("NodeAutonomics",gcQuery);
			}
			mysql_free_result(res);
		}

		if(gsAutoState.luNodeInstalledRam)
		{
			if(uRamUtilConstraints())
			{
				Log("uRamUtilConstraints() error");
				logfileLine("NodeAutonomics","uRamUtilConstraints() error");
				//We locally turn off autonomics for this node until
				//the error is fixed. Or the daemon is reloaded via SIGHUP
				gsAutoState.cNodeAutonomics[0]='N';
			}
		}

		//HDUtil section
		if(!gsAutoState.uNodeHDUtilWarnRatio)
		{
			//TODO define 2 type node
			sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
				" cName='Autonomics-HDUtilWarnRatio' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
			mysqlQuery_Err_Exit;
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&gsAutoState.uNodeHDUtilWarnRatio);
				sprintf(gcQuery,"uNodeHDUtilWarnRatio=%u",gsAutoState.uNodeHDUtilWarnRatio);
				logfileLine("NodeAutonomics",gcQuery);
			}
			mysql_free_result(res);
		}
	
		if(!gsAutoState.uNodeHDUtilActRatio)
		{
			//TODO define 2 type node
			sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
				" cName='Autonomics-HDUtilActRatio' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
			mysqlQuery_Err_Exit;
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&gsAutoState.uNodeHDUtilActRatio);
				sprintf(gcQuery,"uNodeHDUtilActRatio=%u",gsAutoState.uNodeHDUtilActRatio);
				logfileLine("NodeAutonomics",gcQuery);
			}
			mysql_free_result(res);
		}

		if(gsAutoState.luNodeInstalledDiskSpace)
		{
			if(uHDUtilConstraints())
			{
				Log("uHDUtilConstraints() error");
				logfileLine("NodeAutonomics","uHDUtilConstraints() error");
				//We locally turn off autonomics for this node until
				//the error is fixed. Or the daemon is reloaded via SIGHUP
				gsAutoState.cNodeAutonomics[0]='N';
			}
		}

	}//If autonomics on

	return(0);

}//int NodeAutonomics(void)


unsigned uRamUtilConstraints(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	float fNodeRAMUtil=0.0;
	char cMessage[100];

	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
			" uType=2 AND uKey=%u AND tProperty.cName='vzmemcheck.fRAMUtil'",guNode);
	mysqlQuery_Err_Exit;
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%f",&fNodeRAMUtil);
	mysql_free_result(res);

	//Act 
	if(gsAutoState.uNodeRamUtilActRatio &&
		fNodeRAMUtil>=(float)gsAutoState.uNodeRamUtilActRatio)
	{
		if(!gsAutoState.uNodeRamUtilActedOn)
		{
			sprintf(gcQuery,"fNodeRamUtilActRatio=%2.2f",fNodeRAMUtil);
			logfileLine("uNodeMemConstraints",gcQuery);

			//Send warning email via a forked process
			if(gsAutoState.cNodeWarnEmail[0])
				SendEmail(gsAutoState.cNodeWarnEmail,"NodeRamUtilAct");
			//Create a system message log entry
			sprintf(cMessage,"fNodeRamUtilActRatio=%2.2f node=%u datacenter=%u",
					fNodeRAMUtil,guNode,guDatacenter);
			Log(cMessage);

			gsAutoState.uNodeRamUtilActedOn=1;
		}
	}
	else if(gsAutoState.uNodeRamUtilActRatio)
	{
		gsAutoState.uNodeRamUtilActedOn=0;
	}
	//Warn if not acted on already
	if(!gsAutoState.uNodeRamUtilActedOn && gsAutoState.uNodeRamUtilWarnRatio &&
		fNodeRAMUtil>=(float)gsAutoState.uNodeRamUtilWarnRatio)
	{

		if(!gsAutoState.uNodeRamUtilWarned)
		{
			sprintf(gcQuery,"NodeRamUtilWarnRatio=%2.2f",fNodeRAMUtil);
			logfileLine("uNodeMemConstraints",gcQuery);

			//Send warning email via a forked process
			if(gsAutoState.cNodeWarnEmail[0])
				SendEmail(gsAutoState.cNodeWarnEmail,"NodeRamUtilWarn");
			//Create a system message log entry
			sprintf(cMessage,"fNodeRamUtilWarnRatio=%2.2f node=%u datacenter=%u",
					fNodeRAMUtil,guNode,guDatacenter);
			Log(cMessage);
			gsAutoState.uNodeRamUtilWarned=1;
		}
	}
	else if(!gsAutoState.uNodeRamUtilActedOn && gsAutoState.uNodeRamUtilWarnRatio)
	{
		gsAutoState.uNodeRamUtilWarned=0;
	}

	return(0);

}//unsigned uRamUtilConstraints(void)


unsigned uHDUtilConstraints(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	float fHDUtilRatio;
	float fAllNodeContainerHDBlocks=0.0;
	char cMessage[100];
	long unsigned luUsage;

	sprintf(gcQuery,"SELECT tProperty.cValue FROM tContainer,tProperty WHERE tProperty.uKey=tContainer.uContainer AND"
			" tProperty.uType=3 AND tContainer.uNode=%u AND tProperty.cName='1k-hdblocks.luUsage'"
				" AND tContainer.uDatacenter=%u"
				" AND tContainer.uStatus!=11"//Initial Setup
				" AND tContainer.uStatus!=6"//Awating Activation
						,guNode,guDatacenter);
	mysqlQuery_Err_Exit;
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%lu",&luUsage);
		fAllNodeContainerHDBlocks+=(float)luUsage;
	}
	mysql_free_result(res);

	fHDUtilRatio=fAllNodeContainerHDBlocks/(float)gsAutoState.luNodeInstalledDiskSpace* 100.0;
		
	//Act 
	if(gsAutoState.uNodeHDUtilActRatio &&
		fHDUtilRatio>=(float)gsAutoState.uNodeHDUtilActRatio)
	{
		if(!gsAutoState.uNodeHDUtilActedOn)
		{
			sprintf(gcQuery,"fNodeHDUtilActRatio=%2.2f",fHDUtilRatio);
			logfileLine("uNodeMemConstraints",gcQuery);

			//Send warning email via a forked process
			if(gsAutoState.cNodeWarnEmail[0])
				SendEmail(gsAutoState.cNodeWarnEmail,"NodeHDUtilAct");
			//Create a system message log entry
			sprintf(cMessage,"fNodeHDUtilActRatio=%2.2f node=%u datacenter=%u",
					fHDUtilRatio,guNode,guDatacenter);
			Log(cMessage);

			gsAutoState.uNodeHDUtilActedOn=1;
		}
	}
	else if(gsAutoState.uNodeHDUtilActRatio)
	{
		gsAutoState.uNodeHDUtilActedOn=0;
	}
	//Warn if not acted on already
	if(!gsAutoState.uNodeHDUtilActedOn && gsAutoState.uNodeHDUtilWarnRatio &&
		fHDUtilRatio>=(float)gsAutoState.uNodeHDUtilWarnRatio)
	{

		if(!gsAutoState.uNodeHDUtilWarned)
		{
			sprintf(gcQuery,"NodeHDUtilWarnRatio=%2.2f",fHDUtilRatio);
			logfileLine("uNodeMemConstraints",gcQuery);

			//Send warning email via a forked process
			if(gsAutoState.cNodeWarnEmail[0])
				SendEmail(gsAutoState.cNodeWarnEmail,"NodeHDUtilWarn");
			//Create a system message log entry
			sprintf(cMessage,"fNodeHDUtilWarnRatio=%2.2f node=%u datacenter=%u",
					fHDUtilRatio,guNode,guDatacenter);
			Log(cMessage);
			gsAutoState.uNodeHDUtilWarned=1;
		}
	}
	else if(!gsAutoState.uNodeHDUtilActedOn && gsAutoState.uNodeHDUtilWarnRatio)
	{
		gsAutoState.uNodeHDUtilWarned=0;
	}
	//End use privvmpages ratio action section

	//End use data take node based action section
	//

	return(0);

}//unsigned uHDUtilConstraints(void)


unsigned uNodeMemConstraints(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	FILE *fp;
	char cLine[256];
	char cContainerTag[64];
	char cMessage[100];
	register unsigned uStart=0;
	unsigned long luHeld,luMaxheld,luBarrier,luLimit,luFailcnt;
	char cResource[64];
	unsigned uContainer=0;
	float fPrivPagesRatio;
	float fAllNodeContainerMaxheld=0.0;

	if((fp=fopen("/proc/user_beancounters","r"))==NULL)
		return(8);

	//For each node container
	//Main loop. TODO use defines for tStatus.uStatus values.
	//For efficiency do not search file and parse for containers that should not
	//be running (according to unxsVZ that is ;)).
	sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uNode=%u"
				" AND uDatacenter=%u"
				" AND uStatus!=11"//Initial Setup
				" AND uStatus!=6"//Awating Activation
				" AND uStatus!=31"//Stopped
						,guNode,guDatacenter);
	mysqlQuery_Err_Exit;
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{

		sscanf(field[0],"%u",&uContainer);
		sprintf(cContainerTag,"%u:  kmemsize",uContainer);

		rewind(fp);//for each container start from beginning this seems like
			//it can be optimized.
		while(fgets(cLine,1024,fp)!=NULL)
		{
			cResource[0]=0;
			luHeld=0;
			luMaxheld=0;
			luBarrier=0;
			luLimit=0;
			luFailcnt=0;

			//The first line of a container section has a diff format
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
				uStart++;
			}
			else if(uStart)
			{
				sscanf(cLine,"%s %*u %lu %*u %*u %*u",
							cResource,&luMaxheld);
				if(!strcmp("dummy",cResource))
					continue;
				if(!strcmp(cResource,"privvmpages"))
				{
					//Summation of all container privvmpages max held
					//for this node
					fAllNodeContainerMaxheld=+luMaxheld;

					//debug only
					//sprintf(gcQuery,"uContainer=%u fAllNodeContainerMaxheld=%2.2f",
					//				uContainer,fAllNodeContainerMaxheld);
					//logfileLine("uNodeMemConstraints",gcQuery);

					break;//move on to next container
				}
			}
		}//file line while
	}//SQL while
	mysql_free_result(res);
	fclose(fp);



	//
	//Start use data obtained to take node based action section

	//Start max held privvmpages vs. installed node ram ratio action section
	fPrivPagesRatio=fAllNodeContainerMaxheld/(float)gsAutoState.luNodeInstalledRam* 100.0;
	//if(guDryrun)
	//{
	//	sprintf(gcQuery,"fPrivPagesRatio=%2.2f fAllNodeContainerMaxheld=%2.2f",
	//			fPrivPagesRatio,fAllNodeContainerMaxheld);
	//	logfileLine("uNodeMemConstraints",gcQuery);
	//	sprintf(gcQuery,"uPrivPagesActedOn=%u uPrivPagesWarned=%u",
	//			gsAutoState.uPrivPagesActedOn,gsAutoState.uPrivPagesWarned);
	//	logfileLine("uNodeMemConstraints",gcQuery);
	//}
		
	//Act 
	if(gsAutoState.uNodePrivPagesActRatio &&
		fPrivPagesRatio>=(float)gsAutoState.uNodePrivPagesActRatio)
	{
		if(!gsAutoState.uNodePrivPagesActedOn)
		{
			sprintf(gcQuery,"fNodePrivPagesActRatio=%2.2f",fPrivPagesRatio);
			logfileLine("uNodeMemConstraints",gcQuery);

			//Send warning email via a forked process
			if(gsAutoState.cNodeWarnEmail[0])
				SendEmail(gsAutoState.cNodeWarnEmail,"NodePrivPagesAct");
			//Create a system message log entry
			sprintf(cMessage,"fNodePrivPagesActRatio=%2.2f node=%u datacenter=%u",
					fPrivPagesRatio,guNode,guDatacenter);
			Log(cMessage);

			gsAutoState.uNodePrivPagesActedOn=1;
		}
	}
	else if(gsAutoState.uNodePrivPagesActRatio)
	{
		gsAutoState.uNodePrivPagesActedOn=0;
	}
	//Warn if not acted on already
	if(!gsAutoState.uNodePrivPagesActedOn && gsAutoState.uNodePrivPagesWarnRatio &&
		fPrivPagesRatio>=(float)gsAutoState.uNodePrivPagesWarnRatio)
	{

		if(!gsAutoState.uNodePrivPagesWarned)
		{
			sprintf(gcQuery,"NodePrivPagesWarnRatio=%2.2f",fPrivPagesRatio);
			logfileLine("uNodeMemConstraints",gcQuery);

			//Send warning email via a forked process
			if(gsAutoState.cNodeWarnEmail[0])
				SendEmail(gsAutoState.cNodeWarnEmail,"NodePrivPagesWarn");
			//Create a system message log entry
			sprintf(cMessage,"fNodePrivPagesWarnRatio=%2.2f node=%u datacenter=%u",
					fPrivPagesRatio,guNode,guDatacenter);
			Log(cMessage);
			gsAutoState.uNodePrivPagesWarned=1;
		}
	}
	else if(!gsAutoState.uNodePrivPagesActedOn && gsAutoState.uNodePrivPagesWarnRatio)
	{
		gsAutoState.uNodePrivPagesWarned=0;
	}
	//End use privvmpages ratio action section

	//End use data take node based action section
	//

	return(0);

}//unsigned uNodeMemConstraints(void)


void Log(char *cMessage)
{
        sprintf(gcQuery,"INSERT INTO tLog SET cLabel='autonomics',uLogType=4,uPermLevel=1,uLoginClient=1,cLogin='autonomics',cHost='daemon',cMessage='%s',cServer='%s',cHash=MD5(CONCAT('1','1','autonomics','daemon','%s','%s',UNIX_TIMESTAMP(NOW()),'%s')),uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			cMessage,
			gcHostname,
			cMessage,
			gcHostname,
			gcLogKey);
	mysqlQuery_Err_Exit;

}//void Log(char *cMessage)


//fork the sending
void SendEmail(char *cEmail, char *cSubjectPrefix)
{
	char cSystemCall[256]={""};
	char cLine[256];

	sprintf(cSystemCall,"/bin/true");
	sprintf(cLine,"cEmail:%s SubjectPrefix:%s",cEmail,cSubjectPrefix);
	if(guDryrun)
		logfileLine("SendEmail Attempt",cLine);

	switch(fork())
	{
		default:
			return;

		case -1:
			logfileLine("SendEmail","SendEmail() fork failed");
			_exit(0);

		case 0:
			if(!system(cSystemCall))
				logfileLine("SendEmail",cLine);
			_exit(0);
		break;
	}

}//void SendEmail(char *cEmail, char *cSubjectPrefix)
