# LLVM Optimzer

llvm IR function pass in C++ that does constant propagation and constant folding for a set of instructions.
* Constant Folding: Finds and evaluates constant expressions at compile time rather than computing them at runtime, saving space and processing time.
* Constant Propogation: Substitutes the values of known constants in expressions at compile time.

## Function Pass Design:
The function pass has two for loops.
The outer for loop iterates over all the BasicBlocks in the function.
The inner loop iterates over all instructions in each basic block using an iterator of type `llvm::BasicBlock::iterator`

If an instruction has two constant operands, we extract the left and right hand side constants.

Then the type of mathematical operation in the instruction is checked using `getOpcode` and comparing it to the known binary operations. For each mathematical operation case, we perform the addition/subtraction/multiplication/division and store it in a result variable. Then the llvm function `ReplaceInstWithValue` is called to replace all uses in instructions in that block after the current one with the new result value.

For comparison operations, a separate function called `handleComparsionOp` is called. It casts the instruction to a `llvm::CmpInst type` and then runs a switch-case on the predicate. Each type of comparison operator (equal to, not equal to, less than, less than or equal to, greater than & greater than or equal to) is handled and the comparison is performed. All uses are replaced with the true/false result of the comparison.

These two for loops run over all the instructions in each basic block until they have all been optimized.


## Issues with building the pass:
1. Decrementing the iterator:
If an instruction is optimized, it gets deleted. This means that the iterator for the instructions would move to the next instruction in the block. When the iterator was incremented again at the end of the for loop, this meant that the iterator had now moved two instructions forward. This was a problem with the design that had to be fixed.
I fixed this by keeping a boolean variable that tracked whether an instruction had been changed in that iteration. If it had, I would decrement the iterator after that to make sure it moved only one instruction forward and not two.

2. Converting a comparison to a constant int.  
LLVM does booleans by using i1 - 1 bit binary values. In order to convert it to a const, getType was used along with APInt which makes sure it is 1 bit long.

## How to run the pass:
Running make creates optimize.sh which takes an input ll file as its argument. Running optimize.sh on an ll file will output the optimized code.