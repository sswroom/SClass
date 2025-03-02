#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERFUNC
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERFUNC

namespace Media
{
	namespace Resizer
	{
		class LanczosResizerFunc
		{
		public:
			typedef struct
			{
				UOSInt length;
				UnsafeArray<Int64> weight;
				UnsafeArray<OSInt> index;
				UOSInt tap;
			} Parameter;

			static void SetupInterpolationParameterV(UOSInt nTap, Double sourceLength, OSInt sourceMaxPos, UOSInt result_length, NN<Parameter> out, OSInt indexSep, Double offsetCorr);
			static void SetupDecimationParameterV(UOSInt nTap, Double sourceLength, OSInt sourceMaxPos, UOSInt result_length, NN<Parameter> out, OSInt indexSep, Double offsetCorr);
			static void SetupInterpolationParameterH(UOSInt nTap, Double sourceLength, OSInt sourceMaxPos, UOSInt result_length, NN<Parameter> out, OSInt indexSep, Double offsetCorr);
			static void SetupDecimationParameterH(UOSInt nTap, Double sourceLength, OSInt sourceMaxPos, UOSInt result_length, NN<Parameter> out, OSInt indexSep, Double offsetCorr);
		};
	}
}
#endif
