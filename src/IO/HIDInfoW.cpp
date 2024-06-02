#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/HIDInfo.h"
#include "IO/Path.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#include <windows.h>
#include <setupapi.h>

#include <stdio.h>
#undef FindNextFile

struct IO::HIDInfo::ClassData
{
IO::HIDInfo::BusType busType;
UInt16 vendor;
UInt16 product;
NN<Text::String> devPath;
};


IO::HIDInfo::HIDInfo(NN<ClassData> clsData)
{
	this->clsData = clsData;
}

IO::HIDInfo::~HIDInfo()
{
	this->clsData->devPath->Release();
	MemFreeNN(this->clsData);
}

IO::HIDInfo::BusType IO::HIDInfo::GetBusType()
{
	return this->clsData->busType;
}

UInt16 IO::HIDInfo::GetVendorId()
{
	return this->clsData->vendor;
}

UInt16 IO::HIDInfo::GetProductId()
{
	return this->clsData->product;
}

Text::String *IO::HIDInfo::GetDevPath()
{
	return this->clsData->devPath.Ptr();
}

IO::Stream *IO::HIDInfo::OpenHID()
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(this->clsData->devPath, IO::FileMode::ReadWriteExisting, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return 0;
	}
	else
	{
		return fs;
	}
}

OSInt IO::HIDInfo::GetHIDList(NN<Data::ArrayListNN<HIDInfo>> hidList)
{
	OSInt ret = 0;
	NN<ClassData> clsData;
	NN<IO::HIDInfo> hid;
	UInt8 hidGuid[] = {0xb2, 0x55, 0x1e, 0x4d, 0x6f, 0xf1, 0xcf, 0x11, 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30};
	HDEVINFO devInfo = SetupDiGetClassDevs((GUID*)hidGuid, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (devInfo)
	{
		UInt32 i = 0;
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
						UOSInt j;
						clsData = MemAllocNN(ClassData);
						clsData->busType = IO::HIDInfo::BT_USB;
						clsData->devPath = Text::String::NewNotNull(&data2[2]);
						clsData->product = 0;
						clsData->vendor = 0;
						j = Text::StrIndexOf(&data2[2], L"vid_");
						if (j != INVALID_INDEX && data2[2 + 8 + j] == '&')
						{
							data2[2 + 8 + j] = 0;
							clsData->vendor = Text::StrHex2UInt16C(&data2[2 + 4 + j]);
							data2[2 + 8 + j] = '&';
						}
						j = Text::StrIndexOf(&data2[2], L"pid_");
						if (j != INVALID_INDEX && data2[2 + 8 + j] == '&')
						{
							data2[2 + 8 + j] = 0;
							clsData->product = Text::StrHex2UInt16C(&data2[2 + 4 + j]);
							data2[2 + 8 + j] = '&';
						}
						else if (j != INVALID_INDEX && data2[2 + 8 + j] == '#')
						{
							data2[2 + 8 + j] = 0;
							clsData->product = Text::StrHex2UInt16C(&data2[2 + 4 + j]);
							data2[2 + 8 + j] = '#';
						}
						
						NEW_CLASSNN(hid, IO::HIDInfo(clsData));
						hidList->Add(hid);
						ret++;
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
	else
	{
		UTF8Char sbuff[512];
		UTF8Char sbuff2[512];
		UnsafeArray<UTF8Char> sptr;
		UnsafeArray<UTF8Char> sptr2;
		UnsafeArray<UTF8Char> sptr3;
		IO::Path::FindFileSession *sess;
		IO::Path::FindFileSession *sess2;
		IO::Path::PathType pt;
		Int32 busType;
		sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/bus/hid/devices/"));
		sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
		sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
		if (sess)
		{
			while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0).SetTo(sptr2))
			{
				if ((sptr2 - sptr) == 19 && sptr[4] == ':' && sptr[9] == ':' && sptr[14] == '.' && sptr[19] == 0)
				{
					clsData = MemAllocNN(ClassData);
					sptr[4] = 0;
					busType = Text::StrHex2UInt16C(sptr);
					sptr[4] = ':';
					sptr[9] = 0;
					clsData->vendor = Text::StrHex2UInt16C(&sptr[5]);
					sptr[9] = ':';
					sptr[14] = 0;
					clsData->product = Text::StrHex2UInt16C(&sptr[10]);
					sptr[14] = '.';
					switch (busType)
					{
					case 3:
						clsData->busType = IO::HIDInfo::BT_USB;
						break;
					case 24:
						clsData->busType = IO::HIDInfo::BT_I2C;
						break;
					default:
						clsData->busType = IO::HIDInfo::BT_UNKNOWN;
						break;
					}
					sptr2 = Text::StrConcatC(sptr2, UTF8STRC("/hidraw/"));
					sptr3 = Text::StrConcatC(sptr2, UTF8STRC("hidraw*"));
					sess2 = IO::Path::FindFile(CSTRP(sbuff, sptr3));
					if (sess2)
					{
						if (IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0).NotNull())
						{
							sptr3 = Text::StrConcat(Text::StrConcatC(sbuff2, UTF8STRC("/dev/")), sptr2);
							clsData->devPath = Text::String::New(sbuff2, (UOSInt)(sptr3 - sbuff2));
							NEW_CLASSNN(hid, IO::HIDInfo(clsData));
							hidList->Add(hid);
							ret++;
						}
						else
						{
							MemFreeNN(clsData);
						}					
						IO::Path::FindFileClose(sess2);
					}
					else
					{
						MemFreeNN(clsData);
					}
					
				}
				else
				{
				}			
			}
			IO::Path::FindFileClose(sess);
		}
	}
	return ret;
}