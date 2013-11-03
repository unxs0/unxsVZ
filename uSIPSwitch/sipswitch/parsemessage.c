/*
FILE 
	unxsVZ/uSIPSwitch/sipswitch/parsemessage.c
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



//Example cMessage
/*
[INVITE sip:7073613110@65.49.53.120:5060 SIP/2.0
Record-Route: <sip:64.2.142.90;lr=on>
Via: SIP/2.0/UDP 64.2.142.90;branch=z9hG4bKf6a8.e2464312.0
Via: SIP/2.0/UDP 66.241.99.224:5060;received=66.241.99.224;branch=z9hG4bK15cac452;rport=5060
From: "3103566265" <sip:3103566265@66.241.99.224>;tag=as63baabce
To: <sip:7073613110@64.2.142.90:5060>
Contact: <sip:3103566265@66.241.99.224>
Call-ID: 3c56c63d0d18332d6ba961731532ad00@66.241.99.224
CSeq: 102 INVITE
User-Agent: packetrino
Max-Forwards: 69
Date: Thu, 27 Sep 2012 15:53:45 GMT
Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, SUBSCRIBE, NOTIFY, INFO
Supported: replaces
Content-Type: application/sdp
Content-Length: 336
*/

int iSendUDPMessageWrapper(char *cMsg,char *cSourceIP,unsigned uSourcePort)
{
	if(!strncmp(cSourceIP,gcServerIP,strlen(gcServerIP)))
	{
		logfileLine("readEv-send to same server error",cSourceIP);
		return(2);
	}

	char *cp;
	if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
	{
		if(guLogLevel>3)
		{
			if((cp=strchr(cMsg,'\r'))) *cp=0;
			sprintf(gcQuery,"%.64s sent to %s:%u",cMsg,cSourceIP,uSourcePort);
			logfileLine("readEv-send",gcQuery);
		}
		return(0);
	}
	else
	{
		if(guLogLevel>1)
		{
			if((cp=strchr(cMsg,'\r'))) *cp=0;
			sprintf(gcQuery,"%.64s failed to %s:%u",cMsg,cSourceIP,uSourcePort);
			logfileLine("readEv-send",gcQuery);
		}
	}
	return(1);

}//int iSendUDPMessageWrapper()


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
	logfileLine("readEv-parse cFirstLine",cFirstLine);
//cFirstLine

char cCSeq[32]={""};
if((cp=strstr(cMessage,"CSeq: ")))
{
	if((cp1=strchr(cp+strlen("CSeq: "),'\r')))
	{
		*cp1=0;
		sprintf(cCSeq,"%.31s",cp+strlen("CSeq: "));
		*cp1='\r';
	}
}
if(guLogLevel>3 && cCSeq[0])
	logfileLine("readEv-parse cCSeq",cCSeq);
if(guLogLevel>1 && !cCSeq[0])
	logfileLine("readEv-parse","No cCSeq");
//cCSeq

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
	logfileLine("readEv-parse cCallID",cCallID);
if(guLogLevel>1 && !cCallID[0])
	logfileLine("readEv-parse","No Call-ID");
//cCallID

//To: <sip:7073613110@64.2.142.90:5060>
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
char cDID[32]={""};
char cGateway[100]={""};
unsigned uGatewayPort=0;
if(cTo[0])
{
	if((cp=strstr(cTo,"sip:")))
	{
		if((cp1=strchr(cTo,'@')))
			*cp1=0;
		sprintf(cDID,"%.31s",cp+4);
		sscanf(cp1+1,"%[0-9\\.]:%u",cGateway,&uGatewayPort);
	}
}//cDID cGateway uGatewayPort

if(guLogLevel>3)
{
	sprintf(gcQuery,"cTo:%s cDID:%s cGateway:%s:%u",cTo,cDID,cGateway,uGatewayPort);
	logfileLine("readEv-parse",gcQuery);
}

//debug only
//return;


//next section is "sipswitch/postparsecheck.c"
