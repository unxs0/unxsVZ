/*
FILE
	unxsVZ/tools/unxsvz/sbin/changestaffpasswd.c
	(C) 2014 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies
PURPOSE
	Simple but safe way to programatically change /etc/shadow staff passwd.
	Used by scripts that change the hardware node passwds or container
	passwds to a secure random string and log them to unxsVZ tNode tProperty table.
SEE
	unxsvzChangeStaffPasswd.sh
REQUIRES
	Linux
	Modern /bin/sed
BUILD
	gcc -o changestaffpasswd changestaffpasswd.c -lcrypt
*/




#include <stdlib.h>
#include <shadow.h>
#include <stdio.h>
#include <crypt.h>
#include <time.h>
#include <string.h>

//Passwd stuff
static unsigned char itoa64[] =         /* 0 ... 63 => ascii - 64 */
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void to64(s, v, n)
  register char *s;
  register long v;
  register int n;
{
    while (--n >= 0) {
        *s++ = itoa64[v&0x3f];
        v >>= 6;
    }
}//void to64(s, v, n)

void EncryptPasswd(char *pw)
{
	//Notes:
	//	We should change time based salt 
	//	(could be used for faster dictionary attack)
	//	to /dev/random based system.

        char *cpw;

	char cSalt[] = "$1$01234567$";
    	(void)srand((int)time((time_t *)NULL));
	to64(&cSalt[3],rand(),8);
	cpw = crypt(pw,cSalt);
	// error not verified, str NULL ("") returned	
	sprintf(pw,"%.99s",cpw);

}//void EncryptPasswd(char *pw)

//escape / and $ items
int ind(char *s, char c) {
    register int x;

    for(x=0;s[x];x++)
        if(s[x] == c) return x;

    return -1;
}

void escape_shell_cmd(char *cmd) {
    register int x,y,l;

    l=strlen(cmd);
    for(x=0;cmd[x];x++) {
        if(ind("&;`'/\"|*?~<>^()[]{}$\\",cmd[x]) != -1){
            for(y=l+1;y>x;y--)
                cmd[y] = cmd[y-1];
            l++; /* length has been increased */
            cmd[x] = '\\';
            x++; /* skip the character */
        }
    }
}


int main(int iArgc, char *cArgv[])
{

	if(iArgc!=3)
	{
		printf("usage: %s <non root user> <new passwd>\n",cArgv[0]);
		return(0);
	}

	if(strstr(cArgv[1],"root"))
	{
		printf("usage: %s <non root user> <new passwd>\n",cArgv[0]);
		return(0);
	}

	if(!lckpwdf())
	{
		char cPasswd[256]={""};
		sprintf(cPasswd,"%.32s",cArgv[2]);
		EncryptPasswd(cPasswd);
		//printf("%s\n",cPasswd);
		escape_shell_cmd(cPasswd);
		//printf("%s\n",cPasswd);
		char cSystem[256];
		sprintf(cSystem,"/bin/sed -i -e "
				"'s/^%s:$1$[a-zA-Z0-9/\\.]*$[a-zA-Z0-9/\\.]*:"
				"/%s:%s:/' /etc/shadow",cArgv[1],cArgv[1],cPasswd);
		if(system(cSystem))
		{
			fprintf(stderr,"could not modify shadow file\n");
			return(3);
		}
		if(ulckpwdf())
		{
			fprintf(stderr,"could not unlock shadow file\n");
			return(2);
		}
		return(0);
	}
	fprintf(stderr,"could not lock shadow file\n");
	return(1);
}
