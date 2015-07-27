#!/bin/sh

ISPTOOL="avrispmkII"
ISPPORT="usb"
ISPSPEED="-b 115200"


BOARD=`basename $1 | cut -d - -f 2`
MCU_TARGET=`basename $1 | cut -d - -f 3`

case "${MCU_TARGET}" in


"atmega168" )
EFUSE=00
HFUSE=DD
LFUSE=FF
;;
"atmega328p")
EFUSE=05
HFUSE=D2
LFUSE=FF
;;
"atmega1280")
EFUSE=F5
HFUSE=DA
LFUSE=FF
;;
esac


#avrdude -c ${ISPTOOL} -p ${MCU_TARGET} -P ${ISPPORT} ${ISPSPEED} -e -u -U efuse:w:0x${EFUSE}:m -U hfuse:w:0x${HFUSE}:m -U lfuse:w:0x${LFUSE}:m
#if [[ $? != 0 ]];
#then
#exit 1
#fi
#sleep 3
avrdude -c ${ISPTOOL} -p ${MCU_TARGET} -P ${ISPPORT} ${ISPSPEED} -U lock:w:0x3f:m -U flash:w:$1 -U lock:w:0x0f:m
