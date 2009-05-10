/*
FILE
	main.c
	$Id: main.c 618 2007-06-13 14:00:42Z Gary $
AUTHOR
	(C) 2006-2007, Gary Wallis for Unixservice USA
PURPOSE
	Commandline only interface for named (DNS/BIND 9.3.3) query logging.
	Parse one line and increment tQueryLog.uHits for tQueryLog.cZone.
NOTES
	The queries counted by cZone are in fact for specific RRs of the
	given zone. This may be difficult to discern in some cases and
	may not be historically accurate.

SAMPLE LINE
	client 209.177.154.67#52543: query: dsl-189-131-60-146.prod-infinitum.com.mx IN A
TODO
	Add signal handler for clean exit on any kill sig
*/

#include "../../mysqlrad.h"

#define DBIP NULL
#define LOGALL

#define PIPELOG "/var/log/named/named-query.log"
#define ERRLOG "/tmp/idns-querylog.log"

MYSQL gMysql;

void daemonize(void);

int main(int iArgc, char *cArgv[])
{
	char cBuffer[512];
	char *cp,*cp2;
	char cRR[256];
	char cQuery[512];
#ifndef LOGALL
	MYSQL_RES *res;
	MYSQL_ROW field;
#endif
	MYSQL_RES *res2;
	FILE *ifp,*efp;

	if(iArgc==2 && !strncmp(cArgv[1],"--fg",4))
	{
		printf("Running %s in foreground\n",cArgv[0]);
	}
	else if(iArgc==2 && !strncmp(cArgv[1],"--help",6))
	{
		printf("%s help\n\n--help\tthis page\n--fg\trun program in foreground mode\n",
				cArgv[0]);
		return(0);
	}
	else if(iArgc>=2)
	{
		printf("Usage: %s [--help | --fg]\n",cArgv[0]);
		return(0);
	}
	else if(1)
	{
		daemonize();
	}

	//This is a named pipe. It will block i/o, so will named crash if this
	//daemon dies? Please test with utmost caution.
	if((ifp=fopen(PIPELOG,"r"))==NULL)
	{
                fprintf(stderr,"Could not open log pipe!\n");
		return(2);
        }

	if((efp=fopen(ERRLOG,"a"))==NULL)
	{
                fprintf(stderr,"Could not open error log!\n");
		return(3);
        }


        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,DBIP,DBLOGIN,DBPASSWD,DBNAME,0,NULL,0))
        {
                fprintf(efp,"Database server unavailable!\n");
		goto Return_Point;
        }

	fprintf(efp,"%s started and mysql_init() and connect() ran ok.\n",cArgv[0]);

	while(1)
	{
		while(fgets(cBuffer,512,ifp))
		{
			cRR[0]=0;
			//Debug only
			//fprintf(stdout,"%s",cBuffer);

			//1-. Build SQL to find tZone.cZone best effort
			//	See if any tZone.cZone is contained in query RR
			if((cp=strstr(cBuffer,"query: ")))
			{
				if((cp2=strchr(cp+7,' ')))
				{
					*cp2=0;
					sprintf(cRR,"%.100s",cp+7);
				}
			}

			//Not empty and no chars that will break cQuery security and function
			//No truncated super long strange queries that break the unique cZone
			if(cRR[0] && !strchr(cRR,'\'') && (strlen(cRR)<255))
			{
#ifndef LOGALL
				sprintf(cQuery,"SELECT cZone FROM tZone WHERE '%s' LIKE CONCAT('%%',cZone)",cRR);
				//Debug only
				//fprintf(stdout,"%s\n",cQuery);
				mysql_query(&gMysql,cQuery);
				if(mysql_errno(&gMysql))
				{
					fprintf(efp,"mySQL Error:%s\n",mysql_error(&gMysql));
					goto Return_Point;
				}
				
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
				{
#endif
					//Debug only
					//fprintf(stdout,"tZone.cZone=%s\n",field[0]);
					//mysql_free_result(res);
					//continue;
					//
					//2-. Insert or update tHits based on if a tHits.cZone in 1-.
					//	already exists.
					//
					sprintf(cQuery,"SELECT uHit FROM tHit WHERE cZone='%s'\n",
#ifndef LOGALL
							field[0]);
#else
							cRR);
#endif
					//Debug only
					//fprintf(stdout,"%s\n",cQuery);
					mysql_query(&gMysql,cQuery);
					if(mysql_errno(&gMysql))
					{
						fprintf(efp,"mySQL Error:%s\n",
								mysql_error(&gMysql));
						goto Return_Point;
					}
					res2=mysql_store_result(&gMysql);
					if(mysql_num_rows(res2))
					{
						sprintf(cQuery,"UPDATE tHit SET uHitCount=uHitCount+1,uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1 WHERE cZone='%s'",
#ifndef LOGALL
							field[0]);
#else
							cRR);
#endif
						//Debug only
						//fprintf(stdout,"%s\n",cQuery);
						mysql_query(&gMysql,cQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(efp,"mySQL Error:%s\n",
									mysql_error(&gMysql));
							goto Return_Point;
						}
					}
					else
					{
						sprintf(cQuery,"INSERT INTO tHit SET cZone='%s',uHitCount=1,uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
#ifndef LOGALL
							field[0]);
#else
							cRR);
#endif
						//Debug only
						//fprintf(stdout,"%s\n",cQuery);
						mysql_query(&gMysql,cQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(efp,"mySQL Error:%s\n",
									mysql_error(&gMysql));
							goto Return_Point;
						}
					}
					mysql_free_result(res2);
#ifndef LOGALL
				}//if((field=mysql_fetch_row(res)))
				mysql_free_result(res);
#endif
			}//if(cRR[0])
		}//while(fgets(cBuffer,512,ifp))
	}//while(1)

	//As while(1) these lines are never reached are they?
Return_Point:
	fclose(ifp);
	fprintf(efp,"%s ended with a signal or error.\n",cArgv[0]);
	fclose(efp);
	mysql_close(&gMysql);
	return(0);

}//main()


void daemonize(void)
{
	switch(fork())
	{
		default:
			_exit(0);

		case -1:
			fprintf(stderr,"fork failed\n");
			_exit(1);

		case 0:
		break;
	}

	if(setsid()<0)
	{
		fprintf(stderr,"setsid failed\n");
		_exit(1);
	}

}//void daemonize(void)

