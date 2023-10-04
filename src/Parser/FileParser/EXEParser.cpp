#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/EXEFile.h"
#include "Parser/FileParser/EXEParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Parser::FileParser::EXEParser::EXEParser()
{
}

Parser::FileParser::EXEParser::~EXEParser()
{
}

Int32 Parser::FileParser::EXEParser::GetName()
{
	return *(Int32*)"EXEP";
}

void Parser::FileParser::EXEParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::EXEFile)
	{
		selector->AddFilter(CSTR("*.exe"), CSTR("EXE File"));
		selector->AddFilter(CSTR("*.dll"), CSTR("DLL File"));
		selector->AddFilter(CSTR("*.fon"), CSTR("Font File"));
	}
}

IO::ParserType Parser::FileParser::EXEParser::GetParserType()
{
	return IO::ParserType::EXEFile;
}

IO::ParsedObject *Parser::FileParser::EXEParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (*(Int16*)&hdr[0] != *(Int16*)"MZ")
	{
		return 0;
	}
	UInt32 fileSize = *(UInt16*)&hdr[2] + ((UInt32)((*(UInt16*)&hdr[4]) - 1) << 9);
	if (fd->GetDataSize() < fileSize)
		return 0;
	UInt32 relocSize = ReadUInt16(&hdr[6]);
	UInt32 hdrSize = (UInt32)ReadUInt16(&hdr[8]) << 4;
//	UInt32 minAlloc = (Int32)*(UInt16*)&hdr[10];
	UInt32 relocOfst = *(UInt16*)&hdr[24];
	
	Manage::Dasm::DasmX86_16_Regs regs;
	regs.EAX = 0;
	regs.ECX = (UInt16)(fileSize - hdrSize);
	regs.EBX = (UInt16)((fileSize - hdrSize) >> 16);
	regs.EDX = 0;
	regs.ESI = 0;
	regs.EDI = 0;
	regs.ESP = *(UInt16*)&hdr[16];
	regs.EBP = 0;
	regs.IP = *(UInt16*)&hdr[20];
	regs.CS = (UInt16)(0x80 + *(UInt16*)&hdr[22]);
	regs.DS = 0x70;
	regs.ES = 0x70;
	regs.SS = (UInt16)(0x80 + *(UInt16*)&hdr[14]);

	UTF8Char sbuff[256];
	UTF8Char *sptr;
	IO::EXEFile *exef;
	NEW_CLASS(exef, IO::EXEFile(fd->GetFullName()));
	UOSInt codeLen;

	sptr = Text::StrInt32(sbuff, ReadUInt16(&hdr[2]));
	exef->AddProp(CSTR("(DOS)Bytes on last page of file"), CSTRP(sbuff, sptr));
	sptr = Text::StrInt32(sbuff, ReadUInt16(&hdr[4]));
	exef->AddProp(CSTR("(DOS)Pages in file"), CSTRP(sbuff, sptr));
	sptr = Text::StrInt32(sbuff, ReadUInt16(&hdr[6]));
	exef->AddProp(CSTR("(DOS)Relocations"), CSTRP(sbuff, sptr));
	sptr = Text::StrInt32(sbuff, ReadUInt16(&hdr[8]));
	exef->AddProp(CSTR("(DOS)Size of header in paragraphs"), CSTRP(sbuff, sptr));
	sptr = Text::StrInt32(sbuff, ReadUInt16(&hdr[0xa]));
	exef->AddProp(CSTR("(DOS)Minimum extra paragraphs needed"), CSTRP(sbuff, sptr));
	sptr = Text::StrInt32(sbuff, ReadUInt16(&hdr[0xc]));
	exef->AddProp(CSTR("(DOS)Maximum extra paragraphs needed"), CSTRP(sbuff, sptr));
	sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&hdr[0xe]));
	exef->AddProp(CSTR("(DOS)Initial (relative) SS value"), CSTRP(sbuff, sptr));
	sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&hdr[0x10]));
	exef->AddProp(CSTR("(DOS)Initial SP value"), CSTRP(sbuff, sptr));
	sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&hdr[0x12]));
	exef->AddProp(CSTR("(DOS)Checksum"), CSTRP(sbuff, sptr));
	sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&hdr[0x14]));
	exef->AddProp(CSTR("(DOS)Initial IP value"), CSTRP(sbuff, sptr));
	sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&hdr[0x16]));
	exef->AddProp(CSTR("(DOS)Initial (relative) CS value"), CSTRP(sbuff, sptr));
	sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&hdr[0x18]));
	exef->AddProp(CSTR("(DOS)File address of relocation table"), CSTRP(sbuff, sptr));
	sptr = Text::StrInt32(sbuff, ReadUInt16(&hdr[0x1a]));
	exef->AddProp(CSTR("(DOS)Overlay number"), CSTRP(sbuff, sptr));
	sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&hdr[0x24]));
	exef->AddProp(CSTR("(DOS)OEM identifier"), CSTRP(sbuff, sptr));
	sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&hdr[0x26]));
	exef->AddProp(CSTR("(DOS)OEM information"), CSTRP(sbuff, sptr));
	sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&hdr[0x3c]));
	exef->AddProp(CSTR("(DOS)Offset to extended header"), CSTRP(sbuff, sptr));


	exef->AddDOSEnv(fileSize - hdrSize + 256, &regs, 0x70);
	UInt8 *codePtr = exef->GetDOSCodePtr(&codeLen);
	exef->SetDOSHasPSP(true);
	fd->GetRealData(hdrSize, codeLen - 256, Data::ByteArray(&codePtr[256], codeLen - 256));

	if (relocSize > 0)
	{
		UInt32 *relocTab;
		UInt32 i = relocSize;
		UInt32 j;
		UInt8 *u16Ptr;
		relocTab = MemAlloc(UInt32, relocSize);
		fd->GetRealData(relocOfst, relocSize << 2, Data::ByteArray((UInt8*)relocTab, relocSize * sizeof(UInt32)));
		while (i-- > 0)
		{
			j = relocTab[i];
			u16Ptr = &codePtr[256 + (j & 0xffff) + ((j >> 12) & 0xffff0)];
			WriteUInt16(u16Ptr, (UInt16)(ReadUInt16(u16Ptr) + 0x80));
		}
		MemFree(relocTab);
	}

	codePtr[0] = 0xcd;				// INT 20 INSTRUCTION (WORD)
	codePtr[1] = 0x20;
	*(UInt16*)&codePtr[2] = 0x9fff; // ADDRESS OF PARAGRAPH FOLLOWING THE PROGRAM(WORD)
	codePtr[4] = 0;					// RESERVED(0)(BYTE)
	codePtr[5] = 0x9a;				// CALL TO MS-DOS FUNCTION DISPATCHER(5 BYTES)
	*(UInt32*)&codePtr[6] = 0xf01dfeee;
	*(UInt32*)&codePtr[10] = 0;		// INTERRUPT 22H ADDRESS (DWORD)
	*(UInt32*)&codePtr[14] = 0;		// INTERRUPT 23H ADDRESS (DWORD)
	*(UInt32*)&codePtr[18] = 0;		// INTERRUPT 24H ADDRESS (DWORD)
	*(UInt16*)&codePtr[22] = 0;		// RESERVED (22 BYTES)
	codePtr[24] = 1;
	codePtr[25] = 1;
	codePtr[26] = 1;
	codePtr[27] = 0;
	codePtr[28] = 2;
	codePtr[29] = 0xff;
	codePtr[30] = 0xff;
	codePtr[31] = 0xff;
	codePtr[32] = 0xff;
	codePtr[33] = 0xff;
	codePtr[34] = 0xff;
	codePtr[35] = 0xff;
	codePtr[36] = 0xff;
	codePtr[37] = 0xff;
	codePtr[38] = 0xff;
	codePtr[39] = 0xff;
	codePtr[40] = 0xff;
	codePtr[41] = 0xff;
	codePtr[42] = 0xff;
	codePtr[43] = 0xff;
	*(UInt16*)&codePtr[44] = 0;		// SEGMENT ADDRESS OF ENVIRONMENT BLOCK
	*(UInt32*)&codePtr[46] = 0;		// RESERVED (34 BYTES)
	*(UInt16*)&codePtr[50] = 20;
	*(UInt32*)&codePtr[52] = 0x18;
	*(UInt32*)&codePtr[56] = 0xffffffff;
	*(UInt32*)&codePtr[60] = 0;
	*(UInt16*)&codePtr[64] = 0x206;
	UOSInt i = 66;
	while (i < 80)
		codePtr[i++] = 0;
	codePtr[80] = 0xcd;				// INT 21H,RETF INSTRUCTIONS
	codePtr[81] = 0x21;
	codePtr[82] = 0xcb;
	i = 83;							// FCB
	while (i < 128)
		codePtr[i++] = 0;
	while (i < 256)					// COMMAND-LINE PATAMETERS AND DISK TRANSFER AREA (DTA)
		codePtr[i++] = 0;

	UInt32 peOfst = ReadUInt32(&hdr[60]);
	if (peOfst >= 64 && peOfst <= 1024)
	{
		UInt8 peBuff[24];
		fd->GetRealData(peOfst, 24, BYTEARR(peBuff));
		if (*(Int32*)&peBuff[0] == *(Int32*)"PE\0")
		{
			UInt16 machine = ReadUInt16(&peBuff[4]);
			UInt32 numberOfSections = ReadUInt16(&peBuff[6]);
			Int32 timeDateStamp = ReadInt32(&peBuff[8]);
			UInt32 pointerToSymbolTable = ReadUInt32(&peBuff[12]);
			UInt32 numberOfSymbols = ReadUInt32(&peBuff[16]);
			UInt32 sizeOfOptionalHeader = ReadUInt16(&peBuff[20]);
			UInt16 characteristics = ReadUInt16(&peBuff[22]);

			exef->AddProp(CSTR("Extended Header Format"), CSTR("Portable Executable"));
			sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), machine);
			switch (machine)
			{
			case 0x1d3:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Matsushita AM33"));
				break;
			case 0x8664:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" x64"));
				break;
			case 0x1c0:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" ARM little endian"));
				break;
			case 0x1c4:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" ARMv7 (or higher) Thumb mode only"));
				break;
			case 0xaa64:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" ARMv8 in 64-bit mode"));
				break;
			case 0xebc:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" EFI byte code"));
				break;
			case 0x14c:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" i386"));
				break;
			case 0x200:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Intel Itanium"));
				break;
			case 0x9041:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Mitsubishi M32R little endian"));
				break;
			case 0x266:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" MIPS16"));
				break;
			case 0x366:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" MIPS with FPU"));
				break;
			case 0x466:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" MIPS16 with FPU"));
				break;
			case 0x1f0:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Power PC little endian"));
				break;
			case 0x1f1:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Power PC with floating point support"));
				break;
			case 0x166:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" MIPS little endian"));
				break;
			case 0x1a2:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Hitachi SH3"));
				break;
			case 0x1a3:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Hitachi SH3 DSP"));
				break;
			case 0x1a6:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Hitachi SH4"));
				break;
			case 0x1a8:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Hitachi SH5"));
				break;
			case 0x1c2:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" ARM or Thumb"));
				break;
			case 0x169:
				sptr = Text::StrConcatC(sptr, UTF8STRC(" MIPS little-endian WCE v2"));
				break;
			}
			exef->AddProp(CSTR("Machine"), CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, numberOfSections);
			exef->AddProp(CSTR("Number Of Sections"), CSTRP(sbuff, sptr));
			Data::DateTime dt;
			dt.SetUnixTimestamp((UInt32)timeDateStamp);
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
			exef->AddProp(CSTR("File Time"), CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), pointerToSymbolTable);
			exef->AddProp(CSTR("Pointer to symbol table"), CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, numberOfSymbols);
			exef->AddProp(CSTR("Number Of Symbools"), CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, sizeOfOptionalHeader);
			exef->AddProp(CSTR("Size Of Optional Header"), CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), characteristics);
			exef->AddProp(CSTR("Characteristics"), CSTRP(sbuff, sptr));

			if (sizeOfOptionalHeader >= 96)
			{
				Data::ByteBuffer optionalHdr(sizeOfOptionalHeader);
				if (fd->GetRealData(peOfst + 24, sizeOfOptionalHeader, optionalHdr) == sizeOfOptionalHeader)
				{
					UInt32 ofst;
					UInt32 rvaSize;
					UInt16 magic = ReadUInt16(&optionalHdr[0]);
					sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), magic);
					switch (magic)
					{
					case 0x10b:
						sptr = Text::StrConcatC(sptr, UTF8STRC(" PE32"));
						break;
					case 0x20b:
						sptr = Text::StrConcatC(sptr, UTF8STRC(" PE32+"));
						break;
					case 0x107:
						sptr = Text::StrConcatC(sptr, UTF8STRC(" ROM image"));
						break;
					}
					exef->AddProp(CSTR("Magic"), CSTRP(sbuff, sptr));
					sptr = Text::StrInt32(sbuff, optionalHdr[2]);
					if (magic == 0x10b || magic == 0x20b)
					{
						UInt32 sizeOfImage;

						exef->AddProp(CSTR("Major Linker Version"), CSTRP(sbuff, sptr));
						sptr = Text::StrInt32(sbuff, optionalHdr[3]);
						exef->AddProp(CSTR("Minor Linker Version"), CSTRP(sbuff, sptr));
						sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[4]));
						exef->AddProp(CSTR("Size Of Code"), CSTRP(sbuff, sptr));
						sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[8]));
						exef->AddProp(CSTR("Size Of Initialized Data"), CSTRP(sbuff, sptr));
						sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[12]));
						exef->AddProp(CSTR("Size Of Uninitialized Data"), CSTRP(sbuff, sptr));
						sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[16]));
						exef->AddProp(CSTR("Address Of Entry Point"), CSTRP(sbuff, sptr));
						sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[20]));
						exef->AddProp(CSTR("Base Of Code"), CSTRP(sbuff, sptr));
						if (magic == 0x10b)
						{
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[24]));
							exef->AddProp(CSTR("Base Of Data"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[28]));
							exef->AddProp(CSTR("Image Base"), CSTRP(sbuff, sptr));
						}
						else
						{
							sptr = Text::StrHexVal64(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt64(&optionalHdr[24]));
							exef->AddProp(CSTR("Image Base"), CSTRP(sbuff, sptr));
						}
						sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[32]));
						exef->AddProp(CSTR("Section Alignment"), CSTRP(sbuff, sptr));
						sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[36]));
						exef->AddProp(CSTR("File Alignment"), CSTRP(sbuff, sptr));
						sptr = Text::StrInt32(sbuff, ReadUInt16(&optionalHdr[40]));
						exef->AddProp(CSTR("Major Operating System Version"), CSTRP(sbuff, sptr));
						sptr = Text::StrInt32(sbuff, ReadUInt16(&optionalHdr[42]));
						exef->AddProp(CSTR("Minor Operating System Version"), CSTRP(sbuff, sptr));
						sptr = Text::StrInt32(sbuff, ReadUInt16(&optionalHdr[44]));
						exef->AddProp(CSTR("Major Image Version"), CSTRP(sbuff, sptr));
						sptr = Text::StrInt32(sbuff, ReadUInt16(&optionalHdr[46]));
						exef->AddProp(CSTR("Minor Image Version"), CSTRP(sbuff, sptr));
						sptr = Text::StrInt32(sbuff, ReadUInt16(&optionalHdr[48]));
						exef->AddProp(CSTR("Major Subsystem Version"), CSTRP(sbuff, sptr));
						sptr = Text::StrInt32(sbuff, ReadUInt16(&optionalHdr[50]));
						exef->AddProp(CSTR("Minor Subsystem Version"), CSTRP(sbuff, sptr));
						sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[52]));
						exef->AddProp(CSTR("Win32 Version Value"), CSTRP(sbuff, sptr));
						sptr = Text::StrUInt32(sbuff, sizeOfImage = ReadUInt32(&optionalHdr[56]));
						exef->AddProp(CSTR("Size Of Image"), CSTRP(sbuff, sptr));
						sptr = Text::StrUInt32(sbuff, ReadUInt32(&optionalHdr[60]));
						exef->AddProp(CSTR("Size Of Headers"), CSTRP(sbuff, sptr));
						sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[64]));
						exef->AddProp(CSTR("Check Sum"), CSTRP(sbuff, sptr));
						Int32 subsys = ReadUInt16(&optionalHdr[68]);
						sptr = Text::StrInt32(sbuff, subsys);
						switch (subsys)
						{
						case 0:
							sptr = Text::StrConcatC(sptr, UTF8STRC(" Unknown"));
							break;
						case 1:
							sptr = Text::StrConcatC(sptr, UTF8STRC(" Native/Device Driver"));
							break;
						case 2:
							sptr = Text::StrConcatC(sptr, UTF8STRC(" Windows GUI"));
							break;
						case 3:
							sptr = Text::StrConcatC(sptr, UTF8STRC(" Console"));
							break;
						case 7:
							sptr = Text::StrConcatC(sptr, UTF8STRC(" POSIX"));
							break;
						case 9:
							sptr = Text::StrConcatC(sptr, UTF8STRC(" Windows CE"));
							break;
						case 10:
							sptr = Text::StrConcatC(sptr, UTF8STRC(" EFI application"));
							break;
						case 11:
							sptr = Text::StrConcatC(sptr, UTF8STRC(" EFI driver with boot services"));
							break;
						case 12:
							sptr = Text::StrConcatC(sptr, UTF8STRC(" EFI driver with run-time services"));
							break;
						case 13:
							sptr = Text::StrConcatC(sptr, UTF8STRC(" EFI ROM image"));
							break;
						case 14:
							sptr = Text::StrConcatC(sptr, UTF8STRC(" XBOX"));
							break;
						}
						exef->AddProp(CSTR("Subsystem"), CSTRP(sbuff, sptr));
						sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&optionalHdr[70]));
						exef->AddProp(CSTR("DLL Characteristics"), CSTRP(sbuff, sptr));
						if (magic == 0x10b)
						{
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[72]));
							exef->AddProp(CSTR("Size Of Stack Reserve"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[76]));
							exef->AddProp(CSTR("Size Of Stack Commit"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[80]));
							exef->AddProp(CSTR("Size Of Heap Reserve"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[84]));
							exef->AddProp(CSTR("Size Of Heap Commit"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[88]));
							exef->AddProp(CSTR("Loader Flags"), CSTRP(sbuff, sptr));
							sptr = Text::StrUInt32(sbuff, rvaSize = ReadUInt32(&optionalHdr[92]));
							exef->AddProp(CSTR("Number Of Rva And Sizes"), CSTRP(sbuff, sptr));
							ofst = 96;
						}
						else
						{
							sptr = Text::StrInt64(sbuff, ReadInt64(&optionalHdr[72]));
							exef->AddProp(CSTR("Size Of Stack Reserve"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt64(sbuff, ReadInt64(&optionalHdr[80]));
							exef->AddProp(CSTR("Size Of Stack Commit"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt64(sbuff, ReadInt64(&optionalHdr[88]));
							exef->AddProp(CSTR("Size Of Heap Reserve"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt64(sbuff, ReadInt64(&optionalHdr[96]));
							exef->AddProp(CSTR("Size Of Heap Commit"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[104]));
							exef->AddProp(CSTR("Loader Flags"), CSTRP(sbuff, sptr));
							sptr = Text::StrUInt32(sbuff, rvaSize = ReadUInt32(&optionalHdr[108]));
							exef->AddProp(CSTR("Number Of Rva And Sizes"), CSTRP(sbuff, sptr));
							ofst = 112;
						}
						if (rvaSize >= 16)
						{
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 0]));
							exef->AddProp(CSTR("Export Table Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 4]));
							exef->AddProp(CSTR("Export Table Size"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 8]));
							exef->AddProp(CSTR("Import Table Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 12]));
							exef->AddProp(CSTR("Import Table Size"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 16]));
							exef->AddProp(CSTR("Resource Table Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 20]));
							exef->AddProp(CSTR("Resource Table Size"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 24]));
							exef->AddProp(CSTR("Exception Table Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 28]));
							exef->AddProp(CSTR("Exception Table Size"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 32]));
							exef->AddProp(CSTR("Certificate Table Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 36]));
							exef->AddProp(CSTR("Certificate Table Size"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 40]));
							exef->AddProp(CSTR("Base Relocation Table Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 44]));
							exef->AddProp(CSTR("Base Relocation Table Size"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 48]));
							exef->AddProp(CSTR("Debug Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 52]));
							exef->AddProp(CSTR("Debug Size"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 64]));
							exef->AddProp(CSTR("Global Ptr. Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 68]));
							exef->AddProp(CSTR("Global Ptr. Size"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 72]));
							exef->AddProp(CSTR("TLS Table Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 76]));
							exef->AddProp(CSTR("TLS Table Size"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 80]));
							exef->AddProp(CSTR("Load Config Table Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 84]));
							exef->AddProp(CSTR("Load Config Table Size"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 88]));
							exef->AddProp(CSTR("Bound Import Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 92]));
							exef->AddProp(CSTR("Bound Import Size"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 96]));
							exef->AddProp(CSTR("IAT Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 100]));
							exef->AddProp(CSTR("IAT Size"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 104]));
							exef->AddProp(CSTR("Delay Import Descriptor Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 108]));
							exef->AddProp(CSTR("Delay Import Descriptor Size"), CSTRP(sbuff, sptr));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&optionalHdr[ofst + 112]));
							exef->AddProp(CSTR("CLR Runtime Header Address"), CSTRP(sbuff, sptr));
							sptr = Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 116]));
							exef->AddProp(CSTR("CLR Runtime Size"), CSTRP(sbuff, sptr));
						}

						sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), sizeOfOptionalHeader + peOfst + 24);
						exef->AddProp(CSTR("End Of Optional Header"), CSTRP(sbuff, sptr));

						UInt32 rva;
//						UInt32 tabSize;
						UInt32 sOfst;
						UOSInt i;
						Text::StringBuilderUTF8 sb;
						Data::ByteBuffer sectionHeaders(numberOfSections * 40);
						Data::ByteBuffer exeImage(sizeOfImage);
						fd->GetRealData(sizeOfOptionalHeader + peOfst + 24, numberOfSections * 40, sectionHeaders);
						i = 0;
						sOfst = 0;
						while (i < numberOfSections)
						{
							i++;
							UInt32 virtSize = ReadUInt32(&sectionHeaders[sOfst + 8]);
							UInt32 virtAddr = ReadUInt32(&sectionHeaders[sOfst + 12]);
							UInt32 dataSize = ReadUInt32(&sectionHeaders[sOfst + 16]);
							UInt32 dataAddr = ReadUInt32(&sectionHeaders[sOfst + 20]);
							UInt32 readSize;

							sb.ClearStr();
							sb.AppendC(UTF8STRC("Section "));
							sb.AppendUOSInt(i);
							sb.AppendC(UTF8STRC(" Name"));
							sptr = Text::StrConcatS(sbuff, &sectionHeaders[sOfst], 9);
							exef->AddProp(sb.ToCString(), CSTRP(sbuff, sptr));

							if (virtSize < dataSize)
							{
								readSize = virtSize;
							}
							else
							{
								readSize = dataSize;
							}

							fd->GetRealData(dataAddr, readSize, exeImage.SubArray(virtAddr));
							if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".text")))
							{
							}
							else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".rdata")))
							{
							}
							else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".data")))
							{
							}
							else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".pdata")))
							{
							}
							else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".rsrc")))
							{
								ParseResource(exef, 0, sbuff, sbuff, &exeImage[virtAddr], 0, exeImage.Ptr());
								sbuff[0] = 0;
							}
							else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".reloc")))
							{
							}

							sb.ClearStr();
							sb.AppendC(UTF8STRC("Section "));
							sb.AppendUOSInt(i);
							sb.AppendC(UTF8STRC(" Virtual Size"));
							sptr = Text::StrUInt32(sbuff, virtSize);
							exef->AddProp(sb.ToCString(), CSTRP(sbuff, sptr));

							sb.ClearStr();
							sb.AppendC(UTF8STRC("Section "));
							sb.AppendUOSInt(i);
							sb.AppendC(UTF8STRC(" Virtual Address"));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), virtAddr);
							exef->AddProp(sb.ToCString(), CSTRP(sbuff, sptr));

							sb.ClearStr();
							sb.AppendC(UTF8STRC("Section "));
							sb.AppendUOSInt(i);
							sb.AppendC(UTF8STRC(" Size Of Raw Data"));
							sptr = Text::StrUInt32(sbuff, dataSize);
							exef->AddProp(sb.ToCString(), CSTRP(sbuff, sptr));

							sb.ClearStr();
							sb.AppendC(UTF8STRC("Section "));
							sb.AppendUOSInt(i);
							sb.AppendC(UTF8STRC(" Pointer To Raw Data"));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), dataAddr);
							exef->AddProp(sb.ToCString(), CSTRP(sbuff, sptr));

							sb.ClearStr();
							sb.AppendC(UTF8STRC("Section "));
							sb.AppendUOSInt(i);
							sb.AppendC(UTF8STRC(" Pointer To Relocations"));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&sectionHeaders[sOfst + 24]));
							exef->AddProp(sb.ToCString(), CSTRP(sbuff, sptr));

							sb.ClearStr();
							sb.AppendC(UTF8STRC("Section "));
							sb.AppendUOSInt(i);
							sb.AppendC(UTF8STRC(" Pointer To Line numbers"));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&sectionHeaders[sOfst + 28]));
							exef->AddProp(sb.ToCString(), CSTRP(sbuff, sptr));

							sb.ClearStr();
							sb.AppendC(UTF8STRC("Section "));
							sb.AppendUOSInt(i);
							sb.AppendC(UTF8STRC(" Number Of Relocations"));
							sptr = Text::StrInt32(sbuff, ReadUInt16(&sectionHeaders[sOfst + 32]));
							exef->AddProp(sb.ToCString(), CSTRP(sbuff, sptr));

							sb.ClearStr();
							sb.AppendC(UTF8STRC("Section "));
							sb.AppendUOSInt(i);
							sb.AppendC(UTF8STRC(" Number Of Line numbers"));
							sptr = Text::StrInt32(sbuff, ReadUInt16(&sectionHeaders[sOfst + 34]));
							exef->AddProp(sb.ToCString(), CSTRP(sbuff, sptr));

							sb.ClearStr();
							sb.AppendC(UTF8STRC("Section "));
							sb.AppendUOSInt(i);
							sb.AppendC(UTF8STRC(" Characteristics"));
							sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&sectionHeaders[sOfst + 36]));
							exef->AddProp(sb.ToCString(), CSTRP(sbuff, sptr));

							if (dataSize > 0)
							{
								fd->GetRealData(dataAddr, dataSize, exeImage.SubArray(virtAddr));
							}
							if (virtSize > dataSize)
							{
								MemClear(&exeImage[dataSize + virtAddr], virtSize - dataSize);
							}

							sOfst += 40;
						}

						rva = ReadUInt32(&optionalHdr[ofst + 8]);
//						tabSize = ReadInt32(&optionalHdr[ofst + 12]);
						if (rva != 0)
						{
							Int32 ilut;
							UInt32 nameRVA;
							UOSInt j;
							while (true)
							{
								ilut = ReadInt32(&exeImage[rva + 0]);
								nameRVA = ReadUInt32(&exeImage[rva + 12]);
								if (nameRVA == 0 || ilut == 0)
								{
									break;
								}
								sptr = Text::StrConcatS(sbuff, &exeImage[nameRVA], 64);
								j = exef->AddImportModule(CSTRP(sbuff, sptr));
								if (magic == 0x10b)
								{
									UInt32 funcRVA;
									while (true)
									{
										funcRVA = ReadUInt32(&exeImage[ilut]);
										if (funcRVA == 0)
											break;
										if (funcRVA & 0x80000000)
										{
											sptr = Text::StrConcatC(Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("(by ordinal ")), funcRVA & 0x7fff), UTF8STRC(")"));
											exef->AddImportFunc(j, CSTRP(sbuff, sptr));
										}
										else
										{
											sptr = Text::StrConcatC(sbuff, &exeImage[funcRVA + 2], 64);
											exef->AddImportFunc(j, CSTRP(sbuff, sptr));
										}
										ilut += 4;
									}
								}
								else if (magic == 0x20b)
								{
									UInt64 funcRVA;
									while (true)
									{
										funcRVA = ReadUInt64(&exeImage[ilut]);
										if (funcRVA == 0)
											break;
										if (funcRVA & 0x8000000000000000LL)
										{
											sptr = Text::StrConcatC(Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("(by ordinal ")), (Int32)(funcRVA & 0x7fff)), UTF8STRC(")"));
											exef->AddImportFunc(j, CSTRP(sbuff, sptr));
										}
										else
										{
											sptr = Text::StrConcatC(sbuff, &exeImage[(UOSInt)(funcRVA + 2) & 0x7fffffff], 64);
											exef->AddImportFunc(j, CSTRP(sbuff, sptr));
										}
										ilut += 8;
									}
								}

								rva += 20;
							}
						}

						rva = ReadUInt32(&optionalHdr[ofst + 0]);
//						tabSize = ReadInt32(&optionalHdr[ofst + 4]);
						if (rva != 0)
						{
							UInt32 nameRVA;
							UInt32 namePtrRVA;
							UInt32 nName;
							nameRVA = ReadUInt32(&exeImage[rva + 12]);
							if (nameRVA != 0)
							{
								Text::StrConcatC(sbuff, &exeImage[nameRVA], 64);
								nName = ReadUInt32(&exeImage[rva + 24]);
								namePtrRVA = ReadUInt32(&exeImage[rva + 32]);
								while (nName-- > 0)
								{
									rva = ReadUInt32(&exeImage[namePtrRVA]);
									if (rva == 0)
										break;
									sptr = Text::StrConcatS(sbuff, &exeImage[rva], 64);
									exef->AddExportFunc(CSTRP(sbuff, sptr));
									namePtrRVA += 4;
								}
							}
						}
					}
				}
			}
		}
		else if (*(Int16*)&peBuff[0] == *(Int16*)"NE")
		{
			UInt8 neBuff[64];
			MemCopyNO(neBuff, peBuff, 24);
			fd->GetRealData(peOfst + 24, 40, BYTEARR(neBuff).SubArray(24));

			exef->AddProp(CSTR("Extended Header Format"), CSTR("New Executable"));
			sptr = Text::StrInt32(sbuff, neBuff[2]);
			exef->AddProp(CSTR("Version of this linker"), CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, neBuff[3]);
			exef->AddProp(CSTR("Revision of this linker"), CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&neBuff[4]));
			exef->AddProp(CSTR("Entry table file offset"), CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, ReadUInt16(&neBuff[6]));
			exef->AddProp(CSTR("Number of bytes in the entry table"), CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&neBuff[8]));
			exef->AddProp(CSTR("32-bit CRC of entire contents of file"), CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&neBuff[12]));
			exef->AddProp(CSTR("Flag word"), CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, ReadUInt16(&neBuff[14]));
			exef->AddProp(CSTR("Segment number of automatic data segment"), CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, ReadUInt16(&neBuff[16]));
			exef->AddProp(CSTR("Initial size, in bytes, of dynamic heap added to the data segment"), CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, ReadUInt16(&neBuff[18]));
			exef->AddProp(CSTR("Initial size, in bytes, of stack added to the data segment"), CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal16(Text::StrConcatC(Text::StrHexVal16(sbuff, ReadUInt16(&neBuff[22])), UTF8STRC(":")), ReadUInt16(&neBuff[20]));
			exef->AddProp(CSTR("CS:IP"), CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal16(Text::StrConcatC(Text::StrHexVal16(sbuff, ReadUInt16(&neBuff[26])), UTF8STRC(":")), ReadUInt16(&neBuff[24]));
			exef->AddProp(CSTR("SS:SP"), CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, ReadUInt16(&neBuff[28]));
			exef->AddProp(CSTR("Number of entries in the Segment Table"), CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, ReadUInt16(&neBuff[30]));
			exef->AddProp(CSTR("Number of entries in the Module Reference Table"), CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, ReadUInt16(&neBuff[32]));
			exef->AddProp(CSTR("Number of bytes in the Non-Resident Name Table"), CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&neBuff[34]));
			exef->AddProp(CSTR("Segment Table file offset"), CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&neBuff[36]));
			exef->AddProp(CSTR("Resource Table file offset"), CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&neBuff[38]));
			exef->AddProp(CSTR("Resident Name Table file offset"), CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&neBuff[40]));
			exef->AddProp(CSTR("Module Reference Table file offset"), CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&neBuff[42]));
			exef->AddProp(CSTR("Imported Names Table file offset"), CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt32(&neBuff[44]));
			exef->AddProp(CSTR("Non-Resident Name Table offset"), CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, ReadUInt16(&neBuff[48]));
			exef->AddProp(CSTR("Number of movable entries in the Entry Table"), CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, ReadUInt16(&neBuff[50]));
			exef->AddProp(CSTR("Logical sector alignment shift count"), CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, ReadUInt16(&neBuff[52]));
			exef->AddProp(CSTR("Number of resource entries"), CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, neBuff[54]);
			exef->AddProp(CSTR("Executable type"), CSTRP(sbuff, sptr));

			UOSInt tableStart;
			UOSInt tableSize;
			UOSInt j;
			tableStart = ReadUInt16(&neBuff[38]);
			tableSize = ReadUInt16(&neBuff[40]) - tableStart;
			tableStart += peOfst;
			if (tableSize > 0)
			{
				Data::ByteBuffer nameTable(tableSize);
				fd->GetRealData(tableStart, tableSize, nameTable);
				i = 0;
				j = 0;
				while (j < tableSize && nameTable[j] != 0)
				{
					sptr = Text::StrConcatC(sbuff, &nameTable[j + 1], nameTable[j]);
					exef->AddProp(CSTR("ResidentName"), CSTRP(sbuff, sptr));

					i++;
					j += (UOSInt)nameTable[j] + 1;
				}
			}

			tableStart = ReadUInt32(&neBuff[44]);
			tableSize = ReadUInt16(&neBuff[32]);
			if (tableSize > 0)
			{
				Data::ByteBuffer nameTable(tableSize);
				fd->GetRealData(tableStart, tableSize, nameTable);
				i = 0;
				j = 0;
				while (j < tableSize && nameTable[j] != 0)
				{
					sptr = Text::StrConcatC(sbuff, &nameTable[j + 1], nameTable[j]);
					exef->AddProp(CSTR("NonResidentName"), CSTRP(sbuff, sptr));

					i++;
					j += (UOSInt)nameTable[j] + 1;
				}
			}

			tableStart = ReadUInt16(&neBuff[34]);
			tableSize = ReadUInt16(&neBuff[38]) - tableStart;
			tableStart += peOfst;
			if (tableSize > 0)
			{
				Data::ByteBuffer nameTable(tableSize);
				fd->GetRealData(tableStart, tableSize, nameTable);
				sptr = Text::StrInt32(sbuff, ReadUInt16(&nameTable[0]));
				exef->AddProp(CSTR("Resource Table: Alignment shift count"), CSTRP(sbuff, sptr));
				j = 2;
				while (j < tableSize)
				{
					if (ReadUInt16(&nameTable[j]) == 0)
					{
						j += 2;
						break;
					}
					IO::EXEFile::ResourceType rt = GetResourceType(ReadUInt16(&nameTable[j]) & 0x7fff);
					sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&nameTable[j]));
					exef->AddProp(CSTR("-Type ID"), CSTRP(sbuff, sptr));
					i = ReadUInt16(&nameTable[j + 2]);
					sptr = Text::StrOSInt(sbuff, ReadUInt16(&nameTable[j + 2]));
					exef->AddProp(CSTR("-Number of resources for this type"), CSTRP(sbuff, sptr));
					j += 8;
					while (j < tableSize && i-- > 0)
					{
						sptr = Text::StrConcatC(Text::StrHexVal32(Text::StrConcatC(Text::StrInt32(sbuff, ReadUInt16(&nameTable[j])), UTF8STRC(" (0x")), (UInt32)(ReadUInt16(&nameTable[j]) << ReadUInt16(&nameTable[0]))), UTF8STRC(")"));
						exef->AddProp(CSTR("--File offset to the contents of the resource data"), CSTRP(sbuff, sptr));
						sptr = Text::StrInt32(sbuff, ReadUInt16(&nameTable[j + 2]));
						exef->AddProp(CSTR("--Length of the resource in the file"), CSTRP(sbuff, sptr));
						sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&nameTable[j + 4]));
						exef->AddProp(CSTR("--Flag word"), CSTRP(sbuff, sptr));
						sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ReadUInt16(&nameTable[j + 6]));
						exef->AddProp(CSTR("--Resource ID"), CSTRP(sbuff, sptr));

						UOSInt resSize = (UOSInt)ReadUInt16(&nameTable[j + 2]) << ReadUInt16(&nameTable[0]);
						Data::ByteBuffer resBuff(resSize);
						fd->GetRealData((UInt64)(ReadUInt16(&nameTable[j]) << ReadUInt16(&nameTable[0])), resSize, resBuff);

						sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("Resource 0x")), ReadUInt16(&nameTable[j + 6]));
						exef->AddResource(CSTRP(sbuff, sptr), resBuff.Ptr(), resSize, 0, rt);
						j += 12;
					}
				}
			}
		}
	}
	return exef;
}

void Parser::FileParser::EXEParser::ParseResource(IO::EXEFile *exef, UInt32 resType, UTF8Char *sbuff, UTF8Char *sbuffEnd, UInt8 *resBuff, UOSInt resOfst, UInt8 *exeImage)
{
	OSInt i;
	OSInt j;
	OSInt k;
	UInt32 v;
	UInt32 thisRType = resType;
	UTF8Char *sptr;
	i = ReadUInt16(&resBuff[resOfst + 12]);
	j = ReadUInt16(&resBuff[resOfst + 14]);
	resOfst += 16;
	k = 0;
	while (k < i)
	{
		v = ReadUInt32(&resBuff[resOfst]);
		sptr = Text::StrConcatC(sbuffEnd, UTF8STRC("Name"));

		v = ReadUInt32(&resBuff[resOfst + 4]);
		if (v & 0x80000000)
		{
			*sptr++ = '\\';
			ParseResource(exef, thisRType, sbuff, sptr, resBuff, v & 0x7fffffff, exeImage);
		}
		else
		{
			ParseResourceData(exef, thisRType, sbuff, sptr, resBuff, v, exeImage);
		}
		resOfst += 8;
		k++;
	}

	k = 0;
	while (k < j)
	{
		v = ReadUInt32(&resBuff[resOfst]);
		sptr = Text::StrUInt32(sbuffEnd, v);
		if (resType == 0)
		{
			thisRType = v;
		}
		v = ReadUInt32(&resBuff[resOfst + 4]);
		if (v & 0x80000000)
		{
			*sptr++ = '\\';
			ParseResource(exef, thisRType, sbuff, sptr, resBuff, v & 0x7fffffff, exeImage);
		}
		else
		{
			ParseResourceData(exef, thisRType, sbuff, sptr, resBuff, v, exeImage);
		}
		resOfst += 8;
		k++;
	}
}

void Parser::FileParser::EXEParser::ParseResourceData(IO::EXEFile *exef, UInt32 resType, UTF8Char *sbuff, UTF8Char *sbuffEnd, UInt8 *resBuff, UOSInt resOfst, UInt8 *exeImage)
{
	UInt32 dataRVA = ReadUInt32(&resBuff[resOfst]);
	UInt32 size = ReadUInt32(&resBuff[resOfst + 4]);
	UInt32 codePage = ReadUInt32(&resBuff[resOfst + 8]);
	IO::EXEFile::ResourceType rt = GetResourceType(resType);
	exef->AddResource(CSTRP(sbuff, sbuffEnd), &exeImage[dataRVA], size, codePage, rt);
}

IO::EXEFile::ResourceType Parser::FileParser::EXEParser::GetResourceType(UInt32 resType)
{
	switch (resType)
	{
	case 1:
		return IO::EXEFile::RT_CURSOR;
	case 2:
		return IO::EXEFile::RT_BITMAP;
	case 3:
		return IO::EXEFile::RT_ICON;
	case 4:
		return IO::EXEFile::RT_MENU;
	case 5:
		return IO::EXEFile::RT_DIALOG;
	case 6:
		return IO::EXEFile::RT_STRINGTABLE;
	case 7:
		return IO::EXEFile::RT_FONTDIR;
	case 8:
		return IO::EXEFile::RT_FONT;
	case 9:
		return IO::EXEFile::RT_ACCELERATOR;
	case 10:
		return IO::EXEFile::RT_RAW_DATA;
	case 11:
		return IO::EXEFile::RT_MESSAGETABLE;
	case 12:
		return IO::EXEFile::RT_CURSOR; //RT_GROUP_CURSOR
	case 14:
		return IO::EXEFile::RT_ICON; //RT_GROUP_ICON
	case 16:
		return IO::EXEFile::RT_VERSIONINFO;
	case 17:
		return IO::EXEFile::RT_DLGINCLUDE;
	case 19:
		return IO::EXEFile::RT_PLUGPLAY;
	case 20:
		return IO::EXEFile::RT_VXD;
	case 21:
		return IO::EXEFile::RT_ANICURSOR;
	case 22:
		return IO::EXEFile::RT_ANIICON;
	case 23:
		return IO::EXEFile::RT_HTML;
	case 24:
		return IO::EXEFile::RT_MANIFEST;
	default:
		return IO::EXEFile::RT_UNKNOWN;
	}
}
