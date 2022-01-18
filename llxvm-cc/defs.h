#pragma once

#ifdef _WIN32
#define PATH_LIST_SEP ";"
#define LIB_SUFFIX ".lib"
#define OBJ_SUFFIX ".obj"

#pragma comment(lib, "Ws2_32.lib")

#else
#define PATH_LIST_SEP ":"
#define LIB_SUFFIX ".a"
#define OBJ_SUFFIX ".o"
#endif

#define LLXVM_VER "0.0.1"

#define MODE_JVM	1
#define MODE_IL	2

#define BC_SUFFIX ".bc"
#define LL_SUFFIX ".ll"
#define JASMIN_SUFFIX ".j"
#define JASMIN_PRELINK_SUFFIX ".plj"
#define ILASM_SUFFIX ".il"
#define ILASM_PRELINK_SUFFIX ".plil"
#define DEFAULT_CLASSFILE_VER	"5.0"		// Java 5
#define DEFAULT_IL_VER			"4.0"		// .Net 4.0

