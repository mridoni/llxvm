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

#include "JVMWriter.h"

/**
 * Return the bit width of the given type.
 * 
 * @param ty      the type
 * @param expand  specifies whether to expand the type to 32 bits
 * @return        the bit width
 */
unsigned int JVMWriter::getBitWidth(const Type *ty, bool expand) {
    if(ty->getTypeID() == Type::ArrayTyID
#if LLVM_VERSION_MAJOR > 10
    || ty->getTypeID() == Type::FixedVectorTyID
#else
    || ty->getTypeID() == Type::VectorTyID
#endif
    || ty->getTypeID() == Type::StructTyID
    || ty->getTypeID() == Type::PointerTyID)
        return 32;

    if (ty->getTypeID() == Type::X86_FP80TyID) {
        errs() << "WARNING: unsupported type x86_fp80 (80 bit length), changed to double (64 bit length)" << '\n';
        return 64;
    }

    unsigned int n = ty->getPrimitiveSizeInBits();
    switch(n) {
    case 1:
    case 8:
    case 16:
    case 32: if(expand) return 32;
    case 64: return n;
    default:
        
        if (n < 32) {
            errs() << "WARNING: non standard bit length for integer type (" << n << "), changed to 32" << '\n';
            return 32;
        }

        if (n < 64) {
            errs() << "WARNING: non standard bit length for integer type (" << n << "), changed to 64" << '\n';
            return 64;
        }

        errs() << "Bits = " << n << '\n';
        llvm_unreachable("Unsupported integer width");
    }
}

/**
 * Return the ID of the given type.
 * 
 * @param ty      the type
 * @param expand  specifies whether to expand the type to 32 bits
 * @return        the type ID
 */
char JVMWriter::getTypeID(const Type *ty, bool expand) {
    switch(ty->getTypeID()) {
    case Type::VoidTyID:
        return 'V';
    case Type::IntegerTyID:
        switch(getBitWidth(ty, expand)) {
        case  1: return 'Z';
        case  8: return 'B';
        case 16: return 'S';
        case 32: return 'I';
        case 64: return 'J';
        }
    case Type::FloatTyID:
        return 'F';
    case Type::DoubleTyID:
    case Type::X86_FP80TyID:
        return 'D';
    case Type::PointerTyID:
    case Type::StructTyID:
    case Type::ArrayTyID:
#if LLVM_VERSION_MAJOR > 10
    case Type::FixedVectorTyID:
#else
    case Type::VectorTyID:
#endif
        return 'I';
    default:
        errs() << "Type = " << ty << '\n';
        llvm_unreachable("Invalid type");
    }
}
/**
 * Return the name of the given type.
 * 
 * @param ty      the type
 * @param expand  specifies whether to expand the type to 32 bits
 * @return        the type name
 */
std::string JVMWriter::getTypeName(const Type *ty, bool expand) {
    switch(getTypeID(ty, expand)) {
    case 'V': return "void";
    case 'Z': return "boolean";
    case 'B': return "byte";
    case 'S': return "short";
    case 'I': return "int";
    case 'J': return "long";
    case 'F': return "float";
    case 'D': return "double";
    default:
        errs() << "TypeID = " << ty->getTypeID() << '\n';
        llvm_unreachable("Invalid type");

    }
}

/**
 * Return the type descriptor of the given type.
 * 
 * @param ty      the type
 * @param expand  specifies whether to expand the type to 32 bits
 * @return        the type descriptor
 */
std::string JVMWriter::getTypeDescriptor(const Type *ty, bool expand) {
    return std::string() + getTypeID(ty, expand);
}

/**
 * Return the type postfix of the given type.
 * 
 * @param ty      the type
 * @param expand  specifies whether to expand the type to 32 bits
 * @return        the type postfix
 */
std::string JVMWriter::getTypePostfix(const Type *ty, bool expand) {
    switch(ty->getTypeID()) {
    case Type::VoidTyID:
        return "void";
    case Type::IntegerTyID:
        return "i" + utostr(getBitWidth(ty, expand));
    case Type::FloatTyID:
        return "f32";
    case Type::DoubleTyID:
    case Type::X86_FP80TyID:
        return "f64";
    case Type::PointerTyID:
    case Type::StructTyID:
    case Type::ArrayTyID:
#if LLVM_VERSION_MAJOR > 10
    case Type::FixedVectorTyID:
#else
    case Type::VectorTyID:
#endif
        return "i32";
    default:
        errs() << "TypeID = " << ty->getTypeID() << '\n';
        llvm_unreachable("Invalid type");
    }
}

/**
 * Return the type prefix of the given type.
 * 
 * @param ty      the type
 * @param expand  specifies whether to expand the type to 32 bits
 * @return        the type prefix
 */
std::string JVMWriter::getTypePrefix(const Type *ty, bool expand) {
    switch(getTypeID(ty, expand)) {
    case 'Z':
    case 'B': return "b";
    case 'S': return "s";
    case 'I': return "i";
    case 'J': return "l";
    case 'F': return "f";
    case 'D': return "d";
    case 'V': llvm_unreachable("void has no prefix");
    default:
        errs() << "TypeID = " << ty->getTypeID() << '\n';
        llvm_unreachable("Invalid type");


    }
}
