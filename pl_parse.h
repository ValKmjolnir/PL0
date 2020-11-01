#ifndef __PL_PARSE_H__
#define __PL_PARSE_H__

void match(int);
void parse();
void prog();
void block();
void condecl();
void _const();
void vardecl();
void proc();
void body();
void statement();
void lexp();
void exp();
void term();
void factor();
void lop();

void match(int tok_type)
{
	if(token.tok_type!=tok_type)
	{
		std::string expect;
		if(tok_type==tok_identifier)
			expect="identifier";
		else if(tok_type==tok_number)
			expect="number";
		else
			for(int i=0;token_table[i].content;++i)
				if(tok_type==token_table[i].type)
				{
					expect=token_table[i].content;
					break;
				}
		die("["+line_code+"] expect token \""+expect+"\".",line,line_code.length());
	}
	next();
	return;
}

void parse()
{
	next();
	prog();
	return;
}

void prog()
{
	match(tok_program);
	match(tok_identifier);
	match(tok_semi);
	block();
	return;
}

void block()
{
	if(token.tok_type==tok_const)
		condecl();
	if(token.tok_type==tok_var)
		vardecl();
	if(token.tok_type==tok_procedure)
		proc();
	body();
	return;
}

void condecl()
{
	match(tok_const);
	_const();
	while(token.tok_type==tok_comma)
	{
		match(tok_comma);
		_const();
	}
	match(tok_semi);
	return;
}

void _const()
{
	match(tok_identifier);
	match(tok_assign);
	match(tok_number);
	return;
}

void vardecl()
{
	match(tok_var);
	match(tok_identifier);
	while(token.tok_type==tok_comma)
	{
		match(tok_comma);
		match(tok_identifier);
	}
	match(tok_semi);
	return;
}

void proc()
{
	match(tok_procedure);
	match(tok_identifier);
	match(tok_lcurve);
	if(token.tok_type==tok_identifier)
	{
		match(tok_identifier);
		while(token.tok_type==tok_comma)
		{
			match(tok_comma);
			match(tok_identifier);
		}
	}
	match(tok_rcurve);
	match(tok_semi);
	block();
	while(token.tok_type==tok_semi)
	{
		match(tok_semi);
		proc();
	}
	return;
}

void body()
{
	match(tok_begin);
	statement();
	while(token.tok_type==tok_semi)
	{
		match(tok_semi);
		statement();
	}
	match(tok_end);
	return;
}

void statement()
{
	if(token.tok_type==tok_identifier)
	{
		match(tok_identifier);
		match(tok_assign);
		exp();
	}
	else if(token.tok_type==tok_if)
	{
		match(tok_if);
		lexp();
		match(tok_then);
		statement();
		if(token.tok_type==tok_else)
		{
			match(tok_else);
			statement();
		}
	}
	else if(token.tok_type==tok_while)
	{
		match(tok_while);
		lexp();
		match(tok_do);
		statement();
	}
	else if(token.tok_type==tok_call)
	{
		match(tok_call);
		match(tok_identifier);
		match(tok_lcurve);
		if(token.tok_type!=tok_rcurve)
		{
			exp();
			while(token.tok_type==tok_comma)
			{
				match(tok_comma);
				exp();
			}
		}
		match(tok_rcurve);
	}
	else if(token.tok_type==tok_begin)
		body();
	else if(token.tok_type==tok_read)
	{
		match(tok_read);
		match(tok_lcurve);
		match(tok_identifier);
		while(token.tok_type==tok_comma)
		{
			match(tok_comma);
			match(tok_identifier);
		}
		match(tok_rcurve);
	}
	else if(token.tok_type==tok_write)
	{
		match(tok_write);
		match(tok_lcurve);
		exp();
		while(token.tok_type==tok_comma)
		{
			match(tok_comma);
			exp();
		}
		match(tok_rcurve);
	}
	return;
}

void lexp()
{
	if(token.tok_type==tok_odd)
	{
		match(tok_odd);
		exp();
	}
	else
	{
		exp();
		lop();
		exp();
	}
	return;
}

void exp()
{
	if(token.tok_type==tok_add || token.tok_type==tok_sub)
		match(token.tok_type);
	term();
	while(token.tok_type==tok_add || token.tok_type==tok_sub)
	{
		match(token.tok_type);
		term();
	}
	return;
}

void term()
{
	factor();
	while(token.tok_type==tok_mul || token.tok_type==tok_div)
	{
		match(token.tok_type);
		factor();
	}
	return;
}

void factor()
{
	if(token.tok_type==tok_identifier)
		match(tok_identifier);
	else if(token.tok_type==tok_number)
		match(tok_number);
	else if(token.tok_type==tok_lcurve)
	{
		match(tok_lcurve);
		exp();
		match(tok_rcurve);
	}
	return;
}

void lop()
{
	if(token.tok_type==tok_equal || token.tok_type==tok_neq || token.tok_type==tok_less || token.tok_type==tok_leq || token.tok_type==tok_great || token.tok_type==tok_geq)
		match(token.tok_type);
	else
		die("["+line_code+"] expect compare operator.",line,line_code.length());
	return;
}

#endif
