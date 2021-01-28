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

			virtual const UTF8Char *GetName();
			virtual Media::Image *GenerateImage(Media::ColorProfile *colorProfile, OSInt width, OSInt height);
		};
	};
};
#endif
