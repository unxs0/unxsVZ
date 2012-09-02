#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

void ctest1(int *);
void ctest2(int *);

int main(int argc, char **argv) 
{
	void *lib_handle;
	double (*fn1)(int *,char *cBuffer);
	double (*fn2)(int *,char *cBuffer);
	int x;
	char *error;

	lib_handle = dlopen("/var/lib/libctest.so", RTLD_LAZY);
	if(!lib_handle) 
	{
		fprintf(stderr, "%s\n", dlerror());
		exit(1);
	}

	fn1 = dlsym(lib_handle, "ctest1");
	if((error=dlerror())!=NULL)  
	{
		fprintf(stderr, "%s\n", error);
		exit(1);
	}
	fn2 = dlsym(lib_handle, "ctest2");
	if((error=dlerror())!=NULL)  
	{
		fprintf(stderr, "%s\n", error);
		exit(1);
	}


	(*fn1)(&x,"This is data");
	printf("Valx=%d\n",x);
	(*fn2)(&x,"This is more data");
	printf("Valx=%d\n",x);

	dlclose(lib_handle);
	return 0;
}
