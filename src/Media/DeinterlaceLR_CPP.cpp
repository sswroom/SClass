#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"
#include "Core/ByteTool_C.h"

extern "C"
{
void DeinterlaceLR_VerticalFilter(UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep)
{
	UInt8 *sptr;
	UInt8 *dptr;
	UIntOS i;
	UIntOS j;
	Int32x4 vals;
	Int64 *wweight;
	IntOS *iindex;

	while (height-- > 0)
	{
		sptr = inPt;
		dptr = outPt;
		i = width;
		while (i-- > 0)
		{
			vals = PInt32x4Clear();
			iindex = index;
			wweight = weight;
			j = tap;
			while (j > 0)
			{
				vals = PADDD4(vals, PMADDWD(PUNPCKWW4(PLoadInt16x4(&sptr[iindex[0]]), PLoadInt16x4(&sptr[iindex[1]])), PLoadInt16x8A(wweight)));
				wweight += 2;
				iindex += 2;

				j -= 2;
			}

			PStoreInt16x4(dptr, SI32ToI16x4(PSARD4(vals, 15)));
			sptr += 8;
			dptr += 8;
		}

		index += tap;
		weight += tap;
		outPt += dstep;
	}
}

void DeinterlaceLR_VerticalFilterOdd(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep)
{
	UInt8 *sptr;
	UInt8 *dptr;
	UIntOS i;
	UIntOS j;
	Int32x4 vals;
	Int64 *wweight;
	IntOS *iindex;
	height >>= 1;
	index += tap;
	weight += tap;

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
			vals = PInt32x4Clear();
			iindex = index;
			wweight = weight;
			j = tap;
			while (j > 0)
			{
				vals = PADDD4(vals, PMADDWD(PUNPCKWW4(PLoadInt16x4(&sptr[iindex[0]]), PLoadInt16x4(&sptr[iindex[1]])), PLoadInt16x8A(wweight)));
				wweight += 2;
				iindex += 2;

				j -= 2;
			}

			PStoreInt16x4(dptr, SI32ToI16x4(PSARD4(vals, 15)));
			sptr += 8;
			dptr += 8;
		}

		index += tap * 2;
		weight += tap * 2;
		outPt += dstep;
	}
}

void DeinterlaceLR_VerticalFilterEven(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep)
{
	UInt8 *sptr;
	UInt8 *dptr;
	UIntOS i;
	UIntOS j;
	Int32x4 vals;
	Int64 *wweight;
	IntOS *iindex;
	height >>= 1;

	while (height-- > 0)
	{
		sptr = inPt;
		dptr = outPt;
		i = width;
		while (i-- > 0)
		{
			vals = PInt32x4Clear();
			iindex = index;
			wweight = weight;
			j = tap;
			while (j > 0)
			{
				vals = PADDD4(vals, PMADDWD(PUNPCKWW4(PLoadInt16x4(&sptr[iindex[0]]), PLoadInt16x4(&sptr[iindex[1]])), PLoadInt16x8A(wweight)));
				wweight += 2;
				iindex += 2;

				j -= 2;
			}

			PStoreInt16x4(dptr, SI32ToI16x4(PSARD4(vals, 15)));
			sptr += 8;
			dptr += 8;
		}

		index += tap * 2;
		weight += tap * 2;
		outPt += dstep;

		MemCopyNO(outPt, inPtCurr, width * 8);
		inPtCurr += sstep;
		outPt += dstep;
	}
}
}