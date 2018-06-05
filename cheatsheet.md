$LLVM_DIR/bin/clang -S -emit-llvm -Xclang -disable-O0-optnone prog.c
$LLVM_DIR/bin/opt -load build/skeleton/libSkeletonPass.so -mem2reg -mem-usage prog.ll