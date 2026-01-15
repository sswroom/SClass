#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSI420_RGB8.h"

extern "C" void CSI420_RGB8_do_yv12rgb(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *y2r, UInt8 *rgbGammaCorr)
{
	UInt8 *yuv2rgb = (UInt8*)y2r;
	IntOS cSub = (width >> 1) - 2;
	IntOS cSize = width << 3;
	IntOS cWidth4 = width >> 2;
	Int32 cofst = 0;//this->cofst;

	height = (height >> 1) - ((isLast & 1) << 1);

	UInt8 *tmpPtr;
	IntOS widthLeft;
//	Int32 buffTemp;
	IntOS i;

	i = width;
	tmpPtr = csLineBuff;
	while (i-- > 0)
	{
		*(Int64*)&tmpPtr[0] = *(Int64*)&yuv2rgb[yPtr[0] * 8];
		*(Int64*)&tmpPtr[8] = *(Int64*)&yuv2rgb[yPtr[1] * 8];
		yPtr += 2;
		tmpPtr += 16;

	}
	widthLeft = cSub;
/*
	_asm
	{
		mov ecx,cSub
		mov edi,csLineBuff
		mov edx,cSize
		mov esi,uPtr
		mov widthLeft,ecx
		mov ecx,vPtr


		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [ecx]
		movq xmm1,mmword ptr [ebx+eax*8 + 4096]

		movq xmm2,mmword ptr [edi]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [edi+edx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+edx],xmm2
		movq xmm2,mmword ptr [edi+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+8],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [edi+edx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+edx+8],xmm2

		add edi,16
		inc esi
		inc ecx

y2rflop3a:
		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [ecx]
		movq xmm1,mmword ptr [ebx+eax*8 + 4096]

		movq xmm2,mmword ptr [edi]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [edi+edx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+edx],xmm2
		movq xmm2,mmword ptr [edi+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+8],xmm2
		movq xmm2,mmword ptr [edi-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi-8],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [edi+edx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+edx+8],xmm2
		movq xmm2,mmword ptr [edi+edx-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+edx-8],xmm2

		add edi,16
		inc esi
		inc ecx
		dec widthLeft//ecx
		jnz y2rflop3a

		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [ecx]
		movq xmm1,mmword ptr [ebx+eax*8 + 4096]

		movq xmm2,mmword ptr [edi]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi],xmm2
		movq xmm2,mmword ptr [edi+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+8],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [edi+edx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+edx],xmm2
		movq xmm2,mmword ptr [edi-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi-8],xmm2
		movq xmm2,mmword ptr [edi+edx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+edx+8],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [edi+edx-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+edx-8],xmm2

		add edi,16
		inc esi
		inc ecx
		mov uPtr,esi
		mov vPtr,ecx

y2rflop:

		mov ecx,width
		shl ecx,1
		mov edi,csLineBuff2
		mov ebx,yuv2rgb
		mov esi,yPtr

y2rflop2:
		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8]
		movq mmword ptr [edi],xmm0
		inc esi
		add edi,8
		dec ecx
		jnz y2rflop2
		mov yPtr,esi

		mov esi,csLineBuff
		mov ecx,cSub
		mov edi,csLineBuff2
		mov buffTemp,esi
		mov edx,vPtr
		mov esi,uPtr
		mov widthLeft,ecx
		mov ecx,cSize

		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx]
		movq xmm1,mmword ptr [ebx+eax*8 + 4096]

		push esi
		mov esi,buffTemp

		movq xmm2,mmword ptr [edi]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [edi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx],xmm2
		movq xmm2,mmword ptr [esi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx],xmm2
		movq xmm2,mmword ptr [edi+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+8],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [edi+ecx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx+8],xmm2
		movq xmm2,mmword ptr [esi+ecx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx+8],xmm2

		add buffTemp, 16
		pop esi

		add edi,16
		inc esi
		inc edx

y2rflop3:
		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx]
		movq xmm1,mmword ptr [ebx+eax*8 + 4096]

		push esi
		mov esi,buffTemp

		movq xmm2,mmword ptr [edi]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [edi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx],xmm2
		movq xmm2,mmword ptr [esi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx],xmm2
		movq xmm2,mmword ptr [edi+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+8],xmm2
		movq xmm2,mmword ptr [edi-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi-8],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [edi+ecx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx+8],xmm2
		movq xmm2,mmword ptr [esi+ecx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx+8],xmm2
		movq xmm2,mmword ptr [edi+ecx-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx-8],xmm2
		movq xmm2,mmword ptr [esi+ecx-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx-8],xmm2

		add buffTemp, 16
		pop esi

		add edi,16
		inc esi
		inc edx
		dec widthLeft//ecx
		jnz y2rflop3

		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx]
		movq xmm1,mmword ptr [ebx+eax*8 + 4096]

		push esi
		mov esi,buffTemp

		movq xmm2,mmword ptr [edi]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi],xmm2
		movq xmm2,mmword ptr [edi+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+8],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [edi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx],xmm2
		movq xmm2,mmword ptr [esi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx],xmm2
		movq xmm2,mmword ptr [edi+ecx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx+8],xmm2
		movq xmm2,mmword ptr [esi+ecx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx+8],xmm2
		movq xmm2,mmword ptr [edi-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi-8],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [edi+ecx-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx-8],xmm2
		movq xmm2,mmword ptr [esi+ecx-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx-8],xmm2

		add buffTemp, 16
		pop esi

		add edi,16
		inc esi
		inc edx
		mov uPtr,esi
		mov vPtr,edx

		mov ecx,width
		mov esi,csLineBuff
		mov edi,dest
		mov ebx,rgbGammaCorr
y2rflop5:
		movzx eax,word ptr [esi+4]
		movzx edx,byte ptr [ebx+eax]
		shl edx,16
		movzx eax,word ptr [esi]
		mov dl,byte ptr [ebx+eax+131072]
		movzx eax,word ptr [esi+2]
		mov dh,byte ptr [ebx+eax+65536]
		mov dword ptr [edi],edx
		add esi,8
		add edi,4
		dec ecx
		jnz y2rflop5

		mov eax,dbpl
		add dest,eax

		mov ecx,width
		mov edi,dest
y2rflop6:
		movzx eax,word ptr [esi+4]
		movzx edx,byte ptr [ebx+eax]
		shl edx,16
		movzx eax,word ptr [esi]
		mov dl,byte ptr [ebx+eax+131072]
		movzx eax,word ptr [esi+2]
		mov dh,byte ptr [ebx+eax+65536]
		mov dword ptr [edi],edx
		add esi,8
		add edi,4
		dec ecx
		jnz y2rflop6

		mov eax,dbpl
		add dest,eax

		mov esi,csLineBuff
		mov edi,csLineBuff2
		mov csLineBuff2,esi
		mov csLineBuff,edi

		dec heightLeft
		jnz y2rflop

		test isLast,1
		jz yv2rflopexit

		mov ecx,width
		shl ecx,1
		mov edi,csLineBuff2
		mov ebx,yuv2rgb
		mov esi,yPtr

y2rflop2b:
		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8]
		movq mmword ptr [edi],xmm0
		inc esi
		add edi,8
		dec ecx
		jnz y2rflop2b
		mov yPtr,esi

		mov esi,csLineBuff
		mov ecx,cSub
		mov edi,csLineBuff2
		mov buffTemp,esi
		mov edx,vPtr
		mov esi,uPtr
		mov widthLeft,ecx
		mov ecx,cSize

		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx]
		movq xmm1,mmword ptr [ebx+eax*8 + 4096]

		push esi
		mov esi,buffTemp

		movq xmm2,mmword ptr [edi]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi],xmm2
		movq xmm2,mmword ptr [edi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [esi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx],xmm2
		movq xmm2,mmword ptr [edi+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+8],xmm2
		movq xmm2,mmword ptr [edi+ecx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx+8],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [esi+ecx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx+8],xmm2

		add buffTemp, 16
		pop esi

		add edi,16
		inc esi
		inc edx

y2rflop3b:
		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx]
		movq xmm1,mmword ptr [ebx+eax*8 + 4096]

		push esi
		mov esi,buffTemp

		movq xmm2,mmword ptr [edi]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi],xmm2
		movq xmm2,mmword ptr [edi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [esi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx],xmm2
		movq xmm2,mmword ptr [edi+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+8],xmm2
		movq xmm2,mmword ptr [edi-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi-8],xmm2
		movq xmm2,mmword ptr [edi+ecx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx+8],xmm2
		movq xmm2,mmword ptr [edi+ecx-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx-8],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [esi+ecx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx+8],xmm2
		movq xmm2,mmword ptr [esi+ecx-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx-8],xmm2

		add buffTemp, 16
		pop esi

		add edi,16
		inc esi
		inc edx
		dec widthLeft//ecx
		jnz y2rflop3b

		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx]
		movq xmm1,mmword ptr [ebx+eax*8 + 4096]

		push esi
		mov esi,buffTemp

		movq xmm2,mmword ptr [edi]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi],xmm2
		movq xmm2,mmword ptr [edi+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+8],xmm2
		movq xmm2,mmword ptr [edi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx],xmm2
		movq xmm2,mmword ptr [edi+ecx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx+8],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [esi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx],xmm2
		movq xmm2,mmword ptr [esi+ecx+8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx+8],xmm2
		movq xmm2,mmword ptr [edi-8]
		movq mmword ptr [edi-8],xmm2
		psraw xmm0,1
		psraw xmm1,1
		movq xmm2,mmword ptr [edi+ecx-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [edi+ecx-8],xmm2
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq xmm2,mmword ptr [esi+ecx-8]
		paddsw xmm2,xmm0
		paddsw xmm2,xmm1
		movq mmword ptr [esi+ecx-8],xmm2

		add buffTemp, 16
		pop esi

		add edi,16
		inc esi
		inc edx
		mov uPtr,esi
		mov vPtr,edx

		mov ecx,width
		mov esi,csLineBuff
		mov edi,dest
		mov ebx,rgbGammaCorr
y2rflop5b:
		movzx eax,word ptr [esi+4]
		movzx edx,byte ptr [ebx+eax]
		shl edx,16
		movzx eax,word ptr [esi]
		mov dl,byte ptr [ebx+eax+131072]
		movzx eax,word ptr [esi+2]
		mov dh,byte ptr [ebx+eax+65536]
		mov dword ptr [edi],edx
		add esi,8
		add edi,4
		dec ecx
		jnz y2rflop5b

		mov eax,dbpl
		add dest,eax

		mov ecx,width
		mov edi,dest
y2rflop6b:
		movzx eax,word ptr [esi+4]
		movzx edx,byte ptr [ebx+eax]
		shl edx,16
		movzx eax,word ptr [esi]
		mov dl,byte ptr [ebx+eax+131072]
		movzx eax,word ptr [esi+2]
		mov dh,byte ptr [ebx+eax+65536]
		mov dword ptr [edi],edx
		add esi,8
		add edi,4
		dec ecx
		jnz y2rflop6b

		mov eax,dbpl
		add dest,eax

		mov esi,csLineBuff
		mov edi,csLineBuff2
		mov csLineBuff2,esi
		mov csLineBuff,edi

		mov ecx,width
		mov esi,csLineBuff
		mov edi,dest
		mov ebx,rgbGammaCorr
y2rflop5c:
		movzx eax,word ptr [esi+4]
		movzx edx,byte ptr [ebx+eax]
		shl edx,16
		movzx eax,word ptr [esi]
		mov dl,byte ptr [ebx+eax+131072]
		movzx eax,word ptr [esi+2]
		mov dh,byte ptr [ebx+eax+65536]
		mov dword ptr [edi],edx
		add esi,8
		add edi,4
		dec ecx
		jnz y2rflop5c

		mov eax,dbpl
		add dest,eax

		mov ecx,width
		mov edi,dest
y2rflop6c:
		movzx eax,word ptr [esi+4]
		movzx edx,byte ptr [ebx+eax]
		shl edx,16
		movzx eax,word ptr [esi]
		mov dl,byte ptr [ebx+eax+131072]
		movzx eax,word ptr [esi+2]
		mov dh,byte ptr [ebx+eax+65536]
		mov dword ptr [edi],edx
		add esi,8
		add edi,4
		dec ecx
		jnz y2rflop6c

		mov eax,dbpl
		add dest,eax

		mov esi,csLineBuff
		mov edi,csLineBuff2
		mov csLineBuff2,esi
		mov csLineBuff,edi

yv2rflopexit:
	};*/
}

