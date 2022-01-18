#include "CILAsmDriver.h"
#include "libcpputils.h"
#include "cil-backend.h"

#include <string.h>

#include "defs.h"

bool CILAsmDriver::generateAsm()
{
	if (opts->input_files.size() != 1 || (!ends_with(opts->input_files[0], BC_SUFFIX) && !ends_with(opts->input_files[0], LL_SUFFIX))) {
		fprintf(stderr, "ERROR! Bad parameters in bc2j phase\n");
		return 1;
	}

	std::string bc_input_file = opts->input_files[0];

	// generate .j
	std::string j_prelink_outfile = !opts->output_files.empty() ? opts->output_files[0] :
		std::filesystem::path(bc_input_file).stem().string() + ILASM_PRELINK_SUFFIX;

	if (opts->dry_run || opts->verbose) {
		printf("Running cil_backend (infile: %s, outfile: %s, classname: %s)\n",
			bc_input_file.c_str(), j_prelink_outfile.c_str(), opts->classname.c_str());
	}


	if (!opts->dry_run) {
		int bc2j_rc = cil_backend(bc_input_file, j_prelink_outfile, opts->classname, opts);
		if (bc2j_rc) {
			fprintf(stderr, "ERROR! Could not generate .il file, call was: cil_backend (infile: %s, outfile: %s, classname: %s, debug_level: %d)\n",
				bc_input_file.c_str(), j_prelink_outfile.c_str(), opts->classname.c_str(), opts->debug_level);

			if (file_exists(j_prelink_outfile))
				remove(j_prelink_outfile.c_str());

			return bc2j_rc;
		}
	}

	opts->output_files.push_back(j_prelink_outfile);
	if (opts->performing_all_steps)
		opts->temp_files.push_back(j_prelink_outfile);

	return 0;
}

bool CILAsmDriver::linkAsm()
{
	if (opts->input_files.size() != 1 || (!ends_with(opts->input_files[0], JASMIN_SUFFIX) && !ends_with(opts->input_files[0], JASMIN_PRELINK_SUFFIX))) {
		fprintf(stderr, "ERROR! Bad parameters in jlink phase\n");
		return 1;
	}

	//std::string j_prelink_outfile = opts->input_files[0];
	//std::string j_outfile = !opts->outfile.empty() ? opts->outfile : string_replace(j_prelink_outfile, JASMIN_PRELINK_SUFFIX, JASMIN_SUFFIX);

	//JasminLinker jl;
	//jl.setRecursiveScan(opts->link_recursive_scan);
	//jl.setDebugLevel(opts->debug_level);
	//jl.setVerbose(opts->verbose);
	//jl.setUseKrakatau(opts->jvm_use_krakatau);
	//jl.setUseWideLdc(opts->jvm_wide_ldc);
	//bool rc_link = jl.performLink(j_prelink_outfile, j_outfile, opts->libs, opts->lib_dirs);
	//if (!rc_link) {
	//	fprintf(stderr, "ERROR! Link error\n");
	//	if (file_exists(j_prelink_outfile))
	//		remove(j_outfile.c_str());

	//	return 1;
	//}

	//opts->output_files.push_back(j_outfile);
	//if (opts->performing_all_steps)
	//	opts->temp_files.push_back(j_outfile);

	return 0;
}

bool CILAsmDriver::buildObject()
{
	if (opts->verbose)
		printf("Using ilasm to assemble the input file\n");

	if (opts->input_files.size() != 1 || !ends_with(opts->input_files[0], JASMIN_SUFFIX)) {
		fprintf(stderr, "ERROR! Bad parameters in j2class phase\n");
		return 1;
	}

	std::string j_outfile = opts->input_files[0];

	// compile .j ->.class

	auto v = getenv("KRAKATAU_HOME");
	if (!v || !strlen(v)) {
		fprintf(stderr, "ERROR! Environment variable KRAKATAU_HOME not set\n");
		return 1;
	}

	std::filesystem::path krakatau_home = std::filesystem::path(v);
	if (!std::filesystem::exists(krakatau_home)) {
		fprintf(stderr, "ERROR! Cannot locate Krakatau (%s)\n", krakatau_home.string().c_str());
		return 1;
	}

	std::vector<std::string> krakatau_cmd_items;

	krakatau_cmd_items.push_back("python");
	krakatau_cmd_items.push_back(krakatau_home.string() + PATH_SEPARATOR + "assemble.py");

	if (!opts->jvm_outbasedir.empty()) {
		krakatau_cmd_items.push_back("-out");
		krakatau_cmd_items.push_back(opts->jvm_outbasedir);
	}

	//if (opt_debug_level->value() > 0) {
	//	jasmin_cmd_items.push_back("-g");
	//}

	krakatau_cmd_items.push_back(j_outfile);

	std::string krakatau_cmd = vector_join(krakatau_cmd_items, " ");

	if (opts->dry_run || opts->verbose) {
		printf("%s\n", krakatau_cmd.c_str());
	}

	int krakatau_cmd_rc = system(krakatau_cmd.c_str());
	if (krakatau_cmd_rc) {
		fprintf(stderr, "ERROR! Comand was: %s\n", krakatau_cmd.c_str());
		return krakatau_cmd_rc;
	}

	//output_files.push_back(j_outfile);

	return 0;

}
