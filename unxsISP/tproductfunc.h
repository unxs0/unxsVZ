/*
FILE
	svn ID removed
PURPOSE
	tproduct.c expansion non-schema dependent file.
	For application gcFunctionality regarding unxsISP products and related
	issues. Example service and paramaters. Billing and accouting issues.

AUTHOR
	GPL License applies, see www.fsf.org for details
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
	See LICENSE file in this distribution.

 
*/

#include "mysqlisp.h"

void tProductServiceList(void);
void tProductNavList(void);
static unsigned uService=0;
static unsigned uInstance=0;
static char cuServicePullDown[256]={""};
void DeleteFromConfig(unsigned uProduct);
void CheckForProductUse(unsigned const uProduct);
void CheckForUnBilledUse(unsigned const uProduct);

//Extern
void tTablePullDownCustom(char *cTableName, char *cFieldName, char *cOrderby, unsigned uSelector);

void ExtProcesstProductVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cuServicePullDown"))
		{
			strcpy(cuServicePullDown,entries[i].val);
			uService=ReadPullDown("tService","cLabel"
							,cuServicePullDown);
		}
		else if(!strcmp(entries[i].name,"uInstance"))
			sscanf(entries[i].val,"%u",&uInstance);
		else if(!strcmp(entries[i].name,"uProduct"))
			sscanf(entries[i].val,"%u",&uProduct);
	}
}//void ExtProcesstProductVars(pentry entries[], int x)


void CheckForProductUse(unsigned const uProduct)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uProduct FROM tInstance WHERE uProduct=%u",uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tProduct(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		mysql_free_result(res);
		tProduct("Can't delete product is in use.");
	}
	mysql_free_result(res);

}//void CheckForProductUse(unsigned const uProduct)


void CheckForUnBilledUse(unsigned const uProduct)
{
	MYSQL_RES *res;
        MYSQL_ROW field;
	char mLocalPrice[16]={"-999999.99"};
	char mLocalSetupFee[16]={"-999999.99"};

	sprintf(gcQuery,"SELECT mPrice,mSetupFee FROM tProduct WHERE uProduct=%u",uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tProduct(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(mLocalPrice,"%.12s",field[0]);
		sprintf(mLocalSetupFee,"%.12s",field[0]);
	}
	mysql_free_result(res);

	//Check only if mPrice or mSetupFee is to be changed
	if(!strcmp(mLocalPrice,mPrice) && !strcmp(mLocalSetupFee,mSetupFee))
		return;

	sprintf(gcQuery,"SELECT uProduct FROM tInstance WHERE uProduct=%u AND uBilled=0 AND uStatus!=%u",uProduct,unxsISP_Canceled);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tProduct(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		mysql_free_result(res);
		tProduct("Can't modify product price or setup fee it is in use and not invoiced yet.");
	}
	mysql_free_result(res);

}//void CheckForUnBilledUse(unsigned const uProduct)



void DeleteFromServiceGlue(unsigned uProduct)
{
	sprintf(gcQuery,"DELETE FROM tServiceGlue WHERE uServiceGroup=%u",uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tProduct(mysql_error(&gMysql));

}//void DeleteFromServiceGlue(unsigned uProduct)

void ExttProductCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tProductTools"))
	{
		//Custom
		if(!strcmp(gcCommand,"AddService"))
		{
			ProcesstProductVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy)) 
			{
				sprintf(gcQuery,"INSERT INTO tServiceGlue SET uServiceGroup=%u,uService=%u",uProduct,uService);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					tProduct(mysql_error(&gMysql));
				else
					tProduct("Service added");
			}
			else
				tProduct("<blink>Error</blink>: Denied by permissions settings");    
		}
		else if(!strcmp(gcCommand,"AllNotAvailable"))
		{
			if(guPermLevel>=12 && guLoginClient==1)
			{
				sprintf(gcQuery,"UPDATE tProduct SET uAvailable=0");
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					tProduct(mysql_error(&gMysql));
				else
					tProduct("All products made unavailable");
			}
			else
				tProduct("<blink>Error</blink>: Denied by permissions settings");    
		}
		else if(!strcmp(gcCommand,"DelService"))
		{
			ProcesstProductVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				sprintf(gcQuery,"DELETE FROM tServiceGlue WHERE uServiceGroup=%u AND uService=%u",uProduct,uService);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					tProduct(mysql_error(&gMysql));
				else
					tProduct("Service deleted");
			}
			else
				tProduct("<blink>Error</blink>: Denied by permissions settings");    
		}

		else if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstProductVars(entries,x);
                        	guMode=2000;
	                        tProduct(LANG_NB_CONFIRMNEW);
			}
			else
				tProduct("<blink>Error</blink>: Denied by permissions settings");    
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstProductVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uProduct=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtProduct(0);
			}
			else
				tProduct("<blink>Error</blink>: Denied by permissions settings");    
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstProductVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				CheckForProductUse(uProduct);
	                        guMode=2001;
				tProduct(LANG_NB_CONFIRMDEL);
			}
			else
				tProduct("<blink>Error</blink>: Denied by permissions settings");    
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstProductVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
			
				guMode=2001;	
				CheckForProductUse(uProduct);
				DeleteFromServiceGlue(uProduct);//Clean up
				guMode=5;
				DeletetProduct();
			}
			else
				tProduct("<blink>Error</blink>: Denied by permissions settings");    
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstProductVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))

			{
				guMode=2002;
				tProduct(LANG_NB_CONFIRMMOD);
			}
			else
				tProduct("<blink>Error</blink>: Denied by permissions settings");    
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstProductVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				CheckForUnBilledUse(uProduct);
                        	guMode=0;

				uModBy=guLoginClient;
				ModtProduct();
			}
			else
				tProduct("<blink>Error</blink>: Denied by permissions settings");    
                }
	}

}//void ExttProductCommands(pentry entries[], int x)


void ExttProductButtons(void)
{
	OpenFieldSet("tProduct Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter required data</u><br>");
                        printf("<font size=1>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<p><font size=1>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf("<font size=1>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

	}

	if(guPermLevel>7)
	{	
		printf("<p><font size=1>");
		tTablePullDownCustom("tService;cuServicePullDown","cLabel","cLabel",uService);

		printf("<br><input class=largeButton title='Add a service to this product' type=submit name=gcCommand value=AddService><br>");

		printf("<input class=largeButton title='Remove a service from this product' type=submit name=gcCommand value=DelService><br>");
	}
	if(guPermLevel>12 && guLoginClient==1)
	{
		printf("<input class=largeButton title='Set all products uAvailable=0' type=submit name=gcCommand value=AllNotAvailable><br>");
	}

	tProductServiceList();
	tProductNavList();

	CloseFieldSet();
	
}//void ExttProductButtons(void)


void ExttProductAuxTable(void)
{

}//void ExttProductAuxTable(void)


void ExttProductGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uProduct"))
		{
			sscanf(gentries[i].val,"%u",&uProduct);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"uInstance"))
			sscanf(gentries[i].val,"%u",&uInstance);
	}
	tProduct("");

}//void ExttProductGetHook(entry gentries[], int x)


void ExttProductSelect(void)
{
	ExtSelect("tProduct",VAR_LIST_tProduct,0);

}//void ExttProductSelect(void)


void ExttProductSelectRow(void)
{
	ExtSelectRow("tProduct",VAR_LIST_tProduct,uProduct);

}//void ExttProductSelectRow(void)


void ExttProductListSelect(void)
{
	char cCat[512];

	ExtListSelect("tProduct",VAR_LIST_tProduct);
	
	//Changes here must be reflected below in ExttProductListFilter()
        if(!strcmp(gcFilter,"uProduct"))
        {
                sscanf(gcCommand,"%u",&uProduct);
		strcat(gcQuery," WHERE ");
		sprintf(cCat,"tProduct.uProduct=%u \
						ORDER BY uProduct",
						uProduct);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uProduct");
        }

}//void ExttProductListSelect(void)


void ExttProductListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uProduct"))
                printf("<option>uProduct</option>");
        else
                printf("<option selected>uProduct</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttProductListFilter(void)


void ExttProductNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
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

}//void ExttProductNavBar(void)


void tProductNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cProductType[33]={""};

	ExtSelectSearch("tProduct","tProduct.uProduct,tProduct.cLabel,tProduct.uAvailable,"
			"(SELECT tProductType.cLabel FROM tProductType WHERE tProductType.uProductType=tProduct.uProductType)",
			"tProduct.uProductType","%","tProduct.uProductType!=0",0);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>Products by Main Service (uProductType)</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
			if(field[3]!=NULL)
			{
				if(strcmp(field[3],cProductType))
				{
					strcpy(cProductType,field[3]);
					printf("<u>%s</u><br>\n",field[3]);
				}
			}

			printf("&nbsp;<a class=darkLink href=unxsISP.cgi?gcFunction=tProduct&uProduct=%s>",field[0]);
			if(field[2][0]=='0')
				printf("<font color=gray>%s</font></a><br>\n",field[1]);
			else
				printf("%s</a><br>\n",field[1]);
	        }
	}

	sprintf(gcQuery,"SELECT uProduct,cLabel,uAvailable FROM tProduct WHERE uProductType=0 ORDER BY cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<u>Un-Categorized Products</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
			printf("&nbsp;<a class=darkLink href=unxsISP.cgi?gcFunction=tProduct&uProduct=%s>",field[0]);
			if(field[2][0]=='0')
				printf("<font color=gray>%s</font></a><br>\n",field[1]);
			else
				printf("%s</a><br>\n",field[1]);
	        }
	}
        mysql_free_result(res);

}//void tProductNavList(void)


void tProductServiceList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	if(!uProduct) return;

	sprintf(gcQuery,"SELECT tService.uService,tService.cLabel FROM tService,tServiceGlue WHERE "
			"tService.uService=tServiceGlue.uService AND tServiceGlue.uServiceGroup=%u ORDER BY tServiceGlue.uServiceGlue",uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s",mysql_error(&gMysql));
		return;
	}

	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
		printf("<p><u>tProduct Services</u><br>");

		while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tService&uService=%s>%s</a><br>\n",field[0],field[1]);
	}
	mysql_free_result(res);
									
}//void tProductServiceList(void)

// vim:tw=78
