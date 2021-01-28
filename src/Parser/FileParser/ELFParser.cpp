#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/EXEFile.h"
#include "Parser/FileParser/ELFParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Int64 __stdcall Parser::FileParser::ELFParser::TReadInt64(UInt8 *pVal)
{
	return ReadInt64(pVal);
}

Int64 __stdcall Parser::FileParser::ELFParser::TReadMInt64(UInt8 *pVal)
{
	return ReadMInt64(pVal);
}

Int32 __stdcall Parser::FileParser::ELFParser::TReadInt32(UInt8 *pVal)
{
	return ReadInt32(pVal);
}

Int32 __stdcall Parser::FileParser::ELFParser::TReadMInt32(UInt8 *pVal)
{
	return ReadMInt32(pVal);
}

Int16 __stdcall Parser::FileParser::ELFParser::TReadInt16(UInt8 *pVal)
{
	return ReadInt16(pVal);
}

Int16 __stdcall Parser::FileParser::ELFParser::TReadMInt16(UInt8 *pVal)
{
	return ReadMInt16(pVal);
}

/*
CPP Syntax:
c = char
v = void
w = wchar_t
i = int
x = __int64
h = unsigned char
t = unsigned short
j = unsigned int
y = unsigned __int64
d = double
P = *
K = const
l = start of template
N = Start of namespace
E = End of namespace/template
CnE = Constructor
DnE = Destructor
*/

UTF8Char *Parser::FileParser::ELFParser::ToFuncName(UTF8Char *sbuff, const UTF8Char *funcName)
{
	if (funcName[0] == '_' && funcName[1] == 'Z')
	{
		UTF8Char *sbuff2 = sbuff;
		UTF8Char *sbuff3 = sbuff2;
		Bool valid = true;
		Bool foundName = false;
		Bool hasParam = false;
		Bool isConst = false;
		OSInt ptCnt = 0;
		UTF8Char c;
		Int32 i;
		const UTF8Char *sptr = funcName + 2;
		while (true)
		{
			c = *sptr++;
			if (c == 0)
			{
				break;
			}
			else if (c >= '1' && c <= '9')
			{
				i = c - '0';
				if (foundName)
				{
					valid = false;
					break;
				}
				else
				{
					foundName = true;
				}
				c = *sptr++;
				if (c >= '0' && c <= '9')
				{
					i = i * 10 + c - '0';
				}
				else
				{
					*sbuff2++ = c;
					i--;
				}
				while (i-- > 0)
				{
					c = *sptr++;
					if (c == 0)
					{
						valid = false;
						break;
					}
					*sbuff2++ = c;
				}
				if (!valid)
				{
					break;
				}
				*sbuff2++ = '(';
			}
			else if (c == 'N')
			{
				if (foundName)
				{
					if (hasParam)
					{
						*sbuff2++ = ',';
						*sbuff2++ = ' ';
					}
					if (isConst)
					{
						sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"const ");
						isConst = false;
					}
					foundName = false;
					while (true)
					{
						c = *sptr++;
						if (c >= '1' && c <= '9')
						{
							i = c - '0';
							if (foundName)
							{
								*sbuff2++ = '.';
							}
							else
							{
								foundName = true;
							}
							sbuff3 = sbuff2;
							c = *sptr++;
							if (c >= '0' && c <= '9')
							{
								i = i * 10 + c - '0';
							}
							else
							{
								*sbuff2++ = c;
								i--;
							}
							while (i-- > 0)
							{
								c = *sptr++;
								if (c == 0)
								{
									valid = false;
									break;
								}
								*sbuff2++ = c;
							}
							if (!valid)
							{
								break;
							}
						}
						else if (c == 'C')
						{
							c = *sptr++;
							if (c == '0' || c == '1' || c == '2')
							{
								*sbuff2 = 0;
								sbuff3 = Text::StrConcat(sbuff2 + 1, sbuff3);
								*sbuff2 = '.';
								sbuff2 = sbuff3;
								c = *sptr++;
								if (c != 'E')
								{
									valid = false;
									break;
								}
								break;
							}
							else
							{
								valid = false;
								break;
							}
						}
						else if (c == 'D')
						{
							c = *sptr++;
							if (c == '0' || c == '1' || c == '2')
							{
								*sbuff2 = 0;
								sbuff3 = Text::StrConcat(sbuff2 + 2, sbuff3);
								sbuff2[0] = '.';
								sbuff2[1] = '~';
								sbuff2 = sbuff3;
								c = *sptr++;
								if (c != 'E')
								{
									valid = false;
									break;
								}
								break;
							}
							else
							{
								valid = false;
								break;
							}
						}
						else if (c == 'E')
						{
							break;
						}
						else
						{
							valid = false;
							break;
						}
					}
					if (!valid)
					{
						break;
					}
					while (ptCnt-- > 0)
					{
						*sbuff2++ = '*';
					}
					ptCnt = 0;
					hasParam = true;
				}
				else
				{
					while (true)
					{
						c = *sptr++;
						if (c >= '1' && c <= '9')
						{
							i = c - '0';
							if (foundName)
							{
								*sbuff2++ = '.';
							}
							else
							{
								foundName = true;
							}
							sbuff3 = sbuff2;
							c = *sptr++;
							if (c >= '0' && c <= '9')
							{
								i = i * 10 + c - '0';
							}
							else
							{
								*sbuff2++ = c;
								i--;
							}
							while (i-- > 0)
							{
								c = *sptr++;
								if (c == 0)
								{
									valid = false;
									break;
								}
								*sbuff2++ = c;
							}
							if (!valid)
							{
								break;
							}
						}
						else if (c == 'C')
						{
							c = *sptr++;
							if (c == '0' || c == '1' || c == '2')
							{
								*sbuff2 = 0;
								sbuff3 = Text::StrConcat(sbuff2 + 1, sbuff3);
								*sbuff2 = '.';
								sbuff2 = sbuff3;
								c = *sptr++;
								if (c != 'E')
								{
									valid = false;
									break;
								}
								*sbuff2++ = '(';
								break;
							}
							else
							{
								valid = false;
								break;
							}
						}
						else if (c == 'D')
						{
							c = *sptr++;
							if (c == '0' || c == '1' || c == '2')
							{
								*sbuff2 = 0;
								sbuff3 = Text::StrConcat(sbuff2 + 2, sbuff3);
								sbuff2[0] = '.';
								sbuff2[1] = '~';
								sbuff2 = sbuff3;
								c = *sptr++;
								if (c != 'E')
								{
									valid = false;
									break;
								}
								*sbuff2++ = '(';
								break;
							}
							else
							{
								valid = false;
								break;
							}
						}
						else if (c == 'E')
						{
							*sbuff2++ = '(';
							break;
						}
						else
						{
							valid = false;
							break;
						}
					}
				}
			}
			else if (!foundName)
			{
				valid = false;
				break;
			}
			else if (c == 'P')
			{
				ptCnt++;
			}
			else if (c == 'K')
			{
				isConst = true;
			}
			else if (c == 'v')
			{
				if (hasParam)
				{
					valid = false;
					break;
				}
				if (isConst)
				{
					sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"const ");
					isConst = false;
				}
				sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"void");
				while (ptCnt-- > 0)
				{
					*sbuff2++ = '*';
				}
				ptCnt = 0;
				hasParam = true;
			}
			else if (c == 'i')
			{
				if (hasParam)
				{
					*sbuff2++ = ',';
					*sbuff2++ = ' ';
				}
				if (isConst)
				{
					sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"const ");
					isConst = false;
				}
				sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"int");
				while (ptCnt-- > 0)
				{
					*sbuff2++ = '*';
				}
				ptCnt = 0;
				hasParam = true;
			}
			else if (c == 'c')
			{
				if (hasParam)
				{
					*sbuff2++ = ',';
					*sbuff2++ = ' ';
				}
				if (isConst)
				{
					sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"const ");
					isConst = false;
				}
				sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"char");
				while (ptCnt-- > 0)
				{
					*sbuff2++ = '*';
				}
				ptCnt = 0;
				hasParam = true;
			}
			else if (c == 'w')
			{
				if (hasParam)
				{
					*sbuff2++ = ',';
					*sbuff2++ = ' ';
				}
				if (isConst)
				{
					sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"const ");
					isConst = false;
				}
				sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"wchar_t");
				while (ptCnt-- > 0)
				{
					*sbuff2++ = '*';
				}
				ptCnt = 0;
				hasParam = true;
			}
			else if (c == 'h')
			{
				if (hasParam)
				{
					*sbuff2++ = ',';
					*sbuff2++ = ' ';
				}
				if (isConst)
				{
					sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"const ");
					isConst = false;
				}
				sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"unsigned char");
				while (ptCnt-- > 0)
				{
					*sbuff2++ = '*';
				}
				ptCnt = 0;
				hasParam = true;
			}
			else if (c == 't')
			{
				if (hasParam)
				{
					*sbuff2++ = ',';
					*sbuff2++ = ' ';
				}
				if (isConst)
				{
					sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"const ");
					isConst = false;
				}
				sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"unsigned short");
				while (ptCnt-- > 0)
				{
					*sbuff2++ = '*';
				}
				ptCnt = 0;
				hasParam = true;
			}
			else if (c == 'j')
			{
				if (hasParam)
				{
					*sbuff2++ = ',';
					*sbuff2++ = ' ';
				}
				if (isConst)
				{
					sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"const ");
					isConst = false;
				}
				sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"unsigned int");
				while (ptCnt-- > 0)
				{
					*sbuff2++ = '*';
				}
				ptCnt = 0;
				hasParam = true;
			}
			else if (c == 'x')
			{
				if (hasParam)
				{
					*sbuff2++ = ',';
					*sbuff2++ = ' ';
				}
				if (isConst)
				{
					sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"const ");
					isConst = false;
				}
				sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"__int64");
				while (ptCnt-- > 0)
				{
					*sbuff2++ = '*';
				}
				ptCnt = 0;
				hasParam = true;
			}
			else if (c == 'y')
			{
				if (hasParam)
				{
					*sbuff2++ = ',';
					*sbuff2++ = ' ';
				}
				if (isConst)
				{
					sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"const ");
					isConst = false;
				}
				sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"unsigned __int64");
				while (ptCnt-- > 0)
				{
					*sbuff2++ = '*';
				}
				ptCnt = 0;
				hasParam = true;
			}
			else if (c == 'd')
			{
				if (hasParam)
				{
					*sbuff2++ = ',';
					*sbuff2++ = ' ';
				}
				if (isConst)
				{
					sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"const ");
					isConst = false;
				}
				sbuff2 = Text::StrConcat(sbuff2, (const UTF8Char*)"double");
				while (ptCnt-- > 0)
				{
					*sbuff2++ = '*';
				}
				ptCnt = 0;
				hasParam = true;
			}
		}
		if (!valid || !foundName || !hasParam)
		{
			return Text::StrConcat(sbuff, funcName);
		}
		else
		{
			*sbuff2++ = ')';
			*sbuff2 = 0;
			return sbuff2;
		}
	}
	else
	{
		return Text::StrConcat(sbuff, funcName);
	}
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

void Parser::FileParser::ELFParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_EXE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.so", (const UTF8Char*)"Shared Object File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::ELFParser::GetParserType()
{
	return IO::ParsedObject::PT_EXE_PARSER;
}

IO::ParsedObject *Parser::FileParser::ELFParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdr[64];
	if (fd->GetRealData(0, 56, hdr) != 56)
	{
		return 0;
	}
	if (hdr[0] != 0x7f || hdr[1] != 'E' || hdr[2] != 'L' || hdr[3] != 'F')
	{
		return 0;
	}

	RInt64Func readInt64;
	RInt32Func readInt32;
	RInt16Func readInt16;
	UTF8Char sbuff[256];
	UTF8Char sbuff2[256];
	UTF8Char *sptr;
	IO::EXEFile *exef;
	if (hdr[4] != 1 && hdr[4] != 2)
	{
		return 0;
	}
	NEW_CLASS(exef, IO::EXEFile(fd->GetFullName()));
	if (hdr[4] == 1)
	{
		exef->AddProp((const UTF8Char*)"File Format", (const UTF8Char*)"ELF32");
	}
	else if (hdr[4] == 2)
	{
		exef->AddProp((const UTF8Char*)"File Format", (const UTF8Char*)"ELF64");
	}
	if (hdr[5] == 1)
	{
		exef->AddProp((const UTF8Char*)"Endianness", (const UTF8Char*)"1 (Little)");
		readInt64 = TReadInt64;
		readInt32 = TReadInt32;
		readInt16 = TReadInt16;
	}
	else if (hdr[5] == 2)
	{
		exef->AddProp((const UTF8Char*)"Endianness", (const UTF8Char*)"2 (Big)");
		readInt64 = TReadMInt64;
		readInt32 = TReadMInt32;
		readInt16 = TReadMInt16;
	}
	else
	{
		Text::StrUInt32(sbuff, hdr[5]);
		exef->AddProp((const UTF8Char*)"Endianness", sbuff);
		readInt64 = TReadInt64;
		readInt32 = TReadInt32;
		readInt16 = TReadInt16;
	}
	Text::StrUInt32(sbuff, hdr[6]);
	exef->AddProp((const UTF8Char*)"Format Version", sbuff);
	switch (hdr[7])
	{
	case 0:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x00 (System V)");
		break;
	case 1:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x01 (HP-UX)");
		break;
	case 2:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x02 (NetBSD)");
		break;
	case 3:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x03 (Linux)");
		break;
	case 4:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x04 (GNU Hurd)");
		break;
	case 6:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x06 (Solaris)");
		break;
	case 7:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x07 (AIX)");
		break;
	case 8:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x08 (IRIX)");
		break;
	case 9:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x09 (FreeBSD)");
		break;
	case 10:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x0A (Tru64)");
		break;
	case 11:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x0B (Novell Modesto)");
		break;
	case 12:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x0C (OpenBSD)");
		break;
	case 13:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x0D (OpenVMS)");
		break;
	case 14:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x0E (NonStop Kernel)");
		break;
	case 15:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x0F (AROS)");
		break;
	case 16:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x10 (Fenix OS)");
		break;
	case 17:
		exef->AddProp((const UTF8Char*)"OS ABI", (const UTF8Char*)"0x11 (CloudABI)");
		break;
	default:
		Text::StrHexByte(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), hdr[7]);
		exef->AddProp((const UTF8Char*)"OS ABI", sbuff);
		break;
	}
	switch (readInt16(&hdr[16]))
	{
	case 1:
		exef->AddProp((const UTF8Char*)"Type", (const UTF8Char*)"1 (Relocatable)");
		break;
	case 2:
		exef->AddProp((const UTF8Char*)"Type", (const UTF8Char*)"2 (Executable)");
		break;
	case 3:
		exef->AddProp((const UTF8Char*)"Type", (const UTF8Char*)"3 (Shared)");
		break;
	case 4:
		exef->AddProp((const UTF8Char*)"Type", (const UTF8Char*)"3 (Core)");
		break;
	default:
		Text::StrUInt32(sbuff, (UInt16)readInt16(&hdr[16]));
		exef->AddProp((const UTF8Char*)"Type", sbuff);
		break;
	}
	switch (readInt16(&hdr[18]))
	{
	case 0:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"0 (No machine)");
		break;
	case 1:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"1 (AT&T WE 32100)");
		break;
	case 2:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"2 (SPARC)");
		break;
	case 3:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"3 (x86)");
		break;
	case 4:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"4 (Motorola 68000)");
		break;
	case 5:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"5 (Motorola 88000)");
		break;
	case 7:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"7 (Intel 80860)");
		break;
	case 8:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"8 (MIPS)");
		break;
	case 0x14:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"0x14 (PowerPC)");
		break;
	case 0x16:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"0x16 (S390)");
		break;
	case 0x28:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"0x28 (ARM)");
		break;
	case 0x2A:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"0x2A (SuperH)");
		break;
	case 0x32:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"0x32 (IA-64)");
		break;
	case 0x3E:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"0x3E (x86-64)");
		break;
	case 0xB7:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"0xB7 (AArch64)");
		break;
	case 0xF3:
		exef->AddProp((const UTF8Char*)"Machine", (const UTF8Char*)"0xF3 (RISC-V)");
		break;
	default:
		Text::StrUInt32(sbuff, (UInt16)readInt16(&hdr[18]));
		exef->AddProp((const UTF8Char*)"Machine", sbuff);
		break;
	}
	Text::StrUInt32(sbuff, (UInt32)readInt32(&hdr[20]));
	exef->AddProp((const UTF8Char*)"Version", sbuff);
	UInt8 *progHdr = 0;
	UInt8 *secHdr = 0;
	UInt32 phSize;
	UInt32 phCnt;
	UInt32 shSize;
	UInt32 shCnt;
	UInt32 secNameInd;
	OSInt i;
	OSInt j;
	if (hdr[4] == 1)
	{
		UInt32 entryPt = readInt32(&hdr[24]);
		UInt32 pht = readInt32(&hdr[28]);
		UInt32 sht = readInt32(&hdr[32]);
		phSize = (UInt16)readInt16(&hdr[42]);
		phCnt = (UInt16)readInt16(&hdr[44]);
		shSize = (UInt16)readInt16(&hdr[46]);
		shCnt = (UInt16)readInt16(&hdr[48]);
		Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), entryPt);
		exef->AddProp((const UTF8Char*)"Entry Point", sbuff);
		Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), pht);
		exef->AddProp((const UTF8Char*)"Program Header Table", sbuff);
		Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), sht);
		exef->AddProp((const UTF8Char*)"Section Header Table", sbuff);
		Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), readInt32(&hdr[36]));
		exef->AddProp((const UTF8Char*)"Flags", sbuff);
		Text::StrUInt32(sbuff, (UInt16)readInt16(&hdr[40]));
		exef->AddProp((const UTF8Char*)"Header Size", sbuff);
		Text::StrUInt32(sbuff, phSize);
		exef->AddProp((const UTF8Char*)"Program Header Table Entry Size", sbuff);
		Text::StrUInt32(sbuff, phCnt);
		exef->AddProp((const UTF8Char*)"Program Header Table Entry Count", sbuff);
		Text::StrUInt32(sbuff, shSize);
		exef->AddProp((const UTF8Char*)"Section Header Table Entry Size", sbuff);
		Text::StrUInt32(sbuff, shCnt);
		exef->AddProp((const UTF8Char*)"Section Header Table Entry Count", sbuff);
		Text::StrUInt32(sbuff, secNameInd = (UInt16)readInt16(&hdr[50]));
		exef->AddProp((const UTF8Char*)"Section Name Index", sbuff);

		if (phSize >= 32)
		{
			progHdr = MemAlloc(UInt8, phSize * phCnt);
			fd->GetRealData(pht, phSize * phCnt, progHdr);

			i = 0;
			j = 0;
			while ((UOSInt)i < phCnt)
			{
				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Type");
				switch ((UInt32)readInt32(&progHdr[j]))
				{
				case 0:
					exef->AddProp(sbuff, (const UTF8Char*)"0 (Null)");
					break;
				case 1:
					exef->AddProp(sbuff, (const UTF8Char*)"1 (Load)");
					break;
				case 2:
					exef->AddProp(sbuff, (const UTF8Char*)"2 (Dynamic)");
					break;
				case 3:
					exef->AddProp(sbuff, (const UTF8Char*)"3 (Interp)");
					break;
				case 4:
					exef->AddProp(sbuff, (const UTF8Char*)"4 (Note)");
					break;
				case 5:
					exef->AddProp(sbuff, (const UTF8Char*)"5 (ShLib)");
					break;
				case 6:
					exef->AddProp(sbuff, (const UTF8Char*)"6 (PHdr)");
					break;
				default:
					Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&progHdr[j]));
					exef->AddProp(sbuff, sbuff2);
					break;
				}
				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Offset");
				Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&progHdr[j + 4]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Virtual Address");
				Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&progHdr[j + 8]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Physical Address");
				Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&progHdr[j + 12]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" File Size");
				Text::StrUInt32(sbuff2, readInt32(&progHdr[j + 16]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Memory Size");
				Text::StrUInt32(sbuff2, readInt32(&progHdr[j + 20]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Flags");
				Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&progHdr[j + 24]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Align");
				Text::StrUInt32(sbuff2, readInt32(&progHdr[j + 28]));
				exef->AddProp(sbuff, sbuff2);

				i++;
				j += phSize;
			}
			MemFree(progHdr);
		}

		if (shSize >= 40)
		{
			progHdr = 0;
			secHdr = MemAlloc(UInt8, shSize * shCnt);
			fd->GetRealData(sht, shSize * shCnt, secHdr);

			if (readInt32(&secHdr[shSize * secNameInd + 4]) == 3)
			{
				Int32 sz = readInt32(&secHdr[shSize * secNameInd + 20]);
				progHdr = MemAlloc(UInt8, sz + 1);
				fd->GetRealData(readInt32(&secHdr[shSize * secNameInd + 16]), sz, progHdr);
				progHdr[sz] = 0;
			}

			i = 0;
			j = 0;
			while ((UOSInt)i < shCnt)
			{
				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Name Offset");
				Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&secHdr[j + 0]));
				exef->AddProp(sbuff, sbuff2);

				if (progHdr)
				{
					sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
					sptr = Text::StrOSInt(sptr, i);
					sptr = Text::StrConcat(sptr, (const UTF8Char*)" Name Offset");
					exef->AddProp(sbuff, &progHdr[readInt32(&secHdr[j + 0])]);
				}

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Type");
				switch ((UInt32)readInt32(&secHdr[j + 4]))
				{
				case 0:
					exef->AddProp(sbuff, (const UTF8Char*)"0 (Null)");
					break;
				case 1:
					exef->AddProp(sbuff, (const UTF8Char*)"1 (Program Data)");
					break;
				case 2:
					exef->AddProp(sbuff, (const UTF8Char*)"2 (Symbol Table)");
					break;
				case 3:
					exef->AddProp(sbuff, (const UTF8Char*)"3 (String Table)");
					break;
				case 4:
					exef->AddProp(sbuff, (const UTF8Char*)"4 (Relocation Entries with addends)");
					break;
				case 5:
					exef->AddProp(sbuff, (const UTF8Char*)"5 (Symbol Hash Table)");
					break;
				case 6:
					exef->AddProp(sbuff, (const UTF8Char*)"6 (Dynamic Linking Info)");
					break;
				case 7:
					exef->AddProp(sbuff, (const UTF8Char*)"7 (Notes)");
					break;
				case 8:
					exef->AddProp(sbuff, (const UTF8Char*)"8 (Program space with no data)");
					break;
				case 9:
					exef->AddProp(sbuff, (const UTF8Char*)"9 (Relocation Entries)");
					break;
				case 10:
					exef->AddProp(sbuff, (const UTF8Char*)"10 (ShLib)");
					break;
				case 11:
					exef->AddProp(sbuff, (const UTF8Char*)"11 (Dynamic Linker Symbol Table)");
					break;
				case 14:
					exef->AddProp(sbuff, (const UTF8Char*)"14 (Array of Constructors)");
					break;
				case 15:
					exef->AddProp(sbuff, (const UTF8Char*)"15 (Array of Destructors)");
					break;
				case 16:
					exef->AddProp(sbuff, (const UTF8Char*)"16 (Array of Pre-constructors)");
					break;
				case 17:
					exef->AddProp(sbuff, (const UTF8Char*)"17 (Section Group)");
					break;
				case 18:
					exef->AddProp(sbuff, (const UTF8Char*)"18 (Extended Section Indices)");
					break;
				case 19:
					exef->AddProp(sbuff, (const UTF8Char*)"19 (Number of defined types)");
					break;
				default:
					Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&secHdr[j + 4]));
					exef->AddProp(sbuff, sbuff2);
					break;
				}

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Flags");
				Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&secHdr[j + 8]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Virtual Address");
				Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&secHdr[j + 12]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Offset");
				Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&secHdr[j + 16]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Size");
				Text::StrUInt32(sbuff2, readInt32(&secHdr[j + 20]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Associated Section");
				Text::StrUInt32(sbuff2, readInt32(&secHdr[j + 24]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Extra info");
				Text::StrUInt32(sbuff2, readInt32(&secHdr[j + 28]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Align");
				Text::StrUInt32(sbuff2, readInt32(&secHdr[j + 32]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Entry Size");
				Text::StrUInt32(sbuff2, readInt32(&secHdr[j + 36]));
				exef->AddProp(sbuff, sbuff2);

				if (progHdr)
				{
					const UTF8Char *name = (const UTF8Char*)&progHdr[readInt32(&secHdr[j + 0])];
					UInt32 assSec = readInt32(&secHdr[j + 24]);
					if (Text::StrEquals(name, (const UTF8Char*)".dynsym") || Text::StrEquals(name, (const UTF8Char*)".symtab"))
					{
						OSInt k;
						OSInt l;
						UInt32 tmpVal;
						UInt32 symSize = readInt32(&secHdr[j + 20]);
						UInt32 strSize = readInt32(&secHdr[assSec * shSize + 20]);
						UInt8 *symTab = MemAlloc(UInt8, symSize);
						UInt8 *strTab = MemAlloc(UInt8, strSize);
						fd->GetRealData(readInt32(&secHdr[j + 16]), symSize, symTab);
						fd->GetRealData(readInt32(&secHdr[assSec * shSize + 16]), strSize, strTab);
						k = 0;
						l = 0;
						while ((UOSInt)l < symSize)
						{
							if ((symTab[l +12] & 15) == 2)
							{
								tmpVal = readInt32(&symTab[l]);
								ToFuncName(sbuff2, &strTab[tmpVal]);
								exef->AddExportFunc(sbuff2);
							}
							else
							{
								Text::StrOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Symbol "), k);
								tmpVal = readInt32(&symTab[l]);
								if (tmpVal == 0)
								{
									sptr = Text::StrConcat(sbuff2, (const UTF8Char*)"(null)");
								}
								else
								{
									sptr = Text::StrConcat(sbuff2, &strTab[tmpVal]);
								}
								sptr = Text::StrConcat(sptr, (const UTF8Char*)", addr = 0x");
								sptr = Text::StrHexVal32(sptr, readInt32(&symTab[l + 4]));
								sptr = Text::StrConcat(sptr, (const UTF8Char*)", size = ");
								sptr = Text::StrInt32(sptr, readInt32(&symTab[l + 8]));
								sptr = Text::StrConcat(sptr, (const UTF8Char*)", bind = ");
								sptr = Text::StrInt32(sptr, symTab[l + 12] >> 4);
								switch (symTab[l + 12] >> 4)
								{
								case 0:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (Local)");
									break;
								case 1:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (Global)");
									break;
								case 2:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (Weak)");
									break;
								case 10:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (Lo OS)");
									break;
								case 12:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (Hi OS)");
									break;
								case 13:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (Lo Proc)");
									break;
								case 15:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (Hi Proc)");
									break;
								}
								sptr = Text::StrConcat(sptr, (const UTF8Char*)", type = ");
								sptr = Text::StrInt32(sptr, symTab[l + 12] & 15);
								switch (symTab[l + 12] & 15)
								{
								case 0:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (No Type)");
									break;
								case 1:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (Object)");
									break;
								case 2:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (Function)");
									break;
								case 3:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (Section)");
									break;
								case 4:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (File)");
									break;
								case 5:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (Common)");
									break;
								case 6:
									sptr = Text::StrConcat(sptr, (const UTF8Char*)" (TLS)");
									break;
								}
								exef->AddProp(sbuff, sbuff2);
							}

							k++;
							l += 16;
						}
						MemFree(symTab);
						MemFree(strTab);
					}
				}
				i++;
				j += shSize;
			}
			MemFree(secHdr);

			if (progHdr)
			{
				MemFree(progHdr);
			}
		}
	}
	else if (hdr[4] == 2)
	{
		UInt64 entryPt = readInt64(&hdr[24]);
		UInt64 pht = readInt64(&hdr[32]);
		UInt64 sht = readInt64(&hdr[40]);
		phSize = (UInt16)readInt16(&hdr[54]);
		phCnt = (UInt16)readInt16(&hdr[56]);
		shSize = (UInt16)readInt16(&hdr[58]);
		shCnt = (UInt16)readInt16(&hdr[60]);
		Text::StrHexVal64(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), entryPt);
		exef->AddProp((const UTF8Char*)"Entry Point", sbuff);
		Text::StrHexVal64(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), pht);
		exef->AddProp((const UTF8Char*)"Program Header Table", sbuff);
		Text::StrHexVal64(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), sht);
		exef->AddProp((const UTF8Char*)"Section Header Table", sbuff);
		Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), readInt32(&hdr[48]));
		exef->AddProp((const UTF8Char*)"Flags", sbuff);
		Text::StrUInt32(sbuff, (UInt16)readInt16(&hdr[52]));
		exef->AddProp((const UTF8Char*)"Header Size", sbuff);
		Text::StrUInt32(sbuff, phSize);
		exef->AddProp((const UTF8Char*)"Program Header Table Entry Size", sbuff);
		Text::StrUInt32(sbuff, phCnt);
		exef->AddProp((const UTF8Char*)"Program Header Table Entry Count", sbuff);
		Text::StrUInt32(sbuff, shSize);
		exef->AddProp((const UTF8Char*)"Section Header Table Entry Size", sbuff);
		Text::StrUInt32(sbuff, shCnt);
		exef->AddProp((const UTF8Char*)"Section Header Table Entry Count", sbuff);
		Text::StrUInt32(sbuff, secNameInd = (UInt16)readInt16(&hdr[62]));
		exef->AddProp((const UTF8Char*)"Section Name Index", sbuff);

		if (phSize >= 56)
		{
			progHdr = MemAlloc(UInt8, phSize * phCnt);
			fd->GetRealData(pht, phSize * phCnt, progHdr);

			i = 0;
			j = 0;
			while ((UOSInt)i < phCnt)
			{
				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Type");
				switch ((UInt32)readInt32(&progHdr[j]))
				{
				case 0:
					exef->AddProp(sbuff, (const UTF8Char*)"0 (Null)");
					break;
				case 1:
					exef->AddProp(sbuff, (const UTF8Char*)"1 (Load)");
					break;
				case 2:
					exef->AddProp(sbuff, (const UTF8Char*)"2 (Dynamic)");
					break;
				case 3:
					exef->AddProp(sbuff, (const UTF8Char*)"3 (Interp)");
					break;
				case 4:
					exef->AddProp(sbuff, (const UTF8Char*)"4 (Note)");
					break;
				case 5:
					exef->AddProp(sbuff, (const UTF8Char*)"5 (ShLib)");
					break;
				case 6:
					exef->AddProp(sbuff, (const UTF8Char*)"6 (PHdr)");
					break;
				default:
					Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&progHdr[j]));
					exef->AddProp(sbuff, sbuff2);
					break;
				}

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Flags");
				Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&progHdr[j + 4]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Offset");
				Text::StrHexVal64(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt64(&progHdr[j + 8]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Virtual Address");
				Text::StrHexVal64(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt64(&progHdr[j + 16]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Physical Address");
				Text::StrHexVal64(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt64(&progHdr[j + 24]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" File Size");
				Text::StrInt64(sbuff2, readInt64(&progHdr[j + 32]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Memory Size");
				Text::StrInt64(sbuff2, readInt32(&progHdr[j + 40]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Program Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Align");
				Text::StrInt64(sbuff2, readInt64(&progHdr[j + 48]));
				exef->AddProp(sbuff, sbuff2);

				i++;
				j += phSize;
			}
			MemFree(progHdr);
		}

		if (shSize >= 64)
		{
			secHdr = MemAlloc(UInt8, shSize * shCnt);
			fd->GetRealData(sht, shSize * shCnt, secHdr);

			i = 0;
			j = 0;
			while ((UOSInt)i < shCnt)
			{
				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Name Offset");
				Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&secHdr[j + 0]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Type");
				switch ((UInt32)readInt32(&secHdr[j + 4]))
				{
				case 0:
					exef->AddProp(sbuff, (const UTF8Char*)"0 (Null)");
					break;
				case 1:
					exef->AddProp(sbuff, (const UTF8Char*)"1 (Program Data)");
					break;
				case 2:
					exef->AddProp(sbuff, (const UTF8Char*)"2 (Symbol Table)");
					break;
				case 3:
					exef->AddProp(sbuff, (const UTF8Char*)"3 (String Table)");
					break;
				case 4:
					exef->AddProp(sbuff, (const UTF8Char*)"4 (Relocation Entries with addends)");
					break;
				case 5:
					exef->AddProp(sbuff, (const UTF8Char*)"5 (Symbol Hash Table)");
					break;
				case 6:
					exef->AddProp(sbuff, (const UTF8Char*)"6 (Dynamic Linking Info)");
					break;
				case 7:
					exef->AddProp(sbuff, (const UTF8Char*)"7 (Notes)");
					break;
				case 8:
					exef->AddProp(sbuff, (const UTF8Char*)"8 (Program space with no data)");
					break;
				case 9:
					exef->AddProp(sbuff, (const UTF8Char*)"9 (Relocation Entries)");
					break;
				case 10:
					exef->AddProp(sbuff, (const UTF8Char*)"10 (ShLib)");
					break;
				case 11:
					exef->AddProp(sbuff, (const UTF8Char*)"11 (Dynamic Linker Symbol Table)");
					break;
				case 14:
					exef->AddProp(sbuff, (const UTF8Char*)"14 (Array of Constructors)");
					break;
				case 15:
					exef->AddProp(sbuff, (const UTF8Char*)"15 (Array of Destructors)");
					break;
				case 16:
					exef->AddProp(sbuff, (const UTF8Char*)"16 (Array of Pre-constructors)");
					break;
				case 17:
					exef->AddProp(sbuff, (const UTF8Char*)"17 (Section Group)");
					break;
				case 18:
					exef->AddProp(sbuff, (const UTF8Char*)"18 (Extended Section Indices)");
					break;
				case 19:
					exef->AddProp(sbuff, (const UTF8Char*)"19 (Number of defined types)");
					break;
				default:
					Text::StrHexVal32(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt32(&secHdr[j + 4]));
					exef->AddProp(sbuff, sbuff2);
					break;
				}

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Flags");
				Text::StrHexVal64(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt64(&secHdr[j + 8]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Virtual Address");
				Text::StrHexVal64(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt64(&secHdr[j + 16]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Offset");
				Text::StrHexVal64(Text::StrConcat(sbuff2, (const UTF8Char*)"0x"), readInt64(&secHdr[j + 24]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Size");
				Text::StrInt64(sbuff2, readInt64(&secHdr[j + 32]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Associated Section");
				Text::StrUInt32(sbuff2, readInt32(&secHdr[j + 40]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Extra info");
				Text::StrUInt32(sbuff2, readInt32(&secHdr[j + 44]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Align");
				Text::StrInt64(sbuff2, readInt64(&secHdr[j + 48]));
				exef->AddProp(sbuff, sbuff2);

				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Section Header ");
				sptr = Text::StrOSInt(sptr, i);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" Entry Size");
				Text::StrInt64(sbuff2, readInt64(&secHdr[j + 56]));
				exef->AddProp(sbuff, sbuff2);

				i++;
				j += shSize;
			}

			MemFree(secHdr);
		}
	}

	return exef;
}
