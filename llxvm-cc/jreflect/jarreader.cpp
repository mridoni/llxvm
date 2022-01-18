#include "jarreader.h"
#include "libcpputils.h"
#include "classreader.h"

#include <filesystem>

#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

JarReader::JarReader()
{

}

bool JarReader::read(const std::string &jar_path)
{
	if (!file_exists(jar_path))
		return false;

	void *buf = NULL;
	size_t bufsize;
	ClassReader cr;

	auto temp_dir = std::filesystem::temp_directory_path();
	zip_t *zip = zip_open(jar_path.c_str(), 0, 'r');

	int i, n = zip_entries_total(zip);
	for (i = 0; i < n; ++i) {
		zip_entry_openbyindex(zip, i);
		{
			std::string class_file_name;
			std::string entry_name = zip_entry_name(zip);
			int isdir = zip_entry_isdir(zip);
			if (isdir || !ends_with(entry_name, ".class"))
				continue;

			//unsigned long long size = zip_entry_size(zip);
			//unsigned int crc32 = zip_entry_crc32(zip);

			int pos = entry_name.find_last_of("/");
			if (pos == std::string::npos) {
				class_file_name = entry_name;
			}
			else {
				class_file_name = entry_name.substr(pos + 1);
			}

			auto fpath = temp_dir;
			fpath.append(string_replace(class_file_name, "/", PATH_SEP));

			if (zip_entry_fread(zip, fpath.string().c_str())) {
				fprintf(stderr, "ERROR! Cannot extract class file %s in JAR: %s\n", entry_name.c_str(), jar_path.c_str());
				continue;
			}


			ClassInfo *ci = cr.read(fpath.string());
			if (!ci) {
				fprintf(stderr, "ERROR! Cannot parse class file %s in JAR: %s\n", entry_name.c_str(), jar_path.c_str());
				continue;
			}

			auto classname = string_replace(ci->getClassName(), "/", ".");
			classes[classname] = ci;

			if (verbose) {
				printf("Read class %s\n", classname.c_str());
			}

			remove(fpath.string().c_str());

		}
		zip_entry_close(zip);
	}


	free(buf);

	return true;
}

void JarReader::processClass(zip_t *zip)
{

}

void JarReader::setVerbose(bool value)
{
	verbose = value;
}
