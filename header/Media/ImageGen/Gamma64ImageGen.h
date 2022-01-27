#ifndef _SM_MEDIA_IMAGEGEN_GAMMA64IMAGEGEN
#define _SM_MEDIA_IMAGEGEN_GAMMA64IMAGEGEN
#include "Media/ImageGenerator.h"

namespace Media
{
	namespace ImageGen
	{
		class Gamma64ImageGen : public Media::ImageGenerator
		{
		public:
			Gamma64ImageGen();
			virtual ~Gamma64ImageGen();

			virtual Text::CString GetName();
			virtual Media::Image *GenerateImage(Media::ColorProfile *colorProfile, UOSInt width, UOSInt height);
		};
	}
}
#endif
