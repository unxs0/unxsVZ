/*
FILE 
	unxsVZ/uSIPSwitch/natpbx/process.c
	svn ID removed
PURPOSE
	This is where the SIP state machine is implemented
AUTHOR/LEGAL
	(C) 2012-2013 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
DETAILS
AVAILABLE DATA FROM readEv()
	char cMessage[2048]={""};
	char cSourceIP[INET_ADDRSTRLEN]={""};
	char cDestinationIP[256]={""};
	unsigned uSourcePort=ntohs(sourceAddr.sin_port);
	unsigned uDestinationPort=0;
	char cFirstLine[100]={""};
	char cCallID[100]={""};
	char cToDomain[100]={""};
	unsigned uToPort=0;
	char cFrom[100]={""};
	char cFromDomain[100]={""};
	unsigned uFromPort=0;

*/

//Previous section is postparsecheck.c

//Determine if message is a request or a reply
unsigned uReply=0;
if(cMessage[0]=='S') uReply=1;

char cData[256]={""};
char cKey[128]={""};
size_t sizeData=255;
uint32_t flags=0;
memcached_return rc;

//
//Process requests
//	(e.g. commands like INVITE, ACK, BYE, CANCEL, OPTIONS, REGISTER and INFO)
//
if(!uReply)
{
	if(cToDomain[0])
	{
		sprintf(cKey,"%s-pbx",cToDomain);
		sprintf(cData,"%.255s",memcached_get(gsMemc,cKey,strlen(cKey),&sizeData,&flags,&rc));
		if(rc!=MEMCACHED_SUCCESS)
		{
			//Not found. Try the from domain
			sprintf(cKey,"%s-pbx",cFromDomain);
			sprintf(cData,"%.255s",memcached_get(gsMemc,cKey,strlen(cKey),&sizeData,&flags,&rc));
			if(rc!=MEMCACHED_SUCCESS)
			{
				//Not found
				sprintf(cMsg,"SIP/2.0 404 User not found\n");
				if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
				{
					if(guLogLevel>3)
					{
						sprintf(gcQuery,"reply sent to %s:%u",cSourceIP,uSourcePort);
						logfileLine("readEv",gcQuery);
					}
				}
				else
				{
					if(guLogLevel>1)
					{
						sprintf(gcQuery,"reply failed to %s:%u",cSourceIP,uSourcePort);
						logfileLine("readEv",gcQuery);
					}
				}
				if(guLogLevel>3)
					logfileLine("readEv cToDomain/cFromDomain 404 not found",cKey);
				return;
			}
			else if(rc==MEMCACHED_SUCCESS)
			{
				//The destination is the cToDomain and the uToPort
				//If cToDomain is not an IP this will not work.
				sprintf(cDestinationIP,"%.15s",cToDomain);
				uDestinationPort=uToPort;
			}
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
				logfileLine("readEv",gcQuery);
			}
			if(!cDestinationIP[0] || !uDestinationPort)
				logfileLine("readEv pbx parse error",cData);
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
				sprintf(gcQuery,"reply sent to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv",gcQuery);
			}
		}
		else
		{
			if(guLogLevel>1)
			{
				sprintf(gcQuery,"reply failed to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv",gcQuery);
			}
		}
		if(guLogLevel>3)
			logfileLine("readEv 500 Could not create transaction",cKey);
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
				logfileLine("readEv",gcQuery);
			}
		}
		else
		{
			if(guLogLevel>1)
			{
				sprintf(gcQuery,"reply failed to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv",gcQuery);
			}
		}
		if(guLogLevel>3)
			logfileLine("readEv 481 Transaction Does Not Exist",cCallID);
	}
	if(guLogLevel>3 && cData[0])
		logfileLine("readEv reply cData",cData);
	//parse cData for forwarding information
	sscanf(cData,"cSourceIP=%[^;];uSourcePort=%u;",cDestinationIP,&uDestinationPort);
	if(guLogLevel>3)
	{
		sprintf(gcQuery,"cSourceIP:%s uSourcePort:%u",cDestinationIP,uDestinationPort);
		logfileLine("readEv",gcQuery);
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
			logfileLine("readEv",gcQuery);
		}
	}
	else
	{
		if(guLogLevel>1)
		{
			sprintf(gcQuery,"forward failed to %s:%u",cDestinationIP,uDestinationPort);
			logfileLine("readEv",gcQuery);
		}
		sprintf(cMsg,"SIP/2.0 500 Forward failed\n");
		if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
		{
			if(guLogLevel>3)
			{
				sprintf(gcQuery,"reply sent to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv",gcQuery);
			}
		}
		else
		{
			if(guLogLevel>1)
			{
				sprintf(gcQuery,"reply failed to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv",gcQuery);
			}
		}
		if(guLogLevel>3)
			logfileLine("readEv 500 Forward failed",cDestinationIP);
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
			sprintf(gcQuery,"reply sent to %s:%u",cSourceIP,uSourcePort);
			logfileLine("readEv",gcQuery);
		}
	}
	else
	{
		if(guLogLevel>1)
		{
			sprintf(gcQuery,"reply failed to %s:%u",cSourceIP,uSourcePort);
			logfileLine("readEv",gcQuery);
		}
	}
	if(guLogLevel>3)
		logfileLine("readEv 500 Forward failed",cDestinationIP);
	return;
	if(guLogLevel>3)
		logfileLine("readEv unexpected no cDestinationIP/uDestinationPort",cCallID);
}

//no more readEv() sections
