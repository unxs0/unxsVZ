/*
FILE
	$Id: tdidfunc.h 2032 2012-08-10 21:05:15Z Dylan $
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR/LEGAL
	(C) 2001-2010 Gary Wallis.
	GPLv2 license applies. See LICENSE file included.
*/

#include <openisp/ucidr.h>

void tDIDReport(void);
void tDIDNavList(unsigned uAvailable);
void tDIDUsedButAvailableNavList(void);
void tDIDUsedButAvailableFix(void);

//uForClient: Create for, on 'New;
static unsigned uForClient=0;
static char cForClientPullDown[256]={""};
static char cDIDRange[32]={""};
static char cSearch[32]={""};

void AddDIDRange(char *cDIDRange);
void DelDIDRange(char *cDIDRange);

//extern
unsigned uGetSearchGroup(const char *gcUser,unsigned uGroupType);

void ExtProcesstDIDVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cDIDRange"))
			sprintf(cDIDRange,"%.31s",entries[i].val);
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
}//void ExtProcesstDIDVars(pentry entries[], int x)


void ExttDIDCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tDIDTools"))
	{
        	MYSQL_RES *res;

                if(!strncmp(gcCommand,"Set ",4) || !strncmp(gcCommand,"Group ",6) || !strncmp(gcCommand,"Delete Checked",14))
                {
			ProcesstDIDVars(entries,x);
                        guMode=12002;
			tDID(gcCommand);
		}
		else if(!strcmp(gcCommand,"Reload Search Set"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstDIDVars(entries,x);
                        	guMode=12002;
	                        tDID("Search set reloaded");
			}
			else
			{
				tDID("<blink>Error:</blink> Denied by permissions settings");
			}
		}
		else if(!strcmp(gcCommand,"Remove from Search Set"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstDIDVars(entries,x);
                        	guMode=12002;
				char cQuerySection[256];
				unsigned uLink=0;
				unsigned uGroup=0;

				if(cDIDv4Search[0]==0 && uDatacenterSearch==0 && uServerSearch==0 && uServerSearchNot==0 && uAvailableSearch==0
						&& uOwnerSearch==0 && uDIDv4Exclude==0)
	                        	tDID("You must specify at least one search parameter");

				if((uGroup=uGetSearchGroup(gcUser,31))==0)
		                        tDID("No search set exists. Please create one first.");

				//Initial query section
				sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroup=%u AND uDID IN"
						" (SELECT uDID FROM tDID WHERE",uGroup);

				if(cDIDv4Search[0])
				{
					sprintf(cQuerySection," uDID IN (SELECT uDID FROM tDID WHERE cLabel LIKE '%s%%')",cDIDv4Search);
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

				if(uServerSearch || uServerSearchNot)
				{
					if(uLink)
						strcat(gcQuery," AND");
					if(uServerSearchNot && uServerSearch )
						sprintf(cQuerySection," uDID IN (SELECT uDIDv4 FROM tContainer WHERE uServer!=%u)",uServerSearch);
					else if(uServerSearch)
						sprintf(cQuerySection," uDID IN (SELECT uDIDv4 FROM tContainer WHERE uServer=%u)",uServerSearch);
					else
						sprintf(cQuerySection," uDID IN"
								" (SELECT uDID FROM tDID LEFT JOIN tContainer ON tContainer.uDIDv4=tDID.uDID"
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

				if(uDIDv4Exclude)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," cLabel NOT LIKE '10.%%.%%.%%' AND"
								" cLabel NOT LIKE '172.16.%%.%%' AND" //This is only the first class C of the /12
								" cLabel NOT LIKE '192.168.%%.%%'");
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				strcat(gcQuery,")");
				//debug only
	                        //tDID(gcQuery);

				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				unsigned uNumber=0;
				if((uNumber=mysql_affected_rows(&gMysql))>0)
				{
	                        	sprintf(gcQuery,"%u tDID records were removed from your search set",uNumber);
	                        	tDID(gcQuery);
				}
				else
				{
	                        	tDID("No records were removed from your search set");
				}
			}
			else
			{
				tDID("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,"Add to Search Set") || !strcmp(gcCommand,"Create Search Set"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstDIDVars(entries,x);
                        	guMode=12002;
				char cQuerySection[256];
				unsigned uLink=0;
				unsigned uGroup=0;

				if(cDIDv4Search[0]==0 && uDatacenterSearch==0 && uServerSearch==0 && uServerSearchNot==0 && uAvailableSearch==0
						&& uOwnerSearch==0 && uDIDv4Exclude==0)
	                        	tDID("You must specify at least one search parameter");

				if((uGroup=uGetSearchGroup(gcUser,31))==0)
				{
					sprintf(gcQuery,"INSERT INTO tGroup SET cLabel='%s',uGroupType=31"//2 is search group
						",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							gcUser,guCompany,guLoginClient);//2=search set type TODO
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					if((uGroup=mysql_insert_id(&gMysql))==0)
		                        	tDID("An error ocurred when attempting to create your search set");
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
				sprintf(gcQuery,"INSERT INTO tGroupGlue (uGroup,uDID)"
						" SELECT %u,uDID FROM tDID WHERE",uGroup);

				//Build AND query section

				if(cDIDv4Search[0])
				{
					sprintf(cQuerySection," uDID IN (SELECT uDID FROM tDID WHERE cLabel LIKE '%s%%')",cDIDv4Search);
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

				if(uServerSearch || uServerSearchNot)
				{
					if(uLink)
						strcat(gcQuery," AND");
					if(uServerSearchNot && uServerSearch )
						sprintf(cQuerySection," uDID IN (SELECT uDIDv4 FROM tContainer WHERE uServer!=%u)",uServerSearch);
					else if(uServerSearch)
						sprintf(cQuerySection," uDID IN (SELECT uDIDv4 FROM tContainer WHERE uServer=%u)",uServerSearch);
					else
						sprintf(cQuerySection," uDID IN"
								" (SELECT uDID FROM tDID LEFT JOIN tContainer ON tContainer.uDIDv4=tDID.uDID"
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

				if(uDIDv4Exclude)
				{
					if(uLink)
						strcat(gcQuery," AND");
					sprintf(cQuerySection," cLabel NOT LIKE '10.%%.%%.%%' AND"
								" cLabel NOT LIKE '172.16.%%.%%' AND" //This is only the first class C of the /12
								" cLabel NOT LIKE '192.168.%%.%%'");
					strcat(gcQuery,cQuerySection);
					uLink=1;
				}

				//debug only
	                        //tDID(gcQuery);

				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				unsigned uNumber=0;
				if((uNumber=mysql_affected_rows(&gMysql))>0)
				{
	                        	sprintf(gcQuery,"%u tDID records were added to your search set",uNumber);
	                        	tDID(gcQuery);
				}
				else
				{
	                        	tDID("No records were added to your search set. Filter returned 0 records");
				}
			}
			else
			{
				tDID("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,"Search Set Operations"))
                {
			if(guPermLevel>=9)
			{
                        	guMode=12001;
	                        tDID("Search Set Operations");
			}
			else
			{
				tDID("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstDIDVars(entries,x);
                        	guMode=2000;
	                        tDID(LANG_NB_CONFIRMNEW);
			}
			else
			{
				tDID("Not allowed by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstDIDVars(entries,x);

                        	guMode=2000;
				//Check entries here
				//1.2.3.4 min
				if(strlen(cLabel)<7)
					tDID("cLabel too short");
				if(!uDatacenter)
					tDID("Must specify a uDatacenter");
                        	guMode=0;

				uDID=0;
				uCreatedBy=guLoginClient;
				if(uForClient) guCompany=uForClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtDID(0);
			}
			else
			{
				tDID("Not allowed by permissions settings");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstDIDVars(entries,x);
			if((uAllowDel(uOwner,uCreatedBy) && uAvailable) || guPermLevel>=11)
			{
	                        guMode=0;
				sprintf(gcQuery,"SELECT uDIDv4 FROM tContainer WHERE uDIDv4=%u",uDID);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tDID("Can't delete an DID in use");
				}
	                        guMode=2001;
				tDID(LANG_NB_CONFIRMDEL);
			}
			else
			{
				tDID("Not allowed by permissions settings");
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstDIDVars(entries,x);
			if((uAllowDel(uOwner,uCreatedBy) && uAvailable) || guPermLevel>=11)
			{
				guMode=5;
				if(cDIDRange[0])
				{
					if(uForClient) guCompany=uForClient;
					DelDIDRange(cDIDRange);
				}
				else
				{
					sprintf(gcQuery,"SELECT uDIDv4 FROM tContainer WHERE uDIDv4=%u",uDID);
        				mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        				res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res))
					{
						mysql_free_result(res);
	                        		guMode=2001;
						tDID("Can't delete an DID in use");
					}
					DeletetDID();
				}
			}
			else
			{
				tDID("Not allowed by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstDIDVars(entries,x);
			if((uAllowMod(uOwner,uCreatedBy) && uAvailable) || guPermLevel>=11)
			{
	                        guMode=0;
				sprintf(gcQuery,"SELECT uDIDv4 FROM tContainer WHERE uDIDv4=%u",uDID);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tDID("Can't modify an DID in use");
				}
	                        guMode=2002;
				tDID(LANG_NB_CONFIRMMOD);
			}
			else
			{
				tDID("Not allowed by permissions settings");
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstDIDVars(entries,x);
			if((uAllowMod(uOwner,uCreatedBy)) || guPermLevel>=11)
			{
                        	guMode=2002;
				sprintf(gcQuery,"SELECT uDIDv4 FROM tContainer WHERE uDIDv4=%u",uDID);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tDID("Can't modify an DID in use");
				}
				if(strlen(cLabel)<7)
					tDID("cLabel too short");
				if(!uDatacenter)
					tDID("Must specify a uDatacenter");
                        	guMode=0;

				if(uForClient)
				{
					guCompany=uForClient;
					if(!cDIDRange[0])
					{
						uOwner=guCompany;
						sprintf(gcQuery,"UPDATE tDID SET uOwner=%u WHERE uAvailable=1 AND uDID=%u",
								guCompany,uDID);
        					mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
							htmlPlainTextError(mysql_error(&gMysql));
					}
				}
				if(cDIDRange[0])
					AddDIDRange(cDIDRange);
				uModBy=guLoginClient;
				ModtDID();
			}
			else
			{
				tDID("Not allowed by permissions settings");
			}
                }
                else if(!strcmp(gcCommand,"Fix"))
                {
                        ProcesstDIDVars(entries,x);
			if((uAllowMod(uOwner,uCreatedBy)) || guPermLevel>=11)
			{
				tDIDUsedButAvailableFix();
			}
			else
			{
				tDID("Not allowed by permissions settings");
			}
		}
	}

}//void ExttDIDCommands(pentry entries[], int x)


void ExttDIDButtons(void)
{
	OpenFieldSet("tDID Aux Panel",100);
	switch(guMode)
        {
                case 12001:
                case 12002:
			printf("<u>Create or refine your user search set</u><br>");
			printf("In the right panel you can select your search criteria. When refining you do not need"
				" to reuse your initial search critieria. Your search set is persistent even across unxsSPS sessions.<p>");
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
                        printf("<p>Here you can delete a single record or optionally a complete block of DIDs."
				" In the later case you specify a uOwner company to limit the deletion.");
			printf("<p>cDIDRange<br>\n");
			printf("<input title='Optionally enter CIDR DID Range (ex. 10.0.0.1/27) for available mass deletion'"
				" type=text name=cDIDRange><p>\n");
			tTablePullDownResellers(uForClient,1);
			if(cSearch[0])
				printf("<input type=hidden name=cSearch value='%s'>",cSearch);
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review changes</u><br>");
			printf("Change a single record or optionally add a block of available DIDs. In all cases you must"
				" specify a uDatacenter and optionally a new company (uOwner) if your"
				" permissions allow.<br>");
			printf("<p>cDIDRange<br>\n");
			printf("<input title='Optionally enter CIDR DID Range (ex. 10.0.0.1/27) for available mass addition'"
				" type=text name=cDIDRange value='%s'><p>\n",cDIDRange);
			tTablePullDownResellers(uForClient,1);
			if(cSearch[0])
				printf("<input type=hidden name=cSearch value='%s'>",cSearch);
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			printf("<p><u>Table Tips</u><br>");
			printf("DIDs used and available for container and new hardware server"
				" deployment are tracked and maintained here.<p>You can add available DIDs <i>en masse</i>"
				" via the cDIDRange at the [Modify] stage. For example 192.168.0.1/28 would add 16 DIDs"
				" starting at 192.168.0.1 and ending at 192.168.0.16."
				" To add an initial range use [New], then [Modify], enter new CIDR range."
				"<p>You can also delete available DIDs <i>en masse</i> via the cDIDRange at the [Delete]"
				" stage.");
			printf("<p><input type=submit class=largeButton title='Open user search set page. There you can create search sets and operate"
				" on selected containers of the loaded container set.'"
				" name=gcCommand value='Search Set Operations'>\n");

			printf("<p><u>Filter by cLabel</u><br>");
			printf("<input title='Enter cLabel start or MySQL LIKE pattern (%% or _ allowed)' type=text"
					" name=cSearch value='%s'>",cSearch);
			tDIDNavList(0);
	}
	CloseFieldSet();

}//void ExttDIDButtons(void)


void ExttDIDAuxTable(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uGroup=0;
	unsigned uNumRows=0;

	switch(guMode)
	{
		case 12001:
		case 12002:
			//Set operation buttons
			OpenFieldSet("Set Operations",100);
			printf("<input title='Delete checked DIDs from your search set. They will still be visible but will"
				" marked deleted and will not be used in any subsequent set operation'"
				" type=submit class=largeButton name=gcCommand value='Delete Checked'>\n");
			printf("&nbsp; <input title='Deletes DIDs not in use by servers.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Delete'>\n");
			printf("&nbsp; <input title='Updates DIDs to uAvailable=1 for DIDs not in use by containers.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Make Available'>\n");
			printf("&nbsp; <input title='Send one ping packet to DID. Check firewall settings use with care.'"
				" type=submit class=largeButton"
				" name=gcCommand value='Group Ping'>\n");
			CloseFieldSet();

			sprintf(gcQuery,"Search Set Contents");
			OpenFieldSet(gcQuery,100);
			uGroup=uGetSearchGroup(gcUser,31);
			sprintf(gcQuery,"SELECT"
					" tDID.uDID,"
					" tDID.cLabel,"
					" IF(tDID.uAvailable>0,'Yes','No'),"
					" IFNULL(tDatacenter.cLabel,''),"
					" IFNULL(tServer.cLabel,''),"
					" IFNULL(tContainer.cHostname,''),"
					" tClient.cLabel,"
					" FROM_UNIXTIME(tDID.uModDate,'%%a %%b %%d %%T %%Y'),"
					" tDID.cComment"
					" FROM tDID"
					" LEFT JOIN tContainer ON tContainer.uDIDv4=tDID.uDID"
					" LEFT JOIN tDatacenter ON tDID.uDatacenter=tDatacenter.uDatacenter"
					" LEFT JOIN tServer ON tContainer.uServer=tServer.uServer"
					" LEFT JOIN tClient ON tDID.uOwner=tClient.uClient"
					" WHERE uDID IN (SELECT uDID FROM tGroupGlue WHERE uGroup=%u)",uGroup);
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
		unsigned uCtDID=0;

		cResult[0]=0;
		sscanf(field[0],"%u",&uCtDID);
		sprintf(cCtLabel,"Ct%u",uCtDID);
		for(i=0;i<x;i++)
		{
			//insider xss protection
			if(guPermLevel<10)
				continue;

			if(!strcmp(entries[i].name,cCtLabel))
			{
				if(!strcmp(gcCommand,"Delete Checked"))
				{
					sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroup=%u AND uDID=%u",uGroup,uCtDID);
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

					sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroup=%u AND uDID=%u",uGroup,uCtDID);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));

					sprintf(gcQuery,"SELECT uDIDv4 FROM tContainer WHERE uDIDv4=%u",uCtDID);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        				res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)>0)
					{
						sprintf(cResult,"DID in use");
						break;
					}

					sprintf(gcQuery,"DELETE FROM tDID WHERE uDID=%u",uCtDID);
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

					sprintf(gcQuery,"SELECT uDIDv4 FROM tContainer WHERE uDIDv4=%u",uCtDID);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        				res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)>0)
					{
						sprintf(cResult,"DID in use");
						break;
					}

					sprintf(gcQuery,"UPDATE tDID SET uAvailable=1 WHERE uDID=%u AND uAvailable=0",uCtDID);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					if(mysql_affected_rows(&gMysql)>0)
						sprintf(cResult,"make available done");
					else
						cResult[0]=0;
					break;
				}//Group Make Available

				//Group Ping
				else if(!strcmp(gcCommand,"Group Ping"))
				{
					MYSQL_RES *res;
					MYSQL_ROW field;

					sprintf(gcQuery,"SELECT cLabel FROM tDID WHERE uDID=%u",uCtDID);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        				res=mysql_store_result(&gMysql);
	        			if((field=mysql_fetch_row(res)))
					{
						char cPingCommand[64];
						sprintf(cPingCommand,"/bin/ping -c 1 %.15s",field[0]);
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

				else if(1)
				{
					sprintf(cResult,"Unexpected gcCommand=%.64s",gcCommand);
					break;
				}
			}//end if Ct block
		}//end for()
	}

	printf("<tr>");
	printf("<td width=200 valign=top>"
	"<input type=checkbox name=Ct%s >" //0
	"<a class=darkLink href=unxsSPS.cgi?gcFunction=tDID&uDID=%s>%s</a>" //0 and 1
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
					"Check all %u DIDs</td></tr>\n",uNumRows);
			printf("</table>");

			}//If results
			mysql_free_result(res);
			CloseFieldSet();
		break;

		default:
			;

	}//switch(guMode)

}//void ExttContainerAuxTable(void)



void ExttDIDGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uDID"))
		{
			sscanf(gentries[i].val,"%u",&uDID);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.31s",gentries[i].val);
		}
	}
	tDID("");

}//void ExttDIDGetHook(entry gentries[], int x)


void ExttDIDSelect(void)
{
	if(cSearch[0])
		ExtSelectSearchPublic("tDID",VAR_LIST_tDID,"cLabel",cSearch);
	else
		ExtSelectPublic("tDID",VAR_LIST_tDID);

}//void ExttDIDSelect(void)


void ExttDIDSelectRow(void)
{
	ExtSelectRowPublic("tDID",VAR_LIST_tDID,uDID);

}//void ExttDIDSelectRow(void)


void ExttDIDListSelect(void)
{
	char cCat[512];

	ExtListSelectPublic("tDID",VAR_LIST_tDID);
	
	//Changes here must be reflected below in ExttDIDListFilter()
        if(!strcmp(gcFilter,"uDID"))
        {
                sscanf(gcCommand,"%u",&uDID);
		sprintf(cCat," WHERE tDID.uDID=%u ORDER BY uDID",uDID);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uDID");
        }

}//void ExttDIDListSelect(void)


void ExttDIDListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uDID"))
                printf("<option>uDID</option>");
        else
                printf("<option selected>uDID</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttDIDListFilter(void)


void ExttDIDNavBar(void)
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

}//void ExttDIDNavBar(void)


void tDIDNavList(unsigned uAvailable)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uNumRows=0;
	unsigned uMySQLNumRows=0;
#define uLIMIT 32

	if(guPermLevel>11)
	{
		if(cSearch[0])
			sprintf(gcQuery,"SELECT tDID.uDID,tDID.cLabel FROM"
				" tDID WHERE tDID.uAvailable=%u AND tDID.cLabel LIKE '%s%%' ORDER BY tDID.cLabel",uAvailable,cSearch);
		else
			sprintf(gcQuery,"SELECT tDID.uDID,tDID.cLabel FROM"
					" tDID WHERE tDID.uAvailable=%u ORDER BY tDID.cLabel",uAvailable);
	}
	else
	{
		if(cSearch[0])
			sprintf(gcQuery,"SELECT tDID.uDID,tDID.cLabel FROM"
					" tDID WHERE"
					" tDID.uAvailable=%u AND tDID.cLabel LIKE '%s%%' AND"
					" tDID.uOwner=%u ORDER BY tDID.cLabel",uAvailable,cSearch,guCompany);
		else
			sprintf(gcQuery,"SELECT tDID.uDID,tDID.cLabel FROM"
					" tDID WHERE"
					" tDID.uAvailable=%u AND tDID.uOwner=%u ORDER BY tDID.cLabel",
						uAvailable,guCompany);
	}

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tDIDNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if((uMySQLNumRows=mysql_num_rows(res)))
	{	
		if(uAvailable)
        		printf("<p><u>Available tDIDNavList(%u)</u><br>\n",uMySQLNumRows);
		else
        		printf("<p><u>Used tDIDNavList(%u)</u><br>\n",uMySQLNumRows);


	        while((field=mysql_fetch_row(res)))
		{

			if(cSearch[0])
				printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tDID&uDID=%s&cSearch=%s>%s"
						"</a><br>\n",field[0],cURLEncode(cSearch),field[1]);
			else
				printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tDID&uDID=%s>%s</a><br>\n",
							field[0],field[1]);
			if(++uNumRows>uLIMIT)
			{
				printf("(Only %u DIDs shown use search/filters to shorten list.)<br>\n",uLIMIT);
				break;
			}
		}
	}
        mysql_free_result(res);

}//void tDIDNavList(unsigned uAvailable)


void tDIDReport(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uContainer,cHostname FROM tContainer WHERE uDIDv4=%u",uDID);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tDIDReport</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tDIDReport tContainer</u><br>\n");


	        while((field=mysql_fetch_row(res)))
		{
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tContainer"
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
        	printf("<p><u>tDIDReport</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tDIDReport tContainer Extra DIDs</u><br>\n");


	        while((field=mysql_fetch_row(res)))
		{
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tContainer"
					"&uContainer=%s>%s used by %s</a><br>\n",field[0],cLabel,field[1]);
	        }
	}
        mysql_free_result(res);

	sprintf(gcQuery,"SELECT tProperty.uKey,tServer.cLabel FROM tProperty,tServer"
				" WHERE tProperty.uType=2 AND tProperty.cValue='%s'"
				" AND tProperty.uKey=tServer.uServer",cLabel);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tDIDReport</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tDIDReport tServer</u><br>\n");


	        while((field=mysql_fetch_row(res)))
		{
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tServer"
					"&uServer=%s>%s used by %s</a><br>\n",field[0],cLabel,field[1]);
	        }
	}
        mysql_free_result(res);

}//void tDIDReport(void)


void AddDIDRange(char *cDIDRange)
{

}//void AddDIDRange(char *cDIDRange)


void DelDIDRange(char *cDIDRange)
{

}//void DelDIDRange(char *cDIDRange)


void tDIDUsedButAvailableNavList(void)
{

}//void tDIDUsedButAvailableNavList()


void tDIDUsedButAvailableFix(void)
{

}//void tDIDUsedButAvailableFix()

