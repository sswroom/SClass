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
			UOSInt odSize;
			OSInt osStep;
			OSInt *oIndex;
			Int64 *oWeight;
			UOSInt oTap;
			Optional<Media::Resizer::LanczosResizerLR_C32Action::VertFilter> oFilter;

			Double esSize;
			UOSInt edSize;
			OSInt esStep;
			OSInt *eIndex;
			Int64 *eWeight;
			UOSInt eTap;
			Optional<Media::Resizer::LanczosResizerLR_C32Action::VertFilter> eFilter;

			void DestoryVertO();
			void DestoryVertE();
		public:
			DeintResizerLR_C32(UOSInt hnTap, UOSInt vnTap, NN<const Media::ColorProfile> destColor, Optional<Media::ColorManagerSess> colorSess, Media::AlphaType srcAlphaType, Double srcRefLuminance, Media::PixelFormat pf);
			virtual ~DeintResizerLR_C32();

			virtual void DeintResize(Media::DeinterlacingResizer::DeintType dType, UnsafeArray<UInt8> src, UOSInt sbpl, Double swidth, Double sheight, UnsafeArray<UInt8> dest, UOSInt dbpl, UOSInt dwidth, UOSInt dheight, Bool upsideDown);
			virtual void SetDISrcRefLuminance(Double srcRefLuminance);
		};
	}
}
#endif
