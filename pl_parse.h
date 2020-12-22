#ifndef __PL_PARSE_H__
#define __PL_PARSE_H__

void match(int);
ast parse();
ast prog();
ast block(ast);
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
		die("["+line_code+"] expect token \""+expect+"\".");
		// panic mode: if type does not match,get the next token until matches ';'
		panic();
	}
	next();
	return;
}

ast parse()
{
	next();
	ast root(line,ast_root);
	root.addchild(prog());
	return root;
}

ast prog()
{
	match(tok_program);
	ast node(line,ast_prog,token.content);
	match(tok_identifier);
	match(tok_semi);
	add_scope();

	ast nullnode(0,0);
	node.addchild(block(nullnode));
	del_scope();
	return node;
}

ast block(ast para)
{
	int var_size=para.getchild().size();
	ast node(line,ast_block);
	if(token.tok_type==tok_const)
		node.addchild(condecl());
	if(token.tok_type==tok_var)
	{
		node.addchild(vardecl());
		var_size+=node.getchild().back().getchild().size();
	}
	int entry_label=exec_code.size();
	emit(op_jmp,0,0);
	if(token.tok_type==tok_procedure)
		proc(node);
	exec_code[entry_label].opnum=exec_code.size();
	emit(op_int,0,var_size);
	for(int i=para.getchild().size()-1;i>=0;--i)
	{
		int data=get_symbol_place(para.getchild()[i].getstr());
		emit(op_sto,data>>16,data&0xffff);
	}
	node.addchild(body());
	return node;
}

ast condecl()
{
	ast node(line,ast_condecl);
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
	ast node(line,ast_id,token.content);
	std::string const_symbol=token.content;
	
	match(tok_identifier);
	match(tok_assign);
	
	ast num(line,ast_number,token.content);
	add_new_symbol(sym_const,to_number(token.content),const_symbol);
	node.addchild(num);
	
	match(tok_number);
	return node;
}

ast vardecl()
{
	ast node(line,ast_vardecl);
	
	match(tok_var);
	
	ast id(line,ast_id,token.content);
	add_new_symbol(sym_var,0,token.content);
	node.addchild(id);
	
	match(tok_identifier);
	while(token.tok_type==tok_comma)
	{
		match(tok_comma);
		
		ast id(line,ast_id,token.content);
		add_new_symbol(sym_var,0,token.content);
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
	ast node(line,ast_proc);
	
	match(tok_procedure);
	
	node.setstr(token.content);
	std::string proc_name=token.content;
	add_new_symbol(sym_proc,exec_code.size(),proc_name);
	
	match(tok_identifier);
	
	ast para(line,ast_parameter);

	add_scope();
	match(tok_lcurve);
	if(token.tok_type==tok_identifier)
	{
		ast id(line,ast_id,token.content);
		add_new_symbol(sym_var,0,token.content);
		para.addchild(id);
		
		match(tok_identifier);
		while(token.tok_type==tok_comma)
		{
			match(tok_comma);
			
			ast id(line,ast_id,token.content);
			add_new_symbol(sym_var,0,token.content);
			para.addchild(id);
			
			match(tok_identifier);
		}
	}

	node.addchild(para);
	set_procedure_arg_size(proc_name,para.getchild().size());
	match(tok_rcurve);
	match(tok_semi);
	
	node.addchild(block(para));
	root.addchild(node);
	emit(op_opr,0,calc_ret);
	del_scope();
	while(token.tok_type==tok_semi)
	{
		match(tok_semi);
		proc(root);
	}
	return;
}

ast body()
{
	ast node(line,ast_block);
	
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
	ast node(line);
	if(token.tok_type==tok_identifier)
	{
		node.settype(ast_assign);
		node.setstr(token.content);
		symbol sym=get_symbol(token.content);
		
		match(tok_identifier);
		match(tok_assign);
		node.addchild(exp());
		
		if(sym.type==sym_var)
		{
			int data=get_symbol_place(sym.name);
			emit(op_sto,data>>16,data&0xffff);
		}
		else
			die("unchangable symbol: "+sym.name);
	}
	else if(token.tok_type==tok_if)
	{
		node.settype(ast_condition);
		
		ast _if(line,ast_if);
		match(tok_if);
		_if.addchild(lexp());
		int jmp_false_label=exec_code.size();
		emit(op_jpc,0,0);
		match(tok_then);
		_if.addchild(statement());
		int jmp_out=exec_code.size();
		emit(op_jmp,0,0);
		node.addchild(_if);
		exec_code[jmp_false_label].opnum=exec_code.size();
		if(token.tok_type==tok_else)
		{
			ast _else(line,ast_else);
			match(tok_else);
			_else.addchild(statement());
			node.addchild(_else);
		}
		exec_code[jmp_out].opnum=exec_code.size();
	}
	else if(token.tok_type==tok_while)
	{
		int while_jmp_label=exec_code.size();
		node.settype(ast_while);
		match(tok_while);
		node.addchild(lexp());
		match(tok_do);
		int while_check_label=exec_code.size();
		emit(op_jpc,0,0);
		node.addchild(statement());
		emit(op_jmp,0,while_jmp_label);
		exec_code[while_check_label].opnum=exec_code.size();
	}
	else if(token.tok_type==tok_call)
	{
		node.settype(ast_call);
		
		match(tok_call);
		node.setstr(token.content);
		symbol sym=get_symbol(token.content);
		if(sym.type!=sym_proc)
			die(token.content+" is not a procedure");
		
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
		emit(op_cal,get_symbol_place(sym.name)>>16,sym.number);
		if(sym.type==sym_proc)
		{
			if(node.getchild().size()>sym.procedure_arg_size)
				die("too much arguments",node.getline());
			else if(node.getchild().size()<sym.procedure_arg_size)
				die("lack argument(s)",node.getline());
		}
	}
	else if(token.tok_type==tok_begin)
		node=body();
	else if(token.tok_type==tok_read)
	{
		node.settype(ast_read);
		match(tok_read);
		match(tok_lcurve);
		
		ast id(line,ast_id,token.content);
		symbol sym=get_symbol(token.content);
		if(sym.type==sym_var)
		{
			int data=get_symbol_place(token.content);
			emit(op_red,data>>16,data&0xffff);
		}
		else
			die("unchangable symbol: "+token.content);
		node.addchild(id);
		
		match(tok_identifier);
		while(token.tok_type==tok_comma)
		{
			match(tok_comma);
			
			ast id(line,ast_id,token.content);
			symbol sym=get_symbol(token.content);
			if(sym.type==sym_var)
			{
				int data=get_symbol_place(token.content);
				emit(op_red,data>>16,data&0xffff);
			}
			else
				die("unchangable symbol: "+token.content);
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
		emit(op_wrt,0,0);
		while(token.tok_type==tok_comma)
		{
			match(tok_comma);
			node.addchild(exp());
			emit(op_wrt,0,0);
		}
		match(tok_rcurve);
	}
	else
	{ 
		die("["+line_code+"] expect a statement after \";\".");
		panic();
	}
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
		emit(op_opr,0,calc_odd);
	}
	else
	{
		ast tmp1,tmp2;
		tmp1=exp();
		node=lop();
		tmp2=exp();
		switch(node.gettype())
		{
			case ast_eq:    emit(op_opr,0,calc_eq); break;
			case ast_neq:   emit(op_opr,0,calc_neq);break;
			case ast_less:  emit(op_opr,0,calc_les);break;
			case ast_leq:   emit(op_opr,0,calc_leq);break;
			case ast_great: emit(op_opr,0,calc_grt);break;
			case ast_geq:   emit(op_opr,0,calc_geq);break;
		}
		node.addchild(tmp1);
		node.addchild(tmp2);
	}
	return node;
}

ast exp()
{
	ast node;
	node.setline(line);

	int unary_type=token.tok_type;
	if(token.tok_type==tok_add || token.tok_type==tok_sub)
	{
		node.settype(token.tok_type==tok_add? ast_add:ast_sub);
		match(token.tok_type);
	}
	if(node.gettype()!=ast_root)
		node.addchild(term());
	else
		node=term();
	if(unary_type==tok_sub)
		emit(op_opr,0,calc_nega);
	
	while(token.tok_type==tok_add || token.tok_type==tok_sub)
	{
		int type=token.tok_type;
		ast tmp(line,token.tok_type==tok_add? ast_add:ast_sub);
		tmp.addchild(node);
		match(token.tok_type);
		tmp.addchild(term());
		emit(op_opr,0,type==tok_add? calc_plus:calc_minus);
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
		int type=token.tok_type;
		ast tmp(line,token.tok_type==tok_mul?ast_mul:ast_div);
		tmp.addchild(node);
		match(token.tok_type);
		tmp.addchild(factor());
		emit(op_opr,0,type==tok_mul? calc_mult:calc_div);
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

		symbol sym=get_symbol(token.content);
		int type=sym.type;
		int const_num=sym.number;
		if(type==sym_var)
		{
			int data=get_symbol_place(token.content);
			emit(op_lod,data>>16,data&0xffff);
		}
		else if(type==sym_const)
			emit(op_lit,0,const_num);
		else
			die("not var or const: "+token.content);

		match(tok_identifier);
	}
	else if(token.tok_type==tok_number)
	{
		node.setline(line);
		node.settype(ast_number);
		node.setstr(token.content);
		emit(op_lit,0,to_number(token.content));
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
		die("["+line_code+"] expect a factor here.");
		panic();
	}
	return node;
}

ast lop()
{
	ast node(line);
	if(token.tok_type==tok_equal || token.tok_type==tok_neq || token.tok_type==tok_less || token.tok_type==tok_leq || token.tok_type==tok_great || token.tok_type==tok_geq)
	{
		int atype;
		switch(token.tok_type)
		{
			case tok_equal: atype=ast_eq;   break;
			case tok_neq:   atype=ast_neq;  break;
			case tok_less:  atype=ast_less; break;
			case tok_leq:   atype=ast_leq;  break;
			case tok_great: atype=ast_great;break;
			case tok_geq:   atype=ast_geq;  break;
		}
		node.settype(atype);
		match(token.tok_type);
	}
	else
	{
		die("["+line_code+"] expect compare operator.");
		panic();
	}
	return node;
}

#endif
