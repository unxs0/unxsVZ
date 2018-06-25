#include "/usr/include/openisp/upload.h"

int main(int argc, const char * argv[])
{
	char cFilename[100]={""};
	char cName[100]={""};
	char cMessage[512]={""};

	printf("Content-type: text/plain\n\n");
	int iRetVal=upload(argc,argv,cFilename,cName,cMessage);
	if(iRetVal)
		printf("Error");
	else
		printf("Ok %s %s\n%s",cFilename,cName,cMessage);
	exit(0);
}
