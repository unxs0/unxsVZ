/*
FILE 
	{{cProject}}/interfaces/bootstrap/main.c
	template/new/bootstrap/main.c
AUTHOR/LEGAL
	(C) 2010-2018 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	bootstrap user default home page code
*/

#include "interface.h"
#include "/usr/include/openisp/upload.h"

extern unsigned guBrowserFirefox;//main.c
extern unsigned guStatusFilter;//joboffer.c
unsigned uStatusFilter=0;
extern char gcCtHostname[];
static char cCurPasswd[32]={""};
static char cPasswd[32]={""};
static char cPasswd2[32]={""};
static char *gcFiveIn="out";

unsigned uYear=0;
unsigned uSize=0;
unsigned uMaxBid=0;
unsigned uBrand=0;
char cBrand[32]={""};
char cModel[32]={""};
char cColors[100]={""};
char dStart[32]={""};
char dEnd[32]={""};
char *cDescription="";

char cLink1[100]={""};
char cLink1Title[100]={""};
char *cLink1Desc="";

char cLink2[100]={""};
char cLink2Title[100]={""};
char *cLink2Desc="";

char cLink3[100]={""};
char cLink3Title[100]={""};
char *cLink3Desc="";

char cJobOwner[33]={""};
char cuStatus[33]={""};
unsigned uStatus=0;
char cNewOwner[100]={""};

//TOC
void ProcessUserVars(pentry entries[], int x);
void UserGetHook(entry gentries[],int x);
unsigned uNoUpper(const char *cPasswd);
unsigned uNoDigit(const char *cPasswd);
unsigned uNoLower(const char *cPasswd);
unsigned uChangePassword(const char *cPasswd);
void EncryptPasswdWithSalt(char *pw, char *salt);
char *cGetPasswd(char *gcLogin);
unsigned uValidPasswd(char *cPasswd,unsigned guLoginClient);
//extern
void unxsvzLog(unsigned uTablePK,char *cTableName,char *cLogEntry,unsigned guPermLevel,unsigned guLoginClient,char *gcLogin,char *gcHost);
void htmlOperationsInfo(void);
void htmlLoginInfo(void);

static unsigned uDay=0;
unsigned guValidJobLoaded=0;
unsigned guItemJob=0;
unsigned guItem=0;
unsigned uJobToAssign=0;

extern char *gcInvoiceShow;
extern char *gcSummaryShow;


void SendEmail(char *cMsg,char *cMailTo,char *cFrom,char *cSubject,char *cBcc)
{
	FILE *pp;
	pid_t pidChild;

	pidChild=fork();
	if(pidChild!=0)
		return;

	fclose(stdout);
	pp=popen("/usr/lib/sendmail -t","w");
	if(pp==NULL)
		return;
			
	fprintf(pp,"To: %s\n",cMailTo);
	if(cBcc[0]) fprintf(pp,"Bcc: %s\n",cBcc);
	fprintf(pp,"From: %s\n",cFrom);
	fprintf(pp,"Subject: %s\n",cSubject);

	fprintf(pp,"%s\n",cMsg);

	fprintf(pp,".\n");

	pclose(pp);

	exit(0);

}//void SendEmail()


void ProcessJobOfferVars(pentry entries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		
		if(!strcmp(entries[i].name,"uYear"))
			sscanf(entries[i].val,"%u",&uYear);
		else if(!strcmp(entries[i].name,"uSize"))
			sscanf(entries[i].val,"%u",&uSize);
		else if(!strcmp(entries[i].name,"uMaxBid"))
			sscanf(entries[i].val,"%u",&uMaxBid);
		else if(!strcmp(entries[i].name,"uJobOffer"))
			sscanf(entries[i].val,"%u",&guJobOffer);
		else if(!strcmp(entries[i].name,"uStatusFilter"))
			sscanf(entries[i].val,"%u",&uStatusFilter);
		else if(!strcmp(entries[i].name,"uBrand"))
		{
			sscanf(entries[i].val,"%u",&uBrand);
			sprintf(cBrand,"%.31s",cForeignKey("tBrand","cLabel",uBrand));
		}
		else if(!strcmp(entries[i].name,"cModel"))
			sprintf(cModel,"%.31s",entries[i].val);
		else if(!strcmp(entries[i].name,"cColors"))
			sprintf(cColors,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"cDescription"))
			cDescription=entries[i].val;
		else if(!strcmp(entries[i].name,"dStart"))
			sprintf(dStart,"%.31s",entries[i].val);
		else if(!strcmp(entries[i].name,"dEnd"))
			sprintf(dEnd,"%.31s",entries[i].val);
		else if(!strcmp(entries[i].name,"uStatus"))
			sscanf(entries[i].val,"%u",&uStatus);
		else if(!strcmp(entries[i].name,"cNewOwner"))
			sprintf(cNewOwner,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"uItem"))
			sscanf(entries[i].val,"%u",&guItem);
		else if(!strcmp(entries[i].name,"uJobToAssign"))
			sscanf(entries[i].val,"%u",&uJobToAssign);
		else if(!strcmp(entries[i].name,"cJobOwner"))
			sprintf(cJobOwner,"%.32s",entries[i].val);
	}

}//void ProcessJobOfferVars(pentry entries[], int x)


void ProcessCalendarVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"guMonth"))
			sscanf(entries[i].val,"%u",&guMonth);
		else if(!strcmp(entries[i].name,"guYear"))
			sscanf(entries[i].val,"%u",&guYear);
		else if(!strcmp(entries[i].name,"uDay"))
			sscanf(entries[i].val,"%u",&uDay);
	}

}//void ProcessCalendarVars(pentry entries[], int x)


void ProcessUserVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cCurPasswd"))
			sprintf(cCurPasswd,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cPasswd"))
			sprintf(cPasswd,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cPasswd2"))
			sprintf(cPasswd2,"%.32s",entries[i].val);
	}

}//void ProcessUserVars(pentry entries[], int x)


void ToggleAvailableDay(unsigned uYear,unsigned uMonth,unsigned uDay)
{
        MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT uCalendar FROM tCalendar"
				" WHERE (uVendor=%u OR uVendor=%u) AND dDate='%u-%u-%u'",guLoginClient,guOrg,uYear,uMonth,uDay);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return;
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"DELETE FROM tCalendar WHERE uCalendar=%s",field[0]);
		mysql_query(&gMysql,gcQuery);
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tCalendar"
				" SET uVendor=%u,uOwner=%u,dDate='%u-%u-%u',"
				"uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=%u",guLoginClient,guOrg,uYear,uMonth,uDay,guLoginClient);
		mysql_query(&gMysql,gcQuery);
	}
	mysql_free_result(res);
}//


void DelImage(void)
{
	if(!guJobOffer)
	{
		gcMessage="No guJobOffer!";
		htmlJobOffer();
	}
	unsigned uImage=0;
	sscanf(gcFunction,"DelImage%u",&uImage);
	if(!uImage)
	{
		gcMessage="No uImage!";
		htmlJobOffer();
	}
	if(guPermLevel>=10)
		sprintf(gcQuery,"UPDATE tJobOffer SET cLink%u='',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJobOffer=%u",
			uImage,guLoginClient,guJobOffer);
	else
		sprintf(gcQuery,"UPDATE tJobOffer SET cLink%u='',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE uJobOffer=%u AND uOwner=%u",
			uImage,guLoginClient,
			guJobOffer,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		gcMessage="Update image error!";

}//void DelImage(void)


void htmlReport(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<font size=-1><pre>\n");
	fprintf(fp,"Item Report\n");
	sprintf(gcQuery,"SELECT FORMAT(SUM(tItemJob.uQuantity*tItem.mValue),2),tItem.cLabel,SUM(tItemJob.uQuantity) FROM tItemJob,tItem"
				" WHERE tItemJob.uItem=tItem.uItem GROUP BY tItem.uItem");
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	float fSubtotal=0.0;
	float fTotal=0.0;
	unsigned uCount=1;
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"  %u) %s %s $%s\n",uCount++,field[2],field[1],field[0]);
		sscanf(field[0],"%f",&fSubtotal);
		fTotal+=fSubtotal;
	}

	fprintf(fp,"Total: $%2.2f</pre>\n",fTotal);

}//void htmlReport(FILE *fp)


void AdminGetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"gcFunction"))
			sprintf(gcFunction,"%.32s",gentries[i].val);
	}

	if(!strcmp(gcFunction,"Report"))
	{
		printf("Content-type: text/html\n\n");
  		printf("<div class=\"card-body\">\n");
  		printf("<div class=\"row\">\n");
		htmlReport(stdout);
  		printf("</div>\n");
  		printf("</div>\n");
		exit(0);
	}

	htmlAdmin();

}//void UserGetHook(entry gentries[],int x)


void CalendarGetHook(entry gentries[],int x)
{
	register int i;
	unsigned uDay=0;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uMonth"))
			sscanf(gentries[i].val,"%u",&guMonth);
		else if(!strcmp(gentries[i].name,"uYear"))
			sscanf(gentries[i].val,"%u",&guYear);
		else if(!strcmp(gentries[i].name,"uDay"))
			sscanf(gentries[i].val,"%u",&uDay);
	}

	if(!strcmp(gcFunction,"ToggleDay")&&uDay&&guMonth&&guYear)
		ToggleAvailableDay(guYear,guMonth,uDay);

	htmlCalendar();

}//void CalendarGetHook(entry gentries[],int x)


void UserGetHook(entry gentries[],int x)
{
	if(!strcmp(gcFunction,"LoginInfo"))
		htmlLoginInfo();

	htmlUser();

}//void UserGetHook(entry gentries[],int x)

void IfJobDoesNotExistCreate(unsigned uJob);
void IfJobDoesNotExistCreate(unsigned uJob)
{
	if(guPermLevel<10)
		return;

	MYSQL_RES *res;
	sprintf(gcQuery,"SELECT uJobOffer FROM tJobOffer"
				" WHERE uJobOffer=%u",uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{
		mysql_free_result(res);
		printf("Set-Cookie: {{cProject}}JobOffer=%u; secure; httponly; samesite=strict;\n",guJobOffer);
		htmlJobOffer();
	}

	//Create new job offer
	sprintf(gcQuery,"INSERT INTO tJobOffer SET "
				"uJobOffer=%u,"
				"cLabel='New Job Tag %u',"
				"uStatus=1,"
				"uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=%u,"
				"cDescription=CONCAT(NOW(),': ',(SELECT cLabel FROM tStatus WHERE uStatus=1))"
							,uJob,
							uJob,
							guLoginClient,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Error i1 pruebe mas tarde!";
		//debug only
		//gcMessage=gcQuery;
		htmlJobOffer();
	}
	gcMessage="Trabajo nuevo creado";
	htmlJobOffer();
}//void IfJobDoesNotExistCreate(unsigned uJob)


void ItemJob(int iAdd);
void DeleteItemJob(void);


void JobOfferGetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uJobOffer"))
			sscanf(gentries[i].val,"%u",&guJobOffer);
		else if(!strcmp(gentries[i].name,"uItemJob"))
			sscanf(gentries[i].val,"%u",&guItemJob);
		else if(!strcmp(gentries[i].name,"guStatusFilter"))
		{
			sscanf(gentries[i].val,"%u",&guStatusFilter);
		}
		//Scanned job
		else if(!strcmp(gentries[i].name,"uJob"))
		{
			sscanf(gentries[i].val,"%u",&guJobOffer);
			IfJobDoesNotExistCreate(guJobOffer);
		}
	}
	
	if(!strncmp(gcFunction,"DelImage",8))
		DelImage();
	else if(!strncmp(gcFunction,"AddItemJob",10))
		ItemJob(1);
	else if(!strncmp(gcFunction,"DelItemJob",10))
		ItemJob((-1));
	else if(!strncmp(gcFunction,"DeleteItem",10))
		DeleteItemJob();

	else if(!strcmp(gcFunction,"IncSF"))
	{
		if(guStatusFilter<16)
			guStatusFilter++;
		else
			guStatusFilter=0;
	}
	else if(!strcmp(gcFunction,"DecSF"))
	{
		if(guStatusFilter>0)
			guStatusFilter--;
	}
	else if(!strcmp(gcFunction,"DelSF"))
	{
		guStatusFilter=0;
	}
	printf("Set-Cookie: guStatusFilter=%u; secure; httponly; samesite=strict;\n",guStatusFilter);
		

	htmlJobOffer();

}//void JobOfferGetHook(entry gentries[],int x)


unsigned uNoUpper(const char *cPasswd)
{
	register int i;
	for(i=0;cPasswd[i];i++)
		if(isupper(cPasswd[i])) return(0);
	return(1);
}//unsigned uNoUpper(const char *cPasswd)


unsigned uNoLower(const char *cPasswd)
{
	register int i;
	for(i=0;cPasswd[i];i++)
		if(islower(cPasswd[i])) return(0);
	return(1);
}//unsigned uNoLower(const char *cPasswd)


unsigned uNoDigit(const char *cPasswd)
{
	register int i;
	for(i=0;cPasswd[i];i++)
		if(isdigit(cPasswd[i])) return(0);
	return(1);
}//unsigned uNoDigit(const char *cPasswd)


//Passwd stuff
static unsigned char itoa64[] =         /* 0 ... 63 => ascii - 64 */
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void to64(s, v, n)
  register char *s;
  register long long v;
  register int n;
{
    while (--n >= 0) {
        *s++ = itoa64[v&0x3f];
        v >>= 6;
    }
}//void to64(s, v, n)


unsigned uChangePassword(const char *cPasswd)
{
	char cBuffer[100]={""};
	char cSalt[16]={"$1$23abc123$"};//TODO set to random salt;
	(void)srand((long long)time((time_t *)NULL)*getpid());
	to64(&cSalt[3],rand(),8);

	sprintf(cBuffer,"%.99s",cPasswd);
	EncryptPasswdWithSalt(cBuffer,cSalt);

	//Here we allow private passwords
	sprintf(gcQuery,"UPDATE tAuthorize SET cIpMask='',cPasswd='%.99s',cClrPasswd='',"
			"uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE cLabel='%s'",
			cBuffer,guLoginClient,gcLogin);
	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		unxsvzLog(guLoginClient,"tAuthorize","Password Not Changed Error1",guPermLevel,guLoginClient,gcLogin,gcHost);
		return(1);
	}
	if(mysql_affected_rows(&gMysql)<1)
	{
		unxsvzLog(guLoginClient,"tAuthorize","Password Not Changed Error2",guPermLevel,guLoginClient,gcLogin,gcHost);
		return(1);
	}
	unxsvzLog(guLoginClient,"tAuthorize","Password Changed",guPermLevel,guLoginClient,gcLogin,gcHost);
	return(0);
}//unsigned uChangePassword(const char *cPasswd)


void ChangeJobOffer(unsigned uJobOffer,unsigned uJobToAssign);
void ChangeJobOffer(unsigned uJobOffer ,unsigned uJobToAssign)
{
	if(guPermLevel<10)
		return;

	//Change
	sprintf(gcQuery,"UPDATE tJobOffer SET uJobOffer=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE uJobOffer=%u",
					uJobToAssign,guLoginClient,
						uJobOffer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error NJO!";
		return;
	}

	if(mysql_affected_rows(&gMysql)!=1)
	{
		gcMessage="uJobOffer not found!";
		return;
	}


	//Fix Calendar
	sprintf(gcQuery,"UPDATE tCalendar SET uJobOffer=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE uJobOffer=%u",
					uJobToAssign,guLoginClient,
						uJobOffer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error UC-NJO!";
		return;
	}

	//Fix Invoice
	sprintf(gcQuery,"UPDATE tItemJob SET uJobOffer=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE uJobOffer=%u",
					uJobToAssign,guLoginClient,
						uJobOffer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error UIJ-NJO!";
		return;
	}
	gcMessage="Nuevo numero de trabajo asignado";

}//void ChangeJobOffer(unsigned uItemJob)


void SendRemindEmail(void)
{
	sprintf(gcQuery,"SELECT tJobOffer.cLabel,tClient.cEmail FROM tJobOffer,tClient"
			" WHERE tJobOffer.uOwner=tClient.uClient AND uStatus=10");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s101) try again later!";
		htmlJobOffer();
	}
	MYSQL_RES *res;
	MYSQL_ROW field;
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"%s tu trabajo %s esta listo para retirar.\n"
				"Importante: Debe retirar trabajos terminados dentro de los 15 dias. Pasado"
				" este lapso, le sera cobrado U$A 5.00/Semana por servicio de guarda.",field[1],field[0]);
		//void SendEmail(char *cMsg,char *cMailTo,char *cFrom,char *cSubject,char *cBcc)
		SendEmail(gcQuery,field[1],"unxsak@unxs.io","ArregloKites: Tu Trabajo Esta Listo!","unxsak@unxs.io");
	}

}//void SendRemindEmail(void)

//CalendarCommands
void UserCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Calendar"))
	{
		ProcessCalendarVars(entries,x);
		if(!strcmp(gcFunction,"ToggleDay"))
		{
			if(guYear && guMonth && uDay);
			ToggleAvailableDay(guYear,guMonth,uDay);
			htmlCalendar();
		}
	}
	else if(!strcmp(gcPage,"Admin"))
	{
		if(!strcmp(gcFunction,"Remind"))
		{
			gcMessage=": Remind email sent.";
			SendRemindEmail();
			htmlAdmin();
		}
	}
	else if(!strcmp(gcPage,"JobOffer"))
	{
		ProcessJobOfferVars(entries,x);
		if(!strcmp(gcFunction,"SetJobOffer"))
		{
			printf("Set-Cookie: {{cProject}}JobOffer=%u; secure; httponly; samesite=strict;\n",guJobOffer);
			//sscanf(cForeignKey("tStatus","uStatus",guJobOffer),"%u",&uStatusFilter);
			//if(uStatusFilter)
			//{
			//	guStatusFilter=uStatusFilter;
			//	printf("Set-Cookie: guStatusFilter=%u; secure; httponly; samesite=strict;\n",guStatusFilter);
			//}
		}
		else if(!strcmp(gcFunction,"SetStatusFilter"))
		{
			guStatusFilter=uStatusFilter;
			printf("Set-Cookie: guStatusFilter=%u; secure; httponly; samesite=strict;\n",guStatusFilter);
		}
		else if(!strcmp(gcFunction,"ChangeJobOffer"))
		{
			if(!guJobOffer)
				gcMessage="No hay numero de trabajo";
			if(!uJobToAssign)
				gcMessage="No hay numero a asignar";
			if(guPermLevel<10)
				gcMessage="No permitido";
			if(gcMessage[0])
				htmlJobOffer();
			ChangeJobOffer(guJobOffer,uJobToAssign);
			htmlJobOffer();
		}
		else if(!strcmp(gcFunction,"ChangeOwner") && guPermLevel>=10)
		{
			if(!cNewOwner[0])
				gcMessage="Debe proveer nuevo due&ntilde;o";
			if(!guJobOffer)
				gcMessage="No hay numero de trabajo";
			if(gcMessage[0])
				htmlJobOffer();
				

			sprintf(gcQuery,"SELECT uCertClient FROM tAuthorize WHERE cLabel='%.32s'",cNewOwner);
			mysql_query(&gMysql,gcQuery);
			MYSQL_RES *res;
			MYSQL_ROW field;
			unsigned uNewOwner=0;
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)==1)
			{
				field=mysql_fetch_row(res);
				sscanf(field[0],"%u",&uNewOwner);
			}
			else
			{
				gcMessage="No hay tal persona!";
				htmlJobOffer();
			}

			if(uNewOwner)
			{
				sprintf(gcQuery,"UPDATE tJobOffer SET uOwner=%u WHERE uJobOffer=%u",uNewOwner,guJobOffer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					gcMessage="Error inesperado pruebe mas tarde";
				//Update Calendar also
				sprintf(gcQuery,"UPDATE tCalendar SET uVendor=%u WHERE uJobOffer=%u",uNewOwner,guJobOffer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					gcMessage="Error inesperado tCalendar pruebe mas tarde";
			}
			else
			{
				gcMessage="Error inesperado u1 pruebe mas tarde";
			}
			htmlJobOffer();
		}
		else if(!strcmp(gcFunction,"AddItem") && guPermLevel>=10)
		{
			gcMessage="";
			if(!guItem)
				gcMessage="No hay guItem";
			if(!guJobOffer)
				gcMessage="No hay guJobOffer";
			if(gcMessage[0])
				htmlJobOffer();
			sprintf(gcQuery,"SELECT uItemJob FROM tItemJob WHERE uItem=%u AND uJobOffer=%u",guItem,guJobOffer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Error inesperado s31 pruebe mas tarde!";
				htmlJobOffer();
			}
			MYSQL_RES *res;
			MYSQL_ROW field;
			res=mysql_store_result(&gMysql);
			unsigned uItem=0;
			if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&uItem);
				sprintf(gcQuery,"UPDATE tItemJob SET uQuantity=uQuantity+1,"
						"uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uItemJob=%u",
						guLoginClient,uItem);
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO tItemJob SET uItem=%u,uJobOffer=%u,uQuantity=1,"
						"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						guItem,guJobOffer,
						guLoginClient);
			}
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Error inesperado i31 pruebe mas tarde!";
				htmlJobOffer();
			}
			gcInvoiceShow="show";
			gcSummaryShow="";
			htmlJobOffer();
		}
		else if(!strcmp(gcFunction,"SetStatus") && guPermLevel>=10)
		{
			if(!uStatus)
				gcMessage="No hay estado nuevo";
			else if(!guJobOffer)
				gcMessage="No hay numero de trabajo";
			else
			{
				sprintf(gcQuery,"UPDATE tJobOffer SET uStatus=%u,"
					"cDescription=CONCAT(cDescription,'\n',NOW(),': ',(SELECT cLabel FROM tStatus WHERE uStatus=%u))"
					" WHERE uJobOffer=%u",
						uStatus,
						uStatus,
						guJobOffer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="Error inesperado u3 pruebe mas tarde!";
					htmlJobOffer();
				}
				LoadJobOfferData(guJobOffer);
				if(cJobOwner[0] && strcmp(gcLogin,cJobOwner))
				{
					sprintf(gcQuery,"El estado de tu trabajo ha cambiado.\n"
						"%s %s %um %u: %s.\n"
					"Ver: https://portal.arreglokites.com/unxsAKApp?uJobOffer=%u",
						cForeignKey("tBrand","cLabel",uBrand),cModel,uSize,uYear,cuStatus,
						guJobOffer);
					SendEmail(gcQuery,cJobOwner,"unxsak@unxs.io","ArregloKites Cambio de Estado","unxsak@unxs.io");
				}
			}
			htmlJobOffer();
		}
		else if(!strcmp(gcFunction,"AMJobOffer"))
		{
			gcMessage="";
			if(!uYear)
				gcMessage="Falta a&ntilde;o";
			else if(!uSize)
				gcMessage="Falta tama&ntilde;o";
			else if(!cBrand[0] || !uBrand)
				gcMessage="Falta marca";
			else if(!cModel[0])
				gcMessage="Falta modelo";
			else if(!cDescription[0])
				gcMessage="Falta descripci&oacute;n";
			else if(!dStart[0])
				gcMessage="Falta fecha inicial";
			else if(!dEnd[0])
				gcMessage="Falta fecha final";
			if(gcMessage[0])
				htmlJobOffer();
			unsigned uStartMonth=0,uStartDay=0,uStartYear=0;
			unsigned uEndMonth=0,uEndDay=0,uEndYear=0;
			sscanf(dStart,"%u-%u-%u",&uStartYear,&uStartMonth,&uStartDay);
			sscanf(dEnd,"%u-%u-%u",&uEndYear,&uEndMonth,&uEndDay);
			if(!uStartMonth || !uStartDay || !uStartYear ||
				!uEndMonth || !uEndDay || !uEndYear)
			{
				gcMessage="Error de formato de fecha";
				htmlJobOffer();
			}
			char cStart[32],cEnd[32];
			sprintf(cStart,"%u%u%u",uStartYear,uStartMonth,uStartDay);
			sprintf(cEnd,"%u%u%u",uEndYear,uEndMonth,uEndDay);
			unsigned uStart=0,uEnd=0;
			sscanf(cStart,"%u",&uStart);
			sscanf(cEnd,"%u",&uEnd);
			if(uEnd<=uStart)
			{
				gcMessage="Fecha de inicio debe ser anterior a la fecha final";
				htmlJobOffer();
			}

			unsigned uJobOffer=0;

			//Update by deleting selected job offer and then adding with same uK
			//This -1 thing has to be fixed... see joboffer.c
			if(guJobOffer && guJobOffer!= (-1))
			{

				if(guPermLevel>=10)
					sprintf(gcQuery,"UPDATE tJobOffer SET "
					"cLabel='%s %s %um %u',"
					"uBrand=%u,cModel='%s',uYear=%u,"
					"uSize=%u,uMaxBid=%u,"
					"cDescription='%s',"
					"cColors='%s',"
					"dStart='%s',dEnd='%s',"
					"uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJobOffer=%u",
								cBrand,TextAreaSave(cModel),uSize,uYear,
								uBrand,TextAreaSave(cModel),uYear,
								uSize,uMaxBid,
								TextAreaSave(cDescription),
								TextAreaSave(cColors),
								dStart,dEnd,
								guLoginClient,guJobOffer);
				else
					sprintf(gcQuery,"UPDATE tJobOffer SET "
					"cLabel='%s %s %um %u',"
					"uBrand=%u,cModel='%s',uYear=%u,"
					"uSize=%u,uMaxBid=%u,"
					"cDescription='%s',"
					"cColors='%s',"
					"dStart='%s',dEnd='%s',"
					"uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJobOffer=%u AND uOwner=%u",
								cBrand,TextAreaSave(cModel),uSize,uYear,
								uBrand,TextAreaSave(cModel),uYear,
								uSize,uMaxBid,
								TextAreaSave(cDescription),
								TextAreaSave(cColors),
								dStart,dEnd,
								guLoginClient,guJobOffer,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					gcMessage="Error u4 prueba mas tarde!";
					//debug only
					//gcMessage=gcQuery;
					htmlJobOffer();
				}
				uJobOffer=guJobOffer;
				if(mysql_affected_rows(&gMysql)!=1)
				{
					gcMessage="Error inesperado pruebe de nuevo por favor";
					guJobOffer=0;
					printf("Set-Cookie: {{cProject}}JobOffer=%u; secure; httponly; samesite=strict;\n",guJobOffer);
					htmlJobOffer();
				}
				if(strcmp(gcLogin,cJobOwner) && cJobOwner[0])
				{
					sprintf(gcQuery,"Hay algo nuevo con tu trabajo.\n"
						"%s %s %um %u: %s.\n"
					"Ver: https://portal.arreglokites.com/unxsAKApp?uJobOffer=%u",
						cForeignKey("tBrand","cLabel",uBrand),cModel,uSize,uYear,cuStatus,
						guJobOffer);
					SendEmail(gcQuery,cJobOwner,"unxsak@unxs.io","ArregloKites Novedades","unxsak@unxs.io");
				}
			}
			else
			{
				//Create new job offer
				sprintf(gcQuery,"INSERT INTO tJobOffer SET "
					"cLabel='%s %s %um %u',"
					"uBrand=%u,cModel='%s',uYear=%u,"
					"uSize=%u,uMaxBid=%u,"
					"cDescription=CONCAT(NOW(),': ',(SELECT cLabel FROM tStatus WHERE uStatus=1),'\n','%s'),"
					"cColors='%s',"
					"dStart='%s',dEnd='%s',"
					"uStatus=1,"
					"uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=%u",
								cBrand,TextAreaSave(cModel),uSize,uYear,
								uBrand,TextAreaSave(cModel),uYear,
								uSize,uMaxBid,
								TextAreaSave(cDescription),
								TextAreaSave(cColors),
								dStart,dEnd,
								guLoginClient,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				uJobOffer=mysql_insert_id(&gMysql);
				if(mysql_errno(&gMysql) || !uJobOffer)
				{
					gcMessage="Error inesperado (i0) pruebe mas tarde!";
					//debug only
					//gcMessage="D5";
					htmlJobOffer();
				}
				if(!uJobOffer)
				{
					gcMessage="Error inesperado (i01) pruebe mas tarde!";
					htmlJobOffer();
				}
				guJobOffer=uJobOffer;
				//Internal email
				sprintf(gcQuery,"New Job\n"
						"%s %s %um %u: %s.\n"
						"Ver: https://portal.arreglokites.com/unxsAKApp?uJobOffer=%u",
							cForeignKey("tBrand","cLabel",uBrand),cModel,uSize,uYear,cuStatus,
							guJobOffer);
				SendEmail(gcQuery,"unxsak@unxs.io","unxsak@unxs.io","ArregloKites New Job","unxsak@unxs.io");
			}

			printf("Set-Cookie: {{cProject}}JobOffer=%u; secure; httponly; samesite=strict;\n",uJobOffer);

			//Remove all existing for same job
			if(!uJobOffer)
			{
				gcMessage="Error inesperado (ui0) pruebe mas tarde!";
				htmlJobOffer();
			}
			sprintf(gcQuery,"DELETE FROM tCalendar WHERE uVendor=%u AND uJobOffer=%u",
							guLoginClient,uJobOffer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Error inesperado (22) pruebe mas tarde!";
				htmlJobOffer();
			}

			//Add job offer as available for range of days to calendar	
			sprintf(gcQuery,"SELECT ADDDATE('%u-%u-%u', INTERVAL @i:=@i+1 DAY) AS DAY FROM"
					" ( SELECT a.a FROM (SELECT 0 AS a UNION ALL SELECT 1 UNION ALL"
					" SELECT 2 UNION ALL SELECT 3 UNION ALL SELECT 4 UNION ALL"
					" SELECT 5 UNION ALL SELECT 6 UNION ALL SELECT 7 UNION ALL"
					" SELECT 8 UNION ALL SELECT 9)"
					" AS a CROSS JOIN (SELECT 0 AS a UNION ALL SELECT 1 UNION ALL"
					" SELECT 2 UNION ALL SELECT 3 UNION ALL SELECT 4 UNION ALL"
					" SELECT 5 UNION ALL SELECT 6 UNION ALL SELECT 7 UNION ALL"
					" SELECT 8 UNION ALL SELECT 9)"
					" AS b CROSS JOIN (SELECT 0 AS a UNION ALL SELECT 1 UNION ALL"
					" SELECT 2 UNION ALL SELECT 3 UNION ALL SELECT 4 UNION ALL"
					" SELECT 5 UNION ALL SELECT 6 UNION ALL SELECT 7 UNION ALL"
					" SELECT 8 UNION ALL SELECT 9)"
					" AS c ) a JOIN (SELECT @i := -1) r1 WHERE"
					" @i < DATEDIFF('%u-%u-%u', '%u-%u-%u');",
										uStartYear,uStartMonth,uStartDay,
										uEndYear,uEndMonth,uEndDay,
										uStartYear,uStartMonth,uStartDay);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage="Error inesperado (23) pruebe mas tarde!";
				htmlJobOffer();
			}
			MYSQL_RES *res;
			MYSQL_ROW field;
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)>0)
			{
				while((field=mysql_fetch_row(res)))
				{
					sprintf(gcQuery,"INSERT INTO tCalendar"
					" SET uJobOffer=%u,uVendor=%u,uOwner=%u,dDate='%s',"
					"uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=%u",
						uJobOffer,guLoginClient,guOrg,field[0],guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						gcMessage="Error inesperado (i30) pruebe mas tarde!";
						htmlJobOffer();
					}
				}//while
			}//if rows
			else
			{
				gcMessage="Error inesperado (i31) pruebe mas tarde!";
				htmlJobOffer();
			}
			mysql_free_result(res);
			htmlJobOffer();
		}
	}
	else if(!strcmp(gcPage,"User"))
	{
		ProcessUserVars(entries,x);
		if(!strcmp(gcFunction,"Change Password") || !strcmp(gcFunction,"ChangePassword"))
		{
			gcCPShow="show";
			if(guPermLevel>10)
			{
				gcFiveIn="in";
				gcMessage="Error: Usuarios administrativos no pueden cambiar su contrase&ntilde;a aqui.";
				htmlUser();
			}
			if(!cCurPasswd[0])
			{
				gcFiveIn="in";
				gcMessage="Error: Debe ingresar su contrase&ntilde;a.";
				htmlUser();
			}
			if(!uValidPasswd(cCurPasswd,guLoginClient))
			{
				gcFiveIn="in";
				gcMessage="Error: Contrase&ntilde;a actual inv&aacute;lida.";
				htmlUser();
			}
			if(!cPasswd[0] || !cPasswd2[0] || strcmp(cPasswd,cPasswd2))
			{
				gcFiveIn="in";
				gcMessage="Error: Debe ingresar la misma contrase&ntilde;a dos veces.";
				htmlUser();
			}
			if(!strcmp(cCurPasswd,cPasswd))
			{
				gcFiveIn="in";
				gcMessage="Error: Contrase&ntilde;a nueva is igual al actual.";
				htmlUser();
			}
			if(strlen(cPasswd)<8)
			{
				gcFiveIn="in";
				gcMessage="Error: Su contrase&ntilde;a debe tener al menos 8 caracteres.";
				htmlUser();
			}
			if(strstr(cPasswd,gcLogin) || strstr(gcLogin,cPasswd) ||
				strstr(cPasswd,gcName) || strstr(gcName,cPasswd) ||
				strstr(gcOrgName,cPasswd) || strstr(cPasswd,gcOrgName))
			{
				gcFiveIn="in";
				gcMessage="Error: Su contrase&ntilde;a no debe estar relacionado con su usuario o empresa.";
				htmlUser();
			}
			if(uNoUpper(cPasswd) || uNoLower(cPasswd) || uNoDigit(cPasswd))
			{
				gcFiveIn="in";
				gcMessage="Error: Su contrase&ntilde;a debera contener minusculas, mayusculas y un numero.";
				htmlUser();
			}
			if(uChangePassword(cPasswd))
			{
				gcFiveIn="in";
				gcMessage="Error: Su contrase&ntilde;a no fue cambiado por favor contactar administrador.";
				htmlUser();
			}
			gcCPShow="";
			gcMessage="Su contrase&ntilde;a fue cambiado. Debera ingresar/logearse de nuevo.";
			SendEmail("Si usted no cambio su contrasena avisenos de inmediato! Gracias!",
				gcLogin,"unxsak@unxs.io","Portal AK Cambio de Contrasena","unxsak@unxs.io");
			htmlUser();
		}
	}

}//void UserCommands(pentry entries[], int x)


void htmlSignUp(void)
{
	
	htmlHeader("Sign Up","Default.Header");
	htmlUserPage("Sign Up","SignUp.Body");
	htmlFooter("Default.Footer");

}//void htmlSignUp(void)


void NewCodeAndLinkEmail(unsigned uAuthorize,char *cEmail,char *cServer)
{
	char cMsg[512]={""};
	char cEmailCode[32]={""};

	(void)srand((long long)time((time_t *)NULL)*getpid()+77);
	register int i;
	for(i=0;i<16;i++)
		sprintf(cEmailCode+i,"%x",rand()%16);

	sprintf(gcQuery,"UPDATE tAuthorize SET cIpMask='%.20s',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uAuthorize=%u",cEmailCode,uAuthorize);
	mysql_query(&gMysql,gcQuery);

	sprintf(cMsg,"The link to change your password is:\n"
		"https://%s/{{cProject}}App?gcFunction=ChangePassword&gcEmailCode=%.16s&gcLogin=%s\n",
				cServer,cEmailCode,cEmail);

	SendEmail(cMsg,cEmail,"unxsak@unxs.io","unxsAK New Account Confirmation","unxsak@unxs.io");

}//void NewCodeAndLinkEmail()


void NewCodeAndEmail(unsigned uAuthorize,char *cEmail)
{
	char cMsg[256]={""};
	char cEmailCode[32]={""};

	(void)srand((long long)time((time_t *)NULL)*getpid()+77);
	register int i;
	for(i=0;i<16;i++)
		sprintf(cEmailCode+i,"%x",rand()%16);

	sprintf(gcQuery,"UPDATE tAuthorize SET cIpMask='%.20s',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uAuthorize=%u",cEmailCode,uAuthorize);
	mysql_query(&gMysql,gcQuery);

	sprintf(cMsg,"The activation code is: %.16s\n",cEmailCode);

	SendEmail(cMsg,cEmail,"unxsak@unxs.io","unxsAK New Account Confirmation","unxsak@unxs.io");

}//void NewCodeAndEMail(unsigned uAuthorize,char *cEmail)


void htmlSignUpStep1(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPerm=0;
	unsigned uAuthorize=0;

	sprintf(gcQuery,"SELECT uAuthorize,uPerm FROM tAuthorize"
			" WHERE cLabel='%s' LIMIT 1",TextAreaSave(gcLogin));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unknown error try again later";
		htmlHeader("Sign Up","Default.Header");
		htmlUserPage("Sign Up","SignUp.Body");
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uAuthorize);
		sscanf(field[1],"%u",&uPerm);
	}
	mysql_free_result(res);

	if(uAuthorize && uPerm)
	{
		sleep(3);
		gcMessage="El email usado ya esta registrado y activo.";
		htmlHeader("Sign Up","Default.Header");
		htmlUserPage("Sign Up","SignUp.Body");
		htmlFooter("Default.Footer");
	}
	else if(uAuthorize)
	{
		//sleep(3);
		NewCodeAndEmail(uAuthorize,TextAreaSave(gcLogin));
		htmlHeader("Sign Up","Default.Header");
		gcMessage="Este email ya esta esperando activacion, otro mail con un nuevo codido ha sido enviado.";
		htmlUserPage("Sign Up","SignUpStep1.Body");
		htmlFooter("Default.Footer");
	}
	else
	{
		if(!cPasswd[0] || !cPasswd2[0] || strcmp(cPasswd,cPasswd2))
		{
			gcMessage="Las contrase&ntilde;as deben ser iguales.";
			htmlHeader("Sign Up","Default.Header");
			htmlUserPage("Sign Up","SignUp.Body");
			htmlFooter("Default.Footer");//Footer exits
		}
		if(strlen(cPasswd)<8)
		{
			gcMessage="La contrase&ntilde;a debe tener minimo de 8 caracteres de largo.";
			htmlHeader("Sign Up","Default.Header");
			htmlUserPage("Sign Up","SignUp.Body");
			htmlFooter("Default.Footer");//Footer exits
		}
		if(uNoUpper(cPasswd) || uNoLower(cPasswd) || uNoDigit(cPasswd))
		{
			gcMessage="La contrase&ntilde;a debe tener una mayuscula y un numero.";
			htmlHeader("Sign Up","Default.Header");
			htmlUserPage("Sign Up","SignUp.Body");
			htmlFooter("Default.Footer");//Footer exits
		}

		char cBuffer[100]={""};
		char cSalt[16]={"$1$23abc123$"};//TODO set to random salt;
		(void)srand((long long)time((time_t *)NULL)*getpid());
		to64(&cSalt[3],rand(),8);

		sprintf(cBuffer,"%.99s",cPasswd);
		EncryptPasswdWithSalt(cBuffer,cSalt);

		//cuDiscount=10;//must be first line
		sprintf(gcQuery,"INSERT INTO tClient SET"
			" uOwner=2,"//Arreglo Kites
			" uCreatedBy=1,"
			" uCreatedDate=UNIX_TIMESTAMP(NOW()),"
			" cEmail='%1$.31s',"
			" cLabel='%1$.31s',"
			" cInfo='cuDiscount=10;//must be first line'"
				,TextAreaSave(gcLogin));
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			gcMessage="Unknown error try again later";
			htmlHeader("Sign Up","Default.Header");
			htmlUserPage("Sign Up","SignUp.Body");
			htmlFooter("Default.Footer");//Footer exits
		}

		unsigned uClient=mysql_insert_id(&gMysql);
		sprintf(gcQuery,"INSERT INTO tAuthorize SET"
			" cIpMask='',"
			" uPerm=0,"//waiting for activation
			" uCertClient=%u,"
			" cClrPasswd='%s',"
			" cPasswd='%s',"
			" uOwner=2,"//arreglo kites
			" uCreatedBy=1,"
			" uCreatedDate=UNIX_TIMESTAMP(NOW()),"
			" cLabel='%s'"
				,uClient
				,cPasswd
				,cBuffer
				,TextAreaSave(gcLogin));
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			gcMessage="Unknown error try again later";
			htmlHeader("Sign Up","Default.Header");
			htmlUserPage("Sign Up","SignUp.Body");
			htmlFooter("Default.Footer");
		}
		uAuthorize=mysql_insert_id(&gMysql);
		NewCodeAndEmail(uAuthorize,TextAreaSave(gcLogin));
		htmlHeader("Sign Up","Default.Header");
		htmlUserPage("Sign Up","SignUpStep1.Body");
		htmlFooter("Default.Footer");
	}

}//void htmlSignUpStep1(void)


void htmlSignUpDone(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPerm=0;
	unsigned uAuthorize=0;

	sprintf(gcQuery,"SELECT uAuthorize,uPerm FROM tAuthorize"
			" WHERE cLabel='%s' AND cIpMask='%s' AND uPerm=0 LIMIT 1",TextAreaSave(gcLogin),TextAreaSave(gcEmailCode));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql) || strlen(gcEmailCode)!=16)
	{
		gcMessage="Unexpected error try again later";
		htmlHeader("Sign Up","Default.Header");
		htmlUserPage("Sign Up","SignUpStep1.Body");
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uAuthorize);
		sscanf(field[1],"%u",&uPerm);
	}
	mysql_free_result(res);

	//check code
	//if valid update tAuthorize.uPerm
	if(!uAuthorize)
	{
		gcMessage="Code is wrong, or user does not exist, or user has already been activated.";
		htmlHeader("Sign Up","Default.Header");
		htmlUserPage("Sign Up","SignUpStep1.Body");
		htmlFooter("Default.Footer");
	}

	sprintf(gcQuery,"UPDATE tAuthorize SET uPerm=7,cIpMask='',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE uAuthorize=%u",uAuthorize);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unknown error try again later";
		htmlHeader("Sign Up","Default.Header");
		htmlUserPage("Sign Up","SignUpStep1.Body");
		htmlFooter("Default.Footer");
	}

	gcMessage="You can login now.";
	htmlHeader("Sign Up","Default.Header");
	htmlUserPage("Sign Up","SignUpDone.Body");
	htmlFooter("Default.Footer");

}//void htmlSignUpDone(void);


void htmlLostPasswordDone(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPerm=0;
	unsigned uAuthorize=0;

	if(!strcmp(gcFunction,"ChangePassword") && 
			gcLogin[0] && gcEmailCode[0] && strlen(gcEmailCode)==16 && gcPasswd[0] && gcPasswd2[0])
	{
		if(strcmp(gcPasswd,gcPasswd2))
		{
			gcMessage="Both passwords must match";
			htmlHeader("Lost Password","Default.Header");
			htmlUserPage("Lost Password","LostPasswordDone.Body");
			htmlFooter("Default.Footer");
		}
		if(strlen(gcPasswd)<8)
		{
			gcMessage="Password must be at least 8 characters long";
			htmlHeader("Lost Password","Default.Header");
			htmlUserPage("Lost Password","LostPasswordDone.Body");
			htmlFooter("Default.Footer");
		}
		if(uNoUpper(gcPasswd) || uNoLower(gcPasswd) || uNoDigit(gcPasswd))
		{
			gcMessage="Passwords must have upper and lower case letters and at least one number";
			htmlHeader("Lost Password","Default.Header");
			htmlUserPage("Lost Password","LostPasswordDone.Body");
			htmlFooter("Default.Footer");//Footer exits
		}

		char cBuffer[100]={""};
		char cSalt[16]={"$1$23abc123$"};//TODO set to random salt;
		(void)srand((long long)time((time_t *)NULL)*getpid());
		to64(&cSalt[3],rand(),8);

		sprintf(cBuffer,"%.99s",gcPasswd);
		EncryptPasswdWithSalt(cBuffer,cSalt);

		sprintf(gcQuery,"UPDATE tAuthorize SET"
			" cClrPasswd='%s',"
			" cPasswd='%s',"
			" cIpMask='',"
			" uModBy=1,"
			" uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE cLabel='%s' AND cIpMask='%s'"
				,gcPasswd
				,cBuffer
				,TextAreaSave(gcLogin),TextAreaSave(gcEmailCode));
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			gcMessage="Unknown error try again later";
			htmlHeader("Lost Password","Default.Header");
			htmlUserPage("Lost Password","LostPasswordDone.Body");
			htmlFooter("Default.Footer");
		}
		gcMessage="Password changed if code was valid.";
		htmlHeader("Lost Password","Default.Header");
		htmlUserPage("Lost Password","Login.Body");
		htmlFooter("Default.Footer");
	}
	else if(!strcmp(gcFunction,"ChangePassword") && gcLogin[0] && gcEmailCode[0])
	{
		gcMessage="";
		htmlHeader("Lost Password","Default.Header");
		htmlUserPage("Lost Password","LostPasswordDone.Body");//Almost Done ;)
		htmlFooter("Default.Footer");
	}

	//
	//check to see if active account if active send mail change link
	//
	sprintf(gcQuery,"SELECT uAuthorize,uPerm FROM tAuthorize"
			" WHERE cLabel='%s' LIMIT 1",TextAreaSave(gcLogin));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unknown error try again later";
		htmlHeader("Lost Password","Default.Header");
		htmlUserPage("Lost Password","LostPassword.Body");
		htmlFooter("Default.Footer");
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uAuthorize);
		sscanf(field[1],"%u",&uPerm);
	}
	mysql_free_result(res);

	if(uAuthorize && uPerm)
	{
		NewCodeAndLinkEmail(uAuthorize,TextAreaSave(gcLogin),"portal.arreglokites.com");
		sleep(3);
		gcMessage="Link sent to your email if it exists.";
		htmlHeader("Lost Password","Default.Header");
		htmlUserPage("Lost Password","LostPassword.Body");
		htmlFooter("Default.Footer");
	}
	else
	{
		gcMessage="Or user does not exist or it is not active yet.";
		htmlHeader("Lost Password","Default.Header");
		htmlUserPage("Lost Password","LostPassword.Body");
		htmlFooter("Default.Footer");
	}

}//void htmlLostPasswordDone(void);


void htmlLostPassword(void)
{
	gcMessage="";
	htmlHeader("Lost Password","Default.Header");
	htmlUserPage("Lost Password","LostPassword.Body");
	htmlFooter("Default.Footer");

}//void htmlLostPassword(void)


void LoadJobOfferData(unsigned uJobOffer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	if(uJobOffer==0) return;
	if(uJobOffer==(-1))
	{
		if(guStatusFilter)
		{
		if(guPermLevel>=10)
			sprintf(gcQuery,"SELECT tJobOffer.uJobOffer,tJobOffer.cLabel FROM tJobOffer,tClient"
				" WHERE tClient.uClient=tJobOffer.uOwner"
				" AND tJobOffer.uStatus=%u"
				" AND (tJobOffer.uOwner=%u OR tClient.uOwner=%u)"
				" ORDER BY tJobOffer.uModDate DESC, tJobOffer.uCreatedDate DESC LIMIT 99",
					guStatusFilter,guLoginClient,guOrg);
		else
			sprintf(gcQuery,"SELECT uJobOffer,cLabel FROM tJobOffer"
				" WHERE tJobOffer.uStatus=%u"
				" AND uOwner=%u ORDER BY uModDate DESC, uCreatedDate DESC LIMIT 99",
					guStatusFilter,guLoginClient);
		}
		else
		{
		if(guPermLevel>=10)
			//Work In Progress
			sprintf(gcQuery,"SELECT tJobOffer.uJobOffer,tJobOffer.cLabel FROM tJobOffer,tClient"
				" WHERE tClient.uClient=tJobOffer.uOwner"
				" AND tJobOffer.uStatus!=15"//Archivado
				" AND tJobOffer.uStatus!=14"//Cancelado
				" AND tJobOffer.uStatus!=11"//Entregado
				" AND tJobOffer.uStatus!=13"//Trabajo Postergado
				" AND tJobOffer.uStatus!=10"//Listo Para Entrega
				" AND tJobOffer.uStatus!=1"//Trabajo Nuevo
				" AND tJobOffer.uStatus!=2"//Trabajo Aceptado
				" AND (tJobOffer.uOwner=%u OR tClient.uOwner=%u)"
				" ORDER BY tJobOffer.uModDate DESC, tJobOffer.uCreatedDate DESC LIMIT 99",
					guLoginClient,guOrg);
		else
			sprintf(gcQuery,"SELECT uJobOffer,cLabel FROM tJobOffer"
				" WHERE tJobOffer.uStatus!=15"//Arch
				" AND tJobOffer.uStatus!=14"//Cancelado
				" AND tJobOffer.uStatus!=11"//Entregado
				" AND uOwner=%u ORDER BY uModDate DESC, uCreatedDate DESC LIMIT 99",
					guLoginClient);
		}
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uJobOffer);
			guJobOffer=uJobOffer;
			printf("Set-Cookie: {{cProject}}JobOffer=%u; secure; httponly; samesite=strict;\n",guJobOffer);
		}
	}

	if(guPermLevel>=10)
		sprintf(gcQuery,"SELECT tJobOffer.cDescription,tJobOffer.uBrand,tJobOffer.cModel,tJobOffer.uSize,tJobOffer.uYear,"
				"tJobOffer.uMaxBid,"
				"DATE_FORMAT(tJobOffer.dStart,'%%Y-%%m-%%d'),DATE_FORMAT(tJobOffer.dEnd,'%%Y-%%m-%%d'),tJobOffer.cColors"
				",tJobOffer.cLink1,tJobOffer.cLink1Title,tJobOffer.cLink1Desc"
				",tJobOffer.cLink2,tJobOffer.cLink2Title,tJobOffer.cLink2Desc"
				",tJobOffer.cLink3,tJobOffer.cLink3Title,tJobOffer.cLink3Desc,tClient.cLabel,tStatus.cLabel,tJobOffer.uStatus"
				" FROM tJobOffer,tClient,tStatus WHERE tJobOffer.uOwner=tClient.uClient"
				" AND tJobOffer.uJobOffer=%u"
				" AND tJobOffer.uStatus=tStatus.uStatus"
				" AND (tJobOffer.uOwner=%u OR tClient.uOwner=%u)",
					uJobOffer,guLoginClient,guOrg);
	else
		sprintf(gcQuery,"SELECT tJobOffer.cDescription,tJobOffer.uBrand,tJobOffer.cModel,tJobOffer.uSize,tJobOffer.uYear,"
				"tJobOffer.uMaxBid,"
				"DATE_FORMAT(tJobOffer.dStart,'%%Y-%%m-%%d'),DATE_FORMAT(tJobOffer.dEnd,'%%Y-%%m-%%d'),tJobOffer.cColors"
				",tJobOffer.cLink1,tJobOffer.cLink1Title,tJobOffer.cLink1Desc"
				",tJobOffer.cLink2,tJobOffer.cLink2Title,tJobOffer.cLink2Desc"
				",tJobOffer.cLink3,tJobOffer.cLink3Title,tJobOffer.cLink3Desc,tClient.cLabel,tStatus.cLabel,tJobOffer.uStatus"
				" FROM tJobOffer,tClient,tStatus WHERE tJobOffer.uOwner=tClient.uClient"
				" AND tJobOffer.uJobOffer=%u"
				" AND tJobOffer.uStatus=tStatus.uStatus"
				" AND tJobOffer.uOwner=%u",
					uJobOffer,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		cDescription=field[0];
		sscanf(field[1],"%u",&uBrand);
		sprintf(cModel,"%s",field[2]);
		sscanf(field[3],"%u",&uSize);
		sscanf(field[4],"%u",&uYear);
		sscanf(field[5],"%u",&uMaxBid);
		sprintf(dStart,"%s",field[6]);
		sprintf(dEnd,"%s",field[7]);
		sprintf(cColors,"%s",field[8]);

		sprintf(cLink1,"%s",field[9]);
		sprintf(cLink1Title,"%s",field[10]);
                cLink1Desc=field[11];

		sprintf(cLink2,"%s",field[12]);
		sprintf(cLink2Title,"%s",field[13]);
                cLink2Desc=field[14];

		sprintf(cLink3,"%s",field[15]);
		sprintf(cLink3Title,"%s",field[16]);
                cLink3Desc=field[17];

		sprintf(cJobOwner,"%.32s",field[18]);

		sprintf(cuStatus,"%.32s",field[19]);
		sscanf(field[20],"%u",&uStatus);

		guValidJobLoaded=1;
	}
}//void LoadJobOfferData(uJobOffer)


void htmlJobOffer(void)
{
	LoadJobOfferData(guJobOffer);
	htmlHeader("JobOffer","Default.Header");
	htmlUserPage("JobOffer","JobOffer.Body");
	htmlFooter("Default.Footer");

}//void htmlJobOffer(void)


void htmlAdmin(void)
{
	htmlHeader("Admin","Default.Header");
	htmlUserPage("Admin","Admin.Body");
	htmlFooter("Default.Footer");

}//void htmlAdmin(void)


void htmlCalendar(void)
{
	htmlHeader("Calendar","Default.Header");
	htmlUserPage("Calendar","Calendar.Body");
	htmlFooter("Default.Footer");

}//void htmlCalendar(void)


void htmlUser(void)
{
	htmlHeader("User","Default.Header");
	htmlUserPage("User","User.Body");
	htmlFooter("Default.Footer");

}//void htmlUser(void)


void htmlUserPage(char *cTitle, char *cTemplateName)
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
			template.cpValue[1]="/{{cProject}}App/";
			
			template.cpName[2]="gcLogin";
			if(gcUser[0])
				template.cpValue[2]=gcUser;
			else
				//Lost Login or Signup Login
				template.cpValue[2]=gcLogin;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);//Safe?

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcMessage";
			template.cpValue[7]=gcMessage;

			template.cpName[8]="gcBrand";
			template.cpValue[8]=gcBrand;

			template.cpName[9]="gcCopyright";
			template.cpValue[9]=INTERFACE_COPYRIGHT;

			template.cpName[10]="gcFiveIn";
			template.cpValue[10]=gcFiveIn;

			template.cpName[11]="gcEmailCode";
			template.cpValue[11]=gcEmailCode;

			template.cpName[12]="cModel";
			template.cpValue[12]=cModel;

			template.cpName[13]="cColors";
			template.cpValue[13]=cColors;

			template.cpName[14]="uYear";
			char cuYear[16]={""};
			if(uYear)
				sprintf(cuYear,"%u",uYear);
			template.cpValue[14]=cuYear;

			template.cpName[15]="uSize";
			char cuSize[16]={""};
			if(uSize)
				sprintf(cuSize,"%u",uSize);
			template.cpValue[15]=cuSize;

			template.cpName[16]="uMaxBid";
			char cuMaxBid[16]={""};
			if(uMaxBid)
				sprintf(cuMaxBid,"%u",uMaxBid);
			template.cpValue[16]=cuMaxBid;

			template.cpName[17]="dStart";
			template.cpValue[17]=dStart;

			template.cpName[18]="dEnd";
			template.cpValue[18]=dEnd;

			template.cpName[19]="cDescription";
			template.cpValue[19]=cDescription;

			template.cpName[20]="gcImagesShow";
			template.cpValue[20]=gcImagesShow;

char cImage1Src[512]={""};
char cImage2Src[512]={""};
char cImage3Src[512]={""};

			template.cpName[21]="cImage1";
			if(cLink1[0])
				sprintf(cImage1Src,"<a href=/images/%s title='%s' >"
					"<img class='img-fluid img-thumbnail width=25%%' src='/images/%s'></a>"
					"<p>Img1. %s <a title='eliminar imagen' href='?gcPage=JobOffer&gcFunction=DelImage1'>[-]</a> </p>",cLink1,cLink1Title,cLink1,cLink1Desc);
			template.cpValue[21]=cImage1Src;

			template.cpName[22]="cImage2";
			if(cLink2[0])
				sprintf(cImage2Src,"<a href=/images/%s title='%s' >"
					"<img class='img-fluid img-thumbnail width=25%%' src='/images/%s'></a>"
					"<p>Img2. %s <a title='eliminar imagen' href='?gcPage=JobOffer&gcFunction=DelImage2'>[-]</a> </p>",cLink2,cLink2Title,cLink2,cLink2Desc);
			template.cpValue[22]=cImage2Src;

			template.cpName[23]="cImage3";
			if(cLink3[0])
				sprintf(cImage3Src,"<a href=/images/%s title='%s' >"
					"<img class='img-fluid img-thumbnail width=25%%' src='/images/%s'></a>"
					"<p>Img3. %s <a title='eliminar imagen' href='?gcPage=JobOffer&gcFunction=DelImage3'>[-]</a> </p>",cLink3,cLink3Title,cLink3,cLink3Desc);
			template.cpValue[23]=cImage3Src;

			char *cDisabled="disabled";
			if(guValidJobLoaded)
				cDisabled="";
			template.cpName[24]="cDisabled";
			template.cpValue[24]=cDisabled;

			template.cpName[25]="gcCPShow";
			template.cpValue[25]=gcCPShow;

			template.cpName[26]="cJobOwner";
			template.cpValue[26]=cJobOwner;

			template.cpName[27]="cuStatus";
			template.cpValue[27]=cuStatus;

			template.cpName[28]="gcInvoiceShow";
			template.cpValue[28]=gcInvoiceShow;

			template.cpName[29]="gcSummaryShow";
			template.cpValue[29]=gcSummaryShow;

			template.cpName[30]="";

//debug only
//printf("Content-type: text/html\n\n");
//printf("d6 %s",gcUser);
//exit(0);
	
			printf("\n<!-- Start htmlUserPage(%s) -->\n",cTemplateName); 
			Template(field[0],&template,stdout);
			printf("\n<!-- End htmlUserPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlUserPage()


void funcMOTD(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcMOTD(fp) Start -->\n");

	sprintf(gcQuery,"SELECT cComment FROM tConfiguration"
			" WHERE cLabel='cOrg_NewMOTD' LIMIT 1");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		fprintf(fp,"%s",field[0]);
	mysql_free_result(res);

	fprintf(fp,"<!-- funcMOTD(fp) End -->\n");

}//void funcMOTD(FILE *fp)


void funcOperationHistory(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcOperationHistory(fp) Start -->\n");

	sprintf(gcQuery,"SELECT cLabel,FROM_UNIXTIME(uCreatedDate),cHost,cServer"
			" FROM tLog"
			" WHERE uLogType!=8"
			" AND (uCreatedBy=%u OR uLoginClient=%u OR uOwner=%u)"
			" ORDER BY uCreatedDate DESC LIMIT 10",
				guLoginClient,guLoginClient,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"mysql error<br>");
		return;
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"%s %s %s %s<br>",field[1],field[0],field[2],field[3]);
	mysql_free_result(res);

	fprintf(fp,"<!-- funcOperationHistory(fp) End -->\n");

}//void funcOperationHistory(FILE *fp)


void funcLoginHistory(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcLoginHistory(fp) Start -->\n");

	sprintf(gcQuery,"SELECT cLabel,FROM_UNIXTIME(uCreatedDate),cHost,cServer"
			" FROM tLog"
			" WHERE uLoginClient=%u"
			" AND uLogType=8"
			" ORDER BY uCreatedDate DESC LIMIT 10",
				guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"mysql error<br>");
		return;
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"%s %s %s %s<br>",field[1],field[0],field[2],field[3]);
	mysql_free_result(res);

	fprintf(fp,"<!-- funcLoginHistory(fp) End -->\n");

}//void funcLoginHistory(FILE *fp)


unsigned uValidPasswd(char *cPasswd,unsigned guLoginClient)
{
	char cSalt[16]={""};
	char cPassword[100]={""};
	char gcPasswd[100]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cPasswd FROM tAuthorize WHERE uCertClient=%u",guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return(0);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cPassword,"%.99s",field[0]);
	mysql_free_result(res);

	sprintf(gcPasswd,"%.99s",cPasswd);
	if(cPassword[0])
	{
		//MD5 vs DES salt determination
		if(cPassword[0]=='$' && cPassword[2]=='$')
			sprintf(cSalt,"%.12s",cPassword);
		else
			sprintf(cSalt,"%.2s",cPassword);
		EncryptPasswdWithSalt(gcPasswd,cSalt);
		if(!strcmp(gcPasswd,cPassword))
			return(1);
		else
			return(0);
	}
	return(0);
}//unsigned uValidPasswd(char *cPasswd)


void htmlOperationsInfo(void)
{

	htmlHeader("User","Default.Header");

        MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT cLabel,FROM_UNIXTIME(uCreatedDate),cHost,cServer"
			" FROM tLog WHERE uLogType!=8 AND"
			" (uCreatedBy=%u OR uLoginClient=%u OR uOwner=%u)"
			" ORDER BY uCreatedDate DESC LIMIT 20",
				guLoginClient,guLoginClient,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("mysql error<br>");
		return;
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		printf("%s %s %s %s<br>",field[1],field[0],field[2],field[3]);
	mysql_free_result(res);


	exit(0);

}//void htmlOperationsInfo(void)


void htmlLoginInfo(void)
{

	htmlHeader("User","Default.Header");

        MYSQL_RES *res;
	MYSQL_ROW field;
	sprintf(gcQuery,"SELECT cLabel,FROM_UNIXTIME(uCreatedDate),cHost,cServer"
			" FROM tLog WHERE uLoginClient=%u"
			" AND uLogType=8 ORDER BY uCreatedDate DESC LIMIT 20",guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("mysql error<br>");
		return;
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		printf("%s %s %s %s<br>",field[1],field[0],field[2],field[3]);
	mysql_free_result(res);


	exit(0);

}//void htmlLoginInfo(void)


void ItemJob(int iAdd)
{
	if(guPermLevel<10)
		htmlJobOffer();
	if(iAdd>1 || iAdd< -1)
		htmlJobOffer();
	if(!guItemJob)
	{
		gcMessage="No guItemJob!";
		htmlJobOffer();
	}
	if(iAdd>0)
		sprintf(gcQuery,"UPDATE tItemJob SET uQuantity=uQuantity+%d,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE uItemJob=%u",
			iAdd,guLoginClient,
				guItemJob);
	else
		sprintf(gcQuery,"UPDATE tItemJob SET uQuantity=uQuantity+%d,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE uQuantity>0 AND uItemJob=%u",
			iAdd,guLoginClient,
				guItemJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		gcMessage="ItemJob error!";
	else
	{
		gcInvoiceShow="show";
		gcSummaryShow="";
	}

}//void ItemJob(unsigned uAdd)


void DeleteItemJob(void)
{
	if(guPermLevel<10)
		htmlJobOffer();
	if(!guItemJob)
	{
		gcMessage="No guItemJob!";
		htmlJobOffer();
	}
		sprintf(gcQuery,"DELETE FROM tItemJob WHERE uItemJob=%u",guItemJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		gcMessage="DeleteItemJob error!";
	else
	{
		gcInvoiceShow="show";
		gcSummaryShow="";
	}

}//void DeleteItemJob(void)
