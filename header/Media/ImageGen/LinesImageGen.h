#ifndef _SM_MEDIA_IMAGEGEN_LINESIMAGEGEN
#define _SM_MEDIA_IMAGEGEN_LINESIMAGEGEN
#include "Media/ImageGenerator.h"

namespace Media
{
	namespace ImageGen
	{
		class LinesImageGen : public Media::ImageGenerator
		{
		public:
			LinesImageGen();
			virtual ~LinesImageGen();

			virtual const UTF8Char *GetName();
			virtual Media::Image *GenerateImage(Media::ColorProfile *colorProfile, UOSInt width, UOSInt height);
		};
	}
}
#endif
