#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/DeviceManager.h"

#include <windows.h>
#include <setupapi.h>

IO::DeviceManager::DeviceManager()
{
}

IO::DeviceManager::~DeviceManager()
{
}

UOSInt IO::DeviceManager::QueryHIDDevices(NN<Data::ArrayListNN<IO::DeviceInfo>> devList)
{
	UOSInt retCnt = 0;
	UInt8 hidGuid[] = {0xb2, 0x55, 0x1e, 0x4d, 0x6f, 0xf1, 0xcf, 0x11, 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30};
	HDEVINFO devInfo = SetupDiGetClassDevs((GUID*)hidGuid, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	NN<IO::DeviceInfo> dev;

	if (devInfo)
	{
		DWORD i = 0;
		SP_DEVICE_INTERFACE_DATA data;
		while (true)
		{
			data.cbSize = sizeof(data);
			if (SetupDiEnumDeviceInterfaces(devInfo, 0, (GUID*)hidGuid, i, &data))
			{
				NEW_CLASSNN(dev, IO::DeviceInfo(devInfo, &data));
				devList->Add(dev);
				retCnt++;
				i++;
			}
			else
			{
				break;
			}
		}
		SetupDiDestroyDeviceInfoList(devInfo);
	}

	return retCnt;
}
void IO::DeviceManager::FreeDevices(NN<Data::ArrayListNN<IO::DeviceInfo>> devList)
{
	devList->DeleteAll();
}
