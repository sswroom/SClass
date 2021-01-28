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
		static Bool Forward(Double *complexData, UOSInt sampleCount);
		static void ApplyWindow(Double *complexData, UOSInt sampleCount, WindowType wtype);
	public:
		static Bool Forward(ComplexNumber *data, UOSInt sampleCount);
		static Bool Inverse(ComplexNumber *data, UOSInt sampleCount);
		static Bool ForwardBits(UInt8 *samples, Double *freq, UOSInt sampleCount, OSInt sampleAvg, UOSInt nBitPerSample, UOSInt nChannels, WindowType wtype, Double magnify);

/*	protected:
		static void Rearrange(const complex *const Input, complex *const Output, const unsigned int N);
		static void Rearrange(complex *const Data, const unsigned int N);
		static void Perform(complex *const Data, const unsigned int N, const bool Inverse = false);
		static void Scale(complex *const Data, const unsigned int N);*/
	};
}
#endif
