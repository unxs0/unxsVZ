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

unsigned guJobOffer=0;

void htmlJobOfferSelect(FILE *fp)
{
	if(!guJobOffer)
	{
		fprintf(fp,"None found.");
	}
	else
	{
	}
}//void htmlJobOfferSelect(FILE *fp)

void funcJobOffer(FILE *fp)
{

	fprintf(fp,"<!-- funcJobOffer()-->\n");
	htmlJobOfferSelect(fp);
	fprintf(fp,"<!-- End of funcJobOffer()-->\n");


}//void funcJobOffer()
