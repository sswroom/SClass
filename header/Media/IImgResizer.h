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
		Math::Size2D<UOSInt> targetSize;
		ResizeAspectRatio rar;
		Media::AlphaType srcAlphaType;
	public:
		IImgResizer(Media::AlphaType srcAlphaType);
		virtual ~IImgResizer(){};

		virtual void Resize(const UInt8 *src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight) = 0;
		virtual Bool Resize(NotNullPtr<const Media::StaticImage> srcImage, NotNullPtr<Media::StaticImage> destImage) { return false;};

		virtual Bool IsSupported(NotNullPtr<const Media::FrameInfo> srcInfo) = 0;
		virtual Media::StaticImage *ProcessToNew(NotNullPtr<const Media::Image> srcImage);
		virtual Media::StaticImage *ProcessToNewPartial(NotNullPtr<const Media::Image> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR) = 0;
		void SetTargetSize(Math::Size2D<UOSInt> targetSize);
		void SetResizeAspectRatio(ResizeAspectRatio rar);
		void SetSrcAlphaType(Media::AlphaType alphaType);
		virtual void SetSrcRefLuminance(Double srcRefLuminance);

		static void CalOutputSize(NotNullPtr<const Media::FrameInfo> srcInfo, Math::Size2D<UOSInt> targetSize, NotNullPtr<Media::FrameInfo> destInfo, ResizeAspectRatio rar);
	};
}
#endif
