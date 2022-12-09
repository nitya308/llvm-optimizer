clang-10 -O -S -Xclang -disable-llvm-passes -emit-llvm $1 -o /dev/stdout | opt-10 -S -load=./template.so --mem2reg --constpass
