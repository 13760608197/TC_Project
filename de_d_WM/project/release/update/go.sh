#!/bin/sh

if [ -e "nand1-2/lib_romfs.bin" ] ;then
	echo "umount /lib"
	umount /lib
	echo "sync"
	/bin/sync
	echo "mv lib_romfs.bin lib_romfs.bin_bak"
	mv /mnt/nand1-2/lib_romfs.bin /mnt/nand1-2/lib_romfs.bin_bak
fi

cp -pR nand1-2/* /mnt/nand1-2

cp -pR nand1-1/* /mnt/nand1-1

sync
cd /mnt/nand1-2
echo "will reboot..."
sleep 2
./wdt_demo
