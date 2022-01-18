#include <map>
#include <vector>
#include <string>

#include "popl.hpp"
#include "libcpputils.h"
#include "opts.h"
#include "defs.h"
#include "AsmDriver.h"


using namespace popl;

GenerationOpts opts;

std::shared_ptr<popl::Switch> opt_help = nullptr;

// Select JVM/IL mode
std::shared_ptr<popl::Value<std::string>> opt_mode = nullptr;

// General options
std::shared_ptr<popl::Switch> opt_compile_only = nullptr;
std::shared_ptr<popl::Switch> opt_merge_bc_only = nullptr;
std::shared_ptr<popl::Value<std::string>> opt_classname = nullptr;
std::shared_ptr<popl::Value<int>> opt_debug_level = nullptr;
std::shared_ptr<popl::Switch> opt_keep = nullptr;
std::shared_ptr<popl::Value<std::string>> opt_lib = nullptr;
std::shared_ptr<popl::Value<std::string>> opt_lib_dir = nullptr;
std::shared_ptr<popl::Switch> opt_link_recursive_scan = nullptr;
std::shared_ptr<popl::Value<std::string>> opt_outfile = nullptr;
std::shared_ptr<popl::Switch> opt_dry_run = nullptr;
std::shared_ptr<popl::Switch> opt_gen_text_ir = nullptr;
std::shared_ptr<popl::Switch> opt_verbose = nullptr;
std::shared_ptr<popl::Value<std::string>> opt_object_ver = nullptr;
std::shared_ptr<popl::Switch> opt_genasm_only = nullptr;
std::shared_ptr<popl::Switch> opt_asmlink_only = nullptr;
std::shared_ptr<popl::Switch> opt_buildobj_only = nullptr;

// Java Options
std::shared_ptr<popl::Value<std::string>> opt_jvm_outbasedir = nullptr;
std::shared_ptr<popl::Switch> opt_jvm_use_krakatau = nullptr;
std::shared_ptr<popl::Switch> opt_jvm_rewrite_branches = nullptr;
std::shared_ptr<popl::Switch> opt_jvm_wide_gotos = nullptr;
std::shared_ptr<popl::Switch> opt_jvm_wide_ldc = nullptr;
std::shared_ptr<popl::Switch> opt_jvm_skip_locals_init = nullptr;


// IL options

static int do_compile();
static int do_merge_bc();


#define PERFORM_OPT_STEP(STEP, OPT) retval = 0; if (perform_all_steps || OPT->is_set()) { opts.output_files.clear(); retval = STEP(); opts.input_files = opts.output_files; } if (retval || OPT->is_set()) { return retval ? 1 : 0; } 

static int retval = 0;
static bool perform_all_steps = false;

static std::map<std::string, int> classfile_versions = {
	{ "1.0.2", 45 }, { "1.1", 45 }, { "1.2", 46 }, { "1.3", 47 }, { "1.4", 48 },
	{ "5.0", 49 }, { "6", 50 }, { "7", 51 }, { "8", 52 },
	{ "9", 53 }, { "10", 54 }, { "11", 55 }, { "12", 56 }, { "13", 57 },
	{ "14", 58 }, { "15", 59 }, { "16", 60 }
};

static std::map<std::string, int> il_versions = {
	{ "2.0", 20 }, { "4.0", 40 }, { "4.5", 45 }, { "4.6", 46 }, { "4.7", 47 }, { "4.8", 48 }
};

int main(int argc, char **argv)
{
	int mode = 0;
	char vbfr[256];

	sprintf(vbfr, "llxvm - LLXVM C driver\nBuilds a JVM/.Net object file from one or more C sources\nVersion: %s\n\nOptions", LLXVM_VER);

	OptionParser options(vbfr);

	opt_help = options.add<Switch>("h", "help", "displays help on commandline options");

	opt_mode = options.add<Value<std::string>>("M", "mode", "Mode: (J/jvm or N/cil/.net/dotnet)");

	opt_compile_only = options.add<Switch>("c", "compile", "only compile C files");
	opt_merge_bc_only = options.add<Switch>("m", "merge-bc", "merge bitcode filess");
	opt_genasm_only = options.add<Switch>("j", "bc2asm", "complle .bc files to .j/.il");
	opt_asmlink_only = options.add<Switch>("a", "link", "link and resolve .j files against Java class libraries");

	opt_classname = options.add<Value<std::string>>("C", "classname", "class name");
	opt_debug_level = options.add<Value<int>>("g", "debug", "debug level");
	opt_keep = options.add<Switch>("K", "keep", "keep temporary files");
	opt_lib = options.add<Value<std::string>>("l", "lib", "link with/resolve against Java class library");
	opt_lib_dir = options.add<Value<std::string>>("L", "libdir", "link library search path");
	opt_outfile = options.add<Value<std::string>>("o", "outfile", "output file");
	opt_jvm_outbasedir = options.add<Value<std::string>>("O", "javaout", "output base directory for class files");
	opt_dry_run = options.add<Switch>("y", "dry-run", "dry-run (only prints commands)");
	opt_gen_text_ir = options.add<Switch>("T", "llvm-ir-as-text", "also generate LLVM IR in text format");
	opt_verbose = options.add<Switch>("v", "verbose", "verbose");
	opt_link_recursive_scan = options.add<Switch>("r", "link-scan-recursive", "scan recursively when searching for Java class libraries");
	opt_object_ver = options.add<Value<std::string>>("V", "object-version", "JVM class file version (1.0.2, 1.1, 1.2, 1.3, 1.4, 5.0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16)\n"
							".Net framework version (2.0, 4.0, 4.5, 4.6, 4.7, 4.8)");

	opt_jvm_outbasedir->set_default(".");

	opt_jvm_use_krakatau = options.add<Switch>("U", "use_krakatau", "JVM mode: use Krakatau instead of Jasmin when generating or assembling .j files");
	opt_jvm_rewrite_branches = options.add<Switch>("B", "rewrite-branches", "JVM mode: rewrite branches to avoid exceeding the 16 bit offset limit. Implies -W");
	opt_jvm_wide_gotos = options.add<Switch>("W", "wide_gotos", "JVM mode: use wide gotos (goto_w, 32 bit offset) instead of standard goto (goto, 16 bit offset)");
	opt_jvm_wide_ldc = options.add<Switch>("N", "wide_ldc", "JVM mode: always use ldc2_w instead of ldc");
	opt_jvm_skip_locals_init = options.add<Switch>("S", "skip-locals-init", "JVM mode: skip initialization of local variables in .j files");
	opt_buildobj_only = options.add<Switch>("k", "j2class", "JVM mode: uild .class file from .j");

    opt_debug_level->set_default(0);

	options.parse(argc, argv);

	bool opts_set = opt_help->is_set() || 
		opt_compile_only->is_set() || opt_merge_bc_only->is_set() || opt_genasm_only->is_set() || opt_asmlink_only->is_set() || opt_buildobj_only->is_set() ||
		opt_classname->is_set() || opt_debug_level->is_set() || opt_keep->is_set() || opt_lib->is_set() ||
		opt_lib_dir->is_set() || opt_outfile->is_set() || opt_jvm_outbasedir->is_set() || opt_dry_run->is_set() ||
		opt_gen_text_ir->is_set() || opt_verbose->is_set() || opt_link_recursive_scan->is_set() || (options.non_option_args().size() > 0);

	perform_all_steps = !opt_compile_only->is_set() && !opt_merge_bc_only->is_set() && !opt_genasm_only->is_set() && !opt_asmlink_only->is_set() && !opt_buildobj_only->is_set();

	if (opt_help->is_set() || !opts_set) {
		std::cout << options << std::endl;
        return 0;
	}

	if (!opt_mode->is_set() && !getenv("LLXVM_MODE")) {
        fprintf(stderr, "ERROR! you must set a working mode (J/java or N/cil/.net/dotnet");
		return -1;
	}
	else {
		std::string smode = string_to_lower(opt_mode->value());
		if (smode.empty())
			smode = std::string(getenv("LLXVM_MODE"));

		if (smode == "J" || smode == "jvm") {
			mode = MODE_JVM;
			opt_object_ver->set_default(DEFAULT_CLASSFILE_VER);
		}
		else {
			if (smode == "N" || smode == "cil" || smode == ".net" || smode == "dotnet") {
				mode = MODE_IL;
				opt_object_ver->set_default(DEFAULT_IL_VER);
			}
			else {
				fprintf(stderr, "ERROR! invalid mode: you must set a working mode (J/java or N/cil/.net/dotnet");
				return -1;
			}
		}
	}

    if (opt_debug_level->value() < 0 || opt_debug_level->value() > 3) {
        fprintf(stderr, "ERROR! the debug level must be between 0 and 3 (default is 1)");
		return -1;
    }

	if ((opt_genasm_only->is_set() || perform_all_steps) && !opt_classname->is_set()) {
		fprintf(stderr, "ERROR! you must set a class name (-C)");
		return -1;
	}

	if (mode == MODE_JVM) {
		if (opt_object_ver->is_set() && classfile_versions.find(opt_object_ver->value()) == classfile_versions.end()) {
			fprintf(stderr, "ERROR! the classfile version must be one of: 1.0.2, 1.1, 1.2, 1.3, 1.4, 5.0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 (default: %s)", DEFAULT_CLASSFILE_VER);
			return -1;
		}
	}
	else {
		if (opt_object_ver->is_set() && il_versions.find(opt_object_ver->value()) == classfile_versions.end()) {
			fprintf(stderr, "ERROR! the classfile version must be one of: 2.0, 4.0, 4.5, 4.6, 4.7, 4.8 (default: %s)", DEFAULT_IL_VER);
			return -1;
		}
	}
    
	opts.input_files = options.non_option_args();
	opts.clang_passthru_opts = options.unknown_options();

	int n_exclusive_opts = (opt_compile_only->is_set() ? 1 : 0) + (opt_merge_bc_only->is_set() ? 1 : 0 ) + (opt_genasm_only->is_set() ? 1 : 0 ) + (opt_asmlink_only->is_set() ? 1 : 0 ) + (opt_buildobj_only->is_set());
	if (!perform_all_steps && n_exclusive_opts > 1) {
        fprintf(stderr, "ERROR! You can provide only one of the -c, -m, -j, -a, -k options\n");
		return -1;
	}

	PERFORM_OPT_STEP(do_compile, opt_compile_only);
	PERFORM_OPT_STEP(do_merge_bc, opt_merge_bc_only);

	if (opts.output_files.size() > 0) {
		opts.input_files = opts.output_files;
	}

	if (mode == MODE_JVM) {
		opts.object_ver = classfile_versions[opt_object_ver->value()];
	}
	else {
		opts.object_ver = il_versions[opt_object_ver->value()];
	}

	opts.verbose = opt_verbose->is_set();
	opts.dry_run = opt_dry_run->is_set();
	opts.performing_all_steps = perform_all_steps;
	opts.debug_level = opt_debug_level->value();
	opts.classname = opt_classname->value();
	opts.link_recursive_scan = opt_link_recursive_scan->is_set();

	if (opt_outfile->is_set()) {
		if (opts.input_files.size() > 1) {
			fprintf(stderr, "ERROR! You cannot use the -o option here\n");
			return -1;
		}
	}
	
	opts.output_files.clear();
	if (opt_outfile->is_set())
		opts.output_files.push_back(opt_outfile->value());

	opts.jvm_outbasedir = opt_jvm_outbasedir->value();
	opts.jvm_use_krakatau = opt_jvm_use_krakatau->is_set();
	opts.jvm_rewrite_branches = opt_jvm_rewrite_branches->is_set();
	opts.jvm_wide_gotos = opt_jvm_wide_gotos->is_set() || opt_jvm_rewrite_branches->is_set();
	opts.jvm_wide_ldc = opt_jvm_wide_ldc->is_set();
	opts.jvm_skip_locals_init = opt_jvm_skip_locals_init->is_set();

	for (int i = 0; i < opt_lib->count(); i++) opts.libs.push_back((opt_lib->value(i)));
	for (int i = 0; i < opt_lib_dir->count(); i++) opts.lib_dirs.push_back((opt_lib_dir->value(i)));

	AsmDriver *asm_driver = AsmDriver::get(mode, &opts);

	PERFORM_OPT_STEP(asm_driver->generateAsm, opt_genasm_only);
	PERFORM_OPT_STEP(asm_driver->linkAsm, opt_asmlink_only);
	PERFORM_OPT_STEP(asm_driver->buildObject, opt_buildobj_only);

    if (!opt_keep->is_set()) {
        for (std::string f : opts.temp_files) {
            if (opt_dry_run->is_set() || opt_verbose->is_set()) {
                printf("Removing temporary file %s\n", f.c_str());
            }
            
            if (!opt_dry_run->is_set())
                remove(f.c_str());
        }
    }
    
	if (opt_verbose->is_set()) {
		printf("jlink-cc finished without errors, now exiting\n");
	}
	return 0;
}

// 1
int do_compile()
{
    std::string CC = getenv("CC") ? getenv("CC") : "clang";
	std::string CFLAGS = getenv("CFLAGS") ? getenv("CFLAGS") : "";

	std::vector<std::string> args;
	args.push_back(CC);
	args.push_back("-flto -c -D__LLJVM__ -fno-builtin -m32 -mno-sse -mno-x87 -emit-llvm ");
#ifdef WIN32
	args.push_back("-D_NO_CRT_STDIO_INLINE=1 ");
#endif 
	args.push_back(CFLAGS);
	args.push_back(vector_join(opts.clang_passthru_opts, " ", true));

	if (opts.input_files.size() > 1 && opt_outfile->is_set() && !perform_all_steps) {
		fprintf(stderr, "ERROR! You can use the -o option only when linking or if you have only one source file\n");
		return 1;
	}

	for (std::string file : opts.input_files) {
		if (!ends_with(file, ".c") && !ends_with(file, ".C")) {
			fprintf(stderr, "ERROR! Bad file name: %s\n", file.c_str());
			return 1;
		}

        std::string bc_outfile;
		std::vector<std::string>file_args(args);
		file_args.push_back(file);

		if (opts.input_files.size() == 1 && opt_outfile->is_set() && !perform_all_steps) {
			bc_outfile = opt_outfile->value();
		}
		else {
			bc_outfile = filename_change_ext(file, BC_SUFFIX);
		}

		file_args.push_back("-o");
		file_args.push_back(bc_outfile);

		std::string cmd = vector_join(file_args, " ", true);
		if (opt_dry_run->is_set() || opt_verbose->is_set()) {
			printf("%s\n", cmd.c_str());
		}

		if (!opt_dry_run->is_set()) {
			int cmd_rc = system(cmd.c_str());
			if (cmd_rc) {
				fprintf(stderr, "ERROR! (%d) Comand was: [%s], exiting\n", cmd_rc, cmd.c_str());
				return cmd_rc;
			}
		}
        
		opts.output_files.push_back(bc_outfile);

		if (perform_all_steps)
			opts.temp_files.push_back(bc_outfile);

		if (opt_gen_text_ir->is_set()) {
			file_args = args;
			file_args.push_back(file);
			file_args.push_back("-S");
			file_args.push_back("-o");
			file_args.push_back(filename_change_ext(bc_outfile, LL_SUFFIX));

			cmd = vector_join(file_args, " ", true);
			if (opt_dry_run->is_set() || opt_verbose->is_set()) {
				printf("%s\n", cmd.c_str());
			}

			if (!opt_dry_run->is_set()) {
				int cmd_rc = system(cmd.c_str());
				if (cmd_rc) {
					fprintf(stderr, "ERROR! Comand was: %s\n", cmd.c_str());
					return cmd_rc;
				}
			}
		}
	}

	return 0;
}

// 2
int do_merge_bc()
{
	std::vector<std::string> cmd_items;
	std::string bc_outfile;

	if (!perform_all_steps && !opt_outfile->is_set()) {
		fprintf(stderr, "ERROR! Output file not set, cannot generate .bc library\n");
		return 1;
	}

	if (!opt_classname->is_set()) {
		fprintf(stderr, "ERROR! Class name not set, cannot generate merged .bc file\n");
		return 1;
	}

	// Just to be safe
	for (std::string file : opts.input_files) {
		if (!ends_with(file, ".bc") && !ends_with(file, ".BC")) {
			fprintf(stderr, "ERROR! Bad file name: %s\n", file.c_str());
			return 1;
		}
	}

	if (opt_merge_bc_only->is_set()) {
		bc_outfile = opt_outfile->value();
	}
	else {
		std::string classleaf;
		if (opt_classname->value().find(".") == std::string::npos) {
			classleaf = opt_classname->value();
		}
		else {
			classleaf = string_split(opt_classname->value(), "\\.").back();
		}
		bc_outfile = opt_classname->value() + ".bc";
	}

	// here we link all .bc files into a single .bc file (llvm-link)
    std::string LLVMLD = getenv("LLVMLD") ? getenv("LLVMLD") : "llvm-link";
	
	cmd_items.push_back(LLVMLD);
	cmd_items.push_back(vector_join(opts.input_files, " ", true));
	cmd_items.push_back("-o");
	cmd_items.push_back(bc_outfile);

	std::string cmd = vector_join(cmd_items, " ", true);
	if (opt_dry_run->is_set() || opt_verbose->is_set()) {
		printf("%s\n", cmd.c_str());
	}

	if (!opt_dry_run->is_set()) {
		int cmd_rc = system(cmd.c_str());
		if (cmd_rc) {
			fprintf(stderr, "ERROR! Comand was: %s\n", cmd.c_str());
			return 1;
		}
	}

	opts.output_files.push_back(bc_outfile);

	return 0;

}

