#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"

extern "C"
{

void LanczosResizerLR_C16_horizontal_filter(UInt8 *inPt, UInt8 *outPt,OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
{
	OSInt i;
	OSInt j;
	OSInt *currIndex;
	Int16 *currWeight;
	Int32 bval;
	Int32 gval;
	Int32 rval;
	Int32 aval;
	dstep -= width * 8;

	tap = tap >> 1;
	while (height-- > 0)
	{
		currIndex = index;
		currWeight = (Int16*)weight;
		i = width;
		while (i-- > 0)
		{
			bval = 16384;
			gval = 16384;
			rval = 16384;
			aval = 16384;
			j = tap;
			while (j-- > 0)
			{
				bval += ReadInt16(&inPt[currIndex[0] + 0]) * currWeight[0] + ReadInt16(&inPt[currIndex[1] + 0]) * currWeight[1];
				gval += ReadInt16(&inPt[currIndex[0] + 2]) * currWeight[2] + ReadInt16(&inPt[currIndex[1] + 2]) * currWeight[3];
				rval += ReadInt16(&inPt[currIndex[0] + 4]) * currWeight[4] + ReadInt16(&inPt[currIndex[1] + 4]) * currWeight[5];
				aval += ReadInt16(&inPt[currIndex[0] + 6]) * currWeight[6] + ReadInt16(&inPt[currIndex[1] + 6]) * currWeight[7];
				currIndex += 2;
				currWeight += 8;
			}
			bval = bval >> 15;
			gval = gval >> 15;
			rval = rval >> 15;
			aval = aval >> 15;
			if (bval < -32768)
				bval = -32768;
			else if (bval > 32767)
				bval = 32767;
			if (gval < -32768)
				gval = -32768;
			else if (gval > 32767)
				gval = 32767;
			if (rval < -32768)
				rval = -32768;
			else if (rval > 32767)
				rval = 32767;
			if (aval < -32768)
				aval = -32768;
			else if (aval > 32767)
				aval = 32767;
			WriteInt16(&outPt[0], bval);
			WriteInt16(&outPt[2], gval);
			WriteInt16(&outPt[4], rval);
			WriteInt16(&outPt[6], aval);
			outPt += 8;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

void LanczosResizerLR_C16_vertical_filter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
{
	OSInt i;
	OSInt j;
	UInt8 *currIn;
	OSInt *currIndex;
	Int16 *currWeight;
	Int32 bval;
	Int32 gval;
	Int32 rval;
	Int32 aval;
	UInt32 v;
	dstep -= width * 2;
	tap = tap >> 1;
	while (height-- > 0)
	{
		currIn = inPt;
		i = width;
		while (i-- > 0)
		{
			bval = 16384;
			gval = 16384;
			rval = 16384;
			aval = 16384;
			currIndex = index;
			currWeight = (Int16*)weight;
			j = tap;
			while (j-- > 0)
			{
				bval += ReadInt16(&currIn[currIndex[0] + 0]) * currWeight[0] + ReadInt16(&currIn[currIndex[1] + 0]) * currWeight[1];
				gval += ReadInt16(&currIn[currIndex[0] + 2]) * currWeight[2] + ReadInt16(&currIn[currIndex[1] + 2]) * currWeight[3];
				rval += ReadInt16(&currIn[currIndex[0] + 4]) * currWeight[4] + ReadInt16(&currIn[currIndex[1] + 4]) * currWeight[5];
				aval += ReadInt16(&currIn[currIndex[0] + 6]) * currWeight[6] + ReadInt16(&currIn[currIndex[1] + 6]) * currWeight[7];
				currIndex += 2;
				currWeight += 8;
			}
			bval = bval >> 15;
			gval = gval >> 15;
			rval = rval >> 15;
			aval = aval >> 15;
			if (bval < -32768)
				bval = 32768;
			else if (bval > 32767)
				bval = 32767;
			else
				bval = bval & 65535;
			if (gval < -32768)
				gval = 32768;
			else if (gval > 32767)
				gval = 32767;
			else
				gval = gval & 65535;
			if (rval < -32768)
				rval = 32768;
			else if (rval > 32767)
				rval = 32767;
			else
				rval = rval & 65535;
			v  = ReadUInt16(&rgbTable[bval * 2 + 0]);
			v |= ReadUInt16(&rgbTable[gval * 2 + 131072]);
			v |= ReadUInt16(&rgbTable[rval * 2 + 262144]);
			WriteInt16(&outPt[0], v);
			currIn += 8;
			outPt += 2;
		}
		weight += tap << 1;
		index += tap << 1;
		outPt += dstep;
	}
}

void LanczosResizerLR_C16_collapse(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable)
{
	OSInt i;
	UInt16 v;
	sstep = sstep - width * 8;
	dstep = dstep - width * 2;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			v  = ReadUInt16(&rgbTable[ReadUInt16(&inPt[0]) * 2 + 0]);
			v |= ReadUInt16(&rgbTable[ReadUInt16(&inPt[2]) * 2 + 131072]);
			v |= ReadUInt16(&rgbTable[ReadUInt16(&inPt[4]) * 2 + 262144]);
			WriteInt16(outPt, v);
			inPt += 8;
			outPt += 2;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

}
