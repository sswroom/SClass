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

			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
			virtual UOSInt GetDestFrameSize(UOSInt width, UOSInt height);

			virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);

			static Bool IsSupported(UInt32 fmt);
		};
	}
}
#endif
