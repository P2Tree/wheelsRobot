#!/bin/sh

target='robot'

#make

cd ./bin/

/usr/bin/ftp -n << !
open 192.168.2.1
user root fonda199287
passive on
cd /home/
put ${target}
bye
!

