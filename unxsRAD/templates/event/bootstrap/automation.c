/*
AUTHOR/LEGAL
	(C) 2010-2020 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	For judging heat scores
*/

//TOC
char *cParseTextAreaLines(char *cTextArea);
void funcAdmin(FILE *fp);
void PopulateScoreComp(unsigned uHeat);
void AdvanceRidersToNextRound(unsigned uHeat,unsigned uEvent);

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

	//TODO tRound.uHeatSize exists and should be used
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

		fprintf(fp,"<br><br>Automation in alpha...");
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
		unsigned uPrevRoundSeqHeatNumStart=0;
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

				//General rule 1: The next round being created has to
				//provide for 1/2 of the participants 
				//rule 2: The round being created must have the largest number of
				//participants per heat. E.g. better 2 of 4 than 4 of 2.?
				if(uRound>1)
					fprintf(fp,"<br><br>%u>%u",
						(int)fPrevNumHeats*2,(int)fNumHeats*uHeatSizes[uRound-1]);
				if(uRound>1 && ((int)fPrevNumHeats*2)>((int)fNumHeats*uHeatSizes[uRound-1]))
				{
					uHeatSizes[uRound-1]=uHeatSizes[uRound-1]+1;	
				}
				else
				{
					//Flip for case?
					if(uHeatSizes[uRound-1]==3 && fNumHeats==2.0 )
					{
						unsigned uTemp=uHeatSizes[uRound-1];
						uHeatSizes[uRound-1]=fNumHeats;
						fNumHeats=uTemp;
					}
				}

				fprintf(fp,"<br>%sRound %u has %1.0f heats of %u participants each.",
					cRound,uRound,fNumHeats,uHeatSizes[uRound-1]);
				if(floor(fCalc)!=ceil(fCalc))
				{
					fprintf(fp,"<br>&nbsp;&nbsp;&nbsp;One heat has less than %u participants.",uHeatSizes[uRound-1]);
					fprintf(fp,"<br>&nbsp;&nbsp;&nbsp;Was (%2.2f %2.2f)",fParticipants,fCalc);
					fParticipants++;
					fCalc=fParticipants/(float)uHeatSizes[uRound-1];
					fprintf(fp,"<br>&nbsp;&nbsp;&nbsp;Now (%2.2f %2.2f)",fParticipants,fCalc);
				}

			}
			else
			{
				if(uRound==uRounds)
					uHeatSizes[uRound-1]=uHeatSizes[uRound-1]-1;
				else
					uHeatSizes[uRound-1]=uHeatSizes[uRound-1]-2;
				fCalc=fParticipants/(float)(uHeatSizes[uRound-1]);
				fNumHeats=ceil(fCalc);

				fprintf(fp,"<br><br>Adjusted %sround %u has %1.0f heats of %u participants each.",
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
				sprintf(gcQuery,"INSERT INTO tRound SET uEvent=%u,cLabel=LTRIM('%s Round %u')"
						",uNumScores=3,uHeatSize=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
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
				uPrevRoundSeqHeatNumStart=uSeqHeatNum-(int)fPrevNumHeats;
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
						fprintf(fp,"<br>uPrevRoundSeqHeatNumStart=%u uSeqHeatNum=%u uRound=%u"
								"<br> uHeat=j=%d fPrevNumHeats=%1.0f fNumHeats=%1.0f uHeatSizes[]=%u",
							uPrevRoundSeqHeatNumStart,uSeqHeatNum,uRound,
							j,fPrevNumHeats,fNumHeats,uHeatSizes[uRound-1]);

						unsigned uNewRider=0;
						int i41,i42,i43,i44,i31,i32,i33,i21,i22;
						int iTwoXJPlus1=(2*(j+1));
						int iPrevNumHeats=(int)fPrevNumHeats;
						int iThree=1+((iTwoXJPlus1+2) % iPrevNumHeats);
						int iFour=1+((iTwoXJPlus1+3) % iPrevNumHeats);
						if(uHeatSizes[uRound-1]==4)
						{
							fprintf(fp,"<br>WH%u|WH%u|SH%u|SH%u",
							i41=(iTwoXJPlus1-2+uPrevRoundSeqHeatNumStart),
							i42=(iTwoXJPlus1-1+uPrevRoundSeqHeatNumStart),
							i43=(iThree-1+uPrevRoundSeqHeatNumStart),
							i44=(iFour-1+uPrevRoundSeqHeatNumStart));
						}
						else if(uHeatSizes[uRound-1]==2)
						{
							fprintf(fp,"<br>WH%u|SH%u",
							i21=(iThree+uPrevRoundSeqHeatNumStart-1),
							i22=(iFour+uPrevRoundSeqHeatNumStart-1));
						}
						else if(uHeatSizes[uRound-1]==3)
						{
							fprintf(fp,"<br>WH%u|WH%u|WH%u",
							i31=(iTwoXJPlus1-2+uPrevRoundSeqHeatNumStart),
							i32=(iTwoXJPlus1-1+uPrevRoundSeqHeatNumStart),
							i33=(iTwoXJPlus1+uPrevRoundSeqHeatNumStart));
						}

						if(uHeatSizes[uRound-1]==4)
						{
						//1
						sprintf(gcQuery,"INSERT INTO tRider SET"
							" cLabel='WINNER OF R%u H%u',cFirst='WINNER OF',cLast='R%u H%u',"
							" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uImportEvent=%u",
								uRound-1,i41,uRound-1,i41,
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
								uRound-1,i42,uRound-1,i42,
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
						//3
						sprintf(gcQuery,"INSERT INTO tRider SET"
							" cLabel='SECOND OF R%u H%u',cFirst='SECOND OF',cLast='R%u H%u',"
							" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uImportEvent=%u",
								uRound-1,i43,uRound-1,i43,
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
								uRound-1,i44,uRound-1,i44,
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
						}//==4
						else if(uHeatSizes[uRound-1]==2)
						{
						//1 
						sprintf(gcQuery,"INSERT INTO tRider SET"
							" cLabel='WINNER OF R%u H%u',cFirst='WINNER OF',cLast='R%u H%u',"
							" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uImportEvent=%u",
								uRound-1,i21,uRound-1,i21,
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
								uRound-1,i22,uRound-1,i22,
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
						}//==2
						else if(uHeatSizes[uRound-1]==3)
						{
						//1
						sprintf(gcQuery,"INSERT INTO tRider SET"
							" cLabel='WINNER OF R%u H%u',cFirst='WINNER OF',cLast='R%u H%u',"
							" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uImportEvent=%u",
								uRound-1,i31,uRound-1,i31,
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
								uRound-1,i32,uRound-1,i32,
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
						//3
						sprintf(gcQuery,"INSERT INTO tRider SET"
							" cLabel='WINNER OF R%u H%u',cFirst='WINNER OF',cLast='R%u H%u',"
							" uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uImportEvent=%u",
								uRound-1,i33,uRound-1,i33,
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
						}//==3
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


void PopulateScoreComp(unsigned uHeat)
{
        MYSQL_RES *res;
	MYSQL_ROW field;
        MYSQL_RES *res2;
	MYSQL_ROW field2;
        MYSQL_RES *res3;
	MYSQL_ROW field3;
	unsigned uRider=0;
	unsigned uEvent=0;
	unsigned uRound=0;
	unsigned uNumJudges=0;//Varies per number of scores per uIndex
	unsigned uNumScores=0;
	unsigned uScoreComp=0;
	float fScore=0.0;

	if(!uHeat)
	{
		gcMessage="PopulateScoreComp !uHeat";
		return;
	}

	//We need the number of scores so we can setup a loop for each uIndex
	sprintf(gcQuery,"SELECT tRound.uNumScores,tHeat.uEvent,tHeat.uRound"
				" FROM tHeat,tRound WHERE tHeat.uHeat=%u AND tHeat.uRound=tRound.uRound",
				uHeat);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="PopulateScoreComp Error a0";
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNumScores);
		sscanf(field[1],"%u",&uEvent);
		sscanf(field[2],"%u",&uRound);
	}
	if(!uNumScores || uNumScores>8 || !uEvent || !uRound)
	{
		gcMessage="PopulateScoreComp() range error";
		return;
	}

register unsigned i;
for(i=0;i<uNumScores;i++)	
{

	//For each rider per given index
	sprintf(gcQuery,"SELECT uRider FROM tScore WHERE uHeat=%u AND uIndex=%u AND fScore>0.0 GROUP BY uRider",uHeat,i);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="PopulateScoreComp ErrorA";
		return;
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uRider);

		uNumJudges=mysql_num_rows(res);
		sprintf(gcQuery,"SELECT uCreatedBy FROM tScore"
				" WHERE uHeat=%u AND uIndex=%u"
				" AND uRider=%u AND fScore>0.0 GROUP BY uCreatedBy",
					uHeat,i,
						uRider);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			gcMessage="PopulateScoreComp ErrorA";
			return;
		}
		res2=mysql_store_result(&gMysql);
		uNumJudges=mysql_num_rows(res2);
		if(!uNumJudges || uNumJudges>8)
		{
			gcMessage="PopulateScoreComp ErrorB";
			return;
		}

		sprintf(gcQuery,"SELECT (SUM(fScore)/%u)"
			" FROM tScore"
			" WHERE uHeat=%u AND uIndex=%u AND fScore>0.0 AND uRider=%u GROUP BY uIndex",
				uNumJudges,
				uHeat,i,uRider);
		mysql_query(&gMysql,gcQuery);
		if(*mysql_error(&gMysql))
		{
			gcMessage="PopulateScoreComp Error0";
			return;
		}
		res2=mysql_store_result(&gMysql);
		if(mysql_num_rows(res2)<1)
		{
			gcMessage="PopulateScoreComp Error0-1";
			return;
		}
		while((field2=mysql_fetch_row(res2)))
		{
			sscanf(field2[0],"%f",&fScore);

			sprintf(gcQuery,"SELECT uScoreComp FROM tScoreComp WHERE uHeat=%u AND uRider=%u AND uIndex=%u",
				uHeat,uRider,i);
			mysql_query(&gMysql,gcQuery);
			if(*mysql_error(&gMysql))
			{
				gcMessage="PopulateScoreComp Error1";
				return;
			}
			res3=mysql_store_result(&gMysql);
			if((field3=mysql_fetch_row(res3)))
			{
				sscanf(field3[0],"%u",&uScoreComp);
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
					fScore,i,uHeat,uRider,guLoginClient,guOrg,uRound,uEvent);
				mysql_query(&gMysql,gcQuery);
				if(*mysql_error(&gMysql))
				{
					gcMessage="PopulateScoreComp Error3";
					return;
				}
			}
			mysql_free_result(res3);
		}
		mysql_free_result(res2);
	}
	mysql_free_result(res);
}//for i each uIndex

}//void PopulateScoreComp(unsigned uHeat)


void AdvanceRidersToNextRound(unsigned uHeat,unsigned uEvent)
{
	char cBuffer[256]={""};
	char cRoundNum[32]={""};
	char cHeatNum[32]={""};
	register int i=0;
	unsigned uRidersRanked[2]={0,0};
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uRound=0;
	unsigned uRoundNum=0;
	unsigned uHeatNum=0;
	unsigned uNextRound=0;
	unsigned uScore=0;
	//Find tScore.uScore for uNextRound and guEvent for either
	//	cLabel="WINNER OF R(uRoundSeq) H(uHeatSeq)"
	//	cLabel="SECOND OF R(uRoundSeq) H(uHeatSeq)"
	//Update tScore setting uRider=uRiderWinner for tScore.uScore

		
	//Rank riders: WINNER and SECOND get uRiderWinner and uRiderSecond (tRider.uRider)
	//Get the seq round number 1, 2, 3.... etc
	sprintf(gcQuery,"SELECT tScoreComp.uRider FROM tScoreComp"
			" WHERE tScoreComp.uHeat=%u"
			" GROUP BY tScoreComp.uRider ORDER BY SUM(tScoreComp.fScore) DESC LIMIT 2",uHeat);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("AdvanceRiders","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(i>1) break;
		sscanf(field[0],"%u",&uRidersRanked[i++]);
	}
	//Get uRound 
	sprintf(gcQuery,"SELECT uRound FROM tScoreComp"
			" WHERE uHeat=%u",uHeat);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("AdvanceRiders","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uRound);
	}

	//Get uRoundNum the seq heat number 1,2,3...etc
	sprintf(gcQuery,"SELECT cLabel FROM tRound"
			" WHERE uRound=%u",uRound);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("AdvanceRiders","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cRoundNum,"%.31s",field[0]);
		sscanf(field[0],"%*[A-z ] %u",&uRoundNum);
		if(!uRoundNum)
		{
			sprintf(cBuffer,"%.255s",field[0]);
			htmlHeader("AdvanceRiders","Default.Header");
			fprintf(stdout,"uRoundNum==0 %s",cBuffer);
			htmlFooter("Default.Footer");
		}
	}
	//Get uHeatNum the seq heat number 1,2,3...etc
	sprintf(gcQuery,"SELECT cLabel FROM tHeat"
			" WHERE uHeat=%u",uHeat);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("AdvanceRiders","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cHeatNum,"%.31s",field[0]);
		sscanf(field[0],"%u",&uHeatNum);
	}

	//debug
	//htmlHeader("AdvanceRidersError","Default.Header");
	//printf("uRound=%u cRoundNum=%s RoundNum:%u cHeatNum=%s HeatNum:%u",
	//	uRound,cRoundNum,uRoundNum,cHeatNum,uHeatNum);
	//htmlFooter("Default.Footer");

	//Get uNextRound
	sprintf(gcQuery,"SELECT uRound FROM tRound WHERE uEvent=%u AND uRound>%u LIMIT 1",uEvent,uRound);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("AdvanceRidersError","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)!=1)
	{
		//Allow return for final
		return;
/*
		htmlHeader("AdvanceRidersError","Default.Header");
		fprintf(stdout,"Multiple or no uRound>%u",uRound);
		htmlFooter("Default.Footer");
*/
	}
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uNextRound);

	//Find tScore.uScore for uNextRound and guEvent for WINNER
	sprintf(gcQuery,"SELECT uScore FROM tScore WHERE uRound=%u AND cLabel='WINNER OF R%u H%u'",
				uNextRound,uRoundNum,uHeatNum);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("AdvanceRidersError","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)<1)
	{
		htmlHeader("AdvanceRidersError","Default.Header");
		fprintf(stdout,"No WINNER OF R%u(%u) H%u(%u) of next round=%u uRidersRanked[0]=%u",
				uRoundNum,uRound,uHeatNum,uHeat,uNextRound,uRidersRanked[0]);
		htmlFooter("Default.Footer");
	}
	if(mysql_num_rows(res)>1)
	{
		htmlHeader("AdvanceRidersError","Default.Header");
		fprintf(stdout,"Multiple WINNERS OF R%u(%u) H%u(%u) of next round=%u uRidersRanked[0]=%u",
				uRoundNum,uRound,uHeatNum,uHeat,uNextRound,uRidersRanked[0]);
		htmlFooter("Default.Footer");
	}
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uScore);
		sprintf(gcQuery,"UPDATE tScore SET uRider=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uScore=%u",
						uRidersRanked[0],guLoginClient,uScore);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
			htmlHeader("AdvanceRidersError","Default.Header");
			fprintf(stdout,"%s",cBuffer);
			htmlFooter("Default.Footer");
		}
	}

	//Find tScore.uScore for uNextRound and guEvent  for SECOND
	sprintf(gcQuery,"SELECT uScore FROM tScore WHERE uRound=%u AND cLabel='SECOND OF R%u H%u'",
				uNextRound,uRoundNum,uHeatNum);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
		htmlHeader("AdvanceRidersError","Default.Header");
		fprintf(stdout,"%s",cBuffer);
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	//Some finals only take winners from previous heats of only 2 riders.
/*
	if(mysql_num_rows(res)<1)
	{
		htmlHeader("AdvanceRidersError","Default.Header");
		fprintf(stdout,"No SECOND OF R%u(%u) H%u(%u) of next round=%u uRidersRanked[1]=%u",
				uRoundNum,uRound,uHeatNum,uHeat,uNextRound,uRidersRanked[1]);
		htmlFooter("Default.Footer");
	}
	if(mysql_num_rows(res)>1)
	{
		htmlHeader("AdvanceRidersError","Default.Header");
		fprintf(stdout,"Mult SECOND OF R%u(%u) H%u(%u) of next round=%u uRidersRanked[1]=%u",
				uRoundNum,uRound,uHeatNum,uHeat,uNextRound,uRidersRanked[1]);
		htmlFooter("Default.Footer");
	}
*/
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uScore);
		sprintf(gcQuery,"UPDATE tScore SET uRider=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uScore=%u",
						uRidersRanked[1],guLoginClient,uScore);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			sprintf(cBuffer,"%.255s",mysql_error(&gMysql));
			htmlHeader("AdvanceRidersError","Default.Header");
			fprintf(stdout,"%s",cBuffer);
			htmlFooter("Default.Footer");
		}
	}
	mysql_free_result(res);

}//void AdvanceRidersToNextRound(unsigned uHeat,unsigned uEvent)


void funcNewJudge(FILE *fp)
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
			"IF((NOW()<tHeat.dEnd AND NOW()>tHeat.dStart),1,0)"
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
	}
	if(!uHeat || !uRound) 
	{
		fprintf(fp,"<div class=\"sTableCellYellow\">Unexpected error for %s",scEvent);
		fprintf(fp,"</div>");
		fprintf(fp,"</div>");
		return;
	}

	fprintf(fp,"<div class=\"sTableCellGreen\">%s (<a title='Change Rider' href=?gcPage=Judge&ClearRider>X</a>)"
			" (<a title='Toggle Help' href=?gcPage=Judge&ToggleHelp>H</a>)</div>",
					cForeignKey("tRider","cLabel",guRider));
	fprintf(fp,"<div class=\"sTableCellBlack\">%s (<a title='Change Event' href=?gcPage=Judge&ClearEvent>X</a>)</div>",scEvent);
	fprintf(fp,"<div class=\"sTableCellBlack\">%s (<a title='Change Round' href=?gcPage=Judge&ClearRound>X</a>)</div>",cRound);
	fprintf(fp,"<div class=\"sTableCellBlack\"><a title='Overlay' href=?gcFunction=Heat&uHeat=%u >HEAT %s</a>",uHeat,cHeat);
	fprintf(fp," (<a title='Change Heat' href=?gcPage=Judge&ClearHeat>X</a>)");
	if(guPermLevel>=10)
		fprintf(fp," (<a title='Close Heat' href=?gcPage=Judge&CloseHeat>C</a>)");
	fprintf(fp,"</div>");
	if(uStatus==1 && uTimeStatus)
	{
		fprintf(fp,"<div class=\"sTableCellGreen\">ENDING</div>");
		fprintf(fp,"<div class=\"sTableCellBlackBold\"><p id=\"demo\">%s</p></div>",dTimeLeft);
	}
	else
	{
		if(dTimeLeft[0]=='-')
			sprintf(cStatus,"%.31s","FINISHED");
		else
			sprintf(cStatus,"%.31s","STARTING");
		fprintf(fp,"<div class=\"sTableCellGreen\">%s</div>",cStatus);
		if(dTimeLeft[0]=='-')
			fprintf(fp,"<div class=\"sTableCellBlackBold\">%s</div>",dTimeLeft);
		else
			fprintf(fp,"<div class=\"sTableCellBlackBold\"><p id=\"demo\">%s</p></div>",dTimeLeft);
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



}//void funcNewJudge(FILE *fp)
