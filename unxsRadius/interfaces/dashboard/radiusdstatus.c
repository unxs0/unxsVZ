/*
FILE
	radiusdstatus.c
	svn ID removed
PURPOSE
	Provide an example for use in any of our dashboards that 
	generates an html object to indicate status of radiusd.
	In this simple initial case a table field with a green or red
	background.
AUTHOR/LEGAL
	(C) Gary Wallis for Unixservice, 2007. See accompanying license in top level
	directory.
NOTES
*/

#include <stdio.h>
#include <stdlib.h>
#include <openisp/uradius.h> //Our unxsRadiusLib liburadius

int main()
{
	char cRadiusServer[100]={"127.0.0.1:1812"};

	printf("<table width=10 cellspacing=0 cellpadding=0 border=0><tr><td width=10 bgcolor=");
	if(RadiusAuth("steve","testing123",cRadiusServer,"testing123","")>1)
		printf("red");
	else
		printf("green");
	printf(">&nbsp;</td></tr></table>\n");

	return(0);
}//int main()
