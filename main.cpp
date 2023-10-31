#include "pl.h"

void version()
{
	std::cout<<"PL/0 compiler/interpreter ("<<__TIME__<<" "<<__DATE__<<")\n";
}

void help()
{
	std::cout<<"usage:\n";
	std::cout<<"  pl0 -exec [filename]  | execute file directly.\n";
	std::cout<<"  pl0 -debug [filename] | execute file in debug mode.\n";
}

int main(int argc,char* argv[])
{
	if (argc==1) {
		version();
		help();
		return 0;
	}

	if(argc!=3)
	{
		std::cout<<"fatal error: no input file.\n";
		help();
		exit(-1);
	}

	if(strcmp(argv[1],"-exec") && strcmp(argv[1],"-debug"))
	{
		std::cout<<"error parameter: "<<argv[1]<<'\n';
		help();
		exit(-1);
	}

	init();
	if(!read_file(argv[2]))
		exit(-1);
	ast result=parse();
	close_file();

	if(!compile_error)
	{
		result.print();
		for(int i=0;i<exec_code.size();++i)
        	printf("0x%.8x %s 0x%.8x 0x%.8x\n",i,opcode_name[exec_code[i].opcode],exec_code[i].level,exec_code[i].opnum);
	}
	else
		exit(0);

	if(!strcmp(argv[1],"-exec"))
		vm_run();
	else
		vm_debug();
	return 0;
}
