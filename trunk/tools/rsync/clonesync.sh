#!/bin/bash
#
#FILE
#	tools/rsync/clonesync.sh
#PURPOSE
#	keep a cloned unxsVZ container on diff node rsync'd
#NOTES
#	This is a general sync script. Certain types of
#	containers can be kept in sync much faster by only
#	rsync'ing specific container service and log files.
#	For example an Asterix FreePBX container can be analyzed
#	to see what actually changes over a long period and then
#	have this script modified just for the service important items.

cSSHOptions="-C -c blowfish -p 22";

if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ];then
	echo "usage: $0 <source VEID> <target VEID> <target node host>";
fi

#under construction
exit;

#flush as much as possible to disk.
#special services could have hotcopy type commands here.
#if corruptible files will be transferred we need to vzdump with LVM
#and rsync from a dir structure taken from that snaphot.
#this is very slow and HD space consuming.
#so we test as herein a quick and dirty or a targeted single service type
#container approach
/usr/sbin/vzctl exec $1 /bin/sync;
if [ $? != 0 ];then
	echo "/usr/sbin/vzctl exec $1 /bin/sync failed";
	exit 1;
fi

#need to add ssh tunnel stuff here...
echo ""
echo "rsync container private..."
/usr/bin/rsync -avxlH --delete --exclude "/proc/" --exclude "/root/.ccache/" \
			--exclude "/sys" --exclude "/dev" --exclude "/tmp" \
			/vz/private/$1/* /vz/private/$2

echo ""
echo "done"

