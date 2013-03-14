/*
FILE
	tNode source code of unxsVZ.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	$Id$
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tnodefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);

//Table Variables
//Table Variables
//uNode: Primary Key
static unsigned uNode=0;
//cLabel: Short label
static char cLabel[33]={""};
//uDatacenter: Belongs to this Datacenter
static unsigned uDatacenter=0;
static char cuDatacenterPullDown[256]={""};
//uStatus: Status of Hardware Node
static unsigned uStatus=0;
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



#define VAR_LIST_tNode "tNode.uNode,tNode.cLabel,tNode.uDatacenter,tNode.uStatus,tNode.uOwner,tNode.uCreatedBy,tNode.uCreatedDate,tNode.uModBy,tNode.uModDate"

 //Local only
void Insert_tNode(void);
void Update_tNode(char *cRowid);
void ProcesstNodeListVars(pentry entries[], int x);

 //In tNodefunc.h file included below
void ExtProcesstNodeVars(pentry entries[], int x);
void ExttNodeCommands(pentry entries[], int x);
void ExttNodeButtons(void);
void ExttNodeNavBar(void);
void ExttNodeGetHook(entry gentries[], int x);
void ExttNodeSelect(void);
void ExttNodeSelectRow(void);
void ExttNodeListSelect(void);
void ExttNodeListFilter(void);
void ExttNodeAuxTable(void);

#include "tnodefunc.h"

 //Table Variables Assignment Function
void ProcesstNodeVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uNode"))
			sscanf(entries[i].val,"%u",&uNode);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uDatacenter"))
			sscanf(entries[i].val,"%u",&uDatacenter);
		else if(!strcmp(entries[i].name,"cuDatacenterPullDown"))
		{
			sprintf(cuDatacenterPullDown,"%.255s",entries[i].val);
			uDatacenter=ReadPullDown("tDatacenter","cLabel",cuDatacenterPullDown);
		}
		else if(!strcmp(entries[i].name,"uStatus"))
			sscanf(entries[i].val,"%u",&uStatus);
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

	//After so we can overwrite form data if needed.
	ExtProcesstNodeVars(entries,x);

}//ProcesstNodeVars()


void ProcesstNodeListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uNode);
                        guMode=2002;
                        tNode("");
                }
        }
}//void ProcesstNodeListVars(pentry entries[], int x)


int tNodeCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttNodeCommands(entries,x);

	if(!strcmp(gcFunction,"tNodeTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tNodeList();
		}

		//Default
		ProcesstNodeVars(entries,x);
		tNode("");
	}
	else if(!strcmp(gcFunction,"tNodeList"))
	{
		ProcessControlVars(entries,x);
		ProcesstNodeListVars(entries,x);
		tNodeList();
	}

	return(0);

}//tNodeCommands()


void tNode(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttNodeSelectRow();
		else
			ExttNodeSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetNode();
				unxsVZ("New tNode table created");
                	}
			else
			{
				htmlPlainTextError(mysql_error(&gMysql));
			}
        	}

		res=mysql_store_result(&gMysql);
		if((guI=mysql_num_rows(res)))
		{
			if(guMode==6)
			{
			sprintf(gcQuery,"SELECT _rowid FROM tNode WHERE uNode=%u"
						,uNode);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uNode);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uDatacenter);
		sscanf(field[3],"%u",&uStatus);
		sscanf(field[4],"%u",&uOwner);
		sscanf(field[5],"%u",&uCreatedBy);
		sscanf(field[6],"%lu",&uCreatedDate);
		sscanf(field[7],"%u",&uModBy);
		sscanf(field[8],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: Hardware Node",2);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tNodeTools>");
	printf("<input type=hidden name=gluRowid value=%lu>",gluRowid);
	if(guI)
	{
		if(guMode==6)
			//printf(" Found");
			printf(LANG_NBR_FOUND);
		else if(guMode==5)
			//printf(" Modified");
			printf(LANG_NBR_MODIFIED);
		else if(guMode==4)
			//printf(" New");
			printf(LANG_NBR_NEW);
		printf(LANG_NBRF_SHOWING,gluRowid,guI);
	}
	else
	{
		if(!cResult[0])
		//printf(" No records found");
		printf(LANG_NBR_NORECS);
	}
	if(cResult[0]) printf("%s",cResult);
	printf("</td></tr>");
	printf("<tr><td valign=top width=25%%>");

        ExttNodeButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tNode Record Data",100);

	if(guMode==2000 || guMode==2002)
		tNodeInput(1);
	else
		tNodeInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttNodeAuxTable();

	Footer_ism3();

}//end of tNode();


void tNodeInput(unsigned uMode)
{
	if(uNode && uDatacenter)
	{
		char cConfigBuffer[256]={""};

		GetConfiguration("cNodeTrafficDirURL",cConfigBuffer,uDatacenter,0,0,0);
		if(cConfigBuffer[0])
		{
	
			OpenRow("Graph","black");
			printf("<a href=%s/%s.png><img src=%s/%s.png border=0></a>\n",
					cConfigBuffer,cLabel,cConfigBuffer,cLabel);
			printf("</td></tr>\n");
		}
	}

//uContainer

//uNode
	OpenRow(LANG_FL_tNode_uNode,"black");
	printf("<input title='%s' type=text name=uNode value=%u size=16 maxlength=10 "
,LANG_FT_tNode_uNode,uNode);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uNode value=%u >\n",uNode);
	}
//cLabel
	OpenRow(LANG_FL_tNode_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tNode_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uDatacenter
	OpenRow(LANG_FL_tNode_uDatacenter,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownOwner("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,1);
	else
		tTablePullDownOwner("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,0);
//uStatus
	OpenRow(LANG_FL_tNode_uStatus,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uStatus value=%u >\n",ForeignKey("tStatus","cLabel",uStatus),uStatus);
	}
	else
	{
	printf("%s<input type=hidden name=uStatus value=%u >\n",ForeignKey("tStatus","cLabel",uStatus),uStatus);
	}
//uOwner
	OpenRow(LANG_FL_tNode_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tNode_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tNode_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tNode_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tNode_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");

	char cGraph0[256]={""};
	GetNodeProp(uNode,"Graph0",cGraph0);
	if(cGraph0[0])
	{
		OpenRow("Optional Graph","black");
		printf("<img src=%s border=0>\n",cGraph0);
		printf("</tr>\n");
	}



}//void tNodeInput(unsigned uMode)


void NewtNode(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uNode FROM tNode\
				WHERE uNode=%u"
							,uNode);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tNode("<blink>Record already exists");
		tNode(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tNode();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uNode=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tNode",uNode);
	unxsVZLog(uNode,"tNode","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uNode);
	tNode(gcQuery);
	}

}//NewtNode(unsigned uMode)


void DeletetNode(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tNode WHERE uNode=%u AND ( uOwner=%u OR %u>9 )"
					,uNode,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tNode WHERE uNode=%u"
					,uNode);
#endif
	MYSQL_RUN;
	//tNode("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsVZLog(uNode,"tNode","Del");
#endif
		tNode(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsVZLog(uNode,"tNode","DelError");
#endif
		tNode(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetNode(void)


void Insert_tNode(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tNode SET uNode=%u,cLabel='%s',uDatacenter=%u,uStatus=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uNode
			,TextAreaSave(cLabel)
			,uDatacenter
			,uStatus
			,uOwner
			,uCreatedBy
			);

	MYSQL_RUN;

}//void Insert_tNode(void)


void Update_tNode(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tNode SET uNode=%u,cLabel='%s',uDatacenter=%u,uStatus=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uNode
			,TextAreaSave(cLabel)
			,uDatacenter
			,uStatus
			,uModBy
			,cRowid);

	MYSQL_RUN;

}//void Update_tNode(void)


void ModtNode(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tNode.uNode,\
				tNode.uModDate\
				FROM tNode,tClient\
				WHERE tNode.uNode=%u\
				AND tNode.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uNode,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uNode,uModDate FROM tNode\
				WHERE uNode=%u"
						,uNode);
#else
	sprintf(gcQuery,"SELECT uNode FROM tNode\
				WHERE uNode=%u"
						,uNode);
#endif

	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tNode("<blink>Record does not exist");
	if(i<1) tNode(LANG_NBR_RECNOTEXIST);
	//if(i>1) tNode("<blink>Multiple rows!");
	if(i>1) tNode(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tNode(LANG_NBR_EXTMOD);
#endif

	Update_tNode(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tNode",uNode);
	unxsVZLog(uNode,"tNode","Mod");
#endif
	tNode(gcQuery);

}//ModtNode(void)


void tNodeList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttNodeListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tNodeList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttNodeListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uNode<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uDatacenter<td><font face=arial,helvetica color=white>uStatus<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



	mysql_data_seek(res,guStart-1);

	for(guN=0;guN<(guEnd-guStart+1);guN++)
	{
		field=mysql_fetch_row(res);
		if(!field)
		{
			printf("<tr><td><font face=arial,helvetica>End of data</table>");
			Footer_ism3();
		}
			if(guN % 2)
				printf("<tr bgcolor=#BBE1D3>");
			else
				printf("<tr>");
		time_t luTime6=strtoul(field[6],NULL,10);
		char cBuf6[32];
		if(luTime6)
			ctime_r(&luTime6,cBuf6);
		else
			sprintf(cBuf6,"---");
		time_t luTime8=strtoul(field[8],NULL,10);
		char cBuf8[32];
		if(luTime8)
			ctime_r(&luTime8,cBuf8);
		else
			sprintf(cBuf8,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey("tDatacenter","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tStatus","cLabel",strtoul(field[3],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[4],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[5],NULL,10))
			,cBuf6
			,ForeignKey("tClient","cLabel",strtoul(field[7],NULL,10))
			,cBuf8
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tNodeList()


void CreatetNode(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tNode ( uNode INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uDatacenter INT UNSIGNED NOT NULL DEFAULT 0,unique (cLabel,uDatacenter), uStatus INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetNode()

