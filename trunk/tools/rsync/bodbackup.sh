#!/bin/bash
#
#FILE
#	tools/rsync/bodbackup.sh
#PURPOSE
#	Custom server backup script that keeps a daily backup drive ready for emergency boot.
#	This requires that the drives be identical and setup correctly (MBR grub fdisk etc.)
#	Care is taken to not run if drives get cross mounted or other abnormal situations.

#This servers drives
HD0UUID="6b51bc8c-ca03-4678-b585-e122b2236559";
HD1UUID="fd952d79-dcd4-49fd-9e54-0f452f889a3d";

UUIDPART0="/dev/sda1";
UUIDPART1="/dev/sdb1";

SRCROOTDEV="/dev/mapper/VolGroup00-LogVol00";
SRCVZDEV="/dev/mapper/VolGroup00-LogVol03";

BAKROOTDEV="/dev/mapper/VolGroup01-LogVol00";
BAKVZDEV="/dev/mapper/VolGroup01-LogVol03";

#Assumed partitions / and /vz

#Nothing below should need to be changed.

GotUUID0=`/lib/udev/vol_id $UUIDPART0 | grep UUID | cut -f 2 -d =`;
GotUUID1=`/lib/udev/vol_id $UUIDPART1 | grep UUID | cut -f 2 -d =`;

if [ $HD0UUID == $GotUUID0 ];then
        echo "$UUIDPART0 UUID number ok";
else
        echo "$UUIDPART0 UUID number does not match our records, drives swapped?";

        if [ $HD0UUID == $GotUUID1 ];then
                echo "Drives swapped";
        else
                echo "Error! $UUIDPART0 UUID numbers do not match our records!";
                exit 1;
        fi
fi

if [ $HD1UUID == $GotUUID1 ];then
        echo "$UUIDPART1 UUID number ok";
else
        echo "$UUIDPART1 UUID number does not match our records, drives swapped?";

        if [ $HD1UUID == $GotUUID0 ];then
                echo "Drives swapped";
        else
                echo "Error! $UUIDPART1 UUID numbers do not match our records!";
                exit 1;
        fi
fi

#Check to see if we are cross mounted. This can happen on some servers
#solution use UUID in grub.conf
#We do not allow cross mounts.
ROOTDEV=`/bin/mount | grep -w "on /" | awk '{print $1}'`;
VZDEV=`/bin/mount | grep -w "on /vz" | awk '{print $1}'`;

if [ $ROOTDEV == $SRCROOTDEV ];then
        echo "$SRCROOTDEV is mounted as expected";
else
        if [ $ROOTDEV == $BAKROOTDEV ];then
                echo "Error $BAKROOTDEV is mounted on /";
		exit 1;
		#switch
                #BAKROOTDEV="$SRCROOTDEV";
        else
                echo "Error! unexpected device $ROOTDEV is mounted on / aborting";
                exit 1;
        fi
fi

if [ $VZDEV == $SRCVZDEV ];then
        echo "$SRCVZDEV is mounted as expected";
else
        if [ $VZDEV == $BAKVZDEV ];then
                echo "Error $BAKVZDEV is mounted on /vz";
		exit 1;
		#switch
                #BAKVZDEV="$SRCVZDEV";
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

#debug only
#exit 0;

echo ""
echo "rsync vz partition..."
/usr/bin/rsync -avxH --delete /vz/* /mnt/vz

echo ""
echo "rsync root partition..."
#make sure /boot is a diff partition we don't want to clobber "/boot/grub/grub.conf"
#just to be safe we exclude anyway
/usr/bin/rsync -avxH --delete --exclude "/mnt/root/" --exclude "/mnt/vz/" --exclude "/vz" --exclude "/proc/" --exclude "/root/.ccache/" --exclude "/sys" --exclude "/dev" --exclude "/tmp" --exclude "/boot/grub/grub.conf" /* /mnt/root

echo ""
echo "un-mounting..."
/bin/umount /mnt/root
/bin/umount /mnt/vz

echo "rm temp /mnt dirs..."
/bin/rmdir /mnt/root
/bin/rmdir /mnt/vz

echo "done"
exit 0;

