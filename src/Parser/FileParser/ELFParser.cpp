#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/EXEFile.h"
#include "Parser/FileParser/ELFParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/Cpp/CppDemangler.h"

Int64 __stdcall Parser::FileParser::ELFParser::TReadInt64(UnsafeArray<const UInt8> pVal)
{
	return ReadInt64(pVal.Ptr());
}

Int64 __stdcall Parser::FileParser::ELFParser::TReadMInt64(UnsafeArray<const UInt8> pVal)
{
	return ReadMInt64(pVal.Ptr());
}

Int32 __stdcall Parser::FileParser::ELFParser::TReadInt32(UnsafeArray<const UInt8> pVal)
{
	return ReadInt32(pVal.Ptr());
}

Int32 __stdcall Parser::FileParser::ELFParser::TReadMInt32(UnsafeArray<const UInt8> pVal)
{
	return ReadMInt32(pVal.Ptr());
}

Int16 __stdcall Parser::FileParser::ELFParser::TReadInt16(UnsafeArray<const UInt8> pVal)
{
	return ReadInt16(pVal.Ptr());
}

Int16 __stdcall Parser::FileParser::ELFParser::TReadMInt16(UnsafeArray<const UInt8> pVal)
{
	return ReadMInt16(pVal.Ptr());
}

Parser::FileParser::ELFParser::ELFParser()
{
}

Parser::FileParser::ELFParser::~ELFParser()
{
}

Int32 Parser::FileParser::ELFParser::GetName()
{
	return *(Int32*)"ELFP";
}

void Parser::FileParser::ELFParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::EXEFile)
	{
		selector->AddFilter(CSTR("*.so"), CSTR("Shared Object File"));
	}
}

IO::ParserType Parser::FileParser::ELFParser::GetParserType()
{
	return IO::ParserType::EXEFile;
}

Optional<IO::ParsedObject> Parser::FileParser::ELFParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (hdr[0] != 0x7f || hdr[1] != 'E' || hdr[2] != 'L' || hdr[3] != 'F')
	{
		return 0;
	}

	RInt64Func readInt64;
	RInt32Func readInt32;
	RInt16Func readInt16;
	UTF8Char sbuff[256];
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	IO::EXEFile *exef;
	if (hdr[4] != 1 && hdr[4] != 2)
	{
		return 0;
	}
	NEW_CLASS(exef, IO::EXEFile(fd->GetFullName()));
	if (hdr[4] == 1)
	{
		exef->AddProp(CSTR("File Format"), CSTR("ELF32"));
	}
	else if (hdr[4] == 2)
	{
		exef->AddProp(CSTR("File Format"), CSTR("ELF64"));
	}
	if (hdr[5] == 1)
	{
		exef->AddProp(CSTR("Endianness"), CSTR("1 (Little)"));
		readInt64 = TReadInt64;
		readInt32 = TReadInt32;
		readInt16 = TReadInt16;
	}
	else if (hdr[5] == 2)
	{
		exef->AddProp(CSTR("Endianness"), CSTR("2 (Big)"));
		readInt64 = TReadMInt64;
		readInt32 = TReadMInt32;
		readInt16 = TReadMInt16;
	}
	else
	{
		sptr = Text::StrUInt32(sbuff, hdr[5]);
		exef->AddProp(CSTR("Endianness"), CSTRP(sbuff, sptr));
		readInt64 = TReadInt64;
		readInt32 = TReadInt32;
		readInt16 = TReadInt16;
	}
	sptr = Text::StrUInt32(sbuff, hdr[6]);
	exef->AddProp(CSTR("Format Version"), CSTRP(sbuff, sptr));
	switch (hdr[7])
	{
	case 0:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x00 (System V)"));
		break;
	case 1:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x01 (HP-UX)"));
		break;
	case 2:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x02 (NetBSD)"));
		break;
	case 3:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x03 (Linux)"));
		break;
	case 4:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x04 (GNU Hurd)"));
		break;
	case 6:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x06 (Solaris)"));
		break;
	case 7:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x07 (AIX)"));
		break;
	case 8:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x08 (IRIX)"));
		break;
	case 9:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x09 (FreeBSD)"));
		break;
	case 10:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x0A (Tru64)"));
		break;
	case 11:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x0B (Novell Modesto)"));
		break;
	case 12:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x0C (OpenBSD)"));
		break;
	case 13:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x0D (OpenVMS)"));
		break;
	case 14:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x0E (NonStop Kernel)"));
		break;
	case 15:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x0F (AROS)"));
		break;
	case 16:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x10 (Fenix OS)"));
		break;
	case 17:
		exef->AddProp(CSTR("OS ABI"), CSTR("0x11 (CloudABI)"));
		break;
	default:
		sptr = Text::StrHexByte(Text::StrConcatC(sbuff, UTF8STRC("0x")), hdr[7]);
		exef->AddProp(CSTR("OS ABI"), CSTRP(sbuff, sptr));
		break;
	}
	switch (readInt16(&hdr[16]))
	{
	case 1:
		exef->AddProp(CSTR("Type"), CSTR("1 (Relocatable)"));
		break;
	case 2:
		exef->AddProp(CSTR("Type"), CSTR("2 (Executable)"));
		break;
	case 3:
		exef->AddProp(CSTR("Type"), CSTR("3 (Shared)"));
		break;
	case 4:
		exef->AddProp(CSTR("Type"), CSTR("3 (Core)"));
		break;
	default:
		sptr = Text::StrUInt32(sbuff, (UInt16)readInt16(&hdr[16]));
		exef->AddProp(CSTR("Type"), CSTRP(sbuff, sptr));
		break;
	}
	switch (readInt16(&hdr[18]))
	{
	case 0:
		exef->AddProp(CSTR("Machine"), CSTR("0 (No machine)"));
		break;
	case 1:
		exef->AddProp(CSTR("Machine"), CSTR("1 (AT&T WE 32100)"));
		break;
	case 2:
		exef->AddProp(CSTR("Machine"), CSTR("2 (SPARC)"));
		break;
	case 3:
		exef->AddProp(CSTR("Machine"), CSTR("3 (x86)"));
		break;
	case 4:
		exef->AddProp(CSTR("Machine"), CSTR("4 (Motorola 68000)"));
		break;
	case 5:
		exef->AddProp(CSTR("Machine"), CSTR("5 (Motorola 88000)"));
		break;
	case 7:
		exef->AddProp(CSTR("Machine"), CSTR("7 (Intel 80860)"));
		break;
	case 8:
		exef->AddProp(CSTR("Machine"), CSTR("8 (MIPS)"));
		break;
	case 0x14:
		exef->AddProp(CSTR("Machine"), CSTR("0x14 (PowerPC)"));
		break;
	case 0x16:
		exef->AddProp(CSTR("Machine"), CSTR("0x16 (S390)"));
		break;
	case 0x28:
		exef->AddProp(CSTR("Machine"), CSTR("0x28 (ARM)"));
		break;
	case 0x2A:
		exef->AddProp(CSTR("Machine"), CSTR("0x2A (SuperH)"));
		break;
	case 0x32:
		exef->AddProp(CSTR("Machine"), CSTR("0x32 (IA-64)"));
		break;
	case 0x3E:
		exef->AddProp(CSTR("Machine"), CSTR("0x3E (x86-64)"));
		break;
	case 0xB7:
		exef->AddProp(CSTR("Machine"), CSTR("0xB7 (AArch64)"));
		break;
	case 0xF3:
		exef->AddProp(CSTR("Machine"), CSTR("0xF3 (RISC-V)"));
		break;
	default:
		sptr = Text::StrUInt32(sbuff, (UInt16)readInt16(&hdr[18]));
		exef->AddProp(CSTR("Machine"), CSTRP(sbuff, sptr));
		break;
	}
	sptr = Text::StrUInt32(sbuff, (UInt32)readInt32(&hdr[20]));
	exef->AddProp(CSTR("Version"), CSTRP(sbuff, sptr));
	UInt32 phSize;
	UInt32 phCnt;
	UInt32 shSize;
	UInt32 shCnt;
	UInt32 secNameInd;
	UOSInt i;
	UOSInt j;
	if (hdr[4] == 1)
	{
		UInt32 entryPt = (UInt32)readInt32(&hdr[24]);
		UInt32 pht = (UInt32)readInt32(&hdr[28]);
		UInt32 sht = (UInt32)readInt32(&hdr[32]);
		phSize = (UInt16)readInt16(&hdr[42]);
		phCnt = (UInt16)readInt16(&hdr[44]);
		shSize = (UInt16)readInt16(&hdr[46]);
		shCnt = (UInt16)readInt16(&hdr[48]);
		sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), entryPt);
		exef->AddProp(CSTR("Entry Point"), CSTRP(sbuff, sptr));
		sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), pht);
		exef->AddProp(CSTR("Program Header Table"), CSTRP(sbuff, sptr));
		sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), sht);
		exef->AddProp(CSTR("Section Header Table"), CSTRP(sbuff, sptr));
		sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), (UInt32)readInt32(&hdr[36]));
		exef->AddProp(CSTR("Flags"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, (UInt16)readInt16(&hdr[40]));
		exef->AddProp(CSTR("Header Size"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, phSize);
		exef->AddProp(CSTR("Program Header Table Entry Size"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, phCnt);
		exef->AddProp(CSTR("Program Header Table Entry Count"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, shSize);
		exef->AddProp(CSTR("Section Header Table Entry Size"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, shCnt);
		exef->AddProp(CSTR("Section Header Table Entry Count"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, secNameInd = (UInt16)readInt16(&hdr[50]));
		exef->AddProp(CSTR("Section Name Index"), CSTRP(sbuff, sptr));

		if (phSize >= 32)
		{
			Data::ByteBuffer progHdr(phSize * phCnt);
			fd->GetRealData(pht, phSize * phCnt, progHdr);

			i = 0;
			j = 0;
			while ((UOSInt)i < phCnt)
			{
				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Type"));
				switch ((UInt32)readInt32(&progHdr[j]))
				{
				case 0:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("0 (Null)"));
					break;
				case 1:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("1 (Load)"));
					break;
				case 2:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("2 (Dynamic)"));
					break;
				case 3:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("3 (Interp)"));
					break;
				case 4:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("4 (Note)"));
					break;
				case 5:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("5 (ShLib)"));
					break;
				case 6:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("6 (PHdr)"));
					break;
				default:
					sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&progHdr[j]));
					exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));
					break;
				}
				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Offset"));
				sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&progHdr[j + 4]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Virtual Address"));
				sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&progHdr[j + 8]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Physical Address"));
				sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&progHdr[j + 12]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" File Size"));
				sptr2 = Text::StrUInt32(sbuff2, (UInt32)readInt32(&progHdr[j + 16]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Memory Size"));
				sptr2 = Text::StrUInt32(sbuff2, (UInt32)readInt32(&progHdr[j + 20]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Flags"));
				sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&progHdr[j + 24]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Align"));
				sptr2 = Text::StrUInt32(sbuff2, (UInt32)readInt32(&progHdr[j + 28]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				i++;
				j += phSize;
			}
		}

		if (shSize >= 40)
		{
			Data::ByteBuffer progHdr(0);
			Data::ByteBuffer secHdr(shSize * shCnt);
			fd->GetRealData(sht, shSize * shCnt, secHdr);

			if (readInt32(&secHdr[shSize * secNameInd + 4]) == 3)
			{
				UInt32 sz = (UInt32)readInt32(&secHdr[shSize * secNameInd + 20]);
				progHdr.ChangeSizeAndClear(sz + 1);
				fd->GetRealData((UInt32)readInt32(&secHdr[shSize * secNameInd + 16]), sz, progHdr);
				progHdr[sz] = 0;
			}

			i = 0;
			j = 0;
			while (i < shCnt)
			{
				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Name Offset"));
				sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&secHdr[j + 0]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				if (progHdr.GetSize() > 0)
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
					sptr = Text::StrUOSInt(sptr, i);
					sptr = Text::StrConcatC(sptr, UTF8STRC(" Name Offset"));
					sptr2 = &progHdr[readInt32(&secHdr[j + 0])];
					exef->AddProp(CSTRP(sbuff, sptr), Text::CStringNN(UnsafeArray<const UTF8Char>(sptr2), Text::StrCharCnt(sptr2)));
				}

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Type"));
				switch ((UInt32)readInt32(&secHdr[j + 4]))
				{
				case 0:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("0 (Null)"));
					break;
				case 1:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("1 (Program Data)"));
					break;
				case 2:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("2 (Symbol Table)"));
					break;
				case 3:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("3 (String Table)"));
					break;
				case 4:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("4 (Relocation Entries with addends)"));
					break;
				case 5:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("5 (Symbol Hash Table)"));
					break;
				case 6:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("6 (Dynamic Linking Info)"));
					break;
				case 7:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("7 (Notes)"));
					break;
				case 8:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("8 (Program space with no data)"));
					break;
				case 9:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("9 (Relocation Entries)"));
					break;
				case 10:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("10 (ShLib)"));
					break;
				case 11:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("11 (Dynamic Linker Symbol Table)"));
					break;
				case 14:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("14 (Array of Constructors)"));
					break;
				case 15:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("15 (Array of Destructors)"));
					break;
				case 16:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("16 (Array of Pre-constructors)"));
					break;
				case 17:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("17 (Section Group)"));
					break;
				case 18:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("18 (Extended Section Indices)"));
					break;
				case 19:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("19 (Number of defined types)"));
					break;
				default:
					sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&secHdr[j + 4]));
					exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));
					break;
				}

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Flags"));
				sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&secHdr[j + 8]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Virtual Address"));
				sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&secHdr[j + 12]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Offset"));
				sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&secHdr[j + 16]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Size"));
				sptr2 = Text::StrUInt32(sbuff2, (UInt32)readInt32(&secHdr[j + 20]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Associated Section"));
				sptr2 = Text::StrUInt32(sbuff2, (UInt32)readInt32(&secHdr[j + 24]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Extra info"));
				sptr2 = Text::StrUInt32(sbuff2, (UInt32)readInt32(&secHdr[j + 28]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Align"));
				sptr2 = Text::StrUInt32(sbuff2, (UInt32)readInt32(&secHdr[j + 32]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Entry Size"));
				sptr2 = Text::StrUInt32(sbuff2, (UInt32)readInt32(&secHdr[j + 36]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				if (progHdr.GetSize() > 0)
				{
					const UTF8Char *name = (const UTF8Char*)&progHdr[readInt32(&secHdr[j + 0])];
					UInt32 assSec = (UInt32)readInt32(&secHdr[j + 24]);
					if (Text::StrEquals(name, (const UTF8Char*)".dynsym") || Text::StrEquals(name, (const UTF8Char*)".symtab"))
					{
						OSInt k;
						OSInt l;
						UInt32 tmpVal;
						UInt32 symSize = (UInt32)readInt32(&secHdr[j + 20]);
						UInt32 strSize = (UInt32)readInt32(&secHdr[assSec * shSize + 20]);
						Data::ByteBuffer symTab(symSize);
						Data::ByteBuffer strTab(strSize);
						fd->GetRealData((UInt32)readInt32(&secHdr[j + 16]), symSize, symTab);
						fd->GetRealData((UInt32)readInt32(&secHdr[assSec * shSize + 16]), strSize, strTab);
						k = 0;
						l = 0;
						while ((UOSInt)l < symSize)
						{
							if ((symTab[l +12] & 15) == 2)
							{
								tmpVal = (UInt32)readInt32(&symTab[l]);
								sptr2 = Text::Cpp::CppDemangler::ToFuncName(sbuff2, &strTab[tmpVal]);
								exef->AddExportFunc(CSTRP(sbuff2, sptr2));
							}
							else
							{
								sptr = Text::StrOSInt(Text::StrConcatC(sbuff, UTF8STRC("Symbol ")), k);
								tmpVal = (UInt32)readInt32(&symTab[l]);
								if (tmpVal == 0)
								{
									sptr2 = Text::StrConcatC(sbuff2, UTF8STRC("(null)"));
								}
								else
								{
									sptr2 = Text::StrConcat(sbuff2, &strTab[tmpVal]);
								}
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(", addr = 0x"));
								sptr2 = Text::StrHexVal32(sptr2, (UInt32)readInt32(&symTab[l + 4]));
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(", size = "));
								sptr2 = Text::StrUInt32(sptr2, (UInt32)readInt32(&symTab[l + 8]));
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(", bind = "));
								sptr2 = Text::StrInt32(sptr2, symTab[l + 12] >> 4);
								switch (symTab[l + 12] >> 4)
								{
								case 0:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Local)"));
									break;
								case 1:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Global)"));
									break;
								case 2:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Weak)"));
									break;
								case 10:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Lo OS)"));
									break;
								case 12:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Hi OS)"));
									break;
								case 13:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Lo Proc)"));
									break;
								case 15:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Hi Proc)"));
									break;
								}
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(", type = "));
								sptr2 = Text::StrInt32(sptr2, symTab[l + 12] & 15);
								switch (symTab[l + 12] & 15)
								{
								case 0:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (No Type)"));
									break;
								case 1:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Object)"));
									break;
								case 2:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Function)"));
									break;
								case 3:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Section)"));
									break;
								case 4:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (File)"));
									break;
								case 5:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Common)"));
									break;
								case 6:
									sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (TLS)"));
									break;
								}
								exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));
							}

							k++;
							l += 16;
						}
					}
				}
				i++;
				j += shSize;
			}
		}
	}
	else if (hdr[4] == 2)
	{
		UInt64 entryPt = (UInt64)readInt64(&hdr[24]);
		UInt64 pht = (UInt64)readInt64(&hdr[32]);
		UInt64 sht = (UInt64)readInt64(&hdr[40]);
		phSize = (UInt16)readInt16(&hdr[54]);
		phCnt = (UInt16)readInt16(&hdr[56]);
		shSize = (UInt16)readInt16(&hdr[58]);
		shCnt = (UInt16)readInt16(&hdr[60]);
		sptr = Text::StrHexVal64(Text::StrConcatC(sbuff, UTF8STRC("0x")), entryPt);
		exef->AddProp(CSTR("Entry Point"), CSTRP(sbuff, sptr));
		sptr = Text::StrHexVal64(Text::StrConcatC(sbuff, UTF8STRC("0x")), pht);
		exef->AddProp(CSTR("Program Header Table"), CSTRP(sbuff, sptr));
		sptr = Text::StrHexVal64(Text::StrConcatC(sbuff, UTF8STRC("0x")), sht);
		exef->AddProp(CSTR("Section Header Table"), CSTRP(sbuff, sptr));
		sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), (UInt32)readInt32(&hdr[48]));
		exef->AddProp(CSTR("Flags"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, (UInt16)readInt16(&hdr[52]));
		exef->AddProp(CSTR("Header Size"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, phSize);
		exef->AddProp(CSTR("Program Header Table Entry Size"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, phCnt);
		exef->AddProp(CSTR("Program Header Table Entry Count"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, shSize);
		exef->AddProp(CSTR("Section Header Table Entry Size"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, shCnt);
		exef->AddProp(CSTR("Section Header Table Entry Count"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, secNameInd = (UInt16)readInt16(&hdr[62]));
		exef->AddProp(CSTR("Section Name Index"), CSTRP(sbuff, sptr));

		if (phSize >= 56)
		{
			Data::ByteBuffer progHdr(phSize * phCnt);
			fd->GetRealData(pht, phSize * phCnt, progHdr);

			i = 0;
			j = 0;
			while ((UOSInt)i < phCnt)
			{
				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Type"));
				switch ((UInt32)readInt32(&progHdr[j]))
				{
				case 0:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("0 (Null)"));
					break;
				case 1:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("1 (Load)"));
					break;
				case 2:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("2 (Dynamic)"));
					break;
				case 3:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("3 (Interp)"));
					break;
				case 4:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("4 (Note)"));
					break;
				case 5:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("5 (ShLib)"));
					break;
				case 6:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("6 (PHdr)"));
					break;
				default:
					sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&progHdr[j]));
					exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));
					break;
				}

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Flags"));
				sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&progHdr[j + 4]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Offset"));
				sptr2 = Text::StrHexVal64(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt64)readInt64(&progHdr[j + 8]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Virtual Address"));
				sptr2 = Text::StrHexVal64(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt64)readInt64(&progHdr[j + 16]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Physical Address"));
				sptr2 = Text::StrHexVal64(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt64)readInt64(&progHdr[j + 24]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" File Size"));
				sptr2 = Text::StrInt64(sbuff2, readInt64(&progHdr[j + 32]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Memory Size"));
				sptr2 = Text::StrInt64(sbuff2, readInt32(&progHdr[j + 40]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Program Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Align"));
				sptr2 = Text::StrInt64(sbuff2, readInt64(&progHdr[j + 48]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				i++;
				j += phSize;
			}
		}

		if (shSize >= 64)
		{
			Data::ByteBuffer progHdr;
			Data::ByteBuffer funcBuff;
			Data::ByteBuffer secHdr(shSize * shCnt);
			fd->GetRealData(sht, shSize * shCnt, secHdr);

			if (readInt32(&secHdr[shSize * secNameInd + 4]) == 3)
			{
				UOSInt sz = (UOSInt)(UInt64)readInt64(&secHdr[shSize * secNameInd + 32]);
				progHdr.ChangeSizeAndClear(sz + 1);
				fd->GetRealData((UInt32)readInt64(&secHdr[shSize * secNameInd + 24]), sz, progHdr);
				progHdr[sz] = 0;
			}

			i = 0;
			j = 0;
			while ((UOSInt)i < shCnt)
			{
				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Name Offset"));
				sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&secHdr[j + 0]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				if (progHdr.GetSize() > 0)
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
					sptr = Text::StrUOSInt(sptr, i);
					sptr = Text::StrConcatC(sptr, UTF8STRC(" Name Offset"));
					sptr2 = &progHdr[readInt32(&secHdr[j + 0])];
					exef->AddProp(CSTRP(sbuff, sptr), Text::CStringNN(UnsafeArray<const UTF8Char>(sptr2), Text::StrCharCnt(sptr2)));
				}

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Type"));
				switch ((UInt32)readInt32(&secHdr[j + 4]))
				{
				case 0:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("0 (Null)"));
					break;
				case 1:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("1 (Program Data)"));
					break;
				case 2:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("2 (Symbol Table)"));
					break;
				case 3:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("3 (String Table)"));
					break;
				case 4:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("4 (Relocation Entries with addends)"));
					break;
				case 5:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("5 (Symbol Hash Table)"));
					break;
				case 6:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("6 (Dynamic Linking Info)"));
					break;
				case 7:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("7 (Notes)"));
					break;
				case 8:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("8 (Program space with no data)"));
					break;
				case 9:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("9 (Relocation Entries)"));
					break;
				case 10:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("10 (ShLib)"));
					break;
				case 11:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("11 (Dynamic Linker Symbol Table)"));
					break;
				case 14:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("14 (Array of Constructors)"));
					break;
				case 15:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("15 (Array of Destructors)"));
					break;
				case 16:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("16 (Array of Pre-constructors)"));
					break;
				case 17:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("17 (Section Group)"));
					break;
				case 18:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("18 (Extended Section Indices)"));
					break;
				case 19:
					exef->AddProp(CSTRP(sbuff, sptr), CSTR("19 (Number of defined types)"));
					break;
				default:
					sptr2 = Text::StrHexVal32(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt32)readInt32(&secHdr[j + 4]));
					exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));
					break;
				}

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Flags"));
				sptr2 = Text::StrHexVal64(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt64)readInt64(&secHdr[j + 8]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Virtual Address"));
				sptr2 = Text::StrHexVal64(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt64)readInt64(&secHdr[j + 16]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Offset"));
				sptr2 = Text::StrHexVal64(Text::StrConcatC(sbuff2, UTF8STRC("0x")), (UInt64)readInt64(&secHdr[j + 24]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Size"));
				sptr2 = Text::StrUInt64(sbuff2, (UInt64)readInt64(&secHdr[j + 32]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Associated Section"));
				sptr2 = Text::StrUInt32(sbuff2, (UInt32)readInt32(&secHdr[j + 40]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Extra info"));
				sptr2 = Text::StrUInt32(sbuff2, (UInt32)readInt32(&secHdr[j + 44]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Align"));
				sptr2 = Text::StrUInt64(sbuff2, (UInt64)readInt64(&secHdr[j + 48]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				sptr = Text::StrConcatC(sbuff, UTF8STRC("Section Header "));
				sptr = Text::StrUOSInt(sptr, i);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Entry Size"));
				sptr2 = Text::StrUInt64(sbuff2, (UInt64)readInt64(&secHdr[j + 56]));
				exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

				if (progHdr.GetSize() > 0)
				{
					const UTF8Char *name = (const UTF8Char*)&progHdr[readInt32(&secHdr[j + 0])];
					UInt32 assSec = (UInt32)readInt32(&secHdr[j + 40]);
					Bool exportFunc = false;
					Bool symbolSec = false;
					if (Text::StrEquals(name, (const UTF8Char*)".dynsym"))
					{
						symbolSec = true;
					}
					else if (Text::StrEquals(name, (const UTF8Char*)".symtab"))
					{
						symbolSec = true;
						exportFunc = true;
					}
					if (symbolSec)
					{
						UOSInt k;
						UOSInt l;
						UInt32 tmpVal;
						UInt64 thisAddr;
						UInt64 thisSize;
						UInt64 symSize = (UInt64)readInt64(&secHdr[j + 32]);
						UInt64 strSize = (UInt64)readInt64(&secHdr[assSec * shSize + 32]);
						Data::ByteBuffer symTab((UOSInt)symSize);
						Data::ByteBuffer strTab((UOSInt)strSize);
						fd->GetRealData((UInt64)readInt32(&secHdr[j + 24]), symSize, symTab);
						fd->GetRealData((UInt64)readInt32(&secHdr[assSec * shSize + 24]), strSize, strTab);
						k = 0;
						l = 0;
						while (l < symSize)
						{
							if (exportFunc && (symTab[l + 4] & 15) == 2)
							{
								tmpVal = (UInt32)readInt32(&symTab[l]);
								sptr2 = Text::Cpp::CppDemangler::ToFuncName(sbuff2, &strTab[tmpVal]);
								exef->AddExportFunc(CSTRP(sbuff2, sptr2));
							}
							thisAddr = (UInt64)readInt64(&symTab[l + 8]);
							thisSize = (UInt64)readInt64(&symTab[l + 16]);
							if (thisSize > 0 && (symTab[l + 4] & 15) == 2)
							{
								tmpVal = (UInt32)readInt32(&symTab[l]);
								sptr2 = Text::Cpp::CppDemangler::ToFuncName(sbuff2, &strTab[tmpVal]);
								if (sbuff2[0] != '_' || sbuff2[1] != 'Z')
								{
									if (funcBuff.GetSize() < thisSize)
									{
										funcBuff.ChangeSizeAndClear((UOSInt)thisSize);
									}
									fd->GetRealData(thisAddr, (UOSInt)thisSize, funcBuff);
									//exef->AddFunc(CSTRP(sbuff2, sptr2), thisAddr, thisSize, funcBuff);
								}
							}

							sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Symbol ")), k);
							tmpVal = (UInt32)readInt32(&symTab[l]);
							if (tmpVal == 0)
							{
								sptr2 = Text::StrConcatC(sbuff2, UTF8STRC("(null)"));
							}
							else
							{
								sptr2 = Text::StrConcat(sbuff2, &strTab[tmpVal]);
							}
							sptr2 = Text::StrConcatC(sptr2, UTF8STRC(", addr = 0x"));
							sptr2 = Text::StrHexVal64(sptr2, thisAddr);
							sptr2 = Text::StrConcatC(sptr2, UTF8STRC(", size = "));
							sptr2 = Text::StrUInt64(sptr2, thisSize);
							sptr2 = Text::StrConcatC(sptr2, UTF8STRC(", bind = "));
							sptr2 = Text::StrInt32(sptr2, symTab[l + 4] >> 4);
							switch (symTab[l + 4] >> 4)
							{
							case 0:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Local)"));
								break;
							case 1:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Global)"));
								break;
							case 2:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Weak)"));
								break;
							case 10:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Lo OS)"));
								break;
							case 12:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Hi OS)"));
								break;
							case 13:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Lo Proc)"));
								break;
							case 15:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Hi Proc)"));
								break;
							}
							sptr2 = Text::StrConcatC(sptr2, UTF8STRC(", type = "));
							sptr2 = Text::StrInt32(sptr2, symTab[l + 4] & 15);
							switch (symTab[l + 4] & 15)
							{
							case 0:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (No Type)"));
								break;
							case 1:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Object)"));
								break;
							case 2:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Function)"));
								break;
							case 3:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Section)"));
								break;
							case 4:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (File)"));
								break;
							case 5:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (Common)"));
								break;
							case 6:
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(" (TLS)"));
								break;
							}
							exef->AddProp(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));

							k++;
							l += 24;
						}
					}
				}

				i++;
				j += shSize;
			}
		}
	}

	return exef;
}
