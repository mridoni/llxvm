include ../../../Makefile.inc

.EXPORT_ALL_VARIABLES:

#export CFLAGS=-O0 -I../include -I/usr/i686-linux-gnu/include -D"__GLIBC_USE(...)=0" -DMISSING_SYSCALL_NAMES -DHAVE_RENAME -DNO_FLOATING_POINT -DHAVE_MMAP=0 -DINTERNAL_NEWLIB -DDEFINE_MALLOC -DDEFINE_FREE -DDEFINE_REALLOC -DDEFINE_CALLOC -DDEFINE_CFREE -DDEFINE_MEMALIGN -DDEFINE_VALLOC -DDEFINE_PVALLOC -DDEFINE_MALLINFO -DDEFINE_MALLOC_STATS -DDEFINE_MALLOC_USABLE_SIZE -DDEFINE_MALLOPT -DINTEGER_ONLY -DSTRING_ONLY


LLXVM_CC=../../../llxvm-cc/llxvm-cc

STD_OBJS := asiprintf.bc asniprintf.bc asnprintf.bc asprintf.bc clearerr.bc diprintf.bc dprintf.bc fcloseall.bc fclose.bc fdopen.bc feof.bc ferror.bc fflush.bc \
			fgetc.bc fgetpos.bc fgets.bc fgetwc.bc fgetws.bc fileno.bc findfp.bc fiprintf.bc fiscanf.bc flags.bc fmemopen.bc fopen.bc fopencookie.bc fprintf.bc \
			fputc.bc fputs.bc fputwc.bc fputws.bc fread.bc freopen.bc fscanf.bc fseek.bc fseeko.bc fsetpos.bc ftell.bc ftello.bc funopen.bc fvwrite.bc fwalk.bc \
			fwide.bc fwrite.bc getc.bc getchar.bc getchar_u.bc getc_u.bc getdelim.bc getline.bc gets.bc getw.bc getwc.bc getwchar.bc iprintf.bc iscanf.bc makebuf.bc \
			mktemp.bc open_memstream.bc perror.bc printf.bc putc.bc putchar.bc putchar_u.bc putc_u.bc puts.bc putw.bc putwc.bc putwchar.bc refill.bc remove.bc \
			rename.bc rewind.bc rget.bc scanf.bc sccl.bc setbuf.bc setbuffer.bc setlinebuf.bc setvbuf.bc siprintf.bc siscanf.bc sniprintf.bc snprintf.bc sprintf.bc \
			sscanf.bc stdio.bc vfprintf.bc vfscanf.bc viprintf.bc viscanf.bc vprintf.bc vscanf.bc vsiprintf.bc vsiscanf.bc vsniprintf.bc vsnprintf.bc vsprintf.bc \
			vsscanf.bc wbuf.bc wsetup.bc tmpfile.bc tmpnam.bc ungetc.bc ungetwc.bc vasiprintf.bc vasniprintf.bc vasnprintf.bc vasprintf.bc vdiprintf.bc vdprintf.bc 
			
INT_OBJS :=  vfiprintf.bc vfiscanf.bc

STR_OBJS :=  svfprintf.bc svfscanf.bc

INT_STR_OBJS :=  svfiprintf.bc svfiscanf.bc

CFLAGS_STD=-O0 -I../include -I/usr/i686-linux-gnu/include -D"__GLIBC_USE(...)=0" -DMISSING_SYSCALL_NAMES -DHAVE_RENAME -D_WANT_IO_LONG_LONG
CFLAGS_INT=-DINTEGER_ONLY 
CFLAGS_STR=-DSTRING_ONLY
CFLAGS_INT_STR=-DINTEGER_ONLY -DSTRING_ONLY

CLASSNAME := llxvm.lib.c

OBJS = $(STD_OBJS) $(INT_OBJS) $(STR_OBJS) $(INT_STR_OBJS) 

all: ../stdio.bc

# %.bc: %.c 
	# $(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

../stdio.bc: $(OBJS)
	$(LLXVM_CC) -M jvm -K -v -m -C $(CLASSNAME) -o $@ $(OBJS)

clean:
	rm -f $$(find . -name "*.bc") && rm -f $$(find . -name "*.ll") && rm -f $$(find . -name "*.j")
	
%.bc: %.c 	
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@ 	

# STANDARD

asiprintf.bc: export CFLAGS=$(CFLAGS_STD)
asiprintf.bc: asiprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

asniprintf.bc: export CFLAGS=$(CFLAGS_STD)
asniprintf.bc: asniprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

asnprintf.bc: export CFLAGS=$(CFLAGS_STD)
asnprintf.bc: asnprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

asprintf.bc: export CFLAGS=$(CFLAGS_STD)
asprintf.bc: asprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

clearerr.bc: export CFLAGS=$(CFLAGS_STD)
clearerr.bc: clearerr.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

diprintf.bc: export CFLAGS=$(CFLAGS_STD)
diprintf.bc: diprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

dprintf.bc: export CFLAGS=$(CFLAGS_STD)
dprintf.bc: dprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fcloseall.bc: export CFLAGS=$(CFLAGS_STD)
fcloseall.bc: fcloseall.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fclose.bc: export CFLAGS=$(CFLAGS_STD)
fclose.bc: fclose.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fdopen.bc: export CFLAGS=$(CFLAGS_STD)
fdopen.bc: fdopen.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

feof.bc: export CFLAGS=$(CFLAGS_STD)
feof.bc: feof.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

ferror.bc: export CFLAGS=$(CFLAGS_STD)
ferror.bc: ferror.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fflush.bc: export CFLAGS=$(CFLAGS_STD)
fflush.bc: fflush.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fgetc.bc: export CFLAGS=$(CFLAGS_STD)
fgetc.bc: fgetc.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fgetpos.bc: export CFLAGS=$(CFLAGS_STD)
fgetpos.bc: fgetpos.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fgets.bc: export CFLAGS=$(CFLAGS_STD)
fgets.bc: fgets.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fgetwc.bc: export CFLAGS=$(CFLAGS_STD)
fgetwc.bc: fgetwc.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fgetws.bc: export CFLAGS=$(CFLAGS_STD)
fgetws.bc: fgetws.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fileno.bc: export CFLAGS=$(CFLAGS_STD)
fileno.bc: fileno.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

findfp.bc: export CFLAGS=$(CFLAGS_STD)
findfp.bc: findfp.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fiprintf.bc: export CFLAGS=$(CFLAGS_STD)
fiprintf.bc: fiprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fiscanf.bc: export CFLAGS=$(CFLAGS_STD)
fiscanf.bc: fiscanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

flags.bc: export CFLAGS=$(CFLAGS_STD)
flags.bc: flags.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fmemopen.bc: export CFLAGS=$(CFLAGS_STD)
fmemopen.bc: fmemopen.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fopen.bc: export CFLAGS=$(CFLAGS_STD)
fopen.bc: fopen.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fopencookie.bc: export CFLAGS=$(CFLAGS_STD)
fopencookie.bc: fopencookie.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fprintf.bc: export CFLAGS=$(CFLAGS_STD)
fprintf.bc: fprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fputc.bc: export CFLAGS=$(CFLAGS_STD)
fputc.bc: fputc.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fputs.bc: export CFLAGS=$(CFLAGS_STD)
fputs.bc: fputs.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fputwc.bc: export CFLAGS=$(CFLAGS_STD)
fputwc.bc: fputwc.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fputws.bc: export CFLAGS=$(CFLAGS_STD)
fputws.bc: fputws.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fread.bc: export CFLAGS=$(CFLAGS_STD)
fread.bc: fread.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

freopen.bc: export CFLAGS=$(CFLAGS_STD)
freopen.bc: freopen.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fscanf.bc: export CFLAGS=$(CFLAGS_STD)
fscanf.bc: fscanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fseek.bc: export CFLAGS=$(CFLAGS_STD)
fseek.bc: fseek.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fseeko.bc: export CFLAGS=$(CFLAGS_STD)
fseeko.bc: fseeko.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fsetpos.bc: export CFLAGS=$(CFLAGS_STD)
fsetpos.bc: fsetpos.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

ftell.bc: export CFLAGS=$(CFLAGS_STD)
ftell.bc: ftell.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

ftello.bc: export CFLAGS=$(CFLAGS_STD)
ftello.bc: ftello.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

funopen.bc: export CFLAGS=$(CFLAGS_STD)
funopen.bc: funopen.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fvwrite.bc: export CFLAGS=$(CFLAGS_STD)
fvwrite.bc: fvwrite.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fwalk.bc: export CFLAGS=$(CFLAGS_STD)
fwalk.bc: fwalk.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fwide.bc: export CFLAGS=$(CFLAGS_STD)
fwide.bc: fwide.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

fwrite.bc: export CFLAGS=$(CFLAGS_STD)
fwrite.bc: fwrite.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

getc.bc: export CFLAGS=$(CFLAGS_STD)
getc.bc: getc.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

getchar.bc: export CFLAGS=$(CFLAGS_STD)
getchar.bc: getchar.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

getchar_u.bc: export CFLAGS=$(CFLAGS_STD)
getchar_u.bc: getchar_u.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

getc_u.bc: export CFLAGS=$(CFLAGS_STD)
getc_u.bc: getc_u.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

getdelim.bc: export CFLAGS=$(CFLAGS_STD)
getdelim.bc: getdelim.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

getline.bc: export CFLAGS=$(CFLAGS_STD)
getline.bc: getline.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

gets.bc: export CFLAGS=$(CFLAGS_STD)
gets.bc: gets.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

getw.bc: export CFLAGS=$(CFLAGS_STD)
getw.bc: getw.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

getwc.bc: export CFLAGS=$(CFLAGS_STD)
getwc.bc: getwc.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

getwchar.bc: export CFLAGS=$(CFLAGS_STD)
getwchar.bc: getwchar.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

iprintf.bc: export CFLAGS=$(CFLAGS_STD)
iprintf.bc: iprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

iscanf.bc: export CFLAGS=$(CFLAGS_STD)
iscanf.bc: iscanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

makebuf.bc: export CFLAGS=$(CFLAGS_STD)
makebuf.bc: makebuf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

mktemp.bc: export CFLAGS=$(CFLAGS_STD)
mktemp.bc: mktemp.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

open_memstream.bc: export CFLAGS=$(CFLAGS_STD)
open_memstream.bc: open_memstream.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

perror.bc: export CFLAGS=$(CFLAGS_STD)
perror.bc: perror.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

printf.bc: export CFLAGS=$(CFLAGS_STD)
printf.bc: printf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

putc.bc: export CFLAGS=$(CFLAGS_STD)
putc.bc: putc.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

putchar.bc: export CFLAGS=$(CFLAGS_STD)
putchar.bc: putchar.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

putchar_u.bc: export CFLAGS=$(CFLAGS_STD)
putchar_u.bc: putchar_u.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

putc_u.bc: export CFLAGS=$(CFLAGS_STD)
putc_u.bc: putc_u.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

puts.bc: export CFLAGS=$(CFLAGS_STD)
puts.bc: puts.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

putw.bc: export CFLAGS=$(CFLAGS_STD)
putw.bc: putw.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

putwc.bc: export CFLAGS=$(CFLAGS_STD)
putwc.bc: putwc.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

putwchar.bc: export CFLAGS=$(CFLAGS_STD)
putwchar.bc: putwchar.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

refill.bc: export CFLAGS=$(CFLAGS_STD)
refill.bc: refill.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

remove.bc: export CFLAGS=$(CFLAGS_STD)
remove.bc: remove.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

rename.bc: export CFLAGS=$(CFLAGS_STD)
rename.bc: rename.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

rewind.bc: export CFLAGS=$(CFLAGS_STD)
rewind.bc: rewind.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

rget.bc: export CFLAGS=$(CFLAGS_STD)
rget.bc: rget.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

scanf.bc: export CFLAGS=$(CFLAGS_STD)
scanf.bc: scanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

sccl.bc: export CFLAGS=$(CFLAGS_STD)
sccl.bc: sccl.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

setbuf.bc: export CFLAGS=$(CFLAGS_STD)
setbuf.bc: setbuf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

setbuffer.bc: export CFLAGS=$(CFLAGS_STD)
setbuffer.bc: setbuffer.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

setlinebuf.bc: export CFLAGS=$(CFLAGS_STD)
setlinebuf.bc: setlinebuf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

setvbuf.bc: export CFLAGS=$(CFLAGS_STD)
setvbuf.bc: setvbuf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

siprintf.bc: export CFLAGS=$(CFLAGS_STD)
siprintf.bc: siprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

siscanf.bc: export CFLAGS=$(CFLAGS_STD)
siscanf.bc: siscanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

sniprintf.bc: export CFLAGS=$(CFLAGS_STD)
sniprintf.bc: sniprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

snprintf.bc: export CFLAGS=$(CFLAGS_STD)
snprintf.bc: snprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

sprintf.bc: export CFLAGS=$(CFLAGS_STD)
sprintf.bc: sprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

sscanf.bc: export CFLAGS=$(CFLAGS_STD)
sscanf.bc: sscanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

stdio.bc: export CFLAGS=$(CFLAGS_STD)
stdio.bc: stdio.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vfprintf.bc: export CFLAGS=$(CFLAGS_STD)
vfprintf.bc: vfprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vfscanf.bc: export CFLAGS=$(CFLAGS_STD)
vfscanf.bc: vfscanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

viprintf.bc: export CFLAGS=$(CFLAGS_STD)
viprintf.bc: viprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

viscanf.bc: export CFLAGS=$(CFLAGS_STD)
viscanf.bc: viscanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vprintf.bc: export CFLAGS=$(CFLAGS_STD)
vprintf.bc: vprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vscanf.bc: export CFLAGS=$(CFLAGS_STD)
vscanf.bc: vscanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vsiprintf.bc: export CFLAGS=$(CFLAGS_STD)
vsiprintf.bc: vsiprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vsiscanf.bc: export CFLAGS=$(CFLAGS_STD)
vsiscanf.bc: vsiscanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vsniprintf.bc: export CFLAGS=$(CFLAGS_STD)
vsniprintf.bc: vsniprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vsnprintf.bc: export CFLAGS=$(CFLAGS_STD)
vsnprintf.bc: vsnprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vsprintf.bc: export CFLAGS=$(CFLAGS_STD)
vsprintf.bc: vsprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vsscanf.bc: export CFLAGS=$(CFLAGS_STD)
vsscanf.bc: vsscanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

wbuf.bc: export CFLAGS=$(CFLAGS_STD)
wbuf.bc: wbuf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

wsetup.bc: export CFLAGS=$(CFLAGS_STD)
wsetup.bc: wsetup.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

tmpfile.bc: export CFLAGS=$(CFLAGS_STD)
tmpfile.bc: tmpfile.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

tmpnam.bc: export CFLAGS=$(CFLAGS_STD)
tmpnam.bc: tmpnam.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

ungetc.bc: export CFLAGS=$(CFLAGS_STD)
ungetc.bc: ungetc.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

ungetwc.bc: export CFLAGS=$(CFLAGS_STD)
ungetwc.bc: ungetwc.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vasiprintf.bc: export CFLAGS=$(CFLAGS_STD)
vasiprintf.bc: vasiprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vasniprintf.bc: export CFLAGS=$(CFLAGS_STD)
vasniprintf.bc: vasniprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vasnprintf.bc: export CFLAGS=$(CFLAGS_STD)
vasnprintf.bc: vasnprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vasprintf.bc: export CFLAGS=$(CFLAGS_STD)
vasprintf.bc: vasprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vdiprintf.bc: export CFLAGS=$(CFLAGS_STD)
vdiprintf.bc: vdiprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vdprintf.bc: export CFLAGS=$(CFLAGS_STD)
vdprintf.bc: vdprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@
	
# INT_ONLY 
vfiprintf.bc: export CFLAGS=$(CFLAGS_STD) $(CFLAGS_INT)
vfiprintf.bc: vfprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

vfiscanf.bc: export CFLAGS=$(CFLAGS_STD) $(CFLAGS_INT)
vfiscanf.bc: vfscanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

# STRING_ONLY
svfprintf.bc: export CFLAGS=$(CFLAGS_STD) $(CFLAGS_STR)
svfprintf.bc: vfprintf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

svfscanf.bc: export CFLAGS=$(CFLAGS_STD) $(CFLAGS_STR)
svfscanf.bc: vfscanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

# INT_ONLY + STRING_ONLY
svfiprintf.bc: export CFLAGS=$(CFLAGS_STD) $(CFLAGS_INT_STR)
svfiprintf.bc: vfprintf.c 
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@

svfiscanf.bc: export CFLAGS=$(CFLAGS_STD) $(CFLAGS_INT_STR)
svfiscanf.bc: vfscanf.c
	$(LLXVM_CC) -M jvm -T -v -c -C $(CLASSNAME) $< -o $@
