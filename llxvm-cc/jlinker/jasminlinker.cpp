#include "jasminlinker.h"
#include "libcpputils.h"
#include "classreader.h"
#include "jarreader.h"


JasminLinker::JasminLinker()
{

}

bool JasminLinker::performLink(const std::string &_input_file, const std::string &_output_file,
	const std::vector<std::string> libs, const std::vector<std::string> libdirs)
{
	input_file = _input_file;
	output_file = _output_file;
	
	if (verbose)
		printf("Using %s\n", use_krakatau ? "Krakatau syntax" : "Jasmin syntax");

	if (!file_exists(input_file))
		return false;

	is_error = false;
	for (const std::string &libdir : libdirs) {
		scan_dir(libdir);
	}

	if (is_error)
		return false;

	for (const std::string &lib : libs) {
		if (!scan_file(lib))
			return false;
	}

	try {
		buildMethodMap();
		buildFieldMap();

		in.open(input_file, std::ios_base::in);
		out.open(output_file, std::ios_base::out);

		readExtern();
		linkStatic();

		in.close();
		out.close();
	}
	// catch (std::exception ex) {
		// fprintf(stderr, "ERROR! %s\n", ex.what());
		// return false;
	// }	
	catch (std::runtime_error &ex) {
		fprintf(stderr, "ERROR! %s\n", ex.what());
		return false;
	}

	int uc = unresolved_instances_count;
	if (uc) {
		fprintf(stderr, "Unresolved symbol instances: %d\n", unresolved_instances_count);
		fprintf(stderr, "Unresolved symbols: %zu\n", unresolved_symbols.size());
		int i = 1;
		for (std::string s : unresolved_symbols) {
			fprintf(stderr, "#%06d: %s\n", i++, s.c_str());
		}
		return false;
	}

	printf("Link phase performed successfully\n");
	return true;
}

void JasminLinker::setVerbose(bool value)
{
	verbose = value;
}

void JasminLinker::setDebugLevel(int value)
{
	debug_level = value;
}

void JasminLinker::setRecursiveScan(bool value)
{
	recursive_scan = value;
}

void JasminLinker::setUseKrakatau(bool value)
{
	use_krakatau = value;
}

void JasminLinker::setUseWideLdc(bool value)
{
	wide_ldc = value;
}

void JasminLinker::scan_dir(const std::string &libdir)
{
	if (verbose)
		printf("Scanning directory %s\n", libdir.c_str());

	std::filesystem::path libdir_path(libdir);
	if (!std::filesystem::exists(libdir_path)) {
		if (verbose)
			printf("Directory %s does not exist\n", libdir.c_str());
		return;
	}

	for (auto const &child : std::filesystem::directory_iterator{ libdir_path }) {

		if (is_error)
			break;

		if (child.is_regular_file()) {
			if (verbose)
				printf("Examining file %s\n", child.path().string().c_str());

			if (child.path().extension() == ".class") {

				ClassReader cr;
				ClassInfo *ci = cr.read(child.path().string());
				if (!ci) {
					fprintf(stderr, "ERROR! Cannot parse .class file: %s\n", child.path().string().c_str());
					is_error = true;
					return;
				}

				auto classname = string_replace(ci->getClassName(), "/", ".");
				if (classname.empty()) {
					fprintf(stderr, "ERROR! Cannot parse .class file: %s\n", child.path().string().c_str());
					is_error = true;
					return;
				}

				lib_classes[classname] = ci;

				if (verbose) {
					printf("Read class %s\n", classname.c_str());
				}
			}

			if (child.path().extension() == ".jar") {

				if (!scan_jar(child.path().string())) {
					return;
				}

			}
		}

		if (child.is_directory() && recursive_scan) {
			scan_dir(child.path().string());
		}
	}
}


bool JasminLinker::scan_file(const std::string &filename)
{
	if (ends_with(filename, ".jar"))
		return scan_jar(filename);
	else
		if (ends_with(filename, ".j"))
			return scan_jasmin(filename);
		else {
			fprintf(stderr, "ERROR! Invalid file: %s\n", filename.c_str());
			is_error = true;
			return false;
		}
}

bool JasminLinker::scan_jar(const std::string &jarpath)
{
	JarReader jr;
	jr.setVerbose(this->verbose);
	if (!jr.read(jarpath)) {
		fprintf(stderr, "ERROR! Cannot parse .jar file: %s\n", jarpath.c_str());
		is_error = true;
		return false;
	}

	for (std::pair<std::string, ClassInfo *> cp : jr.classes) {
		lib_classes[cp.first] = cp.second;
	}

	return true;
}

bool JasminLinker::scan_jasmin(const std::string &jpath)
{
	if (jpath == input_file || jpath == output_file) {
		printf("WARNING: cannot link with either input or output file, skipping\n");
		is_error = false;
		return true;
	}

	if (!file_exists(jpath)) {
		fprintf(stderr, "ERROR: file %s does not exist\n", jpath.c_str());
		is_error = true;
		return false;
	}

	std::vector<std::string> lines = file_read_all_lines(jpath);
	std::vector<std::string> methods;
	std::vector<std::string> fields;
	std::string classname;

	for (std::string ln : lines) {
		ln = trim_copy(ln);

		if (starts_with(ln, ".class")) {
			std::vector<std::string> items = string_split(ln, " ");
			classname = items.back();
		}

		if (starts_with(ln, ".method")) {
			std::vector<std::string> items = string_split(ln, " ");
			if (vector_contains<std::string>(items, "public") && vector_contains<std::string>(items, "static")) {
				std::string signature;

				for (int n = 0; n < items.size(); n++) {
					std::string i = items[n];
					if (i != ".method" && i != "public" && i != "static" && i != "final") {
						if (!use_krakatau) {
							signature = i;
						}
						else {
							signature = i + items[n+2];
						}
						break;
					}
				}

				if (!signature.empty()) {
					if (verbose)
						printf("%s: found %s\n", jpath.c_str(), signature.c_str());
					
					methods.push_back(signature);
				}
			}
			else
				continue;
		}

		if (starts_with(ln, ".field")) {
			std::vector<std::string> items = string_split(ln, " ");
			if (vector_contains<std::string>(items, "public") && vector_contains<std::string>(items, "static")) {
				std::string signature;
				for (int n = 0; n < items.size(); n++) {
					std::string i = items[n];
					if (i != ".field" && i != "public" && i != "static" && i != "final") {
						signature = i + " " + items[n + 1];
						break;
					}
				}

				if (!signature.empty())
					fields.push_back(signature);
			}
			else
				continue;
		}

	}

	if (classname.empty())
		return false;

	for (auto m : methods)
		method_map[m] = classname;

	for (auto m : fields)
		field_map[m] = classname;

	return true;
}



void JasminLinker::buildMethodMap()
{
	// we only want public static methods
	for (auto const &it : lib_classes) {

		ClassInfo *ci = it.second;
		std::string classname = ci->getClassName();

		for (Method *m : ci->methods) {

			if (!m->isPublic() || !m->isStatic())
				continue;

			std::string method_sig = ci->getMethodSignature(m);

			if (!method_sig.empty()) {
				if (verbose)
					printf("Adding method to link target cache: %s@%s\n", method_sig.c_str(), classname.c_str());

				method_map[method_sig] = classname;
			}
		}
	}
}

void JasminLinker::buildFieldMap()
{
	// we only want public static fields
	for (auto const &it : lib_classes) {

		ClassInfo *ci = it.second;
		std::string classname = ci->getClassName();

		for (Field *f : ci->fields) {

			if (!f->isPublic() || !f->isStatic() || f->isEnum())
				continue;

			std::string field_signature = ci->getFieldSignature(f);

			if (!field_signature.empty()) {
				if (verbose)
					printf("Adding field to link target cache: %s@\%s\n", field_signature.c_str(), classname.c_str());

				field_map[field_signature] = classname;
			}
		}
	}
}

bool JasminLinker::read_line(std::string &line)
{
	std::getline(in, line);
	return in.good();
}

void JasminLinker::out_write(const std::string &s)
{
	out.write(s.c_str(), s.size());
}

std::string JasminLinker::safe_get(const std::map<std::string, std::string> &map, const std::string &key)
{
	if (map.find(key) == map.end())
		return std::string();

	return map.at(key);
}

/**
 * Read external reference directives from the input reader.
 *
 * @throws IOException  if there is a problem reading or writing
 */
void JasminLinker::readExtern()
{
	std::string line;
	while (read_line(line)) {

		line = trim_copy(line);
		std::vector<std::string> args = string_split(line, "\\s+");

		if (args[0] == ".extern") {
			if (args[1] == "field")
				extern_fields.push_back(args[2] + " " + args[3]);
			else if (args.at(1) == "method")
				extern_methods.push_back(args[2]);

			out_write(";");
		}
		out_write(line);
		out_write("\n");

		if (args.at(0) == ".method")
			break;
	}
}

/**
 * Print an invokestatic instruction for the given method.
 *
 * @param methodName    Operand of the instruction.
 * @param methodMap     Mapping of external methods to classes.
 * @throws IOException  if there is a problem reading or writing
 * @throws LinkError    if there is a problem linking external references
 */
void JasminLinker::printInvokeStatic(std::string methodName)
{
	if (methodName.find("(") == std::string::npos) { // non-prototyped function
		for (auto entry : method_map) {
			std::string name = entry.first;
			if (starts_with(name, methodName + "(")) {
				// TODO: throw error unless specified otherwise
				std::cerr << "WARNING: Function '" << methodName << "' should " <<
					"be declared with a prototype. Linking will " <<
					"succeed, but a runtime error will be thrown.";

				out_write("\tinvokestatic ");
				out_write(safe_get(method_map, name));
				out_write("/__non_prototyped__");
				out_write(methodName);
				out_write("()V\n");
				return;
			}
		}
	}

	std::string className = safe_get(method_map, methodName);
	if (className.empty()) {
		fprintf(stderr, "%s\n", std::string("Unable to find external method " + methodName).c_str());
		unresolved_instances_count++;
		unresolved_symbols.insert(methodName);
		return;
		// throw std::runtime_error(std::string("Unable to find external method " + methodName).c_str());
	}

	out_write("\tinvokestatic ");
	out_write(className);
	out_write("/");
	out_write(methodName);
	out_write("\n");

	if (verbose) {
		printf("%s.%s\n", className.c_str(), methodName.c_str());
	}
}

/**
 * Print an getstatic instruction for the given method.
 *
 * @param fieldName     Operand of the instruction.
 * @param fieldMap      Mapping of external fields to classes.
 * @throws IOException  if there is a problem reading or writing
 * @throws LinkError    if there is a problem linking external references
 */
void JasminLinker::printGetStatic(std::string fieldName)
{
	std::string className = safe_get(field_map, fieldName);
	if (className.empty()) {
		fprintf(stderr, "%s\n", std::string("Unable to find external field " + fieldName).c_str());
		unresolved_instances_count++;
		unresolved_symbols.insert(fieldName);
		return;
		// throw std::runtime_error(std::string("Unable to find external field " + fieldName).c_str());
	}
	out_write("\tgetstatic ");
	out_write(className);
	out_write("/");
	out_write(fieldName);
	out_write("\n");
}

/**
 * Print an ldc instruction to load the binary name of the parent class
 * of the given method.
 *
 * @param methodName    the method whose parent class to load
 * @param methodMap     Mapping of external methods to classes.
 * @throws IOException  if there is a problem reading or writing
 * @throws LinkError    if there is a problem linking external references
 */
void JasminLinker::printClassForMethod(std::string methodName)
{
	std::string className = safe_get(method_map, methodName);
	if (className.empty()) {
		fprintf(stderr, "%s\n", std::string("Unable to find external method " + methodName).c_str());
		unresolved_instances_count++;
		unresolved_symbols.insert(methodName);
		return;
		// throw std::runtime_error(std::string("Unable to find external method " + methodName).c_str());
	}

	if (!wide_ldc)
		out_write("\tldc \"");
	else
		out_write("\tldc2_w \"");

	out_write(className);
	out_write("\"\n");
}

/**
 * Link references to static methods and fields in the input to the
 * classes specified in the given maps.
 *
 * @param methodMap     Mapping of external methods to classes.
 * @param fieldMap      Mapping of external fields to classes.
 * @throws IOException  if there is a problem reading or writing
 * @throws LinkError    if there is a problem linking external references
 */
void JasminLinker::linkStatic()
{
	std::string line;
	int lineno = 0;
	while (read_line(line)) {
		lineno++;
		std::string tline = trim_copy(line);

		std::vector<std::string> args = string_split2(tline, ' ');

		if (args[0] == "invokestatic"
			&& vector_contains(extern_methods, args[1])) {
			if (verbose)
				printf("JLinker found (%d): %s\n", lineno, trim_copy(line).c_str());

			printInvokeStatic(args[1]);
		}
		else if (args[0] == "getstatic"
			&& vector_contains(extern_fields, args[1] + " " + args[2])) {
			if (verbose)
				printf("JLinker found (%d): %s\n", lineno, trim_copy(line).c_str());

			printGetStatic(args[1] + " " + args[2]);
		}
		else if (args[0] == "CLASSFORMETHOD"
			&& vector_contains(extern_methods, args[1])) {
			if (verbose)
				printf("JLinker found (%d): %s\n", lineno, trim_copy(line).c_str());

			printClassForMethod(args[1]);
		}
		else
			out_write(line + '\n');
	}
}
