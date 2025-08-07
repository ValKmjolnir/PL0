#pragma once

#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>

enum ast_type
{
	ast_root,
	ast_prog,
	ast_block,
	ast_condecl,
	ast_vardecl,
	ast_proc,
	ast_id,
	ast_number,
	ast_parameter,
	ast_body,
	ast_assign,
	ast_condition,
	ast_if,
	ast_else,
	ast_while,
	ast_call,
	ast_argument,
	ast_read,
	ast_write,
	ast_odd,
	ast_add,
	ast_sub,
	ast_mul,
	ast_div,
	ast_eq,
	ast_neq,
	ast_less,
	ast_leq,
	ast_great,
	ast_geq
};

struct
{
	const char* ast_content;
	int type;
}ast_table[]=
{
	{"root",         ast_root},
	{"program",      ast_prog},
	{"block",        ast_block},
	{"const_declare",ast_condecl},
	{"var_declare",  ast_vardecl},
	{"procedure",    ast_proc},
	{"identifier",   ast_id},
	{"number",       ast_number},
	{"parameter",    ast_parameter},
	{"body",         ast_body},
	{"assignment",   ast_assign},
	{"conditional",  ast_condition},
	{"if",           ast_if},
	{"else",         ast_else},
	{"while",        ast_while},
	{"call",         ast_call},
	{"argument",     ast_argument},
	{"read",         ast_read},
	{"write",        ast_write},
	{"odd",          ast_odd},
	{"+",            ast_add},
	{"-",            ast_sub},
	{"*",            ast_mul},
	{"/",            ast_div},
	{"=",            ast_eq},
	{"<>",           ast_neq},
	{"<",            ast_less},
	{"<=",           ast_leq},
	{">",            ast_great},
	{">=",           ast_geq},
	{NULL,-1}
};

class ast
{
private:
	int line;
	int type;
	std::string content;
	std::vector<ast> child;
public:
	ast(int=0,int=ast_root,std::string="");
	ast(const ast&);
	~ast();
	void addchild(ast);
	void setline(int);
	void settype(int);
	void setstr(std::string&);
	std::vector<ast>& getchild();
	int getline();
	int gettype();
	std::string getstr();
	ast& operator=(const ast&);
	void print(int);
};

ast::ast(int default_line,int default_type,std::string default_content)
{
	line=default_line;
	type=default_type;
	content=default_content;
	return;
}

ast::ast(const ast& tmp)
{
	line=tmp.line;
	type=tmp.type;
	content=tmp.content;
	child=tmp.child;
	return;
}

ast::~ast()
{
	content="";
	child.clear();
	return;
}

void ast::addchild(ast tmp)
{
	child.push_back(tmp);
	return;
}
void ast::setline(int l)
{
	line=l;
	return;
}
void ast::settype(int t)
{
	type=t;
	return;
}
void ast::setstr(std::string& s)
{
	content=s;
	return;
}
std::vector<ast>& ast::getchild()
{
	return child;
}
int ast::getline()
{
	return line;
}
int ast::gettype()
{
	return type;
}
std::string ast::getstr()
{
	return content;
}

ast& ast::operator=(const ast& tmp)
{
	line=tmp.line;
	type=tmp.type;
	content=tmp.content;
	child=tmp.child;
	return *this;
}

void ast::print(int depth=0)
{
	std::string s="";
	for(int i=0;i<depth;++i)
		s+="| ";
	std::cout<<s<<ast_table[type].ast_content;
	if(type==ast_id || type==ast_number || type==ast_prog || type==ast_proc || type==ast_call || type==ast_assign)
		std::cout<<": "<<content;
	std::cout<<"\n";
	for(std::vector<ast>::iterator i=child.begin();i!=child.end();++i)
		i->print(depth+1);
	return;
}
