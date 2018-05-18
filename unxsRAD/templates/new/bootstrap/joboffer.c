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

unsigned guJobOffer= -1;

void htmlJobOfferSelect(FILE *fp)
{
	sprintf(gcQuery,"SELECT uJobOffer,cLabel FROM tJobOffer"
				" WHERE uOwner=%u ORDER BY uModDate DESC, uCreatedDate DESC LIMIT 7",guLoginClient);
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
	fprintf(fp,"\t\t\t<option value='0'>Create new job offer</option>\n");
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

	fprintf(fp,"<!-- funcJobOffer()-->\n");
	htmlJobOfferSelect(fp);
	fprintf(fp,"<!-- End of funcJobOffer()-->\n");


}//void funcJobOffer()
