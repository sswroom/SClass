#ifndef _SM_MEDIA_OPENCV_OCVFRAME
#define _SM_MEDIA_OPENCV_OCVFRAME
#include "Math/RectArea.hpp"
#include "Media/StaticImage.h"

namespace Media
{
	namespace OpenCV
	{
		class OCVFrame
		{
		private:
			void *frame;

		public:
			OCVFrame(void *frame);
			~OCVFrame();

			void *GetFrame();
			NN<OCVFrame> CropToNew(Math::RectArea<UIntOS> area);
			void ClearOutsidePolygon(UnsafeArray<Math::Coord2D<UIntOS>> poly, UIntOS nPoints, UInt8 color);
			UIntOS GetWidth() const;
			UIntOS GetHeight() const;
			Math::Size2D<UIntOS> GetSize() const;
			IntOS GetBpl();
			UInt8 *GetDataPtr();
			void GetImageData(UnsafeArray<UInt8> destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate);
			Media::StaticImage *CreateStaticImage();
			void ToBlackAndWhite(UInt8 middleV);
			void Normalize();
			NN<Media::OpenCV::OCVFrame> BilateralFilter(Int32 d, Double sigmaColor, Double sigmaSpace);

			static Optional<OCVFrame> CreateYFrame(UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, UInt32 fourcc, Math::Size2D<UIntOS> dispSize, UIntOS storeWidth, UIntOS storeBPP, Media::PixelFormat pf);
			static Optional<OCVFrame> CreateYFrame(NN<Media::StaticImage> simg);
		};
	}
}
#endif
