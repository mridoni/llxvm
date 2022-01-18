include ../../../Makefile.inc

.EXPORT_ALL_VARIABLES:

export CFLAGS=-O0 -I../include -I/usr/i686-linux-gnu/include -D"__GLIBC_USE(...)=0" -DMISSING_SYSCALL_NAMES -DHAVE_RENAME -DNO_FLOATING_POINT -DHAVE_MMAP=0 -DINTERNAL_NEWLIB -DDEFINE_MALLOC -DDEFINE_FREE -DDEFINE_REALLOC -DDEFINE_CALLOC -DDEFINE_CFREE -DDEFINE_MEMALIGN -DDEFINE_VALLOC -DDEFINE_PVALLOC -DDEFINE_MALLINFO -DDEFINE_MALLOC_STATS -DDEFINE_MALLOC_USABLE_SIZE -DDEFINE_MALLOPT -DINTEGER_ONLY -DSTRING_ONLY

# -D_WANT_IO_LONG_LONG

LLXVM_CC=../../../llxvm-cc/llxvm-cc

SRCS := $(shell find $(SOURCEDIR) -name '*.c' | grep -v environ.c)

OBJS := ${SRCS:.c=.bc}	

CLASSNAME := llxvm.lib.c

TGT := ../stdlib.bc

all: $(TGT)

%.bc: %.c 
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

$(TGT): $(OBJS)
	$(LLXVM_CC) -M jvm -K -v -m -C $(CLASSNAME) -o $@ $(OBJS)

clean:
	rm -f $$(find . -name "*.bc") && rm -f $$(find . -name "*.ll") && rm -f $$(find . -name "*.j")
