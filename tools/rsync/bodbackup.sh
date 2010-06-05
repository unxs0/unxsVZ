#!/bin/bash
#
#FILE
#	tools/rsync/bodbackup.sh
#PURPOSE
#	unxsVZ server backup script that allows for boot from snapshot drive if primary drive
#	fails. This requires that the drives be identical and setup correctly (/boot fdisk etc.)
#
#	A simple KISS example that however has worked in production.
#	Here for /dev/sda and /dev/sdb 2 drive server.
#	Bunch of drives (bod) local rsync to unmounted "mirror" snapshot drive on same
#	server.
#NOTES
#	Non LVM2 version! But should be very similar.
#	In some hot swap SATA servers the server will boot from an unknown drive/part
#	so we use the unique hdparm serial numbers for HD id.
#	We could use UUID instead of the serial number.
#
#TODO
#	Set everything up with top constants section.

#This servers drives
HD0Serial="8SF0JGS9";
HD1Serial="8SF0JJNY";

DRIVE0="/dev/sda";
DRIVE1="/dev/sdb";

BAKROOTDEV="/dev/sdb1";
BAKVZDEV="/dev/sdb3";

SRCROOTDEV="/dev/sda1";
SRCVZDEV="/dev/sda3";

#Assumed partitions / and /vz

#Nothing below should need to be changed.

GotSerial0=`/sbin/hdparm -i $DRIVE0 | grep SerialNo | awk '{print $6}'`;
GotSerial1=`/sbin/hdparm -i $DRIVE1 | grep SerialNo | awk '{print $6}'`;

if [ $HD0Serial == $GotSerial0 ];then
        echo "$DRIVE0 serial number ok";
else
        echo "$DRIVE0 serial number does not match our records, drives swapped?";

        if [ $HD0Serial == $GotSerial1 ];then
                echo "Drives swapped";
        else
                echo "Error! $DRIVE0 serial numbers do not match our records!";
                exit 1;
        fi
fi

if [ $HD1Serial == $GotSerial1 ];then
        echo "$DRIVE1 serial number ok";
else
        echo "$DRIVE1 serial number does not match our records, drives swapped?";

        if [ $HD1Serial == $GotSerial0 ];then
                echo "Drives swapped";
        else
                echo "Error! $DRIVE1 serial numbers do not match our records!";
                exit 1;
        fi
fi

#Now make sure we are not cross mounted.
ROOTDEV=`/bin/mount | grep -w "on /" | awk '{print $1}'`;
VZDEV=`/bin/mount | grep -w "on /vz" | awk '{print $1}'`;

if [ $ROOTDEV == $SRCROOTDEV ];then
        echo " $SRCROOTDEV is mounted as expected";
else
        if [ $ROOTDEV == $BAKROOTDEV ];then
                echo "Warning $BAKROOTDEV is mounted on /";
		#switch
                BAKROOTDEV="$SRCROOTDEV";
        else
                echo "Error! unexpected device $ROOTDEV is mounted on / aborting";
                exit 1;
        fi
fi

if [ $VZDEV == $SRCVZDEV ];then
        echo "/dev/sda3 is mounted as expected";
else
        if [ $VZDEV == $BAKVZDEV ];then
                echo "Warning $BAKVZDEV is mounted on /vz";
		#switch
                BAKVZDEV="$SRCVZDEV";
        else
                echo "Error! unexpected device $VZDEV is mounted on /vz aborting";
                exit 1;
        fi
fi

#debug only
#echo "BAKVZDEV=$BAKVZDEV BAKROOTDEV=$BAKROOTDEV";
#exit 0;

#Local mirror disk upkeep

umount $BAKROOTDEV > /dev/null 2>&1;
umount $BAKVZDEV > /dev/null 2>&1;

echo ""
echo "mkdirs..."
/bin/mkdir -p /mnt/root
if [ $? -gt 0 ]; then
        echo "Could not create /mnt/root. Aborting";
        exit 1;
fi
/bin/mkdir -p /mnt/vz
if [ $? -gt 0 ]; then
        echo "Could not create /mnt/vz. Aborting";
        /bin/rmdir /mnt/root;
        exit 1;
fi

echo ""
echo "mounting..."
/bin/mount $BAKVZDEV /mnt/vz
if [ $? -gt 0 ]; then
        echo "Could not mount $BAKVZDEV. Aborting";
        exit 1;
fi

/bin/mount $BAKROOTDEV /mnt/root
if [ $? -gt 0 ]; then
        echo "Could not mount $BAKROOTDEV. Aborting";
        /bin/rmdir /mnt/root;
        /bin/rmdir /mnt/vz;
        exit 1;
fi

#disk must be mounted or else we 100% the root partition!

#this command may take forever in systems like mail stores with many open files.
#may not be needed since we should only do best effort not perfect backup.
#/bin/sync;

echo ""
echo "rsync vz partition..."
/usr/bin/rsync -avxlH --delete /vz/* /mnt/vz

echo ""
echo "rsync root partition..."
/usr/bin/rsync -avxlH --delete --exclude "/mnt/root/" --exclude "/mnt/vz/" --exclude "/vz" --exclude "/proc/" --exclude "/root/.ccache/" --exclude "/sys" --exclude "/dev" --exclude "/tmp" /* /mnt/root

echo ""
echo "un-mounting..."
/bin/umount /mnt/root
/bin/umount /mnt/vz

echo ""
echo "rm temp /mnt dirs..."
/bin/rmdir /mnt/root
/bin/rmdir /mnt/vz

echo ""
echo "done"

