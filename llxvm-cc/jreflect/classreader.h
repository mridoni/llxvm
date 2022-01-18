#pragma once

#include <string>
#include <map>
#include <vector>
#include <variant>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

enum class AccessFlags {
	ACC_ABSTRACT = 1024,
	ACC_ANNOTATION = 8192,
	ACC_BRIDGE = 64,
	ACC_DEPRECATED = 131072,
	ACC_ENUM = 16384,
	ACC_FINAL = 16,
	ACC_INTERFACE = 512,
	ACC_MANDATED = 32768,
	ACC_MODULE = 32768,
	ACC_NATIVE = 256,
	ACC_OPEN = 32,
	ACC_PRIVATE = 2,
	ACC_PROTECTED = 4,
	ACC_PUBLIC = 1,
	ACC_RECORD = 65536,
	ACC_STATIC = 8,
	ACC_STATIC_PHASE = 64,
	ACC_STRICT = 2048,
	ACC_SUPER = 32,
	ACC_SYNCHRONIZED = 32,
	ACC_SYNTHETIC = 4096,
	ACC_TRANSIENT = 128,
	ACC_TRANSITIVE = 32,
	ACC_VARARGS = 128,
	ACC_VOLATILE = 64
};

enum class ConstantPoolTag
{
	NONE = 0,

	J_STRING_UTF8 = 1, /* occupies 2+x bytes */
	J_INTEGER = 3, /* 32bit two's-compliment big endian int */
	J_FLOAT = 4, /* 32-bit single precision */
	J_LONG = 5, /* Long: a signed 64-bit two's complement number in big-endian format (takes two slots in the constant pool table) */
	J_DOUBLE = 6, /* Double: a 64-bit double-precision IEEE 754 floating-point number (takes two slots in the constant pool table) */
	J_CLASS = 7, /* Class reference: an index within the constant pool to a UTF-8 string containing the fully qualified class name (in internal format) */
	J_STRING = 8, /* JString reference: an index within the constant pool to a UTF-8 string */
	J_FIELD = 9, /* Field reference: two indexes within the constant pool, the first pointing to a Class reference, the second to a Name and Type descriptor. */
	J_METHOD = 10, /* Method reference: two indexes within the constant pool, the first pointing to a Class reference, the second to a Name and Type descriptor. */
	J_INTERFACE_METHOD = 11, /* Interface method reference: two indexes within the constant pool, the first pointing to a Class reference, the second to a Name and Type descriptor. */
	J_NAME = 12, /* Name and type descriptor: 2 indexes to UTF-8 strings, the first representing a name and the second a specially encoded type descriptor. */
	J_METHOD_HANDLE = 15,
	J_METHOD_TYPE = 16,
	J_INVOKE_DYNAMIC = 18
};

struct Attribute
{
	uint16_t name_idx;
	uint32_t length;
	std::string info;
};

struct Double
{
	uint32_t high;
	uint32_t low;
};

struct Field
{
	uint16_t flags;
	uint16_t name_idx;
	uint16_t type_idx;
	std::vector<Attribute *> attrs;

	bool isStatic();
	bool isPublic();
	bool isEnum();
};

struct Long
{
	uint32_t high;
	uint32_t low;
};

struct Method
{
	uint16_t flags;
	uint16_t name_idx;
	uint16_t type_idx;
	std::vector<Attribute *> attrs;

	bool isStatic();
	bool isPublic();
};

/* Wraps references to an item in the constant pool */
struct Ref
{
	uint16_t class_idx = 0;
	uint16_t name_idx = 0;
};

struct Item {
	ConstantPoolTag tag = ConstantPoolTag::NONE; // the tag byte

	std::variant<std::string, float, Double, Long, int32_t, Ref> value;
};

struct ClassInfo
{
	std::string path;
    std::string name;

	uint16_t minor_version = 0;
	uint16_t major_version = 0;
	uint16_t const_pool_count = 0;
	uint32_t pool_size_bytes = 0;
	std::vector<Item *> items;
	uint16_t flags = 0;
	uint16_t this_class = 0;
	uint16_t super_class = 0;
	std::vector<Ref *>interfaces;
	std::vector<Field *> fields;
	std::vector<Method *> methods;
	std::vector<Attribute *>attributes;

	std::string getClassName();


	std::string getMethodName(Method *m);
	std::string getFieldName(Field *m);
	std::string getMethodSignature(Method * m);
	std::string getFieldSignature(Field *f);
	std::vector<std::string> getMethodParameterTypes(Method *m);
	std::string getMethodReturnType(Method *m);
	bool isMethod(Item *item);

};

class ClassReader
{
public:
    ClassReader();

    ClassInfo *read(const std::string &_classfile_path);


private:
    FILE *file;

	uint16_t read_host_ushort();
	uint32_t read_host_uint();
	int32_t read_host_int();
	std::string read_host_string(int sz);
	uint8_t read_host_byte();	// just for consistency
	float read_host_float();

	bool is_class();
	bool parse_header(ClassInfo *ci);
	bool parse_const_pool(ClassInfo *ci);
	bool parse_attribute(Attribute *a);
};

