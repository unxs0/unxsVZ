/*
FILE 
	unxsVZ/uSIPSwitch/unxsSIPHealth/endpoint.c
	$Id$
PURPOSE
	Parse OPTIONS SIP cMessage
AUTHOR/LEGAL
	(C) 2012-2015 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
DETAILS
	This module requires that these values be parsed from header 
	INVITE:
		cFirstLine
		cDID
		cGatewayIP
		uGatewayPort
		cCallID
		cCSeq
		uMaxForwards

AVAILABLE DATA FROM readEv()
	char cMessage[2048]={""};
	char cSourceIP[INET_ADDRSTRLEN]={""};
	unsigned uSourcePort=ntohs(sourceAddr.sin_port);
*/



//This is used for request/reply determination
char *cp,*cp1;
char cFirstLine[100]={""};
if((cp1=strchr(cMessage,'\r')))
{
	*cp1=0;
	sprintf(cFirstLine,"%.99s",cMessage);
	*cp1='\r';
}
if(guLogLevel>3 && cFirstLine[0])
	logfileLine("endpoint.c cFirstLine",cFirstLine);
//cFirstLine

if((cp=strstr(cMessage,"CSeq: ")))
{
	if((cp1=strchr(cp+strlen("CSeq: "),'\r')))
	{
		*cp1=0;
		sprintf(cCSeq,"%.63s",cp+strlen("CSeq: "));
		*cp1='\r';
	}
}
if(guLogLevel>3 && cCSeq[0])
	logfileLine("endpoint.c cCSeq",cCSeq);
if(guLogLevel>0 && !cCSeq[0])
	logfileLine("endpoint.c","No cCSeq");
//cCSeq

//To: <sip:7073613110@64.2.142.90:5060>
//To: <sip:usips.sipmonster.net> 
char cTo[100]={""};
if((cp=strstr(cMessage,"To: ")))
{
	if((cp1=strchr(cp+strlen("To: "),'\r')))
	{
		*cp1=0;
		sprintf(cTo,"%.99s",cp+strlen("To: "));
		*cp1='\r';
	}
	if(guLogLevel>3)
		logfileLine("endpoint.c cTo",cTo);
}//cTo
char cDID[32]={""};
char cGateway[100]={""};
unsigned uGatewayPort=0;
if(cTo[0])
{
	if((cp=strstr(cTo,"sip:")))
	{
		if((cp1=strchr(cTo,'@')))
		{
			*cp1=0;
			sprintf(cDID,"%.31s",cp+4);
			if(sscanf(cp1+1,"%[0-9\\.]:%u",cGateway,&uGatewayPort)!=2)
			{
				if(guLogLevel>3)
					logfileLine("endpoint.c","cTo sip: sscanf error");
				if(sscanf(cp1+1,"%[0-9\\.]",cGateway)!=1)
				{
					if(guLogLevel>3)
						logfileLine("endpoint.c","cTo sip: sscanf error");
				}
				else
				{
					uGatewayPort=5060;//default
					if(guLogLevel>3)
						logfileLine("endpoint.c","cTo sip: error fixed 5060 default");
				}
			}
			*cp1='@';
		}
		else
		{
			if((cp1=strchr(cCallID,'@')))
			{
				*cp1=0;
				if(sscanf(cp1+1,"%[0-9\\.]:%u",cGateway,&uGatewayPort)!=2)
				{
					if(guLogLevel>3)
						logfileLine("endpoint.c","cTo cCallID sscanf error");
				}
				*cp1='@';
			}
		}
	}
}//cDID cGateway uGatewayPort

if(guLogLevel>3)
{
	sprintf(gcQuery,"cTo:%s cDID:%s cGateway:%s:%u",cTo,cDID,cGateway,uGatewayPort);
	logfileLine("endpoint.c",gcQuery);
}

if(strstr(cFirstLine," 200 OK") && strstr(cCSeq," OPTIONS") && cGateway[0] && uGatewayPort)
{
	sprintf(gcQuery,"UPDATE tAddress"
			" SET uHealthCheckedDate=UNIX_TIMESTAMP(NOW()),"
			" uAvailable=1,"
			" uUptime=uUptime+1"
			" WHERE cIP='%s' AND uPort=%u",
						cGateway,uGatewayPort);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		logfileLine("endpoint.c",mysql_error(&gMysql));
	if(mysql_affected_rows(&gMysql)>0)
	{
		if(guLogLevel>2)
		{
			sprintf(gcQuery,"%s:%u marked available",cGateway,uGatewayPort);
			logfileLine("endpoint.c",gcQuery);
		}
	}
	else
	{
		if(guLogLevel>2)
		{
			sprintf(gcQuery,"%s:%u UPDATE no effect",cGateway,uGatewayPort);
			logfileLine("endpoint.c",gcQuery);
		}
	}
}
