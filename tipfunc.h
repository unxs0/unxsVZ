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
						&& uOwnerSearch==0 && uIPv4Exclude==0 && cCommentSearch[0]==0)
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
		else if(!strcmp(gcCommand,"Add to Search Set") || !strcmp(gcCommand,"Create Search Set"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstIPVars(entries,x);
                        	guMode=12002;
				char cQuerySection[256];
				unsigned uLink=0;
				unsigned uGroup=0;

				if(cIPv4Search[0]==0 && uDatacenterSearch==0 && uNodeSearch==0 && uNodeSearchNot==0 && uAvailableSearch==0
						&& uOwnerSearch==0 && uIPv4Exclude==0 && cCommentSearch[0]==0)
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
					if(!strcmp(gcCommand,"Create Search Set"))
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
			printf("<input type=submit class=largeButton title='Add the results to your current search set. Do not add the same search"
				" over and over again it will not result in any change but may slow down processing.'"
				" name=gcCommand value='Add to Search Set'>");
			printf("<p><input type=submit class=largeButton title='Apply the right panel filter to refine your existing search set"
				" by removing set elements that match the filter settings.'"
				" name=gcCommand value='Remove from Search Set'>\n");
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
			printf("<p><input type=submit class=largeButton title='Open user search set page. There you can create search sets and operate"
				" on selected containers of the loaded container set.'"
				" name=gcCommand value='Search Set Operations'>\n");
			printf("<p><input type=submit class=largeButton title='Create report on IP usage and errors for the loaded IP'"
				" name=gcCommand value='IP Report Single'>\n");
			printf("<p><input type=submit class=largeButton title='!Warning make take some time. Do not abuse!"
				" Create report on assignment errors for all IPs of given datacenter'"
				" name=gcCommand value='IP Report All'>\n");
			printf("<p><input type=submit class=largeButton title='Non rfc-1918 class C's in use by active datacenters'"
				" name=gcCommand value='Datacenter IP Report'>\n");

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
			CloseFieldSet();

			sprintf(gcQuery,"Search Set Contents");
			OpenFieldSet(gcQuery,100);
			uGroup=uGetSearchGroup(gcUser,31);
			sprintf(gcQuery,"SELECT"
					" tIP.uIP,"
					" tIP.cLabel,"
					" IF(tIP.uAvailable>0,'Yes','No'),"
					" IFNULL(tDatacenter.cLabel,''),"
					" IFNULL(tNode.cLabel,''),"
					" IFNULL(tContainer.cHostname,''),"
					" tClient.cLabel,"
					" FROM_UNIXTIME(tIP.uModDate,'%%a %%b %%d %%T %%Y'),"
					" tIP.cComment"
					" FROM tIP"
					" LEFT JOIN tContainer ON tContainer.uIPv4=tIP.uIP"
					" LEFT JOIN tDatacenter ON tIP.uDatacenter=tDatacenter.uDatacenter"
					" LEFT JOIN tNode ON tContainer.uNode=tNode.uNode"
					" LEFT JOIN tClient ON tIP.uOwner=tClient.uClient"
					" WHERE uIP IN (SELECT uIP FROM tGroupGlue WHERE uGroup=%u)",uGroup);
		        mysql_query(&gMysql,gcQuery);
		        if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		        res=mysql_store_result(&gMysql);
			if((uNumRows=mysql_num_rows(res)))
			{
				char cResult[100]={""};
				char cCtLabel[100]={""};

				printf("<table>");
				printf("<tr>");
				printf("<td><input type=checkbox name=all onClick='checkAll(document.formMain,this)'> <u>cLabel</u></td>"
					"<td><u>Available</u></td>"
					"<td><u>Datacenter</u></td>"
					"<td><u>Node</u></td>"
					"<td><u>Hostname</u></td>"
					"<td><u>Owner</u></td>"
					"<td><u>ModifiedDate</u></td>"
					"<td><u>Comment</u></td>"
					"<td><u>Set operation result</u></td></tr>");
//Reset margin start
while((field=mysql_fetch_row(res)))
{
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
						sprintf(cResult,"make not available done");
					else
						cResult[0]=0;
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
	"<td>%s</td>\n", //cResult
		field[0],field[0],field[1],field[2],field[3],field[4],field[5],field[6],field[7],field[8],cResult);
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
			;

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
		sprintf(gcQuery,"INSERT INTO tIP SET cLabel='%s',uOwner=%u,uCreatedBy=%u,uAvailable=%u"
				",uCreatedDate=UNIX_TIMESTAMP(NOW()),uDatacenter=%u",
					cIPs[i],guCompany,guLoginClient,uAvailable,uDatacenter);
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


void tDatacenterIPReport()
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	//LEFT(tIP.cLabel,LENGTH(tIP.cLabel)-LENGTH(SUBSTRING_INDEX(tIP.cLabel,'.',-1))-1)
	sprintf(gcQuery,"SELECT DISTINCT LEFT(tIP.cLabel,LENGTH(tIP.cLabel)-LENGTH(SUBSTRING_INDEX(tIP.cLabel,'.',-1))-1),"
			" tDatacenter.cLabel FROM tIP,tDatacenter"
			" WHERE tIP.uDatacenter=tDatacenter.uDatacenter"
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
        	printf("<p><u>tDatacenterIPReport</u><br>\n");
	        while((field=mysql_fetch_row(res)))
		{
			printf("%s.0/24 %s<br>\n",field[0],field[1]);
	        }
	}
        mysql_free_result(res);

}//void tDatacenterIPReport()
