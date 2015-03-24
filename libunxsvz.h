/*
FILE 
	unxsVZ/libunxsvz.h
	$Id$
PURPOSE
	Header for libunxsvz.c linunxsvz.a library.
	Move the jobqueue out of the cgi.
NOTES
AUTHOR/LEGAL
	(C) 2011-2014 Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
FREE HELP
	support @ openisp . net
	supportgrp @ unixservice . com
	Join mailing list: https://lists.openisp.net/mailman/listinfo/unxsvz
*/
#include "mysqlrad.h"

void ErrorMsg(const char *cText);
void SetContainerStatus(unsigned uContainer,unsigned uStatus);
void SetContainerNode(unsigned uContainer,unsigned uNode);
void SetContainerDatacenter(unsigned uContainer,unsigned uDatacenter);
unsigned uCheckMaxContainers(unsigned uNode);
unsigned uCheckMaxCloneContainers(unsigned uNode);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);
const char *ForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);
unsigned GetConfiguration(const char *cName,char *cValue,
		unsigned uDatacenter,
		unsigned uNode,
		unsigned uContainer,
		unsigned uHtml);
unsigned ConnectToOptionalUBCDb(unsigned uDatacenter,unsigned uPrivate);
void unxsVZFake(const char *cText);
unsigned CreateDNSJob(unsigned uIPv4,unsigned uOwner,char const *cOptionalIPv4,char const *cHostname,
				unsigned uDatacenter,unsigned uCreatedBy,unsigned uContainer,unsigned uNode);
unsigned uNodeCommandJob(unsigned uDatacenter, unsigned uNode, unsigned uContainer,
			unsigned uOwner, unsigned uLoginClient, unsigned uConfiguration, char *cArgs);
void SetNodeProp(char const *cName,char const *cValue,unsigned uNode);
void SetDatacenterProp(char const *cName,char const *cValue,unsigned uDatacenter);
unsigned uGetPrimaryContainerGroup(unsigned uContainer);
time_t cStartDateToUnixTime(char *cStartDate);
time_t cStartTimeToUnixTime(char *cStartTime);
char *ToLower(char *cInput);
void unxsVZLog(unsigned uTablePK, char *cTableName, char *cLogEntry);
unsigned unxsBindPBXRecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
	unsigned uOwner,unsigned uCreatedBy);
unsigned unxsBindARecordJob(unsigned uDatacenter,unsigned uNode,unsigned uContainer,const char *cJobData,
	unsigned uOwner,unsigned uCreatedBy);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);
void GetNodePropUBC(const unsigned uNode,const char *cName,char *cValue);
void GetDatacenterProp(const unsigned uDatacenter,const char *cName,char *cValue);
void logfileLine(const char *cFunction,const char *cLogline);
void GetContainerProp(const unsigned uContainer,const char *cName,char *cValue);
void GetContainerPropUBC(const unsigned uContainer,const char *cName,char *cValue);
unsigned SetContainerPropertyUBC(const unsigned uContainer,const char *cPropertyName,const  char *cPropertyValue);
unsigned SetContainerProperty(const unsigned uContainer,const char *cPropertyName,const  char *cPropertyValue);
void GetGroupProp(const unsigned uGroup,const char *cName,char *cValue);
void GetClientProp(const unsigned uClient,const char *cName,char *cValue);
void GetIPProp(const unsigned uIP,const char *cName,char *cValue);
void GetIPPropFromHost(const char *cHostIP,const char *cName,char *cValue);

//Global vars

extern MYSQL gMysql;
extern MYSQL gMysqlUBC;
extern char *gcUBCDBIP0;//must be set to DBIP0 to be used as alternative
extern char *gcUBCDBIP1;
extern char gcUBCDBIP0Buffer[];
extern char gcUBCDBIP1Buffer[];
extern FILE *gLfp;
extern char gcCommand[];
extern char cLogKey[];

extern int guPermLevel;
extern unsigned guLoginClient;
extern unsigned guReseller;
extern unsigned guCompany;
extern char gcCompany[];
extern char gcUser[];
extern char gcLogin[];
extern char gcHost[];
extern char gcHostname[];
extern unsigned guDatacenter;

extern char gcFunction[];
extern char gcQuery[];
extern char cStartDate[];
extern char cStartTime[];
