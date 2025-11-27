#include "Stdafx.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "Data/UUID.h"
#include "IO/SMBIOS.h"
#include "IO/FileAnalyse/SMBIOSFileAnalyse.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::FileAnalyse::SMBIOSFileAnalyse::SMBIOSFileAnalyse(NN<IO::StreamData> fd)
{
	UOSInt leng = (UOSInt)fd->GetDataSize();
	this->fd = 0;
	if (leng < 256 || leng > 1048576)
	{
		return;
	}
	Data::ByteBuffer buff(leng);
	if (fd->GetRealData(0, leng, buff) != leng)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, leng).Ptr();
	NN<PackInfo> pack;
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
		pack = MemAllocNN(PackInfo);
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
}

IO::FileAnalyse::SMBIOSFileAnalyse::~SMBIOSFileAnalyse()
{
	this->fd.Delete();
	this->packs.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::SMBIOSFileAnalyse::GetFormatName()
{
	return CSTR("SMBIOS");
}

UOSInt IO::FileAnalyse::SMBIOSFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::SMBIOSFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::SMBIOSFileAnalyse::PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
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
	NN<PackInfo> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItemNoCheck((UOSInt)k);
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::SMBIOSFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	NN<PackInfo> pack;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt k;
	UOSInt l;
	NN<IO::StreamData> fd;
	if (!this->packs.GetItem(index).SetTo(pack))
		return 0;
	if (!this->fd.SetTo(fd))
		return 0;

	Data::ByteBuffer packBuff(pack->packSize);
	if (fd->GetRealData(pack->fileOfst, pack->packSize, packBuff) != pack->packSize)
	{
		return 0;
	}
	Text::CString carr[32];
	k = 32;
	while (k-- > 0)
	{
		carr[k] = nullptr;
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
				carr[k - 1].leng = (UOSInt)(&packBuff[l] - carr[k - 1].v.Ptr());
				if (packBuff[l + 1] == 0)
					break;
				carr[k].v = &packBuff[l + 1];
				k++;
			}
			l++;
		}
	}


	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSize));
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
		if (packBuff[1] >= 21 && packBuff[1] >= 21 + packBuff[19] * packBuff[20])
		{
			frame->AddHexBuff(21, packBuff[19] * (UOSInt)packBuff[20], CSTR("Contained Elements"), &packBuff[21], true);
		}
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
		if (packBuff[1] > 17)
		{
			if (packBuff[17] & 0x80)
			{
				frame->AddFloat(17, 1, CSTR("Voltage (V)"), (packBuff[17] & 0x7F) * 0.1);
			}
			else
			{
				frame->AddBit(17, CSTR("Voltage Capability (5V)"), packBuff[17], 0);
				frame->AddBit(17, CSTR("Voltage Capability (3.3V)"), packBuff[17], 1);
				frame->AddBit(17, CSTR("Voltage Capability (2.9V)"), packBuff[17], 2);
				frame->AddBit(17, CSTR("Reserved"), packBuff[17], 3);
				frame->AddBit(17, CSTR("Reserved"), packBuff[17], 4);
				frame->AddBit(17, CSTR("Reserved"), packBuff[17], 5);
				frame->AddBit(17, CSTR("Reserved"), packBuff[17], 6);
				frame->AddBit(17, CSTR("Voltage Mode"), packBuff[17], 7);
			}
		}
		AddUInt16(frame, 18, packBuff, carr, CSTR("External Clock (MHz)"));
		AddUInt16(frame, 20, packBuff, carr, CSTR("Max Speed (MHz)"));
		AddUInt16(frame, 22, packBuff, carr, CSTR("Current Speed (MHz)"));
		if (packBuff[1] > 24)
		{
			switch (packBuff[24] & 7)
			{
			case 0:
				frame->AddUIntName(24, 1, CSTR("Current Speed"), 0, CSTR("Unknown"));
				break;
			case 1:
				frame->AddUIntName(24, 1, CSTR("Current Speed"), 1, CSTR("CPU Enabled"));
				break;
			case 2:
				frame->AddUIntName(24, 1, CSTR("Current Speed"), 2, CSTR("CPU Disabled by User through BIOS Setup"));
				break;
			case 3:
				frame->AddUIntName(24, 1, CSTR("Current Speed"), 3, CSTR("CPU Disabled By BIOS (POST Error)"));
				break;
			case 4:
				frame->AddUIntName(24, 1, CSTR("Current Speed"), 4, CSTR("CPU is Idle, waiting to be enabled"));
				break;
			case 5:
				frame->AddUIntName(24, 1, CSTR("Current Speed"), 5, CSTR("Reserved"));
				break;
			case 6:
				frame->AddUIntName(24, 1, CSTR("Current Speed"), 6, CSTR("Reserved"));
				break;
			case 7:
				frame->AddUIntName(24, 1, CSTR("Current Speed"), 7, CSTR("Other"));
				break;
			}
			frame->AddBit(24, CSTR("Reserved"), packBuff[24], 3);
			frame->AddBit(24, CSTR("Reserved"), packBuff[24], 4);
			frame->AddBit(24, CSTR("Reserved"), packBuff[24], 5);
			frame->AddBit(24, CSTR("CPU Socket Populated"), packBuff[24], 6);
			frame->AddBit(24, CSTR("Reserved"), packBuff[24], 7);
		}
		const Char *names4_2[] = {"Unspecified", "Other", "Unknown", "Daughter Board", "ZIF Socket", "Replaceable Piggy Back", "None", "LIF Socket",
			"Slot 1", "Slot 2", "370-pin socket", "Slot A", "Slot M", "Socket 423", "Socket A", "Socket 478",
			"Socket 754", "Socket 940", "Socket 939", "Socket mPGA604", "Socket LGA771", "Socket LGA775", "Socket S1", "Socket AM2",
			"Socket F", "Socket LGA1366", "Socket G34", "Socket AM3", "Socket C32", "Socket LGA1156", "Socket LGA1567", "Socket PGA988A",
			"Socket BGA1288", "Socket rPGA988B", "Socket BGA1023", "Socket BGA1224", "Socket LGA1155", "Socket LGA1356", "Socket LGA2011", "Socket FS1",
			"Socket FS2", "Socket FM1", "Socket FM2", "Socket LGA2011-3", "Socket LGA1356-3", "Socket LGA1150", "Socket BGA1168", "Socket BGA1234",
			"Socket BGA1364", "Socket AM4", "Socket LGA1151", "Socket BGA1356", "Socket BGA1440", "Socket BGA1515", "Socket LGA3647-1", "Socket SP3",
			"Socket SP3r2", "Socket LGA2066", "Socket BGA1392", "Socket BGA1510", "Socket BGA1528", "Socket LGA4189", "Socket LGA1200", "Socket LGA4677",
			"Socket LGA1700", "Socket BGA1744", "Socket BGA1781", "Socket BGA1211", "Socket BGA2422", "Socket LGA1211", "Socket LGA2422", "Socket LGA5773",
			"Socket BGA5773"};
		AddEnum(frame, 25, packBuff, carr, CSTR("Processor Upgrade"), names4_2, sizeof(names4_2) / sizeof(names4_2[0]));
		AddHex16(frame, 26, packBuff, carr, CSTR("L1 Cache Handle"));
		AddHex16(frame, 28, packBuff, carr, CSTR("L2 Cache Handle"));
		AddHex16(frame, 30, packBuff, carr, CSTR("L3 Cache Handle"));
		AddString(frame, 32, packBuff, carr, CSTR("Serial Number"));
		AddString(frame, 33, packBuff, carr, CSTR("Asset Tag"));
		AddString(frame, 34, packBuff, carr, CSTR("Part Number"));
		AddUInt8(frame, 35, packBuff, carr, CSTR("Core Count"));
		AddUInt8(frame, 36, packBuff, carr, CSTR("Core Enabled"));
		AddUInt8(frame, 37, packBuff, carr, CSTR("Thread Count"));
		const Char *names4_3[] = {"Reserved", "Unknown", "64-bit Capable", "Multi-Core", "Hardware Thread", "Execute Protection", "Enhanced Virtualization", "Power/Performance Control"};
		AddBits(frame, 38, packBuff, carr, names4_3);
		const Char *names4_4[] = {"128-bit Capable", "Arm64 SoC ID", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"};
		AddBits(frame, 39, packBuff, carr, names4_4);
		if (packBuff[1] > 41)
		{
			frame->AddUIntName(40, 2, CSTR("Processor Family 2"), ReadUInt16(&packBuff[40]), IO::SMBIOS::GetProcessorFamily(ReadUInt16(&packBuff[40])));
		}
		AddUInt16(frame, 42, packBuff, carr, CSTR("Core Count 2"));
		AddUInt16(frame, 44, packBuff, carr, CSTR("Core Enabled 2"));
		AddUInt16(frame, 46, packBuff, carr, CSTR("Thread Count 2"));
		AddUInt16(frame, 48, packBuff, carr, CSTR("Thread Enabled"));
		break;
	}
	case 5:
	{
		const Char *names5_1[] = {"Unspecified", "Other", "Unknown", "None", "8-bit Parity", "32-bit ECC", "64-bit ECC", "128-bit ECC",
			"CRC"};
		AddEnum(frame, 4, packBuff, carr, CSTR("Error Detecting Method"), names5_1, sizeof(names5_1) / sizeof(names5_1[0]));
		const Char *names5_2[] = {"Other", "Unknown", "None", "Single-Bit Error Correcting", "Double-Bit Error Correcting", "Error Scrubbing", "Reserved", "Reserved"};
		AddBits(frame, 5, packBuff, carr, names5_2);
		const Char *names5_3[] = {"Unspecified", "Other", "Unknown", "One-Way Interleave", "Two-Way Interleave", "Four-Way Interleave", "Eight-Way Interleave", "Sixteen-Way Interleave"};
		AddEnum(frame, 6, packBuff, carr, CSTR("Supported Interleave"), names5_3, sizeof(names5_3) / sizeof(names5_3[0]));
		AddEnum(frame, 7, packBuff, carr, CSTR("Current Interleave"), names5_3, sizeof(names5_3) / sizeof(names5_3[0]));
		if (packBuff[1] > 8)
		{
			frame->AddUInt(8, 1, CSTR("Maximum Memory Module Size"), (UOSInt)2 << packBuff[8]);
		}
		const Char *names5_4[] = {"Other", "Unknown", "70ns", "60ns", "50ns", "Reserved", "Reserved", "Reserved"};
		AddBits(frame, 9, packBuff, carr, names5_4);
		AddHex8(frame, 10, packBuff, carr, CSTR("Supported Speeds Reserved"));
		const Char *names5_5[] = {"Other", "Unknown", "Standard", "Fast Page Mode", "EDO", "Parity", "ECC", "SIMM"};
		const Char *names5_6[] = {"DIMM", "Burst EDO", "SDRAM", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"};
		AddBits(frame, 11, packBuff, carr, names5_5);
		AddBits(frame, 12, packBuff, carr, names5_6);
		const Char *names5_7[] = {"5V", "3.3V", "2.9V", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"};
		AddBits(frame, 13, packBuff, carr, names5_7);
		AddUInt8(frame, 14, packBuff, carr, CSTR("Number of Associated Memory Slots"));
		if (packBuff[1] > 14)
		{
			UOSInt n = packBuff[14];
			UOSInt i = 0;
			while (i < n)
			{
				AddHex16(frame, 15 + 2 * i, packBuff, carr, CSTR("Memory Module Configuration Handles"));
				i++;
			}
			AddBits(frame, 15 + 2 * n, packBuff, carr, names5_2);
		}
		break;
	}
	case 6:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Socket Designation"));
		if (packBuff[1] > 5)
		{
			frame->AddHex8(5, CSTR("Bank Connections 1"), (UInt8)(packBuff[5] >> 4));
			frame->AddHex8(5, CSTR("Bank Connections 2"), packBuff[5] & 15);
		}
		AddUInt8(frame, 6, packBuff, carr, CSTR("Current Speed (ns)"));
		const Char *names6_1[] = {"Other", "Unknown", "Standard", "Fast Page Mode", "EDO", "Parity", "ECC", "SIMM"};
		const Char *names6_2[] = {"DIMM", "Burst EDO", "SDRAM", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"};
		AddBits(frame, 7, packBuff, carr, names6_1);
		AddBits(frame, 8, packBuff, carr, names6_2);
		if (packBuff[1] > 9)
		{
			frame->AddBit(9, CSTR("double-bank"), packBuff[9], 7);
			if ((packBuff[9] & 0x7F) == 0x7D)
			{
				frame->AddField(9, 1, CSTR("Installed Size"), CSTR("Not determinable"));
			}
			else if ((packBuff[9] & 0x7F) == 0x7E)
			{
				frame->AddField(9, 1, CSTR("Installed Size"), CSTR("Module is installed, but no memory has been enabled"));
			}
			else if ((packBuff[9] & 0x7F) == 0x7F)
			{
				frame->AddField(9, 1, CSTR("Installed Size"), CSTR("Not installed"));
			}
			else
			{
				frame->AddUInt(9, 1, CSTR("Installed Size"), (UOSInt)2 << (packBuff[9] & 0x7F));
			}
		}
		if (packBuff[1] > 10)
		{
			frame->AddBit(10, CSTR("double-bank"), packBuff[10], 7);
			if ((packBuff[10] & 0x7F) == 0x7D)
			{
				frame->AddField(10, 1, CSTR("Enabled Size"), CSTR("Not determinable"));
			}
			else if ((packBuff[10] & 0x7F) == 0x7E)
			{
				frame->AddField(10, 1, CSTR("Enabled Size"), CSTR("Module is installed, but no memory has been enabled"));
			}
			else if ((packBuff[10] & 0x7F) == 0x7F)
			{
				frame->AddField(10, 1, CSTR("Enabled Size"), CSTR("Not installed"));
			}
			else
			{
				frame->AddUInt(10, 1, CSTR("Enabled Size"), (UOSInt)2 << (packBuff[10] & 0x7F));
			}
		}
		const Char *names6_3[] = {"Uncorrectable errors received for the module", "Correctable errors received for the module", "Error Status information should be obtained from the event log", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"};
		AddBits(frame, 11, packBuff, carr, names6_3);
		break;
	}
	case 7:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Socket Designation"));
		if (packBuff[1] > 6)
		{
			frame->AddUInt(5, 1, CSTR("Cache Level"), packBuff[5] & 7);
			frame->AddBit(5, CSTR("Cache Socketed"), packBuff[5], 3);
			frame->AddBit(5, CSTR("Reserved"), packBuff[5], 4);
			switch ((packBuff[5] >> 5) & 3)
			{
			case 0:
				frame->AddField(5, 1, CSTR("Location"), CSTR("Internal"));
				break;
			case 1:
				frame->AddField(5, 1, CSTR("Location"), CSTR("External"));
				break;
			case 2:
				frame->AddField(5, 1, CSTR("Location"), CSTR("Reserved"));
				break;
			case 3:
				frame->AddField(5, 1, CSTR("Location"), CSTR("Unknown"));
				break;
			}
			frame->AddBit(5, CSTR("Enabled"), packBuff[5], 7);
			switch (packBuff[6] & 3)
			{
			case 0:
				frame->AddField(6, 1, CSTR("Operational Mode"), CSTR("Write Through"));
				break;
			case 1:
				frame->AddField(6, 1, CSTR("Operational Mode"), CSTR("Write Back"));
				break;
			case 2:
				frame->AddField(6, 1, CSTR("Operational Mode"), CSTR("Varies with Memory Address"));
				break;
			case 3:
				frame->AddField(6, 1, CSTR("Operational Mode"), CSTR("Unknown"));
				break;
			}
			frame->AddUInt(6, 1, CSTR("Reserved"), (UOSInt)packBuff[6] >> 2);
		}
		if (packBuff[1] > 8)
		{
			if (packBuff[8] & 0x80)
			{
				frame->AddUInt(7, 2, CSTR("Maximum Cache Size (KB)"), (UOSInt)(ReadUInt16(&packBuff[7]) & 0x7FFF) * 64);
			}
			else
			{
				frame->AddUInt(7, 2, CSTR("Maximum Cache Size (KB)"), ReadUInt16(&packBuff[7]));
			}
		}
		if (packBuff[1] > 10)
		{
			if (packBuff[10] & 0x80)
			{
				frame->AddUInt(9, 2, CSTR("Installed Size (KB)"), (UOSInt)(ReadUInt16(&packBuff[9]) & 0x7FFF) * 64);
			}
			else
			{
				frame->AddUInt(9, 2, CSTR("Installed Size (KB)"), ReadUInt16(&packBuff[9]));
			}
		}
		const Char *names7_1[] = {"Other", "Unknown", "Non-Burst", "Burst", "Pipeline Burst", "Synchronous", "Asynchronous", "Reserved"};
		if (packBuff[1] > 12)
		{
			frame->AddHex16(11, CSTR("Supported SRAM Type"), ReadUInt16(&packBuff[11]));
			AddBits(frame, 11, packBuff, carr, names7_1);
			AddHex8(frame, 12, packBuff, carr, CSTR("Reserved"));
		}
		if (packBuff[1] > 14)
		{
			frame->AddHex16(13, CSTR("Current SRAM Type"), ReadUInt16(&packBuff[13]));
			AddBits(frame, 13, packBuff, carr, names7_1);
			AddHex8(frame, 14, packBuff, carr, CSTR("Reserved"));
		}
		AddUInt8(frame, 15, packBuff, carr, CSTR("Cache Speed (ns)"));
		const Char *names7_2[] = {"Unspecified", "Other", "Unknown", "None", "Parity", "Single-bit ECC", "Multi-bit ECC"};
		AddEnum(frame, 16, packBuff, carr, CSTR("Error Correction Type"), names7_2, sizeof(names7_2) / sizeof(names7_2[0]));
		const Char *names7_3[] = {"Unspecified", "Other", "Unknown", "Instruction", "Data", "Unified"};
		AddEnum(frame, 17, packBuff, carr, CSTR("System Cache Type"), names7_3, sizeof(names7_3) / sizeof(names7_3[0]));
		const Char *names7_4[] = {"Unspecified", "Other", "Unknown", "Direct Mapped", "2-way Set-Associative", "4-way Set-Associative", "Fully Associative", "8-way Set-Associative",
			"16-way Set-Associative", "12-way Set-Associative", "24-way Set-Associative", "32-way Set-Associative", "48-way Set-Associative", "64-way Set-Associative", "20-way Set-Associative"};
		AddEnum(frame, 18, packBuff, carr, CSTR("Associativity"), names7_4, sizeof(names7_4) / sizeof(names7_4[0]));
		if (packBuff[1] > 22)
		{
			if (packBuff[22] & 0x80)
			{
				frame->AddUInt(19, 4, CSTR("Maximum Cache Size 2 (KB)"), (UOSInt)(ReadUInt32(&packBuff[19]) & 0x7FFFFFFF) * 64);
			}
			else
			{
				frame->AddUInt(19, 4, CSTR("Maximum Cache Size 2 (KB)"), ReadUInt32(&packBuff[19]));
			}
		}
		if (packBuff[1] > 26)
		{
			if (packBuff[26] & 0x80)
			{
				frame->AddUInt(23, 4, CSTR("Maximum Cache Size 2 (KB)"), (UOSInt)(ReadUInt32(&packBuff[23]) & 0x7FFFFFFF) * 64);
			}
			else
			{
				frame->AddUInt(23, 4, CSTR("Maximum Cache Size 2 (KB)"), ReadUInt32(&packBuff[23]));
			}
		}
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
	case 9:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Slot Designation"));
		if (packBuff[1] > 5) frame->AddUIntName(5, 1, CSTR("Slot Type"), packBuff[5], SlotTypeGetName(packBuff[5]));
		const Char *names9_1[] = {"Unspecified", "Other", "Unknown", "8 bit", "16 bit", "32 bit", "64 bit", "128 bit",
			"1x", "2x", "4x", "8x", "16x", "32x"};
		AddEnum(frame, 6, packBuff, carr, CSTR("Slot Data Bus Width"), names9_1, sizeof(names9_1) / sizeof(names9_1[0]));
		const Char *names9_2[] = {"Unspecified", "Other", "Unknown", "Available", "In use", "Unavailable"};
		AddEnum(frame, 7, packBuff, carr, CSTR("Current Usage"), names9_2, sizeof(names9_2) / sizeof(names9_2[0]));
		const Char *names9_3[] = {"Unspecified", "Other", "Unknown", "Short Length", "Long Length", "2.5\" drive form factor", "3.5\" drive form factor"};
		AddEnum(frame, 8, packBuff, carr, CSTR("Slot Length"), names9_3, sizeof(names9_3) / sizeof(names9_3[0]));
		AddUInt8(frame, 9, packBuff, carr, CSTR("Slot ID 1"));
		AddUInt8(frame, 10, packBuff, carr, CSTR("Slot ID 2"));
		const Char *names9_4[] = {"Characteristics unknown", "Provides 5.0 volts", "Provides 3.3 volts", "Slot's opening is shared with another slot", "PC Card slot supports PC Card-16", "PC Card slot supports CardBus", "PC Card slot supports Zoom Video", "PC Card slot supports Modem Ring Resume"};
		AddBits(frame, 11, packBuff, carr, names9_4);
		const Char *names9_5[] = {"PCI slot supports Power Management Event (PME#) signal", "Slot supports hot-plug devices", "PCI slot supports SMBus signal", "PCIe slot supports bifurcation", "Slot supports async/surprise removal", "Flexbus slot, CXL 1.0 capable", "Flexbus slot, CXL 2.0 capable", "Reserved"};
		AddBits(frame, 12, packBuff, carr, names9_5);
		AddUInt16(frame, 13, packBuff, carr, CSTR("Segment Group Number"));
		AddUInt8(frame, 15, packBuff, carr, CSTR("Bus Number"));
		if (packBuff[1] > 16)
		{
			frame->AddUInt(16, 1, CSTR("device number"), (UOSInt)packBuff[16] >> 3);
			frame->AddUInt(16, 1, CSTR("function number"), packBuff[16] & 7);
		}
		AddUInt8(frame, 17, packBuff, carr, CSTR("Data Bus Width"));
		AddUInt8(frame, 18, packBuff, carr, CSTR("Peer grouping count"));
		if (packBuff[1] > 18)
		{
			UOSInt n = packBuff[18];
			UOSInt i = 0;
			while (i < n)
			{
				AddUInt8(frame, 19 + 5 * i, packBuff, carr, CSTR("Slot Information"));
				AddUInt8(frame, 20 + 5 * i, packBuff, carr, CSTR("Slot Physical Width"));
				AddUInt16(frame, 21 + 5 * i, packBuff, carr, CSTR("Slot Pitch"));
				AddUInt8(frame, 23 + 5 * i, packBuff, carr, CSTR("Slot Height"));
				i++;
			}
		}
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
	case 14:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Group Name"));
		UOSInt i = 5;
		while (i + 3 <= packBuff[1])
		{
			frame->AddUIntName(i, 1, CSTR("Item Type"), packBuff[i], SMBIOSTypeGetName(packBuff[i]));
			AddHex16(frame, i + 1, packBuff, carr, CSTR("Item Handle"));
			i += 3;
		}
		break;
	}
	case 15:
	{
		AddUInt16(frame, 4, packBuff, carr, CSTR("Log Area Length"));
		AddUInt16(frame, 6, packBuff, carr, CSTR("Log Header Start Offset"));
		AddUInt16(frame, 8, packBuff, carr, CSTR("Log Data Start Offset"));
		AddUInt8(frame, 10, packBuff, carr, CSTR("Access Method"));
		const Char *names15_1[] = {"Log area valid", "Log area full", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"};
		AddBits(frame, 11, packBuff, carr, names15_1);
		AddHex32(frame, 12, packBuff, carr, CSTR("Log Change Token"));
		AddHex32(frame, 16, packBuff, carr, CSTR("Access Method Address"));
		AddUInt8(frame, 20, packBuff, carr, CSTR("Log Header Format"));
		AddUInt8(frame, 21, packBuff, carr, CSTR("Number of Supported Log Type Descriptors"));
		AddUInt8(frame, 22, packBuff, carr, CSTR("Length of each Log Type Descriptor"));
		if (packBuff[1] > 23)
		{
			frame->AddHexBuff(23, (UOSInt)packBuff[1] - 23, CSTR("List of Supported Event Log Type Descriptors"), &packBuff[23], true);
		}
		break;
	}
	case 16:
	{
		frame->AddUIntName(4, 1,  CSTR("Location"), packBuff[4], MemoryLocationGetName(packBuff[4]));
		const Char *names13_2[] = {"Unspecified", "Other", "Unknown", "System memory", "Video memory", "Flash memory", "Non-volatile RAM", "Cache memory"};
		AddEnum(frame, 5, packBuff, carr, CSTR("Use"), names13_2, sizeof(names13_2) / sizeof(names13_2[0]));
		const Char *names13_3[] = {"Unspecified", "Other", "Unknown", "None", "Parity", "Single-bit ECC", "Multi-bit ECC", "CRC"};
		AddEnum(frame, 6, packBuff, carr, CSTR("Memory Error Correction"), names13_3, sizeof(names13_3) / sizeof(names13_3[0]));
		frame->AddUInt(7, 4, CSTR("Maximum Capacity (KiB)"), ReadUInt32(&packBuff[7]));
		AddHex16(frame, 11, packBuff, carr, CSTR("Memory Error Information Handle"));
		AddUInt16(frame, 13, packBuff, carr, CSTR("Number of Memory Devices"));
		AddUInt64(frame, 15, packBuff, carr, CSTR("Extended Maximum Capacity (Bytes)"));
		break;
	}
	case 17:
	{
		AddHex16(frame, 4, packBuff, carr, CSTR("Physical Memory Array Handle"));
		AddHex16(frame, 6, packBuff, carr, CSTR("Memory Error Information Handle"));
		AddUInt16(frame, 8, packBuff, carr, CSTR("Total Width (bits)"));
		AddUInt16(frame, 10, packBuff, carr, CSTR("Data Width (bits)"));
		if (ReadUInt16(&packBuff[12]) == 0xFFFF)
		{
			frame->AddField(12, 2, CSTR("Size"), CSTR("Unknown"));
		}
		else if (ReadUInt16(&packBuff[12]) == 0x7FFF)
		{
			frame->AddField(12, 2, CSTR("Size"), CSTR("Use Extended Size"));
		}
		else if (packBuff[13] & 0x80)
		{
			sptr = Text::StrConcatC(Text::StrUOSInt(sbuff, ReadUInt16(&packBuff[12]) & 0x7FFF), UTF8STRC("KB"));
			frame->AddField(12, 2, CSTR("Size"), CSTRP(sbuff, sptr));
		}
		else
		{
			sptr = Text::StrConcatC(Text::StrUOSInt(sbuff, ReadUInt16(&packBuff[12]) & 0x7FFF), UTF8STRC("MB"));
			frame->AddField(12, 2, CSTR("Size"), CSTRP(sbuff, sptr));
		}
		const Char *names17_1[] = {"Unspecified", "Other", "Unknown", "SIMM", "SIP", "Chip", "DIP", "ZIP",
			"Proprietary Card", "DIMM", "TSOP", "Row of chips", "RIMM", "SODIMM", "SRIMM", "FB-DIMM",
			"Die"};
		AddEnum(frame, 14, packBuff, carr, CSTR("Form Factor"), names17_1, sizeof(names17_1) / sizeof(names17_1[0]));
		AddUInt8(frame, 15, packBuff, carr, CSTR("Device Set"));
		AddString(frame, 16, packBuff, carr, CSTR("Device Locator"));
		AddString(frame, 17, packBuff, carr, CSTR("Bank Locator"));
		const Char *names17_2[] = {"Unspecified", "Other", "Unknown", "DRAM", "EDRAM", "VRAM", "SRAM", "RAM",
			"ROM", "FLASH", "EEPROM", "FEPROM", "EPROM", "CDRAM", "3DRAM", "SDRAM",
			"SGRAM", "RDRAM", "DDR", "DDR2", "DDR2 FB-DIMM", "Reserved", "Reserved", "Reserved",
			"DDR3", "FBD2", "DDR4", "LPDDR", "LPDDR2", "LPDDR3", "LPDDR4", "Logical non-volatile device",
			"HBM", "HBM2", "DDR5", "LPDDR5", "HBM3"};
		AddEnum(frame, 18, packBuff, carr, CSTR("Memory Type"), names17_2, sizeof(names17_2) / sizeof(names17_2[0]));
		const Char *names17_3[] = {"Reserved", "Other", "Unknown", "Fast-paged", "Static column", "Pseudo-static", "RAMBUS", "Synchronous"};
		AddBits(frame, 19, packBuff, carr, names17_3);
		const Char *names17_4[] = {"CMOS", "EDO", "Window DRAM", "Cache DRAM", "Non-volatile", "Registered", "Unbuffered", "LRDIMM"};
		AddBits(frame, 20, packBuff, carr, names17_4);
		AddUInt16(frame, 21, packBuff, carr, CSTR("Speed (MT/s)"));
		AddString(frame, 23, packBuff, carr, CSTR("Manufacturer"));
		AddString(frame, 24, packBuff, carr, CSTR("Serial Number"));
		AddString(frame, 25, packBuff, carr, CSTR("Asset Tag"));
		AddString(frame, 26, packBuff, carr, CSTR("Part Number"));
		frame->AddUInt(27, 1, CSTR("Rank"), packBuff[27] & 15);
		frame->AddUInt(27, 1, CSTR("Reserved"), (UOSInt)packBuff[27] >> 4);
		frame->AddUInt(28, 4, CSTR("Extended Size (MB)"), ReadUInt32(&packBuff[28]));
		AddUInt16(frame, 32, packBuff, carr, CSTR("Configured Memory Speed (MT/s)"));
		AddUInt16(frame, 34, packBuff, carr, CSTR("Minimum voltage (mV)"));
		AddUInt16(frame, 36, packBuff, carr, CSTR("Maximum voltage (mV)"));
		AddUInt16(frame, 38, packBuff, carr, CSTR("Configured voltage (mV)"));
		const Char *names17_5[] = {"Unspecified", "Other", "Unknown", "DRAM", "NVDIMM-N", "NVDIMM-F", "NVDIMM-P", "Intel Optane persistent memory"};
		AddEnum(frame, 40, packBuff, carr, CSTR("Memory Technology"), names17_5, sizeof(names17_5) / sizeof(names17_5[0]));
		const Char *names17_6[] = {"Reserved", "Other", "Unknown", "Volatile memory", "Byte-accessible persistent memory", "Block-accessible persistent memory", "Reserved", "Reserved"};
		AddBits(frame, 41, packBuff, carr, names17_6);
		AddHex8(frame, 42, packBuff, carr, CSTR("Reserved"));
		AddString(frame, 43, packBuff, carr, CSTR("Firmware Version"));
		AddHex16(frame, 44, packBuff, carr, CSTR("Module Manufacturer ID"));
		AddHex16(frame, 46, packBuff, carr, CSTR("Module Product ID"));
		AddHex16(frame, 48, packBuff, carr, CSTR("Memory Subsystem Controller Manufacturer ID"));
		AddHex16(frame, 50, packBuff, carr, CSTR("Memory Subsystem Controller Product ID"));
		AddUInt64(frame, 52, packBuff, carr, CSTR("Non-volatile Size (Bytes)"));
		AddUInt64(frame, 60, packBuff, carr, CSTR("Volatile Size (Bytes)"));
		AddUInt64(frame, 68, packBuff, carr, CSTR("Cache Size (Bytes)"));
		AddUInt64(frame, 76, packBuff, carr, CSTR("Logical Size (Bytes)"));
		if (packBuff[1] >= 88)
		{
			frame->AddUInt(84, 4, CSTR("Extended Speed (MT/s)"), ReadUInt32(&packBuff[84]) & 0x7fffffff);
		}
		if (packBuff[1] >= 92)
		{
			frame->AddUInt(88, 4, CSTR("Extended Configured Memory Speed (MT/s)"), ReadUInt32(&packBuff[88]) & 0x7fffffff);
		}
		break;
	}
	case 18:
	{
		const Char* names18_1[] = { "Unspecified", "Other", "Unknown", "OK", "Bad read", "Parity error", "Single-bit error", "Double-bit error",
			"Multi-bit error", "Nibble error", "Checksum error", "CRC error", "Corrected single-bit error", "Corrected error", "Uncorrectable error"};
		AddEnum(frame, 4, packBuff, carr, CSTR("Error Type"), names18_1, sizeof(names18_1) / sizeof(names18_1[0]));
		const Char* names18_2[] = { "Unspecified", "Other", "Unknown", "Device level", "Memory partition level"};
		AddEnum(frame, 5, packBuff, carr, CSTR("Error Granularity"), names18_2, sizeof(names18_2) / sizeof(names18_2[0]));
		const Char* names18_3[] = { "Unspecified", "Other", "Unknown", "Read", "Write", "Partial write"};
		AddEnum(frame, 6, packBuff, carr, CSTR("Error Operation"), names18_3, sizeof(names18_3) / sizeof(names18_3[0]));
		AddHex32(frame, 7, packBuff, carr, CSTR("Vendor Syndrome"));
		AddHex32(frame, 11, packBuff, carr, CSTR("Memory Array Error Address"));
		AddHex32(frame, 15, packBuff, carr, CSTR("Device Error Address"));
		AddHex32(frame, 19, packBuff, carr, CSTR("Error Resolution"));
		break;
	}
	case 19:
		AddHex32(frame, 4, packBuff, carr, CSTR("Starting Address"));
		AddHex32(frame, 8, packBuff, carr, CSTR("Ending Address"));
		AddHex16(frame, 12, packBuff, carr, CSTR("Memory Array Handle"));
		AddUInt8(frame, 14, packBuff, carr, CSTR("Partition Width"));
		AddHex64(frame, 15, packBuff, carr, CSTR("Extended Starting Address"));
		AddHex64(frame, 23, packBuff, carr, CSTR("Extended Ending Address"));
		break;
	case 20:
		AddHex32(frame, 4, packBuff, carr, CSTR("Starting Address"));
		AddHex32(frame, 8, packBuff, carr, CSTR("Ending Address"));
		AddHex16(frame, 12, packBuff, carr, CSTR("Memory Device Handle"));
		AddHex16(frame, 14, packBuff, carr, CSTR("Memory Array Mapped Address Handle"));
		AddUInt8(frame, 16, packBuff, carr, CSTR("Partition Row Position"));
		AddUInt8(frame, 17, packBuff, carr, CSTR("Interleave Position"));
		AddUInt8(frame, 18, packBuff, carr, CSTR("Interleave Data Depth"));
		AddHex64(frame, 19, packBuff, carr, CSTR("Extended Starting Address"));
		AddHex64(frame, 27, packBuff, carr, CSTR("Extended Ending Address"));
		break;
	case 21:
	{
		const Char* names21_1[] = { "Unspecified", "Other", "Unknown", "Mouse", "Track Ball", "Track Point", "Glide Point", "Touch Pad",
			"Touch Screen", "Optical Sensor"};
		AddEnum(frame, 4, packBuff, carr, CSTR("Type"), names21_1, sizeof(names21_1) / sizeof(names21_1[0]));
		if (packBuff[1] > 5) frame->AddUIntName(5, 1, CSTR("Interface"), packBuff[5], PointingDeviceInterfaceGetName(packBuff[5]));
		AddUInt8(frame, 6, packBuff, carr, CSTR("Number of Buttons"));
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
	case 23:
	{
		if (packBuff[1] > 4)
		{
			const Char *names23_1[] = {"Reserved", "Operating system", "System utilities", "Do not reboot"};
			frame->AddBit(4, CSTR("The system reset is enabled by the user"), packBuff[4], 0);
			frame->AddUIntName(4, 1, CSTR("Boot Option"), (packBuff[4] >> 1) & 3, Text::CString::FromPtr((const UTF8Char*)names23_1[(packBuff[4] >> 1) & 3]));
			frame->AddUIntName(4, 1, CSTR("Boot Option on Limit"), (packBuff[4] >> 3) & 3, Text::CString::FromPtr((const UTF8Char*)names23_1[(packBuff[4] >> 3) & 3]));
			frame->AddBit(4, CSTR("System contains a watchdog timer"), packBuff[4], 5);
			frame->AddBit(4, CSTR("Reserved"), packBuff[4], 6);
			frame->AddBit(4, CSTR("Reserved"), packBuff[4], 7);
		}
		AddUInt16(frame, 5, packBuff, carr, CSTR("Reset Count"));
		AddUInt16(frame, 7, packBuff, carr, CSTR("Reset Limit"));
		AddUInt16(frame, 9, packBuff, carr, CSTR("Timer Interval"));
		AddUInt16(frame, 11, packBuff, carr, CSTR("Timeout"));
		break;
	}
	case 24:
	{
		if (packBuff[1] > 4)
		{
			const Char *names24_1[] = {"Disabled", "Enabled", "Not Implemented", "Unknown"};
			frame->AddUIntName(4, 1, CSTR("Front Panel Reset Status"), (packBuff[4] >> 0) & 3, Text::CString::FromPtr((const UTF8Char*)names24_1[(packBuff[4] >> 0) & 3]));
			frame->AddUIntName(4, 1, CSTR("Administrator Password Status"), (packBuff[4] >> 2) & 3, Text::CString::FromPtr((const UTF8Char*)names24_1[(packBuff[4] >> 2) & 3]));
			frame->AddUIntName(4, 1, CSTR("Keyboard Password Status"), (packBuff[4] >> 4) & 3, Text::CString::FromPtr((const UTF8Char*)names24_1[(packBuff[4] >> 4) & 3]));
			frame->AddUIntName(4, 1, CSTR("Power-on Password Status"), (packBuff[4] >> 6) & 3, Text::CString::FromPtr((const UTF8Char*)names24_1[(packBuff[4] >> 6) & 3]));
		}
		break;
	}
	case 25:
	{
		AddHex8(frame, 4, packBuff, carr, CSTR("Next Scheduled Power-on Month"));
		AddHex8(frame, 5, packBuff, carr, CSTR("Next Scheduled Power-on Day-of-month"));
		AddHex8(frame, 6, packBuff, carr, CSTR("Next Scheduled Power-on Hour"));
		AddHex8(frame, 7, packBuff, carr, CSTR("Next Scheduled Power-on Minute"));
		AddHex8(frame, 8, packBuff, carr, CSTR("Next Scheduled Power-on Second"));
		break;
	}
	case 26:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Description"));
		if (packBuff[1] > 5)
		{
			const Char *names26_1[] = {"Unspecified", "Other", "Unknown", "OK", "Non-critical", "Critical", "Non-recoverable"};
			AddEnum(frame, 5, (UInt8)(packBuff[5] >> 5), carr, CSTR("Status"), names26_1, sizeof(names26_1) / sizeof(names26_1[0]));
			const Char *names26_2[] = {"Unspecified", "Other", "Unknown", "OK", "Non-critical", "Critical", "Non-recoverable"};
			AddEnum(frame, 5, (UInt8)(packBuff[5] & 0x1F), carr, CSTR("Status"), names26_2, sizeof(names26_2) / sizeof(names26_2[0]));
		}
		AddUInt16(frame, 6, packBuff, carr, CSTR("Maximum Value (mV)"));
		AddUInt16(frame, 8, packBuff, carr, CSTR("Minimum Value (mV)"));
		if (packBuff[1] > 11) frame->AddFloat(10, 2, CSTR("Resolution (mV)"), ReadUInt16(&packBuff[10]) * 0.1);
		AddUInt16(frame, 12, packBuff, carr, CSTR("Tolerance (mV)"));
		if (packBuff[1] > 15) frame->AddFloat(14, 2, CSTR("Accuracy (%)"), ReadUInt16(&packBuff[14]) * 0.01);
		AddHex32(frame, 16, packBuff, carr, CSTR("OEM-defined"));
		AddUInt16(frame, 20, packBuff, carr, CSTR("Nominal Value (mV)"));
		break;
	}
	case 27:
	{
		AddHex16(frame, 4, packBuff, carr, CSTR("Temperature Probe Handle"));
		if (packBuff[1] > 6)
		{
			const Char *names27_1[] = {"Unspecified", "Other", "Unknown", "OK", "Non-critical", "Critical", "Non-recoverable"};
			AddEnum(frame, 6, (UInt8)(packBuff[6] >> 5), carr, CSTR("Status"), names27_1, sizeof(names27_1) / sizeof(names27_1[0]));
			Text::CString devType;
			switch (packBuff[6] & 0x1F)
			{
			case 0:
				devType = CSTR("Unspecified");
				break;
			case 1:
				devType = CSTR("Other");
				break;
			case 2:
				devType = CSTR("Unknown");
				break;
			case 3:
				devType = CSTR("Fan");
				break;
			case 4:
				devType = CSTR("Centrifugal Blower");
				break;
			case 5:
				devType = CSTR("Chip Fan");
				break;
			case 6:
				devType = CSTR("Cabinet Fan");
				break;
			case 7:
				devType = CSTR("Power Supply Fan");
				break;
			case 8:
				devType = CSTR("Heat Pipe");
				break;
			case 9:
				devType = CSTR("Integrated Refrigeration");
				break;
			case 16:
				devType = CSTR("Active Cooling");
				break;
			case 17:
				devType = CSTR("Passive Cooling");
				break;
			default:
				devType = CSTR("Unknown");
				break;
			}
			frame->AddUIntName(6, 1, CSTR("Device Type"), packBuff[6] & 0x1F, devType);
		}
		AddUInt8(frame, 7, packBuff, carr, CSTR("Cooling Unit Group"));
		AddHex32(frame, 8, packBuff, carr, CSTR("OEM-defined"));
		AddUInt16(frame, 12, packBuff, carr, CSTR("Nominal Speed (rpm)"));
		AddString(frame, 14, packBuff, carr, CSTR("Description"));
		break;
	}
	case 28:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Description"));
		if (packBuff[1] > 5)
		{
			const Char *names28_1[] = {"Unspecified", "Other", "Unknown", "OK", "Non-critical", "Critical", "Non-recoverable"};
			AddEnum(frame, 5, (UInt8)(packBuff[5] >> 5), carr, CSTR("Status"), names28_1, sizeof(names28_1) / sizeof(names28_1[0]));
			const Char *names28_2[] = {"Unspecified", "Other", "Unknown", "Processor", "Disk", "Peripheral Bay", "System Management Module", "Motherboard",
				"Memory Module", "Processor Module", "Power Unit", "Add-in Card", "Front Panel Board", "Back Panel Board", "Power System Board", "Drive Back Plane"};
			AddEnum(frame, 5, packBuff[5] & 0x1F, carr, CSTR("Device Type"), names28_2, sizeof(names28_2) / sizeof(names28_2[0]));
		}
		if (packBuff[1] > 7)
		{
			if (ReadUInt16(&packBuff[6]) == 0x8000)
			{
				frame->AddField(6, 2, CSTR("Maximum Value"), CSTR("Unknown"));
			}
			else
			{
				frame->AddFloat(6, 2, CSTR("Maximum Value"), ReadUInt16(&packBuff[6]) * 0.1);
			}
		}
		if (packBuff[1] > 9)
		{
			if (ReadUInt16(&packBuff[8]) == 0x8000)
			{
				frame->AddField(8, 2, CSTR("Minimum Value"), CSTR("Unknown"));
			}
			else
			{
				frame->AddFloat(8, 2, CSTR("Minimum Value"), ReadUInt16(&packBuff[8]) * 0.1);
			}
		}
		if (packBuff[1] > 11)
		{
			if (ReadUInt16(&packBuff[10]) == 0x8000)
			{
				frame->AddField(10, 2, CSTR("Resolution"), CSTR("Unknown"));
			}
			else
			{
				frame->AddFloat(10, 2, CSTR("Resolution"), ReadUInt16(&packBuff[10]) * 0.001);
			}
		}
		if (packBuff[1] > 13)
		{
			if (ReadUInt16(&packBuff[12]) == 0x8000)
			{
				frame->AddField(12, 2, CSTR("Tolerance"), CSTR("Unknown"));
			}
			else
			{
				frame->AddFloat(12, 2, CSTR("Tolerance"), ReadUInt16(&packBuff[12]) * 0.1);
			}
		}
		if (packBuff[1] > 15)
		{
			if (ReadUInt16(&packBuff[14]) == 0x8000)
			{
				frame->AddField(14, 2, CSTR("Accuracy"), CSTR("Unknown"));
			}
			else
			{
				frame->AddFloat(14, 2, CSTR("Accuracy"), ReadUInt16(&packBuff[14]) * 0.01);
			}
		}
		AddHex32(frame, 16, packBuff, carr, CSTR("OEM-defined"));
		if (packBuff[1] > 21)
		{
			if (ReadUInt16(&packBuff[20]) == 0x8000)
			{
				frame->AddField(20, 2, CSTR("Nominal Value"), CSTR("Unknown"));
			}
			else
			{
				frame->AddFloat(20, 2, CSTR("Nominal Value"), ReadUInt16(&packBuff[20]) * 0.01);
			}
		}
		break;
	}
	case 29:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Description"));
		if (packBuff[1] > 5)
		{
			const Char *names29_1[] = {"Unspecified", "Other", "Unknown", "OK", "Non-critical", "Critical", "Non-recoverable"};
			AddEnum(frame, 5, (UInt8)(packBuff[5] >> 5), carr, CSTR("Status"), names29_1, sizeof(names29_1) / sizeof(names29_1[0]));
			const Char *names29_2[] = {"Unspecified", "Other", "Unknown", "Processor", "Disk", "Peripheral Bay", "System Management Module", "Motherboard",
				"Memory Module", "Processor Module", "Power Unit", "Add-in Card"};
			AddEnum(frame, 5, packBuff[5] & 0x1F, carr, CSTR("Location"), names29_2, sizeof(names29_2) / sizeof(names29_2[0]));
		}
		if (packBuff[1] > 7)
		{
			if (ReadUInt16(&packBuff[6]) == 0x8000)
			{
				frame->AddField(6, 2, CSTR("Maximum Value"), CSTR("Unknown"));
			}
			else
			{
				frame->AddFloat(6, 2, CSTR("Maximum Value(mA)"), ReadUInt16(&packBuff[6]));
			}
		}
		if (packBuff[1] > 9)
		{
			if (ReadUInt16(&packBuff[8]) == 0x8000)
			{
				frame->AddField(8, 2, CSTR("Minimum Value"), CSTR("Unknown"));
			}
			else
			{
				frame->AddFloat(8, 2, CSTR("Minimum Value(mA)"), ReadUInt16(&packBuff[8]));
			}
		}
		if (packBuff[1] > 11)
		{
			if (ReadUInt16(&packBuff[10]) == 0x8000)
			{
				frame->AddField(10, 2, CSTR("Resolution"), CSTR("Unknown"));
			}
			else
			{
				frame->AddFloat(10, 2, CSTR("Resolution(mA)"), ReadUInt16(&packBuff[10]) * 10.0);
			}
		}
		if (packBuff[1] > 13)
		{
			if (ReadUInt16(&packBuff[12]) == 0x8000)
			{
				frame->AddField(12, 2, CSTR("Tolerance"), CSTR("Unknown"));
			}
			else
			{
				frame->AddFloat(12, 2, CSTR("Tolerance(mA)"), ReadUInt16(&packBuff[12]));
			}
		}
		if (packBuff[1] > 15)
		{
			if (ReadUInt16(&packBuff[14]) == 0x8000)
			{
				frame->AddField(14, 2, CSTR("Accuracy"), CSTR("Unknown"));
			}
			else
			{
				frame->AddFloat(14, 2, CSTR("Accuracy(%)"), ReadUInt16(&packBuff[14]) * 0.01);
			}
		}
		AddHex32(frame, 16, packBuff, carr, CSTR("OEM-defined"));
		if (packBuff[1] > 21)
		{
			if (ReadUInt16(&packBuff[20]) == 0x8000)
			{
				frame->AddField(20, 2, CSTR("Nominal Value"), CSTR("Unknown"));
			}
			else
			{
				frame->AddFloat(20, 2, CSTR("Nominal Value(mA)"), ReadUInt16(&packBuff[20]));
			}
		}
		break;
	}
	case 31:
		frame->AddHexBuff(4, (UOSInt)packBuff[1] - 4, CSTR("Boot Integrity Services (BIS)"), &packBuff[4], true);
		break;
	case 32:
		if (packBuff[1] > 9) frame->AddHexBuff(4, 6, CSTR("Reserved"), &packBuff[4], false);
		if (packBuff[1] > 10)
		{
			frame->AddUIntName(10, 1, CSTR("Boot Status"), packBuff[10], IO::SMBIOS::GetSystemBootStatus(packBuff[10]));
			if (packBuff[1] > 11)
			{
				frame->AddHexBuff(11, (UOSInt)packBuff[1] - 11, CSTR("Additional Boot Status"), &packBuff[11], true);
			}
		}
		break;
	case 34:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Description"));
		const Char *names34_1[] = {"Unspecified", "Other", "Unknown", "National Semiconductor LM75", "National Semiconductor LM78", "National Semiconductor LM79", "National Semiconductor LM80", "National Semiconductor LM81",
			"Analog Devices ADM9240", "Dallas Semiconductor DS1780", "Maxim 1617", "Genesys GL518SM", "Winbond W83781D", "Holtek HT82H791"};
		AddEnum(frame, 5, packBuff, carr, CSTR("Type"), names34_1, sizeof(names34_1) / sizeof(names34_1[0]));
		AddHex32(frame, 6, packBuff, carr, CSTR("Address"));
		const Char *names34_2[] = {"Unspecified", "Other", "Unknown", "I/O Port", "Memory", "SM Bus"};
		AddEnum(frame, 10, packBuff, carr, CSTR("Address Type"), names34_2, sizeof(names34_2) / sizeof(names34_2[0]));
		break;
	}
	case 35:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Description"));
		AddHex16(frame, 5, packBuff, carr, CSTR("Management Device Handle"));
		AddHex16(frame, 7, packBuff, carr, CSTR("Component Handle"));
		AddHex16(frame, 9, packBuff, carr, CSTR("Threshold Handle"));
		break;
	}
	case 36:
	{
		AddUInt16(frame, 4, packBuff, carr, CSTR("Lower Threshold - Non-critical"));
		AddUInt16(frame, 6, packBuff, carr, CSTR("Upper Threshold - Non-critical"));
		AddUInt16(frame, 8, packBuff, carr, CSTR("Lower Threshold - Critical"));
		AddUInt16(frame, 10, packBuff, carr, CSTR("Upper Threshold - Critical"));
		AddUInt16(frame, 12, packBuff, carr, CSTR("Lower Threshold - Non-recoverable"));
		AddUInt16(frame, 14, packBuff, carr, CSTR("Upper Threshold - Non-recoverable"));
		break;
	}
	case 39:
	{
		AddUInt8(frame, 4, packBuff, carr, CSTR("Power Unit Group"));
		AddString(frame, 5, packBuff, carr, CSTR("Location"));
		AddString(frame, 6, packBuff, carr, CSTR("Device Name"));
		AddString(frame, 7, packBuff, carr, CSTR("Manufacturer"));
		AddString(frame, 8, packBuff, carr, CSTR("Serial Number"));
		AddString(frame, 9, packBuff, carr, CSTR("Asset Tag Number"));
		AddString(frame, 10, packBuff, carr, CSTR("Model Part Number"));
		AddString(frame, 11, packBuff, carr, CSTR("Revision Level"));
		AddUInt16(frame, 12, packBuff, carr, CSTR("Max Power Capacity (W)"));
		if (packBuff[1] > 15)
		{
			frame->AddBit(14, CSTR("power supply is hot-replaceable"), packBuff[14], 0);
			frame->AddBit(14, CSTR("power supply is present"), packBuff[14], 1);
			frame->AddBit(14, CSTR("power supply is unplugged from the wall"), packBuff[14], 2);
			const Char *names39_1[] = {"Unspecified", "Other", "Unknown", "Manual", "Auto-switch", "Wide range", "Not applicable"};
			AddEnum(frame, 14, (packBuff[14] >> 3) & 15, carr, CSTR("DMTF Input Voltage Range Switching"), names39_1, sizeof(names39_1) / sizeof(names39_1[0]));
			const Char *names39_2[] = {"Unspecified", "Other", "Unknown", "OK", "Non-critical", "Critical"};
			AddEnum(frame, 14, (ReadUInt16(&packBuff[14]) >> 7) & 7, carr, CSTR("Status"), names39_2, sizeof(names39_2) / sizeof(names39_2[0]));
			const Char *names39_3[] = {"Unspecified", "Other", "Unknown", "Linear", "Switching", "Battery", "UPS", "Converter", "Regulator"};
			AddEnum(frame, 15, (packBuff[15] >> 2) & 15, carr, CSTR("DMTF Power Supply Type"), names39_3, sizeof(names39_3) / sizeof(names39_3[0]));
			frame->AddBit(15, CSTR("Reserved"), packBuff[15], 6);
			frame->AddBit(15, CSTR("Reserved"), packBuff[15], 7);
		}
		AddHex16(frame, 16, packBuff, carr, CSTR("Input Voltage Probe Handle"));
		AddHex16(frame, 18, packBuff, carr, CSTR("Cooling Device Handle"));
		AddHex16(frame, 20, packBuff, carr, CSTR("Input Current Probe Handle"));
		break;
	}
	case 40:
	{
		UOSInt n = 0;
		UOSInt i = 5;
		AddUInt8(frame, 4, packBuff, carr, CSTR("Number of Additional Information entries"));
		if (packBuff[1] > 4)
			n = packBuff[4];
		while (n-- > 0)
		{
			UOSInt len;
			if (packBuff[1] <= i)
				break;
			len = packBuff[i];
			AddUInt8(frame, i, packBuff, carr, CSTR("Entry Length"));
			AddHex16(frame, i + 1, packBuff, carr, CSTR("Referenced Handle"));
			AddHex16(frame, i + 3, packBuff, carr, CSTR("Referenced Offset"));
			AddString(frame, i + 5, packBuff, carr, CSTR("String"));
			if (len > 6)
			{
				if (i + len <= packBuff[1])
				{
					frame->AddHexBuff(i + 6, len - 6, CSTR("Value"), &packBuff[i + 6], true);
				}
			}
			i += len;
		}
		break;
	}
	case 41:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Reference Designation"));
		frame->AddBit(5, CSTR("Device Enabled"), packBuff[5], 7);
		const Char *names41_1[] = {"Unspecified", "Other", "Unknown", "Video", "SCSI Controller", "Ethernet", "Token Ring", "Sound",
			"PATA Controller", "SATA Controller", "SAS Controller", "Wireless LAN", "Bluetooth", "WWAN", "eMMC", "NVMe Controller",
			"UFS Controller"};
		AddEnum(frame, 5, packBuff[5] & 0x7F, carr, CSTR("Device Type"), names41_1, sizeof(names41_1)/ sizeof(names41_1[0]));
		AddUInt8(frame, 6, packBuff, carr, CSTR("Device Type Instance"));
		AddHex16(frame, 7, packBuff, carr, CSTR("Segment Group Number"));
		AddHex8(frame, 9, packBuff, carr, CSTR("Bus Number"));
		frame->AddUInt(10, 1, CSTR("Device number"), (UOSInt)packBuff[10] >> 3);
		frame->AddUInt(10, 1, CSTR("Function number"), packBuff[10] & 7);
		break;
	}
	case 43:
	{
		frame->AddStrS(4, 4, CSTR("Vendor ID"), &packBuff[4]);
		AddUInt8(frame, 8, packBuff, carr, CSTR("Major Spec Version"));
		AddUInt8(frame, 9, packBuff, carr, CSTR("Minor Spec Version"));
		AddHex32(frame, 10, packBuff, carr, CSTR("Firmware Version 1"));
		AddHex32(frame, 14, packBuff, carr, CSTR("Firmware Version 2"));
		AddString(frame, 18, packBuff, carr, CSTR("Description"));
		const Char *names43_1[] = {"Reserved", "Reserved", "PM Device Characteristics are not supported", "Family configurable via firmware update", "Family configurable via platform software support", "Family configurable via OEM proprietary mechanism", "Reserved", "Reserved"};
		const Char *names43_2[] = {"Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"};
		AddBits(frame, 19, packBuff, carr, names43_1);
		AddBits(frame, 20, packBuff, carr, names43_2);
		AddBits(frame, 21, packBuff, carr, names43_2);
		AddBits(frame, 22, packBuff, carr, names43_2);
		AddBits(frame, 23, packBuff, carr, names43_2);
		AddBits(frame, 24, packBuff, carr, names43_2);
		AddBits(frame, 25, packBuff, carr, names43_2);
		AddBits(frame, 26, packBuff, carr, names43_2);
		AddHex32(frame, 27, packBuff, carr, CSTR("OEM-defined"));
		break;
	}
	case 44:
	{
		AddHex16(frame, 4, packBuff, carr, CSTR("Referenced Handle"));
		AddUInt8(frame, 6, packBuff, carr, CSTR("Block Length"));
		const Char *names44_1[] = {"Reserved", "IA32", "x64", "IA64", "32-bit ARM", "64-bit ARM", "32-bit RISC-V", "64-bit RISC-V",
			"128-bit RISC-V", "32-bit LoongArch", "64-bit LoongArch"};
		AddEnum(frame, 7, packBuff, carr, CSTR("Processor Type"), names44_1, sizeof(names44_1) / sizeof(names44_1[0]));
		if (packBuff[6] > 0)
		{
			frame->AddHexBuff(8, packBuff[6], CSTR("Processor-Specific Data"), &packBuff[8], true);
		}
		break;
	}
	case 45:
	{
		AddString(frame, 4, packBuff, carr, CSTR("Firmware Component Name"));
		AddString(frame, 5, packBuff, carr, CSTR("Firmware Version"));
		const Char *names45_1[] = {"Free-form string", "MAJOR.MINOR", "Hex32", "Hex64"};
		AddEnum(frame, 6, packBuff, carr, CSTR("Version Format"), names45_1, sizeof(names45_1) / sizeof(names45_1[0]));
		AddString(frame, 7, packBuff, carr, CSTR("Firmware ID"));
		const Char *names45_2[] = {"Free-form string", "UEFI ESRT FwClass GUID or the UEFI Firmware Management Protocol ImageTypeId"};
		AddEnum(frame, 8, packBuff, carr, CSTR("Firmware ID Format"), names45_2, sizeof(names45_2) / sizeof(names45_2[0]));
		AddString(frame, 9, packBuff, carr, CSTR("Release Date"));
		AddString(frame, 10, packBuff, carr, CSTR("Manufacturer"));
		AddString(frame, 11, packBuff, carr, CSTR("Lowest Supported Firmware Version"));
		AddUInt64(frame, 12, packBuff, carr, CSTR("Image Size"));
		const Char *names45_3[] = {"Updatable", "Write-Protect", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"};
		const Char *names45_4[] = {"Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"};
		AddBits(frame, 20, packBuff, carr, names45_3);
		AddBits(frame, 21, packBuff, carr, names45_4);
		const Char *names45_5[] = {"Reserved", "Other", "Unknown", "Disabled", "Enabled", "Absent", "StandbyOffline", "StandbySpare", "UnavailableOffline"};
		AddEnum(frame, 22, packBuff, carr, CSTR("State"), names45_5, sizeof(names45_5) / sizeof(names45_5[0]));
		AddUInt8(frame, 23, packBuff, carr, CSTR("Number of Associated Components (n)"));
		if (packBuff[1] >= 24 + packBuff[23] * 2)
		{
			UOSInt i = 0;
			while (i < packBuff[23])
			{
				AddHex16(frame, 24 + i * 2, packBuff, carr, CSTR("Associated Component Handles"));
				i++;
			}
		}
		break;
	}
	case 126: //Inactive
	case 128:
	case 129:
	case 130:
	case 131:
	case 133:
	case 135:
	case 136:
	case 177:
	case 178:
	case 203: //MEI?
	case 205:
	case 208:
	case 216: //CPU?
	case 217: //Keyboard?
	case 218:
	case 219:
	case 220:
	case 221:
	case 222:
		frame->AddHexBuff(4, (UOSInt)packBuff[1] - 4, CSTR("Unknown data"), &packBuff[4], true);
		break;
	}
	k = 1;
	while (carr[k].v.Ptr())
	{
		sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("String ")), k);
		frame->AddField((UOSInt)(carr[k].v.Ptr() - packBuff.Arr().Ptr()), carr[k].leng + 1, CSTRP(sbuff, sptr), carr[k]);
		k++;
	}
	if (k == 1)
	{
		frame->AddHexBuff(packBuff[1], 2, CSTR("End of String Table"), &packBuff[packBuff[1]], false);
	}
	else
	{
		frame->AddUInt((UOSInt)(carr[k - 1].v.Ptr() - packBuff.Arr().Ptr()) + carr[k - 1].leng + 1, 1, CSTR("End of String Table"), 0);
	}
	return frame;
}

Bool IO::FileAnalyse::SMBIOSFileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::SMBIOSFileAnalyse::IsParsing()
{
	return false;
}

Bool IO::FileAnalyse::SMBIOSFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}

Text::CStringNN IO::FileAnalyse::SMBIOSFileAnalyse::SMBIOSTypeGetName(UInt8 type)
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

Text::CString IO::FileAnalyse::SMBIOSFileAnalyse::MemoryLocationGetName(UInt8 location)
{
	switch (location)
	{
	case 0:
		return CSTR("Unspecified");
	case 1:
		return CSTR("Other");
	case 2:
		return CSTR("Unknown");
	case 3:
		return CSTR("System board or motherboard");
	case 4:
		return CSTR("ISA add-on card");
	case 5:
		return CSTR("EISA add-on card");
	case 6:
		return CSTR("PCI add-on card");
	case 7:
		return CSTR("MCA add-on card");
	case 8:
		return CSTR("PCMCIA add-on card");
	case 9:
		return CSTR("Proprietary add-on card");
	case 10:
		return CSTR("NuBus");
	case 0xA0:
		return CSTR("PC-98/C20 add-on card");
	case 0xA1:
		return CSTR("PC-98/C24 add-on card");
	case 0xA2:
		return CSTR("PC-98/E add-on card");
	case 0xA3:
		return CSTR("PC-98/Local bus add-on card");
	case 0xA4:
		return CSTR("CXL add-on card");
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::FileAnalyse::SMBIOSFileAnalyse::PointingDeviceInterfaceGetName(UInt8 v)
{
	switch (v)
	{
	case 0:
		return CSTR("Unspecified");
	case 1:
		return CSTR("Other");
	case 2:
		return CSTR("Unknown");
	case 3:
		return CSTR("Serial");
	case 4:
		return CSTR("PS/2");
	case 5:
		return CSTR("Infrared");
	case 6:
		return CSTR("HP-HIL");
	case 7:
		return CSTR("Bus mouse");
	case 8:
		return CSTR("ADB");
	case 0xA0:
		return CSTR("Bus mouse DB-9");
	case 0xA1:
		return CSTR("Bus mouse micro-DIN");
	case 0xA2:
		return CSTR("USB");
	case 0xA3:
		return CSTR("I2C");
	case 0xA4:
		return CSTR("SPI");
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::FileAnalyse::SMBIOSFileAnalyse::SlotTypeGetName(UInt8 v)
{
	switch (v)
	{
	case 0:
		return CSTR("Unspecified");
	case 1:
		return CSTR("Other");
	case 2:
		return CSTR("Unknown");
	case 3:
		return CSTR("ISA");
	case 4:
		return CSTR("MCA");
	case 5:
		return CSTR("EISA");
	case 6:
		return CSTR("PCI");
	case 7:
		return CSTR("PC Card");
	case 8:
		return CSTR("VL-VESA");
	case 9:
		return CSTR("Proprietary");
	case 0xA:
		return CSTR("Processor Card Slot");
	case 0xB:
		return CSTR("Proprietary Memory Card Slot");
	case 0xC:
		return CSTR("I/O Riser Card Slot");
	case 0xD:
		return CSTR("NuBus");
	case 0xE:
		return CSTR("PCI - 66MHz Capable");
	case 0xF:
		return CSTR("AGP");
	case 0x10:
		return CSTR("AGP 2X");
	case 0x11:
		return CSTR("AGP 4X");
	case 0x12:
		return CSTR("PCI-X");
	case 0x13:
		return CSTR("AGP 8X");
	case 0x14:
		return CSTR("M.2 Socket 1-DP");
	case 0x15:
		return CSTR("M.2 Socket 1-SD");
	case 0x16:
		return CSTR("M.2 Socket 2");
	case 0x17:
		return CSTR("M.2 Socket 3");
	case 0x18:
		return CSTR("MXM Type I");
	case 0x19:
		return CSTR("MXM Type II");
	case 0x1A:
		return CSTR("MXM Type III (standard connector)");
	case 0x1B:
		return CSTR("MXM Type III (HE connector)");
	case 0x1C:
		return CSTR("MXM Type IV");
	case 0x1D:
		return CSTR("MXM 3.0 Type A");
	case 0x1E:
		return CSTR("MXM 3.0 Type B");
	case 0x1F:
		return CSTR("PCI Express Gen 2 SFF-8639 (U.2)");
	case 0x20:
		return CSTR("PCI Express Gen 3 SFF-8639 (U.2)");
	case 0x21:
		return CSTR("PCI Express Mini 52-pin (CEM spec. 2.0) with bottom-side keep-outs");
	case 0x22:
		return CSTR("PCI Express Mini 52-pin (CEM spec. 2.0) without bottom-side keep-outs");
	case 0x23:
		return CSTR("PCI Express Mini 76-pin (CEM spec. 2.0) Corresponds to Display-Mini card");
	case 0x24:
		return CSTR("PCI Express Gen 4 SFF-8639 (U.2)");
	case 0x25:
		return CSTR("PCI Express Gen 5 SFF-8639 (U.2)");
	case 0x26:
		return CSTR("OCP NIC 3.0 Small Form Factor (SFF)");
	case 0x27:
		return CSTR("OCP NIC 3.0 Large Form Factor (LFF)");
	case 0x28:
		return CSTR("OCP NIC Prior to 3.0");
	case 0x30:
		return CSTR("CXL Flexbus 1.0");
	case 0xA0:
		return CSTR("PC-98/C20");
	case 0xA1:
		return CSTR("PC-98/C24");
	case 0xA2:
		return CSTR("PC-98/E");
	case 0xA3:
		return CSTR("PC-98/Local Bus");
	case 0xA4:
		return CSTR("PC-98/Card");
	case 0xA5:
		return CSTR("PCI Express (see note below)");
	case 0xA6:
		return CSTR("PCI Express x1");
	case 0xA7:
		return CSTR("PCI Express x2");
	case 0xA8:
		return CSTR("PCI Express x4");
	case 0xA9:
		return CSTR("PCI Express x8");
	case 0xAA:
		return CSTR("PCI Express x16");
	case 0xAB:
		return CSTR("PCI Express Gen 2");
	case 0xAC:
		return CSTR("PCI Express Gen 2 x1");
	case 0xAD:
		return CSTR("PCI Express Gen 2 x2");
	case 0xAE:
		return CSTR("PCI Express Gen 2 x4");
	case 0xAF:
		return CSTR("PCI Express Gen 2 x8");
	case 0xB0:
		return CSTR("PCI Express Gen 2 x16");
	case 0xB1:
		return CSTR("PCI Express Gen 3");
	case 0xB2:
		return CSTR("PCI Express Gen 3 x1");
	case 0xB3:
		return CSTR("PCI Express Gen 3 x2");
	case 0xB4:
		return CSTR("PCI Express Gen 3 x4");
	case 0xB5:
		return CSTR("PCI Express Gen 3 x8");
	case 0xB6:
		return CSTR("PCI Express Gen 3 x16");
	case 0xB8:
		return CSTR("PCI Express Gen 4");
	case 0xB9:
		return CSTR("PCI Express Gen 4 x1");
	case 0xBA:
		return CSTR("PCI Express Gen 4 x2");
	case 0xBB:
		return CSTR("PCI Express Gen 4 x4");
	case 0xBC:
		return CSTR("PCI Express Gen 4 x8");
	case 0xBD:
		return CSTR("PCI Express Gen 4 x16");
	case 0xBE:
		return CSTR("PCI Express Gen 5");
	case 0xBF:
		return CSTR("PCI Express Gen 5 x1");
	case 0xC0:
		return CSTR("PCI Express Gen 5 x2");
	case 0xC1:
		return CSTR("PCI Express Gen 5 x4");
	case 0xC2:
		return CSTR("PCI Express Gen 5 x8");
	case 0xC3:
		return CSTR("PCI Express Gen 5 x16");
	case 0xC4:
		return CSTR("PCI Express Gen 6 and Beyond");
	case 0xC5:
		return CSTR("Enterprise and Datacenter 1U E1 Form Factor Slot (EDSFF E1.S, E1.L)");
	case 0xC6:
		return CSTR("Enterprise and Datacenter 3\" E3 Form Factor Slot (EDSFF E3.S, E3.L)");
	default:
		return CSTR("Unknown");
	}
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddString(NN<FrameDetail> frame, UOSInt ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name)
{
	if (ofst >= packBuff[1])
		return;
	UInt8 ind = packBuff[ofst];
	Text::CString val;
	if (ind >= 32)
	{
		val = nullptr;
	}
	else
	{
		val = carr[ind];
	}
	if (val.v.NotNull())
	{
		frame->AddUIntName(ofst, 1, name, ind, val);
	}
	else
	{
		frame->AddUIntName(ofst, 1, name, ind, CSTR("-"));
	}
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddHex8(NN<FrameDetail> frame, UOSInt ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name)
{
	if (ofst >= packBuff[1])
		return;
	frame->AddHex8(ofst, name, packBuff[ofst]);
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddHex16(NN<FrameDetail> frame, UOSInt ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name)
{
	if (ofst + 1 >= packBuff[1])
		return;
	frame->AddHex16(ofst, name, ReadUInt16(&packBuff[ofst]));
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddHex32(NN<FrameDetail> frame, UOSInt ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name)
{
	if (ofst + 3 >= packBuff[1])
		return;
	frame->AddHex32(ofst, name, ReadUInt32(&packBuff[ofst]));
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddHex64(NN<FrameDetail> frame, UOSInt ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name)
{
	if (ofst + 7 >= packBuff[1])
		return;
	frame->AddHex64(ofst, name, ReadUInt64(&packBuff[ofst]));
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddUInt8(NN<FrameDetail> frame, UOSInt ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name)
{
	if (ofst >= packBuff[1])
		return;
	frame->AddUInt(ofst, 1, name, packBuff[ofst]);
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddUInt16(NN<FrameDetail> frame, UOSInt ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name)
{
	if (ofst + 1 >= packBuff[1])
		return;
	frame->AddUInt(ofst, 2, name, ReadUInt16(&packBuff[ofst]));
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddUInt32(NN<FrameDetail> frame, UOSInt ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name)
{
	if (ofst + 3 >= packBuff[1])
		return;
	frame->AddUInt(ofst, 4, name, ReadUInt32(&packBuff[ofst]));
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddUInt64(NN<FrameDetail> frame, UOSInt ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name)
{
	if (ofst + 7 >= packBuff[1])
		return;
	frame->AddUInt64(ofst, name, ReadUInt64(&packBuff[ofst]));
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddUUID(NN<FrameDetail> frame, UOSInt ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name)
{
	if (ofst + 15 >= packBuff[1])
		return;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Data::UUID uuid(&packBuff[ofst]);
	sptr = uuid.ToString(sbuff);
	frame->AddField(ofst, 16, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddDate(NN<FrameDetail> frame, UOSInt ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name)
{
	if (ofst + 1 >= packBuff[1])
		return;
	UInt16 val = ReadUInt16(&packBuff[ofst]);
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
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
			
void IO::FileAnalyse::SMBIOSFileAnalyse::AddBits(NN<FrameDetail> frame, UOSInt ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, const Char *bitNames[])
{
	if (ofst >= packBuff[1])
		return;
	UOSInt i = 0;
	UInt8 val = packBuff[ofst];
	while (i < 8)
	{
		frame->AddBit(ofst, Text::CStringNN::FromPtr((const UTF8Char*)bitNames[i]), val, i);
		i++;
	}
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddEnum(NN<FrameDetail> frame, UOSInt ofst, Data::ByteArrayR packBuff, UnsafeArray<Text::CString> carr, Text::CStringNN name, const Char *names[], UOSInt namesCnt)
{
	if (ofst >= packBuff[1])
		return;
	AddEnum(frame, ofst, packBuff[ofst], carr, name, names, namesCnt);
}

void IO::FileAnalyse::SMBIOSFileAnalyse::AddEnum(NN<FrameDetail> frame, UOSInt ofst, UInt8 val, UnsafeArray<Text::CString> carr, Text::CStringNN name, const Char *names[], UOSInt namesCnt)
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
