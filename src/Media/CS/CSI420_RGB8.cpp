#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSI420_RGB8.h"
#include "Sync/Thread.h"

extern "C"
{
	void CSI420_RGB8_do_yv12rgb(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
}

/*void CSI420_RGB8_do_yv12rgb(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr)
{
	OSInt cSub = (width >> 1) - 2;
	OSInt cSize = width << 3;
	OSInt cWidth4 = width >> 2;
	Int32 cofst = 0;//this->cofst;

	Int32 heightLeft;
	Int32 widthLeft;
	Int32 buffTemp;


	_asm
	{
		cld
		mov ecx,height
		shr ecx,1
		mov ebx,isLast
		and ebx,1
		shl ebx,1
		sub ecx,ebx
		mov heightLeft,ecx

		mov ecx,width
		shr ecx,2
		mov edi,csLineBuff
		mov ebx,yuv2rgb
		mov esi,yPtr

//		test isFirst,1
//		jz y2rflop
y2rflop2a:
		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi],xmm0
		movzx eax,byte ptr [esi+1]
		movq xmm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+8],xmm0
		movzx eax,byte ptr [esi+2]
		movq xmm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+16],xmm0
		movzx eax,byte ptr [esi+3]
		movq xmm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+24],xmm0
		movzx eax,byte ptr [esi+4]
		movq xmm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+32],xmm0
		movzx eax,byte ptr [esi+5]
		movq xmm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+40],xmm0
		movzx eax,byte ptr [esi+6]
		movq xmm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+48],xmm0
		movzx eax,byte ptr [esi+7]
		movq xmm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+56],xmm0

		add esi,8
		add edi,64
		dec ecx
		jnz y2rflop2a

		mov yPtr,esi

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
	};
}*/

UInt32 Media::CS::CSI420_RGB8::WorkerThread(void *obj)
{
	CSI420_RGB8 *converter = (CSI420_RGB8*)obj;
	UOSInt threadId = converter->currId;
	THREADSTAT *ts = &converter->stats[threadId];

	ts->status = 1;
	converter->evtMain->Set();
	while (true)
	{
		ts->evt->Wait();
		if (ts->status == 2)
		{
			break;
		}
		else if (ts->status == 3)
		{
			CSI420_RGB8_do_yv12rgb(ts->yPtr, ts->uPtr, ts->vPtr, ts->dest, ts->width, ts->height, ts->dbpl, ts->isFirst, ts->isLast, ts->csLineBuff, ts->csLineBuff2, converter->yuv2rgb, converter->rgbGammaCorr);
			ts->status = 4;
			converter->evtMain->Set();
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain->Set();
	return 0;
}

Media::CS::CSI420_RGB8::CSI420_RGB8(const Media::ColorProfile *srcColor, const Media::ColorProfile *destColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess) : Media::CS::CSYUV_RGB8(srcColor, destColor, yuvType, colorSess)
{
	UOSInt i;
	this->nThread = Sync::Thread::GetThreadCnt();

	NEW_CLASS(evtMain, Sync::Event());
	stats = MemAlloc(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASS(stats[i].evt, Sync::Event());
		stats[i].status = 0;
		stats[i].csLineSize = 0;
		stats[i].csLineBuff = 0;
		stats[i].csLineBuff2 = 0;

		currId = i;
		Sync::Thread::Create(WorkerThread, this);
		while (stats[i].status == 0)
		{
			evtMain->Wait();
		}
	}
}

Media::CS::CSI420_RGB8::~CSI420_RGB8()
{
	UOSInt i = nThread;
	Bool exited;
	while (i-- > 0)
	{
		if (stats[i].status != 0)
		{
			stats[i].status = 2;
			stats[i].evt->Set();
		}
	}
	while (true)
	{
		exited = true;
		i = nThread;
		while (i-- > 0)
		{
			if (stats[i].status != 0)
			{
				if (stats[i].status == 2)
				{
					stats[i].evt->Set();
					exited = false;
					break;
				}
				else if (stats[i].status > 0)
				{
					stats[i].status = 2;
					stats[i].evt->Set();
					exited = false;
					break;
				}
				else
				{
					stats[i].evt->Set();
					exited = false;
					break;
				}
			}
		}
		if (exited)
			break;

		evtMain->Wait(100);
	}
	i = nThread;
	while (i-- > 0)
	{
		if (stats[i].csLineBuff)
		{
			MemFree(stats[i].csLineBuff);
			stats[i].csLineBuff = 0;
		}
		if (stats[i].csLineBuff2)
		{
			MemFree(stats[i].csLineBuff2);
			stats[i].csLineBuff2 = 0;
		}
		DEL_CLASS(stats[i].evt);
	}
	DEL_CLASS(evtMain);
	MemFree(stats);
}

///////////////////////////////////////////////////////////
void Media::CS::CSI420_RGB8::Convert(UInt8 *srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	this->UpdateTable();
	UOSInt isLast = 1;
	UOSInt isFirst = 0;
	UOSInt i = this->nThread;
	UOSInt lastHeight = dispHeight;
	UOSInt currHeight;
	UOSInt cSize = dispWidth << 4;

	while (i-- > 0)
	{
		if (i == 0)
			isFirst = 1;
		currHeight = MulDivOS(i, dispHeight, nThread) & ~1;

		stats[i].yPtr = srcPtr + srcStoreWidth * currHeight;
		stats[i].uPtr = srcPtr + srcStoreWidth * srcStoreHeight + ((srcStoreWidth * currHeight) >> 2);
		stats[i].vPtr = stats[i].uPtr + ((srcStoreWidth * srcStoreHeight) >> 2);
		stats[i].dest = ((UInt8*)destPtr) + destRGBBpl * currHeight;
		stats[i].isFirst = isFirst;
		stats[i].isLast = isLast;
		isLast = 0;
		stats[i].width = dispWidth;
		stats[i].height = lastHeight - currHeight;
		stats[i].dbpl = destRGBBpl;

		if (stats[i].csLineSize < dispWidth)
		{
			if (stats[i].csLineBuff)
				MemFree(stats[i].csLineBuff);
			if (stats[i].csLineBuff2)
				MemFree(stats[i].csLineBuff2);
			stats[i].csLineSize = dispWidth;
			stats[i].csLineBuff = MemAlloc(UInt8, cSize << 1);
			stats[i].csLineBuff2 = MemAlloc(UInt8, cSize << 1);
		}
		stats[i].status = 3;
		stats[i].evt->Set();
		lastHeight = currHeight;
	}
	Bool exited;
	while (true)
	{
		exited = true;
		i = nThread;
		while (i-- > 0)
		{
			if (stats[i].status == 3)
			{
				stats[i].evt->Set();
				exited = false;
				break;
			}
		}
		if (exited)
			break;
		evtMain->Wait();
	}
}

UOSInt Media::CS::CSI420_RGB8::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return (width * height * 3) >> 1;
}
