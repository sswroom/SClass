#ifndef _SM_MEDIA_MMALSTILLCAPTURE
#define _SM_MEDIA_MMALSTILLCAPTURE
#include "Data/ArrayList.h"
#include "IO/Stream.h"
#include "Media/IPhotoCapture.h"

namespace Media
{
	class MMALStillCapture : public Media::IPhotoCapture
	{
	private:
		void *classData;

	public:
		MMALStillCapture();
		virtual ~MMALStillCapture();

		virtual Bool DeviceBegin();
		virtual void DeviceEnd();
		virtual Bool CapturePhoto(Media::IPhotoCapture::PhotoFormat *fmt, IO::Stream *stm);
	};
}
#endif
