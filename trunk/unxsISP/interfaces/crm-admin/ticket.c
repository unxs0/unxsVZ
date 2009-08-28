/*
FILE 
	order.c
	$Id: invoice.c 508 2009-08-13 23:17:58Z Hugo $
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
static unsigned uCreatedBy=0; 
static char cCreatedDate[64]={""};

static unsigned uTicketStatus=0;
static char *uTicketStatusStyle="type_fields_off";

static unsigned uTicketOwner=0;
static char *uTicketOwnerStyle="type_fields_off";

static unsigned uScheduleDate=0;
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


void htmlTicketPage(char *cTitle, char *cTemplateName);
void LoadTicket(void);
unsigned ValidateTicketInput(unsigned uMode);
void NewTicket(void);
void ModTicket(void);
void SetTicketFieldsOn(void);
void SubmitComment(void);
void EmailTicketChanges(void);
void LoadRecordIntoStruct(structTicket *Target);
void fileDirectTemplate(FILE *fp,char *cTemplateName);


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
		}
		else if(!strcmp(entries[i].name,"cKeywords"))
			sprintf(cKeywords,"%.255s",entries[i].val);
		else if(!strcmp(entries[i].name,"uCreatedBy"))
			sscanf(entries[i].val,"%u",&uCreatedBy);
		//else if(!strcmp(entries[i].name,"uCreatedDate"))
		//	sscanf(entries[i].val,"%u",&uCreatedDate);

	}

}//void ProcessUserVars(pentry entries[], int x)


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
			SubmitComment();
		else if(!strcmp(gcFunction,"New"))
		{
			SetTicketFieldsOn();
			sprintf(gcNewStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm New"))
		{
			if(ValidateTicketInput(0))
			{
				SetTicketFieldsOn();
				sprintf(gcNewStep,"Confirm ");
			}
			else
				NewTicket();
		}
		else if(!strcmp(gcFunction,"Modify"))
		{
			SetTicketFieldsOn();
			sprintf(gcModStep,"Confirm ");
		}
		else if(!strcmp(gcFunction,"Confirm Modify"))
		{
			if(ValidateTicketInput(1))
			{
				SetTicketFieldsOn();
				sprintf(gcModStep,"Confirm ");
			}
			else
			{
				EmailTicketChanges();
				ModTicket();
			}
		}

		htmlTicket();
	}

}//void TicketCommands(pentry entries[], int x)


void htmlTicket(void)
{
	htmlHeader("unxsISP Admin","Header");
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
			struct t_template template;
			char cuTicket[16]={""};
			char cScheduleDate[32]={""};			
			char cCreatedBy[100]={""};

			sprintf(cCreatedBy,"%.99s",ForeignKey("tClient","cLabel",uCreatedBy));
			sprintf(cuTicket,"%u",uTicket);

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
			
			template.cpName[17]="cKeywordsStyle";
			template.cpValue[17]=cKeywordsStyle;

			template.cpName[18]="cScheduleDateStyle";
			template.cpValue[18]=cScheduleDateStyle;

			template.cpName[19]="cText";
			template.cpValue[19]=cText;

			template.cpName[20]="cCreatedDate";
			template.cpValue[20]=cCreatedDate;

			template.cpName[21]="cCommentStatus";
			if(uTicket)
				template.cpValue[21]="";
			else
				template.cpValue[21]="disabled";

			template.cpName[22]="cTicketCommentStyle";
			if(uTicket)
				template.cpValue[22]="type_textarea";
			else
				template.cpValue[22]="type_textarea_off";

			template.cpName[23]="cTicketComment";
			template.cpValue[23]=cTicketComment;

			template.cpName[24]="cCommentConfirm";
			template.cpValue[24]=cCommentConfirm;

			template.cpName[25]="";

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
		sscanf(field[4],"%u",&uScheduleDate);
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
/*	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uDisplayed=0;
	unsigned uFound=0;
	char cTopMessage[100]={""};

	if(cSearch[0])
	{
	//Valid formats are:
	//1. uTicket-uClient
	//2. uTicket
	//3. Part of last name
		char cExtra[100]={""};
		if(strstr(cSearch,"-"))
		{
			unsigned uClient=0;

			sscanf(cSearch,"%u-%u",&uClient,&uTicket);
			sprintf(cExtra,"tTicket.uTicket=%u AND tTicket.uClient=%u",uTicket,uClient);

			ExtSelectSearch("tTicket","tTicket.uTicket,FROM_UNIXTIME(GREATEST(tTicket.uCreatedDate,tTicket.uModDate)),"
				"(SELECT CONCAT(cFirstName,' ',cLastName) FROM tClient WHERE tClient.uClient=tTicket.uClient),tTicket.uClient","1","1",
				cExtra,0);
		}
		else
		{
			sscanf(cSearch,"%u",&uTicket);
			if(uTicket)
			{
				sprintf(cExtra,"tTicket.uTicket=%u",uTicket);
				ExtSelectSearch("tTicket","tTicket.uTicket,FROM_UNIXTIME(GREATEST(tTicket.uCreatedDate,tTicket.uModDate)),"
					"(SELECT CONCAT(cFirstName,' ',cLastName) FROM tClient WHERE tClient.uClient=tTicket.uClient),tTicket.uClient",
					"1","1",cExtra,0);
			}
			else
				ExtSelectSearch("tTicket","tTicket.uTicket,FROM_UNIXTIME(GREATEST(tTicket.uCreatedDate,tTicket.uModDate)),"
					"(SELECT CONCAT(cFirstName,' ',cLastName) FROM tClient WHERE tClient.uClient=tTicket.uClient),tTicket.uClient",
					"tTicket.cLastName",cSearch,NULL,0);
		}
	}
	else
		ExtSelect("tTicket","tTicket.uTicket,FROM_UNIXTIME(GREATEST(tTicket.uCreatedDate,tTicket.uModDate)),"
				"(SELECT CONCAT(cFirstName,' ',cLastName) FROM tClient WHERE tClient.uClient=tTicket.uClient),tTicket.uClient",0);
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
			//This 'loads' the invoice, via funcTicket() ;)
			sscanf(field[0],"%u",&uTicket);
			sscanf(field[3],"%u",&uClient);
			
			fprintf(fp,"<a href=ispCRM.cgi?gcPage=Ticket&uTicket=%s&uClient=%s>%s - %s</a><br>\n",field[0],field[3],field[2],field[1]);
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
		FromMySQLDate(field[1]);
		fprintf(fp,"<a href=ispCRM.cgi?gcPage=Ticket&uTicket=%s&uClient=%s>%s - %s</a><br>\n",field[0],field[3],field[2],field[1]);
		uDisplayed++;
	}

	mysql_free_result(res);
*/
}//void funcTicketNavList(FILE *fp)


void funcAssignedTo(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcSelectAccountType(fp) start -->\n");
	
	sprintf(gcQuery,"SELECT uClient,tClient.cLabel FROM tClient,tAuthorize WHERE tAuthorize.uCertClient=tClient.uClient AND tClient.uOwner=%u",guOrg);

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

	fprintf(fp,"<select class=%s %s title='Select current ticket status' name=uTicketOwner>\n",uTicketStatusStyle,gcInputStatus);

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
			"uTicketOwner=%u,uScheduleDate=%u,cText='%s',"
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

}//void NewTicket(void)


void ModTicket(void)
{
	sprintf(gcQuery,"UPDATE tTicket SET uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()),"
			"uTicketStatus=%u,uTicketOwner=%u,uScheduleDate=%u,cText='%s',"
			"cKeywords='%s',cSubject='%s'",
			guLoginClient
			,uTicketStatus
			,uTicketOwner
			,uScheduleDate
			,TextAreaSave(cText)
			,TextAreaSave(cKeywords)
			,TextAreaSave(cSubject)
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
	//struct t_template template;
	FILE *fp;
	char cFrom[256]={"root"};
	char cSubject[256]={""};
	char cEmail[100]={""};

	cSubject[255]=0;
	LoadRecordIntoStruct(&RecordData);

	//if((fp=popen("/usr/lib/sendmail -t > /dev/null","w")))
	//debug only
	if((fp=fopen("/tmp/eMailInvoice","w")))
	{
		fprintf(fp,"To: %s\n",cEmail);
		fprintf(fp,"From: %s\n",cFrom);
		fprintf(fp, "Reply-to: %s\n",cFrom);
		fprintf(fp,"Subject: %s\n",cSubject);
		fprintf(fp,"MIME-Version: 1.0\n");
		fprintf(fp,"Content-type: text/html\n\n");

		fileDirectTemplate(fp,"TicketChangeMailTop");
			
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
			fprintf(fp,"Ticket rescheduled:\n%u->%u\n",
				RecordData.uScheduleDate
				,uScheduleDate
				);
		}
		if(strcmp(cText,RecordData.cText))
		{
			//cText changed
			fprintf(fp,"Ticket description updated\n");
			fprintf(fp,"Old description:\n%s\n",RecordData.cText);
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
		fclose(fp);
	}	

}//void EmailTicketChanges(void)
	

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


