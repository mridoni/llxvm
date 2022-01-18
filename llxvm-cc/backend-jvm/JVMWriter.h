#pragma once

//========================================================================
// FILE:
//    JVMWriter.h
//
// DESCRIPTION:
//    Declares the JVMWriter Passes
//      * new pass manager interface
//      * legacy pass manager interface
//      * printer pass for the new pass manager
//
// License: MIT
//========================================================================

#include "opts.h"

#include <llvm/ADT/MapVector.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FormattedStream.h>

#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/DenseSet.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/InstIterator.h>

#include <llvm/IR/Module.h>
#include <llvm/IR/Mangler.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/IR/GetElementPtrTypeIterator.h>

#include <set>

#define GOTO_TOKEN (opts->jvm_wide_gotos ? "goto_w" : "goto")
#define LDC_TOKEN  (opts->jvm_wide_ldc ? "ldc2_w" : "ldc")

using namespace llvm;

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------

class JVMWriter : public FunctionPass {

public:

    /** Pass ID */
    static char ID;

    JVMWriter();
    JVMWriter(DataLayout *td, const std::string &_classname, const std::string& outfile, const GenerationOpts *_opts);
    ~JVMWriter();

    void getAnalysisUsage(AnalysisUsage &au) const override;
    bool runOnFunction(Function &f) override;
    bool doInitialization(Module &m) override;
    bool doFinalization(Module &m) override;

    // Part of the official API:
    //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
    static bool isRequired() { return true; }

  StringRef getPassName() const override {
    return "JVMWriter";
  }

private:
    std::string output_filename;
    const GenerationOpts *opts;

    // A special type used by analysis passes to provide an address that
    // identifies that particular analysis pass type.
    static llvm::AnalysisKey Key;
    friend struct llvm::AnalysisInfoMixin<JVMWriter>;

    // JVMWriter specific (non-LLVM) stuff
    /** The output stream */
    raw_ostream *outstream = nullptr;
    /** The name of the source file */
    std::string sourcename;
    /** The binary name of the generated class */
    std::string classname;
    /** The current module */
    Module *module;
    /** The target data for the platform */
    DataLayout *targetData = nullptr;

    Mangler *mangler;

    /** Set of external references */
    DenseSet<const Value *> externRefs;
    /** Mapping of blocks to unique IDs */
    DenseMap<const BasicBlock *, unsigned int> blockIDs;
    /** Mapping of values to local variable numbers */
    DenseMap<const Value *, unsigned int> localVars;
    /** Number of registers allocated for the function */
    unsigned int usedRegisters;
    /** Local variable number of the pointer to the packed list of varargs */
    unsigned int vaArgNum;
    /** Current instruction number */
    unsigned int instNum;

    // block.cpp
    void printBasicBlock(const BasicBlock *block);
    void printInstruction(const Instruction *inst);
    void printNegInstruction(const Value *op);

    // branch.cpp
    void printPHICopy(const BasicBlock *src, const BasicBlock *dest);
    void printBranchInstruction(const BasicBlock *curBlock,
                                const BasicBlock *destBlock);
    void printBranchInstruction(const BasicBlock *curBlock,
                                const BasicBlock *trueBlock,
                                const BasicBlock *falseBlock);
    void printBranchInstruction(const BranchInst *inst);
    void printSelectInstruction(const Value *cond,
        const Value *trueVal,
        const Value *falseVal);
    void printSwitchInstruction(const SwitchInst *inst);
    void printLoop(const Loop *l);
    void rewriteBranch(const std::string &orig_op, const std::string &orig_label);

    // const.cpp
    void printPtrLoad(uint64_t n);
    void printConstLoad(const APInt &i);
    void printConstLoad(float f);
    void printConstLoad(double d);
    void printConstLoad(const Constant *c);
    void printConstLoad(const std::string &str, bool cstring);
    void printStaticConstant(const Constant *c);
    void printConstantExpr(const ConstantExpr *ce);

    // function.cpp
    std::string getCallSignature(const FunctionType *ty);
    void printOperandPack(const Instruction *inst,
        unsigned int minOperand,
        unsigned int maxOperand);
    void printFunctionCall(const Value *functionVal, const Instruction *inst);
    void printIntrinsicCall(const IntrinsicInst *inst);
    void printCallInstruction(const Instruction *inst);
    void printInvokeInstruction(const InvokeInst *inst);
    void printLocalVariable(const Function &f, const Instruction *inst);
    void printFunctionBody(const Function &f);
    unsigned int getLocalVarNumber(const Value *v);
    void printCatchJump(unsigned int numJumps);
    void printFunction(const Function &f);

    // instruction.cpp
    void printCmpInstruction(unsigned int predicate,
        const Value *left,
        const Value *right);
    void printArithmeticInstruction(unsigned int op,
        const Value *left,
        const Value *right);
    void printBitCastInstruction(const Type *ty, const Type *srcTy);
    void printCastInstruction(const std::string &typePrefix,
        const std::string &srcTypePrefix);
    void printCastInstruction(unsigned int op, const Value *v,
        const Type *ty, const Type *srcTy);
    void printGepInstruction(const Value *v, gep_type_iterator i, gep_type_iterator e);
    void printAllocaInstruction(const AllocaInst *inst);
    void printVAArgInstruction(const VAArgInst *inst);
    void printVAIntrinsic(const IntrinsicInst *inst);
    void printMemIntrinsic(const MemIntrinsic *inst);
    void printMathIntrinsic(const IntrinsicInst *inst);
    void printBitIntrinsic(const IntrinsicInst *inst);
    void printArithmeticIntrinsic(const IntrinsicInst *inst);
    void printIndirectBr(const IndirectBrInst *inst);

    // loadstore.cpp
    void printValueLoad(const Value *v);
    void printValueStore(const Value *v);
    void printIndirectLoad(const Value *v);
    void printIndirectLoad(const Type *ty);
    void printIndirectStore(const Value *ptr, const Value *val);
    void printIndirectStore(const Type *ty);

    // name.cpp
    std::string sanitizeName(std::string name);
    std::string getValueName(const Value *v);
    std::string getLabelName(const BasicBlock *block);

    // printinst.cpp
    void printBinaryInstruction(const char *name,
        const Value *left,
        const Value *right);
    void printBinaryInstruction(const std::string &name,
        const Value *left,
        const Value *right);
    void printSimpleInstruction(const char *inst);
    void printSimpleInstruction(const char *inst, const char *operand);
    void printSimpleInstruction(const std::string &inst);
    void printSimpleInstruction(const std::string &inst,
        const std::string &operand);
    void printVirtualInstruction(const char *sig);
    void printVirtualInstruction(const char *sig, const Value *operand);
    void printVirtualInstruction(const char *sig,
        const Value *left,
        const Value *right);
    void printVirtualInstruction(const std::string &sig);
    void printVirtualInstruction(const std::string &sig, const Value *operand);
    void printVirtualInstruction(const std::string &sig,
        const Value *left,
        const Value *right);
    void printLabel(const char *label);
    void printLabel(const std::string &label);

    // sections.cpp
    void printHeader();
    void printFields();
    void printExternalMethods();
    void printConstructor();
    void printClInit();
    void printMainMethod();

    // types.cpp
    unsigned int getBitWidth(const Type *ty, bool expand = false);
    char getTypeID(const Type *ty, bool expand = false);
    std::string getTypeName(const Type *ty, bool expand = false);
    std::string getTypeDescriptor(const Type *ty, bool expand = false);
    std::string getTypePostfix(const Type *ty, bool expand = false);
    std::string getTypePrefix(const Type *ty, bool expand = false);

    unsigned int cur_basicblock_map_id = 100;
    std::map<std::string, std::pair<unsigned int, const BasicBlock *>> basicblock_map;

    unsigned int cur_rewrite_br_lbl_id = 700;

    std::string last_label;

    void get_locals_count(const Function *f, int *count);
    int examine_var(const Function *f, const  Value *v, std::set<const Value *> &vars);

    bool canUseTableLookup(const SwitchInst *i, int *low, int *high);
};

#if 0
//------------------------------------------------------------------------------
// New PM interface for the printer pass
//------------------------------------------------------------------------------
class JVMWriterPrinter
    : public llvm::PassInfoMixin<JVMWriterPrinter>
{
public:
    explicit JVMWriterPrinter(llvm::raw_ostream &OutS) : OS(OutS) {}
    llvm::PreservedAnalyses run(llvm::Module &M,
        llvm::ModuleAnalysisManager &MAM);
    // Part of the official API:
    //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
    static bool isRequired() { return true; }

private:
    llvm::raw_ostream &OS;
};

//------------------------------------------------------------------------------
// Legacy PM interface
//------------------------------------------------------------------------------
struct LegacyJVMWriter : public llvm::ModulePass
{
    static char ID;
    LegacyJVMWriter() : llvm::ModulePass(ID) {}
    bool runOnModule(llvm::Module &M) override;
    // The print method must be implemented by Legacy analysis passes in order to
    // print a human readable version of the analysis results:
    //    http://llvm.org/docs/WritingAnLLVMPass.html#the-print-method
    void print(llvm::raw_ostream &OutS, llvm::Module const *M) const override;

    ResultStaticCC DirectCalls;
    JVMWriter Impl;
};
#endif
