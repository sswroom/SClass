#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "IO/SDCardInfo.h"
#include "Text/MyString.h"

IO::SDCardInfo::SDCardInfo(const UTF8Char *name, const UInt8 *cid, const UInt8 *csd)
{
	UInt8 csdType = csd[0] >> 6;
	this->isEMMC = (csdType >= 2);
	MemCopyNO(this->cid, cid, 16);
	MemCopyNO(this->csd, csd, 16);
	this->name = Text::StrCopyNew(name);
}

IO::SDCardInfo::~SDCardInfo()
{
	Text::StrDelNew(name);
}

OSInt IO::SDCardInfo::GetCID(UInt8 *cid)
{
	MemCopyNO(cid, this->cid, 16);
	return 16;
}

OSInt IO::SDCardInfo::GetCSD(UInt8 *csd)
{
	MemCopyNO(csd, this->csd, 16);
	return 16;
}

const UTF8Char *IO::SDCardInfo::GetName()
{
	return this->name;
}

UInt8 IO::SDCardInfo::GetManufacturerID()
{
	return this->cid[0];
}

UInt16 IO::SDCardInfo::GetOEMID()
{
	return ReadMInt16(&this->cid[1]);
}

Char *IO::SDCardInfo::GetProductName(Char *name)
{
	name[0] = this->cid[3];
	name[1] = this->cid[4];
	name[2] = this->cid[5];
	name[3] = this->cid[6];
	name[4] = this->cid[7];
	name[5] = 0;
	return &name[5];
}

UInt8 IO::SDCardInfo::GetProductRevision()
{
	if (this->isEMMC)
	{
		return this->cid[9];
	}
	else
	{
		return this->cid[8];
	}
}

UInt32 IO::SDCardInfo::GetSerialNo()
{
	if (this->isEMMC)
	{
		return ReadMInt32(&this->cid[10]);
	}
	else
	{
		return ReadMInt32(&this->cid[9]);
	}
}

UInt32 IO::SDCardInfo::GetManufacturingYear()
{
	if (this->isEMMC)
	{
		UInt32 year = 1997 + (this->cid[14] & 15);
		UInt8 month = this->cid[14] >> 4;
		Data::DateTime dt;
		dt.SetCurrTime();
		while (true)
		{
			if (year + 16 > dt.GetYear())
			{
				break;
			}
			if (year + 16 == dt.GetYear() && month >= dt.GetMonth())
				break;
			year += 16;
		}
		return year;
	}
	else
	{
		return 2000 + (((this->cid[13] & 15) << 4) | (this->cid[14] >> 4));
	}
}

UInt8 IO::SDCardInfo::GetManufacturingMonth()
{
	if (this->isEMMC)
	{
		return this->cid[14] >> 4;
	}
	else
	{
		return this->cid[14] & 15;
	}
}

UInt8 IO::SDCardInfo::GetCSDVersion()
{
	if ((this->csd[0] & 0xc0) == 0)
		return 1;
	else if ((this->csd[0] & 0xc0) == 0x40)
		return 2;
	else
		return 0;
}

UInt8 IO::SDCardInfo::GetTAAC()
{
	return this->csd[1];
}

UInt8 IO::SDCardInfo::GetNSAC()
{
	return this->csd[2];
}

Int64 IO::SDCardInfo::GetMaxTranRate()
{
	UInt8 tran_speed = this->csd[3];
	Int64 unit;
	Int64 val;
	switch (tran_speed & 7)
	{
	case 0:
		unit = 10000;
		break;
	case 1:
		unit = 100000;
		break;
	case 2:
		unit = 1000000;
		break;
	case 3:
		unit = 10000000;
		break;
	default:
		unit = 0;
		break;
	}
	switch ((tran_speed >> 3) & 15)
	{
	case 1:
		val = 10;
		break;
	case 2:
		val = 12;
		break;
	case 3:
		val = 13;
		break;
	case 4:
		val = 15;
		break;
	case 5:
		val = 20;
		break;
	case 6:
		val = 25;
		break;
	case 7:
		val = 30;
		break;
	case 8:
		val = 35;
		break;
	case 9:
		val = 40;
		break;
	case 10:
		val = 45;
		break;
	case 11:
		val = 50;
		break;
	case 12:
		val = 55;
		break;
	case 13:
		val = 60;
		break;
	case 14:
		val = 70;
		break;
	case 15:
		val = 80;
		break;
	default:
		val = 0;
		break;
	}
	return val * unit;
}

UInt16 IO::SDCardInfo::GetCardCmdClass()
{
	return ReadMInt16(&this->csd[4]) >> 4;
}

UInt8 IO::SDCardInfo::GetMaxReadBlkLen()
{
	return this->csd[5] & 15;
}

Bool IO::SDCardInfo::GetBlockReadPartial()
{
	return (this->csd[6] & 0x80) != 0;
}

Bool IO::SDCardInfo::GetWriteBlockMisalign()
{
	return (this->csd[6] & 0x40) != 0;
}

Bool IO::SDCardInfo::GetReadBlockMisalign()
{
	return (this->csd[6] & 0x20) != 0;
}

Bool IO::SDCardInfo::GetDSRImplemented()
{
	return (this->csd[6] & 0x10) != 0;
}

UInt32 IO::SDCardInfo::GetDeviceSize()
{
	return ((this->csd[6] & 3) << 10) | (this->csd[7] << 2) | (this->csd[8] >> 6);
}

UInt8 IO::SDCardInfo::GetMaxVDDReadCurr()
{
	return (this->csd[8] >> 3) & 0x7;
}

UInt8 IO::SDCardInfo::GetMinVDDReadCurr()
{
	return this->csd[8] & 0x7;
}

UInt8 IO::SDCardInfo::GetMaxVDDWriteCurr()
{
	return (this->csd[9] >> 5) & 0x7;
}

UInt8 IO::SDCardInfo::GetMinVDDWriteCurr()
{
	return (this->csd[9] >> 2) & 0x7;
}

UInt8 IO::SDCardInfo::GetDeviceSizeMulitply()
{
	return ((this->csd[9] & 3) << 1) | (this->csd[10] >> 7);
}

Bool IO::SDCardInfo::GetEraseSingleBlockEnabled()
{
	return (this->csd[10] & 0x40) != 0;
}

UInt8 IO::SDCardInfo::GetEraseSectorSize()
{
	return ((this->csd[10] & 0x3f) << 1) | (this->csd[11] >> 7);
}

UInt8 IO::SDCardInfo::GetWriteProtectGroupSize()
{
	return this->csd[11] & 0x7f;
}

Bool IO::SDCardInfo::GetWriteProtectGroupEnabled()
{
	return (this->csd[12] & 0x80) != 0;
}

UInt8 IO::SDCardInfo::GetWriteSpeedFactor()
{
	return (this->csd[12] >> 2) & 7;
}

UInt8 IO::SDCardInfo::GetMaxWriteBlockLen()
{
	return ((this->csd[12] & 3) << 2) | (this->csd[13] >> 6);
}

Bool IO::SDCardInfo::GetBlockWritePartial()
{
	return (this->csd[13] & 0x20) != 0;
}

Bool IO::SDCardInfo::GetFileFormatGroup()
{
	return (this->csd[14] & 0x80) != 0;
}

Bool IO::SDCardInfo::GetCopyFlag()
{
	return (this->csd[14] & 0x40) != 0;
}

Bool IO::SDCardInfo::GetPermWriteProtect()
{
	return (this->csd[14] & 0x20) != 0;
}

Bool IO::SDCardInfo::GetTmpWriteProtect()
{
	return (this->csd[14] & 0x10) != 0;
}

UInt8 IO::SDCardInfo::GetFileFormat()
{
	return (this->csd[14] >> 2) & 3;
}

Int64 IO::SDCardInfo::GetCardCapacity()
{
	if ((this->csd[0] & 0xc0) == 0)
	{
		UInt32 c_size_multi = ((this->csd[9] & 3) << 1) | (this->csd[10] >> 7);
		UInt32 read_bl_len = this->csd[5] & 15;
		if (read_bl_len < 12 && c_size_multi < 8)
		{
			UInt32 c_size = ((this->csd[6] & 3) << 10) | (this->csd[7] << 2) | (this->csd[8] >> 6);
			UInt32 block_len = 1 << read_bl_len;
			UInt32 blocknr = (c_size + 1) * (4 << c_size_multi);
			return blocknr * (Int64)block_len;
		}
		else
		{
			return 0;
		}
	}
	else if ((this->csd[0] & 0xc0) == 0x40)
	{
		UInt32 c_size = ((UInt32)(this->csd[7] & 0x3f) << 16) | (this->csd[8] << 8) | this->csd[9];
		return (c_size + 1) * (Int64)524288;
	}
	else
	{
		return 0;
	}
}

const UTF8Char *IO::SDCardInfo::GetManufacturerName(UInt8 mid)
{
	switch (mid)
	{
	case 1:
		return (const UTF8Char*)"Panasonic";
	case 2:
		return (const UTF8Char*)"Toshiba";
	case 3:
		return (const UTF8Char*)"Sandisk";
//	case 0x1a:
//		return (const UTF8Char*)"?PQI";
	case 0x1b:
		return (const UTF8Char*)"Samsung";
//	case 0x1c:
//		return (const UTF8Char*)"?Transcend";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
