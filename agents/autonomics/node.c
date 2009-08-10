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

int NodeAutonomics(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	
	if(!guNode)
		return(0);

	//Load pertinent system settings but only every so often
	//If these change a SIGHUP must be issued to this daemon
	if(!gcNodeWarnEmail[0])
	{
		//TODO define 2 type node
		sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
			" cName='WarningEmail' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
		mysqlQuery_Err_Exit;
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(gcNodeWarnEmail,"%.99s",field[0]);
			sprintf(gcQuery,"gcNodeWarnEmail=%s",field[0]);
			logfileLine(gcQuery);
		}
		mysql_free_result(res);
	}

	if(!gcNodeInstalledRam[0])
	{
		//TODO define 2 type node
		sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
			" cName='luInstalledRam' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
		mysqlQuery_Err_Exit;
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(gcNodeInstalledRam,"%.99s",field[0]);
			sprintf(gcQuery,"gcNodeInstalledRam=%s",field[0]);
			logfileLine(gcQuery);
		}
		mysql_free_result(res);
	}

	if(!gcNodeAutonomics[0])
	{
		//TODO define 2 type node
		sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
			" cName='Autonomics' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
		mysqlQuery_Err_Exit;
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(gcNodeAutonomics,"%.99s",field[0]);
			sprintf(gcQuery,"gcNodeAutonomics=%s",field[0]);
			logfileLine(gcQuery);
		}
		mysql_free_result(res);
	}

	if(!giAutonomicsPrivPagesWarnRatio)
	{
		//TODO define 2 type node
		sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
			" cName='Autonomics-PrivPagesWarnRatio' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
		mysqlQuery_Err_Exit;
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%d",&giAutonomicsPrivPagesWarnRatio);
			sprintf(gcQuery,"giAutonomicsPrivPagesWarnRatio=%d",giAutonomicsPrivPagesWarnRatio);
			logfileLine(gcQuery);
		}
		mysql_free_result(res);
	}

	if(!giAutonomicsPrivPagesActRatio)
	{
		//TODO define 2 type node
		sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE"
			" cName='Autonomics-PrivPagesActRatio' AND tProperty.uKey=%u AND tProperty.uType=2",guNode);
		mysqlQuery_Err_Exit;
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%d",&giAutonomicsPrivPagesActRatio);
			sprintf(gcQuery,"giAutonomicsPrivPagesActRatio=%d",giAutonomicsPrivPagesActRatio);
			logfileLine(gcQuery);
		}
		mysql_free_result(res);
	}

	if(gcNodeInstalledRam[0] && gcNodeAutonomics[0]=='Y')
	{
		if(iNodeMemConstraints())
		{
			logfileLine("iNodeMemConstraints() error");
			//We locally turn off autonomics for this node until
			//the error is fixed. Or the daemon is reloaded via SIGHUP
			gcNodeAutonomics[0]='N';
		}
	}

	return(0);

}//int NodeAutonomics(void)


unsigned iNodeMemConstraints(void)
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
	long unsigned luInstalledRam=0;
	float fPrivPagesRatio;
	float fAllNodeContainerMaxheld=0.0;

	sscanf(gcNodeInstalledRam,"%lu",&luInstalledRam);
	if(!luInstalledRam)
		return(7);

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
					//logfileLine(gcQuery);

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
	fPrivPagesRatio=fAllNodeContainerMaxheld/(float)luInstalledRam * 100.0;
	//Act 
	if(giAutonomicsPrivPagesActRatio &&
		fPrivPagesRatio>=(float)giAutonomicsPrivPagesActRatio)
	{
		if(!guActedOn)
		{
			sprintf(gcQuery,"act-ratio=%2.2f",fPrivPagesRatio);
			logfileLine(gcQuery);

			//Send warning email via a forked process
			if(gcNodeWarnEmail[0])
				SendPrivPagesEmail(gcNodeWarnEmail,"Act");
			//Create a system message log entry
			sprintf(cMessage,"act-ratio=%2.2f node=%u datacenter=%u",
					fPrivPagesRatio,guNode,guDatacenter);
			Log(cMessage);

			guActedOn=1;
		}
	}
	else if(giAutonomicsPrivPagesActRatio)
	{
		guActedOn=0;
	}
	//Warn if not acted on already
	if(!guActedOn && giAutonomicsPrivPagesWarnRatio &&
		fPrivPagesRatio>=(float)giAutonomicsPrivPagesWarnRatio)
	{

		if(!guWarned)
		{
			sprintf(gcQuery,"warn-ratio=%2.2f",fPrivPagesRatio);
			logfileLine(gcQuery);

			//Send warning email via a forked process
			if(gcNodeWarnEmail[0])
				SendPrivPagesEmail(gcNodeWarnEmail,"Warn");
			//Create a system message log entry
			sprintf(cMessage,"warn-ratio=%2.2f node=%u datacenter=%u",
					fPrivPagesRatio,guNode,guDatacenter);
			Log(cMessage);
			guWarned=1;
		}
	}
	else if(!guActedOn && giAutonomicsPrivPagesWarnRatio)
	{
		guWarned=0;
	}
	//End use privvmpages ratio action section

	//End use data take node based action section
	//

	return(0);

}//unsigned iNodeMemConstraints(void)


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


void SendPrivPagesEmail(char *cEmail, char *cSubjectPrefix)
{
	char cSystemCall[256]={""};

	sprintf(cSystemCall,"sleep 300;touch /tmp/delme.SendPrivPagesEmail.%s.%s",cEmail,cSubjectPrefix);

	switch(fork())
	{
		default:
			return;

		case -1:
			logfileLine("SendPrivPagesEmail() fork failed");
			_exit(0);

		case 0:
			system(cSystemCall);
			_exit(0);
		break;
	}

}//void SendPrivPagesEmail(char *cEmail, char *cSubjectPrefix)
