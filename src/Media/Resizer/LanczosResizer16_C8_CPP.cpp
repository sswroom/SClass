#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"
#include "Data/ByteTool.h"

extern "C" void LanczosResizer16_C8_horizontal_filter_pa(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, OSInt swidth, UInt8 *tmpbuff)
{
	OSInt i;
	OSInt j;
	UInt8 *currIn;
	UInt8 *tmpPtr;
	OSInt *currIndex;
	Int16 *currWeight;
	Int16x4 cvals;
	Int32x4 cvald;
	dstep -= width << 3;

	while (height-- > 0)
	{
		currIn = inPt;
		tmpPtr = tmpbuff;
		i = swidth;
		while (i-- > 0)
		{
			cvals = PInt16x4Clear();
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&currIn[0]) * 8 + 1310720]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&currIn[2]) * 8 + 786432]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&currIn[4]) * 8 + 262144]));
			cvals = PMULM2HW4(cvals, PInt16x4SetA((Int16)(ReadUInt16(&currIn[6]) >> 1)));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&currIn[6]) * 8 + 1835008]));
			PStoreInt16x4(tmpPtr, cvals);
			currIn += 8;
			tmpPtr += 8;
		}

		currIndex = index;
		currWeight = (Int16*)weight;
		i = width;
		while (i-- > 0)
		{
			cvald = PInt32x4Clear();
			j = tap >> 1;
			while (j-- > 0)
			{
				cvald = PADDD4(cvald, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
				currWeight += 8;
				currIndex += 2;
			}
			PStoreInt16x4(outPt, PSARSDW4(cvald, 15));
			outPt += 8;
		}
		outPt += dstep;
		inPt += sstep;
	}
}

extern "C" void LanczosResizer16_C8_horizontal_filter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, OSInt swidth, UInt8 *tmpbuff)
{
	OSInt i;
	OSInt j;
	UInt8 *currIn;
	UInt8 *tmpPtr;
	OSInt *currIndex;
	Int16 *currWeight;
	Int16x4 cvals;
	Int32x4 cvald;
	dstep -= width << 3;

	while (height-- > 0)
	{
		currIn = inPt;
		tmpPtr = tmpbuff;
		i = swidth;
		while (i-- > 0)
		{
			cvals = PInt16x4Clear();
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&currIn[0]) * 8 + 1310720]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&currIn[2]) * 8 + 786432]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&currIn[4]) * 8 + 262144]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&currIn[6]) * 8 + 1835008]));
			PStoreInt16x4(tmpPtr, cvals);
			currIn += 8;
			tmpPtr += 8;
		}

		currIndex = index;
		currWeight = (Int16*)weight;
		i = width;
		while (i-- > 0)
		{
			cvald = PInt32x4Clear();
			j = tap >> 1;
			while (j-- > 0)
			{
				cvald = PADDD4(cvald, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
				currWeight += 8;
				currIndex += 2;
			}
			PStoreInt16x4(outPt, PSARSDW4(cvald, 15));
			outPt += 8;
		}
		outPt += dstep;
		inPt += sstep;
	}
}

extern "C" void LanczosResizer16_C8_vertical_filter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
{
	OSInt i;
	OSInt j;
	UInt8 *currIn;
	OSInt *currIndex;
	Int16 *currWeight;
	Int32x4 cvald;
	UInt16x4 cvalw;
	dstep -= width << 2;

	while (height-- > 0)
	{
		currIn = inPt;

		i = width;
		while (i-- > 0)
		{
			currIndex = index;
			currWeight = (Int16*)weight;
			cvald = PInt32x4Clear();
			j = tap >> 1;
			while (j-- > 0)
			{
				cvald = PADDD4(cvald, PMADDWD(PUNPCKWW4(PLoadInt16x4(&currIn[currIndex[0]]), PLoadInt16x4(&currIn[currIndex[1]])), PLoadInt16x8A(currWeight)));
				currWeight += 8;
				currIndex += 2;
			}
			cvalw = PCONVI16x4_U(PSARSDW4(cvald, 15));
			outPt[0] = rgbTable[0 + PEXTUW4(cvalw, 0)];
			outPt[1] = rgbTable[65536 + PEXTUW4(cvalw, 1)];
			outPt[2] = rgbTable[131072 + PEXTUW4(cvalw, 2)];
			outPt[3] = rgbTable[196608 + PEXTUW4(cvalw, 3)];
			currIn += 8;
			outPt += 4;
		}
		index += tap;
		weight += tap;
		outPt += dstep;
	}
}

extern "C" void LanczosResizer16_C8_expand(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
{
	OSInt i;
	Int16x4 cvals;
	sstep -= width << 3;
	dstep -= width << 3;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbTable[1310720 + ReadUInt16(&inPt[0]) * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[786432 + ReadUInt16(&inPt[2]) * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[262144 + ReadUInt16(&inPt[4]) * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[1835008 + ReadUInt16(&inPt[6]) * 8]));
			PStoreInt16x4(outPt, cvals);
			inPt += 8;
			outPt += 8;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizer16_C8_expand_pa(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
{
	OSInt i;
	Int16x4 cvals;
	sstep -= width << 3;
	dstep -= width << 3;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbTable[1310720 + ReadUInt16(&inPt[0]) * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[786432 + ReadUInt16(&inPt[2]) * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[262144 + ReadUInt16(&inPt[4]) * 8]));
			cvals = PMULM2HW4(cvals, PInt16x4SetA((Int16)(ReadUInt16(&inPt[6]) >> 1)));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[1835008 + ReadUInt16(&inPt[6]) * 8]));
			PStoreInt16x4(outPt, cvals);
			inPt += 8;
			outPt += 8;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizer16_C8_collapse(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
{
	OSInt i;
	sstep -= width << 3;
	dstep -= width << 2;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			outPt[0] = rgbTable[0 + ReadUInt16(&inPt[0])];
			outPt[1] = rgbTable[65536 + ReadUInt16(&inPt[2])];
			outPt[2] = rgbTable[131072 + ReadUInt16(&inPt[4])];
			outPt[3] = rgbTable[196608 + ReadUInt16(&inPt[6])];
			inPt += 8;
			outPt += 4;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizer16_C8_imgcopy(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
{
	OSInt i;
	Int16x4 cvals;
	UInt16x4 ucvals;
	sstep -= width << 3;
	dstep -= width << 2;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbTable[1310720 + ReadUInt16(&inPt[0]) * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[786432 + ReadUInt16(&inPt[2]) * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[262144 + ReadUInt16(&inPt[4]) * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[1835008 + ReadUInt16(&inPt[6]) * 8]));
			ucvals = PCONVI16x4_U(cvals);
			outPt[0] = rgbTable[0 + PEXTUW4(ucvals, 0)];
			outPt[1] = rgbTable[65536 + PEXTUW4(ucvals, 1)];
			outPt[2] = rgbTable[131072 + PEXTUW4(ucvals, 2)];
			outPt[3] = inPt[7];
			inPt += 8;
			outPt += 4;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizer16_C8_imgcopy_pa(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
{
	OSInt i;
	Int16x4 cvals;
	UInt16x4 ucvals;
	sstep -= width << 3;
	dstep -= width << 2;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbTable[1310720 + ReadUInt16(&inPt[0]) * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[786432 + ReadUInt16(&inPt[2]) * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[262144 + ReadUInt16(&inPt[4]) * 8]));
			cvals = PMULM2HW4(cvals, PInt16x4SetA((Int16)(ReadUInt16(&inPt[6]) >> 1)));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[1835008 + ReadUInt16(&inPt[6]) * 8]));
			ucvals = PCONVI16x4_U(cvals);
			outPt[0] = rgbTable[0 + PEXTUW4(ucvals, 0)];
			outPt[1] = rgbTable[65536 + PEXTUW4(ucvals, 1)];
			outPt[2] = rgbTable[131072 + PEXTUW4(ucvals, 2)];
			outPt[3] = inPt[7];
			inPt += 8;
			outPt += 4;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

