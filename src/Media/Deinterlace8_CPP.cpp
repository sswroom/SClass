#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"

extern "C"
{
void Deinterlace8_VerticalFilter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
	UInt8 *sptr;
	UInt8 *dptr;
	UOSInt i;
	UOSInt j;
	OSInt *iindex;
	Int64 *wweight;
	Int32x4 v1;
	Int32x4 v2;
	UInt8x8 tmpV1;
	UInt8x8 tmpV2;
	Int16x8 tmpV3;
	Int16x8 tmpV4;
	width >>= 1;
	while (height-- > 0)
	{
		sptr = inPt;
		dptr = outPt;
		i = width;
		while (i-- > 0)
		{
			v1 = PInt32x4Clear();
			v2 = PInt32x4Clear();
			iindex = index;
			wweight = weight;
			j = tap;
			while (j > 0)
			{
				tmpV1 = PLoadUInt8x8(&sptr[iindex[0]]);
				tmpV2 = PLoadUInt8x8(&sptr[iindex[1]]);
				tmpV3 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(tmpV1, tmpV1), 1));
				tmpV4 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(tmpV2, tmpV2), 1));

				v1 = PADDD4(v1, PMADDWD(PUNPCKLWW8(tmpV3, tmpV4), PLoadInt16x8A(wweight)));
				v2 = PADDD4(v2, PMADDWD(PUNPCKHWW8(tmpV3, tmpV4), PLoadInt16x8A(wweight)));
				iindex += 2;
				wweight += 2;
				j -= 2;
			}
			
			PStoreUInt8x8(dptr, SI32ToU8x8(PSARD4(v1, 22), PSARD4(v2, 22)));
			dptr += 8;
			sptr += 8;
		}

		outPt += dstep;
		index += tap;
		weight += tap;
	}
}

void Deinterlace8_VerticalFilterOdd(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
	UInt8 *sptr;
	UInt8 *dptr;
	UOSInt i;
	UOSInt j;
	OSInt *iindex;
	Int64 *wweight;
	Int32x4 v1;
	Int32x4 v2;
	UInt8x8 tmpV1;
	UInt8x8 tmpV2;
	Int16x8 tmpV3;
	Int16x8 tmpV4;
	height >>= 1;
	width >>= 1;
	while (height-- > 0)
	{
		MemCopyNO(outPt, inPtCurr, width * 8);
		inPtCurr += sstep;
		outPt += dstep;

		sptr = inPt;
		dptr = outPt;
		i = width;
		while (i-- > 0)
		{
			v1 = PInt32x4Clear();
			v2 = PInt32x4Clear();
			iindex = index;
			wweight = weight;
			j = tap;
			while (j > 0)
			{
				tmpV1 = PLoadUInt8x8(&sptr[iindex[0]]);
				tmpV2 = PLoadUInt8x8(&sptr[iindex[1]]);
				tmpV3 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(tmpV1, tmpV1), 1));
				tmpV4 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(tmpV2, tmpV2), 1));

				v1 = PADDD4(v1, PMADDWD(PUNPCKLWW8(tmpV3, tmpV4), PLoadInt16x8A(wweight)));
				v2 = PADDD4(v2, PMADDWD(PUNPCKLWW8(tmpV3, tmpV4), PLoadInt16x8A(wweight)));
				iindex += 2;
				wweight += 2;
				j -= 2;
			}
			
			PStoreUInt8x8(dptr, SI32ToU8x8(PSARD4(v1, 22), PSARD4(v2, 22)));
			dptr += 8;
			sptr += 8;
		}

		outPt += dstep;
		index += tap * 2;
		weight += tap * 2;
	}
}

void Deinterlace8_VerticalFilterEven(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
	UInt8 *sptr;
	UInt8 *dptr;
	UOSInt i;
	UOSInt j;
	OSInt *iindex;
	Int64 *wweight;
	Int32x4 v1;
	Int32x4 v2;
	UInt8x8 tmpV1;
	UInt8x8 tmpV2;
	Int16x8 tmpV3;
	Int16x8 tmpV4;
	width >>= 1;
	height >>= 1;
	while (height-- > 0)
	{
		sptr = inPt;
		dptr = outPt;
		i = width;
		while (i-- > 0)
		{
			v1 = PInt32x4Clear();
			v2 = PInt32x4Clear();
			iindex = index;
			wweight = weight;
			j = tap;
			while (j > 0)
			{
				tmpV1 = PLoadUInt8x8(&sptr[iindex[0]]);
				tmpV2 = PLoadUInt8x8(&sptr[iindex[1]]);
				tmpV3 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(tmpV1, tmpV1), 1));
				tmpV4 = PCONVU16x8_I(PSHRW8(PUNPCKUBW8(tmpV2, tmpV2), 1));

				v1 = PADDD4(v1, PMADDWD(PUNPCKLWW8(tmpV3, tmpV4), PLoadInt16x8A(wweight)));
				v2 = PADDD4(v2, PMADDWD(PUNPCKLWW8(tmpV3, tmpV4), PLoadInt16x8A(wweight)));
				iindex += 2;
				wweight += 2;
				j -= 2;
			}
			
			PStoreUInt8x8(dptr, SI32ToU8x8(PSARD4(v1, 22), PSARD4(v2, 22)));
			dptr += 8;
			sptr += 8;
		}

		outPt += dstep;
		index += tap * 2;
		weight += tap * 2;

		MemCopyNO(outPt, inPtCurr, width * 8);
		inPtCurr += sstep;
		outPt += dstep;
	}
}
}
