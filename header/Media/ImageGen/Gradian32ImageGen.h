#ifndef _SM_MEDIA_IMAGEGEN_GRADIAN32IMAGEGEN
#define _SM_MEDIA_IMAGEGEN_GRADIAN32IMAGEGEN
#include "Media/ImageGenerator.h"

namespace Media
{
	namespace ImageGen
	{
		class Gradian32ImageGen : public Media::ImageGenerator
		{
		public:
			Gradian32ImageGen();
			virtual ~Gradian32ImageGen();

			virtual Text::CString GetName();
			virtual Media::Image *GenerateImage(Media::ColorProfile *colorProfile, Math::Size2D<UOSInt> size);
		};
	}
}
#endif
