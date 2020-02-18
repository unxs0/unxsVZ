/*
FILE 
	{{cProject}}/interfaces/bootstrap/judge.c
	template/new/bootstrap/judge.c
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
static unsigned uEvent=0;
static unsigned uRound=0;
static unsigned suHeat=0;
static unsigned uTrickLock=0;
static unsigned uTrickLockProvided=0;
static char dEnd[32]={"Jan 1, 2021 15:37:25"};
static char cEvent[32]={""};

//TOC
void ProcessJudgeVars(pentry entries[], int x);
void JudgeGetHook(entry gentries[],int x);
void JudgeCommands(pentry entries[], int x);
void htmlHeat(void);
void htmlBestTrick(void);
void htmlHeatEnd(void);
void htmlEvent(void);
void htmlJudge(void);
void PopulateScoreComp(unsigned uHeat);
//extern
void unxsvzLog(unsigned uTablePK,char *cTableName,char *cLogEntry,unsigned guPermLevel,unsigned guLoginClient,char *gcLogin,char *gcHost);

unsigned uSelectHeat(FILE *fp,char *cPage)
{
	return(1);
}//unsigned uSelectHeat(FILE *fp,char *cPage)


unsigned uSelectEvent(FILE *fp,char *cPage)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

      if(!guEvent)
      {
	sprintf(gcQuery,"SELECT cLabel,uEvent FROM tEvent WHERE uOwner=%u",guOrg);
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
		sprintf(cEvent,"%.31s",field[0]);
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
			sscanf(entries[i].val,"%u",&uEvent);
		else if(!strcmp(entries[i].name,"uRound"))
			sscanf(entries[i].val,"%u",&uRound);
	}

}//void ProcessJobOfferVars(pentry entries[], int x)


void CommonGetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"guEvent"))
		{
			sscanf(gentries[i].val,"%u",&guEvent);
			printf("Set-Cookie: {{cProject}}Event=%u; secure; httponly; samesite=strict;\n",guEvent);
		}
		else if(!strcmp(gentries[i].name,"guHeat"))
		{
			sscanf(gentries[i].val,"%u",&guHeat);
			printf("Set-Cookie: {{cProject}}Heat=%u; secure; httponly; samesite=strict;\n",guHeat);
		}
		else if(!strcmp(gentries[i].name,"ClearEvent"))
		{
			guEvent=0;
			printf("Set-Cookie: {{cProject}}Event=\"deleted\";"
				" discard; secure; httponly; expires=\"Mon, 01-Jan-1971 00:10:10 GMT\"\n");
		}
	}
}//void CommonGetHook(entry gentries[],int x);


void HeatEndGetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uHeat"))
			sscanf(gentries[i].val,"%u",&suHeat);
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

	htmlEvent();

}//void EventGetHook(entry gentries[],int x)


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


void JudgeCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"Score"))
	{
		ProcessJudgeVars(entries,x);

		gcMessage="";
		if(!guEvent)
			gcMessage="guEvent not specified";
		if(!suHeat)
			gcMessage="uHeat not specified";
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
        				MYSQL_RES *res;
	        			MYSQL_ROW field;

					sprintf(gcQuery,"SELECT uScore FROM tScore"
					" WHERE uHeat=%u AND uIndex=%u AND uRider=%u"
					" AND uOwner=%u AND (uModBy=%u OR uCreatedBy=%u)",
						suHeat,uIndex,uRider,
						guOrg,guLoginClient,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(*mysql_error(&gMysql))
					{
						gcMessage="error0";
						htmlJudge();
					}
					res=mysql_store_result(&gMysql);
					if((field=mysql_fetch_row(res)))
					{
					
						sprintf(gcQuery,"UPDATE tScore"
							" SET fScore=%s,uModDate=UNIX_TIMESTAMP(NOW()),uModBy=%u"
							" WHERE uScore=%s",
								entries[i].val,guLoginClient,
								field[0]);
						mysql_query(&gMysql,gcQuery);
						if(*mysql_error(&gMysql))
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
							" SET cLabel='%.25s %u',fScore=%s,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=%u"
							" ,uHeat=%u,uIndex=%u,uRider=%u,uEvent=%u,uOwner=%u,uRound=%u",
								cForeignKey("tRider","cLast",uRider),uIndex+1,entries[i].val,guLoginClient,
								suHeat,uIndex,uRider,guEvent,guOrg,uRound);
						mysql_query(&gMysql,gcQuery);
						if(*mysql_error(&gMysql))
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
				     }//If trick lock restriction
				}//if sscanf ok
			}//for fScore fields only
		}//for each field
		PopulateScoreComp(suHeat);
	}//gcFunction=Score
	htmlJudge();

}//void UserCommands(pentry entries[], int x)


//Must work with funcJudge e.g.
void GetdEnd(void)
{
        MYSQL_RES *res;
	MYSQL_ROW field;
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
		htmlHeader("Error","Default.Header");
		fprintf(stdout,"%s",mysql_error(&gMysql));
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(field[0][0])
			sprintf(dEnd,"%.31s",field[0]);
	}
}


void htmlHeat(void)
{
	GetdEnd();
	htmlHeader("Heat","Default.Header");
	htmlJudgePage("Heat","Heat.Body");
	htmlFooter("Default.Footer");
}//void htmlHeat(void)


void htmlBestTrick(void)
{
	htmlHeader("BestTrick","Default.Header");
	htmlFooter("Default.Footer");
}//void htmlBestTrick(void)


void htmlHeatEnd(void)
{
	GetdEnd();
	htmlHeader("HeatEnd","Default.Header");
	htmlJudgePage("Heat","HeatEnd.Body");
	htmlFooter("Default.Footer");
}//void htmlHeatEnd(void)


void htmlEvent(void)
{
	htmlHeader("Event","Default.Header");
	htmlJudgePage("Event","Event.Body");
	htmlFooter("Default.Footer");
}//void htmlEvent(void)


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

			template.cpName[8]="";

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
	register int i,j;
	float fTotalScore=0.00;
	float fScoreArray[8]={0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00};


	if(!uSelectEvent(fp,"Judge")) return;
	if(!uSelectHeat(fp,"Judge")) return;

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
		sprintf(cEvent,"%.31s",field[9]);
	}
	if(!uHeat || !uRound) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">No active or suspended heat for event %s</div>",cEvent);
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	sprintf(gcQuery,"SELECT uNumScores,UPPER(cLabel) FROM tRound WHERE uRound=%u AND uOwner=%u",uRound,guOrg);
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
		fprintf(fp,"<div class=\"sTableCellGreen\">TO END</div>");
		fprintf(fp,"<div class=\"sTableCellBlackBold\"><p id=\"demo\">%s</p></div>",dTimeLeft);
	}
	else
	{
		fprintf(fp,"<div class=\"sTableCellGreen\">%s</div>",cStatus);
		fprintf(fp,"<div class=\"sTableCellBlackBold\">%s</div>",dTimeLeft);
	}
	fprintf(fp,"<div class=\"sTableCellBlack\">%s (<a href=?gcPage=Judge&ClearEvent>X</a>)</div>",cEvent);
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
	sprintf(gcQuery,"SELECT uRider FROM tScore WHERE uHeat=%u AND uOwner=%u GROUP BY uRider ORDER BY SUM(fScore) DESC",
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
	j=1;
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
			fprintf(fp,"<input maxlength='5' type='number' step='any' id='inputScore%d-%s'"
				" class='my-form-control' value='%1.2f' name='fScore%d-%s' %s><br>",
					i,field[2],
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


}//void funcJudge(FILE *fp)


char *cParseTextAreaLines(char *cTextArea)
{
	static unsigned uEnd=0;
	static unsigned uStart=0;
	static char cRetVal[512];
	
	uStart=uEnd;
	while(cTextArea[uEnd++])
	{
		if(cTextArea[uEnd]=='\n' || cTextArea[uEnd]=='\r' || cTextArea[uEnd]==0
				|| cTextArea[uEnd]==10 || cTextArea[uEnd]==13 )
		{
			if(cTextArea[uEnd]==0)
				break;

			cTextArea[uEnd]=0;
			sprintf(cRetVal,"%.511s",cTextArea+uStart);

			if(cRetVal[0]=='\n' || cRetVal[0]==13)
			{
				uStart=uEnd=0;
				return("");
			}

			if(cTextArea[uEnd+1]==10)
				uEnd+=2;
			else
				uEnd++;

			return(cRetVal);
		}
	}

	if(uStart!=uEnd)
	{
		sprintf(cRetVal,"%.511s",cTextArea+uStart);
		return(cRetVal);
	}

	uStart=uEnd=0;
	return("");
}//char *cParseTextAreaLines(char *cTextArea)


void funcAdmin(FILE *fp)
{

	//if(guPermLevel<10) return;

	MYSQL_RES *res;
	MYSQL_ROW field;
	char dStart[32]={""};
	long unsigned luStart=0;
	unsigned uHeatDuration=0;
	unsigned uHeatPreStart=0;
	unsigned uHeatPostEnd=0;
	char cEvent[32]={""};
	char cLine[256]={""};
	char cFirst[32]={""};
	char cLast[32]={""};
	char cCountry[32]={""};
	unsigned uParticipants=0;
	unsigned uRounds=0;
	unsigned uRound;
	unsigned uHeatSize=0;

	if(!uSelectEvent(fp,"Admin")) return;
	if(!uSelectHeat(fp,"Admin")) return;

	sprintf(gcQuery,"SELECT cLabel,cParticipants,uRounds,uHeatSize,uPassHeat,"
				"uHeatDuration,uHeatPreStart,uHeatPostEnd,dStart,"
				"UNIX_TIMESTAMP(dStart) FROM tEvent WHERE uEvent=%u",guEvent);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"<br>%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{

		sprintf(cEvent,"%.31s",field[0]);
		fprintf(fp,"<br><u>%s</u>",cEvent);
		fprintf(fp," (<a href=?gcPage=Admin&ClearEvent>X</a>)");
		sprintf(cLine,"%.255s",cParseTextAreaLines(field[1]));
		while(cLine[0])
		{

			//fprintf(fp,"<br>%.255s",cLine);
			sprintf(cLine,"%.255s",cParseTextAreaLines(field[1]));
			if(!cLine[0]) break;
			//164 &ntilde; 0xa4
			//165 &Ntilde; 0xa5
			sscanf(cLine,"%31[ A-z],%31[ A-z],%31[ A-Z]",cFirst,cLast,cCountry);
			fprintf(fp,"<br>|%.31s|%.31s|%.31s|",cFirst,cLast,cCountry);
			uParticipants++;

			//Generate Tournament PART 0
			//Insert participants if not already in tRider
			if(guPermLevel>9 && uGenerate)
			{
				MYSQL_RES *res2;
				MYSQL_ROW field2;

				sprintf(gcQuery,"DELETE FROM tRider WHERE"
						" (cFIRST='WINNER OF' OR cFirst='SECOND OF') AND uImportEvent=%u",guEvent);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					fprintf(fp,"<br>db4 %s",mysql_error(&gMysql));
					return;
				}

				sprintf(gcQuery,"SELECT uRider FROM tRider WHERE"
						" cLabel=UPPER(CONCAT(TRIM('%s'),' ',TRIM('%s'))) AND uOwner=%u AND uImportEvent=%u",
							cFirst,cLast,guOrg,guEvent);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					fprintf(fp,"<br>%s",mysql_error(&gMysql));
					return;
				}
				res2=mysql_store_result(&gMysql);
				if(mysql_num_rows(res2)==0)
				{
					sprintf(gcQuery,"INSERT INTO tRider SET cLabel=UPPER(CONCAT(TRIM('%s'),' ',TRIM('%s'))),"
							"cFirst=TRIM('%s'),cLast=TRIM('%s'),cCountry=TRIM('%s'),"
							"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),"
							"uImportEvent=%u",
								cFirst,cLast,
								cFirst,cLast,cCountry,
								guOrg,guLoginClient,
								guEvent);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						fprintf(fp,"<br>%s",mysql_error(&gMysql));
						return;
					}
				}
				else
				{
					//uRider exists!
					if((field2=mysql_fetch_row(res2)))
					{
						sprintf(gcQuery,"UPDATE tRider SET uModBy=0"
								" WHERE uRider=%s",field2[0]);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>db0 %s",mysql_error(&gMysql));
							return;
						}
					
					}
				}
			}//Generate Part 0
		}

		fprintf(fp,"<br><br>%u participants",uParticipants);
		sscanf(field[2],"%u",&uRounds);
		fprintf(fp,"<br>%u rounds",uRounds);
		sscanf(field[3],"%u",&uHeatSize);
		sprintf(dStart,"%.31s",field[8]);
		sscanf(field[9],"%lu",&luStart);
		fprintf(fp,"<br>First heat starts at %s",dStart);
		fprintf(fp,"<br>Heat size starts at %u participants each",uHeatSize);
		fprintf(fp,"<br>One half of participants pass to next round*.");
		sscanf(field[5],"%u",&uHeatDuration);
		fprintf(fp,"<br>Each heat lasts %u minutes",uHeatDuration);
		sscanf(field[6],"%u",&uHeatPreStart);
		fprintf(fp,"<br>Participants must be in position %u minutes before heat start",uHeatPreStart);
		sscanf(field[7],"%u",&uHeatPostEnd);
		fprintf(fp,"<br>Participants must clear competition area %u minutes after heat ends",uHeatPostEnd);

		fprintf(fp,"<br><br>Under Development...");
		//Generate Tournament PART 1
		if(guPermLevel>9 && uGenerate)
		{
			//Remove any round records for this event
			sprintf(gcQuery,"DELETE FROM tRound WHERE uEvent=%u",guEvent);
			mysql_query(&gMysql,gcQuery);
			res=mysql_store_result(&gMysql);
			if(mysql_errno(&gMysql))
			{
				fprintf(fp,"<br>%s",mysql_error(&gMysql));
				return;
			}
			//Remove any heat records for this event
			sprintf(gcQuery,"DELETE FROM tHeat WHERE uEvent=%u",guEvent);
			mysql_query(&gMysql,gcQuery);
			res=mysql_store_result(&gMysql);
			if(mysql_errno(&gMysql))
			{
				fprintf(fp,"<br>%s",mysql_error(&gMysql));
				return;
			}
			//Remove any score records for this event
			sprintf(gcQuery,"DELETE FROM tScore WHERE uEvent=%u",guEvent);
			mysql_query(&gMysql,gcQuery);
			res=mysql_store_result(&gMysql);
			if(mysql_errno(&gMysql))
			{
				fprintf(fp,"<br>%s",mysql_error(&gMysql));
				return;
			}
		}//Generate PART 1
		unsigned uSeqHeatNum=1;
		unsigned uNewRound=0;
		unsigned uNewHeat=0;
		char *cRound="";
		float fNumHeats=0.0;
		float fPrevNumHeats=0.0;
		float fCalc=0.0;
		float fParticipants=(float)uParticipants;
		register int i;
		register int j;
		unsigned uHeatSizes[8];
		for(i=0;i<8;i++)
			uHeatSizes[i]=uHeatSize;
		for(uRound=1;uRound<(uRounds+1);uRound++)
		{
			if(fNumHeats==1.0)
				break;
			fCalc=fParticipants/(float)uHeatSizes[uRound-1];
			fPrevNumHeats=fNumHeats;
			fNumHeats=ceil(fCalc);
			if(uRound==uRounds)
				cRound="Final ";
			else if(uRound==(uRounds-1))
				cRound="Semifinal ";
			if(fNumHeats==1.0)
				cRound="Final ";
			if(uRound==1 || (float)uHeatSizes[uRound-1]==(fParticipants/floor(fCalc)))
			{
				fprintf(fp,"<br>%sRound %u has %1.0f heats of %u participants each.",
					cRound,uRound,fNumHeats,uHeatSizes[uRound-1]);
				if(floor(fCalc)!=ceil(fCalc))
					fprintf(fp," one heat has less than %u participants.",uHeatSizes[uRound-1]);
				fprintf(fp," (%2.2f %2.2f)",fParticipants,fCalc);

			}
			else
			{
				if(uRound==uRounds)
					uHeatSizes[uRound-1]=uHeatSizes[uRound-1]-1;
				else
					uHeatSizes[uRound-1]=uHeatSizes[uRound-1]-2;
				fCalc=fParticipants/(float)(uHeatSizes[uRound-1]);
				fNumHeats=ceil(fCalc);
				fprintf(fp,"<br>Adjusted %sround %u has %1.0f heats of %u participants each.",
					cRound,uRound,fNumHeats,uHeatSizes[uRound-1]);
				if(floor(fCalc)!=ceil(fCalc))
					fprintf(fp," one heat has less than %u participants.",uHeatSizes[uRound-1]);
				fprintf(fp," (%2.2f %2.2f)",fParticipants,fCalc);
			}
			fParticipants=ceil(fParticipants/2.0);

			//Generate Tournament PART 2 for each round
			if(guPermLevel>9 && uGenerate)
			{

				//TODO uNumScores hardcoded preset. Should come from tEvent.
				sprintf(gcQuery,"INSERT INTO tRound SET uEvent=%u,cLabel='%s Round %u'"
						",uNumScores=3,uHeatSize=%u,uOwner=%u,uCreatedBy=%u"
							,guEvent,cRound,uRound,
							uHeatSizes[uRound-1],guOrg,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					fprintf(fp,"<br>%s",mysql_error(&gMysql));
					return;
				}
				uNewRound=mysql_insert_id(&gMysql);
				//Create heats
				for(j=0;j<(int)fNumHeats;j++)
				{
					sprintf(gcQuery,"INSERT INTO tHeat SET uEvent=%u,cLabel='%d',"
						"dStart=FROM_UNIXTIME(%lu),dEnd=FROM_UNIXTIME(%lu),"
						"uOwner=%u,uCreatedBy=%u,uRound=%u,uStatus=1"
							,guEvent,uSeqHeatNum++,
							luStart,luStart+(uHeatDuration*60),
							guOrg,guLoginClient,uNewRound);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						fprintf(fp,"<br>%s",mysql_error(&gMysql));
						return;
					}
					uNewHeat=mysql_insert_id(&gMysql);
					//Update start time adding pre and post delays
					luStart=luStart+(uHeatDuration*60)+(uHeatPreStart*60)+(uHeatPostEnd*60);

					if(uRound==1)
					{
						//Create one score for all riders in only round 1
						sprintf(gcQuery,"INSERT INTO tScore(cLabel,uRider,uHeat,uRound,uEvent,uOwner,uCreatedBy)"
						" SELECT cLabel,uRider,%u,%u,uImportEvent,%u,%u FROM tRider WHERE uImportEvent=%u"
						" AND uModBy=0 ORDER BY RAND() LIMIT %u",
							uNewHeat,uNewRound, guOrg,guLoginClient, guEvent,
							uHeatSizes[uRound-1]);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>%s",mysql_error(&gMysql));
							return;
						}

						MYSQL_RES *res;
						MYSQL_ROW field;
						unsigned uRemoveRider=0;
						//Take this heat size num of riders out
						sprintf(gcQuery,"SELECT uRider FROM tScore WHERE uHeat=%u",uNewHeat);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>%s",mysql_error(&gMysql));
							return;
						}
						res=mysql_store_result(&gMysql);
						while((field=mysql_fetch_row(res)))
						{
							sscanf(field[0],"%u",&uRemoveRider);
							//If you import again uModBy is set to 0 in PART 0
							sprintf(gcQuery,"UPDATE tRider SET uModBy=%u WHERE uRider=%u",
								guLoginClient,uRemoveRider);
							mysql_query(&gMysql,gcQuery);
							if(mysql_errno(&gMysql))
							{
								fprintf(fp,"<br>db1 %s",mysql_error(&gMysql));
								return;
							}
						}
					}
					else
					{
						fprintf(fp,"<br>uSeqHeatNum=%u uRound=%u uHeat=j=%d"
								" fPrevNumHeats=%1.0f fNumHeats=%1.0f uHeatSizes[]=%u",
							uSeqHeatNum,uRound,j,fPrevNumHeats,fNumHeats,uHeatSizes[uRound-1]);

						unsigned uNewRider=0;
						int iTwoXJPlus1=(2*(j+1));
						int iPrevNumHeats=(int)fPrevNumHeats;
						int iThree=1+((iTwoXJPlus1+2) % iPrevNumHeats);
						int iFour=1+((iTwoXJPlus1+3) % iPrevNumHeats);
						if(uHeatSizes[uRound-1]==4)
						{
							fprintf(fp,"<br>WH%u|WH%u|SH%u|SH%u",
							iTwoXJPlus1-1,iTwoXJPlus1,iThree,iFour);
						}
						else if(uHeatSizes[uRound-1]==2)
						{
							fprintf(fp,"<br>WH%u|SH%u",
							iThree,iFour);
						}
						else if(uHeatSizes[uRound-1]==3)
						{
							fprintf(fp,"<br>WH%u|WH%u|WH%u",
							iTwoXJPlus1-1,iTwoXJPlus1,iTwoXJPlus1+1);
						}

						if(uHeatSizes[uRound-1]==3 || uHeatSizes[uRound-1]==4)
						{
						//1
						sprintf(gcQuery,"INSERT INTO tRider SET"
							" cLabel='WINNER OF R%u H%u',cFirst='WINNER OF',cLast='R%u H%u',"
							" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uImportEvent=%u",
								uRound-1,iTwoXJPlus1-1,uRound-1,iTwoXJPlus1-1,
								guOrg,guLoginClient, guEvent);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>%s",mysql_error(&gMysql));
							return;
						}
						uNewRider=mysql_insert_id((&gMysql));
						sprintf(gcQuery,"INSERT INTO tScore(cLabel,uRider,uHeat,uRound,uEvent,uOwner,uCreatedBy)"
							" SELECT cLabel,uRider,%u,%u,uImportEvent,%u,%u FROM tRider WHERE uRider=%u",
								uNewHeat,uNewRound, guOrg,guLoginClient, uNewRider);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>db2 %s",mysql_error(&gMysql));
							return;
						}

						//2
						sprintf(gcQuery,"INSERT INTO tRider SET"
							" cLabel='WINNER OF R%u H%u',cFirst='WINNER OF',cLast='R%u H%u',"
							" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uImportEvent=%u",
								uRound-1,iTwoXJPlus1,uRound-1,iTwoXJPlus1,
								guOrg,guLoginClient, guEvent);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>%s",mysql_error(&gMysql));
							return;
						}
						uNewRider=mysql_insert_id((&gMysql));
						sprintf(gcQuery,"INSERT INTO tScore(cLabel,uRider,uHeat,uRound,uEvent,uOwner,uCreatedBy)"
							" SELECT cLabel,uRider,%u,%u,uImportEvent,%u,%u FROM tRider WHERE uRider=%u",
								uNewHeat,uNewRound, guOrg,guLoginClient, uNewRider);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>db2 %s",mysql_error(&gMysql));
							return;
						}

						}

						if(uHeatSizes[uRound-1]==2)
						{
						//1
						sprintf(gcQuery,"INSERT INTO tRider SET"
							" cLabel='WINNER OF R%u H%u',cFirst='WINNER OF',cLast='R%u H%u',"
							" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uImportEvent=%u",
								uRound-1,iThree,uRound-1,iThree,
								guOrg,guLoginClient, guEvent);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>%s",mysql_error(&gMysql));
							return;
						}
						uNewRider=mysql_insert_id((&gMysql));
						sprintf(gcQuery,"INSERT INTO tScore(cLabel,uRider,uHeat,uRound,uEvent,uOwner,uCreatedBy)"
							" SELECT cLabel,uRider,%u,%u,uImportEvent,%u,%u FROM tRider WHERE uRider=%u",
								uNewHeat,uNewRound, guOrg,guLoginClient, uNewRider);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>db2 %s",mysql_error(&gMysql));
							return;
						}

						//2
						sprintf(gcQuery,"INSERT INTO tRider SET"
							" cLabel='SECOND OF R%u H%u',cFirst='SECOND OF',cLast='R%u H%u',"
							" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uImportEvent=%u",
								uRound-1,iFour,uRound-1,iFour,
								guOrg,guLoginClient, guEvent);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>%s",mysql_error(&gMysql));
							return;
						}
						uNewRider=mysql_insert_id((&gMysql));
						sprintf(gcQuery,"INSERT INTO tScore(cLabel,uRider,uHeat,uRound,uEvent,uOwner,uCreatedBy)"
							" SELECT cLabel,uRider,%u,%u,uImportEvent,%u,%u FROM tRider WHERE uRider=%u",
								uNewHeat,uNewRound, guOrg,guLoginClient, uNewRider);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>db2 %s",mysql_error(&gMysql));
							return;
						}
						}

						if(uHeatSizes[uRound-1]==4)
						{
						//3
						sprintf(gcQuery,"INSERT INTO tRider SET"
							" cLabel='SECOND OF R%u H%u',cFirst='SECOND OF',cLast='R%u H%u',"
							" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uImportEvent=%u",
								uRound-1,iThree,uRound-1,iThree,
								guOrg,guLoginClient, guEvent);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>%s",mysql_error(&gMysql));
							return;
						}
						uNewRider=mysql_insert_id((&gMysql));
						sprintf(gcQuery,"INSERT INTO tScore(cLabel,uRider,uHeat,uRound,uEvent,uOwner,uCreatedBy)"
							" SELECT cLabel,uRider,%u,%u,uImportEvent,%u,%u FROM tRider WHERE uRider=%u",
								uNewHeat,uNewRound, guOrg,guLoginClient, uNewRider);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>db2 %s",mysql_error(&gMysql));
							return;
						}

						//4
						sprintf(gcQuery,"INSERT INTO tRider SET"
							" cLabel='SECOND OF R%u H%u',cFirst='SECOND OF',cLast='R%u H%u',"
							" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uImportEvent=%u",
								uRound-1,iFour,uRound-1,iFour,
								guOrg,guLoginClient, guEvent);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>%s",mysql_error(&gMysql));
							return;
						}
						uNewRider=mysql_insert_id((&gMysql));
						sprintf(gcQuery,"INSERT INTO tScore(cLabel,uRider,uHeat,uRound,uEvent,uOwner,uCreatedBy)"
							" SELECT cLabel,uRider,%u,%u,uImportEvent,%u,%u FROM tRider WHERE uRider=%u",
								uNewHeat,uNewRound, guOrg,guLoginClient, uNewRider);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>db2 %s",mysql_error(&gMysql));
							return;
						}

						}
						else if(uHeatSizes[uRound-1]==3)
						{
						//3
						sprintf(gcQuery,"INSERT INTO tRider SET"
							" cLabel='WINNER OF R%u H%u',cFirst='WINNER OF',cLast='R%u H%u',"
							" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uImportEvent=%u",
								uRound-1,iTwoXJPlus1+1,uRound-1,iTwoXJPlus1+1,
								guOrg,guLoginClient, guEvent);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>%s",mysql_error(&gMysql));
							return;
						}
						uNewRider=mysql_insert_id((&gMysql));
						sprintf(gcQuery,"INSERT INTO tScore(cLabel,uRider,uHeat,uRound,uEvent,uOwner,uCreatedBy)"
							" SELECT cLabel,uRider,%u,%u,uImportEvent,%u,%u FROM tRider WHERE uRider=%u",
								uNewHeat,uNewRound, guOrg,guLoginClient, uNewRider);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							fprintf(fp,"<br>db2 %s",mysql_error(&gMysql));
							return;
						}

						}
					}
				}
						
			}//Generate PART 2

		}
		if(guPermLevel>9 && !uGenerate)
			fprintf(fp,"<br><br><a href=?gcPage=Admin&Generate type=button >Generate Tournament</a>");
		if(uGenerate)
			fprintf(fp,"<br><br>Tournament Generated v0.1</a>");

	}//if field
}//void funcAdmin(FILE *fp)


char *cRiders(unsigned uHeat)
{
	static char cBuffer[512]={""};
	static char cLast[32]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT DISTINCT CONCAT(UPPER(SUBSTR(tRider.cFirst,1,1)),'. ',UPPER(tRider.cLast)) FROM tScore,tRider WHERE tScore.uHeat=%u AND tRider.uRider=tScore.uRider",uHeat);
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
		sprintf(cLast,"%.31s<br>",field[0]);
		strncat(cBuffer,cLast,35);
		uSize+=strlen(cLast);
		if(uSize>(512-36)) break;
	}

	return(cBuffer);
}//char *cRiders(unsigned uHeat)


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
			sprintf(cEvent,"%.31s",field[6]);
			sprintf(cRound,"%.31s",field[7]);
			sscanf(field[8],"%u",&uNumScores); sscanf(field[9],"%u",&uTrickLock);
			if(!uOnlyOnce++)
			{
				fprintf(fp,"<div class=\"sTable\">\n");
				fprintf(fp,"<div class=\"sTableRow\">\n");
				fprintf(fp,"<div class=\"sTableCellBlue\">%s (<a href=?gcPage=Event&ClearEvent>X</a>)</div>",cEvent);
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
 				" (<a href=?gcPage=Event&ClearEvent>X</a>)</div>",cForeignKey("tEvent","cLabel",guEvent));
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

}//void funcEvent(FILE *fp)


void funcBestTrick(FILE *fp)
{
}


void funcHeat(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	MYSQL_RES *res2;
	MYSQL_ROW field2;
	MYSQL_RES *res3;
	MYSQL_ROW field3;
	unsigned uEvent=0;
	unsigned uHeat=0;
	unsigned uRound=0;
	unsigned uNumScores=0;
	unsigned uStatus=0;
	char cStatus[32]={""};
	char cHeat[32]={""};
	char cEvent[32]={""};
	char cRound[32]={""};
	char dTimeLeft[32]={"---"};
	register int i,j;
	float fTotalScore=0.00;
	float fLeaderTotalScore=0.00;
	float fScoreArray[8]={0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00};

	fprintf(fp,"<div class=\"sTable\">");
	fprintf(fp,"<div class=\"sTableRow\">");

	//sprintf(gcQuery,"SELECT uEvent,cLabel FROM tEvent WHERE uStatus=1 AND uOwner=%u ORDER BY uEvent DESC LIMIT 1",guOrg);
	sprintf(gcQuery,"SELECT uEvent,cLabel,uOwner FROM tEvent WHERE uStatus=1 ORDER BY uEvent DESC LIMIT 1");
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uEvent);
		sprintf(cEvent,"%.31s",field[1]);
		sscanf(field[2],"%u",&guOrg);
	}
	if(!uEvent) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">No active event!</div>");
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	sprintf(gcQuery,"SELECT tHeat.uHeat,tHeat.cLabel,tHeat.uRound,"
			" TIME_FORMAT(TIMEDIFF(tHeat.dEnd,NOW()),'%%H:%%i:%%s'),"
			"UPPER(tStatus.cLabel),tStatus.uStatus"
			" FROM tHeat,tStatus WHERE tHeat.uStatus<3 AND"
			" tHeat.uEvent=%u AND tHeat.dEnd>NOW() AND tStatus.uStatus=tHeat.uStatus"
			" ORDER BY tHeat.uHeat DESC LIMIT 1",uEvent);
	mysql_query(&gMysql,gcQuery);
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
	}
	if(!uHeat || !uRound) 
	{
		funcHeatEnd(fp);
		return;
		fprintf(fp,"<div class=\"sTableCellYellow\">No active or suspended heat for event %s</div>",cEvent);
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	sprintf(gcQuery,"SELECT uNumScores,cLabel FROM tRound WHERE uRound=%u AND uOwner=%u",uRound,guOrg);
	mysql_query(&gMysql,gcQuery);
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
		fprintf(fp,"<div class=\"sTableCellGreenLarge\">TO END</div>");
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
	while((field3=mysql_fetch_row(res3)))
	{
		

	sprintf(gcQuery,"SELECT DISTINCT UPPER(SUBSTR(tRider.cFirst,1,1)),UPPER(tRider.cLast),tRider.uRider"
			" FROM tScoreComp,tRider"
			" WHERE tScoreComp.uRider=tRider.uRider"
			" AND tScoreComp.uHeat=%u AND tRider.uRider=%s",uHeat,field3[0]);
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
		sprintf(gcQuery,"SELECT fScore,uIndex FROM tScoreComp WHERE uHeat=%u AND uRider=%s LIMIT 8",
				uHeat,field[2]);
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
		//fTotalScore=fTotalScore/uNumScores;

		fprintf(fp,"<div class=\"sTableRow\">");
		fprintf(fp,"<div class=\"sTableCellLarge\">%d</div>",j++);
		fprintf(fp,"<div class=\"sTableCellBlackLarge\">%s. %s</div>",field[0],field[1]);
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
		fprintf(fp,"<div class=\"sTableCellBlack\">%s</div>",gcQuery);
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


void PopulateScoreComp(unsigned uHeat)
{
        MYSQL_RES *res;
	MYSQL_ROW field;
        MYSQL_RES *res2;
	MYSQL_ROW field2;
	unsigned uRider=0;
	unsigned uEvent=0;
	unsigned uRound=0;
	unsigned uIndex=0;
	unsigned uNumJudges=0;
	unsigned uScoreComp=0;
	float fScore=0.0;
	//Every time a score is set we need to
	//update the tScoreComp table.
	//We need to average the scores among all judges
	//There can only be one record per uRider/uHeat 
	//this is also insured with a UNIQUE INDEX
	//
	sprintf(gcQuery,"SELECT uCreatedBy FROM tScore WHERE uHeat=%u GROUP BY uCreatedBy",
				uHeat);
	mysql_query(&gMysql,gcQuery);
	if(*mysql_error(&gMysql))
	{
		gcMessage="PopulateScoreComp ErrorA";
		return;
	}
	res=mysql_store_result(&gMysql);
	uNumJudges=mysql_num_rows(res);
	if(!uNumJudges || uNumJudges>8)
	{
		gcMessage="PopulateScoreComp ErrorB";
		return;
	}

	sprintf(gcQuery,"SELECT uIndex,(SUM(fScore)/%u),uRound,uEvent,uRider FROM tScore WHERE uHeat=%u GROUP BY uRider,uIndex",
				uNumJudges,uHeat);
	mysql_query(&gMysql,gcQuery);
	if(*mysql_error(&gMysql))
	{
		gcMessage="PopulateScoreComp Error0";
		return;
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uIndex);
		sscanf(field[1],"%f",&fScore);
		sscanf(field[2],"%u",&uRound);
		sscanf(field[3],"%u",&uEvent);
		sscanf(field[4],"%u",&uRider);
		sprintf(gcQuery,"SELECT uScoreComp FROM tScoreComp WHERE uHeat=%u AND uRider=%u AND uIndex=%u",
				uHeat,uRider,uIndex);
		mysql_query(&gMysql,gcQuery);
		if(*mysql_error(&gMysql))
		{
			gcMessage="PopulateScoreComp Error1";
			return;
		}
		res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			sscanf(field2[0],"%u",&uScoreComp);
			sprintf(gcQuery,"UPDATE tScoreComp SET fScore=%1.2f,uModDate=UNIX_TIMESTAMP(NOW()),uModBy=%u"
					" WHERE uScoreComp=%u",
						fScore,guLoginClient,
						uScoreComp);
			mysql_query(&gMysql,gcQuery);
			if(*mysql_error(&gMysql))
			{
				gcMessage="PopulateScoreComp Error2";
				return;
			}
		}
		else
		{
			sprintf(gcQuery,"INSERT INTO tScoreComp SET fScore=%1.2f,uIndex=%u,uHeat=%u,uRider=%u"
					",uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=%u,uOwner=%u,uRound=%u,uEvent=%u ",
				fScore,uIndex,uHeat,uRider,guLoginClient,guOrg,uRound,uEvent);
			mysql_query(&gMysql,gcQuery);
			if(*mysql_error(&gMysql))
			{
				gcMessage="PopulateScoreComp Error3";
				return;
			}
		}
		
	}
	
}//void PopulateScoreComp(unsigned uHeat)
