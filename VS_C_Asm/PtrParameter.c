#include <stdio.h>
#include <malloc.h>

/*
Designed dynamic allocation using pointer to parameters
*/

int __declspec(naked) PassPointerAllocation(int addr, int value)
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

		; Add dynamic allocation here

		push 0x4
		call malloc
		add esp, 4

		; Move the pointer of the space into the pointer to the parameter

		mov ebx, dword ptr es : [ebp + 0x8]
		mov dword ptr es : [ebx], eax

		; Copy value into the space

		mov ebx, dword ptr es : [ebp + 0xc]
		mov dword ptr es : [eax], ebx 

		; End of the operation of the dynamic allocation

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

		lea edi, dword ptr es : [esp + 0xc]
		mov eax, 0xcccccccc
		mov ecx, 0x10
		rep stosd

		; Free the dynamic allocation here

		mov eax, dword ptr es : [ebp + 0x8]
		push eax
		call free
		add esp, 4

		; End of the operation of the dynamic allocation

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
	// Address of a dynamic-allocated memory, using int instead of int *
	int addr = 0;
	// Mallocation
	PassPointerAllocation(&addr, 0x55555555);
	printf("%X\n", *((int*)addr));
	// Free
	PassPointerFree(addr);
	return 0;
}
