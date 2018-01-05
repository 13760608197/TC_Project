#!/bin/sh

cnt=0

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

while true
do
	top -b -n 1 | grep v_$TYPE | awk '{print $7}' | grep % | awk -F % 'NR==1{print $1}' > cpu.txt
	result=`cat cpu.txt`
	if [ $result -gt 85 ]; then
		break
		if [ "$cnt" -lt 5 ]; then
			cnt=`expr $cnt + 1`
			echo "cnt = $cnt"
		else
			break
		fi
	else
		cnt=0
	fi

	touch /mnt/nand1-1/check_nand
	if [ $? -ne 0 ] 
	then
		echo check nand1-1 fail
		mount -o remount,rw /dev/sda1
	fi
	rm -rf /mnt/nand1-1/check_nand

	touch /mnt/nand1-2/check_nand
	if [ $? -ne 0 ] 
	then
		echo check nand1-2 fail
		mount -o remount,rw /dev/sda5
	fi
	rm -rf /mnt/nand1-2/check_nand

	sleep 10
done

reboot		

