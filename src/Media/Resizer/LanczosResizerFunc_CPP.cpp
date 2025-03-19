#include "Stdafx.h"
#include "SIMD.h"
#include "Data/ByteTool.h"

extern "C" void LanczosResizerFunc_HorizontalFilterB8G8R8A8PA(UInt8 *inPt, UInt8 *outPt,OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbaTable, OSInt swidth, UInt8 *tmpbuff)
{
	OSInt i;
	OSInt j;
	UInt8 *currIn;
	UInt8 *tmpPtr;
	OSInt *currIndex;
	Int16 *currWeight;
	Int16x8 tmpVal1;
	Int16x8 tmpVal2;

	if ((width & 1) != 0)
	{
		Int16x4 cvals;
		Int16x8 cvals2;
		Int32x4 cvals2b;
		dstep -= width << 3;
		tap >>= 1;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			if (i & 1)
			{
				cvals = PInt16x4Clear();
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[0] * 8 + 4096]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[1] * 8 + 2048]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[2] * 8 + 0]));
				cvals = PMULM2HW4(cvals, PInt16x4SetA((Int16)((currIn[3] << 7) | (currIn[3] >> 1))));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[3] * 8 + 6144]));
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 4;
				tmpPtr += 8;
			}
			i >>= 1;
			if (i != 0)
			{
				while (i-- != 0)
				{
					cvals2 = PInt16x8Clear();
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[4] * 8 + 4096], &rgbaTable[currIn[0] * 8 + 4096]));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[5] * 8 + 2048], &rgbaTable[currIn[1] * 8 + 2048]));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[6] * 8 + 0], &rgbaTable[currIn[2] * 8 + 0]));
					cvals2 = PMULM2HW8(cvals2, PMergeW4(PInt16x4SetA((Int16)((currIn[3] << 7) | (currIn[3] >> 1))), PInt16x4SetA((Int16)((currIn[7] << 7) | (currIn[7] >> 1)))));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[7] * 8 + 6144], &rgbaTable[currIn[3] * 8 + 6144]));
					PStoreInt16x8(tmpPtr, cvals2);
					currIn += 8;
					tmpPtr += 16;
				}
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- != 0)
			{
				cvals2b = PInt32x4Clear();
				j = tap;
				while (j-- != 0)
				{
					cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}
				PStoreInt16x4(outPt, PSARSDW4(cvals2b, 14));
				outPt += 8;
			}

			outPt += dstep;
			inPt += sstep;
		}
	}
	else if (tap == 6)
	{
		Int16x4 cvals;
		Int16x8 cvals2;
		Int32x4 cvals2b;
		dstep -= width << 3;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			if (i & 1)
			{
				cvals = PInt16x4Clear();
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[0] * 8 + 4096]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[1] * 8 + 2048]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[2] * 8 + 0]));
				cvals = PMULM2HW4(cvals, PInt16x4SetA((Int16)((currIn[3] << 7) | (currIn[3] >> 1))));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[3] * 8 + 6144]));
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 4;
				tmpPtr += 8;
			}
			i >>= 1;
			if (i != 0)
			{
				while (i-- != 0)
				{
					cvals2 = PInt16x8Clear();
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[4] * 8 + 4096], &rgbaTable[currIn[0] * 8 + 4096]));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[5] * 8 + 2048], &rgbaTable[currIn[1] * 8 + 2048]));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[6] * 8 + 0], &rgbaTable[currIn[2] * 8 + 0]));
					cvals2 = PMULM2HW8(cvals2, PMergeW4(PInt16x4SetA((Int16)((currIn[3] << 7) | (currIn[3] >> 1))), PInt16x4SetA((Int16)((currIn[7] << 7) | (currIn[7] >> 1)))));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[7] * 8 + 6144], &rgbaTable[currIn[3] * 8 + 6144]));
					PStoreInt16x8(tmpPtr, cvals2);
					currIn += 8;
					tmpPtr += 16;
				}
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- != 0)
			{
				cvals2b = PInt32x4Clear();
				tmpVal1 = PLoadInt16x8(&tmpbuff[currIndex[0]]);
				tmpVal2 = PLoadInt16x8(&tmpbuff[currIndex[1]]);
				cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[0])));
				cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[8])));
				cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[2]]), PLoadInt16x4(&tmpbuff[currIndex[2] + 8])), PLoadInt16x8A(&currWeight[16])));
				currWeight += 24;
				currIndex += 3;
				PStoreInt16x4(outPt, PSARSDW4(cvals2b, 14));
				outPt += 8;
			}
			outPt += dstep;
			inPt += sstep;
		}
	}
	else if (swidth & 1)
	{
		Int16x4 cvals;
		Int32x4 cvals2;
		dstep -= width << 3;
		tap >>= 1;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			while (i-- != 0)
			{
				cvals = PInt16x4Clear();
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[0] * 8 + 4096]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[1] * 8 + 2048]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[2] * 8 + 0]));
				cvals = PMULM2HW4(cvals, PInt16x4SetA((Int16)((currIn[3] << 7) | (currIn[3] >> 1))));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[3] * 8 + 6144]));
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 4;
				tmpPtr += 8;
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- > 0)
			{
				cvals2 = PInt32x4Clear();
				j = tap;
				while (j-- > 0)
				{
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}
				PStoreInt16x4(outPt, PSARSDW4(cvals2, 14));
				outPt += 8;
			}

			outPt += dstep;
			inPt += sstep;
		}
	}
	else
	{
		Int16x8 cvals;
		Int32x4 cvals2;
		Int32x4 cvals2b;
		dstep -= width << 3;
		swidth >>= 1;
		width >>= 1;
		tap >>= 1;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			while (i-- != 0)
			{
				cvals = PInt16x8Clear();
				cvals = PSADDW8(cvals, PMLoadInt16x4(&rgbaTable[currIn[4] * 8 + 4096], &rgbaTable[currIn[0] * 8 + 4096]));
				cvals = PSADDW8(cvals, PMLoadInt16x4(&rgbaTable[currIn[5] * 8 + 2048], &rgbaTable[currIn[1] * 8 + 2048]));
				cvals = PSADDW8(cvals, PMLoadInt16x4(&rgbaTable[currIn[6] * 8 + 0], &rgbaTable[currIn[2] * 8 + 0]));
				cvals = PMULM2HW8(cvals, PMergeW4(PInt16x4SetA((Int16)((currIn[3] << 7) | (currIn[3] >> 1))), PInt16x4SetA((Int16)((currIn[7] << 7) | (currIn[7] >> 1)))));
				cvals = PSADDW8(cvals, PMLoadInt16x4(&rgbaTable[currIn[7] * 8 + 6144], &rgbaTable[currIn[3] * 8 + 6144]));
				PStoreInt16x8A(tmpPtr, cvals);
				currIn += 8;
				tmpPtr += 16;
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- != 0)
			{
				cvals2 = PInt32x4Clear();
				cvals2b = PInt32x4Clear();
				j = tap;
				while (j-- != 0)
				{
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}

				j = tap;
				while (j-- != 0)
				{
					cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}
				PStoreInt16x8A(outPt, PMergeSARDW4(cvals2, cvals2b, 14));
				outPt += 16;
			}

			outPt += dstep;
			inPt += sstep;
		}
	}
}

extern "C" void LanczosResizerFunc_HorizontalFilterB8G8R8A8(UInt8 *inPt, UInt8 *outPt,OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbaTable, OSInt swidth, UInt8 *tmpbuff)
{
	OSInt i;
	OSInt j;
	UInt8 *currIn;
	UInt8 *tmpPtr;
	OSInt *currIndex;
	Int16 *currWeight;
	Int16x4 cvals;
	Int32x4 cvals2;
	Int16x8 tmpVal1;
	Int16x8 tmpVal2;
	if (tap == 6 && (width & 1) == 0)
	{
		dstep -= width << 3;

		while (height-- > 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			while (i-- > 0)
			{
				cvals = PInt16x4Clear();
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[0] * 8 + 4096]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[1] * 8 + 2048]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[2] * 8 + 0]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[3] * 8 + 6144]));
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 4;
				tmpPtr += 8;
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- > 0)
			{
				cvals2 = PInt32x4Clear();
				tmpVal1 = PLoadInt16x8(&tmpbuff[currIndex[0]]);
				tmpVal2 = PLoadInt16x8(&tmpbuff[currIndex[1]]);
				cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[0])));
				cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[8])));
				cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[2]]), PLoadInt16x4(&tmpbuff[currIndex[2] + 8])), PLoadInt16x8A(&currWeight[16])));
				currWeight += 24;
				currIndex += 3;
				PStoreInt16x4(outPt, PSARSDW4(cvals2, 14));
				outPt += 8;
			}
			outPt += dstep;
			inPt += sstep;
		}
	}
	else
	{
		dstep -= width << 3;

		while (height-- > 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			while (i-- > 0)
			{
				cvals = PInt16x4Clear();
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[0] * 8 + 4096]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[1] * 8 + 2048]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[2] * 8 + 0]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[3] * 8 + 6144]));
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 4;
				tmpPtr += 8;
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- > 0)
			{
				cvals2 = PInt32x4Clear();
				j = tap >> 1;
				while (j-- > 0)
				{
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}
				PStoreInt16x4(outPt, PSARSDW4(cvals2, 14));
				outPt += 8;
			}

			outPt += dstep;
			inPt += sstep;
		}
	}
}

extern "C" void LanczosResizerFunc_HorizontalFilterB8G8R8(UInt8 *inPt, UInt8 *outPt,OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbaTable, OSInt swidth, UInt8 *tmpbuff)
{
	OSInt i;
	OSInt j;
	UInt8 *currIn;
	UInt8 *tmpPtr;
	OSInt *currIndex;
	Int16 *currWeight;
	Int16x8 tmpVal1;
	Int16x8 tmpVal2;

	if ((width & 1) != 0)
	{
		Int16x4 cvals;
		Int16x8 cvals2;
		Int32x4 cvals2b;
		dstep -= width << 3;
		tap >>= 1;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			if (i & 1)
			{
				cvals = PInt16x4Clear();
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[0] * 8 + 4096]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[1] * 8 + 2048]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[2] * 8 + 0]));
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 3;
				tmpPtr += 8;
			}
			i >>= 1;
			if (i != 0)
			{
				while (i-- != 0)
				{
					cvals2 = PInt16x8Clear();
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[3] * 8 + 4096], &rgbaTable[currIn[0] * 8 + 4096]));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[4] * 8 + 2048], &rgbaTable[currIn[1] * 8 + 2048]));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[5] * 8 + 0], &rgbaTable[currIn[2] * 8 + 0]));
					PStoreInt16x8(tmpPtr, cvals2);
					currIn += 6;
					tmpPtr += 16;
				}
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- != 0)
			{
				cvals2b = PInt32x4Clear();
				j = tap;
				while (j-- != 0)
				{
					cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}
				PStoreInt16x4(outPt, PSARSDW4(cvals2b, 14));
				outPt += 8;
			}

			outPt += dstep;
			inPt += sstep;
		}
	}
	else if (tap == 6)
	{
		Int16x4 cvals;
		Int16x8 cvals2;
		Int32x4 cvals2b;
		dstep -= width << 3;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			if (i & 1)
			{
				cvals = PInt16x4Clear();
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[0] * 8 + 4096]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[1] * 8 + 2048]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[2] * 8 + 0]));
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 3;
				tmpPtr += 8;
			}
			i >>= 1;
			if (i != 0)
			{
				while (i-- != 0)
				{
					cvals2 = PInt16x8Clear();
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[3] * 8 + 4096], &rgbaTable[currIn[0] * 8 + 4096]));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[4] * 8 + 2048], &rgbaTable[currIn[1] * 8 + 2048]));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgbaTable[currIn[5] * 8 + 0], &rgbaTable[currIn[2] * 8 + 0]));
					PStoreInt16x8(tmpPtr, cvals2);
					currIn += 6;
					tmpPtr += 16;
				}
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- != 0)
			{
				cvals2b = PInt32x4Clear();
				tmpVal1 = PLoadInt16x8(&tmpbuff[currIndex[0]]);
				tmpVal2 = PLoadInt16x8(&tmpbuff[currIndex[1]]);
				cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[0])));
				cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[8])));
				cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[2]]), PLoadInt16x4(&tmpbuff[currIndex[2] + 8])), PLoadInt16x8A(&currWeight[16])));
				currWeight += 24;
				currIndex += 3;
				PStoreInt16x4(outPt, PSARSDW4(cvals2b, 14));
				outPt += 8;
			}
			outPt += dstep;
			inPt += sstep;
		}
	}
	else if (swidth & 1)
	{
		Int16x4 cvals;
		Int32x4 cvals2;
		dstep -= width << 3;
		tap >>= 1;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			while (i-- != 0)
			{
				cvals = PInt16x4Clear();
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[0] * 8 + 4096]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[1] * 8 + 2048]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[currIn[2] * 8 + 0]));
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 3;
				tmpPtr += 8;
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- > 0)
			{
				cvals2 = PInt32x4Clear();
				j = tap;
				while (j-- > 0)
				{
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}
				PStoreInt16x4(outPt, PSARSDW4(cvals2, 14));
				outPt += 8;
			}

			outPt += dstep;
			inPt += sstep;
		}
	}
	else
	{
		Int16x8 cvals;
		Int32x4 cvals2;
		Int32x4 cvals2b;
		dstep -= width << 3;
		swidth >>= 1;
		width >>= 1;
		tap >>= 1;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			while (i-- != 0)
			{
				cvals = PInt16x8Clear();
				cvals = PSADDW8(cvals, PMLoadInt16x4(&rgbaTable[currIn[3] * 8 + 4096], &rgbaTable[currIn[0] * 8 + 4096]));
				cvals = PSADDW8(cvals, PMLoadInt16x4(&rgbaTable[currIn[4] * 8 + 2048], &rgbaTable[currIn[1] * 8 + 2048]));
				cvals = PSADDW8(cvals, PMLoadInt16x4(&rgbaTable[currIn[5] * 8 + 0], &rgbaTable[currIn[2] * 8 + 0]));
				PStoreInt16x8A(tmpPtr, cvals);
				currIn += 6;
				tmpPtr += 16;
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- != 0)
			{
				cvals2 = PInt32x4Clear();
				cvals2b = PInt32x4Clear();
				j = tap;
				while (j-- != 0)
				{
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}

				j = tap;
				while (j-- != 0)
				{
					cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}
				PStoreInt16x8A(outPt, PMergeSARDW4(cvals2, cvals2b, 14));
				outPt += 16;
			}

			outPt += dstep;
			inPt += sstep;
		}
	}
}

extern "C" void LanczosResizerFunc_HorizontalFilterR16G16B16(UInt8 *inPt, UInt8 *outPt,OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgba16Table, OSInt swidth, UInt8 *tmpbuff)
{
	OSInt i;
	OSInt j;
	UInt8 *currIn;
	UInt8 *tmpPtr;
	OSInt *currIndex;
	Int16 *currWeight;
	Int16x8 tmpVal1;
	Int16x8 tmpVal2;

	if ((width & 1) != 0)
	{
		Int16x4 cvals;
		Int16x8 cvals2;
		Int32x4 cvals2b;
		dstep -= width << 3;
		tap >>= 1;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			if (i & 1)
			{
				cvals = PInt16x4Clear();
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[0]) * 8 + 0]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[2]) * 8 + 524288]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[4]) * 8 + 1048576]));
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 6;
				tmpPtr += 8;
			}
			i >>= 1;
			if (i != 0)
			{
				while (i-- != 0)
				{
					cvals2 = PInt16x8Clear();
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[6]) * 8 + 0], &rgba16Table[ReadUInt16(&currIn[0]) * 8 + 0]));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[8]) * 8 + 524288], &rgba16Table[ReadUInt16(&currIn[2]) * 8 + 524288]));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[10]) * 8 + 1048576], &rgba16Table[ReadUInt16(&currIn[4]) * 8 + 1048576]));
					PStoreInt16x8(tmpPtr, cvals2);
					currIn += 12;
					tmpPtr += 16;
				}
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- != 0)
			{
				cvals2b = PInt32x4Clear();
				j = tap;
				while (j-- != 0)
				{
					cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}
				PStoreInt16x4(outPt, PSARSDW4(cvals2b, 14));
				outPt += 8;
			}

			outPt += dstep;
			inPt += sstep;
		}
	}
	else if (tap == 6)
	{
		Int16x4 cvals;
		Int16x8 cvals2;
		Int32x4 cvals2b;
		dstep -= width << 3;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			if (i & 1)
			{
				cvals = PInt16x4Clear();
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[0]) * 8 + 0]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[2]) * 8 + 524288]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[4]) * 8 + 1048576]));
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 6;
				tmpPtr += 8;
			}
			i >>= 1;
			if (i != 0)
			{
				while (i-- != 0)
				{
					cvals2 = PInt16x8Clear();
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[6]) * 8 + 0], &rgba16Table[ReadUInt16(&currIn[0]) * 8 + 0]));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[8]) * 8 + 524288], &rgba16Table[ReadUInt16(&currIn[2]) * 8 + 524288]));
					cvals2 = PSADDW8(cvals2, PMLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[10]) * 8 + 1048576], &rgba16Table[ReadUInt16(&currIn[4]) * 8 + 1048576]));
					PStoreInt16x8(tmpPtr, cvals2);
					currIn += 12;
					tmpPtr += 16;
				}
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- != 0)
			{
				cvals2b = PInt32x4Clear();
				tmpVal1 = PLoadInt16x8(&tmpbuff[currIndex[0]]);
				tmpVal2 = PLoadInt16x8(&tmpbuff[currIndex[1]]);
				cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[0])));
				cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[8])));
				cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[2]]), PLoadInt16x4(&tmpbuff[currIndex[2] + 8])), PLoadInt16x8A(&currWeight[16])));
				currWeight += 24;
				currIndex += 3;
				PStoreInt16x4(outPt, PSARSDW4(cvals2b, 14));
				outPt += 8;
			}
			outPt += dstep;
			inPt += sstep;
		}
	}
	else if (swidth & 1)
	{
		Int16x4 cvals;
		Int32x4 cvals2;
		dstep -= width << 3;
		tap >>= 1;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			while (i-- != 0)
			{
				cvals = PInt16x4Clear();
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[0]) * 8 + 0]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[2]) * 8 + 524288]));
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[4]) * 8 + 1048576]));
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 6;
				tmpPtr += 8;
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- > 0)
			{
				cvals2 = PInt32x4Clear();
				j = tap;
				while (j-- > 0)
				{
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}
				PStoreInt16x4(outPt, PSARSDW4(cvals2, 14));
				outPt += 8;
			}

			outPt += dstep;
			inPt += sstep;
		}
	}
	else
	{
		Int16x8 cvals;
		Int32x4 cvals2;
		Int32x4 cvals2b;
		dstep -= width << 3;
		swidth >>= 1;
		width >>= 1;
		tap >>= 1;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			while (i-- != 0)
			{
				cvals = PInt16x8Clear();
				cvals = PSADDW8(cvals, PMLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[6]) * 8 + 0], &rgba16Table[ReadUInt16(&currIn[0]) * 8 + 0]));
				cvals = PSADDW8(cvals, PMLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[8]) * 8 + 524288], &rgba16Table[ReadUInt16(&currIn[2]) * 8 + 524288]));
				cvals = PSADDW8(cvals, PMLoadInt16x4(&rgba16Table[ReadUInt16(&currIn[10]) * 8 + 1048576], &rgba16Table[ReadUInt16(&currIn[4]) * 8 + 1048576]));
				PStoreInt16x8A(tmpPtr, cvals);
				currIn += 12;
				tmpPtr += 16;
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- != 0)
			{
				cvals2 = PInt32x4Clear();
				cvals2b = PInt32x4Clear();
				j = tap;
				while (j-- != 0)
				{
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}

				j = tap;
				while (j-- != 0)
				{
					cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}
				PStoreInt16x8A(outPt, PMergeSARDW4(cvals2, cvals2b, 14));
				outPt += 16;
			}

			outPt += dstep;
			inPt += sstep;
		}
	}
}

extern "C" void LanczosResizerFunc_HorizontalFilterPal8(UInt8 *inPt, UInt8 *outPt,OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *pal8Table, OSInt swidth, UInt8 *tmpbuff)
{
	OSInt i;
	OSInt j;
	UInt8 *currIn;
	UInt8 *tmpPtr;
	OSInt *currIndex;
	Int16 *currWeight;
	Int16x8 tmpVal1;
	Int16x8 tmpVal2;

	if ((width & 1) != 0)
	{
		Int16x4 cvals;
		Int16x8 cvals2;
		Int32x4 cvals2b;
		dstep -= width << 3;
		tap >>= 1;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			if (i & 1)
			{
				cvals = PLoadInt16x4(&pal8Table[currIn[0] * 8]);
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 1;
				tmpPtr += 8;
			}
			i >>= 1;
			if (i != 0)
			{
				while (i-- != 0)
				{
					cvals2 = PMLoadInt16x4(&pal8Table[currIn[1] * 8], &pal8Table[currIn[0] * 8]);
					PStoreInt16x8(tmpPtr, cvals2);
					currIn += 2;
					tmpPtr += 16;
				}
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- != 0)
			{
				cvals2b = PInt32x4Clear();
				j = tap;
				while (j-- != 0)
				{
					cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}
				PStoreInt16x4(outPt, PSARSDW4(cvals2b, 14));
				outPt += 8;
			}

			outPt += dstep;
			inPt += sstep;
		}
	}
	else if (tap == 6)
	{
		Int16x4 cvals;
		Int16x8 cvals2;
		Int32x4 cvals2b;
		dstep -= width << 3;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			if (i & 1)
			{
				cvals = PLoadInt16x4(&pal8Table[currIn[0] * 8]);
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 1;
				tmpPtr += 8;
			}
			i >>= 1;
			if (i != 0)
			{
				while (i-- != 0)
				{
					cvals2 = PMLoadInt16x4(&pal8Table[currIn[1] * 8], &pal8Table[currIn[0] * 8]);
					PStoreInt16x8(tmpPtr, cvals2);
					currIn += 2;
					tmpPtr += 16;
				}
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- != 0)
			{
				cvals2b = PInt32x4Clear();
				tmpVal1 = PLoadInt16x8(&tmpbuff[currIndex[0]]);
				tmpVal2 = PLoadInt16x8(&tmpbuff[currIndex[1]]);
				cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKLWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[0])));
				cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKHWW8(tmpVal1, tmpVal2), PLoadInt16x8A(&currWeight[8])));
				cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[2]]), PLoadInt16x4(&tmpbuff[currIndex[2] + 8])), PLoadInt16x8A(&currWeight[16])));
				currWeight += 24;
				currIndex += 3;
				PStoreInt16x4(outPt, PSARSDW4(cvals2b, 14));
				outPt += 8;
			}
			outPt += dstep;
			inPt += sstep;
		}
	}
	else if (swidth & 1)
	{
		Int16x4 cvals;
		Int32x4 cvals2;
		dstep -= width << 3;
		tap >>= 1;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			while (i-- != 0)
			{
				cvals = PLoadInt16x4(&pal8Table[currIn[0] * 8]);
				PStoreInt16x4(tmpPtr, cvals);
				currIn += 1;
				tmpPtr += 8;
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- > 0)
			{
				cvals2 = PInt32x4Clear();
				j = tap;
				while (j-- > 0)
				{
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}
				PStoreInt16x4(outPt, PSARSDW4(cvals2, 14));
				outPt += 8;
			}

			outPt += dstep;
			inPt += sstep;
		}
	}
	else
	{
		Int16x8 cvals;
		Int32x4 cvals2;
		Int32x4 cvals2b;
		dstep -= width << 3;
		swidth >>= 1;
		width >>= 1;
		tap >>= 1;

		while (height-- != 0)
		{
			currIn = inPt;
			tmpPtr = tmpbuff;
			i = swidth;
			while (i-- != 0)
			{
				cvals = PMLoadInt16x4(&pal8Table[currIn[1] * 8], &pal8Table[currIn[0] * 8]);
				PStoreInt16x8A(tmpPtr, cvals);
				currIn += 2;
				tmpPtr += 16;
			}

			currIndex = index;
			currWeight = (Int16*)weight;
			i = width;
			while (i-- != 0)
			{
				cvals2 = PInt32x4Clear();
				cvals2b = PInt32x4Clear();
				j = tap;
				while (j-- != 0)
				{
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}

				j = tap;
				while (j-- != 0)
				{
					cvals2b = PADDD4(cvals2b, PMADDWD(PUNPCKWW4(PLoadInt16x4(&tmpbuff[currIndex[0]]), PLoadInt16x4(&tmpbuff[currIndex[1]])), PLoadInt16x8A(currWeight)));
					currWeight += 8;
					currIndex += 2;
				}
				PStoreInt16x8A(outPt, PMergeSARDW4(cvals2, cvals2b, 14));
				outPt += 16;
			}

			outPt += dstep;
			inPt += sstep;
		}
	}
}

extern "C" void LanczosResizerFunc_VerticalFilterB8G8R8A8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable)
{
	OSInt i;
	OSInt j;
	UInt8 *currIn;
	if (width & 1)
	{
		OSInt *currIndex;
		Int16 *currWeight;
		Int32x4 cvals;
		UInt16x4 cvals2;
		tap >>= 1;
		dstep -= width << 2;
		while (height-- != 0)
		{
			currIn = inPt;

			i = width;
			while (i-- != 0)
			{
				currIndex = index;
				currWeight = (Int16*)weight;
				cvals = PInt32x4Clear();
				j = tap;
				while (j-- != 0)
				{
					cvals = PADDD4(cvals, PMADDWD(PUNPCKWW4(PLoadInt16x4(&currIn[currIndex[0]]), PLoadInt16x4(&currIn[currIndex[1]])), PLoadInt16x8(&currWeight[0])));
					currWeight += 8;
					currIndex += 2;
				}
				cvals2 = PCONVI16x4_U(PSARSDW4(cvals, 15));
				outPt[0] = lrbgraTable[PEXTUW4(cvals2, 0) + 0];
				outPt[1] = lrbgraTable[PEXTUW4(cvals2, 1) + 65536];
				outPt[2] = lrbgraTable[PEXTUW4(cvals2, 2) + 131072];
				outPt[3] = lrbgraTable[PEXTUW4(cvals2, 3) + 196608];
				currIn += 8;
				outPt += 4;
			}
			index += tap << 1;
			weight += tap << 1;
			outPt += dstep;
		}
	}
	else if (tap == 6)
	{
		Int16x8 w0;
		Int16x8 w1;
		Int16x8 w2;
		OSInt i0;
		OSInt i1;
		OSInt i2;
		OSInt i3;
		OSInt i4;
		OSInt i5;
		Int16x8 v1;
		Int16x8 v2;
		Int32x4 cvals1;
		Int32x4 cvals2;
		Int16x8 cvals3;
		tap >>= 1;
		dstep -= width << 2;
		width >>= 1;
		while (height-- != 0)
		{
			currIn = inPt;
			w0 = PLoadInt16x8(&weight[0]);
			w1 = PLoadInt16x8(&weight[2]);
			w2 = PLoadInt16x8(&weight[4]);
			i0 = index[0];
			i1 = index[1];
			i2 = index[2];
			i3 = index[3];
			i4 = index[4];
			i5 = index[5];
			i = width;
			while (i-- != 0)
			{
				v1 = PLoadInt16x8(&currIn[i0]);
				v2 = PLoadInt16x8(&currIn[i1]);
				cvals1 = PMADDWD(PUNPCKLWW8(v1, v2), w0);
				cvals2 = PMADDWD(PUNPCKHWW8(v1, v2), w0);
				v1 = PLoadInt16x8(&currIn[i2]);
				v2 = PLoadInt16x8(&currIn[i3]);
				cvals1 = PADDD4(cvals1, PMADDWD(PUNPCKLWW8(v1, v2), w1));
				cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(v1, v2), w1));
				v1 = PLoadInt16x8(&currIn[i4]);
				v2 = PLoadInt16x8(&currIn[i5]);
				cvals1 = PADDD4(cvals1, PMADDWD(PUNPCKLWW8(v1, v2), w2));
				cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(v1, v2), w2));
				cvals3 = PMergeSARDW4(cvals1, cvals2, 15);
				outPt[0] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 0) + 0];
				outPt[1] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 1) + 65536];
				outPt[2] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 2) + 131072];
				outPt[3] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 3) + 196608];
				outPt[4] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 4) + 0];
				outPt[5] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 5) + 65536];
				outPt[6] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 6) + 131072];
				outPt[7] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 7) + 196608];

				currIn += 16;
				outPt += 8;
			}
			index += 6;
			weight += 6;
			outPt += dstep;
		}
	}
	else
	{
		OSInt *currIndex;
		Int16 *currWeight;
		Int16x8 v1;
		Int16x8 v2;
		Int32x4 cvals1;
		Int32x4 cvals2;
		Int16x8 cvals3;
		Int16x8 wval;
		tap >>= 1;
		dstep -= width << 2;
		width >>= 1;
		while (height-- != 0)
		{
			currIn = inPt;
			i = width;
			while (i-- != 0)
			{
				currIndex = index;
				currWeight = (Int16*)weight;
				cvals1 = PInt32x4Clear();
				cvals2 = PInt32x4Clear();
				j = tap;
				while (j-- != 0)
				{
					v1 = PLoadInt16x8(&currIn[currIndex[0]]);
					v2 = PLoadInt16x8(&currIn[currIndex[1]]);
					wval = PLoadInt16x8(&currWeight[0]);
					cvals1 = PADDD4(cvals1, PMADDWD(PUNPCKLWW8(v1, v2), wval));
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(v1, v2), wval));
					currWeight += 8;
					currIndex += 2;
				}
				cvals3 = PMergeSARDW4(cvals1, cvals2, 15);
				outPt[0] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 0) + 0];
				outPt[1] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 1) + 65536];
				outPt[2] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 2) + 131072];
				outPt[3] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 3) + 196608];
				outPt[4] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 4) + 0];
				outPt[5] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 5) + 65536];
				outPt[6] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 6) + 131072];
				outPt[7] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 7) + 196608];
				currIn += 16;
				outPt += 8;
			}
			index += tap << 1;
			weight += tap << 1;
			outPt += dstep;
		}
	}
}

extern "C" void LanczosResizerFunc_VerticalFilterB8G8R8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable)
{
	OSInt i;
	OSInt j;
	UInt8 *currIn;
	if (width & 1)
	{
		OSInt *currIndex;
		Int16 *currWeight;
		Int32x4 cvals;
		UInt16x4 cvals2;
		tap >>= 1;
		dstep -= width * 3;
		while (height-- != 0)
		{
			currIn = inPt;

			i = width;
			while (i-- != 0)
			{
				currIndex = index;
				currWeight = (Int16*)weight;
				cvals = PInt32x4Clear();
				j = tap;
				while (j-- != 0)
				{
					cvals = PADDD4(cvals, PMADDWD(PUNPCKWW4(PLoadInt16x4(&currIn[currIndex[0]]), PLoadInt16x4(&currIn[currIndex[1]])), PLoadInt16x8(&currWeight[0])));
					currWeight += 8;
					currIndex += 2;
				}
				cvals2 = PCONVI16x4_U(PSARSDW4(cvals, 15));
				outPt[0] = lrbgraTable[PEXTUW4(cvals2, 0) + 0];
				outPt[1] = lrbgraTable[PEXTUW4(cvals2, 1) + 65536];
				outPt[2] = lrbgraTable[PEXTUW4(cvals2, 2) + 131072];
				currIn += 8;
				outPt += 3;
			}
			index += tap << 1;
			weight += tap << 1;
			outPt += dstep;
		}
	}
	else if (tap == 6)
	{
		Int16x8 w0;
		Int16x8 w1;
		Int16x8 w2;
		OSInt i0;
		OSInt i1;
		OSInt i2;
		OSInt i3;
		OSInt i4;
		OSInt i5;
		Int16x8 v1;
		Int16x8 v2;
		Int32x4 cvals1;
		Int32x4 cvals2;
		Int16x8 cvals3;
		tap >>= 1;
		dstep -= width * 3;
		width >>= 1;
		while (height-- != 0)
		{
			currIn = inPt;
			w0 = PLoadInt16x8(&weight[0]);
			w1 = PLoadInt16x8(&weight[2]);
			w2 = PLoadInt16x8(&weight[4]);
			i0 = index[0];
			i1 = index[1];
			i2 = index[2];
			i3 = index[3];
			i4 = index[4];
			i5 = index[5];
			i = width;
			while (i-- != 0)
			{
				v1 = PLoadInt16x8(&currIn[i0]);
				v2 = PLoadInt16x8(&currIn[i1]);
				cvals1 = PMADDWD(PUNPCKLWW8(v1, v2), w0);
				cvals2 = PMADDWD(PUNPCKHWW8(v1, v2), w0);
				v1 = PLoadInt16x8(&currIn[i2]);
				v2 = PLoadInt16x8(&currIn[i3]);
				cvals1 = PADDD4(cvals1, PMADDWD(PUNPCKLWW8(v1, v2), w1));
				cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(v1, v2), w1));
				v1 = PLoadInt16x8(&currIn[i4]);
				v2 = PLoadInt16x8(&currIn[i5]);
				cvals1 = PADDD4(cvals1, PMADDWD(PUNPCKLWW8(v1, v2), w2));
				cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(v1, v2), w2));
				cvals3 = PMergeSARDW4(cvals1, cvals2, 15);
				outPt[0] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 0) + 0];
				outPt[1] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 1) + 65536];
				outPt[2] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 2) + 131072];
				outPt[3] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 4) + 0];
				outPt[4] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 5) + 65536];
				outPt[5] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 6) + 131072];

				currIn += 16;
				outPt += 6;
			}
			index += 6;
			weight += 6;
			outPt += dstep;
		}
	}
	else
	{
		OSInt *currIndex;
		Int16 *currWeight;
		Int16x8 v1;
		Int16x8 v2;
		Int32x4 cvals1;
		Int32x4 cvals2;
		Int16x8 cvals3;
		Int16x8 wval;
		tap >>= 1;
		dstep -= width * 3;
		width >>= 1;
		while (height-- != 0)
		{
			currIn = inPt;
			i = width;
			while (i-- != 0)
			{
				currIndex = index;
				currWeight = (Int16*)weight;
				cvals1 = PInt32x4Clear();
				cvals2 = PInt32x4Clear();
				j = tap;
				while (j-- != 0)
				{
					v1 = PLoadInt16x8(&currIn[currIndex[0]]);
					v2 = PLoadInt16x8(&currIn[currIndex[1]]);
					wval = PLoadInt16x8(&currWeight[0]);
					cvals1 = PADDD4(cvals1, PMADDWD(PUNPCKLWW8(v1, v2), wval));
					cvals2 = PADDD4(cvals2, PMADDWD(PUNPCKHWW8(v1, v2), wval));
					currWeight += 8;
					currIndex += 2;
				}
				cvals3 = PMergeSARDW4(cvals1, cvals2, 15);
				outPt[0] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 0) + 0];
				outPt[1] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 1) + 65536];
				outPt[2] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 2) + 131072];
				outPt[3] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 4) + 0];
				outPt[4] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 5) + 65536];
				outPt[5] = lrbgraTable[PEXTUW8(PCONVI16x8_U(cvals3), 6) + 131072];
				currIn += 16;
				outPt += 6;
			}
			index += tap << 1;
			weight += tap << 1;
			outPt += dstep;
		}
	}
}

extern "C" void LanczosResizerFunc_ExpandB8G8R8A8PA(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbaTable)
{
	OSInt i;
	Int16x4 cvals;
	sstep -= width << 2;
	dstep -= width << 3;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbaTable[4096 + inPt[0] * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[2048 + inPt[1] * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[0 + inPt[2] * 8]));
			cvals = PMULM2HW4(cvals, PInt16x4SetA((Int16)((inPt[3] << 7) | (inPt[3] >> 1))));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[6144 + inPt[3] * 8]));
			PStoreInt16x4(outPt, cvals);

			inPt += 4;
			outPt += 8;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ExpandB8G8R8A8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbaTable)
{
	OSInt i;
	Int16x4 cvals;
	sstep -= width << 2;
	dstep -= width << 3;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbaTable[4096 + inPt[0] * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[2048 + inPt[1] * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[0 + inPt[2] * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[6144 + inPt[3] * 8]));
			PStoreInt16x4(outPt, cvals);
			inPt += 4;
			outPt += 8;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ExpandB8G8R8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbaTable)
{
	OSInt i;
	Int16x4 cvals;
	sstep -= width * 3;
	dstep -= width << 3;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbaTable[4096 + inPt[0] * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[2048 + inPt[1] * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[0 + inPt[2] * 8]));
			PStoreInt16x4(outPt, cvals);
			inPt += 3;
			outPt += 8;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ExpandPal8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *pal8Table)
{
	OSInt i;
	Int16x4 cvals;
	sstep -= width;
	dstep -= width << 3;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&pal8Table[inPt[0] * 8]);
			PStoreInt16x4(outPt, cvals);
			inPt += 1;
			outPt += 8;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ExpandR16G16B16(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *rgba16Table)
{
	OSInt i;
	Int16x4 cvals;
	sstep -= width * 6;
	dstep -= width << 3;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgba16Table[ReadUInt16(&inPt[0]) * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&inPt[2]) * 8 + 524288]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&inPt[4]) * 8 + 1048576]));
			PStoreInt16x4(outPt, cvals);
			inPt += 6;
			outPt += 8;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_CollapseB8G8R8A8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable)
{
	OSInt i;
	sstep -= width << 3;
	dstep -= width << 2;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			outPt[0] = lrbgraTable[0 + ReadUInt16(&inPt[0])];
			outPt[1] = lrbgraTable[65536 + ReadUInt16(&inPt[2])];
			outPt[2] = lrbgraTable[131072 + ReadUInt16(&inPt[4])];
			outPt[3] = lrbgraTable[196608 + ReadUInt16(&inPt[6])];
			inPt += 8;
			outPt += 4;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_CollapseB8G8R8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable)
{
	OSInt i;
	sstep -= width << 3;
	dstep -= width * 3;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			outPt[0] = lrbgraTable[0 + ReadUInt16(&inPt[0])];
			outPt[1] = lrbgraTable[65536 + ReadUInt16(&inPt[2])];
			outPt[2] = lrbgraTable[131072 + ReadUInt16(&inPt[4])];
			inPt += 8;
			outPt += 3;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ImgCopyB8G8R8A8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable)
{
	OSInt i;
	Int16x4 cvals;
	UInt16x4 cvals2;
	sstep -= width << 2;
	dstep -= width << 2;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbaTable[4096 + inPt[0] * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[2048 + inPt[1] * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[0 + inPt[2] * 8]));
			cvals2 = PCONVI16x4_U(cvals);
			outPt[0] = lrbgraTable[PEXTUW4(cvals2, 0) + 0];
			outPt[1] = lrbgraTable[PEXTUW4(cvals2, 1) + 65536];
			outPt[2] = lrbgraTable[PEXTUW4(cvals2, 2) + 131072];
			outPt[3] = inPt[3];
			inPt += 4;
			outPt += 4;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ImgCopyB8G8R8A8PA(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable)
{
	OSInt i;
	Int16x4 cvals;
	UInt16x4 cvals2;
	sstep -= width << 2;
	dstep -= width << 2;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbaTable[4096 + inPt[0] * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[2048 + inPt[1] * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[0 + inPt[2] * 8]));
			cvals = PMULM2HW4(cvals, PInt16x4SetA((Int16)((inPt[3] << 7) | (inPt[3] >> 1))));
			cvals2 = PCONVI16x4_U(cvals);
			outPt[0] = lrbgraTable[PEXTUW4(cvals2, 0) + 0];
			outPt[1] = lrbgraTable[PEXTUW4(cvals2, 1) + 65536];
			outPt[2] = lrbgraTable[PEXTUW4(cvals2, 2) + 131072];
			outPt[3] = inPt[3];
			inPt += 4;
			outPt += 4;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ImgCopyB8G8R8_B8G8R8A8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable)
{
	OSInt i;
	Int16x4 cvals;
	UInt16x4 cvals2;
	sstep -= width * 3;
	dstep -= width << 2;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbaTable[4096 + inPt[0] * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[2048 + inPt[1] * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[0 + inPt[2] * 8]));
			cvals2 = PCONVI16x4_U(cvals);
			outPt[0] = lrbgraTable[PEXTUW4(cvals2, 0) + 0];
			outPt[1] = lrbgraTable[PEXTUW4(cvals2, 1) + 65536];
			outPt[2] = lrbgraTable[PEXTUW4(cvals2, 2) + 131072];
			outPt[3] = (UInt8)0xff;
			inPt += 3;
			outPt += 4;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ImgCopyPal8_B8G8R8A8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *pal8Table)
{
	OSInt i;
	Int16x4 cvals;
	UInt16x4 cvals2;
	sstep -= width;
	dstep -= width << 2;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&pal8Table[inPt[0] * 8]);
			cvals2 = PCONVI16x4_U(cvals);
			outPt[0] = lrbgraTable[PEXTUW4(cvals2, 0) + 0];
			outPt[1] = lrbgraTable[PEXTUW4(cvals2, 1) + 65536];
			outPt[2] = lrbgraTable[PEXTUW4(cvals2, 2) + 131072];
			outPt[3] = lrbgraTable[PEXTUW4(cvals2, 3) + 196608];
			inPt += 1;
			outPt += 4;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ImgCopyR16G16B16_B8G8R8A8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgba16Table)
{
	OSInt i;
	Int16x4 cvals;
	UInt16x4 cvals2;
	sstep -= width * 6;
	dstep -= width << 2;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgba16Table[ReadUInt16(&inPt[0]) * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&inPt[2]) * 8 + 524288]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&inPt[4]) * 8 + 1048576]));
			cvals2 = PCONVI16x4_U(cvals);
			outPt[0] = lrbgraTable[PEXTUW4(cvals2, 0) + 0];
			outPt[1] = lrbgraTable[PEXTUW4(cvals2, 1) + 65536];
			outPt[2] = lrbgraTable[PEXTUW4(cvals2, 2) + 131072];
			outPt[3] = (UInt8)0xff;
			inPt += 6;
			outPt += 4;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ImgCopyB8G8R8A8_B8G8R8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable)
{
	OSInt i;
	Int16x4 cvals;
	UInt16x4 cvals2;
	sstep -= width << 2;
	dstep -= width * 3;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbaTable[4096 + inPt[0] * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[2048 + inPt[1] * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[0 + inPt[2] * 8]));
			cvals2 = PCONVI16x4_U(cvals);
			outPt[0] = lrbgraTable[PEXTUW4(cvals2, 0) + 0];
			outPt[1] = lrbgraTable[PEXTUW4(cvals2, 1) + 65536];
			outPt[2] = lrbgraTable[PEXTUW4(cvals2, 2) + 131072];
			inPt += 4;
			outPt += 3;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ImgCopyB8G8R8A8PA_B8G8R8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable)
{
	OSInt i;
	Int16x4 cvals;
	UInt16x4 cvals2;
	sstep -= width << 2;
	dstep -= width * 3;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbaTable[4096 + inPt[0] * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[2048 + inPt[1] * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[0 + inPt[2] * 8]));
			cvals = PMULM2HW4(cvals, PInt16x4SetA((Int16)((inPt[3] << 7) | (inPt[3] >> 1))));
			cvals2 = PCONVI16x4_U(cvals);
			outPt[0] = lrbgraTable[PEXTUW4(cvals2, 0) + 0];
			outPt[1] = lrbgraTable[PEXTUW4(cvals2, 1) + 65536];
			outPt[2] = lrbgraTable[PEXTUW4(cvals2, 2) + 131072];
			inPt += 4;
			outPt += 3;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ImgCopyB8G8R8_B8G8R8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable)
{
	OSInt i;
	Int16x4 cvals;
	UInt16x4 cvals2;
	sstep -= width * 3;
	dstep -= width << 2;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbaTable[4096 + inPt[0] * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[2048 + inPt[1] * 8]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbaTable[0 + inPt[2] * 8]));
			cvals2 = PCONVI16x4_U(cvals);
			outPt[0] = lrbgraTable[PEXTUW4(cvals2, 0) + 0];
			outPt[1] = lrbgraTable[PEXTUW4(cvals2, 1) + 65536];
			outPt[2] = lrbgraTable[PEXTUW4(cvals2, 2) + 131072];
			inPt += 3;
			outPt += 3;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ImgCopyPal8_B8G8R8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *pal8Table)
{
	OSInt i;
	Int16x4 cvals;
	UInt16x4 cvals2;
	sstep -= width;
	dstep -= width << 2;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&pal8Table[inPt[0] * 8]);
			cvals2 = PCONVI16x4_U(cvals);
			outPt[0] = lrbgraTable[PEXTUW4(cvals2, 0) + 0];
			outPt[1] = lrbgraTable[PEXTUW4(cvals2, 1) + 65536];
			outPt[2] = lrbgraTable[PEXTUW4(cvals2, 2) + 131072];
			inPt += 1;
			outPt += 3;
		}
		inPt += sstep;
		outPt += dstep;
	}
}

extern "C" void LanczosResizerFunc_ImgCopyR16G16B16_B8G8R8(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgba16Table)
{
	OSInt i;
	Int16x4 cvals;
	UInt16x4 cvals2;
	sstep -= width * 6;
	dstep -= width << 2;

	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgba16Table[ReadUInt16(&inPt[0]) * 8]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&inPt[2]) * 8 + 524288]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgba16Table[ReadUInt16(&inPt[4]) * 8 + 1048576]));
			cvals2 = PCONVI16x4_U(cvals);
			outPt[0] = lrbgraTable[PEXTUW4(cvals2, 0) + 0];
			outPt[1] = lrbgraTable[PEXTUW4(cvals2, 1) + 65536];
			outPt[2] = lrbgraTable[PEXTUW4(cvals2, 2) + 131072];
			inPt += 6;
			outPt += 3;
		}
		inPt += sstep;
		outPt += dstep;
	}
}
