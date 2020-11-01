#ifndef __PL_H__
#define __PL_H__

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

void die(std::string error_info,int line=-1,int column=-1)
{
	if(line>=0 && column>=0)
		std::cout<<"line "<<line<<",column "<<column<<":"<<error_info<<'\n';
	else
		std::cout<<error_info<<'\n';
	return;
}
#include "pl_lex.h"
#include "pl_parse.h"

#endif
