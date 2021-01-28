#ifndef _SM_MEDIA_IPHOTOCAPTURE
#define _SM_MEDIA_IPHOTOCAPTURE
#include "IO/Stream.h"

namespace Media
{
	class IPhotoCapture
	{
	public:
		typedef enum
		{
			PF_PNG,
			PF_JPG
		} PhotoFormat;

	public:
		virtual ~IPhotoCapture() {}

		virtual Bool DeviceBegin() = 0;
		virtual void DeviceEnd() = 0;
		virtual Bool CapturePhoto(PhotoFormat *fmt, IO::Stream *stm) = 0;
	};
};
#endif
