/*
FILE 
	{{cProject}}/interfaces/bootstrap/judge.c
	template/new/bootstrap/judge.c
AUTHOR/LEGAL
	(C) 2010-2020 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	For judging heat scores
*/

#include "interface.h"

//TOC
void ProcessJudgeVars(pentry entries[], int x);
void JudgeGetHook(entry gentries[],int x);
void JudgeCommands(pentry entries[], int x);
void htmlJudge(void);
//extern
void unxsvzLog(unsigned uTablePK,char *cTableName,char *cLogEntry,unsigned guPermLevel,unsigned guLoginClient,char *gcLogin,char *gcHost);

void ProcessJudgeVars(pentry entries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		
	}

}//void ProcessJobOfferVars(pentry entries[], int x)


void JudgeGetHook(entry gentries[],int x)
{
	htmlJudge();

}//void JudgeGetHook(entry gentries[],int x)


void JudgeCommands(pentry entries[], int x)
{
	htmlJudge();

}//void UserCommands(pentry entries[], int x)


void htmlJudge(void)
{
	htmlHeader("Judge","Default.Header");
	htmlJudgePage("Judge","Judge.Body");
	htmlFooter("Default.Footer");

}//void htmlJudge(void)


void htmlJudgePage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uDEFAULT,uBOOTSTRAP);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="";
			
			template.cpName[2]="cMessage";
			template.cpValue[2]=gcMessage;

			template.cpName[3]="gcBrand";
			template.cpValue[3]=gcBrand;

			template.cpName[4]="gcName";
			template.cpValue[4]=gcName;

			template.cpName[5]="gcOrgName";
			template.cpValue[5]=gcOrgName;

			template.cpName[6]="";

			printf("\n<!-- Start htmlLoginPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlLoginPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>d1 %s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}


}//htmlJudgePage()


void funcHeatScoreTable(FILE *fp)
{
	fprintf(fp,"<div class=\"sTable\">");

	fprintf(fp,"<div class=\"sTableRow\">");
	fprintf(fp,"<div class=\"sTableCellGreen\">To End: 0:00</div>");
	fprintf(fp,"<div class=\"sTableCellBlack\">Ronda 1</div>");
	fprintf(fp,"<div class=\"sTableCellBlack\">Heat 1</div>");
	fprintf(fp,"</div>");

	fprintf(fp,"</div>");

	fprintf(fp,"<br><div class=\"sTable\">");

	fprintf(fp,"<div class=\"sTableRow\">");
	fprintf(fp,"<div class=\"sTableCell\">1</div>");
	fprintf(fp,"<div class=\"sTableCellBlack\">K. LANGEREE</div>");
	fprintf(fp,"<div class=\"sTableCellBlackBold\">0.00</div>");
	fprintf(fp,"<div class=\"sTableCellBlack\">0.00</div>");
	fprintf(fp,"<div class=\"sTableCellBlack\">0.00</div>");
	fprintf(fp,"<div class=\"sTableCellBlack\">0.00</div>");
	fprintf(fp,"<div class=\"sTableCellBlue\">Needs 0.00</div>");
	fprintf(fp,"</div>");

	fprintf(fp,"<div class=\"sTableRow\">");
	fprintf(fp,"<div class=\"sTableCell\">2</div>");
	fprintf(fp,"<div class=\"sTableCellBlack\">A. HADLOW</div>");
	fprintf(fp,"<div class=\"sTableCellBlackBold\">0.00</div>");
	fprintf(fp,"<div class=\"sTableCellBlack\">0.00</div>");
	fprintf(fp,"<div class=\"sTableCellBlack\">0.00</div>");
	fprintf(fp,"<div class=\"sTableCellBlack\">0.00</div>");
	fprintf(fp,"<div class=\"sTableCellBlue\">Needs 0.00</div>");
	fprintf(fp,"</div>");

	fprintf(fp,"</div>");

}//void funcHeatScoreTable(FILE *fp)
