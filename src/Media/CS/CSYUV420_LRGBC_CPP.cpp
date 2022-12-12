#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"

extern "C" void CSYUV420_LRGBC_VerticalFilterLRGB(UInt8 *inYPt, UInt8 *inUPt, UInt8 *inVPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weighti64, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt ystep, OSInt dstep, Int64 *yuv2rgbi64, Int64 *rgbGammaCorri64)
{
	Int32x4 tmpV = PInt32x4SetA(32768);
	OSInt yAdd = ystep - width;
	OSInt sWidth = width >> 3;
	OSInt cSub = (width >> 1) - 2;
	OSInt i;
	OSInt index1;
	OSInt index2;
	OSInt index3;
	OSInt index4;
	UInt8 *tmpPtr1;
	UInt8 *tmpPtr2;
	UInt8 *tmpPtr3;
	Int16x8 mulVal1;
	Int16x8 mulVal2;
	Int32x4 i32Val1;
	Int32x4 i32Val2;
	Int16x8 i16Val1;
	Int16x8 i16Val2;
	const UInt8 *weight = (const UInt8*)weighti64;
	const UInt8 *yuv2rgb = (const UInt8*)yuv2rgbi64;
	const UInt8 *rgbGammaCorr = (const UInt8*)rgbGammaCorri64;
	if (tap != 4)
		return;
	
	if (width & 7)
	{
		OSInt widthLeft = (width & 7) >> 2;
		while (height-- > 0)
		{
			tmpPtr1 = inUPt; //rcx
			tmpPtr2 = inVPt; //rdx
			tmpPtr3 = csLineBuff2; //rsi

			mulVal1 = PLoadInt16x8A(&weight[16]); //xmm5
			mulVal2 = PLoadInt16x8A(&weight[32]); //xmm6

			i = sWidth;
			while (i-- > 0)
			{
				i32Val1 = tmpV;
				i32Val2 = tmpV;
				i16Val1 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[*(UInt32*)&weight[0]]), PLoadUInt8x4(&tmpPtr2[*(UInt32*)&weight[0]]))), 1));
				i16Val2 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[*(UInt32*)&weight[4]]), PLoadUInt8x4(&tmpPtr2[*(UInt32*)&weight[4]]))), 1));
				i32Val1 = PADDD4(i32Val1, PMADDWD(PUNPCKLWW8(i16Val1, i16Val2), mulVal1));
				i32Val2 = PADDD4(i32Val2, PMADDWD(PUNPCKHWW8(i16Val1, i16Val2), mulVal1));
				i16Val1 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[*(UInt32*)&weight[8]]), PLoadUInt8x4(&tmpPtr2[*(UInt32*)&weight[8]]))), 1));
				i16Val2 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[*(UInt32*)&weight[12]]), PLoadUInt8x4(&tmpPtr2[*(UInt32*)&weight[12]]))), 1));
				i32Val1 = PADDD4(i32Val1, PMADDWD(PUNPCKLWW8(i16Val1, i16Val2), mulVal2));
				i32Val2 = PADDD4(i32Val2, PMADDWD(PUNPCKHWW8(i16Val1, i16Val2), mulVal2));
				PStoreInt16x8(tmpPtr3, SI32ToI16x8(PSARD4(i32Val1, 16), PSARD4(i32Val2, 16)));
				tmpPtr1 += 4;
				tmpPtr2 += 4;
				tmpPtr3 += 16;
			}

			if (widthLeft)
			{
				i32Val1 = tmpV;
				i32Val2 = tmpV;
				i16Val1 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[*(UInt32*)&weight[0]]), PLoadUInt8x4(&tmpPtr2[*(UInt32*)&weight[0]]))), 1));
				i16Val2 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[*(UInt32*)&weight[4]]), PLoadUInt8x4(&tmpPtr2[*(UInt32*)&weight[4]]))), 1));
				i32Val1 = PADDD4(i32Val1, PMADDWD(PUNPCKLWW8(i16Val1, i16Val2), mulVal1));
				i16Val1 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[*(UInt32*)&weight[8]]), PLoadUInt8x4(&tmpPtr2[*(UInt32*)&weight[8]]))), 1));
				i16Val2 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[*(UInt32*)&weight[12]]), PLoadUInt8x4(&tmpPtr2[*(UInt32*)&weight[12]]))), 1));
				i32Val1 = PADDD4(i32Val1, PMADDWD(PUNPCKLWW8(i16Val1, i16Val2), mulVal2));
				PStoreInt16x4(tmpPtr3, SI32ToI16x4(PSARD4(i32Val1, 16)));
				tmpPtr1 += 2;
				tmpPtr2 += 2;
				tmpPtr3 += 8;
			}

			if (width & 3)
			{
				i32Val1 = tmpV;
				i32Val2 = tmpV;
				i16Val1 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[*(UInt32*)&weight[0]]), PLoadUInt8x4(&tmpPtr2[*(UInt32*)&weight[0]]))), 1));
				i16Val2 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[*(UInt32*)&weight[4]]), PLoadUInt8x4(&tmpPtr2[*(UInt32*)&weight[4]]))), 1));
				i32Val1 = PADDD4(i32Val1, PMADDWD(PUNPCKLWW8(i16Val1, i16Val2), mulVal1));
				i16Val1 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[*(UInt32*)&weight[8]]), PLoadUInt8x4(&tmpPtr2[*(UInt32*)&weight[8]]))), 1));
				i16Val2 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[*(UInt32*)&weight[12]]), PLoadUInt8x4(&tmpPtr2[*(UInt32*)&weight[12]]))), 1));
				i32Val1 = PADDD4(i32Val1, PMADDWD(PUNPCKLWW8(i16Val1, i16Val2), mulVal2));
				PStoreInt16x2(tmpPtr3, SI32ToI16x4(PSARD4(i32Val1, 16)));
				tmpPtr3 += 4;
			}

			i = cSub; //rbp;
			tmpPtr1 = inYPt; //rsi
			tmpPtr2 = csLineBuff; //rdi
			tmpPtr3 = csLineBuff2; //rcx

			i16Val1 = PMergeW4(PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[0]]), PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[1]]));
			i16Val2 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
			i16Val1 = PSADDW8(i16Val1, i16Val2);
			i16Val1 = PSADDW8(i16Val1, PSARW8(PMergeLW4(PInt16x8Clear(), i16Val2), 1));
			tmpPtr1 += 2;
			tmpPtr3 += 4;

			while (i-- > 0)
			{
				i16Val2 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
				mulVal1 = PSARW8(PMergeLW4(PInt16x8Clear(), i16Val2), 1);
				PStoreInt16x8A(tmpPtr2, PSADDW8(i16Val1, mulVal1));

				i16Val1 = PMergeW4(PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[0]]), PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[1]]));
				i16Val1 = PSADDW8(i16Val1, i16Val2);
				i16Val1 = PSADDW8(i16Val1, mulVal1);
				tmpPtr1 += 2;
				tmpPtr2 += 16;
				tmpPtr3 += 4;
			}

			i16Val2 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
			mulVal2 = PMergeLW4(PInt16x8Clear(), i16Val2);
			mulVal1 = PSARW8(mulVal2, 1);
			PStoreInt16x8A(tmpPtr2, PSADDW8(i16Val1, mulVal1));
			i16Val1 = PMergeW4(PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[0]]), PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[1]]));
			i16Val1 = PSADDW8(i16Val1, i16Val2);
			i16Val1 = PSADDW8(i16Val1, mulVal2);
			PStoreInt16x8A(&tmpPtr2[16], i16Val1);
			inYPt = tmpPtr1 + 2 + yAdd;
			tmpPtr3 += 4;

			tmpPtr1 = outPt; //rdi
			tmpPtr2 = csLineBuff; //rsi
			OSInt i = width;
			while (i-- > 0)
			{
				PStoreInt16x4(tmpPtr1, PSADDW4(PSADDW4(PLoadInt16x4(&rgbGammaCorr[0 + 8 * *(UInt16*)&tmpPtr2[4]]), PLoadInt16x4(&rgbGammaCorr[524288 + 8 * *(UInt16*)&tmpPtr2[2]])), PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * *(UInt16*)&tmpPtr2[0]])));
				tmpPtr1 += 8;
				tmpPtr2 += 8;
			}

			weight += 48;
			outPt += dstep;
		}
	}
	else
	{
		if ((15 & (OSInt)outPt) == 0 && (dstep & 15) == 0)
		{
			while (height-- > 0)
			{
				tmpPtr1 = inUPt; //rcx
				tmpPtr2 = inVPt; //rdx
				tmpPtr3 = csLineBuff2; //rsi

				index1 = *(UInt32*)&weight[0];
				index2 = *(UInt32*)&weight[4];
				index3 = *(UInt32*)&weight[8];
				index4 = *(UInt32*)&weight[12];
				mulVal1 = PLoadInt16x8A(&weight[16]); //xmm5
				mulVal2 = PLoadInt16x8A(&weight[32]); //xmm6

				weight += 48;

				i = sWidth;
				while (i-- > 0)
				{
					i32Val1 = tmpV;
					i32Val2 = tmpV;
					i16Val1 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[index1]), PLoadUInt8x4(&tmpPtr2[index1]))), 1));
					i16Val2 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[index2]), PLoadUInt8x4(&tmpPtr2[index2]))), 1));
					i32Val1 = PADDD4(i32Val1, PMADDWD(PUNPCKLWW8(i16Val1, i16Val2), mulVal1));
					i32Val2 = PADDD4(i32Val2, PMADDWD(PUNPCKHWW8(i16Val1, i16Val2), mulVal1));
					i16Val1 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[index3]), PLoadUInt8x4(&tmpPtr2[index3]))), 1));
					i16Val2 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[index4]), PLoadUInt8x4(&tmpPtr2[index4]))), 1));
					i32Val1 = PADDD4(i32Val1, PMADDWD(PUNPCKLWW8(i16Val1, i16Val2), mulVal2));
					i32Val2 = PADDD4(i32Val2, PMADDWD(PUNPCKHWW8(i16Val1, i16Val2), mulVal2));
					PStoreInt16x8A(tmpPtr3, SI32ToI16x8(PSARD4(i32Val1, 16), PSARD4(i32Val2, 16)));
					tmpPtr1 += 4;
					tmpPtr2 += 4;
					tmpPtr3 += 16;
				}

				i = cSub; //rbp;
				tmpPtr1 = inYPt; //rsi
				tmpPtr2 = outPt; //rdi
				tmpPtr3 = csLineBuff2; //rcx

				i16Val1 = PMergeW4(PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[0]]), PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[1]]));
				i16Val2 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
				i16Val1 = PSADDW8(i16Val1, i16Val2);
				i16Val1 = PSADDW8(i16Val1, PSARW8(PMergeLW4(PInt16x8Clear(), i16Val2), 1));
				tmpPtr1 += 2;
				tmpPtr3 += 4;

				while (i-- > 0)
				{
					i16Val2 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
					mulVal1 = PSARW8(PMergeLW4(PInt16x8Clear(), i16Val2), 1);
					i16Val1 = PSADDW8(i16Val1, mulVal1);
					mulVal2 = PMergeW4(PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW8(i16Val1, 0)]), PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW8(i16Val1, 4)]));
					mulVal2 = PSADDW8(mulVal2, PMergeW4(PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW8(i16Val1, 1)]), PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW8(i16Val1, 5)])));
					mulVal2 = PSADDW8(mulVal2, PMergeW4(PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW8(i16Val1, 2)]), PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW8(i16Val1, 6)])));
					PStoreInt16x8NC(tmpPtr2, mulVal2);

					i16Val1 = PMergeW4(PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[0]]), PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[1]]));
					i16Val1 = PSADDW8(i16Val1, i16Val2);
					i16Val1 = PSADDW8(i16Val1, mulVal1);
					tmpPtr1 += 2;
					tmpPtr2 += 16;
					tmpPtr3 += 4;
				}

				i16Val2 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
				mulVal2 = PMergeLW4(PInt16x8Clear(), i16Val2);
				mulVal1 = PSARW8(mulVal2, 1);
				i16Val1 = PSADDW8(i16Val1, mulVal1);
				mulVal1 = PMergeW4(PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW8(i16Val1, 0)]), PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW8(i16Val1, 4)]));
				mulVal1 = PSADDW8(mulVal1, PMergeW4(PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW8(i16Val1, 1)]), PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW8(i16Val1, 5)])));
				mulVal1 = PSADDW8(mulVal1, PMergeW4(PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW8(i16Val1, 2)]), PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW8(i16Val1, 6)])));
				PStoreInt16x8NC(tmpPtr2, mulVal1);
				i16Val1 = PMergeW4(PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[0]]), PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[1]]));
				i16Val1 = PSADDW8(i16Val1, i16Val2);
				i16Val1 = PSADDW8(i16Val1, mulVal2);
				mulVal1 = PMergeW4(PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW8(i16Val1, 0)]), PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW8(i16Val1, 4)]));
				mulVal1 = PSADDW8(mulVal1, PMergeW4(PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW8(i16Val1, 1)]), PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW8(i16Val1, 5)])));
				mulVal1 = PSADDW8(mulVal1, PMergeW4(PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW8(i16Val1, 2)]), PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW8(i16Val1, 6)])));
				PStoreInt16x8NC(&tmpPtr2[16], mulVal1);
				inYPt = tmpPtr1 + 2 + yAdd;

				outPt += dstep;
			}
		}
		else
		{
			while (height-- > 0)
			{
				tmpPtr1 = inUPt; //rcx
				tmpPtr2 = inVPt; //rdx
				tmpPtr3 = csLineBuff2; //rsi

				index1 = *(UInt32*)&weight[0];
				index2 = *(UInt32*)&weight[4];
				index3 = *(UInt32*)&weight[8];
				index4 = *(UInt32*)&weight[12];
				mulVal1 = PLoadInt16x8A(&weight[16]); //xmm5
				mulVal2 = PLoadInt16x8A(&weight[32]); //xmm6

				weight += 48;

				i = sWidth;
				while (i-- > 0)
				{
					i32Val1 = tmpV;
					i32Val2 = tmpV;
					i16Val1 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[index1]), PLoadUInt8x4(&tmpPtr2[index1]))), 1));
					i16Val2 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[index2]), PLoadUInt8x4(&tmpPtr2[index2]))), 1));
					i32Val1 = PADDD4(i32Val1, PMADDWD(PUNPCKLWW8(i16Val1, i16Val2), mulVal1));
					i32Val2 = PADDD4(i32Val2, PMADDWD(PUNPCKHWW8(i16Val1, i16Val2), mulVal1));
					i16Val1 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[index3]), PLoadUInt8x4(&tmpPtr2[index3]))), 1));
					i16Val2 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(PUInt8x8Clear(), PUNPCKUBB4(PLoadUInt8x4(&tmpPtr1[index4]), PLoadUInt8x4(&tmpPtr2[index4]))), 1));
					i32Val1 = PADDD4(i32Val1, PMADDWD(PUNPCKLWW8(i16Val1, i16Val2), mulVal2));
					i32Val2 = PADDD4(i32Val2, PMADDWD(PUNPCKHWW8(i16Val1, i16Val2), mulVal2));
					PStoreInt16x8A(tmpPtr3, SI32ToI16x8(PSARD4(i32Val1, 16), PSARD4(i32Val2, 16)));
					tmpPtr1 += 4;
					tmpPtr2 += 4;
					tmpPtr3 += 16;
				}


				i = cSub; //rbp;
				tmpPtr1 = inYPt; //rsi
				tmpPtr2 = outPt; //rdi
				tmpPtr3 = csLineBuff2; //rcx

				i16Val1 = PMergeW4(PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[0]]), PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[1]]));
				i16Val2 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
				i16Val1 = PSADDW8(i16Val1, i16Val2);
				i16Val1 = PSADDW8(i16Val1, PSARW8(PMergeLW4(PInt16x8Clear(), i16Val2), 1));
				tmpPtr1 += 2;
				tmpPtr3 += 4;

				while (i-- > 0)
				{
					i16Val2 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
					mulVal1 = PSARW8(PMergeLW4(PInt16x8Clear(), i16Val2), 1);
					i16Val1 = PSADDW8(i16Val1, mulVal1);
					mulVal2 = PMergeW4(PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW8(i16Val1, 0)]), PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW8(i16Val1, 4)]));
					mulVal2 = PSADDW8(mulVal2, PMergeW4(PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW8(i16Val1, 1)]), PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW8(i16Val1, 5)])));
					mulVal2 = PSADDW8(mulVal2, PMergeW4(PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW8(i16Val1, 2)]), PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW8(i16Val1, 6)])));
					PStoreInt16x8(tmpPtr2, mulVal2);

					i16Val1 = PMergeW4(PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[0]]), PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[1]]));
					i16Val1 = PSADDW8(i16Val1, i16Val2);
					i16Val1 = PSADDW8(i16Val1, mulVal1);
					tmpPtr1 += 2;
					tmpPtr2 += 16;
					tmpPtr3 += 4;
				}

				i16Val2 = PMergeW4(PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * *(UInt16*)&tmpPtr3[0]]), PLoadInt16x4(&yuv2rgb[526336 + 8 * *(UInt16*)&tmpPtr3[2]])), PInt16x4Clear());
				mulVal2 = PMergeLW4(PInt16x8Clear(), i16Val2);
				mulVal1 = PSARW8(mulVal2, 1);
				i16Val1 = PSADDW8(i16Val1, mulVal1);
				mulVal1 = PMergeW4(PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW8(i16Val1, 0)]), PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW8(i16Val1, 4)]));
				mulVal1 = PSADDW8(mulVal1, PMergeW4(PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW8(i16Val1, 1)]), PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW8(i16Val1, 5)])));
				mulVal1 = PSADDW8(mulVal1, PMergeW4(PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW8(i16Val1, 2)]), PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW8(i16Val1, 6)])));
				PStoreInt16x8(tmpPtr2, mulVal1);
				i16Val1 = PMergeW4(PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[0]]), PLoadInt16x4(&yuv2rgb[8 * tmpPtr1[1]]));
				i16Val1 = PSADDW8(i16Val1, i16Val2);
				i16Val1 = PSADDW8(i16Val1, mulVal2);
				mulVal1 = PMergeW4(PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW8(i16Val1, 0)]), PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW8(i16Val1, 4)]));
				mulVal1 = PSADDW8(mulVal1, PMergeW4(PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW8(i16Val1, 1)]), PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW8(i16Val1, 5)])));
				mulVal1 = PSADDW8(mulVal1, PMergeW4(PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW8(i16Val1, 2)]), PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW8(i16Val1, 6)])));
				PStoreInt16x8(&tmpPtr2[16], mulVal1);
				inYPt = tmpPtr1 + 2 + yAdd;

				outPt += dstep;
			}
		}
	}
}

extern "C" void CSYUV420_LRGBC_do_yv12rgb8(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
{
}

extern "C" void CSYUV420_LRGBC_do_yv12rgb2(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
{
}

