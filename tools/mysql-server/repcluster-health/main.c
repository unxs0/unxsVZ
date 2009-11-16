#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>

int main(void)
{
	char *cServerIps={"70.38.123.40","70.38.123.41","70.38.123.42",""};
	char *cDbLogin="test";
	char *cDbPwd="wsx3edc";
	char *cDbName="test";
	MySQL MySQLVectors[5]; //Up to 6 MySQL servers can be define in the cServerIps array

	ConnectMySQL(&MySQLVectors,cServerIps,cDbLogin,cDbPwd,cDbName);

}//main


void ConnectMySQL(MYSQL *aMySQL[],char *cIPs[],char *cLogin,char *cPwd,char *cDb)
{
	int i=0;
	while(1)
	{
		mysql_init(aMySQL[i]);
		if(mysql_real_connect(aMySQL[i],cIPs[i],cLogin,cPwd,cDb,NULL,0))
		{
			printf("Fatal: failed to connect MySQL vector %i.\n");
			printf("IP address    : %s\n",cIPs[i]);
			printf("Error message : %s\n",mysql_error(aMySQL[i]));
			exit(EXIT_FAILURE);
		}
	}

Ã//void ConnectMySQ

