/*
* Copyright (c) 2009-2010 David Roberts <d@vidr.cc>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "JVMWriter.h"

#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/DebugInfoMetadata.h>

using namespace llvm;

/**
 * Print the given basic block.
 * 
 * @param block  the basic block
 */
void JVMWriter::printBasicBlock(const BasicBlock *block) {
    printLabel(getLabelName(block));
    for(BasicBlock::const_iterator i = block->begin(), e = block->end();
        i != e; i++) {
        instNum++;
        if(opts->debug_level >= 3) {
            // print current instruction as comment
            // note that this block of code significantly increases
            // code generation time
            std::string str;
            raw_string_ostream ss(str); 
            ss << *i;
            std::string::size_type pos = 0;
            while((pos = str.find("\n", pos)) != std::string::npos)
                str.replace(pos++, 1, "\n;");

            *outstream << ';' << str << '\n';
        }

        int line = instNum;

        if (opts->debug_level >= 2) {
            const llvm::DebugLoc &debugInfo = i->getDebugLoc();

            if (debugInfo) {
#if LLVM_VERSION_MAJOR > 10
                std::string directory = debugInfo->getDirectory().str();
                std::string filePath = debugInfo->getFilename().str();
#else
                std::string directory = debugInfo->getDirectory();
                std::string filePath = debugInfo->getFilename();
#endif
                line = debugInfo->getLine();
                int column = debugInfo->getColumn();

                if (line && column) {
                    *outstream << "; <S>: " << line << "," << column << "@" << directory << "/" << filePath << "\n";
                }
            }
        }

        //if (i->hasMetadataOtherThanDebugLoc()) {
        //    int n = 99;
        //    SmallVector<std::pair<unsigned, MDNode *>, 8> MDs;
        //    i->getAllMetadataOtherThanDebugLoc(MDs);
        //    for (unsigned i = 0, e = MDs.size(); i != e; ++i) {
        //        auto t = MDs[i].first;
        //        MDNode *N = MDs[i].second;
        //        N->dump();
        //    }
        //}

        if (opts->debug_level >= 1 && instNum) {
            //printSimpleInstruction(".line", utostr(line));   // This is practically useless, but just in case...
        }       

        if(i->getOpcode() == Instruction::PHI)
            // don't handle phi instruction in current block
            continue;
        printInstruction(dyn_cast<Instruction>(i));
        if(i->getType() != Type::getVoidTy(block->getContext())
        && i->getOpcode() != Instruction::Invoke)
            // instruction doesn't return anything, or is an invoke instruction
            // which handles storing the return value itself
            printValueStore(dyn_cast<Instruction>(i));
    }
}

/**
 * Print the given instruction.
 * 
 * @param inst  the instruction
 */
void JVMWriter::printInstruction(const Instruction *inst) {
    const Value *left = nullptr, *right = nullptr;
    if(inst->getNumOperands() >= 1) left  = inst->getOperand(0);
    if(inst->getNumOperands() >= 2) right = inst->getOperand(1);
    switch(inst->getOpcode()) {
    case Instruction::Ret:
        printSimpleInstruction("invokestatic",
                               "llxvm/runtime/Memory/destroyStackFrame()V");
        if(inst->getNumOperands() >= 1) {
            printValueLoad(left);
            printSimpleInstruction(
                getTypePrefix(left->getType(), true) + "return");
        } else {
            printSimpleInstruction("return");
        }
        break;
#if 0
    case Instruction::Unwind:
        printSimpleInstruction("getstatic",
            "llxvm/runtime/Instruction$Unwind/instance "
            "Lllxvm/runtime/Instruction$Unwind;");
        printSimpleInstruction("athrow");
        // TODO: need to destroy stack frames
        break;
#endif
    case Instruction::Unreachable:
        printSimpleInstruction("getstatic",
            "llxvm/runtime/Instruction$Unreachable/instance "
            "Lllxvm/runtime/Instruction$Unreachable;");
        printSimpleInstruction("athrow");
        break;
    case Instruction::Add:
    case Instruction::FAdd:
    case Instruction::Sub:
    case Instruction::FSub:
    case Instruction::Mul:
    case Instruction::FMul:
    case Instruction::UDiv:
    case Instruction::SDiv:
    case Instruction::FDiv:
    case Instruction::URem:
    case Instruction::SRem:
    case Instruction::FRem:
    case Instruction::And:
    case Instruction::Or:
    case Instruction::Xor:
    case Instruction::Shl:
    case Instruction::LShr:
    case Instruction::AShr:
        printArithmeticInstruction(inst->getOpcode(), left, right);
        break;
    case Instruction::FNeg:
        printNegInstruction(left);
        break;
    case Instruction::SExt:
    case Instruction::Trunc:
    case Instruction::ZExt:
    case Instruction::FPTrunc:
    case Instruction::FPExt:
    case Instruction::UIToFP:
    case Instruction::SIToFP:
    case Instruction::FPToUI:
    case Instruction::FPToSI:
    case Instruction::PtrToInt:
    case Instruction::IntToPtr:
    case Instruction::BitCast:
        printCastInstruction(inst->getOpcode(), left,
                             cast<CastInst>(inst)->getDestTy(),
                             cast<CastInst>(inst)->getSrcTy()); break;
    case Instruction::ICmp:
    case Instruction::FCmp:
        printCmpInstruction(cast<CmpInst>(inst)->getPredicate(),
                            left, right); break;
    case Instruction::Br:
        printBranchInstruction(cast<BranchInst>(inst)); break;
    case Instruction::Select:
        printSelectInstruction(inst->getOperand(0),
                               inst->getOperand(1),
                               inst->getOperand(2)); break;
    case Instruction::Load:
        printIndirectLoad(inst->getOperand(0)); break;
    case Instruction::Store:
        printIndirectStore(inst->getOperand(1), inst->getOperand(0)); break;
    case Instruction::GetElementPtr:
        printGepInstruction(inst->getOperand(0),
                            gep_type_begin(inst),
                            gep_type_end(inst)); break;
    case Instruction::Call:
        printCallInstruction(cast<CallInst>(inst)); break;
    case Instruction::Invoke:
        printInvokeInstruction(cast<InvokeInst>(inst)); break;
    case Instruction::Switch:
        printSwitchInstruction(cast<SwitchInst>(inst)); break;
    case Instruction::Alloca:
        printAllocaInstruction(cast<AllocaInst>(inst)); break;
    case Instruction::VAArg:
        printVAArgInstruction(cast<VAArgInst>(inst)); break;

    case Instruction::IndirectBr:
        printIndirectBr(cast<IndirectBrInst>(inst)); break;
#if 0		
    case Instruction::ExtractElement:
        fprintf(stderr, "WARNING: unsupported instruction (extractelement)\n");
    case Instruction::InsertElement:
        fprintf(stderr, "WARNING: unsupported instruction (insertelement)\n");		
        break;
#endif		
    default:
        errs() << "Instruction = " << *inst << '\n';
        llvm_unreachable("Unsupported instruction");
    }
}

void JVMWriter::printNegInstruction(const Value *op)
{
    printValueLoad(op);
    std::string typePrefix = getTypePrefix(op->getType(), true);
    printSimpleInstruction(typePrefix + "neg");
}
