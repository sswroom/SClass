#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/LRGBLimiter.h"

/*void LRGBLimiter_LimitImageLRGB(UInt8 *imgPtr, OSInt w, OSInt h)
{
	_asm
	{
		mov eax,w
		mov esi,imgPtr
		mul h
		mov ecx,eax
		ALIGN 16
lilrgblop:
		movsx eax,word ptr [esi]
		mov edx,eax //minV
		mov ebx,eax //maxV
		movsx eax,word ptr [esi+2]
		cmp edx,eax
		cmovg edx,eax
		cmp ebx,eax
		cmovl ebx,eax
		movsx eax,word ptr [esi+4]
		cmp edx,eax
		cmovg edx,eax
		cmp ebx,eax
		cmovl ebx,eax

		cmp edx,16384
		jge lilrgblop5
		cmp ebx,16384
		jle lilrgblop4

		push ecx
		mov ecx,ebx
		mov edi,edx
		movsx eax,word ptr [esi]
		mov edx,16384
		sub eax,edi
		sub edx,edi
		sub ecx,edi
		imul edx
		idiv ecx
		add eax,edi
		mov word ptr [esi],ax

		movsx eax,word ptr [esi+2]
		mov edx,16384
		sub eax,edi
		sub edx,edi
		imul edx
		idiv ecx
		add eax,edi
		mov word ptr [esi+2],ax

		movsx eax,word ptr [esi+4]
		mov edx,16384
		sub eax,edi
		sub edx,edi
		imul edx
		idiv ecx
		add eax,edi
		mov word ptr [esi+4],ax
		pop ecx
		jmp lilrgblop4
		ALIGN 16
lilrgblop5:
		mov word ptr [esi],16383
		mov word ptr [esi+2],16383
		mov word ptr [esi+4],16383

		ALIGN 16
lilrgblop4:
		add esi,8
		dec ecx
		jnz lilrgblop
	}
}
*/