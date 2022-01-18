#!/bin/bash

if [ "$1" == "" ] ; then
	echo "Usage: mkandrun <name_without_extension>"
	exit 1
fi

PRG=$1

java -cp ../lljvm.jar:. $PRG $@
