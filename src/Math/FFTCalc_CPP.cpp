#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Math/Math_C.h"


extern "C" void FFTCalc_ApplyWindowI16(Double *complexOut, UInt8 *sampleIn, Double *sampleWindow, OSInt sampleCnt, OSInt sampleAdd, Double sampleMul)
{
	OSInt j;
	j = 0;
	while (j < sampleCnt)
	{
		complexOut[j * 2] = ReadInt16(sampleIn) * sampleMul * sampleWindow[j];
		complexOut[j * 2 + 1] = 0;
		sampleIn += sampleAdd;

		j++;
	}
}

extern "C" void FFTCalc_ApplyWindowI24(Double *complexOut, UInt8 *sampleIn, Double *sampleWindow, OSInt sampleCnt, OSInt sampleAdd, Double sampleMul)
{
	OSInt j;
	j = 0;
	while (j < sampleCnt)
	{
		complexOut[j * 2] = ReadInt24(sampleIn) * sampleMul * sampleWindow[j];
		complexOut[j * 2 + 1] = 0;
		sampleIn += sampleAdd;

		j++;
	}
}

extern "C" void FFTCalc_FFT2Freq(Double *freq, Double *complexIn, OSInt sampleCnt)
{
	OSInt j;
	Double f;
	Double f2;
	j = 0;
	while (j < sampleCnt)
	{
		f = complexIn[j * 2];
		f2 = complexIn[j * 2 + 1];
		freq[j] = Math_Sqrt(f * f + f2 * f2);
		j++;
	}
}

#if !defined(TEST)
extern "C"
{
	void FFTCalc_Rearrange(Double *complexData, OSInt sampleCount);
	void FFTCalc_ForwardCalc(Double *complexData, OSInt sampleCount);
	void FFTCalc_ForwardCalcR(Double *complexData, OSInt sampleCount);
}
#endif

extern "C" OSInt FFTCalc_Forward(Double *complexData, OSInt sampleCount)
{
	if (sampleCount == 1)
		return 1;

	if (sampleCount <= 0)
		return 0;
	if (sampleCount & (sampleCount - 1))
		return 0;

	OSInt n;
	OSInt i;
	OSInt j;
	OSInt m;
	Double tmpVal;
	n = sampleCount << 1;
	i = 0;
	j = 0;
    while (i < n)
	{
		if (j > i)
		{
			tmpVal = complexData[j];
			complexData[j] = complexData[i];
			complexData[i] = tmpVal;
			tmpVal = complexData[j + 1];
			complexData[j + 1] = complexData[i + 1];
			complexData[i + 1] = tmpVal;
		}
		m = sampleCount;
		while (m >= 2 && j >= m)
		{
			j -= m;
			m >>= 1;
		}
		j += m;
		i += 2;
	}
//	FFTCalc_Rearrange(complexData, sampleCount);
//	FFTCalc_ForwardCalcR(complexData, sampleCount);
//	OSInt i;

	OSInt thisSampleCount = 4;
	OSInt groupCurr;
	OSInt groupEnd;
	Double kthMul = -2 * Math::PI / 4.0;
	Double wk[2];
	Double odd[2];
	Double even[2];
	Double csin[2];
	Double csin1[2];
	Double nextSin;

	// thisSampleCount = 2
	csin1[0] = 1.0;
	csin1[1] = 0.0;
	csin[0] = 0.0;
	csin[1] = 0.0;
	groupEnd = 1;
	i = 0;
	while (i < sampleCount)
	{
		even[0] = complexData[i * 2];
		even[1] = complexData[i * 2 + 1];
		odd[0] = complexData[(i + groupEnd) * 2];
		odd[1] = complexData[(i + groupEnd) * 2 + 1];
		wk[0] = csin1[0] * odd[0] - csin1[1] * odd[1];
		wk[1] = csin1[0] * odd[1] + csin1[1] * odd[0];
		complexData[i * 2] = even[0] + wk[0];
		complexData[i * 2 + 1] = even[1] + wk[1];
		complexData[(i + groupEnd) * 2] = even[0] - wk[0];
		complexData[(i + groupEnd) * 2 + 1] = even[1] - wk[1];

		i += 2;
	}

	nextSin = Math_Sin(kthMul);
	while (thisSampleCount <= sampleCount)
	{
		csin1[0] = 1.0;
		csin1[1] = 0.0;
		csin[1] = nextSin;
		kthMul = kthMul * 0.5;
		csin[0] = nextSin = Math_Sin(kthMul);
		csin[0] = -2.0 * csin[0] * csin[0];

		groupCurr = 0;
		groupEnd = thisSampleCount >> 1;
		while (groupCurr < groupEnd)
		{
			i = groupCurr;
			while (i < sampleCount)
			{
				even[0] = complexData[i * 2];
				even[1] = complexData[i * 2 + 1];
				odd[0] = complexData[(i + groupEnd) * 2];
				odd[1] = complexData[(i + groupEnd) * 2 + 1];
				wk[0] = csin1[0] * odd[0] - csin1[1] * odd[1];
				wk[1] = csin1[0] * odd[1] + csin1[1] * odd[0];
				complexData[i * 2] = even[0] + wk[0];
				complexData[i * 2 + 1] = even[1] + wk[1];
				complexData[(i + groupEnd) * 2] = even[0] - wk[0];
				complexData[(i + groupEnd) * 2 + 1] = even[1] - wk[1];

				i += thisSampleCount;
			}

			wk[0] = csin1[0];
			csin1[0] = csin[0] * csin1[0] - csin[1] * csin1[1] + csin1[0];
			csin1[1] = csin[0] * csin1[1] + csin[1] *    wk[0] + csin1[1];
			groupCurr++;
		}

		thisSampleCount = thisSampleCount << 1;
	}

	return 1;
}

/* Inplace (Error)

	OSInt target = 0;
	OSInt position = 0;
	UOSInt mask;
	Double tmp0;
	Double tmp1;
	while (position < sampleCount)
	{
		if (target > position)
		{
			tmp0 = complexData[position * 2 + 0];
			tmp1 = complexData[position * 2 + 1];
			complexData[target * 2 + 0] = complexData[position * 2 + 0];
			complexData[target * 2 + 1] = complexData[position * 2 + 1];
			complexData[position * 2 + 0] = tmp0;
			complexData[position * 2 + 1] = tmp1;
		}
		mask = sampleCount;
		while (target & (mask >>= 1))
		{
			target &= ~mask;
		}
		target |= mask;

		position++;
	}

	const Double pi = (false) ? -Math::PI : Math::PI;
	OSInt step = 1;
	while (step < sampleCount)
	{
		const UOSInt jump = step << 1;
		const Double delta = pi / step;
		const Double sine = Math_Sin(delta * .5);
		const Double multiplier0 = -2. * sine * sine;
		const Double multiplier1 = Math_Sin(delta);
		Double factor0 = 1.0;
		Double factor1 = 0.0;
		OSInt group = 0;
		while (group < step)
		{
			OSInt pair = group;
			while (pair < sampleCount)
			{
				const UOSInt match = pair + step;
				const Double product0 = factor0 * complexData[match * 2 + 0] - factor1 * complexData[match * 2 + 1];
				const Double product1 = factor0 * complexData[match * 2 + 1] + factor1 * complexData[match * 2 + 0];
				complexData[match * 2 + 0] = complexData[pair * 2 + 0] - product0;
				complexData[match * 2 + 1] = complexData[pair * 2 + 1] - product1;
				complexData[pair * 2 + 0] += product0;
				complexData[pair * 2 + 1] += product1;

				pair += jump;
			}
			factor0 = multiplier0 * factor0 - multiplier1 * factor1 + factor0;
			factor1 = multiplier0 * factor1 + multiplier1 * factor0 + factor1;

			group++;
		}
		step <<= 1;
	}*/

/*
	if (sampleCount == 1)
		return true;

	if (sampleCount <= 0)
		return false;
	if (sampleCount & 1)
		return false;
	OSInt j = sampleCount >> 1;
	Double *temp = MemAllocA(Double, sampleCount);
	OSInt i;
	i = 0;
	j = sampleCount >> 1;
	while (i < j)
	{
		temp[i * 2] = complexData[i * 4 + 2];
		temp[i * 2 + 1] = complexData[i * 4 + 3];
		complexData[i * 2] = complexData[i * 4];
		complexData[i * 2 + 1] = complexData[i * 4 + 1];
		i++;
	}
	MemCopyNA(&complexData[j * 2], temp, j * 16);
	MemFreeA(temp);

	if (!Forward(complexData, j) || !Forward(&complexData[j * 2], j))
		return false;

	Double kthMul = -2 * Math::PI / sampleCount;
	Double wk[2];
	Double odd[2];
	Double even[2];
	Double rv;
	Double iv;
	i = 0;
	while (i < j)
	{
		Double kth = i * kthMul;
		even[0] = complexData[i * 2];
		even[1] = complexData[i * 2 + 1];
		odd[0] = complexData[(i + j) * 2];
		odd[1] = complexData[(i + j) * 2 + 1];
		rv = Math_Cos(kth);
		iv = Math_Sin(kth);
		wk[0] = rv * odd[0] - iv * odd[1];
		wk[1] = rv * odd[1] + iv * odd[0];
		complexData[i * 2] = even[0] + wk[0];
		complexData[i * 2 + 1] = even[1] + wk[1];
		complexData[(i + j) * 2] = even[0] - wk[0];
		complexData[(i + j) * 2 + 1] = even[1] - wk[1];
		i++;
	}
	return true;
*/