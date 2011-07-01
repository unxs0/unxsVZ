#!/bin/bash

#FILE
#       tools/mcs/getcommand.sh
#PURPOSE
#       Poll remote http hash based file for
#       control of local mcs remote agent



cSharedSecret="topsecret";
cMCSServerFQDN="mcs.example.com";
cMCSServerPort="80";
cMCSInstallPath="/usr/local/mcsremoteagent";

if [ "$1" == "" ];then
	echo "usage: $0 run";
	exit;
fi

cHostname=`/bin/hostname -f`;
if [ $? != 0 ];then
        echo "cHostname error";
        exit 1;
fi

cFile=`echo "$cHostname$cSharedSecret" | /usr/bin/md5sum | /bin/cut -f 1 -d " "`;
if [ $? != 0 ];then
        echo "cFile error";
        exit 1;
fi

cResponse=`/usr/bin/wget --quiet --no-check-certificate --output-document=-\
         https://$cMCSServerFQDN:$cMCSServerPort/$cFile`;
if [ $? != 0 ];then
        echo "wget error 0";
        exit 1;
fi

#run
echo $cResponse | grep -w "agentVoip=run;" > /dev/null 2>&1;
if [ $? == 0 ];then
        echo "attempt run";
        ps -ef | grep -v grep | grep -w "rta.jar" > /dev/null 2>&1;
        if [ $? == 0 ];then
                echo "already running";
        else
                echo "start rta";
                $cMCSInstallPath/start_rta.sh > /dev/null 2>&1;
                if [ $? != 0 ];then
                        echo "$cMCSInstallPath/start_rta.sh error";
                        exit 1;
                fi
        fi
fi

#stop
echo $cResponse | grep -w "agentVoip=stop;" > /dev/null 2>&1;
if [ $? == 0 ];then
        echo "attempt stop";
        ps -ef | grep -v grep | grep -w "rta.jar" > /dev/null 2>&1;
        if [ $? != 0 ];then
                echo "not running";
        else
                echo "stop rta";
                $cMCSInstallPath/stop_rta.sh > /dev/null 2>&1;
                if [ $? != 0 ];then
                        echo "$cMCSInstallPath/stop_rta.sh error";
                        exit 1;
                fi
        fi
fi

exit 0;
