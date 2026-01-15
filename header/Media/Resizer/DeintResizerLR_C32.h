#ifndef _SM_MEDIA_RESIZER_DEINTRESIZERLR_C32
#define _SM_MEDIA_RESIZER_DEINTRESIZERLR_C32
#include "Media/DeinterlacingResizer.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"

namespace Media
{
	namespace Resizer
	{
		class DeintResizerLR_C32 : public Media::Resizer::LanczosResizerLR_C32, public Media::DeinterlacingResizer
		{
		protected:
			Double osSize;
			UIntOS odSize;
			IntOS osStep;
			UnsafeArrayOpt<IntOS> oIndex;
			UnsafeArrayOpt<Int64> oWeight;
			UIntOS oTap;
			Optional<Media::Resizer::LanczosResizerLR_C32Action::VertFilter> oFilter;

			Double esSize;
			UIntOS edSize;
			IntOS esStep;
			UnsafeArrayOpt<IntOS> eIndex;
			UnsafeArrayOpt<Int64> eWeight;
			UIntOS eTap;
			Optional<Media::Resizer::LanczosResizerLR_C32Action::VertFilter> eFilter;

			void DestoryVertO();
			void DestoryVertE();
		public:
			DeintResizerLR_C32(UIntOS hnTap, UIntOS vnTap, NN<const Media::ColorProfile> destColor, Optional<Media::ColorManagerSess> colorSess, Media::AlphaType srcAlphaType, Double srcRefLuminance, Media::PixelFormat pf);
			virtual ~DeintResizerLR_C32();

			virtual void DeintResize(Media::DeinterlacingResizer::DeintType dType, UnsafeArray<UInt8> src, UIntOS sbpl, Double swidth, Double sheight, UnsafeArray<UInt8> dest, UIntOS dbpl, UIntOS dwidth, UIntOS dheight, Bool upsideDown);
			virtual void SetDISrcRefLuminance(Double srcRefLuminance);
		};
	}
}
#endif
