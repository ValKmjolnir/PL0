#ifndef __PL_CODE_H__
#define __PL_CODE_H__

enum opcode_type          {op_nop,op_lit,op_opr,op_lod,op_sto,op_cal,op_int,op_jmp,op_jpc,op_red,op_wrt};
const char* opcode_name[]={"nop" ,"lit" ,"opr" ,"lod" ,"sto" ,"cal" ,"int" ,"jmp" ,"jpc" ,"red" ,"wrt" ,NULL};
enum opr_type{calc_ret,calc_nega,calc_plus,calc_minus,calc_mult,calc_div,calc_odd,calc_eq,calc_neq,calc_les,calc_leq,calc_grt,calc_geq};
enum sym_type{sym_var=1,sym_const,sym_proc};

struct symbol
{
    std::string name;
    int type;
    int number;
    int procedure_arg_size;
    symbol(int t=sym_var,int num=0,std::string n="",int arg=0)
    {
        type=t;
        number=num;
        name=n;
        procedure_arg_size=arg;
        return;
    }
    symbol(const symbol& tmp)
    {
        name=tmp.name;
        number=tmp.number;
        type=tmp.type;
        procedure_arg_size=tmp.procedure_arg_size;
        return;
    }
};

struct bytecode
{
    unsigned char opcode;
    unsigned char level;
    int opnum;
    bytecode(unsigned char oc,unsigned char lvl,int num)
    {
        opcode=oc;
        level=lvl;
        opnum=num;
        return;
    }
    bytecode(const bytecode& tmp)
    {
        opcode=tmp.opcode;
        level=tmp.level;
        opnum=tmp.opnum;
        return;
    }
};

std::list<std::vector<symbol> > symbol_table;
std::vector<bytecode> exec_code;

void emit(unsigned char opcode=op_nop,unsigned char level=0,int opnum=0)
{
    bytecode new_code(opcode,level,opnum);
    exec_code.push_back(new_code);
    return;
}

void add_scope()
{
    std::vector<symbol> new_scope;
    symbol_table.push_front(new_scope);
    return;
}

// type=sym_const,const_number means the constant
// type=sym_proc ,const_number means the entry
void add_new_symbol(int type=sym_var,int const_number=0,std::string symbol_name="")
{
    int size=symbol_table.front().size();
    for(int i=0;i<size;++i)
        if(symbol_table.front()[i].name==symbol_name)
        {
            die("redefinition of symbol: "+symbol_name);
            return;
        }
    symbol new_symbol(type,const_number,symbol_name);
    symbol_table.front().push_back(new_symbol);
    return;
}

void set_procedure_arg_size(std::string symbol_name,int arg_size)
{
    for(std::list<std::vector<symbol> >::iterator i=symbol_table.begin();i!=symbol_table.end();++i)
    {
        int size=i->size();
        for(int j=0;j<size;++j)
            if((*i)[j].name==symbol_name)
            {
                (*i)[j].procedure_arg_size=arg_size;
                return;
            }
    }
    return;
}

int get_symbol_place(std::string symbol_name)
{
    int level=0;
    for(std::list<std::vector<symbol> >::iterator i=symbol_table.begin();i!=symbol_table.end();++i)
    {
        int size=i->size();
        int cnt=0;
        for(int j=0;j<size;++j)
        {
            if((*i)[j].type==sym_var)++cnt;
            if((*i)[j].name==symbol_name)
                return ((level<<16)+cnt-1);
        }
        ++level;
    }
    die("undefined symbol: "+symbol_name);
    return -1;
}

int get_arg_size(std::string symbol_name)
{
    for(std::list<std::vector<symbol> >::iterator i=symbol_table.begin();i!=symbol_table.end();++i)
    {
        int size=i->size();
        for(int j=0;j<size;++j)
            if((*i)[j].name==symbol_name)
                return (*i)[j].procedure_arg_size;
    }
    return 0;
}

symbol get_symbol(std::string symbol_name)
{
    for(std::list<std::vector<symbol> >::iterator i=symbol_table.begin();i!=symbol_table.end();++i)
    {
        int size=i->size();
        for(int j=0;j<size;++j)
            if((*i)[j].name==symbol_name)
                return (*i)[j];
    }
    die("undefined symbol: "+symbol_name);
    symbol error_sym(-1);
    return error_sym;
}

int to_number(std::string num)
{
    int sum=0;
    for(int i=0;i<num.length();++i)
        sum=sum*10+(num[i]-'0');
    return sum;
}

void del_scope()
{
    symbol_table.pop_front();
    return;
}
#endif