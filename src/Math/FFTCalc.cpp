#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/FFTCalc.h"
#include "Math/Math.h"

extern "C"
{
	void FFTCalc_ApplyWindowI16(Double *complexOut, UInt8 *sampleIn, Double *sampleWindow, UOSInt sampleCnt, UOSInt sampleAdd, Double sampleMul);
	void FFTCalc_ApplyWindowI24(Double *complexOut, UInt8 *sampleIn, Double *sampleWindow, UOSInt sampleCnt, UOSInt sampleAdd, Double sampleMul);
	void FFTCalc_FFT2Freq(Double *freq, Double *complexIn, UOSInt sampleCnt);
	UOSInt FFTCalc_Forward(Double *complexData, UOSInt sampleCount);
}

void Math::FFTCalc::BuildSampleWin()
{
	Double pi2;
	Double a0;
	Double a1;
	Double a2;
	Double a3;
	Double a4;
	Double invK;
	UOSInt j;
	UOSInt k;
	Double dj;
	Double dk;
	Double dsampleCount;
	switch (this->wtype)
	{
	default:
	case WT_RECTANGULAR:
		k = this->sampleCount;
		j = 0;
		while (j < k)
		{
			this->sampleWindow[j] = 1.0;
			j++;
		}
		break;

	case WT_TRIANGULAR:
		k = sampleCount >> 1;
		j = 0;
		dk = UOSInt2Double(k);
		dsampleCount = UOSInt2Double(sampleCount);
		while (j < k)
		{
			this->sampleWindow[j]= (UOSInt2Double(j) + 0.5) / dk;
			j++;
		}
		while (j < sampleCount)
		{
			this->sampleWindow[j] = (dsampleCount - UOSInt2Double(j) + 0.5) / dk;
			j++;
		}
		break;

	case WT_HAMMING:
		k = sampleCount - 1;
		pi2 = 2 * Math::PI;
		a0 = 0.53836;
		a1 = 0.46164;
		j = 0;
		dk = UOSInt2Double(k);
		while (j < sampleCount)
		{
			this->sampleWindow[j] = (a0 - a1 * Math_Cos(UOSInt2Double(j) * pi2 / dk));
			j++;
		}
		break;

	case WT_BLACKMANN:
		k = sampleCount - 1;
		pi2 = 2 * Math::PI;
		a0 = 7938.0 / 18608.0;
		a1 = 9240.0 / 18608.0;
		a2 = 1430.0 / 18608.0;
		dk = UOSInt2Double(k);
		j = 0;
		while (j < sampleCount)
		{
			dj = UOSInt2Double(j);
			this->sampleWindow[j] = (a0 - a1 * Math_Cos(dj * pi2 / dk) + a2 * Math_Cos(2 * pi2 * dj / dk));
			j++;
		}
		break;

	case WT_NUTTALL:
		k = sampleCount - 1;
		pi2 = 2 * Math::PI;
		a0 = 0.355768;
		a1 = 0.487396;
		a2 = 0.144232;
		a3 = 0.012604;
		dk = UOSInt2Double(k);
		j = 0;
		while (j < sampleCount)
		{
			dj = UOSInt2Double(j);
			this->sampleWindow[j] = (a0 - a1 * Math_Cos(dj * pi2 / dk) + a2 * Math_Cos(2 * pi2 * dj / dk) - a3 * Math_Cos(3 * pi2 * dj / dk));
			j++;
		}
		break;

	case WT_BLACKMANN_NUTTALL:
		k = sampleCount - 1;
		pi2 = 2 * Math::PI;
		a0 = 0.3635819;
		a1 = 0.4891775;
		a2 = 0.1365995;
		a3 = 0.0106411;
		dk = UOSInt2Double(k);
		j = 0;
		while (j < sampleCount)
		{
			dj = UOSInt2Double(j);
			this->sampleWindow[j] = (a0 - a1 * Math_Cos(dj * pi2 / dk) + a2 * Math_Cos(2 * pi2 * dj / dk) - a3 * Math_Cos(3 * pi2 * dj / dk));
			j++;
		}
		break;

	case WT_BLACKMANN_HARRIS:
		k = sampleCount - 1;
		dk = UOSInt2Double(k);
		invK = 1.0 / dk;
		pi2 = 2 * Math::PI;
		a0 = 0.35875;
		a1 = 0.48829;
		a2 = 0.14128;
		a3 = 0.01168;
		j = 0;
		while (j < sampleCount)
		{
			dj = UOSInt2Double(j);
			this->sampleWindow[j] = (a0 - a1 * Math_Cos(dj * pi2 * invK) + a2 * Math_Cos(2 * pi2 * dj * invK) - a3 * Math_Cos(3 * pi2 * dj * invK));
			j++;
		}
		break;

	case WT_FLAT_TOP:
		k = sampleCount - 1;
		pi2 = 2 * Math::PI;
		a0 = 1;
		a1 = 1.93;
		a2 = 1.29;
		a3 = 0.388;
		a4 = 0.028;
		dk = UOSInt2Double(k);
		j = 0;
		while (j < sampleCount)
		{
			dj = UOSInt2Double(j);
			this->sampleWindow[j] = (a0 - a1 * Math_Cos(dj * pi2 / dk) + a2 * Math_Cos(2 * pi2 * dj / dk) - a3 * Math_Cos(3 * pi2 * dj / dk) + a4 * Math_Cos(4 * pi2 * dj / dk));
			j++;
		}
		break;
	}
}

Math::FFTCalc::FFTCalc(UOSInt sampleCount, WindowType wtype)
{
	this->sampleCount = sampleCount;
	this->wtype = wtype;
	this->sampleWindow = MemAllocA(Double, sampleCount);
	this->sampleTemp = MemAllocA(Double, sampleCount << 1);
	this->BuildSampleWin();
}

Math::FFTCalc::~FFTCalc()
{
	MemFreeA(this->sampleWindow);
	MemFreeA(this->sampleTemp);
}

Bool Math::FFTCalc::ForwardBits(UInt8 *samples, Double *freq, SampleType sampleType, UOSInt nChannels, Double magnify)
{
	if (sampleType == Math::FFTCalc::ST_I24)
	{
		FFTCalc_ApplyWindowI24(this->sampleTemp, samples, this->sampleWindow, this->sampleCount, nChannels * 3, magnify / 8388608.0);
	}
	else if (sampleType == Math::FFTCalc::ST_I16)
	{
		FFTCalc_ApplyWindowI16(this->sampleTemp, samples, this->sampleWindow, this->sampleCount, nChannels * 2, magnify / 32768.0);
	}

	FFTCalc_Forward(this->sampleTemp, this->sampleCount);

	FFTCalc_FFT2Freq(freq, this->sampleTemp, this->sampleCount);
	return true;
}
