#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/LanczosFilter.h"
#include "Media/RGBLUTGen.h"
#include "Media/CS/TransferFunc.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

#define PI 3.141592653589793

extern "C"
{
	void LanczosResizer8_C8_horizontal_filter_pa(const UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizer8_C8_horizontal_filter(const UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizer8_C8_vertical_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizer8_C8_expand(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizer8_C8_expand_pa(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizer8_C8_collapse(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizer8_C8_imgcopy(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizer8_C8_imgcopy_pa(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
}

void Media::Resizer::LanczosResizer8_C8::setup_interpolation_parameter(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, Media::Resizer::LanczosResizer8_C8::LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	Double *work;
	Double  sum;
	Double  pos;
	Double dnTap = UOSInt2Double(nTap);
	Double posDiff = (dnTap / 2 - 0.5);//2.5);
	Double dresultLength = UOSInt2Double(result_length);
	Math::LanczosFilter lanczos(nTap);
	OSInt maxOfst = (source_max_pos - 1) * indexSep;

	out->length = result_length;
	out->tap = nTap;
	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(OSInt, out->length * out->tap);
	out->tap += out->tap & 1;

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UOSInt2Double(i) + 0.5) * source_length;
		pos = pos / dresultLength + offsetCorr;
		n = (OSInt)Math_Fix(pos - posDiff);
		pos = (OSInt2Double(n) + 0.5 - pos);
		sum = 0;
		for(j = 0; j < out->tap; j++)
		{
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if(n >= source_max_pos){
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

void Media::Resizer::LanczosResizer8_C8::setup_decimation_parameter(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, Media::Resizer::LanczosResizer8_C8::LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	UOSInt ttap;
	Double *work;
	Double  sum;
	Double  pos, phase;
	Double dnTap = UOSInt2Double(nTap);
	Double rLength = UOSInt2Double(result_length);
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = (UOSInt)Math_Fix((dnTap * (source_length) + (rLength - 1)) / rLength);
	ttap = out->tap;
	out->tap += out->tap & 1;

	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(OSInt, out->length * out->tap);
	
	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UOSInt2Double(i) - Math_Fix(dnTap / 2) + 0.5) * source_length / rLength + 0.5;
		n = (OSInt)floor(pos + offsetCorr);
		sum = 0;
		j = 0;
		while (j < ttap)
		{
			phase = (OSInt2Double(n) + 0.5)*rLength;
			phase /= source_length;
			phase -= (UOSInt2Double(i)+0.5);
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if(n >= source_max_pos){
				out->index[i * out->tap + j] = (source_max_pos-1) * indexSep;
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

void Media::Resizer::LanczosResizer8_C8::setup_interpolation_parameter_h(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, Media::Resizer::LanczosResizer8_C8::LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	Double *work;
	Double  sum;
	Double  pos;
	Double posDiff = UOSInt2Double(nTap / 2) - 0.5; //2.5
	Double dresultLength = UOSInt2Double(result_length);
	OSInt ind1;
	OSInt ind2;
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = nTap;
	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(OSInt, out->length * out->tap);

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UOSInt2Double(i) + 0.5) * source_length;
		pos = pos / dresultLength + offsetCorr;
		n = (OSInt)Math_Fix(pos - posDiff);
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
				else if (n >= source_max_pos - 1)
				{
					ind1 = source_max_pos - 2;
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
				else if (n >= source_max_pos - 1)
				{
					ind1 = source_max_pos - 2;
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

void Media::Resizer::LanczosResizer8_C8::setup_decimation_parameter_h(UOSInt nTap, Double source_length, OSInt source_max_pos, UOSInt result_length, Media::Resizer::LanczosResizer8_C8::LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	Double dn;
	Double di;
	UOSInt ttap;
	Double *work;
	Double  sum;
	Double  pos;
	Double phase;
	Double phase2;
	OSInt ind1;
	OSInt ind2;
	Double rLength = UOSInt2Double(result_length);
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = (UOSInt)Math_Fix((UOSInt2Double(nTap) * (source_length) + (rLength - 1)) / rLength);
	ttap = out->tap;
	out->tap += out->tap & 1;

	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(OSInt, out->length * out->tap);
	
	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		di = UOSInt2Double(i);
		pos = (di - UOSInt2Double(nTap / 2) + 0.5) * source_length / rLength + 0.5;
		dn = Math_Fix(pos + offsetCorr);
		n = (Int32)dn;
		sum = 0;
		if (ttap & 1)
		{
			j = 0;
			while (j < ttap - 1)
			{
				dn = OSInt2Double(n);
				phase = ((dn + 0.5) * rLength / source_length) - (di + 0.5);
				phase2 = ((dn + 1.5) * rLength / source_length) - (di + 0.5);
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos.Weight(phase) + lanczos.Weight(phase2);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if (n >= source_max_pos - 1)
				{
					ind1 = source_max_pos - 2;
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
			phase = ((dn + 0.5) * rLength / source_length) - (di + 0.5);
			if (n < 0)
			{
				ind1 = 0;
				ind2 = 0;
				work[j] = lanczos.Weight(phase);
				work[j + 1] = 0;
				sum += work[j];
			}
			else if (n >= source_max_pos - 1)
			{
				ind1 = source_max_pos - 2;
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
				dn = OSInt2Double(n);
				phase = ((dn + 0.5) * rLength / source_length) - (di + 0.5);
				phase2 = ((dn + 1.5) * rLength / source_length) - (di + 0.5);
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos.Weight(phase) + lanczos.Weight(phase2);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if (n >= source_max_pos - 1)
				{
					ind1 = source_max_pos - 2;
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
				dn = OSInt2Double(n);
				phase = ((dn + 0.5) * rLength / source_length) - (di + 0.5);
				phase2 = ((dn + 1.5) * rLength / source_length) - (di + 0.5);
				if (n < 0)
				{
					ind1 = 0;
					ind2 = 0;
					work[j] = lanczos.Weight(phase) + lanczos.Weight(phase2);
					work[j + 1] = 0;
					sum += work[j];
				}
				else if (n >= source_max_pos - 1)
				{
					ind1 = source_max_pos - 2;
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

void Media::Resizer::LanczosResizer8_C8::mt_horizontal_filter_pa(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UOSInt swidth)
{
#if defined(VERBOSE)
	printf("LR8_C8: HFilterPA w = %d, h = %d, tap = %d\r\n", (UInt32)dwidth, (UInt32)height, (UInt32)tap);
#endif
	UOSInt currHeight;
	UOSInt lastHeight = height;
	FuncType funcType = FuncType::HFilterPA;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].dwidth = dwidth;
		this->params[i].swidth = swidth;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].tap = tap;
		this->params[i].index = index;
		this->params[i].weight = weight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		if (this->params[i].swidth != 0 && this->params[i].height != 0)
		{
			this->params[i].funcType = funcType;
			this->ptask->AddTask(DoTask, &this->params[i]);
		}
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizer8_C8::mt_horizontal_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UOSInt swidth)
{
#if defined(VERBOSE)
	printf("LR8_C8: HFilter w = %d, h = %d, tap = %d\r\n", (UInt32)dwidth, (UInt32)height, (UInt32)tap);
#endif
	UOSInt currHeight;
	UOSInt lastHeight = height;
	FuncType funcType = FuncType::HFilter;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].dwidth = dwidth;
		this->params[i].swidth = swidth;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].tap = tap;
		this->params[i].index = index;
		this->params[i].weight = weight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		if (this->params[i].swidth != 0 && this->params[i].height != 0)
		{
			this->params[i].funcType = funcType;
			this->ptask->AddTask(DoTask, &this->params[i]);
		}
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizer8_C8::mt_vertical_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
#if defined(VERBOSE)
	printf("LR8_C8: VFilter w = %d, h = %d, tap = %d\r\n", (UInt32)dwidth, (UInt32)height, (UInt32)tap);
#endif
	UOSInt currHeight;
	UOSInt lastHeight = height;
	FuncType funcType = FuncType::VFilter;
	UOSInt i = this->nThread;
	if (height < 16)
	{
		LanczosResizer8_C8_vertical_filter(inPt.Ptr(), outPt.Ptr(), dwidth, height, tap, index, weight, sstep, dstep, this->rgbTable.Ptr());
	}
	else
	{
		while (i-- > 0)
		{
			currHeight = MulDivUOS(i, height, this->nThread);
			this->params[i].inPt = inPt;
			this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
			this->params[i].dwidth = dwidth;
			this->params[i].height = lastHeight - currHeight;
			this->params[i].tap = tap;
			this->params[i].index = index + currHeight * tap;
			this->params[i].weight = weight + currHeight * tap;
			this->params[i].sstep = sstep;
			this->params[i].dstep = dstep;

			this->params[i].funcType = funcType;
			this->ptask->AddTask(DoTask, &this->params[i]);
			lastHeight = currHeight;
		}
		this->ptask->WaitForIdle();
	}
}

void Media::Resizer::LanczosResizer8_C8::mt_expand_pa(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
	LanczosResizer8_C8_expand_pa(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr());
}

void Media::Resizer::LanczosResizer8_C8::mt_expand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
	LanczosResizer8_C8_expand(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr());
}

void Media::Resizer::LanczosResizer8_C8::mt_collapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
	LanczosResizer8_C8_collapse(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr());
}

void Media::Resizer::LanczosResizer8_C8::mt_copy_pa(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
	LanczosResizer8_C8_imgcopy_pa(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr());
}

void Media::Resizer::LanczosResizer8_C8::mt_copy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
	LanczosResizer8_C8_imgcopy(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr());
}

void Media::Resizer::LanczosResizer8_C8::UpdateRGBTable()
{
	UnsafeArray<UInt8> rgbTable;
	if (!this->rgbTable.SetTo(rgbTable))
	{
		this->rgbTable = rgbTable = MemAllocArr(UInt8, 65536 * 4 + 256 * 4 * 8);
	}
	Media::RGBLUTGen lutGen(this->colorSess);
	lutGen.GenLRGB_BGRA8(rgbTable, this->destProfile, 14, Media::CS::TransferFunc::GetRefLuminance(this->srcProfile.rtransfer));
	lutGen.GenRGBA8_LRGBC((Int64*)&rgbTable[262144], this->srcProfile, this->destProfile.GetPrimaries(), 14);
}

void __stdcall Media::Resizer::LanczosResizer8_C8::DoTask(AnyType obj)
{
	NN<TaskParam> ts = obj.GetNN<TaskParam>();
	switch (ts->funcType)
	{
	case FuncType::HFilter:
		if (ts->swidth != ts->tmpbuffSize)
		{
			if (ts->tmpbuff)
				MemFreeA(ts->tmpbuff);
			ts->tmpbuffSize = ts->swidth;
			ts->tmpbuff = MemAllocA(UInt8, ts->swidth << 3);
		}
		LanczosResizer8_C8_horizontal_filter(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->swidth, ts->tmpbuff);
		break;
	case FuncType::Expand:
		LanczosResizer8_C8_expand(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		break;
	case FuncType::HFilterPA:
		if (ts->swidth != ts->tmpbuffSize)
		{
			if (ts->tmpbuff)
				MemFreeA(ts->tmpbuff);
			ts->tmpbuffSize = ts->swidth;
			ts->tmpbuff = MemAllocA(UInt8, ts->swidth << 3);
		}
		LanczosResizer8_C8_horizontal_filter_pa(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->swidth, ts->tmpbuff);
		break;
	case FuncType::ExpandPA:
		LanczosResizer8_C8_expand_pa(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		break;
	case FuncType::VFilter:
		LanczosResizer8_C8_vertical_filter(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		break;
	case FuncType::Collapse:
		LanczosResizer8_C8_collapse(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		break;
	case FuncType::ImgCopy:
		LanczosResizer8_C8_imgcopy(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		break;
	case FuncType::ImgCopyPA:
		LanczosResizer8_C8_imgcopy_pa(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		break;
	default:
	case FuncType::NoFunction:
		break;
	}
}

void Media::Resizer::LanczosResizer8_C8::DestoryHori()
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

void Media::Resizer::LanczosResizer8_C8::DestoryVert()
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

Media::Resizer::LanczosResizer8_C8::LanczosResizer8_C8(UOSInt hnTap, UOSInt vnTap, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess, Media::AlphaType srcAlphaType) : Media::ImageResizer(srcAlphaType), srcProfile(srcProfile), destProfile(destProfile)
{
	UOSInt i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();
	if (this->nThread > 6)
	{
		this->nThread = 6;
	}

	this->hnTap = hnTap << 1;
	this->vnTap = vnTap << 1;
	this->rgbChanged = true;
	this->rgbTable = 0;
	NN<Media::ColorManagerSess> nncolorSess;
	if (colorSess.SetTo(nncolorSess))
	{
		this->colorSess = nncolorSess;
		nncolorSess->AddHandler(*this);
	}
	else
	{
		this->colorSess = 0;
	}

	this->params = MemAlloc(Media::Resizer::LanczosResizer8_C8::TaskParam, this->nThread);
	MemClear(this->params, sizeof(Media::Resizer::LanczosResizer8_C8::TaskParam) * this->nThread);
	i = nThread;
	while(i-- > 0)
	{
		this->params[i].me = *this;
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
}

Media::Resizer::LanczosResizer8_C8::~LanczosResizer8_C8()
{
	UOSInt i;
	NN<Media::ColorManagerSess> nncolorSess;
	UnsafeArray<UInt8> rgbTable;
	if (this->colorSess.SetTo(nncolorSess))
	{
		nncolorSess->RemoveHandler(*this);
	}
	DEL_CLASS(this->ptask);
	i = this->nThread;
	while (i-- > 0)
	{
		if (this->params[i].tmpbuff)
		{
			MemFreeA(this->params[i].tmpbuff);
			this->params[i].tmpbuff = 0;
			this->params[i].tmpbuffSize = 0;
		}
	}
	MemFree(this->params);

	DestoryHori();
	DestoryVert();
	if (buffPtr)
	{
		MemFreeA(buffPtr);
		buffPtr = 0;
	}
	if (this->rgbTable.SetTo(rgbTable))
	{
		MemFreeArr(rgbTable);
	}
}

void Media::Resizer::LanczosResizer8_C8::Resize(UnsafeArray<const UInt8> src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight)
{
	Media::Resizer::LanczosResizer8_C8::LRHPARAMETER prm;
	Double w;
	Double h;
	OSInt siWidth;
	OSInt siHeight;
	if (dwidth < this->hnTap || dheight < this->vnTap)
		return;

#if defined(VERBOSE)
	printf("LR8_C8: Resize %lf x %lf -> %d x %d\r\n", swidth, sheight, (UInt32)dwidth, (UInt32)dheight);
#endif
	w = xOfst + swidth;
	h = yOfst + sheight;
	siWidth = (OSInt)w;
	siHeight = (OSInt)h;
	w -= OSInt2Double(siWidth);
	h -= OSInt2Double(siHeight);
	if (w > 0)
		siWidth++;
	if (h > 0)
		siHeight++;

	if (siWidth < (OSInt)this->hnTap || siHeight < (OSInt)this->vnTap)
		return;

	if (this->rgbChanged)
	{
		this->rgbChanged = false;
		UpdateRGBTable();
	}

	if (siWidth != (OSInt)dwidth && siHeight != (OSInt)dheight)
	{
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
		}

		if (this->vsSize != sheight || this->vdSize != dheight || this->vsStep != (OSInt)hdSize * 8 || this->vsOfst != yOfst)
		{
			DestoryVert();
			vsStep = (OSInt)hdSize * 8;

			if (sheight > UOSInt2Double(dheight))
			{
				setup_decimation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, vsStep, yOfst);
			}
			else
			{
				setup_interpolation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, vsStep, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			vIndex = prm.index;
			vWeight = prm.weight;
			vTap = prm.tap;
		}
		
		if (siHeight != buffH || (dwidth != buffW))
		{
			if (buffPtr)
			{
				MemFreeA(buffPtr);
				buffPtr = 0;
			}
			buffW = dwidth;
			buffH = siHeight;
			buffPtr = MemAllocA(UInt8, buffW * (UOSInt)buffH << 3);
		}
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			mt_horizontal_filter_pa(src, buffPtr, dwidth, (UOSInt)siHeight, hTap,hIndex, hWeight, sbpl, (OSInt)dwidth << 3, (UOSInt)siWidth);
/*			UInt8 *tmpbuff = MemAllocA(UInt8, siWidth << 3);
			LanczosResizer8_C8_horizontal_filter_pa(src, buffPtr, dwidth, siHeight, hTap, hIndex, hWeight, sbpl, dwidth << 3, this->rgbTable, siWidth, tmpbuff);
			MemFreeA(tmpbuff);*/
		}
		else
		{
			mt_horizontal_filter(src, buffPtr, dwidth, (UOSInt)siHeight, hTap, hIndex, hWeight, sbpl, (OSInt)dwidth << 3, (UOSInt)siWidth);
/*			UInt8 *tmpbuff = MemAllocA(UInt8, (UOSInt)siWidth << 3);
			LanczosResizer8_C8_horizontal_filter(src, buffPtr, dwidth, (UOSInt)siHeight, hTap, hIndex, hWeight, sbpl, (OSInt)dwidth << 3, this->rgbTable, (UOSInt)siWidth, tmpbuff);
			MemFreeA(tmpbuff);*/
		}
		mt_vertical_filter(buffPtr, dest, dwidth, dheight, vTap, vIndex, vWeight, (OSInt)dwidth << 3, dbpl);
//		LanczosResizer8_C8_vertical_filter(buffPtr, dest, dwidth, dheight, vTap, vIndex, vWeight, dwidth << 3, dbpl, this->rgbTable);
		mutUsage.EndUse();
	}
	else if (siWidth != (OSInt)dwidth)
	{
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
		}
		if (siHeight != buffH || (dwidth != buffW))
		{
			if (buffPtr)
			{
				MemFreeA(buffPtr);
				buffPtr = 0;
			}
			buffW = dwidth;
			buffH = siHeight;
			buffPtr = MemAllocA(UInt8, buffW * (UOSInt)buffH << 3);
		}
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			mt_horizontal_filter_pa(src, buffPtr, dwidth, (UOSInt)siHeight, hTap, hIndex, hWeight, sbpl, (OSInt)dwidth << 3, (UOSInt)siWidth);
		}
		else
		{
			mt_horizontal_filter(src, buffPtr, dwidth, (UOSInt)siHeight, hTap, hIndex, hWeight, sbpl, (OSInt)dwidth << 3, (UOSInt)siWidth);
		}
		mt_collapse(buffPtr, dest, dwidth, dheight, (OSInt)dwidth << 3, dbpl);
		mutUsage.EndUse();
	}
	else if (siHeight != (OSInt)dheight)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (vsSize != sheight || vdSize != dheight || vsStep != Double2Int32(swidth) * 8 || vsOfst != yOfst)
		{
			DestoryVert();
			vsStep = siWidth * 8;

			if (sheight > UOSInt2Double(dheight))
			{
				setup_decimation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, vsStep, yOfst);
			}
			else
			{
				setup_interpolation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, vsStep, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			vIndex = prm.index;
			vWeight = prm.weight;
			vTap = prm.tap;
		}
		if (siHeight != buffH || (siWidth != (OSInt)buffW))
		{
			if (buffPtr)
			{
				MemFreeA(buffPtr);
				buffPtr = 0;
			}
			buffW = (UOSInt)siWidth;
			buffH = siHeight;
			buffPtr = MemAllocA(UInt8, buffW * (UOSInt)buffH << 3);
		}
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			mt_expand_pa(src, buffPtr, (UOSInt)siWidth, (UOSInt)siHeight, sbpl, siWidth << 3);
		}
		else
		{
			mt_expand(src, buffPtr, (UOSInt)siWidth, (UOSInt)siHeight, sbpl, siWidth << 3);
		}
		mt_vertical_filter(buffPtr, dest, (UOSInt)siWidth, dheight, vTap, vIndex, vWeight, siWidth << 3, dbpl);
		mutUsage.EndUse();
	}
	else
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			mt_copy_pa(src, dest, (UOSInt)siWidth, dheight, sbpl, dbpl);
		}
		else
		{
			mt_copy(src, dest, (UOSInt)siWidth, dheight, sbpl, dbpl);
		}
		mutUsage.EndUse();

	}
}

Bool Media::Resizer::LanczosResizer8_C8::Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg)
{
	if (srcImg->info.fourcc != 0 && srcImg->info.fourcc != *(UInt32*)"DIB")
		return false;
	if (destImg->info.fourcc != 0 && destImg->info.fourcc != *(UInt32*)"DIB")
		return false;
	if (srcImg->info.pf != Media::PF_B8G8R8A8 || destImg->info.pf != Media::PF_B8G8R8A8)
		return false;
	//destImg->info.color.rgbGamma = srcImg->info.color.rgbGamma;
	SetDestProfile(destImg->info.color);
	SetSrcProfile(srcImg->info.color);
	if (srcImg->info.fourcc == destImg->info.fourcc)
	{
		Resize(srcImg->data.Ptr(), (OSInt)srcImg->GetDataBpl(), UOSInt2Double(srcImg->info.dispSize.x), UOSInt2Double(srcImg->info.dispSize.y), 0, 0, destImg->data.Ptr(), (OSInt)destImg->GetDataBpl(), destImg->info.dispSize.x, destImg->info.dispSize.y);
		return true;
	}
	else
	{
		OSInt dbpl = (OSInt)destImg->GetDataBpl();
		Resize(srcImg->data.Ptr(), (OSInt)srcImg->GetDataBpl(), UOSInt2Double(srcImg->info.dispSize.x), UOSInt2Double(srcImg->info.dispSize.y), 0, 0, destImg->data.Ptr() + (OSInt)(destImg->info.storeSize.y - 1) * dbpl, -dbpl, destImg->info.dispSize.x, destImg->info.dispSize.y);
		return true;
	}
}

void Media::Resizer::LanczosResizer8_C8::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void Media::Resizer::LanczosResizer8_C8::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	this->rgbChanged = true;
}

void Media::Resizer::LanczosResizer8_C8::SetSrcProfile(NN<const Media::ColorProfile> srcProfile)
{
	if (!this->srcProfile.Equals(srcProfile))
	{
		this->srcProfile.Set(srcProfile);
		this->rgbChanged = true;
	}
}

void Media::Resizer::LanczosResizer8_C8::SetDestProfile(NN<const Media::ColorProfile> destProfile)
{
	if (!this->destProfile.Equals(destProfile))
	{
		this->destProfile.Set(destProfile);
		this->rgbChanged = true;
	}
}

Media::AlphaType Media::Resizer::LanczosResizer8_C8::GetDestAlphaType()
{
	if (this->srcAlphaType == Media::AT_NO_ALPHA)
	{
		return Media::AT_NO_ALPHA;
	}
	else
	{
		return Media::AT_PREMUL_ALPHA;
	}
}

Bool Media::Resizer::LanczosResizer8_C8::IsSupported(NN<const Media::FrameInfo> srcInfo)
{
	if (srcInfo->fourcc != 0)
		return false;
	if (srcInfo->pf != Media::PF_B8G8R8A8)
		return false;
	return true;
}

Media::StaticImage *Media::Resizer::LanczosResizer8_C8::ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *newImage;
	if (srcImage->GetImageType() != Media::RasterImage::ImageType::Static || !IsSupported(srcImage->info))
	{
		return 0;
	}
	Math::Size2D<UOSInt> targetSize = this->targetSize;
	if (targetSize.x == 0)
	{
		targetSize.x = (UOSInt)Double2Int32(srcBR.x - srcTL.x);
	}
	if (targetSize.y == 0)
	{
		targetSize.y = (UOSInt)Double2Int32(srcBR.y - srcTL.y);
	}
	CalOutputSize(srcImage->info, targetSize, destInfo, this->rar);
	this->SetSrcProfile(srcImage->info.color);
	this->SetSrcAlphaType(srcImage->info.atype);
	if (this->destProfile.GetRTranParam()->GetTranType() != Media::CS::TRANT_VUNKNOWN && this->destProfile.GetRTranParam()->GetTranType() != Media::CS::TRANT_PUNKNOWN)
	{
		destInfo.color.GetRTranParam()->Set(NN<const Media::CS::TransferParam>(this->destProfile.GetRTranParam()));
		destInfo.color.GetGTranParam()->Set(NN<const Media::CS::TransferParam>(this->destProfile.GetGTranParam()));
		destInfo.color.GetBTranParam()->Set(NN<const Media::CS::TransferParam>(this->destProfile.GetBTranParam()));
	}
	destInfo.color.GetPrimaries()->Set(this->destProfile.GetPrimaries());
	destInfo.atype = this->GetDestAlphaType();
	NEW_CLASS(newImage, Media::StaticImage(destInfo));
	Int32 tlx = (Int32)srcTL.x;
	Int32 tly = (Int32)srcTL.y;
	Resize(NN<Media::StaticImage>::ConvertFrom(srcImage)->data.Ptr() + (tlx << 2) + tly * (OSInt)srcImage->GetDataBpl(), (OSInt)srcImage->GetDataBpl(), srcBR.x - srcTL.x, srcBR.y - srcTL.y, srcTL.x - tlx, srcTL.y - tly, newImage->data.Ptr(), (OSInt)newImage->GetDataBpl(), newImage->info.dispSize.x, newImage->info.dispSize.y);
	return newImage;
}
