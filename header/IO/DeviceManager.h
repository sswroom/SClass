//need setupapi.lib
#ifndef _SM_IO_DEVICEMANAGER
#define _SM_IO_DEVICEMANAGER
#include "Data/ArrayListNN.hpp"
#include "IO/DeviceInfo.h"

namespace IO
{
	class DeviceManager
	{
	public:
		DeviceManager();
		~DeviceManager();

		UIntOS QueryHIDDevices(NN<Data::ArrayListNN<IO::DeviceInfo>> devList);
		void FreeDevices(NN<Data::ArrayListNN<IO::DeviceInfo>> devList);
	};
}
#endif
