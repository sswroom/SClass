#ifndef _SM_MEDIA_IMAGEGEN_GAMMAIMAGEGEN
#define _SM_MEDIA_IMAGEGEN_GAMMAIMAGEGEN
#include "Media/ImageGenerator.h"

namespace Media
{
	namespace ImageGen
	{
		class GammaImageGen : public Media::ImageGenerator
		{
		public:
			GammaImageGen();
			virtual ~GammaImageGen();

			virtual const UTF8Char *GetName();
			virtual Media::Image *GenerateImage(Media::ColorProfile *colorProfile, UOSInt width, UOSInt height);
		};
	}
}
#endif
