#!/bin/sh

cat interface.ini |awk -F '=' '
{
	if($1 == "LCDLight"){
		print "LCDLight=50"
	} else {
		print $0
	}

} ' > tmp.ini

if [ $? == 0 ] ; then
	echo "mv tmp.ini interface.ini"
	mv tmp.ini interface.ini
	echo "cp interface.ini interface.ini_bak"
	cp interface.ini interface.ini_bak
else
	echo "awk error!"
fi
