/*
FILE 
	myaccount.c
AUTHOR
	(C) 2008 Hugo Urquiza for Unixservice
PURPOSE
	unxsRadius Customer Interface
	program file.
	Initially, this module handles  all the password related requests.
*/

#include "interface.h"

static char cEmail[256]={""};

//
//Local only functions
unsigned uValidUser(char *cEmail);
void ResetPassword(char *cEmail);
void ResetPassword(char *cEmail);
void mysqlCartDbConnect(MYSQL *MySQLVector);
void htmlMyAccountPwdOut(void);
void LowerCase(char *cString);
void MyGetConfiguration(MYSQL *MySQLVector,char *cName,char *cValue);
void to64(register char *s, register long v, register int n);
char *GenerateRandomPassword(void);
void EncryptPasswd(MYSQL *MySQLVector,char *pw);
unsigned Update_tUser(unsigned uUser,char *cPasswd);


void ProcessMyAccountVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cEmail"))
			sprintf(cEmail,"%.255s",entries[i].val);
	}

}//void ProcessMyAccountVars(pentry entries[], int x)


void MyAccountGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
//		if(!strcmp(gentries[i].name,"cMyAccount"))
//			sprintf(gcMyAccount,"%.99s",gentries[i].val);
	}
	htmlMyAccount();

}//void MyAccountGetHook(entry gentries[],int x)


void MyAccountCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"MyAccount"))
	{
		ProcessMyAccountVars(entries,x);
		if(!strcmp(gcFunction,"Send me my new password"))
		{
			//Time saver check
			if(!uValidUser(cEmail))
			{
				gcMessage="<font color=red>There's no user account with the entered email</font>";
				htmlMyAccountPwdOut();
			}
			ResetPassword(cEmail);
			gcMessage="<font color=green>Your new password was sent by email</font>";
			htmlMyAccountPwdOut();
		}
		htmlMyAccount();
	}

}//void MyAccountCommands(pentry entries[], int x)


void htmlMyAccountPwdOut(void)
{
	 htmlHeader("unxsRadius User Interface","Header");
	 htmlMyAccountPage("unxsRadius User Interface","MyAccountPwdMsg");
	 htmlFooter("Footer");
}//void htmlMyAccountPwdOut(void)


void htmlMyAccount(void)
{
	htmlHeader("unxsRadius User Interface","Header");
	htmlMyAccountPage("unxsRadius User Interface","MyAccount.Body");
	htmlFooter("Footer");

}//void htmlMyAccount(void)


void htmlMyAccountPage(char *cTitle, char *cTemplateName)
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
			template.cpValue[1]="iRadius.cgi";
			
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

			template.cpName[7]="gcModStep";
			template.cpValue[7]=gcModStep;

			template.cpName[8]="cMessage";
			template.cpValue[8]=gcMessage;
			
			template.cpName[9]="";

			printf("\n<!-- Start htmlMyAccountPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlMyAccountPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlMyAccountPage()


unsigned uValidUser(char *cEmail)
{
	//This function checks if there's an user with the specified email address
	//Please note that to do so, we need to get a MySQL vector pointing to the mysqlCart database!
	MYSQL MySQLVector;
	MYSQL_RES *res;
	unsigned uRet=0;

	mysqlCartDbConnect(&MySQLVector);

	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cEmail='%s'",cEmail);
	mysql_query(&MySQLVector,gcQuery);
	if(mysql_errno(&MySQLVector))
		htmlPlainTextError(mysql_error(&MySQLVector));
	
	res=mysql_store_result(&MySQLVector);

	uRet=(unsigned)mysql_num_rows(res);

	mysql_close(&MySQLVector);

	return(uRet);

}//unsigned uValidUser(char *cEmail)


void mysqlCartDbConnect(MYSQL *MySQLVector)
{
	//This function will connect to the mysqlCart dabatase
	char cmysqlCartDbIp[100]={"localhost"};
	char cmysqlCartDbLogin[100]={"mysqlradius2"};
	char cmysqlCartDbPassword[100]={"wsxedc"};
	char cmysqlCartDbName[100]={"mysqlradius2"};

	GetConfiguration("mysqlCartDbIp",cmysqlCartDbIp);
	GetConfiguration("mysqlCartDbLogin",cmysqlCartDbLogin);
	GetConfiguration("mysqlCartDbPassword",cmysqlCartDbPassword);
	GetConfiguration("mysqlCartDbName",cmysqlCartDbName);

        mysql_init(MySQLVector);
        if (!mysql_real_connect(MySQLVector,cmysqlCartDbIp,cmysqlCartDbLogin,cmysqlCartDbPassword,cmysqlCartDbName,0,NULL,0))
        {
		htmlHeader("mysqlShop.cgi Error","cErrorHeader");
                printf("mysqlCart database <u>unavailable (%s@%s)</u>\n",cmysqlCartDbLogin,cmysqlCartDbIp);
		//fprintf(fp,"mysqlCart database <u>unavailable (%s@%s)</u>\n",cmysqlCartDbLogin,cmysqlCartDbIp);
        	exit(0); //Abort
	}
	//fprintf(fp,"mysqlCartDbConnect() OK\n");

}//void mysqlCartDbConnect(MYSQL *MySQLVector)


void ResetPassword(char *cEmail)
{
	//This function will reset the password of the user pointed by the email address at cEmail
	//For that we need to:
	//1. Get first and last names from mysqlCart db
	//2. Get unxsRadius.tUser.uUser for the login, which is {{cFirstName}}.{{cLastName}}@{{cRadiusDomain}}
	//3. Generate a random password
	//4. Update unxsRadius.tUser record
	//5. Submit a unxsRadius 'Mod' job
	//6. Email the customer with her new login information.
	//
	
	char cFirstName[100]={""};
	char cLastName[100]={""};
	char cRadiusDomain[100]={""};
	char cLogin[512]={""};
	char cPasswd[64]={""};
	char cPwdSave[16]={""};

	unsigned uUser=0;
	
	MYSQL_RES *res;
	MYSQL_ROW field;

	MYSQL mysqlCartDb;
	//1. Get first and last names from mysqlCart db
	mysqlCartDbConnect(&mysqlCartDb);
	sprintf(gcQuery,"SELECT cFirstName,cLastName FROM tClient WHERE cEmail='%s'",cEmail);
	mysql_query(&mysqlCartDb,gcQuery);
	
	if(mysql_errno(&mysqlCartDb))
		htmlPlainTextError(mysql_error(&mysqlCartDb));

	res=mysql_store_result(&mysqlCartDb);
	field=mysql_fetch_row(res); //No if((field because we checked that before with uValidUser()
	
	sprintf(cFirstName,"%.99s",field[0]);
	sprintf(cLastName,"%.99s",field[1]);

	LowerCase(cFirstName);
	LowerCase(cLastName);

	MyGetConfiguration(&mysqlCartDb,"cRadiusDomain",cRadiusDomain);
	
	if(cRadiusDomain[0])
		 sprintf(cLogin,"%s.%s@%s",cFirstName,cLastName,cRadiusDomain);
	else
		sprintf(cLogin,"%s.%s",cFirstName,cLastName);
	
	//2. Get unxsRadius.tUser.uUser for the login
	sprintf(gcQuery,"SELECT uUser FROM tUser WHERE cLogin='%s' AND uOnHold=0",cLogin); //Only active accounts ;)
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	
	if(!mysql_num_rows(res))
	{
		gcMessage="<font color=red>There's no active account associated with the email address you entered. If you think this is an error, please contact support</font>";
		htmlMyAccountPwdOut();
	}

	field=mysql_fetch_row(res); //check above with if(!mysql_num_rows(res))
	sscanf(field[0],"%u",&uUser);

	//3. Generate a random password
	sprintf(cPwdSave,"%.15s",GenerateRandomPassword());
	sprintf(cPasswd,"%s",cPwdSave);
	EncryptPasswd(&gMysql,cPasswd);

	//4. Update unxsRadius.tUser record
	if((Update_tUser(uUser,cPasswd)))
	{
	}
	else
	{
		gcMessage="<font color=red>Update of the unxsRadius.tUser record failed. Please contact support with this message</font>";
		htmlMyAccountPwdOut();
	}

}//void ResetPassword(char *cEmail)


unsigned Update_tUser(unsigned uUser,char *cPasswd)
{
	sprintf(gcQuery,"UPDATE tUser SET cPasswd='%s',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uUser='%u'",cPasswd,uUser);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	return((unsigned)mysql_affected_rows(&gMysql));

}//unsigned Update_tUser(unsigned uUser,char *cPasswd)


char *GenerateRandomPassword(void)
{
	//This function generates a random string containing alphanumeric characters
	//that will be used a the new account password upon reset
	static char cPassword[10]={""};

	to64(&cPassword[0],rand(),6);
	cPassword[6]=0;

	return(cPassword);

}//char *GenerateRandomPassword(void);


static unsigned char itoa64[] =
"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
void to64(register char *s, register long v, register int n)
{
	while (--n >= 0)
	{
		*s++ = itoa64[v&0x3f];
		v >>= 2;
	}
}//void to64


void LowerCase(char *cString)
{
	register int iX;

	for(iX=0;iX<strlen(cString);iX++)
		cString[iX]=tolower(cString[iX]);	

}//void LowerCase(char *cString)


void MyGetConfiguration(MYSQL *MySQLVector,char *cName,char *cValue)
{
	//
	//This function is identical to GetConfiguration() except that it receives
	//a MySQL vector where to run the SELECT query.
	//

	MYSQL_RES *res;
        MYSQL_ROW field;
	
	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='%s'",cName);
	mysql_query(MySQLVector,gcQuery);
	if(mysql_errno(MySQLVector))
		htmlPlainTextError(mysql_error(MySQLVector));
	res=mysql_store_result(MySQLVector);
	if((field=mysql_fetch_row(res)))
		sprintf(cValue,"%s",field[0]);
	mysql_free_result(res);

}//void MyGetConfiguration(MYSQL *MySQLVector,char *cLabel,char cValue,usigned uHTML)


void EncryptPasswd(MYSQL *MySQLVector,char *pw)
{
        char salt[3];
        char passwd[102]={""};
        char *cpw;
        char cMethod[16] ={""};

        MyGetConfiguration(MySQLVector,"cCryptMethod",cMethod);

	(void)srand((int)time((time_t *)NULL));

        if(!strcmp(cMethod,"MD5"))
        {
		char cSalt[] = "$1$01234567$";
		(void)srand((int)time((time_t *)NULL));
		to64(&cSalt[3],rand(),8);

                cpw = crypt(pw,cSalt);
                // error not verified, str NULL ("") returned
        }
        else
        {
                // default DES method
                strcpy(passwd,pw);
                to64(&salt[0],rand(),2);
                cpw=crypt(passwd,salt);
        }
	sprintf(pw,"%s",cpw);

}//void EncryptPasswd(char *pw)

