#ifndef _SM_MEDIA_CS_CSRGB8_LRGBC
#define _SM_MEDIA_CS_CSRGB8_LRGBC
#include "Media/CS/CSConverter.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSRGB8_LRGBC : public Media::CS::CSConverter
		{
		private:
			UOSInt srcNBits;
			Media::PixelFormat srcPF;
			Media::ColorProfile *srcProfile;
			Media::ColorProfile *destProfile;
			Bool invert;

			Media::IColorHandler::RGBPARAM2 *rgbParam;
			UInt8 *srcPal;
			UInt8 *destPal;
			UInt8 *rgbTable;
			Bool rgbUpdated;
		private:
			void UpdateRGBTable();
		public:
			CSRGB8_LRGBC(UOSInt srcNBits, Media::PixelFormat srcPF, Bool invert, const Media::ColorProfile *srcProfile, const Media::ColorProfile *destProfile, Media::ColorManagerSess *colorSess);
			virtual ~CSRGB8_LRGBC();
			virtual void ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
			virtual UOSInt GetDestFrameSize(UOSInt width, UOSInt height);
			virtual void SetPalette(UInt8 *pal);

			virtual void YUVParamChanged(const YUVPARAM *yuv);
			virtual void RGBParamChanged(const RGBPARAM2 *rgb);
		};
	}
}
#endif
