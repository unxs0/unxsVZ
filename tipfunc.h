/*
FILE
	$Id: tipfunc.h 2829 2009-04-03 00:40:45Z Gary $
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis.
 
*/

#include <openisp/ucidr.h>

void tIPReport(void);
void tIPNavList(unsigned uAvailable);

static char cIPRange[32]={""};
void AddIPRange(char *cIPRange);
void DelIPRange(char *cIPRange);

void ExtProcesstIPVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cIPRange"))
			sprintf(cIPRange,"%.31s",entries[i].val);
	}
}//void ExtProcesstIPVars(pentry entries[], int x)


void ExttIPCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tIPTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstIPVars(entries,x);
                        	guMode=2000;
	                        tIP(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstIPVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uIP=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtIP(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstIPVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy) && uAvailable)
			{
	                        guMode=2001;
				tIP(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstIPVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy) && uAvailable)
			{
				guMode=5;
				if(cIPRange[0])
					DelIPRange(cIPRange);
				DeletetIP();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstIPVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tIP(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstIPVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				if(cIPRange[0])
					AddIPRange(cIPRange);

				uModBy=guLoginClient;
				ModtIP();
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
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<p><u>Think twice</u><br>");
			printf("<input title='Optionally enter CIDR IP Range (ex. 10.0.0.1/27) for available mass deletion'"
				" type=text name=cIPRange> cIPRange<p>\n");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review changes</u><br>");
			printf("<input title='Optionally enter CIDR IP Range (ex. 10.0.0.1/27) for available mass addition'"
				" type=text name=cIPRange> cIPRange<p>\n");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			printf("<p><u>Table Tips</u><br>");
			printf("IPs used and available for container and new hardware node"
				" deployment are tracked and maintained here.<p>You can add available IPs <i>en masse</i>"
				" via the cIPRange at the [Modify] stage. For example 192.168.0.1/28 would add 16 IPs"
				" starting at 192.168.0.1 and ending at 192.168.0.16."
				"<p>You can also delete available IPs <i>en masse</i> via the cIPRange at the [Delete]"
				" stage.");
			if(uIP)
				tIPReport();
			tIPNavList(1);
			tIPNavList(0);
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
	}
	tIP("");

}//void ExttIPGetHook(entry gentries[], int x)


void ExttIPSelect(void)
{
	ExtSelect("tIP",VAR_LIST_tIP);

}//void ExttIPSelect(void)


void ExttIPSelectRow(void)
{
	ExtSelectRow("tIP",VAR_LIST_tIP,uIP);

}//void ExttIPSelectRow(void)


void ExttIPListSelect(void)
{
	char cCat[512];

	ExtListSelect("tIP",VAR_LIST_tIP);
	
	//Changes here must be reflected below in ExttIPListFilter()
        if(!strcmp(gcFilter,"uIP"))
        {
                sscanf(gcCommand,"%u",&uIP);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tIP.uIP=%u \
						ORDER BY uIP",
						uIP);
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

	if(uAllowMod(uOwner,uCreatedBy))
		printf(LANG_NBB_MODIFY);

	if(uAllowDel(uOwner,uCreatedBy) && uAvailable)
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

	sprintf(gcQuery,"SELECT uIP,cLabel FROM tIP WHERE uAvailable=%u AND uOwner=%u ORDER BY cLabel",
				uAvailable,guCompany);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tIPNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
		if(uAvailable)
        		printf("<p><u>Available tIPNavList</u><br>\n");
		else
        		printf("<p><u>Used tIPNavList</u><br>\n");


	        while((field=mysql_fetch_row(res)))
		{
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tIP"
					"&uIP=%s>%s</a><br>\n",field[0],field[1]);
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
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tNode"
					"&uNode=%s>%s used by %s</a><br>\n",field[0],cLabel,field[1]);
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
				",uCreatedDate=UNIX_TIMESTAMP(NOW())",cIPs[i],guCompany,guLoginClient);
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
		sprintf(gcQuery,"DELETE FROM tIP WHERE cLabel='%s' AND uAvailable=1",cIPs[i]);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		if(mysql_affected_rows(&gMysql)==1)
			uSkip++;
		free(cIPs[i]);
	}
	unxsVZLog(uIP,"tIP","DelRange");
	if(!uSkip)
		tIP("<blink>Warning:</blink> cIPRange of available IPs did not exist!");
	else if(uSkip==i)
		tIP("Complete cIPRange deleted");
	else if(1)
		tIP("<blink>Note:</blink> Partial cIPRange deleted. At least one available IP did not exist");

}//void DelIPRange(char *cIPRange)
