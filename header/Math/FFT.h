#ifndef _SM_MATH_FFT
#define _SM_MATH_FFT
#include "Math/ComplexNumber.h"

namespace Math
{
	class FFT
	{
	public:
		typedef enum
		{
			WT_RECTANGULAR,
			WT_TRIANGULAR,
			WT_HAMMING,
			WT_BLACKMANN,
			WT_NUTTALL,
			WT_BLACKMANN_NUTTALL,
			WT_BLACKMANN_HARRIS,
			WT_FLAT_TOP
		} WindowType;
	private:
		static Bool Forward(UnsafeArray<Double> complexData, UIntOS sampleCount);
		static void ApplyWindow(UnsafeArray<Double> complexData, UIntOS sampleCount, WindowType wtype);
	public:
		static Bool Forward(UnsafeArray<ComplexNumber> data, UIntOS sampleCount);
		static Bool Inverse(UnsafeArray<ComplexNumber> data, UIntOS sampleCount);
		static Bool ForwardBits(UnsafeArray<UInt8> samples, UnsafeArray<Double> freq, UIntOS sampleCount, UIntOS sampleAvg, UIntOS nBitPerSample, UIntOS nChannels, WindowType wtype, Double magnify);

/*	protected:
		static void Rearrange(const complex *const Input, complex *const Output, const unsigned int N);
		static void Rearrange(complex *const Data, const unsigned int N);
		static void Perform(complex *const Data, const unsigned int N, const bool Inverse = false);
		static void Scale(complex *const Data, const unsigned int N);*/
	};
}
#endif
