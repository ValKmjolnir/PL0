#include "pl.h"
#include "pl_lex.h"
#include "pl_ast.h"
#include "pl_code.h"
#include "pl_parse.h"
#include "pl_vm.h"

void version()
{
	std::cout << "\n";
	std::cout << "Compiler Principles (Prof.Xie) | NUAA CCST 2020\n\n";
	std::cout << "PL/0 compiler ("<<__TIME__<<" "<<__DATE__<<")\n";
}

void help()
{
	std::cout << "usage:\n";
	std::cout << "  pl0 -ast [filename]   | show compiled ast.\n";
	std::cout << "  pl0 -code [filename]  | show compiled bytecode.\n";
	std::cout << "  pl0 -exec [filename]  | execute file directly.\n";
	std::cout << "  pl0 -debug [filename] | execute file in debug mode.\n";
}

int main(int argc, const char* argv[])
{
	if (argc == 1)
	{
		version();
		help();
		return 0;
	}

	if (argc == 2 && !strcmp(argv[1], "-h"))
	{
		version();
		help();
		return 0;
	}

	if (argc != 3)
	{
		std::cout << "fatal error: no input file.\n";
		help();
		exit(-1);
	}

	if (strcmp(argv[1], "-ast") &&
		strcmp(argv[1], "-code") &&
		strcmp(argv[1], "-exec") &&
		strcmp(argv[1], "-debug"))
	{
		std::cout << "error parameter: " << argv[1] << '\n';
		help();
		exit(-1);
	}

	init();
	if (!read_file(argv[2]))
		exit(-1);
	ast result = parse();
	close_file();

	if (!compile_error)
	{
		if (!strcmp(argv[1], "-ast"))
		{
			result.print();
			return 0;
		}
		if (!strcmp(argv[1], "-code"))
		{
			for (int i = 0; i < exec_code.size(); ++i)
				printf("0x%.8x %s 0x%.8x 0x%.8x\n", i, opcode_name[exec_code[i].opcode], exec_code[i].level, exec_code[i].opnum);
			return 0;
		}
	}
	else
		return 0;

	if (!strcmp(argv[1], "-exec"))
		vm_run();
	else
		vm_debug();
	return 0;
}
