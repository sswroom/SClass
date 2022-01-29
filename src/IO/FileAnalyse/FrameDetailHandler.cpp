#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Net/MACInfo.h"
#include "Net/NetBIOSUtil.h"
#include "Net/SocketUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

void IO::FileAnalyse::FrameDetailHandler::AddBool(UOSInt frameOfst, Text::CString name, UInt8 v)
{
	if (v == 0)
	{
		this->AddField(frameOfst, 1, name, CSTR("false"));
	}
	else if (v == 1)
	{
		this->AddField(frameOfst, 1, name, CSTR("true"));
	}
	else
	{
		UTF8Char sbuff[16];
		UTF8Char *sptr;
		sptr = Text::StrHexByte(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
		this->AddField(frameOfst, 1, name, {sbuff, (UOSInt)(sptr - sbuff)});
	}
}

void IO::FileAnalyse::FrameDetailHandler::AddInt64(UOSInt frameOfst, Text::CString name, Int64 v)
{
	UTF8Char sbuff[24];
	UTF8Char *sptr;
	sptr = Text::StrInt64(sbuff, v);
	this->AddField(frameOfst, 8, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddInt64V(UOSInt frameOfst, UOSInt size, Text::CString name, Int64 v)
{
	UTF8Char sbuff[24];
	UTF8Char *sptr;
	sptr = Text::StrInt64(sbuff, v);
	this->AddField(frameOfst, size, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddUInt64(UOSInt frameOfst, Text::CString name, UInt64 v)
{
	UTF8Char sbuff[24];
	UTF8Char *sptr;
	sptr = Text::StrUInt64(sbuff, v);
	this->AddField(frameOfst, 8, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddUInt64V(UOSInt frameOfst, UOSInt size, Text::CString name, UInt64 v)
{
	UTF8Char sbuff[24];
	UTF8Char *sptr;
	sptr = Text::StrUInt64(sbuff, v);
	this->AddField(frameOfst, size, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddInt(UOSInt frameOfst, UOSInt size, Text::CString name, OSInt v)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	sptr = Text::StrOSInt(sbuff, v);
	this->AddField(frameOfst, size, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddUInt(UOSInt frameOfst, UOSInt size, Text::CString name, UOSInt v)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	sptr = Text::StrUOSInt(sbuff, v);
	this->AddField(frameOfst, size, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddUIntName(UOSInt frameOfst, UOSInt size, Text::CString name, UOSInt v, Text::CString vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(v);
	if (vName.v)
	{
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(vName);
		sb.AppendChar(')', 1);
	}
	else
	{
		sb.AppendC(UTF8STRC(" (Unkonwn)"));
	}
	this->AddField(frameOfst, size, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddUInt64Name(UOSInt frameOfst, UOSInt size, Text::CString name, UInt64 v, Text::CString vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendU64(v);
	if (vName.v)
	{
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(vName);
		sb.AppendChar(')', 1);
	}
	else
	{
		sb.AppendC(UTF8STRC(" (Unkonwn)"));
	}
	this->AddField(frameOfst, size, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddFloat(UOSInt frameOfst, UOSInt size, Text::CString name, Double v)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sptr = Text::StrDouble(sbuff, v);
	this->AddField(frameOfst, size, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddHex8(UOSInt frameOfst, Text::CString name, UInt8 v)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	sptr = Text::StrHexByte(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 1, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddHex16(UOSInt frameOfst, Text::CString name, UInt16 v)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 2, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddHex24(UOSInt frameOfst, Text::CString name, UInt32 v)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	sptr = Text::StrHexVal24(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 3, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddHex32(UOSInt frameOfst, Text::CString name, UInt32 v)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 4, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddHex64(UOSInt frameOfst, Text::CString name, UInt64 v)
{
	UTF8Char sbuff[19];
	UTF8Char *sptr;
	sptr = Text::StrHexVal64(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 8, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddHex64V(UOSInt frameOfst, UOSInt size, Text::CString name, UInt64 v)
{
	UTF8Char sbuff[19];
	UTF8Char *sptr;
	sptr = Text::StrHexVal64V(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, size, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddHex8Name(UOSInt frameOfst, Text::CString name, UInt8 v, Text::CString vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("0x"));
	sb.AppendHex8(v);
	if (vName.v)
	{
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(vName);
		sb.AppendChar(')', 1);
	}
	else
	{
		sb.AppendC(UTF8STRC(" (Unkonwn)"));
	}
	this->AddField(frameOfst, 1, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddHex16Name(UOSInt frameOfst, Text::CString name, UInt16 v, Text::CString vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("0x"));
	sb.AppendHex16(v);
	if (vName.v)
	{
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(vName);
		sb.AppendChar(')', 1);
	}
	else
	{
		sb.AppendC(UTF8STRC(" (Unkonwn)"));
	}
	this->AddField(frameOfst, 2, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddStrC(UOSInt frameOfst, UOSInt size, Text::CString name, const UTF8Char *vBuff)
{
	Text::String *s = Text::String::New(vBuff, size);
	this->AddField(frameOfst, size, name, s->ToCString());
	s->Release();
}

void IO::FileAnalyse::FrameDetailHandler::AddStrS(UOSInt frameOfst, UOSInt size, Text::CString name, const UTF8Char *vBuff)
{
	UTF8Char *sbuff = MemAlloc(UTF8Char, size + 1);
	UTF8Char *sptr;
	sptr = Text::StrConcatS(sbuff, vBuff, size + 1);
	this->AddField(frameOfst, size, name, {sbuff, (UOSInt)(sptr - sbuff)});
	MemFree(sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddHexBuff(UOSInt frameOfst, UOSInt size, Text::CString name, const UInt8 *vBuff, Bool multiLine)
{
	this->AddHexBuff(frameOfst, size, name, vBuff, ' ', multiLine);
}

void IO::FileAnalyse::FrameDetailHandler::AddHexBuff(UOSInt frameOfst, UOSInt size, Text::CString name, const UTF8Char *vBuff, UTF8Char seperator, Bool multiLine)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(vBuff, size, seperator, multiLine?Text::LineBreakType::CRLF:Text::LineBreakType::None);
	this->AddField(frameOfst, size, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddIPv4(UOSInt frameOfst, Text::CString name, const UInt8 *vBuff)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(vBuff));
	this->AddField(frameOfst, 4, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddIPv6(UOSInt frameOfst, Text::CString name, const UInt8 *vBuff)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfoV6(&addr, vBuff, 0);
	sptr = Net::SocketUtil::GetAddrName(sbuff, &addr);
	this->AddField(frameOfst, 16, name, {sbuff, (UOSInt)(sptr - sbuff)});
}

void IO::FileAnalyse::FrameDetailHandler::AddMACAddr(UOSInt frameOfst, Text::CString name, const UInt8 *macBuff, Bool showVendor)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(macBuff, 6, ':', Text::LineBreakType::None);
	if (showVendor)
	{
		const Net::MACInfo::MACEntry *entry;
		sb.AppendC(UTF8STRC(" ("));
		entry = Net::MACInfo::GetMACInfoBuff(macBuff);
		if (entry && entry->nameLen > 0)
		{
			sb.AppendC(entry->name, entry->nameLen);
		}
		else
		{
			sb.AppendC(UTF8STRC("Unknown"));
		}
		sb.AppendChar(')', 1);
	}
	this->AddField(frameOfst, 6, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddNetBIOSName(UOSInt frameOfst, UOSInt size, Text::CString name, const UTF8Char *nbName)
{
	UTF8Char sbuff2[17];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	sb.AppendSlow(nbName);
	if ((sptr = Net::NetBIOSUtil::GetName(sbuff2, nbName)) != 0)
	{
		sptr[-1] = 0;
		sb.AppendC(UTF8STRC(" ("));
		sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2 - 1));
		sb.RTrim();
		sb.AppendC(UTF8STRC(")"));
	}
	this->AddField(frameOfst, size, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddTextHexBuff(UOSInt frameOfst, UOSInt size, const UInt8 *vBuff, Bool multiLine)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(vBuff, size, ' ', multiLine?Text::LineBreakType::CRLF:Text::LineBreakType::None);
	this->AddText(frameOfst, sb.ToCString());
}
