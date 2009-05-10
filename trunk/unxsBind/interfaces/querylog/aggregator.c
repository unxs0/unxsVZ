/*
FILE
	aggregator.c
	$Id: main.c 601 2007-06-02 00:46:28Z ggw $
AUTHOR
	(C) 2006-2007, Gary Wallis for Unixservice USA
PURPOSE
	Aggregate table passed as arg1 query hit data to tHit table
NOTES
*/

#include "../../mysqlrad.h"

MYSQL gMysql;

int main(int iArgc, char *cArgv[])
{
	char cQuery[512];
	MYSQL_RES *res;
	MYSQL_ROW field;
	MYSQL_RES *res2;
	MYSQL_ROW field2;
	MYSQL_RES *res3;

	if(iArgc==2 && !strncmp(cArgv[1],"--help",6))
	{
		printf("%s help\n\n--help\tthis page\n",
				cArgv[0]);
		return(0);
	}
	else if(iArgc!=2)
	{
		printf("Usage: %s <tHit tale name from aggregator.sh> | [--help]\n",cArgv[0]);
		return(0);
	}

        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,DBIP,DBLOGIN,DBPASSWD,DBNAME,0,NULL,0))
        {
                fprintf(stdout,"Database server unavailable!\n");
		exit(1);
        }

	fprintf(stdout,"%s started and mysql_init() and connect() ran ok.\n",cArgv[0]);


	//For every record in cArgv[1] table
	sprintf(cQuery,"SELECT cZone,uHitCount FROM %s",cArgv[1]);
	//Debug only
	//fprintf(stdout,"%s\n",cQuery);
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stdout,"mySQL Error:%s\n",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{

		sprintf(cQuery,"SELECT cZone FROM tZone WHERE '%s' LIKE CONCAT('%%',cZone)",
					field[0]);
		//Debug only
		//fprintf(stdout,"%s\n",cQuery);
		mysql_query(&gMysql,cQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stdout,"mySQL Error:%s\n",mysql_error(&gMysql));
			exit(2);
		}
		res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			//Debug only
			//fprintf(stdout,"tZone.cZone=%s\n",field2[0]);

			//2-. Insert or update tHits based on if a tHits.cZone in 1-.
			//	already exists.
			//
			sprintf(cQuery,"SELECT uHit FROM tHit WHERE cZone='%s'",field2[0]);
			//Debug only
			//fprintf(stdout,"%s\n",cQuery);
			mysql_query(&gMysql,cQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stdout,"mySQL Error:%s\n",mysql_error(&gMysql));
				exit(3);
			}
			res3=mysql_store_result(&gMysql);
			if(mysql_num_rows(res3))
			{
				sprintf(cQuery,"UPDATE tHit SET uHitCount=uHitCount+%s,uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1 WHERE cZone='%s'",field[1],field2[0]);
				//Debug only
				//fprintf(stdout,"%s\n",cQuery);
				mysql_query(&gMysql,cQuery);
				if(mysql_errno(&gMysql))
				{
					fprintf(stdout,"mySQL Error:%s\n",mysql_error(&gMysql));
					exit(4);
				}
			}
			else
			{
				sprintf(cQuery,"INSERT INTO tHit SET cZone='%s',uHitCount=%s,uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",field2[0],field[1]);
				//Debug only
				//fprintf(stdout,"%s\n",cQuery);
				mysql_query(&gMysql,cQuery);
				if(mysql_errno(&gMysql))
				{
					fprintf(stdout,"mySQL Error:%s\n",mysql_error(&gMysql));
					exit(5);
				}
			}
			mysql_free_result(res3);
			//Debug only
			//exit(0);
		}//if((field2=mysql_fetch_row(res2)))
		mysql_free_result(res2);
	}//while((field=mysql_fetch_row(res)))
	mysql_free_result(res);

	mysql_close(&gMysql);
	return(0);

}//main()
