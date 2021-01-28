#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/DeviceManager.h"

IO::DeviceManager::DeviceManager()
{
}

IO::DeviceManager::~DeviceManager()
{
}

OSInt IO::DeviceManager::QueryHIDDevices(Data::ArrayList<IO::DeviceInfo*> *devList)
{
	OSInt retCnt = 0;
/*	UInt8 hidGuid[] = {0xb2, 0x55, 0x1e, 0x4d, 0x6f, 0xf1, 0xcf, 0x11, 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30};
	HDEVINFO devInfo = SetupDiGetClassDevs((GUID*)hidGuid, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	IO::DeviceInfo *dev;

	if (devInfo)
	{
		Int32 i = 0;
		SP_DEVICE_INTERFACE_DATA data;
		while (true)
		{
			data.cbSize = sizeof(data);
			if (SetupDiEnumDeviceInterfaces(devInfo, 0, (GUID*)hidGuid, i, &data))
			{
				NEW_CLASS(dev, IO::DeviceInfo(devInfo, &data));
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
	}*/

	return retCnt;
}
void IO::DeviceManager::FreeDevices(Data::ArrayList<IO::DeviceInfo*> *devList)
{
	OSInt i;
	IO::DeviceInfo *dev;	
	i = devList->GetCount();
	while (i-- > 0)
	{
		dev = devList->GetItem(i);
		DEL_CLASS(dev);
	}
	devList->Clear();
}
