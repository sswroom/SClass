#ifndef _SM_MEDIA_IMAGECMP_IMAGEPSNR
#define _SM_MEDIA_IMAGECMP_IMAGEPSNR
#include "Media/ImageCmp/ImageMeanSqrErr.h"

namespace Media
{
	namespace ImageCmp
	{
		class ImagePSNR
		{
		private:
			Media::ImageCmp::ImageMeanSqrErr msr;

		public:
			ImagePSNR();
			~ImagePSNR();

			Double CompareImage(NN<Media::RasterImage> oriImage, NN<Media::RasterImage> cmpImage);
		};
	}
}
#endif
