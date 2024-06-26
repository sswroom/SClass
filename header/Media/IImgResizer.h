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

		virtual void Resize(UnsafeArray<const UInt8> src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight) = 0;
		virtual Bool Resize(NN<const Media::StaticImage> srcImage, NN<Media::StaticImage> destImage) { return false;};

		virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo) = 0;
		virtual Media::StaticImage *ProcessToNew(NN<const Media::RasterImage> srcImage);
		virtual Media::StaticImage *ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR) = 0;
		void SetTargetSize(Math::Size2D<UOSInt> targetSize);
		void SetResizeAspectRatio(ResizeAspectRatio rar);
		void SetSrcAlphaType(Media::AlphaType alphaType);
		virtual void SetSrcRefLuminance(Double srcRefLuminance);

		static void CalOutputSize(NN<const Media::FrameInfo> srcInfo, Math::Size2D<UOSInt> targetSize, NN<Media::FrameInfo> destInfo, ResizeAspectRatio rar);
	};
}
#endif
