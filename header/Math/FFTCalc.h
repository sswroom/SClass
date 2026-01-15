#ifndef _SM_MATH_FFTCALC
#define _SM_MATH_FFTCALC

namespace Math
{
	class FFTCalc
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

		typedef enum
		{
			ST_I16,
			ST_I24
		} SampleType;
	private:
		UIntOS sampleCount;
		WindowType wtype;
		UnsafeArray<Double> sampleWindow;
		UnsafeArray<Double> sampleTemp;

		void BuildSampleWin();
	public:
		FFTCalc(UIntOS sampleCount, WindowType wtype);
		~FFTCalc();

		Bool ForwardBits(UnsafeArray<UInt8> samples, UnsafeArray<Double> freq, SampleType sampleType, UIntOS nChannels, Double magnify);
	};
}
#endif
