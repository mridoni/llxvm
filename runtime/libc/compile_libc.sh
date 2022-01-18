#!/bin/sh

export LLJVM_CC=$HOME/lljvm-cc/lljvm-cc/lljvm-cc
#export CFLAGS="-O0 -I./include -I/usr/i686-linux-gnu/include -D__GLIBC_USE\(...\)=0 -DMISSING_SYSCALL_NAMES -DHAVE_RENAME -DNO_FLOATING_POINT -Xclang -disable-O0-optnone -mno-x87"
export CFLAGS="-O0 -I./include -I/usr/i686-linux-gnu/include -D__GLIBC_USE\(...\)=0 -DMISSING_SYSCALL_NAMES -DHAVE_RENAME -DNO_FLOATING_POINT -DHAVE_MMAP=0 -DINTERNAL_NEWLIB -DDEFINE_MALLOC -DDEFINE_FREE -DDEFINE_REALLOC -DDEFINE_CALLOC -DDEFINE_CFREE -DDEFINE_MEMALIGN -DDEFINE_VALLOC -DDEFINE_PVALLOC -DDEFINE_MALLINFO -DDEFINE_MALLOC_STATS -DDEFINE_MALLOC_USABLE_SIZE -DDEFINE_MALLOPT -D_WANT_IO_LONG_LONG -DINTEGER_ONLY -DSTRING_ONLY"

for d in argz \
    ctype \
    errno \
    iconv \
    locale \
    math \
    misc \
    reent \
    search \
    signal \
    stdio \
    stdlib \
    string \
    time ; do

	for f in $(find ./$d -name "*.c") ; do
		echo "::::::] Compiling $f"
		${LLJVM_CC} -T -v -c -C libc $f
		if [ "$?" != "0" ]; then
			echo "ERROR!"
			exit 1
		fi
	done
done

OBJS=$(find . -name "*.bc")

echo "**************** Merging .bc files *******************"
${LLJVM_CC} -K -v -m -C libc -o libc.bc ${OBJS}
if [ "$?" != "0" ]; then
	echo "ERROR!"
	exit 1
fi
		
echo "**************** Generating .j file *******************"
${LLJVM_CC} -K -v -j -C libc libc.bc
if [ "$?" != "0" ]; then
	echo "ERROR!"
	exit 1
fi
	
echo "**************** J/Linking .j file *******************"
${LLJVM_CC} -K -v -a -C libc libc-prelink.j -r -L ../java/build -o libc.j
if [ "$?" != "0" ]; then
	echo "ERROR!"
	exit 1
fi
	
echo "**************** Generating .class file *******************"
export JASMIN_PATH=../../lljvm-cc/tools/jasmin.jar
${LLJVM_CC} -K -v -k -C libc libc.j
if [ "$?" != "0" ]; then
	echo "ERROR!"
	exit 1
fi
	




