#include <stdio.h>
#include <string.h>

/* why mess with perl, regex, etc. when we can parse very easily with C? */

//compile: gcc -Wall parserules.c -o parserules

int main(int iArgc, char *cArgv[])
{
	FILE *fp;

	if(iArgc!=2)
	{
		printf("usage: %s <snort .rules file>\n",cArgv[0]);
		return(0);
	}

	if((fp=fopen(cArgv[1],"r"))!=NULL)
	{
		char cLine[1024];
		while(fgets(cLine,1023,fp)!=NULL)
		{
			if(cLine[0]=='#') continue;
			if(strncmp(cLine,"alert",5))
				continue;
			char *cp;
			char *cp2;
			if((cp=strstr(cLine,"sid:")))
			{
				if((cp2=strchr(cp+4,';')))
				{
					*cp2=0;
					printf("\"%s\"",cp+4);
					*cp2=';';
				}
			}
			if((cp=strstr(cLine,"msg:")))
			{
				if((cp2=strchr(cp+4,';')))
				{
					*cp2=0;
					printf(",%s\n",cp+4);
					*cp2=';';
				}
			}
		}
		fclose(fp);
	}
	else
	{
		printf("error: could not open %s\n",cArgv[1]);
		return(1);
	}
	return(0);
}//main()
