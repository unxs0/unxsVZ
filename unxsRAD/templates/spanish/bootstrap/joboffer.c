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

void htmlJobOfferSelect(FILE *fp)
{
	if(guPermLevel>=10)
		sprintf(gcQuery,"SELECT tJobOffer.uJobOffer,tJobOffer.cLabel FROM tJobOffer,tClient"
				" WHERE tClient.uClient=tJobOffer.uOwner"
				" AND tJobOffer.uStatus!=6"
				" AND tJobOffer.uStatus!=7"
				" AND (tJobOffer.uOwner=%u OR tClient.uOwner=%u)"
				" ORDER BY tJobOffer.uModDate DESC, tJobOffer.uCreatedDate DESC LIMIT 99",
					guLoginClient,guOrg);
	else
		sprintf(gcQuery,"SELECT uJobOffer,cLabel FROM tJobOffer"
				" WHERE tJobOffer.uStatus!=6"
				" AND tJobOffer.uStatus!=7"
				" AND uOwner=%u ORDER BY uModDate DESC, uCreatedDate DESC LIMIT 99",
					guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		gcMessage="Unexpected error (s7) try again later!";
		htmlJobOffer();
	}
	MYSQL_RES *res;
	MYSQL_ROW field;
	res=mysql_store_result(&gMysql);

	fprintf(fp,"\t\t<input type=hidden name=gcPage value=JobOffer >\n");
	fprintf(fp,"\t\t<input type=hidden name=gcFunction value=SetJobOffer >\n");
	fprintf(fp,"\t\t<select onchange=\"this.form.submit()\" class=\"form-control\" id=\"uJobOffer\" name=\"uJobOffer\">\n");
	fprintf(fp,"\t\t\t<option value='0'>Crear nuevo trabajo</option>\n");
	unsigned uJobOffer=0;
	unsigned uFirst=1;
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uJobOffer);
		if(uFirst && guJobOffer== -1)
		{
			guJobOffer=uJobOffer;
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

	fprintf(fp,"      <div class=\"card card-body\">\n");
	fprintf(fp,"      <form class=\"clearfix\" accept-charset=\"utf-8\" method=\"post\" action=\"/unxsAKApp\">\n");
	fprintf(fp,"      <input type=hidden name=gcPage value=JobOffer >\n");
	fprintf(fp,"      <input type=hidden name=gcFunction value=ChangeOwner>\n");
	fprintf(fp,"      Cambiar Due&ntilde;o\n");
	fprintf(fp,"      <input title=\"Email del nuevo due&ntilde;o. E.g. someemail@somemail.com\" type=text onchange=\"this.form.submit()\" "
			"class=\"form-control\" id=\"cNewOwner\" name=\"cNewOwner\">\n");
	fprintf(fp,"      </form>\n");
	fprintf(fp,"      </div>\n");

	fprintf(fp,"    </form>\n");
	fprintf(fp,"  </div>\n");
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


