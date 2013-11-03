/*
FILE 
	unxsVZ/uSIPSwitch/natpbx/parsemessage.c
	$Id$
PURPOSE
	Parse SIP cMessage	
	Quickly modularize code
	Use ln -s natpbx module
	To compile uSIPSwitch with this module
AUTHOR/LEGAL
	(C) 2012-2013 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
DETAILS
	NATPBX module requires that these values be parsed from header 
	char cFirstLine[100]={""};
	char cCallID[100]={""};
	char cToDomain[100]={""};
	unsigned uToPort=0;
	char cFrom[100]={""};
	char cFromDomain[100]={""};
	unsigned uFromPort=0;

AVAILABLE DATA FROM readEv()
	char cMessage[2048]={""};
	char cSourceIP[INET_ADDRSTRLEN]={""};
	char cDestinationIP[256]={""};
	unsigned uSourcePort=ntohs(sourceAddr.sin_port);
	unsigned uDestinationPort=0;
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
	logfileLine("readEv cFirstLine",cFirstLine);
//cFirstLine

char cCallID[100]={""};
if((cp=strstr(cMessage,"Call-ID: ")))
{
	if((cp1=strchr(cp+strlen("Call-ID: "),'\r')))
	{
		*cp1=0;
		sprintf(cCallID,"%.99s",cp+strlen("Call-ID: "));
		*cp1='\r';
	}
}
if(guLogLevel>3 && cCallID[0])
	logfileLine("readEv cCallID",cCallID);
if(guLogLevel>1 && !cCallID[0])
	logfileLine("readEv","No Call-ID");
//cCallID

char cTo[100]={""};
if((cp=strstr(cMessage,"To: ")))
{
	if((cp1=strchr(cp+strlen("To: "),'\r')))
	{
		*cp1=0;
		sprintf(cTo,"%.99s",cp+strlen("To: "));
		*cp1='\r';
	}
}//cTo

char cToDomain[100]={""};
unsigned uToPort=0;
if(cTo[0])
{
	if((cp=strchr(cTo,'@')))
	{
		sprintf(cToDomain,"%.99s",cp+1);
		if((cp=strchr(cToDomain,':')))
		{
			//get port then chop off
			sscanf(cp+1,"%u",&uToPort);
			*cp=0;
		}
		if((cp=strchr(cToDomain,'>')))
			*cp=0;
		if((cp=strchr(cToDomain,';')))
			*cp=0;
	}
}//cToDomain

char cFrom[100]={""};
if((cp=strstr(cMessage,"From: ")))
{
	if((cp1=strchr(cp+strlen("From: "),'\r')))
	{
		*cp1=0;
		sprintf(cFrom,"%.99s",cp+strlen("From: "));
		*cp1='\r';
	}
}//cFrom

char cFromDomain[100]={""};
unsigned uFromPort=0;
if(cFrom[0])
{
	if((cp=strchr(cFrom,'@')))
	{
		sprintf(cFromDomain,"%.99s",cp+1);
		if((cp=strchr(cFromDomain,':')))
		{
			//get port then chop off
			sscanf(cp+1,"%u",&uFromPort);
			*cp=0;
		}
		if((cp=strchr(cFromDomain,'>')))
			*cp=0;
		if((cp=strchr(cFromDomain,';')))
			*cp=0;
	}
}//cFromDomain

if(guLogLevel>3)
{
	sprintf(gcQuery,"cTo:%s cToDomain:%s:%u",cTo,cToDomain,uToPort);
	logfileLine("readEv",gcQuery);
	sprintf(gcQuery,"cFrom:%s cFromDomain:%s:%u",cFrom,cFromDomain,uFromPort);
	logfileLine("readEv",gcQuery);
}


//next section is "natpbx/postparsecheck.c"


//Example cMessage
/*
[OPTIONS sip:nobody@127.0.0.1 SIP/2.0
Via: SIP/2.0/UDP 72.52.75.232:44736;branch=z9hG4bK.3bcb1ce3;rport;alias
From: sip:sipsak@72.52.75.232:44736;tag=808799b
To: sip:nobody@127.0.0.1
Call-ID: 134773147@72.52.75.232
CSeq: 1 OPTIONS
Contact: sip:sipsak@72.52.75.232:44736
Content-Length: 0
Max-Forwards: 70
User-Agent: sipsak 0.9.6
Accept: text/plain

]

Note most common sip uri scheme:

From: "103" <sip:103@vcinternaltestingonly.callingcloud.net:6002>;tag=1764399065
To: "103" <sip:103@vcinternaltestingonly.callingcloud.net:6002>;tag=as2e679ce3

Notes:
	Lines are cr-lf terminated
	Last text/plain line is double cr-lf terminated.
*/
