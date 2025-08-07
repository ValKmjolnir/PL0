
pl0: main.cpp pl_ast.h pl_code.h pl_lex.h pl_parse.h pl_vm.h pl.h
	c++ -std=c++11 -O3 main.cpp -o pl0

.PHONY=clean
clean:
	rm pl0