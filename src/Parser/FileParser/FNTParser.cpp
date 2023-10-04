#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
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

void Parser::FileParser::FNTParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::FontRenderer)
	{
		selector->AddFilter(CSTR("*.fnt"), CSTR("Font File"));
	}
}

IO::ParserType Parser::FileParser::FNTParser::GetParserType()
{
	return IO::ParserType::FontRenderer;
}

IO::ParsedObject *Parser::FileParser::FNTParser::ParseFile(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdr[118];
	OSInt hdrSize;
	UInt32 fsize;
	Int32 ver;
	if (fd->GetRealData(0, 118, BYTEARR(hdr)) != 118)
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
		Data::ByteBuffer fontBuff(fsize);
		if (fd->GetRealData(0, fsize, fontBuff) == fsize)
		{
			font = ParseFontBuff(fd->GetFullName(), fontBuff.Ptr(), fsize);
		}
	}
	return font;
}

Media::FontRenderer *Parser::FileParser::FNTParser::ParseFontBuff(NotNullPtr<Text::String> sourceName, const UInt8 *fontBuff, UOSInt buffSize)
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

UOSInt Parser::FileParser::FNTParser::GetFileDesc(const UInt8 *fileBuff, UOSInt fileSize, NotNullPtr<Text::StringBuilderUTF8> sb)
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
	sb->AppendC(UTF8STRC("Version = 0x"));
	sb->AppendHex16(ver);
	sb->AppendC(UTF8STRC("\r\nSize = "));
	sb->AppendU32(fsize);
	sb->AppendC(UTF8STRC("\r\nCopyright = "));
	sb->AppendSlow((UTF8Char*)&fileBuff[6]);
	sb->AppendC(UTF8STRC("\r\nType = "));
	sb->AppendU16(ReadUInt16(&fileBuff[66]));
	sb->AppendC(UTF8STRC("\r\nPoint Size = "));
	sb->AppendU16(ReadUInt16(&fileBuff[68]));
	sb->AppendC(UTF8STRC("\r\nVertical Resolution = "));
	sb->AppendU16(ReadUInt16(&fileBuff[70]));
	sb->AppendC(UTF8STRC("\r\nHorizontal Resolution = "));
	sb->AppendU16(ReadUInt16(&fileBuff[72]));
	sb->AppendC(UTF8STRC("\r\nAscent = "));
	sb->AppendU16(ReadUInt16(&fileBuff[74]));
	sb->AppendC(UTF8STRC("\r\nInternal Leading = "));
	sb->AppendU16(ReadUInt16(&fileBuff[76]));
	sb->AppendC(UTF8STRC("\r\nExternal Leading = "));
	sb->AppendU16(ReadUInt16(&fileBuff[78]));
	sb->AppendC(UTF8STRC("\r\nItalic = "));
	sb->AppendU16(fileBuff[80]);
	sb->AppendC(UTF8STRC("\r\nUnderline = "));
	sb->AppendU16(fileBuff[81]);
	sb->AppendC(UTF8STRC("\r\nStrikeOut = "));
	sb->AppendU16(fileBuff[82]);
	sb->AppendC(UTF8STRC("\r\nWeight = "));
	sb->AppendU16(ReadUInt16(&fileBuff[83]));
	sb->AppendC(UTF8STRC("\r\nCharset = "));
	sb->AppendU16(fileBuff[85]);
	sb->AppendC(UTF8STRC("\r\nPixel Width = "));
	sb->AppendU16(ReadUInt16(&fileBuff[86]));
	sb->AppendC(UTF8STRC("\r\nPixel Height = "));
	sb->AppendU16(ReadUInt16(&fileBuff[88]));
	sb->AppendC(UTF8STRC("\r\nPitch And Family = "));
	sb->AppendU16(fileBuff[90]);
	sb->AppendC(UTF8STRC("\r\nAverage Width = "));
	sb->AppendU16(ReadUInt16(&fileBuff[91]));
	sb->AppendC(UTF8STRC("\r\nMax Width = "));
	sb->AppendU16(ReadUInt16(&fileBuff[93]));
	sb->AppendC(UTF8STRC("\r\nFirst Char = 0x"));
	sb->AppendHex8(fileBuff[95]);
	sb->AppendC(UTF8STRC("\r\nLast Char = 0x"));
	sb->AppendHex8(fileBuff[96]);
	sb->AppendC(UTF8STRC("\r\nDefault Char = 0x"));
	sb->AppendHex8(fileBuff[97]);
	sb->AppendC(UTF8STRC("\r\nBreak Char = 0x"));
	sb->AppendHex8(fileBuff[98]);
	sb->AppendC(UTF8STRC("\r\nWidth Bytes = "));
	sb->AppendU16(ReadUInt16(&fileBuff[99]));
	sb->AppendC(UTF8STRC("\r\nDevice = 0x"));
	sb->AppendHex32(ReadUInt32(&fileBuff[101]));
	sb->AppendC(UTF8STRC("\r\nFace Offset = 0x"));
	sb->AppendHex32(ReadUInt32(&fileBuff[105]));
	UInt32 ofst = ReadUInt32(&fileBuff[105]);
	if (ofst != 0 && ofst < (UOSInt)fileSize)
	{
		sb->AppendC(UTF8STRC("\r\nFace Name = "));
		sb->AppendSlow((UTF8Char*)&fileBuff[ofst]);
	}
	sb->AppendC(UTF8STRC("\r\nBits Pointer = 0x"));
	sb->AppendHex32(ReadUInt32(&fileBuff[109]));
	if (fileSize >= 117)
	{
		sb->AppendC(UTF8STRC("\r\nBits Offset = 0x"));
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
			sb->AppendC(UTF8STRC("\r\nChar[0x"));
			sb->AppendHex8((UInt8)c);
			sb->AppendC(UTF8STRC("] : Size = ("));
			sb->AppendU16(ReadUInt16(&fileBuff[i]));
			sb->AppendC(UTF8STRC(", "));
			sb->AppendU16(ReadUInt16(&fileBuff[88]));
			sb->AppendC(UTF8STRC("), Offset = 0x"));
			sb->AppendHex16(ReadUInt16(&fileBuff[i + 2]));
			i += 4;
			c++;
		}
	}
	else if (ver == 0x300)
	{
		sb->AppendC(UTF8STRC("\r\nFlags = 0x"));
		sb->AppendHex32(ReadUInt32(&fileBuff[118]));
		sb->AppendC(UTF8STRC("\r\nA Space = "));
		sb->AppendU16(ReadUInt16(&fileBuff[122]));
		sb->AppendC(UTF8STRC("\r\nB Space = "));
		sb->AppendU16(ReadUInt16(&fileBuff[124]));
		sb->AppendC(UTF8STRC("\r\nC Space = "));
		sb->AppendU16(ReadUInt16(&fileBuff[126]));
		sb->AppendC(UTF8STRC("\r\nColor Pointer = 0x"));
		sb->AppendHex32V(ReadUInt32(&fileBuff[128]));
		i = 148;
		c = fileBuff[95];
		while (c <= fileBuff[96] && i < fileSize)
		{
			sb->AppendC(UTF8STRC("\r\nChar[0x"));
			sb->AppendHex8((UInt8)c);
			sb->AppendC(UTF8STRC("] : Size = ("));
			sb->AppendU16(ReadUInt16(&fileBuff[i]));
			sb->AppendC(UTF8STRC(", "));
			sb->AppendU16(ReadUInt16(&fileBuff[88]));
			sb->AppendC(UTF8STRC("), Offset = 0x"));
			sb->AppendHex32(ReadUInt32(&fileBuff[i + 2]));
			i += 6;
			c++;
		}
	}
	return hdrSize;
}

void Parser::FileParser::FNTParser::GetFileDirDesc(const UInt8 *fileBuff, UOSInt fileSize, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	UOSInt nFonts = ReadUInt16(&fileBuff[0]);
	UOSInt i = 2;
	sb->AppendC(UTF8STRC("Number of Fonts = "));
	sb->AppendUOSInt(nFonts);
	while (i < fileSize && nFonts-- > 0)
	{
		sb->AppendC(UTF8STRC("\r\nUnique Ordinal Identifier = "));
		sb->AppendU16(ReadUInt16(&fileBuff[i]));
		i += 2;
		if (i + 114 > fileSize)
		{
			break;
		}
		sb->AppendC(UTF8STRC("\r\n"));
		GetFileDesc(&fileBuff[i], 114, sb);
		i += 114;
	}
}
