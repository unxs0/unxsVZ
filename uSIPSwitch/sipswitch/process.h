/*
FILE 
	unxsVZ/uSIPSwitch/sipswitch/process.h
	$Id$
PURPOSE
	This is where the SIP state machine, routing logic 
	and SBC items (channel limits and CDR data gathering) 
	are implemented.
AUTHOR/LEGAL
	(C) 2012 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
DETAILS
AVAILABLE DATA FROM readEv()
	char cMessage[2048]={""};
	char cSourceIP[INET_ADDRSTRLEN]={""};
	unsigned uSourcePort=ntohs(sourceAddr.sin_port);
	char cFirstLine[100]={""};
	char cCallID[100]={""};
	char cDID[32]={""};
	char cCSeq[32]={""};
	char cGateway[100]={""};
	unsigned uGatewayPort=0;

THINGS TO DO LIST
	So many impossible to list all at this time	
	Big ticket items:
	1. Keep track of number of calls per PBX and limit to
	unxsSPS.tPBX.uLines
	2. Keep track of calls so as to have enough data
	for CDRs.
	Current next steps:
	1. Add Via: header lines as required.
	2. Add Route: header lines as required.
	3. Add more header lines to proxy generated responses as required by
	Asterisk and our test Carrier UASs. Not by the huge RFC list.

*/

//Previous section is postparsecheck.h

#define GATEWAY 1
#define PBX 2

//Determine if message is a request or a reply
unsigned uReply=0;
if(cMessage[0]=='S') uReply=1;

char cData[256]={""};
char cKey[128]={""};
unsigned uType=0;//1 is gateway, 2 is pbx, 0 is unknown
size_t sizeData=255;
uint32_t flags=0;
memcached_return rc;


//Check to see if request is coming from valid source
//source can be carrier gateway or one of our SBC'd PBXs
//PBXs and gateways are BOTH found in memcached as -gw keys
sprintf(cKey,"%s-gw",cSourceIP);
sprintf(cData,"%.255s",memcached_get(gsMemc,cKey,strlen(cKey),&sizeData,&flags,&rc));
if(rc!=MEMCACHED_SUCCESS)
{
	//Not found
	sprintf(cMsg,"SIP/2.0 403 Unregistered gateway\r\nCSeq: %s\r\n",cCSeq);
	iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
	if(guLogLevel>3)
		logfileLine("readEv-process 403 Forbidden IP not registered",cKey);
	return;
}
else if(rc==MEMCACHED_SUCCESS)
{
	//Found let other side know we are working on their request
	sprintf(cMsg,"SIP/2.0 100 Trying\r\nCSeq: %s\r\n",cCSeq);//Send back same CSeq check this
	iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);

	sscanf(cData,"cDestinationIP=%[^;];uDestinationPort=%u;uType=%u;",cDestinationIP,&uDestinationPort,&uType);
	if(cDestinationIP[0]==0 || uDestinationPort==0 || uType==0 )
	{
		logfileLine("readEv-process","cData incorrect");
		logfileLine("readEv-process",cData);
		return;
	}
}
//Approve gateway and get type

//debug only
//return;

//
//Process requests
//	(e.g. commands like INVITE, ACK, BYE, CANCEL, OPTIONS, REGISTER and INFO)
//
if(!uReply)
{
	if(!strncmp(cFirstLine,"INVITE",6))
	{
		if(uType==GATEWAY)
		{
			sprintf(cKey,"%s-did",cDID);
			sprintf(cData,"%.255s",memcached_get(gsMemc,cKey,strlen(cKey),&sizeData,&flags,&rc));
			if(rc!=MEMCACHED_SUCCESS)
			{
				//Not found
				sprintf(cMsg,"SIP/2.0 404 User not found\r\nCSeq: %s\r\n",cCSeq);
				iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
				if(guLogLevel>3)
					logfileLine("readEv-process cToDomain/cFromDomain 404 not found",cKey);
				return;
			}
			else if(rc==MEMCACHED_SUCCESS)
			{
				//parse cData for forwarding information
				if((cp=strstr(cData,"cDestinationIP=")))
				{
					if((cp1=strchr(cp+15,';')))
					{
						*cp1=0;
						sprintf(cDestinationIP,"%.15s",cp+15);
						*cp1=';';
					}
				}
				if((cp=strstr(cData,"uDestinationPort=")))
					sscanf(cp+17,"%u",&uDestinationPort);
				if(guLogLevel>3)
				{
					sprintf(gcQuery,"cDestinationIP:%s uDestinationPort:%u",cDestinationIP,uDestinationPort);
					logfileLine("readEv-process",gcQuery);
				}
				if(!cDestinationIP[0] || !uDestinationPort)
					logfileLine("readEv-process pbx parse error",cData);
			}
		}//if GATEWAY
		else if(uType==PBX)
		{
			//This is where all the rule code will go

			sprintf(cKey,"outbound");
			sprintf(cData,"%.255s",memcached_get(gsMemc,cKey,strlen(cKey),&sizeData,&flags,&rc));
			if(rc!=MEMCACHED_SUCCESS)
			{
				sprintf(cMsg,"SIP/2.0 500 No outbound gateway\r\nCSeq: %s\r\n",cCSeq);
				if(iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort))
				{
					if(guLogLevel>3)
						logfileLine("readEv-process 500 No outbound gateway",cKey);
					return;
				}
			}
			else
			{
				//Parse outbound gateway0
				sscanf(cData,"cDestinationIP0=%[^;];uDestinationPort0=%u;",cDestinationIP,&uDestinationPort);
				if(cDestinationIP[0]==0 || uDestinationPort==0)
				{
					if(guLogLevel>0)
					{
						logfileLine("readEv-process","cData incorrect");
						logfileLine("readEv-process",cData);
					}
				}
				else
				{
					if(guLogLevel>3)
					{
						logfileLine("readEv-process","cData ok");
						logfileLine("readEv-process",cData);
					}
				}
			}
		}
		else if(1)
		{
			logfileLine("readEv-process unexpected uType",cData);
			return;
		}

		//Create or update transaction
		sprintf(cData,"cSourceIP=%.15s;uSourcePort=%u;",cSourceIP,uSourcePort);
		rc=memcached_set(gsMemc,cCallID,strlen(cCallID),cData,strlen(cData),(time_t)0,(uint32_t)0);
		if(rc!=MEMCACHED_SUCCESS)
		{
			sprintf(cMsg,"SIP/2.0 500 Could not create transaction\r\nCSeq: %s\r\n",cCSeq);
			if(iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort))
			{
				if(guLogLevel>3)
					logfileLine("readEv-process 500 Could not create transaction",cKey);
				return;
			}
		}

	}//INVITE
	else
	{
		sprintf(cMsg,"SIP/2.0 405 Method Not Allowed\r\nCSeq: %s\r\n",cCSeq);
		iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
		return;
	}
	
	
}//if a request (e.g. commands like INVITE, ACK, BYE, CANCEL, OPTIONS, REGISTER and INFO)
//
//Process reply
//	(e.g. SIP/2.0 200 Ok)
//
else
{
	sprintf(cData,"%.255s",memcached_get(gsMemc,cCallID,strlen(cCallID),&sizeData,&flags,&rc));
	if(rc!=MEMCACHED_SUCCESS)
	{
		sprintf(cMsg,"SIP/2.0 481 Transaction Does Not Exist\r\nCSeq: %s\r\n",cCSeq);
		iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
		if(guLogLevel>3)
			logfileLine("readEv-process 481 Transaction Does Not Exist",cCallID);
	}
	if(guLogLevel>3 && cData[0])
		logfileLine("readEv-process reply cData",cData);
	//parse cData for forwarding information
	sscanf(cData,"cSourceIP=%[^;];uSourcePort=%u;",cDestinationIP,&uDestinationPort);
	if(guLogLevel>3)
	{
		sprintf(gcQuery,"cSourceIP:%s uSourcePort:%u",cDestinationIP,uDestinationPort);
		logfileLine("readEv-process",gcQuery);
	}
}//if a reply



//
//Forward unmodified packet
if(cDestinationIP[0] && uDestinationPort)
{
	if(iSendUDPMessageWrapper(cMessage,cDestinationIP,uDestinationPort))
	{
		sprintf(cMsg,"SIP/2.0 500 Forward failed\r\nCSeq: %s\r\n",cCSeq);
		iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
		return;
	}
}//if(cDestinationIP[0] && uDestinationPort)
else
{
	sprintf(cMsg,"SIP/2.0 500 Forward failed\r\nCSeq: %s\r\n",cCSeq);
	iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
	if(guLogLevel>3)
		logfileLine("readEv-process unexpected no cDestinationIP/uDestinationPort",cCallID);
}

//no more readEv() sections
