#!/bin/sh

# Config
ISPTOOL="avrispmkII"
ISPPORT="usb"
ISPSPEED="-b 115200"
# End Config

if [[ $# == 0 ]];
then
	echo "Usage: $0 hexfile [name]"
	echo "  If 'name' is specified, the atmega will be erased and the fuses set appropriately."
	echo "  'name' will be written to the EEPROM in the appropriate location for the bootloader,"
	echo "  and then normal flashing of the bootloader will commence."
	echo
	echo "  Note: 'name' must be at most 8 characters, and will be made upper-case."
	exit
fi

BOARD=`basename $1 | cut -d - -f 2`
MCU_TARGET=`basename $1 | cut -d - -f 3`

case "${MCU_TARGET}" in
"atmega168" )
EFUSE=00
HFUSE=DD
LFUSE=FF
let EEPROM_SIZE=512
;;
"atmega328p")
EFUSE=05
HFUSE=D2
LFUSE=FF
let EEPROM_SIZE=1024
;;
"atmega1280")
EFUSE=F5
HFUSE=D8
LFUSE=FF
let EEPROM_SIZE=4096
;;
esac

function checksum {
	let SUM="$(echo -n $1 | sed -e 's/.\{2\}/0x& + /g' -e 's/ + $//')"
	CHECKSUM=$(printf "%x" "-${SUM}" | tail -c 2)
}

# Notify user of pending erase
res=""
while [[ "$res" != "y" && "$res" != "n" ]]
do
	echo -n "Warning: This will perform a chip erase of the flash and eeprom. Continue? (y/n): "
	read res
	echo
done

if [[ $res == 'n' ]]; then exit 1; fi

if [[ $# == 2 ]];
then
	NAME=$(echo -n $2 | tr '[:lower:]' '[:upper:]' | rev)
	let NAMElen=$(echo -n $NAME | wc -c)

	# Check name constraints
	if [[ $NAMElen > 8 ]];
	then
		echo "'$NAME' is too long a name! Name must be at most 8 characters."
		exit 1
	fi

	# Create EEPROM image
	let START="$EEPROM_SIZE - $NAMElen - 1"
	ROMF=$(mktemp)
	BODY=$(printf "%02x" $(( NAMElen + 1 )) )$(printf "%04x" $START)00FF$(echo -n $NAME | od -A n -t x1 | tr -d ' ')
	checksum $BODY
	cat <<EOF > $ROMF
:$(echo -n $BODY | tr '[:lower:]' '[:upper:]')$(echo -n $CHECKSUM | tr '[:lower:]' '[:upper:]')
:00000001FF
EOF

	avrdude -c ${ISPTOOL} -p ${MCU_TARGET} -P ${ISPPORT} ${ISPSPEED} -u -U efuse:w:0x${EFUSE}:m -U hfuse:w:0x${HFUSE}:m -U lfuse:w:0x${LFUSE}:m
	if [[ $? != 0 ]];
	then
		exit 1
	fi
	sleep 2
fi

avrdude -c ${ISPTOOL} -p ${MCU_TARGET} -P ${ISPPORT} ${ISPSPEED} -U lock:w:0x3f:m -U flash:w:$1 -U lock:w:0x0f:m
RES=$?

# Write EEPROM after since flashing causes a chip-erase to occur
if [[ $# == 2 ]];
then
	if [[ $RES != 0 ]];
	then
		exit 1
	fi
	sleep 2
	avrdude -c ${ISPTOOL} -p ${MCU_TARGET} -P ${ISPPORT} ${ISPSPEED} -U eeprom:w:$ROMF:i
	rm $ROMF
fi
