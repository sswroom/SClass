#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/ArrayListInt32.h"
#include "IO/FileAnalyse/SMBIOSFileAnalyse.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::FileAnalyse::SMBIOSFileAnalyse::SMBIOSFileAnalyse(IO::IStreamData *fd)
{
	UInt8 *buff;
	UOSInt leng = (UOSInt)fd->GetDataSize();
	this->fd = 0;
	if (leng < 256 || leng > 1048576)
	{
		return;
	}
	buff = MemAlloc(UInt8, leng);
	if (fd->GetRealData(0, leng, buff) != leng)
	{
		MemFree(buff);
		return;
	}
	this->fd = fd->GetPartialData(0, leng);
	PackInfo *pack;
	UOSInt fileOfst = 0;
	UOSInt i;
	UOSInt j;
	while (fileOfst < leng)
	{
		i = buff[fileOfst + 1];
		while (fileOfst + i < leng)
		{
			j = Text::StrCharCnt(&buff[fileOfst + i]);
			i += j + 1;
			if (buff[fileOfst + i] == 0)
			{
				i++;
				break;
			}
		}
		pack = MemAlloc(PackInfo, 1);
		pack->fileOfst = fileOfst;
		pack->packSize = i;
		pack->packType = buff[fileOfst];
		this->packs.Add(pack);
		fileOfst += i;
		if (pack->packType == 127)
		{
			break;
		}
	}
	MemFree(buff);
}

IO::FileAnalyse::SMBIOSFileAnalyse::~SMBIOSFileAnalyse()
{
	SDEL_CLASS(this->fd);
	LIST_FREE_FUNC(&this->packs, MemFree);
}

Text::CString IO::FileAnalyse::SMBIOSFileAnalyse::GetFormatName()
{
	return CSTR("SMBIOS");
}

UOSInt IO::FileAnalyse::SMBIOSFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::SMBIOSFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SMBIOSFileAnalyse::PackInfo *pack;
	pack = this->packs.GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendUOSInt(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->AppendU16(pack->packType);
	sb->AppendC(UTF8STRC(" ("));
	sb->Append(SMBIOSTypeGetName(pack->packType));
	sb->AppendC(UTF8STRC("), size="));
	sb->AppendUOSInt(pack->packSize);
	return true;
}

UOSInt IO::FileAnalyse::SMBIOSFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs.GetCount() - 1;
	OSInt k;
	PackInfo *pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItem((UOSInt)k);
		if (ofst < pack->fileOfst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->fileOfst + pack->packSize)
		{
			i = k + 1;
		}
		else
		{
			return (UOSInt)k;
		}
	}
	return INVALID_INDEX;
}

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::SMBIOSFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::FrameDetail *frame;
	PackInfo *pack;
	UInt8 *packBuff;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	pack = this->packs.GetItem(index);
	if (pack == 0)
		return 0;

	packBuff = MemAlloc(UInt8, pack->packSize);
	if (this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff) != pack->packSize)
	{
		MemFree(packBuff);
		return 0;
	}
	Text::CString carr[32];
	k = 32;
	while (k-- > 0)
	{
		carr[k] = CSTR_NULL;
	}
	l = packBuff[1];
	k = 1;
	if (packBuff[l] != 0)
	{
		k = 2;
		carr[1].v = &packBuff[l];
		while (true)
		{
			if (packBuff[l] == 0)
			{
				carr[k - 1].leng = (UOSInt)(&packBuff[l] - carr[k - 1].v);
				if (packBuff[l + 1] == 0)
					break;
				carr[k].v = &packBuff[l + 1];
				k++;
			}
			l++;
		}
	}


	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, (UInt32)pack->packSize));
	frame->AddUIntName(0, 1, CSTR("Type"), pack->packType, SMBIOSTypeGetName(pack->packType));
	frame->AddUInt(1, 1, CSTR("Length"), packBuff[1]);
	frame->AddHex16(2, CSTR("Handle"), ReadUInt16(&packBuff[2]));
	k = 1;
	while (carr[k].v)
	{
		sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("String ")), k);
		frame->AddField((UOSInt)(carr[k].v - packBuff), carr[k].leng + 1, CSTRP(sbuff, sptr), carr[k]);
		k++;
	}
	if (k == 1)
	{
		frame->AddHexBuff(packBuff[1], 2, CSTR("End of String Table"), &packBuff[packBuff[1]], false);
	}
	else
	{
		frame->AddUInt((UOSInt)(carr[k - 1].v - packBuff) + carr[k - 1].leng + 1, 1, CSTR("End of String Table"), 0);
	}
	MemFree(packBuff);
	return frame;
}

Bool IO::FileAnalyse::SMBIOSFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::SMBIOSFileAnalyse::IsParsing()
{
	return false;
}

Bool IO::FileAnalyse::SMBIOSFileAnalyse::TrimPadding(Text::CString outputFile)
{
	return false;
}

Text::CString IO::FileAnalyse::SMBIOSFileAnalyse::SMBIOSTypeGetName(UInt8 type)
{
	switch (type)
	{
	case 0:
		return CSTR("BIOS Information");
	case 1:
		return CSTR("System Information");
	case 2:
		return CSTR("Baseboard (or Module) Information");
	case 3:
		return CSTR("System Enclosure or Chassis");
	case 4:
		return CSTR("Processor Information");
	case 5:
		return CSTR("Memory Controller Information");
	case 6:
		return CSTR("Memory Module Information");
	case 7:
		return CSTR("Cache Information");
	case 8:
		return CSTR("Port Connector Information");
	case 9:
		return CSTR("System Slots");
	case 10:
		return CSTR("On Board Devices Information");
	case 11:
		return CSTR("OEM Strings");
	case 12:
		return CSTR("System Configuration Options");
	case 13:
		return CSTR("BIOS Language Information");
	case 14:
		return CSTR("Group Associations");
	case 15:
		return CSTR("System Event Log");
	case 16:
		return CSTR("Physical Memory Array");
	case 17:
		return CSTR("Memory Device");
	case 18:
		return CSTR("32-Bit Memory Error Information");
	case 19:
		return CSTR("Memory Array Mapped Address");
	case 20:
		return CSTR("Memory Device Mapped Address");
	case 21:
		return CSTR("Built-in Pointing Device");
	case 22:
		return CSTR("Portable Battery");
	case 23:
		return CSTR("System Reset");
	case 24:
		return CSTR("Hardware Security");
	case 25:
		return CSTR("System Power Controls");
	case 26:
		return CSTR("Voltage Probe");
	case 27:
		return CSTR("Cooling Device");
	case 28:
		return CSTR("Temperature Probe");
	case 29:
		return CSTR("Electrical Current Probe");
	case 30:
		return CSTR("Out-of-Band Remote Access");
	case 31:
		return CSTR("Boot Integrity Services (BIS) Entry Point");
	case 32:
		return CSTR("System Boot Information");
	case 33:
		return CSTR("64-Bit Memory Error Information");
	case 34:
		return CSTR("Management Device");
	case 35:
		return CSTR("Management Device Component");
	case 36:
		return CSTR("Management Device Threshold Data");
	case 37:
		return CSTR("Memory Channel");
	case 38:
		return CSTR("IPMI Device Information");
	case 39:
		return CSTR("System Power Supply");
	case 40:
		return CSTR("Additional Information");
	case 41:
		return CSTR("Onboard Devices Extended Information");
	case 42:
		return CSTR("Management Controller Host Interface");
	case 43:
		return CSTR("TPM Device");
	case 44:
		return CSTR("Processor Additional Information");
	case 45:
		return CSTR("Firmware Inventory Information");
	case 46:
		return CSTR("String Property");
	case 126:
		return CSTR("Inactive");
	case 127:
		return CSTR("End-of-Table");
	case 128:
		return CSTR("Apple Firmware Volume");
	case 130:
		return CSTR("Apple Memory SPD Data");
	case 131:
		return CSTR("Apple Processor Type");
	default:
		return CSTR("Unknown");
	}
}
