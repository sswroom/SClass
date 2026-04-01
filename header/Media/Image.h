#ifndef _SM_MEDIA_IMAGE
#define _SM_MEDIA_IMAGE
#include "Math/RectArea.hpp"
#include "Media/FrameInfo.h"
#include "Media/EXIFData.h"
#include "Text/CString.h"

namespace Media
{
	class StaticImage;
	enum class ImageType
	{
		Raster,
		Vector,
		SVG
	};
	class Image
	{
	public:
		virtual ~Image() {};

		virtual ImageType GetImageType() const = 0;
		virtual NN<Media::Image> Clone() const = 0;
		virtual Double GetVisibleWidthPx() const = 0;
		virtual Double GetVisibleHeightPx() const = 0;
		virtual NN<Media::StaticImage> CreateStaticImage() const = 0;
		virtual NN<Media::StaticImage> CreateSubImage(Math::RectArea<IntOS> area) const = 0;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const = 0;
	};
	Text::CStringNN ImageTypeGetName(ImageType imgType);
}
#endif
