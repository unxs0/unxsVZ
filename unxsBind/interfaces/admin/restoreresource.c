/*
FILE 
	restoreresource.c
	$Id: restoreresource.c 660 2008-12-18 18:48:48Z hus $
AUTHOR
	(C) 2006-2009 Gary Wallis  and Hugo Urquiza for Unixservice
PURPOSE
	iDNS Administration (ASP) Interface
	program file.
*/

#include "interface.h"

extern char cuView[]; //zone.c

static unsigned uDeletedResource=0;
static char cName[100]={""};
static char cuTTL[100]={""};
static char cRRType[100]={""};
static char cParam1[100]={""};
static char cNameLabel[100]={""};
static char cParam1Label[100]={""};
static char cParam2Label[100]={""};
static char cParam2[100]={""};
static char cComment[255]={""};
static unsigned uOwner=0;
static unsigned uNameServer=0;

extern unsigned uDeletedZone;
char cuDeletedZone[16]={""};

void LoadDeletedResource(unsigned uRowId);	
void RestoreRR(unsigned uRowId);
void DeleteRestoreRR(unsigned uRowId);
unsigned SelectRRType(char *cRRType); //resource.c
void RestoreUpdateSerialNum(unsigned uZone);

void ProcessRestoreResourceVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uDeletedResource"))
			sscanf(entries[i].val,"%u",&uDeletedResource);
		else if(!strcmp(entries[i].name,"uDeletedZone"))
		{
			sscanf(entries[i].val,"%u",&uDeletedZone);
			sprintf(cuDeletedZone,"%.15u",uDeletedZone);
		}
			
	}

}//void ProcessRestoreResourceVars(pentry entries[], int x)


void RestoreResourceGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uDeletedResource"))
			sscanf(gentries[i].val,"%u",&uDeletedResource);
	}
	if(uDeletedResource)
		LoadDeletedResource(uDeletedResource);
	
	htmlRestoreResource("");

}//void RestoreResourceGetHook(entry gentries[],int x)


void RestoreResourceCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"RestoreResource"))
	{
		ProcessRestoreResourceVars(entries,x);
		
		if(!strcmp(gcFunction," Restore RR"))
		{
			if(!uDeletedResource)
			{
				gcMessage="<blink>Must select a RR to restore</blink>";
				htmlRestoreResource("");
			}
			sprintf(gcNewStep,"Confirm");
			LoadDeletedResource(uDeletedResource);
			htmlRestoreResource("");
		}
		else if(!strcmp(gcFunction,"Confirm Restore RR"))
		{
			time_t luClock;
			
			LoadDeletedResource(uDeletedResource);
			RestoreRR(uDeletedResource);
			DeleteRestoreRR(uDeletedResource);
			RestoreUpdateSerialNum(uDeletedZone);
			time(&luClock);
			if(AdminSubmitJob("Modify",uNameServer,gcZone,0,luClock))
				htmlPlainTextError(mysql_error(&gMysql));
			gcMessage="RR restored ok.";
			htmlRestoreResource("");
	
		}
		
//		printf("Content-type:text/plain\n\n|%s|(%u)\n",gcFunction,strlen(gcFunction));
//		exit(0);
	}
}//void RestoreResourceCommands(pentry entries[], int x)


void SelectDeletedRR(void)
{
	sprintf(gcQuery,"SELECT uDeletedResource FROM tDeletedResource WHERE uZone=%u ORDER BY uDeletedResource LIMIT 1",uDeletedZone);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void SelectDeletedRR(void)


void htmlRestoreResource(char *cuZone)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	if(cuZone[0])
	{
		sscanf(cuZone,"%u",&uDeletedZone);
		//Load the first available RR to restore
		SelectDeletedRR();
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uDeletedResource);
			LoadDeletedResource(uDeletedResource);
		}
		
		sprintf(gcQuery,"SELECT cZone,uView FROM tZone WHERE uZone='%s'",cuZone);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(gcZone,"%.255s",field[0]);
			sprintf(cuView,"%.15s",field[1]);
		}
		mysql_free_result(res);
	}
	htmlHeader("DNS System","Header");
	htmlRestoreResourcePage("DNS System","RestoreResource.Body");
	htmlFooter("Footer");

}//void htmlRestoreResource(void)


void htmlRestoreResourcePage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect(cTemplateName);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			char cuDeletedResource[16]={""};
			char cuDeletedZone[16]={""};

			sprintf(cuDeletedResource,"%u",uDeletedResource);
			sprintf(cuDeletedZone,"%u",uDeletedZone);
			
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="idnsAdmin.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcUser;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcMessage";
			template.cpValue[7]=gcMessage;

			template.cpName[8]="uDeletedResource";
			template.cpValue[8]=cuDeletedResource;
			
			template.cpName[9]="uDeletedZone";
			template.cpValue[9]=cuDeletedZone;
			
			template.cpName[10]="cName";
			template.cpValue[10]=cName;
			
			template.cpName[11]="cRRType";
			template.cpValue[11]=cRRType;
			
			template.cpName[12]="cParam1";
			template.cpValue[12]=cParam1;
			
			template.cpName[13]="cParam2";
			template.cpValue[13]=cParam2;
			
			template.cpName[14]="cComment";
			template.cpValue[14]=cComment;
			
			template.cpName[15]="cParam1Label";
			template.cpValue[15]=cParam1Label;
			
			template.cpName[16]="cMetaParam2";
			if(strcmp(cParam2Label,"Not Used"))
			{
				char cBuffer[1024]={""};
sprintf(cBuffer,"<tr><td><a class=inputLink href=\"#\" onClick=\"javascript:window.open('?gcPage=Glossary&cLabel=%s','Glossary','height=600,width=500,status=yes,toolbar=no,menubar=no,location=no,scrollbars=1')\"><strong>%s</strong></a>\n</td><td><input type=text name=cParam2 value='%s' size=40 maxlength=255 class=type_fields_off disabled></td></tr>\n",cParam2Label,cParam2Label,cParam2);
				template.cpValue[16]=cBuffer;
			}
			else
			{
				template.cpValue[16]="";
			}
			template.cpName[17]="uTTL";
			template.cpValue[17]=cuTTL;

			template.cpName[18]="cNameLabel";
			template.cpValue[18]=cNameLabel;

			template.cpName[19]="gcNewStep";
			template.cpValue[19]=gcNewStep;

			template.cpName[20]="cZone";
			template.cpValue[20]=gcZone;

			template.cpName[21]="cuView";
			template.cpValue[21]=cuView;

			template.cpName[22]="cCustomer";
			template.cpValue[22]=gcCustomer;

			template.cpName[23]="uView";
			template.cpValue[23]=cuView;

			template.cpName[24]="";

			printf("\n<!-- Start htmlRestoreResourcePage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlRestoreResourcePage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlRestoreResourcePage()


void LoadDeletedResource(unsigned uRowId)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	sprintf(gcQuery,"SELECT tDeletedResource.uDeletedResource,tDeletedResource.uZone,tDeletedResource.cName,tDeletedResource.uTTL,tRRType.cLabel,tDeletedResource.cParam1,tDeletedResource.cParam2,tDeletedResource.cComment,tDeletedResource.uOwner,tRRType.cParam1Label,tRRType.cParam2Label,tRRType.cNameLabel,tZone.cZone,tZone.uNameServer FROM tDeletedResource,tRRType,tZone WHERE tRRType.uRRType=tDeletedResource.uRRType AND tZone.uZone=tDeletedResource.uZone AND tDeletedResource.uDeletedResource=%u",uRowId);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uDeletedResource);
		sscanf(field[1],"%u",&uDeletedZone);
		sprintf(cName,"%.255s",field[2]);
		sprintf(cuTTL,"%.15s",field[3]);
		sprintf(cRRType,"%.99s",field[4]);
		sprintf(cParam1,"%.99s",field[5]);
		sprintf(cParam2,"%.99s",field[6]);
		sprintf(cComment,"%255s",field[7]);
		sscanf(field[8],"%u",&uOwner);
		sprintf(cParam1Label,"%.31s",field[9]);
		sprintf(cParam2Label,"%.31s",field[10]);
		sprintf(cNameLabel,"%.31s",field[11]);
		sprintf(gcZone,"%.255s",field[12]);
		sscanf(field[13],"%u",&uNameServer);
	}
	else
		gcMessage="<blink>Could not load record</blink>";

}//void LoadDeletedResource(unsigned uRowId)


void RestoreRR(unsigned uRowId)
{
	strcat(cComment,"- restored");
	sprintf(gcQuery,"INSERT INTO tResource SET uResource='%u',uZone='%u',cName='%s',uRRType='%u',uTTL='%s',cParam1='%s',cParam2='%s',cComment='%s',uOwner='%u',uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uDeletedResource
			,uDeletedZone
			,cName
			,SelectRRType(cRRType)
			,cuTTL
			,cParam1
			,cParam2
			,cComment
			,uOwner
			,guLoginClient
	       );
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	iDNSLog((unsigned)mysql_insert_id(&gMysql),"tResource","New - Restore");
	
}//void RestoreRR(unsigned uRowId)


void DeleteRestoreRR(unsigned uRowId)
{
	sprintf(gcQuery,"DELETE FROM tDeletedResource WHERE uDeletedResource=%u",uDeletedResource);
	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	iDNSLog(uDeletedResource,"tDeletedResource","Del");

}//void DeleteRestoreRR(unsigned uRowId)


void RestoreUpdateSerialNum(unsigned uZone)
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

}//void void RestoreUpdateSerialNum(unsigned uZone)



