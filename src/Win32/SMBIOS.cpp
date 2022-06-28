#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Win32/SMBIOS.h"

void Win32::SMBIOS::GetDataType(Data::ArrayList<const UInt8 *> *dataList, UInt8 dataType) const
{
	UOSInt i = 0;
	UOSInt j = this->smbiosBuffSize;
	const UInt8 *buff = this->smbiosBuff;
	while (i < j)
	{
		if (dataType == buff[i])
		{
			dataList->Add(&buff[i]);
		}
		i += buff[i + 1];
		while (i < j && (buff[i] != 0 || buff[i + 1] != 0))
		{
			i++;
		}
		i += 2;
	}
}

Win32::SMBIOS::SMBIOS(const UInt8 *smbiosBuff, UOSInt smbiosBuffSize, UInt8 *relPtr)
{
	this->smbiosBuff = smbiosBuff;
	this->smbiosBuffSize = smbiosBuffSize;
	this->relPtr = relPtr;
}

Win32::SMBIOS::~SMBIOS()
{
	if (this->relPtr)
	{
		MemFree(this->relPtr);
	}
}

UOSInt Win32::SMBIOS::GetMemoryInfo(Data::ArrayList<MemoryDeviceInfo*> *memList) const
{
	Data::ArrayList<const UInt8 *> dataList;
	const UInt8 *dataBuff;
	const Char *carr[8];
	MemoryDeviceInfo *mem;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt ret = 0;
	this->GetDataType(&dataList, 17);
	i = 0;
	j = dataList.GetCount();
	while (i < j)
	{
		dataBuff = dataList.GetItem(i);
		k = 8;
		while (k-- > 0)
		{
			carr[k] = 0;
		}
		l = dataBuff[1];
		if (dataBuff[l] != 0)
		{
			k = 2;
			carr[1] = (const Char*)&dataBuff[l];
			while (true)
			{
				if (dataBuff[l] == 0)
				{
					if (dataBuff[l + 1] == 0)
						break;
					carr[k] = (const Char*)&dataBuff[l + 1];
					k++;
				}
				l++;
			}
		}

		mem = MemAlloc(MemoryDeviceInfo, 1);
		MemClear(mem, sizeof(MemoryDeviceInfo));
		if (dataBuff[1] >= 6)
			mem->memArrayHandle = ReadUInt16(&dataBuff[4]);
		if (dataBuff[1] >= 8)
			mem->memErrHandle = ReadUInt16(&dataBuff[6]);
		if (dataBuff[1] >= 10)
			mem->totalWidthBits = ReadUInt16(&dataBuff[8]);
		if (dataBuff[1] >= 12)
			mem->dataWidthBits = ReadUInt16(&dataBuff[10]);
		if (dataBuff[1] >= 14)
		{
			UInt16 sizeVal = ReadUInt16(&dataBuff[12]);
			if (sizeVal == 0x7fff)
			{
				mem->memorySize = (ReadUInt32(&dataBuff[28]) & 0x7fffffff) * 1048576ULL;
			}
			else if (sizeVal & 0x8000)
			{
				mem->memorySize = (sizeVal & 0x7fff) * 1024ULL;
			}
			else
			{
				mem->memorySize = sizeVal * 1048576ULL;
			}
		}
		if (dataBuff[1] >= 15)
			mem->formFactor = (MemoryFormFactor)dataBuff[14];
		if (dataBuff[1] >= 16)
			mem->deviceSet = dataBuff[15];
		if (dataBuff[1] >= 17)
			mem->deviceLocator = carr[dataBuff[16]];
		if (dataBuff[1] >= 18)
			mem->bankLocator = carr[dataBuff[17]];
		if (dataBuff[1] >= 19)
			mem->memType = (MemoryType)dataBuff[18];
		if (dataBuff[1] >= 23)
			mem->maxSpeedMTs = ReadUInt16(&dataBuff[21]);
		if (dataBuff[1] >= 24)
			mem->manufacturer = carr[dataBuff[23]];
		if (dataBuff[1] >= 25)
			mem->sn = carr[dataBuff[24]];
		if (dataBuff[1] >= 26)
			mem->assetTag = carr[dataBuff[25]];
		if (dataBuff[1] >= 27)
			mem->partNo = carr[dataBuff[26]];
		if (dataBuff[1] >= 28)
			mem->attributes = dataBuff[27];
		if (dataBuff[1] >= 34)
			mem->confSpeedMTs = ReadUInt16(&dataBuff[32]);
		if (dataBuff[1] >= 36)
			mem->minVolt = ReadUInt16(&dataBuff[34]) * 0.001;
		if (dataBuff[1] >= 38)
			mem->maxVolt = ReadUInt16(&dataBuff[36]) * 0.001;
		if (dataBuff[1] >= 40)
			mem->confVolt = ReadUInt16(&dataBuff[38]) * 0.001;
		memList->Add(mem);
		ret++;
		i++;
	}
	return ret;
}

void Win32::SMBIOS::FreeMemoryInfo(Data::ArrayList<MemoryDeviceInfo*> *memList) const
{
	UOSInt i = memList->GetCount();
	while (i-- > 0)
	{
		MemFree(memList->GetItem(i));
	}
	memList->Clear();
}

UTF8Char *Win32::SMBIOS::GetPlatformName(UTF8Char *buff) const
{
	Data::ArrayList<const UInt8 *> dataList;
	const UInt8 *dataBuff;
	const Char *carr[32];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UTF8Char *ret = 0;
	this->GetDataType(&dataList, 2);
	i = 0;
	j = dataList.GetCount();
	while (i < j)
	{
		dataBuff = dataList.GetItem(i);
		k = 8;
		while (k-- > 0)
		{
			carr[k] = 0;
		}
		l = dataBuff[1];
		if (dataBuff[l] != 0)
		{
			k = 2;
			carr[1] = (const Char*)&dataBuff[l];
			while (true)
			{
				if (dataBuff[l] == 0)
				{
					if (dataBuff[l + 1] == 0)
						break;
					carr[k] = (const Char*)&dataBuff[l + 1];
					k++;
				}
				l++;
			}
		}

		const Char *manufacturer = carr[dataBuff[4]];
		const Char *product = carr[dataBuff[5]];
		if (manufacturer && product)
		{
			ret = Text::StrConcat(buff, (const UTF8Char*)manufacturer);
			*ret++ = ' ';
			ret = Text::StrConcat(ret, (const UTF8Char*)product);
			break;
		}
		i++;
	}
	return ret;
}

UTF8Char *Win32::SMBIOS::GetPlatformSN(UTF8Char *buff) const
{
	Data::ArrayList<const UInt8 *> dataList;
	const UInt8 *dataBuff;
	const Char *carr[32];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UTF8Char *ret = 0;
	this->GetDataType(&dataList, 2);
	i = 0;
	j = dataList.GetCount();
	while (i < j)
	{
		dataBuff = dataList.GetItem(i);
		k = 8;
		while (k-- > 0)
		{
			carr[k] = 0;
		}
		l = dataBuff[1];
		if (dataBuff[l] != 0)
		{
			k = 2;
			carr[1] = (const Char*)&dataBuff[l];
			while (true)
			{
				if (dataBuff[l] == 0)
				{
					if (dataBuff[l + 1] == 0)
						break;
					carr[k] = (const Char*)&dataBuff[l + 1];
					k++;
				}
				l++;
			}
		}

		const Char *sn = carr[dataBuff[7]];
		if (sn)
		{
			ret = Text::StrConcat(buff, (const UTF8Char*)sn);
			ret = Text::StrTrim(buff);
			if (ret == buff)
			{
				ret = 0;
			}
			else if (Text::StrEndsWith(buff, (const UTF8Char*)"XXXX"))
			{
				ret = 0;
			}
			else if (Text::StrEquals(buff, (const UTF8Char*)"Default string"))
			{
				ret = 0;
			}
			break;
		}
		i++;
	}
	if (ret == 0)
	{
		this->GetDataType(&dataList, 1);
		i = 0;
		j = dataList.GetCount();
		while (i < j)
		{
			dataBuff = dataList.GetItem(i);
			k = 8;
			while (k-- > 0)
			{
				carr[k] = 0;
			}
			l = dataBuff[1];
			if (dataBuff[l] != 0)
			{
				k = 2;
				carr[1] = (const Char*)&dataBuff[l];
				while (true)
				{
					if (dataBuff[l] == 0)
					{
						if (dataBuff[l + 1] == 0)
							break;
						carr[k] = (const Char*)&dataBuff[l + 1];
						k++;
					}
					l++;
				}
			}

			const Char *sn = carr[dataBuff[7]];
			if (sn)
			{
				ret = Text::StrConcat(buff, (const UTF8Char*)sn);
				ret = Text::StrTrim(buff);
				if (ret == buff)
				{
					ret = 0;
				}
				else if (Text::StrEndsWith(buff, (const UTF8Char*)"XXXX"))
				{
					ret = 0;
				}
				else if (Text::StrEquals(buff, (const UTF8Char*)"Default string"))
				{
					ret = 0;
				}
				break;
			}
			i++;
		}
	}
	return ret;
}

Int32 Win32::SMBIOS::GetChassisType() const
{
	Data::ArrayList<const UInt8 *> dataList;
	const UInt8 *dataBuff;
	UOSInt i;
	UOSInt j;
	Int32 ret = 0;
	this->GetDataType(&dataList, 3);
	i = 0;
	j = dataList.GetCount();
	while (i < j)
	{
		dataBuff = dataList.GetItem(i);

		ret = dataBuff[5];
		break;
	}
	return ret;
}

Bool Win32::SMBIOS::ToString(Text::StringBuilderUTF8 *sb) const
{
	Text::CString carr[32];
	const UInt8 *dataBuff;
	UOSInt i = 0;
	UOSInt j = this->smbiosBuffSize;
	UOSInt k;
	UOSInt l;
	const UInt8 *buff = this->smbiosBuff;
	while (i < j)
	{
		k = 32;
		while (k-- > 0)
		{
			carr[k] = CSTR_NULL;
		}
		dataBuff = &buff[i];
		l = dataBuff[1];
		k = 1;
		if (dataBuff[l] != 0)
		{
			k = 2;
			carr[1].v = &dataBuff[l];
			while (true)
			{
				if (dataBuff[l] == 0)
				{
					carr[k - 1].leng = (UOSInt)(&dataBuff[l] - carr[k - 1].v);
					if (dataBuff[l + 1] == 0)
						break;
					carr[k].v = &dataBuff[l + 1];
					k++;
				}
				l++;
			}
		}

		switch (dataBuff[0]) //smbios type
		{
		case 0:
			sb->AppendC(UTF8STRC("SMBIOS Type 0 - BIOS Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Vendor: "));
			if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("BIOS Version: "));
			if (carr[dataBuff[5]].leng > 0) sb->Append(carr[dataBuff[5]]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("BIOS Starting Address Segment: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[6]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("BIOS Release Date: "));
			if (carr[dataBuff[8]].leng > 0) sb->Append(carr[dataBuff[8]]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("BIOS ROM Size: "));
			if (dataBuff[9] == 255)
			{
				sb->AppendC(UTF8STRC("16MB or greater"));
			}
			else
			{
				sb->AppendU32(65536 * (UInt32)(dataBuff[9] + 1));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("BIOS Characteristics: 0x"));
			sb->AppendHex64(ReadUInt64(&dataBuff[10]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 19)
			{
				sb->AppendC(UTF8STRC("BIOS Characteristics Extension Byte 1: 0x"));
				sb->AppendHex8(dataBuff[18]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 20)
			{
				sb->AppendC(UTF8STRC("BIOS Characteristics Extension Byte 2: 0x"));
				sb->AppendHex8(dataBuff[19]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 22)
			{
				sb->AppendC(UTF8STRC("System BIOS Revision: "));
				sb->AppendU16(dataBuff[20]);
				sb->AppendC(UTF8STRC("."));
				sb->AppendU16(dataBuff[21]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 24)
			{
				sb->AppendC(UTF8STRC("Embedded Controller Firmware: "));
				sb->AppendU16(dataBuff[22]);
				sb->AppendC(UTF8STRC("."));
				sb->AppendU16(dataBuff[23]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 26)
			{
				sb->AppendC(UTF8STRC("Extended BIOS ROM Size: "));
				sb->AppendI16(ReadInt16(&dataBuff[24]) & 0x3fff);
				if ((dataBuff[25] & 0xc0) == 0)
				{
					sb->AppendC(UTF8STRC("MB"));
				}
				else if ((dataBuff[25] & 0xc0) == 0x40)
				{
					sb->AppendC(UTF8STRC("GB"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 1:
			sb->AppendC(UTF8STRC("SMBIOS Type 1 - System Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Manufacturer: "));
			if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Product Name: "));
			if (carr[dataBuff[5]].leng > 0) sb->Append(carr[dataBuff[5]]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Version: "));
			if (carr[dataBuff[6]].leng > 0) sb->Append(carr[dataBuff[6]]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Serial Number: "));
			if (carr[dataBuff[7]].leng > 0) sb->Append(carr[dataBuff[7]]);
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 25)
			{
				sb->AppendC(UTF8STRC("UUID: {"));
				sb->AppendHex32(ReadUInt32(&dataBuff[8]));
				sb->AppendC(UTF8STRC("-"));
				sb->AppendHex16(ReadUInt16(&dataBuff[12]));
				sb->AppendC(UTF8STRC("-"));
				sb->AppendHex16(ReadUInt16(&dataBuff[14]));
				sb->AppendC(UTF8STRC("-"));
				sb->AppendHex16(ReadMUInt16(&dataBuff[16]));
				sb->AppendC(UTF8STRC("-"));
				sb->AppendHex32(ReadMUInt32(&dataBuff[18]));
				sb->AppendHex16(ReadMUInt16(&dataBuff[22]));
				sb->AppendC(UTF8STRC("}\r\n"));
				sb->AppendC(UTF8STRC("Wake-up Type: "));
				switch (dataBuff[24])
				{
				case 0:
					sb->AppendC(UTF8STRC("Reserved"));
					break;
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("APM Timer"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Modem Ring"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("LAN Remote"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Power Switch"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("PCI PME#"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("AC Power Restored"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[24]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 27)
			{
				sb->AppendC(UTF8STRC("SKU Number: "));
				if (carr[dataBuff[25]].leng > 0) sb->Append(carr[dataBuff[25]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Family: "));
				if (carr[dataBuff[26]].leng > 0) sb->Append(carr[dataBuff[26]]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 2:
			sb->AppendC(UTF8STRC("SMBIOS Type 2 - Baseboard Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 8)
			{
				sb->AppendC(UTF8STRC("Manufacturer: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Product: "));
				if (carr[dataBuff[5]].leng > 0) sb->Append(carr[dataBuff[5]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Version: "));
				if (carr[dataBuff[6]].leng > 0) sb->Append(carr[dataBuff[6]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Serial Number: "));
				if (carr[dataBuff[7]].leng > 0) sb->Append(carr[dataBuff[7]]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 9)
			{
				sb->AppendC(UTF8STRC("Asset Tag: "));
				if (carr[dataBuff[8]].leng > 0) sb->Append(carr[dataBuff[8]]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 10)
			{
				sb->AppendC(UTF8STRC("Feature Flags: 0x"));
				sb->AppendHex8(dataBuff[9]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 11)
			{
				sb->AppendC(UTF8STRC("Location in Chassis: "));
				if (carr[dataBuff[10]].leng > 0) sb->Append(carr[dataBuff[10]]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 13)
			{
				sb->AppendC(UTF8STRC("Chassis Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[11]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 14)
			{
				sb->AppendC(UTF8STRC("Board Type: "));
				switch (dataBuff[13])
				{
				case 1:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Server Blade"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Connectivity Switch"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("System Management Module"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Processor Module"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("I/O Module"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("Memory Module"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("Daughter board"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("Motherboard"));
					break;
				case 11:
					sb->AppendC(UTF8STRC("Processor/Memory Module"));
					break;
				case 12:
					sb->AppendC(UTF8STRC("Processor/IO Module"));
					break;
				case 13:
					sb->AppendC(UTF8STRC("Interconnect board"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[13]);
					sb->AppendC(UTF8STRC(")"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 3:
			sb->AppendC(UTF8STRC("SMBIOS Type 3 - System Enclosure\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 9)
			{
				sb->AppendC(UTF8STRC("Manufacturer: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Chassis Lock Present: "));
				if (dataBuff[5] & 0x80)
				{
					sb->AppendC(UTF8STRC("Yes"));
				}
				else
				{
					sb->AppendC(UTF8STRC("No"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Type: "));
				switch (dataBuff[5] & 0x7f)
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknwon"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Desktop"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Low Profile Desktop"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Pizza Box"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Mini Tower"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("Tower"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("Portable"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("Laptop"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("Notebook"));
					break;
				case 11:
					sb->AppendC(UTF8STRC("Hand Held"));
					break;
				case 12:
					sb->AppendC(UTF8STRC("Docking Station"));
					break;
				case 13:
					sb->AppendC(UTF8STRC("All in One"));
					break;
				case 14:
					sb->AppendC(UTF8STRC("Sub Notebook"));
					break;
				case 15:
					sb->AppendC(UTF8STRC("Space-saving"));
					break;
				case 16:
					sb->AppendC(UTF8STRC("Lunch Box"));
					break;
				case 17:
					sb->AppendC(UTF8STRC("Main Server Chassis"));
					break;
				case 18:
					sb->AppendC(UTF8STRC("Expansion Chassis"));
					break;
				case 19:
					sb->AppendC(UTF8STRC("SubChassis"));
					break;
				case 20:
					sb->AppendC(UTF8STRC("Bus Expansion Chassis"));
					break;
				case 21:
					sb->AppendC(UTF8STRC("Peripheral Chassis"));
					break;
				case 22:
					sb->AppendC(UTF8STRC("RAID Chassis"));
					break;
				case 23:
					sb->AppendC(UTF8STRC("Rack Mount Chassis"));
					break;
				case 24:
					sb->AppendC(UTF8STRC("Sealed-case PC"));
					break;
				case 25:
					sb->AppendC(UTF8STRC("Multi-system chassis"));
					break;
				case 26:
					sb->AppendC(UTF8STRC("Compact PCI"));
					break;
				case 27:
					sb->AppendC(UTF8STRC("Advanced TCA"));
					break;
				case 28:
					sb->AppendC(UTF8STRC("Blade"));
					break;
				case 29:
					sb->AppendC(UTF8STRC("Blade Enclosure"));
					break;
				case 30:
					sb->AppendC(UTF8STRC("Tablet"));
					break;
				case 31:
					sb->AppendC(UTF8STRC("Convertible"));
					break;
				case 32:
					sb->AppendC(UTF8STRC("Detachable"));
					break;
				case 33:
					sb->AppendC(UTF8STRC("IoT Gateway"));
					break;
				case 34:
					sb->AppendC(UTF8STRC("Embedded PC"));
					break;
				case 35:
					sb->AppendC(UTF8STRC("Mini PC"));
					break;
				case 36:
					sb->AppendC(UTF8STRC("Stick PC"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[5] & 0x7f);
					sb->AppendC(UTF8STRC(")"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Version: "));
				if (carr[dataBuff[6]].leng > 0) sb->Append(carr[dataBuff[6]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Serial Number: "));
				if (carr[dataBuff[7]].leng > 0) sb->Append(carr[dataBuff[7]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Asset Tag Number: "));
				if (carr[dataBuff[8]].leng > 0) sb->Append(carr[dataBuff[8]]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 9)
			{
				sb->AppendC(UTF8STRC("Boot-up State: "));
				switch (dataBuff[9])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknwon"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Safe"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Warning"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Critical"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Non-recoverable"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[9]);
					sb->AppendC(UTF8STRC(")"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Power Supply State: "));
				switch (dataBuff[10])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknwon"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Safe"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Warning"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Critical"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Non-recoverable"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[10]);
					sb->AppendC(UTF8STRC(")"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Thermal State: "));
				switch (dataBuff[11])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknwon"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Safe"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Warning"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Critical"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Non-recoverable"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[11]);
					sb->AppendC(UTF8STRC(")"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Security Status: "));
				switch (dataBuff[12])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknwon"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("None"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("External interface locked out"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("External interface enabled"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[12]);
					sb->AppendC(UTF8STRC(")"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 17)
			{
				sb->AppendC(UTF8STRC("OEM-defined: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[13]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 18)
			{
				sb->AppendC(UTF8STRC("Height: "));
				sb->AppendU16(dataBuff[17]);
				sb->AppendC(UTF8STRC("U\r\n"));
			}
			if (dataBuff[1] >= 19)
			{
				sb->AppendC(UTF8STRC("Number of Power Cords: "));
				sb->AppendU16(dataBuff[18]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 21)
			{
				Int32 n = dataBuff[19];
				Int32 m = dataBuff[20];
				sb->AppendC(UTF8STRC("Contained Element Count: "));
				sb->AppendU16(dataBuff[19]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Contained Element Record Length: "));
				sb->AppendU16(dataBuff[20]);
				sb->AppendC(UTF8STRC("\r\n"));
				if (dataBuff[1] >= 22 + (n * m))
				{
					sb->AppendC(UTF8STRC("SKU Number: "));
					if (carr[dataBuff[21 + (n * m)]].leng > 0) sb->Append(carr[dataBuff[21 + (n * m)]]);
					sb->AppendC(UTF8STRC("\r\n"));
				}
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 4:
			sb->AppendC(UTF8STRC("SMBIOS Type 4 - Processor Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 26)
			{
				sb->AppendC(UTF8STRC("Socket Designation: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Processor Type: "));
				switch (dataBuff[5])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknwon"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Central Processor"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Math Processor"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("DSP Processor"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Video Processor"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[5]);
					sb->AppendC(UTF8STRC(")"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Processor Family: "));
				GetProcessorFamily(sb, dataBuff[6]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Processor Manufacturer: "));
				if (carr[dataBuff[7]].leng > 0) sb->Append(carr[dataBuff[7]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Processor ID: 0x"));
				sb->AppendHex64(ReadUInt64(&dataBuff[8]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Processor Version: "));
				if (carr[dataBuff[16]].leng > 0) sb->Append(carr[dataBuff[16]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Voltage: "));
				if (dataBuff[17] & 0x80)
				{
					Text::SBAppendF64(sb, (dataBuff[17] & 0x7f) * 0.1);
					sb->AppendC(UTF8STRC("V"));
				}
				else
				{
					if (dataBuff[17] & 1)
					{
						sb->AppendC(UTF8STRC(" 5V"));
					}
					if (dataBuff[17] & 2)
					{
						sb->AppendC(UTF8STRC(" 3.3V"));
					}
					if (dataBuff[17] & 4)
					{
						sb->AppendC(UTF8STRC(" 2.9V"));
					}
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("External Clock: "));
				sb->AppendI16(ReadInt16(&dataBuff[18]));
				sb->AppendC(UTF8STRC("MHz\r\n"));
				sb->AppendC(UTF8STRC("Max Speed: "));
				sb->AppendI16(ReadInt16(&dataBuff[20]));
				sb->AppendC(UTF8STRC("MHz\r\n"));
				sb->AppendC(UTF8STRC("Current Speed: "));
				sb->AppendI16(ReadInt16(&dataBuff[22]));
				sb->AppendC(UTF8STRC("MHz\r\n"));
				sb->AppendC(UTF8STRC("Status: 0x"));
				sb->AppendHex8(dataBuff[24]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Processor Upgrade: "));
				switch (dataBuff[25])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknwon"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Daughter Board"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("ZIF Socket"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Replaceable Piggy Back"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("None"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("LIF Socket"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("Slot 1"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("Slot 2"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("370-pin socket"));
					break;
				case 11:
					sb->AppendC(UTF8STRC("Slot A"));
					break;
				case 12:
					sb->AppendC(UTF8STRC("Slot M"));
					break;
				case 13:
					sb->AppendC(UTF8STRC("Socket 423"));
					break;
				case 14:
					sb->AppendC(UTF8STRC("Socket A (Socket 462)"));
					break;
				case 15:
					sb->AppendC(UTF8STRC("Socket 478"));
					break;
				case 16:
					sb->AppendC(UTF8STRC("Socket 754"));
					break;
				case 17:
					sb->AppendC(UTF8STRC("Socket 940"));
					break;
				case 18:
					sb->AppendC(UTF8STRC("Socket 939"));
					break;
				case 19:
					sb->AppendC(UTF8STRC("Socket mPGA604"));
					break;
				case 20:
					sb->AppendC(UTF8STRC("Socket LGA771"));
					break;
				case 21:
					sb->AppendC(UTF8STRC("Socket LGA775"));
					break;
				case 22:
					sb->AppendC(UTF8STRC("Socket S1"));
					break;
				case 23:
					sb->AppendC(UTF8STRC("Socket AM2"));
					break;
				case 24:
					sb->AppendC(UTF8STRC("Socket F (1207)"));
					break;
				case 25:
					sb->AppendC(UTF8STRC("Socket LGA1366"));
					break;
				case 26:
					sb->AppendC(UTF8STRC("Socket G34"));
					break;
				case 27:
					sb->AppendC(UTF8STRC("Socket AM3"));
					break;
				case 28:
					sb->AppendC(UTF8STRC("Socket C32"));
					break;
				case 29:
					sb->AppendC(UTF8STRC("Socket LGA1156"));
					break;
				case 30:
					sb->AppendC(UTF8STRC("Socket LGA1567"));
					break;
				case 31:
					sb->AppendC(UTF8STRC("Socket PGA988A"));
					break;
				case 32:
					sb->AppendC(UTF8STRC("Socket BGA1288"));
					break;
				case 33:
					sb->AppendC(UTF8STRC("Socket rPGA988B"));
					break;
				case 34:
					sb->AppendC(UTF8STRC("Socket BGA1023"));
					break;
				case 35:
					sb->AppendC(UTF8STRC("Socket BGA1224"));
					break;
				case 36:
					sb->AppendC(UTF8STRC("Socket LGA1155"));
					break;
				case 37:
					sb->AppendC(UTF8STRC("Socket LGA1356"));
					break;
				case 38:
					sb->AppendC(UTF8STRC("Socket LGA2011"));
					break;
				case 39:
					sb->AppendC(UTF8STRC("Socket FS1"));
					break;
				case 40:
					sb->AppendC(UTF8STRC("Socket FS2"));
					break;
				case 41:
					sb->AppendC(UTF8STRC("Socket FM1"));
					break;
				case 42:
					sb->AppendC(UTF8STRC("Socket FM2"));
					break;
				case 43:
					sb->AppendC(UTF8STRC("Socket LGA2011-3"));
					break;
				case 44:
					sb->AppendC(UTF8STRC("Socket LGA1356-3"));
					break;
				case 45:
					sb->AppendC(UTF8STRC("Socket LGA1150"));
					break;
				case 46:
					sb->AppendC(UTF8STRC("Socket BGA1168"));
					break;
				case 47:
					sb->AppendC(UTF8STRC("Socket BGA1234"));
					break;
				case 48:
					sb->AppendC(UTF8STRC("Socket BGA1364"));
					break;
				case 49:
					sb->AppendC(UTF8STRC("Socket AM4"));
					break;
				case 50:
					sb->AppendC(UTF8STRC("Socket LGA1151"));
					break;
				case 51:
					sb->AppendC(UTF8STRC("Socket BGA1356"));
					break;
				case 52:
					sb->AppendC(UTF8STRC("Socket BGA1440"));
					break;
				case 53:
					sb->AppendC(UTF8STRC("Socket BGA1515"));
					break;
				case 54:
					sb->AppendC(UTF8STRC("Socket LGA3647-1"));
					break;
				case 55:
					sb->AppendC(UTF8STRC("Socket SP3"));
					break;
				case 56:
					sb->AppendC(UTF8STRC("Socket SP3r2"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[25]);
					sb->AppendC(UTF8STRC(")"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 32)
			{
				sb->AppendC(UTF8STRC("L1 Cache Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[26]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("L2 Cache Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[28]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("L3 Cache Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[30]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 35)
			{
				sb->AppendC(UTF8STRC("Serial Number: "));
				if (carr[dataBuff[32]].leng > 0) sb->Append(carr[dataBuff[32]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Asset Tag: "));
				if (carr[dataBuff[33]].leng > 0) sb->Append(carr[dataBuff[33]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Part Number: "));
				if (carr[dataBuff[34]].leng > 0) sb->Append(carr[dataBuff[34]]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 40)
			{
				sb->AppendC(UTF8STRC("Core Count: "));
				sb->AppendU16(dataBuff[35]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Core Enabled: "));
				sb->AppendU16(dataBuff[36]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Thread Count: "));
				sb->AppendU16(dataBuff[37]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Processor Characteristics: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[38]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 48)
			{
				sb->AppendC(UTF8STRC("Processor Family 2: "));
				GetProcessorFamily(sb, ReadInt16(&dataBuff[40]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Core Count 2: "));
				sb->AppendI16(ReadInt16(&dataBuff[42]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Core Enabled 2: "));
				sb->AppendI16(ReadInt16(&dataBuff[44]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Thread Count 2: "));
				sb->AppendI16(ReadInt16(&dataBuff[46]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
/*		case 5:
			sb->AppendC(UTF8STRC("SMBIOS Type 5 - Memory Controller Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 15)
			{
				////////////////////////////////
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;*/
		case 6:
			sb->AppendC(UTF8STRC("SMBIOS Type 6 - Memory Module Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 12)
			{
				sb->AppendC(UTF8STRC("Socket Designation: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Bank Connections: "));
				sb->AppendU16(dataBuff[5]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Current Speed: "));
				sb->AppendU16(dataBuff[6]);
				sb->AppendC(UTF8STRC("ns\r\n"));
				sb->AppendC(UTF8STRC("Current Memory Type: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[7]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Installed Size: "));
				sb->AppendU16(dataBuff[9]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Enabled Size: "));
				sb->AppendU16(dataBuff[10]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Error Status: 0x"));
				sb->AppendHex8(dataBuff[11]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 7:
			sb->AppendC(UTF8STRC("SMBIOS Type 7 - Cache Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 15)
			{
				sb->AppendC(UTF8STRC("Socket Designation: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Cache Configuration: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[5]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Maximum Cache Size: "));
				if (dataBuff[8] & 0x80)
				{
					sb->AppendI32((ReadInt16(&dataBuff[7]) & 0x7fff) * 64);
					sb->AppendC(UTF8STRC("K"));
				}
				else
				{
					sb->AppendI32(ReadInt16(&dataBuff[7]) & 0x7fff);
					sb->AppendC(UTF8STRC("K"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Installed Size: "));
				if (dataBuff[10] & 0x80)
				{
					sb->AppendI32((ReadInt16(&dataBuff[9]) & 0x7fff) * 64);
					sb->AppendC(UTF8STRC("K"));
				}
				else
				{
					sb->AppendI32(ReadInt16(&dataBuff[9]) & 0x7fff);
					sb->AppendC(UTF8STRC("K"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Supported SRAM Type: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[11]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Current SRAM Type: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[13]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 19)
			{
				sb->AppendC(UTF8STRC("Cache Speed: "));
				sb->AppendU16(dataBuff[15]);
				sb->AppendC(UTF8STRC("ns\r\n"));
				sb->AppendC(UTF8STRC("Error Correction Type: "));
				switch (dataBuff[16])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unkown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("None"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Parity"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Single-bit ECC"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Multi-bit ECC"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[16]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("System Cache Type: "));
				switch (dataBuff[16])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Instruction"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Data"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Unified"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[16]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Associativity: "));
				switch (dataBuff[16])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Direct Mapped"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("2-way Set-Associative"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("4-way Set-Associative"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Full Associative"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("8-way Set-Associative"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("16-way Set-Associative"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("12-way Set-Associative"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("24-way Set-Associative"));
					break;
				case 11:
					sb->AppendC(UTF8STRC("32-way Set-Associative"));
					break;
				case 12:
					sb->AppendC(UTF8STRC("48-way Set-Associative"));
					break;
				case 13:
					sb->AppendC(UTF8STRC("64-way Set-Associative"));
					break;
				case 14:
					sb->AppendC(UTF8STRC("20-way Set-Associative"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[16]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 27)
			{
				sb->AppendC(UTF8STRC("Maximum Cache Size 2: "));
				if (dataBuff[22] & 0x80)
				{
					sb->AppendI32((ReadInt32(&dataBuff[19]) & 0x7fffffff) * 64);
					sb->AppendC(UTF8STRC("K"));
				}
				else
				{
					sb->AppendI32(ReadInt32(&dataBuff[19]) & 0x7fffffff);
					sb->AppendC(UTF8STRC("K"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Installed Cache Size 2: "));
				if (dataBuff[26] & 0x80)
				{
					sb->AppendI32((ReadInt32(&dataBuff[23]) & 0x7fffffff) * 64);
					sb->AppendC(UTF8STRC("K"));
				}
				else
				{
					sb->AppendI32(ReadInt32(&dataBuff[23]) & 0x7fffffff);
					sb->AppendC(UTF8STRC("K"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 8:
			sb->AppendC(UTF8STRC("SMBIOS Type 8 - Port Connector Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 9)
			{
				sb->AppendC(UTF8STRC("Internal Reference Designation: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Internal Connector Type: "));
				GetConnectorType(sb, dataBuff[5]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("External Reference Designation: "));
				if (carr[dataBuff[6]].leng > 0) sb->Append(carr[dataBuff[6]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("External Connector Type: "));
				GetConnectorType(sb, dataBuff[7]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Port Type: "));
				switch (dataBuff[8])
				{
				case 0:
					sb->AppendC(UTF8STRC("None"));
					break;
				case 1:
					sb->AppendC(UTF8STRC("Parallel Port XT/AT Compatible"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Parallel Port PS/2"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Parallel Port ECP"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Parallel Port EPP"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Parallel Port ECP/EPP"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Serial Port XT/AT Compatible"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("Serial Port 16450 Compatible"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("Serial Port 16550 Compatible"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("Serial Port 16550A Compatible"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("SCSI Port"));
					break;
				case 11:
					sb->AppendC(UTF8STRC("MIDI Port"));
					break;
				case 12:
					sb->AppendC(UTF8STRC("Joy Stick Port"));
					break;
				case 13:
					sb->AppendC(UTF8STRC("Keyboard Port"));
					break;
				case 14:
					sb->AppendC(UTF8STRC("Mouse Port"));
					break;
				case 15:
					sb->AppendC(UTF8STRC("SSA SCSI"));
					break;
				case 16:
					sb->AppendC(UTF8STRC("USB"));
					break;
				case 17:
					sb->AppendC(UTF8STRC("FireWire (IEEE P1394)"));
					break;
				case 18:
					sb->AppendC(UTF8STRC("PCMCIA Type I2"));
					break;
				case 19:
					sb->AppendC(UTF8STRC("PCMCIA Type II"));
					break;
				case 20:
					sb->AppendC(UTF8STRC("PCMCIA Type III"));
					break;
				case 21:
					sb->AppendC(UTF8STRC("Cardbus"));
					break;
				case 22:
					sb->AppendC(UTF8STRC("Access Bus Port"));
					break;
				case 23:
					sb->AppendC(UTF8STRC("SCSI II"));
					break;
				case 24:
					sb->AppendC(UTF8STRC("SCSI Wide"));
					break;
				case 25:
					sb->AppendC(UTF8STRC("PC-98"));
					break;
				case 26:
					sb->AppendC(UTF8STRC("PC-98-Hireso"));
					break;
				case 27:
					sb->AppendC(UTF8STRC("PC-H98"));
					break;
				case 28:
					sb->AppendC(UTF8STRC("Video Port"));
					break;
				case 29:
					sb->AppendC(UTF8STRC("Audio Port"));
					break;
				case 30:
					sb->AppendC(UTF8STRC("Modem Port"));
					break;
				case 31:
					sb->AppendC(UTF8STRC("Network Port"));
					break;
				case 32:
					sb->AppendC(UTF8STRC("SATA"));
					break;
				case 33:
					sb->AppendC(UTF8STRC("SAS"));
					break;
				case 160:
					sb->AppendC(UTF8STRC("8251 Compatible"));
					break;
				case 161:
					sb->AppendC(UTF8STRC("8251 FIFO Compatible"));
					break;
				case 255:
					sb->AppendC(UTF8STRC("Other"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[8]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 9:
			sb->AppendC(UTF8STRC("SMBIOS Type 9 - System Slots\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 12)
			{
				sb->AppendC(UTF8STRC("Slot Designation: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Slot Type: "));
				switch (dataBuff[5])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("ISA"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("MCA"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("EISA"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("PCI"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("PC Card (PCMCIA)"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("VL-VESA"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("Proprietary"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("Processor Card Slot"));
					break;
				case 11:
					sb->AppendC(UTF8STRC("Proprietary Memory Card Slot"));
					break;
				case 12:
					sb->AppendC(UTF8STRC("I/O Riser Card Slot"));
					break;
				case 13:
					sb->AppendC(UTF8STRC("NuBus"));
					break;
				case 14:
					sb->AppendC(UTF8STRC("PCI - 66MHz Capable"));
					break;
				case 15:
					sb->AppendC(UTF8STRC("AGP"));
					break;
				case 16:
					sb->AppendC(UTF8STRC("AGP 2X"));
					break;
				case 17:
					sb->AppendC(UTF8STRC("AGP 4X"));
					break;
				case 18:
					sb->AppendC(UTF8STRC("PCI-X"));
					break;
				case 19:
					sb->AppendC(UTF8STRC("AGP 8X"));
					break;
				case 20:
					sb->AppendC(UTF8STRC("M.2 Socket 1-DP (Mechanical Key A)"));
					break;
				case 21:
					sb->AppendC(UTF8STRC("M.2 Socket 1-SD (Mechanical Key E)"));
					break;
				case 22:
					sb->AppendC(UTF8STRC("M.2 Socket 2 (Mechanical Key B)"));
					break;
				case 23:
					sb->AppendC(UTF8STRC("M.2 Socket 3 (Mechanical Key M)"));
					break;
				case 24:
					sb->AppendC(UTF8STRC("MXM Type I"));
					break;
				case 25:
					sb->AppendC(UTF8STRC("MXM Type II"));
					break;
				case 26:
					sb->AppendC(UTF8STRC("MXM Type III (standard connector)"));
					break;
				case 27:
					sb->AppendC(UTF8STRC("MXM Type III (HE connector)"));
					break;
				case 28:
					sb->AppendC(UTF8STRC("MXM Type IV"));
					break;
				case 29:
					sb->AppendC(UTF8STRC("MXM 3.0 Type A"));
					break;
				case 30:
					sb->AppendC(UTF8STRC("MXM 3.0 Type B"));
					break;
				case 31:
					sb->AppendC(UTF8STRC("PCI Express Gen 2 SFF-8639"));
					break;
				case 32:
					sb->AppendC(UTF8STRC("PCI Express Gen 3 SFF-8639"));
					break;
				case 33:
					sb->AppendC(UTF8STRC("PCI Express Mini 52-pin (CEM spec. 2.0) with bottom-side keep-outs"));
					break;
				case 34:
					sb->AppendC(UTF8STRC("PCI Express Mini 52-pin (CEM spec. 2.0) without bottom-side keep-outs"));
					break;
				case 35:
					sb->AppendC(UTF8STRC("PCI Express Mini 76-pin (CEM spec. 2.0) Corresponds to Display-Mini card"));
					break;
				case 160:
					sb->AppendC(UTF8STRC("PC-98/C20"));
					break;
				case 161:
					sb->AppendC(UTF8STRC("PC-98/C24"));
					break;
				case 162:
					sb->AppendC(UTF8STRC("PC-98/E"));
					break;
				case 163:
					sb->AppendC(UTF8STRC("PC-98/Local Bus"));
					break;
				case 164:
					sb->AppendC(UTF8STRC("PC-98/Card"));
					break;
				case 165:
					sb->AppendC(UTF8STRC("PCI Express (see below)"));
					break;
				case 166:
					sb->AppendC(UTF8STRC("PCI Express x1"));
					break;
				case 167:
					sb->AppendC(UTF8STRC("PCI Express x2"));
					break;
				case 168:
					sb->AppendC(UTF8STRC("PCI Express x4"));
					break;
				case 169:
					sb->AppendC(UTF8STRC("PCI Express x8"));
					break;
				case 170:
					sb->AppendC(UTF8STRC("PCI Express x16"));
					break;
				case 171:
					sb->AppendC(UTF8STRC("PCI Express Gen 2 (see below)"));
					break;
				case 172:
					sb->AppendC(UTF8STRC("PCI Express Gen 2 x1"));
					break;
				case 173:
					sb->AppendC(UTF8STRC("PCI Express Gen 2 x2"));
					break;
				case 174:
					sb->AppendC(UTF8STRC("PCI Express Gen 2 x4"));
					break;
				case 175:
					sb->AppendC(UTF8STRC("PCI Express Gen 2 x8"));
					break;
				case 176:
					sb->AppendC(UTF8STRC("PCI Express Gen 2 x16"));
					break;
				case 177:
					sb->AppendC(UTF8STRC("PCI Express Gen 3 (see below)"));
					break;
				case 178:
					sb->AppendC(UTF8STRC("PCI Express Gen 3 x1"));
					break;
				case 179:
					sb->AppendC(UTF8STRC("PCI Express Gen 3 x2"));
					break;
				case 180:
					sb->AppendC(UTF8STRC("PCI Express Gen 3 x4"));
					break;
				case 181:
					sb->AppendC(UTF8STRC("PCI Express Gen 3 x8"));
					break;
				case 182:
					sb->AppendC(UTF8STRC("PCI Express Gen 3 x16"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[5]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Slot Data Bus Width: "));
				switch (dataBuff[6])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("8 bit"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("16 bit"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("32 bit"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("64 bit"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("128 bit"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("1x or x1"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("2x or x2"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("4x or x4"));
					break;
				case 11:
					sb->AppendC(UTF8STRC("8x or x8"));
					break;
				case 12:
					sb->AppendC(UTF8STRC("12x or x12"));
					break;
				case 13:
					sb->AppendC(UTF8STRC("16x or x16"));
					break;
				case 14:
					sb->AppendC(UTF8STRC("32x or x32"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[6]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Current Usage: "));
				switch (dataBuff[7])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Available"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("In use"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[7]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Slot Length: "));
				switch (dataBuff[8])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Short Length"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Long Length"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("2.5\" drive form factor"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("3.5\" drive form factor"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[8]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Slot ID: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[9]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Slot Characteristics 1: 0x"));
				sb->AppendHex8(dataBuff[11]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 13)
			{
				sb->AppendC(UTF8STRC("Slot Characteristics 2: 0x"));
				sb->AppendHex8(dataBuff[12]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 17)
			{
				sb->AppendC(UTF8STRC("Segment Group Number: "));
				sb->AppendI16(ReadInt16(&dataBuff[13]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Bus Number: "));
				sb->AppendU16(dataBuff[15]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Device/Function Number: 0x"));
				sb->AppendHex8(dataBuff[16]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 10:
			sb->AppendC(UTF8STRC("SMBIOS Type 10 - On Board Devices Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			l = 4;
			k = 1;
			while (l < dataBuff[1])
			{
				sb->AppendC(UTF8STRC("Device "));
				sb->AppendUOSInt(k);
				sb->AppendC(UTF8STRC(" Status: "));
				if (dataBuff[l] & 0x80)
				{
					sb->AppendC(UTF8STRC("Enabled"));
				}
				else
				{
					sb->AppendC(UTF8STRC("Disabled"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Device "));
				sb->AppendUOSInt(k);
				sb->AppendC(UTF8STRC(" Type: "));
				switch (dataBuff[l] & 0x7f)
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Video"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("SCSI Controller"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Ethernet"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Token Ring"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("Sound"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("PATA Controller"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("SATA Controller"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("SAS Controller"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[l] & 0x7f);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Device "));
				sb->AppendUOSInt(k);
				sb->AppendC(UTF8STRC(" Description: "));
				if (carr[dataBuff[l + 1]].leng > 0) sb->Append(carr[dataBuff[l + 1]]);
				sb->AppendC(UTF8STRC("\r\n"));
				l += 2;
				k += 1;
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 11:
			sb->AppendC(UTF8STRC("SMBIOS Type 11 - OEM Strings\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Number of strings: "));
			sb->AppendU16(dataBuff[4]);
			sb->AppendC(UTF8STRC("\r\n"));
			k = 0;
			while (k < 16)
			{
				if (carr[k].leng > 0)
				{
					sb->AppendC(UTF8STRC("String "));
					sb->AppendUOSInt(k);
					sb->AppendC(UTF8STRC(": "));
					sb->Append(carr[k]);
					sb->AppendC(UTF8STRC("\r\n"));
				}
				k++;
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 12:
			sb->AppendC(UTF8STRC("SMBIOS Type 12 - System Configuration Options\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Number of strings: "));
			sb->AppendU16(dataBuff[4]);
			sb->AppendC(UTF8STRC("\r\n"));
			k = 0;
			while (k < 16)
			{
				if (carr[k].leng > 0)
				{
					sb->AppendC(UTF8STRC("String "));
					sb->AppendUOSInt(k);
					sb->AppendC(UTF8STRC(": "));
					sb->Append(carr[k]);
					sb->AppendC(UTF8STRC("\r\n"));
				}
				k++;
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 13:
			sb->AppendC(UTF8STRC("SMBIOS Type 13 - BIOS Language Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 5)
			{
				sb->AppendC(UTF8STRC("Installable Languages: "));
				sb->AppendU16(dataBuff[4]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 6)
			{
				sb->AppendC(UTF8STRC("Flags: 0x"));
				sb->AppendHex8(dataBuff[5]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 22)
			{
				sb->AppendC(UTF8STRC("Current Language: "));
				sb->AppendU16(dataBuff[21]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			k = 0;
			while (k < 16)
			{
				if (carr[k].leng > 0)
				{
					sb->AppendC(UTF8STRC("Language "));
					sb->AppendUOSInt(k);
					sb->AppendC(UTF8STRC(": "));
					sb->Append(carr[k]);
					sb->AppendC(UTF8STRC("\r\n"));
				}
				k++;
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 14:
			sb->AppendC(UTF8STRC("SMBIOS Type 14 - Group Association\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Group Name: "));
			if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
			sb->AppendC(UTF8STRC("\r\n"));
			k = 5;
			while (k < dataBuff[1])
			{
				sb->AppendC(UTF8STRC("Item Type: "));
				sb->AppendU16(dataBuff[k]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Item Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[k + 1]));
				sb->AppendC(UTF8STRC("\r\n"));
				k += 3;
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 15:
			sb->AppendC(UTF8STRC("SMBIOS Type 15 - System Event Log\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 20)
			{
				sb->AppendC(UTF8STRC("Log Area Length: "));
				sb->AppendI16(ReadInt16(&dataBuff[4]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Log Header Start Offset: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[6]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Log Data Start Offset: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[8]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Access Method: "));
				switch (dataBuff[10])
				{
				case 0:
					sb->AppendC(UTF8STRC("Indexed I/O: 1 8-bit index port, 1 8-bit data port"));
					break;
				case 1:
					sb->AppendC(UTF8STRC("Indexed I/O: 2 8-bit index ports, 1 8-bit data port"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Indexed I/O: 1 16-bit index port, 1 8-bit data port"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Memory-mapped physical 32-bit address"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Available through General-Purpose NonVolatile Data functions"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[10]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Log Status: 0x"));
				sb->AppendHex8(dataBuff[11]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Log Change Token: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[12]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Access Method Address: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[16]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 23)
			{
				sb->AppendC(UTF8STRC("Log Header Format: "));
				switch (dataBuff[20])
				{
				case 0:
					sb->AppendC(UTF8STRC("No Header"));
					break;
				case 1:
					sb->AppendC(UTF8STRC("Type 1 log header"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[20]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Number of Supported Log Type Descriptor: "));
				sb->AppendU16(dataBuff[21]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Length of each Log Type Descriptor: "));
				sb->AppendU16(dataBuff[22]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 16:
			sb->AppendC(UTF8STRC("SMBIOS Type 16 - Physical Memory Array\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 15)
			{
				sb->AppendC(UTF8STRC("Location: "));
				switch (dataBuff[4])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("System board or motherboard"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("ISA add-on card"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("EISA add-on card"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("PCI add-on card"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("MCA add-on card"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("PCMCIA add-on card"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("Proprietary add-on card"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("NuBus"));
					break;
				case 160:
					sb->AppendC(UTF8STRC("PC-98/C20 add-on card"));
					break;
				case 161:
					sb->AppendC(UTF8STRC("PC-98/C24 add-on card"));
					break;
				case 162:
					sb->AppendC(UTF8STRC("PC-98/E add-on card"));
					break;
				case 163:
					sb->AppendC(UTF8STRC("PC-98/Local bus add-on card"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[4]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Use: "));
				switch (dataBuff[5])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("System memory"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Video memory"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Flash memory"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Non-volatile RAM"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("Cache memory"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[5]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Memory Error Correction: "));
				switch (dataBuff[6])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("None"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Parity"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Single-bit ECC"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Multi-bit ECC"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("CRC"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[6]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Maximum Capacity: "));
				sb->AppendI32(ReadInt32(&dataBuff[7]));
				sb->AppendC(UTF8STRC("KB\r\n"));
				sb->AppendC(UTF8STRC("Memory Error Information Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[11]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Number of Memory Devices: "));
				sb->AppendI16(ReadInt16(&dataBuff[13]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 17:
			sb->AppendC(UTF8STRC("SMBIOS Type 17 - Memory Device\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 21)
			{
				sb->AppendC(UTF8STRC("Physical Memory Array Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[4]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Memory Error Information Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[6]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Total Width: "));
				sb->AppendI16(ReadInt16(&dataBuff[8]));
				sb->AppendC(UTF8STRC(" bits\r\n"));
				sb->AppendC(UTF8STRC("Data Width: "));
				sb->AppendI16(ReadInt16(&dataBuff[10]));
				sb->AppendC(UTF8STRC(" bits\r\n"));
				sb->AppendC(UTF8STRC("Size: "));
				if (dataBuff[13] & 0x80)
				{
					sb->AppendI16(ReadInt16(&dataBuff[12]) & 0x7fff);
					sb->AppendC(UTF8STRC("KB"));
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[12]) & 0x7fff);
					sb->AppendC(UTF8STRC("MB"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Form Factor: "));
				switch (dataBuff[14])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("SIMM"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("SIP"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Chip"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("DIP"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("ZIP"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("Proprietary Card"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("DIMM"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("TSOP"));
					break;
				case 11:
					sb->AppendC(UTF8STRC("Row of chips"));
					break;
				case 12:
					sb->AppendC(UTF8STRC("RIMM"));
					break;
				case 13:
					sb->AppendC(UTF8STRC("SODIMM"));
					break;
				case 14:
					sb->AppendC(UTF8STRC("SRIMM"));
					break;
				case 15:
					sb->AppendC(UTF8STRC("FB-DIMM"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[14]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Device Set: "));
				sb->AppendU16(dataBuff[15]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Device Locator: "));
				if (carr[dataBuff[16]].leng > 0) sb->Append(carr[dataBuff[16]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Bank Locator: "));
				if (carr[dataBuff[17]].leng > 0) sb->Append(carr[dataBuff[17]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Memory Type: "));
				switch (dataBuff[18])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("DRAM"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("EDRAM"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("VRAM"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("SRAM"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("RAM"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("ROM"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("FLASH"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("EEPROM"));
					break;
				case 11:
					sb->AppendC(UTF8STRC("FEPROM"));
					break;
				case 12:
					sb->AppendC(UTF8STRC("EPROM"));
					break;
				case 13:
					sb->AppendC(UTF8STRC("CDRAM"));
					break;
				case 14:
					sb->AppendC(UTF8STRC("3DRAM"));
					break;
				case 15:
					sb->AppendC(UTF8STRC("SDRAM"));
					break;
				case 16:
					sb->AppendC(UTF8STRC("SGRAM"));
					break;
				case 17:
					sb->AppendC(UTF8STRC("RDRAM"));
					break;
				case 18:
					sb->AppendC(UTF8STRC("DDR"));
					break;
				case 19:
					sb->AppendC(UTF8STRC("DDR2"));
					break;
				case 20:
					sb->AppendC(UTF8STRC("DDR2 FB-DIMM"));
					break;
				case 24:
					sb->AppendC(UTF8STRC("DDR3"));
					break;
				case 25:
					sb->AppendC(UTF8STRC("FBD2"));
					break;
				case 26:
					sb->AppendC(UTF8STRC("DDR4"));
					break;
				case 27:
					sb->AppendC(UTF8STRC("LPDDR"));
					break;
				case 28:
					sb->AppendC(UTF8STRC("LPDDR2"));
					break;
				case 29:
					sb->AppendC(UTF8STRC("LPDDR3"));
					break;
				case 30:
					sb->AppendC(UTF8STRC("LPDDR4"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[18]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Type Detail: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[19]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 27)
			{
				sb->AppendC(UTF8STRC("Speed: "));
				sb->AppendI16(ReadInt16(&dataBuff[21]));
				sb->AppendC(UTF8STRC("MT/s\r\n"));
				sb->AppendC(UTF8STRC("Manufacturer: "));
				if (carr[dataBuff[23]].leng > 0) sb->Append(carr[dataBuff[23]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Serial Number: "));
				if (carr[dataBuff[24]].leng > 0) sb->Append(carr[dataBuff[24]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Asset Tag: "));
				if (carr[dataBuff[25]].leng > 0) sb->Append(carr[dataBuff[25]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Part Number: "));
				if (carr[dataBuff[26]].leng > 0) sb->Append(carr[dataBuff[26]]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 28)
			{
				sb->AppendC(UTF8STRC("Attributes: 0x"));
				sb->AppendHex8(dataBuff[27]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 34)
			{
				sb->AppendC(UTF8STRC("Extended Size: "));
				sb->AppendI32(ReadInt32(&dataBuff[28]));
				sb->AppendC(UTF8STRC("MB\r\n"));
				sb->AppendC(UTF8STRC("Configured Memory Clock Speed: "));
				sb->AppendI16(ReadInt16(&dataBuff[32]));
				sb->AppendC(UTF8STRC("MT/s\r\n"));
			}
			if (dataBuff[1] >= 40)
			{
				sb->AppendC(UTF8STRC("Minimum voltage: "));
				Text::SBAppendF64(sb, ReadInt16(&dataBuff[34]) * 0.001);
				sb->AppendC(UTF8STRC("V\r\n"));
				sb->AppendC(UTF8STRC("Maximum voltage: "));
				Text::SBAppendF64(sb, ReadInt16(&dataBuff[36]) * 0.001);
				sb->AppendC(UTF8STRC("V\r\n"));
				sb->AppendC(UTF8STRC("Configured voltage: "));
				Text::SBAppendF64(sb, ReadInt16(&dataBuff[38]) * 0.001);
				sb->AppendC(UTF8STRC("V\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
/*		case 18:
			sb->AppendC(UTF8STRC("SMBIOS Type 18 - 32-Bit Memory Error Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));
			break;*/
		case 19:
			sb->AppendC(UTF8STRC("SMBIOS Type 19 - Memory Array Mapped Address\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 15)
			{
				sb->AppendC(UTF8STRC("Starting Address: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[4]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Ending Address: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[8]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Memory Array Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[12]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Partition Width: "));
				sb->AppendU16(dataBuff[14]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 31)
			{
				sb->AppendC(UTF8STRC("Extended Starting Address: 0x"));
				sb->AppendHex64(ReadUInt64(&dataBuff[15]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Extended Ending Address: 0x"));
				sb->AppendHex64(ReadUInt64(&dataBuff[23]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 20:
			sb->AppendC(UTF8STRC("SMBIOS Type 20 - Memory Device Mapped Address\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 19)
			{
				sb->AppendC(UTF8STRC("Starting Address: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[4]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Ending Address: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[8]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Memory Device Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[12]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Memory Array Mapped Address Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[14]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Partition Row Position: "));
				sb->AppendU16(dataBuff[16]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Interleave Position: "));
				sb->AppendU16(dataBuff[17]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Interleave Data Depth: "));
				sb->AppendU16(dataBuff[18]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 35)
			{
				sb->AppendC(UTF8STRC("Extended Starting Address: 0x"));
				sb->AppendHex64(ReadUInt64(&dataBuff[19]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Extended Ending Address: 0x"));
				sb->AppendHex64(ReadUInt64(&dataBuff[27]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 21:
			sb->AppendC(UTF8STRC("SMBIOS Type 21 - Build-in Pointing Device\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 7)
			{
				sb->AppendC(UTF8STRC("Type: "));
				switch (dataBuff[4])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Mouse"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Track Ball"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Track Point"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Glide Point"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("Touch Pad"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("Touch Screen"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("Optical Sensor"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[4]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Interface: "));
				switch (dataBuff[5])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Serial"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("PS/2"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Infrared"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("HP-HIL"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("Bus mouse"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("ADB (Apple Desktop Bus)"));
					break;
				case 160:
					sb->AppendC(UTF8STRC("Bus mouse DB-9"));
					break;
				case 161:
					sb->AppendC(UTF8STRC("Bus mouse micro-DIN"));
					break;
				case 162:
					sb->AppendC(UTF8STRC("USB"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[5]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Number of Buttons: "));
				sb->AppendU16(dataBuff[6]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 22:
			sb->AppendC(UTF8STRC("SMBIOS Type 22 - Portable Battery\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Location: "));
			if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Manufacturer: "));
			if (carr[dataBuff[5]].leng > 0) sb->Append(carr[dataBuff[5]]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Manufacture Date: "));
			if (carr[dataBuff[6]].leng > 0) sb->Append(carr[dataBuff[6]]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Serial Number: "));
			if (carr[dataBuff[7]].leng > 0) sb->Append(carr[dataBuff[7]]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Device Name: "));
			if (carr[dataBuff[8]].leng > 0) sb->Append(carr[dataBuff[8]]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Device Chemistry: "));
			switch (dataBuff[9])
			{
			case 1:
				sb->AppendC(UTF8STRC("Other"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("Unknown"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("Lead Acid"));
				break;
			case 4:
				sb->AppendC(UTF8STRC("Nickel Cadmium"));
				break;
			case 5:
				sb->AppendC(UTF8STRC("Nickel metal hydride"));
				break;
			case 6:
				sb->AppendC(UTF8STRC("Lithium-ion"));
				break;
			case 7:
				sb->AppendC(UTF8STRC("Zinc air"));
				break;
			case 8:
				sb->AppendC(UTF8STRC("Lithium Polymer"));
				break;
			default:
				sb->AppendC(UTF8STRC("Undefined ("));
				sb->AppendU16(dataBuff[9]);
				sb->AppendC(UTF8STRC(")"));
				break;
			}
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Design Capacity: "));
			sb->AppendU16(ReadUInt16(&dataBuff[10]));
			sb->AppendC(UTF8STRC("mWh\r\n"));
			sb->AppendC(UTF8STRC("Design Voltage: "));
			sb->AppendU16(ReadUInt16(&dataBuff[12]));
			sb->AppendC(UTF8STRC("mV\r\n"));
			sb->AppendC(UTF8STRC("SBDS Version Number: "));
			if (carr[dataBuff[14]].leng > 0) sb->Append(carr[dataBuff[14]]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Maximum Error in Battery Data: "));
			sb->AppendU16(dataBuff[15]);
			sb->AppendC(UTF8STRC("%\r\n"));
			if (dataBuff[1] >= 26)
			{
				sb->AppendC(UTF8STRC("SBDS Serial Number: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[16]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("SBDS Manufacture Date: "));
				sb->AppendU16((UInt16)(1980 + (dataBuff[19] >> 1)));
				sb->AppendC(UTF8STRC("-"));
				sb->AppendU16((UInt16)((dataBuff[18] >> 5) | ((dataBuff[19] & 1) << 3)));
				sb->AppendC(UTF8STRC("-"));
				sb->AppendU16(dataBuff[18] & 31);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("SBDS Device Chemistry: "));
				if (carr[dataBuff[20]].leng > 0) sb->Append(carr[dataBuff[20]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Design Capacity Multiplier: "));
				sb->AppendU16(dataBuff[21]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("OEM-specific: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[22]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 23:
			sb->AppendC(UTF8STRC("SMBIOS Type 23 - System Reset\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 13)
			{
				sb->AppendC(UTF8STRC("Capabilities: 0x"));
				sb->AppendHex8(dataBuff[4]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Reset Count: "));
				sb->AppendI16(ReadInt16(&dataBuff[5]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Reset Limit: "));
				sb->AppendI16(ReadInt16(&dataBuff[7]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Time Interval: "));
				sb->AppendI16(ReadInt16(&dataBuff[9]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Timeout: "));
				sb->AppendI16(ReadInt16(&dataBuff[11]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 24:
			sb->AppendC(UTF8STRC("SMBIOS Type 24 - Hardware Security\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 5)
			{
				sb->AppendC(UTF8STRC("Hardware Security Settings: 0x"));
				sb->AppendHex8(dataBuff[4]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 25:
			sb->AppendC(UTF8STRC("SMBIOS Type 25 - System Power Control\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 9)
			{
				sb->AppendC(UTF8STRC("Next Scheduled Power-on Month: "));
				sb->AppendHex8(dataBuff[4]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Next Scheduled Power-on Day-of-month: "));
				sb->AppendHex8(dataBuff[5]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Next Scheduled Power-on Hour: "));
				sb->AppendHex8(dataBuff[6]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Next Scheduled Power-on Minute: "));
				sb->AppendHex8(dataBuff[7]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Next Scheduled Power-on Second: "));
				sb->AppendHex8(dataBuff[8]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 26:
			sb->AppendC(UTF8STRC("SMBIOS Type 26 - Voltage Probe\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 20)
			{
				sb->AppendC(UTF8STRC("Description: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Location: "));
				switch (dataBuff[5] & 0x1f)
				{
				case 1:
					sb->AppendC(UTF8STRC("OTher"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Processor"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Disk"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Peripheral Bay"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("System Management Module"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("Motherboard"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("Memory Module"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("Processor Module"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("Power Unit"));
					break;
				case 11:
					sb->AppendC(UTF8STRC("Add-in Card"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[5] & 0x1f);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Status: "));
				switch (dataBuff[5] >> 5)
				{
				case 1:
					sb->AppendC(UTF8STRC("OTher"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("OK"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Non-critical"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Critical"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Non-recoverable"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16((UInt16)(dataBuff[5] >> 5));
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Maximum Value: "));
				if (ReadInt16(&dataBuff[6]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[6]) * 0.001);
					sb->AppendC(UTF8STRC("V"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Minimum Value: "));
				if (ReadInt16(&dataBuff[8]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[8]) * 0.001);
					sb->AppendC(UTF8STRC("V"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Resolution: "));
				if (ReadInt16(&dataBuff[10]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[10]) * 0.01);
					sb->AppendC(UTF8STRC("V"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Tolerance: "));
				if (ReadInt16(&dataBuff[12]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[12]) * 0.001);
					sb->AppendC(UTF8STRC("V"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Accuracy: "));
				if (ReadInt16(&dataBuff[14]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[14]) * 0.01);
					sb->AppendC(UTF8STRC("%"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("OEM-defined: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[16]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 22)
			{
				sb->AppendC(UTF8STRC("Nominal: "));
				if (ReadInt16(&dataBuff[20]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[20]) * 0.001);
					sb->AppendC(UTF8STRC("V"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 27:
			sb->AppendC(UTF8STRC("SMBIOS Type 27 - Cooling Device\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 12)
			{
				sb->AppendC(UTF8STRC("Temperature Probe Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[4]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Device Type: "));
				switch (dataBuff[6] & 0x1f)
				{
				case 1:
					sb->AppendC(UTF8STRC("OTher"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Fan"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Centrifugal Blower"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Chip Fan"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Cabinet Fan"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("Power Supply Fan"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("Heat Pipe"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("Integrated Refrigeration"));
					break;
				case 16:
					sb->AppendC(UTF8STRC("Active Cooling"));
					break;
				case 17:
					sb->AppendC(UTF8STRC("Passive Cooling"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[6] & 0x1f);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Status: "));
				switch (dataBuff[6] >> 5)
				{
				case 1:
					sb->AppendC(UTF8STRC("OTher"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("OK"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Non-critical"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Critical"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Non-recoverable"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16((UInt16)(dataBuff[6] >> 5));
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Cooling Unit Group: "));
				sb->AppendU16(dataBuff[7]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("OEM-defined: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[8]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 14)
			{
				sb->AppendC(UTF8STRC("Nominal Speed: "));
				if (ReadInt16(&dataBuff[12]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[12]));
					sb->AppendC(UTF8STRC("rpm"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 15)
			{
				sb->AppendC(UTF8STRC("Description: "));
				if (carr[dataBuff[14]].leng > 0) sb->Append(carr[dataBuff[14]]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 28:
			sb->AppendC(UTF8STRC("SMBIOS Type 28 - Temperature Probe\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 20)
			{
				sb->AppendC(UTF8STRC("Description: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Location: "));
				switch (dataBuff[5] & 0x1f)
				{
				case 1:
					sb->AppendC(UTF8STRC("OTher"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Processor"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Disk"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Peripheral Bay"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("System Management Module"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("Motherboard"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("Memory Module"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("Processor Module"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("Power Unit"));
					break;
				case 11:
					sb->AppendC(UTF8STRC("Add-in Card"));
					break;
				case 12:
					sb->AppendC(UTF8STRC("Front Panel Board"));
					break;
				case 13:
					sb->AppendC(UTF8STRC("Back Panel Board"));
					break;
				case 14:
					sb->AppendC(UTF8STRC("Power System Board"));
					break;
				case 15:
					sb->AppendC(UTF8STRC("Drive Back Plane"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[5] & 0x1f);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Status: "));
				switch (dataBuff[5] >> 5)
				{
				case 1:
					sb->AppendC(UTF8STRC("OTher"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("OK"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Non-critical"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Critical"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Non-recoverable"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16((UInt16)(dataBuff[5] >> 5));
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Maximum Value: "));
				if (ReadInt16(&dataBuff[6]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[6]) * 0.1);
					sb->AppendC(UTF8STRC("degree C"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Minimum Value: "));
				if (ReadInt16(&dataBuff[8]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[8]) * 0.1);
					sb->AppendC(UTF8STRC("degree C"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Resolution: "));
				if (ReadInt16(&dataBuff[10]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[10]) * 0.001);
					sb->AppendC(UTF8STRC("degree C"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Tolerance: "));
				if (ReadInt16(&dataBuff[12]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[12]) * 0.1);
					sb->AppendC(UTF8STRC("degree C"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Accuracy: "));
				if (ReadInt16(&dataBuff[14]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[14]) * 0.01);
					sb->AppendC(UTF8STRC("%"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("OEM-defined: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[16]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 22)
			{
				sb->AppendC(UTF8STRC("Nominal Value: "));
				if (ReadInt16(&dataBuff[20]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[20]) * 0.1);
					sb->AppendC(UTF8STRC("degree C"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 29:
			sb->AppendC(UTF8STRC("SMBIOS Type 29 - Electrical Current Probe\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 20)
			{
				sb->AppendC(UTF8STRC("Description: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Location: "));
				switch (dataBuff[5] & 0x1f)
				{
				case 1:
					sb->AppendC(UTF8STRC("OTher"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Processor"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Disk"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Peripheral Bay"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("System Management Module"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("Motherboard"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("Memory Module"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("Processor Module"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("Power Unit"));
					break;
				case 11:
					sb->AppendC(UTF8STRC("Add-in Card"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[5] & 0x1f);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Status: "));
				switch (dataBuff[5] >> 5)
				{
				case 1:
					sb->AppendC(UTF8STRC("OTher"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("OK"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Non-critical"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Critical"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Non-recoverable"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16((UInt16)(dataBuff[5] >> 5));
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Maximum Value: "));
				if (ReadInt16(&dataBuff[6]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[6]) * 0.001);
					sb->AppendC(UTF8STRC("A"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Minimum Value: "));
				if (ReadInt16(&dataBuff[8]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[8]) * 0.001);
					sb->AppendC(UTF8STRC("A"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Resolution: "));
				if (ReadInt16(&dataBuff[10]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[10]) * 0.01);
					sb->AppendC(UTF8STRC("A"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Tolerance: "));
				if (ReadInt16(&dataBuff[12]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[12]) * 0.001);
					sb->AppendC(UTF8STRC("A"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Accuracy: "));
				if (ReadInt16(&dataBuff[14]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[14]) * 0.01);
					sb->AppendC(UTF8STRC("%"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("OEM-defined: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[16]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 22)
			{
				sb->AppendC(UTF8STRC("Nominal Value: "));
				if (ReadInt16(&dataBuff[20]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[20]) * 0.001);
					sb->AppendC(UTF8STRC("A"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 30:
			sb->AppendC(UTF8STRC("SMBIOS Type 30 - Out-of-Band Remote Access\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 6)
			{
				sb->AppendC(UTF8STRC("Manufacturer Name: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Connectionx: 0x"));
				sb->AppendHex8(dataBuff[5]);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
/*		case 31:
			sb->AppendC(UTF8STRC("SMBIOS Type 31 - Boot Integrity Services Entry Point\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));
			break;*/
		case 32:
			sb->AppendC(UTF8STRC("SMBIOS Type 32 - System Boot Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 11)
			{
				sb->AppendC(UTF8STRC("Boot Status: "));
				switch (dataBuff[10])
				{
				case 0:
					sb->AppendC(UTF8STRC("No errors detected"));
					break;
				case 1:
					sb->AppendC(UTF8STRC("No bootable media"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("\"normal\" operating system failed to load"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Firmware-detected hardware failure, including \"unknown\" failure types"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Operating system-detected hardware failure"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("User-requested boot, usually through a keystroke"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("System security violation"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("Previously-requested image"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("System watchdog timer expired, causing the system to reboot"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[10]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
/*		case 33:
			sb->AppendC(UTF8STRC("SMBIOS Type 33 - 64-Bit Memory Error Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));
			break;*/
		case 34:
			sb->AppendC(UTF8STRC("SMBIOS Type 34 - Management Device\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 11)
			{
				sb->AppendC(UTF8STRC("Description: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Type: "));
				switch (dataBuff[5])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("National Semiconductor LM75"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("National Semiconductor LM78"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("National Semiconductor LM79"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("National Semiconductor LM80"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("National Semiconductor LM81"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("Analog Devices ADM9240"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("Dallas Semiconductor DS1780"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("Maxim 1617"));
					break;
				case 11:
					sb->AppendC(UTF8STRC("Genesys GL518SM"));
					break;
				case 12:
					sb->AppendC(UTF8STRC("Winbond W83781D"));
					break;
				case 13:
					sb->AppendC(UTF8STRC("Holtek HT82H791"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[5]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Address: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[6]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Address Type: "));
				switch (dataBuff[10])
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("I/O Port"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("Memory"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("SM Bus"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[10]);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 35:
			sb->AppendC(UTF8STRC("SMBIOS Type 35 - Management Device Component\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 11)
			{
				sb->AppendC(UTF8STRC("Description: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Management Device Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[5]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Component Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[7]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Threshold Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[9]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 36:
			sb->AppendC(UTF8STRC("SMBIOS Type 36 - Management Device Threshold Data\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 16)
			{
				sb->AppendC(UTF8STRC("Lower Threshold - Non-critical: "));
				if (ReadInt16(&dataBuff[4]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[4]));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Upper Threshold - Non-critical: "));
				if (ReadInt16(&dataBuff[6]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[6]));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Lower Threshold - Critical: "));
				if (ReadInt16(&dataBuff[8]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[8]));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Upper Threshold - Critical: "));
				if (ReadInt16(&dataBuff[10]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[10]));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Lower Threshold - Non-recoverable: "));
				if (ReadInt16(&dataBuff[12]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[12]));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Upper Threshold - Non-recoverable: "));
				if (ReadInt16(&dataBuff[14]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[14]));
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
/*		case 37:
			sb->AppendC(UTF8STRC("SMBIOS Type 37 - Memory Channel\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));
			break;*/
/*		case 38:
			sb->AppendC(UTF8STRC("SMBIOS Type 38 - IPMI Device Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));
			break;*/
		case 39:
			sb->AppendC(UTF8STRC("SMBIOS Type 39 - System Power Supply\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 16)
			{
				sb->AppendC(UTF8STRC("Power Unit Group: "));
				sb->AppendU16(dataBuff[4]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Location: "));
				if (carr[dataBuff[5]].leng > 0) sb->Append(carr[dataBuff[5]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Device Name: "));
				if (carr[dataBuff[6]].leng > 0) sb->Append(carr[dataBuff[6]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Manufacturer: "));
				if (carr[dataBuff[7]].leng > 0) sb->Append(carr[dataBuff[7]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Serial Number: "));
				if (carr[dataBuff[8]].leng > 0) sb->Append(carr[dataBuff[8]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Asset Tag Number: "));
				if (carr[dataBuff[9]].leng > 0) sb->Append(carr[dataBuff[9]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Model Part Number: "));
				if (carr[dataBuff[10]].leng > 0) sb->Append(carr[dataBuff[10]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Revision Level: "));
				if (carr[dataBuff[11]].leng > 0) sb->Append(carr[dataBuff[11]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Max Power Capacity: "));
				if (ReadInt16(&dataBuff[12]) == -0x8000)
				{
					sb->AppendC(UTF8STRC("unknown"));
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[12]));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Power Supply Characteristics: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[14]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (dataBuff[1] >= 22)
			{
				sb->AppendC(UTF8STRC("Input Voltage Probe Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[16]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Cooling Device Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[18]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Input Current Probe Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[20]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 40:
			sb->AppendC(UTF8STRC("SMBIOS Type 40 - Additional Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Number of Additional Information entries: "));
			sb->AppendU16(dataBuff[4]);
			sb->AppendC(UTF8STRC("\r\n"));
			l = dataBuff[4];
			k = 5;
			while (k < dataBuff[1])
			{
				if (dataBuff[k] < 6)
					break;
				if (l-- <= 0)
					break;
				
				sb->AppendC(UTF8STRC("Entry Length: "));
				sb->AppendU16(dataBuff[k]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Referenced Handle: 0x"));
				sb->AppendHex16(ReadUInt16(&dataBuff[k + 1]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Referenced Offset: "));
				sb->AppendU16(dataBuff[k + 3]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("String: "));
				if (carr[dataBuff[k + 4]].leng > 0) sb->Append(carr[dataBuff[k + 4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				k += dataBuff[k];
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 41:
			sb->AppendC(UTF8STRC("SMBIOS Type 41 - Onboard Devices Extended Information\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 11)
			{
				sb->AppendC(UTF8STRC("Reference Designation: "));
				if (carr[dataBuff[4]].leng > 0) sb->Append(carr[dataBuff[4]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Device Type: "));
				switch (dataBuff[5] & 0x7f)
				{
				case 1:
					sb->AppendC(UTF8STRC("Other"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("Video"));
					break;
				case 4:
					sb->AppendC(UTF8STRC("SCSI Controller"));
					break;
				case 5:
					sb->AppendC(UTF8STRC("Ethernet"));
					break;
				case 6:
					sb->AppendC(UTF8STRC("Token Ring"));
					break;
				case 7:
					sb->AppendC(UTF8STRC("Sound"));
					break;
				case 8:
					sb->AppendC(UTF8STRC("PATA Controller"));
					break;
				case 9:
					sb->AppendC(UTF8STRC("SATA Controller"));
					break;
				case 10:
					sb->AppendC(UTF8STRC("SAS Controller"));
					break;
				default:
					sb->AppendC(UTF8STRC("Undefined ("));
					sb->AppendU16(dataBuff[5] & 0x7f);
					sb->AppendC(UTF8STRC(")"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Device Status: "));
				if (dataBuff[5] & 0x80)
				{
					sb->AppendC(UTF8STRC("Device Enabled"));
				}
				else
				{
					sb->AppendC(UTF8STRC("Device Disabled"));
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Device Type Instance: "));
				sb->AppendU16(dataBuff[6]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Segment Group Number: "));
				sb->AppendI16(ReadInt16(&dataBuff[7]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Bus Number: "));
				sb->AppendU16(dataBuff[9]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Device Number: "));
				sb->AppendU16((UInt16)(dataBuff[10] >> 3));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Function Number: "));
				sb->AppendU16(dataBuff[10] & 7);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
/*		case 42:
			sb->AppendC(UTF8STRC("SMBIOS Type 42 - Management Controller Host Interface\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));
			break;*/
		case 43:
			sb->AppendC(UTF8STRC("SMBIOS Type 43 - TPM Device\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 0x1F)
			{
				sb->AppendC(UTF8STRC("Vendor ID: "));
				if (dataBuff[4] == 0)
				{

				}
				else if (dataBuff[5] == 0)
				{
					sb->AppendC(&dataBuff[4], 1);
				}
				else if (dataBuff[6] == 0)
				{
					sb->AppendC(&dataBuff[4], 2);
				}
				else if (dataBuff[7] == 0)
				{
					sb->AppendC(&dataBuff[4], 3);
				}
				else
				{
					sb->AppendC(&dataBuff[4], 4);
				}
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Spec Version: "));
				sb->AppendU16(dataBuff[8]);
				sb->AppendUTF8Char('.');
				sb->AppendU16(dataBuff[9]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Firmware Version 1: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[10]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Firmware Version 2: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[14]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Description: "));
				if (carr[dataBuff[18]].leng > 0) sb->Append(carr[dataBuff[18]]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Characteristic: 0x"));
				sb->AppendHex64(ReadUInt64(&dataBuff[19]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("OEM-defined: 0x"));
				sb->AppendHex32(ReadUInt32(&dataBuff[27]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 126:
			sb->AppendC(UTF8STRC("SMBIOS Type 126 - Inactive\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 127:
			sb->AppendC(UTF8STRC("SMBIOS Type 127 - End-of-Table\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		case 128:
			sb->AppendC(UTF8STRC("SMBIOS Type 128 - Apple Firmware Volume\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 16)
			{
				sb->AppendC(UTF8STRC("Region Count: "));
				sb->AppendU16(dataBuff[4]);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Firmware Feature: 0x"));
				sb->AppendHex32(ReadUInt16(&dataBuff[8]));
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Firmware Feature Mask: 0x"));
				sb->AppendHex32(ReadUInt16(&dataBuff[12]));
				sb->AppendC(UTF8STRC("\r\n"));
				
				if (dataBuff[1] >= 24 + dataBuff[4])
				{
					k = 0;
					while (k < dataBuff[4])
					{
						sb->AppendC(UTF8STRC("Region "));
						sb->AppendUOSInt(k);
						sb->AppendC(UTF8STRC(" Type: "));
						switch (dataBuff[16 + k])
						{
						case 0:
							sb->AppendC(UTF8STRC("Reserved"));
							break;
						case 1:
							sb->AppendC(UTF8STRC("Recovery"));
							break;
						case 2:
							sb->AppendC(UTF8STRC("Main"));
							break;
						case 3:
							sb->AppendC(UTF8STRC("NVRAM"));
							break;
						case 4:
							sb->AppendC(UTF8STRC("Config"));
							break;
						case 5:
							sb->AppendC(UTF8STRC("Diag Vault"));
							break;
						default:
							sb->AppendC(UTF8STRC("Undefined ("));
							sb->AppendU16(dataBuff[16 + k]);
							sb->AppendC(UTF8STRC(")"));
							break;
						}
						sb->AppendC(UTF8STRC("\r\n"));
						sb->AppendC(UTF8STRC("Region "));
						sb->AppendUOSInt(k);
						sb->AppendC(UTF8STRC(" Start Address: 0x"));
						sb->AppendHex32(ReadUInt32(&dataBuff[24 + k * 8]));
						sb->AppendC(UTF8STRC("\r\n"));
						sb->AppendC(UTF8STRC("Region "));
						sb->AppendUOSInt(k);
						sb->AppendC(UTF8STRC(" End Address: 0x"));
						sb->AppendHex32(ReadUInt32(&dataBuff[24 + k * 8 + 4]));
						sb->AppendC(UTF8STRC("\r\n"));
						
						k++;
					}
				}
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
/*		case 130:
			sb->AppendC(UTF8STRC("SMBIOS Type 130 - Apple Memory SPD Data\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));
			break;*/
		case 131:
			sb->AppendC(UTF8STRC("SMBIOS Type 131 - Apple Processor Type\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (dataBuff[1] >= 6)
			{
				sb->AppendC(UTF8STRC("Processor Type: "));
				sb->AppendI16(ReadInt16(&dataBuff[4]));
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		default:
			sb->AppendC(UTF8STRC("SMBIOS Type "));
			sb->AppendU16(dataBuff[0]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Length: "));
			sb->AppendU16(dataBuff[1]);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Handle: 0x"));
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Data:\r\n"));
			sb->AppendHexBuff(&dataBuff[4], (UOSInt)dataBuff[1] - 4, ' ', Text::LineBreakType::CRLF);
			sb->AppendC(UTF8STRC("\r\n"));
			l = 1;
			while (l < k)
			{
				sb->AppendC(UTF8STRC("Strings "));
				sb->AppendUOSInt(l);
				sb->AppendUTF8Char(':');
				sb->AppendUTF8Char(' ');
				sb->Append(carr[l]);
				sb->AppendC(UTF8STRC("\r\n"));
				l++;
			}
			sb->AppendC(UTF8STRC("\r\n"));
			break;
		}

		i += buff[i + 1];
		while (i < j && (buff[i] != 0 || buff[i + 1] != 0))
		{
			i++;
		}
		i += 2;
	}
	return true;
}

void Win32::SMBIOS::GetProcessorFamily(Text::StringBuilderUTF8 *sb, Int32 family)
{
	switch (family)
	{
	case 1:
		sb->AppendC(UTF8STRC("Other"));
		break;
	case 2:
		sb->AppendC(UTF8STRC("Unknwon"));
		break;
	case 3:
		sb->AppendC(UTF8STRC("8086"));
		break;
	case 4:
		sb->AppendC(UTF8STRC("80286"));
		break;
	case 5:
		sb->AppendC(UTF8STRC("Intel386 processor"));
		break;
	case 6:
		sb->AppendC(UTF8STRC("Intel486 processor"));
		break;
	case 7:
		sb->AppendC(UTF8STRC("8087"));
		break;
	case 8:
		sb->AppendC(UTF8STRC("80287"));
		break;
	case 9:
		sb->AppendC(UTF8STRC("80387"));
		break;
	case 10:
		sb->AppendC(UTF8STRC("80487"));
		break;
	case 11:
		sb->AppendC(UTF8STRC("Intel Pentium processor"));
		break;
	case 12:
		sb->AppendC(UTF8STRC("Pentium Pro processor"));
		break;
	case 13:
		sb->AppendC(UTF8STRC("Pentium II processor"));
		break;
	case 14:
		sb->AppendC(UTF8STRC("Pentium processor with MMX technology"));
		break;
	case 15:
		sb->AppendC(UTF8STRC("Intel Celeron processor"));
		break;
	case 16:
		sb->AppendC(UTF8STRC("Pentium II Xeon processor"));
		break;
	case 17:
		sb->AppendC(UTF8STRC("Pentium III processor"));
		break;
	case 18:
		sb->AppendC(UTF8STRC("M1 Family"));
		break;
	case 19:
		sb->AppendC(UTF8STRC("M2 Family"));
		break;
	case 20:
		sb->AppendC(UTF8STRC("Intel Celeron M processor"));
		break;
	case 21:
		sb->AppendC(UTF8STRC("Intel Pentium 4 HT processor"));
		break;
	case 24:
		sb->AppendC(UTF8STRC("AMD Duron Processor Family"));
		break;
	case 25:
		sb->AppendC(UTF8STRC("K5 Family"));
		break;
	case 26:
		sb->AppendC(UTF8STRC("K6 Family"));
		break;
	case 27:
		sb->AppendC(UTF8STRC("K6-2"));
		break;
	case 28:
		sb->AppendC(UTF8STRC("K6-3"));
		break;
	case 29:
		sb->AppendC(UTF8STRC("AMD Athlon Processor Family"));
		break;
	case 30:
		sb->AppendC(UTF8STRC("AMD29000 Family"));
		break;
	case 31:
		sb->AppendC(UTF8STRC("K6-2+"));
		break;
	case 32:
		sb->AppendC(UTF8STRC("Power PC Family"));
		break;
	case 33:
		sb->AppendC(UTF8STRC("Power PC 601"));
		break;
	case 34:
		sb->AppendC(UTF8STRC("Power PC 603"));
		break;
	case 35:
		sb->AppendC(UTF8STRC("Power PC 603+"));
		break;
	case 36:
		sb->AppendC(UTF8STRC("Power PC 604"));
		break;
	case 37:
		sb->AppendC(UTF8STRC("Power PC 620"));
		break;
	case 38:
		sb->AppendC(UTF8STRC("Power PC x704"));
		break;
	case 39:
		sb->AppendC(UTF8STRC("Power PC 750"));
		break;
	case 40:
		sb->AppendC(UTF8STRC("Intel Core Duo processor"));
		break;
	case 41:
		sb->AppendC(UTF8STRC("Intel Core Duo mobile processor"));
		break;
	case 42:
		sb->AppendC(UTF8STRC("Intel Core Solo mobile processor"));
		break;
	case 43:
		sb->AppendC(UTF8STRC("Intel Atom processor"));
		break;
	case 44:
		sb->AppendC(UTF8STRC("Intel Core M processor"));
		break;
	case 45:
		sb->AppendC(UTF8STRC("Intel Core m3 processor"));
		break;
	case 46:
		sb->AppendC(UTF8STRC("Intel Core m5 processor"));
		break;
	case 47:
		sb->AppendC(UTF8STRC("Intel Core m7 processor"));
		break;
	case 48:
		sb->AppendC(UTF8STRC("Alpha Family"));
		break;
	case 49:
		sb->AppendC(UTF8STRC("Alpha 21064"));
		break;
	case 50:
		sb->AppendC(UTF8STRC("Alpha 21066"));
		break;
	case 51:
		sb->AppendC(UTF8STRC("Alpha 21164"));
		break;
	case 52:
		sb->AppendC(UTF8STRC("Alpha 21164PC"));
		break;
	case 53:
		sb->AppendC(UTF8STRC("Alpha 21164a"));
		break;
	case 54:
		sb->AppendC(UTF8STRC("Alpha 21264"));
		break;
	case 55:
		sb->AppendC(UTF8STRC("Alpha 21364"));
		break;
	case 56:
		sb->AppendC(UTF8STRC("AMD Turion II Ultra Dual-Core Mobile M Processor Family"));
		break;
	case 57:
		sb->AppendC(UTF8STRC("AMD Turion II Dual-Core Mobile M Processor Family"));
		break;
	case 58:
		sb->AppendC(UTF8STRC("AMD Athlon II Dual-Core M Processor Family"));
		break;
	case 59:
		sb->AppendC(UTF8STRC("AMD Opteron 6100 Series Processor"));
		break;
	case 60:
		sb->AppendC(UTF8STRC("AMD Opteron 4100 Series Processor"));
		break;
	case 61:
		sb->AppendC(UTF8STRC("AMD Opteron 6200 Series Processor"));
		break;
	case 62:
		sb->AppendC(UTF8STRC("AMD Opteron 4200 Series Processor"));
		break;
	case 63:
		sb->AppendC(UTF8STRC("AMD FX Series Processor"));
		break;
	case 64:
		sb->AppendC(UTF8STRC("MIPS Family"));
		break;
	case 65:
		sb->AppendC(UTF8STRC("MIPS R4000"));
		break;
	case 66:
		sb->AppendC(UTF8STRC("MIPS R4200"));
		break;
	case 67:
		sb->AppendC(UTF8STRC("MIPS R4400"));
		break;
	case 68:
		sb->AppendC(UTF8STRC("MIPS R4600"));
		break;
	case 69:
		sb->AppendC(UTF8STRC("MIPS R10000"));
		break;
	case 70:
		sb->AppendC(UTF8STRC("AMD C-Series Processor"));
		break;
	case 71:
		sb->AppendC(UTF8STRC("AMD E-Series Processor"));
		break;
	case 72:
		sb->AppendC(UTF8STRC("AMD A-Series Processor"));
		break;
	case 73:
		sb->AppendC(UTF8STRC("AMD G-Series Processor"));
		break;
	case 74:
		sb->AppendC(UTF8STRC("AMD Z-Series Processor"));
		break;
	case 75:
		sb->AppendC(UTF8STRC("AMD R-Series Processor"));
		break;
	case 76:
		sb->AppendC(UTF8STRC("AMD Opteron 4300 Series Processor"));
		break;
	case 77:
		sb->AppendC(UTF8STRC("AMD Opteron 6300 Series Processor"));
		break;
	case 78:
		sb->AppendC(UTF8STRC("AMD Opteron 3300 Series Processor"));
		break;
	case 79:
		sb->AppendC(UTF8STRC("AMD FirePro Series Processor"));
		break;
	case 80:
		sb->AppendC(UTF8STRC("SPARC Family"));
		break;
	case 81:
		sb->AppendC(UTF8STRC("SuperSPARC"));
		break;
	case 82:
		sb->AppendC(UTF8STRC("microSPARC II"));
		break;
	case 83:
		sb->AppendC(UTF8STRC("microSPARC IIep"));
		break;
	case 84:
		sb->AppendC(UTF8STRC("UltraSPARC"));
		break;
	case 85:
		sb->AppendC(UTF8STRC("UltraSPARC II"));
		break;
	case 86:
		sb->AppendC(UTF8STRC("UltraSPARC Iii"));
		break;
	case 87:
		sb->AppendC(UTF8STRC("UltraSPARC III"));
		break;
	case 88:
		sb->AppendC(UTF8STRC("UltraSPARC IIIi"));
		break;
	case 96:
		sb->AppendC(UTF8STRC("68040 Family"));
		break;
	case 97:
		sb->AppendC(UTF8STRC("68xxx"));
		break;
	case 98:
		sb->AppendC(UTF8STRC("68000"));
		break;
	case 99:
		sb->AppendC(UTF8STRC("68010"));
		break;
	case 100:
		sb->AppendC(UTF8STRC("68020"));
		break;
	case 101:
		sb->AppendC(UTF8STRC("68030"));
		break;
	case 102:
		sb->AppendC(UTF8STRC("AMD Athlon X4 Quad-Core Processor Family"));
		break;
	case 103:
		sb->AppendC(UTF8STRC("AMD Opteron X1000 Series Processor"));
		break;
	case 104:
		sb->AppendC(UTF8STRC("AMD Opteron X2000 Series Processor"));
		break;
	case 105:
		sb->AppendC(UTF8STRC("AMD Opteron A-Series Processor"));
		break;
	case 106:
		sb->AppendC(UTF8STRC("AMD Opteron X3000 Series APU"));
		break;
	case 107:
		sb->AppendC(UTF8STRC("AMD Zen Processor Family"));
		break;
	case 112:
		sb->AppendC(UTF8STRC("Hobbit Family"));
		break;
	case 120:
		sb->AppendC(UTF8STRC("Crusoe TM5000 Family"));
		break;
	case 121:
		sb->AppendC(UTF8STRC("Crusoe TM3000 Family"));
		break;
	case 122:
		sb->AppendC(UTF8STRC("Efficeon TM8000 Family"));
		break;
	case 128:
		sb->AppendC(UTF8STRC("Weitek"));
		break;
	case 130:
		sb->AppendC(UTF8STRC("Itanium processor"));
		break;
	case 131:
		sb->AppendC(UTF8STRC("AMD Athlon 64 Processor Family"));
		break;
	case 132:
		sb->AppendC(UTF8STRC("AMD Opteron Processor Family"));
		break;
	case 133:
		sb->AppendC(UTF8STRC("AMD Sempron Processor Family"));
		break;
	case 134:
		sb->AppendC(UTF8STRC("AMD Turion 64 Mobile Technology"));
		break;
	case 135:
		sb->AppendC(UTF8STRC("Dual-Core AMD Operton Processor Family"));
		break;
	case 136:
		sb->AppendC(UTF8STRC("AMD Athlon 64 X2 Dual-Core Processor Family"));
		break;
	case 137:
		sb->AppendC(UTF8STRC("AMD Turion 64 X2 Mobile Technology"));
		break;
	case 138:
		sb->AppendC(UTF8STRC("Quad-Core AMD Opteron Processor Family"));
		break;
	case 139:
		sb->AppendC(UTF8STRC("Third-Generation AMD Opteron Processor Family"));
		break;
	case 140:
		sb->AppendC(UTF8STRC("AMD Phenom FX Quad-Core Processor Family"));
		break;
	case 141:
		sb->AppendC(UTF8STRC("AMD Phenom X4 Quad-Core Processor Family"));
		break;
	case 142:
		sb->AppendC(UTF8STRC("AMD Phenom X2 Dual-Core Processor Family"));
		break;
	case 143:
		sb->AppendC(UTF8STRC("AMD Athlon X2 Dual-Core Processor Family"));
		break;
	case 144:
		sb->AppendC(UTF8STRC("PA-RISC Family"));
		break;
	case 145:
		sb->AppendC(UTF8STRC("PA-RISC 8500"));
		break;
	case 146:
		sb->AppendC(UTF8STRC("PA-RISC 8000"));
		break;
	case 147:
		sb->AppendC(UTF8STRC("PA-RISC 7300LC"));
		break;
	case 148:
		sb->AppendC(UTF8STRC("PA-RISC 7200"));
		break;
	case 149:
		sb->AppendC(UTF8STRC("PA-RISC 7100LC"));
		break;
	case 150:
		sb->AppendC(UTF8STRC("PA-RISC 7100"));
		break;
	case 160:
		sb->AppendC(UTF8STRC("V30 Family"));
		break;
	case 161:
		sb->AppendC(UTF8STRC("Quad-Core Intel Xeon processor 3200 Series"));
		break;
	case 162:
		sb->AppendC(UTF8STRC("Dual-Core Intel Xeon processor 3000 Series"));
		break;
	case 163:
		sb->AppendC(UTF8STRC("Quad-Core Intel Xeon processor 5300 Series"));
		break;
	case 164:
		sb->AppendC(UTF8STRC("Dual-Core Intel Xeon processor 5100 Series"));
		break;
	case 165:
		sb->AppendC(UTF8STRC("Dual-Core Intel Xeon processor 5000 Series"));
		break;
	case 166:
		sb->AppendC(UTF8STRC("Dual-Core Intel Xeon processor LV"));
		break;
	case 167:
		sb->AppendC(UTF8STRC("Dual-Core Intel Xeon processor ULV"));
		break;
	case 168:
		sb->AppendC(UTF8STRC("Dual-Core Intel Xeon processor 7100 Series"));
		break;
	case 169:
		sb->AppendC(UTF8STRC("Quad-Core Intel Xeon processor 5400 Series"));
		break;
	case 170:
		sb->AppendC(UTF8STRC("Quad-Core Intel Xeon processor"));
		break;
	case 171:
		sb->AppendC(UTF8STRC("Dual-Core Intel Xeon processor 5200 Series"));
		break;
	case 172:
		sb->AppendC(UTF8STRC("Dual-Core Intel Xeon processor 7200 Series"));
		break;
	case 173:
		sb->AppendC(UTF8STRC("Quad-Core Intel Xeon processor 7300 Series"));
		break;
	case 174:
		sb->AppendC(UTF8STRC("Quad-Core Intel Xeon processor 7400 Series"));
		break;
	case 175:
		sb->AppendC(UTF8STRC("Multi-Core Intel Xeon processor 7400 Series"));
		break;
	case 176:
		sb->AppendC(UTF8STRC("Pentium III Xeon processor"));
		break;
	case 177:
		sb->AppendC(UTF8STRC("Pentium III Processor with Intel SpeedStep Technology"));
		break;
	case 178:
		sb->AppendC(UTF8STRC("Pentium 4 Processor"));
		break;
	case 179:
		sb->AppendC(UTF8STRC("Intel Xeon processor"));
		break;
	case 180:
		sb->AppendC(UTF8STRC("AS400 Family"));
		break;
	case 181:
		sb->AppendC(UTF8STRC("Intel Xeon processor MP"));
		break;
	case 182:
		sb->AppendC(UTF8STRC("AMD Athlon XP Processor Family"));
		break;
	case 183:
		sb->AppendC(UTF8STRC("AMD Athlon MP Processor Family"));
		break;
	case 184:
		sb->AppendC(UTF8STRC("Intel Itanium 2 processor"));
		break;
	case 185:
		sb->AppendC(UTF8STRC("Intel Pentium M processor"));
		break;
	case 186:
		sb->AppendC(UTF8STRC("Intel Celeron D processor"));
		break;
	case 187:
		sb->AppendC(UTF8STRC("Intel Pentium D processor"));
		break;
	case 188:
		sb->AppendC(UTF8STRC("Intel Pentium Processor Extreme Edition"));
		break;
	case 189:
		sb->AppendC(UTF8STRC("Intel Core Solo processor"));
		break;
	case 191:
		sb->AppendC(UTF8STRC("Intel Core 2 Duo processor"));
		break;
	case 192:
		sb->AppendC(UTF8STRC("Intel Core 2 Solo processor"));
		break;
	case 193:
		sb->AppendC(UTF8STRC("Intel Core 2 Extreme processor"));
		break;
	case 194:
		sb->AppendC(UTF8STRC("Intel Core 2 Quad processor"));
		break;
	case 195:
		sb->AppendC(UTF8STRC("Intel Core 2 Extreme mobile processor"));
		break;
	case 196:
		sb->AppendC(UTF8STRC("Intel Core 2 Duo mobile processor"));
		break;
	case 197:
		sb->AppendC(UTF8STRC("Intel Core 2 Solo mobile processor"));
		break;
	case 198:
		sb->AppendC(UTF8STRC("Intel Core i7 processor"));
		break;
	case 199:
		sb->AppendC(UTF8STRC("Dual-Core Intel Celeron processor"));
		break;
	case 200:
		sb->AppendC(UTF8STRC("IBM390 Family"));
		break;
	case 201:
		sb->AppendC(UTF8STRC("G4"));
		break;
	case 202:
		sb->AppendC(UTF8STRC("G5"));
		break;
	case 203:
		sb->AppendC(UTF8STRC("ESA/390 G6"));
		break;
	case 204:
		sb->AppendC(UTF8STRC("z/Architecture base"));
		break;
	case 205:
		sb->AppendC(UTF8STRC("Intel Core i5 processor"));
		break;
	case 206:
		sb->AppendC(UTF8STRC("Intel Core i3 processor"));
		break;
	case 210:
		sb->AppendC(UTF8STRC("VIA C7-M Processor Family"));
		break;
	case 211:
		sb->AppendC(UTF8STRC("VIA C7-D Processor Family"));
		break;
	case 212:
		sb->AppendC(UTF8STRC("VIA C7 Processor Family"));
		break;
	case 213:
		sb->AppendC(UTF8STRC("VIA Eden Processor Family"));
		break;
	case 214:
		sb->AppendC(UTF8STRC("Multi-Core Intel Xeon processor"));
		break;
	case 215:
		sb->AppendC(UTF8STRC("Dual-Core Intel Xeon processor 3xxx Series"));
		break;
	case 216:
		sb->AppendC(UTF8STRC("Quad-Core Intel Xeon processor 3xxx Series"));
		break;
	case 217:
		sb->AppendC(UTF8STRC("VIA Nano Processor Family"));
		break;
	case 218:
		sb->AppendC(UTF8STRC("Dual-Core Intel Xeon processor 5xxx Series"));
		break;
	case 219:
		sb->AppendC(UTF8STRC("Quad-Core Intel Xeon processor 5xxx Series"));
		break;
	case 221:
		sb->AppendC(UTF8STRC("Dual-Core Intel Xeon processor 7xxx Series"));
		break;
	case 222:
		sb->AppendC(UTF8STRC("Quad-Core Intel Xeon processor 7xxx Series"));
		break;
	case 223:
		sb->AppendC(UTF8STRC("Multi-Core Intel Xeon processor 7xxx Series"));
		break;
	case 224:
		sb->AppendC(UTF8STRC("Multi-Core Intel Xeon processor 3400 Series"));
		break;
	case 228:
		sb->AppendC(UTF8STRC("AMD Opteron 3000 Series Processor"));
		break;
	case 229:
		sb->AppendC(UTF8STRC("AMD Sempron II Processor"));
		break;
	case 230:
		sb->AppendC(UTF8STRC("Embedded AMD Opteron Quad-Core Processor Family"));
		break;
	case 231:
		sb->AppendC(UTF8STRC("AMD Phenom Triple-Core Processor Family"));
		break;
	case 232:
		sb->AppendC(UTF8STRC("AMD Turion Ultra Dual-Core Mobile Processor Family"));
		break;
	case 233:
		sb->AppendC(UTF8STRC("AMD Turion Dual-Core Mobile Processor Family"));
		break;
	case 234:
		sb->AppendC(UTF8STRC("AMD Athlon Dual-Core Processor Family"));
		break;
	case 235:
		sb->AppendC(UTF8STRC("AMD Sempron SI Processor Family"));
		break;
	case 236:
		sb->AppendC(UTF8STRC("AMD Phenom II Processor Family"));
		break;
	case 237:
		sb->AppendC(UTF8STRC("AMD Athlon II Processor Family"));
		break;
	case 238:
		sb->AppendC(UTF8STRC("Six-Core AMD Opteron Processor Family"));
		break;
	case 239:
		sb->AppendC(UTF8STRC("AMD Sempron M Processor Family"));
		break;
	case 250:
		sb->AppendC(UTF8STRC("i860"));
		break;
	case 251:
		sb->AppendC(UTF8STRC("i960"));
		break;
	case 254:
		sb->AppendC(UTF8STRC("See Processor Family 2"));
		break;
	case 256:
		sb->AppendC(UTF8STRC("ARMv7"));
		break;
	case 257:
		sb->AppendC(UTF8STRC("ARMv8"));
		break;
	case 260:
		sb->AppendC(UTF8STRC("SH-3"));
		break;
	case 261:
		sb->AppendC(UTF8STRC("SH-4"));
		break;
	case 280:
		sb->AppendC(UTF8STRC("ARM"));
		break;
	case 281:
		sb->AppendC(UTF8STRC("StrongARM"));
		break;
	case 300:
		sb->AppendC(UTF8STRC("6x86"));
		break;
	case 301:
		sb->AppendC(UTF8STRC("MediaGX"));
		break;
	case 302:
		sb->AppendC(UTF8STRC("MII"));
		break;
	case 320:
		sb->AppendC(UTF8STRC("WinChip"));
		break;
	case 350:
		sb->AppendC(UTF8STRC("DSP"));
		break;
	case 500:
		sb->AppendC(UTF8STRC("VideoProcessor"));
		break;
	default:
		sb->AppendC(UTF8STRC("Undefined ("));
		sb->AppendI32(family);
		sb->AppendC(UTF8STRC(")"));
	}
}

void Win32::SMBIOS::GetConnectorType(Text::StringBuilderUTF8 *sb, UInt8 type)
{
	switch (type)
	{
	case 0:
		sb->AppendC(UTF8STRC("None"));
		break;
	case 1:
		sb->AppendC(UTF8STRC("Centronics"));
		break;
	case 2:
		sb->AppendC(UTF8STRC("Mini Centronics"));
		break;
	case 3:
		sb->AppendC(UTF8STRC("Proprietary"));
		break;
	case 4:
		sb->AppendC(UTF8STRC("DB-25 pin male"));
		break;
	case 5:
		sb->AppendC(UTF8STRC("DB-25 pin female"));
		break;
	case 6:
		sb->AppendC(UTF8STRC("DB-15 pin male"));
		break;
	case 7:
		sb->AppendC(UTF8STRC("DB-15 pin female"));
		break;
	case 8:
		sb->AppendC(UTF8STRC("DB-9 pin male"));
		break;
	case 9:
		sb->AppendC(UTF8STRC("DB-9 pin female"));
		break;
	case 10:
		sb->AppendC(UTF8STRC("RJ-11"));
		break;
	case 11:
		sb->AppendC(UTF8STRC("RJ-45"));
		break;
	case 12:
		sb->AppendC(UTF8STRC("50-pin MiniSCSI"));
		break;
	case 13:
		sb->AppendC(UTF8STRC("Mini-DIN"));
		break;
	case 14:
		sb->AppendC(UTF8STRC("Micro-DIN"));
		break;
	case 15:
		sb->AppendC(UTF8STRC("PS/2"));
		break;
	case 16:
		sb->AppendC(UTF8STRC("Infrared"));
		break;
	case 17:
		sb->AppendC(UTF8STRC("HP-HIL"));
		break;
	case 18:
		sb->AppendC(UTF8STRC("Access Bus (USB)"));
		break;
	case 19:
		sb->AppendC(UTF8STRC("SSA SCSI"));
		break;
	case 20:
		sb->AppendC(UTF8STRC("Circular DIN-8 male"));
		break;
	case 21:
		sb->AppendC(UTF8STRC("Circular DIN-8 female"));
		break;
	case 22:
		sb->AppendC(UTF8STRC("On Board IDE"));
		break;
	case 23:
		sb->AppendC(UTF8STRC("On Board Floppy"));
		break;
	case 24:
		sb->AppendC(UTF8STRC("9-pin Dual Inline (pin 10 cut)"));
		break;
	case 25:
		sb->AppendC(UTF8STRC("25-pin Dual Inline (pin 26 cut)"));
		break;
	case 26:
		sb->AppendC(UTF8STRC("50-pin Dual Inline"));
		break;
	case 27:
		sb->AppendC(UTF8STRC("68-pin Dual Inline"));
		break;
	case 28:
		sb->AppendC(UTF8STRC("On Board Sound Input from CD-ROM"));
		break;
	case 29:
		sb->AppendC(UTF8STRC("Mini-Centronics Type-14"));
		break;
	case 30:
		sb->AppendC(UTF8STRC("Mini-Centronics Type-26"));
		break;
	case 31:
		sb->AppendC(UTF8STRC("Mini-jack (headphones)"));
		break;
	case 32:
		sb->AppendC(UTF8STRC("BNC"));
		break;
	case 33:
		sb->AppendC(UTF8STRC("1394"));
		break;
	case 34:
		sb->AppendC(UTF8STRC("SAS/SATA Plug Receptacle"));
		break;
	case 160:
		sb->AppendC(UTF8STRC("PC-98"));
		break;
	case 161:
		sb->AppendC(UTF8STRC("PC-98Hireso"));
		break;
	case 162:
		sb->AppendC(UTF8STRC("PC-H98"));
		break;
	case 163:
		sb->AppendC(UTF8STRC("PC-98Note"));
		break;
	case 164:
		sb->AppendC(UTF8STRC("PC-98Full"));
		break;
	case 255:
		sb->AppendC(UTF8STRC("Other"));
		break;
	default:
		sb->AppendC(UTF8STRC("Undefined ("));
		sb->AppendU16(type);
		sb->AppendC(UTF8STRC(")"));
		break;
	}
}
