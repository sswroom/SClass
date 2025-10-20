#ifndef _SM_MEDIA_IMAGECMP_IMAGEMEANSQRERR
#define _SM_MEDIA_IMAGECMP_IMAGEMEANSQRERR
#include "Media/StaticImage.h"

namespace Media
{
	namespace ImageCmp
	{
		class ImageMeanSqrErr
		{
		public:
			ImageMeanSqrErr();
			~ImageMeanSqrErr();

			Double CompareImage(NN<Media::RasterImage> oriImage, NN<Media::RasterImage> cmpImage);
		};
	}
}
#endif
