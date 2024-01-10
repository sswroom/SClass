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

			virtual Text::CStringNN GetName() const;
			virtual Media::RasterImage *GenerateImage(NotNullPtr<const Media::ColorProfile> colorProfile, Math::Size2D<UOSInt> size);
		};
	}
}
#endif
