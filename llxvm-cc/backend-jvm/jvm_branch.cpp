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
#include "libcpputils.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/SymbolTableListTraits.h"
/**
 * Return a unique ID.
 *
 * @return  a unique ID
 */
static uint64_t getUID()
{
	static uint64_t x = 0;
	return ++x;
}

/**
 * Replace PHI instructions with copy instructions (load-store pairs).
 *
 * @param src   the predecessor block
 * @param dest  the destination block
 */
void JVMWriter::printPHICopy(const BasicBlock *src, const BasicBlock *dest)
{
	for (BasicBlock::const_iterator i = dest->begin(); isa<PHINode>(i); i++) {
		const PHINode *phi = cast<PHINode>(i);
		const Value *val = phi->getIncomingValueForBlock(src);
		if (isa<UndefValue>(val))
			continue;
		printValueLoad(val);
		printValueStore(phi);
	}
}

/**
 * Print an unconditional branch instruction.
 *
 * @param curBlock   the current block
 * @param destBlock  the destination block
 */
void JVMWriter::printBranchInstruction(const BasicBlock *curBlock,
	const BasicBlock *destBlock)
{
	printPHICopy(curBlock, destBlock);
	printSimpleInstruction(GOTO_TOKEN, getLabelName(destBlock));
}

/**
 * Print a conditional branch instruction.
 *
 * @param curBlock    the current block
 * @param trueBlock   the destination block if the value on top of the stack is
 *                    non-zero
 * @param falseBlock  the destination block if the value on top of the stack is
 *                    zero
 */
void JVMWriter::printBranchInstruction(const BasicBlock *curBlock,
	const BasicBlock *trueBlock,
	const BasicBlock *falseBlock)
{
	if (trueBlock == falseBlock) {
		printSimpleInstruction("pop");
		printBranchInstruction(curBlock, trueBlock);
	}
	else if (!falseBlock) {
		printPHICopy(curBlock, trueBlock);

		if (!opts->jvm_rewrite_branches)
			printSimpleInstruction("ifne", getLabelName(trueBlock));
		else
			rewriteBranch("ifne", getLabelName(trueBlock));
	}
	else {
		std::string labelname = getLabelName(trueBlock);
		if (isa<PHINode>(trueBlock->begin()))
			labelname += "_phi" + utostr(getUID());

		if (!opts->jvm_rewrite_branches)
			printSimpleInstruction("ifne", labelname);
		else
			rewriteBranch("ifne", labelname);

		if (isa<PHINode>(falseBlock->begin()))
			printPHICopy(curBlock, falseBlock);

		printSimpleInstruction(GOTO_TOKEN, getLabelName(falseBlock));

		if (isa<PHINode>(trueBlock->begin())) {
			printLabel(labelname);
			printPHICopy(curBlock, trueBlock);
			printSimpleInstruction(GOTO_TOKEN, getLabelName(trueBlock));
		}
	}
}

/**
 * Print a branch instruction.
 *
 * @param inst  the branch instrtuction
 */
void JVMWriter::printBranchInstruction(const BranchInst *inst)
{
	if (inst->isUnconditional()) {
		printBranchInstruction(inst->getParent(), inst->getSuccessor(0));
	}
	else {
		printValueLoad(inst->getCondition());
		printBranchInstruction(
			inst->getParent(), inst->getSuccessor(0), inst->getSuccessor(1));
	}
}

/**
 * Print a select instruction.
 *
 * @param cond      the condition
 * @param trueVal   the return value of the instruction if the condition is
 *                  non-zero
 * @param falseVal  the return value of the instruction if the condition is
 *                  zero
 */
void JVMWriter::printSelectInstruction(const Value *cond,
	const Value *trueVal,
	const Value *falseVal)
{
	std::string labelname;
	if (opts->jvm_use_krakatau) {
		labelname = "L_select" + utostr(getUID());
	}
	else {
		labelname = "select" + utostr(getUID());
	}
	printValueLoad(cond);
	printSimpleInstruction("ifeq", labelname + "a");
	printValueLoad(trueVal);
	printSimpleInstruction(GOTO_TOKEN, labelname + "b");
	printLabel(labelname + "a");
	printValueLoad(falseVal);
	printLabel(labelname + "b");
}

/**
 * Print a switch instruction.
 *
 * @param inst  the switch instruction
 */
void JVMWriter::printSwitchInstruction(const SwitchInst *inst)
{
	// TODO: This method does not handle switch statements when the
	// successor contains phi instructions (the value of the phi instruction
	// should be set before branching to the successor). Therefore, it has
	// been replaced by the switch lowering pass. Once this method is
	// fixed the switch lowering pass should be removed.

	std::map<int, unsigned int> cases;
	SwitchInst::ConstCaseIt it = inst->case_begin();
	for (unsigned int i = 0, e = inst->getNumCases(); i < e; i++, it++) {
		cases[(int)it->getCaseValue()->getValue().getSExtValue()] = i;
	}

	//// TODO: tableswitch in cases where it won't increase the size of the
	////       class file
	//printValueLoad(inst->getCondition());
	//int l, h;
	//if (canUseTableLookup(inst, &l, &h)) {

	//}
	//else {
		*outstream << "\tlookupswitch\n";
		for (std::map<int, unsigned int>::const_iterator i = cases.begin(), e = cases.end(); i != e; i++)
			*outstream << "\t\t" << i->first << " : " << getLabelName(inst->getSuccessor(i->second)) << '\n';

		*outstream << "\t\tdefault : " << getLabelName(inst->getDefaultDest()) << '\n';
	//}

}
//
//bool JVMWriter::canUseTableLookup(const SwitchInst *inst, int *low, int *high)
//{
//	std::set<int64_t> values;
//	SwitchInst::ConstCaseIt it = inst->case_begin();
//	for (unsigned int i = 0, e = inst->getNumCases(); i < e; i++, it++) {
//		int64_t v = it->getCaseValue()->getValue().getSExtValue();
//		values.insert(v);
//	}
//
//	return false;
//}

/**
 * Print a loop.
 *
 * @param l  the loop
 */
void JVMWriter::printLoop(const Loop *l)
{
	printLabel(getLabelName(l->getHeader()));
	for (Loop::block_iterator i = l->block_begin(),
		e = l->block_end(); i != e; i++) {
		const BasicBlock *block = *i;

		LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

		//Loop *blockLoop = getAnalysis<LoopInfo>().getLoopFor(block);
		Loop *blockLoop = LI.getLoopFor(dyn_cast<BasicBlock>(block));

		if (l == blockLoop)
			// the loop is the innermost parent of this block
			printBasicBlock(block);
		else if (block == blockLoop->getHeader()
			&& l == blockLoop->getParentLoop())
			// this block is the header of its innermost parent loop,
			// and the loop is the parent of that loop
			printLoop(blockLoop);
	}
	printSimpleInstruction(GOTO_TOKEN, getLabelName(l->getHeader()));
}

/*
	Rewrites a branch condition to use goto_w instructions, in order
	to ovecome the 16 bit offset limit imposed by the if** opcodes.
	If enabled, the "wide gotos" option is also enabled.

	Example:

	iload 2365 ; _cmp4305
	; *********************
	ifne orig_label
	; *********************
	goto_w label967
label967:

	becomes:

	iload 2365 ; _cmp4305
	; *********************
	ifeq lbl_if_000
	goto_w orig_label
lbl_if_000:
	; *********************
	goto_w label967
label967:
*/
void JVMWriter::rewriteBranch(const std::string &orig_op, const std::string &orig_label)
{
	std::string new_op;
	if (orig_op == "ifne")
		new_op = "ifeq";
	else
		if (orig_op == "ifeq")
			new_op = "ifne";
		else
			if (orig_op == "iflt")
				new_op = "ifge";
			else
				if (orig_op == "ifle")
					new_op = "ifgt";
				else
					if (orig_op == "ifgt")
						new_op = "ifle";
					else
						if (orig_op == "ifge")
							new_op = "iflt";

	if (new_op.empty()) {
		errs() << "Unsupported/unknow branch instruction (" << orig_op << "), cannot be rewritten\n";
		llvm_unreachable(("Unsupported/unknow branch instruction (" + orig_op + "), cannot be rewritten\n").c_str());
	}

	std::string new_lbl;
	if (opts->jvm_use_krakatau)
		new_lbl = "L_lbl_br_" + std::to_string(cur_rewrite_br_lbl_id++);
	else
		new_lbl = "lbl_br_" + std::to_string(cur_rewrite_br_lbl_id++);

	printSimpleInstruction(new_op, new_lbl);
	printSimpleInstruction("goto_w", orig_label);
	printSimpleInstruction(new_lbl + ":");
}


void JVMWriter::printIndirectBr(const IndirectBrInst *inst)
{
	const Function *f = inst->getFunction();
#if LLVM_VERSION_MAJOR > 10
	std::string func_name = f->getName().str();
#else
	std::string func_name = f->getName();
#endif

	auto var_name = inst->getOperand(0)->getName().str();
	unsigned int local_var_num = getLocalVarNumber(inst->getOperand(0));
	printSimpleInstruction("; indirectbr start (" + var_name + ")");
	if (local_var_num > 255 && opts->jvm_use_krakatau)
		printSimpleInstruction("wide iload", std::to_string(local_var_num));
	else
		printSimpleInstruction("iload", std::to_string(local_var_num));


	// Extract the labels we need to handle
	std::vector<std::string> indbr_labels;
	for (int i = 1; i < inst->getNumOperands(); i++) {
		auto op = inst->getOperand(i)->getName();
		indbr_labels.push_back(op.str());
	}

	// We extract the relevant labels (only those handled by this instance of the indirectbr statement
	// and put them in another map to sort them by out assigned ID. This is needed because the
	// "lookupswitch" instruction wants its IDs in ascending order
	std::map<unsigned int, std::string> lookup_data;
	for (std::string indbr_label : indbr_labels) {
		if (basicblock_map.find(indbr_label) == basicblock_map.end()) {
			errs() << "blockstore ID  = " << indbr_label << '\n';
			llvm_unreachable(("Invalid BasicBlock ID: " + indbr_label).c_str());
		}

		unsigned int bsid = basicblock_map[indbr_label].first;
		lookup_data[bsid] = indbr_label;
	}

#if 1
	// We loop through our lookup data and build the lookup switch statement
	*outstream << "\tlookupswitch\n";
	for (std::map<unsigned int, std::string >::const_iterator i = lookup_data.begin(), e = lookup_data.end(); i != e; i++) {
		std::string l_name = i->second;
		if (basicblock_map.find(l_name) == basicblock_map.end()) {
			errs() << "Invalid label ID: " + l_name << '\n';
			llvm_unreachable(("Invalid label ID: " + l_name).c_str());
		}

		const BasicBlock *bb = basicblock_map[l_name].second;
		*outstream << "\t\t" << i->first << " : " << getLabelName(bb);
		if (opts->debug_level >= 2)
			*outstream << " ; " << l_name;

		*outstream << '\n';
	}
	*outstream << "\t\tdefault : " << getLabelName(basicblock_map.begin()->second.second) << '\n';
#else
	// We loop through our lookup data and build the lookup switch statement
	std::string cur_l_name;
	*outstream << "\ttableswitch " << lookup_data.begin()->first << "\n";

	for (unsigned int bid = lookup_data.begin()->first; bid <= std::prev(lookup_data.end())->first; bid++) {

		if (map_contains<unsigned int, std::string>(lookup_data, bid)) {
			cur_l_name = lookup_data[bid];
		}

		if (basicblock_map.find(cur_l_name) == basicblock_map.end()) {
			errs() << "Invalid label ID: " + cur_l_name << '\n';
			llvm_unreachable(("Invalid label ID: " + cur_l_name).c_str());
		}

		const BasicBlock *bb = basicblock_map[cur_l_name].second;
		*outstream << "\t\t" << getLabelName(bb);
		if (opts->debug_level >= 2)
			*outstream << " ; " << cur_l_name;

		*outstream << '\n';
	}
	*outstream << "\t\tdefault : " << getLabelName(basicblock_map.begin()->second.second) << '\n';
#endif
	printSimpleInstruction("; indirectbr end");
}
