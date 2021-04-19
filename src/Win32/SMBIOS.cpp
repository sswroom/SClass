#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Win32/SMBIOS.h"

void Win32::SMBIOS::GetDataType(Data::ArrayList<const UInt8 *> *dataList, UInt8 dataType)
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

UOSInt Win32::SMBIOS::GetMemoryInfo(Data::ArrayList<MemoryDeviceInfo*> *memList)
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
				mem->memorySize = (ReadUInt32(&dataBuff[28]) & 0x7fffffff) * 1048576LL;
			}
			else if (sizeVal & 0x8000)
			{
				mem->memorySize = (sizeVal & 0x7fff) * 1024LL;
			}
			else
			{
				mem->memorySize = sizeVal * 1048576LL;
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

void Win32::SMBIOS::FreeMemoryInfo(Data::ArrayList<MemoryDeviceInfo*> *memList)
{
	UOSInt i = memList->GetCount();
	while (i-- > 0)
	{
		MemFree(memList->GetItem(i));
	}
	memList->Clear();
}

UTF8Char *Win32::SMBIOS::GetPlatformName(UTF8Char *buff)
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

UTF8Char *Win32::SMBIOS::GetPlatformSN(UTF8Char *buff)
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

Int32 Win32::SMBIOS::GetChassisType()
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

Bool Win32::SMBIOS::ToString(Text::StringBuilderUTF *sb)
{
	const Char *carr[32];
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
			carr[k] = 0;
		}
		dataBuff = &buff[i];
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

		switch (dataBuff[0]) //smbios type
		{
		case 0:
			sb->Append((const UTF8Char*)"SMBIOS Type 0 - BIOS Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Vendor: ");
			if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"BIOS Version: ");
			if (carr[dataBuff[5]]) sb->Append((const UTF8Char*)carr[dataBuff[5]]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"BIOS Starting Address Segment: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[6]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"BIOS Release Date: ");
			if (carr[dataBuff[8]]) sb->Append((const UTF8Char*)carr[dataBuff[8]]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"BIOS ROM Size: ");
			if (dataBuff[9] == 255)
			{
				sb->Append((const UTF8Char*)"16MB or greater");
			}
			else
			{
				sb->AppendU32(65536 * (UInt32)(dataBuff[9] + 1));
			}
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"BIOS Characteristics: 0x");
			sb->AppendHex64(ReadUInt64(&dataBuff[10]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 19)
			{
				sb->Append((const UTF8Char*)"BIOS Characteristics Extension Byte 1: 0x");
				sb->AppendHex8(dataBuff[18]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 20)
			{
				sb->Append((const UTF8Char*)"BIOS Characteristics Extension Byte 2: 0x");
				sb->AppendHex8(dataBuff[19]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 22)
			{
				sb->Append((const UTF8Char*)"System BIOS Revision: ");
				sb->AppendU16(dataBuff[20]);
				sb->Append((const UTF8Char*)".");
				sb->AppendU16(dataBuff[21]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 24)
			{
				sb->Append((const UTF8Char*)"Embedded Controller Firmware: ");
				sb->AppendU16(dataBuff[22]);
				sb->Append((const UTF8Char*)".");
				sb->AppendU16(dataBuff[23]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 26)
			{
				sb->Append((const UTF8Char*)"Extended BIOS ROM Size: ");
				sb->AppendI16(ReadInt16(&dataBuff[24]) & 0x3fff);
				if ((dataBuff[25] & 0xc0) == 0)
				{
					sb->Append((const UTF8Char*)"MB");
				}
				else if ((dataBuff[25] & 0xc0) == 1)
				{
					sb->Append((const UTF8Char*)"GB");
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 1:
			sb->Append((const UTF8Char*)"SMBIOS Type 1 - System Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Manufacturer: ");
			if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Product Name: ");
			if (carr[dataBuff[5]]) sb->Append((const UTF8Char*)carr[dataBuff[5]]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Version: ");
			if (carr[dataBuff[6]]) sb->Append((const UTF8Char*)carr[dataBuff[6]]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Serial Number: ");
			if (carr[dataBuff[7]]) sb->Append((const UTF8Char*)carr[dataBuff[7]]);
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 25)
			{
				sb->Append((const UTF8Char*)"UUID: {");
				sb->AppendHex32(ReadUInt32(&dataBuff[8]));
				sb->Append((const UTF8Char*)"-");
				sb->AppendHex16(ReadUInt16(&dataBuff[12]));
				sb->Append((const UTF8Char*)"-");
				sb->AppendHex16(ReadUInt16(&dataBuff[14]));
				sb->Append((const UTF8Char*)"-");
				sb->AppendHex16(ReadMUInt16(&dataBuff[16]));
				sb->Append((const UTF8Char*)"-");
				sb->AppendHex32(ReadMUInt32(&dataBuff[18]));
				sb->AppendHex16(ReadMUInt16(&dataBuff[22]));
				sb->Append((const UTF8Char*)"}\r\n");
				sb->Append((const UTF8Char*)"Wake-up Type: ");
				switch (dataBuff[24])
				{
				case 0:
					sb->Append((const UTF8Char*)"Reserved");
					break;
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"APM Timer");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Modem Ring");
					break;
				case 5:
					sb->Append((const UTF8Char*)"LAN Remote");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Power Switch");
					break;
				case 7:
					sb->Append((const UTF8Char*)"PCI PME#");
					break;
				case 8:
					sb->Append((const UTF8Char*)"AC Power Restored");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[24]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 27)
			{
				sb->Append((const UTF8Char*)"SKU Number: ");
				if (carr[dataBuff[25]]) sb->Append((const UTF8Char*)carr[dataBuff[25]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Family: ");
				if (carr[dataBuff[26]]) sb->Append((const UTF8Char*)carr[dataBuff[26]]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 2:
			sb->Append((const UTF8Char*)"SMBIOS Type 2 - Baseboard Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 8)
			{
				sb->Append((const UTF8Char*)"Manufacturer: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Product: ");
				if (carr[dataBuff[5]]) sb->Append((const UTF8Char*)carr[dataBuff[5]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Version: ");
				if (carr[dataBuff[6]]) sb->Append((const UTF8Char*)carr[dataBuff[6]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Serial Number: ");
				if (carr[dataBuff[7]]) sb->Append((const UTF8Char*)carr[dataBuff[7]]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 9)
			{
				sb->Append((const UTF8Char*)"Asset Tag: ");
				if (carr[dataBuff[8]]) sb->Append((const UTF8Char*)carr[dataBuff[8]]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 10)
			{
				sb->Append((const UTF8Char*)"Feature Flags: 0x");
				sb->AppendHex8(dataBuff[9]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 11)
			{
				sb->Append((const UTF8Char*)"Location in Chassis: ");
				if (carr[dataBuff[10]]) sb->Append((const UTF8Char*)carr[dataBuff[10]]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 13)
			{
				sb->Append((const UTF8Char*)"Chassis Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[11]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 14)
			{
				sb->Append((const UTF8Char*)"Board Type: ");
				switch (dataBuff[13])
				{
				case 1:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Server Blade");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Connectivity Switch");
					break;
				case 5:
					sb->Append((const UTF8Char*)"System Management Module");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Processor Module");
					break;
				case 7:
					sb->Append((const UTF8Char*)"I/O Module");
					break;
				case 8:
					sb->Append((const UTF8Char*)"Memory Module");
					break;
				case 9:
					sb->Append((const UTF8Char*)"Daughter board");
					break;
				case 10:
					sb->Append((const UTF8Char*)"Motherboard");
					break;
				case 11:
					sb->Append((const UTF8Char*)"Processor/Memory Module");
					break;
				case 12:
					sb->Append((const UTF8Char*)"Processor/IO Module");
					break;
				case 13:
					sb->Append((const UTF8Char*)"Interconnect board");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[13]);
					sb->Append((const UTF8Char*)")");
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 3:
			sb->Append((const UTF8Char*)"SMBIOS Type 3 - System Enclosure\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 9)
			{
				sb->Append((const UTF8Char*)"Manufacturer: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Chassis Lock Present: ");
				sb->Append((dataBuff[5] & 0x80)?(const UTF8Char*)"Yes":(const UTF8Char*)"No");
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Type: ");
				switch (dataBuff[5] & 0x7f)
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknwon");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Desktop");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Low Profile Desktop");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Pizza Box");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Mini Tower");
					break;
				case 7:
					sb->Append((const UTF8Char*)"Tower");
					break;
				case 8:
					sb->Append((const UTF8Char*)"Portable");
					break;
				case 9:
					sb->Append((const UTF8Char*)"Laptop");
					break;
				case 10:
					sb->Append((const UTF8Char*)"Notebook");
					break;
				case 11:
					sb->Append((const UTF8Char*)"Hand Held");
					break;
				case 12:
					sb->Append((const UTF8Char*)"Docking Station");
					break;
				case 13:
					sb->Append((const UTF8Char*)"All in One");
					break;
				case 14:
					sb->Append((const UTF8Char*)"Sub Notebook");
					break;
				case 15:
					sb->Append((const UTF8Char*)"Space-saving");
					break;
				case 16:
					sb->Append((const UTF8Char*)"Lunch Box");
					break;
				case 17:
					sb->Append((const UTF8Char*)"Main Server Chassis");
					break;
				case 18:
					sb->Append((const UTF8Char*)"Expansion Chassis");
					break;
				case 19:
					sb->Append((const UTF8Char*)"SubChassis");
					break;
				case 20:
					sb->Append((const UTF8Char*)"Bus Expansion Chassis");
					break;
				case 21:
					sb->Append((const UTF8Char*)"Peripheral Chassis");
					break;
				case 22:
					sb->Append((const UTF8Char*)"RAID Chassis");
					break;
				case 23:
					sb->Append((const UTF8Char*)"Rack Mount Chassis");
					break;
				case 24:
					sb->Append((const UTF8Char*)"Sealed-case PC");
					break;
				case 25:
					sb->Append((const UTF8Char*)"Multi-system chassis");
					break;
				case 26:
					sb->Append((const UTF8Char*)"Compact PCI");
					break;
				case 27:
					sb->Append((const UTF8Char*)"Advanced TCA");
					break;
				case 28:
					sb->Append((const UTF8Char*)"Blade");
					break;
				case 29:
					sb->Append((const UTF8Char*)"Blade Enclosure");
					break;
				case 30:
					sb->Append((const UTF8Char*)"Tablet");
					break;
				case 31:
					sb->Append((const UTF8Char*)"Convertible");
					break;
				case 32:
					sb->Append((const UTF8Char*)"Detachable");
					break;
				case 33:
					sb->Append((const UTF8Char*)"IoT Gateway");
					break;
				case 34:
					sb->Append((const UTF8Char*)"Embedded PC");
					break;
				case 35:
					sb->Append((const UTF8Char*)"Mini PC");
					break;
				case 36:
					sb->Append((const UTF8Char*)"Stick PC");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[5] & 0x7f);
					sb->Append((const UTF8Char*)")");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Version: ");
				if (carr[dataBuff[6]]) sb->Append((const UTF8Char*)carr[dataBuff[6]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Serial Number: ");
				if (carr[dataBuff[7]]) sb->Append((const UTF8Char*)carr[dataBuff[7]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Asset Tag Number: ");
				if (carr[dataBuff[8]]) sb->Append((const UTF8Char*)carr[dataBuff[8]]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 9)
			{
				sb->Append((const UTF8Char*)"Boot-up State: ");
				switch (dataBuff[9])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknwon");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Safe");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Warning");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Critical");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Non-recoverable");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[9]);
					sb->Append((const UTF8Char*)")");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Power Supply State: ");
				switch (dataBuff[10])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknwon");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Safe");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Warning");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Critical");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Non-recoverable");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[10]);
					sb->Append((const UTF8Char*)")");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Thermal State: ");
				switch (dataBuff[11])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknwon");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Safe");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Warning");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Critical");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Non-recoverable");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[11]);
					sb->Append((const UTF8Char*)")");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Security Status: ");
				switch (dataBuff[12])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknwon");
					break;
				case 3:
					sb->Append((const UTF8Char*)"None");
					break;
				case 4:
					sb->Append((const UTF8Char*)"External interface locked out");
					break;
				case 5:
					sb->Append((const UTF8Char*)"External interface enabled");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[12]);
					sb->Append((const UTF8Char*)")");
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 17)
			{
				sb->Append((const UTF8Char*)"OEM-defined: 0x");
				sb->AppendHex32(ReadUInt32(&dataBuff[13]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 18)
			{
				sb->Append((const UTF8Char*)"Height: ");
				sb->AppendU16(dataBuff[17]);
				sb->Append((const UTF8Char*)"U\r\n");
			}
			if (dataBuff[1] >= 19)
			{
				sb->Append((const UTF8Char*)"Number of Power Cords: ");
				sb->AppendU16(dataBuff[18]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 21)
			{
				Int32 n = dataBuff[19];
				Int32 m = dataBuff[20];
				sb->Append((const UTF8Char*)"Contained Element Count: ");
				sb->AppendU16(dataBuff[19]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Contained Element Record Length: ");
				sb->AppendU16(dataBuff[20]);
				sb->Append((const UTF8Char*)"\r\n");
				if (dataBuff[1] >= 22 + (n * m))
				{
					sb->Append((const UTF8Char*)"SKU Number: ");
					if (carr[dataBuff[21 + (n * m)]]) sb->Append((const UTF8Char*)carr[dataBuff[21 + (n * m)]]);
					sb->Append((const UTF8Char*)"\r\n");
				}
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 4:
			sb->Append((const UTF8Char*)"SMBIOS Type 4 - Processor Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 26)
			{
				sb->Append((const UTF8Char*)"Socket Designation: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Processor Type: ");
				switch (dataBuff[5])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknwon");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Central Processor");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Math Processor");
					break;
				case 5:
					sb->Append((const UTF8Char*)"DSP Processor");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Video Processor");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[5]);
					sb->Append((const UTF8Char*)")");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Processor Family: ");
				GetProcessorFamily(sb, dataBuff[6]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Processor Manufacturer: ");
				if (carr[dataBuff[7]]) sb->Append((const UTF8Char*)carr[dataBuff[7]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Processor ID: 0x");
				sb->AppendHex64(ReadUInt64(&dataBuff[8]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Processor Version: ");
				if (carr[dataBuff[16]]) sb->Append((const UTF8Char*)carr[dataBuff[16]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Voltage: ");
				if (dataBuff[17] & 0x80)
				{
					Text::SBAppendF64(sb, (dataBuff[17] & 0x7f) * 0.1);
					sb->Append((const UTF8Char*)"V");
				}
				else
				{
					if (dataBuff[17] & 1)
					{
						sb->Append((const UTF8Char*)" 5V");
					}
					if (dataBuff[17] & 2)
					{
						sb->Append((const UTF8Char*)" 3.3V");
					}
					if (dataBuff[17] & 4)
					{
						sb->Append((const UTF8Char*)" 2.9V");
					}
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"External Clock: ");
				sb->AppendI16(ReadInt16(&dataBuff[18]));
				sb->Append((const UTF8Char*)"MHz\r\n");
				sb->Append((const UTF8Char*)"Max Speed: ");
				sb->AppendI16(ReadInt16(&dataBuff[20]));
				sb->Append((const UTF8Char*)"MHz\r\n");
				sb->Append((const UTF8Char*)"Current Speed: ");
				sb->AppendI16(ReadInt16(&dataBuff[22]));
				sb->Append((const UTF8Char*)"MHz\r\n");
				sb->Append((const UTF8Char*)"Status: 0x");
				sb->AppendHex8(dataBuff[24]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Processor Upgrade: ");
				switch (dataBuff[25])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknwon");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Daughter Board");
					break;
				case 4:
					sb->Append((const UTF8Char*)"ZIF Socket");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Replaceable Piggy Back");
					break;
				case 6:
					sb->Append((const UTF8Char*)"None");
					break;
				case 7:
					sb->Append((const UTF8Char*)"LIF Socket");
					break;
				case 8:
					sb->Append((const UTF8Char*)"Slot 1");
					break;
				case 9:
					sb->Append((const UTF8Char*)"Slot 2");
					break;
				case 10:
					sb->Append((const UTF8Char*)"370-pin socket");
					break;
				case 11:
					sb->Append((const UTF8Char*)"Slot A");
					break;
				case 12:
					sb->Append((const UTF8Char*)"Slot M");
					break;
				case 13:
					sb->Append((const UTF8Char*)"Socket 423");
					break;
				case 14:
					sb->Append((const UTF8Char*)"Socket A (Socket 462)");
					break;
				case 15:
					sb->Append((const UTF8Char*)"Socket 478");
					break;
				case 16:
					sb->Append((const UTF8Char*)"Socket 754");
					break;
				case 17:
					sb->Append((const UTF8Char*)"Socket 940");
					break;
				case 18:
					sb->Append((const UTF8Char*)"Socket 939");
					break;
				case 19:
					sb->Append((const UTF8Char*)"Socket mPGA604");
					break;
				case 20:
					sb->Append((const UTF8Char*)"Socket LGA771");
					break;
				case 21:
					sb->Append((const UTF8Char*)"Socket LGA775");
					break;
				case 22:
					sb->Append((const UTF8Char*)"Socket S1");
					break;
				case 23:
					sb->Append((const UTF8Char*)"Socket AM2");
					break;
				case 24:
					sb->Append((const UTF8Char*)"Socket F (1207)");
					break;
				case 25:
					sb->Append((const UTF8Char*)"Socket LGA1366");
					break;
				case 26:
					sb->Append((const UTF8Char*)"Socket G34");
					break;
				case 27:
					sb->Append((const UTF8Char*)"Socket AM3");
					break;
				case 28:
					sb->Append((const UTF8Char*)"Socket C32");
					break;
				case 29:
					sb->Append((const UTF8Char*)"Socket LGA1156");
					break;
				case 30:
					sb->Append((const UTF8Char*)"Socket LGA1567");
					break;
				case 31:
					sb->Append((const UTF8Char*)"Socket PGA988A");
					break;
				case 32:
					sb->Append((const UTF8Char*)"Socket BGA1288");
					break;
				case 33:
					sb->Append((const UTF8Char*)"Socket rPGA988B");
					break;
				case 34:
					sb->Append((const UTF8Char*)"Socket BGA1023");
					break;
				case 35:
					sb->Append((const UTF8Char*)"Socket BGA1224");
					break;
				case 36:
					sb->Append((const UTF8Char*)"Socket LGA1155");
					break;
				case 37:
					sb->Append((const UTF8Char*)"Socket LGA1356");
					break;
				case 38:
					sb->Append((const UTF8Char*)"Socket LGA2011");
					break;
				case 39:
					sb->Append((const UTF8Char*)"Socket FS1");
					break;
				case 40:
					sb->Append((const UTF8Char*)"Socket FS2");
					break;
				case 41:
					sb->Append((const UTF8Char*)"Socket FM1");
					break;
				case 42:
					sb->Append((const UTF8Char*)"Socket FM2");
					break;
				case 43:
					sb->Append((const UTF8Char*)"Socket LGA2011-3");
					break;
				case 44:
					sb->Append((const UTF8Char*)"Socket LGA1356-3");
					break;
				case 45:
					sb->Append((const UTF8Char*)"Socket LGA1150");
					break;
				case 46:
					sb->Append((const UTF8Char*)"Socket BGA1168");
					break;
				case 47:
					sb->Append((const UTF8Char*)"Socket BGA1234");
					break;
				case 48:
					sb->Append((const UTF8Char*)"Socket BGA1364");
					break;
				case 49:
					sb->Append((const UTF8Char*)"Socket AM4");
					break;
				case 50:
					sb->Append((const UTF8Char*)"Socket LGA1151");
					break;
				case 51:
					sb->Append((const UTF8Char*)"Socket BGA1356");
					break;
				case 52:
					sb->Append((const UTF8Char*)"Socket BGA1440");
					break;
				case 53:
					sb->Append((const UTF8Char*)"Socket BGA1515");
					break;
				case 54:
					sb->Append((const UTF8Char*)"Socket LGA3647-1");
					break;
				case 55:
					sb->Append((const UTF8Char*)"Socket SP3");
					break;
				case 56:
					sb->Append((const UTF8Char*)"Socket SP3r2");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[25]);
					sb->Append((const UTF8Char*)")");
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 32)
			{
				sb->Append((const UTF8Char*)"L1 Cache Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[26]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"L2 Cache Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[28]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"L3 Cache Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[30]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 35)
			{
				sb->Append((const UTF8Char*)"Serial Number: ");
				if (carr[dataBuff[32]]) sb->Append((const UTF8Char*)carr[dataBuff[32]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Asset Tag: ");
				if (carr[dataBuff[33]]) sb->Append((const UTF8Char*)carr[dataBuff[33]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Part Number: ");
				if (carr[dataBuff[34]]) sb->Append((const UTF8Char*)carr[dataBuff[34]]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 40)
			{
				sb->Append((const UTF8Char*)"Core Count: ");
				sb->AppendU16(dataBuff[35]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Core Enabled: ");
				sb->AppendU16(dataBuff[36]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Thread Count: ");
				sb->AppendU16(dataBuff[37]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Processor Characteristics: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[38]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 48)
			{
				sb->Append((const UTF8Char*)"Processor Family 2: ");
				GetProcessorFamily(sb, ReadInt16(&dataBuff[40]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Core Count 2: ");
				sb->AppendI16(ReadInt16(&dataBuff[42]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Core Enabled 2: ");
				sb->AppendI16(ReadInt16(&dataBuff[44]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Thread Count 2: ");
				sb->AppendI16(ReadInt16(&dataBuff[46]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
/*		case 5:
			sb->Append((const UTF8Char*)"SMBIOS Type 5 - Memory Controller Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 15)
			{
				////////////////////////////////
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;*/
		case 6:
			sb->Append((const UTF8Char*)"SMBIOS Type 6 - Memory Module Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 12)
			{
				sb->Append((const UTF8Char*)"Socket Designation: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Bank Connections: ");
				sb->AppendU16(dataBuff[5]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Current Speed: ");
				sb->AppendU16(dataBuff[6]);
				sb->Append((const UTF8Char*)"ns\r\n");
				sb->Append((const UTF8Char*)"Current Memory Type: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[7]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Installed Size: ");
				sb->AppendU16(dataBuff[9]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Enabled Size: ");
				sb->AppendU16(dataBuff[10]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Error Status: 0x");
				sb->AppendHex8(dataBuff[11]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 7:
			sb->Append((const UTF8Char*)"SMBIOS Type 7 - Cache Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 15)
			{
				sb->Append((const UTF8Char*)"Socket Designation: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Cache Configuration: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[5]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Maximum Cache Size: ");
				if (dataBuff[8] & 0x80)
				{
					sb->AppendI32((ReadInt16(&dataBuff[7]) & 0x7fff) * 64);
					sb->Append((const UTF8Char*)"K");
				}
				else
				{
					sb->AppendI32(ReadInt16(&dataBuff[7]) & 0x7fff);
					sb->Append((const UTF8Char*)"K");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Installed Size: ");
				if (dataBuff[10] & 0x80)
				{
					sb->AppendI32((ReadInt16(&dataBuff[9]) & 0x7fff) * 64);
					sb->Append((const UTF8Char*)"K");
				}
				else
				{
					sb->AppendI32(ReadInt16(&dataBuff[9]) & 0x7fff);
					sb->Append((const UTF8Char*)"K");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Supported SRAM Type: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[11]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Current SRAM Type: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[13]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 19)
			{
				sb->Append((const UTF8Char*)"Cache Speed: ");
				sb->AppendU16(dataBuff[15]);
				sb->Append((const UTF8Char*)"ns\r\n");
				sb->Append((const UTF8Char*)"Error Correction Type: ");
				switch (dataBuff[16])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unkown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"None");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Parity");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Single-bit ECC");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Multi-bit ECC");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[16]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"System Cache Type: ");
				switch (dataBuff[16])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Instruction");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Data");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Unified");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[16]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Associativity: ");
				switch (dataBuff[16])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Direct Mapped");
					break;
				case 4:
					sb->Append((const UTF8Char*)"2-way Set-Associative");
					break;
				case 5:
					sb->Append((const UTF8Char*)"4-way Set-Associative");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Full Associative");
					break;
				case 7:
					sb->Append((const UTF8Char*)"8-way Set-Associative");
					break;
				case 8:
					sb->Append((const UTF8Char*)"16-way Set-Associative");
					break;
				case 9:
					sb->Append((const UTF8Char*)"12-way Set-Associative");
					break;
				case 10:
					sb->Append((const UTF8Char*)"24-way Set-Associative");
					break;
				case 11:
					sb->Append((const UTF8Char*)"32-way Set-Associative");
					break;
				case 12:
					sb->Append((const UTF8Char*)"48-way Set-Associative");
					break;
				case 13:
					sb->Append((const UTF8Char*)"64-way Set-Associative");
					break;
				case 14:
					sb->Append((const UTF8Char*)"20-way Set-Associative");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[16]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 27)
			{
				sb->Append((const UTF8Char*)"Maximum Cache Size 2: ");
				if (dataBuff[22] & 0x80)
				{
					sb->AppendI32((ReadInt32(&dataBuff[19]) & 0x7fffffff) * 64);
					sb->Append((const UTF8Char*)"K");
				}
				else
				{
					sb->AppendI32(ReadInt32(&dataBuff[19]) & 0x7fffffff);
					sb->Append((const UTF8Char*)"K");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Installed Cache Size 2: ");
				if (dataBuff[26] & 0x80)
				{
					sb->AppendI32((ReadInt32(&dataBuff[23]) & 0x7fffffff) * 64);
					sb->Append((const UTF8Char*)"K");
				}
				else
				{
					sb->AppendI32(ReadInt32(&dataBuff[23]) & 0x7fffffff);
					sb->Append((const UTF8Char*)"K");
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 8:
			sb->Append((const UTF8Char*)"SMBIOS Type 8 - Port Connector Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 9)
			{
				sb->Append((const UTF8Char*)"Internal Reference Designation: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Internal Connector Type: ");
				GetConnectorType(sb, dataBuff[5]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"External Reference Designation: ");
				if (carr[dataBuff[6]]) sb->Append((const UTF8Char*)carr[dataBuff[6]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"External Connector Type: ");
				GetConnectorType(sb, dataBuff[7]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Port Type: ");
				switch (dataBuff[8])
				{
				case 0:
					sb->Append((const UTF8Char*)"None");
					break;
				case 1:
					sb->Append((const UTF8Char*)"Parallel Port XT/AT Compatible");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Parallel Port PS/2");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Parallel Port ECP");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Parallel Port EPP");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Parallel Port ECP/EPP");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Serial Port XT/AT Compatible");
					break;
				case 7:
					sb->Append((const UTF8Char*)"Serial Port 16450 Compatible");
					break;
				case 8:
					sb->Append((const UTF8Char*)"Serial Port 16550 Compatible");
					break;
				case 9:
					sb->Append((const UTF8Char*)"Serial Port 16550A Compatible");
					break;
				case 10:
					sb->Append((const UTF8Char*)"SCSI Port");
					break;
				case 11:
					sb->Append((const UTF8Char*)"MIDI Port");
					break;
				case 12:
					sb->Append((const UTF8Char*)"Joy Stick Port");
					break;
				case 13:
					sb->Append((const UTF8Char*)"Keyboard Port");
					break;
				case 14:
					sb->Append((const UTF8Char*)"Mouse Port");
					break;
				case 15:
					sb->Append((const UTF8Char*)"SSA SCSI");
					break;
				case 16:
					sb->Append((const UTF8Char*)"USB");
					break;
				case 17:
					sb->Append((const UTF8Char*)"FireWire (IEEE P1394)");
					break;
				case 18:
					sb->Append((const UTF8Char*)"PCMCIA Type I2");
					break;
				case 19:
					sb->Append((const UTF8Char*)"PCMCIA Type II");
					break;
				case 20:
					sb->Append((const UTF8Char*)"PCMCIA Type III");
					break;
				case 21:
					sb->Append((const UTF8Char*)"Cardbus");
					break;
				case 22:
					sb->Append((const UTF8Char*)"Access Bus Port");
					break;
				case 23:
					sb->Append((const UTF8Char*)"SCSI II");
					break;
				case 24:
					sb->Append((const UTF8Char*)"SCSI Wide");
					break;
				case 25:
					sb->Append((const UTF8Char*)"PC-98");
					break;
				case 26:
					sb->Append((const UTF8Char*)"PC-98-Hireso");
					break;
				case 27:
					sb->Append((const UTF8Char*)"PC-H98");
					break;
				case 28:
					sb->Append((const UTF8Char*)"Video Port");
					break;
				case 29:
					sb->Append((const UTF8Char*)"Audio Port");
					break;
				case 30:
					sb->Append((const UTF8Char*)"Modem Port");
					break;
				case 31:
					sb->Append((const UTF8Char*)"Network Port");
					break;
				case 32:
					sb->Append((const UTF8Char*)"SATA");
					break;
				case 33:
					sb->Append((const UTF8Char*)"SAS");
					break;
				case 160:
					sb->Append((const UTF8Char*)"8251 Compatible");
					break;
				case 161:
					sb->Append((const UTF8Char*)"8251 FIFO Compatible");
					break;
				case 255:
					sb->Append((const UTF8Char*)"Other");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[8]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 9:
			sb->Append((const UTF8Char*)"SMBIOS Type 9 - System Slots\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 12)
			{
				sb->Append((const UTF8Char*)"Slot Designation: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Slot Type: ");
				switch (dataBuff[5])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"ISA");
					break;
				case 4:
					sb->Append((const UTF8Char*)"MCA");
					break;
				case 5:
					sb->Append((const UTF8Char*)"EISA");
					break;
				case 6:
					sb->Append((const UTF8Char*)"PCI");
					break;
				case 7:
					sb->Append((const UTF8Char*)"PC Card (PCMCIA)");
					break;
				case 8:
					sb->Append((const UTF8Char*)"VL-VESA");
					break;
				case 9:
					sb->Append((const UTF8Char*)"Proprietary");
					break;
				case 10:
					sb->Append((const UTF8Char*)"Processor Card Slot");
					break;
				case 11:
					sb->Append((const UTF8Char*)"Proprietary Memory Card Slot");
					break;
				case 12:
					sb->Append((const UTF8Char*)"I/O Riser Card Slot");
					break;
				case 13:
					sb->Append((const UTF8Char*)"NuBus");
					break;
				case 14:
					sb->Append((const UTF8Char*)"PCI - 66MHz Capable");
					break;
				case 15:
					sb->Append((const UTF8Char*)"AGP");
					break;
				case 16:
					sb->Append((const UTF8Char*)"AGP 2X");
					break;
				case 17:
					sb->Append((const UTF8Char*)"AGP 4X");
					break;
				case 18:
					sb->Append((const UTF8Char*)"PCI-X");
					break;
				case 19:
					sb->Append((const UTF8Char*)"AGP 8X");
					break;
				case 20:
					sb->Append((const UTF8Char*)"M.2 Socket 1-DP (Mechanical Key A)");
					break;
				case 21:
					sb->Append((const UTF8Char*)"M.2 Socket 1-SD (Mechanical Key E)");
					break;
				case 22:
					sb->Append((const UTF8Char*)"M.2 Socket 2 (Mechanical Key B)");
					break;
				case 23:
					sb->Append((const UTF8Char*)"M.2 Socket 3 (Mechanical Key M)");
					break;
				case 24:
					sb->Append((const UTF8Char*)"MXM Type I");
					break;
				case 25:
					sb->Append((const UTF8Char*)"MXM Type II");
					break;
				case 26:
					sb->Append((const UTF8Char*)"MXM Type III (standard connector)");
					break;
				case 27:
					sb->Append((const UTF8Char*)"MXM Type III (HE connector)");
					break;
				case 28:
					sb->Append((const UTF8Char*)"MXM Type IV");
					break;
				case 29:
					sb->Append((const UTF8Char*)"MXM 3.0 Type A");
					break;
				case 30:
					sb->Append((const UTF8Char*)"MXM 3.0 Type B");
					break;
				case 31:
					sb->Append((const UTF8Char*)"PCI Express Gen 2 SFF-8639");
					break;
				case 32:
					sb->Append((const UTF8Char*)"PCI Express Gen 3 SFF-8639");
					break;
				case 33:
					sb->Append((const UTF8Char*)"PCI Express Mini 52-pin (CEM spec. 2.0) with bottom-side keep-outs");
					break;
				case 34:
					sb->Append((const UTF8Char*)"PCI Express Mini 52-pin (CEM spec. 2.0) without bottom-side keep-outs");
					break;
				case 35:
					sb->Append((const UTF8Char*)"PCI Express Mini 76-pin (CEM spec. 2.0) Corresponds to Display-Mini card");
					break;
				case 160:
					sb->Append((const UTF8Char*)"PC-98/C20");
					break;
				case 161:
					sb->Append((const UTF8Char*)"PC-98/C24");
					break;
				case 162:
					sb->Append((const UTF8Char*)"PC-98/E");
					break;
				case 163:
					sb->Append((const UTF8Char*)"PC-98/Local Bus");
					break;
				case 164:
					sb->Append((const UTF8Char*)"PC-98/Card");
					break;
				case 165:
					sb->Append((const UTF8Char*)"PCI Express (see below)");
					break;
				case 166:
					sb->Append((const UTF8Char*)"PCI Express x1");
					break;
				case 167:
					sb->Append((const UTF8Char*)"PCI Express x2");
					break;
				case 168:
					sb->Append((const UTF8Char*)"PCI Express x4");
					break;
				case 169:
					sb->Append((const UTF8Char*)"PCI Express x8");
					break;
				case 170:
					sb->Append((const UTF8Char*)"PCI Express x16");
					break;
				case 171:
					sb->Append((const UTF8Char*)"PCI Express Gen 2 (see below)");
					break;
				case 172:
					sb->Append((const UTF8Char*)"PCI Express Gen 2 x1");
					break;
				case 173:
					sb->Append((const UTF8Char*)"PCI Express Gen 2 x2");
					break;
				case 174:
					sb->Append((const UTF8Char*)"PCI Express Gen 2 x4");
					break;
				case 175:
					sb->Append((const UTF8Char*)"PCI Express Gen 2 x8");
					break;
				case 176:
					sb->Append((const UTF8Char*)"PCI Express Gen 2 x16");
					break;
				case 177:
					sb->Append((const UTF8Char*)"PCI Express Gen 3 (see below)");
					break;
				case 178:
					sb->Append((const UTF8Char*)"PCI Express Gen 3 x1");
					break;
				case 179:
					sb->Append((const UTF8Char*)"PCI Express Gen 3 x2");
					break;
				case 180:
					sb->Append((const UTF8Char*)"PCI Express Gen 3 x4");
					break;
				case 181:
					sb->Append((const UTF8Char*)"PCI Express Gen 3 x8");
					break;
				case 182:
					sb->Append((const UTF8Char*)"PCI Express Gen 3 x16");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[5]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Slot Data Bus Width: ");
				switch (dataBuff[6])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"8 bit");
					break;
				case 4:
					sb->Append((const UTF8Char*)"16 bit");
					break;
				case 5:
					sb->Append((const UTF8Char*)"32 bit");
					break;
				case 6:
					sb->Append((const UTF8Char*)"64 bit");
					break;
				case 7:
					sb->Append((const UTF8Char*)"128 bit");
					break;
				case 8:
					sb->Append((const UTF8Char*)"1x or x1");
					break;
				case 9:
					sb->Append((const UTF8Char*)"2x or x2");
					break;
				case 10:
					sb->Append((const UTF8Char*)"4x or x4");
					break;
				case 11:
					sb->Append((const UTF8Char*)"8x or x8");
					break;
				case 12:
					sb->Append((const UTF8Char*)"12x or x12");
					break;
				case 13:
					sb->Append((const UTF8Char*)"16x or x16");
					break;
				case 14:
					sb->Append((const UTF8Char*)"32x or x32");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[6]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Current Usage: ");
				switch (dataBuff[7])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Available");
					break;
				case 4:
					sb->Append((const UTF8Char*)"In use");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[7]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Slot Length: ");
				switch (dataBuff[8])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Short Length");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Long Length");
					break;
				case 5:
					sb->Append((const UTF8Char*)"2.5\" drive form factor");
					break;
				case 6:
					sb->Append((const UTF8Char*)"3.5\" drive form factor");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[8]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Slot ID: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[9]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Slot Characteristics 1: 0x");
				sb->AppendHex8(dataBuff[11]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 13)
			{
				sb->Append((const UTF8Char*)"Slot Characteristics 2: 0x");
				sb->AppendHex8(dataBuff[12]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 17)
			{
				sb->Append((const UTF8Char*)"Segment Group Number: ");
				sb->AppendI16(ReadInt16(&dataBuff[13]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Bus Number: ");
				sb->AppendU16(dataBuff[15]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Device/Function Number: 0x");
				sb->AppendHex8(dataBuff[16]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 10:
			sb->Append((const UTF8Char*)"SMBIOS Type 10 - On Board Devices Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			l = 4;
			k = 1;
			while (l < dataBuff[1])
			{
				sb->Append((const UTF8Char*)"Device ");
				sb->AppendUOSInt(k);
				sb->Append((const UTF8Char*)" Status: ");
				if (dataBuff[l] & 0x80)
				{
					sb->Append((const UTF8Char*)"Enabled");
				}
				else
				{
					sb->Append((const UTF8Char*)"Disabled");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Device ");
				sb->AppendUOSInt(k);
				sb->Append((const UTF8Char*)" Type: ");
				switch (dataBuff[l] & 0x7f)
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Video");
					break;
				case 4:
					sb->Append((const UTF8Char*)"SCSI Controller");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Ethernet");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Token Ring");
					break;
				case 7:
					sb->Append((const UTF8Char*)"Sound");
					break;
				case 8:
					sb->Append((const UTF8Char*)"PATA Controller");
					break;
				case 9:
					sb->Append((const UTF8Char*)"SATA Controller");
					break;
				case 10:
					sb->Append((const UTF8Char*)"SAS Controller");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[l] & 0x7f);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Device ");
				sb->AppendUOSInt(k);
				sb->Append((const UTF8Char*)" Description: ");
				if (carr[dataBuff[l + 1]]) sb->Append((const UTF8Char*)carr[dataBuff[l + 1]]);
				sb->Append((const UTF8Char*)"\r\n");
				l += 2;
				k += 1;
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 11:
			sb->Append((const UTF8Char*)"SMBIOS Type 11 - OEM Strings\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Number of strings: ");
			sb->AppendU16(dataBuff[4]);
			sb->Append((const UTF8Char*)"\r\n");
			k = 0;
			while (k < 16)
			{
				if (carr[k])
				{
					sb->Append((const UTF8Char*)"String ");
					sb->AppendUOSInt(k);
					sb->Append((const UTF8Char*)": ");
					sb->Append((const UTF8Char*)carr[k]);
					sb->Append((const UTF8Char*)"\r\n");
				}
				k++;
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 12:
			sb->Append((const UTF8Char*)"SMBIOS Type 12 - System Configuration Options\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Number of strings: ");
			sb->AppendU16(dataBuff[4]);
			sb->Append((const UTF8Char*)"\r\n");
			k = 0;
			while (k < 16)
			{
				if (carr[k])
				{
					sb->Append((const UTF8Char*)"String ");
					sb->AppendUOSInt(k);
					sb->Append((const UTF8Char*)": ");
					sb->Append((const UTF8Char*)carr[k]);
					sb->Append((const UTF8Char*)"\r\n");
				}
				k++;
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 13:
			sb->Append((const UTF8Char*)"SMBIOS Type 13 - BIOS Language Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 5)
			{
				sb->Append((const UTF8Char*)"Installable Languages: ");
				sb->AppendU16(dataBuff[4]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 6)
			{
				sb->Append((const UTF8Char*)"Flags: 0x");
				sb->AppendHex8(dataBuff[5]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 22)
			{
				sb->Append((const UTF8Char*)"Current Language: ");
				sb->AppendU16(dataBuff[21]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			k = 0;
			while (k < 16)
			{
				if (carr[k])
				{
					sb->Append((const UTF8Char*)"Language ");
					sb->AppendUOSInt(k);
					sb->Append((const UTF8Char*)": ");
					sb->Append((const UTF8Char*)carr[k]);
					sb->Append((const UTF8Char*)"\r\n");
				}
				k++;
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 14:
			sb->Append((const UTF8Char*)"SMBIOS Type 14 - Group Association\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Group Name: ");
			if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
			sb->Append((const UTF8Char*)"\r\n");
			k = 5;
			while (k < dataBuff[1])
			{
				sb->Append((const UTF8Char*)"Item Type: ");
				sb->AppendU16(dataBuff[k]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Item Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[k + 1]));
				sb->Append((const UTF8Char*)"\r\n");
				k += 3;
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 15:
			sb->Append((const UTF8Char*)"SMBIOS Type 15 - System Event Log\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 20)
			{
				sb->Append((const UTF8Char*)"Log Area Length: ");
				sb->AppendI16(ReadInt16(&dataBuff[4]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Log Header Start Offset: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[6]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Log Data Start Offset: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[8]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Access Method: ");
				switch (dataBuff[10])
				{
				case 0:
					sb->Append((const UTF8Char*)"Indexed I/O: 1 8-bit index port, 1 8-bit data port");
					break;
				case 1:
					sb->Append((const UTF8Char*)"Indexed I/O: 2 8-bit index ports, 1 8-bit data port");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Indexed I/O: 1 16-bit index port, 1 8-bit data port");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Memory-mapped physical 32-bit address");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Available through General-Purpose NonVolatile Data functions");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[10]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Log Status: 0x");
				sb->AppendHex8(dataBuff[11]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Log Change Token: 0x");
				sb->AppendHex32(ReadUInt32(&dataBuff[12]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Access Method Address: 0x");
				sb->AppendHex32(ReadUInt32(&dataBuff[16]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 23)
			{
				sb->Append((const UTF8Char*)"Log Header Format: ");
				switch (dataBuff[20])
				{
				case 0:
					sb->Append((const UTF8Char*)"No Header");
					break;
				case 1:
					sb->Append((const UTF8Char*)"Type 1 log header");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[20]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Number of Supported Log Type Descriptor: ");
				sb->AppendU16(dataBuff[21]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Length of each Log Type Descriptor: ");
				sb->AppendU16(dataBuff[22]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 16:
			sb->Append((const UTF8Char*)"SMBIOS Type 16 - Physical Memory Array\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 15)
			{
				sb->Append((const UTF8Char*)"Location: ");
				switch (dataBuff[4])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"System board or motherboard");
					break;
				case 4:
					sb->Append((const UTF8Char*)"ISA add-on card");
					break;
				case 5:
					sb->Append((const UTF8Char*)"EISA add-on card");
					break;
				case 6:
					sb->Append((const UTF8Char*)"PCI add-on card");
					break;
				case 7:
					sb->Append((const UTF8Char*)"MCA add-on card");
					break;
				case 8:
					sb->Append((const UTF8Char*)"PCMCIA add-on card");
					break;
				case 9:
					sb->Append((const UTF8Char*)"Proprietary add-on card");
					break;
				case 10:
					sb->Append((const UTF8Char*)"NuBus");
					break;
				case 160:
					sb->Append((const UTF8Char*)"PC-98/C20 add-on card");
					break;
				case 161:
					sb->Append((const UTF8Char*)"PC-98/C24 add-on card");
					break;
				case 162:
					sb->Append((const UTF8Char*)"PC-98/E add-on card");
					break;
				case 163:
					sb->Append((const UTF8Char*)"PC-98/Local bus add-on card");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[4]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Use: ");
				switch (dataBuff[5])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"System memory");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Video memory");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Flash memory");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Non-volatile RAM");
					break;
				case 7:
					sb->Append((const UTF8Char*)"Cache memory");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[5]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Memory Error Correction: ");
				switch (dataBuff[6])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"None");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Parity");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Single-bit ECC");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Multi-bit ECC");
					break;
				case 7:
					sb->Append((const UTF8Char*)"CRC");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[6]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Maximum Capacity: ");
				sb->AppendI32(ReadInt32(&dataBuff[7]));
				sb->Append((const UTF8Char*)"KB\r\n");
				sb->Append((const UTF8Char*)"Memory Error Information Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[11]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Number of Memory Devices: ");
				sb->AppendI16(ReadInt16(&dataBuff[13]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 17:
			sb->Append((const UTF8Char*)"SMBIOS Type 17 - Memory Device\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 21)
			{
				sb->Append((const UTF8Char*)"Physical Memory Array Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[4]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Memory Error Information Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[6]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Total Width: ");
				sb->AppendI16(ReadInt16(&dataBuff[8]));
				sb->Append((const UTF8Char*)" bits\r\n");
				sb->Append((const UTF8Char*)"Data Width: ");
				sb->AppendI16(ReadInt16(&dataBuff[10]));
				sb->Append((const UTF8Char*)" bits\r\n");
				sb->Append((const UTF8Char*)"Size: ");
				if (dataBuff[13] & 0x80)
				{
					sb->AppendI16(ReadInt16(&dataBuff[12]) & 0x7fff);
					sb->Append((const UTF8Char*)"KB");
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[12]) & 0x7fff);
					sb->Append((const UTF8Char*)"MB");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Form Factor: ");
				switch (dataBuff[14])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"SIMM");
					break;
				case 4:
					sb->Append((const UTF8Char*)"SIP");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Chip");
					break;
				case 6:
					sb->Append((const UTF8Char*)"DIP");
					break;
				case 7:
					sb->Append((const UTF8Char*)"ZIP");
					break;
				case 8:
					sb->Append((const UTF8Char*)"Proprietary Card");
					break;
				case 9:
					sb->Append((const UTF8Char*)"DIMM");
					break;
				case 10:
					sb->Append((const UTF8Char*)"TSOP");
					break;
				case 11:
					sb->Append((const UTF8Char*)"Row of chips");
					break;
				case 12:
					sb->Append((const UTF8Char*)"RIMM");
					break;
				case 13:
					sb->Append((const UTF8Char*)"SODIMM");
					break;
				case 14:
					sb->Append((const UTF8Char*)"SRIMM");
					break;
				case 15:
					sb->Append((const UTF8Char*)"FB-DIMM");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[14]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Device Set: ");
				sb->AppendU16(dataBuff[15]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Device Locator: ");
				if (carr[dataBuff[16]]) sb->Append((const UTF8Char*)carr[dataBuff[16]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Bank Locator: ");
				if (carr[dataBuff[17]]) sb->Append((const UTF8Char*)carr[dataBuff[17]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Memory Type: ");
				switch (dataBuff[18])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"DRAM");
					break;
				case 4:
					sb->Append((const UTF8Char*)"EDRAM");
					break;
				case 5:
					sb->Append((const UTF8Char*)"VRAM");
					break;
				case 6:
					sb->Append((const UTF8Char*)"SRAM");
					break;
				case 7:
					sb->Append((const UTF8Char*)"RAM");
					break;
				case 8:
					sb->Append((const UTF8Char*)"ROM");
					break;
				case 9:
					sb->Append((const UTF8Char*)"FLASH");
					break;
				case 10:
					sb->Append((const UTF8Char*)"EEPROM");
					break;
				case 11:
					sb->Append((const UTF8Char*)"FEPROM");
					break;
				case 12:
					sb->Append((const UTF8Char*)"EPROM");
					break;
				case 13:
					sb->Append((const UTF8Char*)"CDRAM");
					break;
				case 14:
					sb->Append((const UTF8Char*)"3DRAM");
					break;
				case 15:
					sb->Append((const UTF8Char*)"SDRAM");
					break;
				case 16:
					sb->Append((const UTF8Char*)"SGRAM");
					break;
				case 17:
					sb->Append((const UTF8Char*)"RDRAM");
					break;
				case 18:
					sb->Append((const UTF8Char*)"DDR");
					break;
				case 19:
					sb->Append((const UTF8Char*)"DDR2");
					break;
				case 20:
					sb->Append((const UTF8Char*)"DDR2 FB-DIMM");
					break;
				case 24:
					sb->Append((const UTF8Char*)"DDR3");
					break;
				case 25:
					sb->Append((const UTF8Char*)"FBD2");
					break;
				case 26:
					sb->Append((const UTF8Char*)"DDR4");
					break;
				case 27:
					sb->Append((const UTF8Char*)"LPDDR");
					break;
				case 28:
					sb->Append((const UTF8Char*)"LPDDR2");
					break;
				case 29:
					sb->Append((const UTF8Char*)"LPDDR3");
					break;
				case 30:
					sb->Append((const UTF8Char*)"LPDDR4");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[18]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Type Detail: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[19]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 27)
			{
				sb->Append((const UTF8Char*)"Speed: ");
				sb->AppendI16(ReadInt16(&dataBuff[21]));
				sb->Append((const UTF8Char*)"MT/s\r\n");
				sb->Append((const UTF8Char*)"Manufacturer: ");
				if (carr[dataBuff[23]]) sb->Append((const UTF8Char*)carr[dataBuff[23]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Serial Number: ");
				if (carr[dataBuff[24]]) sb->Append((const UTF8Char*)carr[dataBuff[24]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Asset Tag: ");
				if (carr[dataBuff[25]]) sb->Append((const UTF8Char*)carr[dataBuff[25]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Part Number: ");
				if (carr[dataBuff[26]]) sb->Append((const UTF8Char*)carr[dataBuff[26]]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 28)
			{
				sb->Append((const UTF8Char*)"Attributes: 0x");
				sb->AppendHex8(dataBuff[27]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 34)
			{
				sb->Append((const UTF8Char*)"Extended Size: ");
				sb->AppendI32(ReadInt32(&dataBuff[28]));
				sb->Append((const UTF8Char*)"MB\r\n");
				sb->Append((const UTF8Char*)"Configured Memory Clock Speed: ");
				sb->AppendI16(ReadInt16(&dataBuff[32]));
				sb->Append((const UTF8Char*)"MT/s\r\n");
			}
			if (dataBuff[1] >= 40)
			{
				sb->Append((const UTF8Char*)"Minimum voltage: ");
				Text::SBAppendF64(sb, ReadInt16(&dataBuff[34]) * 0.001);
				sb->Append((const UTF8Char*)"V\r\n");
				sb->Append((const UTF8Char*)"Maximum voltage: ");
				Text::SBAppendF64(sb, ReadInt16(&dataBuff[36]) * 0.001);
				sb->Append((const UTF8Char*)"V\r\n");
				sb->Append((const UTF8Char*)"Configured voltage: ");
				Text::SBAppendF64(sb, ReadInt16(&dataBuff[38]) * 0.001);
				sb->Append((const UTF8Char*)"V\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
/*		case 18:
			sb->Append((const UTF8Char*)"SMBIOS Type 18 - 32-Bit Memory Error Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
			break;*/
		case 19:
			sb->Append((const UTF8Char*)"SMBIOS Type 19 - Memory Array Mapped Address\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 15)
			{
				sb->Append((const UTF8Char*)"Starting Address: 0x");
				sb->AppendHex32(ReadUInt32(&dataBuff[4]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Ending Address: 0x");
				sb->AppendHex32(ReadUInt32(&dataBuff[8]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Memory Array Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[12]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Partition Width: ");
				sb->AppendU16(dataBuff[14]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 31)
			{
				sb->Append((const UTF8Char*)"Extended Starting Address: 0x");
				sb->AppendHex64(ReadUInt64(&dataBuff[15]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Extended Ending Address: 0x");
				sb->AppendHex64(ReadUInt64(&dataBuff[23]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 20:
			sb->Append((const UTF8Char*)"SMBIOS Type 20 - Memory Device Mapped Address\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 19)
			{
				sb->Append((const UTF8Char*)"Starting Address: 0x");
				sb->AppendHex32(ReadUInt32(&dataBuff[4]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Ending Address: 0x");
				sb->AppendHex32(ReadUInt32(&dataBuff[8]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Memory Device Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[12]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Memory Array Mapped Address Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[14]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Partition Row Position: ");
				sb->AppendU16(dataBuff[16]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Interleave Position: ");
				sb->AppendU16(dataBuff[17]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Interleave Data Depth: ");
				sb->AppendU16(dataBuff[18]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 35)
			{
				sb->Append((const UTF8Char*)"Extended Starting Address: 0x");
				sb->AppendHex64(ReadUInt64(&dataBuff[19]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Extended Ending Address: 0x");
				sb->AppendHex64(ReadUInt64(&dataBuff[27]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 21:
			sb->Append((const UTF8Char*)"SMBIOS Type 21 - Build-in Pointing Device\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 7)
			{
				sb->Append((const UTF8Char*)"Type: ");
				switch (dataBuff[4])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Mouse");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Track Ball");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Track Point");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Glide Point");
					break;
				case 7:
					sb->Append((const UTF8Char*)"Touch Pad");
					break;
				case 8:
					sb->Append((const UTF8Char*)"Touch Screen");
					break;
				case 9:
					sb->Append((const UTF8Char*)"Optical Sensor");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[4]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Interface: ");
				switch (dataBuff[5])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Serial");
					break;
				case 4:
					sb->Append((const UTF8Char*)"PS/2");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Infrared");
					break;
				case 6:
					sb->Append((const UTF8Char*)"HP-HIL");
					break;
				case 7:
					sb->Append((const UTF8Char*)"Bus mouse");
					break;
				case 8:
					sb->Append((const UTF8Char*)"ADB (Apple Desktop Bus)");
					break;
				case 160:
					sb->Append((const UTF8Char*)"Bus mouse DB-9");
					break;
				case 161:
					sb->Append((const UTF8Char*)"Bus mouse micro-DIN");
					break;
				case 162:
					sb->Append((const UTF8Char*)"USB");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[5]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Number of Buttons: ");
				sb->AppendU16(dataBuff[6]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 22:
			sb->Append((const UTF8Char*)"SMBIOS Type 22 - Portable Battery\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Location: ");
			if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Manufacturer: ");
			if (carr[dataBuff[5]]) sb->Append((const UTF8Char*)carr[dataBuff[5]]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Manufacture Date: ");
			if (carr[dataBuff[6]]) sb->Append((const UTF8Char*)carr[dataBuff[6]]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Serial Number: ");
			if (carr[dataBuff[7]]) sb->Append((const UTF8Char*)carr[dataBuff[7]]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Device Name: ");
			if (carr[dataBuff[8]]) sb->Append((const UTF8Char*)carr[dataBuff[8]]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Device Chemistry: ");
			switch (dataBuff[9])
			{
			case 1:
				sb->Append((const UTF8Char*)"Other");
				break;
			case 2:
				sb->Append((const UTF8Char*)"Unknown");
				break;
			case 3:
				sb->Append((const UTF8Char*)"Lead Acid");
				break;
			case 4:
				sb->Append((const UTF8Char*)"Nickel Cadmium");
				break;
			case 5:
				sb->Append((const UTF8Char*)"Nickel metal hydride");
				break;
			case 6:
				sb->Append((const UTF8Char*)"Lithium-ion");
				break;
			case 7:
				sb->Append((const UTF8Char*)"Zinc air");
				break;
			case 8:
				sb->Append((const UTF8Char*)"Lithium Polymer");
				break;
			default:
				sb->Append((const UTF8Char*)"Undefined (");
				sb->AppendU16(dataBuff[9]);
				sb->Append((const UTF8Char*)")");
				break;
			}
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Design Capacity: ");
			sb->AppendU16(ReadUInt16(&dataBuff[10]));
			sb->Append((const UTF8Char*)"mWh\r\n");
			sb->Append((const UTF8Char*)"Design Voltage: ");
			sb->AppendU16(ReadUInt16(&dataBuff[12]));
			sb->Append((const UTF8Char*)"mV\r\n");
			sb->Append((const UTF8Char*)"SBDS Version Number: ");
			if (carr[dataBuff[14]]) sb->Append((const UTF8Char*)carr[dataBuff[14]]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Maximum Error in Battery Data: ");
			sb->AppendU16(dataBuff[15]);
			sb->Append((const UTF8Char*)"%\r\n");
			if (dataBuff[1] >= 26)
			{
				sb->Append((const UTF8Char*)"SBDS Serial Number: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[16]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"SBDS Manufacture Date: ");
				sb->AppendU16((UInt16)(1980 + (dataBuff[19] >> 1)));
				sb->Append((const UTF8Char*)"-");
				sb->AppendU16((UInt16)((dataBuff[18] >> 5) | ((dataBuff[19] & 1) << 3)));
				sb->Append((const UTF8Char*)"-");
				sb->AppendU16(dataBuff[18] & 31);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"SBDS Device Chemistry: ");
				if (carr[dataBuff[20]]) sb->Append((const UTF8Char*)carr[dataBuff[20]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Design Capacity Multiplier: ");
				sb->AppendU16(dataBuff[21]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"OEM-specific: 0x");
				sb->AppendHex32(ReadUInt32(&dataBuff[22]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 23:
			sb->Append((const UTF8Char*)"SMBIOS Type 23 - System Reset\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 13)
			{
				sb->Append((const UTF8Char*)"Capabilities: 0x");
				sb->AppendHex8(dataBuff[4]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Reset Count: ");
				sb->AppendI16(ReadInt16(&dataBuff[5]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Reset Limit: ");
				sb->AppendI16(ReadInt16(&dataBuff[7]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Time Interval: ");
				sb->AppendI16(ReadInt16(&dataBuff[9]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Timeout: ");
				sb->AppendI16(ReadInt16(&dataBuff[11]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 24:
			sb->Append((const UTF8Char*)"SMBIOS Type 24 - Hardware Security\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 5)
			{
				sb->Append((const UTF8Char*)"Hardware Security Settings: 0x");
				sb->AppendHex8(dataBuff[4]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 25:
			sb->Append((const UTF8Char*)"SMBIOS Type 25 - System Power Control\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 9)
			{
				sb->Append((const UTF8Char*)"Next Scheduled Power-on Month: ");
				sb->AppendHex8(dataBuff[4]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Next Scheduled Power-on Day-of-month: ");
				sb->AppendHex8(dataBuff[5]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Next Scheduled Power-on Hour: ");
				sb->AppendHex8(dataBuff[6]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Next Scheduled Power-on Minute: ");
				sb->AppendHex8(dataBuff[7]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Next Scheduled Power-on Second: ");
				sb->AppendHex8(dataBuff[8]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 26:
			sb->Append((const UTF8Char*)"SMBIOS Type 26 - Voltage Probe\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 20)
			{
				sb->Append((const UTF8Char*)"Description: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Location: ");
				switch (dataBuff[5] & 0x1f)
				{
				case 1:
					sb->Append((const UTF8Char*)"OTher");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Processor");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Disk");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Peripheral Bay");
					break;
				case 6:
					sb->Append((const UTF8Char*)"System Management Module");
					break;
				case 7:
					sb->Append((const UTF8Char*)"Motherboard");
					break;
				case 8:
					sb->Append((const UTF8Char*)"Memory Module");
					break;
				case 9:
					sb->Append((const UTF8Char*)"Processor Module");
					break;
				case 10:
					sb->Append((const UTF8Char*)"Power Unit");
					break;
				case 11:
					sb->Append((const UTF8Char*)"Add-in Card");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[5] & 0x1f);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Status: ");
				switch (dataBuff[5] >> 5)
				{
				case 1:
					sb->Append((const UTF8Char*)"OTher");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"OK");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Non-critical");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Critical");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Non-recoverable");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16((UInt16)(dataBuff[5] >> 5));
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Maximum Value: ");
				if (ReadInt16(&dataBuff[6]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[6]) * 0.001);
					sb->Append((const UTF8Char*)"V");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Minimum Value: ");
				if (ReadInt16(&dataBuff[8]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[8]) * 0.001);
					sb->Append((const UTF8Char*)"V");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Resolution: ");
				if (ReadInt16(&dataBuff[10]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[10]) * 0.01);
					sb->Append((const UTF8Char*)"V");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Tolerance: ");
				if (ReadInt16(&dataBuff[12]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[12]) * 0.001);
					sb->Append((const UTF8Char*)"V");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Accuracy: ");
				if (ReadInt16(&dataBuff[14]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[14]) * 0.01);
					sb->Append((const UTF8Char*)"%");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"OEM-defined: 0x");
				sb->AppendHex32(ReadUInt32(&dataBuff[16]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 22)
			{
				sb->Append((const UTF8Char*)"Nominal: ");
				if (ReadInt16(&dataBuff[20]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[20]) * 0.001);
					sb->Append((const UTF8Char*)"V");
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 27:
			sb->Append((const UTF8Char*)"SMBIOS Type 27 - Cooling Device\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 12)
			{
				sb->Append((const UTF8Char*)"Temperature Probe Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[4]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Device Type: ");
				switch (dataBuff[6] & 0x1f)
				{
				case 1:
					sb->Append((const UTF8Char*)"OTher");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Fan");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Centrifugal Blower");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Chip Fan");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Cabinet Fan");
					break;
				case 7:
					sb->Append((const UTF8Char*)"Power Supply Fan");
					break;
				case 8:
					sb->Append((const UTF8Char*)"Heat Pipe");
					break;
				case 9:
					sb->Append((const UTF8Char*)"Integrated Refrigeration");
					break;
				case 16:
					sb->Append((const UTF8Char*)"Active Cooling");
					break;
				case 17:
					sb->Append((const UTF8Char*)"Passive Cooling");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[6] & 0x1f);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Status: ");
				switch (dataBuff[6] >> 5)
				{
				case 1:
					sb->Append((const UTF8Char*)"OTher");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"OK");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Non-critical");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Critical");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Non-recoverable");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16((UInt16)(dataBuff[6] >> 5));
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Cooling Unit Group: ");
				sb->AppendU16(dataBuff[7]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"OEM-defined: 0x");
				sb->AppendHex32(ReadUInt32(&dataBuff[8]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 14)
			{
				sb->Append((const UTF8Char*)"Nominal Speed: ");
				if (ReadInt16(&dataBuff[12]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[12]));
					sb->Append((const UTF8Char*)"rpm");
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 15)
			{
				sb->Append((const UTF8Char*)"Description: ");
				if (carr[dataBuff[14]]) sb->Append((const UTF8Char*)carr[dataBuff[14]]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 28:
			sb->Append((const UTF8Char*)"SMBIOS Type 28 - Temperature Probe\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 20)
			{
				sb->Append((const UTF8Char*)"Description: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Location: ");
				switch (dataBuff[5] & 0x1f)
				{
				case 1:
					sb->Append((const UTF8Char*)"OTher");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Processor");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Disk");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Peripheral Bay");
					break;
				case 6:
					sb->Append((const UTF8Char*)"System Management Module");
					break;
				case 7:
					sb->Append((const UTF8Char*)"Motherboard");
					break;
				case 8:
					sb->Append((const UTF8Char*)"Memory Module");
					break;
				case 9:
					sb->Append((const UTF8Char*)"Processor Module");
					break;
				case 10:
					sb->Append((const UTF8Char*)"Power Unit");
					break;
				case 11:
					sb->Append((const UTF8Char*)"Add-in Card");
					break;
				case 12:
					sb->Append((const UTF8Char*)"Front Panel Board");
					break;
				case 13:
					sb->Append((const UTF8Char*)"Back Panel Board");
					break;
				case 14:
					sb->Append((const UTF8Char*)"Power System Board");
					break;
				case 15:
					sb->Append((const UTF8Char*)"Drive Back Plane");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[5] & 0x1f);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Status: ");
				switch (dataBuff[5] >> 5)
				{
				case 1:
					sb->Append((const UTF8Char*)"OTher");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"OK");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Non-critical");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Critical");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Non-recoverable");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16((UInt16)(dataBuff[5] >> 5));
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Maximum Value: ");
				if (ReadInt16(&dataBuff[6]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[6]) * 0.1);
					sb->Append((const UTF8Char*)"degree C");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Minimum Value: ");
				if (ReadInt16(&dataBuff[8]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[8]) * 0.1);
					sb->Append((const UTF8Char*)"degree C");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Resolution: ");
				if (ReadInt16(&dataBuff[10]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[10]) * 0.001);
					sb->Append((const UTF8Char*)"degree C");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Tolerance: ");
				if (ReadInt16(&dataBuff[12]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[12]) * 0.1);
					sb->Append((const UTF8Char*)"degree C");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Accuracy: ");
				if (ReadInt16(&dataBuff[14]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[14]) * 0.01);
					sb->Append((const UTF8Char*)"%");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"OEM-defined: 0x");
				sb->AppendHex32(ReadUInt32(&dataBuff[16]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 22)
			{
				sb->Append((const UTF8Char*)"Nominal Value: ");
				if (ReadInt16(&dataBuff[20]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[20]) * 0.1);
					sb->Append((const UTF8Char*)"degree C");
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 29:
			sb->Append((const UTF8Char*)"SMBIOS Type 29 - Electrical Current Probe\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 20)
			{
				sb->Append((const UTF8Char*)"Description: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Location: ");
				switch (dataBuff[5] & 0x1f)
				{
				case 1:
					sb->Append((const UTF8Char*)"OTher");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Processor");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Disk");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Peripheral Bay");
					break;
				case 6:
					sb->Append((const UTF8Char*)"System Management Module");
					break;
				case 7:
					sb->Append((const UTF8Char*)"Motherboard");
					break;
				case 8:
					sb->Append((const UTF8Char*)"Memory Module");
					break;
				case 9:
					sb->Append((const UTF8Char*)"Processor Module");
					break;
				case 10:
					sb->Append((const UTF8Char*)"Power Unit");
					break;
				case 11:
					sb->Append((const UTF8Char*)"Add-in Card");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[5] & 0x1f);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Status: ");
				switch (dataBuff[5] >> 5)
				{
				case 1:
					sb->Append((const UTF8Char*)"OTher");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"OK");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Non-critical");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Critical");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Non-recoverable");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16((UInt16)(dataBuff[5] >> 5));
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Maximum Value: ");
				if (ReadInt16(&dataBuff[6]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[6]) * 0.001);
					sb->Append((const UTF8Char*)"A");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Minimum Value: ");
				if (ReadInt16(&dataBuff[8]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[8]) * 0.001);
					sb->Append((const UTF8Char*)"A");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Resolution: ");
				if (ReadInt16(&dataBuff[10]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[10]) * 0.01);
					sb->Append((const UTF8Char*)"A");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Tolerance: ");
				if (ReadInt16(&dataBuff[12]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[12]) * 0.001);
					sb->Append((const UTF8Char*)"A");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Accuracy: ");
				if (ReadInt16(&dataBuff[14]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[14]) * 0.01);
					sb->Append((const UTF8Char*)"%");
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"OEM-defined: 0x");
				sb->AppendHex32(ReadUInt32(&dataBuff[16]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 22)
			{
				sb->Append((const UTF8Char*)"Nominal Value: ");
				if (ReadInt16(&dataBuff[20]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					Text::SBAppendF64(sb, ReadInt16(&dataBuff[20]) * 0.001);
					sb->Append((const UTF8Char*)"A");
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 30:
			sb->Append((const UTF8Char*)"SMBIOS Type 30 - Out-of-Band Remote Access\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 6)
			{
				sb->Append((const UTF8Char*)"Manufacturer Name: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Connectionx: 0x");
				sb->AppendHex8(dataBuff[5]);
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
/*		case 31:
			sb->Append((const UTF8Char*)"SMBIOS Type 31 - Boot Integrity Services Entry Point\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
			break;*/
		case 32:
			sb->Append((const UTF8Char*)"SMBIOS Type 32 - System Boot Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 11)
			{
				sb->Append((const UTF8Char*)"Boot Status: ");
				switch (dataBuff[10])
				{
				case 0:
					sb->Append((const UTF8Char*)"No errors detected");
					break;
				case 1:
					sb->Append((const UTF8Char*)"No bootable media");
					break;
				case 2:
					sb->Append((const UTF8Char*)"\"normal\" operating system failed to load");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Firmware-detected hardware failure, including \"unknown\" failure types");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Operating system-detected hardware failure");
					break;
				case 5:
					sb->Append((const UTF8Char*)"User-requested boot, usually through a keystroke");
					break;
				case 6:
					sb->Append((const UTF8Char*)"System security violation");
					break;
				case 7:
					sb->Append((const UTF8Char*)"Previously-requested image");
					break;
				case 8:
					sb->Append((const UTF8Char*)"System watchdog timer expired, causing the system to reboot");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[10]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
/*		case 33:
			sb->Append((const UTF8Char*)"SMBIOS Type 33 - 64-Bit Memory Error Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
			break;*/
		case 34:
			sb->Append((const UTF8Char*)"SMBIOS Type 34 - Management Device\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 11)
			{
				sb->Append((const UTF8Char*)"Description: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Type: ");
				switch (dataBuff[5])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"National Semiconductor LM75");
					break;
				case 4:
					sb->Append((const UTF8Char*)"National Semiconductor LM78");
					break;
				case 5:
					sb->Append((const UTF8Char*)"National Semiconductor LM79");
					break;
				case 6:
					sb->Append((const UTF8Char*)"National Semiconductor LM80");
					break;
				case 7:
					sb->Append((const UTF8Char*)"National Semiconductor LM81");
					break;
				case 8:
					sb->Append((const UTF8Char*)"Analog Devices ADM9240");
					break;
				case 9:
					sb->Append((const UTF8Char*)"Dallas Semiconductor DS1780");
					break;
				case 10:
					sb->Append((const UTF8Char*)"Maxim 1617");
					break;
				case 11:
					sb->Append((const UTF8Char*)"Genesys GL518SM");
					break;
				case 12:
					sb->Append((const UTF8Char*)"Winbond W83781D");
					break;
				case 13:
					sb->Append((const UTF8Char*)"Holtek HT82H791");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[5]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Address: 0x");
				sb->AppendHex32(ReadUInt32(&dataBuff[6]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Address Type: ");
				switch (dataBuff[10])
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"I/O Port");
					break;
				case 4:
					sb->Append((const UTF8Char*)"Memory");
					break;
				case 5:
					sb->Append((const UTF8Char*)"SM Bus");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[10]);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 35:
			sb->Append((const UTF8Char*)"SMBIOS Type 35 - Management Device Component\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 11)
			{
				sb->Append((const UTF8Char*)"Description: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Management Device Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[5]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Component Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[7]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Threshold Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[9]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 36:
			sb->Append((const UTF8Char*)"SMBIOS Type 36 - Management Device Threshold Data\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 16)
			{
				sb->Append((const UTF8Char*)"Lower Threshold - Non-critical: ");
				if (ReadInt16(&dataBuff[4]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[4]));
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Upper Threshold - Non-critical: ");
				if (ReadInt16(&dataBuff[6]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[6]));
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Lower Threshold - Critical: ");
				if (ReadInt16(&dataBuff[8]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[8]));
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Upper Threshold - Critical: ");
				if (ReadInt16(&dataBuff[10]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[10]));
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Lower Threshold - Non-recoverable: ");
				if (ReadInt16(&dataBuff[12]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[12]));
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Upper Threshold - Non-recoverable: ");
				if (ReadInt16(&dataBuff[14]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[14]));
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
/*		case 37:
			sb->Append((const UTF8Char*)"SMBIOS Type 37 - Memory Channel\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
			break;*/
/*		case 38:
			sb->Append((const UTF8Char*)"SMBIOS Type 38 - IPMI Device Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
			break;*/
		case 39:
			sb->Append((const UTF8Char*)"SMBIOS Type 39 - System Power Supply\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 16)
			{
				sb->Append((const UTF8Char*)"Power Unit Group: ");
				sb->AppendU16(dataBuff[4]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Location: ");
				if (carr[dataBuff[5]]) sb->Append((const UTF8Char*)carr[dataBuff[5]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Device Name: ");
				if (carr[dataBuff[6]]) sb->Append((const UTF8Char*)carr[dataBuff[6]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Manufacturer: ");
				if (carr[dataBuff[7]]) sb->Append((const UTF8Char*)carr[dataBuff[7]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Serial Number: ");
				if (carr[dataBuff[8]]) sb->Append((const UTF8Char*)carr[dataBuff[8]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Asset Tag Number: ");
				if (carr[dataBuff[9]]) sb->Append((const UTF8Char*)carr[dataBuff[9]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Model Part Number: ");
				if (carr[dataBuff[10]]) sb->Append((const UTF8Char*)carr[dataBuff[10]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Revision Level: ");
				if (carr[dataBuff[11]]) sb->Append((const UTF8Char*)carr[dataBuff[11]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Max Power Capacity: ");
				if (ReadInt16(&dataBuff[12]) == -0x8000)
				{
					sb->Append((const UTF8Char*)"unknown");
				}
				else
				{
					sb->AppendI16(ReadInt16(&dataBuff[12]));
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Power Supply Characteristics: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[14]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (dataBuff[1] >= 22)
			{
				sb->Append((const UTF8Char*)"Input Voltage Probe Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[16]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Cooling Device Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[18]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Input Current Probe Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[20]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 40:
			sb->Append((const UTF8Char*)"SMBIOS Type 40 - Additional Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Number of Additional Information entries: ");
			sb->AppendU16(dataBuff[4]);
			sb->Append((const UTF8Char*)"\r\n");
			l = dataBuff[4];
			k = 5;
			while (k < dataBuff[1])
			{
				if (dataBuff[k] < 6)
					break;
				if (l-- <= 0)
					break;
				
				sb->Append((const UTF8Char*)"Entry Length: ");
				sb->AppendU16(dataBuff[k]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Referenced Handle: 0x");
				sb->AppendHex16(ReadUInt16(&dataBuff[k + 1]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Referenced Offset: ");
				sb->AppendU16(dataBuff[k + 3]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"String: ");
				if (carr[dataBuff[k + 4]]) sb->Append((const UTF8Char*)carr[dataBuff[k + 4]]);
				sb->Append((const UTF8Char*)"\r\n");
				k += dataBuff[k];
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 41:
			sb->Append((const UTF8Char*)"SMBIOS Type 41 - Onboard Devices Extended Information\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 11)
			{
				sb->Append((const UTF8Char*)"Reference Designation: ");
				if (carr[dataBuff[4]]) sb->Append((const UTF8Char*)carr[dataBuff[4]]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Device Type: ");
				switch (dataBuff[5] & 0x7f)
				{
				case 1:
					sb->Append((const UTF8Char*)"Other");
					break;
				case 2:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				case 3:
					sb->Append((const UTF8Char*)"Video");
					break;
				case 4:
					sb->Append((const UTF8Char*)"SCSI Controller");
					break;
				case 5:
					sb->Append((const UTF8Char*)"Ethernet");
					break;
				case 6:
					sb->Append((const UTF8Char*)"Token Ring");
					break;
				case 7:
					sb->Append((const UTF8Char*)"Sound");
					break;
				case 8:
					sb->Append((const UTF8Char*)"PATA Controller");
					break;
				case 9:
					sb->Append((const UTF8Char*)"SATA Controller");
					break;
				case 10:
					sb->Append((const UTF8Char*)"SAS Controller");
					break;
				default:
					sb->Append((const UTF8Char*)"Undefined (");
					sb->AppendU16(dataBuff[5] & 0x7f);
					sb->Append((const UTF8Char*)")");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Device Status: ");
				sb->Append((dataBuff[5] & 0x80)?(const UTF8Char*)"Device Enabled":(const UTF8Char*)"Device Disabled");
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Device Type Instance: ");
				sb->AppendU16(dataBuff[6]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Segment Group Number: ");
				sb->AppendI16(ReadInt16(&dataBuff[7]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Bus Number: ");
				sb->AppendU16(dataBuff[9]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Device Number: ");
				sb->AppendU16((UInt16)(dataBuff[10] >> 3));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Function Number: ");
				sb->AppendU16(dataBuff[10] & 7);
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
/*		case 42:
			sb->Append((const UTF8Char*)"SMBIOS Type 42 - Management Controller Host Interface\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
			break;*/
/*		case 43:
			sb->Append((const UTF8Char*)"SMBIOS Type 43 - TPM Device\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
			break;*/
		case 126:
			sb->Append((const UTF8Char*)"SMBIOS Type 126 - Inactive\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 127:
			sb->Append((const UTF8Char*)"SMBIOS Type 127 - End-of-Table\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
			break;
		case 128:
			sb->Append((const UTF8Char*)"SMBIOS Type 128 - Apple Firmware Volume\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 16)
			{
				sb->Append((const UTF8Char*)"Region Count: ");
				sb->AppendU16(dataBuff[4]);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Firmware Feature: 0x");
				sb->AppendHex32(ReadUInt16(&dataBuff[8]));
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Firmware Feature Mask: 0x");
				sb->AppendHex32(ReadUInt16(&dataBuff[12]));
				sb->Append((const UTF8Char*)"\r\n");
				
				if (dataBuff[1] >= 24 + dataBuff[4])
				{
					k = 0;
					while (k < dataBuff[4])
					{
						sb->Append((const UTF8Char*)"Region ");
						sb->AppendUOSInt(k);
						sb->Append((const UTF8Char*)" Type: ");
						switch (dataBuff[16 + k])
						{
						case 0:
							sb->Append((const UTF8Char*)"Reserved");
							break;
						case 1:
							sb->Append((const UTF8Char*)"Recovery");
							break;
						case 2:
							sb->Append((const UTF8Char*)"Main");
							break;
						case 3:
							sb->Append((const UTF8Char*)"NVRAM");
							break;
						case 4:
							sb->Append((const UTF8Char*)"Config");
							break;
						case 5:
							sb->Append((const UTF8Char*)"Diag Vault");
							break;
						default:
							sb->Append((const UTF8Char*)"Undefined (");
							sb->AppendU16(dataBuff[16 + k]);
							sb->Append((const UTF8Char*)")");
							break;
						}
						sb->Append((const UTF8Char*)"\r\n");
						sb->Append((const UTF8Char*)"Region ");
						sb->AppendUOSInt(k);
						sb->Append((const UTF8Char*)" Start Address: 0x");
						sb->AppendHex32(ReadUInt32(&dataBuff[24 + k * 8]));
						sb->Append((const UTF8Char*)"\r\n");
						sb->Append((const UTF8Char*)"Region ");
						sb->AppendUOSInt(k);
						sb->Append((const UTF8Char*)" End Address: 0x");
						sb->AppendHex32(ReadUInt32(&dataBuff[24 + k * 8 + 4]));
						sb->Append((const UTF8Char*)"\r\n");
						
						k++;
					}
				}
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
/*		case 130:
			sb->Append((const UTF8Char*)"SMBIOS Type 130 - Apple Memory SPD Data\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
			break;*/
		case 131:
			sb->Append((const UTF8Char*)"SMBIOS Type 131 - Apple Processor Type\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			if (dataBuff[1] >= 6)
			{
				sb->Append((const UTF8Char*)"Processor Type: ");
				sb->AppendI16(ReadInt16(&dataBuff[4]));
				sb->Append((const UTF8Char*)"\r\n");
			}
			sb->Append((const UTF8Char*)"\r\n");
			break;
		default:
			sb->Append((const UTF8Char*)"SMBIOS Type ");
			sb->AppendU16(dataBuff[0]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Handle: 0x");
			sb->AppendHex16(ReadUInt16(&dataBuff[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
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

void Win32::SMBIOS::GetProcessorFamily(Text::StringBuilderUTF *sb, Int32 family)
{
	switch (family)
	{
	case 1:
		sb->Append((const UTF8Char*)"Other");
		break;
	case 2:
		sb->Append((const UTF8Char*)"Unknwon");
		break;
	case 3:
		sb->Append((const UTF8Char*)"8086");
		break;
	case 4:
		sb->Append((const UTF8Char*)"80286");
		break;
	case 5:
		sb->Append((const UTF8Char*)"Intel386 processor");
		break;
	case 6:
		sb->Append((const UTF8Char*)"Intel486 processor");
		break;
	case 7:
		sb->Append((const UTF8Char*)"8087");
		break;
	case 8:
		sb->Append((const UTF8Char*)"80287");
		break;
	case 9:
		sb->Append((const UTF8Char*)"80387");
		break;
	case 10:
		sb->Append((const UTF8Char*)"80487");
		break;
	case 11:
		sb->Append((const UTF8Char*)"Intel Pentium processor");
		break;
	case 12:
		sb->Append((const UTF8Char*)"Pentium Pro processor");
		break;
	case 13:
		sb->Append((const UTF8Char*)"Pentium II processor");
		break;
	case 14:
		sb->Append((const UTF8Char*)"Pentium processor with MMX technology");
		break;
	case 15:
		sb->Append((const UTF8Char*)"Intel Celeron processor");
		break;
	case 16:
		sb->Append((const UTF8Char*)"Pentium II Xeon processor");
		break;
	case 17:
		sb->Append((const UTF8Char*)"Pentium III processor");
		break;
	case 18:
		sb->Append((const UTF8Char*)"M1 Family");
		break;
	case 19:
		sb->Append((const UTF8Char*)"M2 Family");
		break;
	case 20:
		sb->Append((const UTF8Char*)"Intel Celeron M processor");
		break;
	case 21:
		sb->Append((const UTF8Char*)"Intel Pentium 4 HT processor");
		break;
	case 24:
		sb->Append((const UTF8Char*)"AMD Duron Processor Family");
		break;
	case 25:
		sb->Append((const UTF8Char*)"K5 Family");
		break;
	case 26:
		sb->Append((const UTF8Char*)"K6 Family");
		break;
	case 27:
		sb->Append((const UTF8Char*)"K6-2");
		break;
	case 28:
		sb->Append((const UTF8Char*)"K6-3");
		break;
	case 29:
		sb->Append((const UTF8Char*)"AMD Athlon Processor Family");
		break;
	case 30:
		sb->Append((const UTF8Char*)"AMD29000 Family");
		break;
	case 31:
		sb->Append((const UTF8Char*)"K6-2+");
		break;
	case 32:
		sb->Append((const UTF8Char*)"Power PC Family");
		break;
	case 33:
		sb->Append((const UTF8Char*)"Power PC 601");
		break;
	case 34:
		sb->Append((const UTF8Char*)"Power PC 603");
		break;
	case 35:
		sb->Append((const UTF8Char*)"Power PC 603+");
		break;
	case 36:
		sb->Append((const UTF8Char*)"Power PC 604");
		break;
	case 37:
		sb->Append((const UTF8Char*)"Power PC 620");
		break;
	case 38:
		sb->Append((const UTF8Char*)"Power PC x704");
		break;
	case 39:
		sb->Append((const UTF8Char*)"Power PC 750");
		break;
	case 40:
		sb->Append((const UTF8Char*)"Intel Core Duo processor");
		break;
	case 41:
		sb->Append((const UTF8Char*)"Intel Core Duo mobile processor");
		break;
	case 42:
		sb->Append((const UTF8Char*)"Intel Core Solo mobile processor");
		break;
	case 43:
		sb->Append((const UTF8Char*)"Intel Atom processor");
		break;
	case 44:
		sb->Append((const UTF8Char*)"Intel Core M processor");
		break;
	case 45:
		sb->Append((const UTF8Char*)"Intel Core m3 processor");
		break;
	case 46:
		sb->Append((const UTF8Char*)"Intel Core m5 processor");
		break;
	case 47:
		sb->Append((const UTF8Char*)"Intel Core m7 processor");
		break;
	case 48:
		sb->Append((const UTF8Char*)"Alpha Family");
		break;
	case 49:
		sb->Append((const UTF8Char*)"Alpha 21064");
		break;
	case 50:
		sb->Append((const UTF8Char*)"Alpha 21066");
		break;
	case 51:
		sb->Append((const UTF8Char*)"Alpha 21164");
		break;
	case 52:
		sb->Append((const UTF8Char*)"Alpha 21164PC");
		break;
	case 53:
		sb->Append((const UTF8Char*)"Alpha 21164a");
		break;
	case 54:
		sb->Append((const UTF8Char*)"Alpha 21264");
		break;
	case 55:
		sb->Append((const UTF8Char*)"Alpha 21364");
		break;
	case 56:
		sb->Append((const UTF8Char*)"AMD Turion II Ultra Dual-Core Mobile M Processor Family");
		break;
	case 57:
		sb->Append((const UTF8Char*)"AMD Turion II Dual-Core Mobile M Processor Family");
		break;
	case 58:
		sb->Append((const UTF8Char*)"AMD Athlon II Dual-Core M Processor Family");
		break;
	case 59:
		sb->Append((const UTF8Char*)"AMD Opteron 6100 Series Processor");
		break;
	case 60:
		sb->Append((const UTF8Char*)"AMD Opteron 4100 Series Processor");
		break;
	case 61:
		sb->Append((const UTF8Char*)"AMD Opteron 6200 Series Processor");
		break;
	case 62:
		sb->Append((const UTF8Char*)"AMD Opteron 4200 Series Processor");
		break;
	case 63:
		sb->Append((const UTF8Char*)"AMD FX Series Processor");
		break;
	case 64:
		sb->Append((const UTF8Char*)"MIPS Family");
		break;
	case 65:
		sb->Append((const UTF8Char*)"MIPS R4000");
		break;
	case 66:
		sb->Append((const UTF8Char*)"MIPS R4200");
		break;
	case 67:
		sb->Append((const UTF8Char*)"MIPS R4400");
		break;
	case 68:
		sb->Append((const UTF8Char*)"MIPS R4600");
		break;
	case 69:
		sb->Append((const UTF8Char*)"MIPS R10000");
		break;
	case 70:
		sb->Append((const UTF8Char*)"AMD C-Series Processor");
		break;
	case 71:
		sb->Append((const UTF8Char*)"AMD E-Series Processor");
		break;
	case 72:
		sb->Append((const UTF8Char*)"AMD A-Series Processor");
		break;
	case 73:
		sb->Append((const UTF8Char*)"AMD G-Series Processor");
		break;
	case 74:
		sb->Append((const UTF8Char*)"AMD Z-Series Processor");
		break;
	case 75:
		sb->Append((const UTF8Char*)"AMD R-Series Processor");
		break;
	case 76:
		sb->Append((const UTF8Char*)"AMD Opteron 4300 Series Processor");
		break;
	case 77:
		sb->Append((const UTF8Char*)"AMD Opteron 6300 Series Processor");
		break;
	case 78:
		sb->Append((const UTF8Char*)"AMD Opteron 3300 Series Processor");
		break;
	case 79:
		sb->Append((const UTF8Char*)"AMD FirePro Series Processor");
		break;
	case 80:
		sb->Append((const UTF8Char*)"SPARC Family");
		break;
	case 81:
		sb->Append((const UTF8Char*)"SuperSPARC");
		break;
	case 82:
		sb->Append((const UTF8Char*)"microSPARC II");
		break;
	case 83:
		sb->Append((const UTF8Char*)"microSPARC IIep");
		break;
	case 84:
		sb->Append((const UTF8Char*)"UltraSPARC");
		break;
	case 85:
		sb->Append((const UTF8Char*)"UltraSPARC II");
		break;
	case 86:
		sb->Append((const UTF8Char*)"UltraSPARC Iii");
		break;
	case 87:
		sb->Append((const UTF8Char*)"UltraSPARC III");
		break;
	case 88:
		sb->Append((const UTF8Char*)"UltraSPARC IIIi");
		break;
	case 96:
		sb->Append((const UTF8Char*)"68040 Family");
		break;
	case 97:
		sb->Append((const UTF8Char*)"68xxx");
		break;
	case 98:
		sb->Append((const UTF8Char*)"68000");
		break;
	case 99:
		sb->Append((const UTF8Char*)"68010");
		break;
	case 100:
		sb->Append((const UTF8Char*)"68020");
		break;
	case 101:
		sb->Append((const UTF8Char*)"68030");
		break;
	case 102:
		sb->Append((const UTF8Char*)"AMD Athlon X4 Quad-Core Processor Family");
		break;
	case 103:
		sb->Append((const UTF8Char*)"AMD Opteron X1000 Series Processor");
		break;
	case 104:
		sb->Append((const UTF8Char*)"AMD Opteron X2000 Series Processor");
		break;
	case 105:
		sb->Append((const UTF8Char*)"AMD Opteron A-Series Processor");
		break;
	case 106:
		sb->Append((const UTF8Char*)"AMD Opteron X3000 Series APU");
		break;
	case 107:
		sb->Append((const UTF8Char*)"AMD Zen Processor Family");
		break;
	case 112:
		sb->Append((const UTF8Char*)"Hobbit Family");
		break;
	case 120:
		sb->Append((const UTF8Char*)"Crusoe TM5000 Family");
		break;
	case 121:
		sb->Append((const UTF8Char*)"Crusoe TM3000 Family");
		break;
	case 122:
		sb->Append((const UTF8Char*)"Efficeon TM8000 Family");
		break;
	case 128:
		sb->Append((const UTF8Char*)"Weitek");
		break;
	case 130:
		sb->Append((const UTF8Char*)"Itanium processor");
		break;
	case 131:
		sb->Append((const UTF8Char*)"AMD Athlon 64 Processor Family");
		break;
	case 132:
		sb->Append((const UTF8Char*)"AMD Opteron Processor Family");
		break;
	case 133:
		sb->Append((const UTF8Char*)"AMD Sempron Processor Family");
		break;
	case 134:
		sb->Append((const UTF8Char*)"AMD Turion 64 Mobile Technology");
		break;
	case 135:
		sb->Append((const UTF8Char*)"Dual-Core AMD Operton Processor Family");
		break;
	case 136:
		sb->Append((const UTF8Char*)"AMD Athlon 64 X2 Dual-Core Processor Family");
		break;
	case 137:
		sb->Append((const UTF8Char*)"AMD Turion 64 X2 Mobile Technology");
		break;
	case 138:
		sb->Append((const UTF8Char*)"Quad-Core AMD Opteron Processor Family");
		break;
	case 139:
		sb->Append((const UTF8Char*)"Third-Generation AMD Opteron Processor Family");
		break;
	case 140:
		sb->Append((const UTF8Char*)"AMD Phenom FX Quad-Core Processor Family");
		break;
	case 141:
		sb->Append((const UTF8Char*)"AMD Phenom X4 Quad-Core Processor Family");
		break;
	case 142:
		sb->Append((const UTF8Char*)"AMD Phenom X2 Dual-Core Processor Family");
		break;
	case 143:
		sb->Append((const UTF8Char*)"AMD Athlon X2 Dual-Core Processor Family");
		break;
	case 144:
		sb->Append((const UTF8Char*)"PA-RISC Family");
		break;
	case 145:
		sb->Append((const UTF8Char*)"PA-RISC 8500");
		break;
	case 146:
		sb->Append((const UTF8Char*)"PA-RISC 8000");
		break;
	case 147:
		sb->Append((const UTF8Char*)"PA-RISC 7300LC");
		break;
	case 148:
		sb->Append((const UTF8Char*)"PA-RISC 7200");
		break;
	case 149:
		sb->Append((const UTF8Char*)"PA-RISC 7100LC");
		break;
	case 150:
		sb->Append((const UTF8Char*)"PA-RISC 7100");
		break;
	case 160:
		sb->Append((const UTF8Char*)"V30 Family");
		break;
	case 161:
		sb->Append((const UTF8Char*)"Quad-Core Intel Xeon processor 3200 Series");
		break;
	case 162:
		sb->Append((const UTF8Char*)"Dual-Core Intel Xeon processor 3000 Series");
		break;
	case 163:
		sb->Append((const UTF8Char*)"Quad-Core Intel Xeon processor 5300 Series");
		break;
	case 164:
		sb->Append((const UTF8Char*)"Dual-Core Intel Xeon processor 5100 Series");
		break;
	case 165:
		sb->Append((const UTF8Char*)"Dual-Core Intel Xeon processor 5000 Series");
		break;
	case 166:
		sb->Append((const UTF8Char*)"Dual-Core Intel Xeon processor LV");
		break;
	case 167:
		sb->Append((const UTF8Char*)"Dual-Core Intel Xeon processor ULV");
		break;
	case 168:
		sb->Append((const UTF8Char*)"Dual-Core Intel Xeon processor 7100 Series");
		break;
	case 169:
		sb->Append((const UTF8Char*)"Quad-Core Intel Xeon processor 5400 Series");
		break;
	case 170:
		sb->Append((const UTF8Char*)"Quad-Core Intel Xeon processor");
		break;
	case 171:
		sb->Append((const UTF8Char*)"Dual-Core Intel Xeon processor 5200 Series");
		break;
	case 172:
		sb->Append((const UTF8Char*)"Dual-Core Intel Xeon processor 7200 Series");
		break;
	case 173:
		sb->Append((const UTF8Char*)"Quad-Core Intel Xeon processor 7300 Series");
		break;
	case 174:
		sb->Append((const UTF8Char*)"Quad-Core Intel Xeon processor 7400 Series");
		break;
	case 175:
		sb->Append((const UTF8Char*)"Multi-Core Intel Xeon processor 7400 Series");
		break;
	case 176:
		sb->Append((const UTF8Char*)"Pentium III Xeon processor");
		break;
	case 177:
		sb->Append((const UTF8Char*)"Pentium III Processor with Intel SpeedStep Technology");
		break;
	case 178:
		sb->Append((const UTF8Char*)"Pentium 4 Processor");
		break;
	case 179:
		sb->Append((const UTF8Char*)"Intel Xeon processor");
		break;
	case 180:
		sb->Append((const UTF8Char*)"AS400 Family");
		break;
	case 181:
		sb->Append((const UTF8Char*)"Intel Xeon processor MP");
		break;
	case 182:
		sb->Append((const UTF8Char*)"AMD Athlon XP Processor Family");
		break;
	case 183:
		sb->Append((const UTF8Char*)"AMD Athlon MP Processor Family");
		break;
	case 184:
		sb->Append((const UTF8Char*)"Intel Itanium 2 processor");
		break;
	case 185:
		sb->Append((const UTF8Char*)"Intel Pentium M processor");
		break;
	case 186:
		sb->Append((const UTF8Char*)"Intel Celeron D processor");
		break;
	case 187:
		sb->Append((const UTF8Char*)"Intel Pentium D processor");
		break;
	case 188:
		sb->Append((const UTF8Char*)"Intel Pentium Processor Extreme Edition");
		break;
	case 189:
		sb->Append((const UTF8Char*)"Intel Core Solo processor");
		break;
	case 191:
		sb->Append((const UTF8Char*)"Intel Core 2 Duo processor");
		break;
	case 192:
		sb->Append((const UTF8Char*)"Intel Core 2 Solo processor");
		break;
	case 193:
		sb->Append((const UTF8Char*)"Intel Core 2 Extreme processor");
		break;
	case 194:
		sb->Append((const UTF8Char*)"Intel Core 2 Quad processor");
		break;
	case 195:
		sb->Append((const UTF8Char*)"Intel Core 2 Extreme mobile processor");
		break;
	case 196:
		sb->Append((const UTF8Char*)"Intel Core 2 Duo mobile processor");
		break;
	case 197:
		sb->Append((const UTF8Char*)"Intel Core 2 Solo mobile processor");
		break;
	case 198:
		sb->Append((const UTF8Char*)"Intel Core i7 processor");
		break;
	case 199:
		sb->Append((const UTF8Char*)"Dual-Core Intel Celeron processor");
		break;
	case 200:
		sb->Append((const UTF8Char*)"IBM390 Family");
		break;
	case 201:
		sb->Append((const UTF8Char*)"G4");
		break;
	case 202:
		sb->Append((const UTF8Char*)"G5");
		break;
	case 203:
		sb->Append((const UTF8Char*)"ESA/390 G6");
		break;
	case 204:
		sb->Append((const UTF8Char*)"z/Architecture base");
		break;
	case 205:
		sb->Append((const UTF8Char*)"Intel Core i5 processor");
		break;
	case 206:
		sb->Append((const UTF8Char*)"Intel Core i3 processor");
		break;
	case 210:
		sb->Append((const UTF8Char*)"VIA C7-M Processor Family");
		break;
	case 211:
		sb->Append((const UTF8Char*)"VIA C7-D Processor Family");
		break;
	case 212:
		sb->Append((const UTF8Char*)"VIA C7 Processor Family");
		break;
	case 213:
		sb->Append((const UTF8Char*)"VIA Eden Processor Family");
		break;
	case 214:
		sb->Append((const UTF8Char*)"Multi-Core Intel Xeon processor");
		break;
	case 215:
		sb->Append((const UTF8Char*)"Dual-Core Intel Xeon processor 3xxx Series");
		break;
	case 216:
		sb->Append((const UTF8Char*)"Quad-Core Intel Xeon processor 3xxx Series");
		break;
	case 217:
		sb->Append((const UTF8Char*)"VIA Nano Processor Family");
		break;
	case 218:
		sb->Append((const UTF8Char*)"Dual-Core Intel Xeon processor 5xxx Series");
		break;
	case 219:
		sb->Append((const UTF8Char*)"Quad-Core Intel Xeon processor 5xxx Series");
		break;
	case 221:
		sb->Append((const UTF8Char*)"Dual-Core Intel Xeon processor 7xxx Series");
		break;
	case 222:
		sb->Append((const UTF8Char*)"Quad-Core Intel Xeon processor 7xxx Series");
		break;
	case 223:
		sb->Append((const UTF8Char*)"Multi-Core Intel Xeon processor 7xxx Series");
		break;
	case 224:
		sb->Append((const UTF8Char*)"Multi-Core Intel Xeon processor 3400 Series");
		break;
	case 228:
		sb->Append((const UTF8Char*)"AMD Opteron 3000 Series Processor");
		break;
	case 229:
		sb->Append((const UTF8Char*)"AMD Sempron II Processor");
		break;
	case 230:
		sb->Append((const UTF8Char*)"Embedded AMD Opteron Quad-Core Processor Family");
		break;
	case 231:
		sb->Append((const UTF8Char*)"AMD Phenom Triple-Core Processor Family");
		break;
	case 232:
		sb->Append((const UTF8Char*)"AMD Turion Ultra Dual-Core Mobile Processor Family");
		break;
	case 233:
		sb->Append((const UTF8Char*)"AMD Turion Dual-Core Mobile Processor Family");
		break;
	case 234:
		sb->Append((const UTF8Char*)"AMD Athlon Dual-Core Processor Family");
		break;
	case 235:
		sb->Append((const UTF8Char*)"AMD Sempron SI Processor Family");
		break;
	case 236:
		sb->Append((const UTF8Char*)"AMD Phenom II Processor Family");
		break;
	case 237:
		sb->Append((const UTF8Char*)"AMD Athlon II Processor Family");
		break;
	case 238:
		sb->Append((const UTF8Char*)"Six-Core AMD Opteron Processor Family");
		break;
	case 239:
		sb->Append((const UTF8Char*)"AMD Sempron M Processor Family");
		break;
	case 250:
		sb->Append((const UTF8Char*)"i860");
		break;
	case 251:
		sb->Append((const UTF8Char*)"i960");
		break;
	case 254:
		sb->Append((const UTF8Char*)"See Processor Family 2");
		break;
	case 256:
		sb->Append((const UTF8Char*)"ARMv7");
		break;
	case 257:
		sb->Append((const UTF8Char*)"ARMv8");
		break;
	case 260:
		sb->Append((const UTF8Char*)"SH-3");
		break;
	case 261:
		sb->Append((const UTF8Char*)"SH-4");
		break;
	case 280:
		sb->Append((const UTF8Char*)"ARM");
		break;
	case 281:
		sb->Append((const UTF8Char*)"StrongARM");
		break;
	case 300:
		sb->Append((const UTF8Char*)"6x86");
		break;
	case 301:
		sb->Append((const UTF8Char*)"MediaGX");
		break;
	case 302:
		sb->Append((const UTF8Char*)"MII");
		break;
	case 320:
		sb->Append((const UTF8Char*)"WinChip");
		break;
	case 350:
		sb->Append((const UTF8Char*)"DSP");
		break;
	case 500:
		sb->Append((const UTF8Char*)"VideoProcessor");
		break;
	default:
		sb->Append((const UTF8Char*)"Undefined (");
		sb->AppendI32(family);
		sb->Append((const UTF8Char*)")");
	}
}

void Win32::SMBIOS::GetConnectorType(Text::StringBuilderUTF *sb, UInt8 type)
{
	switch (type)
	{
	case 0:
		sb->Append((const UTF8Char*)"None");
		break;
	case 1:
		sb->Append((const UTF8Char*)"Centronics");
		break;
	case 2:
		sb->Append((const UTF8Char*)"Mini Centronics");
		break;
	case 3:
		sb->Append((const UTF8Char*)"Proprietary");
		break;
	case 4:
		sb->Append((const UTF8Char*)"DB-25 pin male");
		break;
	case 5:
		sb->Append((const UTF8Char*)"DB-25 pin female");
		break;
	case 6:
		sb->Append((const UTF8Char*)"DB-15 pin male");
		break;
	case 7:
		sb->Append((const UTF8Char*)"DB-15 pin female");
		break;
	case 8:
		sb->Append((const UTF8Char*)"DB-9 pin male");
		break;
	case 9:
		sb->Append((const UTF8Char*)"DB-9 pin female");
		break;
	case 10:
		sb->Append((const UTF8Char*)"RJ-11");
		break;
	case 11:
		sb->Append((const UTF8Char*)"RJ-45");
		break;
	case 12:
		sb->Append((const UTF8Char*)"50-pin MiniSCSI");
		break;
	case 13:
		sb->Append((const UTF8Char*)"Mini-DIN");
		break;
	case 14:
		sb->Append((const UTF8Char*)"Micro-DIN");
		break;
	case 15:
		sb->Append((const UTF8Char*)"PS/2");
		break;
	case 16:
		sb->Append((const UTF8Char*)"Infrared");
		break;
	case 17:
		sb->Append((const UTF8Char*)"HP-HIL");
		break;
	case 18:
		sb->Append((const UTF8Char*)"Access Bus (USB)");
		break;
	case 19:
		sb->Append((const UTF8Char*)"SSA SCSI");
		break;
	case 20:
		sb->Append((const UTF8Char*)"Circular DIN-8 male");
		break;
	case 21:
		sb->Append((const UTF8Char*)"Circular DIN-8 female");
		break;
	case 22:
		sb->Append((const UTF8Char*)"On Board IDE");
		break;
	case 23:
		sb->Append((const UTF8Char*)"On Board Floppy");
		break;
	case 24:
		sb->Append((const UTF8Char*)"9-pin Dual Inline (pin 10 cut)");
		break;
	case 25:
		sb->Append((const UTF8Char*)"25-pin Dual Inline (pin 26 cut)");
		break;
	case 26:
		sb->Append((const UTF8Char*)"50-pin Dual Inline");
		break;
	case 27:
		sb->Append((const UTF8Char*)"68-pin Dual Inline");
		break;
	case 28:
		sb->Append((const UTF8Char*)"On Board Sound Input from CD-ROM");
		break;
	case 29:
		sb->Append((const UTF8Char*)"Mini-Centronics Type-14");
		break;
	case 30:
		sb->Append((const UTF8Char*)"Mini-Centronics Type-26");
		break;
	case 31:
		sb->Append((const UTF8Char*)"Mini-jack (headphones)");
		break;
	case 32:
		sb->Append((const UTF8Char*)"BNC");
		break;
	case 33:
		sb->Append((const UTF8Char*)"1394");
		break;
	case 34:
		sb->Append((const UTF8Char*)"SAS/SATA Plug Receptacle");
		break;
	case 160:
		sb->Append((const UTF8Char*)"PC-98");
		break;
	case 161:
		sb->Append((const UTF8Char*)"PC-98Hireso");
		break;
	case 162:
		sb->Append((const UTF8Char*)"PC-H98");
		break;
	case 163:
		sb->Append((const UTF8Char*)"PC-98Note");
		break;
	case 164:
		sb->Append((const UTF8Char*)"PC-98Full");
		break;
	case 255:
		sb->Append((const UTF8Char*)"Other");
		break;
	default:
		sb->Append((const UTF8Char*)"Undefined (");
		sb->AppendU16(type);
		sb->Append((const UTF8Char*)")");
		break;
	}
}
