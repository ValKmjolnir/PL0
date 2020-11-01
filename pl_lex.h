#ifndef __PL_LEX_H__
#define __PL_LEX_H__

#define IS_ID(c) (('a'<=(c) && (c)<='z') || ('A'<=(c) && (c)<='Z'))
#define IS_DIGIT(c) ('0'<=(c) && (c)<='9')
#define IS_OPR(c) ((c)=='+' || (c)=='-' || (c)=='*' || (c)=='/' || (c)=='=' || (c)==',' || (c)==';' || (c)=='(' || (c)==')')
#define IS_MOPR(c) ((c)==':' || (c)=='<' || (c)=='>')

enum token_type
{
	tok_begin,tok_end,
	tok_odd,
	tok_program,tok_const,tok_var,
	tok_procedure,
	tok_if,tok_then,tok_else,
	tok_while,tok_do,
	tok_call,tok_read,tok_write,
	tok_identifier,
	tok_number,
	tok_assign,
	tok_add,tok_sub,tok_mul,tok_div,
	tok_equal,tok_neq,
	tok_less,tok_leq,
	tok_great,tok_geq,
	tok_lcurve,tok_rcurve,
	tok_semi,tok_comma,
	tok_eof
};

struct
{
	const char* content;
	int type;
}token_table[]={
	{"begin",tok_begin},
	{"end",tok_end},
	{"odd",tok_odd},
	{"program",tok_program},
	{"const",tok_const},
	{"var",tok_var},
	{"procedure",tok_procedure},
	{"if",tok_if},
	{"then",tok_then},
	{"else",tok_else},
	{"while",tok_while},
	{"do",tok_do},
	{"call",tok_call},
	{"read",tok_read},
	{"write",tok_write},
	{"program",tok_program},
	{",",tok_comma},
	{";",tok_semi},
	{"(",tok_lcurve},
	{")",tok_rcurve},
	{":=",tok_assign},
	{"+",tok_add},
	{"-",tok_sub},
	{"*",tok_mul},
	{"/",tok_div},
	{"=",tok_equal},
	{"<>",tok_neq},
	{"<",tok_less},
	{"<=",tok_leq},
	{">",tok_great},
	{">=",tok_geq},
	{NULL,-1},
};

std::ifstream file_handle;
std::string line_code;
int line;
int col;
char c;

struct
{
	std::string content;
	int tok_type;
}token;

void init()
{
	line_code="";
	line=1;
	col=0;
	return;
}

bool read_file(std::string filename)
{
	file_handle.open(filename,std::ios::binary);
	if(file_handle.fail())
	{
		file_handle.close();
		die("cannot open file \""+filename+"\".");
		return false;
	}
	c=file_handle.get();
	return true;
}

void close_file()
{
	file_handle.close();
	return;
}

void next()
{
	if(file_handle.eof())
	{
		token.content="";
		token.tok_type=tok_eof;
		return;
	}
	if(c) line_code+=c;
	bool unknown_char=false;
	if(c!='\n' && c!='\r' && c!='\t' && c!=' ' && !IS_DIGIT(c) && !IS_ID(c) && !IS_OPR(c) && !IS_MOPR(c))
	{
		if(!col) col=line_code.length();
		unknown_char=true;
	}
	while(!file_handle.eof() && !IS_DIGIT(c) && !IS_ID(c) && !IS_OPR(c) && !IS_MOPR(c))
	{
		c=file_handle.get();
		if(file_handle.eof())
		{
			token.content="";
			token.tok_type=tok_eof;
			return;
		}
		line_code+=c;
		if(IS_DIGIT(c) || IS_ID(c) || IS_OPR(c) || IS_MOPR(c))
			break;
		else if(c=='\n')
		{
			if(unknown_char)
			{
				die("["+line_code+"] unknown character.",line,col);
				unknown_char=false;
				col=0;
			}
			++line;
			line_code="";
		}
		else if(c!='\r' && c!='\t' && c!=' ')
		{
			if(!col) col=line_code.length();
			unknown_char=true;
		}
	}
	if(unknown_char)
	{
		die("["+line_code+"] unknown character.",line,col);
		col=0;
	}
	if(IS_DIGIT(c))
	{
		token.content=c;
		bool error_number=false;
		while(!file_handle.eof())
		{
			c=file_handle.get();
			if(file_handle.eof())
				break;
			if(IS_DIGIT(c))
			{
				line_code+=c;
				token.content+=c;
			}
			else if(IS_ID(c))
			{
				line_code+=c;
				error_number=true;
				if(!col)
					col=line_code.length();
			}
			else
				break;
		}
		token.tok_type=tok_number;
		if(error_number)
			die("["+line_code+"] error number.",line,col);
		col=0;
	}
	else if(IS_ID(c))
	{
		token.content=c;
		while(!file_handle.eof())
		{
			c=file_handle.get();
			if(file_handle.eof())
				break;
			if(IS_ID(c) || (IS_DIGIT(c)))
			{
				line_code+=c;
				token.content+=c;
			}
			else
				break;
		}
		token.tok_type=-1;
		for(int i=0;token_table[i].content;++i)
			if(token.content==token_table[i].content)
			{
				token.tok_type=token_table[i].type;
				break;
			}
		if(token.tok_type<0)
			token.tok_type=tok_identifier;
	}
	else if(IS_OPR(c))
	{
		token.content=c;
		c=file_handle.get();
		for(int i=0;token_table[i].content;++i)
			if(token.content==token_table[i].content)
			{
				token.tok_type=token_table[i].type;
				break;
			}
	}
	else if(IS_MOPR(c))
	{
		token.content=c;
		c=file_handle.get();
		if(!file_handle.eof())
		{
			line_code+=c;
			if(c=='=')
			{
				token.content+=c;
				c=file_handle.get();
			}
			else if(c=='>' && token.content=="<")
			{
				token.content=="<>";
				c=file_handle.get();
			}
			else
				line_code.pop_back();
		}
		token.tok_type=-1;
		for(int i=0;token_table[i].content;++i)
			if(token.content==token_table[i].content)
			{
				token.tok_type=token_table[i].type;
				break;
			}
		if(token.tok_type<0)
			die("["+line_code+"] incorrect operator.",line,line_code.length());
	}
	return;
}



#endif
