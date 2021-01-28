#ifndef _SM_MEDIA_IMAGECMP_IMAGEMEANSQRERR
#define _SM_MEDIA_IMAGECMP_IMAGEMEANSQRERR
#include "Media/Image.h"

namespace Media
{
	namespace ImageCmp
	{
		class ImageMeanSqrErr
		{
		public:
			ImageMeanSqrErr();
			~ImageMeanSqrErr();

			Double CompareImage(Media::Image *oriImage, Media::Image *cmpImage);
		};
	};
};
#endif
