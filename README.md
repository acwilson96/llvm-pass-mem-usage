# llvm-pass-mem-usage
LLVM Pass for estimating memory usage.

# Usage
1. Build LLVM to some directory, $LLVM_DIR
2. `git clone https://github.com/acwilson96/llvm-pass-mem-usage.git`
3. `cd llvm-pass-mem-usage`
4. Compile source file using: `$LLVM_DIR/bin/clang -S -emit-llvm -Xclang -disable-O0-optnone prog.c`
5. Run pass using: `$LLVM_DIR/bin/opt -load build/mem-usage/libMemUsage.so -mem2reg -mem-usage prog.ll -o prog.ll`
