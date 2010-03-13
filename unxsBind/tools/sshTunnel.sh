#/bin/bash

#Setup a tunnel to not have to deal with MySQL SSL certs etc.
#Note that the remote MySQL server will most likey need a GRANT ALL for 
#the userbelow@at-server-itself since MySQL will see the connection as TCP from
#it's own IP. And not use the socket.
#
#Not sure if ssh can use a socket on the remote sice of the -L

/usr/bin/ssh -f -N -L 3307:node2vm:3306 root@node2vm
