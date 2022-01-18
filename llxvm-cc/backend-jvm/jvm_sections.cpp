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

/**
 * Print the header.
 */
void JVMWriter::printHeader()
{
	std::string srcfile = (opts->debug_level >= 2) ? module->getSourceFileName() : sourcename;
	
	if (!opts->jvm_use_krakatau)
		*outstream << ".source " << srcfile << "\n";
	
	if (opts->jvm_use_krakatau) {
		*outstream << ".version " << opts->object_ver << " 0\n";
	}
	
	*outstream << ".class public final " << classname << "\n"
		".super java/lang/Object\n\n";

	
	if (opts->jvm_use_krakatau) {
		*outstream << ".sourcefile \"" << srcfile << "\"\n";
	}
}

/**
 * Print the field declarations.
 */
void JVMWriter::printFields()
{
	*outstream << "; Fields\n";
	for (Module::global_iterator i = module->global_begin(),
		e = module->global_end(); i != e; i++) {
		if (i->isDeclaration()) {
			*outstream << ".extern field ";
			externRefs.insert(dyn_cast<Value>(i));
		}
		else
			*outstream << ".field "
			<< (i->hasLocalLinkage() ? "private " : "public ")
			<< "static final ";
		*outstream << getValueName(dyn_cast<Value>(i)) << ' ' << getTypeDescriptor(i->getType());
		if (opts->debug_level >= 3)
			*outstream << " ; " << *i << '\n';
		else
			*outstream << '\n';
	}
	*outstream << '\n';
}

/**
 * Print the list of external methods.
 */
void JVMWriter::printExternalMethods()
{
	*outstream << "; External methods\n";
	for (Module::const_iterator i = module->begin(),
		e = module->end(); i != e; i++) {
		if (i->isDeclaration() && !i->isIntrinsic()) {
			const Function *f = dyn_cast<Function>(i);
			const FunctionType *ty = f->getFunctionType();
			*outstream << ".extern method "
				<< getValueName(f) << getCallSignature(ty);
			if (opts->debug_level >= 3)
				*outstream << " ; " << *ty;
			*outstream << '\n';
			externRefs.insert(f);
		}
	}
	*outstream << '\n';
}

/**
 * Print the class constructor.
 */
void JVMWriter::printConstructor()
{
	*outstream << "; Constructor\n";

	if (!opts->jvm_use_krakatau) {
		*outstream << ".method private <init>()V\n";
		*outstream << ".limit stack 1\n";
		*outstream << ".limit locals 1\n\n";
	
	}
	else {
		*outstream << ".method private <init> : ()V\n";
		*outstream << "\t.code stack 1 locals 1\n\n";
	}
	
	*outstream << "\t\taload_0\n"
		"\t\tinvokespecial java/lang/Object/<init>()V\n"
		"\t\treturn\n";
		
	if (opts->jvm_use_krakatau) {	
		*outstream << "\t.end code\n";
	}
	
	*outstream << ".end method\n\n";
}

/**
 * Print the static class initialization method.
 */
void JVMWriter::printClInit()
{ 
	if (!opts->jvm_use_krakatau) {
		*outstream << ".method static <clinit>()V\n";
		printSimpleInstruction(".limit stack 4");
		printSimpleInstruction(".limit locals 16");
	}
	else {
		*outstream << ".method static <clinit> : ()V\n";
		*outstream << ".code stack 4 locals 16\n\n";
	}

	*outstream << "\n\t; allocate global variables\n";
	for (Module::global_iterator i = module->global_begin(),
		e = module->global_end(); i != e; i++) {
		if (!i->isDeclaration()) {
			const GlobalVariable *g = dyn_cast<GlobalVariable>(i);
			const Constant *c = g->getInitializer();
			printConstLoad(APInt(32, targetData->getTypeAllocSize(c->getType()), false));
			printSimpleInstruction("invokestatic", "llxvm/runtime/Memory/allocateData(I)I");
			printSimpleInstruction("putstatic", classname + "/" + getValueName(g) + " I");
		}
	}

	*outstream << "\n\t; initialise global variables\n";
	for (Module::global_iterator i = module->global_begin(),
		e = module->global_end(); i != e; i++) {
		if (!i->isDeclaration()) {
			const GlobalVariable *g = dyn_cast<GlobalVariable>(i);
			const Constant *c = g->getInitializer();
			printSimpleInstruction("getstatic",
				classname + "/" + getValueName(g) + " I");
			printStaticConstant(c);
			printSimpleInstruction("pop");
			*outstream << '\n';
		}
	}

	printSimpleInstruction("return");
	
	if (opts->jvm_use_krakatau) {	
		*outstream << ".end code\n\n";
	}
	
	*outstream << ".end method\n\n";
}

/**
 * Print the main method.
 */
void JVMWriter::printMainMethod()
{
	const Function *f = module->getFunction("main");
	if (!f || f->isDeclaration())
		return;

	if (!opts->jvm_use_krakatau)
		*outstream << ".method public static main([Ljava/lang/String;)V\n";
	else
		*outstream << ".method public static main : ([Ljava/lang/String;)V\n";

	if (opts->jvm_use_krakatau) {	
		printSimpleInstruction(".code stack 16 locals 256");
	}
	else {
		printSimpleInstruction(".limit stack 4");
		printSimpleInstruction(".limit locals 16");
	}

	if (f->arg_size() == 0) {
		printSimpleInstruction("invokestatic", classname + "/main()I");
	}
	else if (f->arg_size() == 2) {
		Function::const_arg_iterator arg1, arg2;
		arg1 = arg2 = f->arg_begin(); arg2++;
		if (!arg1->getType()->isIntegerTy()
			|| arg2->getType()->getTypeID() != Type::PointerTyID)
			llvm_unreachable("main function has invalid type signature");
		printSimpleInstruction("aload_0");
		printSimpleInstruction("arraylength");
		printSimpleInstruction("aload_0");
		printSimpleInstruction("invokestatic",
			"llxvm/runtime/Memory/storeStack([Ljava/lang/String;)I");
		printSimpleInstruction("invokestatic", classname + "/main("
			+ getTypeDescriptor(arg1->getType())
			+ getTypeDescriptor(arg2->getType()) + ")I");
	}
	else {
		llvm_unreachable("main function has invalid number of arguments");
	}

	printSimpleInstruction("invokestatic", "llxvm/lib/c/exit(I)V");
	printSimpleInstruction("return");
	
	if (opts->jvm_use_krakatau) {
		*outstream << ".end code\n";
	}
	
	*outstream << ".end method\n";
}
