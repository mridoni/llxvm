include ../../Makefile.inc

.EXPORT_ALL_VARIABLES:

export CFLAGS=-O0 -I./include -I/usr/i686-linux-gnu/include -D"__GLIBC_USE(...)=0" -DMISSING_SYSCALL_NAMES -DHAVE_RENAME -DNO_FLOATING_POINT -DHAVE_MMAP=0 -DINTERNAL_NEWLIB -DDEFINE_MALLOC -DDEFINE_FREE -DDEFINE_REALLOC -DDEFINE_CALLOC -DDEFINE_CFREE -DDEFINE_MEMALIGN -DDEFINE_VALLOC -DDEFINE_PVALLOC -DDEFINE_MALLINFO -DDEFINE_MALLOC_STATS -DDEFINE_MALLOC_USABLE_SIZE -DDEFINE_MALLOPT -D_WANT_IO_LONG_LONG -DINTEGER_ONLY -DSTRING_ONLY

LLXVM_CC=../../llxvm-cc/llxvm-cc

SUBDIRS := argz  ctype  errno  iconv  locale  misc  reent  search  signal  stdio  stdlib  string  time  
BC_OBJS := argz.bc ctype.bc errno.bc iconv.bc locale.bc misc.bc reent.bc search.bc signal.bc stdio.bc stdlib.bc string.bc time.bc 

CLASSNAME := llxvm.lib.c

all: ../java/build/llxvm/lib/c.class

../java/build/llxvm/lib/c.class: libc.j
	@echo "**************** Generating .class file *******************"
	$(LLXVM_CC) -M jvm -K -v -k -C $(CLASSNAME) -O ../java/build $<

libc.j: libc.plj
	rm -f $@
	@echo "**************** J/Linking .j file *******************"
	$(LLXVM_CC) -M jvm -K -v -a -C $(CLASSNAME) -r -L ../java/build -o $@ $<

libc.plj: libc.bc
	@echo "**************** Generating .j file *******************"
	$(LLXVM_CC) -M jvm -K -g 3 -v -j -C $(CLASSNAME) -o $@ $< 
	
libc.bc:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done
	$(LLXVM_CC) -M jvm -K -v -m -C $(CLASSNAME) -o $@ $(BC_OBJS)
	
clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
	rm -f $$(find . -name "*.bc") && rm -f $$(find . -name "*.ll") && rm -f $$(find . -name "*.j") ../java/build/llxvm/lib/c.class
