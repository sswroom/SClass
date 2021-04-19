#ifndef _SM_MEDIA_VIDEOCAPTUREMGR
#define _SM_MEDIA_VIDEOCAPTUREMGR
#include "Data/ArrayList.h"
#include "Media/IVideoCapture.h"

namespace Media
{
	class VideoCaptureMgr
	{
	public:
		typedef struct
		{
			Int32 devType;
			UOSInt devId;
			const UTF8Char *devName;
		} DeviceInfo;
	private:
		void *mgrData;
	public:
		VideoCaptureMgr();
		~VideoCaptureMgr();

		UOSInt GetDeviceList(Data::ArrayList<DeviceInfo *> *devList);
		void FreeDeviceList(Data::ArrayList<DeviceInfo *> *devList);
		Media::IVideoCapture *CreateDevice(Int32 devType, UOSInt devId);

		const UTF8Char *GetDevTypeName(Int32 devType);
	};
}
#endif
