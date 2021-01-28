//need setupapi.lib
#ifndef _SM_IO_DEVICEINFO
#define _SM_IO_DEVICEINFO
#include "IO/Stream.h"

namespace IO
{
	class DeviceInfo
	{
	private:
		const UTF8Char *name;

	public:
		DeviceInfo(void *hDevInfo, void *interfData);
		~DeviceInfo();

		const UTF8Char *GetName();
		IO::Stream *CreateStream();
	};
};
#endif
