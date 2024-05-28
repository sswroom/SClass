#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/DeviceInfo.h"
#include "IO/DeviceStream.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#include <windows.h>
#include <setupapi.h>

IO::DeviceInfo::DeviceInfo(void *hDevInfo, void *interfData)
{
	SP_DEVICE_INTERFACE_DATA *data = (SP_DEVICE_INTERFACE_DATA*)interfData;
	HDEVINFO devInfo = (HDEVINFO)hDevInfo;
	UInt32 reqSize;
	WChar data2[256];
	*(Int32*)data2 = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
	if (SetupDiGetDeviceInterfaceDetailW(devInfo, data, (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)&data2, 254, (DWORD*)&reqSize, 0))
	{
		this->name = Text::String::NewNotNull(&data2[2]).Ptr();
	}
	else
	{
		this->name = Text::String::New(UTF8STRC("Unknown")).Ptr();
	}

}

IO::DeviceInfo::~DeviceInfo()
{
	this->name->Release();
}

Text::String *IO::DeviceInfo::GetName()
{
	return this->name;
}

Optional<IO::Stream> IO::DeviceInfo::CreateStream()
{
	IO::DeviceStream *stm;
	NEW_CLASS(stm, IO::DeviceStream(this->name->ToCString()));
	return stm;
}
