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

int giAutonomicsPrivPagesWarnRatio=0;//default never
int giAutonomicsPrivPagesActRatio=0;//default never

//This is only temporary per container system must be used. It also must have a reset system.
unsigned guWarned=0;
unsigned guActedOn=0;

int NodeAutonomics(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	//Load pertinent system settings but only every so often
	
	if(!guNode)
		return(0);

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
	register unsigned uStart=0;
	unsigned long luHeld,luMaxheld,luBarrier,luLimit,luFailcnt;
	char cResource[64];
	unsigned uContainer=0;
	long unsigned luInstalledRam=0;
	float fPrivPagesRatio;

	sscanf(gcNodeInstalledRam,"%lu",&luInstalledRam);
	if(!luInstalledRam)
		return(7);

	if((fp=fopen("/proc/user_beancounters","r"))==NULL)
		return(8);


	//For each node container
	//sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uNode=%u AND uDatacenter=%u AND"
	//			" uStatus=1",guNode,guDatacenter);
	sprintf(gcQuery,"SELECT uContainer FROM tContainer");
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
				if(!strcmp(cResource,"privvmpages"))
				{

					fPrivPagesRatio=(float) luMaxheld/luInstalledRam * 100.0;

					if(giAutonomicsPrivPagesWarnRatio &&
						fPrivPagesRatio>=(float)giAutonomicsPrivPagesWarnRatio)
					{

						if(!guWarned)
						{
							//debug only
							sprintf(gcQuery,"warn uContainer=%u %s %lu/%lu ratio=%2.2f",
								uContainer,cResource,luMaxheld,
								luInstalledRam,fPrivPagesRatio);
							logfileLine(gcQuery);
							guWarned=1;
						}
					}
					else
					{
						guWarned=0;
					}

					if(giAutonomicsPrivPagesActRatio &&
						fPrivPagesRatio>=(float)giAutonomicsPrivPagesActRatio)
					{
						if(!guActedOn)
						{
							//debug only
							sprintf(gcQuery,"act uContainer=%u %s %lu/%lu ratio=%2.2f",
								uContainer,cResource,luMaxheld,
								luInstalledRam,fPrivPagesRatio);
							logfileLine(gcQuery);
							guActedOn=1;
						}
					}
					else
					{
						guActedOn=0;
					}

					break;
				}
			}
		}//file line while
	}//SQL while
	mysql_free_result(res);

	fclose(fp);
	return(0);

}//unsigned iNodeMemConstraints(void)

