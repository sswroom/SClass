#ifndef _SM_MEDIA_IMAGEGEN_RINGSIMAGEGEN
#define _SM_MEDIA_IMAGEGEN_RINGSIMAGEGEN
#include "Media/ImageGenerator.h"

namespace Media
{
	namespace ImageGen
	{
		class RingsImageGen : public Media::ImageGenerator
		{
		public:
			RingsImageGen();
			virtual ~RingsImageGen();

			virtual Text::CStringNN GetName() const;
			virtual Optional<Media::RasterImage> GenerateImage(NN<const Media::ColorProfile> colorProfile, Math::Size2D<UOSInt> size);
		};
	}
}
#endif
