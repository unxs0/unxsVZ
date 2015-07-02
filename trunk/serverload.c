//gcc -Wall -o unxsvzServerLoad.exe serverload.c
#include <stdio.h>
#define LINUX_SYSINFO_LOADS_SCALE 65536
#include <sys/sysinfo.h>
int main()
{
	struct sysinfo structSysinfo;
	if(sysinfo(&structSysinfo))
		return(1);
	printf("%lu\n",structSysinfo.loads[1]/LINUX_SYSINFO_LOADS_SCALE);
	return(0);
}
