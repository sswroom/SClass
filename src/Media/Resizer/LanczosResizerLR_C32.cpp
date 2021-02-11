#include "Stdafx.h"
#include <float.h>
#include "MyMemory.h"
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
#include "Sync/Thread.h"

Double Media::Resizer::LanczosResizerLR_C32::lanczos3_weight(Double phase, UOSInt nTap)
{
	Double ret;
	Double aphase = Math::Abs(phase);
	
	if(aphase < DBL_EPSILON)
	{
		return 1.0;
	}

	if ((aphase * 2) >= nTap){
		return 0.0;
	}

	ret = Math::Sin(Math::PI * phase) * Math::Sin(Math::PI * phase / nTap * 2) / (Math::PI * Math::PI * phase * phase / nTap * 2);

	return ret;
}

void Media::Resizer::LanczosResizerLR_C32::setup_interpolation_parameter(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
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
		pos = (i + 0.5) * source_length;
		pos = pos / result_length + offsetCorr;
		n = (OSInt)Math::Fix(pos - (nTap / 2 - 0.5));//2.5);
		pos = (n+0.5-pos);
		sum = 0;
		if (nTap == 6)
		{
			ind = (Int32*)&out->weight[i * 10];
			for(j = 0; j < out->tap; j++)
			{
				if(n < 0){
					ind[j] = 0;
				}else if(n >= source_max_pos){
					ind[j] = (Int32)((source_max_pos - 1) * indexSep);
				}else{
					ind[j] = (Int32)(n * indexSep);
				}
				work[j] = lanczos3_weight(pos, nTap);
				sum += work[j];
				pos += 1;
				n += 1;
			}

			j = 0;
			while (j < out->tap)
			{
				UInt16 v1 = (UInt16)(0xffff & Math::Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Math::Double2Int32((work[j + 1] / sum) * 32767.0));
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
				}else if(n >= source_max_pos){
					out->index[i * out->tap + j] = (source_max_pos - 1) * indexSep;
				}else{
					out->index[i * out->tap + j] = n * indexSep;
				}
				work[j] = lanczos3_weight(pos, nTap);
				sum += work[j];
				pos += 1;
				n += 1;
			}

			j = 0;
			while (j < out->tap)
			{
				UInt16 v1 = (UInt16)(0xffff & Math::Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Math::Double2Int32((work[j + 1] / sum) * 32767.0));
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

void Media::Resizer::LanczosResizerLR_C32::setup_decimation_parameter(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
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

	out->length = result_length;
	out->tap = (OSInt)Math::Fix((nTap * (source_length) + (result_length - 1)) / result_length);
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
		pos = ((OSInt)i - (OSInt)(nTap / 2) + 0.5) * source_length / result_length + 0.5;
		n = (Int32)Math::Fix(pos + offsetCorr);
		sum = 0;
		if (ttap == 6)
		{
			ind = (Int32*)&out->weight[i * 10];
			j = 0;
			while (j < ttap)
			{
				phase = (n+0.5)*result_length;
				phase /= source_length;
				phase -= (i+0.5);
				if(n < 0){
					ind[j] = 0;
				}else if(n >= source_max_pos){
					ind[j] = (Int32)((source_max_pos-1) * indexSep);
				}else{
					ind[j] = (Int32)(n * indexSep);
				}
				work[j] = lanczos3_weight(phase, nTap);
				sum += work[j];
				n += 1;
				j++;
			}

			j = 0;
			while (j < ttap)
			{
				UInt16 v1 = (UInt16)(0xffff & Math::Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Math::Double2Int32((work[j + 1] / sum) * 32767.0));
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
				phase = (n+0.5)*result_length;
				phase /= source_length;
				phase -= (i+0.5);
				if(n < 0){
					out->index[i * out->tap + j] = 0;
				}else if(n >= source_max_pos){
					out->index[i * out->tap + j] = (source_max_pos-1) * indexSep;
				}else{
					out->index[i * out->tap + j] = n * indexSep;
				}
				work[j] = lanczos3_weight(phase, nTap);
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
				UInt16 v1 = (UInt16)(0xffff & Math::Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Math::Double2Int32((work[j + 1] / sum) * 32767.0));
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

void Media::Resizer::LanczosResizerLR_C32::setup_interpolation_parameter_h(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
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
		pos = (i + 0.5) * source_length;
		pos = pos / result_length + offsetCorr;
		n = (Int32)Math::Fix(pos - (nTap / 2 - 0.5));//2.5);
		pos = (n + 0.5 - pos);
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
					work[j] = lanczos3_weight(pos, nTap) + lanczos3_weight(pos + 1, nTap);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if (n >= source_max_pos - 1)
				{
					ind1 = source_max_pos - 2;
					ind2 = ind1 + 1;
					work[j] = 0;
					work[j + 1] = lanczos3_weight(pos, nTap) + lanczos3_weight(pos + 1, nTap);
					sum += work[j + 1];
				}
				else
				{
					ind1 = n;
					ind2 = n + 1;
					work[j] = lanczos3_weight(pos, nTap);
					work[j + 1] = lanczos3_weight(pos + 1, nTap);
					sum += work[j] + work[j + 1];
				}
//				out->index[i * out->tap + j] = ind1 * indexSep;
//				out->index[i * out->tap + j + 1] = ind2 * indexSep;
				out->index[i * 3 + (j >> 1)] = ind1 * indexSep;
				pos += 2;
				n += 2;
				j += 2;
			}

			UInt16 v1 = (UInt16)(0xffff & Math::Double2Int32((work[0] / sum) * 32767.0));
			UInt16 v2 = (UInt16)(0xffff & Math::Double2Int32((work[2] / sum) * 32767.0));
			UInt16 *tmpPtr = (UInt16*)&out->weight[i * out->tap];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;

			v1 = (UInt16)(0xffff & Math::Double2Int32((work[1] / sum) * 32767.0));
			v2 = (UInt16)(0xffff & Math::Double2Int32((work[3] / sum) * 32767.0));
			tmpPtr += 8;
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;

			v1 = (UInt16)(0xffff & Math::Double2Int32((work[4] / sum) * 32767.0));
			v2 = (UInt16)(0xffff & Math::Double2Int32((work[5] / sum) * 32767.0));
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
			Double vTmp;
			UOSInt tmpTap;
			j = 0;
			while (j < nTap)
			{
				work[j] = lanczos3_weight(pos, nTap);
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
			while (tmpIndex[nTap - 1] >= source_max_pos)
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
				vTmp = (work[tmpTap] / sum) * 32767.0;
				if (vTmp > 32767.0)
				{
					v1 = 32767;
				}
				else if (vTmp < -32768)
				{
					v1 = -32768;
				}
				else
				{
					v1 = (UInt16)(0xffff & Math::Double2Int32(vTmp));
				}
				vTmp = (work[tmpTap + 2] / sum) * 32767.0;
				if (vTmp > 32767.0)
				{
					v2 = 32767;
				}
				else if (vTmp < -32768)
				{
					v2 = -32768;
				}
				else
				{
					v2 = (UInt16)(0xffff & Math::Double2Int32(vTmp));
				}
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				tmpPtr += 8;

				vTmp = (work[tmpTap + 1] / sum) * 32767.0;
				if (vTmp > 32767.0)
				{
					v1 = 32767;
				}
				else if (vTmp < -32768)
				{
					v1 = -32768;
				}
				else
				{
					v1 = (UInt16)(0xffff & Math::Double2Int32(vTmp));
				}
				vTmp = (work[tmpTap + 3] / sum) * 32767.0;
				if (vTmp > 32767.0)
				{
					v2 = 32767;
				}
				else if (vTmp < -32768)
				{
					v2 = -32768;
				}
				else
				{
					v2 = (UInt16)(0xffff & Math::Double2Int32(vTmp));
				}
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

			pos = (i + 1.5) * source_length;
			pos = pos / result_length + offsetCorr;
			n = (Int32)Math::Fix(pos - (nTap / 2 - 0.5));//2.5);
			pos = (n + 0.5 - pos);
			sum = 0;

			j = 0;
			while (j < nTap)
			{
				work[j] = lanczos3_weight(pos, nTap);
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
			while (tmpIndex[nTap - 1] >= source_max_pos)
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
				vTmp = (work[tmpTap] / sum) * 32767.0;
				if (vTmp > 32767.0)
				{
					v1 = 32767;
				}
				else if (vTmp < -32768)
				{
					v1 = -32768;
				}
				else
				{
					v1 = (UInt16)(0xffff & Math::Double2Int32(vTmp));
				}
				vTmp = (work[tmpTap + 2] / sum) * 32767.0;
				if (vTmp > 32767.0)
				{
					v2 = 32767;
				}
				else if (vTmp < -32768)
				{
					v2 = -32768;
				}
				else
				{
					v2 = (UInt16)(0xffff & Math::Double2Int32(vTmp));
				}
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				tmpPtr += 8;

				vTmp = (work[tmpTap + 1] / sum) * 32767.0;
				if (vTmp > 32767.0)
				{
					v1 = 32767;
				}
				else if (vTmp < -32768)
				{
					v1 = -32768;
				}
				else
				{
					v1 = (UInt16)(0xffff & Math::Double2Int32(vTmp));
				}
				vTmp = (work[tmpTap + 3] / sum) * 32767.0;
				if (vTmp > 32767.0)
				{
					v2 = 32767;
				}
				else if (vTmp < -32768)
				{
					v2 = -32768;
				}
				else
				{
					v2 = (UInt16)(0xffff & Math::Double2Int32(vTmp));
				}
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
					work[j] = lanczos3_weight(pos, nTap) + lanczos3_weight(pos + 1, nTap);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if (n >= source_max_pos - 1)
				{
					ind1 = source_max_pos - 2;
					ind2 = ind1 + 1;
					work[j] = 0;
					work[j + 1] = lanczos3_weight(pos, nTap) + lanczos3_weight(pos + 1, nTap);
					sum += work[j + 1];
				}
				else
				{
					ind1 = n;
					ind2 = n + 1;
					work[j] = lanczos3_weight(pos, nTap);
					work[j + 1] = lanczos3_weight(pos + 1, nTap);
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
				UInt16 v1 = (UInt16)(0xffff & Math::Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Math::Double2Int32((work[j + 1] / sum) * 32767.0));
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

void Media::Resizer::LanczosResizerLR_C32::setup_decimation_parameter_h(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
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
		ttap = (OSInt)Math::Fix((8 * (source_length) + (result_length - 1)) / result_length);
		if (ttap == 7 || ttap == 8 || ttap == 15 || ttap == 16)
		{
			nTap = 8;
		}
		else
		{
			nTap = 6;
		}
	}

	out->length = result_length;
	out->tap = (OSInt)Math::Fix((nTap * (source_length) + (result_length - 1)) / result_length);
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
		pos = ((OSInt)i - (OSInt)(nTap / 2) + 0.5) * source_length / result_length + 0.5;
		n = (Int32)Math::Fix(pos + offsetCorr);
		sum = 0;
		if (ttap & 1)
		{
			j = 0;
			while (j < ttap - 1)
			{
				phase = ((n + 0.5) * result_length / source_length) - (i + 0.5);
				phase2 = ((n + 1.5) * result_length / source_length) - (i + 0.5);
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos3_weight(phase, nTap) + lanczos3_weight(phase2, nTap);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if (n >= source_max_pos - 1)
				{
					ind1 = source_max_pos - 2;
					ind2 = ind1 + 1;
					work[j] = 0;
					work[j + 1] = lanczos3_weight(phase, nTap) + lanczos3_weight(phase2, nTap);
					sum += work[j + 1];
				}
				else
				{
					ind1 = n;
					ind2 = n + 1;
					work[j] = lanczos3_weight(phase, nTap);
					work[j + 1] = lanczos3_weight(phase2, nTap);
					sum += work[j] + work[j + 1];
				}
				out->index[i * out->tap + j] = ind1 * indexSep;
				out->index[i * out->tap + j + 1] = ind2 * indexSep;
				n += 2;
				j += 2;
			}
			phase = ((n + 0.5) * result_length / source_length) - (i + 0.5);
			if (n < 0)
			{
				ind1 = 0;
				ind2 = 0;
				work[j] = lanczos3_weight(phase, nTap);
				work[j + 1] = 0;
				sum += work[j];
			}
			else if (n >= source_max_pos - 1)
			{
				ind1 = source_max_pos - 2;
				ind2 = ind1 + 1;
				work[j] = 0;
				work[j + 1] = lanczos3_weight(phase, nTap);
				sum += work[j + 1];
			}
			else
			{
				ind1 = n;
				ind2 = n + 1;
				work[j] = lanczos3_weight(phase, nTap);
				work[j + 1] = 0;
				sum += work[j];
			}
			out->index[i * out->tap + j] = ind1 * indexSep;
			out->index[i * out->tap + j + 1] = ind2 * indexSep;

			j = 0;
			while (j < ttap)
			{
				UInt16 v1 = (UInt16)(0xffff & Math::Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Math::Double2Int32((work[j + 1] / sum) * 32767.0));
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
				phase = ((n + 0.5) * result_length / source_length) - (i + 0.5);
				phase2 = ((n + 1.5) * result_length / source_length) - (i + 0.5);
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos3_weight(phase, nTap) + lanczos3_weight(phase2, nTap);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if (n >= source_max_pos - 1)
				{
					ind1 = source_max_pos - 2;
					ind2 = ind1 + 1;
					work[j] = 0;
					work[j + 1] = lanczos3_weight(phase, nTap) + lanczos3_weight(phase2, nTap);
					sum += work[j + 1];
				}
				else
				{
					ind1 = n;
					ind2 = n + 1;
					work[j] = lanczos3_weight(phase, nTap);
					work[j + 1] = lanczos3_weight(phase2, nTap);
					sum += work[j] + work[j + 1];
				}
//				out->index[i * out->tap + j] = ind1 * indexSep;
//				out->index[i * out->tap + j + 1] = ind2 * indexSep;
				out->index[i * 3 + (j >> 1)] = ind1 * indexSep;
				n += 2;
				j += 2;
			}

			UInt16 v1 = (UInt16)(0xffff & Math::Double2Int32((work[0] / sum) * 32767.0));
			UInt16 v2 = (UInt16)(0xffff & Math::Double2Int32((work[2] / sum) * 32767.0));
			UInt16 *tmpPtr = (UInt16*)&out->weight[i * out->tap];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;

			v1 = (UInt16)(0xffff & Math::Double2Int32((work[1] / sum) * 32767.0));
			v2 = (UInt16)(0xffff & Math::Double2Int32((work[3] / sum) * 32767.0));
			tmpPtr += 8;
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;

			v1 = (UInt16)(0xffff & Math::Double2Int32((work[4] / sum) * 32767.0));
			v2 = (UInt16)(0xffff & Math::Double2Int32((work[5] / sum) * 32767.0));
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
				phase = ((n + 0.5) * result_length / source_length) - (i + 0.5);
				work[j] = lanczos3_weight(phase, nTap);
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
			while (tmpIndex[ttap - 1] >= source_max_pos - 1)
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
				v1 = (UInt16)(0xffff & Math::Double2Int32((work[tmpTap + 0] / sum) * 32767.0));
				v2 = (UInt16)(0xffff & Math::Double2Int32((work[tmpTap + 2] / sum) * 32767.0));
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				tmpPtr += 8;

				v1 = (UInt16)(0xffff & Math::Double2Int32((work[tmpTap + 1] / sum) * 32767.0));
				v2 = (UInt16)(0xffff & Math::Double2Int32((work[tmpTap + 3] / sum) * 32767.0));
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

			pos = ((OSInt)i - (OSInt)(nTap / 2) + 1.5) * source_length / result_length + 0.5;
			n = (Int32)Math::Fix(pos + offsetCorr);
			sum = 0;

			j = 0;
			while (j < ttap)
			{
				phase = ((n + 0.5) * result_length / source_length) - (i + 1.5);
				work[j] = lanczos3_weight(phase, nTap);
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
			while (tmpIndex[ttap - 1] >= source_max_pos - 1)
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
				v1 = (UInt16)(0xffff & Math::Double2Int32((work[tmpTap + 0] / sum) * 32767.0));
				v2 = (UInt16)(0xffff & Math::Double2Int32((work[tmpTap + 2] / sum) * 32767.0));
				tmpPtr[0] = v1;
				tmpPtr[1] = v2;
				tmpPtr[2] = v1;
				tmpPtr[3] = v2;
				tmpPtr[4] = v1;
				tmpPtr[5] = v2;
				tmpPtr[6] = v1;
				tmpPtr[7] = v2;
				tmpPtr += 8;

				v1 = (UInt16)(0xffff & Math::Double2Int32((work[tmpTap + 1] / sum) * 32767.0));
				v2 = (UInt16)(0xffff & Math::Double2Int32((work[tmpTap + 3] / sum) * 32767.0));
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
				phase = ((n + 0.5) * result_length / source_length) - (i + 0.5);
				phase2 = ((n + 1.5) * result_length / source_length) - (i + 0.5);
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos3_weight(phase, nTap) + lanczos3_weight(phase2, nTap);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if (n >= source_max_pos - 1)
				{
					ind1 = source_max_pos - 2;
					ind2 = ind1 + 1;
					work[j] = 0;
					work[j + 1] = lanczos3_weight(phase, nTap) + lanczos3_weight(phase2, nTap);
					sum += work[j + 1];
				}
				else
				{
					ind1 = n;
					ind2 = n + 1;
					work[j] = lanczos3_weight(phase, nTap);
					work[j + 1] = lanczos3_weight(phase2, nTap);
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
				UInt16 v1 = (UInt16)(0xffff & Math::Double2Int32((work[j] / sum) * 32767.0));
				UInt16 v2 = (UInt16)(0xffff & Math::Double2Int32((work[j + 1] / sum) * 32767.0));
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

Media::Resizer::LanczosResizerLR_C32::LanczosResizerLR_C32(UOSInt hnTap, UOSInt vnTap, const Media::ColorProfile *destColor, Media::ColorManagerSess *colorSess, Media::AlphaType srcAlphaType, Double srcRefLuminance, Media::PixelFormat pf) : Media::IImgResizer(srcAlphaType)
{
	this->hnTap = hnTap << 1;
	this->vnTap = vnTap << 1;
	this->srcRefLuminance = srcRefLuminance;
	this->rgbChanged = true;
	this->pf = pf;
	NEW_CLASS(this->destColor, Media::ColorProfile(destColor));
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

	NEW_CLASS(mut, Sync::Mutex());
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
	DEL_CLASS(mut);
	DEL_CLASS(this->destColor);
}

void Media::Resizer::LanczosResizerLR_C32::Resize(UInt8 *src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight)
{
	LRHPARAMETER prm;
	Double w;
	Double h;
	Int32 siWidth;
	Int32 siHeight;

	w = xOfst + swidth;
	h = yOfst + sheight;
	siWidth = (Int32)w;
	siHeight = (Int32)h;
	w -= siWidth;
	h -= siHeight;
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

	if (swidth != dwidth && sheight != dheight)
	{
		if (dwidth < this->hnTap || dheight < this->vnTap)
			return;
		Sync::MutexUsage mutUsage(mut);
		if (this->hsSize != swidth || this->hdSize != dwidth || this->hsOfst != xOfst)
		{
			DestoryHori();

			if (swidth > dwidth)
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

			if (sheight > dheight)
			{
				setup_decimation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, dwidth << 3, yOfst);
			}
			else
			{
				setup_interpolation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, dwidth << 3, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			vsStep = dwidth << 3;
			vIndex = prm.index;
			vWeight = prm.weight;
			vTap = prm.tap;
			vFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
		}
		
		action->DoHorizontalVerticalFilter(src, dest, dwidth, siHeight, dheight, hFilter, vFilter, sbpl, dbpl, this->srcAlphaType);
		mutUsage.EndUse();
	}
	else if (swidth != dwidth)
	{
		if (dwidth < this->hnTap)
			return;
		Sync::MutexUsage mutUsage(mut);
		if (hsSize != swidth || hdSize != dwidth || hsOfst != xOfst)
		{
			DestoryHori();

			if (swidth > dwidth)
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
	else if (sheight != dheight)
	{
		if (dheight < this->vnTap)
			return;
		Sync::MutexUsage mutUsage(mut);
		if (vsSize != sheight || vdSize != dheight || vsStep != sbpl || vsOfst != yOfst)
		{
			DestoryVert();

			if (sheight > dheight)
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
		action->DoVerticalFilter(src, dest, siWidth, siHeight, dheight, vFilter, sbpl, dbpl, this->srcAlphaType);
		mutUsage.EndUse();
	}
	else
	{
		Sync::MutexUsage mutUsage(mut);
		action->DoCollapse(src, dest, siWidth, dheight, sbpl, dbpl, this->srcAlphaType);
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
	this->srcRefLuminance = srcRefLuminance;
	this->rgbChanged = true;
}

Bool Media::Resizer::LanczosResizerLR_C32::IsSupported(Media::FrameInfo *srcInfo)
{
	if (srcInfo->fourcc != *(UInt32*)"LRGB")
		return false;
	return true;
}

Media::StaticImage *Media::Resizer::LanczosResizerLR_C32::ProcessToNewPartial(Media::StaticImage *srcImage, Double srcX1, Double srcY1, Double srcX2, Double srcY2)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *img;
	if (!IsSupported(srcImage->info))
		return 0;
	OSInt targetWidth = this->targetWidth;
	OSInt targetHeight = this->targetHeight;
	if (targetWidth == 0)
	{
		targetWidth = Math::Double2Int32(srcX2 - srcX1);//srcImage->info->width;
	}
	if (targetHeight == 0)
	{
		targetHeight = Math::Double2Int32(srcY2 - srcY1);//srcImage->info->height;
	}
	CalOutputSize(srcImage->info, targetWidth, targetHeight, &destInfo, rar);
	destInfo.fourcc = 0;
	destInfo.storeBPP = 32;
	destInfo.pf = this->pf;
	destInfo.color->GetPrimaries()->Set(srcImage->info->color->GetPrimaries());
	if (this->destColor->GetRTranParam()->GetTranType() != Media::CS::TRANT_VUNKNOWN && this->destColor->GetRTranParam()->GetTranType() != Media::CS::TRANT_PUNKNOWN)
	{
		destInfo.color->GetRTranParam()->Set(this->destColor->GetRTranParam());
		destInfo.color->GetGTranParam()->Set(this->destColor->GetGTranParam());
		destInfo.color->GetBTranParam()->Set(this->destColor->GetBTranParam());
	}
//	this->srcAlphaType = srcImage->info->atype;
	NEW_CLASS(img, Media::StaticImage(&destInfo));
	Int32 tlx = (Int32)srcX1;
	Int32 tly = (Int32)srcY1;
	Resize(srcImage->data + srcImage->GetDataBpl() * tly + (tlx << 3), srcImage->GetDataBpl(), srcX2 - srcX1, srcY2 - srcY1, srcX1 - tlx, srcY1 - tly, img->data, img->GetDataBpl(), destInfo.dispWidth, destInfo.dispHeight);
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
