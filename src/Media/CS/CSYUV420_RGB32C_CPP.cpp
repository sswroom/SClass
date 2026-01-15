#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"

extern "C" void CSYUV420_RGB32C_VerticalFilterLRGB(UInt8 *inYPt, UInt8 *inUPt, UInt8 *inVPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weighti64, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS ystep, IntOS dstep, Int64 *yuv2rgb14i64, Int64 *rgbGammaCorri64)
{
	if (tap != 4)
		return;

	UInt8 *weight = (UInt8*)weighti64;
	UInt8 *rgbGammaCorr = (UInt8*)rgbGammaCorri64;
	UInt8 *yuv2rgb = (UInt8*)yuv2rgb14i64;

	IntOS yAdd = ystep - width;
	IntOS sWidth = width >> 3;
	IntOS cSub = (width >> 1) - 2;

	Int32x4 tmpV = PInt32x4SetA(32768);
	IntOS i;
	UInt8 *tmpPtr;
	UInt8 *tmpPtr2;
	UInt8 *tmpPtr3;
	Int32x4 valTmp[2];
	Int16x8 mulVal1;
	Int16x8 mulVal2;
	Int16x8 i16Val1;
	Int16x8 i16Val2;
	Int16x4 qVal;

	if (width & 7)
	{
		IntOS widthLeft = (width & 7) >> 2;
		while (height-- > 0)
		{
			tmpPtr = csLineBuff2;
			tmpPtr2 = inUPt;
			tmpPtr3 = inVPt;
			mulVal1 = PLoadInt16x8A(&weight[16]);
			mulVal2 = PLoadInt16x8A(&weight[32]);

			i = sWidth;
			while (i-- > 0)
			{
				valTmp[0] = tmpV;
				valTmp[1] = tmpV;
				i16Val1 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[0]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[0]]))));
				i16Val2 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[4]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[4]]))));
				valTmp[0] = PADDD4(valTmp[0], PMADDWD(PSARW8(PUNPCKLWW8(i16Val1, i16Val2), 1), mulVal1));
				valTmp[1] = PADDD4(valTmp[1], PMADDWD(PSARW8(PUNPCKHWW8(i16Val1, i16Val2), 1), mulVal1));
				i16Val1 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[8]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[0]]))));
				i16Val2 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[12]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[4]]))));
				valTmp[0] = PADDD4(valTmp[0], PMADDWD(PSARW8(PUNPCKLWW8(i16Val1, i16Val2), 1), mulVal2));
				valTmp[1] = PADDD4(valTmp[1], PMADDWD(PSARW8(PUNPCKHWW8(i16Val1, i16Val2), 1), mulVal2));
				PStoreInt16x8(tmpPtr, SI32ToI16x8(PSARD4(valTmp[0], 16), PSARD4(valTmp[1], 16)));

				tmpPtr += 16;
				tmpPtr2 += 4;
				tmpPtr3 += 4;
			}

			if (widthLeft)
			{
				valTmp[0] = tmpV;
				i16Val1 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[0]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[0]]))));
				i16Val2 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[4]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[4]]))));
				valTmp[0] = PADDD4(valTmp[0], PMADDWD(PSARW8(PUNPCKLWW8(i16Val1, i16Val2), 1), mulVal1));
				i16Val1 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[8]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[0]]))));
				i16Val2 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[12]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[4]]))));
				valTmp[0] = PADDD4(valTmp[0], PMADDWD(PSARW8(PUNPCKLWW8(i16Val1, i16Val2), 1), mulVal2));
				PStoreInt16x4(tmpPtr, SI32ToI16x4(PSARD4(valTmp[0], 16)));

				tmpPtr += 8;
				tmpPtr2 += 2;
				tmpPtr3 += 2;
			}

			if (width & 3)
			{
				valTmp[0] = tmpV;
				i16Val1 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[0]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[0]]))));
				i16Val2 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[4]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[4]]))));
				valTmp[0] = PADDD4(valTmp[0], PMADDWD(PSARW8(PUNPCKLWW8(i16Val1, i16Val2), 1), mulVal1));
				i16Val1 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[8]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[0]]))));
				i16Val2 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[12]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[4]]))));
				valTmp[0] = PADDD4(valTmp[0], PMADDWD(PSARW8(PUNPCKLWW8(i16Val1, i16Val2), 1), mulVal2));
				PStoreInt16x4(tmpPtr, SI32ToI16x4(PSARD4(valTmp[0], 16)));

				tmpPtr += 4;
				tmpPtr2 += 1;
				tmpPtr3 += 1;
			}

			tmpPtr = inYPt;
			tmpPtr2 = csLineBuff;
			tmpPtr3 = csLineBuff2;
			i16Val1 = PMLoadInt16x4(&yuv2rgb[tmpPtr[1] * 8], &yuv2rgb[tmpPtr[0] * 8]);

			i16Val2 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
			i16Val1 = PSADDW8(i16Val1, i16Val2);
			i16Val2 = PSARW8(PMergeLW4(PInt16x8Clear(), i16Val2), 1);
			i16Val1 = PSADDW8(i16Val1, i16Val2);

			tmpPtr += 2;
			tmpPtr3 += 4;
			i = cSub;
			while (i-- > 0)
			{
				mulVal1 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
				mulVal2 = PSARW8(PMergeLW4(PInt16x8Clear(), mulVal1), 1);
				PStoreInt16x8A(tmpPtr2, PSADDW8(i16Val1, mulVal2));

				i16Val1 = PMLoadInt16x4(&yuv2rgb[tmpPtr[1] * 8], &yuv2rgb[tmpPtr[0] * 8]);
				i16Val1 = PSADDW8(i16Val1, mulVal1);
				i16Val1 = PSADDW8(i16Val1, mulVal2);

				tmpPtr2 += 16;
				tmpPtr += 2;
				tmpPtr3 += 4;
			}

			mulVal1 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
			mulVal2 = PSARW8(PMergeLW4(PInt16x8Clear(), mulVal1), 1);
			PStoreInt16x8A(tmpPtr2, PSADDW8(i16Val1, mulVal2));

			i16Val1 = PMLoadInt16x4(&yuv2rgb[tmpPtr[1] * 8], &yuv2rgb[tmpPtr[0] * 8]);
			PStoreInt16x8A(&tmpPtr2[16], PSADDW8(i16Val1, mulVal1));
			tmpPtr += 2;
			tmpPtr3 += 4;
			tmpPtr += yAdd;
			inYPt = tmpPtr;

			tmpPtr = outPt;
			tmpPtr2 = csLineBuff;
			i = width;
			while (i-- > 0)
			{
				qVal = PLoadInt16x4(&rgbGammaCorr[8 * *(UInt16*)&tmpPtr2[4]]);
				qVal = PSADDW4(qVal, PLoadInt16x4(&rgbGammaCorr[524288 + 8 * *(UInt16*)&tmpPtr2[2]]));
				qVal = PSADDW4(qVal, PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * *(UInt16*)&tmpPtr2[0]]));
				*(Int32*)tmpPtr = (*(Int32*)&rgbGammaCorr[1572864 + 4 * (UInt16)PEXTW4(qVal, 0)]) | (*(Int32*)&rgbGammaCorr[1835008 + 4 * (UInt16)PEXTW4(qVal, 1)]) | (*(Int32*)&rgbGammaCorr[2097152 + 4 * (UInt16)PEXTW4(qVal, 2)]);

				tmpPtr2 += 8;
				tmpPtr += 4;
			}

			weight += 48;
			outPt += dstep;
		}
	}
	else
	{
//		IntOS widthLeft = (width & 7) >> 2;
		while (height-- > 0)
		{
			tmpPtr = csLineBuff2;
			tmpPtr2 = inUPt;
			tmpPtr3 = inVPt;
			mulVal1 = PLoadInt16x8A(&weight[16]);
			mulVal2 = PLoadInt16x8A(&weight[32]);

			i = sWidth;
			while (i-- > 0)
			{
				valTmp[0] = tmpV;
				valTmp[1] = tmpV;
				i16Val1 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[0]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[0]]))));
				i16Val2 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[4]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[4]]))));
				valTmp[0] = PADDD4(valTmp[0], PMADDWD(PCONVU16x8_I(PSHRW8(PCONVI16x8_U(PUNPCKLWW8(i16Val1, i16Val2)), 1)), mulVal1));
				valTmp[1] = PADDD4(valTmp[1], PMADDWD(PCONVU16x8_I(PSHRW8(PCONVI16x8_U(PUNPCKHWW8(i16Val1, i16Val2)), 1)), mulVal1));
				i16Val1 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[8]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[8]]))));
				i16Val2 = PCONVU16x8_I(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr2[*(Int32*)&weight[12]]), PLoadUInt8x4(&tmpPtr3[*(Int32*)&weight[12]]))));
				valTmp[0] = PADDD4(valTmp[0], PMADDWD(PCONVU16x8_I(PSHRW8(PCONVI16x8_U(PUNPCKLWW8(i16Val1, i16Val2)), 1)), mulVal2));
				valTmp[1] = PADDD4(valTmp[1], PMADDWD(PCONVU16x8_I(PSHRW8(PCONVI16x8_U(PUNPCKHWW8(i16Val1, i16Val2)), 1)), mulVal2));
				PStoreInt16x8(tmpPtr, SI32ToI16x8(PSARD4(valTmp[0], 16), PSARD4(valTmp[1], 16)));

				tmpPtr += 16;
				tmpPtr2 += 4;
				tmpPtr3 += 4;
			}

			tmpPtr = inYPt;
			tmpPtr2 = csLineBuff;
			tmpPtr3 = csLineBuff2;
			i16Val1 = PMLoadInt16x4(&yuv2rgb[tmpPtr[1] * 8], &yuv2rgb[tmpPtr[0] * 8]);

			i16Val2 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
			i16Val1 = PSADDW8(i16Val1, i16Val2);
			i16Val2 = PSARW8(PMergeLW4(PInt16x8Clear(), i16Val2), 1);
			i16Val1 = PSADDW8(i16Val1, i16Val2);

			mulVal2 = i16Val2;

			tmpPtr += 2;
			tmpPtr3 += 4;
			i = cSub;
			while (i-- > 0)
			{
				mulVal1 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
				mulVal2 = PSARW8(PMergeLW4(mulVal2, mulVal1), 1);
				PStoreInt16x8A(tmpPtr2, PSADDW8(i16Val1, mulVal2));

				i16Val1 = PMLoadInt16x4(&yuv2rgb[tmpPtr[1] * 8], &yuv2rgb[tmpPtr[0] * 8]);
				i16Val1 = PSADDW8(i16Val1, mulVal1);
				i16Val1 = PSADDW8(i16Val1, mulVal2);

				tmpPtr2 += 16;
				tmpPtr += 2;
				tmpPtr3 += 4;
			}

			mulVal1 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
			mulVal2 = PMergeLW4(mulVal2, mulVal1);
			mulVal1 = PORW8(mulVal1, mulVal2);
			mulVal2 = PSARW8(mulVal2, 1);
			PStoreInt16x8A(tmpPtr2, PSADDW8(i16Val1, mulVal2));

			i16Val1 = PMLoadInt16x4(&yuv2rgb[tmpPtr[1] * 8], &yuv2rgb[tmpPtr[0] * 8]);
			PStoreInt16x8A(&tmpPtr2[16], PSADDW8(i16Val1, mulVal1));
			tmpPtr += 2;
			tmpPtr3 += 4;
			tmpPtr += yAdd;
			inYPt = tmpPtr;

			tmpPtr = outPt;
			tmpPtr2 = csLineBuff;
			i = width;
			while (i-- > 0)
			{
				qVal = PLoadInt16x4(&rgbGammaCorr[8 * *(UInt16*)&tmpPtr2[4]]);
				qVal = PSADDW4(qVal, PLoadInt16x4(&rgbGammaCorr[524288 + 8 * *(UInt16*)&tmpPtr2[2]]));
				qVal = PSADDW4(qVal, PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * *(UInt16*)&tmpPtr2[0]]));
				*(Int32*)tmpPtr = (*(Int32*)&rgbGammaCorr[1572864 + 4 * (UInt16)PEXTW4(qVal, 0)]) | (*(Int32*)&rgbGammaCorr[1835008 + 4 * PEXTW4(qVal, 1)]) | (*(Int32*)&rgbGammaCorr[2097152 + 4 * PEXTW4(qVal, 2)]);

				tmpPtr2 += 8;
				tmpPtr += 4;
			}

			weight += 48;
			outPt += dstep;
		}
	}
}

extern "C" void CSYUV420_RGB32C_do_yv12rgb8(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS yBpl, IntOS uvBpl, Int64 *yuv2rgbi64, Int64 *rgbGammaCorri64)
{
/*	UInt8 *rgbGammaCorr = (UInt8*)rgbGammaCorri64;
	UInt8 *yuv2rgb = (UInt8*)yuv2rgbi64;

	IntOS yAdd = yBpl - width;
	IntOS sWidth = width >> 3;
	IntOS cSub = (width >> 1) - 2;
	IntOS cSize = width << 3;
	IntOS uvAdd = uvBpl - (width >> 1);
	Int32 tmpV = 32768;
	IntOS i;
	UInt8 *tmpPtr;
	UInt8 *tmpPtr2;
	UInt8 *tmpPtr3;
	Int32 valTmp[8];

	IntOS widthLeft = (width & 7) >> 2;

	tmpPtr = csLineBuff;
	i = width;
	while (i-- > 0)
	{
		*(Int64*)&tmpPtr[0] = *(Int64*)&yuv2rgb[*yPtr * 8];
		yPtr++;
		tmpPtr += 8;
	}
	yPtr += yAdd;*/

/*	mov rcx,qword [rsp+72] ;width
	shr rcx,3
	ALIGN 16
y2r8flop2a2:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0

	movzx rax,byte [rsi+2]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [rsi+3]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi+16],xmm0

	movzx rax,byte [rsi+4]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [rsi+5]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi+32],xmm0

	movzx rax,byte [rsi+6]
	movq xmm0,[rbx+rax * 8]
	movzx rax,byte [rsi+7]
	movq xmm1,[rbx+rax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi+48],xmm0

	lea rsi,[rsi+8]
	lea rdi,[rdi+64]
	dec rcx
	jnz y2r8flop2a2
	add rsi,qword [rsp+128] ;yAdd
	mov r10,rsi ;yPtr

	mov rcx,qword [rsp-16] ;cSub
	mov rdx,qword [rsp-8] ;cSize
	mov rdi,qword [rsp+112] ;csLineBuff
	mov rsi,r11 ;uPtr
	shr rcx,1
	mov qword [rsp-24],rcx ;widthLeft
	mov rcx,r8 ;vPtr

	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rcx]
	movq xmm1,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm1

	pxor xmm1,xmm1
	punpcklqdq xmm1,xmm0
	psraw xmm1,1
	por xmm0,xmm1

	movdqa xmm6,[rdi]
	movdqa xmm7,[rdi+rdx]
	paddsw xmm6,xmm0
	psraw xmm0,1
	paddsw xmm7,xmm0

	lea rdi,[rdi+16]
	inc rsi
	inc rcx

	align 16
y2r8flop3a:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8 + 2048]
	movzx rax,byte [rcx]
	movq xmm4,[rbx+rax*8 + 4096]
	movzx rax,byte [rcx+1]
	movq xmm5,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm4
	paddsw xmm1,xmm5

	pxor xmm4,xmm4
	pxor xmm5,xmm5
	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	paddsw xmm6,xmm4
	psraw xmm4,1
	movdqa [rdi-16],xmm6
	paddsw xmm7,xmm4
	movdqa [rdi+rdx-16],xmm7

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rdx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [rdi],xmm2
	paddsw xmm3,xmm0
	movdqa [rdi+rdx],xmm3

	movdqa xmm6,[rdi+16]
	movdqa xmm7,[rdi+rdx+16]
	paddsw xmm6,xmm1
	psraw xmm1,1
	paddsw xmm7,xmm1

	lea rdi,[rdi+32]
	lea rsi,[rsi+2]
	lea rcx,[rcx+2]
	dec qword [rsp-24] ;widthLeft
	jnz y2r8flop3a

	pxor xmm4,xmm4
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rcx]
	movq xmm1,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm1

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm0,xmm0

	psraw xmm4,1
	paddsw xmm6,xmm4
	psraw xmm4,1
	paddsw xmm7,xmm4
	movdqa [rdi-16],xmm6
	movdqa [rdi+rdx-16],xmm7

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rdx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [rdi],xmm2
	paddsw xmm3,xmm0
	movdqa [rdi+rdx],xmm3

	lea rdi,[rdi+16]
	inc rsi
	inc rcx
	mov rbp,qword [rsp+136] ;uvAdd
	lea r11,[rsi+rbp] ;uPtr
	lea r8,[rcx+rbp] ;vPtr

	align 16
y2r8flop:

	mov rdi,qword [rsp+120] ;csLineBuff2
	mov rbx,qword [rsp+144] ;yuv2rgb
	mov rsi,r10 ;yPtr

	mov rcx,qword [rsp+72] ;width
	shr rcx,1
	align 16
y2r8flop2:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea rsi,[rsi+2]
	lea rdi,[rdi+16]
	dec rcx
	jnz y2r8flop2

	add rsi,qword [rsp+128] ;yAdd

	mov rcx,qword [rsp+72] ;width
	shr rcx,1
	align 16
y2r8flop2_2:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea rsi,[rsi+2]
	lea rdi,[rdi+16]
	dec rcx
	jnz y2r8flop2_2

	add rsi,qword [rsp+128] ;yAdd
	mov r10,rsi ;yPtr

	mov rcx,qword [rsp-16] ;cSub
	mov rbp,qword [rsp+112] ;csLineBuff
	mov rdi,qword [rsp+120] ;csLineBuff2
	mov rdx,r8 ;vPtr
	mov rsi,r11 ;uPtr
	shr rcx,1
	mov qword [rsp-24],rcx ;widthLeft
	mov rcx,qword [rsp-8] ;cSize

	pxor xmm4,xmm4
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm1,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm1

	punpcklqdq xmm4,xmm0
	psraw xmm4,1
	por xmm0,xmm4

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	paddsw xmm3,xmm0
	movdqa xmm6,[rbp+rcx]
	paddsw xmm6,xmm0


	add rbp,16
	add rdi,16
	inc rsi
	inc rdx

	ALIGN 16
y2r8flop3:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm4,[rbx+rax*8 + 4096]
	movzx rax,byte [rdx+1]
	movq xmm5,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm4
	paddsw xmm1,xmm5

	pxor xmm4,xmm4
	pxor xmm5,xmm5

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	paddsw xmm2,xmm4
	psraw xmm4,1
	movdqa [rdi-16],xmm2
	paddsw xmm3,xmm4
	paddsw xmm6,xmm4
	movdqa [rdi+rcx-16],xmm3
	movdqa [rbp+rcx-16],xmm6

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [rdi],xmm2
	paddsw xmm3,xmm0
	movdqa xmm6,[rbp+rcx]
	paddsw xmm6,xmm0
	movdqa [rdi+rcx],xmm3
	movdqa [rbp+rcx],xmm6

	movdqa xmm2,[rdi+16]
	movdqa xmm3,[rdi+rcx+16]
	paddsw xmm2,xmm1
	psraw xmm1,1
	paddsw xmm3,xmm1
	movdqa xmm6,[rbp+rcx+16]
	paddsw xmm6,xmm1

	lea rbp,[rbp+32]
	lea rdi,[rdi+32]
	lea rsi,[rsi+2]
	lea rdx,[rdx+2]
	dec qword [rsp-24] ;widthLeft
	jnz y2r8flop3

	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm1,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm1

	pxor xmm4,xmm4
	punpcklqdq xmm4,xmm0
	por xmm0,xmm4
	psraw xmm4,1

	paddsw xmm2,xmm4
	movdqa [rdi-16],xmm2
	psraw xmm4,1
	paddsw xmm3,xmm4
	paddsw xmm6,xmm4
	movdqa [rdi+rcx-16],xmm3
	movdqa [rbp+rcx-16],xmm6

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	movdqa xmm6,[rbp+rcx]
	paddsw xmm2,xmm0
	movdqa [rdi],xmm2
	psraw xmm0,1
	paddsw xmm3,xmm0
	paddsw xmm6,xmm0
	movdqa [rdi+rcx],xmm3
	movdqa [rbp+rcx],xmm6

	lea rbp,[rbp+16]
	lea rdi,[rdi+16]
	inc rsi
	inc rdx
	add rsi,qword [rsp+136] ;uvAdd
	add rdx,qword [rsp+136] ;uvAdd
	mov r11,rsi ;uPtr
	mov r8,rdx ;vPtr

	mov rcx,qword [rsp+72] ;width
	mov rsi,qword [rsp+112] ;csLineBuff
	mov rdi,r9 ;dest
	mov rbx,qword [rsp+152] ;rgbGammaCorr
	ALIGN 16
y2r8flop5:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	pextrw rax,xmm7,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm7,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm7,2
	or edx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz y2r8flop5

	add r9,qword [rsp+88] ;dbpl

	mov rcx,qword [rsp+72] ;width
	mov rdi,r9 ;dest
	ALIGN 16
y2r8flop6:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	pextrw rax,xmm7,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm7,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm7,2
	or edx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz y2r8flop6

	add r9,qword [rsp+88] ;dbpl
	mov rax,qword [rsp+112] ;csLineBuff
	xchg rax,qword [rsp+120] ;csLineBuff2
	mov qword [rsp+112],rax ;;csLineBuff

	dec qword [rsp+80] ;heightLeft
	jnz y2r8flop

	test qword [rsp+104],1 ;isLast
	jz yv2r8flopexit

	mov rdi,qword [rsp+120] ;csLineBuff2
	mov rbx,qword [rsp+144] ;yuv2rgb
	mov rsi,r10 ;yPtr

	mov rcx,qword [rsp+72] ;width
	shr rcx,1
	ALIGN 16
y2r8flop2b:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea rsi,[rsi+2]
	lea rdi,[rdi+16]
	dec rcx
	jnz y2r8flop2b
	add rsi,qword [rsp+128] ;yAdd

	mov rcx,qword [rsp+72] ;width
	shr rcx,1
	ALIGN 16
y2r8flop2b2:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8]
	punpcklqdq xmm0,xmm1
	movdqa [rdi],xmm0
	lea rsi,[rsi+2]
	lea rdi,[rdi+16]
	dec rcx
	jnz y2r8flop2b2
	add r10,qword [rsp+128] ;yAdd

	mov rbp,qword [rsp+112] ;csLineBuff
	mov rcx,qword [rsp-16] ;cSub
	mov rdi,qword [rsp+120] ;csLineBuff2
	mov rdx,r8 ;vPtr
	mov rsi,r11 ;uPtr
	shr rcx,1
	mov qword [rsp-24],rcx ;widthLeft
	mov rcx,qword [rsp-8] ;cSize

	pxor xmm1,xmm1
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm4,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm4

	punpcklqdq xmm1,xmm0
	psraw xmm1,1
	por xmm0,xmm1

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	movdqa xmm6,[rbp+rcx]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	psraw xmm0,1
	paddsw xmm6,xmm0

	lea rbp,[rbp+16]
	lea rdi,[rdi+16]
	inc rsi
	inc rdx

	align 16
y2r8flop3b:
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8 + 2048]
	movzx rax,byte [rsi+1]
	movq xmm1,[rbx+rax*8 + 2048]
	movzx rax,byte [rdx]
	movq xmm4,[rbx+rax*8 + 4096]
	movzx rax,byte [rdx+1]
	movq xmm5,[rbx+rax*8 + 4096]
	paddsw xmm0,xmm4
	paddsw xmm1,xmm5

	pxor xmm4,xmm4
	pxor xmm5,xmm5

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	paddsw xmm2,xmm4
	paddsw xmm3,xmm4
	movdqa [rdi-16],xmm2
	psraw xmm4,1
	movdqa [rdi+rcx-16],xmm3
	paddsw xmm6,xmm4
	movdqa [rbp+rcx-16],xmm6

	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	movdqa xmm6,[rbp+rcx]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	movdqa [rdi],xmm2
	psraw xmm0,1
	movdqa [rdi+rcx],xmm3
	paddsw xmm6,xmm0
	movdqa [rbp+rcx],xmm6
	movdqa xmm2,[rdi+16]
	movdqa xmm3,[rdi+rcx+16]
	movdqa xmm6,[rbp+rcx+16]
	paddsw xmm2,xmm1
	paddsw xmm3,xmm1
	psraw xmm1,1
	paddsw xmm6,xmm1

	lea rbp,[rbp+32]
	lea rdi,[rdi+32]
	lea rsi,[rsi+2]
	lea rdx,[rdx+2]
	dec qword [rsp-24] ;widthLeft
	jnz y2r8flop3b

	pxor xmm1,xmm1
	movzx rax,byte [rsi]
	movq xmm0,[rbx+rax*8+2048]
	movzx rax,byte [rdx]
	movq xmm4,[rbx+rax*8+4096]
	paddsw xmm0,xmm4

	punpcklqdq xmm1,xmm0
	punpcklqdq xmm0,xmm0
	psraw xmm1,1

	paddsw xmm2,xmm1
	paddsw xmm3,xmm1
	movdqa [rdi-16],xmm2
	psraw xmm1,1
	movdqa [rdi+rcx-16],xmm3
	paddsw xmm6,xmm1
	movdqa [rbp+rcx-16],xmm6
	movdqa xmm2,[rdi]
	movdqa xmm3,[rdi+rcx]
	movdqa xmm6,[rbp+rcx]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	movdqa [rdi],xmm2
	psraw xmm0,1
	movdqa [rdi+rcx],xmm3
	paddsw xmm6,xmm0
	movdqa [rbp+rcx],xmm6

	lea rbp,[rbp+16]
	lea rdi,[rbp+16]
	inc rsi
	inc rdx
	add rsi,qword [rsp+136] ;uvAdd
	add rdx,qword [rsp+136] ;uvAdd
	mov r11,rsi ;uPtr
	mov r8,rdx ;vPtr

	mov rcx,qword [rsp+72] ;width
	mov rsi,qword [rsp+112] ;csLineBuff
	mov rbx,qword [rsp+152] ;rgbGammaCorr
	mov rdi,r9 ;dest
	ALIGN 16
y2r8flop5b:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	pextrw rax,xmm7,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm7,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm7,2
	or edx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz y2r8flop5b

	add r9,qword [rsp+88] ;dbpl

	mov rcx,qword [rsp+72] ;width
	mov rdi,r9 ;dest
	ALIGN 16
y2r8flop6b:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	pextrw rax,xmm7,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm7,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm7,2
	or edx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz y2r8flop6b

	add r9,qword [rsp+88] ;dbpl

	mov rax,qword [rsp+112] ;csLineBuff
	xchg rax,qword [rsp+120] ;csLineBuff2
	mov qword [rsp+112],rax ;csLineBuff

	mov rdi,r9 ;dest
	mov rcx,qword [rsp+72] ;width
	mov rsi,qword [rsp+112] ;csLineBuff
	mov rbx,qword [rsp+152] ;rgbGammaCorr
	ALIGN 16
y2r8flop5c:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	pextrw rax,xmm7,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm7,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm7,2
	or edx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz y2r8flop5c

	add r9,qword [rsp+88] ;dbpl

	mov rcx,qword [rsp+72] ;width
	mov rdi,r9 ;dest
	ALIGN 16
y2r8flop6c:
	movzx rax,word [rsi+4]
	movq xmm7,[rbx+rax*8]
	movzx rax,word [rsi+2]
	movq xmm6,[rbx+rax*8+524288]
	paddsw xmm7,xmm6
	movzx rax,word [rsi]
	movq xmm6,[rbx+rax*8+1048576]
	paddsw xmm7,xmm6
	pextrw rax,xmm7,0
	mov edx,dword [rbx+rax*4+1572864]
	pextrw rax,xmm7,1
	or edx,dword [rbx+rax*4+1835008]
	pextrw rax,xmm7,2
	or edx,dword [rbx+rax*4+2097152]
	movnti dword [rdi],edx
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz y2r8flop6c

	add r9,qword [rsp+88] ;dbpl

	mov rax,qword [rsp+112] ;csLineBuff
	xchg rax,qword [rsp+120] ;csLineBuff2
	mov qword [rsp+112],rax ;csLineBuff

	align 16
yv2r8flopexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret*/
}

extern "C" void CSYUV420_RGB32C_do_yv12rgb2(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS yBpl, IntOS uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
{
}

