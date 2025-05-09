#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Media/DDCReader.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#include <setupapi.h>


Media::DDCReader::DDCReader(Optional<MonitorHandle> hMon)
{
	UnsafeArray<UInt8> edid;
	this->edid = 0;
	this->edidSize = 0;
	this->hMon = hMon;

	const GUID GUID_CLASS_MONITOR = {0x4d36e96e, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18};

	MONITORINFOEXW monInfo;
	monInfo.cbSize = sizeof(monInfo);
	if (!GetMonitorInfoW((HMONITOR)hMon.OrNull(), &monInfo))
	{
		return;
	}

	WChar wbuff[16];
	WChar monVID[4];
	UInt32 monPID;
	UOSInt k;

	DISPLAY_DEVICEW dd;
	MemClear(&dd, sizeof(dd));
	dd.cb = sizeof(dd);
	DWORD dev = 0;
	while ((this->edid == 0) && EnumDisplayDevicesW(0, dev, &dd, 0))
	{
		if (Text::StrEquals(dd.DeviceName, monInfo.szDevice))
		{
			DISPLAY_DEVICEW ddMon;
			MemClear(&ddMon, sizeof(ddMon));
			ddMon.cb = sizeof(ddMon);
			DWORD devMon = 0;
			
			while ((this->edid == 0) && EnumDisplayDevicesW(dd.DeviceName, devMon, &ddMon, 0))
			{
				if (ddMon.StateFlags & DISPLAY_DEVICE_ACTIVE && !(ddMon.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
				{
					k = Text::StrIndexOfCharW(&ddMon.DeviceID[1], '\\') + 2;
					monVID[0] = ddMon.DeviceID[k];
					monVID[1] = ddMon.DeviceID[k + 1];
					monVID[2] = ddMon.DeviceID[k + 2];
					monVID[3] = 0;
					Text::StrConcatS(wbuff, &ddMon.DeviceID[k + 3], 4);
					monPID = (UInt32)Text::StrHex2Int32WC(wbuff);

					HDEVINFO devInfo = SetupDiGetClassDevsEx(&GUID_CLASS_MONITOR, NULL, NULL, DIGCF_PRESENT, NULL, NULL, NULL);
					if (devInfo)
					{
						UInt32 i = 0;
						while (GetLastError() != ERROR_NO_MORE_ITEMS)
						{
							SP_DEVINFO_DATA devInfoData;
							MemClear(&devInfoData, sizeof(devInfoData));
							devInfoData.cbSize = sizeof(devInfoData);

							if (SetupDiEnumDeviceInfo(devInfo, i, &devInfoData))
							{
								HKEY hDevRegKey = SetupDiOpenDevRegKey(devInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
								if((hDevRegKey != 0) && (hDevRegKey != INVALID_HANDLE_VALUE))
								{
									UInt8 edidData[1024];
									DWORD edidSize;
									WChar nameEnt[128];
									DWORD nameLeng;
									DWORD dwType;
									UInt32 j = 0;
									while (true)
									{
										nameLeng = 128;
										edidSize = 1024;
										LONG retVal = RegEnumValueW(hDevRegKey, j, &nameEnt[0], &nameLeng, NULL, &dwType, edidData, &edidSize);
										if (retVal != ERROR_SUCCESS)
										{
											break;
										}
										if (Text::StrEquals(nameEnt, L"EDID"))
										{
											if (ReadUInt16(&edidData[10]) == monPID &&
												(monVID[0] == 0x40 + ((edidData[8] >> 2) & 0x1f)) &&
												(monVID[1] == 0x40 + ((edidData[9] >> 5) | ((edidData[8] << 3) & 0x1f))) &&
												(monVID[2] == 0x40 + (edidData[9] & 0x1f)))
											{
												this->edidSize = edidSize;
												this->edid = edid = MemAllocArr(UInt8, edidSize);
												MemCopyNO(edid.Ptr(), edidData, edidSize);
											}
											break;
										}
										j++;
									}
									RegCloseKey(hDevRegKey);
									if (this->edid.NotNull())
									{
										break;
									}
								}
							}

							i++;
						}
						SetupDiDestroyDeviceInfoList(devInfo);
					}
				}
				devMon++;
	 
				MemClear(&ddMon, sizeof(ddMon));
				ddMon.cb = sizeof(ddMon);
			}
		}
 
        MemClear(&dd, sizeof(dd));
        dd.cb = sizeof(dd);
        dev++;
    }
}

Media::DDCReader::DDCReader(UnsafeArray<const UTF8Char> monitorId)
{
	UnsafeArray<UInt8> edid;
	this->edid = 0;
	this->edidSize = 0;
	this->hMon = 0;

	const GUID GUID_CLASS_MONITOR = {0x4d36e96e, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18};

	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	UTF8Char monVID[4];
	UInt32 monPID;
	UOSInt k;
	UOSInt monitorIdLen = Text::StrCharCnt(monitorId);

	DISPLAY_DEVICEW dd;
	MemClear(&dd, sizeof(dd));
	dd.cb = sizeof(dd);
	DWORD dev = 0;
	while ((this->edid == 0) && EnumDisplayDevicesW(0, dev, &dd, 0))
	{
		DISPLAY_DEVICEW ddMon;
		MemClear(&ddMon, sizeof(ddMon));
		ddMon.cb = sizeof(ddMon);
		DWORD devMon = 0;
		
		while (!this->edid.SetTo(edid) && EnumDisplayDevicesW(dd.DeviceName, devMon, &ddMon, 0))
		{
			if (ddMon.StateFlags & DISPLAY_DEVICE_ACTIVE && !(ddMon.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
			{
				k = Text::StrIndexOfCharW(&ddMon.DeviceID[1], '\\') + 2;
				monVID[0] = (UTF8Char)ddMon.DeviceID[k];
				monVID[1] = (UTF8Char)ddMon.DeviceID[k + 1];
				monVID[2] = (UTF8Char)ddMon.DeviceID[k + 2];
				monVID[3] = 0;
				sptr = Text::StrWChar_UTF8C(sbuff, &ddMon.DeviceID[k + 3], 4);
				monPID = (UInt32)Text::StrHex2Int32C(sbuff);

				if (Text::StrStartsWithC(monitorId, monitorIdLen, monVID, 3) && Text::StrEqualsICaseC(&monitorId[3], monitorIdLen - 3, sbuff, (UOSInt)(sptr - sbuff)))
				{
					HDEVINFO devInfo = SetupDiGetClassDevsEx(&GUID_CLASS_MONITOR, NULL, NULL, DIGCF_PRESENT, NULL, NULL, NULL);
					if (devInfo)
					{
						UInt32 i = 0;
						while (GetLastError() != ERROR_NO_MORE_ITEMS)
						{
							SP_DEVINFO_DATA devInfoData;
							MemClear(&devInfoData, sizeof(devInfoData));
							devInfoData.cbSize = sizeof(devInfoData);

							if (SetupDiEnumDeviceInfo(devInfo, i, &devInfoData))
							{
								HKEY hDevRegKey = SetupDiOpenDevRegKey(devInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
								if((hDevRegKey != 0) && (hDevRegKey != INVALID_HANDLE_VALUE))
								{
									UInt8 edidData[1024];
									DWORD edidSize;
									WChar nameEnt[128];
									DWORD nameLeng;
									DWORD dwType;
									UInt32 j = 0;
									while (true)
									{
										nameLeng = 128;
										edidSize = 1024;
										LONG retVal = RegEnumValueW(hDevRegKey, j, &nameEnt[0], &nameLeng, NULL, &dwType, edidData, &edidSize);
										if (retVal != ERROR_SUCCESS)
										{
											break;
										}
										if (Text::StrEquals(nameEnt, L"EDID"))
										{
											if (ReadUInt16(&edidData[10]) == monPID &&
												(monVID[0] == 0x40 + ((edidData[8] >> 2) & 0x1f)) &&
												(monVID[1] == 0x40 + ((edidData[9] >> 5) | ((edidData[8] << 3) & 0x1f))) &&
												(monVID[2] == 0x40 + (edidData[9] & 0x1f)))
											{
												this->edidSize = edidSize;
												this->edid = edid = MemAllocArr(UInt8, edidSize);
												MemCopyNO(edid.Ptr(), edidData, edidSize);
											}
											break;
										}
										j++;
									}
									RegCloseKey(hDevRegKey);
									if (this->edid.NotNull())
									{
										break;
									}
								}
							}

							i++;
						}
						SetupDiDestroyDeviceInfoList(devInfo);
					}
				}
			}
			devMon++;
 
			MemClear(&ddMon, sizeof(ddMon));
			ddMon.cb = sizeof(ddMon);
		}
 
        MemClear(&dd, sizeof(dd));
        dd.cb = sizeof(dd);
        dev++;
    }
}

Media::DDCReader::DDCReader(UnsafeArray<UInt8> edid, UOSInt edidSize)
{
	UnsafeArray<UInt8> nnedid;
	this->edid = nnedid = MemAllocArr(UInt8, edidSize);
	this->edidSize = edidSize;
	this->hMon = 0;
	MemCopyNO(nnedid.Ptr(), edid.Ptr(), edidSize);
}

Media::DDCReader::~DDCReader()
{
	UnsafeArray<UInt8> nnedid;
	if (this->edid.SetTo(nnedid))
	{
		MemFreeArr(nnedid);
		this->edid = 0;
	}
}

UnsafeArrayOpt<UInt8> Media::DDCReader::GetEDID(OutParam<UOSInt> size)
{
	size.Set(this->edidSize);
	return this->edid;
}

UOSInt Media::DDCReader::CreateDDCReaders(NN<Data::ArrayListNN<DDCReader>> readerList)
{
	const GUID GUID_CLASS_MONITOR = {0x4d36e96e, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18};
	UOSInt ret = 0;
	NN<Media::DDCReader> reader;

	WChar wbuff[16];
	WChar monVID[4];
	UInt32 monPID;
	UOSInt k;
	Bool found;

	DISPLAY_DEVICEW dd;
	MemClear(&dd, sizeof(dd));
	dd.cb = sizeof(dd);
	DWORD dev = 0;
	while (EnumDisplayDevicesW(0, dev, &dd, 0))
	{
		DISPLAY_DEVICEW ddMon;
		MemClear(&ddMon, sizeof(ddMon));
		ddMon.cb = sizeof(ddMon);
		DWORD devMon = 0;
		found = false;
		
		while (!found && EnumDisplayDevicesW(dd.DeviceName, devMon, &ddMon, 0))
		{
			if (ddMon.StateFlags & DISPLAY_DEVICE_ACTIVE && !(ddMon.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
			{
				k = Text::StrIndexOfCharW(&ddMon.DeviceID[1], '\\') + 2;
				monVID[0] = ddMon.DeviceID[k];
				monVID[1] = ddMon.DeviceID[k + 1];
				monVID[2] = ddMon.DeviceID[k + 2];
				monVID[3] = 0;
				Text::StrConcatS(wbuff, &ddMon.DeviceID[k + 3], 4);
				monPID = (UInt32)Text::StrHex2Int32WC(wbuff);

				HDEVINFO devInfo = SetupDiGetClassDevsEx(&GUID_CLASS_MONITOR, NULL, NULL, DIGCF_PRESENT, NULL, NULL, NULL);
				if (devInfo)
				{
					UInt32 i = 0;
					while (GetLastError() != ERROR_NO_MORE_ITEMS)
					{
						SP_DEVINFO_DATA devInfoData;
						MemClear(&devInfoData, sizeof(devInfoData));
						devInfoData.cbSize = sizeof(devInfoData);

						if (SetupDiEnumDeviceInfo(devInfo, i, &devInfoData))
						{
							HKEY hDevRegKey = SetupDiOpenDevRegKey(devInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
							if((hDevRegKey != 0) && (hDevRegKey != INVALID_HANDLE_VALUE))
							{
								UInt8 edidData[1024];
								DWORD edidSize;
								WChar nameEnt[128];
								DWORD nameLeng;
								DWORD dwType;
								UInt32 j = 0;
								while (true)
								{
									nameLeng = 128;
									edidSize = 1024;
									LONG retVal = RegEnumValueW(hDevRegKey, j, &nameEnt[0], &nameLeng, NULL, &dwType, edidData, &edidSize);
									if (retVal != ERROR_SUCCESS)
									{
										break;
									}
									if (Text::StrEquals(nameEnt, L"EDID"))
									{
										if (ReadUInt16(&edidData[10]) == monPID &&
											(monVID[0] == 0x40 + ((edidData[8] >> 2) & 0x1f)) &&
											(monVID[1] == 0x40 + ((edidData[9] >> 5) | ((edidData[8] << 3) & 0x1f))) &&
											(monVID[2] == 0x40 + (edidData[9] & 0x1f)))
										{
											NEW_CLASSNN(reader, Media::DDCReader(edidData, edidSize));
											readerList->Add(reader);
											ret++;
										}
										break;
									}
									j++;
								}
								RegCloseKey(hDevRegKey);
								if (found)
								{
									break;
								}
							}
						}

						i++;
					}
					SetupDiDestroyDeviceInfoList(devInfo);
				}
			}
			devMon++;
 
			MemClear(&ddMon, sizeof(ddMon));
			ddMon.cb = sizeof(ddMon);
		}
 
        MemClear(&dd, sizeof(dd));
        dd.cb = sizeof(dd);
        dev++;
    }
	return ret;
}
