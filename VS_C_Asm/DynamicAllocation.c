#include <malloc.h>
#include <stdio.h>

int __declspec(naked) Allocation()
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

		push 0x4		; Pass in the parameter of dynamic allocation
		call malloc		; Call the function 
		add esp, 4		; Balance the stack

		mov dword ptr es:[ebp - 0x4], eax
		mov dword ptr es:[eax], 0x00004550

		pop ebx
		pop edi
		pop esi

		mov esp, ebp
		pop ebp
		ret
	}
}

int __declspec(naked) Free(int addr)
{
	__asm
	{
		push ebp
		mov ebp, esp
		sub esp, 0x40

		push esi
		push edi
		push ebx

		lea edi, dword ptr es : [esp + 0xc]
		mov eax, 0xcccccccc
		mov ecx, 0x10
		rep stosd

		mov eax, dword ptr es : [ebp + 0x8]	; Copy the parameter from previous stack
		push eax							; Pass in the addr to free
		call free							; Call the free function
		add esp, 4							; balance the stack

		pop ebx
		pop edi
		pop esi

		mov esp, ebp
		pop ebp
		ret
	}
}

int main()
{
	int addr = NULL;
	addr = Allocation();
	printf("%d", *((int*)addr));
	Free(addr);
	return 0;
}