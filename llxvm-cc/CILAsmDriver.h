#pragma once

#include "AsmDriver.h"

class CILAsmDriver : public AsmDriver
{
	// Inherited via AsmDriver
	virtual bool generateAsm() override;
	virtual bool linkAsm() override;
	virtual bool buildObject() override;
};

