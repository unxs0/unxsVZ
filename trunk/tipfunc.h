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

void tIPReport(char *cLabel, unsigned uAux);
void tDatacenterIPReport(void);
void tIPFirewallSubnetReport(void);
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

//extern
unsigned uGetSearchGroup(const char *gcUser,unsigned uGroupType);

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

                if(!strncmp(gcCommand,"Set ",4) || !strncmp(gcCommand,"Group ",6) || !strncmp(gcCommand,"Delete Checked",14))
                {
			ProcesstIPVars(entries,x);
                        guMode=12002;
			tIP(gcCommand);
		}
		else if(!strcmp(gcCommand,"Reload Search Set"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstIPVars(entries,x);
                        	guMode=12002;
	                        tIP("Search set reloaded");
			}
			else
			{
				tIP("<blink>Error:</blink> Denied by permissions settings");
			}
		}
		else if(!strcmp(gcCommand,"Datacenter IP Report"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstIPVars(entries,x);
                        	guMode=16001;
	                        tIP("Datacenter IP Report");
			}
			else
			{
				tIP("<blink>Error:</blink> Denied by permissions settings");
			}
		}
		else if(!strcmp(gcCommand,"Firewall Subnet Report"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstIPVars(entries,x);
                        	guMode=17001;
	                        tIP("Firewall Subnet Report");
			}
			else
			{
				tIP("<blink>Error:</blink> Denied by permissions settings");
			}
		}
		else if(!strcmp(gcCommand,"IP Report Single"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstIPVars(entries,x);
                        	guMode=14001;
				//tIPReport()
	                        tIP("IP Report Single");
			}
			else
			{
				tIP("<blink>Error:</blink> Denied by permissions settings");
			}
		}
		else if(!strcmp(gcCommand,"IP Report All"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstIPVars(entries,x);
				if(!uDatacenter)
					tIP("<blink>Error:</blink> you need to select a datacenter");
                        	guMode=15001;
				//tIPReport()
	                        tIP("IP Report All");
			}
			else
			{
				tIP("<blink>Error:</blink> Denied by permissions settings");
			}
		}
		else if(!strcmp(gcCommand,"Remove from Search Set"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstIPVars(entries,x);
                        	guMode=12002;
				char cQuerySection[256];
				unsigned uLink=0;
				unsigned uGroup=0;

				if(cIPv4Search[0]==0 && uDatacenterSearch==0 && uNodeSearch==0 && uNodeSearchNot==0 && uAvailableSearch==0
						&& uOwnerSearch==0 && uIPv4Exclude==0 && cCommentSearch[0]==0 && uCountryCodeSearch==0)
	                        	tIP("You must specify at least one search parameter");

				if((uGroup=uGetSearchGroup(gcUser,31))==0)
		                        tIP("No search set exists. Please create one first.");

				//Initial query section
				sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroup=%u AND uIP IN"
						" (SELECT uIP FROM tIP WHERE",uGroup);

				if(cIPv4Search[0])
				{
					sprintf(cQuerySection," uIP IN (SELECT uIP FROM tIP WHERE cLabel LIKE '%s%%')",cIPv4Search);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}
				else
				{
					uLink=0;
				}

				if(uDatacenterSearch)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uDatacenter=%u",uDatacenterSearch);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uNodeSearch || uNodeSearchNot)
				{
					if(uLink)
						strcat(gcQuery," AND");
					if(uNodeSearchNot && uNodeSearch )
						sprintf(cQuerySection," uIP IN (SELECT uIPv4 FROM tContainer WHERE uNode!=%u)",uNodeSearch);
					else if(uNodeSearch)
						sprintf(cQuerySection," uIP IN (SELECT uIPv4 FROM tContainer WHERE uNode=%u)",uNodeSearch);
					else
						sprintf(cQuerySection," uIP IN"
								" (SELECT uIP FROM tIP LEFT JOIN tContainer ON tContainer.uIPv4=tIP.uIP"
								" WHERE tContainer.cLabel IS NULL)");
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}


				if(uFirewallMode)
				{
					if(uCountryCodeSearch || uCountryCodeSearchNot)
					{
						if(uLink)
							strcat(gcQuery," AND");
						if(uCountryCodeSearchNot && uCountryCodeSearch)
							sprintf(cQuerySection," uCountryCode!=%u",uCountryCodeSearch);
						else if(uCountryCodeSearch)
							sprintf(cQuerySection," uCountryCode=%u",uCountryCodeSearch);
						else
							sprintf(cQuerySection," uCountryCode=0");
						strcat(gcQuery,cQuerySection);
						uLink=1;
					}
					if(uFWStatusSearch)
					{
						if(uLink)
							strcat(gcQuery," AND");
						sprintf(cQuerySection," uFWStatus=%u",uFWStatusSearch);
						strcat(gcQuery,cQuerySection);
						uLink=1;
					}
				}

				//YesNo tristate
				if(uAvailableSearch)
				{
					if(uLink)
						strcat(gcQuery," AND");
					if(uAvailableSearch==2)//Tri state Yes
						sprintf(cQuerySection," uAvailable>0");
					else
						sprintf(cQuerySection," uAvailable=0");
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uOwnerSearch)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uOwner=%u",uOwnerSearch);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uIPv4Exclude)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," cLabel NOT LIKE '10.%%.%%.%%' AND"
								" cLabel NOT LIKE '172.16.%%.%%' AND" //This is only the first class C of the /12
								" cLabel NOT LIKE '172.17.%%.%%' AND" 
								" cLabel NOT LIKE '172.18.%%.%%' AND"
								" cLabel NOT LIKE '192.168.%%.%%'");
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(cCommentSearch[0])
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," cComment LIKE '%s'",cCommentSearch);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				strcat(gcQuery,")");
				//debug only
	                        //tIP(gcQuery);

				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				unsigned uNumber=0;
				if((uNumber=mysql_affected_rows(&gMysql))>0)
				{
	                        	sprintf(gcQuery,"%u tIP records were removed from your search set",uNumber);
	                        	tIP(gcQuery);
				}
				else
				{
	                        	tIP("No records were removed from your search set");
				}
			}
			else
			{
				tIP("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,"Add to Search Set") || 
				!strcmp(gcCommand,"Create Search Set") ||
				!strcmp(gcCommand,"New Firewall Set") ||
				!strcmp(gcCommand,"Create Firewall Set") )
                {
			if(guPermLevel>=9)
			{
	                        ProcesstIPVars(entries,x);
                        	guMode=12002;
				char cQuerySection[256];
				unsigned uLink=0;
				unsigned uGroup=0;

				if(!strcmp(gcCommand,"Create Firewall Set"))
				{
					uFirewallMode=1;
					uFWStatusAnySearch=1;
				}
				else if(!strcmp(gcCommand,"New Firewall Set"))
				{
					uFirewallMode=1;
					u4Limit=1;
					uFWStatusAnySearch=1;
				}

				if(cIPv4Search[0]==0 && uDatacenterSearch==0 && uNodeSearch==0 && uNodeSearchNot==0 && uAvailableSearch==0
						&& uOwnerSearch==0 && uIPv4Exclude==0 && cCommentSearch[0]==0 
						&& uFWStatusSearch==0 && uFWStatusAnySearch==0 && uCountryCodeSearch==0)
	                        	tIP("You must specify at least one search parameter");

				if((uGroup=uGetSearchGroup(gcUser,31))==0)
				{
					sprintf(gcQuery,"INSERT INTO tGroup SET cLabel='%s',uGroupType=31"//2 is search group
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							gcUser,guCompany,guLoginClient);//2=search set type TODO
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					if((uGroup=mysql_insert_id(&gMysql))==0)
		                        	tIP("An error ocurred when attempting to create your search set");
				}
				else
				{
					if(!strcmp(gcCommand,"Create Search Set") || !strcmp(gcCommand,"Create Firewall Set")
						|| !strcmp(gcCommand,"New Firewall Set"))
					{
						sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroup=%u",uGroup);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					}
                		}

				//Initial query section
				sprintf(gcQuery,"INSERT INTO tGroupGlue (uGroup,uIP)"
						" SELECT %u,uIP FROM tIP WHERE",uGroup);

				//Build AND query section

				if(cIPv4Search[0])
				{
					sprintf(cQuerySection," uIP IN (SELECT uIP FROM tIP WHERE cLabel LIKE '%s%%')",cIPv4Search);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}
				else
				{
					uLink=0;
				}

				if(u24Limit)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uModDate>(UNIX_TIMESTAMP(NOW())-86400)");
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}
				else if(u12Limit)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uModDate>(UNIX_TIMESTAMP(NOW())-43200)");
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}
				else if(u4Limit)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uModDate>(UNIX_TIMESTAMP(NOW())-14400)");
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uDatacenterSearch)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uDatacenter=%u",uDatacenterSearch);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uFWStatusSearch || uFWStatusAnySearch)
				{
					if(uLink)
						strcat(gcQuery," AND");
					if(uFWStatusAnySearch)
						sprintf(cQuerySection," uFWStatus>0");
					else
						sprintf(cQuerySection," uFWStatus=%u",uFWStatusSearch);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uFirewallMode)
				{
					if(uCountryCodeSearch || uCountryCodeSearchNot)
					{
						if(uLink)
							strcat(gcQuery," AND");
						if(uCountryCodeSearchNot && uCountryCodeSearch)
							sprintf(cQuerySection," uCountryCode!=%u",uCountryCodeSearch);
						else if(uCountryCodeSearch)
							sprintf(cQuerySection," uCountryCode=%u",uCountryCodeSearch);
						else
							sprintf(cQuerySection," uCountryCode=0");
						strcat(gcQuery,cQuerySection);
						uLink=1;
					}
					if(uFWStatusSearch)
					{
						if(uLink)
							strcat(gcQuery," AND");
						sprintf(cQuerySection," uFWStatus=%u",uFWStatusSearch);
						strcat(gcQuery,cQuerySection);
						uLink=1;
					}
				}

				if(uNodeSearch || uNodeSearchNot)
				{
					if(uLink)
						strcat(gcQuery," AND");
					if(uNodeSearchNot && uNodeSearch )
						sprintf(cQuerySection," uIP IN (SELECT uIPv4 FROM tContainer WHERE uNode!=%u)",uNodeSearch);
					else if(uNodeSearch)
						sprintf(cQuerySection," uIP IN (SELECT uIPv4 FROM tContainer WHERE uNode=%u)",uNodeSearch);
					else
						sprintf(cQuerySection," uIP IN"
								" (SELECT uIP FROM tIP LEFT JOIN tContainer ON tContainer.uIPv4=tIP.uIP"
								" WHERE tContainer.cLabel IS NULL)");
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				//YesNo tristate
				if(uAvailableSearch)
				{
					if(uLink)
						strcat(gcQuery," AND");
					if(uAvailableSearch==2)//Tri state Yes
						sprintf(cQuerySection," uAvailable>0");
					else
						sprintf(cQuerySection," uAvailable=0");
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uOwnerSearch)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," uOwner=%u",uOwnerSearch);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(uIPv4Exclude)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," cLabel NOT LIKE '10.%%.%%.%%' AND"
								" cLabel NOT LIKE '172.16.%%.%%' AND" //This is only the first class C of the /12
								" cLabel NOT LIKE '172.17.%%.%%' AND" 
								" cLabel NOT LIKE '172.18.%%.%%' AND"
								" cLabel NOT LIKE '192.168.%%.%%'");
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				if(cCommentSearch[0])
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," cComment LIKE '%s'",cCommentSearch);
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				//debug only
	                        //tIP(gcQuery);

				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				unsigned uNumber=0;
				if((uNumber=mysql_affected_rows(&gMysql))>0)
				{
	                        	sprintf(gcQuery,"%u tIP records were added to your search set",uNumber);
	                        	tIP(gcQuery);
				}
				else
				{
	                        	tIP("No records were added to your search set. Filter returned 0 records");
				}
			}
			else
			{
				tIP("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,"Firewall Operations"))
                {
			if(guPermLevel>=9)
			{
                        	guMode=12001;
				uFirewallMode=1;
				u4Limit=1;
	                        tIP("Firewall Operations");
			}
			else
			{
				tIP("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,"Search Set Operations"))
                {
			if(guPermLevel>=9)
			{
                        	guMode=12001;
	                        tIP("Search Set Operations");
			}
			else
			{
				tIP("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_NEW))
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
				sprintf(gcQuery,"DELETE FROM tProperty WHERE uType=31 AND uKey=%u",uIP);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
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

		case 15001:
		case 16001:
		case 17001:
		case 14001:
			printf("<u>IP Report in bottom panel</u><br>");
		break;

                case 12001:
                case 12002:
			printf("<u>Create or refine your user search set</u><br>");
			printf("In the right panel you can select your search criteria. When refining you do not need"
				" to reuse your initial search critieria. Your search set is persistent even across unxsVZ sessions.<p>");
			printf("<input type=submit class=largeButton title='Create an initial or replace an existing search set'"
				" name=gcCommand value='Create Search Set'>");
			printf("<input type=submit class=largeButton title='Create special firewall IP search set'"
				" name=gcCommand value='Create Firewall Set'>");
			printf("<p><input type=submit class=largeButton title='Add the results to your current search set. Do not add the same search"
				" over and over again it will not result in any change but may slow down processing.'"
				" name=gcCommand value='Add to Search Set'>");
			printf("<input type=submit class=largeButton title='Apply the right panel filter to refine your existing search set"
				" by removing set elements that match the filter settings.'"
				" name=gcCommand value='Remove from Search Set'>");
			printf("<p><input type=submit class=largeButton title='Reload current search set. Good for checking for any new status updates'"
				" name=gcCommand value='Reload Search Set'>");
			printf("<input type=submit class=largeButton title='Return to main tContainer tab page'"
				" name=gcCommand value='Cancel'>");
			printf("<p><u>Set Operation Options</u>");

                break;

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
			printf("<p><input type=submit class=largeButton title='Open user search set page in firewall control mode.'"
				" name=gcCommand value='Firewall Operations'>\n");
			printf("<p><input type=submit class=lwarnButton title='Open user search set page in firewall control mode."
				" Creating a new last 4 hour set. Any saved set will be lost.'"
				" name=gcCommand value='New Firewall Set'>\n");
			printf("<p><input type=submit class=largeButton title='Open user search set page. There you can create search sets and operate"
				" on selected IPs of the loaded IP number set.'"
				" name=gcCommand value='Search Set Operations'>\n");
			printf("<p><input type=submit class=largeButton title='Create report on IP usage and errors for the loaded IP'"
				" name=gcCommand value='IP Report Single'>\n");
			printf("<p><input type=submit class=largeButton title='!Warning may take some time. Do not abuse!"
				" Create report on assignment errors for all IPs of given datacenter'"
				" name=gcCommand value='IP Report All'>\n");
			printf("<p><input type=submit class=largeButton title=\"Non rfc-1918 class C's in use by active non CustomerPremise datacenters\""
				" name=gcCommand value='Datacenter IP Report'>\n");
			printf("<p><input type=submit class=largeButton title='Subnet analyzer for firewall consolidation'"
				" name=gcCommand value='Firewall Subnet Report'>\n");

			printf("<p><u>Filter by cLabel</u><br>");
			printf("<input title='Enter cLabel start or MySQL LIKE pattern (%% or _ allowed)' type=text"
					" name=cSearch value='%s'>",cSearch);

			if(cSearch[0])
			{
				tIPNavList(0);
				tIPUsedButAvailableNavList();
			}
	}
	CloseFieldSet();

}//void ExttIPButtons(void)


void ExttIPAuxTable(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uGroup=0;
	unsigned uNumRows=0;
	unsigned uRow=0;

	switch(guMode)
	{
		case 17001:
		case 16001:
		case 15001:
		case 14001:
			OpenFieldSet("IP Report",100);
			if(guMode==14001)
				tIPReport(cLabel,uIP);
			else if(guMode==15001)
				tIPReport("",uDatacenter);
			else if(guMode==16001)
				tDatacenterIPReport();
			else if(guMode==17001)
				tIPFirewallSubnetReport();
			CloseFieldSet();
		break;

		case 12001:
		case 12002:
			//Set operation buttons
			OpenFieldSet("Set Operations",100);
			printf("<input title='Delete checked containers from your search set. They will still be visible but will"
				" marked deleted and will not be used in any subsequent set operation'"
				" type=submit class=largeButton name=gcCommand value='Delete Checked'>\n");
			printf("&nbsp; <input title='Deletes IPs not in use by containers.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Delete'>\n");
			printf("&nbsp; <input title='Updates IPs to uAvailable=1 for IPs not in use by containers.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Make Available'>\n");
			printf("&nbsp; <input title='Updates IPs to uAvailable=0. Adds comment use optionally IPv4 pattern field.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Make Not Available'>\n");
			printf("&nbsp; <input title='Send one ping packet to IP. Check firewall settings use with care.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Ping'>\n");
			printf("&nbsp; <input title='Change owner using filter uOwnerSearch select'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Change Owner'>\n");
			printf("&nbsp; <input title='Change datacenter using filter uDatacenterSearch select'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Change Datacenter'>\n");
			printf("<p><input title='Create BlockAccess (iptables DROP target) jobs for all"
				" firewalls. Click on tIP.cLabel to get complete NOC and FW change history.''"
				" type=submit class=lwarnButton"
				" name=gcCommand value='Group BlockAccess'>\n");
			printf("&nbsp; <input title='Create UndoBlockAccess (changes to iptables ACCEPT target) jobs for all"
				" firewalls. Click on tIP.cLabel to get complete NOC and FW change history.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group UndoBlockAccess'>\n");
			printf("&nbsp; <input title='Show country information in results field. Adds CN=cCountryName; to cComment.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group CountryInfo'>\n");
			printf("&nbsp; <input title='Show datacenter and destination IP counts, as found in linked tProperty entries."
				" Scores over 3 probably justify a firewall DROP.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Reputation Score'>\n");
			printf("&nbsp;<input title='Set firewall status to whitelisted. This keeps the IP from being blocked by unxsSnort agent.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Whitelist'>\n");
			if(guPermLevel>=10)
			{
				printf("&nbsp; <input title='Remove IP from all iptables node firewalls'"
				" type=submit class=lwarnButton"
				" name=gcCommand value='Group RemoveFW'>\n");
				printf("&nbsp; <input title='Fix waiting status to completed'"
				" type=submit class=lwarnButton"
				" name=gcCommand value='Group FixWaiting'>\n");
			}
			CloseFieldSet();

			sprintf(gcQuery,"Search Set Contents");
			OpenFieldSet(gcQuery,100);
			uGroup=uGetSearchGroup(gcUser,31);
			if(uFirewallMode)
				sprintf(gcQuery,"SELECT"
					" tIP.uIP,"
					" tIP.cLabel,"
					//" IF(tIP.uAvailable>0,'Yes','No'),"
					//" IFNULL(tDatacenter.cLabel,''),"
					" IFNULL(tClient.cLabel,''),"
					" FROM_UNIXTIME(tIP.uCreatedDate,'%%a %%b %%d %%T %%Y'),"
					" FROM_UNIXTIME(tIP.uModDate,'%%a %%b %%d %%T %%Y'),"
					" IFNULL(tFWStatus.cLabel,''),"
					" IFNULL(tFWRules.cRuleName,'No rule. Most likely added by NOC staff.'),"
					" IFNULL(tGeoIPCountryCode.cCountryCode,''),"
					" tIP.cComment"
					" FROM tIP"
					" LEFT JOIN tDatacenter ON tIP.uDatacenter=tDatacenter.uDatacenter"
					" LEFT JOIN tClient ON tIP.uModBy=tClient.uClient"
					" LEFT JOIN tFWStatus ON tIP.uFWStatus=tFWStatus.uFWStatus"
					" LEFT JOIN tFWRules ON tIP.uFWRule=tFWRules.uFWRules"
					" LEFT JOIN tGeoIPCountryCode ON tIP.uCountryCode=tGeoIPCountryCode.uGeoIPCountryCode"
					" WHERE uIP IN (SELECT uIP FROM tGroupGlue WHERE uGroup=%u) ORDER BY tIP.uModDate DESC",uGroup);
			else
				sprintf(gcQuery,"SELECT"
					" tIP.uIP,"
					" tIP.cLabel,"
					" IF(tIP.uAvailable>0,'Yes','No'),"
					" IFNULL(tDatacenter.cLabel,''),"
					" IFNULL(tNode.cLabel,''),"
					" IFNULL(tContainer.cHostname,''),"
					" tClient.cLabel,"
					" FROM_UNIXTIME(tIP.uCreatedDate,'%%a %%b %%d %%T %%Y'),"
					" FROM_UNIXTIME(tIP.uModDate,'%%a %%b %%d %%T %%Y'),"
					" tIP.cComment,"
					" tContainer.uContainer"
					" FROM tIP"
					" LEFT JOIN tContainer ON tContainer.uIPv4=tIP.uIP"
					" LEFT JOIN tDatacenter ON tIP.uDatacenter=tDatacenter.uDatacenter"
					" LEFT JOIN tNode ON tContainer.uNode=tNode.uNode"
					" LEFT JOIN tClient ON tIP.uOwner=tClient.uClient"
					" WHERE uIP IN (SELECT uIP FROM tGroupGlue WHERE uGroup=%u) ORDER BY tIP.uIP DESC",uGroup);
		        mysql_query(&gMysql,gcQuery);
		        if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		        res=mysql_store_result(&gMysql);
			if((uNumRows=mysql_num_rows(res)))
			{
				char cResult[100]={""};
				char cCommentUpdated[256]={""};
				char cFWStatusUpdated[256]={""};
				char cCtLabel[100]={""};

				printf("<table>");
				printf("<tr>");
				if(uFirewallMode)
					printf("<td><input type=checkbox name=all onClick='checkAll(document.formMain,this)'> <u>cLabel</u></td>"
						//"<td><u>Available</u></td>"
						//"<td><u>Datacenter</u></td>"
						"<td><u>ModBy</u></td>"
						"<td><u>CreatedDate</u></td>"
						"<td><u>ModifiedDate</u></td>"
						"<td><u>uFWStatus</u></td>"
						"<td><u>uFWRule</u></td>"
						"<td><u>uCountryCode</u></td>"
						"<td><u>Comment</u></td>"
						"<td><u>Set operation result</u></td></tr>");
				else
					printf("<td><input type=checkbox name=all onClick='checkAll(document.formMain,this)'> <u>cLabel</u></td>"
						"<td><u>Available</u></td>"
						"<td><u>Datacenter</u></td>"
						"<td><u>Node</u></td>"
						"<td><u>Hostname</u></td>"
						"<td><u>Owner</u></td>"
						"<td><u>CreatedDate</u></td>"
						"<td><u>ModifiedDate</u></td>"
						"<td><u>Comment</u></td>"
						"<td><u>Set operation result</u></td></tr>");
//Reset margin start
while((field=mysql_fetch_row(res)))
{
	cCommentUpdated[0]=0;
	cFWStatusUpdated[0]=0;
	if(guMode==12002)
	{
		register int i;
		unsigned uCtIP=0;

		cResult[0]=0;
		sscanf(field[0],"%u",&uCtIP);
		sprintf(cCtLabel,"Ct%u",uCtIP);
		for(i=0;i<x;i++)
		{
			//insider xss protection
			if(guPermLevel<10)
				continue;

			if(!strcmp(entries[i].name,cCtLabel))
			{
				if(!strcmp(gcCommand,"Delete Checked"))
				{
					sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroup=%u AND uIP=%u",uGroup,uCtIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						sprintf(cResult,mysql_error(&gMysql));
						break;
					}
					if(mysql_affected_rows(&gMysql)>0)
						sprintf(cResult,"Deleted from set");
					else
						sprintf(cResult,"Unexpected non deletion");
					break;
				}//Delete Checked

				//Group Delete
				else if(!strcmp(gcCommand,"Group Delete"))
				{
					MYSQL_RES *res;

					if(guPermLevel<12)
					{
						sprintf(cResult,"must be root");
						break;
					}

					sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroup=%u AND uIP=%u",uGroup,uCtIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));

					sprintf(gcQuery,"SELECT uIPv4 FROM tContainer WHERE uIPv4=%u",uCtIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        				res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)>0)
					{
						sprintf(cResult,"IP in use");
						break;
					}

					sprintf(gcQuery,"DELETE FROM tProperty WHERE uType=31 AND uKey=%u",uCtIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));

					sprintf(gcQuery,"DELETE FROM tIP WHERE uIP=%u",uCtIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					if(mysql_affected_rows(&gMysql)>0)
						sprintf(cResult,"group delete done");
					else
						cResult[0]=0;
					break;
				}//Group Delete


				//Group Make Available
				else if(!strcmp(gcCommand,"Group Make Available"))
				{
					MYSQL_RES *res;

					sprintf(gcQuery,"SELECT uIPv4 FROM tContainer WHERE uIPv4=%u",uCtIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        				res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)>0)
					{
						sprintf(cResult,"IP in use");
						break;
					}

					sprintf(gcQuery,"UPDATE tIP SET uAvailable=1,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
							" WHERE uIP=%u AND uAvailable=0",
								guLoginClient,
								uCtIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					if(mysql_affected_rows(&gMysql)>0)
						sprintf(cResult,"make available done");
					else
						cResult[0]=0;
					break;
				}//Group Make Available

				//Group Make Not Available
				else if(!strcmp(gcCommand,"Group Make Not Available"))
				{
					if(cIPv4Search[0]==0) sprintf(cIPv4Search,"Reserved via group command");
					sprintf(gcQuery,"UPDATE tIP SET uAvailable=0,cComment='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
							" WHERE uIP=%u AND uAvailable=1",
								cIPv4Search,guLoginClient,
								uCtIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					if(mysql_affected_rows(&gMysql)>0)
					{
						sprintf(cResult,"make not available done");
						sprintf(cCommentUpdated,"%.255s",cIPv4Search);
					}	
					else
					{
						cResult[0]=0;
					}
					break;
				}//Group Make Not Available

				//Group Ping
				else if(!strcmp(gcCommand,"Group Ping"))
				{
					MYSQL_RES *res;
					MYSQL_ROW field;

					sprintf(gcQuery,"SELECT cLabel FROM tIP WHERE uIP=%u",uCtIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        				res=mysql_store_result(&gMysql);
	        			if((field=mysql_fetch_row(res)))
					{
						char cPingCommand[64];
						sprintf(cPingCommand,"/bin/ping -W 1 -c 1 %.15s",field[0]);
						if(system(cPingCommand))
							sprintf(cResult,"no ping response");
						else
							sprintf(cResult,"ping responded");
					}
					else
					{
							sprintf(cResult,"unexpected condition");
					}
					break;

				}//Group Ping

				//Group Change Owner
				else if(!strcmp(gcCommand,"Group Change Owner") && uOwnerSearch)
				{
					sprintf(gcQuery,"UPDATE tIP SET uOwner=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
							" WHERE uIP=%u",
								uOwnerSearch,
								guLoginClient,
								uCtIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					if(mysql_affected_rows(&gMysql)>0)
						sprintf(cResult,"owner changed");
					else
						cResult[0]=0;
					break;
				}//Group Change Owner

				//Group Change Datacenter
				else if(!strcmp(gcCommand,"Group Change Datacenter") && uDatacenterSearch)
				{
					sprintf(gcQuery,"UPDATE tIP SET uDatacenter=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
							" WHERE uIP=%u",
								uDatacenterSearch,
								guLoginClient,
								uCtIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					if(mysql_affected_rows(&gMysql)>0)
						sprintf(cResult,"datacenter changed");
					else
						cResult[0]=0;
					break;
				}//Group Change Datacenter

				//Group RemoveFW
				else if(!strcmp(gcCommand,"Group RemoveFW"))
				{
					MYSQL_RES *res;
					MYSQL_ROW field;
					char cIP[16]={""};
					sprintf(cIP,"%.15s",ForeignKey("tIP","cLabel",uCtIP));
					unsigned uAvailable=0;
					sscanf(ForeignKey("tIP","uAvailable",uCtIP),"%u",&uAvailable);
					unsigned uDatacenter=0;
					sscanf(ForeignKey("tIP","uDatacenter",uCtIP),"%u",&uDatacenter);
					unsigned uFWStatus=0;
					sscanf(ForeignKey("tIP","uFWStatus",uCtIP),"%u",&uFWStatus);
					if(guPermLevel<10)
					{
						sprintf(cResult,"insufficient permlevel");
						break;
					}
					if(!cIP[0])
					{
						sprintf(cResult,"data error");
						break;
					}
					if(uDatacenter!=41)
					{
						sprintf(cResult,"wrong datacenter");
						break;
					}
					if(uAvailable==1)
					{
						sprintf(cResult,"must not be available");
						break;
					}
					if( uFWStatus!=uFWACCESS && uFWStatus!=uFWBLOCKED)
					{
						sprintf(cResult,"incorrect status for op");
						break;
					}

					//Create a tJob for each active tNode
					sprintf(gcQuery,"SELECT tNode.uNode,tDatacenter.uDatacenter FROM tNode,tDatacenter"
								" WHERE tDatacenter.uStatus=1 AND tNode.uStatus=1"
								" AND tDatacenter.cLabel!='CustomerPremise'"
								" AND tNode.uDatacenter=tDatacenter.uDatacenter");
					//			debug only
					//			" AND tNode.uDatacenter=tDatacenter.uDatacenter LIMIT 1");
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						sprintf(cResult,"%.31s",mysql_error(&gMysql));
						break;
					}
					res=mysql_store_result(&gMysql);
					unsigned uCount=0;
					unsigned uMasterJob=0;//first job is master job
					char cJobLabel[32]={"RemoveDropFromIPTables"};
					if(uFWStatus==uFWACCESS)
						sprintf(cJobLabel,"RemoveAcceptFromIPTables");
					while((field=mysql_fetch_row(res)))
					{
						unsigned uNode=0;
						unsigned uDatacenter=0;
						sscanf(field[0],"%u",&uNode);
						sscanf(field[1],"%u",&uDatacenter);
						sprintf(gcQuery,"INSERT INTO tJob"
							" SET uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
							",cLabel='%sNOC'"
							",cJobName='%s'"
							",uDatacenter=%u,uNode=%u"
							",uMasterJob=%u"
							",cJobData='cIPv4=%.15s;'"
							",uJobDate=UNIX_TIMESTAMP(NOW())"
							",uJobStatus=1",
								guCompany,
								guLoginClient,
								cJobLabel,cJobLabel,
								uDatacenter,
								uNode,
								uMasterJob,
								cIP);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							sprintf(cResult,"%.31s",mysql_error(&gMysql));
							break;
						}
						if(!uMasterJob)
							uMasterJob=mysql_insert_id(&gMysql);
						uCount++;
					}//while node
					sprintf(cResult,"%u jobs created",uCount);
					if(uCount)
					{
						sprintf(gcQuery,"UPDATE tIP"
								" SET uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
								",uFWStatus=%u"
								" WHERE uIP=%u",
									guLoginClient,
									uFWWAITINGREMOVAL,
									uCtIP);
						mysql_query(&gMysql,gcQuery);
						sprintf(cFWStatusUpdated,"%.255s",ForeignKey("tFWStatus","cLabel",uFWWAITINGBLOCK));
						sprintf(gcQuery,"INSERT INTO tProperty"
								" SET uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
								",uOwner=1"
								",cName='NOC FW Activity'"
								",cValue=CONCAT(NOW(),' Group RemoveFW %s (%u/%u)')"
								",uType=31"
								",uKey=%u",
									gcUser,guCompany,guPermLevel,uCtIP);
						mysql_query(&gMysql,gcQuery);
					}
					unxsVZLog(uCtIP,"tIP","Group RemoveFW");
					break;
				}//Group RemoveFW

				//Group BlockAccess
				else if(!strcmp(gcCommand,"Group BlockAccess"))
				{
					MYSQL_RES *res;
					MYSQL_ROW field;
					char cIP[16]={""};
					sprintf(cIP,"%.15s",ForeignKey("tIP","cLabel",uCtIP));
					unsigned uAvailable=0;
					sscanf(ForeignKey("tIP","uAvailable",uCtIP),"%u",&uAvailable);
					unsigned uDatacenter=0;
					sscanf(ForeignKey("tIP","uDatacenter",uCtIP),"%u",&uDatacenter);
					unsigned uFWStatus=0;
					sscanf(ForeignKey("tIP","uFWStatus",uCtIP),"%u",&uFWStatus);
					if(guPermLevel<10)
					{
						sprintf(cResult,"insufficient permlevel");
						break;
					}
					if(!cIP[0])
					{
						sprintf(cResult,"data error");
						break;
					}
					if(uDatacenter!=41)
					{
						sprintf(cResult,"wrong datacenter");
						break;
					}
					if(uAvailable==1)
					{
						sprintf(cResult,"must not be available");
						break;
					}
					if( uFWStatus && uFWStatus!=uFWACCESS)
					{
						sprintf(cResult,"incorrect status for op");
						break;
					}

					//Create a BlockAccess tJob for each active tNode
					sprintf(gcQuery,"SELECT tNode.uNode,tDatacenter.uDatacenter FROM tNode,tDatacenter"
								" WHERE tDatacenter.uStatus=1 AND tNode.uStatus=1"
								" AND tDatacenter.cLabel!='CustomerPremise'"
								" AND tNode.uDatacenter=tDatacenter.uDatacenter");
					//			debug only
					//			" AND tNode.uDatacenter=tDatacenter.uDatacenter LIMIT 1");
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						sprintf(cResult,"%.31s",mysql_error(&gMysql));
						break;
					}
					res=mysql_store_result(&gMysql);
					unsigned uCount=0;
					//multiple related job control
					//jobqueue.c must handle jobs
					//that have a uMasterJob!=0 by checking
					//that all tJobs with the same uMasterJob 
					//AND that the master job uJob=uMasterJob have been done ok.
					//If all are done then it updates the related item,
					//in this case the tIP.uFWStatus. So all check but only the last
					//finishes the job set update.
					unsigned uMasterJob=0;//first job is master job
					while((field=mysql_fetch_row(res)))
					{
						unsigned uNode=0;
						unsigned uDatacenter=0;
						sscanf(field[0],"%u",&uNode);
						sscanf(field[1],"%u",&uDatacenter);
						sprintf(gcQuery,"INSERT INTO tJob"
							" SET uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
							",cLabel='BlockAccess NOC blocked'"
							",cJobName='BlockAccess'"
							",uDatacenter=%u,uNode=%u"
							",uMasterJob=%u"
							",cJobData='cIPv4=%.15s;'"
							",uJobDate=UNIX_TIMESTAMP(NOW())"
							",uJobStatus=1",
								guCompany,
								guLoginClient,
								uDatacenter,
								uNode,
								uMasterJob,
								cIP);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							sprintf(cResult,"%.31s",mysql_error(&gMysql));
							break;
						}
						if(!uMasterJob)
							uMasterJob=mysql_insert_id(&gMysql);
						uCount++;
					}//while node
					sprintf(cResult,"%u jobs created",uCount);
					if(uCount)
					{
						sprintf(gcQuery,"UPDATE tIP"
								" SET uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
								",uFWStatus=%u"
								" WHERE uIP=%u",
									guLoginClient,
									uFWWAITINGBLOCK,
									uCtIP);
						mysql_query(&gMysql,gcQuery);
						sprintf(cFWStatusUpdated,"%.255s",ForeignKey("tFWStatus","cLabel",uFWWAITINGBLOCK));
						sprintf(gcQuery,"INSERT INTO tProperty"
								" SET uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
								",uOwner=1"
								",cName='NOC FW Activity'"
								",cValue=CONCAT(NOW(),' Group BlockAccess %s (%u/%u)')"
								",uType=31"
								",uKey=%u",
									gcUser,guCompany,guPermLevel,uCtIP);
						mysql_query(&gMysql,gcQuery);
					}
					unxsVZLog(uCtIP,"tIP","Group BlockAccess");
					break;
				}//Group BlockAccess

				//Group UndoBlockAccess
				else if(!strcmp(gcCommand,"Group UndoBlockAccess"))
				{
					MYSQL_RES *res;
					MYSQL_ROW field;
					char cIP[16]={""};
					sprintf(cIP,"%.15s",ForeignKey("tIP","cLabel",uCtIP));
					unsigned uAvailable=0;
					sscanf(ForeignKey("tIP","uAvailable",uCtIP),"%u",&uAvailable);
					unsigned uDatacenter=0;
					sscanf(ForeignKey("tIP","uDatacenter",uCtIP),"%u",&uDatacenter);
					unsigned uFWStatus=0;
					sscanf(ForeignKey("tIP","uFWStatus",uCtIP),"%u",&uFWStatus);
					if(guPermLevel<10)
					{
						sprintf(cResult,"insufficient permlevel");
						break;
					}
					if(!cIP[0])
					{
						sprintf(cResult,"data error");
						break;
					}
					if(uDatacenter!=41)
					{
						sprintf(cResult,"wrong datacenter");
						break;
					}
					if(uAvailable==1)
					{
						sprintf(cResult,"must not be available");
						break;
					}
					if(uFWStatus!=uFWBLOCKED)
					{
						sprintf(cResult,"incorrect status for op");
						break;
					}

					//Create an UndoBlockAccess tJob for each active tNode
					sprintf(gcQuery,"SELECT tNode.uNode,tDatacenter.uDatacenter FROM tNode,tDatacenter"
								" WHERE tDatacenter.uStatus=1 AND tNode.uStatus=1"
								" AND tDatacenter.cLabel!='CustomerPremise'"
								" AND tNode.uDatacenter=tDatacenter.uDatacenter");
					//			debug only
					//			" AND tNode.uDatacenter=tDatacenter.uDatacenter LIMIT 1");
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						sprintf(cResult,"%.31s",mysql_error(&gMysql));
						break;
					}
					res=mysql_store_result(&gMysql);
					unsigned uCount=0;
					unsigned uMasterJob=0;//first job is master job
					while((field=mysql_fetch_row(res)))
					{
						unsigned uNode=0;
						unsigned uDatacenter=0;
						sscanf(field[0],"%u",&uNode);
						sscanf(field[1],"%u",&uDatacenter);
						sprintf(gcQuery,"INSERT INTO tJob"
							" SET uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
							",cLabel='UndoBlockAccess NOC unblocked'"
							",cJobName='UndoBlockAccess'"
							",uDatacenter=%u,uNode=%u"
							",uMasterJob=%u"
							",cJobData='cIPv4=%.15s;'"
							",uJobDate=UNIX_TIMESTAMP(NOW())"
							",uJobStatus=1",
								guCompany,
								guLoginClient,
								uDatacenter,
								uNode,
								uMasterJob,
								cIP);
						mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
						{
							sprintf(cResult,"%.31s",mysql_error(&gMysql));
							break;
						}
						uCount++;
						if(!uMasterJob)
							uMasterJob=mysql_insert_id(&gMysql);
					}//while node
					sprintf(cResult,"%u jobs created",uCount);
					if(uCount)
					{
						sprintf(gcQuery,"UPDATE tIP"
								" SET uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
								",uFWStatus=%u"
								" WHERE uIP=%u",
									guLoginClient,
									uFWWAITINGACCESS,
									uCtIP);
						mysql_query(&gMysql,gcQuery);
						sprintf(cFWStatusUpdated,"%.255s",ForeignKey("tFWStatus","cLabel",uFWWAITINGACCESS));
						sprintf(gcQuery,"INSERT INTO tProperty"
								" SET uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
								",uOwner=1"
								",cName='NOC FW Activity'"
								",cValue=CONCAT(NOW(),' Group UndoBlockAccess %s (%u/%u)')"
								",uType=31"
								",uKey=%u",
									gcUser,guCompany,guPermLevel,uCtIP);
						mysql_query(&gMysql,gcQuery);
					}
					unxsVZLog(uCtIP,"tIP","Group UndoBlockAccess");
					break;
				}//Group UndoBlockAccess

				//Group Reputation Score
				else if(!strcmp(gcCommand,"Group Reputation Score"))
				{
					MYSQL_RES *res;
					MYSQL_ROW field;

					sprintf(gcQuery,"SELECT cValue"
							" FROM tProperty"
							" WHERE uKey=%u"
							" AND uType=31",uCtIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						sprintf(cResult,"%.31s",mysql_error(&gMysql));
						break;
					}
					res=mysql_store_result(&gMysql);
					unsigned uTotalDstIPCount=0;
					unsigned uDstIPCount;
					register unsigned uIDSIndex;
					unsigned uTotalIDSCount=0;
					unsigned uIDSCount[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
					char *cp=NULL;
					//Load max 32 from tConfiguration
					char cIDS[32][32]={"mon0.gip-plano-tx","mon1.gip-plano-tx",
								"monitor-fremont","monitor-atlanta","GIP","HE",""};
					while((field=mysql_fetch_row(res)))
					{
						if((cp=strstr(field[0],"uDstIPCount="))!=NULL)
						{
							uDstIPCount=0;
							sscanf(cp+strlen("uDstIPCount="),"%u",&uDstIPCount);
							uTotalDstIPCount+=uDstIPCount;
						}
						if((cp=strstr(field[0],"uMaxDstIPCount="))!=NULL)
						{
							uDstIPCount=0;
							sscanf(cp+strlen("uMaxDstIPCount="),"%u",&uDstIPCount);
							uTotalDstIPCount+=uDstIPCount;
						}
						if( (cp=strstr(field[0],"cIDS="))!=NULL &&
							*(cp+strlen("cIDS="))!=0)
						{
							for(uIDSIndex=0;uIDSIndex<31 && cIDS[uIDSIndex][0];uIDSIndex++)
							{
								if(!strncmp(cIDS[uIDSIndex],cp+strlen("cIDS="),strlen(cIDS[uIDSIndex])))
									uIDSCount[uIDSIndex]++;
							}
						}
					}
					for(uIDSIndex=0;uIDSIndex<31 && cIDS[uIDSIndex][0];uIDSIndex++)
						uTotalIDSCount+=uIDSCount[uIDSIndex];
					if(uTotalDstIPCount || uTotalIDSCount)
						sprintf(cResult,"%u (DstIPCount=%u IDSCount=%u)",
							uTotalDstIPCount*uTotalIDSCount,uTotalDstIPCount,uTotalIDSCount);
					else
						sprintf(cResult,"no score available");
					break;
				}//Group Reputation Score

				//Group CountryInfo
				else if(!strcmp(gcCommand,"Group CountryInfo"))
				{
					MYSQL_RES *res;
					MYSQL_ROW field;
					char cIP[16]={""};
					sprintf(cIP,"%.15s",ForeignKey("tIP","cLabel",uCtIP));
					if(!cIP[0])
					{
						sprintf(cResult,"data error");
						break;
					}

					sprintf(gcQuery,"SELECT cCountryCode,cCountryName,uGeoIPCountryCode"
							" FROM tGeoIPCountryCode"
							" WHERE uGeoIPCountryCode="
							"(SELECT uGeoIPCountryCode FROM tGeoIP WHERE uEndIP>=INET_ATON('%s') LIMIT 1)",cIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						sprintf(cResult,"%.31s",mysql_error(&gMysql));
						break;
					}
					res=mysql_store_result(&gMysql);
					if((field=mysql_fetch_row(res)))
					{
						sprintf(cResult,"%s(%s) %s",field[0],field[2],field[1]);
						sprintf(cCommentUpdated,"%.255s",ForeignKey("tIP","cComment",uCtIP));
						unsigned uGeoIPCountryCode=0;
						sscanf(field[2],"%u",&uGeoIPCountryCode);
						if(!strstr(cCommentUpdated," CN="))
						{
							sprintf(gcQuery,"UPDATE tIP"
								" SET uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
								",uCountryCode=%u"
								",cComment=CONCAT(cComment,' CN=%s;')"
								" WHERE uIP=%u",
									guLoginClient,
									uGeoIPCountryCode,
									field[1],
									uCtIP);
							mysql_query(&gMysql,gcQuery);
							sprintf(cCommentUpdated,"%.255s",ForeignKey("tIP","cComment",uCtIP));
						}
						break;
					}
					sprintf(cResult,"no country info available");
					break;
				}//Group CountryInfo

				//Group FixWaiting
				else if(!strcmp(gcCommand,"Group FixWaiting"))
				{
					char cIP[16]={""};
					sprintf(cIP,"%.15s",ForeignKey("tIP","cLabel",uCtIP));
					unsigned uAvailable=0;
					sscanf(ForeignKey("tIP","uAvailable",uCtIP),"%u",&uAvailable);
					unsigned uDatacenter=0;
					sscanf(ForeignKey("tIP","uDatacenter",uCtIP),"%u",&uDatacenter);
					unsigned uFWStatus=0;
					sscanf(ForeignKey("tIP","uFWStatus",uCtIP),"%u",&uFWStatus);
					if(guPermLevel<10)
					{
						sprintf(cResult,"insufficient permlevel");
						break;
					}
					if(!cIP[0])
					{
						sprintf(cResult,"data error");
						break;
					}
					if(uDatacenter!=41)
					{
						sprintf(cResult,"wrong datacenter");
						break;
					}
					if(uAvailable==1)
					{
						sprintf(cResult,"must not be available");
						break;
					}
					if(uFWStatus!=uFWWAITINGACCESS && uFWStatus!=uFWWAITINGBLOCK)
					{
						sprintf(cResult,"incorrect status for op");
						break;
					}

					unsigned uUpdateStatus=uFWBLOCKED;
					if(uFWStatus==uFWWAITINGACCESS)
						uUpdateStatus=uFWACCESS;
					sprintf(gcQuery,"UPDATE tIP"
								" SET uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
								",uFWStatus=%u"
								" WHERE uIP=%u",
									guLoginClient,
									uUpdateStatus,
									uCtIP);
					mysql_query(&gMysql,gcQuery);
					sprintf(cFWStatusUpdated,"%.255s",ForeignKey("tFWStatus","cLabel",uUpdateStatus));
					sprintf(gcQuery,"INSERT INTO tProperty"
								" SET uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
								",uOwner=1"
								",cName='NOC FW Activity'"
								",cValue=CONCAT(NOW(),' Group FixWaiting %s (%u/%u)')"
								",uType=31"
								",uKey=%u",
									gcUser,guCompany,guPermLevel,uCtIP);
					mysql_query(&gMysql,gcQuery);
					unxsVZLog(uCtIP,"tIP","Group FixWaiting");
					sprintf(cResult,"fixed");
					break;
				}//Group FixWaiting

				//Group Whitelist
				else if(!strcmp(gcCommand,"Group Whitelist"))
				{
					char cIP[16]={""};
					sprintf(cIP,"%.15s",ForeignKey("tIP","cLabel",uCtIP));
					unsigned uAvailable=0;
					sscanf(ForeignKey("tIP","uAvailable",uCtIP),"%u",&uAvailable);
					unsigned uDatacenter=0;
					sscanf(ForeignKey("tIP","uDatacenter",uCtIP),"%u",&uDatacenter);
					unsigned uFWStatus=0;
					sscanf(ForeignKey("tIP","uFWStatus",uCtIP),"%u",&uFWStatus);
					if(guPermLevel<10)
					{
						sprintf(cResult,"insufficient permlevel");
						break;
					}
					if(!cIP[0])
					{
						sprintf(cResult,"data error");
						break;
					}
					if(uDatacenter!=41)
					{
						sprintf(cResult,"wrong datacenter");
						break;
					}
					if(uAvailable==1)
					{
						sprintf(cResult,"must not be available");
						break;
					}
					if( uFWStatus && uFWStatus!=uFWACCESS)
					{
						sprintf(cResult,"incorrect status for op");
						break;
					}
					sprintf(gcQuery,"UPDATE tIP"
								" SET uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
								",uFWStatus=%u"
								" WHERE uIP=%u",
									guLoginClient,
									uFWWHITELISTED,
									uCtIP);
					mysql_query(&gMysql,gcQuery);
					sprintf(cFWStatusUpdated,"%.255s",ForeignKey("tFWStatus","cLabel",uFWWHITELISTED));
					sprintf(gcQuery,"INSERT INTO tProperty"
								" SET uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
								",uOwner=1"
								",cName='NOC FW Activity'"
								",cValue=CONCAT(NOW(),' Group Whitelist %s (%u/%u)')"
								",uType=31"
								",uKey=%u",
									gcUser,guCompany,guPermLevel,uCtIP);
					mysql_query(&gMysql,gcQuery);
					unxsVZLog(uCtIP,"tIP","Group Whitelist");
					break;
				}//Group Whitelist

				else if(1)
				{
					sprintf(cResult,"Unexpected gcCommand=%.64s",gcCommand);
					break;
				}
			}//end if Ct block
		}//end for()
	}

	printf("<tr");
	if((uRow++) % 2)
		printf(" bgcolor=#E7F3F1 ");
	else
		printf(" bgcolor=#EFE7CF ");
	printf(">");


/*
			0		" tIP.uIP,"
			1		" tIP.cLabel,"
					" IF(tIP.uAvailable>0,'Yes','No'),"
					" IFNULL(tDatacenter.cLabel,''),"
			2		" tClient.cLabel,"
			3		" FROM_UNIXTIME(tIP.uCreatedDate,'%%a %%b %%d %%T %%Y'),"
			4		" FROM_UNIXTIME(tIP.uModDate,'%%a %%b %%d %%T %%Y'),"
			5		" tIP.uFWStatus,"
			6		" tIP.uFWRule,"
			7		" tIP.uCountryCode"
			8		" tIP.cComment,"
*/
	if(uFirewallMode)
	{
		//Allow instant feedback like cResult
		if(!cCommentUpdated[0])
			sprintf(cCommentUpdated,"%.255s",field[8]);
		if(!cFWStatusUpdated[0])
			sprintf(cFWStatusUpdated,"%.255s",field[5]);
		printf("<td width=120 valign=top>"
		"<input type=checkbox name=Ct%s >" //0
		"<a class=darkLink href=unxsVZ.cgi?gcFunction=tIP&uIP=%s>%s</a>" //0 and 1
		"</td>"
		//"<td>%s</td>" //2
		//"<td>%s</td>" //3
		"<td>%s</td>" //4
		"<td>%s</td>" //5
		"<td>%s</td>" //6
		"<td>%s</td>" //7
		"<td>%s</td>" //8
		"<td>%s</td>" //9
		"<td>%s</td>" //cComment
		"<td>%s</td>\n", //cResult
			field[0],//uIP
			field[0],field[1],//uIP,cLabel
			//field[2],//Available Yes/No
			//field[3],//Datacenter
			field[2],//modby
			field[3],//date
			field[4],//date
			//field[n],//status
			cFWStatusUpdated,//status
			field[6],//rule
			field[7],//country code
				cCommentUpdated,
				cResult);
	}
	else
	{
		//Allow instant feedback like cResult
		if(!cCommentUpdated[0])
			sprintf(cCommentUpdated,"%.255s",field[9]);
		char cContainerURL[256]={""};
		if(field[5][0])
			sprintf(cContainerURL,"<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s</a>",
					field[10],field[5]);

		printf("<td width=200 valign=top>"
		"<input type=checkbox name=Ct%s >" //0
		"<a class=darkLink href=unxsVZ.cgi?gcFunction=tIP&uIP=%s>%s</a>" //0 and 1
		"</td>"
		"<td>%s</td>" //2
		"<td>%s</td>" //3
		"<td>%s</td>" //4
		"<td>%s</td>" //5
		"<td>%s</td>" //6
		"<td>%s</td>" //7
		"<td>%s</td>" //8
		"<td>%s</td>" //8
		"<td>%s</td>\n", //cResult
			field[0],field[0],field[1],field[2],field[3],field[4],
					cContainerURL,//5 with 9
			field[6],field[7],field[8],cCommentUpdated,cResult);
	}
	printf("</tr>");

}//while()
//Reset margin end

			printf("<tr><td><input type=checkbox name=all onClick='checkAll(document.formMain,this)'>"
					"Check all %u IPs</td></tr>\n",uNumRows);
			printf("</table>");

			}//If results
			mysql_free_result(res);
			CloseFieldSet();
		break;

		default:
			if( !uIP || guMode!=6) return;

			sprintf(gcQuery,"%s Property Panel",cLabel);
			OpenFieldSet(gcQuery,100);
			//UBC safe
			sprintf(gcQuery,"SELECT uProperty,cName,cValue"
					" FROM tProperty WHERE uKey=%u AND uType=31"
					" ORDER BY uProperty DESC",uIP);
		        mysql_query(&gMysql,gcQuery);
		        if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		        res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res))
			{
				printf("<table>");
				while((field=mysql_fetch_row(res)))
				{
					printf("<tr>");
					printf("<td width=200 valign=top><a class=darkLink href=unxsVZ.cgi?"
					"gcFunction=tProperty&uProperty=%s&cReturn=tIP_%u>"
					"%s</a></td><td>%s</td>\n",
						field[0],uIP,field[1],field[2]);
					printf("</tr>");
				}
				printf("</table>");
			}
			mysql_free_result(res);
			CloseFieldSet();

	}//switch(guMode)

}//void ExttContainerAuxTable(void)



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


void tIPReport(char *cLabel, unsigned uAux)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(!cLabel[0])
	{
		sprintf(gcQuery,"SELECT tIP.uIP,tIP.cLabel,tContainer.cLabel,tContainer.uContainer"
				" FROM tIP,tContainer WHERE tIP.uIP=tContainer.uIPv4 AND tIP.uIP IN"
				" (SELECT tIP.uIP FROM tIP,tContainer"
				" WHERE tIP.uIP=tContainer.uIPv4"
				" AND tIP.uDatacenter=%u"
				" GROUP BY (tIP.uIP) HAVING COUNT(*)>1)"
				" ORDER BY tIP.uIP",uAux);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
        	{
        		printf("<p><u>tIPReport Duplicate Use</u><br>\n");
			printf("%s",mysql_error(&gMysql));
			return;
        	}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res))
		{	
        		printf("<p><u>tIPReport Duplicate Use (uDatacenter=%u)</u><br>\n",uAux);
			while((field=mysql_fetch_row(res)))
			{
				printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tIP"
					"&uIP=%s>%s</a>"
					" used by <a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s</a><br>\n",
						field[0],field[1],
						field[3],field[2]);
			}
		}
		mysql_free_result(res);

	}
	else
	{

	sprintf(gcQuery,"SELECT uContainer,cHostname FROM tContainer WHERE uIPv4=%u",uAux);
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

	sprintf(gcQuery,"SELECT tProperty.uKey,tContainer.cLabel,tProperty.cName,tProperty.cValue FROM tProperty,tContainer"
				" WHERE tProperty.uType=3 AND tProperty.cValue LIKE '%%%s%%'"
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
					"&uContainer=%s>%s used by %s %s=%s</a><br>\n",field[0],cLabel,field[1],field[2],field[3]);
	        }
	}
        mysql_free_result(res);

	sprintf(gcQuery,"SELECT tProperty.uKey,tNode.cLabel,tProperty.cName,tProperty.cValue FROM tProperty,tNode"
				" WHERE tProperty.uType=2 AND tProperty.cValue LIKE '%%%s%%'"
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
					"&uNode=%s>%s used by %s %s=%s</a><br>\n",field[0],cLabel,field[1],field[2],field[3]);
	        }
	}
        mysql_free_result(res);

	sprintf(gcQuery,"SELECT tProperty.uKey,tDatacenter.cLabel,tProperty.cName,tProperty.cValue FROM tProperty,tDatacenter"
				" WHERE tProperty.uType=1 AND tProperty.cValue LIKE '%%%s%%'"
				" AND tProperty.uKey=tDatacenter.uDatacenter",cLabel);
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
        	printf("<p><u>tIPReport tDatacenter</u><br>\n");


	        while((field=mysql_fetch_row(res)))
		{
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tDatacenter"
					"&uDatacenter=%s>%s used by %s %s=%s</a><br>\n",field[0],cLabel,field[1],field[2],field[3]);
	        }
	}
        mysql_free_result(res);

	}//if cLabel[0]

}//void tIPReport()


void AddIPRange(char *cIPRange)
{
	char *cIPs[2048];//Warning max 4 class C's or /22
	unsigned uCIDR4Mask=0;
	unsigned uCIDR4IP=0;
	register int i,uSkip=0;
	unsigned uAvailable,uD;
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
		
		//auto reserve network, gw and broadcast IPs for class C
		//10.5.0.0 
		uAvailable=1;
		if(sscanf(cIPs[i],"%*u.%*u.%*u.%u",&uD)==1)
		{
			if(uD==0 || uD==1 || uD==255)
				uAvailable=0;
		}
		sprintf(gcQuery,"INSERT INTO tIP SET cLabel='%s',uIPNum=INET_ATON('%s'),uOwner=%u,uCreatedBy=%u,uAvailable=%u"
				",uCreatedDate=UNIX_TIMESTAMP(NOW()),uDatacenter=%u",
					cIPs[i],cIPs[i],guCompany,guLoginClient,uAvailable,uDatacenter);
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


void tIPFirewallSubnetReport()
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_ROW field2;

	//tIP.uCountryCode=tGeoIPCountryCode.uGeoIPCountryCode
	sprintf(gcQuery,"SELECT DISTINCT LEFT(tIP.cLabel,LENGTH(tIP.cLabel)-LENGTH(SUBSTRING_INDEX(tIP.cLabel,'.',-1))-1),"
			" tGeoIPCountryCode.cCountryCode FROM tIP,tDatacenter,tGeoIPCountryCode"
			" WHERE tIP.uDatacenter=tDatacenter.uDatacenter"
			" AND tDatacenter.cLabel='CustomerPremise'"
			" AND tIP.uCountryCode=tGeoIPCountryCode.uGeoIPCountryCode"
			" AND tIP.uFWStatus>0"
			" AND tIP.uIPNum>0"
			" AND tGeoIPCountryCode.cCountryCode!='US'"
			" ORDER BY tGeoIPCountryCode.uGeoIPCountryCode,tIP.uIPNum");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tIPFirewallSubnetReport</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tIPFirewallSubnetReport</u><br>\n");
	        while((field=mysql_fetch_row(res)))
		{
			printf("%s.0/24 %s<br>\n",field[0],field[1]);
			//sprintf(gcQuery,"SELECT COUNT(uIP)"
			sprintf(gcQuery,"SELECT cLabel"
				" FROM tIP"
				" WHERE uIPNum>INET_ATON('%s.0') AND uIPNum<INET_ATON('%s.0')+255",field[0],field[0]);
        		mysql_query(&gMysql,gcQuery);
        		if(mysql_errno(&gMysql))
        		{
        			printf("<p><u>tIPFirewallSubnetReport</u><br>\n");
                		printf("%s",mysql_error(&gMysql));
                		return;
        		}
        		res2=mysql_store_result(&gMysql);
	        	while((field2=mysql_fetch_row(res2)))
        			printf(" &nbsp; %s <br>\n",field2[0]);
        		mysql_free_result(res2);
	        }
	}
	else
	{
        	printf("<p><u>tIPFirewallSubnetReport no rows!</u><br>\n");
	}
        mysql_free_result(res);

}//void tIPFirewallSubnetReport()


void tDatacenterIPReport()
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	//LEFT(tIP.cLabel,LENGTH(tIP.cLabel)-LENGTH(SUBSTRING_INDEX(tIP.cLabel,'.',-1))-1)
	sprintf(gcQuery,"SELECT DISTINCT LEFT(tIP.cLabel,LENGTH(tIP.cLabel)-LENGTH(SUBSTRING_INDEX(tIP.cLabel,'.',-1))-1),"
			" tDatacenter.cLabel FROM tIP,tDatacenter"
			" WHERE tIP.uDatacenter=tDatacenter.uDatacenter"
			" AND tDatacenter.cLabel!='CustomerPremise'"
			" AND tIP.cLabel NOT LIKE '10.%%.%%.%%' AND"
			" tIP.cLabel NOT LIKE '172.16.%%.%%' AND" //This is only the first class C of the /12
			" tIP.cLabel NOT LIKE '172.17.%%.%%' AND" 
			" tIP.cLabel NOT LIKE '172.18.%%.%%' AND"
			" tIP.cLabel NOT LIKE '192.168.%%.%%'"
			" AND tDatacenter.uStatus=1"
			" ORDER BY tIP.uDatacenter,tIP.uIP");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tDatacenterIPReport</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tDatacenterIPReport (excluding CustomerPremise)</u><br>\n");
	        while((field=mysql_fetch_row(res)))
		{
			printf("%s.0/24 %s<br>\n",field[0],field[1]);
	        }
	}
        mysql_free_result(res);

}//void tDatacenterIPReport()
