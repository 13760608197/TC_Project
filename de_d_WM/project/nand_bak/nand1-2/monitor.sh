#!/bin/sh

TYPE=`cat type.txt |awk -F" " '{print $1}'`

result=`echo $TYPE |grep 'WM' |wc -l`
if [ $result = 0 ]; then
	result=`echo $TYPE |grep 'FL' |wc -l`
	if [ $result = 0 ]; then
		TYPE=BZ
	else
		TYPE=FL
	fi
else
	TYPE=WM
fi

/mnt/nand1-2/v_$TYPE
ps -fe|grep \/mnt\/nand1-2\/v_$TYPE |grep -v_$TYPE grep

if [ $? -ne 0 ] 
then
	if [ -e "Update.cab" ] ;then
		echo "updating !!!"
		if [ -d update ] ;then
			rm -rf update
		fi
		echo "mv Update.cab update.tar.gz"
		mv Update.cab update.tar.gz
		echo "tar -xzvf update.tar.gz"
		tar -xzvf update.tar.gz
		if [ -d update ] ;then
			rm update.tar.gz
			cd update
			if [ -e "go.sh" ] ;then
				./go.sh
			else
				echo "the go.sh is not exist !!!,pleas download the new update file again!"
			fi
		else
			echo "update.tar.gz fail"
		fi
	else	
		echo "Update.cab is not exist"
		echo "will run watchdog..."
#		reboot
		sync
		./wdt_demo		
	fi
fi

#sleep 3
#done & 
