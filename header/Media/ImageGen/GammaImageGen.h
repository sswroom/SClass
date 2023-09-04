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

			virtual Text::CString GetName();
			virtual Media::Image *GenerateImage(NotNullPtr<const Media::ColorProfile> colorProfile, Math::Size2D<UOSInt> size);
		};
	}
}
#endif
