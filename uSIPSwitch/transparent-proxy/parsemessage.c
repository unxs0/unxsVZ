/*
FILE 
	unxsVZ/uSIPSwitch/transparent_proxy/parsemessage.c
	svn ID removed
PURPOSE
	Parse SIP cMessage	
	Quickly modularize code
	Use ln -s transparent_proxy module
	To compile uSIPSwitch with this module
AUTHOR/LEGAL
	(C) 2012-2014 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
DETAILS
	Transparent proxy functionality:
	Packets from PBX need to be forwarded with added Via: to a
	static SIP server (GW) that will deal with them.
	Packets from the static SIP server (GW) will be forwarded 
	to the correct PBX based on the initial Via:. The Via: with 
	this transparent proxy IP number will be removed before fwd.

	Via: processing plan:

	We parse the Via: from the header and save in simple datastructure.
	We remove them from the message.
	At process stage we add them Via section back adding new or removing as
	the case maybe.
	
AVAILABLE DATA FROM readEv()
	char cMessage[2048]={""};
	char cSourceIP[INET_ADDRSTRLEN]={""};
	unsigned uSourcePort=ntohs(sourceAddr.sin_port);
*/

//TODO Check RFC
//Rewrite the FIRST LINE for correct IP:Port
//Only for INVITE, CANCEL and OPTIONS
unsigned uRewriteIPAndPortSIPLine(char *cSIPMessage,const char *cNewIP, unsigned uNewPort)
{

	//cSIPMessage should point to a 2048 char message buffer.

	//line formats we are aware of
	// sip:something@a.b.c.d
	// sip:something@a.b.c.d:n
	// sip:@a.b.c.d
	// sip:@a.b.c.d:n
	// sip:a.b.c.d
	// sip:a.b.c.d:n
	// sip:<fqdn>
	// sip:<fqdn>:n
	// sip:@<fqdn>:n
	// sip:@<fqdn>
	// sip:something@<fqdn>
	// sip:something@<fqdn>:n
	//the only formats we care about are the ones with something@
	//we always rewrite the complete sip: part and if the something exists then we add use it 

	char *cp;
	char *cp1;
	char *cpNextLine;
	char cSomething[100]={""};
	char cPrefix[100]={""};
	char cSIPLine[256]={""};
	char cMessage[2048]={""};

	if((cp=strchr(cSIPMessage,'\r')))
	{
		cpNextLine=cp+1;
		*cp=0;
		sprintf(cSIPLine,"%.255s",cSIPMessage);
		*cp='\r';
	}
	else
		return(1);

	if(!(strstr(cSIPLine,"INVITE ") || strstr(cSIPLine,"CANCEL ") || strstr(cSIPLine,"OPTIONS ")))
		return(2);

	cp=strstr(cSIPLine,"sip:");
	*cp=0;
	sprintf(cPrefix,"%.99s",cSIPLine);

	if((cp1=strchr(cp+strlen("sip:"),'@')))
	{
		*cp1=0;
		sprintf(cSomething,"%.99s",cp+strlen("sip:"));
	}

	if(!uNewPort)
		uNewPort=5060;

	if(cSomething[0])
		sprintf(cSIPLine,"%ssip:%s@%s:%u SIP/2.0\r",cPrefix,cSomething,cNewIP,uNewPort);
	else
		sprintf(cSIPLine,"%ssip:%s:%u SIP/2.0\r",cPrefix,cNewIP,uNewPort);

	sprintf(cMessage,"%.2047s",cpNextLine);
	char cFormat[16]={""};
	sprintf(cFormat,"%%s%%.%us",(unsigned)(2047-strlen(cSIPLine)));
	sprintf(cSIPMessage,cFormat,cSIPLine,cMessage);

	return(0);
}//unsigned uRewriteIPAndPortSIPLine(char *cSIPLine,char *cNewIP, unsigned uNewPort)


//Example cMessages
/*
INVITE sip:7073613110@65.49.53.120:5060 SIP/2.0
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

OPTIONS sip:199.200.101.200 SIP/2.0
Via: SIP/2.0/UDP 67.210.243.247:5060;branch=z9hG4bK0c937819;rport
Max-Forwards: 70
From: "Unknown" <sip:Unknown@67.210.243.247>;tag=as53c0bfee
To: <sip:199.200.101.200>
Contact: <sip:Unknown@67.210.243.247>
Call-ID: 7165dc04209cdfcb21dd6c636462cc28@67.210.243.247
CSeq: 102 OPTIONS
User-Agent: Voice Carrier SIP v1.2.9
Date: Wed, 19 Mar 2014 14:38:35 GMT
Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, SUBSCRIBE, NOTIFY, INFO
Supported: replaces, timer
Content-Length: 0

*/

if(guLogLevel>4)
	logfileLine("readEv-parse","start");

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


char *cp;
char *cp1;


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

//Copy Via: lines
char cVia[10][100]={"","","","","","","","","",""};
unsigned uVia=0;
//These will point inside cMessage
char *cpViaSectionStart=NULL;
char *cpViaSectionEnd=NULL;

//First Via
if((cp=strstr(cMessage,"\nVia: ")))
{
	if((cp1=strchr(cp+strlen("\nVia: "),'\r')))
	{
		*cp1=0;
		sprintf(cVia[uVia],"%.99s",cp+strlen("\nVia: "));
		uVia++;
		*cp1='\r';

		cpViaSectionStart=cp;
		cpViaSectionEnd=cp1;
	}
	else
	{
		cp1=NULL;
	}
}
else
{
	cp1=NULL;
}

//Second and more Vias max 9
while(cp1!=NULL && uVia<9)
{
	if((cp=strstr(cp1+1,"\nVia: ")))
	{
		if((cp1=strchr(cp+strlen("\nVia: "),'\r')))
		{
			*cp1=0;
			sprintf(cVia[uVia],"%.99s",cp+strlen("\nVia: "));
			uVia++;
			*cp1='\r';

			cpViaSectionEnd=cp1;
		}
		else
		{
			cp1=NULL;
		}
	}
	else
	{
		cp1=NULL;
	}
}//while(cp1 && uVia<9)

char cPBXIP[32]={""};
unsigned uPBXPort=0;
if(uVia && cVia[uVia-1][0])
{
	//Via: SIP/2.0/UDP 64.2.142.90;branch=z9hG4bKf6a8.e2464312.0
	if((cp=strstr(cVia[uVia-1],"UDP ")))
	{
		sscanf(cp+strlen("UDP "),"%[0-9\\.]:%u",cPBXIP,&uPBXPort);
	}
}//cPBXIP,uPBXPort

register int i;
if(guLogLevel>3)
{
	for(i=0;cVia[i] && i<uVia;i++)
	{
		sprintf(gcQuery,"cVia[%d]:%s",i,cVia[i]);
		logfileLine("readEv-parse",gcQuery);
	}
	sprintf(gcQuery,"uVia=%u cPBXIP:%s uPBXPort:%u",uVia,cPBXIP,uPBXPort);
	logfileLine("readEv-parse",gcQuery);
}
//cVia

char cCallIDPBXIP[32]={""};
unsigned uCallIDPBXPort=0;
if(cCallID[0])
{
	if((cp=strchr(cCallID,'@')))
	{
		*cp=0;
		sscanf(cp+1,"%[0-9\\.]:%u",cCallIDPBXIP,&uCallIDPBXPort);
	}
}//cCallIDPBXIP,uCallIDPBXPort

if(guLogLevel>3)
{
	sprintf(gcQuery,"cCallID:%s cCallIDPBXIP:%s:%u",cCallID,cCallIDPBXIP,uCallIDPBXPort);
	logfileLine("readEv-parse",gcQuery);
}



if(guLogLevel>4)
	logfileLine("readEv-parse","end");
//next section is "sipswitch/postparsecheck.c"
