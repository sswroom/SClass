//need setupapi.lib
#ifndef _SM_IO_DEVICEMANAGER
#define _SM_IO_DEVICEMANAGER
#include "Data/ArrayList.h"
#include "IO/DeviceInfo.h"

namespace IO
{
	class DeviceManager
	{
	public:
		DeviceManager();
		~DeviceManager();

		OSInt QueryHIDDevices(Data::ArrayList<IO::DeviceInfo*> *devList);
		void FreeDevices(Data::ArrayList<IO::DeviceInfo*> *devList);
	};
};
#endif
