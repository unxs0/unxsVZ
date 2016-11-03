/*
FILE 
	order.c
	svn ID removed
AUTHOR
	(C) 2007-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	ispCRM  Interface
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
static unsigned uPage=1;

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
		else if(!strcmp(gentries[i].name,"uPage"))
			sscanf(gentries[i].val,"%u",&uPage);
		else if(!strcmp(gentries[i].name,"cSearch"))
			sprintf(cSearch,"%.99s",gentries[i].val);
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
		else if(!strcmp(gcFunction,"Modify"))
		{
			SetTicketFieldsOn();
			sprintf(gcModStep,"Confirm ");
			gcInputStatus[0]=0;
		}
		else if(!strcmp(gcFunction,"Confirm Modify"))
		{
			if(ValidateTicketInput(1))
			{
				SetTicketFieldsOn();
				sprintf(gcModStep,"Confirm ");
				gcInputStatus[0]=0;
			}
			else
			{
				if(uTicketChanged())
				{
					EmailTicketChanges();
					ModTicket();
					gcMessage="Ticket was modified and changes notification email was sent";
				}
				else
				{
					SetTicketFieldsOn();
					sprintf(gcModStep,"Confirm ");
					gcInputStatus[0]=0;
					gcMessage="<blink>Error: </blink>Nothing was modified";
				}
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
			template.cpValue[1]="ispCRM.cgi";
			
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

			template.cpName[10]="cScheduleDate";
			template.cpValue[10]=cScheduleDate;

			template.cpName[12]="cCreatedBy";
			template.cpValue[12]=cCreatedBy;
			
			template.cpName[13]="cSubjectStyle";
			template.cpValue[13]=cSubjectStyle;
			
			template.cpName[14]="cTextStyle";
			template.cpValue[14]=cTextStyle;
			
			template.cpName[15]="cKeywordsStyle";
			template.cpValue[15]=cKeywordsStyle;

			template.cpName[16]="cScheduleDateStyle";
			template.cpValue[16]=cScheduleDateStyle;

			template.cpName[17]="cText";
			template.cpValue[17]=cText;

			template.cpName[18]="cCreatedDate";
			template.cpValue[18]=cCreatedDate;

			template.cpName[19]="cCommentStatus";
			if(uTicket)
				template.cpValue[19]="";
			else
				template.cpValue[19]="disabled";

			template.cpName[20]="cTicketCommentStyle";
			if(uTicket)
				template.cpValue[20]="type_textarea";
			else
				template.cpValue[20]="type_textarea_off";

			template.cpName[21]="cTicketComment";
			template.cpValue[21]=cTicketComment;

			template.cpName[22]="cCommentConfirm";
			template.cpValue[22]=cCommentConfirm;

			template.cpName[23]="gcInputStatus";
			template.cpValue[23]=gcInputStatus;

			template.cpName[24]="cKeywords";
			template.cpValue[24]=cKeywords;

			template.cpName[25]="uCreatedBy";
			template.cpValue[25]=cuCreatedBy;

			template.cpName[26]="";
			
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
			"cKeywords,uCreatedBy,FROM_UNIXTIME(uCreatedDate) FROM tTicket "
			"WHERE uTicket=%u AND (uOwner=%u OR uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u))",
			uTicket,guOrg,guOrg);
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

double ceil(double x);

void funcTicketNavList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uDisplayed=0;
	unsigned uFound=0;
	unsigned uResultsPerPage=10;
	unsigned uOffSet=0;
	unsigned uMaxPage=0;
	
	static char cTopMessage[100]={""};
	char cExtra[33]={""};


#define SEARCH_FIELDS "uTicket,cSubject,FROM_UNIXTIME(uCreatedDate)"

	if(cSearch[0])
	{
		sscanf(cSearch,"%u",&uTicket);
		if(uTicket)
			sprintf(gcQuery,"SELECT "SEARCH_FIELDS" FROM tTicket "
					"WHERE (uOwner=%u OR uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u)) "
					"AND uTicket=%u",
					guOrg
					,guOrg
					,uTicket);
		else
			sprintf(gcQuery,"SELECT "SEARCH_FIELDS" FROM tTicket "
					"WHERE (uOwner=%u OR uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u)) AND "
					"(cSubject LIKE '%%%s%%' OR cText LIKE '%%%s%%') "
					"ORDER BY uCreatedDate DESC",
					guOrg
					,guOrg
					,cSearch
					,cSearch
					);
		
	}
	else
		sprintf(gcQuery,"SELECT "SEARCH_FIELDS" FROM tTicket "
				"WHERE uOwner=%u OR uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u)"
				"ORDER BY uCreatedDate DESC",
				guOrg
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
			fprintf(fp,"<a href=ispCRM.cgi?gcPage=Ticket&uTicket=%s>Ticket #%s</a> %s (%s)<br>\n",
				field[0]
				,field[0]
				,field[1]
				,field[2]);
			mysql_free_result(res);
			return;
		}
	}
	else
	{
		uOffSet=(uPage-1)*uResultsPerPage;
		uMaxPage=ceil((double)((double)uFound/(double)uResultsPerPage));
		sprintf(cExtra," LIMIT %u,%u",uOffSet,uResultsPerPage);
		
		mysql_free_result(res);
		
		strcat(gcQuery,cExtra);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
	}
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<a href=ispCRM.cgi?gcPage=Ticket&uTicket=%s>Ticket #%s</a> %s (%s)<br>\n",
			field[0]
			,field[0]
			,field[1]
			,field[2]);
		uDisplayed++;
	}
	
	register int x;
	for(x=1;x<(uMaxPage+1);x++)
		fprintf(fp,"<a href=ispCRM.cgi?gcPage=Ticket&uPage=%i&cSearch=%s>%i</a>&nbsp;",x,cSearch,x);

	mysql_free_result(res);


}//void funcTicketNavList(FILE *fp)


void funcAssignedTo(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcSelectAccountType(fp) start -->\n");
	
	sprintf(gcQuery,"SELECT uClient,tClient.cLabel FROM tClient,tAuthorize WHERE "
			"tAuthorize.uCertClient=tClient.uClient AND tClient.uOwner=%u "
			"ORDER BY tClient.cLabel",guOrg);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	fprintf(fp,"<input type=hidden name=uTicketOwner value=%u>\n",uTicketOwner);

	fprintf(fp,"<select class=%s %s title='Select the employee to re-assign this ticket to' name=uTicketOwner>\n",uTicketOwnerStyle,gcInputStatus);

	sprintf(gcQuery,"%u",uTicketOwner);

	fprintf(fp,"<option value=0 ");
	if(!uTicketOwner)
		fprintf(fp,"selected");
	fprintf(fp,">---</option>\n");
	
	sprintf(gcQuery,"%u",uTicketOwner);

	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<option value=%s ",field[0]);
		if(!strcmp(field[0],gcQuery))
				fprintf(fp,"selected");
		fprintf(fp,">%s</option>\n",field[1]);
	}
	fprintf(fp,"</select>\n");
	fprintf(fp,"<!-- funcSelectAccountType(fp) end -->\n");
	

}//void funcAssignedTo(FILE *fp)


void funcTicketStatus(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcSelectAccountType(fp) start -->\n");
	
	sprintf(gcQuery,"SELECT uTicketStatus,cLabel FROM tTicketStatus");

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	
	fprintf(fp,"<input type=hidden name=uTicketStatus value=%u>\n",uTicketStatus);

	fprintf(fp,"<select class=%s %s title='Select current ticket status' name=uTicketStatus>\n",uTicketStatusStyle,gcInputStatus);

	sprintf(gcQuery,"%u",uTicketStatus);

	fprintf(fp,"<option value=0 ");
	if(!uTicketOwner)
		fprintf(fp,"selected");
	fprintf(fp,">---</option>\n");
	
	sprintf(gcQuery,"%u",uTicketStatus);

	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<option value=%s ",field[0]);
		if(!strcmp(field[0],gcQuery))
				fprintf(fp,"selected");
		fprintf(fp,">%s</option>\n",field[1]);
	}
	
	fprintf(fp,"<!-- funcSelectAccountType(fp) end -->\n");
	


}//void funcTicketStatus(FILE *fp)


void funcTicketNavBar(FILE *fp)
{
	if(guPermLevel>=10)
		fprintf(fp,"<input type=submit title='Customer modification with a two step procedure'"
				" class=largeButton name=gcFunction value='%sNew' />",gcNewStep);

	if(uTicket)
	{
		fprintf(fp,"<input type=submit title='Customer modification with a two step procedure'"
				" class=largeButton name=gcFunction value='%sModify' />",gcModStep);
	}

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
			"<td><a class=darkLink href=\"#\" onClick=\"open_popup('ispCRM.cgi?gcPage=TicketComment&uTicketComment=%s')\">%s</a>"
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


void ModTicket(void)
{
	sprintf(gcQuery,"UPDATE tTicket SET uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()),"
			"uTicketStatus=%u,uTicketOwner=%u,uScheduleDate=%lu,cText='%s',"
			"cKeywords='%s',cSubject='%s' WHERE uTicket=%u",
			guLoginClient
			,uTicketStatus
			,uTicketOwner
			,uScheduleDate
			,TextAreaSave(cText)
			,TextAreaSave(cKeywords)
			,TextAreaSave(cSubject)
			,uTicket
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void ModTicket(void)


void SetTicketFieldsOn(void)
{
	uTicketStatusStyle="type_fields";
	uTicketOwnerStyle="type_fields";
	cScheduleDateStyle="type_fields";
	cTextStyle="type_textarea";
	cKeywordsStyle="type_fields";
	cSubjectStyle="type_fields";

}//void SetTicketFieldsOn(void)


void EmailTicketChanges(void)
{
	//
	//This function will be run before ModTicket() call, and it will compare database values against 
	//the values we are commiting to the database.
	//Based on this comparisson will inform of the diferences via email.
	structTicket RecordData;
	FILE *fp;

	//debug only
	//if((fp=fopen("/tmp/eMailInvoice","w")))
	if((fp=popen("/usr/lib/sendmail -t > /dev/null","w")))
	{
		fpEmailTicketHeader(fp);

		LoadRecordIntoStruct(&RecordData);
		if(uTicketStatus!=RecordData.uTicketStatus)
		{
			//uTicketStatus changed
			fprintf(fp,"Ticket status changed:\n%s->%s\n",
				ForeignKey("tTicketStatus","cLabel",RecordData.uTicketStatus)
				,ForeignKey("tTicketStatus","cLabel",uTicketStatus)
				);
		}
		if(uTicketOwner!=RecordData.uTicketOwner)
		{
			//uTicketOwner changed
			fprintf(fp,"Ticket reassigned:\n%s->%s\n",
				ForeignKey("tClient","cLabel",RecordData.uTicketOwner)
				,ForeignKey("tClient","cLabel",uTicketOwner)
				);
		}
		if(uScheduleDate!=RecordData.uScheduleDate)
		{
			//uScheduleDate changed
			fprintf(fp,"Ticket rescheduled:\n%s->%s\n",
				cFromUnixTime(RecordData.uScheduleDate)
				,cFromUnixTime(uScheduleDate)
				);
		}
		if(strcmp(TextAreaSave(cText),RecordData.cText))
		{
			//cText changed
			fprintf(fp,"Ticket description updated\n");
			fprintf(fp,"Old description:\n%s\n\n",RecordData.cText);
			fprintf(fp,"New description:\n%s\n",cText);
			
		}
		if(strcmp(cSubject,RecordData.cSubject))
		{
			//cSubject changed
			fprintf(fp,"Ticket subject changed\n");
			fprintf(fp,"Old subject: %s\n",RecordData.cSubject);
			fprintf(fp,"New subject: %s\n",cSubject);
		}

		fileDirectTemplate(fp,"TicketChangeMailFooter");
		//fclose(fp);
		pclose(fp);
	}	

}//void EmailTicketChanges(void)
	

unsigned uTicketChanged(void)
{
	structTicket RecordData;

	LoadRecordIntoStruct(&RecordData);

	if(uTicketStatus!=RecordData.uTicketStatus) return(1);
	if(uTicketOwner!=RecordData.uTicketOwner) return(1);
	if(uScheduleDate!=RecordData.uScheduleDate) return(1);
	if(strcmp(TextAreaSave(cText),RecordData.cText)) return(1);
	if(strcmp(cSubject,RecordData.cSubject)) return(1);
	
	return(0);

}//unsigned uTicketChanged(void)


void LoadRecordIntoStruct(structTicket *Target)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate,uTicketStatus,uTicketOwner,uScheduleDate,"
			"cText,cKeywords,cSubject FROM tTicket WHERE uTicket=%u AND (uOwner=%u OR uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u))",
			uTicket,guOrg,guOrg);
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
	char cReporterEmail[256]={""};
	char cEmail[100]={""};
	char cuTicket[16]={""};

	cSubject[255]=0;
	LoadRecordIntoStruct(&RecordData);

	GetConfiguration("cReportTicketEmail",cEmail);
	GetConfiguration("cReportTicketFrom",cFrom);

	sprintf(cEmailSubject,"#%u %s",uTicket,RecordData.cSubject);
	
	sprintf(cReporterEmail,"%.255s",ForeignKey("tClient","cEmail",uCreatedBy));

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
	sprintf(cCreatedBy,"%.99s",ForeignKey("tClient","cLabel",uCreatedBy));
	template.cpValue[2]=cCreatedBy;

	char cAssignedTo[100]={""};
	template.cpName[3]="cAssignedTo";
	sprintf(cAssignedTo,"%.99s",ForeignKey("tClient","cLabel",uTicketOwner));
	template.cpValue[3]=cAssignedTo;

	char cStatus[33]={""};
	template.cpName[4]="cStatus";
	sprintf(cStatus,"%.32s",ForeignKey("tTicketStatus","cLabel",uTicketStatus));
	template.cpValue[4]=cStatus;

	template.cpName[5]="cKeywords";
	template.cpValue[5]=cKeywords;

	template.cpName[6]="cScheduleDate";
	if(uScheduleDate)
		template.cpValue[6]=cFromUnixTime(uScheduleDate);
	else
		template.cpValue[6]="---";

	template.cpName[7]="cCreatedDate";
	template.cpValue[7]=cCreatedDate;

	template.cpName[8]="";

	fpTemplate(fp,"TicketChangeMailTop",&template);

}//void fpEmailTicketHeader(FILE *fp)
