/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template and mysqlRAD2 author: (C) 2001-2009 Gary Wallis.
	GPL License applies, see www.fsf.org for details

 
*/

#include <ctype.h> //isalnum() decl

static unsigned uServer=0;
static char cuServerPullDown[256]={""};
static char cSearch[100]={""};

void tUserNavList(void);
void ServerList(void);
void tProfileNavList(unsigned uUser, unsigned uServer);
unsigned GetuServer(char *cLabel);
void PassFile(char *file);

int SubmitJob(const char *cJobName,char *cJobData,unsigned uServerGroup,unsigned uJobDate);
int SubmitSingleJob(const char *cJobName,char *cJobData,const char *cServer,unsigned uJobDate);
#ifndef DEBUG_REPORT_STATS_OFF
	int UpdateInfo();
#endif

void BasictUserCheck(unsigned uMod);
static void htmlRecordContext(void);

//Extern
int MakeUsersFile(unsigned uHtml, unsigned uServer, char *cLogin);
void EncryptPasswd(char *pw);
void GetConfiguration(const char *cLabel,char *cValue,unsigned uHtml);
int ReloadRadius(unsigned uHtml,unsigned uServer,unsigned uStopRequired);

void ExtProcesstUserVars(pentry entries[], int x)
{

	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cuServerPullDown"))
		{
			strcpy(cuServerPullDown,entries[i].val);
			uServer=ReadPullDownOwner("tServer","cLabel",cuServerPullDown,guCompany);
		}
		else if(!strcmp(entries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.99s",entries[i].val);
		}
	}

}//void ExtProcesstUserVars(pentry entries[], int x)


void ExttUserCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tUserTools"))
	{
		//ModuleFunctionProcess()

		//Default wizard like two step creation and deletion
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			ProcesstUserVars(entries,x);
			if(guPermLevel>=10)
			{
				//Prep form for end authorized users
				uUser=0;
				uModBy=0;
				uCreatedBy=0;
				uCreatedDate=0;
				uOwner=0;
				uModDate=0;
				cEnterPasswd[0]=0;
				cPasswd[0]=0;
				cLogin[0]=0;
				cIP[0]=0;
				//uSimulUse=1; default already in tuser.c
				//uProfileName=0;
				//Check global conditions for new record here
                        	guMode=2000;
				tUser(LANG_NB_CONFIRMNEW);
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");  
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			ProcesstUserVars(entries,x);
			if(guPermLevel>=7)
			{
				guMode=2000;
				uUser=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;
				uModDate=0;
				BasictUserCheck(0);
				guMode=5;
				strcpy(cPasswd,cEnterPasswd);
				EncryptPasswd(cPasswd);
				if(!uClearText)
					cEnterPasswd[0]=0;
				NewtUser(1);
				sprintf(gcQuery,"INSERT INTO tServerGroup SET uUser=%u,"
							"uServer=%u",uUser,uServer);
				macro_mySQLQueryHTMLError;

				SubmitJob("NewUser",cLogin,uUser,0);
#ifndef DEBUG_REPORT_STATS_OFF
				UpdateInfo();
#endif
				tUser("New user added");
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");  
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstUserVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))  
			{
	                        guMode=2001;
        	                tUser(LANG_NB_CONFIRMDEL);
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");  
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstUserVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))  
			{
                	        guMode=5;
				SubmitJob("DelUser",cLogin,uUser,0);
				sprintf(gcQuery,"DELETE FROM tServerGroup WHERE uUser=%u",uUser);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
                       		DeletetUser();
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");  
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstUserVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
	                        guMode=2002;
       		                tUser(LANG_NB_CONFIRMMOD);
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");  
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstUserVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
	                        guMode=2002;
				BasictUserCheck(1);
				
				if(cEnterPasswd[0])
				{
					strcpy(cPasswd,cEnterPasswd);
					EncryptPasswd(cPasswd);
					if(!uClearText)
						cEnterPasswd[0]=0;
				}
       		                guMode=0;
				uModBy=guLoginClient;
				SubmitJob("ModUser",cLogin,uUser,0);
       		                ModtUser();
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");  
                }

		//localhost only
                else if(!strcmp(gcCommand,"Reload Radius Files"))
                {
			ProcesstUserVars(entries,x);
			if(guPermLevel>=12)
			{
				//
				//Dev note: Probably it will be much wiser to create a 'Reload' job
				//and submit it, thus you could reload all the server cluster..
				//Or, use the server dropdown to select which server to reload
				//instead of calling the function below.

				ReloadRadius(1,1,0);
				tUser("local radiusd files reloaded");
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");  
		}
                else if(!strcmp(gcCommand,"View Local Users"))
                {
			if(guPermLevel>=12)
			{
				char cRaddbDir[256]={"/usr/local/etc/raddb"};

				GetConfiguration("cRaddbDir",cRaddbDir,1);

				sprintf(gcQuery,"%.255s/users",cRaddbDir);
				PassFile(gcQuery);
				exit(0);
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");  
		}
                else if(!strcmp(gcCommand,"Make Local Users"))
                {
			if(guPermLevel>=12)
			{
				MYSQL_RES *res;
				MYSQL_ROW field;
				unsigned uServer;

				sprintf(gcQuery,"SELECT uServer,cLabel FROM tServer LIMIT 1");
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
                			tUser((char *)mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
        			if((field=mysql_fetch_row(res)))
        			{
					sscanf(field[0],"%u",&uServer);
					MakeUsersFile(1,uServer,"");
					sprintf(gcQuery,"local raddb/users file made for %s",
							field[1]);
					tUser(gcQuery);
				}
				else
				{
					mysql_free_result(res);
					tUser("tServer empty?");
				}
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");  
		}
		else if(!strcmp(gcCommand,"Add Server"))
		{
                        ProcesstUserVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
				time_t clock;
				MYSQL_RES *res;

				time(&clock);
				if(!uUser || !uServer)
					tUser("Must select valid user and server");
				sprintf(gcQuery,"SELECT uServer FROM tServerGroup WHERE uUser=%u AND uServer=%u",
						uUser,uServer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0) tUser("");
				mysql_free_result(res);

				sprintf(gcQuery,"INSERT INTO tServerGroup SET uUser=%u,uServer=%u",uUser,uServer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));

				sprintf(gcQuery,"UPDATE tUser SET uModBy=%u,uModDate=%lu WHERE uUser=%u",
					guLoginClient,clock,uUser);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));

				SubmitSingleJob("NewUser",cLogin,
						(char *)ForeignKey("tServer","cLabel",uServer),0);
				tUser("Server added");
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");  

		}
		else if(!strcmp(gcCommand,"Remove Server"))
		{
                        ProcesstUserVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
				if(!uUser || !uServer)
					tUser("Must select valid user and server");
				sprintf(gcQuery,"DELETE FROM tServerGroup WHERE uUser=%u AND uServer=%u",uUser,uServer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
				SubmitSingleJob("DelUser",cLogin,
						(char *)ForeignKey("tServer","cLabel",uServer),0);
				tUser("Server removed");
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");  
		}
	}

}//void ExttUserCommands(pentry entries[], int x)


void ExttUserButtons(void)
{
        OpenFieldSet("tUser Aux Panel",100);
	

	switch(guMode)
        {
                case 2000:
			printf("<u>Enter required data</u><p>");
			printf("Please select one of your RADIUS servers<br>\n");
			tTablePullDownOwner("tServer;cuServerPullDown","cLabel","cLabel",uServer,1);
			printf("<p>\n");
                        printf(LANG_NBB_CONFIRMNEW);
			printf("<br>\n");
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
			printf("<br>\n");
                break;

                case 2002:
			printf("<br>Review record data<br>");
                        printf(LANG_NBB_CONFIRMMOD);
			printf("<br>\n");
                break;

		default:
				printf("<u>Table Tips</u><br>");
				printf("This is the RADIUS user table. cLogin is the user name string."
					" cEnterPasswd is used to enter a new or modified password cleartext (CHAP)"
					" or otherwise. cPasswd is then set depending on tConfiguration crypt type."
					" The profile name is fundamental and both PAP and CHAP profiles should be"
					" available for the profile name (see tProfile and tProfileName.)"
					" uOnHold can be used to remove a user from the active user file without"
					" deleteing the record. And uSimulUse creates a RADIUS input attribute"
					" that is widely used to control multiple logins."
					" <p>Please note that you will only be able to view your companies users"
					" and assign profile names and servers that your company has permission"
					" to use. The uClient field is for on optional master ISP use.");

				if(uUser)
				{
					htmlRecordContext();

		        		printf("<p><u>User Server Assignment</u><br>\n");
					ServerList();
					tTablePullDownOwner("tServer;cuServerPullDown","cLabel","cLabel",uServer,1);
					printf("<br><input title='Add above server for service for this user'"
						" class=largeButton type=submit name=gcCommand value='Add Server'><br>\n");
					printf("<input title='Remove above server for service for this user' "
						"class=largeButton type=submit name=gcCommand value='Remove Server'>\n");
				}

				printf("<p><u>Search Tools</u><br>");
        			printf("<input title='Enter cLogin search pattern. Can be partial."
					" You can also use advanced"
					" _ and %% pattern matching' type=text name=cSearch value='%s'> cLogin<br>\n",
							cSearch);
				if(guPermLevel>11)
				{
        				printf("<p><u>Local Server Management</u><br>\n");
					printf("<input title='Make raddb/users file on this server. Note:"
						" For first uServer in tServer table, this may not be what you want"
						" check tServer!.' class=largeButton type=submit name=gcCommand"
						" value='Make Local Users'><br>\n");
					printf("<input title='kill -hup radiusd pid: Reloads users/configuration'"
						" class=largeButton type=submit name=gcCommand "
						"value='Reload Radius Files'><br>\n");
					printf("<input title='View local raddb/users file' class=largeButton"
						" type=submit name=gcCommand value='View Local Users'><br>\n");
				}
		break;
	}



	CloseFieldSet();
	
}//void ExttUserButtons(void)


void ExttUserAuxTable(void)
{

}//void ExttUserAuxTable(void)


void ExttUserGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uUser"))
		{
			sscanf(gentries[i].val,"%u",&uUser);
			guMode=6;
		}
	}
	tUser("");

}//void ExttUserGetHook(entry gentries[], int x)


void ExttUserSelect(void)
{
	if(cSearch[0])
		ExtSelectSearch("tUser",VAR_LIST_tUser,"cLogin",cSearch,NULL,0);
	else
		ExtSelect("tUser",VAR_LIST_tUser,0);

}//void ExttUserSelect(void)


void ExttUserSelectRow(void)
{
	ExtSelectRow("tUser",VAR_LIST_tUser,uUser);

}//void ExttUserSelectRow(void)


void ExttUserListSelect(void)
{
	char cCat[512];

	ExtListSelect("tUser",VAR_LIST_tUser);
	
	//Changes here must be reflected below in ExttUserListFilter()
        if(!strcmp(gcFilter,"uUser"))
        {
                sscanf(gcCommand,"%u",&uUser);
		if(guPermLevel<12)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tUser.uUser=%u \
						ORDER BY uUser",
						uUser);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cLogin"))
        {
		if(guPermLevel<12)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tUser.cLogin LIKE '%s%%' ORDER BY tUser.cLogin",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uUser");
        }

}//void ExttUserListSelect(void)


void ExttUserListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uUser"))
                printf("<option>uUser</option>");
        else
                printf("<option selected>uUser</option>");
       if(strcmp(gcFilter,"cLogin"))
       		 printf("<option>cLogin</option>");
       else
		printf("<option selected>cLogin</option>");
	if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttUserListFilter(void)


void ExttUserNavBar(void)
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

}//void ExttUserNavBar(void)


void tUserNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(cSearch[0])
		ExtSelectSearch("tUser","uUser,cLogin","cLogin",cSearch,NULL,20);
	else
		ExtSelect("tUser","uUser,cLogin",20);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
                printf("<a class=darkLink href=unxsRadius.cgi?gcFunction=tUser"
			"&uUser=%s>%s</a><br>\n",field[0],field[1]);
        mysql_free_result(res);

}//void tUserNavList(void)


void ServerList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        sprintf(gcQuery,"SELECT tServer.uServer,tServer.cLabel FROM tServer,tServerGroup "
			"WHERE tServer.uServer=tServerGroup.uServer AND tServerGroup.uUser=%u",uUser);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
                printf("<a class=darkLink href=unxsRadius.cgi?gcFunction=tServer\
&uServer=%s>%s</a><br>\n",field[0],field[1]);
        }
        mysql_free_result(res);

}//void ServerList(void)


unsigned GetuServer(char *cLabel)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uRetVal=0;

        sprintf(gcQuery,"SELECT uServer FROM tServer WHERE cLabel='%s'",cLabel);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
		sscanf(field[0],"%u",&uRetVal);
        }
        mysql_free_result(res);
	
	return(uRetVal);

}//unsigned GetuServer(char *cLabel)


void PassFile(char *file)
{
	FILE *fp;
        char buffer[1024];
 
	printf("Content-type: text/plain\n\n");
        if((fp=fopen(file,"r"))!=NULL)
        {
		
                while(fgets(buffer,1024,fp)!=NULL)
                {
                        printf("%s",buffer);
                }

                fclose(fp);
        }
	else
	{
		printf("Could not open %s",file);
	}

}//void PassFile(char *file)


//Submit one job for each server
int SubmitJob(const char *cJobName,char *cJobData,unsigned uServerGroup,unsigned uJobDate)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tServer.cLabel FROM tServer,tServerGroup WHERE"
			" tServer.uServer=tServerGroup.uServer AND tServerGroup.uUser=%u",uServerGroup);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		SubmitSingleJob(cJobName,cJobData,field[0],uJobDate);
	mysql_free_result(res);

	return(0);

}//int SubmitJob()


//One job not similar and one for provided server
int SubmitSingleJob(const char *cJobName,char *cJobData,const char *cServer,unsigned uJobDate)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE cJobName='%s' AND cJobData='%s' AND cServer='%s'",
					cJobName,cJobData,cServer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	
	if(mysql_num_rows(res)==0)
	{
		if(uJobDate==0)
			sprintf(gcQuery,"INSERT INTO tJob SET cServer='%s',cJobName='%s',cJobData='%.1024s',"
				"uJobDate=UNIX_TIMESTAMP(NOW()),uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW()),cLabel='%.32s',"
				"uJobStatus=1",
			cServer
			,cJobName
			,TextAreaSave(cJobData)
			,guCompany
			,guLoginClient,cJobData);
		else
			sprintf(gcQuery,"INSERT INTO tJob SET cServer='%s',cJobName='%s',cJobData='%.1024s',"
				"uJobDate=%u,uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW()),cLabel='%.32s',"
				"uJobStatus=1",
			cServer
			,cJobName
			,TextAreaSave(cJobData)
			,uJobDate
			,guCompany
			,guLoginClient,cJobData);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

	return(0);

}//int SubmitSingleJob()


#ifndef DEBUG_REPORT_STATS_OFF

int UpdateInfo(void)
{
	register int sd, rc;
	struct sockaddr_in cliAddr, remoteServAddr;
	struct hostent *h;
	time_t luClock=0,luModDate=0;

	MYSQL_RES *res;
	MYSQL_ROW field;

	char cInfo[128]={"Emtpy"};
	unsigned uMaxuUser=0;

	time(&luClock);

	sprintf(gcQuery,"SELECT uModDate FROM tConfiguration WHERE cLabel='UpdateInfo'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return(1);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%lu",&luModDate);
	mysql_free_result(res);

	if(luModDate && ( luClock < (luModDate + 86400) ) )
		return(2);

	sprintf(gcQuery,"SELECT MAX(uUser) FROM tUser");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return(3);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uMaxuUser);
	mysql_free_result(res);

	sprintf(cInfo,"mysqlRadius 2.0 %u %u %u",guLoginClient,guPermLevel,uMaxuUser);

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
		sprintf(gcQuery,"UPDATE tConfiguration SET uModBy=1,uModDate=%lu,cComment='%s' WHERE cLabel='UpdateInfo'",luClock,cInfo);
	else
		sprintf(gcQuery,"INSERT INTO tConfiguration SET cLabel='UpdateInfo',cValue='uModDate',cComment='%s',"
				"uCreatedBy=1,uCreatedDate=%lu,uModDate=%lu,uModBy=1,uOwner=1,uServer=1",
					cInfo,luClock,luClock);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlPlainTextError(mysql_error(&gMysql));
		return(9);
	}
  
	return(0);

}//int UpdateInfo(void)

#endif


void BasictUserCheck(unsigned uMod)
{
	//
	//Set guMode in case we find and issue
	if(uMod)
		guMode=2002;
	else
		guMode=2000;

	if(!cLogin[0])
		tUser("<blink>Error:</blink> cLogin is required");
	else
	{
		register int i;	
		if((strlen(cLogin)<4))
			tUser("<blink>Error:</blink> cLogin is too short");
		for(i=0;i<strlen(cLogin);i++)
		{
			if(!isalnum(cLogin[i]) && cLogin[i]!='.')
				tUser("<blink>Error:</blink> cLogin contains invalid chars");
		}
	}

	if(!uMod)
	{
		//
		//Unique tUser.cLogin check
		MYSQL_RES *res;

		sprintf(gcQuery,"SELECT uUser FROM tUser WHERE cLogin='%s'",TextAreaSave(cLogin));
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		res=mysql_store_result(&gMysql);

		if(mysql_num_rows(res))
			tUser("<blink>Error:</blink> cLogin is already used");
	}
	else
	{
		//
		//Mod tUser.cLogin check
		char cOldLogin[100]={""};

		sprintf(cOldLogin,"%.99s",ForeignKey("tUser","cLabel",uUser));
		if(strcmp(cOldLogin,cLogin))
			tUser("<blink>Error:</blink> You can't modify cLogin value");
	}
		
	if(!cEnterPasswd[0] && (!uMod || uClearText))
		tUser("<blink>Error:</blink> cEnterPasswd is required");
	else
	{
		if((strlen(cEnterPasswd))<5)
			tUser("<blink>Error:</blink> cEnterPasswd is too short");
	}

	if(!uProfileName)
		tUser("<blink>Error:</blink> Must select uProfileName from the drop-down");
	
	if(!uMod && !uServer)
		 tUser("<blink>Error:</blink> Must select valid initial server");

	if(!uSimulUse)
		tUser("<blink>Error:</blink> Minimal value for uSimulUse is 1");

	if(!cIP[0])
	{
		sprintf(cIP,"0.0.0.0");
		tUser("<blink>Warning:</blink> cIP was updated, check/fix");
	}
	else
	{
		unsigned a,b,c,d;
		char cNewIP[64]={""};

		sscanf(cIP,"%u.%u.%u.%u",&a,&b,&c,&d);
		if(a>254) a=0;
		if(b>254) b=0;
		if(c>254) c=0;  
		if(d>254) d=0;

		sprintf(cNewIP,"%u.%u.%u.%u",a,b,c,d);
		if(strcmp(cNewIP,cIP))
		{
			sprintf(cIP,"%s",cNewIP);
			tUser("<blink>Warning:</blink> cIP was updated, check/fix");
		}
	}
	
}//void BasictUserCheck()


void htmlRecordContext(void)
{
	printf("<p><u>Record Context Info</u><br>");
	printf("This user will require a ");
	if(uClearText)
		printf("CHAP");
	else
		printf("PAP");
	printf(" profile to exist for the profile name assigned.");

}//void htmlRecordContext(void)


