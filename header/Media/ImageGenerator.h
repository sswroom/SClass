#ifndef _SM_MEDIA_IMAGEGENERATOR
#define _SM_MEDIA_IMAGEGENERATOR
#include "Media/Image.h"
#include "Media/ColorProfile.h"

namespace Media
{
	class ImageGenerator
	{
	public:
		ImageGenerator();
		virtual ~ImageGenerator();

		virtual const UTF8Char *GetName() = 0;
		virtual Media::Image *GenerateImage(Media::ColorProfile *colorProfile, UOSInt width, UOSInt height) = 0;
	};
}
#endif
