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

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

if [ "$1" == "" ];then
        echo "usage: $0 run";
        exit;
fi

cHostname=`/bin/hostname -f`;
if [ $? != 0 ];then
        fLog "cHostname error";
        exit 1;
fi

cFile=`echo "$cHostname$cSharedSecret" | /usr/bin/md5sum | /bin/cut -f 1 -d " "`;
if [ $? != 0 ];then
        fLog "cFile error";
        exit 1;
fi

#ignore # commented lines
cResponse=`/usr/bin/wget --quiet --no-check-certificate --output-document=-\
         https://$cMCSServerFQDN:$cMCSServerPort/$cFile | grep -v "^\#"`;
if [ $? != 0 ];then
        exit 0;
fi

#run
echo $cResponse | grep -w "agentVoip=run;" > /dev/null 2>&1;
if [ $? == 0 ];then
        #fLog "attempt run";
        ps -ef | grep -v grep | grep -w "rta.jar" > /dev/null 2>&1;
        #if [ $? == 0 ];then
        #       fLog "already running";
        #else
        if [ $? != 0 ];then
                fLog "start rta";
                $cMCSInstallPath/start_rta.sh > /dev/null 2>&1;
                if [ $? != 0 ];then
                        fLog "$cMCSInstallPath/start_rta.sh error";
                        exit 1;
                fi
                exit 0;
        fi
fi

#stop
echo $cResponse | grep -w "agentVoip=stop;" > /dev/null 2>&1;
if [ $? == 0 ];then
        #fLog "attempt stop";
        ps -ef | grep -v grep | grep -w "rta.jar" > /dev/null 2>&1;
        #if [ $? != 0 ];then
        #       fLog "not running";
        #else
        if [ $? == 0 ];then
                fLog "stop rta";
                $cMCSInstallPath/stop_rta.sh > /dev/null 2>&1;
                if [ $? != 0 ];then
                        fLog "$cMCSInstallPath/stop_rta.sh error";
                        exit 1;
                fi
                exit 0;
        fi
fi

exit 0;
