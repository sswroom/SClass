#ifndef _SM_MEDIA_IMAGERESIZER
#define _SM_MEDIA_IMAGERESIZER
#include "Media/StaticImage.h"

namespace Media
{
	class ImageResizer
	{
	public:
		typedef enum
		{
			RAR_SQUAREPIXEL,
			RAR_KEEPAR,
			RAR_IGNOREAR
		} ResizeAspectRatio;

	protected:
		Math::Size2D<UIntOS> targetSize;
		ResizeAspectRatio rar;
		Media::AlphaType srcAlphaType;
	public:
		ImageResizer(Media::AlphaType srcAlphaType);
		virtual ~ImageResizer(){};

		virtual void Resize(UnsafeArray<const UInt8> src, IntOS sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight) = 0;
		virtual Bool Resize(NN<const Media::StaticImage> srcImage, NN<Media::StaticImage> destImage) { return false;};

		virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo) = 0;
		virtual Optional<Media::StaticImage> ProcessToNew(NN<const Media::RasterImage> srcImage);
		virtual Optional<Media::StaticImage> ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR) = 0;
		void SetTargetSize(Math::Size2D<UIntOS> targetSize);
		void SetResizeAspectRatio(ResizeAspectRatio rar);
		void SetSrcAlphaType(Media::AlphaType alphaType);
		virtual void SetSrcRefLuminance(Double srcRefLuminance);

		static void CalOutputSize(NN<const Media::FrameInfo> srcInfo, Math::Size2D<UIntOS> targetSize, NN<Media::FrameInfo> destInfo, ResizeAspectRatio rar);
	};
}
#endif
