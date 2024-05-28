#ifndef _SM_MEDIA_OPENCV_OCVFRAME
#define _SM_MEDIA_OPENCV_OCVFRAME
#include "Math/RectArea.h"
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
			OCVFrame *CropToNew(Math::RectArea<UOSInt> *area);
			void ClearOutsidePolygon(Math::Coord2D<UOSInt> *poly, UOSInt nPoints, UInt8 color);
			UOSInt GetWidth() const;
			UOSInt GetHeight() const;
			Math::Size2D<UOSInt> GetSize() const;
			OSInt GetBpl();
			UInt8 *GetDataPtr();
			void GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate);
			Media::StaticImage *CreateStaticImage();
			void ToBlackAndWhite(UInt8 middleV);
			void Normalize();
			NN<Media::OpenCV::OCVFrame> BilateralFilter(Int32 d, Double sigmaColor, Double sigmaSpace);

			static Optional<OCVFrame> CreateYFrame(UInt8 **imgData, UOSInt dataSize, UInt32 fourcc, Math::Size2D<UOSInt> dispSize, UOSInt storeWidth, UOSInt storeBPP, Media::PixelFormat pf);
			static Optional<OCVFrame> CreateYFrame(NN<Media::StaticImage> simg);
		};
	}
}
#endif
