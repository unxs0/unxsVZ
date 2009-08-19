/*
FILE
	$Id$
PURPOSE
	Non-schema dependent tresource.c expansion.
AUTHOR
	GPL License applies, see www.fsf.org for details
	See LICENSE file in this distribution
	(C) 2001-2009 Gary Wallis for Unixservice.
TODO

*/

//File scope vars
static char cSearch[100]={""};
static char cuNSSetPullDown[256]={""};
static unsigned uSelectNSSet=0;
static unsigned uCIDR=0;
static unsigned uStartBlock=0;
static unsigned uZoneOwner=0;

//Aux drop/pull downs
void CustomerDropDown(unsigned uSelector);//tzonefunc.h
static char cCustomerDropDown[256]={""};
static unsigned uClient=0;

//Called from tzonefunc.h
void tResourceTableAddRR(unsigned uZoneId);

static char cZone[255]={""};
static unsigned uAddArpaPTR=0;

int AutoAddPTRResource(const unsigned d,const char *cDomain,const unsigned uInZone,const unsigned uSourceZoneOwner);

unsigned IsSecondaryOnlyZone(unsigned uZone);

//Extern
int PopulateArpaZone(char *cZone, char *cIPNum, unsigned uHtmlMode,
					unsigned uFromZone, unsigned uZoneOwner);
void UpdateSerialNum(unsigned uZone);//tzonefunc.h

/*
void DebugOnly(const char *cLabel1, const char *cValue1, const char *cLabel2, const char *cValue2)
{
	printf("Content-type: text/plain\n\n");
	printf("%s=(%s) %s=(%s)\n",cLabel1,cValue1,cLabel2,cValue2);
	exit(0);
}//DebugOnly()
*/


void ExtProcesstResourceVars(pentry entries[], int x)
{

	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uAddArpaPTR"))
			uAddArpaPTR=1;	
		else if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"cuNSSetPullDown"))
		{
			sprintf(cuNSSetPullDown,"%.255s",entries[i].val);
			uSelectNSSet=ReadPullDown("tNSSet","cLabel",cuNSSetPullDown);
		}
		else if(!strcmp(entries[i].name,"uCIDR"))
			sscanf(entries[i].val,"%u",&uCIDR);
		else if(!strcmp(entries[i].name,"uStartBlock"))
		{
			sscanf(entries[i].val,"%u",&uStartBlock);
			if(uStartBlock>249) uStartBlock=249;
			else if(uStartBlock<0) uStartBlock=0;
		}
		else if(!strcmp(entries[i].name,"cCustomerDropDown"))
		{
			strcpy(cCustomerDropDown,entries[i].val);
			uClient=ReadPullDown("tClient","cLabel",
					cCustomerDropDown);
		}
	}

}//void ExtProcesstResourceVars(pentry entries[], int x)


void RRCheck(int uMode)
{
	char cRRType[100]={""};
	unsigned a=0,b=0,c=0,d=0;
			
	strcpy(cRRType,ForeignKey("tRRType","cLabel",uRRType));
	strcpy(cZone,ForeignKey("tZone","cZone",uZone));
	
	//For all TODO
	if(!cName[0] && strcmp(cRRType,"NS"))
	{
		guMode=uMode;
		tResource("cName is required");
	}


	//TODO
	if(strchr(cName,'.'))
	{
		sprintf(gcQuery,"%s.",cZone);
		if(strcmp(gcQuery,cName))
		{
			sprintf(gcQuery,".%s.",cZone);
			if(!strstr(cName+(strlen(cName)-strlen(gcQuery)),gcQuery))
			{
				guMode=uMode;
				tResource("If cName is fully qualified it must end with cZone and final period.");
			}
		}
	}

	//TODO
	//Make sure only one cName exists with same value per zone...

		
	if(!strcmp(cRRType,"CNAME"))
	{
		//TODO get docs on valid use and default helper use of this RR
		//(cName) LHS should have no dot '.' or end with zone name. This is done for all
		//zone RRs.
		//(cParam1) RHS can be any FQDN. If with trailing dot ok. If not add zone with dot.
		//All cases
		//cParam2 not used. Erased.
		cParam2[0]=0;

		guMode=uMode;
		if(!cParam1[0])
		{
				sprintf(cParam1,"%s.",cZone);
				tResource("cParam1 is required. Common default entry made for you, check/change.");
		}
		else
		{
			char cParam1Save[101]={""};
			char cParam1Temp[101]={""};

			sprintf(cParam1Save,"%.100s",cParam1);
			//converts, eliminates illegal chars.
			//helpers try to format for you...lol
			FQDomainName(cParam1);

			if(strchr(cParam1,'.'))
			{
				//FQDN
				//Missing trailing dot
				if(cParam1[strlen(cParam1)-1]!='.')
				{
					sprintf(cParam1Temp,"%.100s.",cParam1);
					strcpy(cParam1,cParam1Temp);
				}
				if(strcmp(cParam1,cParam1Save))
					tResource("cParam1 was changed check/fix");
			}
			else
			{
				//Not FQDN, but internal zone member or another zone?

				//TODO
				//Advanced help not implemented yet.
				//Check in this zone for A record. Should not be another CNAME.
				//Check in tZone

				if(cParam1[strlen(cParam1)-1]!='.')
				{
					sprintf(cParam1Temp,"%.49s.%.49s.",cParam1,cZone);
					strcpy(cParam1,cParam1Temp);
				}
				if(strcmp(cParam1,cParam1Save))
					tResource("cParam1 was changed check/fix");
			}

		}
	}
	else if(!strcmp(cRRType,"A"))
	{

		if(!strcmp(cZone+strlen(cZone)-5,".arpa"))
			tResource("Can not add A records to arpa zones");
		sscanf(cParam1,"%u.%u.%u.%u",&a,&b,&c,&d);
		if(a>254) a=0;
		if(b>254) b=0;
		if(c>254) c=0;  
		if(d>254) d=0;  

		sprintf(cParam1,"%u.%u.%u.%u",a,b,c,d);

		cParam2[0]=0;
		if(!a || !d)
		{
			guMode=uMode;
			tResource("Invalid IP Number for cParam1");
		}
	}
	else if(!strcmp(cRRType,"PTR"))
	{
		cParam2[0]=0;
		FQDomainName(cParam1);
		if(!cParam1[0])
		{
			guMode=uMode;
			tResource("cParam1 is required");
		}
		if(cParam1[strlen(cParam1)-1]!='.') strcat(cParam1,".");
	}
	else if(!strcmp(cRRType,"MX"))
	{
		if(!strcmp(cZone+strlen(cZone)-5,".arpa"))
			tResource("Can not add MX records to arpa zones");
		sscanf(cParam1,"%u",&a);
		sprintf(cParam1,"%u",a);
		if(!a || a>1000)
		{
			guMode=uMode;
			tResource("Invalid MX Priority Number for cParam1");
		}
		FQDomainName(cParam2);
		if(!cParam2[0])
		{
			guMode=uMode;
			tResource("cParam2 is required");
		}
		if(cParam2[strlen(cParam2)-1]!='.') strcat(cParam2,".");
		
	}
	else if(!strcmp(cRRType,"NS"))
	{
		//All cases
		cParam2[0]=0;
		if(!cParam1[0])
		{
			guMode=uMode;
			tResource("cParam1 is required");
		}
		FQDomainName(cParam1);
		if(cParam1[strlen(cParam1)-1]!='.') strcat(cParam1,".");

		//Allow subdomains
		//if(strcmp(cZone+strlen(cZone)-5,".arpa"))
		//{
		//	sprintf(cName,"%s.",cZone);
		//}
		//else no other rules for arpa zone for now TODO
	}
	else if(!strcmp(cRRType,"HINFO"))
	{
		if(!cParam1[0])
		{
			guMode=uMode;
			tResource("cParam1 is required");
		}
		if(cParam1[0]!='"' && cParam1[strlen(cParam1)-1]!='"')
		{
			sprintf(gcQuery,"\"%s\"",cParam1);
			strcpy(cParam1,gcQuery);
		}

		if(!cParam2[0])
		{
			guMode=uMode;
			tResource("cParam2 is required");
		}
		if(cParam2[0]!='"' && cParam1[strlen(cParam2)-1]!='"')
		{
			sprintf(gcQuery,"\"%s\"",cParam2);
			strcpy(cParam2,gcQuery);
		}
	}
	else if(!strcmp(cRRType,"TXT"))
	{
		cParam2[0]=0;
		if(!cParam1[0])
		{
			guMode=uMode;
			tResource("cParam1 is required");
		}
		if(cParam1[0]!='"' && cParam1[strlen(cParam1)-1]!='"')
		{
			sprintf(gcQuery,"\"%s\"",cParam1);
			strcpy(cParam1,gcQuery);
		}

	}
	else if(1)
	{
		guMode=uMode;
		tResource("Must select valid uRRType");
	}

}//void RRCheck(int uMode)


void ExttResourceCommands(pentry entries[], int x)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uNSSet;
	time_t clock;

	if(!strcmp(gcFunction,"tResourceTools"))
	{
		//ModuleFunctionProcess()

		//Default wizard like two step creation and deletion
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			ProcesstResourceVars(entries,x);
			sscanf(ForeignKey("tZone","uOwner",uZone),"%u",
					&uZoneOwner);
			if(guPermLevel<10)
			{
				if(uZoneOwner) GetClientOwner(uZoneOwner,&guReseller);
			}
			if( (guPermLevel>=7 && uOwner==uZoneOwner)
				|| (guPermLevel>7 && guReseller==guLoginClient)
				|| (guPermLevel>9) )
			{
				strcpy(cZone,ForeignKey("tZone","cZone",uZone));

				if(uZone==0)
					tResource("Must start from valid zone");
				uOwner=uZoneOwner;
				uCreatedBy=guLoginClient;

				//Check global conditions for new record here
                        	guMode=2000;
                        	tResource(LANG_NB_CONFIRMNEW);
			}
			else
				tResource("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			ProcesstResourceVars(entries,x);
			sscanf(ForeignKey("tZone","uOwner",uZone),"%u",
					&uZoneOwner);
			if(guPermLevel<10)
				if(uZoneOwner) GetClientOwner(uZoneOwner,&guReseller);

			//Check this out
			if( (guPermLevel>=7 && guLoginClient==uZoneOwner)
				|| (guPermLevel>7 && guReseller==guLoginClient)
				|| (guPermLevel>9 ) )
			{

				//char query[256];
				unsigned uSaveLoginClient=guLoginClient;

				RRCheck(2000);

				//New PTR record in automated arpa zones
				//exception allowed
				if(!uZoneOwner && uRRType!=7)
				{
					guMode=0;
					tResource("uZoneOwner==0 contact admin asap");
				}

				uResource=0;
				uCreatedBy=guLoginClient;
				uOwner=uZoneOwner;
				uModBy=0;//Never modified
				
				if(uAddArpaPTR && uRRType==1)
				{
					char cFQDN[512];
					char cNameSave[256];

					sprintf(cNameSave,"%.255s",cName);
				
					if(cName[strlen(cName)-1]!='.')
					{
						if(strcmp(cName,"@"))
							sprintf(cFQDN,"%.255s.%.255s.",cName,cZone);
						else
							sprintf(cFQDN,"%.511s.",cZone);
					}
					else
						sprintf(cFQDN,"%.511s",cName);

					guLoginClient=uZoneOwner;//For tJob coolness
					PopulateArpaZone(cFQDN,cParam1,1,uZone,uZoneOwner);
					sprintf(cName,"%.255s",cNameSave);
					guLoginClient=uSaveLoginClient;
				}

				if(strstr(cZone,"in-addr.arpa") && uClient)
					uOwner=uClient;

				//tZone needs to be updated
				sscanf(ForeignKey("tZone","uNSSet",uZone),"%u",
					&uNSSet);
				time(&clock);
				//cZone set in RRCheck
				guLoginClient=uZoneOwner;//For tJob coolness
				if(SubmitJob("Modify",uNSSet,cZone,0,clock))
					tResource(mysql_error(&gMysql));
				guLoginClient=uSaveLoginClient;
				UpdateSerialNum(uZone);
                        	guMode=5;
				NewtResource(0);
			}
			else
				tResource("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstResourceVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
                        	guMode=2001;
	                        tResource(LANG_NB_CONFIRMDEL);
			}
			else
				tResource("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstResourceVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
                        	guMode=5;
				//tZone needs to be updated
				sscanf(ForeignKey("tZone","uNSSet",uZone),"%u",
					&uNSSet);
				time(&clock);
				if(SubmitJob("Modify",
					uNSSet,ForeignKey("tZone","cZone",uZone),0,clock))
				tResource(mysql_error(&gMysql));
				UpdateSerialNum(uZone);                      	
				sprintf(gcQuery,"INSERT INTO tDeletedResource SET uDeletedResource='%u',uZone='%u',"
						"cName='%s',uTTL='%u',uRRType='%u',cParam1='%s',cParam2='%s',cParam3='%s',cParam4='%s',"
						"cComment='%s',uOwner='%u',uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uResource,
						uZone,
						cName,
						uTTL,
						uRRType,
						cParam1,
						cParam2,
						cParam3,
						cParam4,
						cComment,
						uOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				
				DeletetResource();
			}
			else
				tResource("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstResourceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
                        	tResource(LANG_NB_CONFIRMMOD);
			}
			else
				tResource("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstResourceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{

				//Check entries here
				RRCheck(2002);
				uModBy=guLoginClient;
				if(uAddArpaPTR && uRRType==1)
				{
					char cFQDN[512];
					char cParam1Save[256];
					char cNameSave[256];
					unsigned uResourceSave=uResource;

					sprintf(cParam1Save,"%.255s",cParam1);
					sprintf(cNameSave,"%.255s",cName);
				
					if(cName[strlen(cName)-1]!='.')
					{
						if(strcmp(cName,"@"))
							sprintf(cFQDN,"%.255s.%.255s.",cName,cZone);
						else
							sprintf(cFQDN,"%.511s.",cZone);
					}
					else
						sprintf(cFQDN,"%.511s",cName);


					sscanf(ForeignKey("tZone","uOwner",uZone),"%u",
							&uZoneOwner);
					PopulateArpaZone(cFQDN,cParam1,1,uZone,uZoneOwner);
					sprintf(cParam1,"%.255s",cParam1Save);
					sprintf(cName,"%.255s",cNameSave);
					uResource=uResourceSave;
				}

				if(strstr(cZone,"in-addr.arpa") && uClient && uResource)
				{
					sprintf(gcQuery,"UPDATE tResource SET uOwner=%u WHERE uResource=%u",
							uClient,uResource);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				}

				//tZone needs to be updated
				sscanf(ForeignKey("tZone","uNSSet",uZone),"%u",
					&uNSSet);
				time(&clock);
				if(SubmitJob("Modify",uNSSet,cZone,0,clock))
					tResource(mysql_error(&gMysql));
				UpdateSerialNum(uZone);
                        	guMode=5;
                        	ModtResource();
			}
			else
				tResource("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,"Delegation Wizard"))
                {
                        ProcesstResourceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=3001;
			}
			else
				tResource("<blink>Error</blink>: Denied by permissions settings");
		}
                else if(!strcmp(gcCommand,"Create Delegation"))
                {
                        ProcesstResourceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				register unsigned i;
				unsigned uNumIPs=0;
				char cParam1[16]={""};
				char cNSSet[4][100]={"ns1.clientisp.net","ns2.clientisp.net","",""};
				unsigned uNumNSSets=2;

				//TODO
				guMode=3001;
				//Check uZone,uCIDR and uSelectNSSet
				if(!uZone)
					tResource("Undefined uZone!");
				if(uCIDR>29 || uCIDR<24)
					tResource("CIDR must be in range 24 to 29 (256 to 8 IPs)");
				if(!uSelectNSSet)
					tResource("Must select a valid tNSSet.cLabel from select");
				sprintf(gcQuery,"SELECT tNS.cFQDN FROM tNSSet,tNS WHERE tNSSet.uNSSet=tNS.uNSSet AND"
						" tNSSet.uNSSet=%u",uSelectNSSet);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql)) tResource(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if(!(field=mysql_fetch_row(res)))
					tResource("No NS found for given uNSSet!");

				uNumIPs=2<<(32-uCIDR-1);
				//jic
				if(uNumIPs>256 || uNumIPs<8)
					tResource("uNumIPs must be in range 8 to 256");
				if((uNumIPs+uStartBlock-1)>255)
					tResource("Start/CIDR goes past last octet 255."
							" Please correct CIDR and/or uStartBlock.");

				//Special case 0/24 (Class C)
				if(uNumIPs==256)
				{
					//TODO must fix tzonefunc also...
					strcpy(cZone,ForeignKey("tZone","cZone",uZone));

					for(i=0;i<uNumNSSets;i++)
					{
						sprintf(gcQuery,"INSERT INTO tResource SET uZone=%u,cName='%.99s',"
								"uRRType=2,cParam1='%.99s.',"
								"cComment='DelegationWizard created',uOwner=%u,"
								"uCreatedDate=UNIX_TIMESTAMP(NOW())",
								uZone
								,cZone
								,cNSSet[i]
								,guLoginClient);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							tResource(mysql_error(&gMysql));

					}
					guMode=0;
					sprintf(gcQuery,"Delegation done for tNSSet set: %s and direct"
							" classC delegation</font>",
								ForeignKey("tNSSet","cLabel",uSelectNSSet));
					tResource(gcQuery);
				}

				//Loop for each cList line: Add the NS records.
				for(i=0;i<uNumNSSets;i++)
				{
					sprintf(cParam1,"%u-%u",uStartBlock,uNumIPs-1);
					sprintf(gcQuery,"INSERT INTO tResource SET uZone=%u,cName='%.15s',"
							"uRRType=2,cParam1='%.99s.'"
							",cComment='DelegationWizard created',uOwner=%u,"
							"uCreatedDate=UNIX_TIMESTAMP(NOW())",
							uZone
							,cParam1
							,cNSSet[i]
							,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						tResource(mysql_error(&gMysql));
				}
				

				//Loop for all CNAME records
				for(i=0;i<uNumIPs;i++)
				{
					sprintf(cParam1,"%u.%u-%u",
						i+uStartBlock,uStartBlock,uNumIPs-1);
					sprintf(gcQuery,"INSERT INTO tResource SET uZone=%u,cName='%u',uRRType=5,"
							"cParam1='%s',cComment='DelegationWizard created',uOwner=%u,"
							"uCreatedDate=UNIX_TIMESTAMP(NOW())",
							uZone
							,uStartBlock+i
							,cParam1
							,guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						tResource(mysql_error(&gMysql));
				}

				guMode=0;
				sprintf(gcQuery,"Delegation done for tNSSet set: %s and for %u IPs. CIDR %u/%u",
						ForeignKey("tNSSet","cLabel",uSelectNSSet)
						,uNumIPs
						,uStartBlock
						,uCIDR);
				tResource(gcQuery);
			}
			else
				tResource("<blink>Error</blink>: Denied by permissions settings");
		}
                else if(!strcmp(gcCommand,"Delete Delegation"))
                {
                        ProcesstResourceVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				sprintf(gcQuery,"DELETE FROM tResource WHERE cComment LIKE 'DelegationWizard%%'"
						" AND uZone=%u",uZone);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					tResource(mysql_error(&gMysql));
				tResource("Delete delegation done");
			}
			else
				tResource("<blink>Error</blink>: Denied by permissions settings");
		}
                else if(!strcmp(gcCommand,"Delete Records"))
                {
                        ProcesstResourceVars(entries,x);
			if(guPermLevel>=12 && guLoginClient==1) //Root only operation
			{
				sprintf(gcQuery,"DELETE FROM tResource WHERE uZone=%u",uZone);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					tResource(mysql_error(&gMysql));
				tResource("Delete records done");
			}
			else
				tResource("<blink>Error</blink>: Denied by permissions settings");
		}
	}

}//void ExttResourceCommands(pentry entries[], int x)


void ResourceLinks(unsigned uZone)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uArpa=0;

	if(!strcmp(cZone+strlen(cZone)-5,".arpa"))
	{
		sprintf(gcQuery,"SELECT cName,uResource,cParam1 FROM tResource WHERE uZone=%u ORDER BY ABS(cName)",uZone);
		uArpa=1;
	}
	else
	{
	sprintf(gcQuery,"SELECT tResource.cName,tResource.uResource,tRRType.cLabel FROM tResource,tRRType WHERE "
			"tResource.uZone=%u AND tResource.uRRType=tRRType.uRRType ORDER BY tResource.cName"
				,uZone);
	}
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) tResource(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		if(uArpa)
			printf("<a class=darkLink href=iDNS.cgi?gcFunction=tResource&uResource=%s&cZone=%s>%s %s</a><br>\n",
				field[1],cZone,field[0],field[2]);
		else
			printf("<a class=darkLink href=iDNS.cgi?gcFunction=tResource&uResource=%s>%s %s</a><br>\n",
				field[1],field[0],field[2]);
	}

}//void ResourceLinks(unsigned uZone)


void ExttResourceButtons(void)
{
	unsigned uDefault=0;

	OpenFieldSet("tResource Aux Panel",100);

	strcpy(cZone,ForeignKey("tZone","cZone",uZone));

	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter required data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
			if(strcmp(cZone+strlen(cZone)-5,".arpa"))
			{
				printf("<br><input title='For some RR types this will add a PTR entry to"
					" the correct .arpa zone' type=checkbox name=uAddArpaPTR> RevDNS");
			}
			else if(!strcmp(cZone+strlen(cZone)-5,".arpa"))
			{
				if(guPermLevel>9)
				{
					printf("<p><u>Create for customer</u><br>");
					CustomerDropDown(uClient);
				}
			}
                break;

                case 2001:
			printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
			if(guPermLevel>9)
			printf("<p><input title='Will remove all records for this zone' "
				"class=lwarnButton type=submit name=gcCommand value='Delete Records'>");
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
			if(uRRType==1 && strcmp(cZone+strlen(cZone)-5,".arpa"))
			{
				printf("<br><input title='For some RR types this will add a PTR entry to"
					" the correct .arpa zone' type=checkbox name=uAddArpaPTR> RevDNS");
			}
			else if(!strcmp(cZone+strlen(cZone)-5,".arpa"))
			{
				if(guPermLevel>9)
				{
					printf("<p><u>Change uOwner for customer</u><br>");
					CustomerDropDown(uClient);
				}
				printf("<p><input title='Will add NS records and the CIDR defined number of"
					" CNAME records in n.0-(2^(32-CIDR))-1 "
					"style for supplied NS previously defined in tNSSet to this zone' class=largeButton "
					"type=submit name=gcCommand value='Delegation Wizard'>");
			}

                break;

		case 3001:
		if(guPermLevel>9)
		{
			printf("<p><u>Delegation Wizard</u><br>");
			tTablePullDown("tNSSet;cuNSSetPullDown","cLabel","cLabel",
				uSelectNSSet,1);
			printf("<br><input title='Enter CIDR block start number (uStartBlock.) "
				"Acceptable range 0-248 depends on CIDR below' type=text "
				"name=uStartBlock size=8 maxlength=3 value=0>");
			printf("<br><input title='Enter CIDR number (uCIDR.) Ex. 26 for 64 IP block. "
				"Range 24-29' type=text name=uCIDR size=8 maxlength=2>");
			printf("<br><input title='Will add NS records -as per select set- and the CIDR "
				"defined number of CNAME records in n.0-(2^(32-CIDR))-1 style to this zone' "
				"class=largeButton type=submit name=gcCommand value='Create Delegation'>");
			printf("<br><input title='Will remove all records that say DelegationWizard in the "
				"cComment field for this zone' class=lwarnButton type=submit name=gcCommand"
				" value='Delete Delegation'>");
		}
		break;

		default:
			uDefault=1;
			printf("<u>Table Tips</u><br>");
			printf("Here we gather all zone resource records, usually organized by a single given uZone. "
				"Quite a bit of input and context input checking takes place, but you probably should "
				"know DNS/BIND configuration. A little knowledge will help you use this backend quickly"
				" and correctly.\n");
			printf("<p><u>Search Tools</u><br>");
			printf("<input type=text title=\"cName search input. Use %% . and _ for power searching.\" "
				"name=cSearch value=\"%s\" maxlength=99 size=20>",cSearch);
			if(uZone)
			{
				printf("<p><u>ZoneNavList</u><br>");
				printf("<a class=darkLink href=iDNS.cgi?gcFunction=tZone&uZone=%u&cZone=%s>",uZone,cZone);
				if(cZone[0])
					printf("%s</a>",cZone);
				else
					printf("Back to Zone</a>");
				printf("<p><u>RRNavList</u><br>");
				ResourceLinks(uZone);
			}


	}

	if(!uDefault && cSearch[0])
		printf("<input type=hidden name=cSearch value=\"%s\">",cSearch);

	CloseFieldSet();	

}//void ExttResourceButtons(void)


void ExttResourceAuxTable(void)
{

}//void ExttResourceAuxTable(void)


void ExttResourceGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uResource"))
		{
			sscanf(gentries[i].val,"%u",&uResource);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cZone"))
		{
			sprintf(cZone,"%.99s",gentries[i].val);
		}
	}
	tResource("");

}//void ExttResourceGetHook(entry gentries[], int x)


void ExttResourceSelect(void)
{

	if(uZone)
	{
		char cExtra[33]={""};

		sprintf(cExtra,"uZone=%u",uZone);

		if(cSearch[0])
			ExtSelectSearch("tResource",VAR_LIST_tResource,"cName",cSearch,cExtra,0);
		else
			ExtSelectSearch("tResource",VAR_LIST_tResource,NULL,NULL,cExtra,0);
	}
	else
	{
		if(cSearch[0])
			ExtSelectSearch("tResource",VAR_LIST_tResource,"cName",cSearch,NULL,0);
		else
			ExtSelect("tResource",VAR_LIST_tResource,0);
	}

}//void ExttResourceSelect(void)


void ExttResourceSelectRow(void)
{
	ExtSelectRow("tResource",VAR_LIST_tResource,uResource);

}//void ExttResourceSelectRow(void)


void ExttResourceListSelect(void)
{
	char cCat[512];

	ExtListSelect("tResource",VAR_LIST_tResource);

	//Changes here must be reflected below in ExttResourceListFilter()
        if(!strcmp(gcFilter,"uResource"))
        {
                sscanf(gcCommand,"%u",&uResource);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tResource.uResource=%u \
						ORDER BY uResource",
						uResource);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cName"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tResource.cName LIKE '%s%%' ORDER BY tResource.cName",
				TextAreaSave(gcCommand));
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cParam1"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tResource.cParam1 LIKE '%s%%' ORDER BY tResource.cParam1",
				TextAreaSave(gcCommand));
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uResource");
        }

}//void ExttResourceListSelect(void)


void ExttResourceListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uResource"))
                printf("<option>uResource</option>");
        else
                printf("<option selected>uResource</option>");
        if(strcmp(gcFilter,"cName"))
                printf("<option>cName</option>");
        else
                printf("<option selected>cName</option>");
        if(strcmp(gcFilter,"cParam1"))
                printf("<option>cParam1</option>");
        else
                printf("<option selected>cParam1</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttResourceListFilter(void)


void ExttResourceNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=10 && !guListMode)
		printf(LANG_NBB_NEW);
	
	if(uAllowMod(uOwner,uCreatedBy))
		printf(LANG_NBB_MODIFY);
	
	if(uAllowDel(uOwner,uCreatedBy))
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttResourceNavBar(void)


unsigned IsSecondaryOnlyZone(unsigned uZone)
{
	//This function returns the value of tZone.uSecondaryOnly
	//for ExttResourceNavBar()
	unsigned uSecondaryOnly=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cQuery[100]={""};
	
	sprintf(cQuery,"SELECT uSecondaryOnly FROM tZone WHERE uZone='%u'",uZone);
	mysql_query(&gMysql,cQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uSecondaryOnly);

	mysql_free_result(res);

	return(uSecondaryOnly);
	
}//unsigned IsSecondaryOnlyZone(unsigned uZone)


void tResourceTableAddRR(unsigned uZoneId)
{
	guMode=2000;
	uZone=uZoneId;

	tResource("Add new resource record");

}//void tResourceTableAddRR(unsigned uZoneId)


int AutoAddPTRResource(const unsigned d,const char *cDomain,const unsigned uInZone,const unsigned uSourceZoneOwner)
{
	char gcQuery[1024];
	char cParam1[512];
	char cComment[100]={"GenerateArpaZones()"};

	if(!d || !cDomain[0] || !uInZone || !uSourceZoneOwner) return(1);

	uResource=0;
	if(cDomain[strlen(cDomain)-1]!='.')
		sprintf(cParam1,"%s.",cDomain);
	else
		sprintf(cParam1,"%s",cDomain);

	//Only allow one PTR entry per FQDN for same uOwner

	sprintf(gcQuery,"DELETE FROM tResource WHERE uRRType=7 AND cParam1='%s' AND uOwner=%u",
			cParam1,uSourceZoneOwner);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
		return(2);
	
	//Check for other entries, if they exist first come first served
	//(round-robin PTR records actually)
	//notify admin to resolve dispute for rev dns

	sprintf(cName,"%u",d);
	sprintf(gcQuery,"INSERT INTO tResource SET uZone=%u,cName='%.16s',uRRType=7,cParam1='%.511s',cComment='%.99s',"
			"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uInZone
				,cName
				,cParam1
				,cComment
				,uSourceZoneOwner
				,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
		return(3);

	return(0);

}//int AutoAddPTRResource()

