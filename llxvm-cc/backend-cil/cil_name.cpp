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

#include "CILWriter.h"

#include <llvm/MC/MCContext.h>
#include <llvm/MC/MCAsmInfo.h>
#include <llvm/IR/Mangler.h>
#include <llvm/ADT/SmallString.h>

/**
 * Replace any non-alphanumeric characters with underscores.
 * 
 * @param name  the name to sanitize
 * @return      the sanitized name
 */
std::string CILWriter::sanitizeName(std::string name) {
    for(std::string::iterator i = name.begin(), e = name.end(); i != e; i++)
        if(!isalnum(*i))
            *i = '_';
    return name;
}

/**
 * Return the name of the given value.
 * 
 * @param v  the value
 * @return   the name of the value
 */
std::string CILWriter::getValueName(const Value *v) {
    if(const GlobalValue *gv = dyn_cast<GlobalValue>(v)) {
		SmallString<10> name;
        mangler->getNameWithPrefix(name, gv, false);
        return sanitizeName(name.str().str());
	}
    if(v->hasName())
#if LLVM_VERSION_MAJOR > 10		
        return '_' + sanitizeName(v->getName().str());
#else
        return '_' + sanitizeName(v->getName());
#endif
	
    if(localVars.count(v))
        return '_' + utostr(getLocalVarNumber(v));
    return "_";
}
/**
 * Return the label name of the given block.
 * 
 * @param block  the block
 * @return       the label
 */
std::string CILWriter::getLabelName(const BasicBlock *block) {
    if(!blockIDs.count(block))
        blockIDs[block] = blockIDs.size() + 1;

	if (!opts->jvm_use_krakatau)
    	return sanitizeName("label" + utostr(blockIDs[block]));
	else
		return sanitizeName("L_label" + utostr(blockIDs[block]));
}
