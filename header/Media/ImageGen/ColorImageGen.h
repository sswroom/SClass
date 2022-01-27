#ifndef _SM_MEDIA_IMAGEGEN_COLORIMAGEGEN
#define _SM_MEDIA_IMAGEGEN_COLORIMAGEGEN
#include "Media/ImageGenerator.h"

namespace Media
{
	namespace ImageGen
	{
		class ColorImageGen : public Media::ImageGenerator
		{
		public:
			ColorImageGen();
			virtual ~ColorImageGen();

			virtual Text::CString GetName();
			virtual Media::Image *GenerateImage(Media::ColorProfile *colorProfile, UOSInt width, UOSInt height);
		};
	}
}
#endif
