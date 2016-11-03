#!/bin/bash

#FILE
#	rebootContainerPhones.sh	
#	$Id: PostDNSNode.sh 2294 2013-04-18 19:12:29Z Colin $
#PURPOSE
#	In a container external migration.
#	Waits for dns update and then try to reboot every online phone
#	Will also check success on remote node making sure all previous online extensions are online.
#REQUIRES
#OS
#	Only tested on CentOS 5+ with OpenVZ
#LEGAL
#	Copyright 2013 Ricardo A. Armas, Unixservice, LLC., Voice Carrier, LLC.
#	GPLv2 license applies.
#

if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ] || [ "$4" == "" ];then 
    echo "usage: $0 <uContainerVId> <cHostName> <cDestinationIPv4> <cDestinationNode>"
    exit
fi       

#Variable definition

vid=$1
fqdn=$2
ip=$3
destNode=$4
destVid=$1
ready="no"
errorEmail="rosario@voicecarrier.com"
emailSubject="[rebootContainerPhones] Error "
emailBody="Failed extensions:\n "
#echo $emailBody | /bin/mail -s "$emailSubject" "$errorEmail" 


#Verify dns update.

for i in {1..30}
do
    if [[ ` dig @ns1.callingcloud.net $fqdn +short ` == $ip ]]; then
    	if [[ ` dig @ns2.callingcloud.net $fqdn +short ` == $ip ]]; then
	    if [[ ` dig @ns3.callingcloud.net $fqdn +short ` == $ip ]]; then
                if [[ ` dig @8.8.8.8 $fqdn +short ` == $ip ]]; then
	    	    ready="yes"
		    break
		fi
	    fi
	fi
    fi
    sleep 60 
    echo $i
done

if [[ $ready == "yes" ]]; then
    #Check if container have yealink sip notify configuration installed, if not install it.
    /usr/sbin/vzctl exec2 $vid "grep 'yealink-check-cfg' /etc/asterisk/sip_notify.conf"
    if [[ $? != 0 ]]; then
	/usr/sbin/vzctl exec2 $vid "echo '
    	; Restart yealink devices
    	[yealink-check-cfg]
    	Event=>check-sync\;reboot=true
    	Content-Length=>0' >> /etc/asterisk/sip_notify.conf"
    	/usr/sbin/vzctl exec2 $vid "service asterisk reload"
    fi
#Get online extensions and their useragent.
#Send sip notify according to useragent.
    extensions=` /usr/sbin/vzctl exec2 $vid 'asterisk -rx "sip show peers"|grep OK|cut  -f1 -d/' `
    for e in $extensions
    do
	useragents[$e]=` /usr/sbin/vzctl exec2 $vid "asterisk -rx 'sip show peer $e'|grep Useragent" `
	echo $e ${useragents[$e]}
	case ${useragents[$e]} in
	    *Yealink*) /usr/sbin/vzctl exec2 $vid "asterisk -rx 'sip notify yealink-check-cfg $e'";;
	esac
    done
#Wait for telephones reboot.
    sleep 100
    failedExts=0   
#Check that previous online extensions are online in new container, if their not log an error.
    for e in $extensions
    do
	ssh $destNode vzctl exec2 $destVid 'service asterisk reload'
	ssh $destNode vzctl exec2 $destVid 'asterisk -rx \"sip show peer '$e'\"' | grep Status |grep OK	
	if [[ $? != 0 ]]; then
	    error+="$e ${useragents[$e]} failed.\n"
	    let failedExts+=1
	fi
    done
    if [[ $failedExts != 0 ]]; then
	emailBody+=$error
	echo -e $emailBody
    	emailSubject+="$fqdn"
    	echo -e $emailBody | /bin/mail -s "$emailSubject" "$errorEmail"
    else
        emailbody="Ok."
	emailSubject="[rebootContainerPhones] Success $fqdn"
	echo -e $emailBody | /bin/mail -s "$emailSubject" "$errorEmail"
    fi
    exit $failedExts
    #vzctl exec2 $vid 'for a in `asterisk -rx "sip show peers"|grep OK|cut  -f1 -d/` ;do echo $a && asterisk -rx "sip notify yealink-check-cfg $a"; done; '
else
   echo $fqdn "DNS update failed."
   exit 255
fi

