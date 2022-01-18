#pragma once

#include "opts.h"

int cil_backend(const std::string& input_file, const std::string& output_file, 
					const std::string& classname, const GenerationOpts *_opts);
