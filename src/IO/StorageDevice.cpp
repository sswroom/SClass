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
	if (this->storDesc)
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
	this->storDesc = MemAlloc(UInt8, returnedLength);
	MemCopy(this->storDesc, outBuff, returnedLength);
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
	this->storDesc = 0;

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
	if (this->storDesc)
	{
		MemFree(this->storDesc);
		this->storDesc = 0;
	}
}

Bool IO::StorageDevice::IsError()
{
	return this->hand == 0;
}

Bool IO::StorageDevice::IsRemovable()
{
	STORAGE_DEVICE_DESCRIPTOR *devDesc;
	if (!this->GetStorDesc())
		return 0;
	devDesc = (STORAGE_DEVICE_DESCRIPTOR*)this->storDesc;
	return devDesc->RemovableMedia != 0;
}

Bool IO::StorageDevice::SupportCommandQueue()
{
	STORAGE_DEVICE_DESCRIPTOR *devDesc;
	if (!this->GetStorDesc())
		return 0;
	devDesc = (STORAGE_DEVICE_DESCRIPTOR*)this->storDesc;
	return devDesc->CommandQueueing != 0;
}

WChar *IO::StorageDevice::GetVendorID(WChar *sbuff)
{
	STORAGE_DEVICE_DESCRIPTOR *devDesc;
	if (!this->GetStorDesc())
		return 0;
	Text::Encoding enc;
	devDesc = (STORAGE_DEVICE_DESCRIPTOR*)this->storDesc;
	if (devDesc->VendorIdOffset == 0)
		return 0;
	return enc.FromBytes(sbuff, &this->storDesc[devDesc->VendorIdOffset], -1, 0);
}

WChar *IO::StorageDevice::GetProductID(WChar *sbuff)
{
	STORAGE_DEVICE_DESCRIPTOR *devDesc;
	if (!this->GetStorDesc())
		return 0;
	Text::Encoding enc;
	devDesc = (STORAGE_DEVICE_DESCRIPTOR*)this->storDesc;
	if (devDesc->ProductIdOffset == 0)
		return 0;
	return enc.FromBytes(sbuff, &this->storDesc[devDesc->ProductIdOffset], -1, 0);
}

WChar *IO::StorageDevice::GetProductRevision(WChar *sbuff)
{
	STORAGE_DEVICE_DESCRIPTOR *devDesc;
	if (!this->GetStorDesc())
		return 0;
	Text::Encoding enc;
	devDesc = (STORAGE_DEVICE_DESCRIPTOR*)this->storDesc;
	if (devDesc->ProductRevisionOffset == 0)
		return 0;
	return enc.FromBytes(sbuff, &this->storDesc[devDesc->ProductRevisionOffset], -1, 0);
}

WChar *IO::StorageDevice::GetSerialNumber(WChar *sbuff)
{
	STORAGE_DEVICE_DESCRIPTOR *devDesc;
	if (!this->GetStorDesc())
		return 0;
	Text::Encoding enc;
	devDesc = (STORAGE_DEVICE_DESCRIPTOR*)this->storDesc;
	if (devDesc->SerialNumberOffset == 0)
		return 0;
	return enc.FromBytes(sbuff, &this->storDesc[devDesc->SerialNumberOffset], -1, 0);
}

Bool IO::StorageDevice::GetDiskGeometry(UInt64 *cylinder, UInt32 *trackPerCylinder, UInt32 *sectorPerTrack, UInt32 *bytesPerSector)
{
	DISK_GEOMETRY geom;
	BOOL status;
	DWORD returnedLength;

	status = DeviceIoControl((HANDLE)this->hand,  IOCTL_DISK_GET_DRIVE_GEOMETRY, 0, 0, &geom, sizeof(DISK_GEOMETRY), &returnedLength, NULL);
	if (status == 0)
		return false;

	*cylinder = geom.Cylinders.QuadPart;
	*trackPerCylinder = geom.TracksPerCylinder;
	*sectorPerTrack = geom.SectorsPerTrack;
	*bytesPerSector = geom.BytesPerSector;
	return true;
}

Bool IO::StorageDevice::SMARTGetVersion(UInt8 *ver, UInt8 *rev, Bool *supportATAID, Bool *supportATAPIID, Bool *supportSMART)
{
	GETVERSIONINPARAMS getVersionParams;
	BOOL status;
	DWORD returnedLength;
	MemClear(&getVersionParams, sizeof(GETVERSIONINPARAMS));

	status = DeviceIoControl((HANDLE)this->hand, SMART_GET_VERSION, 0, 0, &getVersionParams, sizeof(GETVERSIONINPARAMS), &returnedLength, NULL);
	if (status == 0)
		return false;
	*ver = getVersionParams.bVersion;
	*rev = getVersionParams.bRevision;
	*supportATAID = (getVersionParams.fCapabilities & CAP_ATA_ID_CMD) != 0;
	*supportATAPIID = (getVersionParams.fCapabilities & CAP_ATAPI_ID_CMD) != 0;
	*supportSMART = (getVersionParams.fCapabilities & CAP_SMART_CMD) != 0;
	return true;
}

Bool IO::StorageDevice::SMARTGetDiskID(UInt8 *idSector)
{
	const OSInt buffSize = sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE;
	UInt8 buff[buffSize];
	DWORD outSize = 0;
	SENDCMDINPARAMS params;
//#define ID_CMD          0xEC            // Returns ID sector for ATA
	params.irDriveRegs.bCommandReg = ID_CMD;
	if (!DeviceIoControl((HANDLE)this->hand, SMART_RCV_DRIVE_DATA, &params, sizeof(SENDCMDINPARAMS), buff, buffSize, &outSize, NULL))
		return false;

	SENDCMDOUTPARAMS *outParams = (SENDCMDOUTPARAMS*)buff;
	UInt8 *src  = outParams->bBuffer;
	UInt16 *dest = (UInt16*)idSector;
	OSInt i = 256;
	while (i-- > 0)
	{
		*dest = (*src << 8) | src[1];
		dest++;
		src += 2;
	}
	return true;
}

WChar *IO::StorageDevice::IDSectorGetSN(WChar *sbuff, UInt8 *idSector)
{
	Text::Encoding enc;
	enc.FromBytes(sbuff, &idSector[20], 20, 0);
	return Text::StrTrim(sbuff);
}
