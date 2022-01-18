//==============================================================================
// FILE:
//    JVMWriter.cpp
//
//==============================================================================
#include "JVMWriter.h"

//#include "llvm/Passes/PassBuilder.h"
//#include "llvm/Passes/PassPlugin.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

//// Pretty-prints the result of this analysis
//static void printStaticCCResult(llvm::raw_ostream &OutS,
//    const ResultStaticCC &DirectCalls);

char JVMWriter::ID = 0;

//------------------------------------------------------------------------------
// JVMWriter Implementation
//------------------------------------------------------------------------------

JVMWriter::JVMWriter() : FunctionPass(ID), outstream(&fouts())
{}

JVMWriter::JVMWriter(DataLayout *td, const std::string &_classname, const std::string &outfile, const GenerationOpts *_opts) : FunctionPass(ID)
{
	targetData = td;
	classname = _classname;
	opts = _opts;

	output_filename = outfile;

	std::error_code ec;
	outstream = new raw_fd_ostream(StringRef(output_filename), ec, sys::fs::FileAccess::FA_Write);
}

JVMWriter::~JVMWriter()
{
	if (outstream) {
		raw_fd_ostream *fs = (raw_fd_ostream *)outstream;
		fs->flush();
		fs->close();
	}
}


/**
 * Register required analysis information.
 *
 * @param au  AnalysisUsage object representing the analysis usage information
 *            of this pass.
 */
void JVMWriter::getAnalysisUsage(AnalysisUsage &au) const
{
	au.addRequired<LoopInfoWrapperPass>();
	au.setPreservesAll();
}

/**
 * Process the given function.
 *
 * @param f  the function to process
 * @return   whether the function was modified (always false)
 */
bool JVMWriter::runOnFunction(Function &f)
{
	if (!f.isDeclaration() && !f.hasAvailableExternallyLinkage())
		printFunction(f);

	return false;
}

/**
 * Perform per-module initialization.
 *
 * @param m  the module
 * @return   whether the module was modified (always false)
 */
bool JVMWriter::doInitialization(Module &m)
{
	module = &m;
	instNum = 0;

	std::string modID = module->getModuleIdentifier();
	size_t slashPos = modID.rfind('/');
	if (slashPos == std::string::npos)
		sourcename = modID;
	else
		sourcename = modID.substr(slashPos + 1);

	if (!classname.empty()) {
		for (std::string::iterator i = classname.begin(),
			e = classname.end(); i != e; i++)
			if (*i == '.') *i = '/';
	}
	else {
		classname = sourcename.substr(0, sourcename.rfind('.'));
		for (std::string::iterator i = classname.begin(),
			e = classname.end(); i != e; i++)
			if (*i == '.') *i = '_';
	}

#if _DEBUG
	std::error_code ec;
	auto dbg_outstream = new raw_fd_ostream(modID + "-pre-jvmwriter.ll", ec, sys::fs::FileAccess::FA_Write);
	m.print(*dbg_outstream, nullptr);
#endif

	printHeader();
	printFields();
	printExternalMethods();
	printConstructor();
	printClInit();
	printMainMethod();
	return false;
}

/**
 * Perform per-module finalization.
 *
 * @param m  the module
 * @return   whether the module was modified (always false)
 */
bool JVMWriter::doFinalization(Module &m)
{
	return false;
}
