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

//File globals
static unsigned uEvent=0;
static unsigned uHeat=0;
static unsigned uTrickLock=0;
static unsigned uTrickLockProvided=0;
static char dEnd[32]={"Jan 1, 2021 15:37:25"};

//TOC
void ProcessJudgeVars(pentry entries[], int x);
void JudgeGetHook(entry gentries[],int x);
void JudgeCommands(pentry entries[], int x);
void htmlHeat(void);
void htmlBestTrick(void);
void htmlHeatEnd(void);
void htmlEvent(void);

void htmlJudge(void);
//extern
void unxsvzLog(unsigned uTablePK,char *cTableName,char *cLogEntry,unsigned guPermLevel,unsigned guLoginClient,char *gcLogin,char *gcHost);

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
	}

}//void ProcessJobOfferVars(pentry entries[], int x)


void JudgeGetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uHeat"))
			sscanf(gentries[i].val,"%u",&uHeat);
	}


}//void JudgeGetHook(entry gentries[],int x)


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
							" ,uHeat=%u,uIndex=%u,uRider=%u,uEvent=%u,uOwner=%u",
								cForeignKey("tRider","cLast",uRider),uIndex+1,entries[i].val,guLoginClient,
								uHeat,uIndex,uRider,uEvent,guOrg);
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
					}
				     }
				}
			}
		}
	}
	htmlJudge();

}//void UserCommands(pentry entries[], int x)


void htmlHeat(void)
{
        MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT DATE_FORMAT(dEnd,'%%b %%d, %%Y %%H:%%i:%%s')"
			" FROM tHeat WHERE uStatus=1 AND uOwner=%u AND dEnd>NOW() ORDER BY uHeat DESC LIMIT 1",guOrg);
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(field[0][0])
			sprintf(dEnd,"%.31s",field[0]);
	}

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
	htmlFooter("Default.Footer");
}//void htmlEvent(void)


void htmlJudge(void)
{
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

			template.cpName[7]="";

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


void funcHeatScoreTable(FILE *fp)
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
	char cHeat[32]={""};
	char cEvent[32]={""};
	char cRound[32]={""};
	char dStart[32]={""};
	char dEnd[32]={""};
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

	sprintf(gcQuery,"SELECT uHeat,cLabel,uRound,"
			" DATE_FORMAT(dStart,'%%H:%%i:%%s'),DATE_FORMAT(dEnd,'%%H:%%i:%%s'),uTrickLock"
			" FROM tHeat WHERE uStatus=1 AND uOwner=%u ORDER BY uHeat DESC LIMIT 1",guOrg);
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
	}
	if(!uHeat || !uRound) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">No active heat for event %s</div>",cEvent);
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


	fprintf(fp,"<div class=\"sTableCellGreen\">Start: %s</div>",dStart);
	fprintf(fp,"<div class=\"sTableCellGreen\">End: %s</div>",dEnd);
	fprintf(fp,"<div class=\"sTableCellBlack\">%s</div>",cRound);
	fprintf(fp,"<div class=\"sTableCellBlack\">Heat %s</div>",cHeat);
	fprintf(fp,"</div>");

	fprintf(fp,"</div>");


      	fprintf(fp,"<form class=\"form-signin\" role=\"form\" method=\"post\" action=\"/unxsEVApp/\">");
	fprintf(fp,"<input type='hidden' name='uEvent' value='%u'>",uEvent);
	fprintf(fp,"<input type='hidden' name='uHeat' value='%u'>",uHeat);
	fprintf(fp,"<input type='hidden' name='uTrickLock' value='%u'>",uTrickLock);
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


}//void funcHeatScoreTable(FILE *fp)




void funcEvent(FILE *fp)
{
}


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

	sprintf(gcQuery,"SELECT uHeat,cLabel,uRound,"
			" TIME_FORMAT(TIMEDIFF(dEnd,NOW()),'%%i:%%s')"
			" FROM tHeat WHERE uStatus=1 AND uOwner=%u AND dEnd>NOW() ORDER BY uHeat DESC LIMIT 1",guOrg);
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uHeat);
		sprintf(cHeat,"%.31s",field[1]);
		sscanf(field[2],"%u",&uRound);
		if(field[3][0])
			sprintf(dTimeLeft,"%.31s",field[3]);
	}
	if(!uHeat || !uRound) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">No active heat for event %s</div>",cEvent);
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


	fprintf(fp,"<div class=\"sTableCellGreenLarge\">TO END</div>");
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
	sprintf(gcQuery,"SELECT uRider FROM tScore WHERE uHeat=%u AND uOwner=%u GROUP BY uRider ORDER BY SUM(fScore) DESC",
		uHeat,guOrg);
	mysql_query(&gMysql,gcQuery);
	if(*mysql_error(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res3=mysql_store_result(&gMysql);
	unsigned uLast=mysql_num_rows(res3);
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

		if(j==uLast)
		{
			fprintf(fp,"<div class=\"sTableRow\">");
			fprintf(fp,"<div class=\"sTableCellEmpty\"></div>");
			fprintf(fp,"<div class=\"sTableCellBlueLarge\">Needs</div>");
			fprintf(fp,"</div>");
		}
		if(j==uLast+1)
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

	sprintf(gcQuery,"SELECT cLabel,uRound FROM tHeat"
				" WHERE uHeat=%u AND uOwner=%u"
				" ORDER BY uHeat DESC LIMIT 1",uHeat,guOrg);
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cHeat,"%.31s",field[0]);
		sscanf(field[1],"%u",&uRound);
	}
	if(!uRound) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">No such heat for event %s</div>",cEvent);
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


	fprintf(fp,"<div class=\"sTableCellGreenLarge\">HEAT FINALIZED</div>");
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
	sprintf(gcQuery,"SELECT uRider FROM tScore WHERE uHeat=%u AND uOwner=%u AND uCreatedBy=%u GROUP BY uRider ORDER BY SUM(fScore) DESC",
		uHeat,guOrg,guLoginClient);
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
		fprintf(fp,"</div>No score config data avaiable!");
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
