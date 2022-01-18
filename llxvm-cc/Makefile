include ../Makefile.inc

BACKEND_JVM_OBJS = backend-jvm/jvm_block.o backend-jvm/jvm_branch.o backend-jvm/jvm_const.o \
				   backend-jvm/jvm_function.o backend-jvm/jvm_instruction.o backend-jvm/JVMWriter.o \
				   backend-jvm/jvm_loadstore.o backend-jvm/jvm_backend.o backend-jvm/jvm_name.o \
				   backend-jvm/jvm_printinst.o backend-jvm/jvm_sections.o backend-jvm/jvm_types.o \
				   JvmAsmDriver.o
				   
BACKEND_JVM_SRCS = backend-jvm/jvm_block.cpp backend-jvm/jvm_branch.cpp backend-jvm/jvm_const.cpp \
				   backend-jvm/jvm_function.cpp backend-jvm/jvm_instruction.cpp JVMWriter.cpp \
				   backend-jvm/jvm_loadstore.cpp backend-jvm/jvm_backend.cpp backend-jvm/jvm_name.cpp \
				   backend-jvm/jvm_printinst.cpp backend-jvm/jvm_sections.cpp backend-jvm/jvm_types.cpp \
				   JvmAsmDriver.cpp
				   
BACKEND_JVM_HDRS = backend-jvm/JVMWriter.h jbackend-jvm/vm-backend.h backend-jvm/opts.h JvmAsmDriver.h

BACKEND_CIL_OBJS = backend-cil/cil_block.o backend-cil/cil_branch.o backend-cil/cil_const.o \
				   backend-cil/cil_function.o backend-cil/cil_instruction.o backend-cil/CILWriter.o \
				   backend-cil/cil_loadstore.o backend-cil/cil_backend.o backend-cil/cil_name.o \
				   backend-cil/cil_printinst.o backend-cil/cil_sections.o backend-cil/cil_types.o \
				   CILAsmDriver.o
				   
BACKEND_CIL_SRCS = backend-cil/cil_block.cpp backend-cil/cil_branch.cpp backend-cil/cil_const.cpp \
				   backend-cil/cil_function.cpp backend-cil/cil_instruction.cpp backend-cil/CILWriter.cpp \
				   backend-cil/cil_loadstore.cpp backend-cil/cil_backend.cpp backend-cil/cil_name.cpp \
				   backend-cil/cil_printinst.cpp backend-cil/cil_sections.cpp backend-cil/cil_types.cpp \
				   CILAsmDriver.cpp
				   
BACKEND_CIL_HDRS = backend-jvm/CILWriter.h backend-cil/cil-backend.h backend-cil/opts.h CILAsmDriver.h

JREFLECT_OBJS = classreader.o jarreader.o
JREFLECT_SRCS = classreader.cpp jarreader.cpp
JREFLECT_HDRS = classreader.h jarreader.h

JLINKER_OBJS = jasminlinker.o
JLINKER_SRCS = jasminlinker.cpp
JLINKER_HDRS = jasminlinker.h

ZIP_OBJS = zip.o
ZIP_SRCS = zip.c
ZIP_HDRS = miniz.h zip.h

OBJS	= libcpputils.o main.o AsmDriver.o $(BACKEND_JVM_OBJS) $(BACKEND_CIL_OBJS) $(JREFLECT_OBJS) $(ZIP_OBJS) $(JLINKER_OBJS)
SOURCE	= libcpputils.cpp main.cpp AsmDriver.cpp $(BACKEND_JVM_SRCS) $(BACKEND_CIL_SRCS) $(JREFLECT_SRCS) $(ZIP_SRCS) $(JLINKER_SRCS)
HEADER	= libcpputils.h AsmDriver.h $(BACKEND_JVM_HDRS) $(BACKEND_CIL_HDRS) $(JREFLECT_HDRS) $(ZIP_HDRS) $(JLINKER_HDRS)

OUT	 = llxvm-cc
CFLAGS	 = -std=gnu99 -I$(shell $(LLVM_CONFIG) --includedir) -I backend-jvm -I backend-cil -I jreflect -I jlinker -I zip -I ..  -I . -fno-rtti
CXXFLAGS = -std=c++17 -I$(shell $(LLVM_CONFIG) --includedir) -I backend-jvm -I backend-cil -I jreflect -I jlinker -I zip -I ..  -I . -fno-rtti
LDFLAGS	 = -std=c++17 -L$(shell $(LLVM_CONFIG) --libdir) $(shell $(LLVM_CONFIG) --libs --system-libs --ldflags) -lstdc++fs

VPATH=backend:jreflect:jlinker:zip

all: llxvm-cc

llxvm-cc: $(OBJS)
	$(CXX) -o $@ $^ -std=c++17 $(LDFLAGS)

%.o: %.c $(HEADER)
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.cpp $(HEADER)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(OUT)
