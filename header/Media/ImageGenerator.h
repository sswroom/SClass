#ifndef _SM_MEDIA_IMAGEGENERATOR
#define _SM_MEDIA_IMAGEGENERATOR
#include "Media/ColorProfile.h"
#include "Media/RasterImage.h"
#include "Text/CString.h"

namespace Media
{
	class ImageGenerator
	{
	public:
		ImageGenerator();
		virtual ~ImageGenerator();

		virtual Text::CStringNN GetName() const = 0;
		virtual Media::RasterImage *GenerateImage(NotNullPtr<const Media::ColorProfile> colorProfile, Math::Size2D<UOSInt> size) = 0;
	};
}
#endif
