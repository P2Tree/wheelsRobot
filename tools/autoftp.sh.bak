#!/bin/sh

target='robot'
remote_addr='/root/'
remote_ipaddr='192.168.1.32'

ftp_user='root'
ftp_password='root'
#make

cd ./bin/

echo remote ip address is: ${remote_ipaddr}
echo remote directory is: ${remote_addr}
echo target is: ${target}

echo open ftp and login: ${ftp_user} ${ftp_password}.
/usr/bin/ftp -n << !
open ${remote_ipaddr}
user ${ftp_user} ${ftp_password}
passive on
cd ${remote_addr}
put ${target}
get ${target} retrieval.$$
bye
!

if [ -f retrieval.$$ ]
then
    echo ftp of transfer ${target} to ${remote_ipaddr} down.
    rm -f retrieval.$$
else
    echo "ftp of transfer ${target} \033[1;31;5merror\033[0m"
fi


