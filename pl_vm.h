#ifndef __PL_VM_H__
#define __PL_VM_H__

int* vm_stack;

void vm_init()
{
    vm_stack=new int[4096];
    return;
}

void vm_exit()
{
    delete []vm_stack;
    return;
}

#endif