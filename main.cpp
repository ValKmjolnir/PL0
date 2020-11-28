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
	if(!compile_error)
		result.print();
	close_file();
	return 0;
}
