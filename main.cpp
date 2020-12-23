#include "pl.h"

int main(int argc,char* argv[])
{
	if(argc!=2)
	{
		std::cout<<"fatal error: no input file.\n";
		exit(0);
	}
	init();
	if(!read_file(argv[1]))
		exit(-1);
	ast result=parse();
	close_file();
	if(!compile_error)
	{
		result.print();
		for(int i=0;i<exec_code.size();++i)
        	printf("0x%.8x %s 0x%.8x 0x%.8x\n",i,opcode_name[exec_code[i].opcode],exec_code[i].level,exec_code[i].opnum);
		vm_run();
	}
	return 0;
}
