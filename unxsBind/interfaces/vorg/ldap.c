/*
FILE
	ldap.c
	$Id: ldap.c 1134 2010-01-04 22:50:11Z Gary $
PURPOSE
	Provide for LDAP based login for mysqlRAD2/3
	based software.
AUTHOR/LEGAL
	(C) 2009-2010 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
NOTES
	tClient must have the LDAP provided OU.
	LDAP schema must be setup in accordance with:
	cFilter, cSearchDN, caAttrs, cOULinePattern and cOUPattern.
*/

#include <lber.h>
#include <ldap.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <lber.h>
#include <ldap.h>

//local protos
void ldapErrorLog(char *cMessage,LDAP *ld);

//extern protos
void iDNSLog(unsigned uTablePK, char *cTableName, char *cLogEntry);//main.c

//Must provide on call to at least a 32 char cOrganization buffer.
//Depending on whether an OpenLDAP or an AD LDAP server is used
//cLogin must be passed with domain information in different formats
//Ex1 AD: jonhdoe@unixservice.com
//Ex2 OpenLDAP: CN=johndoe,OU=members,DC=unixservice,DC=com
int iValidLDAPLogin(const char *cLogin, const char *cPasswd, char *cOrganization)
{
	LDAP *ld;
	LDAPMessage *ldapMsg;
	LDAPMessage *ldapEntry;
	BerElement *berElement;
	int iDesiredVersion=LDAP_VERSION3;
	struct berval structBervalCredentials;
	struct berval **structBervals;
	int  iRes,i;
	char *cpAttr;
	char *cp;
	char *cp2;

	//Most of these should probably be set via tConfiguration
	//for utmost flexibilty.
	char *cFilter="(objectClass=*)";
	char *cSearchDN="ou=members,dc=unixservice,dc=com";
	char *cURI="ldap://127.0.0.1";
	char *caAttrs[8]={"memberOf",NULL};
	char cOULinePattern[32]={"_admins"};
	char cOUPattern[32]={"OU="};

	//Initialize LDAP data structure
	ldap_initialize(&ld,cURI);
	if(ld==NULL)
	{
		ldapErrorLog("ldap_initialize() failed",NULL);
		return(0);
	}

	//set the LDAP version to be 3
	if(ldap_set_option(ld,LDAP_OPT_PROTOCOL_VERSION,&iDesiredVersion)!=LDAP_OPT_SUCCESS)
	{
		ldapErrorLog("ldap_set_option()",ld);
		return(0);
	}

	//Connect/bind to LDAP server
	structBervalCredentials.bv_val=(char *)cPasswd;
	structBervalCredentials.bv_len=strlen(cPasswd);
	if(ldap_sasl_bind_s(ld,cLogin,NULL,&structBervalCredentials,NULL,NULL,NULL)!=LDAP_SUCCESS)
	{
		ldapErrorLog("ldap_sasl_bind_s()",ld);
		return(0);
	}

	//Initiate sync search
	if(ldap_search_ext_s(ld,cSearchDN,LDAP_SCOPE_SUBTREE,cFilter,caAttrs,0,NULL,NULL,NULL,0,&ldapMsg)!=LDAP_SUCCESS)
	{
		ldapErrorLog("ldap_search_ext_s()",ld);
		return(0);
	}

	//Iterate through the returned entries
	for(ldapEntry=ldap_first_entry(ld,ldapMsg);ldapEntry!=NULL;ldapEntry=ldap_next_entry(ld,ldapEntry))
	{

		for(cpAttr=ldap_first_attribute(ld,ldapEntry,&berElement);cpAttr!=NULL;
					cpAttr=ldap_next_attribute(ld,ldapEntry,berElement))
		{
			if((structBervals=ldap_get_values_len(ld,ldapEntry,cpAttr))!=NULL)
			{
				for(i=0;structBervals[i]!=NULL;i++)
				{
					if(strstr(structBervals[i]->bv_val,cOULinePattern))
					{
						if((cp=strstr(structBervals[i]->bv_val,cOUPattern)))
						{
							if((cp2=strchr(cp+3,',')))
								*cp2=0;
							sprintf(cOrganization,"%.31s",cp+3);
						}
					}
				}
				ldap_value_free_len(structBervals);
			}
			ldap_memfree(cpAttr);
		}

		if(berElement!=NULL)
			ber_free(berElement,0);
	}

	//Cleanup (TODO is ldapEntry mem cleaned above? Check)
	ldap_msgfree(ldapMsg);
	//Ignore errors
	iRes=ldap_unbind_ext_s(ld,NULL,NULL);

	if(cOrganization[0])
		return(1);
	else
		return(0);

}//int iValidLDAPLogin()


void ldapErrorLog(char *cMessage,LDAP *ld)
{
	int iResultCode;
	char cLogEntry[256];

	ldap_get_option(ld,LDAP_OPT_RESULT_CODE,&iResultCode);
	//Attempt to log to tLog
	sprintf(cLogEntry,"%s:%s",cMessage,ldap_err2string(iResultCode));
	iDNSLog(0,"ldapErrorLog",cLogEntry);
	
}//void ldapErrorLog(char *cMessage,LDAP *ld)
