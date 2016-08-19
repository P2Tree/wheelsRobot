#!/bin/bash

USERNAME='root'
PASSWORD='root'
HOSTIP='192.168.1.33'
REMOTEDIR='/root'
TARGET='robot'
TARGETDIR='~/forlinx/car/robot/bin/'

lftp -u $USERNAME,$PASSWORD sftp://$HOSTIP <<EOF
cd $REMOTEDIR
put ${TARGETDIR}${TARGET}
bye
EOF

