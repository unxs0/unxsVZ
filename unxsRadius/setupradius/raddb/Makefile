#
# Makefile
#
# Version:      $Id: Makefile,v 1.36 2007/12/31 03:40:31 aland Exp $
#

include ../Make.inc

#
#  The list of files to install.
#
FILES = acct_users attrs attrs.access_reject attrs.accounting_response	\
	attrs.pre-proxy clients.conf dictionary eap.conf templates.conf	\
	experimental.conf hints huntgroups ldap.attrmap otp.conf	\
	policy.txt preproxy_users proxy.conf radiusd.conf		\
	snmp.conf sql.conf sqlippool.conf users	policy.conf	\
	sites-available/default sites-available/example \
	sites-available/README

#
#	This target is here for local debugging
#
all:
	@if [ ! -d sites-enabled ]; then \
		mkdir sites-enabled; \
	fi
	@if [ ! -f sites-enabled/default ]; then \
		cd sites-enabled && ln -s ../sites-available/default; \
	fi

install:
	$(INSTALL) -d -m 750	$(R)$(raddbdir)
	$(INSTALL) -d -m 750	$(R)$(raddbdir)/sites-available
	$(INSTALL) -d -m 750	$(R)$(raddbdir)/sites-enabled
	@echo "Creating/updating files in $(R)$(raddbdir)"; \
	for i in $(FILES); do \
		[ ! -f $(R)$(raddbdir)/$$i ] && $(INSTALL) -m 640 $$i $(R)$(raddbdir)/$$i; \
		if [ "`find $$i -newer $(R)$(raddbdir)/$$i`" ]; then \
			echo "** $(R)$(raddbdir)/$$i"; \
			nt=1; \
		fi; \
	done; \
	if [ "$$nt" ]; then \
		echo "**";\
		echo "** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING **";\
		echo "** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING **";\
		echo "**";\
		echo "** The sample configuration files in `pwd`";\
		echo "** are newer than those in $(R)$(raddbdir)";\
		echo "**";\
		echo "** Please investigate and manually copy (if appropriate) the files listed above.";\
		echo "**";\
		echo "** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING **";\
		echo "** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING **";\
	fi
	chmod 640 $(R)$(raddbdir)/clients.conf
	if [ ! -d $(R)$(raddbdir)/certs ]; then \
		$(INSTALL) -d -m 750	$(R)$(raddbdir)/certs; \
		$(INSTALL) -m 750 certs/bootstrap $(R)$(raddbdir)/certs; \
		for i in Makefile README xpextensions ca.cnf server.cnf client.cnf; do \
			$(INSTALL) -m 640 certs/$$i $(R)$(raddbdir)/certs; \
		done; \
	fi
	if [ ! -d $(R)$(raddbdir)/sql ]; then \
		find sql -name CVS -prune -o -print | \
		  while read file; do \
		    if [ -d "$$file" ]; then \
		      $(INSTALL) -d -m 750 "$(R)$(raddbdir)/$$file"; \
		    else \
		      $(INSTALL) -m 640 "$$file" "$(R)$(raddbdir)/$$file"; \
		    fi; \
		  done; \
	fi
	if [ ! -f $(R)$(raddbdir)/sites-enabled/default ]; then \
		cd $(R)$(raddbdir)/sites-enabled/; \
		ln -s ../sites-available/default; \
	fi

clean:
