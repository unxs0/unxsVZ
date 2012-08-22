
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libmemcached/memcached.h>

char *memcached_get (memcached_st *ptr,
                     const char *key, size_t key_length,
                     size_t *value_length,
                     uint32_t *flags,
                     memcached_return *error);

//You need the memcached server running on localhost:11211

int main(int argc, char *argv[])
{
	memcached_server_st *servers = NULL;
	memcached_st *memc;
	memcached_return rc;
	char *key= "acme";
	char *value= "acme value aksjdhaksdh kashdkashd askdhaksdh askjdh aksjdh akjshd kjash dkjahsd kjahsd kjha sdkjha skjdh aksjdh kajdh aksjdhkjh kasjdh kajsdh akjsdh kajshd kajshd kah";

	memcached_server_st *memcached_servers_parse(const char *server_strings);
	memc=memcached_create(NULL);

	servers=memcached_server_list_append(servers,"localhost",11211,&rc);
	rc=memcached_server_push(memc, servers);

	if(rc==MEMCACHED_SUCCESS)
		fprintf(stderr,"Added server successfully\n");
	else
		fprintf(stderr,"Couldn't add server: %s\n",memcached_strerror(memc, rc));

	rc=memcached_set(memc,key,strlen(key),value,strlen(value),(time_t)0,(uint32_t)0);

	if(rc==MEMCACHED_SUCCESS)
		fprintf(stderr,"Key stored successfully\n");
	else
		fprintf(stderr,"Couldn't store key: %s\n",memcached_strerror(memc,rc));

	char cValue[100]={""};
	size_t size=100;
	uint32_t flags=0;
	sprintf(cValue,"%.99s",memcached_get(memc,key,strlen(key),&size,&flags,&rc));
	if(rc==MEMCACHED_SUCCESS)
		fprintf(stderr,"Key retrieved successfully\n");
	else
		fprintf(stderr,"Couldn't retrieve key: %s\n",memcached_strerror(memc,rc));

	fprintf(stderr,"cValue=%s\n",cValue);

	return 0;
}//main()
