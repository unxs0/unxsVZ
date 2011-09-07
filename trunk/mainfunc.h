/*
FILE
	$Id$
PURPOSE
	Included in main.c. For command line interface and html main link.

AUTHOR/LEGAL
	(C) 2001-2011 Gary Wallis for Unixservice, LLC.
	This software distributed under the GPLv2 license.
	See LICENSE file included.
*/


#define macro_mySQLQueryErrorText	mysql_query(&gMysql,gcQuery);\
					if(mysql_errno(&gMysql))\
					{\
						printf("%s\n",mysql_error(&gMysql));\
						printf("</td></tr>\n");\
						CloseFieldSet();\
						return;\
					}\
					mysqlRes=mysql_store_result(&gMysql);

#define macro_mySQLQueryErrorText2	mysql_query(&gMysql,gcQuery);\
					if(mysql_errno(&gMysql))\
					{\
						printf("%s\n",mysql_error(&gMysql));\
						printf("</td></tr>\n");\
						CloseFieldSet();\
						return;\
					}\
					mysqlRes2=mysql_store_result(&gMysql);

#include "local.h"
#include <dirent.h>
#include <openisp/ucidr.h>
void GetDatacenterProp(const unsigned uDatacenter,const char *cName,char *cValue);//tcontainerfunc.h
void SetContainerStatus(unsigned uContainer,unsigned uStatus);
void ChangeGroup(unsigned uContainer, unsigned uGroup);
void CreateDNSJob(unsigned uIPv4,unsigned uOwner,char const *cOptionalIPv4,char const *cHostname,unsigned uDatacenter,unsigned uCreatedBy);
unsigned CommonCloneContainer(
		unsigned uContainer,
		unsigned uOSTemplate,
		unsigned uConfig,
		unsigned uNameserver,
		unsigned uSearchdomain,
		unsigned uDatacenter,
		unsigned uTargetDatacenter,
		unsigned uOwner,
		const char *cLabel,
		unsigned uNode,
		unsigned uStatus,
		const char *cHostname,
		const char *cClassC,
		unsigned uWizIPv4,
		char *cWizLabel,
		char *cWizHostname,
		unsigned uTargetNode,
		unsigned uSyncPeriod,
		unsigned uLoginClient,
		unsigned uCloneStop);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);//jobqueue.c
char *strptime(const char *s, const char *format, struct tm *tm);

static char cTableList[64][32]={ "tAuthorize", "tClient", "tConfig", "tConfiguration", "tContainer",
		"tDatacenter", "tGlossary", "tGroup", "tGroupGlue", "tGroupType", "tIP", "tJob",
		"tJobStatus", "tLog", "tLogMonth", "tLogType", "tMonth", "tNameserver", "tNode",
		"tOSTemplate", "tProperty", "tSearchdomain", "tStatus", "tTemplate", "tTemplateSet",
			"tTemplateType", "tType", ""};

char cInitTableList[64][32]={ "tAuthorize", "tClient", "tConfig", "tGlossary", "tGroupType",
		"tJobStatus", "tLogType", "tOSTemplate", "tStatus", "tTemplate", "tTemplateSet",
			"tTemplateType", "tType", ""};

void ExtMainShell(int argc, char *argv[]);
void Initialize(char *cPasswd);
void Backup(char *cPasswd);
void Restore(char *cPasswd, char *cTableName);
void RestoreAll(char *cPasswd);
void mySQLRootConnect(char *cPasswd);
void ImportTemplateFile(char *cTemplate, char *cFile, char *cTemplateSet, char *cTemplateType);
void ImportOSTemplates(char *cPath,char *cOwner);
void MassCreateContainers(char *cConfigfileName);
void ImportRemoteDatacenter(
			const char *cLocalDatacenter,
			const char *cRemoteDatacenter,
			const char *cLocalNode,
			const char *cRemoteNode,
			const char *cHost,
			const char *cUser,
			const char *cPasswd,
			const char *cuOwner);
void ExtracttLog(char *cMonth, char *cYear, char *cPasswd, char *cTablePath);
time_t cDateToUnixTime(char *cDate);
void CreatetLogTable(char *cTableName);
void NextMonthYear(char *cMonth,char *cYear,char *cNextMonth,char *cNextYear);

void CalledByAlias(int iArgc,char *cArgv[]);
unsigned TextConnectDb(void);//mysqlconnect.c
void DashBoard(const char *cOptionalMsg);
void CloneReport(const char *cOptionalMsg);
void ContainerReport(const char *cOptionalMsg);
void EncryptPasswdMD5(char *pw);
void GetConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer,
		unsigned uHtml);
void UpdateSchema(void);
void RecoverMode(void);
void ResetAllSyncPeriod(void);

//jobqueue.c
void ProcessJobQueue(unsigned uDebug);


int iExtMainCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"MainTools"))
	{
		if(!strcmp(gcCommand,"CloneReport"))
		{
			unxsVZ("CloneReport");
		}
		else if(!strcmp(gcCommand,"ContainerReport"))
		{
			unxsVZ("ContainerReport");
		}
	}
	return(0);
}


void CloneReport(const char *cOptionalMsg)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
        MYSQL_RES *mysqlRes2;
        MYSQL_ROW mysqlField2;

	char cuContainer[16];
	unsigned uContainer;
	unsigned uCount=0;
	char cuSyncPeriod[16];

	//To handle error messages etc.
	if(cOptionalMsg[0] && strcmp(cOptionalMsg,"CloneReport"))
	{
		printf("%s\n",cOptionalMsg);
		return;
	}

	OpenFieldSet("CloneReport",100);

	OpenRow("<u>Containers with no clone</u>","black");
	sprintf(gcQuery,"SELECT tContainer.cLabel,tContainer.cHostname,tContainer.uContainer,tNode.cLabel,tDatacenter.cLabel"
			" FROM tContainer,tNode,tDatacenter"
			" WHERE tContainer.uNode=tNode.uNode AND tContainer.uDatacenter=tDatacenter.uDatacenter"
			" AND tContainer.uSource=0 AND (tContainer.uStatus=1 OR tContainer.uStatus=31)"
			" ORDER BY tContainer.cLabel,tContainer.uDatacenter,tContainer.uNode");
	macro_mySQLQueryErrorText
	printf("</td></tr><tr><td></td><td><u>cLabel</u></td><td><u>cHostname</u></td>"
			"<td><u>Node</td><td><u>Datacenter</u></td>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		cuContainer[0]=0;

		sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uSource=%s",mysqlField[2]);
		macro_mySQLQueryErrorText2
        	if((mysqlField2=mysql_fetch_row(mysqlRes2)))
			sprintf(cuContainer,"%.15s",mysqlField2[0]);
		mysql_free_result(mysqlRes2);

		if(!cuContainer[0])
		{
			uCount++;
			printf("<tr><td></td><td><a href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s</a></td>"
				"<td>%s</td><td>%s</td><td>%s</td>\n",mysqlField[2],
					mysqlField[0],mysqlField[1],mysqlField[3],mysqlField[4]);
		}
		else
		{
			cuSyncPeriod[0]=0;
			sscanf(mysqlField2[0],"%u",&uContainer);
			GetContainerProp(uContainer,"cuSyncPeriod",cuSyncPeriod);
			if(cuSyncPeriod[0] && cuSyncPeriod[0]=='0')
			{
				uCount++;
				printf("<tr><td>Clone w/cuSyncPeriod=0</td><td>"
					"<a href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%u>%s<a></td>"
					"<td>%s</td><td>%s</td><td>%s</td>\n",uContainer,
						mysqlField[0],mysqlField[1],mysqlField[3],mysqlField[4]);
			}
		}
	}
	mysql_free_result(mysqlRes);
	//Lets add a count
	printf("<tr><td>Total %u</td><td></td><td></td><td></td><td></td>\n",uCount);

	//1=Active 31=Stopped TODO
	uCount=0;
	OpenRow("<p>","black");
	OpenRow("<u>Containers with clones not updated in last hour</u>","black");
	sprintf(gcQuery,"SELECT tContainer.cLabel,tContainer.cHostname,tContainer.uContainer,tContainer.uNode,"
				"tContainer.uDatacenter,tDatacenter.cLabel FROM tContainer,tDatacenter WHERE"
				" tContainer.uSource=0 AND (tContainer.uStatus=1 OR tContainer.uStatus=31) AND"
				" tContainer.uDatacenter=tDatacenter.uDatacenter ORDER BY tContainer.cLabel");
	macro_mySQLQueryErrorText
	printf("</td></tr><tr><td></td><td><u>Source cLabel</u></td>"
			"<td><u>Clone cLabel</u><td><u>Source cHostname</u></td>"
			"<td><u>Clone Status/cuSyncPeriod</u></td><td><u>Clone Node</td><td><u>Datacenter</u></td>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sprintf(gcQuery,"SELECT tContainer.uContainer,tStatus.cLabel,tContainer.cLabel,tContainer.cLabel,"
					"tNode.cLabel"
					" FROM tContainer,tStatus,tNode WHERE"
					" tContainer.uSource=%s AND tContainer.uBackupDate<(UNIX_TIMESTAMP(NOW())-3600)"
					" AND tContainer.uStatus=tStatus.uStatus"
					" AND tContainer.uNode=tNode.uNode",
						mysqlField[2]);
		macro_mySQLQueryErrorText2
        	while((mysqlField2=mysql_fetch_row(mysqlRes2)))
		{
			uCount++;
			sscanf(mysqlField2[0],"%u",&uContainer);
			GetContainerProp(uContainer,"cuSyncPeriod",cuSyncPeriod);
			printf("<tr><td></td><td><a href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s</a></td>"
				"<td><a href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s</a></td>"
				"<td>%s</td><td>%s/%s</a>"
				"</td><td>%s</td><td>%s</td>\n",mysqlField[2],mysqlField[0],
								mysqlField2[0],mysqlField2[2],
								mysqlField[1],mysqlField2[1],cuSyncPeriod,
								mysqlField2[4],mysqlField[5]);
		}
		mysql_free_result(mysqlRes2);
	}
	mysql_free_result(mysqlRes);
	//Lets add a count
	printf("<tr><td>Total %u</td><td></td><td></td><td></td><td></td>\n",uCount);

	uCount=0;
	OpenRow("<p>","black");
	OpenRow("<u>Containers with remote clones</u>","black");
	sprintf(gcQuery,"SELECT tContainer.cLabel,tContainer.cHostname,tContainer.uContainer,tContainer.uNode,"
				"tContainer.uDatacenter,tDatacenter.cLabel FROM tContainer,tDatacenter WHERE"
				" tContainer.uSource=0 AND (tContainer.uStatus=1 OR tContainer.uStatus=31) AND"
				" tContainer.uDatacenter=tDatacenter.uDatacenter ORDER BY tContainer.cLabel");
	macro_mySQLQueryErrorText
	printf("</td></tr><tr><td></td><td><u>Source cLabel</u></td>"
			"<td><u>Clone cLabel</u><td><u>Source cHostname</u></td>"
			"<td><u>Clone Status/cuSyncPeriod</u></td><td><u>Source Datacenter</td><td><u>Clone Datacenter</u></td>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sprintf(gcQuery,"SELECT tContainer.uContainer,tStatus.cLabel,tContainer.cLabel,tContainer.cLabel"
					",tDatacenter.cLabel"
					" FROM tContainer,tStatus,tDatacenter WHERE"
					" tContainer.uSource=%s AND tContainer.uDatacenter!=%s"
					" AND tContainer.uStatus=tStatus.uStatus AND tContainer.uDatacenter=tDatacenter.uDatacenter",
						mysqlField[2],mysqlField[4]);
		macro_mySQLQueryErrorText2
        	while((mysqlField2=mysql_fetch_row(mysqlRes2)))
		{
			uCount++;
			sscanf(mysqlField2[0],"%u",&uContainer);
			GetContainerProp(uContainer,"cuSyncPeriod",cuSyncPeriod);
			printf("<tr><td></td><td><a href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s</a></td>"
				"<td><a href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s</a></td>"
				"<td>%s</td><td>%s/%s</a>"
				"</td><td>%s</td><td>%s</td>\n",mysqlField[2],mysqlField[0],
								mysqlField2[0],mysqlField2[2],
								mysqlField[1],mysqlField2[1],cuSyncPeriod,
								mysqlField[5],mysqlField2[4]);
		}
		mysql_free_result(mysqlRes2);
	}
	mysql_free_result(mysqlRes);
	//Lets add a count
	printf("<tr><td>Total %u</td><td></td><td></td><td></td><td></td>\n",uCount);

	CloseFieldSet();

}//void CloneReport(const char *cOptionalMsg)


void ContainerReport(const char *cOptionalMsg)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

	char cStatus[128]="Unknown";
	char cuProcesses[64]="";
	unsigned uContainer=0;
	unsigned uProcesses=0;
	unsigned uStatus=0;

	//To handle error messages etc.
	if(cOptionalMsg[0] && strcmp(cOptionalMsg,"ContainerReport"))
	{
		printf("%s\n",cOptionalMsg);
		return;
	}

	OpenFieldSet("ContainerReport",100);

	OpenRow("<u>Container Comparison Report</u>","black");
	sprintf(gcQuery,"SELECT cLabel,cHostname,uContainer,uNode,uDatacenter,uStatus FROM tContainer "
				" ORDER BY cLabel,uDatacenter,uNode");
	macro_mySQLQueryErrorText
	printf("</td></tr><tr><td>(requires new veinfo.uProcesses)</td><td><u>uContainer</u></td><td><u>cLabel</u></td>"
		"<td><u>cHostname</u>"
		"</td><td><u>Status</u><td><u>Processes</u><td><u>uStatus</u></td></td><td><u>uNode</td>"
		"<td><u>uDatacenter</u></td>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		cuProcesses[0]=0;

		uContainer=0;
		sscanf(mysqlField[2],"%u",&uContainer);
		uStatus=0;
		sscanf(mysqlField[5],"%u",&uStatus);

		if(!uContainer) continue;

		GetContainerProp(uContainer,"veinfo.uProcesses",cuProcesses);
		if(cuProcesses[0])
		{
			uProcesses=0;
			sscanf(cuProcesses,"%u",&uProcesses);

			if(uProcesses && uStatus==1)
				sprintf(cStatus,"Running");
			else if(uProcesses==0 && uStatus==31)
				sprintf(cStatus,"Stopped");
			else if(uProcesses && uStatus==31)
				sprintf(cStatus,"<font color=red>Inconsistency: Running but should not be!</font>");
			else if(uProcesses==0 && uStatus==1)
				sprintf(cStatus,"<font color=red>Inconsistency: Stopped but should not be!</font>");
			else if(uProcesses && uStatus!=1)
				sprintf(cStatus,"Transitional state");
			else if(1)
				sprintf(cStatus,"Unexpected status");
	
			printf("<tr><td></td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%u</td><td>%u</td><td>%s</td>"
						"<td>%s</td>\n",
						mysqlField[2],mysqlField[0],mysqlField[1],
						cStatus,
						uProcesses,
						uStatus,
						mysqlField[3],mysqlField[4]);
		}
	}
	mysql_free_result(mysqlRes);


	CloseFieldSet();

}//void ContainerReport(const char *cOptionalMsg)


void DashBoard(const char *cOptionalMsg)
{

        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	time_t luClock;
	char cConfigBuffer[256]={""};

	long unsigned luTotalFailcnt=1;
	long unsigned luFailcnt=0;
	long unsigned luTotalUsage=1,luTotalSoftLimit=1;
	double fRatio;
	char *cColor={""};

	//To handle error messages etc.
	if(cOptionalMsg[0] && strcmp(cOptionalMsg,"DashBoard"))
	{
		printf("%s\n",cOptionalMsg);
		return;
	}

	OpenFieldSet("Dashboard",100);

	GetConfiguration("DashGraph0",cConfigBuffer,0,0,0,0);//any server, txt error msg
	if(cConfigBuffer[0])
	{
		char cURL[256]={""};

		OpenRow("DashGraphs","black");
		printf("</td></tr>\n");

		GetConfiguration("DashGraph0URL",cURL,0,0,0,0);
		if(cConfigBuffer[0] && cURL[0])
			printf("<tr><td></td><td colspan=3><a href=%s><img src=%s border=0></a>\n",
					cURL,cConfigBuffer);
		else if(cConfigBuffer[0])
			printf("<tr><td></td><td colspan=3><img src=%s>\n",cConfigBuffer);

		cURL[0]=0;
		cConfigBuffer[0]=0;
		GetConfiguration("DashGraph1",cConfigBuffer,0,0,0,0);
		GetConfiguration("DashGraph1URL",cURL,0,0,0,0);
		if(cConfigBuffer[0] && cURL[0])
			printf("<a href=%s><img src=%s border=0></a>\n",cURL,cConfigBuffer);
		else if(cConfigBuffer[0])
			printf("<img src=%s>\n",cConfigBuffer);

		cURL[0]=0;
		cConfigBuffer[0]=0;
		GetConfiguration("DashGraph2",cConfigBuffer,0,0,0,0);
		GetConfiguration("DashGraph2URL",cURL,0,0,0,0);
		if(cConfigBuffer[0] && cURL[0])
			printf("<a href=%s><img src=%s border=0></a>\n",cURL,cConfigBuffer);
		else if(cConfigBuffer[0])
			printf("<img src=%s>\n",cConfigBuffer);


		printf("</td>");
	}



	//
	OpenRow("Global Cluster Health","black");
	sprintf(gcQuery,"SELECT COUNT(uProperty) FROM tProperty WHERE cName LIKE '%%.luFailDelta'");
	macro_mySQLQueryErrorText
	printf("</td></tr>\n");
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[0],"%lu",&luTotalFailcnt);
	}
	mysql_free_result(mysqlRes);

	sprintf(gcQuery,"SELECT COUNT(uProperty) FROM tProperty WHERE cName LIKE '%%.luFailDelta' AND cValue!='0'");
	macro_mySQLQueryErrorText
	printf("</td></tr>\n");
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[0],"%lu",&luFailcnt);
	}
	mysql_free_result(mysqlRes);
	if(luTotalFailcnt==0) luTotalFailcnt=1;
	fRatio= ((float) luFailcnt / (float) luTotalFailcnt) * 100.00;
	if(fRatio==0.0)
		cColor="green";
	else if(fRatio<1.5)
		cColor="teal";
	else if(fRatio<2.0)
		cColor="yellow";
	else if(fRatio<5.0)
		cColor="fuchsia";
	else if(fRatio>=5.0)
		cColor="red";
	printf("<tr><td></td><td>Current luFailDelta Ratio %2.2f%%</td><td>%lu/%lu</td><td bgcolor=%s colspan=2></td></tr>\n",
		fRatio,luFailcnt,luTotalFailcnt,cColor);
	//


	//
	luTotalFailcnt=1;
	sprintf(gcQuery,"SELECT COUNT(uProperty) FROM tProperty WHERE cName LIKE '%%.luFailcnt'");
	macro_mySQLQueryErrorText
	printf("</td></tr>\n");
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[0],"%lu",&luTotalFailcnt);
	}
	mysql_free_result(mysqlRes);

	//sprintf(gcQuery,"SELECT COUNT(uProperty) FROM tProperty WHERE cName LIKE '%%.luFailcnt' AND cValue!='0'");
	sprintf(gcQuery,"select count(uLog) from tLog where cMessage like '%%>%% %%:%%'"
				" and uCreatedDate>(unix_timestamp(now())-(86400*7))");
	macro_mySQLQueryErrorText
	printf("</td></tr>\n");
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[0],"%lu",&luFailcnt);
	}	
	if(luTotalFailcnt==0) luTotalFailcnt=1;
	mysql_free_result(mysqlRes);
	if(luTotalFailcnt==0) luTotalFailcnt=1;
	fRatio= ((float) luFailcnt / (float) luTotalFailcnt) * 100.00;
	if(fRatio<1.0)
		cColor="green";
	else if(fRatio<2.0)
		cColor="teal";
	else if(fRatio<5.0)
		cColor="yellow";
	else if(fRatio<10.0)
		cColor="fuchsia";
	else if(fRatio>=10.0)
		cColor="red";
	printf("<tr><td></td><td>Last 7 Day <i>failcnt</i> Ratio %2.2f%%</td><td>%lu/%lu</td><td bgcolor=%s colspan=2></td></tr>\n",
		fRatio,luFailcnt,luTotalFailcnt,cColor);
	//


	sprintf(gcQuery,"SELECT SUM(CONVERT(cValue,UNSIGNED)) FROM tProperty WHERE cName='1k-blocks.luUsage'");
	macro_mySQLQueryErrorText
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		if(mysqlField[0]!=NULL)
			sscanf(mysqlField[0],"%lu",&luTotalUsage);
	}
	mysql_free_result(mysqlRes);

	sprintf(gcQuery,"SELECT SUM(CONVERT(cValue,UNSIGNED)) FROM tProperty WHERE cName='1k-blocks.luSoftlimit'");
	macro_mySQLQueryErrorText
        if((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		if(mysqlField[0]!=NULL)
			sscanf(mysqlField[0],"%lu",&luTotalSoftLimit);
	}
	mysql_free_result(mysqlRes);
	if(luTotalSoftLimit==0) luTotalSoftLimit=1;
	fRatio= ((float) luTotalUsage/ (float) luTotalSoftLimit) * 100.00 ;
	if(fRatio<20.00)
		cColor="green";
	else if(fRatio<30.00)
		cColor="teal";
	else if(fRatio<40.00)
		cColor="yellow";
	else if(fRatio<50.00)
		cColor="fuchsia";
	else if(fRatio>=50.00)
		cColor="red";

	if(luTotalSoftLimit==1)
		printf("<tr><td></td><td>No Container Usage Ratio</td>"
		"<td>%luG/%luG</td><td bgcolor=green colspan=2></td></tr>\n",
		luTotalUsage/1048576,luTotalSoftLimit/1048576);
	else
		printf("<tr><td></td><td>All Container Usage Ratio %2.2f%%</td>"
		"<td>%luG/%luG</td><td bgcolor=%s colspan=2></td></tr>\n",
		fRatio,luTotalUsage/1048576,luTotalSoftLimit/1048576,cColor);
	//
	//


	if(guPermLevel>11 && guLoginClient==1)
	{
		OpenRow("System Messages (Last 10)","black");
		sprintf(gcQuery,"SELECT cMessage,GREATEST(uCreatedDate,uModDate),cServer FROM tLog"
				" WHERE uLogType=4 ORDER BY GREATEST(uCreatedDate,uModDate) DESC LIMIT 10");
		macro_mySQLQueryErrorText
		printf("</td></tr>\n");
		while((mysqlField=mysql_fetch_row(mysqlRes)))
		{
			sscanf(mysqlField[1],"%lu",&luClock);
			printf("<tr><td></td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
					ctime(&luClock),mysqlField[0],mysqlField[2]);
		}
		mysql_free_result(mysqlRes);


		OpenRow("tLog (Last 10)","black");
		sprintf(gcQuery,"SELECT tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),tLog.cLogin,"
				"tLog.cTableName,tLog.cHost,tLogType.cLabel FROM tLog,tLogType WHERE "
				"tLog.uLogType=tLogType.uLogType AND tLog.uLogType!=4 AND tLog.uLogType!=6 "
				"ORDER BY GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC LIMIT 10");
		macro_mySQLQueryErrorText
		printf("</td></tr>\n");
	        while((mysqlField=mysql_fetch_row(mysqlRes)))
		{
			sscanf(mysqlField[1],"%lu",&luClock);
			printf("<tr><td></td><td>%s</td><td>%s %s</td><td>%s %s</td><td>%s</td></tr>\n",
				ctime(&luClock),mysqlField[0],mysqlField[3],mysqlField[5],mysqlField[2],mysqlField[4]);
		}
		mysql_free_result(mysqlRes);

		OpenRow("Login Activity (Last 10)","black");
		sprintf(gcQuery,"SELECT cLabel,GREATEST(uCreatedDate,uModDate),cServer,cHost FROM"
				" tLog WHERE uLogType=6 ORDER BY GREATEST(uCreatedDate,uModDate)"
				" DESC LIMIT 10");
		macro_mySQLQueryErrorText
		printf("</td></tr>\n");
	        while((mysqlField=mysql_fetch_row(mysqlRes)))
		{
			sscanf(mysqlField[1],"%lu",&luClock);
			printf("<td></td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
				ctime(&luClock),mysqlField[0],mysqlField[2],mysqlField[3]);
		}
		mysql_free_result(mysqlRes);
	}
	else
	{
		OpenRow("System Messages (Last 10)","black");
		sprintf(gcQuery,"SELECT cMessage,GREATEST(uCreatedDate,uModDate),cServer FROM tLog"
				" WHERE uLogType=4 AND uOwner=%u ORDER BY GREATEST(uCreatedDate,uModDate)"
				" DESC LIMIT 10",guCompany);
		macro_mySQLQueryErrorText
		printf("</td></tr>\n");
		while((mysqlField=mysql_fetch_row(mysqlRes)))
		{
			sscanf(mysqlField[1],"%lu",&luClock);
			printf("<tr><td></td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
					ctime(&luClock),mysqlField[0],mysqlField[2]);
		}
		mysql_free_result(mysqlRes);


		OpenRow("tLog (Last 10)","black");
		sprintf(gcQuery,"SELECT tLog.cLabel,GREATEST(tLog.uCreatedDate,tLog.uModDate),tLog.cLogin,"
				"tLog.cTableName,tLog.cHost,tLogType.cLabel FROM tLog,tLogType WHERE "
				"tLog.uLogType=tLogType.uLogType AND tLog.uLogType!=4 AND tLog.uLogType!=6 AND"
				" tLog.uOwner=%u ORDER BY GREATEST(tLog.uCreatedDate,tLog.uModDate) DESC LIMIT 10",
						guCompany);
		macro_mySQLQueryErrorText
		printf("</td></tr>\n");
	        while((mysqlField=mysql_fetch_row(mysqlRes)))
		{
			sscanf(mysqlField[1],"%lu",&luClock);
			printf("<tr><td></td><td>%s</td><td>%s %s</td><td>%s %s</td><td>%s</td></tr>\n",
				ctime(&luClock),mysqlField[0],mysqlField[3],mysqlField[5],mysqlField[2],mysqlField[4]);
		}
		mysql_free_result(mysqlRes);

		OpenRow("Login Activity (Last 10)","black");
		sprintf(gcQuery,"SELECT cLabel,GREATEST(uCreatedDate,uModDate),cServer,cHost FROM"
				" tLog WHERE uLogType=6 AND uOwner=%u ORDER BY GREATEST(uCreatedDate,uModDate)"
				" DESC LIMIT 10",guCompany);
		macro_mySQLQueryErrorText
		printf("</td></tr>\n");
	        while((mysqlField=mysql_fetch_row(mysqlRes)))
		{
			sscanf(mysqlField[1],"%lu",&luClock);
			printf("<td></td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
				ctime(&luClock),mysqlField[0],mysqlField[2],mysqlField[3]);
		}
		mysql_free_result(mysqlRes);
	}//end of if(guPermLevel>11 ...)

	OpenRow("Pending or Stuck Jobs (Last 10)","black");
	if(guPermLevel>11 && guLoginClient==1)
		sprintf(gcQuery,"SELECT tJob.cLabel,GREATEST(tJob.uCreatedDate,tJob.uModDate),tNode.cLabel,tJobStatus.cLabel"
			" FROM tJob,tJobStatus,tNode WHERE tJob.uNode=tNode.uNode AND"
			" tJob.uJobStatus=tJobStatus.uJobStatus AND tJob.uJobStatus!=3"
			" AND tJob.uJobStatus!=7 AND tJob.uJobStatus!=6"
			" ORDER BY GREATEST(tJob.uCreatedDate,tJob.uModDate) DESC LIMIT 10");
	else
		sprintf(gcQuery,"SELECT tJob.cLabel,GREATEST(tJob.uCreatedDate,tJob.uModDate),tNode.cLabel,tJobStatus.cLabel"
			" FROM tJob,tJobStatus,tNode WHERE tJob.uNode=tNode.uNode AND"
			" tJob.uJobStatus=tJobStatus.uJobStatus AND tJob.uJobStatus!=3"
			" AND tJob.uJobStatus!=7 AND tJob.uJobStatus!=6 AND tJob.uOwner=%u"
			" ORDER BY GREATEST(tJob.uCreatedDate,tJob.uModDate) DESC LIMIT 10",guCompany);
	macro_mySQLQueryErrorText
	printf("</td></tr>\n");
        while((mysqlField=mysql_fetch_row(mysqlRes)))
	{
		sscanf(mysqlField[1],"%lu",&luClock);
		printf("<tr><td></td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
			ctime(&luClock),mysqlField[0],mysqlField[2],mysqlField[3]);
	}
	mysql_free_result(mysqlRes);

	CloseFieldSet();

}//void DashBoard(const char *cOptionalMsg)


void ExtMainContent(void)
{
	printf("<p><input type=hidden name=gcFunction value=MainTools>");

	OpenFieldSet("System Information",100);

	OpenRow("Hostname","black");
	printf("<td>%s</td></tr>\n",gcHostname);

	OpenRow("Build Information","black");
	printf("<td>%s</td></tr>\n",gcBuildInfo);

	OpenRow("RAD Status","black");
	printf("<td>%s %s</td></tr>\n",gcRADStatus,REV);

	OpenRow("Application Summary","black");
	printf("<td>Manages OpenVZ containers across datacenters and hardware nodes. Flexible and"
		" extensible configuration management. Cloning, migration, duplication and backup"
		" operations supported, across both nodes and datacenters.</td></tr>\n");
	if(guPermLevel>9)
	{
		register unsigned int i;
		OpenRow("Table List","black");
		printf("<td>\n");
		for(i=0;cTableList[i][0];i++)
			printf("<a href=unxsVZ.cgi?gcFunction=%.32s>%.32s</a><br>\n",
				cTableList[i],cTableList[i]);
		printf("</td></tr>\n");
        	OpenRow("Admin Functions","black");
		printf("<td><input type=hidden name=gcFunction value=MainTools>\n");
		printf(" <input title='Find containers not being cloned' class=largeButton type=submit name=gcCommand"
			" value=CloneReport > \n");
		printf(" <input title='Find inconsistencies between actual node vz containers and unxsVZ status'"
			" class=largeButton type=submit name=gcCommand value=ContainerReport ></td></tr>\n");
	}

	CloseFieldSet();

}//void ExtMainContent(void)


//If called from command line
void ExtMainShell(int argc, char *argv[])
{
	if(getuid())
	{
		printf("Only root can run this command!\n");
		exit(0);
	}

        if(argc==2 && !strcmp(argv[1],"ProcessJobQueue"))
                ProcessJobQueue(0);
        if(argc==2 && !strcmp(argv[1],"ProcessJobQueueDebug"))
                ProcessJobQueue(1);
        else if(argc==2 && !strcmp(argv[1],"UpdateSchema"))
                UpdateSchema();
        else if(argc==2 && !strcmp(argv[1],"RecoverMode"))
                RecoverMode();
        else if(argc==2 && !strcmp(argv[1],"ResetAllSyncPeriod"))
                ResetAllSyncPeriod();
	else if(argc==6 && !strcmp(argv[1],"ImportTemplateFile"))
                ImportTemplateFile(argv[2],argv[3],argv[4],argv[5]);
	else if(argc==3 && !strcmp(argv[1],"Initialize"))
                Initialize(argv[2]);
        else if(argc==3 && !strcmp(argv[1],"Backup"))
                Backup(argv[2]);
        else if(argc==4 && !strcmp(argv[1],"Restore"))
                Restore(argv[2],argv[3]);
        else if(argc==3 && !strcmp(argv[1],"RestoreAll"))
                RestoreAll(argv[2]);
	else if(argc==6 && !strcmp(argv[1],"ExtracttLog"))
               	ExtracttLog(argv[2],argv[3],argv[4],argv[5]);
	else if(argc==10 && !strcmp(argv[1],"ImportRemoteDatacenter"))
                ImportRemoteDatacenter(argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9]);
	else if(argc==4 && !strcmp(argv[1],"ImportOSTemplates"))
                ImportOSTemplates(argv[2],argv[3]);
	else if(argc==3 && !strcmp(argv[1],"MassCreateContainers"))
                MassCreateContainers(argv[2]);
        else
	{
		printf("\n%s %s Menu\n\nDatabase Ops:\n",argv[0],RELEASE);
		printf("\tInitialize|Backup|RestoreAll <mysql root passwd>\n");
		printf("\tRestore <mysql root passwd> <Restore table name>\n");
		printf("\nCrontab Ops:\n");
		printf("\tProcessJobQueue\n");
		printf("\tProcessJobQueueDebug\n");
		//printf("\tProcessExtJobQueue <cServer>\n");
		printf("\nSpecial Admin Ops:\n");
		printf("\tUpdateSchema\n");
		printf("\tRecoverMode\n");
		printf("\tResetAllSyncPeriod\n");
		printf("\tImportTemplateFile <tTemplate.cLabel> <filespec> <tTemplateSet.cLabel> <tTemplateType.cLabel>\n");
		printf("\tExtracttLog <Mon> <Year> <mysql root passwd> <path to mysql table>\n");
		printf("\tImportRemoteDatacenter <local datacenter> <remote datacenter> <local node> <remote node>\n"
			"\t\t<host> <user> <passwd> <local uOwner>\n");
		printf("\tImportOSTemplates <path to templates e.g. /vz/template/cache/> <tClient.cLabel owner string>\n");
		printf("\tMassCreateContainers <configuration file>\n");
		printf("\n");
	}
	mysql_close(&gMysql);
        exit(0);


}//void ExtMainShell(int argc, char *argv[])


//ProjectFunctionStubs()


void RestoreAll(char *cPasswd)
{
	char cISMROOT[256]={""};
	register int i;

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		printf("You must set ISMROOT env var first. Ex. export ISMROOT=/home/joe\n");
		exit(1);
	}

	printf("Restoring unxsVZ data from .txt file in %s/unxsVZ/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	for(i=0;cTableList[i][0];i++)
	{
		sprintf(gcQuery,"LOAD DATA LOCAL INFILE '%s/unxsVZ/data/%s.txt' REPLACE INTO TABLE %s",
				cISMROOT,cTableList[i],cTableList[i]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		printf("%s\n",cTableList[i]);
	}

	printf("\nDone\n");

}//void RestoreAll(char *cPasswd)


void Restore(char *cPasswd, char *cTableName)
{
	char cISMROOT[256]={""};

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		printf("You must set ISMROOT env var first. Ex. export ISMROOT=/home/joe\n");
		exit(1);
	}

	printf("Restoring unxsVZ data from .txt file in %s/unxsVZ/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	sprintf(gcQuery,"LOAD DATA LOCAL INFILE '%s/unxsVZ/data/%s.txt' REPLACE INTO TABLE %s",
			cISMROOT,cTableName,cTableName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("%s\n\nDone\n",cTableName);

}//void Restore(char *cPasswd, char *cTableName)


void Backup(char *cPasswd)
{
	register int i;
	char cISMROOT[256]={""};

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		printf("You must set ISMROOT env var first. Ex. export ISMROOT=/home/joe\n");
		exit(1);
	}

	printf("Backing up unxsVZ data to .txt files in %s/unxsVZ/data...\n\n",cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	for(i=0;cTableList[i][0];i++)
	{
		char cFileName[300];

		sprintf(cFileName,"%s/unxsVZ/data/%s.txt"
				,cISMROOT,cTableList[i]);
		unlink(cFileName);

		sprintf(gcQuery,"SELECT * INTO OUTFILE '%s' FROM %s",
							cFileName,cTableList[i]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		printf("%s\n",cTableList[i]);
	}


	printf("\nDone.\n");

}//void Backup(char *cPasswd)


void Initialize(char *cPasswd)
{
	char cISMROOT[256]={""};
	register int i;

	if(getenv("ISMROOT")!=NULL)
	{
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}

	if(!cISMROOT[0])
	{
		printf("You must set ISMROOT env var first. Ex. export ISMROOT=/home/joe\n");
		exit(1);
	}

	printf("Creating db and setting permissions, installing data from %sunxsVZ...\n\n",
			cISMROOT);

	//connect as root to master db
	mySQLRootConnect(cPasswd);

	//Create database
	sprintf(gcQuery,"CREATE DATABASE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	//Grant localaccess privileges.
	sprintf(gcQuery,"GRANT ALL ON %s.* to %s@localhost IDENTIFIED BY '%s'",
							DBNAME,DBLOGIN,DBPASSWD);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	
	//Change to mysqlbind db. Then initialize some tables with needed data
	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	

	//debug only
	//Create tables and install default data
	//Standard RAD3 required tables
	CreatetAuthorize();
	CreatetClient();
	CreatetConfig();
        CreatetConfiguration();
	CreatetContainer();
	CreatetDatacenter();
        CreatetGlossary();
	CreatetGroup();
	CreatetGroupGlue();
	CreatetGroupType();
	CreatetIP();
        CreatetJob();
        CreatetJobStatus();
        CreatetLog();
        CreatetLogMonth();
        CreatetLogType();
        CreatetMonth();
	CreatetNameserver();
	CreatetNode();
	CreatetOSTemplate();
	CreatetProperty();
	CreatetSearchdomain();
        CreatetStatus();
        CreatetTemplate();
        CreatetTemplateSet();
        CreatetTemplateType();
	CreatetType();

	//Unique uContainer values starting at 101
	sprintf(gcQuery,"INSERT INTO tContainer SET uContainer=100");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	sprintf(gcQuery,"DELETE FROM tContainer");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

        for(i=0;cInitTableList[i][0];i++)
        {
                sprintf(gcQuery,"LOAD DATA LOCAL INFILE '%s/unxsVZ/data/%s.txt' REPLACE INTO TABLE %s",
			cISMROOT,cInitTableList[i],cInitTableList[i]);
                mysql_query(&gMysql,gcQuery);
                if(mysql_errno(&gMysql))
                {
                        printf("%s.\nAttempting to drop database for re-initialize...\n",mysql_error(&gMysql));

			sprintf(gcQuery,"DROP DATABASE %s",DBNAME);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}

                        exit(1);
                }
        }

        printf("Done\n");

}//void Initialize(void)


//No need for redundancy here like TextConnectDb()
void mySQLRootConnect(char *cPasswd)
{
        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,NULL,"root",cPasswd,"mysql",0,NULL,0))
        {
                printf("Database server unavailable\n");
                exit(1);
        }
}//void mySQLRootConnect(void)


void UpdateSchema(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uVeth=0;
	unsigned uBackupDate=0;
	unsigned uSource=0;

	unsigned uIPDatacenter=0;
	unsigned uOSTemplateDatacenter=0;
	unsigned uConfigDatacenter=0;
	unsigned uNameserverDatacenter=0;
	unsigned uSearchdomainDatacenter=0;

	unsigned uIncorrectSource=0;
	unsigned uIncorrectVeth=0;
	unsigned uSourceIndex=0;
	unsigned uPropertyNameIndex=0;
	unsigned uJobStatusIndex=0;
	unsigned uJobNodeIndex=0;
	unsigned uJobDatacenterIndex=0;
	unsigned uJobContainerIndex=0;
	unsigned uTemplateLabelIndex=0;
	unsigned uGlossaryLabelIndex=0;

	printf("UpdateSchema(): Start\n");

	if(TextConnectDb())
		exit(1);

	//Take note if what we need to change/add
	//This is based on expanded and incorrect schema of previous releases. Yes this sucks.
	sprintf(gcQuery,"SHOW COLUMNS IN tContainer");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[0],"uVeth"))
		{
			uVeth=1;
			if(!strcmp(field[2],"YES"))
				uIncorrectVeth=1;
		}
		if(!strcmp(field[0],"uSource"))
		{
			uSource=1;
			if(!strcmp(field[2],"YES"))
				uIncorrectSource=1;
		}
		if(!strcmp(field[0],"uBackupDate"))
		{
			uBackupDate=1;
		}
	}
       	mysql_free_result(res);

	sprintf(gcQuery,"SHOW COLUMNS IN tIP");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[0],"uDatacenter"))
			uIPDatacenter=1;
	}
       	mysql_free_result(res);

	sprintf(gcQuery,"SHOW COLUMNS IN tOSTemplate");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[0],"uDatacenter"))
			uOSTemplateDatacenter=1;
	}
       	mysql_free_result(res);

	sprintf(gcQuery,"SHOW COLUMNS IN tConfig");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[0],"uDatacenter"))
			uConfigDatacenter=1;
	}
       	mysql_free_result(res);

	sprintf(gcQuery,"SHOW COLUMNS IN tNameserver");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[0],"uDatacenter"))
			uNameserverDatacenter=1;
	}
       	mysql_free_result(res);

	sprintf(gcQuery,"SHOW COLUMNS IN tSearchdomain");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[0],"uDatacenter"))
			uSearchdomainDatacenter=1;
	}
       	mysql_free_result(res);

	sprintf(gcQuery,"SHOW INDEX IN tContainer");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[2],"uSource")) uSourceIndex=1;
	}
       	mysql_free_result(res);

	sprintf(gcQuery,"SHOW INDEX IN tProperty");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[2],"cName")) uPropertyNameIndex=1;
	}
       	mysql_free_result(res);

	sprintf(gcQuery,"SHOW INDEX IN tJob");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[2],"uJobStatus")) uJobStatusIndex=1;
		else if(!strcmp(field[2],"uContainer")) uJobContainerIndex=1;
		else if(!strcmp(field[2],"uNode")) uJobNodeIndex=1;
		else if(!strcmp(field[2],"uDatacenter")) uJobDatacenterIndex=1;
	}
       	mysql_free_result(res);

	sprintf(gcQuery,"SHOW INDEX IN tTemplate");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[2],"cLabel")) uTemplateLabelIndex=1;
	}
       	mysql_free_result(res);

	sprintf(gcQuery,"SHOW INDEX IN tGlossary");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(!strcmp(field[2],"cLabel")) uGlossaryLabelIndex=1;
	}
       	mysql_free_result(res);

	if(uIncorrectVeth)
	{
		sprintf(gcQuery,"ALTER TABLE tContainer MODIFY uVeth INT UNSIGNED NOT NULL DEFAULT 0");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Corrected uVeth of tContainer\n");
	}
	if(!uVeth)
	{
		sprintf(gcQuery,"ALTER TABLE tContainer ADD uVeth INT UNSIGNED NOT NULL DEFAULT 0");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added uVeth to tContainer\n");
	}
	if(!uBackupDate)
	{
		sprintf(gcQuery,"ALTER TABLE tContainer ADD uBackupDate INT UNSIGNED NOT NULL DEFAULT 0");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added uBackupDate to tContainer\n");
	}

	sprintf(gcQuery,"SELECT uStatus FROM tStatus WHERE uStatus=81");	
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)==0)
	{
		sprintf(gcQuery,"INSERT INTO tStatus SET uStatus=81,cLabel='Awaiting Clone',"
				"uCreatedBy=1,uOwner=1,uCreatedDate=UNIX_TIMESTAMP(NOW())");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Inserted uStatus=81 into tStatus\n");
	}
       	mysql_free_result(res);

	sprintf(gcQuery,"SELECT uStatus FROM tStatus WHERE uStatus=91");	
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)==0)
	{
		sprintf(gcQuery,"INSERT INTO tStatus SET uStatus=91,cLabel='Awaiting Failover',"
				"uCreatedBy=1,uOwner=1,uCreatedDate=UNIX_TIMESTAMP(NOW())");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Inserted uStatus=91 into tStatus\n");
	}
       	mysql_free_result(res);

	//Not important if repeated
	sprintf(gcQuery,"UPDATE tConfiguration SET cLabel='cSSHOptions' WHERE cLabel='cSSLOptions'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	//Not important if repeated
	sprintf(gcQuery,"ALTER TABLE tOSTemplate MODIFY cLabel VARCHAR(100) NOT NULL DEFAULT ''");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		printf("%s\n",mysql_error(&gMysql));

	if(uIncorrectSource)
	{
		sprintf(gcQuery,"ALTER TABLE tContainer MODIFY uSource INT UNSIGNED NOT NULL DEFAULT 0");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Corrected uSource of tContainer\n");

		sprintf(gcQuery,"ALTER TABLE tContainer DROP INDEX uSource");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
		}
		else
		{
			printf("Dropped uSource index of tContainer\n");
			uSourceIndex=0;
		}
	}

	if(!uSource)
	{
		sprintf(gcQuery,"ALTER TABLE tContainer ADD uSource INT UNSIGNED NOT NULL DEFAULT 0");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added uSource to tContainer\n");
	}

	if(!uSourceIndex)
	{
		sprintf(gcQuery,"ALTER TABLE tContainer ADD INDEX (uSource)");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added INDEX uSource tContainer\n");
	}

	//alter table tProperty add index (cName)
	if(!uPropertyNameIndex)
	{
		sprintf(gcQuery,"ALTER TABLE tProperty ADD INDEX (cName)");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added INDEX cName tProperty\n");
	}


	//alter table tJob add index
	if(!uJobStatusIndex)
	{
		sprintf(gcQuery,"ALTER TABLE tJob ADD INDEX (uJobStatus)");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added INDEX uJobStatus tJob\n");
	}

	if(!uJobNodeIndex)
	{
		sprintf(gcQuery,"ALTER TABLE tJob ADD INDEX (uNode)");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added INDEX uNode tJob\n");
	}

	if(!uJobDatacenterIndex)
	{
		sprintf(gcQuery,"ALTER TABLE tJob ADD INDEX (uDatacenter)");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added INDEX uDatacenter tJob\n");
	}

	if(!uJobContainerIndex)
	{
		sprintf(gcQuery,"ALTER TABLE tJob ADD INDEX (uContainer)");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added INDEX uContainer tJob\n");
	}

	if(!uTemplateLabelIndex)
	{
		sprintf(gcQuery,"ALTER TABLE tTemplate ADD INDEX (cLabel)");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added INDEX cLabel tTemplate\n");
	}

	if(!uGlossaryLabelIndex)
	{
		sprintf(gcQuery,"ALTER TABLE tGlossary ADD INDEX (cLabel)");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added INDEX cLabel tGlossary\n");
	}

	if(!uIPDatacenter)
	{
		sprintf(gcQuery,"ALTER TABLE tIP ADD uDatacenter INT UNSIGNED NOT NULL DEFAULT 0");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added uDatacenter to tIP\n");
	}

	if(!uOSTemplateDatacenter)
	{
		sprintf(gcQuery,"ALTER TABLE tOSTemplate ADD uDatacenter INT UNSIGNED NOT NULL DEFAULT 0");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added uDatacenter to tOSTemplate\n");
	}

	if(!uConfigDatacenter)
	{
		sprintf(gcQuery,"ALTER TABLE tConfig ADD uDatacenter INT UNSIGNED NOT NULL DEFAULT 0");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added uDatacenter to tConfig\n");
	}

	if(!uNameserverDatacenter)
	{
		sprintf(gcQuery,"ALTER TABLE tNameserver ADD uDatacenter INT UNSIGNED NOT NULL DEFAULT 0");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added uDatacenter to tNameserver\n");
	}

	if(!uSearchdomainDatacenter)
	{
		sprintf(gcQuery,"ALTER TABLE tSearchdomain ADD uDatacenter INT UNSIGNED NOT NULL DEFAULT 0");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Added uDatacenter to tSearchdomain\n");
	}

	//Please fix this TODO
		sprintf(gcQuery,"ALTER TABLE tJob MODIFY cRemoteMsg VARCHAR(64) NOT NULL DEFAULT ''");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			printf("%s\n",mysql_error(&gMysql));
		else
			printf("Corrected cRemoteMsg of tJob\n");


	printf("UpdateSchema(): End\n");

}//void UpdateSchema(void)

void ImportTemplateFile(char *cTemplate, char *cFile, char *cTemplateSet, char *cTemplateType)
{
	FILE *fp;
	unsigned uTemplate=0;
	unsigned uTemplateSet=0;
	unsigned uTemplateType=0;
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;
	char cBuffer[2048]={""};

	printf("\nImportTemplateFile(): Start\n");

	if(TextConnectDb())
		exit(1);

	sprintf(gcQuery,"USE %s",DBNAME);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	if((fp=fopen(cFile,"r"))==NULL)
	{
		printf("Could not open %s\n",cFile);
		exit(1);
	}

	//uTemplateSet
	sprintf(gcQuery,"SELECT uTemplateSet FROM tTemplateSet WHERE cLabel='%s'",cTemplateSet);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        mysqlRes=mysql_store_result(&gMysql);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sscanf(mysqlField[0],"%u",&uTemplateSet);
	mysql_free_result(mysqlRes);

	if(!uTemplateSet)
	{
		printf("Could not find tTemplateSet.clabel=%s\n",cTemplateSet);
		exit(1);
	}

	//uTemplateType
	sprintf(gcQuery,"SELECT uTemplateType FROM tTemplateType  WHERE cLabel='%s'",cTemplateType);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        mysqlRes=mysql_store_result(&gMysql);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sscanf(mysqlField[0],"%u",&uTemplateType);
	mysql_free_result(mysqlRes);

	if(!uTemplateType)
	{
		printf("Could not find tTemplateType.clabel=%s\n",cTemplateSet);
		exit(1);
	}

	//uTemplate
	sprintf(gcQuery,"SELECT uTemplate FROM tTemplate WHERE cLabel='%s' AND uTemplateSet=%u AND uTemplateType=%u",
						cTemplate,uTemplateSet,uTemplateType);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        mysqlRes=mysql_store_result(&gMysql);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sscanf(mysqlField[0],"%u",&uTemplate);
	mysql_free_result(mysqlRes);


	if(uTemplate)
	{
		printf("Updating tTemplate for %s %s %s\n",cTemplate,cTemplateSet,cTemplateType);
		sprintf(cBuffer,"UPDATE tTemplate SET uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()),cTemplate='',"
				"uTemplateSet=%u,uTemplateType=%u,uModBy=1 WHERE uTemplate=%u",
								uTemplateSet,uTemplateType,uTemplate);
		mysql_query(&gMysql,cBuffer);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n%.254s\n",mysql_error(&gMysql),cBuffer);
			exit(1);
		}
	}
	else
	{
		printf("Inserting new tTemplate for %s\n",cTemplate);
		sprintf(cBuffer,"INSERT INTO tTemplate SET uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW()),"
				"cLabel='%s',uTemplateSet=%u,uTemplateType=%u",cTemplate,uTemplateSet,uTemplateType);
		mysql_query(&gMysql,cBuffer);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n%.254s\n",mysql_error(&gMysql),cBuffer);
			exit(1);
		}
		uTemplate=mysql_insert_id(&gMysql);
	}

	while(fgets(gcQuery,1024,fp)!=NULL)
	{
		sprintf(cBuffer,"UPDATE tTemplate SET cTemplate=CONCAT(cTemplate,'%s') WHERE uTemplate=%u",
				TextAreaSave(gcQuery),uTemplate);
		mysql_query(&gMysql,cBuffer);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n%.254s\n",mysql_error(&gMysql),cBuffer);
			exit(1);
		}
	}
	fclose(fp);

	printf("\nDone\n");

}//void ImportTemplateFile()


void CalledByAlias(int iArgc,char *cArgv[])
{
	if(strstr(cArgv[0],"unxsVZRSS.xml"))
	{
		MYSQL_RES *res;
		MYSQL_ROW field;
		char cRSSDate[200];
		char cLinkStart[200]={""};
           	time_t tTime,luClock;
           	struct tm *tmTime;
		char *cHTTP="http";

		time(&luClock);
           	tTime=time(NULL);
           	tmTime=gmtime(&tTime);
           	strftime(cRSSDate,sizeof(cRSSDate),"%a, %d %b %Y %T GMT",tmTime);

		if(getenv("HTTP_HOST")!=NULL)
			sprintf(gcHost,"%.99s",getenv("HTTP_HOST"));
		if(getenv("HTTPS")!=NULL)
			cHTTP="https";

		//This is the standard place. With much parsing nonsense we can
		//	do better by using env vars
		sprintf(cLinkStart,"%s://%s/cgi-bin/unxsVZ.cgi",cHTTP,gcHost);

		printf("Content-type: text/xml\n\n");

		//Open xml
		printf("<?xml version='1.0' encoding='UTF-8'?>\n");
		printf("<rss version='2.0'>\n");
		printf("<channel>\n");
		printf("<title>unxsVZ RSS tJob Errors</title>\n");
		printf("<link>http://openisp.net/unxsVZ</link>\n");
		printf("<description>unxsVZ tJob Errors</description>\n");
		printf("<lastBuildDate>%.199s</lastBuildDate>\n",cRSSDate);
		printf("<generator>unxsVZ RSS Generator</generator>\n");
		printf("<docs>http://blogs.law.harvard.edu/tech/rss</docs>\n");
		printf("<ttl>120</ttl>\n");

		//Loop for each tJob error
		//Connect to local mySQL
		if(TextConnectDb())
			exit(1);

		sprintf(gcQuery,"SELECT uJob,cServer,cJobName,uUser,cJobData FROM tJob WHERE uJobStatus=4");//4 is tJobStatus Done Error(s)
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			printf("\n<item>\n");
			printf("<title>unxsVZ.tJob.uJob=%s</title>\n",field[0]);
			printf("<link>%s?gcFunction=tJob&amp;uJob=%s</link>\n",cLinkStart,
							field[0]);
			printf("<description>cJobName=%s Server=%s uUser=%s\ncJobData=(%s)</description>\n",field[2],field[1],field[3],field[4]);
			printf("<guid isPermaLink='false'>%s-%lu</guid>\n",field[0],luClock);
			printf("<pubDate>%.199s</pubDate>\n",cRSSDate);
			printf("</item>\n");

		}
       		mysql_free_result(res);

		//Close xml
		printf("\n</channel>\n");
		printf("</rss>\n");
		mysql_close(&gMysql);
		exit(0);
	}
	else
	{
		printf("Content-type: text/plain\n\n");
		printf("Called as unsupported alias %s\n",cArgv[0]);
		exit(0);
	}

}//void CalledByAlias(int iArgc,char *cArgv[])


void ExtracttLog(char *cMonth, char *cYear, char *cPasswd, char *cTablePath)
{
	char cTableName[33]={""};
	char cNextMonth[4]={""};
	char cNextYear[8]={""};
	char cThisYear[8]={""};
	char cThisMonth[4]={""};
	long uStart=0;
	long uEnd=0;
	unsigned uRows=0;
	time_t clock;
	struct tm *structTime;
	struct stat info;

	time(&clock);
	structTime=localtime(&clock);
	strftime(cThisYear,8,"%Y",structTime);
	strftime(cThisMonth,4,"%b",structTime);

	printf("ExtracttLog() Start\n");

	printf("cThisMonth:%s cThisYear:%s\n",cThisMonth,cThisYear);

	if(!strcmp(cThisMonth,cMonth) && !strcmp(cThisYear,cYear))
	{
		fprintf(stderr,"Can't extract this months data!\n");
		exit(1);
	}

	if(stat("/usr/bin/myisampack",&info) )
	{
		fprintf(stderr,"/usr/bin/myisampack is not installed!\n");
		exit(1);
	}

	if(stat("/usr/bin/myisamchk",&info))
	{
		fprintf(stderr,"/usr/bin/myisamchk is not installed!\n");
		exit(1);
	}


	mySQLRootConnect(cPasswd);
	mysql_query(&gMysql,"USE idns");
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	sprintf(gcQuery,"INSERT INTO tLog SET cMessage='ExtracttLog() Start...',"
			"cServer='%s',uLogType=4,uOwner=1,uCreatedBy=1,"
			"uCreatedDate=UNIX_TIMESTAMP(NOW())",gcHostname);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		fprintf(stderr,"%s\n",mysql_error(&gMysql));

	sprintf(cTableName,"t%.3s%.7s",cMonth,cYear);

	sprintf(gcQuery,"1-%s-%s",cMonth,cYear);
	uStart=cDateToUnixTime(gcQuery);
	printf("Start: %s",ctime(&uStart));

	if(uStart== -1 || !uStart)
	{
		fprintf(stderr,"Garbled month year input (uStart)\n");
		exit(1);
	}

	NextMonthYear(cMonth,cYear,cNextMonth,cNextYear);
	//Debug only
	//printf("%s %s to %s %s\n",cMonth,cYear,cNextMonth,cNextYear);
	//exit(0);

	sprintf(gcQuery,"1-%s-%s",cNextMonth,cNextYear);
	uEnd=cDateToUnixTime(gcQuery);
	printf("End:   %s",ctime(&uEnd));
	if(uEnd== -1 || !uEnd)
	{
		fprintf(stderr,"Garbled month year input (uEnd)\n");
		exit(1);
	}

	
	CreatetLogTable(cTableName);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Clearing data from %s...\n",cTableName);
	sprintf(gcQuery,"DELETE FROM %s",cTableName);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Getting data from tLog...\n");
	sprintf(gcQuery,"INSERT %s (uLog,cLabel,uLogType,cHash,uPermLevel,uLoginClient,cLogin,"
			"cHost,uTablePK,cTableName,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate)"
			" SELECT uLog,cLabel,uLogType,cHash,uPermLevel,uLoginClient,cLogin,cHost,"
			"uTablePK,cTableName,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate"
			" FROM tLog WHERE uCreatedDate>=%lu AND uCreatedDate<%lu",
				cTableName,uStart,uEnd);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	//If 0 records inserted delete from tMonth and exit now
	uRows=mysql_affected_rows(&gMysql);

	printf("Number of rows found and inserted: %u\n",uRows);

	if(uRows)
	{

		sprintf(gcQuery,"REPLACE tMonth SET cLabel='%s',uOwner=1,uCreatedBy=1,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW())",cTableName);
        	mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}
	}
	else
	{
		sprintf(gcQuery,"DELETE FROM tMonth WHERE cLabel='%s'",cTableName);
        	mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}

		sprintf(gcQuery,"DROP TABLE %s",cTableName);
        	mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}


		printf("Exiting early no data to be archived\n");
		exit(0);
	}

	sprintf(gcQuery,"/usr/bin/myisampack %s/%s",cTablePath,cTableName);
	if(system(gcQuery))
	{
		fprintf(stderr,"Failed: %s\n",gcQuery);
		exit(1);
	}

	sprintf(gcQuery,"/usr/bin/myisamchk -rq %s/%s",cTablePath,cTableName);
	if(system(gcQuery))
	{
		fprintf(stderr,"Failed: %s\n",gcQuery);
		exit(1);
	}

	printf("Flushing compressed RO table...\n");
	sprintf(gcQuery,"FLUSH TABLE %s",cTableName);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Removing records from tLog...\n");
	sprintf(gcQuery,"DELETE QUICK FROM tLog WHERE uCreatedDate>=%lu AND uCreatedDate<%lu AND uLogType!=5",
				uStart,uEnd);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}


	printf("Extracted and Archived. Table flushed: %s\n",cTableName);
	printf("ExtracttLog() End\n");
	sprintf(gcQuery,"INSERT INTO tLog SET cMessage='ExtracttLog() End',cServer='%s',uLogType=4,"
			"uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",gcHostname);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
	exit(0);

}//void ExtracttLog(char *cMonth, char *cYear, char *cPasswd, char *cTablePath)


time_t cDateToUnixTime(char *cDate)
{
        struct  tm locTime;
        time_t  res;

        bzero(&locTime, sizeof(struct tm));
	if(strchr(cDate,'-'))
        	strptime(cDate,"%d-%b-%Y", &locTime);
	else if(strchr(cDate,'/'))
        	strptime(cDate,"%d/%b/%Y", &locTime);
	else if(strchr(cDate,' '))
        	strptime(cDate,"%d %b %Y", &locTime);
        locTime.tm_sec = 0;
        locTime.tm_min = 0;
        locTime.tm_hour = 0;
        res = mktime(&locTime);
        return(res);
}//time_t cDateToUnixTime(char *cDate)


//Another schema dependent item
void CreatetLogTable(char *cTableName)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS %s ( uTablePK VARCHAR(32) NOT NULL DEFAULT '', cHost VARCHAR(32) NOT NULL DEFAULT '', uLoginClient INT UNSIGNED NOT NULL DEFAULT 0, cLogin VARCHAR(32) NOT NULL DEFAULT '', uPermLevel INT UNSIGNED NOT NULL DEFAULT 0, cTableName VARCHAR(32) NOT NULL DEFAULT '', uLog INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(64) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cHash VARCHAR(32) NOT NULL DEFAULT '', uLogType INT UNSIGNED NOT NULL DEFAULT 0,index (uLogType) )",cTableName);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
}//CreatetLogTable()


void NextMonthYear(char *cMonth,char *cYear,char *cNextMonth,char *cNextYear)
{
	unsigned uYear=0;

	//Preset for all but Dec cases
	sprintf(cNextYear,"%.7s",cYear);
	sscanf(cNextYear,"%u",&uYear);

	if(!strcmp(cMonth,"Jan"))
	{
		strcpy(cNextMonth,"Feb");
	}
	else if(!strcmp(cMonth,"Feb"))
	{
		strcpy(cNextMonth,"Mar");
	}
	else if(!strcmp(cMonth,"Mar"))
	{
		strcpy(cNextMonth,"Apr");
	}
	else if(!strcmp(cMonth,"Apr"))
	{
		strcpy(cNextMonth,"May");
	}
	else if(!strcmp(cMonth,"May"))
	{
		strcpy(cNextMonth,"Jun");
	}
	else if(!strcmp(cMonth,"Jun"))
	{
		strcpy(cNextMonth,"Jul");
	}
	else if(!strcmp(cMonth,"Jul"))
	{
		strcpy(cNextMonth,"Aug");
	}
	else if(!strcmp(cMonth,"Aug"))
	{
		strcpy(cNextMonth,"Sep");
	}
	else if(!strcmp(cMonth,"Sep"))
	{
		strcpy(cNextMonth,"Oct");
	}
	else if(!strcmp(cMonth,"Oct"))
	{
		strcpy(cNextMonth,"Nov");
	}
	else if(!strcmp(cMonth,"Nov"))
	{
		strcpy(cNextMonth,"Dec");
	}
	else if(!strcmp(cMonth,"Dec"))
	{
		strcpy(cNextMonth,"Jan");
		uYear++;
		sprintf(cNextYear,"%u",uYear);
	}

}//NextMonthYear()


void TextError(const char *cError, unsigned uContinue)
{
	printf("\nPlease report this unxsVZ fatal error ASAP:\n%s\n",cError);

	//Attempt to report error in tLog
        sprintf(gcQuery,"INSERT INTO tLog SET cLabel='TextError',uLogType=4,uPermLevel=%u,uLoginClient=%u,"
			"cLogin='%s',cHost='%s',cMessage=\"%s\",cServer='%s',uOwner=1,uCreatedBy=%u,"
			"uCreatedDate=UNIX_TIMESTAMP(NOW())",
			guPermLevel,guLoginClient,gcUser,gcHost,cError,gcHostname,guLoginClient);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		printf("Another error occurred while attempting to log: %s\n",
				mysql_error(&gMysql));
	if(!uContinue) exit(0);

}//void TextError(const char *cError, unsigned uContinue)


void GetConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer,
		unsigned uHtml)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        char cQuery[1024];
	char cExtra[100]={""};

        sprintf(cQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='%s'",
			cName);
	if(uDatacenter)
	{
		sprintf(cExtra," AND uDatacenter=%u",uDatacenter);
		strcat(cQuery,cExtra);
	}
	if(uNode)
	{
		sprintf(cExtra," AND uNode=%u",uNode);
		strcat(cQuery,cExtra);
	}
	if(uContainer)
	{
		sprintf(cExtra," AND uContainer=%u",uContainer);
		strcat(cQuery,cExtra);
	}
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
        	        htmlPlainTextError(mysql_error(&gMysql));
		else
        	        TextError(mysql_error(&gMysql),0);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
        	sprintf(cValue,"%.255s",field[0]);
        mysql_free_result(res);

}//void GetConfiguration(...)


void RecoverMode(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cHostname[100];
	unsigned uNode=0,uDatacenter=0;

	printf("RecoverMode(): Start\n");

	if(gethostname(cHostname,99)!=0)
	{
		printf("Could not determine cHostname\n");
		exit(1);
	}

	if(TextConnectDb())
		exit(1);

	//Get node and datacenter via hostname
	sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
	}
	mysql_free_result(res);

	//FQDN vs short name of 2nd NIC mess
	if(!uNode)
	{
		char *cp;

		if((cp=strchr(cHostname,'.')))
			*cp=0;
		sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			mysql_close(&gMysql);
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uNode);
			sscanf(field[1],"%u",&uDatacenter);
		}
		mysql_free_result(res);
	}

	if(!uNode)
	{
		printf("Could not determine uNode\n");
		mysql_close(&gMysql);
		exit(1);
	}

	//Take note if what we need to change/add
	//This is based on expanded and incorrect schema of previous releases. Yes this sucks.
	sprintf(gcQuery,"SELECT uContainer FROM tJob WHERE cJobName='FailoverFrom' AND uJobStatus=1 AND uJobDate<UNIX_TIMESTAMP(NOW())"
			" AND uDatacenter=%u AND uNode=%u",uDatacenter,uNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(1);
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		//Nothing yet. Old startup issue was fixed in jobqueue.c
	}
	mysql_free_result(res);

	mysql_close(&gMysql);
	printf("RecoverMode(): End\nNow you can start vz\n");

}//void RecoverMode(void)


void ResetAllSyncPeriod(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cHostname[100];
	unsigned uNode=0,uDatacenter=0;

	printf("ResetAllSyncPeriod(): Start\n");

	if(gethostname(cHostname,99)!=0)
	{
		printf("Could not determine cHostname\n");
		exit(1);
	}

	if(TextConnectDb())
		exit(1);

	//Get node and datacenter via hostname
	sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
	}
	mysql_free_result(res);

	//FQDN vs short name of 2nd NIC mess
	if(!uNode)
	{
		char *cp;

		if((cp=strchr(cHostname,'.')))
			*cp=0;
		sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			mysql_close(&gMysql);
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uNode);
			sscanf(field[1],"%u",&uDatacenter);
		}
		mysql_free_result(res);
	}

	if(!uNode)
	{
		printf("Could not determine uNode\n");
		mysql_close(&gMysql);
		exit(1);
	}

	//Take note if what we need to change/add
	//This is based on expanded and incorrect schema of previous releases. Yes this sucks.
	sprintf(gcQuery,"SELECT tProperty.uProperty FROM tContainer,tProperty"
			" WHERE tProperty.uKey=tContainer.uContainer AND tProperty.uType=3 AND tProperty.cName='cuSyncPeriod'"
			" AND tProperty.cValue='0'"
			" AND tContainer.uSource>0"
			" AND LOCATE('-clone',tContainer.cLabel)>0"
			" AND tContainer.uDatacenter=%u AND tContainer.uNode=%u",uDatacenter,uNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(1);
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("%s\n",field[0]);
		sprintf(gcQuery,"UPDATE tProperty SET cValue='1200' WHERE uProperty=%s",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			mysql_close(&gMysql);
			exit(1);
		}
	}
	mysql_free_result(res);

	mysql_close(&gMysql);
	printf("ResetAllSyncPeriod(): End\n");

}//void ResetAllSyncPeriod(void)


//Import remote datacenter containers from-to specified node, and set as status offline
//Local tDatacenter and tNode must be ready.
//Local tIP,tOSTemplate,tConfig,tNameserver,tSearchdomain,tProperty will be modified as needed.
//Clone containers will not be imported.
//Only import basic tContainer data
void ImportRemoteDatacenter(
			const char *cLocalDatacenter,
			const char *cRemoteDatacenter,
			const char *cLocalNode,
			const char *cRemoteNode,
			const char *cHost,
			const char *cUser,
			const char *cPasswd,
			const char *cuOwner)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	MYSQL gMysqlExt;
	MYSQL_RES *resExt;
	MYSQL_ROW fieldExt;

	//Same for all import ops
	unsigned uLocalDatacenter=0;
	unsigned uLocalNode=0;
	unsigned uRemoteDatacenter=0;
	unsigned uRemoteNode=0;
	unsigned uClient=0;//Same as c/uOwner
	unsigned uGroup=0;

	//Used in loop
	unsigned uIP;
	unsigned uOSTemplate;
	unsigned uConfig;
	unsigned uNameserver;
	unsigned uSearchdomain;
	unsigned uContainer;

	printf("ImportRemoteDatacenter(): Start\n");

	if(TextConnectDb())
		exit(1);

        mysql_init(&gMysqlExt);
        if(!mysql_real_connect(&gMysqlExt,cHost,cUser,cPasswd,"unxsvz",0,NULL,0))
        {
                printf("Could not connect and/or authenticate to remote database.\n");
                exit(1);
        }

	//Checks
	//1. tDatacenter check
	sprintf(gcQuery,"SELECT uDatacenter FROM tDatacenter WHERE cLabel='%s'",cLocalDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uLocalDatacenter);
       	mysql_free_result(res);
	if(!uLocalDatacenter)
	{
		printf("Local tDatacenter.cLabel=%s does not exist\n",cLocalDatacenter);
		exit(1);
	}

	sprintf(gcQuery,"SELECT uDatacenter FROM tDatacenter WHERE cLabel='%s'",cRemoteDatacenter);
	mysql_query(&gMysqlExt,gcQuery);
	if(mysql_errno(&gMysqlExt))
	{
		printf("%s\n",mysql_error(&gMysqlExt));
		exit(1);
	}
	mysql_query(&gMysqlExt,gcQuery);
	resExt=mysql_store_result(&gMysqlExt);
	if((fieldExt=mysql_fetch_row(resExt)))
		sscanf(fieldExt[0],"%u",&uRemoteDatacenter);
       	mysql_free_result(resExt);
	if(!uRemoteDatacenter)
	{
		printf("Remote tDatacenter.cLabel=%s does not exist\n",cRemoteDatacenter);
		exit(1);
	}

	//2. tNode check
	sprintf(gcQuery,"SELECT uNode FROM tNode WHERE cLabel='%s'",cLocalNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uLocalNode);
       	mysql_free_result(res);
	if(!uLocalNode)
	{
		printf("Local tNode.cLabel=%s does not exist\n",cLocalNode);
		exit(1);
	}

	sprintf(gcQuery,"SELECT uNode FROM tNode WHERE cLabel='%s'",cRemoteNode);
	mysql_query(&gMysqlExt,gcQuery);
	if(mysql_errno(&gMysqlExt))
	{
		printf("%s\n",mysql_error(&gMysqlExt));
		exit(1);
	}
	mysql_query(&gMysqlExt,gcQuery);
	resExt=mysql_store_result(&gMysqlExt);
	if((fieldExt=mysql_fetch_row(resExt)))
		sscanf(fieldExt[0],"%u",&uRemoteNode);
       	mysql_free_result(resExt);
	if(!uRemoteNode)
	{
		printf("Remote tNode.cLabel=%s does not exist\n",cRemoteNode);
		exit(1);
	}

	//3. tClient check
	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE uClient=%s",cuOwner);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uClient);
       	mysql_free_result(res);
	if(!uClient)
	{
		printf("Local tClient.uClient=%s does not exist\n",cuOwner);
		exit(1);
	}

	//4. Optional tGroup with same name as remote datacenter cLabel
	sprintf(gcQuery,"SELECT uGroup FROM tGroup WHERE cLabel='%s'",cRemoteDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uGroup);
       	mysql_free_result(res);

	//Start importing
	//Only active (1) not clones
	sprintf(gcQuery,"SELECT tContainer.cLabel,tContainer.cHostname,"
			" tIP.cLabel,tOSTemplate.cLabel,tConfig.cLabel,tNameserver.cLabel,tSearchdomain.cLabel"
			" FROM tContainer,tIP,tOSTemplate,tConfig,tNameserver,tSearchdomain"
			" WHERE tContainer.uIPv4=tIP.uIP AND tContainer.uOSTemplate=tOSTemplate.uOSTemplate"
			" AND tContainer.uConfig=tConfig.uConfig AND tContainer.uNameserver=tNameserver.uNameserver"
			" AND tContainer.uSearchdomain=tSearchdomain.uSearchdomain"
			" AND tContainer.uStatus=1 AND tContainer.uSource=0 AND tContainer.uDatacenter=%u AND tContainer.uNode=%u",
				uRemoteDatacenter,uRemoteNode);
	mysql_query(&gMysqlExt,gcQuery);
	if(mysql_errno(&gMysqlExt))
	{
		printf("%s\n",mysql_error(&gMysqlExt));
		exit(1);
	}
	mysql_query(&gMysqlExt,gcQuery);
	resExt=mysql_store_result(&gMysqlExt);
	while((fieldExt=mysql_fetch_row(resExt)))
	{
		uIP=0;
		uOSTemplate=0;
		uConfig=0;
		uNameserver=0;
		uSearchdomain=0;
		uContainer=0;

		//printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
		//	fieldExt[0],fieldExt[1],fieldExt[2],fieldExt[3],fieldExt[4],fieldExt[5],fieldExt[6]);

		//Do not add if we already have a local tContainer.cLabel
		sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE cLabel='%s'",fieldExt[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)!=0)
		{
			printf("Local tContainer.cLabel=%s exists\n",fieldExt[0]);
			continue;
		}
       		mysql_free_result(res);

		//tIP.cLabel [2]
		sprintf(gcQuery,"SELECT uIP FROM tIP WHERE cLabel='%s'",fieldExt[2]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)==0)
		{
			sprintf(gcQuery,"INSERT INTO tIP SET cLabel='%s',uAvailable=0,uDatacenter=%u,uOwner=%u,"
					"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						fieldExt[2],uLocalDatacenter,uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
			uIP=mysql_insert_id(&gMysql);
		}
		else
		{
			if((field=mysql_fetch_row(res)))
				sscanf(field[0],"%u",&uIP);
			//printf("Local tIP.cLabel=%s exists. Not adding.\n",fieldExt[2]);
			if(!uIP)
			{
				printf("Unexpected uIP==0 error skip to next container\n");
				continue;
			}
		}
       		mysql_free_result(res);

		//debug only
		//printf("Post tIP import stop %s uClient=%u\n",fieldExt[2],uClient);
		//exit(0);

		//tOSTemplate.cLabel [3]
		sprintf(gcQuery,"SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='%s'",fieldExt[3]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)==0)
		{
			sprintf(gcQuery,"INSERT INTO tOSTemplate SET cLabel='%s',uOwner=%u,"
					"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						fieldExt[3],uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
			uOSTemplate=mysql_insert_id(&gMysql);
			//Enable for only this datacenter via tProperty tOSTemplate type = 8
			sprintf(gcQuery,"INSERT INTO tProperty SET cName='cDatacenter',cValue='%s',uType=8,uKey=%u,"
					" uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						cLocalDatacenter,uOSTemplate,uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
		}
		else
		{
			if((field=mysql_fetch_row(res)))
				sscanf(field[0],"%u",&uOSTemplate);
			//printf("Local tOSTemplate.cLabel=%s exists. Not adding.\n",fieldExt[3]);
			if(!uOSTemplate)
			{
				printf("Unexpected uOSTemplate==0 error skip to next container\n");
				continue;
			}
		}
       		mysql_free_result(res);

		//debug only
		//printf("Post tOSTemplate import stop uOSTemplate=%u\n",uOSTemplate);
		//exit(0);

		//tConfig.cLabel [4]
		sprintf(gcQuery,"SELECT uConfig FROM tConfig WHERE cLabel='%s'",fieldExt[4]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)==0)
		{
			sprintf(gcQuery,"INSERT INTO tConfig SET cLabel='%s',uOwner=%u,"
					"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						fieldExt[4],uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
			uConfig=mysql_insert_id(&gMysql);
			//Enable for only this datacenter via tProperty tConfig type = 6 TODO defines
			sprintf(gcQuery,"INSERT INTO tProperty SET cName='cDatacenter',cValue='%s',uType=6,uKey=%u,"
					"uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						cLocalDatacenter,uConfig,uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
		}
		else
		{
			if((field=mysql_fetch_row(res)))
				sscanf(field[0],"%u",&uConfig);
			//printf("Local tConfig.cLabel=%s exists. Not adding.\n",fieldExt[4]);
			if(!uConfig)
			{
				printf("Unexpected uConfig==0 error skip to next container\n");
				continue;
			}
		}
       		mysql_free_result(res);

		//debug only
		//printf("Post tConfig import stop uConfig=%u\n",uConfig);
		//exit(0);

		//tNameserver.cLabel [5]
		sprintf(gcQuery,"SELECT uNameserver FROM tNameserver WHERE cLabel='%s'",fieldExt[5]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)==0)
		{
			sprintf(gcQuery,"INSERT INTO tNameserver SET cLabel='%s',uOwner=%u,"
					"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						fieldExt[5],uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
			uNameserver=mysql_insert_id(&gMysql);
			//Enable for only this datacenter via tProperty tNameserver type = 7 TODO defines
			sprintf(gcQuery,"INSERT INTO tProperty SET cName='cDatacenter',cValue='%s',uType=7,uKey=%u,"
					" uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						cLocalDatacenter,uNameserver,uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
		}
		else
		{
			if((field=mysql_fetch_row(res)))
				sscanf(field[0],"%u",&uNameserver);
			//printf("Local tNameserver.cLabel=%s exists. Not adding.\n",fieldExt[5]);
			if(!uNameserver)
			{
				printf("Unexpected uNameserver==0 error skip to next container\n");
				continue;
			}
		}

		//debug only
		//printf("Post tNameserver import stop uNameserver=%u\n",uNameserver);
		//exit(0);

		//tSearchdomain.cLabel [6]
		sprintf(gcQuery,"SELECT uSearchdomain FROM tSearchdomain WHERE cLabel='%s'",fieldExt[6]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)==0)
		{
			sprintf(gcQuery,"INSERT INTO tSearchdomain SET cLabel='%s',uOwner=%u,"
					"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						fieldExt[6],uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
			uSearchdomain=mysql_insert_id(&gMysql);
			//Enable for only this datacenter via tProperty tSearchdomain type = 9 TODO defines
			sprintf(gcQuery,"INSERT INTO tProperty SET cName='cDatacenter',cValue='%s',uType=9,uKey=%u,"
					"uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						cLocalDatacenter,uSearchdomain,uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
		}
		else
		{
			if((field=mysql_fetch_row(res)))
				sscanf(field[0],"%u",&uSearchdomain);
			//printf("Local tSearchdomain.cLabel=%s exists. Not adding.\n",fieldExt[6]);
			if(!uSearchdomain)
			{
				printf("Unexpected uSearchdomain==0 error skip to next container\n");
				continue;
			}
		}

		//debug only
		//printf("Post tSearchdomain import stop uSearchdomain=%u\n",uSearchdomain);
		//exit(0);

		//Add container
		sprintf(gcQuery,"INSERT INTO tContainer SET"
				" cLabel='%s',"
				" cHostname='%s',"
				" uVeth=0,"
				" uIPv4=%u,"
				" uOSTemplate=%u,"
				" uConfig=%u,"
				" uNameserver=%u,"
				" uSearchdomain=%u,"
				" uDatacenter=%u,"
				" uNode=%u,"
				" uStatus=3," //Offline
				" uSource=0,"
				" uOwner=%u,"
				" uCreatedBy=1,"
				" uCreatedDate=UNIX_TIMESTAMP(NOW())",
				fieldExt[0],
				fieldExt[1],
				uIP,
				uOSTemplate,
				uConfig,
				uNameserver,
				uSearchdomain,
				uLocalDatacenter,
				uLocalNode,
				uClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(1);
		}
		printf("Imported: %s\n",fieldExt[1]);
		uContainer=mysql_insert_id(&gMysql);
		if(!uContainer)
		{
			printf("Unexpected uContainer==0 error skipping group section\n");
			continue;
		}

		//Add container to premade optional group "cRemoteDatacenter"
		if(uGroup)
		{
			sprintf(gcQuery,"INSERT INTO tGroupGlue SET uGroup=%u,uContainer=%u",uGroup,uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
		}

	}//While remote containers
       	mysql_free_result(resExt);


	mysql_close(&gMysql);
	mysql_close(&gMysqlExt);
	printf("ImportRemoteDatacenter(): End\n");

}//void ImportRemoteDatacenter()


void ImportOSTemplates(char *cPath, char *cOwner)
{
	char cHostname[100]={""};
	char cLabel[100]={""};
	unsigned uOwner=0;
	unsigned uNode=0;
	unsigned uDatacenter=0;
	struct dirent **namelist;
	register int n,i;
	char *cp;
	MYSQL_RES *res;
	MYSQL_ROW field;

	printf("ImportOSTemplates(): Start\n");

	if(TextConnectDb())
		exit(1);

	if(gethostname(cHostname,99)!=0)
	{
		printf("gethostname() failed\n");
		exit(1);
	}

	//Get node and datacenter via hostname
	sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf(mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
	}
	mysql_free_result(res);
	//FQDN vs short name of 2nd NIC mess
	if(!uNode)
	{
		if((cp=strchr(cHostname,'.')))
			*cp=0;
		sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf(mysql_error(&gMysql));
			mysql_close(&gMysql);
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uNode);
			sscanf(field[1],"%u",&uDatacenter);
		}
		mysql_free_result(res);
	}

	//Get uOwner
	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cLabel='%s'",cOwner);
        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf(mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uOwner);
	mysql_free_result(res);
	if(!uOwner)
	{
		printf("No such owner: \"%s\"\n",cOwner);
		exit(1);
	}
	printf("uNode=%u uDatacenter=%u uOwner=%u\n",uNode,uDatacenter,uOwner);

	n=scandir(cPath,&namelist,0,0);
	if(n<0)
	{
		printf("scandir() error.\n");
		exit(1);
	}
	else if(n==2)
	{
		printf("No files found.\n");
		exit(1);
	}
	for(i=0;i<n;i++)
	{

		//Added some end of list test hack a long time ago. Remove?
		if(namelist[i]->d_name[0]=='.' || 
			strstr(namelist[i]->d_name+strlen(namelist[i]->d_name)-5,
				".done"))
		{
			;
		}
		else
		{
			if(!strcmp(namelist[i]->d_name+strlen(namelist[i]->d_name)-6,"tar.gz"))
			{
				unsigned uOSTemplate=0;

				sprintf(cLabel,"%.100s",namelist[i]->d_name);

				//Chop off .tar.gz
				if((cp=strstr(cLabel,".tar.gz")))
					*cp=0;

				//Ignore if already in tOSTemplate
				sprintf(gcQuery,"SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='%s'",cLabel);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf(mysql_error(&gMysql));
					mysql_close(&gMysql);
					exit(2);
				}
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					continue;
				}
				mysql_free_result(res);

				//Add new record
				sprintf(gcQuery,"INSERT INTO tOSTemplate SET cLabel='%s',uOwner=%u,uCreatedBy=1,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW())",TextAreaSave(cLabel),uOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf(mysql_error(&gMysql));
					mysql_close(&gMysql);
					exit(2);
				}
				uOSTemplate=mysql_insert_id(&gMysql);
				sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType="PROP_OSTEMPLATE
						",cName='cDatacenter',cValue='%s',uOwner=%u,uCreatedBy=1"
						",uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,uOSTemplate,ForeignKey("tDatacenter","cLabel",uDatacenter),uOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf(mysql_error(&gMysql));
					mysql_close(&gMysql);
					exit(2);
				}
				printf("%s\n",cLabel);
			}
		}
	}

	printf("ImportOSTemplates(): End\n");

}//void ImportOSTemplates()


void MassCreateContainers(char *cConfigfileName)
{
	char cHostname[100]={""};
	unsigned uNode=0;
	unsigned uDatacenter=0;
	char *cp;
	FILE *fp;
	MYSQL_RES *res;
	MYSQL_ROW field;

	printf("MassCreateContainers(): Start\n\n");

	if(TextConnectDb())
		exit(1);

	if(gethostname(cHostname,99)!=0)
	{
		printf("gethostname() failed\n");
		exit(1);
	}

	//
	//Get node and datacenter via hostname
	//
	sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf(mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
	}
	mysql_free_result(res);
	//FQDN vs short name of 2nd NIC mess
	if(!uNode)
	{
		if((cp=strchr(cHostname,'.')))
			*cp=0;
		sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf(mysql_error(&gMysql));
			mysql_close(&gMysql);
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uNode);
			sscanf(field[1],"%u",&uDatacenter);
		}
		mysql_free_result(res);
	}
	printf("uNode=%u uDatacenter=%u\n",uNode,uDatacenter);

	//
	//Now we can read the config file
	//
	if((fp=fopen(cConfigfileName,"r"))==NULL)
	{
		printf("Could not open %s\n",cConfigfileName);
		exit(1);
	}


	//config file has two sections
	unsigned uList=0;

	//these two are for section two
	char cLabel[33]={""};
	char cCloneLabel[33]={""};
	char cCloneHostname[100]={""};
	char cAltLabel[33]={""};
	//char cHostname[100]={""};

	char cIPv4ClassC[16]={""};
	unsigned uIPv4=0;

	char cPasswd[16]={""};
	char cTimeZone[32]={""};//PST8PDT

	char cIPv4CloneClassC[16]={""};
	unsigned uCloneIPv4=0;

	unsigned uConfig=0;
	unsigned uNameserver=0;
	unsigned uSearchdomain=0;
	unsigned uGroup=0;
	unsigned uOwner=0;
	unsigned uOSTemplate=0;
	unsigned uOSLoopTemplate=0;
	unsigned uDNSJob=0;
	unsigned uSyncPeriod=0;
	unsigned uCloneTargetNode=0;
	unsigned uCloneStopped=1;

	while(fgets(gcQuery,1024,fp)!=NULL)
	{
		if(gcQuery[0]=='#')
			continue;

		if(!strncmp(gcQuery,"ContainerList",strlen("ContainerList")))
		{
			uList=1;
			printf("Global settings\n");
			printf("\tcIPv4ClassC=%s\n",cIPv4ClassC);
			printf("\tcIPv4CloneClassC=%s\n",cIPv4CloneClassC);
			printf("\tcPasswd=%s\n",cPasswd);
			printf("\tcTimeZone=%s\n",cTimeZone);
			printf("\tuOwner=%u\n",uOwner);
			printf("\tuConfig=%u\n",uConfig);
			printf("\tuNameserver=%u\n",uNameserver);
			printf("\tuSearchdomain=%u\n",uSearchdomain);
			printf("\tuGroup=%u\n",uGroup);
			printf("\tuOSTemplate=%u\n",uOSTemplate);
			printf("\tuSyncPeriod=%u\n",uSyncPeriod);
			printf("\tuDNSJob=%u\n",uDNSJob);
			printf("\tuCloneTargetNode=%u\n",uCloneTargetNode);
			printf("\tuCloneStopped=%u\n",uCloneStopped);
			continue;
		}

		if(!uList)
		{
			if(!strncmp(gcQuery,"cIPv4ClassC=",strlen("cIPv4ClassC")))
			{
				sprintf(cIPv4ClassC,"%.15s",gcQuery+strlen("cIPv4ClassC="));
				if((cp=strchr(cIPv4ClassC,';')))
					*cp=0;
			}
			else if(!strncmp(gcQuery,"cIPv4CloneClassC=",strlen("cIPv4CloneClassC")))
			{
				sprintf(cIPv4CloneClassC,"%.15s",gcQuery+strlen("cIPv4CloneClassC="));
				if((cp=strchr(cIPv4CloneClassC,';')))
					*cp=0;
			}
			else if(!strncmp(gcQuery,"cPasswd=",strlen("cPasswd")))
			{
				sprintf(cPasswd,"%.15s",gcQuery+strlen("cPasswd="));
				if((cp=strchr(cPasswd,';')))
					*cp=0;
			}
			else if(!strncmp(gcQuery,"cTimeZone=",strlen("cTimeZone")))
			{
				sprintf(cTimeZone,"%.31s",gcQuery+strlen("cTimeZone="));
				if((cp=strchr(cTimeZone,';')))
					*cp=0;
			}
			else if(!strncmp(gcQuery,"uOwner=",strlen("uOwner=")))
				sscanf(gcQuery,"uOwner=%u;",&uOwner);
			else if(!strncmp(gcQuery,"uConfig=",strlen("uConfig=")))
				sscanf(gcQuery,"uConfig=%u;",&uConfig);
			else if(!strncmp(gcQuery,"uNameserver=",strlen("uNameserver=")))
				sscanf(gcQuery,"uNameserver=%u;",&uNameserver);
			else if(!strncmp(gcQuery,"uSearchdomain=",strlen("uSearchdomain=")))
				sscanf(gcQuery,"uSearchdomain=%u;",&uSearchdomain);
			else if(!strncmp(gcQuery,"uGroup=",strlen("uGroup=")))
				sscanf(gcQuery,"uGroup=%u;",&uGroup);
			else if(!strncmp(gcQuery,"uOSTemplate=",strlen("uOSTemplate=")))
				sscanf(gcQuery,"uOSTemplate=%u;",&uOSTemplate);
			else if(!strncmp(gcQuery,"uSyncPeriod=",strlen("uSyncPeriod=")))
				sscanf(gcQuery,"uSyncPeriod=%u;",&uSyncPeriod);
			else if(!strncmp(gcQuery,"uDNSJob=",strlen("uDNSJob=")))
				sscanf(gcQuery,"uDNSJob=%u;",&uDNSJob);
			else if(!strncmp(gcQuery,"uCloneTargetNode=",strlen("uCloneTargetNode=")))
				sscanf(gcQuery,"uCloneTargetNode=%u;",&uCloneTargetNode);
			else if(!strncmp(gcQuery,"uCloneStopped=",strlen("uCloneStopped=")))
				sscanf(gcQuery,"uCloneStopped=%u;",&uCloneStopped);
		}
		else
		{
			cHostname[0]=0;
			cLabel[0]=0;
			cAltLabel[0]=0;
			if((cp=strchr(gcQuery,'.')))
			{
				*cp=0;
				sprintf(cLabel,"%.31s",gcQuery);
				*cp='.';
				if((cp=strchr(gcQuery,',')))
				{
					*cp=0;
					sprintf(cAltLabel,"%.31s",cp+1);
				}
				sprintf(cHostname,"%.99s",gcQuery);
				if((cp=strchr(cHostname,'\n')))
					*cp=0;
				if((cp=strchr(cAltLabel,'\n')))
					*cp=0;
				printf("\ncHostname=%s cLabel=%s",cHostname,cLabel);
				if(cAltLabel[0])
					printf(" cAltLabel=%s\n",cAltLabel);
				else
					printf("\n");


				//
				//Gather data from config settings
				//

				//Get some available IPs for this new container
				if(!cIPv4ClassC[0])
				{
					printf("cIPv4ClassC must be specified\n");
					continue;
				}
				if(!cIPv4CloneClassC[0])
				{
					printf("cIPv4CloneClassC must be specified\n");
					continue;
				}
				sprintf(gcQuery,"SELECT uIP,cLabel FROM tIP WHERE uAvailable=1 AND"
						" cLabel LIKE '%s%%'"
						" AND uOwner=%u AND uDatacenter=%u",cIPv4ClassC,uOwner,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					sscanf(field[0],"%u",&uIPv4);
					printf("cIPv4ClassC IP %s\n",field[1]);
				}
				mysql_free_result(res);
				sprintf(gcQuery,"SELECT uIP,cLabel FROM tIP WHERE uAvailable=1 AND"
						" cLabel LIKE '%s%%'"
						" AND uOwner=%u AND uDatacenter=%u",cIPv4CloneClassC,uOwner,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
					sscanf(field[0],"%u",&uCloneIPv4);
					printf("cIPv4CloneClassC IP %s\n",field[1]);
				}
				mysql_free_result(res);

				if(!uOSTemplate)
				{
					uOSLoopTemplate=0;

					if(!cAltLabel[0])
						sprintf(cAltLabel,"%31s",cLabel);

					//Try to find a matching tOSTemplate
					sprintf(gcQuery,"SELECT uOSTemplate,cLabel FROM tOSTemplate WHERE "
						" cLabel LIKE '%%%s%%'",cAltLabel);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)==1)
					{
						field=mysql_fetch_row(res);
						sscanf(field[0],"%u",&uOSLoopTemplate);
						printf("tOSTemplate %s\n",field[1]);
					}
					else
					{
						printf("No single tOSTemplate match found based on:%s\n",cAltLabel);
						//printf("%s\n",gcQuery);
					}
					mysql_free_result(res);
				}
				else
				{
					uOSLoopTemplate=uOSTemplate;
				}

				//
				//here we start creating the new container jobs
				//after input validation
				//
				if(uDatacenter==0)
				{
					printf("Unexpected uDatacenter==0!\n");
					continue;
				}
				if(uNode==0)
				{
					printf("Unexpected uNode==0!\n");
					continue;
				}

				char cNCMDatacenter[256]={""};
				GetDatacenterProp(uDatacenter,"NewContainerMode",cNCMDatacenter);
				if(cNCMDatacenter[0] && strcmp(cNCMDatacenter,"Active"))
				{
					printf("Selected datacenter is full or not active. Select another.\n");
					continue;
				}

				char cNCMNode[256]={""};
				GetNodeProp(uNode,"NewContainerMode",cNCMNode);
				if(cNCMNode[0] && strcmp(cNCMNode,"Active"))
				{
					printf("Selected node is not configured for active containers."
							"Select another.\n");
					continue;
				}

				unsigned uLabelLen=0;
				if((uLabelLen=strlen(cLabel))<2)
				{
					printf("cLabel is too short\n");
					continue;
				}
				if(strchr(cLabel,'.'))
				{
					printf("cLabel has at least one '.'\n");
					continue;
				}
				if(strstr(cLabel,"-clone"))
				{
					printf("cLabel can't have '-clone'\n");
					continue;
				}
				unsigned uCreateAppliance=0;
				char gcIPv4[32]={""};
				if(uCreateAppliance)
				{
					if(!strstr(cLabel+(uLabelLen-strlen("-app")-1),"-app"))
					{
						printf("Appliance cLabel must end with '-app'.\n");
						continue;
					}
					if(strlen(gcIPv4)<7 || strlen(gcIPv4)>15)
					{
						printf("Appliance requires valid gcIPv4.\n");
						continue;
					}

					unsigned a=0,b=0,c=0,d=0;
					sscanf(gcIPv4,"%u.%u.%u.%u",&a,&b,&c,&d);
					if(a==0 || d==0)
					{
						printf("Appliance requires valid gcIPv4.\n");
						continue;
					}
				}
				else
				{
					if(strstr(cLabel,"-app"))
					{
						printf("Normal container cLabel can't have '-app'\n");
						continue;
					}
				}

				unsigned uHostnameLen=0;
				if((uHostnameLen=strlen(cHostname))<5)
				{
					printf("cHostname is too short\n");
					continue;
				}
				if(cHostname[uHostnameLen-1]=='.')
				{
					printf("cHostname can't end with a '.'\n");
					continue;
				}
				if(strstr(cHostname+(uHostnameLen-strlen(".cloneNN")-1),".clone"))
				{
					printf("cHostname can't end with '.cloneN'\n");
					continue;
				}
				//New rule: cLabel must be first part (first stop) of cHostname.
				if(strncmp(cLabel,cHostname,uLabelLen))
				{
					printf("cLabel must be first part of cHostname.\n");
					continue;
				}
				if(uIPv4==0)
				{
					printf("You must select a uIPv4\n");
					continue;
				}

				//Let's not allow same cLabel containers in our system for now.
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE cLabel='%s'",cLabel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
				{
					mysql_free_result(res);
					printf("cLabel already in use\n");
					continue;
				}
				mysql_free_result(res);

				char cNCCloneRange[256]={""};
				GetDatacenterProp(uDatacenter,"NewContainerCloneRange",cNCCloneRange);
				if(cNCCloneRange[0] && uIpv4InCIDR4(ForeignKey("tIP","cLabel",uIPv4),cNCCloneRange))
				{
					printf("uIPv4 must not be in datacenter clone IP range\n");
					continue;
				}

				if(uOSLoopTemplate==0)
				{
					printf("You must select a uOSTemplate\n");
					continue;
				}
				if(uConfig==0)
				{
					printf("You must select a uConfig\n");
					continue;
				}
				if(uNameserver==0)
				{
					printf("You must select a uNameserver\n");
					continue;
				}
				if(uSearchdomain==0)
				{
					printf("You must select a uSearchdomain\n");
					continue;
				}

				if(uGroup==0)
				{
					printf("Group is now required\n");
					continue;
				}

				//DNS sanity check
				if(uDNSJob)
				{
					char cunxsBindARecordJobZone[256]={""};
					GetConfiguration("cunxsBindARecordJobZone",cunxsBindARecordJobZone,uDatacenter,0,0,0);
					if(!cunxsBindARecordJobZone[0])
					{
						printf("Create job for unxsBind,"
								" but no cunxsBindARecordJobZone\n");
						continue;
					}
					
					if(!strstr(cHostname+(uHostnameLen-strlen(cunxsBindARecordJobZone)-1),cunxsBindARecordJobZone))
					{
						printf("cHostname must end with cunxsBindARecordJobZone\n");
						continue;
					}
				}
					
				if(cPasswd[0] && strlen(cPasswd)<6)
				{
					printf("Optional password must be at least 6 chars\n");
					continue;
				}
				//Direct datacenter checks
				unsigned uIPv4Datacenter=0;
				sscanf(ForeignKey("tIP","uDatacenter",uIPv4),"%u",&uIPv4Datacenter);
				if(uDatacenter!=uIPv4Datacenter)
				{
					printf("The specified uIPv4 does not "
							"belong to the specified uDatacenter.\n");
					continue;
				}
				unsigned uNodeDatacenter=0;
				sscanf(ForeignKey("tNode","uDatacenter",uNode),"%u",&uNodeDatacenter);
				if(uDatacenter!=uNodeDatacenter)
				{
					printf("The specified uNode does not "
							"belong to the specified uDatacenter.\n");
					continue;
				}

				//If auto clone setup check required values
				char cAutoCloneNode[256]={""};
				GetConfiguration("cAutoCloneNode",cAutoCloneNode,uDatacenter,0,0,0);
				if(cAutoCloneNode[0])
				{
					if(uCloneTargetNode==0)
					{
						printf("Please select a valid target node"
								" for the clone\n");
						continue;
					}
					if(uCloneTargetNode==uNode)
					{
						printf("Can't clone to same node\n");
						continue;
					}

					GetNodeProp(uCloneTargetNode,"NewContainerMode",cNCMNode);
					if(cNCMNode[0] && strcmp(cNCMNode,"Clone"))
					{
						printf("Selected clone target node is not configured for clone containers."
							"Select another.\n");
						continue;
					}

					sscanf(ForeignKey("tNode","uDatacenter",uCloneTargetNode),"%u",&uNodeDatacenter);
					if(uDatacenter!=uNodeDatacenter)
					{
						printf("The specified clone uNode does not "
							"belong to the specified uDatacenter.\n");
						continue;
					}

					char cTargetNodeIPv4[256]={""};
					GetNodeProp(uCloneTargetNode,"cIPv4",cTargetNodeIPv4);
					if(!cTargetNodeIPv4[0])
					{
						printf("Your target node is"
							" missing it's cIPv4 property\n");
						continue;
					}
					if(!uCloneIPv4)
					{
						printf("You must select an IP for the clone\n");
						continue;
					}
					if(uCloneIPv4==uIPv4)
					{
						printf("You must select a different IP for the"
										" clone\n");
						continue;
					}
					sscanf(ForeignKey("tIP","uDatacenter",uCloneIPv4),"%u",&uIPv4Datacenter);
					if(uDatacenter!=uIPv4Datacenter)
					{
						printf("The specified uIPv4 does not "
							"belong to the specified uDatacenter.\n");
						continue;
					}
					if(cNCCloneRange[0] && !uIpv4InCIDR4(ForeignKey("tIP","cLabel",uCloneIPv4),cNCCloneRange))
					{
						printf("Clone start uIPv4 must be in datacenter clone IP range\n");
						continue;
					}
					if(uSyncPeriod>86400*30 || (uSyncPeriod && uSyncPeriod<300))
					{
						printf("Clone uSyncPeriod out of range:"
								" Max 30 days, min 5 minutes or 0 off.\n");
						continue;
					}
				}//if(cAutoCloneNode[0])

				//TODO review this policy.
				//No same names or hostnames for same datacenter allowed.
				//TODO periods "." should be expanded to "[.]"
				//for correct cHostname REGEXP.
				sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE ("
							" cHostname REGEXP '^%s[0-9]+%s$'"
							" OR cLabel REGEXP '%s[0-9]+$'"
							" ) AND uDatacenter=%u LIMIT 1",
								cLabel,cHostname,cLabel,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
				{
					mysql_free_result(res);
					printf("Similar cHostname cLabel pattern already used at this datacenter!\n");
					continue;
				}
				mysql_free_result(res);

				//
				//Everything is ready and checked so we start modifying things
				//
				unsigned uContainer=0;
				sprintf(gcQuery,"INSERT INTO tContainer SET cLabel='%s',cHostname='%s',"
							"uIPv4=%u,"
							"uDatacenter=%u,"
							"uNode=%u,"
							"uOSTemplate=%u,"
							"uConfig=%u,"
							"uNameserver=%u,"
							"uSearchdomain=%u,"
							"uStatus=%u,"
							"uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
								cLabel,cHostname,
								uIPv4,
								uDatacenter,
								uNode,
								uOSLoopTemplate,
								uConfig,
								uNameserver,
								uSearchdomain,
								uINITSETUP,
								uOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				uContainer=mysql_insert_id(&gMysql);
				if(!uContainer)
				{
					printf("uContainer not determined!!");
					continue;
				}

				//Add to group
				if(uGroup)
					ChangeGroup(uContainer,uGroup);

				//tIP
				sprintf(gcQuery,"UPDATE tIP SET uAvailable=0"
						" WHERE uIP=%u AND uAvailable=1 AND uDatacenter=%u",
									uIPv4,uDatacenter);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				if(mysql_affected_rows(&gMysql)!=1)
				{
					sprintf(gcQuery,"DELETE FROM tContainer WHERE uContainer=%u",
						uContainer);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					printf("Someone grabbed your IP container creation aborted!");
					continue;
				}
				
				//Add properties
				//Name property
				sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
					",uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
					",cName='Name',cValue='%s'",
						uContainer,uOwner,cLabel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				//cPasswd property
				sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType=3"
					",uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
					",cName='cPasswd',cValue='%s'",
						uContainer,uOwner,cPasswd);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				//Optional timezone note the --- not selected value.
				if(cTimeZone[0])
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_TimeZone',cValue='%s',uType=3,uKey=%u"
						",uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							cTimeZone,uContainer,uOwner);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				}

				if(cAutoCloneNode[0])
				{
					unsigned uNewVeid=0;
					unsigned uStatus=uINITSETUP;

					uNewVeid=CommonCloneContainer(  uContainer,
									uOSLoopTemplate,
									uConfig,
									uNameserver,
									uSearchdomain,
									uDatacenter,
									uDatacenter,
									uOwner,
									cLabel,
									uNode,
									uStatus,
									cHostname,
									cIPv4CloneClassC,
									uCloneIPv4,
									cCloneLabel,
									cCloneHostname,
									uCloneTargetNode,
									uSyncPeriod,
									1,
									uCloneStopped);
					if(!uNewVeid)
					{
						printf("Clone container %s creation failed!",cCloneHostname);
					}
					else
					{
						SetContainerStatus(uContainer,uINITSETUP);//See CommonCloneContainer()
						if(uGroup)
							ChangeGroup(uNewVeid,uGroup);
					}
				}//cAutoCloneNode

				if(uDNSJob)
					CreateDNSJob(uIPv4,uOwner,NULL,cHostname,uDatacenter,1);

			}//valid hostname and label
		}
	}


	fclose(fp);

	printf("\nMassCreateContainers(): End\n");

}//void MassCreateContainers(char *cConfigfileName)
