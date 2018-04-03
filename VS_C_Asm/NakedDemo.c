// Run in Visual Studio

#include <stdio.h>

// Original function
int plus1(int p1, int p2, int p3)
{
	int la = 13;
	int lb = 15;
	int lc = 42;
	return la + lb + lc + p1 + p2 + p3;
}

// Equivalent naked function
int __declspec(naked) plus2(int p1, int p2, int p3)
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

		mov dword ptr ds:[ebp - 0x4], 0xd
		mov dword ptr ds:[ebp - 0x8], 0xf
		mov dword ptr ds:[ebp - 0xc], 0x2a
		
		xor eax, eax
		add eax, dword ptr ds:[ebp - 0x4]
		add eax, dword ptr ds:[ebp - 0x8]
		add eax, dword ptr ds:[ebp - 0xc]
		add eax, dword ptr ds:[ebp + 0x8]
		add eax, dword ptr ds:[ebp + 0xc]
		add eax, dword ptr ds:[ebp + 0x10]

		pop ebx
		pop edi
		pop esi

		mov esp, ebp
		pop ebp
		ret		
	}
}

// Main function
int main()
{
	printf("%d", plus2(26, 41, 88));
	return 0;
}