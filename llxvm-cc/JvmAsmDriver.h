#pragma once

#include "AsmDriver.h"

class JvmAsmDriver : public AsmDriver
{
public:
	// Inherited via AsmDriver
	virtual bool generateAsm() override;
	virtual bool linkAsm() override;
	virtual bool buildObject() override;

private:
	int do_j2class_jasmin();
	int do_j2class_krakatau();
};

