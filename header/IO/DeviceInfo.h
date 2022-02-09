//need setupapi.lib
#ifndef _SM_IO_DEVICEINFO
#define _SM_IO_DEVICEINFO
#include "IO/Stream.h"
#include "Text/String.h"

namespace IO
{
	class DeviceInfo
	{
	private:
		Text::String *name;

	public:
		DeviceInfo(void *hDevInfo, void *interfData);
		~DeviceInfo();

		Text::String *GetName();
		IO::Stream *CreateStream();
	};
};
#endif
