/*
FILE
	ldap.c
	$Id$
PURPOSE
	Get authentication and authorization data from an LDAP server
	using libldap.
AUTHOR
	(C) Gary Wallis for Unixservice, LLC. 2009-2010. 
	GPLv2 license applies
	Based on deprecated code in the public domain ldap_search.c by Rory Winston 2002
	Dec 2009 found at http://linuxdevcenter.com/pub/a/linux/2003/08/14/libldap.html

NOTES
	Starting out very simple.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <lber.h>
#include <ldap.h>

//local protos
void ldapErrorExit(char *cMessage,LDAP *ld);

int main(int iArgc, char *cArgv[])
{
	LDAP *ld;
	char *cURI="ldap://localhost";
	int iDesiredVersion=LDAP_VERSION3;
	struct berval structBervalCredentials;
	char *cFilter="(objectClass=*)";
	LDAPMessage *ldapMsg;
	struct berval **structBervals;

	int  result;
	BerElement* ber;
	LDAPMessage* entry;
	char* dn = NULL;
	char* attr;
	int i;

	if(iArgc<3 || iArgc>4)
	{
		printf("usage %s: <dn> <userPassword> [<cFilter>]\n",cArgv[0]);
		exit(0);
	}

	if(iArgc==4)
		cFilter=cArgv[3];

	//Initialize LDAP data structure
	ldap_initialize(&ld,cURI);
	if(ld==NULL)
	{
		perror("ldap_initialize() failed");
		exit(EXIT_FAILURE);
	}
	//debug only
	printf("ldap_initialize() ok\n");

	//set the LDAP version to be 3
	if(ldap_set_option(ld,LDAP_OPT_PROTOCOL_VERSION,&iDesiredVersion)!=LDAP_OPT_SUCCESS)
		ldapErrorExit("ldap_set_option()",ld);
	//debug only
	printf("ldap_set_option() for LDAP v%d ok\n",iDesiredVersion);


	//Connect/bind to LDAP server
	structBervalCredentials.bv_val=cArgv[2];
	structBervalCredentials.bv_len=strlen(cArgv[2]);
	//if(ldap_sasl_bind_s(ld,root_dn,LDAP_SASL_SIMPLE,&structBervalCredentials,NULL,NULL,NULL)!=LDAP_SUCCESS)
	if(ldap_sasl_bind_s(ld,cArgv[1],NULL,&structBervalCredentials,NULL,NULL,NULL)!=LDAP_SUCCESS)
		ldapErrorExit("ldap_sasl_bind_s()",ld);
	//debug only
	printf("ldap_sasl_bind_s() ok\n");

	//ldap_search_ext_s(ldap, base, scope, filter, attrs, attrsonly, NULL, NULL, NULL, 0, res);
	if(ldap_search_ext_s(ld,cArgv[1],LDAP_SCOPE_SUBTREE,cFilter,NULL,0,NULL,NULL,NULL,0,&ldapMsg)!=LDAP_SUCCESS)
		ldapErrorExit("ldap_search_ext_s()",ld);
	//debug only
	printf("ldap_search_ext_s() ok\n");
	printf("The number of entries returned was %d\n\n",ldap_count_entries(ld,ldapMsg));

	//Iterate through the returned entries
	for(entry=ldap_first_entry(ld,ldapMsg);entry!=NULL;entry=ldap_next_entry(ld,entry))
	{

		if((dn=ldap_get_dn(ld,entry))!= NULL)
		{
			printf("Returned dn: %s\n", dn);
			ldap_memfree(dn);
		}

		for(attr=ldap_first_attribute(ld,entry,&ber);attr!=NULL;attr=ldap_next_attribute(ld,entry,ber))
		{
			if((structBervals=ldap_get_values_len(ld,entry,attr))!=NULL)
			{
				for(i=0;structBervals[i]!=NULL;i++)
					printf("%s: %s\n",attr,structBervals[i]->bv_val);
				ldap_value_free_len(structBervals);
			}
			ldap_memfree(attr);
		}

		if(ber!=NULL)
			ber_free(ber,0);

		printf("\n");
	}

	ldap_msgfree(ldapMsg);
	result=ldap_unbind_ext_s(ld,NULL,NULL);
	if(result!=0)
		ldapErrorExit("ldap_unbind_s()",ld);

	return(EXIT_SUCCESS);

}//main()


void ldapErrorExit(char *cMessage,LDAP *ld)
{
	int iResultCode;

	ldap_get_option(ld,LDAP_OPT_RESULT_CODE,&iResultCode);
	fprintf(stderr,"%s: %s\n",cMessage,ldap_err2string(iResultCode));
	
	exit(EXIT_FAILURE);

}//void ldapErrorExit(char *cMessage,LDAP *ld)
