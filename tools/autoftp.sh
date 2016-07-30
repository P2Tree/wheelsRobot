#!/bin/sh

target='robot'

#make

cd ./bin/

/usr/bin/ftp -n << !
open 192.168.1.32
user root root
passive on
cd /root/
put ${target}
bye
!

