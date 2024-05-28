#ifndef _SM_MEDIA_CAMERAMANAGER
#define _SM_MEDIA_CAMERAMANAGER
#include "Media/IPhotoCapture.h"

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
		Optional<Media::IPhotoCapture> CreateDevice(OSInt devId);
	};
}
#endif
