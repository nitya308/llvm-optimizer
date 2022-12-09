// LLVM constant propogation and constant folding pass

#include "template.hpp"

#include <llvm/IR/BasicBlock.h>         // for llvm::BasicBlock
#include <llvm/IR/Constants.h>          // for llvm::ConstantInt
#include <llvm/IR/Function.h>           // for llvm::Function
#include <llvm/IR/Instruction.h>        // for llvm::Instruction
#include <llvm/IR/Instructions.h>       // for llvm::Instruction
#include <llvm/IR/LegacyPassManager.h>  // for llvm::legacy::PassManagerBase
#include <llvm/IR/Type.h>               // for llvm::Type
#include <llvm/IR/Use.h>                // for llvm::Use
#include <llvm/IR/User.h>
#include <llvm/IR/Value.h>                           // for llvm::Value
#include <llvm/Pass.h>                               // for llvm::RegisterPass<T>
#include <llvm/Support/raw_ostream.h>                // for llvm::errs
#include <llvm/Transforms/IPO/PassManagerBuilder.h>  // for llvm::RegisterStandardPasses and llvm::PassManagerBuilder

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

template <typename T>
void print_with_indentation(T const &thing, int indentation)
{
  for (int i = 0; i < indentation; i++) {
    llvm::errs() << "    ";
  }
  thing.print(llvm::errs());
  llvm::errs() << "\n";
}

// function to handle comparison instruction
llvm::Constant *handleComparsionOp(const llvm::Instruction &instruction, llvm::ConstantInt const &rhs, llvm::ConstantInt const &lhs)
{
  bool bResult = false;  // to store the comparison result
  // casting instruction to a CmpInst type
  const llvm::CmpInst &comp = llvm::cast<llvm::CmpInst>(instruction);
  switch (comp.getPredicate()) {  // comparing cases for the enum types of comparisons
    case llvm::CmpInst::ICMP_EQ:
      bResult = (lhs.getSExtValue() == rhs.getSExtValue());
      break;
    case llvm::CmpInst::ICMP_SGT:
      bResult = (lhs.getSExtValue() > rhs.getSExtValue());
      break;
    case llvm::CmpInst::ICMP_SGE:
      bResult = (lhs.getSExtValue() >= rhs.getSExtValue());
      break;
    case llvm::CmpInst::ICMP_SLT:
      bResult = (lhs.getSExtValue() < rhs.getSExtValue());
      break;
    case llvm::CmpInst::ICMP_SLE:
      bResult = (lhs.getSExtValue() <= rhs.getSExtValue());
      break;
    case llvm::CmpInst::ICMP_NE:
      bResult = (lhs.getSExtValue() != rhs.getSExtValue());
      break;
    default:
      // ignoring default case
      break;
  }
  // converting comparsion to a constant and returning it
  llvm::Constant *r = llvm::ConstantInt::get(comp.getType(), llvm::APInt(comp.getType()->getIntegerBitWidth(), bResult));
  return r;
}

// main pass function for constant propogation and folding
bool SkeletonPass::runOnFunction(llvm::Function &function)
{
  bool isChanged = false;  // Tracks whether an instruction has been changed in each iteration
  llvm::errs() << "==\n====\n======\nSTARTING MY PASS\n";
  // Looping over the block
  for (llvm::BasicBlock &block : function) {
    for (llvm::BasicBlock::iterator I = block.begin(), IE = block.end(); I != IE; ++I) {
      if (isChanged) {  // Decrementing iterator if an instruction has been changed
        I--;
        isChanged = false;
      }
      // Getting instruction from iterator
      llvm::Instruction const &instruction = *I;
      // Checking if 2 operands are constants
      if (instruction.getNumOperands() == 2 && llvm::isa<llvm::ConstantInt>(instruction.getOperand(0)) && llvm::isa<llvm::ConstantInt>(instruction.getOperand(1))) {
        llvm::errs() << "Both operands are constants\n";
        // Getting left and right operands
        llvm::ConstantInt const &lhs = llvm::cast<llvm::ConstantInt>(*instruction.getOperand(0));
        llvm::ConstantInt const &rhs = llvm::cast<llvm::ConstantInt>(*instruction.getOperand(1));
        llvm::IntegerType *const binopType = lhs.getType();
        llvm::ConstantInt *result = nullptr;
        // Looking for the known operations by getting Opcode
        switch (instruction.getOpcode()) {
          case llvm::Instruction::Add:  // performs the add operation
            result = llvm::ConstantInt::getSigned(binopType, lhs.getSExtValue() + rhs.getSExtValue());
            llvm::errs() << "Addition result is " << *result << "\n";
            llvm::ReplaceInstWithValue(block.getInstList(), I, result);
            isChanged = true;
            break;
          case llvm::Instruction::Sub:  // performs the subtraction operation
            result = llvm::ConstantInt::getSigned(binopType, lhs.getSExtValue() - rhs.getSExtValue());
            llvm::errs() << "Subtraction result is " << *result << "\n";
            llvm::ReplaceInstWithValue(block.getInstList(), I, result);
            isChanged = true;
            break;
          case llvm::Instruction::Mul:  // performs the mult operation
            result = llvm::ConstantInt::getSigned(binopType, lhs.getSExtValue() * rhs.getSExtValue());
            llvm::errs() << "Multiplication result is " << *result << "\n";
            llvm::ReplaceInstWithValue(block.getInstList(), I, result);
            isChanged = true;
            break;
          case llvm::Instruction::SDiv:  // performs the div operation
            result = llvm::ConstantInt::getSigned(binopType, lhs.getSExtValue() / rhs.getSExtValue());
            llvm::errs() << "Division result is " << *result << "\n";
            llvm::ReplaceInstWithValue(block.getInstList(), I, result);
            isChanged = true;
            break;
          case llvm::Instruction::ICmp:  // performs the comparison operation by calling function
            llvm::errs() << "Handling the comparison \n";
            llvm::Constant *r = handleComparsionOp(instruction, lhs, rhs);
            llvm::ReplaceInstWithValue(block.getInstList(), I, r);
            isChanged = true;
            break;
        }
      }
    }
  }

  llvm::errs() << "ENDING MY PASS\n======\n====\n==\n";

  return false;
}

char SkeletonPass::ID = 0;
static llvm::RegisterPass<SkeletonPass> skelly("constpass", "Const prop and folding pass", false, false);

// This is a C++ lambda.
void add_skeleton_to_the_pm(llvm::PassManagerBuilder const &, llvm::legacy::PassManagerBase &PM) { PM.add(new SkeletonPass()); };

static llvm::RegisterStandardPasses RegisterMyPass(llvm::PassManagerBuilder::EP_FullLinkTimeOptimizationLast, add_skeleton_to_the_pm);