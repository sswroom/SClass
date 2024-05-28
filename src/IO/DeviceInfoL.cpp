#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/DeviceInfo.h"
//#include "IO/DeviceStream.h"
#include "Text/MyString.h"

IO::DeviceInfo::DeviceInfo(void *hDevInfo, void *interfData)
{
/*	SP_DEVICE_INTERFACE_DATA *data = (SP_DEVICE_INTERFACE_DATA*)interfData;
	HDEVINFO devInfo = (HDEVINFO)hDevInfo;
	UInt32 reqSize;
	WChar data2[256];
	*(Int32*)data2 = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
	if (SetupDiGetDeviceInterfaceDetailW(devInfo, data, (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)&data2, 254, (DWORD*)&reqSize, 0))
	{
		this->name = Text::StrCopyNew(&data2[2]);
	}
	else
	{
		this->name = Text::StrCopyNew(L"Unknown");
	}*/

}

IO::DeviceInfo::~DeviceInfo()
{
//	Text::StrDelNew(this->name);
}

Text::String *IO::DeviceInfo::GetName()
{
	return this->name;
}

Optional<IO::Stream> IO::DeviceInfo::CreateStream()
{
/*	IO::DeviceStream *stm;
	NEW_CLASS(stm, IO::DeviceStream(this->name));
	return stm;*/
	return 0;
}
