//==============================================================================
// FILE:
//    CILWriter.cpp
//
//==============================================================================
#include "CILWriter.h"
#include "libcpputils.h"

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

char CILWriter::ID = 0;

//------------------------------------------------------------------------------
// CILWriter Implementation
//------------------------------------------------------------------------------

CILWriter::CILWriter() : FunctionPass(ID), outstream(&fouts())
{}

CILWriter::CILWriter(DataLayout *td, const std::string &_classname, const std::string &outfile, const GenerationOpts *_opts) : FunctionPass(ID)
{
	targetData = td;
	auto dotpos = _classname.find(".");
	if (dotpos == std::string::npos) {
		classname = _classname;
		classnamespace = "";
	}
	else {
		classname = _classname.substr(dotpos + 1);
		classnamespace = _classname.substr(0, dotpos);
	}
	classname = _classname;
	opts = _opts;

	output_filename = outfile;

	std::error_code ec;
	outstream = new raw_fd_ostream(StringRef(output_filename), ec, sys::fs::FileAccess::FA_Write);
}

CILWriter::~CILWriter()
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
void CILWriter::getAnalysisUsage(AnalysisUsage &au) const
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
bool CILWriter::runOnFunction(Function &f)
{
	if (!f.isDeclaration() && !f.hasAvailableExternallyLinkage())
		printFunction(f);

	printFooter();

	return false;
}

/**
 * Perform per-module initialization.
 *
 * @param m  the module
 * @return   whether the module was modified (always false)
 */
bool CILWriter::doInitialization(Module &m)
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
	auto dbg_outstream = new raw_fd_ostream(modID + "-pre-CILWriter.ll", ec, sys::fs::FileAccess::FA_Write);
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
bool CILWriter::doFinalization(Module &m)
{
	return false;
}

void GenerationOpts::dump()
{
	//printf("debug_level      : %d\n", debug_level);
	//printf("rewrite_branches : %s\n", rewrite_branches ? "true" : "false");
	//printf("wide_gotos       : %s\n", wide_gotos ? "true" : "false");
	//printf("wide_ldc         : %s\n", wide_ldc ? "true" : "false");
	//printf("use_krakatau     : %s\n", use_krakatau ? "true" : "false");
}