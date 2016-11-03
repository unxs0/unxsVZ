#
# svn ID removed
# tools/freepbx/qos/makefile
#
# (C) 2012 Gary Wallis for Unixservice, LLC.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#	For help contact support @ openisp . net
#	Support for non CentOS 5+ linux systems is not tested or supported yet.
#
#	gcc -Wall test.c -lm -o /root/NodeQOS

CFLAGS= -Wall
LIBS= -lm

all: nodeQOS

nodeQOS: nodeqos.o 
	cc nodeqos.o -o nodeQOS $(LIBS) 

natpbx.o: nodeqos.c 
	cc -c nodeqos.c -o nodeqos.o $(CFLAGS)

clean:
	rm -f *.o nodeQOS

install: nodeQOS
	install -s nodeQOS /usr/sbin/nodeQOS;
	cp -i nodeQOSGraph.sh /usr/sbin/;
	cp -i qos.html /var/www/unxs/html/traffic/;
	sed -i -e "s/s1/`hostname -s`/" /var/www/unxs/html/traffic/qos.html;
	grep nodeQOSGraph.sh /var/spool/cron/root >/dev/null;\
	if [ $$? != 0 ];then\
		echo '* * * * * /usr/sbin/nodeQOSGraph.sh > /dev/null;' >> /var/spool/cron/root;\
	fi;
	rm nodeQOS;
