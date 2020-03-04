/*
AUTHOR/LEGAL
	(C) 2010-2020 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	For judging heat scores
*/

#include "interface.h"

#include <math.h>

//File globals
static unsigned uGenerate=0;
static unsigned suEvent=0;
static unsigned suRound=0;
static unsigned suHeat=0;
static unsigned suMainScore=0;
static unsigned suScoreFamily=0;
static unsigned suScoreFamilyProvided=0;
static float sfFineScore=0.0;
static unsigned uTrickLock=0;
static unsigned uTrickLockProvided=0;
static char dEnd[32]={"Jan 1, 2021 15:37:25"};
static char scEvent[32]={""};
static char *gcBgColor="white";

//TOC
void ProcessJudgeVars(pentry entries[], int x);
void JudgeGetHook(entry gentries[],int x);
void JudgeCommands(pentry entries[], int x);
void AddHour(void);
void SubHour(void);
void AddDay(void);
void SubDay(void);
void funcNewJudge(FILE *fp);

void htmlHeat(void);
void htmlBestTrick(void);
void htmlHeatEnd(void);
void htmlEvent(void);
void htmlLive(void);
void htmlJudge(void);
void PopulateScoreComp(unsigned uHeat);
void AdvanceRidersToNextRound(unsigned uHeat,unsigned uEvent);
//extern
void unxsvzLog(unsigned uTablePK,char *cTableName,char *cLogEntry,unsigned guPermLevel,unsigned guLoginClient,char *gcLogin,char *gcHost);


unsigned uSelectRider(FILE *fp,char *cPage,unsigned uMode)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	if((!guHeat || !guEvent) && !uMode==2)
		return(0);

	if(!guRider || uMode)
	{
		if(uMode==2)
		sprintf(gcQuery,"SELECT DISTINCT tRider.cLabel,tRider.uRider FROM tRider,tScore"
				" WHERE tScore.uOwner=%u"
				" AND tRider.cLabel NOT LIKE 'WINNER%%'"
				" AND tRider.cLabel NOT LIKE 'SECOND%%'"
				" AND tScore.uEvent=%u AND tScore.uRider=tRider.uRider ORDER BY tRider.cLast",
						guOrg,
						guEvent);
		else
		sprintf(gcQuery,"SELECT DISTINCT tRider.cLabel,tRider.uRider FROM tRider,tScore"
				" WHERE tScore.uOwner=%u AND tScore.uHeat=%u"
				" AND tScore.uEvent=%u AND tScore.uRider=tRider.uRider ORDER BY tRider.cLast",
						guOrg,guHeat,
							guEvent);
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if(mysql_errno(&gMysql))
		{
			fprintf(fp,"%s",mysql_error(&gMysql));
			return(0);
		}
		if(!uMode) fprintf(fp,"<br>Select Rider");
		unsigned uRider=0;
		while((field=mysql_fetch_row(res)))
		{
			sscanf(field[1],"%u",&uRider);
			if(guRider!=uRider)
				fprintf(fp,"<br><a title='Change to this rider' href=?gcPage=%s&guRider=%u>%s</a>",cPage,uRider,field[0]);
		}
		return(0);
	}
	return(1);
}//unsigned uSelectRider(FILE *fp,char *cPage)


unsigned uSelectHeat(FILE *fp,char *cPage)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	if(!guEvent || !guRound)
		return(0);

	if(!guHeat)
	{
		sprintf(gcQuery,"SELECT cLabel,uHeat FROM tHeat"
				" WHERE uOwner=%u AND uEvent=%u AND uRound=%u ORDER BY uHeat",guOrg,guEvent,guRound);
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if(mysql_errno(&gMysql))
		{
			fprintf(fp,"%s",mysql_error(&gMysql));
			return(0);
		}
		fprintf(fp,"<br>Select Heat");
		while((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"<br><a href=?gcPage=%s&guHeat=%s>%s</a>",cPage,field[1],field[0]);
		}
		return(0);
	}
	return(1);
}//unsigned uSelectHeat(FILE *fp,char *cPage)


unsigned uSelectRound(FILE *fp,char *cPage)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	if(!guEvent)
		return(0);

	if(!guRound)
	{
		sprintf(gcQuery,"SELECT cLabel,uRound FROM tRound WHERE uOwner=%u AND uEvent=%u ORDER BY uRound",guOrg,guEvent);
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if(mysql_errno(&gMysql))
		{
			fprintf(fp,"%s",mysql_error(&gMysql));
			return(0);
		}
		fprintf(fp,"<br>Select Round");
		while((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"<br><a href=?gcPage=%s&guRound=%s>%s</a>",cPage,field[1],field[0]);
		}
		return(0);
	}
	return(1);
}//unsigned uSelectRound(FILE *fp,char *cPage)


unsigned uSelectEvent(FILE *fp,char *cPage)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

      if(!guEvent)
      {
	sprintf(gcQuery,"SELECT cLabel,uEvent FROM tEvent WHERE uOwner=%u AND uStatus=1",guOrg);
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return(0);
	}
	fprintf(fp,"<br>Select Event");
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<br><a href=?gcPage=%s&guEvent=%s>%s</a>",cPage,field[1],field[0]);
		sprintf(scEvent,"%.31s",field[0]);//TODO change to scEvent
	}
	return(0);
      }
      return(1);
}//unsigned uSelectEvent(FILE *fp,char *cPage)


void ProcessJudgeVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uHeat"))
			sscanf(entries[i].val,"%u",&suHeat);
		else if(!strcmp(entries[i].name,"uTrickLock"))
		{
			uTrickLockProvided=1;
			sscanf(entries[i].val,"%u",&uTrickLock);
		}
		else if(!strcmp(entries[i].name,"uEvent"))
			sscanf(entries[i].val,"%u",&suEvent);
		else if(!strcmp(entries[i].name,"uRound"))
			sscanf(entries[i].val,"%u",&suRound);
		else if(!strcmp(entries[i].name,"MainScore"))
			sscanf(entries[i].val,"%u",&suMainScore);
		else if(!strcmp(entries[i].name,"FineScore"))
			sscanf(entries[i].val,"%f",&sfFineScore);
		else if(!strcmp(entries[i].name,"ScoreFamily"))
		{
			sscanf(entries[i].val,"%u",&suScoreFamily);
			suScoreFamilyProvided=1;
		}
	}

}//void ProcessJudgeVars(pentry entries[], int x)


void AddHour(void)
{
	if(guPermLevel<10 || !guEvent)
		return;

	char cBuffer[256]={""};
		
	sprintf(gcQuery,"UPDATE tEvent SET"
			" dStart=DATE_ADD(dStart,INTERVAL 1 HOUR),"
			" uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uEvent=%u",
				guLoginClient,guEvent);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("AddHour","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}

	sprintf(gcQuery,"UPDATE tHeat SET"
			" dStart=DATE_ADD(dStart,INTERVAL 1 HOUR),"
			" dEnd=DATE_ADD(dEnd,INTERVAL 1 HOUR),"
			" uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uEvent=%u",
				guLoginClient,guEvent);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("AddHour","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}
}//void AddHour(void)


void SubHour(void)
{
	if(guPermLevel<10 || !guEvent)
		return;

	char cBuffer[256]={""};
		
	sprintf(gcQuery,"UPDATE tEvent SET"
			" dStart=DATE_SUB(dStart,INTERVAL 1 HOUR),"
			" uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uEvent=%u",
				guLoginClient,guEvent);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("SubHour","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}

	sprintf(gcQuery,"UPDATE tHeat SET"
			" dStart=DATE_SUB(dStart,INTERVAL 1 HOUR),"
			" dEnd=DATE_SUB(dEnd,INTERVAL 1 HOUR),"
			" uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uEvent=%u",
				guLoginClient,guEvent);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("SubHour","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}
}//void SubHour(void)


void AddDay(void)
{
	if(guPermLevel<10 || !guEvent)
		return;

	char cBuffer[256]={""};
		
	sprintf(gcQuery,"UPDATE tEvent SET"
			" dStart=DATE_ADD(dStart,INTERVAL 1 DAY),"
			" uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uEvent=%u",
				guLoginClient,guEvent);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("AddDay","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}

	sprintf(gcQuery,"UPDATE tHeat SET"
			" dStart=DATE_ADD(dStart,INTERVAL 1 DAY ),"
			" dEnd=DATE_ADD(dEnd,INTERVAL 1 DAY),"
			" uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uEvent=%u",
				guLoginClient,guEvent);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("AddDay","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}
}//void AddDay(void)


void SubDay(void)
{
	if(guPermLevel<10 || !guEvent)
		return;

	char cBuffer[256]={""};
		
	sprintf(gcQuery,"UPDATE tEvent SET"
			" dStart=DATE_SUB(dStart,INTERVAL 1 DAY),"
			" uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uEvent=%u",
				guLoginClient,guEvent);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("SubDay","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}

	sprintf(gcQuery,"UPDATE tHeat SET"
			" dStart=DATE_SUB(dStart,INTERVAL 1 DAY),"
			" dEnd=DATE_SUB(dEnd,INTERVAL 1 DAY),"
			" uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uEvent=%u",
				guLoginClient,guEvent);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("SubDay","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}
}//void SubDay(void)


void CommonGetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"guEvent"))
		{
			sscanf(gentries[i].val,"%u",&guEvent);
			printf("Set-Cookie: unxsEVEvent=%u; secure; httponly; samesite=strict;\n",guEvent);
		}
		else if(!strcmp(gentries[i].name,"guRound"))
		{
			sscanf(gentries[i].val,"%u",&guRound);
			printf("Set-Cookie: unxsEVRound=%u; secure; httponly; samesite=strict;\n",guRound);
		}
		else if(!strcmp(gentries[i].name,"guHeat"))
		{
			sscanf(gentries[i].val,"%u",&guHeat);
			printf("Set-Cookie: unxsEVHeat=%u; secure; httponly; samesite=strict;\n",guHeat);
		}
		else if(!strcmp(gentries[i].name,"guRider"))
		{
			sscanf(gentries[i].val,"%u",&guRider);
			printf("Set-Cookie: unxsEVRider=%u; secure; httponly; samesite=strict;\n",guRider);
		}
		else if(!strcmp(gentries[i].name,"ClearRound"))
		{
			guRound=0;
			guHeat=0;
			guRider=0;
			printf("Set-Cookie: unxsEVRound=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
			printf("Set-Cookie: unxsEVHeat=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
			printf("Set-Cookie: unxsEVRider=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
		}
		else if(!strcmp(gentries[i].name,"ClearHeat"))
		{
			guHeat=0;
			guRider=0;
			printf("Set-Cookie: unxsEVHeat=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
			printf("Set-Cookie: unxsEVRider=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
		}
		else if(!strcmp(gentries[i].name,"ClearRider"))
		{
			guRider=0;
			printf("Set-Cookie: unxsEVRider=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
		}
		else if(!strcmp(gentries[i].name,"ClearEvent"))
		{
			guEvent=0;
			guRound=0;
			guHeat=0;
			guRider=0;
			printf("Set-Cookie: unxsEVEvent=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
			printf("Set-Cookie: unxsEVRound=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
			printf("Set-Cookie: unxsEVHeat=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
			printf("Set-Cookie: unxsEVRider=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
		}
		else if(!strcmp(gentries[i].name,"ChromaKey"))
			gcBgColor="magenta";
		else if(!strcmp(gentries[i].name,"ToggleHelp"))
		{
			if(!guHelp)
			{
				printf("Set-Cookie: unxsEVHelp=1; secure; httponly; samesite=strict;\n");
				guHelp=1;
			}
			else
			{
				printf("Set-Cookie: unxsEVHelp=\"deleted\";"
					" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
				guHelp=0;
			}
		}
	}
}//void CommonGetHook(entry gentries[],int x);


void TournamentGetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uEvent"))
			sscanf(gentries[i].val,"%u",&suEvent);
		else if(!strcmp(gentries[i].name,"ChromaKey"))
			gcBgColor="magenta";
	}
	htmlTournament();

}//void TournamentGetHook(entry gentries[],int x)



void BestTrickGetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uEvent"))
			sscanf(gentries[i].val,"%u",&suEvent);
		else if(!strcmp(gentries[i].name,"ChromaKey"))
			gcBgColor="magenta";
	}
	htmlBestTrick();

}//void BestTrickGetHook(entry gentries[],int x)


void RiderGetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"ChromaKey"))
			gcBgColor="magenta";
	}
	htmlRider();

}//void RiderGetHook(entry gentries[],int x)


void WindGetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"ChromaKey"))
			gcBgColor="magenta";
	}
	htmlWind();

}//void WindGetHook(entry gentries[],int x)


void OverlayGetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uHeat"))
			sscanf(gentries[i].val,"%u",&suHeat);
		else if(!strcmp(gentries[i].name,"uEvent"))
			sscanf(gentries[i].val,"%u",&suEvent);
		else if(!strcmp(gentries[i].name,"ChromaKey"))
			gcBgColor="magenta";
	}
	htmlOverlay();

}//void OverlayGetHook(entry gentries[],int x)


void HeatEndGetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uHeat"))
			sscanf(gentries[i].val,"%u",&suHeat);
		else if(!strcmp(gentries[i].name,"ChromaKey"))
			gcBgColor="magenta";
	}
	htmlHeatEnd();

}//void HeatEndGetHook(entry gentries[],int x)


void HeatGetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uHeat"))
			sscanf(gentries[i].val,"%u",&suHeat);
		else if(!strcmp(gentries[i].name,"ChromaKey"))
			gcBgColor="magenta";
	}
	htmlHeat();

}//void HeatGetHook(entry gentries[],int x)


void JudgeGetHook(entry gentries[],int x)
{
	CommonGetHook(gentries,x);

	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uHeat"))
			sscanf(gentries[i].val,"%u",&suHeat);
	}
	htmlJudge();

}//void JudgeGetHook(entry gentries[],int x)


void EventGetHook(entry gentries[],int x)
{

	CommonGetHook(gentries,x);
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"AddHour"))
		{
			AddHour();
		}
		else if(!strcmp(gentries[i].name,"SubHour"))
		{
			SubHour();
		}
		else if(!strcmp(gentries[i].name,"AddDay"))
		{
			AddDay();
		}
		else if(!strcmp(gentries[i].name,"SubDay"))
		{
			SubDay();
		}
	}

	htmlEvent();

}//void EventGetHook(entry gentries[],int x)


void LiveGetHook(entry gentries[],int x)
{

	CommonGetHook(gentries,x);
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uRider"))
		{
		}
	}

	htmlLive();

}//void LiveGetHook(entry gentries[],int x)


void AdminGetHook(entry gentries[],int x)
{

	CommonGetHook(gentries,x);

	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"Generate"))
		uGenerate=1;
	}

	htmlAdmin();

}//void AdminGetHook(entry gentries[],int x)


void UpdateInsertScore(float fScore,unsigned uIndex,unsigned uEvent, unsigned uRound,unsigned uHeat,unsigned uRider);
void UpdateInsertScore(float fScore,unsigned uIndex,unsigned uEvent, unsigned uRound,unsigned uHeat,unsigned uRider)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uStatus=0;

	sscanf(cForeignKey("tHeat","uStatus",uHeat),"%u",&uStatus);
	if(uStatus!=1)
	{
		gcMessage="Heat status is not active!";
		htmlEvent();
	}

	sprintf(gcQuery,"SELECT uScore FROM tScore"
			" WHERE uHeat=%u AND uIndex=%u AND uRider=%u"
			" AND uOwner=%u AND (uModBy=%u OR uCreatedBy=%u)",
				uHeat,uIndex,uRider,
				guOrg,guLoginClient,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="error0";
		htmlJudge();
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tScore"
				" SET fScore=%2.2f,uModDate=UNIX_TIMESTAMP(NOW()),uModBy=%u"
				" WHERE uScore=%s",
						fScore,guLoginClient,
						field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			gcMessage="error1";
			htmlJudge();
		}
		else
		{
			if(mysql_affected_rows(&gMysql)>0)
			gcMessage="Score Updated";
		}
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tScore"
				" SET cLabel='%.25s %u',fScore=%2.2f,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=%u"
				" ,uHeat=%u,uIndex=%u,uRider=%u,uEvent=%u,uOwner=%u,uRound=%u",
					cForeignKey("tRider","cLast",uRider),uIndex+1,fScore,guLoginClient,
					uHeat,uIndex,uRider,uEvent,guOrg,uRound);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			gcMessage="error2";
			htmlJudge();
		}
		else
		{
			if(mysql_affected_rows(&gMysql)>0)
				gcMessage="Score Created";
		}
	}//If insert or update
	mysql_free_result(res);

}//void UpdateInsertScore()


void JudgeCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"NewScore"))
	{
		ProcessJudgeVars(entries,x);
		if(!suMainScore)
			gcMessage="Main score not specified!";
		if(!suScoreFamilyProvided)
			gcMessage="Score family not specified!";
		if(!guRider || !guEvent || !guRound || !guHeat)
			gcMessage="Critical data missing!";
		if(gcMessage[0])
			htmlJudge();
		float fScore;
		if(suMainScore<10)
			fScore=(float)suMainScore+sfFineScore;
		else
			fScore=(float)suMainScore;
		UpdateInsertScore(fScore,suScoreFamily,guEvent,guRound,guHeat,guRider);
		PopulateScoreComp(guHeat);
		AdvanceRidersToNextRound(guHeat,guEvent);
		htmlJudge();
	}
	else if(!strcmp(gcFunction,"Score"))
	{
		ProcessJudgeVars(entries,x);

		gcMessage="";
		if(!guEvent)
			gcMessage="guEvent not specified";
		if(!suHeat)
			gcMessage="uHeat not specified";
		if(!suRound)
			gcMessage="uRound not specified";
		if(!uTrickLockProvided)
			gcMessage="uTrickLock not specified";
		if(gcMessage[0])
			htmlJudge();

		register int i;
		unsigned uIndex=0;
		unsigned uRider=0;
		for(i=0;i<x;i++)
		{
			if(!strncmp(entries[i].name,"fScore",6))
			{
				if(sscanf(entries[i].name,"fScore%u-%u",&uIndex,&uRider)==2)
				{
				  if(uIndex>=uTrickLock)
				  {
					float fScore=0.0;
					sscanf(entries[i].val,"%f",&fScore);
					UpdateInsertScore(fScore,uIndex,guEvent,suRound,suHeat,uRider);

				  }//If trick lock restriction
				}//if sscanf ok
			}//for fScore fields only
		}//for each field
		PopulateScoreComp(suHeat);
		if(!gcMessage[0])
			AdvanceRidersToNextRound(suHeat,guEvent);
	}//gcFunction=Score
	htmlJudge();

}//void UserCommands(pentry entries[], int x)


//Must work with funcJudge e.g.
void GetdEnd(void)
{
        MYSQL_RES *res;
	MYSQL_ROW field;
	if(!suHeat && guHeat)
		suHeat=guHeat;
	if(!suHeat)
		sprintf(gcQuery,"SELECT DATE_FORMAT(tHeat.dEnd,'%%b %%d, %%Y %%H:%%i:%%s')"
			" FROM tHeat,tEvent WHERE tHeat.uStatus=1 AND tHeat.uEvent=tEvent.uEvent AND tHeat.uEvent=%u"
			" AND NOW()>=DATE_SUB(tHeat.dStart,INTERVAL tEvent.uHeatPreStart MINUTE)"
			" AND NOW()<=DATE_ADD(tHeat.dEnd,INTERVAL tEvent.uHeatPostEnd MINUTE)"
			" LIMIT 1",
				guEvent);
	else
		sprintf(gcQuery,"SELECT DATE_FORMAT(tHeat.dEnd,'%%b %%d, %%Y %%H:%%i:%%s')"
			" FROM tHeat,tEvent WHERE tHeat.uStatus=1 AND tHeat.uEvent=tEvent.uEvent AND tHeat.uHeat=%u",
				suHeat);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlHeader("GetdEnd","Default.Header");
		fprintf(stdout,"%.255s",mysql_error(&gMysql));
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(field[0][0])
			sprintf(dEnd,"%.31s",field[0]);
	}
}//GetdEnd


void htmlHeat(void)
{
	GetdEnd();
	htmlHeader("Heat","Default.Header");
	htmlJudgePage("Heat","Heat.Body");
	htmlFooter("Default.Footer");
}//void htmlHeat(void)


void htmlOverlay(void)
{
	htmlHeader("Overlay","Default.Header");
	htmlJudgePage("Overlay","Overlay.Body");
	htmlFooter("Default.Footer");
}//void htmlOverlay(void)


void htmlBestTrick(void)
{
	htmlHeader("BestTrick","Default.Header");
	htmlJudgePage("BestTrick","BestTrick.Body");
	htmlFooter("Default.Footer");
}//void htmlBestTrick(void)


void htmlTournament(void)
{
	htmlHeader("Tournament","Default.Header");
	htmlJudgePage("Tournament","ATournament.Body");
	htmlFooter("Default.Footer");
}//void htmlTournament(void)


void htmlWind(void)
{
	htmlHeader("Wind","Default.Header");
	htmlJudgePage("Wind","Wind.Body");
	htmlFooter("Default.Footer");
}//void htmlWind(void)


void htmlRider(void)
{
	htmlHeader("Rider","Default.Header");
	htmlJudgePage("Rider","Rider.Body");
	htmlFooter("Default.Footer");
}//void htmlRider(void)


void htmlHeatEnd(void)
{
	GetdEnd();
	htmlHeader("HeatEnd","Default.Header");
	htmlJudgePage("HeatEnd","HeatEnd.Body");
	htmlFooter("Default.Footer");
}//void htmlHeatEnd(void)


void htmlEvent(void)
{
	htmlHeader("Event","Default.Header");
	htmlJudgePage("Event","Event.Body");
	htmlFooter("Default.Footer");
}//void htmlEvent(void)


void htmlLive(void)
{
	htmlHeader("Live","Default.Header");
	htmlJudgePage("Live","Live.Body");
	htmlFooter("Default.Footer");
}//void htmlLive(void)


void htmlAdmin(void)
{
	htmlHeader("Admin","Default.Header");
	htmlJudgePage("Admin","Admin.Body");
	htmlFooter("Default.Footer");
}//void htmlAdmin(void)


void htmlJudge(void)
{
	GetdEnd();
	htmlHeader("Judge","Default.Header");
	htmlJudgePage("Judge","Judge.Body");
	htmlFooter("Default.Footer");

}//void htmlJudge(void)


void htmlJudgePage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
		MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uDEFAULT,uBOOTSTRAP);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="";
			
			template.cpName[2]="gcMessage";
			template.cpValue[2]=gcMessage;

			template.cpName[3]="gcBrand";
			template.cpValue[3]=gcBrand;

			template.cpName[4]="gcName";
			template.cpValue[4]=gcName;

			template.cpName[5]="gcOrgName";
			template.cpValue[5]=gcOrgName;

			//Jan 1, 2021 15:37:25
			template.cpName[6]="dEnd";
			template.cpValue[6]=dEnd;

			template.cpName[7]="gcCopyright";
			template.cpValue[7]=INTERFACE_COPYRIGHT;

			template.cpName[8]="cBgColor";
			template.cpValue[8]=gcBgColor;

			template.cpName[9]="";

			printf("\n<!-- Start htmlJudgePage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlJudgePage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}


}//htmlJudgePage()


void funcJudge(FILE *fp)
{
	funcNewJudge(fp);
	return;

	MYSQL_RES *res;
	MYSQL_ROW field;
	MYSQL_RES *res2;
	MYSQL_ROW field2;
	MYSQL_RES *res3;
	MYSQL_ROW field3;
	unsigned uEvent=0;
	unsigned uHeat=0;
	unsigned uTrickLock=0;
	unsigned uRound=0;
	unsigned uNumScores=0;
	unsigned uStatus=0;
	char cStatus[32]={""};
	char cHeat[32]={""};
	char cRound[32]={""};
	char dStart[32]={""};
	char dEnd[32]={""};
	char dTimeLeft[32]={""};
	register int i;
	float fTotalScore=0.00;
	float fScoreArray[8]={0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00};


	if(!uSelectEvent(fp,"Judge")) return;
	if(!uSelectRound(fp,"Judge")) return;
	if(!uSelectHeat(fp,"Judge")) return;
	if(!uSelectRider(fp,"Judge",0)) return;

	fprintf(fp,"<div class=\"sTable\">");
	fprintf(fp,"<div class=\"sTableRow\">");

	if(!suHeat)
		sprintf(gcQuery,"SELECT tHeat.uHeat,UPPER(tHeat.cLabel),tHeat.uRound,"
			" DATE_FORMAT(tHeat.dStart,'%%H:%%i:%%s'),DATE_FORMAT(tHeat.dEnd,'%%H:%%i:%%s'),"
			"tHeat.uTrickLock,tStatus.cLabel,tStatus.uStatus,"
			"TIME_FORMAT(TIMEDIFF(tHeat.dEnd,NOW()),'%%i:%%s'),UPPER(tEvent.cLabel)"
			" FROM tHeat,tStatus,tEvent"
			" WHERE tHeat.uStatus=1 AND tHeat.uEvent=%u AND tStatus.uStatus=tHeat.uStatus"
			" AND tHeat.uEvent=tEvent.uEvent"
			" AND NOW()>=DATE_SUB(tHeat.dStart,INTERVAL tEvent.uHeatPreStart MINUTE)"
			" AND NOW()<=DATE_ADD(tHeat.dEnd,INTERVAL tEvent.uHeatPostEnd MINUTE)"
			" LIMIT 1",guEvent);
	else
		sprintf(gcQuery,"SELECT tHeat.uHeat,UPPER(tHeat.cLabel),tHeat.uRound,"
			" DATE_FORMAT(tHeat.dStart,'%%H:%%i:%%s'),DATE_FORMAT(tHeat.dEnd,'%%H:%%i:%%s'),"
			"tHeat.uTrickLock,tStatus.cLabel,tStatus.uStatus,"
			"TIME_FORMAT(TIMEDIFF(tHeat.dEnd,NOW()),'%%i:%%s'),UPPER(tEvent.cLabel)"
			" FROM tHeat,tStatus,tEvent"
			" WHERE tHeat.uHeat=%u AND tStatus.uStatus=tHeat.uStatus"
			" AND tHeat.uEvent=tEvent.uEvent",suHeat);
	mysql_query(&gMysql,gcQuery);
	if(*mysql_error(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uHeat);
		sprintf(cHeat,"%.31s",field[1]);
		sscanf(field[2],"%u",&uRound);
		if(field[3][0])
		sprintf(dStart,"%.31s",field[3]);
		if(field[4][0])
		sprintf(dEnd,"%.31s",field[4]);
		sscanf(field[5],"%u",&uTrickLock);
		sprintf(cStatus,"%.31s",field[6]);
		sscanf(field[7],"%u",&uStatus);
		if(field[8][0])
		sprintf(dTimeLeft,"%.31s",field[8]);
		sprintf(scEvent,"%.31s",field[9]);
	}
	if(!uHeat || !uRound) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">Automated selection: No active or suspended heat for event %s"
				"<br> Solution: Select a heat via Event tab.</div>",scEvent);
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	sprintf(gcQuery,"SELECT uNumScores,UPPER(cLabel) FROM tRound WHERE uRound=%u",uRound);
	mysql_query(&gMysql,gcQuery);
	if(*mysql_error(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNumScores);
		sprintf(cRound,"%.31s",field[1]);
	}
	if(!uNumScores || uNumScores>8) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">Incorrectly configured round %s/%u</div>",cRound,uRound);
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}


	if(uStatus==1)
	{
		fprintf(fp,"<div class=\"sTableCellGreen\">ENDING</div>");
		fprintf(fp,"<div class=\"sTableCellBlackBold\"><p id=\"demo\">%s</p></div>",dTimeLeft);
	}
	else
	{
		fprintf(fp,"<div class=\"sTableCellGreen\">%s</div>",cStatus);
		fprintf(fp,"<div class=\"sTableCellBlackBold\">%s</div>",dTimeLeft);
	}
	fprintf(fp,"<div class=\"sTableCellBlack\">%s (<a href=?gcPage=Judge&ClearEvent>X</a>)</div>",scEvent);
	fprintf(fp,"<div class=\"sTableCellBlack\">%s</div>",cRound);
	fprintf(fp,"<div class=\"sTableCellBlack\"><a title='Overlay' href=?gcFunction=Heat&uHeat=%u >HEAT %s</a></div>",
					uHeat,cHeat);
	fprintf(fp,"</div>");

	fprintf(fp,"</div>");


      	fprintf(fp,"<form class=\"form-signin\" role=\"form\" method=\"post\" action=\"/unxsEVApp/\">");
	fprintf(fp,"<input type='hidden' name='uEvent' value='%u'>",uEvent);
	fprintf(fp,"<input type='hidden' name='uHeat' value='%u'>",uHeat);
	fprintf(fp,"<input type='hidden' name='uTrickLock' value='%u'>",uTrickLock);
	fprintf(fp,"<input type='hidden' name='uRound' value='%u'>",uRound);
	fprintf(fp,"<br><div class=\"sTable\">");
	sprintf(gcQuery,"SELECT uRider FROM tScore WHERE uHeat=%u AND uOwner=%u GROUP BY uRider",
		uHeat,guOrg);
	mysql_query(&gMysql,gcQuery);
	if(*mysql_error(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res3=mysql_store_result(&gMysql);
	while((field3=mysql_fetch_row(res3)))
	{
		

	sprintf(gcQuery,"SELECT DISTINCT UPPER(SUBSTR(tRider.cFirst,1,1)),UPPER(tRider.cLast),tRider.uRider"
			" FROM tScore,tRider"
			" WHERE tScore.uRider=tRider.uRider"
			" AND tScore.uHeat=%u AND tScore.uOwner=%u AND tRider.uRider=%s",uHeat,guOrg,field3[0]);
	mysql_query(&gMysql,gcQuery);
	if(*mysql_error(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	unsigned uIndex=0;
	float fScore=0.00;
	if(mysql_num_rows(res)<1)	
	{
		fprintf(fp,"</div></form>No score config data avaiable!");
		return;
	}
	while((field=mysql_fetch_row(res)))
	{
		//Get scores for single judge
		for(i=0;i<uNumScores&&i<8;i++)
			fScoreArray[i]=0.00;
		sprintf(gcQuery,"SELECT fScore,uIndex FROM tScore WHERE uHeat=%u AND uRider=%s AND (uCreatedBy=%u OR uModBy=%u) LIMIT 8",
				uHeat,field[2],guLoginClient,guLoginClient);
		mysql_query(&gMysql,gcQuery);
		if(*mysql_error(&gMysql))
		{
			fprintf(fp,"%s",mysql_error(&gMysql));
			return;
		}
		res2=mysql_store_result(&gMysql);
		while((field2=mysql_fetch_row(res2)))
		{
			
			sscanf(field2[0],"%f",&fScore);
			sscanf(field2[1],"%u",&uIndex);
			fScoreArray[uIndex]=fScore;
		}
		mysql_free_result(res2);
		fTotalScore=0.00;
		for(i=0;i<uNumScores&&i<8;i++)
			fTotalScore+=fScoreArray[i];
		fTotalScore=fTotalScore;

		fprintf(fp,"<div class=\"sTableRow\">");
		//fprintf(fp,"<div class=\"sTableCell\">%d</div>",j++);
		fprintf(fp,"<div class=\"sTableCellBlack\">%s. %s</div>",field[0],field[1]);
		fprintf(fp,"<div class=\"sTableCellBlackBold\">%1.2f</div>",fTotalScore);
		fprintf(fp,"<div class=\"sTableCellBlackSmall\">");
		char *cTrickLock="required";
		for(i=0;i<uNumScores&&i<8;i++)
		{
			if(i<uTrickLock)
				cTrickLock="disabled";
			else
				cTrickLock="required";
			fprintf(fp,"&#%u; <input maxlength='5' type='number' step='any' id='inputScore%d-%s'"
				" class='my-form-control' max='10.00' value='%1.2f' name='fScore%d-%s' %s><br>",
					'A'+i,i,field[2],
					fScoreArray[i],
					i,field[2],cTrickLock);
		}
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");

	}//res while

	}//res3 while

	fprintf(fp,"</div>");
        fprintf(fp,"<br><button class='btn btn-lg btn-primary btn-block' type='submit' name=gcFunction value='Score'>Save</button>");
	fprintf(fp,"</form>");

	//if(guPermLevel<10)
	//	return;
	

	sprintf(gcQuery,"SELECT DISTINCT tClient.cLabel,tClient.uClient"
			" FROM tScore,tClient"
			" WHERE tScore.uModBy=tClient.uClient"
			" AND tScore.uHeat=%u AND tScore.uOwner=%u",uHeat,guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	fprintf(fp,"\n<br><div class=\"sTable\">\n");
	unsigned uClient=0;
	unsigned uPrevRider=0;
	unsigned uRider=0;
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uClient);
		fprintf(fp,"<div class=\"sTableRow\">\n");
		fprintf(fp,"\t<div class=\"sTableCell\">%s</div>\n",field[0]);
		sprintf(gcQuery,"SELECT tScore.uIndex,tScore.fScore,tRider.cLast,tRider.uRider"
			" FROM tScore,tRider"
			" WHERE tScore.uHeat=%u AND tScore.uOwner=%u"
			" AND tRider.uRider=tScore.uRider"
			" AND fScore>0.0"
			" AND tScore.uModBy=%u ORDER BY tScore.uRider,tScore.uIndex",uHeat,guOrg,uClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(fp,"%s",mysql_error(&gMysql));
			return;
		}
		res2=mysql_store_result(&gMysql);
		unsigned uCount=0;
		unsigned uIndex=0;
		unsigned uNumRows=mysql_num_rows(res2);
		while((field2=mysql_fetch_row(res2)))
		{
			sscanf(field2[3],"%u",&uRider);
			sscanf(field2[0],"%u",&uIndex);
			if(uPrevRider!=uRider)
				fprintf(fp,"\t\t<div class=\"sTableCell\">%s</div>\n",field2[2]);
			else
				fprintf(fp,"\t\t<div class=\"sTableCell\"></div>\n");
			fprintf(fp,"\t\t<div class=\"sTableCell\">&#%u;</div>\n",uIndex+'A');
			fprintf(fp,"\t\t<div class=\"sTableCell\">%s</div>\n",field2[1]);

			if(uCount<uNumRows-1)
			{
				fprintf(fp,"</div><!--sTableRow-->\n");
				fprintf(fp,"<div class=\"sTableRow\">\n");
				fprintf(fp,"\t<div class=\"sTableCell\"></div>\n");
			}
			if(uPrevRider!=uRider)
			{
				uPrevRider=uRider;
			}
			uCount++;
		}
		fprintf(fp,"</div><!--sTableRow-->\n");
	}
	fprintf(fp,"</div><!--sTable-->\n");



}//void funcJudge(FILE *fp)




char *cRiders(unsigned uHeat)
{
	static char cBuffer[512]={""};
	static char cLast[32]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT DISTINCT CONCAT(UPPER(SUBSTR(tRider.cFirst,1,1)),'. ',UPPER(tRider.cLast))"
			" FROM tScore,tRider WHERE tScore.uHeat=%u AND tRider.uRider=tScore.uRider",uHeat);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		return(cBuffer);
	}
	res=mysql_store_result(&gMysql);
	unsigned uSize=0;
	cBuffer[0]=0;
	while((field=mysql_fetch_row(res)))
	{
		sprintf(cLast,"%.27s<br>",field[0]);
		strncat(cBuffer,cLast,35);
		uSize+=strlen(cLast);
		if(uSize>(512-36)) break;
	}

	return(cBuffer);
}//char *cRiders(unsigned uHeat)


void funcLive(FILE *fp)
{
	fprintf(fp,"<!-- funcLive() -->\n");

	if(!uSelectEvent(fp,"Live")) return;


	if(guRider && guPermLevel>=10)
	{
		sprintf(gcQuery,"UPDATE tConfiguration"
			" SET cLabel='uLiveRider',cValue='%u',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uConfiguration=10"
				,guRider,guLoginClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(fp,"%s",mysql_error(&gMysql));
			return;
		}
	}
	if(guRider)
		fprintf(fp,"<b>%s</b> OVERLAY ACTIVE<br>",cForeignKey("tRider","cLabel",guRider));

	uSelectRider(fp,"Live",2);

}//void funcLive(FILE *fp)


void funcEvent(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uHeat=0;
	unsigned uRound=0;
	unsigned uNumScores=0;
	unsigned uStatus=0;
	unsigned uOnlyOnce=0;
	char cStatus[32]={""};
	char cHeat[32]={""};
	char cRound[32]={""};
	char dTimeLeft[32]={"---"};

	if(!uSelectEvent(fp,"Event")) return;

	fprintf(fp,"<!-- funcEvent() -->\n");

	sprintf(gcQuery,"SELECT tHeat.uHeat,UPPER(tHeat.cLabel),tHeat.uRound,"
			" TIME_FORMAT(TIMEDIFF(tHeat.dEnd,NOW()),'%%H:%%i:%%S'),"
			"UPPER(tStatus.cLabel),tStatus.uStatus,UPPER(tEvent.cLabel),UPPER(tRound.cLabel),tRound.uNumScores,tHeat.uTrickLock"
			" FROM tEvent,tRound,tHeat,tStatus"
			" WHERE tHeat.uEvent=%u AND tStatus.uStatus=tHeat.uStatus"
			" AND tEvent.uEvent=tHeat.uEvent AND tRound.uRound=tHeat.uRound"
			" ORDER BY tEvent.uEvent,tRound.uRound,tHeat.uHeat",guEvent);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{

		//fprintf(fp,"<div class=\"sTableCellBlue\">TRICK LOCK</div>");
		while((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uHeat);
			sprintf(cHeat,"%.31s",field[1]);
			sscanf(field[2],"%u",&uRound);
			if(field[3][0])
				sprintf(dTimeLeft,"%.31s",field[3]);
			sprintf(cStatus,"%.31s",field[4]);
			sscanf(field[5],"%u",&uStatus);
			sprintf(scEvent,"%.31s",field[6]);
			sprintf(cRound,"%.31s",field[7]);
			sscanf(field[8],"%u",&uNumScores); sscanf(field[9],"%u",&uTrickLock);
			if(!uOnlyOnce++)
			{
				fprintf(fp,"<div class=\"sTable\">\n");
				fprintf(fp,"<div class=\"sTableRow\">\n");
				if(guPermLevel<10)
					fprintf(fp,"<div class=\"sTableCellBlue\">%s"
					" (<a title='Change Event' href=?gcPage=Event&ClearEvent>X</a>)"
					"</div>",scEvent);
				else
					fprintf(fp,"<div class=\"sTableCellBlue\">%s"
					" (<a title='Change Event' href=?gcPage=Event&ClearEvent>X</a>)"
					" (<a title='Add 1 hour to all heats' href=?gcPage=Event&AddHour>+H</a>)"
					" (<a title='Subtract 1 hour from all heats' href=?gcPage=Event&SubHour>-H</a>)"
					" (<a title='Add 1 day to all heats ' href=?gcPage=Event&AddDay>+D</a>)"
					" (<a title='SUbtract 1 day from all heats' href=?gcPage=Event&SubDay>-D</a>)"
					"</div>",scEvent);
				fprintf(fp,"<div class=\"sTableCellBlue\">HEAT</div>\n");
				fprintf(fp,"<div class=\"sTableCellBlue\">STATUS</div>\n");
				fprintf(fp,"<div class=\"sTableCellBlue\">TIME LEFT</div>\n");
				fprintf(fp,"<div class=\"sTableCellBlue\">RIDERS</div>\n");
				fprintf(fp,"</div>\n");//row
			}
			fprintf(fp,"\t<div class=\"sTableRow\">\n");
			fprintf(fp,"\t<div class=\"sTableCellBlack\">%s</div>\n",cRound);
			fprintf(fp,"\t<div class=\"sTableCellBlack\"><a title='Judge' href=?uHeat=%u>%s</a>"
				"<br>&nbsp;&nbsp;&nbsp;&nbsp;<a title='Overlay' href=?gcFunction=Heat&uHeat=%u>%s</a></div>\n",
					uHeat,cHeat,uHeat,cHeat);
			fprintf(fp,"\t<div class=\"sTableCellBlack\">%s</div>\n",cStatus);
			fprintf(fp,"\t<div class=\"sTableCellBlack\">%s</div>\n",dTimeLeft);
			fprintf(fp,"\t<div class=\"sTableCellBlack\">%s</div>\n",cRiders(uHeat));
			fprintf(fp,"\t</div>");//row
		}
		fprintf(fp,"\n</div>\n");//table
	}
	else
	{
		fprintf(fp,"<div class=\"sTableCellBlack\">No heats found for event %s"
 				" (<a title='Change Event' href=?gcPage=Event&ClearEvent>X</a>)</div>",cForeignKey("tEvent","cLabel",guEvent));
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

}//void funcEvent(FILE *fp)


void funcBestTrick(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	MYSQL_RES *res2;
	MYSQL_ROW field2;
	unsigned uEvent=0;
	char cEvent[32]={""};
	register int j;

	fprintf(fp,"<!-- funcBestTrick() 0.1-->\n");
	fprintf(fp,"<div class=\"sTable\">");
	fprintf(fp,"<div class=\"sTableRow\">");

	if(guEvent)
		uEvent=guEvent;
	else if(suEvent)
		uEvent=suEvent;//from GetHook

	if(!uEvent)
		sprintf(gcQuery,"SELECT tEvent.uEvent,tEvent.cLabel"
			" FROM tEvent WHERE tEvent.uStatus=1"
			" AND tEvent.dStart<=NOW()"
			" ORDER BY tEvent.dStart LIMIT 1");
	else
		sprintf(gcQuery,"SELECT tEvent.uEvent,tEvent.cLabel"
			" FROM tEvent WHERE uEvent=%u",uEvent);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uEvent);
		sprintf(cEvent,"%.31s",field[1]);
	}
	if(!uEvent) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">No active event</div>");
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	fprintf(fp,"<div class=\"sTableCellBlueLarge\">Top 10 Best Tricks</div>");
	fprintf(fp,"<div class=\"sTableCellBlackLarge\">%s</div>",cEvent);
	fprintf(fp,"</div>");//row

	fprintf(fp,"</div>");//table


	j=1;
	fprintf(fp,"<br><div class=\"sTable\">");
	sprintf(gcQuery,"SELECT DISTINCT uRider,fScore FROM tScoreComp WHERE uEvent=%u ORDER BY fScore DESC LIMIT 10",uEvent);
	mysql_query(&gMysql,gcQuery);
	if(*mysql_error(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)==0)
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">No rider data</div>");
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	while((field=mysql_fetch_row(res)))
	{
		float fScore=0.00;
		char cRound[32]={""};
		char cHeat[32]={""};
		unsigned uIndex=0;

		sscanf(field[1],"%f",&fScore);
		//We should change to DECIMAL to get exact scores! TODO
		sprintf(gcQuery,"SELECT tRound.cLabel,tHeat.cLabel,tScoreComp.uIndex FROM tScoreComp,tRound,tHeat"
				" WHERE tScoreComp.uRider=%s"
				" AND ABS(tScoreComp.fScore-%f) <= 1e-6"
				" AND tScoreComp.uRound=tRound.uRound AND tScoreComp.uHeat=tHeat.uHeat"
				" AND tScoreComp.uEvent=%u"
						,field[0]
						,fScore
						,uEvent);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(fp,"%s",mysql_error(&gMysql));
			return;
		}
		res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			sprintf(cRound,"%.31s",field2[0]);
			sprintf(cHeat,"%.31s",field2[1]);
			sscanf(field2[2],"%u",&uIndex);
		}
		else
		{
			sprintf(cRound,"%s",field[1]);
		}

		sprintf(gcQuery,"SELECT UPPER(SUBSTR(tRider.cFirst,1,1)),UPPER(tRider.cLast),tRider.cCountry"
			" FROM tRider"
			" WHERE tRider.uRider=%s",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(fp,"%s",mysql_error(&gMysql));
			return;
		}
		res2=mysql_store_result(&gMysql);
		if(mysql_num_rows(res2)<1)	
		{
			fprintf(fp,"</div>No score config data avaiable!");
			return;
		}
		if((field2=mysql_fetch_row(res2)))
		{
			fprintf(fp,"<div class=\"sTableRow\">");
			if(j<=2)
				fprintf(fp,"<div class=\"sTableCellBlueLarge\">%d</div>",j++);
			else
				fprintf(fp,"<div class=\"sTableCellLarge\">%d</div>",j++);
			fprintf(fp,"<div class=\"sTableCellBlackLarge\">%s. %s</div>",field2[0],field2[1]);
			if(field2[2][0])
				fprintf(fp,"<div class=\"sTableCellBlackLarge\">"
					"<img alt=%s src=\"/bs/images/%s.png\"></div>",field2[2],field2[2]);
			else
				fprintf(fp,"<div class=\"sTableCellBlackLarge\">--</div>");
			fprintf(fp,"<div class=\"sTableCellBlackLarge\">%s</div>",cRound);
			fprintf(fp,"<div class=\"sTableCellBlackLarge\">Heat %s</div>",cHeat);
			fprintf(fp,"<div class=\"sTableCellBlackLarge\">Trick %u</div>",uIndex+1);
			fprintf(fp,"<div class=\"sTableCellBlackBoldLarge\">%2.2f</div>",fScore);

			fprintf(fp,"</div>");

		}//res2 if

	}//res while

	fprintf(fp,"</div>");

}//void funcBestTrick(FILE *fp)



void funcHeat(FILE *fp)
{
	//funcHeatEnd(fp);
	//return;

	MYSQL_RES *res;
	MYSQL_ROW field;
	MYSQL_RES *res2;
	MYSQL_ROW field2;
	MYSQL_RES *res3;
	MYSQL_ROW field3;
	unsigned uHeat=0;
	unsigned uRound=0;
	unsigned uNumScores=0;
	unsigned uNumJudges=0;
	unsigned uStatus=0;
	char *cScoreTable="tScoreComp";
	char cStatus[32]={""};
	char cHeat[32]={""};
	char cEvent[32]={""};
	char cRound[32]={""};
	char dTimeLeft[32]={"---"};
	register int i,j;
	float fTotalScore=0.00;
	float fLeaderTotalScore=0.00;
	float fScoreArray[8]={0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00};

	fprintf(fp,"<!-- funcHeat() -->\n");
	fprintf(fp,"<div class=\"sTable\">");
	fprintf(fp,"<div class=\"sTableRow\">");

	if(suHeat)
		sprintf(gcQuery,"SELECT tHeat.uHeat,tHeat.cLabel,tHeat.uRound,"
			" TIME_FORMAT(TIMEDIFF(tHeat.dEnd,NOW()),'%%i:%%s'),"
			"UPPER(tStatus.cLabel),tStatus.uStatus,tEvent.cLabel"
			" FROM tHeat,tStatus,tEvent WHERE"
			" tHeat.uHeat=%u AND tStatus.uStatus=tHeat.uStatus"
			" AND tHeat.uEvent=tEvent.uEvent"
			" ORDER BY tHeat.uHeat DESC LIMIT 1",suHeat);
	else if(guEvent)
		sprintf(gcQuery,"SELECT tHeat.uHeat,tHeat.cLabel,tHeat.uRound,"
			" TIME_FORMAT(TIMEDIFF(tHeat.dEnd,NOW()),'%%i:%%s'),"
			"UPPER(tStatus.cLabel),tStatus.uStatus"
			" FROM tHeat,tStatus WHERE tHeat.uStatus<3 AND tHeat.uEvent=tEvent.uEvent"
			" AND tHeat.uEvent=%u AND tHeat.dEnd>NOW() AND tStatus.uStatus=tHeat.uStatus"
			" ORDER BY tHeat.uHeat DESC LIMIT 1",guEvent);
	else if(1)
		sprintf(gcQuery,"SELECT tHeat.uHeat,tHeat.cLabel,tHeat.uRound,"
			" TIME_FORMAT(TIMEDIFF(tHeat.dEnd,NOW()),'%%i:%%s'),"
			"UPPER(tStatus.cLabel),tStatus.uStatus,tEvent.cLabel"
			" FROM tHeat,tStatus,tEvent WHERE tHeat.uStatus=1 AND tEvent.uStatus=1"
			" AND tHeat.dEnd>NOW() AND tStatus.uStatus=tHeat.uStatus"
			" AND tHeat.uEvent=tEvent.uEvent"
			" ORDER BY tHeat.dStart LIMIT 1");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uHeat);
		sprintf(cHeat,"%.31s",field[1]);
		sscanf(field[2],"%u",&uRound);
		if(field[3][0])
			sprintf(dTimeLeft,"%.31s",field[3]);
		sprintf(cStatus,"%.31s",field[4]);
		sscanf(field[5],"%u",&uStatus);
		sprintf(cEvent,"%.31s",field[6]);
	}
	if(!uHeat || !uRound) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">No active or suspended heat for event %s</div>",cEvent);
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	sprintf(gcQuery,"SELECT uNumScores,cLabel FROM tRound WHERE uRound=%u",uRound);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNumScores);
		sprintf(cRound,"%.31s",field[1]);
	}
	if(!uNumScores || uNumScores>8) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">Incorrectly configured round %s/%u</div>",cRound,uRound);
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	//TODO
	sprintf(gcQuery,"SELECT DISTINCT uCreatedBy FROM tScore WHERE uHeat=%u",uHeat);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	uNumJudges=mysql_num_rows(res);


	if(uStatus==1)
		fprintf(fp,"<div class=\"sTableCellGreenLarge\">ENDING</div>");
	else
		fprintf(fp,"<div class=\"sTableCellGreenLarge\">%s</div>",cStatus);
	fprintf(fp,"</div>");//row

	fprintf(fp,"<div class=\"sTableRow\">");
	if(uStatus==1)
		fprintf(fp,"<div class=\"sTableCellBlackBoldLarge\"><p id=\"demo\">%s</p></div>",dTimeLeft);
	else
		fprintf(fp,"<div class=\"sTableCellBlackBoldLarge\">%s</div>",dTimeLeft);
	fprintf(fp,"<div class=\"sTableCellEmpty\"></div>");
	fprintf(fp,"<div class=\"sTableCellBlackLarge\">%s</div>",cEvent);
	fprintf(fp,"<div class=\"sTableCellBlackLarge\">%s</div>",cRound);
	fprintf(fp,"<div class=\"sTableCellBlackLarge\">Heat %s</div>",cHeat);
	fprintf(fp,"</div>");//row

	fprintf(fp,"</div>");


	j=1;
	fprintf(fp,"<br><div class=\"sTable\">");
	sprintf(gcQuery,"SELECT uRider FROM tScoreComp WHERE uHeat=%u GROUP BY uRider ORDER BY SUM(fScore) DESC",uHeat);
	mysql_query(&gMysql,gcQuery);
	if(*mysql_error(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res3=mysql_store_result(&gMysql);
	if(mysql_num_rows(res3)==0)
	{
		cScoreTable="tScore";
		sprintf(gcQuery,
			"SELECT uRider FROM tScore WHERE uHeat=%u GROUP BY uRider",uHeat);
		mysql_query(&gMysql,gcQuery);
		if(*mysql_error(&gMysql))
		{
			fprintf(fp,"%s",mysql_error(&gMysql));
			return;
		}
		res3=mysql_store_result(&gMysql);
	}
	if(mysql_num_rows(res3)==0)
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">No rider data</div>");
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	while((field3=mysql_fetch_row(res3)))
	{
		sprintf(gcQuery,"SELECT DISTINCT UPPER(SUBSTR(tRider.cFirst,1,1)),UPPER(tRider.cLast),tRider.uRider"
			",tRider.cCountry"
			" FROM %s,tRider"
			" WHERE %s.uRider=tRider.uRider"
			" AND %s.uHeat=%u AND tRider.uRider=%s",cScoreTable,cScoreTable,cScoreTable,uHeat,field3[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(fp,"%s",mysql_error(&gMysql));
			return;
		}
		res=mysql_store_result(&gMysql);
		unsigned uIndex=0;
		float fScore=0.00;
		if(mysql_num_rows(res)<1)	
		{
			fprintf(fp,"</div>No score config data avaiable!");
			return;
		}
		while((field=mysql_fetch_row(res)))
		{
			//Get scores for single judge REFACTOR!
			for(i=0;i<uNumScores&&i<8;i++)
				fScoreArray[i]=0.00;
			//Here we need to average out all scores from all judges for each index.
			//We need to know how many judges have provided scores.
			sprintf(gcQuery,"SELECT fScore,uIndex FROM %s WHERE uHeat=%u AND uRider=%s LIMIT 8",
					cScoreTable,uHeat,field[2]);
			mysql_query(&gMysql,gcQuery);
			if(*mysql_error(&gMysql))
			{
				fprintf(fp,"%s",mysql_error(&gMysql));
				return;
			}
			res2=mysql_store_result(&gMysql);
			while((field2=mysql_fetch_row(res2)))
			{
				
				sscanf(field2[0],"%f",&fScore);
				sscanf(field2[1],"%u",&uIndex);
				fScoreArray[uIndex]=fScore;
			}
			mysql_free_result(res2);
			fTotalScore=0.00;
			for(i=0;i<uNumScores&&i<8;i++)
				fTotalScore+=fScoreArray[i];
			if(j==1)
				fLeaderTotalScore=fTotalScore;
	
			fprintf(fp,"\n<div class=\"sTableRow\">");
			if(j<=2)
				fprintf(fp,"<div class=\"sTableCellBlueLarge\">%d</div>",j++);
			else
				fprintf(fp,"<div class=\"sTableCellLarge\">%d</div>",j++);
			fprintf(fp,"<div class=\"sTableCellBlackLarge\">%s. %s</div>",field[0],field[1]);
			if(field[3][0])
				fprintf(fp,"<div class=\"sTableCellBlackLarge\"><img alt=%s src=\"/bs/images/%s.png\"></div>",field[3],field[3]);
			else
				fprintf(fp,"<div class=\"sTableCellBlackLarge\">--</div>");
			fprintf(fp,"<div class=\"sTableCellBlackBoldLarge\">%1.2f</div>",fTotalScore);
			for(i=0;i<uNumScores&&i<8;i++)
			{
				fprintf(fp,"<div class=\"sTableCellBlackLarge\">%1.2f</div>",fScoreArray[i]);
			}
	
			if(j==2)
			{
				fprintf(fp,"<div class=\"sTableRow\">");
				fprintf(fp,"<div class=\"sTableCellEmpty\"></div>");
				fprintf(fp,"<div class=\"sTableCellBlueLarge\">Needs</div>");
				fprintf(fp,"</div>");
			}
			else
			{
				fprintf(fp,"<div class=\"sTableRow\">");
				fprintf(fp,"<div class=\"sTableCellEmpty\"></div>");
				fprintf(fp,"<div class=\"sTableCellBlueLarge\">%1.2f</div>",fScoreArray[--i]+fLeaderTotalScore-fTotalScore);
				fprintf(fp,"</div>");
			}

			fprintf(fp,"</div>");

		}//res while

	}//res3 while

	fprintf(fp,"</div>");

}//void funcHeat(FILE *fp)


void funcHeatEnd(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	MYSQL_RES *res2;
	MYSQL_ROW field2;
	MYSQL_RES *res3;
	MYSQL_ROW field3;
	unsigned uEvent=0;
	unsigned uRound=0;
	unsigned uNumScores=0;
	unsigned uNumJudges=0;
	char cStatus[32]={""};
	unsigned uStatus=0;
	char *cScoreTable="tScoreComp";
	char cHeat[32]={""};
	char cEvent[32]={""};
	char cRound[32]={""};
	char dTimeToEnd[32]={""};
	char dTimeToStart[32]={""};
	char dStart[32]={""};
	char dEnd[32]={""};
	register int i,j;
	float fTotalScore=0.00;
	float fScoreArray[8]={0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00};

	fprintf(fp,"<!-- funcHeatEnd -->\n");
	fprintf(fp,"<div class=\"sTable\">");
	fprintf(fp,"<div class=\"sTableRow\">");

	sprintf(gcQuery,"SELECT tHeat.cLabel,tHeat.uRound,UPPER(tStatus.cLabel),"
				"tEvent.cLabel,tEvent.uEvent,"
				"tHeat.dStart,tHeat.dEnd,tStatus.uStatus,"
				"TIME_FORMAT(TIMEDIFF(tHeat.dEnd,NOW()),'%%H:%%i:%%s'),TIME_FORMAT(TIMEDIFF(NOW(),tHeat.dStart),'%%H:%%i:%%s')"
				" FROM tHeat,tEvent,tStatus"
				" WHERE tHeat.uHeat=%u AND tHeat.uStatus=tStatus.uStatus AND tHeat.uEvent=tEvent.uEvent"
				" ORDER BY tHeat.uHeat DESC LIMIT 1",suHeat);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cHeat,"%.31s",field[0]);
		sscanf(field[1],"%u",&uRound);
		sprintf(cStatus,"%.31s",field[2]);

		sprintf(cEvent,"%.31s",field[3]);
		sscanf(field[4],"%u",&uEvent);

		sprintf(dStart,"%.31s",field[5]);
		sprintf(dEnd,"%.31s",field[6]);
		sscanf(field[7],"%u",&uStatus);

		sprintf(dTimeToEnd,"%.31s",field[8]);
		sprintf(dTimeToStart,"%.31s",field[9]);
	}
	if(!uRound) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">This heat does not exist. Event %s</div>",cEvent);
		//fprintf(fp,"<div class=\"sTableCellBlack\">%s</div>",gcQuery);
		//fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	sprintf(gcQuery,"SELECT uNumScores,cLabel FROM tRound WHERE uRound=%u",uRound);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNumScores);
		sprintf(cRound,"%.31s",field[1]);
	}
	if(!uNumScores || uNumScores>8) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">Incorrectly configured round %s/%u</div>",cRound,uRound);
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	sprintf(gcQuery,"SELECT DISTINCT uModBy FROM tScore WHERE uHeat=%u",suHeat);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	uNumJudges=mysql_num_rows(res);


	fprintf(fp,"<div class=\"sTableCellGreenLarge\">%s</div>",cStatus);
	fprintf(fp,"<div class=\"sTableCellEmpty\"></div>");
	fprintf(fp,"<div class=\"sTableCellBlack\">%s to %s</div>",dStart,dEnd);
	fprintf(fp,"</div>");//row

	fprintf(fp,"<div class=\"sTableRow\">");
	if(uStatus==1)
		fprintf(fp,"<div class=\"sTableCellBlackBoldLarge\"><p id=\"demo\">%s</p></div>",dTimeToEnd);
	else
		fprintf(fp,"<div class=\"sTableCellBlackBoldLarge\"><p id=\"demo\">%s</p></div>",dTimeToStart);
	fprintf(fp,"<div class=\"sTableCellEmpty\"></div>");
	fprintf(fp,"<div class=\"sTableCellBlackLarge\">%s</div>",cEvent);
	fprintf(fp,"<div class=\"sTableCellBlackLarge\">%s</div>",cRound);
	fprintf(fp,"<div class=\"sTableCellBlackLarge\">Heat %s</div>",cHeat);
	fprintf(fp,"</div>");//row

	fprintf(fp,"</div>");


	j=1;
	fprintf(fp,"<br><div class=\"sTable\">");
	sprintf(gcQuery,
		"SELECT uRider FROM tScoreComp WHERE uHeat=%u GROUP BY uRider ORDER BY SUM(fScore) DESC",
		suHeat);
	mysql_query(&gMysql,gcQuery);
	if(*mysql_error(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res3=mysql_store_result(&gMysql);
	if(mysql_num_rows(res3)==0)
	{
		cScoreTable="tScore";
		sprintf(gcQuery,
			"SELECT uRider FROM tScore WHERE uHeat=%u GROUP BY uRider",
			suHeat);
		mysql_query(&gMysql,gcQuery);
		if(*mysql_error(&gMysql))
		{
			fprintf(fp,"%s",mysql_error(&gMysql));
			return;
		}
		res3=mysql_store_result(&gMysql);
	}
	if(mysql_num_rows(res3)==0)
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">No rider data</div>");
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}
	while((field3=mysql_fetch_row(res3)))
	{
		

	sprintf(gcQuery,"SELECT DISTINCT UPPER(tRider.cFirst),UPPER(tRider.cLast),tRider.uRider,tRider.cCountry"
			" FROM %s,tRider"
			" WHERE %s.uRider=tRider.uRider"
			" AND %s.uHeat=%u AND tRider.uRider=%s",
				cScoreTable,
				cScoreTable,
				cScoreTable,suHeat,field3[0]);
	mysql_query(&gMysql,gcQuery);
	if(*mysql_error(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	unsigned uIndex=0;
	float fScore=0.00;
	if(mysql_num_rows(res)<1)	
	{
		//fprintf(fp,"</div>No score config data avaiable!");
		return;
	}
	while((field=mysql_fetch_row(res)))
	{
		for(i=0;i<uNumScores&&i<8;i++)
			fScoreArray[i]=0.00;
		sprintf(gcQuery,"SELECT fScore,uIndex FROM tScoreComp WHERE uHeat=%u AND uRider=%s LIMIT 8",
				suHeat,field[2]);
		mysql_query(&gMysql,gcQuery);
		if(*mysql_error(&gMysql))
		{
			fprintf(fp,"%s",mysql_error(&gMysql));
			return;
		}
		res2=mysql_store_result(&gMysql);
		while((field2=mysql_fetch_row(res2)))
		{
			
			sscanf(field2[0],"%f",&fScore);
			sscanf(field2[1],"%u",&uIndex);
			fScoreArray[uIndex]=fScore;
		}
		mysql_free_result(res2);
		fTotalScore=0.00;
		for(i=0;i<uNumScores&&i<8;i++)
			fTotalScore+=fScoreArray[i];

		fprintf(fp,"<div class=\"sTableRow\">");
		fprintf(fp,"<div class=\"sTableCellLarge\">%d</div>",j++);
		if(j==2)
			fprintf(fp,"<div class=\"sTableCellBlueLarge\">%s %s</div>",field[0],field[1]);
		else
			fprintf(fp,"<div class=\"sTableCellBlackLarge\">%s %s</div>",field[0],field[1]);
		if(field[3][0])
			fprintf(fp,"<div class=\"sTableCellBlackLarge\"><img alt=%s src=\"/bs/images/%s.png\"></div>",field[3],field[3]);
		else
			fprintf(fp,"<div class=\"sTableCellBlackLarge\">--</div>");
		fprintf(fp,"<div class=\"sTableCellBlackBoldLarge\">%1.2f</div>",fTotalScore);
		for(i=0;i<uNumScores&&i<8;i++)
		{
			fprintf(fp,"<div class=\"sTableCellBlackLarge\">%1.2f</div>",fScoreArray[i]);
		}

		fprintf(fp,"</div>");

	}//res while

	}//res3 while

	fprintf(fp,"</div>");


}//void funcHeatEnd(FILE *fp)


void funcOverlay(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uValue=0;

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uConfiguration=10");
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uValue);

	fprintf(fp,"<!-- funcOverlay() -->\n");
	switch(uValue)
	{
		default:
		case(0):
			funcHeat(fp);
			fprintf(fp,"<br>");
			funcWind(fp);
		break;

		case(1):
			funcHeatEnd(fp);
			fprintf(fp,"<br>");
			funcWind(fp);
		break;

		case(2):
			funcBestTrick(fp);
			fprintf(fp,"<br>");
			funcWind(fp);
		break;

		case(3):
			funcTournament(fp);
			fprintf(fp,"<br>");
			funcWind(fp);
		break;

		case(4):
			funcWind(fp);
		break;

		case(5):
			funcRider(fp);
		break;
	}//switch uValue

}//void funcOverlay(FILE *fp)


void funcTournament(FILE *fp)
{
	fprintf(fp,"<!-- funcTournament() -->\n");
}//void funcTournament(FILE *fp)


void funcRider(FILE *fp)
{
	fprintf(fp,"<!-- funcRider() -->\n");

	unsigned uRider=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uConfiguration=10");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uRider);
	
		sprintf(gcQuery,"SELECT DISTINCT UPPER(tRider.cFirst),UPPER(tRider.cLast),tRider.uRider"
			",tRider.cCountry"
			" FROM tRider"
			" WHERE tRider.uRider=%u",uRider);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(fp,"%s",mysql_error(&gMysql));
			return;
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"<div class=\"sTable\">");
			fprintf(fp,"\n<div class=\"sTableRow\">");
			fprintf(fp,"<div class=\"sTableCellBlackLarge\">%s %s</div>",field[0],field[1]);
			if(field[3][0])
				fprintf(fp,"<div class=\"sTableCellBlackLarge\"><img alt=%s src=\"/bs/images/%s.png\"></div>",field[3],field[3]);
			else
				fprintf(fp,"<div class=\"sTableCellBlackLarge\">--</div>");
			fprintf(fp,"</div>");
			fprintf(fp,"</div>");
		}

	}

}//void funcRider(FILE *fp)


void funcWind(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	char cAvgWind[5]={"---"};
	char cMinWind[5]={"---"};
	char cMaxWind[5]={"---"};
	char cDirWind[5]={"---"};

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uConfiguration=6");
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cAvgWind,"%.4s",field[0]);

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uConfiguration=7");
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cMinWind,"%.4s",field[0]);

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uConfiguration=8");
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cMaxWind,"%.4s",field[0]);

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uConfiguration=9");
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cDirWind,"%.4s",field[0]);


	fprintf(fp,"<!-- funcWind() -->\n");
	fprintf(fp,"<div class=\"sTable\">");
	fprintf(fp,"<div class=\"sTableRow\">");
	fprintf(fp,"<div class=\"sTableCellBlueLarge\">%.4s Knots</div>",cAvgWind);
	fprintf(fp,"<div class=\"sTableCellBlackLarge\">Min %.4s Knots</div>",cMinWind);
	fprintf(fp,"<div class=\"sTableCellBlackLarge\">Max %.4s Knots</div>",cMaxWind);
	fprintf(fp,"<div class=\"sTableCellBlueLarge\">%.3s</div>",cDirWind);
	fprintf(fp,"</div>");
	fprintf(fp,"</div>");
}//void funcWind((FILE *fp)


#include "automation.c"
