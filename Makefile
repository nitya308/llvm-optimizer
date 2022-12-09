#
# Sample Makefile to generate LLVM passes
#
# make all builds the sample template as a shared library
# make test runs the library against an example IR
# make clean deletes the built files
#
# 05 May 2022  jpb  Creation.
#

LLVM-VERSION:=-10
CXX := clang++$(LLVM-VERSION)
CC := clang$(LLVM-VERSION)
OPT = opt$(LLVM-VERSION)
LLVM-CONFIG := llvm-config$(LLVM-VERSION)
CFLAGS := `$(LLVM-CONFIG) --cflags` -fPIC -I/usr/include/llvm-10/
CXXFLAGS := `$(LLVM-CONFIG) --cxxflags` -fPIC -I/usr/include/llvm-10/
STYLE := "{BasedOnStyle: llvm, IndentWidth: 4, ColumnLimit: 200}"

# In order to echo the literal text '$1' into a file, I have to double-escape it as '\$$1'.
# That took too long to figure out!
optimize.sh: template.so
	echo "$(CC) -O -S -Xclang -disable-llvm-passes -emit-llvm \$$1 -o /dev/stdout | $(OPT) -S -load=./template.so --mem2reg --constpass" > optimize.sh
		chmod +x optimize.sh

template.so: template.o
	$(CC) $(CFLAGS) -shared template.o -o template.so

template.o: template.cpp
	$(CXX) $(CXXFLAGS) -c template.cpp -o $@

clean:
	rm -f template.so template.o optimize.sh