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

			virtual Text::CString GetName();
			virtual Media::Image *GenerateImage(Media::ColorProfile *colorProfile, UOSInt width, UOSInt height);
		};
	}
}
#endif
