/*
FILE 
	unxsVZ/uSIPSwitch/sipswitch/postparsecheck.h
	$Id$
PURPOSE
	After we parse SIP cMessage we can
	check to make sure we have enough data to continue
AUTHOR/LEGAL
	(C) 2012 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
DETAILS
AVAILABLE DATA FROM readEv() and parsemessage.h
	char cMessage[2048]={""};
	char cSourceIP[INET_ADDRSTRLEN]={""};
	unsigned uSourcePort=ntohs(sourceAddr.sin_port);
	char cFirstLine[100]={""};
	char cCallID[100]={""};
	char cGateway[100]={""};
	unsigned uGatewayPort=0;
	char cDID[32]={""};

*/

//Previous section is parsemessage.h

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

if(!cGateway[0] || !cDID[0])
{
	//Empty cGateway
	sprintf(cMsg,"SIP/2.0 416 Unsupported URI\n");
	if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
	{
		if(guLogLevel>3)
		{
			sprintf(gcQuery,"reply 416 sent to %s:%u",cSourceIP,uSourcePort);
			logfileLine("readEv",gcQuery);
		}
	}
	else
	{
		if(guLogLevel>1)
		{
			sprintf(gcQuery,"reply 416 failed to %s:%u",cSourceIP,uSourcePort);
			logfileLine("readEv",gcQuery);
		}
	}
	if(guLogLevel>3)
		logfileLine("readEv cGateway or cDID 416 empty",cSourceIP);
	return;
}

//next section is process.h
