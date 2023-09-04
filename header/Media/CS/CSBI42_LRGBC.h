#ifndef _SM_MEDIA_CS_CSBI42_LRGBC
#define _SM_MEDIA_CS_CSBI42_LRGBC
#include "Media/CS/CSYUV420_LRGBC.h"

namespace Media
{
	namespace CS
	{
		class CSBI42_LRGBC : public Media::CS::CSYUV420_LRGBC
		{
		public:
			CSBI42_LRGBC(NotNullPtr<const Media::ColorProfile> srcProfile, NotNullPtr<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess);
			virtual ~CSBI42_LRGBC();
			virtual void ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
		};
	}
}
#endif
