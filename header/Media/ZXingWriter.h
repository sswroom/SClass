#ifndef _SM_MEDIA_ZXINGWRITER
#define _SM_MEDIA_ZXINGWRITER
#include "Media/StaticImage.h"

namespace Media
{
	class ZXingWriter
	{
	public:
		static Media::StaticImage *GenQRCode(Text::CString content, Math::Size2D<UOSInt> outputSize);
	};
}
#endif