#include "AsmDriver.h"
#include "JvmAsmDriver.h"
#include "CILAsmDriver.h"

#define MODE_JVM	1
#define MODE_IL	2

AsmDriver *AsmDriver::get(int mode, GenerationOpts *_opts)
{
    AsmDriver *res = nullptr;
    switch (mode) {
        case MODE_JVM:
            res = new JvmAsmDriver();
            res->opts = _opts;
            break;

        case MODE_IL:
            res = new CILAsmDriver();
            res->opts = _opts;
            break;

    }
    return res;
}
