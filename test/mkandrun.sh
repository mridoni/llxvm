#!/bin/bash

THIS_DIR=$(dirname "$(readlink -f "$0")")
LLJVM_CC=$THIS_DIR/../llxvm-cc/llxvm-cc
export JASMIN_PATH=$THIS_DIR/../tools/jasmin.jar
export CFLAGS=-I$THIS_DIR/../runtime/libc/include/

if [ "$1" == "" ] ; then
	echo "Usage: mkandrun <name_without_extension>"
	exit 1
fi

PRG=$1

rm -f $PRG*.j $PRG*.bc $PRG*.ll $PRG*.class

${LLJVM_CC} -M jvm -v -T -c $PRG.c -o $PRG.bc && \
	${LLJVM_CC} -M jvm -j $PRG.bc -C $PRG  && \
	${LLJVM_CC} -M jvm -a $PRG-prelink.j -L$THIS_DIR/.. -l$THIS_DIR/../llxvm.jar -C $PRG -o $PRG.j  && \
	${LLJVM_CC} -M jvm -k -C $PRG $PRG.j

if [ "$?" != "0" ] ; then
	echo "Build error"
	exit 1
fi

echo "Build ok"

java -cp $THIS_DIR/../llxvm.jar:. $PRG
