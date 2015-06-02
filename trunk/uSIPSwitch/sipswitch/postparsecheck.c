/*
FILE 
	unxsVZ/uSIPSwitch/sipswitch/postparsecheck.c
	$Id$
PURPOSE
	After we parse SIP cMessage we can
	check to make sure we have enough data to continue
AUTHOR/LEGAL
	(C) 2012-2013 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
DETAILS
AVAILABLE DATA FROM readEv() and parsemessage.c
	char cMessage[2048]={""};
	char cSourceIP[INET_ADDRSTRLEN]={""};
	unsigned uSourcePort=ntohs(sourceAddr.sin_port);
	char cFirstLine[100]={""};
	char cCallID[100]={""};
	char cGateway[100]={""};
	unsigned uGatewayPort=0;
	char cDID[32]={""};
	char cCSeq[32]={""};

*/

//Previous section is parsemessage.c
if(guLogLevel>3)
	logfileLine("readEv-postparse start",cSourceIP);

//
//If anything is amiss send back an error message and return.
//
char cMsg[256]={""};

if(!cCallID[0])
{
	if(guLogLevel>2)
		logfileLine("readEv-postparse 400 Missing CallID",cSourceIP);	
	sprintf(cMsg,"400 Missing CallID\r\nCSeq: %s\r\n",cCSeq);
	iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
}

if(!cGateway[0])
{
	if(guLogLevel>2)
		logfileLine("readEv-postparse 416 cGateway empty",cSourceIP);
	//Empty cGateway
	sprintf(cMsg,"SIP/2.0 416 Unsupported URI\r\nCSeq: %s\r\n",cCSeq);
	iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
	return;
}

if(!cCSeq[0])
{
	//Empty cGateway
	if(guLogLevel>2)
		logfileLine("readEv-postparse 400 Missing CSeq",cSourceIP);
	sprintf(cMsg,"SIP/2.0 400 Missing CSeq\r\nCSeq: 0\r\n");
	iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
	return;
}

if(!cDID[0] && !strstr(cCSeq,"OPTIONS"))
{
	if(guLogLevel>2)
		logfileLine("readEv-postparse 416 cDID empty",cSourceIP);
	//Empty cDID when not an OPTIONS
	sprintf(cMsg,"SIP/2.0 416 Unsupported URI\r\nCSeq: %s\r\n",cCSeq);
	iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
	return;
}


//next section is sipswitch/process.c
if(guLogLevel>3)
	logfileLine("readEv-postparse end",cCallID);
