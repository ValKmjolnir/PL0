#ifndef __PL_PARSE_H__
#define __PL_PARSE_H__

void match(int);
ast parse();
ast prog();
ast block();
ast condecl();
ast _const();
ast vardecl();
void proc(ast&);
ast body();
ast statement();
ast lexp();
ast exp();
ast term();
ast factor();
ast lop();

void panic()
{
	while(token.tok_type!=tok_semi && token.tok_type!=tok_begin && token.tok_type!=tok_end && token.tok_type!=tok_do && token.tok_type!=tok_then)
	{
		next();
		if(token.tok_type==tok_eof)
			break;
	}
	return;
}

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
		// panic mode: if type does not match,get the next token until matches ';'
		panic();
	}
	next();
	return;
}

ast parse()
{
	next();
	ast root;
	root.setline(line);
	root.settype(ast_root);
	root.addchild(prog());
	return root;
}

ast prog()
{
	match(tok_program);
	ast node;
	node.setline(line);
	node.settype(ast_prog);
	node.setstr(token.content);
	match(tok_identifier);
	match(tok_semi);
	node.addchild(block());
	return node;
}

ast block()
{
	ast node;
	node.setline(line);
	node.settype(ast_block);
	if(token.tok_type==tok_const)
		node.addchild(condecl());
	if(token.tok_type==tok_var)
		node.addchild(vardecl());
	if(token.tok_type==tok_procedure)
		proc(node);
	node.addchild(body());
	return node;
}

ast condecl()
{
	ast node;
	node.setline(line);
	node.settype(ast_condecl);
	match(tok_const);
	node.addchild(_const());
	while(token.tok_type==tok_comma)
	{
		match(tok_comma);
		node.addchild(_const());
	}
	match(tok_semi);
	return node;
}

ast _const()
{
	ast node;
	node.setline(line);
	node.settype(ast_id);
	node.setstr(token.content);
	
	match(tok_identifier);
	match(tok_assign);
	
	ast num;
	num.setline(line);
	num.settype(ast_number);
	num.setstr(token.content);
	node.addchild(num);
	
	match(tok_number);
	return node;
}

ast vardecl()
{
	ast node;
	node.setline(line);
	node.settype(ast_vardecl);
	
	match(tok_var);
	
	ast id;
	id.setline(line);
	id.settype(ast_id);
	id.setstr(token.content);
	node.addchild(id);
	
	match(tok_identifier);
	while(token.tok_type==tok_comma)
	{
		match(tok_comma);
		
		ast id;
		id.setline(line);
		id.settype(ast_id);
		id.setstr(token.content);
		node.addchild(id);
		
		match(tok_identifier);
	}
	match(tok_semi);
	return node;
}

// <proc> -> procedure <id> ([<id>{,<id>}]);<block>{;<proc>}
// get reference of root to add proc as it's children
// proc node cannot get proc node as it's children,it is awful
void proc(ast& root)
{
	ast node;
	node.setline(line);
	node.settype(ast_proc);
	
	match(tok_procedure);
	
	node.setstr(token.content);
	
	match(tok_identifier);
	
	ast para;
	para.setline(line);
	para.settype(ast_parameter);
	
	match(tok_lcurve);
	if(token.tok_type==tok_identifier)
	{
		ast id;
		id.setline(line);
		id.settype(ast_id);
		id.setstr(token.content);
		para.addchild(id);
		
		match(tok_identifier);
		while(token.tok_type==tok_comma)
		{
			match(tok_comma);
			
			ast id;
			id.setline(line);
			id.settype(ast_id);
			id.setstr(token.content);
			para.addchild(id);
			
			match(tok_identifier);
		}
	}
	
	node.addchild(para);
	match(tok_rcurve);
	match(tok_semi);
	
	node.addchild(block());
	root.addchild(node);
	while(token.tok_type==tok_semi)
	{
		match(tok_semi);
		proc(root);
	}
	return;
}

ast body()
{
	ast node;
	node.setline(line);
	node.settype(ast_block);
	
	match(tok_begin);
	node.addchild(statement());
	while(token.tok_type==tok_semi)
	{
		match(tok_semi);
		node.addchild(statement());
	}
	match(tok_end);
	return node;
}

ast statement()
{
	ast node;
	node.setline(line);
	if(token.tok_type==tok_identifier)
	{
		node.settype(ast_assign);
		node.setstr(token.content);
		
		match(tok_identifier);
		match(tok_assign);
		node.addchild(exp());
	}
	else if(token.tok_type==tok_if)
	{
		node.settype(ast_condition);
		
		ast _if;
		_if.setline(line);
		_if.settype(ast_if);
		match(tok_if);
		_if.addchild(lexp());
		match(tok_then);
		_if.addchild(statement());
		node.addchild(_if);
		if(token.tok_type==tok_else)
		{
			ast _else;
			_else.setline(line);
			_else.settype(ast_else);
			match(tok_else);
			_else.addchild(statement());
			node.addchild(_else);
		}
	}
	else if(token.tok_type==tok_while)
	{
		node.settype(ast_while);
		match(tok_while);
		node.addchild(lexp());
		match(tok_do);
		node.addchild(statement());
	}
	else if(token.tok_type==tok_call)
	{
		node.settype(ast_call);
		
		match(tok_call);
		node.setstr(token.content);
		
		match(tok_identifier);
		match(tok_lcurve);
		if(token.tok_type!=tok_rcurve)
		{
			node.addchild(exp());
			while(token.tok_type==tok_comma)
			{
				match(tok_comma);
				node.addchild(exp());
			}
		}
		match(tok_rcurve);
	}
	else if(token.tok_type==tok_begin)
		node=body();
	else if(token.tok_type==tok_read)
	{
		node.settype(ast_read);
		
		match(tok_read);
		match(tok_lcurve);
		
		ast id;
		id.setline(line);
		id.settype(ast_id);
		id.setstr(token.content);
		node.addchild(id);
		
		match(tok_identifier);
		while(token.tok_type==tok_comma)
		{
			match(tok_comma);
			
			id.setline(line);
			id.settype(ast_id);
			id.setstr(token.content);
			node.addchild(id);
		
			match(tok_identifier);
		}
		match(tok_rcurve);
	}
	else if(token.tok_type==tok_write)
	{
		node.settype(ast_write);
		
		match(tok_write);
		match(tok_lcurve);
		node.addchild(exp());
		while(token.tok_type==tok_comma)
		{
			match(tok_comma);
			node.addchild(exp());
		}
		match(tok_rcurve);
	}
	else
		die("["+line_code+"] expect a statement after \";\".",line,line_code.length());
	// if this terminal symbol is not in the FIRST set of <statement>
	// then print error to get a correct statement
	return node;
}

ast lexp()
{
	ast node;
	node.setline(line);
	if(token.tok_type==tok_odd)
	{
		node.settype(ast_odd);
		match(tok_odd);
		node.addchild(exp());
	}
	else
	{
		ast tmp1,tmp2;
		tmp1=exp();
		node=lop();
		tmp2=exp();
		node.addchild(tmp1);
		node.addchild(tmp2);
	}
	return node;
}

ast exp()
{
	ast node;
	node.setline(line);

	if(token.tok_type==tok_add || token.tok_type==tok_sub)
	{
		node.settype(token.tok_type==tok_add? ast_add:ast_sub);
		match(token.tok_type);
	}
	if(node.gettype()!=ast_root)
		node.addchild(term());
	else
		node=term();
	while(token.tok_type==tok_add || token.tok_type==tok_sub)
	{
		ast tmp;
		tmp.setline(line);
		tmp.settype(token.tok_type==tok_add? ast_add:ast_sub);
		tmp.addchild(node);
		match(token.tok_type);
		tmp.addchild(term());
		node=tmp;
	}
	return node;
}

ast term()
{
	ast node;
	node=factor();
	while(token.tok_type==tok_mul || token.tok_type==tok_div)
	{
		ast tmp;
		tmp.setline(line);
		tmp.settype(token.tok_type==tok_mul?ast_mul:ast_div);
		tmp.addchild(node);
		match(token.tok_type);
		tmp.addchild(factor());
		node=tmp;
	}
	return node;
}

ast factor()
{
	ast node;
	if(token.tok_type==tok_identifier)
	{
		node.setline(line);
		node.settype(ast_id);
		node.setstr(token.content);
		match(tok_identifier);
	}
	else if(token.tok_type==tok_number)
	{
		node.setline(line);
		node.settype(ast_number);
		node.setstr(token.content);
		match(tok_number);
	}
	else if(token.tok_type==tok_lcurve)
	{
		match(tok_lcurve);
		node=exp();
		match(tok_rcurve);
	}
	else
	{
		die("["+line_code+"] expect a factor here.",line,line_code.length());
		panic();
	}
	return node;
}

ast lop()
{
	ast node;
	node.setline(line);
	if(token.tok_type==tok_equal || token.tok_type==tok_neq || token.tok_type==tok_less || token.tok_type==tok_leq || token.tok_type==tok_great || token.tok_type==tok_geq)
	{
		int atype;
		switch(token.tok_type)
		{
			case tok_equal: atype=ast_eq;    break;
			case tok_neq:   atype=ast_neq;   break;
			case tok_less:  atype=ast_less;  break;
			case tok_leq:   atype=ast_leq;   break;
			case tok_great: atype=ast_great; break;
			case tok_geq:   atype=ast_geq;   break;
		}
		node.settype(atype);
		match(token.tok_type);
	}
	else
	{
		die("["+line_code+"] expect compare operator.",line,line_code.length());
		panic();
	}
	return node;
}

#endif
