/*
FILE
	md5encrypt.c
PURPOSE
	Encrypt a string from the command line using MD5
	and print it on stdout.
AUTHOR
	Hugo Urquiza for Unixservice. (C) 2008
USAGE
	./md5encrypt text
COMPILE
	gcc -Wall -o md5encrypt md5encrypt.c -lm -lcrypt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <crypt.h>

void EncryptPasswdMD5(char *pw);

int main(int argc,char *argv[])
{
	if(!argv[0][0]) return(1);

	EncryptPasswdMD5(argv[1]);
	
	printf("%s",argv[1]);
	
	return(0);
}

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


void EncryptPasswdMD5(char *pw)
{
	char cSalt[] = "$1$01234567$";
        char *cpw;

    	(void)srand((int)time((time_t *)NULL));
    	to64(&cSalt[3],rand(),8);
	
	cpw = crypt(pw,cSalt);
	strcpy(pw,cpw);

}//void EncryptPasswdMD5(char *pw)
//End passwd stuff ;)
