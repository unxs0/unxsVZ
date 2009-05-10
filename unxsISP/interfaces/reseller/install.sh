#
#FILE
#	install.sh
#PURPOSE
#	Quickly install the ispAdmin interface in a new server
#AUTHOR
#	Hugo Urquiza for Unixservice. (C) 2007
#USAGE
#	./install.sh <siteroot>
#E.g.:
#	./install.sh /u/web/somesite.com
#	
#	Note that the path do not include the htdocs folder!
#

echo "Instalilng ispAdmin.cgi..."
export CGIDIR=$1/cgi-bin
make install

echo "Installing CSS and images..."
cp -R ./templates/images/ $1/htdocs
cp -R ./css $1/htdocs

echo "Installation completed."

