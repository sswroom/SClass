#include "Stdafx.h"
#include "SIMD.h"
#include "Core/ByteTool_C.h"

#include "Text/MyString.h"
#include <stdio.h>

extern "C" void LanczosResizerH8_8_horizontal_filter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
	OSInt i;
	OSInt j;
	OSInt *currIndex;
	Int16 *currWeight;
	UInt8 *currw;
	UOSInt v;
	Int16x8 cvals;
	Int16x8 cvals2;
	Int16x8 cvals3;
	Int16x4 cvals4;
	if ((width & 3) != 0 || (tap & 1))
	{
		Int16x4 valadd = PInt16x4SetA(-128);
		UInt8x4 valadd2 = PUInt8x4SetA(128);
		dstep -= width << 2;
		while (height-- > 0)
		{
			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- > 0)
			{
				cvals4 = PInt16x4Clear();
				j = tap;
				while (j-- > 0)
				{
					cvals4 = PSADDW4(cvals4, PMULHW4(PLoadInt16x4(&inPt[currIndex[0] * 2 + 0]), PLoadInt16x4(currWeight)));
					currIndex++;
					currWeight += 4;
				}
				PStoreUInt8x4(outPt, PADDUB4(PCONVI8x4_U(SI16ToI8x4(PADDW4(PSARW4(cvals4, 5), valadd))), valadd2));
				outPt += 4;
			}
			inPt += sstep;
			outPt += dstep;
		}
	}
	else if (tap == 8)
	{
		Int16x8 valadd = PInt16x8SetA(-128);
		UInt8x16 valadd2 = PUInt8x16SetA(128);
		dstep -= width << 2;
		width = width >> 2;
		while (height-- > 0)
		{
			currw = (UInt8*)weight;
			i = width;
			while (i-- > 0)
			{
				v = *(UInt32*)&currw[0];
				cvals = PInt16x8Clear();
				cvals = PSADDW8(cvals, PMULHW8(PLoadInt16x8(&inPt[v * 2 +  0]), PLoadInt16x8A(&currw[16])));
				cvals = PSADDW8(cvals, PMULHW8(PLoadInt16x8(&inPt[v * 2 + 16]), PLoadInt16x8A(&currw[32])));
				cvals = PSADDW8(cvals, PMULHW8(PLoadInt16x8(&inPt[v * 2 + 32]), PLoadInt16x8A(&currw[48])));
				cvals = PSADDW8(cvals, PMULHW8(PLoadInt16x8(&inPt[v * 2 + 48]), PLoadInt16x8A(&currw[64])));

				v = *(UInt32*)&currw[4];
				cvals2 = PInt16x8Clear();
				cvals2 = PSADDW8(cvals2, PMULHW8(PLoadInt16x8(&inPt[v * 2 +  0]), PLoadInt16x8A(&currw[80])));
				cvals2 = PSADDW8(cvals2, PMULHW8(PLoadInt16x8(&inPt[v * 2 + 16]), PLoadInt16x8A(&currw[96])));
				cvals2 = PSADDW8(cvals2, PMULHW8(PLoadInt16x8(&inPt[v * 2 + 32]), PLoadInt16x8A(&currw[112])));
				cvals2 = PSADDW8(cvals2, PMULHW8(PLoadInt16x8(&inPt[v * 2 + 48]), PLoadInt16x8A(&currw[128])));
				cvals = PHSADDW8_4(cvals, cvals2);

				v = *(UInt32*)&currw[8];
				cvals3 = PInt16x8Clear();
				cvals3 = PSADDW8(cvals3, PMULHW8(PLoadInt16x8(&inPt[v * 2 +  0]), PLoadInt16x8A(&currw[144])));
				cvals3 = PSADDW8(cvals3, PMULHW8(PLoadInt16x8(&inPt[v * 2 + 16]), PLoadInt16x8A(&currw[160])));
				cvals3 = PSADDW8(cvals3, PMULHW8(PLoadInt16x8(&inPt[v * 2 + 32]), PLoadInt16x8A(&currw[176])));
				cvals3 = PSADDW8(cvals3, PMULHW8(PLoadInt16x8(&inPt[v * 2 + 48]), PLoadInt16x8A(&currw[192])));

				v = *(UInt32*)&currw[12];
				cvals2 = PInt16x8Clear();
				cvals2 = PSADDW8(cvals2, PMULHW8(PLoadInt16x8(&inPt[v * 2 +  0]), PLoadInt16x8A(&currw[208])));
				cvals2 = PSADDW8(cvals2, PMULHW8(PLoadInt16x8(&inPt[v * 2 + 16]), PLoadInt16x8A(&currw[224])));
				cvals2 = PSADDW8(cvals2, PMULHW8(PLoadInt16x8(&inPt[v * 2 + 32]), PLoadInt16x8A(&currw[240])));
				cvals2 = PSADDW8(cvals2, PMULHW8(PLoadInt16x8(&inPt[v * 2 + 48]), PLoadInt16x8A(&currw[256])));
				cvals2 = PHSADDW8_4(cvals3, cvals2);

				PStoreUInt8x16(outPt, PADDUB16(PCONVI8x16_U(SI16ToI8x16(PADDW8(PSARW8(cvals, 5), valadd), PADDW8(PSARW8(cvals2, 5), valadd))), valadd2));

				currw += 272;
				outPt += 16;
			}
			inPt += sstep;
			outPt += dstep;
		}
	}
	else
	{
		Int16x8 valadd = PInt16x8SetA(-128);
		UInt8x8 valadd2 = PUInt8x8SetA(128);
		Int16x4 cvals5;

		dstep -= width << 2;
		width = width >> 1;
		while (height-- > 0)
		{
			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- > 0)
			{
				cvals4 = PInt16x4Clear();
				j = tap;
				while (j-- > 0)
				{
					cvals4 = PSADDW4(cvals4, PMULHW4(PLoadInt16x4(&inPt[currIndex[0] * 2 + 0]), PLoadInt16x4(currWeight)));
					currIndex++;
					currWeight += 4;
				}

				cvals5 = PInt16x4Clear();
				j = tap;
				while (j-- > 0)
				{
					cvals5 = PSADDW4(cvals5, PMULHW4(PLoadInt16x4(&inPt[currIndex[0] * 2 + 0]), PLoadInt16x4(currWeight)));
					currIndex++;
					currWeight += 4;
				}
				cvals = PMergeW4(cvals4, cvals5);
				PStoreUInt8x8(outPt, PADDUB8(PCONVI8x8_U(SI16ToI8x8(PADDW8(PSARW8(cvals, 5), valadd))), valadd2));
				outPt += 8;
			}
			inPt += sstep;
			outPt += dstep;
		}
	}
}

extern "C" void LanczosResizerH8_8_horizontal_filter8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
	OSInt i;
	OSInt j;
	OSInt *currIndex;
	Int16 *currWeight;
	UInt8 *currw;
	if ((width & 3) == 0 && tap == 8)
	{
		Int16x8 cvals;
		Int16x8 cvals2;
		Int16x8 cvals3;
		Int16x8 addval1 = PInt16x8SetA(-128);
		UInt8x16 addval2 = PUInt8x16SetA(128);
		UInt8x16 v;
		dstep -= width << 2;
		width = width >> 2;
		while (height-- > 0)
		{
			currw = (UInt8*)weight;
			i = width;
			while (i-- > 0)
			{
				cvals = PInt16x8Clear();
				v = PLoadUInt8x16(&inPt[(*(UInt32*)&currw[0]) * 2 + 0]);
				cvals = PSADDW8(cvals, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 2)), PLoadInt16x8A(&currw[16])));
				cvals = PSADDW8(cvals, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 2)), PLoadInt16x8A(&currw[32])));
				v = PLoadUInt8x16(&inPt[(*(UInt32*)&currw[0]) * 2 + 16]);
				cvals = PSADDW8(cvals, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 2)), PLoadInt16x8A(&currw[48])));
				cvals = PSADDW8(cvals, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 2)), PLoadInt16x8A(&currw[64])));

				cvals2 = PInt16x8Clear();
				v = PLoadUInt8x16(&inPt[(*(UInt32*)&currw[1]) * 2 + 0]);
				cvals2 = PSADDW8(cvals2, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 2)), PLoadInt16x8A(&currw[80])));
				cvals2 = PSADDW8(cvals2, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 2)), PLoadInt16x8A(&currw[96])));
				v = PLoadUInt8x16(&inPt[(*(UInt32*)&currw[1]) * 2 + 16]);
				cvals2 = PSADDW8(cvals2, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 2)), PLoadInt16x8A(&currw[112])));
				cvals2 = PSADDW8(cvals2, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 2)), PLoadInt16x8A(&currw[128])));

				cvals = PSARW8(PHSADDW8_4(cvals, cvals2), 5);
				
				cvals3 = PInt16x8Clear();
				v = PLoadUInt8x16(&inPt[(*(UInt32*)&currw[0]) * 2 + 0]);
				cvals3 = PSADDW8(cvals3, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 2)), PLoadInt16x8A(&currw[16])));
				cvals3 = PSADDW8(cvals3, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 2)), PLoadInt16x8A(&currw[32])));
				v = PLoadUInt8x16(&inPt[(*(UInt32*)&currw[0]) * 2 + 16]);
				cvals3 = PSADDW8(cvals3, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 2)), PLoadInt16x8A(&currw[48])));
				cvals3 = PSADDW8(cvals3, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 2)), PLoadInt16x8A(&currw[64])));

				cvals2 = PInt16x8Clear();
				v = PLoadUInt8x16(&inPt[(*(UInt32*)&currw[1]) * 2 + 0]);
				cvals2 = PSADDW8(cvals2, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 2)), PLoadInt16x8A(&currw[80])));
				cvals2 = PSADDW8(cvals2, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 2)), PLoadInt16x8A(&currw[96])));
				v = PLoadUInt8x16(&inPt[(*(UInt32*)&currw[1]) * 2 + 16]);
				cvals2 = PSADDW8(cvals2, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 2)), PLoadInt16x8A(&currw[112])));
				cvals2 = PSADDW8(cvals2, PMULHW8(PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 2)), PLoadInt16x8A(&currw[128])));

				cvals2 = PSARW8(PHSADDW8_4(cvals3, cvals2), 5);
				PStoreUInt8x16(outPt, PADDUB16(PCONVI8x16_U(SI16ToI8x16(PSADDW8(cvals, addval1), PSADDW8(cvals2, addval1))), addval2));

				currw += 272;
				outPt += 16;
			}
			inPt += sstep;
			outPt += dstep;
		}
	}
	else
	{
		Int16x4 cvals;
		Int16x4 addval1 = PInt16x4SetA(-128);
		UInt8x4 addval2 = PUInt8x4SetA(128);
		UInt8x4 v4;
		dstep -= width << 2;
		while (height-- > 0)
		{
			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- > 0)
			{
				cvals = PInt16x4Clear();
				j = tap;
				while (j-- > 0)
				{
					v4 = PLoadUInt8x4(&inPt[currIndex[0] * 2]);
					cvals = PADDW4(cvals, PMULHW4(PCONVU16x4_I(PSHRW4(PUNPCKUBW4(v4, v4), 2)), PLoadInt16x4(currWeight)));
					currIndex++;
					currWeight += 4;
				}
				PStoreUInt8x4(outPt, PADDUB4(PCONVI8x4_U(SI16ToI8x4(PADDW4(PSARW4(cvals, 5), addval1))), addval2));
				outPt += 4;
			}
			inPt += sstep;
			outPt += dstep;
		}
	}
}

extern "C" void LanczosResizerH8_8_vertical_filter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
	OSInt i;
	OSInt j;
	OSInt *currIndex;
	Int16 *currWeight;
	UInt8 *currIn;
	dstep -= width << 3;

	if ((width & 3) != 0 || (((OSInt)outPt) & 15) != 0 || (dstep & 15) != 0)
	{
		Int16x4 cvals;
		UInt8x4 v;
		while (height-- > 0)
		{
			currIn = inPt;
			i = width;
			while (i-- > 0)
			{
				currIndex = index;
				currWeight = (Int16*)weight;
				cvals = PInt16x4Clear();
				j = tap;
				while (j-- > 0)
				{
					v = PLoadUInt8x4(&currIn[currIndex[0]]);
					cvals = PSADDW4(cvals, PMULHW4(PLoadInt16x4(currWeight), PCONVU16x4_I(PSHRW4(PUNPCKUBW4(v, v), 1))));
					currIndex++;
					currWeight += 4;
				}
				PStoreInt16x4(outPt, cvals);
				currIn += 4;
				outPt += 8;
			}
			index += tap;
			weight += tap;
			outPt += dstep;
		}
	}
	else if (tap == 6)
	{
		Int16x8 cvals;
		Int16x8 cvals2;
		UInt8x16 v;
		Int16x8 w1;
		Int16x8 w2;
		Int16x8 w3;
		Int16x8 wv2;
		width = width >> 2;
		while (height-- > 0)
		{
			currIn = inPt;
			w1 = PLoadInt16x8A(weight);
			w2 = PLoadInt16x8A(&weight[2]);
			w3 = PLoadInt16x8A(&weight[4]);
			i = width;
			while (i-- > 0)
			{
				currIndex = index;
				cvals = PInt16x8Clear();
				cvals2 = PInt16x8Clear();
				v = PLoadUInt8x16(&currIn[currIndex[0]]);
				wv2 = PMergeLW4(w1, w1);
				cvals = PSADDW8(cvals, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 1))));
				cvals2 = PSADDW8(cvals2, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 1))));
				v = PLoadUInt8x16(&currIn[currIndex[1]]);
				wv2 = PMergeHW4(w1, w1);
				cvals = PSADDW8(cvals, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 1))));
				cvals2 = PSADDW8(cvals2, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 1))));

				v = PLoadUInt8x16(&currIn[currIndex[2]]);
				wv2 = PMergeLW4(w2, w2);
				cvals = PSADDW8(cvals, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 1))));
				cvals2 = PSADDW8(cvals2, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 1))));
				v = PLoadUInt8x16(&currIn[currIndex[3]]);
				wv2 = PMergeHW4(w2, w2);
				cvals = PSADDW8(cvals, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 1))));
				cvals2 = PSADDW8(cvals2, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 1))));

				v = PLoadUInt8x16(&currIn[currIndex[4]]);
				wv2 = PMergeLW4(w3, w3);
				cvals = PSADDW8(cvals, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 1))));
				cvals2 = PSADDW8(cvals2, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 1))));
				v = PLoadUInt8x16(&currIn[currIndex[5]]);
				wv2 = PMergeHW4(w3, w3);
				cvals = PSADDW8(cvals, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 1))));
				cvals2 = PSADDW8(cvals2, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 1))));

				PStoreInt16x8NC(outPt, cvals);
				PStoreInt16x8NC(&outPt[16], cvals2);
				currIn += 16;
				outPt += 32;
			}
			index += tap;
			weight += tap;
			outPt += dstep;
		}
	}
	else
	{
		Int16x8 cvals;
		Int16x8 cvals2;
		UInt8x16 v;
		Int16x4 wv;
		Int16x8 wv2;
		width = width >> 2;
		while (height-- > 0)
		{
			currIn = inPt;
			i = width;
			while (i-- > 0)
			{
				currIndex = index;
				currWeight = (Int16*)weight;
				cvals = PInt16x8Clear();
				cvals2 = PInt16x8Clear();
				j = tap;
				while (j-- > 0)
				{
					v = PLoadUInt8x16(&currIn[currIndex[0]]);
					wv = PLoadInt16x4(currWeight);
					wv2 = PMergeW4(wv, wv);
					cvals = PSADDW8(cvals, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKLUBW8(v, v), 1))));
					cvals2 = PSADDW8(cvals2, PMULHW8(wv2, PCONVU16x8_I(PSHRW8(PUNPCKHUBW8(v, v), 1))));
					currIndex++;
					currWeight += 4;
				}
				PStoreInt16x8NC(outPt, cvals);
				PStoreInt16x8NC(&outPt[16], cvals2);
				currIn += 16;
				outPt += 32;
			}
			index += tap;
			weight += tap;
			outPt += dstep;
		}
	}
}

extern "C" void LanczosResizerH8_8_expand(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep)
{
	OSInt i;
	sstep -= width << 2;
	dstep -= width << 3;
	UInt8x4 v;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			v = PLoadUInt8x4(inPt);
			PStoreUInt16x4(outPt, PSHRW4(PUNPCKUBW4(v, v), 2));
			inPt += 4;
			outPt += 8;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerH8_8_collapse(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep)
{
	OSInt i;
	sstep -= width << 3;
	dstep -= width << 2;
	Int16x4 addval1 = PInt16x4SetA(-128);
	UInt8x4 addval2 = PUInt8x4SetA(128);
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			PStoreUInt8x4(outPt, PADDUB4(PCONVI8x4_U(SI16ToI8x4(PADDW4(PSARW4(PLoadInt16x4(inPt), 6), addval1))), addval2));
			inPt += 8;
			outPt += 4;
		}
		inPt += sstep;
		outPt += dstep;
	}
}
