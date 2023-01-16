#include "Stdafx.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "Data/UUID.h"
#include "IO/SMBIOS.h"
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
	switch (pack->packType)
	{
	case 0:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Vendor"));
		AddString(frame, 5, packBuff, carr, CSTR("BIOS Version"));
		AddHex16(frame, 6, packBuff, carr, CSTR("BIOS Starting Address Segment"));
		AddString(frame, 8, packBuff, carr, CSTR("BIOS Release Date"));
		if (packBuff[9] == 0xff)
		{
			frame->AddField(9, 1, CSTR("BIOS ROM Size"), CSTR("16MB or greater"));
		}
		else
		{
			sptr = Text::StrConcatC(Text::StrUOSInt(sbuff, ((UOSInt)packBuff[9] + 1) * 64), UTF8STRC("KB"));
			frame->AddField(9, 1, CSTR("BIOS ROM Size"), CSTRP(sbuff, sptr));
		}
		const Char *names0_1[] = {"Reserved", "Reserved", "Unknown", "BIOS Characteristics are not supported", "ISA is supported", "MCA is supported", "EISA is supported", "PCI is supported"};
		AddBits(frame, 10, packBuff, carr, names0_1);
		const Char *names0_2[] = {"PC card (PCMCIA) is supported", "Plug and Play is supported", "APM is supported", "BIOS is upgradeable (Flash)", "BIOS shadowing is allowed", "VL-VESA is supported", "ESCD support is available", "Boot from CD is supported"};
		AddBits(frame, 11, packBuff, carr, names0_2);
		const Char *names0_3[] = {"Selectable boot is supported", "BIOS ROM is socketed (e.g. PLCC or SOP socket)", "Boot from PC card (PCMCIA) is supported", "EDD specification is supported", "Int 13h — Japanese floppy for NEC 9800 1.2 MB (3.5\", 1K bytes/sector, 360 RPM) is supported", "Int 13h — Japanese floppy for Toshiba 1.2 MB (3.5\", 360 RPM) is supported", "Int 13h — 5.25\" / 360 KB floppy services are supported", "Int 13h — 5.25\" /1.2 MB floppy services are supported"};
		AddBits(frame, 12, packBuff, carr, names0_3);
		const Char *names0_4[] = {"Int 13h — 3.5\" / 720 KB floppy services are supported", "Int 13h — 3.5\" / 2.88 MB floppy services are supported", "Int 5h, print screen Service is supported", "Int 9h, 8042 keyboard services are supported", "Int 14h, serial services are supported", "Int 17h, printer services are supported", "Int 10h, CGA/Mono Video Services are supported", "NEC PC-98"};
		AddBits(frame, 13, packBuff, carr, names0_4);
		AddHex16(frame, 14, packBuff, carr, CSTR("Reserved for BIOS vendor"));
		AddHex16(frame, 16, packBuff, carr, CSTR("Reserved for system vendor"));
		const Char *names0_Ext1[] = {"ACPI is supported", "USB Legacy is supported", "AGP is supported", "I2O boot is supported", "LS-120 SuperDisk boot is supported", "ATAPI ZIP drive boot is supported", "1394 boot is supported", "Smart battery is supported"};
		AddBits(frame, 18, packBuff, carr, names0_Ext1);
		const Char *names0_Ext2[] = {"BIOS Boot Specification is supported", "Function key-initiated network service boot is supported", "Enable targeted content distribution", "UEFI Specification is supported", "SMBIOS table describes a virtual machine", "Manufacturing mode is supported", "Manufacturing mode is enabled", "Reserved for future assignment by this specification"};
		AddBits(frame, 19, packBuff, carr, names0_Ext2);
		AddUInt8(frame, 20, packBuff, carr, CSTR("System BIOS Major Release"));
		AddUInt8(frame, 21, packBuff, carr, CSTR("System BIOS Minor Release"));
		AddUInt8(frame, 22, packBuff, carr, CSTR("Embedded Controller Firmware Major Release"));
		AddUInt8(frame, 23, packBuff, carr, CSTR("Embedded Controller Firmware Minor Release"));
		if (packBuff[1] >= 26)
		{
			UInt16 sz = ReadUInt16(&packBuff[24]);
			sptr = Text::StrUOSInt(sbuff, sz & 0x3FFF);
			switch (sz >> 14)
			{
			case 0:
				sptr = Text::StrConcatC(sptr, UTF8STRC("MB"));
				break;
			case 1:
				sptr = Text::StrConcatC(sptr, UTF8STRC("GB"));
				break;
			}
			frame->AddField(24, 2, CSTR("Extended BIOS ROM Size"), CSTRP(sbuff, sptr));
		}
		break;
	}
	case 1:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Manufacturer"));
		AddString(frame, 5, packBuff, carr, CSTR("Product Name"));
		AddString(frame, 6, packBuff, carr, CSTR("Version"));
		AddString(frame, 7, packBuff, carr, CSTR("Serial Number"));
		AddUUID(frame, 8, packBuff, carr, CSTR("UUID"));
		const Char *names1_1[] = {"Reserved", "Other", "Unknown", "APM Timer", "Modem Ring", "LAN Remote", "Power Switch", "PCI PME#", "AC Power Restored"};
		AddEnum(frame, 24, packBuff, carr, CSTR("Wake-up Type"), names1_1, sizeof(names1_1) / sizeof(names1_1[0]));
		AddString(frame, 25, packBuff, carr, CSTR("SKU Number"));
		AddString(frame, 26, packBuff, carr, CSTR("Family"));
		break;
	}
	case 2:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Manufacturer"));
		AddString(frame, 5, packBuff, carr, CSTR("Product"));
		AddString(frame, 6, packBuff, carr, CSTR("Version"));
		AddString(frame, 7, packBuff, carr, CSTR("Serial Number"));
		AddString(frame, 8, packBuff, carr, CSTR("Asset Tag"));
		const Char *names2_1[] = {"the board is a hosting board", "the board requires at least one daughter board or auxiliary card to function properly", "the board is removable", "the board is replaceable", "the board is hot swappable", "reserved", "reserved", "reserved"};
		AddBits(frame, 9, packBuff, carr, names2_1);
		AddString(frame, 10, packBuff, carr, CSTR("Location in Chassis"));
		AddHex16(frame, 11, packBuff, carr, CSTR("Chassis Handle"));
		const Char *names2_2[] = {"Unused", "Unknown", "Other", "Server Blade", "Connectivity Switch", "System Management Module", "Processor Module", "I/O Module", "Memory Module", "Daughter board", "Motherboard", "Processor/Memory Module", "Processor/IO Module", "Interconnect board"};
		AddEnum(frame, 13, packBuff, carr, CSTR("Board Type"), names2_2, sizeof(names2_2) / sizeof(names2_2[0]));
		AddUInt8(frame, 14, packBuff, carr, CSTR("Number of Contained Object Handles"));
		if (packBuff[1] > 14)
		{
			k = 0;
			l = packBuff[14];
			while (k < l)
			{
				AddHex16(frame, 15 + k * 2, packBuff, carr, CSTR("Contained Object Handles"));
				k++;
			}
		}
		break;
	}
	case 3:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Manufacturer"));
		frame->AddBit(5, CSTR("Chassis lock is present"), packBuff[5], 7);
		const Char *names3_1[] = {"Unspecified", "Other", "Unknown", "Desktop", "Low Profile Desktop", "Pizza Box", "Mini Tower", "Tower",
			"Portable", "Laptop", "Notebook", "Hand Held", "Docking Station", "All in One", "Sub Notebook", "Space-saving",
			"Lunch Box", "Main Server Chassis", "Expansion Chassis", "SubChassis", "Bus Expansion Chassis", "Peripheral Chassis", "RAID Chassis", "Rack Mount Chassis",
			"Sealed-case PC", "Multi-system chassis", "Compact PCI", "Advanced TCA", "Blade", "Blade Enclosure", "Tablet", "Convertible",
			"Detachable", "IoT Gateway", "Embedded PC", "Mini PC", "Stick PC"};
		AddEnum(frame, 5, (UInt8)(packBuff[5] & 0x7f), carr, CSTR("Type"), names3_1, sizeof(names3_1) / sizeof(names3_1[0]));
		AddString(frame, 6, packBuff, carr, CSTR("Version"));
		AddString(frame, 7, packBuff, carr, CSTR("Serial Number"));
		AddString(frame, 8, packBuff, carr, CSTR("Asset Tag Number"));
		const Char *names3_2[] = {"Unspecified", "Other", "Unknown", "Safe", "Warning", "Critical", "Non-recoverable"};
		AddEnum(frame, 9, packBuff, carr, CSTR("Boot-up State"), names3_2, sizeof(names3_2) / sizeof(names3_2[0]));
		AddEnum(frame, 10, packBuff, carr, CSTR("Power Supply State"), names3_2, sizeof(names3_2) / sizeof(names3_2[0]));
		AddEnum(frame, 11, packBuff, carr, CSTR("Thermal State"), names3_2, sizeof(names3_2) / sizeof(names3_2[0]));
		const Char *names3_3[] = {"Unspecified", "Other", "Unknown", "None", "External interface locked out", "External interface enabled"};
		AddEnum(frame, 12, packBuff, carr, CSTR("Security Status"), names3_3, sizeof(names3_3) / sizeof(names3_3[0]));
		AddHex32(frame, 13, packBuff, carr, CSTR("OEM-defined"));
		AddUInt8(frame, 17, packBuff, carr, CSTR("Height (U)"));
		AddUInt8(frame, 18, packBuff, carr, CSTR("Number of Power Cords"));
		AddUInt8(frame, 19, packBuff, carr, CSTR("Contained Element Count (n)"));
		AddUInt8(frame, 20, packBuff, carr, CSTR("Contained Elemen Record Length (m)"));
		///////////////////////////////////////////
		if (packBuff[1] > 20)
		{
			AddString(frame, 21 + (packBuff[19] * (UOSInt)packBuff[20]), packBuff, carr, CSTR("SKU Number"));
		}
		break;
	}
	case 4:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Socket Designation"));
		const Char *names4_1[] = {"Unspecified", "Other", "Unknown", "Central Processor", "Math Processor", "DSP Processor", "Video Processor"};
		AddEnum(frame, 5, packBuff, carr, CSTR("Processor Type"), names4_1, sizeof(names4_1) / sizeof(names4_1[0]));
		frame->AddUIntName(6, 1, CSTR("Processor Family"), packBuff[6], IO::SMBIOS::GetProcessorFamily(packBuff[6]));
		AddString(frame, 7, packBuff, carr, CSTR("Processor Manufacturer"));
		AddHex64(frame, 8, packBuff, carr, CSTR("Processor ID"));
		AddString(frame, 16, packBuff, carr, CSTR("Processor Version"));
		///////////////////////////////////////////
		break;
	}
	case 8:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Internal Reference Designator"));
		if (packBuff[1] > 5) frame->AddUIntName(5, 1, CSTR("Internal Connector Type"), packBuff[5], IO::SMBIOS::GetConnectorType(packBuff[5]));
		AddString(frame, 6, packBuff, carr, CSTR("External Reference Designator"));
		if (packBuff[1] > 7) frame->AddUIntName(7, 1, CSTR("External Connector Type"), packBuff[7], IO::SMBIOS::GetConnectorType(packBuff[7]));
		if (packBuff[1] > 8) frame->AddUIntName(8, 1, CSTR("Port Type"), packBuff[8], IO::SMBIOS::GetPortType(packBuff[8]));
		break;
	}
	case 10:
	{
		const Char *names10_1[] = {"Unspecified", "Other", "Unknown", "Video", "SCSI Controller", "Ethernet", "Token Ring", "Sound", "PATA Controller", "SATA Controller", "SAS Controller"};
		k = 4;
		l = packBuff[1];
		while (k < l)
		{
			frame->AddBit(k, CSTR("Device Type"), packBuff[k], 7);
			AddEnum(frame, k, packBuff[k], carr, CSTR("Processor Type"), names10_1, sizeof(names10_1) / sizeof(names10_1[0]));
			AddString(frame, k + 1, packBuff, carr, CSTR("Description String"));
			k += 2;
		}
		break;
	}
	case 11:
		AddUInt8(frame, 4, packBuff, carr, CSTR("Count"));
		break;
	case 12:
		AddUInt8(frame, 4, packBuff, carr, CSTR("Count"));
		break;
	case 13:
	{
		AddUInt8(frame, 4, packBuff, carr, CSTR("Installable Languages"));
		const Char *names13_1[] = {"Current Language strings use the abbreviated format", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"};
		AddBits(frame, 5, packBuff, carr, names13_1);
		if (packBuff[1] > 20) frame->AddHexBuff(6, 15, CSTR("Reserved"), &packBuff[6], false);
		AddString(frame, 21, packBuff, carr, CSTR("Current Language"));
		break;
	}
	case 22:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Location"));
		AddString(frame, 5, packBuff, carr, CSTR("Manufacturer"));
		AddString(frame, 6, packBuff, carr, CSTR("Manufacture Date"));
		AddString(frame, 7, packBuff, carr, CSTR("Serial Number"));
		AddString(frame, 8, packBuff, carr, CSTR("Device Name"));
		const Char *names22_1[] = {"Unspecified", "Other", "Unknown", "Lead Acid", "Nickel Cadmium", "Nickel metal hydride", "Lithium-ion", "Zinc air", "Lithium Polymer"};
		AddEnum(frame, 9, packBuff, carr, CSTR("Device Chemistry"), names22_1, sizeof(names22_1) / sizeof(names22_1[0]));
		UOSInt dcMul = 1;
		if (packBuff[1] > 21) dcMul = packBuff[21];
		if (packBuff[1] > 11) frame->AddUInt(10, 2, CSTR("Design Capacity (mWh)"), ReadUInt16(&packBuff[10]) * dcMul);
		if (packBuff[1] > 13) frame->AddUInt(12, 2, CSTR("Design Voltage (mV)"), ReadUInt16(&packBuff[12]));
		AddString(frame, 14, packBuff, carr, CSTR("SBDS Version Number"));
		AddUInt8(frame, 15, packBuff, carr, CSTR("Maximum Error in Battery Data (%)"));
		AddHex16(frame, 16, packBuff, carr, CSTR("SBDS Serial Number"));
		AddDate(frame, 18, packBuff, carr, CSTR("SBDS Manufacture Date"));
		AddString(frame, 20, packBuff, carr, CSTR("SBDS Device Chemistry"));
		AddUInt8(frame, 21, packBuff, carr, CSTR("Design Capacity Multiplier"));
		AddHex32(frame, 22, packBuff, carr, CSTR("OEM-specific"));
		break;
	}
	case 31:
		frame->AddHexBuff(4, packBuff[1] - 4, CSTR("Boot Integrity Services (BIS)"), &packBuff[4], true);
		break;
	case 32:
		if (packBuff[1] > 9) frame->AddHexBuff(4, 6, CSTR("Reserved"), &packBuff[4], false);
		if (packBuff[1] > 10)
		{
			frame->AddUIntName(10, 1, CSTR("Boot Status"), packBuff[10], IO::SMBIOS::GetSystemBootStatus(packBuff[10]));
		}
		break;
	}
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

void IO::FileAnalyse::SMBIOSFileAnalyse::AddString(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name)
{
	if (ofst >= packBuff[1])
		return;
	UInt8 ind = packBuff[ofst];
	Text::CString val;
	if (ind >= 32)
	{
		val = CSTR_NULL;
	}
	else
	{
		val = carr[ind];
	}
	if (val.v)
	{
		frame->AddUIntName(ofst, 1, name, ind, val);
	}
	else
	{
		frame->AddUIntName(ofst, 1, name, ind, CSTR("-"));
	}
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddHex16(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name)
{
	if (ofst + 1 >= packBuff[1])
		return;
	frame->AddHex16(ofst, name, ReadUInt16(&packBuff[ofst]));
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddHex32(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name)
{
	if (ofst + 3 >= packBuff[1])
		return;
	frame->AddHex32(ofst, name, ReadUInt16(&packBuff[ofst]));
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddHex64(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name)
{
	if (ofst + 7 >= packBuff[1])
		return;
	frame->AddHex64(ofst, name, ReadUInt16(&packBuff[ofst]));
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddUInt8(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name)
{
	if (ofst >= packBuff[1])
		return;
	frame->AddUInt(ofst, 1, name, packBuff[ofst]);
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddUUID(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name)
{
	if (ofst + 15 >= packBuff[1])
		return;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Data::UUID uuid(&packBuff[ofst]);
	sptr = uuid.ToString(sbuff);
	frame->AddField(ofst, 16, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddDate(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name)
{
	if (ofst + 1 >= packBuff[1])
		return;
	UInt16 val = ReadUInt16(&packBuff[ofst]);
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if (val == 0)
	{
		frame->AddField(ofst, 2, name, CSTR("-"));
	}
	else
	{
		sptr = Text::StrUInt16(sbuff, (UInt16)((val >> 9) + 1980));
		*sptr++ = '-';
		sptr = Text::StrUInt16(sptr, (UInt16)((val >> 5) & 15));
		*sptr++ = '-';
		sptr = Text::StrUInt16(sptr, (UInt16)(val & 31));
		frame->AddField(ofst, 2, name, CSTRP(sbuff, sptr));
	}
}
			
void IO::FileAnalyse::SMBIOSFileAnalyse::AddBits(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, const Char *bitNames[])
{
	if (ofst >= packBuff[1])
		return;
	UOSInt i = 0;
	UInt8 val = packBuff[ofst];
	while (i < 8)
	{
		frame->AddBit(ofst, Text::CString::FromPtr((const UTF8Char*)bitNames[i]), val, i);
		i++;
	}
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddEnum(FrameDetail *frame, UOSInt ofst, const UInt8 *packBuff, Text::CString *carr, Text::CString name, const Char *names[], UOSInt namesCnt)
{
	if (ofst >= packBuff[1])
		return;
	AddEnum(frame, ofst, packBuff[ofst], carr, name, names, namesCnt);
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddEnum(FrameDetail *frame, UOSInt ofst, UInt8 val, Text::CString *carr, Text::CString name, const Char *names[], UOSInt namesCnt)
{
	if (val >= namesCnt)
	{
		frame->AddUIntName(ofst, 1, name, val, CSTR("Unknown"));
	}
	else
	{
		frame->AddUIntName(ofst, 1, name, val, Text::CString::FromPtr((const UTF8Char*)names[val]));
	}
}
