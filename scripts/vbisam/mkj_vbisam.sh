#!/bin/bash

VBISAM_ROOT="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

LLJVM_CC=$HOME/lljvm-cc/lljvm-cc/lljvm-cc
JASMIN_PATH=$HOME/lljvm-cc/tools/jasmin.jar

CLANG_INCLUDE_DIR=$(llvm-config --prefix)/lib/clang/$(llvm-config --version)/include
LLJVM_LIBC_INCLUDE_DIRS="-I$HOME/lljvm-cc/runtime/libc/include -I$HOME/lljvm-cc/runtime/libc/include/compiler -I$CLANG_INCLUDE_DIR -I$HOME/lljvm-cc/runtime/libc/sys/linux/include"
CFLAGS_STD="-nostdinc -O0 -pipe -finline-functions -fsigned-char $LLJVM_LIBC_INCLUDE_DIRS -I. -I.. -DHAVE_CONFIG_H -D__GLIBC_USE\(...\)=0"
EXTRA_LLJVM_FLAGS=

rm -f $(find . -name "*.bc") $(find . -name "*.ll") $(find . -name "*.j") $(find . -name "*.jar") $(find . -name "*.class")

cd $VBISAM_ROOT/libvbisam
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c isaudit.c -o isaudit.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c isbuild.c -o isbuild.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c ischeck.c -o ischeck.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c isdecimal.c -o isdecimal.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c isdelete.c -o isdelete.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c ishelper.c -o ishelper.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c isopen.c -o isopen.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c isread.c -o isread.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c isrecover.c -o isrecover.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c isrewrite.c -o isrewrite.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c istrans.c -o istrans.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c iswrite.c -o iswrite.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c vbdataio.c -o vbdataio.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c vbindexio.c -o vbindexio.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c vbkeysio.c -o vbkeysio.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c vblocking.c -o vblocking.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c vblowlevel.c -o vblowlevel.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c vbmemio.c -o vbmemio.bc || exit 1 
CFLAGS="$CFLAGS_STD -DOPERATION_fib_table" ${LLJVM_CC} -v $EXTRA_LLJVM_FLAGS -c vbnodememio.c -o vbnodememio.bc || exit 1 

${LLJVM_CC} -v -m -C lib.vbisam -o $VBISAM_ROOT/lib.vbisam.bc isaudit.bc isbuild.bc ischeck.bc isdecimal.bc isdelete.bc ishelper.bc \
									isopen.bc isread.bc isrecover.bc isrewrite.bc istrans.bc iswrite.bc vbdataio.bc vbindexio.bc \
									vbkeysio.bc vblocking.bc vblowlevel.bc vbmemio.bc vbnodememio.bc 

cd $VBISAM_ROOT

echo "${LLJVM_CC} -C lib.vbisam -j -o lib.vbisam-prelink.j lib.vbisam.bc"
${LLJVM_CC} -C lib.vbisam -j -o lib.vbisam-prelink.j lib.vbisam.bc || exit 1

echo "${LLJVM_CC} -v -a lib.vbisam-prelink.j -L$HOME/lljvm-cc -l$HOME/lljvm-cc/lljvm.jar -C lib.vbisam -o lib.vbisam.j"
${LLJVM_CC} -v -a lib.vbisam-prelink.j -L$HOME/lljvm-cc -l$HOME/lljvm-cc/lljvm.jar -C lib.vbisam -o lib.vbisam.j || exit 1

rm -fr ./jout || exit 1
mkdir ./jount || exit 1

echo "java -jar $HOME/lljvm-cc/tools/jasmin.jar -d ./jout lib.vbisam.j"
java -jar $HOME/lljvm-cc/tools/jasmin.jar -d ./jout lib.vbisam.j || exit 1

jar -cf lib.vbisam.jar -C jout/ .  || exit 1