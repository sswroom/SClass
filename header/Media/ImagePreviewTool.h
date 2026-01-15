#ifndef _SM_MEDIA_IMAGEPREVIEWTOOL
#define _SM_MEDIA_IMAGEPREVIEWTOOL
#include "Media/ImageList.h"
#include "Media/StaticImage.h"

namespace Media
{
	class ImagePreviewTool
	{
	public:
		static Bool CreatePreviews(NN<Media::ImageList> imgList, NN<Data::ArrayListNN<Media::StaticImage>> prevImgs, UIntOS maxSize);
	};
}
#endif
