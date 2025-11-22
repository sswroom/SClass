#ifndef _SM_MEDIA_VIDEOCAPTUREMGR
#define _SM_MEDIA_VIDEOCAPTUREMGR
#include "Data/ArrayListNN.hpp"
#include "Media/VideoCapturer.h"
#include "Text/CString.h"

namespace Media
{
	class VideoCaptureMgr
	{
	public:
		typedef struct
		{
			Int32 devType;
			UOSInt devId;
			UnsafeArray<const UTF8Char> devName;
		} DeviceInfo;
	private:
		struct ClassData;
		ClassData *clsData;
	public:
		VideoCaptureMgr();
		~VideoCaptureMgr();

		UOSInt GetDeviceList(NN<Data::ArrayListNN<DeviceInfo>> devList);
		void FreeDeviceList(NN<Data::ArrayListNN<DeviceInfo>> devList);
		Optional<Media::VideoCapturer> CreateDevice(Int32 devType, UOSInt devId);

		Text::CStringNN GetDevTypeName(Int32 devType);
	};
}
#endif
