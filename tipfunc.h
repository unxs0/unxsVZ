/*
FILE
	$Id$
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR/LEGAL
	(C) 2001-2010 Gary Wallis.
	GPLv2 license applies. See LICENSE file included.
*/

#include <openisp/ucidr.h>

void tIPReport(void);
void tIPNavList(unsigned uAvailable);
void tIPUsedButAvailableNavList(void);
void tIPUsedButAvailableFix(void);

//uForClient: Create for, on 'New;
static unsigned uForClient=0;
static char cForClientPullDown[256]={""};
static char cIPRange[32]={""};
static char cSearch[32]={""};

void AddIPRange(char *cIPRange);
void DelIPRange(char *cIPRange);


void ExtProcesstIPVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cIPRange"))
			sprintf(cIPRange,"%.31s",entries[i].val);
		else if(!strcmp(entries[i].name,"cForClientPullDown"))
		{
			sprintf(cForClientPullDown,"%.255s",entries[i].val);
			uForClient=ReadPullDown("tClient","cLabel",cForClientPullDown);
		}
		else if(!strcmp(entries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.31s",TextAreaSave(entries[i].val));
		}
	}
}//void ExtProcesstIPVars(pentry entries[], int x)


void ExttIPCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tIPTools"))
	{
        	MYSQL_RES *res;

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstIPVars(entries,x);
                        	guMode=2000;
	                        tIP(LANG_NB_CONFIRMNEW);
			}
			else
			{
				tIP("Not allowed by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstIPVars(entries,x);

                        	guMode=2000;
				//Check entries here
				//1.2.3.4 min
				if(strlen(cLabel)<7)
					tIP("cLabel too short");
				if(!uDatacenter)
					tIP("Must specify a uDatacenter");
                        	guMode=0;

				uIP=0;
				uCreatedBy=guLoginClient;
				if(uForClient) guCompany=uForClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtIP(0);
			}
			else
			{
				tIP("Not allowed by permissions settings");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstIPVars(entries,x);
			if((uAllowDel(uOwner,uCreatedBy) && uAvailable) || guPermLevel>=11)
			{
	                        guMode=0;
				sprintf(gcQuery,"SELECT uIPv4 FROM tContainer WHERE uIPv4=%u",uIP);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tIP("Can't delete an IP in use");
				}
	                        guMode=2001;
				tIP(LANG_NB_CONFIRMDEL);
			}
			else
			{
				tIP("Not allowed by permissions settings");
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstIPVars(entries,x);
			if((uAllowDel(uOwner,uCreatedBy) && uAvailable) || guPermLevel>=11)
			{
				guMode=5;
				if(cIPRange[0])
				{
					if(uForClient) guCompany=uForClient;
					DelIPRange(cIPRange);
				}
				else
				{
					sprintf(gcQuery,"SELECT uIPv4 FROM tContainer WHERE uIPv4=%u",uIP);
        				mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        				res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res))
					{
						mysql_free_result(res);
	                        		guMode=2001;
						tIP("Can't delete an IP in use");
					}
					DeletetIP();
				}
			}
			else
			{
				tIP("Not allowed by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstIPVars(entries,x);
			if((uAllowMod(uOwner,uCreatedBy) && uAvailable) || guPermLevel>=11)
			{
	                        guMode=0;
				sprintf(gcQuery,"SELECT uIPv4 FROM tContainer WHERE uIPv4=%u",uIP);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tIP("Can't modify an IP in use");
				}
	                        guMode=2002;
				tIP(LANG_NB_CONFIRMMOD);
			}
			else
			{
				tIP("Not allowed by permissions settings");
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstIPVars(entries,x);
			if((uAllowMod(uOwner,uCreatedBy)) || guPermLevel>=11)
			{
                        	guMode=2002;
				sprintf(gcQuery,"SELECT uIPv4 FROM tContainer WHERE uIPv4=%u",uIP);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tIP("Can't modify an IP in use");
				}
				if(strlen(cLabel)<7)
					tIP("cLabel too short");
				if(!uDatacenter)
					tIP("Must specify a uDatacenter");
                        	guMode=0;

				if(uForClient)
				{
					guCompany=uForClient;
					if(!cIPRange[0])
					{
						uOwner=guCompany;
						sprintf(gcQuery,"UPDATE tIP SET uOwner=%u WHERE uAvailable=1 AND uIP=%u",
								guCompany,uIP);
        					mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					}
				}
				if(cIPRange[0])
					AddIPRange(cIPRange);
				uModBy=guLoginClient;
				ModtIP();
			}
			else
			{
				tIP("Not allowed by permissions settings");
			}
                }
                else if(!strcmp(gcCommand,"Fix"))
                {
                        ProcesstIPVars(entries,x);
			if((uAllowMod(uOwner,uCreatedBy)) || guPermLevel>=11)
			{
				tIPUsedButAvailableFix();
			}
			else
			{
				tIP("Not allowed by permissions settings");
			}
		}
	}

}//void ExttIPCommands(pentry entries[], int x)


void ExttIPButtons(void)
{
	OpenFieldSet("tIP Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter/mod data</u><br>");
			tTablePullDownResellers(uForClient,1);
			if(cSearch[0])
				printf("<input type=hidden name=cSearch value='%s'>",cSearch);
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<p><u>Think twice</u>");
                        printf("<p>Here you can delete a single record or optionally a complete block of IPs."
				" In the later case you specify a uOwner company to limit the deletion.");
			printf("<p>cIPRange<br>\n");
			printf("<input title='Optionally enter CIDR IP Range (ex. 10.0.0.1/27) for available mass deletion'"
				" type=text name=cIPRange><p>\n");
			tTablePullDownResellers(uForClient,1);
			if(cSearch[0])
				printf("<input type=hidden name=cSearch value='%s'>",cSearch);
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review changes</u><br>");
			printf("Change a single record or optionally add a block of available IPs. In all cases you must"
				" specify a uDatacenter and optionally a new company (uOwner) if your"
				" permissions allow.<br>");
			printf("<p>cIPRange<br>\n");
			printf("<input title='Optionally enter CIDR IP Range (ex. 10.0.0.1/27) for available mass addition'"
				" type=text name=cIPRange value='%s'><p>\n",cIPRange);
			tTablePullDownResellers(uForClient,1);
			if(cSearch[0])
				printf("<input type=hidden name=cSearch value='%s'>",cSearch);
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			printf("<p><u>Table Tips</u><br>");
			printf("IPs used and available for container and new hardware node"
				" deployment are tracked and maintained here.<p>You can add available IPs <i>en masse</i>"
				" via the cIPRange at the [Modify] stage. For example 192.168.0.1/28 would add 16 IPs"
				" starting at 192.168.0.1 and ending at 192.168.0.16."
				" To add an initial range use [New], then [Modify], enter new CIDR range."
				"<p>You can also delete available IPs <i>en masse</i> via the cIPRange at the [Delete]"
				" stage.");
			printf("<p><u>Filter by cLabel</u><br>");
			printf("<input title='Enter cLabel start or MySQL LIKE pattern (%% or _ allowed)' type=text"
					" name=cSearch value='%s'>",cSearch);
			tIPNavList(0);
			tIPNavList(1);
			if(uIP)
				tIPReport();
			tIPUsedButAvailableNavList();
	}
	CloseFieldSet();

}//void ExttIPButtons(void)


void ExttIPAuxTable(void)
{

}//void ExttIPAuxTable(void)


void ExttIPGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uIP"))
		{
			sscanf(gentries[i].val,"%u",&uIP);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.31s",gentries[i].val);
		}
	}
	tIP("");

}//void ExttIPGetHook(entry gentries[], int x)


void ExttIPSelect(void)
{
	if(cSearch[0])
		ExtSelectSearchPublic("tIP",VAR_LIST_tIP,"cLabel",cSearch);
	else
		ExtSelectPublic("tIP",VAR_LIST_tIP);

}//void ExttIPSelect(void)


void ExttIPSelectRow(void)
{
	ExtSelectRowPublic("tIP",VAR_LIST_tIP,uIP);

}//void ExttIPSelectRow(void)


void ExttIPListSelect(void)
{
	char cCat[512];

	ExtListSelectPublic("tIP",VAR_LIST_tIP);
	
	//Changes here must be reflected below in ExttIPListFilter()
        if(!strcmp(gcFilter,"uIP"))
        {
                sscanf(gcCommand,"%u",&uIP);
		sprintf(cCat," WHERE tIP.uIP=%u ORDER BY uIP",uIP);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uIP");
        }

}//void ExttIPListSelect(void)


void ExttIPListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uIP"))
                printf("<option>uIP</option>");
        else
                printf("<option selected>uIP</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttIPListFilter(void)


void ExttIPNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=10 && !guListMode)
		printf(LANG_NBB_NEW);

	if((uAllowMod(uOwner,uCreatedBy) && uAvailable) || guPermLevel>=11)
		printf(LANG_NBB_MODIFY);

	if((uAllowMod(uOwner,uCreatedBy) && uAvailable) || guPermLevel>=11)
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttIPNavBar(void)


void tIPNavList(unsigned uAvailable)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uNumRows=0;
	unsigned uMySQLNumRows=0;
#define uLIMIT 32

	if(guPermLevel>11)
	{
		if(cSearch[0])
			sprintf(gcQuery,"SELECT tIP.uIP,tIP.cLabel,tContainer.cLabel,tContainer.uContainer FROM"
				" tIP LEFT JOIN tContainer ON tContainer.uIPv4=tIP.uIP WHERE tIP.uAvailable=%u"
				" AND tIP.cLabel LIKE '%s%%' ORDER BY tIP.cLabel",uAvailable,cSearch);
		else
			sprintf(gcQuery,"SELECT tIP.uIP,tIP.cLabel,tContainer.cLabel,tContainer.uContainer FROM"
					" tIP LEFT JOIN tContainer ON tContainer.uIPv4=tIP.uIP WHERE"
					" tIP.uAvailable=%u ORDER BY tIP.cLabel",uAvailable);
	}
	else
	{
		if(cSearch[0])
			sprintf(gcQuery,"SELECT tIP.uIP,tIP.cLabel,tContainer.cLabel,tContainer.uContainer FROM"
					" tIP LEFT JOIN tContainer ON tContainer.uIPv4=tIP.uIP WHERE"
					" tIP.uAvailable=%u AND tIP.cLabel LIKE '%s%%' AND"
					" tIP.uOwner=%u ORDER BY tIP.cLabel",uAvailable,cSearch,guCompany);
		else
			sprintf(gcQuery,"SELECT tIP.uIP,tIP.cLabel,tContainer.cLabel,tContainer.uContainer FROM"
					" tIP LEFT JOIN tContainer ON tContainer.uIPv4=tIP.uIP WHERE"
					" tIP.uAvailable=%u AND tIP.uOwner=%u ORDER BY tIP.cLabel",
						uAvailable,guCompany);
	}

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tIPNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if((uMySQLNumRows=mysql_num_rows(res)))
	{	
		if(uAvailable)
        		printf("<p><u>Available tIPNavList(%u)</u><br>\n",uMySQLNumRows);
		else
        		printf("<p><u>Used tIPNavList(%u)</u><br>\n",uMySQLNumRows);


	        while((field=mysql_fetch_row(res)))
		{

			if(cSearch[0])
				if(field[2]!=NULL)
					printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tIP&uIP=%s&cSearch=%s>%s</a>"
						" (<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s"
						"</a>)<br>\n",
						field[0],cURLEncode(cSearch),field[1],field[3],field[2]);
				else
					printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tIP&uIP=%s&cSearch=%s>%s"
						"</a><br>\n",field[0],cURLEncode(cSearch),field[1]);
			else
				if(field[2]!=NULL)
					printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tIP&uIP=%s>%s</a>"
						" (<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s"
						"</a>)<br>\n",field[0],field[1],field[3],field[2]);
				else
					printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tIP&uIP=%s>%s</a><br>\n",
							field[0],field[1]);
			if(++uNumRows>uLIMIT)
			{
				printf("(Only %u IPs shown use search/filters to shorten list.)<br>\n",uLIMIT);
				break;
			}
		}
	}
        mysql_free_result(res);

}//void tIPNavList(unsigned uAvailable)


void tIPReport(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uContainer,cHostname FROM tContainer WHERE uIPv4=%u",uIP);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tIPReport</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tIPReport tContainer</u><br>\n");


	        while((field=mysql_fetch_row(res)))
		{
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer"
					"&uContainer=%s>%s used by %s</a><br>\n",field[0],cLabel,field[1]);
	        }
	}
        mysql_free_result(res);

	sprintf(gcQuery,"SELECT tProperty.uKey,tContainer.cLabel FROM tProperty,tContainer"
				" WHERE tProperty.uType=3 AND tProperty.cValue='%s'"
				" AND tProperty.uKey=tContainer.uContainer",cLabel);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tIPReport</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tIPReport tContainer Extra IPs</u><br>\n");


	        while((field=mysql_fetch_row(res)))
		{
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer"
					"&uContainer=%s>%s used by %s</a><br>\n",field[0],cLabel,field[1]);
	        }
	}
        mysql_free_result(res);

	sprintf(gcQuery,"SELECT tProperty.uKey,tNode.cLabel FROM tProperty,tNode"
				" WHERE tProperty.uType=2 AND tProperty.cValue='%s'"
				" AND tProperty.uKey=tNode.uNode",cLabel);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tIPReport</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tIPReport tNode</u><br>\n");


	        while((field=mysql_fetch_row(res)))
		{
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tNode"
					"&uNode=%s>%s used by %s</a><br>\n",field[0],cLabel,field[1]);
	        }
	}
        mysql_free_result(res);

}//void tIPReport(void)


void AddIPRange(char *cIPRange)
{
	char *cIPs[2048];//Warning max 4 class C's or /22
	unsigned uCIDR4Mask=0;
	unsigned uCIDR4IP=0;
	register int i,uSkip=0;
        MYSQL_RES *res;

	if(!uInCIDR4Format(cIPRange,&uCIDR4IP,&uCIDR4Mask))
		tIP("<blink>Error:</blink> cIPRange is not in standard CIDR format!");
	if(uCIDR4Mask<22)
		tIP("<blink>Error:</blink> cIPRange is limited to 4 class C's or /22!");
	uCIDR4IP=ExpandCIDR4(cIPRange,&cIPs[0]);
	for(i=0;i<uCIDR4IP;i++)
	{
		sprintf(gcQuery,"SELECT uIP FROM tIP WHERE cLabel='%s'",cIPs[i]);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
        	res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res))
		{
			mysql_free_result(res);
			uSkip++;
			continue;
		}
		mysql_free_result(res);
		
		sprintf(gcQuery,"INSERT INTO tIP SET cLabel='%s',uOwner=%u,uCreatedBy=%u,uAvailable=1"
				",uCreatedDate=UNIX_TIMESTAMP(NOW()),uDatacenter=%u",
					cIPs[i],guCompany,guLoginClient,uDatacenter);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		free(cIPs[i]);
	}
	unxsVZLog(mysql_insert_id(&gMysql),"tIP","NewRange");
	if(!uSkip)
		tIP("Complete cIPRange added");
	else if(uSkip==i)
		tIP("<blink>Warning:</blink> cIPRange already exists no IPs added");
	else if(1)
		tIP("<blink>Note:</blink> Partial cIPRange added. At least one already existed");

}//void AddIPRange(char *cIPRange)


void DelIPRange(char *cIPRange)
{
	char *cIPs[2048];//Warning max 4 class C's or /22
	unsigned uCIDR4Mask=0;
	unsigned uCIDR4IP=0;
	register int i,uSkip=0;

	if(!uInCIDR4Format(cIPRange,&uCIDR4IP,&uCIDR4Mask))
		tIP("<blink>Error:</blink> cIPRange is not in standard CIDR format!");
	if(uCIDR4Mask<22)
		tIP("<blink>Error:</blink> cIPRange is limited to 4 class C's or /22!");
	uCIDR4IP=ExpandCIDR4(cIPRange,&cIPs[0]);
	for(i=0;i<uCIDR4IP;i++)
	{
		if(guCompany==1)
			sprintf(gcQuery,"DELETE FROM tIP WHERE cLabel='%s' AND uAvailable=1",cIPs[i]);
		else
			sprintf(gcQuery,"DELETE FROM tIP WHERE cLabel='%s' AND uAvailable=1 AND uOwner=%u",
										cIPs[i],guCompany);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		if(mysql_affected_rows(&gMysql)==1)
			uSkip++;
		free(cIPs[i]);
	}
	unxsVZLog(uIP,"tIP","DelRange");
	if(!uSkip)
		tIP("<blink>Warning:</blink> cIPRange of available IPs controlled by you did not exist!");
	else if(uSkip==i)
		tIP("Complete cIPRange deleted");
	else if(1)
		tIP("<blink>Note:</blink> Partial cIPRange deleted. At least one available IP controlled by you did not exist");

}//void DelIPRange(char *cIPRange)


void tIPUsedButAvailableNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uNumRows=0;
	unsigned uMySQLNumRows=0;
#define uUBALIMIT 64

	if(guPermLevel>11)
	{
		if(cSearch[0])
			sprintf(gcQuery,"SELECT tIP.uIP,tIP.cLabel,tContainer.cLabel,tContainer.uContainer FROM"
				" tIP,tContainer WHERE tContainer.uIPv4=tIP.uIP AND tIP.uAvailable=1"
				" AND tIP.cLabel LIKE '%s%%' ORDER BY tIP.cLabel",cSearch);
		else
			sprintf(gcQuery,"SELECT tIP.uIP,tIP.cLabel,tContainer.cLabel,tContainer.uContainer FROM"
					" tIP,tContainer WHERE tContainer.uIPv4=tIP.uIP AND"
					" tIP.uAvailable=1 ORDER BY tIP.cLabel");
	}
	else
	{
		if(cSearch[0])
			sprintf(gcQuery,"SELECT tIP.uIP,tIP.cLabel,tContainer.cLabel,tContainer.uContainer FROM"
					" tIP,tContainer WHERE tContainer.uIPv4=tIP.uIP AND"
					" tIP.uAvailable=1 AND tIP.cLabel LIKE '%s%%' AND"
					" tIP.uOwner=%u ORDER BY tIP.cLabel",cSearch,guCompany);
		else
			sprintf(gcQuery,"SELECT tIP.uIP,tIP.cLabel,tContainer.cLabel,tContainer.uContainer FROM"
					" tIP,tContainer WHERE tContainer.uIPv4=tIP.uIP AND"
					" tIP.uAvailable=1 AND tIP.uOwner=%u ORDER BY tIP.cLabel",guCompany);
	}

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tIPUsedButAvailableNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if((uMySQLNumRows=mysql_num_rows(res)))
	{	
        	printf("<p><u>tIPUsedButAvailableNavList(%u)</u><br>\n",uMySQLNumRows);

	        while((field=mysql_fetch_row(res)))
		{

			if(cSearch[0])
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tIP&uIP=%s&cSearch=%s>%s</a>"
						" (<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s"
						"</a>)<br>\n",
						field[0],cURLEncode(cSearch),field[1],field[3],field[2]);
			else
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tIP&uIP=%s>%s</a>"
						" (<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s"
						"</a>)<br>\n",field[0],field[1],field[3],field[2]);
			if(++uNumRows>uLIMIT)
			{
				printf("(Only %u IPs shown use search/filters to shorten list.)<br>\n",uLIMIT);
				break;
			}
		}

		printf("<input type=submit class=lwarnButton name=gcCommand value=Fix"
				" title='Change all the tIPUsedButAvailableNavList IPs to not available'><br>\n");
	}
        mysql_free_result(res);

}//void tIPUsedButAvailableNavList()


void tIPUsedButAvailableFix(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(guPermLevel>11)
	{
		if(cSearch[0])
			sprintf(gcQuery,"SELECT tIP.uIP FROM"
				" tIP,tContainer WHERE tContainer.uIPv4=tIP.uIP AND tIP.uAvailable=1"
				" AND tIP.cLabel LIKE '%s%%' ORDER BY tIP.cLabel",cSearch);
		else
			sprintf(gcQuery,"SELECT tIP.uIP FROM"
					" tIP,tContainer WHERE tContainer.uIPv4=tIP.uIP AND"
					" tIP.uAvailable=1 ORDER BY tIP.cLabel");
	}
	else
	{
		if(cSearch[0])
			sprintf(gcQuery,"SELECT tIP.uIP FROM"
					" tIP,tContainer WHERE tContainer.uIPv4=tIP.uIP AND"
					" tIP.uAvailable=1 AND tIP.cLabel LIKE '%s%%' AND"
					" tIP.uOwner=%u ORDER BY tIP.cLabel",cSearch,guCompany);
		else
			sprintf(gcQuery,"SELECT tIP.uIP FROM"
					" tIP,tContainer WHERE tContainer.uIPv4=tIP.uIP AND"
					" tIP.uAvailable=1 AND tIP.uOwner=%u ORDER BY tIP.cLabel",guCompany);
	}

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		tIP(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
	{	
	        while((field=mysql_fetch_row(res)))
		{
			sprintf(gcQuery,"UPDATE tIP SET uAvailable=0 WHERE uIP=%s",field[0]);
        		mysql_query(&gMysql,gcQuery);
		        if(mysql_errno(&gMysql))
				tIP(mysql_error(&gMysql));
		}
	}
        mysql_free_result(res);

}//void tIPUsedButAvailableFix()

