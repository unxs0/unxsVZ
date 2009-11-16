#include <stdio.h>
#include <mysql/mysql.h>
#include <stdlib.h>

void ConnectMySQL(MYSQL *aMySQL,char *cIP,char *cLogin,char *cPwd,char *cDb);

int main(void)
{
	char *cServerIps[5]={"70.38.123.40","70.38.123.41","70.38.123.42",""};
	char *cDbLogin="test";
	char *cDbPwd="wsx3edc";
	char *cDbName="test";
	char cQuery[1024]={""};

	MYSQL MySQLVectors[5]; //Up to 6 MySQL servers can be define in the cServerIps array
	int i=0;
	int j;
	int x=0;

	while(1)
	{
		ConnectMySQL(&MySQLVectors[i],cServerIps[i],cDbLogin,cDbPwd,cDbName);
		if(!cServerIps[i+1]) break;
		i++;
	}
	printf("Connected to %i MySQL servers OK\n",i);
	
	for(j=0;j<i;j++)
	{
		sprintf(cQuery,"CREATE TABLE delme%i (cData VARCHAR 16)",j);
		mysql_query(&MySQLVectors[j],cQuery);
		if(mysql_errno(&MySQLVectors[j]))
		{
			printf("Fatal: CREATE TABLE failed for server #%i\n",j);
			printf("Error was: %s\n",mysql_error(&MySQLVectors[j]));
			exit(EXIT_FAILURE);
		}
	}
	for(j=0;j<i;j++)
		mysql_close(&MySQLVectors[j]);
	
	printf("Disconnected from %i MySQL servers OK\n",j);

	return(EXIT_SUCCESS);

}//main


void ConnectMySQL(MYSQL *aMySQL,char *cIP,char *cLogin,char *cPwd,char *cDb)
{
	mysql_init(aMySQL);
	if(mysql_real_connect(aMySQL,cIP,cLogin,cPwd,cDb,0,NULL,0))
	{
		printf("Fatal: failed to connect MySQL vector");
		printf("IP address    : %s\n",cIP);
		printf("Error message : %s\n",mysql_error(aMySQL));
		exit(EXIT_FAILURE);
	}

}//void ConnectMySQL(MYSQL *aMySQL[],char *cIPs[],char *cLogin,char *cPwd,char *cDb)

