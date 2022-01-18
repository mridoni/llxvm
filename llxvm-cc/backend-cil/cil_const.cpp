/*
* Copyright (c) 2009 David Roberts <d@vidr.cc>
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

#include "CILWriter.h"
#include <cstdio>

#define __Module_Pointer32 4

using namespace llvm;

#if LLVM_VERSION_MAJOR > 10
static std::string __apint_to_string(const APInt &i)
{
    SmallString<32> s;
    i.toString(s, 10, true);
    return s.str().str();
}
#else
static std::string __apint_to_string(const APInt &i)
{
    return i.toString(10, true);
}
#endif
/**
 * Load the given pointer.
 * 
 * @param n  the value of the pointer
 */
void CILWriter::printPtrLoad(uint64_t n) {
    if(module->getDataLayout().getPointerSize() != __Module_Pointer32)
        llvm_unreachable("Only 32-bit pointers are allowed");
    printConstLoad(APInt(32, n, false));
}

/**
 * Load the given integer.
 * 
 * @param i  the integer
 */
void CILWriter::printConstLoad(const APInt &i) {
    if(i.getBitWidth() <= 32) {
        int64_t value = i.getSExtValue();
        if(value == -1)
            printSimpleInstruction("ldc.i4.m1");
        else if(0 <= value && value <= 5)
            printSimpleInstruction("ldc.i4." + __apint_to_string(i));
        else if(-0x80 <= value && value <= 0x7f)
            printSimpleInstruction("ldc.i4.s", __apint_to_string(i));
        else if(-0x8000 <= value && value <= 0x7fff)
            printSimpleInstruction("ldc.i4", __apint_to_string(i)); // No load short in CIL
        else
            printSimpleInstruction("ldc.i4", __apint_to_string(i));
    } else {
        if(i == 0)
            printSimpleInstruction("ldc.i4.0");
        else if(i == 1)
            printSimpleInstruction("ldc.i4.1");
        else {
            printSimpleInstruction("ldc.i4", __apint_to_string(i));
        }
    }
}

static inline std::string ftostr(double V) {
	char Buffer[200], *B = Buffer;
	sprintf(Buffer, "%20.6e", V);
	while (*B == ' ') ++B;
	return B;
}

/**
 * Load the given single-precision floating point value.
 * 
 * @param f  the value
 */
void CILWriter::printConstLoad(float f) {
    if(f == 0.0)
        printSimpleInstruction("fconst_0");
    else if(f == 1.0)
        printSimpleInstruction("fconst_1");
    else if(f == 2.0)
        printSimpleInstruction("fconst_2");
    else if(std::isnan(f))
        printSimpleInstruction("getstatic", "java/lang/Float/NaN F");
    else if(f == std::numeric_limits<float>::infinity())
        printSimpleInstruction("getstatic",
                               "java/lang/Float/POSITIVE_INFINITY F");
    else if(f == -std::numeric_limits<float>::infinity())
        printSimpleInstruction("getstatic",
                               "java/lang/Float/NEGATIVE_INFINITY F");
    else
        printSimpleInstruction("ldc.i4", ftostr(f));
}

/**
 * Load the given double-precision floating point value.
 * 
 * @param d  the value
 */
void CILWriter::printConstLoad(double d) {
    if(d == 0.0)
        printSimpleInstruction("dconst_0");
    else if(d == 1.0)
        printSimpleInstruction("dconst_1");
    else if(std::isnan(d))
        printSimpleInstruction("getstatic", "java/lang/Double/NaN D");
    else if(d == std::numeric_limits<double>::infinity())
        printSimpleInstruction("getstatic",
                               "java/lang/Double/POSITIVE_INFINITY D");
    else if(d == -std::numeric_limits<double>::infinity())
        printSimpleInstruction("getstatic",
                               "java/lang/Double/NEGATIVE_INFINITY D");
    else
        printSimpleInstruction("ldc2_w", ftostr(d));
}

/**
 * Load the given constant.
 * 
 * @param c  the constant
 */
void CILWriter::printConstLoad(const Constant *c) {
    if(const ConstantInt *i = dyn_cast<ConstantInt>(c)) {
        printConstLoad(i->getValue());
    } else if(const ConstantFP *fp = dyn_cast<ConstantFP>(c)) {
        if(fp->getType()->getTypeID() == Type::FloatTyID)
            printConstLoad(fp->getValueAPF().convertToFloat());
        else
            printConstLoad(fp->getValueAPF().convertToDouble());
    }
    else if (isa<UndefValue>(c)) {
        printPtrLoad(0);
    }
    else if (isa<BlockAddress>(c)) {
        const BlockAddress *ba = dyn_cast<BlockAddress>(c);

        std::string bbname = ba->getOperand(1)->getName().str();
        if (basicblock_map.find(bbname) == basicblock_map.end()) {
            errs() << "Invalid blockstore label ID: " + bbname << '\n';
            llvm_unreachable(("Invalid blockstore label ID: " + bbname).c_str());
        }

        unsigned int bsid = basicblock_map[bbname].first;
        printSimpleInstruction("ldc.i4", std::to_string(bsid));
    } else {
        errs() << "Constant = " << *c << '\n';
		fprintf(stderr, "WARNING: Invalid constant value (TypeID : %d)\n", c->getType()->getTypeID());
		// llvm_unreachable(bfr);
		printPtrLoad(0);
    }
}

/**
 * Load the given string.
 * 
 * @param str      the string
 * @param cstring  true iff the string contains a single null character at the
 *                 end
 */
void CILWriter::printConstLoad(const std::string &str, bool cstring) {
    *outstream << "\t" << "ldstr" << " \"";
    if(cstring)
        for(std::string::const_iterator i = str.begin(),
                                        e = str.end()-1; i != e; i++)
            switch(*i) {
            case '\\': *outstream << "\\\\"; break;
            case '\b': *outstream << "\\b";  break;
            case '\t': *outstream << "\\t";  break;
            case '\n': *outstream << "\\n";  break;
            case '\f': *outstream << "\\f";  break;
            case '\r': *outstream << "\\r";  break;
            case '\"': *outstream << "\\\""; break;
            case '\'': *outstream << "\\\'"; break;
            default:   *outstream << *i;     break;
            }
    else
        for(std::string::const_iterator i = str.begin(),
                                        e = str.end(); i != e; i++) {
            const char c = *i;
            *outstream << "\\u00" << hexdigit((c>>4) & 0xf) << hexdigit(c & 0xf);
        }
    *outstream << "\"\n";
}

/**
 * Store the given static constant. The constant is stored to the address
 * currently on top of the stack, pushing the first address following the
 * constant onto the stack afterwards.
 * 
 * @param c  the constant
 */
void CILWriter::printStaticConstant(const Constant *c) {
    if(isa<ConstantAggregateZero>(c) || c->isNullValue()) {
        // zero initialised constant
        printPtrLoad(targetData->getTypeAllocSize(c->getType()));
        //printSimpleInstruction("invokestatic", "llxvm/runtime/Memory/zero(II)I");
        printSimpleInstruction("call int32", "LLXVM.runtime.Memory::zero(int32, int32)");
        return;
    }
    std::string typeDescriptor = getTypeDescriptor(c->getType());
    switch(c->getType()->getTypeID()) {
    case Type::IntegerTyID:
    case Type::FloatTyID:
    case Type::DoubleTyID:
        printConstLoad(c);
        //printSimpleInstruction("invokestatic", "llxvm/runtime/Memory/pack(I" + typeDescriptor + ")I");
        printSimpleInstruction("call int32", "LLXVM.runtime.Memory::pack(int32, " + typeDescriptor + ")");
        break;
    case Type::ArrayTyID:
        if(const ConstantDataSequential *ca = dyn_cast<ConstantDataSequential>(c)) {
            if(ca->isString()) {
                bool cstring = ca->isCString();
#if LLVM_VERSION_MAJOR > 10
                printConstLoad(ca->getAsString().str(), cstring);
#else
                printConstLoad(ca->getAsString(), cstring);
#endif
                if (cstring) {
                    //printSimpleInstruction("invokestatic", "llxvm/runtime/Memory/pack(ILjava/lang/String;)I");
                    printSimpleInstruction("call int32", "LLXVM.runtime.Memory::pack(string)");
                }
                else {
                    //printSimpleInstruction("invokevirtual", "java/lang/String/toCharArray()[C");
                    printSimpleInstruction("callvirtual", "char[] java/lang/String/toCharArray()[C");

                    //printSimpleInstruction("invokestatic", "llxvm/runtime/Memory/pack(I[C)I");
                    printSimpleInstruction("call i32", "LLXVM.runtime.Memory::pack(int32, char[])");
                }
            } else {
                for(unsigned int i = 0, e = ca->getNumElements(); i < e; i++)
                    printStaticConstant(ca->getElementAsConstant(i));
            }
            break;
        }
        // else fall through
#if LLVM_VERSION_MAJOR > 10
    case Type::FixedVectorTyID:
#else
    case Type::VectorTyID:
#endif
    case Type::StructTyID:
        for(unsigned int i = 0, e = c->getNumOperands(); i < e; i++)
            printStaticConstant(cast<Constant>(c->getOperand(i)));
        break;
    case Type::PointerTyID:
        if(const Function *f = dyn_cast<Function>(c))
            printValueLoad(f);
        else if(const GlobalVariable *g = dyn_cast<GlobalVariable>(c))
            // initialise with address of global variable
            printValueLoad(g);
        else if(isa<ConstantPointerNull>(c) || c->isNullValue())
            printSimpleInstruction("iconst.0");
        else if(const ConstantExpr *ce = dyn_cast<ConstantExpr>(c))
            printConstantExpr(ce);
        else {
            errs() << "Constant = " << *c << '\n';
            llvm_unreachable("Invalid static initializer");
        }
        //printSimpleInstruction("invokestatic", "llxvm/runtime/Memory/pack(I" + typeDescriptor + ")I");
        printSimpleInstruction("call int32", "LLXVM.runtime.Memory::pack(int32, " + typeDescriptor + ")");
        break;
    default:
        errs() << "TypeID = " << c->getType()->getTypeID() << '\n';
        llvm_unreachable("Invalid type in printStaticConstant()");
    }
}

/**
 * Print the given constant expression.
 * 
 * @param ce  the constant expression
 */
void CILWriter::printConstantExpr(const ConstantExpr *ce) {
    const Value *left, *right;
    if(ce->getNumOperands() >= 1) left  = ce->getOperand(0);
    if(ce->getNumOperands() >= 2) right = ce->getOperand(1);
    switch(ce->getOpcode()) {
    case Instruction::Trunc:
    case Instruction::ZExt:
    case Instruction::SExt:
    case Instruction::FPTrunc:
    case Instruction::FPExt:
    case Instruction::UIToFP:
    case Instruction::SIToFP:
    case Instruction::FPToUI:
    case Instruction::FPToSI:
    case Instruction::PtrToInt:
    case Instruction::IntToPtr:
    case Instruction::BitCast:
        printCastInstruction(ce->getOpcode(), left,
                             ce->getType(), left->getType()); break;
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
        printArithmeticInstruction(ce->getOpcode(), left, right); break;
    case Instruction::ICmp:
    case Instruction::FCmp:
        printCmpInstruction(ce->getPredicate(), left, right); break;
    case Instruction::GetElementPtr:
        printGepInstruction(ce->getOperand(0),
                            gep_type_begin(ce),
                            gep_type_end(ce)); break;
    case Instruction::Select:
        printSelectInstruction(ce->getOperand(0),
                               ce->getOperand(1),
                               ce->getOperand(2)); break;
    default:
        errs() << "Expression = " << *ce << '\n';
        llvm_unreachable("Invalid constant expression");
    }
}
