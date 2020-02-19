/*
AUTHOR/LEGAL
	(C) 2010-2020 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	For judging heat scores
*/

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
	sprintf(gcQuery,"SELECT tScoreComp.uRider,tScoreComp.uRound FROM tScoreComp"
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
		if(!uRound)
			sscanf(field[1],"%u",&uRound);
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
		sscanf(field[0],"%*s %u",&uRoundNum);
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
		htmlHeader("AdvanceRidersError","Default.Header");
		fprintf(stdout,"Multiple or no uRound>%u",uRound);
		htmlFooter("Default.Footer");
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
	if(mysql_num_rows(res)!=1)
	{
		htmlHeader("AdvanceRidersError","Default.Header");
		fprintf(stdout,"Mult or no WINNER OF R%u(%u) H%u(%u) of next round=%u uRidersRanked[0]=%u",
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
	if(mysql_num_rows(res)!=1)
	{
		htmlHeader("AdvanceRidersError","Default.Header");
		fprintf(stdout,"Mult or No SECOND OF R%u(%u) H%u(%u) of next round=%u uRidersRanked[1]=%u",
				uRoundNum,uRound,uHeatNum,uHeat,uNextRound,uRidersRanked[1]);
		htmlFooter("Default.Footer");
	}
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

}//void AdvanceRidersToNextRound(unsigned uHeat,unsigned uEvent)


