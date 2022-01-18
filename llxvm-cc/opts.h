#pragma once

#include <string>
#include <vector>

struct GenerationOpts
{
	int debug_level = 0;
	int object_ver = 0;
	bool verbose = false;
	bool dry_run = false;
	bool link_recursive_scan = false;
	bool performing_all_steps;
	std::string classname;
	//std::string outfile;

	std::vector<std::string> input_files;
	std::vector<std::string> output_files;
	std::vector<std::string> temp_files;

	std::vector<std::string> libs;
	std::vector<std::string> lib_dirs;

	std::vector<std::string> clang_passthru_opts;

	// JVM
	std::string jvm_outbasedir;
	
	bool jvm_use_krakatau = false;
	bool jvm_rewrite_branches = false;
	bool jvm_wide_gotos = false;
	bool jvm_wide_ldc = false;
	bool jvm_skip_locals_init = false;

	// IL
	// ...

	void dump();
};
