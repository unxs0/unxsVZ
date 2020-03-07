/*
AUTHOR/LEGAL
	(C) 2010-2020 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	For judging heat scores
MORE
	Included from judge.c
	See automation.c also
*/

//TOC
void GetdEnd(void);
void funcJudge(FILE *fp);


//Must work with funcJudge e.g.
void GetdEnd(void)
{
        MYSQL_RES *res;
	MYSQL_ROW field;
	if(!suHeat && guHeat)
		suHeat=guHeat;
	if(!suHeat && guEvent)
		sprintf(gcQuery,"SELECT DATE_FORMAT(tHeat.dEnd,'%%b %%d, %%Y %%H:%%i:%%s')"
			" FROM tHeat,tEvent WHERE tHeat.uStatus=1 AND tHeat.uEvent=tEvent.uEvent AND tHeat.uEvent=%u"
			" AND NOW()>=DATE_SUB(tHeat.dStart,INTERVAL tEvent.uHeatPreStart MINUTE)"
			" AND NOW()<=DATE_ADD(tHeat.dEnd,INTERVAL tEvent.uHeatPostEnd MINUTE)"
			" LIMIT 1",
				guEvent);
	else if(suHeat)
		sprintf(gcQuery,"SELECT DATE_FORMAT(tHeat.dEnd,'%%b %%d, %%Y %%H:%%i:%%s')"
			" FROM tHeat,tEvent WHERE tHeat.uStatus=1 AND tHeat.uEvent=tEvent.uEvent AND tHeat.uHeat=%u",
				suHeat);
	else if(1)
		sprintf(gcQuery,"SELECT DATE_FORMAT(tHeat.dEnd,'%%b %%d, %%Y %%H:%%i:%%s')"
			" FROM tHeat,tEvent WHERE tHeat.uStatus=1 AND tHeat.uEvent=tEvent.uEvent"
			" AND NOW()>=DATE_SUB(tHeat.dStart,INTERVAL tEvent.uHeatPreStart MINUTE)"
			" ORDER BY tEvent.uEvent,TIMESTAMPDIFF(SECOND,tHeat.dStart,NOW()) LIMIT 1");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlHeader("GetdEnd","Default.Header");
		fprintf(stdout,"%.255s",mysql_error(&gMysql));
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)==0)
	{
		htmlHeader("GetdEnd","Default.Header");
		fprintf(stdout,"No rows GetdEnd():%u %u",guEvent,suHeat);
		htmlFooter("Default.Footer");
	}
	if((field=mysql_fetch_row(res)))
	{
		if(field[0][0])
			sprintf(dEnd,"%.31s",field[0]);
	}
}//GetdEnd


void funcJudge(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	MYSQL_RES *res2;
	MYSQL_ROW field2;
	unsigned uHeat=0;
	unsigned uRound=0;
	unsigned uStatus=0;
	unsigned uTimeStatus=0;
	char cStatus[32]={""};
	char cHeat[32]={""};
	char cRound[32]={""};
	char dStart[32]={""};
	char dEnd[32]={""};
	char dTimeLeft[32]={""};
	char dTimeToStart[32]={""};


	if(!uSelectEvent(fp,"Judge")) return;
	if(!uSelectRound(fp,"Judge")) return;
	if(!uSelectHeat(fp,"Judge")) return;
	if(!uSelectRider(fp,"Judge",0)) return;

	fprintf(fp,"<div class=\"sTable\">");
	fprintf(fp,"<div class=\"sTableRow\">");

	if(!suHeat)
		suHeat=guHeat;
	sprintf(gcQuery,"SELECT tHeat.uHeat,UPPER(tHeat.cLabel),tHeat.uRound,"
			" DATE_FORMAT(tHeat.dStart,'%%H:%%i:%%s'),DATE_FORMAT(tHeat.dEnd,'%%H:%%i:%%s'),"
			"UPPER(tStatus.cLabel),tStatus.uStatus,"
			"TIME_FORMAT(TIMEDIFF(tHeat.dEnd,NOW()),'%%i:%%s'),UPPER(tEvent.cLabel),UPPER(tRound.cLabel),"
			"IF((NOW()<tHeat.dEnd AND NOW()>tHeat.dStart),1,0),"
			"TIME_FORMAT(TIMEDIFF(tHeat.dStart,NOW()),'%%i:%%s')"
			" FROM tHeat,tStatus,tEvent,tRound"
			" WHERE tHeat.uHeat=%u AND tStatus.uStatus=tHeat.uStatus"
			" AND tHeat.uRound=tRound.uRound"
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
		sprintf(cStatus,"%.31s",field[5]);
		sscanf(field[6],"%u",&uStatus);
		if(field[7][0])
			sprintf(dTimeLeft,"%.31s",field[7]);
		sprintf(scEvent,"%.31s",field[8]);
		sprintf(cRound,"%.31s",field[9]);
		sscanf(field[10],"%u",&uTimeStatus);
		if(field[11][0])
			sprintf(dTimeToStart,"%.31s",field[11]);
	}
	if(!uHeat || !uRound) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">Unexpected error for %s",scEvent);
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	fprintf(fp,"<div class=\"sTableCellGreen\">%s/%s (<a title='Change Rider' href=?gcPage=Judge&ClearRider>X</a>)"
			" (<a title='Toggle Help' href=?gcPage=Judge&ToggleHelp>H</a>)</div>",
					cForeignKey("tRider","cLabel",guRider),cForeignKey("tRider","cTeam",guRider));
	fprintf(fp,"<div class=\"sTableCellBlack\">%s (<a title='Change Event' href=?gcPage=Judge&ClearEvent>X</a>)</div>",scEvent);
	fprintf(fp,"<div class=\"sTableCellBlack\">%s (<a title='Change Round' href=?gcPage=Judge&ClearRound>X</a>)</div>",cRound);
	fprintf(fp,"<div class=\"sTableCellBlack\"><a title='Overlay' href=?gcFunction=Heat&uHeat=%u >HEAT %s</a>",uHeat,cHeat);
	fprintf(fp," (<a title='Change Heat' href=?gcPage=Judge&ClearHeat>X</a>)");
	if(guPermLevel>=10)
		fprintf(fp," (<a title='Close Heat' href=?gcPage=Judge&CloseHeat>C</a>)");
	fprintf(fp,"</div>");
	if(uStatus==1 && uTimeStatus)
	{
		fprintf(fp,"<div class=\"sTableCellGreen\">END IN</div>");
		fprintf(fp,"<div class=\"sTableCellBlackBold\"><p id=\"demo\">%s</p></div>",dTimeLeft);
	}
	else
	{
		if(dTimeLeft[0]=='-')
			sprintf(cStatus,"%.31s","FINISHED");
		else
			sprintf(cStatus,"%.31s","START IN");
		fprintf(fp,"<div class=\"sTableCellGreen\">%s</div>",cStatus);
		if(dTimeLeft[0]=='-')
			fprintf(fp,"<div class=\"sTableCellBlackBold\">%s</div>",dTimeLeft);
		else
			fprintf(fp,"<div class=\"sTableCellBlackBold\">%s</div>",dTimeToStart);
	}
	fprintf(fp,"</div>");

	fprintf(fp,"</div>");


      	fprintf(fp,"<form class=\"form-signin\" role=\"form\" method=\"post\" action=\"/unxsEVApp/\">");

	unsigned i;
	if(guHelp)
	{
		fprintf(fp,"You can score this rider during the heat scoring duration time as much as you like but only the last score for each family will be counted.");
		fprintf(fp,"<br>Score Family ");
	}
	char *cChecked="";
	for(i=0;i<3;i++)
	{
		if(suScoreFamily==i) cChecked="checked ";
		else cChecked="";
		fprintf(fp,"<div class='form-check form-check-inline'>");
		fprintf(fp,"\t<input class='form-check-input' name='ScoreFamily' type='radio' id='radio%u' value='%u' %s>",i,i,cChecked);
		fprintf(fp,"\t<label class='form-check-label' for='radio%u'>&#%u;</label>",i,i+'A');
		fprintf(fp,"</div>");
	}
	fprintf(fp,"<br>");
	if(guHelp)
		fprintf(fp,"Main Score ");
	for(i=1;i<=10;i++)
	{
		if(suMainScore==i) cChecked="checked ";
		else cChecked="";
		fprintf(fp,"<div class='form-check form-check-inline'>");
		fprintf(fp,"\t<input class='form-check-input' name='MainScore' type='radio' id='radio%u' value='%u' %s>",i,i,cChecked);
		fprintf(fp,"\t<label class='form-check-label' for='radio%u'>%u</label>",i,i);
		fprintf(fp,"</div>");
	}
	fprintf(fp,"<br>");
	if(guHelp)
		fprintf(fp,"Fine Score ");
	for(i=0;i<10;i++)
	{
		float fFineScore=(float)i/10.0;
		fprintf(fp,"<div class='form-check form-check-inline'>");
		if(sfFineScore==fFineScore)
			cChecked="checked ";
		else
			cChecked="";

		fprintf(fp,"\t<input class='form-check-input' name='FineScore' type='radio' id='radio%u' value='%0.2f' %s>",
						i,fFineScore,cChecked);
		fprintf(fp,"\t<label class='form-check-label' for='radio%u'>%0.2f</label>",i,fFineScore);
		fprintf(fp,"</div>");
	}


	char *cDisable="disabled";
	if(uStatus==1)
		cDisable="";
        fprintf(fp,"<br><button class='btn btn-lg btn-primary btn-block' type='submit' name=gcFunction value='NewScore'"
			" %s >Save</button>",cDisable);
	fprintf(fp,"</form>");

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT DISTINCT tClient.cLabel,tClient.uClient"
			" FROM tScore,tClient"
			" WHERE tScore.uCreatedBy=tClient.uClient"
			" AND tScore.uHeat=%u AND tScore.uOwner=%u AND tClient.uClient=%u",uHeat,guOrg,guLoginClient);
	else
		sprintf(gcQuery,"SELECT DISTINCT tClient.cLabel,tClient.uClient"
			" FROM tScore,tClient"
			" WHERE tScore.uCreatedBy=tClient.uClient"
			" AND tScore.uHeat=%u AND tScore.uOwner=%u",uHeat,guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	uSelectRider(fp,"Judge",1);
	fprintf(fp,"\n<br><div class=\"sTable\">\n");
	unsigned uClient=0;
	unsigned uPrevRider=0;
	unsigned uRider=0;
	while((field=mysql_fetch_row(res)))
	{
		uPrevRider=0;
		sscanf(field[1],"%u",&uClient);
		fprintf(fp,"<div class=\"sTableRow\">\n");
		fprintf(fp,"\t<div class=\"sTableCell\">%s</div>\n",field[0]);
		sprintf(gcQuery,"SELECT tScore.uIndex,tScore.fScore,tRider.cLast,tRider.uRider"
			" FROM tScore,tRider"
			" WHERE tScore.uHeat=%u AND tScore.uOwner=%u"
			" AND tRider.uRider=tScore.uRider"
			" AND fScore>0.0"
			" AND tScore.uCreatedBy=%u ORDER BY tScore.uRider,tScore.uIndex",uHeat,guOrg,uClient);
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

