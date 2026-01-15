#include "Stdafx.h"
#include "Math/LanczosFilter.h"
#include "Math/Math_C.h"
#include "Media/Resizer/LanczosResizerFunc.h"

void Media::Resizer::LanczosResizerFunc::SetupInterpolationParameterV(UIntOS nTap, Double sourceLength, IntOS sourceMaxPos, UIntOS result_length, NN<Parameter> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	Double *work;
	Double  sum;
	Double  pos;
	Double dnTap = UIntOS2Double(nTap);
	Double posDiff = (dnTap / 2 - 0.5);//2.5);
	Double dresultLength = UIntOS2Double(result_length);
	Math::LanczosFilter lanczos(nTap);
	IntOS maxOfst = (sourceMaxPos - 1) * indexSep;

	out->length = result_length;
	out->tap = nTap;
	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(IntOS, out->length * out->tap);
	out->tap += out->tap & 1;

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UIntOS2Double(i) + 0.5) * sourceLength;
		pos = pos / dresultLength + offsetCorr;
		n = (IntOS)Math_Fix(pos - posDiff);
		pos = (IntOS2Double(n) + 0.5 - pos);
		sum = 0;
		for(j = 0; j < out->tap; j++)
		{
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if(n >= sourceMaxPos){
				out->index[i * out->tap + j] = maxOfst;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = lanczos.Weight(pos);
			sum += work[j];
			pos += 1;
			n += 1;
		}

		Double workRate = 32767.0 / sum;
		j = 0;
		while (j < out->tap)
		{
			Int16 v1 = (Int16)(0xffff & Double2Int32(work[j] * workRate));
			Int16 v2 = (Int16)(0xffff & Double2Int32(work[j + 1] * workRate));
			Int16 *tmpPtr = (Int16*)&out->weight[i * out->tap + j];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;
			j += 2;
		}
		i++;
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizerFunc::SetupDecimationParameterV(UIntOS nTap, Double sourceLength, IntOS sourceMaxPos, UIntOS result_length, NN<Parameter> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	UIntOS ttap;
	Double *work;
	Double  sum;
	Double  pos, phase;
	Double dnTap = UIntOS2Double(nTap);
	Double rLength = UIntOS2Double(result_length);
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = (UIntOS)Math_Fix((dnTap * (sourceLength) + (rLength - 1)) / rLength);
	ttap = out->tap;
	out->tap += out->tap & 1;

	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(IntOS, out->length * out->tap);
	
	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UIntOS2Double(i) - Math_Fix(dnTap / 2) + 0.5) * sourceLength / rLength + 0.5;
		n = (IntOS)floor(pos + offsetCorr);
		sum = 0;
		j = 0;
		while (j < ttap)
		{
			phase = (IntOS2Double(n) + 0.5)*rLength;
			phase /= sourceLength;
			phase -= (UIntOS2Double(i)+0.5);
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if(n >= sourceMaxPos){
				out->index[i * out->tap + j] = (sourceMaxPos-1) * indexSep;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = lanczos.Weight(phase);
			sum += work[j];
			n += 1;
			j++;
		}
		if (ttap & 1)
		{
			work[ttap] = 0;
			out->index[i * out->tap + ttap] = out->index[i * out->tap + ttap - 1];
		}

		Double workRate = 32767.0 / sum;
		j = 0;
		while (j < ttap)
		{
			Int16 v1 = (Int16)(0xffff & Double2Int32(work[j] * workRate));
			Int16 v2 = (Int16)(0xffff & Double2Int32(work[j + 1] * workRate));
			Int16 *tmpPtr = (Int16*)&out->weight[i * out->tap + j];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;
			j += 2;
		}
		i++;
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizerFunc::SetupInterpolationParameterH(UIntOS nTap, Double sourceLength, IntOS sourceMaxPos, UIntOS result_length, NN<Parameter> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	Double *work;
	Double  sum;
	Double  pos;
	Double posDiff = UIntOS2Double(nTap / 2) - 0.5; //2.5
	Double dresultLength = UIntOS2Double(result_length);
	IntOS ind1;
	IntOS ind2;
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = nTap;
	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(IntOS, out->length * out->tap);

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UIntOS2Double(i) + 0.5) * sourceLength;
		pos = pos / dresultLength + offsetCorr;
		n = (IntOS)Math_Fix(pos - posDiff);
		pos = (IntOS2Double(n) + 0.5 - pos);
		sum = 0;
		if (out->tap == 6 && (result_length & 1) == 0)
		{
			j = 0;
			while (j < out->tap)
			{
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos.Weight(pos) + lanczos.Weight(pos + 1);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if (n >= sourceMaxPos - 1)
				{
					ind1 = sourceMaxPos - 2;
					ind2 = ind1 + 1;
					work[j] = 0;
					work[j + 1] = lanczos.Weight(pos) + lanczos.Weight(pos + 1);
					sum += work[j + 1];
				}
				else
				{
					ind1 = n;
					ind2 = n + 1;
					work[j] = lanczos.Weight(pos);
					work[j + 1] = lanczos.Weight(pos + 1);
					sum += work[j] + work[j + 1];
				}
//				out->index[i * out->tap + j] = ind1 * indexSep;
//				out->index[i * out->tap + j + 1] = ind2 * indexSep;
				out->index[i * 3 + (j >> 1)] = ind1 * indexSep;
				pos += 2;
				n += 2;
				j += 2;
			}

			UInt16 v1;
			UInt16 v2;
			UInt16 *tmpPtr;
			Double workRate = 16384.0 / sum;
			v1 = (UInt16)(0xffff & Double2Int32(work[0] * workRate));
			v2 = (UInt16)(0xffff & Double2Int32(work[2] * workRate));
			tmpPtr = (UInt16*)&out->weight[i * out->tap];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;

			v1 = (UInt16)(0xffff & Double2Int32(work[1] * workRate));
			v2 = (UInt16)(0xffff & Double2Int32(work[3] * workRate));
			tmpPtr += 8;
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;

			v1 = (UInt16)(0xffff & Double2Int32(work[4] * workRate));
			v2 = (UInt16)(0xffff & Double2Int32(work[5] * workRate));
			tmpPtr += 8;
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;
		}
		else
		{
			j = 0;
			while (j < out->tap)
			{
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos.Weight(pos) + lanczos.Weight(pos + 1);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if (n >= sourceMaxPos - 1)
				{
					ind1 = sourceMaxPos - 2;
					ind2 = ind1 + 1;
					work[j] = 0;
					work[j + 1] = lanczos.Weight(pos) + lanczos.Weight(pos + 1);
					sum += work[j + 1];
				}
				else
				{
					ind1 = n;
					ind2 = n + 1;
					work[j] = lanczos.Weight(pos);
					work[j + 1] = lanczos.Weight(pos + 1);
					sum += work[j] + work[j + 1];
				}
				out->index[i * out->tap + j] = ind1 * indexSep;
				out->index[i * out->tap + j + 1] = ind2 * indexSep;
				pos += 2;
				n += 2;
				j += 2;
			}

			Double workRate = 16384.0 / sum;
			j = 0;
			while (j < out->tap)
			{
				UInt16 v1;
				UInt16 v2;
				UInt16 *tmpPtr;
				v1 = (UInt16)(0xffff & Double2Int32(work[j] * workRate));
				v2 = (UInt16)(0xffff & Double2Int32(work[j + 1] * workRate));
				tmpPtr = (UInt16*)&out->weight[i * out->tap + j];
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				j += 2;
			}
		}
		i++;
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizerFunc::SetupDecimationParameterH(UIntOS nTap, Double sourceLength, IntOS sourceMaxPos, UIntOS result_length, NN<Parameter> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	Double dn;
	Double di;
	UIntOS ttap;
	Double *work;
	Double  sum;
	Double  pos;
	Double phase;
	Double phase2;
	IntOS ind1;
	IntOS ind2;
	Double rLength = UIntOS2Double(result_length);
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = (UIntOS)Math_Fix((UIntOS2Double(nTap) * (sourceLength) + (rLength - 1)) / rLength);
	ttap = out->tap;
	out->tap += out->tap & 1;

	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(IntOS, out->length * out->tap);
	
	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		di = UIntOS2Double(i);
		pos = (di - UIntOS2Double(nTap / 2) + 0.5) * sourceLength / rLength + 0.5;
		dn = Math_Fix(pos + offsetCorr);
		n = (Int32)dn;
		sum = 0;
		if (ttap & 1)
		{
			j = 0;
			while (j < ttap - 1)
			{
				dn = IntOS2Double(n);
				phase = ((dn + 0.5) * rLength / sourceLength) - (di + 0.5);
				phase2 = ((dn + 1.5) * rLength / sourceLength) - (di + 0.5);
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos.Weight(phase) + lanczos.Weight(phase2);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if (n >= sourceMaxPos - 1)
				{
					ind1 = sourceMaxPos - 2;
					ind2 = ind1 + 1;
					work[j] = 0;
					work[j + 1] = lanczos.Weight(phase) + lanczos.Weight(phase2);
					sum += work[j + 1];
				}
				else
				{
					ind1 = n;
					ind2 = n + 1;
					work[j] = lanczos.Weight(phase);
					work[j + 1] = lanczos.Weight(phase2);
					sum += work[j] + work[j + 1];
				}
				out->index[i * out->tap + j] = ind1 * indexSep;
				out->index[i * out->tap + j + 1] = ind2 * indexSep;
				n += 2;
				j += 2;
			}
			phase = ((dn + 0.5) * rLength / sourceLength) - (di + 0.5);
			if (n < 0)
			{
				ind1 = 0;
				ind2 = 0;
				work[j] = lanczos.Weight(phase);
				work[j + 1] = 0;
				sum += work[j];
			}
			else if (n >= sourceMaxPos - 1)
			{
				ind1 = sourceMaxPos - 2;
				ind2 = ind1 + 1;
				work[j] = 0;
				work[j + 1] = lanczos.Weight(phase);
				sum += work[j + 1];
			}
			else
			{
				ind1 = n;
				ind2 = n + 1;
				work[j] = lanczos.Weight(phase);
				work[j + 1] = 0;
				sum += work[j];
			}
			out->index[i * out->tap + j] = ind1 * indexSep;
			out->index[i * out->tap + j + 1] = ind2 * indexSep;

			j = 0;
			while (j < ttap)
			{
				Int16 v1 = (Int16)Double2Int32((work[j] / sum) * 16384.0);
				Int16 v2 = (Int16)Double2Int32((work[j + 1] / sum) * 16384.0);
				Int16 *tmpPtr = (Int16*)&out->weight[i * out->tap + j];
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				j += 2;
			}
		}
		else if (ttap == 6 && (result_length & 1) == 0)
		{
			j = 0;
			while (j < out->tap)
			{
				dn = IntOS2Double(n);
				phase = ((dn + 0.5) * rLength / sourceLength) - (di + 0.5);
				phase2 = ((dn + 1.5) * rLength / sourceLength) - (di + 0.5);
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos.Weight(phase) + lanczos.Weight(phase2);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if (n >= sourceMaxPos - 1)
				{
					ind1 = sourceMaxPos - 2;
					ind2 = ind1 + 1;
					work[j] = 0;
					work[j + 1] = lanczos.Weight(phase) + lanczos.Weight(phase2);
					sum += work[j + 1];
				}
				else
				{
					ind1 = n;
					ind2 = n + 1;
					work[j] = lanczos.Weight(phase);
					work[j + 1] = lanczos.Weight(phase2);
					sum += work[j] + work[j + 1];
				}
//				out->index[i * out->tap + j] = ind1 * indexSep;
//				out->index[i * out->tap + j + 1] = ind2 * indexSep;
				out->index[i * 3 + (j >> 1)] = ind1 * indexSep;
				n += 2;
				j += 2;
			}

			Int16 v1 = (Int16)(0xffff & Double2Int32((work[0] / sum) * 16384.0));
			Int16 v2 = (Int16)(0xffff & Double2Int32((work[2] / sum) * 16384.0));
			Int16 *tmpPtr = (Int16*)&out->weight[i * out->tap];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;

			v1 = (Int16)(0xffff & Double2Int32((work[1] / sum) * 16384.0));
			v2 = (Int16)(0xffff & Double2Int32((work[3] / sum) * 16384.0));
			tmpPtr += 8;
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;

			v1 = (Int16)(0xffff & Double2Int32((work[4] / sum) * 16384.0));
			v2 = (Int16)(0xffff & Double2Int32((work[5] / sum) * 16384.0));
			tmpPtr += 8;
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;
		}
		else
		{
			j = 0;
			while (j < ttap)
			{
				dn = IntOS2Double(n);
				phase = ((dn + 0.5) * rLength / sourceLength) - (di + 0.5);
				phase2 = ((dn + 1.5) * rLength / sourceLength) - (di + 0.5);
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos.Weight(phase) + lanczos.Weight(phase2);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if (n >= sourceMaxPos - 1)
				{
					ind1 = sourceMaxPos - 2;
					ind2 = ind1 + 1;
					work[j] = 0;
					work[j + 1] = lanczos.Weight(phase) + lanczos.Weight(phase2);
					sum += work[j + 1];
				}
				else
				{
					ind1 = n;
					ind2 = n + 1;
					work[j] = lanczos.Weight(phase);
					work[j + 1] = lanczos.Weight(phase2);
					sum += work[j] + work[j + 1];
				}
				out->index[i * out->tap + j] = ind1 * indexSep;
				out->index[i * out->tap + j + 1] = ind2 * indexSep;
				n += 2;
				j += 2;
			}

			j = 0;
			while (j < ttap)
			{
				Int32 v1 = Double2Int32((work[j] / sum) * 16384.0);
				Int32 v2 = Double2Int32((work[j + 1] / sum) * 16384.0);
				Int16 *tmpPtr = (Int16*)&out->weight[i * out->tap + j];
				tmpPtr[0] = (Int16)v1;
				tmpPtr[1] = (Int16)v2;
				tmpPtr[2] = (Int16)v1;
				tmpPtr[3] = (Int16)v2;
				tmpPtr[4] = (Int16)v1;
				tmpPtr[5] = (Int16)v2;
				tmpPtr[6] = (Int16)v1;
				tmpPtr[7] = (Int16)v2;
				j += 2;
			}
		}

		i++;
	}

	MemFree(work);
}
