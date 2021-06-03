#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Math/LanczosFilter.h"
#include "Math/Math.h"
#include "Media/IImgResizer.h"
#include "Media/ImageCopyC.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

#define PI 3.141592653589793

extern "C"
{
	void LanczosResizerH8_8_horizontal_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
	void LanczosResizerH8_8_horizontal_filter8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
	void LanczosResizerH8_8_vertical_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
	void LanczosResizerH8_8_expand(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
	void LanczosResizerH8_8_collapse(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);
}

void Media::Resizer::LanczosResizerH8_8::setup_interpolation_parameter_v(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	Double *work;
	Double  sum;
	Double  pos;

	out->length = result_length;
	out->tap = nTap;
	out->weight = MemAllocA(Int64, out->length * out->tap + 1);
	out->index = MemAllocA(OSInt, out->length * out->tap);

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (Math::UOSInt2Double(i) + 0.5) * source_length;
		pos = pos / Math::UOSInt2Double(result_length) + offsetCorr;
		n = (Int32)Math::Fix(pos - (Math::UOSInt2Double(nTap / 2) - 0.5));//2.5);
		pos = (Math::OSInt2Double(n) + 0.5 - pos);
		sum = 0;
	
		j = 0;
		while (j < out->tap)
		{
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if((UOSInt)n >= source_max_pos){
				out->index[i * out->tap + j] = (OSInt)(source_max_pos - 1) * indexSep;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = Math::LanczosFilter::Weight(pos, nTap);
			sum += work[j];
			pos += 1;
			n += 1;
			j++;
		}

		j = 0;
		while (j < out->tap)
		{
			Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
			out->weight[i * out->tap+j] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
			j++;
		}
		i++;
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizerH8_8::setup_decimation_parameter_v(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	Double *work;
	Double  sum;
	Double  pos, phase;

	out->length = result_length;
	out->tap = (UOSInt)Math::Fix((Math::UOSInt2Double(nTap) * (source_length) + Math::UOSInt2Double(result_length - 1)) / Math::UOSInt2Double(result_length));

	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(OSInt, out->length * out->tap);
	
	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (Math::UOSInt2Double(i) - Math::UOSInt2Double(nTap / 2) + 0.5) * source_length / Math::UOSInt2Double(result_length) + 0.5;
		n = (OSInt)floor(pos + offsetCorr);
		sum = 0;
		j = 0;
		while (j < out->tap)
		{
			phase = (Math::OSInt2Double(n) + 0.5) * Math::UOSInt2Double(result_length);
			phase /= source_length;
			phase -= (Math::UOSInt2Double(i) + 0.5);
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if((UOSInt)n >= source_max_pos){
				out->index[i * out->tap + j] = (OSInt)(source_max_pos-1) * indexSep;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = Math::LanczosFilter::Weight(phase, nTap);
			sum += work[j];
			n += 1;
			j++;
		}

		j = 0;
		while (j < out->tap)
		{
			Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
			out->weight[i * out->tap+j] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
			j++;
		}
		i++;
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizerH8_8::setup_interpolation_parameter_h(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	Double *work;
	Double  sum;
	Double  pos;

	out->length = result_length;
	out->tap = nTap;
	if ((result_length & 3) == 0 && out->tap == 8)
	{
		out->weight = MemAllocA(Int64, (out->length >> 2) * 34);
		out->index = MemAllocA(OSInt, out->length);
	}
	else
	{
		out->weight = MemAllocA(Int64, out->length * out->tap + 1);
		out->index = MemAllocA(OSInt, out->length * out->tap);
	}

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (Math::UOSInt2Double(i) + 0.5) * source_length;
		pos = pos / Math::UOSInt2Double(result_length) + offsetCorr;
		n = (Int32)Math::Fix(pos - (Math::UOSInt2Double(nTap / 2) - 0.5));//2.5);
		pos = (Math::OSInt2Double(n) + 0.5 - pos);
		sum = 0;
		if ((result_length & 3) == 0 && out->tap == 8)
		{
			UOSInt ind = 34 * (i >> 2);
			OSInt index[8];

			j = 0;
			while (j < out->tap)
			{
				work[j] = Math::LanczosFilter::Weight(pos, nTap);
				index[j] = n;
				sum += work[j];
				pos += 1;
				n++;
				j++;
			}
			while (index[0] < 0)
			{
				work[0] = work[0] + work[1];
				index[0] = index[1];
				j = 2;
				while (j < out->tap)
				{
					work[j - 1] = work[j];
					index[j - 1] = index[j];
					j++;
				}
				index[7] = 0;
				work[7] = 0.0;
			}
			while (index[7] >= (OSInt)source_max_pos)
			{
				work[7] = work[7] + work[6];
				index[7] = index[6];
				j = 6;
				while (j-- > 0)
				{
					work[j + 1] = work[j];
					index[j + 1] = index[j];
				}

				index[0] = index[1] - 1;
				work[0] = 0;
			}
			*(Int32*)&out->weight[ind] = (Int32)(index[0] * indexSep);

			j = 0;
			while (j < out->tap)
			{
				Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
				out->weight[ind+j+2] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
				j++;
			}

			pos = (Math::UOSInt2Double(i) + 1.5) * source_length;
			pos = pos / Math::UOSInt2Double(result_length) + offsetCorr;
			n = (Int32)Math::Fix(pos - (Math::UOSInt2Double(nTap / 2) - 0.5));//2.5);
			pos = (Math::OSInt2Double(n) + 0.5 - pos);
			sum = 0;

			j = 0;
			while (j < out->tap)
			{
				work[j] = Math::LanczosFilter::Weight(pos, nTap);
				index[j] = n;
				sum += work[j];
				pos += 1;
				n++;
				j++;
			}
			while (index[0] < 0)
			{
				work[0] = work[0] + work[1];
				index[0] = index[1];
				j = 2;
				while (j < out->tap)
				{
					work[j - 1] = work[j];
					index[j - 1] = index[j];
					j++;
				}
				index[7] = 0;
				work[7] = 0.0;
			}
			while (index[7] >= (OSInt)source_max_pos)
			{
				work[7] = work[7] + work[6];
				index[7] = index[6];
				j = 6;
				while (j-- > 0)
				{
					work[j + 1] = work[j];
					index[j + 1] = index[j];
				}

				index[0] = index[1] - 1;
				work[0] = 0;
			}
			((Int32*)&out->weight[ind])[1] = (Int32)(index[0] * indexSep);

			j = 0;
			while (j < out->tap)
			{
				Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
				out->weight[ind+j+10] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
				j++;
			}

			pos = (Math::UOSInt2Double(i) + 2.5) * source_length;
			pos = pos / Math::UOSInt2Double(result_length) + offsetCorr;
			n = (Int32)Math::Fix(pos - (Math::UOSInt2Double(nTap / 2) - 0.5));//2.5);
			pos = (Math::OSInt2Double(n) + 0.5 - pos);
			sum = 0;

			j = 0;
			while (j < out->tap)
			{
				work[j] = Math::LanczosFilter::Weight(pos, nTap);
				index[j] = n;
				sum += work[j];
				pos += 1;
				n++;
				j++;
			}
			while (index[0] < 0)
			{
				work[0] = work[0] + work[1];
				index[0] = index[1];
				j = 2;
				while (j < out->tap)
				{
					work[j - 1] = work[j];
					index[j - 1] = index[j];
					j++;
				}
				index[7] = 0;
				work[7] = 0.0;
			}
			while (index[7] >= (OSInt)source_max_pos)
			{
				work[7] = work[7] + work[6];
				index[7] = index[6];
				j = 6;
				while (j-- > 0)
				{
					work[j + 1] = work[j];
					index[j + 1] = index[j];
				}

				index[0] = index[1] - 1;
				work[0] = 0;
			}
			((Int32*)&out->weight[ind])[2] = (Int32)(index[0] * indexSep);

			for(j=0;j<out->tap;j++){
				Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
				out->weight[ind+j+18] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
			}

			pos = (Math::UOSInt2Double(i) + 3.5) * source_length;
			pos = pos / Math::UOSInt2Double(result_length) + offsetCorr;
			n = (Int32)Math::Fix(pos - (Math::UOSInt2Double(nTap / 2) - 0.5));//2.5);
			pos = (Math::OSInt2Double(n) + 0.5 - pos);
			sum = 0;

			j = 0;
			while (j < out->tap)
			{
				work[j] = Math::LanczosFilter::Weight(pos, nTap);
				index[j] = n;
				sum += work[j];
				pos += 1;
				n++;
				j++;
			}
			while (index[0] < 0)
			{
				work[0] = work[0] + work[1];
				index[0] = index[1];
				j = 2;
				while (j < out->tap)
				{
					work[j - 1] = work[j];
					index[j - 1] = index[j];
					j++;
				}
				index[7] = 0;
				work[7] = 0.0;
			}
			while (index[7] >= (OSInt)source_max_pos)
			{
				work[7] = work[7] + work[6];
				index[7] = index[6];
				j = 6;
				while (j-- > 0)
				{
					work[j + 1] = work[j];
					index[j + 1] = index[j];
				}

				index[0] = index[1] - 1;
				work[0] = 0;
			}
			((Int32*)&out->weight[ind])[3] = (Int32)(index[0] * indexSep);

			j = 0;
			while (j < out->tap)
			{
				Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
				out->weight[ind+j+26] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
				j++;
			}
			i += 3;
		}
		else
		{
			j = 0;
			while (j < out->tap)
			{
				if(n < 0){
					out->index[i * out->tap + j] = 0;
				}else if((UOSInt)n >= source_max_pos){
					out->index[i * out->tap + j] = (OSInt)(source_max_pos - 1) * indexSep;
				}else{
					out->index[i * out->tap + j] = n * indexSep;
				}
				work[j] = Math::LanczosFilter::Weight(pos, nTap);
				sum += work[j];
				pos += 1;
				n += 1;
				j++;
			}

			j = 0;
			while (j < out->tap)
			{
				Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
				out->weight[i * out->tap+j] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
				j++;
			}
		}
		i++;
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizerH8_8::setup_decimation_parameter_h(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	Double *work;
	Double  sum;
	Double  pos, phase;

	out->length = result_length;
	out->tap = (UOSInt)Math::Fix((Math::UOSInt2Double(nTap) * (source_length) + Math::UOSInt2Double(result_length - 1)) / Math::UOSInt2Double(result_length));

	if ((result_length & 3) == 0 && out->tap == 8)
	{
		out->weight = MemAllocA(Int64, (out->length >> 2) * 34);
		out->index = MemAllocA(OSInt, out->length);
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
		pos = (Math::UOSInt2Double(i) - Math::UOSInt2Double(nTap / 2) + 0.5) * source_length / Math::UOSInt2Double(result_length) + 0.5;
		n = (OSInt)Math::Fix(pos + offsetCorr);
		sum = 0;
		if ((result_length & 3) == 0 && out->tap == 8)
		{
			UOSInt ind = 34 * (i >> 2);
			OSInt index[8];
			j = 0;
			while (j < 8)
			{
				phase = ((Math::OSInt2Double(n) + 0.5) * Math::UOSInt2Double(result_length) / source_length) - (Math::UOSInt2Double(i) + 0.5);
				index[j] = n;
				work[j] = Math::LanczosFilter::Weight(phase, nTap);
				sum += work[j];
				n += 1;
				j++;
			}
			while (index[0] < 0)
			{
				work[0] = work[0] + work[1];
				index[0] = index[1];
				j = 2;
				while (j < out->tap)
				{
					work[j - 1] = work[j];
					index[j - 1] = index[j];
					j++;
				}
				index[7] = 0;
				work[7] = 0.0;
			}
			while (index[7] >= (OSInt)source_max_pos - 1)
			{
				work[7] = work[7] + work[6];
				index[7] = index[6];
				j = 6;
				while (j-- > 0)
				{
					work[j + 1] = work[j];
					index[j + 1] = index[j];
				}

				index[0] = index[1] - 1;
				work[0] = 0;
			}
			((Int32*)&out->weight[ind])[0] = (Int32)(index[0] * indexSep);

			j = 0;
			while (j < out->tap)
			{
				Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
				out->weight[ind + j + 2] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
				j++;
			}

			pos = (Math::UOSInt2Double(i) - Math::UOSInt2Double(nTap / 2) + 1.5) * source_length / Math::UOSInt2Double(result_length) + 0.5;
			n = (OSInt)Math::Fix(pos + offsetCorr);
			sum = 0;
			j = 0;
			while (j < 8)
			{
				phase = ((Math::OSInt2Double(n) + 0.5) * Math::UOSInt2Double(result_length) / source_length) - (Math::UOSInt2Double(i) + 1.5);
				index[j] = n;
				work[j] = Math::LanczosFilter::Weight(phase, nTap);
				sum += work[j];
				n += 1;
				j++;
			}
			while (index[0] < 0)
			{
				work[0] = work[0] + work[1];
				index[0] = index[1];
				j = 2;
				while (j < out->tap)
				{
					work[j - 1] = work[j];
					index[j - 1] = index[j];
					j++;
				}
				index[7] = 0;
				work[7] = 0.0;
			}
			while (index[7] >= (OSInt)source_max_pos - 1)
			{
				work[7] = work[7] + work[6];
				index[7] = index[6];
				j = 6;
				while (j-- > 0)
				{
					work[j + 1] = work[j];
					index[j + 1] = index[j];
				}

				index[0] = index[1] - 1;
				work[0] = 0;
			}
			((Int32*)&out->weight[ind])[1] = (Int32)(index[0] * indexSep);

			for (j = 0; j < out->tap; j++)
			{
				Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
				out->weight[ind + j + 10] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
			}

			pos = (Math::UOSInt2Double(i) - Math::UOSInt2Double(nTap / 2) + 2.5) * source_length / Math::UOSInt2Double(result_length) + 0.5;
			n = (OSInt)Math::Fix(pos + offsetCorr);
			sum = 0;
			j = 0;
			while (j < 8)
			{
				phase = ((Math::OSInt2Double(n) + 0.5) * Math::UOSInt2Double(result_length) / source_length) - (Math::UOSInt2Double(i) + 2.5);
				index[j] = n;
				work[j] = Math::LanczosFilter::Weight(phase, nTap);
				sum += work[j];
				n += 1;
				j++;
			}
			while (index[0] < 0)
			{
				work[0] = work[0] + work[1];
				index[0] = index[1];
				j = 2;
				while (j < out->tap)
				{
					work[j - 1] = work[j];
					index[j - 1] = index[j];
					j++;
				}
				index[7] = 0;
				work[7] = 0.0;
			}
			while (index[7] >= (OSInt)source_max_pos - 1)
			{
				work[7] = work[7] + work[6];
				index[7] = index[6];
				j = 6;
				while (j-- > 0)
				{
					work[j + 1] = work[j];
					index[j + 1] = index[j];
				}

				index[0] = index[1] - 1;
				work[0] = 0;
			}
			((Int32*)&out->weight[ind])[2] = (Int32)(index[0] * indexSep);

			j = 0;
			while (j < out->tap)
			{
				Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
				out->weight[ind + j + 18] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
				j++;
			}

			pos = (Math::UOSInt2Double(i) - Math::UOSInt2Double(nTap / 2) + 3.5) * source_length / Math::UOSInt2Double(result_length) + 0.5;
			n = (OSInt)Math::Fix(pos + offsetCorr);
			sum = 0;
			j = 0;
			while (j < 8)
			{
				phase = ((Math::OSInt2Double(n) + 0.5) * Math::UOSInt2Double(result_length) / source_length) - (Math::UOSInt2Double(i) + 3.5);
				index[j] = n;
				work[j] = Math::LanczosFilter::Weight(phase, nTap);
				sum += work[j];
				n += 1;
				j++;
			}
			while (index[0] < 0)
			{
				work[0] = work[0] + work[1];
				index[0] = index[1];
				j = 2;
				while (j < out->tap)
				{
					work[j - 1] = work[j];
					index[j - 1] = index[j];
					j++;
				}
				index[7] = 0;
				work[7] = 0.0;
			}
			while (index[7] >= (OSInt)source_max_pos - 1)
			{
				work[7] = work[7] + work[6];
				index[7] = index[6];
				j = 6;
				while (j-- > 0)
				{
					work[j + 1] = work[j];
					index[j + 1] = index[j];
				}

				index[0] = index[1] - 1;
				work[0] = 0;
			}
			((Int32*)&out->weight[ind])[3] = (Int32)(index[0] * indexSep);

			for (j = 0; j < out->tap; j++)
			{
				Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
				out->weight[ind + j + 26] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
			}
			i += 3;
		}
		else
		{
			j = 0;
			while (j < out->tap)
			{
				phase = (Math::OSInt2Double(n) + 0.5) * Math::UOSInt2Double(result_length);
				phase /= source_length;
				phase -= (Math::UOSInt2Double(i) + 0.5);
				if(n < 0){
					out->index[i * out->tap + j] = 0;
				}else if(n >= (OSInt)source_max_pos){
					out->index[i * out->tap + j] = (OSInt)(source_max_pos-1) * indexSep;
				}else{
					out->index[i * out->tap + j] = n * indexSep;
				}
				work[j] = Math::LanczosFilter::Weight(phase, nTap);
				sum += work[j];
				n += 1;
				j++;
			}

			j = 0;
			while (j < out->tap)
			{
				Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
				out->weight[i * out->tap+j] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
				j++;
			}
		}
		i++;
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizerH8_8::mt_horizontal_filter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	Manage::HiResClock clk;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].width = width;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].tap = tap;
		this->params[i].index = index;
		this->params[i].weight = weight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		this->params[i].funcType = 3;
		this->ptask->AddTask(DoTask, &this->params[i]);
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
	this->hTime = clk.GetTimeDiff();
}

void Media::Resizer::LanczosResizerH8_8::mt_horizontal_filter8(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	Manage::HiResClock clk;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].width = width;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].tap = tap;
		this->params[i].index = index;
		this->params[i].weight = weight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		this->params[i].funcType = 13;
		this->ptask->AddTask(DoTask, &this->params[i]);
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
	this->hTime = clk.GetTimeDiff();
}

void Media::Resizer::LanczosResizerH8_8::mt_vertical_filter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	Manage::HiResClock clk;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].width = width;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].tap = tap;
		this->params[i].index = index + currHeight * tap;
		this->params[i].weight = weight + currHeight * tap;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		this->params[i].funcType = 5;
		this->ptask->AddTask(DoTask, &this->params[i]);
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
	this->vTime = clk.GetTimeDiff();
}

void Media::Resizer::LanczosResizerH8_8::mt_expand(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].width = width;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		this->params[i].funcType = 7;
		this->ptask->AddTask(DoTask, &this->params[i]);
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();

}

void Media::Resizer::LanczosResizerH8_8::mt_collapse(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].width = width;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		this->params[i].funcType = 9;
		this->ptask->AddTask(DoTask, &this->params[i]);
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizerH8_8::mt_copy(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].width = width;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		this->params[i].funcType = 11;
		this->ptask->AddTask(DoTask, &this->params[i]);
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void __stdcall Media::Resizer::LanczosResizerH8_8::DoTask(void *obj)
{
	TaskParam *ts = (TaskParam*)obj;
	if (ts->funcType == 3)
	{
		LanczosResizerH8_8_horizontal_filter(ts->inPt, ts->outPt, ts->width, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep);
	}
	else if (ts->funcType == 5)
	{
		LanczosResizerH8_8_vertical_filter(ts->inPt, ts->outPt, ts->width, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep);
	}
	else if (ts->funcType == 7)
	{
		LanczosResizerH8_8_expand(ts->inPt, ts->outPt, ts->width, ts->height, ts->sstep, ts->dstep);
	}
	else if (ts->funcType == 9)
	{
		LanczosResizerH8_8_collapse(ts->inPt, ts->outPt, ts->width, ts->height, ts->sstep, ts->dstep);
	}
	else if (ts->funcType == 11)
	{
		ImageCopy_ImgCopy(ts->inPt, ts->outPt, ts->width << 2, ts->height, ts->sstep, ts->dstep);
	}
	else if (ts->funcType == 13)
	{
		LanczosResizerH8_8_horizontal_filter8(ts->inPt, ts->outPt, ts->width, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep);
	}
}

void Media::Resizer::LanczosResizerH8_8::DestoryHori()
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
	hsSize = 0;
}

void Media::Resizer::LanczosResizerH8_8::DestoryVert()
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
	vsSize = 0;
	vsStep = 0;
}

Media::Resizer::LanczosResizerH8_8::LanczosResizerH8_8(UOSInt hnTap, UOSInt vnTap, Media::AlphaType srcAlphaType) : Media::IImgResizer(srcAlphaType)
{
	UOSInt i;
	this->nThread = Sync::Thread::GetThreadCnt();

	if (this->nThread > 6)
	{
		this->nThread = 6;
	}

	this->hnTap = hnTap << 1;
	this->vnTap = vnTap << 1;
	this->params = MemAlloc(TaskParam, this->nThread);
	MemClear(this->params, sizeof(TaskParam) * this->nThread);
	i = nThread;
	while(i-- > 0)
	{
		this->params[i].me = this;
	}
	NEW_CLASS(this->ptask, Sync::ParallelTask(this->nThread, false));

	hsSize = 0;
	hsOfst = 0;
	hdSize = 0;
	hIndex = 0;
	hWeight = 0;
	hTap = 0;

	vsSize = 0;
	vsOfst = 0;
	vdSize = 0;
	vsStep = 0;
	vIndex = 0;
	vWeight = 0;
	vTap = 0;

	buffW = 0;
	buffH = 0;
	buffPtr = 0;
	this->hTime = 0;
	this->vTime = 0;
	NEW_CLASS(mut, Sync::Mutex());
}

Media::Resizer::LanczosResizerH8_8::~LanczosResizerH8_8()
{
	DEL_CLASS(this->ptask);
	MemFree(this->params);

	DestoryHori();
	DestoryVert();
	if (buffPtr)
	{
		MemFreeA64(buffPtr);
		buffPtr = 0;
	}
	DEL_CLASS(mut);
}

void Media::Resizer::LanczosResizerH8_8::Resize(UInt8 *src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight)
{
	LRHPARAMETER prm;
	if (dwidth < 1 || dheight < 1)
		return;

	Double w = xOfst + swidth;
	Double h = yOfst + sheight;
	UOSInt siWidth = (UOSInt)w;
	UOSInt siHeight = (UOSInt)h;
	w -= Math::UOSInt2Double(siWidth);
	h -= Math::UOSInt2Double(siHeight);
	if (w > 0)
		siWidth++;
	if (h > 0)
		siHeight++;

	if (siWidth != dwidth && siHeight != dheight)
	{
		Sync::MutexUsage mutUsage(mut);
		if (this->hsSize != swidth || this->hdSize != dwidth || this->hsOfst != xOfst)
		{
			DestoryHori();

			if (swidth > Math::UOSInt2Double(dwidth))
			{
				setup_decimation_parameter_h(this->hnTap, swidth, siWidth, dwidth, &prm, 4, xOfst);
			}
			else
			{
				setup_interpolation_parameter_h(this->hnTap, swidth, siWidth, dwidth,&prm, 4, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			hIndex = prm.index;
			hWeight = prm.weight;
			hTap = prm.tap;
		}

		if (this->vsSize != sheight || this->vdSize != dheight || this->vsStep != sbpl || this->vsOfst != yOfst)
		{
			DestoryVert();

			if (sheight > Math::UOSInt2Double(dheight))
			{
				setup_decimation_parameter_v(this->vnTap, sheight, siHeight, dheight, &prm, sbpl, yOfst);
			}
			else
			{
				setup_interpolation_parameter_v(this->vnTap, sheight, siHeight, dheight, &prm, sbpl, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			vsStep = sbpl;
			vIndex = prm.index;
			vWeight = prm.weight;
			vTap = prm.tap;
		}
		
		if (dheight != buffH || (siWidth != buffW))
		{
			if (buffPtr)
			{
				MemFreeA64(buffPtr);
				buffPtr = 0;
			}
			buffW = siWidth;
			buffH = dheight;
			buffPtr = MemAllocA64(UInt8, buffW * buffH << 3);
		}
		if (dheight < 16)
		{
			LanczosResizerH8_8_vertical_filter(src, buffPtr, siWidth, dheight, vTap, vIndex, vWeight, sbpl, (OSInt)siWidth << 3);
			LanczosResizerH8_8_horizontal_filter(buffPtr, dest, dwidth, dheight, hTap, hIndex, hWeight, (OSInt)siWidth << 3, dbpl);
		}
		else
		{
			mt_vertical_filter(src, buffPtr, siWidth, dheight, vTap, vIndex, vWeight, sbpl, (OSInt)siWidth << 3);
//			LanczosResizerH8_8_vertical_filter(src, buffPtr, siWidth, dheight, vTap, vIndex, vWeight, sbpl, siWidth << 3);
//			LanczosResizerH8_8_horizontal_filter(buffPtr, dest, dwidth, dheight, hTap,hIndex, hWeight, siWidth << 3, dbpl);
			mt_horizontal_filter(buffPtr, dest, dwidth, dheight, hTap,hIndex, hWeight, (OSInt)siWidth << 3, dbpl);
		}
		mutUsage.EndUse();
	}
	else if (siWidth != dwidth)
	{
		Sync::MutexUsage mutUsage(mut);
		if (hsSize != swidth || hdSize != dwidth || hsOfst != xOfst)
		{
			DestoryHori();

			if (swidth > Math::UOSInt2Double(dwidth))
			{
				setup_decimation_parameter_h(this->hnTap, swidth, siWidth, dwidth, &prm, 4, xOfst);
			}
			else
			{
				setup_interpolation_parameter_h(this->hnTap, swidth, siWidth, dwidth, &prm, 4, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			hIndex = prm.index;
			hWeight = prm.weight;
			hTap = prm.tap;
		}
		if (dheight != buffH || (siWidth != buffW))
		{
			if (buffPtr)
			{
				MemFreeA64(buffPtr);
				buffPtr = 0;
			}
			buffW = siWidth;
			buffH = dheight;
			buffPtr = MemAllocA64(UInt8, buffW * buffH << 3);
		}
		if (sheight < 16)
		{
			LanczosResizerH8_8_horizontal_filter8(src, dest, dwidth, siHeight, hTap, hIndex, hWeight, sbpl, dbpl);
		}
		else
		{
			mt_horizontal_filter8(src, dest, dwidth, siHeight, hTap, hIndex, hWeight, sbpl, dbpl);
		}
		mutUsage.EndUse();
	}
	else if (siHeight != dheight)
	{
		Sync::MutexUsage mutUsage(mut);
		if (vsSize != sheight || vdSize != dheight || vsStep != sbpl || vsOfst != yOfst)
		{
			DestoryVert();

			if (sheight > Math::UOSInt2Double(dheight))
			{
				setup_decimation_parameter_v(this->vnTap, sheight, siHeight, dheight, &prm, sbpl, yOfst);
			}
			else
			{
				setup_interpolation_parameter_v(this->vnTap, sheight, siHeight, dheight, &prm, sbpl, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			vsStep = sbpl;
			vIndex = prm.index;
			vWeight = prm.weight;
			vTap = prm.tap;
		}
		if (dheight != buffH || (siWidth != buffW))
		{
			if (buffPtr)
			{
				MemFreeA64(buffPtr);
				buffPtr = 0;
			}
			buffW = siWidth;
			buffH = dheight;
			buffPtr = MemAllocA64(UInt8, buffW * buffH << 3);
		}
		if (dheight < 16)
		{
			LanczosResizerH8_8_vertical_filter(src, buffPtr, siWidth, dheight, vTap, vIndex, vWeight, sbpl, (OSInt)siWidth << 3);
			LanczosResizerH8_8_collapse(buffPtr, dest, siWidth, dheight, (OSInt)siWidth << 3, dbpl);
		}
		else
		{
			mt_vertical_filter(src, buffPtr, siWidth, dheight, vTap, vIndex, vWeight, sbpl, (OSInt)siWidth << 3);
			mt_collapse(buffPtr, dest, siWidth, dheight, (OSInt)siWidth << 3, dbpl);
		}
		mutUsage.EndUse();
	}
	else
	{
		ImageCopy_ImgCopy(src, dest, siWidth << 2, dheight, sbpl, dbpl);
	}
}

Bool Media::Resizer::LanczosResizerH8_8::Resize(Media::StaticImage *srcImg, Media::StaticImage *destImg)
{
	if (srcImg->info->fourcc != 0 && srcImg->info->fourcc != *(UInt32*)"DIB")
		return false;
	if (destImg->info->fourcc != 0 && destImg->info->fourcc != *(UInt32*)"DIB")
		return false;
	if (srcImg->info->pf != Media::PF_B8G8R8A8 || destImg->info->pf != Media::PF_B8G8R8A8)
		return false;
	if (srcImg->info->fourcc == destImg->info->fourcc)
	{
		Resize(srcImg->data, srcImg->GetDataBpl(), Math::UOSInt2Double(srcImg->info->dispWidth), Math::UOSInt2Double(srcImg->info->dispHeight), 0, 0, destImg->data, destImg->GetDataBpl(), destImg->info->dispWidth, destImg->info->dispHeight);
		return true;
	}
	else
	{
		OSInt dbpl = destImg->GetDataBpl();
		Resize(srcImg->data, srcImg->GetDataBpl(), Math::UOSInt2Double(srcImg->info->dispWidth), Math::UOSInt2Double(srcImg->info->dispHeight), 0, 0, destImg->data + (OSInt)(destImg->info->storeHeight - 1) * dbpl, -dbpl, destImg->info->dispWidth, destImg->info->dispHeight);
		return true;
	}
}

Bool Media::Resizer::LanczosResizerH8_8::IsSupported(Media::FrameInfo *srcInfo)
{
	if (srcInfo->fourcc != 0)
		return false;
	if (srcInfo->pf != Media::PF_B8G8R8A8)
		return false;
	return true;
}

Media::StaticImage *Media::Resizer::LanczosResizerH8_8::ProcessToNewPartial(Media::StaticImage *srcImage, Double srcX1, Double srcY1, Double srcX2, Double srcY2)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *img;
	if (!IsSupported(srcImage->info))
		return 0;
	OSInt targetWidth = (OSInt)this->targetWidth;
	OSInt targetHeight = (OSInt)this->targetHeight;
	if (targetWidth == 0)
	{
		targetWidth = Math::Double2Int32(srcX2 - srcX1);//srcImage->info->width;
	}
	if (targetHeight == 0)
	{
		targetHeight = Math::Double2Int32(srcX2 - srcX1);//srcImage->info->height;
	}
	CalOutputSize(srcImage->info, (UOSInt)targetWidth, (UOSInt)targetHeight, &destInfo, rar);
	NEW_CLASS(img, Media::StaticImage(&destInfo));
	if (srcImage->exif)
	{
		img->exif = srcImage->exif->Clone();
	}
	Int32 tlx = (Int32)srcX1;
	Int32 tly = (Int32)srcY1;
	Resize(srcImage->data + (tlx << 2) + tly * srcImage->GetDataBpl(), srcImage->GetDataBpl(), srcX2 - srcX1, srcY2 - srcY1, srcX1 - tlx, srcY1 - tly, img->data, img->GetDataBpl(), img->info->dispWidth, img->info->dispHeight);
	if (img->exif)
	{
		img->exif->SetWidth((UInt32)img->info->dispWidth);
		img->exif->SetHeight((UInt32)img->info->dispHeight);
	}
	return img;
}

Double Media::Resizer::LanczosResizerH8_8::GetHAvgTime()
{
	return this->hTime;
}

Double Media::Resizer::LanczosResizerH8_8::GetVAvgTime()
{
	return this->vTime;
}
