/*
FILE 
	unxsVZ/uSIPSwitch/sipswitch/postparsecheck.c
	svn ID removed
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
	char cCallID[100]={""};
	unsigned uPBXPort=0;
	char cPBXIP[32]={""};

*/

//Previous section is parsemessage.c

//
//If anything is amiss send back an error message and return.
//
char cMsg[100]={""};

if(guLogLevel>4)
	logfileLine("readEv-postparse","start");

if(guLogLevel>4)
	logfileLine("readEv-postparse","end");
//next section is sipswitch/process.c
