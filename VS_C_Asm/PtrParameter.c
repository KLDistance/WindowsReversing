#include <stdio.h>
#include <malloc.h>

/*
Designed dynamic allocation using parameter 
*/

int __declspec(naked) PassPointerAllocation(int addr)
{
    __asm
    {
        push ebp
        mov ebp, esp
        sub esp, 0x40

        push esi
        push edi
        push ebx

        lea edi, dword ptr es:[esp + 0xc]
        mov eax, 0xcccccccc
        mov ecx, 0x10
        rep stosd

;Add dynamic allocation here
        
        push 0x4
        call malloc
        add esp, 4

;Move the pointer of the space into the pointer to the parameter

        mov ebx, dword ptr es:[esp + 0xc]
        mov dword ptr es:[ebx], eax

;End of the operation of the dynamic allocation 

        pop ebx
        pop edi
        pop esi

        mov esp, ebp
        pop ebp
        ret
    }
}

int __declspec(naked) PassPointerFree(int addr)
{
    __asm
    {
        push esp
        mov ebp, esp
        sub esp, 0x40

        push esi
        push edi
        push esi

        lea edi, dword ptr es:[esp + 0xc]
        mov eax, 0xcccccccc
        mov ecx, 0x10
        rep stosd

;Free the dynamic allocation here

        mov eax, dword ptr es:[ebp + 0x8]
        push eax
        call free
        add esp, 4

;End of the operation of the dynamic allocation

        pop ebx
        pop edi
        pop esi

        mov esp, ebp
        pop ebp
        ret
    }
}

int main(void)
{
    int addr = 0;
    PassPointerAllocation(addr);
    printf("%d\n", *((int*)addr));
    PassPointerFree(addr);
    return 0;
}


