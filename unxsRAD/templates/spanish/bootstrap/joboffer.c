/*
FILE 
	{{cProject}}/interfaces/joboffer.c
	template/new/bootstrap/joboffer.c
AUTHOR/LEGAL
	(C) 2010-2018 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	Aux code for listing job offers
REQUIRES
*/

#include "interface.h"
//TOC
void htmlJobOfferSelect(FILE *fp);
void funcJobOffer(FILE *fp);
void htmlStatusSelect(FILE *fp);
void funcStatusSelect(FILE *fp);

unsigned guJobOffer= -1;
extern unsigned guItem;
unsigned guStatusFilter=0;

float fGetDiscount(unsigned uJobOffer);

char *cStatusLabel(unsigned uStatus)
{
	static char cLabel[32]={"En Tramite"};

	if(!uStatus)
		return(cLabel);

	sprintf(gcQuery,"SELECT cLabel FROM tStatus WHERE uStatus=%u",uStatus);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		return("error");
	}
	MYSQL_RES *res;
	MYSQL_ROW field;
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cLabel,"%.31s",field[0]);
	return(cLabel);

}//char *cStatusLabel(unsigned uStatus)


void htmlRemindPickup(FILE *fp)
{
	fprintf(fp,"<form method=post action=/unxsAKApp >\n");
	fprintf(fp,"<font size=-1><pre>\n");
	fprintf(fp,"Remind to pickup finished job\n\n");
	sprintf(gcQuery,"SELECT tJobOffer.cLabel,tJobOffer.uJobOffer,tClient.cEmail FROM tJobOffer,tClient"
			" WHERE tJobOffer.uOwner=tClient.uClient AND uStatus=10");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s101) try again later!";
		htmlJobOffer();
	}
	MYSQL_RES *res;
	MYSQL_ROW field;
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"%s (%s) %s\n",field[0],field[1],field[2]);
	}
	fprintf(fp,"</font></pre>\n");


	fprintf(fp,"<input type=hidden name=gcPage value=Admin>");
	fprintf(fp,"<button name=gcFunction value=Remind class='btn btn-success btn-lg btn-block' type=submit id=submitbutton >Remind</button>\n");
	fprintf(fp,"</form>\n");

}//void htmlRemindPickup(FILE *fp)


void htmlJobOfferFilterSelect(FILE *fp)
{
	sprintf(gcQuery,"SELECT DISTINCT tStatus.uStatus,tStatus.cLabel"
			" FROM tJobOffer,tStatus"
			" WHERE tJobOffer.uStatus=tStatus.uStatus"
			" ORDER BY tStatus.uStatus");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s91) try again later!";
		htmlJobOffer();
	}
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uStatus=0;
	res=mysql_store_result(&gMysql);
	fprintf(fp,"\t\t<form class=\"clearfix\" accept-charset=\"utf-8\" method=\"post\" action=\"/unxsAKApp\">\n");
	fprintf(fp,"\t\t<input type=hidden name=gcPage value=JobOffer >\n");
	fprintf(fp,"\t\t<input type=hidden name=gcFunction value=SetStatusFilter >\n");
	fprintf(fp,"\t\t<select onchange=\"this.form.submit()\" class=\"form-control\" id=\"uStatusFilter\" name=\"uStatusFilter\">\n");
	fprintf(fp,"\t\t\t<option value='0'>En Tramite</option>\n");
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uStatus);
		fprintf(fp,"\t\t\t<option ");
		if(uStatus==guStatusFilter)
			fprintf(fp,"selected ");
		fprintf(fp," value='%s'>%s</option>\n",field[0],field[1]);
	}
	fprintf(fp,"\t\t</select>\n");
	fprintf(fp,"\t\t</form>\n");
}//void htmlJobOfferFilterSelect(FILE *fp)


void htmlJobOfferSelect(FILE *fp)
{
	if(guStatusFilter)
	{
		if(guPermLevel>=10)
			sprintf(gcQuery,"SELECT tJobOffer.uJobOffer,tJobOffer.cLabel FROM tJobOffer,tClient"
				" WHERE tClient.uClient=tJobOffer.uOwner"
				" AND tJobOffer.uStatus=%u"
				" AND (tJobOffer.uOwner=%u OR tClient.uOwner=%u)"
				" ORDER BY tJobOffer.uModDate DESC, tJobOffer.uCreatedDate DESC LIMIT 99",
					guStatusFilter,guLoginClient,guOrg);
		else
			sprintf(gcQuery,"SELECT uJobOffer,cLabel FROM tJobOffer"
				" WHERE tJobOffer.uStatus=%u"
				" AND uOwner=%u ORDER BY uModDate DESC, uCreatedDate DESC LIMIT 99",
					guStatusFilter,guLoginClient);
	}
	else
	{
		if(guPermLevel>=10)
			//Work In Progress...fix the constants
			sprintf(gcQuery,"SELECT tJobOffer.uJobOffer,tJobOffer.cLabel FROM tJobOffer,tClient"
				" WHERE tClient.uClient=tJobOffer.uOwner"
				" AND tJobOffer.uStatus!=15"//Arch
				" AND tJobOffer.uStatus!=14"//Cancelado
				" AND tJobOffer.uStatus!=10"//Listo para entrega
				" AND tJobOffer.uStatus!=11"//Entregado
				" AND (tJobOffer.uOwner=%u OR tClient.uOwner=%u)"
				" ORDER BY tJobOffer.uModDate DESC, tJobOffer.uCreatedDate DESC LIMIT 99",
					guLoginClient,guOrg);
		else
			sprintf(gcQuery,"SELECT uJobOffer,cLabel FROM tJobOffer"
				" WHERE tJobOffer.uStatus!=15"//Arch
				" AND tJobOffer.uStatus!=14"//Cancelado
				" AND tJobOffer.uStatus!=11"//Entregado
				" AND uOwner=%u ORDER BY uModDate DESC, uCreatedDate DESC LIMIT 99",
					guLoginClient);
	}
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s7) try again later!";
		htmlJobOffer();
	}
	MYSQL_RES *res;
	MYSQL_ROW field;
	res=mysql_store_result(&gMysql);
	unsigned uCount=0;
	uCount=mysql_num_rows(res);

	//Filter
	if(guPermLevel>=10)
		htmlJobOfferFilterSelect(fp);
	else
		if(guStatusFilter==11)
			fprintf(fp,"<a href=\"?gcPage=JobOffer&guStatusFilter=0\">[&#177;]</a>\n"
			" <a href=\"?gcPage=JobOffer&guStatusFilter=11\">%s(%u)</a><br>",
				cStatusLabel(guStatusFilter),uCount);
		else
			fprintf(fp,"<a href=\"?gcPage=JobOffer&guStatusFilter=11\">[&#177;]</a>\n"
			" <a href=\"?gcPage=JobOffer&guStatusFilter=0\">%s(%u)</a><br>",
				cStatusLabel(guStatusFilter),uCount);
	fprintf(fp,"\t\t<form class=\"clearfix\" accept-charset=\"utf-8\" method=\"post\" action=\"/unxsAKApp\">\n");
	fprintf(fp,"\t\t<input type=hidden name=gcPage value=JobOffer >\n");
	fprintf(fp,"\t\t<input type=hidden name=gcFunction value=SetJobOffer >\n");
	fprintf(fp,"\t\t<select onchange=\"this.form.submit()\" class=\"form-control\" id=\"uJobOffer\" name=\"uJobOffer\">\n");
	unsigned uJobOffer=0;
	//unsigned uFirst=1;
	fprintf(fp,"\t\t\t<option value='0'>Crear nuevo trabajo</option>\n");
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uJobOffer);
/*
		if(uFirst && guJobOffer== -1)//guJobOffer = -1 is when no cookies exist.
		{
			guJobOffer=uJobOffer;//This is creating issues with filter
			uFirst=0;
		}
*/
		fprintf(fp,"\t\t\t<option ");
		if(uJobOffer==guJobOffer)
		{
			fprintf(fp,"selected ");
		}
		fprintf(fp," value='%s'>%s</option>\n",field[0],field[1]);
	}
	fprintf(fp,"\t\t</select>\n");
	fprintf(fp,"\t\t</form>\n");
}//void htmlJobOfferSelect(FILE *fp)


void funcRemindPickup(FILE *fp)
{

	fprintf(fp,"<!-- funcRemindPickup()-->\n");
	htmlRemindPickup(fp);
	fprintf(fp,"<!-- End of funcRemindPickup()-->\n");
}//void funcRemindPickup(FILE *fp)


void funcJobOffer(FILE *fp)
{

	fprintf(fp,"<!-- funcJobOffer(Sep7)-->\n");
	htmlJobOfferSelect(fp);
	fprintf(fp,"<!-- End of funcJobOffer()-->\n");


}//void funcJobOffer()

static unsigned guStatus=0;
extern unsigned uStatus;


void htmlStatusSelect(FILE *fp)
{
	sprintf(gcQuery,"SELECT uStatus,cLabel FROM tStatus");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s8) try again later!";
		htmlJobOffer();
	}
	MYSQL_RES *res;
	MYSQL_ROW field;
	res=mysql_store_result(&gMysql);

	fprintf(fp,"  <div class=\"col-sm-9 col-xs-12\">\n");
	fprintf(fp,"    <p class=\"big-para\"><button type=\"button\" data-toggle=\"collapse\""
			" data-target=\"#collapseStatus\" >+</button></p>\n");
	fprintf(fp,"  </div>\n");
	fprintf(fp,"  <div class=\"collapse col-sm-9 col-xs-12\" id=\"collapseStatus\">\n");
	fprintf(fp,"      <div class=\"card card-body\">\n");
	fprintf(fp,"      <form class=\"clearfix\" accept-charset=\"utf-8\" method=\"post\" action=\"/unxsAKApp\">\n");
	fprintf(fp,"      <input type=hidden name=gcPage value=JobOffer >\n");
	fprintf(fp,"      <input type=hidden name=gcFunction value=SetStatus >\n");
	fprintf(fp,"      Cambiar Estado\n");

	fprintf(fp,"      <select onchange=\"this.form.submit()\" class=\"form-control\" id=\"uStatus\" name=\"uStatus\">\n");
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&guStatus);
		fprintf(fp,"      <option ");
		if(uStatus==guStatus)
			fprintf(fp,"selected ");
		fprintf(fp," value='%s'>%s</option>\n",field[0],field[1]);
	}
	fprintf(fp,"      </select>\n");
	fprintf(fp,"      </form>\n");
	fprintf(fp,"      </div>\n");

	//Cambiar dueno
	fprintf(fp,"      <div class=\"card card-body\">\n");
	fprintf(fp,"      <form class=\"clearfix\" accept-charset=\"utf-8\" method=\"post\" action=\"/unxsAKApp\">\n");
	fprintf(fp,"      <input type=hidden name=gcPage value=JobOffer >\n");
	fprintf(fp,"      <input type=hidden name=gcFunction value=ChangeOwner>\n");
	fprintf(fp,"      Cambiar Due&ntilde;o\n");
	fprintf(fp,"      <input title=\"Email del nuevo due&ntilde;o. E.g. someemail@somemail.com\" type=text onchange=\"this.form.submit()\" "
			"class=\"form-control\" id=\"cNewOwner\" name=\"cNewOwner\">\n");
	fprintf(fp,"      </form>\n");
	fprintf(fp,"      </div>\n");

	//Assign new jobnumber
	fprintf(fp,"      <div class=\"card card-body\">\n");
	fprintf(fp,"      <form class=\"clearfix\" accept-charset=\"utf-8\" method=\"post\" action=\"/unxsAKApp\">\n");
	fprintf(fp,"      <input type=hidden name=gcPage value=JobOffer >\n");
	fprintf(fp,"      <input type=hidden name=gcFunction value=ChangeJobOffer>\n");
	fprintf(fp,"      Cambiar Numero de Trabajo\n");
	fprintf(fp,"      <input title=\"New uJobOffer integer\" type=number onchange=\"this.form.submit()\" "
			"class=\"form-control\" id=\"uJobToAssign\" name=\"uJobToAssign\">\n");
	fprintf(fp,"      </form>\n");
	fprintf(fp,"      </div>\n");


}//void htmlStatusSelect(FILE *fp)


void funcStatusSelect(FILE *fp)
{

	if(guPermLevel>=10)
	{
		fprintf(fp,"<!-- funcStatusSelect()-->\n");
		htmlStatusSelect(fp);
		fprintf(fp,"<!-- End of funcStatusSelect()-->\n");
	}

}//void funcStatusSelect()


void htmlInvoiceInteractive(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	float fVarPesos=0.0;

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='cVarPesos'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s9a1) try again later!";
		htmlJobOffer();
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%f",&fVarPesos);

	//Costos y facturacion
	fprintf(fp,"<font size=-1><pre>\n");
	fprintf(fp,"Costos y Facturaci&oacute;n (%u)\n",guJobOffer);
	//Items
	sprintf(gcQuery,"SELECT tItem.cLabel,tItemJob.uQuantity,tItem.mValue,"
				"FORMAT(tItemJob.uQuantity*tItem.mValue,2),"
				"tItemJob.uItemJob"
				" FROM tItemJob,tItem"
				" WHERE tItemJob.uJobOffer=%u AND tItemJob.uItem=tItem.uItem",guJobOffer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s9) try again later!";
		htmlJobOffer();
	}
	res=mysql_store_result(&gMysql);
	unsigned uCount=0;
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"%u) %s %s@$%s $%s"
				" <a href=\"?gcPage=JobOffer&uItemJob=%s&gcFunction=AddItemJob\">[+]</a>"
				" <a href=\"?gcPage=JobOffer&uItemJob=%s&gcFunction=DelItemJob\">[-]</a>"
				" <a href=\"?gcPage=JobOffer&uItemJob=%s&gcFunction=DeleteItem\">[x]</a>\n",
				++uCount,field[0],field[1],field[2],field[3],
					field[4],
					field[4],
					field[4]);
	}

	if(uCount)
	{
		float fDiscount=20.0;
		fDiscount=fGetDiscount(guJobOffer);

		//Total
		float fTotal=0.0;
		float fDiscountTotal=0.0;
		sprintf(gcQuery,"SELECT FORMAT(SUM(tItemJob.uQuantity*tItem.mValue),2) FROM tItemJob,tItem"
				" WHERE tItemJob.uJobOffer=%u AND tItemJob.uItem=tItem.uItem",guJobOffer);
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%f",&fTotal);
			fprintf(fp,"Total $%s\n",field[0]);
			fprintf(fp,"Total pesos (Ambito %2.2f) $%2.2f\n",fVarPesos,fTotal*fVarPesos);
			if(fDiscount>0.0)
			{
				float fDiscountAmount=fTotal*fDiscount/100.0;
				fDiscountTotal=fTotal-fDiscountAmount;
				fprintf(fp,"Total w/%2.0f%% discount of %2.2f: $%2.2f\n",fDiscount,
						fDiscountAmount,fDiscountTotal);
				fprintf(fp,"Total pesos $%2.2f\n",fDiscountTotal*fVarPesos);
			}
			
		}

		//Costos
		sprintf(gcQuery,"SELECT FORMAT(SUM(tItemJob.uQuantity*tItem.mCost),2) FROM tItemJob,tItem"
					" WHERE tItemJob.uJobOffer=%u AND tItemJob.uItem=tItem.uItem",guJobOffer);
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			float fCost=0.0;
			sscanf(field[0],"%f",&fCost);
			if(fDiscountTotal>0.0)
			{
				fprintf(fp,"Profit w/discount: $%2.2f\n",fDiscountTotal-fCost);
			}
			else
			{
				fprintf(fp,"Profit $%2.2f\n",fTotal-fCost);
			}
		}
	}
	else
	{
		fprintf(fp,"No existe factura. Crear agregando un item.\n");
	}

	fprintf(fp,"</pre>\n");
	if(uStatus!=11)
	{
	fprintf(fp,"      <form class=\"clearfix\" accept-charset=\"utf-8\" method=\"post\" action=\"/unxsAKApp\">\n");
	fprintf(fp,"      <input type=hidden name=gcPage value=JobOffer >\n");
	fprintf(fp,"      <input type=hidden name=gcFunction value=AddItem >\n");
	fprintf(fp,"      Agregar Item\n");

	sprintf(gcQuery,"SELECT uItem,cLabel,mValue FROM tItem ORDER BY cLabel");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s82) try again later!";
		htmlJobOffer();
	}
	res=mysql_store_result(&gMysql);

	fprintf(fp,"      <select onchange=\"this.form.submit()\" class=\"form-control\" id=\"uItem\" name=\"uItem\">\n");
	unsigned uItem=0;
	fprintf(fp,"      <option> </option> ");//empty option for invoice with nothing
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uItem);
		fprintf(fp,"      <option ");
		fprintf(fp," value='%s'>%s %s</option>\n",field[0],field[1],field[2]);
	}
	fprintf(fp,"      </select>\n");
	fprintf(fp,"      </form>\n");
	}

}//void htmlInvoiceInteractive(FILE *fp)


float fGetDiscount(unsigned uJobOffer)
{
	float fGetDiscount=0.0;
	sprintf(gcQuery,"SELECT tClient.cInfo FROM tClient,tJobOffer"
			" WHERE tJobOffer.uOwner=tClient.uClient AND tJobOffer.uJobOffer=%u LIMIT 1",uJobOffer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return(1.0);
	MYSQL_RES *res;
	MYSQL_ROW field;
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		unsigned uDiscount=0;
		sscanf(field[0],"cuDiscount=%u;",&uDiscount);
		fGetDiscount=(float)uDiscount;
	}
	return(fGetDiscount);
}//float fGetDiscount(unsigned uJobOffer)


void htmlInvoiceViewOnly(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	float fVarPesos=0.0;

	if(!guJobOffer || guJobOffer==(-1)) return;

	sprintf(gcQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='cVarPesos'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s9a1) try again later!";
		htmlJobOffer();
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%f",&fVarPesos);

	//Facturacion
	fprintf(fp,"<font size=-1><pre>\n");
	fprintf(fp,"Facturaci&oacute;n (%u)\n",guJobOffer);
	//Items
	sprintf(gcQuery,"SELECT tItem.cLabel,tItemJob.uQuantity,tItem.mValue,"
				"FORMAT(tItemJob.uQuantity*tItem.mValue,2)"
				" FROM tItemJob,tItem"
				" WHERE tItemJob.uJobOffer=%u AND tItemJob.uItem=tItem.uItem",guJobOffer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s9) try again later!";
		htmlJobOffer();
	}
	res=mysql_store_result(&gMysql);
	unsigned uCount=0;
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"%u) %s %s@$%s $%s\n",
				++uCount,field[0],field[1],field[2],field[3]);
	}



	if(uCount)
	{
		float fDiscount=0.0;
		fDiscount=fGetDiscount(guJobOffer);

		//Total
		sprintf(gcQuery,"SELECT FORMAT(SUM(tItemJob.uQuantity*tItem.mValue),2) FROM tItemJob,tItem"
				" WHERE tItemJob.uJobOffer=%u AND tItemJob.uItem=tItem.uItem",guJobOffer);
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			float fTotal=0.0;
			sscanf(field[0],"%f",&fTotal);
			fprintf(fp,"Total $%s\n",field[0]);
			fprintf(fp,"Total pesos (Ambito %2.2f) $%2.2f\n",fVarPesos,fTotal*fVarPesos);
			if(fDiscount>0.0)
			{
				float fDiscountAmount=fTotal*fDiscount/100.0;
				float fDiscountTotal=fTotal-fDiscountAmount;
				fprintf(fp,"Total w/%2.0f%% discount of %2.2f: $%2.2f\n",fDiscount,
						fDiscountAmount,fDiscountTotal);
				fprintf(fp,"Total pesos $%2.2f\n",fDiscountTotal*fVarPesos);
			}
		}
	}
	else
	{
		fprintf(fp,"No hay factura disponible.\n");
	}

	fprintf(fp,"</pre>\n");

}//void htmlInvoiceViewOnly(FILE *fp)


void funcInvoice(FILE *fp)
{

	fprintf(fp,"<!-- funcInvoice()-->\n");
	if(guPermLevel>=10)
		htmlInvoiceInteractive(fp);
	else
		htmlInvoiceViewOnly(fp);
	fprintf(fp,"<!-- End of funcInvoice()-->\n");

}//void funcInvoice()


void htmlUserInvoice(FILE *fp);

void funcUserInvoices(FILE *fp)
{

	unsigned uSaveJobOffer=guJobOffer;
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cJobOwner[64]={""};

	if(guPermLevel>=10)
	{
		sprintf(gcQuery,"SELECT tClient.cLabel FROM tClient,tJobOffer"
			" WHERE tClient.uClient=tJobOffer.uOwner AND tJobOffer.uJobOffer=%u",guJobOffer);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			gcMessage="Unexpected error (s9a1) try again later!";
			htmlJobOffer();
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sprintf(cJobOwner,"Cliente: %.31s",field[0]);

		fprintf(fp,"<!-- funcUserInvoices()-->\n");
		sprintf(gcQuery,"SELECT tJobOffer.uJobOffer"
			" FROM tJobOffer,tJobOffer AS tJobOffer2"
			" WHERE tJobOffer2.uJobOffer=%u AND tJobOffer2.uOwner=tJobOffer.uOwner ORDER BY tJobOffer.uModDate DESC",guJobOffer);
	}
	else
	{
		sprintf(gcQuery,"SELECT tJobOffer.uJobOffer"
			" FROM tJobOffer"
			" WHERE tJobOffer.uOwner=%u ORDER BY tJobOffer.uModDate DESC",guLoginClient);
	}
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s200) try again later!";
		htmlJobOffer();
	}
	res=mysql_store_result(&gMysql);
	fprintf(fp,"<font size=-1><pre>\n");
	if(guPermLevel>=10)
		fprintf(fp,"%s\n",cJobOwner);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&guJobOffer);
		if(guJobOffer)
			htmlUserInvoice(fp);
	}


	//Also display active job offer
	if(guPermLevel>=10)
	{
		fprintf(fp,"\nOpen Jobs:\n");
		sprintf(gcQuery,"SELECT tJobOffer.uJobOffer,tJobOffer.cLabel,tClient.cLabel FROM tJobOffer,tClient"
			" WHERE tJobOffer.uStatus!=11 AND tJobOffer.uStatus!=14 AND tJobOffer.uStatus!=15"
			" AND tClient.uClient=tJobOffer.uOwner");
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			gcMessage="Unexpected error (s9f1) try again later!";
			htmlJobOffer();
		}
		res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"<a href=unxsAKApp?gcPage=JobOffer&uJobOffer=%s >%s/%s</a>\n",field[0],field[1],field[2]);
		}
	}

	fprintf(fp,"</pre>\n");
	fprintf(fp,"<!-- End of funcUserInvoices()-->\n");

	guJobOffer=uSaveJobOffer;

}//void funcUserInvoices()


void htmlUserInvoice(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cLabel[100]={""};
	char cDate[100]={""};

	if(!guJobOffer || guJobOffer==(-1)) return;

	//Facturacion
	sprintf(gcQuery,"SELECT cLabel,FROM_UNIXTIME(uModDate,'%%d/%%m/%%Y') FROM tJobOffer WHERE uJobOffer=%u",guJobOffer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s9a1) try again later!";
		htmlJobOffer();
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(cLabel,"%.99s",field[0]);
		sprintf(cDate,"%.31s",field[1]);
	}

	fprintf(fp,"\nFacturaci&oacute;n \"%s\" %s (%u)\n",cLabel,cDate,guJobOffer);
	//Items
	sprintf(gcQuery,"SELECT tItem.cLabel,tItemJob.uQuantity,tItem.mValue,"
				"FORMAT(tItemJob.uQuantity*tItem.mValue,2)"
				" FROM tItemJob,tItem"
				" WHERE tItemJob.uJobOffer=%u AND tItemJob.uItem=tItem.uItem",guJobOffer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s9) try again later!";
		htmlJobOffer();
	}
	res=mysql_store_result(&gMysql);
	unsigned uCount=0;
	while((field=mysql_fetch_row(res)))
	{
		fprintf(fp,"%u) %s %s@$%s $%s\n",
				++uCount,field[0],field[1],field[2],field[3]);
	}



	if(uCount)
	{
		float fDiscount=0.0;
		fDiscount=fGetDiscount(guJobOffer);

		//Total
		sprintf(gcQuery,"SELECT FORMAT(SUM(tItemJob.uQuantity*tItem.mValue),2) FROM tItemJob,tItem"
				" WHERE tItemJob.uJobOffer=%u AND tItemJob.uItem=tItem.uItem",guJobOffer);
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			float fTotal=0.0;
			sscanf(field[0],"%f",&fTotal);
			fprintf(fp,"Total $%s\n",field[0]);
			if(fDiscount>0.0)
			{
				float fDiscountAmount=fTotal*fDiscount/100.0;
				float fDiscountTotal=fTotal-fDiscountAmount;
				fprintf(fp,"Total w/%2.0f%% discount of %2.2f: $%2.2f\n",fDiscount,
						fDiscountAmount,fDiscountTotal);
			}
		}
	}
	else
	{
		fprintf(fp,"No hay factura disponible.\n");
	}


}//void htmlUserInvoice(FILE *fp)

