/*
FILE 
	ticket.c
	svn ID removed
AUTHOR
	(C) 2007-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	ispHelp  Interface
	program file.
*/


#include "interface.h"

typedef struct
{
	unsigned uTicket;
	unsigned uOwner;
	unsigned uCreatedBy;
	time_t uCreatedDate;
	unsigned uModBy;
	time_t uModDate;
	unsigned uTicketStatus;
	unsigned uTicketOwner;
	unsigned uScheduleDate;
	char *cText;
	char cKeywords[256];
	char cSubject[256];

} structTicket;

static unsigned uTicket=0;
static unsigned uTicketComment=0;

static unsigned uCreatedBy=0; 
static char cCreatedDate[64]={""};

static unsigned uTicketStatus=0;
static char *uTicketStatusStyle="type_fields_off";

static unsigned uTicketOwner=0;
static char *uTicketOwnerStyle="type_fields_off";

static time_t uScheduleDate=0;
static char *cScheduleDateStyle="type_fields_off";

static char *cText="";
static char *cTextStyle="type_textarea_off";

static char cKeywords[256]={""};
static char *cKeywordsStyle="type_fields_off";

static char cSubject[256]={""};
static char *cSubjectStyle="type_fields_off";

static char *cTicketComment="";
static char *cCommentConfirm="";

static char cSearch[32]={""};

time_t ToUnixTime(char *cMySQLDate);
char *cFromUnixTime(time_t luDate);
void htmlTicketPage(char *cTitle, char *cTemplateName);
void LoadTicket(void);
unsigned ValidateTicketInput(unsigned uMode);
void NewTicket(void);
void ModTicket(void);
void SetTicketFieldsOn(void);
void SubmitComment(void);
void LoadRecordIntoStruct(structTicket *Target);
void fileDirectTemplate(FILE *fp,char *cTemplateName);
unsigned uTicketChanged(void);

void EmailTicketChanges(void);
void EmailTicketComment(void);
void EmailNewTicket(void);
void fpEmailTicketHeader(FILE *fp);

void ProcessTicketVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uTicket"))
			sscanf(entries[i].val,"%u",&uTicket);
		else if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"cTicketComment"))
			cTicketComment=entries[i].val;
		else if(!strcmp(entries[i].name,"cSubject"))
			sprintf(cSubject,"%.255s",entries[i].val);
		else if(!strcmp(entries[i].name,"cText"))
			cText=entries[i].val;
		else if(!strcmp(entries[i].name,"uTicketOwner"))
			sscanf(entries[i].val,"%u",&uTicketOwner);
		else if(!strcmp(entries[i].name,"uTicketStatus"))
			sscanf(entries[i].val,"%u",&uTicketStatus);
		else if(!strcmp(entries[i].name,"cScheduleDate"))
		{
			//Convert to unix time and store at uScheduleDate
			unsigned uYear,uMonth,uDay;
			char cDate[100]={""};
			sscanf(entries[i].val,"%u-%u-%u",&uYear,&uMonth,&uDay);
			sprintf(cDate,"%u-%u-%u 0:00:00",uYear,uMonth,uDay);
			uScheduleDate=ToUnixTime(cDate);
		}
		else if(!strcmp(entries[i].name,"cKeywords"))
			sprintf(cKeywords,"%.255s",entries[i].val);
		else if(!strcmp(entries[i].name,"uCreatedBy"))
			sscanf(entries[i].val,"%u",&uCreatedBy);
		else if(!strcmp(entries[i].name,"cCreatedDate"))
			sprintf(cCreatedDate,"%.63s",entries[i].val);
	}

}//void ProcessUserVars(pentry entries[], int x)


time_t ToUnixTime(char *cMySQLDate)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	time_t tRet;

	sprintf(gcQuery,"SELECT UNIX_TIMESTAMP('%s')",cMySQLDate);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%lu",&tRet);
	mysql_free_result(res);

	return(tRet);
}//time_t ToUnixTime(char *cMySQLDate)


char *cFromUnixTime(time_t luDate)
{
	static char cRet[16]={""};
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT FROM_UNIXTIME('%lu')",luDate);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cRet,"%.10s",field[0]);
	mysql_free_result(res);

	return(cRet);
}//char *cFromUnixTime(time_t luDate)


void TicketGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uTicket"))
			sscanf(gentries[i].val,"%u",&uTicket);
	}
	if(uTicket) LoadTicket();

	htmlTicket();

}//void TicketGetHook(entry gentries[],int x)


void TicketCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Ticket"))
	{
		ProcessTicketVars(entries,x);
		if(!strcmp(gcFunction,"Submit Comment"))
			cCommentConfirm="Confirm ";
		else if(!strcmp(gcFunction,"Confirm Submit Comment"))
		{
			SubmitComment();
			EmailTicketComment();
			cTicketComment="";
		}
		else if(!strcmp(gcFunction,"New"))
		{
			SetTicketFieldsOn();
			sprintf(gcNewStep,"Confirm ");
			gcInputStatus[0]=0;
		}
		else if(!strcmp(gcFunction,"Confirm New"))
		{
			if(ValidateTicketInput(0))
			{
				SetTicketFieldsOn();
				sprintf(gcNewStep,"Confirm ");
				gcInputStatus[0]=0;
			}
			else
			{
				NewTicket();
				EmailNewTicket();
				gcMessage="Ticket was created and notification email was sent";
			}
		}

		htmlTicket();
	}

}//void TicketCommands(pentry entries[], int x)


void htmlTicket(void)
{
	htmlHeader("unxsISP CRM","Header");
	htmlTicketPage("","Ticket.Body");
	htmlFooter("Footer");

}//void htmlTicket(void)


void htmlTicketPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;
		
		TemplateSelect(cTemplateName,guTemplateSet);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			char cuTicket[16]={""};
			char cCreatedBy[100]={""};
			char cuCreatedBy[16]={""};
			char cScheduleDate[32]={""};

			struct t_template template;
			sprintf(cCreatedBy,"%.99s",ForeignKey("tClient","cLabel",uCreatedBy));
			sprintf(cuCreatedBy,"%u",uCreatedBy);

			sprintf(cuTicket,"%u",uTicket);
			if(uScheduleDate)
				sprintf(cScheduleDate,"%s",cFromUnixTime(uScheduleDate));

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="ispHelp.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcLogin;

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

			template.cpName[8]="uTicket";
			template.cpValue[8]=cuTicket;

			template.cpName[9]="cSubject";
			template.cpValue[9]=cSubject;

			template.cpName[10]="cSubjectStyle";
			template.cpValue[10]=cSubjectStyle;
			
			template.cpName[11]="cTextStyle";
			template.cpValue[11]=cTextStyle;
			
			template.cpName[12]="cScheduleDateStyle";
			template.cpValue[12]=cScheduleDateStyle;

			template.cpName[13]="cText";
			template.cpValue[13]=cText;

			template.cpName[14]="cCreatedDate";
			template.cpValue[14]=cCreatedDate;

			template.cpName[15]="cCommentStatus";
			if(uTicket)
				template.cpValue[15]="";
			else
				template.cpValue[15]="disabled";

			template.cpName[16]="cTicketCommentStyle";
			if(uTicket)
				template.cpValue[16]="type_textarea";
			else
				template.cpValue[16]="type_textarea_off";

			template.cpName[17]="cTicketComment";
			template.cpValue[17]=cTicketComment;

			template.cpName[18]="cCommentConfirm";
			template.cpValue[18]=cCommentConfirm;

			template.cpName[19]="gcInputStatus";
			template.cpValue[19]=gcInputStatus;

			template.cpName[20]="";

			printf("\n<!-- Start htmlTicketPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlTicketPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlTicketPage()


void LoadTicket(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cSubject,cText,uTicketOwner,uTicketStatus,uScheduleDate,"
			"cKeywords,uCreatedBy,FROM_UNIXTIME(uCreatedDate) FROM tTicket WHERE uTicket=%u AND uOwner=%u",
			uTicket,guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
	{
		sprintf(cSubject,"%.255s",field[0]);
		cText=field[1];
		sscanf(field[2],"%u",&uTicketOwner);
		sscanf(field[3],"%u",&uTicketStatus);
		sscanf(field[4],"%lu",&uScheduleDate);
		sprintf(cKeywords,"%.255s",field[5]);
		sscanf(field[6],"%u",&uCreatedBy);
		sprintf(cCreatedDate,"%.63s",field[7]);
	}
	else
		gcMessage="<blink>Error: </blink>Could not load ticket record";

}//void LoadTicket(void)


void SubmitComment()
{
	sprintf(gcQuery,"INSERT INTO tTicketComment SET uTicket=%u,cComment='%s',uOwner=%u,"
			"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uTicket
			,TextAreaSave(cTicketComment)
			,guOrg
			,guLoginClient
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	

}//void SubmitComment()


void funcTicketNavList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uDisplayed=0;
	unsigned uFound=0;
	static char cTopMessage[100]={""};

#define SEARCH_FIELDS "uTicket,cSubject,FROM_UNIXTIME(uCreatedDate)"

	if(cSearch[0])
	{
		sscanf(cSearch,"%u",&uTicket);
		if(uTicket)
			sprintf(gcQuery,"SELECT "SEARCH_FIELDS" FROM tTicket "
					"WHERE uCreatedBy=%u AND uOwner=%u AND uTicket=%u "
					"ORDER BY uCreatedDate DESC",
					guLoginClient
					,guOrg
					,uTicket);
		else
			sprintf(gcQuery,"SELECT "SEARCH_FIELDS" FROM tTicket "
					"WHERE uCreatedBy=%u AND uOwner=%u AND "
					"(cSubject LIKE '%%%s%%' OR cText LIKE '%%%s%%' "
					"ORDER BY uCreatedDate DESC",
					guLoginClient
					,guOrg
					,cSearch
					,cSearch
					);
		
	}
	else
		sprintf(gcQuery,"SELECT "SEARCH_FIELDS" FROM tTicket "
				"WHERE uCreatedBy=%u AND uOwner=%u "
				"ORDER BY uCreatedDate DESC LIMIT 20",
				guLoginClient
				,guOrg
				);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	
	if(!(uFound=mysql_num_rows(res)))
	{
		fprintf(fp,"No records found.<br>\n");
		mysql_free_result(res);
		return;
	}

	sprintf(cTopMessage,"%u record(s) found.",uFound);
	gcMessage=cTopMessage;

	if(uFound==1)
	{
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uTicket);
			LoadTicket();
			fprintf(fp,"<a href=ispHelp.cgi?gcPage=Ticket&uTicket=%s>Ticket #%s</a> %s (%s)<br>\n",
				field[0]
				,field[0]
				,field[1]
				,field[2]);
			mysql_free_result(res);
			return;
		}
	}

	while((field=mysql_fetch_row(res)))
	{
		if(uDisplayed==20)
		{
			fprintf(fp,"Only the first 20 records found. If the invoice you are looking for is not in the list above please "
			"further refine your search.<br>\n");
			break;
		}
		fprintf(fp,"<a href=ispHelp.cgi?gcPage=Ticket&uTicket=%s>Ticket #%s</a> %s (%s)<br>\n",
			field[0]
			,field[0]
			,field[1]
			,field[2]);
		uDisplayed++;
	}

	mysql_free_result(res);

}//void funcTicketNavList(FILE *fp)


void funcTicketNavBar(FILE *fp)
{
	fprintf(fp,"<input type=submit title='Customer modification with a two step procedure'"
			" class=largeButton name=gcFunction value='%sNew' />",gcNewStep);

}//void funcTicketNavBar(FILE *fp)


void funcTicketComments(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	if(!uTicket) return;

	sprintf(gcQuery,"SELECT uTicketComment,cComment,FROM_UNIXTIME(uCreatedDate),uCreatedBy "
			"FROM tTicketComment WHERE uTicket=%u ORDER BY uCreatedDate DESC",uTicket);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<tr>"
			"<td><a class=darkLink href=\"#\" onClick=\"open_popup('ispHelp.cgi?gcPage=TicketComment&uTicketComment=%s')\">%s</a>"
			"</td><td>%s...</td><td>%s</td>"
			"</tr>\n"
			,field[0]
			,field[2]
			,cShortenText(field[1],3)
			,ForeignKey("tClient","cLabel",strtoul(field[3],NULL,10)));
	}
	mysql_free_result(res);

}


unsigned ValidateTicketInput(unsigned uMode)
{
	if(!cSubject[0])
	{
		SetTicketFieldsOn();
		cSubjectStyle="type_fields_req";
		gcMessage="<blink>Error: </blink> Must specify ticket subject";
		return(1);
	}
	if(!cText[0])
	{
		SetTicketFieldsOn();
		cTextStyle="type_fields_req";
		gcMessage="<blink>Error: </blink> Must specify ticket text";
		return(1);
	}

	return(0);

}//unsigned ValidateTicketInput(unsigned uMode)


void NewTicket(void)
{
	//uTicketStatus=5 is new
	sprintf(gcQuery,"INSERT INTO tTicket SET uOwner=%u,uCreatedBy=%u,"
			"uCreatedDate=UNIX_TIMESTAMP(NOW()),uTicketStatus=5,"
			"uTicketOwner=%u,uScheduleDate=%lu,cText='%s',"
			"cKeywords='%s',cSubject='%s'",
			guOrg
			,guLoginClient
			,uTicketOwner
			,uScheduleDate
			,TextAreaSave(cText)
			,TextAreaSave(cKeywords)
			,TextAreaSave(cSubject)
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	uTicket=mysql_insert_id(&gMysql);
	LoadTicket();

}//void NewTicket(void)


void SetTicketFieldsOn(void)
{
	uTicketStatusStyle="type_fields";
	uTicketOwnerStyle="type_fields";
	cScheduleDateStyle="type_fields";
	cTextStyle="type_textarea";
	cKeywordsStyle="type_fields";
	cSubjectStyle="type_fields";

}//void SetTicketFieldsOn(void)


void LoadRecordIntoStruct(structTicket *Target)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate,uTicketStatus,uTicketOwner,uScheduleDate,"
			"cText,cKeywords,cSubject FROM tTicket WHERE uTicket=%u AND uOwner=%u",
			uTicket,guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
	{
		Target->uTicket=uTicket;
		sscanf(field[0],"%u",&Target->uOwner);
		sscanf(field[1],"%u",&Target->uCreatedBy);
		sscanf(field[2],"%lu",&Target->uCreatedDate);
		sscanf(field[3],"%u",&Target->uModBy);
		sscanf(field[4],"%lu",&Target->uModDate);
		sscanf(field[5],"%u",&Target->uTicketStatus);
		sscanf(field[6],"%u",&Target->uTicketOwner);
		sscanf(field[7],"%u",&Target->uScheduleDate);
		Target->cText=field[8];
		Target->cText[strlen(field[8])+1]=0;
		sprintf(Target->cKeywords,"%.255s",field[9]);
		sprintf(Target->cSubject,"%.255s",field[10]);
	}
	else
		htmlPlainTextError("LoadRecordIntoStruct() failed");

}//void LoadRecordIntoStruct(char *structTicket Target)


void fileDirectTemplate(FILE *fp,char *cTemplateName)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	if(!fp) return;

	TemplateSelect(cTemplateName,guTemplateSet);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		fprintf(fp,"%s",field[0]);
	else
		fprintf(fp,"Template %s not found\n",cTemplateName);
	mysql_free_result(res);

}//void fileDirectTemplate(FILE *fp,char *cTemplateName)


static char *cComment="";
static char cCommentCreatedBy[100]={""};
static char cCommentDate[33]={""};

void LoadTicketComment(void);
void htmlTicketComment(void);
void htmlTicketCommentPage(char *cTitle, char *cTemplateName);

void TicketCommentGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uTicketComment"))
			sscanf(gentries[i].val,"%u",&uTicketComment);
	}
	
	if(uTicketComment)
	{
		LoadTicketComment();
		htmlTicketComment();
	}
}//void TicketCommentGetHook(entry gentries[],int x)


void LoadTicketComment(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cComment,uCreatedBy,FROM_UNIXTIME(uCreatedDate) "
			"FROM tTicketComment WHERE uTicketComment=%u",uTicketComment);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
	{
		cComment=field[0];
		sprintf(cCommentCreatedBy,"%.99s",ForeignKey("tClient","cLabel",strtoul(field[1],NULL,10)));
		sprintf(cCommentDate,"%.32s",field[2]);
	}

}//void LoadTicketComment(void)


void htmlTicketComment()
{
	htmlHeader("unxsISP CRM","Header");
	htmlTicketCommentPage("","TicketComment.Body");
	htmlFooter("Footer");

}//void htmlTicket(void)


void htmlTicketCommentPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect(cTemplateName,guTemplateSet);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;

			template.cpName[0]="cComment";
			template.cpValue[0]=cComment;

			template.cpName[1]="cCommentCreatedBy";
			template.cpValue[1]=cCommentCreatedBy;

			template.cpName[2]="cCommentDate";
			template.cpValue[2]=cCommentDate;

			template.cpName[3]="";
			
			printf("\n<!-- Start htmlTicketCommentPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlTicketCommentPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlTicketComment()


void EmailTicketComment(void)
{
	FILE *fp;

	//debug only
	//if((fp=fopen("/tmp/eMailInvoice","w")))
	if((fp=popen("/usr/lib/sendmail -t > /dev/null","w")))
	{
		fpEmailTicketHeader(fp);

		fprintf(fp,"New comment from %s:\n",ForeignKey("tClient","cLabel",guLoginClient));
		fprintf(fp,"%s\n",TextAreaSave(cTicketComment));
		
		//fclose(fp);
		pclose(fp);
	}	

}//void EmailTicketComment(void)



void EmailNewTicket(void)
{
	FILE *fp;

	//debug only
	//if((fp=fopen("/tmp/eMailInvoice","w")))
	if((fp=popen("/usr/lib/sendmail -t > /dev/null","w")))
	{
		fpEmailTicketHeader(fp);
		
		fprintf(fp,"%s\n",TextAreaSave(cText));

		//fclose(fp);
		pclose(fp);
	}

}//void EmailNewTicket(void)


void fpEmailTicketHeader(FILE *fp)
{
	struct t_template template;
	structTicket RecordData;
	char cFrom[256]={"root"};
	char cEmailSubject[256]={""};
	char cEmail[100]={""};
	char cReporterEmail[256]={""};
	char cuTicket[16]={""};

	cSubject[255]=0;
	LoadRecordIntoStruct(&RecordData);

	GetConfiguration("cReportTicketEmail",cEmail);
	GetConfiguration("cReportTicketFrom",cFrom);

	sprintf(cEmailSubject,"#%u %s",uTicket,RecordData.cSubject);

	sprintf(cReporterEmail,"%.255s",ForeignKey("tClient","cEmail",RecordData.uCreatedBy));

	fprintf(fp,"To: %s\n",cEmail);
	fprintf(fp,"Cc: %s\n",cReporterEmail);
	fprintf(fp,"From: %s\n",cFrom);
	fprintf(fp, "Reply-to: %s\n",cFrom);
	fprintf(fp,"Subject: %s\n",cEmailSubject);
	fprintf(fp,"MIME-Version: 1.0\n");
	fprintf(fp,"Content-type: text/plain\n\n");
	
	sprintf(cuTicket,"%u",uTicket);
	template.cpName[0]="uTicket";
	template.cpValue[0]=cuTicket;

	template.cpName[1]="cSubject";
	template.cpValue[1]=cEmailSubject;

	char cCreatedBy[100]={""};
	template.cpName[2]="cCreatedBy";
	sprintf(cCreatedBy,"%.99s",ForeignKey("tClient","cLabel",RecordData.uCreatedBy));
	template.cpValue[2]=cCreatedBy;

	char cAssignedTo[100]={""};
	template.cpName[3]="cAssignedTo";
	sprintf(cAssignedTo,"%.99s",ForeignKey("tClient","cLabel",RecordData.uTicketOwner));
	template.cpValue[3]=cAssignedTo;

	char cStatus[33]={""};
	template.cpName[4]="cStatus";
	sprintf(cStatus,"%.32s",ForeignKey("tTicketStatus","cLabel",RecordData.uTicketStatus));
	template.cpValue[4]=cStatus;

	template.cpName[5]="cKeywords";
	template.cpValue[5]=cKeywords;

	template.cpName[6]="cScheduleDate";
	if(uScheduleDate)
		template.cpValue[6]=cFromUnixTime(RecordData.uScheduleDate);
	else
		template.cpValue[6]="---";

	template.cpName[7]="cCreatedDate";
	template.cpValue[7]=cCreatedDate;

	template.cpName[8]="";

	fpTemplate(fp,"TicketChangeMailTop",&template);

}//void fpEmailTicketHeader(FILE *fp)
