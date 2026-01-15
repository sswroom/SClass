#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Math/LanczosFilter.h"
#include "Math/Math_C.h"
#include "Media/ImageResizer.h"
#include "Media/ImageCopy_C.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

#define PI 3.141592653589793

extern "C"
{
	void LanczosResizerH8_8_horizontal_filter(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep);
	void LanczosResizerH8_8_horizontal_filter8(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep);
	void LanczosResizerH8_8_vertical_filter(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep);
	void LanczosResizerH8_8_expand(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
	void LanczosResizerH8_8_collapse(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
}

void Media::Resizer::LanczosResizerH8_8::SetupInterpolationParameterV(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	Double *work;
	Double  sum;
	Double  pos;
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = nTap;
	out->weight = MemAllocA(Int64, out->length * out->tap + 1);
	out->index = MemAllocA(IntOS, out->length * out->tap);

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UIntOS2Double(i) + 0.5) * source_length;
		pos = pos / UIntOS2Double(result_length) + offsetCorr;
		n = (Int32)Math_Fix(pos - (UIntOS2Double(nTap / 2) - 0.5));//2.5);
		pos = (IntOS2Double(n) + 0.5 - pos);
		sum = 0;
	
		j = 0;
		while (j < out->tap)
		{
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if((UIntOS)n >= source_max_pos){
				out->index[i * out->tap + j] = (IntOS)(source_max_pos - 1) * indexSep;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = lanczos.Weight(pos);
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

void Media::Resizer::LanczosResizerH8_8::SetupDecimationParameterV(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	Double *work;
	Double  sum;
	Double  pos, phase;
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = (UIntOS)Math_Fix((UIntOS2Double(nTap) * (source_length) + UIntOS2Double(result_length - 1)) / UIntOS2Double(result_length));

	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(IntOS, out->length * out->tap);
	
	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UIntOS2Double(i) - UIntOS2Double(nTap / 2) + 0.5) * source_length / UIntOS2Double(result_length) + 0.5;
		n = (IntOS)floor(pos + offsetCorr);
		sum = 0;
		j = 0;
		while (j < out->tap)
		{
			phase = (IntOS2Double(n) + 0.5) * UIntOS2Double(result_length);
			phase /= source_length;
			phase -= (UIntOS2Double(i) + 0.5);
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if((UIntOS)n >= source_max_pos){
				out->index[i * out->tap + j] = (IntOS)(source_max_pos-1) * indexSep;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = lanczos.Weight(phase);
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

void Media::Resizer::LanczosResizerH8_8::SetupInterpolationParameterH(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	Double *work;
	Double  sum;
	Double  pos;
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = nTap;
	if ((result_length & 3) == 0 && out->tap == 8)
	{
		out->weight = MemAllocA(Int64, (out->length >> 2) * 34);
		out->index = MemAllocA(IntOS, out->length);
	}
	else
	{
		out->weight = MemAllocA(Int64, out->length * out->tap + 1);
		out->index = MemAllocA(IntOS, out->length * out->tap);
	}

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UIntOS2Double(i) + 0.5) * source_length;
		pos = pos / UIntOS2Double(result_length) + offsetCorr;
		n = (Int32)Math_Fix(pos - (UIntOS2Double(nTap / 2) - 0.5));//2.5);
		pos = (IntOS2Double(n) + 0.5 - pos);
		sum = 0;
		if ((result_length & 3) == 0 && out->tap == 8)
		{
			UIntOS ind = 34 * (i >> 2);
			IntOS index[8];

			j = 0;
			while (j < out->tap)
			{
				work[j] = lanczos.Weight(pos);
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
			while (index[7] >= (IntOS)source_max_pos)
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

			pos = (UIntOS2Double(i) + 1.5) * source_length;
			pos = pos / UIntOS2Double(result_length) + offsetCorr;
			n = (Int32)Math_Fix(pos - (UIntOS2Double(nTap / 2) - 0.5));//2.5);
			pos = (IntOS2Double(n) + 0.5 - pos);
			sum = 0;

			j = 0;
			while (j < out->tap)
			{
				work[j] = lanczos.Weight(pos);
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
			while (index[7] >= (IntOS)source_max_pos)
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

			pos = (UIntOS2Double(i) + 2.5) * source_length;
			pos = pos / UIntOS2Double(result_length) + offsetCorr;
			n = (Int32)Math_Fix(pos - (UIntOS2Double(nTap / 2) - 0.5));//2.5);
			pos = (IntOS2Double(n) + 0.5 - pos);
			sum = 0;

			j = 0;
			while (j < out->tap)
			{
				work[j] = lanczos.Weight(pos);
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
			while (index[7] >= (IntOS)source_max_pos)
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

			pos = (UIntOS2Double(i) + 3.5) * source_length;
			pos = pos / UIntOS2Double(result_length) + offsetCorr;
			n = (Int32)Math_Fix(pos - (UIntOS2Double(nTap / 2) - 0.5));//2.5);
			pos = (IntOS2Double(n) + 0.5 - pos);
			sum = 0;

			j = 0;
			while (j < out->tap)
			{
				work[j] = lanczos.Weight(pos);
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
			while (index[7] >= (IntOS)source_max_pos)
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
				}else if((UIntOS)n >= source_max_pos){
					out->index[i * out->tap + j] = (IntOS)(source_max_pos - 1) * indexSep;
				}else{
					out->index[i * out->tap + j] = n * indexSep;
				}
				work[j] = lanczos.Weight(pos);
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

void Media::Resizer::LanczosResizerH8_8::SetupDecimationParameterH(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	Double *work;
	Double  sum;
	Double  pos, phase;
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = (UIntOS)Math_Fix((UIntOS2Double(nTap) * (source_length) + UIntOS2Double(result_length - 1)) / UIntOS2Double(result_length));

	if ((result_length & 3) == 0 && out->tap == 8)
	{
		out->weight = MemAllocA(Int64, (out->length >> 2) * 34);
		out->index = MemAllocA(IntOS, out->length);
	}
	else
	{
		out->weight = MemAllocA(Int64, out->length * out->tap);
		out->index = MemAllocA(IntOS, out->length * out->tap);
	}
	
	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UIntOS2Double(i) - UIntOS2Double(nTap / 2) + 0.5) * source_length / UIntOS2Double(result_length) + 0.5;
		n = (IntOS)Math_Fix(pos + offsetCorr);
		sum = 0;
		if ((result_length & 3) == 0 && out->tap == 8)
		{
			UIntOS ind = 34 * (i >> 2);
			IntOS index[8];
			j = 0;
			while (j < 8)
			{
				phase = ((IntOS2Double(n) + 0.5) * UIntOS2Double(result_length) / source_length) - (UIntOS2Double(i) + 0.5);
				index[j] = n;
				work[j] = lanczos.Weight(phase);
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
			while (index[7] >= (IntOS)source_max_pos - 1)
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

			pos = (UIntOS2Double(i) - UIntOS2Double(nTap / 2) + 1.5) * source_length / UIntOS2Double(result_length) + 0.5;
			n = (IntOS)Math_Fix(pos + offsetCorr);
			sum = 0;
			j = 0;
			while (j < 8)
			{
				phase = ((IntOS2Double(n) + 0.5) * UIntOS2Double(result_length) / source_length) - (UIntOS2Double(i) + 1.5);
				index[j] = n;
				work[j] = lanczos.Weight(phase);
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
			while (index[7] >= (IntOS)source_max_pos - 1)
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

			pos = (UIntOS2Double(i) - UIntOS2Double(nTap / 2) + 2.5) * source_length / UIntOS2Double(result_length) + 0.5;
			n = (IntOS)Math_Fix(pos + offsetCorr);
			sum = 0;
			j = 0;
			while (j < 8)
			{
				phase = ((IntOS2Double(n) + 0.5) * UIntOS2Double(result_length) / source_length) - (UIntOS2Double(i) + 2.5);
				index[j] = n;
				work[j] = lanczos.Weight(phase);
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
			while (index[7] >= (IntOS)source_max_pos - 1)
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

			pos = (UIntOS2Double(i) - UIntOS2Double(nTap / 2) + 3.5) * source_length / UIntOS2Double(result_length) + 0.5;
			n = (IntOS)Math_Fix(pos + offsetCorr);
			sum = 0;
			j = 0;
			while (j < 8)
			{
				phase = ((IntOS2Double(n) + 0.5) * UIntOS2Double(result_length) / source_length) - (UIntOS2Double(i) + 3.5);
				index[j] = n;
				work[j] = lanczos.Weight(phase);
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
			while (index[7] >= (IntOS)source_max_pos - 1)
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
				phase = (IntOS2Double(n) + 0.5) * UIntOS2Double(result_length);
				phase /= source_length;
				phase -= (UIntOS2Double(i) + 0.5);
				if(n < 0){
					out->index[i * out->tap + j] = 0;
				}else if(n >= (IntOS)source_max_pos){
					out->index[i * out->tap + j] = (IntOS)(source_max_pos-1) * indexSep;
				}else{
					out->index[i * out->tap + j] = n * indexSep;
				}
				work[j] = lanczos.Weight(phase);
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

void Media::Resizer::LanczosResizerH8_8::MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep)
{
	UIntOS currHeight;
	UIntOS lastHeight = height;
	UIntOS i = this->nThread;
	Manage::HiResClock clk;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (IntOS)currHeight * sstep;
		this->params[i].outPt = outPt + (IntOS)currHeight * dstep;
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

void Media::Resizer::LanczosResizerH8_8::MTHorizontalFilter8(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep)
{
	UIntOS currHeight;
	UIntOS lastHeight = height;
	UIntOS i = this->nThread;
	Manage::HiResClock clk;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (IntOS)currHeight * sstep;
		this->params[i].outPt = outPt + (IntOS)currHeight * dstep;
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

void Media::Resizer::LanczosResizerH8_8::MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep)
{
	UIntOS currHeight;
	UIntOS lastHeight = height;
	UIntOS i = this->nThread;
	Manage::HiResClock clk;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt;
		this->params[i].outPt = outPt + (IntOS)currHeight * dstep;
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

void Media::Resizer::LanczosResizerH8_8::MTExpand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep)
{
	UIntOS currHeight;
	UIntOS lastHeight = height;
	UIntOS i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (IntOS)currHeight * sstep;
		this->params[i].outPt = outPt + (IntOS)currHeight * dstep;
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

void Media::Resizer::LanczosResizerH8_8::MTCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep)
{
	UIntOS currHeight;
	UIntOS lastHeight = height;
	UIntOS i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (IntOS)currHeight * sstep;
		this->params[i].outPt = outPt + (IntOS)currHeight * dstep;
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

void Media::Resizer::LanczosResizerH8_8::MTCopy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep)
{
	UIntOS currHeight;
	UIntOS lastHeight = height;
	UIntOS i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (IntOS)currHeight * sstep;
		this->params[i].outPt = outPt + (IntOS)currHeight * dstep;
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

void __stdcall Media::Resizer::LanczosResizerH8_8::DoTask(AnyType obj)
{
	NN<TaskParam> ts = obj.GetNN<TaskParam>();
	if (ts->funcType == 3)
	{
		LanczosResizerH8_8_horizontal_filter(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep);
	}
	else if (ts->funcType == 5)
	{
		LanczosResizerH8_8_vertical_filter(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep);
	}
	else if (ts->funcType == 7)
	{
		LanczosResizerH8_8_expand(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->sstep, ts->dstep);
	}
	else if (ts->funcType == 9)
	{
		LanczosResizerH8_8_collapse(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->sstep, ts->dstep);
	}
	else if (ts->funcType == 11)
	{
		ImageCopy_ImgCopy(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width << 2, ts->height, ts->sstep, ts->dstep);
	}
	else if (ts->funcType == 13)
	{
		LanczosResizerH8_8_horizontal_filter8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep);
	}
}

void Media::Resizer::LanczosResizerH8_8::DestoryHori()
{
	UnsafeArray<IntOS> hIndex;
	UnsafeArray<Int64> hWeight;
	if (this->hIndex.SetTo(hIndex))
	{
		MemFreeAArr(hIndex);
		this->hIndex = nullptr;
	}
	if (this->hWeight.SetTo(hWeight))
	{
		MemFreeAArr(hWeight);
		this->hWeight = nullptr;
	}
	this->hsSize = 0;
}

void Media::Resizer::LanczosResizerH8_8::DestoryVert()
{
	UnsafeArray<IntOS> vIndex;
	UnsafeArray<Int64> vWeight;
	if (this->vIndex.SetTo(vIndex))
	{
		MemFreeAArr(vIndex);
		this->vIndex = nullptr;
	}
	if (this->vWeight.SetTo(vWeight))
	{
		MemFreeAArr(vWeight);
		this->vWeight = nullptr;
	}
	this->vsSize = 0;
	this->vsStep = 0;
}

Media::Resizer::LanczosResizerH8_8::LanczosResizerH8_8(UIntOS hnTap, UIntOS vnTap, Media::AlphaType srcAlphaType) : Media::ImageResizer(srcAlphaType)
{
	UIntOS i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();

	if (this->nThread > 6)
	{
		this->nThread = 6;
	}

	this->hnTap = hnTap << 1;
	this->vnTap = vnTap << 1;
	this->params = MemAlloc(TaskParam, this->nThread);
	MemClear(this->params.Ptr(), sizeof(TaskParam) * this->nThread);
	i = nThread;
	while(i-- > 0)
	{
		this->params[i].me = *this;
	}
	NEW_CLASSNN(this->ptask, Sync::ParallelTask(this->nThread, false));

	this->hsSize = 0;
	this->hsOfst = 0;
	this->hdSize = 0;
	this->hIndex = nullptr;
	this->hWeight = nullptr;
	this->hTap = 0;

	this->vsSize = 0;
	this->vsOfst = 0;
	this->vdSize = 0;
	this->vsStep = 0;
	this->vIndex = nullptr;
	this->vWeight = nullptr;
	this->vTap = 0;

	this->buffW = 0;
	this->buffH = 0;
	this->buffPtr = nullptr;
	this->hTime = 0;
	this->vTime = 0;
}

Media::Resizer::LanczosResizerH8_8::~LanczosResizerH8_8()
{
	UnsafeArray<UInt8> buffPtr;
	this->ptask.Delete();
	MemFreeArr(this->params);

	DestoryHori();
	DestoryVert();
	if (this->buffPtr.SetTo(buffPtr))
	{
		MemFreeAArr(buffPtr);
		this->buffPtr = nullptr;
	}
}

void Media::Resizer::LanczosResizerH8_8::Resize(UnsafeArray<const UInt8> src, IntOS sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight)
{
	UnsafeArray<IntOS> hIndex;
	UnsafeArray<Int64> hWeight;
	UnsafeArray<IntOS> vIndex;
	UnsafeArray<Int64> vWeight;
	UnsafeArray<UInt8> buffPtr;
	LRHPARAMETER prm;
	if (dwidth < 1 || dheight < 1)
		return;

	Double w = xOfst + swidth;
	Double h = yOfst + sheight;
	UIntOS siWidth = (UIntOS)w;
	UIntOS siHeight = (UIntOS)h;
	w -= UIntOS2Double(siWidth);
	h -= UIntOS2Double(siHeight);
	if (w > 0)
		siWidth++;
	if (h > 0)
		siHeight++;

	if (siWidth != dwidth && siHeight != dheight)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (this->hsSize != swidth || this->hdSize != dwidth || this->hsOfst != xOfst || !this->hIndex.SetTo(hIndex) || !this->hWeight.SetTo(hWeight))
		{
			DestoryHori();

			if (swidth > UIntOS2Double(dwidth))
			{
				SetupDecimationParameterH(this->hnTap, swidth, siWidth, dwidth, prm, 4, xOfst);
			}
			else
			{
				SetupInterpolationParameterH(this->hnTap, swidth, siWidth, dwidth, prm, 4, xOfst);
			}
			this->hsSize = swidth;
			this->hdSize = dwidth;
			this->hsOfst = xOfst;
			this->hIndex = hIndex = prm.index;
			this->hWeight = hWeight = prm.weight;
			this->hTap = prm.tap;
		}

		if (this->vsSize != sheight || this->vdSize != dheight || this->vsStep != sbpl || this->vsOfst != yOfst || !this->vIndex.SetTo(vIndex) || !this->vWeight.SetTo(vWeight))
		{
			DestoryVert();

			if (sheight > UIntOS2Double(dheight))
			{
				SetupDecimationParameterV(this->vnTap, sheight, siHeight, dheight, prm, sbpl, yOfst);
			}
			else
			{
				SetupInterpolationParameterV(this->vnTap, sheight, siHeight, dheight, prm, sbpl, yOfst);
			}
			this->vsSize = sheight;
			this->vdSize = dheight;
			this->vsOfst = yOfst;
			this->vsStep = sbpl;
			this->vIndex = vIndex = prm.index;
			this->vWeight = vWeight = prm.weight;
			this->vTap = prm.tap;
		}
		
		if (dheight != this->buffH || (siWidth != this->buffW) || !this->buffPtr.SetTo(buffPtr))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = nullptr;
			}
			this->buffW = siWidth;
			this->buffH = dheight;
			this->buffPtr = buffPtr = MemAllocA64(UInt8, this->buffW * this->buffH << 3);
		}
		if (dheight < 16)
		{
			LanczosResizerH8_8_vertical_filter(src.Ptr(), buffPtr.Ptr(), siWidth, dheight, vTap, vIndex.Ptr(), vWeight.Ptr(), sbpl, (IntOS)siWidth << 3);
			LanczosResizerH8_8_horizontal_filter(buffPtr.Ptr(), dest.Ptr(), dwidth, dheight, hTap, hIndex.Ptr(), hWeight.Ptr(), (IntOS)siWidth << 3, dbpl);
		}
		else
		{
			MTVerticalFilter(src.Ptr(), buffPtr, siWidth, dheight, vTap, vIndex, vWeight, sbpl, (IntOS)siWidth << 3);
//			LanczosResizerH8_8_vertical_filter(src, buffPtr, siWidth, dheight, vTap, vIndex, vWeight, sbpl, siWidth << 3);
//			LanczosResizerH8_8_horizontal_filter(buffPtr, dest, dwidth, dheight, hTap,hIndex, hWeight, siWidth << 3, dbpl);
			MTHorizontalFilter(buffPtr, dest.Ptr(), dwidth, dheight, hTap,hIndex, hWeight, (IntOS)siWidth << 3, dbpl);
		}
		mutUsage.EndUse();
	}
	else if (siWidth != dwidth)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (hsSize != swidth || hdSize != dwidth || hsOfst != xOfst || !this->hIndex.SetTo(hIndex) || !this->hWeight.SetTo(hWeight))
		{
			DestoryHori();

			if (swidth > UIntOS2Double(dwidth))
			{
				SetupDecimationParameterH(this->hnTap, swidth, siWidth, dwidth, prm, 4, xOfst);
			}
			else
			{
				SetupInterpolationParameterH(this->hnTap, swidth, siWidth, dwidth, prm, 4, xOfst);
			}
			this->hsSize = swidth;
			this->hdSize = dwidth;
			this->hsOfst = xOfst;
			this->hIndex = hIndex = prm.index;
			this->hWeight = hWeight = prm.weight;
			this->hTap = prm.tap;
		}
		if (dheight != this->buffH || (siWidth != this->buffW) || !this->buffPtr.SetTo(buffPtr))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = nullptr;
			}
			this->buffW = siWidth;
			this->buffH = dheight;
			this->buffPtr = buffPtr = MemAllocA64(UInt8, this->buffW * this->buffH << 3);
		}
		if (sheight < 16)
		{
			LanczosResizerH8_8_horizontal_filter8(src.Ptr(), dest.Ptr(), dwidth, siHeight, hTap, hIndex.Ptr(), hWeight.Ptr(), sbpl, dbpl);
		}
		else
		{
			MTHorizontalFilter8(src.Ptr(), dest.Ptr(), dwidth, siHeight, hTap, hIndex, hWeight, sbpl, dbpl);
		}
		mutUsage.EndUse();
	}
	else if (siHeight != dheight)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (vsSize != sheight || vdSize != dheight || vsStep != sbpl || vsOfst != yOfst || !this->vIndex.SetTo(vIndex) || !this->vWeight.SetTo(vWeight))
		{
			DestoryVert();

			if (sheight > UIntOS2Double(dheight))
			{
				SetupDecimationParameterV(this->vnTap, sheight, siHeight, dheight, prm, sbpl, yOfst);
			}
			else
			{
				SetupInterpolationParameterV(this->vnTap, sheight, siHeight, dheight, prm, sbpl, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			vsStep = sbpl;
			this->vIndex = vIndex = prm.index;
			this->vWeight = vWeight = prm.weight;
			vTap = prm.tap;
		}
		if (dheight != buffH || (siWidth != buffW) || !this->buffPtr.SetTo(buffPtr)) 
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = nullptr;
			}
			this->buffW = siWidth;
			this->buffH = dheight;
			this->buffPtr = buffPtr = MemAllocA64(UInt8, this->buffW * this->buffH << 3);
		}
		if (dheight < 16)
		{
			LanczosResizerH8_8_vertical_filter(src.Ptr(), buffPtr.Ptr(), siWidth, dheight, vTap, vIndex.Ptr(), vWeight.Ptr(), sbpl, (IntOS)siWidth << 3);
			LanczosResizerH8_8_collapse(buffPtr.Ptr(), dest.Ptr(), siWidth, dheight, (IntOS)siWidth << 3, dbpl);
		}
		else
		{
			MTVerticalFilter(src.Ptr(), buffPtr, siWidth, dheight, vTap, vIndex, vWeight, sbpl, (IntOS)siWidth << 3);
			MTCollapse(buffPtr, dest.Ptr(), siWidth, dheight, (IntOS)siWidth << 3, dbpl);
		}
		mutUsage.EndUse();
	}
	else
	{
		ImageCopy_ImgCopy(src.Ptr(), dest.Ptr(), siWidth << 2, dheight, sbpl, dbpl);
	}
}

Bool Media::Resizer::LanczosResizerH8_8::Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg)
{
	if (srcImg->info.fourcc != 0 && srcImg->info.fourcc != *(UInt32*)"DIB")
		return false;
	if (destImg->info.fourcc != 0 && destImg->info.fourcc != *(UInt32*)"DIB")
		return false;
	if (srcImg->info.pf != Media::PF_B8G8R8A8 || destImg->info.pf != Media::PF_B8G8R8A8)
		return false;
	if (srcImg->info.fourcc == destImg->info.fourcc)
	{
		Resize(srcImg->data, (IntOS)srcImg->GetDataBpl(), UIntOS2Double(srcImg->info.dispSize.x), UIntOS2Double(srcImg->info.dispSize.y), 0, 0, destImg->data, (IntOS)destImg->GetDataBpl(), destImg->info.dispSize.x, destImg->info.dispSize.y);
		return true;
	}
	else
	{
		IntOS dAdd = (IntOS)destImg->GetDataBpl();
		Resize(srcImg->data, (IntOS)srcImg->GetDataBpl(), UIntOS2Double(srcImg->info.dispSize.x), UIntOS2Double(srcImg->info.dispSize.y), 0, 0, destImg->data + (IntOS)(destImg->info.storeSize.y - 1) * dAdd, -dAdd, destImg->info.dispSize.x, destImg->info.dispSize.y);
		return true;
	}
}

Bool Media::Resizer::LanczosResizerH8_8::IsSupported(NN<const Media::FrameInfo> srcInfo)
{
	if (srcInfo->fourcc != 0)
		return false;
	if (srcInfo->pf != Media::PF_B8G8R8A8)
		return false;
	return true;
}

Optional<Media::StaticImage> Media::Resizer::LanczosResizerH8_8::ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *img;
	if (srcImage->GetImageType() != Media::RasterImage::ImageType::Static || !IsSupported(srcImage->info))
		return nullptr;
	Math::Size2D<UIntOS> targetSize = this->targetSize;
	if (targetSize.x == 0)
	{
		targetSize.x = (UIntOS)Double2IntOS(srcBR.x - srcTL.x);//srcImage->info.width;
	}
	if (targetSize.y == 0)
	{
		targetSize.y = (UIntOS)Double2IntOS(srcBR.y - srcTL.y);//srcImage->info.height;
	}
	CalOutputSize(srcImage->info, targetSize, destInfo, rar);
	NEW_CLASS(img, Media::StaticImage(destInfo));
	NN<Media::EXIFData> exif;
	if (srcImage->exif.SetTo(exif))
	{
		img->exif = exif->Clone();
	}
	Int32 tlx = (Int32)srcTL.x;
	Int32 tly = (Int32)srcTL.y;
	Resize(((Media::StaticImage*)srcImage.Ptr())->data + (tlx << 2) + tly * (IntOS)srcImage->GetDataBpl(), (IntOS)srcImage->GetDataBpl(), srcBR.x - srcTL.x, srcBR.y - srcTL.y, srcTL.x - tlx, srcTL.y - tly, img->data, (IntOS)img->GetDataBpl(), img->info.dispSize.x, img->info.dispSize.y);
	if (img->exif.SetTo(exif))
	{
		exif->SetWidth((UInt32)img->info.dispSize.x);
		exif->SetHeight((UInt32)img->info.dispSize.y);
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
