#ifndef _SM_MEDIA_RESIZER_DEINTRESIZERLR_C32
#define _SM_MEDIA_RESIZER_DEINTRESIZERLR_C32
#include "Media/IDeintResizer.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"

namespace Media
{
	namespace Resizer
	{
		class DeintResizerLR_C32 : public Media::Resizer::LanczosResizerLR_C32, public Media::IDeintResizer
		{
		protected:
			Double osSize;
			UOSInt odSize;
			OSInt osStep;
			OSInt *oIndex;
			Int64 *oWeight;
			OSInt oTap;
			Media::Resizer::LanczosResizerLR_C32Action::VertFilter *oFilter;

			Double esSize;
			UOSInt edSize;
			OSInt esStep;
			OSInt *eIndex;
			Int64 *eWeight;
			OSInt eTap;
			Media::Resizer::LanczosResizerLR_C32Action::VertFilter *eFilter;

			void DestoryVertO();
			void DestoryVertE();
		public:
			DeintResizerLR_C32(UOSInt hnTap, UOSInt vnTap, const Media::ColorProfile *destColor, Media::ColorManagerSess *colorSess, Media::AlphaType srcAlphaType, Double srcRefLuminance, Media::PixelFormat pf);
			virtual ~DeintResizerLR_C32();

			virtual void DeintResize(Media::IDeintResizer::DeintType dType, UInt8 *src, UOSInt sbpl, Double swidth, Double sheight, UInt8 *dest, UOSInt dbpl, UOSInt dwidth, UOSInt dheight, Bool upsideDown);
			virtual void SetDISrcRefLuminance(Double srcRefLuminance);
		};
	}
}
#endif
