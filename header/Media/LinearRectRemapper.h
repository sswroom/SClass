#ifndef _SM_MEDIA_LINEARRECTREMAPPER
#define _SM_MEDIA_LINEARRECTREMAPPER
#include "Math/Quadrilateral.h"
#include "Media/StaticImage.h"

namespace Media
{
	class LinearRectRemapper
	{
	public:
		static Media::StaticImage *RemapW8(const UInt8 *imgPtr, Math::Size2D<UOSInt> imgSize, OSInt imgBpl, Math::Size2D<UOSInt> outputSize, Math::Quadrilateral quad, Media::ColorProfile *color, Media::ColorProfile::YUVType yuvType, Media::YCOffset ycOfst);
	};
}
#endif
