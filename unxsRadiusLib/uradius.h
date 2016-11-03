/*
FILE
	uradius.h
	svn ID removed
PURPOSE
	For use with openisp.net liburadius.a
AUTHOR
	(C) Gary Wallis, 2006. GPL2 License applies
*/

int RadiusAuth(const char *cLogin,const char *cPasswd,const char *cRadiusServer,
		const char *cSharedSecret,char *cAuxAttributes, unsigned short uAttempts);
int RadiusAcct(char *cRadiusServer,char *cSharedSecret,char *cAttributes);
