/*
FILE 
	unxsVZ/uSIPSwitch/sipswitch/process.c
	$Id$
PURPOSE
	This is where the SIP state machine, routing logic 
	and SBC items (channel limits and CDR data gathering) 
	are implemented.
AUTHOR/LEGAL
	(C) 2012-2013 Gary Wallis for Unixservice, LLC.
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
	ASAP code new outgoing gateway determination.

	So many that it is impossible to list all at this time	
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
	4. Modify the Request-URI line for target.

*/

//Previous section is postparsecheck.c

//Determine if message is a request or a reply
//Parse SIP reply code, e.g. 500 for:
//	SIP/2.0 500 Server internal failure
unsigned uReply=0;
if(cMessage[0]=='S')
{
	uReply=1;//mark as reply anyway in case sscanf fails
	sscanf(cFirstLine,"SIP%*[A-Z0-9/\\.] %u",&uReply);
}

char cData[256]={""};
char cKey[128]={""};
char *cpMsg=cMessage;
#define GATEWAY 1
#define PBX 2
unsigned uType=0;//1 is gateway, 2 is pbx, 0 is unknown
//PBXs have the max number of concurrent incoming and outgoing calls set
unsigned uLines=0;
#define INVITE 1
unsigned uRequestType=0;//1 is INVITE
size_t sizeData=255;
uint32_t flags=0;
memcached_return rc;

//nested functions
int iModifyMessage(char *cMessage)
{
/*
16.12 Summary of Proxy Route Processing


   In the absence of local policy to the contrary, the processing a
   proxy performs on a request containing a Route header field can be
   summarized in the following steps.

      1.  The proxy will inspect the Request-URI.  If it indicates a
          resource owned by this proxy, the proxy will replace it with
          the results of running a location service.  Otherwise, the
          proxy will not change the Request-URI.

      2.  The proxy will inspect the URI in the topmost Route header
          field value.  If it indicates this proxy, the proxy removes it
          from the Route header field (this route node has been
          reached).

      3.  The proxy will forward the request to the resource indicated
          by the URI in the topmost Route header field value or in the
          Request-URI if no Route header field is present.  The proxy
          determines the address, port and transport to use when
          forwarding the request by applying the procedures in [4] to
          that URI.

   If no strict-routing elements are encountered on the path of the
   request, the Request-URI will always indicate the target of the
   request.
*/

	//Rewrite URI if required
	//Parse URI
	//Compare domain part with this proxy
	//If same change to forward destination

	//Remove top Record-Route if this proxy
	//Parse first Record-Route
	//Record-Route: <sip:12.34.56.78;lr>
	//If domain part same as this proxy do not use to build new message

	//Add Record-Route if sending message to a PBX
	//Record-Route: <sip:12.34.56.78;lr> if our proxy is 12.34.56.78

	//Rebuild the message with generated top line
	//and insert if non empty the new Record-Route on top of any exisiting ones.

	//Return 0 if no errors, 1 otherwise
	//Initial test just add a Record-Route so this proxy get's everything.
	//The non changing URI does not seem to confuse the Asterisk PBX or Vitelity GW
	//at this time. Maybe the adding of a Record-Route will affect this now?
	if(uType==GATEWAY && uRequestType==INVITE)
	{
		if((cp=strchr(cMessage,'\n')))
		{	//copy upto cp included
			char cSave[1];
			unsigned uLen=0;
			strncpy(cSave,cp+1,1);
			*(cp+1)=0;
			strncat(cMessageModified,cMessage,2047);
			uLen=strlen(cMessage);
			*(cp+1)=cSave[0];

			//add new content
			strcat(cMessageModified,"Record-Route: <sip:");
			uLen+=strlen("Record-Route: <sip:");
			strcat(cMessageModified,gcServerIP);
			uLen+=strlen(gcServerIP);
			strcat(cMessageModified,";lr>\r\n");
			uLen+=strlen(";lr>\r\n");

			//copy rest
			strncat(cMessageModified,cp+1,(2047-uLen));
			uLen+=strlen(cp+1);
			if(uLen>2047)
				logfileLine("readEv-process error","cMessageModified len");
			cpMsg=cMessageModified;
		}
	}
	return(0);

}//int iModifyMessage(char *cMessage)


unsigned uLoadDestinationFromFirstLine(void)
{
	//ACK sip:13103566265@64.62.134.36:5060 SIP/2.0
	if((cp=strchr(cFirstLine,'@')))
	{
		if(sscanf(cp+1,"%[0-9\\.]:%u ",cDestinationIP,&uDestinationPort)!=2)
			return(1);
	}
	else
	{
		return(1);
	}
	return(0);
}//unsigned uLoadDestinationFromFirstLine(void)


unsigned uLoadGWFromCallID(void)
{
	sprintf(cData,"%.255s",memcached_get(gsMemc,cCallID,strlen(cCallID),&sizeData,&flags,&rc));
	if(rc!=MEMCACHED_SUCCESS)
	{
		sprintf(cMsg,"SIP/2.0 481 Transaction Does Not Exist\r\nCSeq: %s\r\n",cCSeq);
		iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
		if(guLogLevel>3)
			logfileLine("readEv-process 481 Transaction Does Not Exist",cCallID);
		return(1);
	}
	if(guLogLevel>3 && cData[0])
		logfileLine("readEv-process uLoadGWFromCallID cData",cData);
	//uType=1=GATEWAY
	if(strstr(cData,";uType=1;"))
		//Initial INVITE by GW
		sscanf(cData,"cSourceIP=%[^;];uSourcePort=%u;",cDestinationIP,&uDestinationPort);
	else
		//Initial INVITE by PBX
		sscanf(cData,"cSourceIP=%*[^;];uSourcePort=%*u;cDestIP=%[^;];uDestPort=%u;",cDestinationIP,&uDestinationPort);
	if(guLogLevel>3)
	{
		sprintf(gcQuery,"cDestIP:%s uDestPort:%u",cDestinationIP,uDestinationPort);
		logfileLine("readEv-process uLoadGWFromCallID()",gcQuery);
	}
	return(0);
}//unsigned uLoadGWFromCallID(void)


unsigned uLoadPBXFromCallID(void)
{
	sprintf(cData,"%.255s",memcached_get(gsMemc,cCallID,strlen(cCallID),&sizeData,&flags,&rc));
	if(rc!=MEMCACHED_SUCCESS)
	{
		sprintf(cMsg,"SIP/2.0 481 Transaction Does Not Exist\r\nCSeq: %s\r\n",cCSeq);
		iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
		if(guLogLevel>3)
			logfileLine("readEv-process 481 Transaction Does Not Exist",cCallID);
		return(1);
	}
	if(guLogLevel>3 && cData[0])
		logfileLine("readEv-process uLoadPBXFromCallID cData",cData);
	//uType=2=PBX
	if(strstr(cData,";uType=2;"))
		//Initial INVITE by PBX
		sscanf(cData,"cSourceIP=%[^;];uSourcePort=%u;",cDestinationIP,&uDestinationPort);
	else
		//Initial INVITE by GW
		sscanf(cData,"cSourceIP=%*[^;];uSourcePort=%*u;cDestIP=%[^;];uDestPort=%u;",cDestinationIP,&uDestinationPort);
	if(guLogLevel>3)
	{
		sprintf(gcQuery,"cDestIP:%s uDestPort:%u",cDestinationIP,uDestinationPort);
		logfileLine("readEv-process uLoadPBXFromCallID()",gcQuery);
	}
	return(0);
}//unsigned uLoadPBXFromCallID(void)


void CallEndCIU(void)
{
	//update or create a new record a.b.c.d-ciu (channels in use by PBX) where a.b.c.d is the IP of the PBX leg of call.
	unsigned uChannelsInUse=0;
	char cCIU[2048]={""};
	char cBigData[2048]={""};
	char cSearch[128]={""};
	sprintf(cKey,"%s-ciu",cDestinationIP);

	sprintf(cCIU,"%.2047s",memcached_get(gsMemc,cKey,strlen(cKey),&sizeData,&flags,&rc));
	if(rc==MEMCACHED_SUCCESS)
	{
		//read top most CIU value
		sscanf(cCIU,"uChannelsInUse=%u;",&uChannelsInUse);
		if(guLogLevel>3)
			logfileLine("readEv-CallEndCIU read",cKey);
	}

	sprintf(cSearch,";cCallID=%s;",cCallID);
	if(cCIU[0] && strstr(cCIU,cSearch))
	{
		if(uChannelsInUse>0)
		{
			time_t luNow=0;
			time(&luNow);
			sprintf(cBigData,"uChannelsInUse=%u;cCallID=%s;CallEndCIU;luTime=%lu;",--uChannelsInUse,cCallID,luNow);
			if(cCIU[0])
			{
				strncat(cBigData,"\n",1);
				strncat(cBigData,cCIU,2047-(strlen(cBigData)+strlen(cCIU)));
			}
			rc=memcached_set(gsMemc,cKey,strlen(cKey),cBigData,strlen(cBigData),(time_t)0,(uint32_t)0);
			if(rc!=MEMCACHED_SUCCESS)
			{
				if(guLogLevel>3)
					logfileLine("readEv-CallEndCIU Could not create channels in use record",cKey);
			}
			else
			{
				if(guLogLevel>3)
					logfileLine("readEv-CallEndCIU set",cKey);
			}
		}
		else
		{
			if(guLogLevel>3)
				logfileLine("readEv-CallEndCIU already zero",cCallID);
		}
	}
	else
	{
		if(guLogLevel>3)
			logfileLine("readEv-CallEndCIU cCallID not found",cCallID);
	}

}// void CallEndCIU(void)


int CallStartCIU(void)
{
	//update or create a new record a.b.c.d-ciu (channels in use by PBX) where a.b.c.d is the IP of the PBX leg of call.
	unsigned uChannelsInUse=0;
	char cCIU[2048]={""};
	char cBigData[2048]={""};
	if(uType==PBX)
		sprintf(cKey,"%s-ciu",cSourceIP);
	else
		sprintf(cKey,"%s-ciu",cDestinationIP);

	sprintf(cCIU,"%.2047s",memcached_get(gsMemc,cKey,strlen(cKey),&sizeData,&flags,&rc));
	if(rc==MEMCACHED_SUCCESS)
	{
		//read top most CIU value
		sscanf(cCIU,"uChannelsInUse=%u;",&uChannelsInUse);
		if(guLogLevel>3)
			logfileLine("readEv-CallStartCIU read",cKey);
	}

	//testing
	//if(uLines && uChannelsInUse && uChannelsInUse>uLines)
	//if(1)
	if(0)
	{

		//You must setup a media server that will provide announcements
		//We are using the sems with announcement module only.
		sprintf(cDestinationIP,"127.0.0.1");
		uDestinationPort=5080;
		return(0);
	}

	time_t luNow=0;
	time(&luNow);
	sprintf(cBigData,"uChannelsInUse=%u;cCallID=%s;CallStartCIU;luTime=%lu;",++uChannelsInUse,cCallID,luNow);
	if(cCIU[0])
	{
		strncat(cBigData,"\n",1);
		strncat(cBigData,cCIU,2047-(strlen(cBigData)+strlen(cCIU)));
	}
	rc=memcached_set(gsMemc,cKey,strlen(cKey),cBigData,strlen(cBigData),(time_t)0,(uint32_t)0);
	if(rc!=MEMCACHED_SUCCESS)
	{
		if(guLogLevel>3)
			logfileLine("readEv-CallStartCIU Could not create channels in use record",cKey);
	}
	else
	{
		if(guLogLevel>3)
			logfileLine("readEv-CallStartCIU set",cKey);
	}

	return(0);

}//int CallStartCIU(void)


if(guLogLevel>4)
	logfileLine("readEv-process check source",cSourceIP);

//Check to see if request or reply is coming from valid source
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
	sscanf(cData,"cDestinationIP=%[^;];uDestinationPort=%u;uType=%u;uLines=%u;",cDestinationIP,&uDestinationPort,&uType,&uLines);
	if(cDestinationIP[0]==0 || uDestinationPort==0 || uType==0 )
	{
		logfileLine("readEv-process","cData incorrect");
		logfileLine("readEv-process",cData);
		return;
	}
}
//Approve gateway and get type

if(guLogLevel>4)
{
	if(uType==GATEWAY)
		logfileLine("readEv-process source ok gateway","");
	else
		logfileLine("readEv-process source ok pbx","");
}


//debug only
//return;

//
//Process requests
//	(e.g. commands like INVITE, ACK, BYE, CANCEL, OPTIONS, REGISTER and INFO)
//
if(!uReply)
{
	if(guLogLevel>4)
		logfileLine("readEv-process request","");
	if(!strncmp(cFirstLine,"INVITE",6))
	{
		uRequestType=INVITE;
		if(guLogLevel>4)
			logfileLine("readEv-process INVITE","");
		if(uType==GATEWAY)
		{
			if(guLogLevel>4)
				logfileLine("readEv-process GATEWAY","");
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
			if(guLogLevel>4)
				logfileLine("readEv-process PBX","");

			cDestinationIP[0]=0;
			uDestinationPort=0;
			register int i;
			for(i=0;gsRuleTest[i].cPrefix[0] && i<MAX_RULES;i++)
			{
				//Find first rule that matches prefix the 'Any' default route 
				//	cPrefix should come after all the number based ones.
				if(!strncmp(cDID,gsRuleTest[i].cPrefix,strlen(gsRuleTest[i].cPrefix)) || gsRuleTest[i].cPrefix[0]=='A')
				{
					//We are looking at the first one only still.
					//Need to add if first one fails try next
					//Need to add marking "down" gateways
					uDestinationPort=gsRuleTest[i].sAddr[0].uPort;
					sprintf(cDestinationIP,"%.31s",gsRuleTest[i].sAddr[0].cIP);
					break;
				}
			}

			if(!cDestinationIP[0] || !uDestinationPort)
			{
				sprintf(cMsg,"SIP/2.0 500 No outbound gateway\r\nCSeq: %s\r\n",cCSeq);
				if(iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort))
				{
					if(guLogLevel>3)
						logfileLine("readEv-process 500 No outbound gateway",cKey);
					return;
				}
			}
		}
		else if(1)
		{
			logfileLine("readEv-process unexpected uType",cData);
			return;
		}

		//create transaction -it maybe updated if INVITE comes in again with same CALL-ID
		//	but thsta should not matter.
		//the initiator is the source for the duration of the callid based transaction
		//an inbound gw can never be a destination  in the callid record
		//the uType (save here at initial INVITE) can be used to determine whether the source or the dest is the GW
		if(guLogLevel>4)
			logfileLine("readEv-process set transaction","");
		sprintf(cData,"cSourceIP=%.15s;uSourcePort=%u;cDestIP=%.15s;uDestPort=%u;uType=%u;",
			cSourceIP,uSourcePort,cDestinationIP,uDestinationPort,uType);
		rc=memcached_set(gsMemc,cCallID,strlen(cCallID),cData,strlen(cData),(time_t)0,(uint32_t)0);
		if(rc!=MEMCACHED_SUCCESS)
		{
			sprintf(cMsg,"SIP/2.0 500 Could not create transaction\r\nCSeq: %s\r\n",cCSeq);
			if(iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort))
			{
				if(guLogLevel>3)
					logfileLine("readEv-process 500 Could not create transaction",cCallID);
				return;
			}
		}

		//update or create a new record a.b.c.d-ciu (channels in use by PBX) where a.b.c.d is the IP of the PBX leg of call.
		//returns non 0 value if no more lines or error.
		if(CallStartCIU())
		{
			//we will end the call here so we need to clean up
			CallEndCIU();
			return;
		}

		//after validation we let inviter know we are continuing.
		sprintf(cMsg,"SIP/2.0 100 Trying\r\nCSeq: %s\r\n",cCSeq);//Send back same CSeq check this
		iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);

	}//INVITE
	//OPTIONS
	else if(!strncmp(cFirstLine,"OPTIONS",7))
	{
		if(uType==GATEWAY)
		{
			if(guLogLevel>4)
				logfileLine("readEv-process OPTIONS GATEWAY","");
		}
		else
		{
			if(guLogLevel>4)
				logfileLine("readEv-process OPTIONS PBX","");
		}
		//Create OPTIONS response and send back.
		//
//Sample OPTIONS exchange 
//OPTIONS sip:5009@181.111.2.106:10273 SIP/2.0
//Via: SIP/2.0/UDP 69.61.19.10:5060;branch=z9hG4bK1886cc29;rport
//Max-Forwards: 70
//From: "Unknown" <sip:Unknown@69.61.19.10>;tag=as2c012818
//To: <sip:5009@181.111.2.106:10273>
//Contact: <sip:Unknown@69.61.19.10:5060>
//Call-ID: 1fb396b0676c407a17f42b9f7c7b02f1@69.61.19.10:5060
//CSeq: 102 OPTIONS
//User-Agent: asterisk
//Date: Wed, 27 May 2015 21:29:17 GMT
//Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, SUBSCRIBE, NOTIFY, INFO, PUBLISH
//Supported: replaces, timer
//Content-Length: 0
//
//
//SIP/2.0 200 OK
//Via: SIP/2.0/UDP 69.61.19.10:5060;branch=z9hG4bK1886cc29;rport=5060
//From: "Unknown" <sip:Unknown@69.61.19.10>;tag=as2c012818
//To: <sip:5009@181.111.2.106:10273>;tag=1965203006
//Call-ID: 1fb396b0676c407a17f42b9f7c7b02f1@69.61.19.10:5060
//CSeq: 102 OPTIONS
//Supported: replaces, path, timer, eventlist
//User-Agent: Grandstream HT-503  V1.1B 1.0.10.9  chip V2.2
//Allow: INVITE, ACK, OPTIONS, CANCEL, BYE, SUBSCRIBE, NOTIFY, INFO, REFER, UPDATE
//Content-Length: 0
//
		char cLargeMsg[1024]={""};

		strcat(cLargeMsg,"SIP/2.0 200 Ok\r\n");

		sprintf(cMsg,"Via: %s\r\n",cVia1);
		strcat(cLargeMsg,cMsg);

		sprintf(cMsg,"From: %s\r\n",cFrom);
		strcat(cLargeMsg,cMsg);

		sprintf(cMsg,"To: %s\r\n",cTo);
		strcat(cLargeMsg,cMsg);

		sprintf(cMsg,"Call-ID: %s\r\n",cCallID);
		strcat(cLargeMsg,cMsg);

		sprintf(cMsg,"CSeq: %s\r\n",cCSeq);
		strcat(cLargeMsg,cMsg);

		sprintf(cMsg,"Supported: %s\r\n","replaces, timer");
		strcat(cLargeMsg,cMsg);

		sprintf(cMsg,"User-Agent: %s\r\n","Unixservice Unxs uSIPSwitch v0.1");
		strcat(cLargeMsg,cMsg);

		sprintf(cMsg,"Allow: %s\r\n","INVITE, ACK, OPTIONS, CANCEL, BYE");
		strcat(cLargeMsg,cMsg);

		sprintf(cMsg,"Content-Length: %u\r\n",0);
		strcat(cLargeMsg,cMsg);

		iSendUDPMessageWrapper(cLargeMsg,cSourceIP,uSourcePort);
		return;
	}//OPTIONS
	//ACK
	else if(!strncmp(cFirstLine,"ACK",3))
	{
		if(uType==GATEWAY)
		{
			if(guLogLevel>4)
				logfileLine("readEv-process ACK GATEWAY","");
			if(uLoadPBXFromCallID())
			{
				if(uLoadDestinationFromFirstLine()) return;
			}

		}
		else
		{
			if(guLogLevel>4)
				logfileLine("readEv-process ACK PBX","");
			if(uLoadGWFromCallID())
			{
				if(uLoadDestinationFromFirstLine()) return;
			}

		}

		//If the source and the destination as calculated is the same we need to try something else
		if(!strncmp(cDestinationIP,cSourceIP,strlen(cSourceIP)))
		{
			if(guLogLevel>3)
				logfileLine("readEv-process ACK same dest as server trying cFirstLine",cSourceIP);
			if(uLoadDestinationFromFirstLine()) return;
			if(!strncmp(cDestinationIP,cSourceIP,strlen(cSourceIP)))
			{
				if(guLogLevel>3)
					logfileLine("readEv-process ACK same dest as server giving up",cSourceIP);
				//giving up
				return;
			}
		}
	}//ACK
	//BYE
	else if(!strncmp(cFirstLine,"BYE",3))
	{
		//always get PBX for ciu and cdr closing
		if(!uLoadPBXFromCallID())
		{
			CallEndCIU();
		}
		else
		{
			if(uLoadDestinationFromFirstLine()) return;
		}

		if(uType==GATEWAY)
		{
			if(guLogLevel>4)
				logfileLine("readEv-process BYE GATEWAY","");
			//already have PBX IP from above
		}
		else
		{
			if(guLogLevel>4)
				logfileLine("readEv-process BYE PBX","");
			if(uLoadGWFromCallID())
			{
				if(uLoadDestinationFromFirstLine()) return;
			}

		}

		//If the source and the destination as calculated is the same we need to try something else
		if(!strncmp(cDestinationIP,cSourceIP,strlen(cSourceIP)))
		{
			if(guLogLevel>3)
				logfileLine("readEv-process BYE same dest as server trying cFirstLine",cSourceIP);
			if(uLoadDestinationFromFirstLine()) return;
			if(!strncmp(cDestinationIP,cSourceIP,strlen(cSourceIP)))
			{
				if(guLogLevel>3)
					logfileLine("readEv-process BYE same dest as server giving up",cSourceIP);
				//giving up
				return;
			}
		}
	}//BYE
	//CANCEL
	else if(!strncmp(cFirstLine,"CANCEL",6))
	{
		//always get PBX for ciu and cdr closing
		if(!uLoadPBXFromCallID())
		{
			CallEndCIU();
		}
		else
		{
			if(uLoadDestinationFromFirstLine()) return;
		}
		//if PBX did the invite then the call-id source is the PBX and destination is the GW.
		//if the GW did the invite then the call-id source is the GW and the destination is the PBX.
		//so to save time here we should identify the case in the call-id record.
		if(uType==GATEWAY)
		{
			if(guLogLevel>4)
				logfileLine("readEv-process CANCEL GATEWAY","");
			//already have destination IP from above
		}
		else
		{
			if(guLogLevel>4)
				logfileLine("readEv-process CANCEL PBX","");
			if(uLoadGWFromCallID())
			{
				if(uLoadDestinationFromFirstLine()) return;
			}

		}

		//If the source and the destination as calculated is the same we need to try something else
		if(!strncmp(cDestinationIP,cSourceIP,strlen(cSourceIP)))
		{
			if(guLogLevel>3)
				logfileLine("readEv-process CANCEL same dest as server trying cFirstLine",cSourceIP);
			if(uLoadDestinationFromFirstLine()) return;
			if(!strncmp(cDestinationIP,cSourceIP,strlen(cSourceIP)))
			{
				if(guLogLevel>3)
					logfileLine("readEv-process CANCEL same dest as server giving up",cSourceIP);
					//giving up
					return;
			}
		}
	}//CANCEL
	else
	{
		//unknown or unsupported request
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
	//we need to clean up channel use here also.
	//for example a 500 type error from GW
	//perm errors we clean the PBX channels in use.
	if(uReply>=400)
	{
		if(!uLoadPBXFromCallID())
		{
			CallEndCIU();
		}
	}

	if(guLogLevel>4)
		logfileLine("readEv-process reply","");

	sprintf(cData,"%.255s",memcached_get(gsMemc,cCallID,strlen(cCallID),&sizeData,&flags,&rc));
	if(rc!=MEMCACHED_SUCCESS)
	{
		sprintf(cMsg,"SIP/2.0 481 Transaction Does Not Exist\r\nCSeq: %s\r\n",cCSeq);
		iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
		if(guLogLevel>3)
			logfileLine("readEv-process 481 Transaction Does Not Exist",cCallID);
		return;
	}
	if(guLogLevel>3 && cData[0])
		logfileLine("readEv-process reply cData",cData);
	//parse cData for forwarding information
	sscanf(cData,"cSourceIP=%[^;];uSourcePort=%u;",cDestinationIP,&uDestinationPort);
	if(guLogLevel>3)
	{
		sprintf(gcQuery,"cSourceIP:%s uSourcePort:%u uReply=%u",cDestinationIP,uDestinationPort,uReply);
		logfileLine("readEv-process",gcQuery);
	}
}//if a reply



//
//Forward unmodified packet
if(cDestinationIP[0])
{
	if(!uDestinationPort) uDestinationPort=DEFAULT_SIP_PORT;

	//Rewrite URI and Record-Route if required
	//

	if(iModifyMessage(cMessage))
	{
		sprintf(cMsg,"SIP/2.0 500 Forward failed-0\r\nCSeq: %s\r\n",cCSeq);
		iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
		if(guLogLevel>3)
			logfileLine("readEv-process iModifyMessage error",cCallID);
	}

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
}//if(cDestinationIP[0] && uDestinationPort)
else
{
	sprintf(cMsg,"SIP/2.0 500 Forward failed-2\r\nCSeq: %s\r\n",cCSeq);
	iSendUDPMessageWrapper(cMsg,cSourceIP,uSourcePort);
	if(guLogLevel>3)
		logfileLine("readEv-process unexpected no cDestinationIP/uDestinationPort",cCallID);
}

//no more readEv() sections

