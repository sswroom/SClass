#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/EXEFileAnalyse.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

UInt32 __stdcall IO::FileAnalyse::EXEFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::EXEFileAnalyse *me = (IO::FileAnalyse::EXEFileAnalyse*)userObj;
	UInt8 buff[256];
	IO::FileAnalyse::EXEFileAnalyse::PackInfo *pack;
	UInt32 val;
	me->threadRunning = true;
	me->threadStarted = true;
	pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
	pack->fileOfst = 0;
	pack->packSize = 64;
	pack->packType = 0;
	me->packs->Add(pack);
	me->fd->GetRealData(0, 64, buff);
	val = ReadUInt32(&buff[60]);
	if (val > 64)
	{
		me->fd->GetRealData(val, 128, buff);
		if (buff[0] == 'P' && buff[1] == 'E' && buff[2] == 0 && buff[3] == 0)
		{
			pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
			pack->fileOfst = 64;
			pack->packSize = val - 64;
			pack->packType = 1;
			me->packs->Add(pack);

			pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
			pack->fileOfst = val;
			pack->packSize = 24;
			pack->packType = 2;
			me->packs->Add(pack);

			UInt16 optHdrSize = ReadUInt16(&buff[20]);
			UOSInt nSection = ReadUInt16(&buff[6]);
			UOSInt tableOfst = 0;
			if (optHdrSize > 0)
			{
				me->imageSize = ReadUInt32(&buff[80]);
				if (ReadUInt16(&buff[24]) == 0x10b)
				{
					tableOfst = val + 24 + 96;
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = val + 24;
					pack->packSize = optHdrSize;
					pack->packType = 3;
					me->packs->Add(pack);
				}
				else if (ReadUInt16(&buff[24]) == 0x20b)
				{
					tableOfst = val + 24 + 112;
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = val + 24;
					pack->packSize = optHdrSize;
					pack->packType = 4;
					me->packs->Add(pack);
				}
				if (me->imageSize > 0)
				{
					me->imageBuff = MemAlloc(UInt8, me->imageSize);
				}
			}
			UOSInt ofst = val + 24 + optHdrSize;
			UOSInt i = 0;
			UInt32 virtualSize;
			UInt32 sizeOfRawData;
			UInt32 virtualAddr;
			while (i < nSection)
			{
				pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
				pack->fileOfst = ofst;
				pack->packSize = 40;
				pack->packType = 5;
				me->packs->Add(pack);

				me->fd->GetRealData(ofst, 40, buff);
				virtualSize = ReadUInt32(&buff[8]);
				sizeOfRawData = ReadUInt32(&buff[16]);
				if (me->imageBuff)
				{
					virtualAddr = ReadUInt32(&buff[12]);
					if (virtualSize > sizeOfRawData)
					{
						me->fd->GetRealData(ReadUInt32(&buff[20]), sizeOfRawData, &me->imageBuff[virtualAddr]);
						MemClear(&me->imageBuff[virtualAddr + sizeOfRawData], virtualSize - sizeOfRawData);
					}
					else
					{
						me->fd->GetRealData(ReadUInt32(&buff[20]), virtualSize, &me->imageBuff[virtualAddr]);
					}
				}

				ofst += 40;
				i++;
			}

			if (optHdrSize > 0 && tableOfst != 0)
			{
				me->fd->GetRealData(tableOfst, 128, buff);
				virtualAddr = ReadUInt32(&buff[0]);
				virtualSize = ReadUInt32(&buff[4]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 6;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[8]);
				virtualSize = ReadUInt32(&buff[12]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 7;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[16]);
				virtualSize = ReadUInt32(&buff[20]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 8;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[24]);
				virtualSize = ReadUInt32(&buff[28]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 9;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[32]);
				virtualSize = ReadUInt32(&buff[36]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 10;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[40]);
				virtualSize = ReadUInt32(&buff[44]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 11;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[48]);
				virtualSize = ReadUInt32(&buff[52]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 12;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[56]);
				virtualSize = ReadUInt32(&buff[60]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 13;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[64]);
				virtualSize = ReadUInt32(&buff[68]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 14;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[72]);
				virtualSize = ReadUInt32(&buff[76]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 15;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[80]);
				virtualSize = ReadUInt32(&buff[84]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 16;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[88]);
				virtualSize = ReadUInt32(&buff[92]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 17;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[96]);
				virtualSize = ReadUInt32(&buff[100]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 18;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[104]);
				virtualSize = ReadUInt32(&buff[108]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 19;
					me->packs->Add(pack);
				}
				virtualAddr = ReadUInt32(&buff[112]);
				virtualSize = ReadUInt32(&buff[116]);
				if (virtualAddr != 0 && virtualSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::EXEFileAnalyse::PackInfo, 1);
					pack->fileOfst = virtualAddr;
					pack->packSize = virtualSize;
					pack->packType = 20;
					me->packs->Add(pack);
				}
			}
		}
	}
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::EXEFileAnalyse::EXEFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[8];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	this->imageBuff = 0;
	this->imageSize = 0;
	NEW_CLASS(this->packs, Data::SyncArrayList<IO::FileAnalyse::EXEFileAnalyse::PackInfo*>());
	fd->GetRealData(0, 8, buff);
	if (ReadInt16(buff) != 0x5A4D)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	Sync::Thread::Create(ParseThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
}

IO::FileAnalyse::EXEFileAnalyse::~EXEFileAnalyse()
{
	if (this->threadRunning)
	{
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}
	SDEL_CLASS(this->fd);
	DEL_LIST_FUNC(this->packs, MemFree);
	DEL_CLASS(this->packs);
	if (this->imageBuff)
	{
		MemFree(this->imageBuff);
		this->imageBuff = 0;
	}
}

UOSInt IO::FileAnalyse::EXEFileAnalyse::GetFrameCount()
{
	return this->packs->GetCount();
}

Bool IO::FileAnalyse::EXEFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::EXEFileAnalyse::PackInfo *pack;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->Append((const UTF8Char*)": Type=");
	sb->Append(PackTypeGetName(pack->packType));
	sb->Append((const UTF8Char*)", size=");
	sb->AppendU64(pack->packSize);
	return true;
}

Bool IO::FileAnalyse::EXEFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::EXEFileAnalyse::PackInfo *pack;
	UInt8 *packBuff;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;

	sb->AppendU64(pack->fileOfst);
	sb->Append((const UTF8Char*)": Type=");
	sb->Append(PackTypeGetName(pack->packType));
	sb->Append((const UTF8Char*)", size=");
	sb->AppendU64(pack->packSize);
	sb->Append((const UTF8Char*)"\r\n");

	if (pack->packType == 0)
	{
		packBuff = MemAlloc(UInt8, (UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);

		sb->Append((const UTF8Char*)"Magic number = 0x");
		sb->AppendHex16(ReadUInt16(packBuff));
		sb->Append((const UTF8Char*)"\r\nBytes on last page of file = ");
		sb->AppendU16(ReadUInt16(&packBuff[2]));
		sb->Append((const UTF8Char*)"\r\nPages in file = ");
		sb->AppendU16(ReadUInt16(&packBuff[4]));
		sb->Append((const UTF8Char*)"\r\nRelocations = ");
		sb->AppendU16(ReadUInt16(&packBuff[6]));
		sb->Append((const UTF8Char*)"\r\nSize of header in paragraphs = ");
		sb->AppendU16(ReadUInt16(&packBuff[8]));
		sb->Append((const UTF8Char*)"\r\nMinimum extra paragraphs needed = ");
		sb->AppendU16(ReadUInt16(&packBuff[10]));
		sb->Append((const UTF8Char*)"\r\nMaximum extra paragraphs needed = ");
		sb->AppendU16(ReadUInt16(&packBuff[12]));
		sb->Append((const UTF8Char*)"\r\nInitial (relative) SS value = 0x");
		sb->AppendHex16(ReadUInt16(&packBuff[14]));
		sb->Append((const UTF8Char*)"\r\nInitial SP value = 0x");
		sb->AppendHex16(ReadUInt16(&packBuff[16]));
		sb->Append((const UTF8Char*)"\r\nChecksum = 0x");
		sb->AppendHex16(ReadUInt16(&packBuff[18]));
		sb->Append((const UTF8Char*)"\r\nInitial IP value = 0x");
		sb->AppendHex16(ReadUInt16(&packBuff[20]));
		sb->Append((const UTF8Char*)"\r\nInitial (relative) CS value = 0x");
		sb->AppendHex16(ReadUInt16(&packBuff[22]));
		sb->Append((const UTF8Char*)"\r\nFile address of relocation table = 0x");
		sb->AppendHex16(ReadUInt16(&packBuff[24]));
		sb->Append((const UTF8Char*)"\r\nOverlay number = ");
		sb->AppendU16(ReadUInt16(&packBuff[26]));
		sb->Append((const UTF8Char*)"\r\nOEM identifier = ");
		sb->AppendU16(ReadUInt16(&packBuff[36]));
		sb->Append((const UTF8Char*)"\r\nOEM information = ");
		sb->AppendU16(ReadUInt16(&packBuff[38]));
		sb->Append((const UTF8Char*)"\r\nFile address of new exe header = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[60]));

		MemFree(packBuff);
	}
	else if (pack->packType == 1)
	{
		packBuff = MemAlloc(UInt8, (UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		sb->AppendHexBuff(packBuff, (UOSInt)pack->packSize, ' ', Text::LBT_CRLF);

		MemFree(packBuff);
	}
	else if (pack->packType == 2)
	{
		packBuff = MemAlloc(UInt8, (UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);

		sb->Append((const UTF8Char*)"Magic number = PE\\0\\0");
		sb->Append((const UTF8Char*)"\r\nMachine = 0x");
		sb->AppendHex16(ReadUInt16(&packBuff[4]));
		switch (ReadUInt16(&packBuff[4]))
		{
		case 0x0:
			sb->Append((const UTF8Char*)" (Unknown)");
			break;
		case 0x1d3:
			sb->Append((const UTF8Char*)" (Matsushita AM33)");
			break;
		case 0x8664:
			sb->Append((const UTF8Char*)" (AMD64)");
			break;
		case 0x1c0:
			sb->Append((const UTF8Char*)" (ARM little endian)");
			break;
		case 0xaa64:
			sb->Append((const UTF8Char*)" (ARM64 little endian)");
			break;
		case 0x1c4:
			sb->Append((const UTF8Char*)" (ARM Thumb-2 little endian)");
			break;
		case 0xebc:
			sb->Append((const UTF8Char*)" (EFI byte code)");
			break;
		case 0x14c:
			sb->Append((const UTF8Char*)" (Intel 386 or later processors and compatible processors)");
			break;
		case 0x200:
			sb->Append((const UTF8Char*)" (Intel Itanium processor family)");
			break;
		case 0x9041:
			sb->Append((const UTF8Char*)" (Mitsubishi M32R little endian)");
			break;
		case 0x266:
			sb->Append((const UTF8Char*)" (MIPS16)");
			break;
		case 0x366:
			sb->Append((const UTF8Char*)" (MIPS with FPU)");
			break;
		case 0x466:
			sb->Append((const UTF8Char*)" (MIPS16 with FPU)");
			break;
		case 0x1f0:
			sb->Append((const UTF8Char*)" (Power PC little endian)");
			break;
		case 0x1f1:
			sb->Append((const UTF8Char*)" (Power PC with floating point support)");
			break;
		case 0x166:
			sb->Append((const UTF8Char*)" (MIPS little endian)");
			break;
		case 0x5032:
			sb->Append((const UTF8Char*)" (RISC-V 32-bit address space)");
			break;
		case 0x5064:
			sb->Append((const UTF8Char*)" (RISC-V 64-bit address space)");
			break;
		case 0x5128:
			sb->Append((const UTF8Char*)" (RISC-V 128-bit address space)");
			break;
		case 0x1a2:
			sb->Append((const UTF8Char*)" (Hitachi SH3)");
			break;
		case 0x1a3:
			sb->Append((const UTF8Char*)" (Hitachi SH3 DSP)");
			break;
		case 0x1a6:
			sb->Append((const UTF8Char*)" (Hitachi SH4)");
			break;
		case 0x1a8:
			sb->Append((const UTF8Char*)" (Hitachi SH5)");
			break;
		case 0x1c2:
			sb->Append((const UTF8Char*)" (Thumb)");
			break;
		case 0x169:
			sb->Append((const UTF8Char*)" (MIPS little-endian WCE v2)");
			break;
		}
		sb->Append((const UTF8Char*)"\r\nNumberOfSections = ");
		sb->AppendU16(ReadUInt16(&packBuff[6]));
		sb->Append((const UTF8Char*)"\r\nTimeDateStamp = ");
		sb->AppendU32(ReadUInt32(&packBuff[8]));
		Data::DateTime dt;
		dt.SetUnixTimestamp(ReadUInt32(&packBuff[8]));
		sb->Append((const UTF8Char*)" (");
		sb->AppendDate(&dt);
		sb->Append((const UTF8Char*)")");
		sb->Append((const UTF8Char*)"\r\nPointerToSymbolTable = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[12]));
		sb->Append((const UTF8Char*)"\r\nNumberOfSymbols = ");
		sb->AppendU32(ReadUInt32(&packBuff[16]));
		sb->Append((const UTF8Char*)"\r\nSizeOfOptionalHeader = ");
		sb->AppendU16(ReadUInt16(&packBuff[20]));
		sb->Append((const UTF8Char*)"\r\nCharacteristics = 0x");
		UInt16 ch = ReadUInt16(&packBuff[22]);
		sb->AppendHex16(ch);
		if (ch & 0x0001) sb->Append((const UTF8Char*)" RELOCS_STRIPPED");
		if (ch & 0x0002) sb->Append((const UTF8Char*)" EXECUTABLE_IMAGE");
		if (ch & 0x0004) sb->Append((const UTF8Char*)" LINE_NUMS_STRIPPED");
		if (ch & 0x0008) sb->Append((const UTF8Char*)" LOCAL_SYMS_STRIPPED");
		if (ch & 0x0010) sb->Append((const UTF8Char*)" AGGRESSIVE_WS_TRIM");
		if (ch & 0x0020) sb->Append((const UTF8Char*)" LARGE_ADDRESS_ AWARE");
		if (ch & 0x0040) sb->Append((const UTF8Char*)" RESERVED");
		if (ch & 0x0080) sb->Append((const UTF8Char*)" BYTES_REVERSED_LO");
		if (ch & 0x0100) sb->Append((const UTF8Char*)" 32BIT_MACHINE");
		if (ch & 0x0200) sb->Append((const UTF8Char*)" DEBUG_STRIPPED");
		if (ch & 0x0400) sb->Append((const UTF8Char*)" REMOVABLE_RUN_ FROM_SWAP");
		if (ch & 0x0800) sb->Append((const UTF8Char*)" NET_RUN_FROM_SWAP");
		if (ch & 0x1000) sb->Append((const UTF8Char*)" SYSTEM");
		if (ch & 0x2000) sb->Append((const UTF8Char*)" DLL");
		if (ch & 0x4000) sb->Append((const UTF8Char*)" UP_SYSTEM_ONLY");
		if (ch & 0x8000) sb->Append((const UTF8Char*)" BYTES_REVERSED_HI");

		MemFree(packBuff);
	}
	else if (pack->packType == 3 || pack->packType == 4)
	{
		packBuff = MemAlloc(UInt8, (UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);

		sb->Append((const UTF8Char*)"Magic number = 0x");
		sb->AppendHex16(ReadUInt16(&packBuff[0]));
		sb->Append((const UTF8Char*)"\r\nLinkerVersion = ");
		sb->AppendU16(packBuff[2]);
		sb->Append((const UTF8Char*)".");
		sb->AppendU16(packBuff[3]);
		sb->Append((const UTF8Char*)"\r\nSizeOfCode = ");
		sb->AppendU32(ReadUInt32(&packBuff[4]));
		sb->Append((const UTF8Char*)"\r\nSizeOfInitializedData = ");
		sb->AppendU32(ReadUInt32(&packBuff[8]));
		sb->Append((const UTF8Char*)"\r\nSizeOfUninitializedData = ");
		sb->AppendU32(ReadUInt32(&packBuff[12]));
		sb->Append((const UTF8Char*)"\r\nAddressOfEntryPoint = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[16]));
		sb->Append((const UTF8Char*)"\r\nBaseOfCode = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[20]));
		if (pack->packType == 3)
		{
			sb->Append((const UTF8Char*)"\r\nBaseOfData = 0x");
			sb->AppendHex32(ReadUInt32(&packBuff[24]));
			sb->Append((const UTF8Char*)"\r\nImageBase = 0x");
			sb->AppendHex32(ReadUInt32(&packBuff[28]));
		}
		else
		{
			sb->Append((const UTF8Char*)"\r\nImageBase = 0x");
			sb->AppendHex64(ReadUInt64(&packBuff[24]));
		}
		sb->Append((const UTF8Char*)"\r\nSectionAlignment = ");
		sb->AppendU32(ReadUInt32(&packBuff[32]));
		sb->Append((const UTF8Char*)"\r\nFileAlignment = ");
		sb->AppendU32(ReadUInt32(&packBuff[36]));
		sb->Append((const UTF8Char*)"\r\nOperatingSystemVersion = ");
		sb->AppendU16(ReadUInt16(&packBuff[40]));
		sb->Append((const UTF8Char*)".");
		sb->AppendU16(ReadUInt16(&packBuff[42]));
		sb->Append((const UTF8Char*)"\r\nImageVersion = ");
		sb->AppendU16(ReadUInt16(&packBuff[44]));
		sb->Append((const UTF8Char*)".");
		sb->AppendU16(ReadUInt16(&packBuff[46]));
		sb->Append((const UTF8Char*)"\r\nSubsystemVersion = ");
		sb->AppendU16(ReadUInt16(&packBuff[48]));
		sb->Append((const UTF8Char*)".");
		sb->AppendU16(ReadUInt16(&packBuff[50]));
		sb->Append((const UTF8Char*)"\r\nWin32VersionValue = ");
		sb->AppendU32(ReadUInt32(&packBuff[52]));
		sb->Append((const UTF8Char*)"\r\nSizeOfImage = ");
		sb->AppendU32(ReadUInt32(&packBuff[56]));
		sb->Append((const UTF8Char*)"\r\nSizeOfHeaders = ");
		sb->AppendU32(ReadUInt32(&packBuff[60]));
		sb->Append((const UTF8Char*)"\r\nCheckSum = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[64]));
		sb->Append((const UTF8Char*)"\r\nSubsystem = ");
		sb->AppendU16(ReadUInt16(&packBuff[68]));
		switch (ReadUInt16(&packBuff[68]))
		{
		case 0:
			sb->Append((const UTF8Char*)" (Unknown)");
			break;
		case 1:
			sb->Append((const UTF8Char*)" (Native)");
			break;
		case 2:
			sb->Append((const UTF8Char*)" (Windows GUI)");
			break;
		case 3:
			sb->Append((const UTF8Char*)" (Windows CUI)");
			break;
		case 5:
			sb->Append((const UTF8Char*)" (OS/2 CUI)");
			break;
		case 7:
			sb->Append((const UTF8Char*)" (Posix CUI)");
			break;
		case 8:
			sb->Append((const UTF8Char*)" (Native Win9x driver)");
			break;
		case 9:
			sb->Append((const UTF8Char*)" (Windows CE)");
			break;
		case 10:
			sb->Append((const UTF8Char*)" (EFI Application)");
			break;
		case 11:
			sb->Append((const UTF8Char*)" (EFI Boot Service Driver)");
			break;
		case 12:
			sb->Append((const UTF8Char*)" (EFI Runtime Driver)");
			break;
		case 13:
			sb->Append((const UTF8Char*)" (EFI ROM)");
			break;
		case 14:
			sb->Append((const UTF8Char*)" (XBOX)");
			break;
		case 16:
			sb->Append((const UTF8Char*)" (Windows Boot Application)");
			break;
		}
		UInt16 ch = ReadUInt16(&packBuff[70]);
		sb->Append((const UTF8Char*)"\r\nDLL Characteristics = 0x");
		sb->AppendHex16(ch);
		if (ch & 0x0020) sb->Append((const UTF8Char*)" HIGH_ENTROPY_VA");
		if (ch & 0x0040) sb->Append((const UTF8Char*)" DYNAMIC_BASE");
		if (ch & 0x0080) sb->Append((const UTF8Char*)" FORCE_INTEGRITY");
		if (ch & 0x0100) sb->Append((const UTF8Char*)" NX_COMPAT");
		if (ch & 0x0200) sb->Append((const UTF8Char*)" NO_ISOLATION");
		if (ch & 0x0400) sb->Append((const UTF8Char*)" NO_SEH");
		if (ch & 0x0800) sb->Append((const UTF8Char*)" NO_BIND");
		if (ch & 0x1000) sb->Append((const UTF8Char*)" APPCONTAINER");
		if (ch & 0x2000) sb->Append((const UTF8Char*)" WDM_DRIVER");
		if (ch & 0x4000) sb->Append((const UTF8Char*)" GUARD_CF");
		if (ch & 0x8000) sb->Append((const UTF8Char*)" TERMINAL_SERVER_AWARE");

		OSInt ofst;
		if (pack->packType == 3)
		{
			sb->Append((const UTF8Char*)"\r\nSizeOfStackReserve = ");
			sb->AppendU32(ReadUInt32(&packBuff[72]));
			sb->Append((const UTF8Char*)"\r\nSizeOfStackCommit = ");
			sb->AppendU32(ReadUInt32(&packBuff[76]));
			sb->Append((const UTF8Char*)"\r\nSizeOfHeapReserve = ");
			sb->AppendU32(ReadUInt32(&packBuff[80]));
			sb->Append((const UTF8Char*)"\r\nSizeOfHeapCommit = ");
			sb->AppendU32(ReadUInt32(&packBuff[84]));
			ofst = 88;
		}
		else
		{
			sb->Append((const UTF8Char*)"\r\nSizeOfStackReserve = ");
			sb->AppendU64(ReadUInt64(&packBuff[72]));
			sb->Append((const UTF8Char*)"\r\nSizeOfStackCommit = ");
			sb->AppendU64(ReadUInt64(&packBuff[80]));
			sb->Append((const UTF8Char*)"\r\nSizeOfHeapReserve = ");
			sb->AppendU64(ReadUInt64(&packBuff[88]));
			sb->Append((const UTF8Char*)"\r\nSizeOfHeapCommit = ");
			sb->AppendU64(ReadUInt64(&packBuff[96]));
			ofst = 104;
		}
		sb->Append((const UTF8Char*)"\r\nLoaderFlags = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst]));
		sb->Append((const UTF8Char*)"\r\nNumberOfRvaAndSizes = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 4]));
		sb->Append((const UTF8Char*)"\r\nExportTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 8]));
		sb->Append((const UTF8Char*)"\r\nExportTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 12]));
		sb->Append((const UTF8Char*)"\r\nImportTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 16]));
		sb->Append((const UTF8Char*)"\r\nImportTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 20]));
		sb->Append((const UTF8Char*)"\r\nResourceTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 24]));
		sb->Append((const UTF8Char*)"\r\nResourceTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 28]));
		sb->Append((const UTF8Char*)"\r\nExceptionTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 32]));
		sb->Append((const UTF8Char*)"\r\nExceptionTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 36]));
		sb->Append((const UTF8Char*)"\r\nCertificateTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 40]));
		sb->Append((const UTF8Char*)"\r\nCertificateTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 44]));
		sb->Append((const UTF8Char*)"\r\nBaseRelocationTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 48]));
		sb->Append((const UTF8Char*)"\r\nBaseRelocationTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 52]));
		sb->Append((const UTF8Char*)"\r\nDebugTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 56]));
		sb->Append((const UTF8Char*)"\r\nDebugTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 60]));
		sb->Append((const UTF8Char*)"\r\nArchitectureTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 64]));
		sb->Append((const UTF8Char*)"\r\nArchitectureTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 68]));
		sb->Append((const UTF8Char*)"\r\nGlobalPtrTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 72]));
		sb->Append((const UTF8Char*)"\r\nGlobalPtrTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 76]));
		sb->Append((const UTF8Char*)"\r\nTLSTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 80]));
		sb->Append((const UTF8Char*)"\r\nTLSTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 84]));
		sb->Append((const UTF8Char*)"\r\nLoadConfigTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 88]));
		sb->Append((const UTF8Char*)"\r\nLoadConfigTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 92]));
		sb->Append((const UTF8Char*)"\r\nBoundImportTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 96]));
		sb->Append((const UTF8Char*)"\r\nBoundImportTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 100]));
		sb->Append((const UTF8Char*)"\r\nImportAddrTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 104]));
		sb->Append((const UTF8Char*)"\r\nImportAddrTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 108]));
		sb->Append((const UTF8Char*)"\r\nDelayImportTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 112]));
		sb->Append((const UTF8Char*)"\r\nDelayImportTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 116]));
		sb->Append((const UTF8Char*)"\r\nCLRRuntimeTableVAddr = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[ofst + 120]));
		sb->Append((const UTF8Char*)"\r\nCLRRuntimeTableSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[ofst + 124]));
		//////////////////////

		MemFree(packBuff);
	}
	else if (pack->packType == 5)
	{
		packBuff = MemAlloc(UInt8, (UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);

		sb->Append((const UTF8Char*)"Name = ");
		sb->AppendS(packBuff, 8);
		sb->Append((const UTF8Char*)"\r\nVirtualSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[8]));
		sb->Append((const UTF8Char*)"\r\nVirtualAddress = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[12]));
		sb->Append((const UTF8Char*)"\r\nSizeOfRawData = ");
		sb->AppendU32(ReadUInt32(&packBuff[16]));
		sb->Append((const UTF8Char*)"\r\nPointerToRawData = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[20]));
		sb->Append((const UTF8Char*)"\r\nPointerToRelocations = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[24]));
		sb->Append((const UTF8Char*)"\r\nPointerToLinenumbers = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[28]));
		sb->Append((const UTF8Char*)"\r\nNumberOfRelocations = ");
		sb->AppendU16(ReadUInt16(&packBuff[32]));
		sb->Append((const UTF8Char*)"\r\nNumberOfLinenumbers = ");
		sb->AppendU16(ReadUInt16(&packBuff[34]));
		sb->Append((const UTF8Char*)"\r\nCharacteristics = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[36]));

		MemFree(packBuff);
	}
	else if (pack->packType == 6)
	{
		UInt32 nAddr;
		UInt32 nName;
		sb->Append((const UTF8Char*)"Export Flags = 0x");
		sb->AppendHex32(ReadUInt32(&this->imageBuff[pack->fileOfst]));
		sb->Append((const UTF8Char*)"\r\nTimestamp = ");
		sb->AppendU32(ReadUInt32(&this->imageBuff[pack->fileOfst + 4]));
		Data::DateTime dt;
		dt.SetUnixTimestamp(ReadUInt32(&this->imageBuff[pack->fileOfst + 4]));
		sb->Append((const UTF8Char*)" (");
		sb->AppendDate(&dt);
		sb->AppendChar(')', 1);
		sb->Append((const UTF8Char*)"\r\nVersion = ");
		sb->AppendU16(ReadUInt16(&this->imageBuff[pack->fileOfst + 8]));
		sb->AppendChar('.', 1);
		sb->AppendU16(ReadUInt16(&this->imageBuff[pack->fileOfst + 10]));
		sb->Append((const UTF8Char*)"\r\nName = ");
		sb->Append((const UTF8Char*)&this->imageBuff[ReadUInt32(&this->imageBuff[pack->fileOfst + 12])]);
		sb->Append((const UTF8Char*)"\r\nOrdinal Base = ");
		sb->AppendU32(ReadUInt32(&this->imageBuff[pack->fileOfst + 16]));
		nAddr = ReadUInt32(&this->imageBuff[pack->fileOfst + 20]);
		nName = ReadUInt32(&this->imageBuff[pack->fileOfst + 24]);
		sb->Append((const UTF8Char*)"\r\nAddress Table Entries = ");
		sb->AppendU32(nAddr);
		sb->Append((const UTF8Char*)"\r\nNumber of Name Pointers = ");
		sb->AppendU32(nName);
		sb->Append((const UTF8Char*)"\r\nExport Address Table RVA = 0x");
		sb->AppendHex32(ReadUInt32(&this->imageBuff[pack->fileOfst + 28]));
		sb->Append((const UTF8Char*)"\r\nName Pointer RVA = 0x");
		sb->AppendHex32(ReadUInt32(&this->imageBuff[pack->fileOfst + 32]));
		sb->Append((const UTF8Char*)"\r\nOrdinal Table RVA = 0x");
		sb->AppendHex32(ReadUInt32(&this->imageBuff[pack->fileOfst + 36]));
		UInt8 *addrTablePtr = &this->imageBuff[ReadUInt32(&this->imageBuff[pack->fileOfst + 28])];
		UInt8 *nameTablePtr = &this->imageBuff[ReadUInt32(&this->imageBuff[pack->fileOfst + 32])];
		UInt8 *ordinalTablePtr = &this->imageBuff[ReadUInt32(&this->imageBuff[pack->fileOfst + 36])];
		UInt32 i = 0;
		while (i < nName)
		{
			sb->Append((const UTF8Char*)"\r\nAddr = 0x");
			sb->AppendHex32(ReadUInt32(&addrTablePtr[ReadUInt16(ordinalTablePtr) * 4]));
			sb->Append((const UTF8Char*)", Name = ");
			sb->Append((const UTF8Char*)&this->imageBuff[ReadUInt32(nameTablePtr)]);
			nameTablePtr += 4;
			ordinalTablePtr += 2;
			i++;
		}
	}
	else if (pack->packType == 7)
	{
		sb->Append((const UTF8Char*)"\r\nImport Lookup Table RVA = 0x");
		sb->AppendHex32(ReadUInt32(&this->imageBuff[pack->fileOfst + 0]));
		sb->Append((const UTF8Char*)"\r\nTimestamp = ");
		sb->AppendU32(ReadUInt32(&this->imageBuff[pack->fileOfst + 4]));
		Data::DateTime dt;
		dt.SetUnixTimestamp(ReadUInt32(&this->imageBuff[pack->fileOfst + 4]));
		sb->Append((const UTF8Char*)" (");
		sb->AppendDate(&dt);
		sb->AppendChar(')', 1);
		sb->Append((const UTF8Char*)"\r\nForwarder Chain = ");
		sb->AppendU32(ReadUInt32(&this->imageBuff[pack->fileOfst + 8]));
		sb->Append((const UTF8Char*)"\r\nName RVA = 0x");
		sb->AppendHex32(ReadUInt32(&this->imageBuff[pack->fileOfst + 12]));
		sb->Append((const UTF8Char*)"\r\nImport Address Table RVA = 0x");
		sb->AppendHex32(ReadUInt32(&this->imageBuff[pack->fileOfst + 16]));
		UInt8 *lutPtr = &this->imageBuff[ReadUInt32(&this->imageBuff[pack->fileOfst + 0])];
		UInt8 *nameTablePtr = &this->imageBuff[ReadUInt32(&this->imageBuff[pack->fileOfst + 12])];
		UInt8 *iatPtr = &this->imageBuff[ReadUInt32(&this->imageBuff[pack->fileOfst + 16])];
	}
	return true;
}

Bool IO::FileAnalyse::EXEFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::EXEFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::EXEFileAnalyse::TrimPadding(const UTF8Char *outputFile)
{
	return false;
}

const UTF8Char *IO::FileAnalyse::EXEFileAnalyse::PackTypeGetName(Int32 packType)
{
	switch (packType)
	{
	case 0:
		return (const UTF8Char*)"MS-DOS Header";
	case 1:
		return (const UTF8Char*)"MS-DOS Byte Code";
	case 2:
		return (const UTF8Char*)"COFF File Header";
	case 3:
		return (const UTF8Char*)"PE32 Optional Header";
	case 4:
		return (const UTF8Char*)"PE32+ Optional Header";
	case 5:
		return (const UTF8Char*)"Section Entry";
	case 6:
		return (const UTF8Char*)"Export Table";
	case 7:
		return (const UTF8Char*)"Import Table";
	case 8:
		return (const UTF8Char*)"Resource Table";
	case 9:
		return (const UTF8Char*)"Exception Table";
	case 10:
		return (const UTF8Char*)"Certificate Table";
	case 11:
		return (const UTF8Char*)"Base Relocation Table";
	case 12:
		return (const UTF8Char*)"Debug";
	case 13:
		return (const UTF8Char*)"Architecture";
	case 14:
		return (const UTF8Char*)"Global Ptr";
	case 15:
		return (const UTF8Char*)"TLS Table";
	case 16:
		return (const UTF8Char*)"Load Config Table";
	case 17:
		return (const UTF8Char*)"Bound Import";
	case 18:
		return (const UTF8Char*)"IAT";
	case 19:
		return (const UTF8Char*)"Delay Import Descriptor";
	case 20:
		return (const UTF8Char*)"CLR Runtime Header";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
