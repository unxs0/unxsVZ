#!/bin/bash
#
#Simple automation of the building of unxsVZ normal packages
#Note that the spec file info has to reflect the version and release provided.
#

if [ "$1" == "" ];then
	echo "usage $0 <unxsVZ subsystem name ex. unxsBind> <version ex. 1.0> <rpm release ex. 1>";
	exit 0;
fi
if [ "$2" == "" ];then
	echo "usage $0 <unxsVZ subsystem name ex. unxsBind> <version ex. 1.0> <rpm release ex. 1>";
	exit 0;
fi
if [ "$3" == "" ];then
	echo "usage $0 <unxsVZ subsystem name ex. unxsBind> <version ex. 1.0> <rpm release ex. 1>";
	exit 0;
fi
if [ "$UNXSVZ" == "" ];then
	echo "This simple script requires that the unxsVZ source tree be set via UNXSVZ env var";
	echo "export UNXSVZ=/home/joe/unxsVZ for example";
	exit 0;
fi
if [ ! -d $UNXSVZ ];then
	echo "This simple script requires that the unxsVZ source tree be set via UNXSVZ env var";
	echo "export UNXSVZ=/home/joe/unxsVZ for example";
	exit 0;
fi
if [ ! -d $UNXSVZ/../rpm-staging ];then
	echo "This simple script requires that a dir called rpm-staging be on level up from $UNXSVZ";
	echo "Your rpm-staging dir should be here $UNXSVZ/../rpm-staging";
	exit 0;
fi

LCNAME=`echo $1 | tr "[:upper:]" "[:lower:]"`;

#debug only
#echo "$1 $2 $3 $LCNAME";
#exit 0;

cd $UNXSVZ;
if [ $? != 0 ];then
	echo error 1;
	exit 1;
fi

if [ ! -d ./$1 ];then
	echo "No such subsystem $1 !";
	exit 1;
fi

if [ ! -f ./$1/$LCNAME.spec ];then
	echo "Your updated spec file should be here $1/$LCNAME.spec ";
	exit 1;
fi

SPECVER=`grep -w "Version: ..." ./$1/$LCNAME.spec`;
grep -w "Version: $2" ./$1/$LCNAME.spec > /dev/null 2>&1;
if [ $? != 0 ];then
	echo "Your spec file seems to have the wrong version number $SPECVER, you specify $2";
	exit 1;
fi

SPECREL=`grep -w "Release: ." ./$1/$LCNAME.spec`;
grep -w "Release: $3" ./$1/$LCNAME.spec > /dev/null 2>&1;
if [ $? != 0 ];then
	echo "Your spec file seems to have the wrong release number $SPECREL, you specify $3";
	exit 1;
fi

tar czvf $1.tar.gz $1/ --exclude .svn
if [ $? != 0 ];then
	echo error 2;
	exit 1;
fi

mv $1.tar.gz ../rpm-staging/
if [ $? != 0 ];then
	echo error 3;
	exit 1;
fi

cd ../rpm-staging
if [ $? != 0 ];then
	echo error 4;
	exit 1;
fi

tar xzf $1.tar.gz
if [ $? != 0 ];then
	echo error 5;
	exit 1;
fi

rm $1.tar.gz
if [ $? != 0 ];then
	echo error 6;
	exit 1;
fi

mv $1 $LCNAME-$2
if [ $? != 0 ];then
	echo error 7;
	exit 1;
fi

tar czvf $LCNAME-$2.tar.gz $LCNAME-$2/
if [ $? != 0 ];then
	echo error 8;
	exit 1;
fi

cp $LCNAME-$2.tar.gz /usr/src/redhat/SOURCES
if [ $? != 0 ];then
	echo error 9;
	exit 1;
fi

cp $LCNAME-$2/$LCNAME.spec /usr/src/redhat/SPECS/$LCNAME-$2-$3.spec
if [ $? != 0 ];then
	echo error 10;
	exit 1;
fi

rm -r $LCNAME-$2
if [ $? != 0 ];then
	echo error 11;
	exit 1;
fi

cd /usr/src/redhat/SPECS
if [ $? != 0 ];then
	echo error 12;
	exit 1;
fi

rpmbuild -ba $LCNAME-$2-$3.spec
if [ $? != 0 ];then
	echo error 13;
	exit 1;
fi

cd $UNXSVZ/../rpm-staging;
if [ $? != 0 ];then
	echo error 14;
	exit 1;
fi

echo "You should now sftp the tar.gz, the src.rpm and the binary rpm to your release server for install";
