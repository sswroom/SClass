#ifndef _SM_MEDIA_IMAGEUTIL
#define _SM_MEDIA_IMAGEUTIL
#include "Media/FrameInfo.h"

namespace Media
{
	class ImageUtil
	{
	public:
		static void ColorReplaceAlpha32(UnsafeArray<UInt8> pixelPtr, UIntOS w, UIntOS h, UInt32 col);
		static void DrawHLineNA32(UnsafeArray<UInt8> pixelPtr, UIntOS w, UIntOS h, UIntOS bpl, IntOS y, IntOS x1, IntOS x2, UInt32 col); //no alpha
		static void DrawVLineNA32(UnsafeArray<UInt8> pixelPtr, UIntOS w, UIntOS h, UIntOS bpl, IntOS x, IntOS y1, IntOS y2, UInt32 col); //no alpha
		static void ImageCopyR(UnsafeArray<UInt8> destPtr, IntOS destBpl, UnsafeArray<const UInt8> srcPtr, IntOS srcBpl, IntOS srcX, IntOS srcY, UIntOS srcw, UIntOS srch, UIntOS bpp, Bool upSideDown, Media::RotateType srcRotate, Media::RotateType destRotate);
	};
}
#endif
