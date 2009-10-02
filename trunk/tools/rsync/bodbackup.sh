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

#This servers drives
HD0Serial="8SF0JGS9";
HD1Serial="8SF0JJNY";

GotSerial0=`/sbin/hdparm -i /dev/sda | grep SerialNo | awk '{print $6}'`;
GotSerial1=`/sbin/hdparm -i /dev/sdb | grep SerialNo | awk '{print $6}'`;

if [ $HD0Serial == $GotSerial0 ];then
        echo "/dev/sda serial number ok";
else
        echo "/dev/sda serial number does not match our records, drives swapped?";

        if [ $HD0Serial == $GotSerial1 ];then
                echo "Drives swapped";
        else
                echo "Error! /dev/sda Serial numbers do not match our records!";
                exit 1;
        fi
fi

if [ $HD1Serial == $GotSerial1 ];then
        echo "/dev/sdb serial number ok";
else
        echo "/dev/sdb serial number does not match our records, drives swapped?";

        if [ $HD1Serial == $GotSerial0 ];then
                echo "Drives swapped";
        else
                echo "Error! /dev/sdb Serial numbers do not match our records!";
                exit 1;
        fi
fi

#Now make sure we are not cross mounted.
ROOTDEV=`/bin/mount | grep -w "on /" | awk '{print $1}'`;
VZDEV=`/bin/mount | grep -w "on /vz" | awk '{print $1}'`;
BAKROOTDEV="/dev/sdb1";
BAKVZDEV="/dev/sdb3";

if [ $ROOTDEV == "/dev/sda1" ];then
        echo "/dev/sda1 is mounted as expected";
else
        if [ $ROOTDEV == $BAKROOTDEV ];then
                echo "Warning /dev/sdb1 is mounted on /";
                BAKROOTDEV="/dev/sda1";
        else
                echo "Error! unexpected device is mounted on / aborting";
                exit 1;
        fi
fi

if [ $VZDEV == "/dev/sda3" ];then
        echo "/dev/sda3 is mounted as expected";
else
        if [ $VZDEV == $BAKVZDEV ];then
                echo "Warning /dev/sdb3 is mounted on /vz";
                BAKVZDEV="/dev/sda3";
        else
                echo "Error! unexpected device is mounted on /vz aborting";
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
/bin/mount $BAKROOTDEV /mnt/root
if [ $? -gt 0 ]; then
        echo "Could not mount $BAKROOTDEV. Aborting";
        /bin/rmdir /mnt/root;
        /bin/rmdir /mnt/vz;
        exit 1;
fi

#disk must be mounted or else we 100% the root partition!

/bin/sync;

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

