#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSNV12_RGB8.h"
#include "Sync/ThreadUtil.h"

extern "C"
{
	void CSNV12_RGB8_do_nv12rgb8(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
	void CSNV12_RGB8_do_nv12rgb2(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
}

/*void CSNV12_RGB8_do_nv12rgb8(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr)
{
	OSInt cSub = (width >> 1) - 2;
	OSInt cSize = width << 3;
	OSInt cWidth4 = width >> 2;
	Int32 cofst = 0;//this->cofst;

	Int32 heightLeft;
	Int32 widthLeft;

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
		movzx eax,byte ptr [esi+1]
		movq xmm1,mmword ptr [ebx+eax * 8]
		punpcklqdq xmm0,xmm1
		movdqa mmword ptr [edi],xmm0

		movzx eax,byte ptr [esi+2]
		movq xmm0,mmword ptr [ebx+eax * 8]
		movzx eax,byte ptr [esi+3]
		movq xmm1,mmword ptr [ebx+eax * 8]
		punpcklqdq xmm0,xmm1
		movdqa mmword ptr [edi+16],xmm0

		movzx eax,byte ptr [esi+4]
		movq xmm0,mmword ptr [ebx+eax * 8]
		movzx eax,byte ptr [esi+5]
		movq xmm1,mmword ptr [ebx+eax * 8]
		punpcklqdq xmm0,xmm1
		movdqa mmword ptr [edi+32],xmm0

		movzx eax,byte ptr [esi+6]
		movq xmm0,mmword ptr [ebx+eax * 8]
		movzx eax,byte ptr [esi+7]
		movq xmm1,mmword ptr [ebx+eax * 8]
		punpcklqdq xmm0,xmm1
		movdqa mmword ptr [edi+48],xmm0

		add esi,8
		add edi,64
		dec ecx
		jnz y2rflop2a

		mov yPtr,esi

		mov ecx,cSub
		mov edi,csLineBuff
		mov edx,cSize
		mov esi,uvPtr
		shr ecx,1
		mov widthLeft,ecx


		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [esi+1]
		movq xmm1,mmword ptr [ebx+eax*8 + 4096]
		paddsw xmm0,xmm1

		pxor xmm1,xmm1
		punpcklqdq xmm1,xmm0
		psraw xmm1,1
		por xmm0,xmm1

		movdqa xmm2,mmword ptr [edi]
		movdqa xmm3,mmword ptr [edi+edx]
		paddsw xmm2,xmm0
		psraw xmm0,1
		movdqa mmword ptr [edi],xmm2
		paddsw xmm3,xmm0
		movdqa mmword ptr [edi+edx],xmm3

		add edi,16
		add esi,2

y2rflop3a:
		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [esi+1]
		movq xmm6,mmword ptr [ebx+eax*8 + 4096]
		movzx eax,byte ptr [esi+2]
		movq xmm1,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [esi+3]
		movq xmm7,mmword ptr [ebx+eax*8 + 4096]
		paddsw xmm0,xmm6
		paddsw xmm1,xmm7

		pxor xmm4,xmm4
		pxor xmm5,xmm5
		punpcklqdq xmm4,xmm0
		punpcklqdq xmm5,xmm1
		psraw xmm4,1
		psraw xmm5,1
		por xmm0,xmm4
		por xmm1,xmm5
		paddsw xmm0,xmm5

		movdqa xmm2,mmword ptr [edi+16]
		movdqa xmm3,mmword ptr [edi+edx+16]
		paddsw xmm2,xmm1
		psraw xmm1,1
		movdqa mmword ptr [edi+16],xmm2
		paddsw xmm3,xmm1
		movdqa mmword ptr [edi+edx+16],xmm3

		movdqa xmm2,mmword ptr [edi]
		movdqa xmm3,mmword ptr [edi+edx]
		paddsw xmm2,xmm0
		psraw xmm0,1
		movdqa mmword ptr [edi],xmm2
		paddsw xmm3,xmm0
		movdqa mmword ptr [edi+edx],xmm3

		movdqa xmm2,mmword ptr [edi-16]
		movdqa xmm3,mmword ptr [edi+edx-16]
		paddsw xmm2,xmm4
		psraw xmm4,1
		movdqa mmword ptr [edi-16],xmm2
		paddsw xmm3,xmm4
		movdqa mmword ptr [edi+edx-16],xmm3

		add edi,32
		add esi,4
		dec widthLeft//ecx
		jnz y2rflop3a

		pxor xmm4,xmm4
		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [esi+1]
		movq xmm1,mmword ptr [ebx+eax*8 + 4096]
		paddsw xmm0,xmm1

		punpcklqdq xmm4,xmm0
		punpcklqdq xmm0,xmm0

		movdqa xmm2,mmword ptr [edi]
		movdqa xmm3,mmword ptr [edi+edx]
		paddsw xmm2,xmm0
		psraw xmm0,1
		movdqa mmword ptr [edi],xmm2
		paddsw xmm3,xmm0
		movdqa mmword ptr [edi+edx],xmm3

		psraw xmm4,1
		movdqa xmm2,mmword ptr [edi-16]
		paddsw xmm2,xmm4
		movdqa xmm3,mmword ptr [edi+edx-16]
		psraw xmm4,1
		paddsw xmm2,xmm4
		movdqa mmword ptr [edi-16],xmm2
		movdqa mmword ptr [edi+edx-16],xmm3

		add edi,16
		add esi,2
		mov uvPtr,esi

y2rflop:

		mov ecx,width
//		shl ecx,1
		mov edi,csLineBuff2
		mov ebx,yuv2rgb
		mov esi,yPtr

y2rflop2:
		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8]
		movzx eax,byte ptr [esi+1]
		movq xmm1,mmword ptr [ebx+eax*8]
		punpcklqdq xmm0,xmm1
		movdqa mmword ptr [edi],xmm0
		add esi,2
		add edi,16
		dec ecx
		jnz y2rflop2
		mov yPtr,esi

		mov esi,csLineBuff
		mov ecx,cSub
		mov edi,csLineBuff2
		mov edx,uvPtr
		shr ecx,1
		mov widthLeft,ecx
		mov ecx,cSize

		pxor xmm4,xmm4
		movzx eax,byte ptr [edx]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx+1]
		movq xmm1,mmword ptr [ebx+eax*8 + 4096]
		paddsw xmm0,xmm1

		punpcklqdq xmm4,xmm0
		psraw xmm4,1
		por xmm0,xmm4

		movdqa xmm2,mmword ptr [edi]
		movdqa xmm3,mmword ptr [edi+ecx]
		paddsw xmm2,xmm0
		psraw xmm0,1
		movdqa mmword ptr [edi],xmm2
		paddsw xmm3,xmm0
		movdqa xmm2,mmword ptr [esi+ecx]
		paddsw xmm2,xmm0
		movdqa mmword ptr [edi+ecx],xmm3
		movdqa mmword ptr [esi+ecx],xmm2


		add esi,16
		add edi,16
		add edx,2

		ALIGN 16
y2rflop3:
		pxor xmm4,xmm4
		pxor xmm5,xmm5
		movzx eax,byte ptr [edx]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx+1]
		movq xmm6,mmword ptr [ebx+eax*8 + 4096]
		movzx eax,byte ptr [edx+2]
		movq xmm1,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx+3]
		movq xmm7,mmword ptr [ebx+eax*8 + 4096]
		paddsw xmm0,xmm6
		paddsw xmm1,xmm7

		punpcklqdq xmm4,xmm0
		punpcklqdq xmm5,xmm1
		psraw xmm4,1
		psraw xmm5,1
		por xmm0,xmm4
		por xmm1,xmm5
		paddsw xmm0,xmm5

		movdqa xmm2,mmword ptr [edi+16]
		movdqa xmm3,mmword ptr [edi+ecx+16]
		paddsw xmm2,xmm1
		psraw xmm1,1
		movdqa mmword ptr [edi+16],xmm2
		paddsw xmm3,xmm1
		movdqa xmm2,mmword ptr [esi+ecx+16]
		paddsw xmm2,xmm1
		movdqa mmword ptr [edi+ecx+16],xmm3
		movdqa mmword ptr [esi+ecx+16],xmm2

		movdqa xmm2,mmword ptr [edi]
		movdqa xmm3,mmword ptr [edi+ecx]
		paddsw xmm2,xmm0
		psraw xmm0,1
		movdqa mmword ptr [edi],xmm2
		paddsw xmm3,xmm0
		movdqa xmm2,mmword ptr [esi+ecx]
		paddsw xmm2,xmm0
		movdqa mmword ptr [edi+ecx],xmm3
		movdqa mmword ptr [esi+ecx],xmm2

		movdqa xmm2,mmword ptr [edi-16]
		movdqa xmm3,mmword ptr [edi+ecx-16]
		paddsw xmm2,xmm4
		psraw xmm4,1
		movdqa mmword ptr [edi-16],xmm2
		paddsw xmm3,xmm4
		movdqa xmm2,mmword ptr [esi+ecx-16]
		paddsw xmm2,xmm4
		movdqa mmword ptr [edi+ecx-16],xmm3
		movdqa mmword ptr [esi+ecx-16],xmm2

		add esi, 32

		add edi,32
		add edx,4
		dec widthLeft//ecx
		jnz y2rflop3

		movzx eax,byte ptr [edx]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx+1]
		movq xmm1,mmword ptr [ebx+eax*8 + 4096]
		paddsw xmm0,xmm1

		pxor xmm4,xmm4
		punpcklqdq xmm4,xmm0
		psraw xmm4,1
		por xmm0,xmm4

		movdqa xmm2,mmword ptr [edi]
		movdqa xmm3,mmword ptr [edi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm3,xmm0
		movdqa mmword ptr [edi],xmm2
		movdqa mmword ptr [edi+ecx],xmm3
		movdqa xmm2,mmword ptr [edi-16]
		movdqa xmm3,mmword ptr [edi+ecx-16]
		paddsw xmm2,xmm4
		paddsw xmm3,xmm4
		movdqa xmm2,mmword ptr [edi-16]
		movdqa xmm3,mmword ptr [edi+ecx-16]
		psraw xmm4,1
		psraw xmm0,1
		movdqa xmm2,mmword ptr [esi+ecx]
		movdqa xmm3,mmword ptr [esi+ecx-16]
		paddsw xmm2,xmm0
		paddsw xmm3,xmm4
		movdqa mmword ptr [esi+ecx],xmm2
		movdqa mmword ptr [esi+ecx-16],xmm3

		add esi,16
		add edi,16
		add edx,2
		mov uvPtr,edx

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
//		shl ecx,1
		mov edi,csLineBuff2
		mov ebx,yuv2rgb
		mov esi,yPtr

y2rflop2b:
		movzx eax,byte ptr [esi]
		movq xmm0,mmword ptr [ebx+eax*8]
		movzx eax,byte ptr [esi+1]
		movq xmm1,mmword ptr [ebx+eax*8]
		punpcklqdq xmm0,xmm1
		movdqa mmword ptr [edi],xmm0
		add esi,2
		add edi,16
		dec ecx
		jnz y2rflop2b
		mov yPtr,esi

		mov esi,csLineBuff
		mov ecx,cSub
		mov edi,csLineBuff2
		mov edx,uvPtr
		shr ecx,1
		mov widthLeft,ecx
		mov ecx,cSize

		pxor xmm1,xmm1
		movzx eax,byte ptr [edx]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx+1]
		movq xmm4,mmword ptr [ebx+eax*8 + 4096]
		paddsw xmm0,xmm4

		punpcklqdq xmm1,xmm0
		psraw xmm1,1
		por xmm0,xmm1

		movdqa xmm2,mmword ptr [edi]
		movdqa xmm3,mmword ptr [edi+ecx]
		paddsw xmm2,xmm0
		paddsw xmm3,xmm0
		movdqa mmword ptr [edi],xmm2
		movdqa mmword ptr [edi+ecx],xmm3
		psraw xmm0,1
		movdqa xmm2,mmword ptr [esi+ecx]
		paddsw xmm2,xmm0
		movdqa mmword ptr [esi+ecx],xmm2

		add esi,16
		add edi,16
		add edx,2

y2rflop3b:
		pxor xmm4,xmm4
		pxor xmm5,xmm5
		movzx eax,byte ptr [edx]
		movq xmm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx+1]
		movq xmm6,mmword ptr [ebx+eax*8 + 4096]
		movzx eax,byte ptr [edx+2]
		movq xmm1,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx+3]
		movq xmm7,mmword ptr [ebx+eax*8 + 4096]
		paddsw xmm0,xmm6
		paddsw xmm1,xmm7

		punpcklqdq xmm4,xmm0
		punpcklqdq xmm5,xmm1
		psraw xmm4,1
		psraw xmm5,1
		por xmm0,xmm4
		por xmm1,xmm5
		paddsw xmm0,xmm5

		movdqa xmm2,mmword ptr [edi]
		movdqa xmm3,mmword ptr [edi+16]
		movdqa xmm6,mmword ptr [edi+ecx]
		movdqa xmm7,mmword ptr [edi+ecx+16]
		paddsw xmm2,xmm0
		paddsw xmm3,xmm1
		paddsw xmm6,xmm0
		paddsw xmm7,xmm1
		movdqa mmword ptr [edi],xmm2
		movdqa mmword ptr [edi+16],xmm3
		movdqa mmword ptr [edi+ecx],xmm6
		movdqa mmword ptr [edi+ecx+16],xmm7
		psraw xmm0,1
		psraw xmm1,1
		movdqa xmm2,mmword ptr [esi+ecx]
		movdqa xmm3,mmword ptr [esi+ecx+16]
		paddsw xmm2,xmm0
		paddsw xmm3,xmm1
		movdqa mmword ptr [esi+ecx],xmm2
		movdqa mmword ptr [esi+ecx+16],xmm3
		movdqa xmm2,mmword ptr [edi-16]
		movdqa xmm3,mmword ptr [edi+ecx-16]
		paddsw xmm2,xmm4
		paddsw xmm3,xmm4
		movdqa mmword ptr [edi-16],xmm2
		movdqa mmword ptr [edi+ecx-16],xmm3
		psraw xmm4,1
		movdqa xmm2,mmword ptr [esi+ecx-16]
		paddsw xmm2,xmm4
		movdqa mmword ptr [esi+ecx-16],xmm2

		add esi,32
		add edi,32
		add edx,4
		dec widthLeft//ecx
		jnz y2rflop3b

		pxor xmm1,xmm1
		movzx eax,byte ptr [edx]
		movq xmm0,mmword ptr [ebx+eax*8+2048]
		movzx eax,byte ptr [edx+1]
		movq xmm4,mmword ptr [ebx+eax*8+4096]
		paddsw xmm0,xmm4

		punpcklqdq xmm1,xmm0
		punpcklqdq xmm0,xmm0
		psraw xmm1,1

		movdqa xmm2,mmword ptr [edi]
		movdqa xmm3,mmword ptr [edi+ecx]
		movdqa xmm4,mmword ptr [edi-16]
		movdqa xmm5,mmword ptr [edi+ecx-16]
		paddsw xmm2,xmm0
		paddsw xmm3,xmm0
		paddsw xmm4,xmm1
		paddsw xmm5,xmm1
		movdqa mmword ptr [edi],xmm2
		movdqa mmword ptr [edi+ecx],xmm3
		movdqa mmword ptr [edi-16],xmm4
		movdqa mmword ptr [edi+ecx-16],xmm5
		psraw xmm0,1
		psraw xmm1,1
		movdqa xmm2,mmword ptr [esi+ecx]
		movdqa xmm3,mmword ptr [esi+ecx-16]
		paddsw xmm2,xmm0
		paddsw xmm3,xmm1
		movdqa mmword ptr [esi+ecx],xmm2
		movdqa mmword ptr [esi+ecx-16],xmm3

		add esi,16
		add edi,16
		add edx,2
		mov uvPtr,edx

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
}
*/
#if 0
void CSNV12_RGB8_do_nv12rgb2(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, Int32 width, Int32 height, Int32 dbpl, Int32 isFirst, Int32 isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr)
{
	Int32 cSub = (width >> 1) - 2;
	Int32 cSize = width << 3;
	Int32 cWidth4 = width >> 2;
	Int32 cofst = 0;//this->cofst;

	Int32 heightLeft;
	Int32 widthLeft;
	Int32 buffTemp;


/*	_asm
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
		push ecx
y2rflop2a:
		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi],mm0
		movzx eax,byte ptr [esi+1]
		movq mm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+8],mm0
		movzx eax,byte ptr [esi+2]
		movq mm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+16],mm0
		movzx eax,byte ptr [esi+3]
		movq mm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+24],mm0
		movzx eax,byte ptr [esi+4]

		add esi,4
		add edi,32
		dec ecx
		jnz y2rflop2a
		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi],mm0
		movzx eax,byte ptr [esi+1]
		movq mm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+8],mm0
		add esi,4
		add edi,16
		pop ecx

y2rflop2c:
		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi],mm0
		movzx eax,byte ptr [esi+1]
		movq mm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+8],mm0
		movzx eax,byte ptr [esi+2]
		movq mm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+16],mm0
		movzx eax,byte ptr [esi+3]
		movq mm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+24],mm0
		movzx eax,byte ptr [esi+4]

		add esi,4
		add edi,32
		dec ecx
		jnz y2rflop2c
		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi],mm0
		movzx eax,byte ptr [esi+1]
		movq mm0,mmword ptr [ebx+eax * 8]
		movq mmword ptr [edi+8],mm0
		add esi,4
		add edi,16

		mov yPtr,esi

		mov ecx,cSub
		mov edi,csLineBuff
		mov edx,cSize
		mov esi,uPtr
		mov widthLeft,ecx
		mov ecx,vPtr


		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [ecx]
		movq mm1,mmword ptr [ebx+eax*8 + 4096]

		movq mm2,mmword ptr [edi]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [edi+edx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+edx],mm2
		movq mm2,mmword ptr [edi+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+8],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [edi+edx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+edx+8],mm2

		add edi,16
		inc esi
		inc ecx

y2rflop3a:
		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [ecx]
		movq mm1,mmword ptr [ebx+eax*8 + 4096]

		movq mm2,mmword ptr [edi]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [edi+edx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+edx],mm2
		movq mm2,mmword ptr [edi+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+8],mm2
		movq mm2,mmword ptr [edi-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi-8],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [edi+edx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+edx+8],mm2
		movq mm2,mmword ptr [edi+edx-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+edx-8],mm2

		add edi,16
		inc esi
		inc ecx
		dec widthLeft//ecx
		jnz y2rflop3a

		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [ecx]
		movq mm1,mmword ptr [ebx+eax*8 + 4096]

		movq mm2,mmword ptr [edi]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi],mm2
		movq mm2,mmword ptr [edi+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+8],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [edi+edx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+edx],mm2
		movq mm2,mmword ptr [edi-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi-8],mm2
		movq mm2,mmword ptr [edi+edx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+edx+8],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [edi+edx-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+edx-8],mm2

		add edi,16
		add esi,2
		add ecx,2
		mov uPtr,esi
		mov vPtr,ecx

y2rflop:

		mov ecx,width
		mov edi,csLineBuff2
		mov ebx,yuv2rgb
		mov esi,yPtr

y2rflop2:
		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax*8]
		movq mmword ptr [edi],mm0
		inc esi
		add edi,8
		dec ecx
		jnz y2rflop2
		add esi,2

		mov ecx,width
y2rflop2d:
		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax*8]
		movq mmword ptr [edi],mm0
		inc esi
		add edi,8
		dec ecx
		jnz y2rflop2d
		add esi,2

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
		movq mm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx]
		movq mm1,mmword ptr [ebx+eax*8 + 4096]

		push esi
		mov esi,buffTemp

		movq mm2,mmword ptr [edi]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [edi+ecx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx],mm2
		movq mm2,mmword ptr [esi+ecx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx],mm2
		movq mm2,mmword ptr [edi+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+8],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [edi+ecx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx+8],mm2
		movq mm2,mmword ptr [esi+ecx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx+8],mm2

		add buffTemp, 16
		pop esi

		add edi,16
		inc esi
		inc edx

y2rflop3:
		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx]
		movq mm1,mmword ptr [ebx+eax*8 + 4096]

		push esi
		mov esi,buffTemp

		movq mm2,mmword ptr [edi]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [edi+ecx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx],mm2
		movq mm2,mmword ptr [esi+ecx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx],mm2
		movq mm2,mmword ptr [edi+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+8],mm2
		movq mm2,mmword ptr [edi-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi-8],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [edi+ecx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx+8],mm2
		movq mm2,mmword ptr [esi+ecx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx+8],mm2
		movq mm2,mmword ptr [edi+ecx-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx-8],mm2
		movq mm2,mmword ptr [esi+ecx-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx-8],mm2

		add buffTemp, 16
		pop esi

		add edi,16
		inc esi
		inc edx
		dec widthLeft//ecx
		jnz y2rflop3

		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx]
		movq mm1,mmword ptr [ebx+eax*8 + 4096]

		push esi
		mov esi,buffTemp

		movq mm2,mmword ptr [edi]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi],mm2
		movq mm2,mmword ptr [edi+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+8],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [edi+ecx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx],mm2
		movq mm2,mmword ptr [esi+ecx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx],mm2
		movq mm2,mmword ptr [edi+ecx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx+8],mm2
		movq mm2,mmword ptr [esi+ecx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx+8],mm2
		movq mm2,mmword ptr [edi-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi-8],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [edi+ecx-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx-8],mm2
		movq mm2,mmword ptr [esi+ecx-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx-8],mm2

		add buffTemp, 16
		pop esi

		add edi,16
		add esi,2
		add edx,2
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
		mov edi,csLineBuff2
		mov ebx,yuv2rgb
		mov esi,yPtr

y2rflop2b:
		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax*8]
		movq mmword ptr [edi],mm0
		inc esi
		add edi,8
		dec ecx
		jnz y2rflop2b
		inc esi

		mov ecx,width
y2rflop2e:
		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax*8]
		movq mmword ptr [edi],mm0
		inc esi
		add edi,8
		dec ecx
		jnz y2rflop2e
		inc esi
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
		movq mm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx]
		movq mm1,mmword ptr [ebx+eax*8 + 4096]

		push esi
		mov esi,buffTemp

		movq mm2,mmword ptr [edi]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi],mm2
		movq mm2,mmword ptr [edi+ecx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [esi+ecx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx],mm2
		movq mm2,mmword ptr [edi+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+8],mm2
		movq mm2,mmword ptr [edi+ecx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx+8],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [esi+ecx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx+8],mm2

		add buffTemp, 16
		pop esi

		add edi,16
		inc esi
		inc edx

y2rflop3b:
		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx]
		movq mm1,mmword ptr [ebx+eax*8 + 4096]

		push esi
		mov esi,buffTemp

		movq mm2,mmword ptr [edi]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi],mm2
		movq mm2,mmword ptr [edi+ecx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [esi+ecx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx],mm2
		movq mm2,mmword ptr [edi+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+8],mm2
		movq mm2,mmword ptr [edi-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi-8],mm2
		movq mm2,mmword ptr [edi+ecx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx+8],mm2
		movq mm2,mmword ptr [edi+ecx-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx-8],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [esi+ecx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx+8],mm2
		movq mm2,mmword ptr [esi+ecx-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx-8],mm2

		add buffTemp, 16
		pop esi

		add edi,16
		inc esi
		inc edx
		dec widthLeft//ecx
		jnz y2rflop3b

		movzx eax,byte ptr [esi]
		movq mm0,mmword ptr [ebx+eax*8 + 2048]
		movzx eax,byte ptr [edx]
		movq mm1,mmword ptr [ebx+eax*8 + 4096]

		push esi
		mov esi,buffTemp

		movq mm2,mmword ptr [edi]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi],mm2
		movq mm2,mmword ptr [edi+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+8],mm2
		movq mm2,mmword ptr [edi+ecx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx],mm2
		movq mm2,mmword ptr [edi+ecx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx+8],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [esi+ecx]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx],mm2
		movq mm2,mmword ptr [esi+ecx+8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx+8],mm2
		movq mm2,mmword ptr [edi-8]
		movq mmword ptr [edi-8],mm2
		psraw mm0,1
		psraw mm1,1
		movq mm2,mmword ptr [edi+ecx-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [edi+ecx-8],mm2
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mm2,mmword ptr [esi+ecx-8]
		paddsw mm2,mm0
		paddsw mm2,mm1
		movq mmword ptr [esi+ecx-8],mm2

		add buffTemp, 16
		pop esi

		add edi,16
		add esi,2
		add edx,2
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
		emms
	};*/
}
#endif

UInt32 Media::CS::CSNV12_RGB8::WorkerThread(void *obj)
{
	CSNV12_RGB8 *converter = (CSNV12_RGB8*)obj;
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
			if (ts->width & 3)
			{
				CSNV12_RGB8_do_nv12rgb2(ts->yPtr, ts->uvPtr, ts->dest, ts->width, ts->height, ts->dbpl, ts->isFirst, ts->isLast, ts->csLineBuff, ts->csLineBuff2, converter->yuv2rgb, converter->rgbGammaCorr);
			}
			else
			{
				CSNV12_RGB8_do_nv12rgb8(ts->yPtr, ts->uvPtr, ts->dest, ts->width, ts->height, ts->dbpl, ts->isFirst, ts->isLast, ts->csLineBuff, ts->csLineBuff2, converter->yuv2rgb, converter->rgbGammaCorr);
			}
			ts->status = 4;
			converter->evtMain->Set();
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain->Set();
	return 0;
}

Media::CS::CSNV12_RGB8::CSNV12_RGB8(NotNullPtr<const Media::ColorProfile> srcColor, NotNullPtr<const Media::ColorProfile> destColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess) : Media::CS::CSYUV_RGB8(srcColor, destColor, yuvType, colorSess)
{
	UOSInt i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();
	if (this->nThread > 4)
	{
		this->nThread = 4;
	}

	NEW_CLASS(evtMain, Sync::Event());
	stats = MemAlloc(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASS(stats[i].evt, Sync::Event());
		stats[i].status = 0;
		stats[i].csLineSize = 0;
		stats[i].csLineBuff = 0;
		stats[i].csNALineBuff = 0;
		stats[i].csLineBuff2 = 0;
		stats[i].csNALineBuff2 = 0;

		currId = i;
		Sync::ThreadUtil::Create(WorkerThread, this, 65536);
		while (stats[i].status == 0)
		{
			evtMain->Wait();
		}
	}
}

Media::CS::CSNV12_RGB8::~CSNV12_RGB8()
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
		if (stats[i].csNALineBuff)
		{
			MemFree(stats[i].csNALineBuff);
			stats[i].csNALineBuff = 0;
			stats[i].csLineBuff = 0;
		}
		if (stats[i].csNALineBuff2)
		{
			MemFree(stats[i].csNALineBuff2);
			stats[i].csNALineBuff2 = 0;
			stats[i].csLineBuff2 = 0;
		}
		DEL_CLASS(stats[i].evt);
	}
	DEL_CLASS(evtMain);
	MemFree(stats);
}

void Media::CS::CSNV12_RGB8::ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	this->UpdateTable();
	UOSInt isLast = 1;
	UOSInt isFirst = 0;
	UOSInt i = this->nThread;
	UOSInt lastHeight = dispHeight;
	UOSInt currHeight;
	UOSInt cSize = dispWidth << 4;
	if (srcStoreWidth & 3)
	{
		srcStoreWidth = srcStoreWidth + 4 - (srcStoreWidth & 3);
	}
	if (srcStoreHeight & 1)
	{
		srcStoreHeight = srcStoreHeight + 1;
	}
	

	while (i-- > 0)
	{
		if (i == 0)
			isFirst = 1;
		currHeight = MulDivOS(i, dispHeight, nThread) & ~1;

		stats[i].yPtr = srcPtr[0] + srcStoreWidth * currHeight;
		stats[i].uvPtr = srcPtr[0] + srcStoreWidth * srcStoreHeight + ((srcStoreWidth * currHeight) >> 1);
		stats[i].dest = ((UInt8*)destPtr) + destRGBBpl * currHeight;
		stats[i].isFirst = isFirst;
		stats[i].isLast = isLast;
		isLast = 0;
		stats[i].width = dispWidth;
		stats[i].height = lastHeight - currHeight;
		stats[i].dbpl = destRGBBpl;

		if (stats[i].csLineSize < dispWidth)
		{
			if (stats[i].csNALineBuff)
				MemFree(stats[i].csNALineBuff);
			if (stats[i].csNALineBuff2)
				MemFree(stats[i].csNALineBuff2);
			stats[i].csLineSize = dispWidth;
			stats[i].csNALineBuff = MemAlloc(UInt8, (cSize << 1) + 15);
			stats[i].csNALineBuff2 = MemAlloc(UInt8, (cSize << 1) + 15);
			if (((OSInt)stats[i].csNALineBuff) & 15)
			{
				stats[i].csLineBuff = stats[i].csNALineBuff + 16 - (((OSInt)stats[i].csNALineBuff) & 15);
			}
			else
			{
				stats[i].csLineBuff = stats[i].csNALineBuff;
			}
			if (((OSInt)stats[i].csNALineBuff2) & 15)
			{
				stats[i].csLineBuff2 = stats[i].csNALineBuff2 + 16 - (((OSInt)stats[i].csNALineBuff2) & 15);
			}
			else
			{
				stats[i].csLineBuff2 = stats[i].csNALineBuff2;
			}
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

UOSInt Media::CS::CSNV12_RGB8::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return (width * height * 3) >> 1;
}
