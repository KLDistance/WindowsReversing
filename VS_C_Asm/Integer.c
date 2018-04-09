/*
This creates a type of handle on large integer calculations.

Construction: 
64 bytes large number (iter = 16 sections)
*/

#include <stdio.h>
#include <malloc.h>

void __declspec(naked) InitInt(int pAddr)
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

		mov ebx, dword ptr es : [ebp + 0x8]

		push 0x40
		call malloc
		add esp, 4

		mov dword ptr es : [ebx], eax

		lea edi, dword ptr es : [eax]
		mov eax, 0x884953423
		mov ecx, 0x10
		rep stosd

		pop ebx
		pop edi
		pop esi

		mov esp, ebp
		pop ebp
		ret
	}
}

void __declspec(naked) Addition(int addr1, int addr2)
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

		mov ebx, dword ptr es : [ebp + 0x8]
		mov dword ptr es : [ebp - 0x4], ebx
		mov ecx, dword ptr es : [ebp + 0xc]
		mov dword ptr es : [ebp - 0x8], ecx

		mov eax, dword ptr es : [ebx]
		add dword ptr es : [ecx], eax

		mov eax, dword ptr es : [ebx + 0x4]
		adc dword ptr es : [ecx + 0x4], eax

		mov eax, dword ptr es : [ebx + 0x8]
		adc dword ptr es : [ecx + 0x8], eax

		mov eax, dword ptr es : [ebx + 0xc]
		adc dword ptr es : [ecx + 0xc], eax

		mov eax, dword ptr es : [ebx + 0x10]
		adc dword ptr es : [ecx + 0x10], eax

		mov eax, dword ptr es : [ebx + 0x14]
		adc dword ptr es : [ecx + 0x14], eax

		mov eax, dword ptr es : [ebx + 0x18]
		adc dword ptr es : [ecx + 0x18], eax

		mov eax, dword ptr es : [ebx + 0x1c]
		adc dword ptr es : [ecx + 0x1c], eax

		mov eax, dword ptr es : [ebx + 0x20]
		adc dword ptr es : [ecx + 0x20], eax

		mov eax, dword ptr es : [ebx + 0x24]
		adc dword ptr es : [ecx + 0x24], eax

		mov eax, dword ptr es : [ebx + 0x28]
		adc dword ptr es : [ecx + 0x28], eax

		mov eax, dword ptr es : [ebx + 0x2c]
		adc dword ptr es : [ecx + 0x2c], eax

		mov eax, dword ptr es : [ebx + 0x30]
		adc dword ptr es : [ecx + 0x30], eax

		mov eax, dword ptr es : [ebx + 0x34]
		adc dword ptr es : [ecx + 0x34], eax

		mov eax, dword ptr es : [ebx + 0x38]
		adc dword ptr es : [ecx + 0x38], eax

		mov eax, dword ptr es : [ebx + 0x3c]
		adc dword ptr es : [ecx + 0x3c], eax

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

		mov eax, dword ptr es : [ebp + 0x8]
		push eax
		call free
		add esp, 4

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
	int *Int1 = NULL;
	int *Int2 = NULL;

	InitInt(&Int1);
	InitInt(&Int2);

	Addition(Int1, Int2);

	int i;
	char nonZeroState = 0;
	for (i = 0; i < 15; i++)
	{
		if (Int2[15 - i] != 0 || nonZeroState)
		{
			printf("%X", Int2[15 - i]);
		}
		nonZeroState = (Int2[15 - i] != 0) ? 1 : 0;
	}

	PassPointerFree(Int1);
	PassPointerFree(Int2);

	return 0;
}