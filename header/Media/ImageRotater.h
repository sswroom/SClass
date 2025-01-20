#ifndef _SM_MEDIA_IMAGEROTATER
#define _SM_MEDIA_IMAGEROTATER
#include "Media/RasterImage.h"
namespace Media
{
	class ImageRotater
	{
	public:
		virtual Optional<Media::RasterImage> Rotate(NN<Media::RasterImage> srcImg, Single centerX, Single centerY, Single angleRad, Bool keepCoord, Bool keepSize) = 0;
	};
}
#endif
