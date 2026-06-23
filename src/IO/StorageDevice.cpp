#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/Encoding.h"
#include "IO/StorageDevice.h"

#include <windows.h>
#include <devguid.h>
#include <setupapi.h>

void IO::StorageDevice::InitDevice(WChar *devName)
{
	SECURITY_ATTRIBUTES attr;
	attr.nLength = sizeof(attr);
	attr.bInheritHandle = true;
	attr.lpSecurityDescriptor = 0;

	this->hand = CreateFileW(devName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, &attr, OPEN_EXISTING, 0, 0);
}

Bool IO::StorageDevice::GetStorDesc()
{
	UnsafeArray<UInt8> storDesc;
	if (this->storDesc.NotNull())
		return true;
	UInt8 outBuff[512];
	BOOL status;
	DWORD returnedLength;
	STORAGE_PROPERTY_QUERY query;

	query.PropertyId = StorageDeviceProperty;
	query.QueryType = PropertyStandardQuery;

	status = DeviceIoControl((HANDLE)this->hand, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(STORAGE_PROPERTY_QUERY), &outBuff, 512, &returnedLength, NULL);
    if (!status)
	{
		return false;
    }
	this->storDesc = storDesc = MemAllocArr(UInt8, returnedLength);
	MemCopyNO(&storDesc[0], outBuff, returnedLength);
	return true;
}

Int32 IO::StorageDevice::GetNumDevice()
{
	Int32 ret = 0;
	HDEVINFO devInfo = SetupDiGetClassDevs((LPGUID)&DiskClassGuid, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (devInfo)
	{
		Int32 i = 0;
		SP_DEVICE_INTERFACE_DATA data;
		while (true)
		{
			data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
			if (SetupDiEnumDeviceInterfaces(devInfo, 0, (LPGUID)&DiskClassGuid, i, &data))
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
	return ret;
}

IO::StorageDevice::StorageDevice(Int32 devNo)
{
	Int32 currId = 0;
	this->hand = 0;
	this->storDesc = nullptr;

	HDEVINFO devInfo = SetupDiGetClassDevs((LPGUID)&DiskClassGuid, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (devInfo)
	{
		Int32 i = 0;
		SP_DEVICE_INTERFACE_DATA data;
		while (true)
		{
			data.cbSize = sizeof(data);
			if (SetupDiEnumDeviceInterfaces(devInfo, 0, (LPGUID)&DiskClassGuid, i, &data))
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
						if (currId == devNo)
						{
							InitDevice(&data2[2]);
							break;
						}
						currId++;
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

IO::StorageDevice::~StorageDevice()
{
	if (this->hand)
	{
		CloseHandle((HANDLE)hand);
		this->hand = 0;
	}
	UnsafeArray<UInt8> storDesc;
	if (this->storDesc.SetTo(storDesc))
	{
		MemFreeArr(storDesc);
		this->storDesc = nullptr;
	}
}

Bool IO::StorageDevice::IsError()
{
	return this->hand == 0;
}

Bool IO::StorageDevice::IsRemovable()
{
	STORAGE_DEVICE_DESCRIPTOR *devDesc;
	UnsafeArray<UInt8> storDesc;
	if (!this->GetStorDesc() || !this->storDesc.SetTo(storDesc))
		return 0;
	devDesc = (STORAGE_DEVICE_DESCRIPTOR*)&storDesc[0];
	return devDesc->RemovableMedia != 0;
}

Bool IO::StorageDevice::SupportCommandQueue()
{
	STORAGE_DEVICE_DESCRIPTOR *devDesc;
	UnsafeArray<UInt8> storDesc;
	if (!this->GetStorDesc() || !this->storDesc.SetTo(storDesc))
		return 0;
	devDesc = (STORAGE_DEVICE_DESCRIPTOR*)&storDesc[0];
	return devDesc->CommandQueueing != 0;
}

UnsafeArrayOpt<UTF8Char> IO::StorageDevice::GetVendorID(UnsafeArray<UTF8Char> sbuff)
{
	STORAGE_DEVICE_DESCRIPTOR *devDesc;
	UnsafeArray<UInt8> storDesc;
	if (!this->GetStorDesc() || !this->storDesc.SetTo(storDesc))
		return nullptr;
	Text::Encoding enc;
	devDesc = (STORAGE_DEVICE_DESCRIPTOR*)&storDesc[0];
	if (devDesc->VendorIdOffset == 0)
		return nullptr;
	return enc.UTF8FromBytes(sbuff, &storDesc[(UInt32)devDesc->VendorIdOffset], -1, 0);
}

UnsafeArrayOpt<UTF8Char> IO::StorageDevice::GetProductID(UnsafeArray<UTF8Char> sbuff)
{
	STORAGE_DEVICE_DESCRIPTOR *devDesc;
	UnsafeArray<UInt8> storDesc;
	if (!this->GetStorDesc() || !this->storDesc.SetTo(storDesc))
		return nullptr;
	Text::Encoding enc;
	devDesc = (STORAGE_DEVICE_DESCRIPTOR*)&storDesc[0];
	if (devDesc->ProductIdOffset == 0)
		return nullptr;
	return enc.UTF8FromBytes(sbuff, &storDesc[(UInt32)devDesc->ProductIdOffset], -1, 0);
}

UnsafeArrayOpt<UTF8Char> IO::StorageDevice::GetProductRevision(UnsafeArray<UTF8Char> sbuff)
{
	STORAGE_DEVICE_DESCRIPTOR *devDesc;
	UnsafeArray<UInt8> storDesc;
	if (!this->GetStorDesc() || !this->storDesc.SetTo(storDesc))
		return nullptr;
	Text::Encoding enc;
	devDesc = (STORAGE_DEVICE_DESCRIPTOR*)&storDesc[0];
	if (devDesc->ProductRevisionOffset == 0)
		return nullptr;
	return enc.UTF8FromBytes(sbuff, &storDesc[(UInt32)devDesc->ProductRevisionOffset], -1, 0);
}

UnsafeArrayOpt<UTF8Char> IO::StorageDevice::GetSerialNumber(UnsafeArray<UTF8Char> sbuff)
{
	STORAGE_DEVICE_DESCRIPTOR *devDesc;
	UnsafeArray<UInt8> storDesc;
	if (!this->GetStorDesc() || !this->storDesc.SetTo(storDesc))
		return nullptr;
	Text::Encoding enc;
	devDesc = (STORAGE_DEVICE_DESCRIPTOR*)&storDesc[0];
	if (devDesc->SerialNumberOffset == 0)
		return nullptr;
	return enc.UTF8FromBytes(sbuff, &storDesc[(UInt32)devDesc->SerialNumberOffset], -1, 0);
}

Bool IO::StorageDevice::GetDiskGeometry(OutParam<UInt64> cylinder, OutParam<UInt32> trackPerCylinder, OutParam<UInt32> sectorPerTrack, OutParam<UInt32> bytesPerSector)
{
	DISK_GEOMETRY geom;
	BOOL status;
	DWORD returnedLength;

	status = DeviceIoControl((HANDLE)this->hand,  IOCTL_DISK_GET_DRIVE_GEOMETRY, 0, 0, &geom, sizeof(DISK_GEOMETRY), &returnedLength, NULL);
	if (status == 0)
		return false;

	cylinder.Set(geom.Cylinders.QuadPart);
	trackPerCylinder.Set(geom.TracksPerCylinder);
	sectorPerTrack.Set(geom.SectorsPerTrack);
	bytesPerSector.Set(geom.BytesPerSector);
	return true;
}

Bool IO::StorageDevice::SMARTGetVersion(OutParam<UInt8> ver, OutParam<UInt8> rev, OutParam<Bool> supportATAID, OutParam<Bool> supportATAPIID, OutParam<Bool> supportSMART)
{
	GETVERSIONINPARAMS getVersionParams;
	BOOL status;
	DWORD returnedLength;
	MemClear(&getVersionParams, sizeof(GETVERSIONINPARAMS));

	status = DeviceIoControl((HANDLE)this->hand, SMART_GET_VERSION, 0, 0, &getVersionParams, sizeof(GETVERSIONINPARAMS), &returnedLength, NULL);
	if (status == 0)
		return false;
	ver.Set(getVersionParams.bVersion);
	rev.Set(getVersionParams.bRevision);
	supportATAID.Set((getVersionParams.fCapabilities & CAP_ATA_ID_CMD) != 0);
	supportATAPIID.Set((getVersionParams.fCapabilities & CAP_ATAPI_ID_CMD) != 0);
	supportSMART.Set((getVersionParams.fCapabilities & CAP_SMART_CMD) != 0);
	return true;
}

Bool IO::StorageDevice::SMARTGetDiskID(UnsafeArray<UInt8> idSector)
{
	const IntOS buffSize = sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE;
	UInt8 buff[buffSize];
	DWORD outSize = 0;
	SENDCMDINPARAMS params;
//#define ID_CMD          0xEC            // Returns ID sector for ATA
	params.irDriveRegs.bCommandReg = ID_CMD;
	if (!DeviceIoControl((HANDLE)this->hand, SMART_RCV_DRIVE_DATA, &params, sizeof(SENDCMDINPARAMS), buff, buffSize, &outSize, NULL))
		return false;

	SENDCMDOUTPARAMS *outParams = (SENDCMDOUTPARAMS*)buff;
	UInt8 *src  = outParams->bBuffer;
	UnsafeArray<UInt16> dest = UnsafeArray<UInt16>::ConvertFrom(idSector);
	IntOS i = 256;
	while (i-- > 0)
	{
		*dest = (*src << 8) | src[1];
		dest++;
		src += 2;
	}
	return true;
}

UnsafeArray<UTF8Char> IO::StorageDevice::IDSectorGetSN(UnsafeArray<UTF8Char> sbuff, UnsafeArray<UInt8> idSector)
{
	Text::Encoding enc;
	enc.UTF8FromBytes(sbuff, &idSector[20], 20, 0);
	return Text::StrTrim(sbuff);
}
