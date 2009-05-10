#FILE
#	script-include.sh
#PURPOSE
#	Provide bash scripts common functions
#AUTHOR
#	(C) 2008-2009 Hugo Urquiza for Unixservice.
#

function BashInput
{
        #This function will return the input entered by the user
        #It takes two params: cPrompt and cDefault
        #Usage:
        #BashInput "Enter your Name" "John Doe"`;
        #Function return value is given via the $cBashInput global

        cPrompt=$1;
        cDefault=$2;
        cReturn=$3
        echo -n "$cPrompt [$cDefault]: ";
        read cInput;

        if [ "$cInput" = "" ]; then
                cBashInput=$cDefault;
        else
                cBashInput=$cInput;
        fi
}

