#!/bin/bash
#
#Simple automation of the building of unxsVZ normal packages
#Notes 
#	The spec file info has to reflect the version and release provided.
#	For unxsVZ build just place unxsVZ inside a dir also called unxsVZ
#	Everything has to be in the format required herein.
#	For scp/ssh options remote account must have sudo setup for running
#	/usr/sbin/unxsrpm-install.sh. The port is required even if 22.
#
#	You must export UNXSVZ=/home/joe/unxsVZ or
#	export UNXSVZ=/home/joe/unxsVZ/unxsVZ for unxsVZ rpm.
#	For unxsVZ run this script for the first level unxsVZ dir
#
#	We will soon fix this rpm beginners mess. And setup a correct build root envirnoment.
#

function NormalExit {
	echo "NormalExit $1";
	pwd;
	if [ "$1" == "unxsVZ" ]; then
		cd $UNXSVZ/..;
		mv -i unxsAdmin unxsVZ/unxsAdmin;
		mv -i unxsApache unxsVZ/unxsApache;
		mv -i unxsBind unxsVZ/unxsBind;
		mv -i unxsCIDRLib unxsVZ/unxsCIDRLib;
		mv -i unxsBlog unxsVZ/unxsBlog;
		mv -i unxsISP unxsVZ/unxsISP;
		mv -i unxsMail unxsVZ/unxsMail;
		mv -i unxsRadius unxsVZ/unxsRadius;
		mv -i unxsRadiusLib unxsVZ/unxsRadiusLib;
		mv -i unxsSMS unxsVZ/unxsSMS;
		mv -i unxsTemplateLib unxsVZ/unxsTemplateLib;
	fi
	exit 0;
}

function ErrorExit {
	echo "ErrorExit $1";
	if [ "$1" == "unxsVZ" ]; then
		#cd $UNXSVZ;
		mv -i ./unxsAdmin unxsVZ/unxsAdmin;
		mv -i ./unxsApache unxsVZ/unxsApache;
		mv -i ./unxsBind unxsVZ/unxsBind;
		mv -i ./unxsCIDRLib unxsVZ/unxsCIDRLib;
		mv -i ./unxsBlog unxsVZ/unxsBlog;
		mv -i ./unxsISP unxsVZ/unxsISP;
		mv -i ./unxsMail unxsVZ/unxsMail;
		mv -i ./unxsRadius unxsVZ/unxsRadius;
		mv -i ./unxsRadiusLib unxsVZ/unxsRadiusLib;
		mv -i ./unxsSMS unxsVZ/unxsSMS;
		mv -i ./unxsTemplateLib unxsVZ/unxsTemplateLib;
	fi
	exit 0;
}

if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ];then
	echo "usage $0 <unxsVZ subsystem> <version 1.0> <rpm release 1> [<scp/ssh port> <scp/ssh login@server>]";
	exit 0;
fi
if [ "$UNXSVZ" == "" ];then
	echo "This simple script requires that the unxsVZ source tree be set via UNXSVZ env var";
	echo "export UNXSVZ=/home/joe/unxsVZ/unxsVZ for example";
	exit 0;
fi
if [ ! -d $UNXSVZ ];then
	echo "This simple script requires that the unxsVZ source tree be set via UNXSVZ env var";
	echo "export UNXSVZ=/home/joe/unxsVZ for example. If creating unxsVZ rpm see this scripts header.";
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

if [ $? != 0 ];then
	echo error 1;
	exit 1;
fi


#Special case need to move stuff out of our giant unxsVZ dir
if [ "$1" == "unxsVZ" ]; then

	if [ ! -d ./unxsVZ ];then
		echo "unxsVZ build must be from a nested unxsVZ dir and one level up";
		exit 1;
	fi

	mv -i unxsVZ/unxsAdmin ./;
	mv -i unxsVZ/unxsApache ./;
	mv -i unxsVZ/unxsBind ./;
	mv -i unxsVZ/unxsCIDRLib ./;
	mv -i unxsVZ/unxsBlog ./;
	mv -i unxsVZ/unxsISP ./;
	mv -i unxsVZ/unxsMail ./;
	mv -i unxsVZ/unxsRadius ./;
	mv -i unxsVZ/unxsRadiusLib ./;
	mv -i unxsVZ/unxsSMS ./;
	mv -i unxsVZ/unxsTemplateLib ./;
else
	cd $UNXSVZ;
fi


if [ ! -d ./$1 ];then
	echo "No such subsystem $1 !";
	ErrorExit $1;
fi

if [ ! -f ./$1/$LCNAME.spec ];then
	echo "Your updated spec file should be here $1/$LCNAME.spec ";
	ErrorExit $1;
fi

SPECVER=`grep -w "Version: ..." ./$1/$LCNAME.spec`;
grep -w "Version: $2" ./$1/$LCNAME.spec > /dev/null 2>&1;
if [ $? != 0 ];then
	echo "Your spec file seems to have the wrong version number $SPECVER, you specify $2";
	ErrorExit $1;
fi

SPECREL=`grep -w "Release: ." ./$1/$LCNAME.spec`;
grep -w "Release: $3" ./$1/$LCNAME.spec > /dev/null 2>&1;
if [ $? != 0 ];then
	echo "Your spec file seems to have the wrong release number $SPECREL, you specify $3";
	ErrorExit $1;
fi

SPECTAR=`grep "$LCNAME-.....tar.gz" ./$1/$LCNAME.spec`;
grep "$LCNAME-$2.tar.gz" ./$1/$LCNAME.spec > /dev/null 2>&1;
if [ $? != 0 ];then
	echo "Your spec file seems to have the wrong Source line, it should be something"
	echo " like Source: http://unixservice.com/source/$LCNAME-$2.tar.gz";
	ErrorExit $1;
fi

#rpm-1.19
LOCALVER=`grep "rpm-...." ./$1/local.h.default`;
grep "rpm-$2" ./$1/local.h.default > /dev/null 2>&1;
if [ $? != 0 ];then
	echo "Your local.h.default file seems to have the wrong line(s):"
	echo "$LOCALVER"
	echo "You should change the last part to rpm-$2";
	ErrorExit $1;
fi

#Checks are done at this stage.
#debug only
#echo "Basic checks done: $1 $2 $3 $LCNAME $SPECVER $SPECREL $SPECTAR $LOCALVER";
#exit 0;

#Saw some *.o files appear at some rpm stage. Check this out.
make clean;


tar czvf $1.tar.gz $1/ --exclude .svn --exclude *.o
if [ $? != 0 ];then
	echo error 2;
	ErrorExit $1;
fi

mv $1.tar.gz $UNXSVZ/../rpm-staging/
if [ $? != 0 ];then
	echo error 3;
	ErrorExit $1;
fi

cd $UNXSVZ/../rpm-staging
if [ $? != 0 ];then
	echo error 4;
	ErrorExit $1;
fi

tar xzf $1.tar.gz
if [ $? != 0 ];then
	echo error 5;
	ErrorExit $1;
fi

rm $1.tar.gz
if [ $? != 0 ];then
	echo error 6;
	ErrorExit $1;
fi

mv $1 $LCNAME-$2
if [ $? != 0 ];then
	echo error 7;
	ErrorExit $1;
fi

tar czvf $LCNAME-$2.tar.gz $LCNAME-$2/
if [ $? != 0 ];then
	echo error 8;
	ErrorExit $1;
fi

cp $LCNAME-$2.tar.gz /usr/src/redhat/SOURCES
if [ $? != 0 ];then
	echo error 9;
	ErrorExit $1;
fi

cp $LCNAME-$2/$LCNAME.spec /usr/src/redhat/SPECS/$LCNAME-$2-$3.spec
if [ $? != 0 ];then
	echo error 10;
	ErrorExit $1;
fi

rm -r $LCNAME-$2
if [ $? != 0 ];then
	echo error 11;
	ErrorExit $1;
fi

cd /usr/src/redhat/SPECS
if [ $? != 0 ];then
	echo error 12;
	ErrorExit $1;
fi

rpmbuild -ba $LCNAME-$2-$3.spec
if [ $? != 0 ];then
	echo error 13;
	ErrorExit $1;
fi

cd $UNXSVZ/../rpm-staging;
if [ $? != 0 ];then
	echo error 14;
	ErrorExit $1;
fi

if [ "$4" == "" ];then
	echo "You should now sftp the tar.gz, the src.rpm and the binary rpm to your release server for install.";
else 
	if [ "$5" != "" ];then
		/usr/bin/scp -P $4 /usr/src/redhat/SOURCES/$LCNAME-$2.tar.gz $5:;
		/usr/bin/scp -P $4  /usr/src/redhat/SRPMS/$LCNAME-$2-$3.src.rpm $5:;
		/usr/bin/scp -P $4  /usr/src/redhat/RPMS/i386/$LCNAME-$2-$3.i386.rpm $5:;
		if [ -f /usr/src/redhat/RPMS/x86_64/$LCNAME-$2-$3.x86_64.rpm ];then
			/usr/bin/scp -P $4  /usr/src/redhat/RPMS/x86_64/$LCNAME-$2-$3.x86_64.rpm $5:;
		fi
		/usr/bin/ssh -p $4 $5 "sudo /usr/sbin/unxsrpm-install.sh $LCNAME-$2-$3 $LCNAME-$2";
	else
		echo "You specified only one of the required scp args.";
	fi
fi


NormalExit $1;
