/*
FILE 
	unxsVZ/uSIPSwitch/transparent-proxy/process.c
	$Id$
PURPOSE
	All we need to do here is forward the message:
	If it comes from a PBX we send to GW.
	If from GW we send to PBX.
	We determine this by checking cSourceIP against
	the GW IP we send all PBX traffic to.
AUTHOR/LEGAL
	(C) 2012-2014 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
DETAILS
AVAILABLE DATA FROM readEv()
	char cMessage[2048]={""};
	char cSourceIP[INET_ADDRSTRLEN]={""};
	unsigned uSourcePort=ntohs(sourceAddr.sin_port);
	char cCallID[100]={""};
	char cPBXIP[32]={""};
	unsigned uPBXPort=0;

THINGS TO DO LIST

*/

//Previous section is postparsecheck.c

if(guLogLevel>4)
	logfileLine("readEv-process tproxy",cSourceIP);

char cRealGWIP[INET_ADDRSTRLEN]={"174.121.136.137"};
char *cpMsg=cMessage;

//If from GW send to PBX
if(!strcmp(cSourceIP,cRealGWIP))
	sprintf(cDestinationIP,"%.15s",cPBXIP);
else
	sprintf(cDestinationIP,"%.15s",cRealGWIP);


//
//Forward unmodified packet
if(cDestinationIP[0])
{
	if(!uDestinationPort) uDestinationPort=DEFAULT_SIP_PORT;

	int iRetVal=0;
	if((iRetVal=iSendUDPMessageWrapper(cpMsg,cDestinationIP,uDestinationPort)))
	{
		//Can be 2 for sending to same server error
		if(iRetVal==1)
		{
			sprintf(cMsg,"SIP/2.0 500 Forward failed-1\r\nCSeq: %s\r\n",cCSeq);
			iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
		}
		return;
	}
}

//no more readEv() sections

