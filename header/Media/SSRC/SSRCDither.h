#ifndef _SM_MEDIA_SSRC_SSRCDITHER
#define _SM_MEDIA_SSRC_SSRCDITHER
#include "Media/SSRC/SleefRNG.h"

#define SSRC_RANDBUFLEN (1 << 16)
#define SSRC_SHAPERLENMAX 64

namespace Media
{
	namespace SSRC
	{
		class SSRCDither
		{
		public:
			enum class PDFID
			{
				Rectangular,
				Triangular,
				Gaussian,
				TwoLevel
			};

			enum class DitherType
			{
				Simple,
				Old,
				ATH_A,
				ATH_B
			};

			struct DitherCoef
			{
				UInt32 fs;
				UInt32 id;

				DitherType type;
				const Char *friendlyName;

				UInt32 len;
				const Double coefs[SSRC_SHAPERLENMAX];
			};
		private:
			static const UInt32 freqs[];
			static const DitherCoef ditherCoef[];

			Bool error;
			UInt32 fs;
			UOSInt shaperIndex;
			PDFID pdfID;
			Double noisePeak;

			int shaperClipMin;
			int shaperClipMax;
			Double peakBottom, peakTop;

			SleefRNG rng;
			int randPtr;

			Double buf[SSRC_SHAPERLENMAX];
			Double randBuf[SSRC_RANDBUFLEN];
		public:
			SSRCDither(UInt32 fs, Int32 min, Int32 max, UInt32 shaperID, PDFID pdfID, Double noisePeak, UInt64 seed);
			~SSRCDither();

			Bool IsError();
			void GetPeaks(Double *peaks);
			Double GetMaxNoiseStrength();
			void QuantizeDouble(Int32 *out, const Double *in, UOSInt nSamples, Double gain);
		};
	}
}
#endif
