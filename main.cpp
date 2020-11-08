#include "pl.h"

int main()
{
	init();
	bool read_success=read_file("example.txt");
	std::ofstream fout("lex_info.txt");
	if(!read_success)
		exit(-1);
	parse().print();
	while(token.tok_type!=tok_eof)
	{
		next();
		if(token.tok_type!=tok_eof)
			fout<<"("<<token.content<<" | "<<token.tok_type<<")\n";
	}
	close_file();
	fout.close();
	return 0;
}
