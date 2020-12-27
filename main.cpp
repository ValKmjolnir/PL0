#include "pl.h"

int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		std::cout<<"fatal error: no input file.\n";
		exit(0);
	}
	if(strcmp(argv[1],"-exec") && strcmp(argv[1],"-debug"))
	{
		std::cout<<"error parameter: "<<argv[1]<<'\n';
		exit(0);
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
