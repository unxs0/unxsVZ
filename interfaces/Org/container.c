/*
FILE 
	container.c
	$Id: container.c 1478 2010-05-25 02:43:12Z Gary $
AUTHOR/LEGAL
	(C) 2010, 2011 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	unxsvzOrg program file.
	Container tab functions.
*/

#include "interface.h"

extern unsigned guBrowserFirefox;//main.c
char gcCtHostname[100]={""};
static char gcSearch[100]={""};
unsigned guContainer=0;
unsigned guNewContainer=0;
//Container details
static char gcLabel[33]={""};
static char gcNewHostname[33]={""};


//TOC
void ProcessContainerVars(pentry entries[], int x);
void ContainerGetHook(entry gentries[],int x);
char *cGetHostname(unsigned uContainer);
char *cGetImageHost(unsigned uContainer);
void SelectContainer(void);
char *NameToLower(char *cInput);

void ProcessContainerVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"guContainer"))
			sscanf(entries[i].val,"%u",&guContainer);
		else if(!strcmp(entries[i].name,"gcCtHostname"))
			sprintf(gcCtHostname,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"gcSearch"))
			sprintf(gcSearch,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"guNewContainer"))
			sscanf(entries[i].val,"%u",&guNewContainer);
		else if(!strcmp(entries[i].name,"gcNewHostname"))
			sprintf(gcNewHostname,"%.32s",NameToLower(entries[i].val));
	}

}//void ProcessContainerVars(pentry entries[], int x)


void ContainerGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"guContainer"))
			sscanf(gentries[i].val,"%u",&guContainer);
	}

	if(guContainer)
	{
		SelectContainer();
		htmlContainer();
	}

	htmlContainer();

}//void ContainerGetHook(entry gentries[],int x)


void ContainerCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Container"))
	{
		ProcessContainerVars(entries,x);
		if(!strcmp(gcFunction,"Change Hostname"))
		{
        		MYSQL_RES *res;
	        	MYSQL_ROW field;
			unsigned uLen=0;

			//1-. Validate gcNewHostname
			if(strstr(gcNewHostname,"."))
			{
				gcMessage="New container name can not have anymore \"stops\" (periods.)";
				htmlContainer();
			}
			if((uLen=strlen(gcNewHostname))<2)
			{
				gcMessage="New container name must have at least two chars.";
				htmlContainer();
			}
			if(uLen>32)
			{
				gcMessage="New container name must have at most 32 chars.";
				htmlContainer();
			}
			//Check uniqueness
			sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE cLabel='%s'",gcNewHostname);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage=(char *)mysql_error(&gMysql);
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)>0)
			{
				gcMessage="New container name must not be already in use.";
				htmlContainer();
			}

			//Validate target container
			if(!guNewContainer)
			{
				gcMessage="Must select a container.";
				htmlContainer();
			}
			//Check job queue
			//Waiting, Running, Done Error, Remote waiting, Error
			sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uContainer=%u AND"
					" (uJobStatus=1 OR uJobStatus=2 OR uJobStatus=4 OR uJobStatus=10 OR uJobStatus=14)",
						guNewContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage=(char *)mysql_error(&gMysql);
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)>0)
			{
				gcMessage="Selected container is being used by another process. Please try another.";
				htmlContainer();
			}
			//uContainer must still exist
			sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uContainer=%u",guNewContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage=(char *)mysql_error(&gMysql);
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)!=1)
			{
				gcMessage="Selected container does not exist. Please try another.";
				htmlContainer();
			}
			//uStatus must still be active
			sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uContainer=%u AND uStatus!=1",guNewContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage=(char *)mysql_error(&gMysql);
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)>0)
			{
				gcMessage="Selected container is not active. Please try another.";
				htmlContainer();
			}

			//Check for configuration problems
			//1-. unxsBind zone and view
			//2-. Datacenter and node availability
			char cunxsBindARecordJobZone[65]={""};

			sprintf(gcQuery,"SELECT cValue FROM tConfiguration"
					" WHERE uDatacenter=(SELECT uDatacenter FROM tContainer WHERE uContainer=%u LIMIT 1)"
					" AND uContainer=0 AND uNode=0 AND cLabel='cunxsBindARecordJobZone'",guNewContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				gcMessage=(char *)mysql_error(&gMysql);
				htmlContainer();
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sprintf(cunxsBindARecordJobZone,"%.63s",field[0]);
			if(strlen(cunxsBindARecordJobZone)<2)
			{
				gcMessage="Configuration error contact your sysadmin: cunxsBindARecordJobZone.";
				htmlContainer();
			}

			//for all the above
			mysql_free_result(res);
				
			gcMessage="Hostname change being attempted. Review new container status in a few minutes.";
			guContainer=guNewContainer;
			htmlContainer();
		}
		htmlContainer();
	}

}//void ContainerCommands(pentry entries[], int x)


void htmlContainer(void)
{
	htmlHeader("unxsvzOrg","Header");
	if(guContainer)
		SelectContainer();
	htmlContainerPage("unxsvzOrg","Container.Body");
	htmlFooter("Footer");

}//void htmlContainer(void)


void htmlContainerPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uPLAINSET,uVDNSORGTYPE);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="unxsvzOrg.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcUser;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);//Safe?

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcMessage";
			template.cpValue[7]=gcMessage;

			template.cpName[8]="gcCtHostname";
			//template.cpValue[8]=gcCtHostname;
			template.cpValue[8]=(char *)cGetHostname(guContainer) ;

			template.cpName[9]="gcSearch";
			template.cpValue[9]=gcSearch;

			template.cpName[10]="guContainer";
			char cguContainer[16];
			sprintf(cguContainer,"%u",guContainer);
			template.cpValue[10]=cguContainer;

			template.cpName[11]="gcLabel";
			template.cpValue[11]=gcLabel;

			template.cpName[12]="cDisabled";
			if(guContainer)
				template.cpValue[12]="";
			else
				template.cpValue[12]="disabled";


			template.cpName[13]="";

			printf("\n<!-- Start htmlContainerPage(%s) -->\n",cTemplateName); 
			Template(field[0],&template,stdout);
			printf("\n<!-- End htmlContainerPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlContainerPage()


void funcContainerImageTag(FILE *fp)
{
	if(guContainer)
		fprintf(fp,"<a href=https://%s/admin ><img src=%s/traffic/%u.png border=0 ></a>",
			cGetHostname(guContainer),cGetImageHost(guContainer),guContainer);

}//void funcContainerImageTag(FILE *fp)


void funcSelectContainer(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCount=1;
	unsigned uContainer=0;

	fprintf(fp,"<!-- funcSelectContainer(fp) Start -->\n");

	if(gcSearch[0])
		sprintf(gcQuery,"SELECT uContainer,cHostname FROM tContainer WHERE "
			"uSource=0 AND uOwner=%u AND cHostname LIKE '%s%%' "
			"ORDER BY cHostname LIMIT 301",guOrg,gcSearch);
	else
		sprintf(gcQuery,"SELECT uContainer,cHostname FROM tContainer WHERE "
			"uOwner=%u AND uSource=0 ORDER BY cHostname LIMIT 301",guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	fprintf(fp,"<select class=type_textarea title='Select the container you want to load with this dropdown'"
			" name=guContainer onChange='submit()'>\n");
	fprintf(fp,"<option>---</option>");
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContainer);
		fprintf(fp,"<option value=%s",field[0]);
		if(guContainer==uContainer)
			fprintf(fp," selected");
		if((uCount++)<=300)
			fprintf(fp,">%s</option>",field[1]);
		else
			fprintf(fp,">Limit reached! Use better filter</option>");
	}
	mysql_free_result(res);

	fprintf(fp,"</select>\n");

	fprintf(fp,"<!-- funcSelectContainer(fp) End -->\n");

}//void funcSelectContainer(FILE *fp)


char *cGetHostname(unsigned uContainer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	static char cHostname[100]={""};

	sprintf(gcQuery,"SELECT cHostname FROM tContainer WHERE uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return((char *)mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cHostname,"%.99s",field[0]);
	return(cHostname);
}//char *cGetHostname(unsigned uContainer)


void SelectContainer(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cLabel FROM tContainer WHERE uContainer=%u"
			,guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcLabel,"%.32s",field[0]);

		if(!gcMessage[0]) gcMessage="Zone Selected";
	}
	else
	{
		gcLabel[0]=0;
		gcMessage="<blink>No Zone Selected</blink>";
	}
	
	mysql_free_result(res);
	
}//void SelectContainer(void)



void funcContainerInfo(FILE *fp)
{
	if(!guContainer)
		return;

	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcContainerInfo (fp) Start -->\n");

	//Datacenter
	sprintf(gcQuery,"SELECT tDatacenter.cLabel FROM tContainer,tDatacenter WHERE tContainer.uDatacenter=tDatacenter.uDatacenter"
			" AND tContainer.uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=uDatacenter')\">"
			" <strong>Datacenter</strong></a></td><td><input type=text name='cDatacenter' value='%s' size=40 maxlength=32"
			" class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
	mysql_free_result(res);

	//Node
	sprintf(gcQuery,"SELECT tNode.cLabel FROM tContainer,tNode WHERE tContainer.uNode=tNode.uNode"
			" AND tContainer.uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=uNode')\">"
			" <strong>Node</strong></a></td><td><input type=text name='cNode' value='%s' size=40 maxlength=32"
			" class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
	mysql_free_result(res);

	//Status
	sprintf(gcQuery,"SELECT tStatus.cLabel FROM tContainer,tStatus WHERE tContainer.uStatus=tStatus.uStatus"
			" AND tContainer.uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=uStatus')\">"
			" <strong>Status</strong></a></td><td><input type=text name='cStatus' value='%s' size=40 maxlength=32"
			" class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
	mysql_free_result(res);

	//IP
	sprintf(gcQuery,"SELECT tIP.cLabel FROM tContainer,tIP WHERE tIP.uIP=tContainer.uIPv4 AND uContainer=%u",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=uIPv4')\">"
			" <strong>IPv4</strong></a></td><td><input type=text name='cIPv4' value='%s' size=40 maxlength=32"
			" class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
	mysql_free_result(res);

	//Groups
	sprintf(gcQuery,"SELECT tGroup.cLabel FROM tGroup,tGroupGlue WHERE tGroup.uGroup=tGroupGlue.uGroup AND tGroupGlue.uContainer=%u",
		guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=Group')\">"
			" <strong>Group</strong></a></td><td><input type=text name='cGroup' value='%s' size=40 maxlength=32"
			" class=\"type_fields_off\"> </td></tr>\n",field[0]);
	}
	mysql_free_result(res);

	//SUBSTR based on 5 char cOrg_ prefix
	sprintf(gcQuery,"SELECT SUBSTR(cName,6),cValue FROM tProperty WHERE uType=3 AND uKey=%u AND cName LIKE 'cOrg_%%'"
			" ORDER BY cName",guContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("<tr><td><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=%s')\">"
			" <strong>%s</strong></a></td><td><input type=text name='%s' value='%s' size=40 maxlength=32"
			" class=\"type_fields_off\"> </td></tr>\n",field[0],field[0],field[0],field[1]);
	}
	mysql_free_result(res);

	fprintf(fp,"<!-- funcContainerInfo(fp) End -->\n");

}//void funcContainerInfo(FILE *fp)


char *cGetImageHost(unsigned uContainer)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	static char cHostname[100]={""};
	char cOrg_ImageNodeDomain[64]={""};

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uDatacenter=0"
			" AND uContainer=0 AND uNode=0 AND cLabel='cOrg_ImageNodeDomain'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return((char *)mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cOrg_ImageNodeDomain,"%.63s",field[0]);

	if(!cOrg_ImageNodeDomain[0])
		return(cHostname);

	sprintf(gcQuery,"SELECT tNode.cLabel FROM tContainer,tNode WHERE tContainer.uNode=tNode.uNode AND uContainer=%u",
		uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return((char *)mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cHostname,"https://%.32s.%.63s",field[0],cOrg_ImageNodeDomain);

	return(cHostname);

}//char *cGetImageHost(unsigned uContainer)


//Func for left panel "new" container "creation" 
//Safe and fast for non tech-users. Just select and provide new name.
//Special "pre-spinned stand-by" containers that can simply 
//be name changed for actual use.
//We use special reserved group name "Stand-by" to identify

void funcNewContainer(FILE *fp)
{
	if(guPermLevel<7)
		return;

	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uCount=1;
	unsigned uContainer=0;
	char cOrg_NewGroupLabel[33]={"Pre-Spinned"};
	char cOrg_NewHostname[65]={"somedomain.tld"};

	fprintf(fp,"<!-- funcNewContainer(fp) Start -->\n");

	fprintf(fp,"<tr><td valign=\"top\"><a class=inputLink href=\"#\" onClick=\"open_popup('unxsvzOrg.cgi?gcPage=Glossary&cLabel=Special+OPs')\" <strong><u>Special OPs</u></strong></a></td><td>\n");

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uDatacenter=0"
			" AND uContainer=0 AND uNode=0 AND cLabel='cOrg_NewGroupLabel'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlPlainTextError(mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cOrg_NewGroupLabel,"%.32s",field[0]);

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE uDatacenter=0"
			" AND uContainer=0 AND uNode=0 AND cLabel='cOrg_NewHostname'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlPlainTextError(mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cOrg_NewHostname,"%.64s",field[0]);

	sprintf(gcQuery,"SELECT tContainer.uContainer,tContainer.cHostname FROM tContainer,tGroupGlue,tGroup WHERE "
			"tContainer.uContainer=tGroupGlue.uContainer AND "
			"tGroupGlue.uGroup=tGroup.uGroup AND tGroup.cLabel='%s' AND "
			"tContainer.uOwner=%u AND tContainer.uSource=0 ORDER BY tContainer.cHostname LIMIT 301",
				cOrg_NewGroupLabel,guOrg);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		htmlPlainTextError(mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	fprintf(fp,"<br><select class=type_textarea title='Select the container you want to use.'"
			" name=guNewContainer >\n");
	fprintf(fp,"<option>---</option>");
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uContainer);
		fprintf(fp,"<option value=%s",field[0]);
		if(guNewContainer==uContainer)
			fprintf(fp," selected");
		if((uCount++)<=300)
			fprintf(fp,">%s</option>",field[1]);
		else
			fprintf(fp,">Limit reached. Contact your sysadmin ASAP!</option>");
	}
	mysql_free_result(res);

	fprintf(fp,"</select>\n");

	fprintf(fp,"<p><input type=text class=type_fields"
			" title='Enter new name for the above selected container'"
			" name=gcNewHostname value='%s' size=16 maxlength=32>",gcNewHostname);

	fprintf(fp,".%s\n",cOrg_NewHostname);

	fprintf(fp,"<p><input type=submit class=largeButton"
			" title='Select a container, enter the first part of the new FQDN hostname. Then use this button.'"
			" name=gcFunction value='Change Hostname'>\n");

	fprintf(fp,"</td></tr>\n");

	fprintf(fp,"<!-- funcNewContainer(fp) End -->\n");

}//void funcNewContainer(FILE *fp)


char *NameToLower(char *cInput)
{
	register int i;

	for(i=0;cInput[i];i++)
	{
	
		if(!isalnum(cInput[i]) ) break;
		if(isupper(cInput[i])) cInput[i]=tolower(cInput[i]);
	}
	cInput[i]=0;

	return(cInput);

}//char *NameToLower(char *cInput)
