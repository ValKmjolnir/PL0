#ifndef __PL_CODE_H__
#define __PL_CODE_H__

enum
{
    op_nop,
    op_lit,
    op_opr,
    op_lod,
    op_sto,
    op_cal,
    op_int,
    op_jmp,
    op_jpc,
    op_red,op_wrt
};

struct symbol
{
    std::string name;
    bool is_const;
};

struct bytecode
{
    unsigned char opcode;
    unsigned char level;
    int opnum;
};

std::list<std::vector<symbol> > symbol_table;

void add_scope()
{
    std::vector<symbol> new_scope;
    symbol_table.push_front(new_scope);
    return;
}

void del_scope()
{
    symbol_table.pop_front();
    return;
}
#endif