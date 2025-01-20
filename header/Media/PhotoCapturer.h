#ifndef _SM_MEDIA_PHOTOCAPTURER
#define _SM_MEDIA_PHOTOCAPTURER
#include "IO/Stream.h"

namespace Media
{
	class PhotoCapturer
	{
	public:
		typedef enum
		{
			PF_PNG,
			PF_JPG
		} PhotoFormat;

	public:
		virtual ~PhotoCapturer() {}

		virtual Bool DeviceBegin() = 0;
		virtual void DeviceEnd() = 0;
		virtual Bool CapturePhoto(PhotoFormat *fmt, IO::Stream *stm) = 0;
	};
}
#endif
