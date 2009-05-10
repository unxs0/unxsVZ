/*
FILE 
	customer.c
	$Id: customer.c 295 2008-05-29 14:59:18Z Hugo $
AUTHOR
	(C) 2006-2008 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	mysqlISP2 Admin (Owner) Interface
	program file.
*/


#include "interface.h"

unsigned uCustomer=0;

static char cFirstName[33]={""};
static char *cFirstNameStyle="type_fields";

static char cLastName[33]={""};
static char *cLastNameStyle="type_fields";

static char cEmail[101]={""};
static char *cEmailStyle="type_fields";

static char cAddr1[101]={""};
static char *cAddr1Style="type_fields";

static char cCity[101]={""};
static char *cCityStyle="type_fields";

static char cZip[33]={""};
static char *cZipStyle="type_fields";

static char cCountry[65]={""};
static char *cCountryStyle="type_fields";

static char cTelephone[33]={""};
static char *cTelephoneStyle="type_fields";

static char cMAC[65]={""};
static char *cMACStyle="type_fields";

static char cCompany[101]={""};
static char *cCompanyStyle="type_fields";

static char cHostName[65]={""};
static char *cHostNameStyle="type_fields";

static unsigned uVPN=0;

static unsigned uProduct=0;
static unsigned uStep=0;

//
//Local only
unsigned ValidateInput(void);
void CommitRegistration(void);
void LowerCase(char *cString);
void mysqlRadius2DbConnect(void);
unsigned CheckRadiusUser(char *cLogin);
void CleanForm(void);
unsigned uSetCompany(char *cLabel);

MYSQL mysqlRadius2Db;


void ProcessCustomerVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{

		if(!strcmp(entries[i].name,"uCustomer"))
			sscanf(entries[i].val,"%u",&uCustomer);
		else if(!strcmp(entries[i].name,"uStep"))
			sscanf(entries[i].val,"%u",&uStep);
		else if(!strcmp(entries[i].name,"uProduct"))
			sscanf(entries[i].val,"%u",&uProduct);
		else if(!strcmp(entries[i].name,"cFirstName"))
			sprintf(cFirstName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLastName"))
			sprintf(cLastName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cEmail"))
			sprintf(cEmail,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cAddr1"))
			sprintf(cAddr1,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCity"))
			sprintf(cCity,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cZip"))
			sprintf(cZip,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCountry"))
			sprintf(cCountry,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cTelephone"))
			sprintf(cTelephone,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cMAC"))
			sprintf(cMAC,"%.18s",entries[i].val);
		else if(!strcmp(entries[i].name,"uVPN"))
			sscanf(entries[i].val,"%u",&uVPN);
		else if(!strcmp(entries[i].name,"uProduct"))
			sscanf(entries[i].val,"%u",&uProduct);
		else if(!strcmp(entries[i].name,"cCompany"))
			sprintf(cCompany,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cHostName"))
			sprintf(cHostName,"%.64s",entries[i].val);
	}

}//void ProcessUserVars(pentry entries[], int x)


void CustomerGetHook(entry gentries[],int x)
{
	htmlCustomer();

}//void CustomerGetHook(entry gentries[],int x)


void CustomerCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Registration"))
	{
		ProcessCustomerVars(entries,x);

		if(!strcmp(gcFunction,"Register"))
		{	
			sprintf(gcNewStep,"Confirm ");
			ValidateInput();
		}
		else if(!strcmp(gcFunction,"Confirm Register"))
		{
			if(ValidateInput())
			{
				sprintf(gcNewStep,"Confirm ");
				htmlCustomer();
			}
			CommitRegistration();
			htmlHeader("Riedel GmbH Registration Page","Header");
			htmlCustomerPage("","Registration.End");
			htmlFooter("Footer");
		}
		htmlCustomer();
	}

}//void CustomerCommands(pentry entries[], int x)


void htmlProductDeployWizard(unsigned uStep)
{
	htmlHeader("mysqlISP2 Admin Interface","Header");
	sprintf(gcQuery,"ProductDeploy.%u",uStep);
	htmlCustomerPage("",gcQuery);
	
	htmlFooter("Footer");
}
void htmlCustomer(void)
{
	htmlHeader("Riedel GmbH Registration Page","Header");
	htmlCustomerPage("","Registration.Body");
	htmlFooter("Footer");

}//void htmlCustomer(void)


void htmlCustomerPage(char *cTitle, char *cTemplateName)
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
		
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="riedelReg.cgi";
			
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

			template.cpName[8]="gcInputStatus";
			template.cpValue[8]=gcInputStatus;
				
			template.cpName[9]="gcNewStep";
			template.cpValue[9]=gcNewStep;

			template.cpName[10]="gcModStep";
			template.cpValue[10]=gcModStep;
			
			template.cpName[11]="gcDelStep";
			template.cpValue[11]=gcDelStep;

			template.cpName[12]="cFirstName";
			template.cpValue[12]=cFirstName;
			
			template.cpName[13]="cFirstNameStyle";
			template.cpValue[13]=cFirstNameStyle;
			
			template.cpName[14]="cLastName";
			template.cpValue[14]=cLastName;
			
			template.cpName[15]="cEmail";
			template.cpValue[15]=cEmail;
			
			template.cpName[16]="cEmailStyle";
			template.cpValue[16]=cEmailStyle;
			
			template.cpName[17]="cAddr1";
			template.cpValue[17]=cAddr1;
			
			template.cpName[18]="cAddr1Style";
			template.cpValue[18]=cAddr1Style;
			
			template.cpName[19]="cTelephone";
			template.cpValue[19]=cTelephone;

			template.cpName[20]="cTelephoneStyle";
			template.cpValue[20]=cTelephoneStyle;

			template.cpName[21]="cCity";
			template.cpValue[21]=cCity;
			
			template.cpName[22]="cCityStyle";
			template.cpValue[22]=cCityStyle;

			template.cpName[23]="cLastNameStyle";
			template.cpValue[23]=cLastNameStyle;

			template.cpName[24]="cZip";
			template.cpValue[24]=cZip;

			template.cpName[25]="cZipStyle";
			template.cpValue[25]=cZipStyle;

			template.cpName[26]="cCountry";
			template.cpValue[26]=cCountry;
			
			template.cpName[27]="cCountryStyle";
			template.cpValue[27]=cCountryStyle;
			
			template.cpName[28]="cMAC";
			template.cpValue[28]=cMAC;

			template.cpName[29]="cMACStyle";
			template.cpValue[29]=cMACStyle;

			template.cpName[30]="cCompany";
			template.cpValue[30]=cCompany;

			template.cpName[31]="cCompanyStyle";
			template.cpValue[31]=cCompanyStyle;

			template.cpName[32]="cHostName";
			template.cpValue[32]=cHostName;

			template.cpName[33]="cHostNameStyle";
			template.cpValue[33]=cHostNameStyle;

			template.cpName[34]="";


			printf("\n<!-- Start htmlCustomerPage(%s) -->\n",cTemplateName); 
//			fpTemplate(stdout,cTemplateName,&template);
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlCustomerPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlCustomerPage()


void funcSelectProduct(FILE *fp)
{
	fprintf(fp,"<select name=uProduct class=type_fields>\n");
	
	fprintf(fp,"<option value=31 ");
	if(uProduct==31) fprintf(fp,"selected");
	fprintf(fp,">Team Captain</option>\n");

	fprintf(fp,"<option value=21 ");
	if(uProduct==21) fprintf(fp,"selected");
	fprintf(fp,">Event Management</option>\n");
	
	fprintf(fp,"<option value=1 ");
	if(uProduct==1) fprintf(fp,"selected");
	fprintf(fp,">Staff</option>\n");
	
	fprintf(fp,"<option value=41 ");
	if(uProduct==41) fprintf(fp,"selected");
	fprintf(fp,">Media</option>\n");

	fprintf(fp,"<option value=51 ");
	if(uProduct==51) fprintf(fp,"selected");
	fprintf(fp,">Mediastation</option>\n");

	
	fprintf(fp,"</select>\n");

}//void func funcSelectProduct(FILE *fp)


void funcSelectVPN(FILE *fp)
{
	fprintf(fp,"<select name=uVPN class=type_fields>\n");

	fprintf(fp,"<option value=0 ");
	if(uVPN==0) fprintf(fp,"selected");
	fprintf(fp,">No</option>\n");

	fprintf(fp,"<option value=3 ");
	if(uVPN==3) fprintf(fp,"selected");
	fprintf(fp,">Don't Know\n");

	fprintf(fp,"<option value=1 ");
	if(uVPN==1) fprintf(fp,"selected");
	fprintf(fp,">Yes</option>\n");


	fprintf(fp,"</select>\n");

}//void funcSelectVPN(FILE *fp)


void funcRegTerms(FILE *fp)
{
	struct t_template template;

	template.cpName[0]="";
	fpTemplate(fp,"Registration.Terms",&template);

}//void funcRegTerms(FILE *fp)

void CleanForm(void)
{
}//void CleanForm(void)


unsigned ValidateInput(void)
{
	MYSQL_RES *res;

	if(!cLastName[0])
	{
		cLastNameStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Last Name is required!";
		return(1);
	}
	else
	{
		LowerCase(cLastName);
		cLastName[0]=toupper(cLastName[0]);
	}

	if(!cFirstName[0])
	{
		cFirstNameStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>First Name is required!";
		return(1);
	}
	else
	{
		LowerCase(cFirstName);
		cFirstName[0]=toupper(cFirstName[0]);
	}
	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cFirstName='%s' AND cLastName='%s'",TextAreaSave(cFirstName),TextAreaSave(cLastName));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if(mysql_num_rows(res))
	{
		cFirstNameStyle="type_fields_req";
		cLastNameStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Customer already exists!";
		return(1);
	}
	if(!cCompany[0])
	{
		cCompanyStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Company is required!";
		return(1);
	}
	else
	{
		LowerCase(cCompany);
		cCompany[0]=toupper(cCompany[0]);
	}
	if(!cAddr1[0])
	{
		cAddr1Style="type_fields_req";
		gcMessage="<blink>Error: </blink>Street Address is required!";
		return(1);
	}
	if(!cCity[0])
	{
		cCityStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>City is required!";
		return(1);
	}
	if(!cZip[0])
	{
		cZipStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Zip Code is required!";
		return(1);
	}
	if(!cCountry[0])
	{
		cCountryStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Country is required!";
		return(1);
	}
	if(!cTelephone[0])
	{
		cTelephoneStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Telephone is required!";
		return(1);
	}
	if(!cEmail[0])
	{
		cEmailStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Email is required!";
		return(1);
	}
	else
	{
		register int x;
		if(!strchr(cEmail,'@') && !strchr(cEmail,'.'))
		{
			cEmailStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>Email format is invalid";
			return(1);
		}
		for(x=0;x<strlen(cEmail);x++)
		{
			if(!isalnum(cEmail[x]) && cEmail[x]!='@' && cEmail[x]!='.')
			{
				cEmailStyle="type_fields_req";
				gcMessage="<blink>Error: </blink>Email format is invalid";
				return(1);
			}
		}
	}
	if(!cMAC[0])
	{
		cMACStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>MAC is required!";
		return(1);
	}
	else
	{
		MYSQL_RES *res;
		unsigned a,b,c,d,e,f;

		sscanf(cMAC,"%x:%x:%x:%x:%x:%x",&a,&b,&c,&d,&e,&f);
		if(a>0xff||b>0xff||c>0xff||d>0xff||e>0xff||f>0xff)
		{
			cMACStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>MAC data incorrect";
			return(1);
		}
		sprintf(cMAC,"%02x:%02x:%02x:%02x:%02x:%02x",a,b,c,d,e,f);
		//Check : chars
		//00:50:8D:A3:76:D3
		if(cMAC[2]!=':'||cMAC[5]!=':'||cMAC[8]!=':'||cMAC[11]!=':'||cMAC[14]!=':')
		{
			cMACStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>MAC format incorrect";
			return(1);
		}
		sprintf(gcQuery,"SELECT uConfig FROM tClientConfig WHERE cValue LIKE '%%%s%%'",cMAC);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res))
		{
			cMACStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>MAC already in use!";
			return(1);
		}
		mysql_free_result(res);
		if(CheckRadiusUser(cMAC))
		{
			cMACStyle="type_fields_req";
			gcMessage="<blink>Error: </blink>MAC already in use!";
			return(1);
		}
	}
	/*
	if(!cHostName[0])
	{
		cHostNameStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Hostname is required!";
		return(1);
	}
	*/
	
	return(0);
}//unsigned ValidateInput(void)


void CommitRegistration(void)
{
	//This function processes the entered data and commits the deployment of the selected product
	//for a given customer. The job entries are not created. This must be later done using ispAdmin
	
	//Insert tClient record.
	unsigned uOwner=0;
	unsigned uClient=0;
	unsigned uInstance=0;
	
	MYSQL_RES *res;
	MYSQL_ROW field;
	MYSQL_RES *res2;
	MYSQL_ROW field2;

	char cProfileName[100]={""};
	char cAPLogin[100]={""};
	char cAPPasswd[100]={""};
	char SAFLogin[100]={""};
	char SAFPasswd[100]={""};
	char *cVPN="";
	
	if(uVPN==0) 
		cVPN="VPN No";
	else if(uVPN==1)
		cVPN="VPN Yes";
	else if(uVPN==3)
		cVPN="VPN Dont know";

	uOwner=uSetCompany(cCompany);

	sprintf(gcQuery,"INSERT INTO tClient SET cLabel='%s %s',cFirstName='%s',cLastName='%s',cEmail='%s',cAddr1='%s',cCity='%s',"
			"cZip='%s',cCountry='%s',cTelephone='%s',cAddr2='%s',cAddr3='%s',uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW()),uOwner=%u",
			TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cEmail)
			,TextAreaSave(cAddr1)
			,TextAreaSave(cCity)
			,TextAreaSave(cZip)
			,TextAreaSave(cCountry)
			,TextAreaSave(cTelephone)
			,cHostName
			,cVPN
			,uOwner
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uClient=mysql_insert_id(&gMysql);

	//Insert tClientConfig records

	//All products have the same common parameters, except profile name
	//There's a pair of products to be deployed, one with the MAC as username/passwd
	//And the other with cFirstName.cLastName, random password

	//Set common parameter values.
	sprintf(cAPLogin,"%.99s",cMAC);
	GetConfiguration("cRegInterfaceKey",cAPPasswd);
	LowerCase(cFirstName);
	LowerCase(cLastName);

	sprintf(SAFLogin,"%s.%s",cFirstName,cLastName);
	sprintf(SAFPasswd,"wsxedc"); //Later will be random generated password, just starting to code ;)

	//Set profile name based on selected product
	sprintf(gcQuery,"SELECT uService AS uServID,(SELECT cLabel FROM tService WHERE uService=uServID) FROM tServiceGlue WHERE uServiceGroup=%u",uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);

	//uStatus=21: Waiting for Approval
	sprintf(gcQuery,"INSERT INTO tInstance SET uClient=%u,uProduct=%u,cLabel='%s',uStatus=21,uCreatedBy=1,uOwner=%u,"
			"uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uClient
			,uProduct
			,ForeignKey("tProduct","cLabel",uProduct)
			,uOwner);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uInstance=mysql_insert_id(&gMysql);

	while((field=mysql_fetch_row(res)))
	{
		//uParameter=3=mysqlRadius2.Profile
		sprintf(gcQuery,"SELECT cValue FROM tConfig WHERE uGroup=%s AND uParameter=3",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
		{
			//Start the real thing here
			sprintf(cProfileName,"%.99s",field2[0]);

			if(strstr(field[1],"Cisco"))
			{
				//uParameter=1: mysqlRadius2.Login
				sprintf(gcQuery,"INSERT INTO tClientConfig SET uGroup=%u,uService=%s,uParameter=1,cValue='%s',"
						"uCreatedBy=1,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uInstance
						,field[0]
						,cAPLogin
						,uClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				//uParameter=2: mysqlRadius2.Passwd
				sprintf(gcQuery,"INSERT INTO tClientConfig SET uGroup=%u,uService=%s,uParameter=2,cValue='%s',"
						"uCreatedBy=1,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uInstance
						,field[0]
						,cAPPasswd
						,uClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
			}
			else if(strstr(field[1],"unxsCP"))
			{
				//uParameter=1: mysqlRadius2.Login
				sprintf(gcQuery,"INSERT INTO tClientConfig SET uGroup=%u,uService=%s,uParameter=1,cValue='%s',"
						"uCreatedBy=1,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uInstance
						,field[0]
						,SAFLogin
						,uClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				//uParameter=2: mysqlRadius2.Passwd
				sprintf(gcQuery,"INSERT INTO tClientConfig SET uGroup=%u,uService=%s,uParameter=2,cValue='%s',"
						"uCreatedBy=1,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uInstance
						,field[0]
						,SAFPasswd
						,uClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				//uParameter=51: mysqlRadius2.IP
				sprintf(gcQuery,"INSERT INTO tClientConfig SET uGroup=%u,uService=%s,uParameter=51,cValue='%s',"
						"uCreatedBy=1,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uInstance
						,field[0]
						,cAPLogin
						,uClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
			}
			//uParameter=3: mysqlRadius2.Profile
			sprintf(gcQuery,"INSERT INTO tClientConfig SET uGroup=%u,uService=%s,uParameter=3,cValue='%s',"
					"uCreatedBy=1,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uInstance
					,field[0]
					,cProfileName
					,uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			
			//uParameter=41: mysqlRadius2.ClearText
			sprintf(gcQuery,"INSERT INTO tClientConfig SET uGroup=%u,uService=%s,uParameter=41,cValue='%s',"
					"uCreatedBy=1,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
					uInstance
					,field[0]
					,"Yes"
					,uClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		}//if((field2=mysql_fetch_row(res)))
		mysql_free_result(res2);
	}//while(...)
	
	mysql_free_result(res);


}//void CommitRegistration(void)


void LowerCase(char *cString)
{
	register int x;
	for(x=0;x<strlen(cString);x++)
	{
		if(cString[x]==' ') break;
		cString[x]=tolower(cString[x]);
	}

}//void LowerCase(char *cString)


void mysqlRadius2DbConnect(void)
{
	char cLogin[100]={"mysqlradius2"};
	char cPasswd[100]={"wsxedc"};
	char cDb[100]={"mysqlradius2"};
	char cHost[100]="localhost";

	GetConfiguration("cExtmysqlRadius2DbIp",cHost);
	GetConfiguration("cExtmysqlRadius2DbName",cDb);
	GetConfiguration("cExtmysqlRadius2DbLogin",cLogin);
	GetConfiguration("cExtmysqlRadius2DbPwd",cPasswd);

        mysql_init(&mysqlRadius2Db);
        if(!mysql_real_connect(&mysqlRadius2Db,cHost,cLogin,cPasswd,cDb,0,NULL,0))
		htmlPlainTextError(mysql_error(&mysqlRadius2Db));

}//void mysqlRadius2DbConnect(void)


unsigned CheckRadiusUser(char *cLogin)
{
	MYSQL_RES *res;
	unsigned uRet=0;

	mysqlRadius2DbConnect();

	sprintf(gcQuery,"SELECT uUser FROM tUser WHERE cLogin='%s'",cLogin);
	mysql_query(&mysqlRadius2Db,gcQuery);
	if(mysql_errno(&mysqlRadius2Db))
		htmlPlainTextError(mysql_error(&mysqlRadius2Db));
	
	res=mysql_store_result(&mysqlRadius2Db);
	uRet=mysql_num_rows(res);

	mysql_free_result(res);
	mysql_close(&mysqlRadius2Db);

	return(uRet);

}//unsigned CheckRadiusUser(char *cLogin)


unsigned uSetCompany(char *cLabel)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uOwner=11; //Will come from tConfiguration. Hardcoded to 'Riedel'
	unsigned uClient=0;

	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE uOwner=%u AND cLabel='%s'",uOwner,cLabel);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uClient);
	else
	{
		sprintf(gcQuery,"INSERT INTO tClient SET cLabel='%s',uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",cLabel,uOwner);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		uClient=mysql_insert_id(&gMysql);
	}
	mysql_free_result(res);
	return(uClient);

}//unsigned uSetCompany(char *cLabel)



