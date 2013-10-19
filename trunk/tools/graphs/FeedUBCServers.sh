#!/bin/bash

#PURPOSE
#	Move tContainer,tNode and tDatacenter tables to ubcdbN servers and then load it there.
#WHY
#	This makes the distributed UBC app queries much easir to build and much faster.

#magic numbers
#71 X DC
#81 Y DC
#node15 has ubcdb0 that is VEID 50671
#node28 has ubcdb1 that is VEID

#ubcdb0
/usr/bin/mysqldump -ppasswd unxsvz tContainer --where=uDatacenter=71 > /root/tContainer.uDatacenter71.mysqldump;
scp -l 2000 /root/tContainer.uDatacenter71.mysqldump node15:/vz/private/50671/root/tContainer.uDatacenter71.mysqldump;
ssh node15 'vzctl exec 50671 "mysql -ppasswd unxsvz < /root/tContainer.uDatacenter71.mysqldump"';

/usr/bin/mysqldump -ppasswd unxsvz tNode --where=uDatacenter=71 > /root/tNode.uDatacenter71.mysqldump;
scp -l 2000 /root/tNode.uDatacenter71.mysqldump node15:/vz/private/50671/root/tNode.uDatacenter71.mysqldump;
ssh node15 'vzctl exec 50671 "mysql -ppasswd unxsvz < /root/tNode.uDatacenter71.mysqldump"';

/usr/bin/mysqldump -ppasswd unxsvz tDatacenter --where=uDatacenter=71 > /root/tDatacenter.uDatacenter71.mysqldump;
scp -l 2000 /root/tDatacenter.uDatacenter71.mysqldump node15:/vz/private/50671/root/tDatacenter.uDatacenter71.mysqldump;
ssh node15 'vzctl exec 50671 "mysql -ppasswd unxsvz < /root/tDatacenter.uDatacenter71.mysqldump"';


#ubcdb1
/usr/bin/mysqldump -ppasswd unxsvz tContainer --where=uDatacenter=81 > /root/tContainer.uDatacenter81.mysqldump;
scp -l 2000 /root/tContainer.uDatacenter81.mysqldump node28:/vz/private/50711/root/tContainer.uDatacenter81.mysqldump;
ssh node28 'vzctl exec 50711 "mysql -ppasswd unxsvz < /root/tContainer.uDatacenter81.mysqldump"';

/usr/bin/mysqldump -ppasswd unxsvz tNode --where=uDatacenter=81 > /root/tNode.uDatacenter81.mysqldump;
scp -l 2000 /root/tNode.uDatacenter81.mysqldump node28:/vz/private/50711/root/tNode.uDatacenter81.mysqldump;
ssh node28 'vzctl exec 50711 "mysql -ppasswd unxsvz < /root/tNode.uDatacenter81.mysqldump"';

/usr/bin/mysqldump -ppasswd unxsvz tDatacenter --where=uDatacenter=81 > /root/tDatacenter.uDatacenter81.mysqldump;
scp -l 2000 /root/tDatacenter.uDatacenter81.mysqldump node28:/vz/private/50711/root/tDatacenter.uDatacenter81.mysqldump;
ssh node28 'vzctl exec 50711 "mysql -ppasswd unxsvz < /root/tDatacenter.uDatacenter81.mysqldump"';

