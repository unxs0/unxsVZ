/*
FILE
	purchase.c
PURPOSE
	Handles RADIUS account deployment,
	invoice setup and payment after first time loginm
AUTHOR
	(C) 2010 Hugo Urquiza for Unixservice, LLC.
*/
#include "interface.h"

void ProcessPurchaseVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{

//		if(!strcmp(entries[i].name,"cFirstName"))
//			sprintf(cFirstName,"%.32s",entries[i].val);
	}
}//void ProcessPurchaseVars(pentry entries[], int x)


void PurchaseCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Purchase"))
	{
		ProcessPurchaseVars(entries,x);

	}
}//void PurchaseCommands(pentry entries[], int x)


void ShowPurchaseRadiusPage(void)
{
	htmlHeader("unxsISP Client Interface","Header");
	htmlCustomerPage("","FirstTime.Body");
	htmlFooter("Footer");
}//void ShowPurchaseRadiusPage(void)


unsigned IsFirstTimeLogin(void)
{
	MYSQL_RES *res;
	unsigned uRet=0;

	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cCode='NeverLogin' AND uClient=%u",guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	uRet=mysql_num_rows(res);

	mysql_free_result(res);
	return(uRet);

}//unsigned IsFirstTimeLogin(void)

