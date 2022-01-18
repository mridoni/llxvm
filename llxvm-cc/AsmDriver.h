#pragma once

#include "opts.h"

class AsmDriver
{
public:
	static AsmDriver *get(int mode, GenerationOpts *_opts);

	virtual bool generateAsm() = 0;
	virtual bool linkAsm() = 0;
	virtual bool buildObject() = 0;

protected:
	GenerationOpts *opts = nullptr;
};

