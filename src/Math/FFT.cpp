#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/FFT.h"
#include "Math/Math.h"

Bool Math::FFT::Forward(UnsafeArray<Double> complexData, UOSInt sampleCount)
{
	if (sampleCount == 1)
		return true;

	if (sampleCount <= 0)
		return false;
	if (sampleCount & 1)
		return false;
	UOSInt i;
	UOSInt j = sampleCount >> 1;
	Double *temp = MemAllocA(Double, sampleCount);
#if defined(HAS_ASM32)
	_asm
	{
		mov ecx,sampleCount
		mov esi,temp
		mov edi,complexData
		mov edx,complexData
		shr ecx,1
		ALIGN 16
fftflop1:
		movdqa xmm0,mmword ptr [edx]
		movdqa xmm1,mmword ptr [edx+16]
		movdqa mmword ptr [edi],xmm0
		movdqa mmword ptr [esi],xmm1
		add edx,32
		add esi,16
		add edi,16
		dec ecx
		jnz fftflop1

		mov ecx,sampleCount
		mov esi,temp
		shr ecx,1
		ALIGN 16
fftflop2:
		movdqa xmm0,mmword ptr [esi]
		movdqa mmword ptr [edi],xmm0
		add esi,16
		add edi,16
		dec ecx
		jnz fftflop2
	}
#else
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
	MemCopyNAC(&complexData[j * 2], temp, j * 16);
#endif
	MemFreeA(temp);

	if (!Forward(complexData, j) || !Forward(&complexData[j * 2], j))
		return false;

	Double kthMul = -2 * Math::PI / UOSInt2Double(sampleCount);
	Double wk[2];
	Double odd[2];
	Double even[2];
	Double rv;
	Double iv;
	i = 0;
	while (i < j)
	{
		Double kth = UOSInt2Double(i) * kthMul;
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
}

void Math::FFT::ApplyWindow(UnsafeArray<Double> complexData, UOSInt sampleCount, WindowType wtype)
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
	switch (wtype)
	{
	default:
	case WT_RECTANGULAR:
		break;

	case WT_TRIANGULAR:
		k = sampleCount >> 1;
		dk = UOSInt2Double(k);
		j = 0;
		while (j < k)
		{
			complexData[j * 2] = complexData[j * 2] * (UOSInt2Double(j) + 0.5) / dk;
			j++;
		}
		while (j < sampleCount)
		{
			complexData[j * 2] = complexData[j * 2] * (UOSInt2Double(sampleCount - j) + 0.5) / dk;
			j++;
		}
		break;

	case WT_HAMMING:
		k = sampleCount - 1;
		dk = UOSInt2Double(k);
		pi2 = 2 * Math::PI;
		a0 = 0.53836;
		a1 = 0.46164;
		j = 0;
		while (j < sampleCount)
		{
			complexData[j * 2] = complexData[j * 2] * (a0 - a1 * Math_Cos(UOSInt2Double(j) * pi2 / dk));
			j++;
		}
		break;

	case WT_BLACKMANN:
		k = sampleCount - 1;
		dk = UOSInt2Double(k);
		pi2 = 2 * Math::PI;
		a0 = 7938.0 / 18608.0;
		a1 = 9240.0 / 18608.0;
		a2 = 1430.0 / 18608.0;
		j = 0;
		while (j < sampleCount)
		{
			dj = UOSInt2Double(j);
			complexData[j * 2] = complexData[j * 2] * (a0 - a1 * Math_Cos(dj * pi2 / dk) + a2 * Math_Cos(2 * pi2 * dj / dk));
			j++;
		}
		break;

	case WT_NUTTALL:
		k = sampleCount - 1;
		dk = UOSInt2Double(k);
		pi2 = 2 * Math::PI;
		a0 = 0.355768;
		a1 = 0.487396;
		a2 = 0.144232;
		a3 = 0.012604;
		j = 0;
		while (j < sampleCount)
		{
			dj = UOSInt2Double(j);
			complexData[j * 2] = complexData[j * 2] * (a0 - a1 * Math_Cos(dj * pi2 / dk) + a2 * Math_Cos(2 * pi2 * dj / dk) - a3 * Math_Cos(3 * pi2 * dj / dk));
			j++;
		}
		break;

	case WT_BLACKMANN_NUTTALL:
		k = sampleCount - 1;
		dk = UOSInt2Double(k);
		pi2 = 2 * Math::PI;
		a0 = 0.3635819;
		a1 = 0.4891775;
		a2 = 0.1365995;
		a3 = 0.0106411;
		j = 0;
		while (j < sampleCount)
		{
			dj = UOSInt2Double(j);
			complexData[j * 2] = complexData[j * 2] * (a0 - a1 * Math_Cos(dj * pi2 / dk) + a2 * Math_Cos(2 * pi2 * dj / dk) - a3 * Math_Cos(3 * pi2 * dj / dk));
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
			complexData[j * 2] = complexData[j * 2] * (a0 - a1 * Math_Cos(dj * pi2 * invK) + a2 * Math_Cos(2 * pi2 * dj * invK) - a3 * Math_Cos(3 * pi2 * dj * invK));
			j++;
		}
		break;

	case WT_FLAT_TOP:
		k = sampleCount - 1;
		dk = UOSInt2Double(k);
		pi2 = 2 * Math::PI;
		a0 = 1;
		a1 = 1.93;
		a2 = 1.29;
		a3 = 0.388;
		a4 = 0.028;
		j = 0;
		while (j < sampleCount)
		{
			dj = UOSInt2Double(j);
			complexData[j * 2] = complexData[j * 2] * (a0 - a1 * Math_Cos(dj * pi2 / dk) + a2 * Math_Cos(2 * pi2 * dj / dk) - a3 * Math_Cos(3 * pi2 * dj / dk) + a4 * Math_Cos(4 * pi2 * dj / dk));
			j++;
		}
		break;
	}
}

Bool Math::FFT::Forward(UnsafeArray<ComplexNumber> data, UOSInt sampleCount)
{
	if (sampleCount == 1)
		return true;

	if (sampleCount <= 0)
		return false;
	if (sampleCount & 1)
		return false;
	UOSInt i;
	UOSInt j;
	ComplexNumber *temp = MemAlloc(ComplexNumber, sampleCount >> 1);
	i = 0;
	j = sampleCount >> 1;
	while (i < j)
	{
		temp[i] = data[i * 2 + 1];
		data[i] = data[i * 2];
		i++;
	}
	MemCopyNAC(&data[j], temp, sizeof(ComplexNumber) * j);
	MemFree(temp);

	if (!Forward(data, j) || !Forward(&data[j], j))
		return false;

	ComplexNumber wk;
	ComplexNumber odd;
	ComplexNumber even;
	Double dsampleCount = UOSInt2Double(sampleCount);
	i = 0;
	while (i < j)
	{
		Double kth = -2 * UOSInt2Double(i) * Math::PI / dsampleCount;
		even = data[i];
		odd = data[i + j];
		wk = ComplexNumber(Math_Cos(kth), Math_Sin(kth)) * odd;
		data[i] = even + wk;
		data[i + j] = even - wk;
		i++;
	}
	return true;
}

Bool Math::FFT::Inverse(UnsafeArray<ComplexNumber> data, UOSInt sampleCount)
{
	return false;
}

Bool Math::FFT::ForwardBits(UnsafeArray<UInt8> samples, UnsafeArray<Double> freq, UOSInt sampleCount, UOSInt sampleAvg, UOSInt nBitPerSample, UOSInt nChannels, WindowType wtype, Double magnify)
{
	UOSInt sampleAdd = nChannels * nBitPerSample >> 3;
	UOSInt i;
	UOSInt j;
	UnsafeArray<UInt8> currSamples;
	Double *temp = MemAllocA(Double, sampleCount * 2);
	Double *tmpFreq = MemAlloc(Double, sampleCount);
	Double f;
	Double f2;

	j = 0;
	while (j < sampleCount)
	{
		freq[j] = 0;
		j++;
	}

	i = sampleAvg;
	while (i-- > 0)
	{
		currSamples = samples;

		if (nBitPerSample == 24)
		{
			magnify = magnify / 8388608.0;
			j = 0;
			while (j < sampleCount)
			{
				temp[j * 2] = ReadInt24(&currSamples[0]) * magnify;
				temp[j * 2 + 1] = 0;
				currSamples += sampleAdd;

				j++;
			}
		}
		else if (nBitPerSample == 16)
		{
			magnify = magnify / 32768.0;
			j = 0;
			while (j < sampleCount)
			{
				temp[j * 2] = ReadInt16(&currSamples[0]) * magnify;
				temp[j * 2 + 1] = 0;
				currSamples += sampleAdd;

				j++;
			}
		}

		ApplyWindow(temp, sampleCount, wtype);
		Forward(temp, sampleCount);
		j = 0;
		while (j < sampleCount)
		{
			f = temp[j * 2];
			f2 = temp[j * 2 + 1];
			freq[j] += Math_Sqrt(f * f + f2 * f2);
			j++;
		}

		samples += sampleAdd;
	}

	f = 1 / (Double)sampleAvg;
	j = 0;
	while (j < sampleCount)
	{
		freq[j] = freq[j] * f;
		j++;
	}
	MemFree(tmpFreq);
	MemFreeA(temp);
	return true;
}
