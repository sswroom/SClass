#ifndef _SM_MEDIA_MMALSTILLCAPTURE
#define _SM_MEDIA_MMALSTILLCAPTURE
#include "Data/ArrayList.h"
#include "IO/Stream.h"
#include "Media/PhotoCapturer.h"

namespace Media
{
	class MMALStillCapture : public Media::PhotoCapturer
	{
	public:
		struct ClassData;
	private:
		NN<ClassData> classData;

	public:
		MMALStillCapture();
		virtual ~MMALStillCapture();

		virtual Bool DeviceBegin();
		virtual void DeviceEnd();
		virtual Bool CapturePhoto(OutParam<Media::PhotoCapturer::PhotoFormat> fmt, NN<IO::Stream> stm);
	};
}
#endif
