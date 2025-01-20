#ifndef _SM_MEDIA_CAMERAMANAGER
#define _SM_MEDIA_CAMERAMANAGER
#include "Media/PhotoCapturer.h"

namespace Media
{
	class CameraManager
	{
	private:
		void *mgrData;
	public:
		CameraManager();
		~CameraManager();

		OSInt GetDeviceCount();
		Optional<Media::PhotoCapturer> CreateDevice(OSInt devId);
	};
}
#endif
