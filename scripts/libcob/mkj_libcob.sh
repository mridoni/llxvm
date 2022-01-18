#!/bin/bash

LIBCOB_ROOT="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

LLJVM_CC=$HOME/lljvm-cc/lljvm-cc/lljvm-cc
JASMIN_PATH=$HOME/lljvm-cc/tools/jasmin.jar

LLVM_JAR_PATH=$HOME/lljvm-cc/lljvm.jar
GMP_JAR_PATH=$HOME/gmp-6.2.1/lib.gmp.jar
LIBCOB_JAR_PATH=$HOME/gnucobol-3.1.2/libcob/lib.gnucobol.jar


LLJVM_LIBC_INCLUDE_DIRS="-I$HOME/lljvm-cc/runtime/libc/include -I$HOME/lljvm-cc/runtime/libc/include/compiler -I$HOME/lljvm-cc/runtime/libc/sys/linux/include"
GMP_FLAGS="-I$HOME/gmp-6.2.1 -I$HOME/gmp-6.2.1/include"
CFLAGS_STD="-g -fno-discard-value-names -nostdinc $LLJVM_LIBC_INCLUDE_DIRS -DHAVE_CONFIG_H -I. -I..  -I.. -I../lib -I../lib  -DLOCALEDIR=\"/opt/gnucobol-3.1.2/share/locale\" $GMP_FLAGS -O0 -pipe -finline-functions -fsigned-char -Wall -Wwrite-strings -Wmissing-prototypes -Wno-format-y2k -D__GLIBC_USE\(...\)=0"
EXTRA_LLJVM_FLAGS="-T"

pushd $LIBCOB_ROOT > /dev/null 2>&1

rm -f *.bc *.ll *.jar *.j

for F in common move numeric strings fileio call intrinsic termio screenio reportio cobgetopt  mlio ; do
# for F in numeric ; do
	CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c -o $F.bc $F.c
done

rm -fr jout
mkdir jout

for F in common move numeric strings fileio call intrinsic termio screenio reportio cobgetopt mlio ; do
# for F in numeric ; do
	echo ${LLJVM_CC} -j $EXTRA_LLJVM_FLAGS -B -C lib.gnucobol.$F -o lib.gnucobol.$F-prelink.j $F.ll
	${LLJVM_CC} -j $EXTRA_LLJVM_FLAGS -B -C lib.gnucobol.$F -o lib.gnucobol.$F-prelink.j $F.ll
	if [ "$?" != "0" ] ; then echo "Build error" ; exit 1 ; fi
done

J_LINK_OPTS=
for F in $(ls -1 *-prelink.j) ; do
	J_LINK_OPTS="$J_LINK_OPTS -l $F "
done

for F in common move numeric strings fileio call intrinsic termio screenio reportio cobgetopt mlio ; do
# for F in numeric ; do
	echo ${LLJVM_CC} -a -v $EXTRA_LLJVM_FLAGS -l$LLVM_JAR_PATH -l$GMP_JAR_PATH $J_LINK_OPTS -C lib.gnucobol.$F -o lib.gnucobol.$F.j lib.gnucobol.$F-prelink.j 
	${LLJVM_CC} -a -v -l$LLVM_JAR_PATH $EXTRA_LLJVM_FLAGS -l$GMP_JAR_PATH $J_LINK_OPTS -C lib.gnucobol.$F -o lib.gnucobol.$F.j lib.gnucobol.$F-prelink.j 
	RC=$?
	if [ "$RC" != "0" ] ; then echo "Build error ($RC)" ; exit 1 ; fi
done

for F in common move numeric strings fileio call intrinsic termio screenio reportio cobgetopt  mlio ; do
# for F in numeric ; do
	echo ${LLJVM_CC} -k $EXTRA_LLJVM_FLAGS -v -O ./jout lib.gnucobol.$F.j
	${LLJVM_CC} -k $EXTRA_LLJVM_FLAGS -v -O ./jout lib.gnucobol.$F.j
	if [ "$?" != "0" ] ; then echo "Build error" ; exit 1 ; fi
done

jar -cf lib.gnucobol.jar -C jout/ .
if [ "$?" != "0" ] ; then echo "Build error" ; exit 1 ; fi

popd > /dev/null 2>&1