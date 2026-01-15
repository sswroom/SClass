#ifndef _SM_MEDIA_LINEARRECTREMAPPER
#define _SM_MEDIA_LINEARRECTREMAPPER
#include "Math/Quadrilateral.h"
#include "Media/StaticImage.h"

namespace Media
{
	class LinearRectRemapper
	{
	public:
		static NN<Media::StaticImage> RemapW8(UnsafeArray<const UInt8> imgPtr, Math::Size2D<UIntOS> imgSize, IntOS imgBpl, Math::Size2D<UIntOS> outputSize, Math::Quadrilateral quad, NN<const Media::ColorProfile> color, Media::ColorProfile::YUVType yuvType, Media::YCOffset ycOfst);
	};
}
#endif
