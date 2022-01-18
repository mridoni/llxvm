//========================================================================
// FILE:
//    StaticMain.cpp
//
// DESCRIPTION:
//    A command-line tool that counts all static calls (i.e. calls as seen
//    in the source code) in the input LLVM file. Internally it uses the
//    StaticCallCounter pass.
//
// USAGE:
//    # First, generate an LLVM file:
//      clang -emit-llvm <input-file> -c -o <output-llvm-file>
//    # Now you can run this tool as follows:
//      <BUILD/DIR>/bin/static <output-llvm-file>
//
// License: MIT
//========================================================================

#include "JVMWriter.h"

#include "llvm/IRReader/IRReader.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/InitializePasses.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/Scalarizer.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

/** Pass ID */
static char ID;

static RegisterPass<JVMWriter> tmp("JVMWriter", "Generate java bytecode");

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//
int jvm_backend(const std::string& input_file, const std::string& output_file, 
                    const std::string& classname, const GenerationOpts *_opts)
{
    // Makes sure llvm_shutdown() is called (which cleans up LLVM objects)
    //  http://llvm.org/docs/ProgrammersManual.html#ending-execution-with-llvm-shutdown
    //llvm_shutdown_obj SDO;

    // Parse the IR file passed on the command line.
    SMDiagnostic Err;
    LLVMContext Ctx;
    std::unique_ptr<Module> M = parseIRFile(input_file, Err, Ctx);

    // Initialize passes
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializeCore(Registry);
    initializeScalarOpts(Registry);
    initializeObjCARCOpts(Registry);
    initializeAnalysis(Registry);
    //initializeIPA(Registry);  // !!!!!!
    initializeTransformUtils(Registry);
    initializeInstCombine(Registry);
    initializeTarget(Registry);
    //initializeRegToMemPass(Registry);

    llvm::legacy::PassManager pm;
    DataLayout td("e-p:32:32:32"
        "-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64"
        "-f32:32:32-f64:64:64");

    M->setDataLayout(td);
    pm.add(createVerifierPass());
    pm.add(createGCLoweringPass());
    pm.add(createCFGSimplificationPass());

    // TODO: fix switch generation so the following pass is not needed
    pm.add(createLowerSwitchPass());
    pm.add(createLowerConstantIntrinsicsPass());    // NEW

	pm.add(createScalarizerPass());

    pm.add(new JVMWriter(&td, classname, output_file, _opts));
    //pm.add(createGCInfoDeleter());    // !!!!

    pm.run(*M);
    
    return 0;
}
