/*
FILE 
	unxsVZ/uSIPSwitch/natpbx/postparsecheck.c
	svn ID removed
PURPOSE
	After we parse SIP cMessage we can
	check to make sure we have enough data to continue
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

//Previous section is parsemessage.c

//
//If anything is amiss send back an error message and return.
//
char cMsg[100]={""};
if(!cCallID[0])
{
	sprintf(cMsg,"481 Call/Transaction Does Not Exist\n");
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
		logfileLine("readEv 481 Call/Transaction Does Not Exist",cSourceIP);	
}

if(!cToDomain[0])
{
	//Empty cToDomain
	sprintf(cMsg,"SIP/2.0 416 Unsupported URI\n");
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
		logfileLine("readEv cToDomain 416 empty",cSourceIP);
	return;
}

//next section is process.c
