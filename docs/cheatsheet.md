$LLVM_DIR/bin/clang -S -emit-llvm -Xclang -disable-O0-optnone tests/prog.c -o tests/prog.ll
$LLVM_DIR/bin/opt -load build/mem-usage/libMemUsage.so -mem2reg -mem-usage tests/prog.ll -o tests/prog.ll