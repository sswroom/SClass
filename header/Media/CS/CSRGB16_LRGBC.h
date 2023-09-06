#ifndef _SM_MEDIA_CS_CSRGB16_LRGBC
#define _SM_MEDIA_CS_CSRGB16_LRGBC
#include "Media/CS/CSConverter.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSRGB16_LRGBC : public Media::CS::CSConverter
		{
		private:
			UOSInt srcNBits;
			Media::PixelFormat srcPF;
			Media::ColorProfile srcProfile;
			Media::ColorProfile destProfile;
			Bool invert;

			Media::IColorHandler::RGBPARAM2 rgbParam;
			UInt8 *rgbTable;
			Bool rgbUpdated;
		private:
			void UpdateRGBTable();
		public:
			CSRGB16_LRGBC(UOSInt srcNBits, Media::PixelFormat srcPF, Bool invert, NotNullPtr<const Media::ColorProfile> srcProfile, NotNullPtr<const Media::ColorProfile> destProfile, Media::ColorManagerSess *colorSess);
			virtual ~CSRGB16_LRGBC();
			virtual void ConvertV2(UInt8 *const*srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
			virtual UOSInt GetDestFrameSize(UOSInt width, UOSInt height);
			virtual void SetPalette(UInt8 *pal);

			virtual void YUVParamChanged(NotNullPtr<const YUVPARAM> yuv);
			virtual void RGBParamChanged(NotNullPtr<const RGBPARAM2> rgb);
		};
	}
}
#endif
