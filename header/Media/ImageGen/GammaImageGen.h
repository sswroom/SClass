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

			virtual Text::CStringNN GetName() const;
			virtual Optional<Media::RasterImage> GenerateImage(NN<const Media::ColorProfile> colorProfile, Math::Size2D<UIntOS> size);
		};
	}
}
#endif
