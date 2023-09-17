>> Compile
clang++ -g lang.cpp `llvm-config -cxxflags --ldflags --system-libs --libs core orcjit native` -O3 -o lang
./lang