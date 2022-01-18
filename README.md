# LLXVM

llxvm is a tool that allows to compile C source files to JVM bytecode (.class) or CIL/.Net (.exe/dll) and run traditional applications written in C to run in a pure-Java environment (with some caveats). 

llxvm is based on [lljvm](https://github.com/davidar/lljvm) by David a. Roberts, a project that unfortunately has been dormant for several years and relies on a now archaic version of LLVM/clang (2.7). I made several changes to adapt it to a more modern version of LLVM/clang (10+), removed some unnecessary tools, rewrote and expanded the linker and added other tools. Moreover, I refactored the backend's code to be able to emit CIL/.Net assembly beside JVM bytecode).

Currently `llxvm` is able to build and run several programs and libraries: as an example libgmp, vbisam and GnuCOBOL's libcob library have been successfully compiled and run, though with some caveats (see "[Notes about code size](notes_code_size)" below). 

*Beware: the .Net/CIL support is pre-alpha, there is no actual working runtime, just some stubs, so for all intents and purposes you should stick (for now) to working with JVM code. In the following paragraphs, for the sake of brevity, only Java/JVM will be referenced, but the concepts also generally apply to the .Net/CIL environment*

## How it works
There are several pieces that make up the llxvm package:
- The `llxvm-cc` tool (compiler driver, assembler generator, linker, etc.)  
- The Java runtime
- The C library 

During the build process `llxvm-cc` is built first, then it is used to build the C library (a old but functional version of newlib). Then the Java runtime (written in Java) is built and linked with the C library into the llxvm.jar/.dll.

The `llxvm-cc` tool performs (separately or in sequence) the following functions:
- Invokes clang on a .c source file to generate a bitcode file (.bc)
- (Optionally) invokes llvm-link to merge more .bc files into a single one
- Generates JVM assembly (Jasmin or Krakatau format) from the .bc files, emitting a .plj ("pre-link JVM") file
- "Links" the .plj file against: .jar archives, .class files and other .plj files and emits a linked .j file, in which all the extern calls and references have been resolved. The runtime must also be linked in during this phase
- Assembles the .j file (with Jasmin or Krakatau) and generates a .class file

At this point you can directly run your class file or package more of them into a .jar (obviously they can be freely mixed with .class files built from Java source code). You will need to have the `llxvm.jar` runtime on your classpath, but that's basically it.

For simpler tasks `llxvm` supports a "in sequence" mode of operation: you can invoke it a single time on a C source file, with the appropriate parameters, and it directly emits the corresponding .class file.

## 
## Building and installing
llxvm runs mainly on Linux. It is possible to run it on Windows, but for now there are no scripts to build the C library on this platform. Since the llxvm-compiled C library (and the runtime) are pure JVM code and have no native dependencies, you can build the C library on Linux, `llxvm-cc` on Windows and mix the two. In the (near) future you will be able to use one of the provided binary packages.

### Requirements
To build llxvm you will need:

- A full install of LLVM/clang 10.x (13.x can also be used and will allow to compile, but it hasn't been fully tested)
- A working JDK (Java 8 has been used for testing and is recommended)
- Ant (to build the Java runtime)
- Assorted buld tools (make, etc.) 
- Jasmin 2.4 (included in the distribution); Krakatau is technically supported but it has not been fully tested and probably llxvm has still some issues with it.

To run llxvm (from one of the binary packages or the one you built yourself) you will basically need the same stuff except for Ant and Make (if you are not already using them, of course).


### Building
Clone the repository then:

- Ensure you have clang 10.x in your PATH, e.g.
		 
		export PATH=/opt/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/bin:$PATH

- Ensure you have the JDK on your path
- Set the JASMIN_PATH environment variable to point to the jasmin.jar file provided in the distribution, e.g (if you have cloned the repository in /tmp):  
		
		export JASMIN_PATH=/tmp/llxvm/tools/jasmin.jar

- Then enter `make` and wait
- If all goes well you can install with:

		sudo make install

	or
	
		sudo DEST_DIR=/opt/wherever make install
	
### Running
For a simple test, cd into the test subdirectory and run:

	llxvm-cc --mode jvm -C hello -l /opt/llxvm/lib/llxvm.jar hello.c

(if you have installed in a different directory, obviously modify the path for llxvm.jar accordingly). You should obtain a hello.class file. To run it execute:

	java -cp .:/opt/llxvm/lib/llxvm.jar hello

and you should get a nice "Hello world!" message.

### Running (advanced)

These are the options provided by llxvm-cc:

	lxvm - LLXVM C driver
	Builds a JVM/.Net object file from one or more C sources
	Version: 0.0.1
	(c) 2021 Marco Ridoni (m.ridoni@gmail.com)
	(c) 2010 David A. Roberts (https://davidar.io/)
	
	Options:
	  -h, --help                 displays help on commandline options
	  -M, --mode arg             Mode: (J/jvm or N/cil/.net/dotnet)
	  -c, --compile              only compile C files
	  -m, --merge-bc             merge bitcode filess
	  -j, --bc2asm               complle .bc files to .j/.il
	  -a, --link                 link and resolve .j files against Java class libraries
	  -C, --classname arg        class name
	  -g, --debug arg (=0)       debug level
	  -K, --keep                 keep temporary files
	  -l, --lib arg              link with/resolve against Java class library
	  -L, --libdir arg           link library search path
	  -o, --outfile arg          output file
	  -O, --javaout arg (=.)     output base directory for class files
	  -y, --dry-run              dry-run (only prints commands)
	  -T, --llvm-ir-as-text      also generate LLVM IR in text format
	  -v, --verbose              verbose
	  -r, --link-scan-recursive  scan recursively when searching for Java class libraries
	  -V, --object-version arg   JVM class file version (1.0.2, 1.1, 1.2, 1.3, 1.4, 5.0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16)
	                             .Net framework version (2.0, 4.0, 4.5, 4.6, 4.7, 4.8)
	  -U, --use_krakatau         JVM mode: use Krakatau instead of Jasmin when generating or assembling .j files
	  -B, --rewrite-branches     JVM mode: rewrite branches to avoid exceeding the 16 bit offset limit. Implies -W
	  -W, --wide_gotos           JVM mode: use wide gotos (goto_w, 32 bit offset) instead of standard goto (goto, 16 bit offset)
	  -N, --wide_ldc             JVM mode: always use ldc2_w instead of ldc
	  -S, --skip-locals-init     JVM mode: skip initialization of local variables in .j files
	  -k, --j2class              JVM mode: uild .class file from .j

Basically you will use the -M option to choose a "mode of operation" (JVM or CIL/.Net) and execute one of the build operations, generally in the order indicated below:

- **-c** : invoke clang on a .c source file to generate a bitcode file (.bc)
- **-m** (optional) invoke llvm-link to merge more .bc files into a single one
- **-j** : emit JVM assembly (Jasmin or Krakatau format) from the .bc files, emitting a .plj ("pre-link JVM") file
- **-a** : link the .plj file and emit a linked .j file, in which all the extern calls and references have been resolved
- **-k** : assemble the .j file and generate a .class file

For almost all of this options you will have to specify a Java class name (with -C).

### Advanced options

- **-T** : emit a text version of the bitcode beside the binary one
- **-l** : link with a file (.plj, .class or .jar), can be used multiple times
- **-L** : look here for files to link, can be used multiple times
- **-O** : use this directory as the output base path for .class file, respecting the hierarchy defined by their namespace
- **-K** : keep intermediate files
- **-g** : debug level (not really useful at this stage)

### Advanced JVM options

- **-U** : Uses Krakatau instead of Jasmin for assembling the .j files. While this is technically supported, it has not been tested like Jasmin. In case, you wlll have to set the environment variable KRAKATAU_HOME to point o the locatin where Krakatau resides.
-  **-W** : use "wide" goto instructions in the generated JVM assembly code. Code using standard goto statements can only handle 16-bit offsets. In some cases (bigger and more complex programs) this might lead to invalid/uncompilable code. Using 32 bit offsets with -W ensure the program will run, at the price of increased code size.
-   **-B** : refactor branches to use jump instructions and avoid problems with 16-bit offsets (see above). Implies -W.
-   **-W** : uses "wide" ldc instructions for constants that exceed a 16-bit index or value
-    **-S** : skip initialization of local variables. This reduces code size but might lead to invalid/not-verifiable JVM bytecode


## <a name="notes_code_size"></a>Notes about code size

The JVM allows allows for a maximum size of 64K of bytecode for each method in a class. This means that very long C functions, when translated, could run over this limit. While it is possibile to compile such code to bytecode, trying to run it will lead to an un-verifiable (and un-runnable) .class file. There are other such limits in the JVM, but this is by far the most annoying one. The only option here is to rewrite the code, splitting very long functions into smaller, more manageable sub-functions.

Unfortunately this problem has been present in the JVM since the dawn of time, and has been the cause of multiple headaches for writers of tools and code generators (it can also affect the processing of JSP pages). At this point it is not likely it will ever be solved, given the amount of code and tools it would impact. 

.Net/CIL, on the other hand, has much bigger limits and no practical issues should arise.