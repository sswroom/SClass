#ifndef _SM_MEDIA_IMAGE
#define _SM_MEDIA_IMAGE
#include "Math/RectArea.hpp"
#include "Media/FrameInfo.h"
#include "Media/EXIFData.h"
#include "Text/CString.h"

namespace Media
{
	class StaticImage;
	class Image
	{
	public:
		virtual ~Image() {};

		virtual Bool IsRaster() const = 0;
		virtual NN<Media::StaticImage> CreateStaticImage() const = 0;
		virtual NN<Media::StaticImage> CreateSubImage(Math::RectArea<IntOS> area) const = 0;
	};
}
#endif
