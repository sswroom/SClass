#ifndef _SM_MEDIA_CS_CSYUV_Y8
#define _SM_MEDIA_CS_CSYUV_Y8
#include "Media/CS/CSConverter.h"

namespace Media
{
	namespace CS
	{
		class CSYUV_Y8 : public Media::CS::CSConverter
		{
		private:
			UInt32 srcFmt;
		public:
			CSYUV_Y8(UInt32 srcFmt);
			virtual ~CSYUV_Y8();

			virtual void ConvertV2(UInt8 *const*srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
			virtual UOSInt GetDestFrameSize(UOSInt width, UOSInt height);

			virtual void YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::IColorHandler::RGBPARAM2> rgbParam);

			static Bool IsSupported(UInt32 fmt);
		};
	}
}
#endif
