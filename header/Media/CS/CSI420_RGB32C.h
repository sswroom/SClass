#ifndef _SM_MEDIA_CS_CSI420_RGB32C
#define _SM_MEDIA_CS_CSI420_RGB32C
#include "Media/CS/CSYUV420_RGB32C.h"

namespace Media
{
	namespace CS
	{
		class CSI420_RGB32C : public Media::CS::CSYUV420_RGB32C
		{
		public:
			CSI420_RGB32C(NotNullPtr<const Media::ColorProfile> srcProfile, NotNullPtr<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess, Media::PixelFormat destPF);
			virtual ~CSI420_RGB32C();
			virtual void ConvertV2(UInt8 *const*srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
		};
	}
}
#endif
