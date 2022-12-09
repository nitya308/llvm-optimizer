//
// LLVM Function Pass Template
//
// Parts taken from skeleton Copyright (c) 2015 Adrian Sampson at
// https://github.com/sampsyo/llvm-pass-skeleton/blob/master/skeleton/Skeleton.cpp
// License file included in directory.
//
// 01 May 2022    jpb    Creation from foundational works shown.
//

#pragma once

#include <llvm/Pass.h>        // for llvm::FunctionPass
#include <llvm/IR/Function.h> // for llvm::Function

// Change the DEBUG_TYPE define to the friendly name of your pass...for some reason?
#define DEBUG_TYPE "skeletonpass"

struct SkeletonPass : public llvm::FunctionPass {
    static char ID;
    // This constructor just calls the parent class's constructor.
    SkeletonPass() : llvm::FunctionPass(ID) {}

    bool runOnFunction(llvm::Function &F);
};