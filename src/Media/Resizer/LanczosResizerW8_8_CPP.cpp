#include "Stdafx.h"
#include "Core/ByteTool_C.h"

extern "C" void LanczosResizerW8_8_horizontal_filter(const UInt8 *inPt, UInt8 *outPt,UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int16 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable, UIntOS swidth, UInt8 *tmpbuff)
{
	const UInt8 *currIn;
	UInt8 *tmpPtr;
	UIntOS i;
	UIntOS j;
	Int16 cvals;
	Int32 cvals2;
	IntOS *currIndex;
	Int16 *currWeight;
	dstep -= (IntOS)width << 1;

	while (height-- != 0)
	{
		currIn = inPt;
		tmpPtr = tmpbuff;
		i = swidth;
		while (i-- != 0)
		{
			cvals = *(Int16*)&rgbTable[65536 + currIn[0] * 2];
			*(Int16*)tmpPtr = cvals;
			currIn += 1;
			tmpPtr += 2;
		}

		currIndex = index;
		currWeight = (Int16*)weight;
		i = width;
		while (i-- > 0)
		{
			cvals2 = 0;
			j = tap;
			while (j-- > 0)
			{
				cvals2 += currWeight[0] * *(Int16*)&tmpbuff[currIndex[0]];
				currWeight += 1;
				currIndex += 1;
			}
			cvals2 >>= 14;
			if (cvals2 < -32768)
				cvals2 = -32768;
			else if (cvals2 >= 32768)
				cvals2 = 32767;
			*(Int16*)outPt = (Int16)cvals2;
			outPt += 2;
		}

		outPt += dstep;
		inPt += sstep;
	}	
}

extern "C" void LanczosResizerW8_8_vertical_filter(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int16 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
{
	UIntOS i;
	UIntOS j;
	const UInt8 *currIn;
	IntOS *currIndex;
	Int16 *currWeight;
	Int32 cvals;
	UInt16 cvals2;
	dstep -= (IntOS)width;
	while (height-- != 0)
	{
		currIn = inPt;

		i = width;
		while (i-- != 0)
		{
			currIndex = index;
			currWeight = weight;
			cvals = 0;
			j = tap;
			while (j-- != 0)
			{
				cvals += currWeight[0] * *(Int16*)&currIn[currIndex[0]];
				currWeight += 1;
				currIndex += 1;
			}
			cvals >>= 15;
			if (cvals < -32768)
				cvals = -32768;
			else if (cvals >= 32768)
				cvals = 32767;
			cvals2 = (UInt16)(Int16)cvals;
			outPt[0] = rgbTable[cvals2];
			currIn += 2;
			outPt += 1;
		}
		index += tap;
		weight += tap;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerW8_8_expand(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
{
	UIntOS i;
	sstep -= (IntOS)width;
	dstep -= (IntOS)width * 2;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			*(UInt16*)outPt = *(UInt16*)&rgbTable[65536 + inPt[0] * 2];
			inPt++;
			outPt += 2;
		}
		inPt += sstep;
		outPt += dstep;
	}	
}

extern "C" void LanczosResizerW8_8_collapse(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
{
	UIntOS i;
	UInt16 v;
	sstep -= (IntOS)width * 2;
	dstep -= (IntOS)width;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			v = ReadNUInt16(inPt);
			*outPt++ = rgbTable[v];
			inPt += 2;
		}
		inPt += sstep;
		outPt += dstep;
	}	
}

extern "C" void LanczosResizerW8_8_imgcopy(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable)
{
	UIntOS i;
	UInt16 v;
	sstep -= (IntOS)width;
	dstep -= (IntOS)width;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			v = *(UInt16*)&rgbTable[65536 + inPt[0] * 2];
			*outPt++ = rgbTable[v];
			inPt++;
		}
		inPt += sstep;
		outPt += dstep;
	}	
}
