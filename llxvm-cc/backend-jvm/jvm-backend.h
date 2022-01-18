#pragma once

#include "opts.h"

int jvm_backend(const std::string& input_file, const std::string& output_file, 
					const std::string& classname, const GenerationOpts *_opts);
