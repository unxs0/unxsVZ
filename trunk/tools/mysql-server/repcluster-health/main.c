#include <stdio.h>
#include <mysql/mysql.h>
#include <stdlib.h>

char gcQuery[1024]={""};

void ConnectMySQL(MYSQL *aMySQL,char *cIP,char *cLogin,char *cPwd,char *cDb);
void CreateTestTable(MYSQL *MySQLVector,int iIndex);
void RemoveTestTable(MYSQL *MySQLVector,int iIndex);
void CheckTestTable(MYSQL *MySQLVector,int iServerIndex,int iTableIndex);


int main(void)
{
	char *cServerIps[5]={"70.38.123.40","70.38.123.41","70.38.123.42",""};
	char *cDbLogin="test";
	char *cDbPwd="wsxedc";
	char *cDbName="test";

	MYSQL MySQLVectors[5]; //Up to 6 MySQL servers can be defined in the cServerIps array
	int i=0;
	int j;
	int x;

	while(1)
	{
		ConnectMySQL(&MySQLVectors[i],cServerIps[i],cDbLogin,cDbPwd,cDbName);
		i++;
		if(!cServerIps[i][0]) break;
	}
	printf("Connected to %i MySQL servers OK\n",i);
	
	for(j=0;j<i;j++)
		CreateTestTable(&MySQLVectors[j],j);

	for(j=0;j<i;j++)
	{
		for(x=0;x<i;x++)
		{
			CheckTestTable(&MySQLVectors[j],j,x);
		}
	}
	
	for(j=0;j<i;j++)
		RemoveTestTable(&MySQLVectors[j],j);


	for(j=0;j<i;j++)
		mysql_close(&MySQLVectors[j]);
	
	printf("Disconnected from %i MySQL servers OK\n",j);

	return(EXIT_SUCCESS);

}//main


void ConnectMySQL(MYSQL *aMySQL,char *cIP,char *cLogin,char *cPwd,char *cDb)
{
	mysql_init(aMySQL);
	if(!mysql_real_connect(aMySQL,cIP,cLogin,cPwd,cDb,0,NULL,0))
	{
		printf("Fatal: failed to connect MySQL vector");
		printf("IP address    : %s\n",cIP);
		printf("Error message : %s\n",mysql_error(aMySQL));
		exit(EXIT_FAILURE);
	}
	
}//void ConnectMySQL(MYSQL *aMySQL[],char *cIPs[],char *cLogin,char *cPwd,char *cDb)


void CreateTestTable(MYSQL *MySQLVector,int iIndex)
{
	sprintf(gcQuery,"CREATE TABLE delme%i (cData VARCHAR(16) NOT NULL DEFAULT '')",iIndex);
	mysql_query(MySQLVector,gcQuery);
	if(mysql_errno(MySQLVector))
	{
		printf("Fatal: CREATE TABLE failed for server #%i\n",iIndex);
		printf("Error was: %s\n",mysql_error(MySQLVector));
		exit(EXIT_FAILURE);
	}

}//void CreateTestTable(MYSQL MySQLVector,int iIndex)


void RemoveTestTable(MYSQL *MySQLVector,int iIndex)
{
	sprintf(gcQuery,"DROP TABLE delme%i",iIndex);
	mysql_query(MySQLVector,gcQuery);
	if(mysql_errno(MySQLVector))
	{
		printf("Fatal: DROP TABLE failed for server #%i\n",iIndex);
		printf("Error was: %s\n",mysql_error(MySQLVector));
		exit(EXIT_FAILURE);
	}
}//void RemoveTestTable(MYSQL *MySQLVector,int iIndex)


void CheckTestTable(MYSQL *MySQLVector,int iServerIndex,int iTableIndex)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"CHECKSUM TABLE delme%i",iTableIndex);
	mysql_query(MySQLVector,gcQuery);
	if(mysql_errno(MySQLVector))
	{
		printf("Fatal: CHECKSUM TABLE failed for server #%i\n",iServerIndex);
		printf("Error was: %s\n",mysql_error(MySQLVector));
		exit(EXIT_FAILURE);
	}
	res=mysql_store_result(MySQLVector);
	field=mysql_fetch_row(res);
	if(field[1]==NULL)
	{
		printf("Replication error: table delme%i was not found at server #%i\n",iTableIndex,iServerIndex);
		printf("field[0]='%s'\n",field[0]);
		exit(EXIT_FAILURE);
	}
	mysql_free_result(res);

}//void CheckTestTable(MYSQL *MySQLVector,int iServerIndex,int iTableIndex)

