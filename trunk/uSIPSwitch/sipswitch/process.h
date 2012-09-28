/*
FILE 
	unxsVZ/uSIPSwitch/sipswitch/process.h
	$Id$
PURPOSE
	This is where the SIP state machine is implemented
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
	char cGateway[100]={""};
	unsigned uGatewayPort=0;

*/

//Previous section is postparsecheck.h

//Determine if message is a request or a reply
unsigned uReply=0;
if(cMessage[0]=='S') uReply=1;

char cData[256]={""};
char cKey[128]={""};
size_t sizeData=255;
uint32_t flags=0;
memcached_return rc;

//Check to see if request is coming from valid source
//source can be carrier gateway or one of our SBC'd PBXs
//PBXs and gateways are BOTH found in memcached as -gw keys
if(cGateway[0])
{
	sprintf(cKey,"%s-gw",cGateway);
	memcached_get(gsMemc,cKey,strlen(cKey),&sizeData,&flags,&rc);
	if(rc!=MEMCACHED_SUCCESS)
	{
		//Not found
		sprintf(cMsg,"SIP/2.0 403 Unregistered gateway\n");
		if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
		{
			if(guLogLevel>3)
			{
				sprintf(gcQuery,"reply 403 Unregistered gateway sent to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv-process",gcQuery);
			}
		}
		else
		{
			if(guLogLevel>1)
			{
				sprintf(gcQuery,"reply 403 Unregistered gateway failed to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv-process",gcQuery);
			}
		}
		if(guLogLevel>3)
			logfileLine("readEv-process 403 Forbidden IP not registered",cKey);
		return;
	}
	else if(rc==MEMCACHED_SUCCESS)
	{
		//Found let other side know we are working on their request
		sprintf(cMsg,"SIP/2.0 100 Trying\n");
		if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
		{
			if(guLogLevel>3)
			{
				sprintf(gcQuery,"reply 100 Trying sent to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv-process",gcQuery);
			}
		}
		else
		{
			if(guLogLevel>1)
			{
				sprintf(gcQuery,"reply 100 Trying failed to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv-process",gcQuery);
			}
			return;
		}
	}
}//Check for valid gateway

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

		if(cDID[0])
		{
			sprintf(cKey,"%s-did",cDID);
			sprintf(cData,"%.255s",memcached_get(gsMemc,cKey,strlen(cKey),&sizeData,&flags,&rc));
			if(rc!=MEMCACHED_SUCCESS)
			{
				//Not found
				sprintf(cMsg,"SIP/2.0 404 User not found\n");
				if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
				{
					if(guLogLevel>3)
					{
						sprintf(gcQuery,"reply 404 sent to %s:%u",cSourceIP,uSourcePort);
						logfileLine("readEv-process",gcQuery);
					}
				}
				else
				{
					if(guLogLevel>1)
					{
						sprintf(gcQuery,"reply 404 failed to %s:%u",cSourceIP,uSourcePort);
						logfileLine("readEv-process",gcQuery);
					}
				}
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
		}
		sprintf(cData,"cSourceIP=%.15s;uSourcePort=%u;",cSourceIP,uSourcePort);
		rc=memcached_set(gsMemc,cCallID,strlen(cCallID),cData,strlen(cData),(time_t)0,(uint32_t)0);
		if(rc!=MEMCACHED_SUCCESS)
		{
			sprintf(cMsg,"SIP/2.0 500 Could not create transaction\n");
			if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
			{
				if(guLogLevel>3)
				{
					sprintf(gcQuery,"reply 500 sent to %s:%u",cSourceIP,uSourcePort);
					logfileLine("readEv-process",gcQuery);
				}
			}
			else
			{
				if(guLogLevel>1)
				{
					sprintf(gcQuery,"reply 500 failed to %s:%u",cSourceIP,uSourcePort);
					logfileLine("readEv-process",gcQuery);
				}
			}
			if(guLogLevel>3)
				logfileLine("readEv-process 500 Could not create transaction",cKey);
			return;
		}
	}//INVITE
	else
	{
		sprintf(cMsg,"SIP/2.0 405 Method Not Allowed\n");
		if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
		{
			if(guLogLevel>3)
			{
				sprintf(gcQuery,"reply 405 Method Not Allowed %.6s sent to %s:%u",cFirstLine,cSourceIP,uSourcePort);
				logfileLine("readEv-process",gcQuery);
			}
		}
		else
		{
			if(guLogLevel>1)
			{
				sprintf(gcQuery,"reply 405 Method Not Allowed %.6s failed to %s:%u",cFirstLine,cSourceIP,uSourcePort);
				logfileLine("readEv-process",gcQuery);
			}
		}
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
		sprintf(cMsg,"SIP/2.0 481 Transaction Does Not Exist\n");
		if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
		{
			if(guLogLevel>3)
			{
				sprintf(gcQuery,"reply sent to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv-process",gcQuery);
			}
		}
		else
		{
			if(guLogLevel>1)
			{
				sprintf(gcQuery,"reply failed to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv-process",gcQuery);
			}
		}
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
	if(!iSendUDPMessage(cMessage,cDestinationIP,uDestinationPort))
	{
		if(guLogLevel>3)
		{
			sprintf(gcQuery,"message forwarded to %s:%u",cDestinationIP,uDestinationPort);
			logfileLine("readEv-process",gcQuery);
		}
	}
	else
	{
		if(guLogLevel>1)
		{
			sprintf(gcQuery,"forward failed to %s:%u",cDestinationIP,uDestinationPort);
			logfileLine("readEv-process",gcQuery);
		}
		sprintf(cMsg,"SIP/2.0 500 Forward failed\n");
		if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
		{
			if(guLogLevel>3)
			{
				sprintf(gcQuery,"reply 500 sent to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv-process",gcQuery);
			}
		}
		else
		{
			if(guLogLevel>1)
			{
				sprintf(gcQuery,"reply 500 failed to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv-process",gcQuery);
			}
		}
		if(guLogLevel>3)
			logfileLine("readEv-process 500 Forward failed",cDestinationIP);
		return;
	}
}//if(cDestinationIP[0] && uDestinationPort)
else
{
	sprintf(cMsg,"SIP/2.0 500 Forward failed\n");
	if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
	{
		if(guLogLevel>3)
		{
			sprintf(gcQuery,"reply 500 sent to %s:%u",cSourceIP,uSourcePort);
			logfileLine("readEv-process",gcQuery);
		}
	}
	else
	{
		if(guLogLevel>1)
		{
			sprintf(gcQuery,"reply 500 failed to %s:%u",cSourceIP,uSourcePort);
			logfileLine("readEv-process",gcQuery);
		}
	}
	if(guLogLevel>3)
		logfileLine("readEv-process unexpected no cDestinationIP/uDestinationPort",cCallID);
}

//no more readEv() sections
