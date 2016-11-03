/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template and mysqlRAD2 author: (C) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details

 
*/

static char cSearch[100]={""};
void UserReport(char *cLogin, char *cTableName);
void TopUsersReport(char *cTableName);
void Radius2TopUsersReport(void);
void ErrorReport(char *cTableName);
void GlobalUseReport(char *cTableName);
void CiscoWLANFix(void);

void ExtProcesstRadacctVars(pentry entries[], int x)
{

	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.99s",entries[i].val);
		}
	}

}//void ExtProcesstRadacctVars(pentry entries[], int x)


void ExttRadacctCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tRadacctTools"))
	{
		if(!strcmp(gcCommand,"User Report"))
		{
			ProcesstRadacctVars(entries,x);

			if(cLogin[0])
				UserReport(cLogin,"tRadacct");
			else if(cSearch[0])
				UserReport(cSearch,"tRadacct");
			else if(1)
				tRadacct("Must specify cLogin");
		}
		else if(!strcmp(gcCommand,"Top Users Report"))
		{
				TopUsersReport("tRadacct");
		}
		else if(!strcmp(gcCommand,"Top Users by Cisco MAC"))
		{
				Radius2TopUsersReport();
		}
		else if(!strcmp(gcCommand,"Error Report"))
		{
				ErrorReport("tRadacct");
		}
		else if(!strcmp(gcCommand,"Global Use Report"))
		{
				GlobalUseReport("tRadacct");
		}
		else if(!strcmp(gcCommand,"Cisco WLAN Fix"))
		{
				CiscoWLANFix();
		}
	}

}//void ExttRadacctCommands(pentry entries[], int x)


void ExttRadacctButtons(void)
{
	OpenFieldSet("tRadacct Aux Panel",100);

	printf("<u>Table Tips</u><br>");
	printf("tRadacct is fed the current RADIUS accounting data from all configured RADIUS servers in your cluster or simple 2 server setup. The only special fields are uStartTime, uStopTime and cInfo the others come from standard RADIUS attributes from the client NAS.");

	printf("<p><u>Search Tools</u><br>");
	printf("You may use %% and standard SQL _ . wildcards for CLIKE matching of cLogin<br>\n");
	printf("<input title='Enter partial cLogin. You may use %% and _ wildcards.' type=text name=cSearch value=%s>",cSearch);

	printf("<p><u>Report Tools</u><br>");
	printf("<input title='Single user in depth report' class=largeButton type=submit"
			" name=gcCommand value='User Report'><br>");
	printf("<input title='Current month top users' class=largeButton type=submit name=gcCommand"
			" value='Top Users Report'><br>");
	printf("<input title='Current month top Cisco WLAN users via unxsRadius.tUser.cIP' class=largeButton"
			" type=submit name=gcCommand value='Top Users by Cisco MAC'><br>");
	printf("<input title='Current month error records' class=largeButton type=submit name=gcCommand"
			" value='Error Report'><br>");
	printf("<input title='Current month global use report' class=largeButton type=submit name=gcCommand"
			" value='Global Use Report'><br>");
	printf("<input title='Fix CISCO WLAN missing cLogin' class=lwarnButton type=submit name=gcCommand"
			" value='Cisco WLAN Fix'><br>");

	CloseFieldSet();

}//void ExttRadacctButtons(void)


void ExttRadacctAuxTable(void)
{

}//void ExttRadacctAuxTable(void)


void ExttRadacctGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uRadacct"))
		{
			sscanf(gentries[i].val,"%u",&uRadacct);
			guMode=6;
		}
	}
	tRadacct("");

}//void ExttRadacctGetHook(entry gentries[], int x)


void ExttRadacctSelect(void)
{
	if(cSearch[0])
		sprintf(gcQuery,"SELECT %s FROM tRadacct WHERE cLogin LIKE '%s%%' ORDER BY cLogin,uStopTime ",VAR_LIST_tRadacct,cSearch);
	else
		sprintf(gcQuery,"SELECT %s FROM tRadacct ORDER BY uStopTime",VAR_LIST_tRadacct);

}//void ExttRadacctSelect(void)


void ExttRadacctSelectRow(void)
{
	sprintf(gcQuery,"SELECT %s FROM tRadacct WHERE uRadacct=%u",
			VAR_LIST_tRadacct,uRadacct);

}//void ExttRadacctSelectRow(void)


void ExttRadacctListSelect(void)
{
	char cCat[512];

	sprintf(gcQuery,"SELECT %s FROM tRadacct",VAR_LIST_tRadacct);

	//Changes here must be reflected below in ExttRadacctListFilter()
        if(!strcmp(gcFilter,"cLogin"))
        {
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tRadacct.cLogin LIKE '%s%%' ORDER BY cLogin",
						gcCommand);
		strcat(gcQuery,cCat);
        }
	else if(!strcmp(gcFilter,"cCallerID"))
        {
                strcat(gcQuery," WHERE ");
                sprintf(cCat,"tRadacct.cCallerID LIKE '%s%%' ORDER BY cCallerID",
                                                gcCommand);
                strcat(gcQuery,cCat);
        }
	else if(!strcmp(gcFilter,"cIP"))
	{
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tRadacct.cIP LIKE '%s%%' ORDER BY cCallerID",
						gcCommand);
		strcat(gcQuery,cCat);
	}
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uRadacct");
        }

}//void ExttRadacctListSelect(void)


void ExttRadacctListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");

        if(strcmp(gcFilter,"cLogin"))
                printf("<option>cLogin</option>");
        else
                printf("<option selected>cLogin</option>");

	if(strcmp(gcFilter,"cCallerID"))
                printf("<option>cCallerID</option>");
        else
                printf("<option selected>cCallerID</option>");
	
	if(strcmp(gcFilter,"cIP"))
		printf("<option>cIP</option>");
	else
		printf("<option selected></option>");

        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");

        printf("</select>");

}//void ExttRadacctListFilter(void)


void ExttRadacctNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttRadacctNavBar(void)


void UserReport(char *cLogin, char *cTableName)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	time_t uStartTime=0,uStopTime=0,uConnectTime=0;
	unsigned uFound=0;

	printf("Content-type: text/plain\n\n");

        sprintf(gcQuery,"SELECT COUNT(*),SUM(uConnectTime),SUM(uInBytes),SUM(uOutbytes),MIN(uStartTime),MAX(uStopTime)"
			" FROM %s WHERE uConnectTime>0 AND cLogin='%s' GROUP BY cLogin",cTableName,cLogin);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Table: %s\n\n",cTableName);

        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
        {

		sscanf(field[4],"%lu",&uStartTime);
		sscanf(field[5],"%lu",&uStopTime);
		sscanf(field[1],"%lu",&uConnectTime);

		printf("cLogin User:\t\t%s\n",cLogin);
		printf("Number of sessions:\t%s\n",field[0]);
		//printf("Total connect time (s):\t%s\n",field[1]);
		printf("Total connect time (Hr):\t%2.2f\n",(float)(uConnectTime/3600.00));
		if(uStartTime==0 || uStartTime== -1 )
			printf("Time of first session:\t%s\n",ctime(&uStopTime));
		else
			printf("Time of first session:\t%s\n",ctime(&uStartTime));
		printf("Time of last session:\t%s\n",ctime(&uStopTime));
		printf("Total bytes downloaded:\t%s\n",field[3]);
		printf("Total bytes uploaded:\t%s\n",field[2]);
		
		uFound++;
        }
        mysql_free_result(res);
	
	if(!uFound)
	{
                printf("No records found for %s\n",cLogin);
		exit(0);
	}

        sprintf(gcQuery,"SELECT uConnectTime,uInBytes,uOutbytes,uStartTime,uStopTime,cCallerID"
			" FROM %s WHERE uConnectTime>0 AND cLogin='%s' ORDER BY uStopTime",cTableName,cLogin);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

        res=mysql_store_result(&gMysql);
	printf("\n---\nSession Records:\nStart\nEnd\nConnect Time, (In/Out), CallerID\n\n");
        while((field=mysql_fetch_row(res)))
        {
		sscanf(field[3],"%lu",&uStartTime);
		sscanf(field[4],"%lu",&uStopTime);

		printf("%s",ctime(&uStartTime));
		printf("%s",ctime(&uStopTime));
		printf("%s",field[0]);
		printf(" (%s/%s),",field[1],field[2]);
		printf(" %s\n\n",field[5]);
        }
        mysql_free_result(res);
	
	exit(0);

}//void UserReport(char *cLogin, char *cTableName)


void TopUsersReport(char *cTableName)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uLen;
	time_t uConnectTime=0;
	float fConnectTime=0.0;

	printf("Content-type: text/plain\n\n");

        sprintf(gcQuery,"SELECT cLogin,SUM(uConnectTime) AS uConnectTime FROM %s WHERE uConnectTime>0 GROUP BY"
			" cLogin ORDER BY uConnectTime DESC LIMIT 50"
				,cTableName);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Table: %s\n\n",cTableName);
	printf("Top 50 by uConnectTime (Hr)\n\n");
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
		uLen=strlen(field[0]);

		sscanf(field[1],"%lu",&uConnectTime);
		fConnectTime=(float)(uConnectTime/3600.00);

		if(uLen<8)
		printf("%s\t\t\t%2.2f\n",field[0],fConnectTime);
		else if(uLen<16)
		printf("%s\t\t%2.2f\n",field[0],fConnectTime);
		else if(1)
		printf("%s\t%2.2f\n",field[0],fConnectTime);
	}
        mysql_free_result(res);
	

        sprintf(gcQuery,"SELECT cLogin,SUM(uOutBytes) AS uOutBytes FROM %s WHERE uConnectTime>0 GROUP BY"
			" cLogin ORDER BY uOutBytes DESC LIMIT 50",
				cTableName);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("\nTop 50 by bytes downloaded (uOutBytes)\n\n");
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
		uLen=strlen(field[0]);

		if(uLen<8)
		printf("%s\t\t\t%s\n",field[0],field[1]);
		else if(uLen<16)
		printf("%s\t\t%s\n",field[0],field[1]);
		else if(1)
		printf("%s\t%s\n",field[0],field[1]);
	}
        mysql_free_result(res);
	
	exit(0);


}//void TopUsersReport()


void ErrorReport(char *cTableName)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("Content-type: text/plain\n\n");

        sprintf(gcQuery,"SELECT cLogin,uRadacct,uStartTime,uStopTime FROM %s WHERE uStopTime=0",
			cTableName);


        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Table: %s\n\n",cTableName);
	printf("Records with error conditions\n");
	printf("cLogin uRadacct uStartTime uStopTime\n\n");
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
		printf("%s\t%s\t%s\t%s\n",field[0],field[1],field[2],field[3]);
	}
        mysql_free_result(res);
	
	exit(0);

}//void ErrorReport()


void GlobalUseReport(char *cTableName)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	time_t luUsers=0;

	printf("Content-type: text/plain\n\n");

	printf("Table: %s\n\n",cTableName);
	printf("Global Use Report:\nPart 1, total connect time in hours\n");
        sprintf(gcQuery,"SELECT (SUM(uConnectTime)/3600.00) FROM %s WHERE uStopTime!=0",cTableName);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
        {
		printf("%s hrs.\n",field[0]);
	}
        mysql_free_result(res);

	printf("Part 2, total number of distinct users\n");
        sprintf(gcQuery,"SELECT DISTINCT cLogin FROM %s",cTableName);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
	luUsers=mysql_num_rows(res);
	printf("%lu users.\n",luUsers);
        mysql_free_result(res);
	
	exit(0);

}//void GlobalUseReport()


void CiscoWLANFix(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char *cp;
	char *cp2;
	char cMAC[32];
	char cLogin[32];
	char cPrevSessionID[64]={"$"};
	unsigned uPrevRadacct;
	register int i,j;

	printf("Content-type: text/plain\n\n");
	printf("CiscoWLANFix() start\n");

	printf("Part 1-. Set missing cLogins\n");
        sprintf(gcQuery,"SELECT cSessionID,uRadacct FROM tRadacct WHERE uStopTime!=0 AND cLogin=''");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
		//extract cLogin
		//49e5ccfd/00:23:6c:7f:fa:35/220
		if((cp=strchr(field[0],'/')))
		{
			if((cp2=strchr(cp+1,'/')))
			{
				cLogin[0]=0;
				cMAC[0]=0;
				*cp2=0;
				sprintf(cMAC,"%s",cp+1);
				//remove colons
				for(i=0,j=0;cMAC[i];i++)
				{
					if(cMAC[i]!=':')
						cLogin[j++]=cMAC[i];
				}
				cLogin[j]=0;
				printf("%s\n",cLogin);
        			sprintf(gcQuery,"UPDATE tRadacct SET cLogin='%s' WHERE uRadacct=%s",
						cLogin,field[1]);
			        mysql_query(&gMysql,gcQuery);
			        if(mysql_errno(&gMysql))
				{
			                printf("%s\n",mysql_error(&gMysql));
					exit(1);
				}
				
			}
		}
	}
        mysql_free_result(res);

	printf("Part 2-. Group by cSessionID\n");
        sprintf(gcQuery,"SELECT uRadacct,cSessionID,uConnectTime,cCallerID,uInBytes,uOutBytes,uStartTime,uStopTime"
				" FROM tRadacct ORDER BY cSessionID,uRadacct");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
		if(strcmp(cPrevSessionID,field[1]))
		{
			sprintf(cPrevSessionID,"%.63s",field[1]);
			sscanf(field[0],"%u",&uPrevRadacct);
		}
		else
		{
			printf("Matching cSessionIDs\n");
			printf("%u %s\n",uPrevRadacct,cPrevSessionID);
			printf("%s %s uConnectTime=%s cCallerID=%s uInBytes=%s uOutBytes=%s uStartTime=%s uStopTime=%s\n",
					field[0],field[1],field[2],field[3],
					field[4],field[5],field[6],field[7]);

			//uStartTime=0 is a safety precaution since all Cisco records have this format we assume.
			//update first uRadacct with data from second
        		sprintf(gcQuery,"UPDATE tRadacct SET uConnectTime=%s,cCallerID='%s',uInBytes=%s,uOutBytes=%s,"
					"uStopTime=%s WHERE uRadacct=%u AND uStartTime>0",
						field[2],field[3],field[4],field[5],field[7],uPrevRadacct);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
				
			//delete second uRadacct
        		sprintf(gcQuery,"DELETE FROM tRadacct WHERE uRadacct=%s AND uStartTime=0",field[0]);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(1);
			}
		}
	}
        mysql_free_result(res);

	printf("CiscoWLANFix() end\n");
	
	exit(0);

}//void CiscoWLANFix(void)


void Radius2TopUsersReport(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uLen;
	time_t uConnectTime=0;
	float fConnectTime=0.0;

	printf("Content-type: text/plain\n\n");

        sprintf(gcQuery,"SELECT tUser.cLogin,SUM(tRadacct.uConnectTime) AS uSumConnectTime FROM"
				" mysqlradacct2.tRadacct,mysqlradius2.tUser WHERE"
				" tRadacct.cLogin=REPLACE(tUser.cIP,':','') AND tRadacct.uConnectTime>0"
				" GROUP BY tRadacct.cLogin ORDER BY uSumConnectTime DESC LIMIT 50");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                printf("(Make sure mysqlradacct2 can select from mysqlradius2) %s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("Top 50 by uConnectTime (Hr)\n\n");
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
		uLen=strlen(field[0]);

		sscanf(field[1],"%lu",&uConnectTime);
		fConnectTime=(float)(uConnectTime/3600.00);

		if(uLen<8)
		printf("%s\t\t\t%2.2f\n",field[0],fConnectTime);
		else if(uLen<16)
		printf("%s\t\t%2.2f\n",field[0],fConnectTime);
		else if(1)
		printf("%s\t%2.2f\n",field[0],fConnectTime);
	}
        mysql_free_result(res);
	

        sprintf(gcQuery,"SELECT tUser.cLogin,SUM(tRadacct.uOutBytes) AS uSumOutBytes FROM"
				" mysqlradacct2.tRadacct,mysqlradius2.tUser WHERE"
				" tRadacct.cLogin=REPLACE(tUser.cIP,':','') AND tRadacct.uConnectTime>0"
				" GROUP BY tRadacct.cLogin ORDER BY uSumOutBytes DESC LIMIT 50;");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                printf("%s\n",mysql_error(&gMysql));
		exit(1);
	}

	printf("\nTop 50 by bytes downloaded (uOutBytes)\n\n");
        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
		uLen=strlen(field[0]);

		if(uLen<8)
		printf("%s\t\t\t%s\n",field[0],field[1]);
		else if(uLen<16)
		printf("%s\t\t%s\n",field[0],field[1]);
		else if(1)
		printf("%s\t%s\n",field[0],field[1]);
	}
        mysql_free_result(res);
	
	exit(0);


}//void TopUsersReport()


