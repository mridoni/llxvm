#include "classreader.h"
#include "portable-endian.h"

#include <stdio.h>
#include <errno.h>
#include <cstring>

static int total_bytes = 0;

#define MIN_CPOOL_TAG 1
#define MAX_CPOOL_TAG 18

static int safe_fread(void *_Buffer, size_t _ElementSize, size_t _ElementCount, FILE *_Stream)
{
	int n = fread(_Buffer, _ElementSize, _ElementCount, _Stream);
	if (n != _ElementCount) {
		fprintf(stderr, "Error while reading from class file, total bytes read so far: %d\n", total_bytes);
	}
	total_bytes += (_ElementSize * _ElementCount);

	return n;
}

uint16_t ClassReader::read_host_ushort()
{
	uint16_t i;
	safe_fread(&i, sizeof(uint16_t), 1, file);
	return be16toh(i);
}

float ClassReader::read_host_float()
{
	float i;
	safe_fread(&i, sizeof(float), 1, file);
	return be32toh(i);
}

int32_t ClassReader::read_host_int()
{
	int32_t i;
	safe_fread(&i, sizeof(uint32_t), 1, file);
	return be32toh(i);
}

uint32_t ClassReader::read_host_uint()
{
	uint32_t i;
	safe_fread(&i, sizeof(uint32_t), 1, file);
	return be32toh(i);
}

std::string ClassReader::read_host_string(int sz)
{
	char *tmp = (char *)calloc(sz + 1, sizeof(char));
	safe_fread(tmp, sizeof(char), sz, file);
	tmp[sz] = '\0';
	std::string s = std::string(tmp);
	free(tmp);
	return s;
}

uint8_t ClassReader::read_host_byte()
{
	uint8_t b; 
	safe_fread(&b, sizeof(uint8_t), 1, file);
	return b;
}

ClassReader::ClassReader()
{

}

#define RETURN_READ_ERR() delete ci; return nullptr

ClassInfo *ClassReader::read(const std::string &_classfile_path)
{
	ClassInfo *ci = new ClassInfo();

	ci->path = _classfile_path;

	file = fopen(_classfile_path.c_str(), "rb");
	if (!file) {
		fprintf(stderr, "Could not open '%s': %s\n", ci->path.c_str(), strerror(errno));
		RETURN_READ_ERR();
	}
	// Check the file header for .class nature
	if (!is_class()) {
		fprintf(stderr, "Skipping '%s': not a valid class file\n", ci->path.c_str());
		RETURN_READ_ERR();
	}

	if (!parse_header(ci)) {
		RETURN_READ_ERR();
	}

	if (!parse_const_pool(ci)) {
		RETURN_READ_ERR();
	}

	if (ci->pool_size_bytes == 0) {
		return NULL;
	}



	ci->flags = read_host_ushort();
	ci->this_class = read_host_ushort();
	ci->super_class = read_host_ushort();
	int interfaces_count = read_host_ushort();


	//ci->interfaces = calloc(c->interfaces_count, sizeof(Ref));

	for (int idx = 0; idx < interfaces_count; idx++) {
		Ref *ref = new Ref();
		ref->class_idx = read_host_ushort();
		ci->interfaces.push_back(ref);
	}

	int fields_count = read_host_ushort();

	for (int idx = 0; idx < fields_count; idx++) {
		Field *f = new Field();

		f->flags = read_host_ushort();
		f->name_idx = read_host_ushort();
		f->type_idx = read_host_ushort();

		int attrs_count = read_host_ushort();

		for (int aidx = 0; aidx < attrs_count; aidx++) {
			Attribute *a = new Attribute();
			parse_attribute(a);
			f->attrs.push_back(a);
		}

		ci->fields.push_back(f);
	}

	int methods_count = read_host_ushort();

	for (int idx = 0; idx < methods_count; idx++) {
		Method *m = new Method();

		m->flags = read_host_ushort();
		m->name_idx = read_host_ushort();
		m->type_idx = read_host_ushort();

		int attrs_count = read_host_ushort();

		
		for (int aidx = 0; aidx < attrs_count; aidx++) {
			Attribute *a = new Attribute();
			parse_attribute(a);
			m->attrs.push_back(a);
		}

		ci->methods.push_back(m);
	}

	int class_attributes_count = read_host_ushort();

	for (int caidx = 0; caidx < class_attributes_count; caidx++) {
		Attribute *a = new Attribute();
		parse_attribute(a);
		ci->attributes.push_back(a);
	}

	return ci;
}

bool ClassReader::parse_header(ClassInfo *ci)
{
	ci->minor_version = read_host_ushort();
	ci->major_version = read_host_ushort();
	ci->const_pool_count = read_host_ushort();

	return true;
}

bool ClassReader::parse_attribute(Attribute *attr)
{
	attr->name_idx = read_host_ushort();
	attr->length = read_host_uint();
	attr->info = read_host_string(attr->length);

	return true;
}

bool ClassReader::parse_const_pool(ClassInfo *ci)
{
	const int MAX_ITEMS = ci->const_pool_count - 1;
	uint32_t table_size_bytes = 0;
	int i, sz;
	std::string s;
	ConstantPoolTag tag_byte;
	Ref r;

	ci->items.push_back(new Item());	// CP #0

	for (i = 1; i <= MAX_ITEMS; i++) {
		tag_byte = (ConstantPoolTag) read_host_byte();

		if (((uint8_t)tag_byte) < MIN_CPOOL_TAG || ((uint8_t)tag_byte) > MAX_CPOOL_TAG) {
			fprintf(stderr, "Tag byte '%d' is outside permitted range %u to %u\n", tag_byte, MIN_CPOOL_TAG, MAX_CPOOL_TAG);
			table_size_bytes = 0;
			break; // fail fast
		}

		Item *item = new Item();

		item->tag = tag_byte;

		// Populate item based on tag_byte
		switch ((ConstantPoolTag) tag_byte) {
			case ConstantPoolTag::J_STRING_UTF8: // JString prefixed by a uint16 indicating the number of bytes in the encoded string which immediately follows
				sz = read_host_ushort();
				s = read_host_string(sz);
				item->value = s;
				table_size_bytes += 2 + sz;
				break;

			case ConstantPoolTag::J_INTEGER: // Integer: a signed 32-bit two's complement number in big-endian format
				item->value = read_host_int();
				table_size_bytes += 4;
				break;

			case ConstantPoolTag::J_FLOAT: // Float: a 32-bit single-precision IEEE 754 floating-point number
				item->value = read_host_float();
				table_size_bytes += 4;
				break;

			case ConstantPoolTag::J_LONG: // Long: a signed 64-bit two's complement number in big-endian format (takes two slots in the constant pool table)
				Long lng;
				lng.high = read_host_int();
				lng.low = read_host_int();
				item->value = lng;
				
				// 8-byte consts take 2 pool entries
				ci->items.push_back(new Item());
				++i;
				table_size_bytes += 8;
				break;

			case ConstantPoolTag::J_DOUBLE: // Double: a 64-bit double-precision IEEE 754 floating-point number (takes two slots in the constant pool table)
				Double dbl;
				dbl.high = read_host_int();
				dbl.low = read_host_int();
				item->value = dbl;

				// 8-byte consts take 2 pool entries
				ci->items.push_back(new Item());
				++i;
				table_size_bytes += 8;
				break;

			case ConstantPoolTag::J_CLASS: // Class reference: an uint16 within the constant pool to a UTF-8 string containing the fully qualified class name
				r.class_idx = read_host_ushort();
				r.name_idx = 0;
				item->value = r;

				table_size_bytes += 2;
				break;

			case ConstantPoolTag::J_STRING: // JString reference: an uint16 within the constant pool to a UTF-8 string
				r.class_idx = read_host_ushort();
				r.name_idx = 0;
				item->value = r;

				table_size_bytes += 2;
				break;

			case ConstantPoolTag::J_FIELD: // Field reference: two uint16 within the pool, 1st pointing to a Class reference, 2nd to a Name and Type descriptor
				/* FALL THROUGH TO METHOD */
			case ConstantPoolTag::J_METHOD: // Method reference: two uint16s within the pool, 1st pointing to a Class reference, 2nd to a Name and Type descriptor
				/* FALL THROUGH TO INTERFACE_METHOD */
			case ConstantPoolTag::J_INTERFACE_METHOD: // Interface method reference: 2 uint16 within the pool, 1st pointing to a Class reference, 2nd to a Name and Type descriptor
				r.class_idx = read_host_ushort();
				r.name_idx = read_host_ushort();
				item->value = r;

				table_size_bytes += 4;
				break;

			case ConstantPoolTag::J_NAME: // Name and type descriptor: 2 uint16 to UTF-8 strings, 1st representing a name (identifier), 2nd a specially encoded type descriptor
				r.class_idx = read_host_ushort();
				r.name_idx = read_host_ushort();
				item->value = r;

				table_size_bytes += 4;
				break;

			case ConstantPoolTag::J_INVOKE_DYNAMIC: // Name and type descriptor: 2 uint16, 1st representing bootstrap_method_attr_index, 2nd name_and_type_index
				r.class_idx = read_host_ushort();
				r.name_idx = read_host_ushort();
				item->value = r;
				table_size_bytes += 4;
				break;

			case ConstantPoolTag::J_METHOD_HANDLE: // Name and type descriptor: 1 byte + 1 uint16, 1st representing reference_kind, 2nd reference_index
				r.class_idx = read_host_byte();
				r.name_idx = read_host_ushort();
				item->value = r;
				table_size_bytes += 3;
				break;

			default:
				fprintf(stderr, "Found tag byte '%d' but don't know what to do with it\n", tag_byte);
				delete item;
				item = nullptr;
				break;
		}
		if (!item)
			return false;

		ci->items.push_back(item);
	}
	ci->pool_size_bytes = table_size_bytes;

	return true;
}

bool ClassReader::is_class()
{
	uint32_t magicNum = read_host_uint();
	return magicNum == 0xcafebabe;
}

std::string ClassInfo::getClassName()
{
	if (!this->this_class || !this->items.size() || this->this_class >= this->items.size())
		return std::string();

	Item *item = this->items.at(this_class);
	if (item->tag != ConstantPoolTag::J_CLASS)
		return std::string();

	Ref r = std::get<Ref>(item->value);
	int idx = r.class_idx;
	if (!idx || idx >= this->items.size())
		return std::string();

	item = this->items.at(idx);
	if (item->tag != ConstantPoolTag::J_STRING_UTF8)
		return std::string();

	return std::get<std::string>(item->value);
}

std::string ClassInfo::getMethodName(Method *m)
{
	if (!m->name_idx || m->name_idx >= this->items.size())
		return std::string();

	Item *name_item = this->items.at(m->name_idx);
	if (name_item->tag != ConstantPoolTag::J_STRING_UTF8)
		return std::string();

	return std::get<std::string>(name_item->value);
}

std::string ClassInfo::getMethodSignature(Method *m)
{
	std::string bfr = this->getMethodName(m);

	Item *signature_item = this->items[m->type_idx];
	if (signature_item->tag != ConstantPoolTag::J_STRING_UTF8)
		return std::string();

	std::string types = std::get<std::string>(signature_item->value);
	bfr += types;
	
	return bfr;
}

std::string ClassInfo::getFieldSignature(Field *f)
{
	std::string bfr = this->getFieldName(f);

	Item *signature_item = this->items[f->type_idx];
	if (signature_item->tag != ConstantPoolTag::J_STRING_UTF8)
		return std::string();

	std::string types = std::get<std::string>(signature_item->value);
	bfr += (" " + types);

	return bfr;
}

std::vector<std::string> ClassInfo::getMethodParameterTypes(Method *m)
{
	return std::vector<std::string>();
}

std::string ClassInfo::getFieldName(Field *f)
{
	if (!f->name_idx || f->name_idx >= this->items.size())
		return std::string();

	Item *name_item = this->items.at(f->name_idx);
	if (name_item->tag != ConstantPoolTag::J_STRING_UTF8)
		return std::string();

	return std::get<std::string>(name_item->value);
}

bool ClassInfo::isMethod(Item *item)
{
	return (item->tag == ConstantPoolTag::J_METHOD);
}

bool Method::isStatic()
{
	return this->flags & (uint16_t)AccessFlags::ACC_STATIC;
}

bool Method::isPublic()
{
	return this->flags & (uint16_t)AccessFlags::ACC_PUBLIC;
}

bool Field::isStatic()
{
	return this->flags & (uint16_t)AccessFlags::ACC_STATIC;
}

bool Field::isPublic()
{
	return this->flags & (uint16_t)AccessFlags::ACC_PUBLIC;
}

bool Field::isEnum()
{
	return this->flags & (uint16_t)AccessFlags::ACC_ENUM;
}

