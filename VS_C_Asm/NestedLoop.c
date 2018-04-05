#include <stdio.h>

/*
Demo of designed Nested Loop in assembly 
*/

int __declspec(naked) NestedWhileLoop()
{
    __asm
    {
        push ebp
        mov ebp, esp
        sub esp, 0x20

        push esi
        push edi
        push ebx

        lea edi, dword ptr es:[esp + 0xc]
        mov eax, 0xcccccccc
        mov ecx, 0x8
        rep stosd

        ;The variable of x
        mov dword ptr es:[esp - 0x4], 0
        ;The variable of y
        mov dword ptr es:[esp - 0x8], 0

        ;The limitation of x
        mov dword ptr es:[esp - 0xc], 40
        ;The limitation of y
        mov dword ptr es:[esp - 0x10], 25

        ;Initialization of the registers
        xor ebx, ebx 
        xor eax, eax 

        ;start the outer loop 
        OuterLoopStart:
        mov ebx, dword ptr es:[esp - 0x8]
        cmp ebx, dword ptr es:[esp - 0x10]
        jg OutterLoopEnd

        ;Start the inner loop
        mov ebx, dword ptr es:[esp - 0x4]
        cmp ebx, dword ptr es:[esp - 0xc]
        jg InnerLoopEnd

        inc eax

        add dword ptr es:[esp - 0x8], 1
        jmp InnerLoopStart

        InnerLoopEnd:

        add dword ptr es:[esp - 0x4], 1
        jmp OuterLoopStart

        OuterLoopEnd:

        ;End the outer loop

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
    int iter = 0;
    iter = NestedWhileLoop();
    printf("%d\n", iter);
    return 0;
}