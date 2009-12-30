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
/*
	int  result;
	int  auth_method = LDAP_AUTH_SIMPLE;
	char *root_dn = "dc=unixservice,dc=com";
	char *root_pw = "secret";

	BerElement* ber;
	LDAPMessage* msg;
	LDAPMessage* entry;

	char* base="dc=unixservice,dc=com";
	char* filter="(objectClass=*)";
	char* errstring;
	char* dn = NULL;
	char* attr;
	char** vals;
	int i;
*/

	//Connect to LDAP server
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


#ifdef CODESEGOFF
   if (ldap_bind_s(ld, root_dn, root_pw, auth_method) != LDAP_SUCCESS ) {
      ldap_perror( ld, "ldap_bind" );
      exit( EXIT_FAILURE );
   }

   if (ldap_search_s(ld, base, LDAP_SCOPE_SUBTREE, filter, NULL, 0, &msg) != LDAP_SUCCESS) {
      ldap_perror( ld, "ldap_search_s" );
      exit(EXIT_FAILURE);
   }

   printf("The number of entries returned was %d\n\n", ldap_count_entries(ld, msg));

   /* Iterate through the returned entries */
   for(entry = ldap_first_entry(ld, msg); entry != NULL; entry = ldap_next_entry(ld, entry)) {

      if((dn = ldap_get_dn(ld, entry)) != NULL) {
	 printf("Returned dn: %s\n", dn);
	 ldap_memfree(dn);
      }

      for( attr = ldap_first_attribute(ld, entry, &ber); 
	    attr != NULL; 
	    attr = ldap_next_attribute(ld, entry, ber)) {
	 if ((vals = ldap_get_values(ld, entry, attr)) != NULL)  {
	    for(i = 0; vals[i] != NULL; i++) {
	       printf("%s: %s\n", attr, vals[i]);
	    }

	    ldap_value_free(vals);
	 }

	 ldap_memfree(attr);
      }

      if (ber != NULL) {
	 ber_free(ber,0);
      }

      printf("\n");
   }

   /* clean up */
   ldap_msgfree(msg);
   result = ldap_unbind_s(ld);

   if (result != 0) {
      fprintf(stderr, "ldap_unbind_s: %s\n", ldap_err2string(result));
      exit( EXIT_FAILURE );
   }

#endif

   return(EXIT_SUCCESS);

}//main()


void ldapErrorExit(char *cMessage,LDAP *ld)
{
	int iResultCode;

	ldap_get_option(ld,LDAP_OPT_RESULT_CODE,&iResultCode);
	fprintf(stderr,"%s: %s",cMessage,ldap_err2string(iResultCode));
	
	exit(EXIT_FAILURE);

}//void ldapErrorExit(char *cMessage,LDAP *ld)
