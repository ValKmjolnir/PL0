#ifndef __PL_VM_H__
#define __PL_VM_H__

int* vm_stack;
int* vm_stack_top;
int ptr;

void vm_init()
{
    vm_stack=new int[4096];
    vm_stack_top=vm_stack;
    return;
}

void vm_exit()
{
    delete []vm_stack;
    return;
}

void opr_call()
{
    return;
}

void opr_int()
{
    int num=exec_code[ptr].opnum;
    for(int i=0;i<num;++i)
        *(++vm_stack_top)=0;
    return;
}

void opr_jmp()
{
    ptr=exec_code[ptr].opnum-1;
    return;
}

void opr_jpc()
{
    if(*vm_stack_top)
        ptr=exec_code[ptr].opnum-1;
    return;
}

void debug()
{
    int max_opr_size=(ptr-8>=0? ptr-8:0);
    for(int i=max_opr_size;i<=ptr;++i)
        printf("0x%.8x %s 0x%.8x 0x%.8x\n",i,opcode_name[exec_code[i].opcode],exec_code[i].level,exec_code[i].opnum);
    printf("---------------------\n");
    int cnt=8;
    for(int* i=vm_stack_top;i!=vm_stack;--i)
    {
        --cnt;
        printf("0x%p 0x%.8x\n",i,*i);
        if(!cnt)break;
    }
    system("pause");
    return;
}

void vm_run()
{
    int size=exec_code.size();
    for(ptr=0;ptr<size;++ptr)
    {
        ;
    }
    return;
}
#endif