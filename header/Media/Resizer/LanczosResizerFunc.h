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
				UIntOS length;
				UnsafeArray<Int64> weight;
				UnsafeArray<IntOS> index;
				UIntOS tap;
			} Parameter;

			static void SetupInterpolationParameterV(UIntOS nTap, Double sourceLength, IntOS sourceMaxPos, UIntOS result_length, NN<Parameter> out, IntOS indexSep, Double offsetCorr);
			static void SetupDecimationParameterV(UIntOS nTap, Double sourceLength, IntOS sourceMaxPos, UIntOS result_length, NN<Parameter> out, IntOS indexSep, Double offsetCorr);
			static void SetupInterpolationParameterH(UIntOS nTap, Double sourceLength, IntOS sourceMaxPos, UIntOS result_length, NN<Parameter> out, IntOS indexSep, Double offsetCorr);
			static void SetupDecimationParameterH(UIntOS nTap, Double sourceLength, IntOS sourceMaxPos, UIntOS result_length, NN<Parameter> out, IntOS indexSep, Double offsetCorr);
		};
	}
}
#endif
