#include "/usr/include/openisp/upload.h"

int main(int argc, const char * argv[])
{
	char cFilename[100]={""};
	char cTitle[100]={""};
	char cDescription[512]={""};
	unsigned uImageNumber=0;

	printf("Content-type: text/plain\n\n");
	int iRetVal=iUpload(argc,argv,cFilename,cTitle,cDescription,&uImageNumber);
	if(iRetVal)
		printf("Error");
	else
		printf("Ok %s %s %u\n%s",cFilename,cTitle,uImageNumber,cDescription);
	exit(0);
}
