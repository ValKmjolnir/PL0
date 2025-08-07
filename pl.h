#pragma once

#include <iostream>
#include <fstream>
#include <cstring>
#include <list>
#include <vector>
#include <map>

std::ifstream file_handle;
std::string line_code;
int line;
char c;
bool compile_error;

void die(const std::string& error_info, int l = -1)
{
	compile_error = true;
	if (l < 0) {
		std::cout << "line " << line << ", column " << line_code.length() << ":" << error_info << '\n';
	} else {
		std::cout <<"line " << l << ":" << error_info << '\n';
	}
	return;
}

