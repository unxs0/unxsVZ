/*
FILE 
	unxsVZ/uSIPSwitch/transparent-proxy/process.c
	svn ID removed
PURPOSE
	process message before forwarding
	determine where to send
AUTHOR/LEGAL
	(C) 2012-2014 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
DETAILS
AVAILABLE DATA FROM readEv()
	char cMessage[2048]={""};
	char cSourceIP[INET_ADDRSTRLEN]={""};
	unsigned uSourcePort=ntohs(sourceAddr.sin_port);

THINGS TO DO LIST

*/

//Previous section is postparsecheck.c

if(guLogLevel>4)
	logfileLine("readEv-process","start");

unsigned uSend=0;
char *cpMsg=cMessage;
char gcGWIP[32]={"174.121.136.137"};

//Remove Via: section and rebuild
if(cpViaSectionStart!=NULL && cpViaSectionEnd!=NULL)
{
	char cAddVia[100]={""};

	for(i=0;i<2047 && cMessage[i] && cpViaSectionStart!=cMessage+i;i++)
	{
		cMessageModified[i]=cMessage[i];
	}
	cMessageModified[i]=0;

	//if going to GW add gcServerIP via
	//Via: SIP/2.0/UDP 199.200.101.200:6060;branch=z9hG4bK.6a15f813;rport;alias
	if(strcmp(cSourceIP,gcGWIP))
	{
		sprintf(cAddVia,"\nVia: SIP/2.0/UDP %s:%u;branch=z9hG4bK.6a15f813;rport;alias\r",gcServerIP,guServerPort);
		strncat(cMessageModified,cAddVia,(2047-strlen(cMessageModified)-strlen(cAddVia)));
		sprintf(cDestinationIP,"%.15s",gcGWIP);
	}
	else
	{
		sprintf(cDestinationIP,"%.15s",cPBXIP);
		uDestinationPort=uPBXPort;
	}

	//remove Via's that match this proxy
	//leave the others
	for(i=0;i<uVia;i++)
	{
		if(!strstr(cVia[i],gcServerIP))
		{
			sprintf(cAddVia,"\nVia: %s\r",cVia[i]);
			strncat(cMessageModified,cAddVia,(2047-strlen(cMessageModified)-strlen(cAddVia)));
		}
	}

	//danger how can we do this correctly and easily?
	strncat(cMessageModified,cpViaSectionEnd+1,(2047-strlen(cMessageModified)-strlen(cpViaSectionEnd)));

	cpMsg=cMessageModified;
	uSend=1;
}

//
//Forward unmodified packet
if(uSend && cDestinationIP[0])
{
	uSend=0;//reset
	if(!uDestinationPort) uDestinationPort=DEFAULT_SIP_PORT;

	uRewriteIPAndPortSIPLine(cpMsg,cDestinationIP,uDestinationPort);

	if(guLogLevel>4 && gLfp!=NULL)
		fprintf(gLfp,"[%s]\n",cpMsg);

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

if(guLogLevel>4)
	logfileLine("readEv-process","end");
