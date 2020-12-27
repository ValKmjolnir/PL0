#ifndef __PL_VM_H__
#define __PL_VM_H__

int* vm_stack;
int vm_stack_top;
int stack_alloc_boundary;
int ptr;
int return_addr;
int static_link;
int dynamic_link;
/*
|  arguments ready to move  | <- this will move to the top of stack when calling a procedure
|___________________________|
|       alloc_boundary      | <- stack_alloc_boundary| stores the address of previous boundary,used to move arguments to the top
|___________________________|
|       values........      | <- local values here
|___________________________|
|       return_address      | <- return_addr| ptr will be assign as this value when return
|___________________________|
|    dynamic_link_address   | <- dynamic_link| stores the address of previous dynamic_link
|___________________________|
|    static_link_address    | <- static_link| stores the address of previous static_link,used in value searching
|___________________________|
*/

void opr_nop()
{
    // useless
    return;
}

void opr_lit()
{
    vm_stack[++vm_stack_top]=exec_code[ptr].opnum;
    return;
}

void opr_opr()
{
    switch(exec_code[ptr].opnum)
    {
        case calc_ret:
            ptr=vm_stack[return_addr];
            dynamic_link=vm_stack[dynamic_link];
            return_addr=dynamic_link+1;
            static_link=dynamic_link-1;
            stack_alloc_boundary=vm_stack[stack_alloc_boundary];
            vm_stack_top=stack_alloc_boundary;
            break;
        case calc_nega:  vm_stack[vm_stack_top]=-vm_stack[vm_stack_top];break;
        case calc_plus:  vm_stack[vm_stack_top-1]+=vm_stack[vm_stack_top];--vm_stack_top;break;
        case calc_minus: vm_stack[vm_stack_top-1]-=vm_stack[vm_stack_top];--vm_stack_top;break;
        case calc_mult:  vm_stack[vm_stack_top-1]*=vm_stack[vm_stack_top];--vm_stack_top;break;
        case calc_div:   vm_stack[vm_stack_top-1]/=vm_stack[vm_stack_top];--vm_stack_top;break;
        case calc_odd:   vm_stack[vm_stack_top]&=1;break;
        case calc_eq:    vm_stack[vm_stack_top-1]=(vm_stack[vm_stack_top-1]==vm_stack[vm_stack_top]);--vm_stack_top;break;
        case calc_neq:   vm_stack[vm_stack_top-1]=(vm_stack[vm_stack_top-1]!=vm_stack[vm_stack_top]);--vm_stack_top;break;
        case calc_les:   vm_stack[vm_stack_top-1]=(vm_stack[vm_stack_top-1]<vm_stack[vm_stack_top]); --vm_stack_top;break;
        case calc_leq:   vm_stack[vm_stack_top-1]=(vm_stack[vm_stack_top-1]<=vm_stack[vm_stack_top]);--vm_stack_top;break;
        case calc_grt:   vm_stack[vm_stack_top-1]=(vm_stack[vm_stack_top-1]>vm_stack[vm_stack_top]); --vm_stack_top;break;
        case calc_geq:   vm_stack[vm_stack_top-1]=(vm_stack[vm_stack_top-1]>=vm_stack[vm_stack_top]);--vm_stack_top;break;
    }
    return;
}

void opr_lod()
{
    int stc_lnk=static_link;
    int level=exec_code[ptr].level;
    for(int i=0;i<level;++i)
        stc_lnk=vm_stack[stc_lnk];
    stc_lnk+=3+exec_code[ptr].opnum;
    vm_stack[++vm_stack_top]=vm_stack[stc_lnk];
    return;
}

void opr_sto()
{
    int stc_lnk=static_link;
    int level=exec_code[ptr].level;
    for(int i=0;i<level;++i)
        stc_lnk=vm_stack[stc_lnk];
    stc_lnk+=3+exec_code[ptr].opnum;
    vm_stack[stc_lnk]=vm_stack[vm_stack_top--];
    return;
}

void opr_cal()
{
    // move arguments to the top of stack
    // after int,this space is the value space
    for(int i=vm_stack_top;i>stack_alloc_boundary;--i)
        vm_stack[i+2+vm_stack_top-stack_alloc_boundary]=vm_stack[i];
    
    int stc_lnk=static_link;
    int level=exec_code[ptr].level;
    for(int i=0;i<level;++i)
        stc_lnk=vm_stack[stc_lnk];
    vm_stack[vm_stack_top]=stc_lnk;
    static_link=vm_stack_top++;
    vm_stack[vm_stack_top]=dynamic_link;
    dynamic_link=vm_stack_top++;
    vm_stack[vm_stack_top]=ptr;
    return_addr=vm_stack_top++;

    ptr=exec_code[ptr].opnum-1;
    return;
}

void opr_int()
{
    vm_stack_top+=exec_code[ptr].opnum+1;
    vm_stack[vm_stack_top]=stack_alloc_boundary;
    stack_alloc_boundary=vm_stack_top;
    return;
}

void opr_jmp()
{
    ptr=exec_code[ptr].opnum-1;
    return;
}

void opr_jpc()
{
    if(!vm_stack[vm_stack_top--])
        ptr=exec_code[ptr].opnum-1;
    return;
}

void opr_red()
{
    int stc_lnk=static_link;
    int level=exec_code[ptr].level;
    for(int i=0;i<level;++i)
        stc_lnk=vm_stack[stc_lnk];
    stc_lnk+=3+exec_code[ptr].opnum;
    std::cin>>vm_stack[stc_lnk];
    return;
}

void opr_wrt()
{
    std::cout<<vm_stack[vm_stack_top--]<<'\n';
    return;
}

void debug()
{
    printf("debug--------------------------------\n");
    printf("0x%.8x %s 0x%.8x 0x%.8x\n",ptr,opcode_name[exec_code[ptr].opcode],exec_code[ptr].level,exec_code[ptr].opnum);
    for(int i=vm_stack_top;i>vm_stack_top-4 && i>=0;--i)
        printf("0x%.8x 0x%.8x (%d)\n",i,vm_stack[i],vm_stack[i]);
    printf("-------------------------------------\n");
    system("pause");
    return;
}

void (*func_ptr[])()={&opr_nop,&opr_lit,&opr_opr,&opr_lod,&opr_sto,&opr_cal,&opr_int,&opr_jmp,&opr_jpc,&opr_red,&opr_wrt};

void vm_init()
{
    vm_stack[0]=0;// static link
    vm_stack[1]=1;// dynamic link
    vm_stack[2]=0x7fffffff;// return address
    vm_stack_top=2;
    static_link=0;
    dynamic_link=1;
    return_addr=2;
    ptr=0;
    stack_alloc_boundary=0;
    return;
}

void vm_run()
{
    vm_stack=new int[8192];
    vm_init();
    int size=exec_code.size();
    for(;ptr<size;++ptr)
        func_ptr[exec_code[ptr].opcode]();
    delete []vm_stack;
    return;
}

void vm_debug()
{
    vm_stack=new int[8192];
    vm_init();
    int size=exec_code.size();
    for(;ptr<size;++ptr)
    {
        debug();
        func_ptr[exec_code[ptr].opcode]();
    }
    delete []vm_stack;
    return;
}
#endif