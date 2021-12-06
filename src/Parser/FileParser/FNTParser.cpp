#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Media/MSFontRenderer.h"
#include "Parser/FileParser/FNTParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

//https://support.microsoft.com/en-us/help/65123/windows-developers-notes-font-file-format

Parser::FileParser::FNTParser::FNTParser()
{
}

Parser::FileParser::FNTParser::~FNTParser()
{
}

Int32 Parser::FileParser::FNTParser::GetName()
{
	return *(Int32*)"FNTP";
}

void Parser::FileParser::FNTParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::FontRenderer)
	{
		selector->AddFilter((const UTF8Char*)"*.fnt", (const UTF8Char*)"Font File");
	}
}

IO::ParserType Parser::FileParser::FNTParser::GetParserType()
{
	return IO::ParserType::FontRenderer;
}

IO::ParsedObject *Parser::FileParser::FNTParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdr[118];
	OSInt hdrSize;
	UInt32 fsize;
	Int32 ver;
	if (fd->GetRealData(0, 118, hdr) != 118)
	{
		return 0;
	}
	ver = ReadUInt16(&hdr[0]);
	fsize = ReadUInt32(&hdr[2]);
	if (ver == 0x200)
	{
		hdrSize = 118;
	}
	else if (ver == 0x300)
	{
		hdrSize = 148;
	}
	else
	{
		return 0;
	}
	Media::FontRenderer *font = 0;
	if ((OSInt)fsize > hdrSize && fsize <= fd->GetDataSize())
	{
		UInt8 *fontBuff = MemAlloc(UInt8, fsize);
		if (fd->GetRealData(0, fsize, fontBuff) == fsize)
		{
			font = ParseFontBuff(fd->GetFullName(), fontBuff, fsize);
		}
		MemFree(fontBuff);
	}
	return font;
}

Media::FontRenderer *Parser::FileParser::FNTParser::ParseFontBuff(Text::String *sourceName, const UInt8 *fontBuff, UOSInt buffSize)
{
	UInt32 ver;
	if (buffSize < 118)
		return 0;

	ver = ReadUInt16(&fontBuff[0]);
//	fsize = ReadUInt32(&fontBuff[2]);
	if (ver == 0x200)
	{
//		hdrSize = 118;
	}
	else if (ver == 0x300)
	{
//		hdrSize = 148;
	}
	else
	{
		return 0;
	}

	Media::MSFontRenderer *font;
	NEW_CLASS(font, Media::MSFontRenderer(sourceName, fontBuff, buffSize));
	if (font->IsError())
	{
		DEL_CLASS(font);
		return 0;
	}
	return font;
}

Media::FontRenderer *Parser::FileParser::FNTParser::ParseFontBuff(const UTF8Char *sourceName, const UInt8 *fontBuff, UOSInt buffSize)
{
	UInt32 ver;
	if (buffSize < 118)
		return 0;

	ver = ReadUInt16(&fontBuff[0]);
//	fsize = ReadUInt32(&fontBuff[2]);
	if (ver == 0x200)
	{
//		hdrSize = 118;
	}
	else if (ver == 0x300)
	{
//		hdrSize = 148;
	}
	else
	{
		return 0;
	}

	Media::MSFontRenderer *font;
	NEW_CLASS(font, Media::MSFontRenderer(sourceName, fontBuff, buffSize));
	if (font->IsError())
	{
		DEL_CLASS(font);
		return 0;
	}
	return font;
}

UOSInt Parser::FileParser::FNTParser::GetFileDesc(const UInt8 *fileBuff, UOSInt fileSize, Text::StringBuilderUTF *sb)
{
	if (fileSize < 100)
		return 0;
	UInt16 ver = ReadUInt16(&fileBuff[0]);
	UInt32 fsize = ReadUInt32(&fileBuff[2]);
	UOSInt hdrSize;
	if (ver == 0x200)
	{
		hdrSize = 118;
	}
	else if (ver == 0x300)
	{
		hdrSize = 148;
	}
	else
	{
		return 0;
	}
	if (fsize < 114)
	{
		return 0;
	}
	sb->Append((const UTF8Char*)"Version = 0x");
	sb->AppendHex16(ver);
	sb->Append((const UTF8Char*)"\r\nSize = ");
	sb->AppendU32(fsize);
	sb->Append((const UTF8Char*)"\r\nCopyright = ");
	sb->Append((UTF8Char*)&fileBuff[6]);
	sb->Append((const UTF8Char*)"\r\nType = ");
	sb->AppendU16(ReadUInt16(&fileBuff[66]));
	sb->Append((const UTF8Char*)"\r\nPoint Size = ");
	sb->AppendU16(ReadUInt16(&fileBuff[68]));
	sb->Append((const UTF8Char*)"\r\nVertical Resolution = ");
	sb->AppendU16(ReadUInt16(&fileBuff[70]));
	sb->Append((const UTF8Char*)"\r\nHorizontal Resolution = ");
	sb->AppendU16(ReadUInt16(&fileBuff[72]));
	sb->Append((const UTF8Char*)"\r\nAscent = ");
	sb->AppendU16(ReadUInt16(&fileBuff[74]));
	sb->Append((const UTF8Char*)"\r\nInternal Leading = ");
	sb->AppendU16(ReadUInt16(&fileBuff[76]));
	sb->Append((const UTF8Char*)"\r\nExternal Leading = ");
	sb->AppendU16(ReadUInt16(&fileBuff[78]));
	sb->Append((const UTF8Char*)"\r\nItalic = ");
	sb->AppendU16(fileBuff[80]);
	sb->Append((const UTF8Char*)"\r\nUnderline = ");
	sb->AppendU16(fileBuff[81]);
	sb->Append((const UTF8Char*)"\r\nStrikeOut = ");
	sb->AppendU16(fileBuff[82]);
	sb->Append((const UTF8Char*)"\r\nWeight = ");
	sb->AppendU16(ReadUInt16(&fileBuff[83]));
	sb->Append((const UTF8Char*)"\r\nCharset = ");
	sb->AppendU16(fileBuff[85]);
	sb->Append((const UTF8Char*)"\r\nPixel Width = ");
	sb->AppendU16(ReadUInt16(&fileBuff[86]));
	sb->Append((const UTF8Char*)"\r\nPixel Height = ");
	sb->AppendU16(ReadUInt16(&fileBuff[88]));
	sb->Append((const UTF8Char*)"\r\nPitch And Family = ");
	sb->AppendU16(fileBuff[90]);
	sb->Append((const UTF8Char*)"\r\nAverage Width = ");
	sb->AppendU16(ReadUInt16(&fileBuff[91]));
	sb->Append((const UTF8Char*)"\r\nMax Width = ");
	sb->AppendU16(ReadUInt16(&fileBuff[93]));
	sb->Append((const UTF8Char*)"\r\nFirst Char = 0x");
	sb->AppendHex8(fileBuff[95]);
	sb->Append((const UTF8Char*)"\r\nLast Char = 0x");
	sb->AppendHex8(fileBuff[96]);
	sb->Append((const UTF8Char*)"\r\nDefault Char = 0x");
	sb->AppendHex8(fileBuff[97]);
	sb->Append((const UTF8Char*)"\r\nBreak Char = 0x");
	sb->AppendHex8(fileBuff[98]);
	sb->Append((const UTF8Char*)"\r\nWidth Bytes = ");
	sb->AppendU16(ReadUInt16(&fileBuff[99]));
	sb->Append((const UTF8Char*)"\r\nDevice = 0x");
	sb->AppendHex32(ReadUInt32(&fileBuff[101]));
	sb->Append((const UTF8Char*)"\r\nFace Offset = 0x");
	sb->AppendHex32(ReadUInt32(&fileBuff[105]));
	UInt32 ofst = ReadUInt32(&fileBuff[105]);
	if (ofst != 0 && ofst < (UOSInt)fileSize)
	{
		sb->Append((const UTF8Char*)"\r\nFace Name = ");
		sb->Append((UTF8Char*)&fileBuff[ofst]);
	}
	sb->Append((const UTF8Char*)"\r\nBits Pointer = 0x");
	sb->AppendHex32(ReadUInt32(&fileBuff[109]));
	if (fileSize >= 117)
	{
		sb->Append((const UTF8Char*)"\r\nBits Offset = 0x");
		sb->AppendHex32(ReadUInt32(&fileBuff[113]));
	}
	UOSInt i;
	UInt32 c;
	if (ver == 0x200)
	{
		i = 118;
		c = fileBuff[95];
		while (c <= fileBuff[96] && i < fileSize)
		{
			sb->Append((const UTF8Char*)"\r\nChar[0x");
			sb->AppendHex8((UInt8)c);
			sb->Append((const UTF8Char*)"] : Size = (");
			sb->AppendU16(ReadUInt16(&fileBuff[i]));
			sb->Append((const UTF8Char*)", ");
			sb->AppendU16(ReadUInt16(&fileBuff[88]));
			sb->Append((const UTF8Char*)"), Offset = 0x");
			sb->AppendHex16(ReadUInt16(&fileBuff[i + 2]));
			i += 4;
			c++;
		}
	}
	else if (ver == 0x300)
	{
		sb->Append((const UTF8Char*)"\r\nFlags = 0x");
		sb->AppendHex32(ReadUInt32(&fileBuff[118]));
		sb->Append((const UTF8Char*)"\r\nA Space = ");
		sb->AppendU16(ReadUInt16(&fileBuff[122]));
		sb->Append((const UTF8Char*)"\r\nB Space = ");
		sb->AppendU16(ReadUInt16(&fileBuff[124]));
		sb->Append((const UTF8Char*)"\r\nC Space = ");
		sb->AppendU16(ReadUInt16(&fileBuff[126]));
		sb->Append((const UTF8Char*)"\r\nColor Pointer = 0x");
		sb->AppendHex32V(ReadUInt32(&fileBuff[128]));
		i = 148;
		c = fileBuff[95];
		while (c <= fileBuff[96] && i < fileSize)
		{
			sb->Append((const UTF8Char*)"\r\nChar[0x");
			sb->AppendHex8((UInt8)c);
			sb->Append((const UTF8Char*)"] : Size = (");
			sb->AppendU16(ReadUInt16(&fileBuff[i]));
			sb->Append((const UTF8Char*)", ");
			sb->AppendU16(ReadUInt16(&fileBuff[88]));
			sb->Append((const UTF8Char*)"), Offset = 0x");
			sb->AppendHex32(ReadUInt32(&fileBuff[i + 2]));
			i += 6;
			c++;
		}
	}
	return hdrSize;
}

void Parser::FileParser::FNTParser::GetFileDirDesc(const UInt8 *fileBuff, UOSInt fileSize, Text::StringBuilderUTF *sb)
{
	UOSInt nFonts = ReadUInt16(&fileBuff[0]);
	UOSInt i = 2;
	sb->Append((const UTF8Char*)"Number of Fonts = ");
	sb->AppendUOSInt(nFonts);
	while (i < fileSize && nFonts-- > 0)
	{
		sb->Append((const UTF8Char*)"\r\nUnique Ordinal Identifier = ");
		sb->AppendU16(ReadUInt16(&fileBuff[i]));
		i += 2;
		if (i + 114 > fileSize)
		{
			break;
		}
		sb->Append((const UTF8Char*)"\r\n");
		GetFileDesc(&fileBuff[i], 114, sb);
		i += 114;
	}
}
