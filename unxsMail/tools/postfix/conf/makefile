#
#FILE
#       makefile
#	svn ID removed
#PURPOSE
#       create/maintain postfix hash tables
#AUTHOR
#	Gary Wallis for openisp.net (C) 2006
#NOTES
#	tricky makefile stuff is used but is not clear
#

#other possible hash tables that may be needed
#all: aliases.db relay_domains.db relay_recipients.db transport.db local_domains.db access.db virtusertable.db sender_restrictions.db

#minimum basic mailgateway tables needed
#all: aliases.db relay_domains.db relay_recipients.db transport.db local_domains.db

#pretty much full featured like sendmail
all: aliases.db relay_domains.db relay_recipients.db transport.db local_domains.db access.db virtusertable.db


%.db: %
	@/usr/sbin/postmap $<
