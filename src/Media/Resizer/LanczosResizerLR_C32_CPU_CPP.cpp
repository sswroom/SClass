#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"
#include "Core/ByteTool_C.h"
#include "Sync/ThreadUtil.h"

//#include "Text/MyString.h"
//#include <stdio.h>

extern "C" void LanczosResizerLR_C32_CPU_horizontal_filter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
{
	IntOS i;
	IntOS j;
	IntOS *currIndex;
	Int16 *currWeight;
	Int32x4 cvals;
	Int32x4 cvals2;
	Int32x4 cinit = PInt32x4SetA(16384);
	Int16x8 tmpVal1;
	Int16x8 tmpVal2;
	dstep -= width * 8;

	if ((width & 1) == 0 && (tap == 6 || tap == 8 || tap == 16))
	{
		if (tap == 6)
		{
			width = width >> 1;
			while (height-- > 0)
			{
				currWeight = (Int16*)weight;
				currIndex = index;
				i = width;
				while (i-- > 0)
				{
					cvals = cinit;
					tmpVal1 = PLoadInt16x8(&inPt[currIndex[0]]);
					tmpVal2 = PLoadInt16x8(&inPt[currIndex[1]]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[0])));
					cvals = PADDD4(cvals, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[8])));
					cvals = PADDD4(cvals, PMADDWD(PUNPCKWW4(PLoadInt16x4(&inPt[currIndex[2]]), PLoadInt16x4(&inPt[currIndex[2] + 8])), PLoadInt16x8A(&currWeight[16])));

					cvals2 = cinit;
					tmpVal1 = PLoadInt16x8(&inPt[currIndex[3]]);
					tmpVal2 = PLoadInt16x8(&inPt[currIndex[4]]);
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[24])));
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[32])));
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKWW4(PLoadInt16x4(&inPt[currIndex[5]]), PLoadInt16x4(&inPt[currIndex[5] + 8])), PLoadInt16x8A(&currWeight[40])));

					PStoreInt16x8(outPt, PSARSDW8(cvals, cvals2, 15));

					currWeight += 48;
					currIndex += 6;
					outPt += 16;
				}
				inPt += sstep;
				outPt += dstep;
			}
		}
		else if (tap == 8)
		{
			width = width >> 1;
			while (height-- > 0)
			{
				currWeight = (Int16*)weight;
				i = width;
				while (i-- > 0)
				{
					cvals = cinit;
					j = *(IntOS*)&currWeight[0];
					tmpVal1 = PLoadInt16x8(&inPt[j + 0]);
					tmpVal2 = PLoadInt16x8(&inPt[j + 16]);
/*					if (height == 0 && i == 100)
					{
						UTF8Char sbuff[512];
						Text::StrHexBytes(sbuff, &inPt[j], 32, ' ');
						printf("LanczosResizerLR_C32_CPU_horizontal_filter buff = %s\r\n", sbuff);
					}*/
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[8])));
					cvals = PADDD4(cvals, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[16])));
					tmpVal1 = PLoadInt16x8(&inPt[j + 32]);
					tmpVal2 = PLoadInt16x8(&inPt[j + 48]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[24])));
					cvals = PADDD4(cvals, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[32])));

					cvals2 = cinit;
					j = *(IntOS*)&currWeight[4];
					tmpVal1 = PLoadInt16x8(&inPt[j + 0]);
					tmpVal2 = PLoadInt16x8(&inPt[j + 16]);
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[40])));
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[48])));
					tmpVal1 = PLoadInt16x8(&inPt[j + 32]);
					tmpVal2 = PLoadInt16x8(&inPt[j + 48]);
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[56])));
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[64])));

/*					if (height == 0 && i == 100)
					{
						printf("LanczosResizerLR_C32_CPU_horizontal_filter cvals = %d, %d, %d, %d\r\n", PEXTD4(cvals, 0), PEXTD4(cvals, 1), PEXTD4(cvals, 2), PEXTD4(cvals, 3));
						printf("LanczosResizerLR_C32_CPU_horizontal_filter cvals2 = %d, %d, %d, %d\r\n", PEXTD4(cvals, 0), PEXTD4(cvals, 1), PEXTD4(cvals, 2), PEXTD4(cvals, 3));
					}*/
					PStoreInt16x8(outPt, PSARSDW8(cvals, cvals2, 15));
/*					if (height == 0 && i == 100)
					{
						UTF8Char sbuff[512];
						Text::StrHexBytes(sbuff, outPt, 16, ' ');
						printf("LanczosResizerLR_C32_CPU_horizontal_filter buff = %s\r\n", sbuff);
					}*/

					currWeight += 72;
					outPt += 16;
				}
				inPt += sstep;
				outPt += dstep;
			}
		}
		else if (tap == 16)
		{
			width = width >> 1;
			while (height-- > 0)
			{
				currWeight = (Int16*)weight;
				i = width;
				while (i-- > 0)
				{
					cvals = cinit;
					j = *(IntOS*)&currWeight[0];
					tmpVal1 = PLoadInt16x8(&inPt[j + 0]);
					tmpVal2 = PLoadInt16x8(&inPt[j + 16]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[8])));
					cvals = PADDD4(cvals, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[16])));
					tmpVal1 = PLoadInt16x8(&inPt[j + 32]);
					tmpVal2 = PLoadInt16x8(&inPt[j + 48]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[24])));
					cvals = PADDD4(cvals, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[32])));
					tmpVal1 = PLoadInt16x8(&inPt[j + 64]);
					tmpVal2 = PLoadInt16x8(&inPt[j + 80]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[40])));
					cvals = PADDD4(cvals, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[48])));
					tmpVal1 = PLoadInt16x8(&inPt[j + 96]);
					tmpVal2 = PLoadInt16x8(&inPt[j + 112]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[56])));
					cvals = PADDD4(cvals, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[64])));

					cvals2 = cinit;
					j = *(IntOS*)&currWeight[4];
					tmpVal1 = PLoadInt16x8(&inPt[j + 0]);
					tmpVal2 = PLoadInt16x8(&inPt[j + 16]);
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[72])));
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[80])));
					tmpVal1 = PLoadInt16x8(&inPt[j + 32]);
					tmpVal2 = PLoadInt16x8(&inPt[j + 48]);
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[88])));
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[96])));
					tmpVal1 = PLoadInt16x8(&inPt[j + 64]);
					tmpVal2 = PLoadInt16x8(&inPt[j + 80]);
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[104])));
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[112])));
					tmpVal1 = PLoadInt16x8(&inPt[j + 96]);
					tmpVal2 = PLoadInt16x8(&inPt[j + 112]);
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[120])));
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[128])));

					PStoreInt16x8(outPt, PSARSDW8(cvals, cvals2, 15));

					currWeight += 136;
					outPt += 16;
				}
				inPt += sstep;
				outPt += dstep;
			}
		}
	}
	else
	{
		tap = tap >> 1;
		while (height-- > 0)
		{
			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- > 0)
			{
				cvals = cinit;

				j = tap;
				while (j-- > 0)
				{
					cvals = PADDD4(cvals, PMADDWD(PUNPCKWW4(PLoadInt16x4(&inPt[currIndex[0]]), PLoadInt16x4(&inPt[currIndex[1]])), PLoadInt16x8A(&currWeight[0])));
					currIndex += 2;
					currWeight += 8;
				}
				PStoreInt16x4(outPt, PSARSDW4(cvals, 15));
				outPt += 8;
			}
			inPt += sstep;
			outPt += dstep;
		}
	}
}

extern "C" void LanczosResizerLR_C32_CPU_vertical_filter(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
{
	IntOS i;
	IntOS j;
	UInt8 *currIn;
	IntOS *currIndex;
	Int16 *currWeight;
	Int32x4 cvals;
	Int32x4 cvals0;
	UInt16x8 cvals2;
	UInt16x4 cvals3;
	Int32x4 cinit = PInt32x4SetA(16384);
	Int16x8 wVal1;
	Int16x8 wVal2;
	Int16x8 wVal3;
	Int16x8 tmpVal1;
	Int16x8 tmpVal2;
	Int16x4 tmpVal3;
	Int16x4 tmpVal4;
	UInt32 v;
	dstep -= width * 4;

	if (width & 1)
	{
		if (tap == 6)
		{
			UInt32 *wptr;
			width = width >> 1;
			currWeight = (Int16*)weight;
			while (height-- > 0)
			{
				wptr = (UInt32*)currWeight;
				currIn = inPt;
				wVal1 = PLoadInt16x8A(&currWeight[16]);
				wVal2 = PLoadInt16x8A(&currWeight[24]);
				wVal3 = PLoadInt16x8A(&currWeight[32]);
				i = width;
				while (i-- > 0)
				{
					cvals = cinit;
					cvals0 = cinit;
					tmpVal1 = PLoadInt16x8(&currIn[wptr[0]]);
					tmpVal2 = PLoadInt16x8(&currIn[wptr[1]]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal1));
					cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal1));
					tmpVal1 = PLoadInt16x8(&currIn[wptr[2]]);
					tmpVal2 = PLoadInt16x8(&currIn[wptr[3]]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal2));
					cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal2));
					tmpVal1 = PLoadInt16x8(&currIn[wptr[4]]);
					tmpVal2 = PLoadInt16x8(&currIn[wptr[5]]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal3));
					cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal3));
					cvals2 = PCONVI16x8_U(PMergeSARDW4(cvals, cvals0, 15));

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 0) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 1) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 2) * 4 + 524288];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 3) * 4 + 786432];
					*(UInt32*)&outPt[0] = v;
					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 4) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 5) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 6) * 4 + 524288];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 7) * 4 + 786432];
					*(UInt32*)&outPt[8] = v;
					currIn += 16;
					outPt += 8;
				}
				cvals = cinit;
				tmpVal3 = PLoadInt16x4(&currIn[wptr[0]]);
				tmpVal4 = PLoadInt16x4(&currIn[wptr[1]]);
				cvals = PADDD4(cvals, PMADDWD(PUNPCKWW4(tmpVal3, tmpVal4), wVal1));
				tmpVal3 = PLoadInt16x4(&currIn[wptr[2]]);
				tmpVal4 = PLoadInt16x4(&currIn[wptr[3]]);
				cvals = PADDD4(cvals, PMADDWD(PUNPCKWW4(tmpVal3, tmpVal4), wVal2));
				tmpVal3 = PLoadInt16x4(&currIn[wptr[4]]);
				tmpVal4 = PLoadInt16x4(&currIn[wptr[5]]);
				cvals = PADDD4(cvals, PMADDWD(PUNPCKWW4(tmpVal3, tmpVal4), wVal3));
				cvals3 = PCONVI16x4_U(PSARSDW4(cvals, 15));

				v  = *(UInt32*)&rgbTable[PEXTUW4(cvals3, 0) * 4 + 0];
				v |= *(UInt32*)&rgbTable[PEXTUW4(cvals3, 1) * 4 + 262144];
				v |= *(UInt32*)&rgbTable[PEXTUW4(cvals3, 2) * 4 + 524288];
				v |= *(UInt32*)&rgbTable[PEXTUW4(cvals3, 3) * 4 + 786432];
				*(UInt32*)&outPt[0] = v;
				currIn += 8;
				outPt += 4;

				currWeight += 40;
				outPt += dstep;
			}
		}
		else
		{
			width = width >> 1;
			tap = tap >> 1;
			while (height-- > 0)
			{
				currIn = inPt;
				i = width;
				while (i-- > 0)
				{
					cvals = cinit;
					cvals0 = cinit;
					currIndex = index;
					currWeight = (Int16*)weight;
					j = tap;
					while (j-- > 0)
					{
						tmpVal1 = PLoadInt16x8(&currIn[currIndex[0]]);
						tmpVal2 = PLoadInt16x8(&currIn[currIndex[1]]);
						wVal1 = PLoadInt16x8A(&currWeight[0]);
						cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal1));
						cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal1));
						currIndex += 2;
						currWeight += 8;
					}
					cvals2 = PCONVI16x8_U(PMergeSARDW4(cvals, cvals0, 15));

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 0) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 1) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 2) * 4 + 524288];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 3) * 4 + 786432];
					*(UInt32*)&outPt[0] = v;

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 4) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 5) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 6) * 4 + 524288];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 7) * 4 + 786432];
					*(UInt32*)&outPt[4] = v;

					currIn += 16;
					outPt += 8;
				}
				cvals = cinit;
				currIndex = index;
				currWeight = (Int16*)weight;
				j = tap;
				while (j-- > 0)
				{
					tmpVal3 = PLoadInt16x4(&currIn[currIndex[0]]);
					tmpVal4 = PLoadInt16x4(&currIn[currIndex[1]]);
					wVal1 = PLoadInt16x8A(&currWeight[0]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKWW4(tmpVal3, tmpVal4), wVal1));
					currIndex += 2;
					currWeight += 8;
				}
				cvals3 = PCONVI16x4_U(PSARSDW4(cvals, 15));

				v  = *(UInt32*)&rgbTable[PEXTUW4(cvals3, 0) * 4 + 0];
				v |= *(UInt32*)&rgbTable[PEXTUW4(cvals3, 1) * 4 + 262144];
				v |= *(UInt32*)&rgbTable[PEXTUW4(cvals3, 2) * 4 + 524288];
				v |= *(UInt32*)&rgbTable[PEXTUW4(cvals3, 3) * 4 + 786432];
				*(UInt32*)&outPt[0] = v;
				currIn += 8;
				outPt += 4;

				weight += tap << 1;
				index += tap << 1;
				outPt += dstep;
			}
		}
	}
	else
	{
		if (tap == 6)
		{
			UInt32 *wptr;
			width = width >> 1;
			currWeight = (Int16*)weight;
			while (height-- > 0)
			{
				wptr = (UInt32*)currWeight;
				currIn = inPt;
				wVal1 = PLoadInt16x8A(&currWeight[16]);
				wVal2 = PLoadInt16x8A(&currWeight[24]);
				wVal3 = PLoadInt16x8A(&currWeight[32]);
				i = width;
				while (i-- > 0)
				{
					cvals = cinit;
					cvals0 = cinit;
					tmpVal1 = PLoadInt16x8(&currIn[wptr[0]]);
					tmpVal2 = PLoadInt16x8(&currIn[wptr[1]]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal1));
					cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal1));
					tmpVal1 = PLoadInt16x8(&currIn[wptr[2]]);
					tmpVal2 = PLoadInt16x8(&currIn[wptr[3]]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal2));
					cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal2));
					tmpVal1 = PLoadInt16x8(&currIn[wptr[4]]);
					tmpVal2 = PLoadInt16x8(&currIn[wptr[5]]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal3));
					cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal3));
					cvals2 = PCONVI16x8_U(PMergeSARDW4(cvals, cvals0, 15));

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 0) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 1) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 2) * 4 + 524288];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 3) * 4 + 786432];
					*(UInt32*)&outPt[0] = v;
					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 4) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 5) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 6) * 4 + 524288];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 7) * 4 + 786432];
					*(UInt32*)&outPt[8] = v;
					currIn += 16;
					outPt += 8;
				}
				currWeight += 40;
				outPt += dstep;
			}
		}
		else
		{
			width = width >> 1;
			tap = tap >> 1;
			while (height-- > 0)
			{
				currIn = inPt;
				i = width;
				while (i-- > 0)
				{
					cvals = cinit;
					cvals0 = cinit;
					currIndex = index;
					currWeight = (Int16*)weight;
					j = tap;
					while (j-- > 0)
					{
						tmpVal1 = PLoadInt16x8(&currIn[currIndex[0]]);
						tmpVal2 = PLoadInt16x8(&currIn[currIndex[1]]);
						wVal1 = PLoadInt16x8A(&currWeight[0]);
						cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal1));
						cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal1));
						currIndex += 2;
						currWeight += 8;
					}
					cvals2 = PCONVI16x8_U(PMergeSARDW4(cvals, cvals0, 15));

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 0) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 1) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 2) * 4 + 524288];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 3) * 4 + 786432];
					*(UInt32*)&outPt[0] = v;

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 4) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 5) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 6) * 4 + 524288];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 7) * 4 + 786432];
					*(UInt32*)&outPt[4] = v;

					currIn += 16;
					outPt += 8;
				}
				weight += tap << 1;
				index += tap << 1;
				outPt += dstep;
			}
		}
	}
}

extern "C" void LanczosResizerLR_C32_CPU_vertical_filter_na(UInt8 *inPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
{
	IntOS i;
	IntOS j;
	UInt8 *currIn;
	IntOS *currIndex;
	Int16 *currWeight;
	Int32x4 cvals;
	Int32x4 cvals0;
	UInt16x8 cvals2;
	UInt16x4 cvals3;
	Int32x4 cinit = PInt32x4SetA(16384);
	Int16x8 wVal1;
	Int16x8 wVal2;
	Int16x8 wVal3;
	Int16x8 tmpVal1;
	Int16x8 tmpVal2;
	Int16x4 tmpVal3;
	Int16x4 tmpVal4;
	UInt32 v;
	dstep -= width * 4;
	if (width & 1)
	{
		if (tap == 6)
		{
			width = width >> 1;
			UInt32 *wptr;
			currWeight = (Int16*)weight;
			while (height-- > 0)
			{
				wptr = (UInt32*)currWeight;
				currIn = inPt;
				wVal1 = PLoadInt16x8A(&currWeight[16]);
				wVal2 = PLoadInt16x8A(&currWeight[24]);
				wVal3 = PLoadInt16x8A(&currWeight[32]);
				i = width;
				while (i-- > 0)
				{
					cvals = cinit;
					cvals0 = cinit;
					tmpVal1 = PLoadInt16x8(&currIn[wptr[0]]);
					tmpVal2 = PLoadInt16x8(&currIn[wptr[1]]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal1));
					cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal1));
					tmpVal1 = PLoadInt16x8(&currIn[wptr[2]]);
					tmpVal2 = PLoadInt16x8(&currIn[wptr[3]]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal2));
					cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal2));
					tmpVal1 = PLoadInt16x8(&currIn[wptr[4]]);
					tmpVal2 = PLoadInt16x8(&currIn[wptr[5]]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal3));
					cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal3));
					cvals2 = PCONVI16x8_U(PMergeSARDW4(cvals, cvals0, 15));
	
/*					if (height == 0 && i == 100)
					{
						printf("LanczosResizerLR_C32_CPU_vertical_filter_na cvals %d %d %d %d\r\n", PEXTD4(cvals, 0), PEXTD4(cvals, 1), PEXTD4(cvals, 2), PEXTD4(cvals, 3));
						printf("LanczosResizerLR_C32_CPU_vertical_filter_na cvals0 %d %d %d %d\r\n", PEXTD4(cvals0, 0), PEXTD4(cvals0, 1), PEXTD4(cvals0, 2), PEXTD4(cvals0, 3));
						printf("LanczosResizerLR_C32_CPU_vertical_filter_na cvals2 %d %d %d %d %d %d %d %d\r\n", PEXTUW8(cvals2, 0), PEXTUW8(cvals2, 1), PEXTUW8(cvals2, 2), PEXTUW8(cvals2, 3), PEXTUW8(cvals2, 4), PEXTUW8(cvals2, 5), PEXTUW8(cvals2, 6), PEXTUW8(cvals2, 7));
					}*/

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 0) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 1) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 2) * 4 + 524288];
					v |= 0xff000000;
					*(UInt32*)&outPt[0] = v;

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 4) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 5) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 6) * 4 + 524288];
					v |= 0xff000000;
					*(UInt32*)&outPt[4] = v;

					currIn += 16;
					outPt += 8;
				}

				cvals = cinit;
				tmpVal3 = PLoadInt16x4(&currIn[wptr[0]]);
				tmpVal4 = PLoadInt16x4(&currIn[wptr[1]]);
				cvals = PADDD4(cvals, PMADDWD(PUNPCKWW4(tmpVal3, tmpVal4), wVal1));
				tmpVal3 = PLoadInt16x4(&currIn[wptr[2]]);
				tmpVal4 = PLoadInt16x4(&currIn[wptr[3]]);
				cvals = PADDD4(cvals, PMADDWD(PUNPCKWW4(tmpVal3, tmpVal4), wVal2));
				tmpVal3 = PLoadInt16x4(&currIn[wptr[4]]);
				tmpVal4 = PLoadInt16x4(&currIn[wptr[5]]);
				cvals = PADDD4(cvals, PMADDWD(PUNPCKWW4(tmpVal3, tmpVal4), wVal3));
				cvals3 = PCONVI16x4_U(PSARSDW4(cvals, 15));

				v  = *(UInt32*)&rgbTable[PEXTUW4(cvals3, 0) * 4 + 0];
				v |= *(UInt32*)&rgbTable[PEXTUW4(cvals3, 1) * 4 + 262144];
				v |= *(UInt32*)&rgbTable[PEXTUW4(cvals3, 2) * 4 + 524288];
				v |= 0xff000000;
				*(UInt32*)&outPt[0] = v;
				currIn += 8;
				outPt += 4;


				currWeight += 40;
				outPt += dstep;
			}
		}
		else
		{
			width = width >> 1;
			tap = tap >> 1;
			while (height-- > 0)
			{
				currIn = inPt;
				i = width;
				while (i-- > 0)
				{
					cvals = cinit;
					cvals0 = cinit;
					currIndex = index;
					currWeight = (Int16*)weight;
					j = tap;
					while (j-- > 0)
					{
						tmpVal1 = PLoadInt16x8(&currIn[currIndex[0]]);
						tmpVal2 = PLoadInt16x8(&currIn[currIndex[1]]);
						wVal1 = PLoadInt16x8A(&currWeight[0]);
						cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal1));
						cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal1));
						currIndex += 2;
						currWeight += 8;
					}
					cvals2 = PCONVI16x8_U(PMergeSARDW4(cvals, cvals0, 15));

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 0) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 1) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 2) * 4 + 524288];
					v |= 0xff000000;
					*(UInt32*)&outPt[0] = v;

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 4) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 5) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 6) * 4 + 524288];
					v |= 0xff000000;
					*(UInt32*)&outPt[4] = v;

					currIn += 16;
					outPt += 8;
				}
				cvals = cinit;
				currIndex = index;
				currWeight = (Int16*)weight;
				j = tap;
				while (j-- > 0)
				{
					tmpVal3 = PLoadInt16x4(&currIn[currIndex[0]]);
					tmpVal4 = PLoadInt16x4(&currIn[currIndex[1]]);
					wVal1 = PLoadInt16x8A(&currWeight[0]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKWW4(tmpVal3, tmpVal4), wVal1));
					currIndex += 2;
					currWeight += 8;
				}
				cvals3 = PCONVI16x4_U(PSARSDW4(cvals, 15));

				v  = *(UInt32*)&rgbTable[PEXTUW4(cvals3, 0) * 4 + 0];
				v |= *(UInt32*)&rgbTable[PEXTUW4(cvals3, 1) * 4 + 262144];
				v |= *(UInt32*)&rgbTable[PEXTUW4(cvals3, 2) * 4 + 524288];
				v |= 0xff000000;
				*(UInt32*)&outPt[0] = v;

				currIn += 8;
				outPt += 4;

				weight += tap << 1;
				index += tap << 1;
				outPt += dstep;
			}
		}
	}
	else
	{
		if (tap == 6)
		{
			width = width >> 1;
			UInt32 *wptr;
			currWeight = (Int16*)weight;
			while (height-- > 0)
			{
				wptr = (UInt32*)currWeight;
				currIn = inPt;
				wVal1 = PLoadInt16x8A(&currWeight[16]);
				wVal2 = PLoadInt16x8A(&currWeight[24]);
				wVal3 = PLoadInt16x8A(&currWeight[32]);
				i = width;
				while (i-- > 0)
				{
					cvals = cinit;
					cvals0 = cinit;
					tmpVal1 = PLoadInt16x8(&currIn[wptr[0]]);
					tmpVal2 = PLoadInt16x8(&currIn[wptr[1]]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal1));
					cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal1));
					tmpVal1 = PLoadInt16x8(&currIn[wptr[2]]);
					tmpVal2 = PLoadInt16x8(&currIn[wptr[3]]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal2));
					cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal2));
					tmpVal1 = PLoadInt16x8(&currIn[wptr[4]]);
					tmpVal2 = PLoadInt16x8(&currIn[wptr[5]]);
					cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal3));
					cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal3));
					cvals2 = PCONVI16x8_U(PMergeSARDW4(cvals, cvals0, 15));
	
/*					if (height == 0 && i == 100)
					{
						printf("LanczosResizerLR_C32_CPU_vertical_filter_na cvals %d %d %d %d\r\n", PEXTD4(cvals, 0), PEXTD4(cvals, 1), PEXTD4(cvals, 2), PEXTD4(cvals, 3));
						printf("LanczosResizerLR_C32_CPU_vertical_filter_na cvals0 %d %d %d %d\r\n", PEXTD4(cvals0, 0), PEXTD4(cvals0, 1), PEXTD4(cvals0, 2), PEXTD4(cvals0, 3));
						printf("LanczosResizerLR_C32_CPU_vertical_filter_na cvals2 %d %d %d %d %d %d %d %d\r\n", PEXTUW8(cvals2, 0), PEXTUW8(cvals2, 1), PEXTUW8(cvals2, 2), PEXTUW8(cvals2, 3), PEXTUW8(cvals2, 4), PEXTUW8(cvals2, 5), PEXTUW8(cvals2, 6), PEXTUW8(cvals2, 7));
					}*/

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 0) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 1) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 2) * 4 + 524288];
					v |= 0xff000000;
					*(UInt32*)&outPt[0] = v;

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 4) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 5) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 6) * 4 + 524288];
					v |= 0xff000000;
					*(UInt32*)&outPt[4] = v;

					currIn += 16;
					outPt += 8;
				}
				currWeight += 40;
				outPt += dstep;
			}
		}
		else
		{
			width = width >> 1;
			tap = tap >> 1;
			while (height-- > 0)
			{
				currIn = inPt;
				i = width;
				while (i-- > 0)
				{
					cvals = cinit;
					cvals0 = cinit;
					currIndex = index;
					currWeight = (Int16*)weight;
					j = tap;
					while (j-- > 0)
					{
						tmpVal1 = PLoadInt16x8(&currIn[currIndex[0]]);
						tmpVal2 = PLoadInt16x8(&currIn[currIndex[1]]);
						wVal1 = PLoadInt16x8A(&currWeight[0]);
						cvals = PADDD4(cvals, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), wVal1));
						cvals0 = PADDD4(cvals0, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), wVal1));
						currIndex += 2;
						currWeight += 8;
					}
					cvals2 = PCONVI16x8_U(PMergeSARDW4(cvals, cvals0, 15));

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 0) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 1) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 2) * 4 + 524288];
					v |= 0xff000000;
					*(UInt32*)&outPt[0] = v;

					v  = *(UInt32*)&rgbTable[PEXTUW8(cvals2, 4) * 4 + 0];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 5) * 4 + 262144];
					v |= *(UInt32*)&rgbTable[PEXTUW8(cvals2, 6) * 4 + 524288];
					v |= 0xff000000;
					*(UInt32*)&outPt[4] = v;

					currIn += 16;
					outPt += 8;
				}
				weight += tap << 1;
				index += tap << 1;
				outPt += dstep;
			}
		}
	}
}

extern "C" void LanczosResizerLR_C32_CPU_hv_filter(UInt8 *inPt, UInt8 *outPt, UIntOS dwidth, UIntOS dheight, UIntOS swidth, UIntOS htap, IntOS *hindex, Int64 *hweight, IntOS vtap, IntOS *vindex, Int64 *vweight, IntOS sstep, IntOS dstep, UInt8 *rgbTable, UInt8 *buffPt)
{
}

extern "C" void LanczosResizerLR_C32_CPU_collapse(UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
{
	UIntOS i;
	UInt32 v;
	sstep = sstep - (IntOS)width * 8;
	dstep = dstep - (IntOS)width * 4;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			v  = ReadUInt32(&rgbTable[ReadUInt16(&inPt[0]) * 4 + 0]);
			v |= ReadUInt32(&rgbTable[ReadUInt16(&inPt[2]) * 4 + 262144]);
			v |= ReadUInt32(&rgbTable[ReadUInt16(&inPt[4]) * 4 + 524288]);
			v |= ReadUInt32(&rgbTable[ReadUInt16(&inPt[6]) * 4 + 786432]);
			WriteUInt32(outPt, v);
			inPt += 8;
			outPt += 4;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerLR_C32_CPU_collapse_na(UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
{
	UIntOS i;
	UInt32 v;
	sstep = sstep - (IntOS)width * 8;
	dstep = dstep - (IntOS)width * 4;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			v  = ReadUInt32(&rgbTable[ReadUInt16(&inPt[0]) * 4 + 0]) | 0xff000000;
			v |= ReadUInt32(&rgbTable[ReadUInt16(&inPt[2]) * 4 + 262144]);
			v |= ReadUInt32(&rgbTable[ReadUInt16(&inPt[4]) * 4 + 524288]);
			WriteUInt32(outPt, v);
			inPt += 8;
			outPt += 4;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

