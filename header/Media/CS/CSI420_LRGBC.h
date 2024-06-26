#ifndef _SM_MEDIA_CS_CSI420_LRGBC
#define _SM_MEDIA_CS_CSI420_LRGBC
#include "Media/CS/CSYUV420_LRGBC.h"

namespace Media
{
	namespace CS
	{
		class CSI420_LRGBC : public Media::CS::CSYUV420_LRGBC
		{
		public:
			CSI420_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSI420_LRGBC();
			virtual void ConvertV2(UnsafeArray<UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
		};
	}
}
#endif
