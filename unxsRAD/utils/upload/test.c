#include "/usr/include/openisp/upload.h"

int main(int argc, const char * argv[])
{
	char cFilename[100]={""};
	char cTitle[100]={""};
	char cDescription[512]={""};

	printf("Content-type: text/plain\n\n");
	int iRetVal=iUpload(argc,argv,cFilename,cTitle,cDescription);
	if(iRetVal)
		printf("Error");
	else
		printf("Ok %s %s\n%s",cFilename,cTitle,cDescription);
	exit(0);
}
