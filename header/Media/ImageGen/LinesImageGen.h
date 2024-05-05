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

			virtual Text::CStringNN GetName() const;
			virtual Optional<Media::RasterImage> GenerateImage(NN<const Media::ColorProfile> colorProfile, Math::Size2D<UOSInt> size);
		};
	}
}
#endif
