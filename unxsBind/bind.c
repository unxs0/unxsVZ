/*
FILE
	$Id$
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza for Unixservice.
PURPOSE
	GUI independent code:
	Creates named configuration and zone data files
WORKINPROGRESS

	Feb 2009. In the middle of major change: Replacing the wierd t Name Server.cList for
	a much more flexible tNSSet schema that allows mixed NS sets on the same named instance.

	Of course next we can do the MX list. A lot easier and for schema reasons only. No one really big
	uses it anyway. ;)

	Search for TODO
	clock+60 stuff why? For wait for next crontab slot.
	New arpa zone: Problems? Check.
	ProcessExtJobQueue: Read mysqlISP job queue and create local job queue
	entries.

	Creation of zone files: Records in what order when? how?

	Import does not support uOwner. Must after import:
	1-. Use iDNS.cgi tZone Mass Operations MassUpdate Selecting customer from drop down.
	2-. Then since that tool may not currently support RRs, we must:
	mysql> update tResource,tZone set tResource.uOwner=8726 where tResource.uZone=tZone.uZone and tZone.uOwner=8726;
	Where uOwner would be replaced by the correct uOwner via tClient search.

	See TODO
*/

//
//This header section has to organized into:
//	1-. Job queue processing and support functions.
//	2-. Import of external DNS data and supporting functions.
//	3-. Other mainfunc.h misc reports and supporting functions.
//Then the source code below also organized in a similar fashion
//	The objective is to navigate quickly around this file.
//	If it continues to get bigger we should break it down into
//	other .c files.

#include "mysqlrad.h"
#include <dirent.h>
#include <openisp/ucidr.h>

//External and local Prototypes (out of order for now)
void EncryptPasswdWithSalt(char *cPasswd,char *cSalt);//main.c
unsigned uGetZoneOwner(unsigned uZone);//local

//#define EXPERIMENTAL
#ifdef EXPERIMENTAL
	unsigned uNamedCheckConf(char *cNSSet);
	#warning "You are compiling with the EXPERIMENTAL define on. You may be including code:"
	#warning "That is in progress, not properly tested, possibly buggy or that does not compile."
	#warning "You have been warned!"
#endif


//aux section
extern char gcBinDir[];
void ConnectMysqlServer(void);//local also used in mainfunc.h
void ExportTable(char *cTable, char *cFile);//local used in mainfunc.h for CLI command
void GenerateArpaZones(void);//local used in tzonefunc.h
void GetConfiguration(const char *cName, char *cValue, unsigned uHtml);//local used in many places
void PassDirectHtml(char *file);//local also used by tzonefunc.h
//local used in tzonefunc.h and tresourcefunc.h:
int PopulateArpaZone(const char *cZone, const char *cIPNum, const unsigned uHtmlMode, 
					const unsigned uFromZone, const unsigned uZoneOwner);
void SerialNum(char *serial);//local also used in tzonefunc.h
unsigned uGetNSSet(char *cNameServer);

//import section
typedef struct {
	unsigned uSerial;
	unsigned uTTL;
	unsigned uRefresh;
	unsigned uRetry;
	unsigned uExp;
	unsigned uNegTTL;
	char cHostmaster[100];
	char cNameServer[100];
} structSOA;
void ProcessRRLine(const char *cLine,char *cZoneName,const unsigned uZone,
		const unsigned uCustId,const unsigned uNSSet,
		const unsigned uCreatedBy,const char *cComment);//local used in tzonefunc.h also
void ImportSORRs(void);//local mainfunc.h CLI command
void ProcessSORRLine(const char *cLine,char *cZoneName,const unsigned uZone,
		const unsigned uCustId,const unsigned uNSSet,
		const unsigned uCreatedBy,const char *cComment);
structSOA *ProcessSOA(FILE *fp);//local support function
void Import(void);//local mainfunc.h CLI command ImportZones
void DropImportedZones(void);//local mainfunc.h CLI command
void ImportCompanies(void);//local mainfunc.h CLI command
void DropCompanies(void);//local mainfunc.h CLI command
void ImportUsers(void);//local mainfunc.h CLI command
void DropUsers(void);//local mainfunc.h CLI command
void ImportBlocks(void);//local mainfunc.h CLI command
void DropBlocks(void);//local mainfunc.h CLI command
void ImportRegistrars(void);//local mainfunc.h CLI command
void DropRegistrars(void);//local mainfunc.h CLI command
void AssociateRegistrarsZones(void);//local mainfunc.h CLI command

//mass operations
void MassZoneUpdate(void);//mainfunc.h CLI command
void MassZoneNSUpdate(char *cLabel);//mainfunc.h CLI command

//Job queue processing functions
static unsigned uReconfig=0;
static unsigned uReload=0;
void SlaveJobQueue(char *cNameServer,char *cMasterIP);//local used in mainfunc.h for several CLI commands
void MasterJobQueue(char *cNameServer);//local used in mainfunc.h for several CLI commands
void ServerJobQueue(char *cServer);//local used in mainfunc.h for CLI command
void CreateMasterFiles(char *cMasterNS,char *cZone, unsigned uModDBFiles, 
		unsigned uModStubs,unsigned uDebug);//local used in mainfunc.h for CLI command
void CreateSlaveFiles(char *cSlaveNS,char *cZone, char *cMasterIP,
		unsigned uDebug);//local used in mainfunc.h for CLI command
void InstallNamedFiles(char *cIpNum);//local used in mainfunc.h for CLI command


int ProcessMasterJob(char *cZone,unsigned uDelete,unsigned uModify,
		unsigned uNew, unsigned uDeleteFirst, char *cMasterNS);
int ProcessSlaveJob(char *cZone,unsigned uDelete,unsigned uModify,unsigned uNew,
		unsigned uDeleteFirst, char *cMasterNS, char *cMasterIP);
char *cPrintNSList(FILE *zfp,char *cuNSSet);//local
void PrintMXList(FILE *zfp,char *cuMailServers);//local
unsigned ViewReloadZone(char *cZone);//local


//External. Used here but located in other files.
int AddNewArpaZone(char *cArpaZone, unsigned uExtNSSet, char *cExtHostmaster);//tzonefunc.h
int AutoAddPTRResource(const unsigned d,const char *cDomain,const unsigned uInZone,const unsigned uSourceZoneOwner);
void UpdateSerialNum(unsigned uZone);//local used in tblockfunc.h, tresourcefunc.h and tzonefunc.h

//Ext jobqueue functions moved
#include "extjobqueue.h"
#include "extjobqueue.c"

//tclientfunc.h
unsigned uGetOrganization(char *cLabel);

//
// Major file section
//	Importing DNS data functions
//

void ProcessRRLine(const char *cLine,char *cZoneName,const unsigned uZone,
			const unsigned uCustId,const unsigned uNSSet,
			const unsigned uCreatedBy,const char *cComment)
{
	char cName[100]={""};
	char cNamePlus[200]={""};
	char cParam1[256]={""};
	char cParam2[256]={""};
	char cParam3[256]={""};
	char cParam4[256]={""};
	char cType[256]={""};
	static char cPrevZoneName[100]={""};
	static char cPrevcName[100]={""};
	unsigned uRRType=0;
	static unsigned uPrevTTL=0;//Has to be reset every new cZoneName
	unsigned uTTL=0;
	static char cPrevOrigin[100]={""};
	char *cp;
	char cResourceImportTable[256]="tResourceImport";

	GetConfiguration("cResourceImportTable",cResourceImportTable,0);


	if(strcmp(cZoneName,cPrevZoneName))
	{
		//printf("New zone %s\n",cZoneName);
		sprintf(cPrevZoneName,"%.99s",cZoneName);
		uPrevTTL=0;
		cPrevOrigin[0]=0;
		cPrevcName[0]=0;
	}
	
	if((cp=strchr(cLine,';')))
	{
		*cp='\n';
		*(cp+1)=0;
	}


	//debug only
	//printf("<u>%s</u>\n",cLine);

	if(cLine[0]=='\t')
	{
		sscanf(cLine,"%100s %255s %255s %255s %255s\n",
			cType,cParam1,cParam2,cParam3,cParam4);
		if(cPrevcName[0])
			sprintf(cName,"%.99s",cPrevcName);
		else	
			strcpy(cName,"\t");
	}
	else if(cLine[0]==' ')
	{
		sscanf(cLine,"%99s %100s %255s %255s %255s %255s\n",
			cName,cType,cParam1,cParam2,cParam3,cParam4);
		sprintf(cName,"@");
		sprintf(cPrevcName,"%.99s",cName);
	}
	else if(cLine[0]=='@')
	{
		sscanf(cLine,"@ %100s %255s %255s %255s %255s\n",
			cType,cParam1,cParam2,cParam3,cParam4);
		if(cPrevcName[0])
			sprintf(cName,"%.99s",cPrevcName);
		else	
			strcpy(cName,"\t");
	}
	else
	{
		sscanf(cLine,"%99s %100s %255s %255s %255s %255s\n",
			cName,cType,cParam1,cParam2,cParam3,cParam4);
		sprintf(cPrevcName,"%.99s",cName);
	}

	if(!cLine[0] || cLine[0]=='\n')
			return;


	if(cName[0]!='$')
	{
		//Shift left on inline TTL NOT $TTL directive
		sscanf(cType,"%u",&uTTL);
		if(uTTL>1 && uTTL<800000)
		{
			strcpy(cType,cParam1);
			strcpy(cParam1,cParam2);
			strcpy(cParam2,cParam3);
			strcpy(cParam3,cParam4);
		}
	}

	//Shift left on IN
	if(!strcasecmp(cType,"IN"))
	{
		strcpy(cType,cParam1);
		strcpy(cParam1,cParam2);
		strcpy(cParam2,cParam3);
		strcpy(cParam3,cParam4);
	}


	//Check for recognized data and verify needed params
	if(!strcasecmp(cType,"A"))
	{
		uRRType=1;
		if(!cParam1[0] || cParam2[0])
		{
			fprintf(stdout,"ProcessRRLine() Error %s: Incorrect A format: %s\n",
					cZoneName,cLine);
			return;
		}
		if(!strcmp(IPNumber(cParam1),"0.0.0.0"))
		{
			printf("Incorrect A IP number format: %s\n",cLine);
			return;
		}
	}
	else if(!strcasecmp(cType,"CNAME"))
	{
		char cZone[254];
		char cName[254];
		uRRType=5;
		if(!cParam1[0] || cParam2[0])
		{
			fprintf(stdout,"ProcessRRLine() Error %s: Incorrect CNAME format: %s\n",
					cZoneName,cLine);
			return;
		}
		if(cParam1[strlen(cParam1)-1]!='.')
		{
			printf("Warning: Incorrect FQDN missing period: %s\n",cParam1);
			if(cPrevOrigin[0])
			{
				sprintf(gcQuery,"%.255s.%.99s",cParam1,cPrevOrigin);
				sprintf(cParam1,"%.255s",gcQuery);
				printf("Fixed: CNAME RHS fixed from $ORIGIN: %s\n",cParam1);
			}
			else
			{
				sprintf(gcQuery,"%.255s.%.99s.",cParam1,cZoneName);
				sprintf(cParam1,"%.255s",gcQuery);
				printf("Fixed: CNAME RHS fixed from cZoneName: %s\n",cParam1);
			}
		}
		sprintf(cZone,"%s.",FQDomainName(cZoneName));
		strcpy(cName,FQDomainName(cParam1));
		if(strcmp(cName+strlen(cName)-strlen(cZone),cZone))
			printf("Warning: CNAME RHS should probably end with zone: %s\n",cLine);
	}
	else if(!strcasecmp(cType,"NS"))
	{
		MYSQL_RES *res;
		char cNS[100];

		uRRType=2;
		if(!cParam1[0] || cParam2[0])
		{
			fprintf(stdout,"ProcessRRLine() Error %s: Incorrect NS format: %s\n",
					cZoneName,cLine);
			return;
		}
		if(cParam1[strlen(cParam1)-1]!='.')
		{
			fprintf(stdout,"Warning %s: Incorrect FQDN missing period: %s\n",
					cZoneName,cLine);
			if(cPrevOrigin[0])
			{
				sprintf(gcQuery,"%.255s.%.99s",cParam1,cPrevOrigin);
				sprintf(cParam1,"%.255s",gcQuery);
				printf("Fixed: NS RHS fixed from $ORIGIN: %s\n",cParam1);
			}
			else
			{
				sprintf(gcQuery,"%.255s.%.99s.",cParam1,cZoneName);
				sprintf(cParam1,"%.255s",gcQuery);
				printf("Fixed: NS RHS fixed from cZoneName: %s\n",cParam1);
			}
		}

		//Get rid of last period for check.
		strcpy(cNS,cParam1);
		cNS[strlen(cNS)-1]=0;
		sprintf(gcQuery,"SELECT uNS FROM tNS WHERE uNSSet=%u AND cFQDN LIKE '%s' ",
					uNSSet,cNS);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql)) 
		{
			fprintf(stdout,"ProcessRRLine() Error %s: %s\n",cZoneName,mysql_error(&gMysql));
			return;
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)) 
		{
			fprintf(stdout,"ProcessRRLine() warning %s:NS RR Ignored. NS belongs to current uNSSet=%u\n",
					cZoneName,uNSSet);
			return;
		}
		mysql_free_result(res);
	}
	else if(!strcasecmp(cType,"MX"))
	{
		unsigned uMX=0;
		uRRType=3;
		if(!cParam1[0] || !cParam2[0] )
		{
			fprintf(stdout,"ProcessRRLine() Error %s: Missing MX param: %s\n",
					cZoneName,cLine);
			return;
		}
		sscanf(cParam1,"%u",&uMX);
		if(uMX<1 || uMX>99999)
		{
			fprintf(stdout,"ProcessRRLine() Error %s: Incorrect MX format: %s\n",
					cZoneName,cLine);
			return;
		}
		if(cParam2[strlen(cParam2)-1]!='.')
		{
			fprintf(stdout,"Warning %s: Incorrect FQDN missing period: %s\n",
					cZoneName,cLine);
			if(cPrevOrigin[0])
			{
				sprintf(gcQuery,"%.255s.%.99s",cParam2,cPrevOrigin);
				sprintf(cParam2,"%.255s",gcQuery);
				printf("Fixed: MX RHS fixed from $ORIGIN: %s\n",cParam2);
			}
			else
			{
				sprintf(gcQuery,"%.255s.%.99s.",cParam2,cZoneName);
				sprintf(cParam2,"%.255s",gcQuery);
				printf("Fixed: MX RHS fixed from cZoneName: %s\n",cParam2);
			}
		}
	}
	else if(!strcasecmp(cType,"PTR"))
	{
		unsigned uFirstDigit=0;

		uRRType=7;
		if(!cParam1[0] || cParam2[0])
		{
			fprintf(stdout,"ProcessRRLine() Error %s: Incorrect PTR format: %s\n",
					cZoneName,cLine);
			return;
		}
		sscanf(cName,"%u.%*s",&uFirstDigit);
		if(!uFirstDigit)
		{
			//Check this rule again
			fprintf(stdout,"ProcessRRLine() Error %s: Incorrect PTR LHS should start with a non zero digit: %s\n",
				cZoneName,cLine);
			return;
		}

	}
	else if(!strcasecmp(cType,"TXT"))
	{
		uRRType=6;
		if((cp=strchr(cLine,'"')))
			sprintf(cParam1,"%.255s",cp);
		//Adjust for cr/lf also
		if(!cParam1[0] || cParam1[0]!='\"' || (cParam1[strlen(cParam1)-2]!='\"' &&
					cParam1[strlen(cParam1)-1]!='\"') )
		{
			fprintf(stdout,"ProcessRRLine() Error %s: Incorrect TXT format: %s\n",
					cZoneName,cParam1);
			return;
		}
		//debug only
		printf("TXT: %s\n",cParam1);
	}

	//Special cases that should keep a static var for next line
	else if(!strcasecmp(cName,"$TTL"))
	{
		if(cType[0])
		{
			sscanf(cType,"%u",&uPrevTTL);
			//debug only
			printf("$TTL changed: %u (%s %s)\n",
					uPrevTTL,cType,cParam1);
			return;
		}
	}
	else if(!strcasecmp(cName,"$ORIGIN"))
	{
		if(cType[0] && strcmp(cType,".") && cType[strlen(cType)-1]=='.')
			sprintf(cPrevOrigin,"%.99s",cType);
		{
			cPrevcName[0]=0;//Reset?
			if(!strncasecmp(cPrevOrigin,cZoneName,strlen(cPrevOrigin)-1))
				cPrevOrigin[0]=0;
			else
				//debug only
				printf("$ORIGIN Changed: %s\n",
					cPrevOrigin);
			return;
		}
	}

	//Unrecognized lines
	//Current missing features -we know about and need: hinfo ignored
	else if(1 && cLine[0] && cLine[0]!='\n')
	{
		fprintf(stdout,"ProcessRRLine() Error %s: RR Not recognized: %s %s\n",cZoneName,cLine,cType);
		return;
	}

	if(!uTTL && uPrevTTL) uTTL=uPrevTTL;
	if(cPrevOrigin[0]) 
		sprintf(cNamePlus,"%.99s.%.99s",cName,cPrevOrigin);
	else
		sprintf(cNamePlus,"%.99s",cName);
	if(uRRType==6)//TXT special case
	sprintf(gcQuery,"INSERT INTO %s SET  uZone=%u,cName='%s',uTTL=%u,uRRType=%u,cParam1='%s',"
			"cComment='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
					,cResourceImportTable
					,uZone
					,FQDomainName(cNamePlus)
					,uTTL
					,uRRType
					,cParam1
					,cComment
					,uCustId
					,uCreatedBy);
	else
	sprintf(gcQuery,"INSERT INTO %s SET uZone=%u,cName='%s',uTTL=%u,uRRType=%u,cParam1='%s',"
			"cParam2='%s',cComment='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
					,cResourceImportTable
					,uZone
					,FQDomainName(cNamePlus)
					,uTTL
					,uRRType
					,FQDomainName(cParam1)
					,FQDomainName(cParam2)
					,cComment
					,uCustId
					,uCreatedBy);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
		fprintf(stdout,"ProcessRRLine() Error %s: %s\n",cZoneName,mysql_error(&gMysql));

	return;

}//void ProcessRRLine()


void Import(void)
{
	struct dirent **namelist;
	structSOA *importSOA;
	register int n,i;
	char cHostMaster[256]="hostmaster.isp.net";
	char cuView[256]="2";
	char cuNSSet[256]="0";
	char cuOwner[256]="1";
	char cZoneImportTable[256]="tZoneImport";

	printf("ImportZones() Start\n");

	GetConfiguration("cHostMaster",cHostMaster,0);
	GetConfiguration("cuView",cuView,0);
	GetConfiguration("cZoneImportTable",cZoneImportTable,0);
	GetConfiguration("cuNSSet",cuNSSet,0);
	GetConfiguration("cuOwner",cuOwner,0);

	printf("Importing master zones from /usr/local/idns/import directory.\n"
		"File name must be same as zone name.\nOnly A, CNAME, MX, PTR and"
		" NS records supported.\nSOA ttl values must be seconds.\n"
		"You can also set cHostMaster, cuNSSet, cuView, cuOwner and cZoneImportTable in\n"
		"tConfiguration. Current defaults:\n"
		"cHostMaster=%s\n"
		"cuNSSet=%s (if set to zero will try to determine from SOA or fallback to uNSSet=1)\n"
		"cuView=%s\n"
		"cuOwner=%s\n"
		"cZoneImportTable=%s\n\n",cHostMaster,cuNSSet,cuView,cuOwner,cZoneImportTable);
	printf("Confirm <enter any key>, <ctrl-c> to abort\n");
	getchar();
		
	n=scandir("/usr/local/idns/import",&namelist,0,0);
			
	if(n<0)
	{
		fprintf(stdout,"scandir() error: Does /usr/local/idns/import exist?\n");
		return;
	}
	else if(n==2)
	{
		fprintf(stdout,"No files found.\n");
		return;
	}

	for(i=0;i<n;i++)
	{

		//Added some end of list test hack a long time ago. Remove?
		if(namelist[i]->d_name[0]=='.' || 
			strstr(namelist[i]->d_name+strlen(namelist[i]->d_name)-5,
				".done"))
		{
			;
		}
		else
		{
			FILE *fp;

			sprintf(gcQuery,"/usr/local/idns/import/%.100s",
					namelist[i]->d_name);
			if(!(fp=fopen(gcQuery,"r")))
			{
				fprintf(stdout,"Error: Could not open: %s\n",gcQuery);
				return;
			}

			//Start processing
			printf("%.100s\n\n",namelist[i]->d_name);
	
			//Major import component
			importSOA=ProcessSOA(fp);

			//Add more rule based checking of TTLs	
			if(	importSOA->uTTL &&
				importSOA->uSerial &&
				importSOA->uRefresh &&
				importSOA->uRetry &&
				importSOA->uExp &&
				importSOA->uNegTTL &&
				importSOA->cHostmaster[0] )
			{
				//All zones will belong to the default #1 NS
				unsigned uZone,uOwner=1,uNSSet=0;

				sscanf(cuOwner,"%u",&uOwner);
				sscanf(cuNSSet,"%u",&uNSSet);

				//Import zone
				importSOA->cNameServer[strlen(importSOA->cNameServer)-1]=0;
				importSOA->cHostmaster[strlen(importSOA->cHostmaster)-1]=0;
				//debug only
				//printf("uTTL=%u\n",importSOA->uTTL);
				//printf("uSerial=%u\n",importSOA->uSerial);
				//printf("uRefresh=%u\n",importSOA->uRefresh);
				//printf("uRetry=%u\n",importSOA->uRetry);
				//printf("uExp=%u\n",importSOA->uExp);
				//printf("uNegTTL=%u\n",importSOA->uNegTTL);
				//printf("cHostmaster=%s\n",importSOA->cHostmaster);
				//printf("cNameServer=%s\n",importSOA->cNameServer);

				//Try to use an existing tNSSet
				if(uNSSet==0)
				{
					uNSSet=uGetNSSet(importSOA->cNameServer);
					
					if(uNSSet==0)
						uNSSet=1;//First tNSSet
					else
						printf("Using uNSSet=%u determined via:%s\n",
								uNSSet,importSOA->cNameServer);
				}
				else
				{
					uNSSet=1;//First tNSSet
				}


				//uZoneTTL is the NegTTL
				sprintf(gcQuery,"INSERT INTO %s SET cZone='%s',uNSSet=%u,uSerial=%u,"
					"uExpire=%u,uRefresh=%u,uTTL=%u,uRetry=%u,uZoneTTL=%u,uMailServers=0,"
					"cMainAddress='0.0.0.0',uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW()),"
					"uModBy=0,uModDate=0,cHostmaster='%s IMPORTED',uView=%.2s"
					,cZoneImportTable
					,FQDomainName(namelist[i]->d_name)
					,uNSSet
					,importSOA->uSerial
					,importSOA->uExp
					,importSOA->uRefresh
					,importSOA->uTTL
					,importSOA->uRetry
					,importSOA->uNegTTL
					,uOwner
					,cHostMaster
					,cuView);
					//,importSOA->cHostmaster);
	
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql)) 
				{
					fprintf(stdout,"Error %s: %s\n",
							namelist[i]->d_name,mysql_error(&gMysql));
					if(!strncmp("Duplicate entry",mysql_error(&gMysql),15))
					{
						MYSQL_RES *res4;
						MYSQL_ROW field4;

						
						uZone=0;
						uOwner=0;
						sprintf(gcQuery,
							"SELECT uZone,uOwner FROM %s WHERE cZone='%s'",
						cZoneImportTable,
						FQDomainName(namelist[i]->d_name));
						mysql_query(&gMysql,gcQuery);
						res4=mysql_store_result(&gMysql);
						if((field4=mysql_fetch_row(res4)))
						{
							sscanf(field4[0],"%u",&uZone);
							sscanf(field4[1],"%u",&uOwner);
						}
						mysql_free_result(res4);
						if(uZone)
						{
							fprintf(stdout,"Warning %s: Adding RR to a zone"
								" that already exists!\n",namelist[i]->d_name);
							goto AddToExisting;
						}
					}

					continue;
				}
				uZone=mysql_insert_id(&gMysql);

AddToExisting:
				if(!uOwner) uOwner=1;
				//Process rest of file for RRs
				//printf("Importing RRs\n");
				while(fgets(gcQuery,254,fp)!=NULL)
				{
				  //skip empty lines
				  if(gcQuery[0]!='\n')
					ProcessRRLine(gcQuery,
						FQDomainName(namelist[i]->d_name),
						uZone,uOwner,uNSSet,1," IMPORTED");
				}
			}
			else
			{
				fprintf(stdout,"Error %s:\nCould not process SOA correctly\n",
						namelist[i]->d_name);
				printf("uTTL=%u\n",importSOA->uTTL);
				printf("uSerial=%u\n",importSOA->uSerial);
				printf("uRefresh=%u\n",importSOA->uRefresh);
				printf("uRetry=%u\n",importSOA->uRetry);
				printf("uExp=%u\n",importSOA->uExp);
				printf("uNegTTL=%u\n",importSOA->uNegTTL);
				printf("cHostmaster=%s\n",importSOA->cHostmaster);
				printf("cNameServer=%s\n",importSOA->cNameServer);
				return;
			}
			fclose(fp);

		}//If valid file for importing

	}//for each file in import dir
	printf("\nImportZones() Done\n");

}//void Import(void)


void ImportSORRs(void)
{
	//
	//This function import the zone RRs for secondary service only zones
	//into the iDNS database.
	//These records will be read only, which will be limited at interface level.
	//The code here is heavily based on the code of the Import() function above.
	//
	//Steps:
	//1. Query the database to find out the secondary service only zones
	//2. Once we have the zone name, we build the zone file path using:
	//   iDNSRootFolder/named.d/slave/[external|internal]/(cZone 1st char)/cZone
	//3. We open the file
	//4. Must remove all existing RRs at the zone, we are updating!
	//5. We run a modified version of the loop in the Import() function that imports the zone RRs by 
	//calling ProcessRRLine()

	MYSQL_RES *res;
	MYSQL_ROW field;
	char cFileName[512]={""};
	char *cView="";
	FILE *fp;
	unsigned uZone=0;
	unsigned uOwner=0;
	unsigned uNSSet=0;
	structSOA *importSOA;
	

	printf("ImportSORRs() Start\n");
	
	//1. Query the database to find out the secondary service only zones
	sprintf(gcQuery,"SELECT cZone,uView,uZone,uOwner,uNSSet FROM tZone WHERE uSecondaryOnly=1 ORDER BY uZone");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,"Error: %s\n",mysql_error(&gMysql));
		exit(1);
	}
	
	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		//2. Once we have the zone name, we build the zone file path
		if(!strcmp(field[1],"1"))
			cView="internal";
		else
			cView="external";
		
		sprintf(cFileName,"/usr/local/idns/named.d/slave/%s/%c/%s",
				cView
				,field[0][0]
				,field[0]
		       );
		//3. We open the file
		if((fp=fopen(cFileName,"r"))==NULL)
		{
			fprintf(stderr,"Warning: could not open zonefile for zone %s, path was %s\n",
				field[0],cFileName);
			continue;
		}
		
		//4. Must remove all existing RRs at the zone, we are updating!
		
		sscanf(field[2],"%u",&uZone);
		sscanf(field[3],"%u",&uOwner);
		sscanf(field[4],"%u",&uNSSet);
	
		sprintf(gcQuery,"DELETE FROM tResource WHERE uZone='%u'",uZone);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stderr,"Error: %s\n",mysql_error(&gMysql));
			exit(1);
		}

		//5. We run a modified version of the loop in the Import() function
		importSOA=ProcessSOA(fp); //Unused, just to move file pointer to 'after soa position'
		
		while(fgets(gcQuery,254,fp)!=NULL)
		{
		  //skip empty lines
		  if(gcQuery[0]!='\n')
			ProcessSORRLine(gcQuery,field[0],uZone,uOwner,uNSSet,1," SecOnly IMPORTED");
		}
	}//while((field=mysql_fetch_row(res)))

	printf("ImportSORRs() End\n");
	
}//void ImportSORRs(void)


void ProcessSORRLine(const char *cLine,char *cZoneName,const unsigned uZone,
			const unsigned uCustId,const unsigned uNSSet,
			const unsigned uCreatedBy,const char *cComment)
{
	char cName[100]={""};
	char cNamePlus[200]={""};
	char cParam1[256]={""};
	char cParam2[256]={""};
	char cParam3[256]={""};
	char cParam4[256]={""};
	char cType[256]={""};
	static char cPrevZoneName[100]={""};
	static char cPrevcName[100]={""};
	unsigned uRRType=0;
	static unsigned uPrevTTL=0;//Has to be reset every new cZoneName
	unsigned uTTL=0;
	static char cPrevOrigin[100]={""};
	char *cp;


	if(strcmp(cZoneName,cPrevZoneName))
	{
		//printf("New zone %s\n",cZoneName);
		sprintf(cPrevZoneName,"%.99s",cZoneName);
		uPrevTTL=0;
		cPrevOrigin[0]=0;
		cPrevcName[0]=0;
	}
	
	if((cp=strchr(cLine,';')))
	{
		*cp='\n';
		*(cp+1)=0;
	}


	//debug only
	//printf("<u>%s</u>\n",cLine);

	if(cLine[0]=='\t')
	{
		sscanf(cLine,"%100s %255s %255s %255s %255s\n",
			cType,cParam1,cParam2,cParam3,cParam4);
		if(cPrevcName[0])
			sprintf(cName,"%.99s",cPrevcName);
		else	
			strcpy(cName,"\t");
	}
	else if(cLine[0]=='@')
	{
		sscanf(cLine,"@ %100s %255s %255s %255s %255s\n",
			cType,cParam1,cParam2,cParam3,cParam4);
		if(cPrevcName[0])
			sprintf(cName,"%.99s",cPrevcName);
		else	
			strcpy(cName,"\t");
	}
	else
	{
		sscanf(cLine,"%99s %100s %255s %255s %255s %255s\n",
			cName,cType,cParam1,cParam2,cParam3,cParam4);
		sprintf(cPrevcName,"%.99s",cName);
	}

	if(!cLine[0] || cLine[0]==';')
			return;

	if(cName[0]!='$')
	{
		//Shift left on inline TTL NOT $TTL directive
		sscanf(cType,"%u",&uTTL);
		if(uTTL>1 && uTTL<800000)
		{
			strcpy(cType,cParam1);
			strcpy(cParam1,cParam2);
			strcpy(cParam2,cParam3);
			strcpy(cParam3,cParam4);
		}
	}

	//Shift left on IN
	if(!strcasecmp(cType,"IN"))
	{
		strcpy(cType,cParam1);
		strcpy(cParam1,cParam2);
		strcpy(cParam2,cParam3);
		strcpy(cParam3,cParam4);
	}

	//Check for recognized data and verify needed params
	if(!strcasecmp(cType,"A"))
	{
		uRRType=1;
		if(!cParam1[0] || cParam2[0])
		{
			fprintf(stdout,"Error %s: Incorrect A format: %s\n",
					cZoneName,cLine);
			return;
		}
		if(!strcmp(IPNumber(cParam1),"0.0.0.0"))
		{
			printf("Incorrect A IP number format: %s\n",cLine);
			return;
		}
	}
	else if(!strcasecmp(cType,"CNAME"))
	{
		char cZone[254];
		char cName[254];
		uRRType=5;
		if(!cParam1[0] || cParam2[0])
		{
			fprintf(stdout,"Error %s: Incorrect CNAME format: %s\n",
					cZoneName,cLine);
			return;
		}
		if(cParam1[strlen(cParam1)-1]!='.')
		{
			printf("Warning: Incorrect FQDN missing period: %s\n",cParam1);
			if(cPrevOrigin[0])
			{
				sprintf(gcQuery,"%.255s.%.99s",cParam1,cPrevOrigin);
				sprintf(cParam1,"%.255s",gcQuery);
				printf("Fixed: CNAME RHS fixed from $ORIGIN: %s\n",cParam1);
			}
			else
			{
				sprintf(gcQuery,"%.255s.%.99s.",cParam1,cZoneName);
				sprintf(cParam1,"%.255s",gcQuery);
				printf("Fixed: CNAME RHS fixed from cZoneName: %s\n",cParam1);
			}
		}
		sprintf(cZone,"%s.",FQDomainName(cZoneName));
		strcpy(cName,FQDomainName(cParam1));
		if(strcmp(cName+strlen(cName)-strlen(cZone),cZone))
			printf("Warning: CNAME RHS should probably end with zone: %s\n",cLine);
	}
	else if(!strcasecmp(cType,"NS"))
	{
		MYSQL_RES *res;
		char cNS[100];

		uRRType=2;
		if(!cParam1[0] || cParam2[0])
		{
			fprintf(stdout,"Error %s: Incorrect NS format: %s\n",
					cZoneName,cLine);
			return;
		}
		if(cParam1[strlen(cParam1)-1]!='.')
		{
			fprintf(stdout,"Warning %s: Incorrect FQDN missing period: %s\n",
					cZoneName,cLine);
			if(cPrevOrigin[0])
			{
				sprintf(gcQuery,"%.255s.%.99s",cParam1,cPrevOrigin);
				sprintf(cParam1,"%.255s",gcQuery);
				printf("Fixed: NS RHS fixed from $ORIGIN: %s\n",cParam1);
			}
			else
			{
				sprintf(gcQuery,"%.255s.%.99s.",cParam1,cZoneName);
				sprintf(cParam1,"%.255s",gcQuery);
				printf("Fixed: NS RHS fixed from cZoneName: %s\n",cParam1);
			}
		}

		//Get rid of last period for check.
		strcpy(cNS,cParam1);
		cNS[strlen(cNS)-1]=0;
		sprintf(gcQuery,"SELECT uNS FROM tNS WHERE uNSSet=%u AND cFQDN LIKE '%s' ",
					uNSSet,cNS);
		mysql_query(&gMysql,gcQuery);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql)) 
		{
			fprintf(stdout,"Error %s: %s\n",cZoneName,mysql_error(&gMysql));
			return;
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res)) 
			//Silent return
			return;
		mysql_free_result(res);
	}
	else if(!strcasecmp(cType,"MX"))
	{
		unsigned uMX=0;
		uRRType=3;
		if(!cParam1[0] || !cParam2[0] )
		{
			fprintf(stdout,"Error %s: Missing MX param: %s\n",
					cZoneName,cLine);
			return;
		}
		sscanf(cParam1,"%u",&uMX);
	
		if(uMX>99999)
		{
			fprintf(stdout,"Error %s: Incorrect MX format: %s\n",
					cZoneName,cLine);
			return;
		}
		if(cParam2[strlen(cParam2)-1]!='.')
		{
			fprintf(stdout,"Warning %s: Incorrect FQDN missing period: %s\n",
					cZoneName,cLine);
			if(cPrevOrigin[0])
			{
				sprintf(gcQuery,"%.255s.%.99s",cParam2,cPrevOrigin);
				sprintf(cParam2,"%.255s",gcQuery);
				printf("Fixed: MX RHS fixed from $ORIGIN: %s\n",cParam2);
			}
			else
			{
				sprintf(gcQuery,"%.255s.%.99s.",cParam2,cZoneName);
				sprintf(cParam2,"%.255s",gcQuery);
				printf("Fixed: MX RHS fixed from cZoneName: %s\n",cParam2);
			}
		}
	}
	else if(!strcasecmp(cType,"PTR"))
	{
		unsigned uFirstDigit=0;

		uRRType=7;
		if(!cParam1[0] || cParam2[0])
		{
			fprintf(stdout,"Error %s: Incorrect PTR format: %s\n",
					cZoneName,cLine);
			return;
		}
		sscanf(cName,"%u.%*s",&uFirstDigit);
		if(!uFirstDigit)
		{
			//Check this rule again
			fprintf(stdout,"Error %s: Incorrect PTR LHS should start with"
					" a non zero digit: %s\n",
						cZoneName,cLine);
			return;
		}

	}
	else if(!strcasecmp(cType,"TXT"))
	{
		uRRType=6;
		if((cp=strchr(cLine,'"')))
			sprintf(cParam1,"%.255s",cp);
		//Adjust for cr/lf also
		if(!cParam1[0] || cParam1[0]!='\"' || (cParam1[strlen(cParam1)-2]!='\"' &&
					cParam1[strlen(cParam1)-1]!='\"') )
		{
			fprintf(stdout,"Error %s: Incorrect TXT format: %s\n",
					cZoneName,cParam1);
			return;
		}
		//debug only
		printf("TXT: %s\n",cParam1);
	}

	//Special cases that should keep a static var for next line
	else if(!strcasecmp(cName,"$TTL"))
	{
		if(cType[0])
		{
			sscanf(cType,"%u",&uPrevTTL);
			//debug only
			//printf("$TTL changed: %u (%s %s)\n",
			//		uPrevTTL,cType,cParam1);
			return;
		}
	}
	else if(!strcasecmp(cName,"$ORIGIN"))
	{
		if(cType[0] && strcmp(cType,".") && cType[strlen(cType)-1]=='.')
			sprintf(cPrevOrigin,"%.99s",cType);
		{
			cPrevcName[0]=0;//Reset?
			if(!strncasecmp(cPrevOrigin,cZoneName,strlen(cPrevOrigin)-1))
				cPrevOrigin[0]=0;
			else
				//debug only
				//printf("$ORIGIN Changed: %s\n",
				//	cPrevOrigin);
			return;
		}
	}

	//Unrecognized lines
	//Current missing features -we know about and need: hinfo ignored
	else if(1)
	{
		fprintf(stdout,"Error %s: RR Not recognized: %s %s\n",cZoneName,cLine,cType);
		return;
	}

	if(!uTTL && uPrevTTL) uTTL=uPrevTTL;
	if(cPrevOrigin[0]) 
		sprintf(cNamePlus,"%.99s.%.99s",cName,cPrevOrigin);
	else
		sprintf(cNamePlus,"%.99s",cName);
	
	if(!uRRType) return; //NOP if not valid RR is found
	if(uRRType==6)//TXT special case
		sprintf(gcQuery,"INSERT INTO tResource SET uZone=%u,cName='%s',uTTL=%u,"
				"uRRType=%u,cParam1='%s',cComment='%s',uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uZone
					,FQDomainName(cNamePlus)
					,uTTL
					,uRRType
					,cParam1
					,cComment
					,uCustId
					,uCreatedBy);
	else
		sprintf(gcQuery,"INSERT INTO tResource SET uZone=%u,cName='%s',uTTL=%u,"
				"uRRType=%u,cParam1='%s',cParam2='%s',cComment='%s',uOwner=%u,"
				"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uZone
					,FQDomainName(cNamePlus)
					,uTTL
					,uRRType
					,FQDomainName(cParam1)
					,FQDomainName(cParam2)
					,cComment
					,uCustId
					,uCreatedBy);
	mysql_query(&gMysql,gcQuery);
	//printf("%s\n",gcQuery);
	if(mysql_errno(&gMysql)) 
		fprintf(stdout,"Error %s: %s\n",cZoneName,mysql_error(&gMysql));

	return;

}//void ProcessSORRLine()


void DropImportedZones(void)
{
	char cZoneImportTable[256]="tZoneImport";
	char cResourceImportTable[256]="tResourceImport";

	GetConfiguration("cZoneImportTable",cZoneImportTable,0);
	GetConfiguration("cResourceImportTable",cResourceImportTable,0);

	printf("DropImportedZones() Start\n");

	sprintf(gcQuery,"DELETE FROM %s WHERE cHostmaster LIKE '%% IMPORTED'",cZoneImportTable);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));

	sprintf(gcQuery,"DELETE FROM %s WHERE cComment LIKE '%% IMPORTED'",cResourceImportTable);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));

	printf("DropImportedZones() Done\n");

}//DropImportedZones()


structSOA *ProcessSOA(FILE *fp)
{
	static structSOA importSOA;
	register unsigned uIn=0;
	register unsigned uParamCount=0;
	char *cp;

	importSOA.uSerial=0;
	importSOA.uTTL=86400;
	importSOA.uRefresh=0;
	importSOA.uRetry=0;
	importSOA.uExp=0;
	importSOA.uNegTTL=0;
	importSOA.cHostmaster[0]=0;
	importSOA.cNameServer[0]=0;

	while(fgets(gcQuery,1024,fp)!=NULL)
	{
		if((cp=strchr(gcQuery,';')))
		{
			*cp='\n';
			*(cp+1)=0;
		}
		//debug only
		//printf("%s",gcQuery);

		if(strstr(gcQuery,"("))
		{
			if((cp=strstr(gcQuery,"SOA")) || (cp=strstr(gcQuery,"soa")))
				sscanf(cp+4,"%s %s",importSOA.cNameServer,
						importSOA.cHostmaster);
			uIn=1;
			continue;
		}
		else if(strstr(gcQuery,")"))
		{
			if(uIn && uParamCount==4)
				sscanf(gcQuery,"%u",&importSOA.uNegTTL);
			return(&importSOA);
		}
		else if(strstr(gcQuery,"$TTL"))
			sscanf(gcQuery,"$TTL %u",&importSOA.uTTL);
		else if((cp=strstr(gcQuery,"SOA")) || (cp=strstr(gcQuery,"soa")))
			sscanf(cp+4,"%s %s",importSOA.cNameServer,
						importSOA.cHostmaster);

		if(uIn)
		{
			if(!uParamCount)
				sscanf(gcQuery,"%u",&importSOA.uSerial);
			else if(uParamCount==1)
				sscanf(gcQuery,"%u",&importSOA.uRefresh);
			else if(uParamCount==2)
				sscanf(gcQuery,"%u",&importSOA.uRetry);
			else if(uParamCount==3)
				sscanf(gcQuery,"%u",&importSOA.uExp);
			else if(uParamCount==4)
				sscanf(gcQuery,"%u",&importSOA.uNegTTL);
			uParamCount++;
		}

		if(uParamCount>5)
			return(&importSOA);
	}
	
	return(&importSOA);

}//structSOA *ProcessSOA(FILE *fp)


//
// Major file section
//	Non import/migration mainfunc.h CLI functions and aux functions
//


char *GetRRType(unsigned uRRType)
{
	MYSQL_RES *res;
	static char cRRType[100];
	
	strcpy(cRRType,"unknown");

	sprintf(gcQuery,"SELECT cLabel FROM tRRType WHERE uRRType=%u",uRRType);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}
	res=mysql_store_result(&gMysql);
	
	if(mysql_num_rows(res)==1) 
	{
		MYSQL_ROW field;
		if((field=mysql_fetch_row(res)))
			strcpy(cRRType,field[0]);

	}
	mysql_free_result(res);
	return(cRRType);

}//char *GetRRType(unsigned uRRType)


char *cPrintNSList(FILE *zfp,char *cuNSSet)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	static char cFirstNS[100]={""};
	unsigned uFirst=1;

	//Do not include HIDDEN NSs. 2 is the fixed HIDDEN TYPE
	//This ORDER BY implies that people will use a NS scheme that is alphabetical
	//in nature like ns1, ns2 etc. This will need to be looked at later ;)
	sprintf(gcQuery,"SELECT tNS.cFQDN,tNS.uNSType FROM tNSSet,tNS WHERE tNSSet.uNSSet=tNS.uNSSet AND"
			" tNS.uNSType!=2 AND tNSSet.uNSSet=%s ORDER BY tNS.cFQDN",cuNSSet);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
			fprintf(zfp,"\t\tNS %s.\n",FQDomainName(field[0]));
			if(uFirst)
			{
				sprintf(cFirstNS,"%.99s",field[0]);
				uFirst=0;
			}
	}
	mysql_free_result(res);

	return(cFirstNS);

}//char *cPrintNSList()


//We still have not changed to a tMXSet based schema.
//And we may never since no one really needs to use this old tZone time saver feature.
void PrintMXList(FILE *zfp,char *cuMailServers)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT cList FROM tMailServer WHERE uMailServer=%s",
			cuMailServers);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}
	res=mysql_store_result(&gMysql);
	
	if(mysql_num_rows(res)==1) 
	{
		MYSQL_ROW field;
		register int i=0,j=0,uMX=10;

		if((field=mysql_fetch_row(res)))
		{
			//parse out
			while(field[0][i])
			{
				if(field[0][i]=='\n' || field[0][i]==0)
				{
					field[0][i]=0;
					if(strlen(FQDomainName(field[0]+j)))
					{	
						fprintf(zfp,"\t\tMX %d %s.\n",uMX,
							FQDomainName(field[0]+j));
					}
					i++;
					j=i;
					uMX+=10;
				}
				i++;
			}
			if(field[0][j] && strlen(FQDomainName(field[0]+j)))
			{	
				fprintf(zfp,"\t\tMX %d %s.\n",uMX,
					FQDomainName(field[0]+j));
			}
		}
	}
	mysql_free_result(res);

}//PrintMXList(FILE *fp,char *cuMailServers)


void CreateMasterFiles(char *cMasterNS, char *cZone, unsigned uModDBFiles, 
		unsigned uModStubs, unsigned uDebug)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	MYSQL_RES *res2;
	MYSQL_ROW field2;
	FILE *sfp=stdout;
	FILE *zfp=stdout;
	FILE *dnfp=NULL;
	unsigned uZone=0;
	unsigned uRRType=0;
	char cTTL[16]={""};
	unsigned uView=0,uCurrentView=0,uFirst=1;

	//printf("CreateMasterFiles() uModDBFiles=%u; uModStubs=%u\n",uModDBFiles,uModStubs);
	//0 cZone
	//1 uZone
	//2 uNSSet
	//3 cHostmaster
	//4 uSerial
	//5 uTTL
	//6 uExpire
	//7 uRefresh
	//8 uRetry
	//9 uZoneTTL
	//10 uMailServers
	//11 cMainAddress
	//12 tView.cLabel
	//13 tView.cMaster
	//14 tView.uView
	//15 tZone.cOptions
	

	if(uModStubs)
	{
		char cTSIGKeyName[256]={""};

		uView=0;
		uCurrentView=0;
		uFirst=1;

		//included stub zone files can only be all for new view based system

		sprintf(gcQuery,"SELECT DISTINCT tZone.cZone,tZone.uZone,tZone.uNSSet,tZone.cHostmaster,"
				"tZone.uSerial,tZone.uTTL,tZone.uExpire,tZone.uRefresh,tZone.uRetry,"
				"tZone.uZoneTTL,tZone.uMailServers,tZone.cMainAddress,tView.cLabel,"
				"tView.cMaster,tView.uView,tZone.cOptions FROM tZone,tNSSet,tNS,tView WHERE"
				" tZone.uNSSet=tNSSet.uNSSet AND tNSSet.uNSSet=tNS.uNSSet AND"
				" tZone.uView=tView.uView AND"
				" tNS.uNSType<4 AND tZone.uSecondaryOnly=0 ORDER BY" //4 is SLAVE last in fixed table
				" tView.uOrder,tZone.cZone");
		//debug only
		//printf("(uModStubs)%s\n",gcQuery);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql)) 
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}
		res=mysql_store_result(&gMysql);
	
		if(mysql_num_rows(res)<1) 
		{
			printf("No zones found for master NS %s\n",cMasterNS);
			return;
		}

		if(!uDebug)
		{
			if(!(sfp=fopen("/usr/local/idns/named.d/master.zones","w")))
			{
				fprintf(stderr,"Could not open master.zones\n");
				exit(1);
			}
		}
		else
		{
			printf("(/usr/local/idns/named.d/master.zones)\n");
		}

		//Only once for all zones. TODO this is a limitation.
		GetConfiguration("cTSIGKeyName",cTSIGKeyName,0);

		while((field=mysql_fetch_row(res)))
		{
			sscanf(field[1],"%u",&uZone);
			sscanf(field[14],"%u",&uView);

			if(uCurrentView!=uView)
			{
				char cExtraForViewName[100]={""};
				char cExtraForView[1024]={""};

				if(!uFirst)
					fprintf(sfp,"};\n");
				uFirst=0;
				uCurrentView=uView;
				fprintf(sfp,"\n%s\n",field[13]);

				sprintf(cExtraForViewName,"cExtraForViewMaster:%.31s",field[12]);
				GetConfiguration(cExtraForViewName,cExtraForView,0);
				if(cExtraForView[0])
					fprintf(sfp,"%s\n",cExtraForView);

			}

			//write stub into include file		
			fprintf(sfp,"\n\tzone \"%s\" {\n",field[0]);
			fprintf(sfp,"\t\ttype master;\n");
			if(field[15][0])
				fprintf(sfp,"\t\t//tZone.cOptions\n\t\t%s\n",field[15]);
			if(cTSIGKeyName[0])
			{
				fprintf(sfp,"\t\tallow-transfer { key %.99s.;};\n",
						cTSIGKeyName);
				fprintf(sfp,"\t\tallow-update { key %.99s.;};\n",
						cTSIGKeyName);
			}
			fprintf(sfp,"\t\tfile \"master/%s/%c/%s\";\n\t};\n",
					field[12],field[0][0],field[0]);

		}
		fprintf(sfp,"};\n");
	}
	if(sfp && !uDebug) fclose(sfp);

	//db files can be single or all
	if(uModDBFiles)
	{
		char cuUID[256]={""};
		char cuGID[256]={""};
		unsigned uGID=25;
		unsigned uUID=25;
#ifdef EXPERIMENTAL
		char cZoneFile[512]={""};
		unsigned uZoneOwner=0;
#endif
		GetConfiguration("cuUID",cuUID,0);
		if(cuUID[0]) sscanf(cuUID,"%u",&uUID);
		GetConfiguration("cuGID",cuGID,0);
		if(cuGID[0]) sscanf(cuGID,"%u",&uGID);

		if(cZone[0])	
		{
			sprintf(gcQuery,"SELECT DISTINCT tZone.cZone,tZone.uZone,tZone.uNSSet,tZone.cHostmaster,"
			"tZone.uSerial,tZone.uTTL,tZone.uExpire,tZone.uRefresh,tZone.uRetry,tZone.uZoneTTL,"
			"tZone.uMailServers,tZone.cMainAddress,tView.cLabel FROM tZone,tNSSet,tNS,tView"
			" WHERE tZone.uNSSet=tNSSet.uNSSet AND tNSSet.uNSSet=tNS.uNSSet AND"
			" tZone.uView=tView.uView AND tZone.cZone='%s'"
							,cZone);
		}
		else
		{
			sprintf(gcQuery,"SELECT DISTINCT tZone.cZone,tZone.uZone,tZone.uNSSet,tZone.cHostmaster,"
			"tZone.uSerial,tZone.uTTL,tZone.uExpire,tZone.uRefresh,tZone.uRetry,tZone.uZoneTTL,"
			"tZone.uMailServers,tZone.cMainAddress,tView.cLabel FROM tZone,tNSSet,tNS,tView"
			" WHERE tZone.uNSSet=tNSSet.uNSSet AND tNSSet.uNSSet=tNS.uNSSet AND"
			" tZone.uView=tView.uView ORDER BY tZone.cZone");
		}

		mysql_query(&gMysql,gcQuery);
		//debug only
		//printf("(uModDBFiles)%s\n",gcQuery);
		if(mysql_errno(&gMysql)) 
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}
		
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			char *cp;
			char cFirstNS[100]={""};
	
			//write master db file. first create dir if needed (fail ok)
			sprintf(gcQuery,"/usr/local/idns/named.d/master/%s",
					field[12]);
			if(!uDebug)
			{
				mkdir(gcQuery,0777);
				chown(gcQuery,uUID,uGID);
			}

			sprintf(gcQuery,"/usr/local/idns/named.d/master/%s/%c",
					field[12],field[0][0]);
			if(!uDebug)
			{
				mkdir(gcQuery,0777);
				chown(gcQuery,uUID,uGID);
			}

			if((cp=strchr(field[0],'/')))
			{
				*cp=0;
				sprintf(gcQuery,"/usr/local/idns/named.d/master/%s/%c/%s",
					field[12],field[0][0],field[0]);
				if(!uDebug)
				{
					mkdir(gcQuery,0777);
					chown(gcQuery,uUID,uGID);
				}
				*cp='/';
			}

			sprintf(gcQuery,"/usr/local/idns/named.d/master/%s/%c/%s",
					field[12],field[0][0],field[0]);
#ifdef EXPERIMENTAL
			sprintf(cZoneFile,"%.511s",gcQuery);
#endif
			if(!uDebug)
			{
				if(!(zfp=fopen(gcQuery,"w")))
				{
					fprintf(stderr,"Could not open master zone db file: %s\n",
						gcQuery);
					exit(1);
				}
			}
			else
			{
				printf("\n(%s)\n",gcQuery);
			}
			if(!dnfp)
			{
				if(!(dnfp=fopen("/dev/null","a")))
				{
					fprintf(stderr,"Could not open /dev/null file\n");
					exit(1);
				}
			}

			//0 cZone
			//1 uZone
			//2 uNSSet
			//3 cHostmaster
			//4 uSerial
			//
			//5 uTTL default TTL macro directive $TTL. See bind 8.2.x docs
			//
			//6 uExpire
			//7 uRefresh
			//8 uRetry
			//9 uZoneTTL is the negative response caching value in the SOA.
			//  See bind 8.2.x docs
			//
			//10 uMailServers
			//11 cMainAddress
			//12 tView.cLabel
			//13 tView.cMaster
			//14 tView.uOrder
			sscanf(field[1],"%u",&uZone);
	
			if((cp=strchr(field[3],' '))) *cp=0;

			fprintf(zfp,"; %s\n",field[0]);
			fprintf(zfp,"$TTL %s\n",field[5]);
	
			//MASTER HIDDEN support
			sprintf(cFirstNS,"%.99s",cPrintNSList(dnfp,field[2]));
			if(cFirstNS[0])
				fprintf(zfp,
				"@ IN SOA %s. %s. (\n",cFirstNS,field[3]);
			else
				fprintf(zfp,
				"@ IN SOA %s. %s. (\n",cMasterNS,field[3]);
			fprintf(zfp,"\t\t\t%s\t;serial\n",field[4]);
			fprintf(zfp,"\t\t\t%s\t\t;slave refresh\n",field[7]);
			fprintf(zfp,"\t\t\t%s\t\t;slave retry\n",field[8]);
			fprintf(zfp,"\t\t\t%s\t\t;slave expiration\n",field[6]);
			fprintf(zfp,"\t\t\t%s )\t\t;negative ttl\n\n",
				field[9]);

			//ns
			cPrintNSList(zfp,field[2]);

			//mx1
			PrintMXList(zfp,field[10]);
		
			//in a 0.0.0.0 is the null IP number
			if(field[11][0] && field[11][0]!='0')
				fprintf(zfp,"\t\tA %s\n;\n",field[11]);
			fprintf(zfp,";\n");

			//TODO
			if(!strcmp(field[0]+strlen(field[0])-5,".arpa"))
sprintf(gcQuery,"SELECT cName,uTTL,uRRType,cParam1,cParam2 FROM tResource WHERE uZone=%u ORDER BY uResource",uZone);
			else
sprintf(gcQuery,"SELECT cName,uTTL,uRRType,cParam1,cParam2,cParam3,cParam4 FROM tResource WHERE uZone=%u ORDER BY cName",uZone);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql)) 
			{
				fprintf(stderr,"%s\n",mysql_error(&gMysql));
				exit(1);
			}
			res2=mysql_store_result(&gMysql);
			while((field2=mysql_fetch_row(res2)))
			{
				sscanf(field2[2],"%u",&uRRType);
				if(field2[1][0]!='0') strcpy(cTTL,field2[1]);
				//Do not write TTL if cName is a $GENERATE line
				if(strstr(field2[0],"$GENERATE")==NULL)
				{
					if(strcmp(GetRRType(uRRType),"SRV"))
						fprintf(zfp,"%s\t%s\t%s\t%s\t%s\n",
								field2[0],
								cTTL,
								GetRRType(uRRType),
								field2[3],
								field2[4]);
					else
						fprintf(zfp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
								field2[0],
								cTTL,
								GetRRType(uRRType),
								field2[3],
								field2[4],
								field2[5],
								field2[6]);
				}
				else
					fprintf(zfp,"%s\t%s\t%s\t%s\n",
							field2[0],
							GetRRType(uRRType),
							field2[3],
							field2[4]);
			}
			mysql_free_result(res2);
#ifdef EXPERIMENTAL
			//Here's when this EXPERIMENTAL code starts to make sense.
			//Does it?
			//If defined, the software will run a named-checkzone for the zone
			//In case an issue is found, a tLog record will be created in order
			//to report to the user via interface func() procedure.
			//The format of the tLog entry will be:
			//tLog.uTablePK=tZone.uZone
			//tLog.cTableName="tZone"
			//tLog.cMessage="Zone with errors"
			//tLog.uOwner will match the tClient.uClient value of the company that owns the zone

			uZoneOwner=uGetZoneOwner(uZone);
			sprintf(gcQuery,"%s/named-checkzone %s %s > /dev/null 2>&1",gcBinDir,field[0],cZoneFile);
			printf("Running:%s\n",gcQuery);
			if(system(gcQuery))
			{
				//Command failed, create tLog entry
				sprintf(gcQuery,"INSERT INTO tLog SET uLogType=1,uPermLevel=12,uLoginClient=1,"
					"cLogin='JobQueue',cHost ='127.0.0.1',uTablePK='%u',cTableName='tZone',"
					"cMessage='Zone with errors',cServer='%s',uOwner=%u,uCreatedBy=1,"
					"uCreatedDate=UNIX_TIMESTAMP(NOW()),cLabel='named-checkzone %.32s'",
						uZone,cMasterNS,uZoneOwner,field[0]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
			}
#endif
			if(zfp && !uDebug) fclose(zfp);
		}
		mysql_free_result(res);

	}//if uModDBFile

	if(dnfp) fclose(dnfp);

}//void CreateMasterFiles()


void CreateSlaveFiles(char *cSlaveNS, char *cZone, char *cMasterIP, unsigned uDebug)
{
	//Create all slave zone stubs in include file.
	MYSQL_RES *res;
	MYSQL_ROW field;
	FILE *fp=stdout;
	unsigned uView=0,uCurrentView=0,uFirst=1;
	char cViewXIP[256]={""};
	char cViewXIPName[100]={""};
	char cuUID[256]={""};
	char cuGID[256]={""};
	unsigned uGID=25;
	unsigned uUID=25;

	GetConfiguration("cuUID",cuUID,0);
	if(cuUID[0]) sscanf(cuUID,"%u",&uUID);
	GetConfiguration("cuGID",cuGID,0);
	if(cuGID[0]) sscanf(cuGID,"%u",&uGID);

	printf("CreateSlaveFiles(%s)\n",cSlaveNS);

	sprintf(gcQuery,"SELECT DISTINCT tZone.cZone,tView.cLabel,tView.cSlave,tView.uView,tNSSet.cMasterIPs,"
			"tZone.cOptions,tZone.uSecondaryOnly FROM tZone,tNSSet,tNS,tView WHERE"
			" tZone.uNSSet=tNSSet.uNSSet AND tNSSet.uNSSet=tNS.uNSSet AND"
			" tNS.uNSType=4 AND tZone.uView=tView.uView AND tNS.cFQDN='%s'"
			" ORDER BY tView.uOrder,tZone.cZone",cSlaveNS);
	mysql_query(&gMysql,gcQuery);
	//debug only
	//printf("%s\n",gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		fprintf(stderr,"%s\n",mysql_error(&gMysql));
		exit(1);
	}
	res=mysql_store_result(&gMysql);
	
	if(mysql_num_rows(res)<1) 
	{
		printf("No zones found for slave NS %s\n",cSlaveNS);
		return;
	}

	if(!uDebug)
	{
		if(!(fp=fopen("/usr/local/idns/named.d/slave.zones","w")))
		{
			fprintf(stderr,"Could not open slave.zones\n");
			exit(1);
		}
	}

	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[3],"%u",&uView);
		if(uCurrentView!=uView)
		{
			char cExtraForViewName[100]={""};
			char cExtraForView[1024]={""};

			uCurrentView=uView;//Actually tView.uOrder
			if(!uFirst)
				fprintf(fp,"};\n");
			uFirst=0;
			fprintf(fp,"\n%s\n",field[2]);

			sprintf(cExtraForViewName,"cExtraForViewSlave:%.31s",field[1]);
			GetConfiguration(cExtraForViewName,cExtraForView,0);
			if(cExtraForView[0])
				fprintf(fp,"%s\n",cExtraForView);

			cViewXIP[0]=0;
			//Note optional second NS must have same view settings as first
			//if not "we have a problem houston."
			sprintf(cViewXIPName,"cViewXIP:%.31s:%.31s",cSlaveNS,field[1]);
			GetConfiguration(cViewXIPName,cViewXIP,0);
			//debug only
			//fprintf(fp,"//cViewXIPName=%s cViewXIP=%s\n",cViewXIPName,cViewXIP);
		}


		fprintf(fp,"\tzone \"%s\" {\n",field[0]);
		fprintf(fp,"\t\ttype slave;\n");
		if(field[4][0])
			fprintf(fp,"\t\tmasters { %s };\n",field[4]);
		else
			//backwards compatability
			fprintf(fp,"\t\tmasters { %s; };\n",cMasterIP);
		if(field[5][0])
			fprintf(fp,"\t\t//tZone.cOptions\n\t\t%s\n",field[5]);
		//For uSecondaryOnly=1 zones we use the default transfer-source
		if(cViewXIP[0] && field[6][0]=='0')
		{
			fprintf(fp,"\t\ttransfer-source %.16s;\n",cViewXIP);
		}
		fprintf(fp,"\t\tfile \"slave/%s/%c/%s\";\n\t};\n",
					field[1],field[0][0],field[0]);

		//Make dirs as we go
		if(!uDebug)
		{
			char *cp;

			sprintf(gcQuery,"/usr/local/idns/named.d/slave/%s",
				field[1]);
			mkdir(gcQuery,0777);
			chown(gcQuery,uUID,uGID);

			sprintf(gcQuery,"/usr/local/idns/named.d/slave/%s/%c",
				field[1],field[0][0]);
			mkdir(gcQuery,0777);
			chown(gcQuery,uUID,uGID);

			if((cp=strchr(field[0],'/')))
			{
				*cp=0;
				sprintf(gcQuery,"/usr/local/idns/named.d/slave/%s/%c/%s",
					field[1],field[0][0],field[0]);
				if(!uDebug)
				{
					mkdir(gcQuery,0777);
					chown(gcQuery,uUID,uGID);
				}
				*cp='/';
			}
		}
	}
	fprintf(fp,"};\n");
	mysql_free_result(res);
	if(fp && !uDebug) fclose(fp);

}//void CreateSlaveFiles();


void InstallNamedFiles(char *cIpNum)
{
	char cISMROOT[256]={""};
	char cSetupDir[16]={"setup9"};
	register char i;


	if(getenv("ISMROOT")!=NULL)
	{               
		strncpy(cISMROOT,getenv("ISMROOT"),255);
		gcHost[255]=0;
	}
	
	if(!cISMROOT[0])
	{
		printf("You must set ISMROOT env var first. Ex. export ISMROOT=/home/joe/unxsVZ\n"
			"If the iDNS dir is located in the /home/joe/unxsVZ dir. For source"
			" code installs you may need to 'ln -s unxsBind iDNS' inside your unxsVZ tree.");
		exit(0);
	}

	printf("Installing named for IP %s from %s/iDNS\n",
			IPNumber(cIpNum), cISMROOT);
	
	mkdir("/usr/local/idns",0777);
	mkdir("/usr/local/idns/named.d",0777);
	mkdir("/usr/local/idns/named.d/master",0777);
	mkdir("/usr/local/idns/named.d/slave",0777);

	printf("Configuring and installing files...\n");

	//Create dir hierarchy	
	for(i='0';i<='9';i++)
	{
		sprintf(gcQuery,"/usr/local/idns/named.d/master/%c",i);
		mkdir(gcQuery,0777);
		sprintf(gcQuery,"/usr/local/idns/named.d/slave/%c",i);
		mkdir(gcQuery,0777);
	}
	for(i='a';i<='z';i++)
	{
		sprintf(gcQuery,"/usr/local/idns/named.d/master/%c",i);
		mkdir(gcQuery,0777);
		sprintf(gcQuery,"/usr/local/idns/named.d/slave/%c",i);
		mkdir(gcQuery,0777);
	}

	if(strcmp(cIpNum,"0.0.0.0"))
	{
		sprintf(gcQuery,"cat %s/iDNS/%s/named.conf|sed -e \"s/{{cIpNumber}}/%s/g\" > /usr/local/idns/named.conf",
			cISMROOT,cSetupDir,IPNumber(cIpNum));	
		if(system(gcQuery))
			printf("Error configuring named.conf\n");
	}

	sprintf(gcQuery,"touch /usr/local/idns/named.d/master.zones");	
	if(system(gcQuery))
		 printf("Error configuring master.zones\n");

	sprintf(gcQuery,"touch /usr/local/idns/named.d/slave.zones");	
	if(system(gcQuery))
		 printf("Error configuring slave.zones\n");

	sprintf(gcQuery,"cat %s/iDNS/%s/localhost > /usr/local/idns/named.d/master/localhost",cISMROOT,cSetupDir);	
	if(system(gcQuery))
		 printf("Error configuring localhost\n");

	sprintf(gcQuery,"cat %s/iDNS/%s/127.0.0 > /usr/local/idns/named.d/master/127.0.0",cISMROOT,cSetupDir);	
	if(system(gcQuery))
		 printf("Error configuring 127.0.0\n");

	sprintf(gcQuery,"/usr/bin/dig @e.root-servers.net . ns > /usr/local/idns/named.d/root.cache");	
	if(system(gcQuery))
		 printf("Error configuring root.cache\n");

	printf("Done.\n");
	exit(0);


}//void InstallNamedFiles(char *cIpNum)


void SerialNum(char *serial)
{
	time_t luClock;
		
	//Make new today serial number
	time(&luClock);
	strftime(serial,12,"%Y%m%d00",localtime(&luClock));

}//void SerialNum(char *serial)


void ExportTable(char *cTable, char *cFile)
{
	printf("ExportTable() Not implemented yet\n");
	exit(0);
}//void ExportTable(char *cTable, char *cFile)


void ConnectMysqlServer(void)
{
	static unsigned uOnlyOnce=0;
#ifdef USECONF
	unsigned ucDbPort=guDbPort;
	char *cEffectiveDbIp=gcEffectiveDbIp;
	char *cEffectiveDbSocket=gcEffectiveDbSocket;
	char cDbIp[256];
	char cDbName[256];
	char cDbPwd[256];
	char cDbLogin[256];
	char cDbPort[256];
	char cDbSocket[256];

	//safe via sprintf in conf reader
	strcpy(cDbName,gcDbName);
	strcpy(cDbPwd,gcDbPasswd);
	strcpy(cDbLogin,gcDbLogin);
	strcpy(cDbIp,gcDbIp);
	strcpy(cDbSocket,gcDbSocket);
#else
	//Provide some fall back if nothing in tConfiguration
	char cDbIp[256]={""};
	char *cEffectiveDbIp=DBIP0;
	char cDbName[256]={DBNAME};
	char cDbPwd[256]={DBPASSWD};
	char cDbLogin[256]={DBLOGIN};
	char cDbPort[256]={""};
	unsigned ucDbPort=DBPORT;
	char cDbSocket[256]={""};
	char *cEffectiveDbSocket=DBSOCKET;
#endif

	if(uOnlyOnce) return;
	uOnlyOnce=1;

	//NOTE difference. Change? TODO
	//Read dbip,dbname (same as mySQL user name), and sourceip (passwd).	
	ConnectDb();

	GetConfiguration("cRemoteDbIp",cDbIp,0);
	GetConfiguration("cRemoteDbName",cDbName,0);
	GetConfiguration("cRemoteDbPwd",cDbPwd,0);
	GetConfiguration("cRemoteDbLogin",cDbLogin,0);
	GetConfiguration("cRemoteDbPort",cDbPort,0);
	GetConfiguration("cRemoteDbSocket",cDbSocket,0);

	//Debug only	
	//printf("%s %s %s %s\n",cDbIp,cDbName,cDbPwd,cDbLogin);

	if(cDbIp[0]) cEffectiveDbIp=cDbIp;
	if(cDbSocket[0]) cEffectiveDbSocket=cDbSocket;
	if(cDbPort[0]) sscanf(cDbPort,"%u",&ucDbPort);

        mysql_init(&gMysql2);
	if (!mysql_real_connect(&gMysql2,cEffectiveDbIp,cDbLogin,cDbPwd
				,cDbName,ucDbPort,cEffectiveDbSocket,0))
        {
		fprintf(stderr,"ConnectMysqlServer failed for %s.%s",
						cEffectiveDbIp,cDbName);
		exit(1);
        }

}//void ConnectMysqlServer(void)


void SlaveJobQueue(char *cNameServer, char *cMasterIP)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	register int first=1;
	char cCurrentZone[100]={" "};
	unsigned uModify=0;
	unsigned uNew=0;
	unsigned uDelete=0;
	unsigned uDeleteFirst=0;
	unsigned uChanged=0;

	ConnectDb();
	
	sprintf(gcQuery,"SELECT uJob,cJob,cZone,uMasterJob FROM tJob WHERE cTargetServer='%s SLAVE'"
			" AND uTime<=UNIX_TIMESTAMP(NOW()) ORDER BY cZone,uJob",cNameServer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		printf("%s -1\n",mysql_error(&gMysql));
		exit(1);
	}

	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)) 
	{
		MYSQL_RES *res2;

		while((field=mysql_fetch_row(res)))
		{
			//If MASTER EXTERNAL delete it from job queue since
			//since it will never be 'handled.'
			sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uMasterJob=%s AND"
					" cTargetServer LIKE '%% MASTER EXTERNAL'",field[3]);
			mysql_query(&gMysql,gcQuery);
			//printf("%s\n",gcQuery);
			if(mysql_errno(&gMysql)) 
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
			res2=mysql_store_result(&gMysql);
			if(mysql_num_rows(res2)) 
			{
				sprintf(gcQuery,"DELETE FROM tJob WHERE uMasterJob=%s AND"
						" cTargetServer LIKE '%% MASTER EXTERNAL'",field[3]);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql)) 
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(1);
				}
				printf("Deleted MASTER EXTERNAL job, and continuing ahead\n");
			}
			mysql_free_result(res2);
			
			//If MASTER OR MASTER HIDDEN still in queue skip.
			sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uMasterJob=%s AND"
					" cTargetServer LIKE '%% MASTER%%'",field[3]);//3-uMasterJob
			mysql_query(&gMysql,gcQuery);
			//printf("%s\n",gcQuery);
			if(mysql_errno(&gMysql)) 
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
			res2=mysql_store_result(&gMysql);
			if(mysql_num_rows(res2)) 
			{
				mysql_free_result(res2);
				//Ignore this job until 'REAL' MASTER
				//handles it.
				printf("Skipping job not yet handled by MASTER\n");
				continue;
			}
			mysql_free_result(res2);
			
			//Start processing SLAVE jobs

			if(strcmp(field[2],cCurrentZone))
			{
				if(!first)
				{
					//debug only
					printf("uDelete=%u uModify=%u uNew=%u uDeleteFirst=%u\n",
						uDelete,uModify,uNew,uDeleteFirst); 
					uChanged+=ProcessSlaveJob(cCurrentZone,uDelete,uModify,
							uNew,uDeleteFirst,cNameServer,cMasterIP);
					uModify=0;
					uNew=0;
					uDelete=0;
					uDeleteFirst=0;
				}
				else
				{
					first=0;
				}
				strcpy(cCurrentZone,field[2]);
			}
			printf("%s\t%s\t%s\n",field[0],field[2],field[1]);
			//Allow for combinations: Modify New, Delete New. Modify overrides a Delete.
			if(strstr(field[1],"New")) uNew++;
			if(strstr(field[1],"Modify")) 
				uModify++;
			else if(strstr(field[1],"Delete")) 
				uDelete++;
			if(uDelete && !uNew) uDeleteFirst=1;

			//Remove job from queue
			sprintf(gcQuery,"DELETE FROM tJob WHERE uJob=%s",field[0]);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql)) 
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}

		}
	}

	if(!first)
	{
		//debug only
		printf("uDelete=%u uModify=%u uNew=%u uDeleteFirst=%u\n",
						uDelete,uModify,uNew,uDeleteFirst); 
		uChanged+=ProcessSlaveJob(cCurrentZone,uDelete,uModify,
				uNew,uDeleteFirst,cNameServer,cMasterIP);
	}
	mysql_free_result(res);
	
	if(uChanged)
	{
		char cuControlPort[8]={""};
		char cCmd[100]={""};
	
		GetConfiguration("cuControlPort",cuControlPort,0);
		
		//Check to see if reconfigure is ok for slaves
		//debug only
		//printf("Reconfiguring slave server...");
#ifdef EXPERIMENTAL		
		if((uNamedCheckConf(cNameServer))) exit(1); //Exit without reloading the server
#endif
		if(cuControlPort[0])
			sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf -p %s reconfig",gcBinDir,cuControlPort);
		else
			sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf reconfig",gcBinDir);
		
		if(system(cCmd))
			exit(1);
		//debug only
		//printf("OK\n");
	}

	exit(0);

}//void SlaveJobQueue()


void MasterJobQueue(char *cNameServer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	register int first=1;
	char cCurrentZone[100]={" "};
	unsigned uModify=0;
	unsigned uNew=0;
	unsigned uDelete=0;
	unsigned uDeleteFirst=0;
	unsigned uChanged=0;
	char *cp;
	char cuControlPort[8]={""};
	char cCmd[100]={""};
	
	ConnectDb();

	GetConfiguration("cuControlPort",cuControlPort,0);

	//debug only
	//printf("MasterJobQueue(%s)\n",cNameServer);

	//MASTER OR MASTER HIDDEN
	sprintf(gcQuery,"SELECT uJob,cJob,cZone,cJobData FROM tJob WHERE (cTargetServer='%s MASTER' OR"
			" cTargetServer='%s MASTER HIDDEN') AND uTime<=UNIX_TIMESTAMP(NOW()) ORDER BY cZone,uJob",
					cNameServer,cNameServer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)) 
	{
		while((field=mysql_fetch_row(res)))
		{
			if(strcmp(field[2],cCurrentZone))
			{
				if(!first)
				{
					//debug only
					printf("uDelete=%u uModify=%u uNew=%u uDeleteFirst=%u\n",
						uDelete,uModify,uNew,uDeleteFirst); 
					uChanged+=ProcessMasterJob(cCurrentZone,uDelete,uModify,
						uNew,uDeleteFirst,cNameServer);

					uModify=0;
					uNew=0;
					uDelete=0;
					uDeleteFirst=0;
				}
				else
				{
					first=0;
				}
				strcpy(cCurrentZone,field[2]);
			}
			printf("%s\t%s\t%s\n",field[0],field[2],field[1]);
			//Allow for combinations: Modify New, Delete New. Modify overrides a Delete.
			if(strstr(field[1],"New")) uNew++;
			if(strstr(field[1],"Modify")) 
				uModify++;
			else if(strstr(field[1],"Delete")) 
				uDelete++;
			if(uDelete && !uNew) uDeleteFirst=1;

			//Inform ext queue of completion. Only for MASTER
			//no error checking takes place fix this.
			if(!strncmp(field[1],"Ext",3) && (cp=strstr(field[3],"uJob=")))
			{
				char cMsg[33];
				unsigned uExtJob=0;

				ExtConnectDb(0);
				sprintf(cMsg,"iDNS.%.20s",field[1]);
				sscanf(cp+5,"%u",&uExtJob);
				InformExtJob(cMsg,cNameServer,
						uExtJob,mysqlISP_Deployed);
			}

			//Remove job from queue
			sprintf(gcQuery,"DELETE FROM tJob WHERE uJob=%s",field[0]);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql)) 
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}

		}
	}

	if(!first)
	{
		//debug only
		printf("uDelete=%u uModify=%u uNew=%u uDeleteFirst=%u\n",
						uDelete,uModify,uNew,uDeleteFirst); 
		if((uChanged=ProcessMasterJob(cCurrentZone,uDelete,uModify,
				uNew,uDeleteFirst,cNameServer)))
		{
			if(uChanged==2)
				uReconfig=1;
			else if(uChanged==3)
				uReload=1;
		}
	}
//	mysql_free_result(res);

#ifdef EXPERIMENTAL
	if((uNamedCheckConf(cNameServer))) exit(1); //Will exit without server reload or reconfig
#endif
	if(uReload)
	{
		if(cuControlPort[0])
			sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf -p %s reload",gcBinDir,cuControlPort);
		else
			sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf reload",gcBinDir);
		
		if(system(cCmd))
			exit(1);
	}
	else if(uReconfig)
	{
		if(cuControlPort[0])
			sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf -p %s reconfig",gcBinDir,cuControlPort);
		else
			sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf reconfig",gcBinDir);
		
		if(system(cCmd))
			exit(1);
	}
	//Create master.zones again.
	//CreateMasterFiles(cNameServer,"",0,1,0);

	exit(0);

}//void MasterJobQueue(char *cNameServer)


int ProcessMasterJob(char *cZone,unsigned uDelete,unsigned uModify,unsigned uNew, unsigned uDeleteFirst, char *cMasterNS)
{
	//return 0 if nothing needs to be done
	//return 1 if zone info has changed
	//return 2 if zone is added or deleted
	//return 3 if zone is modified but we need a reload anyway
	//debug only
	printf("Queue Policy for %s: ",cZone);
	if(uDelete && !uNew)
	{
		//debug only
		printf("Delete for NS %s\n\n",cMasterNS);
		//Replace master.zones named.conf include file
		//All Zones replace, DBFiles No, Stubs Yes
		CreateMasterFiles(cMasterNS,"",0,1,0);
		return(2);
	}
	else if(uDelete && uNew && !uDeleteFirst)
	{
		//debug only
		printf("New then Delete\n\n");
		return(0);
	}
	else if(uNew)
	{
		//debug only
		if(uModify)
			printf("New Modify for NS %s\n\n",cMasterNS);
		else
			printf("New for NS %s\n\n",cMasterNS);
		//Append to master.zones named.conf include file
		//Single zone append, DBFiles yes, Stubs yes
		CreateMasterFiles(cMasterNS,cZone,1,1,0);
		return(2);//rndc reconfig works here for both cases
	}
	else if(uModify)
	{
		//debug only
		printf("Modify for NS %s\n\n",cMasterNS);
		//Single zone, DBFiles yes, Stubs yes
		//We will always build stubs when modyfing a zone
		//This was learned by running 2nd NS set support test case #4
		CreateMasterFiles(cMasterNS,cZone,1,1,0);
		//Single zone reload
		if(!uReload)//Set by return(3) above global
			ViewReloadZone(cZone);
		
		return(1);
	}
	
	return(0);

}//int ProcessMasterJob();


int ProcessSlaveJob(char *cZone,unsigned uDelete,unsigned uModify,unsigned uNew,
			unsigned uDeleteFirst, char *cMasterNS, char *cMasterIP)
{
	char cuControlPort[8]={""};
	char cCmd[100]={""};

	GetConfiguration("cuControlPort",cuControlPort,0);
	
	//debug only
	printf("Queue Policy for %s: ",cZone);
	if(uDelete && !uNew)
	{
		//debug only
		printf("Delete for NS %s\n\n",cMasterNS);
		//Replace slave.zones named.conf include file
		//All Zones replace
		CreateSlaveFiles(cMasterNS,"",cMasterIP,0);
		return(1);
	}
	else if(uDelete && uNew && !uDeleteFirst)
	{
		//debug only
		printf("New then Delete\n\n");
		return(0);
	}
	else if(uModify)
	{
		//debug only
		if(uNew)
			printf("Modify New for NS %s\n\n",cMasterNS);
		else
			printf("Modify for NS %s\n\n",cMasterNS);
		CreateSlaveFiles(cMasterNS,"",cMasterIP,0);
		if(cuControlPort[0])
			sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf -p %s reload",gcBinDir,cuControlPort);
		else
			sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf reload",gcBinDir);
		system(cCmd);
		
		ViewReloadZone(cZone);
		return(0);
	}
	else if(uNew)
	{
		//debug only
		printf("New for NS %s\n\n",cMasterNS);
		//Append to slave.zones named.conf include file
		//Single zone append
		CreateSlaveFiles(cMasterNS,cZone,cMasterIP,0);
		//This is a hack has to be optimized further
		if(cuControlPort[0])
			sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf -p %s reload",gcBinDir,cuControlPort);
		else
			sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf reload",gcBinDir);
		system(cCmd);
		ViewReloadZone(cZone);
		return(1);//reconfigure: new item in slave.zones
	}
	
	return(0);

}//int ProcessSlaveJob();


void GenerateArpaZones(void)
{
	MYSQL_RES *res;

	char cZone[100];
	unsigned uZone=0;
	unsigned uOwner=0;
	

	//Zone A Records
	sprintf(gcQuery,"SELECT cZone,cMainAddress,uZone,uOwner FROM tZone WHERE cZone NOT LIKE '%%.arpa'");
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql)) 
		tZone(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	
	Header_ism3("GenerateArpaZones()",0);
	printf("</center><pre>\n");

	if(mysql_num_rows(res)) 
	{
		MYSQL_ROW field;
		while((field=mysql_fetch_row(res)))
		{
			strcpy(cZone,field[0]);
			sscanf(field[2],"%u",&uZone);
			sscanf(field[3],"%u",&uOwner);
			if( !strcmp(field[1],"0.0.0.0") || field[1][0]==0)
				continue;
			
			if(PopulateArpaZone(field[0],field[1],0,uZone,uOwner)) continue;

		}
	}
	mysql_free_result(res);
	
	
	//A Resource Records
	sprintf(gcQuery,"SELECT tZone.cZone,tResource.cParam1,tResource.cName,tZone.uZone,tZone.uOwner FROM tResource,tZone,tRRType WHERE tResource.uZone=tZone.uZone AND tResource.uRRType=tRRType.uRRType AND tRRType.cLabel='A'");
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql)) 
	{
		printf("%s\n",mysql_error(&gMysql));
		Footer_ism3();
	}
	res=mysql_store_result(&gMysql);
	
	if(mysql_num_rows(res)) 
	{
		MYSQL_ROW field;
		while((field=mysql_fetch_row(res)))
		{
			//If cName=field[2] if FQDN leave as is else
			//add zone to it.
			if(field[2][strlen(field[2])-1]=='.')
				sprintf(cZone,"%s",field[2]);
			else if(field[2][0] && field[2][0]!='\t')
				sprintf(cZone,"%s.%s",field[2],field[0]);
			//default case
			else if(1)
				sprintf(cZone,"%s",field[0]);

			//debug only
			//printf("(%s) (%s) (%s) [%s]\n",
			//		field[2],field[0],cZone,field[1]);
			//Footer_ism3();

			sscanf(field[3],"%u",&uZone);
			sscanf(field[4],"%u",&uOwner);
			if(PopulateArpaZone(cZone,field[1],0,uZone,uOwner)) continue;
		}
	}
	mysql_free_result(res);

	printf("Remember to select and remove duplicate PTR records from .arpa zones now.\n");
	Footer_ism3();

}//void GenerateArpaZones(void)


int PopulateArpaZone(const char *cZone, const char *cIPNum, const unsigned uHtmlMode, 
					const unsigned uFromZone, const unsigned uZoneOwner)
{
	unsigned a=0,b=0,c=0,d=0;
	MYSQL_RES *res2;
	MYSQL_ROW field;
	unsigned uZone=0;
	unsigned uNew=0;
	char cArpaZone[64];
	char cHostMaster[256]="hostmaster.isp.net";
	char cuView[256]="2";

	GetConfiguration("cHostMaster",cHostMaster,0);
	GetConfiguration("cuView",cuView,0);

	sscanf(cIPNum,"%u.%u.%u.%u",&a,&b,&c,&d);
			
	if(!uHtmlMode)
		printf("<u>%s %u.%u.%u.%u</u>\n",cZone,a,b,c,d);

	if(a==0 || d==0 || a>254 || b>254 || c>254 || d>254) return(1);
	sprintf(cArpaZone,"%u.%u.%u.in-addr.arpa",c,b,a);
	sprintf(gcQuery,"SELECT cZone,uZone FROM tZone WHERE cZone='%s' AND uView=%.2s",
			cArpaZone,cuView);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		if(uHtmlMode)
			htmlPlainTextError(mysql_error(&gMysql));
		else
			printf("PopulateArpaZone() Select Error: %s\n",mysql_error(&gMysql));
		return(1);
	}
	res2=mysql_store_result(&gMysql);
	if(!mysql_num_rows(res2)) 
	{
		if(!uHtmlMode)
			printf("<font color=blue>Adding new arpa zone: %s</font>\n",cArpaZone);
		if(AddNewArpaZone(cArpaZone,1,cHostMaster)) 
		{
			if(uHtmlMode)
				htmlPlainTextError(mysql_error(&gMysql));
			else
				printf("AddNewArpaZone() Error: %s\n",mysql_error(&gMysql));
			return(1);//tzonefunc.h
		}
		uZone=mysql_insert_id(&gMysql);
		uNew=1;

	}
	else if((field=mysql_fetch_row(res2)))
	{
		sscanf(field[1],"%u",&uZone);
	}

	mysql_free_result(res2);

	if(uZone)
	{
		unsigned uErrNum=0;

		if(!uHtmlMode)
			printf("Adding RR to arpa zone: d=%u,uZone=%u,uZoneOwner=%u\n",
				d,uZone,uZoneOwner);

		//Add PTR record 
		if((uErrNum=AutoAddPTRResource(d,cZone,uZone,uZoneOwner)))
		{
			if(uHtmlMode)
				tZone(mysql_error(&gMysql));
			else
				if(!uHtmlMode)
					printf("AutoAddPTRResource() Error(%u): %s\n",
						uErrNum,mysql_error(&gMysql));
				else
					htmlPlainTextError(mysql_error(&gMysql));
			return(1);
		}

		if(!uNew)
			SubmitJob("Modify",1,cArpaZone,0,0);
		else
			SubmitJob("New",1,cArpaZone,0,0);
		UpdateSerialNum(uZone);
	}

	//TODO what is this for comments please!
	if(uNew && uFromZone)
	{
		SubmitJob("New",1,cArpaZone,0,0);
	}
	return(0);

}//int PopulateArpaZone()


void PassDirectHtml(char *file)
{
	FILE *fp;
	char buffer[1024];

	if((fp=fopen(file,"r"))!=NULL)
	{
		while(fgets(buffer,1024,fp)!=NULL)
			printf("%s",buffer);

		fclose(fp);
	}

}//void PassDirectHtml(char *file)


void GetConfiguration(const char *cName, char *cValue, unsigned uHtml)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        char cQuery[512];

        sprintf(cQuery,"SELECT cValue,cComment FROM tConfiguration WHERE cLabel='%s'",cName);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
	{
		if(uHtml)
		{
        	        tConfiguration(mysql_error(&gMysql));
		}
		else
		{
			fprintf(stderr,"%s\n",mysql_error(&gMysql));
			exit(1);
		}
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
	{
		if(strcmp(field[0],"Value in cComment"))
                	sprintf(cValue,"%.255s",field[0]);
		else
                	sprintf(cValue,"%.1023s",field[1]);
	}
        mysql_free_result(res);

}//void GetConfiguration(const char *cName, char *cValue)


//cValue must be a buffer for min 512 chars
void GetGlossary(const char *cName, char *cValue)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        char cQuery[512];

        sprintf(cQuery,"SELECT cText FROM tGlossary WHERE cLabel='%s'",cName);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
		tConfiguration(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sprintf(cValue,"%.511s",field[0]);
        mysql_free_result(res);

}//void GetGlossary(const char *cName, char *cValue)

//Import tClient records that will be owners of tClient user records
void ImportCompanies(void)
{
	FILE *fp;
	char cQuery[256];
	unsigned uClient;
	char *cp;

	printf("ImportCompanies() Start\n");

	if(!(fp=fopen("/usr/local/idns/csv/companycode.csv","r")))
	{
		fprintf(stdout,"Error: Could not open: /usr/local/idns/csv/companycode.csv\n");
				return;
	}

	//Start processing. Unix files only!
	while(fgets(gcQuery,254,fp)!=NULL)
	{
		uClient=0;//tClient PK

		//skip empty lines and comments
		if(gcQuery[0]=='\n' || gcQuery[0]=='#')
			continue;
		gcQuery[strlen(gcQuery)-1]=0;


		//Quick and dirty parsing input must be in good shape!
		//Parse uClient
		if((cp=strchr(gcQuery,','))) 
		{
			*cp=0;
			sscanf(gcQuery,"%u",&uClient);
		
			sprintf(cQuery,"INSERT INTO tClient SET uClient=%u,cLabel='%.99s',"
				"cCode='Organization',cInfo='ImportCompanies() IMPORTED',"
				"uOwner=1,uCreatedBy=1,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",uClient,cp+1);
			//debug only
			//printf("%s\n",cQuery);
			mysql_query(&gMysql,cQuery);
			if(mysql_errno(&gMysql))
				fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
		}
		else
			printf("Skipping invalid line: %s\n",gcQuery);
	}

	if(fp) fclose(fp);

	printf("ImportCompanies() End\n");

}//void ImportCompanies(void)


void DropCompanies(void)
{
	printf("DropCompanies() Start\n");

	sprintf(gcQuery,"DELETE FROM tClient WHERE cInfo LIKE '%%ImportCompanies() IMPORTED'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));

	printf("DropCompanies() End\n");

}//void DropCompanies(void)


//Import tClient records that will be owned by an existing tClient company record
void ImportUsers(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	FILE *fp;
	char cQuery[256];
	char cCompany[256];
	char cFirst[100];
	char cLast[100];
	char cEmail[100];
	char cLogin[100];
	char cPasswd[100];
	char *cp,*cp2;
	unsigned uCompany,uUser;

	printf("ImportUsers() Start\n");

	if(!(fp=fopen("/usr/local/idns/csv/companyuser.csv","r")))
	{
		fprintf(stdout,"Error: Could not open: /usr/local/idns/csv/companyuser.csv\n");
				return;
	}

	//Start processing. Unix files only!
	while(fgets(gcQuery,254,fp)!=NULL)
	{
		uCompany=0;
		uUser=0;
		cFirst[0]=0;
		cLast[0]=0;
		cEmail[0]=0;
		cLogin[0]=0;
		cPasswd[0]=0;
		cCompany[0]=0;

		//skip empty lines and comments
		if(gcQuery[0]=='\n' || gcQuery[0]=='#')
			continue;
		gcQuery[strlen(gcQuery)-1]=0;

		//debug only
		//printf("%s\n",gcQuery);

		//Quick and dirty parsing input must be in good shape!
		//Parse uCompany
		if((cp=strchr(gcQuery,','))) 
		{
			*cp=0;
			sscanf(gcQuery,"%u",&uCompany);
			*cp=',';
		}
		//debug only
		//printf("uCompany=%u\n",uCompany);

		//Parse uUser
		if((cp2=strchr(cp+1,','))) 
		{
			*cp2=0;
			sscanf(cp+1,"%u",&uUser);
			*cp2=',';
		}
		//debug only
		//printf("uUser=%u\n",uUser);

		//Parse cFirst
		if((cp=strchr(cp2+1,','))) 
		{
			*cp=0;
			sprintf(cFirst,"%.99s",cp2+1);
			*cp=',';
		}
		//debug only
		//printf("cFirst=%s\n",cFirst);

		//Parse cLast
		if((cp2=strchr(cp+1,','))) 
		{
			*cp2=0;
			sprintf(cLast,"%.99s",cp+1);
			*cp2=',';
		}
		//debug only
		//printf("cLast=%s\n",cLast);

		//Parse cEmail
		if((cp=strchr(cp2+1,','))) 
		{
			*cp=0;
			sprintf(cEmail,"%.99s",cp2+1);
			*cp=',';
		}
		//debug only
		//printf("cEmail=%s\n",cEmail);

		//Parse cLogin
		if((cp2=strchr(cp+1,','))) 
		{
			*cp2=0;
			sprintf(cLogin,"%.99s",cp+1);
			*cp2=',';
		}
		//debug only
		//printf("cLogin=%s\n",cLogin);

		//Parse cPasswd
		sprintf(cPasswd,"%.99s",cp2+1);
		//debug only
		//printf("cPasswd=%s\n",cPasswd);


		//Sanity checks
		if(!uCompany || !uUser || !cFirst[0] || !cLast[0] || !cLogin[0])
			printf("Warning %s may be incorrectly formatted\n",
					gcQuery);

		sprintf(cQuery,"SELECT cLabel FROM tClient WHERE uOwner=1 AND uClient=%u AND"
				" cInfo LIKE '%%ImportCompanies() IMPORTED'",uCompany);
		mysql_query(&gMysql,cQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sprintf(cCompany,"%.255s",field[0]);
		mysql_free_result(res);

		if(cCompany[0])
		{
			//Company same as user, fix company
			sprintf(cQuery,"%.99s %.99s",cFirst,cLast);
			if(!strcmp(cCompany,cQuery))
			{
				sprintf(gcQuery,"UPDATE tClient SET cLabel='%.250s Co.',uModBy=1,"
						"uModDate=UNIX_TIMESTAMP(NOW()) WHERE uClient=%u",cCompany,uCompany);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
					exit(2);
				}
			}
			sprintf(gcQuery,"INSERT INTO tClient SET cLabel='%.99s %.99s',cInfo='ImportUsers() IMPORTED',"
					"uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW()),cEmail='%s',"
					"uClient=%u",TextAreaSave(cFirst),TextAreaSave(cLast),uCompany,cEmail,uUser);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stdout,"Warning: %s\n",mysql_error(&gMysql));
				continue;
			}

			//tAuthorize issues
			if(!cLogin[0])
				sprintf(cLogin,"%.4s%.4s",cFirst,cLast);
			if(!cPasswd[0])
				sprintf(cPasswd,"%.3sH56%.3s",cFirst,cLast);
			EncryptPasswdWithSalt(cPasswd,"..");
			sprintf(gcQuery,"INSERT INTO tAuthorize SET cLabel='%.32s',uPerm=6,uCertClient=%u,cPasswd='%s',"
				"uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW()),cIPMask='0.0.0.0/0'",
				cLogin,uUser,cPasswd,uCompany);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				fprintf(stdout,"Warning: %s\n",mysql_error(&gMysql));
		}
		else
		{
			printf("Error: %s company not found\n",cCompany);
		}

	}

	if(fp) fclose(fp);

	
	printf("ImportUsers() End\n");

}//void ImportUsers(void)


void DropUsers(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cQuery[256];
	unsigned uClient;

	printf("DropUsers() Start\n");

	sprintf(cQuery,"SELECT uClient FROM tClient WHERE uOwner!=1 AND cInfo LIKE '%%ImportUsers() IMPORTED'");
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		uClient=0;
		sscanf(field[0],"%u",&uClient);

		sprintf(gcQuery,"DELETE FROM tAuthorize WHERE uCertClient=%u",uClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
	}
	mysql_free_result(res);

	sprintf(cQuery,"DELETE FROM tClient WHERE uOwner!=1 AND cInfo LIKE '%%ImportUsers() IMPORTED'");
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
		exit(2);
	}

	printf("DropUsers() End\n");

}//void DropUsers(void)


void ImportBlocks(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	FILE *fp;
	char cQuery[256];
	char cCompany[256];
	char cBlock[100];
	char *cp;
	unsigned uClient;

	printf("ImportBlocks() Start\n");

	if(!(fp=fopen("/usr/local/idns/csv/companyblock.csv","r")))
	{
		fprintf(stdout,"Error: Could not open: /usr/local/idns/csv/companyblock.csv\n");
				return;
	}

	//Start processing. Unix files only!
	while(fgets(gcQuery,254,fp)!=NULL)
	{
		cBlock[0]=0;
		cCompany[0]=0;
		uClient=0;

		//skip empty lines and comments
		if(gcQuery[0]=='\n' || gcQuery[0]=='#')
			continue;
		gcQuery[strlen(gcQuery)-1]=0;

		//debug only
		//printf("%s\n",gcQuery);

		//Quick and dirty parsing input must be in good shape!
		//Parse Company uClient
		if((cp=strchr(gcQuery,','))) 
		{
			*cp=0;
			sscanf(gcQuery,"%u",&uClient);
			*cp=',';
		}
		//debug only
		//printf("uClient=%u\n",uClient);

		//Parse cBlock
		sprintf(cBlock,"%.99s",cp+1);
		//printf("cBlock=%s\n",cBlock);

		//Sanity checks
		if(!uClient || !cBlock[0] )
			printf("Warning %s may be incorrectly formatted\n",
					gcQuery);

		sprintf(cQuery,"SELECT cLabel FROM tClient WHERE uOwner=1 AND cInfo LIKE '%%ImportCompanies()"
				" IMPORTED' AND uClient=%u",uClient);
		mysql_query(&gMysql,cQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sprintf(cCompany,"%.255s",field[0]);
		mysql_free_result(res);

		//debug only
		//printf("cCompany=%s\n",cCompany);
		if(cCompany[0])
		{
			sprintf(cQuery,"INSERT INTO tBlock SET cLabel='%.32s',cComment='ImportBlocks() IMPORTED',"
				"uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",cBlock,uClient);
			mysql_query(&gMysql,cQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stdout,"Warning: %s\n",mysql_error(&gMysql));
			}
		}
		else
		{
			printf("Error: %s company not found\n",cCompany);
		}

	}

	if(fp) fclose(fp);
	printf("ImportBlocks() End\n");

}//void ImportBlocks(void)


void DropBlocks(void)
{
	printf("DropBlocks() Start\n");

	sprintf(gcQuery,"DELETE FROM tBlock WHERE cComment LIKE '%%ImportBlocks() IMPORTED'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));

	printf("DropBlocks() End\n");

}//void DropBlocks(void)


void AssociateCompaniesZones(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	FILE *fp;
	char cQuery[256];
	char cCompany[256];
	char cZone[100];
	char cIP[20];
	char *cp;
	unsigned uClient;
	unsigned uA;
	unsigned uB;
	unsigned uC;
	unsigned uD;
	unsigned uResource;

	printf("AssociateCompaniesZones() Start\n");

	if(!(fp=fopen("/usr/local/idns/csv/companyzone.csv","r")))
	{
		fprintf(stdout,"Error: Could not open: /usr/local/idns/csv/companyzone.csv\n");
				return;
	}

	//Start processing. Unix files only!
	while(fgets(gcQuery,254,fp)!=NULL)
	{
		cCompany[0]=0;
		cZone[0]=0;
		uClient=0;

		//skip empty lines and comments
		if(gcQuery[0]=='\n' || gcQuery[0]=='#')
			continue;
		gcQuery[strlen(gcQuery)-1]=0;

		//debug only
		//printf("%s\n",gcQuery);

		//Quick and dirty parsing input must be in good shape!
		//Parse cCompany
		if((cp=strchr(gcQuery,','))) 
		{
			*cp=0;
			sscanf(gcQuery,"%u",&uClient);
			*cp=',';
		}

		sprintf(cZone,"%.99s",cp+1);
		//printf("cZone=%s\n",cZone);

		//Sanity checks
		if(!uClient || !cZone[0] )
			printf("Warning %s may be incorrectly formatted\n",
					gcQuery);

		sprintf(cQuery,"SELECT uClient FROM tClient WHERE uOwner=1 AND cInfo"
				" LIKE '%%ImportCompanies() IMPORTED' AND uClient=%u",uClient);
		mysql_query(&gMysql,cQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sprintf(cCompany,"%.255s",field[0]);
		mysql_free_result(res);

		if(cCompany[0])
		{
			sprintf(cQuery,"UPDATE tZone SET uOwner=%u,uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
					" WHERE cZone='%s'",uClient,cZone);
			mysql_query(&gMysql,cQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stdout,"Warning: %s\n",mysql_error(&gMysql));
			}
			if(mysql_affected_rows(&gMysql)<1)
			{
				fprintf(stdout,"Warning: %s not updated.\n",cZone);
				continue;//Skip RRs
			}
			sprintf(cQuery,"UPDATE tResource,tZone SET tResource.uOwner=%u,tResource.uModBy=1,"
					"tResource.uModDate=UNIX_TIMESTAMP(NOW()) WHERE"
					" tZone.uZone=tResource.uZone AND tZone.cZone='%s'",uClient,cZone);
			mysql_query(&gMysql,cQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stdout,"Warning: %s\n",mysql_error(&gMysql));
			}
		}
		else
		{
			printf("Error: %s company not found\n",cCompany);
		}

	}
	if(fp) fclose(fp);

	//In this section we use block ownership to change PTR RRs ownership
	//Overlapping blocks should not be allowed to exist in tBlock
	//so the first block that contains the IP is the block we use.
	//This simplistic algo only works for class C in-addr.arpa zones
	sprintf(cQuery,"SELECT tResource.uResource,tResource.cName,tZone.cZone FROM tResource,tZone WHERE"
			" tResource.uZone=tZone.uZone AND tResource.uRRType=7");
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		uA=255;
		uB=255;
		uC=255;
		uD=255;

		sscanf(field[0],"%u",&uResource);

		//Try to reconstruct the IP
		sscanf(field[1],"%u",&uD);
		sscanf(field[2],"%u.%u.%u.in-addr.arpa",&uC,&uB,&uA);
		sprintf(cIP,"%u.%u.%u.%u\n",uA,uB,uC,uD);
		if(uA>0 && uA<255 && uB<255 && uC<255 && uD<255)
		{
			MYSQL_RES *res2;
			MYSQL_ROW field2;

			//This next step is Big-O n. i.e. very slow...
			sprintf(cQuery,"SELECT cLabel,uOwner FROM tBlock");
			mysql_query(&gMysql,cQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
				exit(2);
			}
			res2=mysql_store_result(&gMysql);
			while((field2=mysql_fetch_row(res2)))
			{
				if(uIpv4InCIDR4(cIP,field2[0])==1)
				{
					sscanf(field2[1],"%u",&uClient);
					sprintf(cQuery,"UPDATE tResource SET uOwner=%u WHERE uResource=%u",
						uClient,uResource);
					mysql_query(&gMysql,cQuery);
					if(mysql_errno(&gMysql))
					{
						fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
						exit(2);
					}
					//debug only
					//printf("%s is in %s\n",cIP,field2[0]);
					break;	
				}
			}
			mysql_free_result(res2);
		}
		else
		{
			printf("Warning: %s seems incorrect. Skiping.\n",cIP);
		}
	}
	mysql_free_result(res);
	
	printf("AssociateCompaniesZones() End\n");

}//void AssociateCompaniesZones(void)


unsigned ViewReloadZone(char *cZone)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uRetVal=1;
	char cuControlPort[8]={""};
	char cCmd[100]={""};

	GetConfiguration("cuControlPort",cuControlPort,0);
#ifdef EXPERIMENTAL
	if((uNamedCheckConf("ViewReloadZone() call"))) return(1);
#endif
	//Multiple view rndc reload
	//Do we need to reload all views, when we might have only modified the internal view for example?
	sprintf(gcQuery,"SELECT tView.cLabel,tZone.uSecondaryOnly FROM tZone,tView WHERE"
				" tZone.uView=tView.uView AND tZone.cZone='%s'",cZone);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		printf("%s",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(field[1][0]=='1')
		{
			if(cuControlPort[0])
				sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf -p %s retransfer %s in %s",
						gcBinDir,cuControlPort,cZone,field[0]);
			else
				sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf retransfer %s in %s",
						gcBinDir,cZone,field[0]);
		}
		else
		{
			if(cuControlPort[0])
				sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf -p %s reload %s in %s",
						gcBinDir,cuControlPort,cZone,field[0]);
			else
				sprintf(cCmd,"%s/rndc -c /etc/unxsbind-rndc.conf reload %s in %s",
						gcBinDir,cZone,field[0]);
		}
		//debug only
		printf("ViewReloadZone():%s\n",cCmd);
		uRetVal=system(cCmd);
	}
	mysql_free_result(res);

	return(uRetVal);

}//unsigned ViewReloadZone(char *cZone)


//Import tRegistrar records
void ImportRegistrars(void)
{
	FILE *fp;
	char cQuery[256];

	printf("ImportRegistrars() Start\n");

	if(!(fp=fopen("/usr/local/idns/csv/registrar.txt","r")))
	{
		fprintf(stdout,"Error: Could not open: /usr/local/idns/csv/registrar.txt\n");
				return;
	}

	//Start processing. Unix files only!
	while(fgets(gcQuery,254,fp)!=NULL)
	{
		//skip empty lines NOT comments #1 registery in the world :)
		if(gcQuery[0]=='\n')
			continue;
		gcQuery[strlen(gcQuery)-1]=0;

		sprintf(cQuery,"INSERT INTO tRegistrar SET cLabel='%.99s',uOwner=1,uCreatedBy=1,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",gcQuery);
		//debug only
		//printf("%s\n",cQuery);
		mysql_query(&gMysql,cQuery);
		if(mysql_errno(&gMysql))
			fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
	}

	if(fp) fclose(fp);

	printf("ImportRegistrars() End\n");

}//void ImportRegistrars(void)


void DropRegistrars(void)
{
	printf("DropRegistrars() Start\n");

	sprintf(gcQuery,"TRUNCATE tRegistrar");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));

	printf("DropRegistrars() End\n");

}//void DropRegistrars(void)


void AssociateRegistrarsZones(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	FILE *fp;
	char cQuery[256];
	char cRegistrar[256];
	unsigned uRegistrar;
	char cZone[100];
	char *cp;

	printf("AssociateRegistrarsZones() Start\n");

	if(!(fp=fopen("/usr/local/idns/csv/registrarzone.csv","r")))
	{
		fprintf(stdout,"Error: Could not open: /usr/local/idns/csv/registrarzone.csv\n");
				return;
	}

	//Start processing. Unix files only!
	while(fgets(gcQuery,254,fp)!=NULL)
	{
		cRegistrar[0]=0;
		cZone[0]=0;
		uRegistrar=0;

		if(gcQuery[0]=='\n' || gcQuery[0]=='#')
			continue;
		gcQuery[strlen(gcQuery)-1]=0;

		//Sample lines
		//442subs.co.uk,Nominet
		//442subsusa.com,Network Solutions

		//Quick and dirty parsing input must be in good shape!
		//Parse cZone
		if((cp=strchr(gcQuery,','))) 
		{
			*cp=0;
			sprintf(cZone,"%.99s",gcQuery);
			*cp=',';
		}
		//debug only
		//printf("cZone=%s\n",cZone);

		sprintf(cRegistrar,"%.99s",cp+1);
		//debug only
		//printf("cRegistrar=%s\n",cRegistrar);

		//Sanity checks
		if(!cRegistrar[0] || !cZone[0] )
		{
			fprintf(stderr,"Error: %s may be incorrectly formatted\n",
					gcQuery);
			continue;
		}

		sprintf(cQuery,"SELECT uRegistrar FROM tRegistrar WHERE cLabel='%s'",cRegistrar);
		mysql_query(&gMysql,cQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uRegistrar);
		mysql_free_result(res);

		if(uRegistrar)
		{
			sprintf(cQuery,"UPDATE tZone SET uRegistrar=%u,uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
					" WHERE cZone='%s'",uRegistrar,cZone);
			mysql_query(&gMysql,cQuery);
			if(mysql_errno(&gMysql))
			{
				fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
			}
			if(mysql_affected_rows(&gMysql)<1)
			{
				fprintf(stdout,"Warning: %s not updated.\n",cZone);
			}
		}
		else
		{
			printf("Warning: %s registrar not found\n",cRegistrar);
		}

	}
	if(fp) fclose(fp);

}//void AssociateRegistrarsZones(void)


void MassZoneUpdate(void)
{
	FILE *fp;
	char cQuery[512];
	char cZone[256];
	unsigned uNSSet;
	MYSQL_RES *res;
	MYSQL_ROW field;

	printf("MassZoneUpdate() Start\n");

	if(!(fp=fopen("/usr/local/idns/csv/zones2update.txt","r")))
	{
		fprintf(stdout,"Error: Could not open: /usr/local/idns/csv/zones2update.txt\n");
				return;
	}

	//Start processing. Unix files only!
	while(fgets(cZone,254,fp)!=NULL)
	{
		if(cZone[0]=='\n' || cZone[0]=='#')
			continue;
		cZone[strlen(cZone)-1]=0;

		uNSSet=0;
		sprintf(cQuery,"SELECT uNSSet FROM tZone WHERE cZone='%.255s'",cZone);
		mysql_query(&gMysql,cQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uNSSet);
		mysql_free_result(res);
		if(!uNSSet)
		{
			fprintf(stdout,"Warning: %s not found.\n",cZone);
			continue;
		}

		//debug only
		//printf("%s\n",cQuery);
		//break;

		//1-. Update zone serial number
		sprintf(cQuery,"UPDATE tZone SET uSerial=uSerial+1,uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE cZone='%.255s'",cZone);
		mysql_query(&gMysql,cQuery);
		if(mysql_errno(&gMysql))
			fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
		if(mysql_affected_rows(&gMysql)<1)
		{
			fprintf(stdout,"Warning: %s not updated.\n",cZone);
			continue;
		}

		//2-. Submit zone mod job only if we have a NS and the update worked
		//external view only
		SubmitJob("Modify",uNSSet,cZone,0,0);
	}

	if(fp) fclose(fp);

	printf("MassZoneUpdate() End\n");

}//void MassZoneUpdate(void)


void MassZoneNSUpdate(char *cLabel)
{
	FILE *fp;
	char cQuery[512];
	char cZone[256];
	char cMasterIPs[256]={""};
	unsigned uNSSet=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	printf("MassZoneNSUpdate() Start\n");

	if(!cLabel[0])
	{
		fprintf(stderr,"Error: No tNameServer.cLabel provided. Aborting.\n");
		exit(1);
	}

	if(!(fp=fopen("/usr/local/idns/csv/zones2NSupdate.txt","r")))
	{
		fprintf(stdout,"Error: Could not open: /usr/local/idns/csv/zones2NSupdate.txt\n");
				return;
	}
	//3 == MASTER EXTERNAL
	sprintf(cQuery,"SELECT uNSSet,cMasterIPs FROM tNS,tNSSet WHERE tNS.uNSSet=tNSSet.uNSSet AND"
			" tNSSet.cLabel='%.32s' AND tNS.uNSType=3",cLabel);
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNSSet);
		sprintf(cMasterIPs,"%.255s",field[1]);
	}
	mysql_free_result(res);
	if(!uNSSet)
	{
		fprintf(stderr,"Error: %s not found in tNameServer with a MASTER EXTERNAL. Aborting.\n",cLabel);
		exit(3);
	}
	if(!cMasterIPs[0])
	{
		fprintf(stderr,"Error: No tNameServer.cMasterIPs for %s. Aborting.\n",cLabel);
		exit(4);
	}


	//Start processing. Unix files only!
	while(fgets(cZone,254,fp)!=NULL)
	{
		if(cZone[0]=='\n' || cZone[0]=='#')
			continue;
		cZone[strlen(cZone)-1]=0;


		//1-. Update zone serial number, uNSSet, cMasterIPs OLD FORK
		//1-. Update zone serial number, uNSSet NEW GPL FORK
		sprintf(cQuery,"UPDATE tZone SET uNSSet=%u,uSerial=uSerial+1,uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE cZone='%.255s'",uNSSet,cZone);

		//debug only
		//printf("%s\n",cQuery);
		//continue;


		mysql_query(&gMysql,cQuery);
		if(mysql_errno(&gMysql))
		{
			fprintf(stdout,"Error: %s\n",mysql_error(&gMysql));
			continue;
		}
		if(mysql_affected_rows(&gMysql)<1)
		{
			fprintf(stdout,"Warning: %s not updated.\n",cZone);
			continue;
		}

		//2-. Submit zone mod job only if we have a NS and the update worked
		//Modify New means new NS = new slave.zones file
		SubmitJob("Modify New",uNSSet,cZone,0,0);
	}

	if(fp) fclose(fp);

	printf("MassZoneNSUpdate() End\n");

}//void MassZoneNSUpdate(char *cLabel)


unsigned uGetZoneOwner(unsigned uZone)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	unsigned uOwner=1; //Default: Root

	sprintf(gcQuery,"SELECT uOwner FROM tZone WHERE uZone=%u",uZone);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uOwner);
	
	return(uOwner);

}//unsigned uGetZoneOwner(unsigned uZone)


#ifdef EXPERIMENTAL
unsigned uNamedCheckConf(char *cNameServer)
{
	//This function runs a named-checkzone for the iDNS controlled named daemon
	//Returns 0 if OK, 1 if failure. Also it will create a tLog entry in case of error for informing
	//the admin users via the iDNS backennd dashboard.
	//
	//The idea is to not allow incomplete validation or unexpected configuration problems
	//to break a running production NS.
	unsigned uRet=0;

	//We will check named configuration file to see if there are any issues there
	sprintf(gcQuery,"%s/named-checkconf /usr/local/idns/named.conf",gcBinDir);
	
	if(system(gcQuery))
	{
		//Insert tLog record and skip server reload or reconfig
		sprintf(gcQuery,"INSERT INTO tLog SET uLogType=1,uPermLevel=12,uLoginClient=1,"
				"cLogin='MasterJobQueue()',cHost ='127.0.0.1',uTablePK=0,cTableName='(None)',"
				"cMessage='Server named.conf with errors',cServer='%s',uOwner=1,uCreatedBy=1,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())" ,cNameServer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		
		uRet=1;
	}

	return(uRet);

}//unsigned uNamedCheckConf(void)
#endif

unsigned uGetOrganization(char *cLabel)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uOwner=0;
	
	sprintf(gcQuery,"SELECT uOwner FROM tClient WHERE cLabel='%s'",cLabel);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uOwner);
	
	mysql_free_result(res);

	return(uOwner);

}//unsigned uGetOrganization(char *cLabel)


void ServerJobQueue(char *cServer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	ConnectDb();

	//debug only
	printf("ServerJobQueue(%s)\n",cServer);

	sprintf(gcQuery,"SELECT tZone.cZone,tNS.cFQDN,tNSType.cLabel FROM tZone,tNSSet,tNS,tNSType,tServer WHERE"
			" tZone.uNSSet=tNSSet.uNSSet AND tNSSet.uNSSet=tNS.uNSSet AND tNS.uServer=tServer.uServer AND"
			" tNS.uNSType=tNSType.uNSType AND tServer.cLabel='%s'",cServer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("%s %s %s\n",
			field[0],field[1],field[2]);
	}
	mysql_free_result(res);

	exit(0);

}//void ServerJobQueue(char *cNameServer)


unsigned uGetNSSet(char *cNameServer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uNSSet=0;

	sprintf(gcQuery,"SELECT tNSSet.uNSSet FROM tNSSet,tNS WHERE"
			" tNSSet.uNSSet=tNS.uNSSet AND"
			" tNS.cFQDN='%s'",cNameServer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uNSSet);
	mysql_free_result(res);

	return(uNSSet);

}//unsigned uGetNSSet(char *cNameServer)


void ExportRRCSV(char *cCompany, char *cOutFile)
{
	//This function exports the data from the tResource table
	//into a CSV file. The exported data belongs to the company 
	//indicated by the cCompany argument.
	//CSV output file is specified by the cOutFile argument.
	//If empty, we will just use stdout.
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uClient=0;
	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cLabel='%s'",TextAreaSave(cCompany));
	macro_mySQLRunAndStoreText(res);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uClient);
	}
	else
	{
		fprintf(stderr,"Company %s not found.\n",cCompany);
		return;
	}

	mysql_free_result(res);

	if(cOutFile[0])
	{
		sprintf(gcQuery,"SELECT tZone.cZone,cName,tResource.uTTL,tRRType.cLabel,"
				"cParam1,cParam2,FROM_UNIXTIME(tResource.uCreatedDate) "
				"INTO OUTFILE '%s'FIELDS TERMINATED BY ',' OPTIONALLY "
				"ENCLOSED BY '\"' LINES TERMINATED BY '\n' FROM "
				"tResource,tZone,tRRType WHERE tResource.uOwner=%u "
				"AND tRRType.uRRType=tResource.uRRType AND tZone.uZone=tResource.uZone",
				cOutFile
				,uClient
			);
		macro_mySQLQueryTextError;
		printf("Export complete to %s\n",cOutFile);
	}
	else
		printf("Export to stdout is not implemented yet");


}//void ExportRRCSV(char *cCompany)

