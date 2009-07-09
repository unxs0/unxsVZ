 /*
FILE
	$Id$
PURPOSE
	Non-schema dependent tzone.c expansion.
AUTHORS
	GPL License applies, see www.fsf.org for details
	See LICENSE file in this distribution
	(C) 2001-2009 Gary Wallis and Hugo Urquiza for Unixservice.
TODO
	See TODOs in this file.
	Move them and all this to CHANGES.

	Problems:
		1-. Can't allow modifying an existing zone's cZone. Must delete it first
		to create a new one.

		2-. RevDNS checkbox does not work for "first time" add of first ns. So I turned
		it off for ALL New ops. This is dumb but better than broken zones.
		(support @ openisp . net)
	Solution:
		Must check cZone against currently saved cZone. If changed
		must notify user and not allow mod.
	
*/

#include <openisp/ucidr.h>
#include <math.h>

//from tresource.c
#define VAR_LIST_tResource "tResource.uResource,tResource.uZone,tResource.cName,tResource.uTTL,tResource.uRRType,tResource.cParam1,tResource.cParam2,tResource.cComment,tResource.uOwner,tResource.uCreatedBy,tResource.uCreatedDate,tResource.uModBy,tResource.uModDate"

static char *cMassList={""};
static char *cMessage={"Cut and paste your list into cMassList. Select an operation to perform from \"Mass/Bulk OPs Panel.\""};
static char cSearch[64]={""};
static char cExtZone[256]={""};
static char cIPBlock[100]={""};
static unsigned uDelegationTTL=0;
static char *cNSList={""};
static unsigned uSubmitJob=0;

static char cTargetZone[100]={""};

//Aux checkboxes
static unsigned uSearchSecOnly=0;

//Aux drop/pull downs
static char cCustomerDropDown[256]={""};
static unsigned uDDClient=0;

//Local
void ResourceRecordList(unsigned uZone);
void TableAddRR(void);
void UpdateSerialNum(unsigned uZone);
void tResourceTableAddRR(unsigned uZone);
int AddNewArpaZone(char *cArpaZone, unsigned uExtNSSet, char *cExtHostmaster);
int IllegalZoneDataChange(void);
#ifndef DEBUG_REPORT_STATS_OFF
	int UpdateInfo();
#endif
void MassOperations(void);
char *ParseTextAreaLines(char *cTextArea);
void htmlInZone(void);
void htmlSecondaryServiceOnly(void);
void htmlSecondaryServiceCleanup(void);
void htmlMassUpdate(void);
void htmlMassDelete(void);
void htmlMassResourceFix(void);
void htmlCustomerZones(void);
void htmlZoneList(void);
void CustomerDropDown(unsigned uSelector);
void htmlMassResourceImport(void);
void htmlZonesFromBlocks(void);
void htmlMassCopyToOtherViews(void);
void htmlMassUpdate(void);
void htmlMassPTRCheck(void);
void htmlMassCheckZone(void);
void tZoneNavList(void);	
void tZoneContextInfo(void);
void DelegationTools(void);
unsigned uPTRInCIDR(unsigned uZone,char *cIPBlock);
unsigned uPTRInBlock(unsigned uZone,unsigned uStart,unsigned uEnd);
void htmlCheckSOA(void);
void htmlMasterZoneFile(void);
void htmlMasterZonesCheck(void);
void htmlMasterNamedCheckZone(void);
void tNSSetMembers(unsigned uNSSet);//tnssetfunc.h
void CloneZone(char *cSourceZone,char *cTargetZone,unsigned uView);

//bind.c
void ProcessRRLine(const char *cLine,char *cZoneName,const unsigned uZone,const unsigned uCustId,
			const unsigned uNSSet,const unsigned uCreatedBy,const char *cComment);
#include "local.h"
void SerialNum(char *serial);
void GenerateArpaZones(void);
void GetGlossary(const char *cName, char *cValue);
int FetchNSSet(char *cList, char *cNSSet);

//bind.c
void CreateMasterFiles(char *cMasterNS, char *cZone, unsigned uModDBFiles,
		                unsigned uModStubs, unsigned uDebug);
void PassDirectHtml(char *file);//bind.c aux section


void ExtProcesstZoneVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		//printf("%s\n",entries[i].name);
		
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",entries[i].val);
		else if(!strcmp(entries[i].name,"uSearchSecOnly"))
			uSearchSecOnly=1;
		else if(!strcmp(entries[i].name,"cCustomerDropDown"))
		{
			strcpy(cCustomerDropDown,entries[i].val);
			uDDClient=ReadPullDown(TCLIENT,"cLabel",
					cCustomerDropDown);
		}
		else if(!strcmp(entries[i].name,"cMassList"))
			cMassList=entries[i].val;
		else if(!strcmp(entries[i].name,"cIPBlock"))
			sprintf(cIPBlock,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"uDelegationTTL"))
			sscanf(entries[i].val,"%u",&uDelegationTTL);
		else if(!strcmp(entries[i].name,"cNSList"))
			cNSList=entries[i].val;
		else if(!strcmp(entries[i].name,"uSubmitJob"))
			sscanf(entries[i].val,"%u",&uSubmitJob);			
		else if(!strcmp(entries[i].name,"cTargetZone"))
			sprintf(cTargetZone,"%.99s",entries[i].val);
	}

}//void ExtProcesstZoneVars(pentry entries[], int x)


void ZoneCheck(unsigned uMode)
{
	
	//cZone
	//a.tv	
	if(strlen(cZone)<4)
	{
		guMode=uMode;
		tZone("FQDN cZone required. Must specify TLD.");
	}
	if(!strchr(cZone,'.'))
	{
		guMode=uMode;
		tZone("Must use a FQDN for cZone. Must specify TLD.");
	}
	if(cZone[strlen(cZone)-1]=='.')
	{
		guMode=uMode;
		tZone("cZone should not end with a period. Must specify TLD.");
	}
	
	//uNSSet
	if(!uNSSet)
	{
		guMode=uMode;
		tZone("Must select a name server set.");
	}
	
	//cHostmaster
	//a.a.tv
	if(strlen(cHostmaster)<6)
	{
		guMode=uMode;
		tZone("FQDN cHostmaster required. Must specify 'email.' in front.");
	}
	if(!strchr(cHostmaster,'.'))
	{
		guMode=uMode;
		tZone("Must use a FQDN for cHostmaster with 'email.' in front");
	}
	if(cHostmaster[strlen(cHostmaster)-1]=='.')
	{
		guMode=uMode;
		tZone("email.FQDN cHostmaster should not end with a period.");
	}
	
	if(uTTL>1000000)
	{
		guMode=uMode;
		tZone("uTTL out of range.");
	}
	if(!uExpire || uExpire>1000000)
	{
		guMode=uMode;
		tZone("uExpire out of range.");
	}
	if(!uRefresh || uRefresh>1000000)
	{
		guMode=uMode;
		tZone("uRefresh out of range.");
	}
	if(!uRetry || uRetry>1000000)
	{
		guMode=uMode;
		tZone("uRetry out of range.");
	}
	if(!uZoneTTL || uZoneTTL >1000000)
	{
		guMode=uMode;
		tZone("uZoneTTL out of range.");
	}

	//Sanity checks from BIND source db_load.c
	//WARNING SOA expire value is less than SOA refresh+retry
	if(  uExpire < (uRefresh + uRetry) )
	{
		guMode=uMode;
		tZone("SOA expire value is less than SOA refresh+retry.");
	}
	//WARNING SOA expire value is less than refresh + 10 * retry 
	if(  uExpire < (uRefresh + (10 * uRetry)) )
	{
		guMode=uMode;
		tZone("SOA expire value is less than refresh + 10 * retry.");
	}
	//WARNING SOA expire value is less than 7 days
	if(  uExpire < (7 * 24 * 3600) )
	{
		guMode=uMode;
		tZone("SOA expire value is less than 7 days");
	}
	//WARNING SOA expire value is greater than 6 months
	if(  uExpire > ( 183 * 24 * 3600) )
	{
		guMode=uMode;
		tZone("SOA expire value is greater than 6 months");
	}
	//WARNING SOA refresh value is less than 2 * retry
	if(  uRefresh < (uRetry * 2))
	{
		guMode=uMode;
		tZone("SOA refresh value is less than 2 * retry");
	}
	
}//void ZoneCheck(unsigned uMode)


void ExttZoneCommands(pentry entries[], int x)
{
	char cSerial[16]={""};

	if(!strcmp(gcFunction,"tZoneTools"))
	{
		if(!strcmp(gcFind,LANG_BL_TableAddRR))
		{
                        ProcesstZoneVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=7 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				TableAddRR();
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstZoneVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
	                        tZone(LANG_NB_CONFIRMMOD);
			}
			else
				tZone("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstZoneVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				long unsigned luYearMonDay=0;
				char cSerial[12]={""};
				char *cJobType="Modify";


				if(IllegalZoneDataChange())
				{
					guMode=2000;
					tZone("Can't change cZone. Delete and add corrected name if needed.");
				}
				ZoneCheck(2002);
				uModBy=guLoginClient;
				if(!uView) uView=1;//This should be the internal 
						//or one of mutually exclusive restrictive views
						//The uView with the largest uOrder should be the 
						//external or least restrictive view.

				SerialNum(cSerial);
				sscanf(cSerial,"%lu",&luYearMonDay);

				//debug only
				//sprintf(gcQuery,"luYearMonDay=%lu",luYearMonDay);
				//tZone(gcQuery);

				//Typical year month day and 99 changes per day max
				//to stay in correct date format. Will still increment.
				if(uSerial<luYearMonDay)
				{
					sscanf(cSerial,"%u",&uSerial);
				}
				else
				{
					uSerial++;
				}

				//If uNSSet was changed the job we have to submit is different
				if(uZone && uNSSet)
				{
					unsigned uPrevNSSet=0;

					sscanf(ForeignKey("tZone","uNSSet",uZone),"%u",
						&uPrevNSSet);	
					if(uPrevNSSet && uNSSet!=uPrevNSSet)
						cJobType="Modify New";
				}

				if(SubmitJob(cJobType,uNSSet,cZone,0,0))
					htmlPlainTextError(mysql_error(&gMysql));
	                        ModtZone();
			}
			else
				tZone("<blink>Error</blink>: Denied by permissions settings");
                }
		//Default wizard like two step creation and deletion
		else if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=7)
			{
				ProcesstZoneVars(entries,x);
				strcpy(cMainAddress,"0.0.0.0");
				//Check global conditions for new record here
				guMode=2000;
				tZone(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=7)
			{
				MYSQL_RES *res;

	                        ProcesstZoneVars(entries,x);
			
				if(guPermLevel<10 && 
						!strcmp(cZone+strlen(cZone)-5,".arpa"))
				tZone("Only admin level users can add arpa zones directly.");

				if(!uView) uView=1;

				//Make sure cZone not already taken
				sprintf(gcQuery,"SELECT uZone FROM tZone WHERE cZone='%s' AND uView=%u",cZone,uView);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					tZone("cZone/uView already used!");
					mysql_free_result(res);
				}
                                mysql_free_result(res);



				ZoneCheck(2000);
				uZone=0;
				uCreatedBy=guLoginClient;
				if(strcmp(cZone+strlen(cZone)-5,".arpa"))
					uOwner=guCompany;
				else
					uOwner=0;//Public zone
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				SerialNum(cSerial);
				sscanf(cSerial,"%u",&uSerial);

				//TODO figure out...
				//This should be called from inside NewtZone()
				//FIX with ExtNewtZone() success hook or
				//not using NewtZone at all ?

				//Read only cMasterIPs must come from selected
				//uNSSet FK NS set
				sprintf(cMasterIPs,"%.255s",
					ForeignKey("tNSSet","cMasterIPs",uNSSet));

				//For select drop down
				if(!uDDClient)
					uOwner=guCompany;
				else
					uOwner=uDDClient;

				if(uSubmitJob)
				{
					if(SubmitJob("New",uNSSet,cZone,0,0))
						htmlPlainTextError(mysql_error(&gMysql));
				}
#ifndef DEBUG_REPORT_STATS_OFF
				UpdateInfo();
#endif
				//debug only
				//sprintf(gcQuery,"%u",uDDClient);
				//tZone(gcQuery);

				NewtZone(0);
			}
			else
				tZone("<blink>Error</blink>: Denied by permissions settings");
				
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstZoneVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
                        	guMode=2001;
                        	tZone(LANG_NB_CONFIRMDEL);
			}
			else
				tZone("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstZoneVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
#define PRIORITY_LOGERROR 2000
			
				guMode=5;
				//
				//Copy the zone to tDeletedZone before deleting it.
				MYSQL_RES *res;
				MYSQL_ROW field;
				
				sprintf(gcQuery,"INSERT INTO tDeletedZone SET uDeletedZone='%u',cZone='%s',"
						"uNSSet='%u',cHostmaster='%s',uSerial='%u',uExpire='%u',"
						"uRefresh='%u',uTTL='%u',uRetry='%u',uZoneTTL='%u',uMailServers='%u',"
						"uView='%u',cMainAddress='%s',uRegistrar='%u',uSecondaryOnly='%u',"
						"cOptions='%s',uOwner='%u',uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
						uZone,
						cZone,
						uNSSet,
						cHostmaster,
						uSerial,
						uExpire,
						uRefresh,
						uTTL,
						uRetry,
						uZoneTTL,
						uMailServers,
						uView,
						cMainAddress,
						uRegistrar,
						uSecondaryOnly,
						cOptions,
						uOwner);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					printf("Content-type: text/html\n\n%s\n",mysql_error(&gMysql));
				//
				//Copy the tResource records to tDeletedResource
				
				sprintf(gcQuery,"SELECT %s FROM tResource WHERE uZone=%u",VAR_LIST_tResource,uZone);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					printf("Content-type: text/html\n\n%s\n",mysql_error(&gMysql));
				
					//tZone(mysql_error(&gMysql));
				
				res=mysql_store_result(&gMysql);
				while((field=mysql_fetch_row(res)))
				{
					sprintf(gcQuery,"INSERT INTO tDeletedResource SET uDeletedResource='%s',"
							"uZone='%s',cName='%s',uTTL='%s',uRRType='%s',cParam1='%s',"
							"cParam2='%s',cComment='%s',uOwner='%s',uCreatedBy=1,"
							"uCreatedDate=UNIX_TIMESTAMP(NOW())",
							field[0],
							field[1],
							field[2],
							field[3],
							field[4],
							field[5],
							field[6],
							field[7],
							field[8]);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				}
				mysql_free_result(res);
				//Try to delete zone
				sprintf(gcQuery,"DELETE FROM tZone WHERE uZone=%u",uZone);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				if(mysql_affected_rows(&gMysql)>0)
				{
					//If zone deleted delete RR's. Log error only
					sprintf(gcQuery,
					"DELETE FROM tResource WHERE uZone=%u",uZone);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql)) 
						SubmitJob("DeleteRRError",
							uNSSet,
							mysql_error(&gMysql),
							PRIORITY_LOGERROR,0);


					//Schedule job: Queue handler should 
					//replace any mods
					if(SubmitJob("Delete",uNSSet,cZone,0,0))
						htmlPlainTextError(mysql_error(&gMysql));
					tZone(LANG_NBR_RECDELETED);
				}
				else
				{
					tZone(LANG_NBR_RECNOTDELETED);
				}

			}//guPermLevel block
			else
				tZone("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcFind,"Remove Generated PTRs")
				&& guPermLevel>9)
		{
			guMode=2002;
			sprintf(gcQuery,
				"DELETE FROM tResource WHERE cComment='GenerateArpaZones()'");
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
			tZone("GenerateArpaZones() PTR records removed");

		}
		else if(!strcmp(gcFind,"Generate Arpa Zones")
				&& guPermLevel>9)
		{
			guMode=2002;
			ProcesstZoneVars(entries,x);
			ZoneCheck(2002);
			GenerateArpaZones();
		}
		else if(!strcmp(gcCommand,"Mass Operations")
				&& guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			MassOperations();
		}
		else if(!strcmp(gcCommand,"Zones From Blocks")
				&& guPermLevel>9)
		{

			ProcesstZoneVars(entries,x);
			htmlZonesFromBlocks();
		}
		else if(!strcmp(gcCommand,"Customer Zones")
				&& guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!uDDClient)
			{
				cMessage="<blink>Error.</blink> You must select a customer!.";
				MassOperations();
			}
			htmlCustomerZones();
		}
		else if(!strcmp(gcFind,"Zone List")
				&& guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!cMassList[0])
			{
				cMessage="<blink>cMassList empty.</blink> Help below.";
				MassOperations();
			}
			htmlZoneList();
		}
		else if(!strcmp(gcFind,"Copy to Views")
				&& guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!cMassList[0])
			{
				cMessage="<blink>cMassList empty.</blink> Help below.";
				MassOperations();
			}
			htmlMassCopyToOtherViews();
		}
		else if(!strcmp(gcFind,"In tZone?")
				&& guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!cMassList[0])
			{
				cMessage="<blink>cMassList empty.</blink> Help below.";
				MassOperations();
			}
			htmlInZone();
		}
		else if(!strcmp(gcFind,"Secondary Service Cleanup")
				&& guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!cMassList[0])
			{
				cMessage="<blink>cMassList empty.</blink> Help below.";
				MassOperations();
			}
			htmlSecondaryServiceCleanup();
		}
		else if(!strcmp(gcFind,"Secondary Service Only")
				&& guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!cMassList[0])
			{
				cMessage="<blink>cMassList empty.</blink> Help below.";
				MassOperations();
			}
			htmlSecondaryServiceOnly();
		}
		else if(!strcmp(gcFind,"Mass Update")
				&& guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!cMassList[0])
			{
				cMessage="<blink>cMassList empty.</blink> Add one tZone.cZone per line. Help below:";
				MassOperations();
			}
			htmlMassUpdate();
		}
		else if(!strcmp(gcFind,"Mass Delete")
				&& guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!cMassList[0])
			{
				cMessage="<blink>cMassList empty.</blink> Add one tZone.cZone per line. Help below.";
				MassOperations();
			}
			htmlMassDelete();
		}
		else if(!strcmp(gcFind,"Mass PTR Check")
				&& guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!cMassList[0])
			{
				cMessage="<blink>cMassList empty.</blink> Help below.";
				MassOperations();
			}
			htmlMassPTRCheck();
		}
		else if(!strcmp(gcFind,"Mass named-checkzone")
				&& guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!cMassList[0])
			{
				cMessage="<blink>cMassList empty.</blink> Help below.";
				MassOperations();
			}
			htmlMassCheckZone();
		}
		else if(!strcmp(gcFind,"Mass Resource Import")
				&& guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!cMassList[0])
			{
				cMessage="<blink>cMassList empty.</blink> Help below.";
				MassOperations();
			}
			htmlMassResourceImport();
		}
		else if(!strcmp(gcFind,"Mass Resource Fix")
				&& guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!cMassList[0])
			{
				cMessage="<blink>cMassList empty.</blink> Help below.";
				MassOperations();
			}
			MassOperations();
			//htmlMassResourceFix();
		}
		else if(!strcmp(gcCommand,"Delegation Tools") && guPermLevel>=10)
		{
			ProcesstZoneVars(entries,x);
			guMode=4000;
		}
		else if(!strcmp(gcCommand,"Delegate IP Block"))
		{
			ProcesstZoneVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
				guMode=4001;
			else
				tZone("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Confirm Delegation") && guPermLevel>=10)
		{
#define IP_BLOCK_CIDR 1
#define IP_BLOCK_DASH 2
			unsigned uA,uB,uC,uD,uE,uNumIPs;
			unsigned uMa,uMb,uMc;
			unsigned uIPBlockFormat;
			char cNS[100]={""};
			char cName[100]={""};
			char cParam1[100]={""};

			ProcesstZoneVars(entries,x);
			if(!uAllowMod(uOwner,uCreatedBy)) 
				tZone("<blink>Error</blink>: Denied by permissions settings");

			if(!cIPBlock[0])
			{
				guMode=4001;
				tZone("<blink>cIPBlock is a required value</blink>");
			}

			if(!cNSList[0])
			{
				guMode=4001;
				tZone("<blink>cNSList is a required value</blink>");
			}
				
			//remove extra spaces or any other junk in CIDR
			sscanf(cIPBlock,"%s",gcQuery);
			sprintf(cIPBlock,"%.99s",gcQuery);
			
			sscanf(cZone,"%u.%u.%u.in-addr.arpa",&uMc,&uMb,&uMa);
			
			if(strchr(cIPBlock,'/'))
			{
				//cIPBlock is in CIDR format
				sscanf(cIPBlock,"%u.%u.%u.%u/%u",&uA,&uB,&uC,&uD,&uE);

				if((uA!=uMa) || (uB!=uMb) || (uC != uMc))
				{
					guMode=4001;
					tZone("<blink>Error:</blink> The entered block is not inside the loaded zone.");
				}

				//CIDR only checks and calculations
				if(uE<24)
				{
					guMode=4001;
					tZone("<blink>CIDR not supported</blink>");
				}
				
				uNumIPs=2<<(32-uE-1);
				uNumIPs--;

				if((uD+uNumIPs)>255)
				{
					guMode=4001;
					sprintf(gcQuery,"IP Block range error %u (%u)",uD,(2<<(32-uE-1)));
					tZone(gcQuery);
//					tZone("<blink>CIDR range error</blink>");
				}
				
				if(uPTRInCIDR(uZone,cIPBlock))
				{
					guMode=4001;
					tZone("<blink>Delegation overlaps existing PTR records. Can't continue</blink>");
				}
				
				uIPBlockFormat=IP_BLOCK_CIDR;
			}
			else if(strchr(cIPBlock,'-'))
			{
				//cIPBlock is in dash format
				sscanf(cIPBlock,"%u.%u.%u.%u-%u",&uA,&uB,&uC,&uD,&uE);

				if((uA!=uMa) || (uB!=uMb) || (uC != uMc))
				{
					guMode=4001;
					tZone("<blink>Error:</blink> The entered range is not inside the loaded zone.");
				}

				if(uE>255)
				{
					guMode=4001;
					tZone("<blink>IP Block incorrect format</blink>");
				}
				
				if(uE<uD)
				{
					guMode=4001;
					tZone("<blink>IP block range error</blink>");
				}
				
				if(uPTRInBlock(uZone,uD,uE))
				{
					guMode=4001;
					tZone("<blink>Delegation overlaps existing PTR records. Can't continue</blink>");
				}
				
				uNumIPs=uE-uD;
				uIPBlockFormat=IP_BLOCK_DASH;
			}
			
			//basic sanity check (common)
			
			//add check
			if(!uA)
			{
				guMode=4001;
				tZone("<blink>IP Block incorrect format</blink>");
			}
			if((uA>255)||(uB>255)||(uC>255)||(uD>255))
			{
				guMode=4001;
				tZone("<blink>IP Block incorrect format</blink>");
			}
			if(uDelegationTTL>uTTL)
			{
				guMode=4001;
				tZone("<blink>uDelegationTTL out iof range</blink>");
			}
			if(!uDelegationTTL)
				uDelegationTTL=uTTL;
		
			while(1)
			{
				sprintf(cNS,"%.99s",ParseTextAreaLines(cNSList));
				if(!cNS[0]) break;
				
				if(uIPBlockFormat==IP_BLOCK_CIDR)
					sprintf(cName,"%u/%u",uD,uE);
				else if(uIPBlockFormat==IP_BLOCK_DASH)
					sprintf(cName,"%u-%u",uD,uE);

				sprintf(gcQuery,"INSERT INTO tResource SET uZone=%u,cName='%s',uTTL=%u,"
						"uRRType=2,cParam1='%s',cComment='Delegation (%s)',"
						"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uZone
						,cName
						,uDelegationTTL
						,cNS
						,cIPBlock
						,uOwner
						,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
			}

			//$GENERATE 0-255 $ CNAME $.0/24.21.68.217.in-addr.arpa.
			if(uIPBlockFormat==IP_BLOCK_CIDR)
				sprintf(cParam1,"$.%u/%u.%u.%u.%u.in-addr.arpa.",
						uD
						,uE
						,uC
						,uB
						,uA
				       );
			else if(uIPBlockFormat==IP_BLOCK_DASH)
			{
				sprintf(cParam1,"$.%u-%u.%u.%u.%u.in-addr.arpa.",
						uD
						,uE
						,uC
						,uB
						,uA
				       );
			}
			sprintf(gcQuery,"INSERT INTO tResource SET uZone=%u,cName='$GENERATE %u-%u $',"
					"uRRType=5,cParam1='%s',cComment='Delegation (%s)',uOwner=%u,"
					"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uZone
					,uD
					,(uD+uNumIPs)
					,cParam1
					,cIPBlock
					,uOwner
					,guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));

			UpdateSerialNum(uZone);	
			if(uSubmitJob)
			{
				if(SubmitJob("Modify",uNSSet,cZone,0,0))
					htmlPlainTextError(mysql_error(&gMysql));
			}

			tZone("IP block delegation done");
			
		}
		else if(!strcmp(gcCommand,"Remove Delegation") && guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!uAllowDel(uOwner,uCreatedBy))
				tZone("<blink>Error</blink>: Denied by permissions settings");
			guMode=5000;
			tZone("");
		}
		else if(!strcmp(gcCommand,"Confirm Del. Removal") && guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			if(!uAllowDel(uOwner,uCreatedBy))
				tZone("<blink>Error</blink>: Denied by permissions settings");

			if(!cIPBlock[0])
			{
				guMode=5000;
				tZone("<blink>cIPBlock is a required value</blink>");
			}
			sprintf(gcQuery,"DELETE FROM tResource WHERE uZone=%u AND cComment='Delegation (%s)'",uZone,cIPBlock);
			mysql_query(&gMysql,gcQuery);
			if(!mysql_affected_rows(&gMysql))
				 tZone("<blink>No delegation removed</blink>");
			
			if(mysql_errno(&gMysql))
				 htmlPlainTextError(mysql_error(&gMysql));
			UpdateSerialNum(uZone);
			if(uSubmitJob)
			{
				if(SubmitJob("Modify",uNSSet,cZone,0,0))
					htmlPlainTextError(mysql_error(&gMysql));
			}
			tZone("IP block delegation removed");
		}
		else if(!strcmp(gcCommand,"Clone Zone"))
		{
			ProcesstZoneVars(entries,x);
			if(!cZone[0])
				tZone("<blink>Error:</blink> You must select a source zone first");
			if(!cTargetZone[0])
				tZone("<blink>Error:</blink> You must enter a target zone name for clonning a zone");

			CloneZone(cZone,cTargetZone,uView);
			//Submit new job for cTargetZone
		}
		else if(!strcmp(gcFind,"Check SOA") && guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			htmlCheckSOA();
		}
		else if(!strcmp(gcFind,"Master Zone File") && guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			htmlMasterZoneFile();
		}
		else if(!strcmp(gcFind,"Check master.zones") && guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			htmlMasterZonesCheck();
		}
		else if(!strcmp(gcFind,"Run named-checkzone") && guPermLevel>9)
		{
			ProcesstZoneVars(entries,x);
			htmlMasterNamedCheckZone();
		}
			
	}

}//void ExttZoneCommands(pentry entries[], int x)


void ExttZoneButtons(void)
{
	unsigned uDefault=0;

	OpenFieldSet("tZone Aux Panel",100);
	
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter required data into form</u><br>");
			if(guPermLevel>9)
			{
				printf("<p><u>Create for customer</u><br>");
				CustomerDropDown(uDDClient);
			}
                        printf("<br>");
                        printf(LANG_NBB_CONFIRMNEW);
			printf("<br>uSubmitJob <input title='If not checked, no job will "
				"be created for the new zone ' type=checkbox name=uSubmitJob checked value=1>");
                break;

                case 2001:
			printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;
		
		case 4000:
			printf("<p><u>Delegation Tools</u></p>\n");
			printf("<p>Enter below the IP block in CIDR format (e.g. 217.23.24.0/24) "
				"or in dash format (e.g. 217.125.32.17-25) that you wish to delegate. "
				"In the textarea you must place a list with the fully qualified domain "
				"name(s) of the nameserver(s) for the delegation. An optional parameter "
				"is uTTL, if not set the default zone TTL will be used. The uSubmitJob "
				"checkbox determines if a modify job is submitted for the zone upon "
				"delegation. If you want to remove a block delegation enter the IP block "
				"in CIDR format or in dash format in the cIPBlock text box and press the "
				"[Remove Delegation] button below.</p>\n");
			DelegationTools();
			printf("<br><input class=largeButton title='Delegate the entered IP block in a two step procedure'"
				" type=submit name=gcCommand value='Delegate IP Block'><br>\n");
			printf("<br><input class=largeButton title='Remove the entered IP block delegation in a two step procedure' " 
				"type=submit name=gcCommand value='Remove Delegation'><br>\n");
		break;
			
		case 4001:
			printf("<p><u>Delegation Tools. Confirm delegation</u></p>\n");
			printf("<p>By pressing the button below you confirm the delegation. All the required "
			"NS and CNAME records will be created. You can avoid the modify job submission for "
			"this zone upon delegation by un-checking the uSubmitJob checkbox below.</p>\n");
			if(!uSubmitJob)
				uSubmitJob=1;
			DelegationTools();
			printf("<br><input class=largeButton title='Confirms the delegation. Will create all "
			"the required NS and CNAME records' type=submit name=gcCommand value='Confirm Delegation'><br>\n");
		break;

		case 5000:
			printf("<p><u>Delegation Tools. Confirm delegation removal</u></p>\n");
			printf("<p>By pressing the button below you confirm the removal of the  delegation. "
			"You can avoid the modify job submission for this zone upon delegation by un-checking "
			" the uSubmitJob checkbox below.</p>\n");
			if(!uSubmitJob)
				uSubmitJob=1;
			DelegationTools();
			 printf("<br><input class=largeButton title='Confirms the delegation removal. Will "
			 "remove all the NS and CNAME records for the specified IP block' type=submit "
			 "name=gcCommand value='Confirm Del. Removal'><br>\n");
		break;			

		default:
			uDefault=1;
			printf("<u>Table Tips</u><br>");
			printf("The zone table provides the SOA header and some other iDNS only fields. "
				"And is of course the anchor point for all a given zone's resource records. "
				"A zone may have n instances via uView. Important non BIND fields that must be "
				"understood are the uNSSet (required), uMailServer (optional) server groups "
				"and the specialized uSecondaryOnly, cMasterIPs and cOptions. cMainAddress is a "
				"backwords compatible optional field (mysqlBind) for simple A record only zones.\n");
			printf("<p><u>Search Tools</u><br>");
			printf("<input type=text title='cZone, cMainAddr and uOwner=2000 search. Use %% . and _ "
				"for pattern matching when applicable.' name=cSearch value=\"%s\" maxlength=99 size=20> cSearch ",cSearch);
			printf("<input type=checkbox title='Limit to uSecondaryOnly=Yes if checked' name=uSearchSecOnly");
			if(uSearchSecOnly)  printf(" checked");
			printf(" > uSecondaryOnly");
			tZoneNavList();
			if(guLoginClient==uOwner || guReseller==guLoginClient 
						|| guPermLevel>9)
			{
				printf("<p><u>Zone Management Tools</u><br>");
				if(uZone)
				printf("<input class=largeButton title=\"%s\" type=submit name=gcFind value=\"%s\"><br><br>",
					LANG_BT_TableAddRR,LANG_BL_TableAddRR);
			}
			tZoneContextInfo();
			printf("<p><u>Other Zone Tools</u><br>");
			if(uZone && guPermLevel>9)
			{
				printf("<input class=largeButton title='Check SOA via dig text output' "
					"type=submit name=gcFind value='Check SOA'>"); 
				printf("<br><input class=largeButton title='Show master zone file contents' "
					"type=submit name=gcFind value='Master Zone File'>"); 
				printf("<br><input class=largeButton title='Verify that cZone is in master.zones' "
					"type=submit name=gcFind value='Check master.zones'>"); 
				printf("<br><input class=largeButton title='Verify that cZone has no errors that "
					"may cause it not to load or propagate' type=submit name=gcFind"
					" value='Run named-checkzone'>"); 
				if(guPermLevel>9&&strstr(cZone,".in-addr.arpa"))
					printf("<br><input class=largeButton title='IP Block Delegation Tools' "
						"type=submit name=gcCommand value='Delegation Tools'>\n");
			}
			if(guPermLevel>9)
			{
				printf("<p><u>Other Tools</u><br>");
				printf("<input class=largeButton title='Mass/Bulk operations' type=submit"
					" name=gcCommand value='Mass Operations'><br>\n");
				printf("<p><u>Clone Zone</u><br>");
				printf("<input type=text name=cTargetZone size=30 title='Enter the name of the zone you "
					"wish to clone the loaded zone to'<br>\n");
				printf("<input class=lwarnButton title='Clone the loaded zone into a new zone entered above'"
					" type=submit name=gcCommand value='Clone Zone'><br>\n");
			}

	}

	if(!uDefault && cSearch[0])
	{
		printf("<input type=hidden name=cSearch value=\"%s\">",cSearch);
		if(uSearchSecOnly) printf("<input type=hidden name=uSearchSecOnly >");
	}
	
	CloseFieldSet();
	
}//void ExttZoneButtons(void)


void ExttZoneAuxTable(void)
{
	ResourceRecordList(uZone);

}//void ExttZoneAuxTable(void)

unsigned uPTRInBlock(unsigned uZone,unsigned uStart,unsigned uEnd)
{
	MYSQL_RES *res;
	register unsigned uI;

	for(uI=uStart;uI<uEnd;uI++)
	{
		sprintf(gcQuery,"SELECT cName FROM tResource WHERE uRRType=7 AND uZone=%u AND cName='%u'",uZone,uI);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)) return(1);
	}
	
	return(0);
	
}//unsigned uPTRInBlock(unsigned uZone,unsigned uStart,unsigned uEnd)
	
			
unsigned uPTRInCIDR(unsigned uZone,char *cIPBlock)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uA,uB,uC;
	char cIP[100]={""};

	sscanf(cZone,"%u.%u.%u.",&uC,&uB,&uA);
	
	//uRRType=7: PTR
	sprintf(gcQuery,"SELECT cName FROM tResource WHERE uRRType=7 AND uZone=%u",uZone);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(cIP,"%u.%u.%u.%s",uA,uB,uC,field[0]);
		if(uIpv4InCIDR4(cIP,cIPBlock)) return(1);
	}
	return(0);
	
}//unsigned uPTRInCIDR(unsigned uZone,char *cIPBlock)


void DelegationTools(void)
{
	printf("<u>cIPBlock</u><br><input type=text name=cIPBlock size=30 value='%s' "
		"title='IP block in CIDR (e.g. 217.68.21.0/24) or in dash "
		"(e.g. 217.68.21.0-255) format'><br>\n",cIPBlock);
	printf("<u>uTTL</u><br><input type=text name=uDelegationTTL size=30 value='%u' "
		"title='NS RR TTL. If 0, will be set to the default zone TTL'><br>\n",uDelegationTTL);
	printf("<u>cNSList</u><br><textarea  cols=40 wrap=hard rows=3 name=cNSList "
		"title='List of NSs to which you are going to delegate the IP block'>%s</textarea><br>\n",cNSList);
	printf("<u>uSubmitJob</u><br><input title='If not checked, no modify job will "
		"be created for the zone upon delegation' type=checkbox name=uSubmitJob value=1 ");
	if(uSubmitJob)
		printf("checked");
	printf(">\n");

}//void DelegationTools(void)


void ResourceRecordList(unsigned uZone)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	register int i=0;
	register int uArpa=0;
	
	char cTTL[16]={"&nbsp;"};
	char cName[102];
	
	if(!strcmp(cZone+strlen(cZone)-5,".arpa"))
	{
		sprintf(gcQuery,"SELECT tResource.cName,tResource.uTTL,tRRType.cLabel,"
				"tResource.cParam1,tResource.cParam2,tResource.cComment,"
				"tResource.uResource FROM tResource,tRRType WHERE "
				"tResource.uRRType=tRRType.uRRType AND tResource.uZone=%u ORDER BY ABS(tResource.cName)",uZone);
		uArpa=1;
	}
	else
	{
		sprintf(gcQuery,"SELECT tResource.cName,tResource.uTTL,tRRType.cLabel,"
				"tResource.cParam1,tResource.cParam2,tResource.cParam3,"
				"tResource.cParam4,tResource.cComment,tResource.uResource "
				"FROM tResource,tRRType WHERE tResource.uRRType=tRRType.uRRType "
				"AND tResource.uZone=%u ORDER BY tResource.cName",uZone);
	}
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	OpenFieldSet("Resource Records",100);
	if(uArpa)
		printf("<tr bgcolor=black>"
			"<td ><font color=white>Name</td><td ><font color=white>TTL</td>"
			"<td ><font color=white>Type</td><td ><font color=white>Param 1</td>"
			"<td ><font color=white>Param 2</td ><td><font color=white>Comment</td></tr>");
	else
			printf("<tr bgcolor=black>"
				"<td ><font color=white>Name</td><td ><font color=white>TTL</td>"
				"<td ><font color=white>Type</td><td ><font color=white>Param 1</td>"
				"<td ><font color=white>Param 2</td ><td><font color=white>Param 3</td>"
				"<td><font color=white>Param 4</td><td><font color=white>Comment</td></tr>");
	while((field=mysql_fetch_row(res)))
	{
		if(strcmp(field[1],"0"))
			sprintf(cTTL,"%.15s",field[1]);
		else
			sprintf(cTTL,"&nbsp;");

		if(!field[0][0] || field[0][0]=='\t')
			strcpy(cName,"@");
		else
			strcpy(cName,field[0]);

		if(uArpa)
			printf("<tr>"
			"<td valign=top><a class=darkLink href=iDNS.cgi?gcFunction=tResource&uResource=%s&cZone=%s>%s</a>"
			"</td><td valign=top>%s</td><td valign=top>%s</td><td valign=top>%s</td><td valign=top>%s</td>"
			"<td valign=top>%s</td>"
			"</tr>\n",
					field[6],cZone,cName,cTTL,field[2],
					field[3],field[4],field[5]);
		else
/*
tResource.cName 0 
,tResource.uTTL 1
,tRRType.cLabel 2
,tResource.cParam1 3
,tResource.cParam2 4
,tResource.cParam3 5 
,tResource.cParam4 6
,tResource.cComment, 7
tResource.uResource 8
*/
			printf("<tr>"
				"<td valign=top><a class=darkLink href=iDNS.cgi?gcFunction=tResource&uResource=%s>%s</a></td>"
				"<td valign=top>%s</td><td valign=top>%s</td><td valign=top>%.64s</td><td valign=top>%s</td>"
				"<td valign=top>%s</td><td valign=top>%s</td><td valign=top>%s</td>"
				"</tr>\n",
					field[8],cName,cTTL,field[2],
					field[3],field[4],field[5],
					field[6],field[7]);
		i++;

	}
	if(!i)
		printf("<tr><td colspan=6>No resource records</td></tr>\n");
	CloseFieldSet();






}//void ResourceRecordList(void)


void ExttZoneGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uZone"))
		{
			sscanf(gentries[i].val,"%u",&uZone);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cZone"))
		{
			sprintf(cExtZone,"%255s",gentries[i].val);
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",gentries[i].val);
		else if(!strcmp(gentries[i].name,"uSearchSecOnly"))
			uSearchSecOnly=1;

	}
//Debug only
//printf("Content-type: text/plain\n\n");
//printf("uZone=%u,guMode=%u\n",uZone,guMode);
//exit(0);
	if(uZone) ExttZoneSelectRow();

	tZone("");

}//void ExttZoneGetHook(entry gentries[], int x)


void ExttZoneSelect(void)
{
	//Owner 0 is public. Example arpa zones
        //Set non search query here for tTableName()
	if(cSearch[0] && !uSearchSecOnly)
		//ExtSelectSearch("tZone",VAR_LIST_tZone,cSearch,"cZone",NULL,20);
		ExtSelectSearch("tZone",VAR_LIST_tZone,"cZone",cSearch,NULL,20);
	else if(cSearch[0] && uSearchSecOnly)
		ExtSelectSearch("tZone",VAR_LIST_tZone,"cZone",cSearch,"uSecondaryOnly=1",20);
	else if(1)
		ExtSelect("tZone",VAR_LIST_tZone,0);

}//void ExttZoneSelect(void)


void ExttZoneSelectRow(void)
{
	ExtSelectRow("tZone",VAR_LIST_tZone,uZone);

}//void ExttZoneSelectRow(void)


void ExttZoneListSelect(void)
{
	char cCat[512];

	ExtListSelect("tZone",VAR_LIST_tZone);

	//Changes here must be reflected below in ExttZoneListFilter()
        if(!strcmp(gcFilter,"cZone"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tZone.cZone LIKE '%s%%' ORDER BY tZone.cZone",TextAreaSave(gcCommand));
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cMainAddress"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tZone.cMainAddress LIKE '%s%%' ORDER BY tZone.cZone",TextAreaSave(gcCommand));
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uZone"))
        {
                sscanf(gcCommand,"%u",&uZone);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tZone.uZone=%u",uZone);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uOwner"))
        {
                sscanf(gcCommand,"%u",&uZone);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tZone.uOwner=%u ORDER BY tZone.cZone",uZone);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uNSSet"))
        {
                sscanf(gcCommand,"%u",&uZone);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tZone.uNSSet=%u ORDER BY tZone.cZone",uZone);
		strcat(gcQuery,cCat);
	}
        else if(!strcmp(gcFilter,"uMailServer"))
        {
                sscanf(gcCommand,"%u",&uZone);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tZone.uMailServer=%u ORDER BY tZone.cZone",uZone);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uZone");
        }

}//void ExttZoneListSelect(void)


void ExttZoneListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uZone"))
                printf("<option>uZone</option>");
        else
                printf("<option selected>uZone</option>");
        if(strcmp(gcFilter,"cZone"))
                printf("<option>cZone</option>");
        else
                printf("<option selected>cZone</option>");
        if(strcmp(gcFilter,"cMainAddress"))
                printf("<option>cMainAddress</option>");
        else
                printf("<option selected>cMainAddress</option>");
        if(strcmp(gcFilter,"uOwner"))
                printf("<option>uOwner</option>");
        else
                printf("<option selected>uOwner</option>");
        if(strcmp(gcFilter,"uNSSet"))
                printf("<option>uNSSet</option>");
        else
                printf("<option selected>uNSSet</option>");
        if(strcmp(gcFilter,"uMailServer"))
                printf("<option>uMailServer</option>");
        else
                printf("<option selected>uMailServer</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttZoneListFilter(void)


void ExttZoneNavBar(void)
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

	if(uOwner || guLoginClient==1)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttZoneNavBar(void)


void TableAddRR(void)
{
	tResourceTableAddRR(uZone);

}//TableAddRR


int AddNewArpaZone(char *cArpaZone, unsigned uExtNSSet, char *cExtHostmaster)
{
	int retval=0;
	char cSerial[32];
	
	unsigned uSaveuZone=uZone;
	unsigned uSaveuCreatedBy=uCreatedBy;
	unsigned uSaveuOwner=uOwner;
	unsigned uSaveuModBy=uModBy;
	unsigned uSaveuSerial=uSerial;
	char cSavecMainAddress[100];
	char cSavecZone[100];
	
	strcpy(cSavecMainAddress,cMainAddress);
	strcpy(cSavecZone,cZone);

	uZone=0;
	strcpy(cMainAddress,"0.0.0.0");
	strcpy(cZone,cArpaZone);
	uCreatedBy=0;//Do not provide info for other resellers etc...
	uOwner=0;//Public zone
	uModBy=0;//Never modified
	uView=1;
	SerialNum(cSerial);
	sscanf(cSerial,"%u",&uSerial);
	uNSSet=uExtNSSet;
	strcpy(cHostmaster,cExtHostmaster);
	
	Insert_tZone();
	if(mysql_errno(&gMysql)) 
		retval=1;
	//Add to both internal and external views. If system
	//is hosting more than 2 views. Must be edited via tZone
	//by operator.
	uView=2;
	Insert_tZone();
	if(mysql_errno(&gMysql)) 
		retval=1;

	uZone=uSaveuZone;
	uCreatedBy=uSaveuCreatedBy;
	uOwner=uSaveuOwner;
	uModBy=uSaveuModBy;
	uSerial=uSaveuSerial;
	strcpy(cMainAddress,cSavecMainAddress);
	strcpy(cZone,cSavecZone);

	return(retval);

}//int AddNewArpaZone()


void UpdateSerialNum(unsigned uZone)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	long unsigned luYearMonDay=0;
	unsigned uSerial=0;
	char cSerial[16]={""};


	sprintf(gcQuery,"SELECT uSerial FROM tZone WHERE uZone=%u",uZone);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uSerial);
	mysql_free_result(res);
	
	SerialNum(cSerial);
	sscanf(cSerial,"%lu",&luYearMonDay);


	//Typical year month day and 99 changes per day max
	//to stay in correct date format. Will still increment even if>99 changes in one day
	//but will be stuck until 1 day goes by with no changes.
	if(uSerial<luYearMonDay)
		sprintf(gcQuery,"UPDATE tZone SET uSerial=%s WHERE uZone=%u",cSerial,uZone);
	else
		sprintf(gcQuery,"UPDATE tZone SET uSerial=uSerial+1 WHERE uZone=%u",uZone);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void UpdateSerialNum(unsigned uZone)


int IllegalZoneDataChange(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uRetVal=0;
	
	sprintf(gcQuery,"SELECT cZone FROM tZone WHERE uZone=%u",uZone);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
	{
		if(strcmp(field[0],cZone))
			uRetVal=1;
	}
	mysql_free_result(res);
	
	return(uRetVal);

}//int IllegalZoneDataChange(void)


#ifndef DEBUG_REPORT_STATS_OFF

int UpdateInfo(void)
{
	register int sd, rc;
	struct sockaddr_in cliAddr, remoteServAddr;
	struct hostent *h;
	time_t luClock=0,luModDate=0;

	char cQuery[256];

	MYSQL_RES *res;
	MYSQL_ROW field;

	char cInfo[128]={"Emtpy"};
	unsigned uMaxuZone=0;

	time(&luClock);

	sprintf(cQuery,"SELECT uModDate FROM tConfiguration WHERE cLabel='UpdateInfo'");
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
		return(1);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%lu",&luModDate);
	mysql_free_result(res);

	if(luModDate && (luClock < (luModDate + 86400)) )
		return(2);

	sprintf(cQuery,"SELECT MAX(uZone) FROM tZone");
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
		return(3);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		if(field[0]!=NULL)
			sscanf(field[0],"%u",&uMaxuZone);
	}
	mysql_free_result(res);

	sprintf(cInfo,"iDNS%s %u %u %u %.70s",REV,guLoginClient,guPermLevel,uMaxuZone,cZone);

	if(!cInfo[0])
		return(4);

	h=gethostbyname("saturn.openisp.net");
	if(h==NULL)
		return(5);


	remoteServAddr.sin_family = h->h_addrtype;
	memcpy((char *) &remoteServAddr.sin_addr.s_addr,h->h_addr_list[0], h->h_length);
	remoteServAddr.sin_port=htons(53);

	sd=socket(AF_INET,SOCK_DGRAM,0);
	if(sd<0)
		return(6);
  
	cliAddr.sin_family = AF_INET;
	cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	cliAddr.sin_port = htons(0);
  
	rc=bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
	if(rc<0)
		return(7);


	rc=sendto(sd,cInfo,strlen(cInfo)+1,0,(struct sockaddr *)&remoteServAddr,
				sizeof(remoteServAddr));

	if(rc<0)
		return(8);

	if(luModDate)
		sprintf(cQuery,"UPDATE tConfiguration SET uModBy=1,uModDate=%lu,cComment='%s' WHERE cLabel='UpdateInfo'",
			luClock,cInfo);
	else
		sprintf(cQuery,"INSERT INTO tConfiguration SET cLabel='UpdateInfo',cValue='uModDate',cComment='%s',"
				"uCreatedBy=1,uCreatedDate=%lu,uModDate=%lu,uModBy=1",cInfo,luClock,luClock);
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
	{
		char cDebug[512]={""};
		sprintf (cDebug,"query=%s error=%s",cQuery,mysql_error(&gMysql));
		tZone(cDebug);
		return(9);
	}
  
	return(0);

}//int UpdateInfo(void)

#endif


void MassOperations(void)
{
	Header_ism3(":: tZone:Mass Operations",0);

	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");
	printf("<input type=hidden name=gcFunction value=tZoneTools>");
	if(cMessage[0]) 
	{
		printf("%s",cMessage);

		//Extended help via tGlossary
		char cText[512]={"#No tGlossary help text available!"};
		if(gcFind[0])
		{
			GetGlossary(gcFind,cText);
			if(cText[0])
				cMassList=cText;
		}
	}
	printf("</td></tr>");
	printf("<tr><td valign=top width=25%%>");
	OpenFieldSet("Mass/Bulk OPs Panel",100);

	//Panel help
	printf("Warning this panel is not a two step system. No undo available. Operations take place immediately. "
		"Output is text/plain suitable for cutting and pasting into reports and other mass/bulk operations. "
		"<p><font color=red>Red</font> buttons require advanced knowledge and may change your data. Make sure "
		" you have a fresh backup before making mass changes "
		"to your DNS data. "
		"<p>Most lists with more than one column per line can be csv, tab-sv or single space "
		"separated values. More per OP help available by clicking on yellow or red mass OP buttons with "
		"<i>cMassList</i> <b>empty.</b><p>");

	//May apply to buttons below
	printf("<p><u>Use this customer for OP (if applies)</u><br>");
	CustomerDropDown(uDDClient);

	//Report Zones From Blocks
	printf("<p><input class=largeButton title='Get sorted list of zones back based on tBlock entries. Optionally "
		"limit to uOwner. No cMassList entries used.' type=submit name=gcCommand value='Zones From Blocks'\n");

	//Report Customer Zones
	printf("<p><input class=largeButton title='Select customer and get sorted list of zones back. No cMassList "
		"entries used.' type=submit name=gcCommand value='Customer Zones'>\n");

	//Report Zone List
	printf("<p><input class=lalertButton title='Create a custom list of zones based on LIKE syntax. "
		"Customer drop down maybe used.' type=submit name=gcFind value='Zone List'>\n");

	//In tZone?
	printf("<p><input class=lalertButton title='Are these zones in tZone? Only looks at first column. "
		"CSV or TabSV ok. No double quotes.' type=submit name=gcFind value='In tZone?'>\n");

	//named-checkzone
	printf("<p><input class=lalertButton title='Create named-checkzone commands for all zones in list' "
		"type=submit name=gcFind value='Mass named-checkzone'>");

	//Mass PTR Check
	printf("<p><input class=lalertButton title='Check list of in-addr.arpa zones:N PTR cFQDN lines for "
		"existence' type=submit name=gcFind value='Mass PTR Check'>");

	//Mass Resource Import
	printf("<p><input class=lwarnButton title='Import RRs for existing zones. Allows import of multiple "
		"zones with multiple RRs' type=submit name=gcFind value='Mass Resource Import'>");

	//Secondary Service Only
	printf("<p><input class=lwarnButton title='Add zones in two col list: cZone,tNSSet.cMasterIPs(with "
		"the ;)|cLabel|uNSSet. CSV or Tab/SpaceSV. No quotes.' type=submit name=gcFind value='Secondary Service Only'>\n");

	//Secondary Service Cleanup
	printf("<p><input class=lwarnButton title='Remove RRs from all zones with uSecondaryOnly set to yes '"
		" type=submit name=gcFind value='Secondary Service Cleanup'>\n");

	//Mass Update
	printf("<p><input class=lwarnButton title='Create modify jobs for zones in list. Get extended help via "
		"empty cMassList!' type=submit name=gcFind value='Mass Update'>");

	//Mass Delete
	printf("<p><input class=lwarnButton title='delete zones in list from db no jobs created' "
		"type=submit name=gcFind value='Mass Delete'>");

	
	//Mass Resource Fix
	//printf("<p><input class=lwarnButton title='Custom bind.c Import() $ORIGIN bug fix' "
	//	"type=submit name=gcFind value='Mass Resource Fix' disabled>");

	//Copy to Views
	printf("<p><input class=lwarnButton title='Copy existing zone data (and RRs) to all other views' "
		" type=submit name=gcFind value='Copy to Views'>\n");

	CloseFieldSet();

        printf("</td><td valign=top>");
	OpenFieldSet("Mass Data Entry Panel",100);
	OpenRow("cMassList","black");
	printf("<textarea title='Enter your list. Cut and paste from other sources.' cols=80 rows=20 name=cMassList");
	if(guPermLevel>=7)
	{
		printf(">%s</textarea></td></tr>\n",cMassList);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cMassList);
		printf("<input type=hidden name=cMassList value=\"%s\" >\n",EncodeDoubleQuotes(cMassList));
	}
	CloseFieldSet();


	Footer_ism3();

}//void MassOperations(void)


//Does not allow empty lines...this may need reviewing ;) to say the least.
char *ParseTextAreaLines(char *cTextArea)
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

}//char *ParseTextAreaLines(char *cTextArea)


void htmlInZone(void)
{
	char cZone[256]={"ERROR"};
	char cImportSource[256]={""};
	char *cp;
	MYSQL_RES *res;
	unsigned uCount=0;
	unsigned uFoundCount=0;
	unsigned uOnlyCheck=0;

	printf("Content-type: text/plain\n\n");
	printf("htmlInZone() start\n");
	while(cZone[0])
	{

		sprintf(cZone,"%.250s",ParseTextAreaLines(cMassList));
		if(cZone[0]==0) break;

		if(!strncmp(cZone,"#ImportSource=",strlen("#ImportSource=")))
		{
			char *cp;

			sprintf(cImportSource,"%.255s",cZone+strlen("#ImportSource="));
			if((cp=strchr(cImportSource,';'))) *cp=0;
			printf("ImportSource=%s\n",cImportSource);
		}
		if(!strncmp(cZone,"#OnlyCheck",strlen("#OnlyCheck")))
		{
			uOnlyCheck=1;
			printf("OnlyCheck is on\n");
		}


		if(cZone[0]=='#') continue;

		uCount++;
		//Allow tabs, commas or spaces after zone
		if((cp=strchr(cZone,'\t'))) *cp=0;
		if((cp=strchr(cZone,' '))) *cp=0;
		if((cp=strchr(cZone,','))) *cp=0;

		if(cImportSource[0] && !uOnlyCheck)
		{
			if(uDDClient)
				sprintf(gcQuery,"SELECT tResource.cName,tResource.uTTL,tRRType.cLabel,"
						"tResource.cParam1,tResource.cParam2 FROM tResource,tZone,tRRType "
						"WHERE tResource.uZone=tZone.uZone AND tResource.uRRType=tRRType.uRRType "
						"AND tZone.cZone='%s' AND tZone.uOwner=%u",cZone,uDDClient);
			else
				sprintf(gcQuery,"SELECT tResource.cName,tResource.uTTL,tRRType.cLabel,tResource.cParam1,"
						"tResource.cParam2 FROM tResource,tZone,tRRType WHERE"
						" tResource.uZone=tZone.uZone "
						"AND tResource.uRRType=tRRType.uRRType AND tZone.cZone='%s'",cZone);
		}
		else
		{
			if(uDDClient)
				sprintf(gcQuery,"SELECT uZone FROM tZone WHERE cZone='%s' AND uOwner=%u ORDER BY cZone",
						cZone,uDDClient);
			else
				sprintf(gcQuery,"SELECT uZone FROM tZone WHERE cZone='%s' ORDER BY cZone",cZone);
		}
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)==0)
		{
			if(uDDClient)
				printf("%s not found with owner \"%s\"\n",cZone,cCustomerDropDown);
			else
				printf("%s not found\n",cZone);
		}
		else
		{
			if(cImportSource[0])
			{
				char cPath[512]={""};
				FILE *fp;

				sprintf(cPath,"%s/%.254s",cImportSource,cZone);
				if((fp=fopen(cPath,"r"))!=NULL)
				{
					if(!uOnlyCheck)
					{
						MYSQL_ROW field;

						printf("%s\n",cPath);
						while(fgets(gcQuery,254,fp)!=NULL)
							printf("%s",gcQuery);
						printf("\nFrom db\n");

						while((field=mysql_fetch_row(res)))
						{
							printf("%s %s %s %s %s\n",
							field[0],
							field[1],
							field[2],
							field[3],
							field[4]
							);
						}
						printf("\n\n");
					}
					fclose(fp);
				}
				else
				{
					printf("%s not found\n",cPath);
				}
			}
			uFoundCount++;
		}
                mysql_free_result(res);
	}
	printf("htmlInZone() end\n");
	if(!uDDClient)
		printf("%u of %u found. %u not found.\n",uFoundCount,uCount,uCount-uFoundCount);
	else
		printf("%u of %u found. %u not found for \"%s\".\n",
				uFoundCount,uCount,uCount-uFoundCount,cCustomerDropDown);
	exit(0);

}//void htmlInZone(void)


void htmlSecondaryServiceOnly(void)
{
	char cZone[256]={"ERROR"};
	char *cp;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uNSSet=0;
	unsigned uNSSetFromTextArea=0;
	char cNSSet[33]={""};
	char cMasterIPs[100]={""};//10.0.0.1; 192.168.10.23; format
	unsigned uFoundCount=0,uCount=0,uAddCount=0;
	unsigned uOnExistUpdate=0;

	printf("Content-type: text/plain\n\n");
	printf("htmlSecondaryServiceOnly() start\n");
	while(cZone[0])
	{

		sprintf(cZone,"%.250s",ParseTextAreaLines(cMassList));
		if(cZone[0]==0) break;
		uCount++;
		//Allow tabs, commas or single spaces after zone and before
		//One of three optional uNSSet IDs
		if((cp=strchr(cZone,'\t')))
			*cp=0;
		else if((cp=strchr(cZone,' ')))
			*cp=0;
		else if((cp=strchr(cZone,',')))
			*cp=0;

		if(cp==NULL)
		{
			if(!strcmp(cZone,";update"))
			{
				printf("uOnExistUpdate=1\n");
				uOnExistUpdate=1;
				continue;
			}
			printf("Bad format: (%s)\n",cZone);
			continue;
		}

		//Allow multiple options
		sprintf(cNSSet,"%.32s",cp+1);
		sprintf(cMasterIPs,"%.99s",cp+1);
		sscanf(cp+1,"%u",&uNSSetFromTextArea);

		//Debug only
		//printf("cNSSet=(%s)\n",cNSSet);

		//First check tZone
		sprintf(gcQuery,"SELECT uZone FROM tZone WHERE cZone='%s'",cZone);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res))
		{
			printf("%s already exists\n",cZone);
			if(!uOnExistUpdate)
				continue;

		}
                mysql_free_result(res);
		uFoundCount++;

		//NSs selection
		sprintf(gcQuery,"SELECT uNSSet,cMasterIPs FROM tNSSet WHERE cLabel='%s'",
				cNSSet);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uNSSet);
			sprintf(cMasterIPs,"%.99s",field[1]);
		}
                mysql_free_result(res);

		if(uNSSet) goto NSSetSelected;

		sprintf(gcQuery,"SELECT uNSSet FROM tNSSet WHERE cMasterIPs='%s'",cMasterIPs);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uNSSet);
                mysql_free_result(res);

		if(uNSSet) goto NSSetSelected;

		sprintf(gcQuery,"SELECT uNSSet,cMasterIPs FROM tNSSet WHERE uNSSet=%u",uNSSetFromTextArea);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uNSSet);
			sprintf(cMasterIPs,"%.99s",field[1]);
		}
                mysql_free_result(res);

		if(uNSSet)
		{
			goto NSSetSelected;
		}
		else
		{
			printf("Could not determine a uNSSet for %s\n",cZone);
			continue;
		}

NSSetSelected:
		if(!uDDClient) uDDClient=guCompany;
		if(uOnExistUpdate)
			sprintf(gcQuery,"UPDATE tZone SET uNSSet=%u,uSecondaryOnly=1,uModBy=1,"
					"uModDate=UNIX_TIMESTAMP(NOW()) WHERE cZone='%s' AND uView=2 "
					"AND uSecondaryOnly=0 AND uOwner=%u",
					uNSSet,
					cZone,uDDClient);
		else
			sprintf(gcQuery,"INSERT INTO tZone SET cZone='%s',uNSSet=%u,uView=2,"
					"uSecondaryOnly=1,uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					cZone,
					uNSSet,
					uDDClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		if(uOnExistUpdate)
		{
			if(mysql_affected_rows(&gMysql))
			{
				SubmitJob("Modify",uNSSet,cZone,0,0);
				printf("Updated %s and created modify job\n",cZone);
			}
		}
		else
		{
			SubmitJob("New",uNSSet,cZone,0,0);
			printf("Added %s and created new job\n",cZone);
		}
		uAddCount++;

	}
	printf("htmlSecondaryServiceOnly() end\n");
	printf("%u of %u lines in tZone. %u added/modified.\n",uFoundCount,uCount,uAddCount);
	exit(0);

}//void htmlSecondaryServiceOnly(void)


void htmlMassUpdate(void)
{
	char cJobName[32]={"Modify"};
	char cZone[256]={"ERROR"};
	char cNSSet[100]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uNSSet=0;
	unsigned uAssignedNSSet=0;
	time_t luClock;
	unsigned uFoundCount=0,uCount=0,uUpdateCount=0;
	unsigned uZone=0;

	printf("Content-type: text/plain\n\n");
	printf("htmlMassUpdate() start\n");
	while(cZone[0])
	{

		sprintf(cZone,"%.250s",ParseTextAreaLines(cMassList));
		if(cZone[0]==0)
			break;
		//These ignore lines are compatible with ParseTextAreaLines()
		//To be able to ignore empty lines we need to change ParseTextAreaLines()
		if(cZone[0]=='#' || cZone[0]==' ' || cZone[0]==';')
			continue;

		char *cp;
		if((cp=strstr(cZone,"cNSSet=")))
		{
			char *cp2;
			if((cp2=strchr(cp+12,';')))
			{
				*cp2=0;
				sprintf(cNSSet,"%.99s",cp+12);
				//NSs selection
				sprintf(gcQuery,"SELECT uNSSet,cMasterIPs FROM tNSSet WHERE cLabel='%s'",
						cNSSet);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(0);
				}
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
					sscanf(field[0],"%u",&uAssignedNSSet);
       		         	mysql_free_result(res);

				if(uAssignedNSSet)
					printf("cNSSet assigned:%s\n",cNSSet);
				continue;
			}
		}
		else
		{
			uCount++;
		}

		//First check tZone
		sprintf(gcQuery,"SELECT uNSSet,uZone FROM tZone WHERE cZone='%s' AND uOwner=%u"
					,cZone,guCompany);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)==0)
		{
			printf("%s not found. Ignoring this zone for update\n",cZone);
			continue;
		}
		else if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uNSSet);
			sscanf(field[1],"%u",&uZone);
		}
                mysql_free_result(res);
		uFoundCount++;


		//NSs selection
		sprintf(gcQuery,"SELECT uNSSet,cMasterIPs FROM tNSSet WHERE uNSSet=%u",uNSSet);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uNSSet);
                mysql_free_result(res);

		if(!uNSSet)
		{
			printf("Could not determine a uNSSet for %s. Ignoring this zone for update\n",cZone);
			continue;
		}


		if(cNSSet[0] && uNSSet!=uAssignedNSSet)
		{
			sprintf(gcQuery,"UPDATE tZone SET uNSSet=%u WHERE cZone='%s'",
				uAssignedNSSet,cZone);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			printf("NS set changed for this zone\n");
			uNSSet=uAssignedNSSet;
			sprintf(cJobName,"Modify New");
		}
		else if(cNSSet[0])
		{
			printf("NS set not changed ignoring this zone for update\n");
			continue;
		}

		UpdateSerialNum(uZone);
		if(uDDClient)
		{
			sprintf(gcQuery,"UPDATE tZone SET uOwner=%u WHERE cZone='%s'",
				uDDClient,cZone);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}

			//TODO Must test this, creates dependency on mySQL version 4.1.20+
			//Check this out further may work with prev releases.
			sprintf(gcQuery,"UPDATE tResource,tZone SET tResource.uOwner=%u"
					" WHERE tResource.uZone=tZone.uZone AND tZone.cZone='%s'",
							uDDClient,cZone);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			printf("uOwner changed for this zone\n");
		}
		time(&luClock);
		SubmitJob(cJobName,uNSSet,cZone,0,luClock+300);//Run in 5 mins
		printf("Serial number updated %s and new job created\n",cZone);
		uUpdateCount++;

	}
	printf("htmlMassUpdate() end\n");
	printf("%u of %u found in tZone. %u updated.\n",uFoundCount,uCount,uUpdateCount);
	exit(0);


}//void htmlMassUpdate(void)


void htmlMassDelete(void)
{
	char cZone[256]={"ERROR"};
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uNSSet=0;
	unsigned uZone=0;
	unsigned uFoundCount=0,uCount=0,uUpdateCount=0;
	time_t luClock;

	time(&luClock);

	printf("Content-type: text/plain\n\n");
	printf("htmlMassDelete() start\n");
	while(cZone[0])
	{

		sprintf(cZone,"%.250s",ParseTextAreaLines(cMassList));
		if(cZone[0]==0) break;
		uCount++;

		//First check tZone
		if(uDDClient)
			sprintf(gcQuery,"SELECT uZone,uNSSet FROM tZone WHERE cZone='%s' AND uOwner=%u",
							cZone,uDDClient);
		else
			sprintf(gcQuery,"SELECT uZone,uNSSet FROM tZone WHERE cZone='%s' AND uOwner=%u",
							cZone,guCompany);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)==0)
		{
			printf("%s not found (%u)\n",cZone,uDDClient);
			continue;
		}
		else if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uZone);
			sscanf(field[1],"%u",&uNSSet);
		}
                mysql_free_result(res);
		uFoundCount++;

		//NSs selection
		sprintf(gcQuery,"SELECT uNSSet FROM tNSSet WHERE uNSSet=%u",uNSSet);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uNSSet);
                mysql_free_result(res);

		if(!uNSSet)
		{
			printf("Could not determine a uNSSet for %s\n",cZone);
			continue;
		}

		//Delete or Del?
		SubmitJob("Delete",uNSSet,cZone,0,luClock+600);
		printf("Delete job created for %s\n",cZone);

		sprintf(gcQuery,"DELETE FROM tZone WHERE uZone=%u",uZone);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		sprintf(gcQuery,"DELETE FROM tResource WHERE uZone=%u",uZone);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		printf("%s deleted (%u)\n",cZone,uDDClient);
		uUpdateCount++;

	}
	printf("htmlMassDelete() end\n");
	printf("%u of %u found in tZone. %u deleted.\n",uFoundCount,uCount,uUpdateCount);
	exit(0);


}//void htmlMassDelete(void)


void htmlCustomerZones(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCount=0;
	FILE *fp;
	char cPath[512]={""};

	printf("Content-type: text/plain\n\n");
	printf("htmlCustomerZones() start\n");

	if(!uDDClient) uDDClient=guLoginClient;
	//Hardwired external SELECT
	sprintf(gcQuery,"SELECT cZone FROM tZone WHERE uOwner=%u AND uView=2 ORDER BY cZone",
		uDDClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(0);
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("%s\n",field[0]);
		uCount++;

		sprintf(cPath,"/usr/local/idns/named.d/master/external/%c/%s",
				field[0][0],field[0]);
		printf("%s:\n\n",cPath);
		if((fp=fopen(cPath,"r"))!=NULL)
		{
			while(fgets(gcQuery,254,fp)!=NULL)
				printf("%s",gcQuery);
			fclose(fp);
			printf("\n");
		}
	}
	mysql_free_result(res);
	printf("htmlCustomerZones() end\n");
	printf("%u found. External view only.\n",uCount);
	exit(0);

}//void htmlCustomerZones(void)


void htmlZoneList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCount=0;
	char cZone[256]={"Error"};

	printf("Content-type: text/plain\n\n");
	printf("htmlZoneList() start\n");

	while(cZone[0])
	{

		sprintf(cZone,"%.250s",ParseTextAreaLines(cMassList));
		if(cZone[0]==0) break;
		uCount++;

		printf("\n%s\n",cZone);
		if(uDDClient)
		sprintf(gcQuery,"SELECT DISTINCT cZone FROM tZone WHERE cZone LIKE '%s'"
				" AND uOwner=%u ORDER BY cZone",cZone,uDDClient);
		else
		sprintf(gcQuery,"SELECT DISTINCT cZone FROM tZone WHERE cZone LIKE '%s' ORDER BY cZone",
			cZone);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			printf("%s\n",field[0]);
			uCount++;
		}
		mysql_free_result(res);
	}

	printf("uCount:%u\n",uCount);
	printf("htmlZoneList() end\n");
	exit(0);

}//void htmlZoneList(void)


void CustomerDropDown(unsigned uSelector)
{
        register int i,n;
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;
	
	if(guLoginClient==1)
	        sprintf(gcQuery,"SELECT uClient,cLabel FROM "TCLIENT" WHERE cCode='Organization' ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT uClient,cLabel FROM "TCLIENT" WHERE (uClient=%u OR uOwner=%u) AND cCode='Organization'",
				guCompany,guCompany);

        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
	mysqlRes=mysql_store_result(&gMysql);
	i=mysql_num_rows(mysqlRes);

        if(i>0)
        {
                printf("<select name=cCustomerDropDown>\n");

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                        {
                             printf("<option>%s</option>\n",mysqlField[1]);
                        }
                        else
                        {
                             printf("<option selected>%s</option>\n",mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=cCustomerDropDown><option title='No selection'>---"
				"</option></select>\n");
        }
        printf("</select>\n");

}//CustomerDropDown()


//This is a custom one time fix
void htmlMassResourceFix(void)
{
	char cNamePart[256]={"ERROR"};
	char cZone[256]={"ERROR"};
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uNSSet=0;
	unsigned uZone=0;
	unsigned uResource=0;
	unsigned uFoundCount=0,uCount=0,uUpdateCount=0;

	printf("Content-type: text/plain\n\n");
	printf("htmlMassResourceFix() start\n");
	while(cZone[0])
	{

		sprintf(cNamePart,"%.250s",ParseTextAreaLines(cMassList));
		if(cNamePart[0]==0) break;
		uCount++;

		//Fix nasty import bug
		//example ns01lhr1.uk.prv.asc-services.net. should be
		//ns01lhr1.uk.prv.asc-services.net.
		//We have a list from import log of lines (thank goodness)
		//Like this (for this example):
		//lhr1.uk.prv.asc-services.net.
		//That string is cNamePart
		//First check tResource against the lines we need to fix
		sprintf(gcQuery,"SELECT tZone.uZone,tZone.uNSSet,tResource.uResource,"
				"tResource.cName,tZone.cZone FROM tResource,tZone WHERE "
				"tResource.uZone=tZone.uZone AND tResource.cName LIKE '%%%s' "
				"AND tResource.cName NOT LIKE '%%.%s' AND tZone.uView=2 AND "
				"tZone.uSecondaryOnly=0 AND tZone.uOwner=%u",
					cNamePart,cNamePart,guCompany);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)==0)
			printf("%s not found\n",cNamePart);
		while((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uZone);
			sscanf(field[1],"%u",&uNSSet);
			sscanf(field[2],"%u",&uResource);
			sprintf(cZone,"%.255s",field[4]);
			//Debug only
			printf("Will fix this tResource.cName=(%s) using (%s). uResource=%u of cZone=(%s)\n",
					field[3],cNamePart,uResource,cZone);
			uFoundCount++;

			//If the original cName and the $ORIGIN change part are the same skip
			if(!strcmp(cNamePart,field[3]))
				continue;

			//2-. Fix section
			//Ex. SELECT CONCAT(SUBSTR(cName,1,INSTR(cName,'lhr1.uk.asc-services.net.')-1),
			//'.lhr1.uk.asc-services.net.') FROM tResource WHERE uResource=98526;
			sprintf(gcQuery,"UPDATE tResource SET cName=CONCAT(SUBSTR(cName,1,(INSTR(cName,'%s')-1)),'.%s')"
					"WHERE uResource=%u AND uOwner=%u",
							cNamePart,cNamePart,uResource,guCompany);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			if(mysql_affected_rows(&gMysql)==0)
			{
				printf("Did not fix %s\n",cNamePart);
				continue;
			}

			UpdateSerialNum(uZone);
			SubmitJob("Modify",1,cZone,0,0);

			printf("%s fixed and mod job created\n",cNamePart);
			uUpdateCount++;
		}//while uResources
                mysql_free_result(res);
	}
	printf("htmlMassResourceFix() end\n");
	printf("%u of %u found in tResource. %u fixed.\n",uFoundCount,uCount,uUpdateCount);
	exit(0);


}//void htmlMassResourceFix(void)


void htmlMassResourceImport(void)
{
	char cLine[512]={"ERROR"};
	char cZone[256]={"ERROR"};
	char *cp;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uDebug=0;
	unsigned uZone;
	unsigned uView=2;//external
	unsigned uZoneOwner;
	unsigned uNSSet;
	unsigned uZoneCount=0,uZoneFoundCount=0,uResourceCount=0,uImportCount=0;
	unsigned uOnlyOncePerZone;

	printf("Content-type: text/plain\n\n");
	printf("htmlMassResourceImport() start (uDDClient=%u)\n",uDDClient);
	while(cZone[0])
	{

		sprintf(cLine,"%.255s",ParseTextAreaLines(cMassList));
		//ParseTextAreaLines() required break;
		if(cLine[0]==0) break;

		//Comments ignore
		if(cLine[0]=='#') continue;
		if(cLine[0]==';') continue;

		//Debug only
		printf("cLine=(%s)\n",cLine);

		//Debug only line everything after this will
		//have no jobs created. Later add uDebug to ProcessRRLine()
		if(!strncmp(cLine,"cMode=debug;",12))
		{
			uDebug=1;
			printf("uDebug=1\n");
			continue;
		}

		if(!strncmp(cLine,"uView=",6))
		{
			if((cp=strchr(cLine,';')))
				*cp=0;
			sscanf(cLine+6,"%u",&uView);
			if(uView>2 || uView<1)
				uView=2;
			printf("uView=%u\n",uView);
			continue;
		}

		//New zone
		if(!strncmp(cLine,"cZone=",6))
		{
			uOnlyOncePerZone=0;
			uZone=0;
			uZoneOwner=0;
			uNSSet=0;
			uZoneCount++;

			if((cp=strchr(cLine,';')))
				*cp=0;
			sprintf(cZone,"%.255s",cLine+6);
			//Debug only
			printf("cZone=(%s)\n",cZone);

			//First check tZone
			sprintf(gcQuery,"SELECT uZone,uNSSet,uOwner FROM tZoneImport WHERE cZone='%s' AND"
						" uSecondaryOnly=0 AND uView=%u",cZone,uView);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&uZone);
				sscanf(field[1],"%u",&uNSSet);
				sscanf(field[2],"%u",&uZoneOwner);
			}
			mysql_free_result(res);
			if(!uZone || !uZoneOwner || !uNSSet)
			{
				printf("Valid %s not found. Skipping.\n",cZone);
				continue;

			}
			uOnlyOncePerZone=1;
			uZoneFoundCount++;
		}
		else
		{
			//A resource candidate line
			uResourceCount++;
			//If we have no defined zone keep on going.
			if(!uZone) continue;
			//If select is used then the RRs are owned by that customer.
			if(uDDClient) uZoneOwner=uDDClient;
			ProcessRRLine(cLine,cZone,uZone,uZoneOwner,uNSSet,guLoginClient,
				"htmlMassResourceImport()");
			if(mysql_affected_rows(&gMysql)==1)
			{
				uImportCount++;
/*
				if(uOnlyOncePerZone && !uDebug)
				{
					time_t luClock;

					//Submit job for first RR. Time for now + 5 minutes
					//This should allow for many more RRs to be added
					//here without complicating the code. A KISS hack?
					UpdateSerialNum(uZone);
					time(&luClock);
					luClock+=300;
					SubmitJob("Modify",uNSSet,cZone,0,luClock);
					uOnlyOncePerZone=0;
				}
*/
			}
		}

	}
	printf("htmlMassResourceImport() end\n");
	printf("%u of %u tZone.cZone found. %u resource lines found %u imported.\n",
		uZoneFoundCount,uZoneCount,uResourceCount,uImportCount);
	exit(0);

}//void htmlMassResourceImport(void)


void htmlZonesFromBlocks(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	MYSQL_RES *res2;
	MYSQL_ROW field2;
	unsigned a,b,c,d,e;
	unsigned uCount=0,uFound=0,uNotFound=0;
	char cZone[100];
	char cPrevZone[100]={""};

	printf("Content-type: text/plain\n\n");
	printf("htmlZonesFromBlocks() start\n");

	if(uDDClient)
	{
		printf("For tClient.cLabel=%s(%u)\n",
			ForeignKey(TCLIENT,"cLabel",uDDClient),uDDClient);
		sprintf(gcQuery,"SELECT DISTINCT cLabel FROM tBlock WHERE uOwner=%u ORDER BY cLabel",
			uDDClient);
	}
	else
	{
		sprintf(gcQuery,"SELECT DISTINCT cLabel FROM tBlock ORDER BY cLabel");
		printf("For all uOwners\n");
	}
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(0);
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{

		printf("field[0]=%s\n",field[0]);

		a=0;b=0;c=0;d=0;e=0;

		sscanf(field[0],"%u.%u.%u.%u/%u",&a,&b,&c,&d,&e);
		if(e<24)
			printf("Error: Must expand further: Block larger than class C\n");
		sprintf(cZone,"%u.%u.%u.in-addr.arpa",c,b,a);
		if(strcmp(cZone,cPrevZone))
		{
			sprintf(cPrevZone,"%.99s",cZone);
		}
		else
		{
			continue;
		}
		uCount++;
		sprintf(gcQuery,"SELECT uZone,uView,uSecondaryOnly FROM tZone WHERE cZone='%s'",
				cZone);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res2=mysql_store_result(&gMysql);
		if(mysql_num_rows(res2)>0)
		{
			while((field2=mysql_fetch_row(res2)))
			{
				uFound++;
				printf("cBlock=%s;cZone=%s;uView=%s;uSecondaryOnly=%s;\n",
					field[0],cZone,field2[1],field2[2]);
			}
		}
		else
		{
			uNotFound++;
			printf("cBlock=%s;cZone=%s; Not found in tZone.\n",field[0],cZone);
		}
		printf("\n");
		mysql_free_result(res2);
	}
	mysql_free_result(res);
	printf("htmlZonesFromBlocks() end\n");
	printf("%u zones found of %u distinct zones made from blocks. %u not found.\n",uFound,uCount,uNotFound);
	exit(0);

}//void htmlZonesFromBlocks(void)


void htmlMassCopyToOtherViews(void)
{
	char cLine[512]={"Error"};
	char cZone[512]={""};
	char cOptions[512]={""};
	unsigned uZone=0;
	unsigned uTargetZone=0;
	unsigned uView=0;
	char *cp,*cp2;
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;
	MYSQL_ROW field2;
	unsigned uCount=0;
	unsigned uCopied=0;
	unsigned uOnlyOnce=1;
	unsigned uRRCopied=0;
	unsigned uFoundCount=0;
	time_t luClock;


	printf("Content-type: text/plain\n\n");
	printf("htmlMassCopyToOtherViews() start\n");
	while(cLine[0])
	{
		sprintf(cLine,"%.511s",ParseTextAreaLines(cMassList));
		if(cLine[0]==0) break;
		uCount++;

		if((cp=strstr(cLine,"cZone=")))
		{
			if((cp2=strchr(cp+6,';')))
			{
				*cp2=0;
				sprintf(cZone,"%.255s",cp+6);
				*cp2=';';
			}
		}
		if((cp=strstr(cLine,"uView=")))
		{
			if((cp2=strchr(cp+6,';')))
			{
				*cp2=0;
				sscanf(cp+6,"%u",&uView);
				*cp2=';';
			}
		}

		if(!cZone[0] || uView==0)
		{
			printf("Error: Incorrect line format (%s)\n",cLine);
			continue;
		}

		sprintf(gcQuery,"SELECT uZone,tZone.uNSSet,cHostMaster,"
				"uSerial,uExpire,uRefresh,uTTL,uRetry,uZoneTTL,"
				"uMailServers,cMainAddress,uRegistrar,uSecondaryOnly,"
				"tNSSet.cMasterIPs,cOptions,uOwner FROM "
				"tZone,tNSSet WHERE cZone='%s' AND uView=%u "
				"AND tZone.uNSSet=tNSSet.uNSSet AND tZone.uOwner=%u"
				,cZone,uView,guCompany);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)==0)
		{
			printf("%s/(uView=%u) not found\n",cZone,uView);
		}
		else
		{
			uFoundCount++;
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&uZone);
				sscanf(field[1],"%u",&uNSSet);
				sprintf(cHostmaster,"%.99s",field[2]);
				sscanf(field[3],"%u",&uSerial);
				sscanf(field[4],"%u",&uExpire);
				sscanf(field[5],"%u",&uRefresh);
				sscanf(field[6],"%u",&uTTL);
				sscanf(field[7],"%u",&uRetry);
				sscanf(field[8],"%u",&uZoneTTL);
				sscanf(field[9],"%u",&uMailServers);
				sprintf(cMainAddress,"%.99s",field[10]);
				sscanf(field[11],"%u",&uRegistrar);
				sscanf(field[12],"%u",&uSecondaryOnly);
				sprintf(cMasterIPs,"%.99s",field[13]);
				sprintf(cOptions,"%.512s",field[14]);
				sscanf(field[15],"%u",&uOwner);
			}
		}
                mysql_free_result(res);

		if(uZone==0)
		{
			printf("Error: Unexpected uZone value for (%s)\n",cLine);
			continue;
		}

		sprintf(gcQuery,"SELECT uView FROM tView WHERE uView!=%u",uView);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			uOnlyOnce=1;

			sprintf(gcQuery,"SELECT uZone FROM tZone WHERE cZone='%s' AND uView=%s",
				cZone,field[0]);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			res2=mysql_store_result(&gMysql);
			if((field2=mysql_fetch_row(res2)))
			{
				sscanf(field2[0],"%u",&uTargetZone);
				mysql_free_result(res2);
				goto DoNotInsertZone;
			}
			mysql_free_result(res2);

			sprintf(gcQuery,"INSERT INTO tZone SET cZone='%s',uView=%s,uNSSet=%u,"
					"cHostmaster='%s',uSerial=%u,uExpire=%u,uRefresh=%u,uTTL=%u,"
					"uRetry=%u,uZoneTTL=%u,uMailServers=%u,cMainAddress='%s',"
					"uRegistrar=%u,uSecondaryOnly=%u,cOptions='%.512s',uOwner=%u,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW())",
					cZone
					,field[0]
					,uNSSet
					,cHostmaster
					,uSerial
					,uExpire
					,uRefresh,uTTL
					,uRetry
					,uZoneTTL
					,uMailServers
					,cMainAddress
					,uRegistrar
					,uSecondaryOnly
					,cOptions
					,uOwner);
			//printf("%s\n",gcQuery);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			if((uTargetZone=mysql_insert_id(&gMysql)))
			{
				UpdateSerialNum(uTargetZone);
				time(&luClock);
				luClock+=500;
				SubmitJob("New",uNSSet,cZone,0,luClock);
				uCopied++;
			}
DoNotInsertZone:
			//debug only
			//printf("DoNotInsertZone: uZone=%u uTargetZone=%u\n",uZone,uTargetZone);
			sprintf(gcQuery,"SELECT cName,uTTL,uRRType,cParam1,cParam2,uOwner FROM tResource WHERE uZone=%u",
					uZone);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			res2=mysql_store_result(&gMysql);
			while((field2=mysql_fetch_row(res2)))
			{
				sprintf(gcQuery,"INSERT INTO tResource SET uZone=%u,cName='%s',"
						"uTTL=%s,uRRType=%s,cParam1='%s',cParam2='%s',"
						"cComment='htmlMassCopyToOtherViews()',uOwner=%s,"
						"uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uTargetZone,
						field2[0],
						field2[1],
						field2[2],
						field2[3],
						field2[4],
						field2[5]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(0);
				}
				if(mysql_affected_rows(&gMysql) && uOnlyOnce)
				{
					UpdateSerialNum(uTargetZone);
					time(&luClock);
					luClock+=600;
					SubmitJob("Mod",uNSSet,cZone,0,luClock);
					uRRCopied++;
					uOnlyOnce=0;
				}
			}
			mysql_free_result(res2);

		}
                mysql_free_result(res);
	}
	printf("htmlMassCopyToOtherViews() end\n");
	printf("%u of %u found. %u not found. %u copied, %u RR copied\n",
				uFoundCount,uCount,uCount-uFoundCount,uCopied,uRRCopied);
	exit(0);

}//void htmlMassCopyToOtherViews(void)


void htmlSecondaryServiceCleanup(void)
{
	printf("Content-type: text/plain\n\n");
	printf("htmlSecondaryServiceCleanup() start\n");
	printf("htmlSecondaryServiceCleanup() end\n");
	exit(0);

}//void htmlSecondaryServiceCleanup(void)


//input format came from somehting similar to this on actual zone files
//grep PTR import1/*in-addr.arpa* | cut -f 2 -d / 
void htmlMassPTRCheck(void)
{
	char cLine[512]={"ERROR"};
	char cQuery[512];
	char *cp;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uFound=0,uTotal=0,uFix=0,uFirstLine=1,uIgnorecParam1=0;

	printf("Content-type: text/plain\n\n");
	printf("htmlMassPTRCheck() start\n");
	while(cLine[0])
	{
		char cArpaZone[256]={"ERROR"};
		char cFQDN[256]={"ERROR"};
		unsigned uLastOctet=0;
		unsigned uZone=0;

		sprintf(cLine,"%.255s",ParseTextAreaLines(cMassList));
		//ParseTextAreaLines() required break;
		if(cLine[0]==0) break;

		//Comments ignore
		if(cLine[0]=='#') continue;
		if(cLine[0]==';') continue;

		if(uFirstLine)
		{
			if(strstr(cLine,"Ignore-cParam1"))
			{
				if(strstr(cLine,"Fix"))
				{
					uFix=1;
					printf("uFix set.\n");
				}
				uIgnorecParam1=1;
				printf("Ignore-cParam1 set.\n");
				continue;
			}
		}
		else
		{
			uFirstLine=0;
		}

		if((cp=strchr(cLine,':')))
		{
			*cp=0;
			sprintf(cArpaZone,"%.255s",cLine);
			sscanf(cp+1,"%u PTR %s",&uLastOctet,cFQDN);
		}

		if(uIgnorecParam1)
			sprintf(cQuery,"SELECT tZone.uZone FROM tZone,tResource "
			"WHERE tResource.uZone=tZone.uZone AND tZone.cZone='%s' "
			"AND tResource.cName='%u' AND tResource.uRRType=7",
				cArpaZone,uLastOctet);
		else
			sprintf(cQuery,"SELECT tZone.uZone FROM tZone,tResource "
			"WHERE tResource.uZone=tZone.uZone AND tZone.cZone='%s' "
			"AND tResource.cName='%u' AND tResource.cParam1='%s' AND tResource.uRRType=7",
				cArpaZone,uLastOctet,cFQDN);
		//Debug only
		//printf("%s\n",cQuery);
		mysql_query(&gMysql,cQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(0);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			uFound++;
		}
		else
		{
			if(uIgnorecParam1)
			{
				MYSQL_RES *res2;
				MYSQL_ROW field2;
				sprintf(cQuery,"SELECT tZone.uZone FROM tZone WHERE tZone.cZone='%s'",cArpaZone);
				mysql_query(&gMysql,cQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(0);
				}
				res2=mysql_store_result(&gMysql);
				if((field2=mysql_fetch_row(res2)))
					sscanf(field2[0],"%u",&uZone);
				mysql_free_result(res2);
				if(uFix && uZone)
				{
					printf("%u.%s PTR %s not found, adding.\n",
						uLastOctet,cArpaZone,cFQDN);
					sprintf(cQuery,"INSERT INTO tResource SET uZone=%u,cName='%u',uRRType=7,"
							"cParam1='%s',cComment='htmlMassPTRCheck()',uOwner=1,uCreatedBy=1,"
							"uCreatedDate=UNIX_TIMESTAMP(NOW())",
							uZone,uLastOctet,cFQDN);
					//Debug only
					//printf("%s\n",cQuery);
					mysql_query(&gMysql,cQuery);
					if(mysql_errno(&gMysql))
					{
						printf("%s\n",mysql_error(&gMysql));
						exit(0);
					}
					if(mysql_affected_rows(&gMysql)==1)
					{
						UpdateSerialNum(uZone);
						SubmitJob("Modify",1,cArpaZone,0,0);
					}
				}
				else
				{
					printf("%u.%s PTR not found\n",uLastOctet,cArpaZone);
				}
			}
			else
			{
				printf("%u.%s PTR %s not found\n",uLastOctet,cArpaZone,cFQDN);
			}
		}
		mysql_free_result(res);
		uTotal++;
	}
	printf("htmlMassPTRCheck() end\n");
	printf("Found %u of %u total (%u).\n",uFound,uTotal,(uTotal-uFound));
	exit(0);

}//void htmlMassPTRCheck(void)


void htmlMassCheckZone(void)
{
	char cLine[100]={"ERROR"};
	printf("Content-type: text/plain\n\n");
	printf("htmlMassCheckZone() start\n");
	while(cLine[0])
	{
		char cCommand[512]={"ERROR"};
		FILE *fp;

		sprintf(cLine,"%.99s",ParseTextAreaLines(cMassList));
		//ParseTextAreaLines() required break;
		if(cLine[0]==0) break;

		sprintf(cCommand,"named-checkzone %.99s /usr/local/idns/named.d/master/external/%c/%.99s",
			cLine,cLine[0],cLine);
		if((fp=popen(gcQuery,"r")))
		{
			while(fgets(gcQuery,512,fp))
			printf("%s",gcQuery);
			pclose(fp);
		}
		else
			printf("Error: could not run named-checkzone.\n");
	}
	printf("htmlMassCheckZone() end\n");
	exit(0);

}//void htmlMassCheckZone(void)


void tZoneNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cSearchSecOnly[16]={"&uSearchSecOnly"};

	unsigned uCount=0;

	if(!cSearch[0])
	{
        	printf("<p><u>tZoneNavList</u><br>\n");
        	printf("Must restrict via cSearch\n");
		return;
	}

	if(uSearchSecOnly)
		ExtSelectSearch("tZone","uZone,cZone,(SELECT tView.cLabel FROM tView WHERE tView.uView=tZone.uView)",
					"cZone",cSearch,"uSecondaryOnly=1",20);
	else
		ExtSelectSearch("tZone","uZone,cZone,(SELECT tView.cLabel FROM tView WHERE tView.uView=tZone.uView)",
					"cZone",cSearch,NULL,20);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tZoneNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	printf("<p><u>tZoneNavList</u><br>\n");
	if(mysql_num_rows(res))
	{	
		if(!uSearchSecOnly) cSearchSecOnly[0]=0;
	        while((field=mysql_fetch_row(res)))
		{
			uCount++;
			printf("<a class=darkLink href=iDNS.cgi?gcFunction=tZone&uZone=%s&cSearch=%s%s>%s [%s]</a><br>\n",
				field[0]
				,cURLEncode(cSearch)
				,cSearchSecOnly
				,field[1],field[2]);
			if(uCount>=100)
			{
				printf("More than 100 records: You must refine your search further<br>\n");
				break;
			}
	        }
	}
	else
		printf("No results found<br>");	

        mysql_free_result(res);

}//void tZoneNavList(void)


void tZoneContextInfo(void)
{
        printf("<u>Record Context Info</u><br>");
	tNSSetMembers(uNSSet);

}//void tZoneContextInfo(void)


void htmlCheckSOA(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cTmpFile[100]={"/tmp/iDNS.soadig"};
	pid_t uPID;

	uPID=getpid();
	sprintf(cTmpFile,"/tmp/iDNS.soadig.%u",uPID);


	sprintf(gcQuery,"SELECT tNS.cFQDN FROM tNSSet,tNS WHERE tNSSet.uNSSet=tNS.uNSSet AND"
			" tNSSet.uNSSet=%u ORDER BY tNS.cFQDN",uNSSet);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"dig @%s SOA %s >> %.99s 2>&1",
					field[0],cZone,cTmpFile);
		system(gcQuery);
	}
	mysql_free_result(res);

	Header_ism3("htmlCheckSOA()",0);
	printf("</center><pre>%s<blockquote>",cZone);
	PassDirectHtml(cTmpFile);
	printf("</blockquote></pre>");
	printf("<input type=hidden name=gcFunction value=tZoneTools>");
	unlink(cTmpFile);
	Footer_ism3();

}//void htmlCheckSOA(void)


//hard coded default install path should be replaced by GetConfiguration someday
void htmlMasterZoneFile(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cSearchSecOnly[16]={"&uSearchSecOnly"};

	sprintf(gcQuery,"SELECT cLabel FROM tView WHERE uView=%u",uView);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
		printf("%s",mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char cFile[256]={""};
		struct stat statInfo;

		if(!uSearchSecOnly) cSearchSecOnly[0]=0;
		sprintf(cFile,"/usr/local/idns/named.d/master/%s/%.1s/%s",field[0],cZone,cZone);

		if(!stat(cFile,&statInfo))
		{

			Header_ism3("htmlMasterZoneFile()",0);
			printf("</center><pre>%s<blockquote>",cFile);
			PassDirectHtml(cFile);
			printf("</blockquote></pre>");
			printf("<input type=hidden name=gcFunction value=tZoneTools>");
			printf("Back w/search link: <a class=darkLink href=iDNS.cgi?gcFunction=tZone&uZone=%u&cSearch=%s%s>%s [%s]</a><br>\n",
				uZone
				,cURLEncode(cSearch)
				,cSearchSecOnly
				,cZone
				,field[0]);
			Footer_ism3();
		}
		else
		{
			mysql_free_result(res);
			tZone("<blink>Possible error</blink>: File not found. See command line for building all files");
		}
	}
	else
	{
		mysql_free_result(res);
		tZone("<blink>Error</blink>: Zone not found");
	}

}//void htmlMasterZoneFile(void)


//hard coded default install path should be replaced by GetConfiguration someday
void htmlMasterZonesCheck(void)
{
	char cSystem[128];
	char cTmpFile[100]={"/tmp/iDNS.mzc"};
	pid_t uPID;

	uPID=getpid();
	sprintf(cTmpFile,"/tmp/iDNS.mzc.%u",uPID);
	sprintf(cSystem,"grep %s /usr/local/idns/named.d/master.zones > %.99s 2>&1",
				cZone,cTmpFile);
	system(cSystem);

	Header_ism3("htmlMasterZonesCheck()",0);
	printf("</center><pre>%s<blockquote>",cZone);
	PassDirectHtml(cTmpFile);
	printf("</blockquote></pre>");
	printf("<input type=hidden name=gcFunction value=tZoneTools>");
	unlink(cTmpFile);
	Footer_ism3();

}//void htmlMasterZonesCheck(void)


//hard coded default install path should be replaced by GetConfiguration someday
void htmlMasterNamedCheckZone(void)
{
	char cSystem[256];
	char cView[100]={"external"};
	char cTmpFile[100]={"/tmp/iDNS.ncz"};
        MYSQL_RES *res;
        MYSQL_ROW field;
	pid_t uPID;

	sprintf(gcQuery,"SELECT cLabel FROM tView WHERE uView=%u",uView);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
		printf("%s",mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cView,"%.99s",field[0]);
	mysql_free_result(res);

	uPID=getpid();
	sprintf(cTmpFile,"/tmp/iDNS.ncz.%u",uPID);
	sprintf(cSystem,"/usr/sbin/named-checkzone %.64s /usr/local/idns/named.d/master/%.64s/%.1s/%.64s > %s 2>&1",
		cZone,cView,cZone,cZone,cTmpFile);
	system(cSystem);

	Header_ism3("htmlMasterNamedCheckZone()",0);
	printf("</center><pre>%s<blockquote>",cSystem);
	PassDirectHtml(cTmpFile);
	printf("</blockquote></pre>");
	printf("<input type=hidden name=gcFunction value=tZoneTools>");
	unlink(cTmpFile);
	Footer_ism3();

}//void htmlMasterNamedCheckZone(void)


void CloneZone(char *cSourceZone,char *cTargetZone,unsigned uView)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uSrcZone=0;

	//Sanity check
	sprintf(gcQuery,"SELECT uZone FROM tZone WHERE cZone='%s' AND uView=%u AND uOwner=%u",
			cTargetZone,uView,guCompany);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if(mysql_num_rows(res))
		tZone("<blink>Error:</blink> Target zone must exist and be owned by your company");
	
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT uNSSet,cHostmaster,uSerial,uExpire,uRefresh,uTTL,uRetry,uZoneTTL,uMailServers, "
	//sprintf(gcQuery,"SELECT uNameServer,cHostmaster,uSerial,uExpire,uRefresh,uTTL,uRetry,uZoneTTL,uMailServers, "
			"cMainAddress,uRegistrar,uSecondaryOnly,cOptions,uOwner,uZone "
			"FROM tZone WHERE cZone='%s' AND uView=%u",cSourceZone,uView);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[14],"%u",&uSrcZone);
		sprintf(gcQuery,"INSERT INTO tZone SET cZone='%s',uNSSet=%s,cHostmaster='%s',uSerial=%s,uExpire=%s,uRefresh=%s,uTTL=%s,uRetry=%s,"
		//sprintf(gcQuery,"INSERT INTO tZone SET uNameServer=%s,cHostmaster='%s',uSerial=%s,uExpire=%s,uRefresh=%s,uTTL=%s,uRetry=%s,"
				"uZoneTTL=%s,uMailServers=%s,uView=%u,cMainAddress='%s',uRegistrar=%s,uSecondaryOnly=%s,"
				"cOptions='%s',uOwner=%s,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				cTargetZone
				,field[0]
				,field[1]
				,field[2]
				,field[3]
				,field[4]
				,field[5]
				,field[6]
				,field[7]
				,field[8]
				,uView
				,field[9]
				,field[10]
				,field[11]
				,field[12]
				,field[13]
				,guLoginClient
				);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		uZone=mysql_insert_id(&gMysql);

		mysql_free_result(res);

		sprintf(gcQuery,"SELECT cName,uTTL,uRRType,cParam1,cParam2,cParam3,cParam4,cComment,uOwner FROM tResource WHERE uZone=%u",uSrcZone);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			sprintf(gcQuery,"INSERT INTO tResource SET cName='%s',uTTL=%s,uRRType=%s,cParam1='%s',cParam2='%s',cParam3='%s',cParam4='%s',"
					"cComment='%s',uOwner=%s,uZone=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					field[0]
					,field[1]
					,field[2]
					,field[3]
					,field[4]
					,field[5]
					,field[6]
					,field[7]
					,field[8]
					,uZone
					,guLoginClient
					);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));

		}
	}	
	if(SubmitJob("New",uNSSet,cTargetZone,0,0))
		htmlPlainTextError(mysql_error(&gMysql));

	tZone("Zone cloned OK");

}//void CloneZone(char *cSourceZone,char *cTargetZone);
