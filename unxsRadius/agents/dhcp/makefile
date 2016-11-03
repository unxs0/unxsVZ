#
#FILE
# unxsVZ/unxsRadius/agents/dhcp/makefile
# svn ID removed
#
# (C) 2001-2009 Gary Wallis.
#
# Notes:
#	We only test on Linux CentOS5. It is up to you to provide patches for
#	other OSs and/or distros.
#

CFLAGS=-Wall

LIBS=-L/usr/lib/mysql -lmysqlclient -lssl -lm

all: unxsRadiusDHCPConf

unxsRadiusDHCPConf: dhcpd-conf.o 
	cc dhcpd-conf.o -o unxsRadiusDHCPConf $(LIBS) 

dhcpd-conf.o: dhcpd-conf.c ../../mysqlrad.h
	cc -c dhcpd-conf.c -o dhcpd-conf.o $(CFLAGS)

clean:
	rm -f *.o unxsRadiusDHCPConf

install: unxsRadiusDHCPConf
	install -s unxsRadiusDHCPConf /usr/sbin/unxsRadiusDHCPConf
	rm unxsRadiusDHCPConf
