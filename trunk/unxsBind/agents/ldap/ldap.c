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

EXAMPLE USAGE
	/usr/sbin/unxsLDAP 'cn=Dylan Wallis,dc=unixservice,dc=com' 'secret' \
				'(objectClass=inetOrgPerson)' 'dc=unixservice,dc=com'

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
	char *cURI="ldap://10.0.4.36";
	int iDesiredVersion=LDAP_VERSION3;
	struct berval structBervalCredentials;
	char *cFilter="(objectClass=*)";
	char *cSearchDN="dc=ad,dc=cenet,dc=catholic,dc=edu,dc=au";
	LDAPMessage *ldapMsg;
	LDAPMessage *ldapEntry;
	struct berval **structBervals;
	BerElement *berElement;
	char *caAttrs[8]={"memberOf",NULL};
	int  iRes,i;
	char *cpDN=NULL;
	char *cpAttr;

	if(iArgc<3 || iArgc>5)
	{
		printf("usage %s: <cLoginDN> <cLoginPassword> [<cFilter>] [<cSearchDN>]\n",cArgv[0]);
		exit(0);
	}

	if(iArgc>=4)
		cFilter=cArgv[3];

	if(iArgc==5)
		cSearchDN=cArgv[4];

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
	if(ldap_sasl_bind_s(ld,cArgv[1],NULL,&structBervalCredentials,NULL,NULL,NULL)!=LDAP_SUCCESS)
		ldapErrorExit("ldap_sasl_bind_s()",ld);
	//debug only
	printf("ldap_sasl_bind_s() ok\n");

	//Initiate sync search
	//if(ldap_search_ext_s(ld,cSearchDN,LDAP_SCOPE_BASE,cFilter,NULL,0,NULL,NULL,NULL,0,&ldapMsg)!=LDAP_SUCCESS)
	if(ldap_search_ext_s(ld,cSearchDN,LDAP_SCOPE_SUBTREE,cFilter,caAttrs,0,NULL,NULL,NULL,0,&ldapMsg)!=LDAP_SUCCESS)
		ldapErrorExit("ldap_search_ext_s()",ld);
	//debug only
	printf("ldap_search_ext_s() ok. Returned %d entries\n\n",ldap_count_entries(ld,ldapMsg));

	//Iterate through the returned entries
	for(ldapEntry=ldap_first_entry(ld,ldapMsg);ldapEntry!=NULL;ldapEntry=ldap_next_entry(ld,ldapEntry))
	{

		if((cpDN=ldap_get_dn(ld,ldapEntry))!= NULL)
		{
			printf("Returned dn: %s\n",cpDN);
			ldap_memfree(cpDN);
		}

		for(cpAttr=ldap_first_attribute(ld,ldapEntry,&berElement);cpAttr!=NULL;
					cpAttr=ldap_next_attribute(ld,ldapEntry,berElement))
		{
			if((structBervals=ldap_get_values_len(ld,ldapEntry,cpAttr))!=NULL)
			{
				for(i=0;structBervals[i]!=NULL;i++)
					printf("%s: %s\n",cpAttr,structBervals[i]->bv_val);
				ldap_value_free_len(structBervals);
			}
			ldap_memfree(cpAttr);
		}

		if(berElement!=NULL)
			ber_free(berElement,0);

		printf("\n");
	}

	//Cleanup (TODO is ldapEntry mem cleaned above? Check)
	ldap_msgfree(ldapMsg);
	iRes=ldap_unbind_ext_s(ld,NULL,NULL);
	if(iRes!=0)
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
