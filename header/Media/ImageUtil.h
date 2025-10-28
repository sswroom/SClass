#ifndef _SM_MEDIA_IMAGEUTIL
#define _SM_MEDIA_IMAGEUTIL
#include "Media/FrameInfo.h"

namespace Media
{
	class ImageUtil
	{
	public:
		static void ColorReplaceAlpha32(UnsafeArray<UInt8> pixelPtr, UOSInt w, UOSInt h, UInt32 col);
		static void DrawHLineNA32(UnsafeArray<UInt8> pixelPtr, UOSInt w, UOSInt h, UOSInt bpl, OSInt y, OSInt x1, OSInt x2, UInt32 col); //no alpha
		static void DrawVLineNA32(UnsafeArray<UInt8> pixelPtr, UOSInt w, UOSInt h, UOSInt bpl, OSInt x, OSInt y1, OSInt y2, UInt32 col); //no alpha
		static void ImageCopyR(UnsafeArray<UInt8> destPtr, OSInt destBpl, UnsafeArray<const UInt8> srcPtr, OSInt srcBpl, OSInt srcX, OSInt srcY, UOSInt srcw, UOSInt srch, UOSInt bpp, Bool upSideDown, Media::RotateType srcRotate, Media::RotateType destRotate);
	};
}
#endif
