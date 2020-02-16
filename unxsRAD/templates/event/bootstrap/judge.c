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
static unsigned uEvent=0;
static unsigned uRound=0;
static unsigned uHeat=0;
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
			sscanf(entries[i].val,"%u",&uHeat);
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


void JudgeGetHook(entry gentries[],int x)
{
	CommonGetHook(gentries,x);

	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uHeat"))
			sscanf(gentries[i].val,"%u",&uHeat);
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

	htmlAdmin();

}//void AdminGetHook(entry gentries[],int x)


void JudgeCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"Score"))
	{
		ProcessJudgeVars(entries,x);

		gcMessage="";
		if(!uEvent)
			gcMessage="uEvent not specified";
		if(!uHeat)
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
						uHeat,uIndex,uRider,
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
								uHeat,uIndex,uRider,uEvent,guOrg,uRound);
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
		PopulateScoreComp(uHeat);
	}//gcFunction=Score
	htmlJudge();

}//void UserCommands(pentry entries[], int x)

void GetdEnd(void)
{
        MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT DATE_FORMAT(dEnd,'%%b %%d, %%Y %%H:%%i:%%s')"
			" FROM tHeat WHERE uStatus=1 AND dEnd>NOW() ORDER BY uHeat DESC LIMIT 1");
	mysql_query(&gMysql,gcQuery);
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

	sprintf(gcQuery,"SELECT tHeat.uHeat,UPPER(tHeat.cLabel),tHeat.uRound,"
			" DATE_FORMAT(tHeat.dStart,'%%H:%%i:%%s'),DATE_FORMAT(tHeat.dEnd,'%%H:%%i:%%s'),"
			"tHeat.uTrickLock,tStatus.cLabel,tStatus.uStatus,"
			"TIME_FORMAT(TIMEDIFF(tHeat.dEnd,NOW()),'%%i:%%s'),tEvent.cLabel"
			" FROM tHeat,tStatus,tEvent"
			" WHERE tHeat.uStatus<3 AND tHeat.uEvent=%u AND tStatus.uStatus=tHeat.uStatus"
			" AND tHeat.uEvent=tEvent.uEvent"
			" ORDER BY tHeat.uHeat DESC LIMIT 1",
					guEvent);
	mysql_query(&gMysql,gcQuery);
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
	fprintf(fp,"<div class=\"sTableCellBlack\">HEAT %s</div>",cHeat);
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
	char cEvent[256]={""};
	char cLine[256]={""};
	char cFirst[32]={""};
	char cLast[32]={""};
	char cCountry[32]={""};
	unsigned uParticipants=0;
	unsigned uRounds=0;
	unsigned uRound;
	unsigned uHeatSize=0;
	unsigned uPassHeat=0;

	if(!uSelectEvent(fp,"Admin")) return;
	if(!uSelectHeat(fp,"Admin")) return;

	sprintf(gcQuery,"SELECT cLabel,cParticipants,uRounds,uHeatSize,uPassHeat,"
				"uHeatDuration,uHeatPreStart,uHeatPostEnd FROM tEvent WHERE uEvent=%u",guEvent);
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	while((field=mysql_fetch_row(res)))
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
			sscanf(cLine,"%31[ A-z],%31[ A-z],%31[ A-Z]",cFirst,cLast,cCountry);
			fprintf(fp,"<br>%.31s %.31s %.31s",cFirst,cLast,cCountry);
			uParticipants++;
		}

		fprintf(fp,"<br><br>%u participants",uParticipants);
		sscanf(field[2],"%u",&uRounds);
		fprintf(fp,"<br>%u rounds",uRounds);
		sscanf(field[3],"%u",&uHeatSize);
		fprintf(fp,"<br>Heat size starts at %u participants each",uHeatSize);
		sscanf(field[4],"%u",&uPassHeat);
		fprintf(fp,"<br>%u participants pass to next round",uPassHeat);
		fprintf(fp,"<br>Each heat lasts %s minutes",field[5]);
		fprintf(fp,"<br>Participants must be in position %s minutes before heat start",field[6]);
		fprintf(fp,"<br>Participants must clear competition area %s minutes after heat ends",field[7]);

		fprintf(fp,"<br><br>Under Development...");
		char *cRound="";
		float fNumHeats=0.0;
		float fCalc=0.0;
		float fParticipants=(float)uParticipants;
		unsigned uHeatSizes[8];
		register int i;
		for(i=0;i<8;i++)
			uHeatSizes[i]=uHeatSize;
		for(uRound=1;uRound<(uRounds+1);uRound++)
		{
			if(fNumHeats==1.0)
				break;
			fCalc=fParticipants/(float)uHeatSizes[uRound-1];
			fNumHeats=ceil(fCalc);
			if(uRound==uRounds)
				cRound="Final ";
			else if(uRound==(uRounds-1))
				cRound="Semifinal ";
			if(fNumHeats==1.0)
				cRound="Final ";
			if(uRound==1 || (float)uHeatSizes[uRound-1]==(fParticipants/floor(fCalc)))
			{
				fprintf(fp,"<br>%sround %u has %1.0f heats of %u participants each.",
					cRound,uRound,fNumHeats,uHeatSizes[uRound-1]);
				if(floor(fCalc)!=ceil(fCalc))
					fprintf(fp," one heat has less than %u participants.",uHeatSizes[uRound-1]);
				fprintf(fp," (%2.2f %2.2f)",fParticipants,fCalc);
			}
			else
			{
				fCalc=fParticipants/(float)(uHeatSizes[uRound-1]-1);
				fNumHeats=ceil(fCalc);
				fprintf(fp,"<br>Adjusted %sround %u has %1.0f heats of %u participants each.",
					cRound,uRound,fNumHeats,uHeatSizes[uRound-1]-1);
				if(floor(fCalc)!=ceil(fCalc))
					fprintf(fp," one heat has less than %u participants.",uHeatSizes[uRound-1]-1);
				fprintf(fp," (%2.2f %2.2f)",fParticipants,fCalc);
			}
			fParticipants=ceil(fParticipants/2.0);
		}
	}//while field
}//void funcAdmin(FILE *fp)


void funcEvent(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uHeat=0;
	unsigned uRound=0;
	unsigned uNumScores=0;
	unsigned uStatus=0;
	char cStatus[32]={""};
	char cHeat[32]={""};
	char cRound[32]={""};
	char dTimeLeft[32]={"---"};

	if(!uSelectEvent(fp,"Event")) return;

	fprintf(fp,"<div class=\"sTable\">");
	fprintf(fp,"<div class=\"sTableRow\">");

	sprintf(gcQuery,"SELECT tHeat.uHeat,UPPER(tHeat.cLabel),tHeat.uRound,"
			" TIME_FORMAT(TIMEDIFF(tHeat.dEnd,NOW()),'%%i:%%s'),"
			"UPPER(tStatus.cLabel),tStatus.uStatus,UPPER(tEvent.cLabel),UPPER(tRound.cLabel),tRound.uNumScores,tHeat.uTrickLock"
			" FROM tEvent,tRound,tHeat,tStatus"
			" WHERE tHeat.uEvent=%u AND tStatus.uStatus=tHeat.uStatus"
			" AND tEvent.uEvent=tHeat.uEvent AND tRound.uRound=tHeat.uRound"
			" ORDER BY tEvent.uEvent,tRound.uRound,tHeat.uHeat",guEvent);
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
		fprintf(fp,"</div>");//row
		fprintf(fp,"</div>");//table

		fprintf(fp,"<div class=\"sTable\">");
		fprintf(fp,"<div class=\"sTableRow\">");
		fprintf(fp,"<div class=\"sTableCellBlue\">EVENT</div>");
		fprintf(fp,"<div class=\"sTableCellBlue\">ROUND</div>");
		fprintf(fp,"<div class=\"sTableCellBlue\">HEAT</div>");
		fprintf(fp,"<div class=\"sTableCellBlue\">STATUS</div>");
		fprintf(fp,"<div class=\"sTableCellBlue\">TIME LEFT</div>");
		fprintf(fp,"<div class=\"sTableCellBlue\">NUM SCORES</div>");
		fprintf(fp,"<div class=\"sTableCellBlue\">TRICK LOCK</div>");
		fprintf(fp,"</div>");//row
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
			sscanf(field[8],"%u",&uNumScores); sscanf(field[9],"%u",&uTrickLock); fprintf(fp,"<div class=\"sTableRow\">");
			fprintf(fp,"<div class=\"sTableCellBlack\">%s</div>",cEvent);
			fprintf(fp,"<div class=\"sTableCellBlack\">%s</div>",cRound);
			if(uStatus==1)
				fprintf(fp,"<div class=\"sTableCellBlack\"><a href=?gcFunction=Heat>%s</a></div>",cHeat);
			else
				fprintf(fp,"<div class=\"sTableCellBlack\"><a href=?gcFunction=HeatEnd&uHeat=%u>%s</a></div>",uHeat,cHeat);
			fprintf(fp,"<div class=\"sTableCellBlack\">%s</div>",cStatus);
			fprintf(fp,"<div class=\"sTableCellBlack\">%s</div>",dTimeLeft);
			fprintf(fp,"<div class=\"sTableCellBlack\">%u</div>",uNumScores);
			fprintf(fp,"<div class=\"sTableCellBlack\">%u</div>",uTrickLock);
			fprintf(fp,"</div>");//row
		}
		fprintf(fp,"</div>");//table
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
			" TIME_FORMAT(TIMEDIFF(tHeat.dEnd,NOW()),'%%i:%%s'),"
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
	char cHeat[32]={""};
	char cEvent[32]={""};
	char cRound[32]={""};
	char dTimeLeft[32]={""};
	register int i,j;
	float fTotalScore=0.00;
	float fScoreArray[8]={0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00};

	fprintf(fp,"<div class=\"sTable\">");
	fprintf(fp,"<div class=\"sTableRow\">");

	sprintf(gcQuery,"SELECT uEvent,cLabel FROM tEvent WHERE uStatus=1 AND uOwner=%u ORDER BY uEvent DESC LIMIT 1",guOrg);
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uEvent);
		sprintf(cEvent,"%.31s",field[1]);
	}
	if(!uEvent) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">No active event!</div>");
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	sprintf(gcQuery,"SELECT tHeat.cLabel,tHeat.uRound,UPPER(tStatus.cLabel) FROM tHeat,tStatus"
				" WHERE tHeat.uHeat=%u AND tHeat.uOwner=%u AND tHeat.uStatus>1 AND tHeat.uStatus=tStatus.uStatus"
				" ORDER BY tHeat.uHeat DESC LIMIT 1",uHeat,guOrg);
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cHeat,"%.31s",field[0]);
		sscanf(field[1],"%u",&uRound);
		sprintf(cStatus,"%.31s",field[2]);
	}
	if(!uRound) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">This heat does not exist or is still active. Event %s</div>",cEvent);
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


	fprintf(fp,"<div class=\"sTableCellGreenLarge\">%s</div>",cStatus);
	fprintf(fp,"</div>");//row

	fprintf(fp,"<div class=\"sTableRow\">");
	fprintf(fp,"<div class=\"sTableCellBlackBoldLarge\"><p id=\"demo\">%s</p></div>",dTimeLeft);
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
		uHeat);
	mysql_query(&gMysql,gcQuery);
	if(*mysql_error(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res3=mysql_store_result(&gMysql);
	while((field3=mysql_fetch_row(res3)))
	{
		

	sprintf(gcQuery,"SELECT DISTINCT UPPER(tRider.cFirst),UPPER(tRider.cLast),tRider.uRider,tRider.cCountry"
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
		//Get scores for single judge
		for(i=0;i<uNumScores&&i<8;i++)
			fScoreArray[i]=0.00;
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

		fprintf(fp,"<div class=\"sTableRow\">");
		fprintf(fp,"<div class=\"sTableCellLarge\">%d</div>",j++);
		if(j==2)
			fprintf(fp,"<div class=\"sTableCellBlueLarge\">%s %s</div>",field[0],field[1]);
		else
			fprintf(fp,"<div class=\"sTableCellBlackLarge\">%s %s</div>",field[0],field[1]);
		fprintf(fp,"<div class=\"sTableCellBlackLarge\"><img alt=%s src=/bs/images/%s.png></div>",field[3],field[3]);
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
