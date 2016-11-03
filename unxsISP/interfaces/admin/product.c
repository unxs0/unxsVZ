/*
FILE 
	product.c
	svn ID removed
AUTHOR
	(C) 2007-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	unxsISP Admin (Owner) Interface
	program file.
*/


#include "interface.h"

static unsigned uProduct=0;

static char cLabel[33]={""};
static char *cLabelStyle="type_fields_off";

static unsigned uAvailable=0;
static char *cuAvailableStyle="type_fields_off";

static unsigned uPeriod=0;
static char *cuPeriodStyle="type_fields_off";

static char mPrice[16]={""};
static char *mPriceStyle="type_fields_off";

static char mCost[16]={""};
static char *mCostStyle="type_fields_off";

static char mSetupFee[16]={""};
static char *mSetupFeeStyle="type_fields_off";

static char *cComment={""};
static char *cCommentStyle="type_textarea_off";

static char mReleaseFee[16]={""};
static char *mReleaseFeeStyle="type_fields_off";

//uOwner: Record owner
static unsigned uOwner=0;
//uCreatedBy: uClient for last insert
static unsigned uCreatedBy=0;
#define ISM3FIELDS
//uCreatedDate: Unix seconds date last insert
static time_t uCreatedDate=0;
//uModBy: uClient for last update
static unsigned uModBy=0;
//uModDate: Unix seconds date last update
static time_t uModDate=0;

static unsigned uNoSelect=0;


#define VAR_LIST_tProduct "tProduct.cLabel,tProduct.uAvailable,tProduct.uPeriod,tProduct.mPrice,tProduct.mCost,tProduct.mSetupFee,tProduct.cComment,tProduct.mReleaseFee,tProduct.uProductType,tProduct.cTitle,tProduct.uOwner,tProduct.uCreatedBy,tProduct.uCreatedDate,tProduct.uModBy,tProduct.uModDate"

static char cSearch[100]={""};

static unsigned uService=0;

//
//Local only
unsigned ValidateProductInput(void);
void SearchProduct(char *cLabel);
void NewProduct(void);
void DelProduct(void);
void ModProduct(void);
unsigned uProductExists(void);
unsigned uIsProductDeployed(void);

void SetProductFieldsOn(void);
void LoadProduct(unsigned cuProduct);


void ProcessProductVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uProduct"))
			sscanf(entries[i].val,"%u",&uProduct);	
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uAvailable"))
			sscanf(entries[i].val,"%u",&uAvailable);
		else if(!strcmp(entries[i].name,"uPeriod"))
			sscanf(entries[i].val,"%u",&uPeriod);
		else if(!strcmp(entries[i].name,"mPrice"))
			sprintf(mPrice,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"mCost"))
			sprintf(mCost,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"mSetupFee"))
			sprintf(mSetupFee,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"cComment"))
			cComment=entries[i].val;
		else if(!strcmp(entries[i].name,"mReleaseFee"))
			sprintf(mReleaseFee,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"uService"))
			sscanf(entries[i].val,"%u",&uService);
		else if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"uOwner"))
			sscanf(entries[i].val,"%u",&uOwner);
		else if(!strcmp(entries[i].name,"uCreatedBy"))
			sscanf(entries[i].val,"%u",&uCreatedBy);
		else if(!strcmp(entries[i].name,"uCreatedDate"))
			sscanf(entries[i].val,"%lu",&uCreatedDate);
		else if(!strcmp(entries[i].name,"uModBy"))
			sscanf(entries[i].val,"%u",&uModBy);
		else if(!strcmp(entries[i].name,"uModDate"))
			sscanf(entries[i].val,"%lu",&uModDate);
	}

}//void ProcessUserVars(pentry entries[], int x)


void ProductGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uProduct"))
			sscanf(gentries[i].val,"%u",&uProduct);
		else if(!strcmp(gentries[i].name,"uService"))
			sscanf(gentries[i].val,"%u",&uService);
	}

	if(!strcmp(gcFunction,"RemoveService") && uService && uProduct && !uIsProductDeployed())
	{
		sprintf(gcQuery,"DELETE FROM tServiceGlue WHERE uServiceGroup=%u AND uService=%u",uProduct,uService);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

	}
	if(uProduct)
		LoadProduct(uProduct);
	
	htmlProduct();

}//void ProductGetHook(entry gentries[],int x)


void LoadProduct(unsigned uProduct)
{
	
	MYSQL_RES *res;
	MYSQL_ROW field;
	
/*	sprintf(gcQuery,"SELECT cLabel,uAvailable,uPeriod,mPrice,mCost,mSetupFee,cComment,mReleaseFee FROM tProduct WHERE uProduct='%u' AND uOwner=%u",
			uProduct
			,guOrg);*/
	ExtSelectRow("tProduct",VAR_LIST_tProduct,uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{	
		sprintf(cLabel,"%.32s",field[0]);
		sscanf(field[1],"%u",&uAvailable);
		sscanf(field[2],"%u",&uPeriod);
		sprintf(mPrice,"%.12s",field[3]);
		sprintf(mCost,"%.12s",field[4]);
		sprintf(mSetupFee,"%.12s",field[5]);
		cComment=field[6];
		sprintf(mReleaseFee,"%.12s",field[7]);
		sscanf(field[10],"%u",&uOwner);
		sscanf(field[11],"%u",&uCreatedBy);
		sscanf(field[12],"%lu",&uCreatedDate);
		sscanf(field[13],"%u",&uModBy);
		sscanf(field[14],"%lu",&uModDate);
	}
	else
		gcMessage="No records found.";
	mysql_free_result(res);

}//void LoadProduct(char *cuProduct)


void SearchProduct(char *cSearchTerm)
{
	//Dont show authorized clients!
	sprintf(gcQuery,"SELECT tProduct.uProduct,tProduct.cLabel FROM tProduct WHERE tProduct.cLabel LIKE '%s%%'",cSearchTerm);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
}//void SearchProduct(char *cSearchTerm)


void ProductCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Product"))
	{
		ProcessProductVars(entries,x);

		if(!strcmp(gcFunction,"New"))
		{	
			if(guPermLevel>=10)
			{
				uNoSelect=1;
				sprintf(gcNewStep,"Confirm ");
				gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
				gcInputStatus[0]=0;
				SetProductFieldsOn();
				uProduct=0;
				htmlProduct();
			}
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings";
				htmlProduct();
			}
		}
		else if(!strcmp(gcFunction,"Confirm New"))
		{
			if(guPermLevel>=10)
			{
				uNoSelect=1;
				if(!uProductExists())
				{
					if(!ValidateProductInput())
					{
						sprintf(gcNewStep,"Confirm ");
						SetProductFieldsOn();
						gcInputStatus[0]=0;
						htmlProduct();
					}
					NewProduct();
				}
				else
				{
					gcMessage="<blink>Error: </blink>Product already exists!";
					SetProductFieldsOn();
					gcInputStatus[0]=0;
					sprintf(gcNewStep,"Confirm ");
				}
			}
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings";
				htmlProduct();
			}
		}
		else if(!strcmp(gcFunction,"Modify"))
		{
			if(uAllowMod(uOwner,uCreatedBy)&& !uIsProductDeployed())
			{
				uNoSelect=1;
				sprintf(gcModStep,"Confirm ");
				gcMessage="Enter/modify data, review, then confirm. Any other action to cancel.";
				SetProductFieldsOn();
				gcInputStatus[0]=0;
			}
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings";
				htmlProduct();
			}
		}
		else if(!strcmp(gcFunction,"Confirm Modify"))
		{
			if(uAllowMod(uOwner,uCreatedBy)&& !uIsProductDeployed())
			{
				if(!ValidateProductInput())
				{
					uNoSelect=1;
					sprintf(gcModStep,"Confirm ");
					gcInputStatus[0]=0;
					SetProductFieldsOn();
					htmlProduct();
				}
				ModProduct();
			}
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings";
				htmlProduct();
			}
		}
		else if(!strcmp(gcFunction,"Delete"))
		{
			if(uAllowDel(uOwner,uCreatedBy)&& !uIsProductDeployed())
			{
				sprintf(gcDelStep,"Confirm ");
				gcMessage="Double check you have selected the correct record to delete. Then confirm. Any other action to cancel.";
				htmlProduct();
			}
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings";
				htmlProduct();
			}
		}
		else if(!strcmp(gcFunction,"Confirm Delete")&& !uIsProductDeployed())
			if(uAllowDel(uOwner,uCreatedBy))
			{
				DelProduct();
			}
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings";
				htmlProduct();
			}
		else if(!strcmp(gcFunction,"Add Service")&& !uIsProductDeployed())
		{
			if(uAllowMod(uOwner,uCreatedBy))
			{
				unsigned uServiceGlue=0;
			
				sprintf(gcQuery,"INSERT INTO tServiceGlue SET uService=%u,uServiceGroup=%u",uService,uProduct);
				mysql_query(&gMysql,gcQuery);

				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
			
				uServiceGlue=mysql_insert_id(&gMysql);
			
				if(uServiceGlue)
				{
					gcMessage="Service added to product OK";
					unxsISPLog(uServiceGlue,"tServiceGlue","New");
				}
				else
					gcMessage="Could not add service";
			}			
			else
			{
				gcMessage="<blink>Error: </blink> Denied by permissions settings";
				htmlProduct();
			}
		}
			
		htmlProduct();
	}

}//void ProductCommands(pentry entries[], int x)


void htmlProduct(void)
{
	if(cSearch[0])
	{
		FILE *fp;
		if((fp=fopen("/dev/null","w")))
		{
			funcProductNavList(fp);
			fclose(fp);
		}
	}
	htmlHeader("unxsISP Admin Interface","Header");
	htmlProductPage("","Product.Body");
	htmlFooter("Footer");

}//void htmlProduct(void)


void htmlProductPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect(cTemplateName);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			char cuProduct[16]={""};
			char cuAvailable[16]={""};
			char cuPeriod[16]={""};
			char cuOwner[16]={""};
			char cuCreatedBy[16]={""};
			char cuCreatedDate[16]={""};
			char cuModBy[16]={""};
			char cuModDate[16]={""};

		
			sprintf(cuOwner,"%u",uOwner);
			sprintf(cuCreatedBy,"%u",uCreatedBy);
			sprintf(cuCreatedDate,"%lu",uCreatedDate);
			sprintf(cuModBy,"%u",uModBy);
			sprintf(cuModDate,"%lu",uModDate);
		
			sprintf(cuProduct,"%u",uProduct);
			sprintf(cuAvailable,"%u",uAvailable);
			sprintf(cuPeriod,"%u",uPeriod);

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="ispAdmin.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcLogin;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcMessage";
			template.cpValue[7]=gcMessage;

			template.cpName[8]="gcInputStatus";
			template.cpValue[8]=gcInputStatus;
				
			template.cpName[9]="gcNewStep";
			template.cpValue[9]=gcNewStep;

			template.cpName[10]="gcModStep";
			template.cpValue[10]=gcModStep;
			
			template.cpName[11]="gcDelStep";
			template.cpValue[11]=gcDelStep;

			template.cpName[12]="cNavList";
			template.cpValue[12]="";

			template.cpName[12]="cBtnStatus";
			if(uProduct && !uIsProductDeployed())
				template.cpValue[12]="";
			else
				template.cpValue[12]="disabled";

			template.cpName[13]="uProduct";
			template.cpValue[13]=cuProduct;

			template.cpName[14]="cLabel";
			template.cpValue[14]=cLabel;

			template.cpName[15]="cLabelStyle";
			template.cpValue[15]=cLabelStyle;

			template.cpName[16]="mPrice";
			template.cpValue[16]=mPrice;

			template.cpName[17]="mPriceStyle";
			template.cpValue[17]=mPriceStyle;

			template.cpName[18]="mCost";
			template.cpValue[18]=mCost;

			template.cpName[19]="mCostStyle";
			template.cpValue[19]=mCostStyle;

			template.cpName[20]="mSetupFee";
			template.cpValue[20]=mSetupFee;

			template.cpName[21]="cComment";
			template.cpValue[21]="";

			template.cpName[22]="cCommentStyle";
			template.cpValue[22]=cCommentStyle;
			
			template.cpName[23]="mReleaseFee";
			template.cpValue[23]=mReleaseFee;
			
			template.cpName[24]="mReleaseFeeStyle";
			template.cpValue[24]=mReleaseFeeStyle;

			template.cpName[25]="mSetupFeeStyle";
			template.cpValue[25]=mSetupFeeStyle;

			template.cpName[26]="uAvailable";
			template.cpValue[26]=cuAvailable;

			template.cpName[27]="uPeriod";
			template.cpValue[27]=cuPeriod;

			template.cpName[28]="uOwner";
			template.cpValue[28]=cuOwner;

			template.cpName[29]="uCreatedBy";
			template.cpValue[29]=cuCreatedBy;

			template.cpName[30]="uCreatedDate";
			template.cpValue[30]=cuCreatedDate;

			template.cpName[31]="uModBy";
			template.cpValue[31]=cuModBy;

			template.cpName[32]="uModDate";
			template.cpValue[32]=cuModDate;

			template.cpName[33]="";

			printf("\n<!-- Start htmlProductPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlProductPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlProductPage()


unsigned uProductExists(void)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uProduct FROM tProduct WHERE cLabel='%s'",cLabel);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	return(unsigned)mysql_num_rows(res);
	
}//unsigned uProductExists(void)


unsigned uIsProductDeployed(void)
{
	MYSQL_RES *res;
	char cQuery[100]={""};

	sprintf(cQuery,"SELECT uInstance FROM tInstance WHERE uProduct='%u'",uProduct);
	mysql_query(&gMysql,cQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	return(unsigned)mysql_num_rows(res);
	
}//unsigned uIsProductDeployed(void)


void NewProduct(void)
{

	sprintf(gcQuery,"INSERT INTO tProduct SET cLabel='%s',uAvailable=%u,uPeriod=%u,mPrice='%s',mCost='%s',mSetupFee='%s',cComment='%s',mReleaseFee='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			TextAreaSave(cLabel)
			,uAvailable
			,uPeriod
			,TextAreaSave(mPrice)
			,TextAreaSave(mCost)
			,TextAreaSave(mSetupFee)
			,TextAreaSave(cComment)
			,TextAreaSave(mReleaseFee)
			,guOrg
			,guLoginClient
	       );

	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uProduct=mysql_insert_id(&gMysql);

	if(uProduct)
	{
		gcMessage="Product created OK";
		unxsISPLog(uProduct,"tProduct","New");
	}
	else
	{
		gcMessage="Product NOT created";
		unxsISPLog(uProduct,"tProduct","New Fail");
	}
	time(&uCreatedDate);
	uOwner=guOrg;
	uCreatedBy=guLoginClient;

}//void NewProduct(void)


void ModProduct(void)
{
	sprintf(gcQuery,"UPDATE tProduct SET cLabel='%s',uAvailable=%u,uPeriod=%u,mPrice='%s',"
			"mCost='%s',mSetupFee='%s',cComment='%s',mReleaseFee='%s',uModBy=%u,"
			"uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%u",
			TextAreaSave(cLabel)
			,uAvailable
			,uPeriod
			,TextAreaSave(mPrice)
			,TextAreaSave(mCost)
			,TextAreaSave(mSetupFee)
			,TextAreaSave(cComment)
			,TextAreaSave(mReleaseFee)
			,guLoginClient
			,uProduct
	       );
	mysql_query(&gMysql,gcQuery);
	
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="Product modified OK";
		unxsISPLog(uProduct,"tProduct","Mod");
	}
	else
	{
		gcMessage="Product NOT modified";
		unxsISPLog(uProduct,"tProduct","Mod Fail");
	}
	time(&uModDate);
	uModBy=guLoginClient;

}//void ModProduct(void)


void DelProduct(void)
{
	sprintf(gcQuery,"DELETE FROM tProduct WHERE uProduct=%u",uProduct);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	if(mysql_affected_rows(&gMysql))
	{
		gcMessage="Product deleted OK";
		unxsISPLog(uProduct,"tProduct","Del");
	}
	else
	{
		gcMessage="Product NOT deleted";
		unxsISPLog(uProduct,"tProduct","Del Fail");
	}

}//void DelProduct(void)


unsigned ValidateProductInput(void)
{
	if(!cLabel[0])
	{
		SetProductFieldsOn();
		cLabelStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Product name can't be empty";
		return(0);
	}

	if(!uPeriod)
	{
		SetProductFieldsOn();
		cuPeriodStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must select product billing period";
		return(0);
	}

	if(!mPrice[0])
	{
		SetProductFieldsOn();
		mPriceStyle="type_fields_req";
		gcMessage="<blink>Error: </blink>Must enter product price";
		return(0);
	}
	else
	{
		register int i;
		for(i=0;i<strlen(mPrice);i++)
			if(!isdigit(mPrice[i]) && mPrice[i]!='.')
			{
				mPriceStyle="type_fields_req";
				gcMessage="<blink>Error: </blink>Review product price format";
				return(0);
			}
	}

	return(1);
		
}//unsigned ValidateInput(void)


void SetProductFieldsOn(void)
{
	cLabelStyle="type_fields";
	cuAvailableStyle="type_fields";
	cuPeriodStyle="type_fields";
	mPriceStyle="type_fields";
	mCostStyle="type_fields";
	mSetupFeeStyle="type_fields";
	cCommentStyle="type_textarea";
	mReleaseFeeStyle="type_fields";

}//void SetProductFieldsOn(void)


void funcSelectPeriod(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fprintf(fp,"<!-- funcSelectPeriod(fp) start -->\n");
	
	sprintf(gcQuery,"SELECT uPeriod,cLabel FROM tPeriod ORDER BY cLabel");
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s<br>",mysql_error(&gMysql));
		return;
	}
	
	res=mysql_store_result(&gMysql);
	sprintf(gcQuery,"%u",uPeriod);

	fprintf(fp,"<select name=uPeriod title='Select the product billing period' %s class=%s>\n",gcInputStatus,cuPeriodStyle);
	fprintf(fp,"<option>---</option>\n");
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<option value=%s ",field[0]);
		if(!strcmp(gcQuery,field[0]))
			fprintf(fp,"selected");
		fprintf(fp,">%s</option>\n",field[1]);
	}
	fprintf(fp,"</select>\n");

	fprintf(fp,"<!-- funcSelectVendor(fp) end -->\n");
	
}//void funcSelectPeriod(FILE *fp)


void funcSelectAvailable(FILE *fp)
{
	fprintf(fp,"<!-- funcSelectAvailable(fp) start -->\n");

	fprintf(fp,"<select name=uAvailable title='Indicates if the product would be available for customers' %s class=%s>\n",
			gcInputStatus
			,cuAvailableStyle);
	fprintf(fp,"<option>---</option> \n");
	fprintf(fp,"<option value=1 ");
	if(uAvailable)
		fprintf(fp,"selected");
	fprintf(fp,">Yes</option>\n");
	
	fprintf(fp,"<option value=0 ");
	if(!uAvailable)
		fprintf(fp,"selected");
	fprintf(fp,">No</option>\n");

	fprintf(fp,"</select>\n");
	
}//void funcSelectAvailable(FILE *fp)


void funcProductServices(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	fprintf(fp,"<!-- funcProductServices(fp) start -->\n");

	if(!uProduct)
		return;

	sprintf(gcQuery,"SELECT tService.uService,tService.cLabel,cServer FROM tService,tServiceGlue,tProduct WHERE tProduct.uProduct=%u AND tServiceGlue.uServiceGroup=tProduct.uProduct AND tService.uService=tServiceGlue.uService AND tService.uAvailable=1",uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"<tr><td>%s</td><td>%s</td>",
				field[1]
				,field[2]
				);
		if(!uIsProductDeployed())
				fprintf(fp,"<td><a title='Remove the selected service from the product' href=ispAdmin.cgi?gcPage=Product&uProduct=%u&gcFunction=RemoveService&uService=%s onclick=\"return confirm('Are you sure you want to remove this service?','unxsISP Admin Interface')\">Remove</a></td></tr>\n",
				uProduct
				,field[0]
				);
		else
			fprintf(fp,"<td>&nbsp;</td></tr>\n");
	}
	
	fprintf(fp,"<!-- funcProductServices(fp) end -->\n");

}//void funcProductServices(FILE *fp)


void funcServicesToAdd(FILE *fp)
{
	//Same as funcProductServices() but displaying in a dropdown the services that are not associated with the loaded product
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cCondition[1024]={""};
	char cTmp[33]={""};
	
	fprintf(fp,"<!-- funcProductServices(fp) start -->\n");
	
	if(uProduct && !uIsProductDeployed())
		fprintf(fp,"<select name=uService class=type_fields>\n");
	else
	{
		fprintf(fp,"<select name=uService class=type_fields_off disabled>\n");
		fprintf(fp,"<option>---</option>\n");
		fprintf(fp,"</select>\n");
		return;
	}
	
	//1. Get the services associated with this product
	sprintf(gcQuery,"SELECT uService FROM tServiceGlue WHERE tServiceGlue.uServiceGroup=%u",uProduct);
	
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	 
	res=mysql_store_result(&gMysql);
	
	//2. Create the WHERE clause
	sprintf(cCondition," ");
	
	while((field=mysql_fetch_row(res)))
	{
		sprintf(cTmp," tService.uService!=%s AND",field[0]);
		strcat(cCondition,cTmp);
	}

	strcat(cCondition," tService.uAvailable=1");
	
	//Based on 1. decide how we will fill the services drop down.
	if(mysql_num_rows(res))
	{
		//We got services to exclude
		if(guLoginClient==1 && guPermLevel>11)//Root can read access all
			sprintf(gcQuery,"SELECT tService.uService,tService.cLabel,cServer FROM tService WHERE %s ORDER BY tService.cLabel",cCondition);
		else
			sprintf(gcQuery,"SELECT DISTINCT tService.uService,tService.cLabel,cServer FROM tService,"TCLIENT" WHERE %1$s AND "
					"tService.uOwner=tClient.uOwner AND (tClient.uClient=%2$u OR tClient.uOwner IN (SELECT uClient "
					"FROM "TCLIENT" WHERE uOwner=%2$u OR uClient=%2$u)) ORDER BY tService.cLabel",cCondition,guOrg);
	}
	else
	{	
		//Product with no services
		if(guLoginClient==1 && guPermLevel>11)//Root can read access all
			sprintf(gcQuery,"SELECT tService.uService,tService.cLabel,cServer FROM tService ORDER BY tService.cLabel");
		else
			sprintf(gcQuery,"SELECT DISTINCT tService.uService,tService.cLabel,cServer FROM tService,"TCLIENT" WHERE "
					"tService.uOwner=tClient.uOwner AND (tClient.uClient=%1$u OR tClient.uOwner IN (SELECT uClient "
					"FROM "TCLIENT" WHERE uOwner=%1$u OR uClient=%1$u)) ORDER BY tService.cLabel",guOrg);
	}
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);

	while((field=mysql_fetch_row(res)))
		fprintf(fp,"<option value=%s>%s</option>\n",field[0],field[1]);

	fprintf(fp,"</select>\n");

	fprintf(fp,"<!-- funcProductServices(fp) end-->\n");

}//void funcServicesToAdd(FILE *fp)


void funcProductNavList(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uFound=0;
	unsigned uDisplayed=0;
	
	static char cTopMessage[64]={""};

	if(uNoSelect) return;

	if(cSearch[0])
		ExtSelectSearch("tProduct","tProduct.uProduct,tProduct.cLabel","tProduct.cLabel",cSearch,NULL,0);
	else
		ExtSelect("tProduct","tProduct.uProduct,tProduct.cLabel",0);
	
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	
	if(!(uFound=mysql_num_rows(res)))
	{
		fprintf(fp,"No records found.<br>\n");
		mysql_free_result(res);
		return;
	}
	sprintf(cTopMessage,"%u record(s) found.",uFound);
	gcMessage=cTopMessage;
	
	if(uFound==1)
	{
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uProduct);
			LoadProduct(uProduct);

			fprintf(fp,"<a href=ispAdmin.cgi?gcPage=Product&uProduct=%s>%s</a><br>\n",field[0],field[1]);
			mysql_free_result(res);
			return;
		}
	}

	while((field=mysql_fetch_row(res)))
	{
		if(uDisplayed==20)
		{
			fprintf(fp,"Only the first 20 records found. If the product you are looking for is not in the list above please "
			"further refine your search.<br>\n");
			break;
		}
		fprintf(fp,"<a href=ispAdmin.cgi?gcPage=Product&uProduct=%s>%s</a><br>\n",field[0],field[1]);
		uDisplayed++;
	}

	mysql_free_result(res);

}//void funcProductNavList(FILE *fp)


void funcProductNavBar(FILE *fp)
{
	if(guPermLevel>=10)
		fprintf(fp,"<input type=submit title='Product creation with a two step procedure' class=largeButton name=gcFunction "
			"value='%sNew' />",gcNewStep);

	if(uAllowMod(uOwner,uCreatedBy) && !uIsProductDeployed())
		fprintf(fp,"<input type=submit title='Product modification with a two step procedure' class=largeButton name=gcFunction "
			"value='%sModify' />",gcModStep);

	if(uAllowDel(uOwner,uCreatedBy) && !!uIsProductDeployed())
		fprintf(fp,"<input type=submit title='Delete a product with a two step procedure' class=largeButton name=gcFunction "
			"value='%sDelete' />",gcDelStep);

}//void funcCustomerNavBar(FILE *fp)

