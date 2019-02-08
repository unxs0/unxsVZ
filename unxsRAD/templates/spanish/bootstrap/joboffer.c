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
			//Work In Progress
			sprintf(gcQuery,"SELECT tJobOffer.uJobOffer,tJobOffer.cLabel FROM tJobOffer,tClient"
				" WHERE tClient.uClient=tJobOffer.uOwner"
				" AND tJobOffer.uStatus!=15"//Archivado
				" AND tJobOffer.uStatus!=14"//Cancelado
				" AND tJobOffer.uStatus!=11"//Entregado
				" AND tJobOffer.uStatus!=13"//Trabajo Postergado
				" AND tJobOffer.uStatus!=10"//Listo Para Entrega
				" AND tJobOffer.uStatus!=1"//Trabajo Nuevo
				" AND tJobOffer.uStatus!=2"//Trabajo Aceptado
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

	if(guPermLevel>=10)
		fprintf(fp,"<a href=\"?gcPage=JobOffer&gcFunction=IncSF&guStatusFilter=%u\">[+]</a>"
			" <a href=\"?gcPage=JobOffer&gcFunction=DecSF&guStatusFilter=%u\">[-]</a>"
			" <a href=\"?gcPage=JobOffer&gcFunction=DelSF\">[x]</a>"
			" <a href=\"?gcPage=JobOffer&gcFunction=IncSF&guStatusFilter=%u\">%s(%u)</a><br>",
				guStatusFilter,
				guStatusFilter,
				guStatusFilter,cStatusLabel(guStatusFilter),uCount);
	else
		fprintf(fp,"<a href=\"?gcPage=JobOffer&guStatusFilter=11\">[+]</a>\n"
			" <a href=\"?gcPage=JobOffer&guStatusFilter=0\">[-]</a>\n"
			" <a href=\"?gcPage=JobOffer&guStatusFilter=%u\">%s(%u)</a><br>",
				guStatusFilter,cStatusLabel(guStatusFilter),uCount);
	fprintf(fp,"\t\t<input type=hidden name=gcPage value=JobOffer >\n");
	fprintf(fp,"\t\t<input type=hidden name=gcFunction value=SetJobOffer >\n");
	fprintf(fp,"\t\t<select onchange=\"this.form.submit()\" class=\"form-control\" id=\"uJobOffer\" name=\"uJobOffer\">\n");
	unsigned uJobOffer=0;
	unsigned uFirst=1;
	fprintf(fp,"\t\t\t<option value='0'>Crear nuevo trabajo</option>\n");
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uJobOffer);
		if(uFirst && guJobOffer== -1)//guJobOffer = -1 is when no cookies exist.
		{
			guJobOffer=uJobOffer;//This is creating issues with filter
			uFirst=0;
		}
		fprintf(fp,"\t\t\t<option ");
		if(uJobOffer==guJobOffer)
			fprintf(fp,"selected ");
		fprintf(fp," value='%s'>%s</option>\n",field[0],field[1]);
	}
	fprintf(fp,"\t\t</select>\n");
}//void htmlJobOfferSelect(FILE *fp)


void funcJobOffer(FILE *fp)
{

	fprintf(fp,"<!-- funcJobOffer(d1)-->\n");
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
		//Total
		sprintf(gcQuery,"SELECT FORMAT(SUM(tItemJob.uQuantity*tItem.mValue),2) FROM tItemJob,tItem"
				" WHERE tItemJob.uJobOffer=%u AND tItemJob.uItem=tItem.uItem",guJobOffer);
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"Total $%s\n",field[0]);
		}

		//Costos
		sprintf(gcQuery,"SELECT FORMAT(SUM(tItemJob.uQuantity*tItem.mValue-tItemJob.uQuantity*tItem.mCost),2) FROM tItemJob,tItem"
					" WHERE tItemJob.uJobOffer=%u AND tItemJob.uItem=tItem.uItem",guJobOffer);
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"Profit $%s\n",field[0]);
		}
	}
	else
	{
		fprintf(fp,"No existe factura. Crear agregando un item.\n");
	}

	fprintf(fp,"</pre>\n");
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

}//void htmlInvoiceInteractive(FILE *fp)


void htmlInvoiceViewOnly(FILE *fp)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

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
		//Total
		sprintf(gcQuery,"SELECT FORMAT(SUM(tItemJob.uQuantity*tItem.mValue),2) FROM tItemJob,tItem"
				" WHERE tItemJob.uJobOffer=%u AND tItemJob.uItem=tItem.uItem",guJobOffer);
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			fprintf(fp,"Total $%s\n",field[0]);
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

}//void funcStatusSelect()


