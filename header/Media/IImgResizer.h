#ifndef _SM_MEDIA_IIMGRESIZER
#define _SM_MEDIA_IIMGRESIZER
#include "Media/StaticImage.h"

namespace Media
{
	class IImgResizer
	{
	public:
		typedef enum
		{
			RAR_SQUAREPIXEL,
			RAR_KEEPAR,
			RAR_IGNOREAR
		} ResizeAspectRatio;

	protected:
		UOSInt targetWidth;
		UOSInt targetHeight;
		ResizeAspectRatio rar;
		Media::AlphaType srcAlphaType;
	public:
		IImgResizer(Media::AlphaType srcAlphaType);
		virtual ~IImgResizer(){};

		virtual void Resize(const UInt8 *src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight) = 0;
		virtual Bool Resize(const Media::StaticImage *srcImage, Media::StaticImage *destImage) { return false;};

		virtual Bool IsSupported(const Media::FrameInfo *srcInfo) = 0;
		virtual Media::StaticImage *ProcessToNew(const Media::Image *srcImage);
		virtual Media::StaticImage *ProcessToNewPartial(const Media::Image *srcImage, Double srcX1, Double srcY1, Double srcX2, Double srcY2) = 0;
		void SetTargetWidth(UOSInt targetWidth);
		void SetTargetHeight(UOSInt targetHeight);
		void SetResizeAspectRatio(ResizeAspectRatio rar);
		void SetSrcAlphaType(Media::AlphaType alphaType);
		virtual void SetSrcRefLuminance(Double srcRefLuminance);

		static void CalOutputSize(const Media::FrameInfo *srcInfo, UOSInt targetWidth, UOSInt targetHeight, Media::FrameInfo *destInfo, ResizeAspectRatio rar);
	};
}
#endif
