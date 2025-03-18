#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/EXEFileAnalyse.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall IO::FileAnalyse::EXEFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::EXEFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::EXEFileAnalyse>();
	UInt8 buff[256];
	NN<IO::FileAnalyse::EXEFileAnalyse::PackInfo> pack;
	UInt32 val;
	pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
	pack->fileOfst = 0;
	pack->packSize = 64;
	pack->packType = 0;
	me->packs.Add(pack);
	me->fd->GetRealData(0, 64, BYTEARR(buff));
	val = ReadUInt32(&buff[60]);
	if (val > 64)
	{
		me->fd->GetRealData(val, 128, BYTEARR(buff));
		if (buff[0] == 'P' && buff[1] == 'E' && buff[2] == 0 && buff[3] == 0)
		{
			pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
			pack->fileOfst = 64;
			pack->packSize = val - 64;
			pack->packType = 1;
			me->packs.Add(pack);

			pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
			pack->fileOfst = val;
			pack->packSize = 24;
			pack->packType = 2;
			me->packs.Add(pack);

			UInt16 optHdrSize = ReadUInt16(&buff[20]);
			UOSInt nSection = ReadUInt16(&buff[6]);
			UOSInt tableOfst = 0;
			if (optHdrSize > 0)
			{
				me->imageSize = ReadUInt32(&buff[80]);
				if (ReadUInt16(&buff[24]) == 0x10b)
				{
					tableOfst = val + 24 + 96;
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = val + 24;
					pack->packSize = optHdrSize;
					pack->packType = 3;
					me->packs.Add(pack);
				}
				else if (ReadUInt16(&buff[24]) == 0x20b)
				{
					tableOfst = val + 24 + 112;
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = val + 24;
					pack->packSize = optHdrSize;
					pack->packType = 4;
					me->packs.Add(pack);
				}
				if (me->imageSize > 0)
				{
					me->imageBuff.ChangeSizeAndClear(me->imageSize);
				}
			}
			UOSInt ofst = val + 24 + optHdrSize;
			UOSInt i = 0;
			UInt32 virtualSize;
			UInt32 sizeOfRawData;
			UInt32 virtualAddr;
			while (i < nSection)
			{
				pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
				pack->fileOfst = ofst;
				pack->packSize = 40;
				pack->packType = 5;
				me->packs.Add(pack);

				me->fd->GetRealData(ofst, 40, BYTEARR(buff));
				virtualSize = ReadUInt32(&buff[8]);
				sizeOfRawData = ReadUInt32(&buff[16]);
				if (me->imageBuff.GetSize() > 0)
				{
					virtualAddr = ReadUInt32(&buff[12]);
					if (virtualSize > sizeOfRawData)
					{
						me->fd->GetRealData(ReadUInt32(&buff[20]), sizeOfRawData, me->imageBuff.SubArray(virtualAddr));
						MemClear(&me->imageBuff[virtualAddr + sizeOfRawData], virtualSize - sizeOfRawData);
					}
					else
					{
						me->fd->GetRealData(ReadUInt32(&buff[20]), virtualSize, me->imageBuff.SubArray(virtualAddr));
					}
				}

				ofst += 40;
				i++;
			}

			if (optHdrSize > 0 && tableOfst != 0)
			{
				me->fd->GetRealData(tableOfst, 128, BYTEARR(buff));
				virtualAddr = ReadUInt32(&buff[0]);
				virtualSize = ReadUInt32(&buff[4]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 6;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[8]);
				virtualSize = ReadUInt32(&buff[12]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 7;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[16]);
				virtualSize = ReadUInt32(&buff[20]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 8;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[24]);
				virtualSize = ReadUInt32(&buff[28]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 9;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[32]);
				virtualSize = ReadUInt32(&buff[36]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 10;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[40]);
				virtualSize = ReadUInt32(&buff[44]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 11;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[48]);
				virtualSize = ReadUInt32(&buff[52]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 12;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[56]);
				virtualSize = ReadUInt32(&buff[60]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 13;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[64]);
				virtualSize = ReadUInt32(&buff[68]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 14;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[72]);
				virtualSize = ReadUInt32(&buff[76]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 15;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[80]);
				virtualSize = ReadUInt32(&buff[84]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 16;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[88]);
				virtualSize = ReadUInt32(&buff[92]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 17;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[96]);
				virtualSize = ReadUInt32(&buff[100]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 18;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[104]);
				virtualSize = ReadUInt32(&buff[108]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 19;
					me->packs.Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[112]);
				virtualSize = ReadUInt32(&buff[116]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAllocNN(IO::FileAnalyse::EXEFileAnalyse::PackInfo);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 20;
					me->packs.Add(pack);
				}
			}
		}
	}
}

IO::FileAnalyse::EXEFileAnalyse::EXEFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("EXEFileAnalyse"))
{
	UInt8 buff[8];
	this->fd = 0;
	this->pauseParsing = false;
	this->imageSize = 0;
	fd->GetRealData(0, 8, BYTEARR(buff));
	if (ReadInt16(buff) != 0x5A4D)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::EXEFileAnalyse::~EXEFileAnalyse()
{
	this->thread.Stop();
	SDEL_CLASS(this->fd);
	this->packs.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::EXEFileAnalyse::GetFormatName()
{
	return CSTR("EXE");
}

UOSInt IO::FileAnalyse::EXEFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::EXEFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::EXEFileAnalyse::PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->Append(PackTypeGetName(pack->packType));
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendU64(pack->packSize);
	return true;
}

Bool IO::FileAnalyse::EXEFileAnalyse::GetFrameDetail(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::EXEFileAnalyse::PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
		return false;

	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->Append(PackTypeGetName(pack->packType));
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendU64(pack->packSize);
	sb->AppendC(UTF8STRC("\r\n"));

	if (pack->packType == 0)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);

		sb->AppendC(UTF8STRC("Magic number = 0x"));
		sb->AppendHex16(ReadUInt16(&packBuff[0]));
		sb->AppendC(UTF8STRC("\r\nBytes on last page of file = "));
		sb->AppendU16(ReadUInt16(&packBuff[2]));
		sb->AppendC(UTF8STRC("\r\nPages in file = "));
		sb->AppendU16(ReadUInt16(&packBuff[4]));
		sb->AppendC(UTF8STRC("\r\nRelocations = "));
		sb->AppendU16(ReadUInt16(&packBuff[6]));
		sb->AppendC(UTF8STRC("\r\nSize of header in paragraphs = "));
		sb->AppendU16(ReadUInt16(&packBuff[8]));
		sb->AppendC(UTF8STRC("\r\nMinimum extra paragraphs needed = "));
		sb->AppendU16(ReadUInt16(&packBuff[10]));
		sb->AppendC(UTF8STRC("\r\nMaximum extra paragraphs needed = "));
		sb->AppendU16(ReadUInt16(&packBuff[12]));
		sb->AppendC(UTF8STRC("\r\nInitial (relative) SS value = 0x"));
		sb->AppendHex16(ReadUInt16(&packBuff[14]));
		sb->AppendC(UTF8STRC("\r\nInitial SP value = 0x"));
		sb->AppendHex16(ReadUInt16(&packBuff[16]));
		sb->AppendC(UTF8STRC("\r\nChecksum = 0x"));
		sb->AppendHex16(ReadUInt16(&packBuff[18]));
		sb->AppendC(UTF8STRC("\r\nInitial IP value = 0x"));
		sb->AppendHex16(ReadUInt16(&packBuff[20]));
		sb->AppendC(UTF8STRC("\r\nInitial (relative) CS value = 0x"));
		sb->AppendHex16(ReadUInt16(&packBuff[22]));
		sb->AppendC(UTF8STRC("\r\nFile address of relocation table = 0x"));
		sb->AppendHex16(ReadUInt16(&packBuff[24]));
		sb->AppendC(UTF8STRC("\r\nOverlay number = "));
		sb->AppendU16(ReadUInt16(&packBuff[26]));
		sb->AppendC(UTF8STRC("\r\nOEM identifier = "));
		sb->AppendU16(ReadUInt16(&packBuff[36]));
		sb->AppendC(UTF8STRC("\r\nOEM information = "));
		sb->AppendU16(ReadUInt16(&packBuff[38]));
		sb->AppendC(UTF8STRC("\r\nFile address of new exe header = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[60]));
	}
	else if (pack->packType == 1)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		sb->AppendHexBuff(packBuff, ' ', Text::LineBreakType::CRLF);
	}
	else if (pack->packType == 2)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);

		sb->AppendC(UTF8STRC("Magic number = PE\\0\\0"));
		sb->AppendC(UTF8STRC("\r\nMachine = 0x"));
		sb->AppendHex16(ReadUInt16(&packBuff[4]));
		switch (ReadUInt16(&packBuff[4]))
		{
		case 0x0:
			sb->AppendC(UTF8STRC(" (Unknown)"));
			break;
		case 0x1d3:
			sb->AppendC(UTF8STRC(" (Matsushita AM33)"));
			break;
		case 0x8664:
			sb->AppendC(UTF8STRC(" (AMD64)"));
			break;
		case 0x1c0:
			sb->AppendC(UTF8STRC(" (ARM little endian)"));
			break;
		case 0xaa64:
			sb->AppendC(UTF8STRC(" (ARM64 little endian)"));
			break;
		case 0x1c4:
			sb->AppendC(UTF8STRC(" (ARM Thumb-2 little endian)"));
			break;
		case 0xebc:
			sb->AppendC(UTF8STRC(" (EFI byte code)"));
			break;
		case 0x14c:
			sb->AppendC(UTF8STRC(" (Intel 386 or later processors and compatible processors)"));
			break;
		case 0x200:
			sb->AppendC(UTF8STRC(" (Intel Itanium processor family)"));
			break;
		case 0x9041:
			sb->AppendC(UTF8STRC(" (Mitsubishi M32R little endian)"));
			break;
		case 0x266:
			sb->AppendC(UTF8STRC(" (MIPS16)"));
			break;
		case 0x366:
			sb->AppendC(UTF8STRC(" (MIPS with FPU)"));
			break;
		case 0x466:
			sb->AppendC(UTF8STRC(" (MIPS16 with FPU)"));
			break;
		case 0x1f0:
			sb->AppendC(UTF8STRC(" (Power PC little endian)"));
			break;
		case 0x1f1:
			sb->AppendC(UTF8STRC(" (Power PC with floating point support)"));
			break;
		case 0x166:
			sb->AppendC(UTF8STRC(" (MIPS little endian)"));
			break;
		case 0x5032:
			sb->AppendC(UTF8STRC(" (RISC-V 32-bit address space)"));
			break;
		case 0x5064:
			sb->AppendC(UTF8STRC(" (RISC-V 64-bit address space)"));
			break;
		case 0x5128:
			sb->AppendC(UTF8STRC(" (RISC-V 128-bit address space)"));
			break;
		case 0x1a2:
			sb->AppendC(UTF8STRC(" (Hitachi SH3)"));
			break;
		case 0x1a3:
			sb->AppendC(UTF8STRC(" (Hitachi SH3 DSP)"));
			break;
		case 0x1a6:
			sb->AppendC(UTF8STRC(" (Hitachi SH4)"));
			break;
		case 0x1a8:
			sb->AppendC(UTF8STRC(" (Hitachi SH5)"));
			break;
		case 0x1c2:
			sb->AppendC(UTF8STRC(" (Thumb)"));
			break;
		case 0x169:
			sb->AppendC(UTF8STRC(" (MIPS little-endian WCE v2)"));
			break;
		}
		sb->AppendC(UTF8STRC("\r\nNumberOfSections = "));
		sb->AppendU16(ReadUInt16(&packBuff[6]));
		sb->AppendC(UTF8STRC("\r\nTimeDateStamp = "));
		sb->AppendU32(ReadUInt32(&packBuff[8]));
		sb->AppendC(UTF8STRC(" ("));
		sb->AppendTSNoZone(Data::Timestamp::FromEpochSec(ReadUInt32(&packBuff[8]), 0));
		sb->AppendC(UTF8STRC(")"));
		sb->AppendC(UTF8STRC("\r\nPointerToSymbolTable = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[12]));
		sb->AppendC(UTF8STRC("\r\nNumberOfSymbols = "));
		sb->AppendU32(ReadUInt32(&packBuff[16]));
		sb->AppendC(UTF8STRC("\r\nSizeOfOptionalHeader = "));
		sb->AppendU16(ReadUInt16(&packBuff[20]));
		sb->AppendC(UTF8STRC("\r\nCharacteristics = 0x"));
		UInt16 ch = ReadUInt16(&packBuff[22]);
		sb->AppendHex16(ch);
		if (ch & 0x0001) sb->AppendC(UTF8STRC(" RELOCS_STRIPPED"));
		if (ch & 0x0002) sb->AppendC(UTF8STRC(" EXECUTABLE_IMAGE"));
		if (ch & 0x0004) sb->AppendC(UTF8STRC(" LINE_NUMS_STRIPPED"));
		if (ch & 0x0008) sb->AppendC(UTF8STRC(" LOCAL_SYMS_STRIPPED"));
		if (ch & 0x0010) sb->AppendC(UTF8STRC(" AGGRESSIVE_WS_TRIM"));
		if (ch & 0x0020) sb->AppendC(UTF8STRC(" LARGE_ADDRESS_ AWARE"));
		if (ch & 0x0040) sb->AppendC(UTF8STRC(" RESERVED"));
		if (ch & 0x0080) sb->AppendC(UTF8STRC(" BYTES_REVERSED_LO"));
		if (ch & 0x0100) sb->AppendC(UTF8STRC(" 32BIT_MACHINE"));
		if (ch & 0x0200) sb->AppendC(UTF8STRC(" DEBUG_STRIPPED"));
		if (ch & 0x0400) sb->AppendC(UTF8STRC(" REMOVABLE_RUN_ FROM_SWAP"));
		if (ch & 0x0800) sb->AppendC(UTF8STRC(" NET_RUN_FROM_SWAP"));
		if (ch & 0x1000) sb->AppendC(UTF8STRC(" SYSTEM"));
		if (ch & 0x2000) sb->AppendC(UTF8STRC(" DLL"));
		if (ch & 0x4000) sb->AppendC(UTF8STRC(" UP_SYSTEM_ONLY"));
		if (ch & 0x8000) sb->AppendC(UTF8STRC(" BYTES_REVERSED_HI"));
	}
	else if (pack->packType == 3 || pack->packType == 4)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);

		sb->AppendC(UTF8STRC("Magic number = 0x"));
		sb->AppendHex16(ReadUInt16(&packBuff[0]));
		sb->AppendC(UTF8STRC("\r\nLinkerVersion = "));
		sb->AppendU16(packBuff[2]);
		sb->AppendC(UTF8STRC("."));
		sb->AppendU16(packBuff[3]);
		sb->AppendC(UTF8STRC("\r\nSizeOfCode = "));
		sb->AppendU32(ReadUInt32(&packBuff[4]));
		sb->AppendC(UTF8STRC("\r\nSizeOfInitializedData = "));
		sb->AppendU32(ReadUInt32(&packBuff[8]));
		sb->AppendC(UTF8STRC("\r\nSizeOfUninitializedData = "));
		sb->AppendU32(ReadUInt32(&packBuff[12]));
		sb->AppendC(UTF8STRC("\r\nAddressOfEntryPoint = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[16]));
		sb->AppendC(UTF8STRC("\r\nBaseOfCode = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[20]));
		if (pack->packType == 3)
		{
			sb->AppendC(UTF8STRC("\r\nBaseOfData = 0x"));
			sb->AppendHex32(ReadUInt32(&packBuff[24]));
			sb->AppendC(UTF8STRC("\r\nImageBase = 0x"));
			sb->AppendHex32(ReadUInt32(&packBuff[28]));
		}
		else
		{
			sb->AppendC(UTF8STRC("\r\nImageBase = 0x"));
			sb->AppendHex64(ReadUInt64(&packBuff[24]));
		}
		sb->AppendC(UTF8STRC("\r\nSectionAlignment = "));
		sb->AppendU32(ReadUInt32(&packBuff[32]));
		sb->AppendC(UTF8STRC("\r\nFileAlignment = "));
		sb->AppendU32(ReadUInt32(&packBuff[36]));
		sb->AppendC(UTF8STRC("\r\nOperatingSystemVersion = "));
		sb->AppendU16(ReadUInt16(&packBuff[40]));
		sb->AppendC(UTF8STRC("."));
		sb->AppendU16(ReadUInt16(&packBuff[42]));
		sb->AppendC(UTF8STRC("\r\nImageVersion = "));
		sb->AppendU16(ReadUInt16(&packBuff[44]));
		sb->AppendC(UTF8STRC("."));
		sb->AppendU16(ReadUInt16(&packBuff[46]));
		sb->AppendC(UTF8STRC("\r\nSubsystemVersion = "));
		sb->AppendU16(ReadUInt16(&packBuff[48]));
		sb->AppendC(UTF8STRC("."));
		sb->AppendU16(ReadUInt16(&packBuff[50]));
		sb->AppendC(UTF8STRC("\r\nWin32VersionValue = "));
		sb->AppendU32(ReadUInt32(&packBuff[52]));
		sb->AppendC(UTF8STRC("\r\nSizeOfImage = "));
		sb->AppendU32(ReadUInt32(&packBuff[56]));
		sb->AppendC(UTF8STRC("\r\nSizeOfHeaders = "));
		sb->AppendU32(ReadUInt32(&packBuff[60]));
		sb->AppendC(UTF8STRC("\r\nCheckSum = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[64]));
		sb->AppendC(UTF8STRC("\r\nSubsystem = "));
		sb->AppendU16(ReadUInt16(&packBuff[68]));
		switch (ReadUInt16(&packBuff[68]))
		{
		case 0:
			sb->AppendC(UTF8STRC(" (Unknown)"));
			break;
		case 1:
			sb->AppendC(UTF8STRC(" (Native)"));
			break;
		case 2:
			sb->AppendC(UTF8STRC(" (Windows GUI)"));
			break;
		case 3:
			sb->AppendC(UTF8STRC(" (Windows CUI)"));
			break;
		case 5:
			sb->AppendC(UTF8STRC(" (OS/2 CUI)"));
			break;
		case 7:
			sb->AppendC(UTF8STRC(" (Posix CUI)"));
			break;
		case 8:
			sb->AppendC(UTF8STRC(" (Native Win9x driver)"));
			break;
		case 9:
			sb->AppendC(UTF8STRC(" (Windows CE)"));
			break;
		case 10:
			sb->AppendC(UTF8STRC(" (EFI Application)"));
			break;
		case 11:
			sb->AppendC(UTF8STRC(" (EFI Boot Service Driver)"));
			break;
		case 12:
			sb->AppendC(UTF8STRC(" (EFI Runtime Driver)"));
			break;
		case 13:
			sb->AppendC(UTF8STRC(" (EFI ROM)"));
			break;
		case 14:
			sb->AppendC(UTF8STRC(" (XBOX)"));
			break;
		case 16:
			sb->AppendC(UTF8STRC(" (Windows Boot Application)"));
			break;
		}
		UInt16 ch = ReadUInt16(&packBuff[70]);
		sb->AppendC(UTF8STRC("\r\nDLL Characteristics = 0x"));
		sb->AppendHex16(ch);
		if (ch & 0x0020) sb->AppendC(UTF8STRC(" HIGH_ENTROPY_VA"));
		if (ch & 0x0040) sb->AppendC(UTF8STRC(" DYNAMIC_BASE"));
		if (ch & 0x0080) sb->AppendC(UTF8STRC(" FORCE_INTEGRITY"));
		if (ch & 0x0100) sb->AppendC(UTF8STRC(" NX_COMPAT"));
		if (ch & 0x0200) sb->AppendC(UTF8STRC(" NO_ISOLATION"));
		if (ch & 0x0400) sb->AppendC(UTF8STRC(" NO_SEH"));
		if (ch & 0x0800) sb->AppendC(UTF8STRC(" NO_BIND"));
		if (ch & 0x1000) sb->AppendC(UTF8STRC(" APPCONTAINER"));
		if (ch & 0x2000) sb->AppendC(UTF8STRC(" WDM_DRIVER"));
		if (ch & 0x4000) sb->AppendC(UTF8STRC(" GUARD_CF"));
		if (ch & 0x8000) sb->AppendC(UTF8STRC(" TERMINAL_SERVER_AWARE"));

		OSInt ofst;
		if (pack->packType == 3)
		{
			sb->AppendC(UTF8STRC("\r\nSizeOfStackReserve = "));
			sb->AppendU32(ReadUInt32(&packBuff[72]));
			sb->AppendC(UTF8STRC("\r\nSizeOfStackCommit = "));
			sb->AppendU32(ReadUInt32(&packBuff[76]));
			sb->AppendC(UTF8STRC("\r\nSizeOfHeapReserve = "));
			sb->AppendU32(ReadUInt32(&packBuff[80]));
			sb->AppendC(UTF8STRC("\r\nSizeOfHeapCommit = "));
			sb->AppendU32(ReadUInt32(&packBuff[84]));
			ofst = 88;
		}
		else
		{
			sb->AppendC(UTF8STRC("\r\nSizeOfStackReserve = "));
			sb->AppendU64(ReadUInt64(&packBuff[72]));
			sb->AppendC(UTF8STRC("\r\nSizeOfStackCommit = "));
			sb->AppendU64(ReadUInt64(&packBuff[80]));
			sb->AppendC(UTF8STRC("\r\nSizeOfHeapReserve = "));
			sb->AppendU64(ReadUInt64(&packBuff[88]));
			sb->AppendC(UTF8STRC("\r\nSizeOfHeapCommit = "));
			sb->AppendU64(ReadUInt64(&packBuff[96]));
			ofst = 104;
		}
		sb->AppendC(UTF8STRC("\r\nLoaderFlags = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst]));
		sb->AppendC(UTF8STRC("\r\nNumberOfRvaAndSizes = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 4]));
		sb->AppendC(UTF8STRC("\r\nExportTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 8]));
		sb->AppendC(UTF8STRC("\r\nExportTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 12]));
		sb->AppendC(UTF8STRC("\r\nImportTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 16]));
		sb->AppendC(UTF8STRC("\r\nImportTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 20]));
		sb->AppendC(UTF8STRC("\r\nResourceTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 24]));
		sb->AppendC(UTF8STRC("\r\nResourceTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 28]));
		sb->AppendC(UTF8STRC("\r\nExceptionTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 32]));
		sb->AppendC(UTF8STRC("\r\nExceptionTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 36]));
		sb->AppendC(UTF8STRC("\r\nCertificateTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 40]));
		sb->AppendC(UTF8STRC("\r\nCertificateTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 44]));
		sb->AppendC(UTF8STRC("\r\nBaseRelocationTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 48]));
		sb->AppendC(UTF8STRC("\r\nBaseRelocationTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 52]));
		sb->AppendC(UTF8STRC("\r\nDebugTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 56]));
		sb->AppendC(UTF8STRC("\r\nDebugTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 60]));
		sb->AppendC(UTF8STRC("\r\nArchitectureTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 64]));
		sb->AppendC(UTF8STRC("\r\nArchitectureTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 68]));
		sb->AppendC(UTF8STRC("\r\nGlobalPtrTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 72]));
		sb->AppendC(UTF8STRC("\r\nGlobalPtrTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 76]));
		sb->AppendC(UTF8STRC("\r\nTLSTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 80]));
		sb->AppendC(UTF8STRC("\r\nTLSTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 84]));
		sb->AppendC(UTF8STRC("\r\nLoadConfigTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 88]));
		sb->AppendC(UTF8STRC("\r\nLoadConfigTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 92]));
		sb->AppendC(UTF8STRC("\r\nBoundImportTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 96]));
		sb->AppendC(UTF8STRC("\r\nBoundImportTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 100]));
		sb->AppendC(UTF8STRC("\r\nImportAddrTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 104]));
		sb->AppendC(UTF8STRC("\r\nImportAddrTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 108]));
		sb->AppendC(UTF8STRC("\r\nDelayImportTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 112]));
		sb->AppendC(UTF8STRC("\r\nDelayImportTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 116]));
		sb->AppendC(UTF8STRC("\r\nCLRRuntimeTableVAddr = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 120]));
		sb->AppendC(UTF8STRC("\r\nCLRRuntimeTableSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 124]));
		//////////////////////
	}
	else if (pack->packType == 5)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);

		sb->AppendC(UTF8STRC("Name = "));
		sb->AppendS(packBuff.Arr().Ptr(), 8);
		sb->AppendC(UTF8STRC("\r\nVirtualSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[8]));
		sb->AppendC(UTF8STRC("\r\nVirtualAddress = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[12]));
		sb->AppendC(UTF8STRC("\r\nSizeOfRawData = "));
		sb->AppendU32(ReadUInt32(&packBuff[16]));
		sb->AppendC(UTF8STRC("\r\nPointerToRawData = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[20]));
		sb->AppendC(UTF8STRC("\r\nPointerToRelocations = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[24]));
		sb->AppendC(UTF8STRC("\r\nPointerToLinenumbers = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[28]));
		sb->AppendC(UTF8STRC("\r\nNumberOfRelocations = "));
		sb->AppendU16(ReadUInt16(&packBuff[32]));
		sb->AppendC(UTF8STRC("\r\nNumberOfLinenumbers = "));
		sb->AppendU16(ReadUInt16(&packBuff[34]));
		sb->AppendC(UTF8STRC("\r\nCharacteristics = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[36]));
	}
	else if (pack->packType == 6)
	{
		UInt32 nAddr;
		UInt32 nName;
		sb->AppendC(UTF8STRC("Export Flags = 0x"));
		sb->AppendHex32(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst]));
		sb->AppendC(UTF8STRC("\r\nTimestamp = "));
		sb->AppendU32(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 4]));
		sb->AppendC(UTF8STRC(" ("));
		sb->AppendTSNoZone(Data::Timestamp::FromEpochSec(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 4]), 0));
		sb->AppendUTF8Char(')');
		sb->AppendC(UTF8STRC("\r\nVersion = "));
		sb->AppendU16(ReadUInt16(&this->imageBuff[(UOSInt)pack->fileOfst + 8]));
		sb->AppendUTF8Char('.');
		sb->AppendU16(ReadUInt16(&this->imageBuff[(UOSInt)pack->fileOfst + 10]));
		sb->AppendC(UTF8STRC("\r\nName = "));
		sb->AppendSlow((const UTF8Char*)&this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 12])]);
		sb->AppendC(UTF8STRC("\r\nOrdinal Base = "));
		sb->AppendU32(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 16]));
		nAddr = ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 20]);
		nName = ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 24]);
		sb->AppendC(UTF8STRC("\r\nAddress Table Entries = "));
		sb->AppendU32(nAddr);
		sb->AppendC(UTF8STRC("\r\nNumber of Name Pointers = "));
		sb->AppendU32(nName);
		sb->AppendC(UTF8STRC("\r\nExport Address Table RVA = 0x"));
		sb->AppendHex32(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 28]));
		sb->AppendC(UTF8STRC("\r\nName Pointer RVA = 0x"));
		sb->AppendHex32(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 32]));
		sb->AppendC(UTF8STRC("\r\nOrdinal Table RVA = 0x"));
		sb->AppendHex32(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 36]));
		UInt8 *addrTablePtr = &this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 28])];
		UInt8 *nameTablePtr = &this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 32])];
		UInt8 *ordinalTablePtr = &this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 36])];
		UInt32 i = 0;
		while (i < nName)
		{
			sb->AppendC(UTF8STRC("\r\nAddr = 0x"));
			sb->AppendHex32(ReadUInt32(&addrTablePtr[ReadUInt16(ordinalTablePtr) * 4]));
			sb->AppendC(UTF8STRC(", Name = "));
			sb->AppendSlow((const UTF8Char*)&this->imageBuff[ReadUInt32(nameTablePtr)]);
			nameTablePtr += 4;
			ordinalTablePtr += 2;
			i++;
		}
	}
	else if (pack->packType == 7)
	{
		sb->AppendC(UTF8STRC("\r\nImport Lookup Table RVA = 0x"));
		sb->AppendHex32(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 0]));
		sb->AppendC(UTF8STRC("\r\nTimestamp = "));
		sb->AppendU32(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 4]));
		sb->AppendC(UTF8STRC(" ("));
		sb->AppendTSNoZone(Data::Timestamp::FromEpochSec(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 4]), 0));
		sb->AppendUTF8Char(')');
		sb->AppendC(UTF8STRC("\r\nForwarder Chain = "));
		sb->AppendU32(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 8]));
		sb->AppendC(UTF8STRC("\r\nName RVA = 0x"));
		sb->AppendHex32(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 12]));
		sb->AppendC(UTF8STRC("\r\nImport Address Table RVA = 0x"));
		sb->AppendHex32(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 16]));
//		UInt8 *lutPtr = &this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 0])];
//		UInt8 *nameTablePtr = &this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 12])];
//		UInt8 *iatPtr = &this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 16])];
	}
	return true;
}

UOSInt IO::FileAnalyse::EXEFileAnalyse::GetFrameIndex(UInt64 ofst)
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::EXEFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::EXEFileAnalyse::PackInfo> pack;
	Text::CString vName;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (!this->packs.GetItem(index).SetTo(pack))
		return 0;

	NN<IO::FileAnalyse::FrameDetail> frame;
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSize));
	sptr = PackTypeGetName(pack->packType).ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Type=")));
	frame->AddText(0, CSTRP(sbuff, sptr));
	sptr = Text::StrUInt64(Text::StrConcatC(sbuff, UTF8STRC("Size=")), pack->packSize);
	frame->AddText(0, CSTRP(sbuff, sptr));

	if (pack->packType == 0)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		frame->AddHex16(0, CSTR("Magic number"), ReadUInt16(&packBuff[0]));
		frame->AddUInt(2, 2, CSTR("Bytes on last page of file"), ReadUInt16(&packBuff[2]));
		frame->AddUInt(4, 2, CSTR("Pages in file"), ReadUInt16(&packBuff[4]));
		frame->AddUInt(6, 2, CSTR("Relocations"), ReadUInt16(&packBuff[6]));
		frame->AddUInt(8, 2, CSTR("Size of header in paragraphs"), ReadUInt16(&packBuff[8]));
		frame->AddUInt(10, 2, CSTR("Minimum extra paragraphs needed"), ReadUInt16(&packBuff[10]));
		frame->AddUInt(12, 2, CSTR("Maximum extra paragraphs needed"), ReadUInt16(&packBuff[12]));
		frame->AddHex16(14, CSTR("Initial (relative) SS value"), ReadUInt16(&packBuff[14]));
		frame->AddHex16(16, CSTR("Initial SP value"), ReadUInt16(&packBuff[16]));
		frame->AddHex16(18, CSTR("Checksum"), ReadUInt16(&packBuff[18]));
		frame->AddHex16(20, CSTR("Initial IP value"), ReadUInt16(&packBuff[20]));
		frame->AddHex16(22, CSTR("Initial (relative) CS value"), ReadUInt16(&packBuff[22]));
		frame->AddHex16(24, CSTR("File address of relocation table"), ReadUInt16(&packBuff[24]));
		frame->AddUInt(26, 2, CSTR("Overlay number"), ReadUInt16(&packBuff[26]));
		frame->AddUInt(36, 2, CSTR("OEM identifier"), ReadUInt16(&packBuff[36]));
		frame->AddUInt(38, 2, CSTR("OEM information"), ReadUInt16(&packBuff[38]));
		frame->AddHex32(60, CSTR("File address of new exe header"), ReadUInt32(&packBuff[60]));
	}
	else if (pack->packType == 1)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		frame->AddTextHexBuff(0, (UOSInt)pack->packSize, packBuff.Arr(), true);
	}
	else if (pack->packType == 2)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		frame->AddField(0, 4, CSTR("Magic number"), CSTR("PE\\0\\0"));
		vName = CSTR_NULL;
		switch (ReadUInt16(&packBuff[4]))
		{
		case 0x0:
			vName = CSTR("Unknown");
			break;
		case 0x1d3:
			vName = CSTR("Matsushita AM33");
			break;
		case 0x8664:
			vName = CSTR("AMD64");
			break;
		case 0x1c0:
			vName = CSTR("ARM little endian");
			break;
		case 0xaa64:
			vName = CSTR("ARM64 little endian");
			break;
		case 0x1c4:
			vName = CSTR("ARM Thumb-2 little endian");
			break;
		case 0xebc:
			vName = CSTR("EFI byte code");
			break;
		case 0x14c:
			vName = CSTR("Intel 386 or later processors and compatible processors");
			break;
		case 0x200:
			vName = CSTR("Intel Itanium processor family");
			break;
		case 0x9041:
			vName = CSTR("Mitsubishi M32R little endian");
			break;
		case 0x266:
			vName = CSTR("MIPS16");
			break;
		case 0x366:
			vName = CSTR("MIPS with FPU");
			break;
		case 0x466:
			vName = CSTR("MIPS16 with FPU");
			break;
		case 0x1f0:
			vName = CSTR("Power PC little endian");
			break;
		case 0x1f1:
			vName = CSTR("Power PC with floating point support");
			break;
		case 0x166:
			vName = CSTR("MIPS little endian");
			break;
		case 0x5032:
			vName = CSTR("RISC-V 32-bit address space");
			break;
		case 0x5064:
			vName = CSTR("RISC-V 64-bit address space");
			break;
		case 0x5128:
			vName = CSTR("RISC-V 128-bit address space");
			break;
		case 0x1a2:
			vName = CSTR("Hitachi SH3");
			break;
		case 0x1a3:
			vName = CSTR("Hitachi SH3 DSP");
			break;
		case 0x1a6:
			vName = CSTR("Hitachi SH4");
			break;
		case 0x1a8:
			vName = CSTR("Hitachi SH5");
			break;
		case 0x1c2:
			vName = CSTR("Thumb");
			break;
		case 0x169:
			vName = CSTR("MIPS little-endian WCE v2");
			break;
		}
		frame->AddHex16Name(4, CSTR("Machine"), ReadUInt16(&packBuff[4]), vName);
		frame->AddUInt(6, 2, CSTR("NumberOfSections"), ReadUInt16(&packBuff[6]));
		frame->AddUInt(8, 4, CSTR("TimeDateStamp"), ReadUInt32(&packBuff[8]));
		Data::DateTime dt;
		dt.SetUnixTimestamp(ReadUInt32(&packBuff[8]));
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		frame->AddField(8, 4, CSTR("TimeDateStamp"), CSTRP(sbuff, sptr));
		frame->AddHex32(12, CSTR("PointerToSymbolTable"), ReadUInt32(&packBuff[12]));
		frame->AddUInt(16, 4, CSTR("NumberOfSymbols"), ReadUInt32(&packBuff[16]));
		frame->AddUInt(20, 2, CSTR("SizeOfOptionalHeader"), ReadUInt16(&packBuff[20]));
		UInt16 ch = ReadUInt16(&packBuff[22]);
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("0x"));
		sb.AppendHex16(ch);
		if (ch & 0x0001) sb.AppendC(UTF8STRC(" RELOCS_STRIPPED"));
		if (ch & 0x0002) sb.AppendC(UTF8STRC(" EXECUTABLE_IMAGE"));
		if (ch & 0x0004) sb.AppendC(UTF8STRC(" LINE_NUMS_STRIPPED"));
		if (ch & 0x0008) sb.AppendC(UTF8STRC(" LOCAL_SYMS_STRIPPED"));
		if (ch & 0x0010) sb.AppendC(UTF8STRC(" AGGRESSIVE_WS_TRIM"));
		if (ch & 0x0020) sb.AppendC(UTF8STRC(" LARGE_ADDRESS_ AWARE"));
		if (ch & 0x0040) sb.AppendC(UTF8STRC(" RESERVED"));
		if (ch & 0x0080) sb.AppendC(UTF8STRC(" BYTES_REVERSED_LO"));
		if (ch & 0x0100) sb.AppendC(UTF8STRC(" 32BIT_MACHINE"));
		if (ch & 0x0200) sb.AppendC(UTF8STRC(" DEBUG_STRIPPED"));
		if (ch & 0x0400) sb.AppendC(UTF8STRC(" REMOVABLE_RUN_ FROM_SWAP"));
		if (ch & 0x0800) sb.AppendC(UTF8STRC(" NET_RUN_FROM_SWAP"));
		if (ch & 0x1000) sb.AppendC(UTF8STRC(" SYSTEM"));
		if (ch & 0x2000) sb.AppendC(UTF8STRC(" DLL"));
		if (ch & 0x4000) sb.AppendC(UTF8STRC(" UP_SYSTEM_ONLY"));
		if (ch & 0x8000) sb.AppendC(UTF8STRC(" BYTES_REVERSED_HI"));
		frame->AddField(22, 2, CSTR("Characteristics"), sb.ToCString());
	}
	else if (pack->packType == 3 || pack->packType == 4)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		frame->AddHex16(0, CSTR("Magic number"), ReadUInt16(&packBuff[0]));
		frame->AddUInt(2, 1, CSTR("LinkerVersionMajor"), packBuff[2]);
		frame->AddUInt(3, 1, CSTR("LinkerVersionMinor"), packBuff[3]);
		frame->AddUInt(4, 4, CSTR("SizeOfCode"), ReadUInt32(&packBuff[4]));
		frame->AddUInt(8, 4, CSTR("SizeOfInitializedData"), ReadUInt32(&packBuff[8]));
		frame->AddUInt(12, 4, CSTR("SizeOfUninitializedData"), ReadUInt32(&packBuff[12]));
		frame->AddHex32(16, CSTR("AddressOfEntryPoint"), ReadUInt32(&packBuff[16]));
		frame->AddHex32(20, CSTR("BaseOfCode"), ReadUInt32(&packBuff[20]));
		if (pack->packType == 3)
		{
			frame->AddHex32(24, CSTR("BaseOfData"), ReadUInt32(&packBuff[24]));
			frame->AddHex32(28, CSTR("ImageBase"), ReadUInt32(&packBuff[28]));
		}
		else
		{
			frame->AddHex64(24, CSTR("ImageBase"), ReadUInt64(&packBuff[24]));
		}
		frame->AddUInt(32, 4, CSTR("SectionAlignment"), ReadUInt32(&packBuff[32]));
		frame->AddUInt(36, 4, CSTR("FileAlignment"), ReadUInt32(&packBuff[36]));
		frame->AddUInt(40, 2, CSTR("OperatingSystemVersionMajor"), ReadUInt16(&packBuff[40]));
		frame->AddUInt(42, 2, CSTR("OperatingSystemVersionMinor"), ReadUInt16(&packBuff[42]));
		frame->AddUInt(44, 2, CSTR("ImageVersionMajor"), ReadUInt16(&packBuff[44]));
		frame->AddUInt(46, 2, CSTR("ImageVersionMinor"), ReadUInt16(&packBuff[46]));
		frame->AddUInt(48, 2, CSTR("SubsystemVersionMajor"), ReadUInt16(&packBuff[48]));
		frame->AddUInt(50, 2, CSTR("SubsystemVersionMinor"), ReadUInt16(&packBuff[50]));
		frame->AddUInt(52, 4, CSTR("Win32VersionValue"), ReadUInt32(&packBuff[52]));
		frame->AddUInt(56, 4, CSTR("SizeOfImage"), ReadUInt32(&packBuff[56]));
		frame->AddUInt(60, 4, CSTR("SizeOfHeaders"), ReadUInt32(&packBuff[60]));
		frame->AddHex32(64, CSTR("CheckSum"), ReadUInt32(&packBuff[64]));
		vName = CSTR_NULL;
		switch (ReadUInt16(&packBuff[68]))
		{
		case 0:
			vName = CSTR("Unknown");
			break;
		case 1:
			vName = CSTR("Native");
			break;
		case 2:
			vName = CSTR("Windows GUI");
			break;
		case 3:
			vName = CSTR("Windows CUI");
			break;
		case 5:
			vName = CSTR("OS/2 CUI");
			break;
		case 7:
			vName = CSTR("Posix CUI");
			break;
		case 8:
			vName = CSTR("Native Win9x driver");
			break;
		case 9:
			vName = CSTR("Windows CE");
			break;
		case 10:
			vName = CSTR("EFI Application");
			break;
		case 11:
			vName = CSTR("EFI Boot Service Driver");
			break;
		case 12:
			vName = CSTR("EFI Runtime Driver");
			break;
		case 13:
			vName = CSTR("EFI ROM");
			break;
		case 14:
			vName = CSTR("XBOX");
			break;
		case 16:
			vName = CSTR("Windows Boot Application");
			break;
		}
		frame->AddUIntName(68, 2, CSTR("Subsystem"), ReadUInt16(&packBuff[68]), vName);
		UInt16 ch = ReadUInt16(&packBuff[70]);
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("0x"));
		sb.AppendHex16(ch);
		if (ch & 0x0020) sb.AppendC(UTF8STRC(" HIGH_ENTROPY_VA"));
		if (ch & 0x0040) sb.AppendC(UTF8STRC(" DYNAMIC_BASE"));
		if (ch & 0x0080) sb.AppendC(UTF8STRC(" FORCE_INTEGRITY"));
		if (ch & 0x0100) sb.AppendC(UTF8STRC(" NX_COMPAT"));
		if (ch & 0x0200) sb.AppendC(UTF8STRC(" NO_ISOLATION"));
		if (ch & 0x0400) sb.AppendC(UTF8STRC(" NO_SEH"));
		if (ch & 0x0800) sb.AppendC(UTF8STRC(" NO_BIND"));
		if (ch & 0x1000) sb.AppendC(UTF8STRC(" APPCONTAINER"));
		if (ch & 0x2000) sb.AppendC(UTF8STRC(" WDM_DRIVER"));
		if (ch & 0x4000) sb.AppendC(UTF8STRC(" GUARD_CF"));
		if (ch & 0x8000) sb.AppendC(UTF8STRC(" TERMINAL_SERVER_AWARE"));
		frame->AddField(70, 2, CSTR("DLL Characteristics"), sb.ToCString());

		UOSInt ofst;
		if (pack->packType == 3)
		{
			frame->AddUInt(72, 4, CSTR("SizeOfStackReserve"), ReadUInt32(&packBuff[72]));
			frame->AddUInt(76, 4, CSTR("SizeOfStackCommit"), ReadUInt32(&packBuff[76]));
			frame->AddUInt(80, 4, CSTR("SizeOfHeapReserve"), ReadUInt32(&packBuff[80]));
			frame->AddUInt(84, 4, CSTR("SizeOfHeapCommit"), ReadUInt32(&packBuff[84]));
			ofst = 88;
		}
		else
		{
			frame->AddUInt64(72, CSTR("SizeOfStackReserve"), ReadUInt64(&packBuff[72]));
			frame->AddUInt64(80, CSTR("SizeOfStackCommit"), ReadUInt64(&packBuff[80]));
			frame->AddUInt64(88, CSTR("SizeOfHeapReserve"), ReadUInt64(&packBuff[88]));
			frame->AddUInt64(96, CSTR("SizeOfHeapCommit"), ReadUInt64(&packBuff[96]));
			ofst = 104;
		}
		frame->AddUInt(ofst, 4, CSTR("LoaderFlags"), ReadUInt32(&packBuff[ofst]));
		frame->AddUInt(ofst + 4, 4, CSTR("NumberOfRvaAndSizes"), ReadUInt32(&packBuff[ofst + 4]));
		frame->AddHex32(ofst + 8, CSTR("ExportTableVAddr"), ReadUInt32(&packBuff[ofst + 8]));
		frame->AddUInt(ofst + 12, 4, CSTR("ExportTableSize"), ReadUInt32(&packBuff[ofst + 12]));
		frame->AddHex32(ofst + 16, CSTR("ImportTableVAddr"), ReadUInt32(&packBuff[ofst + 16]));
		frame->AddUInt(ofst + 20, 4, CSTR("ImportTableSize"), ReadUInt32(&packBuff[ofst + 20]));
		frame->AddHex32(ofst + 24, CSTR("ResourceTableVAddr"), ReadUInt32(&packBuff[ofst + 24]));
		frame->AddUInt(ofst + 28, 4, CSTR("ResourceTableSize"), ReadUInt32(&packBuff[ofst + 28]));
		frame->AddHex32(ofst + 32, CSTR("ExceptionTableVAddr"), ReadUInt32(&packBuff[ofst + 32]));
		frame->AddUInt(ofst + 36, 4, CSTR("ExceptionTableSize"), ReadUInt32(&packBuff[ofst + 36]));
		frame->AddHex32(ofst + 40, CSTR("CertificateTableVAddr"), ReadUInt32(&packBuff[ofst + 40]));
		frame->AddUInt(ofst + 44, 4, CSTR("CertificateTableSize"), ReadUInt32(&packBuff[ofst + 44]));
		frame->AddHex32(ofst + 48, CSTR("BaseRelocationTableVAddr"), ReadUInt32(&packBuff[ofst + 48]));
		frame->AddUInt(ofst + 52, 4, CSTR("BaseRelocationTableSize"), ReadUInt32(&packBuff[ofst + 52]));
		frame->AddHex32(ofst + 56, CSTR("DebugTableVAddr"), ReadUInt32(&packBuff[ofst + 56]));
		frame->AddUInt(ofst + 60, 4, CSTR("DebugTableSize"), ReadUInt32(&packBuff[ofst + 60]));
		frame->AddHex32(ofst + 64, CSTR("ArchitectureTableVAddr"), ReadUInt32(&packBuff[ofst + 64]));
		frame->AddUInt(ofst + 68, 4, CSTR("ArchitectureTableSize"), ReadUInt32(&packBuff[ofst + 68]));
		frame->AddHex32(ofst + 72, CSTR("GlobalPtrTableVAddr"), ReadUInt32(&packBuff[ofst + 72]));
		frame->AddUInt(ofst + 76, 4, CSTR("GlobalPtrTableSize"), ReadUInt32(&packBuff[ofst + 76]));
		frame->AddHex32(ofst + 80, CSTR("TLSTableVAddr"), ReadUInt32(&packBuff[ofst + 80]));
		frame->AddUInt(ofst + 84, 4, CSTR("TLSTableSize"), ReadUInt32(&packBuff[ofst + 84]));
		frame->AddHex32(ofst + 88, CSTR("LoadConfigTableVAddr"), ReadUInt32(&packBuff[ofst + 88]));
		frame->AddUInt(ofst + 92, 4, CSTR("LoadConfigTableSize"), ReadUInt32(&packBuff[ofst + 92]));
		frame->AddHex32(ofst + 96, CSTR("BoundImportTableVAddr"), ReadUInt32(&packBuff[ofst + 96]));
		frame->AddUInt(ofst + 100, 4, CSTR("BoundImportTableSize"), ReadUInt32(&packBuff[ofst + 100]));
		frame->AddHex32(ofst + 104, CSTR("ImportAddrTableVAddr"), ReadUInt32(&packBuff[ofst + 104]));
		frame->AddUInt(ofst + 108, 4, CSTR("ImportAddrTableSize"), ReadUInt32(&packBuff[ofst + 108]));
		frame->AddHex32(ofst + 112, CSTR("DelayImportTableVAddr"), ReadUInt32(&packBuff[ofst + 112]));
		frame->AddUInt(ofst + 116, 4, CSTR("DelayImportTableSize"), ReadUInt32(&packBuff[ofst + 116]));
		frame->AddHex32(ofst + 2120, CSTR("CLRRuntimeTableVAddr"), ReadUInt32(&packBuff[ofst + 120]));
		frame->AddUInt(ofst + 124, 4, CSTR("CLRRuntimeTableSize"), ReadUInt32(&packBuff[ofst + 124]));
	}
	else if (pack->packType == 5)
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);

		frame->AddStrS(0, 8, CSTR("Name"), &packBuff[0]);
		frame->AddUInt(8, 4, CSTR("VirtualSize"), ReadUInt32(&packBuff[8]));
		frame->AddHex32(12, CSTR("VirtualAddress"), ReadUInt32(&packBuff[12]));
		frame->AddUInt(16, 4, CSTR("SizeOfRawData"), ReadUInt32(&packBuff[16]));
		frame->AddHex32(20, CSTR("PointerToRawData"), ReadUInt32(&packBuff[20]));
		frame->AddHex32(24, CSTR("PointerToRelocations"), ReadUInt32(&packBuff[24]));
		frame->AddHex32(28, CSTR("PointerToLinenumbers"), ReadUInt32(&packBuff[28]));
		frame->AddUInt(32, 2, CSTR("NumberOfRelocations"), ReadUInt32(&packBuff[32]));
		frame->AddUInt(34, 2, CSTR("NumberOfLinenumbers"), ReadUInt32(&packBuff[34]));
		frame->AddHex32(36, CSTR("Characteristics"), ReadUInt32(&packBuff[36]));
	}
	else if (pack->packType == 6)
	{
		UInt32 nAddr;
		UInt32 nName;
		frame->AddHex32(0, CSTR("Export Flags"), ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst]));
		frame->AddUInt(4, 4, CSTR("Timestamp"), ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 4]));
		Data::DateTime dt;
		dt.SetUnixTimestamp(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 4]));
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		frame->AddField(4, 4, CSTR("Timestamp"), CSTRP(sbuff, sptr));
		frame->AddUInt(8, 2, CSTR("VersionMajor"), ReadUInt16(&this->imageBuff[(UOSInt)pack->fileOfst + 8]));
		frame->AddUInt(10, 2, CSTR("VersionMinor"), ReadUInt16(&this->imageBuff[(UOSInt)pack->fileOfst + 10]));
		frame->AddStrS(12, 4, CSTR("Name"), &this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 12])]);
		frame->AddUInt(16, 4, CSTR("Ordinal Base"), ReadUInt16(&this->imageBuff[(UOSInt)pack->fileOfst + 16]));
		nAddr = ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 20]);
		nName = ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 24]);
		frame->AddUInt(20, 4, CSTR("Address Table Entries"), nAddr);
		frame->AddUInt(24, 4, CSTR("Number of Name Pointers"), nName);
		frame->AddHex32(28, CSTR("Export Flags"), ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 28]));
		frame->AddHex32(32, CSTR("Name Pointer RVA"), ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 32]));
		frame->AddHex32(36, CSTR("Ordinal Table RVA"), ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 36]));
		UInt8 *addrTablePtr = &this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 28])];
		UInt8 *nameTablePtr = &this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 32])];
		UInt8 *ordinalTablePtr = &this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 36])];
		UInt32 i = 0;
		while (i < nName)
		{
			frame->AddHex32((UOSInt)(addrTablePtr - &this->imageBuff[(UOSInt)pack->fileOfst]), CSTR("Addr"), ReadUInt32(&addrTablePtr[ReadUInt16(ordinalTablePtr) * 4]));
			frame->AddField((UOSInt)(nameTablePtr - &this->imageBuff[(UOSInt)pack->fileOfst]), 4, CSTR("Name"), Text::CString::FromPtr(&this->imageBuff[ReadUInt32(nameTablePtr)]));
			nameTablePtr += 4;
			ordinalTablePtr += 2;
			i++;
		}
	}
	else if (pack->packType == 7)
	{
		frame->AddHex32(0, CSTR("Import Lookup Table RVA"), ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 0]));
		frame->AddUInt(4, 4, CSTR("Timestamp"), ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 4]));
		Data::DateTime dt;
		dt.SetUnixTimestamp(ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 4]));
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		frame->AddField(4, 4, CSTR("Timestamp"), CSTRP(sbuff, sptr));
		frame->AddUInt(8, 4, CSTR("Forwarder Chain"), ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 8]));
		frame->AddHex32(12, CSTR("Name RVA"), ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 12]));
		frame->AddHex32(16, CSTR("Import Address Table RVA"), ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 16]));
//		UInt8 *lutPtr = &this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 0])];
//		UInt8 *nameTablePtr = &this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 12])];
//		UInt8 *iatPtr = &this->imageBuff[ReadUInt32(&this->imageBuff[(UOSInt)pack->fileOfst + 16])];
	}
	return frame;
}

Bool IO::FileAnalyse::EXEFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::EXEFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::EXEFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}

Text::CStringNN IO::FileAnalyse::EXEFileAnalyse::PackTypeGetName(Int32 packType)
{
	switch (packType)
	{
	case 0:
		return CSTR("MS-DOS Header");
	case 1:
		return CSTR("MS-DOS Byte Code");
	case 2:
		return CSTR("COFF File Header");
	case 3:
		return CSTR("PE32 Optional Header");
	case 4:
		return CSTR("PE32+ Optional Header");
	case 5:
		return CSTR("Section Entry");
	case 6:
		return CSTR("Export Table");
	case 7:
		return CSTR("Import Table");
	case 8:
		return CSTR("Resource Table");
	case 9:
		return CSTR("Exception Table");
	case 10:
		return CSTR("Certificate Table");
	case 11:
		return CSTR("Base Relocation Table");
	case 12:
		return CSTR("Debug");
	case 13:
		return CSTR("Architecture");
	case 14:
		return CSTR("Global Ptr");
	case 15:
		return CSTR("TLS Table");
	case 16:
		return CSTR("Load Config Table");
	case 17:
		return CSTR("Bound Import");
	case 18:
		return CSTR("IAT");
	case 19:
		return CSTR("Delay Import Descriptor");
	case 20:
		return CSTR("CLR Runtime Header");
	default:
		return CSTR("Unknown");
	}
}
