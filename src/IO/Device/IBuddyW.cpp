#include "Stdafx.h"
#include "Text/MyStringW.h"
#include "IO/Device/IBuddy.h"

#include <windows.h>
#include <setupapi.h>

void IO::Device::IBuddy::InitDevice(UnsafeArray<const UTF8Char> devName)
{
//	wprintf(L"%s\r\n", devName);
	SECURITY_ATTRIBUTES attr;
	attr.nLength = sizeof(attr);
	attr.bInheritHandle = true;
	attr.lpSecurityDescriptor = 0;

	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(devName);
	this->hand = CreateFileW(wptr.Ptr(), 0xC0000000, FILE_SHARE_WRITE | FILE_SHARE_READ, &attr, OPEN_EXISTING, 0, 0);
	Text::StrDelNew(wptr);
	this->lastEffect = 0;
}

UIntOS IO::Device::IBuddy::GetNumDevice()
{
	Int32 ret = 0;
	UInt8 hidGuid[] = {0xb2, 0x55, 0x1e, 0x4d, 0x6f, 0xf1, 0xcf, 0x11, 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30};
	HDEVINFO devInfo = SetupDiGetClassDevs((GUID*)hidGuid, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (devInfo)
	{
		Int32 i = 0;
		SP_DEVICE_INTERFACE_DATA data;
		while (true)
		{
			data.cbSize = sizeof(data);
			if (SetupDiEnumDeviceInterfaces(devInfo, 0, (GUID*)hidGuid, i, &data))
			{
				UInt32 reqSize;
				i++;
				if (SetupDiGetDeviceInterfaceDetail(devInfo, &data, 0, 0, (DWORD*)&reqSize, 0))
				{
				}
				else
				{
					WChar data2[256];
					*(Int32*)data2 = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
					if (SetupDiGetDeviceInterfaceDetailW(devInfo, &data, (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)&data2, 254, (DWORD*)&reqSize, 0))
					{
						if (Text::StrIndexOfW(&data2[2], L"vid_1130&pid_") != INVALID_INDEX)
						{
							if (Text::StrIndexOfW(&data2[2], L"mi_01") != INVALID_INDEX)
							{
								ret++;
							}
						}
					}
				}
			}
			else
			{
				break;
			}
		}
		SetupDiDestroyDeviceInfoList(devInfo);
	}
	return ret;
}

IO::Device::IBuddy::IBuddy(UIntOS devNo)
{
	UIntOS currId = 0;
	UInt8 hidGuid[] = {0xb2, 0x55, 0x1e, 0x4d, 0x6f, 0xf1, 0xcf, 0x11, 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30};
	this->hand = 0;
	HDEVINFO devInfo = SetupDiGetClassDevs((GUID*)hidGuid, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (devInfo)
	{
		Int32 i = 0;
		SP_DEVICE_INTERFACE_DATA data;
		while (true)
		{
			data.cbSize = sizeof(data);
			if (SetupDiEnumDeviceInterfaces(devInfo, 0, (GUID*)hidGuid, i, &data))
			{
				UInt32 reqSize;
				i++;
				if (SetupDiGetDeviceInterfaceDetail(devInfo, &data, 0, 0, (DWORD*)&reqSize, 0))
				{
				}
				else
				{
					WChar data2[256];
					*(Int32*)data2 = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
					if (SetupDiGetDeviceInterfaceDetailW(devInfo, &data, (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)&data2, 254, (DWORD*)&reqSize, 0))
					{
						if (Text::StrIndexOfW(&data2[2], L"vid_1130&pid_") != INVALID_INDEX)
						{
							if (Text::StrIndexOfW(&data2[2], L"mi_01") != INVALID_INDEX)
							{
								if (currId == devNo)
								{
									UnsafeArray<const UTF8Char> sptr = Text::StrToUTF8New(&data2[2]);
									InitDevice(sptr);
									Text::StrDelNew(sptr);
									break;
								}
								currId++;
							}
						}
					}
				}
			}
			else
			{
				break;
			}
		}
		SetupDiDestroyDeviceInfoList(devInfo);
	}
}

IO::Device::IBuddy::~IBuddy()
{
	if (this->hand)
	{
		if (lastEffect)
		{
			PlayEffect(IBuddy::IBBE_OFF, IBuddy::IBHDE_OFF, IBuddy::IBHRE_OFF, IBuddy::IBWE_OFF);
		}
		CloseHandle((HANDLE)hand);
		this->hand = 0;
	}
}

Bool IO::Device::IBuddy::IsError()
{
	return this->hand == 0;
}

void IO::Device::IBuddy::PlayEffect(IBuddyBodyEffect be, IBuddyHeadEffect hde, IBuddyHeartEffect hre, IBuddyWingEffect we)
{
	UInt8 effects = (UInt8)(be | hde | hre | we);
	UInt8 buff[9];
	buff[0] = 0x00;
	buff[1] = 0x55;
	buff[2] = 0x53;
	buff[3] = 0x42;
	buff[4] = 0x43;
	buff[5] = 0x00;
	buff[6] = 0x40;
	buff[7] = 0x02;
	buff[8] = 255 - effects;
	if (this->hand)
	{
		UInt32 size;
		WriteFile((HANDLE)this->hand, buff, 9, (LPDWORD)&size, 0);
		lastEffect = effects;
	}
}
