#include "Stdafx.h"
#include "MyMemory.h"
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

void Parser::FileParser::EXEParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_EXE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.exe", (const UTF8Char*)"EXE File");
		selector->AddFilter((const UTF8Char*)"*.dll", (const UTF8Char*)"DLL File");
		selector->AddFilter((const UTF8Char*)"*.fon", (const UTF8Char*)"Font File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::EXEParser::GetParserType()
{
	return IO::ParsedObject::PT_EXE_PARSER;
}

IO::ParsedObject *Parser::FileParser::EXEParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdr[64];
	if (fd->GetRealData(0, 64, hdr) != 64)
	{
		return 0;
	}
	if (*(Int16*)&hdr[0] != *(Int16*)"MZ")
	{
		return 0;
	}
	UInt32 fileSize = *(UInt16*)&hdr[2] + (((*(UInt16*)&hdr[4]) - 1) << 9);
	if (fd->GetDataSize() < fileSize)
		return 0;
	UInt32 relocSize = *(UInt16*)&hdr[6];
	UInt32 hdrSize = (*(UInt16*)&hdr[8]) << 4;
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
	regs.CS = 0x80 + *(UInt16*)&hdr[22];
	regs.DS = 0x70;
	regs.ES = 0x70;
	regs.SS = 0x80 + *(UInt16*)&hdr[14];

	UTF8Char sbuff[256];
	UTF8Char *sptr;
	IO::EXEFile *exef;
	NEW_CLASS(exef, IO::EXEFile(fd->GetFullName()));
	OSInt codeLen;

	Text::StrInt32(sbuff, ReadUInt16(&hdr[2]));
	exef->AddProp((const UTF8Char*)"(DOS)Bytes on last page of file", sbuff);
	Text::StrInt32(sbuff, ReadUInt16(&hdr[4]));
	exef->AddProp((const UTF8Char*)"(DOS)Pages in file", sbuff);
	Text::StrInt32(sbuff, ReadUInt16(&hdr[6]));
	exef->AddProp((const UTF8Char*)"(DOS)Relocations", sbuff);
	Text::StrInt32(sbuff, ReadUInt16(&hdr[8]));
	exef->AddProp((const UTF8Char*)"(DOS)Size of header in paragraphs", sbuff);
	Text::StrInt32(sbuff, ReadUInt16(&hdr[0xa]));
	exef->AddProp((const UTF8Char*)"(DOS)Minimum extra paragraphs needed", sbuff);
	Text::StrInt32(sbuff, ReadUInt16(&hdr[0xc]));
	exef->AddProp((const UTF8Char*)"(DOS)Maximum extra paragraphs needed", sbuff);
	Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&hdr[0xe]));
	exef->AddProp((const UTF8Char*)"(DOS)Initial (relative) SS value", sbuff);
	Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&hdr[0x10]));
	exef->AddProp((const UTF8Char*)"(DOS)Initial SP value", sbuff);
	Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&hdr[0x12]));
	exef->AddProp((const UTF8Char*)"(DOS)Checksum", sbuff);
	Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&hdr[0x14]));
	exef->AddProp((const UTF8Char*)"(DOS)Initial IP value", sbuff);
	Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&hdr[0x16]));
	exef->AddProp((const UTF8Char*)"(DOS)Initial (relative) CS value", sbuff);
	Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&hdr[0x18]));
	exef->AddProp((const UTF8Char*)"(DOS)File address of relocation table", sbuff);
	Text::StrInt32(sbuff, ReadUInt16(&hdr[0x1a]));
	exef->AddProp((const UTF8Char*)"(DOS)Overlay number", sbuff);
	Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&hdr[0x24]));
	exef->AddProp((const UTF8Char*)"(DOS)OEM identifier", sbuff);
	Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&hdr[0x26]));
	exef->AddProp((const UTF8Char*)"(DOS)OEM information", sbuff);
	Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&hdr[0x3c]));
	exef->AddProp((const UTF8Char*)"(DOS)Offset to extended header", sbuff);


	exef->AddDOSEnv(fileSize - hdrSize + 256, &regs, 0x70);
	UInt8 *codePtr = exef->GetDOSCodePtr(&codeLen);
	exef->SetDOSHasPSP(true);
	fd->GetRealData(hdrSize, codeLen - 256, &codePtr[256]);

	if (relocSize > 0)
	{
		Int32 *relocTab;
		Int32 i = relocSize;
		Int32 j;
		relocTab = MemAlloc(Int32, relocSize);
		fd->GetRealData(relocOfst, relocSize << 2, (UInt8*)relocTab);
		while (i-- > 0)
		{
			j = relocTab[i];
			(*(UInt16*)&codePtr[256 + (j & 0xffff) + ((j >> 12) & 0xffff0)]) += 0x80;
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
	OSInt i = 66;
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

	Int32 peOfst = ReadInt32(&hdr[60]);
	if (peOfst >= 64 && peOfst <= 1024)
	{
		UInt8 peBuff[24];
		fd->GetRealData(peOfst, 24, peBuff);
		if (*(Int32*)&peBuff[0] == *(Int32*)"PE\0")
		{
			UInt32 machine = ReadUInt16(&peBuff[4]);
			UInt32 numberOfSections = ReadUInt16(&peBuff[6]);
			Int32 timeDateStamp = ReadInt32(&peBuff[8]);
			Int32 pointerToSymbolTable = ReadInt32(&peBuff[12]);
			Int32 numberOfSymbols = ReadInt32(&peBuff[16]);
			UInt32 sizeOfOptionalHeader = ReadUInt16(&peBuff[20]);
			UInt32 characteristics = ReadUInt16(&peBuff[22]);

			exef->AddProp((const UTF8Char*)"Extended Header Format", (const UTF8Char*)"Portable Executable");
			sptr = Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), machine);
			switch (machine)
			{
			case 0x1d3:
				Text::StrConcat(sptr, (const UTF8Char*)" Matsushita AM33");
				break;
			case 0x8664:
				Text::StrConcat(sptr, (const UTF8Char*)" x64");
				break;
			case 0x1c0:
				Text::StrConcat(sptr, (const UTF8Char*)" ARM little endian");
				break;
			case 0x1c4:
				Text::StrConcat(sptr, (const UTF8Char*)" ARMv7 (or higher) Thumb mode only");
				break;
			case 0xaa64:
				Text::StrConcat(sptr, (const UTF8Char*)" ARMv8 in 64-bit mode");
				break;
			case 0xebc:
				Text::StrConcat(sptr, (const UTF8Char*)" EFI byte code");
				break;
			case 0x14c:
				Text::StrConcat(sptr, (const UTF8Char*)" i386");
				break;
			case 0x200:
				Text::StrConcat(sptr, (const UTF8Char*)" Intel Itanium");
				break;
			case 0x9041:
				Text::StrConcat(sptr, (const UTF8Char*)" Mitsubishi M32R little endian");
				break;
			case 0x266:
				Text::StrConcat(sptr, (const UTF8Char*)" MIPS16");
				break;
			case 0x366:
				Text::StrConcat(sptr, (const UTF8Char*)" MIPS with FPU");
				break;
			case 0x466:
				Text::StrConcat(sptr, (const UTF8Char*)" MIPS16 with FPU");
				break;
			case 0x1f0:
				Text::StrConcat(sptr, (const UTF8Char*)" Power PC little endian");
				break;
			case 0x1f1:
				Text::StrConcat(sptr, (const UTF8Char*)" Power PC with floating point support");
				break;
			case 0x166:
				Text::StrConcat(sptr, (const UTF8Char*)" MIPS little endian");
				break;
			case 0x1a2:
				Text::StrConcat(sptr, (const UTF8Char*)" Hitachi SH3");
				break;
			case 0x1a3:
				Text::StrConcat(sptr, (const UTF8Char*)" Hitachi SH3 DSP");
				break;
			case 0x1a6:
				Text::StrConcat(sptr, (const UTF8Char*)" Hitachi SH4");
				break;
			case 0x1a8:
				Text::StrConcat(sptr, (const UTF8Char*)" Hitachi SH5");
				break;
			case 0x1c2:
				Text::StrConcat(sptr, (const UTF8Char*)" ARM or Thumb");
				break;
			case 0x169:
				Text::StrConcat(sptr, (const UTF8Char*)" MIPS little-endian WCE v2");
				break;
			}
			exef->AddProp((const UTF8Char*)"Machine", sbuff);
			Text::StrInt32(sbuff, numberOfSections);
			exef->AddProp((const UTF8Char*)"Number Of Sections", sbuff);
			Data::DateTime dt;
			dt.SetUnixTimestamp((UInt32)timeDateStamp);
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzz");
			exef->AddProp((const UTF8Char*)"File Time", sbuff);
			Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), pointerToSymbolTable);
			exef->AddProp((const UTF8Char*)"Pointer to symbol table", sbuff);
			Text::StrInt32(sbuff, numberOfSymbols);
			exef->AddProp((const UTF8Char*)"Number Of Symbools", sbuff);
			Text::StrInt32(sbuff, sizeOfOptionalHeader);
			exef->AddProp((const UTF8Char*)"Size Of Optional Header", sbuff);
			Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), characteristics);
			exef->AddProp((const UTF8Char*)"Characteristics", sbuff);

			if (sizeOfOptionalHeader >= 96)
			{
				UInt8 *optionalHdr = MemAlloc(UInt8, sizeOfOptionalHeader);
				if (fd->GetRealData(peOfst + 24, sizeOfOptionalHeader, optionalHdr) == sizeOfOptionalHeader)
				{
					Int32 ofst;
					Int32 rvaSize;
					Int32 magic = ReadUInt16(&optionalHdr[0]);
					sptr = Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), magic);
					switch (magic)
					{
					case 0x10b:
						Text::StrConcat(sptr, (const UTF8Char*)" PE32");
						break;
					case 0x20b:
						Text::StrConcat(sptr, (const UTF8Char*)" PE32+");
						break;
					case 0x107:
						Text::StrConcat(sptr, (const UTF8Char*)" ROM image");
						break;
					}
					exef->AddProp((const UTF8Char*)"Magic", sbuff);
					Text::StrInt32(sbuff, optionalHdr[2]);
					if (magic == 0x10b || magic == 0x20b)
					{
						Int32 sizeOfImage;
						UInt8 *exeImage;

						exef->AddProp((const UTF8Char*)"Major Linker Version", sbuff);
						Text::StrInt32(sbuff, optionalHdr[3]);
						exef->AddProp((const UTF8Char*)"Minor Linker Version", sbuff);
						Text::StrInt32(sbuff, ReadInt32(&optionalHdr[4]));
						exef->AddProp((const UTF8Char*)"Size Of Code", sbuff);
						Text::StrInt32(sbuff, ReadInt32(&optionalHdr[8]));
						exef->AddProp((const UTF8Char*)"Size Of Initialized Data", sbuff);
						Text::StrInt32(sbuff, ReadInt32(&optionalHdr[12]));
						exef->AddProp((const UTF8Char*)"Size Of Uninitialized Data", sbuff);
						Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[16]));
						exef->AddProp((const UTF8Char*)"Address Of Entry Point", sbuff);
						Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[20]));
						exef->AddProp((const UTF8Char*)"Base Of Code", sbuff);
						if (magic == 0x10b)
						{
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[24]));
							exef->AddProp((const UTF8Char*)"Base Of Data", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[28]));
							exef->AddProp((const UTF8Char*)"Image Base", sbuff);
						}
						else
						{
							Text::StrHexVal64(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt64(&optionalHdr[24]));
							exef->AddProp((const UTF8Char*)"Image Base", sbuff);
						}
						Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[32]));
						exef->AddProp((const UTF8Char*)"Section Alignment", sbuff);
						Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[36]));
						exef->AddProp((const UTF8Char*)"File Alignment", sbuff);
						Text::StrInt32(sbuff, ReadUInt16(&optionalHdr[40]));
						exef->AddProp((const UTF8Char*)"Major Operating System Version", sbuff);
						Text::StrInt32(sbuff, ReadUInt16(&optionalHdr[42]));
						exef->AddProp((const UTF8Char*)"Minor Operating System Version", sbuff);
						Text::StrInt32(sbuff, ReadUInt16(&optionalHdr[44]));
						exef->AddProp((const UTF8Char*)"Major Image Version", sbuff);
						Text::StrInt32(sbuff, ReadUInt16(&optionalHdr[46]));
						exef->AddProp((const UTF8Char*)"Minor Image Version", sbuff);
						Text::StrInt32(sbuff, ReadUInt16(&optionalHdr[48]));
						exef->AddProp((const UTF8Char*)"Major Subsystem Version", sbuff);
						Text::StrInt32(sbuff, ReadUInt16(&optionalHdr[50]));
						exef->AddProp((const UTF8Char*)"Minor Subsystem Version", sbuff);
						Text::StrInt32(sbuff, ReadInt32(&optionalHdr[52]));
						exef->AddProp((const UTF8Char*)"Win32 Version Value", sbuff);
						Text::StrInt32(sbuff, sizeOfImage = ReadInt32(&optionalHdr[56]));
						exef->AddProp((const UTF8Char*)"Size Of Image", sbuff);
						Text::StrInt32(sbuff, ReadInt32(&optionalHdr[60]));
						exef->AddProp((const UTF8Char*)"Size Of Headers", sbuff);
						Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[64]));
						exef->AddProp((const UTF8Char*)"Check Sum", sbuff);
						Int32 subsys = ReadUInt16(&optionalHdr[68]);
						sptr = Text::StrInt32(sbuff, subsys);
						switch (subsys)
						{
						case 0:
							Text::StrConcat(sptr, (const UTF8Char*)" Unknown");
							break;
						case 1:
							Text::StrConcat(sptr, (const UTF8Char*)" Native/Device Driver");
							break;
						case 2:
							Text::StrConcat(sptr, (const UTF8Char*)" Windows GUI");
							break;
						case 3:
							Text::StrConcat(sptr, (const UTF8Char*)" Console");
							break;
						case 7:
							Text::StrConcat(sptr, (const UTF8Char*)" POSIX");
							break;
						case 9:
							Text::StrConcat(sptr, (const UTF8Char*)" Windows CE");
							break;
						case 10:
							Text::StrConcat(sptr, (const UTF8Char*)" EFI application");
							break;
						case 11:
							Text::StrConcat(sptr, (const UTF8Char*)" EFI driver with boot services");
							break;
						case 12:
							Text::StrConcat(sptr, (const UTF8Char*)" EFI driver with run-time services");
							break;
						case 13:
							Text::StrConcat(sptr, (const UTF8Char*)" EFI ROM image");
							break;
						case 14:
							Text::StrConcat(sptr, (const UTF8Char*)" XBOX");
							break;
						}
						exef->AddProp((const UTF8Char*)"Subsystem", sbuff);
						Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt16(&optionalHdr[70]));
						exef->AddProp((const UTF8Char*)"DLL Characteristics", sbuff);
						if (magic == 0x10b)
						{
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[72]));
							exef->AddProp((const UTF8Char*)"Size Of Stack Reserve", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[76]));
							exef->AddProp((const UTF8Char*)"Size Of Stack Commit", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[80]));
							exef->AddProp((const UTF8Char*)"Size Of Heap Reserve", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[84]));
							exef->AddProp((const UTF8Char*)"Size Of Heap Commit", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[88]));
							exef->AddProp((const UTF8Char*)"Loader Flags", sbuff);
							Text::StrInt32(sbuff, rvaSize = ReadInt32(&optionalHdr[92]));
							exef->AddProp((const UTF8Char*)"Number Of Rva And Sizes", sbuff);
							ofst = 96;
						}
						else
						{
							Text::StrInt64(sbuff, ReadInt64(&optionalHdr[72]));
							exef->AddProp((const UTF8Char*)"Size Of Stack Reserve", sbuff);
							Text::StrInt64(sbuff, ReadInt64(&optionalHdr[80]));
							exef->AddProp((const UTF8Char*)"Size Of Stack Commit", sbuff);
							Text::StrInt64(sbuff, ReadInt64(&optionalHdr[88]));
							exef->AddProp((const UTF8Char*)"Size Of Heap Reserve", sbuff);
							Text::StrInt64(sbuff, ReadInt64(&optionalHdr[96]));
							exef->AddProp((const UTF8Char*)"Size Of Heap Commit", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[104]));
							exef->AddProp((const UTF8Char*)"Loader Flags", sbuff);
							Text::StrInt32(sbuff, rvaSize = ReadInt32(&optionalHdr[108]));
							exef->AddProp((const UTF8Char*)"Number Of Rva And Sizes", sbuff);
							ofst = 112;
						}
						if (rvaSize >= 16)
						{
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 0]));
							exef->AddProp((const UTF8Char*)"Export Table Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 4]));
							exef->AddProp((const UTF8Char*)"Export Table Size", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 8]));
							exef->AddProp((const UTF8Char*)"Import Table Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 12]));
							exef->AddProp((const UTF8Char*)"Import Table Size", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 16]));
							exef->AddProp((const UTF8Char*)"Resource Table Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 20]));
							exef->AddProp((const UTF8Char*)"Resource Table Size", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 24]));
							exef->AddProp((const UTF8Char*)"Exception Table Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 28]));
							exef->AddProp((const UTF8Char*)"Exception Table Size", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 32]));
							exef->AddProp((const UTF8Char*)"Certificate Table Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 36]));
							exef->AddProp((const UTF8Char*)"Certificate Table Size", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 40]));
							exef->AddProp((const UTF8Char*)"Base Relocation Table Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 44]));
							exef->AddProp((const UTF8Char*)"Base Relocation Table Size", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 48]));
							exef->AddProp((const UTF8Char*)"Debug Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 52]));
							exef->AddProp((const UTF8Char*)"Debug Size", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 64]));
							exef->AddProp((const UTF8Char*)"Global Ptr. Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 68]));
							exef->AddProp((const UTF8Char*)"Global Ptr. Size", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 72]));
							exef->AddProp((const UTF8Char*)"TLS Table Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 76]));
							exef->AddProp((const UTF8Char*)"TLS Table Size", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 80]));
							exef->AddProp((const UTF8Char*)"Load Config Table Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 84]));
							exef->AddProp((const UTF8Char*)"Load Config Table Size", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 88]));
							exef->AddProp((const UTF8Char*)"Bound Import Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 92]));
							exef->AddProp((const UTF8Char*)"Bound Import Size", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 96]));
							exef->AddProp((const UTF8Char*)"IAT Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 100]));
							exef->AddProp((const UTF8Char*)"IAT Size", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 104]));
							exef->AddProp((const UTF8Char*)"Delay Import Descriptor Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 108]));
							exef->AddProp((const UTF8Char*)"Delay Import Descriptor Size", sbuff);
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&optionalHdr[ofst + 112]));
							exef->AddProp((const UTF8Char*)"CLR Runtime Header Address", sbuff);
							Text::StrInt32(sbuff, ReadInt32(&optionalHdr[ofst + 116]));
							exef->AddProp((const UTF8Char*)"CLR Runtime Size", sbuff);
						}

						Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), sizeOfOptionalHeader + peOfst + 24);
						exef->AddProp((const UTF8Char*)"End Of Optional Header", sbuff);

						UInt32 rva;
//						UInt32 tabSize;
						Int32 sOfst;
						UOSInt i;
						Text::StringBuilderUTF8 sb;
						UInt8 *sectionHeaders = MemAlloc(UInt8, numberOfSections * 40);
						exeImage = MemAlloc(UInt8, sizeOfImage);
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
							sb.Append((const UTF8Char*)"Section ");
							sb.AppendOSInt(i);
							sb.Append((const UTF8Char*)" Name");
							Text::StrConcatC(sbuff, &sectionHeaders[sOfst], 8);
							exef->AddProp(sb.ToString(), sbuff);

							if (virtSize < dataSize)
							{
								readSize = virtSize;
							}
							else
							{
								readSize = dataSize;
							}

							fd->GetRealData(dataAddr, readSize, &exeImage[virtAddr]);
							if (Text::StrEquals(sbuff, (const UTF8Char*)".text"))
							{
							}
							else if (Text::StrEquals(sbuff, (const UTF8Char*)".rdata"))
							{
							}
							else if (Text::StrEquals(sbuff, (const UTF8Char*)".data"))
							{
							}
							else if (Text::StrEquals(sbuff, (const UTF8Char*)".pdata"))
							{
							}
							else if (Text::StrEquals(sbuff, (const UTF8Char*)".rsrc"))
							{
								ParseResource(exef, 0, sbuff, sbuff, &exeImage[virtAddr], 0, exeImage);
								sbuff[0] = 0;
							}
							else if (Text::StrEquals(sbuff, (const UTF8Char*)".reloc"))
							{
							}

							sb.ClearStr();
							sb.Append((const UTF8Char*)"Section ");
							sb.AppendOSInt(i);
							sb.Append((const UTF8Char*)" Virtual Size");
							Text::StrInt32(sbuff, virtSize);
							exef->AddProp(sb.ToString(), sbuff);

							sb.ClearStr();
							sb.Append((const UTF8Char*)"Section ");
							sb.AppendOSInt(i);
							sb.Append((const UTF8Char*)" Virtual Address");
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), virtAddr);
							exef->AddProp(sb.ToString(), sbuff);

							sb.ClearStr();
							sb.Append((const UTF8Char*)"Section ");
							sb.AppendOSInt(i);
							sb.Append((const UTF8Char*)" Size Of Raw Data");
							Text::StrInt32(sbuff, dataSize);
							exef->AddProp(sb.ToString(), sbuff);

							sb.ClearStr();
							sb.Append((const UTF8Char*)"Section ");
							sb.AppendOSInt(i);
							sb.Append((const UTF8Char*)" Pointer To Raw Data");
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), dataAddr);
							exef->AddProp(sb.ToString(), sbuff);

							sb.ClearStr();
							sb.Append((const UTF8Char*)"Section ");
							sb.AppendOSInt(i);
							sb.Append((const UTF8Char*)" Pointer To Relocations");
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&sectionHeaders[sOfst + 24]));
							exef->AddProp(sb.ToString(), sbuff);

							sb.ClearStr();
							sb.Append((const UTF8Char*)"Section ");
							sb.AppendOSInt(i);
							sb.Append((const UTF8Char*)" Pointer To Line numbers");
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&sectionHeaders[sOfst + 28]));
							exef->AddProp(sb.ToString(), sbuff);

							sb.ClearStr();
							sb.Append((const UTF8Char*)"Section ");
							sb.AppendOSInt(i);
							sb.Append((const UTF8Char*)" Number Of Relocations");
							Text::StrInt32(sbuff, ReadUInt16(&sectionHeaders[sOfst + 32]));
							exef->AddProp(sb.ToString(), sbuff);

							sb.ClearStr();
							sb.Append((const UTF8Char*)"Section ");
							sb.AppendOSInt(i);
							sb.Append((const UTF8Char*)" Number Of Line numbers");
							Text::StrInt32(sbuff, ReadUInt16(&sectionHeaders[sOfst + 34]));
							exef->AddProp(sb.ToString(), sbuff);

							sb.ClearStr();
							sb.Append((const UTF8Char*)"Section ");
							sb.AppendOSInt(i);
							sb.Append((const UTF8Char*)" Characteristics");
							Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&sectionHeaders[sOfst + 36]));
							exef->AddProp(sb.ToString(), sbuff);

							if (dataSize > 0)
							{
								fd->GetRealData(dataAddr, dataSize, &exeImage[virtAddr]);
							}
							if (virtSize > dataSize)
							{
								MemClear(&exeImage[dataSize + virtAddr], virtSize - dataSize);
							}

							sOfst += 40;
						}

						rva = ReadInt32(&optionalHdr[ofst + 8]);
//						tabSize = ReadInt32(&optionalHdr[ofst + 12]);
						if (rva != 0)
						{
							Int32 ilut;
							Int32 nameRVA;
							OSInt j;
							while (true)
							{
								ilut = ReadInt32(&exeImage[rva + 0]);
								nameRVA = ReadInt32(&exeImage[rva + 12]);
								if (nameRVA == 0 || ilut == 0)
								{
									break;
								}
								Text::StrConcatC(sbuff, &exeImage[nameRVA], 64);
								j = exef->AddImportModule(sbuff);
								if (magic == 0x10b)
								{
									Int32 funcRVA;
									while (true)
									{
										funcRVA = ReadInt32(&exeImage[ilut]);
										if (funcRVA == 0)
											break;
										if (funcRVA & 0x80000000)
										{
											Text::StrConcat(Text::StrInt32(Text::StrConcat(sbuff, (const UTF8Char*)"(by ordinal "), funcRVA & 0x7fff), (const UTF8Char*)")");
											exef->AddImportFunc(j, sbuff);
										}
										else
										{
											Text::StrConcatC(sbuff, &exeImage[funcRVA + 2], 64);
											exef->AddImportFunc(j, sbuff);
										}
										ilut += 4;
									}
								}
								else if (magic == 0x20b)
								{
									Int64 funcRVA;
									while (true)
									{
										funcRVA = ReadInt64(&exeImage[ilut]);
										if (funcRVA == 0)
											break;
										if (funcRVA & 0x8000000000000000LL)
										{
											Text::StrConcat(Text::StrInt32(Text::StrConcat(sbuff, (const UTF8Char*)"(by ordinal "), (Int32)(funcRVA & 0x7fff)), (const UTF8Char*)")");
											exef->AddImportFunc(j, sbuff);
										}
										else
										{
											Text::StrConcatC(sbuff, &exeImage[(funcRVA + 2) & 0x7fffffff], 64);
											exef->AddImportFunc(j, sbuff);
										}
										ilut += 8;
									}
								}

								rva += 20;
							}
						}

						rva = ReadInt32(&optionalHdr[ofst + 0]);
//						tabSize = ReadInt32(&optionalHdr[ofst + 4]);
						if (rva != 0)
						{
							Int32 nameRVA;
							Int32 namePtrRVA;
							Int32 nName;
							nameRVA = ReadInt32(&exeImage[rva + 12]);
							if (nameRVA != 0)
							{
								Text::StrConcatC(sbuff, &exeImage[nameRVA], 64);
								nName = ReadInt32(&exeImage[rva + 24]);
								namePtrRVA = ReadInt32(&exeImage[rva + 32]);
								while (nName-- > 0)
								{
									rva = ReadInt32(&exeImage[namePtrRVA]);
									if (rva == 0)
										break;
									Text::StrConcatC(sbuff, &exeImage[rva], 64);
									exef->AddExportFunc(sbuff);
									namePtrRVA += 4;
								}
							}
						}

						MemFree(sectionHeaders);
						MemFree(exeImage);
					}
				}
				MemFree(optionalHdr);
			}
		}
		else if (*(Int16*)&peBuff[0] == *(Int16*)"NE")
		{
			UInt8 neBuff[64];
			MemCopyNO(neBuff, peBuff, 24);
			fd->GetRealData(peOfst + 24, 40, &neBuff[24]);

			exef->AddProp((const UTF8Char*)"Extended Header Format", (const UTF8Char*)"New Executable");
			Text::StrInt32(sbuff, neBuff[2]);
			exef->AddProp((const UTF8Char*)"Version of this linker", sbuff);
			Text::StrInt32(sbuff, neBuff[3]);
			exef->AddProp((const UTF8Char*)"Revision of this linker", sbuff);
			Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&neBuff[4]));
			exef->AddProp((const UTF8Char*)"Entry table file offset", sbuff);
			Text::StrInt32(sbuff, ReadUInt16(&neBuff[6]));
			exef->AddProp((const UTF8Char*)"Number of bytes in the entry table", sbuff);
			Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadInt32(&neBuff[8]));
			exef->AddProp((const UTF8Char*)"32-bit CRC of entire contents of file", sbuff);
			Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&neBuff[12]));
			exef->AddProp((const UTF8Char*)"Flag word", sbuff);
			Text::StrInt32(sbuff, ReadUInt16(&neBuff[14]));
			exef->AddProp((const UTF8Char*)"Segment number of automatic data segment", sbuff);
			Text::StrInt32(sbuff, ReadUInt16(&neBuff[16]));
			exef->AddProp((const UTF8Char*)"Initial size, in bytes, of dynamic heap added to the data segment", sbuff);
			Text::StrInt32(sbuff, ReadUInt16(&neBuff[18]));
			exef->AddProp((const UTF8Char*)"Initial size, in bytes, of stack added to the data segment", sbuff);
			Text::StrHexVal16(Text::StrConcat(Text::StrHexVal16(sbuff, ReadInt16(&neBuff[22])), (const UTF8Char*)":"), ReadInt16(&neBuff[20]));
			exef->AddProp((const UTF8Char*)"CS:IP", sbuff);
			Text::StrHexVal16(Text::StrConcat(Text::StrHexVal16(sbuff, ReadInt16(&neBuff[26])), (const UTF8Char*)":"), ReadInt16(&neBuff[24]));
			exef->AddProp((const UTF8Char*)"SS:SP", sbuff);
			Text::StrInt32(sbuff, ReadUInt16(&neBuff[28]));
			exef->AddProp((const UTF8Char*)"Number of entries in the Segment Table", sbuff);
			Text::StrInt32(sbuff, ReadUInt16(&neBuff[30]));
			exef->AddProp((const UTF8Char*)"Number of entries in the Module Reference Table", sbuff);
			Text::StrInt32(sbuff, ReadUInt16(&neBuff[32]));
			exef->AddProp((const UTF8Char*)"Number of bytes in the Non-Resident Name Table", sbuff);
			Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&neBuff[34]));
			exef->AddProp((const UTF8Char*)"Segment Table file offset", sbuff);
			Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&neBuff[36]));
			exef->AddProp((const UTF8Char*)"Resource Table file offset", sbuff);
			Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&neBuff[38]));
			exef->AddProp((const UTF8Char*)"Resident Name Table file offset", sbuff);
			Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&neBuff[40]));
			exef->AddProp((const UTF8Char*)"Module Reference Table file offset", sbuff);
			Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&neBuff[42]));
			exef->AddProp((const UTF8Char*)"Imported Names Table file offset", sbuff);
			Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt32(&neBuff[44]));
			exef->AddProp((const UTF8Char*)"Non-Resident Name Table offset", sbuff);
			Text::StrInt32(sbuff, ReadUInt16(&neBuff[48]));
			exef->AddProp((const UTF8Char*)"Number of movable entries in the Entry Table", sbuff);
			Text::StrInt32(sbuff, ReadUInt16(&neBuff[50]));
			exef->AddProp((const UTF8Char*)"Logical sector alignment shift count", sbuff);
			Text::StrInt32(sbuff, ReadUInt16(&neBuff[52]));
			exef->AddProp((const UTF8Char*)"Number of resource entries", sbuff);
			Text::StrInt32(sbuff, neBuff[54]);
			exef->AddProp((const UTF8Char*)"Executable type", sbuff);

			OSInt tableStart;
			OSInt tableSize;
			UInt8 *nameTable;
			OSInt j;
			tableStart = ReadUInt16(&neBuff[38]);
			tableSize = ReadUInt16(&neBuff[40]) - tableStart;
			tableStart += peOfst;
			if (tableSize > 0)
			{
				nameTable = MemAlloc(UInt8, tableSize);
				fd->GetRealData(tableStart, tableSize, nameTable);
				i = 0;
				j = 0;
				while (j < tableSize && nameTable[j] != 0)
				{
					Text::StrConcatC(sbuff, &nameTable[j + 1], nameTable[j]);
					exef->AddProp((const UTF8Char*)"ResidentName", sbuff);

					i++;
					j += nameTable[j] + 1;
				}
				MemFree(nameTable);
			}

			tableStart = ReadUInt32(&neBuff[44]);
			tableSize = ReadUInt16(&neBuff[32]);
			if (tableSize > 0)
			{
				nameTable = MemAlloc(UInt8, tableSize);
				fd->GetRealData(tableStart, tableSize, nameTable);
				i = 0;
				j = 0;
				while (j < tableSize && nameTable[j] != 0)
				{
					Text::StrConcatC(sbuff, &nameTable[j + 1], nameTable[j]);
					exef->AddProp((const UTF8Char*)"NonResidentName", sbuff);

					i++;
					j += nameTable[j] + 1;
				}
				MemFree(nameTable);
			}

			tableStart = ReadUInt16(&neBuff[34]);
			tableSize = ReadUInt16(&neBuff[38]) - tableStart;
			tableStart += peOfst;
			if (tableSize > 0)
			{
				nameTable = MemAlloc(UInt8, tableSize);
				fd->GetRealData(tableStart, tableSize, nameTable);
				Text::StrInt32(sbuff, ReadUInt16(&nameTable[0]));
				exef->AddProp((const UTF8Char*)"Resource Table: Alignment shift count", sbuff);
				j = 2;
				while (j < tableSize)
				{
					if (ReadUInt16(&nameTable[j]) == 0)
					{
						j += 2;
						break;
					}
					IO::EXEFile::ResourceType rt = GetResourceType(ReadUInt16(&nameTable[j]) & 0x7fff);
					Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&nameTable[j]));
					exef->AddProp((const UTF8Char*)"-Type ID", sbuff);
					i = ReadUInt16(&nameTable[j + 2]);
					Text::StrOSInt(sbuff, ReadUInt16(&nameTable[j + 2]));
					exef->AddProp((const UTF8Char*)"-Number of resources for this type", sbuff);
					j += 8;
					while (j < tableSize && i-- > 0)
					{
						Text::StrConcat(Text::StrHexVal32(Text::StrConcat(Text::StrInt32(sbuff, ReadUInt16(&nameTable[j])), (const UTF8Char*)" (0x"), (ReadUInt16(&nameTable[j]) << ReadUInt16(&nameTable[0]))), (const UTF8Char*)")");
						exef->AddProp((const UTF8Char*)"--File offset to the contents of the resource data", sbuff);
						Text::StrInt32(sbuff, ReadUInt16(&nameTable[j + 2]));
						exef->AddProp((const UTF8Char*)"--Length of the resource in the file", sbuff);
						Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&nameTable[j + 4]));
						exef->AddProp((const UTF8Char*)"--Flag word", sbuff);
						Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), ReadUInt16(&nameTable[j + 6]));
						exef->AddProp((const UTF8Char*)"--Resource ID", sbuff);

						UInt8 *resBuff;
						OSInt resSize = ReadUInt16(&nameTable[j + 2]) << ReadUInt16(&nameTable[0]);
						resBuff = MemAlloc(UInt8, resSize);
						fd->GetRealData((ReadUInt16(&nameTable[j]) << ReadUInt16(&nameTable[0])), resSize, resBuff);

						Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"Resource 0x"), ReadUInt16(&nameTable[j + 6]));
						exef->AddResource(sbuff, resBuff, resSize, 0, rt);
						MemFree(resBuff);
						j += 12;
					}
				}
				MemFree(nameTable);
			}
		}
	}
	return exef;
}

void Parser::FileParser::EXEParser::ParseResource(IO::EXEFile *exef, UInt32 resType, UTF8Char *sbuff, UTF8Char *sbuffEnd, UInt8 *resBuff, OSInt resOfst, UInt8 *exeImage)
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
		sptr = Text::StrConcat(sbuffEnd, (const UTF8Char*)"Name");

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

void Parser::FileParser::EXEParser::ParseResourceData(IO::EXEFile *exef, UInt32 resType, UTF8Char *sbuff, UTF8Char *sbuffEnd, UInt8 *resBuff, OSInt resOfst, UInt8 *exeImage)
{
	UInt32 dataRVA = ReadUInt32(&resBuff[resOfst]);
	UInt32 size = ReadUInt32(&resBuff[resOfst + 4]);
	UInt32 codePage = ReadUInt32(&resBuff[resOfst + 8]);
	IO::EXEFile::ResourceType rt = GetResourceType(resType);
	exef->AddResource(sbuff, &exeImage[dataRVA], size, codePage, rt);
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
