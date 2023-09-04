#include "Stdafx.h"
#include <float.h>
#include "MyMemory.h"
#include "Math/LanczosFilter.h"
#include "Math/Math.h"
#include "Media/RGBLUTGen.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
#ifdef USE_OPENCL
#include "Media/Resizer/LanczosResizerLR_C32_OCL.h"
#else
#include "Media/Resizer/LanczosResizerLR_C32_CPU.h"
#endif 
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

void Media::Resizer::LanczosResizerLR_C32::setup_interpolation_parameter(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	Double *work;
	Double  sum;
	Double  pos;
	Int32 *ind;

	if (nTap == 0)
	{
		nTap = 6;
	}
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = nTap;
	if (nTap == 6)
	{
		out->weight = MemAllocA(Int64, out->length * 10);
		out->index = MemAllocA(OSInt, 1);
	}
	else
	{
		out->weight = MemAllocA(Int64, out->length * out->tap);
		out->index = MemAllocA(OSInt, out->length * out->tap);
	}

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UOSInt2Double(i) + 0.5) * source_length;
		pos = pos / UOSInt2Double(result_length) + offsetCorr;
		n = (OSInt)Math_Fix(pos - (UOSInt2Double(nTap / 2) - 0.5));//2.5);
		pos = (OSInt2Double(n) + 0.5 - pos);
		sum = 0;
		if (nTap == 6)
		{
			ind = (Int32*)&out->weight[i * 10];
			for(j = 0; j < out->tap; j++)
			{
				if(n < 0){
					ind[j] = 0;
				}else if((UOSInt)n >= source_max_pos){
					ind[j] = (Int32)((OSInt)(source_max_pos - 1) * indexSep);
				}else{
					ind[j] = (Int32)(n * indexSep);
				}
				work[j] = lanczos.Weight(pos);
				sum += work[j];
				pos += 1;
				n += 1;
			}

			j = 0;
			while (j < out->tap)
			{
				UInt16 v1 = (UInt16)(0xffff & Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Double2Int32((work[j + 1] / sum) * 32767.0));
				UInt16 *tmpPtr = (UInt16*)&out->weight[i * 10 + j + 4];
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
		else
		{
			for(j = 0; j < out->tap; j++)
			{
				if(n < 0){
					out->index[i * out->tap + j] = 0;
				}else if((UOSInt)n >= source_max_pos){
					out->index[i * out->tap + j] = (OSInt)(source_max_pos - 1) * indexSep;
				}else{
					out->index[i * out->tap + j] = n * indexSep;
				}
				work[j] = lanczos.Weight(pos);
				sum += work[j];
				pos += 1;
				n += 1;
			}

			j = 0;
			while (j < out->tap)
			{
				UInt16 v1 = (UInt16)(0xffff & Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Double2Int32((work[j + 1] / sum) * 32767.0));
				UInt16 *tmpPtr = (UInt16*)&out->weight[i * out->tap + j];
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
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizerLR_C32::setup_decimation_parameter(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	UOSInt ttap;
	Double *work;
	Double  sum;
	Double  pos, phase;
	Int32 *ind;

	if (nTap == 0)
	{
		nTap = 6;
	}
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = (UOSInt)Math_Fix((UOSInt2Double(nTap) * (source_length) + UOSInt2Double(result_length - 1)) / UOSInt2Double(result_length));
	ttap = out->tap;
	out->tap += out->tap & 1;

	if (out->tap == 6)
	{
		out->weight = MemAllocA(Int64, out->length * 10);
		out->index = MemAllocA(OSInt, 1);
	}
	else
	{
		out->weight = MemAllocA(Int64, out->length * out->tap);
		out->index = MemAllocA(OSInt, out->length * out->tap);
	}
	
	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UOSInt2Double(i) - UOSInt2Double(nTap / 2) + 0.5) * source_length / UOSInt2Double(result_length) + 0.5;
		n = (Int32)Math_Fix(pos + offsetCorr);
		sum = 0;
		if (ttap == 6)
		{
			ind = (Int32*)&out->weight[i * 10];
			j = 0;
			while (j < ttap)
			{
				phase = (OSInt2Double(n) + 0.5) * UOSInt2Double(result_length);
				phase /= source_length;
				phase -= (UOSInt2Double(i) + 0.5);
				if(n < 0){
					ind[j] = 0;
				}else if((UOSInt)n >= source_max_pos){
					ind[j] = (Int32)((OSInt)(source_max_pos - 1) * indexSep);
				}else{
					ind[j] = (Int32)(n * indexSep);
				}
				work[j] = lanczos.Weight(phase);
				sum += work[j];
				n += 1;
				j++;
			}

			j = 0;
			while (j < ttap)
			{
				UInt16 v1 = (UInt16)(0xffff & Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Double2Int32((work[j + 1] / sum) * 32767.0));
				UInt16 *tmpPtr = (UInt16*)&out->weight[i * 10 + j + 4];
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
		else
		{
			j = 0;
			while (j < ttap)
			{
				phase = (OSInt2Double(n) + 0.5) * UOSInt2Double(result_length);
				phase /= source_length;
				phase -= (UOSInt2Double(i) + 0.5);
				if(n < 0){
					out->index[i * out->tap + j] = 0;
				}else if((UOSInt)n >= source_max_pos){
					out->index[i * out->tap + j] = (OSInt)(source_max_pos-1) * indexSep;
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

			j = 0;
			while (j < ttap)
			{
				UInt16 v1 = (UInt16)(0xffff & Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Double2Int32((work[j + 1] / sum) * 32767.0));
				UInt16 *tmpPtr = (UInt16*)&out->weight[i * out->tap + j];
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

void Media::Resizer::LanczosResizerLR_C32::setup_interpolation_parameter_h(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	Double *work;
	Double  sum;
	Double  pos;
	OSInt ind1;
	OSInt ind2;

	if (nTap == 0)
	{
		nTap = 8;
	}
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = nTap;
	if ((result_length & 1) == 0 && (out->tap == 8 || out->tap == 16))
	{
		out->weight = MemAllocA(Int64, out->length * (out->tap + 1));
		out->index = MemAllocA(OSInt, 1);
	}
	else
	{
		out->weight = MemAllocA(Int64, out->length * out->tap);
		out->index = MemAllocA(OSInt, out->length * out->tap);
	}

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UOSInt2Double(i) + 0.5) * source_length;
		pos = pos / UOSInt2Double(result_length) + offsetCorr;
		n = (Int32)Math_Fix(pos - (UOSInt2Double(nTap / 2) - 0.5));//2.5);
		pos = (OSInt2Double(n) + 0.5 - pos);
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
				else if ((UOSInt)n >= source_max_pos - 1)
				{
					ind1 = (OSInt)source_max_pos - 2;
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

			UInt16 v1 = (UInt16)(0xffff & Double2Int32((work[0] / sum) * 32767.0));
			UInt16 v2 = (UInt16)(0xffff & Double2Int32((work[2] / sum) * 32767.0));
			UInt16 *tmpPtr = (UInt16*)&out->weight[i * out->tap];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;

			v1 = (UInt16)(0xffff & Double2Int32((work[1] / sum) * 32767.0));
			v2 = (UInt16)(0xffff & Double2Int32((work[3] / sum) * 32767.0));
			tmpPtr += 8;
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;

			v1 = (UInt16)(0xffff & Double2Int32((work[4] / sum) * 32767.0));
			v2 = (UInt16)(0xffff & Double2Int32((work[5] / sum) * 32767.0));
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
		else if ((out->tap == 8 || out->tap == 16) && (result_length & 1) == 0)
		{
			OSInt tmpIndex[16];
			UInt16 *tmpPtr;
			UInt16 v1;
			UInt16 v2;
			UOSInt tmpTap;
			j = 0;
			while (j < nTap)
			{
				work[j] = lanczos.Weight(pos);
				sum += work[j];
				tmpIndex[j] = n;
				ind1 = n;
				pos += 1;
				n++;
				j++;
			}
			while (tmpIndex[0] < 0)
			{
				work[0] = work[0] + work[1];
				tmpIndex[0] = tmpIndex[1];
				j = 2;
				while (j < nTap)
				{
					work[j - 1] = work[j];
					tmpIndex[j - 1] = tmpIndex[j];
					j++;
				}
				tmpIndex[nTap - 1] = 0;
				work[nTap - 1] = 0.0;
			}
			while (tmpIndex[nTap - 1] >= (OSInt)source_max_pos)
			{
				work[nTap - 1] = work[nTap - 1] + work[nTap - 2];
				tmpIndex[nTap - 1] = tmpIndex[nTap - 2];
				j = nTap - 2;
				while (j-- > 0)
				{
					work[j + 1] = work[j];
					tmpIndex[j + 1] = tmpIndex[j];
				}

				tmpIndex[0] = tmpIndex[1] - 1;
				work[0] = 0;
			}
			*(OSInt*)&out->weight[i * (nTap + 1)] = tmpIndex[0] * indexSep;

			tmpPtr = (UInt16*)&out->weight[i * (nTap + 1) + 2];
			tmpTap = 0;
			while (tmpTap < nTap)
			{
				v1 = (UInt16)Math::SDouble2Int16((work[tmpTap] / sum) * 32767.0);
				v2 = (UInt16)Math::SDouble2Int16((work[tmpTap + 2] / sum) * 32767.0);
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				tmpPtr += 8;

				v1 = (UInt16)Math::SDouble2Int16((work[tmpTap + 1] / sum) * 32767.0);
				v2 = (UInt16)Math::SDouble2Int16((work[tmpTap + 3] / sum) * 32767.0);
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				tmpPtr += 8;
				tmpTap += 4;
			}

			pos = (UOSInt2Double(i) + 1.5) * source_length;
			pos = pos / UOSInt2Double(result_length) + offsetCorr;
			n = (Int32)Math_Fix(pos - (UOSInt2Double(nTap / 2) - 0.5));//2.5);
			pos = (OSInt2Double(n) + 0.5 - pos);
			sum = 0;

			j = 0;
			while (j < nTap)
			{
				work[j] = lanczos.Weight(pos);
				sum += work[j];
				tmpIndex[j] = n;
				ind1 = n;
				pos += 1;
				n++;
				j++;
			}
			while (tmpIndex[0] < 0)
			{
				work[0] = work[0] + work[1];
				tmpIndex[0] = tmpIndex[1];
				j = 2;
				while (j < nTap)
				{
					work[j - 1] = work[j];
					tmpIndex[j - 1] = tmpIndex[j];
					j++;
				}
				tmpIndex[nTap - 1] = 0;
				work[nTap - 1] = 0.0;
			}
			while (tmpIndex[nTap - 1] >= (OSInt)source_max_pos)
			{
				work[nTap - 1] = work[nTap - 1] + work[nTap - 2];
				tmpIndex[nTap - 1] = tmpIndex[nTap - 2];
				j = nTap - 2;
				while (j-- > 0)
				{
					work[j + 1] = work[j];
					tmpIndex[j + 1] = tmpIndex[j];
				}

				tmpIndex[0] = tmpIndex[1] - 1;
				work[0] = 0;
			}
			*(OSInt*)&out->weight[i * (nTap + 1) + 1] = tmpIndex[0] * indexSep;

			tmpPtr = (UInt16*)&out->weight[i * (nTap + 1) + 2 + nTap];
			tmpTap = 0;
			while (tmpTap < nTap)
			{
				v1 = (UInt16)Math::SDouble2Int16((work[tmpTap] / sum) * 32767.0);
				v2 = (UInt16)Math::SDouble2Int16((work[tmpTap + 2] / sum) * 32767.0);
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				tmpPtr += 8;

				v1 = (UInt16)Math::SDouble2Int16((work[tmpTap + 1] / sum) * 32767.0);
				v2 = (UInt16)Math::SDouble2Int16((work[tmpTap + 3] / sum) * 32767.0);
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				tmpPtr += 8;
				tmpTap += 4;
			}

			i++;
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
				else if ((UOSInt)n >= source_max_pos - 1)
				{
					ind1 = (OSInt)source_max_pos - 2;
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

			j = 0;
			while (j < out->tap)
			{
				UInt16 v1 = (UInt16)(0xffff & Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Double2Int32((work[j + 1] / sum) * 32767.0));
				UInt16 *tmpPtr = (UInt16*)&out->weight[i * out->tap + j];
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

void Media::Resizer::LanczosResizerLR_C32::setup_decimation_parameter_h(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	UOSInt ttap;
	Double *work;
	Double  sum;
	Double  pos;
	Double phase;
	Double phase2;
	OSInt ind1;
	OSInt ind2;

	if (nTap == 0)
	{
		ttap = (UOSInt)Math_Fix((8 * (source_length) + UOSInt2Double(result_length - 1)) / UOSInt2Double(result_length));
		if (ttap == 7 || ttap == 8 || ttap == 15 || ttap == 16)
		{
			nTap = 8;
		}
		else
		{
			nTap = 6;
		}
	}
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = (UOSInt)Math_Fix((UOSInt2Double(nTap) * (source_length) + UOSInt2Double(result_length - 1)) / UOSInt2Double(result_length));
//	ttap = out->tap;
	out->tap += out->tap & 1;
	ttap = out->tap;

	if ((result_length & 1) == 0 && (out->tap == 8 || out->tap == 16))
	{
		out->weight = MemAllocA(Int64, out->length * (out->tap + 1));
		out->index = MemAllocA(OSInt, 1);
	}
	else
	{
		out->weight = MemAllocA(Int64, out->length * out->tap);
		out->index = MemAllocA(OSInt, out->length * out->tap);
	}
	
	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UOSInt2Double(i) - UOSInt2Double(nTap / 2) + 0.5) * source_length / UOSInt2Double(result_length) + 0.5;
		n = (Int32)Math_Fix(pos + offsetCorr);
		sum = 0;
		if (ttap & 1)
		{
			j = 0;
			while (j < ttap - 1)
			{
				phase = ((OSInt2Double(n) + 0.5) * UOSInt2Double(result_length) / source_length) - (UOSInt2Double(i) + 0.5);
				phase2 = ((OSInt2Double(n) + 1.5) * UOSInt2Double(result_length) / source_length) - (UOSInt2Double(i) + 0.5);
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos.Weight(phase) + lanczos.Weight(phase2);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if ((UOSInt)n >= source_max_pos - 1)
				{
					ind1 = (OSInt)source_max_pos - 2;
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
			phase = ((OSInt2Double(n) + 0.5) * UOSInt2Double(result_length) / source_length) - (UOSInt2Double(i) + 0.5);
			if (n < 0)
			{
				ind1 = 0;
				ind2 = 0;
				work[j] = lanczos.Weight(phase);
				work[j + 1] = 0;
				sum += work[j];
			}
			else if ((UOSInt)n >= source_max_pos - 1)
			{
				ind1 = (OSInt)source_max_pos - 2;
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
				UInt16 v1 = (UInt16)(0xffff & Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Double2Int32((work[j + 1] / sum) * 32767.0));
				UInt16 *tmpPtr = (UInt16*)&out->weight[i * out->tap + j];
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
				phase = ((OSInt2Double(n) + 0.5) * UOSInt2Double(result_length) / source_length) - (UOSInt2Double(i) + 0.5);
				phase2 = ((OSInt2Double(n) + 1.5) * UOSInt2Double(result_length) / source_length) - (UOSInt2Double(i) + 0.5);
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos.Weight(phase) + lanczos.Weight(phase2);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if ((UOSInt)n >= source_max_pos - 1)
				{
					ind1 = (OSInt)source_max_pos - 2;
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

			UInt16 v1 = (UInt16)(0xffff & Double2Int32((work[0] / sum) * 32767.0));
			UInt16 v2 = (UInt16)(0xffff & Double2Int32((work[2] / sum) * 32767.0));
			UInt16 *tmpPtr = (UInt16*)&out->weight[i * out->tap];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;

			v1 = (UInt16)(0xffff & Double2Int32((work[1] / sum) * 32767.0));
			v2 = (UInt16)(0xffff & Double2Int32((work[3] / sum) * 32767.0));
			tmpPtr += 8;
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;

			v1 = (UInt16)(0xffff & Double2Int32((work[4] / sum) * 32767.0));
			v2 = (UInt16)(0xffff & Double2Int32((work[5] / sum) * 32767.0));
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
		else if ((ttap == 8 || ttap == 16) && (result_length & 1) == 0)
		{
			OSInt tmpIndex[16];
			UInt16 v1;
			UInt16 v2;
			UInt16 *tmpPtr;
			UOSInt tmpTap;
			j = 0;
			while (j < ttap)
			{
				phase = ((OSInt2Double(n) + 0.5) * UOSInt2Double(result_length) / source_length) - (UOSInt2Double(i) + 0.5);
				work[j] = lanczos.Weight(phase);
				tmpIndex[j] = n;
				ind1 = n;
				sum += work[j];
				n++;
				j++;
			}
			while (tmpIndex[0] < 0)
			{
				work[0] = work[0] + work[1];
				tmpIndex[0] = tmpIndex[1];
				j = 2;
				while (j < ttap)
				{
					work[j - 1] = work[j];
					tmpIndex[j - 1] = tmpIndex[j];
					j++;
				}
				tmpIndex[ttap - 1] = 0;
				work[ttap - 1] = 0.0;
			}
			while (tmpIndex[ttap - 1] >= (OSInt)source_max_pos - 1)
			{
				work[ttap - 1] = work[ttap - 1] + work[ttap - 2];
				tmpIndex[ttap - 1] = tmpIndex[ttap - 2];
				j = ttap - 2;
				while (j-- > 0)
				{
					work[j + 1] = work[j];
					tmpIndex[j + 1] = tmpIndex[j];
				}

				tmpIndex[0] = tmpIndex[1] - 1;
				work[0] = 0;
			}
			*(OSInt*)&out->weight[i * (ttap + 1)] = tmpIndex[0] * indexSep;

			tmpPtr = (UInt16*)&out->weight[i * (ttap + 1) + 2];
			tmpTap = 0;
			while (tmpTap < ttap)
			{
				v1 = (UInt16)(0xffff & Double2Int32((work[tmpTap + 0] / sum) * 32767.0));
				v2 = (UInt16)(0xffff & Double2Int32((work[tmpTap + 2] / sum) * 32767.0));
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				tmpPtr += 8;

				v1 = (UInt16)(0xffff & Double2Int32((work[tmpTap + 1] / sum) * 32767.0));
				v2 = (UInt16)(0xffff & Double2Int32((work[tmpTap + 3] / sum) * 32767.0));
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				tmpPtr += 8;
				tmpTap += 4;
			}

			pos = (UOSInt2Double(i) - UOSInt2Double(nTap / 2) + 1.5) * source_length / UOSInt2Double(result_length) + 0.5;
			n = (Int32)Math_Fix(pos + offsetCorr);
			sum = 0;

			j = 0;
			while (j < ttap)
			{
				phase = ((OSInt2Double(n) + 0.5) * UOSInt2Double(result_length) / source_length) - (UOSInt2Double(i) + 1.5);
				work[j] = lanczos.Weight(phase);
				tmpIndex[j] = n;
				ind1 = n;
				sum += work[j];
				n++;
				j++;
			}
			while (tmpIndex[0] < 0)
			{
				work[0] = work[0] + work[1];
				tmpIndex[0] = tmpIndex[1];
				j = 2;
				while (j < out->tap)
				{
					work[j - 1] = work[j];
					tmpIndex[j - 1] = tmpIndex[j];
					j++;
				}
				tmpIndex[ttap - 1] = 0;
				work[ttap - 1] = 0.0;
			}
			while (tmpIndex[ttap - 1] >= (OSInt)source_max_pos - 1)
			{
				work[ttap - 1] = work[ttap - 1] + work[ttap - 2];
				tmpIndex[ttap - 1] = tmpIndex[ttap - 2];
				j = ttap - 2;
				while (j-- > 0)
				{
					work[j + 1] = work[j];
					tmpIndex[j + 1] = tmpIndex[j];
				}

				tmpIndex[0] = tmpIndex[1] - 1;
				work[0] = 0;
			}
			*(OSInt*)&out->weight[i * (ttap + 1) + 1] = tmpIndex[0] * indexSep;

			tmpPtr = (UInt16*)&out->weight[i * (ttap + 1) + 2 + ttap];
			tmpTap = 0;
			while (tmpTap < ttap)
			{
				v1 = (UInt16)(0xffff & Double2Int32((work[tmpTap + 0] / sum) * 32767.0));
				v2 = (UInt16)(0xffff & Double2Int32((work[tmpTap + 2] / sum) * 32767.0));
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				tmpPtr += 8;

				v1 = (UInt16)(0xffff & Double2Int32((work[tmpTap + 1] / sum) * 32767.0));
				v2 = (UInt16)(0xffff & Double2Int32((work[tmpTap + 3] / sum) * 32767.0));
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				tmpPtr += 8;
				tmpTap += 4;
			}

			i++;
		}
		else
		{
			j = 0;
			while (j < ttap)
			{
				phase = ((OSInt2Double(n) + 0.5) * UOSInt2Double(result_length) / source_length) - (UOSInt2Double(i) + 0.5);
				phase2 = ((OSInt2Double(n) + 1.5) * UOSInt2Double(result_length) / source_length) - (UOSInt2Double(i) + 0.5);
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos.Weight(phase) + lanczos.Weight(phase2);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if ((UOSInt)n >= source_max_pos - 1)
				{
					ind1 = (OSInt)source_max_pos - 2;
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
				UInt16 v1 = (UInt16)(0xffff & Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Double2Int32((work[j + 1] / sum) * 32767.0));
				UInt16 *tmpPtr = (UInt16*)&out->weight[i * out->tap + j];
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
 
void Media::Resizer::LanczosResizerLR_C32::UpdateRGBTable()
{
	if (this->rgbTable == 0)
	{
		this->rgbTable = MemAlloc(UInt8, 65536 * 16);
	}
	Media::RGBLUTGen lutGen(this->colorSess);
	if (this->pf == Media::PF_LE_A2B10G10R10)
	{
		lutGen.GenLARGB_A2B10G10R10(this->rgbTable, this->destColor, 14, this->srcRefLuminance);
	}
	else
	{
		lutGen.GenLARGB_B8G8R8A8(this->rgbTable, this->destColor, 14, this->srcRefLuminance);
	}
}

void Media::Resizer::LanczosResizerLR_C32::DestoryHori()
{
	if (hIndex)
	{
		MemFreeA(hIndex);
		hIndex = 0;
	}
	if (hWeight)
	{
		MemFreeA(hWeight);
		hWeight = 0;
	}
	if (this->hFilter)
	{
		this->action->DestroyHoriFilter(this->hFilter);
		this->hFilter = 0;
	}
	hsSize = 0;
}

void Media::Resizer::LanczosResizerLR_C32::DestoryVert()
{
	if (vIndex)
	{
		MemFreeA(vIndex);
		vIndex = 0;
	}
	if (vWeight)
	{
		MemFreeA(vWeight);
		vWeight = 0;
	}
	if (this->vFilter)
	{
		this->action->DestroyVertFilter(this->vFilter);
		this->vFilter = 0;
	}
	vsSize = 0;
	vsStep = 0;
}

Media::Resizer::LanczosResizerLR_C32::LanczosResizerLR_C32(UOSInt hnTap, UOSInt vnTap, NotNullPtr<const Media::ColorProfile> destColor, Media::ColorManagerSess *colorSess, Media::AlphaType srcAlphaType, Double srcRefLuminance, Media::PixelFormat pf) : Media::IImgResizer(srcAlphaType), destColor(destColor)
{
	this->hnTap = hnTap << 1;
	this->vnTap = vnTap << 1;
	this->srcRefLuminance = srcRefLuminance;
	this->rgbChanged = true;
	this->pf = pf;
	if (colorSess)
	{
		this->colorSess = colorSess;
		this->colorSess->AddHandler(this);
	}
	else
	{
		this->colorSess = 0;
	}
	this->rgbTable = 0;
#ifdef USE_OPENCL
	NEW_CLASS(this->action, Media::Resizer::LanczosResizerLR_C32_OCL());
#else
	NEW_CLASS(this->action, Media::Resizer::LanczosResizerLR_C32_CPU());
#endif

	hsSize = 0;
	hsOfst = 0;
	hdSize = 0;
	hIndex = 0;
	hWeight = 0;
	hTap = 0;
	hFilter = 0;

	vsSize = 0;
	vsOfst = 0;
	vdSize = 0;
	vsStep = 0;
	vIndex = 0;
	vWeight = 0;
	vTap = 0;
	vFilter = 0;
}

Media::Resizer::LanczosResizerLR_C32::~LanczosResizerLR_C32()
{
	if (this->colorSess)
	{
		this->colorSess->RemoveHandler(this);
	}

	DestoryHori();
	DestoryVert();
	DEL_CLASS(this->action);
	if (this->rgbTable)
	{
		MemFree(this->rgbTable);
	}
}

void Media::Resizer::LanczosResizerLR_C32::Resize(const UInt8 *src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight)
{
	LRHPARAMETER prm;
	Double w;
	Double h;
	UOSInt siWidth;
	UOSInt siHeight;

	w = xOfst + swidth;
	h = yOfst + sheight;
	siWidth = (UOSInt)w;
	siHeight = (UOSInt)h;
	w -= UOSInt2Double(siWidth);
	h -= UOSInt2Double(siHeight);
	if (w > 0.00001)
		siWidth++;
	if (h > 0.00001)
		siHeight++;

	if (siWidth <= 0 || siHeight <= 0)
		return;

	if (this->rgbChanged)
	{
		this->rgbChanged = false;
		UpdateRGBTable();
		this->action->UpdateRGBTable(this->rgbTable);
	}

	if (swidth != UOSInt2Double(dwidth) && sheight != UOSInt2Double(dheight))
	{
		if (dwidth < this->hnTap || dheight < this->vnTap)
			return;
		Sync::MutexUsage mutUsage(this->mut);
		if (this->hsSize != swidth || this->hdSize != dwidth || this->hsOfst != xOfst)
		{
			DestoryHori();

			if (swidth > UOSInt2Double(dwidth))
			{
				setup_decimation_parameter_h(this->hnTap, swidth, siWidth, dwidth, &prm, 8, xOfst);
			}
			else
			{
				setup_interpolation_parameter_h(this->hnTap, swidth, siWidth, dwidth,&prm, 8, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			hIndex = prm.index;
			hWeight = prm.weight;
			hTap = prm.tap;
			hFilter = action->CreateHoriFilter(prm.tap, prm.index, prm.weight, prm.length);
		}

		if (this->vsSize != sheight || this->vdSize != dheight || this->vsStep != (OSInt)(dwidth << 3) || this->vsOfst != yOfst)
		{
			DestoryVert();

			if (sheight > UOSInt2Double(dheight))
			{
				setup_decimation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, (OSInt)dwidth << 3, yOfst);
			}
			else
			{
				setup_interpolation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, (OSInt)dwidth << 3, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			vsStep = (OSInt)dwidth << 3;
			vIndex = prm.index;
			vWeight = prm.weight;
			vTap = prm.tap;
			vFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
		}
		
		action->DoHorizontalVerticalFilter(src, dest, dwidth, (UOSInt)siHeight, dheight, hFilter, vFilter, sbpl, dbpl, this->srcAlphaType);
		mutUsage.EndUse();
	}
	else if (swidth != UOSInt2Double(dwidth))
	{
		if (dwidth < this->hnTap)
			return;
		Sync::MutexUsage mutUsage(this->mut);
		if (hsSize != swidth || hdSize != dwidth || hsOfst != xOfst)
		{
			DestoryHori();

			if (swidth > UOSInt2Double(dwidth))
			{
				setup_decimation_parameter_h(this->hnTap, swidth, siWidth, dwidth, &prm, 8, xOfst);
			}
			else
			{
				setup_interpolation_parameter_h(this->hnTap, swidth, siWidth, dwidth, &prm, 8, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			hIndex = prm.index;
			hWeight = prm.weight;
			hTap = prm.tap;
			this->hFilter = action->CreateHoriFilter(prm.tap, prm.index, prm.weight, prm.length);
		}
		action->DoHorizontalFilterCollapse(src, dest, dwidth, dheight, this->hFilter, sbpl, dbpl, this->srcAlphaType);
		mutUsage.EndUse();
	}
	else if (sheight != UOSInt2Double(dheight))
	{
		if (dheight < this->vnTap)
			return;
		Sync::MutexUsage mutUsage(this->mut);
		if (vsSize != sheight || vdSize != dheight || vsStep != sbpl || vsOfst != yOfst)
		{
			DestoryVert();

			if (sheight > UOSInt2Double(dheight))
			{
				setup_decimation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, sbpl, yOfst);
			}
			else
			{
				setup_interpolation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, sbpl, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			vsStep = sbpl;
			vIndex = prm.index;
			vWeight = prm.weight;
			vTap = prm.tap;
			this->vFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
		}
		action->DoVerticalFilter(src, dest, (UOSInt)siWidth, (UOSInt)siHeight, dheight, vFilter, sbpl, dbpl, this->srcAlphaType);
		mutUsage.EndUse();
	}
	else
	{
		Sync::MutexUsage mutUsage(this->mut);
		action->DoCollapse(src, dest, (UOSInt)siWidth, dheight, sbpl, dbpl, this->srcAlphaType);
		mutUsage.EndUse();

	}
}

void Media::Resizer::LanczosResizerLR_C32::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
}

void Media::Resizer::LanczosResizerLR_C32::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	this->rgbChanged = true;
}

void Media::Resizer::LanczosResizerLR_C32::SetSrcRefLuminance(Double srcRefLuminance)
{
	if (this->srcRefLuminance != srcRefLuminance)
	{
		this->srcRefLuminance = srcRefLuminance;
		this->rgbChanged = true;
	}
}

Bool Media::Resizer::LanczosResizerLR_C32::IsSupported(const Media::FrameInfo *srcInfo)
{
	if (srcInfo->fourcc != *(UInt32*)"LRGB")
		return false;
	return true;
}

Media::StaticImage *Media::Resizer::LanczosResizerLR_C32::ProcessToNewPartial(const Media::Image *srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *img;
	if (srcImage->GetImageType() != Media::Image::ImageType::Static || !IsSupported(&srcImage->info))
		return 0;
	Math::Size2D<UOSInt> targeSize = this->targetSize;
	if (targeSize.x == 0)
	{
		targeSize.x = (UOSInt)Double2OSInt(srcBR.x - srcTL.x);//srcImage->info.width;
	}
	if (targeSize.y == 0)
	{
		targeSize.y = (UOSInt)Double2OSInt(srcBR.y - srcTL.y);//srcImage->info.height;
	}
	CalOutputSize(&srcImage->info, targetSize, &destInfo, rar);
	destInfo.fourcc = 0;
	destInfo.storeBPP = 32;
	destInfo.pf = this->pf;
	destInfo.color.GetPrimaries()->Set(srcImage->info.color.GetPrimariesRead());
	if (this->destColor.GetRTranParam()->GetTranType() != Media::CS::TRANT_VUNKNOWN && this->destColor.GetRTranParam()->GetTranType() != Media::CS::TRANT_PUNKNOWN)
	{
		destInfo.color.GetRTranParam()->Set(this->destColor.GetRTranParam());
		destInfo.color.GetGTranParam()->Set(this->destColor.GetGTranParam());
		destInfo.color.GetBTranParam()->Set(this->destColor.GetBTranParam());
	}
//	this->srcAlphaType = srcImage->info.atype;
	NEW_CLASS(img, Media::StaticImage(destInfo));
	Int32 tlx = (Int32)srcTL.x;
	Int32 tly = (Int32)srcTL.y;
	Resize(((Media::StaticImage*)srcImage)->data + (OSInt)srcImage->GetDataBpl() * tly + (tlx << 3), (OSInt)srcImage->GetDataBpl(), srcBR.x - srcTL.x, srcBR.y - srcTL.y, srcTL.x - tlx, srcTL.y - tly, img->data, (OSInt)img->GetDataBpl(), destInfo.dispSize.x, destInfo.dispSize.y);
	return img;
}

Double Media::Resizer::LanczosResizerLR_C32::GetHAvgTime()
{
	return this->action->GetHAvgTime();
}

Double Media::Resizer::LanczosResizerLR_C32::GetVAvgTime()
{
	return this->action->GetVAvgTime();
}

Bool Media::Resizer::LanczosResizerLR_C32::IsRGBChanged()
{
	return this->rgbChanged;
}